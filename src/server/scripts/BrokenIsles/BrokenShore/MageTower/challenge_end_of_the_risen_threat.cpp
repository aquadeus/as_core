////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

/// Lord Erdris Thorn <The Rooksguard> - 118488
class boss_lord_erdris_thorn : public CreatureScript
{
    public:
        boss_lord_erdris_thorn() : CreatureScript("boss_lord_erdris_thorn") { }

        enum eSpells
        {
            SpellFelChanneling  = 237706,
            SpellFelCorruption  = 237846,

            SpellFactionChange  = 243548,

            SpellStrike         = 218097,
            SpellIgniteSoulAura = 237188,
            SpellFelStompJump   = 237190,
            SpellFelStompAT     = 237192
        };

        enum eEvents
        {
            EventSpirits = 1,
            EventStrike,
            EventFelStomp,
            EventIgniteSoul
        };

        enum eActions
        {
            ActionMoveIntro,
            ActionSpiritHealed
        };

        enum eTalks
        {
            TalkIntro,
            TalkSpirits,
            TalkAggro
        };

        enum eGuids
        {
            GuidRemoveSpirit,
            GuidAddSpirit
        };

        enum eConversation
        {
            ConversationStage6 = 5060
        };

        struct boss_lord_erdris_thornAI : public BossAI
        {
            boss_lord_erdris_thornAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            Position const m_IntroPos = { 3184.74f, 7408.95f, 270.5648f, 0.0f };

            std::map<uint32, std::set<uint64>> m_AvailableSpirits;
            std::set<uint64> m_CurrentSpirits;
            std::set<uint64> m_CorruptedSpirits;

            uint8 m_SpiritsHealed = 0;

            EventMap m_Events;

            bool m_Jumping = false;

            void Reset() override
            {
                _Reset();

                /// Has a shield
                me->SetCanDualWield(false);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_Jumping = false;

                _EnterCombat();

                Talk(eTalks::TalkAggro);

                events.ScheduleEvent(eEvents::EventStrike, 4 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFelStomp, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventIgniteSoul, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!instance)
                    return;

                Player* l_Player = sObjectAccessor->FindPlayer(instance->GetData64(eData::DataPlayerGuid));
                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());

                if (!l_Scenario || !l_Player)
                    return;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage7Asset, 0, 0, l_Player, l_Player);

                l_Player->KilledMonsterCredit(eCreatures::CreatureFinalBossDefeatedHealer);

                instance->DoCombatStopOnPlayers();

                me->RemoveAllAreasTrigger();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionMoveIntro:
                    {
                        me->SetWalk(true);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, m_IntroPos);
                        break;
                    }
                    case eActions::ActionSpiritHealed:
                    {
                        ++m_SpiritsHealed;

                        if (!instance)
                            break;

                        Player* l_Player = sObjectAccessor->FindPlayer(instance->GetData64(eData::DataPlayerGuid));
                        Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());

                        if (!l_Scenario || !l_Player)
                            break;

                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage5Asset, m_SpiritsHealed, 0, l_Player, l_Player);

                        if (m_SpiritsHealed >= eData::DataSpiritsToHeal)
                        {
                            m_Events.Reset();

                            for (uint64 const& l_Guid : m_CurrentSpirits)
                            {
                                if (Creature* l_Spirit = Creature::GetCreature(*me, l_Guid))
                                {
                                    if (l_Spirit->IsAIEnabled)
                                        l_Spirit->AI()->DoAction(0);
                                }
                            }

                            m_CurrentSpirits.clear();
                            m_AvailableSpirits.clear();

                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversation::ConversationStage6, l_Player, nullptr, *l_Player))
                                delete l_Conversation;

                            std::set<uint32> l_Entries = { eCreatures::CreatureJarodShadowsong, eCreatures::CreatureCallieCarrington, eCreatures::CreatureGrannyMarl };

                            for (uint32 const& l_Entry : l_Entries)
                            {
                                if (Creature* l_Creature = Creature::GetCreature(*me, instance->GetData64(l_Entry)))
                                {
                                    l_Creature->StopAttack();
                                    l_Creature->CombatStop();

                                    l_Creature->RemoveAura(eSpells::SpellFactionChange);
                                }
                            }

                            AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (!instance)
                                    return;

                                instance->DoCombatStopOnPlayers();
                            });

                            /// Turn corrupted spirits into ennemies
                            AddTimedDelayedOperation(30 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                for (uint64 const& l_Guid : m_CorruptedSpirits)
                                {
                                    if (Creature* l_Creature = Creature::GetCreature(*me, l_Guid))
                                    {
                                        Creature* l_Summon = nullptr;

                                        switch (l_Creature->GetEntry())
                                        {
                                            case eCreatures::CreatureDamagedArcher:
                                            {
                                                l_Summon = me->SummonCreature(eCreatures::CreatureCorruptedRisenArbalest, l_Creature->GetPosition());
                                                break;
                                            }
                                            case eCreatures::CreatureDamagedMage:
                                            {
                                                l_Summon = me->SummonCreature(eCreatures::CreatureCorruptedRisenMage, l_Creature->GetPosition());
                                                break;
                                            }
                                            case eCreatures::CreatureDamagedSoldier:
                                            {
                                                l_Summon = me->SummonCreature(eCreatures::CreatureCorruptedRisenSoldier, l_Creature->GetPosition());
                                                break;
                                            }
                                            default:
                                                break;
                                        }

                                        if (instance && l_Summon && l_Summon->IsAIEnabled)
                                        {
                                            l_Summon->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                                            m_CurrentSpirits.insert(l_Summon->GetGUID());

                                            if (Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid)))
                                                l_Summon->AI()->AttackStart(l_Player);

                                            std::set<uint32> l_Entries = { eCreatures::CreatureJarodShadowsong, eCreatures::CreatureCallieCarrington, eCreatures::CreatureGrannyMarl };

                                            for (uint32 const& l_Entry : l_Entries)
                                            {
                                                if (Creature* l_Ally = Creature::GetCreature(*me, instance->GetData64(l_Entry)))
                                                {
                                                    if (l_Ally->IsAIEnabled)
                                                        l_Ally->AI()->AttackStart(l_Summon);
                                                }
                                            }
                                        }

                                        l_Creature->DespawnOrUnsummon();
                                    }
                                }

                                m_CorruptedSpirits.clear();
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 1:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro);

                            DoCast(me, eSpells::SpellFelChanneling);
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
                    case eSpells::SpellFelChanneling:
                    {
                        switch (p_Target->GetEntry())
                        {
                            case eCreatures::CreatureDamagedSoldier:
                            case eCreatures::CreatureDamagedMage:
                            case eCreatures::CreatureDamagedArcher:
                            {
                                m_AvailableSpirits[p_Target->GetEntry()].insert(p_Target->GetGUID());

                                p_Target->GetMotionMaster()->Clear();
                                p_Target->GetMotionMaster()->MoveRandom(2.0f);
                                break;
                            }
                            default:
                                break;
                        }

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
                    case eSpells::SpellFelChanneling:
                    {
                        m_Events.ScheduleEvent(eEvents::EventSpirits, 7 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eSpells::SpellFelStompAT:
                    {
                        m_Jumping = false;

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            if (!instance)
                                return;

                            if (Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid)))
                            {
                                AttackStart(l_Player);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Player);
                            }
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                switch (p_ID)
                {
                    case eGuids::GuidRemoveSpirit:
                    {
                        m_CurrentSpirits.erase(p_Guid);
                        break;
                    }
                    case eGuids::GuidAddSpirit:
                    {
                        m_CorruptedSpirits.insert(p_Guid);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                switch (p_Summon->GetEntry())
                {
                    case eCreatures::CreatureCorruptedRisenArbalest:
                    case eCreatures::CreatureCorruptedRisenMage:
                    case eCreatures::CreatureCorruptedRisenSoldier:
                    {
                        if (m_CurrentSpirits.find(p_Summon->GetGUID()) == m_CurrentSpirits.end())
                            break;

                        m_CurrentSpirits.erase(p_Summon->GetGUID());

                        /// End of Stage 6
                        if (m_CurrentSpirits.empty() && instance)
                        {
                            Player* l_Player = sObjectAccessor->FindPlayer(instance->GetData64(eData::DataPlayerGuid));
                            Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());

                            if (!l_Scenario || !l_Player)
                                break;

                            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage6Asset, 0, 0, l_Player, l_Player);

                            me->RemoveAura(eSpells::SpellFelChanneling);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            AttackStart(l_Player);
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

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventSpirits:
                    {
                        std::set<uint32> l_Entries = { eCreatures::CreatureDamagedSoldier, eCreatures::CreatureDamagedMage, eCreatures::CreatureDamagedArcher };

                        for (uint32 const& l_Entry : l_Entries)
                        {
                            if (m_AvailableSpirits.find(l_Entry) == m_AvailableSpirits.end())
                                continue;

                            std::set<uint64>& l_Set = m_AvailableSpirits.find(l_Entry)->second;
                            if (l_Set.empty())
                                continue;

                            auto l_Iter = l_Set.begin();

                            bool l_Found = false;

                            do
                            {
                                std::advance(l_Iter, urand(0, l_Set.size() - 1));

                                if (m_CorruptedSpirits.find((*l_Iter)) != m_CorruptedSpirits.end())
                                    continue;

                                if (Creature* l_Spirit = Creature::GetCreature(*me, (*l_Iter)))
                                {
                                    if (!l_Spirit->HasAura(eSpells::SpellFelCorruption))
                                    {
                                        l_Found = true;

                                        Talk(eTalks::TalkSpirits);

                                        DoCast(l_Spirit, eSpells::SpellFelCorruption, true);

                                        m_CurrentSpirits.insert((*l_Iter));

                                        l_Set.erase(l_Iter);
                                    }
                                }
                            }
                            while (!l_Found);
                        }

                        m_Events.ScheduleEvent(eEvents::EventSpirits, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                if (!UpdateVictim() || m_Jumping)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventStrike:
                    {
                        DoCastVictim(eSpells::SpellStrike);
                        events.ScheduleEvent(eEvents::EventStrike, 4 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFelStomp:
                    {
                        if (instance)
                        {
                            if (Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid)))
                            {
                                m_Jumping = true;

                                me->StopMoving();
                                me->GetMotionMaster()->Clear();

                                DoCast(l_Player, eSpells::SpellFelStompJump, true);
                            }
                        }

                        events.ScheduleEvent(eEvents::EventFelStomp, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventIgniteSoul:
                    {
                        if (instance)
                        {
                            if (Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid)))
                                DoCast(l_Player, eSpells::SpellIgniteSoulAura, true);
                        }

                        events.ScheduleEvent(eEvents::EventIgniteSoul, 20 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_lord_erdris_thornAI(p_Creature);
        }
};

/// Commander Jarod Shadowsong - 118447
class npc_commander_jarod_shadowsong : public CreatureScript
{
    public:
        npc_commander_jarod_shadowsong() : CreatureScript("npc_commander_jarod_shadowsong") { }

        enum eSpells
        {
            SpellCharge         = 218104,
            SpellChargeSlow     = 236027,
            SpellChargDamage    = 126664,

            SpellTaunt          = 197197,

            SpellWhirlwind      = 235844,

            SpellBloodthirst    = 235843,

            SpellFactionChange  = 243548
        };

        enum eEvents
        {
            EventCharge = 1,
            EventTaunt,
            EventWhirlwind,
            EventBloodthirst
        };

        enum eConversations
        {
            DataIntroConversation   = 4414,
            DataStep3Conversation   = 4424,
            DataStep4Conversation   = 4573,
            DataErdrisIntroConvo    = 4574
        };

        enum eActions
        {
            ActionStage1Completed,
            ActionJarodDeath,
            ActionStage3Completed,
            ActionStage4Triggered
        };

        enum eTalks
        {
            TalkStage1Completed,
            TalkDeath,
            TalkErdrisIntro
        };

        enum eSteps
        {
            StepWaves,
            StepFollowJarod,
            StepRescueAllies,
            StepEdrisAndTheRisen
        };

        struct npc_commander_jarod_shadowsongAI : public ScriptedAI
        {
            npc_commander_jarod_shadowsongAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            Position const m_TeleportPos = { 3163.699f, 7343.786f, 268.8984f, 2.335217f };

            InstanceScript* m_Instance;

            uint8 m_Step;

            uint64 m_ChargeTarget = 0;

            void Reset() override
            {
                m_Step = eSteps::StepWaves;

                me->m_CombatDistance = 45.0f;

                me->DisableEvadeMode();

                me->DisableHealthRegen();

                me->SetPower(Powers::POWER_RAGE, 0);

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Player* l_Player = me->SelectNearestPlayer(150.0f))
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::DataIntroConversation, l_Player, nullptr, *l_Player))
                            delete l_Conversation;
                    }
                });

                AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!m_Instance)
                        return;

                    m_Instance->SetData(eData::DataRisenThreatMoves, 0);
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                /// Don't reschedule events if it has already been done, NPCs do not reset
                if (events.Empty())
                {
                    events.ScheduleEvent(eEvents::EventCharge, 1);
                    events.ScheduleEvent(eEvents::EventTaunt, 3 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventWhirlwind, 16 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventBloodthirst, 17 * TimeConstants::IN_MILLISECONDS);
                }

                if (!m_Instance)
                    return;

                if (p_Attacker->GetEntry() == eCreatures::CreatureCallieCarrington || p_Attacker->GetEntry() == eCreatures::CreatureCallieCarrington)
                {
                    m_Step = eSteps::StepEdrisAndTheRisen;
                    return;
                }

                if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                {
                    if (!l_Granny->isInCombat() && l_Granny->IsAIEnabled)
                        l_Granny->AI()->AttackStart(p_Attacker);
                }

                if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                {
                    if (!l_Callie->isInCombat() && l_Callie->IsAIEnabled)
                        l_Callie->AI()->AttackStart(p_Attacker);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);

                if (!m_Instance)
                    return;

                if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                {
                    if (l_Callie->isAlive() && l_Callie->IsAIEnabled)
                        l_Callie->AI()->DoAction(eActions::ActionJarodDeath);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionStage1Completed:
                    {
                        Talk(eTalks::TalkStage1Completed);

                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            Position const l_MovePos = { 3180.876f, 7389.7134f, 224.8813f, 1.37811f };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(2, l_MovePos);
                        });

                        break;
                    }
                    case eActions::ActionStage3Completed:
                    {
                        if (!m_Instance)
                            break;

                        Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid));
                        Scenario* l_Scenario = sScenarioMgr->GetScenario(m_Instance->instance->GetScenarioGuid());

                        if (!l_Scenario || !l_Player)
                            return;

                        m_Instance->SetData(eData::DataRisenThreatTimer, 0);

                        l_Scenario->RemoveCriteriaTimer(CriteriaTimedTypes::CRITERIA_TIMED_TYPE_EVENT2, eData::DataStage3Timer);

                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage3Asset, 0, 0, l_Player, l_Player);

                        l_Scenario->SetCurrentStep(l_Scenario->GetCurrentStep() + 1);

                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::DataStep4Conversation, l_Player, nullptr, *l_Player))
                            delete l_Conversation;

                        me->CombatStop();

                        me->RemoveAura(eCommonSpells::SpellFelCorruption);

                        DoCast(me, eCommonSpells::SpellFelTouched, true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->SendPlayHoverAnim(false);

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                SetFlyMode(false);

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MoveFall();

                                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        if (m_Instance)
                                            m_Instance->DoCombatStopOnPlayers();

                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, Anim::ANIM_KNEEL_LOOP);
                                    });
                                });
                            });
                        });

                        if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                        {
                            l_Granny->CombatStop();

                            if (l_Granny->IsAIEnabled)
                                l_Granny->AI()->DoAction(p_Action);
                        }

                        if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                        {
                            l_Callie->CombatStop();

                            if (l_Callie->IsAIEnabled)
                                l_Callie->AI()->DoAction(p_Action);
                        }

                        break;
                    }
                    case eActions::ActionStage4Triggered:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, 0);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 3163.639f, 7345.670f, 264.3070f },
                                { 3163.639f, 7357.670f, 264.3070f },
                                { 3163.639f, 7359.670f, 264.3070f },
                                { 3163.639f, 7361.420f, 265.0570f },
                                { 3163.639f, 7363.420f, 265.3070f },
                                { 3163.639f, 7364.670f, 265.3070f },
                                { 3163.639f, 7366.670f, 266.0570f },
                                { 3163.579f, 7367.555f, 266.1493f },
                                { 3163.906f, 7367.619f, 266.6050f },
                                { 3165.156f, 7370.619f, 267.6050f },
                                { 3165.906f, 7371.869f, 268.3550f },
                                { 3166.656f, 7374.369f, 269.6050f },
                                { 3167.406f, 7376.119f, 270.3550f },
                                { 3168.156f, 7378.119f, 271.1050f },
                                { 3168.655f, 7378.854f, 271.2453f },
                                { 3168.891f, 7379.119f, 271.3291f },
                                { 3169.641f, 7380.369f, 272.0791f },
                                { 3170.391f, 7381.619f, 272.0791f },
                                { 3171.391f, 7383.619f, 271.8291f },
                                { 3172.641f, 7385.869f, 271.8291f },
                                { 3174.141f, 7388.369f, 271.8291f },
                                { 3174.498f, 7389.161f, 271.6910f },
                                { 3174.623f, 7389.396f, 271.9173f },
                                { 3175.373f, 7390.646f, 271.6673f },
                                { 3176.123f, 7392.146f, 270.9173f },
                                { 3180.123f, 7399.896f, 270.9173f },
                                { 3180.509f, 7400.883f, 270.5648f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothPath(5, l_Path.data(), l_Path.size(), false);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_Type)
                {
                    case MovementGeneratorType::POINT_MOTION_TYPE:
                    {
                        switch (p_ID)
                        {
                            case 2:
                            {
                                ++m_Step;

                                me->SetHomePosition(*me);
                                break;
                            }
                            case 3:
                            {
                                AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    std::vector<G3D::Vector3> l_Path =
                                    {
                                        { 3186.680f, 7404.230f, 231.7170f },
                                        { 3182.020f, 7405.240f, 230.7020f },
                                        { 3188.474f, 7414.992f, 233.7102f },
                                        { 3188.974f, 7421.992f, 233.2102f },
                                        { 3174.974f, 7422.242f, 234.7102f },
                                        { 3161.427f, 7419.244f, 239.7183f }
                                    };

                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MoveSmoothFlyPath(4, l_Path.data(), l_Path.size());
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
                        switch (p_ID)
                        {
                            case eSpells::SpellCharge:
                            {
                                if (!m_ChargeTarget)
                                    break;

                                if (Unit* l_Target = Unit::GetUnit(*me, m_ChargeTarget))
                                {
                                    DoCast(l_Target, eSpells::SpellChargeSlow, true);
                                    DoCast(l_Target, eSpells::SpellChargDamage, true);
                                }

                                m_ChargeTarget = 0;
                                break;
                            }
                            case 4:
                            {
                                if (!m_Instance)
                                    break;

                                if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                                {
                                    if (GroupPtr l_Group = l_Player->GetGroup())
                                        l_Group->RemoveCreatureMember(me);
                                }

                                me->NearTeleportTo(m_TeleportPos);
                                break;
                            }
                            case 5:
                            {
                                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY1H);

                                if (!m_Instance)
                                    break;

                                if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                                {
                                    Conversation* l_Conversation = new Conversation;
                                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::DataErdrisIntroConvo, l_Player, nullptr, *l_Player))
                                        delete l_Conversation;
                                }

                                uint32 l_Time = 12 * TimeConstants::IN_MILLISECONDS + 100;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_QUESTION);
                                });

                                l_Time += 200;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    Talk(eTalks::TalkErdrisIntro);
                                });

                                l_Time += 2 * TimeConstants::IN_MILLISECONDS + 700;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                        me->SetFacingTo(me->GetAngle(l_Callie));
                                });

                                l_Time += 1 * TimeConstants::IN_MILLISECONDS + 500;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                        me->SetFacingTo(me->GetAngle(l_Granny));

                                    me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_POINT);
                                });

                                l_Time += 2 * TimeConstants::IN_MILLISECONDS + 700;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                    {
                                        if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                            l_Callie->SetFacingTo(l_Callie->GetAngle(l_Granny));
                                    }
                                });

                                l_Time += 1 * TimeConstants::IN_MILLISECONDS;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_STUN);
                                });

                                l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                        l_Callie->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_STUN);
                                });

                                l_Time += 3 * TimeConstants::IN_MILLISECONDS;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                    {
                                        if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                            l_Granny->SetFacingTo(l_Granny->GetAngle(l_Callie));
                                    }
                                });

                                l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                        l_Callie->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);
                                });

                                l_Time += 1 * TimeConstants::IN_MILLISECONDS;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                                    if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                        l_Granny->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_STUN);
                                });

                                l_Time += 4 * TimeConstants::IN_MILLISECONDS + 400;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                    {
                                        l_Granny->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                                        if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                            l_Granny->SetFacingTo(l_Granny->GetAngle(l_Callie));
                                    }
                                });

                                l_Time += 4 * TimeConstants::IN_MILLISECONDS + 700;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    DoCast(me, eSpells::SpellFactionChange, true);

                                    if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                        l_Callie->CastSpell(l_Callie, eSpells::SpellFactionChange, true);
                                });

                                l_Time += 1 * TimeConstants::IN_MILLISECONDS + 200;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                    {
                                        l_Granny->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);
                                        l_Granny->CastSpell(l_Granny, eSpells::SpellFactionChange, true);
                                    }
                                });

                                l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                                AddTimedDelayedOperation(l_Time, [this]() -> void
                                {
                                    if (!m_Instance)
                                        return;

                                    if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                    {
                                        AttackStart(l_Callie);

                                        if (l_Callie->IsAIEnabled)
                                            l_Callie->AI()->AttackStart(me);
                                    }

                                    if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                    {
                                        if (l_Granny->IsAIEnabled)
                                            l_Granny->AI()->AttackStart(me, false);
                                    }

                                    Player* l_Player = sObjectAccessor->FindPlayer(m_Instance->GetData64(eData::DataPlayerGuid));
                                    Scenario* l_Scenario = sScenarioMgr->GetScenario(m_Instance->instance->GetScenarioGuid());

                                    if (!l_Scenario || !l_Player)
                                        return;

                                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage4Asset2, 0, 0, l_Player, l_Player);

                                    if (Creature* l_Erdris = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureLordErdrisThorn)))
                                    {
                                        if (l_Erdris->IsAIEnabled)
                                            l_Erdris->AI()->DoAction(0);
                                    }
                                });

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (m_Step != eSteps::StepFollowJarod || !m_Instance || !p_Who->IsPlayer() || p_Who->GetDistance(*me) > 10.0f)
                    return;

                ++m_Step;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(m_Instance->instance->GetScenarioGuid());

                if (!l_Scenario)
                    return;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage2Asset, 0, 0, p_Who->ToPlayer(), p_Who->ToPlayer());

                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Player* l_Player = me->SelectNearestPlayer(150.0f))
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::DataStep3Conversation, l_Player, nullptr, *l_Player))
                            delete l_Conversation;
                    }

                    if (m_Instance)
                    {
                        m_Instance->SetData(eData::DataRisenThreatCapture, 1);

                        if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(eGameObjects::GameObjectDoorToThirdStage)))
                            l_Door->SetGoState(GOState::GO_STATE_READY);
                    }
                });
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eCommonSpells::SpellFelCorruption:
                    {
                        me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        SetFlyMode(true);

                        me->SendPlayHoverAnim(true);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            Position l_Pos = me->GetPosition();

                            l_Pos.m_positionZ += 5.0f;

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(3, l_Pos);
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

                if (!UpdateVictim() || m_Step == eSteps::StepRescueAllies)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCharge:
                    {
                        if (Unit* l_Victim = me->getVictim())
                        {
                            if (!l_Victim->IsWithinMeleeRange(me))
                            {
                                m_ChargeTarget = l_Victim->GetGUID();

                                DoCast(l_Victim, eSpells::SpellCharge);
                            }
                        }

                        events.ScheduleEvent(eEvents::EventCharge, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTaunt:
                    {
                        DoCastVictim(eSpells::SpellTaunt);

                        events.ScheduleEvent(eEvents::EventTaunt, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventWhirlwind:
                    {
                        DoCast(me, eSpells::SpellWhirlwind);

                        events.ScheduleEvent(eEvents::EventWhirlwind, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBloodthirst:
                    {
                        DoCastVictim(eSpells::SpellBloodthirst);

                        events.ScheduleEvent(eEvents::EventBloodthirst, 20 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_commander_jarod_shadowsongAI(p_Creature);
        }
};

/// Granny Marl - 118448
class npc_granny_marl : public CreatureScript
{
    public:
        npc_granny_marl() : CreatureScript("npc_granny_marl") { }

        enum eSpells
        {
            SpellShoot = 235841
        };

        enum eEvents
        {
            EventShoot = 1
        };

        enum eActions
        {
            ActionStage1Completed,
            ActionStage3Completed = 2,
            ActionStage4Triggered
        };

        struct npc_granny_marlAI : public ScriptedAI
        {
            npc_granny_marlAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            Position const m_TeleportPos = { 3160.311f, 7344.003f, 269.0049f, 2.101546f };

            InstanceScript* m_Instance;

            bool m_Disabled = false;

            void Reset() override
            {
                me->m_CombatDistance = 45.0f;

                me->DisableEvadeMode();

                me->DisableHealthRegen();

                SetCombatMovement(false);

                me->SetPower(Powers::POWER_RAGE, 0);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                /// Don't reschedule events if it has already been done, NPCs do not reset
                if (events.Empty())
                    events.ScheduleEvent(eEvents::EventShoot, 7 * TimeConstants::IN_MILLISECONDS);

                if (!m_Instance)
                    return;

                if (p_Attacker->GetEntry() == eCreatures::CreatureJarodShadowsong || p_Attacker->GetEntry() == eCreatures::CreatureCallieCarrington)
                {
                    m_Disabled = false;
                    return;
                }

                if (Creature* l_Jarod = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureJarodShadowsong)))
                {
                    if (!l_Jarod->isInCombat() && l_Jarod->IsAIEnabled)
                        l_Jarod->AI()->AttackStart(p_Attacker);
                }

                if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                {
                    if (!l_Callie->isInCombat() && l_Callie->IsAIEnabled)
                        l_Callie->AI()->AttackStart(p_Attacker);
                }
            }

            void AttackStart(Unit* p_Target, bool /*p_Melee = true*/) override
            {
                if (p_Target && me->Attack(p_Target, false))
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveChase(p_Target, 20.0f);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionStage1Completed:
                    {
                        SetCombatMovement(true);

                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            Position const l_MovePos = { 3184.2095f, 7386.0186f, 224.8813f, 1.8729f };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(2, l_MovePos);
                        });

                        break;
                    }
                    case eActions::ActionStage3Completed:
                    {
                        me->RemoveAura(eCommonSpells::SpellFelCorruption);

                        DoCast(me, eCommonSpells::SpellFelTouched, true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->SendPlayHoverAnim(false);

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                SetFlyMode(false);

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MoveFall();

                                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, Anim::ANIM_KNEEL_LOOP);
                                    });
                                });
                            });
                        });

                        break;
                    }
                    case eActions::ActionStage4Triggered:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, 0);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 3162.410f, 7358.833f, 264.3277f },
                                { 3162.660f, 7360.083f, 264.3277f },
                                { 3162.910f, 7362.083f, 265.3277f },
                                { 3163.160f, 7363.833f, 265.5777f },
                                { 3163.410f, 7366.583f, 266.3277f },
                                { 3164.160f, 7370.583f, 267.5777f },
                                { 3164.410f, 7372.833f, 268.5777f },
                                { 3164.510f, 7373.663f, 268.6906f },
                                { 3164.745f, 7373.234f, 268.7958f },
                                { 3164.745f, 7373.984f, 269.0458f },
                                { 3165.495f, 7375.734f, 269.7958f },
                                { 3165.745f, 7376.984f, 270.2958f },
                                { 3166.245f, 7377.984f, 270.5458f },
                                { 3166.745f, 7379.234f, 271.2958f },
                                { 3166.995f, 7380.484f, 271.7958f },
                                { 3167.714f, 7381.979f, 272.0081f },
                                { 3167.902f, 7382.177f, 272.2943f },
                                { 3168.652f, 7383.177f, 272.0443f },
                                { 3169.402f, 7384.927f, 272.0443f },
                                { 3169.902f, 7385.927f, 272.0443f },
                                { 3171.402f, 7388.427f, 272.0443f },
                                { 3171.652f, 7389.427f, 272.0443f },
                                { 3171.902f, 7391.677f, 271.7943f },
                                { 3172.152f, 7393.427f, 271.2943f },
                                { 3172.402f, 7394.677f, 270.7943f },
                                { 3173.122f, 7398.850f, 270.6098f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothPath(5, l_Path.data(), l_Path.size(), false);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE && p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 2:
                    {
                        me->SetHomePosition(*me);
                        break;
                    }
                    case 3:
                    {
                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 3186.680f, 7404.230f, 231.7170f },
                                { 3188.553f, 7414.987f, 233.7177f },
                                { 3189.053f, 7421.987f, 233.2177f },
                                { 3174.803f, 7422.237f, 234.7177f },
                                { 3161.427f, 7419.244f, 239.7183f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(4, l_Path.data(), l_Path.size());
                        });

                        break;
                    }
                    case 4:
                    {
                        if (!m_Instance)
                            break;

                        if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                        {
                            if (GroupPtr l_Group = l_Player->GetGroup())
                                l_Group->RemoveCreatureMember(me);
                        }

                        me->NearTeleportTo(m_TeleportPos);
                        break;
                    }
                    case 5:
                    {
                        me->SetSheath(SheathState::SHEATH_STATE_RANGED);
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READYRIFLE);
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
                    case eCommonSpells::SpellFelCorruption:
                    {
                        m_Disabled = true;

                        me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        SetFlyMode(true);

                        me->SendPlayHoverAnim(true);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            Position l_Pos = me->GetPosition();

                            l_Pos.m_positionZ += 5.0f;

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(3, l_Pos);
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

                if (!UpdateVictim() || m_Disabled)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShoot:
                    {
                        DoCastVictim(eSpells::SpellShoot);

                        events.ScheduleEvent(eEvents::EventShoot, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_granny_marlAI(p_Creature);
        }
};

/// Callie Carrington - 118451
class npc_callie_carrington : public CreatureScript
{
    public:
        npc_callie_carrington() : CreatureScript("npc_callie_carrington") { }

        enum eSpells
        {
            SpellShadowstep     = 235980,

            SpellShadowstrike   = 235845
        };

        enum eEvents
        {
            EventShadowstep = 1,
            EventShadowstrike
        };

        enum eActions
        {
            ActionStage1Completed,
            ActionJarodDeath,
            ActionStage3Completed,
            ActionStage4Triggered
        };

        enum eTalks
        {
            TalkJarodDeath
        };

        struct npc_callie_carringtonAI : public ScriptedAI
        {
            npc_callie_carringtonAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            Position const m_TeleportPos = { 3163.327f, 7347.531f, 268.8984f, 2.634931f };

            InstanceScript* m_Instance;

            bool m_Disabled = false;

            void Reset() override
            {
                me->m_CombatDistance = 45.0f;

                me->DisableEvadeMode();

                me->DisableHealthRegen();

                me->SetPower(Powers::POWER_RAGE, 0);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                /// Don't reschedule events if it has already been done, NPCs do not reset
                if (events.Empty())
                {
                    events.ScheduleEvent(eEvents::EventShadowstep, 10 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventShadowstrike, 17 * TimeConstants::IN_MILLISECONDS);
                }

                if (!m_Instance)
                    return;

                if (p_Attacker->GetEntry() == eCreatures::CreatureJarodShadowsong || p_Attacker->GetEntry() == eCreatures::CreatureGrannyMarl)
                {
                    m_Disabled = false;
                    return;
                }

                if (Creature* l_Jarod = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureJarodShadowsong)))
                {
                    if (!l_Jarod->isInCombat() && l_Jarod->IsAIEnabled)
                        l_Jarod->AI()->AttackStart(p_Attacker);
                }

                if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                {
                    if (!l_Granny->isInCombat() && l_Granny->IsAIEnabled)
                        l_Granny->AI()->AttackStart(p_Attacker);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionStage1Completed:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            Position const l_MovePos = { 3177.7789f, 7393.041f, 224.8813f, 1.0875f };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(2, l_MovePos);
                        });

                        break;
                    }
                    case eActions::ActionJarodDeath:
                    {
                        Talk(eTalks::TalkJarodDeath);
                        break;
                    }
                    case eActions::ActionStage3Completed:
                    {
                        me->RemoveAura(eCommonSpells::SpellFelCorruption);

                        DoCast(me, eCommonSpells::SpellFelTouched, true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->SendPlayHoverAnim(false);

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                SetFlyMode(false);

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MoveFall();

                                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, Anim::ANIM_KNEEL_LOOP);
                                    });
                                });
                            });
                        });

                        break;
                    }
                    case eActions::ActionStage4Triggered:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, 0);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 3164.219f, 7357.733f, 264.2786f },
                                { 3163.719f, 7359.983f, 264.2786f },
                                { 3163.969f, 7361.983f, 265.2786f },
                                { 3163.719f, 7363.483f, 265.5286f },
                                { 3163.969f, 7364.483f, 265.5286f },
                                { 3164.219f, 7366.733f, 266.0286f },
                                { 3166.719f, 7368.483f, 267.2786f },
                                { 3167.111f, 7369.436f, 267.5923f },
                                { 3169.104f, 7373.021f, 269.3626f },
                                { 3170.854f, 7376.521f, 271.1126f },
                                { 3171.354f, 7377.771f, 271.6126f },
                                { 3171.604f, 7378.271f, 271.8626f },
                                { 3172.104f, 7379.271f, 272.1126f },
                                { 3172.604f, 7380.521f, 271.8626f },
                                { 3173.104f, 7381.771f, 271.8626f },
                                { 3174.699f, 7385.332f, 271.7647f },
                                { 3175.728f, 7386.537f, 272.0308f },
                                { 3176.978f, 7387.287f, 272.0308f },
                                { 3178.228f, 7389.037f, 271.7808f },
                                { 3179.478f, 7390.287f, 271.2808f },
                                { 3182.728f, 7393.787f, 271.0308f },
                                { 3183.000f, 7394.093f, 270.6801f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothPath(5, l_Path.data(), l_Path.size(), false);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE && p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 2:
                    {
                        me->SetHomePosition(*me);
                        break;
                    }
                    case 3:
                    {
                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 3186.140f, 7409.790f, 234.0220f },
                                { 3188.533f, 7415.017f, 233.6202f },
                                { 3189.033f, 7422.017f, 233.1202f },
                                { 3174.783f, 7422.267f, 234.6202f },
                                { 3161.427f, 7419.244f, 239.7183f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(4, l_Path.data(), l_Path.size());
                        });

                        break;
                    }
                    case 4:
                    {
                        if (!m_Instance)
                            break;

                        if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                        {
                            if (GroupPtr l_Group = l_Player->GetGroup())
                                l_Group->RemoveCreatureMember(me);
                        }

                        me->NearTeleportTo(m_TeleportPos);
                        break;
                    }
                    case 5:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY1H);
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
                    case eCommonSpells::SpellFelCorruption:
                    {
                        m_Disabled = true;

                        me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        SetFlyMode(true);

                        me->SendPlayHoverAnim(true);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            Position l_Pos = me->GetPosition();

                            l_Pos.m_positionZ += 5.0f;

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(3, l_Pos);
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

                if (!UpdateVictim() || m_Disabled)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShadowstep:
                    {
                        DoCastVictim(eSpells::SpellShadowstep);

                        events.ScheduleEvent(eEvents::EventShadowstep, 8 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventShadowstrike:
                    {
                        DoCastVictim(eSpells::SpellShadowstrike);

                        events.ScheduleEvent(eEvents::EventShadowstrike, 5 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_callie_carringtonAI(p_Creature);
        }
};

/// Damaged Soldier - 119476
/// Damaged Mage - 119477
/// Damaged Archer - 119480
class npc_damaged_spirits : public CreatureScript
{
    public:
        npc_damaged_spirits() : CreatureScript("npc_damaged_spirits") { }

        enum eSpells
        {
            SpellFelCorruption      = 237846,
            SpellFelCorruptionAura  = 237187,
            SpellGeneralTriggerSelf = 237874,
            SpellFullyCorrupted     = 237847
        };

        enum eTalk
        {
            TalkHealed
        };

        enum eGuids
        {
            GuidRemoveSpirit,
            GuidAddSpirit
        };

        struct npc_damaged_spiritsAI : public ScriptedAI
        {
            npc_damaged_spiritsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Healed = false;
            bool m_Corrupted = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->DisableHealthRegen();
                me->DisableEvadeMode();

                me->SetHealth(me->CountPctFromMaxHealth(10));

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFelCorruption:
                    {
                        DoCast(me, eSpells::SpellFelCorruptionAura);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                        me->SetWalk(true);

                        float l_Angle = me->GetAngle(p_Caster);

                        Position l_Pos = me->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 26.5f, l_Angle, true);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, l_Pos);
                        break;
                    }
                    case eSpells::SpellGeneralTriggerSelf:
                    {
                        if (m_Corrupted)
                            break;

                        m_Healed = true;

                        me->RemoveAura(eSpells::SpellFelCorruption);
                        me->RemoveAura(eSpells::SpellFelCorruptionAura);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                        me->StopMoving();

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveFall();

                        Talk(eTalk::TalkHealed);

                        me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_BOW);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetWalk(false);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(0, me->GetHomePosition());

                            me->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);
                        });

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Erdris = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureLordErdrisThorn)))
                            {
                                if (l_Erdris->IsAIEnabled)
                                {
                                    l_Erdris->AI()->SetGUID(me->GetGUID(), eGuids::GuidRemoveSpirit);
                                    l_Erdris->AI()->DoAction(1);
                                }
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 1:
                    {
                        if (m_Healed)
                            break;

                        m_Corrupted = true;

                        me->RemoveAura(eSpells::SpellFelCorruption);
                        me->RemoveAura(eSpells::SpellFelCorruptionAura);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        DoCast(me, eSpells::SpellFullyCorrupted, true);

                        SetFlyMode(true);

                        me->SetPlayerHoverAnim(true);
                        me->SendPlayHoverAnim(true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            Position l_Pos = me->GetPosition();

                            l_Pos.m_positionZ += 5.0f;

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveTakeoff(2, l_Pos);
                        });

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Erdris = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureLordErdrisThorn)))
                            {
                                if (l_Erdris->IsAIEnabled)
                                {
                                    l_Erdris->AI()->SetGUID(me->GetGUID(), eGuids::GuidRemoveSpirit);
                                    l_Erdris->AI()->SetGUID(me->GetGUID(), eGuids::GuidAddSpirit);
                                }
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                m_Healed = true;

                me->RemoveAura(eSpells::SpellFelCorruption);
                me->RemoveAura(eSpells::SpellFelCorruptionAura);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                me->StopMoving();

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveFall();

                Talk(eTalk::TalkHealed);

                me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_BOW);

                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(false);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, me->GetHomePosition());

                    me->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_damaged_spiritsAI(p_Creature);
        }
};

/// Corrupted Risen Soldier - 118489
class npc_corrupted_risen_soldier : public CreatureScript
{
    public:
        npc_corrupted_risen_soldier() : CreatureScript("npc_corrupted_risen_soldier") { }

        enum eSpells
        {
            SpellKnifeDance         = 235823,

            SpellFrenziedAssault    = 236720
        };

        enum eEvents
        {
            EventKnifeDance = 1,
            EventFrenziedAssault
        };

        struct npc_corrupted_risen_soldierAI : public ScriptedAI
        {
            npc_corrupted_risen_soldierAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->DisableHealthRegen();

                me->SetPower(Powers::POWER_RAGE, 0);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoZoneInCombat();

                events.ScheduleEvent(eEvents::EventKnifeDance, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFrenziedAssault, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE || p_ID != 1)
                    return;

                me->SetHomePosition(*me);

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY1H);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
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
                    case eEvents::EventKnifeDance:
                    {
                        DoCast(me, eSpells::SpellKnifeDance);

                        events.ScheduleEvent(eEvents::EventKnifeDance, 23 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFrenziedAssault:
                    {
                        DoCast(me, eSpells::SpellFrenziedAssault);

                        me->ClearUnitState(UnitState::UNIT_STATE_CASTING);

                        events.ScheduleEvent(eEvents::EventFrenziedAssault, 25 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_corrupted_risen_soldierAI(p_Creature);
        }
};

/// Corrupted Risen Mage - 118491
class npc_corrupted_risen_mage : public CreatureScript
{
    public:
        npc_corrupted_risen_mage() : CreatureScript("npc_corrupted_risen_mage") { }

        enum eSpell
        {
            SpellArcaneBlitz = 235833
        };

        enum eEvent
        {
            EventArcaneBlitz = 1
        };

        struct npc_corrupted_risen_mageAI : public ScriptedAI
        {
            npc_corrupted_risen_mageAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoZoneInCombat();

                events.ScheduleEvent(eEvent::EventArcaneBlitz, 1);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE || p_ID != 1)
                    return;

                me->SetHomePosition(*me);

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY1H);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
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
                    case eEvent::EventArcaneBlitz:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpell::SpellArcaneBlitz);

                        events.ScheduleEvent(eEvent::EventArcaneBlitz, 3 * TimeConstants::IN_MILLISECONDS + 500);
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
            return new npc_corrupted_risen_mageAI(p_Creature);
        }
};

/// Corrupted Risen Arbalest - 118492
/// Corrupted Risen Arbalest - 122397
class npc_corrupted_risen_arbalest : public CreatureScript
{
    public:
        npc_corrupted_risen_arbalest() : CreatureScript("npc_corrupted_risen_arbalest") { }

        enum eSpells
        {
            SpellDeadeyeShot    = 235837,
            SpellPhaseShift     = 236688,
            SpellManaSting      = 235984,
            SpellManaStingAura  = 235992
        };

        enum eEvents
        {
            EventDeadeyeShot = 1,
            EventManaSting
        };

        enum eEntry
        {
            NpcCorruptedArbalestStairs = 122397
        };

        struct npc_corrupted_risen_arbalestAI : public ScriptedAI
        {
            npc_corrupted_risen_arbalestAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            bool m_ManaSting = false;

            InstanceScript* m_Instance;

            void Reset() override
            {
                if (me->GetEntry() != eEntry::NpcCorruptedArbalestStairs)
                    me->SetReactState(ReactStates::REACT_PASSIVE);

                me->DisableHealthRegen();

                me->SetPower(Powers::POWER_RAGE, 0);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (me->GetEntry() != eEntry::NpcCorruptedArbalestStairs)
                    DoZoneInCombat();

                events.ScheduleEvent(eEvents::EventDeadeyeShot, 1 * TimeConstants::IN_MILLISECONDS + 400);

                if (me->GetEntry() != eEntry::NpcCorruptedArbalestStairs)
                    events.ScheduleEvent(eEvents::EventManaSting, 8 * TimeConstants::IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE || p_ID != 1)
                    return;

                me->SetHomePosition(*me);

                Position const l_FirstPos = { 3262.880f, 7295.610f, me->m_positionZ, 5.4757f };

                /// First Arbalest to attack has his weapon ready
                if (me->IsNearPosition(&l_FirstPos, 0.2f))
                {
                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READYBOW);

                    if (m_Instance)
                        m_Instance->SetData(eData::DataRisenThreatWaves, 0);
                }
                else
                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY2HL);

                me->SetSheath(SheathState::SHEATH_STATE_RANGED);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellManaSting:
                    {
                        m_ManaSting = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::SpellManaStingAura && !p_Targets.empty())
                {
                    p_Targets.sort(JadeCore::ObjectDistanceOrderPred(me));

                    /// Pickup nearest target
                    WorldObject* l_Target = p_Targets.front();

                    p_Targets.clear();
                    p_Targets.push_back(l_Target);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_ManaSting)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDeadeyeShot:
                    {
                        if (me->IsMoving())
                        {
                            SetCombatMovement(false);

                            me->StopMoving();

                            me->GetMotionMaster()->Clear();
                        }

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellDeadeyeShot);

                        events.ScheduleEvent(eEvents::EventDeadeyeShot, 1 * TimeConstants::IN_MILLISECONDS + 400);
                        break;
                    }
                    case eEvents::EventManaSting:
                    {
                        m_ManaSting = true;

                        DoCast(me, eSpells::SpellPhaseShift);

                        events.ScheduleEvent(eEvents::EventManaSting, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_corrupted_risen_arbalestAI(p_Creature);
        }
};

/// Dread Corruptor - 118487
class npc_dread_corruptor : public CreatureScript
{
    public:
        npc_dread_corruptor() : CreatureScript("npc_dread_corruptor") { }

        enum eSpells
        {
            SpellDemonSpawn     = 233240,
            SpellWrathOfKruul   = 237381,

            SpellMindSpike      = 235825,
            SpellBeamingGaze    = 235826,
            SpellBeamingGazeSum = 235828
        };

        enum eEvents
        {
            EventMindSpike = 1,
            EventBeamingGaze
        };

        enum eActions
        {
            ActionFight,
            ActionStage3Triggered
        };

        struct npc_dread_corruptorAI : public ScriptedAI
        {
            npc_dread_corruptorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            Position const m_TeleportPos = { 3155.696f, 7352.925f, 264.0049f, 5.37258f };

            InstanceScript* m_Instance;

            bool m_Fight = false;

            void InitializeAI() override
            {
                me->SetDisplayId(eData::DataInvisDisplay);
            }

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                SetCombatMovement(false);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionFight:
                    {
                        m_Fight = true;

                        if (!m_Instance)
                            break;

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_UNK_15 | eUnitFlags::UNIT_FLAG_UNK_16);

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        SetCombatMovement(true);

                        if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                            AttackStart(l_Player);

                        break;
                    }
                    case eActions::ActionStage3Triggered:
                    {
                        DoCast(me, eSpells::SpellDemonSpawn);

                        AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (!m_Instance)
                                return;

                            if (Creature* l_Granny = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureGrannyMarl)))
                                l_Granny->CastSpell(l_Granny, eCommonSpells::SpellFelCorruption, true);

                            if (Creature* l_Callie = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureCallieCarrington)))
                                l_Callie->CastSpell(l_Callie, eCommonSpells::SpellFelCorruption, true);
                        });

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                        {
                            if (!m_Instance)
                                return;

                            m_Instance->SetData(eData::DataRisenThreatEyes, 1);

                            if (Creature* l_Jarod = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureJarodShadowsong)))
                                l_Jarod->CastSpell(l_Jarod, eCommonSpells::SpellFelCorruption, true);

                            if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                                l_Player->CastSpell(l_Player, eSpells::SpellWrathOfKruul, true);
                        });

                        AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                            std::vector<G3D::Vector3> l_Path =
                            {
                                { 3186.176f, 7409.189f, 228.5207f },
                                { 3186.676f, 7412.939f, 228.2707f },
                                { 3187.176f, 7416.439f, 228.2707f },
                                { 3184.060f, 7421.576f, 228.5236f },
                                { 3182.560f, 7422.326f, 228.2736f },
                                { 3178.310f, 7425.576f, 228.5236f },
                                { 3175.810f, 7425.326f, 228.5236f },
                                { 3174.560f, 7425.326f, 228.5236f },
                                { 3174.227f, 7425.421f, 228.7021f },
                                { 3170.727f, 7425.171f, 229.2021f },
                                { 3166.977f, 7423.671f, 230.4521f },
                                { 3164.977f, 7422.671f, 231.2021f }
                            };

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothPath(1, l_Path.data(), l_Path.size(), false);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE || p_ID != 1)
                    return;

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->NearTeleportTo(m_TeleportPos);

                    me->SetHomePosition(m_TeleportPos);
                });
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSpells::SpellDemonSpawn)
                    me->RestoreDisplayId();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (!m_Instance)
                    return;

                m_Instance->SetData64(eData::DataRisenThreatEyes, me->GetGUID());

                std::list<Creature*> l_Eyes;

                me->GetCreatureListWithEntryInGrid(l_Eyes, eCreatures::CreatureFlickeringEye, 40.0f);

                for (Creature* l_Cre : l_Eyes)
                {
                    m_Instance->SetData64(eData::DataRisenThreatEyes, l_Cre->GetGUID());

                    if (l_Cre->IsAIEnabled)
                        l_Cre->AI()->DoAction(eActions::ActionFight);
                }

                events.ScheduleEvent(eEvents::EventMindSpike, 2 * TimeConstants::IN_MILLISECONDS + 400);
                events.ScheduleEvent(eEvents::EventBeamingGaze, 6 * TimeConstants::IN_MILLISECONDS + 200);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBeamingGaze)
                {
                    for (uint8 l_I = 0; l_I < 3; ++l_I)
                    {
                        Position l_Pos = p_Target->GetPosition();

                        l_Pos.GetRandomNearPosition(l_Pos, 5.0f, p_Target);

                        DoCast(l_Pos, eSpells::SpellBeamingGazeSum, true);
                    }
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                summons.DespawnAll();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Fight || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventMindSpike:
                    {
                        DoCastVictim(eSpells::SpellMindSpike);

                        events.ScheduleEvent(eEvents::EventMindSpike, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBeamingGaze:
                    {
                        DoCast(me, eSpells::SpellBeamingGaze);

                        events.ScheduleEvent(eEvents::EventBeamingGaze, 5 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_dread_corruptorAI(p_Creature);
        }
};

/// Flickering Eye - 119475
class npc_flickering_eye : public CreatureScript
{
    public:
        npc_flickering_eye() : CreatureScript("npc_flickering_eye") { }

        enum eSpells
        {
            SpellFlickering = 237183,
            SpellDetonate   = 237401
        };

        enum eAction
        {
            ActionActivateEye
        };

        struct npc_flickering_eyeAI : public ScriptedAI
        {
            npc_flickering_eyeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionActivateEye)
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        InstanceScript* l_Instance = me->GetInstanceScript();
                        if (!l_Instance)
                            return;

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (Player* l_Player = Player::GetPlayer(*me, l_Instance->GetData64(eData::DataPlayerGuid)))
                            AttackStart(l_Player);

                        DoCast(me, eSpells::SpellFlickering, true);
                    });
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellDetonate, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_flickering_eyeAI(p_Creature);
        }
};

/// Felspite Dominator - 119472
class npc_felspite_dominator_119472 : public CreatureScript
{
    public:
        npc_felspite_dominator_119472() : CreatureScript("npc_felspite_dominator_119472") { }

        enum eSpell
        {
            SpellBatFrenzy = 237172
        };

        enum eEvent
        {
            EventBatFrenzy = 1
        };

        struct npc_felspite_dominator_119472AI : public ScriptedAI
        {
            npc_felspite_dominator_119472AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvent::EventBatFrenzy, 8 * TimeConstants::IN_MILLISECONDS + 500);
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
                    case eEvent::EventBatFrenzy:
                    {
                        DoCast(me, eSpell::SpellBatFrenzy);
                        events.ScheduleEvent(eEvent::EventBatFrenzy, 30 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_felspite_dominator_119472AI(p_Creature);
        }
};

/// Unstable Fel Orb - 119473
class npc_unstable_fel_orb : public CreatureScript
{
    public:
        npc_unstable_fel_orb() : CreatureScript("npc_unstable_fel_orb") { }

        enum eSpells
        {
            SpellUnstableFelExplosionAura   = 237173,
            SpellUnstableFelExplosionProc   = 237174,
            SpellUnstableFelExplosionAT     = 237654
        };

        struct npc_unstable_fel_orbAI : public ScriptedAI
        {
            npc_unstable_fel_orbAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Disabled = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellUnstableFelExplosionAura, true);
                DoCast(me, eSpells::SpellUnstableFelExplosionAT, true);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveRandom(10.0f);
                });
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Disabled)
                {
                    p_Damage = 0;
                    return;
                }

                me->StopMoving();

                me->GetMotionMaster()->Clear();

                m_Disabled = true;

                p_Damage = 0;

                DoCast(me, eSpells::SpellUnstableFelExplosionProc, true);

                me->RemoveAllAreasTrigger();

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSpells::SpellUnstableFelExplosionProc)
                {
                    m_Disabled = false;

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    DoCast(me, eSpells::SpellUnstableFelExplosionAT, true);

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveRandom(10.0f);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_unstable_fel_orbAI(p_Creature);
        }
};

/// Panicked Soul - 119471
class npc_panicked_soul : public CreatureScript
{
    public:
        npc_panicked_soul() : CreatureScript("npc_panicked_soul") { }

        enum eSpell
        {
            SpellIncitePanic = 237178
        };

        enum eTalk
        {
            TalkDispel
        };

        struct npc_panicked_soulAI : public ScriptedAI
        {
            npc_panicked_soulAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                DoCast(me, eSpell::SpellIncitePanic, true);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode != AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL || p_SpellID != eSpell::SpellIncitePanic)
                    return;

                Talk(eTalk::TalkDispel);

                me->GetMotionMaster()->Clear();

                me->StopMoving();
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_panicked_soulAI(p_Creature);
        }
};

/// Damaged Soul - 119470
class npc_damaged_soul : public CreatureScript
{
    public:
        npc_damaged_soul() : CreatureScript("npc_damaged_soul") { }

        enum eSpells
        {
            SpellBlueGhost      = 237848,
            SpellFelCorruption  = 237176
        };

        enum eTalks
        {
            TalkFire,
            TalkHealed,
            TalkRest
        };

        struct npc_damaged_soulAI : public ScriptedAI
        {
            npc_damaged_soulAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->DisableHealthRegen();

                me->SetHealth(1662827);

                DoCast(me, eSpells::SpellBlueGhost, true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellFelCorruption)
                    Talk(eTalks::TalkFire);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_SpellID != eSpells::SpellFelCorruption || p_Mode != AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                Talk(eTalks::TalkHealed);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_BOW);

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::TalkRest);
                    });
                });
            }

            void DoAction(int32 const p_Action) override
            {
                DoCast(me, eSpells::SpellFelCorruption, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->GetHealthPct() >= 99.0f)
                    me->RemoveAura(eSpells::SpellFelCorruption, 0, 0, AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_damaged_soulAI(p_Creature);
        }
};

/// Beaming Eye - 118839
class npc_beaming_eye : public CreatureScript
{
    public:
        npc_beaming_eye() : CreatureScript("npc_beaming_eye") { }

        enum eSpells
        {
            SpellBeamingEyeAT = 235831
        };

        struct npc_beaming_eyeAI : public ScriptedAI
        {
            npc_beaming_eyeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetHover(true);
                me->SetPlayerHoverAnim(true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellBeamingEyeAT);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBeamingEyeAT)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        MoveRotate();
                    });
                }
            }

            void LastOperationCalled() override
            {
                AddTimedDelayedOperation(1, [this]() -> void
                {
                    MoveRotate();
                });
            }

            void MoveRotate()
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveRotate(20 * TimeConstants::IN_MILLISECONDS, RotateDirection::ROTATE_DIRECTION_LEFT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_beaming_eyeAI(p_Creature);
        }
};

/// Mana Sting - 235984
class spell_jarod_mana_sting : public SpellScriptLoader
{
    public:
        spell_jarod_mana_sting() : SpellScriptLoader("spell_jarod_mana_sting") { }

        enum eSpell
        {
            SpellManaSting = 235992
        };

        class spell_jarod_mana_sting_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_jarod_mana_sting_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                PreventDefaultAction();

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, eSpell::SpellManaSting, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_jarod_mana_sting_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_jarod_mana_sting_AuraScript();
        }
};

/// Wrath of Kruul - 237381
class spell_jarod_wrath_of_kruul : public SpellScriptLoader
{
    public:
        spell_jarod_wrath_of_kruul() : SpellScriptLoader("spell_jarod_wrath_of_kruul") { }

        enum eAction
        {
            ActionActivateEye
        };

        enum eBroadcast
        {
            BroadcastID = 128000
        };

        class spell_jarod_wrath_of_kruul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_jarod_wrath_of_kruul_AuraScript);

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (!GetCaster() || !GetCaster()->IsPlayer())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                    l_Player->SetRooted(true);
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (!GetCaster() || !GetCaster()->IsPlayer())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    l_Player->SetRooted(false);

                    PlayerTextBuilder l_CreatureTextBuilder(l_Player, ChatMsg::CHAT_MSG_RAID_BOSS_EMOTE, eBroadcast::BroadcastID, Language::LANG_UNIVERSAL);
                    CreatureTextLocalizer<PlayerTextBuilder> l_CreatureTextLocalizer(l_CreatureTextBuilder, ChatMsg::CHAT_MSG_RAID_BOSS_EMOTE);

                    l_CreatureTextLocalizer(l_Player);

                    if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
                    {
                        l_Instance->SetData(eData::DataRisenThreatTimer, 5 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);

                        Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Instance->instance->GetScenarioGuid());

                        if (!l_Scenario)
                            return;

                        l_Scenario->StartCriteriaTimer(CriteriaTimedTypes::CRITERIA_TIMED_TYPE_EVENT2, eData::DataStage3Timer);
                    }

                    std::list<Creature*> l_Eyes;

                    l_Player->GetCreatureListWithEntryInGrid(l_Eyes, eCreatures::CreatureFlickeringEye, 40.0f);

                    for (Creature* l_Cre : l_Eyes)
                    {
                        if (l_Cre->IsAIEnabled && l_Cre->m_positionZ <= 227.0f)
                            l_Cre->AI()->DoAction(eAction::ActionActivateEye);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_jarod_wrath_of_kruul_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_jarod_wrath_of_kruul_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_jarod_wrath_of_kruul_AuraScript();
        }
};

/// Ignite Soul - 237188
class spell_erdris_ignite_soul : public SpellScriptLoader
{
    public:
        spell_erdris_ignite_soul() : SpellScriptLoader("spell_erdris_ignite_soul") { }

        enum eSpell
        {
            SpellIgniteSoulAoE = 237189
        };

        class spell_erdris_ignite_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_erdris_ignite_soul_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpell::SpellIgniteSoulAoE, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_erdris_ignite_soul_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_erdris_ignite_soul_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_end_of_the_risen_threat()
{
    /// Boss
    new boss_lord_erdris_thorn();

    /// Allies
    new npc_commander_jarod_shadowsong();
    new npc_granny_marl();
    new npc_callie_carrington();
    new npc_damaged_spirits();

    /// Enemies
    new npc_corrupted_risen_soldier();
    new npc_corrupted_risen_mage();
    new npc_corrupted_risen_arbalest();
    new npc_dread_corruptor();
    new npc_flickering_eye();
    new npc_felspite_dominator_119472();
    new npc_unstable_fel_orb();
    new npc_panicked_soul();
    new npc_damaged_soul();
    new npc_beaming_eye();

    /// Spells
    new spell_jarod_mana_sting();
    new spell_jarod_wrath_of_kruul();
    new spell_erdris_ignite_soul();
}
#endif
