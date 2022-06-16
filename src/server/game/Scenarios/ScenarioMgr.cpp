////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScenarioMgr.h"
#include "LFGMgr.h"
#include "WorldSession.h"

ScenarioMgr::ScenarioMgr()
{
    m_HigherScenarioGuid = 1;

    m_stepVector.assign(sScenarioStore.GetNumRows() + 1, nullptr);

    std::unordered_map<std::pair<uint32, uint8>, bool> l_ScenarioIdIndexMap;

    for (uint32 l_Id = 0; l_Id < sScenarioStepStore.GetNumRows(); l_Id++)
    {
        ScenarioStepEntry const* l_Entry = sScenarioStepStore.LookupEntry(l_Id);
        if (!l_Entry)
            continue;

        if (!sScenarioStore.LookupEntry(l_Entry->ScenarioID))
            continue;

        if (m_stepMap[l_Entry->ScenarioID].size() <= l_Entry->Step)
            m_stepMap[l_Entry->ScenarioID].resize(l_Entry->Step + 1);

        if (l_ScenarioIdIndexMap.find(std::make_pair(l_Entry->ScenarioID, l_Entry->Step)) != l_ScenarioIdIndexMap.end())
            continue;

        l_ScenarioIdIndexMap[std::make_pair(l_Entry->ScenarioID, l_Entry->Step)] = true;

        m_stepMap[l_Entry->ScenarioID][l_Entry->Step] = l_Entry;
        m_stepVector[l_Entry->ScenarioID] = &m_stepMap[l_Entry->ScenarioID];
    }
}

ScenarioMgr::~ScenarioMgr()
{
    for (auto l_Itr : m_ScenarioStore)
        delete l_Itr.second;

    m_ScenarioStore.clear();
}

void ScenarioMgr::RemoveScenario(uint32 p_ScenarioGuid)
{
    ScenarioMap::iterator l_Itr = m_ScenarioStore.find(p_ScenarioGuid);
    if (l_Itr == m_ScenarioStore.end())
        return;

    delete l_Itr->second;
    m_ScenarioStore.erase(l_Itr);
}

ScenarioSteps const* ScenarioMgr::GetScenarioSteps(uint32 p_ScenarioId)
{
    return m_stepVector[p_ScenarioId];
}

Scenario* ScenarioMgr::AddScenario(Map* p_Map, LFGDungeonEntry const* p_DungeonData, Player* p_Player, bool p_Find)
{
    uint32 l_ScenarioGuid = GenerateNewGuid();
    return m_ScenarioStore[l_ScenarioGuid] = new Scenario(p_Map, p_DungeonData, p_Player, p_Find, l_ScenarioGuid);
}

Scenario* ScenarioMgr::GetScenario(uint32 p_ScenarioGuid)
{
    ScenarioMap::iterator l_Itr = m_ScenarioStore.find(p_ScenarioGuid);
    return l_Itr != m_ScenarioStore.end() ? l_Itr->second : nullptr;
}

bool ScenarioMgr::HasScenarioStep(LFGDungeonEntry const* p_DungeonData, Player* p_Player, Map* m_Map)
{
    uint32 l_ScenarioId = 0;
    if (ScenarioData const* l_ScenarioData = sObjectMgr->GetScenarioOnMap(p_DungeonData->map, p_DungeonData->difficulty, p_Player->GetTeam(), p_Player->getClass(), p_DungeonData->ID, m_Map->GetInstanceZoneId() & 0xFFFF))
        l_ScenarioId = l_ScenarioData->ScenarioID;
    else if (p_DungeonData->ScenarioID)
        l_ScenarioId = p_DungeonData->ScenarioID;

    if (!l_ScenarioId)
        return false;

    return m_stepVector[l_ScenarioId] != nullptr;
}

uint32 ScenarioMgr::GenerateNewGuid()
{
    return m_HigherScenarioGuid++;
}
