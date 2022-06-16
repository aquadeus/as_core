////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "eye_of_azshara.hpp"

class instance_eye_of_azshara : public InstanceMapScript
{
    public:
        instance_eye_of_azshara() : InstanceMapScript("instance_eye_of_azshara", 1456) { }

        struct instance_eye_of_azshara_InstanceMapScript : public InstanceScript
        {
            instance_eye_of_azshara_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map)
            {
                SetBossNumber(MAX_ENCOUNTER);

                m_AzsharaBubbleGuid = 0;

                m_EyeOfAzsharaTimer = 1 * TimeConstants::IN_MILLISECONDS;
            }

            uint64 m_AzsharaGuid;
            uint64 m_AzsharaBubbleGuid;
            uint64 m_NagasGuids[4];

            uint8 m_NagasCoun;

            bool m_CryOfWrath;
            uint32 m_WindsTimer;

            bool m_OnInitEnterState;

            uint32 m_PlayerCount;
            bool m_StartEvent;
            uint32 m_StormTime;
            uint32 m_ShelterCheckTimer;

            uint32 m_EyeOfAzsharaTimer;

            uint64 m_LadyHatecoil = 0;

            bool m_WrathIntroDone = false;

            void Initialize() override
            {
                m_AzsharaGuid = 0;

                for (uint8 l_I = 0; l_I < 4; l_I++)
                    m_NagasGuids[l_I] = 0;

                m_NagasCoun = 0;
                m_CryOfWrath = false;
                m_WindsTimer = urand(1, 90) * IN_MILLISECONDS;
                m_StormTime = 4000;
                m_ShelterCheckTimer = 1000;
                m_OnInitEnterState = false;

                m_PlayerCount = 0;
                m_StartEvent = false;
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

                switch (p_Target->GetEntry())
                {
                    case eCreatures::NPC_BINDER_ASHIOI:
                    case eCreatures::NPC_MYSTIC_SSAVEH:
                    case eCreatures::NPC_CHANNELER_VARISZ:
                    case eCreatures::NPC_RITUALIST_LESHA:
                    case eCreatures::NPC_WEATHERMAN:
                    case eCreatures::NPC_BLAZING_HYDRA_SPAWN:
                    case eCreatures::NPC_ARCANE_HYDRA_SPAWN:
                    case eCreatures::NPC_ARCANE_BOMB:
                    case eCreatures::NPC_SALTSEA_GLOBULE:
                        return false;

                    default: return true;
                }

                return true;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case NPC_WRATH_OF_AZSHARA:
                    {
                        m_AzsharaGuid = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_MYSTIC_SSAVEH:
                    case NPC_RITUALIST_LESHA:
                    case NPC_CHANNELER_VARISZ:
                    case NPC_BINDER_ASHIOI:
                    {
                        m_NagasGuids[m_NagasCoun++] = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_WEATHERMAN:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        break;
                    }
                    case NPC_LADY_HATECOIL:
                    {
                        m_LadyHatecoil = p_Creature->GetGUID();
                        break;
                    }
                    case NPC_SKROG_WAVECRASHER:
                    {
                        p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_INTERRUPT_CAST, true);
                        p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                        p_Creature->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
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
                    case GO_BUBBLE:
                    {
                        m_AzsharaBubbleGuid = p_GameObject->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 p_Type, EncounterState p_State) override
            {
                if (!InstanceScript::SetBossState(p_Type, p_State))
                {
                    /// Mostly in case of loading
                    DoEventCreatures();
                    return false;
                }

                switch (p_Type)
                {
                    case DATA_PARJESH:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        SendWeatherToPlayers(WeatherState::WEATHER_STATE_RAIN_DROP, 0.3f);
                        break;
                    }
                    case DATA_HATECOIL:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        SendLightOverride(7152, 7153, 0);
                        break;
                    }
                    case DATA_SERPENTRIX:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        SendWeatherToPlayers(WeatherState::WEATHER_STATE_LIGHT_RAIN, 0.5f);
                        SendLightOverride(7152, 7154, 0);
                        break;
                    }
                    case DATA_DEEPBEARD:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        SendWeatherToPlayers(WeatherState::WEATHER_STATE_RAIN_DROP, 0.3f);
                        SendWeatherToPlayers(WeatherState::WEATHER_STATE_LIGHT_RAIN, 0.5f);
                        SendLightOverride(7152, 7155, 0);
                        break;
                    }
                    case DATA_WRATH_OF_AZSHARA:
                    {
                        if (p_State != EncounterState::DONE)
                            break;

                        DoRemoveAurasDueToSpellOnPlayers(eEoASpells::SpellShelter);
                        DoRemoveAurasDueToSpellOnPlayers(eEoASpells::SpellEyeOfAzshara);
                        DoRemoveAurasDueToSpellOnPlayers(eEoASpells::SpellViolentWinds);

                        SendWeatherToPlayers(WeatherState::WEATHER_STATE_FOG, 0.0f);
                        SendLightOverride(7152, 0, 0);
                        break;
                    }
                    default:
                        break;
                }

                DoEventCreatures();
                return true;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                m_CryOfWrath = p_Value != 0;
            }

            bool CheckBossDone()
            {
                for (uint8 l_I = 0; l_I < DATA_WRATH_OF_AZSHARA; l_I++)
                {
                    if (GetBossState(l_I) != DONE)
                        return false;
                }

                return true;
            }

            void DoEventCreatures()
            {
                if (m_WrathIntroDone || !CheckBossDone())
                    return;

                m_WrathIntroDone = true;

                for (uint8 l_I = 0; l_I < 4; l_I++)
                {
                    if (Creature* l_Naga = instance->GetCreature(m_NagasGuids[l_I]))
                    {
                        if (l_Naga->IsAIEnabled)
                            l_Naga->AI()->DoAction(true);
                    }
                }

                if (Creature* l_Boss = instance->GetCreature(m_AzsharaGuid))
                {
                    if (GameObject* l_Bubble = instance->GetGameObject(m_AzsharaBubbleGuid))
                        l_Bubble->Delete();

                    l_Boss->setFaction(FACTION_MONSTER_2);
                    l_Boss->SetVisible(true);

                    if (l_Boss->IsAIEnabled)
                        l_Boss->AI()->ZoneTalk(0);
                }
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                if (!m_StartEvent)
                {
                    if (m_PlayerCount < 5)
                        m_PlayerCount++;
                    else
                    {
                        m_StartEvent = true;
                        if (GroupPtr l_Group = p_Player->GetGroup())
                        {
                            if (Player* l_Leader = ObjectAccessor::FindPlayer(l_Group->GetLeaderGUID()))
                            {
                                if (Creature* l_Target = l_Leader->FindNearestCreature(100216, 50.0f, true))
                                {
                                    if (l_Target->IsAIEnabled)
                                        l_Target->AI()->Talk(0);
                                }
                            }
                        }
                    }
                }

                if (m_OnInitEnterState)
                    return;

                m_OnInitEnterState = true;

                DoEventCreatures();
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case NPC_LADY_HATECOIL:
                        return m_LadyHatecoil;
                    default:
                        break;
                }

                return 0;
            }

            void Update(uint32 p_Diff)  override
            {
                /// Challenge
                InstanceScript::Update(p_Diff);

                /// Don't process instance events after last boss death
                if (GetBossState(eData::DATA_WRATH_OF_AZSHARA) == EncounterState::DONE)
                    return;

                if (m_ShelterCheckTimer <= p_Diff)
                {
                    Map::PlayerList const& l_Players = instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->IsOutdoors())
                                l_Player->RemoveAura(eEoASpells::SpellShelter);
                            else
                                l_Player->CastSpell(l_Player, eEoASpells::SpellShelter, true);
                        }
                    }

                    m_ShelterCheckTimer = 1000;
                }
                else
                    m_ShelterCheckTimer -= p_Diff;

                if (m_EyeOfAzsharaTimer <= p_Diff)
                {
                    Map::PlayerList const& l_Players = instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->isGameMaster() || l_Player->GetAreaId() != 8040)
                            {
                                l_Player->RemoveAura(eEoASpells::SpellEyeOfAzshara);
                                continue;
                            }

                            /// Eye of Azshara dummy for Tempest Attunement damages
                            l_Player->CastSpell(l_Player, eEoASpells::SpellEyeOfAzshara, true);

                            m_EyeOfAzsharaTimer = 1 * TimeConstants::IN_MILLISECONDS;
                        }
                    }
                }
                else
                    m_EyeOfAzsharaTimer -= p_Diff;

                /// Violent winds begins after two bosses killed
                if (CountDefeatedEncounters() < 2)
                    return;

                if (m_WindsTimer <= p_Diff)
                {
                    Map::PlayerList const& l_Players = instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            if (l_Player->isGameMaster() || m_CryOfWrath)
                                continue;

                            /// Sheltered from the storm
                            if (l_Player->HasAura(eEoASpells::SpellShelter))
                                continue;

                            l_Player->CastSpell(l_Player, eEoASpells::SpellViolentWinds, true); ///< Violent Winds
                        }
                    }

                    m_WindsTimer = 90 * IN_MILLISECONDS;
                }
                else
                    m_WindsTimer -= p_Diff;

                /// Lightning Strikes begins after the third boss death
                if (CountDefeatedEncounters() < 3)
                    return;

                if (m_StormTime <= p_Diff)
                {
                    Map::PlayerList const& l_Players = instance->GetPlayers();

                    std::list<Player*> l_Targets;

                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                            l_Targets.push_back(l_Player);
                    }

                    if (!l_Targets.empty())
                    {
                        std::list<Player*>::const_iterator l_Iter = l_Targets.begin();
                        std::advance(l_Iter, urand(0, l_Targets.size() - 1));

                        if ((*l_Iter)->GetAreaId() == 8040 || (*l_Iter)->GetAreaId() == 8084) ///< Only needed area
                            (*l_Iter)->CastSpell((*l_Iter)->GetPosition(), eEoASpells::SpellLightningStrikes, true);
                    }

                    m_StormTime = 4000;
                }
                else
                    m_StormTime -= p_Diff;
            }

            void SendLightOverride(uint32 p_LightEntry, uint32 p_OverrideID, uint32 p_FadeInTime, Player* p_Target = nullptr) const
            {
                WorldPacket l_Data(Opcodes::SMSG_OVERRIDE_LIGHT, 12);
                l_Data << uint32(p_LightEntry);
                l_Data << uint32(p_OverrideID);
                l_Data << uint32(p_FadeInTime);

                if (p_Target)
                    p_Target->GetSession()->SendPacket(&l_Data);
                else
                    instance->SendToPlayers(&l_Data);
            }

            void SendWeather(WeatherState p_Weather, float p_Intensity, Player* p_Target = nullptr, bool p_Abrupt = false) const
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
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_eye_of_azshara_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_eye_of_azshara()
{
    new instance_eye_of_azshara();
}
#endif
