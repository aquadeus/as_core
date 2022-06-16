////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_emerald_nightmare.hpp"

DoorData const g_DoorData[] =
{
    { GO_NYTHENDRA_DOOR_1,       DATA_NYTHENDRA,         DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_NYTHENDRA_DOOR_2,       DATA_NYTHENDRA,         DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_NYTHENDRA_DOOR_3,       DATA_NYTHENDRA,         DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_NYTHENDRA_DOOR_4,       DATA_NYTHENDRA,         DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_NYTHENDRA_DOOR_5,       DATA_NYTHENDRA,         DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_NYTHENDRA_DOOR_6,       DATA_NYTHENDRA,         DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_RENFERAL_DOOR,          DATA_RENFERAL,          DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_ILGYNOTH_DOOR_1,        DATA_ILGYNOTH,          DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_ILGYNOTH_DOOR_2,        DATA_ILGYNOTH,          DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_ILGYNOTH_DOOR_3,        DATA_ILGYNOTH,          DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_URSOC_DOOR_1,           DATA_URSOC,             DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_URSOC_DOOR_2,           DATA_URSOC,             DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    {               0,                     0,            DOOR_TYPE_ROOM,       BOUNDARY_NONE } ///< End
};

class instance_the_emerald_nightmare : public InstanceMapScript
{
    public:
        instance_the_emerald_nightmare() : InstanceMapScript("instance_the_emerald_nightmare", 1520) { }

        struct instance_the_emerald_nightmare_InstanceMapScript : public InstanceScript
        {
            instance_the_emerald_nightmare_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);

                m_Initialized = false;

                m_DungeonID = 0;
            }

            bool m_Initialized;

            uint32 m_DungeonID;

            std::set<uint64> m_EggGuids;
            uint64 m_NythendraGuid;
            uint64 m_YsondreGuid;
            uint64 m_EmerissGuid;
            uint64 m_LethonGuid;
            uint64 m_AshenvalePortalGuid;
            uint64 m_FeralasPortalGuid;
            uint64 m_HinterlandsPortalGuid;
            uint64 m_EleretheRenferalGuid;
            uint64 m_NightmareSpawnGuid;
            uint64 m_IlgynothGuid;
            uint64 m_CenariusGuid;
            uint64 m_CenariusChestGuid;
            uint64 m_CenariusPortalGuid;
            uint64 m_XaviusGuid;
            uint64 m_RiftOfAlnGuid;
            uint64 m_CosmeticCenariusGuid;
            uint64 m_MalfurionStormrageGuid;

            uint32 m_XaviusPreEventState;

            uint32 m_CenariusActivatePortalTimer;

            void Initialize() override
            {
                LoadDoorData(g_DoorData);

                m_NythendraGuid = 0;
                m_YsondreGuid = 0;
                m_EmerissGuid = 0;
                m_LethonGuid = 0;
                m_AshenvalePortalGuid = 0;
                m_FeralasPortalGuid = 0;
                m_HinterlandsPortalGuid = 0;
                m_EleretheRenferalGuid = 0;
                m_NightmareSpawnGuid = 0;
                m_IlgynothGuid = 0;
                m_CenariusGuid = 0;
                m_CenariusChestGuid = 0;
                m_CenariusPortalGuid = 0;
                m_XaviusGuid = 0;
                m_RiftOfAlnGuid = 0;
                m_CosmeticCenariusGuid = 0;
                m_MalfurionStormrageGuid = 0;

                m_XaviusPreEventState = NOT_STARTED;

                m_CenariusActivatePortalTimer = 5000;

                if (GetBossState(DATA_XAVIUS) != DONE)
                    SetData(DATA_PRE_EVENT_XAVIUS, NOT_STARTED); ///< Reset event
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case NPC_NYTHENDRA:
                    {
                        m_NythendraGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_ELERETHE_RENFERAL:
                    {
                        m_EleretheRenferalGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_ILGYNOTH:
                    {
                        if (!p_Creature->isAlive())
                            SetBossState(eData::DATA_ILGYNOTH, EncounterState::DONE);

                        m_IlgynothGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_YSONDRE:
                    {
                        m_YsondreGuid = p_Creature->GetGUID();
                        /// No break!
                    }
                    case NPC_TAERAR:
                    case NPC_LETHON:
                    case NPC_EMERISS:
                    {
                        if (p_Creature->GetEntry() == NPC_LETHON)
                            m_LethonGuid = p_Creature->GetGUID();
                        else if (p_Creature->GetEntry() == NPC_EMERISS)
                            m_EmerissGuid = p_Creature->GetGUID();

                        AddToDamageManager(p_Creature);
                        break;
                    }
                    case NPC_NIGHTMARE_RIFT:
                    {
                        if (p_Creature->ToTempSummon() != nullptr)
                            break;

                        m_AshenvalePortalGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_SHADOWY_RIFT:
                    {
                        if (p_Creature->ToTempSummon() != nullptr)
                            break;

                        m_FeralasPortalGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_DISEASED_RIFT:
                    {
                        if (p_Creature->ToTempSummon() != nullptr)
                            break;

                        m_HinterlandsPortalGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_SURGING_EGG_SAC:
                    case NPC_PULSING_EGG_SAC:
                    {
                        m_EggGuids.insert(p_Creature->GetGUID());
                        break;
                    }
                    case NPC_CENARIUS:
                    {
                        m_CenariusGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_XAVIUS:
                    {
                        m_XaviusGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_RIFT_OF_ALN:
                    {
                        m_RiftOfAlnGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_COSMETIC_CENARIUS:
                    {
                        m_CosmeticCenariusGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_MALFURION_STORMRAGE:
                    {
                        m_MalfurionStormrageGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_CORRUPTED_FEELER:
                    {
                        p_Creature->AddUnitState(UNIT_STATE_ROOT);
                        break;
                    }
                    case NPC_NIGHTMARE_SPAWN_STALKER:
                    {
                        m_NightmareSpawnGuid = p_Creature->GetGUID();
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
                    case NPC_YSONDRE:
                    case NPC_TAERAR:
                    case NPC_LETHON:
                    case NPC_EMERISS:
                    {
                        RemoveFromDamageManager(p_Creature);
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
                    case GO_NYTHENDRA_DOOR_1:
                    case GO_NYTHENDRA_DOOR_2:
                    case GO_NYTHENDRA_DOOR_3:
                    case GO_NYTHENDRA_DOOR_4:
                    case GO_NYTHENDRA_DOOR_5:
                    case GO_NYTHENDRA_DOOR_6:
                    case GO_RENFERAL_DOOR:
                    case GO_ILGYNOTH_DOOR_1:
                    case GO_ILGYNOTH_DOOR_2:
                    case GO_ILGYNOTH_DOOR_3:
                    case GO_URSOC_DOOR_1:
                    case GO_URSOC_DOOR_2:
                        AddDoor(p_GameObject, true);
                        break;
                    case GO_CENARIUS_CHEST:
                        m_CenariusChestGuid = p_GameObject->GetGUID();
                        break;
                    case GO_CENARIUS_PORTAL:
                        m_CenariusPortalGuid = p_GameObject->GetGUID();
                        p_GameObject->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case GO_NYTHENDRA_DOOR_1:
                    case GO_NYTHENDRA_DOOR_2:
                    case GO_NYTHENDRA_DOOR_3:
                    case GO_NYTHENDRA_DOOR_4:
                    case GO_NYTHENDRA_DOOR_5:
                    case GO_NYTHENDRA_DOOR_6:
                    case GO_RENFERAL_DOOR:
                    case GO_ILGYNOTH_DOOR_1:
                    case GO_ILGYNOTH_DOOR_2:
                    case GO_ILGYNOTH_DOOR_3:
                    case GO_URSOC_DOOR_1:
                    case GO_URSOC_DOOR_2:
                        AddDoor(p_GameObject, false);
                        break;
                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* p_Unit) override
            {
                /// Dragons of Nightmare: Corruption of the Dream
                if (p_Unit->GetMap()->IsHeroicOrMythic())
                {
                    Creature* l_Ysondre = instance->GetCreature(m_YsondreGuid);
                    if (!l_Ysondre)
                        return;

                    Creature* l_Emeriss = instance->GetCreature(m_EmerissGuid);
                    if (!l_Emeriss || !l_Emeriss->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1))
                        return;

                    switch (p_Unit->GetEntry())
                    {
                        case NPC_DREAD_HORROR:
                        {
                            p_Unit->CastSpell(p_Unit, 204008, true, nullptr, nullptr, l_Ysondre->GetGUID());
                            break;
                        }
                        case NPC_ESSENCE_OF_CORRUPTION:
                        case NPC_SPIRIT_SHADE:
                        case NPC_DEFILED_DRUID_SPIRIT:
                        {
                            p_Unit->CastSpell(p_Unit, 204009, true, nullptr, nullptr, l_Ysondre->GetGUID());
                            break;
                        }
                        case NPC_SHADE_OF_TAERAR:
                        {
                            p_Unit->CastSpell(p_Unit, 204010, true, nullptr, nullptr, l_Ysondre->GetGUID());
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            bool SetBossState(uint32 p_Type, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Type, p_State))
                    return false;

                switch (p_Type)
                {
                    case DATA_RENFERAL:
                    {
                        switch (p_State)
                        {
                            case EncounterState::NOT_STARTED:
                            case EncounterState::FAIL:
                            case EncounterState::TO_BE_DECIDED:
                            {
                                for (uint64 l_Guid : m_EggGuids)
                                {
                                    if (Creature* l_Egg = instance->GetCreature(l_Guid))
                                    {
                                        if (l_Egg->isDead())
                                        {
                                            l_Egg->DespawnOrUnsummon();
                                            l_Egg->Respawn();
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
                    case DATA_CENARIUS:
                    {
                        if (p_State == DONE)
                        {
                            if (Creature* l_Cenarius = instance->GetCreature(m_CenariusGuid))
                            {
                                if (!sObjectMgr->IsDisabledEncounter(GetEncounterIDForBoss(l_Cenarius), instance->GetDifficultyID()))
                                {
                                    if (GameObject* l_Chest = instance->GetGameObject(m_CenariusChestGuid))
                                        l_Chest->SetRespawnTime(604800);
                                }
                            }
                        }

                        break;
                    }
                    case DATA_XAVIUS:
                    {
                        if (p_State == DONE)
                        {
                            AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Creature* l_Xavius = instance->GetCreature(m_XaviusGuid))
                                {
                                    l_Xavius->NearTeleportTo({ -2987.14f, 8534.22f, 33.26f, 4.331034f });
                                    l_Xavius->SetDisplayId(XAVIUS_DEFEATED_DISPLAY);

                                    DoNearTeleportPlayers({ -2976.79f, 8558.08f, 33.44f, 4.312709f });
                                }
                            });

                            AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Creature* l_Cenarius = instance->GetCreature(m_CosmeticCenariusGuid))
                                {
                                    Conversation* l_Conversation = new Conversation;
                                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3797, l_Cenarius, nullptr, *l_Cenarius))
                                        delete l_Conversation;
                                }
                            });
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
                    case DATA_PRE_EVENT_XAVIUS:
                    {
                        m_XaviusPreEventState = p_Data;
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case DATA_PRE_EVENT_XAVIUS:
                        return m_XaviusPreEventState;
                    default:
                        break;
                }

                return 0;
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case NPC_NYTHENDRA:
                        return m_NythendraGuid;
                    case NPC_ILGYNOTH:
                        return m_IlgynothGuid;
                    case NPC_YSONDRE:
                        return m_YsondreGuid;
                    case NPC_LETHON:
                        return m_LethonGuid;
                    case NPC_NIGHTMARE_RIFT:
                        return m_AshenvalePortalGuid;
                    case NPC_SHADOWY_RIFT:
                        return m_FeralasPortalGuid;
                    case NPC_DISEASED_RIFT:
                        return m_HinterlandsPortalGuid;
                    case NPC_ELERETHE_RENFERAL:
                        return m_EleretheRenferalGuid;
                    case NPC_XAVIUS:
                        return m_XaviusGuid;
                    case NPC_RIFT_OF_ALN:
                        return m_RiftOfAlnGuid;
                    case NPC_NIGHTMARE_SPAWN_STALKER:
                        return m_NightmareSpawnGuid;
                    default:
                        break;
                }

                return 0;
            }

            void Update(uint32 p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_CenariusActivatePortalTimer)
                {
                    if (m_CenariusActivatePortalTimer <= p_Diff)
                    {
                        m_CenariusActivatePortalTimer = 5000;

                        if (CheckFirstBosses())
                        {
                            if (GameObject* l_Portal = instance->GetGameObject(m_CenariusPortalGuid))
                            {
                                m_CenariusActivatePortalTimer = 0;
                                l_Portal->SendGameObjectActivateAnimKit(3761, true);
                                l_Portal->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            }
                        }
                    }
                    else
                        m_CenariusActivatePortalTimer -= p_Diff;
                }
            }

            bool CheckRequiredBosses(uint32 p_BossID, Player const* p_Player) const override
            {
                if (!InstanceScript::CheckRequiredBosses(p_BossID, p_Player))
                    return false;

                /// Don't need to check bosses for LFR
                if (instance->IsLFR())
                    return true;

                switch (p_BossID)
                {
                    case DATA_RENFERAL:
                    case DATA_ILGYNOTH:
                    case DATA_URSOC:
                    case DATA_DRAGON_NIGHTMARE:
                    case DATA_CENARIUS:
                    {
                        if (GetBossState(DATA_NYTHENDRA) != DONE)
                            return false;

                        break;
                    }
                    case DATA_XAVIUS:
                    {
                        if (GetBossState(DATA_CENARIUS) != DONE)
                            return false;

                        break;
                    }
                    default:
                        break;
                }

                return true;
            }

            bool CheckFirstBosses()
            {
                if (instance->IsLFR() && m_DungeonID == 1288)
                    return true;

                if (GetBossState(DATA_NYTHENDRA) == DONE &&
                    GetBossState(DATA_RENFERAL) == DONE && GetBossState(DATA_ILGYNOTH) == DONE &&
                    GetBossState(DATA_URSOC) == DONE && GetBossState(DATA_DRAGON_NIGHTMARE) == DONE)
                    return true;
                else
                    return false;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = nullptr;

                bool l_MalfurionIntro = false;
                if (Creature* l_Malfurion = instance->GetCreature(m_MalfurionStormrageGuid))
                {
                    if (l_Malfurion->IsAIEnabled)
                        l_MalfurionIntro = l_Malfurion->AI()->GetData();
                }

                switch (m_DungeonID)
                {
                    case 1287: ///< Darkbough
                    {
                        if (GetBossState(DATA_NYTHENDRA) != EncounterState::DONE || !l_MalfurionIntro)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(5533);
                        else
                            l_Loc = sWorldSafeLocsStore.LookupEntry(5681);

                        break;
                    }
                    case 1288: ///< Tormented Guardians
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(5681);
                        break;
                    }
                    case 1289: ///< Rift of Aln
                    {
                        l_Loc = sWorldSafeLocsStore.LookupEntry(5534);
                        break;
                    }
                    default:
                    {
                        /// In the dream if Xavius is dead
                        if (GetBossState(DATA_XAVIUS) == EncounterState::DONE)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(5859);
                        /// In front of the Rift of Aln if Cenarius is alive but previous bosses are dead
                        else if (CheckFirstBosses() && GetBossState(DATA_CENARIUS) != EncounterState::DONE)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(5692);
                        /// Instance entrance if Nythendra is alive, or Malfurion intro is not done
                        else if (GetBossState(DATA_NYTHENDRA) != EncounterState::DONE || !l_MalfurionIntro)
                            l_Loc = sWorldSafeLocsStore.LookupEntry(5533);
                        /// Center of the raid - Near malfurion
                        else
                            l_Loc = sWorldSafeLocsStore.LookupEntry(5681);

                        break;
                    }
                }

                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (GetBossState(DATA_XAVIUS) == IN_PROGRESS)
                    p_Player->CastSpell(p_Player, 189960, true); ///< Alter power

                /// Disable non available bosses for LFR
                if (!m_Initialized)
                {
                    m_Initialized = true;

                    m_DungeonID = p_Player->GetGroup() ? sLFGMgr->GetDungeon(p_Player->GetGroup()->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    std::vector<uint64> m_DisabledGoBs;
                    uint32 l_DisabledMask = 0;

                    switch (m_DungeonID)
                    {
                        case 1287: ///< Darkbough
                        {
                            l_DisabledMask |= (1 << DATA_URSOC);
                            l_DisabledMask |= (1 << DATA_DRAGON_NIGHTMARE);
                            l_DisabledMask |= (1 << DATA_CENARIUS);
                            l_DisabledMask |= (1 << DATA_XAVIUS);

                            m_DisabledGoBs.push_back(m_CenariusChestGuid);
                            m_DisabledGoBs.push_back(m_CenariusPortalGuid);

                            m_CenariusActivatePortalTimer = 0;
                            break;
                        }
                        case 1288: ///< Tormented Guardians
                        {
                            l_DisabledMask |= (1 << DATA_NYTHENDRA);
                            l_DisabledMask |= (1 << DATA_RENFERAL);
                            l_DisabledMask |= (1 << DATA_ILGYNOTH);
                            l_DisabledMask |= (1 << DATA_XAVIUS);
                            break;
                        }
                        case 1289: ///< Rift of Aln
                        {
                            l_DisabledMask |= (1 << DATA_NYTHENDRA);
                            l_DisabledMask |= (1 << DATA_RENFERAL);
                            l_DisabledMask |= (1 << DATA_ILGYNOTH);
                            l_DisabledMask |= (1 << DATA_URSOC);
                            l_DisabledMask |= (1 << DATA_DRAGON_NIGHTMARE);
                            l_DisabledMask |= (1 << DATA_CENARIUS);

                            m_DisabledGoBs.push_back(m_CenariusChestGuid);
                            m_DisabledGoBs.push_back(m_CenariusPortalGuid);

                            m_CenariusActivatePortalTimer = 0;
                            break;
                        }
                        default:
                            break;
                    }

                    SetDisabledBosses(l_DisabledMask);

                    for (uint64 l_Guid : m_DisabledGoBs)
                    {
                        if (GameObject* l_Door = instance->GetGameObject(l_Guid))
                            l_Door->SetGoState(GOState::GO_STATE_READY);
                    }
                }
            }

            std::string GetSaveData() override
            {
                std::ostringstream l_SaveStream;
                l_SaveStream << "E N " << GetBossSaveData();
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

                char l_DataHead1, l_DataHead2;

                std::istringstream l_LoadStream(p_Data);
                l_LoadStream >> l_DataHead1 >> l_DataHead2;

                if (l_DataHead1 == 'E' && l_DataHead2 == 'N')
                {
                    for (uint32 l_I = 0; l_I < eData::MAX_ENCOUNTER; ++l_I)
                    {
                        uint32 l_TmpState;

                        l_LoadStream >> l_TmpState;

                        if (l_TmpState == EncounterState::IN_PROGRESS || l_TmpState > EncounterState::SPECIAL)
                            l_TmpState = EncounterState::NOT_STARTED;

                        SetBossState(l_I, EncounterState(l_TmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_the_emerald_nightmare_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_the_emerald_nightmare()
{
    new instance_the_emerald_nightmare();
}
#endif
