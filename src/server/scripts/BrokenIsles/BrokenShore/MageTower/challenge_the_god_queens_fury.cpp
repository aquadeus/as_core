////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

Position const g_RingPos = { 3298.33f, 529.0052f, 635.663f, 3.141593f };

/// Sigryn - 116484
class boss_sigryn : public CreatureScript
{
    public:
        boss_sigryn() : CreatureScript("boss_sigryn") { }

        enum eSpells
        {
            SpellFelEyes            = 231985,
            SpellAttackingBarrier   = 233807,
            SpellRingOfValorAT      = 242667,

            SpellFelSkeinAura       = 239056,
            SpellSharedHealthAura   = 79920,

            SpellSessile            = 237939,
            SpellEmpowered          = 237937,

            SpellThrowSpearSearcher = 238694,
            SpellThrowSpearMissile  = 238675,

            SpellBloodOfTheFather   = 237945,

            SpellAdvance            = 237849,

            SpellDarkWingsDummy     = 239089,
            SpellDarkWingsSummon    = 237730,

            SpellMorphSigrynNormal  = 233848,
            SpellCreditSigryn       = 233860
        };

        enum eEvents
        {
            EventThrowSpear = 1,
            EventDarkWings,
            EventBloodOfTheFather,
            EventEmpower,
            EventOrbOfValor,
            EventAdvance
        };

        enum eSigrynData
        {
            DataIntroConv   = 4267,
            DataOutroConv1  = 4274,
            DataOutroConv2  = 4275,

            EndFactionID    = 1770,
            EndMainHand     = 127346,
            EndOffHand      = 77408,

            TeleportAT      = 13470
        };

        enum eTalks
        {
            TalkIntro0,
            TalkIntro1,
            TalkAggro,
            TalkEmpowerOne,
            TalkEmpowered,
            TalkDarkWings,
            TalkValkyrsWarn,
            TalkEnd0,
            TalkEnd1,
            TalkEnd2,
            TalkEnd3
        };

        struct boss_sigrynAI : public BossAI
        {
            boss_sigrynAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            EventMap m_Events;

            bool m_IntroDone = false;
            bool m_BarrierReady = false;
            Position const m_EyirPos = { 3095.15f, 283.2812f, 672.6748f, 0.0f };

            uint32 m_EmpowerCounter = 0;

            std::vector<uint32> m_EmpowerPattern =
            {
                eCreatures::CreatureJarlVelbrand,
                eCreatures::CreatureSigryn,
                eCreatures::CreatureRuneseerFaljar,
                eCreatures::CreatureSigryn,
                eCreatures::CreatureRuneseerFaljar,
                eCreatures::CreatureJarlVelbrand,
                eCreatures::CreatureSigryn
            };

            std::array<std::array<Position const, eData::DataMaxDarkWingsPos>, eData::DataMaxDarkWings> m_DarkWingsPos =
            {
                {
                    {
                        {
                            { 3271.075f, 558.868f, 635.0f, 0.0f },
                            { 3271.075f, 552.868f, 635.0f, 0.0f },
                            { 3271.075f, 546.868f, 635.0f, 0.0f },
                            { 3271.075f, 540.868f, 635.0f, 0.0f },
                            { 3271.075f, 534.868f, 635.0f, 0.0f },
                            { 3271.075f, 528.868f, 635.0f, 0.0f },
                            { 3271.075f, 522.868f, 635.0f, 0.0f },
                            { 3271.075f, 516.868f, 635.0f, 0.0f },
                            { 3271.075f, 510.868f, 635.0f, 0.0f },
                            { 3271.075f, 504.868f, 635.0f, 0.0f },
                            { 3271.075f, 498.868f, 635.0f, 0.0f }
                        }
                    },
                    {
                        {
                            { 3327.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3321.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3315.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3309.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3303.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3297.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3291.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3285.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3279.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3273.965f, 583.7847f, 635.0f, 4.712389f },
                            { 3267.965f, 583.7847f, 635.0f, 4.712389f }
                        }
                    },
                    {
                        {
                            { 3267.494f, 476.0105f, 635.0f, 1.53589f },
                            { 3273.490f, 475.8011f, 635.0f, 1.53589f },
                            { 3279.487f, 475.5917f, 635.0f, 1.53589f },
                            { 3285.483f, 475.3823f, 635.0f, 1.53589f },
                            { 3291.479f, 475.1729f, 635.0f, 1.53589f },
                            { 3297.476f, 474.9635f, 635.0f, 1.53589f },
                            { 3303.472f, 474.7541f, 635.0f, 1.53589f },
                            { 3309.469f, 474.5447f, 635.0f, 1.53589f },
                            { 3315.465f, 474.3353f, 635.0f, 1.53589f },
                            { 3321.461f, 474.1259f, 635.0f, 1.53589f },
                            { 3327.458f, 473.9165f, 635.0f, 1.53589f }
                        }
                    },
                    {
                        {
                            { 3331.555f, 499.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 505.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 511.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 517.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 523.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 529.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 535.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 541.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 547.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 553.0903f, 635.0f, 3.141593f },
                            { 3331.555f, 559.0903f, 635.0f, 3.141593f }
                        }
                    }
                }
            };

            bool m_Defeated = false;

            void Reset() override
            {
                _Reset();

                m_Events.Reset();

                /// Has a shield
                me->SetCanDualWield(false);

                me->SetPower(me->getPowerType(), me->GetMaxPower(me->getPowerType()));

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellFelEyes, true);
                });

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!m_BarrierReady)
                        DoCast(me, eSpells::SpellAttackingBarrier, true);
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                m_EmpowerCounter = 0;

                Talk(eTalks::TalkAggro);

                events.ScheduleEvent(eEvents::EventThrowSpear, 14 * TimeConstants::IN_MILLISECONDS + 400);
                events.ScheduleEvent(eEvents::EventAdvance, 24 * TimeConstants::IN_MILLISECONDS);

                m_Events.ScheduleEvent(eEvents::EventDarkWings, 111 * TimeConstants::IN_MILLISECONDS + 500);
                m_Events.ScheduleEvent(eEvents::EventEmpower, 25 * TimeConstants::IN_MILLISECONDS + 500);
                m_Events.ScheduleEvent(eEvents::EventOrbOfValor, 30 * TimeConstants::IN_MILLISECONDS);

                if (!instance)
                    return;

                if (Creature* l_Velbrand = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJarlVelbrand)))
                {
                    if (!l_Velbrand->isInCombat() && l_Velbrand->IsAIEnabled)
                    {
                        DoCast(l_Velbrand, eSpells::SpellSessile, true);

                        l_Velbrand->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                        l_Velbrand->AI()->AttackStart(p_Attacker);
                    }
                }

                if (Creature* l_Faljar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRuneseerFaljar)))
                {
                    if (!l_Faljar->isInCombat() && l_Faljar->IsAIEnabled)
                    {
                        DoCast(l_Faljar, eSpells::SpellSessile, true);

                        l_Faljar->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                        l_Faljar->AI()->AttackStart(p_Attacker);
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon->GetEntry() == eCreatures::CreatureEyir)
                    summons.Summon(p_Summon);
                else
                    BossAI::JustSummoned(p_Summon);
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
                    instance->DoRemoveForcedMovementsOnPlayers();
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID != 1)
                    return;

                me->SetWalk(false);

                Talk(eTalks::TalkIntro1);

                AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    DoCast(me, eSpells::SpellFelSkeinAura, true);
                    DoCast(me, eSpells::SpellSharedHealthAura, true);

                    if (!instance)
                        return;

                    if (Creature* l_Velbrand = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJarlVelbrand)))
                    {
                        l_Velbrand->CastSpell(l_Velbrand, eSpells::SpellFelSkeinAura, true);
                        l_Velbrand->CastSpell(l_Velbrand, eSpells::SpellSharedHealthAura, true);
                    }

                    if (Creature* l_Faljar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRuneseerFaljar)))
                    {
                        l_Faljar->CastSpell(l_Faljar, eSpells::SpellFelSkeinAura, true);
                        l_Faljar->CastSpell(l_Faljar, eSpells::SpellSharedHealthAura, true);
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

                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataDefendingHalls, 0, 0, l_Player, l_Player);
                });
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellThrowSpearSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellThrowSpearMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                if (p_Mode == SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT && p_SpellInfo->Id == eSpells::SpellDarkWingsDummy)
                    DoCast(p_Dest->_position, eSpells::SpellDarkWingsSummon, true);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellEmpowered:
                    {
                        events.ScheduleEvent(eEvents::EventBloodOfTheFather, 1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Defeated)
                {
                    p_Damage = 0;
                    return;
                }

                if (!m_Defeated && me->HealthBelowPctDamaged(21, p_Damage) && instance)
                {
                    m_Defeated = true;

                    if (p_Damage >= me->GetHealth())
                        p_Damage = me->GetHealth() - 1;

                    if (Creature* l_Faljar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRuneseerFaljar)))
                    {
                        if (!l_Faljar->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC) && l_Faljar->IsAIEnabled)
                            l_Faljar->AI()->DoAction(0);
                    }

                    if (Creature* l_Jarl = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJarlVelbrand)))
                    {
                        if (!l_Jarl->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC) && l_Jarl->IsAIEnabled)
                            l_Jarl->AI()->DoAction(0);
                    }

                    events.Reset();
                    m_Events.Reset();

                    me->RemoveAllAuras();
                    me->RemoveAllAreasTrigger();

                    me->InterruptNonMeleeSpells(true);

                    me->DisableEvadeMode();

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    me->StopAttack();

                    me->StopMoving();

                    instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    Talk(eTalks::TalkEnd0);

                    uint32 l_Time = 7 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Time, [this]() -> void
                    {
                        if (instance)
                            instance->DoCombatStopOnPlayers();

                        if (Player* l_Player = me->SelectNearestPlayer(120.0f))
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eSigrynData::DataOutroConv1, l_Player, nullptr, *l_Player))
                                delete l_Conversation;
                        }
                    });

                    l_Time += 6 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Time, [this]() -> void
                    {
                        Talk(eTalks::TalkEnd1);
                    });

                    l_Time += 6 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Time, [this]() -> void
                    {
                        if (Player* l_Player = me->SelectNearestPlayer(120.0f))
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eSigrynData::DataOutroConv2, l_Player, nullptr, *l_Player))
                                delete l_Conversation;
                        }
                    });

                    l_Time += 12 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Time, [this]() -> void
                    {
                        Talk(eTalks::TalkEnd2);

                        DoCast(me, eSpells::SpellMorphSigrynNormal);

                        me->setFaction(eSigrynData::EndFactionID);

                        SetEquipmentSlots(false, eSigrynData::EndMainHand, eSigrynData::EndOffHand);
                    });

                    l_Time += 11 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Time, [this]() -> void
                    {
                        Talk(eTalks::TalkEnd3);

                        DoCast(me, eSpells::SpellCreditSigryn, true);

                        me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);

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

                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataDefendingHalls, 0, 0, l_Player, l_Player);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_IntroDone)
                {
                    if (Player* l_Player = me->SelectNearestPlayer(120.0f))
                    {
                        m_IntroDone = true;

                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eSigrynData::DataIntroConv, l_Player, nullptr, *l_Player))
                            delete l_Conversation;
                    }
                }

                if (!m_BarrierReady)
                {
                    if (Player* l_Player = me->SelectNearestPlayer(55.0f))
                    {
                        m_BarrierReady = true;

                        me->InterruptNonMeleeSpells(false);

                        uint32 l_Time = 1 * TimeConstants::IN_MILLISECONDS + 300;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro0);

                            me->SetFacingTo(0.122173f);

                            if (instance)
                            {
                                if (GameObject* l_Barrier = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GameObjectBarrier)))
                                    l_Barrier->SetGoState(GOState::GO_STATE_READY);

                                if (Creature* l_Velbrand = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJarlVelbrand)))
                                    l_Velbrand->SetFacingTo(0.122173f);

                                if (Creature* l_Faljar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRuneseerFaljar)))
                                    l_Faljar->SetFacingTo(0.122173f);
                            }

                            DoCast(g_RingPos, eSpells::SpellRingOfValorAT, true);
                        });

                        l_Time += 7 * TimeConstants::IN_MILLISECONDS + 300;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            me->SetWalk(true);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(1, { 3278.56f, 528.911f, 635.0531f, 0.122173f }, false);
                        });
                    }
                }

                if (m_Defeated || !UpdateVictim())
                    return;

                events.Update(p_Diff);
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventDarkWings:
                    {
                        Talk(eTalks::TalkDarkWings);
                        Talk(eTalks::TalkValkyrsWarn);

                        TriggerDarkWings();

                        m_Events.ScheduleEvent(eEvents::EventDarkWings, 60 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventEmpower:
                    {
                        if (!instance || m_EmpowerCounter >= m_EmpowerPattern.size())
                            break;

                        uint32 l_Entry = m_EmpowerPattern[m_EmpowerCounter++];

                        if (Creature* l_Add = Creature::GetCreature(*me, instance->GetData64(l_Entry)))
                        {
                            Talk(eTalks::TalkEmpowerOne, l_Add->GetGUID());

                            DoCast(l_Add, eSpells::SpellEmpowered, true);
                        }

                        m_Events.ScheduleEvent(eEvents::EventEmpower, 35 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventOrbOfValor:
                    {
                        me->SummonCreature(eCreatures::CreatureEyir, m_EyirPos);

                        m_Events.ScheduleEvent(eEvents::EventOrbOfValor, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventThrowSpear:
                    {
                        DoCast(me, eSpells::SpellThrowSpearSearcher);

                        events.ScheduleEvent(eEvents::EventThrowSpear, 13 * TimeConstants::IN_MILLISECONDS + 400);
                        break;
                    }
                    case eEvents::EventBloodOfTheFather:
                    {
                        Talk(eTalks::TalkEmpowered);

                        DoCast(me, eSpells::SpellBloodOfTheFather);
                        break;
                    }
                    case eEvents::EventAdvance:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Target, eSpells::SpellAdvance);

                        events.ScheduleEvent(eEvents::EventAdvance, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void TriggerDarkWings()
            {
                uint8 l_IDx = urand(0, (eData::DataMaxDarkWings - 1));
                uint8 l_Gap = 2;
                uint8 l_JDx = urand(1, 9);

                auto const& l_Array = m_DarkWingsPos[l_IDx];

                l_IDx = 0;

                for (Position const& l_Pos : l_Array)
                {
                    if (l_JDx == l_IDx && l_Gap > 0)
                    {
                        ++l_JDx;
                        --l_Gap;
                    }
                    else
                        DoCast(l_Pos, eSpells::SpellDarkWingsDummy, true);

                    ++l_IDx;
                }
            }
        };

        Position const m_TeleportATPos = { 3298.153f, 528.9566f, 635.662f, 0.0f };

        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, Quest const* p_Quest) override
        {
            AreaTrigger* l_AreaTrigger = new AreaTrigger;
            if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_AREATRIGGER), 0, p_Creature, p_Creature, nullptr, m_TeleportATPos, m_TeleportATPos, nullptr, 0, eSigrynData::TeleportAT))
                delete l_AreaTrigger;

            return true;
        }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_sigrynAI(p_Creature);
        }
};

/// Jarl Velbrand - 116499
class boss_jarl_velbrand : public CreatureScript
{
    public:
        boss_jarl_velbrand() : CreatureScript("boss_jarl_velbrand") { }

        enum eSpells
        {
            SpellHurlAxeAT      = 237879,

            SpellEmpowered      = 237937,

            SpellBerserkersRage = 237947,

            SpellBladestorm     = 237857
        };

        enum eEvents
        {
            EventHurlAxe = 1,
            EventBladestorm,
            EventBerserkersRage
        };

        enum eTalks
        {
            TalkEmpowered,
            TalkBladestorm,
            TalkBladestormWarn
        };

        enum eEntry
        {
            EntryBladestormAT = 8347
        };

        struct boss_jarl_velbrandAI : public BossAI
        {
            boss_jarl_velbrandAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            bool m_MeleeAttacks = false;
            bool m_Defeated = false;

            void Reset() override
            {
                _Reset();

                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, true);

                SetCombatMovement(false);

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_MeleeAttacks = false;

                _EnterCombat();

                events.ScheduleEvent(eEvents::EventHurlAxe, 1);
                events.ScheduleEvent(eEvents::EventBladestorm, 125 * TimeConstants::IN_MILLISECONDS);

                if (!instance)
                    return;

                if (Creature* l_Sigryn = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureSigryn)))
                {
                    if (!l_Sigryn->isInCombat() && l_Sigryn->IsAIEnabled)
                        l_Sigryn->AI()->AttackStart(p_Attacker);
                }

                if (Creature* l_Faljar = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureRuneseerFaljar)))
                {
                    if (!l_Faljar->isInCombat() && l_Faljar->IsAIEnabled)
                        l_Faljar->AI()->AttackStart(p_Attacker);
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
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellEmpowered:
                    {
                        m_MeleeAttacks = true;

                        Talk(eTalks::TalkEmpowered);

                        events.CancelEvent(eEvents::EventHurlAxe);
                        events.ScheduleEvent(eEvents::EventBerserkersRage, 1);
                        break;
                    }
                    case eSpells::SpellBerserkersRage:
                    {
                        SetCombatMovement(true);

                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Target);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellEmpowered:
                    {
                        me->StopMoving();

                        me->GetMotionMaster()->Clear();

                        SetCombatMovement(false);

                        m_MeleeAttacks = false;

                        events.ScheduleEvent(eEvents::EventHurlAxe, 1);
                        break;
                    }
                    case eSpells::SpellBladestorm:
                    {
                        me->RemoveAreaTrigger(eSpells::SpellBladestorm);
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
                    case eSpells::SpellBladestorm:
                    {
                        Talk(eTalks::TalkBladestormWarn);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetEntry() == eEntry::EntryBladestormAT)
                {
                    if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                        l_Player->SendApplyMovementForce(p_AreaTrigger->GetGUID(), true, p_AreaTrigger->GetPosition(), 3.0f, 1);
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool /*p_OnDespawn*/) override
            {
                if (p_AreaTrigger->GetEntry() == eEntry::EntryBladestormAT)
                {
                    if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                        l_Player->SendApplyMovementForce(p_AreaTrigger->GetGUID(), false, Position());
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                EndEncounter();
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Defeated)
                {
                    p_Damage = 0;
                    return;
                }

                if (!m_Defeated && p_Damage >= me->GetHealth())
                {
                    p_Damage = me->GetHealth() - 1;

                    EndEncounter();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Defeated || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventHurlAxe:
                    {
                        DoCast(me, eSpells::SpellHurlAxeAT);

                        events.ScheduleEvent(eEvents::EventHurlAxe, 4 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBladestorm:
                    {
                        Talk(eTalks::TalkBladestorm);

                        DoCast(me, eSpells::SpellBladestorm);

                        events.ScheduleEvent(eEvents::EventBladestorm, 105 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBerserkersRage:
                    {
                        DoCast(me, eSpells::SpellBerserkersRage);
                        break;
                    }
                    default:
                        break;
                }

                if (m_MeleeAttacks)
                    DoMeleeAttackIfReady();
            }

            void EndEncounter()
            {
                m_Defeated = true;

                events.Reset();

                me->RemoveAllAuras();
                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);

                me->DisableEvadeMode();

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                if (instance)
                    instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->StopAttack();

                me->StopMoving();

                me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_jarl_velbrandAI(p_Creature);
        }
};

/// Runeseer Faljar - 116496
class boss_runeseer_faljar : public CreatureScript
{
    public:
        boss_runeseer_faljar() : CreatureScript("boss_runeseer_faljar") { }

        enum eSpells
        {
            SpellShadowbolt             = 237890,

            SpellEmpowered              = 237937,

            SpellRunicDetonation15s     = 237910,
            SpellRunicDetonation12s     = 237911,
            SpellRunicDetonation9s      = 237912,
            SpellRunicDetonationDum     = 237914,
            SpellRunicDetonationAoE     = 237908,
            SpellRunicDetonationSoak    = 239021,

            SpellAncestralKnowledge     = 237949,
            SpellAncestralKnowledgeCast = 237952
        };

        enum eEvents
        {
            EventShadowbolt = 1,
            EventRunes,
            EventAncestralKnowledge
        };

        enum eTalks
        {
            TalkRunes,
            TalkEmpowered,
            TalkAncestralKnowledge
        };

        struct boss_runeseer_faljarAI : public BossAI
        {
            boss_runeseer_faljarAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            bool m_Defeated = false;

            void Reset() override
            {
                _Reset();

                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, true);

                me->SetPower(me->getPowerType(), me->GetMaxPower(me->getPowerType()));

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 333);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventShadowbolt, 1);
                events.ScheduleEvent(eEvents::EventRunes, 41 * TimeConstants::IN_MILLISECONDS);

                if (!instance)
                    return;

                if (Creature* l_Sigryn = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureSigryn)))
                {
                    if (!l_Sigryn->isInCombat() && l_Sigryn->IsAIEnabled)
                        l_Sigryn->AI()->AttackStart(p_Attacker);
                }

                if (Creature* l_Velbrand = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureJarlVelbrand)))
                {
                    if (!l_Velbrand->isInCombat() && l_Velbrand->IsAIEnabled)
                        l_Velbrand->AI()->AttackStart(p_Attacker);
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
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                if (p_Mode != SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellRunicDetonationDum:
                    {
                        Position l_Pos = p_Dest->_position;

                        float l_Angle = frand(0.0f, 2.0f * M_PI);

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 15.0f, l_Angle, false);

                        DoCast(l_Pos, eSpells::SpellRunicDetonation15s, true);

                        l_Pos = g_RingPos;

                        l_Angle = Position::NormalizeOrientation(l_Angle + (2.0f * M_PI / 3.0f));

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 10.0f, l_Angle, false);

                        DoCast(l_Pos, eSpells::SpellRunicDetonation12s, true);

                        l_Pos = g_RingPos;

                        l_Angle = Position::NormalizeOrientation(l_Angle + (2.0f * M_PI / 3.0f));

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 9.0f, l_Angle, false);

                        DoCast(l_Pos, eSpells::SpellRunicDetonation9s, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellEmpowered:
                    {
                        events.ScheduleEvent(eEvents::EventAncestralKnowledge, 1);
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
                    case eSpells::SpellRunicDetonation15s:
                    case eSpells::SpellRunicDetonation12s:
                    case eSpells::SpellRunicDetonation9s:
                    {
                        /// Only on expire
                        if (p_Removed)
                        {
                            DoCast(p_AreaTrigger->GetPosition(), eSpells::SpellRunicDetonationSoak, true);
                            break;
                        }

                        /// Damages
                        DoCast(p_AreaTrigger->GetPosition(), eSpells::SpellRunicDetonationAoE, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void NonInterruptCast(Unit* /*p_Caster*/, uint32 p_SpellID, uint32 /*p_SchoolMask*/) override
            {
                /// Ancestral Knowledge can be interrupted if absorb is gone
                if (p_SpellID == eSpells::SpellAncestralKnowledgeCast && !me->HasAura(eSpells::SpellAncestralKnowledge))
                    me->InterruptNonMeleeSpells(false, p_SpellID);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Defeated)
                {
                    p_Damage = 0;
                    return;
                }

                if (!m_Defeated && me->HealthBelowPctDamaged(9, p_Damage))
                {
                    if (p_Damage >= me->GetHealth())
                        p_Damage = me->GetHealth() - 1;

                    EndEncounter();
                }
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                if (!m_Defeated)
                    EndEncounter();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Defeated || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShadowbolt:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Target, eSpells::SpellShadowbolt);

                        events.ScheduleEvent(eEvents::EventShadowbolt, 3 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventRunes:
                    {
                        Talk(eTalks::TalkRunes);

                        DoCast(g_RingPos, eSpells::SpellRunicDetonationDum, true);

                        events.ScheduleEvent(eEvents::EventRunes, 17 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAncestralKnowledge:
                    {
                        Talk(eTalks::TalkEmpowered);
                        Talk(eTalks::TalkAncestralKnowledge);

                        DoCast(me, eSpells::SpellAncestralKnowledge);
                        break;
                    }
                    default:
                        break;
                }
            }

            void EndEncounter()
            {
                m_Defeated = true;

                events.Reset();

                me->RemoveAllAuras();
                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);

                me->DisableEvadeMode();

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                if (instance)
                    instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->StopAttack();

                me->StopMoving();

                me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_runeseer_faljarAI(p_Creature);
        }
};

/// Eyir - 119814
class npc_sigryn_eyir : public CreatureScript
{
    public:
        npc_sigryn_eyir() : CreatureScript("npc_sigryn_eyir") { }

        enum eSpells
        {
            SpellOrbOfValorDummy    = 238020,
            SpellOrbOfValorAT1      = 238019,
            SpellOrbOfValorAT2      = 238014
        };

        struct npc_sigryn_eyirAI : public ScriptedAI
        {
            npc_sigryn_eyirAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::vector<G3D::Vector3> m_Path =
            {
                { 3104.617f, 293.8559f, 675.1388f },
                { 3114.084f, 304.4306f, 677.6028f },
                { 3208.618f, 387.1649f, 677.6028f },
                { 3262.470f, 442.1945f, 662.1527f },
                { 3293.120f, 472.3524f, 662.1527f },
                { 3291.522f, 485.8646f, 662.1527f }
            };

            Position m_OrbPosMove;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_OrbPosMove = Position();

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                me->SendPlayHoverAnim(true);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveSmoothFlyPath(1, m_Path.data(), m_Path.size());
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID != 1)
                    return;

                me->GetMotionMaster()->Clear();

                DoCast(g_RingPos, eSpells::SpellOrbOfValorDummy, true);
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                if (p_Mode != SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellOrbOfValorDummy:
                    {
                        Position l_Pos = p_Dest->_position;

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(0.0f, 15.0f), frand(0.0f, 2.0f * M_PI));

                        me->SetFacingTo(me->GetAngle(&l_Pos));

                        m_OrbPosMove = l_Pos;

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellOrbOfValorAT1, true);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                switch (p_AreaTrigger->GetSpellId())
                {
                    case eSpells::SpellOrbOfValorAT1:
                    {
                        p_AreaTrigger->MoveAreaTrigger(m_OrbPosMove, 1 * TimeConstants::IN_MILLISECONDS);
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
                    case eSpells::SpellOrbOfValorAT1:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            l_Owner->CastSpell(m_OrbPosMove, eSpells::SpellOrbOfValorAT2, true);

                            me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_sigryn_eyirAI(p_Creature);
        }
};

/// Dark Val'kyr - 119696
class npc_sigryn_dark_valkyr : public CreatureScript
{
    public:
        npc_sigryn_dark_valkyr() : CreatureScript("npc_sigryn_dark_valkyr") { }

        enum eSpells
        {
            SpellShadowDissolveIn   = 237758,
            SpellDarkWingsJump      = 237759,
            SpellDarkWingsDummy     = 237769,
            SpellDarkWingsDamage    = 237761
        };

        struct npc_sigryn_dark_valkyrAI : public Scripted_NoMovementAI
        {
            npc_sigryn_dark_valkyrAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            Position m_Position;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                SetCombatMovement(false);

                me->DisableEvadeMode();

                me->SetCollision(false);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellShadowDissolveIn, true);

                m_Position = me->GetPosition();

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Position l_Pos = me->GetPosition();

                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 95.0f, l_Pos.m_orientation, true);

                    DoCast(l_Pos, eSpells::SpellDarkWingsJump, true);
                    DoCast(me, eSpells::SpellDarkWingsDummy, true);

                    AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        DoCast(m_Position, eSpells::SpellDarkWingsDamage, true);

                        me->DespawnOrUnsummon(1);
                    });
                });
            }

            void EnterEvadeMode() override { }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == MovementGeneratorType::EFFECT_MOTION_TYPE && p_ID == eSpells::SpellDarkWingsJump)
                {
                    me->GetMotionMaster()->Clear();

                    me->StopMoving();
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellDarkWingsDamage:
                    {
                        if (p_Targets.empty())
                            break;

                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer())
                                return true;

                            if (!m_Position.HasInLine(p_Object, 6.0f, false))
                                return true;

                            return false;
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
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_sigryn_dark_valkyrAI(p_Creature);
        }
};

/// Jump to Eyir's Vault - 233867
class spell_sigryn_jump_to_eyirs_vault : public SpellScriptLoader
{
    public:
        spell_sigryn_jump_to_eyirs_vault() : SpellScriptLoader("spell_sigryn_jump_to_eyirs_vault") { }

        enum eSpell
        {
            SpellEyirsVaultTeleport = 233866
        };

        class spell_sigryn_jump_to_eyirs_vault_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sigryn_jump_to_eyirs_vault_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpell::SpellEyirsVaultTeleport, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sigryn_jump_to_eyirs_vault_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sigryn_jump_to_eyirs_vault_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_the_god_queens_fury()
{
    new boss_sigryn();
    new boss_jarl_velbrand();
    new boss_runeseer_faljar();

    new npc_sigryn_eyir();
    new npc_sigryn_dark_valkyr();

    new spell_sigryn_jump_to_eyirs_vault();
}
#endif
