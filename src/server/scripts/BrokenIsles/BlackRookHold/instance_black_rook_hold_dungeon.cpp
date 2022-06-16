////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "black_rook_hold_dungeon.hpp"

DoorData const g_DoorData[] =
{
    { eGameObjects::AmalgamWall_1,              eData::Amalgam,         DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { eGameObjects::AmalgamWall_2,              eData::Amalgam,         DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { eGameObjects::AmalgamWall_3,              eData::Amalgam,         DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { eGameObjects::AmalgamWall_4,              eData::Amalgam,         DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { eGameObjects::IllysannaDoor1,             eData::Illysanna,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { eGameObjects::IllysannaDoor2,             eData::Illysanna,       DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { eGameObjects::IllysannaDoor3,             eData::Illysanna,       DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { eGameObjects::SmashspiteDoor,             eData::Smashspite,      DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { eGameObjects::SmashspiteToKurtalosDoor1,  eData::Smashspite,      DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { eGameObjects::SmashspiteToKurtalosDoor2,  eData::Smashspite,      DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { eGameObjects::KurtalosDoor,               eData::Kurtalos,        DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { 0,                                        0,                      DOOR_TYPE_ROOM,         BOUNDARY_NONE }
};

class instance_black_rook_hold_dungeon : public InstanceMapScript
{
    public:
        instance_black_rook_hold_dungeon() : InstanceMapScript("instance_black_rook_hold_dungeon", 1501) { }

        struct instance_black_rook_hold_dungeon_InstanceMapScript : public InstanceScript
        {
            instance_black_rook_hold_dungeon_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

            WorldLocation loc_res_pla;

            uint64 m_AmalgamGate;
            uint64 m_IllysannaGate;

            uint64 m_AmbushDoorLeft;
            uint64 m_AmbushDoorRight;

            uint8 m_KurtalosState;
            uint8 m_AmalgamState;

            uint64 m_CommanderShemdahSohn;
            uint64 m_Smashspite;

            bool m_RightDoorOpen;
            uint64 m_RightDoorEntrance;
            uint64 m_LeftDoorEntrance;
            bool m_ChallengeDoorOpened;

            int32 m_BoulderTimer;
            bool m_ChangeRoll;
            bool m_BoulderEnded[2];

            std::set<uint64> m_FelspiteDominators;
            bool m_FelspiteDominatorsDead;
            int32 m_FelBatPupsTimer;

            bool m_WindowAmbush = false;
            bool m_LatosiusConversation = false;
            bool m_IntroLastBoss = false;
            bool m_RookSpiderAmbush = false;
            std::set<uint64> m_SecondAmbushCreatures;
            std::set<uint64> m_SpiderAmbushCreatures;
            std::set<uint64> m_SummonedBats;

            enum eSpells
            {
                ConversationIntroIllysanna = 228134,
                ConversationIntroKurtalos = 205275
            };

            void Initialize() override
            {
                LoadDoorData(g_DoorData);

                m_RightDoorOpen = roll_chance_i(50);
                m_ChallengeDoorOpened = false;

                m_KurtalosState = 0;
                m_AmalgamState = 0;

                m_BoulderTimer = 4 * IN_MILLISECONDS;
                m_BoulderEnded[0] = false;
                m_BoulderEnded[1] = false;
                m_FelspiteDominatorsDead = false;
                m_FelBatPupsTimer = 5 * IN_MILLISECONDS;
                m_ChangeRoll = false;
            }

            bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
            {
                if (p_Target == nullptr)
                    return false;

                switch (p_Target->GetEntry())
                {
                    case eCreatures::SoulEchoesStalker:
                    case eCreatures::EyeBeamStalker:
                    case eCreatures::NPC_LATOSIUS:
                    case eCreatures::NPC_KURTALOS:
                    case eCreatures::NPC_DANTALIONAX:
                    case eCreatures::NPC_SOUL_KURTALOS:
                    case eCreatures::NPC_KURTALOS_BLADE_TRIGGER:
                    case eCreatures::NPC_IMAGE_OF_LATOSIUS:
                    case eCreatures::NPC_CLOUD_OF_HYPNOSIS:
                    case eCreatures::NPC_STINGING_SWARM:
                    case eCreatures::AmalgamofSouls:
                        return false;

                    case eCreatures::RisenCompagnion:
                    {
                        if (p_Affix == Affixes::Bolstering)
                            return false;

                        return false;
                    }

                    default: return true;
                }

                return true;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CommanderShemdahSohn:
                    {
                        m_CommanderShemdahSohn = p_Creature->GetGUID();
                        AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                        {
                            Creature* l_Creature = instance->GetCreature(m_CommanderShemdahSohn);
                            GameObject* l_GobLeft = instance->GetGameObject(m_AmbushDoorLeft);
                            GameObject* l_GobRight = instance->GetGameObject(m_AmbushDoorRight);
                            if (!l_Creature || !l_GobLeft || !l_GobRight)
                                return;

                            if (l_Creature->isAlive())
                            {
                                l_GobLeft->SetGoState(GO_STATE_READY);
                                l_GobRight->SetGoState(GO_STATE_READY);
                            }
                            else
                            {
                                l_GobLeft->SetGoState(GO_STATE_ACTIVE);
                                l_GobRight->SetGoState(GO_STATE_ACTIVE);
                            }
                        });
                        break;
                    }
                    case eCreatures::SmashspiteEntry:
                    {
                        m_Smashspite = p_Creature->GetGUID();
                        if (!p_Creature->isAlive())
                        {
                            m_BoulderEnded[0] = true;
                            m_BoulderEnded[1] = true;
                        }
                        break;
                    }
                    case eCreatures::FelspiteDominator:
                    {
                        if (p_Creature->GetPositionX() >= 3222.0f && p_Creature->GetPositionX() < 3223.0f && p_Creature->isAlive())
                            m_FelspiteDominators.insert(p_Creature->GetGUID());

                        break;
                    }
                    case eCreatures::RisenArcher:
                    case eCreatures::SoulTornChampion:
                    {
                        if (p_Creature->GetPositionZ() >= 92.0f && p_Creature->GetPositionZ() < 94.0f && p_Creature->isAlive())
                            m_SecondAmbushCreatures.insert(p_Creature->GetGUID());

                        break;
                    }
                    case eCreatures::RookSpider:
                    case eCreatures::RookSpiderling:
                    {
                        if (p_Creature->GetPositionZ() >= 71.5f && p_Creature->GetPositionZ() < 72.0f)
                            m_SpiderAmbushCreatures.insert(p_Creature->GetGUID());
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::AmalgamWall_1:
                    case eGameObjects::AmalgamWall_2:
                    case eGameObjects::AmalgamWall_3:
                    case eGameObjects::AmalgamWall_4:
                    case eGameObjects::IllysannaDoor1:
                    case eGameObjects::IllysannaDoor2:
                    case eGameObjects::IllysannaDoor3:
                    case eGameObjects::SmashspiteDoor:
                    case eGameObjects::SmashspiteToKurtalosDoor1:
                    case eGameObjects::SmashspiteToKurtalosDoor2:
                    case eGameObjects::KurtalosDoor:
                    {
                        AddDoor(p_GameObject, true);
                        break;
                    }
                    case eGameObjects::AmalgamExitDoor:
                    {
                        m_AmalgamGate = p_GameObject->GetGUID();

                        AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                        {
                            GameObject* l_Gob = instance->GetGameObject(m_AmalgamGate);
                            if (!l_Gob)
                                return;

                            if (GetBossState(eData::Amalgam) == DONE)
                                l_Gob->SetGoState(GO_STATE_ACTIVE);
                            else
                                l_Gob->SetGoState(GO_STATE_READY);
                        });

                        break;
                    }
                    case eGameObjects::EntranceDoorLeft:
                    {
                        if (!m_RightDoorOpen || IsChallenge())
                            p_GameObject->SetGoState(GO_STATE_ACTIVE);

                        m_RightDoorEntrance = p_GameObject->GetGUID();

                        break;
                    }
                    case eGameObjects::EntranceDoorRight:
                    {
                        if (m_RightDoorOpen || IsChallenge())
                            p_GameObject->SetGoState(GO_STATE_ACTIVE);

                        m_LeftDoorEntrance = p_GameObject->GetGUID();

                        break;
                    }
                    case eGameObjects::AmbushEventDoorLeft:
                    {
                        m_AmbushDoorLeft = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::AmbushEventDoorRight:
                    {
                        m_AmbushDoorRight = p_GameObject->GetGUID();
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
                    case eGameObjects::AmalgamWall_1:
                    case eGameObjects::AmalgamWall_2:
                    case eGameObjects::AmalgamWall_3:
                    case eGameObjects::AmalgamWall_4:
                    case eGameObjects::IllysannaDoor1:
                    case eGameObjects::IllysannaDoor2:
                    case eGameObjects::IllysannaDoor3:
                    case eGameObjects::SmashspiteDoor:
                    case eGameObjects::SmashspiteToKurtalosDoor1:
                    case eGameObjects::SmashspiteToKurtalosDoor2:
                    case eGameObjects::KurtalosDoor:
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

                return true;
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                switch (p_Type)
                {
                    case eData::KurtalosState:
                    {
                        m_KurtalosState = p_Data;
                        break;
                    }
                    case eData::AmalgamOutro:
                    {
                        if (p_Data == 0)
                            m_AmalgamState++;
                        if (p_Data == DONE)
                            HandleGameObject(m_AmalgamGate, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData64(uint32 p_Type, uint64 p_Data) override
            {
                switch (p_Type)
                {
                    case eCreatures::FelspiteDominator:
                    {
                        m_FelspiteDominators.erase(p_Data);
                        break;
                    }
                    case eCreatures::FelBatPup:
                    {
                        m_SummonedBats.erase(p_Data);
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
                    case eData::KurtalosState:
                        return m_KurtalosState;
                    case eData::AmalgamOutro:
                        return m_AmalgamState;
                }

                return 0;
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eGameObjects::AmalgamExitDoor:
                        return m_AmalgamGate;
                    case eGameObjects::AmbushEventDoorLeft:
                        return m_AmbushDoorLeft;
                    case eGameObjects::AmbushEventDoorRight:
                        return m_AmbushDoorRight;
                    case eCreatures::CommanderShemdahSohn:
                        return m_CommanderShemdahSohn;
                    default:
                        break;
                }

                return 0;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                uint32 l_Graveyard = 5352;

                if (GetBossState(eData::Smashspite) == DONE)
                    l_Graveyard = 5486;
                else if (GetBossState(eData::Illysanna) == DONE)
                    l_Graveyard = 5485;
                else if (GetBossState(eData::Amalgam) == DONE)
                    l_Graveyard = 5484;

                if (WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(l_Graveyard))
                {
                    p_X     = l_Loc->x;
                    p_Y     = l_Loc->y;
                    p_Z     = l_Loc->z;
                    p_O     = l_Loc->o;
                    p_MapID = l_Loc->MapID;
                }
            }

            std::string GetSaveData() override
            {
                std::ostringstream l_SaveStream;
                l_SaveStream << "B R H " << GetBossSaveData() << " " << m_AmalgamState;
                return l_SaveStream.str();
            }

            void Load(char const* p_Data) override
            {
                if (!p_Data)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(p_Data);

                char l_DataHead1, l_DataHead2, l_DataHead3;
                std::istringstream l_LoadStream(p_Data);
                l_LoadStream >> l_DataHead1 >> l_DataHead2 >> l_DataHead3;

                if (l_DataHead1 == 'B' && l_DataHead2 == 'R' && l_DataHead3 == 'H')
                {
                    for (uint32 l_I = 0; l_I < MAX_ENCOUNTER; ++l_I)
                    {
                        uint32 l_TmpState;

                        l_LoadStream >> l_TmpState;

                        if (l_TmpState == IN_PROGRESS || l_TmpState > SPECIAL)
                            l_TmpState = NOT_STARTED;

                        SetBossState(l_I, EncounterState(l_TmpState));
                    }

                    l_LoadStream >> m_AmalgamState;
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (IsChallenge() && !m_ChallengeDoorOpened)
                {
                    GameObject* l_RightDoor = instance->GetGameObject(m_RightDoorEntrance);
                    GameObject* l_LeftDoor = instance->GetGameObject(m_LeftDoorEntrance);
                    if (l_RightDoor && l_LeftDoor)
                    {
                        l_RightDoor->SetGoState(GO_STATE_ACTIVE);
                        l_LeftDoor->SetGoState(GO_STATE_ACTIVE);
                        m_ChallengeDoorOpened = true;
                    }
                }

                bool l_BatCanSpawn = false;
                bool l_WindowAmbushCanStart = false;
                bool l_TalkLatosius = false;
                bool l_IntroLastBoss = false;
                bool l_RookSpiderAmbush = false;
                for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
                {
                    if (Player* player = itr->getSource())
                    {
                        if (player->m_positionZ > 70.0f)
                            l_RookSpiderAmbush = true;

                        if (player->m_positionZ > 235.0f)
                            l_IntroLastBoss = true;

                        if (player->m_positionZ < 30.0f && player->m_positionY < 7459.0f)
                            l_TalkLatosius = true;

                        if (player->m_positionZ > 83.5f && player->m_positionX < 3076.0f && player->m_positionY < 7350.0f)
                            l_WindowAmbushCanStart = true;

                        if (player->m_positionZ > 195.0f)
                        {
                            m_BoulderEnded[1] = true;
                            l_BatCanSpawn = true;
                        }

                        if (player->m_positionZ > 129.0f)
                            m_BoulderEnded[0] = true;
                    }
                }

                if (!m_RookSpiderAmbush && l_RookSpiderAmbush)
                {
                    m_RookSpiderAmbush = true;
                    uint32 l_I = 0;
                    std::array<Position, 3> l_JumpPositions =
                    { {
                        { 3128.06f, 7434.75f, 72.58f, 5.76f },
                        { 3126.46f, 7433.37f, 72.58f, 6.06f },
                        { 3126.81f, 7431.71f, 72.58f, 5.98f }
                    } };

                    for (uint64 l_GUID : m_SpiderAmbushCreatures)
                    {
                        Creature* l_Creature = instance->GetCreature(l_GUID);
                        if (!l_Creature || !l_Creature->isAlive())
                            continue;

                        switch (l_Creature->GetEntry())
                        {
                            case eCreatures::RookSpiderling:
                                l_Creature->GetMotionMaster()->MoveJump(l_JumpPositions[l_I], 30.0f, 10.0f);
                                l_Creature->SetHomePosition(l_JumpPositions[l_I]);
                                l_I++;
                                break;
                            case eCreatures::RookSpider:
                                l_Creature->GetMotionMaster()->MoveJump(l_JumpPositions[2], 30.0f, 5.0f);
                                l_Creature->SetHomePosition(l_JumpPositions[2]);
                                break;
                            default:
                                break;
                        }
                    }
                }

                if (!m_IntroLastBoss && l_IntroLastBoss)
                {
                    m_IntroLastBoss = true;
                    if (Creature* l_Creature = instance->GetCreature(m_Smashspite))
                        l_Creature->CastSpell(l_Creature, eSpells::ConversationIntroKurtalos, true);
                }

                if (!m_LatosiusConversation && l_TalkLatosius)
                {
                    m_LatosiusConversation = true;
                    if (Creature* l_Creature = instance->GetCreature(m_CommanderShemdahSohn))
                        l_Creature->CastSpell(l_Creature, eSpells::ConversationIntroIllysanna, true);
                }

                if (!m_WindowAmbush && l_WindowAmbushCanStart)
                {
                    m_WindowAmbush = true;
                    if (GetBossState(eData::Illysanna) != DONE && !m_SecondAmbushCreatures.empty())
                    {
                        uint32 l_I = 0;
                        std::array<Position, 3> l_JumpPositions =
                        { {
                            { 3041.96f, 7326.21f, 89.75f, 6.14f },
                            { 3042.06f, 7322.09f, 89.04f, 0.13f },
                            { 3046.09f, 7331.57f, 89.04f, 5.71f }
                        } };

                        for (uint64 l_GUID : m_SecondAmbushCreatures)
                        {
                            Creature* l_Creature = instance->GetCreature(l_GUID);
                            if (!l_Creature || !l_Creature->isAlive())
                                continue;

                            switch (l_Creature->GetEntry())
                            {
                                case eCreatures::RisenArcher:
                                    l_Creature->GetMotionMaster()->MoveJump(l_JumpPositions[l_I], 20.0f, 20.0f);
                                    l_Creature->SetHomePosition(l_JumpPositions[l_I]);
                                    l_I++;
                                    break;
                                case eCreatures::SoulTornChampion:
                                    l_Creature->GetMotionMaster()->MoveJump(l_JumpPositions[2], 20.0f, 20.0f);
                                    l_Creature->SetHomePosition(l_JumpPositions[2]);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }

                if (Creature* l_Creature = instance->GetCreature(m_Smashspite))
                {
                    if (l_Creature->isAlive() && GetBossState(eData::Smashspite) != IN_PROGRESS)
                    {
                        if (!m_BoulderEnded[1])
                        {
                            m_BoulderTimer -= p_Diff;
                            if (m_BoulderTimer < 0)
                            {
                                m_BoulderTimer = 4 * IN_MILLISECONDS;

                                Position l_UpperSpawnPosition;
                                Position l_LowerSpawnPosition;

                                if (m_ChangeRoll)
                                {
                                    l_LowerSpawnPosition = { 3178.931f, 7312.166f, 129.7976f };
                                    l_UpperSpawnPosition = { 3175.473f, 7396.355f, 195.5903f };
                                }
                                else
                                {
                                    l_LowerSpawnPosition = { 3174.345f, 7311.097f, 129.741f };
                                    l_UpperSpawnPosition = { 3173.466f, 7400.026f, 195.7867f };
                                }

                                m_ChangeRoll = !m_ChangeRoll;
                                Creature* l_Boulder = nullptr;

                                if (!m_BoulderEnded[0])
                                {
                                    SpellDestination const l_LowerSpellDest(l_LowerSpawnPosition);
                                    l_Boulder = l_Creature->SummonCreature(eCreatures::Boulder, &l_LowerSpellDest);
                                }

                                if (!m_BoulderEnded[1])
                                {
                                    if (m_BoulderEnded[0])
                                        m_BoulderTimer = 4750;

                                    SpellDestination const l_UpperSpellDest(l_UpperSpawnPosition);
                                    l_Boulder = l_Creature->SummonCreature(eCreatures::Boulder, &l_UpperSpellDest);
                                }

                                if (l_Boulder)
                                {
                                    float l_Rate = !m_ChangeRoll ? 1.15f : 0.9f;
                                    l_Boulder->SetSpeed(UnitMoveType::MOVE_RUN, l_Boulder->GetSpeedRate(UnitMoveType::MOVE_RUN) * l_Rate, true);
                                }
                            }
                        }
                        else if (!m_FelspiteDominatorsDead && l_BatCanSpawn)
                        {
                            if (!m_FelspiteDominators.empty())
                            {
                                m_FelBatPupsTimer -= p_Diff;
                                if (m_FelBatPupsTimer < 0)
                                {
                                    m_FelBatPupsTimer = 8 * IN_MILLISECONDS;

                                    if (m_SummonedBats.size() < 15)
                                    {
                                        std::vector<Position> m_Spawns =
                                        {
                                            { 3240.519f, 7322.328f, 231.1545f },
                                            { 3236.863f, 7320.938f, 231.0745f },
                                            { 3241.514f, 7323.298f, 231.0813f }
                                        };

                                        for (Position l_Pos : m_Spawns)
                                        {
                                            SpellDestination const l_SpellDest(l_Pos);
                                            TempSummon* l_Bat = l_Creature->SummonCreature(eCreatures::FelBatPup, &l_SpellDest);
                                            if (l_Bat)
                                                m_SummonedBats.insert(l_Bat->GetGUID());
                                        }
                                    }
                                }
                            }
                            else
                                m_FelspiteDominatorsDead = true;
                        }
                    }
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_black_rook_hold_dungeon_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_black_rook_hold_dungeon()
{
    new instance_black_rook_hold_dungeon();
}
#endif
