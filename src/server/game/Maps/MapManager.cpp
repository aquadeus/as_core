////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "MapManager.h"
#include "InstanceSaveMgr.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Transport.h"
#include "GridDefines.h"
#include "MapInstanced.h"
#include "InstanceScript.h"
#include "Config.h"
#include "World.h"
#include "CellImpl.h"
#include "Corpse.h"
#include "ObjectMgr.h"
#include "Language.h"
#include "WorldPacket.h"
#include "Group.h"
#include "Common.h"
#include "POIMgr.hpp"
#include "OutdoorPvPMgr.h"

extern GridState* si_GridStates[];                          // debugging code, should be deleted some day

MapManager::MapManager()
{
    i_gridCleanUpDelay = sWorld->getIntConfig(CONFIG_INTERVAL_GRIDCLEAN);
    i_timer.SetInterval(sWorld->getIntConfig(CONFIG_INTERVAL_MAPUPDATE));
    m_LogMapPerformance = false;
}

MapManager::~MapManager()
{
}

void MapManager::Initialize()
{
    Map::InitStateMachine();

    // debugging code, should be deleted some day
    {
        for (uint8 i = 0; i < MAX_GRID_STATE; ++i)
             i_GridStates[i] = si_GridStates[i];

        i_GridStateErrorCount = 0;
    }

    int32 l_ThreadsCount = sWorld->getIntConfig(CONFIG_NUMTHREADS);
    if (l_ThreadsCount < 1)
        l_ThreadsCount = 1;

    m_MapsUpdater.activate(l_ThreadsCount);
}

void MapManager::InitializeVisibilityDistanceInfo()
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_MapsLock);
    for (MapMapType::iterator iter=i_maps.begin(); iter != i_maps.end(); ++iter)
        (*iter).second->InitVisibilityDistance();
}

// debugging code, should be deleted some day
void MapManager::checkAndCorrectGridStatesArray()
{
    bool ok = true;
    for (int i=0; i<MAX_GRID_STATE; i++)
    {
        if (i_GridStates[i] != si_GridStates[i])
        {
            sLog->outError(LOG_FILTER_MAPS, "MapManager::checkGridStates(), GridState: si_GridStates is currupt !!!");
            ok = false;
            si_GridStates[i] = i_GridStates[i];
        }
        #ifdef TRINITY_DEBUG
        // inner class checking only when compiled with debug
        if (!si_GridStates[i]->checkMagic())
        {
            ok = false;
            si_GridStates[i]->setMagic();
        }
        #endif
    }
    if (!ok)
        ++i_GridStateErrorCount;
}

Map* MapManager::CreateBaseMap(uint32 id, uint32 zoneId)
{
    MapEntry const* entry = sMapStore.LookupEntry(id);
    ASSERT(entry);

    /// Instanced map aren't zone-instanced
    if (entry->Instanceable())
        zoneId = 0;

    /// Dalaran Sewers - Can't be splitted but need a valid and fix zoneid to retreive outdoor pvp instance
    if (id == 1502 && zoneId != 0)
        zoneId = 8392;

    /// Ashran - Can't be splitted but need a valid and fix zoneid to retreive outdoor pvp instance
    if (id == 1191 && zoneId != 0)
        zoneId = 6941;

    Map* map = FindBaseMap(id, zoneId);

    if (map == nullptr)
    {
        if (entry->Instanceable())
            map = new MapInstanced(id, i_gridCleanUpDelay);
        else
        {
            Map* l_ParentMap = nullptr;
            if (zoneId)
                l_ParentMap = CreateBaseMap(id, 0);

            map = new Map(id, i_gridCleanUpDelay, 0, DifficultyNone, zoneId, l_ParentMap);
            map->LoadRespawnTimes();
        }

        sScriptMgr->OnCreateBaseMap(map, zoneId);

        m_MapsLock.lock();
        i_maps[std::make_pair(id, zoneId)] = map;
        m_ZonesMap[id << 16  | (zoneId & 0xFFFF)].push_back(map);
        m_MapsLock.unlock();
    }

    ASSERT(map);

    return map;
}

Map* MapManager::FindBaseNonInstanceMap(uint32 mapId, uint32 zoneId) const
{
    Map* map = FindBaseMap(mapId, zoneId);
    if (map && map->Instanceable())
        return nullptr;
    return map;
}

Map* MapManager::CreateMap(uint32 id, uint32 zoneId, Player* player, CustomInstanceZone const* p_CustomInstanceZone /*=nullptr*/)
{
    if (p_CustomInstanceZone)
        zoneId = p_CustomInstanceZone->CustomZoneID;

    /// Try to find zone instance with group first
    if (GroupPtr l_Group = player->GetGroup())
    {
        Map* m = nullptr;

        l_Group->GetMemberSlots().foreach_until([&](Group::MemberSlotPtr l_Itr) -> bool
        {
            Player* l_Member = l_Itr->player;
            if (!l_Member || !l_Member->GetMap() || (l_Member->GetMap()->GetInstanceZoneId() & 0xFFFF) != zoneId || l_Member == player)
                return false;

            zoneId = l_Member->GetMap()->GetInstanceZoneId();

            Map* map = CreateBaseMap(id, zoneId);

            if (map && map->Instanceable())
                map = ((MapInstanced*)map)->CreateInstanceForPlayer(id, player);

            m = map;

            return true;
        });

        if (m)
            return m;
    }

    /// GM with command .appear playername bypass the max player per zone check and appear in the same instance.
    if (uint32 l_AppearZoneInstanceId = player->GetAppearZoneInstanceId())
    {
        Map* m = CreateBaseMap(id, l_AppearZoneInstanceId);

        if (m && m->Instanceable())
            m = ((MapInstanced*)m)->CreateInstanceForPlayer(id, player);

        player->SetAppearZoneInstanceId(0);

        return m;
    }

    uint32 l_MaxZonePlayerCount = p_CustomInstanceZone ? p_CustomInstanceZone->MaxPlayerCount : sWorld->getIntConfig(CONFIG_MAP_PLAYERS_PER_ZONE_INSTANCE);

    /// Special case
    switch (zoneId)
    {
        case 7543:  ///< Broken Shore, prevent high delay when there is too much people in the zone
        case 8701:  ///< Argus zone 'Mac'Aree'
        case 8574:  ///< Argus zone 'Krokuun'
        case 8899:  ///< Argus zone 'Antoran Waste'
            l_MaxZonePlayerCount = 50;
            break;
        /// Argus Invasions Points
        case 9024:
        case 9025:
        case 9100:
        case 9102:
        case 9106:
        case 9126:
        case 9127:
        case 9128:
        case 9129:
        case 9180:
            l_MaxZonePlayerCount = 10;
            break;
        /// Argus Greater Invasions Points
        case 9295:
        case 9296:
        case 9297:
        case 9298:
        case 9299:
        case 9300:
            l_MaxZonePlayerCount = 40;
            break;
        default:
            break;
    }

    for (uint32 l_ZoneInstanceId = 0; l_ZoneInstanceId < 0xFFFFFFFF; l_ZoneInstanceId++)
    {
        Map* l_Map = CreateBaseMap(id, l_ZoneInstanceId << 16 | zoneId);
        if (!l_Map)
            return l_Map;

        if (l_Map && (l_Map->Instanceable()))
        {
            l_Map = ((MapInstanced*)l_Map)->CreateInstanceForPlayer(id, player);
            return l_Map;
        }

        if (id == 1191 || id == 1502)
            return l_Map;

        ///  Orgrimmar & Stormwind
        if ((zoneId == 1617 || zoneId == 1519) && !sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
            return l_Map;

        /// We can't instanciate outdoor pvp zones
        if (l_Map->GetOutdoorPvP())
            return l_Map;

        if (l_Map->IsClosed() || l_Map->GetPlayerCount() > l_MaxZonePlayerCount)
        {
            l_ZoneInstanceId++;
            continue;
        }

        return l_Map;
    }

    return nullptr;
}

Map* MapManager::FindMap(uint32 mapid, uint32 instanceId) const
{
    Map* map = FindBaseMap(mapid, 0);
    if (!map)
        return nullptr;

    if (!map->Instanceable())
        return instanceId == 0 ? map : NULL;

    return ((MapInstanced*)map)->FindInstanceMap(instanceId);
}

bool MapManager::CanPlayerEnter(uint32 mapid, Player* player, bool loginCheck)
{
    MapEntry const* entry = sMapStore.LookupEntry(mapid);
    if (!entry)
       return false;

    // Molten Front
    // That's not an instance, so check it here
    if (entry->MapID == 861)
        if (player->getLevel() < 85)
            return false;

    if (!entry->IsDungeon())
        return true;

    InstanceTemplate const* instance = sObjectMgr->GetInstanceTemplate(mapid);
    if (!instance)
        return false;

    Difficulty targetDifficulty = player->GetDifficultyID(entry);

#ifndef CROSS
    if (entry->MapID == player->GetGarrisonMapID() || entry->MapID == player->GetShipyardMapID())
        targetDifficulty = Difficulty::DifficultyNormal;
#endif

    //The player has a heroic mode and tries to enter into instance which has no a heroic mode
    MapDifficultyEntry const* mapDiff = GetMapDifficultyData(entry->MapID, targetDifficulty);
    if (!mapDiff)
    {
        // Send aborted message for dungeons
        if (entry->IsNonRaidDungeon())
        {
            player->SendTransferAborted(mapid, TRANSFER_ABORT_DIFFICULTY, player->GetDungeonDifficultyID());
            return false;
        }
        else    // attempt to downscale
            mapDiff = GetDownscaledMapDifficultyData(entry->MapID, targetDifficulty); ///< mapDiff is never read 01/18/16
    }

    //Bypass checks for GMs
    if (player->isGameMaster())
        return true;

    char const* mapName = entry->MapNameLang->Get(sWorld->GetDefaultDb2Locale());

    if (!player->isAlive())
    {
        if (player->HasCorpse())
        {
            // let enter in ghost mode in instance that connected to inner instance with corpse
            uint32 corpseMap = player->GetCorpseLocation().GetMapId();
            do
            {
                if (corpseMap == mapid)
                    break;

                InstanceTemplate const* corpseInstance = sObjectMgr->GetInstanceTemplate(corpseMap);
                corpseMap = corpseInstance ? corpseInstance->Parent : 0;
            }
            while (corpseMap);

            if (!corpseMap)
            {
                WorldPacket l_Data(SMSG_AREA_TRIGGER_NO_CORPSE);
                player->GetSession()->SendPacket(&l_Data);
                sLog->outDebug(LOG_FILTER_MAPS, "MAP: Player '%s' does not have a corpse in instance '%s' and cannot enter.", player->GetName(), mapName);
                return false;
            }

            sLog->outDebug(LOG_FILTER_MAPS, "MAP: Player '%s' has corpse in instance '%s' and can enter.", player->GetName(), mapName);
        }
        else
            sLog->outDebug(LOG_FILTER_MAPS, "Map::CanPlayerEnter - player '%s' is dead but does not have a corpse!", player->GetName());
    }

    GroupPtr group = player->GetGroup();
    if (entry->IsRaid() && entry->Expansion() >= 6)
    {
        /// can only enter in a raid group except for raids before Legion
        if ((!group || !group->isRaidGroup()) && !sWorld->getBoolConfig(CONFIG_INSTANCE_IGNORE_RAID))
        {
            sLog->outDebug(LOG_FILTER_MAPS, "MAP: Player '%s' must be in a raid group to enter instance '%s'", player->GetName(), mapName);
            return false;
        }
    }

    //Get instance where player's group is bound & its map
    if (group)
    {
        InstanceGroupBindPtr boundInstance = group->GetBoundInstance(entry);
        if (boundInstance && boundInstance->save)
            if (Map* boundMap = sMapMgr->FindMap(mapid, boundInstance->save->GetInstanceId()))
                if (!loginCheck && !boundMap->CanEnter(player))
                    return false;
    }

    // players are only allowed to enter 5 instances per hour
    if (entry->IsDungeon() && (!player->GetGroup() || (player->GetGroup() && !player->GetGroup()->isLFGGroup())))
    {
        uint32 instaceIdToCheck = 0;
        if (InstanceSavePtr save = player->GetInstanceSave(mapid))
            instaceIdToCheck = save->GetInstanceId();

        // instanceId can never be 0 - will not be found
        if (!player->CheckInstanceCount(instaceIdToCheck) && !player->isDead())
        {
            player->SendTransferAborted(mapid, TRANSFER_ABORT_TOO_MANY_INSTANCES);
            return false;
        }
    }

    /// Check if guild is allowed to enter, even if the instance is locked
    if (player->Satisfy(sObjectMgr->GetAccessRequirement(mapid, targetDifficulty), mapid, true) == false)
    {
        if (sObjectMgr->IsGuildWhitelisted(mapid, player->GetGuildId()))
            return true;
        else
            return false;
    }

    return true;
}

void MapManager::AddTaskToAllMaps(std::function<void()> p_Task)
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_MapsLock);

    for (MapMapType::iterator l_Itr = i_maps.begin(); l_Itr != i_maps.end(); ++l_Itr)
    {
        Map* l_Map = l_Itr->second;

        l_Map->AddTask(p_Task);

        if (!l_Map->ToMapInstanced())
            continue;

        MapInstanced::InstancedMaps& l_Maps = ((MapInstanced*)l_Map)->GetInstancedMaps();
        ((MapInstanced*)l_Map)->GetInstancedMapsLock().lock();
        for (MapInstanced::InstancedMaps::iterator l_MItr = l_Maps.begin(); l_MItr != l_Maps.end(); ++l_MItr)
            l_MItr->second->AddTask(p_Task);
        ((MapInstanced*)l_Map)->GetInstancedMapsLock().unlock();
    }
}

void MapManager::AddTaskToAllMapsWithId(uint32 p_MapId, std::function<void()> p_Task)
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_MapsLock);

    for (MapMapType::iterator l_Itr = i_maps.begin(); l_Itr != i_maps.end(); ++l_Itr)
    {
        Map* l_Map = l_Itr->second;
        if (l_Map->GetId() != p_MapId)
            continue;

        if (!l_Map->ToMapInstanced())
        {
            l_Map->AddTask(p_Task);
            continue;
        }

        MapInstanced::InstancedMaps& l_Maps = ((MapInstanced*)l_Map)->GetInstancedMaps();
        ((MapInstanced*)l_Map)->GetInstancedMapsLock().lock();
        for (MapInstanced::InstancedMaps::iterator l_MItr = l_Maps.begin(); l_MItr != l_Maps.end(); ++l_MItr)
            l_MItr->second->AddTask(p_Task);
        ((MapInstanced*)l_Map)->GetInstancedMapsLock().unlock();
    }

}

void MapManager::UpdateLoop()
{
#ifndef CROSS
    uint32 l_CoreStuckTime = ConfigMgr::GetIntDefault("MaxCoreStuckTime", 0) * IN_MILLISECONDS;

    while (!sWorld->IsStopped())
    {
        if (!m_MapsUpdater.activated())
        {
            ACE_Based::Thread::Sleep(1);
            continue;
        }

        uint32 l_Now = getMSTime();

        m_MapsLock.lock();
        MapMapType::iterator l_Iter = i_maps.begin();
        for (; l_Iter != i_maps.end(); ++l_Iter)
        {
            Map* l_MasterMap = l_Iter->second;

            if (l_MasterMap->CanUpdate())
            {
                l_MasterMap->SetUpdating(true);
                uint32 l_Diff = getMSTimeDiff(l_MasterMap->GetLastUpdateTime(), l_Now);
                l_MasterMap->SetLastUpdateTime(l_Now);

                m_MapsUpdater.schedule_update(*l_MasterMap, l_Diff);
            }
            /// The map freeze
            else if (l_CoreStuckTime && GetMSTimeDiffToNow(l_MasterMap->GetLastUpdateTime()) > l_CoreStuckTime)
            {
                auto l_Creatures = l_MasterMap->m_CreatureEntryCount;

                std::string l_CreaturesCountDump;

                for (auto l_Itr : l_Creatures)
                {
                    if (l_Itr.second < 5)
                        continue;

                    l_CreaturesCountDump += "m_CreatureEntryCount : {entry:" + std::to_string(l_Itr.first) + ", count:" + std::to_string(l_Itr.second) + "} \n";
                }

                sLog->outExtChat("#jarvis", "danger", true, "Map %u (id %u last creature updated %u x : %f y : %f z : %f) freeze",
                    l_MasterMap->m_CurrentThreadId, l_MasterMap->GetId(), l_MasterMap->m_LastCreatureUpdatedEntry,
                    l_MasterMap->m_LastCreatureUpdatedX, l_MasterMap->m_LastCreatureUpdatedY, l_MasterMap->m_LastCreatureUpdatedZ);

                sLog->outExtChat("#jarvis", "danger", true, "%s", l_CreaturesCountDump.c_str());

                sLog->outAshran("Map %u (id %u last creature updated %u x : %f y : %f z : %f) freeze",
                    l_MasterMap->m_CurrentThreadId, l_MasterMap->GetId(), l_MasterMap->m_LastCreatureUpdatedEntry,
                    l_MasterMap->m_LastCreatureUpdatedX, l_MasterMap->m_LastCreatureUpdatedY, l_MasterMap->m_LastCreatureUpdatedZ);

                sLog->outAshran("%s", l_CreaturesCountDump.c_str());

                ACE_Based::Thread::Sleep(10000);

                abort();
            }

            MapInstanced* l_MapInstanced = l_MasterMap->ToMapInstanced();
            if (!l_MapInstanced)
                continue;

            /// Instanced Map, schedule a update to all the instances
            l_MapInstanced->GetInstancedMapsLock().lock();
            auto l_Itr = l_MapInstanced->GetInstancedMaps().begin();
            while (l_Itr != l_MapInstanced->GetInstancedMaps().end())
            {
                Map* l_Map = l_Itr->second;

                if (l_Map->CanUpdate())
                {
                    uint32 l_Diff = getMSTimeDiff(l_Map->GetLastUpdateTime(), l_Now);
                    l_Map->SetLastUpdateTime(l_Now);

                    if (l_Map->CanUnload(l_Diff) && l_MapInstanced->DestroyInstance(l_Itr))
                        continue;

                    l_Map->SetUpdating(true);
                    m_MapsUpdater.schedule_update(*l_Map, l_Diff);
                }
                else if (l_CoreStuckTime && GetMSTimeDiffToNow(l_Map->GetLastUpdateTime()) > l_CoreStuckTime)
                {
                    auto l_Creatures = l_Map->m_CreatureEntryCount;

                    std::string l_CreaturesCountDump;

                    for (auto l_Itr : l_Creatures)
                    {
                        if (l_Itr.second < 5)
                            continue;

                        l_CreaturesCountDump += "m_CreatureEntryCount : {entry:" + std::to_string(l_Itr.first) + ", count:" + std::to_string(l_Itr.second) + "} \n";
                    }

                    sLog->outExtChat("#jarvis", "danger", true, "Map %u (id %u last creature updated %u x : %f y : %f z : %f) freeze",
                        l_Map->m_CurrentThreadId, l_Map->GetId(), l_Map->m_LastCreatureUpdatedEntry,
                        l_Map->m_LastCreatureUpdatedX, l_Map->m_LastCreatureUpdatedY, l_Map->m_LastCreatureUpdatedZ);

                    sLog->outExtChat("#jarvis", "danger", true, "%s", l_CreaturesCountDump.c_str());

                    sLog->outAshran("Map %u (id %u last creature updated %u x : %f y : %f z : %f) freeze",
                        l_Map->m_CurrentThreadId, l_Map->GetId(), l_Map->m_LastCreatureUpdatedEntry,
                        l_Map->m_LastCreatureUpdatedX, l_Map->m_LastCreatureUpdatedY, l_Map->m_LastCreatureUpdatedZ);

                    sLog->outAshran("%s", l_CreaturesCountDump.c_str());

                    ACE_Based::Thread::Sleep(10000);

                    abort();
                }

                l_Itr++;
            }
            l_MapInstanced->GetInstancedMapsLock().unlock();
        }
        m_MapsLock.unlock();

        ACE_Based::Thread::Sleep(1);
    }
#else
    uint32 l_Now = getMSTime();

    m_MapsLock.lock();
    MapMapType::iterator l_Iter = i_maps.begin();
    for (; l_Iter != i_maps.end(); ++l_Iter)
    {
        Map* l_MasterMap = l_Iter->second;

        if (l_MasterMap->CanUpdate())
        {
            l_MasterMap->SetUpdating(true);
            uint32 l_Diff = getMSTimeDiff(l_MasterMap->GetLastUpdateTime(), l_Now);
            l_MasterMap->SetLastUpdateTime(l_Now);

            m_MapsUpdater.schedule_update(*l_MasterMap, l_Diff);
        }

        MapInstanced* l_MapInstanced = l_MasterMap->ToMapInstanced();
        if (!l_MapInstanced)
            continue;

        /// Instanced Map, schedule a update to all the instances
        l_MapInstanced->GetInstancedMapsLock().lock();
        auto l_Itr = l_MapInstanced->GetInstancedMaps().begin();
        while (l_Itr != l_MapInstanced->GetInstancedMaps().end())
        {
            Map* l_Map = l_Itr->second;

            if (l_Map->CanUpdate())
            {
                uint32 l_Diff = getMSTimeDiff(l_Map->GetLastUpdateTime(), l_Now);
                l_Map->SetLastUpdateTime(l_Now);

                if (l_Map->CanUnload(l_Diff) && l_MapInstanced->DestroyInstance(l_Itr))
                    continue;

                l_Map->SetUpdating(true);
                m_MapsUpdater.schedule_update(*l_Map, l_Diff);
            }

            l_Itr++;
        }
        l_MapInstanced->GetInstancedMapsLock().unlock();
    }
    m_MapsLock.unlock();

    m_MapsUpdater.wait();
#endif
}

void MapManager::DoDelayedMovesAndRemoves()
{
}

bool MapManager::ExistMapAndVMap(uint32 mapid, float x, float y)
{
    GridCoord p = JadeCore::ComputeGridCoord(x, y);

    int gx=63-p.x_coord;
    int gy=63-p.y_coord;

    return Map::ExistMap(mapid, gx, gy) && Map::ExistVMap(mapid, gx, gy);
}

bool MapManager::IsValidMAP(uint32 mapid, bool startUp)
{
    MapEntry const* mEntry = sMapStore.LookupEntry(mapid);

    if (startUp)
        return mEntry ? true : false;
    else
        return mEntry && (!mEntry->IsDungeon() || sObjectMgr->GetInstanceTemplate(mapid));

    // TODO: add check for battleground template
}

void MapManager::UnloadAll()
{
    m_MapsLock.lock();
    for (MapMapType::iterator iter = i_maps.begin(); iter != i_maps.end();)
    {
        ///iter->second->UnloadAll();
        delete iter->second;
        i_maps.erase(iter++);
    }
    m_MapsLock.unlock();

    if (m_MapsUpdater.activated())
        m_MapsUpdater.deactivate();

    Map::DeleteStateMachine();
}

uint32 MapManager::GetNumInstances()
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_MapsLock);

    uint32 ret = 0;
    for (MapMapType::iterator itr = i_maps.begin(); itr != i_maps.end(); ++itr)
    {
        Map* map = itr->second;
        if (!map->Instanceable())
            continue;

        MapInstanced::InstancedMaps &maps = ((MapInstanced*)map)->GetInstancedMaps();
        ((MapInstanced*)map)->GetInstancedMapsLock().lock();
        for (MapInstanced::InstancedMaps::iterator mitr = maps.begin(); mitr != maps.end(); ++mitr)
            if (mitr->second->IsDungeon()) ret++;
        ((MapInstanced*)map)->GetInstancedMapsLock().unlock();
    }
    return ret;
}

uint32 MapManager::GetNumPlayersInInstances()
{
    std::lock_guard<std::recursive_mutex> l_Guard(m_MapsLock);

    uint32 ret = 0;
    for (MapMapType::iterator itr = i_maps.begin(); itr != i_maps.end(); ++itr)
    {
        Map* map = itr->second;
        if (!map->Instanceable())
            continue;
        MapInstanced::InstancedMaps &maps = ((MapInstanced*)map)->GetInstancedMaps();
        ((MapInstanced*)map)->GetInstancedMapsLock().lock();
        for (MapInstanced::InstancedMaps::iterator mitr = maps.begin(); mitr != maps.end(); ++mitr)
            if (mitr->second->IsDungeon())
                ret += ((InstanceMap*)mitr->second)->GetPlayers().getSize();
        ((MapInstanced*)map)->GetInstancedMapsLock().unlock();
    }
    return ret;
}

void MapManager::InitInstanceIds()
{
    m_NextInstanceID = 1;
}

void MapManager::RegisterInstanceId(uint32 p_InstanceID)
{
    /// Allocation and sizing was done in InitInstanceIds()
    m_InstanceIDs.insert(p_InstanceID);
}

uint32 MapManager::GenerateInstanceId()
{
    uint32 l_NewInstanceID = m_NextInstanceID;

    /// Find the lowest available id starting from the current NextInstanceId (which should be the lowest according to the logic in FreeInstanceId()
    for (uint32 l_I = ++m_NextInstanceID; l_I < 0xFFFFFFFF; ++l_I)
    {
        if (m_InstanceIDs.find(l_I) != m_InstanceIDs.end())
            continue;

        m_NextInstanceID = l_I;
        break;
    }

    if (l_NewInstanceID == m_NextInstanceID)
    {
        sLog->outError(LogFilterType::LOG_FILTER_MAPS, "Instance ID overflow!! Can't continue, shutting down server. ");
        World::StopNow(ShutdownExitCode::ERROR_EXIT_CODE);
    }

    m_InstanceIDs.insert(l_NewInstanceID);

    return l_NewInstanceID;
}

void MapManager::FreeInstanceId(uint32 p_InstanceID)
{
    /// If freed instance id is lower than the next id available for new instances, use the freed one instead
    if (p_InstanceID < m_NextInstanceID)
        SetNextInstanceId(p_InstanceID);

    m_InstanceIDs.erase(p_InstanceID);
}

void MapsUpdaterRunnable::run()
{
    sMapMgr->UpdateLoop();
}
