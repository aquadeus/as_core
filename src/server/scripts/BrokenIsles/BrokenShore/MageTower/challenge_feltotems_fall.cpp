////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

/// Jormog the Behemoth - 117484
class boss_jormog_the_behemoth : public CreatureScript
{
    public:
        boss_jormog_the_behemoth() : CreatureScript("boss_jormog_the_behemoth") { }

        enum eSpells
        {
            SpellFelHardenedScales  = 238471,

            SpellFelRageTugar       = 241932,

            SpellFelInfusedSaliva   = 243238,

            SpellSonicScream        = 241687
        };

        enum eEvents
        {
            EventFelInfusedSaliva = 1,
            EventSonicScream,
            EventBurrow
        };

        enum eVisuals
        {
            VisualBurrow    = 7742,
            VisualBurrowed  = 71887
        };

        struct boss_jormog_the_behemothAI : public BossAI
        {
            boss_jormog_the_behemothAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            Position const m_TriggerPos = { 5230.696f, 5017.852f, 628.5599f, 1.724431f };

            uint64 m_TriggerGuid = 0;

            bool m_Burrowed = false;

            void Reset() override
            {
                _Reset();

                for (uint8 l_I = 0; l_I < 9; ++l_I)
                    DoCast(me, eSpells::SpellFelHardenedScales, true);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventFelInfusedSaliva, 5 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSonicScream, 7 * TimeConstants::IN_MILLISECONDS + 200);
                events.ScheduleEvent(eEvents::EventBurrow, 22 * TimeConstants::IN_MILLISECONDS);

                if (!instance)
                    return;

                if (Creature* l_Tugar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureTugarBloodtotem)))
                {
                    if (!l_Tugar->isInCombat() && l_Tugar->IsAIEnabled)
                        l_Tugar->AI()->AttackStart(p_Attacker);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (!instance)
                    return;

                if (Creature* l_Tugar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureTugarBloodtotem)))
                {
                    if (l_Tugar->isAlive())
                        DoCast(l_Tugar, eSpells::SpellFelRageTugar, true);
                }

                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());
                Player* l_Player = nullptr;

                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Plr = l_Iter->getSource())
                    {
                        l_Player = l_Plr;
                        break;
                    }
                }

                if (!l_Scenario || !l_Player)
                    return;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataJormogAsset, 0, 0, l_Player, l_Player);
            }

            void EnterEvadeMode() override
            {
                /// Only evades if player is dead
                if (instance && !instance->IsWipe())
                    return;

                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);

                me->DespawnOrUnsummon();

                if (!instance)
                    return;

                if (Creature* l_Tugar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureTugarBloodtotem)))
                    l_Tugar->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_Burrowed)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFelInfusedSaliva:
                    {
                        DoCast(me, eSpells::SpellFelInfusedSaliva);

                        events.ScheduleEvent(eEvents::EventFelInfusedSaliva, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSonicScream:
                    {
                        DoCast(me, eSpells::SpellSonicScream);

                        events.ScheduleEvent(eEvents::EventSonicScream, 17 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBurrow:
                    {
                        m_Burrowed = true;

                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            me->RemoveAllAurasByCaster(l_Target->GetGUID());

                        if (Creature* l_Trigger = me->SummonCreature(eCreatures::CreatureJormogTrigger, m_TriggerPos))
                            m_TriggerGuid = l_Trigger->GetGUID();

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_UNK6);

                        me->SetAIAnimKitId(eVisuals::VisualBurrow);
                        me->SetDisplayId(eVisuals::VisualBurrowed);

                        AddTimedDelayedOperation(38 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_Burrowed = false;

                            if (Creature* l_Trigger = Creature::GetCreature(*me, m_TriggerGuid))
                            {
                                l_Trigger->RemoveAllAuras();
                                l_Trigger->DespawnOrUnsummon(1);

                                m_TriggerGuid = 0;

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);

                                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_UNK6);

                                    me->SetAIAnimKitId(0);
                                    me->RestoreDisplayId();
                                });
                            }
                        });

                        events.ScheduleEvent(eEvents::EventBurrow, 61 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_jormog_the_behemothAI(p_Creature);
        }
};

/// Tugar Bloodtotem - 117230
class boss_tugar_bloodtotem : public CreatureScript
{
    public:
        boss_tugar_bloodtotem() : CreatureScript("boss_tugar_bloodtotem") { }

        enum eSpells
        {
            SpellFelRageJormog      = 241933,

            SpellFelBurst           = 242733,

            SpellEarthquake         = 237950,
            SpellStalactiteAT       = 238302,
            SpellStalactiteDmg      = 238027,

            SpellFelRuptureAT       = 241676,
            SpellFelRuptureVisual   = 243382,
            SpellFelRuptureDamage   = 241666,

            SpellFelShock           = 242730,

            SpellFelSurgeTotem      = 243224,

            SpellSummonEgg          = 242384
        };

        enum eEvents
        {
            EventFelBurst = 1,
            EventEarthquake,
            EventFelRupture,
            EventFelShock,
            EventFelSurgeTotem,
            EventSummonEgg
        };

        enum eTalk
        {
            TalkDeath
        };

        struct boss_tugar_bloodtotemAI : public BossAI
        {
            boss_tugar_bloodtotemAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            uint8 m_FelburstCount = 0;

            void Reset() override
            {
                _Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventFelBurst, 3 * TimeConstants::IN_MILLISECONDS + 200);
                events.ScheduleEvent(eEvents::EventEarthquake, 20 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventFelRupture, 5 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventFelShock, 9 * TimeConstants::IN_MILLISECONDS + 600);
                events.ScheduleEvent(eEvents::EventFelSurgeTotem, 60 * TimeConstants::IN_MILLISECONDS + 600);
                events.ScheduleEvent(eEvents::EventSummonEgg, 24 * TimeConstants::IN_MILLISECONDS + 500);

                if (!instance)
                    return;

                if (Creature* l_Jormog = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJormogTheBehemoth)))
                {
                    if (!l_Jormog->isInCombat() && l_Jormog->IsAIEnabled)
                        l_Jormog->AI()->AttackStart(p_Attacker);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                Talk(eTalk::TalkDeath);

                if (!instance)
                    return;

                if (Creature* l_Jormog = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJormogTheBehemoth)))
                {
                    if (l_Jormog->isAlive())
                        DoCast(l_Jormog, eSpells::SpellFelRageJormog, true);
                }

                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());
                Player* l_Player = nullptr;

                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Plr = l_Iter->getSource())
                    {
                        l_Player = l_Plr;
                        break;
                    }
                }

                if (!l_Scenario || !l_Player)
                    return;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataTugarAsset, 0, 0, l_Player, l_Player);
            }

            void EnterEvadeMode() override
            {
                /// Only evades if player is dead
                if (instance && !instance->IsWipe())
                    return;

                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);

                me->DespawnOrUnsummon();

                if (!instance)
                    return;

                if (Creature* l_Jormog = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJormogTheBehemoth)))
                    l_Jormog->DespawnOrUnsummon();
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellStalactiteAT)
                {
                    p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET, 1 * TimeConstants::IN_MILLISECONDS + 200);

                    Position l_Pos = p_AreaTrigger->GetPosition();

                    l_Pos.m_positionZ = p_AreaTrigger->GetMap()->GetHeight(p_AreaTrigger->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);
                    l_Pos.m_positionX += 0.1f;

                    p_AreaTrigger->MoveAreaTrigger(l_Pos, 1 * TimeConstants::IN_MILLISECONDS + 200, false, false);

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS + 200, [this, l_Pos]() -> void
                    {
                        DoCast(l_Pos, eSpells::SpellStalactiteDmg, true);
                    });
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool /*p_OnDespawn*/) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellFelRuptureAT)
                {
                    if (Player* l_Target = p_AreaTrigger->FindNearestPlayer(p_AreaTrigger->GetRadius()))
                        DoCast(l_Target, eSpells::SpellFelRuptureDamage, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFelBurst:
                    {
                        ++m_FelburstCount;

                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Target, eSpells::SpellFelBurst);

                        std::vector<uint32> l_FelburstTimers = { 0, 21400, 23100, 21800, 20600, 19400, 18200, 17700, 19200, 15400, 15800, 14000, 13400, 13300, 12200, 12200, 11700, 10900, 10000, 10900, 19400, 10900, 13300 };

                        if (m_FelburstCount >= l_FelburstTimers.size())
                            m_FelburstCount = 1;

                        events.ScheduleEvent(eEvents::EventFelBurst, l_FelburstTimers[m_FelburstCount]);
                        break;
                    }
                    case eEvents::EventEarthquake:
                    {
                        DoCast(me, eSpells::SpellEarthquake);

                        events.ScheduleEvent(eEvents::EventEarthquake, 60 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFelRupture:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                        {
                            Position l_Pos = l_Target->GetPosition();

                            Position l_SumPos   = l_Pos;
                            float l_Step        = 3.8f;
                            float l_Dist        = l_Step;

                            auto l_FelRupture = [this](Position p_Pos) -> void
                            {
                                Position l_SumPos = p_Pos;

                                DoCast(l_SumPos, eSpells::SpellFelRuptureAT, true);

                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this, l_SumPos]() -> void
                                {
                                    DoCast(l_SumPos, eSpells::SpellFelRuptureVisual, true);
                                });
                            };

                            l_FelRupture(l_SumPos);

                            /// Front
                            for (uint8 l_I = 0; l_I < 3; ++l_I)
                            {
                                l_Pos.SimplePosXYRelocationByAngle(l_SumPos, l_Dist, l_Pos.m_orientation, true);

                                l_SumPos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_SumPos.m_positionX, l_SumPos.m_positionY, l_SumPos.m_positionZ + 5.0f);

                                l_Dist += l_Step;

                                l_FelRupture(l_SumPos);
                            }

                            l_Dist = l_Step;

                            /// Back
                            for (uint8 l_I = 0; l_I < 3; ++l_I)
                            {
                                l_Pos.SimplePosXYRelocationByAngle(l_SumPos, l_Dist, Position::NormalizeOrientation(l_Pos.m_orientation - M_PI), true);

                                l_SumPos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_SumPos.m_positionX, l_SumPos.m_positionY, l_SumPos.m_positionZ + 5.0f);

                                l_Dist += l_Step;

                                l_FelRupture(l_SumPos);
                            }

                            l_Dist = l_Step;

                            /// Left
                            for (uint8 l_I = 0; l_I < 3; ++l_I)
                            {
                                l_Pos.SimplePosXYRelocationByAngle(l_SumPos, l_Dist, Position::NormalizeOrientation(l_Pos.m_orientation - (M_PI / 2.0f)), true);

                                l_SumPos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_SumPos.m_positionX, l_SumPos.m_positionY, l_SumPos.m_positionZ + 5.0f);

                                l_Dist += l_Step;

                                l_FelRupture(l_SumPos);
                            }

                            l_Dist = l_Step;

                            /// Right
                            for (uint8 l_I = 0; l_I < 3; ++l_I)
                            {
                                l_Pos.SimplePosXYRelocationByAngle(l_SumPos, l_Dist, Position::NormalizeOrientation(l_Pos.m_orientation + (M_PI / 2.0f)), true);

                                l_SumPos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_SumPos.m_positionX, l_SumPos.m_positionY, l_SumPos.m_positionZ + 5.0f);

                                l_Dist += l_Step;

                                l_FelRupture(l_SumPos);
                            }

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->RemoveAreaTrigger(eSpells::SpellFelRuptureAT);
                            });
                        }

                        events.ScheduleEvent(eEvents::EventFelRupture, 11 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFelShock:
                    {
                        DoCast(me, eSpells::SpellFelShock);

                        events.ScheduleEvent(eEvents::EventFelShock, 25 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventFelSurgeTotem:
                    {
                        DoCast(me, eSpells::SpellFelSurgeTotem);

                        events.ScheduleEvent(eEvents::EventFelSurgeTotem, 28 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSummonEgg:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                        {
                            Position l_Pos = l_Target->GetPosition();

                            uint32 l_Time = 0;

                            for (uint8 l_I = 0; l_I < 2; ++l_I)
                            {
                                l_Target->SimplePosXYRelocationByAngle(l_Pos, frand(3.0f, 8.0f), frand(0.0f, 2.0f * M_PI), true);

                                if (l_Time)
                                {
                                    AddTimedDelayedOperation(l_Time, [this, l_Pos]() -> void
                                    {
                                        DoCast(l_Pos, eSpells::SpellSummonEgg, true);
                                    });
                                }
                                else
                                    DoCast(l_Pos, eSpells::SpellSummonEgg, true);

                                l_Time += 1 * TimeConstants::IN_MILLISECONDS;
                            }
                        }

                        events.ScheduleEvent(eEvents::EventSummonEgg, 60 * TimeConstants::IN_MILLISECONDS + 700);
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
            return new boss_tugar_bloodtotemAI(p_Creature);
        }
};

/// Navarogg <Stonedark Leader> - 117853
class npc_feltotem_navarogg : public CreatureScript
{
    public:
        npc_feltotem_navarogg() : CreatureScript("npc_feltotem_navarogg") { }

        enum eSpell
        {
            SpellNatureChanneling = 73093
        };

        enum eTalk
        {
            TalkIntro
        };

        struct npc_feltotem_navaroggAI : public Scripted_NoMovementAI
        {
            npc_feltotem_navaroggAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override
            {
                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpell::SpellNatureChanneling, true);

                    Talk(eTalk::TalkIntro);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_feltotem_navaroggAI(p_Creature);
        }
};

/// Tugar Bloodtotem - 117852
class npc_feltotem_tugar_bloodtotem : public CreatureScript
{
    public:
        npc_feltotem_tugar_bloodtotem() : CreatureScript("npc_feltotem_tugar_bloodtotem") { }

        enum eTalk
        {
            TalkIntro
        };

        enum eAsset
        {
            AssetStep1 = 56203
        };

        struct npc_feltotem_tugar_bloodtotemAI : public ScriptedAI
        {
            npc_feltotem_tugar_bloodtotemAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::vector<G3D::Vector3> m_Path =
            {
                { 5304.29f, 4969.568f, 626.7555f },
                { 5295.79f, 4971.318f, 626.7555f },
                { 5294.29f, 4971.568f, 627.0055f },
                { 5291.79f, 4971.818f, 627.2555f },
                { 5290.04f, 4972.068f, 627.2555f },
                { 5287.04f, 4972.568f, 627.5055f },
                { 5279.67f, 4973.917f, 627.1955f },
                { 5277.75f, 4975.339f, 627.5847f },
                { 5272.50f, 4978.339f, 627.8347f },
                { 5271.50f, 4978.589f, 627.8347f },
                { 5267.25f, 4981.089f, 627.8347f },
                { 5266.25f, 4981.589f, 627.8347f },
                { 5264.75f, 4982.339f, 628.0847f },
                { 5263.34f, 4983.253f, 628.6516f },
                { 5261.84f, 4984.003f, 628.6516f },
                { 5260.84f, 4984.503f, 628.6516f },
                { 5259.84f, 4985.003f, 628.6516f },
                { 5258.09f, 4986.253f, 628.9016f },
                { 5254.59f, 4988.253f, 629.1516f },
                { 5252.34f, 4989.753f, 629.1516f },
                { 5248.77f, 4991.850f, 629.2861f },
                { 5247.71f, 4992.694f, 629.4346f },
                { 5245.21f, 4994.444f, 629.4346f },
                { 5243.96f, 4995.194f, 629.4346f },
                { 5242.46f, 4996.194f, 629.4346f },
                { 5239.71f, 4998.194f, 628.6846f },
                { 5237.21f, 4999.694f, 628.4346f },
                { 5236.96f, 4999.944f, 628.1846f },
                { 5235.71f, 5001.944f, 627.9346f },
                { 5234.96f, 5002.944f, 628.1846f },
                { 5233.29f, 5005.862f, 628.1532f },
                { 5231.79f, 5008.862f, 628.1532f },
                { 5230.79f, 5010.862f, 628.1532f },
                { 5230.54f, 5012.862f, 628.4032f },
                { 5230.29f, 5014.862f, 628.6532f },
                { 5229.79f, 5017.612f, 628.6532f },
                { 5229.54f, 5019.862f, 628.6532f },
                { 5227.45f, 5033.197f, 627.9420f },
                { 5226.95f, 5036.197f, 627.9420f },
                { 5226.70f, 5037.947f, 627.6920f },
                { 5223.20f, 5039.697f, 627.1920f },
                { 5222.20f, 5040.447f, 627.1920f },
                { 5221.20f, 5040.947f, 626.9420f },
                { 5219.70f, 5041.697f, 626.9420f },
                { 5217.20f, 5043.197f, 626.4420f },
                { 5216.45f, 5043.697f, 626.4420f },
                { 5210.45f, 5046.947f, 625.9420f },
                { 5209.20f, 5047.697f, 625.6920f },
                { 5207.95f, 5048.447f, 625.9420f },
                { 5205.70f, 5049.697f, 625.9420f },
                { 5203.60f, 5050.721f, 625.4910f }
            };

            uint64 m_PlayerGuid = 0;

            void Reset() override
            {
                SetCanSeeEvenInPassiveMode(true);

                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || p_Who->GetDistance(me) > 45.0f || m_PlayerGuid)
                    return;

                LaunchIntroMovement(p_Who->ToPlayer());
            }

            void DamageTaken(Unit* p_Attacker, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                LaunchIntroMovement(p_Attacker->IsPlayer() ? p_Attacker->ToPlayer() : nullptr);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE || p_ID != 100)
                    return;

                me->DespawnOrUnsummon();

                if (Player* l_Player = Player::GetPlayer(*me, m_PlayerGuid))
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureJormogTheBehemoth)))
                            l_Boss->SetVisible(true);

                        if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureTugarBloodtotem)))
                            l_Boss->SetVisible(true);
                    }

                    Scenario* l_Scenario = sScenarioMgr->GetScenario(me->GetMap()->GetScenarioGuid());
                    if (!l_Scenario)
                        return;

                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eAsset::AssetStep1, 0, 0, l_Player, l_Player);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }

            void LaunchIntroMovement(Player* p_Player = nullptr)
            {
                if (Player* l_Player = (p_Player != nullptr ? p_Player : me->SelectNearestPlayerNotGM(150.0f)))
                {
                    m_PlayerGuid = l_Player->GetGUID();

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                    me->SetWalk(false);

                    AddTimedDelayedOperation(500, [this]() -> void
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                            l_Instance->DoCombatStopOnPlayers();

                        me->RemoveAllAuras();

                        Talk(eTalk::TalkIntro);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothPath(100, m_Path.data(), m_Path.size(), false);
                    });
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_feltotem_tugar_bloodtotemAI(p_Creature);
        }
};

/// Fel Surge Totem - 121499
class npc_feltotem_fel_surge_totem : public CreatureScript
{
    public:
        npc_feltotem_fel_surge_totem() : CreatureScript("npc_feltotem_fel_surge_totem") { }

        struct npc_feltotem_fel_surge_totemAI : public Scripted_NoMovementAI
        {
            npc_feltotem_fel_surge_totemAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpell
            {
                SpellFelSurgeCast = 242496
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpell::SpellFelSurgeCast);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpell::SpellFelSurgeCast)
                    me->DespawnOrUnsummon(1);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_feltotem_fel_surge_totemAI(p_Creature);
        }
};

/// Bile Spitter Egg - 121459
class npc_feltotem_bile_spitter_egg : public CreatureScript
{
    public:
        npc_feltotem_bile_spitter_egg() : CreatureScript("npc_feltotem_bile_spitter_egg") { }

        struct npc_feltotem_bile_spitter_eggAI : public Scripted_NoMovementAI
        {
            npc_feltotem_bile_spitter_eggAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpells
            {
                SpellSummonBileSpitterCast  = 242379,
                SpellSummonBileSpitter      = 243167
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->DisableEvadeMode();

                me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellSummonBileSpitterCast);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellSummonBileSpitterCast)
                {
                    DoCast(me, eSpells::SpellSummonBileSpitter, true);

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->Kill(me);
                    });
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (Player* l_Player = me->SelectNearestPlayerNotGM(150.0f))
                {
                    if (p_Summon->IsAIEnabled)
                        p_Summon->AI()->AttackStart(l_Player);

                    p_Summon->GetMotionMaster()->Clear();
                    p_Summon->GetMotionMaster()->MoveChase(l_Player);
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_feltotem_bile_spitter_eggAI(p_Creature);
        }
};

/// Bile Spitter - 121460
class npc_feltotem_bile_spitter : public CreatureScript
{
    public:
        npc_feltotem_bile_spitter() : CreatureScript("npc_feltotem_bile_spitter") { }

        struct npc_feltotem_bile_spitterAI : public ScriptedAI
        {
            npc_feltotem_bile_spitterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellSpitBileSearcher   = 242468,
                SpellSpitBileMissile    = 242566
            };

            enum eEvent
            {
                EventSpitBile = 1
            };

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvent::EventSpitBile, 2 * TimeConstants::IN_MILLISECONDS + 500);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSpitBileSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellSpitBileMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventSpitBile:
                    {
                        DoCast(me, eSpells::SpellSpitBileSearcher);

                        events.ScheduleEvent(eEvent::EventSpitBile, 6 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_feltotem_bile_spitterAI(p_Creature);
        }
};

/// Jormog the Behemoth - 119900
class npc_feltotem_jormog_trigger : public CreatureScript
{
    public:
        npc_feltotem_jormog_trigger() : CreatureScript("npc_feltotem_jormog_trigger") { }

        enum eSpells
        {
            SpellTransformBurrowedWorm  = 238703,
            SpellCreatureAcceleration   = 242332,

            SpellFelHardenedScales      = 238471,

            SpellCharge                 = 243242,

            SpellHitStalactite          = 242187,
            SpellStalactiteAT           = 238302,

            SpellJormogsFury            = 241719
        };

        enum eEvent
        {
            EventCharge = 1
        };

        enum eTalks
        {
            TalkHitStalactite,
            TalkChargeEmote
        };

        struct npc_feltotem_jormog_triggerAI : public ScriptedAI
        {
            npc_feltotem_jormog_triggerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::vector<G3D::Vector3> m_Path =
            {
                { 5230.89f, 5039.004f, 628.6969f },
                { 5227.39f, 5038.754f, 627.6969f },
                { 5223.89f, 5039.004f, 627.6969f },
                { 5220.39f, 5039.754f, 627.4469f },
                { 5217.39f, 5041.254f, 626.9469f },
                { 5214.39f, 5043.254f, 626.6969f },
                { 5211.89f, 5045.754f, 626.4469f },
                { 5209.89f, 5048.754f, 626.1969f },
                { 5208.64f, 5051.754f, 625.9469f },
                { 5207.64f, 5055.254f, 626.4469f },
                { 5207.39f, 5058.504f, 626.6969f },
                { 5207.64f, 5061.754f, 627.4469f },
                { 5208.64f, 5065.254f, 627.9469f },
                { 5209.89f, 5068.504f, 628.4469f },
                { 5211.89f, 5071.254f, 628.9469f },
                { 5214.39f, 5073.754f, 629.4469f },
                { 5217.39f, 5075.754f, 629.9469f },
                { 5220.39f, 5077.254f, 630.1969f },
                { 5223.89f, 5078.004f, 629.6969f },
                { 5227.39f, 5078.504f, 629.1969f },
                { 5230.89f, 5078.004f, 628.9469f },
                { 5234.14f, 5077.254f, 628.6969f },
                { 5237.39f, 5075.754f, 628.1969f },
                { 5240.14f, 5073.754f, 627.9469f },
                { 5242.64f, 5071.254f, 627.9469f },
                { 5244.64f, 5068.504f, 627.9469f },
                { 5246.14f, 5065.254f, 628.6969f },
                { 5246.89f, 5061.754f, 628.9469f },
                { 5247.08f, 5058.399f, 629.0936f },
                { 5246.89f, 5055.254f, 628.9469f },
                { 5246.14f, 5051.754f, 629.1969f },
                { 5244.64f, 5048.754f, 629.1969f },
                { 5242.64f, 5045.754f, 629.4469f },
                { 5240.14f, 5043.254f, 628.9469f },
                { 5237.39f, 5041.254f, 628.9469f },
                { 5234.14f, 5039.754f, 628.9469f }
            };

            bool m_Charge = false;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellTransformBurrowedWorm, true);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, m_Path[0]);
                });

                events.ScheduleEvent(eEvent::EventCharge, 8 * TimeConstants::IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_Type)
                {
                    case MovementGeneratorType::POINT_MOTION_TYPE:
                    {
                        switch (p_ID)
                        {
                            case 1:
                            {
                                if (m_Charge || p_ID != 1)
                                    break;

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MoveSmoothFlyPath(1, m_Path.data(), m_Path.size());
                                });

                                break;
                            }
                            case EventId::EVENT_CHARGE:
                            {
                                if (!m_Charge)
                                    break;

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    m_Charge = false;

                                    uint8 l_IDx    = 0;
                                    uint8 l_MaxIDx = 0;
                                    float l_Dist   = 100000.0f;

                                    for (auto const& l_Iter : m_Path)
                                    {
                                        if (me->GetExactDist(l_Iter.x, l_Iter.y, l_Iter.z) < l_Dist)
                                        {
                                            l_Dist   = me->GetExactDist(l_Iter.x, l_Iter.y, l_Iter.z);
                                            l_MaxIDx = l_IDx;
                                        }

                                        ++l_IDx;
                                    }

                                    std::vector<G3D::Vector3> l_RemovedPos;

                                    for (int8 l_I = l_MaxIDx; l_I >= 0; --l_I)
                                    {
                                        l_RemovedPos.insert(l_RemovedPos.begin(), m_Path[l_I]);

                                        m_Path.erase(m_Path.begin() + l_I);
                                    }

                                    for (auto const& l_Iter : l_RemovedPos)
                                        m_Path.push_back(l_Iter);

                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MoveSmoothFlyPath(1, m_Path.data(), m_Path.size());
                                });

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case MovementGeneratorType::EFFECT_MOTION_TYPE:
                    {
                        if (m_Charge || p_ID != 1)
                            break;

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(1, m_Path.data(), m_Path.size());
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                {
                    if (me->GetExactDist(l_Target) <= 1.0f)
                        DoCast(l_Target, eSpells::SpellJormogsFury, true);
                }

                if (AreaTrigger* l_AT = me->FindNearestAreaTrigger(eSpells::SpellStalactiteAT, 4.5f))
                {
                    DoCast(me, eSpells::SpellHitStalactite, true);

                    l_AT->Remove();

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        if (Creature* l_Jormog = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureJormogTheBehemoth)))
                        {
                            if (Aura* l_Aura = l_Jormog->GetAura(eSpells::SpellFelHardenedScales))
                            {
                                Talk(eTalks::TalkHitStalactite);

                                l_Aura->DropStack();
                            }
                        }
                    }

                    return;
                }

                if (events.ExecuteEvent() == eEvent::EventCharge)
                {
                    m_Charge = true;

                    me->StopMoving();

                    me->GetMotionMaster()->Clear();

                    Talk(eTalks::TalkChargeEmote);

                    DoCast(me, eSpells::SpellCharge, true);
                    DoCast(me, eSpells::SpellCreatureAcceleration, true);

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                        {
                            me->SetFacingTo(me->GetAngle(l_Target));

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveCharge(l_Target, 20.0f);
                        }
                    });

                    events.ScheduleEvent(eEvent::EventCharge, 12 * TimeConstants::IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_feltotem_jormog_triggerAI(p_Creature);
        }
};

/// Navarogg <Stonedark Leader> - 117863
class npc_feltotem_navarogg_outro : public CreatureScript
{
    public:
        npc_feltotem_navarogg_outro() : CreatureScript("npc_feltotem_navarogg_outro") { }

        enum eTalks
        {
            TalkOutro0,
            TalkOutro1,
            TalkOutro2,
            TalkOutro3
        };

        struct npc_feltotem_navarogg_outroAI : public ScriptedAI
        {
            npc_feltotem_navarogg_outroAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            Position const m_PortalPos = { 5242.76f, 4996.305f, 629.1935f, 0.0f };

            std::vector<G3D::Vector3> m_PathIn =
            {
                { 5248.172f, 4992.211f, 629.3448f },
                { 5247.172f, 4992.961f, 629.5948f },
                { 5245.172f, 4994.211f, 629.5948f },
                { 5243.922f, 4994.961f, 629.3448f },
                { 5242.672f, 4995.711f, 629.3448f },
                { 5239.672f, 4997.711f, 628.5948f },
                { 5237.056f, 4999.724f, 628.3145f },
                { 5235.806f, 5000.474f, 628.0645f },
                { 5235.056f, 5001.224f, 627.8145f },
                { 5234.056f, 5002.474f, 628.0645f },
                { 5233.056f, 5003.724f, 628.0645f },
                { 5229.806f, 5007.224f, 627.8145f },
                { 5229.806f, 5011.474f, 628.3145f },
                { 5229.556f, 5012.974f, 628.5645f },
                { 5229.556f, 5015.224f, 628.5645f },
                { 5229.566f, 5017.518f, 628.4975f },
                { 5229.566f, 5018.768f, 628.4975f },
                { 5229.566f, 5019.768f, 628.4975f },
                { 5229.566f, 5021.018f, 628.7475f },
                { 5229.316f, 5032.518f, 627.9975f },
                { 5225.904f, 5042.048f, 627.6241f },
                { 5224.241f, 5042.680f, 627.3051f }
            };

            std::vector<G3D::Vector3> m_PathOut =
            {
                { 5224.491f, 5040.680f, 627.5551f },
                { 5224.991f, 5036.180f, 627.5551f },
                { 5225.241f, 5033.430f, 627.5551f },
                { 5227.118f, 5020.896f, 628.4391f },
                { 5227.118f, 5019.646f, 628.1891f },
                { 5227.118f, 5018.396f, 628.1891f },
                { 5226.865f, 5013.817f, 627.7998f },
                { 5227.865f, 5011.317f, 627.7998f },
                { 5229.787f, 5006.706f, 627.9283f },
                { 5232.537f, 5004.456f, 627.9283f },
                { 5233.537f, 5003.956f, 627.9283f },
                { 5234.037f, 5003.456f, 627.9283f },
                { 5235.537f, 5002.456f, 627.9283f },
                { 5237.537f, 5001.206f, 628.1783f },
                { 5238.317f, 5000.570f, 628.3877f }
            };

            void Reset() override
            {
                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::TalkOutro0);

                    me->SetWalk(false);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothPath(0, m_PathIn.data(), m_PathIn.size(), false);
                });

                AddTimedDelayedOperation(13 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                {
                    Talk(eTalks::TalkOutro1);
                });

                AddTimedDelayedOperation(19 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                {
                    Talk(eTalks::TalkOutro2);
                });

                AddTimedDelayedOperation(20 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothPath(0, m_PathOut.data(), m_PathOut.size(), false);
                });

                AddTimedDelayedOperation(29 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                {
                    if (Player* l_Player = me->SelectNearestPlayer(100.0f))
                        Talk(eTalks::TalkOutro3, l_Player->GetGUID());
                });

                AddTimedDelayedOperation(30 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SummonGameObject(eGameObjects::GameObjectPortalToDalaran, m_PortalPos);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_feltotem_navarogg_outroAI(p_Creature);
        }
};

/// Earthquake - 237950
class spell_feltotem_earthquake : public SpellScriptLoader
{
    public:
        spell_feltotem_earthquake() : SpellScriptLoader("spell_feltotem_earthquake") { }

        enum eSpells
        {
            SpellStalactiteAT           = 238302,
            SpellStalactiteSwirlyVisual = 241395
        };

        class spell_feltotem_earthquake_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_feltotem_earthquake_Aurascript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    Position l_Pos = l_Target->GetPosition();

                    l_Target->GetNearPosition(l_Pos, frand(0.0f, 40.0f), frand(0.0f, 2.0f * M_PI));

                    l_Target->CastSpell(l_Pos, eSpells::SpellStalactiteSwirlyVisual, true);

                    l_Pos.m_positionZ += 24.0f;

                    l_Target->CastSpell(l_Pos, eSpells::SpellStalactiteAT, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_feltotem_earthquake_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_feltotem_earthquake_Aurascript();
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_feltotems_fall()
{
    new boss_jormog_the_behemoth();
    new boss_tugar_bloodtotem();

    new npc_feltotem_navarogg();
    new npc_feltotem_tugar_bloodtotem();
    new npc_feltotem_fel_surge_totem();
    new npc_feltotem_bile_spitter_egg();
    new npc_feltotem_bile_spitter();
    new npc_feltotem_jormog_trigger();
    new npc_feltotem_navarogg_outro();

    new spell_feltotem_earthquake();
}
#endif
