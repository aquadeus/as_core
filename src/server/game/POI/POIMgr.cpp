////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "POIMgr.hpp"
#include "MapManager.h"
#include "ScriptMgr.h"

AreaPOI::AreaPOI() : GroupID(0), StartTime(0), AreaPoiID(0), Duration(0), WorldStateID(0), WorldStateValue(0), Active(false)
{
    WorldStateValue = 1;
}

void AreaPOI::SetAreaPoiID(uint32 p_AreaPoiID)
{
    AreaPoiID = p_AreaPoiID;

    WorldStates.clear();
    DependantEventAreaPoiID.clear();

    GetWorldStateID(WorldStates);
    if (!WorldStates.empty())
    {
        WorldStateID = WorldStates.front().first;
        WorldStateValue = WorldStates.front().second;
    }

    switch (AreaPoiID)
    {
        case 5175: ///< Azsuna Invasion
            DependantEventAreaPoiID.insert(5273);
            DependantEventAreaPoiID.insert(5248);
            break;
        case 5177: ///< Highmountain Invasion
            DependantEventAreaPoiID.insert(5271);
            DependantEventAreaPoiID.insert(5247);
            break;
        case 5178: ///< Stormheim Invasion
            DependantEventAreaPoiID.insert(5270);
            DependantEventAreaPoiID.insert(5249);
            break;
        case 5210: ///< Val'sharah Invasion
            DependantEventAreaPoiID.insert(5272);
            DependantEventAreaPoiID.insert(5246);
            break;
        default:
            break;
    }
}

void AreaPOI::SaveToDB()
{
    PreparedStatement* l_UpdateStmt = CharacterDatabase.GetPreparedStatement(CharacterDatabaseStatements::CHAR_UPD_MAP_EVENTS);
    l_UpdateStmt->setUInt32(0, StartTime);
    l_UpdateStmt->setUInt32(1, AreaPoiID);
    l_UpdateStmt->setUInt32(2, GroupID);

    CharacterDatabase.Execute(l_UpdateStmt);
}

uint32 AreaPOI::GetAreaPoiID() const
{
    return AreaPoiID;
}

POIMgr::POIMgr()
{
    m_TimeToUpdate = MAP_EVENT_UPDATE_DELAY;
}

AreaPOIEntry const* POIMgr::GetAreaPOIEntry(uint32 p_AreaPoiID) const
{
    return sAreaPOIStore.LookupEntry(p_AreaPoiID);
}

void POIMgr::LoadMapEventsPools()
{
    m_MapEventPools[5006] = { 5175, 5177, 5178, 5210 };                                 ///< Broken Isles Invasions
    m_MapEventPools[6388] = { 5252, 5254, 5255, 5256, 5257, 5258, 5259, 5260, 5261 };   ///< Sentinax
    m_MapEventPools[5639] = { 5284, 5287, 5290, 5293, 5296, 5299, 5302, 5305, 5308};    ///
    m_MapEventPools[5640] = { 5285, 5288, 5291, 5294, 5297, 5300, 5303, 5306 };         ///< Broken shore rares
    m_MapEventPools[5641] = { 5286, 5289, 5292, 5295, 5298, 5301, 5304, 5307 };         ///
    m_MapEventPools[6703] = { 5350, 5359, 5360 };                                       ///< Argus Small Invasions in Krokuun
    m_MapEventPools[6704] = { 5369, 5370, 5372, 5373, 5374 };                           ///< Argus Small Invasions in Antoran
    m_MapEventPools[6705] = { 5366, 5367, 5368 };                                       ///< Argus Small Invasions in Mac
    m_MapEventPools[6706] = { 5375, 5376, 5377, 5379, 5380, 5381 };                     ///< Argus Greater Invasions
}

void POIMgr::LoadMapEvents()
{
    uint32 l_OldMSTime = getMSTime();

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_MAP_EVENTS);
    PreparedQueryResult l_Result = CharacterDatabase.Query(l_Stmt);

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 map_events. DB table `map_events` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        uint8 l_I = 0;
        Field* l_Fields = l_Result->Fetch();
        l_Count++;

        AreaPOI l_MapEvent;

        l_MapEvent.GroupID          = l_Fields[l_I++].GetUInt32();
        l_MapEvent.SetAreaPoiID(      l_Fields[l_I++].GetUInt32());
        l_MapEvent.StartTime        = l_Fields[l_I++].GetUInt32();
        l_MapEvent.Duration         = l_Fields[l_I++].GetUInt32();
        l_MapEvent.RepeatInterval   = l_Fields[l_I++].GetUInt32();

        if (!GetAreaPOIEntry(l_MapEvent.GetAreaPoiID()))
            continue;

        m_EventMap[l_MapEvent.GroupID] = l_MapEvent;

    } while (l_Result->NextRow());

    LoadMapEventsPools();

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u map_events in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

AreaPOIEntry const* POIMgr::GetCurrentPOIEntryForGroup(uint32 p_GroupID) const
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_EventMapLock);

    auto l_Itr = m_EventMap.find(p_GroupID);
    if (l_Itr == m_EventMap.end())
        return nullptr;

    if (!l_Itr->second.Active)
        return nullptr;

    return GetAreaPOIEntry(l_Itr->second.GetAreaPoiID());
}

std::set<uint32> POIMgr::GetActivePOIIds() const
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_ActiveEventsLock);

    std::set<uint32> l_POIIds;
    for (auto const& l_ActivePOI : m_ActiveEvents)
        l_POIIds.insert(l_ActivePOI.GetAreaPoiID());

    return l_POIIds;
}

std::set<uint32> POIMgr::GetAllPOIIds() const
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_EventMapLock);

    std::set<uint32> l_POIIds;
    for (auto const& l_POI : m_EventMap)
        l_POIIds.insert(l_POI.second.GetAreaPoiID());

    return l_POIIds;
}

void POIMgr::Update(uint32 const p_Diff)
{
    m_TimeToUpdate -= p_Diff;
    if (m_TimeToUpdate > 0)
        return;

    m_TimeToUpdate = MAP_EVENT_UPDATE_DELAY;
    uint32 l_Time = sWorld->GetGameTime();

    std::lock_guard<std::recursive_mutex> l_Guard(m_EventMapLock);
    for (std::map<uint32, AreaPOI>::iterator l_Itr = m_EventMap.begin(); l_Itr != m_EventMap.end(); ++l_Itr)
    {
        AreaPOI& l_MapEvent = l_Itr->second;
        if (l_MapEvent.Active && l_MapEvent.StartTime + l_MapEvent.Duration < l_Time)
        {
            l_MapEvent.Active = false;
            UpdateMapEvent(l_Itr->first);
        }
        else if (!l_MapEvent.Active && l_MapEvent.StartTime < l_Time)
        {
            l_MapEvent.Active = true;
            UpdateMapEvent(l_Itr->first);
        }
    }
}

void POIMgr::UpdateMapEvent(uint32 p_GroupID)
{
    AreaPOI& l_MapEvent = m_EventMap[p_GroupID];

    if (l_MapEvent.Active)
    {
        {
            std::lock_guard<std::recursive_mutex> l_Guard(m_ActiveEventsLock);

            m_ActiveEvents.push_back(l_MapEvent);
            for (uint32 l_DependantEventAreaPOI : l_MapEvent.DependantEventAreaPoiID)
            {
                AreaPOI l_MapEventCopy = l_MapEvent;
                l_MapEventCopy.SetAreaPoiID(l_DependantEventAreaPOI);

                m_ActiveEvents.push_back(l_MapEventCopy);
            }
        }

        sScriptMgr->OnPOIAppears(l_MapEvent);

        UpdateForPlayers(l_MapEvent);
    }
    else
    {
        {
            std::lock_guard<std::recursive_mutex> l_Guard(m_ActiveEventsLock);

            m_ActiveEvents.remove_if([p_GroupID](AreaPOI& p_MapEvent) -> bool
            {
                if (p_MapEvent.GroupID == p_GroupID)
                    return true;

                return false;
            });
        }

        l_MapEvent.StartTime += l_MapEvent.Duration + l_MapEvent.RepeatInterval;

        sScriptMgr->OnPOIDisappears(l_MapEvent);

        UpdateForPlayers(l_MapEvent);

        std::set<uint32> l_MapEventPool = m_MapEventPools[p_GroupID];
        if (!l_MapEventPool.empty())
        {
            if (l_MapEventPool.size() > 1)
                l_MapEventPool.erase(l_MapEvent.GetAreaPoiID());

            sScriptMgr->OnNextPOISelect(p_GroupID, l_MapEventPool);

            auto l_Itr = l_MapEventPool.begin();
            std::advance(l_Itr, urand(0, l_MapEventPool.size() - 1));
            l_MapEvent.SetAreaPoiID(*l_Itr);
        }

        l_MapEvent.SaveToDB();
    }
}

void POIMgr::BuildAreaPOIUpdatePacket(WorldPacket& p_Data, uint64 p_PlayerGUID /*= 0*/) const
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_ActiveEventsLock);

    uint32 l_Count = m_ActiveEvents.size();

    bool l_ItrEnd = true;

    m_PlayerEventMapLock.lock();
    auto l_Itr = m_PlayerEventMap.find(p_PlayerGUID);
    if (l_Itr != m_PlayerEventMap.end())
    {
        l_Count += l_Itr->second.size();
        l_ItrEnd = false;
    }

    ByteBuffer l_Buffer;

    if (!l_ItrEnd)
    {
        for (AreaPOI const& l_MapEvent : l_Itr->second)
        {
            l_Buffer << (uint32)l_MapEvent.StartTime;
            l_Buffer << (uint32)l_MapEvent.GetAreaPoiID();
            l_Buffer << (uint32)l_MapEvent.Duration;
            l_Buffer << (uint32)l_MapEvent.WorldStateID;
            l_Buffer << (uint32)l_MapEvent.WorldStateValue;
        }
    }

    m_PlayerEventMapLock.unlock();

    p_Data.Initialize(SMSG_AREA_POI_UPDATE, 4 + 20 * l_Count);

    p_Data << (uint32)l_Count;

    for (AreaPOI const& l_MapEvent : m_ActiveEvents)
    {
        p_Data << (uint32)l_MapEvent.StartTime;
        p_Data << (uint32)l_MapEvent.GetAreaPoiID();
        p_Data << (uint32)l_MapEvent.Duration;
        p_Data << (uint32)l_MapEvent.WorldStateID;
        p_Data << (uint32)l_MapEvent.WorldStateValue;
    }

    if (l_ItrEnd)
        return;

    p_Data.append(l_Buffer);
}

uint32 AreaPOI::GetWorldStateID(std::vector<std::pair<uint32, uint32>>& p_WorldStates) const
{
    AreaPOIEntry const* l_AreaPOIEntry = sAreaPOIStore.LookupEntry(AreaPoiID);
    if (!l_AreaPOIEntry || !l_AreaPOIEntry->PlayerConditionID)
        return 0;

    PlayerConditionEntry const* l_PlayerConditionEntry = sPlayerConditionStore.LookupEntry(l_AreaPOIEntry->PlayerConditionID);
    if (!l_PlayerConditionEntry || !l_PlayerConditionEntry->WorldStateExpressionID)
        return 0;

    WorldStateExpressionEntry const* l_WorldStateExpressionEntry = sWorldStateExpressionStore.LookupEntry(l_PlayerConditionEntry->WorldStateExpressionID);
    if (!l_WorldStateExpressionEntry)
        return 0;

    return l_WorldStateExpressionEntry->GetRequiredWorldStatesIfExists(&p_WorldStates);
}

void POIMgr::AddEventForPlayer(uint32 p_AreaPoiID, uint64 p_PlayerGUID, uint32 p_Duration /*= 0*/, uint32 p_WorldStateValue /*= 1*/)
{
    m_PlayerEventMapLock.lock();

    auto l_Itr = std::find_if(m_PlayerEventMap[p_PlayerGUID].begin(), m_PlayerEventMap[p_PlayerGUID].end(), [p_AreaPoiID](AreaPOI const& p_MapEvent) -> bool
    {
        if (p_MapEvent.GetAreaPoiID() == p_AreaPoiID)
            return true;

        return false;
    });

    if (l_Itr != m_PlayerEventMap[p_PlayerGUID].end())
    {
        m_PlayerEventMapLock.unlock();
        return;
    }

    AreaPOI l_MapEvent;
    l_MapEvent.SetAreaPoiID(p_AreaPoiID);
    l_MapEvent.StartTime = sWorld->GetGameTime();
    l_MapEvent.Duration = p_Duration ? p_Duration : -1;
    l_MapEvent.WorldStateValue = p_WorldStateValue;
    l_MapEvent.Active = true;

    m_PlayerEventMap[p_PlayerGUID].push_back(l_MapEvent);
    m_PlayerEventMapLock.unlock();

    UpdateForPlayer(l_MapEvent, p_PlayerGUID);
}

void POIMgr::EndEventForPlayer(uint32 p_AreaPoiID, uint64 p_PlayerGUID)
{
    m_PlayerEventMapLock.lock();
    m_PlayerEventMap[p_PlayerGUID].remove_if([p_AreaPoiID, p_PlayerGUID](AreaPOI& p_MapEvent) -> bool
    {
        if (p_MapEvent.GetAreaPoiID() == p_AreaPoiID)
        {
            p_MapEvent.Active = false;
            sPOIMgr->UpdateForPlayer(p_MapEvent, p_PlayerGUID);
            return true;
        }

        return false;
    });
    m_PlayerEventMapLock.unlock();
}

void POIMgr::UpdateForPlayer(AreaPOI const& p_MapEvent, uint64 p_PlayerGUID)
{
    if (!p_PlayerGUID)
        return;

    Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_PlayerGUID);
    if (!l_Player)
    {
        m_PlayerEventMapLock.lock();
        m_PlayerEventMap[p_PlayerGUID].clear();
        m_PlayerEventMapLock.unlock();
        return;
    }

    SendUpdateWorldStates(l_Player, &p_MapEvent);
    SendAreaPoiUpdate(l_Player);
}

void POIMgr::UpdateForPlayers(AreaPOI const& p_MapEvent) const
{
    std::list<AreaPOI> l_UpdatePOI;
    for (uint32 l_DependantEventAreaPOI : p_MapEvent.DependantEventAreaPoiID)
    {
        AreaPOI l_MapEventCopy = p_MapEvent;
        l_MapEventCopy.SetAreaPoiID(l_DependantEventAreaPOI);

        l_UpdatePOI.push_back(l_MapEventCopy);
    }

    l_UpdatePOI.push_back(p_MapEvent);

    for (AreaPOI l_POI : l_UpdatePOI)
        sWorld->setWorldState(l_POI.WorldStateID, l_POI.Active ? 1 : 0);

    sWorld->GetSessionsLock().acquire_read();
    SessionMap const& l_Sessions = sWorld->GetAllSessions();
    for (SessionMap::const_iterator itr = l_Sessions.begin(); itr != l_Sessions.end(); ++itr)
    {
        if (Player* l_Player = itr->second->GetPlayer())
        {
            for (AreaPOI l_POI : l_UpdatePOI)
            {
                SendUpdateWorldStates(l_Player, &l_POI);
                SendAreaPoiUpdate(l_Player);
            }
        }
    }
    sWorld->GetSessionsLock().release();
}

void POIMgr::InitializeForPlayer(Player* p_Player)
{
    {
        std::lock_guard<std::recursive_mutex> l_Guard(m_ActiveEventsLock);
        for (auto l_Itr = m_ActiveEvents.begin(); l_Itr != m_ActiveEvents.end(); ++l_Itr)
            SendUpdateWorldStates(p_Player, &*l_Itr);
    }

    m_PlayerEventMapLock.lock();
    for (auto l_Itr = m_PlayerEventMap[p_Player->GetGUID()].begin(); l_Itr != m_PlayerEventMap[p_Player->GetGUID()].end(); ++l_Itr)
        SendUpdateWorldStates(p_Player, &*l_Itr);
    m_PlayerEventMapLock.unlock();

    SendAreaPoiUpdate(p_Player);
}

void POIMgr::SendAreaPoiUpdate(Player* p_Player) const
{
    WorldPacket l_Data;
    BuildAreaPOIUpdatePacket(l_Data, p_Player->GetGUID());

    p_Player->GetSession()->SendPacket(&l_Data);
}

void POIMgr::SendUpdateWorldStates(Player* p_Player, AreaPOI const* p_MapEvent) const
{
    for (std::pair<uint32, uint32> l_WorldState : p_MapEvent->WorldStates)
        p_Player->SendUpdateWorldState(l_WorldState.first, p_MapEvent->Active ? l_WorldState.second : 0);
}
