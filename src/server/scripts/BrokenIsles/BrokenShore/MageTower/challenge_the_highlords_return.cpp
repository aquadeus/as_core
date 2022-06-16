////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

std::array<Position const, eData::DataMaxPortals> g_PortalPos =
{
    {
        { 3952.930f, -2416.960f, 98.83798f, 0.4784193f },
        { 4017.862f, -2375.644f, 98.83797f, 3.7891810f },
        { 3951.425f, -2385.833f, 98.83797f, 5.8249780f },
        { 4022.642f, -2426.399f, 98.83799f, 2.5199180f }
    }
};

/// Highlord Kruul - 117198
class boss_highlord_kruul : public CreatureScript
{
    public:
        boss_highlord_kruul() : CreatureScript("boss_highlord_kruul") { }

        enum eConversations
        {
            ConversationIntro = 4236,
            ConversationOutro = 4246
        };

        enum eSpells
        {
            SpellDarkReformation            = 233449,
            SpellFelChannel                 = 233446,
            SpellSoulMissile                = 233447,
            SpellSoulRip                    = 190670,

            SpellShadowSweepDummy           = 234920,
            SpellShadowSweepSearcher        = 234922,

            SpellVoidBolt                   = 234680,

            SpellNetherstompSearcher        = 234672,
            SpellNetherstompJump            = 234673,
            SpellNetherstompAT              = 234674,

            SpellNetherAberration           = 235110,

            SpellAnnihilate                 = 236572,

            SpellTwistedReflection          = 234676,

            SpellGiftOfSargerasVisual       = 233458,
            SpellGiftOfSargerasDummy        = 233459,
            SpellGiftOfSargerasLast         = 233460,
            SpellGiftOfSargerasPeriodic1    = 233462,
            SpellGiftOfSargerasPeriodic2    = 233464,
            SpellGiftOfSargerasPeriodic3    = 233466,
            SpellGiftOfSargerasPeriodic4    = 233468,
            SpellGiftOfSargerasScreenEffect = 233469,
            SpellGazeOfSargeras             = 233457,

            SpellCameraShake10s             = 208447
        };

        enum eEvents
        {
            EventShadowSweep = 1,
            EventVoidBolt,
            EventNetherstomp,
            EventNetherAberration,
            EventAnnihilate,
            EventTwistedReflection
        };

        enum eTalks
        {
            TalkAnnihilate,
            TalkTwistedReflection
        };

        struct boss_highlord_kruulAI : public BossAI
        {
            boss_highlord_kruulAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            std::queue<uint64> m_TrashesToKill;

            bool m_FightEnded = false;

            Position const m_EndPos = { 3986.01f, -2410.51f, 98.78467f, 1.48353f };

            void Reset() override
            {
                uint32 l_Time = 14 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    if (!instance)
                        return;

                    if (Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid)))
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::ConversationIntro, l_Player, nullptr, *l_Player))
                            delete l_Conversation;
                    }
                });

                l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    me->SetAIAnimKitId(0);

                    me->SetSheath(SheathState::SHEATH_STATE_MELEE);

                    DoCast(me, eSpells::SpellFelChannel);

                    std::list<Creature*> l_TrashList;

                    me->GetCreatureListWithEntryInGrid(l_TrashList, eCreatures::CreatureEmpoweredDoomherald, 70.0f);
                    me->GetCreatureListWithEntryInGridAppend(l_TrashList, eCreatures::CreatureDarkbladeChampion, 70.0f);

                    for (Creature* l_Creature : l_TrashList)
                        m_TrashesToKill.push(l_Creature->GetGUID());

                    if (m_TrashesToKill.empty() || m_TrashesToKill.size() != 13)
                        return;

                    auto l_SoulMissile = [](Creature* p_Creature) -> void
                    {
                        p_Creature->CastSpell(p_Creature, eSpells::SpellSoulMissile, true);
                        p_Creature->DelayedCastSpell(p_Creature, eSpells::SpellSoulRip, true, 1);

                        p_Creature->DespawnOrUnsummon(17 * TimeConstants::IN_MILLISECONDS);
                    };

                    uint32 l_Time = 1;
                    AddTimedDelayedOperation(l_Time, [this, l_SoulMissile]() -> void
                    {
                        for (uint8 l_I = 0; l_I < 5; ++l_I)
                        {
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_TrashesToKill.front()))
                                l_SoulMissile(l_Creature);

                            m_TrashesToKill.pop();
                        }
                    });

                    l_Time += 1 * TimeConstants::IN_MILLISECONDS + 900;
                    AddTimedDelayedOperation(l_Time, [this, l_SoulMissile]() -> void
                    {
                        if (Creature* l_Creature = Creature::GetCreature(*me, m_TrashesToKill.front()))
                            l_SoulMissile(l_Creature);

                        m_TrashesToKill.pop();
                    });

                    l_Time += 500;
                    AddTimedDelayedOperation(l_Time, [this, l_SoulMissile]() -> void
                    {
                        for (uint8 l_I = 0; l_I < 7; ++l_I)
                        {
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_TrashesToKill.front()))
                                l_SoulMissile(l_Creature);

                            m_TrashesToKill.pop();
                        }
                    });

                    AddTimedDelayedOperation(14 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->RemoveAura(eSpells::SpellDarkReformation);

                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 4.0f);

                        SetFlyMode(true);

                        me->SendPlayHoverAnim(true);

                        Position l_Pos = me->GetPosition();

                        l_Pos.m_positionZ += 15.0f;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveTakeoff(0, l_Pos, Movement::AnimType::FlyToGround);
                    });
                });

                l_Time += 9 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    if (!instance)
                        return;

                    if (Creature* l_Korvas = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureKorvasBloodthorn)))
                    {
                        l_Korvas->GetMotionMaster()->Clear();
                        l_Korvas->GetMotionMaster()->MovePoint(1, { 3983.69f, -2395.16f, 98.78467f, 0.0f });
                    }
                });

                l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    if (!instance)
                        return;

                    if (Creature* l_Velen = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureProphetVelen)))
                    {
                        l_Velen->GetMotionMaster()->Clear();
                        l_Velen->GetMotionMaster()->MovePoint(1, { 3986.68f, -2393.1f, 98.78467f, 0.0f });
                    }
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventShadowSweep, 2 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventVoidBolt, 2 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventNetherstomp, 8 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventNetherAberration, 11 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventAnnihilate, 14 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTwistedReflection, 19 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAllAreasTrigger();
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (m_FightEnded)
                {
                    p_Damage = 0;
                    return;
                }

                if (p_Damage >= me->GetHealth())
                {
                    m_FightEnded = true;

                    events.Reset();

                    p_Damage = me->GetHealth() - 1;

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    if (Creature* l_Korvas = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureKorvasBloodthorn)))
                        l_Korvas->StopAttack();

                    me->StopAttack();

                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJump(m_EndPos, 30.0f, 10.0f, 3);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                /// Variss died
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveJump({ 3990.71f, -2418.97f, 98.6777f, 0.0f }, 20.0f, 10.0f, 2);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == MovementGeneratorType::EFFECT_MOTION_TYPE)
                {
                    switch (p_ID)
                    {
                        case 2:
                        {
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                SetFlyMode(false);

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    me->SendPlayHoverAnim(false);

                                    if (!instance)
                                        return;

                                    if (Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid)))
                                        AttackStart(l_Player);

                                    if (Creature* l_Korvas = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureKorvasBloodthorn)))
                                    {
                                        if (l_Korvas->IsAIEnabled)
                                            l_Korvas->AI()->DoAction(0);
                                    }
                                });
                            });

                            break;
                        }
                        case 3:
                        {
                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                me->SetFacingTo(m_EndPos.m_orientation);

                                DoCast(me, eSpells::SpellGiftOfSargerasVisual);

                                if (instance)
                                {
                                    if (Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid)))
                                    {
                                        Conversation* l_Conversation = new Conversation;
                                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::ConversationIntro, l_Player, nullptr, *l_Player))
                                            delete l_Conversation;
                                    }
                                }

                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    std::list<Creature*> l_List;

                                    me->GetCreatureListWithEntryInGrid(l_List, eCreatures::CreatureGenericBunny, 150.0f);

                                    for (Creature* l_Creature : l_List)
                                        l_Creature->CastSpell(l_Creature, eSpells::SpellGazeOfSargeras, true);
                                });

                                AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                                {
                                    DoCast(me, eSpells::SpellGiftOfSargerasDummy, true);
                                    DoCast(me, eSpells::SpellCameraShake10s, true);
                                });

                                AddTimedDelayedOperation(17 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                                {
                                    DoCast(me, eSpells::SpellGiftOfSargerasLast, true);

                                    me->AddAura(eSpells::SpellGiftOfSargerasPeriodic1, me);
                                    me->AddAura(eSpells::SpellGiftOfSargerasPeriodic2, me);
                                    me->AddAura(eSpells::SpellGiftOfSargerasPeriodic3, me);
                                    me->AddAura(eSpells::SpellGiftOfSargerasPeriodic4, me);

                                    AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        if (instance)
                                            instance->DoKilledMonsterKredit(eData::DataQuestID, me->GetEntry());

                                        DoCast(me, eSpells::SpellGiftOfSargerasScreenEffect, true);
                                    });
                                });
                            });

                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellShadowSweepDummy:
                    {
                        DoCast(me, eSpells::SpellShadowSweepSearcher, true);
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
                    case eSpells::SpellNetherstompSearcher:
                    {
                        Position l_Pos = p_Target->GetPosition();

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(l_Pos, 30.0f, 10.0f, eSpells::SpellNetherstompJump, eSpells::SpellNetherstompAT);
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
                    case eEvents::EventShadowSweep:
                    {
                        DoCast(me, eSpells::SpellShadowSweepDummy);

                        events.ScheduleEvent(eEvents::EventShadowSweep, 21 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventVoidBolt:
                    {
                        DoCastVictim(eSpells::SpellVoidBolt);

                        events.ScheduleEvent(eEvents::EventVoidBolt, urand(30, 40) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventNetherstomp:
                    {
                        DoCast(me, eSpells::SpellNetherstompSearcher, true);

                        events.ScheduleEvent(eEvents::EventNetherstomp, 16 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventNetherAberration:
                    {
                        DoCast(me, eSpells::SpellNetherAberration, true);

                        for (Position const& l_Pos : g_PortalPos)
                            me->SummonCreature(eCreatures::CreaturePortal, l_Pos);

                        events.ScheduleEvent(eEvents::EventNetherAberration, urand(40, 46) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAnnihilate:
                    {
                        Talk(eTalks::TalkAnnihilate);

                        DoCastVictim(eSpells::SpellAnnihilate);

                        events.ScheduleEvent(eEvents::EventAnnihilate, 28 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTwistedReflection:
                    {
                        Talk(eTalks::TalkTwistedReflection);

                        DoCastVictim(eSpells::SpellTwistedReflection);

                        events.ScheduleEvent(eEvents::EventTwistedReflection, urand(60, 70) * TimeConstants::IN_MILLISECONDS);
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
            return new boss_highlord_kruulAI(p_Creature);
        }
};

/// Inquisitor Variss - 117933
class boss_inquisitor_variss : public CreatureScript
{
    public:
        boss_inquisitor_variss() : CreatureScript("boss_inquisitor_variss") { }

        enum eSpells
        {
            SpellAuraOfDecay        = 234421,

            SpellMindRend           = 234401,

            SummonTormentingEye     = 234428,

            SpellDrainLife          = 234423,

            SpellNetherAberration   = 235110,

            SpellSmolderingInfernal = 235112
        };

        enum eEvents
        {
            EventMindRend = 1,
            EventTormentingEye,
            EventDrainLife,
            EventNetherAberration,
            EventSmolderingInfernal
        };

        enum eTalk
        {
            TalkDrainLife
        };

        enum eConversation
        {
            ConversationP2 = 4242
        };

        struct boss_inquisitor_varissAI : public BossAI
        {
            boss_inquisitor_varissAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            bool m_Engaged = false;

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                SetCombatMovement(false);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (m_Engaged || !instance)
                    return;

                Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid));
                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());

                if (!l_Scenario || !l_Player)
                    return;

                m_Engaged = true;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataFirstStep, 0, 0, l_Player, l_Player);

                _EnterCombat();

                DoZoneInCombat();

                DoCast(me, eSpells::SpellAuraOfDecay, true);

                events.ScheduleEvent(eEvents::EventMindRend, 1);
                events.ScheduleEvent(eEvents::EventTormentingEye, 5 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDrainLife, 10 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventNetherAberration, 20 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSmolderingInfernal, 52 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->DespawnCreaturesInArea(eCreatures::CreatureTormentingEye, 150.0f);

                if (!instance)
                    return;

                Player* l_Player = Player::GetPlayer(*me, instance->GetData64(eData::DataPlayerGuid));
                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());

                if (!l_Scenario || !l_Player)
                    return;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataSecondStep1, 0, 0, l_Player, l_Player);
                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataSecondStep2, 0, 0, l_Player, l_Player);

                Conversation* l_Conversation = new Conversation;
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversation::ConversationP2, l_Player, nullptr, *l_Player))
                    delete l_Conversation;

                if (Creature* l_Kruul = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureHighlordKruul)))
                {
                    if (l_Kruul->IsAIEnabled)
                        l_Kruul->AI()->DoAction(0);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE || p_ID != 1)
                    return;

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_UNK_15);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
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
                    case eEvents::EventMindRend:
                    {
                        DoCastVictim(eSpells::SpellMindRend);

                        events.ScheduleEvent(eEvents::EventMindRend, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTormentingEye:
                    {
                        DoCast(me, eSpells::SummonTormentingEye, true);

                        events.ScheduleEvent(eEvents::EventTormentingEye, urand(13, 17) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventDrainLife:
                    {
                        DoCast(me, eSpells::SpellDrainLife);

                        Talk(eTalk::TalkDrainLife);

                        events.ScheduleEvent(eEvents::EventDrainLife, urand(28, 32) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventNetherAberration:
                    {
                        DoCast(me, eSpells::SpellNetherAberration, true);

                        for (Position const& l_Pos : g_PortalPos)
                            me->SummonCreature(eCreatures::CreaturePortal, l_Pos);

                        events.ScheduleEvent(eEvents::EventNetherAberration, urand(40, 46) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSmolderingInfernal:
                    {
                        DoCast(me, eSpells::SpellSmolderingInfernal, true);

                        events.ScheduleEvent(eEvents::EventSmolderingInfernal, urand(60, 120) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_inquisitor_varissAI(p_Creature);
        }
};

/// Prophet Velen - 117042
class npc_kruul_prophet_velen : public CreatureScript
{
    public:
        npc_kruul_prophet_velen() : CreatureScript("npc_kruul_prophet_velen") { }

        enum eSpells
        {
            SpellHolyWardAT = 233473,

            SpellSmite      = 239736
        };

        enum eEvents
        {
            EventHolyWard = 1,
            EventSmite
        };

        enum eVisual
        {
            VisualKitSpawn = 12040
        };

        enum eTalks
        {
            TalkIntro
        };

        struct npc_kruul_prophet_velenAI : public ScriptedAI
        {
            npc_kruul_prophet_velenAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                ApplyAllImmunities(true);

                me->DisableEvadeMode();

                me->DisableHealthRegen();

                SetCombatMovement(false);

                AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetVisible(true);

                    me->SendPlaySpellVisualKit(eVisual::VisualKitSpawn, 0);

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (!m_Instance)
                            return;

                        if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                        {
                            if (GroupPtr l_Group = l_Player->GetGroup())
                                l_Group->AddCreatureMember(me);
                        }
                    });
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (!m_Instance)
                    return;

                events.ScheduleEvent(eEvents::EventHolyWard, 10 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSmite, 20 * TimeConstants::IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE || p_ID != 1)
                    return;

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetFacingTo(4.694936f);

                    Talk(eTalks::TalkIntro);
                });
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
                    case eEvents::EventHolyWard:
                    {
                        DoCast(me, eSpells::SpellHolyWardAT, false);

                        events.ScheduleEvent(eEvents::EventHolyWard, 40 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSmite:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellSmite);

                        events.ScheduleEvent(eEvents::EventSmite, urand(4, 5) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kruul_prophet_velenAI(p_Creature);
        }
};

/// Kor'vas Bloodthorn <Illidari> - 116880
class npc_kruul_korvas_bloodthorn : public CreatureScript
{
    public:
        npc_kruul_korvas_bloodthorn() : CreatureScript("npc_kruul_korvas_bloodthorn") { }

        enum eSpells
        {
            SpellWrathOfKruul   = 233472,

            SpellFixate         = 243461,

            SpellFelRushCharge  = 188669,
            SpellFelRushDamage  = 192611,

            SpellThrowGlaive    = 188667
        };

        enum eEvents
        {
            EventThrowGlaive = 1,
            EventFelRush
        };

        enum eVisual
        {
            VisualKitSpawn = 12040
        };

        struct npc_kruul_korvas_bloodthornAI : public ScriptedAI
        {
            npc_kruul_korvas_bloodthornAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                SetCombatMovement(false);

                ApplyAllImmunities(true);

                me->DisableEvadeMode();

                me->DisableHealthRegen();

                me->SetPower(Powers::POWER_RAGE, 0);

                AddTimedDelayedOperation(11 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                {
                    me->SetVisible(true);

                    me->SendPlaySpellVisualKit(eVisual::VisualKitSpawn, 0);

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (!m_Instance)
                            return;

                        if (Player* l_Player = Player::GetPlayer(*me, m_Instance->GetData64(eData::DataPlayerGuid)))
                        {
                            if (GroupPtr l_Group = l_Player->GetGroup())
                                l_Group->AddCreatureMember(me);
                        }
                    });
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (!m_Instance)
                    return;

                DoCast(p_Attacker, eSpells::SpellThrowGlaive);
                DoCast(me, eSpells::SpellFixate, true);

                me->ClearUnitState(UnitState::UNIT_STATE_CASTING);

                events.ScheduleEvent(eEvents::EventThrowGlaive, 8 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventFelRush, 1 * TimeConstants::IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_Type)
                {
                    case MovementGeneratorType::POINT_MOTION_TYPE:
                    {
                        if (p_ID != 1)
                            break;

                        if (m_Instance)
                        {
                            if (Creature* l_Variss = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureInquisitorVariss)))
                            {
                                l_Variss->HandleEmoteCommand(Emote::EMOTE_ONESHOT_SPELL_CAST);

                                l_Variss->SetFacingTo(1.448623f);
                            }

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS + 700, [this]() -> void
                            {
                                if (!m_Instance)
                                    return;

                                if (Creature* l_Variss = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureInquisitorVariss)))
                                {
                                    l_Variss->GetMotionMaster()->Clear();
                                    l_Variss->GetMotionMaster()->MovePoint(1, { 3989.44f, -2421.25f, 98.78466f, 0.0f });
                                }
                            });
                        }

                        DoCast(me, eSpells::SpellWrathOfKruul, true);

                        SetFlyMode(true);

                        me->SendPlayHoverAnim(true);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Position l_Pos = me->GetPosition();

                            l_Pos.m_positionZ += 5.0f;

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveTakeoff(2, l_Pos, Movement::AnimType::FlyToGround);
                        });

                        break;
                    }
                    case MovementGeneratorType::EFFECT_MOTION_TYPE:
                    {
                        if (p_ID != 3)
                            break;

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        SetCombatMovement(true);

                        if (!m_Instance)
                            break;

                        if (Creature* l_Kruul = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureHighlordKruul)))
                            AttackStart(l_Kruul);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                AddTimedDelayedOperation(1, [this]() -> void
                {
                    SetFlyMode(false);

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        me->SendPlayHoverAnim(false);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveFall(3);

                        me->RemoveAura(eSpells::SpellWrathOfKruul);
                    });
                });
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellFixate:
                    {
                        p_Targets.clear();

                        if (!m_Instance)
                            break;

                        if (Creature* l_Kruul = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::CreatureHighlordKruul)))
                            p_Targets.push_back(l_Kruul);

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
                    case eSpells::SpellFixate:
                    {
                        DoCast(p_Target, eSpells::SpellFelRushCharge, true);
                        DoCast(p_Target, eSpells::SpellFelRushDamage, true);
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
                    case eEvents::EventThrowGlaive:
                    {
                        DoCastVictim(eSpells::SpellThrowGlaive);

                        events.ScheduleEvent(eEvents::EventThrowGlaive, 8 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventFelRush:
                    {
                        if (Unit* l_Victim = me->getVictim())
                        {
                            if (me->GetDistance(l_Victim) >= 10.0f)
                            {
                                DoCast(l_Victim, eSpells::SpellFelRushCharge, true);
                                DoCast(l_Victim, eSpells::SpellFelRushDamage, true);

                                events.ScheduleEvent(eEvents::EventFelRush, 5 * TimeConstants::IN_MILLISECONDS);
                                break;
                            }
                        }

                        events.ScheduleEvent(eEvents::EventFelRush, 1 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_kruul_korvas_bloodthornAI(p_Creature);
        }
};

/// Tormenting Eye - 118032
class npc_kruul_tormenting_eye : public CreatureScript
{
    public:
        npc_kruul_tormenting_eye() : CreatureScript("npc_kruul_tormenting_eye") { }

        enum eSpells
        {
            SpellInquisitiveStare       = 234425,
            SpellTormentingEyeImmune    = 240548
        };

        enum eEvent
        {
            EventInquisitiveStare = 1
        };

        struct npc_kruul_tormenting_eyeAI : public Scripted_NoMovementAI
        {
            npc_kruul_tormenting_eyeAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (Unit* l_Victim = p_Summoner->getVictim())
                    AttackStart(l_Victim, false);

                SetCombatMovement(false);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                events.ScheduleEvent(eEvent::EventInquisitiveStare, 6 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetData(eData::DataPlayerClass) == Classes::CLASS_DRUID)
                        DoCast(me, eSpells::SpellTormentingEyeImmune, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (events.ExecuteEvent() == eEvent::EventInquisitiveStare)
                {
                    DoCast(me, eSpells::SpellInquisitiveStare);

                    events.ScheduleEvent(eEvent::EventInquisitiveStare, 16 * TimeConstants::IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kruul_tormenting_eyeAI(p_Creature);
        }
};

/// Portal - 118452
class npc_kruul_portal : public CreatureScript
{
    public:
        npc_kruul_portal() : CreatureScript("npc_kruul_portal") { }

        enum eSpell
        {
            SpellSummonAberration = 235108
        };

        struct npc_kruul_portalAI : public Scripted_NoMovementAI
        {
            npc_kruul_portalAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpell::SpellSummonAberration);
                });

                me->DespawnOrUnsummon(10 * TimeConstants::IN_MILLISECONDS + 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kruul_portalAI(p_Creature);
        }
};

/// Nether Horror - 118044
class npc_kruul_nether_horror : public CreatureScript
{
    public:
        npc_kruul_nether_horror() : CreatureScript("npc_kruul_nether_horror") { }

        enum eSpells
        {
            SpellNetherStorm        = 240790,
            SpellNetherStormNotMove = 240763,

            SpellPsytalons          = 234461
        };

        enum eEvents
        {
            EventNetherStorm = 1,
            EventPsytalons
        };

        struct npc_kruul_nether_horrorAI : public ScriptedAI
        {
            npc_kruul_nether_horrorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();
                if (!l_Instance)
                    return;

                me->SetInCombatWithZone();

                if (Creature* l_Velen = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureProphetVelen)))
                    AttackStart(l_Velen);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventPsytalons, 5 * TimeConstants::IN_MILLISECONDS);

                /// No Nether Storm for Paladins, Warriors and Monks tank specs
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetData(eData::DataPlayerClass) == Classes::CLASS_PALADIN ||
                        l_Instance->GetData(eData::DataPlayerClass) == Classes::CLASS_WARRIOR ||
                        l_Instance->GetData(eData::DataPlayerClass) == Classes::CLASS_MONK)
                        return;
                }

                events.ScheduleEvent(eEvents::EventNetherStorm, 10 * TimeConstants::IN_MILLISECONDS);
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
                    case eEvents::EventNetherStorm:
                    {
                        InstanceScript* l_Instance = me->GetInstanceScript();
                        if (!l_Instance)
                            break;

                        if (l_Instance->GetData(eData::DataPlayerClass) == Classes::CLASS_DEATH_KNIGHT)
                            DoCast(me, eSpells::SpellNetherStormNotMove);
                        else
                            DoCast(me, eSpells::SpellNetherStorm);

                        events.ScheduleEvent(eEvents::EventNetherStorm, urand(15, 20) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventPsytalons:
                    {
                        DoCastVictim(eSpells::SpellPsytalons);

                        events.ScheduleEvent(eEvents::EventPsytalons, 3 * TimeConstants::IN_MILLISECONDS + 500);
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
            return new npc_kruul_nether_horrorAI(p_Creature);
        }
};

/// Smoldering Infernal - 118065
class npc_kruul_smoldering_infernal : public CreatureScript
{
    public:
        npc_kruul_smoldering_infernal() : CreatureScript("npc_kruul_smoldering_infernal") { }

        enum eSpells
        {
            SpellReverseDeath   = 236532,
            SpellFelResonation  = 234476,
            SpellSmash          = 234631,
            SpellSmashQuick     = 241717,
            SpellGatheringHeat  = 234573,
            SpellBrittle        = 236422
        };

        enum eEvent
        {
            EventSmash = 1
        };

        struct npc_kruul_smoldering_infernalAI : public ScriptedAI
        {
            npc_kruul_smoldering_infernalAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Heating = false;
            bool m_QuickSmash = false;
            bool m_Brittle = false;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                SetCombatMovement(false);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellReverseDeath, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoCast(me, eSpells::SpellFelResonation, true);

                events.ScheduleEvent(eEvent::EventSmash, urand(7, 10) * TimeConstants::IN_MILLISECONDS);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellReverseDeath:
                    {
                        SetCombatMovement(true);

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    case eSpells::SpellGatheringHeat:
                    {
                        me->SetFullHealth();

                        SetCombatMovement(true);

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                        DoCast(me, eSpells::SpellFelResonation, true);

                        m_Heating = false;
                        m_QuickSmash = true;
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_Brittle && me->HealthBelowPctDamaged(25, p_Damage))
                {
                    m_Brittle = true;

                    DoCast(me, eSpells::SpellBrittle, true);
                }

                if (me->GetHealth() <= p_Damage)
                {
                    p_Damage = me->GetHealth() - 1;

                    m_Heating = true;

                    me->StopMoving();
                    me->StopAttack();

                    me->GetMotionMaster()->Clear();

                    me->RemoveAllAuras();

                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    SetCombatMovement(false);

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    DoCast(me, eSpells::SpellGatheringHeat);
                    return;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellSmash:
                    case eSpells::SpellSmashQuick:
                    {
                        if (p_EffIndex == SpellEffIndex::EFFECT_0 || p_Targets.empty())
                            break;

                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsInAxe(me, 5.0f, 30.0f))
                                return true;

                            return false;
                        });

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
                    case eSpells::SpellSmash:
                    case eSpells::SpellSmashQuick:
                    {
                        me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_TURN);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Heating || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvent::EventSmash)
                {
                    me->AddUnitState(UnitState::UNIT_STATE_CANNOT_TURN);

                    me->DelayedCastSpell(me, m_QuickSmash ? eSpells::SpellSmashQuick : eSpells::SpellSmash, false, 1);

                    events.ScheduleEvent(eEvent::EventSmash, urand(9, 13) * TimeConstants::IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kruul_smoldering_infernalAI(p_Creature);
        }
};

/// Inquisitive Stare - 234425
class spell_kruul_inquisitive_stare : public SpellScriptLoader
{
    public:
        spell_kruul_inquisitive_stare() : SpellScriptLoader("spell_kruul_inquisitive_stare") { }

        enum eSpell
        {
            SpellInquisitveStareMissile = 234426
        };

        class spell_kruul_inquisitive_stare_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kruul_inquisitive_stare_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        PreventDefaultAction();

                        if (l_Target->isInFront(l_Caster))
                            return;
                        else
                            l_Caster->CastSpell(l_Caster, eSpell::SpellInquisitveStareMissile, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kruul_inquisitive_stare_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kruul_inquisitive_stare_AuraScript();
        }
};

/// Gathering Heat - 234573
class spell_kruul_gathering_heat : public SpellScriptLoader
{
    public:
        spell_kruul_gathering_heat() : SpellScriptLoader("spell_kruul_gathering_heat") { }

        enum eSpell
        {
            SpellFelResonationScale = 234464
        };

        class spell_kruul_gathering_heat_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kruul_gathering_heat_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpell::SpellFelResonationScale))
                        l_Aura->DropStack();
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kruul_gathering_heat_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kruul_gathering_heat_AuraScript();
        }
};

/// Twisted Reflection - 234676
class spell_kruul_twisted_reflection : public SpellScriptLoader
{
    public:
        spell_kruul_twisted_reflection() : SpellScriptLoader("spell_kruul_twisted_reflection") { }

        enum eSpell
        {
            SpellTwistedReflectionHeal = 234678
        };

        class spell_kruul_twisted_reflection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kruul_twisted_reflection_AuraScript);

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                if (Unit* l_Kruul = GetCaster())
                    l_Kruul->CastSpell(l_Kruul, eSpell::SpellTwistedReflectionHeal, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_kruul_twisted_reflection_AuraScript::OnProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kruul_twisted_reflection_AuraScript();
        }
};

/// Gift of Sargeras - 233469
class spell_kruul_gift_of_sargeras : public SpellScriptLoader
{
    public:
        spell_kruul_gift_of_sargeras() : SpellScriptLoader("spell_kruul_gift_of_sargeras") { }

        enum eSpell
        {
            SpellWeary = 233455
        };

        class spell_kruul_gift_of_sargeras_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kruul_gift_of_sargeras_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->IsPlayer())
                        return;

                    l_Target->CastSpell(l_Target, eSpell::SpellWeary, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kruul_gift_of_sargeras_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCREEN_EFFECT, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kruul_gift_of_sargeras_AuraScript();
        }
};

/// Restart - 236132
class spell_kruul_restart : public SpellScriptLoader
{
    public:
        spell_kruul_restart() : SpellScriptLoader("spell_kruul_restart") { }

        class spell_kruul_restart_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kruul_restart_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->IsPlayer())
                        return;

                    l_Target->CastSpell(l_Target, eCommonSpells::SpellRestartPlayer, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kruul_restart_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kruul_restart_AuraScript();
        }
};

/// Shadow Sweep - 234440
class areatrigger_kruul_shadow_sweep : public AreaTriggerEntityScript
{
    public:
        areatrigger_kruul_shadow_sweep() : AreaTriggerEntityScript("areatrigger_kruul_shadow_sweep") { }

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer() || !p_AreaTrigger->GetCaster())
                return false;

            p_Target->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), true, p_AreaTrigger->GetDestination(), 9.0f, 1);
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer() || !p_AreaTrigger->GetCaster())
                return false;

            p_Target->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), false, Position());
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_kruul_shadow_sweep();
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_the_highlords_return()
{
    /// Bosses
    new boss_highlord_kruul();
    new boss_inquisitor_variss();

    /// Allies
    new npc_kruul_prophet_velen();
    new npc_kruul_korvas_bloodthorn();

    /// Enemies
    new npc_kruul_tormenting_eye();
    new npc_kruul_portal();
    new npc_kruul_nether_horror();
    new npc_kruul_smoldering_infernal();

    /// Spells
    new spell_kruul_inquisitive_stare();
    new spell_kruul_gathering_heat();
    new spell_kruul_twisted_reflection();
    new spell_kruul_gift_of_sargeras();
    new spell_kruul_restart();

    /// AreaTrigger
    new areatrigger_kruul_shadow_sweep();
}
#endif
