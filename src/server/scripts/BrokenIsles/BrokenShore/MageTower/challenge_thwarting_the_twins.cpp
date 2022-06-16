////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

/// Raest Magespear - 116409
class boss_raest_magespear : public CreatureScript
{
    public:
        boss_raest_magespear() : CreatureScript("boss_raest_magespear") { }

        enum eSpells
        {
            SpellShadowPower                = 235411,
            SpellShadowChannel              = 234289,
            SpellDarkPossession             = 234312,
            SpellRitual                     = 235089,
            SpellShadowyEnergies            = 235091,
            SpellDismiss                    = 235317,

            SpellTearRiftAura               = 235525,
            SpellTearRiftDummy              = 235446,
            SpellTearRiftTriggered          = 235447,
            SpellTearRiftSummon1            = 235456,
            SpellTearRiftSummon2            = 235457,

            SpellShadowBolt                 = 9613,

            SpellRuneOfSummoningSearcher    = 236468,
            SpellRuneOfSummoningMissile     = 236469,
            SpellRuneOfSummoningAreaTrigger = 236460,
            SpellRuneOfSummoningSummon      = 236461,
            SpellDisruptionAura             = 236466
        };

        enum eEvents
        {
            EventShadowBolt = 1,
            EventPhaseChange,
            EventRuneOfSummoning
        };

        enum eTalks
        {
            TalkIntro0,
            TalkIntro1,
            TalkIntro2,
            TalkEngaged,
            TalkTearRift,
            TalkRuneOfSummoning,
            TalkThingOfNightmare
        };

        enum eActions
        {
            ActionIntro,
            ActionPhaseChange
        };

        struct boss_raest_magespearAI : public BossAI
        {
            boss_raest_magespearAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            uint8 m_PhaseChangeIDx = 0;

            void Reset() override
            {
                _Reset();

                ApplyAllImmunities(true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                SetCombatMovement(false);

                SetFlyMode(true);

                m_PhaseChangeIDx = 0;

                uint32 l_Time = 1 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    me->SendPlayHoverAnim(true);

                    DoCast(me, eSpells::SpellShadowPower, true);
                    DoCast(me, eSpells::SpellShadowChannel, true);
                });

                l_Time += 5 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    Talk(eTalks::TalkIntro0);

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                    if (instance)
                    {
                        if (Creature* l_Karam = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureKaramMagespear)))
                        {
                            if (l_Karam->IsAIEnabled)
                                l_Karam->AI()->DoAction(eActions::ActionIntro);
                        }
                    }
                });

                l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    Position l_Pos = me->GetPosition();

                    l_Pos.m_positionZ = 437.9727f;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveTakeoff(1, l_Pos);
                });

                l_Time += 4 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    me->RemoveAura(eSpells::SpellShadowChannel);
                });

                l_Time += 19 * TimeConstants::IN_MILLISECONDS + 300;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    Talk(eTalks::TalkIntro1);
                });

                l_Time += 5 * TimeConstants::IN_MILLISECONDS + 600;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    DoCast(me, eSpells::SpellDarkPossession);
                });

                l_Time += 11 * TimeConstants::IN_MILLISECONDS + 100;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    Talk(eTalks::TalkIntro2);

                    if (!instance)
                        return;

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

                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataRaestAsset, 0, 0, l_Player, l_Player);
                });

                l_Time += 8 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    DoCast(me, eSpells::SpellRitual, true);
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                if (!instance)
                    return;

                instance->SetData(eData::DataTwartingTheTwins, 1);

                if (Creature* l_Karam = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureKaramMagespear)))
                {
                    if (!l_Karam->isInCombat() && l_Karam->IsAIEnabled)
                        l_Karam->AI()->AttackStart(p_Attacker);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (!instance)
                    return;

                instance->SetData(eData::DataTwartingTheTwins, 0);

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

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataTwinsAsset, 0, 0, l_Player, l_Player);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                {
                    p_Summon->Attack(l_Target, true);

                    p_Summon->GetMotionMaster()->Clear();
                    p_Summon->GetMotionMaster()->MoveChase(l_Target);
                }
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

                if (instance)
                    instance->SetData(eData::DataTwartingTheTwins, 0);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionPhaseChange:
                    {
                        me->InterruptNonMeleeSpells(true);

                        Talk(eTalks::TalkEngaged);

                        DoCast(me, eSpells::SpellDismiss);

                        me->RemoveAura(eSpells::SpellShadowyEnergies);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDismiss:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        switch (++m_PhaseChangeIDx)
                        {
                            /// Same abilities as Phase 2, as well as Rune of Summoning.
                            case 2:
                            {
                                DoCast(me, eSpells::SpellTearRiftAura, true);

                                events.ScheduleEvent(eEvents::EventShadowBolt, 100);
                                events.ScheduleEvent(eEvents::EventRuneOfSummoning, 24 * TimeConstants::IN_MILLISECONDS);
                                break;
                            }
                            /// Shadow Bolt: Hurls a bolt of dark magic at an enemy, inflicting Shadow damage. This deal little damage but spawn Shadowy Vestige and Shadowy Fiend which must be AoEd down.
                            case 1:
                            {
                                DoCast(me, eSpells::SpellTearRiftAura, true);

                                events.ScheduleEvent(eEvents::EventShadowBolt, 100);
                                events.ScheduleEvent(eEvents::EventPhaseChange, 40 * TimeConstants::IN_MILLISECONDS);
                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    case eSpells::SpellTearRiftDummy:
                    {
                        Talk(eTalks::TalkTearRift);

                        Position l_Pos = me->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 13.0f, frand(0.0f, 2.0f * M_PI), true);

                        DoCast(l_Pos, eSpells::SpellTearRiftTriggered, true);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this, l_Pos]() -> void
                        {
                            Position l_NewPos = l_Pos;

                            l_NewPos.SimplePosXYRelocationByAngle(l_NewPos, frand(2.0f, 3.0f), frand(0.0f, 2.0f * M_PI), true);

                            DoCast(l_Pos, eSpells::SpellTearRiftSummon1, true);

                            for (uint8 l_I = 0; l_I < 4; ++l_I)
                            {
                                l_NewPos.SimplePosXYRelocationByAngle(l_NewPos, frand(2.0f, 3.0f), frand(0.0f, 2.0f * M_PI), true);

                                DoCast(l_Pos, eSpells::SpellTearRiftSummon2, true);
                            }

                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellRuneOfSummoningSearcher:
                    {
                        Position l_Pos = me->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 15.0f, frand(0.0f, 2.0f * M_PI));

                        l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                        DoCast(l_Pos, eSpells::SpellRuneOfSummoningMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool p_Removed) override
            {
                switch (p_AreaTrigger->GetSpellId())
                {
                    case eSpells::SpellRuneOfSummoningAreaTrigger:
                    {
                        /// Only summons add at expiration
                        if (p_Removed)
                            break;

                        Talk(eTalks::TalkThingOfNightmare);

                        DoCast(p_AreaTrigger->GetPosition(), eSpells::SpellRuneOfSummoningSummon, true);
                        break;
                    }
                    default:
                        break;
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
                    case eEvents::EventShadowBolt:
                    {
                        if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Player, eSpells::SpellShadowBolt);

                        events.ScheduleEvent(eEvents::EventShadowBolt, 3 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventPhaseChange:
                    {
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        events.Reset();

                        DoCast(me, eSpells::SpellRitual, true);

                        if (!instance)
                            return;

                        if (Creature* l_Karam = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureKaramMagespear)))
                        {
                            if (!l_Karam->isAlive() || !l_Karam->IsAIEnabled)
                                return;

                            l_Karam->AI()->DoAction(eActions::ActionPhaseChange);
                        }

                        break;
                    }
                    case eEvents::EventRuneOfSummoning:
                    {
                        Talk(eTalks::TalkRuneOfSummoning);

                        DoCast(me, eSpells::SpellRuneOfSummoningSearcher);

                        events.ScheduleEvent(eEvents::EventRuneOfSummoning, 35 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_raest_magespearAI(p_Creature);
        }
};

/// Karam Magespear - 116410
class boss_karam_magespear : public CreatureScript
{
    public:
        boss_karam_magespear() : CreatureScript("boss_karam_magespear") { }

        enum eSpells
        {
            SpellPossess            = 234308,
            SpellPossessed          = 234311,

            SpellFixate             = 202081,
            SpellDefiler            = 237683,
            SpellUnrelenting        = 221419,
            SpellDismiss            = 235317,

            SpellRisingDragon       = 235426,

            SpellPurgatory          = 235308,

            SpellHandFromBeyondAura = 237562,
            SpellHandFromBeyondAoE  = 235619,
            SpellHandFromBeyondMiss = 235602
        };

        enum eEvents
        {
            EventUnrelenting = 1
        };

        enum eTalks
        {
            TalkIntro0,
            TalkIntro1,
            TalkIntro2,
            TalkChaseAgain,
            TalkChaseLast
        };

        enum eActions
        {
            ActionIntro,
            ActionPhaseChange
        };

        enum eVisual
        {
            VisualPossessed = 1878
        };

        struct boss_karam_magespearAI : public BossAI
        {
            boss_karam_magespearAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            bool m_Purgatory = false;

            uint8 m_PhaseChangeIDx = 0;

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_ATTACK_ME, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_STATE, AuraType::SPELL_AURA_MOD_TAUNT, true);

                _Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                m_Purgatory = false;

                m_PhaseChangeIDx = 0;

                /// Has a shield
                me->SetCanDualWield(false);

                SetCombatMovement(true);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_31);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_UNK1);

                DoCast(me, eSpells::SpellFixate, true);
                DoCast(me, eSpells::SpellDefiler, true);

                /// Must be able to auto-attack while fixating
                me->ClearUnitState(UnitState::UNIT_STATE_CASTING);

                events.ScheduleEvent(eEvents::EventUnrelenting, 25 * TimeConstants::IN_MILLISECONDS);

                if (!instance)
                    return;

                instance->SetData(eData::DataTwartingTheTwins, 1);

                if (Creature* l_Raest = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRaestMagespear)))
                {
                    if (!l_Raest->isInCombat() && l_Raest->IsAIEnabled)
                        l_Raest->AI()->AttackStart(p_Attacker);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (!instance)
                    return;

                if (Creature* l_Raest = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRaestMagespear)))
                {
                    if (l_Raest->isAlive())
                        return;
                }

                instance->SetData(eData::DataTwartingTheTwins, 0);

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

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataTwinsAsset, 0, 0, l_Player, l_Player);
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

                if (instance)
                    instance->SetData(eData::DataTwartingTheTwins, 0);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionIntro:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);

                        uint32 l_Time = 13 * TimeConstants::IN_MILLISECONDS + 400;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro0);
                        });

                        l_Time += 17 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellPossess, true);
                        });

                        l_Time += 1 * TimeConstants::IN_MILLISECONDS + 400;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro1);
                        });

                        l_Time += 3 * TimeConstants::IN_MILLISECONDS + 600;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellPossessed, true);
                        });

                        l_Time += 1 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            me->SetAIAnimKitId(eVisual::VisualPossessed);

                            me->setFaction(14);

                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_UNK1);
                        });

                        l_Time += 7 * TimeConstants::IN_MILLISECONDS + 200;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro2);
                        });

                        l_Time += 1 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            me->SetAIAnimKitId(0);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            if (Player* l_Target = me->SelectNearestPlayerNotGM(150.0f))
                                AttackStart(l_Target);
                        });

                        break;
                    }
                    case eActions::ActionPhaseChange:
                    {
                        m_Purgatory = false;

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_31);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            switch (++m_PhaseChangeIDx)
                            {
                                /// Karam Magespear wakes up, adding his abilities from Phase 1 and 3 to the mix.
                                case 2:
                                {
                                    Talk(eTalks::TalkChaseLast);
                                    break;
                                }
                                /// Karam Magespear has his abilities from phase 1, with the addition of the Hand from Beyond mechanic.
                                case 1:
                                {
                                    Talk(eTalks::TalkChaseAgain);
                                    break;
                                }
                                default:
                                    break;
                            }

                            DoCast(me, eSpells::SpellFixate, true);
                            DoCast(me, eSpells::SpellDefiler, true);
                            DoCast(me, eSpells::SpellHandFromBeyondAura, true);

                            /// Must be able to auto-attack while fixating
                            me->ClearUnitState(UnitState::UNIT_STATE_CASTING);

                            events.ScheduleEvent(eEvents::EventUnrelenting, 25 * TimeConstants::IN_MILLISECONDS + (m_PhaseChangeIDx * (2 * TimeConstants::IN_MILLISECONDS + 500)));
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageDealt(Unit* /*p_Victim*/, uint32& /*p_Damage*/, DamageEffectType /*p_DamageType*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo)
                    return;

                DoCast(me, eSpells::SpellRisingDragon, true);
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_Purgatory && me->HealthBelowPctDamaged(35, p_Damage))
                {
                    m_Purgatory = true;

                    me->AttackStop();
                    me->StopMoving();

                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    me->InterruptNonMeleeSpells(true);

                    SetCombatMovement(false);

                    if (!instance)
                        return;

                    if (Creature* l_Raest = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRaestMagespear)))
                    {
                        if (l_Raest->IsAIEnabled)
                            l_Raest->AI()->DoAction(eActions::ActionPhaseChange);
                    }
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDismiss:
                    {
                        DoCast(me, eSpells::SpellPurgatory);

                        events.Reset();

                        me->RemoveAura(eSpells::SpellUnrelenting);
                        me->RemoveAura(eSpells::SpellDefiler);

                        me->UpdateSpeed(UnitMoveType::MOVE_RUN, true);

                        me->SetObjectScale(1.0f);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_31);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellHandFromBeyondAoE:
                    {
                        if (!instance)
                            break;

                        if (Creature* l_Raest = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRaestMagespear)))
                        {
                            Position l_Pos = l_Raest->GetPosition();

                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, 20.0f, frand(0.0f, 2.0f * M_PI));

                            l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                            DoCast(l_Pos, eSpells::SpellHandFromBeyondMiss, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnAddThreat(Unit* p_Victim, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                if (!p_Victim->IsPlayer())
                {
                    p_Threat = 0.0f;
                    return;
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
                    case eEvents::EventUnrelenting:
                    {
                        DoCast(me, eSpells::SpellUnrelenting, true);

                        events.ScheduleEvent(eEvents::EventUnrelenting, 25 * TimeConstants::IN_MILLISECONDS + (m_PhaseChangeIDx * (2 * TimeConstants::IN_MILLISECONDS + 500)));
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
            return new boss_karam_magespearAI(p_Creature);
        }
};

/// Orbiter - 119580
/// Orbiter - 119581
/// Orbiter - 119582
/// Orbiter - 119583
/// Orbiter - 119584
class npc_twins_orbiter : public CreatureScript
{
    public:
        npc_twins_orbiter() : CreatureScript("npc_twins_orbiter") { }

        struct npc_twins_orbiterAI : public ScriptedAI
        {
            npc_twins_orbiterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                AddTimedDelayedOperation(1, [this]() -> void
                {
                    Position l_Pos = me->GetPosition();

                    /// Creating the circle path from the center
                    Movement::MoveSplineInit l_Init(me);

                    bool l_Clockwise = urand(0, 1);

                    switch (me->GetEntry())
                    {
                        case eCreatures::CreatureOrbiter4:
                        {
                            FillCirclePath(l_Pos, 15.0f, me->GetPositionZ(), l_Init.Path(), l_Clockwise);
                            break;
                        }
                        case eCreatures::CreatureOrbiter3:
                        {
                            FillCirclePath(l_Pos, 12.0f, me->GetPositionZ(), l_Init.Path(), l_Clockwise);
                            break;
                        }
                        case eCreatures::CreatureOrbiter2:
                        {
                            FillCirclePath(l_Pos, 10.0f, me->GetPositionZ(), l_Init.Path(), l_Clockwise);
                            break;
                        }
                        case eCreatures::CreatureOrbiter1:
                        {
                            FillCirclePath(l_Pos, 8.0f, me->GetPositionZ(), l_Init.Path(), l_Clockwise);
                            break;
                        }
                        case eCreatures::CreatureOrbiter0:
                        {
                            FillCirclePath(l_Pos, 14.0f, me->GetPositionZ(), l_Init.Path(), l_Clockwise);
                            break;
                        }
                        default:
                            break;
                    }

                    l_Init.SetWalk(true);
                    l_Init.SetCyclic();
                    l_Init.Launch();
                });
            }

            void FillCirclePath(Position const& p_Center, float p_Radius, float p_Z, Movement::PointsArray& p_Path, bool p_Clockwise)
            {
                float l_Step = p_Clockwise ? -M_PI / 10.0f : M_PI / 10.0f;
                float l_Angle = p_Center.GetAngle(me->GetPositionX(), me->GetPositionY());

                for (uint8 l_Iter = 0; l_Iter < 20; l_Angle += l_Step, ++l_Iter)
                {
                    G3D::Vector3 l_Point;
                    l_Point.x = p_Center.GetPositionX() + p_Radius * cosf(l_Angle);
                    l_Point.y = p_Center.GetPositionY() + p_Radius * sinf(l_Angle);
                    l_Point.z = p_Z;
                    p_Path.push_back(l_Point);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_twins_orbiterAI(p_Creature);
        }
};

/// Hand from Beyond - 118698
class npc_twins_hand_from_beyond : public CreatureScript
{
    public:
        npc_twins_hand_from_beyond() : CreatureScript("npc_twins_hand_from_beyond") { }

        enum eSpells
        {
            SpellHandFromBeyond     = 235580,
            SpellGraspFromBeyond    = 235578
        };

        enum eEvent
        {
            EventGraspFromBeyond = 1
        };

        enum eTalk
        {
            TalkGraspFromBeyond
        };

        struct npc_twins_hand_from_beyondAI : public Scripted_NoMovementAI
        {
            npc_twins_hand_from_beyondAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellHandFromBeyond, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 3);

                events.ScheduleEvent(eEvent::EventGraspFromBeyond, 1);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventGraspFromBeyond:
                    {
                        Talk(eTalk::TalkGraspFromBeyond);

                        DoCast(me, eSpells::SpellGraspFromBeyond);

                        events.ScheduleEvent(eEvent::EventGraspFromBeyond, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_twins_hand_from_beyondAI(p_Creature);
        }
};

/// Thing of Nightmare - 119098
class npc_twins_thing_of_nightmare : public CreatureScript
{
    public:
        npc_twins_thing_of_nightmare() : CreatureScript("npc_twins_thing_of_nightmare") { }

        enum eSpells
        {
            SpellThingOfNightmareScale  = 236470,
            SpellUnrelenting            = 221419,
            SpellFixate                 = 202081
        };

        enum eEvent
        {
            EventUnrelenting = 1
        };

        struct npc_twins_thing_of_nightmareAI : public ScriptedAI
        {
            npc_twins_thing_of_nightmareAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_ATTACK_ME, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_STATE, AuraType::SPELL_AURA_MOD_TAUNT, true);

                DoCast(me, eSpells::SpellThingOfNightmareScale, true);
                DoCast(me, eSpells::SpellFixate, true);

                /// Should be able to auto-attack while Fixated
                me->ClearUnitState(UnitState::UNIT_STATE_CASTING);

                events.ScheduleEvent(eEvent::EventUnrelenting, 25 * TimeConstants::IN_MILLISECONDS);
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
                    case eEvent::EventUnrelenting:
                    {
                        DoCast(me, eSpells::SpellUnrelenting, true);

                        events.ScheduleEvent(eEvent::EventUnrelenting, 25 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_twins_thing_of_nightmareAI(p_Creature);
        }
};

/// Rune of Summoning - 236460
class areatrigger_twins_rune_of_summoning : public AreaTriggerEntityScript
{
    public:
        areatrigger_twins_rune_of_summoning() : AreaTriggerEntityScript("areatrigger_twins_rune_of_summoning") { }

        enum eSpell
        {
            SpellDisrupted = 236707
        };

        int32 m_SoakingTime = -1;
        uint32 m_NeededTimeAtEnter = 0;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return false;

            m_SoakingTime = 0;
            m_NeededTimeAtEnter = p_AreaTrigger->GetDuration() / 2;
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return false;

            m_SoakingTime = -1;
            m_NeededTimeAtEnter = 0;
            return false;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Diff) override
        {
            if (m_SoakingTime < 0)
                return;

            m_SoakingTime += p_Diff;

            if (m_SoakingTime >= m_NeededTimeAtEnter)
            {
                m_SoakingTime = -1;
                m_NeededTimeAtEnter = 0;

                if (Unit* l_Caster = p_AreaTrigger->GetCaster())
                {
                    l_Caster->CastSpell(p_AreaTrigger->GetPosition(), eSpell::SpellDisrupted, true);

                    p_AreaTrigger->SetOnDespawn(true);
                    p_AreaTrigger->Remove();
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_twins_rune_of_summoning();
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_thwarting_the_twins()
{
    new boss_raest_magespear();
    new boss_karam_magespear();

    new npc_twins_orbiter();
    new npc_twins_hand_from_beyond();
    new npc_twins_thing_of_nightmare();

    new areatrigger_twins_rune_of_summoning();
}
#endif
