////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScenarioMgr.h"
#include "POIMgr.hpp"
#include "MoveSplineInit.h"
#include "outdoor_argus_invasion_points.h"

float const g_HealthScalingMod = 0.05f;

/// Demon Hunter - 126446
class npc_demon_hunter_126446 : public CreatureScript
{
    public:
        npc_demon_hunter_126446() : CreatureScript("npc_demon_hunter_126446") { }

        struct npc_demon_hunter_126446AI : public ScriptedAI
        {
            npc_demon_hunter_126446AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Breaking = 252320
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Breaking)
                    {
                        Map* l_Map = me->GetMap();
                        if (!l_Map)
                            return;

                        Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                        if (!l_Scenario)
                            return;

                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->SendPlaySpellVisualKit(41953, 0, 0);
                        me->RemoveAllAuras();
                        me->SetFacingToObject(p_Caster);
                        Talk(0, p_Caster->GetGUID());
                        me->DespawnOrUnsummon(4000);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                        }, 2000);

                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, InvasionPointVal::eAssets::Asset_EnhancedEnemies, 0, 0, l_Player, l_Player);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_demon_hunter_126446AI(p_Creature);
        }
};

/// Fel Lord Kaz'ral - 125272
class npc_fel_lord_kazral_125272 : public CreatureScript
{
    public:
        npc_fel_lord_kazral_125272() : CreatureScript("npc_fel_lord_kazral_125272") { }

        struct npc_fel_lord_kazral_125272AI : public BossAI
        {
            npc_fel_lord_kazral_125272AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_SummonConversation = 254524,
                Spell_ChaosWave = 248650,
                Spell_FelSpike = 248697
            };

            enum eEvents
            {
                Event_CastChaosWave = 1,
                Event_CastFelSpike = 2
            };

            enum eNpcs
            {
                Npc_FelSpike = 125274
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_SummonConversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 3000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastChaosWave, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastFelSpike, 10000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastChaosWave:
                {
                    DoCast(eSpells::Spell_ChaosWave);
                    m_Events.ScheduleEvent(eEvents::Event_CastChaosWave, urand(15000, 16000));
                    break;
                }
                case eEvents::Event_CastFelSpike:
                {
                    DoCast(eSpells::Spell_FelSpike);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelSpike, urand(10000, 14000));
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
            return new npc_fel_lord_kazral_125272AI(p_Creature);
        }
};

/// Vogrethar the Defiled - 125587
class npc_vogrethar_the_defiled_125587 : public CreatureScript
{
    public:
        npc_vogrethar_the_defiled_125587() : CreatureScript("npc_vogrethar_the_defiled_125587") { }

        struct npc_vogrethar_the_defiled_125587AI : public BossAI
        {
            npc_vogrethar_the_defiled_125587AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_SummonConversation = 254527,
                Spell_MassMindBlast = 249260,
                Spell_Narcolepsy = 249257,
                Spell_WakeOfBlood = 249259
            };

            enum eEvents
            {
                Event_CastMassMindBlast = 1,
                Event_CastNarcolepsy = 2,
                Event_CastWakeOfBlood = 3
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_SummonConversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastMassMindBlast, 3000);
                m_Events.ScheduleEvent(eEvents::Event_CastNarcolepsy, 15000);
                m_Events.ScheduleEvent(eEvents::Event_CastWakeOfBlood, 19000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastMassMindBlast:
                {
                    DoCast(eSpells::Spell_MassMindBlast);
                    m_Events.ScheduleEvent(eEvents::Event_CastMassMindBlast, urand(8000, 9000));
                    break;
                }
                case eEvents::Event_CastNarcolepsy:
                {
                    DoCast(eSpells::Spell_Narcolepsy);
                    m_Events.ScheduleEvent(eEvents::Event_CastNarcolepsy, 34000);
                    break;
                }
                case eEvents::Event_CastWakeOfBlood:
                {
                    DoCast(eSpells::Spell_WakeOfBlood);
                    m_Events.ScheduleEvent(eEvents::Event_CastWakeOfBlood, 22000);
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
            return new npc_vogrethar_the_defiled_125587AI(p_Creature);
        }
};

/// Felflame Subjugator - 125197
class npc_felflame_subjugator_125197 : public CreatureScript
{
    public:
        npc_felflame_subjugator_125197() : CreatureScript("npc_felflame_subjugator_125197") { }

        struct npc_felflame_subjugator_125197AI : public ScriptedAI
        {
            npc_felflame_subjugator_125197AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_FelFireball = 202295,
                Spell_FelFlamestrike = 253606,
                Spell_SummonBurstingElemental = 252581,
                Spell_PullFire = 252241,
                Spell_Mezmerize = 252582
            };

            enum eEvents
            {
                Event_CastFelFireball = 1,
                Event_CastFelFlamestrike = 2,
                Event_SummonElemental = 3
            };

            enum eNpcs
            {
                Npc_CrazedCorruptor = 126230,
                Npc_BurstingElemental = 127214
            };

            enum eActions
            {
                Action_SubjugatorReset = 1
            };

            EventMap m_Events;
            bool m_CanSummonElemental = true;
            bool m_MezmerizeProcessed = false;

            void InitializeAI() override
            {
                if (me->GetZoneId() == eScenarioZones::Zone_Cengar && me->GetCreatureAddon() && !me->GetCreatureAddon()->Auras.empty())
                {
                    m_Events.ScheduleEvent(eEvents::Event_SummonElemental, 1000);
                }
            }

            void Reset() override
            {
                m_Events.Reset();
            }

            void JustReachedHome() override
            {
                m_CanSummonElemental = true;
                m_MezmerizeProcessed = false;
                m_Events.RescheduleEvent(eEvents::Event_SummonElemental, 1000);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActions::Action_SubjugatorReset)
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->LoadCreaturesAddon();
                        m_MezmerizeProcessed = false;
                    }, 3000);
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                if (roll_chance_i(50))
                {
                    Talk(0);
                }

                me->CastStop();
                m_Events.ScheduleEvent(eEvents::Event_CastFelFireball, urand(1000, 4000));
                m_Events.ScheduleEvent(eEvents::Event_CastFelFlamestrike, urand(4000, 7000));
                m_CanSummonElemental = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->GetZoneId() == eScenarioZones::Zone_Cengar && !m_MezmerizeProcessed && !me->isInCombat() &&
                    me->GetCreatureAddon() && !me->GetCreatureAddon()->Auras.empty())
                {
                    if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_BurstingElemental, 10.0f, true))
                    {
                        if (!l_Creature->HasAura(eSpells::Spell_Mezmerize) && !l_Creature->isInCombat())
                        {
                            DoCast(l_Creature, eSpells::Spell_Mezmerize);
                            m_MezmerizeProcessed = true;
                        }
                    }
                }

                if (!UpdateVictim() && !m_CanSummonElemental)
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastFelFireball:
                {
                    DoCastVictim(eSpells::Spell_FelFireball);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelFireball, urand(6000, 7000));
                    break;
                }
                case eEvents::Event_CastFelFlamestrike:
                {
                    DoCastVictim(eSpells::Spell_FelFlamestrike);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelFlamestrike, 20000);
                    break;
                }
                case eEvents::Event_SummonElemental:
                {
                    if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_CrazedCorruptor, 40.0f, true))
                    {
                        if (!l_Creature->isInCombat())
                        {
                            Position l_SpellDestPos;
                            l_Creature->GetRandomNearPosition(l_SpellDestPos, 5.0f);
                            l_Creature->CastStop();
                            l_Creature->CastSpell(l_SpellDestPos, eSpells::Spell_SummonBurstingElemental, false);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                if (!l_Creature->isInCombat())
                                {
                                    l_Creature->CastSpell(l_Creature, eSpells::Spell_PullFire, false);
                                }
                            }, 3000);
                        }
                    }

                    m_Events.ScheduleEvent(eEvents::Event_SummonElemental, 30000);
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
            return new npc_felflame_subjugator_125197AI(p_Creature);
        }
};

/// Bursting Elemental - 127214
class npc_bursting_elemental_127214 : public CreatureScript
{
    public:
        npc_bursting_elemental_127214() : CreatureScript("npc_bursting_elemental_127214") { }

        struct npc_bursting_elemental_127214AI : public ScriptedAI
        {
            npc_bursting_elemental_127214AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Mezmerize = 252582,
                Spell_FieryRelease = 252575
            };

            enum ePoints
            {
                Point_MoveToCenterEnd = 1
            };

            enum eActions
            {
                Action_SubjugatorReset = 1
            };

            ObjectGuid m_CasterGuid;
            Position const m_MoveToCenterPos = { 662.490845f, 619.768311f, 40.359234f };
            bool m_CanRelease = true;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToCenterEnd, m_MoveToCenterPos);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_Mezmerize)
                {
                    m_CasterGuid = p_Caster->GetGUID();

                    if (!me->IsMoving())
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToCenterEnd, m_MoveToCenterPos);
                    }
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetHealthPct() <= 20 && m_CanRelease)
                {
                    DoCast(eSpells::Spell_FieryRelease);
                    me->DespawnOrUnsummon(5000);
                    m_CanRelease = false;
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                me->GetMotionMaster()->Clear();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                {
                    if (!me->IsMoving() && me->GetExactDist2d(&m_MoveToCenterPos) > 20.0f && !me->HasAura(eSpells::Spell_Mezmerize))
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToCenterEnd, m_MoveToCenterPos);
                    }

                    if (me->GetExactDist2d(&m_MoveToCenterPos) < 20.0f && !me->HasAura(eSpells::Spell_Mezmerize) && me->IsMoving())
                    {
                        me->GetMotionMaster()->Clear();
                    }

                    if (me->GetExactDist2d(&m_MoveToCenterPos) < 5.0f && m_CanRelease)
                    {
                        if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_CasterGuid))
                        {
                            me->GetMotionMaster()->Clear();

                            if (l_Creature->isAlive())
                            {
                                me->SetFacingToObject(l_Creature);
                                l_Creature->AI()->DoAction(eActions::Action_SubjugatorReset);
                            }

                            DoCast(eSpells::Spell_FieryRelease);
                            me->DespawnOrUnsummon(5000);
                            m_CanRelease = false;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_bursting_elemental_127214AI(p_Creature);
        }
};

/// Magma Giant - 124835
class npc_magma_giant_124835 : public CreatureScript
{
    public:
        npc_magma_giant_124835() : CreatureScript("npc_magma_giant_124835") { }

        struct npc_magma_giant_124835AI : public ScriptedAI
        {
            npc_magma_giant_124835AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_PyrochlasticBurst = 252790,
                Spell_MoltenBoulder = 252731
            };

            enum ePoints
            {
                Point_MoveFromLavaEnd = 1,
                Point_MoveToCenterEnd = 2
            };

            enum eEvents
            {
                Event_CastMoltenBoulder = 1
            };

            EventMap m_Events;
            Position const m_FirstGiantMovePos = { 701.8446f, 649.6245f, 40.17906f };
            Position const m_SecondGiantMovePos = { 705.9819f, 593.2598f, 40.74989f };
            Position const m_ThirdGiantMovePos = { 658.4035f, 568.6135f, 40.51607f };
            Position const m_FourthGiantMovePos = { 614.6817f, 604.7519f, 40.66399f };
            Position const m_FifthGiantMovePos = { 639.9336f, 663.0723f, 40.0517f };
            Position const m_CenterMovePos = { 657.9393f, 621.0382f, 40.41438f };

            void Reset() override
            {
                m_Events.Reset();
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == InvasionPointCengar::eActions::Action_ActivateMagmaGiant)
                {
                    DoCast(eSpells::Spell_PyrochlasticBurst, true);

                    if (me->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[0])
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(ePoints::Point_MoveFromLavaEnd, m_FirstGiantMovePos);
                        }, 4000);
                    }
                    else if (me->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[1])
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(ePoints::Point_MoveFromLavaEnd, m_SecondGiantMovePos);
                        }, 4000);
                    }
                    else if (me->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[2])
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(ePoints::Point_MoveFromLavaEnd, m_ThirdGiantMovePos);
                        }, 4000);
                    }
                    else if (me->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[3])
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(ePoints::Point_MoveFromLavaEnd, m_FourthGiantMovePos);
                        }, 4000);
                    }
                    else if (me->ComputeLinkedId() == InvasionPointCengar::g_GiantLinkedIds[4])
                    {
                        me->AddDelayedEvent([this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(ePoints::Point_MoveFromLavaEnd, m_FifthGiantMovePos);
                        }, 4000);
                    }
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveFromLavaEnd)
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 16384);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToCenterEnd, m_CenterMovePos);
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                me->GetMotionMaster()->Clear();
                m_Events.ScheduleEvent(eEvents::Event_CastMoltenBoulder, 7000);
                me->DisableEvadeMode();
            }

            void JustDied(Unit* p_Killer) override
            {
                Map* l_Map = me->GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_KILL_CREATURE, InvasionPointCengar::eAssets::Asset_MagmaFlows, 1, 0, GetAnyPlayer(p_Killer), GetAnyPlayer(p_Killer));

                m_Events.Reset();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastMoltenBoulder:
                {
                    DoCast(eSpells::Spell_MoltenBoulder);
                    m_Events.ScheduleEvent(eEvents::Event_CastMoltenBoulder, urand(20000, 30000));
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
            return new npc_magma_giant_124835AI(p_Creature);
        }
};

/// Malphazel - 125578
class npc_malphazel_125578 : public CreatureScript
{
    public:
        npc_malphazel_125578() : CreatureScript("npc_malphazel_125578") { }

        struct npc_malphazel_125578AI : public BossAI
        {
            npc_malphazel_125578AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_SummonConversation = 254528,
                Spell_SlumberFog = 249235,
                Spell_WaveOfDread = 249246,
                Spell_WaveOfDreadPeriodic = 249250,
                Spell_ShadowIllusion = 249245
            };

            enum eEvents
            {
                Event_CastSlumberFog = 1,
                Event_CastWaveOfDread = 2,
                Event_CastShadowIllusion = 3
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_SummonConversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastSlumberFog, 11000);
                m_Events.ScheduleEvent(eEvents::Event_CastWaveOfDread, 16000);
                m_Events.ScheduleEvent(eEvents::Event_CastShadowIllusion, 25000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastSlumberFog:
                {
                    DoCast(eSpells::Spell_SlumberFog);
                    m_Events.ScheduleEvent(eEvents::Event_CastSlumberFog, urand(20000, 30000));
                    break;
                }
                case eEvents::Event_CastWaveOfDread:
                {
                    DoCast(eSpells::Spell_WaveOfDread);
                    m_Events.ScheduleEvent(eEvents::Event_CastWaveOfDread, urand(20000, 30000));
                    break;
                }
                case eEvents::Event_CastShadowIllusion:
                {
                    DoCast(eSpells::Spell_ShadowIllusion);
                    m_Events.ScheduleEvent(eEvents::Event_CastShadowIllusion, urand(20000, 30000));
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
            return new npc_malphazel_125578AI(p_Creature);
        }
};

/// Slumber Fog - 125576
class npc_slumber_fog_125576 : public CreatureScript
{
    public:
        npc_slumber_fog_125576() : CreatureScript("npc_slumber_fog_125576") { }

        struct npc_slumber_fog_125576AI : public ScriptedAI
        {
            npc_slumber_fog_125576AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_SlumberFogAreatrigger = 249239
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(eSpells::Spell_SlumberFogAreatrigger);

                me->AddDelayedEvent([this]() -> void
                {
                    me->GetMotionMaster()->MoveCirclePath(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 5.0f, false, 15);
                }, 2000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_slumber_fog_125576AI(p_Creature);
        }
};

/// Pit Lord Vilemus - 124719
class npc_pit_lord_vilemus_124719 : public CreatureScript
{
    public:
        npc_pit_lord_vilemus_124719() : CreatureScript("npc_pit_lord_vilemus_124719") { }

        struct npc_pit_lord_vilemus_124719AI : public BossAI
        {
            npc_pit_lord_vilemus_124719AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_FelBreath = 247731,
                Spell_Stomp = 247733,
                Spell_Drain = 247739,
                Spell_BonusRollPrompt = 254439
            };

            enum eEvents
            {
                Event_CastFelBreath = 1,
                Event_CastStomp = 2,
                Event_CastDrain = 3
            };

            EventMap m_Events;

            bool GetHealthModPersonal(float& p_Modifier, uint32 p_Count) override
            {
                if (p_Count > 10)
                {
                    p_Count -= 10;
                    p_Modifier = g_HealthScalingMod;
                }

                return true;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void Reset() override
            {
                m_Events.Reset();
                me->SetBonusLootSpell(eSpells::Spell_BonusRollPrompt);
            }

            void EnterEvadeMode() override
            {
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (me->GetPositionZ() < me->GetHomePosition().GetPositionZ())
                {
                    me->NearTeleportTo(me->GetHomePosition());
                }

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastFelBreath, 13000);
                m_Events.ScheduleEvent(eEvents::Event_CastStomp, 19000);
                m_Events.ScheduleEvent(eEvents::Event_CastDrain, 21000);
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(0);
                m_Events.Reset();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (Unit* l_Victim = me->getVictim())
                {
                    if (l_Victim->GetPositionZ() < me->GetPositionZ() &&
                        l_Victim->IsFalling())
                    {
                        me->getThreatManager().modifyThreatPercent(l_Victim, -99.0f);
                    }
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastFelBreath:
                {
                    DoCast(eSpells::Spell_FelBreath);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelBreath, urand(13000, 20000));
                    break;
                }
                case eEvents::Event_CastStomp:
                {
                    DoCast(eSpells::Spell_Stomp);
                    m_Events.ScheduleEvent(eEvents::Event_CastStomp, urand(15000, 20000));
                    break;
                }
                case eEvents::Event_CastDrain:
                {
                    DoCastVictim(eSpells::Spell_Drain);
                    m_Events.ScheduleEvent(eEvents::Event_CastDrain, urand(16000, 20000));
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
            return new npc_pit_lord_vilemus_124719AI(p_Creature);
        }
};

/// Mazgoroth - 125137
class npc_mazgoroth_125137 : public CreatureScript
{
    public:
        npc_mazgoroth_125137() : CreatureScript("npc_mazgoroth_125137") { }

        struct npc_mazgoroth_125137AI : public BossAI
        {
            npc_mazgoroth_125137AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_SummonConversation = 254522,
                Spell_Hellstomp = 248506,
                Spell_FelBurn = 248500,
                Spell_FelSlash = 248509
            };

            enum eEvents
            {
                Event_CastHellstomp = 1,
                Event_CastFelBurn = 2,
                Event_CastFelSlash = 3
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_SummonConversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->RemoveAllAreasTrigger();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastHellstomp, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastFelBurn, 11000);
                m_Events.ScheduleEvent(eEvents::Event_CastFelSlash, 16000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastHellstomp:
                {
                    DoCast(eSpells::Spell_Hellstomp);
                    m_Events.ScheduleEvent(eEvents::Event_CastHellstomp, 10000);
                    break;
                }
                case eEvents::Event_CastFelBurn:
                {
                    DoCast(eSpells::Spell_FelBurn);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelBurn, 40000);
                    break;
                }
                case eEvents::Event_CastFelSlash:
                {
                    DoCastVictim(eSpells::Spell_FelSlash);
                    me->DeleteThreatList();
                    m_Events.ScheduleEvent(eEvents::Event_CastFelSlash, 31000);
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
            return new npc_mazgoroth_125137AI(p_Creature);
        }
};

/// Sotanathor - 124555
class npc_sotanathor_124555 : public CreatureScript
{
    public:
        npc_sotanathor_124555() : CreatureScript("npc_sotanathor_124555") { }

        struct npc_sotanathor_124555AI : public BossAI
        {
            npc_sotanathor_124555AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_SeedOfDestruction = 247437,
                Spell_SoulCleave = 247410,
                Spell_Cavitation = 247415,
                Spell_Silence = 247698,
                Spell_BonusRollPrompt = 254446
            };

            enum eEvents
            {
                Event_CastSoulCleave = 1,
                Event_CastSeedOfDestruction = 2,
                Event_CastCavitation = 3,
                Event_CastSilence = 4
            };

            EventMap m_Events;

            bool GetHealthModPersonal(float& p_Modifier, uint32 p_Count) override
            {
                if (p_Count > 10)
                {
                    p_Count -= 10;
                    p_Modifier = g_HealthScalingMod;
                }

                return true;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void Reset() override
            {
                m_Events.Reset();
                me->RemoveAllAreasTrigger();
                me->SetBonusLootSpell(eSpells::Spell_BonusRollPrompt);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastSeedOfDestruction, 16000);
                m_Events.ScheduleEvent(eEvents::Event_CastSoulCleave, 17000);
                m_Events.ScheduleEvent(eEvents::Event_CastCavitation, 21000);
                m_Events.ScheduleEvent(eEvents::Event_CastSilence, 22000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastSeedOfDestruction:
                {
                    DoCast(eSpells::Spell_SeedOfDestruction);
                    m_Events.ScheduleEvent(eEvents::Event_CastSeedOfDestruction, 17000);
                    break;
                }
                case eEvents::Event_CastSoulCleave:
                {
                    DoCastVictim(eSpells::Spell_SoulCleave);
                    m_Events.ScheduleEvent(eEvents::Event_CastSoulCleave, 27000);
                    break;
                }
                case eEvents::Event_CastCavitation:
                {
                    DoCast(eSpells::Spell_Cavitation);
                    m_Events.ScheduleEvent(eEvents::Event_CastCavitation, 28000);
                    break;
                }
                case eEvents::Event_CastSilence:
                {
                    DoCast(eSpells::Spell_Silence);
                    m_Events.ScheduleEvent(eEvents::Event_CastSilence, 28000);
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
            return new npc_sotanathor_124555AI(p_Creature);
        }
};

/// Mistress Alluradel - 124625
class npc_mistress_alluradel_124625 : public CreatureScript
{
    public:
        npc_mistress_alluradel_124625() : CreatureScript("npc_mistress_alluradel_124625") { }

        struct npc_mistress_alluradel_124625AI : public BossAI
        {
            npc_mistress_alluradel_124625AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_HeartBreaker = 247517,
                Spell_FelLash = 247604,
                Spell_BeguilingCharm = 247549,
                Spell_Sadist = 247590,
                Spell_BonusRollPrompt = 254437
            };

            enum eEvents
            {
                Event_CastHeartBreaker = 1,
                Event_CastFelLash = 2,
                Event_CastBeguilingCharm = 3
            };

            EventMap m_Events;

            bool GetHealthModPersonal(float& p_Modifier, uint32 p_Count) override
            {
                if (p_Count > 10)
                {
                    p_Count -= 10;
                    p_Modifier = g_HealthScalingMod;
                }

                return true;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void Reset() override
            {
                m_Events.Reset();
                me->SetBonusLootSpell(eSpells::Spell_BonusRollPrompt);
                DoCast(eSpells::Spell_Sadist, true);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_CastHeartBreaker, 8000);
                m_Events.ScheduleEvent(eEvents::Event_CastFelLash, 16000);
                m_Events.ScheduleEvent(eEvents::Event_CastBeguilingCharm, 32000);
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(2);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastHeartBreaker:
                {
                    DoCast(eSpells::Spell_HeartBreaker);
                    m_Events.ScheduleEvent(eEvents::Event_CastHeartBreaker, urand(21000, 25000));
                    break;
                }
                case eEvents::Event_CastFelLash:
                {
                    if (Unit* l_Target = me->SelectNearbyTarget(NULL, 30.0f, 0U, true, true, true, true))
                    {
                        me->AttackStop();
                        me->SetFacingToObject(l_Target);
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        DoCast(l_Target, eSpells::Spell_FelLash);

                        me->AddDelayedEvent([this]() -> void
                        {
                            me->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        }, 2000);
                    }

                    m_Events.ScheduleEvent(eEvents::Event_CastFelLash, urand(31000, 32000));
                    break;
                }
                case eEvents::Event_CastBeguilingCharm:
                {
                    DoCast(eSpells::Spell_BeguilingCharm);
                    m_Events.ScheduleEvent(eEvents::Event_CastBeguilingCharm, urand(36000, 41000));
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
            return new npc_mistress_alluradel_124625AI(p_Creature);
        }
};

/// Marsh Shambler - 127098
class npc_marsh_shambler_127098 : public CreatureScript
{
    public:
        npc_marsh_shambler_127098() : CreatureScript("npc_marsh_shambler_127098") { }

        struct npc_marsh_shambler_127098AI : public ScriptedAI
        {
            npc_marsh_shambler_127098AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_FirstMarshShambler[14] =
            {
                { -1837.283f, -1391.218f, 10.17467f },
                { -1838.033f, -1393.968f, 9.924675f },
                { -1839.783f, -1400.968f, 9.674675f },
                { -1840.283f, -1402.718f, 9.424675f },
                { -1840.533f, -1403.718f, 9.174675f },
                { -1841.033f, -1405.718f, 8.924675f },
                { -1841.283f, -1407.468f, 8.674675f },
                { -1841.533f, -1409.468f, 8.424675f },
                { -1842.033f, -1411.218f, 8.424675f },
                { -1842.283f, -1412.218f, 8.174675f },
                { -1843.283f, -1416.218f, 7.674675f },
                { -1844.033f, -1418.968f, 7.424675f },
                { -1845.283f, -1423.968f, 7.174675f },
                { -1846.66f, -1429.1f, 6.526266f }
            };

            G3D::Vector3 const Path_SecondtMarshShambler[13] =
            {
                { -1793.233f, -1676.149f, -9.925103f },
                { -1792.233f, -1657.149f, -7.425103f },
                { -1791.983f, -1652.149f, -1.175103f },
                { -1791.733f, -1646.149f, 2.824897f },
                { -1791.483f, -1642.149f, 3.574897f },
                { -1791.233f, -1637.149f, 4.074897f },
                { -1790.733f, -1622.399f, 4.324897f },
                { -1790.483f, -1616.399f, 4.574897f },
                { -1790.233f, -1609.399f, 4.324897f },
                { -1789.983f, -1604.399f, 4.074897f },
                { -1789.733f, -1600.399f, 3.324897f },
                { -1789.483f, -1597.399f, 2.824897f },
                { -1788.8f, -1583.47f, 2.938676f }
            };

            enum eSpells
            {
                Spell_FelEnraged = 252137,
                Spell_BalefulBeam = 251388,
                Spell_DarkSpores = 251380
            };

            enum eEvents
            {
                Event_CastBalefulBeam = 1,
                Event_CastDarkSpores = 2
            };

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void JustReachedHome() override
            {
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 4196352);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == InvasionPointNaigtal::eActions::Action_FirstMarshShamblerFreed)
                {
                    DoCast(eSpells::Spell_FelEnraged, true);
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_FirstMarshShambler, 14, true);
                }
                else if (p_Action == InvasionPointNaigtal::eActions::Action_SecondMarshShamblerFreed)
                {
                    DoCast(eSpells::Spell_FelEnraged, true);
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_SecondtMarshShambler, 13, true);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveEnd)
                {
                    me->SetHomePosition(me->GetPosition());
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 4196352);
                    me->SetRespawnRadius(20.0f);
                    me->SetDefaultMovementType(RANDOM_MOTION_TYPE);
                    me->GetMotionMaster()->InitDefault();
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastBalefulBeam, 11000);
                m_Events.ScheduleEvent(eEvents::Event_CastDarkSpores, 16000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastBalefulBeam:
                {
                    me->AttackStop();
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    DoCast(eSpells::Spell_BalefulBeam);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    }, 4000);

                    m_Events.ScheduleEvent(eEvents::Event_CastBalefulBeam, urand(21000, 25000));
                    break;
                }
                case eEvents::Event_CastDarkSpores:
                {
                    DoCast(eSpells::Spell_DarkSpores);
                    m_Events.ScheduleEvent(eEvents::Event_CastDarkSpores, urand(31000, 32000));
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
            return new npc_marsh_shambler_127098AI(p_Creature);
        }
};

/// Beaming Eye - 127990
class npc_beaming_eye_127990 : public CreatureScript
{
    public:
        npc_beaming_eye_127990() : CreatureScript("npc_beaming_eye_127990") { }

        struct npc_beaming_eye_127990AI : public ScriptedAI
        {
            npc_beaming_eye_127990AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_BeamingGazeAreatrigger = 253991
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetRooted(true);
                DoCast(eSpells::Spell_BeamingGazeAreatrigger);
                me->GetMotionMaster()->MoveRotate(10000, RAND(RotateDirection::ROTATE_DIRECTION_RIGHT, RotateDirection::ROTATE_DIRECTION_LEFT));
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_beaming_eye_127990AI(p_Creature);
        }
};

/// Harbinger Drel'nathar - 125483
class npc_harbinger_drelnathar_125483 : public CreatureScript
{
    public:
        npc_harbinger_drelnathar_125483() : CreatureScript("npc_harbinger_drelnathar_125483") { }

        struct npc_harbinger_drelnathar_125483AI : public BossAI
        {
            npc_harbinger_drelnathar_125483AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_SecreteShadows = 249071,
                Spell_ShadowClone = 249072,
                Spell_ShadowNova = 249066,
                Spell_EyeOfDarkness = 249068
            };

            enum eEvents
            {
                Event_CastSecreteShadows = 1,
                Event_CastShadowClone = 2,
                Event_CastShadowNova = 3,
                Event_CastEyeOfDarkness = 4
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastSecreteShadows, 7000);
                m_Events.ScheduleEvent(eEvents::Event_CastShadowClone, 11000);
                m_Events.ScheduleEvent(eEvents::Event_CastShadowNova, 16000);
                m_Events.ScheduleEvent(eEvents::Event_CastEyeOfDarkness, 26000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastSecreteShadows:
                {
                    DoCastVictim(eSpells::Spell_SecreteShadows);
                    m_Events.ScheduleEvent(eEvents::Event_CastSecreteShadows, urand(22000, 29000));
                    break;
                }
                case eEvents::Event_CastShadowClone:
                {
                    DoCast(eSpells::Spell_ShadowClone);
                    m_Events.ScheduleEvent(eEvents::Event_CastShadowClone, urand(20000, 25000));
                    break;
                }
                case eEvents::Event_CastShadowNova:
                {
                    DoCast(eSpells::Spell_ShadowNova);
                    m_Events.ScheduleEvent(eEvents::Event_CastShadowNova, 34000);
                    break;
                }
                case eEvents::Event_CastEyeOfDarkness:
                {
                    DoCast(eSpells::Spell_EyeOfDarkness);
                    m_Events.ScheduleEvent(eEvents::Event_CastEyeOfDarkness, 34000);
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
            return new npc_harbinger_drelnathar_125483AI(p_Creature);
        }
};

/// Matron Folnuna - 124514
class npc_matron_folnuna_124514 : public CreatureScript
{
    public:
        npc_matron_folnuna_124514() : CreatureScript("npc_matron_folnuna_124514") { }

        struct npc_matron_folnuna_124514AI : public BossAI
        {
            npc_matron_folnuna_124514AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Nausea = 247441,
                Spell_FelBlast = 254147,
                Spell_GrotesqueSpawn = 247443,
                Spell_SlumberingGasp = 247382,
                Spell_BonusRollPrompt = 254441
            };

            enum eEvents
            {
                Event_CastFelBlast = 1,
                Event_CastGrotesqueSpawn = 2
            };

            EventMap m_Events;

            bool GetHealthModPersonal(float& p_Modifier, uint32 p_Count) override
            {
                if (p_Count > 10)
                {
                    p_Count -= 10;
                    p_Modifier = g_HealthScalingMod;
                }

                return true;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void Reset() override
            {
                m_Events.Reset();
                summons.DespawnAll();
                SetCombatMovement(false);
                me->SetBonusLootSpell(eSpells::Spell_BonusRollPrompt);
                DoCast(eSpells::Spell_Nausea);
                me->SetPower(Powers::POWER_ENERGY, 0);
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_CastFelBlast, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastGrotesqueSpawn, 9000);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(1);
                summons.DespawnAll();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                if (me->GetPower(Powers::POWER_ENERGY) == 100)
                {
                    DoCast(eSpells::Spell_SlumberingGasp);
                    me->SetPower(Powers::POWER_ENERGY, 0);
                }

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastFelBlast:
                {
                    if (Unit* l_Target = me->SelectNearbyTarget(nullptr, 30.0f, 0, true, true, true, true))
                    {
                        DoCast(l_Target, eSpells::Spell_FelBlast);
                    }

                    m_Events.ScheduleEvent(eEvents::Event_CastFelBlast, urand(9000, 18000));
                    break;
                }
                case eEvents::Event_CastGrotesqueSpawn:
                {
                    DoCast(eSpells::Spell_GrotesqueSpawn);
                    m_Events.ScheduleEvent(eEvents::Event_CastGrotesqueSpawn, urand(32000, 34000));
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
            return new npc_matron_folnuna_124514AI(p_Creature);
        }
};

/// Dreadbringer Valus - 125527
class npc_dreadbringer_valus_125527 : public CreatureScript
{
    public:
        npc_dreadbringer_valus_125527() : CreatureScript("npc_dreadbringer_valus_125527") { }

        struct npc_dreadbringer_valus_125527AI : public BossAI
        {
            npc_dreadbringer_valus_125527AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Conversation = 254526,
                Spell_CarrionSwarm = 249229,
                Spell_SlumberFog = 249235,
                Spell_VampiricCleave = 249226
            };

            enum eEvents
            {
                Event_CastCarrionSwarm = 1,
                Event_CastSlumberFog = 2,
                Event_CastVampiricCleave = 3
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_Conversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastCarrionSwarm, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastSlumberFog, 10000);
                m_Events.ScheduleEvent(eEvents::Event_CastVampiricCleave, 20000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastCarrionSwarm:
                {
                    DoCast(eSpells::Spell_CarrionSwarm);
                    m_Events.ScheduleEvent(eEvents::Event_CastCarrionSwarm, 18000);
                    break;
                }
                case eEvents::Event_CastSlumberFog:
                {
                    DoCast(eSpells::Spell_SlumberFog);
                    m_Events.ScheduleEvent(eEvents::Event_CastSlumberFog, urand(15000, 20000));
                    break;
                }
                case eEvents::Event_CastVampiricCleave:
                {
                    me->AttackStop();
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    DoCast(eSpells::Spell_VampiricCleave);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    }, 4000);

                    m_Events.ScheduleEvent(eEvents::Event_CastVampiricCleave, urand(15000, 20000));
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
            return new npc_dreadbringer_valus_125527AI(p_Creature);
        }
};

/// Flamebringer Az'rothel - 125655
class npc_flamebringer_azrothel_125655 : public CreatureScript
{
    public:
        npc_flamebringer_azrothel_125655() : CreatureScript("npc_flamebringer_azrothel_125655") { }

        struct npc_flamebringer_azrothel_125655AI : public BossAI
        {
            npc_flamebringer_azrothel_125655AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Conversation = 254530,
                Spell_RainOfFire = 249400,
                Spell_FlameBreath = 249438,
                Spell_Cripple = 249391,
                Spell_SummonExplosiveOrbs = 249440
            };

            enum eEvents
            {
                Event_CastRainOfFire = 1,
                Event_CastFlameBreath = 2,
                Event_CastCripple = 3,
                Event_CastSummonExplosiveOrbs = 4
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_Conversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->RemoveAllAreasTrigger();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastRainOfFire, 5000);
                m_Events.ScheduleEvent(eEvents::Event_CastFlameBreath, 10000);
                m_Events.ScheduleEvent(eEvents::Event_CastCripple, 15000);
                m_Events.ScheduleEvent(eEvents::Event_CastSummonExplosiveOrbs, 20000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastRainOfFire:
                {
                    DoCast(eSpells::Spell_RainOfFire);
                    m_Events.ScheduleEvent(eEvents::Event_CastRainOfFire, urand(20000, 21000));
                    break;
                }
                case eEvents::Event_CastFlameBreath:
                {
                    DoCast(eSpells::Spell_FlameBreath);
                    m_Events.ScheduleEvent(eEvents::Event_CastFlameBreath, 41000);
                    break;
                }
                case eEvents::Event_CastCripple:
                {
                    DoCast(eSpells::Spell_Cripple);
                    m_Events.ScheduleEvent(eEvents::Event_CastCripple, urand(20000, 21000));
                    break;
                }
                case eEvents::Event_CastSummonExplosiveOrbs:
                {
                    DoCast(eSpells::Spell_SummonExplosiveOrbs);
                    m_Events.ScheduleEvent(eEvents::Event_CastSummonExplosiveOrbs, 43000);
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
            return new npc_flamebringer_azrothel_125655AI(p_Creature);
        }
};

/// Flamecaller Vezrah - 125280
class npc_flamecaller_vezrah_125280 : public CreatureScript
{
    public:
        npc_flamecaller_vezrah_125280() : CreatureScript("npc_flamecaller_vezrah_125280") { }

        struct npc_flamecaller_vezrah_125280AI : public BossAI
        {
            npc_flamecaller_vezrah_125280AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Conversation = 251256,
                Spell_Pyrogenics = 248757,
                Spell_DestructiveFlames = 248768,
                Spell_LavaWaves = 248758
            };

            enum eEvents
            {
                Event_CastPyrogenics = 1,
                Event_CastDestructiveFlames = 2,
                Event_CastLavaWaves = 3
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_Conversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastPyrogenics, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastDestructiveFlames, 11000);
                m_Events.ScheduleEvent(eEvents::Event_CastLavaWaves, 16000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastPyrogenics:
                {
                    DoCast(eSpells::Spell_Pyrogenics);
                    m_Events.ScheduleEvent(eEvents::Event_CastPyrogenics, 16000);
                    break;
                }
                case eEvents::Event_CastDestructiveFlames:
                {
                    DoCast(eSpells::Spell_DestructiveFlames);
                    m_Events.ScheduleEvent(eEvents::Event_CastDestructiveFlames, 46000);
                    break;
                }
                case eEvents::Event_CastLavaWaves:
                {
                    DoCast(eSpells::Spell_LavaWaves);
                    m_Events.ScheduleEvent(eEvents::Event_CastLavaWaves, 35000);
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
            return new npc_flamecaller_vezrah_125280AI(p_Creature);
        }
};

/// Lava Wave - 125313
class npc_lava_wave_125313 : public CreatureScript
{
    public:
        npc_lava_wave_125313() : CreatureScript("npc_lava_wave_125313") { }

        struct npc_lava_wave_125313AI : public ScriptedAI
        {
            npc_lava_wave_125313AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_LavaWavesAreatrigger = 248760
            };

            enum ePoints
            {
                Point_MoveToSummonerEnd = 1,
                Point_MoveFromSummonerEnd = 2
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                DoCast(eSpells::Spell_LavaWavesAreatrigger);
                me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToSummonerEnd, p_Summoner->GetPosition(), true);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_MoveToSummonerEnd:
                {
                    Position l_StartPos = me->GetPosition();
                    Position l_MovePos = l_StartPos;
                    std::vector<G3D::Vector3> l_Path;

                    for (uint8 l_Itr = 0; l_Itr < 100; l_Itr++)
                    {
                        l_MovePos.SimplePosXYRelocationByAngle(l_MovePos, 4.0f, l_StartPos.m_orientation, true);
                        me->UpdateAllowedPositionZ(l_MovePos.m_positionX, l_MovePos.m_positionY, l_MovePos.m_positionZ);

                        if (l_MovePos.IsPositionValid() && l_StartPos.GetExactDist2d(&l_MovePos) < 60.0f)
                        {
                            l_Path.push_back(l_MovePos.AsVector3());
                        }
                        else
                            break;
                    }

                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveFromSummonerEnd, l_Path.data(), l_Path.size(), false);
                    break;
                }
                case ePoints::Point_MoveFromSummonerEnd:
                {
                    me->DespawnOrUnsummon();
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lava_wave_125313AI(p_Creature);
        }
};

/// Dread Knight Zak'gal - 125252
class npc_dread_knight_zakgal_125252 : public CreatureScript
{
    public:
        npc_dread_knight_zakgal_125252() : CreatureScript("npc_dread_knight_zakgal_125252") { }

        struct npc_dread_knight_zakgal_125252AI : public BossAI
        {
            npc_dread_knight_zakgal_125252AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Conversation = 254525,
                Spell_ChaosWave = 248650,
                Spell_FragmentOfArgus = 248655
            };

            enum eEvents
            {
                Event_CastChaosWave = 1,
                Event_CastFragmentOfArgus = 2
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_Conversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastChaosWave, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastFragmentOfArgus, 11000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastChaosWave:
                {
                    DoCast(eSpells::Spell_ChaosWave);
                    m_Events.ScheduleEvent(eEvents::Event_CastChaosWave, 10000);
                    break;
                }
                case eEvents::Event_CastFragmentOfArgus:
                {
                    DoCast(eSpells::Spell_FragmentOfArgus);
                    m_Events.ScheduleEvent(eEvents::Event_CastFragmentOfArgus, 25000);
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
            return new npc_dread_knight_zakgal_125252AI(p_Creature);
        }
};

/// Future Image - 126024
class npc_future_image_126024 : public CreatureScript
{
    public:
        npc_future_image_126024() : CreatureScript("npc_future_image_126024") { }

        struct npc_future_image_126024AI : public ScriptedAI
        {
            npc_future_image_126024AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_CopyAppearance = 250738,
                Spell_TemporalPrecognition = 252488
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                p_Summoner->CastSpell(me, eSpells::Spell_CopyAppearance, true);
                DoCast(eSpells::Spell_TemporalPrecognition, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_future_image_126024AI(p_Creature);
        }
};

/// Decimax - 126007
class npc_decimax_126007 : public CreatureScript
{
    public:
        npc_decimax_126007() : CreatureScript("npc_decimax_126007") { }

        struct npc_decimax_126007AI : public ScriptedAI
        {
            npc_decimax_126007AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            G3D::Vector3 const Path_Decimax[16] =
            {
                { -3852.457f, -4988.672f, 231.9951f },
                { -3920.884f, -5058.189f, 231.9951f },
                { -4010.707f, -5096.229f, 231.9951f },
                { -4108.249f, -5097.0f, 231.9951f },
                { -4198.661f, -5060.383f, 231.9951f },
                { -4268.178f, -4991.956f, 231.9951f },
                { -4306.218f, -4902.133f, 231.9951f },
                { -4306.988f, -4804.591f, 231.9951f },
                { -4270.373f, -4714.179f, 231.9951f },
                { -4201.945f, -4644.662f, 231.9951f },
                { -4112.123f, -4606.622f, 231.9951f },
                { -4014.58f, -4605.852f, 231.9951f },
                { -3924.168f, -4642.468f, 231.9951f },
                { -3854.651f, -4710.896f, 231.9951f },
                { -3816.611f, -4800.718f, 231.9951f },
                { -3815.841f, -4898.26f, 231.9951f }
            };

            enum ePoints
            {
                Point_CirclePathEnd = 1
            };

            void InitializeAI() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == InvasionPointAurinor::eActions::Action_DecimaxStartCirclePath)
                {
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_Decimax, 16);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_CirclePathEnd:
                {
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_Decimax, 16);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_decimax_126007AI(p_Creature);
        }
};

/// Vel'thrak the Punisher - 125634
class npc_velthrak_the_punisher_125634 : public CreatureScript
{
    public:
        npc_velthrak_the_punisher_125634() : CreatureScript("npc_velthrak_the_punisher_125634") { }

        struct npc_velthrak_the_punisher_125634AI : public BossAI
        {
            npc_velthrak_the_punisher_125634AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Conversation = 254529,
                Spell_BlastWaves = 249403,
                Spell_Cripple = 249391,
                Spell_RainOfFire = 249400,
                Spell_FelStomp = 249392
            };

            enum eEvents
            {
                Event_CastBlastWaves = 1,
                Event_CastCripple = 2,
                Event_RainOfFire = 3,
                Event_FelStomp = 4
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_Conversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->RemoveAllAreasTrigger();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastBlastWaves, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastCripple, 11000);
                m_Events.ScheduleEvent(eEvents::Event_RainOfFire, 13000);
                m_Events.ScheduleEvent(eEvents::Event_FelStomp, 21000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastBlastWaves:
                {
                    DoCast(eSpells::Spell_BlastWaves);
                    m_Events.ScheduleEvent(eEvents::Event_CastBlastWaves, 32000);
                    break;
                }
                case eEvents::Event_CastCripple:
                {
                    DoCast(eSpells::Spell_Cripple);
                    m_Events.ScheduleEvent(eEvents::Event_CastCripple, 21000);
                    break;
                }
                case eEvents::Event_RainOfFire:
                {
                    DoCast(eSpells::Spell_RainOfFire);
                    m_Events.ScheduleEvent(eEvents::Event_RainOfFire, 21000);
                    break;
                }
                case eEvents::Event_FelStomp:
                {
                    DoCast(eSpells::Spell_FelStomp);
                    m_Events.ScheduleEvent(eEvents::Event_FelStomp, 32000);
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
            return new npc_velthrak_the_punisher_125634AI(p_Creature);
        }
};

/// Flameweaver Verathix - 125314
class npc_flameweaver_verathix_125314 : public CreatureScript
{
    public:
        npc_flameweaver_verathix_125314() : CreatureScript("npc_flameweaver_verathix_125314") { }

        struct npc_flameweaver_verathix_125314AI : public BossAI
        {
            npc_flameweaver_verathix_125314AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Conversation = 251252,
                Spell_FlameFissure = 248772,
                Spell_FelLightning = 248769,
                Spell_LavaWaves = 248758
            };

            enum eEvents
            {
                Event_CastFlameFissure = 1,
                Event_CastFelLightning = 2,
                Event_LavaWaves = 3
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_Conversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastFlameFissure, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastFelLightning, 12000);
                m_Events.ScheduleEvent(eEvents::Event_LavaWaves, 21000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastFlameFissure:
                {
                    DoCastVictim(eSpells::Spell_FlameFissure);
                    m_Events.ScheduleEvent(eEvents::Event_CastFlameFissure, 44000);
                    break;
                }
                case eEvents::Event_CastFelLightning:
                {
                    DoCastVictim(eSpells::Spell_FelLightning);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelLightning, 11000);
                    break;
                }
                case eEvents::Event_LavaWaves:
                {
                    DoCast(eSpells::Spell_LavaWaves);
                    m_Events.ScheduleEvent(eEvents::Event_LavaWaves, 45000);
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
            return new npc_flameweaver_verathix_125314AI(p_Creature);
        }
};

/// Baldrazar - 125666
class npc_baldrazar_125666 : public CreatureScript
{
    public:
        npc_baldrazar_125666() : CreatureScript("npc_baldrazar_125666") { }

        struct npc_baldrazar_125666AI : public BossAI
        {
            npc_baldrazar_125666AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Conversation = 254531,
                Spell_BreathOfShadows = 219960,
                Spell_ShadowBoltVolley = 249464,
                Spell_GrippingShadows = 249465
            };

            enum eEvents
            {
                Event_CastBreathOfShadows = 1,
                Event_CastShadowBoltVolley = 2,
                Event_GrippingShadows = 3
            };

            EventMap m_Events;

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void InitializeAI() override
            {
                DoCast(eSpells::Spell_Conversation, true);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                }, 4000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastBreathOfShadows, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastShadowBoltVolley, 12000);
                m_Events.ScheduleEvent(eEvents::Event_GrippingShadows, 23000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastBreathOfShadows:
                {
                    me->AttackStop();
                    me->SetFacingTo(me->GetOrientation());
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    DoCast(eSpells::Spell_BreathOfShadows);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    }, 6000);

                    m_Events.ScheduleEvent(eEvents::Event_CastBreathOfShadows, 33000);
                    break;
                }
                case eEvents::Event_CastShadowBoltVolley:
                {
                    DoCast(eSpells::Spell_ShadowBoltVolley);
                    m_Events.ScheduleEvent(eEvents::Event_CastShadowBoltVolley, urand(7000, 26000));
                    break;
                }
                case eEvents::Event_GrippingShadows:
                {
                    DoCast(eSpells::Spell_GrippingShadows);
                    m_Events.ScheduleEvent(eEvents::Event_GrippingShadows, 41000);
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
            return new npc_baldrazar_125666AI(p_Creature);
        }
};

/// Gorgoloth - 125148
class npc_gorgoloth_125148 : public CreatureScript
{
public:
    npc_gorgoloth_125148() : CreatureScript("npc_gorgoloth_125148") { }

    struct npc_gorgoloth_125148AI : public BossAI
    {
        npc_gorgoloth_125148AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

        enum eSpells
        {
            Spell_FelBreath = 248641,
            Spell_CreepingDoom = 248508,
        };

        enum eEvents
        {
            Event_CastFelBreath = 1,
            Event_CastCreepingDoom = 2
        };

        EventMap m_Events;

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
        {
            if (Map* l_Map = me->GetMap())
            {
                if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                {
                    l_Map->CloseMap();
                }
            }
        }

        void JustSummoned(Creature* p_Summon) override
        {
            summons.Summon(p_Summon);
        }

        void InitializeAI() override
        {
            me->AddDelayedEvent([this]() -> void
            {
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
            }, 4000);
        }

        void EnterEvadeMode() override
        {
            BossAI::EnterEvadeMode();
            m_Events.Reset();
            me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);

            if (Map* l_Map = me->GetMap())
            {
                if (l_Map->IsClosed())
                {
                    l_Map->OpenMap();
                }
            }
        }

        void JustDied(Unit* /*p_Killer*/) override
        {
            summons.DespawnAll();
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            m_Events.ScheduleEvent(eEvents::Event_CastFelBreath, 6000);
            m_Events.ScheduleEvent(eEvents::Event_CastCreepingDoom, 12000);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            m_Events.Update(p_Diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (m_Events.ExecuteEvent())
            {
                case eEvents::Event_CastFelBreath:
                {
                    DoCast(eSpells::Spell_FelBreath);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelBreath, 12000);
                    break;
                }
                case eEvents::Event_CastCreepingDoom:
                {
                    DoCast(eSpells::Spell_CreepingDoom);
                    m_Events.ScheduleEvent(eEvents::Event_CastCreepingDoom, 30000);
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
        return new npc_gorgoloth_125148AI(p_Creature);
    }
};

/// Inquisitor Meto - 124592
class npc_inquisitor_meto_124592 : public CreatureScript
{
    public:
        npc_inquisitor_meto_124592() : CreatureScript("npc_inquisitor_meto_124592") { }

        struct npc_inquisitor_meto_124592AI : public BossAI
        {
            npc_inquisitor_meto_124592AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_Sow = 247495,
                Spell_Reap = 247492,
                Spell_DeathField = 247632,
                Spell_SeedsOfChaos = 247585,
                Spell_BonusRollPrompt = 254435
            };

            enum eEvents
            {
                Event_CastSow = 1,
                Event_CastReap = 2,
                Event_CastDeathField = 3,
                Event_CastSeedsOfChaos = 4
            };

            EventMap m_Events;

            bool GetHealthModPersonal(float& p_Modifier, uint32 p_Count) override
            {
                if (p_Count > 10)
                {
                    p_Count -= 10;
                    p_Modifier = g_HealthScalingMod;
                }

                return true;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void Reset() override
            {
                m_Events.Reset();
                me->RemoveAllAreasTrigger();
                me->SetBonusLootSpell(eSpells::Spell_BonusRollPrompt);

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_CastSow, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastReap, 13000);
                m_Events.ScheduleEvent(eEvents::Event_CastDeathField, 15000);
                m_Events.ScheduleEvent(eEvents::Event_CastSeedsOfChaos, 33000);
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(1);
                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastSow:
                {
                    DoCast(eSpells::Spell_Sow);
                    m_Events.ScheduleEvent(eEvents::Event_CastSow, urand(13000, 22000));
                    break;
                }
                case eEvents::Event_CastReap:
                {
                    DoCast(eSpells::Spell_Reap);
                    m_Events.ScheduleEvent(eEvents::Event_CastReap, urand(21000, 25000));
                    break;
                }
                case eEvents::Event_CastDeathField:
                {
                    DoCast(eSpells::Spell_DeathField);
                    m_Events.ScheduleEvent(eEvents::Event_CastDeathField, urand(13000, 16000));
                    break;
                }
                case eEvents::Event_CastSeedsOfChaos:
                {
                    DoCast(eSpells::Spell_SeedsOfChaos);
                    m_Events.ScheduleEvent(eEvents::Event_CastSeedsOfChaos, urand(30000, 33000));
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
            return new npc_inquisitor_meto_124592AI(p_Creature);
        }
};

/// Occularus - 124492
class npc_occularus_124492 : public CreatureScript
{
    public:
        npc_occularus_124492() : CreatureScript("npc_occularus_124492") { }

        struct npc_occularus_124492AI : public BossAI
        {
            npc_occularus_124492AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_SearingGaze = 247320,
                Spell_Lash = 247325,
                Spell_GushingWound = 247318,
                Spell_EyeSore = 247332,
                Spell_Phantasm = 247393,
                Spell_BonusRollPrompt = 254443
            };

            enum eEvents
            {
                Event_CastSearingGaze = 1,
                Event_CastLash = 2,
                Event_CastGushingWound = 3,
                Event_CastEyeSore = 4,
                Event_CastPhantasm = 5
            };

            EventMap m_Events;

            bool GetHealthModPersonal(float& p_Modifier, uint32 p_Count) override
            {
                if (p_Count > 10)
                {
                    p_Count -= 10;
                    p_Modifier = g_HealthScalingMod;
                }

                return true;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (Map* l_Map = me->GetMap())
                {
                    if (me->GetHealthPct() <= 25 && !l_Map->IsClosed())
                    {
                        l_Map->CloseMap();
                    }
                }
            }

            void Reset() override
            {
                m_Events.Reset();
                SetCombatMovement(false);
                me->SetBonusLootSpell(eSpells::Spell_BonusRollPrompt);
                me->RemoveAllAreasTrigger();

                if (Map* l_Map = me->GetMap())
                {
                    if (l_Map->IsClosed())
                    {
                        l_Map->OpenMap();
                    }
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_CastSearingGaze, 9000);
                m_Events.ScheduleEvent(eEvents::Event_CastLash, 15000);
                m_Events.ScheduleEvent(eEvents::Event_CastGushingWound, 23000);
                m_Events.ScheduleEvent(eEvents::Event_CastEyeSore, 24000);
                m_Events.ScheduleEvent(eEvents::Event_CastPhantasm, 38000);
            }

            void JustDied(Unit* p_Killer) override
            {
                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastSearingGaze:
                {
                    DoCastVictim(eSpells::Spell_SearingGaze);
                    m_Events.ScheduleEvent(eEvents::Event_CastSearingGaze, urand(8000, 14000));
                    break;
                }
                case eEvents::Event_CastLash:
                {
                    DoCastVictim(eSpells::Spell_Lash);
                    m_Events.ScheduleEvent(eEvents::Event_CastLash, urand(16000, 24000));
                    break;
                }
                case eEvents::Event_CastGushingWound:
                {
                    DoCastVictim(eSpells::Spell_GushingWound);
                    m_Events.ScheduleEvent(eEvents::Event_CastGushingWound, 8000);
                    break;
                }
                case eEvents::Event_CastEyeSore:
                {
                    DoCast(eSpells::Spell_EyeSore);
                    m_Events.ScheduleEvent(eEvents::Event_CastEyeSore, urand(21000, 26000));
                    break;
                }
                case eEvents::Event_CastPhantasm:
                {
                    DoCast(eSpells::Spell_Phantasm);
                    m_Events.ScheduleEvent(eEvents::Event_CastPhantasm, urand(33000, 40000));
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
            return new npc_occularus_124492AI(p_Creature);
        }
};

/// Custom Brazier\'s Warmth Areatrigger - 251547
class at_argus_braziers_warmth : public AreaTriggerEntityScript
{
    public:
        at_argus_braziers_warmth() : AreaTriggerEntityScript("at_argus_braziers_warmth") { }

        enum eSpells
        {
            Spell_BraziersWarmth = 251547,
            Spell_CripplingCold = 251563
        };

        bool OnAddTarget(AreaTrigger* /*p_Areatrigger*/, Unit* p_Target) override
        {
            p_Target->RemoveAurasDueToSpell(eSpells::Spell_CripplingCold);
            p_Target->CastSpell(p_Target, eSpells::Spell_BraziersWarmth, true);
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (ZoneScript* l_ZoneScript = p_Areatrigger->GetZoneScript())
            {
                if (l_ZoneScript->GetData(eGlobalDatas::Data_CurrentModifier) == InvasionPointVal::eModifiers::Mod_CripplingCold)
                {
                    if (p_Target->ToPlayer())
                    {
                        p_Target->CastSpell(p_Target, eSpells::Spell_CripplingCold, true);
                    }
                }
            }

            p_Target->RemoveAura(eSpells::Spell_BraziersWarmth);
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_braziers_warmth();
        }
};

/// Called by Fel Spike - 248702
class at_argus_fel_spike : public AreaTriggerEntityScript
{
    public:
        at_argus_fel_spike() : AreaTriggerEntityScript("at_argus_fel_spike") {}

        enum eSpells
        {
            Spell_FelLightning = 248700
        };

        enum eGobs
        {
            Gob_FelSpike = 216056
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<GameObject*> l_GobList;
            p_AreaTrigger->GetGameObjectListWithEntryInGrid(l_GobList, eGobs::Gob_FelSpike, 5.0f);

            if (l_GobList.empty())
                return;

            for (auto l_Itr : l_GobList)
            {
                if (l_Itr->GetOwner() == l_Caster)
                {
                    l_Itr->Delete();
                }
            }
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer >= p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }
            else
            {
                m_UpdateTimer = 2 * IN_MILLISECONDS; // Maybe need to reduce wait time?
            }

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();

            JadeCore::NearestAttackableUnitInPositionRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            for (auto l_Itr : l_TargetList)
            {
                if (l_Caster->IsValidAttackTarget(l_Itr) && l_Itr->ToPlayer())
                {
                    l_Caster->CastSpell(l_Itr, eSpells::Spell_FelLightning, true);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_fel_spike();
        }

    private:
        uint32 m_UpdateTimer = 2 * IN_MILLISECONDS;
};

/// Called by Wake of Destruction - 247440, 247419
class at_argus_wake_of_destruction : public AreaTriggerEntityScript
{
    public:
        at_argus_wake_of_destruction() : AreaTriggerEntityScript("at_argus_wake_of_destruction") { }

        enum eSpells
        {
            Spell_WakeOfDestructionMany = 247440,
            Spell_WakeOfDestructionDamage = 254201
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (!p_Target->ToPlayer())
                return false;

            if (p_Areatrigger->GetSpellId() == eSpells::Spell_WakeOfDestructionMany)
            {
                Unit* l_Caster = sObjectAccessor->GetUnit(*p_Areatrigger, p_Areatrigger->GetAreatriggerSpellCasterGuid());
                Unit* l_OriginalCaster = p_Areatrigger->GetCaster();
                if (!l_Caster || !l_OriginalCaster)
                    return false;

                if (p_Target != l_Caster)
                {
                    l_OriginalCaster->CastSpell(p_Target, eSpells::Spell_WakeOfDestructionDamage, true);
                    p_Areatrigger->Remove();
                }
            }
            else
            {
                Unit* l_Caster = p_Areatrigger->GetCaster();
                if (!l_Caster)
                    return false;

                l_Caster->CastSpell(p_Target, eSpells::Spell_WakeOfDestructionDamage, true);
                p_Areatrigger->Remove();
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_wake_of_destruction();
        }
};

/// Called by Energized - 252171
class at_argus_energized : public AreaTriggerEntityScript
{
    public:
        at_argus_energized() : AreaTriggerEntityScript("at_argus_energized") { }

        enum eSpells
        {
            Spell_VolatileReactionDummy = 252172,
            Spell_SporeBurst = 251165,
            Spell_EmbeddedSpores = 251123
        };

        enum eNpcs
        {
            Npc_SporeStalk = 126566
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Owner = p_Areatrigger->GetCaster();
            if (!l_Owner || p_Target->GetEntry() != eNpcs::Npc_SporeStalk)
                return false;

            l_Owner->CastSpell(p_Target, eSpells::Spell_VolatileReactionDummy, true);
            p_Target->CastSpell(l_Owner, eSpells::Spell_SporeBurst, true);
            l_Owner->CastSpell(l_Owner, eSpells::Spell_EmbeddedSpores, true);
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_energized();
        }
};

/// Called by Fiery Release - 251891
class at_argus_fiery_release : public AreaTriggerEntityScript
{
    public:
        at_argus_fiery_release() : AreaTriggerEntityScript("at_argus_fiery_release") { }

        enum eSpells
        {
            Spell_FieryRelease = 251890
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::Spell_FieryRelease, true);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_fiery_release();
        }
};

/// Called by Temporal Precognition - 252488
class at_argus_temporal_precognition : public AreaTriggerEntityScript
{
    public:
        at_argus_temporal_precognition() : AreaTriggerEntityScript("at_argus_temporal_precognition") { }

        enum eSpells
        {
            Spell_Synchronicity = 252490
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Owner = p_Areatrigger->GetCaster();
            if (!l_Owner || !l_Owner->ToCreature())
                return false;

            Unit* l_PlayerOwner = l_Owner->GetAnyOwner();
            if (!l_PlayerOwner)
                return false;

            if (l_PlayerOwner != p_Target)
                return false;

            l_Owner->CastSpell(l_Owner, eSpells::Spell_Synchronicity, true);
            p_Areatrigger->Remove();
            l_Owner->ToCreature()->DespawnOrUnsummon();
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_temporal_precognition();
        }
};

/// Called by Sacred Vines - 252086
class at_argus_sacred_vines : public AreaTriggerEntityScript
{
    public:
        at_argus_sacred_vines() : AreaTriggerEntityScript("at_argus_sacred_vines") { }

        enum eSpells
        {
            Spell_SacredVines = 252093,
            Spell_SacredVinesAggro = 252085
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Owner = p_Areatrigger->GetCaster();
            if (!l_Owner || !l_Owner->ToCreature() || !p_Target->ToPlayer())
                return false;

            Unit* l_PlayerOwner = l_Owner->GetAnyOwner();
            if (!l_PlayerOwner)
                return false;

            p_Target->CastSpell(p_Target, eSpells::Spell_SacredVines, true);
            p_Areatrigger->Remove();
            l_PlayerOwner->RemoveAura(eSpells::Spell_SacredVinesAggro, l_Owner->GetGUID());
            l_Owner->ToCreature()->DespawnOrUnsummon();
            return false;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Owner = p_AreaTrigger->GetCaster();
            if (!l_Owner || !l_Owner->ToCreature())
                return;

            Unit* l_PlayerOwner = l_Owner->GetAnyOwner();
            if (!l_PlayerOwner)
                return;

            l_PlayerOwner->RemoveAura(eSpells::Spell_SacredVinesAggro, l_Owner->GetGUID());
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_sacred_vines();
        }
};

/// Called by Flame Fissure - 248771
class at_argus_flame_fissure : public AreaTriggerEntityScript
{
    public:
        at_argus_flame_fissure() : AreaTriggerEntityScript("at_argus_flame_fissure") {}

        enum eSpells
        {
            Spell_FlameFissureDamage = 248770
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer >= p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }
            else
            {
                m_UpdateTimer = 1 * IN_MILLISECONDS;
            }

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();

            JadeCore::NearestAttackableUnitInPositionRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            for (auto l_Itr : l_TargetList)
            {
                if (l_Caster->IsValidAttackTarget(l_Itr) && l_Itr->ToPlayer())
                {
                    l_Caster->CastSpell(l_Itr, eSpells::Spell_FlameFissureDamage, true);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_flame_fissure();
        }

    private:
        uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
};

/// Called by Pool of Shadows - 249469
class at_argus_pool_of_shadows : public AreaTriggerEntityScript
{
    public:
        at_argus_pool_of_shadows() : AreaTriggerEntityScript("at_argus_pool_of_shadows") { }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer >= p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }
            else
            {
                m_UpdateTimer = 0.5 * IN_MILLISECONDS;
            }

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();

            JadeCore::NearestAttackableUnitInPositionRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            for (auto l_Itr : l_TargetList)
            {
                if (l_Caster->IsValidAttackTarget(l_Itr) && l_Itr->ToPlayer())
                {
                    l_Itr->KnockbackFrom(l_Caster->GetPosition(), 20.0f, 10.0f, l_Caster);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_pool_of_shadows();
        }

    private:
        uint32 m_UpdateTimer = 0.5 * IN_MILLISECONDS;
};

/// Called by Seeds of Chaos - 247588
class at_argus_seeds_of_chaos : public AreaTriggerEntityScript
{
    public:
        at_argus_seeds_of_chaos() : AreaTriggerEntityScript("at_argus_seeds_of_chaos") { }

        enum eSpells
        {
            Spell_SeedsOfChaosPeriodic = 247592,
            Spell_SeedsOfChaosDamage = 247589
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Owner = p_Areatrigger->GetCaster();
            if (!l_Owner || !p_Target->ToPlayer())
                return false;

            l_Owner->CastSpell(p_Target, eSpells::Spell_SeedsOfChaosPeriodic, true);
            m_Increase = false;
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Owner = p_Areatrigger->GetCaster();
            if (!l_Owner || !p_Target->ToPlayer())
                return false;

            p_Target->RemoveAura(eSpells::Spell_SeedsOfChaosPeriodic);
            m_Increase = true;
            return false;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Owner = p_AreaTrigger->GetCaster();
            if (!l_Owner)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Spell_SeedsOfChaosDamage);
            if (!l_SpellInfo)
                return;

            if (!p_AreaTrigger->IsOnDespawn())
            {
                float l_Pct = (100.0f / 5.0f) * p_AreaTrigger->GetRadius();
                int32 l_BaseDamage = l_SpellInfo->Effects[EFFECT_0].CalcValue(l_Owner);
                int32 l_Damage = CalculatePct(l_BaseDamage, l_Pct);

                l_Owner->CastCustomSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::Spell_SeedsOfChaosDamage, &l_Damage, nullptr, nullptr, true);
            }
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer >= p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }
            else
            {
                m_UpdateTimer = 0.5 * IN_MILLISECONDS;
            }

            float l_Param = m_Increase ? 6 : -10;
            float l_BaseScale = 3.0f;
            float l_Scale = p_AreaTrigger->GetRadius();
            float l_PctPerMSec = l_Param / 1000.0f;

            float l_Pct = (l_Scale / l_BaseScale) + (500 * l_PctPerMSec / 100.0f);

            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, p_AreaTrigger->GetLiveTime());
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, l_Scale / l_BaseScale);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Pct);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 500);

            p_AreaTrigger->SetRadius(l_BaseScale * l_Pct);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_seeds_of_chaos();
        }

    private:
        uint32 m_UpdateTimer = 0.5 * IN_MILLISECONDS;
        bool m_Increase = true;
};

/// Called by Searing Rune - 251915
class at_argus_searing_rune : public AreaTriggerEntityScript
{
    public:
        at_argus_searing_rune() : AreaTriggerEntityScript("at_argus_searing_rune") { }

        enum eSpells
        {
            Spell_SearingRuneDamage = 251913,
            Spell_SearingRuneBuff = 251914
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (!p_Target->ToPlayer())
                return false;

            p_Target->CastSpell(p_Target, eSpells::Spell_SearingRuneDamage, true);

            if (!p_Target->HasAura(eSpells::Spell_SearingRuneBuff))
            {
                p_Target->CastSpell(p_Target, eSpells::Spell_SearingRuneBuff, true);
            }

            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (!p_Target->ToPlayer())
                return false;

            p_Target->RemoveAura(eSpells::Spell_SearingRuneDamage);
            return false;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer >= p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }
            else
            {
                m_UpdateTimer = 1 * IN_MILLISECONDS;
            }

            std::list<Player*> l_PlayerList;
            JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, p_AreaTrigger->GetRadius());
            JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
            p_AreaTrigger->VisitNearbyObject(p_AreaTrigger->GetRadius(), l_Searcher);

            if (l_PlayerList.empty())
                return;

            for (auto l_Itr : l_PlayerList)
            {
                if (Aura* l_Aura = l_Itr->GetAura(eSpells::Spell_SearingRuneBuff))
                {
                    l_Aura->SetDuration((l_Aura->GetDuration() + 20000));
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_argus_searing_rune();
        }

    private:
        uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
};

/// Chaos Wave - 248650
class spell_argus_chaos_wave : public SpellScriptLoader
{
    public:
        spell_argus_chaos_wave() : SpellScriptLoader("spell_argus_chaos_wave") { }

        class spell_argus_chaos_wave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_chaos_wave_SpellScript);

            enum eSpells
            {
                Spell_ChaosWaveAreatriggerKazral = 217958,
                Spell_ChaosWaveAreatriggerZakgal = 248663
            };

            enum eNpcs
            {
                Npc_DreadKnightZakgal = 125252
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);

                if (l_Caster->GetEntry() == eNpcs::Npc_DreadKnightZakgal)
                {
                    l_Caster->CastSpell(l_Target, eSpells::Spell_ChaosWaveAreatriggerZakgal);
                }
                else
                {
                    l_Caster->CastSpell(l_Target, eSpells::Spell_ChaosWaveAreatriggerZakgal);
                }

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 2000);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_chaos_wave_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_chaos_wave_SpellScript();
        }
};

/// Fel Spike - 248697
class spell_argus_fel_spike : public SpellScriptLoader
{
    public:
        spell_argus_fel_spike() : SpellScriptLoader("spell_argus_fel_spike") { }

        class spell_argus_fel_spike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_fel_spike_SpellScript);

            enum eSpells
            {
                Spell_FelSpikeSummon = 248696
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;


                l_Caster->CastSpell(l_Target, eSpells::Spell_FelSpikeSummon);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_fel_spike_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_fel_spike_SpellScript();
        }
};

/// Enter Rift - 247069
class spell_argus_enter_rift : public SpellScriptLoader
{
    public:
        spell_argus_enter_rift() : SpellScriptLoader("spell_argus_enter_rift") { }

        class spell_argus_enter_rift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_enter_rift_SpellScript);

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToPlayer() || !l_Target)
                    return;

                switch (l_Target->GetEntry())
                {
                case eGlobalNpcs::Npc_InvasionPointExit:
                {
                    bool l_ExitPosFound = false;

                    for (uint32 l_POIId : sPOIMgr->GetActivePOIIds())
                    {
                        if (IsInvasionPointPOI(l_POIId))
                        {
                            if (g_InvasionPointDatas[l_POIId]->ScenarioZoneId == l_Caster->GetZoneId())
                            {
                                l_Caster->ToPlayer()->TeleportTo(1669, g_InvasionPointDatas[l_POIId]->PortalSpawnPosition);
                                l_Caster->Dismount();
                                l_ExitPosFound = true;
                                break;
                            }
                        }
                    }

                    if (!l_ExitPosFound)
                    {
                        for (auto l_Itr : g_InvasionPointDatas)
                        {
                            if (l_Caster->GetZoneId() == l_Itr.second->ScenarioZoneId)
                            {
                                l_Caster->Dismount();
                                l_Caster->ToPlayer()->TeleportTo(1669, l_Itr.second->PortalSpawnPosition);
                                break;
                            }
                        }
                    }

                    break;
                }
                default:
                {
                    for (uint32 l_POIId : sPOIMgr->GetActivePOIIds())
                    {
                        if (IsInvasionPointPOI(l_POIId))
                        {
                            if (g_InvasionPointDatas[l_POIId]->PortalEntry == l_Target->GetEntry())
                            {
                                l_Caster->CastSpell(l_Caster, g_InvasionPointDatas[l_POIId]->TeleportSpellId, true);
                                l_Caster->Dismount();
                                break;
                            }
                        }
                    }

                    break;
                }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_enter_rift_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_enter_rift_SpellScript();
        }
};

/// Power Dilation - 250244
class spell_argus_power_dilation : public SpellScriptLoader
{
    public:
        spell_argus_power_dilation() : SpellScriptLoader("spell_argus_power_dilation") { }

        class  spell_argus_power_dilation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_power_dilation_AuraScript);

            bool m_SurgingPowerCanBeApplied = true;
            bool m_SurgingPowerApplied = false;
            bool m_SurgingPowerCanBeIncreased = false;
            bool m_WaningPowerCanBeApplied = false;
            bool m_WaningPowerApplied = false;
            bool m_WaningPowerCanBeIncreased = false;

            enum eSpells
            {
                Spell_SurgingPower = 250245,
                Spell_WaningPower = 250246
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::Spell_SurgingPower) && !l_Caster->HasAura(eSpells::Spell_WaningPower))
                {
                    if (m_SurgingPowerCanBeApplied)
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_SurgingPower, true);
                        m_SurgingPowerApplied = true;
                        m_SurgingPowerCanBeIncreased = true;
                        m_SurgingPowerCanBeApplied = false;
                        return;
                    }
                }

                if (m_WaningPowerCanBeApplied)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_WaningPower, true);
                    m_WaningPowerApplied = true;
                    m_WaningPowerCanBeIncreased = true;
                    m_WaningPowerCanBeApplied = false;
                    return;
                }

                if (m_SurgingPowerApplied && m_SurgingPowerCanBeIncreased)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_SurgingPower, true);
                }

                if (m_WaningPowerApplied && m_WaningPowerCanBeIncreased)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_WaningPower, true);
                }

                if (Aura* l_SurgingPower = l_Caster->GetAura(eSpells::Spell_SurgingPower))
                {
                    if (l_SurgingPower->GetStackAmount() == 10 && m_SurgingPowerCanBeIncreased)
                    {
                        m_SurgingPowerCanBeIncreased = false;
                        return;
                    }

                    if (!m_SurgingPowerCanBeIncreased)
                    {
                        l_SurgingPower->ModStackAmount(-1);

                        if (!l_Caster->HasAura(eSpells::Spell_SurgingPower))
                        {
                            m_SurgingPowerApplied = false;
                            m_WaningPowerCanBeApplied = true;
                        }
                    }
                }

                if (Aura* l_WaningPower = l_Caster->GetAura(eSpells::Spell_WaningPower))
                {
                    if (l_WaningPower->GetStackAmount() == 10 && m_WaningPowerCanBeIncreased)
                    {
                        m_WaningPowerCanBeIncreased = false;
                        return;
                    }

                    if (!m_WaningPowerCanBeIncreased)
                    {
                        l_WaningPower->ModStackAmount(-1);

                        if (!l_Caster->HasAura(eSpells::Spell_WaningPower))
                        {
                            m_WaningPowerApplied = false;
                            m_SurgingPowerCanBeApplied = true;
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_power_dilation_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_power_dilation_AuraScript();
        }
};

/// Narcolepsy - 249257
class spell_argus_narcolepsy : public SpellScriptLoader
{
    public:
        spell_argus_narcolepsy() : SpellScriptLoader("spell_argus_narcolepsy") { }

        class  spell_argus_narcolepsy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_narcolepsy_AuraScript);

            enum eSpells
            {
                Spell_Sleep = 224090
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->IsMoving())
                {
                    l_Caster->CastSpell(l_Target, eSpells::Spell_Sleep, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_narcolepsy_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_narcolepsy_AuraScript();
        }
};

/// Pull Fire - 252241
class spell_argus_pull_fire : public SpellScriptLoader
{
    public:
        spell_argus_pull_fire() : SpellScriptLoader("spell_argus_pull_fire") { }

        class  spell_argus_pull_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_pull_fire_AuraScript);

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, p_AuraEffect->GetTriggerSpell(), true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_pull_fire_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_pull_fire_AuraScript();
        }
};

/// Burning Steps - 251896
class spell_argus_burning_steps : public SpellScriptLoader
{
    public:
        spell_argus_burning_steps() : SpellScriptLoader("spell_argus_burning_steps") { }

        class  spell_argus_burning_steps_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_burning_steps_AuraScript);

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->isInCombat())
                {
                    l_Caster->CastSpell(l_Caster, p_AuraEffect->GetTriggerSpell(), true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_burning_steps_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_burning_steps_AuraScript();
        }
};

/// Molten Boulder - 252731
class spell_argus_molten_boulder : public SpellScriptLoader
{
    public:
        spell_argus_molten_boulder() : SpellScriptLoader("spell_argus_molten_boulder") { }

        class spell_argus_molten_boulder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_molten_boulder_SpellScript);

            enum eSpells
            {
                Spell_MoltenBoulderMissile = 252733
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;


                l_Caster->CastSpell(l_Target, eSpells::Spell_MoltenBoulderMissile);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_molten_boulder_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_molten_boulder_SpellScript();
        }
};

/// Wave of Dread - 249246, 249250
class spell_argus_wave_of_dread : public SpellScriptLoader
{
    public:
        spell_argus_wave_of_dread() : SpellScriptLoader("spell_argus_wave_of_dread") { }

        class spell_argus_wave_of_dread_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_wave_of_dread_SpellScript);

            enum eSpells
            {
                Spell_WavesOfDreadPeriodic = 249250
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                l_Caster->CastSpell(l_Caster, eSpells::Spell_WavesOfDreadPeriodic);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_wave_of_dread_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_wave_of_dread_SpellScript();
        }

        class  spell_argus_wave_of_dread_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_wave_of_dread_AuraScript);

            enum eSpells
            {
                Spell_WaveOfDreadFear = 249248,
                Spell_WaveOfDreadVisual = 216919
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Spell_WaveOfDreadFear, true);
            }

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_DestPos = l_Caster->GetPosition();
                l_DestPos.SimplePosXYRelocationByAngle(l_DestPos, 50.0f, l_DestPos.GetOrientation(), true);

                uint16 l_Delay = 0;

                while (l_Delay <= 4000)
                {
                    Position l_RandPos = l_DestPos;
                    l_RandPos.SimplePosXYRelocationByAngle(l_RandPos, urand(0.0f, 10.0f), frand(0.0f, 2.0f * M_PI), true);

                    uint16 l_UrandDelay = urand(50, 300);

                    l_Caster->AddDelayedEvent([l_Caster, l_RandPos]() -> void
                    {
                        l_Caster->CastSpell(l_RandPos, eSpells::Spell_WaveOfDreadVisual, true);
                    }, l_Delay);

                    l_Delay += l_UrandDelay;
                }
            }

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_wave_of_dread_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectApply += AuraEffectApplyFn(spell_argus_wave_of_dread_AuraScript::HandleAfterApply, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_wave_of_dread_AuraScript::HandleAfterRemove, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_wave_of_dread_AuraScript();
        }
};

/// Obliteration Beam - 250236, 250242
class spell_argus_obliteration_beam : public SpellScriptLoader
{
    public:
        spell_argus_obliteration_beam() : SpellScriptLoader("spell_argus_obliteration_beam") { }

        class spell_argus_obliteration_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_obliteration_beam_SpellScript);

            enum eSpells
            {
                Spell_ObliterationBeamArea = 250236,
                Spell_ObliterationBeamSummon = 250239
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!p_Targets.empty())
                {
                    p_Targets.resize(1);
                }
            }

            void HandleHitTargetArea(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_ObliterationBeamSummon, true);
            }

            void HandleHitTargetText(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->ToCreature()->AI()->Talk(0, l_Target->GetGUID());
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Spell_ObliterationBeamArea)
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_obliteration_beam_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                    OnEffectHitTarget += SpellEffectFn(spell_argus_obliteration_beam_SpellScript::HandleHitTargetArea, EFFECT_0, SPELL_EFFECT_DUMMY);
                }
                else
                    OnEffectHitTarget += SpellEffectFn(spell_argus_obliteration_beam_SpellScript::HandleHitTargetText, EFFECT_2, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_obliteration_beam_SpellScript();
        }
};

/// Something Stirs - 250688
class spell_argus_something_stirs : public SpellScriptLoader
{
    public:
        spell_argus_something_stirs() : SpellScriptLoader("spell_argus_something_stirs") { }

        class spell_argus_something_stirs_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_something_stirs_SpellScript);

            enum eSpells
            {
                Spell_SomethingStirsSummon = 250690
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Map* l_Map = l_Caster->GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Scenario)
                    return;

                if (l_Scenario->GetCurrentStep() == InvasionPointSangua::eSteps::Step_StemTheBleeding && !l_Scenario->IsCompleted())
                {
                    if (!p_Targets.empty())
                    {
                        p_Targets.clear();
                    }
                }
                else
                {
                    p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                    {
                        if (Player* l_Player = p_Target->ToPlayer())
                        {
                            if (l_Player->isInCombat())
                            {
                                return false;
                            }
                        }

                        return true;
                    });

                    if (!p_Targets.empty())
                    {
                        p_Targets.resize(1);
                    }
                }
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_SomethingStirsSummon, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_something_stirs_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_something_stirs_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_something_stirs_SpellScript();
        }
};

/// Alert - 250221
class spell_argus_alert : public SpellScriptLoader
{
    public:
        spell_argus_alert() : SpellScriptLoader("spell_argus_alert") { }

        class spell_argus_alert_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_alert_SpellScript);

            enum eSpells
            {
                Spell_SomethingStirsSummon = 250690
            };

            enum ePoints
            {
                Point_AssisMoveEnd = 1
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                {
                    if (Creature* l_Creature = p_Target->ToCreature())
                    {
                        if (!l_Creature->isInCombat())
                        {
                            return false;
                        }
                    }

                    return true;
                });
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Position l_MovePos = l_Target->GetPosition();
                l_MovePos.SimplePosXYRelocationByAngle(l_MovePos, l_Target->GetDistance2d(l_Caster) - 2.0f, l_Target->GetAngle(l_Caster), true);
                l_Target->GetMotionMaster()->MovePoint(ePoints::Point_AssisMoveEnd, l_MovePos);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_alert_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_alert_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_alert_SpellScript();
        }
};

/// Crush - 250685
class spell_argus_crush : public SpellScriptLoader
{
    public:
        spell_argus_crush() : SpellScriptLoader("spell_argus_crush") { }

        class spell_argus_crush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_crush_SpellScript);

            enum eSpells
            {
                Spell_CrushCone = 250686
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                l_Caster->CastSpell(l_Caster, eSpells::Spell_CrushCone);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 4000);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_crush_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_crush_SpellScript();
        }
};

/// Venom Storm - 239265, 239266
class spell_argus_venom_storm : public SpellScriptLoader
{
    public:
        spell_argus_venom_storm() : SpellScriptLoader("spell_argus_venom_storm") { }

        class spell_argus_venom_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_venom_storm_SpellScript);

            enum eSpells
            {
                Spell_VenomStormCone = 239266,
                Spell_VenomStormConeVisual = 239269
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                l_Caster->CastSpell(l_Target, eSpells::Spell_VenomStormCone);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 3000);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position const l_CasterPos = l_Caster->GetPosition();
                float l_Angle = l_Caster->GetOrientation() - 0.5f;
                float l_AddToAngleCount = 1.0f / 13.0f;

                for (uint8 i = 0; i < 13; i++)
                {
                    Position l_DestPos = l_CasterPos;
                    l_DestPos.SimplePosXYRelocationByAngle(l_DestPos, 30.0f, l_Angle, true);
                    l_Caster->CastSpell(l_DestPos, eSpells::Spell_VenomStormConeVisual, true);
                    l_Angle += l_AddToAngleCount;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_venom_storm_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);

                if (m_scriptSpellId == eSpells::Spell_VenomStormCone)
                    AfterCast += SpellCastFn(spell_argus_venom_storm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_venom_storm_SpellScript();
        }
};

/// Blood Bags - 250154
class spell_argus_blood_bags : public SpellScriptLoader
{
    public:
        spell_argus_blood_bags() : SpellScriptLoader("spell_argus_blood_bags") { }

        class spell_argus_blood_bags_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_blood_bags_AuraScript);

            enum eSpells
            {
                Spell_DropBloodBag = 250308
            };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_DropBloodBag, true);
                }

                if (l_Caster->isSummon())
                {
                    l_Caster->ToCreature()->DespawnOrUnsummon(5000);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_blood_bags_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_blood_bags_AuraScript();
        }
};

/// Seed of Destruction - 247437
class spell_argus_seed_of_destruction : public SpellScriptLoader
{
    public:
        spell_argus_seed_of_destruction() : SpellScriptLoader("spell_argus_seed_of_destruction") { }

        class spell_argus_seed_of_destruction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_seed_of_destruction_AuraScript);

            enum eSpells
            {
                Spell_SeedOfDestruction = 247440
            };

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->ToCreature()->AI()->Talk(1, l_Target->GetGUID());
            }

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Position const l_TargetPos = l_Target->GetPosition();
                std::vector<float> const l_Orientations =
                {
                    0.7853982f,
                    1.570796f,
                    2.356194f,
                    3.141593f,
                    3.926991f,
                    4.712389f,
                    5.497787f,
                    0.0f
                };

                for (auto l_Itr : l_Orientations)
                {
                    Position l_DestPos = l_TargetPos;
                    l_DestPos.m_orientation = l_Itr;

                    l_Target->CastSpell(l_DestPos, eSpells::Spell_SeedOfDestruction, true, nullptr, nullptr, l_Caster->GetGUID());
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_argus_seed_of_destruction_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_seed_of_destruction_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_seed_of_destruction_AuraScript();
        }
};

/// Soul Cleave - 247410
class spell_argus_soul_cleave : public SpellScriptLoader
{
    public:
        spell_argus_soul_cleave() : SpellScriptLoader("spell_argus_soul_cleave") { }

        class spell_argus_soul_cleave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_soul_cleave_SpellScript);

            enum eSpells
            {
                Spell_ClovenSoul = 247444
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_TargetsSize = p_Targets.size();
            }

            void HandleHitTargetAura(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_ClovenSoul, true);
            }

            void HandleHitTargetDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !m_TargetsSize)
                    return;

                SetHitDamage(GetHitDamage() / m_TargetsSize);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_soul_cleave_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_soul_cleave_SpellScript::HandleHitTargetAura, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_soul_cleave_SpellScript::HandleHitTargetDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }

        private:
            uint8 m_TargetsSize = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_soul_cleave_SpellScript();
        }
};

/// Cavitation - 247415, 247417
class spell_argus_cavitation : public SpellScriptLoader
{
    public:
        spell_argus_cavitation() : SpellScriptLoader("spell_argus_cavitation") { }

        class spell_argus_cavitation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_cavitation_SpellScript);

            enum eSpells
            {
                Spell_CavitationTalk = 247415,
                Spell_WakeOfDestruction = 247419
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!p_Targets.empty())
                {
                    p_Targets.resize(1);
                }
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 2000);
            }

            void HandleAfterCastTalk()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->ToCreature()->AI()->Talk(0);
            }

            void HandleAfterCastAreatrigger()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Spell_WakeOfDestruction, true);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Spell_CavitationTalk)
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_cavitation_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                    OnEffectHitTarget += SpellEffectFn(spell_argus_cavitation_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
                    AfterCast += SpellCastFn(spell_argus_cavitation_SpellScript::HandleAfterCastTalk);
                }
                else
                    AfterCast += SpellCastFn(spell_argus_cavitation_SpellScript::HandleAfterCastAreatrigger);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_cavitation_SpellScript();
        }
};

/// Beguiling Charm - 247549, 247551
class spell_argus_beguiling_charm : public SpellScriptLoader
{
    public:
        spell_argus_beguiling_charm() : SpellScriptLoader("spell_argus_beguiling_charm") { }

        class spell_argus_beguiling_charm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_beguiling_charm_SpellScript);

            enum eSpells
            {
                Spell_BeguilingCharm = 247551
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->HasInArc(M_PI, l_Caster))
                        return true;

                    return false;
                });
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_BeguilingCharm, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_beguiling_charm_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_beguiling_charm_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_beguiling_charm_SpellScript();
        }

        class spell_argus_beguiling_charm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_beguiling_charm_AuraScript);

            void HandleOnEffectAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& /*p_DmgInfo*/, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                p_AbsorbAmount = 0;

                if (l_Target->HealthBelowPct(51))
                {
                    Remove();
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_argus_beguiling_charm_AuraScript::HandleOnEffectAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_beguiling_charm_AuraScript();
        }
};

/// Volatile Reaction - 252174
class spell_argus_volatile_reaction : public SpellScriptLoader
{
    public:
        spell_argus_volatile_reaction() : SpellScriptLoader("spell_argus_volatile_reaction") { }

        class spell_argus_volatile_reaction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_volatile_reaction_SpellScript);

            enum eSpells
            {
                Spell_EmbeddedSpores = 251123,
                Spell_VolatileReaction = 252185
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(eSpells::Spell_EmbeddedSpores))
                {
                    l_Caster->CastSpell(l_Target, eSpells::Spell_VolatileReaction, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_volatile_reaction_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_volatile_reaction_SpellScript();
        }
};

/// Dark Spores - 251380, 252134
class spell_argus_dark_spores : public SpellScriptLoader
{
    public:
        spell_argus_dark_spores() : SpellScriptLoader("spell_argus_dark_spores") { }

        class spell_argus_dark_spores_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_dark_spores_SpellScript);

            enum eSpells
            {
                Spell_DarkSporesMissiles = 251379,
                Spell_DarkSporesMain = 251380
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint8 l_I = 0; l_I < 33; l_I++)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_DarkSporesMissiles, true);
                }
            }

            void HandleLaunch(SpellEffIndex /*p_EffIndex*/)
            {
                PreventHitDefaultEffect(EFFECT_0);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_DestPos = l_Caster->GetPosition();
                l_DestPos.m_positionZ = l_Caster->GetMap()->GetHeight(l_DestPos.m_positionX, l_DestPos.m_positionY, l_DestPos.m_positionZ);
                l_DestPos.SimplePosXYRelocationByAngle(l_DestPos, 15.0f, frand(0.0f, M_PI * 2.0f), true);
                WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest());
                l_Dest->Relocate(l_DestPos);

                SetExplTargetDest(*l_Dest);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Spell_DarkSporesMain)
                    AfterCast += SpellCastFn(spell_argus_dark_spores_SpellScript::HandleAfterCast);
                else
                    OnEffectLaunch += SpellEffectFn(spell_argus_dark_spores_SpellScript::HandleLaunch, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_dark_spores_SpellScript();
        }
};

/// Beaming Gaze - 253994
class spell_argus_beaming_gaze : public SpellScriptLoader
{
    public:
        spell_argus_beaming_gaze() : SpellScriptLoader("spell_argus_beaming_gaze") { }

        class spell_argus_beaming_gaze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_beaming_gaze_SpellScript);

            enum eSpells
            {
                Spell_BeamingGazeSummon = 253993
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                Position l_TargetPos = l_Target->GetPosition();
                float l_Angle = 0.0f;

                for (uint8 l_I = 0; l_I < 3; l_I++)
                {
                    Position l_DestPos = l_TargetPos;
                    l_DestPos.SimplePosXYRelocationByAngle(l_DestPos, 10.0f, l_Angle, true);
                    l_DestPos.m_positionZ = l_Target->GetMap()->GetHeight(l_DestPos.m_positionX, l_DestPos.m_positionY, l_DestPos.m_positionZ) + 1.0f;

                    l_Target->CastSpell(l_DestPos, eSpells::Spell_BeamingGazeSummon, true);

                    l_Angle += 2.0f;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_beaming_gaze_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_beaming_gaze_SpellScript();
        }
};

/// Secrete Shadows - 249071
class spell_argus_secrete_shadows : public SpellScriptLoader
{
    public:
        spell_argus_secrete_shadows() : SpellScriptLoader("spell_argus_secrete_shadows") { }

        class  spell_argus_secrete_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_secrete_shadows_AuraScript);

            enum eSpells
            {
                Spell_SecreteShadowsMissiles = 219178
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::Spell_SecreteShadowsMissiles, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_secrete_shadows_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_secrete_shadows_AuraScript();
        }
};

/// Nausea - 247441
class spell_argus_nausea : public SpellScriptLoader
{
    public:
        spell_argus_nausea() : SpellScriptLoader("spell_argus_nausea") { }

        class  spell_argus_nausea_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_nausea_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                l_Caster->SetPower(Powers::POWER_ENERGY, l_Caster->GetPower(Powers::POWER_ENERGY) + 2);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_nausea_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_nausea_AuraScript();
        }
};

/// Grotesque Spawn - 247443
class spell_argus_grotesque_spawn : public SpellScriptLoader
{
    public:
        spell_argus_grotesque_spawn() : SpellScriptLoader("spell_argus_grotesque_spawn") { }

        class spell_argus_grotesque_spawn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_grotesque_spawn_SpellScript);

            enum eSpells
            {
                Spell_GrotesqueSpawnSummonTrickster = 247447,
                Spell_GrotesqueSpawnSummonManiac = 247497
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, RAND(eSpells::Spell_GrotesqueSpawnSummonTrickster, eSpells::Spell_GrotesqueSpawnSummonManiac), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_grotesque_spawn_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_grotesque_spawn_SpellScript();
        }
};

/// Slumbering Gasp - 247382
class spell_argus_slumbering_gasp : public SpellScriptLoader
{
    public:
        spell_argus_slumbering_gasp() : SpellScriptLoader("spell_argus_slumbering_gasp") { }

        class spell_argus_slumbering_gasp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_slumbering_gasp_SpellScript);

            enum eSpells
            {
                Spell_SlumberingGaspChanneling = 247379
            };

            enum eNpcs
            {
                Npc_SlumberingGasp = 124537
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);

                Position l_CasterPosition = l_Caster->GetPosition();
                Position l_SlumberingGaspSpawnPos = l_Caster->GetPosition();
                l_SlumberingGaspSpawnPos.SimplePosXYRelocationByAngle(l_SlumberingGaspSpawnPos, 10.0f, l_Caster->GetOrientation(), true);

                if (Creature* l_Creature = l_Caster->SummonCreature(eNpcs::Npc_SlumberingGasp, l_SlumberingGaspSpawnPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 20000))
                {
                    l_Creature->SetReactState(ReactStates::REACT_PASSIVE);

                    l_Creature->AddDelayedEvent([l_Creature, l_CasterPosition]() -> void
                    {
                        Movement::MoveSplineInit l_Init(l_Creature);

                        float l_Angle = l_CasterPosition.m_orientation;
                        float l_AddCount = M_PI * 2 / 10.0f;

                        for (uint8 l_Itr = 0; l_Itr < 10; l_Itr++)
                        {
                            Position l_SlumberingMovePos = l_CasterPosition;
                            l_SlumberingMovePos.SimplePosXYRelocationByAngle(l_SlumberingMovePos, 10.0f, l_Angle + l_AddCount, true);

                            l_Init.Path().push_back(G3D::Vector3(l_SlumberingMovePos.m_positionX, l_SlumberingMovePos.m_positionY, l_SlumberingMovePos.m_orientation));
                            l_Angle += l_AddCount;
                        }

                        l_Init.SetWalk(true);
                        l_Init.SetSmooth();
                        l_Init.Launch();
                    }, 2000);

                    l_Caster->CastSpell(l_Creature, eSpells::Spell_SlumberingGaspChanneling);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_slumbering_gasp_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_slumbering_gasp_SpellScript();
        }

        class spell_argus_slumbering_gasp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_slumbering_gasp_AuraScript);

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_slumbering_gasp_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_slumbering_gasp_AuraScript();
        }
};

/// Carrion Swarm - 249230
class spell_argus_carrion_swarm : public SpellScriptLoader
{
    public:
        spell_argus_carrion_swarm() : SpellScriptLoader("spell_argus_carrion_swarm") { }

        class spell_argus_carrion_swarm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_carrion_swarm_SpellScript);

            enum eSpells
            {
                Spell_CarrionSwarmCone = 249230
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                l_Caster->CastSpell(l_Caster, eSpells::Spell_CarrionSwarmCone);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 3000);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_carrion_swarm_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_carrion_swarm_SpellScript();
        }
};

/// Rain of Fire - 249400
class spell_argus_rain_of_fire : public SpellScriptLoader
{
    public:
        spell_argus_rain_of_fire() : SpellScriptLoader("spell_argus_rain_of_fire") { }

        class spell_argus_rain_of_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_rain_of_fire_SpellScript);

            enum eSpells
            {
                Spell_RainOfFire = 249399
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() > 5)
                {
                    p_Targets.resize(5);
                }
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_RainOfFire, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_rain_of_fire_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_rain_of_fire_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_rain_of_fire_SpellScript();
        }
};

/// Flame Breath - 249438
class spell_argus_flame_breath : public SpellScriptLoader
{
    public:
        spell_argus_flame_breath() : SpellScriptLoader("spell_argus_flame_breath") { }

        class spell_argus_flame_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_flame_breath_SpellScript);

            enum eSpells
            {
                Spell_FlameBreathCone = 249457
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                l_Caster->CastSpell(l_Caster, eSpells::Spell_FlameBreathCone);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 3000);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_flame_breath_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_flame_breath_SpellScript();
        }
};

/// Cripple - 249391
class spell_argus_cripple : public SpellScriptLoader
{
    public:
        spell_argus_cripple() : SpellScriptLoader("spell_argus_cripple") { }

        class spell_argus_cripple_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_cripple_AuraScript);

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (Aura* l_Aura = GetAura())
                {
                    l_Aura->ModStackAmount(2);
                }
            }

            void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (Aura* l_Aura = GetAura())
                {
                    l_Aura->ModStackAmount(-1);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_argus_cripple_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
                OnProc += AuraProcFn(spell_argus_cripple_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_cripple_AuraScript();
        }
};

/// Summon Explosive Orbs - 249440
class spell_argus_summon_explosive_orbs : public SpellScriptLoader
{
    public:
        spell_argus_summon_explosive_orbs() : SpellScriptLoader("spell_argus_summon_explosive_orbs") { }

        class spell_argus_summon_explosive_orbs_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_summon_explosive_orbs_SpellScript);

            enum eSpells
            {
                Spell_ExplosiveOrb = 249439
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint8 l_I = 0; l_I < 3; l_I++)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_ExplosiveOrb, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_argus_summon_explosive_orbs_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_summon_explosive_orbs_SpellScript();
        }
};

/// Lava Waves - 248758
class spell_argus_lava_waves : public SpellScriptLoader
{
    public:
        spell_argus_lava_waves() : SpellScriptLoader("spell_argus_lava_waves") { }

        class spell_argus_lava_waves_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_lava_waves_SpellScript);

            enum eSpells
            {
                Spell_LavaWavesSummon = 248761
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_CasterPos = l_Caster->GetPosition();
                float l_Angle = 0.0f;

                for (uint8 l_I = 0; l_I < 3; l_I++)
                {
                    Position l_DestPos;
                    l_Caster->GetNearestPosition(l_DestPos, 40.0f, l_Angle);
                    l_DestPos.m_orientation = l_DestPos.GetAngle(&l_CasterPos);
                    l_Angle += M_PI * 2.0f / 3;

                    l_Caster->CastSpell(l_DestPos, eSpells::Spell_LavaWavesSummon, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_argus_lava_waves_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_lava_waves_SpellScript();
        }
};

/// Fragment of Argus - 248655
class spell_argus_fragment_of_argus : public SpellScriptLoader
{
    public:
        spell_argus_fragment_of_argus() : SpellScriptLoader("spell_argus_fragment_of_argus") { }

        class spell_argus_fragment_of_argus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_fragment_of_argus_SpellScript);

            enum eSpells
            {
                Spell_FragmentOfArgusMissile = 248654
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() > 2)
                {
                    p_Targets.resize(2);
                }
            }

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_FragmentOfArgusMissile);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_fragment_of_argus_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_fragment_of_argus_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_fragment_of_argus_SpellScript();
        }
};

/// Spore Filled - 252113
class spell_argus_spore_filled : public SpellScriptLoader
{
    public:
        spell_argus_spore_filled() : SpellScriptLoader("spell_argus_spore_filled") { }

        class spell_argus_spore_filled_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_spore_filled_AuraScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                {
                    l_Caster->CastSpell(l_Caster, p_AuraEffect->GetTriggerSpell(), true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_spore_filled_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_spore_filled_AuraScript();
        }
};

/// Fire Enchanted - 251888
class spell_argus_fire_enchanted : public SpellScriptLoader
{
    public:
        spell_argus_fire_enchanted() : SpellScriptLoader("spell_argus_fire_enchanted") { }

        class spell_argus_fire_enchanted_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_fire_enchanted_AuraScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                {
                    l_Caster->CastSpell(l_Caster, p_AuraEffect->GetTriggerSpell(), true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_fire_enchanted_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_fire_enchanted_AuraScript();
        }
};

/// Temporal Precognition - 250252, 252488
class spell_argus_temporal_precognition : public SpellScriptLoader
{
    public:
        spell_argus_temporal_precognition() : SpellScriptLoader("spell_argus_temporal_precognition") { }

        class  spell_argus_temporal_precognition_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_temporal_precognition_AuraScript);

            enum eSpells
            {
                Spell_TemporalPrecognitionPlayer = 250252,
                Spell_TemporalPrecognitionSummon = 250253,
                Spell_TimeParadox = 252487
            };

            void HandleOnPeriodicPlayer(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Spell_TemporalPrecognitionSummon, true);
            }

            void HandleOnPeriodicImage(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = l_Caster->GetAnyOwner();
                if (!l_Owner)
                    return;

                l_Caster->CastSpell(l_Owner, eSpells::Spell_TimeParadox, true);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Spell_TemporalPrecognitionPlayer)
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_temporal_precognition_AuraScript::HandleOnPeriodicPlayer, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                else
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_temporal_precognition_AuraScript::HandleOnPeriodicImage, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_temporal_precognition_AuraScript();
        }
};

/// Sadist - 247590
class spell_argus_sadist : public SpellScriptLoader
{
    public:
        spell_argus_sadist() : SpellScriptLoader("spell_argus_sadist") { }

        class spell_argus_sadist_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_sadist_AuraScript);

            enum eSpells
            {
                Spell_Sadist = 247544
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (!m_LastTargetGuid)
                {
                    m_LastTargetGuid = l_Target->GetGUID();
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_Sadist, true);
                    return;
                }

                if (m_LastTargetGuid == l_Target->GetGUID())
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_Sadist, true);
                }
                else
                {
                    m_LastTargetGuid = l_Target->GetGUID();
                    l_Caster->RemoveAura(eSpells::Spell_Sadist);
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_argus_sadist_AuraScript::HandleOnProc);
            }

        private:
            uint64 m_LastTargetGuid;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_sadist_AuraScript();
        }
};

/// Seeds of Chaos - 247585
class spell_argus_seeds_of_chaos : public SpellScriptLoader
{
    public:
        spell_argus_seeds_of_chaos() : SpellScriptLoader("spell_argus_seeds_of_chaos") { }

        class spell_argus_seeds_of_chaos_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_seeds_of_chaos_SpellScript);

            enum eSpells
            {
                Spell_ExplosiveOrb = 249439
            };

            void HandleAfterCast()
            {
                PreventHitDefaultEffect(EFFECT_0);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint8 l_I = 0; l_I < 3; l_I++)
                {
                    l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_argus_seeds_of_chaos_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_seeds_of_chaos_SpellScript();
        }
};

/// Death Field - 247632
class spell_argus_death_field : public SpellScriptLoader
{
    public:
        spell_argus_death_field() : SpellScriptLoader("spell_argus_death_field") { }

        class spell_argus_death_field_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_death_field_SpellScript);

            enum eSpells
            {
                Spell_DeathField = 247635
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position const l_CasterPos = l_Caster->GetPosition();
                float l_Angle = l_Caster->GetOrientation() - 1.0f;
                float l_AddToAngleCount = 2.0f / 13.0f;

                for (uint8 i = 0; i < 13; i++)
                {
                    Position l_DestPos = l_CasterPos;
                    l_DestPos.SimplePosXYRelocationByAngle(l_DestPos, 60.0f, l_Angle, true);
                    l_Caster->CastSpell(l_DestPos, eSpells::Spell_DeathField, true);
                    l_Angle += l_AddToAngleCount;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_argus_death_field_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_death_field_SpellScript();
        }
};

/// Maddening Spores - 252122, 252338
class spell_argus_maddening_spores : public SpellScriptLoader
{
    public:
        spell_argus_maddening_spores() : SpellScriptLoader("spell_argus_maddening_spores") { }

        class  spell_argus_maddening_spores_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_maddening_spores_AuraScript);

            enum eSpells
            {
                Spell_MaddeningSporesPeriodic = 252122,
                Spell_MaddeningSporesCharm = 252338,
                Spell_Inoculation = 252364
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (m_scriptSpellId == eSpells::Spell_MaddeningSporesPeriodic)
                {
                    l_Caster->CastSpell(l_Target, eSpells::Spell_MaddeningSporesCharm, true);
                    Remove();
                }
                else
                {
                    if (l_Target->HealthBelowPct(51))
                    {
                        l_Target->CastSpell(l_Target, eSpells::Spell_Inoculation, true);
                        Remove();
                    }
                }
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::Spell_Inoculation, true);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Spell_MaddeningSporesPeriodic)
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_maddening_spores_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                else
                {
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_maddening_spores_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_argus_maddening_spores_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_AOE_CHARM, AURA_EFFECT_HANDLE_REAL);
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_maddening_spores_AuraScript();
        }
};

/// Phantasm - 247393
class spell_argus_phantasm : public SpellScriptLoader
{
    public:
        spell_argus_phantasm() : SpellScriptLoader("spell_argus_phantasm") { }

        class spell_argus_phantasm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_phantasm_SpellScript);

            enum eSpells
            {
                Spell_PhantasmMissile = 247370
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->isDead())
                    return;

                uint16 l_Delay = 0;

                for (uint8 l_Itr = 0; l_Itr < 5; l_Itr++)
                {
                    Position l_RandomDestPos;
                    l_RandomDestPos.m_orientation = frand(0.0f, M_PI * 2.0f);
                    l_Caster->GetRandomNearPosition(l_RandomDestPos, 30.0f);

                    l_Caster->AddDelayedEvent([l_Caster, l_RandomDestPos]() -> void
                    {
                        uint8 l_LeftSideBallsCount = urand(1, 8);
                        uint8 l_RightSideBallsCount = 8 - l_LeftSideBallsCount;

                        if (l_LeftSideBallsCount && l_RightSideBallsCount)
                        {
                            Position l_LeftSideDestPos;
                            Position l_RightSideDestPos;
                            float l_Distance = 3.0f;

                            for (uint8 l_Itr = 0; l_Itr < l_LeftSideBallsCount; l_Itr++)
                            {
                                l_RandomDestPos.SimplePosXYRelocationByAngle(l_LeftSideDestPos, l_Distance, l_RandomDestPos.GetOrientation() + static_cast<float>(M_PI / 2), true);
                                l_Caster->CastSpell(l_LeftSideDestPos, eSpells::Spell_PhantasmMissile, true);
                                l_Distance += 1.0f;
                            }

                            l_Distance = 3.0f;

                            for (uint8 l_Itr = 0; l_Itr < l_RightSideBallsCount; l_Itr++)
                            {
                                l_RandomDestPos.SimplePosXYRelocationByAngle(l_RightSideDestPos, l_Distance, l_RandomDestPos.GetOrientation() + static_cast<float>(-M_PI / 2), true);
                                l_Caster->CastSpell(l_RightSideDestPos, eSpells::Spell_PhantasmMissile, true);
                                l_Distance += 1.0f;
                            }
                        }
                        else
                        {
                            Position l_DestPos;
                            float l_Distance = 4.0f;
                            bool l_Increasing = false;

                            for (uint8 l_Itr = 0; l_Itr < l_LeftSideBallsCount; l_Itr++)
                            {
                                l_RandomDestPos.SimplePosXYRelocationByAngle(l_DestPos, l_Distance, l_Increasing ? l_RandomDestPos.GetOrientation() + static_cast<float>(-M_PI / 2) : l_RandomDestPos.GetOrientation() + static_cast<float>(M_PI / 2), true);
                                l_Caster->CastSpell(l_DestPos, eSpells::Spell_PhantasmMissile, true);

                                if (l_Distance - 1.0f)
                                {
                                    l_Distance -= 1.0f;
                                }
                                else
                                {
                                    l_Increasing = true;
                                    l_Distance += 1.0f;
                                }
                            }
                        }
                    }, l_Delay);

                    l_Delay += 4000;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_argus_phantasm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_phantasm_SpellScript();
        }
};

class poi_invasion_points : public POIScript
{
    public:
        poi_invasion_points() : POIScript("poi_invasion_points") { }

        void OnCreateBaseMap(Map* p_Map, uint32 p_ZoneID) override
        {
            if (p_Map->GetId() == 1669)
            {
                p_Map->AddTask([this, p_Map]()->void
                {
                    SummonInvasionPortals(p_Map);
                });
            }
        }

        void OnPOIAppears(AreaPOI const& p_AreaPOI) override
        {
            if (!IsInvasionPointPOI(p_AreaPOI.GetAreaPoiID()))
                return;

            uint32 l_AreaPoiId = p_AreaPOI.GetAreaPoiID();
            std::list<Map*> l_Maps = sMapMgr->GetMapsPerZoneId(1669, g_InvasionPointDatas[p_AreaPOI.GetAreaPoiID()]->ZoneId);
            for (Map* l_Map : l_Maps)
                l_Map->AddTask([l_Map, l_AreaPoiId, this]()->void { SummonInvasionPortals(l_Map, l_AreaPoiId); });

            ClearInvasionPointQuest(p_AreaPOI.GetAreaPoiID());
        }

        void OnPOIDisappears(AreaPOI const& p_AreaPOI) override
        {
            if (!IsInvasionPointPOI(p_AreaPOI.GetAreaPoiID()))
                return;

            std::lock_guard<std::mutex> l_Guard(m_Lock);

            for (uint64 l_Itr : m_PortalGuids)
            {
                if (Creature* l_Creature = ObjectAccessor::FindCreature(l_Itr))
                {
                    if (l_Creature->GetEntry() == g_InvasionPointDatas[p_AreaPOI.GetAreaPoiID()]->PortalEntry)
                    {
                        l_Creature->GetMap()->AddTask([l_Creature]()->void
                        {
                            l_Creature->DespawnOrUnsummon();
                        });

                        m_PortalGuids.erase(l_Creature->GetGUID());
                        break;
                    }
                }
            }
        }

        void OnNextPOISelect(uint32 p_GroupId, std::set<uint32>& p_AvailableNextPOIs) override
        {
            for (uint32 l_POIId : sPOIMgr->GetAllPOIIds())
            {
                std::set<uint32> l_AvailabeNextPOICopy = p_AvailableNextPOIs;
                for (auto l_AvailablePOI : l_AvailabeNextPOICopy)
                {
                    auto l_Itr = g_InvasionPointDatas.find(l_POIId);
                    if (l_Itr == g_InvasionPointDatas.end())
                        continue;

                    auto l_Itr2 = g_InvasionPointDatas.find(l_AvailablePOI);
                    if (l_Itr2 == g_InvasionPointDatas.end())
                        continue;

                    if (l_Itr->second->ScenarioZoneId == l_Itr2->second->ScenarioZoneId)
                        p_AvailableNextPOIs.erase(l_AvailablePOI);
                }
            }
        }

    private:
        void SummonInvasionPortals(Map* p_Map, uint16 p_AreaPoiID = 0)
        {
            std::lock_guard<std::mutex> l_Guard(m_Lock);

            if (p_AreaPoiID)
            {
                if (IsInvasionPointPOI(p_AreaPoiID))
                {
                    if (Creature* l_Creature = p_Map->SummonCreature(g_InvasionPointDatas[p_AreaPoiID]->PortalEntry, g_InvasionPointDatas[p_AreaPoiID]->PortalSpawnPosition))
                    {
                        m_PortalGuids.insert(l_Creature->GetGUID());
                    }
                }
            }
            else
            {
                for (uint32 l_POIId : sPOIMgr->GetActivePOIIds())
                {
                    if (IsInvasionPointPOI(l_POIId))
                    {
                        if (Creature* l_Creature = p_Map->SummonCreature(g_InvasionPointDatas[l_POIId]->PortalEntry, g_InvasionPointDatas[l_POIId]->PortalSpawnPosition))
                        {
                            m_PortalGuids.insert(l_Creature->GetGUID());
                        }
                    }
                }
            }
        }

        void ClearInvasionPointQuest(uint16 p_AreaPoiID)
        {
            uint32 l_QuestId = g_InvasionPointDatas[p_AreaPoiID]->QuestId;
            SessionMap const& l_Sessions = sWorld->GetAllSessions();
            std::ostringstream l_Query;

            l_Query << "DELETE FROM `character_queststatus_rewarded` WHERE `quest` = " << l_QuestId << ";";

            CharacterDatabase.Execute(l_Query.str().c_str());

            sWorld->GetSessionsLock().acquire_read();
            for (SessionMap::const_iterator l_Session = l_Sessions.begin(); l_Session != l_Sessions.end(); ++l_Session)
            {
                if (Player* l_Player = l_Session->second->GetPlayer())
                {
                    l_Player->RemoveRewardedQuest(l_QuestId);
                }
            }
            sWorld->GetSessionsLock().release();
        }

    private:
        std::set<uint64> m_PortalGuids;
        std::mutex m_Lock;
};

#ifndef __clang_analyzer__
void AddSC_argus_invasion_points()
{
    new npc_demon_hunter_126446();
    new npc_fel_lord_kazral_125272();
    new npc_vogrethar_the_defiled_125587();
    new npc_felflame_subjugator_125197();
    new npc_bursting_elemental_127214();
    new npc_magma_giant_124835();
    new npc_malphazel_125578();
    new npc_slumber_fog_125576();
    new npc_pit_lord_vilemus_124719();
    new npc_mazgoroth_125137();
    new npc_sotanathor_124555();
    new npc_mistress_alluradel_124625();
    new npc_marsh_shambler_127098();
    new npc_beaming_eye_127990();
    new npc_harbinger_drelnathar_125483();
    new npc_matron_folnuna_124514();
    new npc_dreadbringer_valus_125527();
    new npc_flamebringer_azrothel_125655();
    new npc_flamecaller_vezrah_125280();
    new npc_lava_wave_125313();
    new npc_dread_knight_zakgal_125252();
    new npc_future_image_126024();
    new npc_decimax_126007();
    new npc_velthrak_the_punisher_125634();
    new npc_flameweaver_verathix_125314();
    new npc_baldrazar_125666();
    new npc_gorgoloth_125148();
    new npc_inquisitor_meto_124592();
    new npc_occularus_124492();
    new at_argus_braziers_warmth();
    new at_argus_fel_spike();
    new at_argus_wake_of_destruction();
    new at_argus_energized();
    new at_argus_fiery_release();
    new at_argus_temporal_precognition();
    new at_argus_sacred_vines();
    new at_argus_flame_fissure();
    new at_argus_pool_of_shadows();
    new at_argus_seeds_of_chaos();
    new at_argus_searing_rune();
    new spell_argus_chaos_wave();
    new spell_argus_fel_spike();
    new spell_argus_enter_rift();
    new spell_argus_power_dilation();
    new spell_argus_narcolepsy();
    new spell_argus_pull_fire();
    new spell_argus_burning_steps();
    new spell_argus_molten_boulder();
    new spell_argus_wave_of_dread();
    new spell_argus_obliteration_beam();
    new spell_argus_something_stirs();
    new spell_argus_alert();
    new spell_argus_crush();
    new spell_argus_venom_storm();
    new spell_argus_blood_bags();
    new spell_argus_seed_of_destruction();
    new spell_argus_soul_cleave();
    new spell_argus_cavitation();
    new spell_argus_beguiling_charm();
    new spell_argus_volatile_reaction();
    new spell_argus_dark_spores();
    new spell_argus_beaming_gaze();
    new spell_argus_secrete_shadows();
    new spell_argus_nausea();
    new spell_argus_grotesque_spawn();
    new spell_argus_slumbering_gasp();
    new spell_argus_carrion_swarm();
    new spell_argus_rain_of_fire();
    new spell_argus_flame_breath();
    new spell_argus_cripple();
    new spell_argus_summon_explosive_orbs();
    new spell_argus_lava_waves();
    new spell_argus_fragment_of_argus();
    new spell_argus_spore_filled();
    new spell_argus_fire_enchanted();
    new spell_argus_temporal_precognition();
    new spell_argus_sadist();
    new spell_argus_seeds_of_chaos();
    new spell_argus_death_field();
    new spell_argus_maddening_spores();
    new spell_argus_phantasm();
    new poi_invasion_points();
}
#endif
