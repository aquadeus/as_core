////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SCENARIOMGR_H
#define SCENARIOMGR_H

#include "Common.h"
#include "Scenario.h"

class Scenario;
struct ScenarioStepEntry;

typedef std::vector<ScenarioStepEntry const*> ScenarioSteps;
typedef ACE_Based::LockedMap<uint32 /*instance_id*/, Scenario*> ScenarioMap;
typedef std::unordered_map<uint32, ScenarioSteps> ScenarioStepsByScenarioMap;
typedef std::vector<ScenarioSteps*> ScenarioStepsByScenarioVector;

class ScenarioMgr
{
    public:
        ScenarioMgr();
        ~ScenarioMgr();

        static ScenarioMgr* instance()
        {
            static ScenarioMgr g_Instance;
            return &g_Instance;
        }

        Scenario* AddScenario(Map* p_Map, LFGDungeonEntry const* p_DungeonData, Player* p_Player, bool p_Find = false);
        void RemoveScenario(uint32 p_ScenarioGuid);
        Scenario* GetScenario(uint32 p_ScenarioGuid);

        ScenarioSteps const* GetScenarioSteps(uint32 p_ScenarioId);
        bool HasScenarioStep(LFGDungeonEntry const* p_DungeonData, Player* p_Player, Map* p_Map);

        uint32 GenerateNewGuid();

    private:

        ScenarioMap m_ScenarioStore;
        ScenarioStepsByScenarioMap    m_stepMap;
        ScenarioStepsByScenarioVector m_stepVector;

        std::atomic<uint32> m_HigherScenarioGuid;
};

#define sScenarioMgr ScenarioMgr::instance()

#endif
