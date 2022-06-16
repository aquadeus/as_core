////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Vehicle.h"

/// Sister Subversia - 123464
class npc_sister_subversia_123464 : public CreatureScript
{
    public:
        npc_sister_subversia_123464() : CreatureScript("npc_sister_subversia_123464") { }

        struct npc_sister_subversia_123464AI : public ScriptedAI
        {
            npc_sister_subversia_123464AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_CorruptedBlades = 251279,
                Spell_ComeToMe = 251283
            };

            enum eEvents
            {
                Event_CastCorruptedBlades = 1,
                Event_CastComeToMe = 2
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
                me->SetControlled(false, UnitState::UNIT_STATE_ROOT);
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastCorruptedBlades, urand(2000, 4000));
                m_Events.ScheduleEvent(eEvents::Event_CastComeToMe, urand(6000, 8000));
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(0);
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
                case eEvents::Event_CastCorruptedBlades:
                {
                    DoCast(eSpells::Spell_CorruptedBlades);
                    m_Events.ScheduleEvent(eEvents::Event_CastCorruptedBlades, urand(15000, 20000));
                    break;
                }
                case eEvents::Event_CastComeToMe:
                {
                    me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                    DoCast(eSpells::Spell_ComeToMe);
                    m_Events.ScheduleEvent(eEvents::Event_CastComeToMe, urand(20000, 25000));
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetControlled(false, UnitState::UNIT_STATE_ROOT);
                    }, 7000);
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
            return new npc_sister_subversia_123464AI(p_Creature);
        }
};

/// Devastator Anchor - 123708, 123706, 123704
class npc_devastator_anchors_besieged : public CreatureScript
{
    public:
        npc_devastator_anchors_besieged() : CreatureScript("npc_devastator_anchors_besieged") { }

        struct npc_devastator_anchors_besiegedAI : public ScriptedAI
        {
            npc_devastator_anchors_besiegedAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Destroying = 246188,
                Spell_ProphetsWrath = 245262,
                Spell_LargeCameraShake = 211741
            };

            enum eObjectives
            {
                Obj_CannonADestroyed = 290849,
                Obj_CannonBDestroyed = 294129,
                Obj_CannonCDestroyed = 294130
            };

            enum eKillcredits
            {
                Killcredit_SouthernDevastatorDestroyed = 123049,
                Killcredit_WesternDevastatorDestroyed = 128719,
                Killcredit_EasternDevastatorDestroyed = 128720
            };

            enum eNpcs
            {
                Npc_SouthernLegionDevastator = 123705,
                Npc_WesternLegionDevastator = 123707,
                Npc_EasternLegionDevastator = 123709,
                Npc_SouthernDevastatorAnchor = 123704,
                Npc_WesternDevastatorAnchor = 123706,
                Npc_EasternDevastatorAnchor = 123708
            };

            enum eConversations
            {
                Conv_SouthernDevastatorDestroyed = 5253,
                Conv_WesternDevastatorDestroyed = 5254,
                Conv_EasternDevastatorDestroyed = 5255
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player || p_Spell->Id != eSpells::Spell_Destroying)
                    return;

                me->DestroyForPlayer(l_Player);

                if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                {
                    l_Creature->SetAIAnimKitId(13340);
                    l_Creature->DespawnOrUnsummon();
                }

                switch (me->GetEntry())
                {
                case eNpcs::Npc_SouthernDevastatorAnchor:
                {
                    l_Player->SetQuestObjectiveCounter(eObjectives::Obj_CannonCDestroyed, l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CannonCDestroyed) + 1);

                    if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CannonCDestroyed) == 3)
                    {
                        if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_SouthernLegionDevastator, 10.0f, true))
                        {
                            if (Creature* l_NewCreature = l_Player->SummonCreature(l_Creature->GetEntry(), l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SouthernDevastatorDestroyed);
                                l_Creature->DestroyForPlayer(l_Player);
                                l_NewCreature->RemoveAllAuras();
                                l_NewCreature->DelayedCastSpell(l_NewCreature, eSpells::Spell_ProphetsWrath, true, 2000);
                                l_NewCreature->DelayedCastSpell(l_NewCreature, eSpells::Spell_LargeCameraShake, true, 2000);
                                l_NewCreature->DespawnOrUnsummon(2000);

                                if (Conversation* l_Conversation = new Conversation)
                                {
                                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_SouthernDevastatorDestroyed, l_Player, nullptr, l_Player->GetPosition()))
                                        delete l_Conversation;
                                }
                            }
                        }
                    }

                    break;
                }
                case eNpcs::Npc_WesternDevastatorAnchor:
                {
                    l_Player->SetQuestObjectiveCounter(eObjectives::Obj_CannonBDestroyed, l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CannonBDestroyed) + 1);

                    if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CannonBDestroyed) == 3)
                    {
                        if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_WesternLegionDevastator, 10.0f, true))
                        {
                            if (Creature* l_NewCreature = l_Player->SummonCreature(l_Creature->GetEntry(), l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_WesternDevastatorDestroyed);
                                l_Creature->DestroyForPlayer(l_Player);
                                l_NewCreature->RemoveAllAuras();
                                l_NewCreature->DelayedCastSpell(l_NewCreature, eSpells::Spell_ProphetsWrath, true, 2000);
                                l_NewCreature->DelayedCastSpell(l_NewCreature, eSpells::Spell_LargeCameraShake, true, 2000);
                                l_NewCreature->DespawnOrUnsummon(2000);

                                if (Conversation* l_Conversation = new Conversation)
                                {
                                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_WesternDevastatorDestroyed, l_Player, nullptr, l_Player->GetPosition()))
                                        delete l_Conversation;
                                }
                            }
                        }
                    }

                    break;
                }
                case eNpcs::Npc_EasternDevastatorAnchor:
                {
                    l_Player->SetQuestObjectiveCounter(eObjectives::Obj_CannonADestroyed, l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CannonADestroyed) + 1);

                    if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_CannonADestroyed) == 3)
                    {
                        if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_EasternLegionDevastator, 10.0f, true))
                        {
                            if (Creature* l_NewCreature = l_Player->SummonCreature(l_Creature->GetEntry(), l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                            {
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_EasternDevastatorDestroyed);
                                l_Creature->DestroyForPlayer(l_Player);
                                l_NewCreature->RemoveAllAuras();
                                l_NewCreature->DelayedCastSpell(l_NewCreature, eSpells::Spell_ProphetsWrath, true, 2000);
                                l_NewCreature->DelayedCastSpell(l_NewCreature, eSpells::Spell_LargeCameraShake, true, 2000);
                                l_NewCreature->DespawnOrUnsummon(2000);

                                if (Conversation* l_Conversation = new Conversation)
                                {
                                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_EasternDevastatorDestroyed, l_Player, nullptr, l_Player->GetPosition()))
                                        delete l_Conversation;
                                }
                            }
                        }
                    }

                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_devastator_anchors_besiegedAI(p_Creature);
        }
};

/// Tome of Torment - 122937
class npc_tome_of_torment_122937 : public CreatureScript
{
    public:
        npc_tome_of_torment_122937() : CreatureScript("npc_tome_of_torment_122937") { }

        struct npc_tome_of_torment_122937AI : public ScriptedAI
        {
            npc_tome_of_torment_122937AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Burn = 245030
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Burn)
                    {
                        me->DestroyForPlayer(l_Player);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 6000, 0, l_Player->GetGUID()))
                        {
                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            l_Creature->AddAura(eSpells::Spell_Burn, l_Creature);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tome_of_torment_122937AI(p_Creature);
        }
};

/// Tortured Slave - 122974
class npc_tortured_slave_122974 : public CreatureScript
{
    public:
        npc_tortured_slave_122974() : CreatureScript("npc_tortured_slave_122974") { }

        struct npc_tortured_slave_122974AI : public ScriptedAI
        {
            npc_tortured_slave_122974AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_ReleasingSpirit = 245096,
                Spell_ShadowyGhostVisual = 144465,
                Spell_PermanentFeignDeath = 186571
            };

            enum eKillcredits
            {
                Killcredit_SpiritReleased = 122974
            };

            enum ePoints
            {
                Point_FlyEnd = 1
            };

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_FlyEnd)
                {
                    me->DespawnOrUnsummon();
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_ReleasingSpirit)
                    {
                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SpiritReleased);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                        {
                            l_Creature->SetAIAnimKitId(1152);
                            l_Creature->RemoveAura(eSpells::Spell_PermanentFeignDeath);
                            l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, 537166336);
                            l_Creature->RemoveFlag(UNIT_FIELD_FLAGS_2, 2049);
                            l_Creature->RemoveFlag(UNIT_FIELD_FLAGS_3, 8193);
                            l_Creature->SetFlag(UNIT_FIELD_FLAGS, 33587712);
                            l_Creature->SetFlag(UNIT_FIELD_FLAGS_2, 2048);
                            l_Creature->SetFlag(UNIT_FIELD_FLAGS_3, 1);
                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_ShadowyGhostVisual, true);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position l_FlyPos = l_Creature->GetPosition();
                                l_FlyPos.m_positionZ += 8.0f;

                                l_Creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                                l_Creature->SetPlayerHoverAnim(true);
                                l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyEnd, l_FlyPos);
                            }, 1000);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tortured_slave_122974AI(p_Creature);
        }
};

/// Lightforged Armament - 126877, 126874, 126878, 126872, 126863
class npc_lightforged_armament : public CreatureScript
{
    public:
        npc_lightforged_armament() : CreatureScript("npc_lightforged_armament") { }

        struct npc_lightforged_armamentAI : public ScriptedAI
        {
            npc_lightforged_armamentAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Extracting = 251866,
                Spell_BeamUp = 251874
            };

            enum eKillcredits
            {
                Killcredit_LightforgedArmamentRecovered = 126871
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Extracting)
                    {
                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_LightforgedArmamentRecovered);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                        {
                            l_Creature->DespawnOrUnsummon(4000);
                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_BeamUp, true);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->SetUInt32Value(UNIT_FIELD_FLAGS, 33554688);
                                l_Creature->SetDisplayId(11686);
                            }, 2000);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lightforged_armamentAI(p_Creature);
        }
};

/// Legion Hullbreaker - 127058
class npc_legion_hullbreaker_127058 : public CreatureScript
{
    public:
        npc_legion_hullbreaker_127058() : CreatureScript("npc_legion_hullbreaker_127058") { }

        struct npc_legion_hullbreaker_127058AI : public ScriptedAI
        {
            npc_legion_hullbreaker_127058AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Sabotaging = 252164
            };

            enum eKillcredits
            {
                Killcredit_LegionHullbreakerFusesReplaced = 127058
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Sabotaging)
                    {
                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_LegionHullbreakerFusesReplaced);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 360000, 0, l_Player->GetGUID()))
                        {
                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            l_Creature->SetUInt32Value(UNIT_FIELD_STATE_SPELL_VISUAL_ID, 70076);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_legion_hullbreaker_127058AI(p_Creature);
        }
};

/// Legion Hullbreaker - 127722
class npc_slave_trap_127722 : public CreatureScript
{
    public:
        npc_slave_trap_127722() : CreatureScript("npc_slave_trap_127722") { }

        struct npc_slave_trap_127722AI : public ScriptedAI
        {
            npc_slave_trap_127722AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Disarming = 253607
            };

            enum eKillcredits
            {
                Killcredit_SlaveTrapsDisarmed = 127722
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Disarming)
                    {
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->PlayOneShotAnimKitId(3408);
                        me->DespawnOrUnsummon();
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SlaveTrapsDisarmed);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_slave_trap_127722AI(p_Creature);
        }
};

/// Nathraxxan Rift - 126911
class npc_nathraxxan_rift_126911 : public CreatureScript
{
    public:
        npc_nathraxxan_rift_126911() : CreatureScript("npc_nathraxxan_rift_126911") { }

        struct npc_nathraxxan_rift_126911AI : public ScriptedAI
        {
            npc_nathraxxan_rift_126911AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Closing = 251959,
                Spell_Destruction = 251609
            };

            enum eGobs
            {
                Gob_LegionPortal = 272681
            };

            enum eKillcredits
            {
                Killcredit_NathraxxanRiftClosed = 126741
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Closing)
                    {
                        if (GameObject* l_Gob = l_Player->FindNearestGameObject(eGobs::Gob_LegionPortal, 10.0f))
                        {
                            l_Gob->DestroyForPlayer(l_Player);
                        }

                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_NathraxxanRiftClosed);
                        l_Player->CastSpell(l_Player, eSpells::Spell_Destruction, true);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_nathraxxan_rift_126911AI(p_Creature);
        }
};

/// Test Subjects - 127287, 127285, 127286, 127281
class npc_test_subjects : public CreatureScript
{
    public:
        npc_test_subjects() : CreatureScript("npc_test_subjects") { }

        struct npc_test_subjectsAI : public ScriptedAI
        {
            npc_test_subjectsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Releasing = 252674
            };

            enum eKillcredits
            {
                Killcredit_TestSubjectsReleased = 127283
            };

            enum eNpcs
            {
                Npc_DazedManaRay = 127287
            };

            enum ePoints
            {
                Point_MoveForward = 1
            };

            enum eEvents
            {
                Event_RemoveAuras = 1,
                Event_Emote = 2,
                Event_MoveOffset = 3
            };

            enum eActions
            {
                Action_SummonEvent = 1
            };

            EventMap m_Events;

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                case eActions::Action_SummonEvent:
                {
                    m_Events.ScheduleEvent(eEvents::Event_RemoveAuras, 1000);
                    break;
                }
                default:
                    break;
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                if (p_PointId == ePoints::Point_MoveForward)
                {
                    me->DespawnOrUnsummon();
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Releasing)
                    {
                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_TestSubjectsReleased);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 5000, 0, l_Player->GetGUID()))
                        {
                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            l_Creature->AI()->DoAction(eActions::Action_SummonEvent);
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_RemoveAuras:
                {
                    if (me->GetEntry() != eNpcs::Npc_DazedManaRay)
                    {
                        me->RemoveAllAuras();
                        me->RemoveFlag(UNIT_FIELD_BYTES_1, 50331648);
                        me->SetPlayerHoverAnim(false);
                        me->SetDisableGravity(false);
                        me->SetFall(true);
                    }
                    else
                    {
                        me->RemoveAllAuras();
                    }

                    m_Events.ScheduleEvent(eEvents::Event_Emote, 1000);
                    break;
                }
                case eEvents::Event_Emote:
                {
                    if (me->GetEntry() != eNpcs::Npc_DazedManaRay)
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 64);
                    }

                    m_Events.ScheduleEvent(eEvents::Event_MoveOffset, 1000);
                    break;
                }
                case eEvents::Event_MoveOffset:
                {
                    Position l_MovePos = me->GetPosition();

                    l_MovePos.m_positionX = l_MovePos.GetPositionX() + (std::cos(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::cos(l_MovePos.m_orientation) * 10);
                    l_MovePos.m_positionY = l_MovePos.GetPositionY() + (std::sin(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::sin(l_MovePos.m_orientation) * 10);
                    l_MovePos.m_positionZ = l_MovePos.GetPositionZ() + 0;
                    me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_MoveForward, l_MovePos, true);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_test_subjectsAI(p_Creature);
        }
};

/// Fel Devastator - 123467
class npc_fel_devastator_123467 : public CreatureScript
{
    public:
        npc_fel_devastator_123467() : CreatureScript("npc_fel_devastator_123467") { }

        struct npc_fel_devastator_123467AI : public VehicleAI
        {
            npc_fel_devastator_123467AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eNpcs
            {
                Npc_FelDevastatorMain = 125265,
                Npc_VfxBunny = 123594
            };

            enum eKillcredits
            {
                Killcredit_FaceTheCannonTowardsKiljaedensShip = 125880,
                Killcredit_FireTheFelDevastator = 123569
            };

            ObjectGuid m_SummonerGuid;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->ToPlayer())
                {
                    if (Creature* l_Creature = p_Summoner->FindNearestCreature(eNpcs::Npc_FelDevastatorMain, 10.0f))
                    {
                        l_Creature->DestroyForPlayer(p_Summoner->ToPlayer());
                    }
                }

                me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void SpellHitTarget(Unit* target, SpellInfo const* /*spell*/) override
            {
                if (target->GetEntry() == eNpcs::Npc_VfxBunny)
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FireTheFelDevastator);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (me->GetOrientation() >= 2.1f && me->GetOrientation() <= 2.6f)
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FaceTheCannonTowardsKiljaedensShip);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fel_devastator_123467AI(p_Creature);
        }
};

/// Broken Soul - 126944
class npc_broken_soul_126944 : public CreatureScript
{
    public:
        npc_broken_soul_126944() : CreatureScript("npc_broken_soul_126944") { }

        struct npc_broken_soul_126944AI : public ScriptedAI
        {
            npc_broken_soul_126944AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_TalismanOfTheProphet = 252295
            };

            enum eKillcredits
            {
                Killcredit_BrokenSoulsFreed = 126944
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_TalismanOfTheProphet)
                    {
                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_BrokenSoulsFreed);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_broken_soul_126944AI(p_Creature);
        }
};

/// Lightforged Dragoons - 128364, 128365, 128366, 128367
class npc_lightforged_dragoons : public CreatureScript
{
    public:
        npc_lightforged_dragoons() : CreatureScript("npc_lightforged_dragoons") { }

        struct npc_lightforged_dragoonsAI : public ScriptedAI
        {
            npc_lightforged_dragoonsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Freeing = 253096
            };

            enum eGobs
            {
                Gob_LegionRocks = 272316
            };

            enum eKillcredits
            {
                Killcredit_LightforgedDragoonsFreed = 127546
            };

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Freeing)
                    {
                        if (GameObject* l_Gob = l_Player->FindNearestGameObject(eGobs::Gob_LegionRocks, 10.0f))
                        {
                            l_Gob->AddToHideList(l_Player->GetGUID(), 360000);
                            l_Gob->DestroyForPlayer(l_Player);

                            if (GameObject* l_NewGob = l_Player->SummonGameObject(eGobs::Gob_LegionRocks, l_Gob->GetPosition(), 0, 0, l_Player->GetGUID()))
                            {
                                l_NewGob->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 8602);
                                l_NewGob->AddDelayedEvent([l_NewGob]() -> void
                                {
                                    l_NewGob->Delete();
                                }, 1000);
                            }
                        }

                        me->AddToHideList(l_Player->GetGUID(), 360000);
                        me->DestroyForPlayer(l_Player);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                        {
                            l_Creature->DespawnOrUnsummon(3000);
                            l_Creature->RemoveAllAuras();
                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                            }, 1000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position l_MovePos = l_Creature->GetPosition();

                                l_MovePos.m_positionX = l_MovePos.GetPositionX() + (std::cos(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::cos(l_MovePos.m_orientation) * 20);
                                l_MovePos.m_positionY = l_MovePos.GetPositionY() + (std::sin(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::sin(l_MovePos.m_orientation) * 20);
                                l_MovePos.m_positionZ = l_MovePos.GetPositionZ() + 0;
                                l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_MoveEnd, l_MovePos, true);
                            }, 2000);
                        }

                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_LightforgedDragoonsFreed);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lightforged_dragoonsAI(p_Creature);
        }
};

/// Frenzied Marsuul Kit - 123764
class npc_frenzied_marsuul_kit_123764 : public CreatureScript
{
    public:
        npc_frenzied_marsuul_kit_123764() : CreatureScript("npc_frenzied_marsuul_kit_123764") { }

        struct npc_frenzied_marsuul_kit_123764AI : public ScriptedAI
        {
            npc_frenzied_marsuul_kit_123764AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_CalmedState = 243995
            };

            enum eKillcredits
            {
                Killcredit_FrenziedMarsuulKitsCalmed = 123763
            };

            enum ePoints
            {
                Point_MoveEnd = 1
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_CalmedState && !me->isSummon())
                    {

                        me->DestroyForPlayer(l_Player);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                        {
                            l_Creature->DespawnOrUnsummon(6000);
                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->PlayOneShotAnimKitId(6761);
                            }, 500);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                Position l_MovePos = l_Creature->GetPosition();
                                l_MovePos.m_positionX = l_MovePos.GetPositionX() + (std::cos(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::cos(l_MovePos.m_orientation) * 20);
                                l_MovePos.m_positionY = l_MovePos.GetPositionY() + (std::sin(l_MovePos.m_orientation - (M_PI / 2)) * 0) + (std::sin(l_MovePos.m_orientation) * 20);
                                l_MovePos.m_positionZ = l_MovePos.GetPositionZ() + 0;

                                l_Creature->RemoveAllAuras();
                                l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_MoveEnd, l_MovePos, true);
                            }, 4000);
                        }

                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FrenziedMarsuulKitsCalmed);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_frenzied_marsuul_kit_123764AI(p_Creature);
        }
};

/// Lightforged Scout - 125926
class npc_lightforged_scout_125926 : public CreatureScript
{
    public:
        npc_lightforged_scout_125926() : CreatureScript("npc_lightforged_scout_125926") { }

        struct npc_lightforged_scout_125926AI : public ScriptedAI
        {
            npc_lightforged_scout_125926AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Bandaging = 192960
            };

            enum eKillcredits
            {
                Killcredit_LightforgedScoutHealed = 125926
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Bandaging)
                    {

                        me->DestroyForPlayer(l_Player);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                        {
                            uint64 l_PlayerGuid = l_Player->GetGUID();

                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->RemoveAllAuras();
                                l_Creature->SendPlaySpellVisualKit(59264, 0, 0);
                                l_Creature->PlayOneShotAnimKitId(10058);
                            }, 500);

                            l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                            {
                                if (Player* l_Pl = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    l_Creature->SetFacingToObject(l_Pl);
                            }, 2000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->PlayOneShotAnimKitId(11771);
                            }, 3000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->DespawnOrUnsummon(2000);
                                l_Creature->SendPlaySpellVisualKit(85978, 0, 0);
                            }, 6000);
                        }

                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_LightforgedScoutHealed);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lightforged_scout_125926AI(p_Creature);
        }
};

/// Shadowguard Hologram - 127527
class npc_shadowguard_hologram_127527 : public CreatureScript
{
    public:
        npc_shadowguard_hologram_127527() : CreatureScript("npc_shadowguard_hologram_127527") { }

        struct npc_shadowguard_hologram_127527AI : public ScriptedAI
        {
            npc_shadowguard_hologram_127527AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Destroying = 253071
            };

            enum eKillcredits
            {
                Killcredit_EtherealActivitiesDisrupted = 127498
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Destroying)
                    {
                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_EtherealActivitiesDisrupted);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_shadowguard_hologram_127527AI(p_Creature);
        }
};

/// Wrangler Kravos - 126852
class npc_wrangler_kravos_126852 : public CreatureScript
{
    public:
        npc_wrangler_kravos_126852() : CreatureScript("npc_wrangler_kravos_126852") { }

        struct npc_wrangler_kravos_126852AI : public ScriptedAI
        {
            npc_wrangler_kravos_126852AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Wrangled = 252012,
                Spell_Trample = 253974,
                Spell_SummonSteed = 255236,
                Spell_Bladestorm = 253978
            };

            enum eEvents
            {
                Event_CastTrample = 1,
                Event_CastBladestorm = 2
            };

            enum ePoints
            {
                Point_WanderersMoveEnd = 1
            };

            std::list<TempSummon*> m_SummonList;
            EventMap m_Events;
            bool m_SteedSummoned = false;

            void InitializeAI() override
            {
                me->SummonCreatureGroup(0, &m_SummonList);
            }

            void Reset() override
            {
                m_SteedSummoned = false;
                m_Events.Reset();
            }

            void JustSummoned(Creature* p_Summon) override
            {
                p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
                p_Summon->SetWalk(false);
                p_Summon->CastSpell(me, eSpells::Spell_Wrangled, true);
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                for (auto l_Itr : m_SummonList)
                {
                    if (l_Itr->isAlive())
                    {
                        switch (int32(l_Itr->GetHomePosition().m_positionX))
                        {
                        case 5318:
                        {
                            l_Itr->RemoveAllAuras();
                            l_Itr->PlayOneShotAnimKitId(12518);
                            l_Itr->AddDelayedEvent([l_Itr]() -> void
                            {
                                if (l_Itr->isAlive())
                                    l_Itr->GetMotionMaster()->MovePoint(ePoints::Point_WanderersMoveEnd, 5318.479f, 9713.537f, -94.26237f, false);
                            }, 2000);
                            l_Itr->DespawnOrUnsummon(3000);
                            break;
                        }
                        case 5324:
                        {
                            l_Itr->RemoveAllAuras();
                            l_Itr->PlayOneShotAnimKitId(12518);
                            l_Itr->AddDelayedEvent([l_Itr]() -> void
                            {
                                if (l_Itr->isAlive())
                                    l_Itr->GetMotionMaster()->MovePoint(ePoints::Point_WanderersMoveEnd, 5333.917f, 9716.905f, -94.26226f, false);
                            }, 2000);
                            l_Itr->DespawnOrUnsummon(3000);
                            break;
                        }
                        case 5325:
                        {
                            l_Itr->RemoveAllAuras();
                            l_Itr->PlayOneShotAnimKitId(12518);
                            l_Itr->AddDelayedEvent([l_Itr]() -> void
                            {
                                if (l_Itr->isAlive())
                                    l_Itr->GetMotionMaster()->MovePoint(ePoints::Point_WanderersMoveEnd, 5338.171f, 9726.646f, -94.38726f, false);
                            }, 2000);
                            l_Itr->DespawnOrUnsummon(3000);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }

                m_SummonList.clear();
                m_Events.ScheduleEvent(eEvents::Event_CastTrample, 8000);
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->GetHealthPct() <= 50 && !m_SteedSummoned)
                {
                    m_Events.Reset();
                    DoCast(eSpells::Spell_SummonSteed, true);
                    m_Events.ScheduleEvent(eEvents::Event_CastBladestorm, 5000);
                    me->Dismount();
                    m_SteedSummoned = true;
                }
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
                case eEvents::Event_CastTrample:
                {
                    DoCastVictim(eSpells::Spell_Trample);
                    m_Events.ScheduleEvent(eEvents::Event_CastTrample, 20000);
                    break;
                }
                case eEvents::Event_CastBladestorm:
                {
                    DoCast(eSpells::Spell_Bladestorm);
                    m_Events.ScheduleEvent(eEvents::Event_CastBladestorm, 22000);
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
            return new npc_wrangler_kravos_126852AI(p_Creature);
        }
};

/// Legion Portal - 127271
class npc_legion_portal_127271 : public CreatureScript
{
    public:
        npc_legion_portal_127271() : CreatureScript("npc_legion_portal_127271") { }

        struct npc_legion_portal_127271AI : public ScriptedAI
        {
            npc_legion_portal_127271AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Destroying = 240091
            };

            enum eKillcredits
            {
                Killcredit_LegionPortalsClosed = 127271
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Destroying)
                    {
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_LegionPortalsClosed);
                        me->DestroyForPlayer(l_Player);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_legion_portal_127271AI(p_Creature);
        }
};

/// Jed'hin Adept - 127773, 128776
class npc_jedhin_adept_vehicle : public CreatureScript
{
    public:
        npc_jedhin_adept_vehicle() : CreatureScript("npc_jedhin_adept_vehicle") { }

        struct npc_jedhin_adept_vehicleAI : public VehicleAI
        {
            npc_jedhin_adept_vehicleAI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            enum eSpells
            {
                Spell_RideVehicle = 52391,
                Spell_ConsumeEnergy = 248319,
                Spell_Faction = 253673,
                Spell_StartMatch = 253671,
                Spell_Bonk = 249864,
                Spell_AttackAura = 249865,
                Spell_Ping = 219431
            };

            enum eKillcredits
            {
                Killcredit_ParticipateInAJedhinMatch = 127759
            };

            enum eNpcs
            {
                // Spawns
                Npc_JedhinAdeptMain = 127775,
                Npc_JedhinAdeptOpponent = 125537,
                Npc_JedhinNoviceMain = 128777,
                Npc_JedhinNoviceOpponent = 128778,
                // Vehicles
                Npc_JedhinAdeptMainVehicle = 127773,
                Npc_JedhinNoviceMainVehicle = 128776,
                // Opponents
                Npc_Npc_JedhinNoviceOpponent = 128775
            };

            enum ePoints
            {
                Point_MoveToOpponentEnd = 1,
                Point_BackwardMoveEnd = 2
            };

            enum eEvents
            {
                Event_MoveToOpponent = 1,
                Event_MoveBackward = 2,
                Event_CastAttackAura = 3,
                Event_Emote = 4,
                Event_Killcredit = 5
            };

            ObjectGuid m_SummonerGuid;
            Position const m_MoveToOpponentPosition = { 5631.063f, 9985.189f, -70.79627f };
            Position const m_MoveToNoviceOpponentPosition = { 5749.120605f, 9856.611328f, -71.045120f };
            EventMap m_Events;
            bool m_FightStarted = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetEntry() == eNpcs::Npc_JedhinAdeptMainVehicle)
                {
                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_JedhinAdeptMain, 50.0f, true))
                    {
                        l_Creature->DestroyForPlayer(l_Player);
                    }

                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_JedhinAdeptOpponent, 50.0f, true))
                    {
                        l_Creature->DestroyForPlayer(l_Player);
                    }
                }
                else
                {
                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_JedhinNoviceMain, 50.0f, true))
                    {
                        l_Creature->DestroyForPlayer(l_Player);
                    }

                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_JedhinNoviceOpponent, 50.0f, true))
                    {
                        l_Creature->DestroyForPlayer(l_Player);
                    }
                }

                me->setPowerType(Powers::POWER_ENERGY);
                me->SetMaxPower(Powers::POWER_ENERGY, 100);
                l_Player->CastSpell(me, eSpells::Spell_RideVehicle, true);
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                DoCast(eSpells::Spell_ConsumeEnergy, true);
                DoCast(eSpells::Spell_Faction, true);
                m_SummonerGuid = l_Player->GetGUID();
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_StartMatch && p_Caster->GetGUID() == m_SummonerGuid && !m_FightStarted)
                {
                    me->SetAIAnimKitId(13692);
                    m_Events.ScheduleEvent(eEvents::Event_MoveToOpponent, 3000);
                    m_FightStarted = true;
                }
                else if (p_Spell->Id == eSpells::Spell_Ping && p_Caster->GetGUID() == m_SummonerGuid)
                {
                    p_Caster->RemoveAura(eSpells::Spell_AttackAura);
                    me->RemoveAura(eSpells::Spell_AttackAura);
                    m_Events.ScheduleEvent(eEvents::Event_Emote, 3000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_MoveToOpponentEnd:
                {
                    me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                    DoCast(eSpells::Spell_Bonk, true);
                    me->SetAIAnimKitId(0);
                    m_Events.ScheduleEvent(eEvents::Event_MoveBackward, 1500);
                    break;
                }
                case ePoints::Point_BackwardMoveEnd:
                {
                    me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                    DoCast(eSpells::Spell_AttackAura, true);
                    me->SetAIAnimKitId(13739);
                    break;
                }
                default:
                    break;
                }
            }

            void OnCharmed(bool apply) override
            {
                if (!apply)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_MoveToOpponent:
                {
                    me->SetWalk(false);
                    me->SetControlled(false, UnitState::UNIT_STATE_ROOT);

                    if (me->GetEntry() == eNpcs::Npc_JedhinNoviceMainVehicle)
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToOpponentEnd, m_MoveToNoviceOpponentPosition, false);
                    else
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToOpponentEnd, m_MoveToOpponentPosition, false);

                    break;
                }
                case eEvents::Event_MoveBackward:
                {
                    me->SetControlled(false, UnitState::UNIT_STATE_ROOT);

                    if (me->GetEntry() == eNpcs::Npc_JedhinNoviceMainVehicle)
                        me->GetMotionMaster()->MoveBackward(ePoints::Point_BackwardMoveEnd, 5748.270996f, 9855.568359f, -71.045120f);
                    else
                        me->GetMotionMaster()->MoveBackward(ePoints::Point_BackwardMoveEnd, 5630.188f, 9984.705f, -70.79627f);

                    break;
                }
                case eEvents::Event_Emote:
                {
                    me->SetAIAnimKitId(0);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    m_Events.ScheduleEvent(eEvents::Event_Killcredit, 2000);
                    break;
                }
                case eEvents::Event_Killcredit:
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_ParticipateInAJedhinMatch);

                    if (me->GetVehicleKit())
                    {
                        me->GetVehicleKit()->RemoveAllPassengers();
                    }

                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_jedhin_adept_vehicleAI(p_Creature);
        }
};

/// Jed'hin Adept - 127777, 128775
class npc_jedhin_adept_opponent : public CreatureScript
{
    public:
        npc_jedhin_adept_opponent() : CreatureScript("npc_jedhin_adept_opponent") { }

        struct npc_jedhin_adept_opponentAI : public ScriptedAI
        {
            npc_jedhin_adept_opponentAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_ConsumeEnergy = 248319,
                Spell_StartMatch = 253671,
                Spell_Bonk = 249864,
                Spell_ShoveDamage = 255322,
                Spell_ShoveEnergize = 255321,
                Spell_GrappleAura = 255323,
                Spell_GrappleDamage = 255324,
                Spell_Winner = 255320,
                Spell_Ping = 219431
            };

            enum ePoints
            {
                Point_MoveToOpponentEnd = 1,
                Point_BackwardMoveEnd = 2
            };

            enum eEvents
            {
                Event_MoveToOpponent = 1,
                Event_MoveBackward = 2,
                Event_CastAttackAura = 3,
                Event_CastShove = 4,
                Event_CastGrapple = 5,
                Event_Emote = 6
            };

            enum eNpcs
            {
                Npc_JedhinNoviceOpponent = 128775
            };

            ObjectGuid m_SummonerGuid;
            Position const m_MoveToOpponentPosition = { 5632.786f, 9986.198f, -70.79627f };
            Position const m_MoveToNoviceOpponentPosition = { 5750.176270f, 9857.906250f, -71.045120f };
            EventMap m_Events;
            bool m_FightStarted = false;
            bool m_FightEnd = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->setPowerType(Powers::POWER_ENERGY);
                me->SetMaxPower(Powers::POWER_ENERGY, 100);
                me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                DoCast(eSpells::Spell_ConsumeEnergy, true);
                m_SummonerGuid = p_Summoner->GetGUID();
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_StartMatch && p_Caster->GetGUID() == m_SummonerGuid)
                {
                    m_FightStarted = true;
                    me->SetAIAnimKitId(13692);
                    m_Events.ScheduleEvent(eEvents::Event_MoveToOpponent, 3000);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                switch (p_PointId)
                {
                case ePoints::Point_MoveToOpponentEnd:
                {
                    me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                    DoCast(eSpells::Spell_Bonk, true);
                    me->SetAIAnimKitId(0);
                    m_Events.ScheduleEvent(eEvents::Event_MoveBackward, 1500);
                    break;
                }
                case ePoints::Point_BackwardMoveEnd:
                {
                    me->SetControlled(true, UnitState::UNIT_STATE_ROOT);
                    me->SetAIAnimKitId(13739);
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    m_Events.ScheduleEvent(eEvents::Event_CastShove, 1000);
                    m_Events.ScheduleEvent(eEvents::Event_CastGrapple, 16000);
                    break;
                }
                default:
                    break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                if (damage >= me->GetHealth() && !m_FightEnd)
                {
                    damage = 0;
                    me->SetHealth(1);
                    me->CastStop();
                    me->StopAttack();
                    me->SetAIAnimKitId(0);
                    DoCast(eSpells::Spell_Winner);
                    l_Player->CastSpell(l_Player, eSpells::Spell_Ping, true);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 527104);
                    m_Events.Reset();
                    m_FightEnd = true;
                    m_Events.ScheduleEvent(eEvents::Event_Emote, 3000);
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (m_FightStarted && !l_Player->IsOnVehicle())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_MoveToOpponent:
                {
                    me->SetWalk(false);
                    me->SetControlled(false, UnitState::UNIT_STATE_ROOT);

                    if (me->GetEntry() == eNpcs::Npc_JedhinNoviceOpponent)
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToOpponentEnd, m_MoveToNoviceOpponentPosition, false);
                    else
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToOpponentEnd, m_MoveToOpponentPosition, false);

                    break;
                }
                case eEvents::Event_MoveBackward:
                {
                    me->SetControlled(false, UnitState::UNIT_STATE_ROOT);

                    if (me->GetEntry() == eNpcs::Npc_JedhinNoviceOpponent)
                        me->GetMotionMaster()->MoveBackward(ePoints::Point_BackwardMoveEnd, 5750.968750f, 9858.878906f, -71.045120f);
                    else
                        me->GetMotionMaster()->MoveBackward(ePoints::Point_BackwardMoveEnd, 5633.636f, 9986.725f, -70.79627f);

                    break;
                }
                case eEvents::Event_CastShove:
                {
                    DoCast(eSpells::Spell_ShoveDamage);
                    DoCast(eSpells::Spell_ShoveEnergize);
                    m_Events.ScheduleEvent(eEvents::Event_CastShove, 2000);
                    break;
                }
                case eEvents::Event_CastGrapple:
                {
                    DoCast(eSpells::Spell_GrappleAura);
                    DoCast(eSpells::Spell_GrappleDamage);
                    m_Events.ScheduleEvent(eEvents::Event_CastGrapple, 8000);
                    break;
                }
                case eEvents::Event_Emote:
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 525072);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    me->DespawnOrUnsummon(2000);
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_jedhin_adept_opponentAI(p_Creature);
        }
};

/// Household Vigilant - 125842
class npc_household_vigilant_125842 : public CreatureScript
{
    public:
        npc_household_vigilant_125842() : CreatureScript("npc_household_vigilant_125842") { }

        struct npc_household_vigilant_125842AI : public ScriptedAI
        {
            npc_household_vigilant_125842AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Activate = 249972,
                Spell_VigilantActivated = 250100
            };

            enum eKillcredits
            {
                Killcredit_HouseholdVigilantActivated = 125842
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Activate)
                    {
                        uint64 l_PlayerGuid = l_Player->GetGUID();

                        me->DestroyForPlayer(l_Player);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                        {
                            l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->PlayOneShotAnimKitId(11859);
                            }, 500);

                            l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                            {
                                if (Player* l_Pl = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                    l_Pl->CastSpell(l_Creature, eSpells::Spell_VigilantActivated, true);

                                l_Creature->DespawnOrUnsummon();
                            }, 2000);
                        }

                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_HouseholdVigilantActivated);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_household_vigilant_125842AI(p_Creature);
        }
};

/// Gatekeepers Image - 127403
class npc_gatekeepers_image_127403 : public CreatureScript
{
    public:
        npc_gatekeepers_image_127403() : CreatureScript("npc_gatekeepers_image_127403") { }

        struct npc_gatekeepers_image_127403AI : public ScriptedAI
        {
            npc_gatekeepers_image_127403AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eQuests
            {
                Quest_GatekeepersReviewTenacity = 48103
            };

            enum eKillcredits
            {
                Killcredit_CompleteTheTrialOfTenacity = 127401
            };

            EventMap m_Events;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player || me->isSummon())
                    return;

                if (me->GetExactDist2d(l_Player) <= 10.0f && l_Player->GetQuestStatus(eQuests::Quest_GatekeepersReviewTenacity) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                    {
                        l_Creature->AI()->Talk(0, l_Player->GetGUID());

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->SendPlaySpellVisualKit(1364, 0, 0);
                            l_Creature->PlayOneShotAnimKitId(12359);
                            l_Creature->DespawnOrUnsummon();
                        }, 9000);
                    }

                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_CompleteTheTrialOfTenacity);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gatekeepers_image_127403AI(p_Creature);
        }
};

/// Immortal Squadron Fighter - 127012
class npc_immortal_squadron_fighter_127012 : public CreatureScript
{
    public:
        npc_immortal_squadron_fighter_127012() : CreatureScript("npc_immortal_squadron_fighter_127012") { }

        struct npc_immortal_squadron_fighter_127012AI : public ScriptedAI
        {
            npc_immortal_squadron_fighter_127012AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_MarkTarget = 252103,
                Spell_AntiAirFire = 252107
            };

            enum eKillcredits
            {
                Killcredit_ImmortalSquadronFightersTargeted = 127012
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_MarkTarget)
                    {
                        me->DestroyForPlayer(l_Player);

                        if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_AntiAirFire, true);
                            l_Creature->DespawnOrUnsummon(1000);

                            l_Creature->AddDelayedEvent([l_Creature]() -> void
                            {
                                l_Creature->PlayOneShotAnimKitId(976);
                            }, 500);
                        }

                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_ImmortalSquadronFightersTargeted);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_immortal_squadron_fighter_127012AI(p_Creature);
        }
};

/// Artificer's Expanding Explosive - 126535, 126572, 126573
class npc_artificers_expanding_explosives : public CreatureScript
{
    public:
        npc_artificers_expanding_explosives() : CreatureScript("npc_artificers_expanding_explosives") { }

        struct npc_artificers_expanding_explosivesAI : public ScriptedAI
        {
            npc_artificers_expanding_explosivesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void InitializeAI() override
            {
                if (me->HasInPhaseList(485260) || me->HasInPhaseList(485261) || me->HasInPhaseList(485262))
                {
                    me->SetUInt32Value(UNIT_FIELD_STATE_SPELL_VISUAL_ID, 37794);
                }
                else
                {
                    me->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, 0);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_artificers_expanding_explosivesAI(p_Creature);
        }
};

/// Lightforged Warframe - 126844
class npc_lightforged_warframe_126844 : public CreatureScript
{
    public:
        npc_lightforged_warframe_126844() : CreatureScript("npc_lightforged_warframe_126844") { }

        struct npc_lightforged_warframe_126844AI : public VehicleAI
        {
            npc_lightforged_warframe_126844AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_LightforgedWarframeToAntorus[13] =
            {
                { -2932.363f, 8827.597f, -231.1855f },
                { -2906.214f, 8791.229f, -213.826f },
                { -2879.264f, 8815.065f, -191.429f },
                { -2773.427f, 8937.386f, -172.5981f },
                { -2797.116f, 9046.458f, -172.5981f },
                { -2850.026f, 9104.283f, -172.5981f },
                { -2904.516f, 9156.059f, -172.5981f },
                { -2954.95f, 9233.058f, -172.5981f },
                { -2954.358f, 9331.646f, -150.6266f },
                { -2967.314f, 9387.624f, -133.6407f },
                { -3014.495f, 9428.505f, -70.98208f },
                { -3093.078f, 9538.622f, -61.11577f },
                { -3120.536f, 9578.0f, -52.93395f }
            };

            G3D::Vector3 const Path_LightforgedWarframeCirclingUnderAntorus[11] =
            {
                { -3145.71f, 9577.83f, -38.07336f },
                { -3201.231f, 9588.583f, -29.73593f },
                { -3236.736f, 9533.254f, -34.28746f },
                { -3335.686f, 9462.25f, -34.28746f },
                { -3362.503f, 9428.068f, -34.28746f },
                { -3348.469f, 9394.757f, -34.28746f },
                { -3311.549f, 9347.063f, -34.28746f },
                { -3266.972f, 9369.271f, -15.47403f },
                { -3192.467f, 9437.445f, -28.14229f },
                { -3154.844f, 9508.677f, -34.28746f },
                { -3148.253f, 9550.474f, -34.28746f }
            };

            G3D::Vector3 const Path_LightforgedWarframeCirclingNearSentinax[8] =
            {
                { -3296.144f, 9268.019f, 30.71307f },
                { -3392.764f, 9213.286f, 30.71307f },
                { -3467.571f, 9079.599f, 30.71307f },
                { -3424.955f, 8988.911f, 30.71307f },
                { -3324.609f, 8971.911f, 30.71307f },
                { -3233.658f, 9108.314f, 30.71307f },
                { -3207.068f, 9198.817f, 30.71307f },
                { -3257.983f, 9255.448f, 30.71307f }
            };

            enum eSpells
            {
                Spell_RideVehicle = 52391,
                Spell_HolyFadeInWithLightBeam = 248895,
                Spell_ReturnToHopesLanding = 252494
            };

            enum ePoints
            {
                Point_FlyToAntorusEnd = 1,
                Point_CirclingUnderAntorusEnd = 2,
                Point_CirclingNearSentinaxEnd = 3
            };

            enum eKillcredits
            {
                Killcredit_FlyAWarframeIntoAntorus = 127185
            };

            enum eObjectives
            {
                Obj_FlyAWarframeIntoAntorus = 292875,
                Obj_SpireOfWoeDestroyed = 292879,
                Obj_DemonsSlain = 292934
            };

            enum eQuests
            {
                Quest_Gladius = 48780
            };

            ObjectGuid m_SummonerGuid;
            bool m_PathStarted = false;
            bool m_CanStartSentinaxPath = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (me->GetVehicleKit())
                {
                    if (Unit* l_Unit = me->GetVehicleKit()->GetPassenger(0))
                    {
                        l_Unit->SetInPhase(48780, false, true);
                        l_Unit->SetInPhase(487800, false, true);
                        l_Unit->SetInPhase(487801, false, true);
                        l_Unit->SetInPhase(487802, false, true);
                        l_Unit->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                        p_Summoner->CastSpell(l_Unit, eSpells::Spell_RideVehicle, true);
                        l_Unit->CastSpell(l_Unit, eSpells::Spell_HolyFadeInWithLightBeam, true);
                    }
                }

                me->SetSpeed(MOVE_FLIGHT, 4.0f);
                me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyToAntorusEnd, Path_LightforgedWarframeToAntorus, 13);
                m_SummonerGuid = p_Summoner->GetGUID();
                m_PathStarted = true;

            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                switch (p_PointId)
                {
                case ePoints::Point_FlyToAntorusEnd:
                {
                    me->SetSpeed(MOVE_FLIGHT, 2.0f);
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FlyAWarframeIntoAntorus);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclingUnderAntorusEnd, Path_LightforgedWarframeCirclingUnderAntorus, 11);
                    m_CanStartSentinaxPath = true;
                    break;
                }
                case ePoints::Point_CirclingUnderAntorusEnd:
                {
                    if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SpireOfWoeDestroyed) != 3 || l_Player->GetQuestObjectiveCounter(eObjectives::Obj_DemonsSlain) != 15)
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclingUnderAntorusEnd, Path_LightforgedWarframeCirclingUnderAntorus, 11);
                    }

                    break;
                }
                case ePoints::Point_CirclingNearSentinaxEnd:
                {
                    if (!l_Player->IsQuestRewarded(eQuests::Quest_Gladius))
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclingNearSentinaxEnd, Path_LightforgedWarframeCirclingNearSentinax, 8);
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!l_Player->IsOnVehicle() && m_PathStarted)
                {
                    l_Player->CastSpell(l_Player, eSpells::Spell_ReturnToHopesLanding, true);
                    me->DespawnOrUnsummon();
                    return;
                }

                if (l_Player->IsQuestRewarded(eQuests::Quest_Gladius))
                {
                    if (Unit* l_Unit = me->GetVehicleKit()->GetPassenger(0))
                    {
                        if (l_Unit->GetVehicleKit())
                        {
                            l_Unit->GetVehicleKit()->RemoveAllPassengers();
                        }

                        l_Player->CastSpell(l_Player, eSpells::Spell_ReturnToHopesLanding, true);
                    }

                    me->DespawnOrUnsummon();
                    return;
                }

                if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SpireOfWoeDestroyed) == 3 && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_DemonsSlain) == 15 &&
                    m_CanStartSentinaxPath)
                {
                    m_CanStartSentinaxPath = false;
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclingNearSentinaxEnd, Path_LightforgedWarframeCirclingNearSentinax, 8);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_lightforged_warframe_126844AI(p_Creature);
        }
};

/// Spire Of Woe, Sentinax - 127183, 127264
class npc_gladius_quest_creatures : public CreatureScript
{
    public:
        npc_gladius_quest_creatures() : CreatureScript("npc_gladius_quest_creatures") { }

        struct npc_gladius_quest_creaturesAI : public ScriptedAI
        {
            npc_gladius_quest_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eKillcredits
            {
                Killcredit_SpireOfWoeDestroyed = 127183,
                Killcredit_SentinaxDestroyed = 127264,
                Killcredit_FirstSpireDestroyed = 2929350,
                Killcredit_SecondSpireDestroyed = 2929351,
                Killcredit_ThirdSpireDestroyed = 2929352
            };

            enum eNpcs
            {
                Npc_SpireOfWoe = 127183
            };

            void InitializeAI() override
            {
                me->setRegeneratingHealth(false);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (damage >= me->GetHealth())
                {
                    for (auto l_Itr : me->getThreatManager().getThreatList())
                    {
                        if (Unit* l_Unit = l_Itr->getTarget())
                        {
                            if (Player* l_Player = l_Unit->ToPlayer())
                            {
                                if (me->GetEntry() == eNpcs::Npc_SpireOfWoe)
                                {
                                    switch (int32(me->GetPositionX()))
                                    {
                                    case -3207:
                                    {
                                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FirstSpireDestroyed);
                                        break;
                                    }
                                    case -3324:
                                    {
                                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SecondSpireDestroyed);
                                        break;
                                    }
                                    case -3301:
                                    {
                                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_ThirdSpireDestroyed);
                                        break;
                                    }
                                    default:
                                        break;
                                    }

                                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SpireOfWoeDestroyed);
                                }
                                else
                                {
                                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_SentinaxDestroyed);
                                }
                            }
                        }
                    }

                    damage = 0;
                    me->SetFullHealth();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gladius_quest_creaturesAI(p_Creature);
        }
};

/// Void Manifestation - 124398
class npc_void_manifestation_124398 : public CreatureScript
{
    public:
        npc_void_manifestation_124398() : CreatureScript("npc_void_manifestation_124398") { }

        struct npc_void_manifestation_124398AI : public ScriptedAI
        {
            npc_void_manifestation_124398AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_DispelManifestation = 255460
            };

            enum eKillcredits
            {
                Killcredit_VoidManifestationsCleansed = 124398
            };

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_DispelManifestation)
                    {
                        me->AddToHideList(l_Player->GetGUID(), 360000);
                        me->DestroyForPlayer(l_Player);
                        l_Player->KilledMonsterCredit(eKillcredits::Killcredit_VoidManifestationsCleansed);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_void_manifestation_124398AI(p_Creature);
        }
};

/// Mother Rosula - 127705
class npc_mother_rosula_127705 : public CreatureScript
{
    public:
        npc_mother_rosula_127705() : CreatureScript("npc_mother_rosula_127705") { }

        struct npc_mother_rosula_127705AI : public BossAI
        {
            npc_mother_rosula_127705AI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            enum eSpells
            {
                Spell_FelDissolveIn = 211762,
                Spell_Imps = 253692,
                Spell_BadBreath = 253679
            };

            enum eKillcredits
            {
                Killcredit_VoidManifestationsCleansed = 124398
            };

            enum eEvents
            {
                Event_CastImps = 1,
                Event_CastBadBreath = 2
            };

            EventMap m_Events;

            void InitializeAI() override
            {
                me->SetDisplayId(45494);
                SetCombatMovement(false);
            }

            void JustRespawned() override
            {
                me->SetDisplayId(45494);
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                DoCast(eSpells::Spell_FelDissolveIn, true);
                me->SetDisplayId(65648);

                me->AddDelayedEvent([this]() -> void
                {
                    me->PlayOneShotAnimKitId(14054);
                    Talk(0);
                }, 3000);

                me->AddDelayedEvent([this]() -> void
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 2048);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
                }, 5000);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                m_Events.Reset();
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 2048);
                me->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(1);
                m_Events.ScheduleEvent(eEvents::Event_CastImps, 4000);
                m_Events.ScheduleEvent(eEvents::Event_CastBadBreath, 8000);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetReactState() == REACT_PASSIVE)
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastImps:
                {
                    DoCast(eSpells::Spell_Imps);
                    m_Events.ScheduleEvent(eEvents::Event_CastImps, urand(4000, 9000));
                    break;
                }
                case eEvents::Event_CastBadBreath:
                {
                    me->AttackStop();
                    me->SetFacingTo(me->GetOrientation());
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    DoCast(eSpells::Spell_BadBreath);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    }, 3500);

                    m_Events.ScheduleEvent(eEvents::Event_CastBadBreath, 16000);
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
            return new npc_mother_rosula_127705AI(p_Creature);
        }
};

/// Resonating Augari - 128358, 128359
class npc_resonating_augari : public CreatureScript
{
    public:
        npc_resonating_augari() : CreatureScript("npc_resonating_augari") { }

        struct npc_resonating_augariAI : public ScriptedAI
        {
            npc_resonating_augariAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_ArcaneOrb = 252182,
                Spell_EldritchStrike = 252181,
                Spell_Attuning = 254715
            };

            enum eKillcredits
            {
                Killcredit_AncientAugariEssence = 128360
            };

            enum eEvents
            {
                Event_CastArcaneOrb = 1,
                Event_CastEldritchStrike = 2
            };

            EventMap m_Events;
            bool m_DespawnInit = false;

            void Reset() override
            {
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 33024);
                me->setFaction(190);
                m_Events.Reset();
                m_DespawnInit = false;
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Attuning && !m_DespawnInit)
                    {
                        if (roll_chance_i(50))
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                            me->DespawnOrUnsummon(4000);
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_AncientAugariEssence);
                            m_DespawnInit = true;
                        }
                        else
                        {
                            me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                            me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                            me->RemoveAllAuras();
                            me->setFaction(14);

                            if (Player* l_Victim = me->FindNearestPlayer(10.0f))
                            {
                                me->Attack(l_Victim, false);
                            }
                        }
                    }
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                if (roll_chance_i(50))
                {
                    Talk(0);
                }

                m_Events.ScheduleEvent(eEvents::Event_CastArcaneOrb, 2000);
                m_Events.ScheduleEvent(eEvents::Event_CastEldritchStrike, 4000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (roll_chance_i(50))
                {
                    Talk(1);
                }
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
                case eEvents::Event_CastArcaneOrb:
                {
                    DoCastVictim(eSpells::Spell_ArcaneOrb);
                    m_Events.ScheduleEvent(eEvents::Event_CastArcaneOrb, 9000);
                    break;
                }
                case eEvents::Event_CastEldritchStrike:
                {
                    DoCastVictim(eSpells::Spell_EldritchStrike);
                    m_Events.ScheduleEvent(eEvents::Event_CastEldritchStrike, 7000);
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
            return new npc_resonating_augariAI(p_Creature);
        }
};

/// Vishax's Portal - 127943
class npc_vishaxs_portal_127943 : public CreatureScript
{
    public:
        npc_vishaxs_portal_127943() : CreatureScript("npc_vishaxs_portal_127943") { }

        struct npc_vishaxs_portal_127943AI : public ScriptedAI
        {
            npc_vishaxs_portal_127943AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_Dummy = 17648,
                Spell_ActivatePortal = 253909,
                Spell_TeleportToVishaxsShip = 253910
            };

            enum eEvents
            {
                Event_Deactivate = 1
            };

            enum eKillcredits
            {
                Killcredit_UseVishaxsPortal = 127943
            };

            enum eQuests
            {
                Quest_CommanderOnDeck = 49007
            };

            EventMap m_Events;
            bool m_Activated = false;

            void Reset() override
            {
                m_Activated = false;
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (p_Spell->Id == eSpells::Spell_Dummy)
                    {
                        if (!m_Activated && (l_Player->HasQuest(eQuests::Quest_CommanderOnDeck) || l_Player->IsQuestRewarded(eQuests::Quest_CommanderOnDeck)))
                        {
                            l_Player->DelayedCastSpell(me, eSpells::Spell_ActivatePortal, false, 500);
                            l_Player->TalkedToCreature(eKillcredits::Killcredit_UseVishaxsPortal, 0);
                        }
                        else if (m_Activated)
                        {
                            l_Player->CastSpell(l_Player, eSpells::Spell_TeleportToVishaxsShip, true);
                        }
                    }
                    else if (p_Spell->Id == eSpells::Spell_ActivatePortal)
                    {
                        me->SetAIAnimKitId(11718);
                        m_Activated = true;
                        m_Events.ScheduleEvent(eEvents::Event_Deactivate, 120000);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_Deactivate:
                {
                    me->SetAIAnimKitId(0);
                    m_Activated = false;
                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_vishaxs_portal_127943AI(p_Creature);
        }
};

/// Squadron Commander Vishax - 127700
class npc_squadron_commander_vishax_127700 : public CreatureScript
{
    public:
        npc_squadron_commander_vishax_127700() : CreatureScript("npc_squadron_commander_vishax_127700") { }

        struct npc_squadron_commander_vishax_127700AI : public ScriptedAI
        {
            npc_squadron_commander_vishax_127700AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Spell_MassiveSlam = 253523,
                Spell_ArtilleryStrike = 253531,
                Spell_CrushArmor = 33661
            };

            enum eEvents
            {
                Event_ResetLineVariable = 1,
                Event_CastMassiveSlam = 2,
                Event_CastArtilleryStrike = 3,
                Event_CrushArmor = 4,
                Event_SummonFighter = 5
            };

            enum eNpcs
            {
                Npc_LegionFighter = 127945
            };

            enum ePoints
            {
                Point_LegionFighterFlygthEnd = 1
            };

            EventMap m_Events;
            bool m_CanSayLine = true;
            uint8 m_CurrentFighter = 0;

            Position m_LegionFighterLeftSpawnPos = { -3596.0f, 8549.724f, 103.2745f, 4.691879f };
            Position m_LegionFighterRightSpawnPos = { -3704.705f, 8544.149f, 105.4774f, 5.020845f };

            void Reset() override
            {
                m_Events.Reset();
                m_Events.ScheduleEvent(eEvents::Event_SummonFighter, 1000);
                m_Events.ScheduleEvent(eEvents::Event_ResetLineVariable, 60000);
                m_CurrentFighter = 0;
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (me->GetExactDist2d(l_Player) <= 100.0f && m_CanSayLine && !me->isInCombat())
                {
                    Talk(0);
                    m_CanSayLine = false;
                    m_Events.ScheduleEvent(eEvents::Event_ResetLineVariable, 60000);
                }
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                m_Events.ScheduleEvent(eEvents::Event_CastMassiveSlam, 9000);
                m_Events.ScheduleEvent(eEvents::Event_CastArtilleryStrike, 16000);
                m_Events.ScheduleEvent(eEvents::Event_CrushArmor, 21000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                switch (m_Events.GetEvent())
                {
                case eEvents::Event_ResetLineVariable:
                {
                    m_CanSayLine = true;
                    m_Events.CancelEvent(eEvents::Event_ResetLineVariable);
                    break;
                }
                case eEvents::Event_SummonFighter:
                {
                    if (m_CurrentFighter % 2)
                    {
                        if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_LegionFighter, m_LegionFighterLeftSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            G3D::Vector3 const Path_LegionFighter[7] =
                            {
                                { -3600.826f, 8519.536f, 105.4327f },
                                { -3596.361f, 8412.721f, 80.99121f },
                                { -3591.12f, 8378.389f, 79.41051f },
                                { -3583.094f, 8332.271f, 88.04812f },
                                { -3562.55f, 8238.781f, 94.86294f },
                                { -3541.943f, 8186.832f, 127.6576f },
                                { -3445.889f, 8070.445f, 127.6576f }
                            };

                            l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LegionFighterFlygthEnd, Path_LegionFighter, 7);
                        }

                        m_CurrentFighter = 0;
                    }
                    else
                    {
                        if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_LegionFighter, m_LegionFighterRightSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            G3D::Vector3 const Path_LegionFighter[7] =
                            {
                                { -3692.939f, 8502.703f, 104.172f },
                                { -3662.087f, 8407.413f, 82.47073f },
                                { -3649.257f, 8355.433f, 78.02065f },
                                { -3641.109f, 8315.799f, 82.62057f },
                                { -3622.757f, 8232.24f, 94.86294f },
                                { -3629.915f, 8106.86f, 127.6576f },
                                { -3687.587f, 7967.685f, 127.6576f }
                            };

                            l_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                            l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_LegionFighterFlygthEnd, Path_LegionFighter, 7);
                        }

                        m_CurrentFighter = 1;
                    }

                    m_Events.RepeatEvent(30000);
                    break;
                }
                default:
                    break;
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastMassiveSlam:
                {
                    DoCast(eSpells::Spell_MassiveSlam);
                    m_Events.ScheduleEvent(eEvents::Event_CastMassiveSlam, 16000);
                    break;
                }
                case eEvents::Event_CastArtilleryStrike:
                {
                    DoCast(eSpells::Spell_ArtilleryStrike);
                    m_Events.ScheduleEvent(eEvents::Event_CastArtilleryStrike, 16000);
                    break;
                }
                case eEvents::Event_CrushArmor:
                {
                    DoCastVictim(eSpells::Spell_CrushArmor);
                    m_Events.ScheduleEvent(eEvents::Event_CrushArmor, 12000);
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
            return new npc_squadron_commander_vishax_127700AI(p_Creature);
        }
};

/// Control Stalker - 127845
class npc_doomcaster_suprax_control_stalker : public CreatureScript
{
    public:
        npc_doomcaster_suprax_control_stalker() : CreatureScript("npc_doomcaster_suprax_control_stalker") { }

        struct npc_doomcaster_suprax_control_stalkerAI : public ScriptedAI
        {
            npc_doomcaster_suprax_control_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::map<uint32, Position> m_DarkRitualRuneSpawnPositions =
            {
                { 253740, Position(-2060.615f, 9276.391f, -66.26068f) },
                { 253743, Position(-2050.493f, 9304.548f, -66.25999f) },
                { 253744, Position(-2043.28f, 9285.097f, -66.53582f) }
            };

            enum eSpells
            {
                Spell_ControlActiveFirst = 253775,
                Spell_ControlActiveSecond = 253780,
                Spell_ControlActiveThird = 253781,
                Spell_ControlInactiveFirst = 253779,
                Spell_ControlInactiveSecond = 253782,
                Spell_ControlInactiveThird = 253783,
            };

            enum eEvents
            {
                Event_SpawnRunes = 1
            };

            enum eNpcs
            {
                Npc_DoomcasterSuprax = 127703
            };

            EventMap m_Events;
            Position m_DoomcasterSupraxSpawnPos = { -2056.144f, 9290.413f, -66.34904f, 2.838749f };
            bool m_SupraxSummoned = false;

            void SpawnRunes()
            {
                for (auto l_Itr : m_DarkRitualRuneSpawnPositions)
                {
                    me->CastSpell(l_Itr.second, l_Itr.first, true);
                }
            }

            void Reset() override
            {
                m_Events.Reset();
                SpawnRunes();
                m_SupraxSummoned = false;
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (m_SupraxSummoned)
                    return;

                switch (p_Spell->Id)
                {
                case eSpells::Spell_ControlInactiveFirst:
                {
                    me->RemoveAura(eSpells::Spell_ControlActiveFirst);
                    break;
                }
                case eSpells::Spell_ControlInactiveSecond:
                {
                    me->RemoveAura(eSpells::Spell_ControlActiveSecond);
                    break;
                }
                case eSpells::Spell_ControlInactiveThird:
                {
                    me->RemoveAura(eSpells::Spell_ControlActiveThird);
                    break;
                }
                default:
                    break;
                }

                if (me->HasAura(eSpells::Spell_ControlActiveFirst) &&
                    me->HasAura(eSpells::Spell_ControlActiveSecond) &&
                    me->HasAura(eSpells::Spell_ControlActiveThird))
                {
                    std::list<AreaTrigger*> l_AreatriggerList;
                    me->GetAreatriggerListInRange(l_AreatriggerList, 20.0f);

                    if (!l_AreatriggerList.empty())
                    {
                        for (auto l_Itr : l_AreatriggerList)
                        {
                            if (m_DarkRitualRuneSpawnPositions.find(l_Itr->GetSpellId()) != m_DarkRitualRuneSpawnPositions.end())
                            {
                                l_Itr->Despawn();
                            }
                        }
                    }

                    me->RemoveAllAuras();
                    me->SummonCreature(eNpcs::Npc_DoomcasterSuprax, m_DoomcasterSupraxSpawnPos, TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    m_SupraxSummoned = true;
                    m_Events.ScheduleEvent(eEvents::Event_SpawnRunes, 30000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_SpawnRunes:
                {
                    if (me->FindNearestCreature(eNpcs::Npc_DoomcasterSuprax, 100.0f, false))
                    {
                        SpawnRunes();
                        m_SupraxSummoned = false;
                    }
                    else
                    {
                        m_Events.ScheduleEvent(eEvents::Event_SpawnRunes, 30000);
                    }

                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_doomcaster_suprax_control_stalkerAI(p_Creature);
        }
};

/// Leaking Fel Spreader - 272313
class gob_leaking_fel_spreader_272313 : public GameObjectScript
{
    public:
        gob_leaking_fel_spreader_272313() : GameObjectScript("gob_leaking_fel_spreader_272313") { }

        struct gob_leaking_fel_spreader_272313AI : public GameObjectAI
        {
            gob_leaking_fel_spreader_272313AI(GameObject* go) : GameObjectAI(go) { }

            void InitializeAI() override
            {
                go->SetUInt32Value(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 69116);
                go->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 10455);
            }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2 && p_Unit->ToPlayer())
                {
                    go->DestroyForPlayer(p_Unit->ToPlayer());

                    if (GameObject* l_Gob = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), 0, 0, p_Unit->GetGUID()))
                    {
                        l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 10455);
                        l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 0);
                        l_Gob->SetFlag(GAMEOBJECT_FIELD_FLAGS, 20);
                        l_Gob->SetGoState(GOState::GO_STATE_ACTIVE_ALTERNATIVE);
                        l_Gob->AddDelayedEvent([l_Gob]() -> void
                        {
                            l_Gob->Delete();
                        }, 360000);
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_leaking_fel_spreader_272313AI(go);
        }
};

/// Smelting Vessel - 276918
class gob_smelting_vessel_276918 : public GameObjectScript
{
    public:
        gob_smelting_vessel_276918() : GameObjectScript("gob_smelting_vessel_276918") { }

        struct gob_smelting_vessel_276918AI : public GameObjectAI
        {
            gob_smelting_vessel_276918AI(GameObject* go) : GameObjectAI(go) { }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2 && p_Unit->ToPlayer())
                {
                    go->DestroyForPlayer(p_Unit->ToPlayer());

                    if (GameObject* l_Gob = p_Unit->SummonGameObject(go->GetEntry(), go->GetPosition(), 0, 0, p_Unit->GetGUID()))
                    {
                        l_Gob->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 151);
                        l_Gob->SetFlag(GAMEOBJECT_FIELD_FLAGS, 20);
                        l_Gob->SetGoState(GOState::GO_STATE_ACTIVE_ALTERNATIVE);
                        l_Gob->AddDelayedEvent([l_Gob]() -> void
                        {
                            l_Gob->Delete();
                        }, 360000);
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_smelting_vessel_276918AI(go);
        }
};

/// Shadowguard Supplies, Shadowguard Portal - 276478, 276472
class gob_hidden_in_the_hollows_gobs : public GameObjectScript
{
    public:
        gob_hidden_in_the_hollows_gobs() : GameObjectScript("gob_hidden_in_the_hollows_gobs") { }

        struct gob_hidden_in_the_hollows_gobsAI : public GameObjectAI
        {
            gob_hidden_in_the_hollows_gobsAI(GameObject* go) : GameObjectAI(go) { }

            enum eGobs
            {
                Gob_ShadowguardPortal = 276472,
                Gob_DeactivatedShadowguardPortal = 276473
            };

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2 && p_Unit->ToPlayer())
                {
                    go->DestroyForPlayer(p_Unit->ToPlayer());

                    if (go->GetEntry() == eGobs::Gob_ShadowguardPortal)
                    {
                        if (GameObject* l_Gob = p_Unit->SummonGameObject(eGobs::Gob_DeactivatedShadowguardPortal, go->GetPosition(), 0, 0, p_Unit->GetGUID()))
                        {
                            l_Gob->SetFlag(GAMEOBJECT_FIELD_FLAGS, 20);
                            l_Gob->SetGoState(GOState::GO_STATE_ACTIVE_ALTERNATIVE);
                            l_Gob->AddDelayedEvent([l_Gob]() -> void
                            {
                                l_Gob->Delete();
                            }, 360000);
                        }
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_hidden_in_the_hollows_gobsAI(go);
        }
};

/// Spire Stabilizer - 273991
class gob_spire_stabilizer_273991 : public GameObjectScript
{
    public:
        gob_spire_stabilizer_273991() : GameObjectScript("gob_spire_stabilizer_273991") { }

        struct gob_spire_stabilizer_273991AI : public GameObjectAI
        {
            gob_spire_stabilizer_273991AI(GameObject* go) : GameObjectAI(go) { }

            void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
            {
                if (p_State == 2 && p_Unit->ToPlayer())
                {
                    go->DestroyForPlayer(p_Unit->ToPlayer());
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_spire_stabilizer_273991AI(go);
        }
};

/// Rubbles - 277256, 276404, 277257, 273959, 276406, 273957
class gob_argus_rubbles_chests : public GameObjectScript
{
    public:
        gob_argus_rubbles_chests() : GameObjectScript("gob_argus_rubbles_chests") { }

        struct gob_argus_rubbles_chestsAI : public GameObjectAI
        {
            gob_argus_rubbles_chestsAI(GameObject* go) : GameObjectAI(go) { }

            void InitializeAI() override
            {
                go->SetUInt32Value(GAMEOBJECT_FIELD_SPELL_VISUAL_ID, 70986);
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_argus_rubbles_chestsAI(go);
        }
};

/// Rubbles - 273958, 276232, 277258, 276423
class gob_argus_rubbles_warframe_chests : public GameObjectScript
{
    public:
        gob_argus_rubbles_warframe_chests() : GameObjectScript("gob_argus_rubbles_warframe_chests") { }

        struct gob_argus_rubbles_warframe_chestsAI : public GameObjectAI
        {
            gob_argus_rubbles_warframe_chestsAI(GameObject* go) : GameObjectAI(go) { }

            void InitializeAI() override
            {
                go->SetUInt32Value(GAMEOBJECT_FIELD_SPELL_VISUAL_ID, 70989);
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_argus_rubbles_warframe_chestsAI(go);
        }
};

/// Call of Flames - 251411
class spell_gen_call_of_flames : public SpellScriptLoader
{
    public:
        spell_gen_call_of_flames() : SpellScriptLoader("spell_gen_call_of_flames") { }

        class spell_gen_call_of_flames_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_call_of_flames_SpellScript);

            enum eSpells
            {
                Spell_CallOfFlamesMissiles = 251412
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                uint16 l_Delay = 0;

                for (uint8 l_Itr = 0; l_Itr < 15; l_Itr++)
                {
                    l_Caster->DelayedCastSpell(l_Caster, eSpells::Spell_CallOfFlamesMissiles, true, l_Delay);
                    l_Delay += 300;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_call_of_flames_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_call_of_flames_SpellScript();
        }
};

/// Void Slip - 246476
class spell_gen_void_slip : public SpellScriptLoader
{
    public:
        spell_gen_void_slip() : SpellScriptLoader("spell_gen_void_slip") { }

        class spell_gen_void_slip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_void_slip_SpellScript);

            enum eSpells
            {
                Spell_VoidSlipDamage = 246483
            };

            enum eNpcs
            {
                Npc_VoidImage = 123889
            };

            enum ePoints
            {
                Point_JumpEnd = 1
            };

            void HandleHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Creature* l_Creature = l_Caster->SummonCreature(eNpcs::Npc_VoidImage, l_Caster->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 2000))
                {
                    l_Creature->GetMotionMaster()->MoveJump(l_Target->GetPosition(), 50.0f, 0.0f, ePoints::Point_JumpEnd, eSpells::Spell_VoidSlipDamage);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_void_slip_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_void_slip_SpellScript();
        }
};

/// Devastator Blast - 245879
class spell_gen_devastator_blast : public SpellScriptLoader
{
    public:
        spell_gen_devastator_blast() : SpellScriptLoader("spell_gen_devastator_blast") { }

        class spell_gen_devastator_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_devastator_blast_SpellScript);

            enum eNpcs
            {
                Npc_VfxBunny = 123594
            };

            enum eKillcredits
            {
                Killcredit_FireTheFelDevastator = 123569
            };

            enum eSpells
            {
                Spell_DevastatorBlastKillcredit = 252097,
                Spell_SummonCannon = 248679
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToTempSummon())
                    return;

                Unit* l_Summoner = l_Caster->ToTempSummon()->GetSummoner();
                if (!l_Summoner || !l_Summoner->ToPlayer())
                    return;

                if (l_Caster->GetOrientation() >= 2.1f && l_Caster->GetOrientation() <= 2.6f)
                {
                    uint64 l_PlayerGuid = l_Summoner->GetGUID();

                    l_Caster->AddDelayedEvent([l_Caster, l_PlayerGuid]() -> void
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Caster, l_PlayerGuid))
                        {
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FireTheFelDevastator);
                            l_Player->RemoveAura(eSpells::Spell_SummonCannon);
                        }

                        if (Vehicle* l_Vehicle = l_Caster->GetVehicleKit())
                        {
                            l_Vehicle->RemoveAllPassengers();
                        }
                    }, 2000);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_devastator_blast_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_devastator_blast_SpellScript();
        }
};

/// Extracting Core - 254710
class spell_gen_extracting_core : public SpellScriptLoader
{
    public:
        spell_gen_extracting_core() : SpellScriptLoader("spell_gen_extracting_core") { }

        class spell_gen_extracting_core_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_extracting_core_SpellScript);

            enum eNpcs
            {
                Npc_DamagedArgusConstruct = 128357
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                if (Creature* l_Creature = l_Caster->FindNearestCreature(eNpcs::Npc_DamagedArgusConstruct, 10.0f))
                {
                    l_Creature->DestroyForPlayer(l_Caster->ToPlayer());

                    if (Creature* l_NewCreature = l_Caster->SummonCreature(eNpcs::Npc_DamagedArgusConstruct, l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 360000, 0, l_Caster->GetGUID()))
                    {
                        l_NewCreature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_extracting_core_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_extracting_core_SpellScript();
        }
};

/// Oozing Leap - 254213
class spell_gen_oozing_leap: public SpellScriptLoader
{
    public:
        spell_gen_oozing_leap() : SpellScriptLoader("spell_gen_oozing_leap") { }

        class spell_gen_oozing_leap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_oozing_leap_SpellScript);

            enum eSpells
            {
                Spell_OozingLeapDamage = 254216
            };

            enum ePoints
            {
                Point_JumpEnd = 1
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->GetMotionMaster()->MoveJump(l_Target->GetPosition(), 20.0f, 10.0f, ePoints::Point_JumpEnd, eSpells::Spell_OozingLeapDamage);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_oozing_leap_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_gen_oozing_leap_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_oozing_leap_SpellScript();
        }
};

/// Dark Shift - 254071
class spell_gen_dark_shift : public SpellScriptLoader
{
    public:
        spell_gen_dark_shift() : SpellScriptLoader("spell_gen_dark_shift") { }

        class spell_gen_dark_shift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dark_shift_SpellScript);

            enum eSpells
            {
                Spell_DarkShiftTeleport = 254072
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Spell_DarkShiftTeleport, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_dark_shift_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_gen_dark_shift_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_dark_shift_SpellScript();
        }
};

/// Cross Over - 253032
class spell_gen_cross_over_wq : public SpellScriptLoader
{
    public:
        spell_gen_cross_over_wq() : SpellScriptLoader("spell_gen_cross_over_wq") { }

        class spell_gen_cross_over_wq_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cross_over_wq_SpellScript);

            enum eSpells
            {
                Spell_VoidRift = 253035,
                Spell_CancelVoidWalking = 253034
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                if (!l_Caster->HasAura(eSpells::Spell_VoidRift))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_VoidRift, true);
                    l_Caster->SetInPhase(48175, true, true);
                }
                else
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Spell_CancelVoidWalking, true);
                    l_Caster->SetInPhase(48175, true, false);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_cross_over_wq_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_cross_over_wq_SpellScript();
        }
};

/// Banish Doombringer - 253727
class spell_gen_banish_doombringer : public SpellScriptLoader
{
    public:
        spell_gen_banish_doombringer() : SpellScriptLoader("spell_gen_banish_doombringer") { }

        class spell_gen_banish_doombringer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_banish_doombringer_SpellScript);

            enum eNpcs
            {
                Npc_EverburningDoombringer = 127811
            };

            enum eKillcredits
            {
                Killcredit_BanishTheEverburningDoombringer = 127811
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                if (Creature* l_OldCreature = l_Caster->FindNearestCreature(eNpcs::Npc_EverburningDoombringer, 10.0f, true))
                {

                    l_OldCreature->DestroyForPlayer(l_Caster->ToPlayer());

                    if (Creature* l_Creature = l_Caster->SummonCreature(l_OldCreature->GetEntry(), l_OldCreature->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Caster->GetGUID()))
                    {
                        uint64 l_PlayerGuid = l_Caster->GetGUID();

                        l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                        l_Creature->AddDelayedEvent([l_Creature]() -> void
                        {
                            l_Creature->RemoveAllAuras();
                            l_Creature->SendPlaySpellVisualKit(88736, 4, 2000);
                            l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_DROWN);
                        }, 500);

                        l_Creature->AddDelayedEvent([l_Creature, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_PlayerGuid))
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_BanishTheEverburningDoombringer);

                            l_Creature->SendPlaySpellVisualKit(78943, 4, 2000);
                            l_Creature->DespawnOrUnsummon(2000);
                        }, 1000);
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_banish_doombringer_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_banish_doombringer_SpellScript();
        }
};

/// Sculpture Blessings - 252867, 252874, 252862
class spell_gen_sculpture_blessings_wq : public SpellScriptLoader
{
    public:
        spell_gen_sculpture_blessings_wq() : SpellScriptLoader("spell_gen_sculpture_blessings_wq") { }

        class spell_gen_sculpture_blessings_wq_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_sculpture_blessings_wq_SpellScript);

            enum eSpells
            {
                // Child Auras
                Spell_Red = 252863,
                Spell_Blue = 252858,
                Spell_Yellow = 252869,
                // Red Sculpture
                Spell_RedSculptureBlessingDummy = 252867,
                Spell_RedReceiveBlessingCast = 252865,
                Spell_RedRemoveChildAura = 252866,
                // Blue Sculpture
                Spell_BlueSculptureBlessingDummy = 252862,
                Spell_BlueReceiveBlessingCast = 252859,
                Spell_BlueRemoveChildAura = 252860,
                // Yellow Sculpture
                Spell_YellowSculptureBlessingDummy = 252874,
                Spell_YellowReceiveBlessingCast = 252872,
                Spell_YellowRemoveChildAura = 252873
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                switch (m_scriptSpellId)
                {
                case eSpells::Spell_RedSculptureBlessingDummy:
                {
                    if (!l_Caster->HasAura(eSpells::Spell_Red) && !l_Caster->HasAura(eSpells::Spell_Blue) && !l_Caster->HasAura(eSpells::Spell_Yellow))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_RedReceiveBlessingCast, false);
                    }
                    else if (!l_Caster->HasAura(eSpells::Spell_Red) && l_Caster->HasAura(eSpells::Spell_Blue))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_RedReceiveBlessingCast, false);
                    }
                    else if (!l_Caster->HasAura(eSpells::Spell_Red) && l_Caster->HasAura(eSpells::Spell_Yellow))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_RedReceiveBlessingCast, false);
                    }
                    else
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_RedRemoveChildAura, true);
                    }

                    break;
                }
                case eSpells::Spell_BlueSculptureBlessingDummy:
                {
                    if (!l_Caster->HasAura(eSpells::Spell_Blue) && !l_Caster->HasAura(eSpells::Spell_Red) && !l_Caster->HasAura(eSpells::Spell_Yellow))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_BlueReceiveBlessingCast, false);
                    }
                    else if (!l_Caster->HasAura(eSpells::Spell_Blue) && l_Caster->HasAura(eSpells::Spell_Red))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_BlueReceiveBlessingCast, false);
                    }
                    else if (!l_Caster->HasAura(eSpells::Spell_Blue) && l_Caster->HasAura(eSpells::Spell_Yellow))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_BlueReceiveBlessingCast, false);
                    }
                    else
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_BlueRemoveChildAura, true);
                    }

                    break;
                }
                case eSpells::Spell_YellowSculptureBlessingDummy:
                {
                    if (!l_Caster->HasAura(eSpells::Spell_Yellow) && !l_Caster->HasAura(eSpells::Spell_Blue) && !l_Caster->HasAura(eSpells::Spell_Red))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_YellowReceiveBlessingCast, false);
                    }
                    else if (!l_Caster->HasAura(eSpells::Spell_Yellow) && l_Caster->HasAura(eSpells::Spell_Blue))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_YellowReceiveBlessingCast, false);
                    }
                    else if (!l_Caster->HasAura(eSpells::Spell_Yellow) && l_Caster->HasAura(eSpells::Spell_Red))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_YellowReceiveBlessingCast, false);
                    }
                    else
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Spell_YellowRemoveChildAura, true);
                    }

                    break;
                }
                default:
                    break;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_sculpture_blessings_wq_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_sculpture_blessings_wq_SpellScript();
        }
};

/// Prismatic Child Auras - 252869, 252858, 252863
class spell_gen_prismatic_child_auras_wq : public SpellScriptLoader
{
    public:
        spell_gen_prismatic_child_auras_wq() : SpellScriptLoader("spell_gen_prismatic_child_auras_wq") { }

        class spell_gen_prismatic_child_auras_wq_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_prismatic_child_auras_wq_AuraScript);

            void ChangePhases(Unit* p_Caster)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_FragmentOfGuile) == 0)
                        l_Player->SetInPhase(480972, true, true);
                    if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_FragmentOfAcuity) == 0)
                        l_Player->SetInPhase(480971, true, true);
                    if (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_FragmentOfWit) == 0)
                        l_Player->SetInPhase(480970, true, true);
                }
            }

            enum eSpells
            {
                // Parent Aura
                Spell_Prismatic = 252905,
                // Child Auras
                Spell_Red = 252863,
                Spell_Blue = 252858,
                Spell_Yellow = 252869
            };

            enum eVisuals
            {
                Visual_Red = 119533,
                Visual_Blue = 119539,
                Visual_Yellow = 119538,
                Visual_Red_Blue_Purple = 119534,
                Visual_Red_Yellow_Orange = 119535,
                Visual_Yellow_Blue_Green = 119537,
                Visual_Yellow_Red_Blue = 119536
            };

            enum eObjectives
            {
                Obj_FragmentOfAcuity = 292267,
                Obj_FragmentOfWit = 292268,
                Obj_FragmentOfGuile = 292269
            };

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 SpellVisualId = 0;

                if (l_Caster->HasAura(Spell_Yellow) && l_Caster->HasAura(Spell_Red) && l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Yellow_Red_Blue;
                else if (l_Caster->HasAura(Spell_Red) && l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Red_Blue_Purple;
                else if (l_Caster->HasAura(Spell_Red) && l_Caster->HasAura(Spell_Yellow))
                    SpellVisualId = Visual_Red_Yellow_Orange;
                else if (l_Caster->HasAura(Spell_Yellow) && l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Yellow_Blue_Green;
                else if (l_Caster->HasAura(Spell_Red))
                    SpellVisualId = Visual_Red;
                else if (l_Caster->HasAura(Spell_Yellow))
                    SpellVisualId = Visual_Yellow;
                else if (l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Blue;

                Aura* l_Aura = l_Caster->GetAura(eSpells::Spell_Prismatic);
                if (!l_Aura)
                    l_Aura = l_Caster->AddAura(eSpells::Spell_Prismatic, l_Caster);

                l_Aura->SetSpellXSpellVisualID(SpellVisualId);

                AuraApplication* l_AuraApplication = l_Aura->GetApplicationOfTarget(l_Caster->GetGUID());
                if (!l_AuraApplication)
                    return;

                switch (SpellVisualId)
                {
                case eVisuals::Visual_Red_Blue_Purple:
                {
                    l_Caster->SetInPhase(480972, true, false);
                    break;
                }
                case eVisuals::Visual_Red_Yellow_Orange:
                {
                    l_Caster->SetInPhase(480970, true, false);
                    break;
                }
                case eVisuals::Visual_Yellow_Blue_Green:
                {
                    l_Caster->SetInPhase(480971, true, false);
                    break;
                }
                default:
                {
                    ChangePhases(l_Caster);
                    break;
                }
                }

                l_AuraApplication->ClientUpdate();
            }

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 SpellVisualId = 0;

                if (l_Caster->HasAura(Spell_Yellow) && l_Caster->HasAura(Spell_Red) && l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Yellow_Red_Blue;
                else if (l_Caster->HasAura(Spell_Red) && l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Red_Blue_Purple;
                else if (l_Caster->HasAura(Spell_Red) && l_Caster->HasAura(Spell_Yellow))
                    SpellVisualId = Visual_Red_Yellow_Orange;
                else if (l_Caster->HasAura(Spell_Yellow) && l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Yellow_Blue_Green;
                else if (l_Caster->HasAura(Spell_Red))
                    SpellVisualId = Visual_Red;
                else if (l_Caster->HasAura(Spell_Yellow))
                    SpellVisualId = Visual_Yellow;
                else if (l_Caster->HasAura(Spell_Blue))
                    SpellVisualId = Visual_Blue;

                Aura* l_Aura = l_Caster->GetAura(eSpells::Spell_Prismatic);
                if (!l_Aura)
                    l_Aura = l_Caster->AddAura(eSpells::Spell_Prismatic, l_Caster);

                l_Aura->SetSpellXSpellVisualID(SpellVisualId);

                AuraApplication* l_AuraApplication = l_Aura->GetApplicationOfTarget(l_Caster->GetGUID());
                if (!l_AuraApplication)
                    return;

                switch (SpellVisualId)
                {
                case eVisuals::Visual_Red_Blue_Purple:
                {
                    l_Caster->SetInPhase(480972, true, false);
                    break;
                }
                case eVisuals::Visual_Red_Yellow_Orange:
                {
                    l_Caster->SetInPhase(480970, true, false);
                    break;
                }
                case eVisuals::Visual_Yellow_Blue_Green:
                {
                    l_Caster->SetInPhase(480971, true, false);
                    break;
                }
                default:
                {
                    ChangePhases(l_Caster);
                    break;
                }
                }

                l_AuraApplication->ClientUpdate();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_prismatic_child_auras_wq_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_prismatic_child_auras_wq_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_prismatic_child_auras_wq_AuraScript();
        }
};

/// Order Bombardment - 251091
class spell_gen_order_bombardment_251091 : public SpellScriptLoader
{
    public:
        spell_gen_order_bombardment_251091() : SpellScriptLoader("spell_gen_order_bombardment_251091") { }

        class spell_gen_order_bombardment_251091_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_order_bombardment_251091_SpellScript);

            enum eSpells
            {
                Spell_FelBombardment = 251092
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint16 l_Delay = 0;

                for (uint8 l_Itr = 0; l_Itr < 40; l_Itr++)
                {
                    l_Delay += urand(200, 400);
                    l_Caster->DelayedCastSpell(l_Caster, eSpells::Spell_FelBombardment, true, l_Delay);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_order_bombardment_251091_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_order_bombardment_251091_SpellScript();
        }
};

/// Ruinous Strike - 254579
class spell_argus_ruinous_strike : public SpellScriptLoader
{
    public:
        spell_argus_ruinous_strike() : SpellScriptLoader("spell_argus_ruinous_strike") { }

        class spell_argus_ruinous_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_ruinous_strike_SpellScript);

            enum eSpells
            {
                Spell_RuinousStrikeCone = 254581
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
                l_Caster->CastSpell(l_Caster, eSpells::Spell_RuinousStrikeCone);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 2500);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_ruinous_strike_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_ruinous_strike_SpellScript();
        }
};

/// Avenging Wave - 254032, 254035
class spell_argus_avenging_wave : public SpellScriptLoader
{
    public:
        spell_argus_avenging_wave() : SpellScriptLoader("spell_argus_avenging_wave") { }

        class spell_argus_avenging_wave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_avenging_wave_SpellScript);

            enum eSpells
            {
                Spell_AvengingWaveArea = 254032,
                Spell_AvengingWaveCone = 254035,
                Spell_AvengingWaveMissile = 254036
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
                l_Caster->CastSpell(l_Caster, eSpells::Spell_AvengingWaveCone);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }, 3500);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_CasterPos = l_Caster->GetPosition();
                float l_Distance = 2.0f;

                for (uint8 l_Itr = 0; l_Itr < 25; l_Itr++)
                {
                    Position l_DestPos = l_CasterPos;
                    float l_Angle = frand(-0.5f, 0.5f);

                    l_DestPos.SimplePosXYRelocationByAngle(l_DestPos, l_Distance, l_DestPos.GetOrientation() + l_Angle, true);
                    l_DestPos.m_positionZ = l_Caster->GetMap()->GetHeight(l_DestPos.m_positionX, l_DestPos.m_positionY, l_DestPos.m_positionZ);
                    l_Caster->CastSpell(l_DestPos, eSpells::Spell_AvengingWaveMissile, true);
                    l_Distance += 1.5f;
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Spell_AvengingWaveArea)
                    OnEffectHitTarget += SpellEffectFn(spell_argus_avenging_wave_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
                else
                    AfterCast += SpellCastFn(spell_argus_avenging_wave_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_avenging_wave_SpellScript();
        }
};

/// Dark Torrent - 254046
class spell_argus_dark_torrent : public SpellScriptLoader
{
    public:
        spell_argus_dark_torrent() : SpellScriptLoader("spell_argus_dark_torrent") { }

        class sspell_argus_dark_torrent_SpellScript : public SpellScript
        {
            PrepareSpellScript(sspell_argus_dark_torrent_SpellScript);

            enum eSpells
            {
                Spell_DarkTorrentMissile = 254047
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Position l_CasterPos = l_Caster->GetPosition();
                Position l_TargetPos = l_Target->GetPosition();
                float l_Distance = 6.0f;

                for (uint8 l_Itr = 0; l_Itr < 5; l_Itr++)
                {
                    Position l_DestPos = l_CasterPos;
                    l_DestPos.m_orientation = l_TargetPos.GetOrientation();

                    l_DestPos.SimplePosXYRelocationByAngle(l_DestPos, l_Distance, l_CasterPos.GetOrientation(), true);
                    l_DestPos.m_positionZ = l_Caster->GetMap()->GetHeight(l_DestPos.m_positionX, l_DestPos.m_positionY, l_DestPos.m_positionZ);
                    l_Caster->CastSpell(l_DestPos, eSpells::Spell_DarkTorrentMissile, true);
                    l_Distance += 6.0f;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(sspell_argus_dark_torrent_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new sspell_argus_dark_torrent_SpellScript();
        }
};

/// Teleport to/from Observer's Locus - 254665
class spell_argus_teleport_to_observers_locus : public SpellScriptLoader
{
    public:
        spell_argus_teleport_to_observers_locus() : SpellScriptLoader("spell_argus_teleport_to_observers_locus") { }

        class spell_argus_teleport_to_observers_locus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_teleport_to_observers_locus_SpellScript);

            enum eAreas
            {
                Area_ObserversLocus = 9308
            };

            Position m_ObserversLocusTeleportPos = { -3552.61f, 9004.01f, -54.35f, 2.036974f };
            Position m_OrixTheAllSeerCaveTeleportPos = { -2817.93f, 9237.08f, -196.60f, 3.960419f };

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (l_Player->GetAreaId() != eAreas::Area_ObserversLocus)
                        {
                            l_Player->SaveRecallPosition();
                            l_Player->TeleportTo(1669, m_ObserversLocusTeleportPos);
                        }
                        else
                        {
                            if (l_Player->GetMap()->GetAreaId(l_Player->m_recallX, l_Player->m_recallY, l_Player->m_recallZ) != eAreas::Area_ObserversLocus)
                            {
                                l_Player->TeleportTo(l_Player->m_recallMap, l_Player->m_recallX, l_Player->m_recallY, l_Player->m_recallZ, l_Player->m_recallO);
                            }
                            else
                            {
                                l_Player->TeleportTo(1669, m_OrixTheAllSeerCaveTeleportPos);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_teleport_to_observers_locus_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_teleport_to_observers_locus_SpellScript();
        }
};

/// Empower Effigy - 253223
class spell_argus_empower_effigy : public SpellScriptLoader
{
    public:
        spell_argus_empower_effigy() : SpellScriptLoader("spell_argus_empower_effigy") { }

        class spell_argus_empower_effigy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_empower_effigy_SpellScript);

            enum eNpcs
            {
                Npc_TheManyFacedDevourer = 127581
            };

            Position m_TheManyFacedDevourerSpawnPos = { -2671.696f, 9408.8f, -163.8711f, 2.199784f };

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Creature* l_Target = GetHitCreature();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Map* l_Map = l_Player->GetMap();
                if (!l_Map)
                    return;

                if (uint32 l_TrackingQuestId = Vignette::GetTrackingQuestIdFromWorldObject(l_Target))
                {
                    uint32 l_QuestBit = GetQuestUniqueBitFlag(l_TrackingQuestId);
                    if (l_Player->IsQuestBitFlaged(l_QuestBit))
                        return;

                    l_Player->AddTrackingQuestIfNeeded(l_Target->GetGUID());
                }

                l_Map->SummonCreature(eNpcs::Npc_TheManyFacedDevourer, m_TheManyFacedDevourerSpawnPos);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_empower_effigy_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_empower_effigy_SpellScript();
        }
};

/// Siphoning Souls - 251615
class spell_argus_siphoning_souls : public SpellScriptLoader
{
    public:
        spell_argus_siphoning_souls() : SpellScriptLoader("spell_argus_siphoning_souls") { }

        class spell_argus_siphoning_souls_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_siphoning_souls_AuraScript);

            enum eSpells
            {
                Spell_SiphoningSoulsVisual = 251615,
                Spell_SiphoningSoulsMain = 251663
            };

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::Spell_SiphoningSoulsMain))
                {
                    l_Target->CastSpell(l_Target, eSpells::Spell_SiphoningSoulsMain, true);
                }
            }

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::Spell_SiphoningSoulsVisual))
                {
                    l_Target->RemoveAura(eSpells::Spell_SiphoningSoulsMain);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_argus_siphoning_souls_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_siphoning_souls_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_siphoning_souls_AuraScript();
        }
};

/// Open Augari Chest - 255145
class spell_argus_open_augari_chest : public SpellScriptLoader
{
    public:
        spell_argus_open_augari_chest() : SpellScriptLoader("spell_argus_open_augari_chest") { }

        class spell_argus_open_augari_chest_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_open_augari_chest_SpellScript);

            enum eSpells
            {
                Spell_ShroudOfArcaneEchoes = 248779
            };

            SpellCastResult HandleOnCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                if (!l_Caster->HasAura(eSpells::Spell_ShroudOfArcaneEchoes))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_REQUIRES_POWER_THAT_ECHOES_THAT_OF_THE_AUGARI);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_argus_open_augari_chest_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_argus_open_augari_chest_SpellScript();
        }
};

/// Light's Judgment - 247427
class spell_argus_lights_judgment_rubbles : public SpellScriptLoader
{
    public:
        spell_argus_lights_judgment_rubbles() : SpellScriptLoader("spell_argus_lights_judgment_rubbles") { }

        class spell_argus_lights_judgment_rubbles_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_lights_judgment_rubbles_SpellScript);

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                GameObject* l_Gob = GetHitGObj();
                if (!l_Caster || !l_Caster->ToPlayer() || !l_Gob || l_Gob->GetUInt32Value(GAMEOBJECT_FIELD_SPELL_VISUAL_ID) != 70986)
                    return;

                uint64 l_PlayerGuid = l_Caster->GetGUID();
                l_Gob->SendGameObjectActivateAnimKit(8182, true, l_Caster->ToPlayer());

                l_Gob->AddDelayedEvent([l_Gob, l_PlayerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Gob, l_PlayerGuid))
                    {
                        l_Gob->DestroyForPlayer(l_Player);
                    }
                }, 1500);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_lights_judgment_rubbles_SpellScript::HandleOnEffectHit, EFFECT_3, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_lights_judgment_rubbles_SpellScript();
        }
};

/// Open Rocks - 254743
class spell_argus_warframe_rubbles : public SpellScriptLoader
{
    public:
        spell_argus_warframe_rubbles() : SpellScriptLoader("spell_argus_warframe_rubbles") { }

        class spell_argus_warframe_rubbles_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_warframe_rubbles_SpellScript);

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                GameObject* l_Gob = GetHitGObj();
                if (!l_Caster || !l_Gob || l_Gob->GetUInt32Value(GAMEOBJECT_FIELD_SPELL_VISUAL_ID) != 70989)
                    return;

                Unit* l_Owner = l_Caster->GetAnyOwner();
                if (!l_Owner || !l_Owner->ToPlayer())
                    return;

                uint64 l_PlayerGuid = l_Owner->GetGUID();
                l_Gob->SendGameObjectActivateAnimKit(8182, true, l_Owner->ToPlayer());

                l_Gob->AddDelayedEvent([l_Gob, l_PlayerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Gob, l_PlayerGuid))
                    {
                        l_Gob->DestroyForPlayer(l_Player);
                    }
                }, 1500);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_warframe_rubbles_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_warframe_rubbles_SpellScript();
        }
};

/// Explosive Munitions - 250771, 250772, 250773
class spell_argus_explosive_munitions : public SpellScriptLoader
{
    public:
        spell_argus_explosive_munitions() : SpellScriptLoader("spell_argus_explosive_munitions") { }

        class spell_argus_explosive_munitions_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_explosive_munitions_AuraScript);

            enum eSpells
            {
                Spell_ExplosiveMunitionsOverrideAura = 250771
            };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::Spell_ExplosiveMunitionsOverrideAura);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_explosive_munitions_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_explosive_munitions_AuraScript();
        }

        class spell_argus_explosive_munitions_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_explosive_munitions_SpellScript);

            enum eSpells
            {
                Spell_ExplosiveMunitionsOverrideAura = 250771,
                Spell_ExplosiveMunitionsDummyAura = 250772,
                Spell_ExplosiveMunitionsDamage = 250773
            };

            void HandleAfterCastOverride()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Spell_ExplosiveMunitionsDummyAura))
                {
                    uint32 l_MaxStackAmount = l_Aura->GetSpellInfo()->StackAmount;
                    uint32 l_NewStackAmount = l_Aura->GetStackAmount() + 3;

                    l_Aura->SetStackAmount(std::min(l_MaxStackAmount, l_NewStackAmount));
                }
                else
                {
                    l_Caster->SetAuraStack(eSpells::Spell_ExplosiveMunitionsDummyAura, l_Caster, 3);
                }
            }

            void HandleAfterCastDamage()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Spell_ExplosiveMunitionsDummyAura))
                {
                    l_Aura->ModStackAmount(-1);
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Spell_ExplosiveMunitionsOverrideAura)
                    AfterCast += SpellCastFn(spell_argus_explosive_munitions_SpellScript::HandleAfterCastOverride);
                else if (m_scriptSpellId == eSpells::Spell_ExplosiveMunitionsDamage)
                    AfterCast += SpellCastFn(spell_argus_explosive_munitions_SpellScript::HandleAfterCastDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_explosive_munitions_SpellScript();
        }
};

/// Empowered Doom - 253545
class spell_argus_empowered_doom : public SpellScriptLoader
{
    public:
        spell_argus_empowered_doom() : SpellScriptLoader("spell_argus_empowered_doom") { }

        class spell_argus_empowered_doom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_empowered_doom_AuraScript);

            enum eSpells
            {
                Spell_EmpoweredDoomInstakill = 253554
            };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::Spell_EmpoweredDoomInstakill);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_empowered_doom_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_empowered_doom_AuraScript();
        }
};

class playerscript_argus_world_quest_scripts : public PlayerScript
{
    public:
        playerscript_argus_world_quest_scripts() : PlayerScript("playerscript_argus_world_quest_scripts") {}

        enum eQuests
        {
            Quest_ForTheirOwnGood = 47135,
            Quest_GatekeepersChallengeCunning = 48097
        };

        enum eObjectives
        {
            Obj_FragmentOfAcuity = 292267,
            Obj_FragmentOfWit = 292268,
            Obj_FragmentOfGuile = 292269
        };

        enum eSpells
        {
            Spell_CalmingWaveOverride = 243997,
            Spell_RemoveAllChildAuras = 252910
        };

        enum eItems
        {
            Item_FragmentOfAcuity = 151126,
            Item_FragmentOfWit = 151127,
            Item_FragmentOfGuile = 151128
        };

        void OnQuestAccept(Player * p_Player, const Quest * p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
            case eQuests::Quest_ForTheirOwnGood:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_CalmingWaveOverride, true);
                break;
            }
            case eQuests::Quest_GatekeepersChallengeCunning:
            {
                if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_FragmentOfGuile) == 0)
                    p_Player->SetInPhase(480972, true, true);
                if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_FragmentOfAcuity) == 0)
                    p_Player->SetInPhase(480971, true, true);
                if (p_Player->GetQuestObjectiveCounter(eObjectives::Obj_FragmentOfWit) == 0)
                    p_Player->SetInPhase(480970, true, true);

                break;
            }
            default:
                break;
            }
        }

        void OnQuestAbandon(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
            case eQuests::Quest_ForTheirOwnGood:
            {
                p_Player->RemoveAura(eSpells::Spell_CalmingWaveOverride);
                break;
            }
            default:
                break;
            }
        }

        void OnQuestReward(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
            case eQuests::Quest_ForTheirOwnGood:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_CalmingWaveOverride, true);
                break;
            }
            case eQuests::Quest_GatekeepersChallengeCunning:
            {
                p_Player->SetInPhase(480972, true, false);
                p_Player->SetInPhase(480971, true, false);
                p_Player->SetInPhase(480970, true, false);
                break;
            }
            default:
                break;
            }
        }

        void OnItemLooted(Player* p_Player, Item* p_Item) override
        {
            switch (p_Item->GetEntry())
            {
            case  eItems::Item_FragmentOfAcuity:
            {
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_RemoveAllChildAuras, true, 1000);
                break;
            }
            case  eItems::Item_FragmentOfWit:
            {
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_RemoveAllChildAuras, true, 1000);
                break;
            }
            case  eItems::Item_FragmentOfGuile:
            {
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_RemoveAllChildAuras, true, 1000);
                break;
            }
            default:
                break;
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_argus_world_quest_scripts()
{
    new npc_sister_subversia_123464();
    new npc_devastator_anchors_besieged();
    new npc_tome_of_torment_122937();
    new npc_tortured_slave_122974();
    new npc_lightforged_armament();
    new npc_legion_hullbreaker_127058();
    new npc_slave_trap_127722();
    new npc_nathraxxan_rift_126911();
    new npc_test_subjects();
    new npc_fel_devastator_123467();
    new npc_broken_soul_126944();
    new npc_lightforged_dragoons();
    new npc_frenzied_marsuul_kit_123764();
    new npc_lightforged_scout_125926();
    new npc_shadowguard_hologram_127527();
    new npc_wrangler_kravos_126852();
    new npc_legion_portal_127271();
    new npc_jedhin_adept_vehicle();
    new npc_jedhin_adept_opponent();
    new npc_household_vigilant_125842();
    new npc_gatekeepers_image_127403();
    new npc_immortal_squadron_fighter_127012();
    new npc_artificers_expanding_explosives();
    new npc_lightforged_warframe_126844();
    new npc_gladius_quest_creatures();
    new npc_void_manifestation_124398();
    new npc_mother_rosula_127705();
    new npc_resonating_augari();
    new npc_vishaxs_portal_127943();
    new npc_squadron_commander_vishax_127700();
    new npc_doomcaster_suprax_control_stalker();
    new gob_leaking_fel_spreader_272313();
    new gob_smelting_vessel_276918();
    new gob_hidden_in_the_hollows_gobs();
    new gob_spire_stabilizer_273991();
    new gob_argus_rubbles_chests();
    new gob_argus_rubbles_warframe_chests();
    new spell_gen_call_of_flames();
    new spell_gen_void_slip();
    new spell_gen_devastator_blast();
    new spell_gen_extracting_core();
    new spell_gen_oozing_leap();
    new spell_gen_dark_shift();
    new spell_gen_cross_over_wq();
    new spell_gen_banish_doombringer();
    new spell_gen_sculpture_blessings_wq();
    new spell_gen_prismatic_child_auras_wq();
    new spell_gen_order_bombardment_251091();
    new spell_argus_ruinous_strike();
    new spell_argus_avenging_wave();
    new spell_argus_dark_torrent();
    new spell_argus_teleport_to_observers_locus();
    new spell_argus_empower_effigy();
    new spell_argus_siphoning_souls();
    new spell_argus_open_augari_chest();
    new spell_argus_lights_judgment_rubbles();
    new spell_argus_warframe_rubbles();
    new spell_argus_explosive_munitions();
    new spell_argus_empowered_doom();
    new playerscript_argus_world_quest_scripts();
}
#endif
