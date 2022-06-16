////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include "MMapManager.h"
#include "Log.h"
#include "Config.h"
#include "MapDefines.h"
#include "Errors.h"
#include <stdio.h>
#include "MapManager.h"

namespace MMAP
{
    static char const* const MAP_FILE_NAME_FORMAT = "%smmaps/%04i.mmap";
    static char const* const TILE_FILE_NAME_FORMAT = "%smmaps/%04i%02i%02i.mmtile";

    // ######################## MMapManager ########################
    MMapManager::~MMapManager()
    {
        for (MMapDataSet::iterator i = loadedMMaps.begin(); i != loadedMMaps.end(); ++i)
            delete i->second;

        // by now we should not have maps loaded
        // if we had, tiles in MMapData->mmapLoadedTiles, their actual data is lost!
    }

    void MMapManager::InitializeThreadUnsafe(std::unordered_map<uint32, std::vector<uint32>> const& mapData)
    {
        childMapData = mapData;
        // the caller must pass the list of all mapIds that will be used in the VMapManager2 lifetime
        for (std::pair<uint32 const, std::vector<uint32>> const& mapId : mapData)
        {
            loadedMMaps.insert(MMapDataSet::value_type(mapId.first, nullptr));
            for (uint32 childMapId : mapId.second)
                parentMapData[childMapId] = mapId.first;
        }

        thread_safe_environment = false;
    }

    MMapDataSet::const_iterator MMapManager::GetMMapData(uint32 mapId) const
    {
        // return the iterator if found or end() if not found/NULL
        MMapDataSet::const_iterator itr = loadedMMaps.find(mapId);
        if (itr != loadedMMaps.cend() && !itr->second)
            itr = loadedMMaps.cend();

        return itr;
    }

    bool MMapManager::loadMapData(std::string const& basePath, uint32 mapId)
    {
        // we already have this map loaded?
        MMapDataSet::iterator itr = loadedMMaps.find(mapId);
        if (itr != loadedMMaps.end())
        {
            if (itr->second)
                return true;
        }
        else
        {
            if (thread_safe_environment)
                itr = loadedMMaps.insert(MMapDataSet::value_type(mapId, nullptr)).first;
            else
                WPError(false, "Invalid mapId passed to MMapManager after startup in thread unsafe environment");
        }

        // load and init dtNavMesh - read parameters from file
        char l_Buffer[4096];
        sprintf(l_Buffer, MAP_FILE_NAME_FORMAT, basePath.c_str(), mapId);

        std::string fileName = l_Buffer;
        FILE* file = fopen(fileName.c_str(), "rb");
        if (!file)
        {
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:loadMapData: Error: Could not open mmap file '%s'", fileName.c_str());
            return false;
        }

        dtNavMeshParams params;
        uint32 count = uint32(fread(&params, sizeof(dtNavMeshParams), 1, file));
        fclose(file);
        if (count != 1)
        {
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:loadMapData: Error: Could not read params from file '%s'", fileName.c_str());
            return false;
        }

        dtNavMesh* mesh = dtAllocNavMesh();
        ASSERT(mesh);
        if (dtStatusFailed(mesh->init(&params)))
        {
            dtFreeNavMesh(mesh);
            sLog->outError(LOG_FILTER_GENERAL, "MMAP:loadMapData: Failed to initialize dtNavMesh for mmap %04u from file %s", mapId, fileName.c_str());
            return false;
        }

        sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:loadMapData: Loaded %04i.mmap", mapId);

        // store inside our map list
        MMapData* mmap_data = new MMapData(mesh);

        itr->second = mmap_data;
        return true;
    }

    uint32 MMapManager::packTileID(int32 x, int32 y)
    {
        return uint32(x << 16 | y);
    }

    ACE_RW_Thread_Mutex& MMapManager::GetMMapLock(uint32 mapId, uint32 zoneId)
    {
        Map* map = sMapMgr->FindBaseMap(mapId, 0);
        if (!map)
            return this->MMapLock;

        return map->GetMMapLock();
    }

    bool MMapManager::loadMap(std::string const& basePath, uint32 mapId, uint32 zoneId, int32 x, int32 y)
    {
        if (!loadMapImpl(basePath, mapId, zoneId, x, y))
            return false;
        bool success = true;
        auto childMaps = childMapData.find(mapId);
        if (childMaps != childMapData.end())
            for (uint32 childMapId : childMaps->second)
                if (!loadMapImpl(basePath, childMapId, zoneId, x, y))
                    success = false;
        return success;
    }

    bool MMapManager::loadMapImpl(std::string const& basePath, uint32 mapId, uint32 zoneId, int32 x, int32 y)
    {
        TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, MMapManagerLock);

        // make sure the mmap is loaded and ready to load tiles
        if (!loadMapData(basePath, mapId))
            return false;

        // get this mmap data
        MMapData* mmap = loadedMMaps[mapId];
        ASSERT(mmap->navMesh);

        // check if we already have this tile loaded
        uint32 packedGridPos = packTileID(x, y);
        if (mmap->loadedTileRefs.find(packedGridPos) != mmap->loadedTileRefs.end())
            return false;

        // load this tile :: mmaps/MMMMXXYY.mmtile
        char l_Buffer[4096];
        sprintf(l_Buffer, TILE_FILE_NAME_FORMAT, basePath.c_str(), mapId, x, y);
        std::string fileName = l_Buffer;
        FILE* file = fopen(fileName.c_str(), "rb");
        if (!file)
        {
            auto parentMapItr = parentMapData.find(mapId);
            if (parentMapItr != parentMapData.end())
            {
                char l_Buffer[4096];
                sprintf(l_Buffer, TILE_FILE_NAME_FORMAT, basePath.c_str(), parentMapItr->second, x, y);
                std::string fileName = l_Buffer;
                file = fopen(fileName.c_str(), "rb");
            }
        }

        if (!file)
        {
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:loadMap: Could not open mmtile file '%s'", fileName.c_str());
            return false;
        }

        // read header
        MmapTileHeader fileHeader;
        if (fread(&fileHeader, sizeof(MmapTileHeader), 1, file) != 1 || fileHeader.mmapMagic != MMAP_MAGIC)
        {
            sLog->outError(LOG_FILTER_GENERAL, "MMAP:loadMap: Bad header in mmap %04u%02i%02i.mmtile", mapId, x, y);
            fclose(file);
            return false;
        }

        if (fileHeader.mmapVersion != MMAP_VERSION)
        {
            sLog->outError(LOG_FILTER_GENERAL, "MMAP:loadMap: %04u%02i%02i.mmtile was built with generator v%i, expected v%i",
                mapId, x, y, fileHeader.mmapVersion, MMAP_VERSION);
            fclose(file);
            return false;
        }

        unsigned char* data = (unsigned char*)dtAlloc(fileHeader.size, DT_ALLOC_PERM);
        ASSERT(data);

        size_t result = fread(data, fileHeader.size, 1, file);
        if (!result)
        {
            sLog->outError(LOG_FILTER_GENERAL, "MMAP:loadMap: Bad header or data in mmap %04u%02i%02i.mmtile", mapId, x, y);
            fclose(file);
            return false;
        }

        fclose(file);

        dtMeshHeader* header = (dtMeshHeader*)data;
        dtTileRef tileRef = 0;

        {
            TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, GetMMapLock(mapId, zoneId));
            // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
            if (dtStatusSucceed(mmap->navMesh->addTile(data, fileHeader.size, DT_TILE_FREE_DATA, 0, &tileRef)))
            {
                mmap->loadedTileRefs.insert(std::pair<uint32, dtTileRef>(packedGridPos, tileRef));
                ++loadedTiles;
                sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:loadMap: Loaded mmtile %04i[%02i, %02i] into %04i[%02i, %02i]", mapId, x, y, mapId, header->x, header->y);

                return true;
            }
        }
        dtFree(data);
        return false;
    }

    bool MMapManager::loadMapInstance(std::string const& basePath, uint32 mapId, uint32 instanceId)
    {
        if (!loadMapInstanceImpl(basePath, mapId, instanceId))
            return false;
        bool success = true;
        auto childMaps = childMapData.find(mapId);
        if (childMaps != childMapData.end())
            for (uint32 childMapId : childMaps->second)
                if (!loadMapInstanceImpl(basePath, childMapId, instanceId))
                    success = false;
        return success;
    }

    bool MMapManager::loadMapInstanceImpl(std::string const& basePath, uint32 mapId, uint32 instanceId)
    {
        if (!loadMapData(basePath, mapId))
            return false;
        MMapData* mmap = loadedMMaps[mapId];
        if (mmap->navMeshQueries.find(instanceId) != mmap->navMeshQueries.end())
            return true;
        // allocate mesh query
        dtNavMeshQuery* query = dtAllocNavMeshQuery();
        ASSERT(query);
        if (dtStatusFailed(query->init(mmap->navMesh, 1024)))
        {
            dtFreeNavMeshQuery(query);
            return false;
        }
        mmap->navMeshQueries.insert(std::pair<uint32, dtNavMeshQuery*>(instanceId, query));
        return true;
    }

    bool MMapManager::unloadMap(uint32 mapId, uint32 zoneId, int32 x, int32 y)
    {
        auto childMaps = childMapData.find(mapId);
        if (childMaps != childMapData.end())
            for (uint32 childMapId : childMaps->second)
                unloadMapImpl(childMapId, zoneId, x, y);
        return unloadMapImpl(mapId, zoneId,  x, y);
    }

    bool MMapManager::unloadMapImpl(uint32 mapId, uint32 zoneId, int32 x, int32 y)
    {
        TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, MMapManagerLock);

        // check if we have this map loaded
        MMapDataSet::const_iterator itr = GetMMapData(mapId);
        if (itr == loadedMMaps.end())
        {
            // file may not exist, therefore not loaded
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMap: Asked to unload not loaded navmesh map. %04u%02i%02i.mmtile", mapId, x, y);
            return false;
        }

        MMapData* mmap = itr->second;

        // check if we have this tile loaded
        uint32 packedGridPos = packTileID(x, y);
        auto tileRefItr = mmap->loadedTileRefs.find(packedGridPos);
        if (tileRefItr == mmap->loadedTileRefs.end())
        {
            // file may not exist, therefore not loaded
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMap: Asked to unload not loaded navmesh tile. %04u%02i%02i.mmtile", mapId, x, y);
            return false;
        }

        {
            TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, GetMMapLock(mapId, zoneId));

            // unload, and mark as non loaded
            if (dtStatusFailed(mmap->navMesh->removeTile(tileRefItr->second, nullptr, nullptr)))
            {
                // this is technically a memory leak
                // if the grid is later reloaded, dtNavMesh::addTile will return error but no extra memory is used
                // we cannot recover from this error - assert out
                sLog->outError(LOG_FILTER_GENERAL, "MMAP:unloadMap: Could not unload %04u%02i%02i.mmtile from navmesh", mapId, x, y);
                abort();
            }
            else
            {
                mmap->loadedTileRefs.erase(tileRefItr);
                --loadedTiles;
                sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMap: Unloaded mmtile %04i[%02i, %02i] from %04i", mapId, x, y, mapId);
                return true;
            }
        }

        return false;
    }

    bool MMapManager::unloadMap(uint32 mapId, uint32 zoneId)
    {
        auto childMaps = childMapData.find(mapId);
        if (childMaps != childMapData.end())
            for (uint32 childMapId : childMaps->second)
                unloadMapImpl(childMapId, zoneId);
        return unloadMapImpl(mapId, zoneId);
    }

    bool MMapManager::unloadMapImpl(uint32 mapId, uint32 zoneId)
    {
        TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, MMapManagerLock);

        MMapDataSet::iterator itr = loadedMMaps.find(mapId);
        if (itr == loadedMMaps.end() || !itr->second)
        {
            // file may not exist, therefore not loaded
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMap: Asked to unload not loaded navmesh map %04u", mapId);
            return false;
        }

        // unload all tiles from given map
        MMapData* mmap = itr->second;
        for (MMapTileSet::iterator i = mmap->loadedTileRefs.begin(); i != mmap->loadedTileRefs.end(); ++i)
        {
            uint32 x = (i->first >> 16);
            uint32 y = (i->first & 0x0000FFFF);
            {
                TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, GetMMapLock(mapId, zoneId));

                if (dtStatusFailed(mmap->navMesh->removeTile(i->second, nullptr, nullptr)))
                    sLog->outError(LOG_FILTER_GENERAL, "MMAP:unloadMap: Could not unload %04u%02i%02i.mmtile from navmesh", mapId, x, y);
                else
                {
                    --loadedTiles;
                    sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMap: Unloaded mmtile %04i[%02i, %02i] from %04i", mapId, x, y, mapId);
                }
            }
        }

        delete mmap;
        itr->second = nullptr;
        sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMap: Unloaded %04i.mmap", mapId);

        return true;
    }

    bool MMapManager::unloadMapInstance(uint32 mapId, uint32 instanceId)
    {
        TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, MMapManagerLock);

        // check if we have this map loaded
        MMapDataSet::const_iterator itr = GetMMapData(mapId);
        if (itr == loadedMMaps.end())
        {
            // file may not exist, therefore not loaded
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMapInstance: Asked to unload not loaded navmesh map %04u", mapId);
            return false;
        }

        MMapData* mmap = itr->second;
        if (mmap->navMeshQueries.find(instanceId) == mmap->navMeshQueries.end())
        {
            sLog->outDebug(LOG_FILTER_GENERAL, "MMAP:unloadMapInstance: Asked to unload not loaded dtNavMeshQuery mapId %04u instanceId %u", mapId, instanceId);
            return false;
        }

        dtNavMeshQuery* query = mmap->navMeshQueries[instanceId];

        dtFreeNavMeshQuery(query);
        mmap->navMeshQueries.erase(instanceId);
        sLog->outDebug(LOG_FILTER_GENERAL,"MMAP:unloadMapInstance: Unloaded mapId %04u instanceId %u", mapId, instanceId);

        return true;
    }

    dtNavMesh const* MMapManager::GetNavMesh(uint32 mapId)
    {
        MMapDataSet::const_iterator itr = GetMMapData(mapId);
        if (itr == loadedMMaps.end())
            return nullptr;

        return itr->second->navMesh;
    }

    dtNavMeshQuery const* MMapManager::GetNavMeshQuery(uint32 mapId, uint32 instanceId)
    {
        auto itr = GetMMapData(mapId);
        if (itr == loadedMMaps.end())
            return nullptr;

        TRINITY_WRITE_GUARD(ACE_RW_Thread_Mutex, GetMMapLock(mapId, instanceId));

        auto queryItr = itr->second->navMeshQueries.find(instanceId);
        if (queryItr == itr->second->navMeshQueries.end())
            return nullptr;

        return queryItr->second;
    }
}
