////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "WorldSession.h"
#include "Common.h"

#define MAP_EVENT_UPDATE_DELAY 50

enum ePOIGroups
{
    BrokenShoreInvasions    = 5006,
    GroupSentinax           = 6388
};

struct AreaPOI
{
    public:
        AreaPOI();

        uint32 GroupID;
        uint32 StartTime;
        uint32 Duration;
        uint32 WorldStateID;
        uint32 WorldStateValue;
        std::vector<std::pair<uint32, uint32>> WorldStates;
        uint32 RepeatInterval;
        std::set<uint32> DependantEventAreaPoiID;
        bool   Active;

        uint32 GetWorldStateID(std::vector<std::pair<uint32, uint32>>& p_WorldStates) const;

        void SaveToDB();

        void SetAreaPoiID(uint32 p_AreaPoiID);
        uint32 GetAreaPoiID() const;

    private:
        uint32 AreaPoiID;
};

class POIMgr
{
    public:
        POIMgr();

        void LoadMapEvents();

        /// Only in Session thread
        void Update(uint32 const p_Diff);
        void BuildAreaPOIUpdatePacket(WorldPacket& p_Data, uint64 p_PlayerGUID) const;
        void AddEventForPlayer(uint32 p_AreaPoiID, uint64 p_PlayerGUID, uint32 p_Duration = 0, uint32 p_WorldStateValue = 1);
        void EndEventForPlayer(uint32 p_AreaPoiID, uint64 p_PlayerGUID);

        /// Usable at any time
        AreaPOIEntry const* GetCurrentPOIEntryForGroup(uint32 p_GroupID) const;
        std::set<uint32> GetActivePOIIds() const;
        std::set<uint32> GetAllPOIIds() const;
        AreaPOIEntry const* GetAreaPOIEntry(uint32 p_AreaPoiID) const;

        /// Only In Map thread
        void InitializeForPlayer(Player* p_Player);

    private:
        void UpdateForPlayer(AreaPOI const& p_MapEvent, uint64 p_PlayerGUID);
        void UpdateForPlayers(AreaPOI const& p_MapEvent) const;
        void SendAreaPoiUpdate(Player* p_Player) const;
        void SendUpdateWorldStates(Player* p_Player, AreaPOI const* p_MapEvent) const;

        void UpdateMapEvent(uint32 p_GroupID);
        void LoadMapEventsPools();

    private:
        std::map<uint32, AreaPOI> m_EventMap;
        mutable std::recursive_mutex m_EventMapLock;

        std::list<AreaPOI> m_ActiveEvents;
        mutable std::recursive_mutex m_ActiveEventsLock;

        std::map<uint64, std::list<AreaPOI>> m_PlayerEventMap;
        mutable std::recursive_mutex m_PlayerEventMapLock;

        std::map<uint32, std::set<uint32>> m_MapEventPools;

        int32 m_TimeToUpdate;
};

#define sPOIMgr ACE_Singleton<POIMgr, ACE_Null_Mutex>::instance()

