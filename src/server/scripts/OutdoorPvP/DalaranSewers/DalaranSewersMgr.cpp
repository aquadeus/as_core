////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "DalaranSewersMgr.hpp"

OutdoorPvPDalaran::OutdoorPvPDalaran()
{
    m_TypeId = OutdoorPvPTypes::OUTDOOR_PVP_DALARAN;

    m_ActivatedFFA      = false;
    m_CanSwitchFFAState = true;

    m_PhaseSwitchingTimer         = 0;
    m_RareMobSpawningTimer        = 0;
    m_PlayersPvPStateCheckTimer   = 0;
    m_ItemTempSummonTimers        = 0;

    m_ControllerGUID              = 0;
    m_BigTreasureVisualGUID       = 0;
}

void OutdoorPvPDalaran::SwitchFFAState(bool p_State)
{
    m_ActivatedFFA        = p_State;
    m_CanSwitchFFAState   = false;
    m_PhaseSwitchingTimer = 10 * MINUTE * IN_MILLISECONDS;
    Map* l_Map = Map::GetCurrentMapThread();

    if (m_ActivatedFFA == false && l_Map)
    {
        for (uint64 l_GUID : m_PlayersInMap)
        {
            if (Player* l_Player = sObjectAccessor->FindPlayer(l_GUID))
            {
                l_Player->RemoveAura(eDalaranSewersSpells::AuraNoGuards);
                l_Player->RemoveAura(eDalaranSewersSpells::AuraHiredGuard);
                l_Player->AddAura(eDalaranSewersSpells::AuraSewerGuard, l_Player);
                l_Player->DisableForceFFA();
                l_Player->ForcePvPArea();
                l_Player->UpdatePvPState(true);
                l_Player->SetPhaseMask(l_Player->GetPhaseMask() | eDalaranPhasemasks::PhaseGuards, true);
                l_Player->RemoveAllMinionsByEntry(eCreatures::NpcDalaranBodyGuard);
            }
        }

        for (uint64 l_GobGUID : m_GobsGUIDs)
        {
            if (GameObject* l_GameObject = l_Map->GetGameObject(l_GobGUID))
                l_GameObject->Delete();
        }

        if (GameObject* l_Gob = l_Map->GetGameObject(m_BigTreasureVisualGUID))
            l_Gob->Delete();

        m_BigTreasureVisualGUID = 0;

        for (uint64 l_CGUID : m_RareGUIDs)
        {
            if (Creature* l_Creature = l_Map->GetCreature(l_CGUID))
                l_Creature->DespawnOrUnsummon();
        }

    }
    else
    {
        for (uint64 l_GUID : m_PlayersInMap)
        {
            if (Player* l_Player = sObjectAccessor->FindPlayer(l_GUID))
            {
                l_Player->RemoveAura(eDalaranSewersSpells::AuraSewerGuard);
                l_Player->AddAura(eDalaranSewersSpells::AuraNoGuards, l_Player);
                l_Player->ForceFFA();
                l_Player->UnForcePvPArea();
                l_Player->UpdatePvPState(true);
                l_Player->SetPhaseMask(l_Player->GetPhaseMask() & ~eDalaranPhasemasks::PhaseGuards, true);
            }
        }
 
        SpawnChests();
    }
}

bool OutdoorPvPDalaran::HasPlayer(Player* p_Player) const
{
    if (p_Player != nullptr && p_Player->GetTeamId() < 2)
        return std::find(m_PlayersInMap.begin(), m_PlayersInMap.end(), p_Player->GetGUID()) != m_PlayersInMap.end();

    return false;
}

void OutdoorPvPDalaran::HandlePlayerEnterMap(Player* p_Player, uint32 p_MapID)
{
    if (p_MapID == eGpsData::DalaranSewersMapID)
        m_PlayersInMap.insert(p_Player->GetGUID());
}

void OutdoorPvPDalaran::HandlePlayerLeaveMap(Player* p_Player, uint32 p_MapID)
{
    if (p_MapID == eGpsData::DalaranSewersMapID)
    {
        m_PlayersInMap.erase(p_Player->GetGUID());

        p_Player->RemoveAura(eDalaranSewersSpells::AuraNoGuards);
        p_Player->RemoveAura(eDalaranSewersSpells::AuraSewerGuard);
        p_Player->RemoveAura(eDalaranSewersSpells::AuraHiredGuard);

        p_Player->DisableForceFFA();
        p_Player->UpdatePvPState(false);

        p_Player->SetUInt32Value(PLAYER_FIELD_OVERRIDE_ZONE_PVPTYPE, 0);
    }
}

void OutdoorPvPDalaran::HandlePlayerUpdateArea(Player* p_Player, uint32 p_OldAreaID, uint32 p_NewAreaID)
{
    uint64 l_GUID = p_Player->GetGUID();

    /// Enter Sewers
    if (std::find(g_DalaranSewersAreas.begin(), g_DalaranSewersAreas.end(), p_OldAreaID) == g_DalaranSewersAreas.end() &&
        std::find(g_DalaranSewersAreas.begin(), g_DalaranSewersAreas.end(), p_NewAreaID) != g_DalaranSewersAreas.end())
    {
        m_PlayersInMap.insert(l_GUID);

        p_Player->AddMapTask(eTaskType::TaskSwitchToPhasedMap, [=]() -> void
        {
            if (p_Player->IsInWorld())
            {
                p_Player->SwitchToPhasedMap(1502, nullptr, [=](Player* p_Player) -> void
                {
                    if (GetFFAState())
                    {
                        p_Player->RemoveAura(eDalaranSewersSpells::AuraSewerGuard);
                        p_Player->AddAura(eDalaranSewersSpells::AuraNoGuards, p_Player);
                        p_Player->ForceFFA();
                        p_Player->UpdatePvPState(true);
                    }
                    else
                    {
                        p_Player->RemoveAura(eDalaranSewersSpells::AuraNoGuards);
                        p_Player->RemoveAura(eDalaranSewersSpells::AuraHiredGuard);
                        p_Player->AddAura(eDalaranSewersSpells::AuraSewerGuard, p_Player);
                        p_Player->DisableForceFFA();
                        p_Player->UnForcePvPArea();
                        p_Player->UpdatePvPState(true);
                    }
                });
            }
            else
                p_Player->RemoveTaskInProgress();

        });
    }
    /// Leave Sewers
    else if (std::find(g_DalaranSewersAreas.begin(), g_DalaranSewersAreas.end(), p_NewAreaID) == g_DalaranSewersAreas.end() &&
        std::find(g_DalaranSewersAreas.begin(), g_DalaranSewersAreas.end(), p_OldAreaID) != g_DalaranSewersAreas.end())
    {
        m_PlayersInMap.erase(p_Player->GetGUID());

        p_Player->AddMapTask(eTaskType::TaskSwitchToPhasedMap, [=]() -> void
        {
            if (p_Player->IsInWorld())
            {
                p_Player->SwitchToPhasedMap(1220, nullptr, [=](Player* l_Player) -> void
                {
                    l_Player->RemoveAura(eDalaranSewersSpells::AuraNoGuards);
                    l_Player->RemoveAura(eDalaranSewersSpells::AuraSewerGuard);
                    l_Player->RemoveAura(eDalaranSewersSpells::AuraHiredGuard);

                    l_Player->DisableForceFFA();
                    l_Player->UnForcePvPArea();
                    p_Player->UpdatePvPState(true);

                    l_Player->SetUInt32Value(PLAYER_FIELD_OVERRIDE_ZONE_PVPTYPE, 0);
                });
            }
            else
                p_Player->RemoveTaskInProgress();
        });
    }

    /// Switch from FFA to sanctuary
    if (p_OldAreaID == eGpsData::DalaranUnderbellyAreaFFA && p_NewAreaID == eGpsData::DalaranSanctuaryArea)
    {
        p_Player->AddAura(eDalaranSewersSpells::AuraFairGame, p_Player);

        /// Probably needs to prevent override, to keep getting Fair Game working properly and make FFA doable in sanctuary zone...
        /// p_Player->SetUInt32Value(PLAYER_FIELD_OVERRIDE_ZONE_PVPTYPE, 0);
    }
    /// Switch from sanctuary to FFA...
    else if (p_NewAreaID == eGpsData::DalaranUnderbellyAreaFFA && p_OldAreaID == eGpsData::DalaranSanctuaryArea)
    {
        p_Player->SetUInt32Value(PLAYER_FIELD_OVERRIDE_ZONE_PVPTYPE, 2);
        p_Player->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);
        p_Player->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
    }
}

void OutdoorPvPDalaran::HandleKill(Player* p_KillerPlayer, Unit* p_KilledUnit)
{
    if (p_KillerPlayer->HasAura(eDalaranSewersSpells::AuraNoGuards) && !p_KillerPlayer->GetGroup() && p_KilledUnit->IsPlayer())
    {
        m_PlayersInMap.erase(p_KilledUnit->GetGUID());
        p_KillerPlayer->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_SLIGHTLESS_EYE, urand(15, 50), false);
    }
}

void OutdoorPvPDalaran::HandleLootReleased(Player* p_Player, uint64 p_GUID)
{
    uint32 l_GobEntry = GUID_ENPART(p_GUID);

    switch (l_GobEntry)
    {
        case eGameObjects::GobUnderbellyTreasure:
        case eGameObjects::GobUnderbellyCache:
        case eGameObjects::GobUnderbellyBooty:
            if (GameObject* l_Gob = GetGameObjectFromMap(p_GUID, eGpsData::DalaranSewersMapID))
                l_Gob->Delete();

            /// Also delete the visual effect for the treasure
            if (l_GobEntry == eGameObjects::GobUnderbellyTreasure)
            {
                if (GameObject* l_Gob = GetGameObjectFromMap(m_BigTreasureVisualGUID, eGpsData::DalaranSewersMapID))
                    l_Gob->Delete();
            }
            break;
    }
}

void OutdoorPvPDalaran::HandlePlayerResurrects(Player* p_Player, uint32 p_ZoneID)
{
    if (p_ZoneID == eGpsData::DalaranZoneID)
    {
        m_PlayersInMap.insert(p_Player->GetGUID());

        if (GetFFAState())
        {
            p_Player->RemoveAura(eDalaranSewersSpells::AuraSewerGuard);
            p_Player->AddAura(eDalaranSewersSpells::AuraNoGuards, p_Player);
            p_Player->ForceFFA();
            p_Player->ForcePvPArea();
            p_Player->UpdatePvPState(true);
        }
        else
        {
            p_Player->RemoveAura(eDalaranSewersSpells::AuraNoGuards);
            p_Player->RemoveAura(eDalaranSewersSpells::AuraHiredGuard);
            p_Player->AddAura(eDalaranSewersSpells::AuraSewerGuard, p_Player);
            p_Player->DisableForceFFA();
            p_Player->UnForcePvPArea();
            p_Player->UpdatePvPState(true);
        }
    }
}

void OutdoorPvPDalaran::SpawnChests()
{
    Map* l_Map = Map::GetCurrentMapThread();

    /// Select 12 rancom entries
    std::vector<Position*> l_TempPosList;
    uint16 l_WarningCounter = 0;
    bool l_Spawned = false;

    do
    {
        Position* const l_PotentialPos = &g_ChestSpawnCoords[urand(0, g_ChestSpawnCoords.size() - 1)];
        bool l_IsSafe = true;

        for (Position* l_Pos : l_TempPosList)
        {
            if (l_Pos->IsNearPosition(l_PotentialPos, 0.01f))
            {
                l_IsSafe = false;
                break;
            }
        }

        if (l_IsSafe)
            l_TempPosList.push_back(const_cast<Position*>(l_PotentialPos));

        ++l_WarningCounter;

    } while (l_TempPosList.size() < 12 && l_WarningCounter < 80);


    /// Spawning gobs Process

    if (l_Map)
    {
        if (Creature* l_Controller = l_Map->GetCreature(m_ControllerGUID))
        {
            uint32 l_GobType;

            if (l_Spawned)
            {
                for (uint8 l_Itr = 0; l_Itr < 12; l_Itr++)
                {
                    switch (l_Itr)
                    {
                        case 0:
                            l_GobType = eGameObjects::GobUnderbellyTreasure;
                            break;
                        case 1:
                        case 2:
                        case 3:
                        case 4: ///< 4 spawns
                            l_GobType = eGameObjects::GobUnderbellyBooty;
                            break;
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11: ///< 7 spawns
                            l_GobType = eGameObjects::GobUnderbellyCache;
                            break;
                        default:
                            break;
                    }

                    if (GameObject* l_Gob = l_Controller->SummonGameObject(l_GobType, *l_TempPosList[l_Itr], Position(0.0f, 0.0f, 0.0f, 0.0f)))
                        m_GobsGUIDs.insert(l_Gob->GetGUID());

                    if (l_GobType == eGameObjects::GobUnderbellyTreasure)
                    {
                        if (GameObject* l_Gob = l_Controller->SummonGameObject(eGameObjects::GobTrapsTreasureVisual, *l_TempPosList[l_Itr], Position(0.0f, 0.0f, 0.0f, 0.0f)))
                            m_BigTreasureVisualGUID = l_Gob->GetGUID();
                    }
                }

                l_Spawned = true;
            }
        }
    }
}

void OutdoorPvPDalaran::FillCustomPvPLoots(Player* p_Looter, Loot& p_Loot, uint64 p_Container)
{
    /// If killer has aura 219725, loot candles (http://www.wowhead.com/npc=110642/fizzi-liverzapper#comments)

    if (!p_Looter->HasAura(eDalaranSewersSpells::SpellAuraExperimentalPotion))
        return;

    /// Need to add loot but can't find ItemID at all yet
}

void OutdoorPvPDalaran::RareCreatureKilled(uint32 p_Entry, uint64 p_GUID)
{
    m_RareMobsSpawned.erase(std::remove(m_RareMobsSpawned.begin(), m_RareMobsSpawned.end(), p_Entry), m_RareMobsSpawned.end());
    DelCreatureWithGUID(p_GUID);
}

bool OutdoorPvPDalaran::Update(uint32 p_Diff)
{
    Map* l_Map = Map::GetCurrentMapThread();

    if (m_PhaseSwitchingTimer)
    {
        if (m_PhaseSwitchingTimer <= p_Diff)
        {
            m_CanSwitchFFAState = true;
            m_PhaseSwitchingTimer = 0;
        }
        else
            m_PhaseSwitchingTimer -= p_Diff;
    }

    if (m_RareMobSpawningTimer)
    {
        if (m_RareMobSpawningTimer <= p_Diff)
        {
            if (m_RareMobsSpawned.size() < 3 && l_Map)
            {
                if (Creature* l_Controller = l_Map->GetCreature(m_ControllerGUID))
                {
                    std::map<uint32, uint64>::reverse_iterator l_Itr = m_Creatures.rbegin();
                    uint32 l_SpawnEntry = 0;

                    do
                    {
                        l_SpawnEntry = g_RareMobSEntries[urand(0, g_RareMobSEntries.size() - 1)];
                    } while (std::find(m_RareMobsSpawned.begin(), m_RareMobsSpawned.end(), l_SpawnEntry) != m_RareMobsSpawned.end());

                    if (Creature* l_NewRare = l_Controller->SummonCreature(l_SpawnEntry, g_RareMobsSpawnLocs[l_SpawnEntry]))
                        m_RareGUIDs.insert(l_NewRare->GetGUID());

                    m_RareMobsSpawned.push_back(l_SpawnEntry);
                }
            }

            m_RareMobSpawningTimer = urand(5 * MINUTE * IN_MILLISECONDS, 8 * MINUTE * IN_MILLISECONDS);
        }
        else
            m_RareMobSpawningTimer -= p_Diff;
    }

    if (m_PlayersPvPStateCheckTimer)
    {
        if (m_PlayersPvPStateCheckTimer <= p_Diff)
        {
            if (m_ActivatedFFA == false)
            {
                for (uint64 l_GUID : m_PlayersInMap)
                {
                    if (Player* l_Player = sObjectAccessor->FindPlayer(l_GUID))
                    {
                        switch (l_Player->GetAreaId())
                        {
                            case eGpsData::DalaranSanctuaryArea:
                            case eGpsData::DalaranUnderbellyAreaFFA:
                                l_Player->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
                                l_Player->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);
                                break;
                            default:
                                break;
                        }

                        l_Player->UnForcePvPArea();
                        l_Player->DisableForceFFA();
                        l_Player->UpdatePvPState(false);
                    }
                }
            }
            else
            {
                for (uint64 l_GUID : m_PlayersInMap)
                {
                    if (Player* l_Player = sObjectAccessor->FindPlayer(l_GUID))
                    {
                        switch (l_Player->GetAreaId())
                        {
                            case eGpsData::DalaranSanctuaryArea:
                                if (!l_Player->HasAura(eDalaranSewersSpells::AuraFairGame))
                                {
                                    l_Player->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
                                    l_Player->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);
                                }
                                else if (l_Player->HasAura(eDalaranSewersSpells::AuraFairGame))
                                {
                                    l_Player->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
                                    l_Player->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);
                                }
                                break;
                            case eGpsData::DalaranUnderbellyAreaFFA:
                                if (!l_Player->HasAura(eDalaranSewersSpells::AuraHiredGuard))
                                {
                                    l_Player->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
                                    l_Player->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);
                                }
                                break;
                            default:
                                break;
                        }

                        l_Player->ForcePvPArea();
                        l_Player->ForceFFA();
                        l_Player->UpdatePvPState(true);
                    }
                }
            }

            m_PlayersPvPStateCheckTimer = 2500;
        }
        else
            m_PlayersPvPStateCheckTimer -= p_Diff;
    }

    if (m_ItemTempSummonTimers)
    {
        if (m_ItemTempSummonTimers <= p_Diff)
            m_ItemTempSummonTimers = 0;
        else
            m_ItemTempSummonTimers -= p_Diff;
    }

    return OutdoorPvP::Update(p_Diff);
}

bool OutdoorPvPDalaran::SetupOutdoorPvP()
{
    RegisterZone(eGpsData::DalaranSewersMapID, eGpsData::DalaranZoneID);
    RegisterZone(eGpsData::DalaranSewersMapID, eGpsData::DalaranWorldZoneID);

    if (Map* l_Map = sMapMgr->CreateBaseMap(eGpsData::DalaranSewersMapID, 0))
        sMapMgr->GetZoneMapsList(eGpsData::DalaranSewersMapID, 0)->push_back(l_Map);

    m_PlayersPvPStateCheckTimer = 1500;
    m_RareMobSpawningTimer      = urand(5, 8) * MINUTE * IN_MILLISECONDS;

    return true;
}

class OutdoorPvP_DalaranSewers : public OutdoorPvPScript
{
    public:

        OutdoorPvP_DalaranSewers() : OutdoorPvPScript("OutdoorPvP_DalaranSewers") { }

        OutdoorPvP* GetOutdoorPvP() const
        {
            return new OutdoorPvPDalaran();
        }
};

/// Quests buffs handling
class playerScript_dalaransewers_quests_rewards : public PlayerScript
{
    public:
        playerScript_dalaransewers_quests_rewards() : PlayerScript("playerScript_dalaransewers_quests_rewards") { }

        enum eQuests
        {
            QuestExperimentalPotion1 = 43473,
            QuestExperimentalPotion2 = 43474,
            QuestExperimentalPotion3 = 43475,
            QuestExperimentalPotion4 = 43476,
            QuestExperimentalPotion5 = 43477,
            QuestExperimentalPotion6 = 43478,
        };

        enum eSpellRewards
        {
            ExperimentalPotionTuskarr      = 219622, ///< Transform to 89017 + necessary to fish sightless eyes (currency) in open waters from sewers
            ExperimentalPotionHarpy        = 219723, ///< Transform to 91121
            ExperimentalPotionKobold       = 219725, ///< Transform to 103177 + SPELL_AURA_ANIM_REPLACEMENT_SET (556)
            ExperimentalPotionDryad        = 219726, ///< Transform to 105937 + SPELL_AURA_ANIM_REPLACEMENT_SET (555)
            NaturesCharge                  = 219734, ///< Grans use of ability 219735 (homonymy)
            ExperimentalPotionNightFallen1 = 219726, ///< Transform to 109563 + UPDATE_ZONE_AURAS_AND_PHASES
            ExperimentalPotionNightFallen2 = 219841, ///< Transform to 109826 + UPDATE_ZONE_AURAS_AND_PHASES + Screen_effect
            ExperimentalPotionNightFallen3 = 219842, ///< Transform to 109011 + UPDATE_ZONE_AURAS_AND_PHASES + Screen_effect
            ExperimentalPotionNightFallen4 = 219845, ///< SPELL_AURA_PERIODIC_DUMMY (1000 ms) (useless ?)
            ExperimentalPotionSkrog        = 219731, ///< Transform to 105298 + health x2 + damage x2
            ExperimentalPotionSlowMelee10  = 219862,
            ExperimentalPotionSlowMelee20  = 219863,
            ExperimentalPotionSlowMelee30  = 219864,
            ExperimentalPotionAuraDummy    = 219988  ///< Skrog ? Description
        };

        void OnQuestReward(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case QuestExperimentalPotion1:
                    p_Player->AddAura(eSpellRewards::ExperimentalPotionTuskarr, p_Player);
                    break;
                case QuestExperimentalPotion2:
                    p_Player->AddAura(eSpellRewards::ExperimentalPotionHarpy, p_Player);
                    break;
                case QuestExperimentalPotion3:
                    p_Player->AddAura(eSpellRewards::ExperimentalPotionKobold, p_Player);
                    break;
                case QuestExperimentalPotion4:
                    p_Player->AddAura(eSpellRewards::ExperimentalPotionDryad, p_Player);
                    p_Player->AddAura(eSpellRewards::NaturesCharge, p_Player);
                    break;
                case QuestExperimentalPotion5:
                    /// not that simple :D fuck :(
//                     if (p_Player->IsRangedDamageDealer(true))
//                         p_Player->AddAura(eSpellRewards::ExperimentalPotionNightFallen2, p_Player);
//                     else
//                         p_Player->AddAura(eSpellRewards::ExperimentalPotionNightFallen3, p_Player);

                    break;
                case QuestExperimentalPotion6:
                    p_Player->AddAura(eSpellRewards::ExperimentalPotionSkrog, p_Player);
                    break;
                default:
                    break;
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_DalaranSewersMgr()
{
    new OutdoorPvP_DalaranSewers();

    /// Custom Handling, Playerscripts
    new playerScript_dalaransewers_quests_rewards();
}
#endif
