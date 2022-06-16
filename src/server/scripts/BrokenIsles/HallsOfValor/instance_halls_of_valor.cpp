////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "halls_of_valor.hpp"

DoorData const g_DoorData[] =
{
    { GO_HYMDALL_ENTER_DOOR,         DATA_HYMDALL,         DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_HYMDALL_EXIT_DOOR,          DATA_HYMDALL,         DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_HYRJA_DOOR,                 DATA_HYRJA,           DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_GATES_OF_GLORY_DOOR,        DATA_HYRJA,           DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_GATES_OF_GLORY_DOOR,        DATA_FENRYR,          DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_ODYN_BALCONY,               DATA_ODYN,            DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { 0,                             0,                    DOOR_TYPE_PASSAGE,    BOUNDARY_NONE }
};

class instance_halls_of_valor : public InstanceMapScript
{
    public:
        instance_halls_of_valor() : InstanceMapScript("instance_halls_of_valor", 1477) { }

        struct instance_halls_of_valor_InstanceMapScript : public InstanceScript
        {
            instance_halls_of_valor_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            uint64 m_HymdallCacheGuid;
            uint64 m_ValarjarAspirantGuid;
            uint64 m_FenryrGuid;
            uint64 m_SkovaldGuid;
            uint64 m_AegisGuid;
            uint64 m_OdynGuid;
            uint64 m_OdynChestGuid;
            uint64 m_ValhallaBridgeGuid;
            std::map<uint32, uint64> m_SkovaldKingsGuids;

            std::map<uint32, uint64> m_RunicBrandGuids;
            std::vector<uint64> m_PossibleFenryrs;

            uint32 m_FenryrEventDone;
            uint32 m_SkovaldEventDone;

            bool m_TameableFenryr;

            uint64 m_DrakeRider = 0;

            bool m_OdynAchievement = true;

            uint64 m_HyrjaGuid = 0;

            void Initialize() override
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(g_DoorData);

                m_HymdallCacheGuid = 0;
                m_ValarjarAspirantGuid = 0;
                m_FenryrGuid = 0;
                m_SkovaldGuid = 0;
                m_OdynGuid = 0;
                m_AegisGuid = 0;
                m_OdynChestGuid = 0;
                m_ValhallaBridgeGuid = 0;

                m_FenryrEventDone = 0;
                m_SkovaldEventDone = 0;

                /// Can only be done in mythic mode
                if (!instance->IsMythic())
                    m_TameableFenryr = false;
                else
                    m_TameableFenryr = true;
            }

            bool IsValidTargetForAffix(Unit const* p_Target, Affixes const p_Affix) override
            {
                switch (p_Target->GetEntry())
                {
                    case eCreatures::NPC_BOSS_FENRYR:
                    case eCreatures::NPC_ODYN:
                    case eCreatures::NPC_HYRJA:
                    case eCreatures::NPC_GOD_KING_SKOVALD:
                    case eCreatures::NPC_OLMYR_THE_ENLIGHTENED:
                    case eCreatures::NPC_SOLSTEN:
                    {
                        return false;
                    }

                    case eCreatures::NPC_HALL_OF_VALOR_TELEPORTER:
                    case eCreatures::NPC_FIELD_OF_THE_ETERNAL_HUNT:
                    case eCreatures::NPC_FENRYR:
                    case eCreatures::NPC_STORMFORGED_OBLITERATOR:
                    {
                        return false;
                    }

                    default:
                        break;
                }

                return true;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case NPC_HYRJA:
                    {
                        m_HyrjaGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_BOSS_FENRYR:
                    {
                        m_FenryrGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_FENRYR:
                    {
                        m_PossibleFenryrs.push_back(p_Creature->GetGUID());

                        if (m_PossibleFenryrs.size() > 1)
                        {
                            auto l_Seed = std::chrono::system_clock::now().time_since_epoch().count();
                            std::shuffle(m_PossibleFenryrs.begin(), m_PossibleFenryrs.end(), std::default_random_engine(l_Seed));

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Creature* l_Fenryr = instance->GetCreature(m_PossibleFenryrs[0]))
                                    l_Fenryr->DespawnOrUnsummon();
                            });

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Creature* l_Fenryr = instance->GetCreature(m_PossibleFenryrs[1]))
                                {
                                    if (l_Fenryr->IsAIEnabled)
                                        l_Fenryr->AI()->DoAction(0);
                                }
                            });
                        }

                        break;
                    }
                    case NPC_GOD_KING_SKOVALD:
                    {
                        m_SkovaldGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_AEGIS_OF_AGGRAMAR:
                    {
                        m_AegisGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_ODYN:
                    {
                        m_OdynGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_VALARJAR_ASPIRANT:
                    {
                        m_ValarjarAspirantGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_FIELD_OF_THE_ETERNAL_HUNT:
                    case NPC_HALL_OF_VALOR_TELEPORTER:
                    {
                        p_Creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        p_Creature->SetFullHealth();
                        break;
                    }
                    case NPC_KING_HALDOR:
                    case NPC_KING_BJORN:
                    case NPC_KING_RANULF:
                    case NPC_KING_TOR:
                    {
                        m_SkovaldKingsGuids[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_STORM_DRAKE_ACHIEVEMENT:
                    {
                        p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                        p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                        if (Creature* l_Rider = p_Creature->SummonCreature(NPC_VALARJAR_STORMRIDER, p_Creature->GetPosition()))
                        {
                            uint64 l_DrakeGuid = p_Creature->GetGUID();
                            m_DrakeRider = l_Rider->GetGUID();

                            AddTimedDelayedOperation(1, [this, l_DrakeGuid]() -> void
                            {
                                if (Creature* l_Rider = instance->GetCreature(m_DrakeRider))
                                {
                                    if (Creature* l_Drake = instance->GetCreature(l_DrakeGuid))
                                        l_Rider->EnterVehicle(l_Drake);
                                }
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
            {
                InstanceScript::CreatureDiesForScript(p_Creature, p_Killer);

                switch (p_Creature->GetEntry())
                {
                    case NPC_STORM_DRAKE_ACHIEVEMENT:
                    {
                        if (Creature* l_Rider = instance->GetCreature(m_DrakeRider))
                            l_Rider->DespawnOrUnsummon(1);

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
                    case NPC_KING_HALDOR:
                    case NPC_KING_BJORN:
                    case NPC_KING_RANULF:
                    case NPC_KING_TOR:
                    {
                        if (m_SkovaldKingsGuids.find(p_Creature->GetEntry()) != m_SkovaldKingsGuids.end())
                            m_SkovaldKingsGuids.erase(p_Creature->GetEntry());

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
                    case GO_HYMDALL_ENTER_DOOR:
                    case GO_HYMDALL_EXIT_DOOR:
                    case GO_HYRJA_DOOR:
                    case GO_GATES_OF_GLORY_DOOR:
                    case GO_ODYN_BALCONY:
                        AddDoor(p_GameObject, true);
                        break;
                    case GO_ODYN_CHEST:
                        m_OdynChestGuid = p_GameObject->GetGUID();
                        break;
                    case GO_RUNIC_BRAND_PURE:
                        m_GoRunicColour[0] = p_GameObject->GetEntry();
                        m_RunicBrandGuids[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    case GO_RUNIC_BRAND_RED:
                        m_GoRunicColour[1] = p_GameObject->GetEntry();
                        m_RunicBrandGuids[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    case GO_RUNIC_BRAND_YELLOW:
                        m_GoRunicColour[2] = p_GameObject->GetEntry();
                        m_RunicBrandGuids[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    case GO_RUNIC_BRAND_BLUE:
                        m_GoRunicColour[3] = p_GameObject->GetEntry();
                        m_RunicBrandGuids[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    case GO_RUNIC_BRAND_GREEN:
                        m_GoRunicColour[4] = p_GameObject->GetEntry();
                        m_RunicBrandGuids[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        break;
                    case GO_HYMDALL_CACHE:
                        m_HymdallCacheGuid = p_GameObject->GetGUID();
                        break;
                    case GO_VALHALLA_BRIDGE:
                        m_ValhallaBridgeGuid = p_GameObject->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case GO_HYMDALL_ENTER_DOOR:
                    case GO_HYMDALL_EXIT_DOOR:
                    case GO_HYRJA_DOOR:
                    case GO_GATES_OF_GLORY_DOOR:
                    case GO_ODYN_BALCONY:
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
                    case DATA_HYMDALL:
                    {
                        if (p_State == IN_PROGRESS && m_TameableFenryr)
                        {
                            /// You must solo Hymdall and Fenryr on Mythic Difficulty!
                            if (instance->GetPlayersCountExceptGMs() > 1)
                                m_TameableFenryr = false;
                        }

                        break;
                    }
                    case DATA_HYRJA:
                    case DATA_FENRYR:
                    {
                        if (CountDefeatedEncounters() >= 3)
                        {
                            DoCastSpellOnPlayers(SPELL_ODYNS_BLESSING);

                            HandleGameObject(m_ValhallaBridgeGuid, true, nullptr);

                            Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                            for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                    l_Player->CastSpell(l_Player, SPELL_THREE_BOSS_DEFEAT, true);
                            }
                        }

                        if (p_Type == DATA_FENRYR)
                        {
                            if (p_State == IN_PROGRESS && m_TameableFenryr)
                            {
                                /// You must solo Hymdall and Fenryr on Mythic Difficulty!
                                if (instance->GetPlayersCountExceptGMs() > 1)
                                    m_TameableFenryr = false;
                            }
                            else if (p_State == DONE && m_TameableFenryr)
                            {

                            }
                        }

                        break;
                    }
                    case DATA_SKOVALD:
                    {
                        if (p_State != IN_PROGRESS)
                        {
                            if (Creature* l_Aegis = instance->GetCreature(m_AegisGuid))
                                l_Aegis->DespawnOrUnsummon();
                        }

                        if (p_State == DONE)
                        {
                            AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Creature* l_Odyn = instance->GetCreature(m_OdynGuid))
                                {
                                    if (l_Odyn->IsAIEnabled)
                                        l_Odyn->AI()->DoAction(true);
                                }
                            });
                        }

                        break;
                    }
                    case DATA_ODYN:
                    {
                        if (p_State != IN_PROGRESS)
                        {
                            for (uint8 l_I = 0; l_I < 5; ++l_I)
                            {
                                if (GameObject* rune = instance->GetGameObject(m_RunicBrandGuids[m_GoRunicColour[l_I]]))
                                    rune->SetGoState(GO_STATE_READY);
                            }
                        }

                        if (p_State == DONE)
                        {
                            if (!instance->IsChallengeMode())
                            {
                                if (GameObject* l_Chest = instance->GetGameObject(m_OdynChestGuid))
                                    l_Chest->SetRespawnTime(86400);
                            }

                            instance->SummonCreature(NPC_SPOILS_CHEST_VISUAL, g_OdynsSpoilPos);

                            if (m_OdynAchievement && instance->IsMythic())
                                DoCompleteAchievement(SURGE_PROTECTOR);
                        }
                        else if (p_State == FAIL)
                            m_OdynAchievement = true;

                        break;
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
                    case DATA_FENRYR_EVENT:
                    {
                        m_FenryrEventDone = p_Data;

                        if (p_Data == DONE)
                        {
                            if (Creature* l_Fenryr = instance->GetCreature(m_FenryrGuid))
                            {
                                l_Fenryr->SetVisible(true);
                                l_Fenryr->SetReactState(REACT_AGGRESSIVE);

                                if (l_Fenryr->IsAIEnabled)
                                    l_Fenryr->AI()->DoAction(0);
                            }

                            SaveToDB();
                        }

                        break;
                    }
                    case DATA_SKOVALD_EVENT:
                    {
                        m_SkovaldEventDone = p_Data;

                        if (p_Data == DONE)
                            SaveToDB();

                        break;
                    }
                    case DATA_RUNES_ACTIVATED:
                    {
                        if (GameObject* l_Rune = instance->GetGameObject(m_RunicBrandGuids[m_GoRunicColour[p_Data]]))
                            l_Rune->SetGoState(GO_STATE_ACTIVE);

                        break;
                    }
                    case DATA_RUNES_DEACTIVATED:
                    {
                        if (GameObject* l_Rune = instance->GetGameObject(m_RunicBrandGuids[m_GoRunicColour[p_Data]]))
                            l_Rune->SetGoState(GO_STATE_READY);

                        if (Creature* l_Odyn = instance->GetCreature(m_OdynGuid))
                        {
                            if (l_Odyn->IsAIEnabled)
                                l_Odyn->AI()->SetData(1, p_Data);
                        }

                        break;
                    }
                    case DATA_SURGE_PROTECTOR:
                    {
                        m_OdynAchievement = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_SKOVALD:
                        return m_SkovaldGuid;
                    case DATA_ODYN:
                        return m_OdynGuid;
                    case GO_HYMDALL_CACHE:
                        return m_HymdallCacheGuid;
                    case NPC_VALARJAR_ASPIRANT:
                        return m_ValarjarAspirantGuid;
                    case GO_VALHALLA_BRIDGE:
                        return m_ValhallaBridgeGuid;
                    case NPC_HYRJA:
                        return m_HyrjaGuid;
                    case NPC_KING_HALDOR:
                    case NPC_KING_BJORN:
                    case NPC_KING_RANULF:
                    case NPC_KING_TOR:
                    {
                        if (m_SkovaldKingsGuids.find(p_Type) != m_SkovaldKingsGuids.end())
                            return m_SkovaldKingsGuids[p_Type];

                        break;
                    }
                    default:
                        break;
                }

                return 0;
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case DATA_FENRYR_EVENT:
                        return m_FenryrEventDone;
                    case DATA_SKOVALD_EVENT:
                        return m_SkovaldEventDone;
                    default:
                        break;
                }

                return 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                /// Has to be done in solo
                if (m_TameableFenryr && instance->GetPlayersCountExceptGMs() > 1)
                    m_TameableFenryr = false;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream l_SaveStream;
                l_SaveStream << "H V " << GetBossSaveData() << m_FenryrEventDone << " " << m_SkovaldEventDone << " ";

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

                char l_DataHead1, l_DataHead2;

                std::istringstream l_LoadStream(p_In);

                l_LoadStream >> l_DataHead1 >> l_DataHead2;

                if (l_DataHead1 == 'H' && l_DataHead2 == 'V')
                {
                    for (uint8 l_I = 0; l_I < MAX_ENCOUNTER; ++l_I)
                    {
                        uint32 l_TmpState;

                        l_LoadStream >> l_TmpState;

                        if (l_TmpState == IN_PROGRESS || l_TmpState > SPECIAL)
                            l_TmpState = NOT_STARTED;

                        SetBossState(l_I, EncounterState(l_TmpState));
                    }

                    l_LoadStream >> m_FenryrEventDone;
                    l_LoadStream >> m_SkovaldEventDone;
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            uint32 m_GoRunicColour[5];
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_halls_of_valor_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_halls_of_valor()
{
    new instance_halls_of_valor();
}
#endif
