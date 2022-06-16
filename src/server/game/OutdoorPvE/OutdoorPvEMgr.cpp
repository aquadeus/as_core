/////////////////////////////////////////////////////////////////////////   ///////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "OutdoorPvEMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "DisableMgr.h"
#include "ScriptMgr.h"

void OutdoorPvEMgr::LoadOutdoorPvETemplate()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0       1
    QueryResult result = WorldDatabase.Query("SELECT map, ScriptName FROM outdoor_pve_template");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 outdoor PvE definitions. DB table `outdoor_pve_template` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* l_Fields = result->Fetch();

        uint16 l_MapId = l_Fields[0].GetUInt16();

        if (!MapManager::IsValidMAP(l_MapId, true))
        {
            sLog->outError(LOG_FILTER_SQL, "ObjectMgr::LoadOutdoorPvETemplate: bad mapid %d for template!", l_MapId);
            continue;
        }

        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_OUTDOORPVE, l_MapId, NULL))
            continue;

        m_OutdoorPvEScriptIDs[l_MapId] = sObjectMgr->GetScriptId(l_Fields[1].GetCString());

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u outdoor PvE definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

uint32 OutdoorPvEMgr::GetScriptId(uint16 p_MapId)
{
    auto l_Itr = m_OutdoorPvEScriptIDs.find(p_MapId);
    if (l_Itr != m_OutdoorPvEScriptIDs.end())
        return l_Itr->second;

    return 0;
}

void OutdoorPvEMgr::CreateOutdoorPvEScript(Map* p_Map)
{
    sLog->outAshran("OutdoorPvEMgr::CreateOutdoorPvEScript: %u (%u)" , p_Map->GetId(), p_Map->GetInstanceZoneId());

    if (OutdoorPvEScript* l_Script = sScriptMgr->CreateOutdoorPvE(p_Map))
    {
        l_Script->Initialize();

        p_Map->SetOutoorPve(l_Script);

        sLog->outAshran("OutdoorPvEMgr::CreateOutdoorPvEScript: %u SetOutoorPve (%u)", p_Map->GetId(), p_Map->GetInstanceZoneId());
    }

    if (p_Map->GetId() == 1779)
    {
        ACE_Stack_Trace l_StackTrace;
        sLog->outAshran("%s", l_StackTrace.c_str());
    }
}

ZoneScript* OutdoorPvEMgr::GetZoneScript(Map* p_Map)
{
    return p_Map->GetOudoorPvE();
}

void OutdoorPvEMgr::OnScenarioCreate(Map* p_Map, uint32 p_ScenarioGuid)
{
    if (!p_Map->GetOudoorPvE())
        return;

    p_Map->GetOudoorPvE()->OnScenarioCreate(p_ScenarioGuid);
}

void OutdoorPvEMgr::OnScenarioNextStep(Map* p_Map, uint32 p_NewStep)
{
    if (!p_Map->GetOudoorPvE())
        return;

    p_Map->GetOudoorPvE()->OnScenarioNextStep(p_NewStep);
}

void OutdoorPvEMgr::OnScenarioComplete(Map* p_Map)
{
    if (!p_Map->GetOudoorPvE())
        return;

    p_Map->GetOudoorPvE()->OnScenarioComplete();
}

void OutdoorPvEMgr::OnPlayerEnter(Map* p_Map, Player* p_Player)
{
    if (!p_Map->GetOudoorPvE())
        return;

    p_Map->GetOudoorPvE()->OnPlayerEnter(p_Player);
}
