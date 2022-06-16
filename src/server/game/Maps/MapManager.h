////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_MAPMANAGER_H
#define TRINITY_MAPMANAGER_H

#include "Define.h"
#include "Common.h"
#include "Map.h"
#include "GridStates.h"
#include "MapUpdater.h"

class Transport;
struct TransportCreatureProto;
struct CustomInstanceZone;

class MapManager
{
    friend class ACE_Singleton<MapManager, ACE_Thread_Mutex>;

    public:
        Map* CreateBaseMap(uint32 mapId, uint32 zoneId);
        Map* FindBaseNonInstanceMap(uint32 mapId, uint32 zoneId) const;
        Map* CreateMap(uint32 p_MapId, uint32 p_ZoneId, Player* p_Player, CustomInstanceZone const* p_CustomInstanceZone = nullptr);
        Map* FindMap(uint32 mapId, uint32 instanceId) const;

        std::list<Map*>* GetZoneMapsList(uint32 p_MapID, uint32 p_ZoneID)
        {
            std::lock_guard<std::recursive_mutex> l_Guard(m_MapsLock);
            return &m_ZonesMap[(p_MapID << 16) | (p_ZoneID & 0xFFFF)];
        }

        uint32 GetAreaId(uint32 mapid, float x, float y, float z) const
        {
            Map const* m = const_cast<MapManager*>(this)->CreateBaseMap(mapid, 0);
            return m->GetAreaId(x, y, z);
        }
        uint32 GetZoneId(uint32 mapid, float x, float y, float z) const
        {
            Map const* m = const_cast<MapManager*>(this)->CreateBaseMap(mapid, 0);
            return m->GetZoneId(x, y, z);
        }
        void GetZoneAndAreaId(uint32& zoneid, uint32& areaid, uint32 mapid, float x, float y, float z)
        {
            Map const* m = const_cast<MapManager*>(this)->CreateBaseMap(mapid, 0);
            m->GetZoneAndAreaId(zoneid, areaid, x, y, z);
        }

        void Initialize(void);
        void UpdateLoop();

        void SetGridCleanUpDelay(uint32 t)
        {
            if (t < MIN_GRID_DELAY)
                i_gridCleanUpDelay = MIN_GRID_DELAY;
            else
                i_gridCleanUpDelay = t;
        }

        void SetMapUpdateInterval(uint32 t)
        {
            if (t < MIN_MAP_UPDATE_DELAY)
                t = MIN_MAP_UPDATE_DELAY;

            i_timer.SetInterval(t);
            i_timer.Reset();
        }

        //void LoadGrid(int mapid, int instId, float x, float y, const WorldObject* obj, bool no_unload = false);
        void UnloadAll();

        static bool ExistMapAndVMap(uint32 mapid, float x, float y);
        static bool IsValidMAP(uint32 mapid, bool startUp);

        static bool IsValidMapCoord(uint32 mapid, float x, float y)
        {
            return IsValidMAP(mapid, false) && JadeCore::IsValidMapCoord(x, y);
        }

        static bool IsValidMapCoord(uint32 mapid, float x, float y, float z)
        {
            return IsValidMAP(mapid, false) && JadeCore::IsValidMapCoord(x, y, z);
        }

        static bool IsValidMapCoord(uint32 mapid, float x, float y, float z, float o)
        {
            return IsValidMAP(mapid, false) && JadeCore::IsValidMapCoord(x, y, z, o);
        }

        static bool IsValidMapCoord(WorldLocation const& loc)
        {
            return IsValidMapCoord(loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), loc.GetOrientation());
        }

        // modulos a radian orientation to the range of 0..2PI
        static float NormalizeOrientation(float o)
        {
            // fmod only supports positive numbers. Thus we have
            // to emulate negative numbers
            if (o < 0)
            {
                float mod = o *-1;
                mod = fmod(mod, 2.0f * static_cast<float>(M_PI));
                mod = -mod + 2.0f * static_cast<float>(M_PI);
                return mod;
            }
            return fmod(o, 2.0f * static_cast<float>(M_PI));
        }

        void DoDelayedMovesAndRemoves();

        //Load transport to instance
        Transport* LoadTransportInMap(Map* instance, uint32 goEntry, uint32 period);
        void UnLoadTransportFromMap(Transport* t);
        void LoadTransportForPlayers(Player* player);
        void UnLoadTransportForPlayers(Player* player);

        bool CanPlayerEnter(uint32 mapid, Player* player, bool loginCheck = false);
        void InitializeVisibilityDistanceInfo();

        /* statistics */
        uint32 GetNumInstances();
        uint32 GetNumPlayersInInstances();

        // Instance ID management
        void InitInstanceIds();
        uint32 GenerateInstanceId();
        void RegisterInstanceId(uint32 instanceId);
        void FreeInstanceId(uint32 instanceId);

        // Map max diff functions
        bool HaveMaxDiff() const { return m_mapDiffLimit; }
        void SetMapDiffLimit(bool value) { m_mapDiffLimit = value; }

        uint32 GetNextInstanceId() const { return m_NextInstanceID; };
        void SetNextInstanceId(uint32 nextInstanceId) { m_NextInstanceID = nextInstanceId; };

        MapUpdater * GetMapUpdater() { return &m_MapsUpdater; }

        std::list<Map*> GetMapsPerZoneId(uint32 p_MapID, uint32 p_ZoneId)
        {
            std::lock_guard<std::recursive_mutex> l_Guard(m_MapsLock);
            return m_ZonesMap[p_MapID << 16 | p_ZoneId];
        }

        bool m_LogMapPerformance;

        Map* FindBaseMap(uint32 mapId, uint32 zoneId) const
        {
            MapMapType::const_iterator iter = i_maps.find(std::make_pair(mapId, zoneId));
            return (iter == i_maps.end() ? NULL : iter->second);
        }

        void RegisterBigObjectPositionForMap(uint32 p_MapId, std::pair<float, float> p_Position)
        {
            m_MapBigObjectsPositions[p_MapId].push_back(p_Position);
        }

        std::vector<std::pair<float, float>> const& GetBigObjectPositionsForMap(uint32 p_MapId)
        {
            return m_MapBigObjectsPositions[p_MapId];
        }

        void AddTaskToAllMaps(std::function<void()> p_Task);
        void AddTaskToAllMapsWithId(uint32 p_MapId, std::function<void()> p_Task);


    private:
        typedef ACE_Based::LockedMap<std::pair<uint32, uint32>, Map*> MapMapType;
        typedef std::set<uint32> InstanceIDs;
        typedef std::map<uint32, std::list<Map*>> MapZones;

        // debugging code, should be deleted some day
        void checkAndCorrectGridStatesArray();              // just for debugging to find some memory overwrites
        GridState* i_GridStates[MAX_GRID_STATE];            // shadow entries to the global array in Map.cpp
        int i_GridStateErrorCount;

        MapManager();
        ~MapManager();

        MapManager(const MapManager &);
        MapManager& operator=(const MapManager &);

        std::recursive_mutex m_MapsLock;
        uint32 i_gridCleanUpDelay;
        MapMapType i_maps;
        IntervalTimer i_timer;

        MapZones m_ZonesMap;
        InstanceIDs m_InstanceIDs;
        uint32 m_NextInstanceID;
        MapUpdater m_MapsUpdater;
        bool m_mapDiffLimit;

        ACE_Based::LockedMap<uint32, std::vector<std::pair<float, float>>> m_MapBigObjectsPositions;
};

class MapsUpdaterRunnable : public ACE_Based::Runnable
{
    public:
        void run();
};

#define sMapMgr ACE_Singleton<MapManager, ACE_Thread_Mutex>::instance()
#endif
