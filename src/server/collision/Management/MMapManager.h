////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MMAP_MANAGER_H
#define _MMAP_MANAGER_H

#include "Define.h"
#include "DetourAlloc.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "MapDefines.h"
#include "Common.h"

//  move map related classes
namespace MMAP
{
    typedef ACE_Based::LockedMap<uint32, dtTileRef> MMapTileSet;
    typedef ACE_Based::LockedMap<uint32, dtNavMeshQuery*> NavMeshQuerySet;

    class MMapData
    {
    public:
        MMapData(dtNavMesh* mesh) : navMesh(mesh) { }
        ~MMapData()
        {
            for (NavMeshQuerySet::iterator i = navMeshQueries.begin(); i != navMeshQueries.end(); ++i)
                dtFreeNavMeshQuery(i->second);
            if (navMesh)
                dtFreeNavMesh(navMesh);
        }

        // we have to use single dtNavMeshQuery for every instance, since those are not thread safe
        NavMeshQuerySet navMeshQueries;     // instanceId to query

        dtNavMesh* navMesh;
        MMapTileSet loadedTileRefs;        // maps [map grid coords] to [dtTile]
    };


    typedef std::unordered_map<uint32, MMapData*> MMapDataSet;

    // singleton class
    // holds all all access to mmap loading unloading and meshes
    class MMapManager
    {
        public:
            MMapManager() : loadedTiles(0), thread_safe_environment(true) {}
            ~MMapManager();

            void InitializeThreadUnsafe(std::unordered_map<uint32, std::vector<uint32>> const& mapData);
            bool loadMap(std::string const& basePath, uint32 mapId, uint32 zoneId, int32 x, int32 y);
            bool loadMapInstance(std::string const& basePath, uint32 mapId, uint32 instanceId);
            bool unloadMap(uint32 mapId, uint32 zoneId, int32 x, int32 y);
            bool unloadMap(uint32 mapId, uint32 zoneId);
            bool unloadMapInstance(uint32 mapId, uint32 instanceId);

            // the returned [dtNavMeshQuery const*] is NOT threadsafe
            dtNavMeshQuery const* GetNavMeshQuery(uint32 mapId, uint32 instanceId);
            dtNavMesh const* GetNavMesh(uint32 mapId);

            ACE_RW_Thread_Mutex& GetMMapLock(uint32 mapId, uint32 zoneId);
            ACE_RW_Thread_Mutex& GetMMapGeneralLock() { return MMapLock; }
            ACE_RW_Thread_Mutex& GetManagerLock() { return MMapManagerLock; }

            uint32 getLoadedTilesCount() const { return loadedTiles; }
            uint32 getLoadedMapsCount() const { return loadedMMaps.size(); }

        private:
            bool loadMapData(std::string const& basePath, uint32 mapId);
            bool loadMapImpl(std::string const& basePath, uint32 mapId, uint32 zoneId, int32 x, int32 y);
            bool loadMapInstanceImpl(std::string const& basePath, uint32 mapId, uint32 instanceId);
            bool unloadMapImpl(uint32 mapId, uint32 zoneId, int32 x, int32 y);
            bool unloadMapImpl(uint32 mapId, uint32 zoneId);
            uint32 packTileID(int32 x, int32 y);

            MMapDataSet::const_iterator GetMMapData(uint32 mapId) const;
            MMapDataSet loadedMMaps;
            uint32 loadedTiles;
            bool thread_safe_environment;

            std::unordered_map<uint32, std::vector<uint32>> childMapData;
            std::unordered_map<uint32, uint32> parentMapData;

            ACE_RW_Thread_Mutex MMapManagerLock;
            ACE_RW_Thread_Mutex MMapLock;
    };
}

#endif
