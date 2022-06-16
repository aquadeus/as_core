////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

/// Last Update 7.1.5 Build 23420
/// The Curator - 114247
class boss_the_curator : public CreatureScript
{
    public:
        boss_the_curator() : CreatureScript("boss_the_curator") { }

        struct boss_the_curatorAI : public BossAI
        {
            boss_the_curatorAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataCurator)
            {
                me->GetMotionMaster()->MovePath(me->GetEntry() * 100, true);
            }

            enum eTalks
            {
                Aggro           = 0,
                TalkEvocation   = 1,
                Death           = 2
            };

            enum eSpells
            {
                CosmeticManaDrain           = 228840,
                VolatileEnergyFilter        = 234416,
                VolatileEnergy              = 227267,
                PowerDischargeFilter        = 227278,
                Evocation                   = 227254,
                Overload                    = 227257,
            };

            enum eEvents
            {
                EventSummonVolatileEnergy   = 1,
                EventPowerDischarge         = 2,
                EventManaCheck              = 3,
            };

            bool p_PhaseRegenMana;
            bool m_VolatileEnergyCastedTwice;

            Position m_EntranceTunnel = { -11034.72f, -1843.79f, 165.77f };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.85f;
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::DataCurator, EncounterState::FAIL);
            }

            void Reset() override
            {
                _Reset();
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                p_PhaseRegenMana = false;
                m_VolatileEnergyCastedTwice = false;
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA));

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    _Reset();
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);
                _EnterCombat();
                DoCast(me, eSpells::CosmeticManaDrain, true);
                DefaultEvent();

                events.ScheduleEvent(eEvents::EventManaCheck, 500);
            }

            void DefaultEvent()
            {
                events.ScheduleEvent(eEvents::EventSummonVolatileEnergy, 500);
                events.ScheduleEvent(eEvents::EventPowerDischarge, 13000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);

                Creature* l_Cristal = me->FindNearestCreature(eCreatures::CuratorCristal, 50.0f);
                if (l_Cristal)
                    l_Cristal->SetVisible(true);

                _JustDied();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->GetExactDist(&m_EntranceTunnel) < 5.0f)
                    EnterEvadeMode();

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSummonVolatileEnergy:
                    {
                        DoCast(me, eSpells::VolatileEnergyFilter, true);
                        if (!m_VolatileEnergyCastedTwice)
                        {
                            events.ScheduleEvent(eEvents::EventSummonVolatileEnergy, 12000);
                            m_VolatileEnergyCastedTwice = true;
                        }
                        break;
                    }
                    case eEvents::EventPowerDischarge:
                    {
                        DoCast(me, eSpells::PowerDischargeFilter, true);
                        events.ScheduleEvent(eEvents::EventPowerDischarge, 14000);
                        break;
                    }
                    case eEvents::EventManaCheck:
                    {
                        if (me->GetPowerPct(POWER_MANA) < 1.0f && !p_PhaseRegenMana)
                        {
                            p_PhaseRegenMana = true;
                            m_VolatileEnergyCastedTwice = false;
                            events.CancelEvent(eEvents::EventSummonVolatileEnergy);
                            events.CancelEvent(eEvents::EventPowerDischarge);
                            me->StopAttack();
                            me->RemoveAurasDueToSpell(eSpells::CosmeticManaDrain);
                            DoCast(me, eSpells::Evocation, true);
                            Talk(eTalks::TalkEvocation);
                        }
                        else if (me->GetPowerPct(POWER_MANA) > 99.0f && p_PhaseRegenMana)
                        {
                            p_PhaseRegenMana = false;
                            DefaultEvent();
                            DoCast(me, eSpells::Overload, true);
                            DoCast(me, eSpells::CosmeticManaDrain, true);
                            me->SetReactState(REACT_AGGRESSIVE);
                        }

                        events.ScheduleEvent(eEvents::EventManaCheck, 500);
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
            return new boss_the_curatorAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Summon Volatile Energy - 234416
class spell_rtk_summon_volatile_energy : public SpellScriptLoader
{
    public:
        spell_rtk_summon_volatile_energy() : SpellScriptLoader("spell_rtk_summon_volatile_energy") { }

        class spell_rtk_summon_volatile_energy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_summon_volatile_energy_SpellScript);

            enum eSpells
            {
                SummonVolatileEnergy = 227267
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SummonVolatileEnergy, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rtk_summon_volatile_energy_SpellScript::HandleAfterHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_summon_volatile_energy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_summon_volatile_energy_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Static Charge - 227270
class spell_rtk_static_charge : public SpellScriptLoader
{
    public:
        spell_rtk_static_charge() : SpellScriptLoader("spell_rtk_static_charge") { }

        class spell_rtk_static_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_static_charge_SpellScript);

            enum eSpells
            {
                StaticCharge = 228736
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::StaticCharge, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rtk_static_charge_SpellScript::HandleAfterHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_static_charge_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_static_charge_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Power Discharge - 227270
class spell_rtk_power_discharge : public SpellScriptLoader
{
    public:
        spell_rtk_power_discharge() : SpellScriptLoader("spell_rtk_power_discharge") { }

        class spell_rtk_power_discharge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_power_discharge_SpellScript);

            enum eSpells
            {
                PowerDischarge = 227279
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PowerDischarge, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rtk_power_discharge_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_power_discharge_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Arc Lightning - 227268
class spell_curator_arc_lightning_trigger : public SpellScriptLoader
{
    public:
        spell_curator_arc_lightning_trigger() : SpellScriptLoader("spell_curator_arc_lightning_trigger") { }

        class spell_curator_arc_lightning_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_curator_arc_lightning_trigger_AuraScript);

            enum eSpells
            {
                ArcLightning = 227270
            };

            void OnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Player*> l_Targets;
                JadeCore::AnyPlayerInObjectRangeCheck l_Check(l_Caster, 100.0f);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(l_Caster, l_Targets, l_Check);
                l_Caster->VisitNearbyWorldObject(100.0f, l_Searcher);

                if (l_Targets.empty())
                    return;

                JadeCore::RandomResizeList(l_Targets, 1);

                l_Caster->CastSpell(l_Targets.front(), eSpells::ArcLightning, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_curator_arc_lightning_trigger_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_curator_arc_lightning_trigger_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Arc Lightning - 227270
class spell_curator_arc_lightning : public SpellScriptLoader
{
    public:
        spell_curator_arc_lightning() : SpellScriptLoader("spell_curator_arc_lightning") { }

        class spell_curator_arc_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_curator_arc_lightning_SpellScript);

            void DealDamage()
            {
                if (!GetCaster())
                    return;

                if (Aura* l_Aura = GetCaster()->GetAura(GetSpellInfo()->Id))
                    SetHitDamage(GetHitDamage() * (l_Aura->GetStackAmount() + 1));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_curator_arc_lightning_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_curator_arc_lightning_SpellScript();
        }
};

/// 50
class eventobject_karazhan_library : public EventObjectScript
{
    public:
        eventobject_karazhan_library() : EventObjectScript("eventobject_karazhan_library") { }

        bool OnTrigger(Player* /*p_Player*/, EventObject* p_EventObject) override
        {
            if (InstanceScript* l_Instance = p_EventObject->GetInstanceScript())
            {
                if (l_Instance->GetBossState(eData::DataCurator) == DONE)
                    return true;
            }

            return false;
        }
};

/// Last Update 7.1.5 Build 23420
/// Volatile Energy - 114249
class npc_rtk_volatile_energy : public CreatureScript
{
    public:
        npc_rtk_volatile_energy() : CreatureScript("npc_rtk_volatile_energy") { }

        struct npc_rtk_volatile_energyAI : public LegionCombatAI
        {
            npc_rtk_volatile_energyAI(Creature* p_Creature) : LegionCombatAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            bool CanChase() override
            {
                return false;
            }

            bool CanAutoAttack() override
            {
                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_volatile_energyAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Curator Vision - 114462
class npc_curator_vision : public CreatureScript
{
    public:
        npc_curator_vision() : CreatureScript("npc_curator_vision") { }

        struct npc_curator_visionAI : public ScriptedAI
        {
            npc_curator_visionAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eTalks
            {
                ConvTalk
            };

            void DoAction(int32 const p_Action) override
            {
                AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                {
                });
                AddTimedDelayedOperation(13 * IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::ConvTalk);
                });
                AddTimedDelayedOperation(25 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(true);
                    Position const l_MovePos = { -11161.32f, -1902.34f, 165.77f };
                    me->GetMotionMaster()->MovePoint(0, l_MovePos);
                });
                AddTimedDelayedOperation(26500, [this]() -> void
                {
                    me->DespawnOrUnsummon();
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_curator_visionAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Curator Vision Medivh - 115491
class npc_curator_vision_medivh : public CreatureScript
{
    public:
        npc_curator_vision_medivh() : CreatureScript("npc_curator_vision_medivh") { }

        struct npc_curator_vision_medivhAI : public ScriptedAI
        {
            npc_curator_vision_medivhAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eTalks
            {
                ConvTalk1,
                ConvTalk2
            };

            void DoAction(int32 const p_Action) override
            {
                AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                {

                });
                AddTimedDelayedOperation(4 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(true);
                    Position const l_MovePos = { -11170.49f, -1909.03f, 165.77f };
                    me->GetMotionMaster()->MovePoint(0, l_MovePos);
                });
                AddTimedDelayedOperation(5500, [this]() -> void
                {
                    Creature* l_Curator = me->FindNearestCreature(eCreatures::CuratorVision, 20.0f);
                    if (!l_Curator)
                        return;

                    me->SetFacingToObject(l_Curator);
                });
                AddTimedDelayedOperation(7 * IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::ConvTalk1);
                });
                AddTimedDelayedOperation(19 * IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::ConvTalk2);
                });
                AddTimedDelayedOperation(23 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(true);
                    Position const l_MovePos = { -11186.65f, -1889.66f, 158.05f };
                    me->GetMotionMaster()->MovePoint(0, l_MovePos);
                });
                AddTimedDelayedOperation(26500, [this]() -> void
                {
                    me->DespawnOrUnsummon();
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_curator_vision_medivhAI(p_Creature);
        }
};


#ifndef __clang_analyzer__
void AddSC_boss_the_curator()
{
    /// Boss
    new boss_the_curator();

    /// Adds
    new npc_rtk_volatile_energy();

    /// Spell
    new spell_curator_arc_lightning_trigger();
    new spell_curator_arc_lightning();
    new spell_rtk_static_charge();
    new spell_rtk_power_discharge();
    new spell_rtk_summon_volatile_energy();

    /// EventObject
    new eventobject_karazhan_library();

    /// Event
    new npc_curator_vision();
    new npc_curator_vision_medivh();
}
#endif
