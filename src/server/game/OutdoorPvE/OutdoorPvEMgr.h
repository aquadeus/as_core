////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef OUTDOOR_PVE_MGR_H_
#define OUTDOOR_PVE_MGR_H_

#include "OutdoorPvE.h"

class Player;
class GameObject;
class Creature;
class ZoneScript;
struct GossipMenuItems;

// class to handle player enter / leave / areatrigger / GO use events
class OutdoorPvEMgr
{
    friend class ACE_Singleton<OutdoorPvEMgr, ACE_Null_Mutex>;

    private:
        OutdoorPvEMgr() {}
        ~OutdoorPvEMgr() {};

    public:
        // create outdoor PvE events
        void LoadOutdoorPvETemplate();

        uint32 GetScriptId(uint16 p_MapId);

        void CreateOutdoorPvEScript(Map* p_Map);

        ZoneScript* GetZoneScript(Map* p_Map);

        void OnScenarioCreate(Map* p_Map, uint32 p_ScenarioGuid);
        void OnScenarioNextStep(Map* p_Map, uint32 p_NewStep);
        void OnScenarioComplete(Map* p_Map);

        void OnPlayerEnter(Map* p_Map, Player* p_Player);

    private:

        std::map<uint16, uint32> m_OutdoorPvEScriptIDs;
};

#define sOutdoorPvEMgr ACE_Singleton<OutdoorPvEMgr, ACE_Null_Mutex>::instance()

#endif /*OUTDOOR_PVE_MGR_H_*/
