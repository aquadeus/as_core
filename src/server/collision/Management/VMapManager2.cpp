////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "VMapManager2.h"
#include "MapTree.h"
#include "ModelInstance.h"
#include "WorldModel.h"
#include <G3D/Vector3.h>
#include "Log.h"
#include "VMapDefinitions.h"
#include "Errors.h"
#include "Common.h"
#include "World.h"

using G3D::Vector3;

namespace VMAP
{
    VMapManager2::VMapManager2()
    {
        GetLiquidFlagsPtr = &GetLiquidFlagsDummy;
        IsVMAPDisabledForPtr = &IsVMAPDisabledForDummy;
        thread_safe_environment = true;
    }

    VMapManager2::~VMapManager2()
    {
        for (auto i = iInstanceMapTrees.begin(); i != iInstanceMapTrees.end(); ++i)
            delete i->second;

        for (auto i = iLoadedModelFiles.begin(); i != iLoadedModelFiles.end(); ++i)
            delete i->second.getModel();
    }

    InstanceTreeMap::const_iterator VMapManager2::GetMapTree(uint32 mapId) const
    {
        // return the iterator if found or end() if not found/NULL
        auto itr = iInstanceMapTrees.find(mapId);
        if (itr != iInstanceMapTrees.cend() && !itr->second)
            itr = iInstanceMapTrees.cend();
        return itr;
    }

    void VMapManager2::InitializeThreadUnsafe(std::unordered_map<uint32, std::vector<uint32>> const& mapData)
    {
        // the caller must pass the list of all mapIds that will be used in the VMapManager2 lifetime
        iChildMapData = mapData;
        for (std::pair<uint32 const, std::vector<uint32>> const& mapId : mapData)
        {
            iInstanceMapTrees.insert(InstanceTreeMap::value_type(mapId.first, nullptr));
            for (uint32 childMapId : mapId.second)
                iParentMapData[childMapId] = mapId.first;
        }

        thread_safe_environment = false;
    }

    Vector3 VMapManager2::convertPositionToInternalRep(float x, float y, float z) const
    {
        Vector3 pos;
        const float mid = 0.5f * 64.0f * 533.33333333f;
        pos.x = mid - x;
        pos.y = mid - y;
        pos.z = z;

        return pos;
    }

    // move to MapTree too?
    std::string VMapManager2::getMapFileName(unsigned int mapId)
    {
        std::stringstream fname;
        fname.width(4);
        fname << std::setfill('0') << mapId << std::string(MAP_FILENAME_EXTENSION2);

        return fname.str();
    }

    int VMapManager2::loadMap(const char* basePath, unsigned int mapId, int x, int y)
    {
        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.acquire_write();

        int result = VMAP_LOAD_RESULT_IGNORED;
        if (isMapLoadingEnabled())
        {
            if (loadSingleMap(mapId, basePath, x, y))
            {
                result = VMAP_LOAD_RESULT_OK;
                auto childMaps = iChildMapData.find(mapId);
                if (childMaps != iChildMapData.end())
                    for (uint32 childMapId : childMaps->second)
                        if (!loadSingleMap(childMapId, basePath, x, y))
                            result = VMAP_LOAD_RESULT_ERROR;
            }
            else
                result = VMAP_LOAD_RESULT_ERROR;
        }

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.release();

        return result;
    }

    // load one tile (internal use only)
    bool VMapManager2::loadSingleMap(uint32 mapId, const std::string& basePath, uint32 tileX, uint32 tileY)
    {
        auto instanceTree = iInstanceMapTrees.find(mapId);
        if (instanceTree == iInstanceMapTrees.end())
        {
            if (thread_safe_environment)
                instanceTree = iInstanceMapTrees.insert(InstanceTreeMap::value_type(mapId, nullptr)).first;
        }

        if (!instanceTree->second)
        {
            std::string mapFileName = getMapFileName(mapId);
            StaticMapTree* newTree = new StaticMapTree(mapId, basePath);
            if (!newTree->InitMap(mapFileName, this))
            {
                delete newTree;
                return false;
            }
            instanceTree->second = newTree;
        }

        return instanceTree->second->LoadMapTile(tileX, tileY, this);
    }

    void VMapManager2::unloadMap(unsigned int mapId, int x, int y)
    {
        auto childMaps = iChildMapData.find(mapId);
        if (childMaps != iChildMapData.end())
            for (uint32 childMapId : childMaps->second)
                unloadSingleMap(childMapId, x, y);

        unloadSingleMap(mapId, x, y);
    }

    void VMapManager2::unloadSingleMap(uint32 mapId, int x, int y)
    {
        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.acquire_write();

        auto instanceTree = iInstanceMapTrees.find(mapId);
        if (instanceTree != iInstanceMapTrees.end() && instanceTree->second)
        {
            instanceTree->second->UnloadMapTile(x, y, this);
            if (instanceTree->second->numLoadedTiles() == 0)
            {
                delete instanceTree->second;
                instanceTree->second = nullptr;
            }
        }

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.release();
    }


    void VMapManager2::unloadMap(unsigned int mapId)
    {
        auto childMaps = iChildMapData.find(mapId);
        if (childMaps != iChildMapData.end())
            for (uint32 childMapId : childMaps->second)
                unloadSingleMap(childMapId);

        unloadSingleMap(mapId);
    }

    void VMapManager2::unloadSingleMap(uint32 mapId)
    {
        auto instanceTree = iInstanceMapTrees.find(mapId);
        if (instanceTree != iInstanceMapTrees.end() && instanceTree->second)
        {
            instanceTree->second->UnloadMap(this);
            if (instanceTree->second->numLoadedTiles() == 0)
            {
                delete instanceTree->second;
                instanceTree->second = nullptr;
            }
        }
    }

    bool VMapManager2::isInLineOfSight(unsigned int mapId, float x1, float y1, float z1, float x2, float y2, float z2)
    {
        /// Optimization, vmaps are always enable
        //if (!isLineOfSightCalcEnabled() || IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_LOS))
        //    return true;

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.acquire_read();

        auto instanceTree = GetMapTree(mapId);
        if (instanceTree != iInstanceMapTrees.end())
        {
            Vector3 pos1 = convertPositionToInternalRep(x1, y1, z1);
            Vector3 pos2 = convertPositionToInternalRep(x2, y2, z2);
            if (pos1 != pos2)
            {
                bool result =  instanceTree->second->isInLineOfSight(pos1, pos2);
                if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
                    m_LoadMapLock.release();
                return result;
            }
        }

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.release();

        return true;
    }

    /**
    get the hit position and return true if we hit something
    otherwise the result pos will be the dest pos
    */
    bool VMapManager2::getObjectHitPos(unsigned int mapId, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float &ry, float& rz, float modifyDist)
    {
        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.acquire_read();

        /// Optimization, vmaps are always enable
        //if (isLineOfSightCalcEnabled() && !IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_LOS))
        {
            auto instanceTree = GetMapTree(mapId);
            if (instanceTree != iInstanceMapTrees.end())
            {
                Vector3 pos1 = convertPositionToInternalRep(x1, y1, z1);
                Vector3 pos2 = convertPositionToInternalRep(x2, y2, z2);
                Vector3 resultPos;
                bool result = instanceTree->second->getObjectHitPos(pos1, pos2, resultPos, modifyDist);
                if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
                    m_LoadMapLock.release();
                resultPos = convertPositionToInternalRep(resultPos.x, resultPos.y, resultPos.z);
                rx = resultPos.x;
                ry = resultPos.y;
                rz = resultPos.z;
                return result;
            }
        }

        rx = x2;
        ry = y2;
        rz = z2;

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.release();

        return false;
    }

    /**
    get height or INVALID_HEIGHT if no height available
    */

    float VMapManager2::getHeight(unsigned int mapId, float x, float y, float z, float maxSearchDist, bool fromUnder /*=false*/)
    {
        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.acquire_read();

        /// Optimization, vmaps are always enable
        //if (isHeightCalcEnabled() && !IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_HEIGHT))
        {
            InstanceTreeMap::const_iterator instanceTree = GetMapTree(mapId);
            if (instanceTree != iInstanceMapTrees.end())
            {
                Vector3 pos = convertPositionToInternalRep(x, y, fromUnder ? z - 4.0f : z);
                float height = instanceTree->second->getHeight(pos, maxSearchDist, fromUnder);
                if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
                    m_LoadMapLock.release();
                if (!(height < G3D::finf()))
                    return height = VMAP_INVALID_HEIGHT_VALUE; // No height

                return height;
            }
        }

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.release();

        return VMAP_INVALID_HEIGHT_VALUE;
    }

    bool VMapManager2::getAreaInfo(unsigned int mapId, float x, float y, float& z, uint32& flags, int32& adtId, int32& rootId, int32& groupId) const
    {
        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.acquire_read();

        /// Optimization, vmaps are always enable
        //if (!IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_AREAFLAG))
        {
            auto instanceTree = GetMapTree(mapId);
            if (instanceTree != iInstanceMapTrees.end())
            {
                Vector3 pos = convertPositionToInternalRep(x, y, z);
                bool result = instanceTree->second->getAreaInfo(pos, flags, adtId, rootId, groupId);
                if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
                    m_LoadMapLock.release();
                // z is not touched by convertPositionToInternalRep(), so just copy
                z = pos.z;
                return result;
            }
        }

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.release();

        return false;
    }

    bool VMapManager2::GetLiquidLevel(uint32 mapId, float x, float y, float z, uint8 reqLiquidType, float& level, float& floor, uint32& type) const
    {
        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.acquire_read();

        /// Optimization, vmaps are always enable
        //if (!IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_LIQUIDSTATUS))
        {
            auto instanceTree = GetMapTree(mapId);
            if (instanceTree != iInstanceMapTrees.end())
            {
                LocationInfo info;
                Vector3 pos = convertPositionToInternalRep(x, y, z);
                if (instanceTree->second->GetLocationInfo(pos, info))
                {
                    floor = info.ground_Z;
                    ASSERT(floor < std::numeric_limits<float>::max());
                    ASSERT(info.hitModel);
                    type = info.hitModel->GetLiquidType();  // entry from LiquidType.db2
                    if (reqLiquidType && !(GetLiquidFlagsPtr(type) & reqLiquidType))
                    {
                        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
                            m_LoadMapLock.release();
                        return false;
                    }
                    ASSERT(info.hitInstance);
                    if (info.hitInstance->GetLiquidLevel(pos, info, level))
                    {
                        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
                            m_LoadMapLock.release();
                        return true;
                    }
                }
            }
        }

        if (sWorld->getBoolConfig(CONFIG_USE_VMAP_LOAD_LOCK))
            m_LoadMapLock.release();

        return false;
    }

    int32 VMapManager2::GetDistanceToClosestPrimaryTile(uint32 mapId, int32 x, int32 y) const
    {
        auto instanceTree = GetMapTree(mapId);
        if (instanceTree != iInstanceMapTrees.end())
            return instanceTree->second->GetDistanceToClosestPrimaryTile(x, y);
        return std::numeric_limits<int32>::max();
    }

    WorldModel* VMapManager2::acquireModelInstance(const std::string& basepath, const std::string& filename)
    {
        //! Critical section, thread safe access to iLoadedModelFiles
        std::lock_guard<std::mutex> lock(LoadedModelFilesLock);

        auto model = iLoadedModelFiles.find(filename);
        if (model == iLoadedModelFiles.end())
        {
            WorldModel* worldmodel = new WorldModel();

            if (filename.find("7Ne_Nightelf_Druidcat_Statue04") != std::string::npos)
            {
                if (!worldmodel->readFile(basepath + "7Ne_Nightelf_Druidcat_Statue02_Base.m2" + ".vmo"))
                {
                    sLog->outDebug(LOG_FILTER_MAPS, "VMapManager2: could not load '%s%s.m2'", basepath.c_str(), filename.c_str());
                    delete worldmodel;
                    return NULL;
                }
            }
            else if (filename.find("7Ne_Nightelf_Druidcat_Statue03") != std::string::npos)
            {
                if (!worldmodel->readFile(basepath + "7Ne_Nightelf_Druidcat_Statue03_Normalcollision.m2" + ".vmo"))
                {
                    sLog->outDebug(LOG_FILTER_MAPS, "VMapManager2: could not load '%s%s.m2'", basepath.c_str(), filename.c_str());
                    delete worldmodel;
                    return NULL;
                }
            }
            else
            {
                if (!worldmodel->readFile(basepath + filename + ".vmo"))
                {
                    sLog->outDebug(LOG_FILTER_MAPS, "VMapManager2: could not load '%s%s.vmo'", basepath.c_str(), filename.c_str());
                    delete worldmodel;
                    return NULL;
                }
            }

            sLog->outDebug(LOG_FILTER_MAPS, "VMapManager2: loading file '%s%s'", basepath.c_str(), filename.c_str());
            model = iLoadedModelFiles.insert(std::pair<std::string, ManagedModel>(filename, ManagedModel())).first;
            model->second.setModel(worldmodel);
        }
        model->second.incRefCount();
        return model->second.getModel();
    }

    void VMapManager2::releaseModelInstance(const std::string &filename)
    {
        //! Critical section, thread safe access to iLoadedModelFiles
        std::lock_guard<std::mutex> lock(LoadedModelFilesLock);

        auto model = iLoadedModelFiles.find(filename);
        if (model == iLoadedModelFiles.end())
        {
            sLog->outDebug(LOG_FILTER_MAPS, "VMapManager2: trying to unload non-loaded file '%s'", filename.c_str());
            return;
        }
        if (model->second.decRefCount() == 0)
        {
            sLog->outDebug(LOG_FILTER_MAPS, "VMapManager2: unloading file '%s'", filename.c_str());
            delete model->second.getModel();
            iLoadedModelFiles.erase(model);
        }
    }

    bool VMapManager2::existsMap(const char* basePath, unsigned int mapId, int x, int y)
    {
        return StaticMapTree::CanLoadMap(std::string(basePath), mapId, x, y, this);
    }

    void VMapManager2::getInstanceMapTree(InstanceTreeMap &instanceMapTree)
    {
        instanceMapTree = iInstanceMapTrees;
    }

    int32 VMapManager2::getParentMapId(uint32 mapId) const
    {
        auto itr = iParentMapData.find(mapId);
        if (itr != iParentMapData.end())
            return int32(itr->second);
        return -1;
    }

} // namespace VMAP
