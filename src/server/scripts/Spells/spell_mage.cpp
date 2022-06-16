////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "HelperDefines.h"
#include "CellImpl.h"

/// Item - Mage WoD PvP Frost 2P Bonus - 180723
class spell_areatrigger_mage_wod_frost_2p_bonus : public AreaTriggerEntityScript
{
public:
    spell_areatrigger_mage_wod_frost_2p_bonus()
        : AreaTriggerEntityScript("at_mage_wod_frost_2p_bonus")
    {
    }

    enum eSpells
    {
        SlickIce = 180724
    };

    AreaTriggerEntityScript* GetAI() const override
    {
        return new spell_areatrigger_mage_wod_frost_2p_bonus();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
    {
        std::list<Unit*> l_TargetList;
        float l_Radius = 20.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> l_Searcher(l_Caster, l_TargetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

        for (auto l_Itr : l_TargetList)
        {
            if (l_Itr->GetDistance(p_AreaTrigger) <= 6.0f)
                l_Caster->CastSpell(l_Itr, eSpells::SlickIce, true);
            else
                l_Itr->RemoveAura(eSpells::SlickIce, l_Caster->GetGUID());
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
    {
        std::list<Unit*> l_TargetList;
        float l_Radius = 10.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> l_Searcher(l_Caster, l_TargetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

        for (auto l_Itr : l_TargetList)
        {
            if (l_Itr->HasAura(eSpells::SlickIce, l_Caster->GetGUID()))
                l_Itr->RemoveAura(eSpells::SlickIce, l_Caster->GetGUID());
        }
    }
};

/// Meteor - 153561
class spell_mage_meteor : public SpellScriptLoader
{
    public:
        spell_mage_meteor() : SpellScriptLoader("spell_mage_meteor") { }

        class spell_mage_meteor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_meteor_SpellScript);

            enum eMeteorDatas
            {
                MeteorTimeStamp = 177345
            };

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (WorldLocation const* l_Dest = GetExplTargetDest())
                        l_Caster->CastSpell(l_Dest->GetPositionX(), l_Dest->GetPositionY(), l_Dest->GetPositionZ(), eMeteorDatas::MeteorTimeStamp, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_meteor_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_meteor_SpellScript();
        }
};

/// Prismatic Crystal - 155152
class spell_mage_prysmatic_crystal_damage : public SpellScriptLoader
{
    public:
        spell_mage_prysmatic_crystal_damage() : SpellScriptLoader("spell_mage_prysmatic_crystal_damage") { }

        class spell_mage_prysmatic_crystal_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_prysmatic_crystal_damage_SpellScript);

            enum eCreature
            {
                PrismaticCrystalNpc = 76933
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (p_Object->ToUnit()->GetEntry() == eCreature::PrismaticCrystalNpc)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_prysmatic_crystal_damage_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_prysmatic_crystal_damage_SpellScript();
        }
};

/// Greater Invisibility (remove timer) - 122293
class spell_mage_greater_invisibility_removed: public SpellScriptLoader
{
    public:
        spell_mage_greater_invisibility_removed() : SpellScriptLoader("spell_mage_greater_invisibility_removed") { }

        class spell_mage_greater_invisibility_removed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_greater_invisibility_removed_AuraScript);

            enum eSpells
            {
                GreaterInvisibilityLessDamage = 113862
            };

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpells::GreaterInvisibilityLessDamage);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_greater_invisibility_removed_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_greater_invisibility_removed_AuraScript();
        }
};

/// Cord of infinity - 209311
class spell_mage_cord_of_infinity : public SpellScriptLoader
{
    public:
        spell_mage_cord_of_infinity() : SpellScriptLoader("spell_mage_cord_of_infinity") { }

        class spell_mage_cord_of_infinity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_cord_of_infinity_AuraScript);

            enum eSpells
            {
                CordOfInfinityLegendaryAura = 209311,
                CordOfInfinityAura = 209316
            };

            void OnUpdate(uint32 /*p_Diff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::CordOfInfinityLegendaryAura) && !l_Caster->HasAura(eSpells::CordOfInfinityAura)
                        && ((100 * l_Caster->GetPower(POWER_MANA) / l_Caster->GetMaxPower(POWER_MANA)) < 40.0f))
                        l_Caster->CastSpell(l_Caster, eSpells::CordOfInfinityAura, true);
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_mage_cord_of_infinity_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_cord_of_infinity_AuraScript();
        }
};

/// last update : 7.3.5 - Build 26365
/// Greater Invisibility (triggered) - 110960
class spell_mage_greater_invisibility_triggered: public SpellScriptLoader
{
    public:
        spell_mage_greater_invisibility_triggered() : SpellScriptLoader("spell_mage_greater_invisibility_triggered") { }

        class spell_mage_greater_invisibility_triggered_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_greater_invisibility_triggered_AuraScript);

            enum eSpells
            {
                GreaterInvisibilityLessDamage   = 113862,
                GreaterInvisibilityRemovedTimer = 122293,
                Dampening                       = 110310                /// Greater Invisibility was removing Dampening effect when it shouldnt
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->CastSpell(l_Target, eSpells::GreaterInvisibilityLessDamage, true);
                    l_Target->CombatStop();
                }
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpells::GreaterInvisibilityRemovedTimer, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_greater_invisibility_triggered_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_greater_invisibility_triggered_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_greater_invisibility_triggered_AuraScript();
        }

        class spell_mage_greater_invisibility_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_greater_invisibility_triggered_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_DAMAGE, SpellSchoolMask::SPELL_SCHOOL_MASK_ALL, true);

                for (uint32 l_Itr = 0; l_Itr < Mechanics::MAX_MECHANIC; l_Itr++)
                    l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, l_Itr, true);

            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_DAMAGE, SpellSchoolMask::SPELL_SCHOOL_MASK_ALL, false);

                    for (uint32 l_Itr = 0; l_Itr < Mechanics::MAX_MECHANIC; l_Itr++)
                        l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, l_Itr, false);

                }, 400);
            }

            void Register()
            {
                OnPrepare += SpellOnPrepareFn(spell_mage_greater_invisibility_triggered_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_mage_greater_invisibility_triggered_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_greater_invisibility_triggered_SpellScript();
        }
};

/// Last Update 6.2.3
/// Frost Nova (Water Elemental) - 33395
class spell_mage_pet_frost_nova: public SpellScriptLoader
{
    public:
        spell_mage_pet_frost_nova() : SpellScriptLoader("spell_mage_pet_frost_nova") { }

        class spell_mage_pet_frost_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_pet_frost_nova_SpellScript);

            enum eSpells
            {
                FingerFrost         = 44544,
                FingerFrostVisual   = 126084
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

               if (l_Player->HasAura(eSpells::FingerFrost))
                   l_Player->CastSpell(l_Player, eSpells::FingerFrostVisual, true); ///< Fingers of frost visual 2 procs

               l_Player->CastSpell(l_Player, eSpells::FingerFrost, true);  ///< Fingers of frost proc
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_pet_frost_nova_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_pet_frost_nova_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ice Block - 45438
class spell_mage_ice_block : public SpellScriptLoader
{
    public:
        spell_mage_ice_block() : SpellScriptLoader("spell_mage_ice_block") { }

        class spell_mage_ice_block_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_ice_block_SpellScript);

            enum eSpells
            {
                Cyclone     = 33786,
                Imprison    = 221527,
                Unworthy    = 198190,
                DiamondIce  = 203337
            };

            SpellCastResult CheckCyclone()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// Odyn's test in Hall of Valor - Removed from combat while judged "Unworthy"
                    if (l_Caster->HasAura(eSpells::Unworthy))
                        return SpellCastResult::SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                    l_Caster->RemoveAura(eSpells::Cyclone);
                    l_Caster->RemoveAura(eSpells::Imprison);
                    l_Caster->RemoveAura(eSpells::DiamondIce);
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_mage_ice_block_SpellScript::CheckCyclone);
            }
        };

        class spell_mage_ice_block_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_ice_block_AuraScript);

            enum eSpells
            {
                GlacialInsulation   = 235297,
                IceBarrier          = 11426
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                // Don't use 'castspell' because it will not be casted if IceBarrier is on cd
                if (l_Caster->HasAura(eSpells::GlacialInsulation))
                    l_Caster->AddAura(eSpells::IceBarrier, l_Caster);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_ice_block_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_ice_block_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_ice_block_AuraScript();
        }
};

/// Last Update 6.2.3
/// Nether Tempest - 114954
class spell_mage_nether_tempest_damage : public SpellScriptLoader
{
    public:
        spell_mage_nether_tempest_damage() : SpellScriptLoader("spell_mage_nether_tempest_damage") { }

        class spell_mage_nether_tempest_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_nether_tempest_damage_SpellScript);

            enum eSpells
            {
                NetherTempestAura           = 114923,
                NetherTempestLuncherVisual  = 114956,
                ArcaneCharge                = 36032
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_FirstTarget = nullptr;
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                for (WorldObject* l_Unit : p_Targets)
                {
                    if (l_Unit->ToUnit() && l_Unit->ToUnit()->HasAura(eSpells::NetherTempestAura, l_Caster->GetGUID()))
                        l_FirstTarget = l_Unit->ToUnit();
                }

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (p_Object->ToUnit()->HasAura(eSpells::NetherTempestAura, l_Caster->GetGUID()))
                        return true;

                    return false;
                });

                if (l_FirstTarget == nullptr)
                    return;

                for (WorldObject* l_Unit : p_Targets)
                {
                    l_FirstTarget->CastSpell(l_Unit->ToUnit(), eSpells::NetherTempestLuncherVisual, true);
                }
            }

            void HandleDamage()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ArcaneCharge);

                    if (l_Caster->HasAura(eSpells::ArcaneCharge) && l_SpellInfo != nullptr)
                    {
                        if (Aura* l_ArcaneCharge = l_Caster->GetAura(eSpells::ArcaneCharge))
                        {
                            int32 l_Damage = GetHitDamage();
                            SetHitDamage(AddPct(l_Damage, int32(l_SpellInfo->Effects[EFFECT_0].BasePoints * l_ArcaneCharge->GetStackAmount())));
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_nether_tempest_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_mage_nether_tempest_damage_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_nether_tempest_damage_SpellScript();
        }
};

/// Nether Tempest - 114923
class spell_mage_nether_tempest : public SpellScriptLoader
{
    public:
        spell_mage_nether_tempest() : SpellScriptLoader("spell_mage_nether_tempest") { }

        class spell_mage_nether_tempest_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_nether_tempest_AuraScript);

            enum eSpells
            {
                NetherTempestDirectDamage   = 114954,
                ArcaneCharge                = 36032
            };

            void CalculateAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ArcaneCharge);

                    if (l_Caster->HasAura(eSpells::ArcaneCharge) && l_SpellInfo != nullptr)
                    {
                        if (Aura* l_ArcaneCharge = l_Caster->GetAura(eSpells::ArcaneCharge))
                            p_Amount += CalculatePct(p_Amount, l_SpellInfo->Effects[EFFECT_0].BasePoints) * l_ArcaneCharge->GetStackAmount();
                    }
                }
            }

            void OnTick(AuraEffect const*)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, eSpells::NetherTempestDirectDamage, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_nether_tempest_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_nether_tempest_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_nether_tempest_AuraScript();
        }
};

// Blazing Speed - 108843
class spell_mage_blazing_speed: public SpellScriptLoader
{
    public:
        spell_mage_blazing_speed() : SpellScriptLoader("spell_mage_blazing_speed") { }

        class spell_mage_blazing_speed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_blazing_speed_SpellScript);

            enum eSpells
            {
                GreaterInvisibility             = 110960,
                GreaterInvisibilityLessDamage   = 113862
            };

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    l_Player->RemoveMovementImpairingAuras();

                    if (l_Player->HasAura(eSpells::GreaterInvisibility))
                    {
                        l_Player->RemoveAura(eSpells::GreaterInvisibility);
                        l_Player->RemoveAura(eSpells::GreaterInvisibilityLessDamage);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_blazing_speed_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_blazing_speed_SpellScript();
        }
};

// Evocation - 12051
class spell_mage_evocation : public SpellScriptLoader
{
    public:
        spell_mage_evocation() : SpellScriptLoader("spell_mage_evocation") { }

        class spell_mage_evocation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_evocation_AuraScript);

            enum eSpells
            {
                T17Arcane2P             = 165475,
                ArcaneAffinity          = 166871,
                AegwynnsAscendanceAura  = 187680,
                AegwynnsAscendanceDmg   = 187677
            };

            int32 m_StartMana = 0;

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;
                m_StartMana = l_Caster->GetPower(POWER_MANA);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::T17Arcane2P))
                        l_Caster->CastSpell(l_Caster, eSpells::ArcaneAffinity, true);

                    if (AuraEffect* l_AegwynnsEffect = l_Caster->GetAuraEffect(eSpells::AegwynnsAscendanceAura, EFFECT_0))
                    {
                        int32 l_Damage = CalculatePct(l_Caster->GetPower(POWER_MANA) - m_StartMana, l_AegwynnsEffect->GetAmount());
                        if (l_Damage <= 0)
                            return;

                        l_Caster->CastCustomSpell(l_Caster, eSpells::AegwynnsAscendanceDmg, &l_Damage, nullptr, nullptr, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_mage_evocation_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_OBS_MOD_POWER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_evocation_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_OBS_MOD_POWER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_evocation_AuraScript();
        }
};

/// Time Warp - 80353
class spell_mage_time_warp : public SpellScriptLoader
{
    public:
        spell_mage_time_warp() : SpellScriptLoader("spell_mage_time_warp") { }

        class spell_mage_time_warp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_time_warp_SpellScript);

            enum eSpells
            {
                TemporalDisplacement    = 80354,
                Insanity                = 95809,
                Exhausted               = 57723,
                Sated                   = 57724,
                Fatigued                = 160455,
                ShardOfTheExodar        = 207970
            };

            void ApplyDebuff()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (GetSpellInfo() && l_Target->HasAura(GetSpellInfo()->Id) && !(l_Target->HasAura(eSpells::ShardOfTheExodar) && l_Target == l_Caster))
                    l_Target->CastSpell(l_Target, eSpells::TemporalDisplacement, true);
            }


            void HandleImmunity(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || (l_Caster == l_Target && l_Caster->HasAura(eSpells::ShardOfTheExodar)))
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
                AfterHit += SpellHitFn(spell_mage_time_warp_SpellScript::ApplyDebuff);
                OnEffectHitTarget += SpellEffectFn(spell_mage_time_warp_SpellScript::HandleImmunity, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                OnEffectHitTarget += SpellEffectFn(spell_mage_time_warp_SpellScript::HandleImmunity, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_time_warp_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mirror Image - 55342
class spell_mage_mirror_image_summon: public SpellScriptLoader
{
    public:
        spell_mage_mirror_image_summon() : SpellScriptLoader("spell_mage_mirror_image_summon") { }

        class spell_mage_mirror_image_summon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_mirror_image_summon_SpellScript);

            enum eSpells
            {
                MirrorImageLeft     = 58834,
                MirrorImageFront    = 58833,
                MirrorImageRight    = 58831
            };

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MirrorImageLeft, true);
                l_Caster->CastSpell(l_Caster, eSpells::MirrorImageFront, true);
                l_Caster->CastSpell(l_Caster, eSpells::MirrorImageRight, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_mirror_image_summon_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_mirror_image_summon_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Arcane Blast (mirror image) - 88084
class spell_mage_arcane_blast_mirror_image : public SpellScriptLoader
{
    public:
        spell_mage_arcane_blast_mirror_image() : SpellScriptLoader("spell_mage_arcane_blast_mirror_image") { }

        class spell_mage_arcane_blast_mirror_image_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_arcane_blast_mirror_image_SpellScript);

            enum eSpells
            {
                ArcaneCharge = 36032
            };

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::ArcaneCharge);
                if (l_Aura)
                    SetHitDamage(GetHitDamage() * (1 + 0.25f * l_Aura->GetStackAmount())); ///< SpellClassMask from Arcane Charge doest not increase this arcane blast
                /// (formula from http://www.askmrrobot.com/wow/theory/mechanic/spell/mirrorimagearcaneblast?spec=MageArcane&version=7_1_5_23420)
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::ArcaneCharge);
                if (l_Aura)
                    l_Aura->SetDuration(10000); ///< Mirror Image's Arcane Charge only lasts 10sec
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_blast_mirror_image_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_mage_arcane_blast_mirror_image_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_arcane_blast_mirror_image_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Arcane Blast - 30451, Fireball - 133, Frostbolt - 116
/// Called for Unstable Magic - 157976
class spell_mage_unstable_magic: public SpellScriptLoader
{
    public:
        spell_mage_unstable_magic() : SpellScriptLoader("spell_mage_unstable_magic") { }

        class spell_mage_unstable_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_unstable_magic_SpellScript);

            enum eSpells
            {
                ArcaneBlast         = 30451,
                Fireball            = 133,
                FrostboltDamage     = 228597,
                UnstableMagic       = 157976,
                UnstableMagicArcane = 157979,
                UnstableMagicFrost  = 157978,
                UnstableMagicFire   = 157977
            };

            void HandleDamage(SpellEffIndex p_SpellEffectIndex)
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                Aura* l_Aura = l_Player->GetAura(eSpells::UnstableMagic);
                Unit* l_Target = GetHitUnit();
                if (!l_Player || !l_Aura || !l_Target)
                    return;

                int32 l_Chance = 0;
                switch (l_Player->GetActiveSpecializationID())
                {
                    case SpecIndex::SPEC_MAGE_ARCANE:
                        l_Chance = l_Aura->GetEffect(EFFECT_0) ? l_Aura->GetEffect(EFFECT_0)->GetAmount() : 0;
                        break;
                    case SpecIndex::SPEC_MAGE_FIRE:
                        l_Chance = l_Aura->GetEffect(EFFECT_2) ? l_Aura->GetEffect(EFFECT_2)->GetAmount() : 0;
                        break;
                    case SpecIndex::SPEC_MAGE_FROST:
                        l_Chance = l_Aura->GetEffect(EFFECT_1) ? l_Aura->GetEffect(EFFECT_1)->GetAmount() : 0;
                        break;
                    default:
                        break;
                }

                if (roll_chance_i(l_Chance))
                {
                    Spell const* l_Spell = GetSpell();
                    if (!l_Spell)
                        return;

                    int32 l_Pct = l_Aura->GetEffect(EFFECT_3) ? l_Aura->GetEffect(EFFECT_3)->GetAmount() : 0;

                    bool l_IsCrit = l_Spell->IsCritForTarget(l_Target);
                    int32 l_BasePoints = 0;
                    if (l_IsCrit)
                        l_BasePoints = (CalculatePct(GetHitDamage(), l_Pct)) * 2;
                    else
                        l_BasePoints = CalculatePct(GetHitDamage(), l_Pct);

                    switch (m_scriptSpellId)
                    {
                        case eSpells::ArcaneBlast:
                        {
                            if (l_Player->CanApplyPvPSpellModifiers())
                                l_BasePoints *= 0.60f; ///< Unstable Magic (Arcane) damages is reduced by 40% in PvP

                            l_Player->CastCustomSpell(l_Target, eSpells::UnstableMagicArcane, &l_BasePoints, nullptr, nullptr, true);
                            break;
                        }
                        case eSpells::FrostboltDamage:
                        {
                            l_Player->CastCustomSpell(l_Target, eSpells::UnstableMagicFrost, &l_BasePoints, nullptr, nullptr, true);
                            break;
                        }
                        case eSpells::Fireball:
                        {
                            l_Player->CastCustomSpell(l_Target, eSpells::UnstableMagicFire, &l_BasePoints, nullptr, nullptr, true);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->ModifyPower(POWER_ARCANE_CHARGES, GetSpellInfo()->Effects[EFFECT_1].BasePoints);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_unstable_magic_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);

                if (m_scriptSpellId == eSpells::ArcaneBlast)
                    AfterCast += SpellCastFn(spell_mage_unstable_magic_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_unstable_magic_SpellScript();
        }
};

/// Pyroblast - 11366
class spell_mage_pyroblast: public SpellScriptLoader
{
    public:
        spell_mage_pyroblast() : SpellScriptLoader("spell_mage_pyroblast") { }

        class spell_mage_pyroblast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_pyroblast_SpellScript);

            enum eSpells
            {
                HeatingUp                   = 48108,
                WodPvPFire4PBonus           = 171169,
                WodPvPFire4PBonusEffect     = 171170,
                MarqueeBindingsOfTheSunKing = 209450,
                KaelthassUltimateAbility    = 209455

            };

            bool m_InstantPyroblast;
            bool m_BoostedPyroblast;

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                m_InstantPyroblast = false;
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::HeatingUp))
                    m_InstantPyroblast = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::HeatingUp) && m_InstantPyroblast == true)
                {
                    m_InstantPyroblast = true;
                    l_Caster->RemoveAura(eSpells::HeatingUp);
                }

                if (l_Caster->HasAura(eSpells::KaelthassUltimateAbility) && m_InstantPyroblast == false)
                {
                    l_Caster->RemoveAura(eSpells::KaelthassUltimateAbility);
                    m_BoostedPyroblast = true;
                }
            }

            void HandleDamage()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::KaelthassUltimateAbility);
                if (m_InstantPyroblast)
                {
                    /// Item - Mage WoD PvP Fire 4P Bonus
                    if (l_Caster->HasAura(eSpells::WodPvPFire4PBonus))
                        l_Caster->CastSpell(l_Target, eSpells::WodPvPFire4PBonusEffect, true);

                    if (m_InstantPyroblast == true)
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MarqueeBindingsOfTheSunKing);
                        /// 209450 Marquee Bindings of the Sun King -> Item in game says 15% chances proc rate
                        if (l_SpellInfo && l_Caster->HasAura(eSpells::MarqueeBindingsOfTheSunKing) && roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                            l_Caster->CastSpell(l_Caster, eSpells::KaelthassUltimateAbility, true);
                    }
                }
                else if (l_SpellInfo && m_BoostedPyroblast)
                {
                    int32 l_Damage = GetHitDamage();
                    AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                    SetHitDamage(l_Damage);
                    m_BoostedPyroblast = false;
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_mage_pyroblast_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_mage_pyroblast_SpellScript::HandleAfterCast);
                BeforeHit += SpellHitFn(spell_mage_pyroblast_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_pyroblast_SpellScript();
        }
};

// Call by Ring of Frost (Aura) - 82691
// Ring of Frost (immunity 2.5s) - 91264
class spell_mage_ring_of_frost_immunity : public SpellScriptLoader
{
    public:
        spell_mage_ring_of_frost_immunity() : SpellScriptLoader("spell_mage_ring_of_frost_immunity") { }

        class spell_mage_ring_of_frost_immunity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_ring_of_frost_immunity_AuraScript);

            enum eSpells
            {
                RingOfFrostImmmunate = 91264
            };

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpells::RingOfFrostImmmunate, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_ring_of_frost_immunity_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_mage_ring_of_frost_immunity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_ring_of_frost_immunity_SpellScript);

            enum eSpells
            {
                PVPAura = 134735
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target)
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                if (!l_Caster->IsValidAttackTarget(l_Target))
                    return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                if (l_Caster->HasAura(eSpells::PVPAura)) /// In pvp, if the mage is Hard CC'ed, the root won't trigger: http://www.warcraftmovies.com/movieview.php?id=210911
                {
                    uint32 unitflag = l_Caster->GetUInt32Value(UNIT_FIELD_FLAGS);     /// Get unit state. taken from Spell::CheckCast(bool strict)

                    if (unitflag & UNIT_FLAG_STUNNED)
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    else if (unitflag & UNIT_FLAG_CONFUSED)
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    else if (unitflag & UNIT_FLAG_FLEEING)
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    else if (unitflag & UNIT_FLAG_SILENCED)
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    else if (unitflag & UNIT_FLAG_PACIFIED)
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    else if (l_Caster->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_NO_ACTIONS))
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_mage_ring_of_frost_immunity_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_ring_of_frost_immunity_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_ring_of_frost_immunity_AuraScript();
        }
};


/// 7.1.5 - Build 23420
class PlayerScript_mage_arcane_charges_check : public PlayerScript
{
public:
    PlayerScript_mage_arcane_charges_check() :PlayerScript("PlayerScript_mage_arcane_charges_check") {}

    void OnEnterBG(Player* p_Player, uint32 p_MapID) override
    {
        if (!p_Player || p_Player->getClass() != CLASS_MAGE)
            return;

        p_Player->SetPower(POWER_ARCANE_CHARGES, 0, false);
    }

    void OnMapEnter(Player* p_Player) override
    {
        if (!p_Player)
            return;

        auto l_Map = p_Player->GetMap();

        if (l_Map && l_Map->IsRaid())   ///< Shoudn't preserve arcane charges on this
            p_Player->SetPower(POWER_ARCANE_CHARGES, 0, false);
    }
};

/// Last Update 7.1.5 - Build 23420
/// Called by Scorch - 2948
class spell_mage_scorch: public SpellScriptLoader
{
    public:
        spell_mage_scorch() : SpellScriptLoader("spell_mage_scorch") { }

        class spell_mage_scorch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_scorch_SpellScript);

            enum eSpells
            {
                WodPvPFire2PBonus       = 165977,
                WodPvPFire2PBonusEffect = 165979,
                KoralonsBurningTouch    = 208099
            };

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// Item  Mage WoD PvP Fire 2P Bonus
                    if (l_Caster->HasAura(eSpells::WodPvPFire2PBonus))
                        l_Caster->CastSpell(l_Caster, eSpells::WodPvPFire2PBonusEffect, true);
                }
            }

            void HandleDamages(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Spell* l_Spell = GetSpell();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::KoralonsBurningTouch);
                if (!l_SpellInfo || !l_Caster || !l_Caster->HasAura(eSpells::KoralonsBurningTouch) || !l_Spell)
                    return;

                if (!l_Target)
                    return;

                if (l_Target->HealthBelowPct(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                {
                    int32 l_Damage = GetHitDamage();
                    AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_1].BasePoints);
                    SetHitDamage(l_Damage);

                    l_Spell->SetCustomCritChance(100);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_scorch_SpellScript::HandleOnHit);
                OnEffectHitTarget += SpellEffectFn(spell_mage_scorch_SpellScript::HandleDamages, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_scorch_SpellScript();
        }
};

/// Enhanced Pyrotechnics - 157642
class spell_mage_enhanced_pyrotechnics : public SpellScriptLoader
{
    public:
        spell_mage_enhanced_pyrotechnics() : SpellScriptLoader("spell_mage_enhanced_pyrotechnics") { }

        class spell_mage_enhanced_pyrotechnics_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_enhanced_pyrotechnics_AuraScript);

            enum eSpells
            {
                Fireball = 133,
                EnchancedPyrotechnicsProc = 157644
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_EventInfo.GetActor()->GetGUID() != l_Caster->GetGUID())
                    return;

                if (!p_EventInfo.GetDamageInfo()->GetSpellInfo())
                    return;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::Fireball)
                    return;

                if (p_EventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
                    l_Caster->RemoveAura(eSpells::EnchancedPyrotechnicsProc);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::EnchancedPyrotechnicsProc, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mage_enhanced_pyrotechnics_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_enhanced_pyrotechnics_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Mage WoD PvP Frost 2P Bonus - 180721
class spell_mage_WoDPvPFrost2PBonus : public SpellScriptLoader
{
    public:
        spell_mage_WoDPvPFrost2PBonus() : SpellScriptLoader("spell_mage_WoDPvPFrost2PBonus") { }

        class spell_mage_WoDPvPFrost2PBonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_WoDPvPFrost2PBonus_AuraScript);

            enum eSpells
            {
                PvpFrost2PBonusTrigger  = 180723,
                ConeOfCold              = 120
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mage_WoDPvPFrost2PBonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_WoDPvPFrost2PBonus_AuraScript();
        }
};

/// Last Update 7.0.3
/// Cone of Frost - 120
class spell_mage_cone_of_frost : public SpellScriptLoader
{
    public:
        spell_mage_cone_of_frost() : SpellScriptLoader("spell_mage_cone_of_frost") { }

        class spell_mage_cone_of_frost_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_cone_of_frost_SpellScript);

            enum eSpells
            {
                PvpFrost2PBonusTrigger  = 180723,
                WoDPvPFrost2PBonus      = 180721,
                ConeOfColdAura          = 212792
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || GetMissInfo() != SpellMissInfo::SPELL_MISS_NONE)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ConeOfColdAura, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::WoDPvPFrost2PBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::PvpFrost2PBonusTrigger, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_cone_of_frost_SpellScript::HandleAfterCast);
                OnHit += SpellHitFn(spell_mage_cone_of_frost_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_cone_of_frost_SpellScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Called by Mass Invisibility - 198158
class spell_mage_mass_invis : public SpellScriptLoader
{
public:
    spell_mage_mass_invis() : SpellScriptLoader("spell_mage_mass_invis") { }

    class spell_mage_mass_invis_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_mass_invis_SpellScript);

        void HandleOnHit()
        {
            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            std::set<Unit*> l_ChannelingPlayers;
            for (std::pair<uint32, AuraApplication*> const& l_AuraApp : l_Target->GetAppliedAuras())
            {
                Aura* l_Aura = l_AuraApp.second->GetBase();
                if (!l_Aura || !l_Aura->GetCaster())
                    continue;

                if (l_Aura->GetSpellInfo()->IsChanneled())
                    l_ChannelingPlayers.insert(l_Aura->GetCaster());
            }

            for (Unit* l_ChannelingPlayer : l_ChannelingPlayers)
                l_ChannelingPlayer->InterruptSpell(CURRENT_CHANNELED_SPELL, false, true, GetSpell());
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_mage_mass_invis_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_mass_invis_SpellScript();
    }
};

// Call by Blast Wave 157981 - Supernova 157980 - Ice Nova 157997
class spell_mage_novas_talent : public SpellScriptLoader
{
    public:
        spell_mage_novas_talent() : SpellScriptLoader("spell_mage_novas_talent") { }

        class spell_mage_novas_talent_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_novas_talent_SpellScript);

            enum eSpells
            {
                Supernova   = 157980,
                BlastWave   = 157981,
                IceNova     = 157997
            };

            uint64 m_MainTarget;

            void HandleOnCast()
            {
                m_MainTarget = 0;

                if (GetExplTargetUnit() != nullptr)
                    m_MainTarget = GetExplTargetUnit()->GetGUID();
            }

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Target->GetGUID() == m_MainTarget && !l_Target->IsFriendlyTo(l_Caster))
                        {
                            SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), GetSpellInfo()->Effects[EFFECT_0].BasePoints));
                        }
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_novas_talent_SpellScript::HandleOnCast);
                switch (m_scriptSpellId)
                {
                    case eSpells::Supernova:
                    case eSpells::IceNova:
                        OnEffectHitTarget += SpellEffectFn(spell_mage_novas_talent_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                        break;
                    case eSpells::BlastWave:
                        OnEffectHitTarget += SpellEffectFn(spell_mage_novas_talent_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                        break;
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_novas_talent_SpellScript();
        }
};

/// Incanter's Flow - 1463
class spell_mage_incanters_flow : public SpellScriptLoader
{
    public:
        spell_mage_incanters_flow() : SpellScriptLoader("spell_mage_incanters_flow") { }

        class spell_mage_incanters_flow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_incanters_flow_AuraScript);

            enum  eSpells
            {
                IncantersFlow = 116267
            };

            bool m_Up = true;
            bool m_Changed = false;

            void OnTick(AuraEffect const*)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// Break the cycle if caster is out of combat
                    if (!l_Caster->isInCombat())
                        return;

                    if (l_Caster->HasAura(eSpells::IncantersFlow))
                    {
                        if (Aura* l_IncantersFlow = l_Caster->GetAura(eSpells::IncantersFlow))
                        {
                            m_Changed = false;

                            if (l_IncantersFlow->GetStackAmount() == 5 && m_Up)
                            {
                                m_Up = false;
                                m_Changed = true;
                            }
                            else if (l_IncantersFlow->GetStackAmount() == 1 && !m_Up)
                            {
                                m_Up = true;
                                m_Changed = true;
                            }

                            if (!m_Changed)
                                l_IncantersFlow->ModStackAmount(m_Up ? 1 : -1);
                        }
                    }
                    else if (l_Caster->isInCombat())
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::IncantersFlow, true);
                        m_Up = true;
                        m_Changed = false;
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_incanters_flow_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_incanters_flow_AuraScript();
        }
};

/// Last update 7.1.5
/// Ignite - 12654
class spell_mage_ignite : public SpellScriptLoader
{
public:
    spell_mage_ignite() : SpellScriptLoader("spell_mage_ignite") { }

    class spell_mage_ignite_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_ignite_AuraScript);

        enum  eSpells
        {
            PhoenixRebornTrait = 215773,
            PhoenixReborn = 215775,
            PhoenixFlame = 194466
        };

        void HandleOnTick(AuraEffect const* /*p_AuraEffect*/)
        {
            Unit* l_Target = GetTarget();
            Unit* l_Caster = GetCaster();
            uint32 l_ChanceToProc = 10;

            if (!l_Target || !l_Caster)
                return;

            Player* l_Player = GetCaster()->ToPlayer();
            if (!l_Player)
                return;

            if (!l_Caster->HasAura(eSpells::PhoenixRebornTrait))
                return;

            if (!roll_chance_i(l_ChanceToProc))
                return;

            l_Caster->CastSpell(l_Target, eSpells::PhoenixReborn, true);

            if (SpellInfo const* l_PhoenixFlame = sSpellMgr->GetSpellInfo(eSpells::PhoenixFlame))
                l_Player->ReduceChargeCooldown(l_PhoenixFlame->ChargeCategoryEntry, 10 * IN_MILLISECONDS);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ignite_AuraScript::HandleOnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_ignite_AuraScript();
    }
};

/// Glyph of the Unbound Elemental - 146976
class spell_mage_glyph_of_the_unbound_elemental : public SpellScriptLoader
{
    public:
        spell_mage_glyph_of_the_unbound_elemental() : SpellScriptLoader("spell_mage_glyph_of_the_unbound_elemental") { }

        class spell_mage_glyph_of_the_unbound_elemental_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_glyph_of_the_unbound_elemental_AuraScript);

            enum eUnboundWaterElemental
            {
                UnboundWaterElementalTransform = 147358
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    Pet* l_Pet = l_Player->GetPet();

                    if (l_Pet == nullptr)
                        return;

                    l_Pet->CastSpell(l_Pet, eUnboundWaterElemental::UnboundWaterElementalTransform, true);
                }
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    Pet* l_Pet = l_Player->GetPet();

                    if (l_Pet == nullptr)
                        return;

                    if (l_Pet->HasAura(eUnboundWaterElemental::UnboundWaterElementalTransform))
                        l_Pet->RemoveAurasDueToSpell(eUnboundWaterElemental::UnboundWaterElementalTransform);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_glyph_of_the_unbound_elemental_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_glyph_of_the_unbound_elemental_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_glyph_of_the_unbound_elemental_AuraScript();
        }
};

/// last update : 7.3.5 26365
/// Arcane Power - 12042
class spell_mage_arcane_power : public SpellScriptLoader
{
    public:
        spell_mage_arcane_power() : SpellScriptLoader("spell_mage_arcane_power") { }

        class spell_mage_arcane_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_arcane_power_AuraScript);

            enum eSpells
            {
                WodPvpArcane4pBonusAura = 171351,
                WodPvpArcane4pBonus = 171375
            };

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::WodPvpArcane4pBonusAura))
                    l_Caster->CastSpell(l_Caster, eSpells::WodPvpArcane4pBonus, true);
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::WodPvpArcane4pBonus))
                    l_Caster->RemoveAurasDueToSpell(eSpells::WodPvpArcane4pBonus);
            }

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.83333f;  ///< Arcane Power is reduced by 16.7% in PvP
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_arcane_power_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_arcane_power_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_arcane_power_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_arcane_power_AuraScript();
        }
};

/// Flameglow - 140468
class spell_mage_flameglow : public SpellScriptLoader
{
    public:
        spell_mage_flameglow() : SpellScriptLoader("spell_mage_flameglow") { }

        class spell_mage_flameglow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_flameglow_AuraScript);

            enum eSpells
            {
                Flameglow = 140468
            };

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                if (Unit *l_Caster = GetCaster())
                    p_Amount = (l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL) * GetSpellInfo()->Effects[EFFECT_1].BasePoints) / 100;
            }

            void AfterAbsorb(AuraEffect* p_AurEff, DamageInfo & /*p_DmgInfo*/, uint32 & p_AbsorbAmount)
            {
                if (Unit *l_Caster = GetCaster())
                if (l_Caster->HasSpell(eSpells::Flameglow))
                    p_AurEff->SetAmount(p_AbsorbAmount + ((l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL) * GetSpellInfo()->Effects[EFFECT_1].BasePoints) / 100));
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo & p_DmgInfo, uint32 & p_AbsorbAmount)
            {
                if (Unit* l_Attacker = p_DmgInfo.GetActor())
                    p_AbsorbAmount = std::min(p_AbsorbAmount, CalculatePct(p_DmgInfo.GetAmount(), GetSpellInfo()->Effects[EFFECT_2].BasePoints));
            }


            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_flameglow_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_flameglow_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_flameglow_AuraScript::AfterAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_flameglow_AuraScript();
        }
};

/// Ring of Frost (Freeze) - 82691 - last update: 6.1.2 19865
class spell_ring_of_frost_freeze : public SpellScriptLoader
{
    public:
        spell_ring_of_frost_freeze() : SpellScriptLoader("spell_ring_of_frost_freeze") { }

        enum Spells
        {
            RingOfFrostImmune = 91264
        };

        class spell_ring_of_frost_freeze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ring_of_frost_freeze_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                    return;

                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(GetTarget(), Spells::RingOfFrostImmune, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_ring_of_frost_freeze_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ring_of_frost_freeze_AuraScript();
        }
};


/// Item - Mage T17 Fire 4P Bonus - 165459
class spell_mage_item_t17_fire_4p_bonus : public SpellScriptLoader
{
    public:
        spell_mage_item_t17_fire_4p_bonus() : SpellScriptLoader("spell_mage_item_t17_fire_4p_bonus") { }

        class spell_mage_item_t17_fire_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_item_t17_fire_4p_bonus_AuraScript);

            enum eSpells
            {
                PyroblastInstantProc    = 48108,
                Pyromaniac              = 166868
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_ProcSpell || l_ProcSpell->Id != eSpells::PyroblastInstantProc)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Pyromaniac, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mage_item_t17_fire_4p_bonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_item_t17_fire_4p_bonus_AuraScript();
        }
};

/// Item - Mage T17 Arcane 4P Bonus - 165476
class spell_mage_item_t17_arcane_4p_bonus : public SpellScriptLoader
{
    public:
        spell_mage_item_t17_arcane_4p_bonus() : SpellScriptLoader("spell_mage_item_t17_arcane_4p_bonus") { }

        class spell_mage_item_t17_arcane_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_item_t17_arcane_4p_bonus_AuraScript);

            enum eSpells
            {
                ArcaneCharge        = 36032,
                ArcaneInstability   = 166872
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_ProcSpell || l_ProcSpell->Id != eSpells::ArcaneCharge)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ArcaneInstability, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mage_item_t17_arcane_4p_bonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_item_t17_arcane_4p_bonus_AuraScript();
        }
};

/// last update : 6.2.3
/// Illusion - 131784
class spell_mage_illusion : public SpellScriptLoader
{
    public:
        spell_mage_illusion() : SpellScriptLoader("spell_mage_illusion") { }

        class spell_mage_illusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_illusion_SpellScript);

            enum eSpells
            {
                IllusionEffect = 80396,
                IllusionBasic = 94632
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                if (Unit* l_Target = l_Player->GetSelectedPlayer())
                    l_Target->CastSpell(l_Player, eSpells::IllusionEffect, true);
                else
                    l_Player->CastSpell(l_Player, eSpells::IllusionBasic);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_illusion_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_illusion_SpellScript();
        }
};

/// Last Update 7.3.5 26365
/// Arcane Missiles! - 79683
class spell_mage_arcane_missiles_visual : public SpellScriptLoader
{
    public:
        spell_mage_arcane_missiles_visual() : SpellScriptLoader("spell_mage_arcane_missiles_visual") { }

        class spell_mage_arcane_missiles_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_arcane_missiles_visual_AuraScript);

            enum eSpells
            {
                ArcaneMissilesVisualOneCharge = 170571,
                ArcaneMissilesVisualTwoCharge = 79808,
                ArcaneMissilesVisualThreeCharge = 170572
            };

            void OnRemove(AuraEffect const* /*p_Aura*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualOneCharge);
                l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualTwoCharge);
                l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualThreeCharge);
            }

            void OnApply(AuraEffect const* /*p_Aura*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualOneCharge);
                l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualTwoCharge);
                l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualThreeCharge);

                switch(GetStackAmount())
                {
                     case 1:
                          l_Caster->CastSpell(l_Caster, eSpells::ArcaneMissilesVisualOneCharge, true);
                          break;
                     case 2:
                          l_Caster->CastSpell(l_Caster, eSpells::ArcaneMissilesVisualTwoCharge, true);
                          break;
                     case 3:
                          l_Caster->CastSpell(l_Caster, eSpells::ArcaneMissilesVisualThreeCharge, true);
                          break;
                }
            }

            void OnUpdate(uint32 p_Diff)
            {
                if (p_Diff < m_UpdateDiff)
                {
                    m_UpdateDiff -= p_Diff;
                    return;
                }

                m_UpdateDiff = 100;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (GetStackAmount() >= 3)
                {
                    if (!l_Caster->HasAura(eSpells::ArcaneMissilesVisualThreeCharge))
                    {
                        l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualOneCharge);
                        l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualTwoCharge);
                        l_Caster->CastSpell(l_Caster, eSpells::ArcaneMissilesVisualThreeCharge, true);
                    }
                }
                else if (GetStackAmount() == 2)
                {
                    if (!l_Caster->HasAura(eSpells::ArcaneMissilesVisualTwoCharge))
                    {
                        l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualOneCharge);
                        l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualThreeCharge);
                        l_Caster->CastSpell(l_Caster, eSpells::ArcaneMissilesVisualTwoCharge, true);
                    }
                }
                else if (GetStackAmount() == 1 && !l_Caster->HasAura(eSpells::ArcaneMissilesVisualOneCharge))
                {
                    l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualTwoCharge);
                    l_Caster->RemoveAura(eSpells::ArcaneMissilesVisualThreeCharge);
                    l_Caster->CastSpell(l_Caster, eSpells::ArcaneMissilesVisualOneCharge, true);
                }
            }

            uint32 m_UpdateDiff = 100;

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_mage_arcane_missiles_visual_AuraScript::OnUpdate);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_arcane_missiles_visual_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_mage_arcane_missiles_visual_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_arcane_missiles_visual_AuraScript();
        }
};

//////////////////////////////////////////////
////////////////              ////////////////
////////////////    LEGION    ////////////////
////////////////              ////////////////
//////////////////////////////////////////////

/// Last Update 7.3.2 Build 25549
/// Conjure Refreshment - 190336
class spell_mage_conjure_refreshment: public SpellScriptLoader
{
    public:
        spell_mage_conjure_refreshment() : SpellScriptLoader("spell_mage_conjure_refreshment") { }

        class spell_mage_conjure_refreshment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_conjure_refreshment_SpellScript);

            enum eSpells
            {
                ConjusteRefreshmentCreateItem   = 116136,
                ConjusteRefreshmentTable        = 43987
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                GroupPtr l_Group = l_Player->GetGroup();
                if (!l_Group)
                    l_Caster->CastSpell(l_Caster, eSpells::ConjusteRefreshmentCreateItem, true);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::ConjusteRefreshmentTable, true);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_mage_conjure_refreshment_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mage_conjure_refreshment_SpellScript();
        }
};

/// Last Update 7.0.3 build 22045
/// Arcane charge 195302
class spell_mage_reset_arcane_charge : public SpellScriptLoader
{
    public:
        spell_mage_reset_arcane_charge() : SpellScriptLoader("spell_mage_reset_arcane_charge") { }

        class spell_mage_reset_arcane_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_reset_arcane_charge_SpellScript);

            enum eSpells
            {
                ArcaneCharge = 36032,
            };

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::ArcaneCharge))
                        l_Caster->RemoveAura(eSpells::ArcaneCharge);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_reset_arcane_charge_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_reset_arcane_charge_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Arcane Missiles - 5143
class spell_mage_arcane_missile : public SpellScriptLoader
{
    public:
        spell_mage_arcane_missile() : SpellScriptLoader("spell_mage_arcane_missile") { }

        class spell_mage_arcane_missile_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_arcane_missile_AuraScript);

            enum eSpell
            {
                ArcaneInstability   = 166872,
                ArcaneMissiles      = 79683,
                T20Arcane4P         = 246225,
                PresenceOfMind      = 205025
            };

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpell::T20Arcane4P))
                    if (SpellInfo const* l_T20Arcane4PInfo = sSpellMgr->GetSpellInfo(eSpell::T20Arcane4P))
                        if (l_Player->HasSpellCooldown(eSpell::PresenceOfMind))
                            l_Player->ReduceSpellCooldown(eSpell::PresenceOfMind, l_T20Arcane4PInfo->Effects[EFFECT_0].BasePoints * 100);

                Aura* l_Aura = l_Caster->GetAura(eSpell::ArcaneMissiles);
                if (!l_Aura)
                    return;

                if (l_Aura->GetStackAmount() >= 2)
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() - 1);
                else
                    l_Player->RemoveAuraFromStack(l_Aura->GetId());

                if (l_Player->HasAura(eSpell::ArcaneInstability))
                    l_Player->RemoveAura(eSpell::ArcaneInstability);
            }

            void HandleAfterEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ModifyPower(POWER_ARCANE_CHARGES, 1);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_arcane_missile_AuraScript::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_arcane_missile_AuraScript::HandleAfterEffectRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);

            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_arcane_missile_AuraScript();
        }
};

/// Rhonin's Assaulting Armwraps - 208080
class spell_mage_rhonins_assaulting_armwraps : public SpellScriptLoader
{
public:
    spell_mage_rhonins_assaulting_armwraps() : SpellScriptLoader("spell_mage_rhonins_assaulting_armwraps") { }

    class spell_mage_rhonins_assaulting_armwraps_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_rhonins_assaulting_armwraps_AuraScript);

        enum eSpells
        {
            ArcaneMissile = 5143
        };

        uint32 m_Time = 0;

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Spell const* l_Spell = p_EventInfo.GetDamageInfo()->GetSpell();

            if (!l_Spell)
                return false;

            if (m_Time + 2010 < getMSTime())
                m_Time = getMSTime();
            else
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_mage_rhonins_assaulting_armwraps_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_rhonins_assaulting_armwraps_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Arcane explosion - 1449
class spell_mage_arcane_explosion : public SpellScriptLoader
{
    public:
        spell_mage_arcane_explosion() : SpellScriptLoader("spell_mage_arcane_explosion") { }

        enum eSpells
        {
            TimeAndSpace = 238126,
            TimeAndSpaceDamage = 240689,
            TimeAndSpaceTrigger = 240692
        };

        class spell_mage_arcane_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_arcane_explosion_SpellScript);

            bool m_AlreadyEnergized = false;

            void HandleEnergize(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);
            }

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (m_AlreadyEnergized || !l_Caster || !l_SpellInfo)
                    return;

                l_Caster->ModifyPower(POWER_ARCANE_CHARGES, l_SpellInfo->Effects[EFFECT_1].BasePoints);
                m_AlreadyEnergized = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::TimeAndSpace))
                {
                    if (AreaTrigger* l_AreaTrigger = l_Caster->GetAreaTrigger(eSpells::TimeAndSpaceTrigger))
                    {
                        l_Caster->CastSpell(l_AreaTrigger, eSpells::TimeAndSpaceDamage, true);
                        l_AreaTrigger->Remove();
                    }

                    l_Caster->CastSpell(l_Caster, eSpells::TimeAndSpaceTrigger, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_explosion_SpellScript::HandleEnergize, EFFECT_1, SPELL_EFFECT_ENERGIZE);
                OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_explosion_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_mage_arcane_explosion_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_arcane_explosion_SpellScript();
        }
};

// Arcane Missiles 79684 proc after cast
class PlayerScript_mage_arcane_missiles : public PlayerScript
{
    public:
        PlayerScript_mage_arcane_missiles() : PlayerScript("PlayerScript_mage_arcane_missiles") {}

        enum eSpells
        {
            ArcaneMissiles = 79684,

            ArcaneMissilesProc = 79683,

            ArcaneBlast = 30451,
            ArcaneBarrage = 44425,
            ArcaneExplosion = 1449,
            ArcaneOrb = 153626,
            NetherTempest = 114923,
            Supernova = 157980,
            MarkOfAluneth = 224968,
            WordsOfPower = 205035,
            EtherealSensitivity = 187276,
            T192PBonus = 211379 ///<Item - Mage T19 Arcane 2P Bonus
        };

        std::vector<eSpells> m_Spells =
        {
            eSpells::ArcaneBlast,
            eSpells::ArcaneBarrage,
            eSpells::ArcaneExplosion,
            eSpells::ArcaneOrb,
            eSpells::NetherTempest,
            eSpells::Supernova,
            eSpells::MarkOfAluneth
        };

        // Only for arcane mages
        void AfterSpellCast(Player* p_Caster, Spell* p_Spell, bool p_SkipCheck)
        {
            if (!p_Caster || !p_Spell)
                return;

            if (p_Caster->getClass() == CLASS_MAGE && p_Caster->GetActiveSpecializationID() == SPEC_MAGE_ARCANE && p_Caster->HasAura(ArcaneMissiles))
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ArcaneMissiles);
                SpellInfo const* l_ProcSpell = p_Spell->GetSpellInfo();
                if (!l_SpellInfo || !l_ProcSpell)
                    return;

                if (p_Spell->m_FromClient)
                    if (std::find(m_Spells.begin(), m_Spells.end(), l_ProcSpell->Id) == m_Spells.end())
                        return;

                if (std::find(m_Spells.begin(), m_Spells.end(), l_ProcSpell->Id) == m_Spells.end())
                    return;

                uint8 l_ProcChance = l_SpellInfo->Effects[EFFECT_0].BasePoints + 5; ///< Arcane Missiles 79684 different from tooltip
                if (AuraEffect* l_AuraEffect = p_Caster->GetAuraEffect(eSpells::EtherealSensitivity, EFFECT_0))
                    l_ProcChance += l_AuraEffect->GetAmount();

                if (Aura* l_Aura = p_Caster->GetAura(eSpells::WordsOfPower))
                {
                    AuraEffect* l_AuraEffect0 = l_Aura->GetEffect(EFFECT_0);
                    AuraEffect* l_AuraEffect1 = l_Aura->GetEffect(EFFECT_1);
                    if (l_AuraEffect0 && l_AuraEffect1)
                        l_ProcChance += l_AuraEffect0->GetAmount() * ((100.0f - p_Caster->GetPowerPct(POWER_MANA)) / l_AuraEffect1->GetAmount());
                }

                if (AuraEffect* l_AuraEffect = p_Caster->GetAuraEffect(eSpells::T192PBonus, EFFECT_0))
                    l_ProcChance += l_AuraEffect->GetAmount();

                if (l_ProcSpell->Id == eSpells::ArcaneBlast)
                    l_ProcChance *= 2; ///< Proc chance is double for Arcane Blast

                if (roll_chance_i(l_ProcChance))
                    p_Caster->CastSpell(p_Caster, eSpells::ArcaneMissilesProc, true);
            }
        }
};

/// Last Update 7.1.5 Build 22522
/// Called by Arcane Barrage - 44425
class spell_mage_arcane_barrage: public SpellScriptLoader
{
    public:
        spell_mage_arcane_barrage() : SpellScriptLoader("spell_mage_arcane_barrage") { }

        class spell_mage_arcane_barrage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_arcane_barrage_SpellScript);

            enum eSpells
            {
                ChronoShiftAura                     = 235711,
                ChronoShiftSlow                     = 236299,
                ChronoShiftFast                     = 236298,
                ArcaneReboundAura                   = 188006,
                ArcaneReboundDamage                 = 210817,
                MysticKiltOfTheRuneMaster           = 209280,
                MysticKiltOfTheRuneMasterEnergize   = 209302,
                Resonance                           = 205028,
                MantleOfTheFirstKirinTor            = 248098,
                ArcaneOrb                           = 153626
            };

            uint64  m_MainTargetGUID = 0;
            int32   m_NbHit = 0;
            bool    m_ArcanRebound = false;

            void HandleBeforeCast()
            {
                Unit* l_MainTarget = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_MainTarget)
                    return;

                m_MainTargetGUID = l_MainTarget->GetGUID();

                float l_Range = 10.0f; ///< not found in spellinfos, assuming it's the same range as before.
                std::list<Unit*> l_Targets;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_MainTarget, l_Caster, l_Range);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_MainTarget, l_Targets, l_Check);
                l_MainTarget->VisitNearbyObject(l_Range, l_Searcher);

                m_NbHit = std::min((l_Caster->GetPower(POWER_ARCANE_CHARGES)) + 1, static_cast<int32>(l_Targets.size()));
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Target->GetGUID() != m_MainTargetGUID)
                    SetHitDamage(CalculatePct(GetHitDamage(), GetSpellInfo()->Effects[EFFECT_1].BasePoints));

                if (l_Target->GetGUID() == m_MainTargetGUID && l_Caster->HasAura(eSpells::ChronoShiftAura))
                {
                    l_Target->CastSpell(l_Target, eSpells::ChronoShiftSlow, true);
                    l_Caster->CastSpell(l_Caster, eSpells::ChronoShiftFast, true);
                }
            }

            void HandleConsumeArcaneCharge()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MysticKiltOfTheRuneMaster);
                if (l_SpellInfo && l_Caster->HasAura(eSpells::MysticKiltOfTheRuneMaster))
                {
                    uint32 l_RegenAmout = CalculatePct(l_Caster->GetMaxPower(POWER_MANA), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                    l_RegenAmout *= l_Caster->GetPower(POWER_ARCANE_CHARGES);
                    l_Caster->EnergizeBySpell(l_Caster, eSpells::MysticKiltOfTheRuneMasterEnergize, l_RegenAmout, POWER_MANA);
                }

                if (l_Caster->HasAura(eSpells::MantleOfTheFirstKirinTor))
                {
                    if (SpellInfo const* l_MantleSpellInfo = sSpellMgr->GetSpellInfo(eSpells::MantleOfTheFirstKirinTor))
                    {
                        float l_Chance = l_MantleSpellInfo->Effects[EFFECT_0].BasePoints * l_Caster->GetPower(POWER_ARCANE_CHARGES);
                        if (roll_chance_i(l_Chance))
                            l_Caster->CastSpell(l_Caster, eSpells::ArcaneOrb, true);
                    }
                }

                l_Caster->SetPower(POWER_ARCANE_CHARGES, 0, false);
            }

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Resonance);
                if (!l_Caster || !l_Target)
                    return;

                if (l_SpellInfo && l_Caster->HasAura(eSpells::Resonance))
                {
                    int32 l_Damages = GetHitDamage();
                    SetHitDamage(AddPct(l_Damages, l_SpellInfo->Effects[EFFECT_0].BasePoints * m_NbHit));
                }

                if (m_NbHit >= 3 && l_Caster->HasAura(eSpells::ArcaneReboundAura) && m_ArcanRebound == false)
                {
                    Unit* l_MainTarget = sObjectAccessor->FindUnit(m_MainTargetGUID);
                    int32 l_Damage = l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL) * 2;

                    l_Caster->CastCustomSpell(l_MainTarget, eSpells::ArcaneReboundDamage, &l_Damage, nullptr, nullptr, true);
                    m_ArcanRebound = true;
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_arcane_barrage_SpellScript::HandleHit);
                BeforeCast += SpellCastFn(spell_mage_arcane_barrage_SpellScript::HandleBeforeCast);
                OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_barrage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_mage_arcane_barrage_SpellScript::HandleConsumeArcaneCharge);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_arcane_barrage_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Blink - 1953 - Shimmer - 212653 - Displacement - 195676  Displacement - 212801
/// Used for Displacement - 195676  and Shimmer + Displacement - 212801
class spell_mage_displacement_beacon: public SpellScriptLoader
{
    public:
        spell_mage_displacement_beacon() : SpellScriptLoader("spell_mage_displacement_beacon") { }

        class spell_mage_displacement_beacon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_displacement_beacon_SpellScript);

            enum eSpells
            {
                Blink                   = 1953,
                Shimmer                 = 212653,
                DisplacementBeacon      = 212799,
                Displacement            = 195676,
                DisplacementShimmer     = 212801,
                BelovirsFinalStandAura  = 207277,
                BelovirsFinalStandBuff  = 207283,
                EveryWhereAtOnce        = 187301
            };

            enum eArtifacts
            {
                EveryWhereAtOnceArtifact = 83
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                AreaTrigger* l_AreaTrigger = l_Caster->GetLastAreaTrigger(eSpells::DisplacementBeacon);
                if (!l_AreaTrigger || !l_Caster->IsPlayer())
                    return SPELL_FAILED_ERROR;

                if (l_Caster->GetDistance2d(l_AreaTrigger) > 100.0f)
                    return SPELL_FAILED_OUT_OF_RANGE;

                return SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::BelovirsFinalStandAura))
                    l_Player->CastSpell(l_Player, eSpells::BelovirsFinalStandBuff, true);

                if (Spell* spell = l_Player->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                {
                    if (spell->GetSpellInfo()->SpellFamilyName != SPELLFAMILY_MAGE)
                    {
                        l_Player->InterruptSpell(CURRENT_CHANNELED_SPELL, false);
                    }
                }

                if (l_SpellInfo->Id == eSpells::Blink || l_SpellInfo->Id == eSpells::Shimmer)
                {
                    if (!l_Player->HasSpell(eSpells::Displacement) && !l_Player->HasSpell(eSpells::DisplacementShimmer))
                        return;

                    l_Player->RemoveSpellCooldown(eSpells::DisplacementBeacon);
                    l_Player->CastSpell(l_Caster, eSpells::DisplacementBeacon, true);
                    if (Aura* l_Aura = l_Player->GetAura(eSpells::DisplacementBeacon))
                    {
                        if (l_Player->HasAura(eSpells::EveryWhereAtOnce))
                        {
                            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EveryWhereAtOnce);
                            if (!l_SpellInfo)
                                return;

                            uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifacts::EveryWhereAtOnceArtifact);
                            l_Aura->SetMaxDuration(l_Aura->GetDuration() + (l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank * IN_MILLISECONDS));
                            l_Aura->SetDuration(l_Aura->GetDuration() + (l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank * IN_MILLISECONDS));

                            if (AreaTrigger* l_DisplacementBeacon = l_Player->GetLastAreaTrigger(eSpells::DisplacementBeacon))
                                l_DisplacementBeacon->SetDuration(l_Aura->GetDuration());
                    return;
                        }
                    }

                    return;
                }

                AreaTrigger* l_DisplacementBeacon = l_Player->GetLastAreaTrigger(eSpells::DisplacementBeacon);
                if (!l_DisplacementBeacon)
                    return;

                l_Player->NearTeleportTo(*l_DisplacementBeacon);
                l_Player->RemoveAura(eSpells::DisplacementBeacon);

                if (l_Player->HasSpell(eSpells::Shimmer))
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Shimmer);
                    if (!l_SpellInfo)
                        return;

                    l_Player->ModSpellCharge(eSpells::Shimmer, l_Player->GetMaxCharges(l_SpellInfo->ChargeCategoryEntry) + 1 - l_Player->GetConsumedCharges(l_SpellInfo->ChargeCategoryEntry));
                }
                else
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Blink);
                    if (!l_SpellInfo)
                        return;

                    l_Player->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
                }

                if (l_Player->HasAura(eSpells::EveryWhereAtOnce))
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EveryWhereAtOnce);
                    if (!l_SpellInfo)
                        return;

                    uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifacts::EveryWhereAtOnceArtifact);
                    l_Player->ReduceSpellCooldown(eSpells::DisplacementShimmer, l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank * IN_MILLISECONDS * 2);
                    l_Player->ReduceSpellCooldown(eSpells::Displacement, l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank * IN_MILLISECONDS * 2);
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::DisplacementShimmer || m_scriptSpellId == eSpells::Displacement)
                    OnCheckCast += SpellCheckCastFn(spell_mage_displacement_beacon_SpellScript::CheckCast);

                AfterCast += SpellCastFn(spell_mage_displacement_beacon_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_displacement_beacon_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Living Bomb - 44457
class spell_mage_living_bomb : public SpellScriptLoader
{
    public:
        spell_mage_living_bomb() : SpellScriptLoader("spell_mage_living_bomb") { }

        class spell_mage_living_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_living_bomb_SpellScript);

            enum eSpells
            {
                LivingBombAura = 217694
            };

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Unit* l_OriginalCaster = GetOriginalCaster();
                if (!l_Caster || !l_Target || !l_OriginalCaster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::LivingBombAura, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_living_bomb_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_living_bomb_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22045
/// Living Bomb Aura - 217694
class spell_mage_living_bomb_aura : public SpellScriptLoader
{
    public:
        spell_mage_living_bomb_aura() : SpellScriptLoader("spell_mage_living_bomb_aura") { }

        class spell_mage_living_bomb_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_living_bomb_aura_AuraScript);

            enum eSpells
            {
                LivingBombExplosion = 44461
            };

            void OnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (p_AuraEffect && p_AuraEffect->GetAmount() != 0)
                {
                    int32 l_Value = 1;
                    l_Caster->CastCustomSpell(l_Target, eSpells::LivingBombExplosion, &l_Value, NULL, NULL, true, NULL, nullptr, l_Caster->GetGUID());
                }
                else
                    l_Caster->CastSpell(l_Target, eSpells::LivingBombExplosion, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb_aura_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_living_bomb_aura_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Living Bomb Explosion - 44461
class spell_mage_living_bomb_explosion : public SpellScriptLoader
{
    public:
        spell_mage_living_bomb_explosion() : SpellScriptLoader("spell_mage_living_bomb_explosion") { }

        class spell_mage_living_bomb_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_living_bomb_explosion_SpellScript);

            enum eSpells
            {
                LivingBombAura = 217694
            };

            bool m_ShouldNotSpread = 0;

            void HandleShouldSpread(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                m_ShouldNotSpread = GetEffectValue();
            }

            void HandleSpread(SpellEffIndex p_EffIndex)
            {
                Unit* l_ExplicitTarget = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_ExplicitTarget)
                    return;

                if (l_Target == l_ExplicitTarget)
                    return;

                if (m_ShouldNotSpread == 1)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::LivingBombAura, true);

                if (Aura* l_LivingBombAura = l_Target->GetAura(eSpells::LivingBombAura, l_Caster->GetGUID()))
                {
                    if (AuraEffect* l_LivingBombAuraEffect = l_LivingBombAura->GetEffect(EFFECT_1))
                        l_LivingBombAuraEffect->SetAmount(1);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_living_bomb_explosion_SpellScript::HandleShouldSpread, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHitTarget += SpellEffectFn(spell_mage_living_bomb_explosion_SpellScript::HandleSpread, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_living_bomb_explosion_SpellScript();
        }
};

/// Last Update 7.0.3
/// Called by Flamestrike - 2120
class spell_mage_flamestrike : public SpellScriptLoader
{
    public:
        spell_mage_flamestrike() : SpellScriptLoader("spell_mage_flamestrike") { }

        class spell_mage_flamestrike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_flamestrike_SpellScript);

            enum eSpells
            {
                FlameStrikeVisual = 213084,
                FlamePatch = 205037,
                FlamePatchAT = 205470,
                HeatingUp = 48108,
                MarqueeBindingsOfTheSunKing = 209450,
                KaelthassUltimateAbility = 209455
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                m_InstantPyroblast = false;
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::HeatingUp))
                    m_InstantPyroblast = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::KaelthassUltimateAbility);
                if (m_InstantPyroblast)
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MarqueeBindingsOfTheSunKing);
                    /// 209450 Marquee Bindings of the Sun King -> Item in game says 15% chances proc rate
                    if (l_SpellInfo && l_Caster->HasAura(eSpells::MarqueeBindingsOfTheSunKing) && roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                        l_Caster->CastSpell(l_Caster, eSpells::KaelthassUltimateAbility, true);
                }

                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest->GetPositionX(), l_Dest->GetPositionY(), l_Dest->GetPositionZ(), eSpells::FlameStrikeVisual, true);

                if (l_Caster->HasAura(eSpells::FlamePatch))
                    l_Caster->CastSpell(l_Dest->GetPositionX(), l_Dest->GetPositionY(), l_Dest->GetPositionZ(), eSpells::FlamePatchAT, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_mage_flamestrike_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_mage_flamestrike_SpellScript::HandleAfterCast);
            }

        private:
            bool m_InstantPyroblast;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_flamestrike_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Kindling - 155148
class spell_mage_kindling : public SpellScriptLoader
{
    public:
        spell_mage_kindling() : SpellScriptLoader("spell_mage_kindling") { }

        class spell_mage_kindling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_kindling_AuraScript);

            enum eSpells
            {
                Fireball    = 133,
                Pyroblast   = 11366,
                FireBlast   = 108853,
                Combustion  = 190319
            };

            std::vector<uint32> m_Spells =
            {
                eSpells::Fireball,
                eSpells::Pyroblast,
                eSpells::FireBlast
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || std::find(m_Spells.begin(), m_Spells.end(), l_SpellInfo->Id) == m_Spells.end())
                    return false;

                if (!(p_ProcEventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->ReduceSpellCooldown(eSpells::Combustion, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_kindling_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_mage_kindling_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_kindling_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Cauterize - 86949
class spell_mage_cauterize: public SpellScriptLoader
{
    public:
        spell_mage_cauterize() : SpellScriptLoader("spell_mage_cauterize") { }

        class spell_mage_cauterize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_cauterize_AuraScript);

            enum eSpells
            {
                CauterizeBurn   = 87023,
                Cauterize       = 108843,
                Cauterized      = 87024
            };

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* /*auraEffect*/, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = GetTarget();
                SpellInfo const* l_CauterizeBurn = sSpellMgr->GetSpellInfo(eSpells::CauterizeBurn);
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target  || !l_CauterizeBurn || !l_SpellInfo)
                    return;

                if (p_DamageInfo.GetAmount() < l_Target->GetHealth())
                    return;

                if (l_Target->HasAura(eSpells::Cauterized))
                    return;

                l_Target->SetHealth(CalculatePct(l_Target->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_1].BasePoints));
                l_Target->CastSpell(l_Target, eSpells::CauterizeBurn, true);
                l_Target->CastSpell(l_Target, eSpells::Cauterized, true);
                l_Target->CastSpell(l_Target, eSpells::Cauterize, true);

                p_AbsorbAmount = p_DamageInfo.GetAmount();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_cauterize_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_cauterize_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_cauterize_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Greater Pyroblast - 203286
class spell_mage_greater_pyroblast : public SpellScriptLoader
{
    public:
        spell_mage_greater_pyroblast() : SpellScriptLoader("spell_mage_greater_pyroblast") { }

        class spell_mage_greater_pyroblast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_greater_pyroblast_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_SpellInfo || !l_Caster)
                    return;

                if (Creature* l_Creature = l_Target->ToCreature())
                {
                    if (CreatureTemplate const* l_CreatureTemplate = l_Creature->GetCreatureTemplate())
                    {
                        if (!l_Creature->isPet() && l_CreatureTemplate->rank != CreatureEliteType::CREATURE_ELITE_NORMAL)
                            SetHitDamage(int32((float)l_Caster->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_2].BasePoints) / 100.0f));
                        else
                            SetHitDamage(int32((float)l_Target->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_2].BasePoints) / 100.0f));
                    }
                }
                else
                    SetHitDamage(int32((float)l_Target->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_2].BasePoints) / 100.0f));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_greater_pyroblast_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_greater_pyroblast_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Fireball - 133, Pyroblast - 11366
/// For Firestarter - 205026
class spell_mage_firestarter : public SpellScriptLoader
{
    public:
        spell_mage_firestarter() : SpellScriptLoader("spell_mage_firestarter") { }

        class spell_mage_firestarter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_firestarter_SpellScript);

            enum eSpells
            {
                Firestarter = 205026,
            };

            void HandleOnPrepare()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                Spell* l_Spell = GetSpell();
                SpellInfo const* l_Firestarter = sSpellMgr->GetSpellInfo(eSpells::Firestarter);
                if (!l_Target || !l_Caster || !l_Spell)
                    return;

                if (l_Target->GetHealthPct() > l_Firestarter->Effects[EFFECT_0].BasePoints && l_Caster->HasAura(eSpells::Firestarter))
                    l_Spell->SetCustomCritChance(100);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_mage_firestarter_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_firestarter_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Tinder - 203275, 203277
class spell_mage_tinder : public SpellScriptLoader
{
    public:
        spell_mage_tinder() : SpellScriptLoader("spell_mage_tinder") { }

        class spell_mage_tinder_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_tinder_AuraScript);

            enum eSpells
            {
                Fireball    = 133,
                Tinder      = 203275,
                TinderBuff  = 203277
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Fireball)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_TinderAura = l_Caster->GetAura(eSpells::Tinder);
                if (!l_TinderAura)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Tinder) = false;
                for (uint8 l_I = EFFECT_0; l_I <= EFFECT_1; ++l_I) ///< Need to unset effects values
                {
                    AuraEffect* l_AuraEffect = l_TinderAura->GetEffect(l_I);
                    if (l_AuraEffect)
                        l_AuraEffect->CalculateSpellMod();
                }
            }

            void HandleOnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || l_Caster->HasAura(eSpells::TinderBuff) || !l_Aura)
                    return;

                AuraEffect* l_AuraEffect = const_cast<AuraEffect*>(p_AurEff);
                l_AuraEffect->SetAmount(l_AuraEffect->GetAmount() + 1);
                if (l_AuraEffect->GetAmount() < 8)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Tinder) = true;
                for (uint8 l_I = EFFECT_0; l_I <= EFFECT_1; ++l_I) ///< Need to reset effects values
                {
                    AuraEffect* l_TinderAuraEffect = l_Aura->GetEffect(l_I);
                    if (l_TinderAuraEffect)
                        l_TinderAuraEffect->CalculateSpellMod();
                }

                l_Caster->CastSpell(l_Caster, eSpells::TinderBuff, true);
                l_AuraEffect->SetAmount(0);
            }

            void HandleCalcSpellMod(AuraEffect const* p_AurEff, SpellModifier*& p_SpellMod)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->m_SpellHelper.GetBool(eSpellHelpers::Tinder))
                {
                    l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::Tinder][p_AurEff->GetEffIndex()] = p_SpellMod->value;
                    p_SpellMod->value = 0;
                }
                else
                    p_SpellMod->value = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::Tinder][p_AurEff->GetEffIndex()];
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::TinderBuff:
                        DoCheckProc += AuraCheckProcFn(spell_mage_tinder_AuraScript::HandleCheckProc);
                        OnProc += AuraProcFn(spell_mage_tinder_AuraScript::HandleOnProc);
                        break;
                    case eSpells::Tinder:
                        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_mage_tinder_AuraScript::HandleCalcSpellMod, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
                        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_mage_tinder_AuraScript::HandleCalcSpellMod, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_tinder_AuraScript::HandleOnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_tinder_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Tinder - 210126
class PlayerScript_tinder : public PlayerScript
{
    public:
        PlayerScript_tinder() : PlayerScript("PlayerScript_tinder") { }

        enum eSpells
        {
            Fireball    = 133,
            Tinder      = 203275,
            TinderBuff  = 203277
        };

        void OnSpellRemoved(Player* p_Player, uint32 p_SpellID)
        {
            if (p_Player->getClass() == CLASS_MAGE && p_Player->GetActiveSpecializationID() == SPEC_MAGE_FIRE && p_SpellID == eSpells::Tinder)
                p_Player->RemoveAura(eSpells::TinderBuff);
        }

        void AfterSpellCast(Player* p_Player, Spell* p_Spell, bool /*p_SkipCheck*/)
        {
            if (p_Player->getClass() != CLASS_MAGE || p_Player->GetActiveSpecializationID() != SPEC_MAGE_FIRE)
                return;

            if (!p_Spell->m_spellInfo || p_Spell->m_spellInfo->Id != eSpells::Fireball)
                return;

            AuraEffect* l_AuraEffect1 = p_Player->GetAuraEffect(eSpells::Tinder, EFFECT_1);
            AuraEffect* l_AuraEffect2 = p_Player->GetAuraEffect(eSpells::Tinder, EFFECT_2);
            if (!l_AuraEffect1 || !l_AuraEffect2)
                return;

            p_Player->m_SpellHelper.GetBool(eSpellHelpers::Tinder) = false;
            l_AuraEffect1->CalculateSpellMod();
            l_AuraEffect2->SetAmount(0);
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Pyroblast - 11366 - Flamestrike - 2120
/// For Pyromaniac - 205020
class spell_mage_pyromaniac : public SpellScriptLoader
{
    public:
        spell_mage_pyromaniac() : SpellScriptLoader("spell_mage_pyromaniac") { }

        class spell_mage_pyromaniac_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_pyromaniac_SpellScript);

            enum eSpells
            {
                Pyromaniac  = 205020,
                HotStreak   = 48108
            };

            bool m_RefreshHotStreak = false;

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_Pyromaniac = sSpellMgr->GetSpellInfo(eSpells::Pyromaniac);
                if (!l_Caster || !l_Pyromaniac)
                    return;

                if (!l_Caster->HasAura(eSpells::Pyromaniac) || !l_Caster->HasAura(eSpells::HotStreak))
                    return;

                if (roll_chance_i(l_Pyromaniac->Effects[EFFECT_0].BasePoints))
                    m_RefreshHotStreak = true;
            }

            void HandleRefresh()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !m_RefreshHotStreak)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::HotStreak, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_mage_pyromaniac_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_mage_pyromaniac_SpellScript::HandleRefresh);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_pyromaniac_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Heating Up - 48107
/// For Hot Streak - 48108 - Controlled Burn - 205033
class spell_mage_heating_up : public SpellScriptLoader
{
    public:
        spell_mage_heating_up() : SpellScriptLoader("spell_mage_heating_up") { }

        class spell_mage_heating_up_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_heating_up_AuraScript);

            enum eSpells
            {
                HotStreak       = 48108,
                ControlledBurn  = 205033
            };

            void OnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::HotStreak))
                    return;

                if (l_Caster->HasAura(eSpells::ControlledBurn))
                {
                    if (SpellInfo const* l_ControlledBurn = sSpellMgr->GetSpellInfo(eSpells::ControlledBurn))
                    {
                        if (roll_chance_i(l_ControlledBurn->Effects[EFFECT_0].BasePoints))
                            l_Caster->CastSpell(l_Caster, eSpells::HotStreak, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_heating_up_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_heating_up_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Hot Streak - 48108
class spell_mage_hot_streak : public SpellScriptLoader
{
public:
    spell_mage_hot_streak() : SpellScriptLoader("spell_mage_hot_streak") { }

    class spell_mage_hot_streak_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_hot_streak_AuraScript);

        enum eSpells
        {
            HeatingUp = 48107,
            Streaking = 211395, ///< Item - Mage T19 Fire 4P Bonus
            StreakingTrigger = 211399
        };

        void OnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->DelayedRemoveAura(eSpells::HeatingUp, 1);

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Streaking, EFFECT_0))
                if (roll_chance_i(l_AuraEffect->GetAmount()))
                    l_Caster->CastSpell(l_Caster, eSpells::StreakingTrigger, true);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_mage_hot_streak_AuraScript::OnApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_hot_streak_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Frostbolt - 116
class spell_mage_frostbolt_procs : public SpellScriptLoader
{
    public:
        spell_mage_frostbolt_procs() : SpellScriptLoader("spell_mage_frostbolt_procs") { }

        class spell_mage_frostbolt_procs_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_frostbolt_procs_SpellScript);

            enum eSpells
            {
                BrainFreeze        = 190447,
                BrainFreezeProc    = 190446,
                ClarityOfThought   = 195351,
                T192PBonus         = 211411, ///< Item - Mage T19 Frost 2P Bonus
                FingerFrost        = 44544,
                FingerFrostVisual  = 126084,
                FingerFrostSpell   = 112965,
                DeepShatter        = 198123,
                T20FrostMage4P     = 242254,
                FrozenOrb          = 84714
            };

            enum eTraits
            {
                ClarityOfThoughtTrait = 789
            };

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                SpellInfo const* l_BrainFreezeInfo = sSpellMgr->GetSpellInfo(eSpells::BrainFreeze);
                SpellInfo const* l_ClarityOfThought = sSpellMgr->GetSpellInfo(eSpells::ClarityOfThought);
                SpellInfo const* l_T192PBonus = sSpellMgr->GetSpellInfo(eSpells::T192PBonus);

                if (!l_Player || !l_BrainFreezeInfo || !l_ClarityOfThought || !l_T192PBonus)
                    return;

                uint32 l_Chance = l_BrainFreezeInfo->Effects[EFFECT_0].BasePoints + l_T192PBonus->Effects[EFFECT_0].BasePoints + l_Player->GetRankOfArtifactPowerId(eTraits::ClarityOfThoughtTrait) * l_ClarityOfThought->Effects[EFFECT_0].BasePoints;
                if (l_Player->HasAura(eSpells::BrainFreeze) && roll_chance_i(l_Chance))
                {
                    l_Player->CastSpell(l_Player, eSpells::BrainFreezeProc, true);

                    if (l_Player->HasAura(eSpells::T20FrostMage4P))
                        if (SpellInfo const* l_T20Frost4PInfo = sSpellMgr->GetSpellInfo(eSpells::T20FrostMage4P))
                            if (l_Player->HasSpellCooldown(eSpells::FrozenOrb))
                                l_Player->ReduceSpellCooldown(eSpells::FrozenOrb, l_T20Frost4PInfo->Effects[EFFECT_0].BasePoints * 100);
                }

                SpellInfo const* l_FingerFrostInfo = sSpellMgr->GetSpellInfo(eSpells::FingerFrostSpell);
                if (!l_FingerFrostInfo)
                    return;

                if (roll_chance_i(l_FingerFrostInfo->Effects[EFFECT_0].BasePoints))
                {
                    if (l_Player->HasAura(eSpells::FingerFrost))
                        l_Player->CastSpell(l_Player, eSpells::FingerFrostVisual, true);

                    if (!l_Player->HasAura(eSpells::DeepShatter))
                        l_Player->CastSpell(l_Player, eSpells::FingerFrost, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_frostbolt_procs_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_frostbolt_procs_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Frostbolt - 228597 - triggered by 116
class spell_mage_frostbolt : public SpellScriptLoader
{
    public:
        spell_mage_frostbolt() : SpellScriptLoader("spell_mage_frostbolt") { }

        class spell_mage_frostbolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_frostbolt_SpellScript);

            bool m_TargetIsSnare = false;

            enum eSpells
            {
                WoDPvPFrost4PBonus = 180741,
                DeepShatter        = 198123,
                WaterJet           = 135029,
                FingerFrost        = 44544,
                FingerFrostVisual  = 126084
            };

            void HandleOnPrepare()
            {
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || l_Target->HasAuraType(SPELL_AURA_MOD_ROOT))
                    m_TargetIsSnare = true;
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WoDPvPFrost4PBonus);

                if (l_Target == nullptr || l_SpellInfo == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::WoDPvPFrost4PBonus))
                {
                    if (m_TargetIsSnare)
                        SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), l_SpellInfo->Effects[EFFECT_0].BasePoints));
                }

                if (l_Caster->HasAura(eSpells::DeepShatter) && l_Target->isFrozen())
                {
                    if (SpellInfo const* l_DeepShatter = sSpellMgr->GetSpellInfo(eSpells::DeepShatter))
                        SetHitDamage(GetHitDamage() + GetHitDamage() * (l_DeepShatter->Effects[EFFECT_0].BasePoints / 100.0f));
                }
            }

            void HandleOnHit()
            {
                if (GetCaster())
                {
                    Player* l_Player = GetCaster()->ToPlayer();
                    Unit* l_Target = GetHitUnit();

                    if (!l_Player)
                        return;

                    Pet* l_Pet = l_Player->GetPet();

                    if (!l_Pet || !l_Target)
                        return;

                    if (l_Target->HasAura(eSpells::WaterJet, l_Pet->GetGUID()))
                    {
                        if (l_Player->HasAura(eSpells::FingerFrost))
                            l_Player->CastSpell(l_Player, eSpells::FingerFrostVisual, true); ///< Fingers of frost visual 2 procs

                        l_Player->CastSpell(l_Player, eSpells::FingerFrost, true);  ///< Fingers of frost proc
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_frostbolt_SpellScript::HandleOnHit);
                OnPrepare += SpellOnPrepareFn(spell_mage_frostbolt_SpellScript::HandleOnPrepare);
                OnEffectHitTarget += SpellEffectFn(spell_mage_frostbolt_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_frostbolt_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Ice Lance - 228598
class spell_mage_ice_lance : public SpellScriptLoader
{
    public:
        spell_mage_ice_lance() : SpellScriptLoader("spell_mage_ice_lance") { }

        class spell_mage_ice_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_ice_lance_SpellScript);

            enum eSpells
            {
                T17Frost4P          = 165469,
                IceShard            = 166869,
                SplittingIce        = 56377,
                FingersOfFrost      = 44544,
                FrostBombTriggered  = 113092,
                FrostBombAura       = 112948,
                IcyVeins            = 12472,
                ThermalVoid         = 155149,
                WintersChill        = 228358,
                ObsidianLance       = 238056,
                ArcticBlast         = 253257
            };

            enum eArtifactRank
            {
                ObsidianLanceArtifact = 1537
            };

            bool m_Second = false;
            bool m_PrimaryTargetWasFrozen = false;

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                /// Each Ice Lance cast while Frozen Orb is active reduces the cast time of Frostbolt by 2% and increases the damage of Frostbolt by 2% for 10 sec. Stacks up to 10 times.
                if (l_Caster->HasAura(eSpells::T17Frost4P))
                {
                    Creature* l_FrostOrb = nullptr;
                    for (auto l_Iter : l_Caster->m_Controlled)
                    {
                        if (l_Iter->GetEntry() == MagePet::FrozenOrb)
                            l_FrostOrb = l_Iter->ToCreature();
                    }

                    if (l_FrostOrb == nullptr)
                        return;

                    l_Caster->CastSpell(l_Caster, eSpells::IceShard, true);
                }

                if (Unit* l_Target = GetHitUnit())
                {
                    if (l_Target->HasAuraState(AURA_STATE_FROZEN, l_SpellInfo, l_Caster))
                    {
                        if (l_Target->HasAura(eSpells::FrostBombAura, l_Caster->GetGUID()))
                            l_Caster->CastSpell(l_Target, eSpells::FrostBombTriggered, true);
                    }
                }
            }

            void HandleDamage()
            {
                Unit* l_Caster = GetCaster();
                uint32 l_Damage = GetHitDamage();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_Damage || !l_Caster || !l_SpellInfo)
                    return;

                if (m_Second)
                {
                    SpellInfo const* l_SplittingIceInfo = sSpellMgr->GetSpellInfo(eSpells::SplittingIce);

                    if (l_SplittingIceInfo)
                    {
                        l_Damage = CalculatePct(l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IceLance), l_SplittingIceInfo->Effects[EFFECT_1].BasePoints);
                        if (l_Target->isFrozen() || l_Caster->m_SpellHelper.GetBool(eSpellHelpers::HadFingersOfFrost))
                            l_Damage *= 3;
                    }

                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::HadFingersOfFrost) = false;
                }

                if (l_Caster->HasSpell(eSpells::ThermalVoid) && l_Target->HasAuraState(AURA_STATE_FROZEN, l_SpellInfo, l_Caster))
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::IcyVeins, l_Caster->GetGUID()))
                    {
                        int32 l_IncreaseDuration = sSpellMgr->GetSpellInfo(eSpells::ThermalVoid)->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS;
                        int32 l_NewDuration = (l_Aura->GetDuration() + l_IncreaseDuration) > 30000 ? 30000 : (l_Aura->GetDuration() + l_IncreaseDuration);
                        l_Aura->SetDuration(l_NewDuration);
                    }
                }

                Aura* l_FingerOfFrost = l_Caster->GetAura(eSpells::FingersOfFrost);
                bool l_WintersChill = false;
                if (Aura* l_WintersChillAura = l_Target->GetAura(eSpells::WintersChill, l_Caster->GetGUID()))
                    l_WintersChill = true;

                if (!m_Second)
                {
                    if (l_FingerOfFrost)
                    {
                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::HadFingersOfFrost) = true;
                        l_FingerOfFrost->DropStack();
                    }

                    if (l_Target->isFrozen() || l_FingerOfFrost || l_WintersChill)
                    {
                        l_Damage *= 3;
                        m_PrimaryTargetWasFrozen = true;
                    }
                }

                m_Second = false;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_ObsidianInfo = sSpellMgr->GetSpellInfo(eSpells::ObsidianLance);
                if (l_Caster->HasAura(eSpells::ObsidianLance) && l_Player && l_ObsidianInfo && (l_Target->isFrozen() || l_FingerOfFrost || l_WintersChill))
                    AddPct(l_Damage, l_Player->GetRankOfArtifactPowerId(eArtifactRank::ObsidianLanceArtifact) * l_ObsidianInfo->Effects[EFFECT_0].BasePoints);

                SetHitDamage(l_Damage);
            }

            void SelectLance(SpellEffIndex /*p_EffectIndex*/)
            {
                if (GetEffectValue() && GetEffectValue() == 99)
                {
                    GetSpell()->SetCustomCritChance(0);
                    m_Second = true;
                }
            }

            void SaveFirstLanceDamage()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!m_Second)
                {
                    Unit* l_Target = GetHitUnit();
                    int32 l_Damage = GetHitDamage();

                    if (!l_Target || (l_Target->ToCreature() && l_Target->ToCreature()->GetCreatureTemplate()
                        && l_Target->ToCreature()->GetCreatureTemplate()->type == CreatureType::CREATURE_TYPE_CRITTER))
                        l_Damage = 0;

                    if (l_Caster)
                        l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IceLance) = l_Damage * (m_PrimaryTargetWasFrozen ? 0.33f : 1); ///< Saving "base" damage without taking frozen into account
                }

                l_Caster->DelayedRemoveAura(eSpells::ArcticBlast, 500);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_ice_lance_SpellScript::HandleOnHit);
                OnHit += SpellHitFn(spell_mage_ice_lance_SpellScript::HandleDamage);
                AfterHit += SpellHitFn(spell_mage_ice_lance_SpellScript::SaveFirstLanceDamage);
                OnEffectLaunch += SpellEffectFn(spell_mage_ice_lance_SpellScript::SelectLance, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_ice_lance_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Fingers of Frost - 44544
class spell_mage_fingers_of_frost : public SpellScriptLoader
{
    public:
        spell_mage_fingers_of_frost() : SpellScriptLoader("spell_mage_fingers_of_frost") { }

        class spell_mage_fingers_of_frost_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_fingers_of_frost_AuraScript);

            enum eSpells
            {
                FingersOfFrostVisualRight = 126084
            };

            void HandleVisual(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_FingersOfFrost = GetAura();
                if (!l_Caster || !l_FingersOfFrost)
                    return;

                if (l_FingersOfFrost->GetStackAmount() == 1)
                    l_Caster->RemoveAura(eSpells::FingersOfFrostVisualRight);
                else if (l_FingersOfFrost->GetStackAmount() == 2 && !l_Caster->HasAura(eSpells::FingersOfFrostVisualRight))
                    l_Caster->AddAura(eSpells::FingersOfFrostVisualRight, l_Caster);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_fingers_of_frost_AuraScript::HandleVisual, EFFECT_0, SPELL_AURA_ABILITY_IGNORE_AURASTATE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_fingers_of_frost_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Comet Storm - 153595 and Comet Storm damage spell - 153596
class spell_mage_comet_storm : public SpellScriptLoader
{
    public:
        spell_mage_comet_storm() : SpellScriptLoader("spell_mage_comet_storm") { }

        class spell_mage_comet_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_comet_storm_SpellScript);

            enum eCometDatas
            {
                MaxComets           = 7,
                CometStorm          = 153596,
                CometStormVisual    = 228601
            };

            uint16 m_CountTarget = 0;
            bool m_AlreadyLaunch = false;

            void OnCometFalls(SpellEffIndex p_Idx)
            {
                if (m_AlreadyLaunch) ///< Prevent to enter on each targets hit
                    return;

                m_AlreadyLaunch = true;

                Unit* l_Caster = GetCaster();

                if (WorldLocation const* l_Dest = GetExplTargetDest())
                {
                    uint32& l_AmountOfComets = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::AmountOfComets);
                    int32 l_AmountOfUsedComets = l_AmountOfComets;
                    if (GetSpellInfo()->Id == 153596 && l_AmountOfUsedComets >= 1 && l_AmountOfUsedComets <= 7)
                    {
                        l_AmountOfComets = l_AmountOfUsedComets + 1;
                        /// It's done, all comets are used
                        if (l_AmountOfUsedComets >= 7)
                            l_AmountOfComets = 0;
                        else
                        {
                            float l_X = l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::CometCoordinateX) + frand(-4.0f, 4.0f);
                            float l_Y = l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::CometCoordinateY) + frand(-4.0f, 4.0f);

                            l_Caster->CastSpell(l_X, l_Y, l_Dest->m_positionZ, eCometDatas::CometStormVisual, true);
                            l_Caster->CastSpell(l_X, l_Y, l_Dest->m_positionZ, eCometDatas::CometStorm, true);
                        }
                    }
                }
            }

            void HandleAfterHit()
            {
                if (GetSpellInfo()->Id == eCometDatas::CometStorm)
                {
                    int32 l_Damage = GetHitDamage(); /// 7.1.5 - Build 23420: damage is no longer split between targets hit

                    if (Unit* l_Target = GetHitUnit())
                    {
                        /// Comet Storm (Frost) damage has increased by 94% but deals 33.3% less damage in PvP combat. - 6.1
                        if (l_Target && l_Target->IsPlayer())
                            l_Damage = l_Damage - CalculatePct(l_Damage, 33.3f);

                        SetHitDamage(l_Damage);

                        /// Polymorph
                        if (l_Target->IsPolymorphed())
                            l_Target->RemoveAurasDueToSpell(l_Target->getTransForm());
                    }
                }
            }

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (WorldLocation const* l_Dest = GetExplTargetDest())
                    {
                        if (GetSpellInfo()->Id == 153595)
                        {
                            /// Store launch information
                            l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::AmountOfComets) = 1;
                            l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::CometCoordinateX) = l_Dest->m_positionX;
                            l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::CometCoordinateY) = l_Dest->m_positionY;
                            /// First comet
                            l_Caster->CastSpell(l_Dest->m_positionX, l_Dest->m_positionY, l_Dest->m_positionZ, eCometDatas::CometStormVisual, true);
                            l_Caster->CastSpell(l_Dest->m_positionX, l_Dest->m_positionY, l_Dest->m_positionZ, eCometDatas::CometStorm, true);
                        }
                    }
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                case 153596:
                    OnEffectHit += SpellEffectFn(spell_mage_comet_storm_SpellScript::OnCometFalls, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                    break;
                default:
                    break;
                }
                AfterCast += SpellCastFn(spell_mage_comet_storm_SpellScript::HandleAfterCast);
                AfterHit += SpellHitFn(spell_mage_comet_storm_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_comet_storm_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Glacial Spike - 199786
class spell_mage_glacial_spike : public SpellScriptLoader
{
    public:
        spell_mage_glacial_spike() : SpellScriptLoader("spell_mage_glacial_spike") { }

        class spell_mage_glacial_spike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_glacial_spike_SpellScript);

            std::array<const int, 5> m_IcicleAuras = { { 214124, 214125, 214126, 214127, 214130 } };
            std::array<int32, 5> m_IcicleDamages = { { 0, 0, 0, 0, 0 } };

            enum eSpells
            {
                GlacialSpikeDamage  = 228600,
                GlacialSpikeUsable  = 199844
            };

            uint32 m_StartingCastTime;
            bool m_IsPrepared;
            int32 m_IciclesDamage = 0;

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                if (!m_IsPrepared)
                {
                    for (const int l_Aura : m_IcicleAuras)
                    {
                        if (!l_Caster->HasAura(l_Aura))
                            return SPELL_FAILED_CASTER_AURASTATE;
                    }
                }

                return SPELL_CAST_OK;
            }

            void HandleOnPrepare()
            {
                m_IsPrepared = true;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target)
                    return;

                m_StartingCastTime = getMSTime();

                int32 l_TotalIciclesDamage = 0;

                for (int8 l_Itr = 0; l_Itr < 5; ++l_Itr)
                {
                    if (AuraEffect* l_IcicleAuraEffect = l_Caster->GetAuraEffect(m_IcicleAuras[l_Itr], EFFECT_0))
                    {
                        m_IcicleDamages[l_Itr] = l_IcicleAuraEffect->GetAmount();
                        l_TotalIciclesDamage += m_IcicleDamages[l_Itr];
                        l_Caster->RemoveAura(m_IcicleAuras[l_Itr]);
                    }
                }

                m_IciclesDamage = l_TotalIciclesDamage;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_GlacialSpike = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_GlacialSpike)
                    return;

                bool l_IsSecond = GetExplTargetUnit() != l_Target;

                CustomSpellValues values;
                values.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT4, m_IciclesDamage);
                values.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT5, l_IsSecond ? 1 : 0);
                l_Caster->CastCustomSpell(eSpells::GlacialSpikeDamage, values, l_Target, true);
            }

            void HandleOnInterrupt(uint32 /*p_Time*/)
            {
                Unit* l_Caster = GetCaster();
                Spell* l_Spell = GetSpell();
                if (!l_Caster || !l_Spell)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                int32 l_TimeDiff = static_cast<int32>(l_Spell->GetCastTime()) - static_cast<int32>(GetMSTimeDiffToNow(m_StartingCastTime));

                if (l_TimeDiff < 0)
                    l_TimeDiff = 0;

                std::array<int32, 5> p_IciclesDamages = m_IcicleDamages;

                l_Player->m_Functions.AddFunction([l_Player, p_IciclesDamages]() -> void
                {
                    static const std::vector<uint32> l_IcicleAuras { 214124, 214125, 214126, 214127, 214130 };

                    uint8 l_I = 0;
                    for (uint32 l_SpellID : l_IcicleAuras)
                        l_Player->CastCustomSpell(l_Player, l_SpellID, &p_IciclesDamages[l_I++], nullptr, nullptr, true);
                }, l_Player->m_Functions.CalculateTime(l_TimeDiff));
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::GlacialSpikeUsable);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_mage_glacial_spike_SpellScript::HandleCheckCast);
                OnPrepare   += SpellOnPrepareFn(spell_mage_glacial_spike_SpellScript::HandleOnPrepare);
                OnHit       += SpellHitFn(spell_mage_glacial_spike_SpellScript::HandleOnHit);
                OnInterrupt += SpellInterruptFn(spell_mage_glacial_spike_SpellScript::HandleOnInterrupt);
                AfterCast += SpellCastFn(spell_mage_glacial_spike_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_glacial_spike_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Glacial Spike Damage - 228600
class spell_mage_glacial_spike_damage : public SpellScriptLoader
{
    public:
        spell_mage_glacial_spike_damage() : SpellScriptLoader("spell_mage_glacial_spike_damage") { }

        class spell_mage_glacial_spike_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_glacial_spike_damage_SpellScript);

            enum eSpells
            {
                GlacialSpikeUsable  = 199844,
                SplittingIce        = 56377,
                WintersChill        = 228358

            };

            int32 m_Damage = 0;
            bool m_IsSecond = false;

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Spell* l_Spell = GetSpell())
                {
                    m_Damage += l_Spell->GetSpellValue(SpellValueMod::SPELLVALUE_BASE_POINT4);
                    m_IsSecond = l_Spell->GetSpellValue(SpellValueMod::SPELLVALUE_BASE_POINT5) == 1;
                    if (m_IsSecond && !l_Caster->m_SpellHelper.GetBool(eSpellHelpers::MainGlacialSpikeCritted))
                        l_Spell->SetCustomCritChance(0.0f);
                }
            }

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Spell const* l_Spell = GetSpell();
                if (!l_Spell || !l_Caster || !l_Target)
                    return;

                m_Damage += GetHitDamage();

                bool l_HadSavedDamage = false;
                if (m_IsSecond)
                {
                    if (Aura const* l_SplittingIceAura = l_Caster->GetAura(eSpells::SplittingIce))
                    {
                        /// At first try to use saved damage
                        if (uint32 l_SavedDamage = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::GlacialSpikeLastDamage))
                        {
                            m_Damage = CalculatePct(l_SavedDamage, l_SplittingIceAura->GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints);
                            l_HadSavedDamage = true;
                        }
                        else
                        {
                            m_Damage = CalculatePct(m_Damage, l_SplittingIceAura->GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints);
                        }
                    }
                }

                if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::MainGlacialSpikeCritted) && m_IsSecond)
                {
                    if (l_Target->IsPlayer())
                        m_Damage /= 1.5f; ///< To avoid "double crits" (main spike crits, off spike crits based on main spike crit damage) NB: damage of spell from the Frost mage are multiplied by 1.5 if they crit in pvp, and by 2
                    else
                        m_Damage /= 2.0f;
                }

                m_Damage *= (l_Caster->CanApplyPvPSpellModifiers() ? 0.90f : 1.f); ///< Glacial Spike is reduced by 10% in PvP

                SetHitDamage(m_Damage);

                if (m_IsSecond)
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::GlacialSpikeLastDamage) = 0;
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                uint32 l_Damage = GetHitDamage();

                if (l_Target->ToCreature() && l_Target->ToCreature()->GetCreatureTemplate() && l_Target->ToCreature()->GetCreatureTemplate()->type == CreatureType::CREATURE_TYPE_CRITTER)
                    l_Damage = 0;

                if (!m_IsSecond)
                {
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::GlacialSpikeLastDamage) = l_Damage;
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::MainGlacialSpikeCritted) = (GetSpell()->IsCritForTarget(l_Target));
                }
            }

            void Register() override
            {
                OnCast              += SpellCastFn(spell_mage_glacial_spike_damage_SpellScript::HandleOnCast);
                OnEffectHitTarget   += SpellEffectFn(spell_mage_glacial_spike_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterHit            += SpellHitFn(spell_mage_glacial_spike_damage_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_glacial_spike_damage_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Ray of Frost - 205021
class spell_mage_ray_of_frost: public SpellScriptLoader
{
    public:
        spell_mage_ray_of_frost() : SpellScriptLoader("spell_mage_ray_of_frost") { }

        class spell_mage_ray_of_frost_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_ray_of_frost_AuraScript);

            enum eSpells
            {
                RayOfFrostBuff = 208141
            };

            void HandleOnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RayOfFrostBuff, true);
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::RayOfFrostBuff);
            }

            void CalcPeriodic(AuraEffect const* /*P_AurEff*/, bool& /*p_IsPeriodic*/, int32& p_Amplitude)
            {
                if (!GetCaster())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                p_Amplitude = int32(p_Amplitude * std::max<float>(l_Player->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED), 0.5f));
            }

            void Register() override
            {
                DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_mage_ray_of_frost_AuraScript::CalcPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ray_of_frost_AuraScript::HandleOnTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_ray_of_frost_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_ray_of_frost_AuraScript();
        }

        class spell_mage_ray_of_frost_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_ray_of_frost_SpellScript);

            void HandleOnCast()
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Spell* l_Spell = GetSpell();
                if (!l_SpellInfo || !l_Spell)
                    return;

                l_Spell->SetChanneledDuration(l_SpellInfo->GetDuration());
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_ray_of_frost_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_ray_of_frost_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Flurry - 44614
class spell_mage_flurry : public SpellScriptLoader
{
    public:
        spell_mage_flurry() : SpellScriptLoader("spell_mage_flurry") { }

        class spell_mage_flurry_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_flurry_SpellScript);

            enum eSpells
            {
                Flurry          = 228596,
                BrainFreeze     = 190446,
                WintersChill    = 228358,
                T21Frost2P      = 251859,
                T21Frost4P      = 251860,
                ArcticBlast     = 253257
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::BrainFreeze))
                {
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BrainFreezeFlurry) = 3;

                    if (l_Caster->HasAura(eSpells::T21Frost4P))
                        l_Caster->CastSpell(l_Caster, eSpells::ArcticBlast, true);
                }

                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint64 l_CasterGuid = l_Caster->GetGUID();
                uint64 l_TargetGuid = l_Target->GetGUID();

                if (l_Caster->HasAura(eSpells::BrainFreeze))
                    l_Caster->CastSpell(l_Target, eSpells::WintersChill, true);

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::T21Frost2PBonus) = 0;

                for (uint32 l_Itr = 1; l_Itr < 3; l_Itr++)
                {
                    l_Caster->m_Functions.AddFunction([l_CasterGuid, l_TargetGuid, l_Itr]() -> void
                    {
                        Player* l_Player = sObjectAccessor->FindPlayer(l_CasterGuid);
                        if (!l_Player)
                            return;

                        Unit* l_Target = sObjectAccessor->GetUnit(*l_Player, l_TargetGuid);
                        if (!l_Target)
                            return;

                        l_Player->CastSpell(l_Target, eSpells::Flurry, true);
                    }, l_Player->m_Functions.CalculateTime(350 * l_Itr));
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_flurry_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_flurry_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Flurry - 228596
class spell_mage_flurry_debuff : public SpellScriptLoader
{
    public:
        spell_mage_flurry_debuff() : SpellScriptLoader("spell_mage_flurry_debuff") { }

        class spell_mage_flurry_debuff_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_flurry_debuff_SpellScript);

            enum eSpells
            {
                WintersChill    = 228358,
                BrainFreezeProc = 190446
            };

            bool m_HasBrainFreeze = false;

            /// Called 3 times (once per bolt)
            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BrainFreezeFlurry) > 0 || l_Caster->HasAura(eSpells::BrainFreezeProc))
                    m_HasBrainFreeze = true;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::WintersChill, l_Caster->GetGUID()) && m_HasBrainFreeze)
                    l_Caster->CastSpell(l_Target, eSpells::WintersChill, true);

                else if (Aura* l_Aura = l_Target->GetAura(eSpells::WintersChill, l_Caster->GetGUID())) /// Considering Flurry's Cast time and travel time, we consider that if target has winterchill, it got applied by THIS VERY flurry cast (the first bolt actually) so we reset the timer of winter chill (blizz-like)
                    l_Aura->SetDuration(l_Aura->GetMaxDuration());
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_flurry_debuff_SpellScript::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_mage_flurry_debuff_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_flurry_debuff_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Flurry - 228354
class spell_mage_flurry_missile : public SpellScriptLoader
{
    public:
        spell_mage_flurry_missile() : SpellScriptLoader("spell_mage_flurry_missile") { }

        class spell_mage_flurry_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_flurry_missile_SpellScript);

            enum eSpells
            {
                BrainFreeze         = 190447,
                BrainFreezeEffect   = 190446,
                BoneChilling        = 205027,
                BoneChillingAura    = 205766,
                T21Frost2P          = 251859
            };

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::BoneChilling))
                    l_Caster->CastSpell(l_Caster, eSpells::BoneChillingAura, true);

                int32 l_Damage = GetHitDamage();
                uint32& l_BrainFreezeCharges = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BrainFreezeFlurry);
                if (l_BrainFreezeCharges > 0)
                {
                    SpellInfo const* l_BrainFreezeInfo = sSpellMgr->GetSpellInfo(eSpells::BrainFreezeEffect);

                    if (l_BrainFreezeInfo)
                        l_Damage = AddPct(l_Damage, l_BrainFreezeInfo->Effects[1].BasePoints);

                    --l_BrainFreezeCharges;
                }

                ///< Each subsequent flurry bolt beyond the first deals 15% more damage than the one before - 7.3.5 Build 26365
                if (l_Caster->HasAura(eSpells::T21Frost2P))
                {
                    if (SpellInfo const* l_T21SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Frost2P))
                    {
                        float l_Bonus = pow(1.0f + (float)(l_T21SpellInfo->Effects[EFFECT_0].BasePoints / 100.0f), (float)l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::T21Frost2PBonus));
                        l_Damage *= l_Bonus;
                        l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::T21Frost2PBonus) += 1;
                    }
                }

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_flurry_missile_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_flurry_missile_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// PlayerScript for Lonely Winter
class PlayerScript_Lonely_Winter : public PlayerScript
{
    public:
        PlayerScript_Lonely_Winter() :PlayerScript("PlayerScript_Lonely_Winter") {}

        enum eSpells
        {
            LonelyWinter = 205024
        };

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID)
        {
            if (p_SpellID == eSpells::LonelyWinter)
            {
                if (Pet* l_Pet = p_Player->GetPet())
                {
                    l_Pet->DespawnOrUnsummon();
                }
            }
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Mark of Aluneth - 224968
class spell_mage_mark_of_aluneth : public SpellScriptLoader
{
    public:
        spell_mage_mark_of_aluneth() : SpellScriptLoader("spell_mage_mark_of_aluneth") { }

        class spell_mage_mark_of_aluneth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_mark_of_aluneth_AuraScript);

            enum eSpells
            {
                MarkOfAlunethDamages    = 211088,
                CordOfInfinity          = 209316,
                AlunethsAvariceAura     = 238090,
                AlunethsAvarice         = 211076
            };

            void HandleDamages(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::MarkOfAlunethDamages, true);
            }

            void HandleExplosion(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                int32 l_Damages = int32(float(l_Caster->GetMaxPower(POWER_MANA)) * (float(p_AuraEffect->GetAmount()) / 100.0f));
                l_Caster->CastCustomSpell(l_Target, eSpells::MarkOfAlunethDamages, &l_Damages, NULL, NULL, true);
                l_Caster->RemoveAura(eSpells::CordOfInfinity);

                if (l_Caster->HasAura(eSpells::AlunethsAvariceAura))
                    l_Caster->CastSpell(l_Caster, eSpells::AlunethsAvarice, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_mark_of_aluneth_AuraScript::HandleDamages, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_mark_of_aluneth_AuraScript::HandleExplosion, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_mark_of_aluneth_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Mark of Aluneth Damage - 211088
class spell_mage_mark_of_aluneth_damage : public SpellScriptLoader
{
public:
    spell_mage_mark_of_aluneth_damage() : SpellScriptLoader("spell_mage_mark_of_aluneth_damage") { }

    class spell_mage_mark_of_aluneth_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_mark_of_aluneth_damage_SpellScript);

        enum eSpells
        {
            CordOfInfinity = 209316
        };

        void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            int32 l_Damage = GetHitDamage();

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::CordOfInfinity, EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount() / 10);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_mage_mark_of_aluneth_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_mark_of_aluneth_damage_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Pheonix's Flames - 194466
class spell_mage_pheonixs_flames : public SpellScriptLoader
{
    public:
        spell_mage_pheonixs_flames() : SpellScriptLoader("spell_mage_pheonixs_flames") { }

        class spell_mage_pheonixs_flames_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_pheonixs_flames_SpellScript);

            enum eSpells
            {
                PhoenixsFlamesSpread = 224637,
                StrafingRun = 238127,
                WarmthOfThePhoenix = 238091,
                WarmthOfThePhoenixProc = 240671
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::StrafingRun))
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PhoenixsFlamesChain) = true;

                if (l_Caster->HasAura(eSpells::WarmthOfThePhoenix))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WarmthOfThePhoenix))
                        if (roll_chance_i(l_SpellInfo->ProcChance))
                            l_Caster->CastSpell(l_Caster, eSpells::WarmthOfThePhoenixProc, true);
            }

            void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PhoenixsFlamesSpread, true);
            }

            void Register() override
            {
                OnEffectHitTarget   += SpellEffectFn(spell_mage_pheonixs_flames_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_mage_pheonixs_flames_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_pheonixs_flames_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Pheonix's Flames - 224637
class spell_mage_pheonixs_flames_spread : public SpellScriptLoader
{
    public:
        spell_mage_pheonixs_flames_spread() : SpellScriptLoader("spell_mage_pheonixs_flames_spread") { }

        class spell_mage_pheonixs_flames_spread_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_pheonixs_flames_spread_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target == GetExplTargetUnit())
                    PreventHitDamage();
            }

            void HandleOnPrepare()
            {
                Spell *l_Spell = GetSpell();

                if (l_Spell)
                    l_Spell->SetCustomCritChance(100);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_mage_pheonixs_flames_spread_SpellScript::HandleOnPrepare);
                OnHit += SpellHitFn(spell_mage_pheonixs_flames_spread_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_pheonixs_flames_spread_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// PlayerScript for Phoenix's Flames
class PlayerScript_Phoenixs_Flames : public PlayerScript
{
    public:
        PlayerScript_Phoenixs_Flames() :PlayerScript("PlayerScript_Phoenixs_Flames") {}

        enum eSpells
        {
            FirstOrb        = 194461,
            SecondOrb       = 226754,
            ThirdOrb        = 226755,
            Immolation      = 211918,
            VisualActivate1 = 232632,
            VisualActivate2 = 232635,
            BlueOrbs        = 232770,
            BlueFlame       = 210182
        };

        enum eDatas
        {
            PheonixsFlamesChargeCategory = 1633
        };

        static void RefreshVisual(Player* p_Player, uint32 p_Delay = 1)
        {
            ClearVisual(p_Player);
            if (p_Player->HasAura(eSpells::Immolation))
            {
                ApplyVisual(p_Player, p_Delay);
            }
        }

        static void ClearVisual(Player* p_Player)
        {
            p_Player->RemoveAurasDueToSpell(eSpells::FirstOrb);
            p_Player->RemoveAurasDueToSpell(eSpells::SecondOrb);
            p_Player->RemoveAurasDueToSpell(eSpells::ThirdOrb);
            p_Player->RemoveAurasDueToSpell(eSpells::BlueOrbs);
            p_Player->RemoveAurasDueToSpell(eSpells::VisualActivate1);
        }

        static void ApplyVisual(Player* p_Player, uint32 p_Delay)
        {
            p_Player->CastSpell(p_Player, eSpells::VisualActivate1, true);
            switch (p_Player->m_CategoryCharges[eDatas::PheonixsFlamesChargeCategory].size())
            {
                case 0:
                    p_Player->CastSpell(p_Player, eSpells::ThirdOrb, true);
                    break;
                case 1:
                    p_Player->CastSpell(p_Player, eSpells::SecondOrb, true);
                    break;
                case 2:
                    p_Player->CastSpell(p_Player, eSpells::FirstOrb, true);
                    break;
                default:
                    break;
            }
            if (p_Player->HasAura(eSpells::BlueFlame))
            {
                p_Player->CastSpell(p_Player, eSpells::BlueOrbs, true);
            }
            p_Player->DelayedCastSpell(p_Player, eSpells::VisualActivate2, true, p_Delay);
        }

        void OnModifyCharges(Player* p_Player, uint32 p_ChargeCategoryEntryID)
        {
            if (!p_Player)
                return;

            if (p_ChargeCategoryEntryID != eDatas::PheonixsFlamesChargeCategory)
                return;

            if (p_Player->GetActiveSpecializationID() != SPEC_MAGE_FIRE)
                return;

            RefreshVisual(p_Player);
        }

        void OnLogin(Player* p_Player) 
        {
            RefreshVisual(p_Player, 2000);
            p_Player->m_SpellHelper.GetBool(eSpellHelpers::FlameOrbActivatedAfterLogin) = true;
        }
};

/// Last Update 7.3.5
/// Called by Immolation - 211918
/// Called for Flame Orb
class spell_mage_immolation : public SpellScriptLoader 
{
public:
    spell_mage_immolation() : SpellScriptLoader("spell_mage_immolation") { }

    class spell_mage_immolation_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_immolation_AuraScript);

        void OnApply(AuraEffect const*, AuraEffectHandleModes)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if (!l_Player->m_SpellHelper.GetBool(eSpellHelpers::FlameOrbActivatedAfterLogin))
                return;

            PlayerScript_Phoenixs_Flames::RefreshVisual(l_Player);
        }

        void OnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if(!l_Player->m_SpellHelper.GetBool(eSpellHelpers::FlameOrbActivatedAfterLogin))
                return;

            PlayerScript_Phoenixs_Flames::RefreshVisual(l_Player);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_mage_immolation_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_mage_immolation_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_immolation_AuraScript();
    }
};

/// Last Update 7.0.3
/// Chilled - 205708
class spell_mage_chilled : public SpellScriptLoader
{
    public:
        spell_mage_chilled() : SpellScriptLoader("spell_mage_chilled") { }

        class spell_mage_chilled_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_chilled_SpellScript);

            enum eSpells
            {
                BoneChilling        = 205027,
                BoneChillingAura    = 205766
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::BoneChilling))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::BoneChillingAura, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_chilled_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_chilled_SpellScript();
        }
};

/// Last update 7.0.3
/// Temporal shield - 198111
class spell_mage_temporal_shield : public SpellScriptLoader
{
    public:
        spell_mage_temporal_shield() : SpellScriptLoader("spell_mage_temporal_shield") { }

        class spell_mage_temporal_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_temporal_shield_AuraScript);

            enum eSpells
            {
                TemporalRipples = 198116,
                TemporalShield  = 198111
            };

            void HandleOnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (!l_Caster || l_DamageInfo == nullptr)
                    return;

                AuraEffect* l_AuraEff = l_Caster->GetAuraEffect(eSpells::TemporalShield, 0);
                l_AuraEff->SetAmount((l_AuraEff->GetAmount() + l_DamageInfo->GetAmount()));
            }

            void OnRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /* p_AuraRemoveMode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH || GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_NONE || l_Caster->HasAuraWithMechanic(1LL << Mechanics::MECHANIC_BANISH))
                    return;

                int32 l_AbsorbedDamage = p_AurEff->GetAmount();
                if (Player* l_Player = l_Caster->ToPlayer())
                    l_Player->CastCustomSpell(l_Player, eSpells::TemporalRipples, &l_AbsorbedDamage, nullptr, nullptr, true);
            }

            void Register() override
            {

                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_temporal_shield_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectProc += AuraEffectProcFn(spell_mage_temporal_shield_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_temporal_shield_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Blink - 1953 - Shimmer - 212653
/// Called for Cauteriz
class spell_mage_blink : public SpellScriptLoader
{
    public:
        spell_mage_blink() : SpellScriptLoader("spell_mage_blink") { }

        class spell_mage_blink_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_blink_SpellScript);

            enum eSpells
            {
                CauterizingBlink        = 194318,
                CauterizingBlinkAura    = 194316,
                PrismaticCloak          = 198064,
                PrismaticCloakBuff      = 198065
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Player* l_Player = GetCaster()->ToPlayer();
                if (l_Player == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;;

                if (l_Player->IsInBattleground())
                {
                    if (Battleground* l_Battleground = l_Player->GetBattleground())
                    {
                        if (l_Battleground->GetStatus() == STATUS_WAIT_JOIN)
                            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                    }
                }

                return SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::CauterizingBlink))
                    l_Caster->CastSpell(l_Caster, eSpells::CauterizingBlinkAura, true);

                if (l_Caster->HasAura(eSpells::PrismaticCloak))
                    l_Caster->CastSpell(l_Caster, eSpells::PrismaticCloakBuff, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_mage_blink_SpellScript::CheckCast);
                AfterCast += SpellCastFn(spell_mage_blink_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_blink_SpellScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Cauterizing Blink - 194316
class spell_mage_cauterizing_blink : public SpellScriptLoader
{
    public:
        spell_mage_cauterizing_blink() : SpellScriptLoader("spell_mage_cauterizing_blink") { }

        class spell_mage_cauterizing_blink_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_cauterizing_blink_AuraScript);

            enum eSpells
            {
                CauterizingBlink = 194318
            };

            enum eArtifactPowers
            {
                CauterizingBlinkID = 756
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                Aura const* l_Aura = l_Caster->GetAura(eSpells::CauterizingBlink);
                if (!l_Player || !l_Aura)
                    return;

                SpellInfo const* l_SpellInfo = l_Aura->GetSpellInfo();
                AuraEffect const* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_AuraEffect || !l_SpellInfo || !l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::CauterizingBlinkID, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::CauterizingBlinkID) - 1);
                if (!l_RankEntry)
                    return;

                p_Amount = CalculatePct(l_Caster->GetMaxHealth(), l_RankEntry->AuraPointsOverride);
                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.50f; ///< Cauterizing blink heal is reduced by 50% in PvP.
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_cauterizing_blink_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };


        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_cauterizing_blink_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Pyretic Incantation - 194331
class spell_mage_pyretic_incantation : public SpellScriptLoader
{
    public:
        spell_mage_pyretic_incantation() : SpellScriptLoader("spell_mage_pyretic_incantation") { }

        class spell_mage_pyretic_incantation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_pyretic_incantation_AuraScript);

            enum eSpells
            {
                PyreticIncantionBonus = 194329
            };

            enum eBlackListSpells
            {
                Flamepatch         = 205472
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();

                if (!l_ProcSpellInfo)
                    return false;

                switch (l_ProcSpellInfo->Id)
                {
                    case eBlackListSpells::Flamepatch:
                        return false;
                    default:
                        return true;
                }
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                uint32 l_HitMask = p_ProcEventInfo.GetHitMask();
                Unit* l_Caster = GetCaster();
                int32 bp = 0;

                if (!l_Caster || !(l_HitMask & PROC_HIT_CRITICAL))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IsPyreticIncantationPrevCrit) = false;
                    l_Caster->RemoveAurasDueToSpell(eSpells::PyreticIncantionBonus);
                    return;
                }

                if (!l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IsPyreticIncantationPrevCrit))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IsPyreticIncantationPrevCrit) = true;

                    /// Pyretic Incantation buff should appear on the first crit u do. Only the stacking part requires successive crits (confirmed by Alexiuly)
                    if (!l_Caster->HasAura(eSpells::PyreticIncantionBonus))
                        l_Caster->CastSpell(l_Caster, eSpells::PyreticIncantionBonus, true);

                    return;
                }

                l_Caster->CastSpell(l_Caster, eSpells::PyreticIncantionBonus, true);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::PyreticIncantionBonus);
                if (!l_SpellInfo)
                    return;

                bp = l_SpellInfo->Effects[EFFECT_0].BasePoints * 2; ///< we multiply here by 2 to get dammages based on critical strikes

                int32 l_AuraStack = l_Caster->GetAuraCount(eSpells::PyreticIncantionBonus);
                l_Caster->CastCustomSpell(l_Caster, eSpells::PyreticIncantionBonus, &bp, NULL, NULL, true);

                Aura *l_Aura = l_Caster->GetAura(eSpells::PyreticIncantionBonus);

                if (l_Aura)
                    l_Aura->SetStackAmount((l_AuraStack == 5) ? 5 : l_Aura->GetStackAmount() - 1);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_pyretic_incantation_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_mage_pyretic_incantation_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_pyretic_incantation_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Combustion - 190319
class spell_mage_combustion : public SpellScriptLoader
{
    public:
        spell_mage_combustion() : SpellScriptLoader("spell_mage_combustion") { }

        class spell_mage_combustion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_combustion_AuraScript);

            enum eSpells
            {
                GlyphOfSmolder          = 219592,
                GlyphOfSmolderEffect    = 219589,
                T21Fire4P               = 251858,
                Inferno                 = 253220
            };

            void HandleComputeAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool&)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;

                if (!l_Caster || !(l_Player = l_Caster->ToPlayer()))
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                int32 l_BonusMastery = l_Caster->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_CRIT_SPELL);
                l_BonusMastery -= 15; ///< Must remove aura bonus fire mage
                AddPct(l_BonusMastery, -(l_SpellInfo->Effects[1].BasePoints));
                p_Amount = l_BonusMastery;
            }

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::GlyphOfSmolder))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::GlyphOfSmolderEffect, true);
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::T21Fire4P))
                    if (Aura* l_Aura = l_Caster->AddAura(eSpells::Inferno, l_Caster))
                        l_Aura->SetDuration(GetDuration());
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_combustion_AuraScript::HandleComputeAmount, EFFECT_1, SPELL_AURA_MOD_RATING);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_combustion_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_MOD_RATING, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_mage_combustion_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_MOD_RATING, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_combustion_AuraScript();
        }
};

class PlayerScript_mage_learn_class_hall_teleport : public PlayerScript
{
    public:
        PlayerScript_mage_learn_class_hall_teleport() :PlayerScript("PlayerScript_dh_learn_spells") {}

        enum eSpells
        {
            MageClassHallTeleport = 193759
        };

        void OnLogin(Player* p_Player)
        {
            if (!(p_Player->getClass() == CLASS_MAGE))
                return;

            if (p_Player->getLevel() > 97 && !p_Player->HasSpell(eSpells::MageClassHallTeleport))
                p_Player->learnSpell(eSpells::MageClassHallTeleport, false, false, false, 0, true);
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Rune of Power - 116014
class spell_mage_rune_of_power : public SpellScriptLoader
{
    public:
        spell_mage_rune_of_power() : SpellScriptLoader("spell_mage_rune_of_power") { }

        class spell_mage_rune_of_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_rune_of_power_AuraScript);

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(l_SpellInfo->Id, p_AuraEffect->GetEffIndex(), l_Caster->GetGUID());
                if (!l_AuraEffect)
                    return;

                /// Rune of Power increases damage by 28 % in PvP (instead of 40 %)
                l_AuraEffect->ChangeAmount(28);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_rune_of_power_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_mage_rune_of_power_AuraScript::HandleAfterApply, EFFECT_2, SPELL_AURA_MOD_PET_DAMAGE_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_rune_of_power_AuraScript();
        }
};

/// Last Update 7.3.5
/// Called by Rune of Power - 116011
class spell_mage_rune_of_power_trigger : public SpellScriptLoader
{
    public:
        spell_mage_rune_of_power_trigger() : SpellScriptLoader("spell_mage_rune_of_power_trigger") { }

        class spell_mage_rune_of_power_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_rune_of_power_trigger_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                l_Caster->ToPlayer()->AddSpellCooldown(l_SpellInfo->Id, 0, 10 * IN_MILLISECONDS, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_rune_of_power_trigger_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_rune_of_power_trigger_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// PlayerScript for Flamecannon - 203284
class PlayerScript_Flamecannon : public PlayerScript
{
    public:
        PlayerScript_Flamecannon() :PlayerScript("PlayerScript_Flamecannon") {}

        enum eSpells
        {
            Flamecannon = 203284
        };

        void OnUpdate(Player* p_Player, uint32 p_Diff) override
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Flamecannon);
            if (!p_Player || p_Player->getClass() != CLASS_MAGE || !l_SpellInfo || !p_Player->HasAura(eSpells::Flamecannon))
                return;

            if (!p_Player->isInCombat() || p_Player->IsMoving())
            {
                if (AuraEffect* l_AurEff = p_Player->GetAuraEffect(eSpells::Flamecannon, EFFECT_0))
                    l_AurEff->ResetPeriodic(true);
            }
        }
};

/// Last Update 7.3.5 26365
/// Polymorph - 118, 28272, 28271, 61780, 61305, 161372, 61721, 161354, 126819, 161355, 161353
class spell_mage_polymorph : public SpellScriptLoader
{
    public:
        spell_mage_polymorph() : SpellScriptLoader("spell_mage_polymorph") { }

        class spell_mage_polymorph_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_polymorph_SpellScript);

            enum eSpells
            {
                VampiricTouch = 34914,
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAuraType(SPELL_AURA_PERIODIC_DAMAGE))
                    l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE);

                if (l_Target->HasAuraType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT))
                    l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);

                if (l_Target->HasAura(eSpells::VampiricTouch))
                    l_Target->RemoveAura(eSpells::VampiricTouch);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_polymorph_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_polymorph_SpellScript();
        }

        class spell_mage_polymorph_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_polymorph_AuraScript);

            enum eSpells
            {
                ArcaneAssault                   = 225119,
                GlyphOfPolymorphicProportions   = 219630
            };

            void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature || l_Creature->GetCreatureType() != CREATURE_TYPE_CRITTER)
                    return;

                if (l_Caster->HasAura(eSpells::GlyphOfPolymorphicProportions))
                {
                    l_Target->ApplyPercentModFloatValue(OBJECT_FIELD_SCALE, 50, true);
                    l_Target->SetObjectScale(l_Target->GetFloatValue(OBJECT_FIELD_SCALE));
                }
            }

            void HandleEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::GlyphOfPolymorphicProportions))
                {
                    l_Target->ApplyPercentModFloatValue(OBJECT_FIELD_SCALE, -50, true);
                    l_Target->SetObjectScale(l_Target->GetFloatValue(OBJECT_FIELD_SCALE));
                }
            }

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo && l_SpellInfo->Id == eSpells::ArcaneAssault)
                    return false;

                return true;
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_mage_polymorph_AuraScript::HandleEffectApply, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_polymorph_AuraScript::HandleEffectRemove, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                DoCheckProc += AuraCheckProcFn(spell_mage_polymorph_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_polymorph_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Flamecannon - 203284
class spell_mage_flamecannon : public SpellScriptLoader
{
    public:
        spell_mage_flamecannon() : SpellScriptLoader("spell_mage_flamecannon") { }

        class spell_mage_flamecannon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_flamecannon_AuraScript);

            enum eSpells
            {
                Flamecannon         = 203284,
                FlamecannonEffect   = 203285
            };

            void HandleAfterEffectPeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Flamecannon))
                    l_Caster->CastSpell(l_Caster, eSpells::FlamecannonEffect, true);
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_mage_flamecannon_AuraScript::HandleAfterEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_flamecannon_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Time Anomaly - 210805
class spell_mage_time_anomaly : public SpellScriptLoader
{
    public:
        spell_mage_time_anomaly() : SpellScriptLoader("spell_mage_time_anomaly") { }

        class spell_mage_time_anomaly_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_time_anomaly_AuraScript);

            enum eSpells
            {
                TimeAnomaly                 = 210805,
                TimeAnomalyArcaneCharges    = 210808,
                ArcanePower                 = 12042
            };

            void HandleEffectPeriodic(AuraEffect const* /*p_aurEff*/)
            {
                Unit* l_Caster = GetCaster();
                int32 l_Chance = (GetSpellInfo())->ProcChance;
                int32 l_Duration = ((GetSpellInfo())->Effects[EFFECT_0].BasePoints)*1000;
                if (!l_Caster || !l_Caster->HasAura(eSpells::TimeAnomaly) || !l_Chance || !l_Duration)
                    return;

                if (roll_chance_i(l_Chance))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::TimeAnomalyArcaneCharges, true);

                    l_Caster->CastSpell(l_Caster, eSpells::ArcanePower, true);
                    Aura* l_Aura = l_Caster->GetAura(eSpells::ArcanePower);
                    if (l_Aura != nullptr)
                        l_Aura->SetDuration(l_Duration);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_time_anomaly_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_time_anomaly_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Frostbolt - 116 - Flurry - 44614 - Frozen Orb - 84714 - Cone of Cold - 120 - Blizzard - 190356
/// Called for Frostbite - 198120
class spell_mage_frostbite : public SpellScriptLoader
{
    public:
        spell_mage_frostbite() : SpellScriptLoader("spell_mage_frostbite") { }

        class spell_mage_frostbite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_frostbite_SpellScript);

            enum eSpells
            {
                Frostbite       = 198120,
                FrostbiteEffect = 198121
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::Frostbite))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Frostbite, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_frostbite_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_frostbite_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Frostbite - 198120
class spell_mage_frostbite_aura : public SpellScriptLoader
{
    public:
        spell_mage_frostbite_aura() : SpellScriptLoader("spell_mage_frostbite_aura") { }

        class spell_mage_frostbite_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_frostbite_aura_AuraScript);

            enum eSpells
            {
                Frostbite               = 198120,
                FrostbiteFreezeEffect   = 198121,
                Frostbolt               = 116,
                Flurry                  = 44614,
                FrozenOrb               = 84721,
                ConeOfCold              = 120,
                Blizzard                = 190356
            };

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActionTarget();
                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::Frostbolt:
                    case eSpells::Flurry:
                    case eSpells::FrozenOrb:
                    case eSpells::ConeOfCold:
                    case eSpells::Blizzard:
                        l_Caster->CastSpell(l_Target, eSpells::FrostbiteFreezeEffect, true);
                        break;
                    default:
                        return;
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mage_frostbite_aura_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_frostbite_aura_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Charged Up - 205032
class spell_mage_charged_up : public SpellScriptLoader
{
    public:
        spell_mage_charged_up() : SpellScriptLoader("spell_mage_charged_up") { }

        class spell_mage_charged_up_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_charged_up_SpellScript);

            enum eSpells
            {
                ArcaneCharge = 36032
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (int i = 0; i < 4; i = i + 1)
                    l_Caster->CastSpell(l_Caster, eSpells::ArcaneCharge, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_charged_up_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_charged_up_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Cinderstorm - 198928
class spell_mage_cinderstorm : public SpellScriptLoader
{
    public:
        spell_mage_cinderstorm() : SpellScriptLoader("spell_mage_cinderstorm") { }

        class spell_mage_cinderstorm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_cinderstorm_SpellScript);

            enum eSpells
            {
                Ignite      = 12654,
                Cinderstorm = 198929
            };

            void HandleOnHit()
            {
                Unit *l_Caster = GetCaster();
                Unit *l_Target = GetHitUnit();

                if (!l_Caster || l_Caster->HasAura(eSpells::Cinderstorm))
                    return;
                if (l_Target && l_Target->HasAura(eSpells::Ignite, l_Caster->GetGUID()))
                    SetHitDamage(GetHitDamage() * 1.3f);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_cinderstorm_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_cinderstorm_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blazing Barrier - 235313, Prismatic Barrier - 235450, Ice Barrier - 11426
class spell_mage_blazing_prismatic_barrier : public SpellScriptLoader
{
    public:
        spell_mage_blazing_prismatic_barrier() : SpellScriptLoader("spell_mage_blazing_prismatic_barrier") { }

        enum eSpells
        {
            BlazingBarrier  = 235313,
            MoltenSkin      = 194315,
            ManaShield      = 235463,
            IceBarrier      = 11426,
            ForceBarrier    = 210716,
            ShieldOfAdoli   = 195354,
            ShieldOfAlodiId = 791
        };

        enum eArtifactPowerIds
        {
            MoltenSkinArtifact = 757
        };

        class spell_mage_blazing_prismatic_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_blazing_prismatic_barrier_AuraScript);

            void CalcAmountAbsorb(AuraEffect const* p_AurEff, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;

                if (!l_Caster || !(l_Player = l_Caster->GetAffectingPlayer()))
                    return;

                int32 l_SpellPower = l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                float l_Versatility = l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f;

                p_Amount = l_SpellPower * (m_scriptSpellId == eSpells::IceBarrier ? 10 : 7) * (1 + l_Versatility);

                if (m_scriptSpellId == eSpells::IceBarrier)
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShieldOfAdoli))
                        AddPct(p_Amount, l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eSpells::ShieldOfAlodiId));
                }

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ForceBarrier, EFFECT_0))
                    AddPct(p_Amount, l_AuraEffect->GetAmount());
            }

            void HandleAbsorb(AuraEffect*, DamageInfo &p_DmgInfo, uint32& p_Amount)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (m_scriptSpellId == eSpells::BlazingBarrier && l_Caster->ToPlayer() &&
                    l_Caster->HasAura(eSpells::MoltenSkin) && p_DmgInfo.GetSchoolMask() == SPELL_SCHOOL_MASK_NORMAL)
                {
                    uint32 l_Damage = p_DmgInfo.GetAmount();
                    if (MS::Artifact::Manager* l_Manager = l_Caster->ToPlayer()->GetCurrentlyEquippedArtifact())
                    {
                        if (ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowerIds::MoltenSkinArtifact,
                            l_Manager->GetCurrentRankWithBonus(eArtifactPowerIds::MoltenSkinArtifact) - 1))
                        {
                            if (l_RankEntry->AuraPointsOverride < 0)
                                AddPct(l_Damage, l_RankEntry->AuraPointsOverride);

                            p_DmgInfo.SetAmount(l_Damage);
                        }
                    }
                }

                if (AuraEffect *l_ManaShieldEffect = l_Caster->GetAuraEffect(eSpells::ManaShield, EFFECT_0))
                {
                    int32 l_Absorbed = (p_DmgInfo.GetAmount() > p_Amount) ? p_Amount : p_DmgInfo.GetAmount();
                    int32 l_DrainedMana = CalculatePct(l_Absorbed, l_ManaShieldEffect->GetAmount());

                    l_Caster->ModifyPower(POWER_MANA, -l_DrainedMana);
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_blazing_prismatic_barrier_AuraScript::HandleAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_blazing_prismatic_barrier_AuraScript::CalcAmountAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_blazing_prismatic_barrier_AuraScript();
        }

        class spell_mage_blazing_prismatic_barrier_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_blazing_prismatic_barrier_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;

                if (l_Caster && (l_Player = l_Caster->ToPlayer()))
                {
                    if (l_Caster->HasAura(eSpells::ManaShield))
                        l_Player->RemoveSpellCooldown(m_scriptSpellId);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_blazing_prismatic_barrier_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_blazing_prismatic_barrier_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Called by Arcane Missiles  - 5143
class spell_mage_rule_of_threes : public SpellScriptLoader
{
    public:
        spell_mage_rule_of_threes() : SpellScriptLoader("spell_mage_rule_of_threes") { }

        class spell_mage_rule_of_threes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_rule_of_threes_SpellScript);

            enum eSpells
            {
                RuleOfThrees = 215463,
                RuleOfThreesProc = 187292
            };

            void HandleBeforeCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (AuraEffect const* l_AurEff = l_Caster->GetAuraEffect(eSpells::RuleOfThrees, EFFECT_0))
                    {
                        if (roll_chance_f(float(l_AurEff->GetAmount()) / 10.0f))
                            l_Caster->CastSpell(l_Caster, eSpells::RuleOfThreesProc, true);
                    }
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_mage_rule_of_threes_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_rule_of_threes_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Ebonbolt  - 214634
class spell_mage_ebonbolt : public SpellScriptLoader
{
    public:
        spell_mage_ebonbolt() : SpellScriptLoader("spell_mage_ebonbolt") { }

        class spell_mage_ebonbolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_ebonbolt_SpellScript);

            enum eSpells
            {
                BrainFreeze             = 190446,
                GlacialEruptionAura     = 238128,
                GlacialEruption         = 242851,
                T20FrostMage4P          = 242254,
                FrozenOrb               = 84714
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Caster->HasAura(eSpells::BrainFreeze))
                    l_Caster->DelayedCastSpell(l_Caster, eSpells::BrainFreeze, true, 300);
                else
                {
                    l_Caster->CastSpell(l_Caster, eSpells::BrainFreeze, true);

                    if (l_Player->HasAura(eSpells::T20FrostMage4P))
                        if (SpellInfo const* l_T20Frost4PInfo = sSpellMgr->GetSpellInfo(eSpells::T20FrostMage4P))
                            if (l_Player->HasSpellCooldown(eSpells::FrozenOrb))
                                l_Player->ReduceSpellCooldown(eSpells::FrozenOrb, l_T20Frost4PInfo->Effects[EFFECT_0].BasePoints * 100);
                }
            }

            void HandleOnHit()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::GlacialEruptionAura))
                    l_Caster->DelayedCastSpell(l_Target, eSpells::GlacialEruption, true, 1000);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_ebonbolt_SpellScript::HandleOnCast);
                OnHit += SpellHitFn(spell_mage_ebonbolt_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_ebonbolt_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Icy Veins - 12472 - Ice Form - 198144
class spell_mage_chilled_to_the_core : public SpellScriptLoader
{
    public:
        spell_mage_chilled_to_the_core() : SpellScriptLoader("spell_mage_chilled_to_the_core") { }

        enum eSpells
        {
            ChilledToTheCoreAura    = 195448,
            ChilledToTheCoreProc    = 195446,
            IceForm                 = 198144,
            IcyVeins                = 12472
        };

        class spell_mage_chilled_to_the_core_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_chilled_to_the_core_AuraScript);

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::ChilledToTheCoreAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ChilledToTheCoreProc, true);

                Aura* l_ChilledToTheCore    = l_Caster->GetAura(eSpells::ChilledToTheCoreProc);
                Aura* l_IceForm             = l_Caster->GetAura(eSpells::IceForm);
                if (!l_ChilledToTheCore || !l_IceForm)
                    return;

                l_ChilledToTheCore->SetDuration(l_IceForm->GetDuration());
                l_ChilledToTheCore->SetMaxDuration(l_IceForm->GetMaxDuration());
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->HasAura(eSpells::ChilledToTheCoreAura))
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::ChilledToTheCoreProc);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::IcyVeins:
                        AfterEffectApply  += AuraEffectApplyFn(spell_mage_chilled_to_the_core_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_chilled_to_the_core_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
                        break;
                    case eSpells::IceForm:
                        AfterEffectApply += AuraEffectApplyFn(spell_mage_chilled_to_the_core_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_chilled_to_the_core_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_chilled_to_the_core_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Touch of the Magi - 210824
class spell_mage_touch_of_the_magi : public SpellScriptLoader
{
    public:
        spell_mage_touch_of_the_magi() : SpellScriptLoader("spell_mage_touch_of_the_magi") { }

        class spell_mage_touch_of_the_magi_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_touch_of_the_magi_AuraScript);

            enum eSpells
            {
                TouchOfTheMagiAura  = 210725,
                TouchOfTheMagiDmg   = 210833
            };

            int32 m_Damage = 0;

            void HandleProc(ProcEventInfo &p_Info)
            {
                if (p_Info.GetDamageInfo())
                {
                    if (p_Info.GetActor() == GetCaster())
                        m_Damage += p_Info.GetDamageInfo()->GetAmount();
                }
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (!l_Caster || !l_Target)
                    return;
                SpellInfo const* l_Info = sSpellMgr->GetSpellInfo(eSpells::TouchOfTheMagiAura);

                if (!l_Info)
                    return;
                m_Damage = CalculatePct(m_Damage, l_Info->Effects[0].BasePoints);
                l_Caster->CastCustomSpell(l_Target, eSpells::TouchOfTheMagiDmg, &m_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_mage_touch_of_the_magi_AuraScript::HandleProc);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_touch_of_the_magi_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_touch_of_the_magi_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Overpowered - 155147
class spell_mage_overpowered : public SpellScriptLoader
{
    public:
        spell_mage_overpowered() : SpellScriptLoader("spell_mage_overpowered") { }

        class spell_mage_overpowered_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_overpowered_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.65f;   ///< Overpowered effect is reduced by 35% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_overpowered_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_overpowered_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_overpowered_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blizzard  - 190357
class spell_mage_blizzard_hit : public SpellScriptLoader
{
    public:
        spell_mage_blizzard_hit() : SpellScriptLoader("spell_mage_blizzard_hit") { }

        class spell_mage_blizzard_hit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_blizzard_hit_SpellScript);

            enum eSpells
            {
                FrozenOrb           = 84714,
                FrozenOrbOverrided  = 198149,
                ZannesuJourney      = 226852
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (l_Player->HasSpellCooldown(eSpells::FrozenOrb))
                        l_Player->ReduceSpellCooldown(eSpells::FrozenOrb, 500);
                    else if (l_Player->HasSpellCooldown(eSpells::FrozenOrbOverrided))
                        l_Player->ReduceSpellCooldown(eSpells::FrozenOrbOverrided, 500);
                }
            }

            void HandleOnEffectHitTarget(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Damage = GetHitDamage();
                AddPct(l_Damage, l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ZannesuJourney));
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_blizzard_hit_SpellScript::HandleOnHit);
                OnEffectHitTarget += SpellEffectFn(spell_mage_blizzard_hit_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_blizzard_hit_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blizzard  - 190356
class spell_mage_zannesu_journey : public SpellScriptLoader
{
    public:
        spell_mage_zannesu_journey() : SpellScriptLoader("spell_mage_zannesu_journey") { }

        class spell_mage_zannesu_journey_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_zannesu_journey_SpellScript);

            enum eSpells
            {
                ZannesuJourney      = 226852
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ZannesuJourney, 0);
                if (!l_AuraEffect)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ZannesuJourney) = l_AuraEffect->GetAmount();

                l_Caster->RemoveAurasDueToSpell(eSpells::ZannesuJourney);

                uint64 l_CasterGuid = l_Caster->GetGUID();
                l_Caster->m_Functions.AddFunction([=]() -> void
                {
                    Unit* l_Caster = sObjectAccessor->FindUnit(l_CasterGuid);
                    if (!l_Caster)
                        return;

                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ZannesuJourney) = 0;
                }, l_Caster->m_Functions.CalculateTime(l_SpellInfo->GetDuration()));
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_zannesu_journey_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_zannesu_journey_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Cold Snap - 235219
class spell_mage_cold_snap : public SpellScriptLoader
{
    public:
        spell_mage_cold_snap() : SpellScriptLoader("spell_mage_cold_snap") { }

        class spell_mage_cold_snap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_cold_snap_SpellScript);

            enum eSpells
            {
                FrostNova       = 122,
                ConeOfCold      = 120,
                IceBarrier      = 11426,
                IceBlock        = 45438
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player *l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                SpellInfo const* l_SpellInfoFrostNova = sSpellMgr->GetSpellInfo(eSpells::FrostNova);
                SpellInfo const* l_SpellInfoIceBlock = sSpellMgr->GetSpellInfo(eSpells::IceBlock);
                if (!l_SpellInfoFrostNova || !l_SpellInfoIceBlock)
                    return;

                l_Player->RestoreCharge(l_SpellInfoFrostNova->ChargeCategoryEntry); // RemoveSpellCooldown not working here
                l_Player->RemoveSpellCooldown(eSpells::ConeOfCold, true);
                l_Player->RemoveSpellCooldown(eSpells::IceBarrier, true);
                l_Player->RemoveSpellCooldown(eSpells::IceBlock, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_mage_cold_snap_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_cold_snap_SpellScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Erosion - 210134
class spell_mage_erosion : public SpellScriptLoader
{
    public:
        spell_mage_erosion() : SpellScriptLoader("spell_mage_erosion") { }

        class spell_mage_erosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_erosion_SpellScript);

            enum  eSpells
            {
                ErosionTimer = 210154
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ErosionTimer, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_erosion_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_erosion_SpellScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Erosion (Timer) - 210154
class spell_mage_erosion_timer : public SpellScriptLoader
{
    public:
        spell_mage_erosion_timer() : SpellScriptLoader("spell_mage_erosion_timer") { }

        class spell_mage_erosion_timer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_erosion_timer_AuraScript);

            enum eSpells
            {
                ErosionDebuff = 210134
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target || p_AuraEffect->GetAmount())
                    return;

                int32 l_Bp = 1;

                l_Caster->CastCustomSpell(l_Target, m_scriptSpellId, &l_Bp, nullptr, nullptr, true);
            }

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target || !p_AuraEffect->GetAmount())
                    return;

                Aura* l_Aura = l_Target->GetAura(eSpells::ErosionDebuff, l_Caster->GetGUID());
                if (!l_Aura)
                    return;

                l_Aura->DropStack();

                Aura* l_Periodic = GetAura();
                if (!l_Periodic)
                    return;

                l_Periodic->RefreshDuration();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_erosion_timer_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_mage_erosion_timer_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_erosion_timer_AuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Called by Frozen Veins 195345
class spell_mage_frozen_veins : public SpellScriptLoader
{
    public:
        spell_mage_frozen_veins() : SpellScriptLoader("spell_mage_frozen_veins") { }

        class spell_mage_frozen_veins_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_frozen_veins_AuraScript);

            enum eSpells
            {
                Frostbolt   = 228597,
                IcyVeins    = 12472
            };

            enum eTraits
            {
                FrozenVeinsTrait = 788
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Frostbolt)
                    return false;

                if (p_EventInfo.GetHitMask() & PROC_HIT_CRITICAL)
                    return true;

                return false;
            }

            void HandleProc(ProcEventInfo& /* p_EventInfo */)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                AuraEffect const * l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                l_Player->ReduceSpellCooldown(eSpells::IcyVeins, -l_AuraEffect->GetAmount());
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_frozen_veins_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_mage_frozen_veins_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_frozen_veins_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Jouster - 214626
class spell_mage_jouster : public SpellScriptLoader
{
    public:
        spell_mage_jouster() : SpellScriptLoader("spell_mage_jouster") { }

        class spell_mage_jouster_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_jouster_AuraScript);

            enum eSpells
            {
                Jouster     = 195391,
                IceLance    = 228598
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::IceLance)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Aura* l_Aura = GetAura();
                Unit* l_Caster = GetCaster();
                if (!l_Aura || !l_Caster)
                    return;

                AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0);
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Effect || !l_Player)
                    return;

                int32 l_Amount = l_Effect->GetAmount();
                l_Caster->CastCustomSpell(l_Caster, eSpells::Jouster, &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_jouster_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_mage_jouster_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_jouster_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by AfterShocks - 194431
class spell_mage_aftershock : public SpellScriptLoader
{
    public:
        spell_mage_aftershock() : SpellScriptLoader("spell_mage_aftershock") { }

        class spell_mage_aftershock_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_aftershock_AuraScript);

            enum eSpells
            {
                AfterShock     = 194432,
                Flamestrike    = 2120
            };

            Guid128 m_LastProcSpellGuid = Guid128();

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Flamestrike)
                    return false;

                if (!l_Spell || l_Spell->GetCastGuid() == m_LastProcSpellGuid)
                    return false;

                m_LastProcSpellGuid = l_Spell->GetCastGuid();
                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AfterShock, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_aftershock_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_mage_aftershock_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_aftershock_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Sloooow Down - 210730
class spell_mage_sloooow_down : public SpellScriptLoader
{
    public:
        spell_mage_sloooow_down() : SpellScriptLoader("spell_mage_sloooow_down") { }

        class spell_mage_sloooow_down_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_sloooow_down_AuraScript);

            enum eSpells
            {
                Slow = 31589
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Slow)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_sloooow_down_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_sloooow_down_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Arcane Familiar - 210126
class spell_mage_arcane_familiar : public SpellScriptLoader
{
    public:
        spell_mage_arcane_familiar() : SpellScriptLoader("spell_mage_arcane_familiar") { }

        class spell_mage_arcane_familiar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_arcane_familiar_AuraScript);

            enum eSpells
            {
                ArcaneAssault = 225119
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_Target || !l_DamageInfo)
                    return;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Spell || !l_Player)
                    return;

                l_Player->m_SpellHelper.GetUint64(eSpellHelpers::ArcaneAssault) = l_Target->GetGUID();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_mage_arcane_familiar_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_arcane_familiar_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Conflagration - 226757, Ignite - 12654
class spell_mage_conflagration : public SpellScriptLoader
{
    public:
        spell_mage_conflagration() : SpellScriptLoader("spell_mage_conflagration") { }

        class spell_mage_conflagration_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_conflagration_AuraScript);

            enum eSpells
            {
                ConflagrationDebuff     = 226757,
                Conflagration           = 205023,
                FlareUp                 = 205345
            };

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes p_Mode)
            {
                SpellInfo const* l_ConflagrationInfo = sSpellMgr->GetSpellInfo(eSpells::Conflagration);
                SpellInfo const* l_ConflagrationDebuffInfo = sSpellMgr->GetSpellInfo(eSpells::ConflagrationDebuff);
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::Conflagration))
                    return;

                if (p_Mode == AURA_EFFECT_HANDLE_REAPPLY)
                    if (Aura* l_Aura = l_Target->GetAura(ConflagrationDebuff))
                        l_Aura->SetDuration(l_ConflagrationDebuffInfo->GetDuration());
            }

            void OnTick(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Conflagration, EFFECT_0))
                {
                    if (roll_chance_i(l_AuraEffect->GetAmount()))
                        l_Caster->CastSpell(l_Target, eSpells::FlareUp, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_conflagration_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                if (m_scriptSpellId == eSpells::ConflagrationDebuff)
                    AfterEffectApply += AuraEffectApplyFn(spell_mage_conflagration_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_conflagration_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called for Arcane Familiar - 210126
class PlayerScript_arcane_familiar : public PlayerScript
{
    public:
        PlayerScript_arcane_familiar() : PlayerScript("PlayerScript_arcane_familiar") { }

        enum eSpells
        {
            ArcaneFamiliar      = 205022,
            ArcaneFamiliarBuff  = 210126,
            ArcaneAssaultDamage = 225119
        };

        void OnSpellRemoved(Player* p_Player, uint32 p_SpellID)
        {
            if (p_SpellID == eSpells::ArcaneFamiliar)
                p_Player->RemoveAura(eSpells::ArcaneFamiliarBuff);
        }

        void OnUpdate(Player* p_Player, uint32 /* p_Diff */)
        {
            if (!p_Player->HasAura(eSpells::ArcaneFamiliarBuff) || p_Player->HasSpellCooldown(eSpells::ArcaneAssaultDamage) || !p_Player->isInCombat())
                return;

            if (Unit* l_Target = sObjectAccessor->GetUnit(*p_Player, p_Player->m_SpellHelper.GetUint64(eSpellHelpers::ArcaneAssault)))
            {
                if (l_Target->HasBreakableByDamageCrowdControlAura() || l_Target->HasInvisibilityAura() || !l_Target->isInCombat())
                    return;

                if (l_Target->GetDistance(p_Player) > 40.0f)
                    return;

                if (p_Player->HasInvisibilityAura() || p_Player->HasStealthAura())
                    return;

                p_Player->CastSpell(l_Target, eSpells::ArcaneAssaultDamage, true);
                p_Player->AddSpellCooldown(eSpells::ArcaneAssaultDamage, 0, 3 * IN_MILLISECONDS, false);
            }
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Frenetic Speed - 236058
class spell_mage_frenetic_speed : public SpellScriptLoader
{
    public:
        spell_mage_frenetic_speed() : SpellScriptLoader("spell_mage_frenetic_speed") { }

        class spell_mage_frenetic_speed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_frenetic_speed_AuraScript);

            enum eSpells
            {
                Scorch = 2948
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Scorch)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_frenetic_speed_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_frenetic_speed_AuraScript();
        }
};

class PlayerScript_Shimmer : public PlayerScript
{
    public:
        PlayerScript_Shimmer() : PlayerScript("PlayerScript_Shimmer") {}

        enum eSpells
        {
            Shimmer               = 212653,
            DisplacementShimmer   = 191738
        };

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID)
        {
            if (!p_Player || p_Player->GetActiveSpecializationID() != SPEC_MAGE_ARCANE || p_SpellID != eSpells::Shimmer)
                return;

            p_Player->CastSpell(p_Player, eSpells::DisplacementShimmer, true);
        }

         void OnSpellRemoved(Player* p_Player, uint32 p_SpellID)
        {
            if (!p_Player || p_Player->GetActiveSpecializationID() != SPEC_MAGE_ARCANE || p_SpellID != eSpells::Shimmer)
                return;

            p_Player->RemoveAura(eSpells::DisplacementShimmer, false, false);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Summon Water Elemental- 31687
class spell_mage_summon_water_elemental : public SpellScriptLoader
{
    public:
        spell_mage_summon_water_elemental() : SpellScriptLoader("spell_mage_summon_water_elemental") { }

        class spell_mage_summon_water_elemental_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_summon_water_elemental_SpellScript);

            enum eSpells
            {
                LonelyWinter                    = 205024,
                GlyphOfTheUnboundElemental      = 146976
            };

            enum eUnboundWaterElemental
            {
                UnboundWaterElementalTransform = 147358
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                uint64 l_CasterGuid = l_Caster->ToPlayer()->GetGUID();
                if (l_Caster->HasAura(eSpells::LonelyWinter))
                {
                    l_Caster->AddDelayedEvent([l_CasterGuid]() -> void
                    {
                        Player* l_Caster = sObjectAccessor->FindPlayer(l_CasterGuid);
                        if (!l_Caster)
                            return;

                        Pet* l_Pet = l_Caster->GetPet();
                        if (!l_Pet)
                            return;

                        l_Pet->Remove(PET_SLOT_OTHER_PET);
                    }, 2 * IN_MILLISECONDS);
                }

                l_Caster->AddDelayedEvent([l_CasterGuid]() -> void
                {
                    Player* l_Caster = sObjectAccessor->FindPlayer(l_CasterGuid);
                    if (!l_Caster)
                        return;

                    Pet* l_Pet = l_Caster->GetPet();
                    if (!l_Pet)
                        return;

                    if (!l_Caster->HasAura(eSpells::GlyphOfTheUnboundElemental))
                        l_Pet->RemoveAurasDueToSpell(eUnboundWaterElemental::UnboundWaterElementalTransform);
                }, 2 * IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_summon_water_elemental_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_summon_water_elemental_SpellScript();
        }
};

/// Update to Legion 7.1.5 build: 23420
/// Called By Burst of Cold 206431
class spell_mage_burst_of_cold : public SpellScriptLoader
{
    public:
        spell_mage_burst_of_cold() : SpellScriptLoader("spell_mage_burst_of_cold") { }

        class spell_mage_burst_of_cold_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_burst_of_cold_AuraScript);

            enum eSpells
            {
                FrostNova   = 122,
                ConeOfCold  = 120
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEvent)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEvent.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const * l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::FrostNova)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /* p_ProcEvent */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (l_SpellInfo)
                    l_Player->ReduceSpellCooldown(eSpells::ConeOfCold, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_burst_of_cold_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_mage_burst_of_cold_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_burst_of_cold_AuraScript();
        }
};

/// Update to Legion 7.1.5 build: 23420
/// Called By Dragon Breath 31661
/// Called For Alexstrasza's Fury - 235870
class spell_mage_dragon_breath : public SpellScriptLoader
{
    public:
        spell_mage_dragon_breath() : SpellScriptLoader("spell_mage_dragon_breath") { }

        class spell_mage_dragon_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_dragon_breath_SpellScript);

            enum eSpells
            {
                AlexstraszaFury = 235870,
                EatingUp        = 48107,
                HotStreak       = 48108
            };

            Guid128 m_CastGuid;

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Spell* l_Spell = GetSpell();
                if (!l_Caster || !l_Spell || !l_Caster->HasAura(eSpells::AlexstraszaFury))
                    return;

                if (m_CastGuid == l_Spell->GetCastGuid())
                    return;

                m_CastGuid = l_Spell->GetCastGuid();
                l_Caster->CastSpell(l_Caster, !l_Caster->HasAura(eSpells::EatingUp) && !l_Caster->HasAura(eSpells::HotStreak) ? eSpells::EatingUp : eSpells::HotStreak, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_dragon_breath_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_dragon_breath_SpellScript();
        }
};

/// Update to Legion 7.1.5 build: 23420
/// Called By Ice Barrier 11426
class spell_mage_ice_barrier : public SpellScriptLoader
{
    public:
        spell_mage_ice_barrier() : SpellScriptLoader("spell_mage_ice_barrier") { }

        class spell_mage_ice_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_ice_barrier_AuraScript);

            enum eSpells
            {
                IceBarrierSlow = 205708,
            };

            Guid128 m_CastGuid;

            void HandleProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetActor();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::IceBarrierSlow, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_mage_ice_barrier_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_ice_barrier_AuraScript();
        }
};

class PlayerScript_Mass_Invisibility : public PlayerScript
{
    public:
        PlayerScript_Mass_Invisibility() : PlayerScript("PlayerScript_Mass_Invisibility") { }

        enum eSpells
        {
            MassInvisibility        = 198158,
            RingOfFrost             = 113724,
            RingOfFrostTriggered    = 136511,
            Blink                   = 1953,
            Shimmer                 = 212653,
            DisplacementBeacon      = 212799,
            Displacement            = 195676,
            ShimmerDisplacement     = 212801,
            IceBlock                = 45438,
            PrismaticBarrier        = 235450,
            ErosionBuff             = 210134,
            ErosionDummy            = 210154,
            LifeSteal               = 146347,
            MarkofAlunethDOT        = 211088,
            NetherTempestDOT        = 114954
        };

        void OnSpellFinished(Player* p_Player, Spell* p_Spell, bool ok)
        {
            if (!ok)
                return;

            SpellInfo const* l_SpellInfo = p_Spell->GetSpellInfo();
            if (!l_SpellInfo)
                return;

            switch (l_SpellInfo->Id)
            {
                case eSpells::MassInvisibility:
                case eSpells::RingOfFrost:
                case eSpells::Blink:
                case eSpells::Shimmer:
                case eSpells::Displacement:
                case eSpells::IceBlock:
                case eSpells::PrismaticBarrier:
                case eSpells::RingOfFrostTriggered:
                case eSpells::DisplacementBeacon:
                case eSpells::ShimmerDisplacement:
                case eSpells::ErosionBuff:
                case eSpells::ErosionDummy:
                case eSpells::LifeSteal:
                case eSpells::MarkofAlunethDOT:
                case eSpells::NetherTempestDOT:
                    return;
                default:
                    p_Player->RemoveAura(eSpells::MassInvisibility);
            }
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Scorched Earth - 227481
class spell_mage_scorched_earth : public SpellScriptLoader
{
public:
    spell_mage_scorched_earth() : SpellScriptLoader("spell_mage_scorched_earth") { }

    class spell_mage_scorched_earth_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_scorched_earth_AuraScript);

        enum eSpells
        {
            Scorch = 2948,
            ScorchedEarth = 227482
        };

        bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::Scorch)
                return false;

            return true;
        }

        void HandleOnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& /*p_EventInfo*/)
        {
            if (Unit* l_Caster = GetCaster())
                l_Caster->CastSpell(l_Caster, eSpells::ScorchedEarth, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_mage_scorched_earth_AuraScript::HandleDoCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_mage_scorched_earth_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_scorched_earth_AuraScript();
    }
};

/// Last update 7.1.5 Build 23420
/// Called by Arcane Missiles - 5143
/// Called For Item - Mage T19 Arcane 4P Bonus - 211386
class spell_mage_item_t19_arcane_4p_bonus : public SpellScriptLoader
{
    public:
        spell_mage_item_t19_arcane_4p_bonus() : SpellScriptLoader("spell_mage_item_t19_arcane_4p_bonus") { }

        class spell_mage_item_t19_arcane_4p_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_item_t19_arcane_4p_bonus_SpellScript);

            enum eSpells
            {
                Evocation = 12051,
                T194PBonus = 211386
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T194PBonus, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                if (SpellInfo const* l_Evocation = sSpellMgr->GetSpellInfo(eSpells::Evocation))
                    l_Player->ReduceChargeCooldown(l_Evocation->ChargeCategoryEntry, l_AuraEffect->GetAmount() * IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mage_item_t19_arcane_4p_bonus_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_item_t19_arcane_4p_bonus_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Chilled By Frost Nova (122) and Freeze (33395)
class spell_mage_chilled_to_the_bone : public SpellScriptLoader
{
    public:
        spell_mage_chilled_to_the_bone() : SpellScriptLoader("spell_mage_chilled_to_the_bone") { }

        class spell_mage_chilled_to_the_bone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_chilled_to_the_bone_AuraScript);

            enum eSpells
            {
                ChilledToTheBoneTalent       = 198126,
                ChilledToTheBoneDamage       = 198127,
                ChilledToTheBoneDispelDamage = 198139,
                FrostNova                    = 122,
                Freeze                       = 33395
            };

            void OnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->ToPet())
                    l_Caster = l_Caster->ToPet()->GetOwner();

                if (!l_Caster->HasAura(eSpells::ChilledToTheBoneTalent))
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    l_Caster->CastSpell(l_Target, eSpells::ChilledToTheBoneDamage, true);
                else if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_ENEMY_SPELL)
                    l_Caster->CastSpell(l_Target, eSpells::ChilledToTheBoneDispelDamage, true);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Freeze)
                    AfterEffectRemove += AuraEffectRemoveFn(spell_mage_chilled_to_the_bone_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_ROOT_2, AURA_EFFECT_HANDLE_REAL);
                else
                    AfterEffectRemove += AuraEffectRemoveFn(spell_mage_chilled_to_the_bone_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_ROOT_2, AURA_EFFECT_HANDLE_REAL);
           }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_chilled_to_the_bone_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Fireball - 133
/// For Firestarter - 203283
class spell_mage_firestarter_2 : public SpellScriptLoader
{
    public:
        spell_mage_firestarter_2() : SpellScriptLoader("spell_mage_firestarter_2") { }

        class spell_mage_firestarter_2_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_firestarter_2_AuraScript);

            enum eSpells
            {
                Fireball    = 133,
                Combustion  = 190319
            };

            bool Load() override
            {
                m_ReduceTime = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].BasePoints * IN_MILLISECONDS;
                return true;
            }

            /// Cannot use spell_proc_event, the spellfamilymask is not unique
            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (p_EventInfo.GetDamageInfo() == nullptr)
                    return false;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() == nullptr)
                    return false;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::Fireball)
                    return false;

                return true;
            }

            void HandleOnEffectProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Owner = GetUnitOwner();
                if (l_Owner == nullptr || !l_Owner->IsPlayer())
                    return;

                l_Owner->ToPlayer()->ReduceSpellCooldown(eSpells::Combustion, m_ReduceTime);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_firestarter_2_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_mage_firestarter_2_AuraScript::HandleOnEffectProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
            }

            int32 m_ReduceTime = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_firestarter_2_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Flare Up - 203282
class spell_mage_flare_up : public SpellScriptLoader
{
    public:
        spell_mage_flare_up() : SpellScriptLoader("spell_mage_flare_up") { }

        class spell_mage_flare_up_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_flare_up_AuraScript);

            enum eSpells
            {
                FireBlast   = 108853,
                Ignite      = 12654
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (p_EventInfo.GetDamageInfo() == nullptr ||
                    p_EventInfo.GetDamageInfo()->GetSpellInfo() == nullptr)
                    return false;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::FireBlast)
                    return false;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActionTarget();
                if (l_Caster == nullptr || l_Target == nullptr || l_Target->HasAura(eSpells::Ignite, l_Caster->GetGUID()))
                    return false;

                return true;
            }

            void HandleOnEffectProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                PreventDefaultAction();

                Unit* l_Owner = GetUnitOwner();
                if (l_Owner == nullptr || !l_Owner->IsPlayer())
                    return;

                l_Owner->AddDelayedEvent([l_Owner]() -> void
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FireBlast))
                        l_Owner->ToPlayer()->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
                }, 1);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_flare_up_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_mage_flare_up_AuraScript::HandleOnEffectProc, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_flare_up_AuraScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Legendary: Soul of the Archmage - 247553
class spell_mage_soul_of_the_archmage : public SpellScriptLoader
{
    public:
        spell_mage_soul_of_the_archmage() : SpellScriptLoader("spell_mage_soul_of_the_archmage") { }

        class spell_mage_soul_of_the_archmage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_soul_of_the_archmage_AuraScript);

            enum eSpells
            {
                TemporalFlux         = 234302,
                FlameOn              = 205029,
                FrozenTouch          = 205030
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
                case SPEC_MAGE_ARCANE:
                    l_Player->CastSpell(l_Player, eSpells::TemporalFlux, true);
                    break;
                case SPEC_MAGE_FIRE:
                    l_Player->CastSpell(l_Player, eSpells::FlameOn, true);
                    break;
                default:
                    l_Player->CastSpell(l_Player, eSpells::FrozenTouch, true);
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
                case SPEC_MAGE_ARCANE:
                    l_Player->RemoveAura(eSpells::TemporalFlux);
                    break;
                case SPEC_MAGE_FIRE:
                    l_Player->RemoveAura(eSpells::FlameOn);
                    break;
                default:
                    l_Player->RemoveAura(eSpells::FrozenTouch);
                    break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_soul_of_the_archmage_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_soul_of_the_archmage_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_soul_of_the_archmage_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Warmth of the Phoenix - 240671
class spell_mage_warmth_of_the_phoenix_triggered : public SpellScriptLoader
{
public:
    spell_mage_warmth_of_the_phoenix_triggered() : SpellScriptLoader("spell_mage_warmth_of_the_phoenix_triggered") { }

    enum eSpells
    {
        WarmthOfThePhoenixAura = 238091
    };

    class spell_mage_warmth_of_the_phoenix_triggered_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_warmth_of_the_phoenix_triggered_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WarmthOfThePhoenixAura);
            if (!l_SpellInfo)
                return;

            uint8 l_MaxTargets = l_SpellInfo->Effects[EFFECT_1].BasePoints + 1; /// + 1 caster

            if (p_Targets.size() > l_MaxTargets)
                p_Targets.resize(l_MaxTargets);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_warmth_of_the_phoenix_triggered_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ALLY_OR_RAID);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_warmth_of_the_phoenix_triggered_SpellScript();
    }
};

/// Last Update 7.3.2 build 25549
/// Called by Frigid Winds - 235224
class spell_mage_frigid_winds : public SpellScriptLoader
{
    public:
        spell_mage_frigid_winds() : SpellScriptLoader("spell_mage_frigid_winds") { }

        class spell_mage_frigid_winds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_frigid_winds_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.66666f;   ///< Frigid Winds is reduced by 33,334% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_frigid_winds_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_frigid_winds_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Chilled - 205708
class spell_mage_chilled_speed : public SpellScriptLoader
{
    public:
        spell_mage_chilled_speed() : SpellScriptLoader("spell_mage_chilled_speed") { }

        class spell_mage_chilled_speed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_chilled_speed_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.8f;   ///< Chains of Ice is reduced by 20% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_chilled_speed_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_chilled_speed_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Chilled to the Core - 195446
class spell_mage_chilled_to_the_core_bonus : public SpellScriptLoader
{
    public:
        spell_mage_chilled_to_the_core_bonus() : SpellScriptLoader("spell_mage_chilled_to_the_core_bonus") { }

        class spell_mage_chilled_to_the_core_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_chilled_to_the_core_bonus_AuraScript);

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

                p_Amount *= 0.5f;   ///< Chilled to the Core bonus damage is reduced by 50% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_chilled_to_the_core_bonus_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_chilled_to_the_core_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 23911
/// Torment of Weak - 240692
class spell_mage_torment_of_weak : public SpellScriptLoader
{
    public:
        spell_mage_torment_of_weak() : SpellScriptLoader("spell_mage_torment_of_weak")
        {}

        class spell_mage_torment_of_weak_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_torment_of_weak_AuraScript);

            bool HandleCheckProc(ProcEventInfo & p_Proc)
            {
                if (p_Proc.GetActionTarget() == nullptr ||
                    p_Proc.GetDamageInfo() == nullptr)
                    return false;

                Unit* l_Target = p_Proc.GetActionTarget();

                bool l_Result = l_Target->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED);

                if (l_Result)
                {
                    DamageInfo* l_DmgInfo = p_Proc.GetDamageInfo();

                    uint32 l_Dmg = l_DmgInfo->GetAmount();

                    AddPct(l_Dmg, GetEffect(EFFECT_0)->GetAmount());

                    l_DmgInfo->SetAmount(l_Dmg);
                }

                return l_Result;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_torment_of_weak_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_torment_of_weak_AuraScript();
        }
};

/// Called by All Mage Portals - 49362, 49363, 33728, 35718, 53141, 132623, 132625, 168901, 176245, 168904, 176241, 224873
class spell_mage_portals : public SpellScriptLoader
{
    public:
        spell_mage_portals() : SpellScriptLoader("spell_mage_portals")
        {}

        enum eSpells
        {
            SpellPortalTheramore = 49362,
            SpellPortalStonard = 49363,
            SpellPortalShattrah = 33728,
            SpellPortalShattrah2 = 35718,
            SpellPortalDalaranNorthrend = 53141,
            SpellPortalShrines = 132623,
            SpellPortalShrines2 = 132625,
            SpellPortalAshranAlliance = 168901,
            SpellPortalAshranAlliance2 = 176245,
            SpellPortalAshranHorde2 = 168904,
            SpellPortalAshranHorde = 176241,
            SpellPortalDalaranBroken = 224873,
        };

        class spell_mage_portals_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_portals_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (GetCaster() == nullptr || GetSpellInfo() == nullptr)
                    return SPELL_FAILED_CUSTOM_ERROR;

                uint32 l_SpellId = GetSpellInfo()->Id;

                switch (l_SpellId)
                {
                case eSpells::SpellPortalTheramore:
                {
                    return GetCaster()->getLevel() >= 35 ? SPELL_CAST_OK : SPELL_FAILED_LOWLEVEL;
                }

                case eSpells::SpellPortalStonard:
                {
                    return GetCaster()->getLevel() >= 40 ? SPELL_CAST_OK : SPELL_FAILED_LOWLEVEL;
                }

                case eSpells::SpellPortalShattrah:
                case eSpells::SpellPortalShattrah2:
                {
                    return GetCaster()->getLevel() >= 58 ? SPELL_CAST_OK : SPELL_FAILED_LOWLEVEL;
                }

                case eSpells::SpellPortalDalaranNorthrend:
                {
                    return GetCaster()->getLevel() >= 68 ? SPELL_CAST_OK : SPELL_FAILED_LOWLEVEL;
                }

                case eSpells::SpellPortalShrines:
                case eSpells::SpellPortalShrines2:
                {
                    return GetCaster()->getLevel() >= 85 ? SPELL_CAST_OK : SPELL_FAILED_LOWLEVEL;
                }

                case eSpells::SpellPortalAshranAlliance:
                case eSpells::SpellPortalAshranAlliance2:
                case eSpells::SpellPortalAshranHorde:
                case eSpells::SpellPortalAshranHorde2:
                {
                    return GetCaster()->getLevel() >= 89 ? SPELL_CAST_OK : SPELL_FAILED_LOWLEVEL;
                }

                case eSpells::SpellPortalDalaranBroken:
                {
                    return GetCaster()->getLevel() >= 98 ? SPELL_CAST_OK : SPELL_FAILED_LOWLEVEL;
                }


                default: return SPELL_CAST_OK;
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_mage_portals_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_portals_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26124
/// Time and Space (trigger) - 240692
class spell_mage_time_and_space_trigger : public SpellScriptLoader
{
    public:
        spell_mage_time_and_space_trigger() : SpellScriptLoader("spell_mage_time_and_space_trigger") { }

        class spell_mage_time_and_space_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_time_and_space_trigger_AuraScript);

            enum eSpells
            {
                TimeAndSpaceDamage = 240689,
                ArcaneExplosion    = 1449
            };

            Position m_AreatriggerPosition;

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_AreatriggerPosition = l_Caster->GetPosition();
            }

            void OnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    l_Caster->CastSpell(m_AreatriggerPosition, eSpells::TimeAndSpaceDamage, true);
            }

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (!p_EventInfo.GetDamageInfo())
                    return false;

                if (!p_EventInfo.GetDamageInfo()->GetSpellInfo())
                    return false;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::ArcaneExplosion)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_time_and_space_trigger_AuraScript::HandleCheckProc);
                AfterEffectApply += AuraEffectApplyFn(spell_mage_time_and_space_trigger_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_time_and_space_trigger_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
           }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_time_and_space_trigger_AuraScript();
        }
};

/// Last Update 7.3.5
/// Called by Frozen Orb PVE - 84714, Frozen Orb PVP  - 198149
class spell_mage_frozen_orb : public SpellScriptLoader
{
public:
    spell_mage_frozen_orb() : SpellScriptLoader("spell_mage_frozen_orb") { }

    class spell_mage_frozen_orb_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_frozen_orb_SpellScript);

        enum eSpells
        {
            FrozenOrb           = 84714,
            FrozenOrbOverrided  = 198149,
            T20FrostMage2P      = 242252,
            FrozenMass          = 242253,
            FreezingRain        = 238092,
            FreezingRainProc    = 240555

        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_scriptSpellId);
            if (!l_Caster || !l_Caster->IsPlayer() || !l_SpellInfo)
                return;

            uint32 l_Cooldown = l_Caster->ToPlayer()->GetSpellCooldownDelay(m_scriptSpellId);

            if (m_scriptSpellId == eSpells::FrozenOrb)
                l_Caster->ToPlayer()->AddSpellCooldown(eSpells::FrozenOrbOverrided, 0, l_Cooldown, true);
            else
                l_Caster->ToPlayer()->AddSpellCooldown(eSpells::FrozenOrb, 0, l_Cooldown, true);

            if (m_scriptSpellId == eSpells::FrozenOrb) ///< T20 Frost Mage 2P
            {
                 if (l_Caster->HasAura(eSpells::T20FrostMage2P))
                     l_Caster->CastSpell(l_Caster, eSpells::FrozenMass, true);
            }

            if (l_Caster->HasAura(eSpells::FreezingRain))
                l_Caster->CastSpell(l_Caster, eSpells::FreezingRainProc, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_mage_frozen_orb_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_frozen_orb_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Blazing Soul - 235365
class spell_mage_blazing_soul : public SpellScriptLoader
{
    public:
        spell_mage_blazing_soul() : SpellScriptLoader("spell_mage_blazing_soul") { }

        class spell_mage_blazing_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_blazing_soul_AuraScript);

            enum eSpells
            {
                BlazingBarrier      = 235313
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_DamageInfo->GetAmount() <= 0)
                    return false;

                if (!(l_SpellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FIRE))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                if (l_DamageInfo->GetAmount() <= 0)
                    return;

                Unit* l_Target = p_ProcEventInfo.GetProcTarget();
                if (!l_Target)
                    return;

                if (l_Target->GetDistance(l_Caster) > float(l_SpellInfo->Effects[EFFECT_1].BasePoints))
                    return;

                if (l_Caster->HasAura(eSpells::BlazingBarrier))
                {
                    if (AuraEffect *l_BarrierEffect = l_Caster->GetAuraEffect(eSpells::BlazingBarrier, EFFECT_0))
                    {
                        int32 l_SpellPower = l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                        float l_Versatility = l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f;
                        int32 l_MaxAbsorbAmount = l_SpellPower * 7 * (1 + l_Versatility);
                        int32 l_AmountFromDamage = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);

                        if ((l_AmountFromDamage + l_BarrierEffect->GetAmount()) > l_MaxAbsorbAmount)
                            l_BarrierEffect->SetAmount(l_MaxAbsorbAmount);
                        else
                            l_BarrierEffect->SetAmount(l_AmountFromDamage + l_BarrierEffect->GetAmount());
                    }
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_blazing_soul_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_mage_blazing_soul_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_blazing_soul_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Mage T20 Fire 2P Bonus - 242249
class spell_mage_item_t20_fire_2p_bonus : public SpellScriptLoader
{
    public:
        spell_mage_item_t20_fire_2p_bonus() : SpellScriptLoader("spell_mage_item_t20_fire_2p_bonus") { }

        class spell_mage_item_t20_fire_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_item_t20_fire_2p_bonus_AuraScript);

            enum eSpells
            {
                Fireball    = 133
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Fireball)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_item_t20_fire_2p_bonus_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_item_t20_fire_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Ignition - 246261
class spell_mage_ignition : public SpellScriptLoader
{
    public:
        spell_mage_ignition() : SpellScriptLoader("spell_mage_ignition") { }

        class spell_mage_ignition_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_ignition_AuraScript);

            enum eSpells
            {
                Pyroblast   = 11366,
                Flamestrike = 2120
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                if (DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo())
                    if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                        if (l_SpellInfo->Id == eSpells::Pyroblast || l_SpellInfo->Id == eSpells::Flamestrike)
                            return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_ignition_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_ignition_AuraScript();
        }
};

/// Last Update 7.2.0 - Build 23578
/// Called By Dragon's Breath - 31661
class spell_mage_dragons_breath : public SpellScriptLoader
{
    public:
        spell_mage_dragons_breath() : SpellScriptLoader("spell_mage_dragons_breath")
        {}

        class spell_mage_dragons_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_dragons_breath_AuraScript);

            bool HandleCheckProc(ProcEventInfo & p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr)
                    return false;

                SpellInfo const* p_ProcSpell = p_Proc.GetDamageInfo()->GetSpellInfo();

                if (p_ProcSpell == nullptr)
                    return false;

                return !p_ProcSpell->IsPeriodic();
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_dragons_breath_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_dragons_breath_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Mage T20 Frost 2P Bonus - 242252
class spell_mage_item_t20_frost_2p_bonus : public SpellScriptLoader
{
    public:
        spell_mage_item_t20_frost_2p_bonus() : SpellScriptLoader("spell_mage_item_t20_frost_2p_bonus") { }

        class spell_mage_item_t20_frost_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_item_t20_frost_2p_bonus_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_item_t20_frost_2p_bonus_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_item_t20_frost_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Presence of Mind - 205025
class spell_mage_presence_of_mind : public SpellScriptLoader
{
public:
    spell_mage_presence_of_mind() : SpellScriptLoader("spell_mage_presence_of_mind") { }

    class spell_mage_presence_of_mind_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_presence_of_mind_SpellScript);

        enum eSpells
        {
            T20ArcaneMage2P = 242248,
            CracklingEnergy = 246224,
            ArcaneCharge    = 36032
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (l_Caster->HasAura(eSpells::T20ArcaneMage2P))
            {
                l_Caster->CastSpell(l_Caster, eSpells::CracklingEnergy, true);

                for (int i = 0; i < 4; i = i + 1)
                    l_Caster->CastSpell(l_Caster, eSpells::ArcaneCharge, true);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_mage_presence_of_mind_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_presence_of_mind_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Mage T20 Arcane 2P Bonus - 242248
class spell_mage_item_t20_arcane_2p_bonus : public SpellScriptLoader
{
    public:
        spell_mage_item_t20_arcane_2p_bonus() : SpellScriptLoader("spell_mage_item_t20_arcane_2p_bonus") { }

        class spell_mage_item_t20_arcane_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_item_t20_arcane_2p_bonus_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_item_t20_arcane_2p_bonus_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_item_t20_arcane_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Item - Mage T20 Fire 4P Bonus - 242250
class spell_mage_t20_fire_4p_bonus : public SpellScriptLoader
{
    public:
        spell_mage_t20_fire_4p_bonus() : SpellScriptLoader("spell_mage_t20_fire_4p_bonus") { }

        class spell_mage_t20_fire_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_t20_fire_4p_bonus_AuraScript);

            enum eSpells
            {
                FlameStrike     = 2120,
                PyroBlast       = 11366
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (!(p_EventInfo.GetHitMask() & PROC_HIT_CRITICAL))
                    return false;

                if (l_SpellInfo->Id == eSpells::FlameStrike || l_SpellInfo->Id == eSpells::PyroBlast)
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_mage_t20_fire_4p_bonus_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_t20_fire_4p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Meteor - 153564
/// Called for Pyretic Incantation - 194331
class spell_mage_meteor_pyretic_incantation : public SpellScriptLoader
{
    public:
        spell_mage_meteor_pyretic_incantation() : SpellScriptLoader("spell_mage_meteor_pyretic_incantation") { }

        class spell_mage_meteor_pyretic_incantation_SpellScript : public SpellScript
        {
            enum eSpells
            {
                PyreticIncantion      = 194331,
                PyreticIncantionBonus = 194329
            };

            bool m_Critted = false;
            bool m_Procced = false;
            uint32 m_CurrentTarget = 0;

            PrepareSpellScript(spell_mage_meteor_pyretic_incantation_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::PyreticIncantion))
                    return;

                if (m_Procced)
                    return;

                Spell* l_Spell = GetSpell();
                if (!l_Spell)
                    return;

                if (!m_Critted && m_CurrentTarget == l_Spell->GetUnitTargetCount())
                {
                    if (!l_Spell->IsCritForTarget(l_Target))
                    {
                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IsPyreticIncantationPrevCrit) = false;
                        l_Caster->RemoveAurasDueToSpell(eSpells::PyreticIncantionBonus);
                        return;
                    }
                }
                else if (!m_Critted && m_CurrentTarget < l_Spell->GetUnitTargetCount())
                {
                    ++m_CurrentTarget;
                    if (!l_Spell->IsCritForTarget(l_Target))
                        return;
                    else
                        m_Critted = true;
                }

                if (!l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IsPyreticIncantationPrevCrit))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IsPyreticIncantationPrevCrit) = true;
                    return;
                }

                l_Caster->CastSpell(l_Caster, eSpells::PyreticIncantionBonus, true);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::PyreticIncantionBonus);
                if (!l_SpellInfo)
                    return;

                int32 bp = 0;
                bp = l_SpellInfo->Effects[EFFECT_0].BasePoints * 2; ///< we multiply here by 2 to get dammages based on critical strikes

                int32 l_AuraStack = l_Caster->GetAuraCount(eSpells::PyreticIncantionBonus);
                l_Caster->CastCustomSpell(l_Caster, eSpells::PyreticIncantionBonus, &bp, NULL, NULL, true);

                Aura *l_Aura = l_Caster->GetAura(eSpells::PyreticIncantionBonus);

                if (l_Aura)
                    l_Aura->SetStackAmount((l_AuraStack == 5) ? 5 : l_Aura->GetStackAmount() - 1);

                m_Procced = true;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_meteor_pyretic_incantation_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_meteor_pyretic_incantation_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// For Item - Mage T21 Arcane 2P Bonus - 251861
class PlayerScript_T21Arcane2P : public PlayerScript
{
    public:
        PlayerScript_T21Arcane2P() : PlayerScript("PlayerScript_T21Arcane2P") { }

        enum eSpells
        {
            T21Arcane2P     = 251861,
            ExpandingMind   = 253262
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_ARCANE_CHARGES || p_OldValue <= p_NewValue || p_Regen || !p_After)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Arcane2P);
            if (p_Player->GetActiveSpecializationID() != SPEC_MAGE_ARCANE || !p_Player->HasAura(eSpells::T21Arcane2P) || !l_SpellInfo)
                return;

            uint32 l_PowerSpent = p_OldValue - p_NewValue;

            p_Player->CastSpell(p_Player, eSpells::ExpandingMind, true);

            if (AuraEffect* l_NewBuffEffect = p_Player->GetAuraEffect(eSpells::ExpandingMind, EFFECT_0))
                l_NewBuffEffect->SetAmount(l_PowerSpent * l_SpellInfo->Effects[EFFECT_0].BasePoints);
        }
};

/// Last Update: 7.3.5 build 26365
/// For Item - Mage T21 Arcane 4P Bonus - 251862
class PlayerScript_T21Arcane4P : public PlayerScript
{
    public:
        PlayerScript_T21Arcane4P() : PlayerScript("PlayerScript_T21Arcane4P") { }

        enum eSpells
        {
            T21Arcane4P     = 251862,
            QuickThinker    = 253299
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_ARCANE_CHARGES || p_Regen || !p_After)
                return;

            if (p_OldValue >= p_NewValue && (p_OldValue != 4 || p_NewValue != 4))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Arcane4P);
            if (p_Player->GetActiveSpecializationID() != SPEC_MAGE_ARCANE || !p_Player->HasAura(eSpells::T21Arcane4P) || !l_SpellInfo)
                return;

            if (roll_chance_i(l_SpellInfo->ProcChance))
                p_Player->CastSpell(p_Player, eSpells::QuickThinker, true);
        }
};

#ifndef __clang_analyzer__
void AddSC_mage_spell_scripts()
{
    /// AreaTriggers
    new spell_areatrigger_mage_wod_frost_2p_bonus();

    new spell_mage_frozen_orb();

    /// Spells
    new spell_mage_summon_water_elemental();
    new spell_mage_chilled_to_the_core();
    new spell_mage_rule_of_threes();
    new spell_mage_blazing_prismatic_barrier();
    new spell_mage_ice_block();
    new spell_mage_illusion();
    new spell_mage_ring_of_frost_immunity();
    new spell_mage_flameglow();
    new spell_mage_incanters_flow();
    new spell_mage_greater_invisibility_removed();
    new spell_mage_greater_invisibility_triggered();
    new spell_mage_blazing_speed();
    new spell_mage_mirror_image_summon();
    new spell_mage_arcane_blast_mirror_image();
    new spell_mage_glyph_of_the_unbound_elemental();
    new spell_mage_WoDPvPFrost2PBonus();
    new spell_ring_of_frost_freeze();
    new spell_mage_item_t17_fire_4p_bonus();
    new spell_mage_item_t17_arcane_4p_bonus();
    new spell_mage_rhonins_assaulting_armwraps();

    /// Common
    new spell_mage_rune_of_power();
    new spell_mage_rune_of_power_trigger();
    new spell_mage_unstable_magic();
    new spell_mage_novas_talent();
    new spell_mage_polymorph();
    new spell_mage_prysmatic_crystal_damage();
    new spell_mage_conjure_refreshment();
    new spell_mage_time_warp();

    /// Arcane
    new spell_mage_torment_of_weak();
    new spell_mage_reset_arcane_charge();
    new spell_mage_arcane_explosion();
    new PlayerScript_mage_arcane_missiles();
    new spell_mage_arcane_missile();
    new spell_mage_arcane_barrage();
    new spell_mage_displacement_beacon();
    new spell_mage_charged_up();
    new spell_mage_overpowered();
    new spell_mage_erosion();
    new spell_mage_erosion_timer();
    new spell_mage_sloooow_down();
    new spell_mage_arcane_missiles_visual();
    new spell_mage_touch_of_the_magi();
    new spell_mage_arcane_power();
    new spell_mage_nether_tempest_damage();
    new spell_mage_nether_tempest();
    new spell_mage_evocation();
    new spell_mage_arcane_familiar();
    new PlayerScript_arcane_familiar();
    new PlayerScript_Mass_Invisibility();
    new spell_mage_mass_invis();
    new spell_mage_item_t19_arcane_4p_bonus();
    new spell_mage_presence_of_mind();
    new spell_mage_item_t20_arcane_2p_bonus();
    new PlayerScript_T21Arcane2P();
    new PlayerScript_T21Arcane4P();

    /// Fire
    new spell_mage_dragons_breath();
    new spell_mage_living_bomb_explosion();
    new spell_mage_living_bomb_aura();
    new spell_mage_living_bomb();
    new spell_mage_firestarter();
    new spell_mage_tinder();
    new PlayerScript_tinder();
    new spell_mage_pyromaniac();
    new spell_mage_heating_up();
    new spell_mage_hot_streak();
    new spell_mage_flamestrike();
    new spell_mage_kindling();
    new spell_mage_cauterize();
    new spell_mage_greater_pyroblast();
    new spell_mage_blink();
    new spell_mage_cauterizing_blink();
    new spell_mage_pyretic_incantation();
    new spell_mage_combustion();
    new spell_mage_ignite();
    new spell_mage_aftershock();
    new spell_mage_pyroblast();
    new spell_mage_scorch();
    new spell_mage_enhanced_pyrotechnics();
    new spell_mage_cinderstorm();
    new spell_mage_frenetic_speed();
    new spell_mage_conflagration();
    new spell_mage_dragon_breath();
    new spell_mage_warmth_of_the_phoenix_triggered();
    new spell_mage_blazing_soul();
    new spell_mage_item_t20_fire_2p_bonus();
    new spell_mage_ignition();
    new spell_mage_t20_fire_4p_bonus();
    new spell_mage_immolation();

    /// Frost
    new spell_mage_frostbolt();
    new spell_mage_ice_lance();
    new spell_mage_fingers_of_frost();
    new spell_mage_comet_storm();
    new spell_mage_glacial_spike();
    new spell_mage_glacial_spike_damage();
    new spell_mage_ray_of_frost();
    new spell_mage_flurry();
    new spell_mage_flurry_debuff();
    new spell_mage_flurry_missile();
    new PlayerScript_Lonely_Winter();
    new spell_mage_temporal_shield();
    new spell_mage_chilled();
    new spell_mage_cold_snap();
    new spell_mage_frozen_veins();
    new spell_mage_blizzard_hit();
    new spell_mage_pet_frost_nova();
    new spell_mage_meteor();
    new spell_mage_cone_of_frost();
    new spell_mage_ice_barrier();
    new spell_mage_frostbolt_procs();
    new spell_mage_item_t20_frost_2p_bonus();

    /// Artifact
    new spell_mage_mark_of_aluneth();
    new spell_mage_mark_of_aluneth_damage();
    new spell_mage_pheonixs_flames();
    new spell_mage_pheonixs_flames_spread();
    new PlayerScript_Phoenixs_Flames();
    new spell_mage_ebonbolt();
    new spell_mage_zannesu_journey();
    new spell_mage_jouster();
    new spell_mage_scorched_earth();
    new spell_mage_time_and_space_trigger();
    new spell_mage_meteor_pyretic_incantation();

    /// Honor Talents
    new PlayerScript_Flamecannon();
    new spell_mage_flamecannon();
    new spell_mage_frostbite();
    new spell_mage_frostbite_aura();
    new spell_mage_time_anomaly();
    new spell_mage_burst_of_cold();
    new spell_mage_chilled_to_the_bone();
    new spell_mage_firestarter_2();
    new spell_mage_flare_up();

    /// Legendaries
    new spell_mage_cord_of_infinity();
    new spell_mage_soul_of_the_archmage();

    /// Miscelannous
    new spell_mage_portals();
    new PlayerScript_mage_learn_class_hall_teleport();
    new PlayerScript_Shimmer();
    new PlayerScript_mage_arcane_charges_check();

    /// PvP Modifiers
    new spell_mage_frigid_winds();
    new spell_mage_chilled_speed();
    new spell_mage_chilled_to_the_core_bonus();
}
#endif
