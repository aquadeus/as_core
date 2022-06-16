////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

class instance_violet_hold_legion : public InstanceMapScript
{
public:
    instance_violet_hold_legion() : InstanceMapScript("instance_violet_hold_legion", 1544) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_violet_hold_legion_InstanceMapScript(map);
    }

    struct instance_violet_hold_legion_InstanceMapScript : public InstanceScript
    {
        instance_violet_hold_legion_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            SetBossNumber(MAX_ENCOUNTER);
        }

        uint64 uiKaahrj;
        uint64 uiMillificent;
        uint64 uiFesterface;
        uint64 uiShivermaw;
        uint64 uiAnubesset;
        uint64 uiSaelorn;
        uint64 uiThalena;
        uint64 uiMalgath;
        uint64 uiBetrug;

        uint64 uiKaahrjCell;
        uint64 uiMillificentCell;
        uint64 uiFesterfaceCell;
        uint64 uiShivermawCell;
        uint64 uiAnubessetCell;
        uint64 uiThalenaCell;

        uint64 uiMainDoor;
        uint64 uiActivationCrystal[4];
        uint64 uiSinclari;
        uint64 uiTeleportationPortal;

        GuidSet trashMobs;

        uint8 uiMainEventPhase;
        uint8 uiWaveCount;
        uint8 uiLocation;
        uint8 uiFirstBoss;
        uint8 uiSecondBoss;
        uint8 uiThirdBoss;
        uint8 uiRemoveNpc;
        uint8 uiDoorIntegrity;
        uint8 uiCountActivationCrystals;

        uint16 m_auiEncounter[MAX_ENCOUNTER];

        uint32 uiActivationTimer;

        bool bActive;
        bool bWiped;
        bool bCrystalActivated;
        bool defenseless;

        bool m_Integrity75Warned;
        bool m_Integrity50Warned;
        bool m_Integrity25Warned;

        bool m_IntroDone;

        std::string str_data;

        void Initialize() override
        {
            uiKaahrj = 0;
            uiMillificent = 0;
            uiFesterface = 0;
            uiShivermaw = 0;
            uiAnubesset = 0;
            uiSaelorn = 0;
            uiThalena = 0;
            uiMalgath = 0;
            uiBetrug = 0;

            uiKaahrjCell = 0;
            uiMillificentCell = 0;
            uiFesterfaceCell = 0;
            uiShivermawCell = 0;
            uiAnubessetCell = 0;
            uiThalenaCell = 0;

            uiMainDoor = 0;
            uiSinclari = 0;
            uiTeleportationPortal = 0;

            trashMobs.clear();

            uiWaveCount = 0;
            uiLocation = urand(0, 4);

            uint64 l_Data = sWorld->getWorldState(WS_VIOLET_HOLD_WEEKLY_ROTATION);
            if (l_Data == 0)
            {
                uiFirstBoss = urand(0, 5);

                do
                {
                    uiSecondBoss = urand(0, 5);
                }
                while (uiFirstBoss == uiSecondBoss);

                uiThirdBoss = urand(0, 1) ? DATA_SAELORN : DATA_BETRUG;

                l_Data = uiFirstBoss | (uiSecondBoss << 8) | (uiThirdBoss << 16);

                sWorld->setWorldState(WS_VIOLET_HOLD_WEEKLY_ROTATION, l_Data);
            }
            else
            {
                uiFirstBoss = l_Data & 0xFF;
                uiSecondBoss = (l_Data >> 8) & 0xFF;
                uiThirdBoss = (l_Data >> 16) & 0xFF;
            }

            uiRemoveNpc = 0;
            uiCountActivationCrystals = 0;

            uiDoorIntegrity = 100;
            uiActivationTimer = 5000;

            bActive = false;
            bCrystalActivated = false;
            defenseless = true;
            uiMainEventPhase = NOT_STARTED;

            memset(&m_auiEncounter, 0, sizeof (m_auiEncounter));

            m_Integrity75Warned = false;
            m_Integrity50Warned = false;
            m_Integrity25Warned = false;

            m_IntroDone = false;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_LIEUTENANT_SINCLARI:
                    uiSinclari = creature->GetGUID();
                    break;
                case NPC_MINDFLAYER_KAAHRJ:
                    uiKaahrj = creature->GetGUID();
                    break;
                case NPC_MILLIFICENT_MANASTORM:
                    uiMillificent = creature->GetGUID();
                    break;
                case NPC_FESTERFACE:
                    uiFesterface = creature->GetGUID();
                    break;
                case NPC_SHIVERMAW:
                    uiShivermaw = creature->GetGUID();
                    break;
                case NPC_ANUBESSET:
                    uiAnubesset = creature->GetGUID();
                    break;
                case NPC_SAELORN:
                    uiSaelorn = creature->GetGUID();
                    break;
                case NPC_PRINCESS_THALENA:
                    uiThalena = creature->GetGUID();
                    break;
                case NPC_LORD_MALGATH:
                    uiMalgath = creature->GetGUID();
                    break;
                case NPC_FEL_LORD_BETRUG:
                    uiBetrug = creature->GetGUID();
                    break;
                case NPC_VIOLET_HOLD_GUARD:
                {
                    creature->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_STUNNED);
                    creature->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_29);
                    creature->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_FEIGN_DEATH);
                    break;
                }
                default:
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_MAIN_DOOR:
                    uiMainDoor = go->GetGUID();
                    break;
                case GO_KAAHRJ_DOOR:
                    uiKaahrjCell = go->GetGUID();
                    break;
                case GO_MILLIFICENT_DOOR:
                    uiMillificentCell = go->GetGUID();
                    break;
                case GO_FESTERFACE_DOOR:
                    uiFesterfaceCell = go->GetGUID();
                    break;
                case GO_SHIVERMAW_DOOR:
                    uiShivermawCell = go->GetGUID();
                    break;
                case GO_ANUBESSET_DOOR:
                    uiAnubessetCell = go->GetGUID();
                    break;
                case GO_THALENA_DOOR:
                    uiThalenaCell = go->GetGUID();
                    break;
                case GO_ACTIVATION_CRYSTAL:
                    if (uiCountActivationCrystals < 4)
                        uiActivationCrystal[uiCountActivationCrystals++] = go->GetGUID();
                    break;
                default:
                    break;
            }
        }

        void HandlePlayerKilled(Player* p_Player) override
        {
            p_Player->CastSpell(p_Player, otherSpells::SPELL_DEATH_PENALTY, true);
        }

        void OnPlayerEnter(Player* p_Player) override
        {
            InstanceScript::OnPlayerEnter(p_Player);

            if (!m_IntroDone)
            {
                uint64 l_Guid = p_Player->GetGUID();
                AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                {
                    if (Player* l_Player = sObjectAccessor->FindPlayer(l_Guid))
                    {
                        if (l_Player->GetMapId() != instance->GetId())
                            return;

                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3418, l_Player, nullptr, *l_Player))
                            delete l_Conversation;
                    }
                });

                m_IntroDone = true;
            }
        }

        bool IsEncounterInProgress(int32 /*p_ExcludeBossID = -1*/) const override
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    return true;

            return false;
        }

        bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
        {
            if (!p_Target)
                return false;

            if (Creature const* l_Creature = p_Target->ToCreature())
            {
                if (l_Creature->IsDungeonBoss())
                    return false;
            }

            return true;

        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            if (state == DONE)
            {
                if (type != DATA_BETRUG && type != DATA_SAELORN)
                {
                    if (GetData(DATA_WAVE_COUNT) == 6)
                    {
                        SetData(DATA_1ST_BOSS_EVENT, DONE);
                        SetData(DATA_WAVE_COUNT, 7);
                    }
                    else if (GetData(DATA_WAVE_COUNT) == 12)
                    {
                        SetData(DATA_2ND_BOSS_EVENT, DONE);
                        SetData(DATA_WAVE_COUNT, 13);
                    }
                }
                else
                    SetData(DATA_3RD_BOSS_EVENT, DONE);
            }

            return true;
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_1ST_BOSS_EVENT:
                    m_auiEncounter[0] = data;
                    if (data == DONE)
                        SaveToDB();
                    break;
                case DATA_2ND_BOSS_EVENT:
                    m_auiEncounter[1] = data;
                    if (data == DONE)
                        SaveToDB();
                    break;
                case DATA_3RD_BOSS_EVENT:
                    m_auiEncounter[2] = data;
                    if (data == DONE)
                    {
                        SetData(DATA_MAIN_DOOR, GO_STATE_ACTIVE);
                        uiMainEventPhase = DONE;
                        SaveToDB();
                    }
                    break;
                case DATA_START_BOSS_ENCOUNTER:
                    StartBossEncounter(uiWaveCount == 6 ? uiFirstBoss : uiSecondBoss);
                    break;
                case DATA_WAVE_COUNT:
                    uiWaveCount = data;
                    bActive = true;
                    break;
                case DATA_REMOVE_NPC:
                    uiRemoveNpc = data;
                    break;
                case DATA_PORTAL_LOCATION:
                    uiLocation = (uint8)data;
                    break;
                case DATA_MAIN_DOOR:
                    if (GameObject* pMainDoor = instance->GetGameObject(uiMainDoor))
                    {
                        switch (data)
                        {
                            case GO_STATE_ACTIVE:
                                pMainDoor->SetGoState(GO_STATE_ACTIVE);
                                break;
                            case GO_STATE_READY:
                                pMainDoor->SetGoState(GO_STATE_READY);
                                break;
                            case GO_STATE_ACTIVE_ALTERNATIVE:
                                pMainDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                                break;
                        }
                    }
                    break;
                case DATA_DOOR_INTEGRITY:
                    if (!uiDoorIntegrity)
                        break;

                    uiDoorIntegrity = data;
                    defenseless = false;
                    DoUpdateWorldState(eWorldStates::WORLD_STATE_VH_PRISON_STATE, uiDoorIntegrity);

                    if (uiDoorIntegrity == 0)
                    {
                        if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                        {
                            if (Creature* malgath = pSinclari->SummonCreature(NPC_LORD_MALGATH, MiddleRoomSaboLoc, TEMPSUMMON_TIMED_DESPAWN, 1000))
                            {
                                if (malgath->IsAIEnabled)
                                    malgath->AI()->ZoneTalk(6);
                            }
                        }
                    }
                    else if (uiDoorIntegrity <= 25 && !m_Integrity25Warned)
                    {
                        m_Integrity25Warned = true;

                        if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                        {
                            if (pSinclari->IsAIEnabled)
                                pSinclari->AI()->ZoneTalk(7);
                        }
                    }
                    else if (uiDoorIntegrity <= 50 && !m_Integrity50Warned)
                    {
                        m_Integrity50Warned = true;

                        if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                        {
                            if (pSinclari->IsAIEnabled)
                                pSinclari->AI()->ZoneTalk(6);
                        }
                    }
                    else if (uiDoorIntegrity <= 75 && !m_Integrity75Warned)
                    {
                        m_Integrity75Warned = true;

                        if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                        {
                            if (pSinclari->IsAIEnabled)
                                pSinclari->AI()->ZoneTalk(5);
                        }
                    }

                    break;
                case DATA_MAIN_EVENT_PHASE:
                    uiMainEventPhase = data;
                    if (data == IN_PROGRESS) // Start event
                    {
                        SetData(DATA_MAIN_DOOR, GO_STATE_READY);
                        uiWaveCount = 1;
                        bActive = true;
                        for (int i = 0; i < 4; ++i)
                            if (GameObject* crystal = instance->GetGameObject(uiActivationCrystal[i]))
                                crystal->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        uiRemoveNpc = 0; // might not have been reset after a wipe on a boss.
                    }
                    break;
                case eData::DATA_BLACK_BILE_KILLED:
                {
                    Map::PlayerList const& l_Players = instance->GetPlayers();
                    for (auto l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                    {
                        if (Player* l_Player = l_Itr->getSource())
                            l_Player->SetWorldState(ePlayerWorldStates::WORLD_STATE_BLACK_BILE, 1);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void SetData64(uint32 type, uint64 data) override
        {
            switch (type)
            {
                case DATA_ADD_TRASH_MOB:
                    trashMobs.insert(data);
                    break;
                case DATA_DEL_TRASH_MOB:
                    trashMobs.erase(data);
                    break;
            }
        }

        uint32 GetData(uint32 type) override
        {
            switch (type)
            {
                case DATA_MAIN_EVENT_PHASE:
                    return uiMainEventPhase;
                case DATA_1ST_BOSS_EVENT:
                    return m_auiEncounter[0];
                case DATA_2ND_BOSS_EVENT:
                    return m_auiEncounter[1];
                case DATA_3RD_BOSS_EVENT:
                    return m_auiEncounter[2];
                case DATA_PORTAL_LOCATION:
                    return uiLocation;
                case DATA_DOOR_INTEGRITY:
                    return uiDoorIntegrity;
                case DATA_REMOVE_NPC:
                    return uiRemoveNpc;
                case DATA_WAVE_COUNT:
                    return uiWaveCount;
                case DATA_FIRST_BOSS:
                    return uiFirstBoss;
                case DATA_SECOND_BOSS:
                    return uiSecondBoss;
                case DATA_THIRD_BOSS:
                    return uiThirdBoss;
            }

            return 0;
        }

        uint64 GetData64(uint32 type) override
        {
            switch (type)
            {
                case DATA_SINCLARI:
                    return uiSinclari;
                case DATA_TELEPORTATION_PORTAL:
                    return uiTeleportationPortal;
                case DATA_KAAHRJ:
                    return uiKaahrj;
                case DATA_MILLIFICENT:
                    return uiMillificent;
                case DATA_FESTERFACE:
                    return uiFesterface;
                case DATA_SHIVERMAW:
                    return uiShivermaw;
                case DATA_ANUBESSET:
                    return uiAnubesset;
                case DATA_SAELORN:
                    return uiSaelorn;
                case DATA_THALENA:
                    return uiThalena;
                case DATA_BETRUG:
                    return uiBetrug;
                case DATA_MAIN_DOOR:
                    return uiMainDoor;
                //Door Data
                case DATA_KAAHRJ_CELL:
                    return uiKaahrjCell;
                case DATA_MILLIFICENT_CELL:
                    return uiMillificentCell;
                case DATA_FESTERFACE_CELL:
                    return uiFesterfaceCell;
                case DATA_SHIVERMAW_CELL:
                    return uiShivermawCell;
                case DATA_ANUBESSET_CELL:
                    return uiAnubessetCell;
                case DATA_THALENA_CELL:
                    return uiThalenaCell;
            }
            return 0;
        }

        void SpawnPortal()
        {
            SetData(DATA_PORTAL_LOCATION, (GetData(DATA_PORTAL_LOCATION) + urand(0, 7)) % 7);

            if (Creature* pSinclari = instance->GetCreature(uiSinclari))
            {
                if (Creature* portal = pSinclari->SummonCreature(NPC_TELEPORTATION_PORTAL, g_PortalLocation[GetData(DATA_PORTAL_LOCATION)], TEMPSUMMON_CORPSE_DESPAWN))
                    uiTeleportationPortal = portal->GetGUID();
            }
        }

        void StartBossEncounter(uint8 uiBoss, bool bForceRespawn = true)
        {
            Creature* pBoss = nullptr;

            switch (uiBoss)
            {
                case DATA_KAAHRJ:
                    HandleGameObject(uiKaahrjCell, bForceRespawn);
                    pBoss = instance->GetCreature(uiKaahrj);
                    break;
                case DATA_MILLIFICENT:
                    HandleGameObject(uiMillificentCell, bForceRespawn);
                    pBoss = instance->GetCreature(uiMillificent);
                    break;
                case DATA_FESTERFACE:
                    HandleGameObject(uiFesterfaceCell, bForceRespawn);
                    pBoss = instance->GetCreature(uiFesterface);
                    break;
                case DATA_SHIVERMAW:
                    HandleGameObject(uiShivermawCell, bForceRespawn);
                    pBoss = instance->GetCreature(uiShivermaw);
                    break;
                case DATA_ANUBESSET:
                    HandleGameObject(uiAnubessetCell, bForceRespawn);
                    pBoss = instance->GetCreature(uiAnubesset);
                    break;
                case DATA_THALENA:
                    HandleGameObject(uiThalenaCell, bForceRespawn);
                    pBoss = instance->GetCreature(uiThalena);
                    break;
            }

            // generic boss state changes
            if (pBoss)
            {
                if (!bForceRespawn)
                {
                    if (pBoss->isDead())
                    {
                        // respawn but avoid to be looted again
                        pBoss->Respawn();
                        pBoss->AI()->DoAction(ACTION_REMOVE_LOOT);
                    }
                    pBoss->SetReactState(REACT_PASSIVE);
                    pBoss->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                    pBoss->SetHomePosition(pBoss->GetHomePosition().GetPositionX(), pBoss->GetHomePosition().GetPositionY(), pBoss->GetHomePosition().GetPositionZ(), pBoss->GetHomePosition().GetOrientation());
                    pBoss->NearTeleportTo(pBoss->GetHomePosition().GetPositionX(), pBoss->GetHomePosition().GetPositionY(), pBoss->GetHomePosition().GetPositionZ(), pBoss->GetHomePosition().GetOrientation());
                    uiWaveCount = 0;

                    if (Creature* betrug = instance->GetCreature(uiBetrug))
                    {
                        betrug->SetReactState(REACT_PASSIVE);
                        betrug->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        betrug->SetHomePosition(betrug->GetHomePosition().GetPositionX(), betrug->GetHomePosition().GetPositionY(), betrug->GetHomePosition().GetPositionZ(), betrug->GetHomePosition().GetOrientation());
                        betrug->NearTeleportTo(betrug->GetHomePosition().GetPositionX(), betrug->GetHomePosition().GetPositionY(), betrug->GetHomePosition().GetPositionZ(), 2.99f);
                        betrug->SetVisible(false);
                    }

                    if (Creature* l_Saelorn = instance->GetCreature(uiSaelorn))
                    {
                        l_Saelorn->SetReactState(REACT_PASSIVE);
                        l_Saelorn->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        l_Saelorn->SetHomePosition(l_Saelorn->GetHomePosition().GetPositionX(), l_Saelorn->GetHomePosition().GetPositionY(), l_Saelorn->GetHomePosition().GetPositionZ(), l_Saelorn->GetHomePosition().GetOrientation());
                        l_Saelorn->NearTeleportTo(l_Saelorn->GetHomePosition().GetPositionX(), l_Saelorn->GetHomePosition().GetPositionY(), l_Saelorn->GetHomePosition().GetPositionZ(), 2.99f);
                        l_Saelorn->SetVisible(false);
                    }
                }
                else
                {
                    Position l_Pos      = bossStartMove[uiBoss];
                    l_Pos.m_orientation = pBoss->GetAngle(&centrPos);

                    pBoss->SetHomePosition(l_Pos);
                    pBoss->GetMotionMaster()->MovePoint(1, l_Pos);
                    if (uiBoss == 1) //manastorm event
                        pBoss->AI()->DoAction(1);
                    else
                    {
                        pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC|UNIT_FLAG_NON_ATTACKABLE);
                        pBoss->SetReactState(REACT_AGGRESSIVE);
                    }
                }
            }
        }

        void AddWave()
        {
            DoUpdateWorldState(eWorldStates::WORLD_STATE_VH, 1);

            switch (uiWaveCount)
            {
                case 6:
                case 12:
                case 18:
                {
                    if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                    {
                        if (Creature* malgath = pSinclari->SummonCreature(NPC_LORD_MALGATH, MiddleRoomSaboLoc, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
                            malgath->CastSpell(malgath, SPELL_FEL_SHIELD, true);
                    }
                    break;
                }
                case 1:
                {
                    SetData(DATA_MAIN_DOOR, GO_STATE_READY);
                    DoUpdateWorldState(eWorldStates::WORLD_STATE_VH_PRISON_STATE, 100);
                    // no break
                }
                default:
                    SpawnPortal();
                    break;
            }
        }

        std::string GetSaveData() override
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "V H " << (uint16)m_auiEncounter[0]
                << ' ' << (uint16)m_auiEncounter[1]
                << ' ' << (uint16)m_auiEncounter[2]
                << ' ' << (uint16)uiFirstBoss
                << ' ' << (uint16)uiSecondBoss;

            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
        }

        void Load(const char* in) override
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            uint16 data0, data1, data2, data3, data4;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4;

            if (dataHead1 == 'V' && dataHead2 == 'H')
            {
                m_auiEncounter[0] = data0;
                m_auiEncounter[1] = data1;
                m_auiEncounter[2] = data2;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

                uiFirstBoss = uint8(data3);
                uiSecondBoss = uint8(data4);
            } else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        bool CheckWipe()
        {
            Map::PlayerList const &players = instance->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                Player* player = itr->getSource();
                if (player->isGameMaster())
                    continue;

                if (player->isAlive())
                    return false;
            }

            return true;
        }

        void Reset_Event()
        {
            uiMainEventPhase = NOT_STARTED;
            uiDoorIntegrity = 100;
            SetData(DATA_REMOVE_NPC, 1);
            StartBossEncounter(uiFirstBoss, false);
            StartBossEncounter(uiSecondBoss, false);
            SetData(DATA_MAIN_DOOR, GO_STATE_ACTIVE);
            SetData(DATA_WAVE_COUNT, 0);
            DoUpdateWorldState(eWorldStates::WORLD_STATE_VH, 0);

            for (int i = 0; i < 4; ++i)
                if (GameObject* crystal = instance->GetGameObject(uiActivationCrystal[i]))
                    crystal->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

            for (GuidSet::const_iterator itr = trashMobs.begin(); itr != trashMobs.end(); ++itr)
            {
                if (Creature* creature = instance->GetCreature(*itr))
                    if (creature && creature->isAlive())
                        creature->DespawnOrUnsummon();
            }
            trashMobs.clear();

            if (Creature* pSinclari = instance->GetCreature(uiSinclari))
            {
                pSinclari->SetVisible(true);
                std::list<Creature*> GuardList;
                pSinclari->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
                if (!GuardList.empty())
                {
                    for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                    {
                        if (Creature* pGuard = *itr)
                        {
                            pGuard->SetVisible(true);
                            pGuard->SetReactState(REACT_AGGRESSIVE);
                            pGuard->GetMotionMaster()->MovePoint(1, pGuard->GetHomePosition());
                        }
                    }
                }
                pSinclari->GetMotionMaster()->MovePoint(1, pSinclari->GetHomePosition());
                pSinclari->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pSinclari->AI()->DoAction(true);
            }
        }

        void Update(uint32 diff) override
        {
            UpdateOperations(diff);

            if (!instance->HavePlayers())
                return;

            // portals should spawn if other portal is dead and doors are closed
            if (bActive && uiMainEventPhase == IN_PROGRESS)
            {
                if (uiActivationTimer < diff)
                {
                    AddWave();
                    bActive = false;
                    // 30 seconds waiting time after each boss fight
                    uiActivationTimer = (uiWaveCount == 6 || uiWaveCount == 12) ? 30000 : 5000;
                } else uiActivationTimer -= diff;
            }

            // if main event is in progress and players have wiped then reset instance
            //if (uiMainEventPhase == IN_PROGRESS && CheckWipe())
            //    Reset_Event();

            if (!GetData(DATA_DOOR_INTEGRITY) && uiMainEventPhase == IN_PROGRESS)
            {
                uiMainEventPhase = NOT_STARTED;
                Reset_Event();
            }
        }

        void ActivateCrystal()
        {
            // just to make things easier we'll get the gameobject from the map
            GameObject* invoker = instance->GetGameObject(uiActivationCrystal[0]);
            if (!invoker)
                return;

            SpellInfo const* spellInfoLightning = sSpellMgr->GetSpellInfo(SPELL_ARCANE_LIGHTNING);
            if (!spellInfoLightning)
                return;

            // the orb
            TempSummon* trigger = invoker->SummonCreature(NPC_DEFENSE_SYSTEM, MiddleRoomSaboLoc, TEMPSUMMON_MANUAL_DESPAWN, 0);
            if (!trigger)
                return;

            // visuals
            trigger->CastSpell(trigger, spellInfoLightning, true, 0, 0, trigger->GetGUID());

            // Kill all mobs registered with SetGuidData(ADD_TRASH_MOB)
            for (GuidSet::const_iterator itr = trashMobs.begin(), next; itr != trashMobs.end(); itr = next)
            {
                next = itr;
                ++next;

                Creature* creature = instance->GetCreature(*itr);
                if (creature && creature->isAlive())
                    trigger->Kill(creature);
            }
        }

        void ProcessEvent(WorldObject* /*go*/, uint32 uiEventId) override
        {
            switch (uiEventId)
            {
                case EVENT_ACTIVATE_CRYSTAL:
                    bCrystalActivated = true; // Activation by player's will throw event signal
                    ActivateCrystal();
                    break;
            }
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_instance_violet_hold_legion()
{
    new instance_violet_hold_legion();
}
#endif
