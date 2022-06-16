////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "Creature.h"
#include "antorus_the_burning_throne.hpp"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Map.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "LFGMgr.h"
#include "Group.h"

DoorData const g_DoorData[] =
{
    { eGameObjects::GarothiRocks,               eData::DataGarothiWorldbreaker,         DoorType::DOOR_TYPE_PASSAGE,    BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GarothiRocksCollision,      eData::DataGarothiWorldbreaker,         DoorType::DOOR_TYPE_PASSAGE,    BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoAntoranHighCommandDoor,   eData::DataAntoranHighCommand,          DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoHasabelEncounterDoor,     eData::DataPortalKeeperHasabel,         DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoEonarEncounterDoor1,      eData::DataEonarTheLifeBinder,          DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoEonarEncounterDoor2,      eData::DataEonarTheLifeBinder,          DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoKingarothDoor,            eData::DataKinGaroth,                   DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoVarimathrasDoor1,         eData::DataVarimathras,                 DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoVarimathrasDoor2,         eData::DataVarimathras,                 DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoCovenShivarraDoor1,       eData::DataTheCovenOfShivarra,          DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoCovenShivarraDoor2,       eData::DataTheCovenOfShivarra,          DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoAggramarDoor,             eData::DataAggramar,                    DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE },
    { eGameObjects::GoAggramarBridge,           eData::DataAggramar,                    DoorType::DOOR_TYPE_SPAWN_HOLE, BoundaryType::BOUNDARY_NONE },
    { 0,                                        0,                                      DoorType::DOOR_TYPE_ROOM,       BoundaryType::BOUNDARY_NONE }
};

class instance_antorus_the_burning_throne : public InstanceMapScript
{
    public:
        instance_antorus_the_burning_throne() : InstanceMapScript("instance_antorus_the_burning_throne", 1712) { }

        struct instance_antorus_the_burning_throne_InstanceMapScript : public InstanceScript
        {
            instance_antorus_the_burning_throne_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                m_BatCount = 0;

                p_Map->SetObjectVisibility(SIZE_OF_GRIDS);
            }

            uint32 m_BatCount;

            std::map<uint32, uint64> m_UniqueObjectGUIDs;
            std::map<uint32, std::set<uint64>> m_MultipleNpcGuids;
            std::map<uint32, uint32> m_MiscData;

            uint64 m_EonarChestGuid = 0;
            uint64 m_ParaxisShipGuid = 0;

            uint64 m_MagniBronzebeard = 0;

            bool m_FirstWingClearedIntro = false;

            std::set<uint64> m_ClubfistBeastlordPack;
            uint32 m_uiDialogs[3];

            uint8 m_LastPulledBoss;

            bool m_ImonarSkipQuestDone = false;
            bool m_AggramarSkipQuest = false;

            uint32 m_DungeonID = eDungeonIDs::DungeonIDNone;

            enum eEventList
            {
                EventIntroConversation,
                EventGarothiAnnihilatorIntro,
                EventBrokenCliffEnabled,
                EventAntoranHighCommandIntro,
                EventAggramarOutro,

                MaxEvents
            };

            enum eSpells
            {
                ImageOfVelenAura    = 249105,
                AppearingFromPod    = 247963,
                TalonLaser          = 252756
            };

            enum eConversations
            {
                IntroConversation1          = 5511,
                IntroConversation2          = 5512,
                GarothiWorldbreakerOutro    = 5519,
                AntoranHighCommandIntro     = 5524,
                AntoranHighCommandOutro     = 5525,
                AggramarOutro               = 6127
            };

            enum eTimers
            {
                TimerEonarsFall,
                TimerImonarsFall,
                TimerKingarothFall,
                TimerHasabelsFall,

                TimerMax
            };

            std::array<bool, eEventList::MaxEvents> m_EventState =
            {{
                false,
                false,
                false,
                false,
                false
            }};

            std::array<IntervalTimer, eTimers::TimerMax> m_Timers;

            std::map<uint32, std::unordered_set<uint64>> m_PantheonSpells;

            void SpawnBat()
            {
                AddTimedDelayedOperation(urand(4 * IN_MILLISECONDS, 6 * IN_MILLISECONDS), [this]() -> void
                {
                    SpawnBat();
                });

                if (m_BatCount >= 15)
                    return;

                PlayCosmeticEvent(eCosmeticEvents::AntorusSpawnBat);

                m_BatCount++;
            }

            void SpawnLegionShip()
            {
                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    SpawnLegionShip();
                });

                PlayCosmeticEvent(eCosmeticEvents::AntorusSpawnLegionShip);
            }

            void SpawnArmy()
            {
                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    SpawnArmy();
                });

                PlayCosmeticEvent(eCosmeticEvents::AntorusSpawnArmy);
            }

            void SpawnPatrol()
            {
                AddTimedDelayedOperation(45 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    SpawnPatrol();
                });

                PlayCosmeticEvent(eCosmeticEvents::AntorusLightforgedPatrol);

            }

            bool CheckRequiredBosses(uint32 p_BossId, Player const* p_Player) const override
            {
                if (instance->IsLFR())
                    return true;

                if (!InstanceScript::CheckRequiredBosses(p_BossId, p_Player))
                    return false;

                switch (p_BossId)
                {
                    case eData::DataGarothiWorldbreaker:
                        return true;
                    case eData::DataFelhoundsOfSargeras:
                    case eData::DataAntoranHighCommand:
                        return GetBossState(eData::DataGarothiWorldbreaker) == EncounterState::DONE;
                    case eData::DataEonarTheLifeBinder:
                    case eData::DataPortalKeeperHasabel:
                        return m_DungeonID == eDungeonIDs::DungeonIDForbiddenDescent || GetBossState(eData::DataGarothiWorldbreaker) == EncounterState::DONE;
                    case eData::DataImonarTheSoulhunter:
                        return m_DungeonID == eDungeonIDs::DungeonIDForbiddenDescent || m_ImonarSkipQuestDone ||
                            (GetBossState(eData::DataFelhoundsOfSargeras) == EncounterState::DONE
                            && GetBossState(eData::DataAntoranHighCommand) == EncounterState::DONE
                            && GetBossState(eData::DataEonarTheLifeBinder) == EncounterState::DONE
                            && GetBossState(eData::DataPortalKeeperHasabel) == EncounterState::DONE);
                    case eData::DataKinGaroth:
                        return GetBossState(eData::DataImonarTheSoulhunter) == EncounterState::DONE;
                    case eData::DataVarimathras:
                    case eData::DataTheCovenOfShivarra:
                        return GetBossState(eData::DataKinGaroth) == EncounterState::DONE;
                    case eData::DataAggramar:
                        return m_AggramarSkipQuest || (GetBossState(eData::DataVarimathras) == EncounterState::DONE
                            && GetBossState(eData::DataTheCovenOfShivarra) == EncounterState::DONE);
                        return true;
                    case eData::DataArgusTheUnmaker:
                        return GetBossState(eData::DataAggramar) == EncounterState::DONE;
                    default:
                        break;
                }

                return false;
            }

            void SetData(uint32 p_Index, uint32 p_Value) override
            {
                switch (p_Index)
                {
                    case eData::DataFelBatDown:
                    {
                        m_BatCount--;
                        break;
                    }
                    case eData::DataAnnihilatorDeath:
                    {
                        PlayCosmeticEvent(eCosmeticEvents::AntorusAnnihilatorDeath);

                        if (Creature* l_Creature = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::BossGarothiWorldbreaker]))
                        {
                            if (l_Creature->IsAIEnabled)
                                l_Creature->AI()->DoAction(ACTION_1);
                        }

                        break;
                    }
                    case eData::DataActivateLegionShips:
                    {
                        SpawnLegionShip();
                        break;
                    }
                    case eData::DataDestroyKingarothDoor:
                    {
                        if (p_Value == 1) ///< Fast Activate door for Kingaroth
                        {
                            AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoKingarothAccessDoor)))
                                {
                                    l_Door->SendCustomAnim(1);
                                    l_Door->SendGameObjectActivateAnimKit(6132);
                                }

                                if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoKingarothAccessDoorColl)))
                                    l_Door->SetGoState(GOState::GO_STATE_ACTIVE);
                            });

                            break;
                        }

                        uint32 l_Time = 5 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            DoCastSpellOnPlayers(eSharedSpells::SpellVindicaarDestroysDoor);

                            if (Creature* l_Canon = instance->GetCreature(GetData64(eCreatures::NpcVindicaarCanon)))
                                l_Canon->CastSpell(l_Canon, eSharedSpells::SpellDestroyDoor);
                        });

                        l_Time += 7 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoKingarothAccessDoor)))
                                l_Door->SendCustomAnim(1);
                        });

                        l_Time += 3 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoKingarothAccessDoor)))
                                l_Door->SendGameObjectActivateAnimKit(6132);

                            if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoKingarothAccessDoorColl)))
                                l_Door->SetGoState(GOState::GO_STATE_ACTIVE);
                        });

                        break;
                    }
                    case eData::DataKinGarothIntro:
                    {
                        m_uiDialogs[0] = p_Value;
                        if (p_Value == DONE)
                            SaveToDB();
                        break;
                    }
                    case eData::DataTarneth:
                    {
                        m_uiDialogs[1] = p_Value;
                        if (p_Value == DONE)
                            SaveToDB();

                        if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoPreVarimathrasDoor)))
                            l_Door->SetGoState(GOState::GO_STATE_ACTIVE);

                        break;
                    }
                    case eData::DataVarimathrasIntro:
                    {
                        m_uiDialogs[2] = p_Value;
                        if (p_Value == DONE)
                            SaveToDB();
                        break;
                    }
                    case eData::DataClobex:
                    {
                        if (m_UniqueObjectGUIDs.find(eGameObjects::GoAntoranHighCommandElevator) != m_UniqueObjectGUIDs.end())
                        {
                            if (GameObject* l_Elevator = instance->GetGameObject(m_UniqueObjectGUIDs[eGameObjects::GoAntoranHighCommandElevator]))
                                l_Elevator->SetGoState(GOState::GO_STATE_TRANSPORT_ACTIVE);
                        }

                        m_MiscData[p_Index] = p_Value;

                        break;
                    }
                    default:
                        m_MiscData[p_Index] = p_Value;
                        break;
                }
            }

            void Initialize() override
            {
                SpawnBat();
                SpawnArmy();
                SetBossNumber(eData::DataMaxBosses);
                LoadDoorData(g_DoorData);

                for (uint8 j = 0; j < 3; ++j)
                    m_uiDialogs[j] = NOT_STARTED;

                m_LastPulledBoss = eData::DataGarothiWorldbreaker;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                switch (instance->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidNormal:
                    {
                        if (p_Player->IsQuestRewarded(eSkipQuests::SkipQuestImonarNormal))
                            m_ImonarSkipQuestDone = true;

                        if (p_Player->IsQuestRewarded(eSkipQuests::SkipQuestAggramarNormal))
                            m_AggramarSkipQuest = true;

                        break;
                    }
                    case Difficulty::DifficultyRaidHeroic:
                    {
                        if (p_Player->IsQuestRewarded(eSkipQuests::SkipQuestImonarHeroic))
                            m_ImonarSkipQuestDone = true;

                        if (p_Player->IsQuestRewarded(eSkipQuests::SkipQuestAggramarHeroic))
                            m_AggramarSkipQuest = true;

                        break;
                    }
                    case Difficulty::DifficultyRaidMythic:
                    {
                        if (p_Player->IsQuestRewarded(eSkipQuests::SkipQuestImonarMythic))
                            m_ImonarSkipQuestDone = true;

                        if (p_Player->IsQuestRewarded(eSkipQuests::SkipQuestAggramarMythic))
                            m_AggramarSkipQuest = true;

                        break;
                    }
                    default:
                        break;
                }

                if (m_DungeonID == eDungeonIDs::DungeonIDNone && instance->IsLFR())
                {
                    m_DungeonID = (p_Player->GetGroup() ? sLFGMgr->GetDungeon(p_Player->GetGroup()->GetGUID()) : eDungeonIDs::DungeonIDNone);

                    if (m_DungeonID == eDungeonIDs::DungeonIDHopesEnd)
                    {
                        if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoKingarothAccessDoorColl))) /// after immonar
                            l_Door->SetGoState(GOState::GO_STATE_ACTIVE);

                        if (Creature* l_Imonar = instance->GetCreature(GetData64(eCreatures::BossImonarTheSoulhunter)))
                        {
                            l_Imonar->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            l_Imonar->SetReactState(ReactStates::REACT_PASSIVE);
                            l_Imonar->SetVisible(false);
                        }
                    }
                }
            }

            void OnQuestRewarded(Player* p_Player, Quest const* p_Quest) override
            {
                switch (p_Quest->GetQuestId())
                {
                    case eSkipQuests::SkipQuestImonarNormal:
                    case eSkipQuests::SkipQuestImonarHeroic:
                    case eSkipQuests::SkipQuestImonarMythic:
                    {
                        m_ImonarSkipQuestDone = true;

                        auto const& l_Iter = m_MultipleNpcGuids.find(eCreatures::NpcLightforgedWarframeToImonar);
                        if (l_Iter == m_MultipleNpcGuids.end())
                            break;

                        for (uint64 const& l_Guid : l_Iter->second)
                        {
                            if (Creature* l_Warframe = instance->GetCreature(l_Guid))
                                l_Warframe->SetVisible(false);
                        }

                        break;
                    }
                    case eSkipQuests::SkipQuestAggramarNormal:
                    case eSkipQuests::SkipQuestAggramarHeroic:
                    case eSkipQuests::SkipQuestAggramarMythic:
                    {
                        m_AggramarSkipQuest = true;
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::NpcGarothiDecimator:
                    {
                        p_Creature->SetBig(true);
                        break;
                    }
                    case eCreatures::NpcHighExarchTuralyon:
                    case eCreatures::NpcImageOfProphetVelen:
                    case eCreatures::NpcArchmageKhadgar:
                    case eCreatures::NpcGarothiAnnihilator:
                    case eCreatures::NpcTheParaxis:
                    case eCreatures::NpcTheParaxisDummy:
                    case eCreatures::NpcVindicaarCanon:
                    case eCreatures::BossArgusTheUnmaker:
                    case eCreatures::NpcBeaconFromPantheon:
                    case eCreatures::NpcAmanThul:
                    case eCreatures::NpcGolganneth:
                    case eCreatures::NpcKhazgoroth:
                    case eCreatures::NpcEonar:
                    case eCreatures::NpcAggramar:
                    case eCreatures::NpcNorgannon:
                    case eCreatures::NpcMagniBronzebeard:
                    case eCreatures::NpcImageOfEonar:
                    case eCreatures::BossAdmiralSvirax:
                    case eCreatures::BossShatug:
                    {
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::BossEssenceOfEonar:
                    {
                        m_Timers[eTimers::TimerEonarsFall].SetInterval(200);
                        p_Creature->SetBig(true);
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::BossImonarTheSoulhunter:
                    {
                        m_Timers[eTimers::TimerImonarsFall].SetInterval(200);
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Creature->SetVisible(false);
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::BossPortalKeeperHasabel:
                    {
                        m_Timers[eTimers::TimerHasabelsFall].SetInterval(200);
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcDevastatorStalker:
                    case eCreatures::NpcLightforgedBarricade2:
                    {
                        m_MultipleNpcGuids[p_Creature->GetEntry()].insert(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::NpcLightforgedWarframeToImonar:
                    {
                        if (!m_ImonarSkipQuestDone && m_DungeonID != eDungeonIDs::DungeonIDForbiddenDescent)
                            p_Creature->SetVisible(false);

                        m_MultipleNpcGuids[p_Creature->GetEntry()].insert(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::NpcGatewayToXoroth:
                    case eCreatures::NpcGatewayFromXoroth:
                    case eCreatures::NpcGatewayToRancora:
                    case eCreatures::NpcGatewayFromRancora:
                    case eCreatures::NpcGatewayToNathreza:
                    case eCreatures::NpcGatewayFromNathreza:
                    case eCreatures::NpcPortalXoroth:
                    case eCreatures::NpcPortalRancora:
                    case eCreatures::NpcPortalNathreza:
                    case eCreatures::NpcFlamesOfXoroth:
                    case eCreatures::NpcEverburningFlames:
                    case eCreatures::NpcEternalDarkness:
                    case eCreatures::NpcAcidOfRancora:
                    case eCreatures::NpcShadowsOfNathreza:
                    case eCreatures::NpcMagniAntoranFelguard:
                    case eCreatures::NpcDestroyedShip:
                    {
                        p_Creature->DisableEvadeMode();
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        break;
                    }
                    case eCreatures::NpcKingaroth:
                    {
                        if (p_Creature->isDead() && !p_Creature->IsNeedRespawn() && GetBossState(eData::DataKinGaroth) != EncounterState::DONE)
                        {
                            AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                            {
                                SetBossState(eData::DataKinGaroth, EncounterState::DONE);
                            });
                        }

                        m_Timers[eTimers::TimerKingarothFall].SetInterval(500);
                        p_Creature->SetBig(true);
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcTeleportPodTheExhaust:
                    {
                        if (p_Creature->ComputeLinkedId() != "36D02E3CB7C89311DBF30552EDC5C400D3ED5890")
                            break;

                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        if (GetBossState(eData::DataKinGaroth) != DONE)
                            p_Creature->SetVisible(false);

                        break;
                    }
                    case eCreatures::NpcSlobberingFiendGround:
                    {
                        if (!p_Creature->isDead() && p_Creature->FindNearestCreature(eCreatures::NpcClubfistBeastlord, 20.0f))
                            m_ClubfistBeastlordPack.insert(p_Creature->GetGUID());

                        break;
                    }
                    case eCreatures::NpcClubfistBeastlord:
                    {
                        if (p_Creature->isDead())
                        {
                            auto l_Itr = p_Creature->GetMap()->GetPlayers().begin();
                            if (l_Itr != p_Creature->GetMap()->GetPlayers().end())
                            {
                                if (Player* l_Player = p_Creature->GetMap()->GetPlayers().begin()->getSource())
                                    PlayCosmeticEvent(eCosmeticEvents::ClubfistBeastlordDefeated, l_Player->GetGUID());
                            }

                            break;
                        }

                        std::list<Creature*> l_SlobberingFiends;
                        p_Creature->GetCreatureListWithEntryInGrid(l_SlobberingFiends, eCreatures::NpcSlobberingFiendGround, 20.0f);
                        for (Creature* l_Creature : l_SlobberingFiends)
                        {
                            if (!l_Creature->isDead())
                                m_ClubfistBeastlordPack.insert(l_Creature->GetGUID());
                        }

                        m_ClubfistBeastlordPack.insert(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::BossGarothiWorldbreaker:
                    {
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();

                        if (p_Creature->isDead())
                        {
                            auto l_Itr = p_Creature->GetMap()->GetPlayers().begin();
                            if (l_Itr != p_Creature->GetMap()->GetPlayers().end())
                            {
                                if (Player* l_Player = p_Creature->GetMap()->GetPlayers().begin()->getSource())
                                    PlayCosmeticEvent(eCosmeticEvents::AntorusWorldbreakerDeath, m_UniqueObjectGUIDs[eCreatures::BossGarothiWorldbreaker]);
                            }
                        }
                        break;
                    }
                    case eCreatures::NpcVarimathras:
                    {
                        if (p_Creature->isDead() && !p_Creature->IsNeedRespawn() && GetBossState(eData::DataVarimathras) != EncounterState::DONE)
                        {
                            AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void ///< with delay for loading Gameobjects
                            {
                                SetBossState(eData::DataVarimathras, EncounterState::DONE);
                            });
                        }

                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcAsaraMotherOfNight:
                    {
                        if (p_Creature->isDead() && !p_Creature->IsNeedRespawn() && GetBossState(eData::DataTheCovenOfShivarra) != EncounterState::DONE)
                        {
                            AddTimedDelayedOperation(2100, [this]() -> void ///< with delay for loading Gameobjects
                            {
                                SetBossState(eData::DataTheCovenOfShivarra, EncounterState::DONE);
                            });
                        }

                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::BossAggramar:
                    {
                        if (p_Creature->isDead() && !p_Creature->IsNeedRespawn() && GetBossState(eData::DataAggramar) != EncounterState::DONE)
                        {
                            AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                            {
                                SetBossState(eData::DataAggramar, EncounterState::DONE);
                            });
                        }

                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcNouraMotherOfFlames:
                    case eCreatures::NpcDiimaMotherofGloom:
                    case eCreatures::NpcThurayaMotherOfTheCosmos:
                    {
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::NpcClobex:
                    {
                        m_UniqueObjectGUIDs[p_Creature->GetEntry()] = p_Creature->GetGUID();

                        if (p_Creature->getDeathState() != ALIVE)
                            SetData(eData::DataClobex, DONE);

                        break;
                    }
                    case eCreatures::NpcTarneth:
                    {
                        if (p_Creature->isDead() && !p_Creature->IsNeedRespawn() && GetData(eData::DataTarneth) != EncounterState::DONE)
                        {
                            AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void ///< Set Data with delay for loading Gameobjects
                            {
                                SetData(eData::DataTarneth, EncounterState::DONE);
                            });
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                auto l_Itr = std::find_if(std::begin(g_DoorData), std::end(g_DoorData), [&p_GameObject](DoorData const& p_Itr)
                {
                    return p_Itr.entry == p_GameObject->GetEntry();
                });

                if (l_Itr != std::end(g_DoorData))
                    InstanceScript::AddDoor(p_GameObject, true);

                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::GoParaxisShip:
                    {
                        m_UniqueObjectGUIDs[p_GameObject->GetEntry()] = p_GameObject->GetGUID();

                        /// Sniffed value
                        p_GameObject->SetDynamicValue(EGameObjectDynamicFields::GAMEOBJECT_DYNAMIC_FIELD_ENABLE_DOODAD_SETS, 0, 5);
                        break;
                    }
                    case eGameObjects::GoParaxisDoor1:
                    case eGameObjects::GoParaxisDoor2:
                    case eGameObjects::GoParaxisDoor3:
                    case eGameObjects::GoKingarothAccessDoor:
                    case eGameObjects::GoKingarothAccessDoorColl:
                    {
                        m_UniqueObjectGUIDs[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoPreVarimathrasDoor:
                    {
                        m_UniqueObjectGUIDs[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        if (GetData(eData::DataTarneth) == DONE)
                            p_GameObject->SetGoState(GOState::GO_STATE_ACTIVE);
                        break;
                    }
                    case eGameObjects::GoBlessingOfLife:
                    {
                        m_EonarChestGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GoAntoranHighCommandElevator:
                    {
                        m_UniqueObjectGUIDs[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        if (GetData(eData::DataClobex) != DONE)
                            p_GameObject->SetGoState(GOState::GO_STATE_TRANSPORT_STOPPED);
                        else
                            p_GameObject->SetGoState(GOState::GO_STATE_TRANSPORT_ACTIVE);

                        break;
                    }
                    case eGameObjects::GoSpoilsOfThePantheon:
                    {
                        m_UniqueObjectGUIDs[p_GameObject->GetEntry()] = p_GameObject->GetGUID();

                        if (GetBossState(eData::DataArgusTheUnmaker) == EncounterState::DONE)
                        {
                            bool l_DisabledLoots = sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID());
                            if (l_DisabledLoots && sObjectMgr->GetItemContextOverride(instance->GetId(), instance->GetDifficultyID()) != ItemContext::None)
                                l_DisabledLoots = false;

                            if (!l_DisabledLoots)
                                l_DisabledLoots = sObjectMgr->IsDisabledEncounter(eData::DataArgusEncounterID, instance->GetDifficultyID());

                            if (!l_DisabledLoots)
                                p_GameObject->SetRespawnTime(TimeConstants::WEEK);
                        }

                        break;
                    }
                    case eGameObjects::GoPreAgrammarDoor:
                    {
                        if (GetBossState(eData::DataTheCovenOfShivarra) == DONE && GetBossState(eData::DataVarimathras) == DONE)
                            p_GameObject->SetGoState(GO_STATE_ACTIVE);

                        m_UniqueObjectGUIDs[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* p_GameObject) override
            {
                auto l_Itr = std::find_if(std::begin(g_DoorData), std::end(g_DoorData), [&p_GameObject](DoorData const& p_Itr)
                {
                    return p_Itr.entry == p_GameObject->GetEntry();
                });

                if (l_Itr != std::end(g_DoorData))
                    InstanceScript::AddDoor(p_GameObject, false);
            }

            bool SetBossState(uint32 p_Id, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Id, p_State))
                {
                    FirstWingIntro();
                    TriggerAggramarOutro();
                    return false;
                }

                /// Register pulled Boss ID
                if (p_State == EncounterState::IN_PROGRESS && p_Id != m_LastPulledBoss)
                    m_LastPulledBoss = p_Id;

                switch (p_Id)
                {
                    case eData::DataGarothiWorldbreaker:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        PlayCosmeticEvent(eCosmeticEvents::AntorusWorldbreakerDeath, m_UniqueObjectGUIDs[eCreatures::BossGarothiWorldbreaker]);

                        AddTimedDelayedOperation(6 * IN_MILLISECONDS, [this]() -> void
                        {
                            Creature* l_Turalyon = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::NpcHighExarchTuralyon]);
                            if (!l_Turalyon)
                                return;

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::GarothiWorldbreakerOutro, l_Turalyon, nullptr, *l_Turalyon))
                                delete l_Conversation;
                        });

                        break;
                    }
                    case eData::DataAntoranHighCommand:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        AddTimedDelayedOperation(6 * IN_MILLISECONDS, [this]() -> void
                        {
                            Creature* l_Svirax = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::BossAdmiralSvirax]);
                            if (!l_Svirax)
                                return;

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::AntoranHighCommandOutro, l_Svirax, nullptr, *l_Svirax))
                                delete l_Conversation;
                        });

                        break;
                    }
                    case eData::DataEonarTheLifeBinder:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        bool l_DisabledLoots = sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID());
                        if (l_DisabledLoots && sObjectMgr->GetItemContextOverride(instance->GetId(), instance->GetDifficultyID()) != ItemContext::None)
                            l_DisabledLoots = false;

                        if (!l_DisabledLoots)
                            l_DisabledLoots = sObjectMgr->IsDisabledEncounter(eEncounterIDs::EncounterDefenseOfEonar, instance->GetDifficultyID());

                        if (!l_DisabledLoots)
                        {
                            if (GameObject* l_Chest = instance->GetGameObject(m_EonarChestGuid))
                                l_Chest->SetRespawnTime(604800);
                        }

                        break;
                    }
                    case eData::DataKinGaroth:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        if (Creature* l_Beacon = instance->GetCreature(GetData64(eCreatures::NpcTeleportPodTheExhaust)))
                            l_Beacon->SetVisible(true);

                        break;
                    }
                    case eData::DataArgusTheUnmaker:
                    {
                        switch (p_State)
                        {
                            case EncounterState::IN_PROGRESS:
                            {
                                if (Creature* l_Beacon = instance->GetCreature(GetData64(eCreatures::NpcBeaconFromPantheon)))
                                    l_Beacon->SetVisible(false);

                                break;
                            }
                            case EncounterState::DONE:
                            {
                                if (Creature* l_Turalyon = instance->GetCreature(GetData64(eCreatures::NpcHighExarchTuralyon)))
                                    l_Turalyon->DespawnOrUnsummon();

                                if (Creature* l_Khadgar = instance->GetCreature(GetData64(eCreatures::NpcArchmageKhadgar)))
                                    l_Khadgar->DespawnOrUnsummon();

                                instance->SummonCreature(eCreatures::NpcHighExarchTuralyon, { 3090.158f, -9640.691f, 70.47287f, 0.4112027f });
                                instance->SummonCreature(eCreatures::NpcArchmageKhadgar,    { 3094.088f, -9628.064f, 70.46185f, 5.6632220f });

                                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    DoCastSpellOnPlayers(eSharedSpells::SpellArgusLeavingMovie);
                                    DoCastSpellOnPlayers(eSharedSpells::SpellArgusOutroTeleport);
                                });

                                Map::PlayerList const& l_Players = instance->GetPlayers();
                                for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                                {
                                    if (Player* l_Player = l_Itr->getSource())
                                    {
                                        if (l_Player->isGameMaster())
                                            continue;

                                        l_Player->CombatStopWithPets();
                                    }
                                }

                                bool l_DisabledLoots = sObjectMgr->IsDisabledMap(instance->GetId(), instance->GetDifficultyID());
                                if (l_DisabledLoots && sObjectMgr->GetItemContextOverride(instance->GetId(), instance->GetDifficultyID()) != ItemContext::None)
                                    l_DisabledLoots = false;

                                if (!l_DisabledLoots)
                                    l_DisabledLoots = sObjectMgr->IsDisabledEncounter(eData::DataArgusEncounterID, instance->GetDifficultyID());

                                if (!l_DisabledLoots)
                                {
                                    if (GameObject* l_Chest = instance->GetGameObject(GetData64(eGameObjects::GoSpoilsOfThePantheon)))
                                        l_Chest->SetRespawnTime(TimeConstants::WEEK);
                                }

                                /// No break here so the Beacon will still be visible
                            }
                            case EncounterState::FAIL:
                            {
                                if (Creature* l_Beacon = instance->GetCreature(GetData64(eCreatures::NpcBeaconFromPantheon)))
                                    l_Beacon->SetVisible(true);

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eData::DataVarimathras:
                    {
                        auto l_EnableImmunityCoven = [this](bool p_Enable)
                        {
                            std::array<uint32, 4> l_Shivarras = {
                                NpcAsaraMotherOfNight,
                                NpcDiimaMotherofGloom,
                                NpcNouraMotherOfFlames,
                                NpcThurayaMotherOfTheCosmos,
                            };

                            for (uint32 l_Itr : l_Shivarras)
                            {
                                Creature* l_Shivarra = instance->GetCreature(GetData64(l_Itr));

                                if (l_Shivarra)
                                {
                                    if (p_Enable)
                                        l_Shivarra->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                    else
                                        l_Shivarra->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                }
                            }
                        };

                        if (p_State == IN_PROGRESS)
                            l_EnableImmunityCoven(true);
                        else
                            l_EnableImmunityCoven(false);

                        if (p_State == EncounterState::DONE && !instance->IsLFR())
                        {
                            if (GetBossState(eData::DataTheCovenOfShivarra) == DONE && GetBossState(eData::DataVarimathras) == DONE)
                                if (GameObject* l_GoPreAgrammar = instance->GetGameObject(GetData64(eGameObjects::GoPreAgrammarDoor)))
                                    l_GoPreAgrammar->SetGoState(GO_STATE_ACTIVE);
                        }

                        break;
                    }
                    case eData::DataTheCovenOfShivarra:
                    {
                        if (p_State != EncounterState::DONE || instance->IsLFR())
                            break;

                        if (GetBossState(eData::DataTheCovenOfShivarra) == DONE && GetBossState(eData::DataVarimathras) == DONE)
                            if (GameObject* l_GoPreAgrammar = instance->GetGameObject(GetData64(eGameObjects::GoPreAgrammarDoor)))
                                l_GoPreAgrammar->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case eData::DataAggramar:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        TriggerAggramarOutro();
                        break;
                    }
                    default:
                        break;
                }

                FirstWingIntro();

                return true;
            }

            uint64 GetData64(uint32 p_ID) override
            {
                auto const& l_Iter = m_MultipleNpcGuids.find(p_ID);
                if (l_Iter != m_MultipleNpcGuids.end())
                {
                    auto l_Guid = l_Iter->second.begin();
                    std::advance(l_Guid, urand(0, l_Iter->second.size() - 1));
                    return *l_Guid;
                }

                auto const& l_Itr = m_UniqueObjectGUIDs.find(p_ID);
                if (l_Itr != m_UniqueObjectGUIDs.end())
                    return l_Itr->second;

                return 0;
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataFirstWingClearedIntro:
                        return m_FirstWingClearedIntro;
                    case eData::DataBrokenCliffsEnabled:
                        return m_EventState[eEventList::EventBrokenCliffEnabled] ? 1 : 0;
                    case eData::DataKinGarothIntro:
                        return m_uiDialogs[0];
                    case eData::DataTarneth:
                        return m_uiDialogs[1];
                    case eData::DataVarimathrasIntro:
                        return m_uiDialogs[2];
                    case eData::DataImonarSkipQuest:
                        return m_ImonarSkipQuestDone;
                    case eData::DataAggramarSkipQuest:
                        return m_AggramarSkipQuest;
                    case eData::DataDungeonID:
                        return m_DungeonID;
                    default:
                    {
                        if (m_MiscData.find(p_ID) != m_MiscData.end())
                            return m_MiscData[p_ID];

                        break;
                    }
                }

                return 0;
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                for (uint8 l_I = 0; l_I < eTimers::TimerMax; ++l_I)
                {
                    if (m_Timers[l_I].GetCurrent() >= 0)
                        m_Timers[l_I].Update(p_Diff);
                    else
                        m_Timers[l_I].SetCurrent(0);
                }

                for (Map::PlayerList::const_iterator l_Itr = instance->GetPlayers().begin(); l_Itr != instance->GetPlayers().end(); ++l_Itr)
                {
                    if (Player* l_Player = l_Itr->getSource())
                    {
                        if (!m_EventState[eEventList::EventIntroConversation])
                        {
                            if (m_UniqueObjectGUIDs.find(eCreatures::NpcHighExarchTuralyon) != m_UniqueObjectGUIDs.end())
                            {
                                if (Creature* l_Turalyon = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::NpcHighExarchTuralyon]))
                                {
                                    m_EventState[eEventList::EventIntroConversation] = true;

                                    Conversation* l_Conversation = new Conversation;
                                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::IntroConversation1, l_Turalyon, nullptr, *l_Turalyon))
                                        delete l_Conversation;

                                    AddTimedDelayedOperation(200, [this]() -> void
                                    {
                                        Position l_SpawnPos = { -3363.394f, 9534.41f, -1.277003f, 3.005452f };
                                        if (Creature* l_Velen = instance->SummonCreature(eCreatures::NpcImageOfProphetVelen, l_SpawnPos, nullptr, 11 * TimeConstants::IN_MILLISECONDS))
                                            l_Velen->CastSpell(l_Velen, eSpells::ImageOfVelenAura, true);
                                    });

                                    AddTimedDelayedOperation(800, [this]() -> void
                                    {
                                        if (m_UniqueObjectGUIDs.find(eCreatures::NpcHighExarchTuralyon) != m_UniqueObjectGUIDs.end())
                                        {
                                            if (Creature* l_Turalyon = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::NpcHighExarchTuralyon]))
                                            {
                                                Conversation* l_Conversation = new Conversation;
                                                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::IntroConversation2, l_Turalyon, nullptr, *l_Turalyon))
                                                    delete l_Conversation;
                                            }
                                        }
                                    });

                                    AddTimedDelayedOperation(TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        if (m_UniqueObjectGUIDs.find(eCreatures::NpcHighExarchTuralyon) != m_UniqueObjectGUIDs.end())
                                        {
                                            if (Creature* l_Turalyon = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::NpcHighExarchTuralyon]))
                                                l_Turalyon->HandleEmoteCommand(Emote::EMOTE_ONESHOT_SALUTE);
                                        }
                                    });

                                    AddTimedDelayedOperation(TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        if (m_UniqueObjectGUIDs.find(eCreatures::NpcImageOfProphetVelen) != m_UniqueObjectGUIDs.end())
                                        {
                                            if (Creature* l_Turalyon = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::NpcImageOfProphetVelen]))
                                                l_Turalyon->HandleEmoteCommand(Emote::EMOTE_STATE_ONESHOT_TALK_NOSHEATHE);
                                        }
                                    });
                                }
                            }
                        }

                        if (!m_EventState[eEventList::EventGarothiAnnihilatorIntro] && l_Player->GetPositionZ() < -60.0f)
                        {
                            m_EventState[eEventList::EventGarothiAnnihilatorIntro] = true;

                            if (GetBossState(eData::DataGarothiWorldbreaker) == EncounterState::DONE)
                                continue;

                            if (m_UniqueObjectGUIDs.find(eCreatures::NpcHighExarchTuralyon) != m_UniqueObjectGUIDs.end())
                            {
                                if (Creature* l_Turalyon = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::NpcHighExarchTuralyon]))
                                {
                                    std::list<Creature*> l_CreaturesToDespawn;
                                    l_CreaturesToDespawn.push_back(l_Turalyon);
                                    l_Turalyon->GetCreatureListWithEntryInGrid(l_CreaturesToDespawn, eCreatures::NpcLightforgedCenturion, 50.0f);
                                    l_Turalyon->GetCreatureListWithEntryInGrid(l_CreaturesToDespawn, eCreatures::NpcLightforgedCleric, 50.0f);
                                    l_Turalyon->GetCreatureListWithEntryInGrid(l_CreaturesToDespawn, eCreatures::NpcLightforgedCommander, 50.0f);
                                    l_Turalyon->GetCreatureListWithEntryInGrid(l_CreaturesToDespawn, eCreatures::NpcLightforgedWarframe, 50.0f);
                                    l_Turalyon->GetCreatureListWithEntryInGrid(l_CreaturesToDespawn, eCreatures::NpcLightforgedBarricade, 50.0f);

                                    for (Creature* l_Creature : l_CreaturesToDespawn)
                                        l_Creature->DespawnOrUnsummon(1);
                                }
                            }

                            Position const l_AnnihilatorSpawn = { -3297.708f, 9794.954f, -37.61304f, 4.687736f };
                            Creature* l_Annihilator =  instance->SummonCreature(eCreatures::NpcGarothiAnnihilator, l_AnnihilatorSpawn);

                            continue;
                        }

                        if (!m_EventState[eEventList::EventBrokenCliffEnabled] && l_Player->GetPositionZ() < -128.0f)
                            m_EventState[eEventList::EventBrokenCliffEnabled] = true;

                        Position l_AntoranHighCommandTrigger = {-2858.11f, 10812.48f, 137.59f };
                        if (!m_EventState[eEventList::EventAntoranHighCommandIntro] && l_Player->GetExactDist(&l_AntoranHighCommandTrigger) < 20.0f)
                        {
                            m_EventState[eEventList::EventAntoranHighCommandIntro] = true;

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::AntoranHighCommandIntro, l_Player, nullptr, *l_Player))
                                delete l_Conversation;
                        }

                        if (m_Timers[eTimers::TimerImonarsFall].Passed())
                        {
                            float l_WestDist = l_Player->GetExactDist2d(&g_WestImonarPos);
                            float l_EastDist = l_Player->GetExactDist2d(&g_EastImonarPos);

                            if ((l_WestDist <= 120.0f || l_EastDist <= 120.0f) && l_Player->m_positionZ < 1860.0f && l_Player->IsFalling())
                            {
                                l_Player->Kill(l_Player);

                                if (l_EastDist > l_WestDist)
                                    l_Player->NearTeleportTo(g_WestImonarPos);
                                else
                                    l_Player->NearTeleportTo(g_EastImonarPos);
                            }
                        }

                        if (m_Timers[eTimers::TimerHasabelsFall].Passed())
                        {
                            Creature* l_Hasabel = instance->GetCreature(GetData64(eCreatures::BossPortalKeeperHasabel));
                            if (l_Hasabel && l_Player->IsFalling() && l_Player->m_positionZ < 617.0f && l_Player->GetExactDist(l_Hasabel) < 150.0f)
                            {
                                l_Player->Kill(l_Player);
                                l_Player->NearTeleportTo({ -3703.95f, -1380.39f, 624.51f, 1.57f });
                            }
                        }

                        /// Elarian Sanctuary
                        if (m_Timers[eTimers::TimerEonarsFall].Passed() && l_Player->GetAreaId() == 9333)
                        {
                            /// Player is falling without Surge of Life feather fall buff - Need to make him jump on platforms
                            if (l_Player->IsFalling() && l_Player->m_positionZ < 645.0f && !l_Player->HasAura(eSharedSpells::SpellSurgeOfLifeSecond))
                            {
                                ///         Jump Dest
                                std::vector<Position> l_CoordsLinks =
                                {
                                    { -3907.979f, -10891.87f, 686.339f, 0.0f },
                                    { -3885.675f, -10853.19f, 711.578f, 0.0f },
                                    { -3978.941f, -10769.44f, 700.832f, 0.0f },
                                    { -3891.101f, -10753.82f, 745.753f, 0.0f },
                                    { -3899.035f, -10798.59f, 706.646f, 0.0f },
                                    { -3864.285f, -10700.90f, 725.913f, 0.0f },
                                    { -3980.483f, -10820.61f, 684.833f, 0.0f },
                                    { -3956.927f, -10829.21f, 684.833f, 0.0f }
                                };

                                Position l_Pos;
                                float l_MaxDist = 1000000.0f;

                                /// Select nearest jump pos
                                for (Position const& l_TestPos : l_CoordsLinks)
                                {
                                    float l_Dist = l_Player->GetExactDist2d(&l_TestPos);

                                    if (l_Dist < l_MaxDist)
                                    {
                                        l_MaxDist = l_Dist;
                                        l_Pos     = l_TestPos;
                                    }
                                }

                                l_Player->CastSpell(l_Pos, eSharedSpells::SpellJumpOnPad, true);
                            }
                        }

                        if (m_Timers[eTimers::TimerKingarothFall].Passed())
                        {
                            Position const l_KingarothPos = { -10574.4629f, 8141.8179f, 1873.6526f, 4.91f };
                            float l_Dist = l_Player->GetExactDist2d(&l_KingarothPos);

                            if (l_Dist <= 200.f && l_Player->m_positionZ <= 1820.0f && l_Player->IsFalling())
                            {
                                l_Player->Kill(l_Player);
                                l_Player->NearTeleportTo(l_KingarothPos);
                            }
                        }
                    }
                }

                /// Reset timers only after Players loop
                for (uint8 l_I = 0; l_I < eTimers::TimerMax; ++l_I)
                {
                    if (m_Timers[l_I].Passed())
                        m_Timers[l_I].Reset();
                }
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* p_Player) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationRaidEntrance);

                /// Argus the Unmaker - Fourth phase allows players to release and respawn directly in the encounter area
                if (GetBossState(eData::DataArgusTheUnmaker) == EncounterState::IN_PROGRESS && IsReleaseAllowed())
                {
                    p_X     = p_Player->m_positionX;
                    p_Y     = p_Player->m_positionY;
                    p_Z     = p_Player->m_positionZ;
                    p_O     = p_Player->m_orientation;
                    p_MapID = p_Player->GetMapId();
                    return;
                }

                switch (m_LastPulledBoss)
                {
                    case eData::DataFelhoundsOfSargeras:
                    {
                        p_X     = -3417.615f;
                        p_Y     = 10145.85f;
                        p_Z     = -149.925f;
                        p_O     = 6.0365f;
                        p_MapID = 1712;
                        return;
                    }
                    case eData::DataAntoranHighCommand:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationWarCouncil);
                        break;
                    }
                    case eData::DataPortalKeeperHasabel:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationLFRWing2);
                        break;
                    }
                    case eData::DataEonarTheLifeBinder:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationEonarLifebinder);
                        break;
                    }
                    case eData::DataImonarTheSoulhunter:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationImonarHunter);
                        break;
                    }
                    case eData::DataKinGaroth:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationLFRWing3);
                        break;
                    }
                    case eData::DataVarimathras:
                    case eData::DataTheCovenOfShivarra:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAntorusCore);
                        break;
                    }
                    case eData::DataAggramar:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAggramar);
                        break;
                    }
                    case eData::DataArgusTheUnmaker:
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationPantheon);
                        break;
                    }
                    default:
                    {
                        switch (m_DungeonID)
                        {
                            case eDungeonIDs::DungeonIDForbiddenDescent:
                            {
                                l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationLFRWing2);
                                break;
                            }
                            case eDungeonIDs::DungeonIDHopesEnd:
                            {
                                l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationLFRWing3);
                                break;
                            }
                            case eDungeonIDs::DungeonIDSeatofthePantheon:
                            {
                                l_Loc = sWorldSafeLocsStore.LookupEntry(eLocations::LocationAggramar);
                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                }

                if (!l_Loc)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
            {
                if (m_ClubfistBeastlordPack.find(p_Creature->GetGUID()) != m_ClubfistBeastlordPack.end())
                {
                    m_ClubfistBeastlordPack.erase(p_Creature->GetGUID());
                    if (m_ClubfistBeastlordPack.empty())
                        PlayCosmeticEvent(eCosmeticEvents::ClubfistBeastlordDefeated, p_Killer->GetGUID());
                }
            }

            std::string GetDialogSaveData()
            {
                std::ostringstream saveStream;
                for (uint8 i = 0; i < 3; i++)
                    saveStream << (uint32)m_uiDialogs[i] << " ";
                return saveStream.str();
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "A B T " << GetBossSaveData() << GetDialogSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void CheckDataAfterLoad()
            {
                if (GetData(eData::DataTarneth) == DONE)
                  if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GoPreVarimathrasDoor)))
                      l_Door->SetGoState(GOState::GO_STATE_ACTIVE);

                if (GetBossState(eData::DataKinGaroth) == DONE)
                    if (Creature* l_Beacon = instance->GetCreature(GetData64(eCreatures::NpcTeleportPodTheExhaust)))
                        l_Beacon->SetVisible(true);

                if (GetBossState(eData::DataTheCovenOfShivarra) == DONE && GetBossState(eData::DataVarimathras) == DONE && !instance->IsLFR())
                    if (GameObject* l_GoPreAgrammar = instance->GetGameObject(GetData64(eGameObjects::GoPreAgrammarDoor)))
                        l_GoPreAgrammar->SetGoState(GO_STATE_ACTIVE);
            }

            void Load(const char* strIn) override
            {
                if (!strIn)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(strIn);

                char dataHead1, dataHead2, dataHead3;

                std::istringstream loadStream(strIn);
                loadStream >> dataHead1 >> dataHead2 >> dataHead3;

                if (dataHead1 == 'A' && dataHead2 == 'B' && dataHead3 == 'T')
                {
                    for (uint8 i = 0; i < eData::DataMaxBosses; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    for (uint8 j = 0; j < 3; j++)
                    {
                        uint32 tmpEvent;
                        loadStream >> tmpEvent;
                        if (tmpEvent == IN_PROGRESS || tmpEvent > SPECIAL)
                            tmpEvent = NOT_STARTED;

                        m_uiDialogs[j] = tmpEvent;
                    }

                    CheckDataAfterLoad();
                }

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            bool IsValidEncounter(EncounterCreditType p_Type, Unit* p_Source) const override
            {
                switch (p_Source->GetEntry())
                {
                    case eCreatures::NpcShadowofVarimathras2:
                    case eCreatures::NpcShadowofVarimathras:
                    case eCreatures::NpcNouraMotherOfFlames:
                    case eCreatures::NpcDiimaMotherofGloom:
                    case eCreatures::NpcThurayaMotherOfTheCosmos:
                         return false;
                    default:
                        break;
                }

                return true;
            }

            void OnPlayerAuraApplied(Player* p_Player, uint32 p_SpellID) override
            {
                if (!IsPantheonSpell(p_SpellID))
                    return;

                m_PantheonSpells[p_SpellID].insert(p_Player->GetGUID());

                /// Don't try to Empower players if only 3 unique buffs are active
                if (m_PantheonSpells.size() < eData::DataPantheonsBuffNeeded)
                    return;

                uint8 l_PossibleProcsCount = 0;

                for (auto& l_Iter : m_PantheonSpells)
                {
                    if (l_Iter.second.empty())
                        continue;

                    ++l_PossibleProcsCount;
                }

                if (l_PossibleProcsCount >= eData::DataPantheonsBuffNeeded)
                {
                    /// Just in case more are found
                    l_PossibleProcsCount = eData::DataPantheonsBuffNeeded;

                    for (auto l_Iter : m_PantheonSpells)
                    {
                        if (l_Iter.second.empty())
                            continue;

                        if (!l_PossibleProcsCount)
                            break;

                        std::unordered_set<uint64> l_GuidSet = l_Iter.second;

                        for (auto& l_Guid : l_GuidSet)
                        {
                            if (Player* l_Player = Player::GetPlayer(*p_Player, l_Guid))
                            {
                                l_Iter.second.erase(l_Guid);

                                EmpowerPantheonProc(l_Player);
                                break;
                            }
                        }

                        --l_PossibleProcsCount;
                    }
                }
            }

            void OnPlayerAuraRemoved(Player* p_Player, uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (!IsPantheonSpell(p_SpellID))
                    return;

                auto l_Iter = m_PantheonSpells.find(p_SpellID);
                if (l_Iter == m_PantheonSpells.end())
                    return;

                l_Iter->second.erase(p_Player->GetGUID());

                if (l_Iter->second.empty())
                    m_PantheonSpells.erase(p_SpellID);
            }

            bool IsPantheonSpell(uint32 p_SpellID) const
            {
                if (p_SpellID == eSharedSpells::SpellCelestialBulwark ||
                    p_SpellID == eSharedSpells::SpellGlimpseOfEnlightenment ||
                    p_SpellID == eSharedSpells::SpellMarkOfGolganneth ||
                    p_SpellID == eSharedSpells::SpellMarkOfEonar ||
                    p_SpellID == eSharedSpells::SpellWorldforgersFlame ||
                    p_SpellID == eSharedSpells::SpellRushOfKnowledge)
                    return true;

                return false;
            }

            void EmpowerPantheonProc(Player* p_Player)
            {
                std::map<uint32, uint32> l_ProcsForAuras =
                {
                    { eSharedSpells::SpellCelestialBulwark,         eSharedSpells::SpellAggramarsFortitude },
                    { eSharedSpells::SpellGlimpseOfEnlightenment,   eSharedSpells::SpellAmanThulsGrandeur },
                    { eSharedSpells::SpellMarkOfGolganneth,         eSharedSpells::SpellGolgannethsThunderousWrath },
                    { eSharedSpells::SpellMarkOfEonar,              0 },
                    { eSharedSpells::SpellWorldforgersFlame,        eSharedSpells::SpellKhazgorothsShaping },
                    { eSharedSpells::SpellRushOfKnowledge,          eSharedSpells::SpellNorgannonsCommand }
                };

                for (auto const& l_Iter : l_ProcsForAuras)
                {
                    if (Aura* l_Aura = p_Player->GetAura(l_Iter.first))
                    {
                        Item* l_CastItem = nullptr;
                        if (l_Aura->GetCastItemGUID())
                            l_CastItem = p_Player->GetItemByGuid(l_Aura->GetCastItemGUID());

                        if (l_Iter.second)
                        {
                            if (l_Iter.second == eSharedSpells::SpellNorgannonsCommand)
                            {
                                for (uint8 l_I = 0; l_I < 6; ++l_I)
                                    p_Player->CastSpell(p_Player, l_Iter.second, true, l_CastItem);
                            }
                            else
                                p_Player->CastSpell(p_Player, l_Iter.second, true, l_CastItem);
                        }
                        else
                        {
                            uint32 l_HealSpellID = 0;

                            switch (p_Player->GetActiveSpecializationID())
                            {
                                case SpecIndex::SPEC_PRIEST_DISCIPLINE:
                                    l_HealSpellID = eSharedSpells::SpellEonarsVerdentEmbraceDiscPriest;
                                    break;
                                case SpecIndex::SPEC_PRIEST_HOLY:
                                    l_HealSpellID = eSharedSpells::SpellEonarsVerdentEmbraceHolyPriest;
                                    break;
                                case SpecIndex::SPEC_DRUID_RESTORATION:
                                    l_HealSpellID = eSharedSpells::SpellEonarsVerdentEmbraceDruid;
                                    break;
                                case SpecIndex::SPEC_PALADIN_HOLY:
                                    l_HealSpellID = eSharedSpells::SpellEonarsVerdentEmbracePaladin;
                                    break;
                                case SpecIndex::SPEC_MONK_MISTWEAVER:
                                    l_HealSpellID = eSharedSpells::SpellEonarsVerdentEmbraceMonk;
                                    break;
                                case SpecIndex::SPEC_SHAMAN_RESTORATION:
                                    l_HealSpellID = eSharedSpells::SpellEonarsVerdentEmbraceShaman;
                                    break;
                                default:
                                    break;
                            }

                            if (l_HealSpellID)
                            {
                                for (uint8 l_I = 0; l_I < 4; ++l_I)
                                    p_Player->CastSpell(p_Player, l_HealSpellID, true, l_CastItem);
                            }
                        }
                    }
                }
            }

            void BeforeAddBossLooted(uint32 p_CreatureID) override
            {
                switch (p_CreatureID)
                {
                    case eCreatures::BossArgusTheUnmaker:
                        HandleArgusPersonalRewards();
                        break;
                    case eCreatures::BossShatug:
                    {
                        instance->ApplyOnEveryPlayer([&](Player* player)
                        {
                            if (roll_chance_f(1.f))
                            {
                                if (auto fharg = Creature::GetCreature(*player, GetData64(eCreatures::BossShatug)))
                                {
                                    auto IsLooted = player->BossAlreadyLooted(fharg, false);
                                    if (!IsLooted)
                                    {
                                        ItemPosCountVec dest;
                                        if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 152816, 1, nullptr) == EQUIP_ERR_OK)
                                        {
                                            if (auto item = player->StoreNewItem(dest, 152816, true))
                                            {
                                                player->SendNewItem(item, 1, true, false, true);
                                                player->SendDisplayToast(152816, 1, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_ITEM, false, false);
                                            }
                                        }
                                    }
                                }
                            }
                        });
                        break;
                    }
                }
            }

            void HandleArgusPersonalRewards()
            {
                std::array<uint32, ePantheonItems::ItemMaxTrinkets> l_Trinkets =
                {
                    {
                        ePantheonItems::ItemAggramarsConviction,
                        ePantheonItems::ItemGolgannethsVitality,
                        ePantheonItems::ItemEonarsCompassion,
                        ePantheonItems::ItemKhazgorothsCourage,
                        ePantheonItems::ItemNorgannonsProwess
                    }
                };

                Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        /// Don't handle personal rewards if boss was already looted
                        Creature* l_Argus = Creature::GetCreature(*l_Player, GetData64(eCreatures::BossArgusTheUnmaker));
                        if (!l_Argus)
                            continue;

                        bool l_AlreadyLooted = l_Player->BossAlreadyLooted(l_Argus, false, DifficultyRaidLFR);

                        /// Handle Pantheon trinkets
                        if (!instance->IsLFR())
                        {
                            bool l_Trinket = false;

                            std::set<uint32> l_NotForSpecItems;

                            /// Try to add a spec trinket if possible, once per weekly kill
                            if (!l_AlreadyLooted && roll_chance_i(30))
                            {
                                for (uint32 const& l_ItemID : l_Trinkets)
                                {
                                    if (ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(l_ItemID))
                                    {
                                        /// Trinket already looted once, try to pick up another one
                                        if (l_Player->HasItemCount(l_ItemID))
                                            continue;

                                        /// Not for spec, register it for later
                                        if (!l_Template->IsUsableBySpecialization(l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID(), l_Player->getLevel()))
                                        {
                                            /// Register it only if usable by class
                                            if (l_Template->HasClassSpec(l_Player->getClass(), l_Player->getLevel()))
                                                l_NotForSpecItems.insert(l_ItemID);

                                            continue;
                                        }

                                        l_Trinket = true;

                                        if (Item* l_Item = l_Player->AddItem(l_ItemID, 1, { }, false, 0, false, true))
                                        {
                                            l_Player->SendDisplayToast(l_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
                                            break;
                                        }
                                    }
                                }

                                /// Didn't find lootable trinket for current spec, try to add another one
                                if (!l_Trinket)
                                {
                                    for (uint32 const& l_ItemID : l_NotForSpecItems)
                                    {
                                        if (ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(l_ItemID))
                                        {
                                            bool l_UsableForAnotherSpec = false;
                                            std::vector<ChrSpecializationsEntry const*> const& l_SpecializationsForClass = GetChrSpecializationsForClass(l_Player->getClass());
                                            for (ChrSpecializationsEntry const* l_CharSpecializationEntry : l_SpecializationsForClass)
                                            {
                                                if (!l_Template->IsUsableBySpecialization(l_CharSpecializationEntry->ID, l_Player->getLevel()))
                                                    continue;

                                                l_UsableForAnotherSpec = true;
                                                break;
                                            }

                                            if (!l_UsableForAnotherSpec)
                                                continue;

                                            if (Item* l_Item = l_Player->AddItem(l_ItemID, 1, { }, false, 0, false, true))
                                            {
                                                l_Player->SendDisplayToast(l_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
                                                break;
                                            }
                                        }
                                    }
                                }
                            }

                            /// Handle legendary Aman'Thul's Visions separately, it can be looted at the same time as an epic trinket, once per weekly kill
                            if (!l_AlreadyLooted && roll_chance_f(0.1f) && !l_Player->HasItemCount(ePantheonItems::ItemAmanThulsVision))
                            {
                                if (Item* l_Item = l_Player->AddItem(ePantheonItems::ItemAmanThulsVision, 1, { }, false, 0, false, true))
                                    l_Player->SendDisplayToast(ePantheonItems::ItemAmanThulsVision, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
                            }
                        }

                        /// Blood of a Titan quest, don't check if already killed
                        if (instance->IsHeroicOrMythic())
                        {
                            if (!l_Player->HasItemCount(ePantheonItems::ItemBloodOfTheUnmaker) && !l_Player->HasSpell(eSharedSpells::SpellVioletSpellwing))
                            {
                                if (Item* l_Item = l_Player->AddItem(ePantheonItems::ItemBloodOfTheUnmaker, 1, { }, false, 0, false, true))
                                    l_Player->SendDisplayToast(ePantheonItems::ItemBloodOfTheUnmaker, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false);
                            }
                        }

                        /// Cosmetic weapon - Scythe of the Unmaker, once per weekly kill
                        switch (l_Player->getClass())
                        {
                            case Classes::CLASS_WARRIOR:
                            case Classes::CLASS_DEATH_KNIGHT:
                            case Classes::CLASS_DRUID:
                            case Classes::CLASS_HUNTER:
                            case Classes::CLASS_MONK:
                            case Classes::CLASS_PALADIN:
                            {
                                if (l_AlreadyLooted || !roll_chance_i(1))
                                    break;

                                uint32 l_ItemID = instance->IsMythic() ? ePantheonItems::ItemArgusScytheRed : ePantheonItems::ItemArgusScytheBlue;

                                if (Item* l_Item = l_Player->AddItem(l_ItemID, 1, { }, false, 0, false, true))
                                    l_Player->SendDisplayToast(l_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false);

                                break;
                            }
                            default:
                                break;
                        }

                        /// First kill of the week can give you Pantheon's Blessing, must not be the very first kill of the boss
                        if (!l_AlreadyLooted && !instance->IsLFR())
                        {
                            std::vector<uint32> l_Achievements =
                            {
                                eAchievements::ArgusTheUnmakerNm,
                                eAchievements::ArgusTheUnmakerHm,
                                eAchievements::ArgusTheUnmakerMm
                            };

                            for (uint32 const& l_AchievementID : l_Achievements)
                            {
                                if (l_Player->GetAchievementProgress(l_AchievementID))
                                {
                                    if (Item* l_Item = l_Player->AddItem(ePantheonItems::ItemPantheonsBlessing, 1, { }, false, 0, false, true))
                                        l_Player->SendDisplayToast(ePantheonItems::ItemPantheonsBlessing, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false);

                                    break;
                                }
                            }
                        }
                    }
                }
            }

            void FirstWingIntro()
            {
                /// Trigger cosmetic conversation after the 5 first bosses have been killed
                if (!m_FirstWingClearedIntro)
                {
                    if (GetBossState(eData::DataGarothiWorldbreaker) == EncounterState::DONE &&
                        GetBossState(eData::DataFelhoundsOfSargeras) == EncounterState::DONE &&
                        GetBossState(eData::DataAntoranHighCommand) == EncounterState::DONE &&
                        GetBossState(eData::DataPortalKeeperHasabel) == EncounterState::DONE &&
                        GetBossState(eData::DataEonarTheLifeBinder) == EncounterState::DONE)
                    {
                        m_FirstWingClearedIntro = true;

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            auto const& l_Iter = m_MultipleNpcGuids.find(eCreatures::NpcLightforgedWarframeToImonar);
                            if (l_Iter != m_MultipleNpcGuids.end())
                            {
                                for (uint64 const& l_Guid : l_Iter->second)
                                {
                                    if (Creature* l_Warframe = instance->GetCreature(l_Guid))
                                        l_Warframe->SetVisible(true);
                                }
                            }

                            SpawnPatrol();

                            if (GetBossState(eData::DataImonarTheSoulhunter) != EncounterState::DONE)
                            {
                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    DoCastSpellOnPlayers(eSharedSpells::SpellWing1ClearedIntro, nullptr, true);
                                });
                            }

                            if (Creature* l_Magni = instance->GetCreature(GetData64(eCreatures::NpcMagniBronzebeard)))
                            {
                                l_Magni->NearTeleportTo({ -3422.229f, 10158.06f, -150.0222f, 0.914228f });

                                l_Magni->SetAIAnimKitId(0);
                            }

                            if (Creature* l_Turalyon = instance->GetCreature(GetData64(eCreatures::NpcHighExarchTuralyon)))
                                l_Turalyon->DespawnOrUnsummon();

                            instance->SummonCreature(eCreatures::NpcHighExarchTuralyon, { -3420.99f, 10158.7778f, -150.11f, 1.127666f });
                        });
                    }
                }
            }

            void TriggerAggramarOutro()
            {
                if (GetBossState(eData::DataAggramar) != EncounterState::DONE)
                    return;

                if (m_EventState[eEventList::EventAggramarOutro])
                    return;

                m_EventState[eEventList::EventAggramarOutro] = true;

                PlayCosmeticEvent(eCosmeticEvents::AntorusAggramarOutro);

                AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Aggramar = instance->GetCreature(m_UniqueObjectGUIDs[eCreatures::BossAggramar]);
                    if (!l_Aggramar)
                        return;

                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::AggramarOutro, l_Aggramar, nullptr, *l_Aggramar))
                        delete l_Conversation;
                });
            }

            bool IsLineOfSightDisabled() const override
            {
                return GetBossState(eData::DataArgusTheUnmaker) == EncounterState::IN_PROGRESS;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_antorus_the_burning_throne_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_antorus_the_burning_throne()
{
    new instance_antorus_the_burning_throne();
}
#endif
