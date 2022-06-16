////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_WARRIOR and SPELLFAMILY_GENERIC spells used by warrior players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warr_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "HelperDefines.h"
#include "CellImpl.h"

/// Last Update 7.1.5 Build 23420
/// Called by Ravager - 152277 and Ravager - 228920
class spell_warr_ravager : public SpellScriptLoader
{
    public:
        spell_warr_ravager() : SpellScriptLoader("spell_warr_ravager") { }

        class spell_warr_ravager_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_ravager_AuraScript);

            enum eDatas
            {
                RavagerBuff   = 227744,
                RavagerNPC    = 76168,
                RavagerDamage = 156287,
                T20Arms4PBonus = 242297,
                MortalStrike = 246820
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() == SpecIndex::SPEC_WARRIOR_PROTECTION)
                    l_Player->CastSpell(l_Player, eDatas::RavagerBuff, true);
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eDatas::RavagerDamage);
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Creature* l_Ravager = nullptr;
                for (auto l_Iter : l_Caster->m_Controlled)
                {
                    if (l_Iter->GetEntry() == eDatas::RavagerNPC)
                        l_Ravager = l_Iter->ToCreature();
                }

                if (l_Ravager)
                {
                    l_Caster->CastSpell(l_Ravager, eDatas::RavagerDamage, true);

                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eDatas::T20Arms4PBonus, EFFECT_1))
                    {
                        if (m_Count >= l_AuraEffect->GetAmount())
                            return;

                        float l_Radius = l_SpellInfo->Effects[EFFECT_0].CalcRadius();

                        std::list<Unit*> l_Targets;
                        l_Caster->GetAttackableUnitListInRange(l_Targets, l_Radius);
                        l_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                        {
                            if (!p_Target || !p_Target->ToUnit())
                                return true;

                            if (!l_Caster->IsValidAttackTarget(p_Target->ToUnit()) || l_Caster->IsFriendlyTo(p_Target->ToUnit()))
                                return true;

                            return false;
                        });

                        if (l_Targets.empty())
                            return;

                        JadeCore::RandomResizeList(l_Targets, 1);

                        l_Ravager->CastSpell(l_Targets.front(), eDatas::MortalStrike, true, 0, nullptr, l_Caster->GetGUID());

                        m_Count++;
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_ravager_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_ravager_AuraScript::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }

        private:
            int32 m_Count = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_ravager_AuraScript();
        }

        class spell_warr_ravager_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_ravager_SpellScript);

            enum eSpells
            {
                RavagerSummon = 227876,
            };

            void HandleSummon(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::RavagerSummon, true);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_warr_ravager_SpellScript::HandleSummon, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_ravager_SpellScript();
        }
};

/// Last Update 6.2.3
/// Shield Block - 2565
class spell_warr_shield_block: public SpellScriptLoader
{
    public:
        spell_warr_shield_block() : SpellScriptLoader("spell_warr_shield_block") { }

        class spell_warr_shield_block_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_shield_block_SpellScript);

            enum eSpells
            {
                ShieldBlockTriggered = 132404
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                int32 l_PreviousDuration = 0;

                if (Aura* l_Previous = l_Caster->GetAura(eSpells::ShieldBlockTriggered))
                    l_PreviousDuration = l_Previous->GetDuration();

                l_Caster->CastSpell(l_Caster, eSpells::ShieldBlockTriggered, true);

                if (l_PreviousDuration)
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::ShieldBlockTriggered))
                        l_Aura->SetDuration(l_Aura->GetDuration() + l_PreviousDuration);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_shield_block_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_shield_block_SpellScript();
        }
};

/// Shield Block (aura) - 132404
class spell_warr_shield_block_aura : public SpellScriptLoader
{
    public:
        spell_warr_shield_block_aura() : SpellScriptLoader("spell_warr_shield_block_aura") { }

        class spell_warr_shield_block_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_shield_block_AuraScript);

            enum eSpells
            {
                T17Protection4P = 165351,
                ShieldMastery   = 169688,
                T19Protection2P = 212237,
                ShieldBlock     = 212236
            };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    /// While Shield Block is active, your shield block value is increased by 5%.
                    if (l_Target->HasAura(eSpells::T17Protection4P))
                        l_Target->CastSpell(l_Target, eSpells::ShieldMastery, true);
                    /// Item - Warrior T19 Protection 2P Bonus
                    if (l_Target->HasAura(eSpells::T19Protection2P))
                        l_Target->CastSpell(l_Target, eSpells::ShieldBlock, true);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->RemoveAura(eSpells::ShieldMastery);
                    l_Target->RemoveAura(eSpells::ShieldBlock);
                }
            }

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->CanApplyPvPSpellModifiers())
                    return;

                if (l_AuraEffect->GetEffIndex() == EFFECT_0)
                    p_Amount = l_AuraEffect->GetBaseAmount() * l_Aura->GetStackAmount() * 0.40f;   ///< Shield Block bonus block is reduced by 60% in PvP
                else
                    p_Amount *= 0.50f;   ///< Shield Slam (23922) damage amp is reduced by 50% in PvP (including with Honor Talent 'Sword and Board' (199127)

            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_shield_block_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_BLOCK_PERCENT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_shield_block_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_BLOCK_PERCENT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_shield_block_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_BLOCK_PERCENT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_shield_block_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_shield_block_AuraScript();
        }
};

/// Storm Bolt - 107570
class spell_warr_storm_bolt: public SpellScriptLoader
{
    public:
        spell_warr_storm_bolt() : SpellScriptLoader("spell_warr_storm_bolt") { }

        class spell_warr_storm_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_storm_bolt_SpellScript);

            enum eSpells
            {
                StormBoltOffHand = 107570,
                StormBoltStun = 132169
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return;

                if (GetSpellInfo()->Id == eSpells::StormBoltOffHand)
                    l_Caster->CastSpell(l_Target, eSpells::StormBoltStun, true);
            }

            void HandleOnHit()
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    if (l_Target->IsImmunedToSpellEffect(sSpellMgr->GetSpellInfo(eSpells::StormBoltStun), EFFECT_0))
                        SetHitDamage(GetHitDamage() * 4); ///< Deals quadruple damage to targets permanently immune to stuns
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_warr_storm_bolt_SpellScript::HandleOnCast);
                OnHit += SpellHitFn(spell_warr_storm_bolt_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_storm_bolt_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Colossus Smash - 167105
class spell_warr_colossus_smash: public SpellScriptLoader
{
    public:
        spell_warr_colossus_smash() : SpellScriptLoader("spell_warr_colossus_smash") { }

        class spell_warr_colossus_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_colossus_smash_SpellScript);

            enum eSpells
            {
                DamageIncreasedDebuff = 208086
            };

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        l_Caster->CastSpell(l_Target, eSpells::DamageIncreasedDebuff, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_colossus_smash_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_colossus_smash_SpellScript();
        }
};

/// Last Update 6.2.3
/// Dragon Roar - 118000
class spell_warr_dragon_roar: public SpellScriptLoader
{
    public:
        spell_warr_dragon_roar() : SpellScriptLoader("spell_warr_dragon_roar") { }

        class spell_warr_dragon_roar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_dragon_roar_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_ARMS)
                    SetHitDamage(GetHitDamage() * 1.25f);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_dragon_roar_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_dragon_roar_SpellScript();
        }
};

/// Staggering Shout - 107566
class spell_warr_staggering_shout: public SpellScriptLoader
{
    public:
        spell_warr_staggering_shout() : SpellScriptLoader("spell_warr_staggering_shout") { }

        class spell_warr_staggering_shout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_staggering_shout_SpellScript);

            void RemoveInvalidTargets(std::list<WorldObject*>& l_Targets)
            {
                l_Targets.remove_if(JadeCore::UnitAuraTypeCheck(false, SPELL_AURA_MOD_DECREASE_SPEED));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_staggering_shout_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_staggering_shout_SpellScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Berzerker Rage - 18499
class spell_warr_berzerker_rage: public SpellScriptLoader
{
    public:
        spell_warr_berzerker_rage() : SpellScriptLoader("spell_warr_berzerker_rage") { }

        class spell_warr_berzerker_rage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_berzerker_rage_SpellScript);

            enum eSpells
            {
                Outburst    = 206320,
                Enrage      = 184362
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::Outburst)) ///< Berserker Rage now causes Enrage.
                    l_Caster->CastSpell(l_Caster, eSpells::Enrage, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_berzerker_rage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_berzerker_rage_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Enrage - 184361
class spell_warr_enrage: public SpellScriptLoader
{
    public:
        spell_warr_enrage() : SpellScriptLoader("spell_warr_enrage") { }

        class spell_warr_enrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_enrage_AuraScript);

            enum eSpells
            {
                Bloodthirst     = 23881,
                TasteForBlood   = 206333,
                Outburst        = 206320,
                BerserkerRage   = 18499,
                RampageFirst    = 218617
            };

            Guid128 m_CastId;

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& p_EventInfo)
            {
                bool l_PreventProc = true;

                if (DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo())
                {
                    if (Spell const* l_Spell = l_DamageInfo->GetSpell())
                    {
                        if (Unit* l_Caster = l_DamageInfo->GetActor())
                        {
                            if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                            {
                                if (l_SpellInfo->Id == eSpells::Bloodthirst && (p_EventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                                {
                                    l_Caster->RemoveAurasDueToSpell(eSpells::TasteForBlood);
                                    l_PreventProc = false;
                                }
                                else if ((l_SpellInfo->Id == eSpells::BerserkerRage && l_Caster->HasAura(eSpells::Outburst)) ||
                                    (l_SpellInfo->Id == eSpells::RampageFirst && l_Spell->GetCastGuid() != m_CastId))               ///< Prevent Whirlwind - Rampage to generate several time Rage
                                {
                                    l_PreventProc = false;
                                    m_CastId = l_Spell->GetCastGuid();
                                }
                            }
                        }
                    }
                }

                if (l_PreventProc)
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_enrage_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_enrage_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Devastate - 20243
class spell_warr_devaste: public SpellScriptLoader
{
    public:
        spell_warr_devaste() : SpellScriptLoader("spell_warr_devaste") { }

        class spell_warr_devaste_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_devaste_SpellScript);

            enum eSpells
            {
                UnyieldingStrikesAura   = 169685,
                UnyieldingStrikesProc   = 169686,
                SwordandBoard           = 46953,
                ShieldSlam              = 23922
            };

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SwordandBoard);

                if (l_Player->HasAura(eSpells::UnyieldingStrikesAura))
                {
                    /// If we already have 5 charges, we don't need to update an aura
                    if (Aura* l_UnyieldingStrikes = l_Player->GetAura(eSpells::UnyieldingStrikesProc))
                    {
                        if (l_UnyieldingStrikes->GetStackAmount() < 6)
                        {
                            l_UnyieldingStrikes->SetStackAmount(l_UnyieldingStrikes->GetStackAmount() + 1);
                            l_UnyieldingStrikes->SetDuration(l_UnyieldingStrikes->GetMaxDuration());
                        }
                    }
                    else
                        l_Player->CastSpell(l_Player, eSpells::UnyieldingStrikesProc, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_devaste_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_devaste_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Mortal strike - 12294, 246820
class spell_warr_mortal_strike: public SpellScriptLoader
{
    public:
        spell_warr_mortal_strike() : SpellScriptLoader("spell_warr_mortal_strike") { }

        class spell_warr_mortal_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_mortal_strike_SpellScript);

            enum eSpells
            {
                MortalStrikeAura          = 12294,
                AllowOverpower            = 60503,
                FocusedRage               = 207982,
                MortalWounds              = 115804,
                SharpenBlade              = 198817,
                SharpenBladeHealEffect    = 198819,
                ShatteredDefenses         = 248625,
                ExecutionersPrecision     = 242188,
                PreciseStrikeBuff         = 248195,
                ColossusSmashDebuff       = 208086
            };

            bool m_HasPvPAura;

            bool Load() override
            {
                m_HasPvPAura = false;
                return true;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::MortalStrikeAura))
                    l_Caster->RemoveAura(eSpells::MortalStrikeAura);

                /// Shattered Defenses
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::ShatteredDefenses))
                    l_Aura->Remove();

                if (m_HasPvPAura && l_Caster != l_Target)
                {
                    l_Target->RemoveAurasDueToSpell(eSpells::MortalWounds); ///< Update 7.1.5 build 23420 to prevent stacking of both buffs (they dont stack on retail)
                    l_Caster->CastSpell(l_Target, eSpells::SharpenBladeHealEffect, true);
                }
                else if (!l_Target->HasAura(eSpells::SharpenBladeHealEffect) && l_Caster != l_Target) ///< Update 7.1.5 build 23420 to prevent mortal wounds from erasing sharpen blade's stronger debuff
                    l_Caster->CastSpell(l_Target, eSpells::MortalWounds, true);
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Unit* l_Owner = l_Caster->GetOwner())
                {
                    uint32 l_Damage = GetHitDamage();
                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ShatteredDefenses, EFFECT_0))
                        AddPct(l_Damage, l_AuraEffect->GetAmount());

                    if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::ColossusSmashDebuff, EFFECT_2))
                        AddPct(l_Damage, l_AuraEffect->GetAmount());

                    if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::ExecutionersPrecision, EFFECT_0))
                        AddPct(l_Damage, l_AuraEffect->GetAmount());

                    SetHitDamage(l_Damage);
                    l_Owner->RemoveAura(eSpells::ShatteredDefenses);
                    if (Aura* l_Aura = l_Target->GetAura(eSpells::ExecutionersPrecision, l_Owner->GetGUID()))
                    {
                        if (l_Aura->GetStackAmount() > 1)
                            l_Aura->DropStack();
                        else
                            l_Aura->Remove();
                    }
                }

                l_Target->RemoveAurasDueToSpell(eSpells::ExecutionersPrecision, l_Caster->GetGUID());
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_HasPvPAura = l_Caster->HasAura(eSpells::SharpenBlade);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::FocusedRage);
            }

            void HandleAfterHit()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->RemoveAura(eSpells::PreciseStrikeBuff);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_mortal_strike_SpellScript::HandleOnHit);
                AfterHit += SpellHitFn(spell_warr_mortal_strike_SpellScript::HandleAfterHit);
                BeforeCast += SpellCastFn(spell_warr_mortal_strike_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_warr_mortal_strike_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_warr_mortal_strike_SpellScript::HandleDamage, EFFECT_2, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_mortal_strike_SpellScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Commanding Shout - 97462
class spell_warr_commanding_shout : public SpellScriptLoader
{
    public:
        spell_warr_commanding_shout() : SpellScriptLoader("spell_warr_commanding_shout") { }

        class spell_warr_commanding_shout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_commanding_shout_SpellScript);

            enum eSpells
            {
                CommandingShoutAura = 97463
            };

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    int32 l_Bp0 = 0;
                    std::list<Unit*> l_MemberList;

                    l_Player->GetRaidMembers(l_MemberList);

                    for (auto l_Itr : l_MemberList)
                    {
                        if (l_Itr->IsWithinDistInMap(l_Player, GetSpellInfo()->Effects[EFFECT_0].CalcRadius()))
                        {
                            l_Bp0 = CalculatePct(l_Itr->GetMaxHealth(), GetSpellInfo()->Effects[EFFECT_0].BasePoints);
                            l_Player->CastCustomSpell(l_Itr, eSpells::CommandingShoutAura, &l_Bp0, NULL, NULL, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_warr_commanding_shout_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_commanding_shout_SpellScript();
        }
};

/// Heroic leap - 6544
class spell_warr_heroic_leap: public SpellScriptLoader
{
    public:
        spell_warr_heroic_leap() : SpellScriptLoader("spell_warr_heroic_leap") { }

        class spell_warr_heroic_leap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_heroic_leap_SpellScript);

            enum eSpells
            {
                HeroicLeapJump         = 94954
            };

            enum eAreaID
            {
                GurubashiRingBattle = 2177
            };

            SpellCastResult CheckElevation()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                WorldLocation* l_SpellDest = const_cast<WorldLocation*>(GetExplTargetDest());

                if (!l_Player || !l_SpellDest)
                    return SPELL_FAILED_DONT_REPORT;

                if (!l_Player->IsWithinLOS(l_SpellDest->m_positionX, l_SpellDest->m_positionY, l_SpellDest->m_positionZ))
                    return SPELL_FAILED_NOPATH;

                else if (l_Player->HasAuraType(SPELL_AURA_MOD_ROOT) || l_Player->HasAuraType(SPELL_AURA_MOD_ROOT_2))
                    return SPELL_FAILED_ROOTED;
                else if (l_Player->GetMap()->IsBattlegroundOrArena())
                {
                    if (Battleground* l_Bg = l_Player->GetBattleground())
                    {
                        if (l_Bg->GetStatus() != STATUS_IN_PROGRESS)
                            return SPELL_FAILED_NOT_READY;
                    }
                }

                Map* l_Map = l_Player->GetMap();
                if (l_Map == nullptr)
                    return SPELL_FAILED_SUCCESS;

                PathGenerator l_Path(l_Player);
                l_Path.SetPathLengthLimit(80.0f);
                bool l_Result = l_Path.CalculatePath(l_SpellDest->GetPositionX(), l_SpellDest->GetPositionY(), l_SpellDest->GetPositionZ());

                if (!l_Result || l_Path.GetPathType() == PATHFIND_SHORTCUT || l_Path.GetPathType() == PATHFIND_NOPATH)
                    return SPELL_FAILED_NOPATH;

                if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) && l_Player->GetAreaId() == eAreaID::GurubashiRingBattle)
                {
                    WorldLocation const* l_Dest = GetExplTargetDest();
                    if (!l_Dest)
                        return SPELL_FAILED_SUCCESS;

                    uint32 l_DestAreaID = l_Map->GetAreaId(l_Dest->m_positionX, l_Dest->m_positionY, l_Dest->m_positionZ);
                    if (l_DestAreaID != eAreaID::GurubashiRingBattle)
                        return SPELL_FAILED_LINE_OF_SIGHT;
                }

                return SPELL_CAST_OK;
            }

            void HandleOnCast()
            {
                WorldLocation* l_SpellDest = const_cast<WorldLocation*>(GetExplTargetDest());
                if (l_SpellDest)
                    GetCaster()->CastSpell(l_SpellDest->GetPositionX(), l_SpellDest->GetPositionY(), l_SpellDest->GetPositionZ(), eSpells::HeroicLeapJump, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warr_heroic_leap_SpellScript::CheckElevation);
                OnCast += SpellCastFn(spell_warr_heroic_leap_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_heroic_leap_SpellScript();
        }
};

/// Heroic Leap (damage) - 52174
class spell_warr_heroic_leap_damage: public SpellScriptLoader
{
    public:
        spell_warr_heroic_leap_damage() : SpellScriptLoader("spell_warr_heroic_leap_damage") { }

        class spell_warr_heroic_leap_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_heroic_leap_damage_SpellScript);

            enum eSpells
            {
                Taunt                   = 355,
                HeroicLeapJump          = 94954,
                SpellPvp4PBonus         = 133277,
                BoundingStride          = 202163,
                BoundingStrideAura      = 202164,
                WeightOfTheEarth        = 208177,
                ColossusSmashDebuff     = 208086,
                TacticalAdvance         = 209483,
                TacticalAdvanceBuff     = 209484,
                WarpathStun             = 199085,
                WarpathTalent           = 199086
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::BoundingStride))
                    l_Caster->CastSpell(l_Caster, eSpells::BoundingStrideAura, true);

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::TacticalAdvance, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                int32 l_Amount = l_AuraEffect->GetAmount();
                l_Caster->CastCustomSpell(l_Caster, eSpells::TacticalAdvanceBuff, &l_Amount, &l_Amount, nullptr, true);

            }

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();
                if (!l_Player || !l_Target)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_FURY)
                    SetHitDamage(int32(GetHitDamage() * 1.2f));

                if (l_Player->HasAura(eSpells:: WeightOfTheEarth) && l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_ARMS)
                    l_Player->CastSpell(l_Target, eSpells::ColossusSmashDebuff, true);

                if (l_Player->HasAura(eSpells::WarpathTalent))
                    l_Player->CastSpell(l_Target, eSpells::WarpathStun, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_heroic_leap_damage_SpellScript::HandleAfterCast);
                OnHit += SpellHitFn(spell_warr_heroic_leap_damage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_heroic_leap_damage_SpellScript();
        }
};

/// Update to Legion 7.3.2 build 25549
/// Called by Shockwave - 46968
class spell_warr_shockwave: public SpellScriptLoader
{
    public:
        spell_warr_shockwave() : SpellScriptLoader("spell_warr_shockwave") { }

        class spell_warr_shockwave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_shockwave_SpellScript);

            enum eSpells
            {
                ShockwaveStun   = 132168,
                Shockwave       = 46968
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                    if (Unit* l_Target = GetHitUnit())
                        l_Caster->CastSpell(l_Target, eSpells::ShockwaveStun, true);
            }

            /// Cooldown reduced by 20 sec if it strikes at least 3 targets.
            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                Spell* l_Spell = GetSpell();
                if (!l_Player || !l_Spell)
                    return;

                /// Caster is in GetUnitTargetCount()
                if (int32(l_Spell->GetUnitTargetCount() - 1) >= GetSpellInfo()->Effects[EFFECT_0].BasePoints)
                    l_Player->ReduceSpellCooldown(eSpells::Shockwave, GetSpellInfo()->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_shockwave_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_warr_shockwave_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_shockwave_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Bloodthirst - 23881
class spell_warr_bloodthirst: public SpellScriptLoader
{
    public:
        spell_warr_bloodthirst() : SpellScriptLoader("spell_warr_bloodthirst") { }

        class spell_warr_bloodthirst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_bloodthirst_SpellScript);

            enum eSpells
            {
                BloodthirstHeal     = 117313,
                MeatCleaver         = 85739,
                Bloodcraze          = 200859,
                BloodyRage50        = 242953,
                BloodyRage20        = 242952
            };

            uint8 m_HealPct = 0;

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !GetHitDamage())
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Bloodcraze, EFFECT_0);

                if (l_Caster->HealthBelowPct(20) && l_AuraEffect)
                    m_HealPct += l_AuraEffect->GetAmount();

                Unit* l_Target = GetHitUnit();
                Unit* l_ExplTarget = GetExplTargetUnit();

                if (!l_Target || !l_ExplTarget || !GetHitDamage())
                    return;

                Aura* l_Aura = l_Caster->GetAura(MeatCleaver);

                if (l_Aura && l_Target->GetGUID() != l_ExplTarget->GetGUID())
                    SetHitDamage(CalculatePct(GetHitDamage(), 50));
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_HealPct += sSpellMgr->GetSpellInfo(eSpells::BloodthirstHeal)->Effects[EFFECT_0].BasePoints * (l_Caster->CanApplyPvPSpellModifiers() ? 0.75f: 1.0f); ///< Bloodthirst is reduced by 25% in PvP

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::BloodyRage50, EFFECT_0))
                    AddPct(m_HealPct, l_AuraEffect->GetAmount());

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::BloodyRage20, EFFECT_0))
                    AddPct(m_HealPct, l_AuraEffect->GetAmount());

                l_Caster->CastCustomSpell(eSpells::BloodthirstHeal, SpellValueMod::SPELLVALUE_BASE_POINT0, m_HealPct, l_Caster, true);

                l_Caster->RemoveAura(eSpells::BloodyRage50);
                l_Caster->RemoveAura(eSpells::BloodyRage20);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_bloodthirst_SpellScript::HandleOnHit);
                AfterCast += SpellCastFn(spell_warr_bloodthirst_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_bloodthirst_SpellScript();
        }
};

/// Victory Rush - 34428
class spell_warr_victory_rush: public SpellScriptLoader
{
    public:
        spell_warr_victory_rush() : SpellScriptLoader("spell_warr_victory_rush") { }

        class spell_warr_victory_rush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_victory_rush_SpellScript);

            enum eSpells
            {
                VictoriousState   = 32216,
                VictoryRushDamage = 34428,
                VictoryRushHeal   = 118779
            };

            bool Validate(SpellInfo const* /*SpellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::VictoryRushDamage))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
                if (!GetHitUnit())
                    return;

                if (Unit* caster = GetCaster())
                {
                    caster->CastSpell(caster, eSpells::VictoryRushHeal, true);

                    if (caster->HasAura(eSpells::VictoriousState))
                        caster->RemoveAura(eSpells::VictoriousState);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_victory_rush_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_victory_rush_SpellScript();
        }
};

/// Last Update 7.1.5
/// Charge - 100
class spell_warr_charge: public SpellScriptLoader
{
    public:
        spell_warr_charge() : SpellScriptLoader("spell_warr_charge") { }

        enum eSpells
        {
            Charge                  = 40602,
            ChargeDamage            = 126664,
            FuriousCharge           = 202224,
            FuriousChargeAura       = 202225,
            WarbringerDamage        = 7922,
            WarbringerStun          = 237744,
            Warbringer              = 103828,
            ChargeRoot              = 105771,
            FireVisual              = 96840,
            GlyphOfTheBlazingTrail  = 123779,
            SpellWodPvPFury2p       = 165639,
            SpellWodPvPFury2pEffect = 165640,
            SpellWodPvPArms2p       = 165636,
            SpellWodPvPArms2pEffect = 165638,
            SpellWodPvPProt2p       = 165641,
            SpellWodPvPProt2pEffect = 165642,
            ChargeSlow              = 236027
        };

        class spell_warr_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_charge_SpellScript);

            void HandleOnCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// Warrior WoD PvP 2P bonuses
                    if (l_Caster->getLevel() == 100)
                    {
                        switch (l_Caster->ToPlayer()->GetActiveSpecializationID())
                        {
                        case SPEC_WARRIOR_FURY:
                        {
                            if (l_Caster->HasAura(eSpells::SpellWodPvPFury2p))
                                l_Caster->CastSpell(l_Caster, eSpells::SpellWodPvPFury2pEffect, true);
                            break;
                        }
                        case SPEC_WARRIOR_ARMS:
                        {
                            if (l_Caster->HasAura(eSpells::SpellWodPvPArms2p))
                                l_Caster->CastSpell(l_Caster, eSpells::SpellWodPvPArms2pEffect, true);
                            break;
                        }
                        case SPEC_WARRIOR_PROTECTION:
                        {
                            if (l_Caster->HasAura(eSpells::SpellWodPvPProt2p))
                                l_Caster->CastSpell(l_Caster, eSpells::SpellWodPvPProt2pEffect, true);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
            }

            void HandleCharge(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Charge, true);
                l_Caster->CastSpell(l_Target, eSpells::ChargeDamage, true);
                l_Caster->CastSpell(l_Target, l_Caster->HasAura(eSpells::Warbringer) ? eSpells::WarbringerDamage : eSpells::ChargeRoot, true);
                if (l_Caster->HasAura(eSpells::Warbringer))
                    l_Caster->CastSpell(l_Target, eSpells::WarbringerStun, true);
                l_Caster->CastSpell(l_Target, eSpells::ChargeSlow, true);

                /// Furious Charge
                if (l_Caster->HasAura(eSpells::FuriousCharge))
                    l_Caster->CastSpell(l_Caster, eSpells::FuriousChargeAura, true);

                /// Glyph of Blazing Trail
                if (l_Caster->HasAura(eSpells::GlyphOfTheBlazingTrail))
                    l_Caster->CastSpell(l_Caster, eSpells::FireVisual, true);
            }

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                if (l_Caster->isInRoots())
                    return SpellCastResult::SPELL_FAILED_ROOTED;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warr_charge_SpellScript::HandleCheckCast);
                OnCast += SpellCastFn(spell_warr_charge_SpellScript::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_warr_charge_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_charge_SpellScript;
        }
};

/// Last Update 7.3.5 Build 26365
/// Shield Wall - 871, Defensive Stance - 197690
class spell_warr_shield_wall: public SpellScriptLoader
{
    public:
        spell_warr_shield_wall() : SpellScriptLoader("spell_warr_shield_wall") { }

        class spell_warr_shield_wall_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_shield_wall_AuraScript);

            enum eSpells
            {
                ShieldOfWallNoShield    = 146128,
                ShieldOfWallHorde       = 146127,
                ShieldOfWallAlliance    = 147925
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->GetShield())
                        l_Player->AddAura(eSpells::ShieldOfWallNoShield, l_Player);
                    else
                    {
                        if (l_Player->GetTeam() == HORDE)
                            l_Player->AddAura(eSpells::ShieldOfWallHorde, l_Player);
                        else
                            l_Player->AddAura(eSpells::ShieldOfWallAlliance, l_Player);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::ShieldOfWallNoShield))
                        l_Player->RemoveAura(eSpells::ShieldOfWallNoShield);
                    else if (l_Player->HasAura(eSpells::ShieldOfWallHorde))
                        l_Player->RemoveAura(eSpells::ShieldOfWallHorde);
                    else
                        l_Player->RemoveAura(eSpells::ShieldOfWallAlliance);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_shield_wall_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_shield_wall_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_shield_wall_AuraScript();
        }
};

/// Spell Reflection - 23920
class spell_warr_spell_reflection: public SpellScriptLoader
{
    public:
        spell_warr_spell_reflection() : SpellScriptLoader("spell_warr_spell_reflection") { }

        class spell_warr_spell_reflection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_spell_reflection_AuraScript);

            enum eSpells
            {
                SpellReflectionNoshield = 146120,
                SpellReflectionHorde    = 146122,
                SpellReflectionAlliance = 147923,
                SpellReflectivePlating  = 188672,
            };

            void HandlePrepareProc(ProcEventInfo & p_Proc)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (l_Owner->HasAura(eSpells::SpellReflectivePlating))
                    PreventDefaultAction();
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->GetShield())
                        l_Player->AddAura(eSpells::SpellReflectionNoshield, l_Player);
                    else
                    {
                        if (l_Player->GetTeam() == HORDE)
                            l_Player->AddAura(eSpells::SpellReflectionHorde, l_Player);
                        else
                            l_Player->AddAura(eSpells::SpellReflectionAlliance, l_Player);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::SpellReflectionNoshield))
                        l_Player->RemoveAura(eSpells::SpellReflectionNoshield);
                    else if (l_Player->HasAura(eSpells::SpellReflectionHorde))
                        l_Player->RemoveAura(eSpells::SpellReflectionHorde);
                    else
                        l_Player->RemoveAura(eSpells::SpellReflectionAlliance);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_spell_reflection_AuraScript::OnApply, EFFECT_0, SPELL_AURA_REFLECT_SPELLS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_spell_reflection_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_REFLECT_SPELLS, AURA_EFFECT_HANDLE_REAL);
                DoPrepareProc += AuraProcFn(spell_warr_spell_reflection_AuraScript::HandlePrepareProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_spell_reflection_AuraScript();
        }
};

/// last update : 6.2.3
/// Intervene - 3411
class spell_warr_intervene: public SpellScriptLoader
{
    public:
        spell_warr_intervene() : SpellScriptLoader("spell_warr_intervene") { }

        class spell_warr_intervene_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_intervene_SpellScript);

            enum eSpells
            {
                InterveneAura           = 34784,
                InterveneCharge         = 147833,
            };

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Player->HasAuraType(SPELL_AURA_MOD_ROOT) || l_Player->HasAuraType(SPELL_AURA_MOD_ROOT_2))
                    return SPELL_FAILED_ROOTED;

                if (!l_Player || !l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Player->GetGUID() == l_Target->GetGUID())
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Player->GetDistance(l_Target) >= 25.0f)
                    return SPELL_FAILED_OUT_OF_RANGE;

                return SPELL_CAST_OK;
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::InterveneAura, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warr_intervene_SpellScript::CheckCast);
                OnCast += SpellCastFn(spell_warr_intervene_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_intervene_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Anger Management - 152278
class spell_warr_anger_management: public PlayerScript
{
    public:
        spell_warr_anger_management() : PlayerScript("spell_warr_anger_management") {}

        enum eSpells
        {
            AngerManagement     = 152278,
            Bloodbath           = 12292,
            Bladestorm          = 46924,
            BladestormArm       = 227847,
            DragonRoar          = 118000,
            ShieldWall          = 871,
            DemoralizingShout   = 1160,
            LastStand           = 12975,
            Recklessness        = 1719
        };

        enum eData
        {
            ReduceSpellIdMax = 13
        };

        std::array<uint32, ReduceSpellIdMax> m_ReducedSpellsId =
        {
            {
                Bloodbath,
                Bladestorm,
                BladestormArm,
                DragonRoar,
                ShieldWall,
                DemoralizingShout,
                LastStand,
                Recklessness,
            }
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After)
                return;

            if (!p_Player || p_Player->getClass() != CLASS_WARRIOR || p_Power != POWER_RAGE || p_Regen)
                return;

            SpellEffIndex l_EffIndex = p_Player->GetActiveSpecializationID() == SPEC_WARRIOR_PROTECTION ? EFFECT_1 : EFFECT_0;
            AuraEffect* l_AngerManagementAura = p_Player->GetAuraEffect(eSpells::AngerManagement, l_EffIndex);

            if (!l_AngerManagementAura)
                return;

            // Get the power earn (if > 0 ) or consum (if < 0)
            int32 l_diffValue = (p_OldValue - p_NewValue) / p_Player->GetPowerCoeff(POWER_RAGE);

            // Only get spended rage
            if (l_diffValue <= 0)
                return;

            int32 l_accumulatedRage = l_diffValue + p_Player->m_SpellHelper.GetUint32(eSpellHelpers::AngerManagmentAccumulatedRage);

            if (l_accumulatedRage >= l_AngerManagementAura->GetAmount())
            {
                for (uint8 l_I = 0; l_I < eData::ReduceSpellIdMax; l_I++)
                {
                    if (p_Player->HasSpell(m_ReducedSpellsId[l_I]))
                    {
                        if (p_Player->HasSpellCooldown(m_ReducedSpellsId[l_I]))
                        {
                            uint32 l_MaxCount = (uint32)(l_accumulatedRage / l_AngerManagementAura->GetAmount());
                            for (uint32 l_J = 0; l_J < l_MaxCount; l_J++)
                                p_Player->ReduceSpellCooldown(m_ReducedSpellsId[l_I], 1 * IN_MILLISECONDS);
                        }
                    }
                }
            }

            p_Player->m_SpellHelper.GetUint32(eSpellHelpers::AngerManagmentAccumulatedRage) = l_diffValue % l_AngerManagementAura->GetAmount();
        }
};

/// Last Update 7.3.2 - 25549
/// Execute (Arms) - 163201 - execute (Death Row Arms) - 217955
class spell_warr_execute: public SpellScriptLoader
{
    public:
        spell_warr_execute() : SpellScriptLoader("spell_warr_execute") { }

        class spell_warr_execute_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_execute_SpellScript);

            int32 m_ConsumedPct = 0;
            int32 m_PowerCostTemp = 0;

            enum eSpells
            {
                SuddenDeath         = 52437,
                AyalasStoneHeart    = 225947,   ///< legendary item proc
                Dauntless           = 202297,
                PreciseStrikes      = 248195,
                Tactician           = 184783,
                ExploitTheWeakness  = 209494,
                TacticianEffect     = 199854,
                ColossusSmash       = 167105,
                MortalStrike        = 12294,
                ShatteredDefenses   = 248625
            };

            enum eArtifactPowers
            {
                ExploitTheWeaknessPowerId = 1150
            };

            bool m_AyalasStoneHeartProc = false;

            int32 m_RageUsed = 0;
            void HandleEnergize(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                int32 l_Damage = GetHitDamage();

                /// If damage is 0 we should return script, to prevent double rage consuming
                if (l_Damage == 0)
                    return;

                if (l_Target->GetHealthPct() < 20.0f || l_Target != GetExplTargetUnit() || m_AyalasStoneHeartProc) ///< Sudden Death: however when you use the proc at execute range (<20% HP) it will still consume extra rage and do the extra damage just with no initial cost.
                    SetHitDamage(CalculatePct(l_Damage * 4, m_ConsumedPct ? m_ConsumedPct : 100)); ///< if m_ConsumedPct = 0, it means the caster has talent Deadly Calm and is under battle cry, refunding all power costs: Execute should deal full damage (7.1.5 - build 23420)

                /// Sudden Death
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::SuddenDeath))
                    l_Aura->Remove();

                /// Shattered Defenses
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::ShatteredDefenses))
                    l_Aura->Remove();

                /// Tactician
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Tactician, EFFECT_1))
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (l_Player->HasAura(eSpells::ExploitTheWeakness))
                        {
                            SpellInfo const* l_WeaknessSpellInfo = sSpellMgr->GetSpellInfo(eSpells::ExploitTheWeakness);
                            if (l_WeaknessSpellInfo == nullptr)
                                return;

                            uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::ExploitTheWeaknessPowerId);
                            uint32 l_PowerBonus = l_WeaknessSpellInfo->Effects[EFFECT_0].BasePoints * l_Rank;
                            AddPct(m_PowerCostTemp, l_PowerBonus);
                        }

                        float chance = (m_PowerCostTemp / l_Player->GetPowerCoeff(POWER_RAGE)) * (l_AuraEffect->GetAmount() / 100.0f);

                        if (roll_chance_f(chance))
                        {
                            if (SpellInfo const* l_MortalStrike = sSpellMgr->GetSpellInfo(eSpells::MortalStrike))
                                l_Player->RestoreCharge(l_MortalStrike->ChargeCategoryEntry);

                            l_Player->CastSpell(l_Player, eSpells::TacticianEffect, true);
                        }
                    }
                }

                /// Restore 30% of rage used
                if (GetHitDamage() < l_Target->GetHealth())
                {
                    if (int32 l_RefundPct = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_4].BasePoints)
                        l_Caster->ModifyPower(Powers::POWER_RAGE, CalculatePct(m_RageUsed, l_RefundPct));
                }
            }

            void HandleTakePowers(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != Powers::POWER_RAGE)
                    return;

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                const SpellPowerEntry* l_PowerEntry = nullptr;

                if (!l_Caster || !l_SpellInfo || !(l_PowerEntry = l_SpellInfo->GetSpellPower(POWER_RAGE)))
                    return;

                Unit* l_Target = GetExplTargetUnit();

                int32 l_PowerCost = l_PowerEntry->Cost;
                int32 l_MaxPowerCost = l_PowerCost + 300;
                int32 l_MaxPowerCostTemp = l_MaxPowerCost;

                /// Apply cost mod by spell
                if (Player* modOwner = l_Caster->GetSpellModOwner())
                    modOwner->ApplySpellMod(l_SpellInfo->Id, SPELLMOD_COST, l_MaxPowerCost);

                int32 l_TotalRage = l_Caster->GetPower(POWER_RAGE);
                int32 l_ToUse = 0;
                int32 l_ToUseTemp = 0;

                if (l_Caster->HasAura(eSpells::AyalasStoneHeart))
                {
                    p_PowerCost   = 0;
                    m_PowerCostTemp = 400;
                    m_ConsumedPct = 100;
                    m_AyalasStoneHeartProc = true;
                    return;
                }

                if (l_TotalRage < l_MaxPowerCost)
                    l_ToUse = l_TotalRage;
                else
                    l_ToUse = l_MaxPowerCost;

                if (l_TotalRage < l_MaxPowerCostTemp)
                    l_ToUseTemp = l_TotalRage;
                else
                    l_ToUseTemp = l_MaxPowerCostTemp;

                m_ConsumedPct = static_cast<uint32>((static_cast<float>(l_ToUse) / l_MaxPowerCost) * 100); ///< Calculate percentage of the max power cost used
                p_PowerCost = l_ToUse;
                m_PowerCostTemp = l_ToUseTemp;
                m_RageUsed = p_PowerCost;
            }

            void HandleAfterHit()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->RemoveAura(eSpells::PreciseStrikes);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_execute_SpellScript::HandleEnergize, EFFECT_2, SPELL_EFFECT_ENERGIZE);
                OnHit += SpellHitFn(spell_warr_execute_SpellScript::HandleOnHit);
                OnTakePowers += SpellTakePowersFn(spell_warr_execute_SpellScript::HandleTakePowers);
                AfterHit += SpellHitFn(spell_warr_execute_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_execute_SpellScript();
        }
};

/// Execute - 5308 (Prot, Fury, Default)
/// last update : 6.1.2 19802
class spell_warr_execute_default: public SpellScriptLoader
{
    public:
        spell_warr_execute_default() : SpellScriptLoader("spell_warr_execute_default") { }

        class spell_warr_execute_default_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_execute_default_SpellScript);

            enum eSpells
            {
                SpellWarrExecuteOffHand = 163558,
            };

            void HandleOnCast()
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->GetActiveSpecializationID() == SPEC_WARRIOR_FURY)
                    l_Caster->CastSpell(l_Target, eSpells::SpellWarrExecuteOffHand, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_warr_execute_default_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_execute_default_SpellScript();
        }
};

/// BloodBath - 12292
class spell_warr_blood_bath : public SpellScriptLoader
{
    public:
        spell_warr_blood_bath() : SpellScriptLoader("spell_warr_blood_bath") { }

        class spell_warr_blood_bath_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_warr_blood_bath_Aurascript);

            enum eSpells
            {
                BloodBath       = 12292,
                BloodBathDamage = 113344,
                OdynsFuryDmg1   = 205546,
                OdynsFuryDmg2   = 205547,
            };

            void HandleOnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& p_ProcInfo)
            {
                PreventDefaultAction();

                if (!p_ProcInfo.GetDamageInfo() || !p_ProcInfo.GetDamageInfo()->GetAmount())
                    return;

                if (p_ProcInfo.GetDamageInfo()->GetSpellInfo() && p_ProcInfo.GetDamageInfo()->GetSpellInfo()->Id == eSpells::BloodBathDamage)
                    return;

                Unit* l_Target = p_ProcInfo.GetActionTarget();
                Unit* l_Caster = GetCaster();
                if (l_Target == nullptr || l_Caster == nullptr || l_Target == l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BloodBath);
                SpellInfo const* l_SpellInfoDamage = sSpellMgr->GetSpellInfo(eSpells::BloodBathDamage);
                Spell const* l_Spell = p_ProcInfo.GetDamageInfo()->GetSpell();
                if (!l_SpellInfo || !l_SpellInfoDamage)
                    return;

                if ((l_Spell && l_Spell->m_castItemGUID) || (p_ProcInfo.GetDamageInfo()->GetSchoolMask() != SPELL_SCHOOL_MASK_NORMAL &&
                    (l_Spell && l_Spell->GetSpellInfo()->Id != OdynsFuryDmg1 && l_Spell->GetSpellInfo()->Id != OdynsFuryDmg2)))
                    return;

                int32 l_Damage = CalculatePct(p_ProcInfo.GetDamageInfo()->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);

                int32 l_PreviousTotalDamage = 0;

                if (AuraEffect* l_PreviousBloodBath = l_Target->GetAuraEffect(eSpells::BloodBathDamage, EFFECT_0, l_Caster->GetGUID()))
                {
                    int32 l_PeriodicDamage = l_PreviousBloodBath->GetAmount();
                    int32 l_Amplitude = l_PreviousBloodBath->GetAmplitude();

                    if (l_PreviousBloodBath->GetTotalTicks())
                    {
                        if (l_Amplitude)
                            l_PreviousTotalDamage = l_PeriodicDamage * (l_PreviousBloodBath->GetTotalTicks() - l_PreviousBloodBath->GetTickNumber());

                        l_PreviousTotalDamage /= l_PreviousBloodBath->GetTotalTicks();
                    }
                }

                if (l_Target->HasAura(eSpells::BloodBathDamage, l_Caster->GetGUID()))
                {
                    if (Aura* l_ActualBloodBath = l_Target->GetAura(eSpells::BloodBathDamage, l_Caster->GetGUID()))
                        l_ActualBloodBath->SetDuration(l_ActualBloodBath->GetMaxDuration());
                }
                else
                    l_Caster->CastSpell(l_Target, eSpells::BloodBathDamage, true);

                if (AuraEffect* l_NewBloodBath = l_Target->GetAuraEffect(eSpells::BloodBathDamage, EFFECT_0, l_Caster->GetGUID()))
                {
                    if (l_SpellInfoDamage->Effects[EFFECT_0].Amplitude)
                        l_Damage /= std::max(1, int32(l_NewBloodBath->GetTotalTicks() - l_NewBloodBath->GetTickNumber()));

                    l_Damage += l_PreviousTotalDamage;
                    l_NewBloodBath->SetAmount(l_Damage);

                    l_NewBloodBath->ResetPeriodic(false);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_blood_bath_Aurascript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_blood_bath_Aurascript();
        }
};

/// Last Update 6.2.3
/// Single-Minded Fury - 81099
class spell_warr_single_minded_fury : public SpellScriptLoader
{
    public:
        spell_warr_single_minded_fury() : SpellScriptLoader("spell_warr_single_minded_fury") { }

        class spell_warr_single_minded_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_single_minded_fury_AuraScript);

            void CalculateFirstEffect(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (GetCaster() == nullptr)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    Item* l_MainItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                    Item* l_OffHandItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

                    if (l_MainItem && l_OffHandItem)
                    {
                        if (l_MainItem->GetTemplate()->IsTwoHandedWeapon() || l_OffHandItem->GetTemplate()->IsTwoHandedWeapon())
                            p_Amount = 0;
                    }
                    else
                        p_Amount = 0;
                }
            }

            void CalculateSecondEffect(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (GetCaster() == nullptr)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    Item* l_MainItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                    Item* l_OffHandItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

                    if (l_MainItem && l_OffHandItem)
                    {
                        if (l_MainItem->GetTemplate()->IsTwoHandedWeapon() || l_OffHandItem->GetTemplate()->IsTwoHandedWeapon())
                            p_Amount = 0;
                    }
                    else
                        p_Amount = 0;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_single_minded_fury_AuraScript::CalculateFirstEffect, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_single_minded_fury_AuraScript::CalculateSecondEffect, EFFECT_1, SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_single_minded_fury_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Unyielding Strikes - 169685
class spell_warr_unyielding_strikes : public SpellScriptLoader
{
    public:
        spell_warr_unyielding_strikes() : SpellScriptLoader("spell_warr_unyielding_strikes") { }

        class spell_warr_unyielding_strikes_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_warr_unyielding_strikes_Aurascript);

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_ProcInfos*/)
            {
                PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_unyielding_strikes_Aurascript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_unyielding_strikes_Aurascript();
        }
};

/// last update : 7.3.2 Build 25549
/// Glyph of Crow Feast - 115943
class spell_warr_glyph_of_crow_feast : public SpellScriptLoader
{
    public:
        spell_warr_glyph_of_crow_feast() : SpellScriptLoader("spell_warr_glyph_of_crow_feast") { }

        class spell_warr_glyph_of_crow_feast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_glyph_of_crow_feast_AuraScript);

            enum eSpells
            {
                Execute             = 163201,
                GlyphOfCrowFeast    = 115944
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                if (p_ProcEventInfo.GetDamageInfo() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetDamageInfo()->GetTarget();
                SpellInfo const* l_SpellInfoTriggerSpell = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_Caster == nullptr || l_Target == nullptr || l_SpellInfoTriggerSpell == nullptr)
                    return;

                if (l_SpellInfoTriggerSpell->Id != eSpells::Execute)
                    return;

                if (!(p_ProcEventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::GlyphOfCrowFeast, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_glyph_of_crow_feast_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_glyph_of_crow_feast_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Revenge - 5301
class spell_warr_revenge : public SpellScriptLoader
{
    public:
        spell_warr_revenge() : SpellScriptLoader("spell_warr_revenge") { }

        class spell_warr_revenge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_revenge_AuraScript);

            enum eSpells
            {
                Revenger    = 6572,
                RevengeProc = 5302,
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcInfos)
            {
                // if default action is prevented, we need to cast a trigger spell manually.
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (!(p_ProcInfos.GetHitMask() & (PROC_EX_PARRY | PROC_EX_DODGE)))
                    return;

                if (p_ProcInfos.GetDamageInfo() == nullptr || p_ProcInfos.GetDamageInfo()->GetTarget() == nullptr)
                    return;

                Player* l_Player = p_ProcInfos.GetDamageInfo()->GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (l_Player->HasSpellCooldown(eSpells::Revenger))
                    l_Player->RemoveSpellCooldown(eSpells::Revenger, true);

                l_Player->CastSpell(l_Player, eSpells::RevengeProc, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_revenge_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_revenge_AuraScript();
        }
};

/// Glyph of the Weaponmaster - 146974
/// Weaponmaster - 147367
class spell_warr_weaponmaster : public SpellScriptLoader
{
public:
    spell_warr_weaponmaster() : SpellScriptLoader("spell_warr_weaponmaster") { }

    class spell_warr_weaponmaster_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_weaponmaster_AuraScript);

        void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (!GetCaster())
                return;

            if (Player* l_Player = GetCaster()->ToPlayer())
            {
                for (uint16 l_I = EQUIPMENT_SLOT_MAINHAND; l_I <= EQUIPMENT_SLOT_OFFHAND; l_I++)
                    if (Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_I))
                        l_Item->RandomWeaponTransmogrificationFromPrimaryBag(l_Player, l_Item, true);
            }
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (!GetCaster())
                return;

            if (Player* l_Player = GetCaster()->ToPlayer())
            {
                for (uint16 l_I = EQUIPMENT_SLOT_MAINHAND; l_I <= EQUIPMENT_SLOT_OFFHAND; l_I++)
                    if (Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_I))
                        l_Item->RandomWeaponTransmogrificationFromPrimaryBag(l_Player, l_Item, false);
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_warr_weaponmaster_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_warr_weaponmaster_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_weaponmaster_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Cleave - 845
class spell_warr_cleave : public SpellScriptLoader
{
    public:
        spell_warr_cleave() : SpellScriptLoader("spell_warr_cleave") { }

        class spell_warr_cleave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_cleave_SpellScript);

            enum eSpells
            {
                CleaveAura          = 188923,
                VoidCleaveAura      = 209573,
                VoidCleaveDamage    = 209700,
                OneAgainstMany      = 209462
            };

            uint32 m_HitUnit = 0;

            void HandleOnHit()
            {
                m_HitUnit++;
            }

            void HandleCleave()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CleaveAura);
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_Bp = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                Player* l_Player = l_Caster->ToPlayer();
                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::OneAgainstMany, EFFECT_0);
                if (l_Player && l_AuraEffect)
                    l_Bp += l_AuraEffect->GetAmount();

                l_Caster->CastCustomSpell(l_Caster, eSpells::CleaveAura, &l_Bp, nullptr, nullptr, true);
            }

            void HandleVoidCleave()
            {
                if (m_HitUnit != 3)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::VoidCleaveAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::VoidCleaveDamage, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_cleave_SpellScript::HandleOnHit);
                AfterHit += SpellHitFn(spell_warr_cleave_SpellScript::HandleCleave);
                AfterHit += SpellHitFn(spell_warr_cleave_SpellScript::HandleVoidCleave);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_cleave_SpellScript();
        }
};

/// Last Update 7.1.5 - 23420
class spell_warr_tactician : public SpellScriptLoader
{
    public:
        spell_warr_tactician() : SpellScriptLoader("spell_warr_tactician") { }

        enum eSpells
        {
            Tactician           = 184783,
            TacticianEffect     = 199854,
            ExploitTheWeakness  = 209494,
            MortalStrike        = 12294,
            Execute             = 163201,
            Whirlwind           = 1680,
            WhirlwindTrigger    = 199658
        };

        enum eArtifactPowers
        {
            ExploitTheWeaknessPowerId = 1150
        };

        class spell_warr_tactician_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_tactician_AuraScript);

            Guid128 m_CastId;

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                if (!GetUnitOwner() || !GetUnitOwner()->IsPlayer() || !p_ProcEventInfo.GetDamageInfo()->GetSpell())
                    return;

                if (m_CastId == p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid())
                    return;

                m_CastId = p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid();
                Player* l_Owner = GetUnitOwner()->ToPlayer();

                if (l_Owner->getClass() != CLASS_WARRIOR)
                    return;

                if (!p_ProcEventInfo.GetDamageInfo())
                    return;

                SpellInfo const* l_triggerSpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_triggerSpellInfo)
                    return;

                if (l_triggerSpellInfo->Id == eSpells::Execute)
                    return;

                if (l_triggerSpellInfo->SpellPowers.empty())
                {
                    if (l_triggerSpellInfo->Id == eSpells::WhirlwindTrigger)
                    {
                        l_triggerSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Whirlwind);
                        if (!l_triggerSpellInfo || l_triggerSpellInfo->SpellPowers.empty())
                            return;
                    }
                    else
                        return;
                }

                int32 l_RageCost = 0;
                for (auto spellPower : l_triggerSpellInfo->SpellPowers)
                {
                    if (spellPower->PowerType == POWER_RAGE)
                        l_RageCost += spellPower->Cost;
                }

                if (l_Owner->HasAura(eSpells::ExploitTheWeakness))
                {
                    SpellInfo const* l_WeaknessSpellInfo = sSpellMgr->GetSpellInfo(eSpells::ExploitTheWeakness);
                    if (l_WeaknessSpellInfo == nullptr)
                        return;
                    uint8 l_Rank = l_Owner->GetRankOfArtifactPowerId(eArtifactPowers::ExploitTheWeaknessPowerId);
                    uint32 l_PowerBonus = l_WeaknessSpellInfo->Effects[EFFECT_0].BasePoints * l_Rank;
                    AddPct(l_RageCost, l_PowerBonus);
                }

                AuraEffect* l_AuraEffect = GetAura()->GetEffect(EFFECT_1);
                if (l_AuraEffect == nullptr)
                    return;

                float chance = (l_RageCost / l_Owner->GetPowerCoeff(POWER_RAGE)) * (l_AuraEffect->GetAmount() / 100.0f);

                if (roll_chance_f(chance))
                {
                    uint64 l_CasterGuid = l_Owner->GetGUID();
                    l_Owner->m_Functions.AddFunction([l_CasterGuid]() -> void
                    {
                        Player* l_Player = sObjectAccessor->FindPlayer(l_CasterGuid);
                        if (!l_Player)
                            return;

                        if (SpellInfo const* l_MortalStrike = sSpellMgr->GetSpellInfo(eSpells::MortalStrike))
                            l_Player->RestoreCharge(l_MortalStrike->ChargeCategoryEntry);
                    }, l_Owner->m_Functions.CalculateTime(100));

                    l_Owner->CastSpell(l_Owner, TacticianEffect, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_tactician_AuraScript::OnProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_tactician_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Tactician Effect - 199854
class spell_warr_tactician_effect : public SpellScriptLoader
{
public:
    spell_warr_tactician_effect() : SpellScriptLoader("spell_warr_tactician_effect") { }

    class spell_warr_tactician_effect_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_tactician_effect_SpellScript);

        enum eSpells
        {
            ColossusSmash = 167105
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (Player* l_Player = l_Caster->ToPlayer())
                l_Player->RemoveSpellCooldown(eSpells::ColossusSmash, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_warr_tactician_effect_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {return new spell_warr_tactician_effect_SpellScript();
    }
};

/// Last Update 7.0.3 - 22045
/// Frenzy - 206313
class spell_warr_frenzy : public SpellScriptLoader
{
    public:
        spell_warr_frenzy() : SpellScriptLoader("spell_warr_frenzy") { }

        class spell_warr_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_frenzy_AuraScript);

            enum eSpells
            {
                FuriousSlash = 100130
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_TriggeredBySpell = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_TriggeredBySpell == nullptr)
                    return;

                if (l_TriggeredBySpell->Id != eSpells::FuriousSlash)
                    return;

                uint32 l_SpellId = p_AurEff->GetTriggerSpell();
                l_Caster->CastSpell(l_Caster, l_SpellId, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_frenzy_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_frenzy_AuraScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Massacre - 206315
class spell_warr_massacre : public SpellScriptLoader
{
    public:
        spell_warr_massacre() : SpellScriptLoader("spell_warr_massacre") { }

        class spell_warr_massacre_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_massacre_AuraScript);

            enum eSpells
            {
                Execute         = 5308,
                ExecuteOffHand  = 163558,
                ExecuteOffHand2 = 217957
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_TriggeredBySpell = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_TriggeredBySpell == nullptr)
                    return;

                /// Only proc from Execute
                if (l_TriggeredBySpell->Id != eSpells::Execute && l_TriggeredBySpell->Id != eSpells::ExecuteOffHand && l_TriggeredBySpell->Id != eSpells::ExecuteOffHand2)
                    return;

                /// Should be critical
                if (!(p_ProcEventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                    return;

                uint32 l_SpellId = p_AurEff->GetTriggerSpell();
                l_Caster->CastSpell(l_Caster, l_SpellId, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_massacre_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_massacre_AuraScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Intercept - 198304
class spell_warr_intercept : public SpellScriptLoader
{
    public:
        spell_warr_intercept() : SpellScriptLoader("spell_warr_intercept") { }

        class spell_warr_intercept_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_intercept_SpellScript);

            enum eSpells
            {
                Intervene           = 147833,
                Charge              = 40602,
                ChargeDamage        = 126664,
                ChargeRoot          = 105771,
                ChargeSlow          = 236027,
                WarbringerDamage    = 7922,
                WarbringerStun      = 237744,
                Warbringer          = 103828,
                Safeguard           = 223657,
                SafeguardTrigger    = 223658,
                LeaveNoManBehind    = 199037,
                LeaveNoManBehindTrigger = 199038
            };


            SpellCastResult CheckRange()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                float l_MinimumRange = (float)GetSpellInfo()->Effects[EFFECT_0].BasePoints;

                if (l_Target == nullptr || l_Target == l_Caster)
                    return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                if (l_Caster->IsValidAttackTarget(l_Target))
                {
                    if (l_Caster->GetDistance(l_Target) < l_MinimumRange)
                        return SpellCastResult::SPELL_FAILED_TOO_CLOSE;
                }

                if (l_Caster->isInRoots())
                    return SpellCastResult::SPELL_FAILED_ROOTED;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    l_Caster->CastSpell(l_Caster, eSpells::Intervene, true);

                if (l_Caster->IsValidAttackTarget(l_Target))
                {
                    l_Caster->CastSpell(l_Target, eSpells::Charge, true);
                    l_Caster->CastSpell(l_Target, eSpells::ChargeDamage, true);
                    l_Caster->CastSpell(l_Target, eSpells::ChargeRoot, true);
                    l_Caster->CastSpell(l_Target, eSpells::ChargeSlow, true);
                }

                if (l_Caster->IsValidAssistTarget(l_Target))
                {
                    l_Caster->CastSpell(l_Target, eSpells::Intervene, true);
                    if (l_Caster->HasAura(eSpells::Safeguard))
                        l_Caster->CastSpell(l_Target, eSpells::SafeguardTrigger, true);
                    if (l_Caster->HasAura(eSpells::LeaveNoManBehind))
                        l_Caster->CastSpell(l_Target, eSpells::LeaveNoManBehindTrigger, true);
                }

                if (l_Caster->HasAura(eSpells::Warbringer))
                {
                    l_Caster->CastSpell(l_Target, eSpells::WarbringerDamage, true);
                    l_Caster->CastSpell(l_Target, eSpells::WarbringerStun, true);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warr_intercept_SpellScript::CheckRange);
                OnEffectHitTarget += SpellEffectFn(spell_warr_intercept_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_intercept_SpellScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Devastate - 20243
class spell_warr_devastate : public SpellScriptLoader
{
    public:
        spell_warr_devastate() : SpellScriptLoader("spell_warr_devastate") { }

        class spell_warr_devastate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_devastate_SpellScript);

            enum eSpells
            {
                ShieldSlam = 23922
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                uint8 l_ChancePct = GetSpellInfo()->Effects[EFFECT_2].BasePoints;
                if (roll_chance_i(l_ChancePct))
                    l_Player->RemoveSpellCooldown(eSpells::ShieldSlam, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_devastate_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_devastate_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Ignore Pain - 190456
class spell_warr_ignore_pain : public SpellScriptLoader
{
    public:
        spell_warr_ignore_pain() : SpellScriptLoader("spell_warr_ignore_pain") { }

        enum eSpells
        {
            VenganceIgnorePain  = 202574,
            NeverSurrender      = 202561,
            Indomitable         = 202095,
            IgnorePain          = 190456,
            DragonScales        = 203581,
            DragonSkin          = 203225
        };

        enum eArtifactPowers
        {
            DragonSkinArtifact = 100
        };

        class spell_warr_ignore_pain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_ignore_pain_AuraScript);

            void HandleCalculateAmount(AuraEffect const*, int32& p_Amount, bool&) ///< Done exactly as displayed on https://www.askmrrobot.com/wow/theory/mechanic/spell/ignorepain?spec=WarriorProtection&version=7_3_5_25864
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                const SpellPowerEntry* l_PowerEntry = nullptr;
                if (!l_Caster || !l_SpellInfo || !(l_PowerEntry = l_SpellInfo->GetSpellPower(POWER_RAGE)))
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Indomitable, EFFECT_1))
                    AddPct(p_Amount, l_AuraEffect->GetAmount());

                int32 l_MaxAbsorb = p_Amount * 3;

                if (AuraEffect* l_DragonSkinEffect = l_Caster->GetAuraEffect(eSpells::DragonSkin, EFFECT_0))
                    AddPct(p_Amount, l_DragonSkinEffect->GetAmount());

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::NeverSurrender, EFFECT_0))
                {
                    float l_MissingHealthPct = 100.0f - l_Caster->GetHealthPct();
                    AddPct(p_Amount, CalculatePct(l_MissingHealthPct, l_AuraEffect->GetAmount()));
                    l_MaxAbsorb *= 2;
                }

                if (AuraEffect* l_DragonScaleEffect = l_Caster->GetAuraEffect(eSpells::DragonScales, EFFECT_0))
                {
                    int32 l_DragonScaleIncrease = CalculatePct(p_Amount, l_DragonScaleEffect->GetAmount());
                    p_Amount += l_DragonScaleIncrease;
                    l_MaxAbsorb += l_DragonScaleIncrease;
                }

                int32 l_PctRagetoUse = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IgnorePain);
                p_Amount = CalculatePct(p_Amount, l_PctRagetoUse);

                int32 l_MaxPowerCost = l_PowerEntry->Cost + l_PowerEntry->CostAdditional;
                int32 l_RageToUse = CalculatePct(l_MaxPowerCost, l_PctRagetoUse) / l_Caster->GetPowerCoeff(POWER_RAGE);

                if (AuraEffect* l_Vengeance = l_Caster->GetAuraEffect(eSpells::VenganceIgnorePain, EFFECT_0))
                    l_RageToUse = CalculatePct(l_RageToUse, 100 + l_Vengeance->GetAmount());

                l_Caster->ModifyPower(POWER_RAGE, -l_RageToUse * l_Caster->GetPowerCoeff(POWER_RAGE));

                if (l_Caster->CanApplyPvPSpellModifiers()) ///< 20% nerf to gained absorb, and only to this. not nerfing max cap. https://cdn.discordapp.com/attachments/424196700287139841/463738101404270602/unknown.png
                    p_Amount *= 0.8f;

                if (Aura* l_PreviousAura = l_Caster->GetAura(eSpells::IgnorePain))
                    p_Amount += l_PreviousAura->GetEffect(EFFECT_0)->GetAmount();

                if (p_Amount < 0)
                    p_Amount = 0;

                if (p_Amount > l_MaxAbsorb)
                    p_Amount = l_MaxAbsorb;
            }

            void HandleAfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->RemoveAura(eSpells::DragonScales);
            }

            void HandleOnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster || !l_SpellInfo)
                    return;

                int32 l_AbsorbPct = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                int32 l_AbsorbCapacity = p_AurEff->GetAmount();

                if (l_Caster->IsPlayer() && l_Caster->HasAura(eSpells::DragonSkin))
                {
                    Player* l_Player = l_Caster->ToPlayer();
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::DragonSkinArtifact);
                    SpellInfo const* l_DragonSkinInfo = sSpellMgr->GetSpellInfo(eSpells::DragonSkin);

                    if (l_DragonSkinInfo)
                        l_AbsorbPct += l_Rank * l_DragonSkinInfo->Effects[EFFECT_0].BasePoints;
                }

                int32 l_ToAbsorb = CalculatePct(p_DmgInfo.GetAmount(), l_AbsorbPct);

                if (l_ToAbsorb > l_AbsorbCapacity)
                    l_ToAbsorb = l_AbsorbCapacity;

                p_AbsorbAmount = l_ToAbsorb;
                p_AurEff->ChangeAmount(l_AbsorbCapacity - l_ToAbsorb);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_ignore_pain_AuraScript::HandleCalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_warr_ignore_pain_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_warr_ignore_pain_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_ignore_pain_AuraScript();
        }

        class spell_warr_ignore_pain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_ignore_pain_SpellScript);

            void HandleOnPrepare()
            {
                Spell* l_Spell = GetSpell();

                if (l_Spell)
                    l_Spell->SetCustomCritChance(0.0f);
            }

            void HandleTakePowers(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != Powers::POWER_RAGE)
                    return;

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                const SpellPowerEntry* l_PowerEntry = nullptr;

                if (!l_Caster || !l_SpellInfo || !(l_PowerEntry = l_SpellInfo->GetSpellPower(POWER_RAGE)))
                    return;

                int32 l_MaxPowerCost = l_PowerEntry->Cost + l_PowerEntry->CostAdditional;
                if (l_Caster->HasAura(eSpells::VenganceIgnorePain))
                    l_MaxPowerCost = 390;
                int32 l_TotalRage = l_Caster->GetPower(POWER_RAGE);
                int32 l_ToUse = 0;

                if (l_TotalRage < l_MaxPowerCost)
                    l_ToUse = l_TotalRage;
                else
                    l_ToUse = l_MaxPowerCost;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IgnorePain) = static_cast<uint32>((static_cast<float>(l_ToUse) / l_MaxPowerCost) * 100); ///< Calculate percentage of the max power cost used

                p_PowerCost = 0;
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_warr_ignore_pain_SpellScript::HandleOnPrepare);
                OnTakePowers += SpellTakePowersFn(spell_warr_ignore_pain_SpellScript::HandleTakePowers);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_ignore_pain_SpellScript();
        }
};

/// Last Update 7.3.5 - 25996
/// Inspiring Presence - 205484
class spell_warr_inspiring_presence : public SpellScriptLoader
{
    public:
        spell_warr_inspiring_presence() : SpellScriptLoader("spell_warr_inspiring_presence") { }

    class spell_warr_inspiring_presence_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_inspiring_presence_AuraScript);

        enum eSpells
        {
            InspiringPresenceHeal = 222944
        };

        void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
        {
            PreventDefaultAction();
            Unit* l_Target = GetTarget();

            if (!l_Target)
                return;

            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (l_DamageInfo == nullptr)
                return;

            Unit* l_Attacker = l_DamageInfo->GetActor();
            if (l_Attacker == nullptr)
                return;

            if (l_Attacker->GetGUID() != l_Target->GetGUID())
                return;

            uint64 l_CasterGuid = GetCasterGUID();
            Player* l_Caster = sObjectAccessor->FindPlayer(l_CasterGuid);
            if (!l_Caster)
                return;

            int32 l_HealAmount = CalculatePct(l_DamageInfo->GetAmount(), p_AurEff->GetAmount());
            l_Caster->CastCustomSpell(l_Target, eSpells::InspiringPresenceHeal, &l_HealAmount, NULL, NULL, true);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_warr_inspiring_presence_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_inspiring_presence_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Impending Victory - 202168
class spell_warr_impending_victory : public SpellScriptLoader
{
    public:
        spell_warr_impending_victory() : SpellScriptLoader("spell_warr_impending_victory") { }

        class spell_warr_impending_victory_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_impending_victory_SpellScript);

            enum eSpells
            {
                ImpendingVictoryHeal    = 202166,
                Victorious              = 32216
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::Victorious);
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ImpendingVictoryHeal, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_impending_victory_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_warr_impending_victory_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_impending_victory_SpellScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Ultimatum - 122509
class spell_warr_ultimatum : public SpellScriptLoader
{
    public:
        spell_warr_ultimatum() : SpellScriptLoader("spell_warr_ultimatum") { }

        class spell_warr_ultimatum_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_ultimatum_AuraScript);

            enum eSpells
            {
                ShieldSlam = 23922
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_TriggeredBySpell = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_TriggeredBySpell == nullptr)
                    return;

                /// Only proc from Shield Slam
                if (l_TriggeredBySpell->Id != eSpells::ShieldSlam)
                    return;

                /// Should be critical
                if (!(p_ProcEventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                    return;

                uint32 l_SpellId = p_AurEff->GetTriggerSpell();
                l_Caster->CastSpell(l_Caster, l_SpellId, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_ultimatum_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_ultimatum_AuraScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Whirlwind - 1680, 190411
class spell_warr_whirliwind : public SpellScriptLoader
{
    public:
        spell_warr_whirliwind() : SpellScriptLoader("spell_warr_whirliwind") { }

        class spell_warr_whirliwind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_whirliwind_SpellScript);

            enum eSpells
            {
                WhirlwindDamage = 199667,
                WhirlwindDamageOffHand = 199851,
                WreckingBall = 215570,
                NanjetussVertebrae = 215096,
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster && l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Whirlwind))
                {
                    if (l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::Whirlwind].size() >= 3)
                    {
                        Unit* l_Target = GetHitUnit();
                        if (l_Target)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::WhirlwindDamage, true);
                            if (l_Caster->haveOffhandWeapon())
                                l_Caster->CastSpell(l_Target, eSpells::WhirlwindDamageOffHand, true);
                        }
                    }
                }
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Whirlwind) = false;
                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::Whirlwind].clear();

                ProcessWrackingBall(l_Caster);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_warr_whirliwind_SpellScript::HandleBeforeCast);
                AfterHit += SpellHitFn(spell_warr_whirliwind_SpellScript::HandleAfterHit);
            }

        private:

            void ProcessWrackingBall(Unit* l_Caster)
            {
                if (l_Caster->HasAura(eSpells::WreckingBall))
                {
                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::WrackingBall) = time(NULL) + 2;
                    l_Caster->RemoveAura(eSpells::WreckingBall);
                }
                else
                {
                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::WrackingBall) = 0;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_whirliwind_SpellScript();
        }
};

/// Last Update 7.1.5
/// Whirlwind (damage) - 199667 44949, 199658, 199850
class spell_warr_whirlwind_damage : public SpellScriptLoader
{
    public:
        spell_warr_whirlwind_damage() : SpellScriptLoader("spell_warr_whirlwind_damage") { }

        class spell_warr_whirlwind_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_whirlwind_damage_SpellScript);

            enum eSpells
            {
                NanjetussVertebrae      = 215096,
                WreckingBallBuff        = 215570,
                FervorOfBattle          = 202316,
                MeatCleaver             = 85739
            };

            void FilterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster && l_Caster->HasAura(eSpells::NanjetussVertebrae))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Whirlwind) = true;
                    for (auto l_Target : p_Targets)
                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::Whirlwind].insert(l_Target->GetGUID());
                }

                if (!p_Targets.empty() && m_scriptSpellId == 199667) /// Should only proc when at least one target is hit, and added check on one of the 5 damage spells so it doesnt proc 5 times per cast (spell chosen only called in Fury spec, as expected)
                    l_Caster->CastSpell(l_Caster, eSpells::MeatCleaver, true);
            }

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                UpdateWrackingBallDamage(l_Caster);
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Unit* l_Victim = l_Caster->getVictim() ? l_Caster->getVictim() : l_Caster->getAttackerForHelper() ? l_Caster->getAttackerForHelper() : nullptr;

                if (l_Victim != l_Target || !l_Caster->HasAura(eSpells::FervorOfBattle))
                    return;

                int32 l_Damage = GetHitDamage();

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::FervorOfBattle, EFFECT_0))
                    AddPct(l_Damage, l_AuraEffect->GetAmount());
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_whirlwind_damage_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_warr_whirlwind_damage_SpellScript::HandleHit);
                OnEffectHitTarget += SpellEffectFn(spell_warr_whirlwind_damage_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }

        private:

            void UpdateWrackingBallDamage(Unit* l_Caster)
            {
                auto delayTime = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::WrackingBall);
                if (delayTime > time(NULL))
                {
                    auto l_spellInfoWreckingBall = sSpellMgr->GetSpellInfo(eSpells::WreckingBallBuff);
                    if (!l_spellInfoWreckingBall)
                        return;

                    uint32 damagePct = l_spellInfoWreckingBall->Effects[EFFECT_0].BasePoints;
                    if (!damagePct)
                        return;

                    SetHitDamage(GetHitDamage() * ((float)damagePct / 100));
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_whirlwind_damage_SpellScript();
        }
};


/// Last Update 7.3.2 - Build 25549
/// Mastodonte - 200875
class spell_warr_mastodonte : public SpellScriptLoader
{
    public:
        spell_warr_mastodonte() : SpellScriptLoader("spell_warr_mastodonte") { }

        class spell_warr_ultimatum_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_ultimatum_AuraScript);

            enum eSpells
            {
                Execute         = 5308,
                ExecuteOffHand  = 163558,
                ExecuteOffHand2 = 217957
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                SpellInfo const* l_TriggeredBySpell = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_TriggeredBySpell == nullptr)
                    return false;

                /// Only proc from Execute, but for some reason, making the check on the main hand prevents the cast of Juggernaut (but worked fine by putting a breakpoint... trully WTF)
                if (l_TriggeredBySpell->Id != eSpells::ExecuteOffHand && l_TriggeredBySpell->Id != eSpells::ExecuteOffHand2)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_ultimatum_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_ultimatum_AuraScript();
        }
};

/// last update : 7.0.2
/// Blood Hunt - 198492
class spell_warr_blood_hunt : public SpellScriptLoader
{
    public:
        spell_warr_blood_hunt() : SpellScriptLoader("spell_warr_blood_hunt") { }

        class spell_warr_blood_hunt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_blood_hunt_AuraScript);

            enum eSpells
            {
                BloodHuntAura = 198498
            };

            std::list<uint64> m_TargetList;

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::BloodHuntAura))
                    return;

                float l_Radius = (float)p_AurEff->GetAmount();

                std::list<Unit*> l_UnFriendlyList;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_UnFriendlyList, l_Check);
                l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                for (auto l_Target : l_UnFriendlyList)
                {
                    if (l_Target->HasAuraState(AuraStateType::AURA_STATE_HEALTHLESS_20_PERCENT))
                        l_Caster->CastSpell(l_Caster, eSpells::BloodHuntAura, true);
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_warr_blood_hunt_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_AREATRIGGER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_blood_hunt_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Warbreaker - 209574
class spell_warr_warbreaker : public SpellScriptLoader
{
    public:
        spell_warr_warbreaker() : SpellScriptLoader("spell_warr_warbreaker") { }

        class spell_warr_warbreaker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_warbreaker_SpellScript);

            enum eSpells
            {
                ColossusSmash           = 208086,
                ShatteredDefenses       = 209574,
                ShatteredDefensesBuff   = 209706
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ColossusSmash, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::ShatteredDefenses))
                    l_Caster->CastSpell(l_Caster, eSpells::ShatteredDefensesBuff, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_warbreaker_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                AfterCast += SpellCastFn(spell_warr_warbreaker_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_warbreaker_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dragon Charge - 206579
class spell_warr_dragon_charge : public SpellScriptLoader
{
public:
    spell_warr_dragon_charge() : SpellScriptLoader("spell_warr_dragon_charge") { }

    class spell_warr_dragon_charge_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_dragon_charge_SpellScript);

        void FilterTarget(WorldObject*& p_Target)
        {
            Unit* l_Target = p_Target->ToUnit();
            Unit* l_Caster = GetCaster();
            if (!l_Target || !l_Caster || !l_Caster->IsValidAttackTarget(l_Target))
                p_Target = nullptr;
        }

        void Register() override
        {
            OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_warr_dragon_charge_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_TARGET_ANY);
            OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_warr_dragon_charge_SpellScript::FilterTarget, EFFECT_1, TARGET_UNIT_TARGET_ANY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_dragon_charge_SpellScript();
    }
};

/// Last Update 7.0.3
/// Shield Slam - 23922
class spell_warr_shield_slam : public SpellScriptLoader
{
    public:
        spell_warr_shield_slam() : SpellScriptLoader("spell_warr_shield_slam") { }

        class spell_warr_shield_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_shield_slam_SpellScript);

            enum eSpells
            {
                ShieldBlockAura             = 132404,
                HeavyRepercussions          = 203177,
                ShieldWall                  = 871,
                TheWallsFell                = 215057
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();

                Aura* l_Aura = l_Caster->GetAura(eSpells::ShieldBlockAura);
                if (l_Aura != nullptr)
                {
                    AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::HeavyRepercussions, EFFECT_0);
                    if (l_AuraEffect != nullptr) ///< Shield Slam extends the duration of Shield Block by ${$m1/100}.
                    {
                        l_Aura->SetMaxDuration(l_Aura->GetDuration() + (l_AuraEffect->GetAmount() * 10));
                        l_Aura->SetDuration(l_Aura->GetDuration() + (l_AuraEffect->GetAmount() * 10));
                    }
                }

                Aura* l_AuraWallsFell = l_Caster->GetAura(eSpells::TheWallsFell);
                if (l_AuraWallsFell != nullptr)
                {
                    AuraEffect* l_AuraEffectCD = l_Caster->GetAuraEffect(eSpells::TheWallsFell, EFFECT_0);
                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player || !l_AuraEffectCD)
                        return;
                    l_Player->ReduceSpellCooldown(eSpells::ShieldWall, l_AuraEffectCD->GetAmount() * 1000);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_shield_slam_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_shield_slam_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called for Trauma - 215538
class spell_warr_trauma: public SpellScriptLoader
{
    public:
        spell_warr_trauma() : SpellScriptLoader("spell_warr_trauma") { }

        class spell_warr_trauma_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_trauma_AuraScript);

            enum eSpells
            {
                TraumaBleed = 215537,
                Slam        = 1464,
                Whirlwind   = 199658,
                Whirlwind2  = 199850,
                Execute     = 163201
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                Unit*  l_UnitTarget = p_EventInfo.GetActionTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_BleedSpellInfo = sSpellMgr->GetSpellInfo(eSpells::TraumaBleed);
                if (!l_Caster || !l_DamageInfo || !l_UnitTarget || !l_SpellInfo || !l_BleedSpellInfo)
                    return;

                int32 l_Amount = l_DamageInfo->GetAmount();
                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Amount || !l_Player || !l_ProcSpellInfo || (l_ProcSpellInfo->Id != eSpells::Slam && l_ProcSpellInfo->Id != eSpells::Whirlwind && l_ProcSpellInfo->Id != eSpells::Execute && l_ProcSpellInfo->Id != eSpells::Whirlwind2))
                    return;

                int32 l_Damage = CalculatePct(l_DamageInfo->GetAmount(), p_AurEff->GetAmount());
                int32 l_NumberOfTicks = l_BleedSpellInfo->GetMaxDuration() / l_BleedSpellInfo->Effects[EFFECT_0].Amplitude;
                l_Damage /= l_NumberOfTicks;

                if (AuraEffect* l_PreviousTrauma = l_UnitTarget->GetAuraEffect(eSpells::TraumaBleed, EFFECT_0, l_Caster->GetGUID()))
                {
                    int32 l_PreviousTotalDamage = 0;
                    int32 l_PeriodicDamage = l_PreviousTrauma->GetAmount();
                    int32 l_Amplitude = l_PreviousTrauma->GetAmplitude();
                    int32 l_PeriodicTick = l_PreviousTrauma->GetPeriodicTimer();
                    int32 l_Duration = l_PreviousTrauma->GetBase()->GetDuration();
                    if (!l_Amplitude)
                        return;

                    l_PreviousTotalDamage = l_PeriodicDamage * (((l_Duration - (l_Amplitude - l_PeriodicTick)) / l_Amplitude) + 1 );
                    l_PreviousTotalDamage /= l_NumberOfTicks;
                    l_Damage += l_PreviousTotalDamage;

                    l_PreviousTrauma->GetBase()->RefreshDuration();
                    l_PreviousTrauma->ChangeAmount(l_Damage);
                }
                else
                    l_Caster->CastCustomSpell(l_UnitTarget, eSpells::TraumaBleed, &l_Damage, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_trauma_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_trauma_AuraScript();
        }
};

/// Last Update 7.0.1
/// Overpower - 7384
class spell_warr_overpower : public SpellScriptLoader
{
    public:
        spell_warr_overpower() : SpellScriptLoader("spell_warr_overpower") { }

        class spell_warr_overpower_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_overpower_SpellScript);

            enum eSpells
            {
                OverpowerAura = 60503
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();

                l_Caster->RemoveAura(eSpells::OverpowerAura);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_overpower_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_overpower_SpellScript();
        }
};

/// Victorious - 32216
class spell_warr_spell_victorious : public SpellScriptLoader
{
    public:
        spell_warr_spell_victorious() : SpellScriptLoader("spell_warr_spell_victorious") { }

        class spell_warr_spell_victorious_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_spell_victorious_AuraScript);

            enum eSpells
            {
                ImpendingVictory = 202168
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                l_Player->RemoveSpellCooldown(eSpells::ImpendingVictory, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_spell_victorious_AuraScript::OnApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_spell_victorious_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Shadow Of The Colossus - 198807
class spell_warr_shadow_of_the_colossus : public SpellScriptLoader
{
    public:
        spell_warr_shadow_of_the_colossus() : SpellScriptLoader("spell_warr_shadow_of_the_colossus") { }

        class spell_warr_shadow_of_the_colossus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_shadow_of_the_colossus_AuraScript);

            enum eSpells
            {
                Charge          = 100,
                ColossusSmash   = 167105
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                SpellInfo const * l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_ProcSpellInfo || !l_Player)
                    return;

                if (l_ProcSpellInfo->Id == eSpells::Charge)
                    l_Player->RemoveSpellCooldown(eSpells::ColossusSmash, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warr_shadow_of_the_colossus_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_shadow_of_the_colossus_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Execute (Arms) - 163201 - Execute (Fury) - 5308 - execute (Death Row Arms) - 217955
/// Called For Death Sentence - 198500
class spell_warr_death_sentence : public SpellScriptLoader
{
    public:
        spell_warr_death_sentence() : SpellScriptLoader("spell_warr_death_sentence") { }

        class spell_warr_death_sentence_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_death_sentence_SpellScript);

            enum eSpells
            {
                DeathSentence   = 198500,
                Charge          = 40602,
                SenseDeath      = 200979
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                Spell* l_Spell = GetSpell();
                if (!l_Caster || !l_Spell || !l_Target || !l_Caster->HasAura(eSpells::DeathSentence))
                    return;

                if (!l_Caster->IsWithinMeleeRange(l_Target))
                    l_Caster->CastSpell(l_Target, eSpells::Charge, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_warr_death_sentence_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_death_sentence_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Corrupted Blood of Zakajz - 209567
class spell_warr_corrupted_blood_of_zakajz : public SpellScriptLoader
{
    public:
        spell_warr_corrupted_blood_of_zakajz() : SpellScriptLoader("spell_warr_corrupted_blood_of_zakajz") { }

        class spell_warr_corrupted_blood_of_zakajz_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_corrupted_blood_of_zakajz_AuraScript);

            enum eSpells
            {
                CorruptedBloodOfZakajzDot = 209569
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = p_ProcEventInfo.GetActor();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit*  l_UnitTarget = p_ProcEventInfo.GetActionTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_DotSpellInfo = sSpellMgr->GetSpellInfo(eSpells::CorruptedBloodOfZakajzDot);
                if (!l_Caster || !l_DamageInfo || !l_UnitTarget || !l_SpellInfo || !l_DotSpellInfo)
                    return;

                int32 l_Amount = l_DamageInfo->GetAmount();
                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Amount || !l_Player || (l_ProcSpellInfo && l_ProcSpellInfo->Id == l_DotSpellInfo->Id))
                    return;

                float l_DamagePart = l_SpellInfo->Effects[EFFECT_0].BasePoints / 100.0f;
                int32 l_Bp = l_DamagePart * l_DamageInfo->GetAmount();

                if (Aura* l_BleedAura = l_UnitTarget->GetAura(eSpells::CorruptedBloodOfZakajzDot, l_Caster->GetGUID()))
                {
                    uint32 l_RemainingTime = l_BleedAura->GetDuration();
                    uint32 l_MaxDuration = l_BleedAura->GetMaxDuration();
                    AuraEffect* l_AuraEffect = l_BleedAura->GetEffect(EFFECT_0);
                    if (!l_AuraEffect)
                        return;

                    uint32 l_Amplitude = l_AuraEffect->GetAmplitude();
                    uint32 l_RemainingTick = l_RemainingTime / l_Amplitude;
                    l_Bp += (l_AuraEffect->GetAmount() * l_RemainingTick);
                    if (l_RemainingTime < l_MaxDuration)
                    {
                        l_RemainingTime += l_Amplitude;
                        ++l_RemainingTick;
                        l_BleedAura->SetDuration(l_RemainingTime);
                    }
                    l_Bp /= l_RemainingTick;
                    l_AuraEffect->SetAmount(l_Bp);
                }
                else
                {
                    l_Bp = l_Bp / (l_DotSpellInfo->GetMaxDuration() / l_DotSpellInfo->Effects[EFFECT_0].Amplitude);
                    l_Player->CastCustomSpell(l_UnitTarget, eSpells::CorruptedBloodOfZakajzDot, &l_Bp, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warr_corrupted_blood_of_zakajz_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_corrupted_blood_of_zakajz_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Precise Strikes - 209492
class spell_warr_precise_strikes : public SpellScriptLoader
{
    public:
        spell_warr_precise_strikes() : SpellScriptLoader("spell_warr_precise_strikes") { }

        class spell_warr_precise_strikes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_precise_strikes_AuraScript);

            enum eSpells
            {
                ColossusSmash       = 167105,
                PreciseStrikesBuff  = 209493
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ColossusSmash)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                AuraEffect* l_AuraEffect = GetEffect(EFFECT_0);
                if (!l_Caster || !l_AuraEffect)
                    return;

                int32 l_Bp = l_AuraEffect->GetAmount();
                l_Caster->CastCustomSpell(l_Caster, eSpells::PreciseStrikesBuff, &l_Bp, &l_Bp, nullptr, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_precise_strikes_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warr_precise_strikes_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_precise_strikes_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Precise Strikes Artifact Trait - 248579
class spell_warr_precise_strikes_trait : public SpellScriptLoader
{
public:
    spell_warr_precise_strikes_trait() : SpellScriptLoader("spell_warr_precise_strikes_trait") { }

    class spell_warr_precise_strikes_trait_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_precise_strikes_trait_AuraScript);

        enum eSpells
        {
            ColossusSmash               = 167105,
            PreciseStrikesTraitProc     = 248195
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ColossusSmash)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            AuraEffect* l_PreciseStrikesAura = l_Caster->GetAuraEffect(m_scriptSpellId, 0);
            if (!l_PreciseStrikesAura)
                return;

            int32 l_Value = l_PreciseStrikesAura->GetAmount();
            l_Caster->CastCustomSpell(l_Caster, eSpells::PreciseStrikesTraitProc, &l_Value, NULL, NULL, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_precise_strikes_trait_AuraScript::HandleCheckProc);
            OnProc += AuraProcFn(spell_warr_precise_strikes_trait_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_precise_strikes_trait_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Precise Strikes (Buff) - 209493
class spell_warr_precise_strikes_buff : public SpellScriptLoader
{
    public:
        spell_warr_precise_strikes_buff() : SpellScriptLoader("spell_warr_precise_strikes_buff") { }

        class spell_warr_precise_strikes_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_precise_strikes_buff_AuraScript);

            enum eSpells
            {
                MortalStrike    = 12294,
                MortalStrike2   = 246820,
                Execute         = 163201,
                ExecuteOverride = 217955
            };

            std::vector<uint32> m_Spells =
            {
                eSpells::MortalStrike,
                eSpells::MortalStrike2,
                eSpells::Execute,
                eSpells::ExecuteOverride
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || std::find(m_Spells.begin(), m_Spells.end(), l_SpellInfo->Id) == m_Spells.end())
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                SetDuration(0);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_precise_strikes_buff_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warr_precise_strikes_buff_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_precise_strikes_buff_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Battle Trance - 213857
class spell_warr_battle_trance : public SpellScriptLoader
{
    public:
        spell_warr_battle_trance() : SpellScriptLoader("spell_warr_battle_trance") { }

        class spell_warr_battle_trance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_battle_trance_AuraScript);

            enum eSpells
            {
                RagingBlow          = 96103,
                BattleTranceBuff    = 213858
            };

            uint64 m_LastTargetGUID = 0;
            uint8 m_Counter = 0;

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster ||!l_DamageInfo)
                    return;

                Unit* l_Victim = l_DamageInfo->GetTarget();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Victim || !l_SpellInfo || l_SpellInfo->Id != eSpells::RagingBlow)
                    return;

                bool l_Reset = false; ///< keep better track of counter per target

                if (m_LastTargetGUID == l_Victim->GetGUID() || m_LastTargetGUID == 0) /// target still same and was hit twice
                {
                    if (++m_Counter >= 2)
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::BattleTranceBuff, true);
                        l_Reset = true;
                    }
                }
                else
                {
                    l_Caster->RemoveAurasDueToSpell(eSpells::BattleTranceBuff); ///< @wowhead "Using Raging Blow on a new target will cancel this effect."
                    l_Reset = true;
                }

                /// reset proc counter so that it doesn't proc every hit
                if (l_Reset)
                {
                    m_Counter = 0;
                    m_LastTargetGUID = 0;
                }
                m_LastTargetGUID = l_Victim->GetGUID();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warr_battle_trance_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_battle_trance_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Battle Cry - 1719
/// Called For Pain Train - 198765
class spell_warr_pain_train : public SpellScriptLoader
{
    public:
        spell_warr_pain_train() : SpellScriptLoader("spell_warr_pain_train") { }

        class spell_warr_pain_train_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_pain_train_SpellScript);

            enum eSpells
            {
                PainTrain   = 198765,
                CorruptedBloodOfZakajz = 209566,
                BattleCryDK = 198768,
                BattleCryDH = 198769,
                BattleCryDru = 198770,
                BattleCryHun = 198771,
                BattleCryMage = 198772,
                BattleCryMonk = 198773,
                BattleCryPal = 198774,
                BattleCryPri = 198775,
                BattleCryRog = 198776,
                BattleCrySha = 198777,
                BattleCryWarlock = 198778,
                BattleCryWarrior = 198780,
                GlyphOfTheWeaponmaster = 146974,
                Weaponmaster = 147367
            };

            std::map<uint8, uint32> m_BattleCrySpells =
            {
                { CLASS_DEATH_KNIGHT, eSpells::BattleCryDK },
                { CLASS_DEMON_HUNTER, eSpells::BattleCryDH },
                { CLASS_DRUID, eSpells::BattleCryDru },
                { CLASS_HUNTER, eSpells::BattleCryHun },
                { CLASS_MAGE, eSpells::BattleCryMage },
                { CLASS_MONK, eSpells::BattleCryMonk },
                { CLASS_PALADIN, eSpells::BattleCryPal },
                { CLASS_PRIEST, eSpells::BattleCryPri },
                { CLASS_ROGUE, eSpells::BattleCryRog },
                { CLASS_SHAMAN, eSpells::BattleCrySha },
                { CLASS_WARLOCK, eSpells::BattleCryWarlock },
                { CLASS_WARRIOR, eSpells::BattleCryWarrior }
            };

            void HandleOnTargets(std::list<WorldObject*>& p_TargetList)
            {
                Unit* l_Caster = GetCaster();
                Spell const* l_Spell = GetSpell();
                if (!l_Spell || !l_Caster || !l_Caster->HasAura(eSpells::PainTrain))
                    return;

                if (l_Spell->GetSpellValue(SPELLVALUE_BASE_POINT1) == 0)
                    return;

                for (WorldObject* l_Target : p_TargetList)
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    if (m_BattleCrySpells.find(l_UnitTarget->getClass()) != m_BattleCrySpells.end() && l_UnitTarget != l_Caster)
                        l_UnitTarget->CastSpell(l_UnitTarget, m_BattleCrySpells[l_UnitTarget->getClass()], true);
                }
            }

            void HandleAfterCast()
            {
                // Disabled due to the crash https://paste2.org/dHyI66n7
                /*if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::GlyphOfTheWeaponmaster))
                        l_Caster->CastSpell(l_Caster, eSpells::Weaponmaster, true);*/
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_pain_train_SpellScript::HandleOnTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                AfterCast += SpellCastFn(spell_warr_pain_train_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_pain_train_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Shattered Defenses - 209574
class spell_warr_shattered_defenses : public SpellScriptLoader
{
    public:
        spell_warr_shattered_defenses() : SpellScriptLoader("spell_warr_shattered_defenses") { }

        class spell_warr_shattered_defenses_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_shattered_defenses_AuraScript);

            enum eSpells
            {
                ColossusSmash = 167105
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ColossusSmash)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_shattered_defenses_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_shattered_defenses_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Shattered Defenses (Buff) - 209706
class spell_warr_shattered_defenses_buff : public SpellScriptLoader
{
    public:
        spell_warr_shattered_defenses_buff() : SpellScriptLoader("spell_warr_shattered_defenses_buff") { }

        class spell_warr_shattered_defenses_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_shattered_defenses_buff_AuraScript);

            enum eSpells
            {
                MortalStrike    = 12294,
                MortalStrike2   = 246820,
                Execute         = 163201,
                ExecuteOverride = 217955
            };

            std::vector<uint32> m_Spells =
            {
                eSpells::MortalStrike,
                eSpells::MortalStrike2,
                eSpells::Execute,
                eSpells::ExecuteOverride
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || std::find(m_Spells.begin(), m_Spells.end(), l_SpellInfo->Id) == m_Spells.end())
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                SetDuration(0);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_shattered_defenses_buff_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warr_shattered_defenses_buff_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_shattered_defenses_buff_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Spell Reflection - 216890
class spell_warr_spell_reflection_visual : public SpellScriptLoader
{
    public:
        spell_warr_spell_reflection_visual() : SpellScriptLoader("spell_warr_spell_reflection_visual") { }

        class spell_warr_spell_reflection_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_spell_reflection_visual_AuraScript);

            enum eSpells
            {
                SpellReflectionVisual = 186704
            };

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SpellReflectionVisual, true);
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SpellReflectionVisual);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_spell_reflection_visual_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_REFLECT_SPELLS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_spell_reflection_visual_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_REFLECT_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_spell_reflection_visual_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Echo Slam - 198821
class spell_warr_echo_slam: public SpellScriptLoader
{
    public:
        spell_warr_echo_slam() : SpellScriptLoader("spell_warr_echo_slam") { }

        class spell_warr_echo_slam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_echo_slam_AuraScript);

            enum eSpells
            {
                ColossusSmash = 167105,
                EchoSlam = 198827
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::ColossusSmash || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EchoSlam, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warr_echo_slam_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_echo_slam_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Echo Slam - 198827
class spell_warr_effect_echo_slam : public SpellScriptLoader
{
    public:
        spell_warr_effect_echo_slam() : SpellScriptLoader("spell_warr_effect_echo_slam") { }

        class spell_warr_effect_echo_slam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_effect_echo_slam_AuraScript);

            enum eSpell
            {
                EchoSlam = 198821
            };

            void HandleOnEffectPeriodic(AuraEffect const* aurEff)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = aurEff->GetBase();
                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::EchoSlam);

                if (!l_Target || !l_Aura || !l_SpellInfo || !l_Caster)
                    return;

                int32 l_Amount = CalculatePct(l_Target->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                SpellNonMeleeDamage l_DamageInfo = SpellNonMeleeDamage(l_Caster, l_Target, l_Amount, GetSpellInfo(), DamageEffectType::SPELL_DIRECT_DAMAGE, l_Aura->GetSpellXSpellVisualID());
                l_Caster->DealSpellDamage(l_DamageInfo, false);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_effect_echo_slam_AuraScript::HandleOnEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_effect_echo_slam_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Endless Rage - 198877
class spell_warr_endless_rage : public SpellScriptLoader
{
    public:
        spell_warr_endless_rage() : SpellScriptLoader("spell_warr_endless_rage") { }

        class spell_warr_endless_rage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_endless_rage_AuraScript);

            enum eSpells
            {
                RagingBlow  = 96103,
                Enrage      = 184362
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::RagingBlow)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::Enrage);
                if (!l_Aura)
                    return;

                l_Aura->RefreshDuration();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warr_endless_rage_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_endless_rage_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Bloodthirst - 23881
/// Called For Slaughterhouse - 199204
class spell_warr_slaughterhouse : public SpellScriptLoader
{
    public:
        spell_warr_slaughterhouse() : SpellScriptLoader("spell_warr_slaughterhouse") { }

        class spell_warr_slaughterhouse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_slaughterhouse_SpellScript);

            enum eSpells
            {
                Slaughterhouse = 199204
            };

            uint32 m_CooldownReduction = 0;

            void HandleOnHit(SpellEffIndex/* p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SlaughterhouseInfo = sSpellMgr->GetSpellInfo(eSpells::Slaughterhouse);
                if (!l_Caster || !l_SlaughterhouseInfo || !l_Caster->HasAura(eSpells::Slaughterhouse))
                    return;

                uint32 l_Damage = GetHitDamage();
                SetHitDamage(AddPct(l_Damage, l_SlaughterhouseInfo->Effects[EFFECT_0].BasePoints));
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SlaughterhouseInfo = sSpellMgr->GetSpellInfo(eSpells::Slaughterhouse);
                if (!l_Caster || !l_Target || l_Caster == l_Target || !l_SlaughterhouseInfo || !l_Caster->HasAura(eSpells::Slaughterhouse))
                    return;

                m_CooldownReduction = static_cast<uint32>(100.0f - l_Target->GetHealthPct()) / l_SlaughterhouseInfo->Effects[EFFECT_2].BasePoints;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!m_CooldownReduction || !l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                    l_Player->ReduceSpellCooldown(m_scriptSpellId, m_CooldownReduction * IN_MILLISECONDS);

                m_CooldownReduction = 0;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_slaughterhouse_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
                AfterHit += SpellHitFn(spell_warr_slaughterhouse_SpellScript::HandleAfterHit);
                AfterCast += SpellCastFn(spell_warr_slaughterhouse_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_slaughterhouse_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Taunt - 355
/// Called for Oppressor - 205800
class spell_warr_oppressor : public SpellScriptLoader
{
    public:
        spell_warr_oppressor() : SpellScriptLoader("spell_warr_oppressor") { }

        class spell_warr_oppressor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_oppressor_SpellScript);

            enum eSpells
            {
                Oppressor   = 205800,
                Intimidated = 206891
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::Oppressor))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Intimidated, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_oppressor_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_oppressor_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Furious Slash - 100130
class spell_warr_furious_slash : public SpellScriptLoader
{
    public:
        spell_warr_furious_slash() : SpellScriptLoader("spell_warr_furious_slash") { }

        class spell_warr_furious_slash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_furious_slash_SpellScript);

            enum eSpells
            {
                TasteForBlood   = 206333,
                FuriousSlash    = 231824
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::FuriousSlash))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::TasteForBlood, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_furious_slash_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_furious_slash_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Focused Rage - 207982, 204488
class spell_warr_focused_rage : public SpellScriptLoader
{
public:
    spell_warr_focused_rage() : SpellScriptLoader("spell_warr_focused_rage") { }

    class spell_warr_focused_rage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_focused_rage_AuraScript);

        void HandleCalcAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool&)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = nullptr;

            if (!l_Caster || !(l_Target = sObjectAccessor->FindUnit(l_Caster->GetTargetGUID())))
                return;
            if (l_Target->IsPlayer())
                p_Amount = p_AuraEffect->GetBaseAmount() * 0.50f; ///< Focused rage effect is reduced by 50% in PvP

            FixToolkitRemoveFromProt();
        }

        void FixToolkitRemoveFromProt()
        {
            Unit* l_Caster = GetCaster();

            if (Player* l_Player = l_Caster->ToPlayer())
            {
                if (l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_PROTECTION && m_scriptSpellId == 204488)
                {
                    uint32 l_SpellId = m_scriptSpellId;
                    l_Player->AddDelayedEvent([l_Player, l_SpellId]() -> void
                    {
                        l_Player->removeSpell(l_SpellId);
                    }, 1);
                }
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_focused_rage_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_focused_rage_AuraScript();
    }
};

/// Last Update: 7.1.5 23420
/// Called by Enrage - 184362
class spell_warr_enrage_pvp : public SpellScriptLoader
{
    public:
        spell_warr_enrage_pvp() : SpellScriptLoader("spell_warr_enrage_pvp") { }

        class spell_warr_enrage_pvp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_enrage_pvp_SpellScript);

            enum eSpells
            {
                GlyphOfBurningAnger = 115946,
                BurningAnger = 115993
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::GlyphOfBurningAnger))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::BurningAnger, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_enrage_pvp_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_enrage_pvp_SpellScript();
        }

        class spell_warr_enrage_pvp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_enrage_pvp_AuraScript);

            enum eSpells
            {
                BattleScars     = 200857,
                BattleScarsHeal = 200954
            };

            void HandlePvPModifier(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::BattleScars, EFFECT_0);
                if (l_AuraEffect)
                {
                    int32 l_Bp = l_AuraEffect->GetAmount();

                    l_Caster->AddDelayedEvent([l_Bp, l_Caster]() -> void
                    {
                        l_Caster->CastCustomSpell(l_Caster, eSpells::BattleScarsHeal, &l_Bp, nullptr, nullptr, true);
                    }, 1);
                }

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                p_Amount = p_AuraEffect->GetBaseAmount() * l_Aura->GetStackAmount() * 0.50f;   ///< Enrage bonus damage taken is reduced by 50% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_enrage_pvp_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_enrage_pvp_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Thunder Clap - 6343, Devastator - 236282 and Revenge - 6572
class spell_warr_shield_slam_cd_reset : public SpellScriptLoader
{
public:
    spell_warr_shield_slam_cd_reset() : SpellScriptLoader("spell_warr_shield_slam_cd_reset") { }

    class spell_warr_shield_slam_cd_reset_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_shield_slam_cd_reset_SpellScript);

        enum eSpells
        {
            ShieldSlam = 23922
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!roll_chance_i(30))
                return;

            if (Player* l_Player = l_Caster->ToPlayer())
            {
                if (l_Player->HasSpellCooldown(eSpells::ShieldSlam))
                    l_Player->RemoveSpellCooldown(eSpells::ShieldSlam, true);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_warr_shield_slam_cd_reset_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_shield_slam_cd_reset_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Thunder Clap - 6343
class spell_warr_thunder_clap: public SpellScriptLoader
{
    public:
        spell_warr_thunder_clap() : SpellScriptLoader("spell_warr_thunder_clap") { }

        class spell_warr_thunder_clap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_thunder_clap_SpellScript);

            enum eSpells
            {
                ThunderGodVigor     = 215176,
                DemoralizingShout   = 1160,
                ThunderstruckAura   = 199045,
                Thunderstruck       = 199042
            };

            void HandleDamage()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ThunderGodVigor, EFFECT_0))
                {
                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;
                    l_Player->ReduceSpellCooldown(eSpells::DemoralizingShout, l_AuraEffect->GetAmount() * IN_MILLISECONDS);
                }

                if (l_Caster->HasAura(eSpells::ThunderstruckAura))
                    l_Caster->CastSpell(l_Target, eSpells::Thunderstruck, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_thunder_clap_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_thunder_clap_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Devastate - 20243, 236282
/// Called for Kakushan's Stormscale Gauntlets -207841
class spell_warr_kakushans_stormscale_gauntlets : public SpellScriptLoader
{
    public:
        spell_warr_kakushans_stormscale_gauntlets() : SpellScriptLoader("spell_warr_kakushans_stormscale_gauntlets") { }

        class spell_warr_kakushans_stormscale_gauntlets_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_kakushans_stormscale_gauntlets_SpellScript);

            enum eSpells
            {
                KakushansStormscaleGauntlets = 207841
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::KakushansStormscaleGauntlets);
                if (!l_Caster || !l_SpellInfo || !l_Caster->HasAura(eSpells::KakushansStormscaleGauntlets))
                    return;

                l_Caster->CastSpell(l_Caster, l_SpellInfo->Effects[EFFECT_0].TriggerSpell, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_kakushans_stormscale_gauntlets_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_kakushans_stormscale_gauntlets_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Thunder Clap - 6343 - Shield Slam - 23922
/// Called for Kakushan's Stormscale Gauntlets -207841
class spell_warr_kakushans_stormscale_gauntlets_remove : public SpellScriptLoader
{
    public:
        spell_warr_kakushans_stormscale_gauntlets_remove() : SpellScriptLoader("spell_warr_kakushans_stormscale_gauntlets_remove") { }

        class spell_warr_kakushans_stormscale_gauntlets_remove_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_kakushans_stormscale_gauntlets_remove_SpellScript);

            enum eSpells
            {
                KakushansStormscaleGauntlets = 207844
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::KakushansStormscaleGauntlets);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_kakushans_stormscale_gauntlets_remove_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_kakushans_stormscale_gauntlets_remove_SpellScript();
        }
};

/// Last Update 7.1.0 - 22636
/// Odyns Champion Proc Check- 200872
class spell_warr_odyns_champion_proc : public SpellScriptLoader
{
    public:
        spell_warr_odyns_champion_proc() : SpellScriptLoader("spell_warr_odyns_champion_proc") { }

        class spell_warr_odyns_champion_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_odyns_champion_proc_AuraScript);

            enum eSpells
            {
                Rampage = 184709
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Rampage)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_odyns_champion_proc_AuraScript::HandleCheckProc);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_odyns_champion_proc_AuraScript();
        }
};

/// Last Update 7.3.2
/// Called by Mortal strike - 12294, 246820
class spell_warr_touch_of_zakajs : public SpellScriptLoader
{
    public:
        spell_warr_touch_of_zakajs() : SpellScriptLoader("spell_warr_touch_of_zakajs") { }

        class spell_warr_touch_of_zakajs_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_touch_of_zakajs_SpellScript);

            enum eSpells
            {
                TouchOfZakajs       = 209541,
                TouchOfZakajsHeal   = 209933
            };

            enum eArtifactPowers
            {
                TouchOfZakajsArtifact = 1151
            };

            void HandleAfterHit()
            {
                Spell* l_Spell = GetSpell();
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Spell || !l_Target || !l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (AuraEffect* l_Effect = l_Caster->GetAuraEffect(eSpells::TouchOfZakajs, EFFECT_0))
                {
                    bool crit = l_Spell->IsCritForTarget(l_Target);
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::TouchOfZakajsArtifact);
                    int32 l_Damage = (crit ? GetHitDamage() * 2 : GetHitDamage());
                    int32 l_Heal = CalculatePct(l_Damage, l_Rank * l_Effect->GetBaseAmount());

                    if (SpellInfo const* l_TouchOfZakajsHealSpellInfo = sSpellMgr->GetSpellInfo(eSpells::TouchOfZakajsHeal))
                        l_Caster->HealBySpell(l_Caster, l_TouchOfZakajsHealSpellInfo, l_Heal, crit);
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_warr_touch_of_zakajs_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_touch_of_zakajs_SpellScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Odyns Champion Effect - 200986
class spell_warr_odyns_champion_effect : public SpellScriptLoader
{
    public:
        spell_warr_odyns_champion_effect() : SpellScriptLoader("spell_warr_odyns_champion_effect") { }

        class spell_warr_odyns_champion_effect_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_odyns_champion_effect_AuraScript);

            enum eSpells
            {
                Whirlwind           = 199667,
                WhirlwindOffHand    = 44949
            };

            enum eReducedSpell
            {
                ShockWave               = 46968,
                Stormbolt               = 107570,
                DragonRoar              = 118000,
                BattleCry               = 1719,
                Avatar                  = 107574,
                BloodBath               = 12292,
                BladeStorm              = 46924,
                OdynsFury               = 205545,
                Charge                  = 100,
                IntimidatingShout       = 5246,
                Pummel                  = 6552,
                HeroicLeap              = 6544,
                HeroicThrow             = 57755
            };

            Guid128 m_LastCastGuid;

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id == eSpells::Whirlwind || l_SpellInfo->Id == eSpells::WhirlwindOffHand)
                    return false;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_Spell || (l_Spell->getTriggerCastFlags() & TRIGGERED_FULL_MASK) || l_Spell->GetCastGuid() == m_LastCastGuid)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->ReduceSpellCooldown(eReducedSpell::ShockWave, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::Stormbolt, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::DragonRoar, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::BattleCry, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::Avatar, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::BloodBath, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::BladeStorm, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::OdynsFury, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::Charge, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::IntimidatingShout, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::Pummel, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::HeroicLeap, 1 * TimeConstants::IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eReducedSpell::HeroicThrow, 1 * TimeConstants::IN_MILLISECONDS);

                if (SpellInfo const* l_Charge = sSpellMgr->GetSpellInfo(eReducedSpell::Charge))
                    l_Player->ReduceChargeCooldown(l_Charge->ChargeCategoryEntry, 1 * IN_MILLISECONDS);

                if (SpellInfo const* l_HeroicLeap = sSpellMgr->GetSpellInfo(eReducedSpell::HeroicLeap))
                    l_Player->ReduceChargeCooldown(l_HeroicLeap->ChargeCategoryEntry, 1 * IN_MILLISECONDS);

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_Spell)
                    return;

                m_LastCastGuid = l_Spell->GetCastGuid();
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_odyns_champion_effect_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warr_odyns_champion_effect_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_odyns_champion_effect_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Demoralizing Shout - 125565
class spell_warr_might_of_the_vrykul : public SpellScriptLoader
{
    public:
        spell_warr_might_of_the_vrykul() : SpellScriptLoader("spell_warr_might_of_the_vrykul") { }

        class spell_warr_might_of_the_vrykul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_might_of_the_vrykul_AuraScript);

            enum eSpells
            {
                MightOfTheVrykul        = 188778,
                MightOfTheVrykulProc    = 188783
            };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();

                if (l_Owner && l_Owner->HasAura(eSpells::MightOfTheVrykul))
                    l_Owner->CastSpell(l_Owner, eSpells::MightOfTheVrykulProc, true);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();

                if (l_Owner)
                    l_Owner->RemoveAurasDueToSpell(eSpells::MightOfTheVrykulProc);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_might_of_the_vrykul_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_might_of_the_vrykul_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_might_of_the_vrykul_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Ignore Pain - 190456, Revenge - 6572
class spell_warr_vengeance : public SpellScriptLoader
{
    public:
        spell_warr_vengeance() : SpellScriptLoader("spell_warr_vengeance") { }

        class spell_warr_vengeance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_vengeance_SpellScript);

            enum eSpells
            {
                Vengeance           = 202572,
                VenganceIgnorePain  = 202574,
                VengeanceRevenge    = 202573,
                IgnorePain          = 190456,
                Revenge             = 6572
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->HasAura(eSpells::Vengeance))
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::IgnorePain:
                    {
                        l_Caster->RemoveAurasDueToSpell(eSpells::VenganceIgnorePain);
                        l_Caster->CastSpell(l_Caster, eSpells::VengeanceRevenge, true);
                        break;
                    }
                    case eSpells::Revenge:
                        l_Caster->RemoveAurasDueToSpell(eSpells::VengeanceRevenge);
                        l_Caster->CastSpell(l_Caster, eSpells::VenganceIgnorePain, true);
                        break;
                    default:
                        return;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_vengeance_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_vengeance_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dragon Scales - 203576
class spell_warr_dragon_scales : public SpellScriptLoader
{
    public:
        spell_warr_dragon_scales() : SpellScriptLoader("spell_warr_dragon_scales") { }

        class spell_warr_dragon_scales_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_dragon_scales_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (p_EventInfo.GetHitMask() & ProcFlagsHit::PROC_HIT_BLOCK)
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_dragon_scales_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_dragon_scales_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Intervene - 147833
/// Called for Destiny Driver (item) - 137018
class spell_warr_destiny_driver : public SpellScriptLoader
{
    public:
        spell_warr_destiny_driver() : SpellScriptLoader("spell_warr_destiny_driver") { }

        class spell_warr_destiny_driver_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_destiny_driver_AuraScript);

            enum eSpells
            {
                DestinyDriverAura   = 215090,
                DestinyDriverShield = 215157
            };

            uint64 m_TargetGUID = 0;

            void HandleOnEffectProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DestinyDriverAura);
                if (!l_Caster || !l_Target || l_Caster != l_Target || !l_DamageInfo || !l_SpellInfo || !m_TargetGUID)
                    return;

                if (!l_Caster->HasAura(eSpells::DestinyDriverAura))
                    return;

                int32 l_Amount = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);

                Unit* l_RealTarget = sObjectAccessor->GetUnit(*l_Caster, m_TargetGUID);
                if (!l_RealTarget)
                    return;

                l_Caster->CastCustomSpell(l_RealTarget, eSpells::DestinyDriverShield, &l_Amount, nullptr, nullptr, true);

                l_Caster->CastCustomSpell(l_Caster, eSpells::DestinyDriverShield, &l_Amount, nullptr, nullptr, true);

                m_TargetGUID = 0;
            }

            void SetGuid(uint32 /*l_Index*/, uint64 l_GUID) override
            {
                m_TargetGUID = l_GUID;
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_destiny_driver_AuraScript::HandleOnEffectProc, EFFECT_2, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_destiny_driver_AuraScript();
        }
};

class PlayerScript_destiny_driver_hit : public PlayerScript
{
    public:
        PlayerScript_destiny_driver_hit() : PlayerScript("PlayerScript_destiny_driver_hit") { }

        enum eSpells
        {
            DestinyDriverAura   = 215090,
            Intervene           = 147833
        };

        void OnMeleeHitRedirect(Unit* p_NewVictim, Unit* p_OldVictim, Aura* p_RedirectAura)
        {
            if (!p_NewVictim || !p_OldVictim || p_NewVictim->getClass() != CLASS_WARRIOR)
                return;

            if (!p_NewVictim->HasAura(eSpells::DestinyDriverAura))
                return;

            Aura* l_Aura = p_NewVictim->GetAura(eSpells::Intervene, p_NewVictim->GetGUID());
            if (!l_Aura)
                return;

            l_Aura->SetScriptGuid(0, p_OldVictim->GetGUID());
        }
};

/// Very ugly hack to fix problems with rage cost
/// For example, if Warrior has Dauntless + Archavon's Heavy Hand, his Mortal Strike has 10 rage cost.
/// Client shows 10, but it's really between 10 and 11. So, if warrior has 10 rage (100 with the coefficient) he is not able to use Mortal Strike.
/// This fix sets Warrior's rage to the max possible value with the coefficient (100 -> 109, 390 -> 399 etc.)
class spell_warr_rage_fix : public PlayerScript
{
    public:
        spell_warr_rage_fix() : PlayerScript("spell_warr_rage_fix") {}

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player || p_Player->getClass() != CLASS_WARRIOR)
                return;

            m_Coeff = p_Player->GetPowerCoeff(POWER_RAGE);
        }

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After) override
        {
            if (p_After)
                return;

            if (!p_Player || p_Player->getClass() != CLASS_WARRIOR || p_Power != POWER_RAGE || p_Regen)
                return;

            /// Do it for arms only now
            if (p_Player->GetActiveSpecializationID() != SpecIndex::SPEC_WARRIOR_ARMS)
                return;

            if (m_Coeff != 10)
                return;

            if (p_NewValue % m_Coeff != 9)
                p_NewValue = (p_NewValue / m_Coeff) * m_Coeff + 9;
        }

    private:
        int32 m_Coeff = 1;

};

/// Last Update 7.3.5 build 26365
/// Called by Battle Cry - 1719
/// Called for Unrivaled Strength - 200860
class spell_warr_unrivaled_strength : public SpellScriptLoader
{
    public:
        spell_warr_unrivaled_strength() : SpellScriptLoader("spell_warr_unrivaled_strength") { }

        class spell_warr_unrivaled_strength_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_unrivaled_strength_AuraScript);

            enum eSpells
            {
                UnrivaledStrength        = 200860,
                UnrivaledStrengthProc    = 200977
            };

            enum eArtifactPowers
            {
                UnrivaledStrengthArtifact = 995
            };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::UnrivaledStrength))
                {
                    MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                    if (!l_Manager)
                        return;

                    ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::UnrivaledStrengthArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::UnrivaledStrengthArtifact) - 1);
                    if (!l_RankEntry)
                        return;

                    int32 l_Bp = l_RankEntry->AuraPointsOverride;
                    if (l_Bp > 0)
                        l_Player->CastCustomSpell(l_Player, eSpells::UnrivaledStrengthProc, &l_Bp, nullptr, nullptr, true);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();

                if (l_Owner)
                    l_Owner->RemoveAurasDueToSpell(eSpells::UnrivaledStrengthProc);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_unrivaled_strength_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_unrivaled_strength_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_unrivaled_strength_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Duel - 236276 - Duel - 237261
class spell_warr_duel : public SpellScriptLoader
{
    public:
        spell_warr_duel() : SpellScriptLoader("spell_warr_duel") { }

        class spell_warr_duel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_duel_SpellScript);

            enum eSpells
            {
                Duel = 236273
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Aura* l_Duel = l_Caster->GetAura(eSpells::Duel);
                if (!l_Duel)
                    return SPELL_FAILED_DONT_REPORT;

                uint64 l_DuelCasterGuid = l_Duel->GetCasterGUID();

                if (l_Target->HasAura(eSpells::Duel, l_DuelCasterGuid))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warr_duel_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_duel_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Rage of the Valarjar - 200845
class spell_warr_rage_of_the_valarjar : public SpellScriptLoader
{
    public:
        spell_warr_rage_of_the_valarjar() : SpellScriptLoader("spell_warr_rage_of_the_valarjar") { }

        class spell_warr_rage_of_the_valarjar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_rage_of_the_valarjar_AuraScript);

            enum eSpells
            {
                Rampage = 218617,
                Execute = 5308
            };

            std::vector<uint32> m_Spells =
            {
                eSpells::Rampage,
                eSpells::Execute
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || std::find(m_Spells.begin(), m_Spells.end(), l_SpellInfo->Id) == m_Spells.end())
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_rage_of_the_valarjar_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_rage_of_the_valarjar_AuraScript();
        }
};


/// Update to Legion 7.1.5 23420
/// Called by Execute 163201 for Ayalas Stone Heart 225947
class spell_warr_ayalas_stone_heart : public SpellScriptLoader
{
    public:
        spell_warr_ayalas_stone_heart() : SpellScriptLoader("spell_warr_ayalas_stone_heart") { }

        class spell_warr_ayalas_stone_heart_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_ayalas_stone_heart_SpellScript);

            enum eSpells
            {
                AyalasStoneHeart = 225947
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::AyalasStoneHeart);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_ayalas_stone_heart_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_ayalas_stone_heart_SpellScript();
        }
};

/// Update to Legion 7.3.5
/// Called by Battle Cry 1719
class spell_warr_argus_prot_2p : public SpellScriptLoader
{
public:
    spell_warr_argus_prot_2p() : SpellScriptLoader("spell_warr_argus_prot_2p") { }

    class spell_warr_argus_prot_2p_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_argus_prot_2p_SpellScript);

        enum eSpells
        {
            ShieldSlam    = 23922,
            WallOfIron    = 253428,
            warrProtT212P = 251882,
            WarrFuryT214P = 251881,
            Outrage       = 253385
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if (l_Player->HasAura(eSpells::warrProtT212P))
            {
                l_Player->CastSpell(l_Player, eSpells::WallOfIron);
                l_Player->RemoveSpellCooldown(eSpells::ShieldSlam, true);
            }

            if (l_Player->HasAura(eSpells::WarrFuryT214P))
                l_Player->CastSpell(l_Player, eSpells::Outrage);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_warr_argus_prot_2p_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_argus_prot_2p_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Intolerance - 203227
class spell_warr_intolerance : public SpellScriptLoader
{
    public:
        spell_warr_intolerance() : SpellScriptLoader("spell_warr_intolerance") { }

        class spell_warr_intolerance_AuraScript : public AuraScript
        {
            enum eArtifactPowers
            {
                IntoleranceArtifact = 101
            };

            PrepareAuraScript(spell_warr_intolerance_AuraScript);

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::IntoleranceArtifact);
                    p_Amount = l_Rank * GetSpellInfo()->Effects[p_AurEff->GetEffIndex()].BasePoints;
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_intolerance_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_MAX_POWER);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_intolerance_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Second Wind - 29838
class spell_warr_second_wind : public SpellScriptLoader
{
    public:
        spell_warr_second_wind() : SpellScriptLoader("spell_warr_second_wind") { }

        class spell_warr_second_wind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_second_wind_AuraScript);

            enum eSpells
            {
                SecondWindHeal  = 202147,
                SecondWindCD    = 202149
            };

            void HandleAfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (!l_Owner->HasAura(eSpells::SecondWindCD))
                    l_Owner->CastSpell(l_Owner, eSpells::SecondWindHeal, true);
            }

            void HandleAfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;
                l_Owner->RemoveAura(eSpells::SecondWindHeal);
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Aura* l_Aura = l_Owner->GetAura(eSpells::SecondWindCD))
                {
                    l_Aura->RefreshDuration();
                }
                else
                {
                    l_Owner->RemoveAura(eSpells::SecondWindHeal);
                    l_Owner->CastSpell(l_Owner, eSpells::SecondWindCD, true);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_second_wind_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_second_wind_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectProc += AuraEffectProcFn(spell_warr_second_wind_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_second_wind_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Second Wind - 202149
class spell_warr_second_wind_cd : public SpellScriptLoader
{
    public:
        spell_warr_second_wind_cd() : SpellScriptLoader("spell_warr_second_wind_cd") { }

        class spell_warr_second_wind_cd_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_second_wind_cd_AuraScript);

            enum eSpells
            {
                SecondWind      = 29838,
                SecondWindHeal  = 202147
            };

            void HandleAfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (l_Owner->HasAura(eSpells::SecondWind))
                    l_Owner->CastSpell(l_Owner, eSpells::SecondWindHeal, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_second_wind_cd_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_second_wind_cd_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Intimidating Shout - 5246
class spell_warr_intimidation_shout : public SpellScriptLoader
{
    public:
        spell_warr_intimidation_shout() : SpellScriptLoader("spell_warr_intimidation_shout") { }

        class spell_warr_intimidation_shout_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_intimidation_shout_AuraScript);

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                ApplyCustomFear(l_Owner);
            }

            void CalculateMaxDuration(int32& p_Duration)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->CanApplyPvPSpellModifiers())
                    p_Duration = 6 * IN_MILLISECONDS;
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_warr_intimidation_shout_AuraScript::CalculateMaxDuration);
                AfterEffectApply += AuraEffectApplyFn(spell_warr_intimidation_shout_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_FEAR_2, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            /// It's copied from AuraEffect::HandleModFear but without MoveFleeing.
            void ApplyCustomFear(Unit* p_Owner)
            {
                if (p_Owner->HasUnitState(UNIT_STATE_FLEEING))
                    return;

                p_Owner->AddUnitState(UNIT_STATE_FLEEING);

                if (!p_Owner->HasUnitState(UNIT_STATE_STUNNED | UNIT_STATE_CONFUSED))
                {
                    p_Owner->ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                    p_Owner->SendMeleeAttackStop();
                    // SendAutoRepeatCancel ?
                    p_Owner->SetTarget(0);

                    if (Player* player = p_Owner->ToPlayer())
                        if (!player->HasUnitState(UNIT_STATE_POSSESSED))
                            player->SetClientControl(player, false);

                    p_Owner->CastStop();
                }

                p_Owner->SendAddLossOfControl(GetTargetApplication(), Mechanics::MECHANIC_FEAR, LossOfControlType::TypeFear);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_intimidation_shout_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Uncontrolled Rage - 200856
/// Need to recalculate amounts because ArtifactPowerRankEntry has RankBasePoints=0.
class spell_warr_uncontrolled_rage : public SpellScriptLoader
{
    public:
        spell_warr_uncontrolled_rage() : SpellScriptLoader("spell_warr_uncontrolled_rage") { }

        class spell_warr_uncontrolled_rage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_uncontrolled_rage_AuraScript);

            enum eArtifactPowers
            {
                UncontrolledRageArtifact = 992
            };

            void CalculateAmount(AuraEffect const* p_AurEff, int32 & p_Amount, bool & /*canBeRecalculated*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                p_Amount = 50;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::UncontrolledRageArtifact);
                    p_Amount = l_Rank * 50;
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_uncontrolled_rage_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_uncontrolled_rage_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Gaze of the Val'kyr - 218836
class spell_warr_gaze_of_the_valkyr : public SpellScriptLoader
{
public:
    spell_warr_gaze_of_the_valkyr() : SpellScriptLoader("spell_warr_gaze_of_the_valkyr") { }

    class spell_warr_gaze_of_the_valkyr_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_gaze_of_the_valkyr_AuraScript);

        enum eSpells
        {
            Heal = 218835,
            Kill = 218834,
            Debuff = 218822
        };

        void HandleAfterRemove(AuraEffect const*, AuraEffectHandleModes)
        {
            Unit* l_Owner = GetUnitOwner();
            if (!l_Owner)
                return;

            if (GetDuration() == 0)
                l_Owner->CastSpell(l_Owner, eSpells::Kill, true);
            else
                l_Owner->CastSpell(l_Owner, eSpells::Heal, true);

            l_Owner->CastSpell(l_Owner, eSpells::Debuff, true);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_warr_gaze_of_the_valkyr_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_gaze_of_the_valkyr_AuraScript();
    }
};

/// Last Update: 7.1.5 23420
/// Called Raging Berserker - 200861
class spell_warr_raging_berserker : public SpellScriptLoader
{
    public:
        spell_warr_raging_berserker() : SpellScriptLoader("spell_warr_raging_berserker") { }

        class spell_warr_raging_berserker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_raging_berserker_AuraScript);

            enum eArtifactPowers
            {
                RagingBerserkerArtifact = 996
            };

            void HandleCalcAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Owner = GetUnitOwner();

                if (!l_SpellInfo || !l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::RagingBerserkerArtifact);
                    p_Amount = l_Rank * l_SpellInfo->Effects[0].BasePoints;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_raging_berserker_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_raging_berserker_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Thunder Crash - 188644
class spell_monk_thunder_crash : public SpellScriptLoader
{
    public:
        spell_monk_thunder_crash() : SpellScriptLoader("spell_monk_thunder_crash") { }

        class spell_monk_thunder_crash_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_thunder_crash_AuraScript);

            enum eArtifactPowers
            {
                ThunderCrashArtifact = 106
            };

            void HandleCalcAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Owner = GetUnitOwner();

                if (!l_SpellInfo || !l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::ThunderCrashArtifact);
                    amount = l_Rank * l_SpellInfo->Effects[0].BasePoints;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_thunder_crash_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_thunder_crash_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Thirst for Battle - 199203
class spell_warr_thirst_for_battle : public SpellScriptLoader
{
    public:
        spell_warr_thirst_for_battle() : SpellScriptLoader("spell_warr_thirst_for_battle") { }

        class spell_warr_thirst_for_battle_AuraScript : public AuraScript
        {
                PrepareAuraScript(spell_warr_thirst_for_battle_AuraScript);

                void HandleAfterApply(AuraEffect const*, AuraEffectHandleModes)
                {
                    Unit* l_Owner = GetUnitOwner();
                    if (!l_Owner)
                        return;

                    std::list<AuraEffect*> l_EffectList = l_Owner->GetAuraEffectsByMechanic(1 << Mechanics::MECHANIC_SNARE);

                    for (AuraEffect* l_Effect : l_EffectList)
                    {
                        Aura* l_Aura = l_Effect->GetBase();

                        if (l_Aura)
                            l_Aura->Remove();
                    }
                }

                void Register() override
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_warr_thirst_for_battle_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_SPEED_NOT_STACK, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_warr_thirst_for_battle_AuraScript();
            }

            class spell_warr_thirst_for_battle_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_warr_thirst_for_battle_SpellScript);

                SpellCastResult HandleCheckCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return SPELL_FAILED_ERROR;

                    return SPELL_CAST_OK;
                }

                void Register() override
                {
                    OnCheckCast += SpellCheckCastFn(spell_warr_thirst_for_battle_SpellScript::HandleCheckCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_warr_thirst_for_battle_SpellScript();
            }
};

/// Update to legion 7.1.5 build: 23420
/// Called By Death Wish (Honor Talent) 199261
class spell_warr_death_wish: public SpellScriptLoader
{
    public:
        spell_warr_death_wish() : SpellScriptLoader("spell_warr_death_wish") { }

        class spell_warr_death_wish_AuraScript : public AuraScript
        {
                PrepareAuraScript(spell_warr_death_wish_AuraScript);

                void HandleAfterApply(AuraEffect const* /* p_AuraEffect */ , AuraEffectHandleModes /* p_Modes */)
                {
                    Unit* l_Owner = GetUnitOwner();
                    Aura* l_Aura = GetAura();
                    if (!l_Owner || !l_Aura)
                        return;

                    SpellInfo const* l_SpellInfo = GetSpellInfo();
                    if (!l_SpellInfo)
                        return;

                    if (l_Aura->GetStackAmount() > l_SpellInfo->Effects[EFFECT_1].BasePoints)
                        l_Aura->DropStack();

                }

                void Register() override
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_warr_death_wish_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                }
            };

            AuraScript* GetAuraScript() const override
            {
                return new spell_warr_death_wish_AuraScript();
            }
};

/// Last Update 7.1.5 Build 23420
/// Called by Whirlwind - 1680, 199850
/// Called for Cleave - 188923
class spell_warr_cleave_aura : public SpellScriptLoader
{
    public:
        spell_warr_cleave_aura() : SpellScriptLoader("spell_warr_cleave_aura") { }

        class spell_warr_cleave_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_cleave_aura_SpellScript);

            enum eSpells
            {
                Whirlwind = 1680,
                Cleave = 188923
            };

            void HandleOnLaunch(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Cleave) = true;
                }, l_SpellInfo->Effects[p_EffIndex].MiscValue);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Cleave))
                    return;

                l_Caster->RemoveAura(eSpells::Cleave);
                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Cleave) = false;
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Whirlwind)
                    OnEffectLaunch += SpellEffectFn(spell_warr_cleave_aura_SpellScript::HandleOnLaunch, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
                else
                    AfterHit += SpellHitFn(spell_warr_cleave_aura_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_cleave_aura_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Shield Wall - 871, Demoralizing Shout - 1160, Shield Block - 2565, Last Stand - 12975, Spell Reflection - 23920, Ignore Pain - 190456
class spell_warr_neltharions_fury_triggered : public SpellScriptLoader
{
    public:
        spell_warr_neltharions_fury_triggered() : SpellScriptLoader("spell_warr_neltharions_fury_triggered") { }

        enum eSpells
        {
            NeltharionsFury = 203524
        };

        class spell_warr_neltharions_fury_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_neltharions_fury_triggered_SpellScript);

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Spell* l_ChanneledSpell = l_Caster->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL))
                {
                    if (l_ChanneledSpell->GetSpellInfo()->Id == eSpells::NeltharionsFury)
                    {
                        if (Spell* l_Spell = GetSpell())
                        {
                            l_Spell->setTriggerCastFlags(TriggerCastFlags(TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_IGNORE_AURA_INTERRUPT_FLAGS));
                        }
                    }
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_warr_neltharions_fury_triggered_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_neltharions_fury_triggered_SpellScript();
        }
};

/// Update to legion 7.1.5 build: 23420
/// Called By Ceann-Ar Rage 207779
class spell_warr_ceann_ar_rage : public SpellScriptLoader
{
    public:
        spell_warr_ceann_ar_rage() : SpellScriptLoader("spell_warr_ceann_ar_rage") { }

        class spell_warr_ceann_ar_rage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_ceann_ar_rage_AuraScript);

            enum eSpells
            {
                Enrage = 184362
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Enrage)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_ceann_ar_rage_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_ceann_ar_rage_AuraScript();
        }
};

/// Update to legion 7.1.5 build: 23420
/// Called By Enrage 184362
/// Called for Ceann-Ar Rage 207779 and Focus In Chaos 200871
class spell_warr_enrage_bonus : public SpellScriptLoader
{
    public:
        spell_warr_enrage_bonus() : SpellScriptLoader("spell_warr_enrage_bonus") { }

        class spell_warr_enrage_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_enrage_bonus_AuraScript);

            enum eSpells
            {
                CeannArRageAura     = 207779,
                CeannArRageBonus    = 207780,
                FocusInChaos        = 200871,
                FocusInChaosProc    = 200876
            };

            uint32 m_LastProcTime = 0;

            void HandleAfterEffectApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Modes */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if ((l_Caster->HasAura(eSpells::CeannArRageAura)) && ((getMSTime() - m_LastProcTime) > 200))   ///< Preventing multiple rage gain when whirliwind into rampage is cast on several targets
                {
                    l_Caster->CastSpell(l_Caster, eSpells::CeannArRageBonus, true);
                    m_LastProcTime = getMSTime();
                }

                if (l_Caster->HasAura(eSpells::FocusInChaos))
                    l_Caster->CastSpell(l_Caster, eSpells::FocusInChaosProc, true);

            }

            void HandleAfterEffectRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Modes */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::CeannArRageBonus);
                l_Caster->RemoveAurasDueToSpell(eSpells::FocusInChaosProc);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warr_enrage_bonus_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_MOD_MELEE_RANGED_HASTE_2, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warr_enrage_bonus_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_MOD_MELEE_RANGED_HASTE_2, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_enrage_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 - 26356
/// Called By Mannoroth's Bloodletting Manacles - 208908
class spell_mannoroths_bloodletting_manacles : public SpellScriptLoader
{
    public:
        spell_mannoroths_bloodletting_manacles() : SpellScriptLoader("spell_mannoroths_bloodletting_manacles") { }

        enum eSpells
        {
            MannorothsBloodlettingManacles      = 208908,
            MannorothsBloodlettingManaclesHeal  = 208909,
            IgnorePain                          = 190456
        };

        class spell_mannoroths_bloodletting_manacles_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mannoroths_bloodletting_manacles_AuraScript);

            Guid128 m_CastId;

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Owner = GetUnitOwner();
                if (l_Owner == nullptr || !l_Owner->IsPlayer())
                    return false;

                if (l_Owner->getClass() != CLASS_WARRIOR)
                    return false;

                if (m_CastId == p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid())
                    return false;

                if (!p_ProcEventInfo.GetDamageInfo() || !p_ProcEventInfo.GetDamageInfo()->GetSpell())
                    return false;

                SpellInfo const* l_triggerSpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_triggerSpellInfo)
                    return false;

                auto l_SpellPower = l_triggerSpellInfo->GetSpellPower(Powers::POWER_RAGE);
                if (l_SpellPower == nullptr)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Owner = GetUnitOwner();
                if (l_Owner == nullptr || !l_Owner->IsPlayer())
                    return;

                Player* l_Player = l_Owner->ToPlayer();

                if (!p_ProcEventInfo.GetDamageInfo())
                    return;

                SpellInfo const* l_triggerSpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_triggerSpellInfo)
                    return;

                auto l_SpellPower = l_triggerSpellInfo->GetSpellPower(Powers::POWER_RAGE);
                if (l_SpellPower == nullptr)
                    return;

                m_CastId = p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid();

                if (l_triggerSpellInfo->Id == eSpells::IgnorePain)
                {
                    int32 l_MaxPowerCost = l_SpellPower->Cost + l_SpellPower->CostAdditional;
                    int32 l_PctRagetoUse = l_Owner->m_SpellHelper.GetUint32(eSpellHelpers::IgnorePain);
                    m_Rage += CalculatePct(l_MaxPowerCost, l_PctRagetoUse) / l_Owner->GetPowerCoeff(POWER_RAGE);
                }
                else
                {
                    m_Rage += l_SpellPower->Cost / l_Owner->GetPowerCoeff(Powers::POWER_RAGE);
                }

                int32 l_Divider = GetAura()->GetEffect(SpellEffIndex::EFFECT_1)->GetAmount();
                int32 l_HealAmount = GetAura()->GetEffect(SpellEffIndex::EFFECT_0)->GetAmount();

                int32 l_Count = m_Rage / l_Divider;
                m_Rage = m_Rage % l_Divider;

                if (l_Count > 0)
                    l_Owner->CastCustomSpell(eSpells::MannorothsBloodlettingManaclesHeal, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Count * l_HealAmount, l_Owner, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mannoroths_bloodletting_manacles_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_mannoroths_bloodletting_manacles_AuraScript::OnProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
            }

        private:

            int32 m_Rage = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mannoroths_bloodletting_manacles_AuraScript();
        }
};

/// Update to legion 7.1.5 build: 23420
/// Called By War Machine - 215557
class spell_warr_war_machine : public SpellScriptLoader
{
    public:
        spell_warr_war_machine() : SpellScriptLoader("spell_warr_war_machine") { }

        class spell_warr_war_machine_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_war_machine_AuraScript);

            enum eSpells
            {
                WarMachineKillAura = 215562
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActor();

                if (!l_Caster || !l_Caster->IsPlayer() || !l_Target || !l_Caster->ToPlayer()->isHonorOrXPTarget(l_Target))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::WarMachineKillAura, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warr_war_machine_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_war_machine_AuraScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Overpower! - 119938
class spell_warr_overpower_proc : public SpellScriptLoader
{
    public:
        spell_warr_overpower_proc() : SpellScriptLoader("spell_warr_overpower_proc") { }

        class spell_warr_overpower_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_overpower_proc_AuraScript);

            enum eSpells
            {
                OverPower   = 7384
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id == eSpells::OverPower) /// must not make proc with auto-attacks, and may not proc itself
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_overpower_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_overpower_proc_AuraScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Shattered Defenses - 248580
class spell_warr_shattered_defenses_trait : public SpellScriptLoader
{
    public:
        spell_warr_shattered_defenses_trait() : SpellScriptLoader("spell_warr_shattered_defenses_trait") { }

        class spell_warr_shattered_defenses_trait_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_shattered_defenses_trait_AuraScript);

            enum eSpells
            {
                Warbreaker    = 209577,
                ColossusSmash = 167105
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id == eSpells::Warbreaker || l_SpellInfo->Id == eSpells::ColossusSmash)
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_shattered_defenses_trait_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_shattered_defenses_trait_AuraScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Ravager - 156287
class spell_warr_ravager_damage : public SpellScriptLoader
{
    public:
        spell_warr_ravager_damage() : SpellScriptLoader("spell_warr_ravager_damage") { }

        class spell_warr_ravager_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_ravager_damage_SpellScript);

            enum eSpells
            {
                RavagerGain   = 248439,
                TheGreatStormsEye = 248118,
                TornadosEye = 248145
            };

            void OnDamage()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RavagerGain, true);
                /// Legendary: The Great Storm's Eye
                if (l_Caster->HasAura(eSpells::TheGreatStormsEye))
                    l_Caster->CastSpell(l_Caster, eSpells::TornadosEye, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_ravager_damage_SpellScript::OnDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_ravager_damage_SpellScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Bladestorm - 50622
class spell_warr_bladestorm : public SpellScriptLoader
{
    public:
        spell_warr_bladestorm() : SpellScriptLoader("spell_warr_bladestorm") { }

        class spell_warr_bladestorm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_bladestorm_SpellScript);

            enum eSpells
            {
                TheGreatStormsEye = 248118,
                TornadosEye = 248142
            };

            void OnDamage()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// Legendary: The Great Storm's Eye
                if (l_Caster->HasAura(eSpells::TheGreatStormsEye))
                    l_Caster->CastSpell(l_Caster, eSpells::TornadosEye, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_bladestorm_SpellScript::OnDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_bladestorm_SpellScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by In For The Kill - 248621
class spell_warr_in_for_the_kill : public SpellScriptLoader
{
    public:
        spell_warr_in_for_the_kill() : SpellScriptLoader("spell_warr_in_for_the_kill") { }

        class spell_warr_in_for_the_kill_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_in_for_the_kill_AuraScript);

            enum eSpells
            {
                ColossusSmash = 167105,
                Warbreaker = 209577
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::ColossusSmash && l_SpellInfo->Id != eSpells::Warbreaker))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_in_for_the_kill_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_in_for_the_kill_AuraScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Executioner's Precision - 238147
class spell_warr_executioners_precision : public SpellScriptLoader
{
    public:
        spell_warr_executioners_precision() : SpellScriptLoader("spell_warr_executioners_precision") { }

        class spell_warr_executioners_precision_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_executioners_precision_AuraScript);

            enum eSpells
            {
                Execute         = 163201,
                ExecuteDeathRow = 217955
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::Execute && l_SpellInfo->Id != eSpells::ExecuteDeathRow))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_executioners_precision_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_executioners_precision_AuraScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Soul of the Slaughter - 238111
class spell_warr_soul_of_the_slaughter : public SpellScriptLoader
{
    public:
        spell_warr_soul_of_the_slaughter() : SpellScriptLoader("spell_warr_soul_of_the_slaughter") { }

        enum eSpells
        {
            Whirlwind           = 1680,
            WhirlwindTrigger    = 199658
        };

        class spell_warr_soul_of_the_slaughter_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_soul_of_the_slaughter_AuraScript);

            Guid128 m_CastId;

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr || l_Spell == nullptr)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return false;

                if (l_Player->getClass() != CLASS_WARRIOR)
                    return false;

                if (l_Spell->GetCastGuid() == m_CastId)
                    return false;

                if (l_SpellInfo->SpellPowers.empty())
                {
                    if (l_SpellInfo->Id == eSpells::WhirlwindTrigger)
                    {
                        l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Whirlwind);
                        if (!l_SpellInfo || l_SpellInfo->SpellPowers.empty())
                            return false;
                    }
                    else
                        return false;
                }

                int32 l_RageCost = 0;
                for (auto spellPower : l_SpellInfo->SpellPowers)
                {
                    if (spellPower->PowerType == POWER_RAGE)
                        l_RageCost += spellPower->Cost;
                }

                m_CastId = l_Spell->GetCastGuid();

                AuraEffect* l_AuraEffect = GetAura()->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return false;

                float chance = (l_RageCost / l_Caster->GetPowerCoeff(POWER_RAGE)) * (l_AuraEffect->GetAmount() / 10.0f);
                if (!roll_chance_f(chance))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_soul_of_the_slaughter_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_soul_of_the_slaughter_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Defensive Stance - 197690, 212520
class spell_warr_defensive_stance_bonus : public SpellScriptLoader
{
    public:
        spell_warr_defensive_stance_bonus() : SpellScriptLoader("spell_warr_defensive_stance_bonus") { }

        class spell_warr_defensive_stance_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_defensive_stance_bonus_SpellScript);

            enum eSpells
            {
                DefensiveStance = 197690
            };

            SpellCastResult CheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!UpdateCooldown(l_Caster))
                        return SPELL_FAILED_NOT_READY;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warr_defensive_stance_bonus_SpellScript::CheckCast);
            }

        private:
            bool UpdateCooldown(Unit* l_Caster)
            {
                int32 l_Cooldown = l_Caster->m_SpellHelper.GetUint64(eSpells::DefensiveStance);
                if (l_Cooldown <= time(NULL))
                {
                    l_Caster->m_SpellHelper.GetUint64(eSpells::DefensiveStance) = time(NULL) + 6;
                    return true;
                }

                return false;
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_defensive_stance_bonus_SpellScript();
        }

        class spell_warr_defensive_stance_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_defensive_stance_bonus_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 2.0f;   ///< Defensive Stance is reduced by 200% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_defensive_stance_bonus_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_defensive_stance_bonus_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_defensive_stance_bonus_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Odyn's Fury - 205545
/// Called for Death and Glory - 238148
class spell_warr_odyns_fury : public SpellScriptLoader
{
    public:
        spell_warr_odyns_fury() : SpellScriptLoader("spell_warr_odyns_fury") { }

        class spell_warr_odyns_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_odyns_fury_SpellScript);

            enum eSpells
            {
                DeathandGlory = 238148,
                HelyasScorn   = 243223,
                OdynsGlory    = 243228
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::DeathandGlory))
                    return;

                uint32 spellId = (urand(0, 1) ? eSpells::HelyasScorn : eSpells::OdynsGlory);
                l_Caster->CastSpell(l_Caster, spellId, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warr_odyns_fury_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_odyns_fury_SpellScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Bloodthirst - 23881
/// Called for Oathblood - 238112
class spell_warr_oathblood: public SpellScriptLoader
{
    public:
        spell_warr_oathblood() : SpellScriptLoader("spell_warr_oathblood") { }

        class spell_warr_oathblood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_oathblood_SpellScript);

            enum eSpells
            {
                Oathblood     = 238112,
                Bloodthirst   = 23881
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                AuraEffect const* l_AurEff = l_Caster->GetAuraEffect(eSpells::Oathblood, EFFECT_0);
                if (!l_AurEff)
                    return;

                if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Bloodthirst))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Bloodthirst) = false;
                    return;
                }

                if (roll_chance_i(l_AurEff->GetAmount()))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Bloodthirst) = true;
                    l_Caster->CastSpell(l_Target, eSpells::Bloodthirst, true);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_warr_oathblood_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_oathblood_SpellScript();
        }
};

/// Neltharion's Thunder - 238149
class spell_warr_neltharions_thunder : public SpellScriptLoader
{
    public:
        spell_warr_neltharions_thunder() : SpellScriptLoader("spell_warr_neltharions_thunder") { }

        class spell_warr_neltharions_thunder_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_neltharions_thunder_AuraScript);

            enum eSpells
            {
                ThunderClap = 6343
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();

                if (!l_SpellInfo || l_SpellInfo->Id != ThunderClap)  /// Proc only for Thunder Clap
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_neltharions_thunder_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_neltharions_thunder_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
///  Meat Cleaver - 85739
class spell_warr_meat_cleaver : public SpellScriptLoader
{
    public:
        spell_warr_meat_cleaver() : SpellScriptLoader("spell_warr_meat_cleaver") { }

        class spell_warr_meat_cleaver_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_meat_cleaver_AuraScript);

            void HandleCheckProc(AuraEffect const* aurEff, SpellModifier*& spellMod)
            {
                /// Add flag to use rampage.
                spellMod->mask[3] += 4096;
            }

            void Register() override
            {
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_warr_meat_cleaver_AuraScript::HandleCheckProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_meat_cleaver_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called By Berserking - 200951
class spell_warr_berserking : public SpellScriptLoader
{
    public:
        spell_warr_berserking() : SpellScriptLoader("spell_warr_berserking") { }

        class spell_warr_berserking_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_berserking_AuraScript);

            enum eSpells
            {
                Berserking = 200953
            };

            void HandlePeriodic(AuraEffect const* p_AurEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Berserking))
                    if (l_Aura->GetStackAmount() == 12)
                            l_Caster->RemoveAura(eSpells::Berserking);
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_warr_berserking_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_berserking_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called By Last Stand - 12975
class spell_warr_last_stand : public SpellScriptLoader
{
    public:
        spell_warr_last_stand() : SpellScriptLoader("spell_warr_last_stand") { }

        class spell_warr_last_stand_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_last_stand_SpellScript);

            enum eSpells
            {
                LastStand           = 12975,
                WillToSurviveAura   = 188683
            };

            enum eArtifactPower
            {
                WillToSurvive       = 98
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LastStand);
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_Percentage = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player && l_Player->HasAura(eSpells::WillToSurviveAura))
                {
                    SpellInfo const* l_WillToSurviveInfo = sSpellMgr->GetSpellInfo(eSpells::LastStand);
                    l_Percentage += l_WillToSurviveInfo->Effects[EFFECT_0].BasePoints;
                }

                int32 l_MaxHealth = l_Caster->GetMaxHealth(l_Caster);
                int32 l_Value = (l_MaxHealth * l_Percentage / 100);

                int32 l_CurrentHp = l_Caster->GetHealth();
                l_CurrentHp -= (l_CurrentHp * l_Percentage / 100);
                l_Value -= l_CurrentHp;

                if (l_Value > 0)
                    l_Caster->SetHealth(l_Caster->GetHealth() + l_Value);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_warr_last_stand_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_last_stand_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Revenge - 6572
class spell_warr_best_served_cold : public SpellScriptLoader
{
public:
    spell_warr_best_served_cold() : SpellScriptLoader("spell_warr_best_served_cold") { }

    class spell_warr_best_served_cold_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_best_served_cold_SpellScript);

        enum eSpells
        {
            BestServerCold = 202560
        };

        uint32 m_TargetsCount = 1;

        void FilterTarget(std::list<WorldObject*>& p_Targets)
        {
            const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BestServerCold);
            if (!l_SpellInfo)
                return;

            if (p_Targets.size() > 0)
                m_TargetsCount = p_Targets.size();

            if (m_TargetsCount > l_SpellInfo->Effects[EFFECT_1].BasePoints)
                m_TargetsCount = l_SpellInfo->Effects[EFFECT_1].BasePoints;
        }

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BestServerCold);
            if (!l_Caster || !l_SpellInfo)
                return;

            if (Unit* l_Target = GetHitUnit())
                if (l_Caster->HasAura(eSpells::BestServerCold))
                    SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), l_SpellInfo->Effects[EFFECT_0].BasePoints * m_TargetsCount));
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_best_served_cold_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            OnHit += SpellHitFn(spell_warr_best_served_cold_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_best_served_cold_SpellScript();
    }
};

/// Update to Legion 7.3.5 build: 25996
/// Called By Pulse of Battle 238076
class spell_warr_pusle_of_battle : public SpellScriptLoader
{
public:
    spell_warr_pusle_of_battle() : SpellScriptLoader("spell_warr_pusle_of_battle") { }

    class spell_warr_pusle_of_battle_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_pusle_of_battle_AuraScript);

        enum eSpells
        {
            RagingBlowMain = 96103,
            RagingBlowOff  = 85384
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEvent)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_ProcEvent.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            SpellInfo const * l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::RagingBlowMain && l_SpellInfo->Id != eSpells::RagingBlowOff))
                return false;

            return true;
        }

        void HandleOnProc(AuraEffect const* aurEff, ProcEventInfo& p_ProcInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->EnergizeBySpell(l_Caster, eSpells::RagingBlowMain, aurEff->GetAmount(), POWER_RAGE);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_pusle_of_battle_AuraScript::HandleCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_warr_pusle_of_battle_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_pusle_of_battle_AuraScript();
    }
};

/// Last Update 7.3.5 - 25996
/// Shield Bash - 198912
class spell_warr_shield_bash : public SpellScriptLoader
{
public:
    spell_warr_shield_bash() : SpellScriptLoader("spell_warr_shield_bash") { }

    class spell_warr_shield_bash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_shield_bash_SpellScript);

        enum eSpells
        {
            ShieldBash = 198912
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Caster || !l_Caster->IsPlayer() || !l_Target)
                return;

            if (l_Target->HasUnitState(UNIT_STATE_CASTING))
                l_Caster->ToPlayer()->RemoveSpellCooldown(eSpells::ShieldBash, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_warr_shield_bash_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_shield_bash_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Booming Voice - 202743
class spell_warr_booming_voice : public SpellScriptLoader
{
public:
    spell_warr_booming_voice() : SpellScriptLoader("spell_warr_booming_voice") { }

    class spell_warr_booming_voice_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_booming_voice_AuraScript);

        void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount *= (l_AuraEffect->GetEffIndex() == EFFECT_0 ? 0.50f : 0.60f);   ///< Booming Voice(rage regeneration) is reduced by 50% in PvP and Booming Voice(damage multiple) is reduced by 40% in PvP
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_booming_voice_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_booming_voice_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_booming_voice_AuraScript::HandlePvPModifier, EFFECT_2, SPELL_AURA_ADD_FLAT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_booming_voice_AuraScript();
    }
};

/// Last Update 7.3.5
/// Called by Bodyguard - 213871
class spell_warr_bodyguard : public SpellScriptLoader
{
public:
    spell_warr_bodyguard() : SpellScriptLoader("spell_warr_bodyguard") { }

    class spell_warr_bodyguard_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_bodyguard_AuraScript);

        enum eSpells
        {
            ShieldSlam = 23922
        };

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            l_Caster->ToPlayer()->RemoveSpellCooldown(eSpells::ShieldSlam, true);
        }

        void OnTick(AuraEffect const* p_AurEff)
        {
            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();
            if (!l_Aura || !l_Caster || !l_Target)
                return;

            if (l_Target->GetDistance2d(l_Caster) > p_AurEff->GetAmount())
                l_Aura->Remove();
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_warr_bodyguard_AuraScript::HandleOnProc);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_bodyguard_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_bodyguard_AuraScript();
    }
};

/// Last Update 7.3.5 build 25996
/// Item - Warrior T20 Arms 2P Bonus - 242298
class spell_warr_t20_arms_2p_bonus : public SpellScriptLoader
{
public:
    spell_warr_t20_arms_2p_bonus() : SpellScriptLoader("spell_warr_t20_arms_2p_bonus") { }

    class spell_warr_t20_arms_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_t20_arms_2p_bonus_AuraScript);

        enum eSpells
        {
            Bladestorm = 227847,
            Ravager = 152277,
            ColossusSmash = 167105
        };

        bool OnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::ColossusSmash)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            if (l_Caster->HasSpell(eSpells::Bladestorm))
                if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0))
                    l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::Bladestorm, l_AuraEffect->GetAmount() * 100);

            if (l_Caster->HasSpell(eSpells::Ravager))
                if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_1))
                    l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::Ravager, l_AuraEffect->GetAmount() * 100);
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_t20_arms_2p_bonus_AuraScript::OnCheckProc);
            OnProc += AuraProcFn(spell_warr_t20_arms_2p_bonus_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_t20_arms_2p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 25996
/// Victorious State - 32215
class spell_warr_spell_victorious_state : public SpellScriptLoader
{
    public:
        spell_warr_spell_victorious_state() : SpellScriptLoader("spell_warr_spell_victorious_state") { }

        class spell_warr_spell_victorious_state_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_spell_victorious_state_AuraScript);

            enum eSpells
            {
                ImpendingVictory = 202168
            };

            bool HandleCheckProc(ProcEventInfo& eventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr || l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_FURY)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_spell_victorious_state_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_spell_victorious_state_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Warrior T20 Fury 4P Bonus - 242301
class spell_warr_t20_fury_4p_bonus : public SpellScriptLoader
{
public:
    spell_warr_t20_fury_4p_bonus() : SpellScriptLoader("spell_warr_t20_fury_4p_bonus") { }

    class spell_warr_t20_fury_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_t20_fury_4p_bonus_AuraScript);

        enum eSpells
        {
            RagingBlow = 96103,
            InnerRage = 215573,
            BloodyRage50 = 242953,
            BloodyRage20 = 242952
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::RagingBlow)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (l_Caster->HasAura(eSpells::InnerRage))
                l_Caster->CastSpell(l_Caster, eSpells::BloodyRage50, true);
            else
                l_Caster->CastSpell(l_Caster, eSpells::BloodyRage20, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_t20_fury_4p_bonus_AuraScript::HandleCheckProc);
            OnProc += AuraProcFn(spell_warr_t20_fury_4p_bonus_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_t20_fury_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 25996
/// Bladestorm - 227847
class spell_warr_bladestorm_aura : public SpellScriptLoader
{
public:
    spell_warr_bladestorm_aura() : SpellScriptLoader("spell_warr_bladestorm_aura") { }

    class spell_warr_bladestorm_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_bladestorm_aura_AuraScript);

        enum eSpells
        {
            T20Arms4PBonus = 242297,
            MortalStrike = 246820,
            Bladestorm = 50622
        };

        void OnTick(AuraEffect const* p_AurEff)
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Bladestorm);
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_SpellInfo)
                return;

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T20Arms4PBonus, EFFECT_0))
            {
                if (m_Count >= l_AuraEffect->GetAmount())
                    return;

                float l_Radius = l_SpellInfo->Effects[EFFECT_0].CalcRadius();

                std::list<Unit*> l_Targets;
                l_Caster->GetAttackableUnitListInRange(l_Targets, l_Radius);
                l_Targets.remove_if([l_Caster, l_SpellInfo](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->ToUnit())
                        return true;

                    if (!l_Caster->IsValidAttackTarget(p_Target->ToUnit()) || l_Caster->IsFriendlyTo(p_Target->ToUnit()))
                        return true;

                    return false;
                });

                if (l_Targets.empty())
                    return;

                JadeCore::RandomResizeList(l_Targets, 1);

                l_Caster->CastSpell(l_Targets.front(), eSpells::MortalStrike, true);
                m_Count++;
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_bladestorm_aura_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }

    private:
        int32 m_Count = 0;
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_bladestorm_aura_AuraScript();
    }
};

/// Last Update 7.3.5 build 25996
/// Called by Warrior T20 Protection 4P Bonus - 242373
class spell_warr_t20_protection_4p_bonus : public SpellScriptLoader
{
public:
    spell_warr_t20_protection_4p_bonus() : SpellScriptLoader("spell_warr_t20_protection_4p_bonus") { }

    class spell_warr_t20_protection_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_t20_protection_4p_bonus_AuraScript);

        enum eSpells
        {
            ShieldSlam = 23922,
            BerserkerRage = 18499
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ShieldSlam)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
        {
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer() || !l_SpellInfo)
                return;

            l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::BerserkerRage, uint32((l_SpellInfo->Effects[EFFECT_0].BasePoints / 10) * IN_MILLISECONDS));
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_t20_protection_4p_bonus_AuraScript::HandleCheckProc);
            OnProc += AuraProcFn(spell_warr_t20_protection_4p_bonus_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_t20_protection_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 - 26365
/// Demoralizing Shout - 1160
class spell_warr_demoralizing_shout : public SpellScriptLoader
{
    public:
        spell_warr_demoralizing_shout() : SpellScriptLoader("spell_warr_demoralizing_shout") { }

        enum eSpells
        {
            MoraleKiller = 199023
        };

        class spell_warr_demoralizing_shout_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_demoralizing_shout_AuraScript);

            bool Load() override
            {
                Unit* l_Caster = GetCaster();
                m_MoraleKiller = (l_Caster != nullptr && l_Caster->HasAura(eSpells::MoraleKiller));

                return true;
            }

            void HandleCalculateAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                if (!m_MoraleKiller)
                    p_Amount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_demoralizing_shout_AuraScript::HandleCalculateAmount, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }

        private:

            bool m_MoraleKiller = false;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_demoralizing_shout_AuraScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Demoralizing Shout - 125565
class spell_warr_demoralizing_shout_absorb : public SpellScriptLoader
{
    public:
        spell_warr_demoralizing_shout_absorb() : SpellScriptLoader("spell_warr_demoralizing_shout_absorb") { }

        enum eSpells
        {
            DemoralizingShout   = 1160,
            MoraleKiller        = 199023
        };

        class spell_warr_demoralizing_shout_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_demoralizing_shout_absorb_AuraScript);

            bool Load() override
            {
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DemoralizingShout))
                    m_AbsorbPct = abs(l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints);

                return true;
            }

            void HandleCalculateAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                p_Amount = -1;
            }

            void HandleOnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();

                if (m_AbsorbPct > 0 && !(l_Caster && l_Caster->HasAura(eSpells::MoraleKiller)))
                    p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), m_AbsorbPct);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_demoralizing_shout_absorb_AuraScript::HandleCalculateAmount, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_warr_demoralizing_shout_absorb_AuraScript::HandleOnAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
            }

        private:

            int32 m_AbsorbPct = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_demoralizing_shout_absorb_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Thirst For Battle - 199202
class spell_warr_thirst_for_battle_proc : public SpellScriptLoader
{
    public:
        spell_warr_thirst_for_battle_proc() : SpellScriptLoader("spell_warr_thirst_for_battle_proc") { }

        class spell_warr_thirst_for_battle_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_thirst_for_battle_proc_AuraScript);

            enum eSpells
            {
                Bloodthirst     = 23881
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Bloodthirst)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_thirst_for_battle_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_thirst_for_battle_proc_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by 251878 - Warrior T21 Arms 2P Bonus
class spell_warr_arms_argus_2p : public SpellScriptLoader
{
public:
    spell_warr_arms_argus_2p() : SpellScriptLoader("spell_warr_arms_argus_2p") { }

    class spell_warr_arms_argus_2p_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_arms_argus_2p_AuraScript);

        enum eSpells
        {
            ColossusSmash = 167105
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                if (l_SpellInfo->Id == eSpells::ColossusSmash)
                    return true;

            return false;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_arms_argus_2p_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_arms_argus_2p_AuraScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by 251879 - Warrior T21 Arms 4P Bonus
class spell_warr_arms_argus_4p : public SpellScriptLoader
{
public:
    spell_warr_arms_argus_4p() : SpellScriptLoader("spell_warr_arms_argus_4p") { }

    class spell_warr_arms_argus_4p_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_arms_argus_4p_AuraScript);

        enum eSpells
        {
            MortalStrike = 12294
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                if (l_SpellInfo->Id == eSpells::MortalStrike)
                    return true;

            return false;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_arms_argus_4p_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_arms_argus_4p_AuraScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by Slam - 1464
class spell_warr_weighted_blade_slam : public SpellScriptLoader
{
public:
    spell_warr_weighted_blade_slam() : SpellScriptLoader("spell_warr_weighted_blade_slam") { }

    class spell_warr_weighted_blade_slam_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_weighted_blade_slam_SpellScript);

        enum eSpells
        {
            WeightedBlade = 253383
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::WeightedBlade);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_warr_weighted_blade_slam_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_weighted_blade_slam_SpellScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by Whirlwind - 1680
class spell_warr_weighted_blade_whirlwind : public SpellScriptLoader
{
public:
    spell_warr_weighted_blade_whirlwind() : SpellScriptLoader("spell_warr_weighted_blade_whirlwind") { }

    class spell_warr_weighted_blade_whirlwind_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_weighted_blade_whirlwind_SpellScript);

        enum eSpells
        {
            WeightedBlade = 253383
        };

        void HandleOnCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (Aura* l_Aura = l_Caster->GetAura(eSpells::WeightedBlade))
            {
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightedBlade) = l_Aura->GetStackAmount();
                l_Caster->RemoveAura(eSpells::WeightedBlade);
            }
            else
            {
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightedBlade) = 0;
            }
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_warr_weighted_blade_whirlwind_SpellScript::HandleOnCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_weighted_blade_whirlwind_SpellScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by Whirlwind - 199658 - 199850
class spell_warr_argus_arms_4p : public SpellScriptLoader
{
public:
    spell_warr_argus_arms_4p() : SpellScriptLoader("spell_warr_argus_arms_4p") { }

    class spell_warr_argus_arms_4p_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_argus_arms_4p_SpellScript);

        enum eSpells
        {
            WeightedBlade = 253383
        };

        void HandleDamage(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightedBlade) == 0)
                return;

            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WeightedBlade))
            {
                int32 l_Amount = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                int32 l_Damage = GetHitDamage();
                AddPct(l_Damage, l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::WeightedBlade) * l_Amount);
                SetHitDamage(l_Damage);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_warr_argus_arms_4p_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
            OnEffectHitTarget += SpellEffectFn(spell_warr_argus_arms_4p_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_argus_arms_4p_SpellScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by 251883
class spell_warr_argus_prot_4p : public SpellScriptLoader
{
public:
    spell_warr_argus_prot_4p() : SpellScriptLoader("spell_warr_argus_prot_4p") { }

    class spell_warr_argus_prot_4p_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_argus_prot_4p_AuraScript);

        enum eSpells
        {
            IgnorePain   = 190456
        };

        void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
        {
            if (!(p_ProcEventInfo.GetHitMask() & PROC_EX_BLOCK))
                return;

            Unit* l_Caster = GetCaster();
            AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::IgnorePain, EFFECT_0);
            if (!l_Caster || !l_AuraEffect)
                return;

            int32 l_ShieldAmount = l_AuraEffect->GetAmount();
            int32 l_MultAmount = p_AurEff->GetAmount();
            uint32 l_AttackPower = GetCaster()->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
            l_AuraEffect->SetAmount(l_ShieldAmount + l_AttackPower * l_MultAmount);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_warr_argus_prot_4p_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_argus_prot_4p_AuraScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by 251880
class spell_warr_argus_fury_2p : public SpellScriptLoader
{
public:
    spell_warr_argus_fury_2p() : SpellScriptLoader("spell_warr_argus_fury_2p") { }

    class spell_warr_argus_fury_2p_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_argus_fury_2p_AuraScript);

        enum eSpells
        {
            LastRampageHit     = 201363,
            PreciseStrikesBuff = 209493,
            Slaughter          = 253384
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::LastRampageHit)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

            if (!l_DamageInfo)
                return;

            Unit* l_Caster = GetCaster();
            Unit* l_Target = l_DamageInfo->GetTarget();
            if (!l_Caster || !l_Target)
                return;

            std::map<uint64, uint64>& m_Helper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::RampageTarget];
            auto l_Itr = m_Helper.find(l_Target->GetGUID());
            if (l_Itr == m_Helper.end())
                return;

            l_Caster->CastSpell(l_Target, eSpells::Slaughter);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_argus_fury_2p_AuraScript::HandleCheckProc);
            OnProc += AuraProcFn(spell_warr_argus_fury_2p_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_argus_fury_2p_AuraScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by Rampage 184367
class spell_warr_rampage : public SpellScriptLoader
{
public:
    spell_warr_rampage() : SpellScriptLoader("spell_warr_rampage") { }

    class spell_warr_rampage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_rampage_SpellScript);

        enum eSpells
        {
            WeightedBlade = 253383
        };

        void HandleBeforeCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Caster || !l_Target)
                return;

            l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::RampageTarget].clear();
        }

        void Register() override
        {
            BeforeCast += SpellCastFn(spell_warr_rampage_SpellScript::HandleBeforeCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warr_rampage_SpellScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by Slaughter - 253384
class spell_war_slaughter : public SpellScriptLoader
{
public:
    spell_war_slaughter() : SpellScriptLoader("spell_war_slaughter") { }

    class spell_war_slaughter_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_war_slaughter_AuraScript);

        enum eSpells
        {
            warrFuryT212P = 251880
        };

        void HandleOnEffectCalcAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetUnitOwner();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::warrFuryT212P);
            if (l_Caster == nullptr || l_Target == nullptr || l_SpellInfo == nullptr)
                return;

            std::map<uint64, uint64>& m_Helper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::RampageTarget];
            auto l_Itr = m_Helper.find(l_Target->GetGUID());
            if (l_Itr == m_Helper.end())
                return;

            int32 l_pct = l_SpellInfo->Effects[EFFECT_0].BasePoints;
            int32 l_Damage = l_Itr->second;
            int32 l_Amount = l_Damage * l_pct / 100;

            p_Amount = l_Amount;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_war_slaughter_AuraScript::HandleOnEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_war_slaughter_AuraScript();
    }
};

/// Last Update 7.3.0 - Build 24920
/// Opportunity Strikes - 203179
class spell_warr_opportunity_strikes : public SpellScriptLoader
{
    public:
        spell_warr_opportunity_strikes() : SpellScriptLoader("spell_warr_opportunity_strikes")
        {}

        class spell_warr_opportunity_strikes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_opportunity_strikes_AuraScript);

            void HandleProcChance(float & p_Chance, ProcEventInfo & p_Proc)
            {
                /// Opportunity strikes proc chance is scaled on the missing life of the target (100% hp is 0% proc chance, 0% hp is 60% proc chance)
                if (Unit* l_Victim = p_Proc.GetActionTarget())
                    p_Chance -= p_Chance * (double(l_Victim->GetHealth()) / double(l_Victim->GetMaxHealth()));
            }

            void Register() override
            {
                DoCalcProcChance += AuraProcChanceFn(spell_warr_opportunity_strikes_AuraScript::HandleProcChance);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_opportunity_strikes_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_warrior_spell_scripts()
{
    new spell_warr_war_machine();
    new spell_warr_thirst_for_battle();
    new spell_monk_thunder_crash();
    new spell_warr_raging_berserker();
    new spell_warr_gaze_of_the_valkyr();
    new spell_warr_duel();
    new spell_warr_unrivaled_strength();
    new spell_warr_vengeance();
    new spell_warr_might_of_the_vrykul();
    new spell_warr_touch_of_zakajs();
    new spell_warr_shield_slam_cd_reset();
    new spell_warr_focused_rage();
    new spell_warr_shattered_defenses();
    new spell_warr_shattered_defenses_buff();
    new spell_warr_pain_train();
    new spell_warr_battle_trance();
    new spell_warr_precise_strikes();
    new spell_warr_precise_strikes_buff();
    new spell_warr_corrupted_blood_of_zakajz();
    new spell_warr_death_sentence();
    new spell_warr_shadow_of_the_colossus();
    new spell_warr_spell_victorious();
    new spell_warr_overpower();
    new spell_warr_warbreaker();
    new spell_warr_shield_slam();
    new spell_warr_blood_hunt();
    new spell_warr_mastodonte();
    new spell_warr_whirlwind_damage();
    new spell_warr_whirliwind();
    new spell_warr_ultimatum();
    new spell_warr_impending_victory();
    new spell_warr_inspiring_presence();
    new spell_warr_ignore_pain();
    new spell_warr_devastate();
    new spell_warr_intercept();
    new spell_warr_massacre();
    new spell_warr_frenzy();
    new spell_warr_cleave();
    new spell_warr_revenge();
    new spell_warr_glyph_of_crow_feast();
    new spell_warr_unyielding_strikes();
    new spell_warr_ravager();
    new spell_warr_shield_block();
    new spell_warr_storm_bolt();
    new spell_warr_colossus_smash();
    new spell_warr_dragon_roar();
    new spell_warr_staggering_shout();
    new spell_warr_berzerker_rage();
    new spell_warr_enrage();
    new spell_warr_devaste();
    new spell_warr_mortal_strike();
    new spell_warr_commanding_shout();
    new spell_warr_heroic_leap_damage();
    new spell_warr_heroic_leap();
    new spell_warr_shockwave();
    new spell_warr_bloodthirst();
    new spell_warr_victory_rush();
    new spell_warr_charge();
    new spell_warr_shield_wall();
    new spell_warr_spell_reflection();
    new spell_warr_intervene();
    new spell_warr_anger_management();
    new spell_warr_execute();
    new spell_warr_execute_default();
    new spell_warr_blood_bath();
    new spell_warr_single_minded_fury();
    new spell_warr_weaponmaster();
    new spell_warr_trauma();
    new spell_warr_spell_reflection_visual();
    new spell_warr_furious_slash();
    new spell_warr_enrage_pvp();
    new spell_warr_thunder_clap();
    new spell_warr_kakushans_stormscale_gauntlets();
    new spell_warr_kakushans_stormscale_gauntlets_remove();
    new spell_warr_tactician();
    new spell_warr_odyns_champion_proc();
    new spell_warr_odyns_champion_effect();
    new spell_warr_ayalas_stone_heart();
    new spell_warr_dragon_scales();
    new spell_warr_rage_of_the_valarjar();
    new spell_warr_intolerance();
    new spell_warr_second_wind();
    new spell_warr_second_wind_cd();
    new spell_warr_intimidation_shout();
    new spell_warr_uncontrolled_rage();
    new spell_warr_cleave_aura();
    new spell_warr_neltharions_fury_triggered();
    new spell_warr_overpower_proc();
    new spell_warr_shattered_defenses_trait();
    new spell_warr_ravager_damage();
    new spell_warr_bladestorm();
    new spell_warr_in_for_the_kill();
    new spell_warr_tactician_effect();
    new spell_warr_executioners_precision();
    new spell_warr_soul_of_the_slaughter();
    new spell_warr_odyns_fury();
    new spell_warr_oathblood();
    new spell_warr_neltharions_thunder();
    new spell_warr_meat_cleaver();
    new spell_warr_berserking();
    new spell_warr_last_stand();
    new spell_warr_best_served_cold();
    new spell_warr_pusle_of_battle();
    new spell_warr_precise_strikes_trait();
    new spell_warr_t20_arms_2p_bonus();
    new spell_warr_spell_victorious_state();
    new spell_warr_t20_fury_4p_bonus();
    new spell_warr_bladestorm_aura();
    new spell_warr_t20_protection_4p_bonus();
    new spell_warr_demoralizing_shout();
    new spell_warr_demoralizing_shout_absorb();
    new spell_warr_arms_argus_2p();
    new spell_warr_arms_argus_4p();
    new spell_warr_weighted_blade_slam();
    new spell_warr_weighted_blade_whirlwind();
    new spell_warr_argus_prot_2p();
    new spell_warr_argus_arms_4p();
    new spell_warr_shield_block_aura();
    new spell_warr_argus_prot_4p();
    new spell_warr_argus_fury_2p();
    new spell_warr_rampage();
    new spell_war_slaughter();
    new spell_warr_opportunity_strikes();

    ///Honor Talents
    new spell_warr_echo_slam();
    new spell_warr_effect_echo_slam();
    new spell_warr_endless_rage();
    new spell_warr_slaughterhouse();
    new spell_warr_oppressor();
    new spell_warr_death_wish();
    new spell_warr_dragon_charge();
    new spell_warr_shield_bash();
    new spell_warr_bodyguard();
    new spell_warr_thirst_for_battle_proc();

    /// Legendary Items
    new spell_warr_destiny_driver();
    new spell_warr_ceann_ar_rage();
    new spell_warr_enrage_bonus();
    new spell_mannoroths_bloodletting_manacles();

    /// Playerscripts
    new PlayerScript_destiny_driver_hit();
    new spell_warr_rage_fix();

    /// PvP Modifiers
    new spell_warr_defensive_stance_bonus();
    new spell_warr_booming_voice();
}
#endif
