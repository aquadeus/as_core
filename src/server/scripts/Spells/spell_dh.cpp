////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_DEMONHUNTER and SPELLFAMILY_GENERIC spells used by deathknight players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dh_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Containers.h"
#include "GridNotifiersImpl.h"
#include "HelperDefines.h"
#include "CellImpl.h"

/// Last Update 7.3.5 build 26365
/// Glide - 131347
class spell_dh_glide: public SpellScriptLoader
{
    public:
        spell_dh_glide() : SpellScriptLoader("spell_dh_glide") { }

        enum class Spells : uint32
        {
            GlideKnockback  = 196353,
            VengefulRetreat = 198793
        };

        class spell_dh_glide_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_glide_SpellScript);

            SpellCastResult CheckIfFallingOrMounted()
            {
                if (!GetCaster()->IsFalling() && !GetCaster()->isHover())
                    return SPELL_FAILED_NOT_ON_GROUND;

                if (GetCaster()->IsMounted())
                    return SPELL_FAILED_NOT_ON_MOUNTED;

                if (GetCaster()->HasAura((uint32)Spells::VengefulRetreat))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                l_Caster->CastSpell(l_Caster, (uint32)Spells::GlideKnockback, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dh_glide_SpellScript::CheckIfFallingOrMounted);
                OnHit += SpellHitFn(spell_dh_glide_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_glide_SpellScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Called by Vengeful Retreat - 198793
/// Called for Momentum - 206476
class spell_dh_momentum_vengeful_retreat: public SpellScriptLoader
{
    public:
        spell_dh_momentum_vengeful_retreat() : SpellScriptLoader("spell_dh_momentum_vengeful_retreat") { }

        class spell_dh_momentum_vengeful_retreat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_momentum_vengeful_retreat_SpellScript);

            enum eSpells
            {
                Momentum        = 206476,
                MomentumProc    = 208628
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::Momentum))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MomentumProc, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dh_momentum_vengeful_retreat_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_momentum_vengeful_retreat_SpellScript();
        }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Fel Rush - 195072
class spell_dh_fel_rush : public SpellScriptLoader
{
    public:
        spell_dh_fel_rush() : SpellScriptLoader("spell_dh_fel_rush") { }

        class spell_dh_fel_rush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fel_rush_SpellScript);

            enum eSpells
            {
                Glide = 131347
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_GlideSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Glide);

                if (!l_Player || !l_GlideSpellInfo)
                    return;

                l_Player->AddSpellCooldown(eSpells::Glide, 0, l_GlideSpellInfo->RecoveryTime / 5, true); ///< Added small internal CD manually. Was already done for Vengeful Retreat.
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dh_fel_rush_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fel_rush_SpellScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Fel Rush - 197922
class spell_dh_fel_rush_charge: public SpellScriptLoader
{
    public:
        spell_dh_fel_rush_charge() : SpellScriptLoader("spell_dh_fel_rush_charge") { }

        class spell_dh_fel_rush_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_fel_rush_charge_AuraScript);

            enum class Spells : uint32
            {
                FelRushDamage   = 192611,
                FelMastery      = 192939,
                FelMasteryEnergize  = 234244,
                MomentumAura    = 208628,
                Momentum        = 206476
            };

            std::list<uint64> m_Targets;
            bool m_Energized = false;

            void OnUpdate(uint32 p_Diff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Unit*> l_UnitList;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(l_Caster, l_Caster, 5.0f);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(l_Caster, l_UnitList, u_check);
                l_Caster->VisitNearbyObject(5.0f, searcher);

                l_UnitList.remove_if([] (Unit* p_Itr) -> bool
                {
                    return p_Itr == nullptr || p_Itr->isTotem() || (p_Itr->ToCreature() && p_Itr->IsAIEnabled && p_Itr->ToCreature()->AI()->IgnoreAoESpells());
                });

                for (Unit* l_Itr : l_UnitList)
                    m_Targets.push_back(l_Itr->GetGUID());
            }

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->SetDisableGravity(true);
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::FelRushDamage) = 0;
                if (l_Caster->HasAura((uint32)Spells::Momentum))
                    l_Caster->CastSpell(l_Caster, (uint32)Spells::MomentumAura, true);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->SetDisableGravity(false);
                    l_Caster->SetFall(true);
                }, 0);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->SetDisableGravity(false);
                l_Caster->SetFall(true);

                m_Targets.sort();
                m_Targets.unique();

                for (auto l_TargetItr : m_Targets)
                {
                    Unit* l_Target = Unit::GetUnit(*l_Caster, l_TargetItr);
                    if (!l_Target)
                        continue;

                    if (l_Caster->IsValidAttackTarget(l_Target))
                    {
                        l_Caster->CastSpell(l_Target, (uint32)Spells::FelRushDamage, true);

                        if (!m_Energized && l_Caster->HasAura((uint32)Spells::FelMastery))
                        {
                            m_Energized = true;
                            l_Caster->CastSpell(l_Caster, (uint32)Spells::FelMasteryEnergize, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_dh_fel_rush_charge_AuraScript::OnUpdate);
                AfterEffectApply += AuraEffectApplyFn(spell_dh_fel_rush_charge_AuraScript::OnApply, EFFECT_1, SPELL_AURA_INCREASE_MIN_SWIM_SPEED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_dh_fel_rush_charge_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_INCREASE_MIN_SWIM_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

         AuraScript* GetAuraScript() const override
         {
            return new spell_dh_fel_rush_charge_AuraScript();
         }
};

/// Fel Rush (damage) - 192611
class spell_dh_fel_rush_charge_damage : public SpellScriptLoader
{
    public:
        spell_dh_fel_rush_charge_damage() : SpellScriptLoader("spell_dh_fel_rush_charge_damage") { }

        class spell_dh_fel_rush_charge_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fel_rush_charge_damage_SpellScript);

            enum eSpells
            {
                LoramusThalipedesSacrifice  = 209002,
            };

            void HandleOnEffectHitTarget(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() == nullptr || GetHitUnit() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LoramusThalipedesSacrifice);
                int32 l_Damage = GetHitDamage();

                if (!l_Caster->HasAura(eSpells::LoramusThalipedesSacrifice) || !l_SpellInfo)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::FelRushDamage)++;
                SetHitDamage(l_Damage + CalculatePct(l_Damage, l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::FelRushDamage) * l_SpellInfo->Effects[EFFECT_0].BasePoints));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_fel_rush_charge_damage_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fel_rush_charge_damage_SpellScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Metamorphosis - 191427
class spell_dh_metamorphosis: public SpellScriptLoader
{
    public:
        spell_dh_metamorphosis() : SpellScriptLoader("spell_dh_metamorphosis") { }

        enum eSpells
        {
            MetamorphosisJump       = 191428,
            DemonicOriginsDamages   = 235894,
            DemonicOriginsTalent    = 235893,
            Metamorphosis           = 162264,
        };

        class spell_dh_metamorphosis_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_metamorphosis_SpellScript);

            SpellCastResult CheckIfRooted() {

                Unit* caster = GetCaster();
                if (caster && caster->isInRoots())
                    return SPELL_FAILED_ROOTED;

                return SPELL_CAST_OK;
            }

            void HandleOnHit(SpellEffIndex /* p_EffIndex */)
            {
                WorldLocation const* l_Dest = GetExplTargetDest();
                Unit* l_Caster = GetCaster();
                if (!l_Dest || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Dest->m_positionX, l_Dest->m_positionY, l_Dest->m_positionZ, eSpells::MetamorphosisJump, true);
                if (l_Caster->HasAura(eSpells::DemonicOriginsDamages))
                    l_Caster->RemoveAura(eSpells::DemonicOriginsDamages);
            }

            void HandleOnTransform(SpellEffIndex /**/)
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Metamorphosis))
                {
                    m_TimerPlus = (l_Caster->HasAura(eSpells::DemonicOriginsTalent) ? 15 : 30) * IN_MILLISECONDS;
                    m_TimerPlus += l_Aura->GetDuration();
                }
            }

            void HandleAfterHit()
            {
                if (GetCaster() == nullptr || m_TimerPlus == 0)
                    return;

                if (Aura* l_Aura = GetCaster()->GetAura(eSpells::Metamorphosis))
                    l_Aura->SetDuration(m_TimerPlus);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dh_metamorphosis_SpellScript::CheckIfRooted);
                OnEffectLaunch += SpellEffectFn(spell_dh_metamorphosis_SpellScript::HandleOnTransform, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectHit += SpellEffectFn(spell_dh_metamorphosis_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                AfterHit += SpellHitFn(spell_dh_metamorphosis_SpellScript::HandleAfterHit);
            }

            private:
                uint32 m_TimerPlus = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_metamorphosis_SpellScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Metamorphosis (immunity) - 201453
class spell_dh_metamorphosis_immunity: public SpellScriptLoader
{
    public:
        spell_dh_metamorphosis_immunity() : SpellScriptLoader("spell_dh_metamorphosis_immunity") { }

        enum class Spells : uint32
        {
            MetamorphosisDamage = 200166
        };

        class spell_dh_metamorphosis_immunity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_metamorphosis_immunity_AuraScript);

            void HandleOnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, (uint32)Spells::MetamorphosisDamage, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_metamorphosis_immunity_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_SCHOOL_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_metamorphosis_immunity_AuraScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Called By Metamorphosis - 200166
class spell_dh_metamorphosis_dmg : public SpellScriptLoader
{
    public:
        spell_dh_metamorphosis_dmg() : SpellScriptLoader("spell_dh_metamorphosis_dmg") { }

        enum eSpells
        {
            MetamorphosisSnare = 247121
        };

        class spell_dh_metamorphosis_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_metamorphosis_dmg_SpellScript);

            void HandleOnCast()
            {
                Spell* l_Spell = GetSpell();
                if (!l_Spell)
                    return;

                std::list<Spell::TargetInfo> const& l_TargetInfoList = l_Spell->GetTargetInfos();
                for (Spell::TargetInfo const& l_TargetInfo : l_TargetInfoList)
                {
                    if (!IS_PLAYER_GUID(l_TargetInfo.targetGUID))
                        continue;

                    const_cast<Spell::TargetInfo&>(l_TargetInfo).effectMask &= ~ (1 << EFFECT_0);
                }
            }

            void HandleOnHit(SpellEffIndex /* p_EffIndex */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->IsPlayer() && l_Target != l_Caster)
                    l_Caster->CastSpell(l_Target, eSpells::MetamorphosisSnare, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_metamorphosis_dmg_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnCast += SpellCastFn(spell_dh_metamorphosis_dmg_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_metamorphosis_dmg_SpellScript();
        }

        class spell_dh_metamorphosis_dmg_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_metamorphosis_dmg_AuraScript);

            void OnApply(AuraEffect const* /*l_AurEff*/, AuraEffectHandleModes /*l_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    if (l_Target->IsPlayer())
                        PreventDefaultAction();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_metamorphosis_dmg_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_metamorphosis_dmg_AuraScript();
        }
};

/// Last Update 7.3.5
/// Infernal Strike- 189112
class spell_dh_infernal_strike : public SpellScriptLoader
{
    public:
        spell_dh_infernal_strike() : SpellScriptLoader("spell_dh_infernal_strike") { }

        enum eSpells
        {
            RainOfChaos = 205628,
            RainOfChaosTrigger = 232538
        };

        class spell_dh_infernal_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_infernal_strike_SpellScript);

            void HandleOnHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::RainOfChaos))
                    l_Caster->CastSpell(l_Target, eSpells::RainOfChaosTrigger, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_infernal_strike_SpellScript::HandleOnHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_infernal_strike_SpellScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Vengeful Retreat - 198793
class spell_dh_vengeful_retreat : public SpellScriptLoader
{
    public:
        spell_dh_vengeful_retreat() : SpellScriptLoader("spell_dh_vengeful_retreat") { }

        class spell_dh_vengeful_retreat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_vengeful_retreat_SpellScript);

            enum eSpells
            {
                Glide = 131347,
                EternalHunger = 208985,
                EternalHungerTrigger = 216758
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster)
                    l_Caster->RemoveAurasWithMechanic(Mechanics::MECHANIC_SNARE);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::EternalHunger))
                    l_Caster->CastSpell(l_Caster, eSpells::EternalHungerTrigger, true);

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_GlideSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Glide);

                if (!l_Player || !l_GlideSpellInfo)
                    return;

                l_Player->AddSpellCooldown(eSpells::Glide, 0, l_GlideSpellInfo->RecoveryTime / 5, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_dh_vengeful_retreat_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_dh_vengeful_retreat_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_vengeful_retreat_SpellScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Vengeful Retreat (damage) - 198813
class spell_dh_vengeful_retreat_damage : public SpellScriptLoader
{
    public:
        spell_dh_vengeful_retreat_damage() : SpellScriptLoader("spell_dh_vengeful_retreat_damage") { }

        class spell_dh_vengeful_retreat_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_vengeful_retreat_damage_SpellScript);

            enum eSpells
            {
                Prepared            = 203551,
                PreparedEnergize    = 203650
            };

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster && l_Caster->HasAura(eSpells::Prepared) && !p_Targets.empty())
                    l_Caster->CastSpell(l_Caster, eSpells::PreparedEnergize, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dh_vengeful_retreat_damage_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_vengeful_retreat_damage_SpellScript();
        }
};

/// Last Update 7.3.5
/// Called by Demon Blades - 203555
class spell_dh_demon_blades : public SpellScriptLoader
{
    public:
        spell_dh_demon_blades() : SpellScriptLoader("spell_dh_demon_blades") { }

        class spell_dh_demon_blades_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_demon_blades_Aurascript);

            enum eSpells
            {
                DemonBladesDamage = 203796
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_ProcInfos.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo) ///< Only auto attacks
                    return;

                Unit* l_Victim = l_DamageInfo->GetTarget();
                if (!l_Victim)
                    return;

                if (roll_chance_i(p_AurEff->GetAmount()))
                    l_Caster->CastSpell(l_Victim, eSpells::DemonBladesDamage, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_demon_blades_Aurascript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_demon_blades_Aurascript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Fel Barrage - 211053
class spell_dh_fel_barrage : public SpellScriptLoader
{
    public:
        spell_dh_fel_barrage() : SpellScriptLoader("spell_dh_fel_barrage") { }

        class spell_dh_fel_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_fel_barrage_AuraScript);

            enum eSpells
            {
                FelBarrageDamage = 211052
            };

            void OnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FelBarrageDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_fel_barrage_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_fel_barrage_AuraScript();
        }

        class spell_dh_fel_barrage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fel_barrage_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster)
                    return SpellCastResult::SPELL_CAST_OK;

                return SpellCastResult::SPELL_FAILED_DONT_REPORT;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dh_fel_barrage_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fel_barrage_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Fel Barrage Passive - 222703
class spell_dh_fel_barrage_passive : public SpellScriptLoader
{
    public:
        spell_dh_fel_barrage_passive() : SpellScriptLoader("spell_dh_fel_barrage_passive") { }

        class spell_dh_fel_barrage_passive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_fel_barrage_passive_AuraScript);

            enum eSpells
            {
                FelBarrage          = 211053,
                FelBarrageDamage    = 211052
            };

            void HandleOnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_PassiveInfo = GetSpellInfo();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (p_ProcEventInfo.GetDamageInfo())
                {
                    if (SpellInfo const* l_SpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo())
                    {
                        if (l_SpellInfo->Id == eSpells::FelBarrageDamage) ///< 0x0 0x800 0x0 0x0 if needed for future
                            return; ///< don't proc from itself
                    }
                }

                if (l_Player->HasSpellCooldown(l_PassiveInfo->Id))
                    return;


                l_Player->ReduceSpellCooldown(eSpells::FelBarrage, 5 * IN_MILLISECONDS);

                l_Player->AddSpellCooldown(l_PassiveInfo->Id, 0, 250); ///< proc once per missile (build 23337 hotfix - https://www.icy-veins.com/forums/topic/23337-legion-hotfixes-september-16th/)
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_fel_barrage_passive_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_fel_barrage_passive_AuraScript();
        }
};

/// Last Update 7.1.5 - 22045
/// Called by Felblade - 213241
class spell_dh_felblade_charge : public SpellScriptLoader
{
    public:
        spell_dh_felblade_charge() : SpellScriptLoader("spell_dh_felblade_charge") { }

        class spell_dh_felblade_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_felblade_charge_SpellScript);

            enum eSpells
            {
                FelBladeDamage = 213243
            };

            void HandleCharge(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FelBladeDamage, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_felblade_charge_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_felblade_charge_SpellScript;
        }
};

/// Last Update 7.1.5 - 22045
/// Called by Felblade - 232893
class spell_dh_felblade : public SpellScriptLoader
{
    public:
        spell_dh_felblade() : SpellScriptLoader("spell_dh_felblade") { }

        class spell_dh_felblade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_felblade_SpellScript);

            enum eSpells
            {
                FelBladeCharge = 213241
            };

            SpellCastResult CheckIfRooted() {

                Unit* caster = GetCaster();
                if (caster && caster->isInRoots())
                    return SPELL_FAILED_ROOTED;

                return SPELL_CAST_OK;
            }

            void HandleCharge(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FelBladeCharge, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dh_felblade_SpellScript::CheckIfRooted);
                OnEffectHitTarget += SpellEffectFn(spell_dh_felblade_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_felblade_SpellScript;
        }
};

/// Last Update 7.1.5 Build 23420
/// Blade Dance (damage) - 199552, Blade Dance (last damage) - 200685, Death Sweep (damage) - 210153, Death Sweep (last damage) - 210155
class spell_dh_blade_dance_damage : public SpellScriptLoader
{
    public:
        spell_dh_blade_dance_damage() : SpellScriptLoader("spell_dh_blade_dance_damage") { }

        class spell_dh_blade_dance_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_blade_dance_damage_SpellScript);

            enum eSpells
            {
                FirstBlood      = 206416,
                BladeDanceDmg   = 199552,
                LastBladeDance  = 200685,
                BalancedBlades  = 201470,
                DeathSweepDmg   = 210153,
                LastDeathSweep  = 210155
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfoBalancedBlades = sSpellMgr->GetSpellInfo(eSpells::BalancedBlades);
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfoBalancedBlades || !l_SpellInfo)
                    return;

                int32 l_Damage = GetHitDamage();
                if (AuraEffect const* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::FirstBlood, EFFECT_0))
                {
                    if ((l_SpellInfo->Id == eSpells::BladeDanceDmg || l_SpellInfo->Id == eSpells::DeathSweepDmg) && !l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::BladeDanceFirstBlood))
                        l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::BladeDanceFirstBlood) = l_Target->GetGUID();

                    bool l_SameTarget = l_Target->GetGUID() == l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::BladeDanceFirstBlood);
                    if (l_SameTarget)
                    {
                        /// (100+$m1)/100
                        int32 l_Multiplier = ((100 + l_AuraEffect->GetAmount()) / 100);
                        if (l_Caster->CanApplyPvPSpellModifiers())
                            l_Multiplier -= l_Multiplier / 3; ///< First blood bonus is reduced by 33.33% in PvP

                        l_Damage *= l_Multiplier;
                    }

                    /// Reset after last tick calculations
                    if ((l_SpellInfo->Id == eSpells::LastBladeDance || l_SpellInfo->Id == eSpells::LastDeathSweep) && l_SameTarget)
                        l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::BladeDanceFirstBlood) = 0;
                }

                if (l_Caster->HasAura(eSpells::BalancedBlades))
                    AddPct(l_Damage, l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BladeDanceFirstBlood) * l_SpellInfoBalancedBlades->Effects[EFFECT_0].BasePoints);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_blade_dance_damage_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_blade_dance_damage_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Blade Dance - 188499, Death Sweep (Blade Dance while metamorphosis) - 210152
class spell_dh_blade_dance : public SpellScriptLoader
{
    public:
        spell_dh_blade_dance() : SpellScriptLoader("spell_dh_blade_dance") { }

        class spell_dh_blade_dance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_blade_dance_AuraScript);

            void HandleAfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->m_Functions.AddFunction([=]() -> void
                    {
                        l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::BladeDanceFirstBlood) = 0;
                        l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BladeDanceFirstBlood) = 0;
                    }, l_Caster->m_Functions.CalculateTime(10));

                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_blade_dance_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_MOD_DODGE_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_blade_dance_AuraScript();
        }

        class spell_dh_blade_dance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_blade_dance_SpellScript);

            enum eSpells
            {
                BladeDance      = 188499,
                Metamorphosis   = 162264,
                ChaosTheory     = 248072,
                ChaosBlades     = 247938,
                T20Havoc4P      = 242226,
                T20Havoc4pFury  = 245862
            };

            bool m_HasTargets = false;

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetSpellInfo()->Id == eSpells::BladeDance && l_Caster->HasAura(eSpells::Metamorphosis))
                    GetSpell()->cancel();
            }

            void OnFilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BladeDanceFirstBlood) = p_Targets.size();

                m_HasTargets = p_Targets.size() > 0;
            }

            void HandleAfterCast()
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ChaosTheory);
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::ChaosTheory))
                {
                    if (roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                    {
                        if (Aura* l_Aura = l_Caster->AddAura(eSpells::ChaosBlades, l_Caster))
                        {
                            l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                            l_Aura->SetMaxDuration(l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                        }
                    }
                }

                if (m_HasTargets)
                    if (l_Caster->HasAura(eSpells::T20Havoc4P))
                        l_Caster->CastSpell(l_Caster, eSpells::T20Havoc4pFury , true);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_dh_blade_dance_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_dh_blade_dance_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dh_blade_dance_SpellScript::OnFilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_blade_dance_SpellScript();
        }
};


/// Last Update 7.0.3 - 22045
/// Throw Glaive - 185123
class spell_dh_throw_glaive : public SpellScriptLoader
{
    public:
        spell_dh_throw_glaive() : SpellScriptLoader("spell_dh_throw_glaive")
        {}

        class spell_dh_throw_glaive_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_throw_glaive_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject*& p_Itr)
                {
                    if (p_Itr == nullptr)
                        return true;

                    Unit* l_Target = p_Itr->ToUnit();

                    return (l_Target && l_Target->isTotem());
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dh_throw_glaive_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dh_throw_glaive_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_TARGET_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_throw_glaive_SpellScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Bloodlet - 206473
class spell_dh_bloodlet : public SpellScriptLoader
{
    public:
        spell_dh_bloodlet() : SpellScriptLoader("spell_dh_bloodlet") { }

        class spell_dh_bloodlet_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_bloodlet_Aurascript);

            enum eSpells
            {
                ThrowGlaive     = 185123,
                BloodletAura    = 207690
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                DamageInfo const* l_DamageInfo = p_ProcInfos.GetDamageInfo();

                if (l_DamageInfo == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::ThrowGlaive)
                    return;

                Unit* l_Victim = l_DamageInfo->GetTarget();

                if (l_Victim == nullptr)
                    return;

                int32 l_Damage = l_DamageInfo->GetAmount();
                l_Damage = CalculatePct(l_Damage, p_AurEff->GetAmount());
                int32 l_RemainingAmount = 0;
                if (AuraEffect* l_AuraEffect = l_Victim->GetAuraEffect(eSpells::BloodletAura, EFFECT_0, l_Caster->GetGUID()))
                {
                    l_RemainingAmount = l_AuraEffect->GetAmount();
                    l_RemainingAmount *= (l_AuraEffect->GetTotalTicks() - l_AuraEffect->GetTickNumber());
                    l_Victim->RemoveAura(eSpells::BloodletAura, l_Caster->GetGUID());
                }
                l_Damage += l_RemainingAmount;
                l_Caster->CastSpell(l_Victim, eSpells::BloodletAura, true);
                if (AuraEffect* l_AuraEffect = l_Victim->GetAuraEffect(eSpells::BloodletAura, EFFECT_0, l_Caster->GetGUID()))
                {
                    if (l_AuraEffect->GetTotalTicks())
                        l_Damage /= l_AuraEffect->GetTotalTicks();
                    l_AuraEffect->SetAmount(l_Damage);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_bloodlet_Aurascript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_bloodlet_Aurascript();
        }
};

/// Last Update 7.0.3 - 22293
/// Momentum - 206476
class spell_dh_momentum : public SpellScriptLoader
{
    public:
        spell_dh_momentum() : SpellScriptLoader("spell_dh_momentum") { }

        class spell_dh_momentum_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_momentum_Aurascript);

            enum eSpells
            {
                FelRush         = 195072,
                VengefulRetreat = 198813,
                MomentumAura    = 208628
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                DamageInfo const* l_DamageInfo = p_ProcInfos.GetDamageInfo();

                if (l_DamageInfo == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::FelRush && l_SpellInfo->Id != eSpells::VengefulRetreat)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MomentumAura, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_momentum_Aurascript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_momentum_Aurascript();
        }
};

/// Last Update 7.0.3 - 22293
/// Master of the Glaive - 203556
class spell_dh_master_of_the_glaive : public SpellScriptLoader
{
    public:
        spell_dh_master_of_the_glaive() : SpellScriptLoader("spell_dh_master_of_the_glaive") { }

        class spell_dh_master_of_the_glaive_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_master_of_the_glaive_Aurascript);

            enum eSpells
            {
                ThrowGlaive = 185123
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                DamageInfo const* l_DamageInfo = p_ProcInfos.GetDamageInfo();

                if (l_DamageInfo == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr)
                    return;

                Unit* l_Victim = l_DamageInfo->GetTarget();

                if (l_Victim == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::ThrowGlaive)
                    return;

                l_Caster->CastSpell(l_Victim, p_AurEff->GetTriggerSpell(), true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_master_of_the_glaive_Aurascript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_master_of_the_glaive_Aurascript();
        }
};

/// Last Update 7.0.3 - 22293
/// Called by Metamorphosis - 191427
/// Demon Reborn - 162264
class spell_dh_demon_reborn : public SpellScriptLoader
{
    public:
        spell_dh_demon_reborn() : SpellScriptLoader("spell_dh_demon_reborn") { }

    class spell_dh_demon_reborn_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dh_demon_reborn_SpellScript);

        enum eSpells
        {
            ChaosNova   = 179057,
            Blur        = 198589,
            EyeBeam     = 198013,
            DemonReborn = 193897
        };

        void HandleAfterCast()
        {
            Player* l_Player = GetCaster()->ToPlayer();

            if (l_Player == nullptr)
                return;

            if (!l_Player->HasAura(eSpells::DemonReborn))
                return;

            l_Player->RemoveSpellCooldown(eSpells::Blur, true);
            l_Player->RemoveSpellCooldown(eSpells::EyeBeam, true);
            l_Player->RemoveSpellCooldown(eSpells::ChaosNova, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_dh_demon_reborn_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dh_demon_reborn_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Soul Cleave - 228477
class spell_dh_soul_cleave : public SpellScriptLoader
{
    public:
        spell_dh_soul_cleave() : SpellScriptLoader("spell_dh_soul_cleave") { }

        class spell_dh_soul_cleave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_soul_cleave_SpellScript);

            enum eSpells
            {
                FeastofSouls            = 207697,
                FeastofSoulsAura        = 207693,
                SoulFragment            = 203795,
                SoulFragment2           = 204255,
                SoulCleave              = 228478,
                SigilOfSilence          = 202137,
                SigilOfFlame            = 204596,
                SigilOfChains           = 202138,
                RazelikhDefilement      = 210840,
                T19Vengeance4p          = 211077,
                DemonSpikes             = 203720,
                SigilOfMisery           = 207684,
                EruptingSoulsAura       = 238082,
                EruptingSoulsDmg        = 243160,
                T20Vengeance4P          = 242229,
                SigilOfVersatility      = 242230,
                DevourSoulsTrait        = 212821
            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::SigilOfSilence,
                eSpells::SigilOfFlame,
                eSpells::SigilOfChains,
                eSpells::SigilOfMisery
            };

            int32 m_PowerCost;

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer() || p_PowerType != POWER_PAIN)
                    return;

                float& l_Multiplier = l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::SoulCleaveCostMultiplier);
                l_Multiplier = 1.0f;

                uint32 minCost = 300;
                uint32 maxCost = 600;

                // Check if it was changed.
                for (auto l_Power : GetSpellInfo()->SpellPowers)
                {
                    if (l_Power->PowerType == POWER_PAIN)
                    {
                        minCost = l_Power->Cost;
                        maxCost = minCost + l_Power->CostAdditional;
                        break;
                    }
                }

                int32 currentPower = l_Caster->GetPower(POWER_PAIN);
                if (currentPower > minCost && maxCost > minCost)
                {
                    int32 additionalCost = currentPower > maxCost ? maxCost - minCost : currentPower - minCost;
                    l_Multiplier = 1.0f + (float)additionalCost / (maxCost - minCost);

                    if (l_Caster->HasAura(eSpells::T19Vengeance4p))
                    {
                        if (SpellInfo const* l_DemonSpikes = sSpellMgr->GetSpellInfo(eSpells::DemonSpikes))
                            l_Caster->ToPlayer()->ReduceChargeCooldown(l_DemonSpikes->ChargeCategoryEntry, (additionalCost / 100) * IN_MILLISECONDS);
                    }

                    p_PowerCost += additionalCost;
                }

                m_PowerCost = p_PowerCost;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::FeastofSouls))
                {
                    int32 l_HealingAmount = int32(l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * (l_Caster->GetHealthPct() / 100) * 1.3f * 3.0f);
                    l_HealingAmount /= 2;
                    l_Caster->CastCustomSpell(l_Caster, eSpells::FeastofSoulsAura, &l_HealingAmount, NULL, NULL, true);
                }

                std::list<AreaTrigger*> l_SoulFragments;
                l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment);
                l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment2);

                if (!l_SoulFragments.empty())
                {
                    for (AreaTrigger* l_SoulFragment : l_SoulFragments)
                    {
                        if (l_SoulFragment == nullptr)
                            continue;

                        if (l_Caster->GetDistance(l_SoulFragment) <= GetSpellInfo()->Effects[EFFECT_0].BasePoints)
                            l_SoulFragment->CastAction();
                    }
                }

                if (l_Caster->HasAura(eSpells::T20Vengeance4P))
                    l_Caster->CastSpell(l_Caster, eSpells::SigilOfVersatility, true);

                if (l_Caster->HasAura(eSpells::EruptingSoulsAura))
                    DoEruptingSouls(l_Caster, l_SoulFragments.size());

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::RazelikhDefilement);
                if (!l_Player || !l_Caster->HasAura(eSpells::RazelikhDefilement) || !l_SpellInfo)
                    return;

                uint8 m_SigilResult = 0;
                std::vector<eSpells> l_SpellsInCooldown;
                for (auto l_Spell : m_Spells)
                    if (l_Player->HasSpellCooldown(l_Spell))
                        l_SpellsInCooldown.push_back(l_Spell);

                if (l_SpellsInCooldown.empty())
                    return;

                m_SigilResult = urand(0, l_SpellsInCooldown.size() - 1);
                l_Player->ReduceSpellCooldown(l_SpellsInCooldown[m_SigilResult], l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                // Prevent bugs
                float& l_Multiplier = l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::SoulCleaveCostMultiplier);
                if (l_Multiplier > 2.0f)
                    l_Multiplier = 2.0f;
                else if (l_Multiplier < 1.0f)
                    l_Multiplier = 1.0f;

                ///AskMrRobot :  2 * 7.26 * TotalAttackPower * TotalHealingMultiplier * (1 + 0.03 * ArtifactTraitRank(DevourSouls)) * PowerSpent / 50
                int32 l_HealAmount = int32(l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 7.6f * l_Multiplier * m_PowerCost / 500);

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                AuraEffect* l_DevourSouls = l_Caster->GetAuraEffect(eSpells::DevourSoulsTrait, EFFECT_0);
                if (l_DevourSouls)
                {
                    AddPct(l_HealAmount, l_DevourSouls->GetAmount());
                }

                float l_Versatility = l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f;
                l_HealAmount *= (1 + l_Versatility);

                SetHitHeal(l_HealAmount);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_dh_soul_cleave_SpellScript::HandlePowerCost);
                AfterCast += SpellCastFn(spell_dh_soul_cleave_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_dh_soul_cleave_SpellScript::HandleHeal, EFFECT_3, SPELL_EFFECT_HEAL);
            }

        private:

            void DoEruptingSouls(Unit* p_Caster, uint32 p_SoulsCount)
            {
                Unit* l_Target = GetExplTargetUnit();

                p_Caster->CastCustomSpell(eSpells::EruptingSoulsDmg, SpellValueMod::SPELLVALUE_BASE_POINT1, (int32)p_SoulsCount, l_Target, true);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_soul_cleave_SpellScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Soul Cleave - 228478
class spell_dh_soul_cleave_dmg : public SpellScriptLoader
{
    public:
        spell_dh_soul_cleave_dmg() : SpellScriptLoader("spell_dh_soul_cleave_dmg") { }

        class spell_dh_soul_cleave_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_soul_cleave_dmg_SpellScript);

            void HandleHitTarget(SpellEffIndex effIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                // Prevent bugs
                float& l_Multiplier = l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::SoulCleaveCostMultiplier);
                if (l_Multiplier > 2.0f)
                    l_Multiplier = 2.0f;
                else if (l_Multiplier < 1.0f)
                    l_Multiplier = 1.0f;

                SetHitDamage(GetHitDamage() * l_Multiplier);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_soul_cleave_dmg_SpellScript::HandleHitTarget, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_soul_cleave_dmg_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Erupting Souls - 243160
class spell_dh_erupting_souls_dmg : public SpellScriptLoader
{
    public:
        spell_dh_erupting_souls_dmg() : SpellScriptLoader("spell_dh_erupting_souls_dmg") { }

        class spell_dh_erupting_souls_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_erupting_souls_dmg_SpellScript);

            void HandleHitTarget(SpellEffIndex effIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_SoulsCount = 0;
                if (Spell const* l_Spell = GetSpell())
                    l_SoulsCount = l_Spell->GetSpellValue(SpellValueMod::SPELLVALUE_BASE_POINT1);

                if (l_SoulsCount > 0)
                    SetHitDamage(GetHitDamage() * l_SoulsCount);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_erupting_souls_dmg_SpellScript::HandleHitTarget, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_erupting_souls_dmg_SpellScript();
        }
};

/// Last Update: 7.3.5 build 26365
/// Called by Burning Alive 207771
class spell_dh_burning_alive_dot : public SpellScriptLoader
{
public:
    spell_dh_burning_alive_dot() : SpellScriptLoader("spell_dh_burning_alive_dot") { }

    class spell_dh_burning_alive_dot_AuraScript : public AuraScript
    {
        enum eSpells
        {
            BurningAlive                        = 207739,
            BurningAliveDot                     = 207771
        };

        PrepareAuraScript(spell_dh_burning_alive_dot_AuraScript);

        void OnPeriodic(AuraEffect const* /*p_AuraEffect*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::BurningAlive))
                return;

            Unit* l_Target = GetTarget();
            if (!l_Target)
                return;

            Aura* l_Aura = GetAura();
            if (!l_Aura)
                return;

            if (l_Aura->GetMaxDuration() < 8 * IN_MILLISECONDS)
                return;

            std::list<Unit*> l_UnitList;

            JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(l_Target, l_Caster, 30.0f);
            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(l_Target, l_UnitList, u_check);
            l_Target->VisitNearbyObject(30.0f, searcher);

            l_UnitList.remove_if([l_Caster, l_Target](Unit* p_Unit) -> bool
            {
                if (!p_Unit)
                    return true;

                if (!l_Caster->IsValidAttackTarget(p_Unit))
                    return true;

                if (p_Unit->GetGUID() == l_Target->GetGUID())
                    return true;

                if (p_Unit->HasAura(eSpells::BurningAliveDot))
                    return true;

                return false;
            });

            if (l_UnitList.empty())
                return;

            l_UnitList.sort(JadeCore::DistanceCompareOrderPred(l_Target));

            Unit* l_SecondTarget = l_UnitList.front();

            if (!l_SecondTarget)
                return;

            if (Aura* l_DotAura = l_Caster->AddAura(eSpells::BurningAliveDot, l_SecondTarget))
            {
                l_DotAura->SetMaxDuration(8 * IN_MILLISECONDS - 1);
                l_DotAura->SetDuration(8 * IN_MILLISECONDS - 1);
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_burning_alive_dot_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dh_burning_alive_dot_AuraScript();
    }
};

/// Last Update 7.3.5 - 26365
/// Fiery Brand - 204021
class spell_dh_fiery_brand : public SpellScriptLoader
{
    public:
        spell_dh_fiery_brand() : SpellScriptLoader("spell_dh_fiery_brand") { }

        class spell_dh_fiery_brand_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fiery_brand_SpellScript);

            enum eSpells
            {
                BurningAlive                        = 207739,
                BurningAliveDot                     = 207771,
                FieryBrandMarker                    = 207744,
                FieryBrandAbsorb                    = 204022,
                SpiritOfTheDarknessFlameBonusHeal   = 235551,
                SpiritOfTheDarknessFlameBonusDamage = 235543,
                SpiritOfTheDarknessFlame            = 235524
            };

            void HandleCharge(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FieryBrandAbsorb, true);

                if (l_Caster->HasAura(eSpells::BurningAlive))
                {
                    l_Caster->CastSpell(l_Target, eSpells::BurningAliveDot, true);

                    std::list<Unit*> l_UnitList;

                    JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(l_Target, l_Caster, 30.0f);
                    JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(l_Target, l_UnitList, u_check);
                    l_Target->VisitNearbyObject(30.0f, searcher);

                    l_UnitList.remove_if([l_Caster, l_Target](Unit* p_Unit) -> bool
                    {
                        if (!p_Unit)
                            return true;

                        if (!l_Caster->IsValidAttackTarget(p_Unit))
                            return true;

                        if (p_Unit == l_Target)
                            return true;

                        if (p_Unit->HasAura(eSpells::BurningAliveDot))
                            return true;

                        return false;
                    });

                    if (l_UnitList.empty())
                        return;

                    l_UnitList.sort(JadeCore::DistanceCompareOrderPred(l_Target));

                    Unit* l_SecondTarget = l_UnitList.front();

                    if (l_SecondTarget == nullptr)
                        return;

                    if (Aura* l_DotAura = l_Caster->AddAura(eSpells::BurningAliveDot, l_SecondTarget))
                    {
                        l_DotAura->SetMaxDuration(8 * IN_MILLISECONDS - 1);
                        l_DotAura->SetDuration(8 * IN_MILLISECONDS - 1);
                    }
                }
                else
                    l_Caster->CastSpell(l_Target, eSpells::FieryBrandMarker, true);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Healing = GetHitDamage();
                if (l_Caster->HasAura(eSpells::SpiritOfTheDarknessFlame))
                    l_Caster->CastCustomSpell(l_Caster, eSpells::SpiritOfTheDarknessFlameBonusHeal, &l_Healing, nullptr, nullptr, true);

                l_Caster->RemoveAura(eSpells::SpiritOfTheDarknessFlameBonusDamage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_fiery_brand_SpellScript::HandleCharge, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnHit += SpellHitFn(spell_dh_fiery_brand_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fiery_brand_SpellScript;
        }
};

/// Last Update 7.3.0
/// Infernal Strike (jump) - 189110
class PlayerScript_dh_infernal_strike : public PlayerScript
{
public:
    PlayerScript_dh_infernal_strike() : PlayerScript("PlayerScript_infernal_strike") {}

    enum eSpells
    {
        InfernalStrike      = 189110,
        InfernalStrikeDamage = 189112,
        FlameCrash          = 227322,
        SigilOfFlame        = 204596
    };

    void OnFinishMovement(Player* p_Player, uint32 p_SpellID, uint64 const /*p_TargetGUID*/) override
    {
        if (!(p_SpellID == eSpells::InfernalStrike))
            return;

        if (p_Player->HasAura(eSpells::FlameCrash))
            p_Player->m_SpellHelper.GetBool(eSpellHelpers::FlameCrash) = true;
        p_Player->m_SpellHelper.GetBool(eSpellHelpers::InfernalStrike) = true;
    }

    void OnMoveSplineDone(Player* p_Player) override
    {
        if (p_Player->getClass() != CLASS_DEMON_HUNTER)
            return;

        p_Player->AddDelayedEvent([p_Player]() -> void
        {
            bool& l_FlameCrash = p_Player->m_SpellHelper.GetBool(eSpellHelpers::FlameCrash);
            bool& l_InfernalStrike = p_Player->m_SpellHelper.GetBool(eSpellHelpers::InfernalStrike);

            if (l_FlameCrash)
            {
                l_FlameCrash = false;
                p_Player->CastCustomSpell(eSpells::SigilOfFlame, SpellValueMod::SPELLVALUE_IGNORE_CD, 1, p_Player, true);
            }

            if (l_InfernalStrike)
            {
                l_InfernalStrike = false;
                p_Player->CastSpell(p_Player, eSpells::InfernalStrikeDamage, true);
            }
        }, 250);
    }
};

/// Last Update 7.0.3 - 22293
/// Fel Devastation - 212084
class spell_dh_fel_devastation : public SpellScriptLoader
{
    public:
        spell_dh_fel_devastation() : SpellScriptLoader("spell_dh_fel_devastation") { }

        class spell_dh_fel_devastation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_fel_devastation_AuraScript);

            enum eSpells
            {
                FelDevastationDamage = 212105
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FelDevastationDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_fel_devastation_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_fel_devastation_AuraScript();
        }
};

/// Fel Devastation Damage - 212105
/// Updated as of 7.0.3 - 22522
class spell_dh_fel_devastation_damage : public SpellScriptLoader
{
    public:
        spell_dh_fel_devastation_damage() : SpellScriptLoader("spell_dh_fel_devastation_damage") { }

        class spell_impl : public SpellScript
        {
            PrepareSpellScript(spell_impl);

            enum
            {
                SPELL_DH_FEL_DEVASTATION_HEAL    = 212106
            };

            void HandleDamage()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetHitUnit())
                    l_Caster->CastSpell(l_Caster, SPELL_DH_FEL_DEVASTATION_HEAL, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_impl::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_impl();
        }
};

/// Last Update 7.3.5 - 26365
/// Called by Demon Spikes - 203720, 203819
class spell_dh_demon_spikes : public SpellScriptLoader
{
    public:
        spell_dh_demon_spikes() : SpellScriptLoader("spell_dh_demon_spikes") { }

        enum eSpells
        {
            DemonSpikesAura     = 203819,
            DefensiveSpikes     = 212829,
            DefensiveSpikesProc = 212871,
            JaggedSpikesTalent  = 205627,
            JaggedSpikesAura    = 208796
        };

        class spell_dh_demon_spikes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_demon_spikes_SpellScript);

            void HandleAfterHit()
            {
                SpellInfo const* l_spellInfo = sSpellMgr->GetSpellInfo(eSpells::DemonSpikesAura);
                if (!l_spellInfo)
                    return;

                Unit* l_Caster = GetCaster();

                int32 l_PrevDuration = 0;

                if (Aura* l_PrevAura = l_Caster->GetAura(eSpells::DemonSpikesAura))
                    l_PrevDuration = l_PrevAura->GetDuration();

                l_Caster->CastSpell(l_Caster, eSpells::DemonSpikesAura, true);

                /// Demon Spikes can only be extended to a maximum of three times its base duration.
                if (Aura* l_NewAura = l_Caster->GetAura(eSpells::DemonSpikesAura))
                {
                    int32 l_NewDuration = std::min(l_spellInfo->GetDuration() * 3, l_NewAura->GetDuration() + l_PrevDuration);

                    if (l_NewDuration != l_NewAura->GetDuration())
                    {
                        l_NewAura->SetMaxDuration(l_NewDuration);
                        l_NewAura->SetDuration(l_NewDuration);
                    }
                }

                if (l_Caster->HasAura(eSpells::DefensiveSpikes))
                    l_Caster->CastSpell(l_Caster, eSpells::DefensiveSpikesProc, true);
            }

            void Register() override
            {
                if (m_scriptSpellId == 203720)
                    AfterCast += SpellCastFn(spell_dh_demon_spikes_SpellScript::HandleAfterHit);
            }
        };

        class spell_dh_demon_spikes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_demon_spikes_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect const* l_AurEff = l_Caster->GetAuraEffect(eSpells::DefensiveSpikesProc, SpellEffIndex::EFFECT_0))
                    p_Amount += l_AurEff->GetAmount();
            }

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::JaggedSpikesTalent))
                    l_Caster->CastSpell(l_Caster, eSpells::JaggedSpikesAura, true);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::DefensiveSpikesProc);
                l_Caster->RemoveAura(eSpells::JaggedSpikesAura);
            }

            void Register() override
            {
                if (m_scriptSpellId == 203819)
                {
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_demon_spikes_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_PARRY_PERCENT);
                    AfterEffectApply += AuraEffectApplyFn(spell_dh_demon_spikes_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_PARRY_PERCENT, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_dh_demon_spikes_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_PARRY_PERCENT, AURA_EFFECT_HANDLE_REAL);
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_demon_spikes_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_demon_spikes_AuraScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called By Jagged Spikes - 208796
class spell_dh_jagged_spikes : public SpellScriptLoader
{
public:
    spell_dh_jagged_spikes() : SpellScriptLoader("spell_dh_jagged_spikes") { }

    class spell_dh_jagged_spikes_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_jagged_spikes_AuraScript);

        enum eSpells
        {
            JaggedSpikes = 205627,
            JaggedSpikesDmg = 208790
        };

        void HandleOnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
        {
            SpellInfo const* l_JuggedSpikes = sSpellMgr->GetSpellInfo(eSpells::JaggedSpikes);
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_ProcEventInfo.GetProcTarget();
            if (!l_DamageInfo || !l_JuggedSpikes)
                return;

            int32 l_Dmg = l_DamageInfo->GetAmount();
            l_Dmg = CalculatePct(l_Dmg, l_JuggedSpikes->Effects[EFFECT_0].BasePoints);

            l_Caster->CastCustomSpell(l_Target, eSpells::JaggedSpikesDmg, &l_Dmg, NULL, NULL, true);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dh_jagged_spikes_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_jagged_spikes_AuraScript();
    }
};

/// Last Update 7.1.5 - 23038
/// Razor Spikes - 209400
class spell_dh_razor_spikes : public SpellScriptLoader
{
    public:
        spell_dh_razor_spikes() : SpellScriptLoader("spell_dh_razor_spikes")
        {}

        enum eSpells
        {
            SpellRazorSpikesSlow    = 210003,
            SpellDemonSpikesBuff    = 203819,
        };

        class spell_dh_razor_spikes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_razor_spikes_AuraScript);

            void HandleOnProc(ProcEventInfo & p_Proc)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (Unit* l_Target = p_Proc.GetActionTarget())
                    l_Owner->CastSpell(l_Target, eSpells::SpellRazorSpikesSlow, true);
            }

            bool HandleOnCheckEffectProc(AuraEffect const* /**/, ProcEventInfo & p_Proc)
            {
                return true;
            }

            bool HandleCheckProc(ProcEventInfo & /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return false;

                Unit* l_Owner = GetUnitOwner();

                return l_Owner->HasAura(eSpells::SpellDemonSpikesBuff);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_razor_spikes_AuraScript::HandleCheckProc);
                DoCheckEffectProc += AuraCheckEffectProcFn(spell_dh_razor_spikes_AuraScript::HandleOnCheckEffectProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
                OnProc += AuraProcFn(spell_dh_razor_spikes_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_razor_spikes_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Last Resort - 209258
class spell_dh_last_resort : public SpellScriptLoader
{
    public:
        spell_dh_last_resort() : SpellScriptLoader("spell_dh_last_resort") { }

        class spell_dh_last_resort_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_last_resort_AuraScript);

            enum eSpells
            {
                Metamorphosis = 187827,
                UncontainedFel = 209261
            };

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (!GetTarget())
                    return;

                if (Player* l_Target = GetTarget()->ToPlayer())
                {
                    if (p_DmgInfo.GetAmount() >= l_Target->GetHealth() && p_DmgInfo.GetAmount() <= (l_Target->GetMaxHealth() * 2) && !l_Target->HasAura(eSpells::UncontainedFel))
                    {
                        l_Target->CastSpell(l_Target, eSpells::Metamorphosis, true);
                        l_Target->CastSpell(l_Target, eSpells::UncontainedFel, true);
                        p_AbsorbAmount = p_DmgInfo.GetAmount() + 1;
                        l_Target->SetHealth(l_Target->CountPctFromMaxHealth(30));
                    }
                    else
                        p_AbsorbAmount = 0;
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dh_last_resort_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_last_resort_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Nether Bond - 207810
class spell_dh_nether_bond : public SpellScriptLoader
{
    public:
        spell_dh_nether_bond() : SpellScriptLoader("spell_dh_nether_bond") { }

        class spell_dh_nether_bond_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_nether_bond_AuraScript);

            enum eSpells
            {
                NetherBondCaster = 207811
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::NetherBondCaster);
                l_Caster->CastSpell(l_Caster, eSpells::NetherBondCaster, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_nether_bond_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_nether_bond_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Nether Bond (periodic) - 207811
class spell_dh_nether_bond_periodic : public SpellScriptLoader
{
    public:
        spell_dh_nether_bond_periodic() : SpellScriptLoader("spell_dh_nether_bond_periodic") { }

        class spell_dh_nether_bond_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_nether_bond_periodic_AuraScript);

            enum eSpells
            {
                NetherBondAura = 207810,
                NetherBondHealDamage = 207812
            };

            void SplitHp(Unit* p_Unit1, Unit* p_Unit2)
            {
                uint64 l_TotalHealth = p_Unit1->GetHealth() + p_Unit2->GetHealth();
                uint64 l_TotalMaxHealth = p_Unit1->GetMaxHealth() + p_Unit2->GetMaxHealth();

                float l_PctHealth1 = (float)p_Unit1->GetMaxHealth() / ((float)l_TotalMaxHealth / 100.0f);
                float l_PctHealth2 = (float)p_Unit2->GetMaxHealth() / ((float)l_TotalMaxHealth / 100.0f);

                uint32 l_Health1 = CalculatePct(l_TotalHealth, l_PctHealth1);
                uint32 l_Health2 = CalculatePct(l_TotalHealth, l_PctHealth2);

                int32 l_DiffActualLife1 = l_Health1 - p_Unit1->GetHealth();
                int32 l_DiffActualLife2 = l_Health2 - p_Unit2->GetHealth();

                if (l_DiffActualLife1 > 0)
                    p_Unit2->CastCustomSpell(p_Unit1, eSpells::NetherBondHealDamage, NULL, &l_DiffActualLife1, NULL, true);
                else
                {
                    l_DiffActualLife1 *= -1;
                    p_Unit2->CastCustomSpell(p_Unit1, eSpells::NetherBondHealDamage, &l_DiffActualLife1, NULL, NULL, true);
                }

                if (l_DiffActualLife2 > 0)
                    p_Unit1->CastCustomSpell(p_Unit2, eSpells::NetherBondHealDamage, NULL, &l_DiffActualLife2, NULL, true);
                else
                {
                    l_DiffActualLife2 *= -1;
                    p_Unit1->CastCustomSpell(p_Unit2, eSpells::NetherBondHealDamage, &l_DiffActualLife2, NULL, NULL, true);
                }
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                std::list<Unit*> l_Party;

                l_Caster->GetRaidMembers(l_Party);

                if (l_Party.empty())
                    return;

                for (Unit* l_Unit : l_Party)
                {
                    if (l_Unit == nullptr)
                        continue;

                    if (l_Unit->HasAura(eSpells::NetherBondAura, l_Caster->GetGUID()))
                    {
                        if (l_Caster->GetMaxHealth() > l_Unit->GetMaxHealth())
                            SplitHp(l_Caster, l_Unit);
                        else
                            SplitHp(l_Unit, l_Caster);
                        return;
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_nether_bond_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_nether_bond_periodic_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Eye Beam - 198030
class spell_dh_eye_beam_proc : public SpellScript
{
    PrepareSpellScript(spell_dh_eye_beam_proc);

    enum eSpells
    {
        AnguishOfTheDeceiver = 201473,
        AnguishTrigger = 202443
    };

    void HandleAfterHit()
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        Unit* l_Target = GetHitUnit();
        if (!l_Target)
            return;

        if (l_Caster->HasAura(eSpells::AnguishOfTheDeceiver))
            l_Caster->DelayedCastSpell(l_Target, eSpells::AnguishTrigger, true, 1);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_dh_eye_beam_proc::HandleAfterHit);
    }
};

/// Last Update 7.3.5 build 26365
/// Eye Beam - 198013
class spell_dh_eye_beam : public SpellScriptLoader
{
    public:
        spell_dh_eye_beam() : SpellScriptLoader("spell_dh_eye_beam") { }

        enum eSpells
        {
            Metamorphosis   = 162264,
            Demonic         = 213410,
            BlindFury       = 203550,
            T21Havoc4P      = 251769,
            BetrayersFury   = 252165
        };

        class spell_dh_eye_beam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_eye_beam_AuraScript);

            uint32 m_RegenAmount = 0;

            bool Load() override
            {
                if (Unit* l_Caster = GetCaster())
                    if (AuraEffect const* l_Effect = l_Caster->GetAuraEffect(eSpells::BlindFury, SpellEffIndex::EFFECT_2))
                        m_RegenAmount = l_Effect->GetAmount();

                return true;
            }

            void HandlePeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_RegenAmount > 0 && p_AuraEffect->GetAmplitude())
                {
                    int32 l_Amount = m_RegenAmount / 5 * 3;
                    l_Amount /= std::max(p_AuraEffect->GetTotalTicks(), 1);
                    l_Caster->EnergizeBySpell(l_Caster, eSpells::BlindFury, l_Amount, Powers::POWER_FURY);
                }
            }

            void HandleOnRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::T21Havoc4P))
                    return;

                l_Caster->DelayedCastSpell(l_Caster, eSpells::BetrayersFury, true, 1);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_eye_beam_AuraScript::HandlePeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_eye_beam_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_eye_beam_AuraScript();
        }

        class spell_dh_eye_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_eye_beam_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Owner = GetCaster();
                if (!l_Owner)
                    return;

                if (l_Owner->HasAura(eSpells::Demonic))
                {
                    Aura* l_Aura = l_Owner->GetAura(eSpells::Metamorphosis);

                    if (l_Aura)
                    {
                        if (!l_Aura->GetCustomData())
                        {
                            l_Aura->SetDuration(l_Aura->GetDuration() + 10 * IN_MILLISECONDS);
                            l_Aura->SetCustomData(1); /// +10 seconds have been added
                        }
                        else
                        {
                            if (l_Aura->GetDuration() < 10 * IN_MILLISECONDS)
                            {
                                l_Aura->Remove();

                                if (Aura* l_NewAura = l_Owner->AddAura(eSpells::Metamorphosis, l_Owner))
                                    l_NewAura->SetDuration(10 * IN_MILLISECONDS);
                            }
                        }
                    }
                    else
                    {
                        if ((l_Aura = l_Owner->AddAura(eSpells::Metamorphosis, l_Owner)))
                            l_Aura->SetDuration(10 * IN_MILLISECONDS);
                    }
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_dh_eye_beam_SpellScript::HandleBeforeCast); ///< Checked retail: Meta is casted at the BEGGINING of Eye beam, for 10 sec instead of 8 (cuz eye beam lasts 2sec) so that eye beam benefits from meta Haste bonus. this induces one of the visual issue, but not all of them :c
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_eye_beam_SpellScript();
        }
};

/// Last Update 7.1.5
/// Blur - 212800
class spell_dh_blur_aura : public SpellScriptLoader
{
public:
    spell_dh_blur_aura() : SpellScriptLoader("spell_dh_blur_aura") { }

    class spell_dh_blur_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_blur_aura_AuraScript);

        enum eSpells
        {
            DesperateInstincts = 205411,
            Blur = 212800
        };

        void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            if (!l_Caster->HasAura(eSpells::DesperateInstincts))
                return;

            AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Blur, EFFECT_2);
            if (l_AuraEffect == nullptr)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DesperateInstincts);
            if (!l_SpellInfo)
                return;

            p_Amount += (l_AuraEffect->GetAmount() + CalculatePct(l_AuraEffect->GetAmount(), l_SpellInfo->Effects[EFFECT_2].BasePoints));
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_blur_aura_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_blur_aura_AuraScript();
    }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Soul Fragments (VDH Soul tracker spell) - 203981
class spell_soul_fragments_tracker : public SpellScriptLoader
{
    public:
        spell_soul_fragments_tracker() : SpellScriptLoader("spell_soul_fragments_tracker") { }

        class spell_soul_fragments_tracker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_soul_fragments_tracker_SpellScript);

            SpellCastResult CheckSpec()
            {

                Unit* l_Caster = GetCaster();
                if (l_Caster && l_Caster->IsPlayer() && l_Caster->ToPlayer()->GetActiveSpecializationID() == SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE)
                    return SPELL_CAST_OK;

                return SPELL_FAILED_DONT_REPORT;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_soul_fragments_tracker_SpellScript::CheckSpec);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_soul_fragments_tracker_SpellScript;
        }

        class spell_soul_fragments_tracker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_soul_fragments_tracker_AuraScript);

            enum eSpells
            {
                GreaterSoulFragment = 203795,
                LesserSoulFragment  = 204255
            };

            void OnUpdate(uint32 p_Diff) ///< This thing is added because sometimes, the tracker is off (souls spawning undermap, several souls being consumed in 1 update, etc)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(m_scriptSpellId);
                if (!l_Aura)
                    return;

                std::list<AreaTrigger*> l_ATList;
                l_Caster->GetAreaTriggerList(l_ATList, eSpells::GreaterSoulFragment);
                l_Caster->GetAreaTriggerList(l_ATList, eSpells::LesserSoulFragment);
                if (l_ATList.empty())
                    l_Caster->RemoveAura(l_Aura);
                else
                {
                    uint32 l_Souls = l_ATList.size();
                    l_Aura->SetStackAmount(l_Souls);
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_soul_fragments_tracker_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_soul_fragments_tracker_AuraScript();
        }
};


/// Last Update 7.1.5 - 23420
/// Called By Consume Soul - 203794 228532 210047 178963
/// Called For Feed the Demon - 218612
/// Called For Painbringer - 207387
class spell_dh_consume_soul : public SpellScriptLoader
{
    public:
        spell_dh_consume_soul() : SpellScriptLoader("spell_dh_consume_soul") { }

        enum eSpells
        {
            FeedTheDemon        = 218612,
            DemonSpikes         = 203720,
            Painbringer         = 207387,
            PainbringerAura     = 212988,
            PainbringerVisual   = 225413,
            FeastOnTheSouls     = 201468,
            EyeBeam             = 198013,
            ChaosNova           = 179057,
            SoulBarrier         = 227225
        };

        class spell_dh_consume_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_consume_soul_SpellScript);

            void HandleAfterCast()
            {
                SpellInfo const* l_SoulBarrier = sSpellMgr->GetSpellInfo(eSpells::SoulBarrier);
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::FeedTheDemon))
                {
                    if (SpellInfo const* l_DemonSpikes = sSpellMgr->GetSpellInfo(eSpells::DemonSpikes))
                        l_Player->ReduceChargeCooldown(l_DemonSpikes->ChargeCategoryEntry, 1 * IN_MILLISECONDS);
                }

                if (l_Player->HasAura(eSpells::Painbringer))
                    l_Player->CastSpell(l_Player, eSpells::PainbringerAura, true);

                if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::FeastOnTheSouls, EFFECT_0))
                {
                    l_Player->ReduceSpellCooldown(eSpells::EyeBeam, l_AuraEffect->GetAmount());
                    l_Player->ReduceSpellCooldown(eSpells::ChaosNova, l_AuraEffect->GetAmount());
                }

                if (l_SoulBarrier)
                {
                    if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::SoulBarrier, EFFECT_0))
                    {
                        float l_HealthPct = l_Player->GetHealthPct() / 100.f;
                        int32 l_Amount = (l_SoulBarrier->Effects[EFFECT_1].BasePoints / 100.f) * l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * l_HealthPct;
                        l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() + l_Amount);
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dh_consume_soul_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_consume_soul_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Fracture - 209795
class spell_dh_fracture : public SpellScriptLoader
{
    public:
        spell_dh_fracture() : SpellScriptLoader("spell_dh_fracture") { }

        class spell_dh_fracture_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fracture_SpellScript);

            enum eSpells
            {
                SpawnLesserSoul = 226258
            };

            void HandleHitTarget(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpawnLesserSoul, true);
                l_Caster->CastSpell(l_Target, eSpells::SpawnLesserSoul, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_fracture_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fracture_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Soul Barrier - 227225
class spell_dh_soul_barrier : public SpellScriptLoader
{
    public:
        spell_dh_soul_barrier() : SpellScriptLoader("spell_dh_soul_barrier") { }

        class spell_dh_soul_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_soul_barrier_AuraScript);

            enum eSpells
            {
                SoulFragment    = 203795,
                SoulFragment2   = 204255
            };

            int32 m_MinAmount = 0;

            bool Load() override
            {
                if (Unit* l_Caster = GetCaster())
                {
                    auto l_SpellInfo = GetSpellInfo();
                    m_MinAmount = (l_SpellInfo->Effects[SpellEffIndex::EFFECT_2].BasePoints / 100) * l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
                }

                return true;
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<AreaTrigger*> l_SoulFragments;
                l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment);
                l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment2);

                l_SoulFragments.remove_if([l_SpellInfo, l_Caster](AreaTrigger* p_At) -> bool
                {
                    if (l_Caster->GetDistance(p_At) <= l_SpellInfo->Effects[EFFECT_3].BasePoints)
                        return false;

                    return true;
                });

                float l_HealthPct = l_Caster->GetHealthPct() / 100.f;

                p_Amount *= l_HealthPct;
                p_Amount += l_SoulFragments.size() * ((GetSpellInfo()->Effects[EFFECT_1].BasePoints / 100.f) * l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * l_HealthPct);

                l_Caster->AddDelayedEvent([l_SpellInfo, l_Caster]() -> void
                {
                    std::list<AreaTrigger*> l_SoulFragments;
                    l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment);
                    l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment2);

                    for (AreaTrigger* l_SoulFragment : l_SoulFragments)
                    {
                        if (l_SoulFragment == nullptr)
                            continue;

                        if (l_Caster->GetDistance(l_SoulFragment) <= l_SpellInfo->Effects[EFFECT_3].BasePoints)
                            l_SoulFragment->CastAction();
                    }
                }, 1);
            }

            void OnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                /// Absorb shield cannot go below m_MinAmount, if that's the case, don't absorb
                int32 l_RealAbsorb = std::max(0, p_AurEff->GetAmount() - m_MinAmount);

                p_AbsorbAmount = std::min((int32)p_AbsorbAmount, l_RealAbsorb);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_soul_barrier_AuraScript::CalculateAmount, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dh_soul_barrier_AuraScript::OnAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_soul_barrier_AuraScript();
        }
};

/// Last Update 7.1.5 - 23420
/// PainBringer 212988
class spell_dh_painbringer : public SpellScriptLoader
{
    public:
        spell_dh_painbringer() : SpellScriptLoader("spell_dh_painbringer") { }

        class spell_dh_painbringer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_painbringer_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                m_AapplyTimes.push_back(getMSTime());
            }

            void OnUpdate(uint32 p_Diff, AuraEffect* p_AurEff)
            {
                if (m_UpdateTimer <= p_Diff)
                {
                    m_UpdateTimer = 1 * IN_MILLISECONDS;

                    // Leave the last stack
                    if (m_AapplyTimes.size() <= 1 || GetStackAmount() <= 1)
                        return;

                    if (m_AapplyTimes.front() + GetMaxDuration() < getMSTime())
                    {
                        m_AapplyTimes.pop_front();
                        SetStackAmount(GetStackAmount() - 1);
                    }
                }
                else
                {
                    m_UpdateTimer -= p_Diff;
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_painbringer_AuraScript::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectUpdate += AuraEffectUpdateFn(spell_dh_painbringer_AuraScript::OnUpdate, EFFECT_1, SPELL_AURA_DUMMY);
            }

        private:

            std::list<uint32> m_AapplyTimes;
            uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dh_painbringer_AuraScript();
        }
};

/// Last Update 7.1.5
/// Desperate Instincts - 205411
class spell_dh_desperate_instincts : public SpellScriptLoader
{
public:
    spell_dh_desperate_instincts() : SpellScriptLoader("spell_dh_desperate_instincts") { }

    enum eSpells
    {
        DesperateInstincts     = 205411,
        BlurSpell              = 198589,
        Blur                   = 212800
    };

    class spell_dh_desperate_instincts_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_desperate_instincts_AuraScript);

        uint32 m_castTime = 0;

        void OnUpdate(uint32 p_Diff)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = GetCaster()->ToPlayer();
            if (!l_Player || !l_Caster->HasAura(eSpells::DesperateInstincts))
                return;

            if (l_Caster->GetHealthPct() < 35 && !l_Player->HasSpellCooldown(eSpells::BlurSpell))
            {
                l_Caster->CastSpell(l_Caster, eSpells::Blur, true);
                l_Player->AddSpellCooldown(eSpells::BlurSpell, 0, 60 * IN_MILLISECONDS, true);
            }
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_dh_desperate_instincts_AuraScript::OnUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_desperate_instincts_AuraScript();
    }
};

/// Last Update 7.0.3 - 22293
/// Fel Eruption - 211881
class spell_dh_fel_eruption : public SpellScriptLoader
{
    public:
        spell_dh_fel_eruption() : SpellScriptLoader("spell_dh_fel_eruption") { }

        class spell_dh_fel_eruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fel_eruption_SpellScript);

            enum eSpells
            {
                FelEruptionDamage = 225102
            };

            bool m_Stunned = false;

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FelEruptionDamage, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dh_fel_eruption_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fel_eruption_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Fel Eruption Damage - 225102
class spell_dh_fel_eruption_damage : public SpellScriptLoader
{
    public:
        spell_dh_fel_eruption_damage() : SpellScriptLoader("spell_dh_fel_eruption_damage") { }

        class spell_dh_fel_eruption_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fel_eruption_damage_SpellScript);

            enum eSpells
            {
                FelEruption = 211881
            };

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FelEruption);
                if (!l_Caster || !l_Target)
                    return;

                auto l_IsPermanentImmune = [](Unit* p_Target, SpellInfo const* p_SpellInfo, SpellEffIndex p_Index) -> bool
                {
                    if (uint32 l_Mechanic = p_SpellInfo->Effects[p_Index].Mechanic)
                    {
                        SpellImmuneContainer const& l_MechanicList = p_Target->GetSpellImmuneMechanics();
                        for (SpellImmuneContainer::const_iterator l_Itr = l_MechanicList.begin(); l_Itr != l_MechanicList.end(); ++l_Itr)
                        {
                            SpellInfo const* l_MechanicSpellInfo = sSpellMgr->GetSpellInfo(l_Itr->second);

                            if (l_Itr->first != l_Mechanic)
                                continue;

                            if (!l_MechanicSpellInfo)
                                continue;

                            if (l_MechanicSpellInfo->GetMaxDuration() > 0)
                                continue;

                            return true;
                        }
                    }

                    return false;
                };

                if (l_IsPermanentImmune(l_Target, l_SpellInfo, EFFECT_0))
                    SetHitDamage(GetHitDamage() * 2);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_fel_eruption_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fel_eruption_damage_SpellScript();
        }
};

class PlayerScript_dh_learn_spells : public PlayerScript
{
    public:
        PlayerScript_dh_learn_spells() :PlayerScript("PlayerScript_dh_learn_spells") {}

        enum eMaps
        {
            Mardum = 1481
        };

        enum eQuests
        {
            PrevQuestDHAlly = 39691,
            PrevQuestDHHorde = 40976
        };

        enum eSpells
        {
            ActivingSpecialization = 200749
        };

        enum eTitles
        {
            IllidariTitle = 458
        };

        std::vector<uint32> m_SpellListHavoc = { 188499, 179057, 162794, 183752, 196718, 162243, 198013, 195072, 131347, 217832, 191427, 188501, 185123, 198793, 200175, 198013, 198589, 200749, 218386 };
        std::vector<uint32> m_SpellListVengeance = { 183752, 203720, 218256, 204021, 178740, 217832, 189110, 187827, 203782, 204596, 207684, 202137, 228477, 188501, 204157, 185245, 203513, 200175, 191427 };

        std::vector<uint32> m_QuestsToReward = { 40253, 40254, 40255 };

        void LearnSpells(Player* p_Player)
        {
            if (!p_Player->HasSpell(eSpells::ActivingSpecialization)) ///< Activating Specialization
                p_Player->learnSpell(eSpells::ActivingSpecialization, false, false, false);

            if (p_Player->m_homebindMapId == eMaps::Mardum)
            {
                /// New Dalaran
                WorldLocation l_NewHomeBind = WorldLocation(1220, -799.350464f, 4476.684570f, 735.013733f, 0.000255f);
                p_Player->SetHomebind(l_NewHomeBind, 8445);
            }

            bool l_MissingSpell = false;

            if (p_Player->GetActiveSpecializationID() == SpecIndex::SPEC_DEMON_HUNTER_HAVOC)
            {
                for (uint32 l_SpellID : m_SpellListHavoc)
                {
                    if (!p_Player->HasSpell(l_SpellID))
                    {
                        p_Player->learnSpell(l_SpellID, false, false, false, 0, true);
                        l_MissingSpell = true;
                    }
                }
            }
            else if (p_Player->GetActiveSpecializationID() == SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE)
            {
                for (uint32 l_SpellID : m_SpellListVengeance)
                {
                    if (!p_Player->HasSpell(l_SpellID))
                    {
                        p_Player->learnSpell(l_SpellID, false, false, false, 0, true);
                        l_MissingSpell = true;
                    }
                }
            }

            for (uint32 l_QuestID : m_QuestsToReward)
            {
                Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_QuestID);
                if (l_Quest && !p_Player->IsQuestRewarded(l_QuestID))
                {
                    p_Player->CompleteQuest(l_QuestID);
                    p_Player->RewardQuest(l_Quest, 0, p_Player, false);
                }
            }

            if (l_MissingSpell)
            {
                p_Player->RemoveSpecializationSpells();
                p_Player->LearnSpecializationSpells();

                uint32 l_QuestId = p_Player->getFaction() == TeamId::TEAM_ALLIANCE ? eQuests::PrevQuestDHAlly : eQuests::PrevQuestDHHorde;
                Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_QuestId);

                if (l_Quest && !p_Player->IsQuestRewarded(l_QuestId))
                {
                    p_Player->CompleteQuest(l_QuestId);
                    p_Player->RewardQuest(l_Quest, 0, p_Player, false);
                }
            }
        }

        void OnLogin(Player* p_Player) override
        {
            if (!(p_Player->getClass() == CLASS_DEMON_HUNTER))
                return;

            CharTitlesEntry const* l_TitleInfo = sCharTitlesStore.LookupEntry(eTitles::IllidariTitle);
            if (l_TitleInfo != nullptr)
            {
                if (!p_Player->HasTitle(l_TitleInfo))
                    p_Player->SetTitle(l_TitleInfo);
            }

            if (p_Player->GetMapId() != eMaps::Mardum)
                LearnSpells(p_Player);
        }

        void OnTeleport(Player* p_Player, SpellInfo const* /*p_Spell*/, bool) override
        {
            if (!(p_Player->getClass() == CLASS_DEMON_HUNTER))
                return;

            if (p_Player->GetMapId() != eMaps::Mardum)
                LearnSpells(p_Player);
        }
};

/// Last Update 7.3.2 Build 25549
/// Spirit Bomb - 247454
class spell_dh_spirit_bomb : public SpellScriptLoader
{
    public:
        spell_dh_spirit_bomb() : SpellScriptLoader("spell_dh_spirit_bomb") {}

        class spell_dh_spirit_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_spirit_bomb_AuraScript);

            enum eSpells
            {
                SoulFragment        = 203795,
                SoulFragment2       = 204255,
                SpiritBombDamage    = 226377,
                Frailty             = 247456,
                Painbringer         = 207387,
                PainbringerAura     = 212988
            };

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<AreaTrigger*> l_SoulFragments;
                l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment);
                l_Caster->GetAreaTriggerList(l_SoulFragments, eSpells::SoulFragment2);

                for (AreaTrigger* l_SoulFragment : l_SoulFragments)
                {
                    if (l_SoulFragment == nullptr)
                        continue;

                    if (l_Caster->GetDistance(l_SoulFragment) <= p_AuraEffect->GetAmount())
                    {
                        l_SoulFragment->CastAction();
                        ++m_FragmentsCount;
                    }
                }
            }

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_FragmentsCount > 0)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Frailty, true);
                    l_Caster->CastCustomSpell(eSpells::SpiritBombDamage, SpellValueMod::SPELLVALUE_BASE_POINT2, m_FragmentsCount, l_Caster, true);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_spirit_bomb_AuraScript::HandleAfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_spirit_bomb_AuraScript::HandleAfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }

        private:

            int32 m_FragmentsCount = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_spirit_bomb_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Spirit Bomb - 226377
class spell_dh_spirit_bomb_damage : public SpellScriptLoader
{
    public:
        spell_dh_spirit_bomb_damage() : SpellScriptLoader("spell_dh_spirit_bomb_damage") { }

        class spell_dh_spirit_bomb_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_spirit_bomb_damage_SpellScript);

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                if (Spell const* l_Spell = GetSpell())
                {
                    int32 l_Count = l_Spell->GetSpellValue(SpellValueMod::SPELLVALUE_BASE_POINT2);
                    if (l_Count > 0)
                        SetHitDamage(GetHitDamage() * l_Count);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_spirit_bomb_damage_SpellScript::HandleDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_spirit_bomb_damage_SpellScript;
        }
};

/// Last Update 7.3.2 Build 25549
/// Frailty - 247456
class spell_dh_frailty : public SpellScriptLoader
{
    public:
        spell_dh_frailty() : SpellScriptLoader("spell_dh_frailty") { }

        class spell_dh_frailty_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_frailty_Aurascript);

            bool HandleCheckProc(ProcEventInfo& p_ProcInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (l_DamageInfo == nullptr)
                    return false;

                Unit* l_Victim = l_DamageInfo->GetTarget();
                Unit* l_Attacker = l_DamageInfo->GetActor();
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr || l_Attacker == nullptr || l_Victim == nullptr)
                    return false;

                if (l_Attacker->GetGUID() != l_Caster->GetGUID())
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcInfos.GetDamageInfo();
                if (!l_Caster || l_DamageInfo == nullptr)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(GetSpellInfo()->Id, EFFECT_1))
                {
                    float l_HealPct = p_AurEff->GetAmount();
                    l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() + CalculatePct(l_DamageInfo->GetAmount(), l_HealPct));
                }

            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_frailty_Aurascript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_dh_frailty_Aurascript::OnProc, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_frailty_Aurascript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Frailty (heal) - 227255
class spell_dh_frailty_heal : public SpellScriptLoader
{
    public:
        spell_dh_frailty_heal() : SpellScriptLoader("spell_dh_frailty_heal") { }

        class spell_dh_frailty_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_frailty_heal_SpellScript);

            enum eSpells
            {
                Frailty = 247456
            };

            void HandleCharge(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Frailty, EFFECT_1);
                if (l_AuraEffect == nullptr)
                    return;

                SetHitHeal(l_AuraEffect->GetAmount());
                l_AuraEffect->ChangeAmount(0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_frailty_heal_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_frailty_heal_SpellScript;
        }
};

/// Last Update 7.1.5
/// Chaos Strike - 206475
class spell_dh_chaos_strike_damage : public SpellScriptLoader
{
    public:
        spell_dh_chaos_strike_damage() : SpellScriptLoader("spell_dh_chaos_strike_damage") { }

        class spell_dh_chaos_strike_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_chaos_strike_damage_AuraScript);

            enum eSpells
            {
                ChaosStrikeMainHand     = 222031,
                ChaosStrikeOffHand      = 199547,
                AnnihilationMainHand    = 227518,
                AnnihilationOffHand     = 201428,
                ChaosCleaveDamage       = 236237
            };

            void HandleOnProc(AuraEffect const* p_AuraEffect, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (l_DamageInfo == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                int32 l_Damage = l_DamageInfo->GetAmount();
                l_Damage = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_1].BasePoints);

                l_Caster->CastCustomSpell(l_Caster, eSpells::ChaosCleaveDamage, &l_Damage, nullptr, nullptr, true);
            }

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::ChaosStrikeMainHand && l_SpellInfo->Id != ChaosStrikeOffHand &&
                    l_SpellInfo->Id != eSpells::AnnihilationMainHand && l_SpellInfo->Id != AnnihilationOffHand)
                    return false;

                return true;
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_chaos_strike_damage_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
                DoCheckProc += AuraCheckProcFn(spell_dh_chaos_strike_damage_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_chaos_strike_damage_AuraScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Chaos Strike - 197125
class spell_dh_chaos_strike : public SpellScriptLoader
{
    public:
        spell_dh_chaos_strike() : SpellScriptLoader("spell_dh_chaos_strike") { }

        class spell_dh_chaos_strike_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_chaos_strike_Aurascript);

            enum eSpells
            {
                ChaosStrikeDamage           = 222031,
                ChaosStrikeDamageOffHand    = 199547,
                AnihilationDamage           = 227518,
                AnihilationDamageOffHand    = 201428
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                DamageInfo const* l_DamageInfo = p_ProcInfos.GetDamageInfo();

                if (l_DamageInfo == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr)
                    return;

                if (!(p_ProcInfos.GetHitMask() & PROC_EX_CRITICAL_HIT))  /// Either both hits crit, or they dont.
                    return;

                if (l_SpellInfo->Id != eSpells::ChaosStrikeDamage &&
                    l_SpellInfo->Id != eSpells::AnihilationDamage)
                    return;

                l_Caster->EnergizeBySpell(l_Caster, l_SpellInfo->Id, p_AurEff->GetAmount(), POWER_FURY);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_chaos_strike_Aurascript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_chaos_strike_Aurascript();
        }
};

/// Last Update 7.3.5 - Build 25996
/// Chaos Strike - 162794, Annihilation - 201427
class spell_dh_chaotic_onslaught : public SpellScriptLoader
{
public:
    spell_dh_chaotic_onslaught() : SpellScriptLoader("spell_dh_chaotic_onslaught") { }

    class spell_dh_spell_dh_chaotic_onslaught_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dh_spell_dh_chaotic_onslaught_SpellScript);

        enum eSpells
        {
            ChaoticOnslaught    = 238117,
            ChaosStrike         = 162794,
            Annihilation        = 201427
        };

        void HandleHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();

            if (!l_Caster || !l_Target)
                return;

            if (!l_Caster->HasAura(eSpells::ChaoticOnslaught))
                return;

            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ChaoticOnslaught))
                if (roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                {
                    if (m_scriptSpellId == eSpells::ChaosStrike)
                        l_Caster->CastSpell(l_Target, eSpells::ChaosStrike, true);
                    else
                        l_Caster->CastSpell(l_Target, eSpells::Annihilation, true);
                }
        }

        void Register() override
        {
            AfterHit += SpellHitFn(spell_dh_spell_dh_chaotic_onslaught_SpellScript::HandleHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dh_spell_dh_chaotic_onslaught_SpellScript();
    }
};

/// Last Update 7.0.3 - 22293
/// Chaos Blades - 211048
class spell_dh_demon_chaos_blades : public SpellScriptLoader
{
    public:
        spell_dh_demon_chaos_blades() : SpellScriptLoader("spell_dh_demon_chaos_blades") { }

        class spell_dh_demon_chaos_blades_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_demon_chaos_blades_AuraScript);

            enum eSpells
            {
                MasteryDemonicPresence = 185164
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::MasteryDemonicPresence, EFFECT_0);

                if (l_AuraEffect == nullptr)
                    return;

                p_Amount = l_AuraEffect->GetAmount();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_demon_chaos_blades_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_demon_chaos_blades_AuraScript();
        }
};

/// Last Update 7.0.3 - 22045
/// Fel Lance - 206966
class spell_dh_fel_lance : public SpellScriptLoader
{
    public:
        spell_dh_fel_lance() : SpellScriptLoader("spell_dh_fel_lance") { }

        class spell_dh_fel_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fel_lance_SpellScript);

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                int32 l_Damage = l_Target->GetMaxHealth() / 100.0f * l_SpellInfo->Effects[p_EffIndex].BasePoints;
                l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, l_SpellInfo, l_Damage, DamageEffectType::SPELL_DIRECT_DAMAGE, p_EffIndex);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_fel_lance_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fel_lance_SpellScript;
        }
};

/// Last Update 7.0.3 Build 22522
/// Inner Demons - 201471
class spell_dh_inner_demons : public SpellScriptLoader
{
    public:
        spell_dh_inner_demons() : SpellScriptLoader("spell_dh_inner_demons") { }

        class spell_dh_inner_demons_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_inner_demons_AuraScript);

            enum eSpells
            {
                ChaosStrikeLeft     = 222031,
                ChaosStrikeRight    = 199547,
                AnihiliationLeft    = 227518,
                AnihiliationRight   = 201428
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::ChaosStrikeLeft && l_SpellInfo->Id != ChaosStrikeRight &&
                    l_SpellInfo->Id != eSpells::AnihiliationLeft && l_SpellInfo->Id != AnihiliationRight)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_inner_demons_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_inner_demons_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Sigil of Chains - 204843 root effect
class spell_dh_sigil_of_chains_root : public SpellScriptLoader
{
    public:
        spell_dh_sigil_of_chains_root() : SpellScriptLoader("spell_dh_sigil_of_chains_root") { }

        enum eSpells : uint32
        {
            SephuzsSecret = 234867,
            SephuzsSecret2 = 236763,
            SephuzsSecret3 = 208051,
            SephuzsSecretProc = 208052,
            SephuzsSecretMarker = 226262
        };

        class script_impl : public SpellScript
        {
            PrepareSpellScript(script_impl);

            void HandleEffect(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::SephuzsSecret) &&
                    !l_Caster->HasAura(eSpells::SephuzsSecret2) &&
                    !l_Caster->HasAura(eSpells::SephuzsSecret3))
                    return;

                if (l_Caster->HasAura(eSpells::SephuzsSecretMarker))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SephuzsSecretProc, true);
                l_Caster->CastSpell(l_Caster, eSpells::SephuzsSecretMarker, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(script_impl::HandleEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new script_impl();
        }
};

/// Last Update 7.3.5 Build 26124
/// Sigil of Chains - 204834
class spell_dh_sigil_of_chains: public SpellScriptLoader
{
    public:
        spell_dh_sigil_of_chains() : SpellScriptLoader("spell_dh_sigil_of_chains") { }

        enum Spells : uint32
        {
            RootEffect      = 204843,
            PullEffect      = 208674,
        };

        class script_impl : public SpellScript
        {
            PrepareSpellScript(script_impl);

            void HandleEffect(SpellEffIndex /*p_EffIndex*/)
            {
                Position l_Pos;
                GetCaster()->GetPosition(&l_Pos);
                if (GetExplTargetDest())
                    l_Pos = { GetExplTargetDest()->GetPositionX(), GetExplTargetDest()->GetPositionY(), GetExplTargetDest()->GetPositionZ() };

                GetHitUnit()->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), Spells::PullEffect, true);
                GetCaster()->CastSpell(GetHitUnit(), Spells::RootEffect, true);

                if (GetHitUnit()->ToCreature() && !GetHitUnit()->ToCreature()->IsDungeonBoss())
                {
                    Spell* l_CurrentSpell = GetHitUnit()->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
                    if (!l_CurrentSpell)
                    {
                        l_CurrentSpell = GetHitUnit()->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL);
                        if (!l_CurrentSpell)
                        {
                            l_CurrentSpell = GetHitUnit()->GetCurrentSpell(CURRENT_GENERIC_SPELL);
                            if (!l_CurrentSpell)
                                l_CurrentSpell = GetHitUnit()->GetCurrentSpell(CURRENT_MELEE_SPELL);
                        }
                    }

                    if (l_CurrentSpell)
                        l_CurrentSpell->cancel();
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(script_impl::HandleEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new script_impl();
        }
};

/// Anguish of the Deceiver Artifact Trait - 201473, called by Angiush - 202443
/// Updated as of 7.0.3 - 22522
class spell_dh_angiush_of_the_deceiver : public SpellScriptLoader
{
    public:
        spell_dh_angiush_of_the_deceiver() : SpellScriptLoader("spell_dh_angiush_of_the_deceiver") { }

        class aura_impl : public AuraScript
        {
            PrepareAuraScript(aura_impl);

            enum eSpells
            {
                AngiushDamage = 202446
            };

            void HandleDamageOnRemove(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AngiushDamage);
                if (!l_SpellInfo || !l_Player)
                    return;

                int32 l_Damage = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * l_SpellInfo->Effects[EFFECT_0].AttackPowerMultiplier * GetStackAmount();
                l_Caster->CastCustomSpell(l_Target, eSpells::AngiushDamage, &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(aura_impl::HandleDamageOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new aura_impl();
        }
};

/// Last Update 7.3.5 Build 26124
/// Awaken the Demon Within - 205598
class spell_dh_awaken_the_demon_within : public SpellScriptLoader
{
    public:
        spell_dh_awaken_the_demon_within() : SpellScriptLoader("spell_dh_awaken_the_demon_within") { }

        class spell_dh_awaken_the_demon_within_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_awaken_the_demon_within_AuraScript);

            enum eSpells
            {
                Metamorphosis           = 162264,
                MetamorphosisCooldown   = 207128
            };

            void HandleAfterProc(AuraEffect const*, ProcEventInfo& p_ProcInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (l_Caster->HealthBelowPct(l_SpellInfo->Effects[EFFECT_1].BasePoints))
                    if (Aura* l_MetaAura = l_Caster->GetAura(eSpells::Metamorphosis))
                        if (l_MetaAura->GetDuration() < l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS)
                            l_MetaAura->SetDuration(l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);

                if (l_Caster->HasAura(eSpells::MetamorphosisCooldown))
                    return;

                if (Aura* l_MetaAura = l_Caster->GetAura(eSpells::Metamorphosis))
                    if (l_MetaAura->GetDuration() > l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS)
                        return;

                if (l_Caster->HealthBelowPct(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                {
                    if (Aura* l_Aura = l_Caster->AddAura(eSpells::Metamorphosis, l_Caster))
                    {
                        l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
                        l_Caster->CastSpell(l_Caster, eSpells::MetamorphosisCooldown, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectProc += AuraEffectProcFn(spell_dh_awaken_the_demon_within_AuraScript::HandleAfterProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_awaken_the_demon_within_AuraScript;
        }
};

/// Last Update 7.0.3
/// Unending Hatred 205603
class spell_dh_unending_hatred : public SpellScriptLoader
{
    public:
        spell_dh_unending_hatred() : SpellScriptLoader("spell_dh_unending_hatred") { }

        class spell_dh_unending_hatredAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_unending_hatredAuraScript);

            enum eSpells
            {
                UnendingHatredEnergy= 207528
            };

            bool HandleDoCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return false;

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();

                if (!l_DamageInfo)
                    return false;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_Spell)
                    return false;

                if (SpellInfo const* l_SpellInfo = l_Spell->GetSpellInfo())
                {
                    if (l_SpellInfo->DmgClass != SpellDmgClass::SPELL_DAMAGE_CLASS_MAGIC)
                        return false;

                    return true;
                }

                return false;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {/*
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_Energy = (l_SpellInfo->Effects[EFFECT_0].BasePoints);
                l_Caster->CastCustomSpell(l_Caster, eSpells::UnendingHatredEnergy, &l_Energy, nullptr, nullptr, true);*/
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dh_unending_hatredAuraScript::HandleOnProc);
                DoCheckProc += AuraCheckProcFn(spell_dh_unending_hatredAuraScript::HandleDoCheckProc);
            }
        };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_unending_hatredAuraScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Called By Fel Barrage Damage - 211052
class spell_dh_fel_barrage_damage : public SpellScriptLoader
{
    public:
        spell_dh_fel_barrage_damage() : SpellScriptLoader("spell_dh_fel_barrage_damage") { }

        class spell_dh_fel_barrage_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fel_barrage_damage_SpellScript);

            enum eSpells
            {
                FelBarrageCharges   = 211053
            };

            void HandleOnHit(SpellEffIndex)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FelBarrageCharges);
                if (!GetCaster() || !l_SpellInfo)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (int32 l_Multi = GetSpell()->GetSpellValue(SpellValueMod::SPELLVALUE_BASE_POINT1))
                    {
                        if (uint32 l_Charges = l_Player->GetMaxCharges(l_SpellInfo->ChargeCategoryEntry))
                            SetHitDamage((GetHitDamage() / l_SpellInfo->GetMaxTicks()) * l_Multi);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_fel_barrage_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fel_barrage_damage_SpellScript();
        }
};

///Last Update: 7.3.5 23635
///Called by Eye Of Leotheras - 206649
class spell_eye_of_leotheras : public SpellScriptLoader
{
    public:
        spell_eye_of_leotheras() : SpellScriptLoader("spell_eye_of_leotheras") { }

        class spell_eye_of_leotherasAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eye_of_leotherasAuraScript);

            enum eSpells
            {
                EyeOfLeotheras = 206649,
                EyeOfLeotherasDmg = 206650
            };

            bool HandleOnCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_damageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_damageInfo)
                    return false;

                const Spell* l_Spell = l_damageInfo->GetSpell();
                if (!l_Spell)
                    return false;

                if (l_Spell->IsTriggered())
                    return false;

                if (SpellInfo const* l_SpellInfo = l_Spell->GetSpellInfo())
                {
                    if (l_SpellInfo->DmgClass != SpellDmgClass::SPELL_DAMAGE_CLASS_MAGIC)
                        return false;

                    return l_SpellInfo->IsDamageSpell();
                }

                return false;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                SpellInfo const* l_DamageSpellInfo = sSpellMgr->GetSpellInfo(eSpells::EyeOfLeotherasDmg);
                if (!l_DamageSpellInfo)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = GetUnitOwner();
                if (l_Owner == nullptr)
                    return;

                int32 l_Damage = CalculatePct(l_Owner->GetMaxHealth(), l_DamageSpellInfo->Effects[EFFECT_0].BasePoints);
                l_Damage = l_Owner->SpellDamageBonusTaken(l_Caster, l_DamageSpellInfo, l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

                l_Caster->CastCustomSpell(l_Owner, eSpells::EyeOfLeotherasDmg, &l_Damage, nullptr, nullptr, true);
                GetAura()->RefreshTimers();
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_eye_of_leotherasAuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_eye_of_leotherasAuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_eye_of_leotherasAuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Vengeful Retreat - 198793
/// Called for Glimpse - 203468
class spell_dh_glimpse : public SpellScriptLoader
{
    public:
        spell_dh_glimpse() : SpellScriptLoader("spell_dh_glimpse") { }

        class spell_dh_glimpse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_glimpse_SpellScript);

            enum eSpells
            {
                Glimpse = 203468,
                Blur    = 212800
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Glimpse, EFFECT_0))
                {
                    if (Aura* l_Aura = l_Caster->AddAura(eSpells::Blur, l_Caster))
                        l_Aura->SetDuration(l_AuraEffect->GetAmount());
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_dh_glimpse_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_glimpse_SpellScript();
        }
};

/// Called for Rushing Vault - 205601
class spell_dh_rushing_vault : public SpellScriptLoader
{
    public:
        spell_dh_rushing_vault() : SpellScriptLoader("spell_dh_rushing_vault") { }

        class spell_dh_rushing_vault_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_rushing_vault_SpellScript);

            enum eSpells
            {
                RushingVault    = 205601,
                FelRush         = 195072
            };

            void HandleOnCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::RushingVault))
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FelRush);
                    if (l_SpellInfo != nullptr)
                        l_Player->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_dh_rushing_vault_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_rushing_vault_SpellScript();
        }
};

/// Last Update: 7.0.3: 22522
/// Solitude - 211509
class spell_dh_solitude : public SpellScriptLoader
{
    public:
        spell_dh_solitude() : SpellScriptLoader("spell_dh_solitude") { }

        enum eSpells
        {
            SolituteEffect = 211510
        };

        class spell_dh_solitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_solitude_AuraScript);

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::SolituteEffect))
                    l_Caster->RemoveAura(eSpells::SolituteEffect);
            }


            void OnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->IsInDuel() && !l_Player->InArena() && !l_Player->InBattleground() && !l_Player->InRatedBattleGround())
                {
                    /// Honor talents work only in duel / arena / BG
                    if (l_Player->HasAura(eSpells::SolituteEffect))
                        l_Player->RemoveAura(eSpells::SolituteEffect);

                    return;
                }

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_scriptSpellId);
                if (!l_SpellInfo)
                    return;

                std::list<Player*> l_PlayerList;
                bool l_HasFriendlyInRange = false;
                l_Caster->GetPlayerListInGrid(l_PlayerList, l_SpellInfo->Effects[EFFECT_1].BasePoints);

                for (Player* l_Target : l_PlayerList)
                {
                    if (l_Target->IsFriendlyTo(l_Caster))
                    {
                        l_HasFriendlyInRange = true;
                        break;
                    }
                }

                if (!l_HasFriendlyInRange)
                    l_Caster->AddAura(eSpells::SolituteEffect, l_Caster);
                else
                    l_Caster->RemoveAura(SolituteEffect);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_solitude_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_solitude_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dh_solitude_AuraScript();
        }
};

/// Last Update: 7.1.5
/// Mana Break - 203704
class spell_dh_mana_break : public SpellScriptLoader
{
public:
    spell_dh_mana_break() : SpellScriptLoader("spell_dh_mana_break") { }

    class spell_dh_mana_break_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dh_mana_break_SpellScript);

        enum eSpells
        {
            ChaosBlade = 211048,
            Nemesis = 206491
        };

        void HandleDamage(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_scriptSpellId);
            if (!l_SpellInfo)
                return;

            /// 1% of the target's health per 2% of the target's missed mana with a 20% base
            float l_Pct = l_SpellInfo->Effects[EFFECT_0].BasePoints;

            if (l_Target->GetPower(POWER_MANA) > 0)
            {
                l_Pct += (100.0f - l_Target->GetPowerPct(POWER_MANA)) / l_SpellInfo->Effects[EFFECT_2].BasePoints;
                if (l_Pct > l_SpellInfo->Effects[EFFECT_3].BasePoints)
                    l_Pct = l_SpellInfo->Effects[EFFECT_3].BasePoints;
            }

            uint32 l_AdditionalDamage = uint32(CalculatePct(l_Target->GetMaxHealth(), l_Pct));

            if (AuraEffect* l_Effect = l_Caster->GetAuraEffect(eSpells::ChaosBlade, EFFECT_1))
                l_AdditionalDamage = AddPct(l_AdditionalDamage, l_Effect->GetAmount());

            if (AuraEffect* l_Effect = l_Target->GetAuraEffect(eSpells::Nemesis, EFFECT_0))
                l_AdditionalDamage = AddPct(l_AdditionalDamage, l_Effect->GetAmount());

            int32 l_Damage = GetHitDamage() + l_AdditionalDamage;

            l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dh_mana_break_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dh_mana_break_SpellScript();
    }
};

/// Last Update: 7.1.5 23420
/// Called by Prepared - 203650
class spell_dh_prepared : public SpellScriptLoader
{
    public:
        spell_dh_prepared() : SpellScriptLoader("spell_dh_prepared") { }

        class spell_dh_prepared_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_prepared_AuraScript);

            void OnTick(const AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_PreparedInfo = GetSpellInfo();
                if (!l_Caster || !l_PreparedInfo)
                    return;

                l_Caster->ModifyPower(Powers(l_PreparedInfo->Effects[EFFECT_0].MiscValue), p_AurEff->GetAmount());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_prepared_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_prepared_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Rage Of The Illidari - 201472
class spell_dh_rage_of_the_illidari : public SpellScriptLoader
{
    public:
        spell_dh_rage_of_the_illidari() : SpellScriptLoader("spell_dh_rage_of_the_illidari") { }

        class spell_dh_rage_of_the_illidari_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_rage_of_the_illidari_AuraScript);

            enum eSpells
            {
                FuryOfTheIllidari           = 201628,
                FuryOfTheIllidariOffhand    = 201789
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::FuryOfTheIllidari && l_SpellInfo->Id != eSpells::FuryOfTheIllidariOffhand))
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::RageOfTheIllidari) += l_DamageInfo->GetAmount();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dh_rage_of_the_illidari_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_rage_of_the_illidari_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Spectral Sight - 188501
class spell_dh_spectral_sight : public SpellScriptLoader
{
    public:
        spell_dh_spectral_sight() : SpellScriptLoader("spell_dh_spectral_sight") { }

        enum eSpells
        {
            SpectralSightDetection  = 202688,
            SpectralSightDummy      = 215725
        };

        class spell_dh_spectral_sight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_spectral_sight_AuraScript);

            void AfterRemove(AuraEffect const* l_AuraEffect, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::SpectralSightDetection);
                l_Caster->RemoveAura(eSpells::SpectralSightDummy);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_spectral_sight_AuraScript::AfterRemove, EFFECT_2, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_spectral_sight_AuraScript();
        }
};

///Last Update: 7.1.5 23420
///Called by Rain from Above - 206803
class spell_dh_rain_from_above : public SpellScriptLoader
{
    public:
        spell_dh_rain_from_above() : SpellScriptLoader("spell_dh_rain_from_above"){ }

        class spell_dh_rain_from_above_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_rain_from_above_SpellScript);

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasByType(AuraType::SPELL_AURA_MOD_ROOT);
                l_Caster->RemoveAurasByType(AuraType::SPELL_AURA_MOD_ROOT_2);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_dh_rain_from_above_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_rain_from_above_SpellScript();
        }
};

///Last Update: 7.3.5 23635
///Called by Rain from Above - 206959, 206804
class spell_dh_rain_from_above_check : public SpellScriptLoader
{
    public:
        spell_dh_rain_from_above_check() : SpellScriptLoader("spell_dh_rain_from_above_check") { }

        class spell_dh_rain_from_above_check_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_rain_from_above_check_AuraScript);

            enum eSpells
            {
                RainFromAbove       = 206803,
                RainFromAboveFall   = 206804,
                RainFromAboveSpell  = 206959
            };

            void OnUpdate(uint32 p_Diff)
            {
                if (m_StartCheckTimer >= p_Diff)
                {
                    m_StartCheckTimer -= p_Diff;
                    return;
                }

                m_StartCheckTimer = 0;

                Unit* l_Owner = GetUnitOwner();
                if (l_Owner == nullptr)
                    return;

                /// Remove 'Fall' if the owner is not falling.
                /// Remove 'Spell' if the owner is not falling and has no 'Fall' aura.
                if (!l_Owner->IsFalling() &&
                    (m_scriptSpellId == eSpells::RainFromAboveFall ||
                    (m_scriptSpellId == eSpells::RainFromAboveSpell && !l_Owner->HasAura(eSpells::RainFromAboveFall))))
                {
                    Remove();
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_dh_rain_from_above_check_AuraScript::OnUpdate);
            }

        private:
            uint32 m_StartCheckTimer = 1000;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_rain_from_above_check_AuraScript();
        }
};

/// Update to Legion 7.1.5 - build 23420
/// Kirel Narak - 210970
class spell_dh_kirel_narak : public SpellScriptLoader
{
    public:
        spell_dh_kirel_narak() : SpellScriptLoader("spell_dh_kirel_narak") { }

        class spell_dh_kirel_narak_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_kirel_narak_AuraScript);

            enum eSpells
            {
                AuraImmolation              = 178740,
                FieryBrand                  = 204021,
                AuraImmolationInstantDamage = 187727
            };

            bool CheckOnProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_ProcSpellInfo && l_ProcSpellInfo->Id == eSpells::AuraImmolationInstantDamage)
                    return true;
                return false;
            }

            void HandleOnProc(ProcEventInfo& /* p_EventInfo */)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player*  l_Player = l_Caster->ToPlayer();
                if (l_Player || l_Caster->HasAura(eSpells::AuraImmolation))
                    l_Player->ReduceSpellCooldown(eSpells::FieryBrand, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_kirel_narak_AuraScript::CheckOnProc);
                OnProc += AuraProcFn(spell_dh_kirel_narak_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_kirel_narak_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Delusions of Grandeur - 209354
class spell_dh_delusion_of_grandeur : public SpellScriptLoader
{
    public:
        spell_dh_delusion_of_grandeur() : SpellScriptLoader("spell_dh_delusion_of_grandeur") { }

        class spell_dh_delusion_of_grandeur_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_delusion_of_grandeur_AuraScript);

            enum eSpells
            {
                Metamorphosis = 191427
            };

            uint32 m_PowerCost = 0;

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo || !l_DamageInfo)
                    return;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Player* l_Player = l_Caster->ToPlayer();
                Spell const* l_ProcSpell = l_DamageInfo->GetSpell();
                if (!l_Player || !l_ProcSpellInfo || !l_ProcSpell)
                    return;

                m_PowerCost += l_ProcSpell->GetPowerCost(Powers::POWER_FURY);
                if (m_PowerCost < l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    return;

                l_Player->ReduceSpellCooldown(eSpells::Metamorphosis, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                m_PowerCost -= l_SpellInfo->Effects[EFFECT_1].BasePoints;
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dh_delusion_of_grandeur_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_delusion_of_grandeur_AuraScript();
        }
};

/// Last Update: 7.3.2 Build 25549
/// Called by Runemaster's Pauldrons - 210867
class spell_dh_runemasters_pauldrons : public SpellScriptLoader
{
    public:
        spell_dh_runemasters_pauldrons() : SpellScriptLoader("spell_dh_runemasters_pauldrons") { }

        class spell_dh_runemasters_pauldrons_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_runemasters_pauldrons_AuraScript);

            enum eSpells
            {
                Metamorphosis       = 187827,
                DemonSpikes         = 203720,
                EmpowerWards        = 218256,
                SigilOfFlame        = 204596,
                SigilOfMisery       = 207684,
                SigilOfSilence      = 202137,
                SigilOfChains       = 202138,
                SigilOfSilence2     = 207682,
                SigilOfFlame2       = 204513,
                SigilOfMisery2      = 202140,
                SpellsNumber        = 7
            };

            std::array<uint32, eSpells::SpellsNumber> m_Spells =
            {{
                eSpells::SigilOfChains,
                eSpells::SigilOfFlame,
                eSpells::SigilOfMisery,
                eSpells::SigilOfSilence,
                eSpells::SigilOfSilence2,
                eSpells::SigilOfFlame2,
                eSpells::SigilOfMisery2
            }};

            bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::Metamorphosis)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                for (uint32 l_Spell : m_Spells)
                    l_Player->RemoveSpellCooldown(l_Spell, true);

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DemonSpikes))
                    l_Player->RestoreCharge(l_SpellInfo->ChargeCategoryEntry, true);

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EmpowerWards))
                    l_Player->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_runemasters_pauldrons_AuraScript::HandleDoCheckProc);
                OnProc += AuraProcFn(spell_dh_runemasters_pauldrons_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_runemasters_pauldrons_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Consume Soul - 210042
class spell_dh_consume_soul_havoc : public SpellScriptLoader
{
    public:
        spell_dh_consume_soul_havoc() : SpellScriptLoader("spell_dh_consume_soul_havoc") { }

        class spell_dh_consume_soul_havoc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_consume_soul_havoc_SpellScript);

            enum eSpells
            {
                ConsumeSoulHavoc    = 178963
            };

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SPEC_DEMON_HUNTER_HAVOC)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ConsumeSoulHavoc);
                uint32 l_HealthPct = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                SetHitHeal(CalculatePct(l_Player->GetMaxHealth(), l_HealthPct));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_consume_soul_havoc_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_consume_soul_havoc_SpellScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Sigil of Flame - 204598
class spell_dh_sigil_of_flame : public SpellScriptLoader
{
    public:
        spell_dh_sigil_of_flame() : SpellScriptLoader("spell_dh_sigil_of_flame") { }

        class spell_dh_sigil_of_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_sigil_of_flame_SpellScript);

            enum eSpells
            {
                SpiritOfTheDarknessFlameBonusDamage = 235543,
                SpiritOfTheDarknessFlame            = 235524
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::SpiritOfTheDarknessFlame))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SpiritOfTheDarknessFlameBonusDamage, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_dh_sigil_of_flame_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_sigil_of_flame_SpellScript();
        }

        class spell_dh_sigil_of_flame_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_sigil_of_flame_AuraScript);

            enum eSpells
            {
                SpiritOfTheDarknessFlameBonusDamage = 235543,
                SpiritOfTheDarknessFlame = 235524
            };

            bool m_FirstTick = true;

            void OnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !m_FirstTick)
                    return;

                m_FirstTick = false;
               const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(p_AuraEffect->GetAmount() / 3);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_sigil_of_flame_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_sigil_of_flame_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Immolation Aura - 178740
/// Called for Cloak of Fel flames - 217735
class spell_dh_cloak_of_fel_flames : public SpellScriptLoader
{
    public:
        spell_dh_cloak_of_fel_flames() : SpellScriptLoader("spell_dh_cloak_of_fel_flames") { }

        class spell_dh_cloak_of_fel_flames_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_cloak_of_fel_flames_AuraScript);

            enum eSpells
            {
                CloakOfFelFlames        = 217735,
                CloakOfFelFlamesBuff    = 217741
            };

            void HanldeAfterEffectApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::CloakOfFelFlames))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CloakOfFelFlamesBuff, true);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::CloakOfFelFlamesBuff);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_cloak_of_fel_flames_AuraScript::HanldeAfterEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_cloak_of_fel_flames_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_cloak_of_fel_flames_AuraScript();
        }
};

/// Last Update 7.3.5
/// Darkness - 209426
class spell_dh_darkness : public SpellScriptLoader
{
public:
    spell_dh_darkness() : SpellScriptLoader("spell_dh_darkness") { }

    class spell_dh_darkness_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_darkness_AuraScript);

        enum eSpells
        {
            CoverOfDarkness = 227635,
        };

        void OnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
        {
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            SpellInfo const* l_CoverOfDarknessInfo = sSpellMgr->GetSpellInfo(eSpells::CoverOfDarkness);
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_SpellInfo || !l_CoverOfDarknessInfo)
                return;

            int32 l_Bp = l_SpellInfo->Effects[0].BasePoints;
            if (l_Caster->HasAura(eSpells::CoverOfDarkness))
                l_Bp += l_CoverOfDarknessInfo->Effects[0].BasePoints;

            if (roll_chance_i(l_Bp))
                p_AbsorbAmount = p_DmgInfo.GetAmount();
            else
                PreventDefaultAction();
        }

        void HanldeOnEffectApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes p_Modes)
        {
            Unit* l_Owner = GetUnitOwner();

            if (!l_Owner)
                return;

            if (Aura* l_Aura = l_Owner->GetAura(m_scriptSpellId))
            {
                if (l_Aura->GetCasterGUID() != GetCasterGUID())
                    l_Aura->Remove();
            }
        }

        void CalculateAmount(AuraEffect const* p_AurEff, int32 & p_Amount, bool & /*canBeRecalculated*/)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_CoverOfDarknessInfo = sSpellMgr->GetSpellInfo(eSpells::CoverOfDarkness);
            if (!l_Caster || !l_CoverOfDarknessInfo || !GetSpellInfo())
                return;

            if (l_Caster->HasAura(eSpells::CoverOfDarkness))
                p_Amount = GetSpellInfo()->Effects[EFFECT_0].BasePoints + l_CoverOfDarknessInfo->Effects[EFFECT_0].BasePoints;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_darkness_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            AfterEffectApply += AuraEffectApplyFn(spell_dh_darkness_AuraScript::HanldeOnEffectApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            OnEffectAbsorb   += AuraEffectAbsorbFn(spell_dh_darkness_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_darkness_AuraScript();
    }
};

/// Last Update 7.1.5
/// Mastery: Demonic Presence - 185164
class spell_dh_demonic_presence : public SpellScriptLoader
{
    public:
        spell_dh_demonic_presence() : SpellScriptLoader("spell_dh_demonic_presence") { }

        class spell_dh_demonic_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_demonic_presence_AuraScript);

            void CalculateAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraEffect* l_AuraEffect = GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                p_Amount = l_AuraEffect->GetAmount();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_demonic_presence_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_SPEED_ALWAYS);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_demonic_presence_AuraScript();
        }
};

/// Last Update 7.1.5
/// Deceiver's Fury - 201463
class spell_dh_deceivers_fury : public SpellScriptLoader
{
    public:
        spell_dh_deceivers_fury() : SpellScriptLoader("spell_dh_deceivers_fury") { }

        class spell_dh_deceivers_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_deceivers_fury_AuraScript);

            enum eSpells
            {
                DeceiversFuryProc = 202120
            };

            enum eArtifactPowers
            {
                DeceiversFuryArtifact = 1007
            };

            void HandleProc(AuraEffect const*, ProcEventInfo& p_ProcInfo)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;
                if (!l_Caster || !(l_Player = l_Caster->ToPlayer()))
                    return;

                if (p_ProcInfo.GetHitMask() & ProcFlagsHit::PROC_HIT_DODGE)
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeceiversFuryProc);
                    if (!l_SpellInfo)
                        return;

                    uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::DeceiversFuryArtifact);
                    int32 l_Bp0 = l_Rank * l_SpellInfo->Effects[EFFECT_0].BasePoints;

                    l_Caster->CastCustomSpell(l_Caster, eSpells::DeceiversFuryProc, &l_Bp0, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dh_deceivers_fury_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_deceivers_fury_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Reverse Magic - 205604
class spell_dh_reverse_magic : public SpellScriptLoader
{
    public:
        spell_dh_reverse_magic() : SpellScriptLoader("spell_dh_reverse_magic") { }

        class spell_dh_reverse_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_reverse_magic_SpellScript);

            enum eSpells
            {
                Fear                    = 118699,
                FreezingTrap            = 3355,
                FelEruption             = 211881,
                Polymorph               = 118,
                HammerOfJustice         = 853,
                ShadowWordPain          = 589,
                VampiricTouch           = 34914,
                UnstableAffliction      = 30108,
                Corruption              = 172,
                EntanglingRoots         = 339,
                FrostNova               = 122,
                MindControl             = 605,
                DragonsBreath           = 31661,
                PsychicScream           = 8122
            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::Fear,
                eSpells::FreezingTrap,
                eSpells::FelEruption
            };

            std::vector<eSpells> m_NonReflectSpells =
            {
                eSpells::Polymorph,
                eSpells::HammerOfJustice,
                eSpells::ShadowWordPain,
                eSpells::VampiricTouch,
                eSpells::UnstableAffliction,
                eSpells::Corruption,
                eSpells::EntanglingRoots,
                eSpells::FrostNova,
                eSpells::MindControl,
                eSpells::DragonsBreath,
                eSpells::PsychicScream
            };

            void HandleOnEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                DispelChargesList l_DispelChargeList;
                uint32 l_DispelMask = 1 << DispelType::DISPEL_MAGIC;
                l_Target->GetDispellableAuraList(l_Caster, l_DispelMask, l_DispelChargeList);

                for (eSpells l_Spell : m_Spells)
                {
                    if (Aura* l_Aura = l_Target->GetAura(l_Spell))
                        l_DispelChargeList.push_back(std::make_pair(l_Aura, 1));
                }

                for (std::pair<Aura*, uint8> l_DispelAura : l_DispelChargeList)
                {
                    if (!l_DispelAura.first)
                        continue;

                    Unit* l_OriginalCaster = l_DispelAura.first->GetCaster();
                    if (!l_OriginalCaster)
                        continue;

                    uint64 l_OriginalCasterGUID = l_OriginalCaster->GetGUID();
                    for (uint8 l_Itr = 0; l_Itr < l_DispelAura.second; ++l_Itr)
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_DispelAura.first->GetId());
                        if (!l_SpellInfo || l_SpellInfo->AttributesEx & (SpellAttr1::SPELL_ATTR1_CANT_BE_REFLECTED | SpellAttr1::SPELL_ATTR1_CANT_BE_REDIRECTED))
                            continue;

                        bool l_AllowReflecting = true;
                        for (eSpells l_Spell : m_NonReflectSpells)
                            if (l_SpellInfo->Id == l_Spell)
                                l_AllowReflecting = false;

                        if (!l_AllowReflecting)
                            continue;

                        l_Target->CastSpell(l_OriginalCaster, l_DispelAura.first->GetId(), true, nullptr, nullptr, l_OriginalCasterGUID);

                        if (l_SpellInfo->AttributesEx4 & SpellAttr4::SPELL_ATTR4_HAS_DELAY)
                        {
                            l_Target->AddDelayedEvent([l_Target, l_OriginalCasterGUID, l_DispelAura]() -> void
                            {
                                Unit* l_OriginalCaster = sObjectAccessor->GetUnit(*l_Target, l_OriginalCasterGUID);
                                if (!l_OriginalCaster)
                                    return;

                                l_Target->CastSpell(l_OriginalCaster, l_DispelAura.first->GetId(), true, nullptr, nullptr, l_OriginalCasterGUID);
                            }, 10);
                        }

                        Aura* l_Aura = l_OriginalCaster->GetAura(l_DispelAura.first->GetId());
                        if (l_Aura)
                            l_Aura->SetDuration(l_DispelAura.first->GetDuration());
                    }

                    l_Target->RemoveAurasDueToSpellByDispel(l_DispelAura.first->GetId(), m_scriptSpellId, l_OriginalCaster->GetGUID(), l_Caster, l_DispelAura.second);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_reverse_magic_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_reverse_magic_SpellScript();
        }
};

/// Demonic Appetite - 206478
class spell_dh_demonic_appetite : public SpellScriptLoader
{
    public:
        spell_dh_demonic_appetite() : SpellScriptLoader("spell_dh_demonic_appetite") { }

        class spell_dh_demonic_appetite_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_demonic_appetite_Aurascript);

            enum eSpells
            {
                ChaosStrikeDamage           = 222031,
                AnnihilationDamage          = 227518
            };

            bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (l_DamageInfo == nullptr)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr)
                    return false;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::ChaosStrikeDamage:
                    case eSpells::AnnihilationDamage:
                        return true;
                }

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_demonic_appetite_Aurascript::HandleDoCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_demonic_appetite_Aurascript();
        }
};

/// Last update to Legion 7.1.5
/// Called by Demonic Origins 235893
class spell_dh_demonic_origins : public SpellScriptLoader
{
    public:
        spell_dh_demonic_origins() : SpellScriptLoader("spell_dh_demonic_origins") { }

        class spell_dh_demonic_origins_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_demonic_origins_AuraScript);

            enum eSpells
            {
                DemonicOriginsDamages = 235894
            };

            void HandleOnApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DemonicOriginsDamages, true);
            }

            void HandleOnRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::DemonicOriginsDamages);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_demonic_origins_AuraScript::HandleOnApply, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_demonic_origins_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_demonic_origins_AuraScript();
        }
};

/// Last update to Legion 7.1.5
/// Called by Metamorphosis 162264 for Demonic Origins 235893
class spell_dh_metamorphosis_transform : public SpellScriptLoader
{
    public:
        spell_dh_metamorphosis_transform() : SpellScriptLoader("spell_dh_metamorphosis_transform") { }

        class spell_dh_metamorphosis_transform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_metamorphosis_transform_AuraScript);

            enum eSpells
            {
                DemonicOriginsDamages   = 235894,
                DemonicOriginAura       = 235893,
                PvpAura                 = 134735,
                Metamorphosis           = 162264,

                // Glide glyphs
                GlyphOfFallowWings      = 220083,
                GlyphOfFelWings         = 220228,
                GlyphOfTatteredWings    = 220226
            };

            std::vector<eSpells> m_Auras =
            {
                eSpells::GlyphOfFallowWings,
                eSpells::GlyphOfFelWings,
                eSpells::GlyphOfTatteredWings
            };

            uint32 m_GlideGlyphAura = 0;

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_GlideGlyphAura)
                    l_Caster->AddAura(m_GlideGlyphAura, l_Caster);

                if (!l_Caster->HasAura(eSpells::PvpAura) || !l_Caster->HasAura(eSpells::DemonicOriginAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DemonicOriginsDamages, true);
            }

            void HandleAfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !p_AurEff)
                    return;

                for (auto l_Aura : m_Auras)
                {
                    if (l_Caster->HasAura(l_Aura))
                    {
                        m_GlideGlyphAura = l_Aura;
                        l_Caster->RemoveAura(l_Aura);
                    }
                }

                if (l_Caster->CanApplyPvPSpellModifiers() || l_Caster->HasAura(eSpells::PvpAura))
                    l_Caster->RemoveAura(eSpells::DemonicOriginsDamages);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_dh_metamorphosis_transform_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_metamorphosis_transform_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_metamorphosis_transform_AuraScript();
        }
};



/// Last Update: 7.1.5 Build 23420
/// Called by Charred Warblades - 213010
class spell_dh_charred_warblades : public SpellScriptLoader
{
    public:
        spell_dh_charred_warblades() : SpellScriptLoader("spell_dh_charred_warblades") { }

        class spell_dh_charred_warblades_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_charred_warblades_AuraScript);

            enum eSpells
            {
                CharredWarbladesHeal = 213011
            };

            bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !(l_DamageInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FIRE))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return;

                int32 l_HealAmount = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);

                l_Caster->CastCustomSpell(l_Caster, eSpells::CharredWarbladesHeal, &l_HealAmount, nullptr, nullptr, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_charred_warblades_AuraScript::HandleDoCheckProc);
                OnProc += AuraProcFn(spell_dh_charred_warblades_AuraScript::HandleOnProc);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_charred_warblades_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Contained Fury- 201454
/// Need to recalculate amounts because ArtifactPowerRankEntry has RankBasePoints=0.
class spell_dh_contained_fury : public SpellScriptLoader
{
    public:
        spell_dh_contained_fury() : SpellScriptLoader("spell_dh_contained_fury") { }

        class spell_dh_contained_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_contained_fury_AuraScript);

            enum eArtifactTrait
            {
                ContainedFury = 1000
            };

            void CalculateAmount(AuraEffect const* p_AurEff, int32 & p_Amount, bool & /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                p_Amount = GetSpellInfo()->Effects[p_AurEff->GetEffIndex()].BasePoints * l_Player->GetRankOfArtifactPowerId(eArtifactTrait::ContainedFury);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_contained_fury_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_MAX_POWER);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dh_contained_fury_AuraScript();
        }
};


/// Last Update 7.1.5 - 23420
/// Called For Fueled By Pain - 213017
class spell_dh_fueled_by_pain : public SpellScriptLoader
{
    public:
        spell_dh_fueled_by_pain() : SpellScriptLoader("spell_dh_fueled_by_pain") { }

        class spell_dh_fueled_by_pain_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_fueled_by_pain_Aurascript);

            enum eSpells
            {
                ConsumeSoul1    = 203794, ///< Simple heal, AP * 2.5
                ConsumeSoul2    = 228532, ///< 10% heal
                ConsumeSoul3    = 210042, ///< Simple heal, AP * 21.25, without Demonic Appetite
                ConsumeSoul4    = 178963, ///< 25% heal
                Metamorphosis   = 187827
            };

            enum eArtifact
            {
                FueledByPain    = 1497
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                if (Aura const* l_Aura = l_Caster->GetAura(eSpells::Metamorphosis))
                    if (l_Aura->GetDuration() >= 5 * IN_MILLISECONDS)
                        return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!p_ProcInfos.GetDamageInfo() || !l_Player)
                    return;

                SpellInfo const* l_SpellInfo = p_ProcInfos.GetDamageInfo()->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::ConsumeSoul1:
                    case eSpells::ConsumeSoul2:
                    case eSpells::ConsumeSoul3:
                    case eSpells::ConsumeSoul4:
                        break;
                    default:
                        return;
                }

                if (Aura* l_Aura = l_Caster->AddAura(eSpells::Metamorphosis, l_Caster))
                    l_Aura->SetDuration((p_AurEff->GetAmount() + (l_Player->GetRankOfArtifactPowerId(eArtifact::FueledByPain) / 2)) * IN_MILLISECONDS );
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_fueled_by_pain_Aurascript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_dh_fueled_by_pain_Aurascript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_fueled_by_pain_Aurascript();
        }
};

/// Last Update 7.1.5 - 23420
/// Chaos Nova - 179057
class spell_dh_chaos_nova : public SpellScriptLoader
{
    public:
        spell_dh_chaos_nova() : SpellScriptLoader("spell_dh_chaos_nova") { }

        class spell_dh_chaos_nova_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_chaos_nova_AuraScript);

            enum eSpells
            {
                OverwhelmingPower = 201464,
                LesserSoulFragment = 215393
            };

            enum eArtifactPowers
            {
                OverwhelmingPowerArtifact = 1008
            };

            void HandleApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                HandleOverwhelmingPower(l_Caster);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_chaos_nova_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            void HandleOverwhelmingPower(Unit* p_Caster)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (AuraEffect const* l_AurEffect = p_Caster->GetAuraEffect(eSpells::OverwhelmingPower, EFFECT_0))
                    {
                        uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::OverwhelmingPowerArtifact);
                        int32 l_Chance = l_Rank * l_AurEffect->GetSpellInfo()->Effects[EFFECT_0].BasePoints;

                        if (roll_chance_i(l_Chance))
                        {
                            l_Player->CastSpell(l_Player, eSpells::LesserSoulFragment, true);
                        }
                    }
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_chaos_nova_AuraScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Empower Wards - 218256
class spell_dh_empower_wards : public SpellScriptLoader
{
    public:
        spell_dh_empower_wards() : SpellScriptLoader("spell_dh_empower_wards") { }

        class spell_dh_empower_wards_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_empower_wards_AuraScript);

            enum eSpells
            {
                SiphonPower     = 218910,
                SiphonedPower   = 218561,
                NecroticDoT     = 209858
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                /// Don't be affected by Necrotic Affix
                if (SpellInfo const* l_SpellInfo = p_DmgInfo.GetSpellInfo())
                    if (l_SpellInfo->Id == eSpells::NecroticDoT)
                        return;

                HandleSiphonPower(l_Owner, p_DmgInfo.GetAmount());

                p_AbsorbAmount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_empower_wards_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dh_empower_wards_AuraScript::OnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
            }

        private:

            void HandleSiphonPower(Unit* p_Owner, uint32 p_Absorbed)
            {
                m_AbsorbedDamage += p_Absorbed;

                if (AuraEffect const* l_SiphonPowerEffect = p_Owner->GetAuraEffect(eSpells::SiphonPower, EFFECT_0))
                {
                    uint32 l_MaxPercent = l_SiphonPowerEffect->GetAmount();
                    int32 l_HealthPercent = (m_AbsorbedDamage * 100) / p_Owner->GetMaxHealth();

                    if (l_HealthPercent > 0)
                    {
                        if (l_HealthPercent > l_MaxPercent)
                            l_HealthPercent = l_MaxPercent;

                        if (AuraEffect* l_SiphonedPowerEffect = p_Owner->GetAuraEffect(eSpells::SiphonedPower, EFFECT_0))
                        {
                            int32 l_CurrentAmount = l_SiphonedPowerEffect->GetAmount();
                            if (l_HealthPercent > l_CurrentAmount)
                            {
                                l_SiphonedPowerEffect->ChangeAmount(l_HealthPercent);
                                l_SiphonedPowerEffect->GetBase()->RefreshDuration();
                            }
                        }
                        else
                        {
                            p_Owner->CastCustomSpell(eSpells::SiphonedPower, SpellValueMod::SPELLVALUE_BASE_POINT0, l_HealthPercent, p_Owner, true);
                        }
                    }
                }
            }

        private:

            uint32 m_AbsorbedDamage = 0;

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_empower_wards_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Soul Carver - 207407
class spell_dh_soul_carver : public SpellScriptLoader
{
    public:
        spell_dh_soul_carver() : SpellScriptLoader("spell_dh_soul_carver") { }

        enum eSpells
        {
            LesserSoulFragment = 226258
        };

        class spell_dh_soul_carver_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_soul_carver_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Caster || !l_Player)
                    return SPELL_FAILED_DONT_REPORT;


                if (l_Player->GetActiveSpecializationID() != SPEC_DEMON_HUNTER_VENGEANCE)
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void HandleHitTarget(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::LesserSoulFragment, true);
                l_Caster->CastSpell(l_Target, eSpells::LesserSoulFragment, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dh_soul_carver_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_dh_soul_carver_SpellScript::HandleHitTarget, EFFECT_1, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_soul_carver_SpellScript();
        }

        class spell_dh_soul_carver_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_soul_carver_AuraScript);

            void HandlePeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::LesserSoulFragment, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dh_soul_carver_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_soul_carver_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Immolation Aura (initial burst) - 187727
/// Called for Fallout - 227174
class spell_dh_fallout : public SpellScriptLoader
{
    public:
        spell_dh_fallout() : SpellScriptLoader("spell_dh_fallout") { }

        class spell_dh_fallout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_fallout_SpellScript);

            enum eSpells
            {
                FalloutAura         = 227174,
                LesserSoulFragment  = 226258
            };

            void HandleOnEffectHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (l_Caster == nullptr || l_Target  == nullptr || !l_Caster->HasAura(eSpells::FalloutAura))
                    return;

                if (roll_chance_i(60)) ///< Missing proc chance, tmp value
                    l_Caster->CastSpell(l_Target, eSpells::LesserSoulFragment, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_fallout_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_fallout_SpellScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Chaos Strike - 222031
class spell_dh_chaos_strike_mh : public SpellScriptLoader
{
    public:
        spell_dh_chaos_strike_mh() : SpellScriptLoader("spell_dh_chaos_strike_mh") { }

        class spell_dh_chaos_strike_mh_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_chaos_strike_mh_SpellScript);

            enum eSpells
            {
                ChaoticOnSlaught = 197125
            };

            void HandleOnHit()
            {
                Spell* l_Spell = GetSpell();
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Spell || !l_Target || !l_Caster)
                    return;

                if (l_Spell->IsCritForTarget(l_Target))
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ChaosStrike) = true;
                else
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ChaosStrike) = false;
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dh_chaos_strike_mh_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_chaos_strike_mh_SpellScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Defensive Spikes - 212871
class spell_dh_defensive_spikes : public SpellScriptLoader
{
    public:
        spell_dh_defensive_spikes() : SpellScriptLoader("spell_dh_defensive_spikes") { }

        class spell_dh_defensive_spikes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_defensive_spikes_AuraScript);

            enum eSpells
            {
                DemonSpikes = 203819
            };

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (AuraEffect* l_Effect = l_Caster->GetAuraEffect(eSpells::DemonSpikes, EFFECT_0))
                {
                    int32 l_Amount = l_Effect->GetAmount();
                    SpellInfo const* l_SpellInfo = GetSpellInfo();

                    if (!l_SpellInfo)
                        return;

                    l_Amount -= l_SpellInfo->Effects[EFFECT_0].BasePoints;

                    l_Effect->ChangeAmount(l_Amount);
                }
            }

            void HandleAfterApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (AuraEffect* l_Effect = l_Caster->GetAuraEffect(eSpells::DemonSpikes, EFFECT_0))
                {
                    int32 l_Amount = l_Effect->GetAmount();
                    SpellInfo const* l_SpellInfo = GetSpellInfo();

                    if (!l_SpellInfo)
                        return;

                    l_Amount += l_SpellInfo->Effects[EFFECT_0].BasePoints;

                    l_Effect->ChangeAmount(l_Amount);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_dh_defensive_spikes_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_defensive_spikes_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_defensive_spikes_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Chaos Strike - 199547
class spell_dh_chaos_strike_off : public SpellScriptLoader
{
    public:
        spell_dh_chaos_strike_off() : SpellScriptLoader("spell_dh_chaos_strike_off") { }

        class spell_dh_chaos_strike_off_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_chaos_strike_off_SpellScript);

            void HandleOnHit()
            {
                Spell* l_Spell = GetSpell();
                Unit* l_Caster = GetCaster();
                if (!l_Spell || !l_Caster)
                    return;

                l_Spell->SetCustomCritChance(l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ChaosStrike) ? 100 : 0);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_dh_chaos_strike_off_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_chaos_strike_off_SpellScript();
        }
};

/// Called by Demon's Bite - 162243, Demon Blades - 203796
class spell_dh_demons_bite : public SpellScriptLoader
{
    public:
        spell_dh_demons_bite() : SpellScriptLoader("spell_dh_demons_bite") { }

        enum eSpells
        {
            DemonsBite = 162243,
            DemonBlade = 203796,
            T19Havoc2P = 211054
        };

        class spell_dh_demons_bite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_demons_bite_SpellScript);

            bool m_AlreadyEnergized = false;

            void HandleEnergize(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Unit* l_Caster = GetCaster();
                int32 l_BP = 0;

                l_BP = 0;

                switch (m_scriptSpellId)
                {
                    case eSpells::DemonBlade:
                        l_BP = 12 + urand(0, 8);
                        break;
                    case eSpells::DemonsBite:
                        l_BP = 20 + urand(0, 10);
                        break;
                    default:
                        return;
                }

                /// Item - Demon Hunter T19 Havoc 2P Bonus
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Havoc2P, EFFECT_0))
                    AddPct(l_BP, l_AuraEffect->GetAmount());

                l_Caster->ModifyPower(POWER_FURY, l_BP);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_demons_bite_SpellScript::HandleEnergize, EFFECT_2, SPELL_EFFECT_ENERGIZE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_demons_bite_SpellScript();
        }
};

/// Last Update: 7.3.5 build 26365
/// Nemesis - 206491
class spell_dh_nemesis : public SpellScriptLoader
{
public:
    spell_dh_nemesis() : SpellScriptLoader("spell_dh_nemesis") { }

    class spell_dh_nemesis_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_nemesis_AuraScript);

        enum eSpells
        {
            Nemesis = 206491,
            NemesisDragonkin = 208610,
            NemesisAberration = 208607,
            NemesisBeast = 208608,
            NemesisGiant = 208612,
            NemesisMechanical = 208613,
            NemesisDemon = 208579,
            NemesisUndead = 208614,
            NemesisCritter = 208609,
            NemesisElemental = 208611,
            NemesisHumanoid = 208605,
            PvpRules = 134735
        };

        void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Owner = GetUnitOwner();

            if (!l_Caster || !l_Owner)
                return;

            Aura* l_Aura = GetAura();
            if (!l_Aura)
                return;

            uint32 l_RemainingDuration = l_Aura->GetDuration();
            uint32 l_SpellId = 0;

            switch (l_Owner->GetCreatureType())
            {
                case CREATURE_TYPE_DRAGONKIN:
                    l_SpellId = NemesisDragonkin;
                    break;
                case CREATURE_TYPE_ABBERATION:
                    l_SpellId = NemesisAberration;
                    break;
                case CREATURE_TYPE_BEAST:
                    l_SpellId = NemesisBeast;
                    break;
                case CREATURE_TYPE_GIANT:
                    l_SpellId = NemesisGiant;
                    break;
                case CREATURE_TYPE_MECHANICAL:
                    l_SpellId = NemesisMechanical;
                    break;
                case CREATURE_TYPE_DEMON:
                    l_SpellId = NemesisDemon;
                    break;
                case CREATURE_TYPE_UNDEAD:
                    l_SpellId = NemesisUndead;
                    break;
                case CREATURE_TYPE_CRITTER:
                    l_SpellId = NemesisCritter;
                    break;
                case CREATURE_TYPE_ELEMENTAL:
                    l_SpellId = NemesisElemental;
                    break;
                case CREATURE_TYPE_HUMANOID:
                    l_SpellId = NemesisHumanoid;
                    break;
                default:
                    break;
            }

            if (Aura* l_BuffAura = l_Caster->AddAura(l_SpellId, l_Caster))
                l_BuffAura->SetDuration(l_RemainingDuration);
        }

        void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
        {
            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            if (!l_Caster || !l_Aura)
                return;

            if (l_Caster->HasAura(eSpells::PvpRules))
                l_Aura->SetDuration(GetMaxDuration() * 0.5f); ///< Nemesis is reduced duration by 50% in PvP
        }

        void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount -= 10;
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_dh_nemesis_AuraScript::HandleOnProc);
            AfterEffectApply += AuraEffectApplyFn(spell_dh_nemesis_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_nemesis_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_nemesis_AuraScript();
    }
};

/// Last update 7.3.2 Build 25549
/// Called by Legendary: Soul of the Slayer - 247786
class spell_dh_soul_of_the_slayer : public SpellScriptLoader
{
    public:
        spell_dh_soul_of_the_slayer() : SpellScriptLoader("spell_dh_soul_of_the_slayer") { }

        class spell_dh_soul_of_the_slayer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_soul_of_the_slayer_AuraScript);

            enum eSpells
            {
                FirstBlood         = 206416,
                Fallout            = 227174
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
                case SPEC_DEMON_HUNTER_HAVOC:
                    l_Player->CastSpell(l_Player, eSpells::FirstBlood, true);
                    break;
                default:
                    l_Player->CastSpell(l_Player, eSpells::Fallout, true);
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
                case SPEC_DEMON_HUNTER_HAVOC:
                    l_Player->RemoveAura(eSpells::FirstBlood);
                    break;
                default:
                    l_Player->RemoveAura(eSpells::Fallout);
                    break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_soul_of_the_slayer_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_soul_of_the_slayer_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_soul_of_the_slayer_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Demonic Infusion - 236189
class spell_dh_demonic_infusion : public SpellScriptLoader
{
    public:
        spell_dh_demonic_infusion() : SpellScriptLoader("spell_dh_demonic_infusion") { }

        enum eSpell
        {
            DemonSpikes = 203720
        };

        class spell_dh_demonic_infusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_demonic_infusion_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::DemonSpikes))
                        {
                            if (l_Player->GetCommandStatus(CHEAT_COOLDOWN))
                                return;

                            l_Player->ResetCharges(l_SpellInfo->ChargeCategoryEntry);
                            l_Player->CastSpell(l_Player, eSpell::DemonSpikes, true);
                            l_Player->ResetCharges(l_SpellInfo->ChargeCategoryEntry);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dh_demonic_infusion_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_demonic_infusion_SpellScript();
        }
};

/// Last update 7.3.5 Build 26365
class PlayerScript_dh_felblade_auras : public PlayerScript
{
    public:
        PlayerScript_dh_felblade_auras() :PlayerScript("PlayerScript_dh_felblade_auras") { }

        enum eSpells
        {
            Felblade            = 232893,
            FelbladeAuraHavoc   = 236167,
            FelbladeAuraVeng    = 203557
        };

        void OnLogin(Player* p_Player) override
        {
            if (p_Player->HasSpell(eSpells::Felblade))
                if (uint32 l_AuraId = GetAuraForSpec((SpecIndex)p_Player->GetActiveSpecializationID()))
                    p_Player->AddAura(l_AuraId, p_Player);
        }

        /// The cross server doesn't call OnLogin
        void OnEnterBG(Player* p_Player, uint32 p_MapID) override
        {
            if (p_Player->HasSpell(eSpells::Felblade))
            {
                uint32 l_AuraId = GetAuraForSpec((SpecIndex)p_Player->GetActiveSpecializationID());
                if (l_AuraId > 0 && !p_Player->HasAura(l_AuraId))
                    p_Player->AddAura(l_AuraId, p_Player);
            }
        }

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID) override
        {
            if (p_SpellID == eSpells::Felblade)
            {
                if (uint32 l_AuraId = GetAuraForSpec((SpecIndex)p_Player->GetActiveSpecializationID()))
                    p_Player->AddAura(l_AuraId, p_Player);
            }
        }

        void OnSpellRemoved(Player* p_Player, uint32 p_SpellID) override
        {
            if (p_SpellID == eSpells::Felblade)
            {
                p_Player->RemoveAura(eSpells::FelbladeAuraHavoc);
                p_Player->RemoveAura(eSpells::FelbladeAuraVeng);
            }
        }

    private:

        uint32 GetAuraForSpec(SpecIndex p_Spec)
        {
            if (p_Spec == SpecIndex::SPEC_DEMON_HUNTER_HAVOC)
                return eSpells::FelbladeAuraHavoc;
            else if (p_Spec == SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE)
                return eSpells::FelbladeAuraVeng;

            return 0;
        }
};

/// Last Update: 7.3.5 build 26365
/// Called by Metamorphosis(Leech) - 162264
class spell_dh_metamorphosis_leech : public SpellScriptLoader
 {
    public:
        spell_dh_metamorphosis_leech() : SpellScriptLoader("spell_dh_metamorphosis_leech") { }

        enum eSpells
        {
            SoulRending = 204909,
            Metamorphosis = 162264
        };

        class spell_dh_metamorphosis_leech_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_metamorphosis_leech_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                SpellInfo const* l_MetamorphosisSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Metamorphosis);
                SpellInfo const* l_SoulRendingSpellInfo = sSpellMgr->GetSpellInfo(eSpells::SoulRending);
                ///< Metamorphosis(Leech) and Soul Rending is reduced by 60% in PvP
                int32 MetamorphosisBp = (l_MetamorphosisSpellInfo ? l_MetamorphosisSpellInfo->Effects[EFFECT_3].BasePoints : 0) * 0.4f;
                int32 SoulRendingBp = (l_SoulRendingSpellInfo ? l_SoulRendingSpellInfo->Effects[EFFECT_0].BasePoints : 0) * 0.4f;

                p_Amount = (l_Player->HasAura(eSpells::SoulRending) ? SoulRendingBp + MetamorphosisBp : MetamorphosisBp);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_metamorphosis_leech_AuraScript::HandlePvPModifier, EFFECT_3, SPELL_AURA_MOD_LEECH_PCT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_metamorphosis_leech_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Flaming Soul - 238118
class spell_dh_flaming_soul : public SpellScriptLoader
{
public:
    spell_dh_flaming_soul() : SpellScriptLoader("spell_dh_flaming_soul") { }

    class spell_dh_flaming_soul_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_flaming_soul_AuraScript);

        enum eSpells
        {
            ImmolationAura      = 187727,
            ImmolationAuraTicks = 178741,
            SoulCarver          = 207407,
            FieryBrand          = 207744,
            FieryDemise         = 212818
        };

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

            if (!l_DamageInfo)
                return;

            Unit* l_Caster = GetCaster();
            Unit* l_Target = l_DamageInfo->GetTarget();

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();

            if (!l_Caster || !l_Target || !l_SpellInfo)
                return;

            if (l_SpellInfo->Id != eSpells::ImmolationAura && l_SpellInfo->Id != eSpells::ImmolationAuraTicks && l_SpellInfo->Id != eSpells::SoulCarver)
                return;

            if (Aura* l_FieryBrandAura = l_Target->GetAura(FieryBrand))
                l_FieryBrandAura->SetDuration(l_FieryBrandAura->GetDuration() + 500);   ///< 0.5 sec

            if (Aura* l_FieryDemise = l_Target->GetAura(eSpells::FieryDemise, l_Caster->GetGUID()))
                l_FieryDemise->SetDuration(l_FieryDemise->GetDuration() + 500);   ///< 0.5 sec
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_dh_flaming_soul_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_flaming_soul_AuraScript();
    }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Immolation Aura - 187727
/// Called for Cleansed by Flame - 205625
class spell_dh_cleansed_by_flame : public SpellScriptLoader
{
    public:
        spell_dh_cleansed_by_flame() : SpellScriptLoader("spell_dh_cleansed_by_flame") { }

        class spell_dh_cleansed_by_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_cleansed_by_flame_SpellScript);

            enum eSpells
            {
                CleansedbyFlameAura    = 205625,
                CleansedbyFlameSpell   = 208770
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::CleansedbyFlameAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CleansedbyFlameSpell, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_dh_cleansed_by_flame_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_cleansed_by_flame_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Cleansed by Flame - 205625
class spell_dh_cleansed_by_flame_proc : public SpellScriptLoader
{
    public:
        spell_dh_cleansed_by_flame_proc() : SpellScriptLoader("spell_dh_cleansed_by_flame_proc") { }

        class spell_dh_cleansed_by_flame_proc_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_cleansed_by_flame_proc_Aurascript);

            bool HandleCheckProc(ProcEventInfo& p_ProcInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_cleansed_by_flame_proc_Aurascript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_cleansed_by_flame_proc_Aurascript();
        }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Illidan's Grasp - 205630
class spell_dh_illidans_grasp : public SpellScriptLoader
{
    public:
        spell_dh_illidans_grasp() : SpellScriptLoader("spell_dh_illidans_grasp") { }

        class spell_dh_illidans_grasp_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_illidans_grasp_Aurascript);

            void HandleEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::IllidansGraspTarget) = l_Target->GetGUID();
            }

            void HandleEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->AddDelayedEvent([l_Caster]() -> void
                    {
                        l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::IllidansGraspTarget) = 0;
                    }, 1 * IN_MILLISECONDS);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dh_illidans_grasp_Aurascript::HandleEffectApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_STUN, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dh_illidans_grasp_Aurascript::HandleEffectRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_STUN, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_illidans_grasp_Aurascript();
        }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Illidan's Grasp: Throw - 208173
class spell_dh_illidans_grasp_throw : public SpellScriptLoader
{
    public:
        spell_dh_illidans_grasp_throw() : SpellScriptLoader("spell_dh_illidans_grasp_throw") { }

        class spell_dh_illidans_grasp_throw_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_illidans_grasp_throw_SpellScript);

            enum eSpells
            {
                IllidansGraspJump = 208175
            };

            void HandleOnEffectHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = l_Caster->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::IllidansGraspTarget));
                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                if (auto l_Dest = GetExplTargetDest())
                {
                    l_Target->CastSpell(l_Dest, eSpells::IllidansGraspJump, true);
                    l_Target->m_SpellHelper.GetUint64(eSpellHelpers::IllidansGraspCaster) = l_Caster->GetGUID();
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_dh_illidans_grasp_throw_SpellScript::HandleOnEffectHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_illidans_grasp_throw_SpellScript();
        }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Illidan's Grasp - 208813
class spell_dh_illidans_grasp_dummy : public SpellScriptLoader
{
    public:
        spell_dh_illidans_grasp_dummy() : SpellScriptLoader("spell_dh_illidans_grasp_dummy") { }

        class spell_dh_illidans_grasp_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_illidans_grasp_dummy_SpellScript);

            enum eSpells
            {
                IllidansGraspDamage = 208618
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_GraspCaster = l_Caster->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::IllidansGraspCaster));
                if (l_Caster == nullptr || l_GraspCaster == nullptr)
                    return;

                l_GraspCaster->CastSpell(l_Caster, eSpells::IllidansGraspDamage, true);

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::IllidansGraspCaster) = 0;
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_dh_illidans_grasp_dummy_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_illidans_grasp_dummy_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Illidari Knowledge - 201459
class spell_dh_illidari_knowledge : public SpellScriptLoader
{
    public:
        spell_dh_illidari_knowledge() : SpellScriptLoader("spell_dh_illidari_knowledge") { }

        class spell_dh_illidari_knowledge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dh_illidari_knowledge_AuraScript);

            enum eArtifactPowers
            {
                IllidariKnowledgeArtifact = 1005
            };

            enum eSpells
            {
                IllidariKnowledge = 201459
            };

            void HandleEffectCalcAmount(AuraEffect const* /*auraEffect*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_IllidariKnowledgeSpellInfo = sSpellMgr->GetSpellInfo(eSpells::IllidariKnowledge);
                if (!l_IllidariKnowledgeSpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                p_Amount = l_IllidariKnowledgeSpellInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eArtifactPowers::IllidariKnowledgeArtifact);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_illidari_knowledge_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_illidari_knowledge_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Fiery Demise - 212817
class spell_dh_fiery_demise: public SpellScriptLoader
{
    public:
        spell_dh_fiery_demise() : SpellScriptLoader("spell_dh_fiery_demise") { }

        class spell_dh_fiery_demise_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_fiery_demise_Aurascript);

            enum eSpells
            {
                FieryBrand = 204021,
                FieryBrandAura = 207744,
                FieryDemise = 212818
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::FieryBrand)
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
                l_Caster->CastCustomSpell(l_Target, eSpells::FieryDemise, &l_Bp, NULL, NULL, true);

                if (Aura* l_FieryBrand = l_Target->GetAura(eSpells::FieryBrandAura, l_Caster->GetGUID())) ///< the duration of Fiery Brand and Fiery Demise should be the same.
                    if (Aura* l_FieryDemise = l_Target->GetAura(eSpells::FieryDemise, l_Caster->GetGUID()))
                        l_FieryDemise->SetDuration(l_FieryBrand->GetDuration());
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_fiery_demise_Aurascript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_dh_fiery_demise_Aurascript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_fiery_demise_Aurascript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Demonic Flames  - 212894
class spell_dh_demonic_flames : public SpellScriptLoader
{
public:
    spell_dh_demonic_flames() : SpellScriptLoader("spell_dh_demonic_flames") { }

    class spell_dh_demonic_flames_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_demonic_flames_AuraScript);

        enum eSpells
        {
            FieryBrand  = 204021,
            FieryDemise = 212818
        };

        void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& /*p_Amount*/, bool& /*p_CanBeRecalculated*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (Aura* l_FieryBrand = l_Caster->GetAura(eSpells::FieryBrand))
                l_FieryBrand->SetDuration(l_FieryBrand->GetDuration() + 2000); ///< 2 sec

            if (Aura* l_FieryDemise = l_Caster->GetAura(eSpells::FieryDemise))
                l_FieryDemise->SetDuration(l_FieryDemise->GetDuration() + 2000); ///< 2 sec
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_demonic_flames_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_demonic_flames_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Consumme Soul - 210042
class spell_dh_consume_soul_heal : public SpellScriptLoader
{
    public:
        spell_dh_consume_soul_heal() : SpellScriptLoader("spell_dh_consume_soul_heal") { }

        class spell_dh_consume_soul_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_consume_soul_heal_SpellScript);

            enum eSpells
            {
                DemonicAppetiteAura = 206478,
                DemonicAppetite = 210041
            };

            void HandleOnHit(SpellEffIndex /* p_EffIndex */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::DemonicAppetiteAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DemonicAppetite, true);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_dh_consume_soul_heal_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_consume_soul_heal_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Consume Soul Heal (Veng) - 203794
class spell_dh_veng_consume_soul_heal : public SpellScriptLoader
{
    public:
        spell_dh_veng_consume_soul_heal() : SpellScriptLoader("spell_dh_veng_consume_soul_heal") { }

        class spell_dh_veng_consume_soul_heal_SpellScript : public SpellScript
        {
            enum eSpells
            {
                T20Veng2PBonus      = 242228,
                Tormented           = 246115
            };

            PrepareSpellScript(spell_dh_veng_consume_soul_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::T20Veng2PBonus))
                        l_Caster->CastSpell(l_Caster, eSpells::Tormented, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dh_veng_consume_soul_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_veng_consume_soul_heal_SpellScript;
        }
};

/// Last Update 7.3.5 Build 25996
/// Imprison - 217832
class spell_dh_imprison : public SpellScriptLoader
{
public:
    spell_dh_imprison() : SpellScriptLoader("spell_dh_imprison") { }

    class spell_dh_imprison_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dh_imprison_SpellScript);

        void HandleAfterHit()
        {
            if (Unit* l_Target = GetHitUnit())
            {
                l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);

                std::list<AuraEffect*> l_AuraEffects = l_Target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DUMMY);
                for (AuraEffect* l_AuraEffect : l_AuraEffects)
                {
                    SpellInfo const* l_SpellInfo = l_AuraEffect->GetSpellInfo();
                    if (!l_SpellInfo || (l_SpellInfo->Mechanic != MECHANIC_BLEED && l_SpellInfo->Mechanic != MECHANIC_INFECTED))
                        continue;

                    if (Aura* l_Aura = l_AuraEffect->GetBase())
                        l_Aura->Remove();
                }
            }
        }

        void Register() override
        {
            AfterHit += SpellHitFn(spell_dh_imprison_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dh_imprison_SpellScript;
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Felblade - 203557, Felblade - 236167
class spell_dh_felblade_aura : public SpellScriptLoader
{
    public:
        spell_dh_felblade_aura() : SpellScriptLoader("spell_dh_felblade_aura") { }

        class spell_dh_felblade_aura_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_dh_felblade_aura_Aurascript);

            enum eSpells
            {
                Felblade            = 232893,
                FelbladeAuraHavoc   = 236167,
                FelbladeAuraVeng    = 203557,
                DemonsBite          = 162243,
                DemonBladesDamage   = 203796,
                Shear               = 203782
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                /// Cannot check it with spell_proc_event and spellfamilymask

                if (GetId() == eSpells::FelbladeAuraHavoc)
                {
                    if (l_SpellInfo->Id != eSpells::DemonsBite &&
                        l_SpellInfo->Id != eSpells::DemonBladesDamage)
                        return false;
                }
                else if (GetId() == eSpells::FelbladeAuraVeng)
                {
                    if (l_SpellInfo->Id != eSpells::Shear)
                        return false;
                }

                return true;
            }

            void HandleAfterProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || !l_Caster->IsPlayer())
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                l_Player->RemoveSpellCooldown(eSpells::Felblade, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dh_felblade_aura_Aurascript::HandleCheckProc);
                AfterProc += AuraProcFn(spell_dh_felblade_aura_Aurascript::HandleAfterProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dh_felblade_aura_Aurascript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Anger of the Half-Giants - 208827
class spell_dh_anger_of_the_half_giants : public SpellScriptLoader
{
public:
    spell_dh_anger_of_the_half_giants() : SpellScriptLoader("spell_dh_anger_of_the_half_giants") { }

    class spell_dh_anger_of_the_half_giants_Aurascript : public AuraScript
    {
        PrepareAuraScript(spell_dh_anger_of_the_half_giants_Aurascript);

        enum eSpells
        {
            DemonsBite                  = 162243,
            DemonBladesDamage           = 203796,
            AngerOfTheHalfGiantsEnergy  = 208828
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (l_DamageInfo == nullptr)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (l_SpellInfo == nullptr)
                return false;

            /// Cannot check it with spell_proc_event and spellfamilymask

            if (l_SpellInfo->Id != eSpells::DemonsBite &&
                l_SpellInfo->Id != eSpells::DemonBladesDamage)
                return false;

            return true;
        }

        void HandleEffectProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
        {
            PreventDefaultAction();

            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            int32 l_Amount = urand(1, p_AurEff->GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].BasePoints);

            l_Caster->CastCustomSpell(eSpells::AngerOfTheHalfGiantsEnergy, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Amount, l_Caster, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_dh_anger_of_the_half_giants_Aurascript::HandleCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_dh_anger_of_the_half_giants_Aurascript::HandleEffectProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_anger_of_the_half_giants_Aurascript();
    }
};

class spell_dh_shattered_souls : public SpellScriptLoader
{
    public:
        spell_dh_shattered_souls() : SpellScriptLoader("spell_dh_shattered_souls") { }

        class spell_dh_shattered_souls_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_shattered_souls_SpellScript);

            void HandleHit(SpellEffIndex /* p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                auto l_Loc = GetExplTargetDest();
                if (l_Caster == nullptr || l_Loc == nullptr)
                    return;

                Position l_LocPos;
                l_Loc->GetPosition(&l_LocPos);

                float l_Angle = l_Loc->GetRelativeAngle(l_Caster) + (urand(0, 1) == 0 ? M_PI / 2 : -M_PI / 2);

                float l_Dist = frand(7.0f, 10.0f);

                Position l_NewPos = l_LocPos;
                l_LocPos.MovePosition(l_NewPos, l_Dist, l_Angle, l_Caster);

                WorldLocation l_TargetPos(l_Caster->GetMapId(), l_NewPos.GetPositionX(), l_NewPos.GetPositionY(), l_NewPos.GetPositionZ(), l_NewPos.GetOrientation());
                SetExplTargetDest(l_TargetPos);
                GetHitDest()->Relocate(l_NewPos);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_dh_shattered_souls_SpellScript::HandleHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_shattered_souls_SpellScript();
        }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Sigil of Chains 202138
class spell_dh_sigil_of_chains_interrupting : public SpellScriptLoader
{
    public:
        spell_dh_sigil_of_chains_interrupting() : SpellScriptLoader("spell_dh_sigil_of_chains_interrupting") { }

        class spell_dh_sigil_of_chains_interrupting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dh_sigil_of_chains_interrupting_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastStop();
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_dh_sigil_of_chains_interrupting_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dh_sigil_of_chains_interrupting_SpellScript();
        }
};

/// Last Update: 7.3.5
/// Called by Eternal Hunger - 208985
class spell_dh_eternal_hunger : public SpellScriptLoader
{
public:
    spell_dh_eternal_hunger() : SpellScriptLoader("spell_dh_eternal_hunger") { }

    class spell_dh_eternal_hunger_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_eternal_hunger_AuraScript);

        enum eSpells
        {
            VengefulRetreat = 198793
        };

        bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
        {
            return false;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_dh_eternal_hunger_AuraScript::HandleDoCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_eternal_hunger_AuraScript();
    }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Shear - 203782, Demon's Bite - 162243, Chaos Blade - 211797, 211796
/// Called for Felblade - 213241
class spell_dh_felblade_reset_cd : public SpellScriptLoader
{
public:
    spell_dh_felblade_reset_cd() : SpellScriptLoader("spell_dh_felblade_reset_cd") { }

    class spell_dh_felblade_reset_cd_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dh_felblade_reset_cd_SpellScript);

        enum eSpells
        {
            Felblade = 213241
        };

        void HandlOnHit()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            if (roll_chance_i(15))
            {
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Felblade))
                {
                    if (l_SpellInfo->ChargeCategoryEntry)
                        l_Caster->ToPlayer()->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
                }
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_dh_felblade_reset_cd_SpellScript::HandlOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dh_felblade_reset_cd_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Soul Fragment - 203795
class spell_dh_soul_fragment_consume_oldest : public SpellScriptLoader
{
public:
    spell_dh_soul_fragment_consume_oldest() : SpellScriptLoader("spell_dh_soul_fragment_consume_oldest") { }

    class spell_dh_soul_fragment_consume_oldest_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dh_soul_fragment_consume_oldest_SpellScript);

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            std::list<AreaTrigger*> l_SoulFragments;
            l_Caster->GetAreaTriggerList(l_SoulFragments, GetSpellInfo()->Id);

            const uint8 l_MaxFragmentsCount = 5;
            if (l_SoulFragments.size() > l_MaxFragmentsCount)
            {
                l_SoulFragments.sort(JadeCore::AreaTriggerDurationPctOrderPred());

                while (l_SoulFragments.size() > l_MaxFragmentsCount)
                {
                    if (AreaTrigger* l_AreaTrigger = l_SoulFragments.front())
                    {
                        l_AreaTrigger->CastAction();
                        l_SoulFragments.pop_front();
                    }

                }
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_dh_soul_fragment_consume_oldest_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dh_soul_fragment_consume_oldest_SpellScript();
    }
};

/// Last Update: 7.3.5 build 26365
/// Called by Chaos Blades - 247938
class spell_dh_chaos_blades_pvp : public SpellScriptLoader
{
public:
    spell_dh_chaos_blades_pvp() : SpellScriptLoader("spell_dh_chaos_blades_pvp") { }

    class spell_dh_chaos_blades_pvp_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_chaos_blades_pvp_AuraScript);

        void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount -= 10;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_chaos_blades_pvp_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_chaos_blades_pvp_AuraScript();
    }
};

// Remove certain auras on spec change
class PlayerScript_dh_spec_change : public PlayerScript
{
    public:
        PlayerScript_dh_spec_change() : PlayerScript("PlayerScript_dh_spec_change") {}

        enum eSpells
        {
            MetamorphosisHavoc = 162264
        };

        void OnModifySpec(Player* p_Player, int32 p_OldSpec, int32 /* p_NewSpec */) override
        {
            if (!p_Player)
                return;

            if (p_OldSpec == SPEC_DEMON_HUNTER_HAVOC)
                p_Player->RemoveAurasDueToSpell(eSpells::MetamorphosisHavoc);
        }
};

/// Last Update: 7.3.5 build 26365
/// Demonic Trample - 205629
class spell_dh_demonic_trample : public SpellScriptLoader
{
public:
    spell_dh_demonic_trample() : SpellScriptLoader("spell_dh_demonic_trample") { }

    class spell_dh_demonic_trample_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dh_demonic_trample_AuraScript);

        void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
        {
            if (!GetCaster())
                return;

            Player* l_Caster = GetCaster()->ToPlayer();
            if (!l_Caster || l_Caster->GetTeamId() == TeamId::TEAM_NEUTRAL)
                return;

            Battleground* l_BG = l_Caster->GetBattleground();
            if (!l_BG)
                return;

            /// Demonic Trample now provides a 50% movement speed increase while carrying a Flag in battlegrounds.
            auto const l_FlagKeepers = l_BG->GetFlagPickersGUID(TeamId(!l_Caster->GetTeamId()));
            if (l_FlagKeepers.find(l_Caster->GetGUID()) == l_FlagKeepers.end())
                return;

            p_Amount -= 150;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_demonic_trample_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_SPEED_NOT_STACK);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dh_demonic_trample_AuraScript::CalculateAmount, EFFECT_3, SPELL_AURA_MOD_MINIMUM_SPEED);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dh_demonic_trample_AuraScript();
    }
};

#ifndef __clang_analyzer__
void AddSC_DemonHunterSpellScripts()
{
    new spell_dh_throw_glaive();
    new spell_dh_consume_soul_heal();
    new spell_dh_demons_bite();
    new spell_dh_defensive_spikes();
    new spell_dh_deceivers_fury();
    new spell_dh_demonic_presence();
    new spell_dh_demonic_appetite();
    new spell_dh_darkness();
    new spell_dh_rain_from_above();
    new spell_dh_rain_from_above_check();
    new spell_dh_fel_lance();
    new spell_dh_demon_chaos_blades();
    new spell_dh_chaos_strike();
    new spell_dh_frailty();
    new spell_dh_frailty_heal();
    new spell_dh_spirit_bomb();
    new spell_dh_spirit_bomb_damage();
    new spell_dh_fel_eruption();
    new spell_dh_eye_beam();
    new spell_dh_nether_bond_periodic();
    new spell_dh_nether_bond();
    new spell_dh_last_resort();
    new spell_dh_demon_spikes();
    new spell_dh_jagged_spikes();
    new spell_dh_fel_devastation();
    new spell_dh_fel_devastation_damage();
    new spell_dh_burning_alive_dot();
    new spell_dh_fiery_brand();
    new spell_dh_soul_cleave();
    new spell_dh_soul_cleave_dmg();
    new spell_dh_erupting_souls_dmg();
    new spell_dh_demon_reborn();
    new spell_dh_master_of_the_glaive();
    new spell_dh_momentum();
    new spell_dh_bloodlet();
    new spell_dh_blade_dance_damage();
    new spell_dh_blade_dance();
    new spell_dh_felblade_charge();
    new spell_dh_felblade();
    new spell_dh_demon_blades();
    new spell_dh_vengeful_retreat();
    new spell_dh_vengeful_retreat_damage();
    new spell_dh_glide();
    new spell_dh_fel_rush();
    new spell_dh_fel_rush_charge();
    new spell_dh_fel_rush_charge_damage();
    new spell_dh_metamorphosis();
    new spell_dh_metamorphosis_immunity();
    new spell_dh_metamorphosis_dmg();
    new spell_dh_infernal_strike();
    new spell_dh_fel_barrage();
    new spell_dh_fel_barrage_passive();
    new spell_dh_fel_eruption_damage();
    new spell_dh_inner_demons();
    new spell_dh_sigil_of_chains();
    new spell_dh_sigil_of_chains_root();
    new spell_dh_angiush_of_the_deceiver();
    new spell_dh_awaken_the_demon_within();
    new spell_dh_unending_hatred();
    new spell_dh_fel_barrage_damage();
    new spell_dh_prepared();
    new spell_dh_rage_of_the_illidari();
    new spell_dh_momentum_vengeful_retreat();
    new spell_dh_chaos_strike_damage();
    new spell_dh_spectral_sight();
    new spell_dh_kirel_narak();
    new spell_dh_delusion_of_grandeur();
    new spell_dh_runemasters_pauldrons();
    new spell_dh_sigil_of_flame();
    new spell_dh_cloak_of_fel_flames();
    new spell_dh_desperate_instincts();
    new spell_dh_blur_aura();
    new spell_dh_consume_soul();
    new spell_dh_fracture();
    new spell_dh_soul_barrier();
    new spell_dh_painbringer();
    new spell_dh_nemesis();
    new spell_dh_soul_of_the_slayer();
    new spell_dh_imprison();
    new spell_dh_felblade_aura();
    new spell_dh_anger_of_the_half_giants();
    new spell_dh_shattered_souls();
    new spell_soul_fragments_tracker();
    new spell_dh_demonic_trample();

    /// Artifact Traits
    new spell_dh_charred_warblades();
    new spell_dh_contained_fury();
    new spell_dh_fueled_by_pain();
    new spell_dh_chaos_nova();
    new spell_dh_empower_wards();
    new spell_dh_soul_carver();
    new spell_dh_illidari_knowledge();
    new spell_dh_fiery_demise();
    new spell_dh_demonic_flames();

    /// Havoc
    new spell_dh_metamorphosis_transform();
    new spell_dh_chaos_strike_mh();
    new spell_dh_chaos_strike_off();
    new spell_dh_consume_soul_havoc();
    new spell_dh_chaotic_onslaught();
    RegisterSpellScript(spell_dh_eye_beam_proc);
    new spell_eye_of_leotheras();

    /// Vengeance
    new spell_dh_razor_spikes();
    new spell_dh_fallout();
    new spell_dh_demonic_infusion();
    new spell_dh_flaming_soul();
    new spell_dh_veng_consume_soul_heal();
    new spell_dh_sigil_of_chains_interrupting();
    new spell_dh_soul_fragment_consume_oldest();

    /// Honor Talents
    new spell_dh_glimpse();
    new spell_dh_rushing_vault();
    new spell_dh_solitude();
    new spell_dh_mana_break();
    new spell_dh_reverse_magic();
    new spell_dh_demonic_origins();
    new spell_dh_cleansed_by_flame();
    new spell_dh_cleansed_by_flame_proc();
    new spell_dh_illidans_grasp();
    new spell_dh_illidans_grasp_throw();
    new spell_dh_illidans_grasp_dummy();

    /// PlayerScript
    new PlayerScript_dh_infernal_strike();
    new PlayerScript_dh_learn_spells();
    new PlayerScript_dh_felblade_auras();
    new PlayerScript_dh_spec_change();

    /// PvP Modifiers
    new spell_dh_metamorphosis_leech();
    new spell_dh_chaos_blades_pvp();

    /// Legendary
    new spell_dh_eternal_hunger();
}
#endif
