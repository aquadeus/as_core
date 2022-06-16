////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

DoorData const g_DoorData[] =
{
    { eGameObjects::OperaSceneLeftDoor,     eData::OperaHall,           DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { eGameObjects::OperaSceneRightDoor,    eData::OperaHall,           DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { eGameObjects::OperDoorToMoroes,       eData::OperaHall,           DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { eGameObjects::MoroesDoor1,            eData::DataMoroes,          DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { eGameObjects::MoroesDoor2,            eData::DataMoroes,          DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { eGameObjects::CuratorDoor,            eData::DataCurator,         DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { eGameObjects::MedivhDoor1,            eData::DataShadeOfMedivh,   DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { eGameObjects::MedivhDoor2,            eData::DataShadeOfMedivh,   DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { eGameObjects::VizaduumDoor1,          eData::Vizaduum,            DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { eGameObjects::DoorInstancePH2_1,      eData::DataNightbane,       DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { eGameObjects::DoorInstancePH2_2,      eData::DataNightbane,       DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { 0,                                    0,                          DOOR_TYPE_ROOM,       BOUNDARY_NONE }
};

Position const g_KhadgarEventPos = { 3459.40f, -2090.32f, 1053.33f, 1.36f };

Position const g_MoroesScenePos[2] =
{
    { -10987.85f, -1882.53f, 81.72f, 0.12f },
    { -10979.08f, -1881.64f, 81.72f, 3.27f }
};

Position const g_NeantspaceEntrance = { 3694.59f, -2198.73f, 815.60f };

Position const g_TeleportToMenagerie = { -4491.78f, -2236.52f, 2932.47f };
Position const g_TeleportToMenagerieArrival = { -11144.79f, -1887.55f, 165.77f };

std::array<Position, 2> g_Ships =
{{
    {3491.0f, -1971.0f, 1039.0f},
    {3950.0f, -1999.0f, 926.00f},
}};

class instance_return_to_karazhan : public InstanceMapScript
{
    public:
        instance_return_to_karazhan() : InstanceMapScript("instance_return_to_karazhan", 1651) { }

        struct instance_return_to_karazhan_InstanceMapScript : public InstanceScript
        {
            instance_return_to_karazhan_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(eData::MaxEncounter);
            }

            enum eScenes
            {
                MedivhMoroesDead    = 1777,
                MedivhMaidenDead    = 1778,
                MedivhCuratorDead   = 1779,
                MedivhDevourerDead  = 1781
            };

            enum eEventList
            {
                EventKhadgarEntrance,
                EventTimerCheck,
                EventSceneMaidenDead,
                EventAttumenIntro,
                EventCuratorOutro,
                EventMoroesIntro,
                EventLibraryMedivh,
                EventChessOutro,
                EventKhadgarChess,
                EventVizaduumIntro,

                MaxEvents
            };

            enum eSpells
            {
                PermanentFeignDeath = 29266,
                JaggedShards        = 228834,
                Frostbite           = 227592,
                CeaselessWinter     = 227806
            };

            enum eWorldSafeLocs
            {
                ToCuratorTarget = 5820
            };

            std::map<uint32, uint64> m_OperaEncountersGuid;
            std::map<uint32, uint32> m_OperaEncountersCount;
            std::list<uint64> m_OperaDecorGuid[4];
            std::list<uint64> m_OtherGuidContainer;

            uint64 m_OperaVelumGuid;
            uint64 m_GroupGuid;
            uint64 m_Phase2Door[3];

            uint8 m_OperaHallIntro;
            uint32 m_OperaHallScene;
            uint32 m_LowerKarzhanEncounterEnded = 0;
            uint32 m_DungeonID;

            uint64 m_EntranceKhadgarGUID;
            Position m_MaidenHome;
            Position m_AttumenHome;
            uint64 m_AttumenGUID;
            uint64 m_CuratorGUID;
            uint64 m_CuratorMedivhGUID;
            Position m_MoroesHome;
            uint64 m_MoroesGUID;
            uint64 m_KingGUID;
            uint64 m_ShatteredBoardGUID;
            uint64 m_GamesmansRoomWall;
            std::vector<uint32> m_CristalGUIDS;
            uint64 m_VizaduumGUID;
            Position m_KhadgarChessHome;
            uint64 m_KhadgarChessGUID;
            uint32 m_dataPhase2Door;

            int32 m_NightbaneTimer = -1;

            Position m_LittleManaDevourerHome = { 4151.0f, -2032.13f, 730.65f };

            std::array<bool, eEventList::MaxEvents> m_EventState =
            { {
                false,
                false,
                false,
                false,
                false,
                false,
                false,
                false,
                false,
                false
            } };

            uint32 m_ChallengeMapID = 0;

            void Initialize() override
            {
                LoadDoorData(g_DoorData);

                m_OperaHallIntro = 0;
                m_OperaHallScene = 0;
                m_DungeonID = 0;
                m_dataPhase2Door = 0;

                AddTimedDelayedOperation(500, [this]() -> void
                {
                    SetData(eData::Phase2Door, DONE);
                });
            }

            bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
            {
                switch (p_Target->GetEntry())
                {
                    case eCreatures::BaronessDorothea:
                    case eCreatures::BaronRafe:
                    case eCreatures::LadyCatriona:
                    case eCreatures::LadyKeira:
                    case eCreatures::LordRobin:
                    case eCreatures::LordCrispin:
                    case eCreatures::AttumenTheHuntsman:
                    case eCreatures::Mrrgria:
                    case eCreatures::Galindre:
                    case eCreatures::Elfyra:
                    case eCreatures::Luminore:
                    case eCreatures::Babblet:
                    case eCreatures::Cauldrons:
                    case eCreatures::PetaVenker:
                    case eCreatures::EgonaSpangly:
                    case eCreatures::KorenTheBlacksmith:
                    case eCreatures::CalliardTheNightman:
                    case eCreatures::BarnesTheStageManager:
                    case eCreatures::GuardianImage:
                    case eCreatures::King:
                    case eCreatures::Queen:
                    case eCreatures::Rook:
                    case eCreatures::Knight:
                    case eCreatures::BlackBishop:
                    case eCreatures::WhiteBishop:
                    case eCreatures::ArchmageKhadgarEntrance:
                    case eCreatures::ArchmageKhadgarChess:
                    case eCreatures::ArchmageKhagdarVizaduum:
                    case eCreatures::MedivhChess:
                    case eCreatures::MedivhVizaduum:
                        return false;

                    default: 
                        return true;
                }

                return true;
            }

            void OnStartChallenge(Item const* p_Item) override
            {
                if (p_Item == nullptr)
                    return;

                m_ChallengeMapID = p_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID);
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream l_SaveStream;
                l_SaveStream << GetBossSaveData();

                for (bool l_EventState : m_EventState)
                    l_SaveStream << l_EventState << " ";

                l_SaveStream << ((m_NightbaneTimer == -1) ? 9 : ((m_NightbaneTimer < -1) ? 0 : (m_NightbaneTimer / IN_MILLISECONDS / MINUTE)))<< " ";

                l_SaveStream << m_OperaHallScene << " " << m_dataPhase2Door;

                OUT_SAVE_INST_DATA_COMPLETE;
                return l_SaveStream.str();
            }

            void Load(char const* p_In) override
            {
                if (!p_In)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(p_In);

                char l_DataHead1;

                std::istringstream l_LoadStream(p_In);

                l_LoadStream >> l_DataHead1;

                for (uint8 l_I = 0; l_I < 9; ++l_I)
                {
                    uint32 l_TmpState;

                    l_LoadStream >> l_TmpState;

                    if (l_TmpState == IN_PROGRESS || l_TmpState > SPECIAL)
                        l_TmpState = NOT_STARTED;

                    SetBossState(l_I, EncounterState(l_TmpState));
                }

                for (bool& l_EventState : m_EventState)
                    l_LoadStream >> l_EventState;

                int32 l_NightbaneTimer;
                l_LoadStream >> l_NightbaneTimer;

                switch (l_NightbaneTimer)
                {
                    case 0:
                        m_NightbaneTimer = -2;
                        break;
                    case 9:
                        m_NightbaneTimer = -1;
                        break;
                    default:
                        m_NightbaneTimer = l_NightbaneTimer * MINUTE * IN_MILLISECONDS;
                        break;
                }

                l_LoadStream >> m_OperaHallScene >> m_dataPhase2Door;

                m_EventState[eEventList::EventTimerCheck] = false;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                if (!m_GroupGuid)
                {
                    m_GroupGuid = p_Player->GetGroup() ? p_Player->GetGroup()->GetGUID() : 0;

                    if (m_GroupGuid)
                        m_DungeonID = sLFGMgr->GetDungeon(m_GroupGuid);
                }

                if (instance->IsMythic())
                    return;

                uint32 l_BossMask = 0;
                l_BossMask |= 1 << eData::DataNightbane;
                switch (m_DungeonID)
                {
                    case eDungeons::RTKLower:
                    {
                        l_BossMask |= 1 << eData::DataCurator;
                        l_BossMask |= 1 << eData::ManaDevourer;
                        l_BossMask |= 1 << eData::DataShadeOfMedivh;
                        l_BossMask |= 1 << eData::Vizaduum;
                        SetDisabledBosses(l_BossMask);
                        break;
                    }
                    case eDungeons::RTKSummit:
                    {
                        l_BossMask |= 1 << eData::OperaHall;
                        l_BossMask |= 1 << eData::MaidenVirtue;
                        l_BossMask |= 1 << eData::DataMidnight;
                        l_BossMask |= 1 << eData::DataMoroes;
                        SetDisabledBosses(l_BossMask);
                        break;
                    }
                }
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::Galindre:
                    case eCreatures::Elfyra:
                    case eCreatures::Mrrgria:
                    case eCreatures::ToeKnee:
                    case eCreatures::Coggleston:
                    case eCreatures::Luminore:
                    case eCreatures::Babblet:
                    case eCreatures::Cauldrons:
                    case eCreatures::Bella:
                    case eCreatures::Brute:
                    {
                        auto const& l_Iter = m_OperaEncountersGuid.find(p_Creature->GetEntry());
                        if (l_Iter != m_OperaEncountersGuid.end())
                        {
                            auto const& l_Count = m_OperaEncountersCount.find(p_Creature->GetEntry());

                            if (Creature* l_Creature = instance->GetCreature(l_Iter->second))
                                l_Creature->DespawnOrUnsummon();

                            m_OperaEncountersCount[p_Creature->GetEntry()]++;
                        }
                        else
                            m_OperaEncountersCount[p_Creature->GetEntry()] = 1;

                        m_OperaEncountersGuid[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CagedAssistant:
                    case eCreatures::HozenCage:
                    {
                        m_OperaDecorGuid[3].push_back(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::VizaduumTheWatcher:
                    {
                        m_OtherGuidContainer.push_back(p_Creature->GetGUID());
                        m_VizaduumGUID = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::ArchmageKhadgarEntrance:
                    {
                        m_EntranceKhadgarGUID = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::MaidenOfVirtue:
                    {
                        m_MaidenHome = p_Creature->GetHomePosition();
                        break;
                    }
                    case eCreatures::AttumenTheHuntsman:
                    {
                        m_AttumenHome = p_Creature->GetHomePosition();
                        m_AttumenGUID = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CuratorVision:
                    {
                        m_CuratorGUID = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CuratorMedivh:
                    {
                        m_CuratorMedivhGUID = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::Moroes:
                    {
                        m_MoroesHome = p_Creature->GetHomePosition();
                        m_MoroesGUID = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CuratorCristal:
                    {
                        p_Creature->SetVisible(false);
                        /// no break inteded
                    }
                    case eCreatures::MaidenCristal:
                    case eCreatures::MoroesCristal:
                    case eCreatures::OperaCristal:
                    case eCreatures::ServantsQuartersCristal:
                    {
                        m_CristalGUIDS.push_back(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::LittleManaDevourer:
                    {
                        if (p_Creature->GetExactDist(&m_LittleManaDevourerHome) < 5.0f)
                        {
                            p_Creature->AddAura(eSpells::PermanentFeignDeath, p_Creature);
                            p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        }
                        break;
                    }
                    case eCreatures::King:
                    {
                        m_KingGUID = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::ArchmageKhadgarChess:
                    {
                        m_KhadgarChessGUID = p_Creature->GetGUID();
                        m_KhadgarChessHome = p_Creature->GetPosition();
                        break;
                    }
                    case eCreatures::ForlornSpirit:
                    {
                        Position const l_Pos = { -11054.90f, -2028.26f, 115.366f, 1.5139f };

                        if (!p_Creature->IsNearPosition(&l_Pos, 0.1f))
                            break;

                        uint64 l_Guid = p_Creature->GetGUID();
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                        {
                            if (m_DungeonID != eDungeons::RTKSummit)
                                return;

                            if (Creature* l_Creature = instance->GetCreature(l_Guid))
                                l_Creature->DespawnOrUnsummon();
                        });

                        break;
                    }
                    case eCreatures::ShriekingTerror:
                    {
                        Position const l_Pos = { -11072.70f, -2038.56f, 115.366f, 5.37647f };

                        if (!p_Creature->IsNearPosition(&l_Pos, 0.1f))
                            break;

                        uint64 l_Guid = p_Creature->GetGUID();
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                        {
                            if (m_DungeonID != eDungeons::RTKSummit)
                                return;

                            if (Creature* l_Creature = instance->GetCreature(l_Guid))
                                l_Creature->DespawnOrUnsummon();
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnCreatureRemove(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CagedAssistant:
                    case eCreatures::HozenCage:
                    case eCreatures::Galindre:
                    case eCreatures::Elfyra:
                    case eCreatures::Coggleston:
                    case eCreatures::Luminore:
                    case eCreatures::Babblet:
                    case eCreatures::Cauldrons:
                    case eCreatures::Bella:
                    case eCreatures::Brute:
                    {
                        m_OperaEncountersGuid.erase(p_Creature->GetEntry());
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::VizaduumDoor1:
                    {
                        uint64 l_DoorGUID = p_GameObject->GetGUID();
                        AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this, l_DoorGUID]() -> void
                        {
                            GameObject* l_Door = instance->GetGameObject(l_DoorGUID);
                            l_Door->SetGoState(GO_STATE_READY);
                        });
                        /// No break Intended
                    }
                    case eGameObjects::OperaSceneLeftDoor:
                    case eGameObjects::OperaSceneRightDoor:
                    case eGameObjects::MoroesDoor1:
                    case eGameObjects::MoroesDoor2:
                    case eGameObjects::CuratorDoor:
                    case eGameObjects::MedivhDoor1:
                    case eGameObjects::MedivhDoor2:
                    case eGameObjects::OperDoorToMoroes:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case eGameObjects::OperaDecorWikket1:
                    case eGameObjects::OperaSceneWikket2:
                    case eGameObjects::OperaSceneWikket3:
                    {
                        m_OperaDecorGuid[0].push_back(p_GameObject->GetGUID());
                        break;
                    }
                    case eGameObjects::OperaDecorWestfall1:
                    case eGameObjects::OperaDecorWestfall2:
                    case eGameObjects::OperaDecorWestfall3:
                    case eGameObjects::OperaDecorWestfall4:
                    case eGameObjects::OperaDecorWestfall5:
                    {
                        m_OperaDecorGuid[1].push_back(p_GameObject->GetGUID());
                        break;
                    }
                    case eGameObjects::OperaDecorBeautifulBeast1:
                    case eGameObjects::OperaDecorBeautifulBeast2:
                    case eGameObjects::OperaDecorBeautifulBeast3:
                    case eGameObjects::OperaDecorBeautifulBeast4:
                    case eGameObjects::OperaDecorBeautifulBeast5:
                    {
                        m_OperaDecorGuid[2].push_back(p_GameObject->GetGUID());
                        break;
                    }
                    case eGameObjects::OperaSceneVelum:
                    {
                        m_OperaVelumGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GhostTrap:
                    {
                        m_OtherGuidContainer.push_back(p_GameObject->GetGUID());
                        break;
                    }
                    case eGameObjects::DoorInstancePH2_1:
                    {
                        m_Phase2Door[0] = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::DoorInstancePH2_2:
                    {
                        m_Phase2Door[1] = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::DoorInstancePH2_3:
                    {
                        m_Phase2Door[2] = p_GameObject->GetGUID();
                        break;
                    }
                    /////////////////////////////////////////////////////////////////
                    /// 7.2 Only!
                    case ChallengeModeOrb:
                    {
                        p_GameObject->setActive(false);
                        break;
                    }
                    case eGameObjects::ChallengersCache:
                    {
                        /// Upper
                        if (p_GameObject->GetPositionZ() > 500.0f)
                            m_ChallengeChestFirst = p_GameObject->GetGUID();
                        /// Lower
                        else
                            m_ChallengeChestFirst = p_GameObject->GetGUID();

                        break;
                    }
                    /////////////////////////////////////////////////////////////////
                    case eGameObjects::ShatteredBoard:
                    {
                        p_GameObject->SetGoState(GO_STATE_READY);
                        p_GameObject->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        m_ShatteredBoardGUID = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GamesmansRoomWall:
                    {
                        p_GameObject->SetGoState(GO_STATE_READY);
                        p_GameObject->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        m_GamesmansRoomWall = p_GameObject->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::OperaSceneLeftDoor:
                    case eGameObjects::OperaSceneRightDoor:
                    case eGameObjects::MoroesDoor1:
                    case eGameObjects::MoroesDoor2:
                    case eGameObjects::CuratorDoor:
                    case eGameObjects::MedivhDoor1:
                    case eGameObjects::MedivhDoor2:
                    case eGameObjects::VizaduumDoor1:
                    case eGameObjects::OperDoorToMoroes:
                    {
                        AddDoor(p_GameObject, false);
                        break;
                    }
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 p_Type, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Type, p_State))
                    return false;

                switch (p_Type)
                {
                    case eData::OperaHall:
                    {
                        if (m_OperaEncountersGuid.empty())
                            break;

                        switch (p_State)
                        {
                            case NOT_STARTED:
                            {
                                ActionListGUIDManager(eOperaActions::EncounterDespawn, ACTION_1);

                                AddTimedDelayedOperation(10000, [this]() -> void
                                {
                                    SetData(eData::OperaHallIntro, IN_PROGRESS);
                                });

                                break;
                            }
                            case IN_PROGRESS:
                            {
                                if (m_OperaHallScene != eData::OperaHallWestfall)
                                    ActionListGUIDManager(eOperaActions::ZoneInCombat);

                                break;
                            }
                            case DONE:
                            {
                                if (!m_OperaDecorGuid[3].empty())
                                {
                                    for (uint64 const& l_Guid : m_OperaDecorGuid[3])
                                    {
                                        if (Creature* l_Decor = instance->GetCreature(l_Guid))
                                        {
                                            if (l_Decor->GetEntry() == eCreatures::HozenCage)
                                                l_Decor->SetAIAnimKitId(9779);

                                            if (l_Decor->GetEntry() == eCreatures::CagedAssistant)
                                            {
                                                Position l_Pos;
                                                l_Decor->GetNearPosition(l_Pos, 5.0f, 0.0f);

                                                uint64 l_DecorGuid = l_Decor->GetGUID();
                                                AddTimedDelayedOperation(2000, [this, l_DecorGuid, l_Pos]() -> void
                                                {
                                                    if (Creature* l_Decor = instance->GetCreature(l_DecorGuid))
                                                        l_Decor->GetMotionMaster()->MovePoint(1, l_Pos);
                                                });
                                            }
                                        }
                                    }
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eData::DataMoroes:
                    {
                        if (p_State == NOT_STARTED)
                        {
                            if (!m_OtherGuidContainer.empty())
                            {
                                for (uint64 const& l_Guid : m_OtherGuidContainer)
                                {
                                    if (GameObject* l_GameObject = instance->GetGameObject(l_Guid))
                                    {
                                        if (l_GameObject->GetEntry() == eGameObjects::GhostTrap)
                                            l_GameObject->Respawn();
                                    }
                                }
                            }
                        }

                        if (p_State == EncounterState::DONE)
                        {
                            Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                            if (l_PlayerList.isEmpty())
                                break;

                            for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                    l_Player->PlayStandaloneScene(eScenes::MedivhMoroesDead, 16, *l_Player);
                            }

                            instance->SummonCreature(eCreatures::Medivh, g_MoroesScenePos[0]);
                            instance->SummonCreature(eCreatures::NielasAran, g_MoroesScenePos[1]);
                        }

                        break;
                    }
                    case eData::Vizaduum:
                    {
                        if (p_State == DONE)
                        {
                            if (Creature* l_Khadgar = instance->SummonCreature(eCreatures::ArchmageKhagdarVizaduum, g_KhadgarEventPos))
                                l_Khadgar->CastSpell(l_Khadgar, 229645, true);
                        }

                        break;
                    }
                    case eData::DataNightbane:
                    {
                        if (p_State == EncounterState::FAIL)
                        {
                            AddTimedDelayedOperation(30 * IN_MILLISECONDS, [this]() -> void
                            {
                                Position const l_SpawnPos = { -11153.50f, -1891.30f, 91.56f };
                                instance->SummonCreature(eCreatures::Nightbane, l_SpawnPos);
                            });
                        }

                        for (uint8 l_I = 0; l_I < 2; l_I++)
                            HandleGameObject(m_Phase2Door[l_I], p_State != EncounterState::IN_PROGRESS);

                        for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
                        {
                            if (Player* player = itr->getSource())
                                player->RemoveAurasDueToSpell(eSpells::JaggedShards);
                        }
                        break;
                    }
                    case eData::DataShadeOfMedivh:
                    {
                        for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
                        {
                            if (Player* player = itr->getSource())
                            {
                                player->RemoveAurasDueToSpell(eSpells::CeaselessWinter);
                                player->RemoveAurasDueToSpell(eSpells::Frostbite);
                            }
                        }
                    }
                    default:
                        break;
                }

                return true;
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                switch (p_Type)
                {
                    case eData::OperaHallIntro:
                    {
                        switch (p_Data)
                        {
                            case SPECIAL:
                            {
                                if (!m_OperaHallScene)
                                {
                                    m_OperaHallScene = sWorld->getWorldState(WorldStates::WS_RETURN_TO_KARAZHAN) & 0x7;
                                    switch (m_OperaHallScene)
                                    {
                                        case 1:
                                            DoUpdateWorldState(eRTKWorldStates::OperaWikket, 1);
                                            break;
                                        case 2:
                                            DoUpdateWorldState(eRTKWorldStates::OperaWestfall, 1);
                                            break;
                                        case 4:
                                            m_OperaHallScene = 3;
                                            DoUpdateWorldState(eRTKWorldStates::OperaBeautifulBeast, 1);
                                            break;
                                        default:
                                            break;
                                    }
                                    BindPlayer();
                                    SaveToDB();
                                }

                                if (!m_OperaHallScene || m_OperaHallScene >= 4)
                                    m_OperaHallScene = 1;

                                if (!m_OperaDecorGuid[m_OperaHallScene-1].empty())
                                {
                                    for (uint64 const& l_Guid : m_OperaDecorGuid[m_OperaHallScene - 1])
                                    {
                                        if (GameObject* l_Decor = instance->GetGameObject(l_Guid))
                                            l_Decor->SetRespawnTime(86400);
                                    }
                                }

                                break;
                            }
                            case IN_PROGRESS:
                            {
                                switch (m_OperaHallScene)
                                {
                                    case eData::OperaHallWikket:
                                    {
                                        if (m_OperaHallIntro != IN_PROGRESS)
                                        {
                                            for (uint8 l_I = 0; l_I < 10; l_I++)
                                                instance->SummonCreature(l_I < 5 ? eCreatures::CagedAssistant : eCreatures::HozenCage, g_OperaSpawnPos[l_I]);

                                            /// Boss Summons
                                            instance->SummonCreature(eCreatures::Galindre, g_OperaSpawnPos[10]);
                                            instance->SummonCreature(eCreatures::Elfyra, g_OperaSpawnPos[12]);
                                        }
                                        else
                                        {
                                            /// Boss Summons
                                            instance->SummonCreature(eCreatures::Galindre, g_OperaSpawnPos[11]);
                                            instance->SummonCreature(eCreatures::Elfyra, g_OperaSpawnPos[13]);
                                        }

                                        break;
                                    }
                                    case eData::OperaHallWestfall:
                                    {
                                        /// Boss Summons
                                        if (Creature* l_Boss = instance->SummonCreature(eCreatures::Mrrgria, g_OperaSpawnPos[14]))
                                        {
                                            for (uint8 l_I = 19; l_I < 22; l_I++)
                                                l_Boss->SummonCreature(eCreatures::ShorelineTidespeaker, g_OperaSpawnPos[l_I]);
                                        }

                                        if (Creature* l_Boss = instance->SummonCreature(eCreatures::ToeKnee, g_OperaSpawnPos[15]))
                                        {
                                            for (uint8 l_I = 16; l_I < 19; l_I++)
                                                l_Boss->SummonCreature(eCreatures::GangRuffian, g_OperaSpawnPos[l_I]);
                                        }

                                        break;
                                    }
                                    case eData::OperaHallBeautifulBeast:
                                    {
                                        /// Boss Summons
                                        instance->SummonCreature(eCreatures::Coggleston, g_OperaSpawnPos[34]);
                                        instance->SummonCreature(eCreatures::Luminore, g_OperaSpawnPos[35]);
                                        instance->SummonCreature(eCreatures::Babblet, g_OperaSpawnPos[36]);

                                        if (Creature* l_Boss = instance->SummonCreature(eCreatures::Cauldrons, g_OperaSpawnPos[37]))
                                        {
                                            l_Boss->SummonCreature(eCreatures::Bella, g_OperaSpawnPos[38]);
                                            l_Boss->SummonCreature(eCreatures::Brute, g_OperaSpawnPos[39]);
                                        }

                                        break;
                                    }
                                    default:
                                        break;
                                }

                                break;
                            }
                            default:
                                break;
                        }

                        if (p_Data == IN_PROGRESS && m_OperaHallIntro != IN_PROGRESS)
                        {
                            m_OperaHallIntro = p_Data;
                            HandleGameObject(m_OperaVelumGuid, true);
                        }

                        break;
                    }
                    case eData::OperaHallWikket:
                    {
                        if (p_Data == SPECIAL)
                            ActionListGUIDManager(eOperaActions::WikketIntroEnd);

                        break;
                    }
                    case eData::Phase2Door:
                    {
                        for (uint8 l_I = 0; l_I < 3; l_I++)
                            HandleGameObject(m_Phase2Door[l_I], GetBossState(eData::DataMoroes) == EncounterState::DONE);

                        break;
                    }
                    case eData::CancelCristalTimer:
                    {
                        m_EventState[EventTimerCheck] = true;
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case eData::OperaHallIntro:
                        return m_OperaHallIntro;
                    case eData::OperaHallScene:
                        return m_OperaHallScene;
                    default:
                        break;
                }

                return 0;
            }

            uint64 GetData64(uint32 type) override
            {
                switch (type)
                {
                    case eCreatures::ArchmageKhadgarEntrance:
                    {
                        if (m_EventState[eEventList::EventKhadgarEntrance])
                            return 0;

                        m_EventState[eEventList::EventKhadgarEntrance] = true;

                        m_NightbaneTimer = 8 * MINUTE * IN_MILLISECONDS;

                        return m_EntranceKhadgarGUID;
                    }
                    default:
                        break;
                }

                if (!m_OperaEncountersGuid.empty())
                {
                    auto const& l_Iter = m_OperaEncountersGuid.find(type);
                    if (l_Iter != m_OperaEncountersGuid.end())
                        return l_Iter->second;
                }

                if (!m_OtherGuidContainer.empty())
                {
                    for (uint64 const& l_Guid : m_OtherGuidContainer)
                    {
                        if (Creature* l_Creature = instance->GetCreature(l_Guid))
                        {
                            if (l_Creature->GetEntry() == type)
                                return l_Guid;
                        }

                        if (GameObject* l_GameObject = instance->GetGameObject(l_Guid))
                        {
                            if (l_GameObject->GetEntry() == type)
                                return l_Guid;
                        }
                    }
                }

                return 0;
            }

            void ActionListGUIDManager(uint8 p_Action, uint8 p_DoActionID = 0)
            {
                if (m_OperaEncountersGuid.empty())
                    return;

                for (auto const& l_Iter : m_OperaEncountersGuid)
                {
                    if (Creature* l_Creature = instance->GetCreature(l_Iter.second))
                    {
                        switch (p_Action)
                        {
                            case eOperaActions::WikketIntroEnd:
                            {
                                l_Creature->SetReactState(REACT_DEFENSIVE);
                                l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

                                if (l_Creature->GetEntry() == eCreatures::Elfyra)
                                {
                                    if (Creature* l_Vehicle = l_Creature->GetVehicleCreatureBase())
                                    {
                                        l_Vehicle->GetVehicleKit()->RemoveAllPassengers();
                                        l_Vehicle->DespawnOrUnsummon(100);
                                    }
                                }

                                break;
                            }
                            case eOperaActions::ZoneInCombat:
                            {
                                if (l_Creature->IsAIEnabled)
                                    l_Creature->AI()->DoZoneInCombat();

                                break;
                            }
                            case eOperaActions::EncounterDespawn:
                            {
                                if (l_Creature->IsAIEnabled)
                                    l_Creature->AI()->DoAction(p_DoActionID);

                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
            }

            void BindPlayer()
            {
                Map::PlayerList const& players = instance->GetPlayers();
                if (!players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        if (Player* pPlayer = itr->getSource())
                            instance->ToInstanceMap()->PermBindAllPlayers(pPlayer);
                    }
                }
            }

            void ProcessEvent(WorldObject* obj, uint32 eventId) override
            {
                if (eventId == 56130 && m_dataPhase2Door != DONE && (instance->GetDifficultyID() == DifficultyHeroic || instance->GetDifficultyID() == DifficultyMythic))
                {
                    m_dataPhase2Door = DONE;
                    SaveToDB();
                    SetData(eData::Phase2Door, DONE);
                }
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                uint32 l_GraveyardID = 5784;

                if (GetBossState(eData::Vizaduum) == DONE)
                    l_GraveyardID = 0;
                else if (GetBossState(eData::ManaDevourer) == DONE)
                    l_GraveyardID = 5824;
                else if (GetBossState(eData::DataShadeOfMedivh) == DONE)
                    l_GraveyardID = 5844;
                else if (GetBossState(eData::DataCurator) == DONE)
                    l_GraveyardID = 5819;
                else if (m_ChallengeMapID == 234 || m_DungeonID == eDungeons::RTKSummit) ///< UPPER (mythic+ and LFG cases)
                    l_GraveyardID = 5906;
                else if (m_EventState[eEventList::EventKhadgarEntrance])
                    l_GraveyardID = 5818;

                if (l_GraveyardID)
                {
                    if (WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(l_GraveyardID))
                    {
                        p_X     = l_Loc->x;
                        p_Y     = l_Loc->y;
                        p_Z     = l_Loc->z;
                        p_O     = l_Loc->o;
                        p_MapID = l_Loc->MapID;
                    }
                }
                else
                {
                    p_X     = 3461.74f;
                    p_Y     = -2026.78f;
                    p_Z     = 1044.20f;
                    p_O     = 1.05f;
                    p_MapID = 1651;
                }
            }

            bool CheckRequiredBosses(uint32 p_BossId, Player const* p_Player) const override
            {
                if (!InstanceScript::CheckRequiredBosses(p_BossId, p_Player))
                    return false;

                switch (p_BossId)
                {
                    case eData::OperaHall:
                        return true;
                    case eData::DataMoroes:
                    case eData::MaidenVirtue:
                    case eData::DataMidnight:
                        return GetBossState(eData::OperaHall) == EncounterState::DONE;
                    case eData::DataCurator:
                    {
                        if (instance->IsMythic() && instance->GetSpawnMode() != Difficulty::DifficultyMythicKeystone)
                            return GetBossState(eData::OperaHall) == EncounterState::DONE;
                        else
                            return true;
                    }
                    case eData::DataNightbane:
                    case eData::DataShadeOfMedivh:
                        return GetBossState(eData::DataCurator) == EncounterState::DONE;
                    default:
                        return GetBossState(p_BossId - 1) == EncounterState::DONE;
                }

                return false;
            }

            bool IsInNoPathArea(Position const& p_Position) const override
            {
                return p_Position.IsInDist(&g_Ships[0], 100.0f) || p_Position.IsInDist(&g_Ships[1], 100.0f);
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (m_NightbaneTimer >= 0)
                    m_NightbaneTimer -= p_Diff;
                else if (!m_EventState[eEventList::EventTimerCheck] && m_EventState[eEventList::EventKhadgarEntrance])
                {
                    m_EventState[EventTimerCheck] = true;

                    bool l_AllFound = true;
                    for (uint64 l_GUID : m_CristalGUIDS)
                    {
                        if (!instance->GetCreature(l_GUID))
                            l_AllFound = false;
                    }

                    if (l_AllFound)
                    {
                        for (uint64 l_GUID : m_CristalGUIDS)
                        {
                            if (Creature* l_Creature = instance->GetCreature(l_GUID))
                                l_Creature->DespawnOrUnsummon();
                        }
                    }
                }

                for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
                {
                    if (Player* player = itr->getSource())
                    {
                        if (player->GetDistance(g_TeleportToMenagerie) < 7.0f)
                        {
                            player->TeleportTo(eWorldSafeLocs::ToCuratorTarget);
                            continue;
                        }

                        if (!m_EventState[eEventList::EventAttumenIntro] && player->GetPositionZ() < 52.0f && player->GetDistance(m_AttumenHome) < 35.0f)
                        {
                            Creature* l_Creature = instance->GetCreature(m_AttumenGUID);
                            if (l_Creature && l_Creature->IsAIEnabled)
                                l_Creature->GetAI()->DoAction(ACTION_9);

                            m_EventState[eEventList::EventAttumenIntro] = true;
                            break;
                        }

                        if (!m_EventState[eEventList::EventMoroesIntro] && player->GetDistance(m_MoroesHome) < 25.0f)
                        {
                            Creature* l_Creature = instance->GetCreature(m_MoroesGUID);
                            if (l_Creature && l_Creature->IsAIEnabled)
                                l_Creature->GetAI()->DoAction(ACTION_1);

                            m_EventState[eEventList::EventMoroesIntro] = true;
                            break;
                        }

                        if (!m_EventState[eEventList::EventChessOutro])
                        {
                            Creature* l_King = instance->GetCreature(m_KingGUID);
                            if (l_King && !l_King->isAlive())
                            {
                                GameObject* l_ShatteredBoard = instance->GetGameObject(m_ShatteredBoardGUID);
                                if (l_ShatteredBoard)
                                {
                                    l_ShatteredBoard->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                                    l_ShatteredBoard->SetGoState(GO_STATE_ACTIVE);
                                }

                                AddTimedDelayedOperation(7 * IN_MILLISECONDS, [this]() -> void
                                {
                                    GameObject* l_GamesmansRoomWall = instance->GetGameObject(m_GamesmansRoomWall);
                                    if (l_GamesmansRoomWall)
                                    {
                                        l_GamesmansRoomWall->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                                        l_GamesmansRoomWall->SetGoState(GO_STATE_ACTIVE);
                                    }
                                });

                                m_EventState[eEventList::EventChessOutro] = true;
                                break;
                            }
                        }

                        if (!m_EventState[eEventList::EventKhadgarChess] && player->GetDistance(m_KhadgarChessHome) < 10.0f)
                        {
                            Creature* l_Creature = instance->GetCreature(m_KhadgarChessGUID);
                            if (l_Creature && l_Creature->IsAIEnabled)
                                l_Creature->GetAI()->DoAction(ACTION_1);

                            m_EventState[eEventList::EventKhadgarChess] = true;
                            break;
                        }

                        if (!m_EventState[eEventList::EventVizaduumIntro] && player->GetDistance(g_NeantspaceEntrance) < 7.0f)
                        {
                            Creature* l_Creature = instance->GetCreature(m_VizaduumGUID);
                            if (l_Creature && l_Creature->IsAIEnabled)
                                l_Creature->GetAI()->DoAction(ACTION_1);

                            m_EventState[eEventList::EventVizaduumIntro] = true;
                            break;
                        }

                        if (GetBossState(eData::Vizaduum) == IN_PROGRESS)
                        {
                            if (player->GetPositionZ() < 800.f)
                            {
                                player->Kill(player);
                                player->TeleportTo(1651, g_NeantspaceEntrance);
                            }
                        }

                        /// Following Events are Disabled in MM+
                        if (IsChallenge())
                            continue;

                        /// First player getting out of Maiden's room trigger the scene
                        if (!m_EventState[eEventList::EventSceneMaidenDead] && (GetBossState(eData::MaidenVirtue) == EncounterState::DONE) && player->GetDistance(m_MaidenHome) > 38.0f)
                        {
                            Position l_AnduinSpawn = { -10910.37f, -2056.85f, 92.18f, 4.89f };
                            Position l_LlaneSpawn = { -10907.18f, -2057.18f, 92.18f, 4.89f };
                            Position l_MedivhSpawn = { -10849.60f, -2104.29f, 93.18f, 2.56f };
                            instance->SummonCreature(eCreatures::AnduinMaiden, l_AnduinSpawn);
                            instance->SummonCreature(eCreatures::LlaneMaiden, l_LlaneSpawn);
                            instance->SummonCreature(eCreatures::MedivhMaiden, l_MedivhSpawn);

                            Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                            if (l_PlayerList.isEmpty())
                                return;

                            for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                    l_Player->PlayStandaloneScene(eScenes::MedivhMaidenDead, 16, *l_Player);
                            }

                            m_EventState[eEventList::EventSceneMaidenDead] = true;
                            break;
                        }

                        if (!m_EventState[eEventList::EventCuratorOutro] && (GetBossState(eData::DataCurator) == EncounterState::DONE))
                        {
                            Position const l_TriggerPos = { -11152.31f, -1894.62f, 165.76f };
                            if (player->GetExactDist(&l_TriggerPos) < 7.0f)
                            {
                                Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                                if (l_PlayerList.isEmpty())
                                    return;

                                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                                {
                                    if (Player* l_Player = l_Iter->getSource())
                                        l_Player->PlayStandaloneScene(eScenes::MedivhCuratorDead, 16, *l_Player);
                                }

                                Creature* l_Creature = instance->GetCreature(m_CuratorGUID);
                                if (l_Creature && l_Creature->IsAIEnabled)
                                    l_Creature->GetAI()->DoAction(ACTION_1);

                                l_Creature = instance->GetCreature(m_CuratorMedivhGUID);
                                if (l_Creature && l_Creature->IsAIEnabled)
                                    l_Creature->GetAI()->DoAction(ACTION_1);

                                m_EventState[eEventList::EventCuratorOutro] = true;
                                break;
                            }
                        }

                        if (!m_EventState[eEventList::EventLibraryMedivh] && (GetBossState(eData::ManaDevourer) == EncounterState::DONE) && player->GetPositionZ() < 729.0f && player->GetPositionZ() > 700.0f)
                        {
                            Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                            if (l_PlayerList.isEmpty())
                                return;

                            for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                    l_Player->PlayStandaloneScene(eScenes::MedivhDevourerDead, 16, *l_Player);
                            }

                            Position const l_LotharSpawn = { 4143.53f, -2068.81f, 727.82f, 1.53f };
                            Position const l_KhadgarSpawn = { 4146.19f, -2068.64f, 727.90f, 1.96f };
                            Position const l_MedivhSpawn = { 4145.50f, -2065.43f, 727.62f, 4.58f };
                            instance->SummonCreature(eCreatures::LotharLibrary, l_LotharSpawn);
                            instance->SummonCreature(eCreatures::ArchmageKhadgarLibrary, l_KhadgarSpawn);
                            instance->SummonCreature(eCreatures::MedivhMaiden, l_MedivhSpawn);

                            m_EventState[eEventList::EventLibraryMedivh] = true;
                            break;
                        }
                    }
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_return_to_karazhan_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_return_to_karazhan()
{
    new instance_return_to_karazhan();
}
#endif
