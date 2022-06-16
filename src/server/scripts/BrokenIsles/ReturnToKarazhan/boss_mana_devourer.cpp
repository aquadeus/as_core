////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

/// Mana Devourer - 114252
class boss_mana_devourer : public CreatureScript
{
    public:
        boss_mana_devourer() : CreatureScript("boss_mana_devourer") { }

        struct boss_mana_devourerAI : public BossAI
        {
            boss_mana_devourerAI(Creature* p_Creature) : BossAI(p_Creature, eData::ManaDevourer)
            {
                me->SetMaxPower(POWER_MANA, 100000);
            }

            enum eEvents
            {
                EventArcaneBomb         = 1,
                EventEnergyVoid         = 2,
                EventEnergyDischarge    = 3,
                EventCoalescePower      = 4
            };

            enum eSpells
            {
                ManaDevourerEnergize    = 227451,
                ArcaneBomb              = 227618,
                EnergyVoid              = 227523,
                EngulfingPower          = 228577,
                EnergyDischarge         = 227457,
                DecimatingEssence       = 227507,
                CoalescePower           = 227297,

                PlayerTeleport          = 231500, ///< 231499

                RideVehicleHardcoded    = 46598
            };

            enum eAchievements
            {
                TheRatPack = 11431
            };

            bool m_Achievement;

            void Reset() override
            {
                _Reset();
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                me->SetPower(POWER_MANA, 0);
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.60f;
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::ManaDevourer, EncounterState::FAIL);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                _EnterCombat();
                DoCast(me, eSpells::ManaDevourerEnergize, true);

                events.ScheduleEvent(eEvents::EventArcaneBomb, 8000);
                events.ScheduleEvent(eEvents::EventEnergyVoid, 15000);
                events.ScheduleEvent(eEvents::EventEnergyDischarge, 22000);
                events.ScheduleEvent(eEvents::EventCoalescePower, 30000);

                m_Achievement = true;
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (instance->instance)
                    {
                        for (Map::PlayerList::const_iterator itr = instance->instance->GetPlayers().begin(); itr != instance->instance->GetPlayers().end(); ++itr)
                        {
                            if (Player* player = itr->getSource())
                            {
                                if (!player->HasAura(eSpells::RideVehicleHardcoded))
                                    m_Achievement = false;
                            }
                        }
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                me->ClearLootContainers();

                DoCast(me, eSpells::PlayerTeleport, true);

                if (m_Achievement && me->GetInstanceScript() && IsMythic())
                    me->GetInstanceScript()->DoCompleteAchievement(eAchievements::TheRatPack);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::DecimatingEssence)
                    me->SetPower(POWER_MANA, 0);
            }

            void OnAddThreat(Unit* p_Attacker, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Attacker->GetEntry() == eCreatures::RodentOfUsualSize)
                    p_Threat = 0.0f;
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                m_Achievement = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArcaneBomb:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80, true))
                            me->CastSpell(static_cast<Position>(*l_Target), eSpells::ArcaneBomb, false);
                        events.ScheduleEvent(eEvents::EventArcaneBomb, 14000);
                        break;
                    }
                    case eEvents::EventEnergyVoid:
                    {
                        Position l_Pos;
                        me->GetNearPosition(l_Pos, 38.0f, frand(0.0f, 6.28f));
                        me->CastSpell(l_Pos, eSpells::EnergyVoid, false);
                        events.ScheduleEvent(eEvents::EventEnergyVoid, 22000);
                        break;
                    }
                    case eEvents::EventEnergyDischarge:
                    {
                        for (uint8 l_I = 0; l_I < 30; l_I++)
                            DoCast(me, eSpells::EngulfingPower, true);
                        DoCast(me, eSpells::EnergyDischarge, true);
                        events.ScheduleEvent(eEvents::EventEnergyDischarge, 27000);
                        break;
                    }
                    case eEvents::EventCoalescePower:
                    {
                        DoCast(me, eSpells::CoalescePower, true);
                        events.ScheduleEvent(eEvents::EventCoalescePower, 30000);
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
            return new boss_mana_devourerAI(p_Creature);
        }
};

/// Mana Devourer Energize - 227451
class spell_mana_devourer_energize : public SpellScriptLoader
{
    public:
        spell_mana_devourer_energize() : SpellScriptLoader("spell_mana_devourer_energize") { }

        class spell_mana_devourer_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mana_devourer_energize_AuraScript);

            enum eSpells
            {
                CoalescePower       = 227297,
                Energize            = 232066,
                DecimatingEssence   = 227507
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                if (l_Caster->GetPowerPct(POWER_MANA) < 100.0f)
                {
                    if (!l_Caster->HasAura(eSpells::CoalescePower))
                        l_Caster->CastSpell(l_Caster, eSpells::Energize, true);
                }
                else
                {
                    if (!l_Caster->HasUnitState(UNIT_STATE_CASTING))
                        l_Caster->CastSpell(l_Caster, eSpells::DecimatingEssence, false);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mana_devourer_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mana_devourer_energize_AuraScript();
        }
};

/// Energy Void - 227524
class spell_mana_energy_void : public SpellScriptLoader
{
    public:
        spell_mana_energy_void() : SpellScriptLoader("spell_mana_energy_void") { }

        class spell_mana_energy_void_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mana_energy_void_AuraScript);

            enum eSpells
            {
                EneryVoidManaDrain  = 227528,
                UnstableMana        = 227502
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetTarget())
                {
                    if (!GetTarget()->HasAura(eSpells::UnstableMana))
                        GetTarget()->CastSpell(GetTarget(), eSpells::EneryVoidManaDrain, true);
                    else
                    {
                        if (Aura* l_Aura = GetTarget()->GetAura(eSpells::UnstableMana))
                            l_Aura->ModStackAmount(-1);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mana_energy_void_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mana_energy_void_AuraScript();
        }
};

/// Coalesce Power - 227297
class spell_mana_coalesce_power : public SpellScriptLoader
{
    public:
        spell_mana_coalesce_power() : SpellScriptLoader("spell_mana_coalesce_power") { }

        class spell_mana_coalesce_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mana_coalesce_power_AuraScript);

            enum eSpells
            {
                LooseMana = 228066
            };

            uint8 m_AreatriggerToSpawn = 0;
            uint8 m_Tick = 0;

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_AreatriggerToSpawn = l_Caster->GetPowerPct(POWER_MANA) / 5.0f;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (m_Tick >= 20 - m_AreatriggerToSpawn)
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::LooseMana, true);
                        l_Caster->ModifyPower(Powers::POWER_MANA, -static_cast<int32>(l_Caster->CountPctFromMaxMana(5)));
                    }

                    m_Tick++;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mana_coalesce_power_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mana_coalesce_power_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mana_coalesce_power_AuraScript();
        }
};

/// Teleport - 231499
class spell_rtk_mana_teleport : public SpellScriptLoader
{
    public:
        spell_rtk_mana_teleport() : SpellScriptLoader("spell_rtk_mana_teleport") {}

        class spell_rtk_mana_teleport_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rtk_mana_teleport_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                    l_Player->TeleportTo(1651, 4147.74f, -2031.47f, 730.55f, l_Player->GetOrientation());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rtk_mana_teleport_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rtk_mana_teleport_AuraScript();
        }
};

/// Rodent of Usual Size- 232648
class spell_rtk_rodent_of_usual_size : public SpellScriptLoader
{
    public:
        spell_rtk_rodent_of_usual_size() : SpellScriptLoader("spell_rtk_rodent_of_usual_size") {}

        class spell_rtk_rodent_of_usual_size_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_rodent_of_usual_size_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ClearUnitState(UNIT_STATE_ONVEHICLE);
                }, 1);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_rtk_rodent_of_usual_size_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_rodent_of_usual_size_SpellScript();
        }
};

/// Mana Devourer - 114252
class npc_rtk_rodent_of_usual_size : public CreatureScript
{
    public:
        npc_rtk_rodent_of_usual_size() : CreatureScript("npc_rtk_rodent_of_usual_size") { }

        struct npc_rtk_rodent_of_usual_sizeAI : public ScriptedAI
        {
            npc_rtk_rodent_of_usual_sizeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
            }

            void CancelAchievement()
            {
                Creature* l_ManaDevourer = me->FindNearestCreature(eCreatures::CreatureManaDevourer, 100.0f);
                if (!l_ManaDevourer || !l_ManaDevourer->IsAIEnabled)
                    return;

                l_ManaDevourer->GetAI()->DoAction(ACTION_1);
            }

            void JustDied(Unit* p_Killer) override
            {
                CancelAchievement();
            }

            void JustDespawned() override
            {
                CancelAchievement();
            }

            void PassengerBoarded(Unit* p_Passenger, int8 p_SeatID, bool p_Apply) override
            {
                if (p_Apply)
                    return;

                CancelAchievement();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_rodent_of_usual_sizeAI(p_Creature);
        }
};


#ifndef __clang_analyzer__
void AddSC_boss_mana_devourer()
{
    /// Boss
    new boss_mana_devourer();

    /// Spells
    new spell_mana_devourer_energize();
    new spell_mana_energy_void();
    new spell_mana_coalesce_power();
    new spell_rtk_mana_teleport();

    /// Achievements
    new npc_rtk_rodent_of_usual_size();
    new spell_rtk_rodent_of_usual_size();
}
#endif
