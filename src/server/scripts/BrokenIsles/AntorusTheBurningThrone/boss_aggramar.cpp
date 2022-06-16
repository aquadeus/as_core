////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"

static Position k_RoomCenter = { -12634.08f, -2254.26f, 2514.27f, 4.78f };

static std::array<Position, 10> k_EmberSpawns =
{{
    { -12616.18f, -2207.783f, 2514.406f, 1.204f },
    { -12647.04f, -2206.166f, 2514.406f, 1.832f },
    { -12672.96f, -2222.996f, 2514.406f, 2.461f },
    { -12684.03f, -2251.845f, 2514.406f, 3.089f },
    { -12676.03f, -2281.694f, 2514.406f, 3.718f },
    { -12652.02f, -2301.141f, 2514.406f, 4.346f },
    { -12621.16f, -2302.758f, 2514.406f, 4.974f },
    { -12595.24f, -2285.928f, 2514.406f, 5.603f },
    { -12584.17f, -2257.079f, 2514.406f, 6.231f },
    { -12592.17f, -2227.230f, 2514.406f, 0.576f }
}};

static std::array<Position, 2> k_FlameSpawns =
{{
    { -12647.40f, -2248.681f, 2514.406f, 2.731519f },
    { -12620.86f, -2248.517f, 2514.406f, 0.421921f }
}};

enum eScriptData
{
    MoveCenter = 99,
    MeteorSwarmBig = 1,
    DontSweatTheTechniqueWorldState = 13704
};

/// Last Update 7.3.5 - Build 26365
/// Aggramar - 121975
class boss_aggramar : public CreatureScript
{
    public:
        boss_aggramar() : CreatureScript("boss_aggramar") { }

        struct boss_aggramarAI : BossAI
        {
            boss_aggramarAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataAggramar)
            {

            }

            enum eSpells
            {
                FoeBreaker              = 245458,
                FlameRend               = 245463,
                SearingTempest          = 245301,
                FlamesOff               = 243429,
                FlamesOn                = 245912,

                WakeOfFlame             = 244693,
                ScorchingBlaze          = 245994,
                Flare                   = 245983,
                EmpoweredFlare          = 246037,

                CorruptAegis            = 244894,
                SummonEmber             = 245056,
                MeteorSwarmPeriodic     = 244678,
                WakeOfFlameAreatrigger  = 244718,

                RavenousBlaze           = 254452,
                EmpoweredFlameRend      = 255058,
                EmpoweredSearingTempest = 255061,
                EmpoweredFoeBreaker     = 255059,

                Berserk                 = 26662,
            };

            enum eEvents
            {
                EventEnergize = 1,
                EventWakeOfFlame,
                EventScorchingBlaze,
                EventFlare,
                EventEmpoweredFlare,
                EventIntermissionWakeOfFlame
            };

            enum eTalks
            {
                TalkAggro,
                TalkWakeOfFlame,
                TalkTaeshalachTechnique,
                TalkIntermission1,
                TalkPhase2,
                TalkFlare,
                TalkIntermission2,
                TalkPhase3,
                TalkEmpoweredFlare,
                TalkKillPlayer,
                TalkWipe,
                TalkDeath
            };

            bool m_TaeshalachTechnique;
            uint8 m_Intermission;
            uint8 m_flameCounter;
            std::vector<std::list<uint64>> m_ScorchingBlazeGroups;
            uint32 m_ScorchingBlazeCastIndex;
            std::set<uint64> m_SummonsGUID;
            EventMap m_IntermissionEvent;
            bool m_IntermissionCanEnd;
            int32 m_CheckPlayersTimer;
            int32 m_MeteorSwarmIndex;
            int32 m_BerserkTimer;
            int32 m_transitionStartTimer;
            bool m_Energizing;

            void Reset() override
            {
                _Reset();

                me->SetDisableGravity(false);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
                me->SetPower(Powers::POWER_ENERGY, 0);
                if (IsMythic())
                    me->SetPower(Powers::POWER_ENERGY, 60);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                m_TaeshalachTechnique = false;
                me->RemoveAura(eSpells::FlamesOn);
                me->CastSpell(me, eSpells::FlamesOff, true);
                m_Intermission = 0;
                m_flameCounter = 0;
                m_transitionStartTimer = 0;
                m_IntermissionCanEnd = true;

                m_ScorchingBlazeCastIndex = 0;
                m_ScorchingBlazeGroups.clear();

                m_SummonsGUID.clear();
                m_IntermissionEvent.Reset();

                if (instance)
                    instance->DoUpdateWorldState(eScriptData::DontSweatTheTechniqueWorldState, 0);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->instance->ForEachPlayer([](Player* p_Itr) -> void
                    {
                        if (p_Itr->isDead())
                            p_Itr->NearTeleportTo(k_RoomCenter);
                    });
                }
            }

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();
                ClearDelayedOperations();
                m_CheckPlayersTimer = 1 * TimeConstants::IN_MILLISECONDS;
                m_MeteorSwarmIndex = 0;
                DefaultEvents(false, true);
                Talk(eTalks::TalkAggro);
                m_BerserkTimer = 9 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);
                _JustDied();
            }

            void Energize()
            {
                if (m_Energizing)
                    me->ModifyPower(Powers::POWER_ENERGY, 1);

                AddTimedDelayedOperation(350, [this]()-> void
                {
                    Energize();
                });
            }

            void DefaultEvents(bool m_IntermissionJustEnded, bool m_Engage = false)
            {
                AddTimedDelayedOperation(350, [this]()-> void
                {
                    Energize();
                });

                events.Reset();
                me->SetPower(Powers::POWER_ENERGY, 0);
                if (IsMythic() && m_Engage)
                    me->SetPower(Powers::POWER_ENERGY, 60);

                m_Energizing = true;

                if (m_IntermissionJustEnded || m_Engage)
                {
                    events.ScheduleEvent(eEvents::EventScorchingBlaze, m_Engage ? 4700 : (IsMythic() ? 21000 : 6000));
                    switch (m_Intermission)
                    {
                        case 0:
                            events.ScheduleEvent(eEvents::EventWakeOfFlame, IsMythic() ? 10500 : 5500);
                            break;
                        case 2:
                            events.ScheduleEvent(eEvents::EventFlare, IsMythic() ? 8400 : 10500);
                            break;
                        case 4:
                            events.ScheduleEvent(eEvents::EventEmpoweredFlare, IsMythic() ? 8400 : 10000);
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    events.ScheduleEvent(eEvents::EventScorchingBlaze, (IsMythic() ? 21000 : 6000));
                    switch (m_Intermission)
                    {
                        case 0:
                            events.ScheduleEvent(eEvents::EventWakeOfFlame, 5000);
                            break;
                        case 2:
                            events.ScheduleEvent(eEvents::EventFlare, IsMythic() ? 6600 : 9000);
                            break;
                        case 4:
                            events.ScheduleEvent(eEvents::EventEmpoweredFlare, IsMythic() ? 7700 : 9000);
                            break;
                        default:
                            break;
                    }
                }
            }

            uint32 GetData(uint32 p_ID /* = 0 */) override
            {
                switch (p_ID)
                {
                    case eScriptData::MeteorSwarmBig:
                    {
                        return !(++m_MeteorSwarmIndex % 3);
                    }
                    default:
                        break;
                }

                return 0;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::WakeOfFlame:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void PrepareScorchingBlazeTargets()
            {
                uint32 l_TargetAmount = std::ceil(((static_cast<float>(me->GetMap()->GetPlayersCountExceptGMs()) * 3.0f / 5.0f) - 1.0f) / 2.0f);
                uint32 l_I = 0;

                std::vector<Player*> l_CasterList;
                for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->isGameMaster())
                            continue;

                        if (l_Player->IsActiveSpecTankSpec() || l_Player->IsMeleeDamageDealer())
                            continue;

                        l_CasterList.push_back(l_Player);
                    }
                }

                auto l_Seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::shuffle(l_CasterList.begin(), l_CasterList.end(), std::default_random_engine(l_Seed));

                if (m_ScorchingBlazeGroups.empty())
                {
                    m_ScorchingBlazeGroups.push_back({});
                    m_ScorchingBlazeGroups.push_back({});
                }

                for (Player* l_Player : l_CasterList)
                {
                    if (l_I < l_TargetAmount)
                        m_ScorchingBlazeGroups[0].push_back(l_Player->GetGUID());
                    else if (l_I < 2 * l_TargetAmount)
                        m_ScorchingBlazeGroups[0].push_back(l_Player->GetGUID());

                    l_I++;
                }
            }

            void StartIntermission()
            {
                ClearDelayedOperations();
                m_Energizing = false;
                m_TaeshalachTechnique = false;
                m_IntermissionCanEnd = false;

                me->RemoveAura(eSpells::FlamesOn);
                me->CastSpell(me, eSpells::FlamesOff, true);

                me->CastStop();
                me->AttackStop();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(eScriptData::MoveCenter, k_RoomCenter);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                Talk(m_Intermission == 1 ? eTalks::TalkIntermission1 : eTalks::TalkIntermission2);

                if ((me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic
                    || me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                    && m_Intermission == 3)
                    m_IntermissionEvent.ScheduleEvent(eEvents::EventIntermissionWakeOfFlame, 3 * IN_MILLISECONDS);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (p_Summon->GetEntry() == eCreatures::NpcFlameOfTaeshalach)
                {
                    if (--m_flameCounter == 0)
                    {
                        EndIntermission();
                    }
                }
                p_Summon->DespawnOrUnsummon(1);
            }

            void SummonedCreatureDespawn(Creature* p_Summon) override
            {
                //m_SummonsGUID.erase(p_Summon->GetGUID());
                //if (m_SummonsGUID.empty() && m_Intermission % 2)
                //    EndIntermission();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (me->isInCombat() && (!summon->IsAIEnabled || !summon->AI()->IsPassived()))
                    DoZoneInCombat(summon);

                if (summon->GetEntry() == eCreatures::NpcFlameOfTaeshalach)
                    ++m_flameCounter;
            }

            void EndIntermission()
            {
                me->RemoveAura(eSpells::CorruptAegis);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                me->RemoveAura(eSpells::MeteorSwarmPeriodic);
                m_Intermission++;
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                m_IntermissionEvent.Reset();
                Talk(m_Intermission == 2 ? eTalks::TalkPhase2 : eTalks::TalkPhase3);
                DefaultEvents(true);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(me->getVictim());
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (!p_Victim->IsPlayer())
                    return;

                if (me->GetInstanceScript() && me->GetInstanceScript()->IsWipe())
                {
                    Talk(eTalks::TalkWipe);
                    return;
                }

                if (urand(0, 5))
                    return;

                Talk(eTalks::TalkKillPlayer);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eScriptData::MoveCenter:
                    {
                        me->CastSpell(me, eSpells::CorruptAegis, false);
                        me->SetFacingTo(4.653450f);
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        });
                        if (m_Intermission > 2 || IsHeroicOrMythic())
                            me->CastSpell(me, eSpells::MeteorSwarmPeriodic, true);

                        std::vector<Position> l_EmberSpawns;
                        if (IsMythic())
                        {
                            for (Position const& l_Pos : k_EmberSpawns)
                                l_EmberSpawns.push_back(l_Pos);
                        }
                        else if (IsHeroic())
                        {
                            for (uint32 l_I = 0; l_I < 8; ++l_I)
                            {
                                Position l_Spawn;
                                float l_Angle = (3.0f * M_PI / 2.0f) + (l_I * 2.0f * M_PI / 9.0f);
                                k_RoomCenter.SimplePosXYRelocationByAngle(l_Spawn, 47.0f, l_Angle, true);
                                l_Spawn.m_orientation = l_Angle + M_PI;
                                l_EmberSpawns.push_back(l_Spawn);
                            }
                        }
                        else
                        {
                            for (uint32 l_I = 0; l_I < 6; ++l_I)
                            {
                                Position l_Spawn;
                                float l_Angle = (3.0f * M_PI / 2.0f) + (l_I * 2.0f * M_PI / 7.0f);
                                k_RoomCenter.SimplePosXYRelocationByAngle(l_Spawn, 47.0f, l_Angle, true);
                                l_Spawn.m_orientation = l_Angle + M_PI;
                                l_EmberSpawns.push_back(l_Spawn);
                            }
                        }

                        for (uint32 l_I = 0; l_I < l_EmberSpawns.size(); l_I++)
                        {
                            Position l_Spawn = l_EmberSpawns[l_I];
                            AddTimedDelayedOperation(l_I * 100, [this, l_Spawn]() -> void
                            {
                                me->CastSpell(l_Spawn, eSpells::SummonEmber, true);
                            });
                        }

                        for (uint32 l_I = 0; l_I < k_FlameSpawns.size(); l_I++)
                        {
                            Position l_Spawn = k_FlameSpawns[l_I];
                            AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this, l_Spawn]() -> void
                            {
                                if (Creature* l_Summon = me->SummonCreature(eCreatures::NpcFlameOfTaeshalach, l_Spawn))
                                    m_SummonsGUID.insert(l_Summon->GetGUID());
                            });
                        }

                        m_IntermissionCanEnd = true;

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                auto GetPct = [=]()
                {
                    switch (m_Intermission)
                    {
                        case 0:
                        {
                            if (me->GetHealthPct() < (IsLFR() ? 60.0f : 80.0f))
                                return true;
                        }
                        case 2:
                        {
                            float l_HealthPct = IsMythic() ? 35.0f : 40.0f;
                            if (!IsLFR() && me->HealthBelowPct(l_HealthPct))
                                return true;
                        }
                    }
                    return false;
                };

                if (GetPct() && !m_transitionStartTimer)
                {
                    m_Intermission++;
                    m_transitionStartTimer = 500;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                events.Update(p_Diff);

                if (!UpdateVictim())
                    return;

                if (m_transitionStartTimer > 0)
                {
                    if (m_transitionStartTimer <= p_Diff)
                    {
                        m_transitionStartTimer = 0;
                        StartIntermission();
                    }
                    else
                        m_transitionStartTimer -= p_Diff;
                }

                m_BerserkTimer -= p_Diff;
                if (m_BerserkTimer < 0)
                    me->CastSpell(me, eSpells::Berserk, true);

                if (m_CheckPlayersTimer < 0)
                {
                    instance->instance->ForEachPlayer([this](Player* p_Itr)
                    {
                        if (!p_Itr->isGameMaster() && p_Itr->GetPositionZ() < 2512.0f)
                            me->Kill(p_Itr);
                    });

                    m_CheckPlayersTimer = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_CheckPlayersTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || (m_Intermission == 1))
                    return;

                if (m_Intermission != 3 && me->getVictim())
                {
                    if (me->GetPositionZ() - me->getVictim()->GetPositionZ() > 2.0f)
                    {
                        bool l_Found = false;
                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                        if (l_ThreatList.empty())
                            return;

                        l_ThreatList.sort(JadeCore::ThreatOrderPred());
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (me->GetPositionZ() - l_Ref->getTarget()->GetPositionZ() > 2.0f)
                                continue;

                            AttackStart(l_Ref->getTarget());
                            l_Found = true;
                            break;
                        }

                        if (!l_Found)
                            EnterEvadeMode();

                        return;
                    }
                }

                if (m_Intermission == 3)
                {
                    m_IntermissionEvent.Update(p_Diff);
                    if (auto eventId = m_IntermissionEvent.ExecuteEvent())
                    { 
                        switch (eventId)
                        {
                            case eEvents::EventIntermissionWakeOfFlame:
                            {
                                Position l_Dest;
                                float l_Angle = frand(0.0f, 2.0f * M_PI);
                                me->SimplePosXYRelocationByAngle(l_Dest, 6.0f, l_Angle);
                                l_Dest.m_orientation = l_Angle + me->GetOrientation();
                                me->CastSpell(l_Dest, eSpells::WakeOfFlameAreatrigger, true);

                                m_IntermissionEvent.ScheduleEvent(eEvents::EventIntermissionWakeOfFlame, 3 * IN_MILLISECONDS);
                            }
                            default:
                                break;
                        }
                    }
                    return;
                }

                if (m_TaeshalachTechnique)
                    return;

                if (me->GetPower(Powers::POWER_ENERGY) >= 100)
                {
                    ClearDelayedOperations();
                    Talk(eTalks::TalkTaeshalachTechnique);
                    m_TaeshalachTechnique = true;
                    m_Energizing = false;

                    me->RemoveAura(eSpells::FlamesOff);
                    me->CastSpell(me, eSpells::FlamesOn, true);

                    events.Reset();
                    m_ScorchingBlazeCastIndex = 0;

                    me->SetPower(Powers::POWER_ENERGY, 0);

                    std::array<uint32, 5> l_TaeshalachTechniqueSpells;
                    l_TaeshalachTechniqueSpells.fill(0);

                    if (IsMythic())
                    {
                        std::array<std::vector<uint32>, 4> m_PatternOrders =
                        {{
                            {eSpells::EmpoweredFlameRend, eSpells::SearingTempest, eSpells::FoeBreaker, eSpells::FoeBreaker, eSpells::EmpoweredFlameRend },
                            {eSpells::EmpoweredFlameRend, eSpells::FoeBreaker, eSpells::FoeBreaker, eSpells::SearingTempest, eSpells::EmpoweredFlameRend },
                            {eSpells::FoeBreaker, eSpells::SearingTempest, eSpells::EmpoweredFlameRend, eSpells::FoeBreaker, eSpells::EmpoweredFlameRend },
                            {eSpells::FoeBreaker, eSpells::EmpoweredFlameRend, eSpells::SearingTempest, eSpells::FoeBreaker, eSpells::EmpoweredFlameRend }
                        }};

                        std::vector<uint32> l_Pattern = m_PatternOrders[urand(0, m_PatternOrders.size() - 1)];
                        uint32 l_I = 0;
                        for (uint32 l_SpellId : l_Pattern)
                        {
                            if (m_Intermission >= 2 && l_SpellId == eSpells::SearingTempest)
                                l_TaeshalachTechniqueSpells[l_I] = eSpells::EmpoweredSearingTempest;
                            else if (m_Intermission >= 4 && l_SpellId == eSpells::FoeBreaker)
                                l_TaeshalachTechniqueSpells[l_I] = eSpells::EmpoweredFoeBreaker;
                            else
                                l_TaeshalachTechniqueSpells[l_I] = l_SpellId;

                            l_I++;
                        }
                    }
                    else
                    {
                        if (!IsLFR())
                        {
                            l_TaeshalachTechniqueSpells[0] = eSpells::FoeBreaker;
                            l_TaeshalachTechniqueSpells[2] = eSpells::FoeBreaker;
                        }
                        l_TaeshalachTechniqueSpells[1] = eSpells::FlameRend;
                        l_TaeshalachTechniqueSpells[3] = eSpells::FlameRend;
                        l_TaeshalachTechniqueSpells[4] = eSpells::SearingTempest;
                    }

                    uint32 l_Delay = 0;
                    for (uint32 l_I = 0; l_I < 5; ++l_I)
                    {
                        uint32 l_SpellId = l_TaeshalachTechniqueSpells[l_I];
                        AddTimedDelayedOperation(l_Delay, [this, l_SpellId]() -> void
                        {
                            if (l_SpellId != eSpells::SearingTempest && l_SpellId != eSpells::EmpoweredSearingTempest)
                                me->SetFacingToObject(me->getVictim());

                            me->CastSpell(me->getVictim(), l_SpellId, false);
                        });

                        if (l_SpellId == eSpells::SearingTempest || l_SpellId == eSpells::EmpoweredSearingTempest)
                            l_Delay += 7000;
                        else
                            l_Delay += 4 * IN_MILLISECONDS;
                    }

                    AddTimedDelayedOperation(22 * IN_MILLISECONDS, [this]() -> void
                    {
                        DefaultEvents(false);
                        m_TaeshalachTechnique = false;
                        me->RemoveAura(eSpells::FlamesOn);
                        me->CastSpell(me, eSpells::FlamesOff, true);
                        m_Energizing = true;
                    });
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventWakeOfFlame:
                    {
                        Talk(eTalks::TalkWakeOfFlame);
                        Unit* l_Target = SelectPlayerTarget(UnitAI::eTargetTypeMask::TypeMaskNonTank);
                        if (!l_Target)
                            break;

                        me->SetFacingToObject(l_Target);
                        me->NearTeleportTo(*me);
                        AddTimedDelayedOperation(50, [this]() -> void
                        {
                            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });

                        me->CastSpell(l_Target, eSpells::WakeOfFlame, false);

                        events.ScheduleEvent(eEvents::EventWakeOfFlame, IsMythic() ? 12100 : 24000);
                        break;
                    }
                    case eEvents::EventScorchingBlaze:
                    {
                        if (!IsMythic())
                        {
                            if (!m_ScorchingBlazeCastIndex)
                                PrepareScorchingBlazeTargets();

                            for (uint64 l_PlayerGUID : m_ScorchingBlazeGroups[m_ScorchingBlazeCastIndex % 2])
                            {
                                Player* l_Player = sObjectAccessor->GetPlayer(*me, l_PlayerGUID);
                                if (!l_Player)
                                    return;

                                me->CastSpell(l_Player, eSpells::ScorchingBlaze, true);
                            }

                            m_ScorchingBlazeCastIndex++;

                            events.ScheduleEvent(eEvents::EventScorchingBlaze, 7300);
                        }
                        else
                        {
                            std::vector<Player*> l_CasterList;
                            std::vector<Player*> l_MeleeList;
                            for (Map::PlayerList::const_iterator l_Iter = instance->instance->GetPlayers().begin(); l_Iter != instance->instance->GetPlayers().end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                {
                                    if (l_Player->isGameMaster())
                                        continue;

                                    if (l_Player->IsActiveSpecTankSpec())
                                        continue;

                                    if (l_Player->IsMeleeDamageDealer())
                                    {
                                        l_MeleeList.push_back(l_Player);
                                        continue;
                                    }

                                    l_CasterList.push_back(l_Player);
                                }
                            }

                            auto l_Seed = std::chrono::system_clock::now().time_since_epoch().count();
                            std::shuffle(l_CasterList.begin(), l_CasterList.end(), std::default_random_engine(l_Seed));
                            uint32 l_I = 0;
                            while (l_I < l_MeleeList.size() && l_CasterList.size() < 5)
                                l_CasterList.push_back(l_MeleeList[l_I++]);

                            for (uint32 l_J = 0; l_J < 5 && l_J < l_CasterList.size(); ++l_J)
                                me->CastSpell(l_CasterList[l_J], eSpells::RavenousBlaze, true);

                            events.ScheduleEvent(eEvents::EventScorchingBlaze, 21 * IN_MILLISECONDS);
                        }
                        break;
                    }
                    case eEvents::EventFlare:
                    {
                        Talk(eTalks::TalkFlare);
                        me->CastSpell(me, eSpells::Flare, false);
                        if (!IsMythic())
                            events.ScheduleEvent(eEvents::EventFlare, 15800);
                        break;
                    }
                    case eEvents::EventEmpoweredFlare:
                    {
                        Talk(eTalks::TalkEmpoweredFlare);
                        me->CastSpell(me, eSpells::EmpoweredFlare, false);
                        if (!IsMythic())
                            events.ScheduleEvent(eEvents::EventEmpoweredFlare, 16200);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_aggramarAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Foe Breaker - 245458 - Empowered Foe Breaker - 255059
class spell_aggramar_foe_breaker : public SpellScriptLoader
{
    public:
        spell_aggramar_foe_breaker() : SpellScriptLoader("spell_aggramar_foe_breaker") { }

        class spell_aggramar_foe_breaker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_foe_breaker_SpellScript);

            enum eSpells
            {
                FoeBreakerDamage    = 244291,
                EmpoweredFoeBreaker = 255059,
                SearingBinding      = 255528,
                FoeBreakerTracker   = 254655
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                float l_ConeAngle = M_PI / 1.64f;
                float l_Radius = 25.0f;

                std::list<WorldObject*> l_Targets;
                JadeCore::WorldObjectSpellConeTargetCheck l_Check(l_ConeAngle, l_Radius, l_Caster, GetSpellInfo(), TARGET_CHECK_DEFAULT, nullptr);
                JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellConeTargetCheck> l_Searcher(l_Caster, l_Targets, l_Check, GRID_MAP_TYPE_MASK_CREATURE);
                Spell::SearchTargets<JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellConeTargetCheck> >(l_Searcher, GRID_MAP_TYPE_MASK_CREATURE, l_Caster, l_Caster, l_Radius);

                for (WorldObject* l_Target : l_Targets)
                {
                    Creature* l_Creature = l_Target->ToCreature();
                    if (!l_Creature || l_Creature->GetEntry() != eCreatures::NpcEmberOfTaeshalach)
                        continue;

                    l_Creature->AddAura(eSpells::FoeBreakerTracker, l_Creature);
                }

                Unit* l_Target = l_Caster->getVictim();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FoeBreakerDamage, true);

                if (m_scriptSpellId == eSpells::EmpoweredFoeBreaker)
                    l_Caster->CastSpell(l_Target, eSpells::SearingBinding, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_aggramar_foe_breaker_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_foe_breaker_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Flame Rend - 245463
class spell_aggramar_flame_rend : public SpellScriptLoader
{
    public:
        spell_aggramar_flame_rend() : SpellScriptLoader("spell_aggramar_flame_rend") { }

        class spell_aggramar_flame_rend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_flame_rend_SpellScript);

            enum eSpells
            {
                FlameRendDamage = 244033,
                FlameRendKnockback = 244208,
                FlameRendTracker = 254654
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = l_Caster->getVictim();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FlameRendDamage, true);
                l_Caster->CastSpell(l_Target, eSpells::FlameRendKnockback, true);

                float l_ConeAngle = M_PI / 1.64f;
                float l_Radius = 25.0f;

                std::list<WorldObject*> l_Targets;
                JadeCore::WorldObjectSpellConeTargetCheck l_Check(l_ConeAngle, l_Radius, l_Caster, GetSpellInfo(), TARGET_CHECK_DEFAULT, nullptr);
                JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellConeTargetCheck> l_Searcher(l_Caster, l_Targets, l_Check, GRID_MAP_TYPE_MASK_CREATURE);
                Spell::SearchTargets<JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellConeTargetCheck> >(l_Searcher, GRID_MAP_TYPE_MASK_CREATURE, l_Caster, l_Caster, l_Radius);

                for (WorldObject* l_Target : l_Targets)
                {
                    Creature* l_Creature = l_Target->ToCreature();
                    if (!l_Creature || l_Creature->GetEntry() != eCreatures::NpcEmberOfTaeshalach)
                        continue;

                    l_Creature->AddAura(eSpells::FlameRendTracker, l_Creature);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_aggramar_flame_rend_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_flame_rend_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Empowered Flame Rend - 255058
class spell_aggramar_empowered_flame_rend_cast : public SpellScriptLoader
{
    public:
        spell_aggramar_empowered_flame_rend_cast() : SpellScriptLoader("spell_aggramar_empowered_flame_rend_cast") { }

        class spell_aggramar_empowered_flame_rend_cast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_empowered_flame_rend_cast_SpellScript);

            enum eSpells
            {
                FlameRend = 247079
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = l_Caster->getVictim();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FlameRend, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_aggramar_empowered_flame_rend_cast_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_empowered_flame_rend_cast_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Searing Tempest - 245301
class spell_aggramar_searing_tempest : public SpellScriptLoader
{
    public:
        spell_aggramar_searing_tempest() : SpellScriptLoader("spell_aggramar_searing_tempest") { }

        class spell_aggramar_searing_tempest_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_searing_tempest_SpellScript);

            enum eSpells
            {
                SearingTempestDamage    = 246014,
                SearingTempestTracker   = 254653
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SearingTempestDamage, true);

                float l_Radius = 35.0f;
                std::list<Creature*> l_Embers;
                l_Caster->GetCreatureListWithEntryInGrid(l_Embers, eCreatures::NpcEmberOfTaeshalach, l_Radius);
                for (Creature* l_Creature : l_Embers)
                    l_Creature->AddAura(eSpells::SearingTempestTracker, l_Creature);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_searing_tempest_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_searing_tempest_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Empowered Searing Tempest - 255061
class spell_aggramar_empowered_searing_tempest : public SpellScriptLoader
{
    public:
        spell_aggramar_empowered_searing_tempest() : SpellScriptLoader("spell_aggramar_empowered_searing_tempest") { }

        class spell_aggramar_empowered_searing_tempest_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_empowered_searing_tempest_SpellScript);

            enum eSpells
            {
                EmpoweredSearingTempestDamage       = 255062,
                EmpoweredSearingTempestAreatrigger  = 255063,
                Catalyzed                           = 247091
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::EmpoweredSearingTempestAreatrigger, true);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->IsPlayer())
                    l_Caster->CastSpell(l_Target, eSpells::EmpoweredSearingTempestDamage, true);

                if (!l_Caster->GetMap() || !l_Caster->GetMap()->IsMythic())
                    return;

                if (l_Target->ToCreature() && l_Target->ToCreature()->GetEntry() == eCreatures::NpcEmberOfTaeshalach)
                    l_Target->CastSpell(l_Target, eSpells::Catalyzed, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_aggramar_empowered_searing_tempest_SpellScript::HandleOnPrepare);
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_empowered_searing_tempest_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_empowered_searing_tempest_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Taeshalach's Reach - 243431
class spell_aggramar_taeshalachs_reach : public SpellScriptLoader
{
    public:
        spell_aggramar_taeshalachs_reach() : SpellScriptLoader("spell_aggramar_taeshalachs_reach") { }

        class spell_aggramar_taeshalachs_reach_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggramar_taeshalachs_reach_AuraScript);

            enum eSpells
            {
                TaeshalachsReach        = 244675,
                TaeshalachsReachDebuff  = 245990
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::TaeshalachsReach, true);
                l_Caster->CastSpell(l_Target, eSpells::TaeshalachsReachDebuff, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_aggramar_taeshalachs_reach_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aggramar_taeshalachs_reach_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Taeshalach's Reach - 244675
class spell_aggramar_taeshalachs_reach_damage : public SpellScriptLoader
{
    public:
        spell_aggramar_taeshalachs_reach_damage() : SpellScriptLoader("spell_aggramar_taeshalachs_reach_damage") { }

        class spell_aggramar_taeshalachs_reach_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_taeshalachs_reach_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() <= 1)
                    return;

                Unit* l_MainTarget = GetExplTargetUnit();
                if (!l_MainTarget)
                    return;

                auto l_Itr = std::find(p_Targets.begin(), p_Targets.end(), l_MainTarget);
                if (l_Itr == p_Targets.end())
                    return;

                p_Targets.erase(l_Itr);
                p_Targets.sort(JadeCore::DistanceOrderPred(l_MainTarget));

                WorldObject* l_FinalTarget = p_Targets.front();
                p_Targets.clear();
                p_Targets.push_back(l_FinalTarget);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggramar_taeshalachs_reach_damage_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_taeshalachs_reach_damage_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Wake Of Flame - 244693
class spell_aggramar_wake_of_flame : public SpellScriptLoader
{
    public:
        spell_aggramar_wake_of_flame() : SpellScriptLoader("spell_aggramar_wake_of_flame") { }

        class spell_aggramar_wake_of_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_wake_of_flame_SpellScript);

            enum eSpells
            {
                WakeOfFlameAreatrigger = 244718
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_Count = l_Caster->GetMap()->IsHeroicOrMythic() ? 5 : 3;

                for (uint32 l_I = 0; l_I < l_Count; ++l_I)
                {
                    Position l_Dest;
                    float l_Angle = -M_PI / 3.0f + (l_I * M_PI / (l_Caster->GetMap()->IsHeroicOrMythic() ? 6.0f : 3.0f));
                    l_Caster->SimplePosXYRelocationByAngle(l_Dest, 6.0f, l_Angle);
                    l_Dest.m_orientation = l_Angle + l_Caster->GetOrientation();
                    l_Caster->CastSpell(l_Dest, eSpells::WakeOfFlameAreatrigger, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_aggramar_wake_of_flame_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_wake_of_flame_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Ember Of Taeshalach - 122532
class npc_aggramar_ember_of_taeshalach : public CreatureScript
{
    public:
        npc_aggramar_ember_of_taeshalach() : CreatureScript("npc_aggramar_ember_of_taeshalach") { }

        struct npc_aggramar_ember_of_taeshalachAI : public ScriptedAI
        {
            npc_aggramar_ember_of_taeshalachAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_SummonerGUID = 0;
                m_RepathTimer = 500;

                if (IsMythic())
                    p_Creature->SetFlagExtra(CREATURE_FLAG_EXTRA_ALL_DIMINISH);
            }

            enum eSpells
            {
                BlazingEruption = 244912,
                MoltenRemnants  = 245905
            };

            int32 m_RepathTimer;
            uint64 m_SummonerGUID;
            Position m_CurrentDest;
            Position m_SpawnPosition;
            bool m_Exploded;
            bool m_NeedRepath;

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (m_Exploded)
                    return;

                if (!IsHeroicOrMythic())
                    return;

                if (p_Damage < me->GetHealth())
                    return;

                p_Damage = 0;
                if (me->HasAura(eSpells::MoltenRemnants))
                    return;

                me->SetHealth(me->GetMaxHealth());
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                AddTimedDelayedOperation(19 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                    Creature* l_Summoner = sObjectAccessor->GetCreature(*me, m_SummonerGUID);
                    if (!l_Summoner)
                        return;

                    me->GetMotionMaster()->MovePoint(0, *l_Summoner);
                });

                me->CastSpell(me, eSpells::MoltenRemnants, true);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SpawnPosition = *me;
                m_SummonerGUID = p_Summoner->GetGUID();
                m_NeedRepath = false;
                m_Exploded = false;
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->GetMotionMaster()->MoveFollow(p_Summoner, 0.0f, 0.0f);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
                me->SetPower(Powers::POWER_ENERGY, 0);
            }

            void OnAddAura(Aura* p_Aura, bool /*p_OnReapply*/) override
            {
                if (!IsMythic())
                    return;

                if (GetDiminishingReturnsGroupForSpell(p_Aura->GetSpellInfo(), p_Aura->GetCaster()) == DiminishingGroup::DIMINISHING_INCAPACITATE)
                {
                    uint32 l_Duration = std::min<uint32>(p_Aura->GetDuration(), 7 * TimeConstants::IN_MILLISECONDS);
                    p_Aura->SetMaxDuration(l_Duration);
                    p_Aura->SetDuration(l_Duration);
                }
            }

            void _ExplodeAndDissapear()
            {
                m_Exploded = true;
                me->CastSpell(me, eSpells::BlazingEruption, true);
                me->GetMotionMaster()->Clear();
                me->SetVisible(false);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
            }

            void SetPower(Powers p_PowerType, int32 p_Value) override
            {
                if (p_PowerType == POWER_ENERGY && p_Value >= 100 && !m_Exploded)
                    _ExplodeAndDissapear();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Exploded)
                    return;

                UpdateOperations(p_Diff);
                m_RepathTimer -= p_Diff;

                if (me->GetPositionZ() < (m_SpawnPosition.m_positionZ - 2.0f))
                {
                    me->NearTeleportTo(m_SpawnPosition);
                    m_NeedRepath = true;
                }

                Creature* l_Summoner = sObjectAccessor->GetCreature(*me, m_SummonerGUID);
                if (!l_Summoner)
                {
                    me->DespawnOrUnsummon(0);
                    return;
                }

                if (m_RepathTimer < 0)
                {
                    m_RepathTimer = 500;
                    if (((m_NeedRepath || l_Summoner->GetExactDist2d(&m_CurrentDest) > 2.0f)
                        || me->HasUnitState(UnitState::UNIT_STATE_CHASE)
                        || !me->IsMoving()) && !me->HasUnitState(UnitState::UNIT_STATE_NOT_MOVE | UnitState::UNIT_STATE_CONTROLLED))
                    {
                        me->GetMotionMaster()->Clear(false);
                        me->ClearUnitState(UnitState::UNIT_STATE_CHASE);

                        m_CurrentDest = *l_Summoner;
                        me->GetMotionMaster()->MovePoint(0, *l_Summoner);
                        m_NeedRepath = false;
                    }
                }

                if (me->GetExactDist2d(l_Summoner) > 11.0f)
                    return;

                _ExplodeAndDissapear();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_aggramar_ember_of_taeshalachAI(p_Creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Flame Of Taeshalach - 121985
class npc_aggramar_flame_of_taeshalach : public CreatureScript
{
    public:
        npc_aggramar_flame_of_taeshalach() : CreatureScript("npc_aggramar_flame_of_taeshalach") { }

        struct npc_aggramar_flame_of_taeshalachAI: public ScriptedAI
        {
            npc_aggramar_flame_of_taeshalachAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                BlazingEruption     = 244912,
                CatalyzingPresence  = 244903
            };

            std::set<uint64> m_AffectedList;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetDisableGravity(false);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                std::list<Creature*> l_CreaturesInRange;
                me->GetCreatureListWithEntryInGrid(l_CreaturesInRange, eCreatures::NpcFlameOfTaeshalach, 15.0f);
                me->GetCreatureListWithEntryInGrid(l_CreaturesInRange, eCreatures::NpcEmberOfTaeshalach, 15.0f);

                std::set<uint64> l_CurrentAffectList;
                for (Creature* l_Creature : l_CreaturesInRange)
                {
                    l_CurrentAffectList.insert(l_Creature->GetGUID());
                    if (m_AffectedList.count(l_Creature->GetGUID()))
                        continue;

                    if (l_Creature->GetGUID() == me->GetGUID())
                        continue;

                    l_Creature->CastSpell(l_Creature, eSpells::CatalyzingPresence, true);
                    m_AffectedList.insert(l_Creature->GetGUID());
                }

                std::vector<uint64> removeList;
                for (uint64 l_CreatureGUID : m_AffectedList)
                {
                    Creature* l_Creature = sObjectAccessor->GetCreature(*me, l_CreatureGUID);
                    if (!l_Creature)
                        continue;

                    if (l_CurrentAffectList.count(l_CreatureGUID))
                    {
                        if (!l_Creature->HasAura(eSpells::CatalyzingPresence))
                            l_Creature->CastSpell(l_Creature, eSpells::CatalyzingPresence, true);

                        continue;
                    }

                    l_Creature->RemoveAura(eSpells::CatalyzingPresence);
                    removeList.push_back(l_CreatureGUID);
                }

                for (auto& itr : removeList)
                    m_AffectedList.erase(itr);

                if (!UpdateVictim())
                    return;

                if (std::fabs(me->GetPositionZ() - me->getVictim()->GetPositionZ()) > 2.0f)
                {
                    bool l_Found = false;
                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                    if (l_ThreatList.empty())
                        return;

                    l_ThreatList.sort(JadeCore::ThreatOrderPred());
                    for (HostileReference* l_Ref : l_ThreatList)
                    {
                        if (std::fabs(me->GetPositionZ() - me->getVictim()->GetPositionZ()) > 2.0f)
                            continue;

                        AttackStart(l_Ref->getTarget());
                        l_Found = true;
                        break;
                    }

                    if (!l_Found)
                    {
                        if (Player* l_PlayerTarget = me->FindNearestPlayer(100.0f))
                            AttackStart(l_PlayerTarget);
                        else
                            EnterEvadeMode();
                    }

                    return;
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                for (uint64 l_CreatureGUID : m_AffectedList)
                {
                    Creature* l_Creature = sObjectAccessor->GetCreature(*me, l_CreatureGUID);
                    if (!l_Creature)
                        continue;

                    l_Creature->RemoveAura(eSpells::CatalyzingPresence);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_aggramar_flame_of_taeshalachAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Flare - 245983
class spell_aggramar_flare_searcher : public SpellScriptLoader
{
    public:
        spell_aggramar_flare_searcher() : SpellScriptLoader("spell_aggramar_flare_searcher") { }

        class spell_aggramar_flare_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_flare_searcher_SpellScript);

            enum eSpells
            {
                FlareMissileSmall   = 245924,
                FlareMissileMedium  = 245926,
                FlareMissileLarge   = 245927
            };

            uint32 m_Delay = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                uint32 l_TargetCount = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (p_Targets.size() <= l_TargetCount)
                    return;

                for (auto l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    if (p_Targets.size() <= l_TargetCount)
                        break;

                    Player* l_Player = (*l_Itr)->ToPlayer();
                    if (!l_Player || !l_Player->IsRangedDamageDealer(true))
                    {
                        l_Itr = p_Targets.erase(l_Itr);
                        continue;
                    }

                    ++l_Itr;
                }

                JadeCore::RandomResizeList(p_Targets, l_TargetCount);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->DelayedCastSpell(l_Target, eSpells::FlareMissileSmall, true, m_Delay += 1500);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggramar_flare_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_flare_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_flare_searcher_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Flare (Missiles) - 244935 - 244937- 244938
class spell_aggramar_flare_missile : public SpellScriptLoader
{
    public:
        spell_aggramar_flare_missile() : SpellScriptLoader("spell_aggramar_flare_missile") { }

        class spell_aggramar_flare_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_flare_missile_SpellScript);

            enum eSpells
            {
                FlareDamage = 245391,
                Catalyzed   = 247091
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Target->ToPlayer())
                    l_Caster->CastSpell(l_Target, eSpells::FlareDamage, true);

                if (!l_Caster->GetMap() || !l_Caster->GetMap()->IsMythic())
                    return;

                if (l_Target->GetEntry() == eCreatures::NpcEmberOfTaeshalach)
                    l_Target->CastSpell(l_Target, eSpells::Catalyzed, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_flare_missile_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_flare_missile_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Meteor Swarm (Periodic) - 244678
class spell_aggramar_meteor_swarm_periodic : public SpellScriptLoader
{
    public:
        spell_aggramar_meteor_swarm_periodic() : SpellScriptLoader("spell_aggramar_meteor_swarm_periodic") { }

        class spell_aggramar_meteor_swarm_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggramar_meteor_swarm_periodic_AuraScript);

            enum eSpells
            {
                MeteorSwarmSearcher = 244680
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MeteorSwarmSearcher, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_aggramar_meteor_swarm_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aggramar_meteor_swarm_periodic_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Meteor Swarm (Searcher) - 244680
class spell_aggramar_meteor_swarm_searcher : public SpellScriptLoader
{
    public:
        spell_aggramar_meteor_swarm_searcher() : SpellScriptLoader("spell_aggramar_meteor_swarm_searcher") { }

        class spell_aggramar_meteor_swarm_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_meteor_swarm_searcher_SpellScript);

            enum eSpells
            {
                MeteorSwarmMissile      = 245920,
                MeteorSwarmMissileSmall = 245767
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                for (auto l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    if (p_Targets.size() <= 1)
                        break;

                    Player* l_Player = (*l_Itr)->ToPlayer();
                    if (!l_Player || !l_Player->IsRangedDamageDealer(true))
                    {
                        l_Itr = p_Targets.erase(l_Itr);
                        continue;
                    }

                    ++l_Itr;
                }
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster || !l_Caster->IsAIEnabled)
                    return;

                Position l_Dest = *l_Target;
                l_Dest.m_positionZ += 0.5f;

                if (l_Caster->GetAI()->GetData(eScriptData::MeteorSwarmBig))
                    l_Caster->CastSpell(l_Dest, eSpells::MeteorSwarmMissile, true);
                else
                    l_Caster->CastSpell(l_Dest, eSpells::MeteorSwarmMissileSmall, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggramar_meteor_swarm_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_meteor_swarm_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_meteor_swarm_searcher_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Empowered Flare - 246037
class spell_aggramar_empowered_flare_searcher : public SpellScriptLoader
{
    public:
        spell_aggramar_empowered_flare_searcher() : SpellScriptLoader("spell_aggramar_empowered_flare_searcher") { }

        class spell_aggramar_empowered_flare_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_empowered_flare_searcher_SpellScript);

            enum eSpells
            {
                EmpoweredFlareMissileSmall   = 246051,
                EmpoweredFlareMissileMedium  = 246052,
                EmpoweredFlareMissileLarge   = 246053
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                uint32 l_TargetCount = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (p_Targets.size() <= l_TargetCount)
                    return;

                for (auto l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    if (p_Targets.size() <= l_TargetCount)
                        break;

                    Player* l_Player = (*l_Itr)->ToPlayer();
                    if (!l_Player || !l_Player->IsRangedDamageDealer(true))
                    {
                        l_Itr = p_Targets.erase(l_Itr);
                        continue;
                    }

                    ++l_Itr;
                }

                JadeCore::RandomResizeList(p_Targets, l_TargetCount);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EmpoweredFlareMissileSmall, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggramar_empowered_flare_searcher_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_empowered_flare_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_empowered_flare_searcher_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Empowered Flare (Missiles) - 246054- 246055 - 246056
class spell_aggramar_empowered_flare_missile : public SpellScriptLoader
{
    public:
        spell_aggramar_empowered_flare_missile() : SpellScriptLoader("spell_aggramar_empowered_flare_missile") { }

        class spell_aggramar_empowered_flare_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_empowered_flare_missile_SpellScript);

            enum eSpells
            {
                EmpoweredFlareDamage    = 245392,
                WakeOfFlameAreatrigger  = 244718,
                Catalyzed               = 247091,
                SummonEmber             = 245056
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Target->ToPlayer())
                    l_Caster->CastSpell(l_Target, eSpells::EmpoweredFlareDamage, true);

                if (!l_Caster->GetMap() || !l_Caster->GetMap()->IsMythic())
                    return;

                if (l_Target->GetEntry() == eCreatures::NpcEmberOfTaeshalach)
                    l_Target->CastSpell(l_Target, eSpells::Catalyzed, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_SpellDest = GetExplTargetDest();
                if (!l_Caster || !l_SpellDest)
                    return;

                float l_OriginalAngle = frand(0.0f, 2.0f * M_PI);
                for (uint32 l_I = 0; l_I < 3; ++l_I)
                {
                    Position l_Dest;
                    float l_Angle = l_OriginalAngle + (l_I * 2.0f * M_PI / 3.0f);
                    l_SpellDest->SimplePosXYRelocationByAngle(l_Dest, 4.0f, l_Angle);
                    l_Dest.m_orientation = l_Angle + l_SpellDest->m_orientation;
                    l_Caster->CastSpell(l_Dest, eSpells::WakeOfFlameAreatrigger, true);
                }

                if (l_Caster->GetMap()->IsMythic())
                    l_Caster->CastSpell(l_SpellDest, eSpells::SummonEmber, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_empowered_flare_missile_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                AfterCast += SpellCastFn(spell_aggramar_empowered_flare_missile_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_empowered_flare_missile_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Flame Rend - 244208
class spell_aggramar_flame_rend_knockback : public SpellScriptLoader
{
    public:
        spell_aggramar_flame_rend_knockback() : SpellScriptLoader("spell_aggramar_flame_rend_knockback") { }

        class spell_aggramar_flame_rend_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_flame_rend_knockback_SpellScript);

            enum eSpells
            {
                BurningRage = 244713
            };

            uint32 m_Size = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                m_Size = p_Targets.size();
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_Size >= 4)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::BurningRage, true);
            }

            void HandleKnockBack(SpellEffIndex /*p_EffIndex*/)
            {
                if (!m_Size)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_HitUnit = GetHitUnit())
                    {
                        if (int32 l_HitDamage = GetSpell()->GetDamage())
                        {
                            l_HitDamage = std::max<int32>(float(l_HitDamage) / float(m_Size), 55);

                            /// Split knock back force between affected players
                            GetSpell()->SetDamage(l_HitDamage);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_aggramar_flame_rend_knockback_SpellScript::HandleKnockBack, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggramar_flame_rend_knockback_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
                AfterCast += SpellCastFn(spell_aggramar_flame_rend_knockback_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_flame_rend_knockback_SpellScript();
        }
};

/// Wrought in Flame - 245911
class spell_aggramar_wrought_in_flame : public SpellScriptLoader
{
    public:
        spell_aggramar_wrought_in_flame() : SpellScriptLoader("spell_aggramar_wrought_in_flame") { }


        class spell_aggramar_wrought_in_flame_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggramar_wrought_in_flame_AuraScript);

            float m_PowerRate;
            uint8 _counter;

            bool Load() override
            {
                m_PowerRate = 0;
                _counter = 0;

                SpellInfo const* l_Spell = GetSpellInfo();
                int32 l_Timer = l_Spell->Effects[EFFECT_0].BasePoints;

                if (l_Timer == 0)
                    return true;

                switch (l_Spell->DifficultyID)
                {
                    case Difficulty::DifficultyRaidLFR:
                    case Difficulty::DifficultyRaidHeroic:
                    case Difficulty::DifficultyRaidNormal:
                    case Difficulty::DifficultyRaidMythic:
                    {
                        m_PowerRate = 100.0f / float(l_Timer);
                        break;
                    }

                    default:
                        break;
                }

                return true;
            }

            void HandleAfterEffPeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner || !m_PowerRate)
                    return;

                l_Owner->SetPower(Powers::POWER_ENERGY, static_cast<int32>(std::floor(m_PowerRate * _counter + 0.5f)));
                _counter++;
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_aggramar_wrought_in_flame_AuraScript::HandleAfterEffPeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aggramar_wrought_in_flame_AuraScript();
        }
};

/// Magni Bronzebeard - 128169
class npc_aggramar_magni_bronzebeard : public CreatureScript
{
    public:
        npc_aggramar_magni_bronzebeard() : CreatureScript("npc_aggramar_magni_bronzebeard") { }

        enum eSpells
        {
            PantheonIntroCinematic = 254311
        };

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Player->CastSpell(p_Player, eSpells::PantheonIntroCinematic, true);
            return true;
        }
};

class PlayerScript_aggramar_outro_cinematic : public PlayerScript
{
    public:
        PlayerScript_aggramar_outro_cinematic() : PlayerScript("PlayerScript_aggramar_outro_cinematic") {}

        void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceId) override
        {
            if (p_Player->GetMapId() != 1712 || p_Player->GetSceneInstanceIDByPackage(1800) != p_SceneInstanceId)
                return;

            p_Player->TeleportTo(1712, 2821.69f, -4568.84f, 292.34f, 6.14f);
        }

        void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceId) override
        {
            if (p_Player->GetMapId() != 1712 || p_Player->GetSceneInstanceIDByPackage(1800) != p_SceneInstanceId)
                return;

            p_Player->TeleportTo(1712, 2821.69f, -4568.84f, 292.34f, 6.14f);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Ravenous Blaze - 254452
class spell_aggramar_ravenous_blaze : public SpellScriptLoader
{
    public:
        spell_aggramar_ravenous_blaze() : SpellScriptLoader("spell_aggramar_ravenous_blaze") { }

        class spell_aggramar_ravenous_blaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggramar_ravenous_blaze_AuraScript);

            enum eSpells
            {
                RavenousBlazeDamage = 254455
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                uint32 l_TickCount = p_AuraEffect->GetTickNumber();
                std::list<Player*> l_Players;
                l_Target->GetPlayerListInGrid(l_Players, 5.0f * static_cast<float>(l_TickCount));
                for (Player* l_Player : l_Players)
                    l_Target->CastSpell(l_Player, eSpells::RavenousBlazeDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_aggramar_ravenous_blaze_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aggramar_ravenous_blaze_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Empowered Flame Rend - 247079
class spell_aggramar_empowered_flame_rend : public SpellScriptLoader
{
    public:
        spell_aggramar_empowered_flame_rend() : SpellScriptLoader("spell_aggramar_empowered_flame_rend") { }

        class spell_aggramar_empowered_flame_rend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggramar_empowered_flame_rend_SpellScript);

            enum eSpells
            {
                FlameRendKnockback = 244208
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FlameRendKnockback, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_aggramar_empowered_flame_rend_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggramar_empowered_flame_rend_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Catalyzed- 247091
class spell_aggramar_catalyzed : public SpellScriptLoader
{
    public:
        spell_aggramar_catalyzed() : SpellScriptLoader("spell_aggramar_catalyzed") { }

        class spell_aggramar_catalyzed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggramar_catalyzed_AuraScript);

            enum eSpells
            {
                UnleashedFlame = 254329
            };

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetUnitOwner();
                if (l_Target && l_Target->GetEntry() == eCreatures::NpcEmberOfTaeshalach)
                    l_Target->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void HandleAfterDispel(DispelInfo* /*l_DispelInfo*/)
            {
                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::UnleashedFlame, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_aggramar_catalyzed_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterDispel += AuraDispelFn(spell_aggramar_catalyzed_AuraScript::HandleAfterDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aggramar_catalyzed_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Searing Binding - 255528
class spell_aggramar_searing_binding : public SpellScriptLoader
{
    public:
        spell_aggramar_searing_binding() : SpellScriptLoader("spell_aggramar_searing_binding") { }

        class spell_aggramar_searing_binding_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggramar_searing_binding_AuraScript);

            enum eSpells
            {
                SearingBindingDamage = 255530
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SearingBindingDamage, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_aggramar_searing_binding_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aggramar_searing_binding_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Searing Tempest (Tracker) - 254653
class spell_aggramar_searing_tempest_tracker : public SpellScriptLoader
{
    public:
        spell_aggramar_searing_tempest_tracker() : SpellScriptLoader("spell_aggramar_searing_tempest_tracker") { }

        class spell_aggramar_searing_tempest_tracker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggramar_searing_tempest_tracker_AuraScript);

            enum eSpells
            {
                FlameRendTracker    = 254654,
                FoeBreakerTracker   = 254655
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Creature* l_Target = GetTarget() ? GetTarget()->ToCreature() : nullptr;
                if (!l_Target)
                    return;

                Aura* l_Aura = l_Target->GetAura(m_scriptSpellId);
                if (!l_Aura || l_Aura->GetStackAmount() < 2)
                    return;

                l_Aura = l_Target->GetAura(eSpells::FlameRendTracker);
                if (!l_Aura || l_Aura->GetStackAmount() < 4)
                    return;

                l_Aura = l_Target->GetAura(eSpells::FoeBreakerTracker);
                if (!l_Aura || l_Aura->GetStackAmount() < 4)
                    return;

                l_Target->UpdateEntry(eCreatures::NpcManifestationOfTaeshalach);

                l_Target->DelayedRemoveAura(eSpells::FlameRendTracker, 1);
                l_Target->DelayedRemoveAura(eSpells::FoeBreakerTracker, 1);
                l_Target->DelayedRemoveAura(m_scriptSpellId, 1);

                for (Map::PlayerList::const_iterator l_Iter = l_Target->GetMap()->GetPlayers().begin(); l_Iter != l_Target->GetMap()->GetPlayers().end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->isGameMaster())
                            continue;

                        l_Player->SetWorldState(eScriptData::DontSweatTheTechniqueWorldState, 1); ///< Criteria for Achievement
                    }
                }
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_aggramar_searing_tempest_tracker_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aggramar_searing_tempest_tracker_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_aggramar()
{
    new boss_aggramar();

    new spell_aggramar_foe_breaker();
    new spell_aggramar_flame_rend();
    new spell_aggramar_searing_tempest();
    new spell_aggramar_taeshalachs_reach();
    new spell_aggramar_taeshalachs_reach_damage();
    new spell_aggramar_wake_of_flame();
    new spell_aggramar_flare_searcher();
    new spell_aggramar_flare_missile();
    new spell_aggramar_meteor_swarm_periodic();
    new spell_aggramar_meteor_swarm_searcher();
    new spell_aggramar_empowered_flare_searcher();
    new spell_aggramar_empowered_flare_missile();
    new spell_aggramar_wrought_in_flame();
    new spell_aggramar_flame_rend_knockback();
    new npc_aggramar_ember_of_taeshalach();
    new npc_aggramar_flame_of_taeshalach();
    new spell_aggramar_ravenous_blaze();
    new spell_aggramar_empowered_flame_rend_cast();
    new spell_aggramar_empowered_flame_rend();
    new spell_aggramar_empowered_searing_tempest();
    new spell_aggramar_catalyzed();
    new spell_aggramar_searing_binding();
    new spell_aggramar_searing_tempest_tracker();

    new npc_aggramar_magni_bronzebeard();
    new PlayerScript_aggramar_outro_cinematic();
}
#endif
