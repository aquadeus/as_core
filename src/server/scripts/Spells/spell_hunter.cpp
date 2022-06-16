////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_HUNTER, SPELLFAMILY_PET and SPELLFAMILY_GENERIC spells used by hunter players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_hun_".
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "spell_generic.hpp"
#include "HelperDefines.h"

/// Lesser Proportion - 57894
class spell_hun_lesser_proportion : public SpellScriptLoader
{
    public:
        spell_hun_lesser_proportion() : SpellScriptLoader("spell_hun_lesser_proportion") { }

        class spell_hun_lesser_proportion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_lesser_proportion_AuraScript);

            enum eSpells
            {
                GlyphOfLesserProportion = 57870
            };

            void OnUpdate(uint32, AuraEffect* p_AurEff)
            {
                if (!GetUnitOwner())
                    return;

                if (Pet* l_Pet = GetUnitOwner()->ToPet())
                {
                    if (Unit* l_Owner = l_Pet->GetOwner())
                    {
                        if (!l_Owner->HasAura(eSpells::GlyphOfLesserProportion))
                            p_AurEff->GetBase()->Remove();
                    }
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_hun_lesser_proportion_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_MOD_SCALE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_lesser_proportion_AuraScript();
        }
};

/// Glyph of Lesser Proportion - 57870
class spell_hun_glyph_of_lesser_proportion : public SpellScriptLoader
{
    public:
        spell_hun_glyph_of_lesser_proportion() : SpellScriptLoader("spell_hun_glyph_of_lesser_proportion") { }

        class spell_hun_glyph_of_lesser_proportion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_glyph_of_lesser_proportion_AuraScript);

            enum eSpells
            {
                LesserProportion = 57894
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                        l_Player->CastSpell(l_Pet, eSpells::LesserProportion, true);
                }
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (!GetTarget())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                        l_Pet->RemoveAura(eSpells::LesserProportion);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_glyph_of_lesser_proportion_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_glyph_of_lesser_proportion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_glyph_of_lesser_proportion_AuraScript();
        }
};

/// Fetch (Glyph) - 125050
class spell_hun_fetch_glyph : public SpellScriptLoader
{
    public:
        spell_hun_fetch_glyph() : SpellScriptLoader("spell_hun_fetch_glyph") { }

        class spell_hun_fetch_glyph_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_fetch_glyph_SpellScript);

            void HandleScript(SpellEffIndex)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Target->isAlive())
                            return;

                        if (Pet* l_Pet = l_Player->GetPet())
                        {
                            l_Pet->GetMotionMaster()->MoveCharge(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ());
                            l_Player->SendLoot(l_Target->GetGUID(), LootType::LOOT_CORPSE, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_fetch_glyph_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_fetch_glyph_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Thunderstomp - 63900
class spell_hun_thunderstomp : public SpellScriptLoader
{
    public:
        spell_hun_thunderstomp() : SpellScriptLoader("spell_hun_thunderstomp") { }

        class spell_hun_thunderstomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_thunderstomp_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetCaster()->GetOwner();
                Unit* l_Target = GetHitUnit();

                if (l_Owner == nullptr || l_Target == nullptr)
                    return;

                int32 l_Damage = (int32)(l_Owner->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 0.250f);

                l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);
                l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_thunderstomp_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_thunderstomp_SpellScript();
        }
};

/// Burrow Attack  - 95714
class spell_hun_burrow_attack : public SpellScriptLoader
{
    public:
        spell_hun_burrow_attack() : SpellScriptLoader("spell_hun_burrow_attack") { }

        class spell_hun_burrow_attack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_burrow_attack_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                SetHitDamage((int32)(GetHitDamage() + ((GetCaster()->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 0.40f) * 0.288) * 8));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_burrow_attack_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_burrow_attack_SpellScript();
        }
};

/// Cornered - 53497
class spell_hun_cornered : public SpellScriptLoader
{
    public:
        spell_hun_cornered() : SpellScriptLoader("spell_hun_cornered") { }

        class spell_hun_cornered_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_cornered_AuraScript);

            void OnUpdate(uint32, AuraEffect* p_AurEff)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (l_Target->HasAuraState(AuraStateType::AURA_STATE_HEALTHLESS_35_PERCENT))
                    {
                        p_AurEff->ChangeAmount(GetSpellInfo()->Effects[EFFECT_0].BasePoints);

                        if (AuraEffect* l_AurEff = p_AurEff->GetBase()->GetEffect(EFFECT_1))
                            l_AurEff->ChangeAmount(GetSpellInfo()->Effects[EFFECT_1].BasePoints);
                    }
                    else
                    {
                        p_AurEff->ChangeAmount(0);

                        if (AuraEffect* l_AurEff = p_AurEff->GetBase()->GetEffect(EFFECT_1))
                            l_AurEff->ChangeAmount(0);
                    }
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_hun_cornered_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_cornered_AuraScript();
        }
};

/// Exotic Munitions - 162534
class spell_hun_exotic_munitions : public SpellScriptLoader
{
    public:
        spell_hun_exotic_munitions() : SpellScriptLoader("spell_hun_exotic_munitions") { }

        class spell_hun_exotic_munitions_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_exotic_munitions_AuraScript);

            enum eSpells
            {
                IncendiaryAmmo = 162536,
                PoisonedAmmo = 162537,
                FrozenAmmo = 162539
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    l_Player->learnSpell(eSpells::IncendiaryAmmo, false);
                    l_Player->learnSpell(eSpells::PoisonedAmmo, false);
                    l_Player->learnSpell(eSpells::FrozenAmmo, false);
                }
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    l_Player->removeSpell(eSpells::IncendiaryAmmo);
                    l_Player->removeSpell(eSpells::PoisonedAmmo);
                    l_Player->removeSpell(eSpells::FrozenAmmo);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_exotic_munitions_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_exotic_munitions_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_exotic_munitions_AuraScript();
        }
};

const uint32 m_FirewoksSpells[4] = { 127937, 127936, 127961, 127951 };

// Fireworks - 127933
class spell_hun_fireworks: public SpellScriptLoader
{
    public:
        spell_hun_fireworks() : SpellScriptLoader("spell_hun_fireworks") { }

        class spell_hun_fireworks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_fireworks_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, m_FirewoksSpells[urand(0, 3)], true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_fireworks_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_fireworks_SpellScript();
        }
};


/// Last Update 6.2.3
/// Glaive Toss (damage) - 120761 and 121414
class spell_hun_glaive_toss_damage: public SpellScriptLoader
{
    public:
        spell_hun_glaive_toss_damage() : SpellScriptLoader("spell_hun_glaive_toss_damage") { }

        class spell_hun_glaive_toss_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_glaive_toss_damage_SpellScript);

            uint64 m_MainTargetGUID = 0;

            void CorrectRange(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_OriginalCaster = GetOriginalCaster();
                Unit* l_MainTarget = nullptr;

                for (auto l_Itr : p_Targets)
                {
                    Unit* l_Target = l_Itr->ToUnit();

                    uint64& l_GlaiveTossTarget = l_OriginalCaster->m_SpellHelper.GetUint64(eSpellHelpers::GlaiveTossTarget);
                    if (l_Target->GetGUID() == l_GlaiveTossTarget)
                    {
                        m_MainTargetGUID = l_Itr->GetGUID();
                        l_GlaiveTossTarget = 0;
                        break;
                    }
                }

                if (!m_MainTargetGUID)
                    m_MainTargetGUID = l_OriginalCaster->GetGUID();

                l_MainTarget = ObjectAccessor::FindUnit(m_MainTargetGUID);

                if (l_MainTarget == nullptr)
                {
                    p_Targets.clear();
                    return;
                }

                p_Targets.remove_if([this, l_Caster, l_MainTarget](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsWithinLOSInMap(l_Caster))
                        return true;

                    if (p_Object->ToUnit() && !l_Caster->IsValidAttackTarget(p_Object->ToUnit()))
                        return true;

                    if (p_Object->GetGUID() == l_MainTarget->GetGUID())
                        return false;

                    if (p_Object->IsInElipse(l_Caster, l_MainTarget, 7.0f, 4.0f))
                        return false;
                    return true;
                });
            }

            void OnDamage()
            {
                Unit* l_Target = GetHitUnit();

                if (!m_MainTargetGUID || l_Target == nullptr)
                    return;

                Unit* l_MainTarget = ObjectAccessor::FindUnit(m_MainTargetGUID);

                if (l_MainTarget != nullptr && l_MainTarget->GetGUID() == l_Target->GetGUID())
                    SetHitDamage(GetHitDamage() * 4);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_glaive_toss_damage_SpellScript::CorrectRange, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_glaive_toss_damage_SpellScript::CorrectRange, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_hun_glaive_toss_damage_SpellScript::OnDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_glaive_toss_damage_SpellScript();
        }
};

// Glaive Toss (Missile data) - 120755 and 120756
class spell_hun_glaive_toss_missile: public SpellScriptLoader
{
    public:
        spell_hun_glaive_toss_missile() : SpellScriptLoader("spell_hun_glaive_toss_missile") { }

        class spell_hun_glaive_toss_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_glaive_toss_missile_SpellScript);

            enum eSpells
            {
                GlaiveTossRight                 = 120755,
                GlaiveTossLeft                  = 120756,
                GlaiveTossDamageAndSnareRight   = 121414,
                GlaiveTossDamageAndSnareLeft    = 120761,
            };

            void HandleAfterCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                Unit* l_OriginalCaster = GetOriginalCaster();

                if (l_Target != nullptr)
                {
                    if (l_Caster->GetGUID() == GetOriginalCaster()->GetGUID())
                        l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::GlaiveTossTarget) = l_Target->GetGUID();
                    else
                        l_OriginalCaster->m_SpellHelper.GetUint64(eSpellHelpers::GlaiveTossTarget) = l_Caster->GetGUID();
                }

                if (GetSpellInfo()->Id == eSpells::GlaiveTossRight)
                {
                    if (l_OriginalCaster != nullptr)
                        l_OriginalCaster->CastSpell(l_OriginalCaster, eSpells::GlaiveTossDamageAndSnareRight, true);
                    else
                        l_Caster->CastSpell(l_Caster, eSpells::GlaiveTossDamageAndSnareRight, true);
                }
                if (GetSpellInfo()->Id == eSpells::GlaiveTossLeft)
                {
                    if (l_OriginalCaster != nullptr)
                        l_OriginalCaster->CastSpell(l_OriginalCaster, eSpells::GlaiveTossDamageAndSnareLeft, true);
                    else
                        l_Caster->CastSpell(l_Caster, eSpells::GlaiveTossDamageAndSnareLeft, true);
                }
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                if (GetSpellInfo()->Id == eSpells::GlaiveTossRight)
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Caster == GetOriginalCaster())
                            l_Target->CastSpell(l_Caster, eSpells::GlaiveTossLeft, true, NULL, nullptr, l_Caster->GetGUID());
                    }
                }
                else
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Caster == GetOriginalCaster())
                            l_Target->CastSpell(l_Caster, eSpells::GlaiveTossRight, true, NULL, nullptr, l_Caster->GetGUID());
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_glaive_toss_missile_SpellScript::HandleAfterCast);
                OnHit += SpellHitFn(spell_hun_glaive_toss_missile_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_glaive_toss_missile_SpellScript();
        }
};

/// last update : 7.0.1
/// A Murder of Crows - 131894 and Murder of Crows - 206505
class spell_hun_a_murder_of_crows: public SpellScriptLoader
{
    public:
        spell_hun_a_murder_of_crows() : SpellScriptLoader("spell_hun_a_murder_of_crows") { }

        class spell_hun_a_murder_of_crows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_a_murder_of_crows_AuraScript);

            enum eSpells
            {
                FreezingTrap                = 3355,
                MurderOfCrowsVisualFirst    = 131951,
                MurderOfCrowsVisualSecond   = 131952,
                MurderOfCrowsDamage         = 131900
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster == nullptr)
                    return;

                /// Visual effect
                /// Four crows fall from the sky to target
                for (int8 i = 0; i < 2; ++i)
                {
                    l_Target->CastSpell(l_Target, eSpells::MurderOfCrowsVisualFirst, true);
                    l_Target->CastSpell(l_Target, eSpells::MurderOfCrowsVisualSecond, true);
                }

                if (l_Caster->IsValidAttackTarget(l_Target))
                    l_Caster->CastSpell(l_Target, eSpells::MurderOfCrowsDamage, true);
                else
                    p_AurEff->GetBase()->Remove();

                if (l_Target->HasAura(eSpells::FreezingTrap))
                    l_Target->RemoveAura(eSpells::FreezingTrap);
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (l_Player->HasSpellCooldown(GetSpellInfo()->Id))
                            l_Player->RemoveSpellCooldown(GetSpellInfo()->Id, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_hun_a_murder_of_crows_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_a_murder_of_crows_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_a_murder_of_crows_AuraScript();
        }
};

/// last update : 6.2.3
/// A Murder of Crows (damage) - 131900
class spell_hun_a_murder_of_crows_damage : public SpellScriptLoader
{
    public:
        spell_hun_a_murder_of_crows_damage() : SpellScriptLoader("spell_hun_a_murder_of_crows_damage") { }

        class spell_hun_a_murder_of_crows_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_a_murder_of_crows_damage_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Target == nullptr || l_Player == nullptr)
                    return;

                /// A Murder of Crows now deals only 65% of normal damage in pvp: https://blue.mmo-champion.com/topic/417264-pvp-stat-templates-and-spell-multipliers-july-15th-2016/
                if (l_Target->GetSpellModOwner())
                    SetHitDamage(CalculatePct(GetHitDamage(), 65));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_a_murder_of_crows_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_a_murder_of_crows_damage_SpellScript();
        }
};

/// Frenzy - 19615
class spell_hun_frenzy : public SpellScriptLoader
{
    public:
        spell_hun_frenzy() : SpellScriptLoader("spell_hun_frenzy") { }

        enum eSpells
        {
            FrenzyReady     = 88843,
            FrenzyStacks    = 19615
        };

        class spell_hun_frenzy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_frenzy_SpellScript);

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Owner = l_Caster->GetOwner())
                    {
                        if (Aura* l_Frenzy = l_Caster->GetAura(eSpells::FrenzyStacks))
                        {
                            if (l_Frenzy->GetStackAmount() >= 5)
                                l_Owner->CastSpell(l_Owner, eSpells::FrenzyReady, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_frenzy_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_frenzy_SpellScript();
        }

        class spell_hun_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_frenzy_AuraScript);

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpells::FrenzyReady);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_hun_frenzy_AuraScript::HandleRemove, EFFECT_2, SPELL_AURA_ADD_VIRTUAL_CHARGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_frenzy_AuraScript();
        }
};

/// Beast Cleave - 118455
class spell_hun_beast_cleave_proc: public SpellScriptLoader
{
    public:
        spell_hun_beast_cleave_proc() : SpellScriptLoader("spell_hun_beast_cleave_proc") { }

        class spell_hun_beast_cleave_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_beast_cleave_proc_AuraScript);

            enum eSpells
            {
                BeastCleaveAura     = 115939,
                BeastCleaveDamage   = 118459
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                Unit* l_Target = GetTarget();
                if (l_Target == nullptr)
                    return;

                if (p_EventInfo.GetActor()->GetGUID() != l_Target->GetGUID())
                    return;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() && p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id == eSpells::BeastCleaveDamage)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Target->HasAura(p_AurEff->GetSpellInfo()->Id, l_Player->GetGUID()))
                    {
                        if (AuraEffect* l_AurEff = l_Player->GetAuraEffect(eSpells::BeastCleaveAura, EFFECT_0))
                        {
                            int32 l_BP = CalculatePct(p_EventInfo.GetDamageInfo()->GetAmount(), l_AurEff->GetAmount());
                            l_Target->CastCustomSpell(l_Target, eSpells::BeastCleaveDamage, &l_BP, nullptr, nullptr, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_beast_cleave_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_beast_cleave_proc_AuraScript();
        }
};

// Called by Multi Shot - 2643
// Beast Cleave - 115939
class spell_hun_beast_cleave: public SpellScriptLoader
{
    public:
        spell_hun_beast_cleave() : SpellScriptLoader("spell_hun_beast_cleave") { }

        class spell_hun_beast_cleave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_beast_cleave_SpellScript);

            enum eSpells
            {
                BeastCleaveAura = 115939,
                BeastCleaveProc = 118455,
                MasterOfBeast   = 197248
            };

            void HandleAfterCast()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::BeastCleaveAura))
                    {
                        if (Pet* l_Pet = l_Player->GetPet())
                        {
                            l_Player->CastSpell(l_Pet, eSpells::BeastCleaveProc, true);
                            if (l_Player->HasAura(eSpells::MasterOfBeast))
                            {
                                if (Creature* l_Hati = l_Player->GetHati())
                                    l_Player->CastSpell(l_Hati, eSpells::BeastCleaveProc, true);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
               AfterCast += SpellCastFn(spell_hun_beast_cleave_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_beast_cleave_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Barrage damage - 120361
class spell_hun_barrage : public SpellScriptLoader
{
    public:
        spell_hun_barrage() : SpellScriptLoader("spell_hun_barrage") { }

        class spell_hun_barrage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_barrage_SpellScript);

            enum eSpells
            {
                BarrageTalent = 120360
            };

            void CheckLOS(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsWithinLOSInMap(l_Caster))
                        return true;

                    if (p_Object->ToUnit() && !l_Caster->IsValidAttackTarget(p_Object->ToUnit()))
                        return true;

                    return false;
                });

                for (auto l_Target : p_Targets)
                    m_Targets.push_back(l_Target->GetGUID());
            }

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Player == nullptr || l_Target == nullptr)
                    return;

                float l_Mindamage = 0.0f;
                float l_Maxdamage = 0.0f;

                l_Player->CalculateMinMaxDamage(WeaponAttackType::RangedAttack, true, true, l_Mindamage, l_Maxdamage);

                int32 l_Damage = (l_Mindamage + l_Maxdamage) / 2 * 0.7f;

                if (!l_Target->HasAura(eSpells::BarrageTalent, l_Player->GetGUID()))
                    l_Damage /= 2;

                l_Damage = l_Player->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);
                l_Damage = l_Target->SpellDamageBonusTaken(l_Player, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

                /// Barrage now deals only 80% of normal damage against player-controlled targets.
                if (l_Target->GetSpellModOwner())
                    l_Damage = CalculatePct(l_Damage, 80);

                SetHitDamage(l_Damage);
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || m_Targets.empty())
                    return;

                for (uint64 l_Guid : m_Targets)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Guid);
                    if (!l_Target)
                        continue;

                    Position l_Pos = l_Target->GetPosition();
                    l_Pos.m_positionX += frand(-2.f, 2.f);
                    l_Pos.m_positionY += frand(-2.f, 2.f);
                    l_Pos.m_positionZ += frand(0.f, 2.f);

                    l_Caster->SendPlaySpellVisual(63371, nullptr, frand(35.0, 45.f), l_Pos, false, false);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_barrage_SpellScript::HandleOnCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_barrage_SpellScript::CheckLOS, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_barrage_SpellScript::CheckLOS, EFFECT_1, TARGET_UNIT_CONE_ENEMY_24);
                OnHit += SpellHitFn(spell_hun_barrage_SpellScript::HandleOnHit);
            }

            std::vector<uint64> m_Targets;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_barrage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 26365
/// Called by Barrage - 120360
class spell_hun_barrage_haste : public SpellScriptLoader
{
public:
    spell_hun_barrage_haste() : SpellScriptLoader("spell_hun_barrage_haste") { }

    class spell_hun_barrage_haste_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_barrage_haste_AuraScript);

        void HandleOnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            int32 l_Amplitude = int32(p_AuraEffect->GetAmplitude() * std::max<float>(l_Caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED), 0.4f));
            const_cast<AuraEffect*>(p_AuraEffect)->SetAmplitude(l_Amplitude);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_hun_barrage_haste_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_barrage_haste_AuraScript();
    }
};

/// Binding Shot - 117405
class spell_hun_binding_shot : public SpellScriptLoader
{
    public:
        spell_hun_binding_shot() : SpellScriptLoader("spell_hun_binding_shot") { }

        class spell_hun_binding_shot_zone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_binding_shot_zone_AuraScript);

            enum eSpells
            {
                BindingShotArrow    = 109248,
                BindingShotImmune   = 117553,
                BindingShotStun     = 117526,
                BindingShotLink     = 117405
            };

            void OnUpdate(uint32, AuraEffect*)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetUnitOwner())
                    {
                        AreaTrigger* l_BindingShot = l_Caster->GetAreaTrigger(eSpells::BindingShotArrow);
                        if (l_BindingShot == nullptr)
                            return;

                        /// If AreaTrigger exists, spellInfo exists too
                        float l_Radius = sSpellMgr->GetSpellInfo(eSpells::BindingShotArrow)->Effects[EFFECT_1].CalcRadius(l_Caster);

                        if (l_Target->GetDistance(l_BindingShot) > l_Radius)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::BindingShotStun, true);
                            l_Caster->CastSpell(l_Target, eSpells::BindingShotImmune, true);
                            l_Target->RemoveAura(eSpells::BindingShotLink);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_hun_binding_shot_zone_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_binding_shot_zone_AuraScript();
        }
};

/// Binding Shot - 109248
class spell_hun_binding_shot_zone : public SpellScriptLoader
{
    public:
        spell_hun_binding_shot_zone() : SpellScriptLoader("spell_hun_binding_shot_zone") { }

        class spell_hun_binding_shot_zone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_binding_shot_zone_SpellScript);

            enum eSpells
            {
                BindingShotArrow = 118306
            };

            void HandleHitDest(SpellEffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (WorldLocation const* l_Dest = GetExplTargetDest())
                        l_Caster->CastSpell(l_Dest->m_positionX, l_Dest->m_positionY, l_Dest->m_positionZ, eSpells::BindingShotArrow, true);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_hun_binding_shot_zone_SpellScript::HandleHitDest, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_binding_shot_zone_SpellScript();
        }
};

// Called by Multi Shot - 2643
// Serpent Spread - 87935
class spell_hun_serpent_spread: public SpellScriptLoader
{
    public:
        spell_hun_serpent_spread() : SpellScriptLoader("spell_hun_serpent_spread") { }

        class spell_hun_serpent_spread_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_serpent_spread_SpellScript);

            enum eSpells
            {
                SerpentSting    = 118253,
                SerpentSpread   = 87935
            };

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                    if (Unit* l_Target = GetHitUnit())
                        if (l_Player->HasAura(eSpells::SerpentSpread))
                            l_Player->CastSpell(l_Target, eSpells::SerpentSting, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_serpent_spread_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_serpent_spread_SpellScript();
        }
};

/// Ancient Hysteria - 90355 - last update: 6.1.2 19802
class spell_hun_ancient_hysteria : public SpellScriptLoader
{
    public:
        spell_hun_ancient_hysteria() : SpellScriptLoader("spell_hun_ancient_hysteria") { }

        class spell_hun_ancient_hysteria_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_ancient_hysteria_SpellScript);

            enum eSpells
            {
                Insanity                = 95809,
                Sated                   = 57724,
                Fatigued                = 160455,
                TemporalDisplacement    = 80354,
                Exhausted               = 57723
            };

            bool Validate(SpellInfo const* /*p_SpellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::Insanity))
                    return false;

                return true;
            }

            void ApplyDebuff()
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    if (GetSpellInfo() && l_Target->HasAura(GetSpellInfo()->Id))
                        l_Target->CastSpell(l_Target, eSpells::Insanity, true);
                }
            }

            void HandleImmunity(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eSpells::Exhausted) ||
                    l_Target->HasAura(eSpells::Insanity) ||
                    l_Target->HasAura(eSpells::Sated) ||
                    l_Target->HasAura(eSpells::TemporalDisplacement) ||
                    l_Target->HasAura(eSpells::Fatigued))
                {
                    PreventHitAura();
                    return;
                }

                if (!l_Target->IsPlayer())
                    return;

                if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    l_Instance->HandleBloodlustApplied(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hun_ancient_hysteria_SpellScript::ApplyDebuff);
                OnEffectHitTarget += SpellEffectFn(spell_hun_ancient_hysteria_SpellScript::HandleImmunity, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                OnEffectHitTarget += SpellEffectFn(spell_hun_ancient_hysteria_SpellScript::HandleImmunity, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_ancient_hysteria_SpellScript();
        }
};

/// Netherwinds - 160452 - last update: 6.1.2 19802
class spell_hun_netherwinds : public SpellScriptLoader
{
    public:
        spell_hun_netherwinds() : SpellScriptLoader("spell_hun_netherwinds") { }

        class spell_hun_netherwinds_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_netherwinds_SpellScript);

            enum eSpells
            {
                TemporalDisplacement    = 80354,
                Insanity                = 95809,
                Exhausted               = 57723,
                Sated                   = 57724,
                Fatigued                = 160455
            };

            void ApplyDebuff()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->isPet())
                    l_Caster = l_Caster->GetAnyOwner();

                if (l_Caster == nullptr)
                    return;

                if (GetSpellInfo() && l_Target->HasAura(GetSpellInfo()->Id))
                    l_Caster->CastSpell(l_Target, eSpells::Fatigued, true);
            }

            void HandleImmunity(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eSpells::Exhausted) || l_Target->HasAura(eSpells::Insanity) ||
                    l_Target->HasAura(eSpells::Sated) || l_Target->HasAura(eSpells::TemporalDisplacement) ||
                    l_Target->HasAura(eSpells::Fatigued))
                {
                    PreventHitAura();
                    return;
                }

                if (!l_Target->IsPlayer())
                    return;

                if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    l_Instance->HandleBloodlustApplied(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hun_netherwinds_SpellScript::ApplyDebuff);
                OnEffectHitTarget += SpellEffectFn(spell_hun_netherwinds_SpellScript::HandleImmunity, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                OnEffectHitTarget += SpellEffectFn(spell_hun_netherwinds_SpellScript::HandleImmunity, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_netherwinds_SpellScript();
        }
};

/// last update : 7.0.3
/// Kill Command - 83381
class spell_hun_kill_command_proc : public SpellScriptLoader
{
    public:
        spell_hun_kill_command_proc() : SpellScriptLoader("spell_hun_kill_command_proc") { }

        class spell_hun_kill_command_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_kill_command_proc_SpellScript);

            enum eSpells
            {
                SpiritBond      = 197199,
                SpiritBondHeal  = 197205
            };

            enum eNpcs
            {
                Hati = 100324
            };

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = l_Caster->GetOwner();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || l_Owner == nullptr || !l_Caster)
                    return;

                int32 l_Damage = 0;
                if (l_Caster->GetEntry() != eNpcs::Hati)
                {
                    l_Damage = int32(l_Caster->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 3.6f * 1.5f); ///< taking pet stats, not hunter stats. No official proof, but retail damage logs :c
                }
                else
                {
                    l_Damage = int32(l_Caster->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 1.8f * 1.15f); ///< same as ^
                }

                l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, SpellEffIndex::EFFECT_0, DamageEffectType::SPELL_DIRECT_DAMAGE);
                l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, DamageEffectType::SPELL_DIRECT_DAMAGE, SpellEffIndex::EFFECT_0);

                if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::SpiritBond, EFFECT_0))
                {
                    uint8 l_AmountPct = l_AuraEffect->GetAmount();
                    if (l_AmountPct)
                    {
                        int32 l_Bp = CalculatePct(l_Damage, l_AmountPct);
                        l_Owner->CastCustomSpell(l_Owner, eSpells::SpiritBondHeal, &l_Bp, nullptr, nullptr, true);
                    }
                }

                if (l_Target->GetTypeId() == TYPEID_UNIT)
                    l_Damage *= l_Caster->CalculateDamageDealtFactor(l_Caster, l_Target->ToCreature());

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_kill_command_proc_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_kill_command_proc_SpellScript();
        }
};

// Chimaera Shot - 53209
class spell_hun_chimaera_shot: public SpellScriptLoader
{
    public:
        spell_hun_chimaera_shot() : SpellScriptLoader("spell_hun_chimaera_shot") { }

        class spell_hun_chimaera_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_chimaera_shot_SpellScript);

            enum ChimaeraSpells
            {
                SpellChimaeraFrost  = 171454,
                SpellChimaeraNature = 171457,
                ChimaeraShotHealing = 53353
            };

            bool m_Healed;

            bool Load() override
            {
                m_Healed = false;
                return true;
            }

            void HandleOnHit(SpellEffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        /// 50% chance to deals frost damage or nature damage
                        if (urand(0, 1))
                            l_Caster->CastSpell(l_Target, ChimaeraSpells::SpellChimaeraFrost, true);
                        else
                            l_Caster->CastSpell(l_Target, ChimaeraSpells::SpellChimaeraNature, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_chimaera_shot_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_chimaera_shot_SpellScript();
        }
};

// Master's Call - 53271
class spell_hun_masters_call: public SpellScriptLoader
{
    public:
        spell_hun_masters_call() : SpellScriptLoader("spell_hun_masters_call") { }

        class spell_hun_masters_call_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_masters_call_SpellScript);

            enum eSpells
            {
                MastersCall             = 62305,
                MastersCallTriggered    = 54216
            };

            SpellCastResult CheckIfPetInLOS()
            {
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                    {
                        if (l_Pet->isDead())
                            return SPELL_FAILED_NO_PET;

                        float l_X, l_Y, l_Z;
                        l_Pet->GetPosition(l_X, l_Y, l_Z);

                        if (!l_Pet->IsWithinDist(l_Target, 40.0f, true))
                            return SPELL_FAILED_OUT_OF_RANGE;

                        if (l_Pet->HasAuraType(SPELL_AURA_MOD_STUN) || l_Pet->HasAuraType(SPELL_AURA_MOD_CONFUSE) || l_Pet->HasAuraType(SPELL_AURA_MOD_SILENCE) ||
                            l_Pet->HasAuraType(SPELL_AURA_MOD_FEAR) || l_Pet->HasAuraType(SPELL_AURA_MOD_FEAR_2))
                            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                        if (l_Player->IsWithinLOS(l_X, l_Y, l_Z))
                            return SPELL_CAST_OK;
                    }
                }
                return SPELL_FAILED_LINE_OF_SIGHT;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* l_Caster = GetCaster()->ToPlayer())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (Pet* l_Pet = l_Caster->GetPet())
                        {
                            l_Pet->RemoveMovementImpairingAuras();
                            l_Pet->CastSpell(l_Target, eSpells::MastersCallTriggered, true);
                        }
                    }
                }
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    l_Target->RemoveMovementImpairingAuras();
                    l_Target->CastSpell(l_Target, eSpells::MastersCall, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_masters_call_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHitTarget += SpellEffectFn(spell_hun_masters_call_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
                OnCheckCast += SpellCheckCastFn(spell_hun_masters_call_SpellScript::CheckIfPetInLOS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_masters_call_SpellScript();
        }
};

/// Heart of the Phoenix - 55709
class spell_hun_pet_heart_of_the_phoenix: public SpellScriptLoader
{
    public:
        spell_hun_pet_heart_of_the_phoenix() : SpellScriptLoader("spell_hun_pet_heart_of_the_phoenix") { }

        class spell_hun_pet_heart_of_the_phoenix_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_pet_heart_of_the_phoenix_SpellScript);

            enum eSpells
            {
                HeartOfThePhoenixTriggered  = 54114,
                HeartOfThePhoenixDebuff     = 55711
            };

            bool Load() override
            {
                if (!GetCaster()->isPet())
                    return false;
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::HeartOfThePhoenixTriggered) || !sSpellMgr->GetSpellInfo(eSpells::HeartOfThePhoenixDebuff))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (Unit* l_Owner = l_Caster->GetOwner())
                    if (!l_Caster->HasAura(eSpells::HeartOfThePhoenixDebuff))
                    {
                        l_Owner->CastCustomSpell(eSpells::HeartOfThePhoenixTriggered, SPELLVALUE_BASE_POINT0, 100, l_Caster, true);
                        l_Caster->CastSpell(l_Caster, eSpells::HeartOfThePhoenixDebuff, true);
                    }
            }

            void Register() override
            {
                // add dummy effect spell handler to pet's Last Stand
                OnEffectHitTarget += SpellEffectFn(spell_hun_pet_heart_of_the_phoenix_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_pet_heart_of_the_phoenix_SpellScript();
        }
};

/// Sticky Bomb - 191241
class spell_hun_sticky_bomb : public SpellScriptLoader
{
    public:
        spell_hun_sticky_bomb() : SpellScriptLoader("spell_hun_sticky_bomb") { }

        class spell_hun_sticky_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_sticky_bomb_AuraScript);

            enum eSpells
            {
                StickyBomb = 191244
            };

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                Position l_TargetPos;
                l_Target->SimplePosXYRelocationByAngle(l_TargetPos, l_Target->GetObjectSize(), 0.0f);

                l_Caster->CastSpell(l_TargetPos, eSpells::StickyBomb, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_sticky_bomb_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_sticky_bomb_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
// Misdirection - 34477
class spell_hun_misdirection: public SpellScriptLoader
{
    public:
        spell_hun_misdirection() : SpellScriptLoader("spell_hun_misdirection") { }

        enum MisdirectionSpells
        {
            MisdirectionSpell       = 34477,
            MisdirectionProc        = 35079,
            HuntersAdvantage        = 197178,
            HuntersAdvantageBuff    = 211138
        };


        class spell_hun_misdirection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_misdirection_SpellScript);

            void HandleAfterHit()
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Caster == nullptr)
                    return;
                if (l_Target == nullptr)
                    return;

                Pet* l_Pet = l_Caster->GetPet();

                if (l_Pet == nullptr)
                    return;

                /// Self proc
                l_Caster->CastSpell(l_Caster, MisdirectionSpells::MisdirectionProc, true);

                if (l_Caster->HasAura(MisdirectionSpells::HuntersAdvantage))
                    l_Caster->CastSpell(l_Pet, MisdirectionSpells::HuntersAdvantageBuff, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hun_misdirection_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_misdirection_SpellScript;
        }

        class spell_hun_misdirection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_misdirection_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Player* l_Caster = GetCaster()->ToPlayer();

                if (l_Caster == nullptr)
                    return;

                if (!GetDuration())
                    l_Caster->SetReducedThreatPercent(0, 0);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_misdirection_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_misdirection_AuraScript();
        }
};

// Misdirection (proc) - 35079
class spell_hun_misdirection_proc: public SpellScriptLoader
{
    public:
        spell_hun_misdirection_proc() : SpellScriptLoader("spell_hun_misdirection_proc") { }

        class spell_hun_misdirection_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_misdirection_proc_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    GetCaster()->SetReducedThreatPercent(0, 0);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_misdirection_proc_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_misdirection_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Disengage - 781
class spell_hun_disengage: public SpellScriptLoader
{
    public:
        spell_hun_disengage() : SpellScriptLoader("spell_hun_disengage") { }

        class spell_hun_disengage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_disengage_SpellScript);

            enum eSpells
            {
                Posthaste                       = 109215,
                PostHastBuff                    = 118922,
                NarrowEscape                    = 109298,
                NarrowEscapeRets                = 128405,
                SurvivalOfTheFittest            = 190514,
                SurvivalOfTheFittestEffect      = 190515
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Posthaste))
                {
                    l_Caster->RemoveMovementImpairingAuras();
                    l_Caster->CastSpell(l_Caster, eSpells::PostHastBuff, true);
                }
            }

            void HandleAfterCast()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::NarrowEscape))
                    {
                        std::list<Unit*> l_UnitList;
                        std::list<Unit*> l_RetsList;

                        l_Player->GetAttackableUnitListInRange(l_UnitList, 8.0f);

                        for (auto l_Itr : l_UnitList)
                            if (l_Player->IsValidAttackTarget(l_Itr))
                                l_RetsList.push_back(l_Itr);

                        for (auto l_Itr : l_RetsList)
                            l_Player->CastSpell(l_Itr, eSpells::NarrowEscapeRets, true);
                    }
                }
            }

            void HandleCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if(l_Caster->HasAura(eSpells::SurvivalOfTheFittest))
                        l_Caster->CastSpell(l_Caster, eSpells::SurvivalOfTheFittestEffect, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_disengage_SpellScript::HandleAfterCast);
                OnCast += SpellCastFn(spell_hun_disengage_SpellScript::HandleCast);
                BeforeCast += SpellCastFn(spell_hun_disengage_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_disengage_SpellScript();
        }
};

/// Survival Of The Fittest - 190515
class spell_hun_survival_of_the_fittest : public SpellScriptLoader
{
    public:
        spell_hun_survival_of_the_fittest() : SpellScriptLoader("spell_hun_survival_of_the_fittest") { }

        class spell_hun_survival_of_the_fittest_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_survival_of_the_fittest_AuraScript);

            enum eArtifactPowers
            {
                SurvivalOfTheFittestArtifact = 317
            };

            void HandleEffectCalcAmount(AuraEffect const* /*auraEffect*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    switch (l_Player->GetRankOfArtifactPowerId(eArtifactPowers::SurvivalOfTheFittestArtifact))
                    {
                        case 1:
                            p_Amount = -3;
                            break;
                        case 2:
                            p_Amount = -6;
                            break;
                        case 3:
                            p_Amount = -10;
                            break;
                        case 4:
                            p_Amount = -13;
                            break;
                        case 5:
                            p_Amount = -15;
                            break;
                        default:
                            break;
                    };
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_survival_of_the_fittest_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_survival_of_the_fittest_AuraScript();
        }
};

// Tame Beast - 1515
class spell_hun_tame_beast: public SpellScriptLoader
{
    public:
        spell_hun_tame_beast() : SpellScriptLoader("spell_hun_tame_beast") { }

        class spell_hun_tame_beast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_tame_beast_SpellScript);

            enum eCreature
            {
                Chimaeron = 43296
            };

            SpellCastResult CheckCast()
            {
                int8 l_ResultId = -1;
                Player* l_Player = GetCaster()->ToPlayer();

                if (!GetExplTargetUnit() || GetExplTargetUnit()->GetTypeId() != TYPEID_UNIT)
                    l_ResultId = PET_TAME_ERROR_INVALID_CREATURE;

                Creature* l_Target = GetExplTargetUnit()->ToCreature();

                // No errors until now, time to do more checks
                if (l_ResultId == -1)
                {
                    if (l_Player->getSlotForNewPet() == PET_SLOT_FULL_LIST)
                        l_ResultId = PET_TAME_ERROR_TOO_MANY_PETS;
                    else if (l_Player->GetPetGUID() || l_Player->GetCharmGUID())
                        l_ResultId = PET_TAME_ERROR_ANOTHER_SUMMON_ACTIVE;
                    else if (l_Target->isPet())
                        l_ResultId = PET_TAME_ERROR_CREATURE_ALREADY_OWNED;
                    else if (l_Target->getLevelForTarget(l_Player) > l_Player->getLevel())
                        l_ResultId = PET_TAME_ERROR_TOO_HIGH_LEVEL;
                    else if ((l_Target->GetCreatureTemplate()->type_flags & CREATURE_TYPE_FLAG_EXOTIC_PET) && !l_Player->CanTameExoticPets())
                        l_ResultId = PET_TAME_ERROR_CANT_CONTROL_EXOTIC;
                    else if ((l_Player->getRace() == RACE_GNOME || l_Player->getRace() == RACE_GOBLIN || l_Player->HasAura(205154)) && l_Target->GetCreatureTemplate()->type == CREATURE_TYPE_MECHANICAL)
                        l_ResultId = -1;
                    else if (!l_Target->GetCreatureTemplate()->isTameable(l_Player->CanTameExoticPets()))
                        l_ResultId = PET_TAME_ERROR_NOT_TAMEABLE;
                }

                if (l_ResultId >= 0)
                {
                    l_Player->SendPetTameResult((PetTameResult) l_ResultId);
                    return SPELL_FAILED_DONT_REPORT;
                }

                /// Chimaeron can be tamed but only at 20%
                if (l_Target->GetEntry() == eCreature::Chimaeron && l_Target->GetHealthPct() > 20.0f)
                {
                    SetCustomCastResultMessage(SpellCustomErrors::SPELL_CUSTOM_ERROR_CHIMAERON_TOO_CALM_TO_TAME);
                    return SpellCastResult::SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_tame_beast_SpellScript::CheckCast);
            }
        };

        class spell_hun_tame_beast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_tame_beast_AuraScript);

            enum eSpells
            {
                TameMechanicalVisual = 205835
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Creature* l_Creature = GetTarget()->ToCreature();
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Creature && l_Creature->GetCreatureTemplate()->type == CREATURE_TYPE_MECHANICAL)
                    l_Caster->CastSpell(l_Creature, eSpells::TameMechanicalVisual, true);
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAura(eSpells::TameMechanicalVisual);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_tame_beast_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_tame_beast_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_tame_beast_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_tame_beast_SpellScript();
        }
};

/// Claw - 16827 / Bite - 17253 / Smack - 49966
class spell_hun_claw_bite : public SpellScriptLoader
{
    public:
        spell_hun_claw_bite() : SpellScriptLoader("spell_hun_claw_bite") { }

        class spell_hun_claw_bite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_claw_bite_SpellScript);

            enum eSpells
            {
                Invigoration            = 53253,
                InvigorationEffect      = 53398,
                CobraStrikesStacks      = 53257,
                BasicAttackCostModifier = 62762,
                SpikedCollar            = 53184,
                Frenzy                  = 19623,
                FrenzyStacks            = 19615,
                BlinkStrikes            = 130392,
                BlinkStrikesTeleport    = 130393
            };

            SpellCastResult CheckCastRange()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                Unit* l_Owner = l_Caster->GetOwner();

                if (l_Owner == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Owner->HasAura(eSpells::BlinkStrikes))
                {
                    if (l_Owner->ToPlayer()->HasSpellCooldown(eSpells::BlinkStrikes) && l_Caster->GetDistance(l_Target) > 10.0f)
                        return SPELL_FAILED_OUT_OF_RANGE;

                    if ((l_Caster->isInRoots() || l_Caster->isInStun()) && l_Caster->GetDistance(l_Target) > 5.0f)
                        return SPELL_FAILED_ROOTED;

                    /// Blinking Strikes
                    if (!l_Owner->ToPlayer()->HasSpellCooldown(eSpells::BlinkStrikes) && l_Target->IsWithinLOSInMap(l_Caster) &&
                        l_Caster->GetDistance(l_Target) > 10.0f && l_Caster->GetDistance(l_Target) < 30.0f && !l_Caster->isInRoots() && !l_Caster->isInStun())
                    {
                        l_Caster->CastSpell(l_Target, eSpells::BlinkStrikesTeleport, true);

                        if (l_Caster->ToCreature()->IsAIEnabled && l_Caster->ToPet())
                        {
                            l_Caster->ToPet()->ClearUnitState(UNIT_STATE_FOLLOW);
                            if (l_Caster->ToPet()->getVictim())
                                l_Caster->ToPet()->AttackStop();
                            l_Caster->GetMotionMaster()->Clear();
                            l_Caster->ToPet()->GetCharmInfo()->SetIsCommandAttack(true);
                            l_Caster->ToPet()->GetCharmInfo()->SetIsAtStay(false);
                            l_Caster->ToPet()->GetCharmInfo()->SetIsReturning(false);
                            l_Caster->ToPet()->GetCharmInfo()->SetIsFollowing(false);

                            l_Caster->ToCreature()->AI()->AttackStart(l_Target);
                        }

                        l_Owner->ToPlayer()->AddSpellCooldown(eSpells::BlinkStrikes, 0, 20 * IN_MILLISECONDS, true);
                    }
                }
                return SPELL_CAST_OK;
            }


            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (Pet* l_Pet = GetCaster()->ToPet())
                {
                    if (Unit* l_Hunter = GetCaster()->GetOwner())
                    {
                        Unit* l_Target = GetHitUnit();

                        if (l_Target == nullptr)
                            return;

                        int32 l_Damage = int32(l_Hunter->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 0.333f);

                        SpellInfo const* l_SpikedCollar = sSpellMgr->GetSpellInfo(eSpells::SpikedCollar);
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BasicAttackCostModifier);
                        SpellInfo const* l_Frenzy = sSpellMgr->GetSpellInfo(eSpells::Frenzy);

                        /// Increases the damage done by your pet's Basic Attacks by 10%
                        if (l_Pet->HasAura(eSpells::SpikedCollar) && l_SpikedCollar != nullptr)
                            AddPct(l_Damage, l_SpikedCollar->Effects[EFFECT_0].BasePoints);

                        /// Deals 100% more damage and costs 100% more Focus when your pet has 50 or more Focus.
                        if (l_Pet->GetPower(POWER_FOCUS) + 25 >= 50)
                        {
                            if (l_SpellInfo != nullptr)
                                l_Damage += CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_1].BasePoints);
                            l_Pet->EnergizeBySpell(l_Pet, GetSpellInfo()->Id, -25, POWER_FOCUS);
                        }

                        /// Frenzy - 19623
                        if (l_Hunter->HasAura(eSpells::Frenzy) && roll_chance_i(l_Frenzy->Effects[EFFECT_1].BasePoints))
                            l_Pet->CastSpell(l_Pet, eSpells::FrenzyStacks, true);

                        l_Damage *= l_Pet->GetModifierValue(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_PCT);

                        l_Damage = l_Pet->MeleeDamageBonusDone(l_Target, l_Damage, WeaponAttackType::BaseAttack, GetSpellInfo());
                        l_Damage = l_Target->MeleeDamageBonusTaken(l_Pet, l_Damage, WeaponAttackType::BaseAttack, GetSpellInfo());

                        if (l_Target->GetTypeId() == TYPEID_UNIT)
                            l_Damage *= l_Pet->CalculateDamageDealtFactor(l_Pet, l_Target->ToCreature());

                        SetHitDamage(l_Damage);

                        /// Invigoration - 53253
                        if (l_Hunter->HasAura(eSpells::Invigoration))
                        if (roll_chance_i(15))
                            l_Hunter->CastSpell(l_Hunter, eSpells::InvigorationEffect, true);
                    }
                }
            }

            void HandleAfterHit()
            {
                Pet* l_Pet = GetCaster()->ToPet();
                Unit* l_Hunter = GetCaster()->GetOwner();

                if (l_Hunter == nullptr || l_Pet == nullptr)
                    return;

                Player* l_Player = l_Hunter->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (Aura* l_CobraStrike = l_Hunter->GetAura(eSpells::CobraStrikesStacks))
                    l_CobraStrike->ModStackAmount(-1);
                if (Aura* l_CobraStrikePet = l_Pet->GetAura(eSpells::CobraStrikesStacks))
                    l_CobraStrikePet->ModStackAmount(-1);
            }

            void HandleBeforeHit()
            {
                SetHitDamage(1);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_claw_bite_SpellScript::CheckCastRange);
                BeforeHit += SpellHitFn(spell_hun_claw_bite_SpellScript::HandleBeforeHit);
                OnEffectHitTarget += SpellEffectFn(spell_hun_claw_bite_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterHit += SpellHitFn(spell_hun_claw_bite_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_claw_bite_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Spirit Mend - 90361
class spell_hun_spirit_mend : public SpellScriptLoader
{
    public:
        spell_hun_spirit_mend() : SpellScriptLoader("spell_hun_spirit_mend") { }

        class spell_hun_spirit_mend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_spirit_mend_AuraScript);

            void CalculateAmount(AuraEffect const* l_AuraEffect, int32& l_Amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Owner = l_Caster->GetOwner())
                    {
                        if (l_AuraEffect->GetAmplitude() && GetMaxDuration())
                            l_Amount = int32(l_Owner->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 0.6f * 2.0f);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_spirit_mend_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_spirit_mend_AuraScript();
        }

        class spell_hun_spirit_mend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_spirit_mend_SpellScript);

            void HandleHeal(SpellEffIndex l_Idx)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner  = l_Caster->GetOwner();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || l_Owner == nullptr)
                    return;

                int32 l_Heal = int32(l_Owner->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 0.6f * 3.0f);
                l_Heal = l_Owner->SpellHealingBonusDone(l_Target, GetSpellInfo(), l_Heal, l_Idx, SPELL_DIRECT_DAMAGE);
                l_Heal = l_Target->SpellHealingBonusTaken(l_Caster, GetSpellInfo(), l_Heal, SPELL_DIRECT_DAMAGE);
                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_spirit_mend_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_spirit_mend_SpellScript();
        }
};

/// Poisoned Ammo - 162543
class spell_hun_poisoned_ammo : public SpellScriptLoader
{
    public:
        spell_hun_poisoned_ammo() : SpellScriptLoader("spell_hun_poisoned_ammo") { }

        class spell_hun_poisoned_ammo_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_poisoned_ammo_SpellScript);

            enum eSpells
            {
                PoisonedAmmo = 162543
            };

            int32 m_Damage = 0;

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (Aura* l_PoisonedAmmo = l_Target->GetAura(eSpells::PoisonedAmmo))
                        {
                            if (m_Damage > 0)
                                m_Damage = m_Damage / (l_PoisonedAmmo->GetDuration() / l_PoisonedAmmo->GetEffect(0)->GetAmplitude());

                            int32 l_BasicDamage = int32(0.046f * l_Caster->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack));
                            l_BasicDamage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_BasicDamage, SpellEffIndex::EFFECT_0, DOT, l_PoisonedAmmo->GetEffect(0)->GetBase()->GetStackAmount());
                            l_BasicDamage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_BasicDamage, DOT, SpellEffIndex::EFFECT_0, l_PoisonedAmmo->GetEffect(0)->GetBase()->GetStackAmount());

                            m_Damage += l_BasicDamage;

                            l_PoisonedAmmo->GetEffect(0)->SetAmount(m_Damage);
                        }
                    }
                }
            }

            void HandleBeforeHit()
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    if (Aura* l_PoisonedAmmo = l_Target->GetAura(eSpells::PoisonedAmmo))
                        m_Damage = l_PoisonedAmmo->GetEffect(0)->GetAmount() * (l_PoisonedAmmo->GetDuration() / l_PoisonedAmmo->GetEffect(0)->GetAmplitude());
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_hun_poisoned_ammo_SpellScript::HandleBeforeHit);
                OnHit += SpellHitFn(spell_hun_poisoned_ammo_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_poisoned_ammo_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Adaptation - 152244
class spell_hun_adaptation : public SpellScriptLoader
{
    public:
        spell_hun_adaptation() : SpellScriptLoader("spell_hun_adaptation") { }

        class spell_hun_adaptation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_adaptation_AuraScript);

            enum eSpells
            {
                CombatExperienceAdaptation  = 156843,
                CombatExperience            = 20782
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                    {
                        if (l_Pet->HasSpell(eSpells::CombatExperience))
                        {
                            l_Pet->RemoveAura(eSpells::CombatExperience);
                            l_Pet->CastSpell(l_Pet, eSpells::CombatExperienceAdaptation, true);
                        }
                    }
                }
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                    {
                        l_Pet->RemoveAura(eSpells::CombatExperienceAdaptation);
                        if (l_Pet->HasSpell(eSpells::CombatExperience))
                            l_Pet->CastSpell(l_Pet, eSpells::CombatExperience, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_adaptation_AuraScript::OnApply, EFFECT_0, SPELL_AURA_OVERRIDE_PET_SPECS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_adaptation_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_PET_SPECS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_adaptation_AuraScript();
        }
};

/// Update to Legion 7.3.5 26365
/// Called By Aimed Shot - 19434
class spell_hun_aimed_shot : public SpellScriptLoader
{
    public:
        spell_hun_aimed_shot() : SpellScriptLoader("spell_hun_aimed_shot") { }

        class spell_hun_aimed_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_aimed_shot_SpellScript);

            enum eSpells
            {
                T17Marksmanship2P           = 165519,
                AimedShotEnergize           = 167153,
                RapidFire                   = 3045,
                WodPvpMM4PBonus             = 170884,
                TrickShot                   = 199522,
                TrickShotEffect             = 199885,
                AimedShotMarker             = 236641,
                SentinelsSight              = 208913,
                MKIIGyroscopicStabilizer    = 235691,
                GyroscopicStabilization     = 235712,
                T20Marksmanship2P           = 242242,
                CriticalAimed               = 242243,
                T20Marksmanship4P           = 242241,
                Precision                   = 246153
            };

            bool m_DoubleUse = false;

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::LastAimedShotTarget) = l_Target->GetGUID();

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::T20Marksmanship2P))
                    if (l_Player->GetLastCastedSpell() == GetSpellInfo()->Id)
                        m_DoubleUse = true;

                if (l_Player->HasAura(eSpells::T20Marksmanship4P))
                    l_Player->CastSpell(l_Player, eSpells::Precision, true);

                if (l_Player->HasAura(eSpells::WodPvpMM4PBonus))
                    if (l_Player->HasSpellCooldown(eSpells::RapidFire))
                        l_Player->ReduceSpellCooldown(eSpells::RapidFire, 5000);

                l_Player->RemoveAura(eSpells::SentinelsSight);
                if (!l_Player->HasAura(eSpells::MKIIGyroscopicStabilizer))
                    return;

                if (l_Player->HasAura(eSpells::GyroscopicStabilization) || l_Player->InBattleground())
                    l_Player->RemoveAura(eSpells::GyroscopicStabilization);
                else
                    l_Player->CastSpell(l_Player, eSpells::GyroscopicStabilization, true);

            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                if (m_DoubleUse)
                    l_Caster->CastSpell(l_Caster, eSpells::CriticalAimed, true);

                /// Aimed Shot critical strikes restore 8 additional Focus.
                if (GetSpell()->IsCritForTarget(l_Target) && l_Caster->HasAura(eSpells::T17Marksmanship2P))
                    l_Caster->CastSpell(l_Caster, eSpells::AimedShotEnergize, true);

                std::set<uint64>& l_TargetsHit = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShot];
                if (!l_Target->HasAura(eSpells::AimedShotMarker) && std::find(l_TargetsHit.begin(), l_TargetsHit.end(), l_Target->GetGUID()) == l_TargetsHit.end())
                {
                    int32 l_Damage = GetHitDamage();
                    AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_2].BasePoints);
                    SetHitDamage(l_Damage);
                }

                /// Trick Shot
                Aura* l_TrickShot = l_Caster->GetAura(eSpells::TrickShot);
                if (!l_TrickShot)
                    return;

                int32 l_RicochetDamage = CalculatePct(GetHitDamage(), l_TrickShot->GetEffect(0)->GetAmount());
                l_TrickShot->m_SpellHelper.GetBool(eSpellHelpers::TrickShot) = GetSpell()->IsCritForTarget(l_Target);
                l_TrickShot->m_SpellHelper.GetUint32(eSpellHelpers::TrickShot) = l_RicochetDamage;
                l_Caster->CastSpell(l_Target, eSpells::TrickShotEffect, true);
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster || l_Target->HasAura(eSpells::AimedShotMarker))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AimedShotMarker, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_aimed_shot_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_hun_aimed_shot_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
                AfterHit += SpellHitFn(spell_hun_aimed_shot_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_aimed_shot_SpellScript();
        }
};

/// 7.3.5 - Build 26365
/// Called for Item MKII Gyroscopic Stabilizer - 144303
class PlayerScript_gyroscopic_stabilizer : public PlayerScript
{
    public:
        PlayerScript_gyroscopic_stabilizer() : PlayerScript("PlayerScript_gyroscopic_stabilizer") {}

        enum eItems
        {
            GryoscopicStabilizer = 144303
        };

        enum eSpells
        {
            GyroscopicStabilization = 235712
        };

        void OnAfterUnequipItem(Player* p_Player, Item const* p_Item) override
        {
            if (!p_Player || !p_Item || p_Item->GetEntry() != eItems::GryoscopicStabilizer)
                return;

            p_Player->RemoveAura(GyroscopicStabilization);
        }

        void OnUpdateZone(Player* p_Player, uint32 /*p_NewZoneID*/, uint32 /*p_OldZoneID*/, uint32 /*p_NewAreaID*/) override
        {
            if (!p_Player || !p_Player->HasAura(eSpells::GyroscopicStabilization))
                return;

            if (p_Player->GetMap()->IsBattlegroundOrArena())
            {
                p_Player->RemoveAura(eSpells::GyroscopicStabilization);
            }
        }
};

/// last update : 6.1.2 19802
/// Thick Hide - 160057
class spell_hun_thick_hide : public SpellScriptLoader
{
    public:
        spell_hun_thick_hide() : SpellScriptLoader("spell_hun_thick_hide") { }

        class spell_hun_thick_hide_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_thick_hide_AuraScript);

            enum eSpells
            {
                ThickHideEffect = 160058
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                Pet* l_Pet = l_Caster->ToPet();

                if (l_Pet == nullptr)
                    return;

                if (l_Pet->HasSpellCooldown(eSpells::ThickHideEffect))
                    return;

                if (l_Pet->GetHealthPct() > (float)GetSpellInfo()->Effects[EFFECT_1].BasePoints)
                    return;

                l_Pet->CastSpell(l_Pet, eSpells::ThickHideEffect, true);
                l_Pet->_AddCreatureSpellCooldown(eSpells::ThickHideEffect, time(nullptr) + GetSpellInfo()->Effects[EFFECT_2].BasePoints);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_thick_hide_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_thick_hide_AuraScript();
        }
};

/// Called by Rapid Fire - 3045
/// Item - Hunter T17 Marksmanship 4P Bonus - 165525
class spell_hun_t17_marksmanship_4p : public SpellScriptLoader
{
    public:
        spell_hun_t17_marksmanship_4p() : SpellScriptLoader("spell_hun_t17_marksmanship_4p") { }

        class spell_hun_t17_marksmanship_4p_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_t17_marksmanship_4p_SpellScript);

            enum eSpells
            {
                T17Marksmanship4P   = 165525,
                DeadlyAimDriver     = 170186
            };

            void HandleOnHit()
            {
                /// While Rapid Fire is active, your critical strike damage increases by 3% per second.
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::T17Marksmanship4P))
                        l_Caster->CastSpell(l_Caster, eSpells::DeadlyAimDriver, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_t17_marksmanship_4p_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_t17_marksmanship_4p_SpellScript();
        }
};

//////////////////////////////////////////////
////////////////              ////////////////
////////////////    LEGION    ////////////////
////////////////              ////////////////
//////////////////////////////////////////////

/// Last Update 7.0.3 Build 22522
/// Called by Wild call 185791
class spell_hun_wild_call : public SpellScriptLoader
{
    public:
        spell_hun_wild_call() : SpellScriptLoader("spell_hun_wild_call") { }

        class spell_hun_wild_call_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_wild_call_SpellScript);

            enum eCategories
            {
                DireBeast   = 1674,
                DireFrenzy  = 1675
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                for (uint16 l_I = eCategories::DireBeast; l_I <= eCategories::DireFrenzy; ++l_I)
                {
                    SpellCategoryEntry const* l_Category = sSpellCategoryStore.LookupEntry(l_I);
                    if (l_Category == nullptr)
                        continue;

                    l_Player->RestoreCharge(l_Category);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_wild_call_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_wild_call_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Wild Call - 185789
class spell_hun_wild_call_proc : public SpellScriptLoader
{
    public:
        spell_hun_wild_call_proc() : SpellScriptLoader("spell_hun_wild_call_proc") { }

        class spell_hun_wild_call_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_wild_call_proc_AuraScript);

            enum eSpells
            {
                AutoShot = 75
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!(p_ProcEventInfo.GetHitMask() & PROC_HIT_CRITICAL) || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::AutoShot)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_wild_call_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_wild_call_proc_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Dire Beast - 120679
class spell_hun_dire_beast : public SpellScriptLoader
{
    public:
        spell_hun_dire_beast() : SpellScriptLoader("spell_hun_dire_beast") { }

        class spell_hun_dire_beast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_dire_beast_SpellScript);

            enum eSpells
            {
                DireBeastJadeForest             = 121118,
                DireBeastKalimdor               = 122802,
                DireBeastEasternKingdoms        = 122804,
                DireBeastOutland                = 122806,
                DireBeastNorthrend              = 122807,
                DireBeastKrasarangWilds         = 122809,
                DireBeastValleyOfTheFourWinds   = 122811,
                DireBeastValeOfTheEternalBlossom= 126213,
                DireBeastKunLaiSummit           = 126214,
                DireBeastTownlongSteppes        = 126215,
                DireBeastDreadWastes            = 126216,
                DireBeastIsleOfGiants           = 138574,
                DireBeastIsleOfThunder          = 138580,
                DireBeastThroneOfThunder        = 139226,
                DireBeastSiegeOfOrgrimmar       = 139227,
                DireBeastTimelessIsle           = 149365,
                DireBeastGorgrond               = 170357,
                DireBeastFrostfireRidge         = 170358,
                DireBeastAshran                 = 170359,
                DireBeastTanaanJungle           = 170360,
                DireBeastDraenorNagrand         = 170361,
                DireBeastTalador                = 170362,
                DireBeastDraenorShadowmoonValley= 170363,
                DireBeastSpiresOfArak           = 170364,
                DireBeastBrokenIsles            = 224573,
                DireBeastAzsuna                 = 224574,
                DireBeastValsharah              = 224575,
                DireBeastSuramar                = 224576,
                DireBeastStormheim              = 224577,
                DireBeastHighmountain           = 224578,
                DireBeastArgus                  = 255283,

                WildCall                        = 185791,
                BestialWrath                    = 19574,
                DireBeastDungeons               = 132764,
                DireBeastFocusRegen             = 120694,
                DireBeastDamageBuff             = 215778,
                StompAura                       = 199530,
                StompDamage                     = 201754,
                EredunWarOrder                  = 212278,
                KillCommand                     = 34026,
                TheMantleofCommand              = 235721,
                TheMantleofCommandTrigger       = 247993,
                T19BeastMastery2P               = 211172,
                GlyphOfArachnophobia            = 212407,
                DireBeastArachnophobia          = 212397,
                DireBeastArachnophobia2         = 212396,
                GlyphOfNesingwarysNemeses       = 212410,
                DireBeastNemeses                = 212386,
                DireBeastNemeses2               = 204432,
                DireBeastNemeses3               = 212382,
                GlyphOfTheDireStable            = 219183,
                T19BeastMastery4P               = 211181,
                BestialWrathBonus               = 211183
            };

            const std::vector<uint32> m_NPCEntry = { 62856, 62855, 62857, 62858, 62005, /*Arachnophobia (2)*/106989, 106988, /*Nemeses (3)*/106975, 103268, 106972 };

            void HandleAfterCast()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BestialWrath))
                    {
                        if (l_Player->HasSpellCooldown(eSpells::BestialWrath))
                        {
                            int32 l_Duration = l_SpellInfo->Effects[EFFECT_2].BasePoints;
                            /// Item - Hunter T19 Beast Mastery 2P
                            if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::T19BeastMastery2P, EFFECT_0))
                                l_Duration += l_AuraEffect->GetAmount();
                            l_Player->ReduceSpellCooldown(eSpells::BestialWrath, l_Duration * IN_MILLISECONDS);
                        }
                    }
                }
            }

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::WildCall))
                        l_Player->RemoveAura(eSpells::WildCall);

                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Player->HasAura(eSpells::GlyphOfArachnophobia))
                        {
                            l_Player->CastSpell(l_Target, RAND(eSpells::DireBeastArachnophobia, eSpells::DireBeastArachnophobia2), true);
                        }
                        else if (l_Player->HasAura(eSpells::GlyphOfNesingwarysNemeses))
                        {
                            l_Player->CastSpell(l_Target, RAND(eSpells::DireBeastNemeses, eSpells::DireBeastNemeses2, eSpells::DireBeastNemeses3), true);
                        }
                        else
                        {
                            switch (l_Player->GetZoneId())
                            {
                                case 5785: // The Jade Forest
                                    l_Player->CastSpell(l_Target, DireBeastJadeForest, true);
                                    break;
                                case 5805: // Valley of the Four Winds
                                    l_Player->CastSpell(l_Target, DireBeastValleyOfTheFourWinds, true);
                                    break;
                                case 5840: // Vale of Eternal Blossoms
                                    l_Player->CastSpell(l_Target, DireBeastValeOfTheEternalBlossom, true);
                                    break;
                                case 5841: // Kun-Lai Summit
                                    l_Player->CastSpell(l_Target, DireBeastKunLaiSummit, true);
                                    break;
                                case 5842: // Townlong Steppes
                                    l_Player->CastSpell(l_Target, DireBeastTownlongSteppes, true);
                                    break;
                                case 6134: // Krasarang Wilds
                                    l_Player->CastSpell(l_Target, DireBeastKrasarangWilds, true);
                                    break;
                                case 6138: // Dread Wastes
                                    l_Player->CastSpell(l_Target, DireBeastDreadWastes, true);
                                    break;
                                case 6662: // Talador 
                                    l_Player->CastSpell(l_Target, DireBeastTalador, true);
                                    break;
                                case 6719: // Frostfire Ridge
                                    l_Player->CastSpell(l_Target, DireBeastDraenorShadowmoonValley, true);
                                    break;
                                case 6720: // Frostfire Ridge
                                    l_Player->CastSpell(l_Target, DireBeastFrostfireRidge, true);
                                    break;
                                case 6721: // Gorgrond
                                    l_Player->CastSpell(l_Target, DireBeastGorgrond, true);
                                    break;
                                case 6722: // Spires of Arak
                                    l_Player->CastSpell(l_Target, DireBeastSpiresOfArak, true);
                                    break;
                                case 6755: // Nagrand 
                                    l_Player->CastSpell(l_Target, DireBeastDraenorNagrand, true);
                                    break;
                                case 6941: // Ashran 
                                    l_Player->CastSpell(l_Target, DireBeastAshran, true);
                                    break;
                                case 7503: // Highmountain
                                    l_Player->CastSpell(l_Target, DireBeastHighmountain, true);
                                    break;
                                case 7541: // Stormheim
                                    l_Player->CastSpell(l_Target, DireBeastStormheim, true);
                                    break;
                                case 7558: // Valsharah
                                    l_Player->CastSpell(l_Target, DireBeastValsharah, true);
                                    break;
                                case 7637: // Suramar
                                    l_Player->CastSpell(l_Target, DireBeastSuramar, true);
                                    break;
                                case 7734: // Azsuna
                                case 7578: // Eye of Azshara
                                    l_Player->CastSpell(l_Target, DireBeastAzsuna, true);
                                    break;
                                default:
                                {
                                    switch (l_Player->GetMapId())
                                    {
                                        case 0: // Eastern Kingdoms
                                            l_Player->CastSpell(l_Target, DireBeastEasternKingdoms, true);
                                            break;
                                        case 1: // Kalimdor
                                            l_Player->CastSpell(l_Target, DireBeastKalimdor, true);
                                            break;
                                        case 8: // Outland
                                        case 530:
                                            l_Player->CastSpell(l_Target, DireBeastOutland, true);
                                            break;
                                        case 10: // Northrend
                                            l_Player->CastSpell(l_Target, DireBeastNorthrend, true);
                                            break;
                                        case 1152: // Garrisons Horde
                                        case 1153:
                                        case 1154:
                                            l_Player->CastSpell(l_Target, DireBeastFrostfireRidge, true);
                                            break;
                                        case 1158: // Garrisons Alliance
                                        case 1159:
                                        case 1160:
                                            l_Player->CastSpell(l_Target, DireBeastDraenorShadowmoonValley, true);
                                            break;
                                        case 1464: // Tanaan Jungle
                                            l_Player->CastSpell(l_Target, DireBeastTanaanJungle, true);
                                            break;
                                        case 1220: // Broken Isle
                                        case 1502: // Broken Isle (Dalaran sewers)
                                            l_Player->CastSpell(l_Target, DireBeastBrokenIsles, true);
                                            break;
                                        case 1669: // Argus
                                        case 1779: // Argus invasion points
                                        case 1712: // Antorus
                                            l_Player->CastSpell(l_Target, DireBeastArgus, true);
                                            break;
                                        default:
                                            l_Player->CastSpell(l_Target, DireBeastDungeons, true);
                                            break;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            void HandleAfterHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::EredunWarOrder))
                    {
                        SpellInfo const* l_EredunWarOrderInfo = sSpellMgr->GetSpellInfo(eSpells::EredunWarOrder);

                        if (l_EredunWarOrderInfo)
                            l_Player->ModifySpellCooldown(eSpells::KillCommand, -l_EredunWarOrderInfo->Effects[0].BasePoints);
                    }

                    /// Legendary The Mantle of Command
                    if (l_Player->HasAura(eSpells::TheMantleofCommand))
                        l_Player->CastSpell(l_Player, eSpells::TheMantleofCommandTrigger, true);

                    if (Unit* l_Target = GetHitUnit())
                    {
                        auto& direBeasts = sSpellMgr->GetDireBeastEntries();

                        std::list<Unit*> l_TmpUnitList;
                        for (auto l_Itr : l_Player->m_Controlled)
                        {
                            if (direBeasts.find(l_Itr->GetEntry()) != direBeasts.end() || std::find(m_NPCEntry.begin(), m_NPCEntry.end(), l_Itr->GetEntry()) != m_NPCEntry.end())
                                l_TmpUnitList.push_back(l_Itr);
                        }

                        TempSummon* l_DireBeast = nullptr;
                        for (Unit* l_Summon : l_TmpUnitList)
                        {
                            if (!l_DireBeast || (l_Summon && l_Summon->ToTempSummon() && l_Summon->ToTempSummon()->GetTimer() > l_DireBeast->GetTimer()))
                                l_DireBeast = l_Summon->ToTempSummon();
                        }

                        if (!l_DireBeast)
                            return;

                        l_DireBeast->AddAura(eSpells::DireBeastDamageBuff, l_DireBeast);

                        if (l_Player->HasAura(eSpells::StompAura))
                            l_DireBeast->CastSpell(l_DireBeast, eSpells::StompDamage, true);

                        l_DireBeast->AddAura(eSpells::DireBeastFocusRegen, l_Player);

                        if (l_Player->HasAura(eSpells::T19BeastMastery4P))
                        {
                            if (Aura* l_Aura = l_Player->GetAura(BestialWrath))
                            {
                                l_Player->CastSpell(l_DireBeast, eSpells::BestialWrathBonus, true);
                                uint32 l_RemainingDuration = l_Aura->GetDuration();
                                Aura* l_BestialWrathBonus = l_DireBeast->GetAura(eSpells::BestialWrathBonus);
                                if (!l_BestialWrathBonus || !l_RemainingDuration)
                                    return;

                                l_BestialWrathBonus->SetDuration(l_RemainingDuration);
                            }
                        }

                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_dire_beast_SpellScript::HandleAfterCast);
                OnHit += SpellHitFn(spell_hun_dire_beast_SpellScript::HandleOnHit);
                AfterHit += SpellHitFn(spell_hun_dire_beast_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_dire_beast_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Kill Command - 34026
class spell_hun_kill_command : public SpellScriptLoader
{
    public:
        spell_hun_kill_command() : SpellScriptLoader("spell_hun_kill_command") { }

        class spell_hun_kill_command_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_kill_command_SpellScript);

            enum eSpells
            {
                T17BeastMaster2P            = 165517,
                BestialWrath                = 19574,
                KillCommand                 = 34026,
                KillCommandTrigger          = 83381,
                KillCommandCharge           = 118171,
                MasterOfBeast               = 197248,
                T20Beast2PBonus             = 246126,
                AspectOfTheWild             = 193530,
                T21BeastMastery4P           = 251756
            };

            bool Validate(SpellInfo const* /*SpellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::KillCommand))
                    return false;
                return true;
            }

            SpellCastResult CheckCastMeet()
            {
                Unit* l_Pet = GetCaster()->GetGuardianPet();
                Unit* l_PetTarget = GetExplTargetUnit();

                if (!l_Pet || l_Pet->isDead())
                    return SPELL_FAILED_NO_PET;

                SpellInfo const* l_KillCommandInfo = sSpellMgr->GetSpellInfo(m_scriptSpellId);
                if (!l_KillCommandInfo)
                    return SPELL_FAILED_DONT_REPORT;

                uint32 l_DistCheck = l_KillCommandInfo->Effects[EFFECT_2].BasePoints;
                if (l_DistCheck <= 0)
                    return SPELL_FAILED_DONT_REPORT;

                if (!l_PetTarget)
                    return SPELL_FAILED_BAD_TARGETS;
                if (!l_Pet->IsWithinDist(l_PetTarget, l_DistCheck, true))
                    return SPELL_FAILED_OUT_OF_RANGE;

                if (!l_PetTarget->IsWithinLOSInMap(l_Pet))
                    return SPELL_FAILED_LINE_OF_SIGHT;

                if (l_Pet->HasAuraType(SPELL_AURA_MOD_STUN) || l_Pet->HasAuraType(SPELL_AURA_MOD_CONFUSE) || l_Pet->HasAuraType(SPELL_AURA_MOD_SILENCE) ||
                    l_Pet->HasAuraType(SPELL_AURA_MOD_FEAR) || l_Pet->HasAuraType(SPELL_AURA_MOD_FEAR_2))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    /// Kill Command has a chance to reset the cooldown of Bestial Wrath.
                    if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::T17BeastMaster2P, EFFECT_0))
                    {
                        if (l_Player->HasSpellCooldown(eSpells::BestialWrath) && roll_chance_i(12))
                            l_Player->RemoveSpellCooldown(eSpells::BestialWrath, true);
                    }

                    if (l_Player->HasAura(eSpells::T21BeastMastery4P))
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21BeastMastery4P))
                            if (l_Player->HasSpellCooldown(eSpells::AspectOfTheWild))
                                l_Player->ReduceSpellCooldown(eSpells::AspectOfTheWild, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                    }

                    if (Unit* l_PetUnit = GetCaster()->GetGuardianPet())
                    {
                        if (!l_PetUnit)
                            return;

                        Unit* l_Target = GetExplTargetUnit();
                        if (!l_Target)
                            return;

                        l_PetUnit->CastSpell(l_Target, eSpells::KillCommandTrigger, true);
                        if (l_Player->HasAura(eSpells::MasterOfBeast))
                        {
                            if (Creature* l_Hati = l_Player->GetHati())
                                l_Hati->CastSpell(l_Target, eSpells::KillCommandTrigger, true);
                        }

                        if (l_PetUnit->getVictim())
                        {
                            l_PetUnit->AttackStop();
                            l_PetUnit->ToCreature()->AI()->AttackStart(l_Target);
                        }
                        else
                            l_PetUnit->ToCreature()->AI()->AttackStart(l_Target);

                        l_PetUnit->CastSpell(l_Target, eSpells::KillCommandCharge, true);
                        if (l_Player->HasAura(eSpells::MasterOfBeast))
                        {
                            if (Creature* l_Hati = l_Player->GetHati())
                                l_Hati->CastSpell(l_Target, eSpells::KillCommandCharge, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_kill_command_SpellScript::CheckCastMeet);
                OnEffectHit += SpellEffectFn(spell_hun_kill_command_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_kill_command_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Cobra Shot - 193455
class spell_hun_cobra_shot : public SpellScriptLoader
{
    public:
        spell_hun_cobra_shot() : SpellScriptLoader("spell_hun_cobra_shot") { }

        class spell_hun_cobra_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_cobra_shot_SpellScript);

            enum eSpells
            {
                KillerCobra     = 199532,
                BestialWrath    = 19574,
                KillCommand     = 34026,
                WayOfTheCobra   = 194397,
                Stampede        = 201430
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::KillerCobra) && l_Player->HasAura(eSpells::BestialWrath))
                    l_Player->RemoveSpellCooldown(eSpells::KillCommand, true);
            }

            void HandleWayOfTheCobra()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::WayOfTheCobra))
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WayOfTheCobra);
                    int32 l_Damage = GetHitDamage();
                    if (!l_SpellInfo)
                        return;

                    /// Revamp to only take into account "real" pets (no toys etc) was previously taking m_Controlled.size() without checking (7.1.5 - 23420)
                    uint8 l_Count = 0;
                    for (Unit const* l_Unit : l_Caster->m_Controlled)
                    {
                        if (l_Unit->isPet() || (l_Unit->ToCreature() && (l_Unit->ToCreature()->IsDireBeast() || l_Unit->ToCreature()->IsHati())))
                            l_Count++;
                    }

                    if (l_Caster->HasAura(eSpells::Stampede))
                        l_Count++;

                    l_Damage = AddPct(l_Damage, l_SpellInfo->Effects[0].BasePoints * l_Count);
                    SetHitDamage(l_Damage);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_cobra_shot_SpellScript::HandleWayOfTheCobra);
                OnEffectHit += SpellEffectFn(spell_hun_cobra_shot_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_cobra_shot_SpellScript();
        }
};

/// Last Update 7.0.3
/// Piercing Shot - 198670
class spell_hun_piercing_shot : public SpellScriptLoader
{
    public:
        spell_hun_piercing_shot() : SpellScriptLoader("spell_hun_piercing_shot") { }

        class spell_hun_piercing_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_piercing_shot_SpellScript);

            enum eSpells
            {
                PiercingShotDamage = 213678
            };

            uint64 m_MainTargetGUID = 0;
            uint32 m_PowerPercentage = 0;

            void HandleOnCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (l_Target == nullptr)
                    return;

                m_MainTargetGUID = l_Target->GetGUID();
            }

            void HandleEffectDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::PiercingShotDamage);

                if (!l_SpellInfo)
                    return;

                int32 l_Bp = l_Target->GetGUID() == m_MainTargetGUID ? (l_SpellInfo->Effects[EFFECT_1].BasePoints * 2) : l_SpellInfo->Effects[EFFECT_1].BasePoints;
                int32 l_Damage = CalculatePct(l_Bp, m_PowerPercentage);

                l_Caster->CastCustomSpell(l_Target, eSpells::PiercingShotDamage, nullptr, &l_Damage, nullptr, true);
            }

            void HandleOnHit()
            {
                int32 l_Damage = GetHitDamage();

                l_Damage = CalculatePct(l_Damage, m_PowerPercentage);
                SetHitDamage(l_Damage);
            }

            void HandleTakePowers(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != Powers::POWER_FOCUS)
                    return;

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                const SpellPowerEntry* l_PowerEntry = nullptr;

                if (!l_Caster || !l_SpellInfo || !(l_PowerEntry = l_SpellInfo->GetSpellPower(POWER_FOCUS)))
                    return;

                int32 l_MaxPowerCost = l_PowerEntry->Cost + l_PowerEntry->CostAdditional;
                int32 l_TotalFocus = l_Caster->GetPower(POWER_FOCUS);
                int32 l_ToUse = 0;

                if (l_TotalFocus < l_MaxPowerCost)
                    l_ToUse = l_TotalFocus;
                else
                    l_ToUse = l_MaxPowerCost;

                m_PowerPercentage = static_cast<uint32>((static_cast<float>(l_ToUse) / l_MaxPowerCost) * 100); ///< Calculate percentage of the max power cost used

                p_PowerCost = l_ToUse;
            }

            void FilterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_MainTarget = GetExplTargetUnit();

                if (!l_Caster || !l_MainTarget)
                    return;

                int32 l_Dist = l_Caster->GetDistance2d(l_MainTarget);

                p_Targets.remove_if([this, l_Caster, l_MainTarget, l_Dist](WorldObject* p_Target) -> bool
                {
                    if (p_Target == nullptr || p_Target->ToUnit() == nullptr)
                        return true;

                    if (l_MainTarget != p_Target && l_Caster->GetDistance2d(p_Target) > l_Dist)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_hun_piercing_shot_SpellScript::HandleTakePowers);
                OnCast += SpellCastFn(spell_hun_piercing_shot_SpellScript::HandleOnCast);
                OnHit += SpellHitFn(spell_hun_piercing_shot_SpellScript::HandleOnHit);
                OnEffectHitTarget += SpellEffectFn(spell_hun_piercing_shot_SpellScript::HandleEffectDamage, EFFECT_1, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_piercing_shot_SpellScript::FilterTarget, EFFECT_1, TARGET_ENNEMIES_IN_CYLINDER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_piercing_shot_SpellScript::FilterTarget, EFFECT_2, TARGET_ENNEMIES_IN_CYLINDER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_piercing_shot_SpellScript::FilterTarget, EFFECT_3, TARGET_ENNEMIES_IN_CYLINDER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_piercing_shot_SpellScript::FilterTarget, EFFECT_4, TARGET_ENNEMIES_IN_CYLINDER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_piercing_shot_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Bestial Wrath - 19574
class spell_hun_bestial_wrath: public SpellScriptLoader
{
    public:
        spell_hun_bestial_wrath() : SpellScriptLoader("spell_hun_bestial_wrath") { }

        class spell_hun_bestial_wrath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_bestial_wrath_SpellScript);

            enum eSpells
            {
                T19BeastMastery4P   = 211181,
                T20BeastMastery4P   = 242240,
                BestialRage         = 246116,
                BestialWrathBonus   = 211183,
                DireFrenzy          = 217200
            };

            void HandleAfterCast()
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::T20BeastMastery4P))
                    l_Caster->CastSpell(l_Caster, eSpells::BestialRage, true);

                if (l_Caster->HasAura(eSpells::T19BeastMastery4P))
                {
                    /// +10% damage on Pet + Hati
                    if (l_Caster->HasSpell(eSpells::DireFrenzy))
                    {
                        if (Creature* l_Hati = l_Caster->GetHati())
                            l_Caster->CastSpell(l_Hati, eSpells::BestialWrathBonus, true);

                        if (Pet* l_Pet = l_Caster->GetPet())
                            l_Caster->CastSpell(l_Pet, eSpells::BestialWrathBonus, true);
                    }
                    /// +50% damage on all Dire Beasts
                    else
                    {
                        for (Unit* l_Summon : l_Caster->m_Controlled)
                        {
                            if (l_Summon->ToCreature() && l_Summon->ToCreature()->IsDireBeast())
                                l_Caster->CastSpell(l_Summon, eSpells::BestialWrathBonus, true);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_bestial_wrath_SpellScript::HandleAfterCast);
            }
        };

        class spell_hun_bestial_wrath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_bestial_wrath_AuraScript);

            enum eSpells
            {
                T17BeastMaster4P        = 165518,
                BestialWrathStampede    = 167135,
                BestialWrathHati        = 207033,
                MasterOfBeast           = 197248,
                RoarOfTheSevenLions     = 207280,
                RoarOfTheSevenLionsEff  = 207318
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    /// While Bestial Wrath is active, one additional pet is summoned to fight with you.
                    if (Unit* l_Caster = GetCaster())
                    {
                        if (Player* l_Player = l_Caster->ToPlayer())
                        {
                            if (l_Player->HasAura(eSpells::MasterOfBeast))
                            {
                                if (Creature* l_Hati = l_Player->GetHati())
                                    l_Player->CastSpell(l_Hati, eSpells::BestialWrathHati, true);
                            }
                            if (l_Player->HasAura(eSpells::RoarOfTheSevenLions))
                                l_Player->CastSpell(l_Caster, eSpells::RoarOfTheSevenLionsEff, true);
                        }
                        if (l_Caster->IsPlayer() && l_Caster->HasAura(eSpells::T17BeastMaster4P))
                        {
                            Unit* l_Victim = l_Caster->ToPlayer()->GetSelectedUnit();
                            if (!l_Victim || !l_Caster->IsValidAttackTarget(l_Victim))
                                l_Victim = l_Caster->getVictim();
                            if (!l_Victim || !l_Caster->IsValidAttackTarget(l_Victim))
                                l_Victim = l_Caster;

                            l_Caster->CastSpell(l_Victim, eSpells::BestialWrathStampede, true);
                        }
                    }
                }
            }

            void OnEffectRemove(AuraEffect const* /*p_AuraEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::RoarOfTheSevenLionsEff))
                        l_Caster->RemoveAura(eSpells::RoarOfTheSevenLionsEff);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_bestial_wrath_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_bestial_wrath_AuraScript::OnEffectRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_bestial_wrath_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_bestial_wrath_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Bestial Wrath - 211183
class spell_hun_bestial_wrath_aura : public SpellScriptLoader
{
    public:
        spell_hun_bestial_wrath_aura() : SpellScriptLoader("spell_hun_bestial_wrath_aura") { }

        class spell_hun_bestial_wrath_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_bestial_wrath_aura_AuraScript);

            enum eSpells
            {
                T19BeastMastery4P   = 211181,
                DireFrenzy          = 217200
            };

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                if (!GetCaster())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::T19BeastMastery4P))
                {
                    if (l_Player->HasSpell(eSpells::DireFrenzy))
                        p_Amount = GetSpellInfo()->Effects[EFFECT_1].BasePoints;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_bestial_wrath_aura_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_bestial_wrath_aura_AuraScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Called By Arcane Shot 185358 - MultiShot 2643
/// Called For Marking Targets - 223138
class spell_hunter_marking_targets : public SpellScriptLoader
{
public:
    spell_hunter_marking_targets() : SpellScriptLoader("spell_hunter_marking_targets") { }

    class spell_hunter_marking_targets_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hunter_marking_targets_SpellScript);

        enum eSpells
        {
            ArcaneShot      = 185358,
            MarkingTargets  = 223138,
            HuntersMark     = 185365,
            CasterMark      = 185743,
            Trueshot        = 193526,
            BoP             = 1022
        };

        void FilterTargetAS(WorldObject*& p_Target) /// Arcane Shot
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !(l_Caster->HasAura(eSpells::MarkingTargets) || l_Caster->HasAura(eSpells::Trueshot)))
                return;

            Unit* l_Target = p_Target->ToUnit();
            if (!l_Target || l_Target->IsImmunedToSpell(GetSpellInfo()))
                return;

            l_Caster->AddAura(eSpells::HuntersMark, l_Target);
            l_Caster->CastSpell(l_Caster, eSpells::CasterMark, true);
        }


        void FilterTarget(std::list<WorldObject*>& p_Targets) /// MultiShot
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !(l_Caster->HasAura(eSpells::MarkingTargets) || l_Caster->HasAura(eSpells::Trueshot)))
                return;

            uint32 l_NbTargetHit = 0;

            for (WorldObject* l_Target : p_Targets)
            {
                Unit* l_UnitTarget = l_Target->ToUnit();
                if (!l_UnitTarget || l_UnitTarget->IsImmunedToSpell(GetSpellInfo()) || l_UnitTarget->HasAura(eSpells::BoP))
                    continue;

                l_Caster->AddAura(eSpells::HuntersMark, l_UnitTarget);
                l_NbTargetHit++;
            }

            if (l_NbTargetHit > 0)
                l_Caster->CastSpell(l_Caster, eSpells::CasterMark, true);
        }

        void HandleOnCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;
            l_Caster->RemoveAura(eSpells::MarkingTargets);
        }


        void Register() override
        {
            OnCast += SpellCastFn(spell_hunter_marking_targets_SpellScript::HandleOnCast);
            if (m_scriptSpellId == eSpells::ArcaneShot)
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_hunter_marking_targets_SpellScript::FilterTargetAS, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
            else
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunter_marking_targets_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_hunter_marking_targets_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Marked Shot - 185901
class spell_hun_marked_shot : public SpellScriptLoader
{
    public:
        spell_hun_marked_shot() : SpellScriptLoader("spell_hun_marked_shot") { }

        class spell_hun_marked_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_marked_shot_SpellScript);

            enum eSpells
            {
                HuntersMark             = 185365,
                HuntersMarkCaster       = 185743,
                MarkedShot              = 212621,
                Vulnerable              = 187131,
                ZevrimsHunger           = 224550,
                T21Marksmanship4P       = 251753
            };

            void FilterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ZevrimsHunger, EFFECT_0))
                    if (roll_chance_i(l_AuraEffect->GetAmount()))
                        m_ZevrimsHunger = true;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Target) -> bool
                {
                    if (p_Target == nullptr || p_Target->ToUnit() == nullptr)
                        return true;

                    if (p_Target->ToUnit()->HasAura(eSpells::HuntersMark, l_Caster->GetGUID()))
                    {
                        if (!m_ZevrimsHunger)
                            p_Target->ToUnit()->RemoveAura(eSpells::HuntersMark, l_Caster->GetGUID()); ///< 7.1.5 - Build 23420: line added to remove Hunter's mark on targets immuned at the cast of Marked Shot
                        return false;
                    }
                    return true;
                });

                uint64 l_CasterGuid = l_Player->GetGUID();
                uint64 l_TargetGuid = 0;
                uint16 l_Itr = 1;

                l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::CallOfTheHuntTargets].clear();

                uint8 l_ExtraShots = 0;
                if (l_Caster->HasAura(eSpells::T21Marksmanship4P))
                    if (SpellInfo const* l_T21SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Marksmanship4P))
                        if (roll_chance_i(l_T21SpellInfo->Effects[EFFECT_0].BasePoints))
                            l_ExtraShots = l_T21SpellInfo->Effects[EFFECT_1].BasePoints;

                for (auto l_Target : p_Targets)
                {
                    l_TargetGuid = l_Target->GetGUID();
                    l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::CallOfTheHuntTargets].push_back(l_TargetGuid);
                    l_Caster->m_Functions.AddFunction([l_CasterGuid, l_TargetGuid, l_ExtraShots]() -> void
                    {
                        Player* l_Player = sObjectAccessor->FindPlayer(l_CasterGuid);
                        if (!l_Player)
                            return;

                        Unit* l_Target = sObjectAccessor->GetUnit(*l_Player, l_TargetGuid);
                        if (!l_Target)
                            return;

                        l_Player->CastSpell(l_Target, eSpells::MarkedShot);

                        if (l_ExtraShots > 0)
                            l_Player->CastSpell(l_Target, eSpells::MarkedShot);

                    }, l_Player->m_Functions.CalculateTime(50 * l_Itr));
                    l_Itr++;

                    if (l_ExtraShots > 0)
                        --l_ExtraShots;
                }
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Vulnerable, true);

                if (m_ZevrimsHunger)
                    return;

                l_Target->RemoveAurasDueToSpell(eSpells::HuntersMark, l_Caster->GetGUID());

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HuntersMark) == 0)
                    l_Caster->RemoveAurasDueToSpell(eSpells::HuntersMarkCaster);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_marked_shot_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_hun_marked_shot_SpellScript::HandleOnHit);
            }

        private:
            bool m_ZevrimsHunger = false;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_marked_shot_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Call of the Hunter - 191048
class spell_hun_call_of_the_hunter : public SpellScriptLoader
{
public:
    spell_hun_call_of_the_hunter() : SpellScriptLoader("spell_hun_call_of_the_hunter") { }

    class spell_hun_call_of_the_hunter_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_call_of_the_hunter_AuraScript);

        enum eSpells
        {
            MarkedShot = 212621,
            WindArrowBarrage = 191061,
            WindArrowBarrageVisual = 49447,
            WindArrowBarrageRadius = 191323,
            CallOfTheHuntAura = 191048
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo)
                return false;

            if (l_SpellInfo->Id != eSpells::MarkedShot)
                return false;
            return true;
        }

        void HandleOnProc(AuraEffect const* p_AurEff, ProcEventInfo& /*p_EventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster)
                return;

            std::list<uint64> l_List = l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::CallOfTheHuntTargets];

            for (uint64 l_TargetGuid : l_List)
            {
                if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGuid))
                {
                    for (size_t i = 0; i < l_SpellInfo->Effects[EFFECT_0].BasePoints / 2; i++)
                        l_Caster->CastSpell(l_Target, eSpells::WindArrowBarrage, true);
                }
            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_hun_call_of_the_hunter_AuraScript::HandleCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_hun_call_of_the_hunter_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_call_of_the_hunter_AuraScript();
    }
};

/// Last Update 7.1.5
/// Called by Flanking Strike - 202800 - Flanking Strike - 204740
class spell_hun_flanking_strike : public SpellScriptLoader
{
    public:
        spell_hun_flanking_strike() : SpellScriptLoader("spell_hun_flanking_strike") { }

        class spell_hun_flanking_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_flanking_strike_SpellScript);

            enum eSpells
            {
                MongooseBite            = 190928,
                AspectOfTheEagle        = 186289,
                Harpoon                 = 190925,
                FlankingStrike          = 202800,
                PetFlankingStrike       = 204740,
                AnimalInstincts         = 204315,
                AnimalInstinctsValue    = 232646,
                HuntingCompanion        = 191334,
                T19Survival2P           = 211340
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return SPELL_FAILED_DONT_REPORT;

                Pet* l_Pet = l_Player->GetPet();
                if (!l_Pet)
                    return SPELL_FAILED_NO_PET;

                return SPELL_CAST_OK;
            }

            void HandleOnCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_TargetsVictim = l_Target->getVictim();

                if (l_TargetsVictim == l_Player)
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlankingStrikePetIncreasedDamages) = true;
                else
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlankingStrikePetIncreasedDamages) = false;

                Pet* l_Pet = l_Player->GetPet();
                if (!l_Pet)
                    return;

                l_Pet->CastSpell(l_Target, eSpells::PetFlankingStrike, false);
            }

            void HandleThreat(Unit* /*p_Target*/, float& p_Threat)
            {
                SpellInfo const* l_FlankingStrike = sSpellMgr->GetSpellInfo(eSpells::FlankingStrike);
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Pet* l_Pet = l_Caster->ToPet();
                if (!l_Pet)
                    return;

                Player* l_Player = l_Pet->GetOwner();
                if (!l_Player)
                    return;

                if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlankingStrikePetIncreasedDamages))
                    p_Threat += CalculatePct(p_Threat, l_FlankingStrike->Effects[EFFECT_3].BasePoints);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                SpellInfo const* l_FlankingStrike = sSpellMgr->GetSpellInfo(eSpells::FlankingStrike);
                if (!l_FlankingStrike)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                switch (GetSpellInfo()->Id)
                {
                    case eSpells::FlankingStrike:
                    {
                        Player* l_Player = l_Caster->ToPlayer();
                        if (!l_Player)
                            return;

                        if (!l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlankingStrikePetIncreasedDamages))
                            SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), l_FlankingStrike->Effects[EFFECT_2].BasePoints));

                        return;
                    }
                    case eSpells::PetFlankingStrike:
                    {
                        Pet* l_Pet = l_Caster->ToPet();
                        if (!l_Pet)
                            return;

                        Player* l_Player = l_Pet->GetOwner();
                        if (!l_Player)
                            return;

                        int32 l_Damage = 1.5f * (l_Player->GetUInt32Value(UNIT_FIELD_ATTACK_POWER) * 2.152f) * (1 + (l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f));

                        if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlankingStrikePetIncreasedDamages))
                            SetHitDamage(l_Damage + CalculatePct(l_Damage, l_FlankingStrike->Effects[EFFECT_2].BasePoints));
                        else
                            SetHitDamage(l_Damage);

                        if (l_Player->HasAura(eSpells::HuntingCompanion))
                        {
                            float l_Mastery = l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) / 2.0f;

                            if (l_Player->HasAura(eSpells::AspectOfTheEagle))
                                if (SpellInfo const * l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AspectOfTheEagle))
                                    l_Mastery += float(l_SpellInfo->Effects[EFFECT_1].BasePoints);

                            if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::T19Survival2P, EFFECT_0))
                                l_Mastery *= l_AuraEffect->GetAmount();
                            else
                                l_Mastery *= 2;

                            if (roll_chance_f(l_Mastery))
                                if (SpellInfo const* l_MoongoseBite = sSpellMgr->GetSpellInfo(eSpells::MongooseBite))
                                    l_Player->RestoreCharge(l_MoongoseBite->ChargeCategoryEntry);
                        }

                        return;
                    }
                    default:
                        break;
                }
            }

            void HandleAnimalInstinct()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;

                if (!l_Caster || !l_Caster->HasAura(eSpells::AnimalInstincts) || !(l_Player = l_Caster->ToPlayer()))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AnimalInstinctsValue);

                if (!l_SpellInfo)
                    return;

                eSpells m_SpellList[4] =
                {
                    eSpells::FlankingStrike,
                    eSpells::MongooseBite,
                    eSpells::Harpoon,
                    eSpells::AspectOfTheEagle
                };

                l_Player->ReduceSpellCooldown(m_SpellList[urand(0, 3)], l_SpellInfo->Effects[EFFECT_0].Amplitude);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::FlankingStrike:
                        OnCheckCast += SpellCheckCastFn(spell_hun_flanking_strike_SpellScript::HandleCheckCast);
                        OnCast += SpellCastFn(spell_hun_flanking_strike_SpellScript::HandleOnCast);
                        AfterCast += SpellCastFn(spell_hun_flanking_strike_SpellScript::HandleAnimalInstinct);
                        OnEffectHitTarget += SpellEffectFn(spell_hun_flanking_strike_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                        break;
                    case  eSpells::PetFlankingStrike:
                        OnEffectHitTarget += SpellEffectFn(spell_hun_flanking_strike_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                        OnCalculateThreat += SpellCalculateThreatFn(spell_hun_flanking_strike_SpellScript::HandleThreat);
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_flanking_strike_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Moongose Bite 190928
class spell_hun_mongoose_bite : public SpellScriptLoader
{
    public:
        spell_hun_mongoose_bite() : SpellScriptLoader("spell_hun_mongoose_bite") { }

        class spell_hun_mongoose_bite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_mongoose_bite_SpellScript);

            enum eSpells
            {
                MoongoseFury = 190931,
                T19Survival4P = 211357,
                MongoosePower = 211362
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_MoongoseFury = l_Caster->GetAura(eSpells::MoongoseFury);
                if (!l_MoongoseFury)
                    return;

                /// Item - Hunter T19 Survival 4P Bonus
                if (l_Caster->HasAura(eSpells::T19Survival4P))
                    if (l_MoongoseFury->GetStackAmount() >= 6 && !l_Caster->HasAura(eSpells::MongoosePower))
                        l_Caster->CastSpell(l_Caster, MongoosePower, true);
            }

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Duration = 0;
                Aura* l_MoongoseFury = l_Caster->GetAura(eSpells::MoongoseFury);
                if (l_MoongoseFury)
                    l_Duration = l_MoongoseFury->GetDuration();

                l_Caster->CastSpell(l_Caster, eSpells::MoongoseFury, true);

                if (l_Duration)
                    l_MoongoseFury->SetDuration(l_Duration);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_mongoose_bite_SpellScript::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_hun_mongoose_bite_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_mongoose_bite_SpellScript();
        }
};


/// Last Update 7.0.3 Build 22522
/// Called by Titanstrike - 197344
class spell_hun_titanstrike: public SpellScriptLoader
{
    public:
        spell_hun_titanstrike() : SpellScriptLoader("spell_hun_titanstrike") { }

        class spell_hun_titanstrike_AuraScript : public AuraScript
        {
                PrepareAuraScript(spell_hun_titanstrike_AuraScript);

                void OnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;

                    l_Player->m_Events.AddEvent(new SummonHati(l_Player->GetGUID()), l_Player->m_Events.CalculateTime(1));
                }

                void OnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;

                    Creature* l_Hati = l_Player->GetHati();
                    if (!l_Hati)
                        return;

                    l_Hati->DespawnOrUnsummon();
                }

                void OnUpdate(uint32 /*p_Diff*/)
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;

                    Creature* l_Hati = l_Player->GetHati();
                    if (!l_Hati)
                        l_Player->m_Events.AddEvent(new SummonHati(l_Player->GetGUID()), l_Player->m_Events.CalculateTime(1));
                }

                void Register() override
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_hun_titanstrike_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_hun_titanstrike_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    OnAuraUpdate += AuraUpdateFn(spell_hun_titanstrike_AuraScript::OnUpdate);
                }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_titanstrike_AuraScript();
        }
};

/// Titanstrike (BM Artifact Weapon)
class PlayerScript_Titanstrike : public PlayerScript
{
    public:
        PlayerScript_Titanstrike() :PlayerScript("PlayerScript_Titanstrike") {}

        enum eDatas
        {
            Titanstrike = 56
        };

        void OnSummonPet(Player* p_Player)
        {
            if (!p_Player || p_Player->getClass() != CLASS_HUNTER)
                return;

            MS::Artifact::Manager* l_Artifact = p_Player->GetCurrentlyEquippedArtifact();
            if (!l_Artifact || l_Artifact->GetArtifactId() != eDatas::Titanstrike)
                return;

            p_Player->m_Events.AddEvent(new SummonHati(p_Player->GetGUID()), p_Player->m_Events.CalculateTime(1));
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Mend Pet - 136
class spell_hun_mend_pet : public SpellScriptLoader
{
    public:
        spell_hun_mend_pet() : SpellScriptLoader("spell_hun_mend_pet") { }

        class spell_hun_mend_pet_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_mend_pet_SpellScript);

            enum eSpells
            {
                MendPet = 214303
            };

            const uint32 m_HatiEntries[5] =
            {
                100324,
                106548,
                106549,
                106550,
                106551
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Hati = nullptr;
                for (std::set<Unit*>::iterator l_Itr = l_Player->m_Controlled.begin(); l_Itr != l_Player->m_Controlled.end(); ++l_Itr)
                {
                    for (int8 l_SecondItr = 0; l_SecondItr < 5; ++l_SecondItr)
                    {
                        if ((*l_Itr)->GetEntry() == m_HatiEntries[l_SecondItr])
                        {
                            l_Hati = (*l_Itr);
                            break;
                        }
                    }
                    if (l_Hati != nullptr)
                        break;
                }
                if (!l_Hati)
                    return;

                l_Player->CastSpell(l_Hati, eSpells::MendPet, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_mend_pet_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_mend_pet_SpellScript();
        }
};

/// last update : 7.0.3
/// Mimiron's Shell - 197160
class spell_hun_mimirons_shell : public SpellScriptLoader
{
    public:
        spell_hun_mimirons_shell() : SpellScriptLoader("spell_hun_mimirons_shell") { }

        class spell_hun_mimirons_shell_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_mimirons_shell_AuraScript);

            enum eSpells
            {
                MimironsShellHeal = 197161,
                AspectOfTheTurtle = 186265
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (l_DamageInfo == nullptr || l_Caster == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::AspectOfTheTurtle)
                    return;


                l_Caster->CastSpell(l_Caster, eSpells::MimironsShellHeal, true);
                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::MimironsShellHeal, EFFECT_0);
                if (l_AuraEffect == nullptr)
                    return;

                int32 l_Amount = p_AurEff->GetAmount();
                if (l_Amount == 0)
                    l_Amount = 10;

                uint32 l_NewAmount = CalculatePct(l_Caster->GetMaxHealth(), l_Amount) / (l_AuraEffect->GetBase()->GetDuration() / l_AuraEffect->GetAmplitude());
                l_AuraEffect->SetAmount(l_NewAmount);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_mimirons_shell_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_mimirons_shell_AuraScript();
        }
};

/// Last Update 7.0.3
/// Surge of the Stormgod - 197354
class spell_hun_surge_of_the_stormgod : public SpellScriptLoader
{
    public:
        spell_hun_surge_of_the_stormgod() : SpellScriptLoader("spell_hun_surge_of_the_stormgod") { }

        class sspell_hun_surge_of_the_stormgod_AuraScript : public AuraScript
        {
            PrepareAuraScript(sspell_hun_surge_of_the_stormgod_AuraScript);

            enum eSpells
            {
                MultiShot                   = 2643,
                SurgeOfTheStormgodDamage    = 197465
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() == nullptr || p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::MultiShot)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                if (Pet* l_Pet = l_Player->GetPet())
                    l_Player->CastSpell(l_Pet, eSpells::SurgeOfTheStormgodDamage, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(sspell_hun_surge_of_the_stormgod_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new sspell_hun_surge_of_the_stormgod_AuraScript();
        }
};

/// Call Pet 1 - 883, Call Pet 2 - 83242, Call Pet 3 - 83243, Call Pet 4 - 83244, Call Pet 5 - 83245
class spell_hun_call_pet : public SpellScriptLoader
{
    public:
        spell_hun_call_pet() : SpellScriptLoader("spell_hun_call_pet") { }

        class spell_hun_call_pet_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_call_pet_SpellScript);

            enum eSpells
            {
                LoneWolf = 155228
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::LoneWolf))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_CALL_PET_WITH_LONE_WOLF);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_call_pet_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_call_pet_SpellScript();
        }
};

/// Last Update 7.0.3
/// Lone Wolf - 155228
class spell_hun_lone_wolf : public SpellScriptLoader
{
    public:
        spell_hun_lone_wolf() : SpellScriptLoader("spell_hun_lone_wolf") { }

        class spell_hun_lone_wolf_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_lone_wolf_AuraScript);

            enum eSpells
            {
                DismissPet = 2641
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                l_Player->AddDelayedEvent([l_Player]() -> void
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                        l_Player->CastSpell(l_Pet, eSpells::DismissPet, true);
                }, 2000);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_lone_wolf_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_lone_wolf_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called By Titan's Thunder - 218635
class spell_hun_titan_thunder_dmg : public SpellScriptLoader
{
public:
    spell_hun_titan_thunder_dmg() : SpellScriptLoader("spell_hun_titan_thunder_dmg") { }

    class spell_hun_titan_thunder_dmg_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hun_titan_thunder_dmg_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            if (Player* l_OwnerPlayer = l_Owner->ToPlayer())
            {
                int32 l_Damage = l_Owner->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 2;

                SetHitDamage(l_Damage);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_hun_titan_thunder_dmg_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_hun_titan_thunder_dmg_SpellScript();
    }
};


/// Last Update 7.0.3
/// Dire Frenzy - 217200
class spell_hun_dire_frenzy : public SpellScriptLoader
{
    public:
        spell_hun_dire_frenzy() : SpellScriptLoader("spell_hun_dire_frenzy") { }

        class spell_hun_dire_frenzy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_dire_frenzy_SpellScript);

            enum eSpells
            {
                DireFrenzyDamage    = 217207,
                WildCall            = 185791,
                BestialWrath        = 19574,
                EredunWarOrder      = 212278,
                KillCommand         = 34026,
                TitansThunderBuff   = 218638,
                TitansThunderDamage = 218635,
                T19BeastMastery2P   = 211172,

                DireFrenzyFocus     = 246152, /// Last Update 7.3.5 Build 26365
                DireFrenzyFocus2    = 246851,
                DireFrenzyFocus3    = 246852,
                DireFrenzyFocus4    = 246853,
                DireFrenzyFocus5    = 246854
            };

            enum eDatas
            {
                MaxDireFrenzyFocus = 5
            };

            void CastFocusBuff(Player* p_Player)
            {
                std::array<std::pair<uint32, Aura*>, eDatas::MaxDireFrenzyFocus> l_DireFrenziesFocus =
                {{
                    std::make_pair<uint32, Aura*>(eSpells::DireFrenzyFocus,     nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::DireFrenzyFocus2,    nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::DireFrenzyFocus3,    nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::DireFrenzyFocus4,    nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::DireFrenzyFocus5,    nullptr)
                }};

                std::pair<uint32, int32> l_LessDurationIndex = { 0, -1 };
                for (uint32 l_I = 0; l_I < l_DireFrenziesFocus.size(); ++l_I)
                {
                    l_DireFrenziesFocus[l_I].second = p_Player->GetAura(l_DireFrenziesFocus[l_I].first, p_Player->GetGUID());
                    if (l_DireFrenziesFocus[l_I].second == nullptr)
                    {
                        p_Player->CastSpell(p_Player, l_DireFrenziesFocus[l_I].first, true);
                        return;
                    }

                    if (l_LessDurationIndex.second == -1 || l_DireFrenziesFocus[l_I].second->GetDuration() < l_LessDurationIndex.second)
                        l_LessDurationIndex = { l_I, l_DireFrenziesFocus[l_I].second->GetDuration() };
                }

                p_Player->CastSpell(p_Player, l_DireFrenziesFocus[l_LessDurationIndex.first].first, true);
            }

            void HandleAfterCast()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BestialWrath))
                    {
                        if (l_Player->HasSpellCooldown(eSpells::BestialWrath))
                        {
                            int32 l_Duration = l_SpellInfo->Effects[EFFECT_2].BasePoints;
                            /// Item - Hunter T19 Beast Mastery 2P
                            if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::T19BeastMastery2P, EFFECT_0))
                                l_Duration += l_AuraEffect->GetAmount();
                            l_Player->ReduceSpellCooldown(eSpells::BestialWrath, l_Duration * IN_MILLISECONDS);
                        }
                    }
                }
            }

            void HandleDummy(SpellEffIndex p_EffIndex)
            {
                if (!GetCaster())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Player->HasAura(eSpells::WildCall))
                    l_Player->RemoveAura(eSpells::WildCall);

                if (l_Target == nullptr || l_Player == nullptr)
                    return;

                if (l_Player->HasAura(eSpells::EredunWarOrder))
                {
                    SpellInfo const* l_EredunWarOrderInfo = sSpellMgr->GetSpellInfo(eSpells::EredunWarOrder);

                    if (l_EredunWarOrderInfo)
                        l_Player->ModifySpellCooldown(eSpells::KillCommand, -l_EredunWarOrderInfo->Effects[0].BasePoints);
                }

                CastFocusBuff(l_Player);

                Pet* l_Pet = l_Player->GetPet();

                if (l_Pet == nullptr)
                    return;

                l_Player->m_SpellHelper.GetUint64(eSpellHelpers::DireFrenzyDamage) = l_Target->GetGUID();

                bool l_HasTitansThunder = l_Pet->HasAura(eSpells::TitansThunderBuff);

                for (uint8 i = 0; i < GetSpellInfo()->Effects[p_EffIndex].BasePoints; ++i)
                {
                    if (l_HasTitansThunder)
                        l_Pet->CastSpell(l_Target, eSpells::TitansThunderDamage, true);
                }

                if (l_HasTitansThunder)
                    l_Pet->RemoveAura(eSpells::TitansThunderBuff);

                if (!l_Pet->IsWithinMeleeRange(l_Target))
                {
                    l_Pet->GetMotionMaster()->MoveJump(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ(), 60, 0, 0, true, 0, l_Target->GetGUID());
                    l_Pet->Attack(l_Target, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_dire_frenzy_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_hun_dire_frenzy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };


        class spell_hun_dire_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_dire_frenzy_AuraScript);

            enum eSpells
            {
                DireFrenzyDamage = 217207
            };

            uint64 m_TargetGUID = 0;

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                if (l_Player == nullptr)
                    return;

                Pet* l_Pet = l_Player->GetPet();

                if (l_Pet == nullptr)
                    return;

                Unit* l_Target = nullptr;
                if ((l_Target = sObjectAccessor->FindUnit(l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::DireFrenzyDamage))))
                    m_TargetGUID = l_Target->GetGUID();

                if (!l_Target)
                    return;

                for (size_t i = 0; i < 5; i++)
                    l_Pet->DelayedCastSpell(l_Target, eSpells::DireFrenzyDamage, true, std::max(l_Pet->GetExactDist2d(l_Target) - 4, 0.0f) * 25);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_dire_frenzy_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_MOD_MELEE_RANGED_HASTE_2, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_dire_frenzy_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_dire_frenzy_AuraScript();
        }
};

/// Last Update 7.0.3
/// Snake Hunter - 201078
class spell_hun_snake_hunter : public SpellScriptLoader
{
    public:
        spell_hun_snake_hunter() : SpellScriptLoader("spell_hun_snake_hunter") { }

        class spell_hun_snake_hunter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_snake_hunter_SpellScript);

            enum eSpells
            {
                MongooseBite = 190928
            };

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                /// Instantly grants you 3 charges of Mongoose Bite.
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MongooseBite);
                if (SpellCategoryEntry const* l_CastegoryEntry = l_SpellInfo->ChargeCategoryEntry)
                {
                    for (uint8 i = 0; i < GetSpellInfo()->Effects[EFFECT_0].BasePoints; ++i)
                        l_Player->RestoreCharge(l_CastegoryEntry);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_snake_hunter_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_snake_hunter_SpellScript();
        }
};

/// Last Update 7.0.3
/// Aspect of the Turtle - 186265
class spell_hun_aspect_of_the_turtle : public SpellScriptLoader
{
    public:
        spell_hun_aspect_of_the_turtle() : SpellScriptLoader("spell_hun_aspect_of_the_turtle") { }

        class spell_hun_aspect_of_the_turtle_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_aspect_of_the_turtle_AuraScript);

            enum eSpells
            {
                AspectOfTheTurtlePacified = 205769
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::AspectOfTheTurtlePacified, true);

                l_Caster->RemoveFlagsAuras();
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAura(eSpells::AspectOfTheTurtlePacified);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_aspect_of_the_turtle_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_aspect_of_the_turtle_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_aspect_of_the_turtle_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Harpon - 190925
class spell_hun_harpon : public SpellScriptLoader
{
    public:
        spell_hun_harpon() : SpellScriptLoader("spell_hun_harpon") { }

        class spell_hun_harpon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_harpon_SpellScript);

            enum eSpells
            {
                HarponJump              = 186260,
                HarponRoot              = 190927,
                PostHaste               = 109215,
                PostHastBuff            = 118922,
                RopeOfTheMistMarauder   = 213154,
                MarkOfHelbrine          = 213156
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::HarponRoot, true);
            }

            void HandlDummy(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::RopeOfTheMistMarauder))
                {
                    if (l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkedByHelbrine].find(l_Target->GetGUID()) == l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkedByHelbrine].end())
                    {
                        float l_Distance = l_Target->GetDistance(l_Caster->GetPosition());
                        SpellInfo const* l_HarpoonInfo = GetSpellInfo();
                        SpellInfo const* l_MarkOfHelbrineInfo = sSpellMgr->GetSpellInfo(eSpells::RopeOfTheMistMarauder);

                        if (l_HarpoonInfo && l_MarkOfHelbrineInfo && l_HarpoonInfo->RangeEntry)
                        {
                            int32 l_DamageModifier = static_cast<int32>(l_MarkOfHelbrineInfo->Effects[0].BasePoints * (l_Distance / l_HarpoonInfo->RangeEntry->maxRangeHostile));

                            l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkedByHelbrine].clear();
                            l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkedByHelbrine].insert(l_Target->GetGUID());
                            l_Caster->CastCustomSpell(l_Target, eSpells::MarkOfHelbrine, &l_DamageModifier, nullptr, nullptr, true);
                        }
                    }
                }

                if (l_Caster->HasAura(eSpells::PostHaste))
                {
                    l_Caster->RemoveMovementImpairingAuras();
                    l_Caster->CastSpell(l_Caster, eSpells::PostHastBuff, true);
                }

                l_Caster->CastSpell(l_Target, eSpells::HarponJump, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_harpon_SpellScript::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_hun_harpon_SpellScript::HandlDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_harpon_SpellScript();
        }
};

/// Last Update 7.3.5
/// Called By Aspect of the Cheetah - 186257
class spell_hun_aspect_of_the_cheetah : public SpellScriptLoader
{
    public:
        spell_hun_aspect_of_the_cheetah() : SpellScriptLoader("spell_hun_aspect_of_the_cheetah") { }

        class spell_hun_aspect_of_the_cheetah_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_aspect_of_the_cheetah_AuraScript);

            enum eSpells
            {
                AspectOfTheCheetahSecond    = 186258,
                HuntingPack                 = 203235,
                AspectOfTheCheetahRange     = 203233,
                FeetOfWindAura              = 238088,
                FeetOfWind                  = 240777,
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::HuntingPack))
                    l_Caster->CastSpell(l_Caster, eSpells::AspectOfTheCheetahRange, true);

                if (l_Caster->HasAura(eSpells::FeetOfWindAura))
                    l_Caster->CastSpell(l_Caster, eSpells::FeetOfWind, true);
            }

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();

                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                l_Target->CastSpell(l_Target, eSpells::AspectOfTheCheetahSecond, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_aspect_of_the_cheetah_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_hun_aspect_of_the_cheetah_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_aspect_of_the_cheetah_AuraScript();
        }
};

/// last update : 7.3.5 build
/// Farstrider - 199523
class spell_hun_farstrider : public SpellScriptLoader
{
    public:
        spell_hun_farstrider() : SpellScriptLoader("spell_hun_farstrider") { }

        class spell_hun_farstrider_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_farstrider_AuraScript);

            enum eSpells
            {
                Harpoon         = 190925,
                Disengage       = 781
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                SpellInfo const* l_DisengageSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Disengage);
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_DisengageSpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_ProcEventInfo.GetHitMask() & ProcFlagsHit::PROC_HIT_CRITICAL)
                {
                    l_Player->RestoreCharge(l_DisengageSpellInfo->ChargeCategoryEntry);
                    l_Player->RemoveSpellCooldown(eSpells::Disengage, true);
                    l_Player->RemoveSpellCooldown(eSpells::Harpoon, true);
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_hun_farstrider_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_farstrider_AuraScript();
        }
};

/// last update : 7.0.3
/// Dragonsfire Grenade - 194858
class spell_hun_dragonsfire_grenade : public SpellScriptLoader
{
    public:
        spell_hun_dragonsfire_grenade() : SpellScriptLoader("spell_hun_dragonsfire_grenade") { }

        class spell_hun_dragonsfire_grenade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_dragonsfire_grenade_AuraScript);

            enum eSpells
            {
                DragonsFireGrande = 194859
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DragonsFireGrande, true);

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    l_Caster->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_dragonsfire_grenade_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_dragonsfire_grenade_AuraScript();
        }
};

/// Last Update 7.0.3
/// Dragonsfire Conflagration - 194859
class spell_hun_dragonsfire_conflagration : public SpellScriptLoader
{
    public:
        spell_hun_dragonsfire_conflagration() : SpellScriptLoader("spell_hun_dragonsfire_conflagration") { }

        class spell_hun_dragonsfire_conflagration_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_dragonsfire_conflagration_SpellScript);

            void FilterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_MainTarget = GetExplTargetUnit();

                if (l_MainTarget == nullptr)
                    return;

                p_Targets.remove_if([this, l_MainTarget](WorldObject* p_Target) -> bool
                {
                    if (p_Target == nullptr || p_Target->ToUnit() == nullptr)
                        return true;

                    if (p_Target->GetGUID() == l_MainTarget->GetGUID())
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_dragonsfire_conflagration_SpellScript::FilterTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_dragonsfire_conflagration_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Talon Strike - 203563
class spell_hun_talon_strike : public SpellScriptLoader
{
    public:
        spell_hun_talon_strike() : SpellScriptLoader("spell_hun_talon_strike") { }

        class spell_hun_talon_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_talon_strike_AuraScript);

            enum eSpells
            {
                TalonBond   = 238089,
                TalonSlash  = 242735
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Victim = l_DamageInfo->GetTarget();
                if (l_SpellInfo || !l_Victim)
                    return;

                if (roll_chance_i(30))
                {
                    l_Caster->CastSpell(l_Victim, p_AurEff->GetTriggerSpell(), true);

                    if (l_Caster->HasAura(TalonBond))
                    {
                        Player* l_Player = l_Caster->ToPlayer();

                        if (!l_Player)
                            return;

                        Unit* l_Pet = l_Player->GetPet();

                        if (!l_Pet)
                            return;

                        l_Pet->CastSpell(l_Victim, eSpells::TalonSlash, true);
                        l_Pet->DelayedCastSpell(l_Victim, eSpells::TalonSlash, true, 0.4 * IN_MILLISECONDS);
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_talon_strike_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_talon_strike_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Talon Slash - 242735
class spell_hun_talon_slash : public SpellScriptLoader
{
public:
    spell_hun_talon_slash() : SpellScriptLoader("spell_hun_talon_slash") { }

    class spell_hun_talon_slash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hun_talon_slash_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Owner = GetCaster()->GetOwner();
            Unit* l_Target = GetHitUnit();

            if (!l_Owner || !l_Target)
                return;

            int32 l_Damage = (int32)(l_Owner->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 2.0f);

            l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, SpellEffIndex::EFFECT_0, SPELL_DIRECT_DAMAGE);
            l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, SpellEffIndex::EFFECT_0);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_hun_talon_slash_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_hun_talon_slash_SpellScript();
    }
};

/// Last Update 7.0.3
/// Dragonscale Armor - 202589
class spell_hun_dragonscale_armor : public SpellScriptLoader
{
    public:
        spell_hun_dragonscale_armor() : SpellScriptLoader("spell_hun_dragonscale_armor") { }

        class spell_hun_dragonscale_armor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_dragonscale_armor_AuraScript);

            void OnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
                Unit* l_Victim = p_DmgInfo.GetTarget();
                Unit* l_Attacker = p_DmgInfo.GetActor();
                SpellInfo const* l_SpellInfo = p_DmgInfo.GetSpellInfo();

                if (l_Victim == nullptr || l_Attacker == nullptr || l_SpellInfo == nullptr)
                    return;

                if (!l_SpellInfo->IsPeriodic())
                    return;

                AuraEffect* l_AuraEffect = p_AurEff->GetBase()->GetEffect(EFFECT_1);
                if (l_AuraEffect == nullptr)
                    return;

                p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_AuraEffect->GetAmount());
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1; ///< Initialize to infinite Absorb
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_dragonscale_armor_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_hun_dragonscale_armor_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hun_dragonscale_armor_AuraScript();
        }
};

class WaylayHelper
{
    enum eSpells
    {
        Waylay           = 234955,
        GuerrillaTactics = 236698,

        FreezingTrapAura = 3355,
        SteelTrapDamage  = 162487,
        SteelTrapAreaTrigger = 162496,
        ExpertTrapperPassive = 199543,
    };

    public:

        void ResetWaylayForNextSpell(Unit* l_Caster, uint32 spellId)
        {
            l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::WaylayActivated][spellId] = false;
        }

        bool IsWaylayActivatedForSpell(Unit* l_Caster, uint32 spellId)
        {
            auto l_ItrWaylayActivatedTraps = l_Caster->m_SpellHelper.GetUint64Map().find(eSpellHelpers::WaylayActivated);
            if (l_ItrWaylayActivatedTraps == l_Caster->m_SpellHelper.GetUint64Map().end())
                return false;

            auto l_ItrWayLayActivated = l_ItrWaylayActivatedTraps->second.find(spellId);
            if (l_ItrWayLayActivated == l_ItrWaylayActivatedTraps->second.end())
                return false;

            return l_ItrWayLayActivated->second == 1;
        }

        uint32 CalcWaylayEffectForSpell(Unit* l_Caster, uint32 spellId)
        {
            Aura* aurWaylay = l_Caster->GetAura(eSpells::Waylay);
            if (!aurWaylay)
                return 0;

            switch (spellId)
            {
                case eSpells::FreezingTrapAura:
                    return aurWaylay->GetEffect(EFFECT_1)->GetAmount();
                case eSpells::SteelTrapDamage:
                    return aurWaylay->GetEffect(EFFECT_2)->GetAmount();
            }

            return 0;
        }

        void PrepareWaylayEffect(Unit* l_Caster, uint32 spellId, Unit* l_Target = nullptr)
        {
            bool isReady = IsWaylayReadyForSpell(l_Caster, spellId);
            if (isReady)
            {
                if (spellId == eSpells::SteelTrapDamage && (!l_Target || l_Target->isInCombat()))
                    return;

                ActivateWaylayForNextSpell(l_Caster, spellId);
            }
        }

    private:

        bool IsWaylayReadyForSpell(Unit* l_Caster, uint32 spellId)
        {
            auto l_ItrTriggeredByAreaTrigger = l_Caster->m_SpellHelper.GetUint64Map().find(eSpellHelpers::TriggeredByAreaTrigger);
            if (l_ItrTriggeredByAreaTrigger == l_Caster->m_SpellHelper.GetUint64Map().end())
                return false;

            auto l_ItrAreaTrigger = l_ItrTriggeredByAreaTrigger->second.find(spellId);
            if (l_ItrAreaTrigger == l_ItrTriggeredByAreaTrigger->second.end())
                return false;

            auto l_AreaTrigger = l_Caster->GetMap()->GetAreaTrigger(l_ItrAreaTrigger->second);
            if (!l_AreaTrigger)
                return false;

            uint32 maxDuration = l_AreaTrigger->GetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_DURATION);

            bool isReady = maxDuration > l_AreaTrigger->GetDuration() && maxDuration - l_AreaTrigger->GetDuration() >= 2000;

            return isReady;
        }

        void ActivateWaylayForNextSpell(Unit* l_Caster, uint32 spellId)
        {
            l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::WaylayActivated][spellId] = true;
        }
};

/// Last Update 7.3.5 build 26365
/// Freezing Trap - 3355
class spell_hun_freezing_trap : public SpellScriptLoader
{
    public:
        spell_hun_freezing_trap() : SpellScriptLoader("spell_hun_freezing_trap") { }

        enum eSpells
        {
            ExpertTrapperPassive        = 199543,
            ExpertTrapperFreezingTrap   = 201142,

            FreezingTrapAura            = 3355,
            Waylay                      = 234955,
            UnbreakableWill             = 182234, ///< Paladin Artifact Trait
            UnbreakableWillCrowedControlled = 182497,
            UnbreakableWillTimer        = 182496
        };

        class spell_hun_freezing_trap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_freezing_trap_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Waylay))
                {
                    m_WaylayHelper.PrepareWaylayEffect(l_Caster, eSpells::FreezingTrapAura);
                }
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Target->IsPlayer()
                    || l_Target->getClass() != CLASS_PALADIN || l_Target->ToPlayer()->GetActiveSpecializationID() != SPEC_PALADIN_RETRIBUTION)
                    return;

                if (!l_Target->HasAura(eSpells::UnbreakableWill) || l_Target->HasAura(eSpells::UnbreakableWillTimer))
                    return;

                Aura* l_AuraUnbreakableWill = l_Target->AddAura(eSpells::UnbreakableWillCrowedControlled, l_Target);
                if (!l_AuraUnbreakableWill)
                    return;

                l_AuraUnbreakableWill->SetScriptGuid(GetSpellInfo()->Id, l_Caster->GetGUID());
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_hun_freezing_trap_SpellScript::HandleBeforeCast);
                AfterHit += SpellHitFn(spell_hun_freezing_trap_SpellScript::HandleAfterHit);
            }

        private:

            WaylayHelper m_WaylayHelper;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_freezing_trap_SpellScript();
        }

        class spell_hun_freezing_trap_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_freezing_trap_AuraScript);

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                /// Freezing Trap removes all DoTs from target
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_LEECH);
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_HEALTH_FUNNEL);

                m_IsWaylayActivated = m_WaylayHelper.IsWaylayActivatedForSpell(l_Caster, eSpells::FreezingTrapAura);
                m_WayLayDurationEffect = m_WaylayHelper.CalcWaylayEffectForSpell(l_Caster, eSpells::FreezingTrapAura);
                m_WaylayHelper.ResetWaylayForNextSpell(l_Caster, eSpells::FreezingTrapAura);
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// Handle Expert Trapper's Freezing Trap Part
                /// "When Freezing Trap's incapacitate effect ends, the victim and all neaby enemies' movement speed is reduced by 50% for 4 sec."
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Caster->HasAura(eSpells::ExpertTrapperPassive))
                        l_Caster->CastSpell(l_Target, eSpells::ExpertTrapperFreezingTrap, true, nullptr, nullptr);
                }
            }

            void HandleProc(ProcEventInfo& p_ProcInfos)
            {
                if (IsWaylayEffectActive())
                    return;

                if (p_ProcInfos.GetTypeMask() & POSITIVE_PROC_FLAG_MASK)
                    return;

                ///< Why is it here?
                SetDuration(0);
            }

            void HandleOnProc(AuraEffect const*, ProcEventInfo& p_ProcInfos)
            {
                // It prevents dropping the aura by damage
                if (IsWaylayEffectActive())
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_hun_freezing_trap_AuraScript::HandleProc);
                OnEffectProc += AuraEffectProcFn(spell_hun_freezing_trap_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_MOD_STUN);
                AfterEffectApply += AuraEffectApplyFn(spell_hun_freezing_trap_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_freezing_trap_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            bool IsWaylayEffectActive()
            {
                return GetMaxDuration() - GetDuration() <= m_WayLayDurationEffect && m_IsWaylayActivated;
            }

        private:

            WaylayHelper m_WaylayHelper;
            bool m_IsWaylayActivated = false;
            uint32 m_WayLayDurationEffect = 0;

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_freezing_trap_AuraScript();
        }
};

/// Explosive Trap Effect - 13812
/// Updated as of 7.3.2 - 25549
class spell_hun_explosive_trap : public SpellScriptLoader
{
    public:
        spell_hun_explosive_trap() : SpellScriptLoader("spell_hun_explosive_trap") { }

        enum eSpells
        {
            ExplosiveTrapDamage         = 13812,
            ExplosiveTrapStagger        = 237338,
            Waylay                      = 234955,
            UnseenPredatorsCloak        = 248089,
            UnseenPredatorsCloakTrigger = 248212,

            SPELL_HUNTER_EXPERT_TRAPPER = 199543
        };

        class spell_impl : public SpellScript
        {
            PrepareSpellScript(spell_impl);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Waylay))
                {
                    m_WaylayHelper.PrepareWaylayEffect(l_Caster, eSpells::ExplosiveTrapDamage);
                }
            }

            void HandleBeforeHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    l_Caster->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
            }

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    l_Caster->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);

                /// Initial damage is done only to the trap triggerer
                if (Unit* l_Target = GetHitUnit())
                {
                    /// Increases Explosive Trap's damage to the triggering enemy by 75%
                    if (AuraEffect* const l_AurEff = l_Caster->GetAuraEffect(SPELL_HUNTER_EXPERT_TRAPPER, EFFECT_0))
                    {
                        SetHitDamage(CalculatePct(GetHitDamage(), 100 + l_AurEff->GetBaseAmount()));
                        m_TargetGUID = l_Target->GetGUID();
                    }

                    if (m_WaylayHelper.IsWaylayActivatedForSpell(l_Caster, eSpells::ExplosiveTrapDamage))
                    {
                        l_Caster->DelayedCastSpell(l_Target, eSpells::ExplosiveTrapStagger, true, 1);
                        m_WaylayHelper.ResetWaylayForNextSpell(l_Caster, eSpells::ExplosiveTrapDamage);
                    }

                    /// Legendary Unseen Predator's Cloak
                    if (l_Caster->HasAura(eSpells::UnseenPredatorsCloak))
                        l_Caster->CastSpell(l_Target, eSpells::UnseenPredatorsCloakTrigger, true);
                }
            }

            void HandleSetMain()
            {
                if (!m_TargetGUID)
                    return;

                if (Unit* l_Target = GetHitUnit())
                {
                    if (l_Target->GetGUID() != m_TargetGUID)
                        return;

                    if (Aura* const l_Aura = l_Target->GetAura(GetSpellInfo()->Id))
                        l_Aura->SetScriptData(1, 0);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_impl::HandleBeforeCast);
                BeforeHit += SpellHitFn(spell_impl::HandleBeforeHit);
                OnEffectHitTarget += SpellEffectFn(spell_impl::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterHit += SpellHitFn(spell_impl::HandleSetMain);
            }

            private:
                WaylayHelper m_WaylayHelper;
                uint64 m_TargetGUID = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_impl();
        }

        class aura_impl : public AuraScript
        {
            PrepareAuraScript(aura_impl);

            bool m_AlreadyApply = false;

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                if (p_Type != 1)
                    return;

                if (m_AlreadyApply)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    /// Increases Explosive Trap's damage to the triggering enemy by 75%
                    if (AuraEffect* const l_AurEffExpert = l_Caster->GetAuraEffect(SPELL_HUNTER_EXPERT_TRAPPER, EFFECT_0))
                    {
                        if (AuraEffect* const l_AurEff = GetEffect(EFFECT_1))
                        {
                            m_AlreadyApply = true;
                            l_AurEff->SetAmount(CalculatePct(l_AurEff->GetAmount(), 100 + l_AurEffExpert->GetBaseAmount()));
                        }
                    }
                }
            }

            void Register() override
            {
                /// empty :P
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new aura_impl();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Tar Trap - 187700
class spell_hun_tar_trap_areatrigger : public SpellScriptLoader
{
    public:
        spell_hun_tar_trap_areatrigger() : SpellScriptLoader("spell_hun_tar_trap_areatrigger") { }

        class spell_hun_tar_trap_areatrigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_tar_trap_areatrigger_SpellScript);

            enum eSpells
            {
                TarTrapAreaTrigger  = 187700,
                SuperStickyTar      = 236699,
                Waylay              = 234955
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Waylay))
                {
                    m_WaylayHelper.PrepareWaylayEffect(l_Caster, eSpells::TarTrapAreaTrigger);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Loc = GetExplTargetDest();
                if (!l_Caster || !l_Loc)
                    return;

                if (m_WaylayHelper.IsWaylayActivatedForSpell(l_Caster, eSpells::TarTrapAreaTrigger))
                {
                    l_Caster->CastSpell(l_Loc->GetPositionX(), l_Loc->GetPositionY(), l_Loc->GetPositionZ(), eSpells::SuperStickyTar, true);
                    m_WaylayHelper.ResetWaylayForNextSpell(l_Caster, eSpells::TarTrapAreaTrigger);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_hun_tar_trap_areatrigger_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_hun_tar_trap_areatrigger_SpellScript::HandleAfterCast);
            }

        private:

            WaylayHelper m_WaylayHelper;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_tar_trap_areatrigger_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Tar Trap - 178364
class spell_hun_tar_trap : public SpellScriptLoader
{
    public:
        spell_hun_tar_trap() : SpellScriptLoader("spell_hun_tar_trap") { }

        class spell_hun_tar_trap_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_tar_trap_AuraScript);

            enum eSpells
            {
                TarTrapDummy    = 178364,
                TarTrap         = 135299,
                SuperStickyTar  = 236699
            };

            void HandleAfterEffect(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::SuperStickyTar))
                {
                    l_Caster->AddAura(eSpells::TarTrap, l_Target);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_tar_trap_AuraScript::HandleAfterEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_tar_trap_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Steel Trap - 162487
class spell_hun_steel_trap_dmg : public SpellScriptLoader
{
    public:
        spell_hun_steel_trap_dmg() : SpellScriptLoader("spell_hun_steel_trap_dmg") { }

        class spell_hun_steel_trap_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_steel_trap_dmg_SpellScript);

            enum eSpells
            {
                SteelTrapDamage = 162487,
                SteelTrapAreaTrigger = 162496,
                Waylay = 234955
            };

            void HandleOnPrepare()
            {
                // Use OnPrepare hook to check if the target is in combat state (in PrepareWaylayEffect).
                // BeforeCast hook is called after 'SetInCombatState'

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::Waylay))
                {
                    m_WaylayHelper.PrepareWaylayEffect(l_Caster, eSpells::SteelTrapDamage, l_Target);
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_hun_steel_trap_dmg_SpellScript::HandleOnPrepare);
            }

        private:

            WaylayHelper m_WaylayHelper;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_steel_trap_dmg_SpellScript();
        }

        class spell_hun_steel_trap_dmg_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_steel_trap_dmg_AuraScript);

            enum eSpells
            {
                SteelTrapDamage = 162487,
                SteelTrapAreaTrigger = 162496,
                ExpertTrapperPassive = 199543
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (!l_Caster || !l_Target)
                    return;

                if (m_WaylayHelper.IsWaylayActivatedForSpell(l_Caster, eSpells::SteelTrapDamage))
                {
                    AddPct(p_Amount, m_WaylayHelper.CalcWaylayEffectForSpell(l_Caster, eSpells::SteelTrapDamage));
                    m_WaylayHelper.ResetWaylayForNextSpell(l_Caster, eSpells::SteelTrapDamage);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_steel_trap_dmg_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }

        private:

            WaylayHelper m_WaylayHelper;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_steel_trap_dmg_AuraScript();
        }
};

/// last update : 7.0.3
/// Fury of the Eagle - 203415
class spell_hun_fury_of_the_eagle_trigger_aura : public SpellScriptLoader
{
    public:
        spell_hun_fury_of_the_eagle_trigger_aura() : SpellScriptLoader("spell_hun_fury_of_the_eagle_trigger_aura") { }

        class spell_hun_fury_of_the_eagle_trigger_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_fury_of_the_eagle_trigger_aura_AuraScript);

            enum eSpells
            {
                MongooseFury = 190931
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;


                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::MongooseFury, EFFECT_0))
                {
                    /// When the ability is used, it will actually extend your Mongoose Fury buff by 4 seconds so that it can't drop off during the channel. Last edited by Oldmana on 2016 / 09 / 11 (Patch 7.0.3)
                    /// I don't know why it's not written but in retail it increases the duration of mongoose fury with 4 seconds
                    l_AuraEffect->GetBase()->SetDuration(l_AuraEffect->GetBase()->GetDuration() + (4 * IN_MILLISECONDS));
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_fury_of_the_eagle_trigger_aura_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_fury_of_the_eagle_trigger_aura_AuraScript();
        }
};

/// last update : 7.0.3
/// Fury of the Eagle(damage) - 203413
class spell_hun_fury_of_the_eagle : public SpellScriptLoader
{
    public:
        spell_hun_fury_of_the_eagle() : SpellScriptLoader("spell_hun_fury_of_the_eagle") { }

        class spell_hun_fury_of_the_eagle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_fury_of_the_eagle_SpellScript);

            enum eSpells
            {
                MongooseFury = 190931
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::MongooseFury, EFFECT_0))
                {
                    /// This spell do not have FamillyFlag
                    SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), l_AuraEffect->GetAmount()));
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_fury_of_the_eagle_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_fury_of_the_eagle_SpellScript();
        }
};

/// Expert Trapper - Called by Steel Strap Root - 162480
/// Updated as of 7.0.3 - 22522
class spell_hun_expert_trapper_steel : public SpellScriptLoader
{
    public:
        spell_hun_expert_trapper_steel() : SpellScriptLoader("spell_hun_expert_trapper_steel") { }

        class spell_impl : public SpellScript
        {
            PrepareSpellScript(spell_impl);

            enum
            {
                SPELL_HUNTER_EXPERT_TRAPPER_STEEL_TRAP_EFFECT   = 201199,
                ExpertTrapperPassive        = 199543
            };

            void HandleBeforeHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    l_Caster->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);

                if (!l_Caster->HasAura(ExpertTrapperPassive))
                    return;

                if (Unit* l_Target = GetHitUnit())
                {
                    /// Your Steel Trap also deals an immediate (500% of Attack power) Bleed damage when triggered.
                    l_Caster->CastSpell(l_Target, SPELL_HUNTER_EXPERT_TRAPPER_STEEL_TRAP_EFFECT, true);
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_impl::HandleBeforeHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_impl();
        }
};

/// 190931 - Mongoose Fury
/// Updated as of 7.0.3 - 22522
class spell_hun_mongoose_fury : public SpellScriptLoader
{
    public:
        spell_hun_mongoose_fury() : SpellScriptLoader("spell_hun_mongoose_fury") { }

        class aura_impl : public AuraScript
        {
            PrepareAuraScript(aura_impl);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& p_Amount, bool& p_Recalculate)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->IsInPvPCombat() || l_Player->CanApplyPvPSpellModifiers())
                        p_Amount = GetSpellInfo()->Effects[EFFECT_1].BasePoints; ///< PvP Modifier
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(aura_impl::CalculateAmount, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new aura_impl();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Viper Sting Debuff - 202797
class spell_hun_viper_sting_debuff : public SpellScriptLoader
{
    public:
        spell_hun_viper_sting_debuff() : SpellScriptLoader("spell_hun_viper_sting_debuff") { }

        class spell_hun_viper_sting_debuff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_viper_sting_debuff_AuraScript);

            enum eSpells
            {
                ViperStingDebuff    = 202797
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Target = GetTarget();
                if (!l_DamageInfo || !l_Target)
                    return false;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_Spell)
                    return false;

                if (l_Spell->GetCastTime() <= 0)
                    return false;

                Aura* l_Aura = l_Target->GetAura(eSpells::ViperStingDebuff);
                if (l_Aura)
                    l_Aura->SetDuration(0);

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_viper_sting_debuff_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_viper_sting_debuff_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Survival Tactics - 202746
class spell_hun_survival_tactics : public SpellScriptLoader
{
    public:
        spell_hun_survival_tactics() : SpellScriptLoader("spell_hun_survival_tactics") { }

        class spell_hun_survival_tactics_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_survival_tactics_AuraScript);

            enum eSpells
            {
                SurvivalTacticsBuff = 202748,
                FeignDeath          = 5384
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::FeignDeath)
                    return false;

                l_Caster->DelayedCastSpell(l_Caster, eSpells::SurvivalTacticsBuff, true, 1);

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_survival_tactics_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_survival_tactics_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Focused Fire - 213543
class spell_hun_focused_fire : public SpellScriptLoader
{
    public:
        spell_hun_focused_fire() : SpellScriptLoader("spell_hun_focused_fire") { }

        class spell_hun_focused_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_focused_fire_AuraScript);

            enum eSpells
            {
                FocusedFireBuff = 195638,
                DurationSpell   = 195640
            };

            uint32 m_TickElapsed = 0;

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FocusedFireBuff, true);
            }

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                ++m_TickElapsed;
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_DurationSpell = sSpellMgr->GetSpellInfo(eSpells::DurationSpell);
                if (!l_Caster || !l_DurationSpell)
                    return;

                if ((m_TickElapsed >= l_DurationSpell->GetMaxDuration() / IN_MILLISECONDS) && !l_Caster->HasAura(eSpells::FocusedFireBuff))
                    l_Caster->CastSpell(l_Caster, eSpells::FocusedFireBuff, true);
            }

            bool HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                m_TickElapsed = 0;
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Aura* l_Aura = l_Caster->GetAura(eSpells::FocusedFireBuff);
                if (l_Aura)
                    l_Aura->SetDuration(0);

                return false;
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::FocusedFireBuff);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_focused_fire_AuraScript::HandleOnApply, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_focused_fire_AuraScript::HandleOnRemove, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_focused_fire_AuraScript::HandleOnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
                DoCheckProc += AuraCheckProcFn(spell_hun_focused_fire_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_focused_fire_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Throwing Axes - 200163
class spell_hun_throwing_axes : public SpellScriptLoader
{
    public:
        spell_hun_throwing_axes() : SpellScriptLoader("spell_hun_throwing_axes") { }

        class spell_hun_throwing_axes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_throwing_axes_SpellScript);

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    l_Caster->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_throwing_axes_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_throwing_axes_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Bestial Wrath - 19574
/// Called for The Beast Within - 212668
class spell_hunter_the_beast_within : public SpellScriptLoader
{
    public:
        spell_hunter_the_beast_within() : SpellScriptLoader("spell_hunter_the_beast_within") { }

        class spell_hunter_the_beast_within_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hunter_the_beast_within_AuraScript);

            enum eSpells
            {
                TheBeastWithin              = 212668,
                TheBeastWithinImmuneEffects = 212704
            };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::TheBeastWithin))
                    l_Caster->CastSpell(l_Caster, eSpells::TheBeastWithinImmuneEffects, true);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::TheBeastWithin) && l_Caster->HasAura(eSpells::TheBeastWithinImmuneEffects))
                    l_Caster->RemoveAurasDueToSpell(eSpells::TheBeastWithinImmuneEffects);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hunter_the_beast_within_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hunter_the_beast_within_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hunter_the_beast_within_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Tar Trap - 135299
/// Called for Sticky Tar - 203264
class spell_hun_sticky_tar : public SpellScriptLoader
{
    public:
        spell_hun_sticky_tar() : SpellScriptLoader("spell_hun_sticky_tar") { }

        class spell_hun_sticky_tar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_sticky_tar_AuraScript);

            enum eSpells
            {
                TarTrap         = 135299,
                StickyTar       = 203264,
                StickyTarEffect = 203266,
                ExpertTrapper   = 199543,
                SuperStickyTar  = 201158
            };

            void HandleAfterEffect(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::StickyTar) && l_Target->HasAura(eSpells::TarTrap))
                    l_Target->CastSpell(l_Target, eSpells::StickyTarEffect, true);

                if (l_Caster->HasAura(ExpertTrapper) && roll_chance_i(50))
                    l_Caster->CastSpell(l_Target, eSpells::SuperStickyTar, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_sticky_tar_AuraScript::HandleAfterEffect, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_sticky_tar_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Kill Command - 34026
/// Called for Go to the Throat - 212670
class spell_hun_go_to_the_throat : public SpellScriptLoader
{
    public:
        spell_hun_go_to_the_throat() : SpellScriptLoader("spell_hun_go_to_the_throat") { }

        class spell_hun_go_to_the_throat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_go_to_the_throat_SpellScript);

            enum eSpells
            {
                GoToTheThroat = 212670
            };

            int32 l_TargetPct = 0;

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_TargetPct = l_Target->GetHealthPct();
            }

            void HandleAfterHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();
                int32 l_RequiredPct = (sSpellMgr->GetSpellInfo(eSpells::GoToTheThroat))->Effects[EFFECT_0].BasePoints;
                if (!l_Player || !l_Player->HasAura(eSpells::GoToTheThroat) || !l_Target || !l_RequiredPct)
                    return;

                if (l_TargetPct > l_RequiredPct)
                    return;

                l_Player->RemoveSpellCooldown(GetSpellInfo()->Id, true);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_hun_go_to_the_throat_SpellScript::HandleBeforeCast);
                AfterHit += SpellHitFn(spell_hun_go_to_the_throat_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_go_to_the_throat_SpellScript();
        }
};

/// Last update 7.0.3: 22522
/// Called by Trick Shot (target selection spell) - 199885
/// Called for Trick Shot - 199522
class spell_hun_trick_shot : public SpellScriptLoader
{
    public:
        spell_hun_trick_shot() : SpellScriptLoader("spell_hun_trick_shot") { }

        class spell_hun_trick_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_trick_shot_SpellScript);

            enum eSpells
            {
                Vulnerable               = 187131,
                AimedShotRicochet        = 168941,
                TrickShotNoTargetsEffect = 227272
            };

            void FilterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_OriginalTarget = GetExplTargetUnit();
                if (!l_Caster || !l_OriginalTarget)
                    return;

                p_Targets.remove_if([this, l_Caster, l_OriginalTarget](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->ToUnit())
                        return true;

                    /// Shouldn't richochet Aimed Shot to original target
                    if (l_OriginalTarget->GetGUID() == p_Target->GetGUID())
                        return true;

                    /// Can ricochet only to targets that have Vulnerable
                    if (p_Target->ToUnit()->HasAura(eSpells::Vulnerable, l_Caster->GetGUID()))
                        return false;

                    return true;
                });

                /// If there are no targets with Vulnerable, the damage of your next Aimed Shot is increased by 15%
                if (p_Targets.empty())
                    l_Caster->CastSpell(l_Caster, TrickShotNoTargetsEffect, true);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AimedShotRicochet, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_trick_shot_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_hun_trick_shot_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_trick_shot_SpellScript();
        }
};

/// Last Update: 7.0.3: 22522
/// Called by Aimed Shot Copy - 168941
/// Called for Trick Shot - 199522
class spell_hun_aimed_shot_copy : public SpellScriptLoader
{
    public:
        spell_hun_aimed_shot_copy() : SpellScriptLoader("spell_hun_aimed_shot_copy") { }

        class spell_hun_aimed_shot_copy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_aimed_shot_copy_SpellScript);

            enum eSpells
            {
                TrickShot = 199522
            };

            void HandlePrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_TrickShot = l_Caster->GetAura(eSpells::TrickShot))
                    if (l_TrickShot->m_SpellHelper.GetBool(eSpellHelpers::TrickShot))
                        GetSpell()->SetCustomCritChance(100.0f);
            }

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (Aura* l_TrickShot = l_Caster->GetAura(eSpells::TrickShot))
                {
                    if (uint32 l_Damage = l_TrickShot->m_SpellHelper.GetUint32(eSpellHelpers::TrickShot))
                    {
                        SetHitDamage(l_Damage);
                    }
                }
                else
                    SetHitDamage(0);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_hun_aimed_shot_copy_SpellScript::HandlePrepare);
                OnEffectHitTarget += SpellEffectFn(spell_hun_aimed_shot_copy_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_aimed_shot_copy_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Raptor Strike - 186270 - Carve - 187708
/// Called for Serpent Sting - 87935
class spell_hun_serpent_sting : public SpellScriptLoader
{
    public:
        spell_hun_serpent_sting() : SpellScriptLoader("spell_hun_serpent_sting") { }

        class spell_hun_serpent_sting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_serpent_sting_SpellScript);

            enum eSpells
            {
                SerpentSting        = 87935,
                SerpentStingEffect  = 118253,
                BirdOfPrey          = 224764,
                HealSpell           = 224765,
                InForTheKill        = 252095, ///< Hunter T21 Survival 4P: spell ID: 251752: buff should be removed on Raptor Strike Cast
                ExposedFlank        = 252094,
                RaptorStrike        = 186270
            };

            enum eTraits
            {
                BirdOfPrayID = 1077
            };

            void HandleT21Survival4P()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->RemoveAura(eSpells::InForTheKill);
            }

            void HandleT21Survival2P()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->RemoveAura(eSpells::ExposedFlank);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::SerpentSting))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SerpentStingEffect, true);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || !l_Caster->HasAura(eSpells::BirdOfPrey))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BirdOfPrey);
                if (!l_SpellInfo)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    int32 l_HealAmount = CalculatePct(GetHitDamage(), l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eTraits::BirdOfPrayID));
                    l_Caster->CastCustomSpell(l_Caster, eSpells::HealSpell, &l_HealAmount, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_serpent_sting_SpellScript::HandleOnHit);
                AfterHit += SpellHitFn(spell_hun_serpent_sting_SpellScript::HandleAfterHit);

                if (m_scriptSpellId == eSpells::RaptorStrike)
                {
                    AfterHit += SpellHitFn(spell_hun_serpent_sting_SpellScript::HandleT21Survival2P);
                    AfterCast += SpellCastFn(spell_hun_serpent_sting_SpellScript::HandleT21Survival4P);
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_serpent_sting_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Aimed Shot - 19434
class spell_hun_careful_aim : public SpellScriptLoader
{
    public:
        spell_hun_careful_aim() : SpellScriptLoader("spell_hun_careful_aim") { }

        class spell_hun_careful_aim_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_careful_aim_SpellScript);

            enum eSpells
            {
                CarefulAim    = 53238,
                CarefulAimDOT = 63468
            };

            bool m_IsCarefulAim = false;

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Aura* l_CarefulAim = l_Caster->GetAura(eSpells::CarefulAim))
                {
                    if (l_Target->GetHealthPct() > l_CarefulAim->GetEffect(1)->GetAmount())
                        m_IsCarefulAim = true;
                }
            }

            void HandleDamage()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Aura* l_CarefulAim = l_Caster->GetAura(eSpells::CarefulAim))
                {
                    if (m_IsCarefulAim && GetSpell()->IsCritForTarget(l_Target))
                    {
                        /// Calculate damage per 1 tick for Careful Aim DOT
                        /// 30% of hit damage (hit damage * 2.0 - because of crit) for 8 ticks
                        int32 l_Damage = int32(CalculatePct(int32(GetHitDamage() * 2.f), l_CarefulAim->GetEffect(EFFECT_2)->GetAmount()));
                        int32 l_PreviousTotalDamage = 0;
                        if (AuraEffect* l_PreviousCarefulAimDOT = l_Target->GetAuraEffect(eSpells::CarefulAimDOT, EFFECT_0, l_Caster->GetGUID()))
                        {
                            int32 l_PeriodicDamage = l_PreviousCarefulAimDOT->GetAmount();
                            int32 l_Amplitude = l_PreviousCarefulAimDOT->GetAmplitude();

                            if (l_Amplitude)
                                l_PreviousTotalDamage = l_PeriodicDamage * (l_PreviousCarefulAimDOT->GetTotalTicks() - l_PreviousCarefulAimDOT->GetTickNumber());

                            l_PreviousTotalDamage /= l_PreviousCarefulAimDOT->GetTotalTicks();
                        }

                        if (l_Target->HasAura(eSpells::CarefulAimDOT, l_Caster->GetGUID()))
                        {
                            if (Aura* l_ActualCarefulAimDOT = l_Target->GetAura(eSpells::CarefulAimDOT, l_Caster->GetGUID()))
                                l_ActualCarefulAimDOT->SetDuration(l_ActualCarefulAimDOT->GetMaxDuration());
                        }
                        else
                            l_Caster->CastSpell(l_Target, eSpells::CarefulAimDOT, true);

                        if (AuraEffect* l_NewCarefulAimDOT = l_Target->GetAuraEffect(eSpells::CarefulAimDOT, EFFECT_0, l_Caster->GetGUID()))
                        {
                            if (l_NewCarefulAimDOT->GetAmplitude())
                                l_Damage /= std::max(1, int32(l_NewCarefulAimDOT->GetTotalTicks() - l_NewCarefulAimDOT->GetTickNumber()));

                            l_Damage += l_PreviousTotalDamage;
                            l_NewCarefulAimDOT->SetAmount(l_Damage);

                            l_NewCarefulAimDOT->ResetPeriodic(true);
                        }
                    }
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_hun_careful_aim_SpellScript::HandleBeforeCast);
                OnHit += SpellHitFn(spell_hun_careful_aim_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_careful_aim_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
class PlayerScript_hun_black_arrow : public PlayerScript
{
    public:
        PlayerScript_hun_black_arrow() : PlayerScript("PlayerScript_hun_black_arrow") {}

        enum eSpells
        {
            BlackArrow          = 194599
        };

        void OnKill(Player* p_Killer, Unit* p_Killed, bool p_BeforeAuraRemoved) override
        {
            p_Killer->RemoveSpellCooldown(eSpells::BlackArrow, true);
        }
};


class PlayerScript_hun_terms_of_engagement : public PlayerScript
{
public:
    PlayerScript_hun_terms_of_engagement() : PlayerScript("PlayerScript_hun_terms_of_engagement") {}

    enum eSpells
    {
        TermsOfEngagement = 203754,
        Harpoon = 190925,
        HarpoonJump = 186260,
        HarpoonRoot = 190927,
        Harpoon2 = 232977
    };

    void OnKill(Player* p_Killer, Unit* p_Killed, bool p_BeforeAuraRemoved) override
    {
        if (p_Killer->HasAura(eSpells::TermsOfEngagement))
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Harpoon);

            if (SpellCategoryEntry const* l_CastegoryEntry = l_SpellInfo->ChargeCategoryEntry)
                p_Killer->RestoreCharge(l_CastegoryEntry);
        }
    }

    void OnKillReward(Unit* p_Killed, Unit* p_Attacker) override
    {
        Player* l_Player = p_Attacker->ToPlayer();
        if (!l_Player)
            return;

        if (!l_Player->IsInMap(p_Killed))
            return;

        if (l_Player->HasAura(eSpells::TermsOfEngagement))
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Harpoon);

            if (SpellCategoryEntry const* l_CastegoryEntry = l_SpellInfo->ChargeCategoryEntry)
                l_Player->RestoreCharge(l_CastegoryEntry);
        }
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Kill Command Trigger - 83381
/// Called For Jaws Of Thunder - 197162
class spell_hun_jaws_of_thunder : public SpellScriptLoader
{
    public:
        spell_hun_jaws_of_thunder() : SpellScriptLoader("spell_hun_jaws_of_thunder") { }

        class spell_hun_jaws_of_thunder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_jaws_of_thunder_SpellScript);

            enum eSpells
            {
                JawsOfThunder           = 197162,
                JawsOfThunderDamages    = 197163
            };

            enum eTraits
            {
                JawsOfThunderTrait = 875
            };

            enum eEntry
            {
                Hati = 100324
            };

            void HandleAfterHit()
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::JawsOfThunder);
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_SpellInfo || !l_Caster || !l_Target)
                    return;

                Pet* l_Pet = l_Caster->ToPet();
                if (!l_Pet)
                {
                    if (l_Caster->GetEntry() != eEntry::Hati)
                        return;
                }

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner || !l_Owner->HasAura(eSpells::JawsOfThunder))
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_Chance = l_Player->GetRankOfArtifactPowerId(eTraits::JawsOfThunderTrait) * l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (!roll_chance_i(l_Chance))
                    return;

                int32 l_Damage = GetHitDamage();
                l_Damage = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_1].BasePoints);

                l_Caster->CastCustomSpell(l_Target, eSpells::JawsOfThunderDamages, &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hun_jaws_of_thunder_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_jaws_of_thunder_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Titan's Thunder - 207068
class spell_hun_titans_thunder : public SpellScriptLoader
{
    public:
        spell_hun_titans_thunder() : SpellScriptLoader("spell_hun_titans_thunder") { }

        class spell_hun_titans_thunder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_titans_thunder_SpellScript);

            enum eSpells
            {
                TitansThunderPeriodic = 207094,
                TitansThunderBuff     = 218638,
                DireFrenzy            = 217200
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                std::list<Unit*> l_PetList;

                if (Creature* l_Hati = l_Player->GetHati())
                    l_PetList.push_back(l_Hati);

                if (Pet* l_Pet = l_Player->GetPet())
                    l_PetList.push_back(l_Pet);

                if (!l_Player->HasSpell(eSpells::DireFrenzy))
                {
                    for (Unit* l_Summon : l_Player->m_Controlled)
                    {
                        if (l_Summon->ToCreature() && l_Summon->ToCreature()->IsDireBeast())
                            l_PetList.push_back(l_Summon);
                    }
                }
                else
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                        l_Player->CastSpell(l_Pet, eSpells::TitansThunderBuff, true);
                }

                for (Unit* l_Target : l_PetList)
                    l_Player->CastSpell(l_Target, eSpells::TitansThunderPeriodic, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hun_titans_thunder_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_titans_thunder_SpellScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called by Titan's Thunder (Periodic) - 207094
class spell_hun_titans_thunder_periodic : public SpellScriptLoader
{
    public:
        spell_hun_titans_thunder_periodic() : SpellScriptLoader("spell_hun_titans_thunder_periodic") { }

        class spell_hun_titans_thunder_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_titans_thunder_periodic_AuraScript);

            enum eSpells
            {
                TitansThunderDamage = 207097
            };

            void HandleOnPeriodic(AuraEffect const* aurEff)
            {
                Unit* l_Caster = GetTarget();
                if (!l_Caster)
                    return;

                Unit* l_Target = l_Caster->getVictim();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::TitansThunderDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_titans_thunder_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_titans_thunder_periodic_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called by Deadly Aim - 190449
class spell_hun_deadly_aim : public SpellScriptLoader
{
public:
    spell_hun_deadly_aim() : SpellScriptLoader("spell_hun_deadly_aim") { }

    class spell_hun_deadly_aim_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_deadly_aim_AuraScript);

        enum eArtifactPowerId
        {
            DeadlyAimArtifact = 312
        };

        void HandleCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            uint32 l_TraitRank = l_Caster->ToPlayer()->GetRankOfArtifactPowerId(eArtifactPowerId::DeadlyAimArtifact);
            float l_Value = 5.0f;
            for (int32 l_I = 1; l_I < l_TraitRank && l_TraitRank >= 2; l_I++)
                l_Value += 2.5f;

            spellMod->value = (int32)ceil(l_Value);
        }

        void Register() override
        {
            DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_hun_deadly_aim_AuraScript::HandleCalcSpellMod, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_deadly_aim_AuraScript();
    }
};

/// Last Update 7.1.5 build 23420
/// Called By Stampede - 201631
class spell_hun_stampede : public SpellScriptLoader
{
    public:
        spell_hun_stampede() : SpellScriptLoader("spell_hun_stampede") { }

        class spell_hun_stampede_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_stampede_AuraScript);

            enum eSpells
            {
                StampedeBear = 201591,
                StampedeBird = 201610
            };

            std::vector<uint32> m_StampedeSpells =
            {
                eSpells::StampedeBear,
                eSpells::StampedeBird
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (int8 l_I = 0; l_I < 3; ++l_I)
                    l_Caster->CastSpell(l_Caster, m_StampedeSpells[urand(0, m_StampedeSpells.size() - 1)], true, nullptr, nullptr, l_Caster->GetOwnerGUID());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_stampede_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_stampede_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Eagle's Bite - 203757
class spell_hun_eagles_bite : public SpellScriptLoader
{
    public:
        spell_hun_eagles_bite() : SpellScriptLoader("spell_hun_eagles_bite") { }

        class spell_hun_eagles_bite_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_eagles_bite_AuraScript);

            enum eSpells
            {
                Harpoon = 190925
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Harpoon)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_eagles_bite_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_eagles_bite_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By On the Trail - 204081
class spell_hun_on_the_trail : public SpellScriptLoader
{
    public:
        spell_hun_on_the_trail() : SpellScriptLoader("spell_hun_on_the_trail") { }

        class spell_hun_on_the_trail_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_on_the_trail_AuraScript);

            void HandleProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Actor = p_EventInfo.GetActor();
                if (!l_Caster || !l_Actor)
                    return;

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo)
                    return;

                if (l_Caster->GetGUID() != l_Actor->GetGUID())
                    return;

                int32 maxDuration = GetMaxDuration() * 3;
                int32 newDuration = GetDuration() + 6 * IN_MILLISECONDS;
                if (newDuration > maxDuration)
                    newDuration = maxDuration;

                SetDuration(newDuration);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_on_the_trail_AuraScript::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_on_the_trail_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Aspect Of The Skylord - 203755
class spell_hun_aspect_of_the_skylord : public SpellScriptLoader
{
    public:
        spell_hun_aspect_of_the_skylord() : SpellScriptLoader("spell_hun_aspect_of_the_skylord") { }

        class spell_hun_aspect_of_the_skylord_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_aspect_of_the_skylord_AuraScript);

            enum eSpells
            {
                AspectOfTheEagle = 186289
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::AspectOfTheEagle)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_aspect_of_the_skylord_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_aspect_of_the_skylord_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Feign Death - 5384
class spell_hun_the_shadow_hunters_voodoo_mask : public SpellScriptLoader
{
    public:
        spell_hun_the_shadow_hunters_voodoo_mask() : SpellScriptLoader("spell_hun_the_shadow_hunters_voodoo_mask") { }

        class spell_hun_the_shadow_hunters_voodoo_mask_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_the_shadow_hunters_voodoo_mask_AuraScript);

            enum eSpells
            {
                ShadowHuntersVoodooMask = 208884,
                ShadowHuntersRegeneration = 208888
            };

            void HandleAfterApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                WorldPacket l_Data(SMSG_CLEAR_TARGET, 16 + 2);
                l_Data.appendPackGUID(l_Caster->GetGUID());

                std::list<Unit*> l_EnemyList;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(l_Caster, l_Caster, l_Player->GetVisibilityRange());
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(l_Caster, l_EnemyList, u_check);
                l_Caster->VisitNearbyObject(l_Player->GetVisibilityRange(), searcher);

                for (Unit* l_Enemy : l_EnemyList)
                {
                    if (l_Enemy->ToPlayer())
                        l_Enemy->ToPlayer()->SendDirectMessage(&l_Data);
                }

                if (!l_Caster->HasAura(eSpells::ShadowHuntersVoodooMask))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ShadowHuntersRegeneration, true);
            }

            void HandleAfterRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->HasAura(eSpells::ShadowHuntersRegeneration))
                    return;

                l_Caster->RemoveAura(eSpells::ShadowHuntersRegeneration);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_the_shadow_hunters_voodoo_mask_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_FEIGN_DEATH, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_hun_the_shadow_hunters_voodoo_mask_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_FEIGN_DEATH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_the_shadow_hunters_voodoo_mask_AuraScript();
        }
};


/// Last Update 7.3.5 Build 26365
/// Called by Carve - 187708, Butchery - 212436
class spell_hun_frizzos_fingertrap : public SpellScriptLoader
{
    public:
        spell_hun_frizzos_fingertrap() : SpellScriptLoader("spell_hun_frizzos_fingertrap") { }

        class spell_hun_frizzos_fingertrap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_frizzos_fingertrap_SpellScript);

            enum eSpells
            {
                Lacerate            = 185855,
                FrizzosFingerTrap   = 225155,
                Butchery            = 212436,
                Hellcarver          = 203673
            };

            enum eArtifactIds
            {
                HellcarverArtifact  = 1076
            };

            std::set<uint64> m_Targets;
            std::set<uint64> m_SpreadTargets;

            void HandleDamage(SpellEffIndex effIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_Targets.size() > 0 && l_Caster->HasAura(eSpells::Hellcarver))
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactIds::HellcarverArtifact);
                        uint32 l_Pct = l_Rank * 3;

                        if (l_Rank >= 3)
                            ++l_Pct;
                        if (l_Rank >= 6)
                            ++l_Pct;

                        int32 l_Damage = GetHitDamage() + CalculatePct(GetHitDamage(), l_Pct) * m_Targets.size();

                        SetHitDamage(l_Damage);
                    }
                }
            }

            void HandleAfterHit()
            {
                Unit* l_HitTarget = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_HitTarget || !l_Caster->HasAura(eSpells::FrizzosFingerTrap))
                    return;

                if (m_SpreadTargets.find(l_HitTarget->GetGUID()) != m_SpreadTargets.end())
                    return;

                if (!l_HitTarget->HasAura(eSpells::Lacerate))
                    return;

                if (Unit* l_SpreadTarget = FindClosestTargetWithoutLacerate(l_HitTarget, l_Caster->GetGUID()))
                {
                    m_SpreadTargets.insert(l_SpreadTarget->GetGUID());
                    l_Caster->CastSpell(l_SpreadTarget, eSpells::Lacerate, true);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::FrizzosFingerTrap))
                    return;

                if (m_SpreadTargets.empty())
                {
                    if (Unit* l_Target = FindTargetWithLowestLacerateDuration(l_Caster->GetGUID()))
                        l_Caster->CastSpell(l_Target, eSpells::Lacerate, true);
                }
            }

            Unit* FindTargetWithLowestLacerateDuration(uint64 p_CasterGUID)
            {
                Unit* l_LowestTarget = nullptr;
                int32 l_LowestDuration = 0;

                for (auto l_TargetGUID : m_Targets)
                {
                    Unit* l_Target = sObjectAccessor->FindUnit(l_TargetGUID);
                    Aura* l_Lacerate = nullptr;
                    int32 l_Duration = 0;

                    if (!l_Target || !(l_Lacerate = l_Target->GetAura(eSpells::Lacerate, p_CasterGUID)))
                        continue;

                    l_Duration = l_Lacerate->GetDuration();
                    if (!l_LowestTarget || l_Duration < l_LowestDuration)
                    {
                        l_LowestTarget = l_Target;
                        l_LowestDuration = l_Duration;
                    }
                }
                return l_LowestTarget;
            }

            Unit* FindClosestTargetWithoutLacerate(Unit* p_Origin, uint64 p_CasterGUID)
            {
                Unit* l_ClosestTarget = nullptr;
                float l_ClosestDistance = .0f;

                for (auto l_TargetGUID : m_Targets)
                {
                    Unit* l_Target = sObjectAccessor->FindUnit(l_TargetGUID);
                    float l_Distance = .0f;

                    if (!l_Target || l_Target->HasAura(eSpells::Lacerate, p_CasterGUID))
                        continue;

                    l_Distance = p_Origin->GetDistance(l_Target->GetPosition());
                    if (!l_ClosestTarget || l_Distance < l_ClosestDistance)
                    {
                        l_ClosestTarget = l_Target;
                        l_ClosestDistance = l_Distance;
                    }
                }
                return l_ClosestTarget;
            }

            void HandleTargetSelect(std::list<WorldObject*>& p_Targets)
            {
                for (auto l_Target : p_Targets)
                    m_Targets.insert(l_Target->GetGUID());

                m_SpreadTargets.clear();
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Butchery)
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_frizzos_fingertrap_SpellScript::HandleTargetSelect, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                    OnEffectHitTarget += SpellEffectFn(spell_hun_frizzos_fingertrap_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                }
                else
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_frizzos_fingertrap_SpellScript::HandleTargetSelect, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
                    OnEffectHitTarget += SpellEffectFn(spell_hun_frizzos_fingertrap_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
                }
                AfterHit += SpellHitFn(spell_hun_frizzos_fingertrap_SpellScript::HandleAfterHit);
                AfterCast += SpellCastFn(spell_hun_frizzos_fingertrap_SpellScript::HandleAfterCast);

            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_frizzos_fingertrap_SpellScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Tar Trap - 187700, Explosive Trap - 13812, Freezing Trap - 3355, Steel Trap - 162480, Caltrops - 194279
class spell_hun_nesingwarys_trapping_treads : public SpellScriptLoader
{
    public:
        spell_hun_nesingwarys_trapping_treads() : SpellScriptLoader("spell_hun_nesingwarys_trapping_treads") { }

        class spell_hun_nesingwarys_trapping_treads_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_nesingwarys_trapping_treads_SpellScript);

            enum eSpells
            {
                NesingwarysTrappingTread        = 212574,
                NesingwarysTrappingTreadProc    = 212575
            };

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->HasAura(eSpells::NesingwarysTrappingTread))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::NesingwarysTrappingTreadProc, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_nesingwarys_trapping_treads_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_nesingwarys_trapping_treads_SpellScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Aimed Shot - 19434, Marked Shot - 185901, Arcane Shot - 185358, Multi-Shot - 2643
class spell_hun_ullrs_featherweight_snowshoes : public SpellScriptLoader
{
    public:
        spell_hun_ullrs_featherweight_snowshoes() : SpellScriptLoader("spell_hun_ullrs_featherweight_snowshoes") { }

        class spell_hun_ullrs_featherweight_snowshoes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_ullrs_featherweight_snowshoes_SpellScript);

            enum eSpells
            {
                UllrsDeatherWeightSnowshoes = 206889,
                Trueshot = 193526
            };

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;

                if (!l_Caster || !l_Caster->HasAura(eSpells::UllrsDeatherWeightSnowshoes) || !(l_Player = l_Caster->ToPlayer()))
                    return;

                if (AuraEffect const* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::UllrsDeatherWeightSnowshoes, SpellEffIndex::EFFECT_0))
                {
                    if (l_Player->HasSpellCooldown(eSpells::Trueshot))
                        l_Player->ReduceSpellCooldown(eSpells::Trueshot, abs(l_AuraEffect->GetAmount()));
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_ullrs_featherweight_snowshoes_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_ullrs_featherweight_snowshoes_SpellScript();
        }
};

/// Pet Active (BM Artifact Weapon)
class PlayerScript_Pet_Active : public PlayerScript
{
    public:
        PlayerScript_Pet_Active() :PlayerScript("PlayerScript_Pet_Active") {}

        enum eSpells
        {
            PetActive = 166615
        };

        void OnSummonPet(Player* p_Player)
        {
            if (!p_Player || p_Player->getClass() != CLASS_HUNTER)
                return;

            if (p_Player->HasAura(155228)) ///< Lone Wolf
                return;

            p_Player->CastSpell(p_Player, eSpells::PetActive, true);
        }

        void OnMinionDespawn(Player* p_Player, Minion* p_Minion)
        {
            if (!p_Player || p_Player->getClass() != CLASS_HUNTER || !p_Minion->isPet())
                return;

            p_Player->RemoveAura(eSpells::PetActive);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Trailblazer - 199921
class spell_hun_trailblazer : public SpellScriptLoader
{
    public:
        spell_hun_trailblazer() : SpellScriptLoader("spell_hun_trailblazer") { }

        class spell_hun_trailblazer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_trailblazer_AuraScript);

            enum eSpells
            {
                Trailblazer = 231390
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Trailblazer, true);
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::Trailblazer);

                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                l_Aura->RefreshTimers();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_trailblazer_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                OnProc += AuraProcFn(spell_hun_trailblazer_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_trailblazer_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Butcher's Bone Apron - 236447
class spell_hun_butchers_bone_apron : public SpellScriptLoader
{
    public:
        spell_hun_butchers_bone_apron() : SpellScriptLoader("spell_hun_butchers_bone_apron") { }

        class spell_hun_butchers_bone_apron_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_butchers_bone_apron_AuraScript);

            enum eSpells
            {
                MongooseBite = 190928
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::MongooseBite)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_butchers_bone_apron_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_butchers_bone_apron_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Explosive Shot: Detonate! - 212679
class spell_hun_explosive_shot_detonate : public SpellScriptLoader
{
    public:
        spell_hun_explosive_shot_detonate() : SpellScriptLoader("spell_hun_explosive_shot_detonate") { }

        class spell_hun_explosive_shot_detonate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_explosive_shot_detonate_SpellScript);

            enum eSpells
            {
                ExplosiveShot       = 212431,
                ExplosiveShotDamage = 212680
            };

            void HandleDummy(SpellEffIndex effIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AreaTrigger* l_AreaTrigger = l_Caster->GetAreaTrigger(eSpells::ExplosiveShot))
                {
                    if (l_AreaTrigger->GetDuration() > 0)
                    {
                        l_Caster->CastSpell(l_AreaTrigger->GetPositionX(), l_AreaTrigger->GetPositionY(), l_AreaTrigger->GetPositionZ(), eSpells::ExplosiveShotDamage, true);
                        l_AreaTrigger->SetDuration(0);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_explosive_shot_detonate_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_explosive_shot_detonate_SpellScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Explosive Shot - 212680
class spell_hun_explosive_shot_damage : public SpellScriptLoader
{
    public:
        spell_hun_explosive_shot_damage() : SpellScriptLoader("spell_hun_explosive_shot_damage") { }

        class spell_hun_explosive_shot_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_explosive_shot_damage_SpellScript);

            enum eSpells
            {
                ExplosiveShot = 212431,
                ExplosiveShotDamage = 212680
            };

            void HandleDamage(SpellEffIndex effIndex)
            {
                Unit* l_Target = GetHitUnit();
                WorldLocation const* l_Loc = GetExplTargetDest();
                if (!l_Target || !l_Loc)
                    return;

                uint32 maxDamage = GetHitDamage();
                uint32 minDamage = CalculatePct(maxDamage, 30);

                float l_MaxDistance = 8.f;

                float l_Distance = l_Target->GetDistance(*l_Loc);

                float damage = maxDamage - (l_Distance * ((maxDamage - minDamage) / l_MaxDistance));

                SetHitDamage(damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_explosive_shot_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_explosive_shot_damage_SpellScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called for The Apex Predator's Claw - 212329
class PlayerScript_Apex_Predator_Claw : public PlayerScript
{
    public:
        PlayerScript_Apex_Predator_Claw() :PlayerScript("PlayerScript_Apex_Predator_Claw") {}

        enum eSpells
        {
            ApexPredatorsClaw   = 212329,
        };

        enum eItems
        {
            ApexPredatorsClawItem   = 137382,
        };

        void OnSummonPet(Player* p_Player)
        {
            if (!p_Player || p_Player->GetActiveSpecializationID() != SPEC_HUNTER_BEASTMASTERY)
                return;

            if (!p_Player->HasAura(eSpells::ApexPredatorsClaw) && !p_Player->HasItemOrGemWithIdEquipped(eItems::ApexPredatorsClawItem, 1))
                return;

            Unit* l_Pet = p_Player->GetPet();
            if (!l_Pet)
                return;

            p_Player->CastSpell(p_Player, eSpells::ApexPredatorsClaw, true);
        }
};

/// Last Update 7.3.2 build 25549
/// Called by The Apex Predator's Claw - 212329
class spell_hun_apex_predator_claw : public SpellScriptLoader
{
    public:
        spell_hun_apex_predator_claw() : SpellScriptLoader("spell_hun_apex_predator_claw") { }

        class spell_hun_apex_predator_claw_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_apex_predator_claw_AuraScript);

            enum eSpells
            {
                ApexPredatorsClaw   = 212329,
                SpikedCollar        = 53184,
                BloodOfTheRhino     = 53482,
                GreatStamina        = 61688,
                BoarsSpeed          = 19596,
                Cornered            = 53497
            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::SpikedCollar,
                eSpells::BloodOfTheRhino,
                eSpells::GreatStamina,
                eSpells::BoarsSpeed,
                eSpells::Cornered
            };

            void HandleAfterEffectApply(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target || l_Target->isPet())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SPEC_HUNTER_BEASTMASTERY)
                    return;

                Pet* l_Pet = l_Player->GetPet();
                if (!l_Pet)
                    return;

                /// Needed to apply spellmod from this aura to the 'Combat Experience' Aura from the pet (20782)
                l_Pet->RecalculateAllAurasAmount();

                for (eSpells l_Spell : m_Spells)
                    if (!l_Pet->HasSpell(l_Spell) && !l_Pet->HasAura(l_Spell))
                        l_Pet->LearnSpell(l_Spell);
            }

            void HandleAfterEffectRemove(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SPEC_HUNTER_BEASTMASTERY)
                    return;

                Pet* l_Pet = l_Player->GetPet();
                if (!l_Pet)
                    return;

                l_Pet->RemoveAura(eSpells::ApexPredatorsClaw);
                l_Pet->RecalculateAllAurasAmount();

                for (eSpells l_Spell : m_Spells)
                {
                    bool l_Continue = false;
                    if (std::vector<SpecializationSpellEntry const*> const* l_SpecializationSpells = GetSpecializationSpells(l_Pet->GetSpecialization()))
                    {
                        for (size_t l_J = 0; l_J < l_SpecializationSpells->size(); ++l_J)
                        {
                            SpecializationSpellEntry const* specSpell = (*l_SpecializationSpells)[l_J];

                            if (specSpell->SpellID == l_Spell)
                                l_Continue = true;
                        }
                    }

                    if (l_Continue)
                        continue;

                    if (l_Pet->HasSpell(l_Spell))
                        l_Pet->UnlearnSpell(l_Spell, false);

                    if (l_Pet->HasAura(l_Spell))
                        l_Pet->RemoveAura(l_Spell);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_apex_predator_claw_AuraScript::HandleAfterEffectApply, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_apex_predator_claw_AuraScript::HandleAfterEffectRemove, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_apex_predator_claw_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Intimidation - 19577
class spell_hun_intimidation : public SpellScriptLoader
{
    public:
        spell_hun_intimidation() : SpellScriptLoader("spell_hun_intimidation") { }

        class spell_hun_intimidation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_intimidation_AuraScript);

            enum eSpells
            {
                IntimidationStun = 24394
            };

            void HandleProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::IntimidationStun, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_intimidation_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_intimidation_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Sentinel's Sight - 208912
class spell_hun_sentinels_sight : public SpellScriptLoader
{
    public:
        spell_hun_sentinels_sight() : SpellScriptLoader("spell_hun_sentinels_sight") { }

        class spell_hun_sentinels_sight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_sentinels_sight_AuraScript);

            enum eSpells
            {
                MultiShot = 2643,
                Sidewinders = 214581
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::MultiShot && l_SpellInfo->Id != eSpells::Sidewinders))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_sentinels_sight_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_sentinels_sight_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called by Bullseye - 204089
class spell_hun_bullseye : public SpellScriptLoader
{
    public:
        spell_hun_bullseye() : SpellScriptLoader("spell_hun_bullseye") { }

        class spell_hun_bullseye_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_bullseye_AuraScript);

            enum eSpells
            {
                MultiShot = 2643
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_DamageInfo || !l_SpellInfo)
                    return false;

                Unit* l_ProcTarget = l_DamageInfo->GetTarget();
                if (!l_ProcTarget || l_ProcTarget->GetHealthPct() > l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_bullseye_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_bullseye_AuraScript();
        }
};

/// last update : 7.1.5 23420
/// Patient Sniper - 234588
/// Called by Vulnerable 187131
class spell_hun_patient_sniper : public SpellScriptLoader
{
    public:
        spell_hun_patient_sniper() : SpellScriptLoader("spell_hun_patient_sniper") { }

        class spell_hun_patient_sniper_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_patient_sniper_AuraScript);

            enum eSpells
            {
                PatientSniper   = 234588,
                Vulnerable      = 187131
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (AuraEffect* pVulnerableEffect = p_AurEff->GetBase()->GetEffect(EFFECT_1))
                {
                    if (AuraEffect const* patientSniperEffect = caster->GetAuraEffect(eSpells::PatientSniper, EFFECT_0))
                    {
                        pVulnerableEffect->SetAmount(pVulnerableEffect->GetAmount() + patientSniperEffect->GetAmount());
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_patient_sniper_AuraScript::OnTick, EFFECT_3, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_patient_sniper_AuraScript();
        }
};

/// last update : 7.1.5 23420
/// Volley - 194386
class spell_hun_volley : public SpellScriptLoader
{
    public:
        spell_hun_volley() : SpellScriptLoader("spell_hun_volley") { }

        class spell_hun_volley_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_volley_AuraScript);

            enum eSpells
            {
                VolleyDamage = 194392
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_LastSpellCasted = l_Player->GetLastCastedSpell();
                TriggerCastFlags triggerFlags = TriggerCastFlags(TriggerCastFlags::TRIGGERED_FULL_MASK & ~TriggerCastFlags::TRIGGERED_IGNORE_POWER_AND_REAGENT_COST);
                l_Caster->CastSpell(l_Target, eSpells::VolleyDamage, triggerFlags);
                l_Player->SetLastCastedSpell(l_LastSpellCasted);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_volley_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_volley_AuraScript();
        }
};

/// last update : 7.1.5 23420
/// Sentinel - 206817
class spell_areatrigger_hun_sentinel : public AreaTriggerEntityScript
{
    public:
        spell_areatrigger_hun_sentinel() : AreaTriggerEntityScript("spell_areatrigger_hun_sentinel") { }

        enum eSpells
        {
            HuntersMark         = 185365,
            HuntersMarkCaster   = 185743
        };

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_areatrigger_hun_sentinel();
        }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            m_UpdateTimer = 0;
            m_Amplitude = p_AreaTrigger->GetSpellInfo()->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS;
            m_Radius = p_AreaTrigger->GetSpellInfo()->Effects[EFFECT_0].CalcRadius();
            m_MaxCount = 1 + p_AreaTrigger->GetUInt32Value(AREATRIGGER_FIELD_DURATION) / std::max(m_Amplitude, uint32(1));
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer <= p_Time)
            {
                m_UpdateTimer = m_Amplitude;

                ApplyMark(p_AreaTrigger);
            }
            else
            {
                m_UpdateTimer -= p_Time;
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            // HACK: marks are triggered every 6 seconds (at 0/6/12/18 secs), but the last mark cannot be triggered in 'Update'
            // So check if not all marks was applied
            if (m_MaxCount > 0)
                ApplyMark(p_AreaTrigger);
        }

    private:

        void ApplyMark(AreaTrigger* p_AreaTrigger)
        {
            if (m_MaxCount == 0)
                return;

            --m_MaxCount;

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<Unit*> l_TargetList;
            p_AreaTrigger->GetAttackableUnitListInPositionRange(l_TargetList, *p_AreaTrigger, m_Radius, true);

            for (auto l_Target : l_TargetList)
            {
                l_Caster->CastSpell(l_Target, eSpells::HuntersMark, true);
            }

            if (!l_TargetList.empty())
                l_Caster->CastSpell(l_Caster, eSpells::HuntersMarkCaster, true);
        }

    private:

        uint32 m_UpdateTimer;
        uint32 m_Amplitude;
        float m_Radius;
        uint32 m_MaxCount;
};


/// last update : 7.1.5 23420
/// Hunting Party - 212658
class spell_hun_hunting_party : public SpellScriptLoader
{
    public:
        spell_hun_hunting_party() : SpellScriptLoader("spell_hun_hunting_party") { }

        class spell_hun_hunting_party_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_hunting_party_AuraScript);

            enum eSpells
            {
                Exhilaration    = 109304
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                if (!GetUnitOwner() || !GetUnitOwner()->IsPlayer())
                    return;

                uint32 reduceAmount = p_AurEff->GetSpellEffectInfo()->BasePoints;
                GetUnitOwner()->ToPlayer()->ReduceSpellCooldown(eSpells::Exhilaration, reduceAmount * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_hunting_party_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_hunting_party_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Arcane Shot - 185358
class spell_hun_arcane_shot : public SpellScriptLoader
{
    public:
        spell_hun_arcane_shot() : SpellScriptLoader("spell_hun_arcane_shot") { }

        class spell_hun_arcane_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_arcane_shot_SpellScript);

            enum eSpells
            {
                ArcaneShotEnergize = 187675
            };

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ArcaneShotEnergize, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_arcane_shot_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_arcane_shot_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Aspect of the Turtle - 186265
class spell_hun_healing_shell : public SpellScriptLoader
{
public:
    spell_hun_healing_shell() : SpellScriptLoader("spell_hun_healing_shell") { }

    class spell_hun_healing_shell_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_healing_shell_AuraScript);

        enum eSpells
        {
            HealingShellAura = 190503,
            HealingShellBuff = 203924
        };

        enum eArtifactPowers
        {
            HealingShellArtifact = 316
        };

        void OnApply(AuraEffect const*, AuraEffectHandleModes)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->HasAura(eSpells::HealingShellAura))
                return;

            if (Player* l_Player = l_Caster->ToPlayer())
            {
                SpellInfo const* l_HealingShellInfo = sSpellMgr->GetSpellInfo(eSpells::HealingShellBuff);
                if (!l_HealingShellInfo)
                    return;

                int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::HealingShellArtifact);
                int32 l_Bp0 = l_Rank * l_HealingShellInfo->Effects[0].BasePoints;

                l_Caster->CastCustomSpell(l_Caster, eSpells::HealingShellBuff, &l_Bp0, nullptr, nullptr, true);
            }
        }

        void OnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (GetTargetApplication()->GetRemoveMode() != AuraRemoveMode::AURA_REMOVE_BY_CANCEL)
                l_Caster->RemoveAurasDueToSpell(eSpells::HealingShellBuff);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_hun_healing_shell_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_hun_healing_shell_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_healing_shell_AuraScript();
    }
};

/// Last Update 7.1.5 build 23420
/// Called By Exhilaration - 109304
class spell_hun_exhilaration : public SpellScriptLoader
{
public:
    spell_hun_exhilaration() : SpellScriptLoader("spell_hun_exhilaration") { }

    class spell_hun_exhilaration_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hun_exhilaration_SpellScript);

        enum eSpells
        {
            ExhilarationPet = 128594
        };

        void HandleCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (Guardian* l_Pet = l_Caster->GetGuardianPet())
                l_Caster->CastSpell(l_Pet, eSpells::ExhilarationPet, true);
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_hun_exhilaration_SpellScript::HandleCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_hun_exhilaration_SpellScript();
    }
};

/// Last Update 7.1.5 Build 22522
/// Called by Legacy of the Windrunners - 190854
class spell_hun_legacy_of_the_windrunners : public SpellScriptLoader
{
public:
    spell_hun_legacy_of_the_windrunners() : SpellScriptLoader("spell_hun_legacy_of_the_windrunners") { }

    class spell_hun_legacy_of_the_windrunners_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_legacy_of_the_windrunners_AuraScript);

        enum eSpells
        {
            AimedShot = 191043
        };

        void OnApply(AuraEffect const*, AuraEffectHandleModes)
        {
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            Unit* l_Caster = GetCaster();

            if (!l_SpellInfo || !l_Caster || !l_Caster->IsPlayer())
                return;

            Player* l_Player = l_Caster->ToPlayer();
            uint64 l_AimedShotTargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::LastAimedShotTarget);
            if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_AimedShotTargetGUID))
            {

                uint64 l_CasterGuid = l_Caster->GetGUID();
                uint64 l_TargetGuid = l_Target->GetGUID();

               for (uint32 l_Itr = 0; l_Itr < l_SpellInfo->Effects[0].BasePoints; l_Itr++)
                {
                    l_Caster->m_Functions.AddFunction([l_CasterGuid, l_TargetGuid]() -> void
                    {
                        Player* l_Player = sObjectAccessor->FindPlayer(l_CasterGuid);
                        if (!l_Player)
                            return;

                        Unit* l_Target = sObjectAccessor->GetUnit(*l_Player, l_TargetGuid);
                        if (!l_Target)
                            return;

                        l_Player->CastSpell(l_Target, eSpells::AimedShot, true);
                    }, l_Player->m_Functions.CalculateTime(50 * l_Itr));
                }
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_hun_legacy_of_the_windrunners_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_legacy_of_the_windrunners_AuraScript();
    }
};

/// Last Update 7.1.5 Build 22522
/// Called by Wild Protector - 204190
class spell_hun_wild_protector : public SpellScriptLoader
{
public:
    spell_hun_wild_protector() : SpellScriptLoader("spell_hun_wild_protector") { }

    class spell_hun_wild_protector_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_wild_protector_AuraScript);

        enum eSpells
        {
            WildProtectorAreaTrigger = 204358
        };

        void OnApply(AuraEffect const*, AuraEffectHandleModes)
        {
            Unit* l_Caster = GetCaster();
            Guardian* l_Pet = nullptr;

            if (!l_Caster || !(l_Pet = l_Caster->GetGuardianPet()))
                return;

            l_Pet->CastSpell(l_Pet, eSpells::WildProtectorAreaTrigger, true);
        }

        void OnRemove(AuraEffect const*, AuraEffectHandleModes)
        {
            Unit* l_Caster = GetCaster();
            Guardian* l_Pet = nullptr;

            if (!l_Caster || !(l_Pet = l_Caster->GetGuardianPet()))
                return;

            l_Pet->RemoveAurasDueToSpell(eSpells::WildProtectorAreaTrigger);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_hun_wild_protector_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectApplyFn(spell_hun_wild_protector_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_wild_protector_AuraScript();
    }
};

/// Last Update: 7.1.5 23420
/// Called by Rapid Killing - 191342
class spell_hun_rapid_killing_pvp : public SpellScriptLoader
{
    public:
        spell_hun_rapid_killing_pvp() : SpellScriptLoader("spell_hun_rapid_killing_pvp") { }

        class spell_hun_rapid_killing_pvp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_rapid_killing_pvp_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.60f;   ///< Rapid Killing Bonus in PVP is reduced by 40%
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_rapid_killing_pvp_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_CRIT_DAMAGE_BONUS);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_rapid_killing_pvp_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Steady Focus - 193533
class spell_hun_steady_focus : public SpellScriptLoader
{
    public:
        spell_hun_steady_focus() : SpellScriptLoader("spell_hun_steady_focus") { }

        class spell_hun_steady_focus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_steady_focus_AuraScript);

            enum eSpells
            {
                SteadyFocusBuff = 193534,
                ArcaneShot      = 185358,
                MultiShot       = 2643,
                Sidewinders     = 214581
            };

            void HandleOnProc(AuraEffect const* p_AuraEff, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (!p_ProcEventInfo.GetDamageInfo())
                    return;

                SpellInfo const* l_SpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::ArcaneShot:
                    case eSpells::MultiShot:
                    case eSpells::Sidewinders:
                        ++m_TimesInRow;
                        break;
                    default:
                        m_TimesInRow = 0;
                        break;
                }

                if (m_TimesInRow >= 2)
                {
                    l_Owner->CastSpell(l_Owner, eSpells::SteadyFocusBuff, true);

                    // Prevent overflow
                    m_TimesInRow = 2;
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_steady_focus_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }

        private:

            uint8 m_TimesInRow = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_steady_focus_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Separation Anxiety - 213875
class spell_hun_separation_anxiety : public SpellScriptLoader
{
    public:
        spell_hun_separation_anxiety() : SpellScriptLoader("spell_hun_separation_anxiety") { }

        class spell_hun_separation_anxiety_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_separation_anxiety_AuraScript);

            enum eSpells
            {
                SeparationAnxiety = 213882
            };

            bool m_Alone = false;

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Pet = GetUnitOwner();
                Unit* l_Owner = GetCaster();
                if (!l_Owner || !l_Pet)
                    return;

                if (m_Alone && l_Owner->GetTargetGUID() != l_Pet->GetTargetGUID() && l_Pet->GetTargetGUID() != 0)
                    l_Pet->CastSpell(l_Pet, eSpells::SeparationAnxiety, true);

                m_Alone = (l_Owner->GetTargetGUID() != l_Pet->GetTargetGUID());
                SetDuration(GetMaxDuration());
            }

            void HandleOnProc(AuraEffect const*, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = nullptr;
                if (!l_Caster || !(l_Owner = l_Caster->GetOwner()))
                    return;

                if (l_Caster->GetTargetGUID() == l_Owner->GetTargetGUID())
                {
                    m_Alone = false;
                    l_Caster->RemoveAurasDueToSpell(eSpells::SeparationAnxiety);

                    Aura* l_Aura = GetAura();
                    if (!l_Aura)
                        return;

                    l_Aura->RefreshTimers();
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_hun_separation_anxiety_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_separation_anxiety_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_separation_anxiety_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Separation Anxiety - 212669
class spell_hun_separation_anxiety_apply : public SpellScriptLoader
{
    public:
        spell_hun_separation_anxiety_apply() : SpellScriptLoader("spell_hun_separation_anxiety_apply") { }

        class spell_hun_separation_anxiety_apply_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_separation_anxiety_apply_AuraScript);

            enum eSpells
            {
                SeparationAnxiety = 213875
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Guardian* l_Pet = nullptr;

                if (!l_Caster || !(l_Pet = l_Caster->GetGuardianPet()))
                    return;

                l_Caster->CastSpell(l_Pet, eSpells::SeparationAnxiety, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_separation_anxiety_apply_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_separation_anxiety_apply_AuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Sidewinders 214579
class PlayerScript_Sidewinders : public PlayerScript
{
    public:
        PlayerScript_Sidewinders() :PlayerScript("PlayerScript_Sidewinders") {}

        enum eSpells
        {
            Sidewinders             = 214579,
            SidewindersAuraReplace  = 214641,
            PiercingShot            = 198670,
            TrickShot               = 199522
        };

        enum Talents
        {
            Sidewinder  = 22274
        };

        void OnModifySpec(Player* p_Player, int32 p_OldSpec, int32 /* p_NewSpec */) override
        {
            if (!p_Player)
                return;

            if (p_OldSpec == SPEC_HUNTER_MARKSMANSHIP)
                p_Player->RemoveAura(eSpells::SidewindersAuraReplace);

            if (p_OldSpec != SPEC_HUNTER_MARKSMANSHIP && p_Player->HasTalent(eSpells::Sidewinders, p_Player->GetActiveTalentGroup()))
                p_Player->CastSpell(p_Player, eSpells::SidewindersAuraReplace, true);

        }

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID) override
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_HUNTER_MARKSMANSHIP)
                return;

            switch (p_SpellID)
            {
                case eSpells::TrickShot:
                case eSpells::PiercingShot:
                    p_Player->RemoveAura(eSpells::SidewindersAuraReplace);
                    break;
                case eSpells::Sidewinders:
                    p_Player->CastSpell(p_Player, eSpells::SidewindersAuraReplace, true);
                    break;
                default:
                    break;
            }
        }

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player || !p_Player->HasTalent(Talents::Sidewinder, p_Player->GetActiveTalentGroup()))
                return;

            p_Player->AddAura(eSpells::SidewindersAuraReplace, p_Player);
        }
};

/// Update Legion 7.1.5 build 23420
/// Called by Emmarel's Assault - 219698
class spell_hun_emmarels_assault : public SpellScriptLoader
{
    public:
        spell_hun_emmarels_assault() : SpellScriptLoader("spell_hun_emmarels_assault") { }

        class spell_hun_emmarels_assault_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_emmarels_assault_SpellScript);

            enum eSpells
            {
                Volley                      = 220024,
                SummonEmmarelShadewarden    = 219700
            };

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest->GetPositionX(), l_Dest->GetPositionY(), l_Dest->GetPositionZ(), eSpells::Volley, true);
                l_Caster->CastSpell(l_Caster, eSpells::SummonEmmarelShadewarden, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_emmarels_assault_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_emmarels_assault_SpellScript();
        }
};

/// Update Legion 7.1.5 build 23420
/// Called by Kill Command - 34026, Flanking Strike - 202800
class spell_hun_aspect_of_the_beast : public SpellScriptLoader
{
    public:
        spell_hun_aspect_of_the_beast() : SpellScriptLoader("spell_hun_aspect_of_the_beast") { }

        class spell_hun_aspect_of_the_beast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_aspect_of_the_beast_SpellScript);

            enum eSpells
            {
                KillCommand                 = 34026,
                FlankingStrike              = 202800,
                AspectOfTheBeast            = 191384,
                AspectOfTheBeastFerocity    = 191413,
                AspectOfTheBeastTenacity    = 191414,
                AspectOfTheBeastCunning     = 191397
            };

            void HandleOnHit(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_PetUnit = l_Caster->GetGuardianPet();
                if (!l_PetUnit)
                    return;

                Pet* l_Pet = l_PetUnit->ToPet();
                if (!l_Pet)
                    return;

                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::AspectOfTheBeast))
                    return;

                switch (l_Pet->GetSpecialization())
                {
                    case SPEC_PET_FEROCITY:
                        l_Pet->CastSpell(l_Target, eSpells::AspectOfTheBeastFerocity, true);
                        break;
                    case SPEC_PET_TENACITY:
                        l_Pet->CastSpell(l_Target, eSpells::AspectOfTheBeastTenacity, true);
                        break;
                    case SPEC_PET_CUNNING:
                        l_Pet->CastSpell(l_Target, eSpells::AspectOfTheBeastCunning, true);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::KillCommand)
                    OnEffectHit += SpellEffectFn(spell_hun_aspect_of_the_beast_SpellScript::HandleOnHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
                else if (m_scriptSpellId == eSpells::FlankingStrike)
                    OnEffectHit += SpellEffectFn(spell_hun_aspect_of_the_beast_SpellScript::HandleOnHit, SpellEffIndex::EFFECT_2, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_aspect_of_the_beast_SpellScript();
        }
};

/// Update Legion 7.1.5 build 23420
/// Called by Multi-Shot - 2643
class spell_hunt_multishot : public SpellScriptLoader
{
    public:
        spell_hunt_multishot() : SpellScriptLoader("spell_hunt_multishot") { }

        class spell_hunt_multishot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hunt_multishot_SpellScript);

            enum eSpells
            {
                MultishotFocus = 213363,
            };

            enum eVisuals
            {
                MultiShotVisual = 66627
            };

            void GetTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (auto l_Target : p_Targets)
                {
                    if (!l_Target || !l_Target->ToUnit())
                        continue;

                    Position l_Pos = l_Target->GetPosition();
                    l_Pos.m_positionX += frand(-5.0f, 5.0f);
                    l_Pos.m_positionY += frand(-5.0f, 5.0f);
                    l_Pos.m_positionZ += 2.0f;

                    l_Caster->SendPlaySpellVisual(eVisuals::MultiShotVisual, nullptr, 50.0f, l_Pos);
                }
            }

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo || l_Caster->ToPlayer()->GetActiveSpecializationID() != SPEC_HUNTER_MARKSMANSHIP)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MultishotFocus, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_multishot_SpellScript::GetTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_hunt_multishot_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hunt_multishot_SpellScript();
        }
};

/// Update Legion 7.1.5 build 23420
/// Called by Critical Focus - 191328
class spell_hunt_critical_focus : public SpellScriptLoader
{
    public:
        spell_hunt_critical_focus() : SpellScriptLoader("spell_hunt_critical_focus") { }

        class spell_hunt_critical_focus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hunt_critical_focus_AuraScript);

            enum eSpells
            {
                Sidewinders    = 214581,
                ArcaneShot     = 185358,
                CriticalFocus  = 215107
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                if (l_SpellInfo->Id == eSpells::ArcaneShot && (p_EventInfo.GetHitMask() & PROC_HIT_CRITICAL))
                    l_Caster->CastSpell(l_Caster, eSpells::CriticalFocus, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_hunt_critical_focus_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hunt_critical_focus_AuraScript();
        }
};

/// Update Legion 7.1.5 build 23420
/// Called by Windburst - 204147
class spell_hunt_windburst : public SpellScriptLoader
{
    public:
        spell_hunt_windburst() : SpellScriptLoader("spell_hunt_windburst") { }

        class spell_hunt_windburst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hunt_windburst_SpellScript);

            enum eSpells
            {
                WindburstAreatrigger    = 204475,
                WindburstAreatrigger2   = 223114,
                MarkOfTheWindrunner     = 204219,
                Vulnerable              = 187131,
                CyclonicBurst           = 238124,
                CyclonicBurstDebuff     = 242712
            };

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::WindburstAreatrigger, true);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || !l_Caster->IsWithinLOSInMap(l_Target))
                    return;

                if (l_Caster->HasAura(eSpells::MarkOfTheWindrunner))
                    l_Caster->CastSpell(l_Target, eSpells::Vulnerable, true);

                float l_MaxDist = l_Caster->GetDistance2d(l_Target) + 1.f;
                float l_Dist = 1.0f;
                float l_Angle = l_Caster->GetAngle(l_Target);

                for (int32 l_I = 1; l_I <= (int32)l_MaxDist / 5; l_I++)
                {
                    float x = l_Caster->GetPositionX() + l_Dist * std::cos(l_Angle) * l_I;
                    float y = l_Caster->GetPositionY() + l_Dist * std::sin(l_Angle) * l_I;
                    float z = l_Caster->GetPositionZ();

                    l_Caster->UpdateAllowedPositionZ(x, y, z);
                    Position l_Position;
                    l_Position.Relocate(x, y, z, 0.0f);
                    l_Caster->CastSpell(l_Position, eSpells::WindburstAreatrigger2, true);
                    l_Dist = 5.f;
                }

                if (l_Caster->HasSpell(eSpells::CyclonicBurst))
                    l_Caster->CastSpell(l_Target, eSpells::CyclonicBurstDebuff, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hunt_windburst_SpellScript::HandleCast);
                OnHit += SpellHitFn(spell_hunt_windburst_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hunt_windburst_SpellScript();
        }
};

/// Update Legion 7.1.5 build 23420
/// Called by Sidewinder - 214579, 240711
class spell_hunt_sidewinders: public SpellScriptLoader
{
    public:
        spell_hunt_sidewinders() : SpellScriptLoader("spell_hunt_sidewinders") { }

        class spell_hunt_sidewinders_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hunt_sidewinders_SpellScript);

            enum eSpells
            {
                Vulnerable              = 187131,
                SideWindersDamages      = 214581,
                CriticalFocusAura       = 191328,
                CriticalFocus           = 215107,
                HunterMarkCaster        = 223138,
                HuntersMarkedShot       = 185743,
                HunterMark              = 185365,
                Trueshot                = 193526,
                Sidewinder              = 240711
            };

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                bool l_MarkApplication = false;
                if (!l_Caster || !l_Target)
                    return;

                p_Targets.remove_if([this, l_Target, l_Caster](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->ToUnit() || p_Target->ToUnit()->IsImmunedToSpell(GetSpellInfo()))
                        return true;

                    if (p_Target->GetDistance2d(l_Caster) > l_Target->GetDistance2d(l_Caster))
                        return true;

                    return false;
                });

                bool l_IsTargetHit = true;
                if (l_Target->IsImmunedToSpell(GetSpellInfo()))
                    l_IsTargetHit = false;

                uint32 l_NbTargetHit = p_Targets.size() + l_IsTargetHit;
                /// Update to prevent Caster from getting HuntersMarkedShot buff if no target have been hit due to immune
                if (!l_NbTargetHit)
                    return;

                if (l_Caster->HasAura(eSpells::HunterMarkCaster) || l_Caster->HasAura(eSpells::Trueshot))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::HuntersMarkedShot, true);
                    l_MarkApplication = true;
                }

                for (auto l_Target : p_Targets)
                    if (l_Target)
                    {
                        l_Caster->CastSpell(static_cast<Unit*>(l_Target), eSpells::Vulnerable, true);
                        if (l_MarkApplication == true)
                            l_Caster->CastSpell(static_cast<Unit*>(l_Target), eSpells::HunterMark, true);
                    }

                for (auto l_Target : p_Targets)
                    if (l_Target)
                        l_Caster->CastSpell(static_cast<Unit*>(l_Target), eSpells::SideWindersDamages, true);
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::HunterMarkCaster);

                if (m_scriptSpellId != eSpells::Sidewinder)
                    return;

                if (!l_Caster->HasAura(eSpells::CriticalFocusAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CriticalFocus, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hunt_sidewinders_SpellScript::HandleOnCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_sidewinders_SpellScript::HandleTargets, EFFECT_2, TARGET_UNIT_CONE_ENEMY_54);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hunt_sidewinders_SpellScript();
        }
};

/// Update Legion 7.1.5 - build 23420
/// Aimed Shot
class PlayerScript_aimed_shot : public PlayerScript
{
    public:
        PlayerScript_aimed_shot() :PlayerScript("PlayerScript_aimed_shot") {}

        enum eSpells
        {
            AimedShotMarker = 236641
        };

        void OnDealDamage(Player* p_Player, Unit* p_Target, DamageEffectType p_DamageEffectType, uint32 p_Damage, SpellSchoolMask p_SchoolMask, CleanDamage const* p_CleanDamage) override
        {
            if (!p_Target)
                return;

            if (p_Player->getClass() == CLASS_HUNTER && p_Player->GetActiveSpecializationID() == SPEC_HUNTER_MARKSMANSHIP)
            {
                p_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShot].insert(p_Target->GetGUID());
                p_Player->m_SpellHelper.GetUint64(eSpellHelpers::AimedShot) = p_Damage;
                p_Target->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShotTarget].insert(p_Player->GetGUID());
            }
        }

        void OnLeaveCombat(Player* p_Player) override
        {
            p_Player->RemoveAurasDueToSpell(eSpells::AimedShotMarker);
            ClearSets(p_Player);
        }

        void OnKill(Player* p_Killer, Unit* p_Killed, bool p_BeforeAuraRemoved) override
        {
            ClearSets(p_Killed);
        }

        void ClearSets(Unit* p_Unit)
        {
            /// Remove p_Unit from hunter's list of all of its victims
            for (uint64 l_CasterGUID : p_Unit->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShotTarget])
            {
                Unit* l_Caster = sObjectAccessor->GetUnit(*p_Unit, l_CasterGUID);
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShot].erase(p_Unit->GetGUID());
            }

            p_Unit->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShotTarget].clear();

            /// Remove p_Unit from victim list of all hunter's which attacked him
            for (uint64 l_TargetGUID : p_Unit->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShot])
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_Unit, l_TargetGUID);
                if (!l_Target)
                    return;

                l_Target->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShotTarget].erase(p_Unit->GetGUID());
                l_Target->RemoveAurasDueToSpell(eSpells::AimedShotMarker);
            }

            p_Unit->m_SpellHelper.GetUint64Set()[eSpellHelpers::AimedShot].clear();
        }
};

/// Last Update 7.3.5 build 26365
/// Bursting Shot - 186387
class spell_hun_bursting_shot : public SpellScriptLoader
{
    public:
        spell_hun_bursting_shot() : SpellScriptLoader("spell_hun_bursting_shot") { }

        class spell_hun_bursting_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_bursting_shot_SpellScript);

            enum eSpells
            {
                BurstingShotConfuse = 224729,
                ConcussiveShot      = 5116,
                TNT                 = 209793,
                AMurderOfCrows      = 131894
            };

            std::vector<AuraType> m_DoTsAuraTypes =
            {
                SPELL_AURA_PERIODIC_DAMAGE,
                SPELL_AURA_PERIODIC_SCHOOL_DAMAGE,
                SPELL_AURA_PERIODIC_DAMAGE_PERCENT,
                SPELL_AURA_PERIODIC_LEECH,
                SPELL_AURA_PERIODIC_HEALTH_FUNNEL
            };

            void HandleOnHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return;

                uint64 l_TargetGUID = l_Target->GetGUID();
                l_Caster->AddDelayedEvent([l_Caster, l_TargetGUID]()
                {
                    if (Unit* l_Target = sObjectAccessor->FindUnit(l_TargetGUID))
                        l_Caster->AddAura(eSpells::BurstingShotConfuse, l_Target);
                }, 500);

                if(l_Caster->HasAura(eSpells::TNT))
                    l_Caster->CastSpell(l_Target, eSpells::ConcussiveShot, true);

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->CanApplyPvPSpellModifiers())
                {
                    for (auto l_AuraType : m_DoTsAuraTypes)
                        if (l_Target->HasAuraType(l_AuraType))
                            l_Target->RemoveAurasByType(l_AuraType);

                    l_Target->RemoveAurasDueToSpell(eSpells::AMurderOfCrows);
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hun_bursting_shot_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_bursting_shot_SpellScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Precision - 190520
class spell_hun_precision : public SpellScriptLoader
{
    public:
        spell_hun_precision() : SpellScriptLoader("spell_hun_precision") { }

        class spell_hun_precision_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_precision_AuraScript);

            enum eArtifactPowers
            {
                PrecisionArtifact = 318
            };

            void HandleCalcAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Owner = GetUnitOwner();

                if (!l_SpellInfo || !l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::PrecisionArtifact);
                    amount = l_Rank * l_SpellInfo->Effects[0].BasePoints;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_precision_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_precision_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Hunter's Mark - 185365
class spell_hun_hunters_mark : public SpellScriptLoader
{
    public:
        spell_hun_hunters_mark() : SpellScriptLoader("spell_hun_hunters_mark") { }

        class spell_hun_hunters_mark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_hunters_mark_AuraScript);

            enum eSpells
            {
                HunterMarkCaster = 185743
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HuntersMark)++;
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HuntersMark)--;
                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HuntersMark) == 0)
                    l_Caster->RemoveAura(eSpells::HunterMarkCaster);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_hunters_mark_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_hunters_mark_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_hunters_mark_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Lock and Load - 194595
class spell_hun_lock_and_load_proc : public SpellScriptLoader
{
    public:
        spell_hun_lock_and_load_proc() : SpellScriptLoader("spell_hun_lock_and_load_proc") { }

        class spell_hun_lock_and_load_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_lock_and_load_proc_AuraScript);

            enum eSpells
            {
                AutoShot = 75
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::AutoShot)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_lock_and_load_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_lock_and_load_proc_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Aimed Shot - 19434
/// Called For Lock and Load - 194595
class spell_hun_lock_and_load : public SpellScriptLoader
{
    public:
        spell_hun_lock_and_load() : SpellScriptLoader("spell_hun_lock_and_load") { }

        class spell_hun_lock_and_load_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_lock_and_load_SpellScript);

            enum eSpells
            {
                LockAndLoad = 194594
            };

            bool m_LockAndLoad = false;

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::LockAndLoad))
                    m_LockAndLoad = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !m_LockAndLoad)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::LockAndLoad);
                if (!l_Aura)
                    return;

                if (GetSpell() && GetSpell()->GetCastTime() <= 0)
                    l_Aura->DropCharge();
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_hun_lock_and_load_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_hun_lock_and_load_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_lock_and_load_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Spider Sting - 202914
class spell_hun_spider_sting: public SpellScriptLoader
{
    public:
        spell_hun_spider_sting() : SpellScriptLoader("spell_hun_spider_sting") { }

        class spell_hun_spider_sting_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_spider_sting_AuraScript);

            enum eSpells
            {
                SpiderSting = 202933
            };

            void HandleOnProc(ProcEventInfo& /* p_EventInfo */)
            {
                Unit* l_Caster = GetCaster();
                WorldObject* l_Target = GetOwner();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpiderSting, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_hun_spider_sting_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_spider_sting_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Mending Bandage - 212640
class spell_hun_mending_bandage : public SpellScriptLoader
{
    public:
        spell_hun_mending_bandage() : SpellScriptLoader("spell_hun_mending_bandage") { }

        class spell_hun_mending_bandage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_mending_bandage_SpellScript);

            enum eSpells
            {
                SpiderSting = 202933
            };

            void HandleEffectApply(SpellEffIndex /* effIndex */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<AuraEffect*> l_BleedAuraEffects = l_Caster->GetAuraEffectsByMechanic(1LL << MECHANIC_BLEED);
                auto l_PoisonAura = l_Caster->GetAppliedAuras();

                for (AuraEffect* l_AuraEffect : l_BleedAuraEffects)
                    l_Caster->RemoveAura(l_AuraEffect->GetId());

                for (auto l_Effect : l_PoisonAura)
                {
                    Aura* l_Aura = l_Effect.second->GetBase();
                    if (!l_Aura)
                        return;

                    SpellInfo const* l_SpellInfo = l_Aura->GetSpellInfo();
                    if (!l_SpellInfo)
                        return;

                    if (l_SpellInfo->IsLethalPoison())
                        l_Caster->RemoveAura(l_SpellInfo->Id);
                }
            }

            void Register() override
            {
               OnEffectHit += SpellEffectFn(spell_hun_mending_bandage_SpellScript::HandleEffectApply, EFFECT_1, SPELL_EFFECT_DISPEL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_mending_bandage_SpellScript();
        }
};

class PlayerScript_disengage : public PlayerScript
{
    public:
        PlayerScript_disengage() :PlayerScript("PlayerScript_disengage") {}

        enum eSpells
        {
            Disengage = 781
        };

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID)
        {
            if (!p_Player || p_Player->GetActiveSpecializationID() != SPEC_HUNTER_SURVIVAL || p_SpellID != eSpells::Disengage)
                return;

            p_Player->addSpell(eSpells::Disengage, true, true, false, false, false, false, false, false, 0, true);
        }
};

/// Last Update 7.3.2 Build 25549
/// Ranger's Finesse - 248443
class spell_hun_rangers_finesse : public SpellScriptLoader
{
    public:
        spell_hun_rangers_finesse() : SpellScriptLoader("spell_hun_rangers_finesse") { }

        class spell_hun_rangers_finesse_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_rangers_finesse_AuraScript);

            enum eSpells
            {
                AimedShot           = 19434,
                AspectoftheCheetah  = 186257,
                AspectoftheTurtle   = 186265,
                Exhilaration        = 109304,
                Vulnerable          = 187131
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::AimedShot)
                    return false;
                return true;
            }

            void HandleOnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                if (!l_Caster || !l_Target || !l_Target->HasAura(eSpells::Vulnerable))
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    uint32 reduceTime = p_AurEff->GetAmount() * IN_MILLISECONDS;
                    l_Player->ReduceSpellCooldown(eSpells::AspectoftheCheetah, reduceTime);
                    l_Player->ReduceSpellCooldown(eSpells::AspectoftheTurtle, reduceTime);
                    l_Player->ReduceSpellCooldown(eSpells::Exhilaration, reduceTime);
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_rangers_finesse_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_hun_rangers_finesse_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_rangers_finesse_AuraScript();
        }
};

class PlayerScript_t19_marksmanship_2p_bonus : public PlayerScript
{
    public:
        PlayerScript_t19_marksmanship_2p_bonus() : PlayerScript("PlayerScript_t19_marksmanship_2p_bonus") {}

        enum eSpells
        {
            T19Marksmanship2P = 211331,
            Trueshot = 193526
        };

        int32 m_PowerSpend = 0;

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After)
                return;

            if (p_Player->getClass() != CLASS_HUNTER || p_Power != POWER_FOCUS || p_Regen)
                return;

            AuraEffect* l_AuraEffect = p_Player->GetAuraEffect(eSpells::T19Marksmanship2P, EFFECT_0);
            if (!l_AuraEffect)
                return;

            // Get the power earn (if > 0 ) or consum (if < 0)
            int32 l_diffValue = p_NewValue - p_OldValue;

            // Only get spended power
            if (l_diffValue > 0)
                return;

            m_PowerSpend += -l_diffValue;
            if (m_PowerSpend >= l_AuraEffect->GetAmount())
            {
                if (p_Player->HasSpellCooldown(eSpells::Trueshot))
                    p_Player->ReduceSpellCooldown(eSpells::Trueshot, 1 * IN_MILLISECONDS);

                m_PowerSpend = 0;
            }
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Trueshot - 193526
class spell_hun_trueshot : public SpellScriptLoader
{
    public:
        spell_hun_trueshot() : SpellScriptLoader("spell_hun_trueshot") { }

        class spell_hun_trueshot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_trueshot_AuraScript);

            enum eSpells
            {
                T19Marksmanship4P   = 211322,
                Trueshot            = 211327,
                RapidKillingAura    = 191339,
                RapidKilling        = 191342
            };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::T19Marksmanship4P))
                    l_Caster->CastSpell(l_Caster, eSpells::Trueshot, true);

                if (l_Caster->HasAura(eSpells::RapidKillingAura))
                    l_Caster->CastSpell(l_Caster, eSpells::RapidKilling, true);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::Trueshot);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_trueshot_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_trueshot_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_trueshot_AuraScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Legendary: Soul of the Huntmaster - 247529
class spell_hun_soul_of_the_huntmaster : public SpellScriptLoader
{
    public:
        spell_hun_soul_of_the_huntmaster() : SpellScriptLoader("spell_hun_soul_of_the_huntmaster") { }

        class spell_hun_soul_of_the_huntmaster_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_soul_of_the_huntmaster_AuraScript);

            enum eSpells
            {
                BestialFury   = 194306,
                LockAndLoad   = 194595,
                SerpentSting  = 87935
            };

            void HandleApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                switch (l_Player->GetActiveSpecializationID())
                {
                case SPEC_HUNTER_SURVIVAL:
                    l_Player->CastSpell(l_Player, eSpells::SerpentSting, true);
                    break;
                case SPEC_HUNTER_BEASTMASTERY:
                    l_Player->CastSpell(l_Player, eSpells::BestialFury, true);
                    break;
                default:
                    l_Player->CastSpell(l_Player, eSpells::LockAndLoad, true);
                    break;
                }
            }

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                switch (l_Player->GetActiveSpecializationID())
                {
                case SPEC_HUNTER_SURVIVAL:
                    l_Player->RemoveAura(eSpells::SerpentSting);
                    break;
                case SPEC_HUNTER_BEASTMASTERY:
                    l_Player->RemoveAura(eSpells::BestialFury);
                    break;
                default:
                    l_Player->RemoveAura(eSpells::LockAndLoad);
                    break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_soul_of_the_huntmaster_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_soul_of_the_huntmaster_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_soul_of_the_huntmaster_AuraScript();
        }
};

/// Hunter's Bounty - 203749
class PlayerScript_hunters_bounty : public PlayerScript
{
public:
    PlayerScript_hunters_bounty() : PlayerScript("PlayerScript_hunters_bounty") {}

    enum eSpells
    {
        Exhilaration    = 109304,
        HuntersBounty   = 203749
    };

    enum eTraits
    {
        HuntersBountyTrait = 1078
    };

    void OnKill(Player* p_Killer, Unit* p_Killed, bool p_BeforeAuraRemoved)
    {
        if (!p_Killer || !p_Killed || p_Killer->GetActiveSpecializationID() != SPEC_HUNTER_SURVIVAL || !p_Killer->HasAura(HuntersBounty))
            return;

        if (p_BeforeAuraRemoved)
            p_Killer->ReduceSpellCooldown(Exhilaration, 5 * IN_MILLISECONDS * p_Killer->GetRankOfArtifactPowerId(eTraits::HuntersBountyTrait));
    }
};

class PlayerScript_Last_Breath_Of_The_Forest : public PlayerScript
{
    public:
        PlayerScript_Last_Breath_Of_The_Forest() :PlayerScript("PlayerScript_Last_Breath_Of_The_Forest") {}

        enum eSpells
        {
            CallToTheEagles     = 219376,
            CooldownDebuff      = 219380
        };

        enum eArtifactId
        {
            Talonclaw           = 34
        };

        void OnModifyHealth(Player* p_Player, int64 p_Value, int64 p_oldValue) override
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_HUNTER_SURVIVAL)
                return;

            if (p_Player->HasAura(eSpells::CooldownDebuff))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CallToTheEagles);

            if (!l_SpellInfo)
                return;

            if (MS::Artifact::Manager* l_Artifact = p_Player->GetCurrentlyEquippedArtifact())
            {
                if (l_Artifact->GetArtifactId() == eArtifactId::Talonclaw)
                {
                    uint32 l_HealthPct = p_Value * 100 / p_Player->GetMaxHealth();
                    if (l_HealthPct < 40)
                    {
                        Creature* l_Eagle = p_Player->FindNearestCreature(l_SpellInfo->Effects[EFFECT_0].MiscValue, 100.0f);
                        if (l_Eagle && l_Eagle->GetOwner() == p_Player)
                            return;

                        p_Player->CastSpell(p_Player, eSpells::CallToTheEagles, true);
                        p_Player->CastSpell(p_Player, eSpells::CooldownDebuff, true);
                    }
                }
            }
        }
};

/// Echoes of Ohn'ara - 238125
class spell_hun_echoes_of_ohnara : public SpellScriptLoader
{
public:
    spell_hun_echoes_of_ohnara() : SpellScriptLoader("spell_hun_echoes_of_ohnara") { }

    class spell_hun_echoes_of_ohnara_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_echoes_of_ohnara_AuraScript);

        enum eSpells
        {
            FlankingStrike = 202800
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo)
                return false;

            if (l_SpellInfo->Id != eSpells::FlankingStrike)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_hun_echoes_of_ohnara_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_echoes_of_ohnara_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
class PlayerScript_hun_artifact_appearance : public PlayerScript
{
public:
    PlayerScript_hun_artifact_appearance() : PlayerScript("PlayerScript_hun_artifact_appearance") {}

    enum eSpells
    {
        Windrunning = 219222,
        WindrunningAura = 219223
    };

    enum eArtifactId
    {
        Thasdorah = 55
    };

    void OnUpdate(Player* p_Player, uint32 p_Diff)
    {
        if (p_Player->getClass() != CLASS_HUNTER)
            return;

        if (m_UpdateTimer > p_Diff)
        {
            m_UpdateTimer -= p_Diff;
            return;
        }

        m_UpdateTimer = 5 * IN_MILLISECONDS;

        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Windrunning);
        if (!l_SpellInfo)
            return;

        if (p_Player->GetActiveSpecializationID() != SPEC_HUNTER_MARKSMANSHIP || (!p_Player->HasAura(l_SpellInfo->Id) && p_Player->isInCombat()))
        {
            if (p_Player->HasAura(l_SpellInfo->Id))
                p_Player->RemoveAura(l_SpellInfo->Id);

            return;
        }

        MS::Artifact::Manager* l_Artifact = p_Player->GetCurrentlyEquippedArtifact();
        if (!l_Artifact || l_Artifact->GetArtifactId() != eArtifactId::Thasdorah)
        {
            if (p_Player->HasAura(l_SpellInfo->Id))
                p_Player->RemoveAura(l_SpellInfo->Id);

            return;
        }

        SpellCastResult locRes = l_SpellInfo->CheckLocation(p_Player->GetMapId(), p_Player->GetZoneId(), p_Player->GetAreaId(), p_Player);
        if (locRes == SPELL_CAST_OK)
            p_Player->CastSpell(p_Player, l_SpellInfo->Id, true);
    }

private:
    uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
};

/// Last Update 7.3.2 Build 25549
/// Called by Windrunning - 219223
class spell_hun_art_windrunning : public SpellScriptLoader
{
public:
    spell_hun_art_windrunning() : SpellScriptLoader("spell_hun_art_windrunning") { }

    enum eSpells
    {
        Windrunning = 219222
    };

    class spell_hun_art_windrunning_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hun_art_windrunning_SpellScript);

        SpellCastResult CheckCast()
        {
            Unit* l_Caster = GetCaster();

            if (!l_Caster || l_Caster->isInCombat())
                return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_hun_art_windrunning_SpellScript::CheckCast);
        }
    };

    class spell_hun_art_windrunning_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_art_windrunning_AuraScript);

        void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();
            if (l_Caster && l_Target && l_Caster == l_Target)
                l_Caster->RemoveAura(eSpells::Windrunning);
            Remove();
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_hun_art_windrunning_AuraScript::HandleOnProc);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_hun_art_windrunning_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_art_windrunning_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called By Stomp - 201754
class spell_hun_stomp : public SpellScriptLoader
{
    public:
        spell_hun_stomp() : SpellScriptLoader("spell_hun_stomp") { }

        class spell_hun_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_stomp_SpellScript);

            enum eSpells
            {
                SpiderSting = 202933
            };

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetCaster()->GetOwner();
                Unit* l_Target = GetHitUnit();

                if (l_Owner == nullptr || l_Target == nullptr)
                    return;

                int32 l_Damage = (int32)(l_Owner->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack) * 3.0f * 1.4f);

                l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, SpellEffIndex::EFFECT_0, SPELL_DIRECT_DAMAGE);
                l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, SpellEffIndex::EFFECT_0);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
               OnHit += SpellHitFn(spell_hun_stomp_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_stomp_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Fiery Demise - 212817
class spell_hun_cobra_commander : public SpellScriptLoader
{
    public:
        spell_hun_cobra_commander() : SpellScriptLoader("spell_hun_cobra_commander") { }

        class spell_hun_cobra_commander_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_hun_cobra_commander_Aurascript);

            enum eSpells
            {
                CobraShot = 193455,
                CobraCommander = 243042
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::CobraShot)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfo)
            {
                Unit* l_Caster = p_ProcInfo.GetActor();
                DamageInfo const* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                int32 l_Bp = p_AurEff->GetAmount();
                l_Caster->CastCustomSpell(l_Target, eSpells::CobraCommander, &l_Bp, NULL, NULL, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_cobra_commander_Aurascript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_hun_cobra_commander_Aurascript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_cobra_commander_Aurascript();
        }
};

/// Mortal Wounds - 201091
class spell_hun_mortal_wounds : public SpellScriptLoader
{
public:
    spell_hun_mortal_wounds() : SpellScriptLoader("spell_hun_mortal_wounds") { }

    class spell_hun_mortal_wounds_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hun_mortal_wounds_SpellScript);

        enum eSpells
        {
            MongooseBite = 190928
        };

        void HandleEffectHit(SpellEffIndex p_EffIndex)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MongooseBite);
            if (!l_SpellInfo)
                return;

            l_Player->ModSpellCharge(eSpells::MongooseBite, l_Player->GetMaxCharges(l_SpellInfo->ChargeCategoryEntry) + 1 - l_Player->GetConsumedCharges(l_SpellInfo->ChargeCategoryEntry));
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_hun_mortal_wounds_SpellScript::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_hun_mortal_wounds_SpellScript();
    }
};

/// Last Update: 7.3.5 Build 26365
/// Vulnerable - 187131
class spell_hun_vulnerable : public SpellScriptLoader
{
    public:
        spell_hun_vulnerable() : SpellScriptLoader("spell_hun_vulnerable") { }

        class spell_hun_vulnerable_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_vulnerable_AuraScript);

            enum eSpells
            {
                MarkedForDeath = 190529
            };

            void CalculateAmount(AuraEffect const* l_AuraEffect, int32& l_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (AuraEffect const* l_MarkedEffect = l_Caster->GetAuraEffect(eSpells::MarkedForDeath, SpellEffIndex::EFFECT_0))
                    l_Amount = l_MarkedEffect->GetAmount();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_vulnerable_AuraScript::CalculateAmount, SpellEffIndex::EFFECT_2, AuraType::SPELL_AURA_MOD_CRIT_CHANCE_FOR_CASTER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_vulnerable_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Mortal Wounds - 201091
class spell_hun_dash : public SpellScriptLoader
{
    public:
        spell_hun_dash() : SpellScriptLoader("spell_hun_dash") { }

        class spell_hun_dash_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_dash_AuraScript);

            void HandleEffectApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Pet* l_Creature = l_Caster->ToPet();
                if (!l_Creature)
                    return;

                Player* l_Player = l_Creature->GetOwner();
                if (!l_Player)
                    return;

                Creature* l_Hati = l_Player->GetHati();
                if (!l_Hati)
                    return;

                l_Hati->CastSpell(l_Hati, GetId(), true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_dash_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_dash_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Flare - 1543
class spell_hun_flare : public SpellScriptLoader
{
public:
    spell_hun_flare() : SpellScriptLoader("spell_hun_flare") { }

    class spell_hun_flare_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hun_flare_SpellScript);

        enum eSpells
        {
            GlyphStellarFlare = 212462,
            GlyphGoblinAntiGravFlare = 212451,
        };

        enum eVisuals
        {
            VisualStellarFlare = 58473,
            VisualGoblinAntiGravFlare = 58474
        };

        void HandleOnPrepare()
        {
            Unit* l_Caster = GetCaster();
            WorldLocation const* l_Dest = GetExplTargetDest();
            if (!l_Caster || !l_Dest)
                return;

            if (l_Caster->HasAura(eSpells::GlyphStellarFlare))
                l_Caster->SendPlaySpellVisual(eVisuals::VisualStellarFlare, nullptr, 20.79684f, l_Dest->GetPosition(), true, false);

            if (l_Caster->HasAura(eSpells::GlyphGoblinAntiGravFlare))
                l_Caster->SendPlaySpellVisual(eVisuals::VisualGoblinAntiGravFlare, nullptr, 20.8943f, l_Dest->GetPosition(), true, false);
        }

        void Register() override
        {
            OnPrepare += SpellOnPrepareFn(spell_hun_flare_SpellScript::HandleOnPrepare);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_hun_flare_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26124
/// Called by Wyvern Sting - 19386
class spell_hun_wyvern_sting : public SpellScriptLoader
{
    public:
        spell_hun_wyvern_sting() : SpellScriptLoader("spell_hun_wyvern_sting") { }

        class spell_hun_wyvern_sting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_wyvern_sting_SpellScript);

            enum eSpells
            {
                AMurderOfCrows      = 131894
            };

            std::vector<AuraType> m_DiseaseAuraTypes =
            {
                SPELL_AURA_PERIODIC_DAMAGE,
                SPELL_AURA_PERIODIC_SCHOOL_DAMAGE,
                SPELL_AURA_PERIODIC_DAMAGE_PERCENT,
                SPELL_AURA_PERIODIC_LEECH,
                SPELL_AURA_PERIODIC_HEALTH_FUNNEL
            };

            void HandleOnCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                for (auto l_AuraType : m_DiseaseAuraTypes)
                    if (l_Target->HasAuraType(l_AuraType))
                        l_Target->RemoveAurasByType(l_AuraType);

                l_Target->RemoveAurasDueToSpell(eSpells::AMurderOfCrows);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_wyvern_sting_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_wyvern_sting_SpellScript();
        }
};

/// Last Update 7.3.5
/// Called By Feet Of Wind - 240777
class spell_hun_feet_of_wind : public SpellScriptLoader
{
    public:
        spell_hun_feet_of_wind() : SpellScriptLoader("spell_hun_feet_of_wind") { }

        class spell_hun_feet_of_wind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_feet_of_wind_AuraScript);

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SNARE, true);
            }

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SNARE, false);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_feet_of_wind_AuraScript::HandleAfterEffectApply, EFFECT_2, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_hun_feet_of_wind_AuraScript::HandleRemove, EFFECT_2, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_feet_of_wind_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called for Item - Hunter T20 Beast Mastery 2P Bonus - 242239
/// Called by Cobra Shot 193455, Multi-Shot 2643, and Kill Command 34026
class spell_hun_bm_t20_2p_bonus_proc : public SpellScriptLoader
{
    public:
        spell_hun_bm_t20_2p_bonus_proc() : SpellScriptLoader("spell_hun_bm_t20_2p_bonus_proc") { }

        class spell_hun_bm_t20_2p_bonus_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_bm_t20_2p_bonus_proc_SpellScript);

            enum eSpells
            {
                BestialWrath    = 19574,
                BestialWrathPet = 186254,
                BMT20_2P        = 242239,
                BonusEffect     = 246126
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::BMT20_2P) || !l_Caster->HasAura(eSpells::BestialWrath))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::BestialWrath, EFFECT_0))
                {
                    uint32 l_Boost = urand(1, 2); ///< Piggy fix cuz it's supposed to increase by basepoint / 10 which is 1.5 but we DONT HANDLE FLOAT BASEPOINTS atm.
                    l_Caster->CastSpell(l_Caster, eSpells::BonusEffect, true);
                    l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() + l_Boost);

                    Pet* l_Pet = l_Player->GetPet();

                    if (l_Pet && l_Pet->HasAura(eSpells::BestialWrathPet))
                    {
                        if (AuraEffect* l_BeastialPet = l_Pet->GetAuraEffect(eSpells::BestialWrathPet, EFFECT_0))
                            l_BeastialPet->ChangeAmount(l_BeastialPet->GetAmount() + l_Boost);
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_hun_bm_t20_2p_bonus_proc_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_bm_t20_2p_bonus_proc_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by 242241 - Hunter T20 Marksman 4P Bonus
class spell_hunt_tos_set : public SpellScriptLoader
{
    public:
        spell_hunt_tos_set() : SpellScriptLoader("spell_hunt_tos_set") { }

        class spell_hunt_tos_set_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hunt_tos_set_AuraScript);

            bool HandleCheckProc(ProcEventInfo& /* p_EventInfo */)
            {
                return false;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_hunt_tos_set_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hunt_tos_set_AuraScript();
        }
};


/// Last Update 7.3.2 Build 25549
/// Called by Stone Armor- 160049
class spell_hun_stone_armor : public SpellScriptLoader
{
    public:
        spell_hun_stone_armor() : SpellScriptLoader("spell_hun_stone_armor") { }

        class spell_hun_stone_armor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_stone_armor_AuraScript);

            bool HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || l_Caster->GetHealthPct() >= l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_stone_armor_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_stone_armor_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Tar Trap - 187700, 187699 and Freeaing Trap - 187651
class spell_hun_limit_for_traps : public SpellScriptLoader
{
    public:
        spell_hun_limit_for_traps() : SpellScriptLoader("spell_hun_limit_for_traps") { }

        class spell_hun_limit_for_traps_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_limit_for_traps_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AreaTrigger* l_Trap = l_Caster->GetAreaTrigger(GetSpellInfo()->Id))
                    l_Trap->SetDuration(0);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_hun_limit_for_traps_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_limit_for_traps_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Hunter's Mark - 185987
class spell_hun_hunters_mark_marksmanship : public SpellScriptLoader
{
    public:
        spell_hun_hunters_mark_marksmanship() : SpellScriptLoader("spell_hun_hunters_mark_marksmanship")
        {}

        class spell_hun_hunters_mark_marksmanship_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_hunters_mark_marksmanship_AuraScript);

            void HandleCalcProcsPerMinute(float & p_ProcsPerMinute, ProcEventInfo & p_Proc)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (!l_Owner->IsPlayer())
                    return;

                float l_TotalHaste = l_Owner->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_HASTE_RANGED) * l_Owner->ToPlayer()->GetRatingMultiplier(CR_HASTE_RANGED) / 100.0f;
                p_ProcsPerMinute *= (1 + l_TotalHaste);
            }

            void Register() override
            {
                DoCalcPPM += AuraProcPerMinuteFn(spell_hun_hunters_mark_marksmanship_AuraScript::HandleCalcProcsPerMinute);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_hunters_mark_marksmanship_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Hunter T21 Survival 2P Bonus - 251751
class spell_hun_t21_survival_2p_bonus : public SpellScriptLoader
{
    public:
        spell_hun_t21_survival_2p_bonus() : SpellScriptLoader("spell_hun_t21_survival_2p_bonus") { }

        class spell_hun_t21_survival_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_t21_survival_2p_bonus_AuraScript);

            enum eSpells
            {
                FlankingStrike = 202800
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::FlankingStrike)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_t21_survival_2p_bonus_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_t21_survival_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Hunter T21 Survival 4P Bonus - 251752
class spell_hun_t21_survival_4p_bonus : public SpellScriptLoader
{
    public:
        spell_hun_t21_survival_4p_bonus() : SpellScriptLoader("spell_hun_t21_survival_4p_bonus") { }

        class spell_hun_t21_survival_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_t21_survival_4p_bonus_AuraScript);

            enum eSpells
            {
                MoongoseBite = 190928
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::MoongoseBite)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_t21_survival_4p_bonus_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_t21_survival_4p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Marking Targets - 223138
class spell_hun_marking_targets : public SpellScriptLoader
{
    public:
        spell_hun_marking_targets() : SpellScriptLoader("spell_hun_marking_targets")
        {}

        enum eSpells
        {
            Multishot = 2643,
            ArcaneShot  = 185358,
            Sidewinders = 214581,
            HuntersMark = 185365,
        };

        class spell_hun_marking_targets_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_marking_targets_AuraScript);

            bool HandleCheckProc(ProcEventInfo & p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr)
                    return false;

                Unit* l_Target = p_Proc.GetActionTarget();

                SpellInfo const* l_ProcSpell = p_Proc.GetDamageInfo()->GetSpellInfo();

                if (l_ProcSpell == nullptr || l_Target == nullptr)
                    return false;

                /// Only Multi Shot, Arcane Shot, and Sidewinders should remove Marking Targets, IF the target has been marked by the current cast.
                if (l_ProcSpell->Id == eSpells::Multishot ||
                    l_ProcSpell->Id == eSpells::ArcaneShot ||
                    l_ProcSpell->Id == eSpells::Sidewinders ||
                    !l_Target->HasAura(eSpells::HuntersMark))
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_hun_marking_targets_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_marking_targets_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Fluffy, Go - 203669
class spell_hun_fluffy_go : public SpellScriptLoader
{
public:
    spell_hun_fluffy_go() : SpellScriptLoader("spell_hun_fluffy_go") { }

    enum eSpells
    {
        FluffyGo = 203669,
        FluffyGoPetAura = 218955
    };

    enum eArtifactPowers
    {
        FluffyGoArtifact = 1074
    };

    static void TryApplyAuraToPet(Unit* p_Caster)
    {
        if (!p_Caster)
            return;

        Player* l_Player = p_Caster->ToPlayer();
        if (!l_Player)
            return;

        Aura* l_Aura = l_Player->GetAura(eSpells::FluffyGo);
        if (!l_Aura)
            return;

        Pet* l_Pet = l_Player->GetPet();
        if (!l_Pet)
            return;

        uint8 l_Level = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::FluffyGoArtifact);
        uint32 l_Amount = l_Aura->GetEffect(EFFECT_0)->GetBaseAmount();
        uint32 l_HastePct = l_Level * l_Amount;

        l_Pet->CastCustomSpell(eSpells::FluffyGoPetAura, SPELLVALUE_BASE_POINT0, l_HastePct, l_Pet, false);
    }

    class spell_hun_fluffy_go_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_fluffy_go_AuraScript);

        void OnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
        {
            spell_hun_fluffy_go::TryApplyAuraToPet(GetCaster());
        }

        void OnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Pet* l_Pet = l_Player->GetPet();
            if (!l_Pet)
                return;

            l_Pet->RemoveAura(eSpells::FluffyGoPetAura);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_hun_fluffy_go_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_hun_fluffy_go_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_hun_fluffy_go_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called for Fluffy, Go - 203669
class PlayerScript_hun_fluffy_go : public PlayerScript
{
public:
    PlayerScript_hun_fluffy_go() : PlayerScript("PlayerScript_hun_fluffy_go") {}

    void OnSummonPet(Player* p_Player)
    {
        spell_hun_fluffy_go::TryApplyAuraToPet(p_Player);
    }
};

#ifndef __clang_analyzer__
void AddSC_hunter_spell_scripts()
{
    new spell_hun_hunters_mark();
    new spell_hun_precision();
    new spell_hun_bursting_shot();
    new PlayerScript_aimed_shot();
    new spell_hunt_windburst();
    new spell_hun_separation_anxiety_apply();
    new spell_hun_separation_anxiety();
    new spell_hun_wild_protector();
    new spell_hun_legacy_of_the_windrunners();
    new spell_hun_exhilaration();
    new spell_hun_healing_shell();
    new spell_hun_arcane_shot();
    new spell_hun_ullrs_featherweight_snowshoes();
    new spell_hun_nesingwarys_trapping_treads();
    new spell_hun_frizzos_fingertrap();
    new spell_hun_the_shadow_hunters_voodoo_mask();
    new spell_hun_trailblazer();
    new spell_hun_fury_of_the_eagle();
    new spell_hun_freezing_trap();
    new spell_hun_explosive_trap();
    new spell_hun_tar_trap_areatrigger();
    new spell_hun_tar_trap();
    new spell_hun_steel_trap_dmg();
    new spell_hun_dragonscale_armor();
    new spell_hun_talon_strike();
    new spell_hun_talon_slash();
    new spell_hun_dragonsfire_conflagration();
    new spell_hun_dragonsfire_grenade();
    new spell_hun_farstrider();
    new spell_hun_aspect_of_the_cheetah();
    new spell_hun_harpon();
    new spell_hun_aspect_of_the_turtle();
    new spell_hun_snake_hunter();
    new spell_hun_dire_frenzy();
    new spell_hun_titan_thunder_dmg();
    new spell_hun_lone_wolf();
    new spell_hun_call_pet();
    new spell_hun_surge_of_the_stormgod();
    new spell_hun_mimirons_shell();
    new spell_hun_piercing_shot();
    new spell_hun_a_murder_of_crows_damage();
    new spell_hun_thick_hide();
    new spell_hun_lesser_proportion();
    new spell_hun_glyph_of_lesser_proportion();
    new spell_hun_fetch_glyph();
    new spell_hun_kill_command_proc();
    new spell_hun_spirit_mend();
    new spell_hun_thunderstomp();
    new spell_hun_cornered();
    new spell_hun_exotic_munitions();
    new spell_hun_claw_bite();
    new spell_hun_fireworks();
    new spell_hun_glaive_toss_damage();
    new spell_hun_glaive_toss_missile();
    new spell_hun_a_murder_of_crows();
    new spell_hun_frenzy();
    new spell_hun_beast_cleave_proc();
    new spell_hun_beast_cleave();
    new spell_hun_barrage();
    new spell_hun_barrage_haste();
    new spell_hun_binding_shot();
    new spell_hun_binding_shot_zone();
    new spell_hun_serpent_spread();
    new spell_hun_ancient_hysteria();
    new spell_hun_netherwinds();
    new spell_hun_kill_command();
    new spell_hun_chimaera_shot();
    new spell_hun_masters_call();
    new spell_hun_pet_heart_of_the_phoenix();
    new spell_hun_misdirection();
    new spell_hun_misdirection_proc();
    new spell_hun_disengage();
    new spell_hun_tame_beast();
    new spell_hun_burrow_attack();
    new spell_hun_aimed_shot();
    new spell_hun_poisoned_ammo();
    new spell_hun_adaptation();
    new spell_hun_t17_marksmanship_4p();
    new spell_hun_fury_of_the_eagle_trigger_aura();
    new PlayerScript_Pet_Active();
    new spell_hun_emmarels_assault();
    new spell_hun_aspect_of_the_beast();
    new spell_hun_soul_of_the_huntmaster();
    new spell_hun_dash();
    new spell_hun_flare();
    new spell_hun_stone_armor();
    new spell_hun_limit_for_traps();

    /// Beast Master
    new spell_hun_wild_call();
    new spell_hun_wild_call_proc();
    new spell_hun_dire_beast();
    new spell_hun_cobra_shot();
    new spell_hun_bestial_wrath();
    new spell_hun_bestial_wrath_aura();
    new spell_hun_stampede();
    new PlayerScript_Apex_Predator_Claw();
    new spell_hun_apex_predator_claw();
    new spell_hun_intimidation();
    new spell_hun_stomp();
    new spell_hun_bm_t20_2p_bonus_proc();

    /// Marksmanship
    new spell_hun_feet_of_wind();
    new spell_hun_marked_shot();
    new spell_hun_call_of_the_hunter();
    new spell_hunter_marking_targets();
    new spell_hun_trick_shot();
    new spell_hun_aimed_shot_copy();
    new spell_hun_careful_aim();
    new PlayerScript_hun_black_arrow();
    new spell_hun_sentinels_sight();
    new spell_hun_bullseye();
    new spell_hun_patient_sniper();
    new spell_hun_volley();
    new spell_areatrigger_hun_sentinel();
    new spell_hun_hunting_party();
    new spell_hun_rapid_killing_pvp();
    new spell_hun_steady_focus();
    new PlayerScript_Sidewinders();
    new spell_hunt_multishot();
    new spell_hunt_critical_focus();
    new spell_hunt_sidewinders();
    new spell_hun_lock_and_load_proc();
    new spell_hun_lock_and_load();
    new PlayerScript_t19_marksmanship_2p_bonus();
    new spell_hun_trueshot();
    new spell_hun_wyvern_sting();
    new spell_hunt_tos_set();
    new spell_hun_hunters_mark_marksmanship();
    new spell_hun_marking_targets();

    /// Survival
    new spell_hun_flanking_strike();
    new spell_hun_mongoose_bite();
    new spell_hun_expert_trapper_steel();
    new spell_hun_mongoose_fury();
    new spell_hun_throwing_axes();
    new spell_hun_eagles_bite();
    new spell_hun_on_the_trail();
    new spell_hun_aspect_of_the_skylord();
    new spell_hun_serpent_sting();
    new spell_hun_butchers_bone_apron();
    new spell_hun_explosive_shot_detonate();
    new spell_hun_explosive_shot_damage();
    new PlayerScript_disengage();
    new PlayerScript_hun_terms_of_engagement();
    new spell_hun_sticky_bomb();
    new PlayerScript_hunters_bounty();
    new PlayerScript_Last_Breath_Of_The_Forest();
    new spell_hun_echoes_of_ohnara();
    new spell_hun_t21_survival_2p_bonus();
    new spell_hun_t21_survival_4p_bonus();

    /// Honor Talents
    new spell_hun_viper_sting_debuff();
    new spell_hun_survival_tactics();
    new spell_hun_sticky_tar();
    new spell_hun_focused_fire();
    new spell_hunter_the_beast_within();
    new spell_hun_go_to_the_throat();
    new spell_hun_spider_sting();
    new spell_hun_mending_bandage();
    new spell_hun_rangers_finesse();

    /// Artifacts
    new spell_hun_titanstrike();
    new spell_hun_mend_pet();
    new spell_hun_jaws_of_thunder();
    new spell_hun_titans_thunder();
    new spell_hun_titans_thunder_periodic();
    new spell_hun_deadly_aim();
    new spell_hun_survival_of_the_fittest();
    new PlayerScript_hun_artifact_appearance();
    new spell_hun_art_windrunning();
    new spell_hun_cobra_commander();
    new spell_hun_mortal_wounds();
    new spell_hun_vulnerable();
    new spell_hun_fluffy_go();
    new PlayerScript_hun_fluffy_go();

    new PlayerScript_gyroscopic_stabilizer();
}
#endif
