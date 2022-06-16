////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "instance_argus_arcatraz.h"

/// High Inquisitor Raalgar - 124511
class npc_high_inquisitor_raalgar_124511 : public CreatureScript
{
    public:
        npc_high_inquisitor_raalgar_124511() : CreatureScript("npc_high_inquisitor_raalgar_124511") { }

        struct npc_high_inquisitor_raalgar_124511AI : public ScriptedAI
        {
            npc_high_inquisitor_raalgar_124511AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_InquisitorMoveToPortalEnd = 1
            };

            Position m_PortalBunnySpawnPos = { 170.8594f, -77.58507f, 961.3751f };
            Position m_MoveToPortalPos = { 171.8036f, -77.64638f, 959.6131f };

            void MoveInLineOfSight(Unit* p_Who) override
            {
                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                InstanceScript* l_Instance = me->GetInstanceScript();
                if (!l_Instance)
                    return;

                if (me->GetExactDist2d(p_Who) < 70.0f && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_HighInquisitorRaalgarFound) == 0 &&
                    l_Player->HasQuest(eQuests::Quest_FoilingTheLegionsJailbreak))
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_HighInquisitorRaalgarFound);
                    l_Instance->SetData(0, eDatas::Data_InquisitorFound);

                    if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_GenericBunnyKmart, 10.0f, true))
                    {
                        l_Creature->SendPlaySpellVisualKit(76924, 0, 0);
                        l_Creature->DespawnOrUnsummon();
                    }

                    std::list<Creature*> l_FelshadowSeekersList;
                    me->GetCreatureListWithEntryInGrid(l_FelshadowSeekersList, eNpcs::Npc_FelshadowSeeker, 20.0f);

                    if (!l_FelshadowSeekersList.empty())
                    {
                        for (auto l_Itr : l_FelshadowSeekersList)
                        {
                            l_Itr->SetAIAnimKitId(8337);
                        }
                    }

                    std::list<Creature*> l_EredarEnforcerList;
                    me->GetCreatureListWithEntryInGrid(l_EredarEnforcerList, eNpcs::Npc_EredarEnforcer, 40.0f);

                    if (!l_EredarEnforcerList.empty())
                    {
                        for (auto l_Itr : l_EredarEnforcerList)
                        {
                            l_Itr->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);

                            l_Itr->AddDelayedEvent([l_Itr]() -> void
                            {
                                l_Itr->SetFacingTo(0.01745329f);
                            }, 1000);

                            l_Itr->AddDelayedEvent([l_Itr]() -> void
                            {
                                l_Itr->HandleEmoteCommand(EMOTE_ONESHOT_BATTLEROAR);
                            }, 4000);


                            l_Itr->AddDelayedEvent([l_Itr]() -> void
                            {
                                l_Itr->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 27);
                                l_Itr->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                                l_Itr->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
                            }, 8000);
                        }
                    }

                    Talk(0);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                case eActions::Action_HighInquisitorRaalgarStartEvent:
                {
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetAIAnimKitId(10419);
                    }, 2000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetAIAnimKitId(0);
                    }, 6000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        Talk(1);
                    }, 8000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_GenericBunnyKmart, m_PortalBunnySpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::Spell_CreateLegionPortal, true);
                            l_Creature->DespawnOrUnsummon(19000);
                        }

                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_InquisitorMoveToPortalEnd, m_MoveToPortalPos);
                    }, 15000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        std::list<Creature*> l_FelshadowSeekersList;
                        me->GetCreatureListWithEntryInGrid(l_FelshadowSeekersList, eNpcs::Npc_FelshadowSeeker, 20.0f);

                        if (!l_FelshadowSeekersList.empty())
                        {
                            for (auto l_Itr : l_FelshadowSeekersList)
                            {
                                l_Itr->CastSpell(l_Itr, eSpells::Spell_FelChanneling, true);

                                l_Itr->AddDelayedEvent([l_Itr]() -> void
                                {
                                    l_Itr->CastStop();
                                    l_Itr->SetAIAnimKitId(8337);
                                }, 5000);
                            }
                        }
                    }, 28000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        DoCast(eSpells::Spell_FelDissolveOut, true);
                        me->DespawnOrUnsummon();
                    }, 31000);

                    break;
                }
                default:
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_high_inquisitor_raalgar_124511AI(p_Creature);
        }
};

/// Lah'zaruun - 121320
class npc_lahzaruun_121320 : public CreatureScript
{
    public:
        npc_lahzaruun_121320() : CreatureScript("npc_lahzaruun_121320") { }

        struct npc_lahzaruun_121320AI : public ScriptedAI
        {
            npc_lahzaruun_121320AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum ePoints
            {
                Point_MoveToInquisitorEnd = 1,
                Point_MoveToFightPosEnd = 2
            };

            enum eCombatSpells
            {
                Spell_BurningClaw = 254243,
                Spell_UnleashDarkness = 254394,
                Spell_FelBulwark = 254356
            };

            enum eEvents
            {
                Event_CastBurningClaw = 1,
                Event_CastUnleashDarkness = 2,
                Event_CastFelBulwark = 3
            };

            EventMap m_Events;
            Position m_MoveToInquisitorPos = { 191.4725f, -80.01795f, 959.6131f };
            Position m_MoveToFightPos = { 197.7203f, -78.21935f, 960.6854f };
            bool m_EightyPctLine = false;
            bool m_FiftyPctLine = false;
            bool m_TenPctLine = false;

            void JustReachedHome() override
            {
                me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 27);
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                me->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
            }

            void Reset() override
            {
                m_Events.Reset();
                m_EightyPctLine = false;
                m_FiftyPctLine = false;
                m_TenPctLine = false;
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(3);
                m_Events.ScheduleEvent(eEvents::Event_CastBurningClaw, 3000);
                m_Events.ScheduleEvent(eEvents::Event_CastUnleashDarkness, 6000);
                m_Events.ScheduleEvent(eEvents::Event_CastFelBulwark, 13000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();
                if (!l_Instance)
                    return;

                l_Instance->SetData(0, eDatas::Data_LahzaruunSlained);
            }

            void DamageTaken(Unit* p_Attacker, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetHealthPct() <= 80 && !m_EightyPctLine)
                {
                    Talk(4);
                    m_EightyPctLine = true;
                }
                if (me->GetHealthPct() <= 50 && !m_FiftyPctLine)
                {
                    Talk(5);

                    std::list<Creature*> l_FelshadowSeekersList;
                    me->GetCreatureListWithEntryInGrid(l_FelshadowSeekersList, eNpcs::Npc_FelshadowSeeker, 100.0f);

                    if (!l_FelshadowSeekersList.empty())
                    {
                        for (auto l_Itr : l_FelshadowSeekersList)
                        {
                            if (l_Itr->HasAura(eSpells::Spell_FelShield))
                            {
                                l_Itr->SetAIAnimKitId(0);
                                l_Itr->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                                l_Itr->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
                                l_Itr->RemoveAura(eSpells::Spell_FelShield);
                                l_Itr->AI()->AttackStart(p_Attacker);
                            }
                        }
                    }

                    m_FiftyPctLine = true;
                }
                if (me->GetHealthPct() <= 10 && !m_TenPctLine)
                {
                    Talk(6);
                    m_TenPctLine = true;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                case eActions::Action_LahzaruunStartEvent:
                {
                    me->GetMotionMaster()->Clear();
                    me->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    me->GetMotionMaster()->InitDefault();
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToInquisitorEnd, m_MoveToInquisitorPos);
                    break;
                }
                case eActions::Action_LahzaruunMoveToFightPos:
                {
                    me->GetMotionMaster()->Clear();
                    me->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    me->GetMotionMaster()->InitDefault();
                    me->SetWalk(true);
                    me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 27);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToFightPosEnd, m_MoveToFightPos);
                    break;
                }
                default:
                    break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_PointId) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                switch (p_PointId)
                {
                case ePoints::Point_MoveToInquisitorEnd:
                {
                    if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_HighInquisitorRaalgar, 50.0f))
                    {
                        l_Creature->AI()->DoAction(eActions::Action_HighInquisitorRaalgarStartEvent);
                    }

                    me->SetFacingTo(2.919225f);

                    me->AddDelayedEvent([this]() -> void
                    {
                        Talk(0);
                    }, 4000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                    }, 15000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        Talk(1);
                    }, 21000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToFightPosEnd, m_MoveToFightPos);
                    }, 24000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        Talk(2);
                    }, 25000);

                    me->AddDelayedEvent([this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 27);
                        me->SetUInt32Value(UNIT_FIELD_FLAGS, 32768);
                        me->SetUInt32Value(UNIT_FIELD_FLAGS_3, 0);
                    }, 28000);

                    break;
                }
                case ePoints::Point_MoveToFightPosEnd:
                {
                    me->SetFacingTo(0.0f);
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

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case eEvents::Event_CastBurningClaw:
                {
                    DoCast(eCombatSpells::Spell_BurningClaw);
                    m_Events.ScheduleEvent(eEvents::Event_CastBurningClaw, urand(7000, 15000));
                    break;
                }
                case eEvents::Event_CastUnleashDarkness:
                {
                    DoCast(eCombatSpells::Spell_UnleashDarkness);
                    m_Events.ScheduleEvent(eEvents::Event_CastUnleashDarkness, urand(15000, 17000));
                    break;
                }
                case eEvents::Event_CastFelBulwark:
                {
                    DoCast(eCombatSpells::Spell_FelBulwark);
                    m_Events.ScheduleEvent(eEvents::Event_CastFelBulwark, urand(18000, 27000));
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
            return new npc_lahzaruun_121320AI(p_Creature);
        }
};

/// Permanent Feign Death - 145362
class spell_argus_arcatraz_feign_death : public SpellScriptLoader
{
    public:
        spell_argus_arcatraz_feign_death() : SpellScriptLoader("spell_argus_arcatraz_feign_death") { }

        class spell_argus_arcatraz_feign_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_arcatraz_feign_death_AuraScript);

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                l_Caster->SetUInt32Value(UNIT_FIELD_FLAGS, 570721024);
                l_Caster->SetUInt32Value(UNIT_FIELD_FLAGS_2, 2049);
                l_Caster->SetUInt32Value(UNIT_FIELD_FLAGS_3, 8192);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_argus_arcatraz_feign_death_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_arcatraz_feign_death_AuraScript();
        }
};

/// Fel Channeling - 244481
class spell_argus_arcatraz_fel_channeling : public SpellScriptLoader
{
    public:
        spell_argus_arcatraz_fel_channeling() : SpellScriptLoader("spell_argus_arcatraz_fel_channeling") { }

        class spell_argus_arcatraz_fel_channeling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_arcatraz_fel_channeling_AuraScript);

            enum eSpells
            {
                Spell_FelShield = 245218
            };

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Target->ToCreature())
                    return;

                if (l_Target->ToCreature()->ComputeLinkedId() == "789834F36F74BD55DCBAE4800B0666166E835CD9")
                {
                    Remove();
                    return;
                }

                if (l_Target->HasAura(eSpells::Spell_FelShield))
                {
                    l_Target->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
                }
            }

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Target->ToCreature())
                    return;

                if (!l_Target->HasAura(GetId()))
                {
                    l_Target->RemoveAura(eSpells::Spell_FelShield);
                    l_Target->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_argus_arcatraz_fel_channeling_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_arcatraz_fel_channeling_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_arcatraz_fel_channeling_AuraScript();
        }
};

/// Fel Bulwark - 254356
class spell_argus_arcatraz_fel_bulwark : public SpellScriptLoader
{
    public:
        spell_argus_arcatraz_fel_bulwark() : SpellScriptLoader("spell_argus_arcatraz_fel_bulwark") { }

        class spell_argus_arcatraz_fel_bulwark_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_arcatraz_fel_bulwark_SpellScript);

            enum eSpells
            {
                Spell_FelBulwarkShield = 254342
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                {
                    if (Creature* l_Target = p_Target->ToCreature())
                    {
                        if (l_Target == l_Caster || l_Caster->IsFriendlyTo(l_Target))
                            return false;
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

                l_Caster->CastSpell(l_Target, eSpells::Spell_FelBulwarkShield);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_arcatraz_fel_bulwark_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_argus_arcatraz_fel_bulwark_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_arcatraz_fel_bulwark_SpellScript();
        }
};

/// Teleport Players to Argus Vindicaar - 241999
class spell_argus_arcatraz_teleport_players_to_argus_vindicaar : public SpellScriptLoader
{
    public:
        spell_argus_arcatraz_teleport_players_to_argus_vindicaar() : SpellScriptLoader("spell_argus_arcatraz_teleport_players_to_argus_vindicaar") { }

        class spell_argus_arcatraz_teleport_players_to_argus_vindicaar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_arcatraz_teleport_players_to_argus_vindicaar_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToPlayer())
                    return;

                if (l_Caster->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_LahzaruunSlain) == 1)
                {
                    l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_EscapeFromTheArcatraz);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_argus_arcatraz_teleport_players_to_argus_vindicaar_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_arcatraz_teleport_players_to_argus_vindicaar_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_argus_arcatraz()
{
    new npc_high_inquisitor_raalgar_124511();
    new npc_lahzaruun_121320();
    new spell_argus_arcatraz_feign_death();
    new spell_argus_arcatraz_fel_channeling();
    new spell_argus_arcatraz_fel_bulwark();
    new spell_argus_arcatraz_teleport_players_to_argus_vindicaar();
}
#endif
