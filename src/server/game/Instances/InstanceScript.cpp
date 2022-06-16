////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "InstanceScript.h"
#include "DatabaseEnv.h"
#include "Map.h"
#include "Player.h"
#include "Group.h"
#include "GameObject.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Log.h"
#include "LFGMgr.h"
#ifndef CROSS
#include "Guild.h"
#endif /* not CROSS */
#include "WowTime.hpp"
#include "ScenarioMgr.h"
#include "WaypointManager.h"
#include "MoveSplineInit.h"

InstanceScript::InstanceScript(Map* p_Map)
{
    instance                    = p_Map;
    m_CompletedEncounters       = 0;

    m_EncounterTime             = 0;
    m_DisabledMask              = 0;

    m_InCombatResCount          = 0;
    m_MaxInCombatResCount       = 0;
    m_CombatResChargeTime       = 0;
    m_NextCombatResChargeTime   = 0;

    scenarioStep                = 0;

    m_EncounterID               = 0;

    m_EncounterDatas            = EncounterDatas();

    m_InitDamageManager         = false;

    m_ChallengeOrbGuid      = 0;
    m_ChallengeChestFirst   = 0;
    m_ChallengeChestSecond  = 0;
    m_ChallengeChestThird   = 0;

    m_ChallengeChestGuids.assign(3, 0);
    m_ChallengeDoorGuids.clear();

    m_Challenge = nullptr;

    m_ReleaseAllowed = false;
}

void InstanceScript::SaveToDB()
{
    std::string data = GetSaveData();
    if (data.empty())
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_INSTANCE_DATA);
    stmt->setUInt32(0, GetCompletedEncounterMask());
    stmt->setString(1, data);
    stmt->setUInt32(2, instance->GetInstanceId());
    CharacterDatabase.Execute(stmt);
}

void InstanceScript::HandleGameObject(uint64 GUID, bool open, GameObject* go)
{
    if (!go)
        go = instance->GetGameObject(GUID);
    if (go)
        go->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
    else
        sLog->outDebug(LOG_FILTER_TSCR, "InstanceScript: HandleGameObject failed");
}

void InstanceScript::Update(uint32 p_Diff)
{
    UpdateOperations(p_Diff);
    UpdateCombatResurrection(p_Diff);

    m_Functions.Update(p_Diff);

    if (m_Challenge)
        m_Challenge->Update(p_Diff);
}

void InstanceScript::UpdateOperations(uint32 const p_Diff)
{
    std::vector<std::function<void()>> l_OperationsReady;

    for (auto l_Iter = m_TimedDelayedOperations.begin(); l_Iter != m_TimedDelayedOperations.end();)
    {
        auto& l_Pair = *l_Iter;

        l_Pair.first -= p_Diff;

        if (l_Pair.first < 0)
        {
            l_OperationsReady.push_back(l_Pair.second);

            l_Iter = m_TimedDelayedOperations.erase(l_Iter);
        }
        else
            ++l_Iter;
    }

    for (auto l_Operation : l_OperationsReady)
        l_Operation();

    if (m_TimedDelayedOperations.empty() && !m_EmptyWarned)
    {
        m_EmptyWarned = true;
        LastOperationCalled();
    }
}

bool InstanceScript::IsEncounterInProgress(int32 p_ExcludeBossID  /*= -1*/) const
{
    uint8 l_BossID = 0;
    for (std::vector<BossInfo>::const_iterator itr = m_Bosses.begin(); itr != m_Bosses.end(); ++itr)
    {
        if ((p_ExcludeBossID == -1 || p_ExcludeBossID != l_BossID) && itr->state == IN_PROGRESS)
            return true;

        ++l_BossID;
    }

    return false;
}

uint32 InstanceScript::CountDefeatedEncounters() const
{
    uint32 l_Count = 0;

    for (std::vector<BossInfo>::const_iterator l_Iter = m_Bosses.begin(); l_Iter != m_Bosses.end(); ++l_Iter)
    {
        if (l_Iter->state == EncounterState::DONE)
            ++l_Count;
    }

    return l_Count;
}

void InstanceScript::OnPlayerEnter(Player* p_Player)
{
    /// Enable cheat god for testing purposes
    /*if (sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID()))
    {
        if (sObjectMgr->GetItemContextOverride(instance->GetId(), instance->GetDifficultyID()) == ItemContext::None)
            p_Player->CastSpell(p_Player, eInstanceSpells::SpellPvEEventsDebug, true);
    }*/
}

void InstanceScript::OnPlayerExit(Player* p_Player)
{
    p_Player->RemoveAura(eInstanceSpells::SpellDetermination);

    /// Remove cheat god
    p_Player->RemoveAura(eInstanceSpells::SpellPvEEventsDebug);
}

void InstanceScript::LoadMinionData(const MinionData* data)
{
    while (data->entry)
    {
        if (data->bossId < m_Bosses.size())
            minions.insert(std::make_pair(data->entry, MinionInfo(&m_Bosses[data->bossId])));

        ++data;
    }
    sLog->outDebug(LOG_FILTER_TSCR, "InstanceScript::LoadMinionData: " UI64FMTD " minions loaded.", uint64(minions.size()));
}

void InstanceScript::SetBossNumber(uint32 p_Number)
{
    m_Bosses.resize(p_Number);
}

void InstanceScript::LoadDoorData(const DoorData* data)
{
    while (data->entry)
    {
        if (data->bossId < m_Bosses.size())
            doors.insert(std::make_pair(data->entry, DoorInfo(&m_Bosses[data->bossId], data->type, BoundaryType(data->boundary))));

        ++data;
    }
    sLog->outDebug(LOG_FILTER_TSCR, "InstanceScript::LoadDoorData: " UI64FMTD " doors loaded.", uint64(doors.size()));
}

void InstanceScript::UpdateMinionState(Creature* minion, EncounterState state)
{
    switch (state)
    {
        case NOT_STARTED:
            if (!minion->isAlive())
                minion->Respawn();
            else if (minion->isInCombat())
                minion->AI()->EnterEvadeMode();
            break;
        case IN_PROGRESS:
            if (!minion->isAlive())
                minion->Respawn();
            else if (!minion->getVictim())
                minion->AI()->DoZoneInCombat();
            break;
        default:
            break;
    }
}

void InstanceScript::UpdateDoorState(GameObject* door, bool p_WithDelay /*= false*/)
{
    if (!door)
        return;

    DoorInfoMap::iterator lower = doors.lower_bound(door->GetEntry());
    DoorInfoMap::iterator upper = doors.upper_bound(door->GetEntry());
    if (lower == upper)
        return;

    bool open = true;
    for (DoorInfoMap::iterator itr = lower; itr != upper && open; ++itr)
    {
        switch (itr->second.type)
        {
            case DOOR_TYPE_ROOM:
                open = (itr->second.bossInfo->state != IN_PROGRESS);
                break;
            case DOOR_TYPE_PASSAGE:
                open = (itr->second.bossInfo->state == DONE);
                break;
            case DOOR_TYPE_SPAWN_HOLE:
                open = (itr->second.bossInfo->state == IN_PROGRESS);
                break;
            case DOOR_TYPE_RANDOM:
                open = false; ///< False because it has to be handled in the OnGameObjectCreate hook in the instance script.
                break;
            default:
                break;
        }
    }

    /// Delay Door closing, like retail
    if (!open)
    {
        if (!p_WithDelay)
        {
            door->SetGoState(GOState::GO_STATE_READY);
            return;
        }

        uint64 l_DoorGuid = door->GetGUID();
        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this, l_DoorGuid]() -> void
        {
            if (GameObject* l_Door = sObjectAccessor->FindGameObject(l_DoorGuid))
                UpdateDoorState(l_Door);
        });
    }
    else
        door->SetGoState(GOState::GO_STATE_ACTIVE);
}

void InstanceScript::AddDoor(GameObject* door, bool add)
{
    if (!door)
        return;

    DoorInfoMap::iterator lower = doors.lower_bound(door->GetEntry());
    DoorInfoMap::iterator upper = doors.upper_bound(door->GetEntry());

    if (lower == upper)
        return;

    for (DoorInfoMap::iterator itr = lower; itr != upper; ++itr)
    {
        DoorInfo const& data = itr->second;

        if (add)
        {
            data.bossInfo->door[data.type].insert(door);
            switch (data.boundary)
            {
                default:
                case BOUNDARY_NONE:
                    break;
                case BOUNDARY_N:
                case BOUNDARY_S:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX();
                    break;
                case BOUNDARY_E:
                case BOUNDARY_W:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionY();
                    break;
                case BOUNDARY_NW:
                case BOUNDARY_SE:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() + door->GetPositionY();
                    break;
                case BOUNDARY_NE:
                case BOUNDARY_SW:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() - door->GetPositionY();
                    break;
            }
        }
        else
            data.bossInfo->door[data.type].erase(door);
    }

    if (add)
        UpdateDoorState(door);
}

void InstanceScript::OnGameObjectRemove(GameObject* p_Go)
{
    /// - If gameobject is door, remove it from DoorInfoMap
    {
        DoorInfoMap::iterator l_Lower = doors.lower_bound(p_Go->GetEntry());
        DoorInfoMap::iterator l_Upper = doors.upper_bound(p_Go->GetEntry());

        if (l_Lower != l_Upper)
        {
            for (DoorInfoMap::iterator l_Iterator = l_Lower; l_Iterator != l_Upper; ++l_Iterator)
            {
                DoorInfo const& l_DoorInfo = l_Iterator->second;
                l_DoorInfo.bossInfo->door[l_DoorInfo.type].erase(p_Go);
            }
        }
    }

    ZoneScript::OnGameObjectRemove(p_Go);
}

void InstanceScript::AddMinion(Creature* minion, bool add)
{
    MinionInfoMap::iterator itr = minions.find(minion->GetEntry());
    if (itr == minions.end())
        return;

    if (add)
        itr->second.bossInfo->minion.insert(minion);
    else
        itr->second.bossInfo->minion.erase(minion);
}

bool InstanceScript::SetBossState(uint32 p_ID, EncounterState p_State)
{
    if (p_ID < m_Bosses.size())
    {
        BossInfo* l_BossInfos = &m_Bosses[p_ID];

        if (l_BossInfos->state == EncounterState::TO_BE_DECIDED) // loading
        {
            l_BossInfos->state = p_State;

            for (uint32 l_Type = 0; l_Type < DoorType::MAX_DOOR_TYPES; ++l_Type)
            {
                for (DoorSet::iterator l_Iter = l_BossInfos->door[l_Type].begin(); l_Iter != l_BossInfos->door[l_Type].end(); ++l_Iter)
                    UpdateDoorState(*l_Iter, true);
            }

            return false;
        }
        else
        {
            if (l_BossInfos->state == p_State)
                return false;

            switch (p_State)
            {
                case EncounterState::DONE:
                {
                    for (MinionSet::iterator l_Iter = l_BossInfos->minion.begin(); l_Iter != l_BossInfos->minion.end(); ++l_Iter)
                    {
                        if ((*l_Iter)->isWorldBoss() && (*l_Iter)->isAlive())
                            return false;
                    }

                    /// This buff disappears immediately after killing the boss
                    DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::SpellDetermination);

                    /// @TODO: cross sync
                    #ifndef CROSS
                    /// Handle Guild challenges
                    {
                        InstanceMap::PlayerList const& l_PlayersMap = instance->GetPlayers();
                        for (InstanceMap::PlayerList::const_iterator l_Itr = l_PlayersMap.begin(); l_Itr != l_PlayersMap.end(); ++l_Itr)
                        {
                            if (Player* l_Player = l_Itr->getSource())
                            {
                                GroupPtr l_Group = l_Player->GetGroup();

                                if (l_Group && l_Group->IsGuildGroup())
                                {
                                    if (Guild* l_Guild = l_Player->GetGuild())
                                    {
                                        if (instance->IsRaid())
                                            l_Guild->CompleteGuildChallenge(GuildChallengeType::ChallengeRaid, l_Player);
                                        else if (instance->IsChallengeMode() && p_ID == (m_Bosses.size() - 1))
                                            l_Guild->CompleteGuildChallenge(GuildChallengeType::ChallengeDungeonChallenge, l_Player);
                                        else if (instance->IsDungeon() && p_ID == (m_Bosses.size() - 1))
                                            l_Guild->CompleteGuildChallenge(GuildChallengeType::ChallengeDungeon, l_Player);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    #endif

                    m_EncounterTime = 0;

                    ResetCombatResurrection();

                    if (instance->IsRaid())
                    {
                        /// Bloodlust, Heroism, Temporal Displacementn Insanity and DH's Last Resort debuffs are removed at the end of an encounter
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::HunterInsanity);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::MageTemporalDisplacement);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::ShamanExhaustion);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::ShamanSated);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::SpellDHLastResortDebuff);

                        /// Remove all cooldowns with a recovery time equal or less than 5 minutes
                        DoRemoveSpellCooldownWithTimeOnPlayers(5 * TimeConstants::IN_MILLISECONDS * TimeConstants::MINUTE);

                        /// Remove some specific cooldowns
                        for (uint8 l_I = 0; l_I < eInstanceSpells::MaxSpellResetInInstance; ++l_I)
                            DoRemoveSpellCooldownOnPlayers(g_InstanceSpellsReset[l_I]);
                    }

                    break;
                }
                case EncounterState::IN_PROGRESS:
                {
                    m_EncounterTime = uint32(time(nullptr));
                    StartCombatResurrection();

                    if (instance->IsRaid() || instance->IsLFR())
                    {
                        ///< Tormented Souls
                        // refresh on start encounter and 65% chance to add 3 stacks.
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::TormentedSouls);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::DeadwindHarvester);
                        Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                        if (!l_PlayerList.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                            {
                                if (Player* l_Player = l_Itr->getSource())
                                {
                                    if (l_Player && l_Player->HasSpell(eInstanceSpells::ReapSouls))
                                    {
                                        uint64 l_CasterGuid = l_Player->GetGUID();
                                        uint32 l_Interval = 400;
                                        uint32 l_CastTime = 500;
                                        if (roll_chance_i(65))
                                        {
                                            for (uint8 l_i = 0; l_i < 3; ++l_i)
                                            {
                                                l_Player->AddDelayedEvent([l_CasterGuid]() -> void
                                                {
                                                    if (Player* l_Warlock = sObjectAccessor->FindPlayer(l_CasterGuid))
                                                        l_Warlock->CastSpell(l_Warlock, eInstanceSpells::TormentedSouls, true);
                                                }, l_CastTime);
                                                l_CastTime += l_Interval;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        ///< Removes Wakener's Loyalty stacks before raid encounter
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::WakernersLoyality);
                        DoRemoveAurasWithEncounterStartFlags();
                    }
                    break;
                }
                case EncounterState::FAIL:
                {
                    ///< Fixed exploit with reset Bloodlust, Heroism and etc, when the bosses is near and you can attack with help pet
                    if (IsEncounterInProgress(p_ID))
                        break;

                    /// Now you have to fight for at least 3mins to get a stack.
                    /// It was nerfed due to people intentionally reseting the boss to gain max stack to kill the boss faster.
                    if (m_EncounterTime && instance->IsLFR() && (time(nullptr) - m_EncounterTime) >= 3 * TimeConstants::MINUTE)
                        DoCastSpellOnPlayers(eInstanceSpells::SpellDetermination);

                    if (instance->IsRaid())
                    {
                        /// Bloodlust, Heroism, Temporal Displacement, Insanity and DH Last Resort's debuffs are removed at the end of an encounter
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::HunterInsanity);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::MageTemporalDisplacement);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::ShamanExhaustion);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::ShamanSated);
                        DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::SpellDHLastResortDebuff);

                        /// Remove all cooldowns with a recovery time equal or less than 5 minutes
                        DoRemoveSpellCooldownWithTimeOnPlayers(5 * TimeConstants::IN_MILLISECONDS * TimeConstants::MINUTE);

                        /// Remove some specific cooldowns
                        for (uint8 l_I = 0; l_I < eInstanceSpells::MaxSpellResetInInstance; ++l_I)
                            DoRemoveSpellCooldownOnPlayers(g_InstanceSpellsReset[l_I]);
                    }

                    break;
                }
                default:
                    break;
            }

            if (instance->IsRaid() && ((l_BossInfos->state == EncounterState::IN_PROGRESS && p_State == EncounterState::NOT_STARTED) || p_State == EncounterState::FAIL || p_State == EncounterState::DONE))
                DoRemoveAurasDueToSpellOnPlayers(eInstanceSpells::ArchbishopBenedictus);

            l_BossInfos->state = p_State;
            SaveToDB();
        }

        for (uint32 l_Type = 0; l_Type < DoorType::MAX_DOOR_TYPES; ++l_Type)
        {
            for (DoorSet::iterator l_Iter = l_BossInfos->door[l_Type].begin(); l_Iter != l_BossInfos->door[l_Type].end(); ++l_Iter)
                UpdateDoorState(*l_Iter, true);
        }

        for (MinionSet::iterator l_Iter = l_BossInfos->minion.begin(); l_Iter != l_BossInfos->minion.end(); ++l_Iter)
            UpdateMinionState(*l_Iter, p_State);

        /// Send encounters for Bosses
        if (instance->IsRaid() || instance->IsChallengeMode())
        {
            if (p_State == EncounterState::IN_PROGRESS)
                SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_START);
            else if (p_State == EncounterState::DONE || p_State == EncounterState::FAIL)
                SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_END);
        }

        return true;
    }

    return false;
}

std::string InstanceScript::LoadBossState(const char * data)
{
    if (!data)
        return NULL;
    std::istringstream loadStream(data);
    uint32 buff;
    uint32 bossId = 0;
    for (std::vector<BossInfo>::iterator i = m_Bosses.begin(); i != m_Bosses.end(); ++i, ++bossId)
    {
        loadStream >> buff;
        if (buff < TO_BE_DECIDED)
            SetBossState(bossId, (EncounterState)buff);
    }
    return loadStream.str();
}

std::string InstanceScript::GetBossSaveData()
{
    std::ostringstream saveStream;
    for (std::vector<BossInfo>::iterator i = m_Bosses.begin(); i != m_Bosses.end(); ++i)
        saveStream << (uint32)i->state << ' ';
    return saveStream.str();
}

void InstanceScript::DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime, bool bUseAlternativeState)
{
    if (!uiGuid)
        return;

    GameObject* go = instance->GetGameObject(uiGuid);

    if (go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR || go->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (go->getLootState() == GO_READY)
                go->UseDoorOrButton(uiWithRestoreTime, bUseAlternativeState);
            else if (go->getLootState() == GO_ACTIVATED)
                go->ResetDoorOrButton();
        }
        else
            sLog->outError(LOG_FILTER_GENERAL, "SD2: Script call DoUseDoorOrButton, but gameobject entry %u is type %u.", go->GetEntry(), go->GetGoType());
    }
}

void InstanceScript::DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn)
{
    if (GameObject* go = instance->GetGameObject(uiGuid))
    {
        //not expect any of these should ever be handled
        if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE || go->GetGoType() == GAMEOBJECT_TYPE_DOOR ||
            go->GetGoType() == GAMEOBJECT_TYPE_BUTTON || go->GetGoType() == GAMEOBJECT_TYPE_TRAP)
            return;

        if (go->isSpawned())
            return;

        go->SetRespawnTime(uiTimeToDespawn);
        go->UpdateObjectVisibility();
    }
}

void InstanceScript::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (!lPlayers.isEmpty())
    {
        for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            if (Player* player = itr->getSource())
                player->SendUpdateWorldState(uiStateId, uiStateData);
    }
    else
        sLog->outDebug(LOG_FILTER_TSCR, "DoUpdateWorldState attempt send data but no players in map.");
}

// Send Notify to all players in instance
void InstanceScript::DoSendNotifyToInstance(char const* format, ...)
{
    InstanceMap::PlayerList const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        va_list ap;
        va_start(ap, format);
        char buff[1024];
        vsnprintf(buff, 1024, format, ap);
        va_end(ap);
        for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            if (Player* player = i->getSource())
                if (WorldSession* session = player->GetSession())
                    session->SendNotification("%s", buff);
    }
}

// Reset Achievement Criteria for all players in instance
void InstanceScript::DoResetCriteria(CriteriaTypes type, uint64 miscValue1 /*= 0*/, uint64 miscValue2 /*= 0*/, bool evenIfCriteriaComplete /*= false*/)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* pPlayer = i->getSource())
                pPlayer->ResetCriteria(type, miscValue1, miscValue2, evenIfCriteriaComplete);
}

// Complete Achievement for all players in instance
void InstanceScript::DoCompleteAchievement(uint32 p_AchievementID)
{
    if (sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID()))
        return;

    AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(p_AchievementID);
    if (!l_Achievement)
        return;

    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            l_Player->CompletedAchievement(l_Achievement);
    }
}

// Update Achievement Criteria for all players in instance
void InstanceScript::DoUpdateCriteria(CriteriaTypes type, uint32 miscValue1 /*= 0*/, uint32 miscValue2 /*= 0*/, Unit* unit /*= NULL*/)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* player = i->getSource())
                player->UpdateCriteria(type, miscValue1, miscValue2, 0, unit);
}

// Start timed achievement for all players in instance
void InstanceScript::DoStartCriteriaTimer(CriteriaTimedTypes type, uint32 entry)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* player = i->getSource())
                player->StartCriteriaTimer(type, entry);
}

// Stop timed achievement for all players in instance
void InstanceScript::DoStopCriteriaTimer(CriteriaTimedTypes type, uint32 entry)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* player = i->getSource())
                player->RemoveCriteriaTimer(type, entry);
}

// Remove Auras due to Spell on all players in instance
void InstanceScript::DoRemoveAurasDueToSpellOnPlayers(uint32 spell, uint64 p_Guid /*= 0*/)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            if (Player* player = itr->getSource())
            {
                player->RemoveAurasDueToSpell(spell, p_Guid);
                if (Pet* pet = player->GetPet())
                    pet->RemoveAurasDueToSpell(spell, p_Guid);
            }
        }
    }
}

void InstanceScript::DoRemoveStackOnPlayers(uint32 p_Spell)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator l_Itr = PlayerList.begin(); l_Itr != PlayerList.end(); ++l_Itr)
        {
            if (Player* l_Player = l_Itr->getSource())
            {
                l_Player->RemoveStackOnDuration(p_Spell);
                if (Pet* l_Pet = l_Player->GetPet())
                    l_Pet->RemoveStackOnDuration(p_Spell);
            }
        }
    }
}

void InstanceScript::DoRemoveForcedMovementsOnPlayers(uint64 p_Source /*= 0*/)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (!l_PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
        {
            if (Player* l_Player = l_Iter->getSource())
            {
                if (p_Source)
                {
                    if (l_Player->HasMovementForce(p_Source))
                        l_Player->SendApplyMovementForce(p_Source, false, Position());
                }
                else
                    l_Player->RemoveAllMovementForces();
            }
        }
    }
}

/// Cast spell on all players in instance
void InstanceScript::DoCastSpellOnPlayers(uint32 p_SpellID, Unit* p_Caster /*= nullptr*/, bool p_BreakFirst /*= false*/, bool p_IgnoreGM /*= false*/)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();

    if (!l_PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator l_I = l_PlayerList.begin(); l_I != l_PlayerList.end(); ++l_I)
        {
            if (Player* l_Player = l_I->getSource())
            {
                if (p_IgnoreGM && l_Player->isGameMaster())
                    continue;

                if (p_Caster)
                    p_Caster->CastSpell(l_Player, p_SpellID, true);
                else
                    l_Player->CastSpell(l_Player, p_SpellID, true);

                if (p_BreakFirst)
                    break;
            }
        }
    }
}

void InstanceScript::DoStartConversationOnPlayers(uint32 p_ConversationEntry)
{
    Map::PlayerList const &l_PlayerList = instance->GetPlayers();

    if (!l_PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator l_I = l_PlayerList.begin(); l_I != l_PlayerList.end(); ++l_I)
        {
            if (Player* l_Player = l_I->getSource())
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), p_ConversationEntry, l_Player, nullptr, l_Player->GetPosition()))
                        delete l_Conversation;
                }
            }
        }
    }
}

void InstanceScript::DoStartMovie(uint32 p_MovieID)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (!l_PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
        {
            if (Player* l_Player = l_Iter->getSource())
                l_Player->SendMovieStart(p_MovieID);
        }
    }
}

void InstanceScript::DoSetAlternatePowerOnPlayers(int32 value)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* pPlayer = i->getSource())
                pPlayer->SetPower(POWER_ALTERNATE_POWER, value);
}

void InstanceScript::DoModifyPlayerCurrencies(uint32 id, int32 value)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* pPlayer = i->getSource())
                pPlayer->ModifyCurrency(id, value);
}

void InstanceScript::DoNearTeleportPlayers(Position const p_Pos, bool p_Casting /*= false*/)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();

    if (!l_PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
        {
            if (Player* l_Player = l_Iter->getSource())
            {
                if (l_Player->isAlive())
                    l_Player->NearTeleportTo(p_Pos, p_Casting);
                else
                {
                    bool l_HasOutOfBounds = l_Player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IS_OUT_OF_BOUNDS);

                    l_Player->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IS_OUT_OF_BOUNDS);

                    l_Player->m_Functions.AddFunction([=]() -> void
                    {
                        l_Player->NearTeleportTo(p_Pos, p_Casting);
                    }, l_Player->m_Functions.CalculateTime(200));

                    l_Player->m_Functions.AddFunction([=]() -> void
                    {
                        if (l_HasOutOfBounds)
                            l_Player->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IS_OUT_OF_BOUNDS);
                    }, l_Player->m_Functions.CalculateTime(400));
                }
            }
        }
    }
}

void InstanceScript::DoKilledMonsterKredit(uint32 questId, uint32 entry, uint64 guid/* =0*/)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* pPlayer = i->getSource())
                if (pPlayer->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->KilledMonsterCredit(entry, guid);
}

// Add aura on all players in instance
void InstanceScript::DoAddAuraOnPlayers(uint32 spell)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* player = i->getSource())
                player->AddAura(spell, player);
}

void InstanceScript::DoRemoveSpellCooldownOnPlayers(uint32 p_SpellID)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();

    if (!l_PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
        {
            if (Player* l_Player = l_Iter->getSource())
            {
                if (l_Player->HasSpellCooldown(p_SpellID))
                    l_Player->RemoveSpellCooldown(p_SpellID, true);
            }
        }
    }
}

void InstanceScript::DoRemoveSpellCooldownWithTimeOnPlayers(uint32 p_MinRecoveryTime)
{
    instance->ApplyOnEveryPlayer([&](Player* player)
    {
        player->RemoveSpellCooldownsWithTime(p_MinRecoveryTime);

        auto& Auras = player->GetAppliedAuras();
        for (auto itr = Auras.begin(); itr != Auras.end();)
        {
            if (auto aura = itr->second->GetBase()->GetSpellInfo())
            {
                if (aura->AuraInterruptFlags2 & AURA_INTERRUPT_FLAG2_ENCOUNTER_END || aura->HasAttribute(SPELL_ATTR10_RESET_COOLDOWN_UPON_ENDING_AN_ENCOUNTER))
                {
                    player->RemoveAura(itr);
                }
                else
                    ++itr;
            }
            else
                ++itr;
        }
    });
}

void InstanceScript::DoRemoveAurasWithEncounterStartFlags()
{
    instance->ApplyOnEveryPlayer([&](Player* player)
    {
        auto& Auras = player->GetAppliedAuras();
        for (auto itr = Auras.begin(); itr != Auras.end();)
        {
            if (auto aura = itr->second->GetBase()->GetSpellInfo())
            {
                if (aura->AuraInterruptFlags2 & AURA_INTERRUPT_FLAG2_START_OF_ENCOUNTER)
                {
                    player->RemoveAura(itr);
                }
                else
                    ++itr;
            }
            else
                ++itr;
        }
    });
}

void InstanceScript::DoRemoveSpellCooldownOnPlayers()
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (l_PlayerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            l_Player->RemoveArenaSpellCooldowns();
    }
}

void InstanceScript::HandleItemSetBonusesOnPlayers(bool p_Apply)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (l_PlayerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            l_Player->HandleItemSetBonuses(p_Apply);
    }
}

void InstanceScript::HandleGemBonusesOnPlayers(bool p_Apply)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (l_PlayerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            l_Player->HandleGemBonuses(p_Apply);
    }
}

void InstanceScript::DoCombatStopOnPlayers()
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (l_PlayerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
        {
            if (!l_Player->isInCombat())
                continue;

            l_Player->CombatStopWithPets();
        }
    }
}

void InstanceScript::RepopPlayersAtGraveyard(bool p_ForceGraveyard /*= false*/)
{
    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (l_PlayerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            l_Player->RepopAtGraveyard(p_ForceGraveyard);
    }
}

void InstanceScript::RespawnCreature(uint64 p_Guid /*= 0*/)
{
    auto l_WorldObjects = instance->GetObjectsOnMap();
    for (auto l_WorldObject = l_WorldObjects.begin(); l_WorldObject != l_WorldObjects.end(); l_WorldObject++)
    {
        if ((*l_WorldObject)->GetTypeId() != TypeID::TYPEID_UNIT)
            continue;

        Creature* l_Creature = (*l_WorldObject)->ToCreature();

        if (p_Guid && l_Creature->GetGUID() != p_Guid)
            continue;

        if (l_Creature->isAlive() && l_Creature->isInCombat() && l_Creature->IsAIEnabled)
            l_Creature->AI()->EnterEvadeMode();
        else if (l_Creature->isDead())
        {
            l_Creature->Respawn();

            uint64 l_Guid = l_Creature->GetGUID();
            AddTimedDelayedOperation(100, [this, l_Guid]() -> void
            {
                if (Creature* l_Creature = instance->GetCreature(l_Guid))
                {
                    l_Creature->GetMotionMaster()->Clear();
                    l_Creature->GetMotionMaster()->MoveTargetedHome();
                }
            });
        }
    }
}

void InstanceScript::PlaySceneForPlayers(Position const p_Pos, uint32 p_ScenePackageID)
{
    Map::PlayerList const& l_Players = instance->GetPlayers();
    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            l_Player->PlayStandaloneScene(p_ScenePackageID, 16, p_Pos);
    }
}

void InstanceScript::SendWeatherToPlayers(WeatherState p_Weather, float p_Intensity, Player* p_Target /*= nullptr*/, bool p_Abrupt /*= false*/) const
{
    WorldPacket l_Data(Opcodes::SMSG_WEATHER, 9);
    l_Data << uint32(p_Weather);
    l_Data << float(p_Intensity);
    l_Data.WriteBit(p_Abrupt);
    l_Data.FlushBits();

    if (p_Target)
        p_Target->GetSession()->SendPacket(&l_Data);
    else
        instance->SendToPlayers(&l_Data);
}

void InstanceScript::SendScenarioEventToAllPlayers(uint32 p_EventID, uint32 p_Count /*= 1*/)
{
    Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());
    if (!l_Scenario)
        return;

    if (!instance->GetPlayers().getSize())
        return;

    Player* l_Player = instance->GetPlayers().begin()->getSource();
    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, p_EventID, 0, 0, l_Player, l_Player);
}

bool InstanceScript::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/ /*= NULL*/, uint64 /*miscvalue1*/ /*= 0*/)
{
    sLog->outError(LOG_FILTER_GENERAL, "Achievement system call InstanceScript::CheckAchievementCriteriaMeet but instance script for map %u not have implementation for achievement criteria %u",
        instance->GetId(), criteria_id);
    return false;
}

bool InstanceScript::CheckRequiredBosses(uint32 p_ID, Player const* p_Player) const
{
    /// Disable case (for LFR)
    if (m_DisabledMask & (1 << p_ID))
        return false;

    if (p_Player && p_Player->isGameMaster())
        return true;

    if (instance->GetPlayersCountExceptGMs() > instance->ToInstanceMap()->GetMaxPlayers())
        return false;

    return true;
}

void InstanceScript::SendEncounterUnit(uint32 p_Type, Unit* p_Unit /*= NULL*/, uint8 p_Param1 /*= 0*/, uint8 /*p_Param2*/ /*= 0*/)
{
    WorldPacket l_Data;

    switch (p_Type)
    {
        case EncounterFrameType::ENCOUNTER_FRAME_ENGAGE:
            if (!p_Unit)
                return;
            p_Unit->BuildEncounterFrameData(&l_Data, true, p_Param1);
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE:
            if (!p_Unit)
                return;
            p_Unit->BuildEncounterFrameData(&l_Data, false);
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_UPDATE_PRIORITY:
            if (!p_Unit)
                return;
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_CHANGE_PRIORITY, 8 + 1);
            l_Data.append(p_Unit->GetPackGUID());
            l_Data << uint8(p_Param1);  // TargetFramePriority
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_START_TIMER:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_TIMER_START, 4);
            l_Data << int32(0);         // TimeRemaining
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_START_OBJECTIVE:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_START, 4);
            l_Data << int32(0);         // ObjectiveID
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_COMPLETE_OBJECTIVE:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_COMPLETE, 4);
            l_Data << int32(0);         // ObjectiveID
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_START:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_START, 4 * 4);
            l_Data << uint32(m_InCombatResCount);
            l_Data << int32(m_MaxInCombatResCount);
            l_Data << int32(m_CombatResChargeTime);
            l_Data << uint32(m_NextCombatResChargeTime);
            l_Data.WriteBit(true);  ///< InProgress 7.3.5
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_UPDATE_OBJECTIVE:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_UPDATE, 4 * 2);
            l_Data << int32(0);         // ProgressAmount
            l_Data << int32(0);         // ObjectiveID
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_END:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_END, 0);
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_IN_COMBAT_RESURRECTION:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_IN_COMBAT_RESURRECTION, 0);
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_PHASE_SHIFT_CHANGED:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_PHASE_SHIFT_CHANGED, 0);
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_GAIN_COMBAT_RESURRECTION_CHARGE:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_GAIN_COMBAT_RESURRECTION_CHARGE, 4 * 2);
            l_Data << int32(m_InCombatResCount);
            l_Data << uint32(m_CombatResChargeTime);
            break;
        case EncounterFrameType::ENCOUNTER_FRAME_SET_ALLOWING_RELEASE:
            l_Data.Initialize(Opcodes::SMSG_INSTANCE_ENCOUNTER_SET_ALLOWING_RELEASE, 1);
            l_Data.WriteBit(p_Param1 != 0);

            m_ReleaseAllowed = p_Param1 != 0;
            break;
        default:
            break;
    }

    instance->SendToPlayers(&l_Data);
}

bool InstanceScript::IsWipe()
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();

    if (PlayerList.isEmpty())
        return true;

    for (Map::PlayerList::const_iterator Itr = PlayerList.begin(); Itr != PlayerList.end(); ++Itr)
    {
        Player* player = Itr->getSource();

        if (!player)
            continue;

        if (player->isAlive() && !player->isGameMaster()
            && !player->HasAura(27827)) ///< Spirit of Redemption
            return false;
    }

    return true;
}

enum eLegoUpgradeData
{
    ItemMinCount            = 18,
    ItemMaxCount            = 36
};

void InstanceScript::UpdateEncounterState(EncounterCreditType p_Type, uint32 p_CreditEntry, Unit* p_Source)
{
    if (!p_Source)
        return;

    DungeonEncounterList const* l_Encounters = sObjectMgr->GetDungeonEncounterList(instance->GetId(), instance->GetDifficultyID());
    if (!l_Encounters || l_Encounters->empty())
        return;

    if (!IsValidEncounter(p_Type, p_Source))
        return;

    int32 l_MaxIndex = -100000;
    for (DungeonEncounterList::const_iterator l_Iter = l_Encounters->begin(); l_Iter != l_Encounters->end(); ++l_Iter)
    {
        if ((*l_Iter)->dbcEntry->OrderIndex > l_MaxIndex)
            l_MaxIndex = (*l_Iter)->dbcEntry->OrderIndex;
    }

    for (DungeonEncounter const* l_Iter : *l_Encounters)
    {
        if (l_Iter->dbcEntry->CreatureDisplayID == p_Source->GetNativeDisplayId() || (l_Iter->creditType == p_Type && l_Iter->creditEntry == p_CreditEntry))
        {
            m_CompletedEncounters |= 1 << l_Iter->dbcEntry->Bit;

            SendEncounterEnd(l_Iter->dbcEntry->ID, true);

            if (p_Source->GetTypeId() == TypeID::TYPEID_UNIT)
                SaveEncounterLogs(p_Source->ToCreature(), l_Iter->dbcEntry->ID);

            SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_END, p_Source);

            WorldPacket l_Data(Opcodes::SMSG_BOSS_KILL_CREDIT, 4);
            l_Data << int32(l_Iter->dbcEntry->ID);
            instance->SendToPlayers(&l_Data);

            Player* l_PlayerRef = nullptr;
            Map::PlayerList const& l_PlayerList = instance->GetPlayers();
            for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
            {
                if (Player* l_Player = l_Itr->getSource())
                {
                    if (p_Source->GetMap() != l_Player->GetMap() || l_Player->isGameMaster())
                        continue;

                    if (l_PlayerRef == nullptr)
                        l_PlayerRef = l_Player;

                    if ((l_Iter->dbcEntry->OrderIndex == l_MaxIndex && l_Iter->dbcEntry->ID != 2053) || l_Iter->dbcEntry->ID == 2039)
                    {
                        if (p_Source->GetMap()->IsNonRaidDungeon() && p_Source->GetMap()->GetEntry()->ExpansionID == Expansion::EXPANSION_LEGION)
                        {
                            if (p_Source->GetMap()->IsMythic())
                                l_Player->CompleteLegendaryActivity(LegendaryActivity::MythicDungeon, l_Iter->dbcEntry->ID);
                            else if (p_Source->GetMap()->IsHeroic())
                                l_Player->CompleteLegendaryActivity(LegendaryActivity::HeroicDungeon, l_Iter->dbcEntry->ID);
                            else
                                l_Player->CompleteLegendaryActivity(LegendaryActivity::NormalDungeon, l_Iter->dbcEntry->ID);
                        }

                        uint32 l_DungeonID = l_Player->GetGroup() ? sLFGMgr->GetDungeon(l_Player->GetGroup()->GetGUID()) : 0;
                        sLFGMgr->RewardDungeonDoneFor(l_DungeonID, l_Player);
                    }

                    if (p_Source->GetMap()->IsRaid() && p_Source->GetMap()->GetEntry()->ExpansionID == Expansion::EXPANSION_LEGION && !l_Player->BossAlreadyLooted(p_Source))
                    {
                        if (p_Source->GetMap()->IsLFR())
                            l_Player->CompleteLegendaryActivity(LegendaryActivity::LfrRaidBoss, l_Iter->dbcEntry->ID);
                        else if (p_Source->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidNormal)
                            l_Player->CompleteLegendaryActivity(LegendaryActivity::NormalRaidBoss, l_Iter->dbcEntry->ID);
                        else if (p_Source->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic)
                            l_Player->CompleteLegendaryActivity(LegendaryActivity::HeroicRaidBoss, l_Iter->dbcEntry->ID);
                        else if (p_Source->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                            l_Player->CompleteLegendaryActivity(LegendaryActivity::MythicRaidBoss, l_Iter->dbcEntry->ID);
                    }

                    /// First Antorus the Burning Throne boss kill of the week award Wakening Essences - Don't check difficulty
                    if (p_Source->GetMap()->GetId() == 1712 && !l_Player->BossAlreadyLooted(p_Source, false))
                    {
                        uint32 l_Count = urand(eLegoUpgradeData::ItemMinCount, eLegoUpgradeData::ItemMaxCount);

                        l_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, l_Count);
                        l_Player->SendDisplayToast(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, l_Count, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
                    }
                }
            }

            if (!sObjectMgr->IsDisabledEncounter(l_Iter->dbcEntry->ID, instance->GetDifficultyID()) &&
                !sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID()))
            {
                DoUpdateCriteria(CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER, l_Iter->dbcEntry->ID, 0, p_Source);

                if (p_Source != nullptr)
                {
                    if (Scenario* l_Progress = sScenarioMgr->GetScenario(instance->GetScenarioGuid()))
                        l_Progress->UpdateAchievementCriteria(CRITERIA_TYPE_KILL_CREATURE, p_Source->GetEntry(), 1, 0, p_Source, l_PlayerRef);
                }

                uint32 l_Bonus = sObjectMgr->GetCreatureLootBonus(p_Source->GetEntry());
                if (l_Bonus && !IsChallenge())
                    DoCastSpellOnPlayers(l_Bonus, p_Source);
            }

            BeforeAddBossLooted(p_CreditEntry);

            DoAddBossLootedOnPlayers(p_Source);
            return;
        }
    }
}

void InstanceScript::DoAddBossLootedOnPlayers(Unit* p_Source)
{
    if (p_Source->GetTypeId() != TypeID::TYPEID_UNIT)
        return;

    if (!instance->IsRaid() && !instance->IsMythic())
        return;

    if (p_Source->ToCreature()->GetCreatureTemplate()->expansion < Expansion::EXPANSION_LEGION)
        return;

    Map::PlayerList const& l_PlayerList = instance->GetPlayers();
    if (l_PlayerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
            l_Player->AddBossLooted(p_Source->ToCreature());
    }
}

void InstanceScript::StartEncounter(uint32 p_EncounterID, Creature* p_Source /*= nullptr*/)
{
    if (!p_EncounterID)
        return;

    HandleVantusRune(p_EncounterID, true);

    m_EncounterID = p_EncounterID;

    SendEncounterStart(p_EncounterID);

    /// Don't record the kill if encounter is disabled
    if (sObjectMgr->IsDisabledEncounter(p_EncounterID, instance->GetDifficultyID()) ||
        sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID()))
        return;

    /// Don't record the kill if it's a test realm
    if (!sWorld->CanBeSaveInLoginDatabase())
        return;

    /// Reset datas before each attempt
    m_EncounterDatas = EncounterDatas();

    m_EncounterDatas.Expansion = instance->GetEntry()->ExpansionID;

    /// Register encounter datas for further logs
    if (instance->IsRaid() && m_EncounterDatas.Expansion == sWorld->getIntConfig(WorldIntConfigs::CONFIG_EXPANSION))
    {
        m_EncounterDatas.RealmID        = g_RealmID;
        m_EncounterDatas.EncounterID    = p_EncounterID;

        uint32 l_GuildID = 0;

        Map::PlayerList const& l_PlayerList = instance->GetPlayers();
        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
        {
            if (Player* l_Player = l_Iter->getSource())
            {
                if (l_Player->isGameMaster())
                    continue;

                if (GroupPtr l_Group = l_Player->GetGroup())
                {
                    if (!l_Group->IsGuildGroup())
                        break;

                    l_GuildID = l_Group->GetGroupGuildID();

                    if (!l_GuildID || l_Player->GetGuildId() != l_GuildID)
                        continue;

                    m_EncounterDatas.GuildID        = l_Player->GetGuildId();
                    m_EncounterDatas.GuildFaction   = l_Player->GetTeamId();
                    m_EncounterDatas.GuildName      = l_Player->GetGuildName();
                    break;
                }
            }
        }

        m_EncounterDatas.MapID          = instance->GetId();
        m_EncounterDatas.DifficultyID   = instance->GetDifficultyID();
        m_EncounterDatas.StartTime      = uint32(time(nullptr));
    }

    if (p_Source != nullptr)
        m_EncounterDatas.EncounterHealth = p_Source->GetMaxHealth();
}

void InstanceScript::SendEncounterStart(uint32 p_EncounterID, Player* p_Target /*= nullptr*/)
{
    WorldPacket l_Data(Opcodes::SMSG_ENCOUNTER_START);
    l_Data << uint32(p_EncounterID);
    l_Data << uint32(instance->GetDifficultyID());
    l_Data << uint32(instance->GetPlayers().getSize());

    if (p_Target)
        p_Target->SendDirectMessage(&l_Data);
    else
        instance->SendToPlayers(&l_Data);
}

void InstanceScript::SendEncounterEnd(uint32 p_EncounterID, bool p_Success)
{
    if (!p_EncounterID)
        return;

    HandleVantusRune(p_EncounterID, false);

    WorldPacket l_Data(Opcodes::SMSG_ENCOUNTER_END);
    l_Data << uint32(p_EncounterID);
    l_Data << uint32(instance->GetDifficultyID());
    l_Data << uint32(instance->GetPlayers().getSize());
    l_Data.WriteBit(p_Success);
    l_Data.FlushBits();
    instance->SendToPlayers(&l_Data);

    /// Don't record the kill if encounter is disabled
    if (sObjectMgr->IsDisabledEncounter(p_EncounterID, instance->GetDifficultyID()) ||
        sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID()))
        return;

    /// Don't record the kill if it's a test realm
    if (!sWorld->CanBeSaveInLoginDatabase())
        return;

    m_EncounterDatas.CombatDuration = uint32(time(nullptr)) - m_EncounterDatas.StartTime;
    m_EncounterDatas.EndTime        = uint32(time(nullptr));
    m_EncounterDatas.Success        = p_Success;

    if (m_EncounterDatas.GuildID)
    {
        Map::PlayerList const& l_PlayerList = instance->GetPlayers();
        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
        {
            if (Player* l_Player = l_Iter->getSource())
            {
                if (l_Player->isGameMaster())
                    continue;

                RosterData l_Data;

                l_Data.GuidLow      = l_Player->GetGUIDLow();
                l_Data.Name         = l_Player->GetName();
                l_Data.Level        = l_Player->getLevel();
                l_Data.Class        = l_Player->getClass();
                l_Data.SpecID       = l_Player->GetActiveSpecializationID();
                l_Data.Role         = l_Player->GetRoleForGroup();
                l_Data.ItemLevel    = l_Player->GetAverageItemLevelEquipped();

                m_EncounterDatas.RosterDatas.push_back(l_Data);
            }
        }

        sScriptMgr->OnEncounterEnd(&m_EncounterDatas);
    }
}

void InstanceScript::HandleVantusRune(uint32 p_EncounterID, bool p_Apply)
{
    using VantusEncounter = std::pair<uint32, uint32>;

    std::vector<VantusEncounter> l_VantusEncounters =
    {
        { 1841 , 191464 }, ///< Vantus Rune : Ursoc
        { 1853 , 192761 }, ///< Vantus Rune : Nythendra
        { 1873 , 192762 }, ///< Vantus Rune : Il'gynoth, The Heart Of Corruption
        { 1854 , 192763 }, ///< Vantus Rune : Dragons of Nightmare
        { 1864 , 192764 }, ///< Vantus Rune : Xavius
        { 1876 , 192765 }, ///< Vantus Rune : Elerethe Renferal
        { 1877 , 192766 }, ///< Vantus Rune : Cenarius
        { 1849 , 192767 }, ///< Vantus Rune : Skorpyron
        { 1865 , 192768 }, ///< Vantus Rune : Chronomatic Anomaly
        { 1867 , 192769 }, ///< Vantus Rune : Trilliax
        { 1871 , 192770 }, ///< Vantus Rune : Spellblade Aluriel
        { 1862 , 192771 }, ///< Vantus Rune : Tichondrius
        { 1886 , 192772 }, ///< Vantus Rune : High Botanist Tel'arn
        { 1842 , 192773 }, ///< Vantus Rune : Krosus
        { 1863 , 192774 }, ///< Vantus Rune : Star Augur Etraeus
        { 1872 , 192775 }, ///< Vantus Rune : Grand Magistrix Elisande
        { 1866 , 192776 }, ///< Vantus Rune : Gul'dan
        { 2038 , 237820 }, ///< Vantus Rune : Fallen Avatar
        { 2032 , 237821 }, ///< Vantus Rune : Goroth
        { 2050 , 237822 }, ///< Vantus Rune : Sisters of the Moon
        { 2052 , 237823 }, ///< Vantus Rune : Maiden of Vigilance
        { 2036 , 237824 }, ///< Vantus Rune : Harjatan
        { 2051 , 237825 }, ///< Vantus Rune : Kil'jaeden
        { 2037 , 237826 }, ///< Vantus Rune : Mistress Sassz'ine
        { 2054 , 237827 }, ///< Vantus Rune : The Desolate Host
        { 2048 , 237828 }, ///< Vantus Rune : Demonic Inquisition
        { 2063 , 250144 }, ///< Vantus Rune : Aggramar
        { 2092 , 250146 }, ///< Vantus Rune : Argus the Unmaker
        { 2088 , 250148 }, ///< Vantus Rune : Kin'Garoth
        { 2075 , 250150 }, ///< Vantus Rune : The defense of Eonar
        { 2076 , 250153 }, ///< Vantus Rune : Garothi the WorldBreaker
        { 2074 , 250155 }, ///< Vantus Rune : Felhounds of Sargeras
        { 2082 , 250158 }, ///< Vantus Rune : Imonar the Soulhunter
        { 2064 , 250160 }, ///< Vantus Rune : Portal keeper Hasabel
        { 2073 , 250163 }, ///< Vantus Rune : The Coven of Shivarra
        { 2069 , 250165 }, ///< Vantus Rune : Varimathras
        { 2070 , 250167 }, ///< Vantus Rune : Antoran High Command
    };

    auto l_Itr = std::find_if(l_VantusEncounters.begin(), l_VantusEncounters.end(),
    [&p_EncounterID] (VantusEncounter& p_Itr) -> bool
    {
        return p_EncounterID == p_Itr.first;
    });

    if (l_Itr == l_VantusEncounters.end())
        return;

    SpellInfo const* l_PassiveSpell = sSpellMgr->GetSpellInfo((*l_Itr).second);

    if (!l_PassiveSpell)
        return;

    uint32 l_TriggeredSpellId = l_PassiveSpell->Effects[EFFECT_0].TriggerSpell;

    if (!l_TriggeredSpellId)
        return;

    instance->ForEachPlayer([&p_Apply, &l_TriggeredSpellId, &l_Itr]
    (Player* p_Itr) -> void
    {
        if (p_Itr == nullptr)
            return;

        if (p_Apply)
        {
            if (p_Itr->HasAura(l_Itr->second))
                p_Itr->CastSpell(p_Itr, l_TriggeredSpellId, true);
        }
        else
            p_Itr->RemoveAura(l_TriggeredSpellId);
    });
}

void InstanceScript::SaveEncounterLogs(Creature* p_Creature, uint32 p_EncounterID)
{
    if ((p_Creature->GetNativeTemplate()->flags_extra & CREATURE_FLAG_EXTRA_LOG_GROUP_DMG) == 0)
        return;

    CreatureDamageLogList const& l_DamageLogs  = p_Creature->GetDamageLogs();
    GroupDumpList const& l_GroupDumps          = p_Creature->GetGroupDumps();

    SQLTransaction l_Transaction = LogDatabase.BeginTransaction();

    for (auto l_Log : l_DamageLogs)
    {
        PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_ENCOUNTER_DAMAGE_LOG);
        l_Statement->setUInt32(0, p_EncounterID);
        l_Statement->setUInt64(1, m_EncounterDatas.StartTime);
        l_Statement->setUInt64(2, l_Log.Time);
        l_Statement->setUInt32(3, l_Log.AttackerGuid);
        l_Statement->setUInt32(4, l_Log.Damage);
        l_Statement->setUInt32(5, l_Log.Spell);
        l_Transaction->Append(l_Statement);
    }

    for (auto l_Dump : l_GroupDumps)
    {
        PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_ENCOUNTER_GROUP_DUMP_LOG);
        l_Statement->setUInt32(0, p_EncounterID);
        l_Statement->setUInt64(1, m_EncounterDatas.StartTime);
        l_Statement->setUInt64(2, l_Dump.Time);
        l_Statement->setString(3, l_Dump.Dump);
        l_Transaction->Append(l_Statement);
    }

    LogDatabase.CommitTransaction(l_Transaction);

    p_Creature->ClearDamageLog();
    p_Creature->ClearGroupDumps();
}

uint32 InstanceScript::GetEncounterIDForBoss(Creature* p_Boss) const
{
    DungeonEncounterList const* l_Encounters = sObjectMgr->GetDungeonEncounterList(instance->GetId(), instance->GetDifficultyID());
    if (!l_Encounters || l_Encounters->empty() || p_Boss == nullptr)
        return 0;

    for (DungeonEncounterList::const_iterator l_Iter = l_Encounters->begin(); l_Iter != l_Encounters->end(); ++l_Iter)
    {
        if (((*l_Iter)->dbcEntry->CreatureDisplayID == p_Boss->GetNativeDisplayId()) || ((*l_Iter)->creditEntry == p_Boss->GetEntry()))
            return (*l_Iter)->dbcEntry->ID;
    }

    return 0;
}

uint32 InstanceScript::GetEncounterIDForChest(uint32 p_Entry) const
{
    switch (p_Entry)
    {
        case 245847: ///< Halls of Valor - Odyn
            return 1809;
        case 246036: ///< Maw of Souls - Helya
            return 1824;
        case 251482: ///< Neltharion's Lair - Naraxas
            return 1792;
        case 246430: ///< Violet Hold - Millificent Manastorm
            return 1847;
        case 266592: ///< Return to Karazhan - Mana Devourer
            return 1959;
        case 254168: ///< The Emerald Nightmare - Cenarius
            return 1877;
        case 260526: ///< Trial of Valor - Odyn
            return 1958;
        case 252680: ///< Lucky Challenger Chest
        case 252681: ///< Lucky Challenger Chest
        case 252673: ///< Lucky Challenger Chest
        {
            switch (instance->GetId())
            {
                case 1501: ///< Black Rook Hold - Lord Kurtalos Ravencrest
                    return 1835;
                case 1571: ///< Court of Stars - Advisor Melandrus
                    return 1870;
                case 1466: ///< Darkheart Thicket - Shade of Xavius
                    return 1839;
                case 1456: ///< Eye of Azshara - Wrath of Azshara
                    return 1814;
                case 1477: ///< Halls of Valor - Odyn
                    return 1809;
                case 1492: ///< Maw of Souls - Helya
                    return 1824;
                case 1458: ///< Neltharion's Lair - Dargrul the Underking
                    return 1793;
                case 1516: ///< The Arcway - Advisor Vandros
                    return 1829;
                case 1493: ///< Vault of the Wardens - Cordana Felsong
                    return 1818;
                case 1677: ///< Cathedral of Eternal Night - Mephistroth
                    return 2039;
                default:
                    return 0;
            }

            break;
        }
        default:
            return 0;
    }

    return 0;
}

void InstanceScript::UpdatePhasing()
{
    PhaseUpdateData phaseUdateData;
    phaseUdateData.AddConditionType(CONDITION_INSTANCE_DATA);

    Map::PlayerList const& players = instance->GetPlayers();
    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        if (Player* player = itr->getSource())
            player->GetPhaseMgr().NotifyConditionChanged(phaseUdateData);
}

void InstanceScript::UpdateCreatureGroupSizeStats()
{
    if (instance->IsMythic())
        return;

    auto l_WorldObjects = instance->GetObjectsOnMap();
    for (auto l_WorldObject = l_WorldObjects.begin(); l_WorldObject != l_WorldObjects.end(); l_WorldObject++)
    {
        if ((*l_WorldObject)->GetTypeId() != TYPEID_UNIT || !(*l_WorldObject)->IsInWorld())
            continue;

        Creature* l_Creature = (*l_WorldObject)->ToCreature();
        l_Creature->UpdateGroupSizeStats();
    }
}

void InstanceScript::UpdateDamageManager(uint64 p_Guid, int32 p_Damage, bool p_Heal /*= false*/)
{
    if (!p_Damage)
        return;

    int8 l_PullNum = GetPullDamageManager(p_Guid);
    if (l_PullNum < 0)
        return;

    DamageManagerMap::const_iterator l_Itr = m_DamageManager.find(l_PullNum);
    if (l_Itr == m_DamageManager.end())
        return;

    std::vector<uint64> const l_Manager = l_Itr->second;
    if (l_Manager.empty())
        return;

    for (std::vector<uint64>::const_iterator l_Iter = l_Manager.begin(); l_Iter != l_Manager.end(); ++l_Iter)
    {
        if (!instance)
            return;

        if (Creature* l_Pull = instance->GetCreature((*l_Iter))) ///< If crashed uncomment this
        {
            if (!l_Pull->isAlive() || (*l_Iter) == p_Guid)
                continue;

            Map::PlayerList const& l_Players = l_Pull->GetMap()->GetPlayers();
            if (l_Players.isEmpty())
                continue;

            for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
            {
                if (Player* l_Player = l_Itr->getSource())
                {
                    if (!l_Player->isInCombat())
                        continue;

                    if (!p_Heal && p_Damage >= l_Pull->GetHealth())
                        l_Player->Kill(l_Pull, true);
                    else
                        l_Pull->SetHealth(l_Pull->GetHealth() - p_Damage);

                    break;
                }
            }
        }
    }
}

void InstanceScript::AddToDamageManager(Creature* p_Creature, uint8 p_PullNum)
{
    if (!p_Creature || !p_Creature->isAlive())
        return;

    SetPullDamageManager(p_Creature->GetGUID(), p_PullNum);

    m_DamageManager[p_PullNum].push_back(p_Creature->GetGUID());
    m_InitDamageManager = true;
}

void InstanceScript::RemoveFromDamageManager(Creature* p_Creature, uint8 p_PullNum /*= 0*/)
{
    if (!p_Creature)
        return;

    m_PullDamageManager.erase(p_Creature->GetGUID());

    auto l_Iter = m_DamageManager.find(p_PullNum);
    if (l_Iter == m_DamageManager.end())
        return;

    for (std::vector<uint64>::iterator l_I = l_Iter->second.begin(); l_I < l_Iter->second.end();)
    {
        if ((*l_I) == p_Creature->GetGUID())
            l_I = l_Iter->second.erase(l_I);
        else
            ++l_I;
    }
}

int8 InstanceScript::GetPullDamageManager(uint64 p_Guid) const
{
    if (m_PullDamageManager.empty())
        return -1;

    PullDamageManagerMap::const_iterator l_Iter = m_PullDamageManager.find(p_Guid);
    if (l_Iter == m_PullDamageManager.end())
        return -1;

    return l_Iter->second;
}

void InstanceScript::HandleBloodlustApplied(uint64 p_Guid)
{
    if (!IsEncounterInProgress())
        return;

    /// Already recorded
    uint32 l_Time = uint32(time(nullptr));
    if (m_BloodlustGuids.find(std::make_pair(p_Guid, l_Time)) != m_BloodlustGuids.end())
        return;

    m_BloodlustGuids.insert(std::make_pair(p_Guid, l_Time));
    ++m_EncounterDatas.BloodlustCount;
}

void InstanceScript::HandleRespawnAtLoading(uint64 p_Guid, uint32 p_BossID)
{
    uint64 l_Guid   = p_Guid;
    uint32 l_BossID = p_BossID;

    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid, l_BossID]() -> void
    {
        if (GetBossState(l_BossID) == EncounterState::FAIL)
        {
            if (Creature* l_Creature = sObjectAccessor->FindCreature(l_Guid))
            {
                l_Creature->NearTeleportTo(l_Creature->GetHomePosition());
                l_Creature->Respawn(true, true);
            }
        }
    });
}

//////////////////////////////////////////////////////////////////////////
/// Combat Resurrection - http://wow.gamepedia.com/Resurrect#Combat_resurrection
void InstanceScript::ResetCombatResurrection()
{
    if (!instance->IsRaid())
        return;

    m_InCombatResCount = 0;
    m_MaxInCombatResCount = 0;
    m_CombatResChargeTime = 0;
    m_NextCombatResChargeTime = 0;
}

/// Mythic+ Challenge have a different kind of Combat Resurrection system
void InstanceScript::StartChallengeCombatResurrection()
{
    /// Starting a challenge dungeon, all combat resurrection spells will have their cooldowns reset and begin with 1 charge.
    for (uint8 l_I = 0; l_I < eInstanceSpells::MaxBattleResSpells; ++l_I)
        DoRemoveSpellCooldownOnPlayers(g_BattleResSpells[l_I]);

    m_InCombatResCount        = 1;
    m_MaxInCombatResCount     = 5;
    m_CombatResChargeTime     = 10 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS;
    m_NextCombatResChargeTime = 10 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS;
}

void InstanceScript::StartCombatResurrection()
{
    if (!instance->IsRaid())
        return;

    /// Upon engaging a boss, all combat resurrection spells will have their cooldowns reset and begin with 1 charge.
    for (uint8 l_I = 0; l_I < eInstanceSpells::MaxBattleResSpells; ++l_I)
        DoRemoveSpellCooldownOnPlayers(g_BattleResSpells[l_I]);

    m_InCombatResCount = 1;

    /// Charges will accumulate at a rate of 1 per (90/RaidSize) minutes.
    /// Example 1: A 10-player raid will accumulate 1 charge every 9 minutes (90/10 = 9).
    /// Example 2: A 20-player raid will accumulate 1 charge every 4.5 minutes (90/20 = 4.5).
    uint32 l_PlayerCount = instance->GetPlayers().getSize();
    if (!l_PlayerCount)
        return;

    float l_Value = 9000.0f / (float)l_PlayerCount;
    uint32 l_Timer = uint32(l_Value / 100.0f);

    l_Value -= (float)l_Timer * 100.0f;
    l_Timer *= TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS;
    l_Value *= TimeConstants::MINUTE / 100.0f * TimeConstants::IN_MILLISECONDS;
    l_Timer += uint32(l_Value);

    m_MaxInCombatResCount = 9;
    m_CombatResChargeTime = l_Timer;
    m_NextCombatResChargeTime = l_Timer;
}

void InstanceScript::UpdateCombatResurrection(uint32 const p_Diff)
{
    if (!m_NextCombatResChargeTime)
        return;

    /// Add a charge
    if (m_NextCombatResChargeTime <= p_Diff)
    {
        m_NextCombatResChargeTime = m_CombatResChargeTime;
        m_InCombatResCount = std::min(++m_InCombatResCount, m_MaxInCombatResCount);

        SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_GAIN_COMBAT_RESURRECTION_CHARGE);
    }
    else
        m_NextCombatResChargeTime -= p_Diff;
}

bool InstanceScript::CanUseCombatResurrection() const
{
    if (!instance->IsRaid() && !instance->IsChallengeMode())
        return true;

    if (!IsEncounterInProgress())
        return true;

    if (m_InCombatResCount <= 0)
        return false;

    return true;
}

void InstanceScript::ConsumeCombatResurrectionCharge()
{
    /// Shouldn't happen
    if (m_InCombatResCount <= 0)
        return;

    --m_InCombatResCount;
    ++m_EncounterDatas.BattleRezCount;
    SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_IN_COMBAT_RESURRECTION);
}

void InstanceScript::ResetChallengeMode()
{
    if (m_Challenge)
        m_Challenge->ResetGo();

    instance->m_respawnChallenge = time(NULL); // For respawn all mobs
    RepopPlayersAtGraveyard();
    instance->SetSpawnMode(Difficulty::DifficultyMythic);
}

// Redirect query to challenge
void InstanceScript::OnCreatureCreateForScript(Creature* creature)
{
    if (m_Challenge)
        m_Challenge->OnCreatureCreateForScript(creature);
}

void InstanceScript::OnCreatureRemoveForScript(Creature* creature)
{
    if (m_Challenge)
        m_Challenge->OnCreatureRemoveForScript(creature);
}

void InstanceScript::OnCreatureUpdateDifficulty(Creature* creature)
{
    if (m_Challenge)
        m_Challenge->OnCreatureUpdateDifficulty(creature);
}

void InstanceScript::EnterCombatForScript(Creature* creature, Unit* enemy)
{
    if (m_Challenge)
        m_Challenge->EnterCombatForScript(creature, enemy);
}

void InstanceScript::CreatureDiesForScript(Creature* creature, Unit* killer)
{
    if (m_Challenge)
        m_Challenge->CreatureDiesForScript(creature, killer);
}

void InstanceScript::OnPlayerEnterForScript(Player* player)
{
    if (m_Challenge)
        m_Challenge->OnPlayerEnterForScript(player);
}

void InstanceScript::OnPlayerLeaveForScript(Player* player)
{
    if (m_Challenge)
        m_Challenge->OnPlayerLeaveForScript(player);
}

void InstanceScript::OnPlayerDiesForScript(Player* player)
{
    if (m_Challenge)
        m_Challenge->OnPlayerDiesForScript(player);
}

void InstanceScript::OnGameObjectCreateForScript(GameObject* go)
{
    if (m_Challenge)
        m_Challenge->OnGameObjectCreateForScript(go);

    switch (go->GetEntry())
    {
        case ChallengeModeOrb:
            AddChallengeModeOrb(go->GetGUID());
            break;
        case 211989:
        case 211991:
        case 212972:
        case 211992:
        case 211988:
        case 212282:
        case 212387:
        case 239323:
        case 239408:
            AddChallengeModeDoor(go->GetGUID());
            break;
        case 252680:    ///< Challenger's Cache
            if (!m_ChallengeChestFirst)
                m_ChallengeChestFirst = go->GetGUID();
            break;
        case 252681:    ///< Superior Challenger's Cache
            m_ChallengeChestSecond = go->GetGUID();
            break;
        case 252673:    ///< Peerless Challenger's Cache
            m_ChallengeChestThird = go->GetGUID();
            break;
        default:
            break;
    }
}

void InstanceScript::OnGameObjectRemoveForScript(GameObject* go)
{
    if (m_Challenge)
        m_Challenge->OnGameObjectRemoveForScript(go);
}

void BuildParallelPath(std::vector<G3D::Vector3> const& l_Path, float l_Distance, Unit* l_Ref, std::vector<G3D::Vector3>& l_Result, bool l_RightSide)
{
    if (l_Path.size() < 2)
        return;

    struct StraightLine
    {
        struct Point
        {
            float X;
            float Y;

            void operator=(Position const& l_Pos)
            {
                X = l_Pos.m_positionX;
                Y = l_Pos.m_positionY;
            }

            void operator=(G3D::Vector3 const& l_Pos)
            {
                X = l_Pos.x;
                Y = l_Pos.y;
            }
        };

        float A;
        float B;

        Point Start;
        Point End;

        void ComputeEquation()
        {
            if (End.X == Start.X)
                End.X += 0.005f;

            A = (End.Y - Start.Y) / (End.X - Start.X);
            B = Start.Y - (A * Start.X);
        }

        void BuildAsParallel(StraightLine const& l_Base, float p_Distance)
        {
            float l_Xta = (l_Base.End.X - l_Base.Start.X)/* / 10.0f*/;
            float l_Xtb = l_Base.Start.X;

            float l_Yta = (l_Base.End.Y - l_Base.Start.Y)/* / 10.0f*/;
            float l_Ytb = l_Base.Start.Y;

            if (l_Xta == 0.0f && l_Yta == 0.0f)
                return;

            Start.X = l_Xtb + ((p_Distance * l_Yta) / (std::sqrt(std::pow(l_Xta, 2) + std::pow(l_Yta, 2))));
            Start.Y = l_Ytb - ((p_Distance * l_Xta) / (std::sqrt(std::pow(l_Xta, 2) + std::pow(l_Yta, 2))));

            /// End.X = l_Xta + l_Xtb + ((p_Distance * l_Yta) / (std::sqrt(std::pow(l_Xta, 2) + std::pow(l_Yta, 2))));
            /// End.Y = l_Yta + l_Ytb - ((p_Distance * l_Xta) / (std::sqrt(std::pow(l_Xta, 2) + std::pow(l_Yta, 2))));
            End.X = Start.X + l_Xta;
            End.Y = Start.Y + l_Yta;

            ComputeEquation();
        }
    };

    if (!l_RightSide)
        l_Distance = -l_Distance;

    for (uint32 l_I = 0; l_I < l_Path.size() - 2; ++l_I)
    {
        StraightLine l_Base;
        l_Base.Start = l_Path[l_I];
        l_Base.End = l_Path[l_I + 1];

        StraightLine l_Parallel;
        l_Parallel.BuildAsParallel(l_Base, l_Distance);

        if (!l_I)
        {
            G3D::Vector3 l_Origin;
            l_Origin.x = l_Parallel.Start.X;
            l_Origin.y = l_Parallel.Start.Y;
            l_Origin.z = l_Ref->GetMap()->GetHeight(l_Ref->GetPhaseMask(), l_Origin.x, l_Origin.y, l_Path[l_I].z + 5.0f);
            l_Result.push_back(l_Origin);
        }

        l_Base.Start = l_Path[l_I + 1];
        l_Base.End = l_Path[l_I + 2];

        StraightLine l_NextParallel;
        l_NextParallel.BuildAsParallel(l_Base, l_Distance);

        G3D::Vector3 l_Intersect;
        if (l_Parallel.A != l_NextParallel.A)
        {
            l_Intersect.x = (l_NextParallel.B - l_Parallel.B) / (l_Parallel.A - l_NextParallel.A);
            l_Intersect.y = (l_Parallel.A * l_Intersect.x) + l_Parallel.B;
        }
        else
        {
            l_Intersect.x = l_Parallel.End.X;
            l_Intersect.y = l_Parallel.End.Y;
        }

        l_Intersect.z = l_Ref->GetMap()->GetHeight(l_Ref->GetPhaseMask(), l_Intersect.x, l_Intersect.y, l_Path[l_I + 1].z + 2.0f);

        l_Result.push_back(l_Intersect);

        if (l_I == (l_Path.size() - 3))
        {
            G3D::Vector3 l_End;
            l_End.x = l_NextParallel.End.X;
            l_End.y = l_NextParallel.End.Y;
            l_End.z = l_Ref->GetMap()->GetHeight(l_Ref->GetPhaseMask(), l_End.x, l_End.y, l_Path[l_I + 2].z + 2.0f);
            l_Result.push_back(l_End);
        }
    }
}

void InstanceScript::PlayCosmeticEvent(uint32 p_EventId, uint64 p_OriginGuid /*= 0*/, bool p_SummonsFromGuid /*= false*/)
{
    auto const& l_Datas = sObjectMgr->GetCosmeticEventDatas();
    for (uint32 l_I = 0;; ++l_I)
    {
        auto l_Itr = l_Datas.find(MAKE_PAIR64(p_EventId, l_I));
        if (l_Itr == l_Datas.end() || l_Itr->second.empty())
            break;

        auto l_EventEntryItr = l_Itr->second.begin();
        std::advance(l_EventEntryItr, urand(0, l_Itr->second.size() - 1));
        CosmeticEventEntry const& l_EventEntry = *l_EventEntryItr;

        if (l_EventEntry.DifficultyMask && !(l_EventEntry.DifficultyMask & 1 << instance->GetDifficultyID()))
            break;

        AddTimedDelayedOperation(l_EventEntry.Delay, [this, l_EventEntry, p_OriginGuid, p_SummonsFromGuid]() -> void
        {
            WaypointPath const* l_Waypoints = sWaypointMgr->GetPath(l_EventEntry.PathId);
            if (!l_Waypoints)
                return;

            std::vector<G3D::Vector3> l_Path;
            for (WaypointData const* l_WaypointData : *l_Waypoints)
                l_Path.push_back(G3D::Vector3(l_WaypointData->x, l_WaypointData->y, l_WaypointData->z));

            if (l_Path.empty())
                return;

            Position l_Pos;
            l_Pos.VectorToPosition(l_Path[0]);
            l_Pos.m_orientation = (*l_Waypoints)[0]->orientation;
            Creature* l_Creature = nullptr;
            if (l_EventEntry.Flags & CosmeticEventFlags::CosmeticSummonByPlayer)
            {
                Player* l_Player = nullptr;
                Creature* l_Summoner = nullptr;

                if (IS_PLAYER_GUID(p_OriginGuid))
                    l_Player = ObjectAccessor::GetObjectInMap(p_OriginGuid, instance, (Player*)nullptr);
                else if (p_SummonsFromGuid && IS_UNIT_GUID(p_OriginGuid))
                    l_Summoner = instance->GetCreature(p_OriginGuid);
                else
                {
                    Unit* l_This = instance->GetCreature(p_OriginGuid);
                    if (!l_This)
                        return;

                    l_Player = l_This->FindNearestPlayer(300.0f);
                }

                if (l_Player)
                {
                    if ((l_Creature = l_Player->SummonCreature(l_EventEntry.NpcEntry, l_Pos)))
                        l_Creature->SetVisible(true);
                }
                else if (l_Summoner)
                {
                    if ((l_Creature = l_Summoner->SummonCreature(l_EventEntry.NpcEntry, l_Pos)) && l_Summoner->IsAIEnabled)
                        l_Summoner->AI()->JustSummonedCosmetic(l_Creature, l_EventEntry.EventId);
                }
                else
                    l_Creature = instance->SummonCreature(l_EventEntry.NpcEntry, l_Pos);
            }
            else
                l_Creature = instance->SummonCreature(l_EventEntry.NpcEntry, l_Pos);

            if (!l_Creature)
                return;

            l_Creature->setActive(true);
            l_Creature->SetCosmetic(true);

            l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
            l_Creature->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

            if (l_EventEntry.Speed)
                l_Creature->SetSpeed(static_cast<UnitMoveType>(l_EventEntry.MovementType), l_EventEntry.Speed);

            l_Creature->AddDelayedEvent([this, l_Creature, l_EventEntry, l_Path, l_Pos]() -> void
            {
                Position l_Position = l_Pos;

                l_Creature->GetMotionMaster()->Clear();

                int32 l_Duration = 0;

                if (l_EventEntry.Flags & CosmeticEventFlags::CosmeticActsAsWaypoints)
                    l_Creature->GetMotionMaster()->MovePath(l_EventEntry.PathId, false, irand(l_EventEntry.RandomDistMin, l_EventEntry.RandomDistMax), irand(l_EventEntry.RandomDistMin, l_EventEntry.RandomDistMax));
                else
                    l_Duration = l_Creature->GetMotionMaster()->MoveCosmeticPath(0, &l_EventEntry, l_Path);

                if (l_Creature->IsAIEnabled)
                    l_Creature->AI()->StartCosmeticEvent(l_EventEntry, l_Duration);

                if (l_EventEntry.DespawnTime >= 0)
                    l_Creature->DespawnOrUnsummon(l_Duration + l_EventEntry.DespawnTime);

                if (l_EventEntry.Addons.empty())
                    return;

                float l_MainDist = 0.0f;
                Position l_A;
                Position l_B;
                for (uint32 l_I = 0; l_I < l_Path.size() - 1; ++l_I)
                {
                    l_A.VectorToPosition(l_Path[l_I]);
                    l_B.VectorToPosition(l_Path[l_I + 1]);
                    l_MainDist += l_A.GetExactDist2d(&l_B);
                }

                for (CosmeticEventAddonEntry const& l_Addon : l_EventEntry.Addons)
                {
                    std::vector<G3D::Vector3> l_Parallel;
                    BuildParallelPath(l_Path, l_Addon.Distance, l_Creature, l_Parallel, l_Addon.Angle < 0.0f);

                    l_Position.VectorToPosition(l_Parallel[0]);
                    Creature* l_Follower = l_Creature->GetMap()->SummonCreature(l_Addon.FollowingNpc, l_Position);
                    if (!l_Follower)
                        return;

                    uint32 l_StepTime = 0;
                    float l_MainSpeed = l_Creature->GetSpeed(static_cast<UnitMoveType>(l_EventEntry.MovementType));
                    for (uint32 l_I = 0; l_I < l_Path.size() - 1; ++l_I)
                    {
                        l_A.VectorToPosition(l_Path[l_I]);
                        l_B.VectorToPosition(l_Path[l_I + 1]);
                        float l_StepDist = l_A.GetExactDist2d(&l_B);

                        l_A.VectorToPosition(l_Parallel[l_I]);
                        l_B.VectorToPosition(l_Parallel[l_I + 1]);
                        float l_FollowerStepDist = l_A.GetExactDist2d(&l_B);

                        l_Follower->AddDelayedEvent([this, l_EventEntry, l_B, l_StepDist, l_FollowerStepDist, l_Follower]() -> void
                        {
                            l_Follower->SetWalk(true);
                            l_Follower->SetSpeed(static_cast<UnitMoveType>(l_EventEntry.MovementType), l_EventEntry.Speed / l_StepDist * l_FollowerStepDist);
                            l_Follower->GetMotionMaster()->MovePoint(0, l_B, false);
                        }, l_StepTime);

                        l_StepTime += l_StepDist / (l_MainSpeed / static_cast<float>(IN_MILLISECONDS));

                        /// Debugging
                        /*
                        l_Pos.VectorToPosition(l_Parallel[l_I]);
                        AddTimedDelayedOperation(l_I * 500, [this, l_Pos, l_Creature]() -> void
                        {
                            Creature* l_aCreature = l_Creature->SummonCreature(31146, l_Pos, TEMPSUMMON_TIMED_DESPAWN, 30 * IN_MILLISECONDS);
                            l_aCreature->SetObjectScale(0.5f);
                        });*/
                    }

                    if (l_Follower->IsAIEnabled)
                        l_Follower->AI()->StartCosmeticEvent(l_EventEntry, l_Duration);

                    if (l_EventEntry.DespawnTime >= 0)
                        l_Follower->DespawnOrUnsummon(l_Duration + l_EventEntry.DespawnTime);
                }
            }, l_EventEntry.MoveTime);
        });
    }
}
//////////////////////////////////////////////////////////////////////////

class EncounterScript_Global : public EncounterScript
{
    public:
        EncounterScript_Global() : EncounterScript("EncounterScript_PvE_Logs") { }

        void OnEncounterEnd(EncounterDatas const* p_EncounterDatas) override
        {
            uint64 l_AttemptID  = sObjectMgr->GenerateEncounterAttemptID();
            uint8 l_Index       = 0;

            SQLTransaction l_Transaction    = CharacterDatabase.BeginTransaction();
            PreparedStatement* l_Statement  = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_ENCOUNTER_DATA);

            l_Statement->setUInt64(l_Index++, l_AttemptID);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->Expansion);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->RealmID);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->GuildID);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->GuildFaction);
            l_Statement->setString(l_Index++, p_EncounterDatas->GuildName);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->MapID);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->EncounterID);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->DifficultyID);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->StartTime);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->CombatDuration);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->EndTime);
            l_Statement->setBool(l_Index++, p_EncounterDatas->Success);
            l_Statement->setUInt64(l_Index++, p_EncounterDatas->EncounterHealth);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->BattleRezCount);
            l_Statement->setUInt32(l_Index++, p_EncounterDatas->BloodlustCount);

            l_Transaction->Append(l_Statement);

            for (std::size_t l_I = 0; l_I < p_EncounterDatas->RosterDatas.size(); ++l_I)
            {
                RosterData const& l_Data = p_EncounterDatas->RosterDatas[l_I];

                l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_ENCOUNTER_ROSTER);
                l_Index     = 0;

                l_Statement->setUInt64(l_Index++, l_AttemptID);
                l_Statement->setString(l_Index++, l_Data.Name);
                l_Statement->setUInt32(l_Index++, l_Data.GuidLow);
                l_Statement->setUInt32(l_Index++, l_Data.Level);
                l_Statement->setUInt32(l_Index++, l_Data.Class);
                l_Statement->setUInt32(l_Index++, l_Data.SpecID);
                l_Statement->setUInt32(l_Index++, l_Data.Role);
                l_Statement->setUInt32(l_Index++, l_Data.ItemLevel);

                l_Transaction->Append(l_Statement);
            }

            CharacterDatabase.CommitTransaction(l_Transaction);
        }
};

#ifndef __clang_analyzer__
void AddSC_EncounterScripts()
{
    new EncounterScript_Global();
}
#endif
