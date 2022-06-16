////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_DEATHKNIGHT and SPELLFAMILY_GENERIC spells used by deathknight players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dk_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Containers.h"
#include "GridNotifiersImpl.h"
#include "HelperDefines.h"
#include "CellImpl.h"

enum DeathKnightSpells
{
    DK_SPELL_RUNIC_POWER_ENERGIZE               = 49088,
    DK_SPELL_ANTI_MAGIC_SHELL_TALENT            = 51052,
    DK_SPELL_BLOOD_BOIL_TRIGGERED               = 65658,
    DK_SPELL_BLOOD_PLAGUE                       = 55078,
    DK_SPELL_FROST_FEVER                        = 55095,
    DK_SPELL_GHOUL_AS_GUARDIAN                  = 46585,
    DK_SPELL_GHOUL_AS_PET                       = 52150,
    DK_SPELL_BLOOD_BOIL                         = 50842,
    DK_SPELL_CHILBLAINS                         = 50041,
    DK_SPELL_PLAGUE_LEECH                       = 123693,
    DK_SPELL_PERDITION                          = 123981,
    DK_SPELL_SHROUD_OF_PURGATORY                = 116888,
    DK_SPELL_PURGATORY_INSTAKILL                = 123982,
    DK_SPELL_DEATH_SIPHON_HEAL                  = 116783,
    DK_SPELL_BLOOD_CHARGE                       = 114851,
    DK_SPELL_PILLAR_OF_FROST                    = 51271,
    DK_SPELL_SOUL_REAPER_HASTE                  = 114868,
    DK_SPELL_SOUL_REAPER_DAMAGE                 = 114867, ///< 7.0.3 deprecated
    DK_SPELL_REMORSELESS_WINTER_STUN            = 115001,
    DK_SPELL_REMORSELESS_WINTER                 = 115000,
    DK_SPELL_SCENT_OF_BLOOD                     = 49509,
    DK_SPELL_SCENT_OF_BLOOD_AURA                = 50421,
    DK_SPELL_CHAINS_OF_ICE                      = 45524,
    DK_SPELL_EBON_PLAGUEBRINGER                 = 51160,
    DK_SPELL_DESECRATED_GROUND                  = 118009,
    DK_SPELL_DESECRATED_GROUND_IMMUNE           = 115018,
    DK_SPELL_ASPHYXIATE                         = 108194,
    DK_SPELL_DARK_INFUSION_STACKS               = 91342, ///< 7.0.3 deprecated
    DK_SPELL_DARK_INFUSION_AURA                 = 93426,
    DK_NPC_WILD_MUSHROOM                        = 59172,
    DK_SPELL_GLYPH_OF_CORPSE_EXPLOSION          = 127344,
    DK_SPELL_GLYPH_OF_HORN_OF_WINTER            = 58680,
    DK_SPELL_GLYPH_OF_HORN_OF_WINTER_EFFECT     = 121920,
    DK_SPELL_DEATH_COIL_DAMAGE                  = 47632,
    DK_SPELL_DEATH_STRIKE_HEAL                  = 45470,
    DK_SPELL_PLAGUEBEARER                       = 161497,
    DK_SPELL_NECROTIC_PLAGUE                    = 152281,
    DK_SPELL_DEATH_PACT                         = 48743,
    DK_SPELL_ICY_TOUCH                          = 45477,
    DK_SPELL_CHILBLAINS_TRIGGER                 = 50435,
    DK_SPELL_NECROTIC_PLAGUE_ENERGIZE           = 155165,
    DK_SPELL_EMPOWERED_OBLITERATE               = 157409,
    DK_SPELL_FREEZING_FOG_AURA                  = 59052,
    DK_SPELL_ENHANCED_DEATH_COIL                = 157343,
    DK_SPELL_SHADOW_OF_DEATH                    = 164047,
    DK_SPELL_DEATH_COIL                         = 47541,
    DK_WOD_PVP_UNHOLY_4P_BONUS                  = 166061,
    DK_WOD_PVP_UNHOLY_4P_BONUS_EFFECT           = 166062,
    DK_WOD_PVP_FROST_4P_BONUS                   = 166056,
    DK_WOD_PVP_FROST_4P_BONUS_EFFECT            = 166057,

    PestilentPustules                           = 194917
};

/// last update : 7.1.5
/// Gorefiend's Grasp - 108199
class spell_dk_gorefiends_grasp: public SpellScriptLoader
{
    public:
        spell_dk_gorefiends_grasp() : SpellScriptLoader("spell_dk_gorefiends_grasp") { }

        class spell_dk_gorefiends_grasp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_gorefiends_grasp_SpellScript);

            enum eSpells
            {
                DeathGripOnlyJump           = 146599,
                GorefiendsGraspGripVisual   = 114869,
                BloodDeathKnight            = 137008,
                DeathGripTaunt              = 51399
            };

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->IsPlayer())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                std::list<Unit*> l_TempList;
                std::list<Unit*> l_GripList;

                l_Player->GetAttackableUnitListInRange(l_TempList, 20.0f);
                for (auto l_Itr : l_TempList)
                {
                    if (l_Itr->GetGUID() == l_Player->GetGUID() ||
                        !l_Player->IsValidAttackTarget(l_Itr) ||
                        l_Itr->IsImmunedToSpell(GetSpellInfo()) ||
                        !l_Itr->IsWithinLOSInMap(l_Player))
                        continue;

                    l_GripList.push_back(l_Itr);
                }

                bool l_Success = false;

                for (auto l_Itr : l_GripList)
                {
                    if (l_Itr->ToCreature() && !l_Itr->ToCreature()->IsDungeonBoss() &&
                        l_Itr->GetEntry() != 125615) ///< Viceroy Nezhar Guards
                    {
                        Spell* l_CurrentSpell = l_Itr->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
                        if (!l_CurrentSpell)
                        {
                            l_CurrentSpell = l_Itr->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL);
                            if (!l_CurrentSpell)
                            {
                                l_CurrentSpell = l_Itr->GetCurrentSpell(CURRENT_GENERIC_SPELL);
                                if (!l_CurrentSpell)
                                    l_CurrentSpell = l_Itr->GetCurrentSpell(CURRENT_MELEE_SPELL);
                            }
                        }

                        if (l_CurrentSpell)
                            l_CurrentSpell->cancel();

                        if (!l_Itr->HasUnitState(UNIT_STATE_STUNNED))
                        {
                            uint32 l_Distance = l_Itr->GetExactDist2d(l_Caster);
                            uint32 l_StunDuration = 300 + (l_Distance * l_Distance);
                            l_Itr->AddUnitState(UNIT_STATE_STUNNED);
                            l_Itr->AddDelayedEvent([l_Itr]() -> void
                            {
                                l_Itr->ClearUnitState(UNIT_STATE_STUNNED);
                            }, l_StunDuration);
                        }
                    }

                    l_Success = true;

                    l_Itr->CastSpell(l_Target, eSpells::DeathGripOnlyJump, true);
                    l_Itr->CastSpell(l_Target, eSpells::GorefiendsGraspGripVisual, true);

                    if (l_Player->HasAura(eSpells::BloodDeathKnight))
                        l_Caster->CastSpell(l_Itr, eSpells::DeathGripTaunt, true);
                }

                /// Death Grip is considered as a loss of control, it should proc Sephuz's Secret if at least one target got gripped
                if (l_Success)
                {
                    std::vector<uint32> l_AuraIds = { 208051, 236763, 234867 };
                    for (auto l_AuraId : l_AuraIds)
                    {
                        if (Aura* l_Aura = l_Player->GetAura(l_AuraId))
                        {
                            l_Aura->SetScriptData(0, 0);
                            break;
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_gorefiends_grasp_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_gorefiends_grasp_SpellScript();
        }
};

// Desecrated ground - 118009
class spell_dk_desecrated_ground: public SpellScriptLoader
{
    public:
        spell_dk_desecrated_ground() : SpellScriptLoader("spell_dk_desecrated_ground") { }

        class spell_dk_desecrated_ground_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_desecrated_ground_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (GetCaster())
                    if (DynamicObject* dynObj = GetCaster()->GetDynObject(DK_SPELL_DESECRATED_GROUND))
                        if (GetCaster()->GetDistance(dynObj) <= 8.0f)
                            GetCaster()->CastSpell(GetCaster(), DK_SPELL_DESECRATED_GROUND_IMMUNE, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_desecrated_ground_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_desecrated_ground_AuraScript();
        }
};

/// last update : 7.0.3
/// Festering Strike - 85948
class spell_dk_festering_strike: public SpellScriptLoader
{
    public:
        spell_dk_festering_strike() : SpellScriptLoader("spell_dk_festering_strike") { }

        class spell_dk_festering_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_festering_strike_SpellScript);

            enum eSpells
            {
                FesteringStrike = 85948,
                FesteringWound  = 194310,
                Castigator      = 207305
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FesteringStrike))
                        {
                            uint32 l_MinBp = l_SpellInfo->Effects[EFFECT_2].BasePoints + 1;
                            uint32 l_MaxBp = l_SpellInfo->Effects[EFFECT_2].BasePoints + l_SpellInfo->Effects[EFFECT_2].DieSides;

                            uint32 l_StackNumber = urand(l_MinBp, l_MaxBp);

                            if (AuraEffect* l_CastigatorAura = l_Caster->GetAuraEffect(eSpells::Castigator, EFFECT_0)) // Castigator
                            {
                                Spell const* l_Spell = GetSpell();

                                if (l_Spell != nullptr && l_Spell->IsCritForTarget(l_Target))
                                    l_StackNumber += l_CastigatorAura->GetAmount();
                            }

                            for (uint32 l_Itr = 0; l_Itr < l_StackNumber; l_Itr++)
                                l_Caster->CastSpell(l_Target, eSpells::FesteringWound, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_festering_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_festering_strike_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Death Strike heal - 45470
class spell_dk_death_strike_heal: public SpellScriptLoader
{
    public:
        spell_dk_death_strike_heal() : SpellScriptLoader("spell_dk_death_strike_heal") { }

        class spell_dk_death_strike_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_strike_heal_SpellScript);

            enum eSpells
            {
                ScentOfBloodAura = 50421,
                DarkSuccor       = 101568
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::ScentOfBloodAura))
                    l_Caster->RemoveAura(eSpells::ScentOfBloodAura);
            }

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Heal = GetHitHeal();

                if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::DarkSuccor))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DarkSuccor))
                        AddPct(l_Heal, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                SetHitHeal(l_Heal);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_dk_death_strike_heal_SpellScript::HandleOnHit);
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_strike_heal_SpellScript::HandleHeal, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_strike_heal_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Howling Blast - 49184
class spell_dk_howling_blast: public SpellScriptLoader
{
    public:
        spell_dk_howling_blast() : SpellScriptLoader("spell_dk_howling_blast") { }

        class spell_dk_howling_blast_SpellScript : public SpellScript
        {
            enum eSpells
            {
                Rime                = 59052,
                EbonMartyr          = 216059,
                HowlingBlastAOE     = 237680,
                RemorselessWinter   = 196770,
                GatheringStormAura  = 211805,
                GatheringStorm      = 194912
            };

            PrepareSpellScript(spell_dk_howling_blast_SpellScript);

            bool m_HadRimeProc = false;

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                /// Must be handled there because under Rime proc Howling Blast will consume no RUNE_POWER
                if (!l_Caster || !l_Caster->HasAura(eSpells::Rime))
                    return;

                m_HadRimeProc = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                /// Must be handled there because under Rime proc Howling Blast will consume no RUNE_POWER
                if (!l_Caster || !m_HadRimeProc)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::RemorselessWinter);
                if (!l_Aura)
                    return;

                if (AuraEffect* l_GatheringStorm = l_Caster->GetAuraEffect(eSpells::GatheringStorm, SpellEffIndex::EFFECT_0))
                {
                    l_Aura->SetDuration(l_Aura->GetDuration() + 500);
                    l_Caster->CastSpell(l_Caster, eSpells::GatheringStormAura, true);
                }
            }

            void HandleOnEffectHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EbonMartyr);
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                uint32 l_Damage = GetHitDamage();

                std::set<uint64>& l_WinterTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RemorlessWinterTargets];
                if (l_Caster->HasAura(eSpells::EbonMartyr) && l_WinterTargets.find(l_Target->GetGUID()) != l_WinterTargets.end())
                {
                    AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                    SetHitDamage(l_Damage);
                }
            }

            void HandleOnHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, DK_SPELL_FROST_FEVER, true);
                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HowlingBlastTargetGUID) = l_Target->GetGUID();
                l_Caster->CastSpell(l_Target, eSpells::HowlingBlastAOE, true);
            }

            void Register()
            {
                OnPrepare += SpellOnPrepareFn(spell_dk_howling_blast_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_dk_howling_blast_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_dk_howling_blast_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnHit += SpellHitFn(spell_dk_howling_blast_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_howling_blast_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Howling Blast - 237680
class spell_dk_howling_blast_aoe: public SpellScriptLoader
{
    public:
        spell_dk_howling_blast_aoe() : SpellScriptLoader("spell_dk_howling_blast_aoe") { }

        class spell_dk_howling_blast_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_howling_blast_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HowlingBlastTargetGUID) == p_Object->GetGUID())
                        return true;

                    return false;
                });
            }

            void HandleOnHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, DK_SPELL_FROST_FEVER, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_howling_blast_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_dk_howling_blast_aoe_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_howling_blast_aoe_SpellScript();
        }
};

// Conversion - 119975
class spell_dk_conversion: public SpellScriptLoader
{
    public:
        spell_dk_conversion() : SpellScriptLoader("spell_dk_conversion") { }

        class spell_dk_conversion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_conversion_AuraScript);

            enum eSpells
            {
                ConversionRegen = 119980
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                int32 l_CurrentPower = l_Caster->GetPower(POWER_RUNIC_POWER) / l_Caster->GetPowerCoeff(POWER_RUNIC_POWER);

                if (l_CurrentPower < p_AurEff->GetAmount())
                    l_Caster->RemoveAura(GetSpellInfo()->Id);

                l_Caster->CastSpell(l_Caster, eSpells::ConversionRegen, true);
                l_Caster->EnergizeBySpell(l_Caster, eSpells::ConversionRegen, -(p_AurEff->GetAmount() * l_Caster->GetPowerCoeff(POWER_RUNIC_POWER)), POWER_RUNIC_POWER);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_conversion_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_conversion_AuraScript();
        }
};

// Soul Reaper - 130736 (unholy) or 130735 (frost) or 114866 (blood)
class spell_dk_soul_reaper: public SpellScriptLoader
{
    public:
        spell_dk_soul_reaper() : SpellScriptLoader("spell_dk_soul_reaper") { }

        class spell_dk_soul_reaper_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_soul_reaper_SpellScript);

            enum eSpells
            {
                T18Blood2P = 187872
            };

            void HandleAfterHit()
            {
                if (!GetCaster())
                    return;

                if (Player* l_Caster = GetCaster()->ToPlayer())
                {
                    /// Only in blood spec
                    if (l_Caster->GetActiveSpecializationID() == SPEC_DK_BLOOD)
                    {
                        l_Caster->CastSpell(l_Caster, DK_SPELL_SCENT_OF_BLOOD_AURA, true);

                        if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::T18Blood2P, EFFECT_1))
                        {
                            if (roll_chance_i(l_AurEff->GetAmount()))
                                l_Caster->CastSpell(l_Caster, DK_SPELL_SCENT_OF_BLOOD_AURA, true);
                        }
                    }
                }
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_dk_soul_reaper_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_soul_reaper_SpellScript();
        }
};

// Purgatory - 116888
class spell_dk_purgatory: public SpellScriptLoader
{
    public:
        spell_dk_purgatory() : SpellScriptLoader("spell_dk_purgatory") { }

        class spell_dk_purgatory_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_purgatory_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                {
                    AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                    if (removeMode == AURA_REMOVE_BY_EXPIRE)
                        _player->CastSpell(_player, DK_SPELL_PURGATORY_INSTAKILL, true);
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_purgatory_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_purgatory_AuraScript();
        }
};

// Purgatory - 114556
class spell_dk_purgatory_absorb: public SpellScriptLoader
{
    public:
        spell_dk_purgatory_absorb() : SpellScriptLoader("spell_dk_purgatory_absorb") { }

        class spell_dk_purgatory_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_purgatory_absorb_AuraScript);

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*auraEffect*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Unit* target = GetTarget();

                if (dmgInfo.GetAmount() < target->GetHealth())
                    return;

                // No damage received under Shroud of Purgatory
                if (target->ToPlayer()->HasAura(DK_SPELL_SHROUD_OF_PURGATORY))
                {
                    absorbAmount = dmgInfo.GetAmount();
                    return;
                }

                if (target->ToPlayer()->HasAura(DK_SPELL_PERDITION))
                    return;

                int32 bp = dmgInfo.GetAmount();

                target->CastCustomSpell(target, DK_SPELL_SHROUD_OF_PURGATORY, &bp, NULL, NULL, true);
                target->CastSpell(target, DK_SPELL_PERDITION, true);
                target->SetHealth(2);
                absorbAmount = dmgInfo.GetAmount();
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_purgatory_absorb_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_purgatory_absorb_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_purgatory_absorb_AuraScript();
        }
};

// Plague Leech - 123693
class spell_dk_plague_leech: public SpellScriptLoader
{
    public:
        spell_dk_plague_leech() : SpellScriptLoader("spell_dk_plague_leech") { }

        class spell_dk_plague_leech_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_plague_leech_SpellScript);

            enum eSpells
            {
                NecroticPlague = 152281,
                BloodPlague = 55078,
                FrostEver = 55095
            };

            std::list<uint8> m_LstRunesUsed;

            SpellCastResult CheckRunes()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Player == nullptr || l_Target == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                for (uint8 i = 0; i < MAX_RUNES; ++i)
                {
                    if (l_Player->GetRuneCooldown(i))
                        m_LstRunesUsed.push_back(i);
                }

                if (m_LstRunesUsed.size() < 2)
                    return SPELL_FAILED_DONT_REPORT;

                if (!l_Target->HasAura(eSpells::BloodPlague, l_Player->GetGUID()) || !l_Target->HasAura(eSpells::FrostEver, l_Player->GetGUID()))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Player == nullptr || l_Target == nullptr)
                    return;

                l_Target->RemoveAura(eSpells::FrostEver, l_Player->GetGUID());
                l_Target->RemoveAura(eSpells::BloodPlague, l_Player->GetGUID());
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_plague_leech_SpellScript::CheckRunes);
                OnHit += SpellHitFn(spell_dk_plague_leech_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_plague_leech_SpellScript();
        }
};

// Unholy Blight - 115994
class spell_dk_unholy_blight: public SpellScriptLoader
{
    public:
        spell_dk_unholy_blight() : SpellScriptLoader("spell_dk_unholy_blight") { }

        class spell_dk_unholy_blight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_unholy_blight_SpellScript);

            enum eSpells
            {
                chilblains = 50041,
                chilblainsAura = 50435
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, DK_SPELL_BLOOD_PLAGUE, true);
                l_Caster->CastSpell(l_Target, DK_SPELL_FROST_FEVER, true);

                if (l_Caster->HasAura(eSpells::chilblains))
                    l_Caster->CastSpell(l_Target, eSpells::chilblainsAura, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_dk_unholy_blight_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_unholy_blight_SpellScript();
        }
};

/// last update : 7.0.3
/// Outbreak - 77575
class spell_dk_outbreak: public SpellScriptLoader
{
    public:
        spell_dk_outbreak() : SpellScriptLoader("spell_dk_outbreak") { }

        class spell_dk_outbreak_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_outbreak_SpellScript);

            enum eSpells
            {
                OutbreakExtendAura = 196782
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::OutbreakExtendAura, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_dk_outbreak_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_outbreak_SpellScript();
        }
};

/// last update : 7.3.5 build 26365
/// Outbreak - 196782
class spell_dk_outbreak_periodic : public SpellScriptLoader
{
    public:
        spell_dk_outbreak_periodic() : SpellScriptLoader("spell_dk_outbreak_periodic") { }

        class spell_dk_outbreak_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_outbreak_periodic_AuraScript);

            enum eSpells
            {
                OutbreakVirulentPlague = 196780,
                EbonFever              = 207269,
                VirtulentPlague        = 191587
            };

            void HandlePeriodic(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (!l_Target || !l_Caster)
                    return;

                uint32 l_Duration = 0;
                if (Aura* l_Aura = l_Target->GetAura(eSpells::VirtulentPlague, l_Caster->GetGUID()))
                    l_Duration = l_Aura->GetDuration();

                l_Caster->CastSpell(l_Target, eSpells::OutbreakVirulentPlague, true);

                if (l_Duration)
                {
                    if (Aura* l_NewAura = l_Target->GetAura(eSpells::VirtulentPlague, l_Caster->GetGUID()))
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VirtulentPlague))
                        {
                            uint32 l_MaxSpellDuration = l_SpellInfo->GetMaxDuration();

                            if (l_Caster->HasAura(eSpells::EbonFever))
                                l_MaxSpellDuration /= 2;

                            if (l_Duration > CalculatePct(l_MaxSpellDuration, 30))
                            {
                                if (l_NewAura->GetMaxDuration() < l_MaxSpellDuration + CalculatePct(l_MaxSpellDuration, 30))
                                    l_NewAura->SetMaxDuration(l_MaxSpellDuration + CalculatePct(l_MaxSpellDuration, 30));

                                l_NewAura->SetDuration(l_NewAura->GetMaxDuration());
                            }
                            else
                            {
                                 if (l_NewAura->GetMaxDuration() < l_MaxSpellDuration + l_Duration)
                                     l_NewAura->SetMaxDuration(l_MaxSpellDuration + l_Duration);

                                l_NewAura->SetDuration(l_NewAura->GetMaxDuration());
                            }
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_outbreak_periodic_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_outbreak_periodic_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Outbreak 196780 (extend Virulent Plague - 191587), and Pandemic - 199799
class spell_outbreak_aura : public SpellScriptLoader
{
    public:
        spell_outbreak_aura() : SpellScriptLoader("spell_outbreak_aura") { }

        class spell_outbreak_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_outbreak_aura_SpellScript);

            enum eSpells
            {
                Outbreak                = 196780,
                VirulentPlague          = 191587,
                DebilitatingInfestation = 207316,
                DebilitatingAura        = 208278
            };

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                Aura* l_Aura = l_Target->GetAura(eSpells::VirulentPlague, l_Caster->GetGUID());
                if (l_Aura)
                    l_Aura->RefreshDuration();
                else
                    l_Caster->CastSpell(l_Target, eSpells::VirulentPlague, true);

                if (l_Caster->HasAura(eSpells::DebilitatingInfestation))
                    l_Caster->CastSpell(l_Target, eSpells::DebilitatingAura, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitAuraCheck(false, eSpells::VirulentPlague, l_Caster->GetGUID()));
            }

            void Register()
            {
                if (m_scriptSpellId == eSpells::Outbreak)
                    OnEffectHitTarget += SpellEffectFn(spell_outbreak_aura_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                else
                {
                    OnEffectHitTarget += SpellEffectFn(spell_outbreak_aura_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_outbreak_aura_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_outbreak_aura_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                }
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_outbreak_aura_SpellScript();
        }
};

/// Raise Dead - 46584
/// Last Update 7.0.3
class spell_dk_raise_dead: public SpellScriptLoader
{
    public:
        spell_dk_raise_dead() : SpellScriptLoader("spell_dk_raise_dead") { }

        class spell_dk_raise_dead_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_raise_dead_SpellScript);

            enum eSpells
            {
                GhoulPet          = 52150,
                SludgeBelcherPet  = 212027,
                SludgeBelcherAura = 207313,
                AllWillServeAura  = 194916,
                RaiseSkulker      = 196910
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return SPELL_FAILED_SUCCESS;

                Player* l_Player = l_Caster->ToPlayer();

                if (!l_Player)
                    return SPELL_FAILED_SUCCESS;

                if (Pet* l_pet = l_Player->GetPet())
                    l_pet->DespawnOrUnsummon();

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::SludgeBelcherAura))
                        l_Caster->CastSpell(l_Caster, eSpells::SludgeBelcherPet, true);
                    else
                        l_Caster->CastSpell(l_Caster, eSpells::GhoulPet, true);

                    if (l_Caster->HasAura(eSpells::AllWillServeAura))
                        l_Caster->CastSpell(l_Caster, eSpells::RaiseSkulker, true);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_raise_dead_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_dk_raise_dead_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_raise_dead_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Sludge Belcher - 207313
class spell_dk_sludge_belcher : public SpellScriptLoader
{
    public:
        spell_dk_sludge_belcher() : SpellScriptLoader("spell_dk_sludge_belcher") { }

        class spell_dk_sludge_belcher_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_sludge_belcher_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (auto l_Pet : l_Caster->m_Controlled)
                {
                    if (l_Pet && l_Pet->GetEntry() == DeathKnightPet::Abomination)
                    {
                        l_Pet->ToCreature()->DespawnOrUnsummon();
                        break;
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_sludge_belcher_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_sludge_belcher_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by All Will Serve - 194916
class spell_dk_all_will_serve : public SpellScriptLoader
{
    public:
        spell_dk_all_will_serve() : SpellScriptLoader("spell_dk_all_will_serve") { }

        class spell_dk_all_will_serve_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_all_will_serve_AuraScript);

            enum eNPCs
            {
                RisenSkulker = 99541
            };

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (auto l_Pet : l_Caster->m_Controlled)
                {
                    if (l_Pet && l_Pet->GetEntry() == eNPCs::RisenSkulker)
                    {
                        l_Pet->ToCreature()->DespawnOrUnsummon();
                        break;
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_all_will_serve_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_all_will_serve_AuraScript();
        }
};

// 50462 - Anti-Magic Shell (on raid member)
class spell_dk_anti_magic_shell_raid: public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_raid() : SpellScriptLoader("spell_dk_anti_magic_shell_raid") { }

        class spell_dk_anti_magic_shell_raid_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_raid_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // TODO: this should absorb limited amount of damage, but no info on calculation formula
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                 absorbAmount = CalculatePct(dmgInfo.GetAmount(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_raid_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_raid_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_shell_raid_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Anti-Magic Shell (on self) - 48707
class spell_dk_anti_magic_shell_self: public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_self() : SpellScriptLoader("spell_dk_anti_magic_shell_self") { }

        class spell_dk_anti_magic_shell_self_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_self_AuraScript);

            int32 m_AbsorbPct, m_HpPct = 0;
            uint32 m_Absorbed, m_AmountAbsorb = 0;

            enum eSpells
            {
                AntiMagicShell           = 48707,
                WoDPvPBlood4PBonus       = 171456,
                AntiMagicBarrier         = 205727,
                AntiMagicBarrierAura     = 205725,
                AcherusDrapes            = 210852,
                AcherusDrapesHeal        = 210862,
                VolatileShielding        = 207188
            };

            std::vector<AuraType> m_AuraTypesToRemove =
            {
                SPELL_AURA_MOD_DECREASE_SPEED,
                SPELL_AURA_MOD_ROOT,
                SPELL_AURA_MOD_ROOT_2,
                SPELL_AURA_PERIODIC_DAMAGE,
                SPELL_AURA_PERIODIC_SCHOOL_DAMAGE,
                SPELL_AURA_PERIODIC_DAMAGE_PERCENT,
                SPELL_AURA_PERIODIC_LEECH,
                SPELL_AURA_PERIODIC_HEALTH_FUNNEL
            };

            bool Load() override
            {
                m_Absorbed = 0;
                m_AbsorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                m_HpPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(DK_SPELL_RUNIC_POWER_ENERGIZE))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (GetCaster())
                {
                    amount = GetCaster()->CountPctFromMaxHealth(m_HpPct);
                    m_AmountAbsorb = amount;
                }
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& /*absorbAmount*/)
            {
                m_Absorbed += dmgInfo.GetAmount();
            }

            void Trigger(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                /// damage absorbed by Anti-Magic Shell energizes the DK with additional runic power.
                /// 1% of lost HP restore DK 2 runic power
                uint32 l_MaxHealth = l_Target->GetMaxHealth();
                uint32 l_AbsorbAmount = absorbAmount;
                float l_Percent = (float)l_AbsorbAmount / (float)l_MaxHealth * 200.0f;
                int32 l_Bp = (int32)(l_Percent * 10);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::VolatileShielding, EFFECT_1))
                    AddPct(l_Bp, l_AuraEffect->GetAmount());

                l_Target->EnergizeBySpell(l_Target, DK_SPELL_RUNIC_POWER_ENERGIZE, l_Bp, POWER_RUNIC_POWER);

                if (l_Caster->HasAura(eSpells::AcherusDrapes))
                {
                    int32 l_Heal = l_AbsorbAmount;
                    l_Caster->CastCustomSpell(l_Caster, eSpells::AcherusDrapesHeal, &l_Heal, nullptr, nullptr, true);
                }
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (l_Player->GetActiveSpecializationID() == SPEC_DK_FROST)
                    {
                        for (auto l_AuraType : m_AuraTypesToRemove)
                        {
                            Unit::AuraEffectList l_Auras = l_Caster->GetAuraEffectsByType(l_AuraType);

                            if (l_Auras.empty())
                                continue;

                            for (auto l_AuraEffect : l_Auras)
                                if (Aura* l_Aura = l_AuraEffect->GetBase())
                                    if (AuraApplication* l_AuraApp = l_Aura->GetApplicationOfTarget(l_Caster->GetGUID()))
                                        if (SpellInfo const* l_AuraSpellInfo = l_Aura->GetSpellInfo())
                                            if (l_AuraSpellInfo->SchoolMask & SPELL_SCHOOL_MASK_MAGIC)
                                                l_Caster->RemoveAura(l_AuraApp);
                        }
                    }
                }

                if (l_Caster->HasAura(eSpells::WoDPvPBlood4PBonus))
                {
                    if (l_Caster->GetGUID() != l_Target->GetGUID())
                        return;

                    std::list<Unit*> l_TargetList;
                    float l_Radius = 30.0f;

                    JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Ucheck(l_Target, l_Target, l_Radius);
                    JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(l_Target, l_TargetList, l_Ucheck);
                    l_Target->VisitNearbyObject(l_Radius, l_Searcher);

                    l_TargetList.remove_if([this, l_Caster, l_Target](Unit* p_Unit) -> bool
                    {
                        if (p_Unit == nullptr)
                            return true;

                        if (!l_Caster->IsValidAssistTarget(p_Unit))
                            return true;

                        if (l_Caster->GetGUID() == p_Unit->GetGUID())
                            return true;

                        return false;
                    });

                    l_TargetList.sort(JadeCore::WorldObjectDistanceCompareOrderPred(l_Caster));

                    if (l_TargetList.size() > 2)
                        l_TargetList.resize(2);

                    for (auto l_Itr : l_TargetList)
                        l_Itr->CastCustomSpell(l_Itr, GetSpellInfo()->Id, 0, NULL, NULL, true, NULL, nullptr, l_Caster->GetGUID());
                }

                if (l_Caster->HasSpell(eSpells::AntiMagicBarrier))
                {
                    SpellInfo const * l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AntiMagicBarrierAura);

                    if (l_SpellInfo == nullptr)
                        return;

                    int32 l_Bp = CalculatePct(l_Caster->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_1].BasePoints);
                    l_Caster->CastCustomSpell(l_Caster, eSpells::AntiMagicBarrierAura, &l_Bp, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_self_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_dk_anti_magic_shell_self_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Trigger, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_anti_magic_shell_self_AuraScript();
        }
};

// 50461 - Anti-Magic Zone
class spell_dk_anti_magic_zone: public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_zone() : SpellScriptLoader("spell_dk_anti_magic_zone") { }

        class spell_dk_anti_magic_zone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_zone_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                amount = 136800;
                if (Player* player = GetCaster()->ToPlayer())
                     amount += int32(player->GetStat(STAT_STRENGTH) * 4);
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                 absorbAmount = CalculatePct(dmgInfo.GetAmount(), absorbPct);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_zone_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_zone_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_zone_AuraScript();
        }
};

/// Last update 7.0.3
/// Death Gate - 53822
class spell_dk_death_gate_teleport: public SpellScriptLoader
{
    public:
        spell_dk_death_gate_teleport() : SpellScriptLoader("spell_dk_death_gate_teleport") {}

        class spell_dk_death_gate_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_gate_teleport_SpellScript);

            enum eMapIds
            {
                EasternKingdoms = 0,
                TheBrokenIsles  = 1220
            };

            enum eAreaIds
            {
                AcherusTheEbonHoldOld   = 4281,
                AcherusTheEbonHoldNew   = 7679
            };

            SpellCastResult CheckClass()
            {
                if (GetCaster()->getClass() != CLASS_DEATH_KNIGHT)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_DEATH_KNIGHT);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }
                return SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (!IsInEbonHold(l_Player))
                    {
                        l_Player->SaveRecallPosition();
                        if (l_Player->getLevel() >= 98)
                            l_Player->TeleportTo(1220, -1442.2021f, 1161.9827f, 275.2528f, 4.1646f);
                        else
                            l_Player->TeleportTo(0, 2359.41f, -5662.084f, 382.259f, 0.60f);
                    }
                    else
                    {
                        l_Player->TeleportTo(l_Player->m_recallMap, l_Player->m_recallX, l_Player->m_recallY, l_Player->m_recallZ, l_Player->m_recallO);
                    }
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_teleport_SpellScript::CheckClass);
                AfterCast += SpellCastFn(spell_dk_death_gate_teleport_SpellScript::HandleAfterCast);
            }

        private:

            bool IsInEbonHold(Player* l_Player)
            {
                if (l_Player->GetMapId() == eMapIds::EasternKingdoms && l_Player->GetAreaId() == eAreaIds::AcherusTheEbonHoldOld)
                {
                    return true;
                }
                else if (l_Player->GetMapId() == eMapIds::TheBrokenIsles && l_Player->GetAreaId() == eAreaIds::AcherusTheEbonHoldNew)
                {
                    return true;
                }

                return false;
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_gate_teleport_SpellScript();
        }
};

// Death Gate - 52751
class spell_dk_death_gate: public SpellScriptLoader
{
    public:
        spell_dk_death_gate() : SpellScriptLoader("spell_dk_death_gate") {}

        class spell_dk_death_gate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_gate_SpellScript);

            SpellCastResult CheckClass()
            {
                if (GetCaster()->getClass() != CLASS_DEATH_KNIGHT)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_DEATH_KNIGHT);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, GetEffectValue(), false);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_SpellScript::CheckClass);
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_gate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_gate_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Blood Boil - 50842
class spell_dk_blood_boil: public SpellScriptLoader
{
    public:
        spell_dk_blood_boil() : SpellScriptLoader("spell_dk_blood_boil") { }

        class spell_dk_blood_boil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_blood_boil_SpellScript);

            enum eSpells
            {
                BloodPlague = 55078,
                Soulgorge   = 212744
            };

            void HandleHitTarget(SpellEffIndex /* p_EffIndex */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                /// The spell can be casted by Dancing Rune Weapon
                if (Player* l_OriginalCaster = l_Caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    if (l_OriginalCaster->HasSpell(eSpells::Soulgorge))
                        return;

                    l_Caster->CastSpell(l_Target, eSpells::BloodPlague, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_blood_boil_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_blood_boil_SpellScript();
        }
};

enum DeathGripSpells
{
    SpellImprovedDeathGrip = 157367
};

// Corpse Explosion (Glyph) - 127344
class spell_dk_corpse_explosion: public SpellScriptLoader
{
    public:
        spell_dk_corpse_explosion() : SpellScriptLoader("spell_dk_corpse_explosion") { }

        class spell_dk_corpse_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_corpse_explosion_SpellScript);

            SpellCastResult CheckTarget()
            {
                // Any effect on Mechanical or Elemental units
                if (Unit* caster = GetCaster())
                {
                    Unit* target = GetExplTargetUnit();
                    if (!target)
                        return SPELL_FAILED_NO_VALID_TARGETS;

                    if (Creature* c = target->ToCreature())
                    {
                        if (c->GetCreatureTemplate() && (c->GetCreatureTemplate()->type == CREATURE_TYPE_MECHANICAL ||
                                                         c->GetCreatureTemplate()->type == CREATURE_TYPE_ELEMENTAL))
                            return SPELL_FAILED_BAD_TARGETS;
                        else if (c->IsDungeonBoss())
                            return SPELL_FAILED_BAD_TARGETS;
                    }
                    else if (target->GetGUID() == caster->GetGUID())
                        return SPELL_FAILED_BAD_TARGETS;

                    if (target->isAlive())
                        return SPELL_FAILED_BAD_TARGETS;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_corpse_explosion_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_corpse_explosion_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Runic Empowerment - 81229
class spell_dk_runic_empowerment : public PlayerScript
{
    public:
        spell_dk_runic_empowerment() : PlayerScript("spell_dk_runic_empowerment") {}

        enum eSpells
        {
           RunicEmpowerment = 81229,
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After || p_Player->getClass() != CLASS_DEATH_KNIGHT || p_Power != POWER_RUNIC_POWER || p_Regen || p_NewValue >= p_OldValue)
                return;

            AuraEffect* l_RunicEmpowerment = p_Player->GetAuraEffect(eSpells::RunicEmpowerment, EFFECT_0);
            int32 l_PowerSpent = p_OldValue - p_NewValue;
            if (!l_RunicEmpowerment)
                return;

            float l_Chance = float((l_RunicEmpowerment->GetAmount() * float(l_PowerSpent / p_Player->GetPowerCoeff(p_Power))) / 10); ///< 1.50% chance per Runic Power spent
            if (roll_chance_f(l_Chance))
                p_Player->RestoreRune();
        }
};

/// Last Update 7.1.5 Build 23420
/// Runic Corruption - 51462
class spell_dk_runic_corruption : public SpellScriptLoader
{
    public:
        spell_dk_runic_corruption() : SpellScriptLoader("spell_dk_runic_corruption") { }

        enum eSpells
        {
            RunicCorruption     = 51460,
            RunicCorruptionAura = 51462
        };

        class spell_dk_runic_corruption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_runic_corruption_AuraScript);

            Guid128 m_CastId;

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                if (!GetUnitOwner() || !GetUnitOwner()->IsPlayer() || !p_ProcEventInfo.GetDamageInfo() || !p_ProcEventInfo.GetDamageInfo()->GetSpell())
                    return;

                if (m_CastId == p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid())
                    return;

                m_CastId = p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid();

                Player* l_Owner = GetUnitOwner()->ToPlayer();
                if (l_Owner->getClass() != Classes::CLASS_DEATH_KNIGHT)
                    return;

                SpellInfo const* l_TriggerSpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_TriggerSpellInfo || l_TriggerSpellInfo->SpellPowers.empty())
                    return;

                int32 l_PowerCost = 0;
                for (auto l_SpellPower : l_TriggerSpellInfo->SpellPowers)
                {
                    if (l_SpellPower->PowerType == Powers::POWER_RUNIC_POWER)
                        l_PowerCost += l_SpellPower->Cost;
                }

                if (l_PowerCost <= 0)
                    return;

                AuraEffect const* p_ChanceEffect = GetAura()->GetEffect(SpellEffIndex::EFFECT_1);
                if (!p_ChanceEffect)
                    return;

                /// 1.50% chance per Runic Power spent
                float l_Chance = (p_ChanceEffect->GetAmount() / 100.0f) * (l_PowerCost / l_Owner->GetPowerCoeff(Powers::POWER_RUNIC_POWER));

                if (roll_chance_f(l_Chance))
                {
                    if (Aura* l_AuraRunicCorruption = l_Owner->GetAura(eSpells::RunicCorruption))
                        l_AuraRunicCorruption->SetDuration(l_AuraRunicCorruption->GetDuration() + l_AuraRunicCorruption->GetMaxDuration());
                    else
                        l_Owner->CastSpell(l_Owner, eSpells::RunicCorruption, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_runic_corruption_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_runic_corruption_AuraScript();
        }
};

// Death Pact - 48743
class spell_dk_death_pact: public SpellScriptLoader
{
    public:
        spell_dk_death_pact() : SpellScriptLoader("spell_dk_death_pact") { }

        class spell_dk_death_pact_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_death_pact_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Caster = GetCaster())
                    p_Amount = l_Caster->CountPctFromMaxHealth(p_Amount);
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (p_DmgInfo.GetSpellInfo() && p_DmgInfo.GetSpellInfo()->Id == GetSpellInfo()->Id)
                        p_AbsorbAmount = 0;
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_death_pact_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_death_pact_AuraScript::OnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_death_pact_AuraScript();
        }
};

/// Last Update 6.2.3
/// Chains of Ice - 45524
class spell_dk_chain_of_ice: public SpellScriptLoader
{
    public:
        spell_dk_chain_of_ice() : SpellScriptLoader("spell_dk_chain_of_ice") { }

        class spell_dk_chain_of_ice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_chain_of_ice_SpellScript);

            enum eSpells
            {
                ChainOfIceRoot = 96294,
                chilblains = 50041,
                chilblainsAura = 50435,
                GlyphoftheIceReaper = 159416
            };

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();
                SetHitDamage(0);

                if (l_Target == nullptr || l_Player == nullptr)
                    return;

                if (l_Player->HasAura(eSpells::chilblains))
                    l_Player->CastSpell(l_Target, eSpells::chilblainsAura, true);

                if (l_Player->HasAura(DK_SPELL_CHILBLAINS))
                    l_Player->CastSpell(l_Target, eSpells::ChainOfIceRoot, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_dk_chain_of_ice_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_chain_of_ice_SpellScript();
        }
};

/// Chilblains - 50041
class spell_dk_chilblains_aura : public SpellScriptLoader
{
    public:
        spell_dk_chilblains_aura() : SpellScriptLoader("spell_dk_chilblains_aura") { }

        class spell_dk_chilblains_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_chilblains_aura_AuraScript);

            enum eSpell
            {
                HowlingBlast = 49184
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                PreventDefaultAction();
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_chilblains_aura_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_chilblains_aura_AuraScript();
        }
};

enum DeathCoilSpells
{
    SpellGlyphOfDeathsEmbrace = 58679
};

class DarkArbiterEmpowerment
{
    public:
        enum eSpells
        {
            ShadowEmpowerment   = 211947
        };

        enum eNpc
        {
            DarkArbiter = 100876
        };

        void Empower(Unit* p_Caster, int32 p_Rp)
        {
            for (auto l_Summon : p_Caster->m_Controlled)
            {
                if (l_Summon->GetEntry() != eNpc::DarkArbiter)
                    continue;

                int32 l_CountMod = abs(p_Rp / p_Caster->GetPowerCoeff(POWER_RUNIC_POWER));

                if (p_Caster->CanApplyPvPSpellModifiers())
                    l_CountMod *= 0.6f; ///< 60% in PvP

                l_Summon->m_SpellHelper.GetUint32(eSpellHelpers::DarkArbiterEmpowermentValue) += l_CountMod;
                int32 l_Amount = l_Summon->m_SpellHelper.GetUint32(eSpellHelpers::DarkArbiterEmpowermentValue);

                if (AuraEffect* l_Effect = l_Summon->GetAuraEffect(eSpells::ShadowEmpowerment, EFFECT_0))
                    l_Effect->SetAmount(l_Amount);
                else
                    p_Caster->CastCustomSpell(l_Summon, eSpells::ShadowEmpowerment, &l_Amount, nullptr, nullptr, true);
            }
        }
};

/// Called by ID - 251872 Item - Death Knight T21 Unholy 4P Bonus
class spell_dk_item_t21_unholy_4p_proc : public SpellScriptLoader
{
    public:
        spell_dk_item_t21_unholy_4p_proc() : SpellScriptLoader("spell_dk_item_t21_unholy_4p_proc") { }

        class spell_dk_item_t21_unholy_4p_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_item_t21_unholy_4p_proc_AuraScript);

            enum eSpells
            {
                DeathCoilEnemy = 47632
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                const SpellInfo* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Unit* l_Target = p_EventInfo.GetProcTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DeathCoilEnemy, true);
                if (roll_chance_i(l_SpellInfo->ProcChance))
                    l_Caster->CastSpell(l_Target, eSpells::DeathCoilEnemy, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_item_t21_unholy_4p_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_item_t21_unholy_4p_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Death Coil - 47541
class spell_dk_death_coil : public SpellScriptLoader
{
    public:
        spell_dk_death_coil() : SpellScriptLoader("spell_dk_death_coil") { }

        class spell_dk_death_coil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_coil_SpellScript);

            enum eSpells
            {
                GlyphofDeathCoil    = 63333,
                Lichborne           = 49039,
                DeathBarrier        = 115635,
                DeathCoilAlly       = 47633,
                DeathCoilEnemy      = 47632,
                Necrosis            = 207346,
                NecrosisAura        = 216974,
                Defile              = 152280,
                DeathMarch          = 235556,
                DeathAndDecay       = 43265,
                DeathCoilEnergize   = 196263,
                SuddenDoom          = 81340
            };

            bool m_IsDeathCoilFree = false;

            void HandleBeforeCast()
            {
                m_IsDeathCoilFree = GetCaster()->HasAura(eSpells::SuddenDoom);
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Caster->IsFriendlyTo(l_Target))
                {
                    /// Glyph of Death Coil
                    if (l_Caster->HasAura(eSpells::GlyphofDeathCoil) && l_Target->GetCreatureType() != CREATURE_TYPE_UNDEAD && !l_Target->HasAura(eSpells::Lichborne))
                        l_Caster->CastSpell(l_Target, eSpells::DeathBarrier, true); ///< Death Barrier
                    else
                    {
                        int32 l_Healing = int32(l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 0.80f * 5.0f);
                        l_Caster->CastCustomSpell(l_Target, eSpells::DeathCoilAlly, &l_Healing, NULL, NULL, true);
                    }
                }
                else
                {
                    l_Caster->CastSpell(l_Target, eSpells::DeathCoilEnemy, m_IsDeathCoilFree);
                    if (m_IsDeathCoilFree)
                    {
                        if (SpellInfo const* l_SpellInfo = GetSpellInfo())
                        {
                            if (SpellPowerEntry const* l_PowerEntry = l_SpellInfo->GetSpellPower(POWER_RUNIC_POWER))
                            {
                                int32 l_PowerCost = l_PowerEntry->Cost;
                                DarkArbiterEmpowerment l_DarkArbiterEmpower;
                                l_DarkArbiterEmpower.Empower(l_Caster, l_PowerCost);
                            }
                        }
                    }

                    if (l_Caster->HasAura(eSpells::Necrosis))
                        l_Caster->CastSpell(l_Target, eSpells::NecrosisAura, true);
                }

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (l_Player->GetActiveSpecializationID() == SPEC_DK_UNHOLY)
                        l_Player->CastSpell(l_Player, eSpells::DeathCoilEnergize, true);
                }
            }

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(DK_SPELL_ENHANCED_DEATH_COIL))
                        l_Caster->CastSpell(l_Caster, DK_SPELL_SHADOW_OF_DEATH, true);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeathMarch);
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::DeathMarch))
                {
                    l_Player->ReduceSpellCooldown(eSpells::Defile, l_SpellInfo->Effects[EFFECT_0].BasePoints / 10 * IN_MILLISECONDS);
                    l_Player->ReduceSpellCooldown(eSpells::DeathAndDecay, l_SpellInfo->Effects[EFFECT_0].BasePoints / 10 * IN_MILLISECONDS);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_coil_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnHit += SpellHitFn(spell_dk_death_coil_SpellScript::HandleOnHit);
                AfterCast += SpellCastFn(spell_dk_death_coil_SpellScript::HandleAfterCast);
                BeforeCast += SpellCastFn(spell_dk_death_coil_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_death_coil_SpellScript();
        }
};

/// Last update : 7.3.5 build 26365
/// Death Coil - 47632
class spell_dk_death_coil_damage : public SpellScriptLoader
{
    public:
        spell_dk_death_coil_damage() : SpellScriptLoader("spell_dk_death_coil_damage") { }

        class spell_dk_death_coil_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_coil_damage_SpellScript);

            enum eSpells
            {
                T19Unholy4P         = 211050,
                FesteringWound      = 194310,
                DeathDebuff         = 191730,
                MasterOfGhouls      = 246995,
                T21Unholy2P         = 251871,
                CoilsOfDevastation  = 253367,
                DeathCoil           = 47541,
            };

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        int32 l_Damage = int32(l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 2.226f);

                        l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);
                        l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), (uint32)l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

                        /// Item - Death Knight T19 Unholy 4P Bonus
                        if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Unholy4P, EFFECT_0))
                            if (roll_chance_i(l_AuraEffect->GetAmount()))
                                l_Caster->CastSpell(l_Target, eSpells::FesteringWound, true);

                        if (Unit* l_Owner = l_Caster->GetOwner())
                        {
                            if (Aura* l_DeathDebuff = l_Target->GetAura(eSpells::DeathDebuff, l_Owner->GetGUID()))
                                if (SpellInfo const* l_DebuffInfo = sSpellMgr->GetSpellInfo(eSpells::DeathDebuff))
                                    AddPct(l_Damage, l_DebuffInfo->Effects[EFFECT_0].BasePoints * l_DeathDebuff->GetStackAmount());

                            if (l_Owner->HasAura(eSpells::MasterOfGhouls))
                                if (SpellInfo const* l_BuffInfo = sSpellMgr->GetSpellInfo(eSpells::MasterOfGhouls))
                                    AddPct(l_Damage, l_BuffInfo->Effects[EFFECT_0].BasePoints);
                        }

                        SetHitDamage(l_Damage);

                        if (l_Caster->HasAura(eSpells::T21Unholy2P) && l_Caster->IsPlayer() && !l_Caster->ToPlayer()->IsInBattleground())
                        {
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Unholy2P))
                            {
                                if (SpellInfo const* l_TriggerSpellInfo = sSpellMgr->GetSpellInfo(eSpells::CoilsOfDevastation))
                                {
                                    int32 l_Amount = CalculatePct(l_Damage / (l_TriggerSpellInfo->GetMaxDuration() / l_TriggerSpellInfo->Effects[EFFECT_0].Amplitude), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                                    if (GetSpell()->IsCritForTarget(l_Target))
                                        l_Amount *= 2;

                                    l_Caster->CastCustomSpell(l_Target, eSpells::CoilsOfDevastation, &l_Amount, nullptr, nullptr, true);
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_dk_death_coil_damage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_death_coil_damage_SpellScript();
        }
};

enum SkeletonSpells
{
    SpellSkeletonForm = 147157
};

enum GeistSpells
{
    SpellGeistForm = 121916
};

/// Called on removing Glyph of the Geist - 58640
class spell_dk_glyph_of_the_geist : public SpellScriptLoader
{
    public:
        spell_dk_glyph_of_the_geist() : SpellScriptLoader("spell_dk_glyph_of_the_geist") { }

        class spell_dk_glyph_of_the_geist_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_glyph_of_the_geist_AuraScript);

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                    {
                        if (!l_Pet->HasAura(SkeletonSpells::SpellSkeletonForm))
                            l_Pet->CastSpell(l_Pet, GeistSpells::SpellGeistForm, true);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                    {
                        if (l_Pet->HasAura(GeistSpells::SpellGeistForm))
                            l_Pet->RemoveAura(GeistSpells::SpellGeistForm);
                    }
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_glyph_of_the_geist_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_glyph_of_the_geist_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_glyph_of_the_geist_AuraScript();
        }
};

/// Called on removing Glyph of the Skeleton - 146652
class spell_dk_glyph_of_the_skeleton : public SpellScriptLoader
{
    public:
        spell_dk_glyph_of_the_skeleton() : SpellScriptLoader("spell_dk_glyph_of_the_skeleton") { }

        class spell_dk_glyph_of_the_skeleton_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_glyph_of_the_skeleton_AuraScript);

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                    {
                        if (!l_Pet->HasAura(GeistSpells::SpellGeistForm))
                            l_Pet->CastSpell(l_Pet, SkeletonSpells::SpellSkeletonForm, true);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Player* l_Player = GetTarget()->ToPlayer())
                {
                    if (Pet* l_Pet = l_Player->GetPet())
                    {
                        if (l_Pet->HasAura(SkeletonSpells::SpellSkeletonForm))
                            l_Pet->RemoveAura(SkeletonSpells::SpellSkeletonForm);
                    }
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_glyph_of_the_skeleton_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_dk_glyph_of_the_skeleton_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_glyph_of_the_skeleton_AuraScript();
        }
};

/// Army Transform - 127517
class spell_dk_army_transform : public SpellScriptLoader
{
    public:
        spell_dk_army_transform() : SpellScriptLoader("spell_dk_army_transform") { }

        class spell_dk_army_transform_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_army_transform_SpellScript);

            enum eArmyTransformSpells
            {
                GlyphOfFullMenagerie        = 58642,
                TransformFleahBeast         = 127533,
                TransformGeist              = 127534,
                TransformNothrendSkeleton   = 127528,
                TransformSkeleton           = 127527,
                TransformSpikedGhoul        = 127525,
                TransformSuperZombie        = 127526
            };

            bool Load() override
            {
                return GetCaster()->isGuardian();
            }

            SpellCastResult CheckCast()
            {
                Unit* l_Owner = GetCaster()->GetOwner();

                if (l_Owner == nullptr)
                    return SPELL_FAILED_SPELL_UNAVAILABLE;

                if (l_Owner->HasAura(eArmyTransformSpells::GlyphOfFullMenagerie))
                    return SPELL_CAST_OK;

                return SPELL_FAILED_SPELL_UNAVAILABLE;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                static uint32 const ArmyTransforms[6] = { TransformFleahBeast, TransformGeist, TransformNothrendSkeleton, TransformSkeleton, TransformSpikedGhoul, TransformSuperZombie };
                Unit *l_Caster = GetCaster();

                l_Caster->CastSpell(l_Caster, ArmyTransforms[urand(0, 5)], true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_army_transform_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_dk_army_transform_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_army_transform_SpellScript();
        }
};

/// Control Undead - 111673
class spell_dk_control_undead : public SpellScriptLoader
{
    public:
        spell_dk_control_undead() : SpellScriptLoader("spell_dk_control_undead") {}

        class spell_dk_control_undead_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_control_undead_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return SPELL_FAILED_SUCCESS;

                if (l_Target->IsPlayer())
                    return SPELL_FAILED_BAD_TARGETS;

                if (Unit* l_Owner = l_Target->GetOwner())
                {
                    if (l_Owner->IsPlayer())
                        return SPELL_FAILED_BAD_TARGETS;
                }

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature)
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Creature->getLevelForTarget(l_Caster) > (l_Caster->getLevel() + 1) || !l_Creature->GetCreatureTemplate() || l_Creature->GetCreatureTemplate()->rank > CreatureEliteType::CREATURE_ELITE_ELITE)
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void HandleBeforeCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target)
                    return;

                enum eSpells
                {
                    Bolstering = 209859
                };

                if (l_Target->HasAura(eSpells::Bolstering))
                    l_Target->RemoveAura(eSpells::Bolstering);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_control_undead_SpellScript::CheckCast);
                BeforeCast += SpellCastFn(spell_dk_control_undead_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_control_undead_SpellScript();
        }
};

/// DK Presences
/// Called by Blood Presence (48263), Unholy Presence (48265), Frost Presence (48266)
class spell_dk_presences : public SpellScriptLoader
{
    public:
        spell_dk_presences() : SpellScriptLoader("spell_dk_presences") { }

        class spell_dk_presences_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_presences_AuraScript);

            enum eSpells
            {
                ImprovedBloodPresence  = 50371,
                ImprovedFrostPresence  = 50385,
                ImprovedUnholyPresence = 50392
            };

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Player* l_Target = GetCaster()->ToPlayer();
                if (!l_Target)
                    return;

                if (l_Target->GetActiveSpecializationID() == SPEC_DK_BLOOD && !l_Target->HasAura(ImprovedBloodPresence))
                    l_Target->CastSpell(l_Target, ImprovedBloodPresence, true, nullptr, p_AurEff);
                if (l_Target->GetActiveSpecializationID() == SPEC_DK_UNHOLY && !l_Target->HasAura(ImprovedUnholyPresence))
                    l_Target->CastSpell(l_Target, ImprovedUnholyPresence, true, nullptr, p_AurEff);
                if (l_Target->GetActiveSpecializationID() == SPEC_DK_FROST && !l_Target->HasAura(ImprovedFrostPresence))
                    l_Target->CastSpell(l_Target, ImprovedFrostPresence, true, nullptr, p_AurEff);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_presences_AuraScript::OnApply, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_presences_AuraScript();
        }
};

/// Breath of Sindragosa - 152279
class spell_dk_breath_of_sindragosa : public SpellScriptLoader
{
public:
    spell_dk_breath_of_sindragosa() : SpellScriptLoader("spell_dk_breath_of_sindragosa") { }

    class spell_dk_breath_of_sindragosa_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_breath_of_sindragosa_AuraScript);

        enum eSpells
        {
            DarkTransformation = 63560
        };

        void OnTick(AuraEffect const* p_AurEff)
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (l_Player == nullptr)
                return;

            if (l_Player->GetActiveSpecializationID() == SPEC_DK_UNHOLY)
            {
                /// Receive Dark Infusion for every 30 runic power
                if (p_AurEff->GetTickNumber() % 2 == 0)
                    if (Pet* l_Pet = l_Player->GetPet())
                        if (!l_Pet->HasAura(eSpells::DarkTransformation))
                            l_Player->CastSpell(l_Pet, DK_SPELL_DARK_INFUSION_STACKS, true);
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_breath_of_sindragosa_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_breath_of_sindragosa_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called for Breath of Sindragosa - 155166
class spell_dk_breath_of_sindragosa_damage : public SpellScriptLoader
{
    public:
        spell_dk_breath_of_sindragosa_damage() : SpellScriptLoader("spell_dk_breath_of_sindragosa_damage") { }

        class spell_dk_breath_of_sindragosa_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_breath_of_sindragosa_damage_SpellScript);

            bool m_FirstTarget = true;

            void HandleOnHitTarget(SpellEffIndex /*p_effIndex*/)
            {
                if (!m_FirstTarget)
                    SetHitDamage(GetHitDamage() / 2);
                else
                    m_FirstTarget = false;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_breath_of_sindragosa_damage_SpellScript::HandleOnHitTarget, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_breath_of_sindragosa_damage_SpellScript();
        }
};

/// Called by Scent of Blood - 50421
/// Item - Death Knight T17 Blood 2P Bonus - 167192
class spell_dk_item_t17_blood_2p_bonus : public SpellScriptLoader
{
    public:
        spell_dk_item_t17_blood_2p_bonus() : SpellScriptLoader("spell_dk_item_t17_blood_2p_bonus") { }

        class spell_dk_item_t17_blood_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_item_t17_blood_2p_bonus_AuraScript);

            enum eSpells
            {
                T17Blood2P      = 167192,
                VampiricBlood   = 55233
            };

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// Each stack of Scent of Blood consumed reduces the cooldown of Vampiric Blood by 5 sec.
                    if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::T17Blood2P, EFFECT_0))
                    {
                        uint8 l_Stacks = p_AurEff->GetBase()->GetStackAmount();

                        if (l_Caster->GetTypeId() == TypeID::TYPEID_PLAYER)
                            l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::VampiricBlood, l_Stacks * l_AurEff->GetAmount());
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_dk_item_t17_blood_2p_bonus_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_item_t17_blood_2p_bonus_AuraScript();
        }
};

/// Blood Shield - 77535
class spell_dk_blood_shield : public SpellScriptLoader
{
    public:
        spell_dk_blood_shield() : SpellScriptLoader("spell_dk_blood_shield") { }

        class spell_dk_blood_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_blood_shield_AuraScript);

            enum eSpells
            {
                T17Blood4P = 165571
            };

            void AfterAbsorb(AuraEffect* p_AurEff, DamageInfo& /*p_DmgInfo*/, uint32& p_AbsorbAmount)
            {
                if (Unit* l_Target = GetTarget())
                {
                    /// While Vampiric Blood is active, your Blood Shield cannot be reduced below 3% of your maximum health.
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpells::T17Blood4P, EFFECT_0))
                    {
                        int32 l_FutureAbsorb = p_AurEff->GetAmount() - p_AbsorbAmount;
                        int32 l_MinimaAbsorb = l_Target->CountPctFromMaxHealth(l_AurEff->GetAmount());

                        /// We need to add some absorb amount to correct the absorb amount after that, and set it to 3% of max health
                        if (l_FutureAbsorb < l_MinimaAbsorb)
                        {
                            int32 l_AddedAbsorb = l_MinimaAbsorb - l_FutureAbsorb;
                            p_AurEff->ChangeAmount(p_AurEff->GetAmount() + l_AddedAbsorb);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dk_blood_shield_AuraScript::AfterAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_blood_shield_AuraScript();
        }
};

/// 7.1.5 Build 23420
/// Called by Nighthold T19 Blood 4P Bonus 211041
class spell_dk_t19_blood_4p_proc : public SpellScriptLoader
{
    public:
        spell_dk_t19_blood_4p_proc() : SpellScriptLoader("spell_dk_t19_blood_4p_proc") { }

        class spell_dk_t19_blood_4p_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_t19_blood_4p_proc_AuraScript);

            enum eSpells
            {
                DeathStrike = 49998
            };

            bool OnCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::DeathStrike)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_t19_blood_4p_proc_AuraScript::OnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_t19_blood_4p_proc_AuraScript();
        }
};

/// Item - Death Knight T17 Frost 4P Driver - 167655
class spell_dk_item_t17_frost_4p_driver : public SpellScriptLoader
{
    public:
        spell_dk_item_t17_frost_4p_driver() : SpellScriptLoader("spell_dk_item_t17_frost_4p_driver") { }

        class spell_dk_item_t17_frost_4p_driver_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_item_t17_frost_4p_driver_AuraScript);

            enum eSpells
            {
                FrozenRuneblade = 170202
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (l_ProcSpell == nullptr)
                    return;

                Unit* l_Target = p_EventInfo.GetActionTarget();
                if (l_Target == nullptr || l_Target == l_Caster)
                    return;

                /// While Pillar of Frost is active, your special attacks trap a soul in your rune weapon.
                l_Caster->CastSpell(l_Target, eSpells::FrozenRuneblade, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_item_t17_frost_4p_driver_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_item_t17_frost_4p_driver_AuraScript();
        }
};

/// Item - Death Knight T17 Frost 4P Driver (Periodic) - 170205
class spell_dk_item_t17_frost_4p_driver_periodic : public SpellScriptLoader
{
    public:
        spell_dk_item_t17_frost_4p_driver_periodic() : SpellScriptLoader("spell_dk_item_t17_frost_4p_driver_periodic") { }

        class spell_dk_item_t17_frost_4p_driver_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_item_t17_frost_4p_driver_periodic_AuraScript);

            enum eSpells
            {
                FrozenRunebladeMainHand = 165569,
                FrozenRunebladeOffHand  = 178808,
                FrozenRunebladeStacks   = 170202
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Unit* l_Target = l_Caster->getVictim();
                if (l_Target == nullptr)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (Aura* l_Aura = l_Player->GetAura(eSpells::FrozenRunebladeStacks))
                    {
                        if (Item* l_MainHand = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND))
                            l_Player->CastSpell(l_Target, eSpells::FrozenRunebladeMainHand, true);

                        if (Item* l_OffHand = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_OFFHAND))
                            l_Player->CastSpell(l_Target, eSpells::FrozenRunebladeOffHand, true);

                        l_Aura->DropStack();
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_item_t17_frost_4p_driver_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_item_t17_frost_4p_driver_periodic_AuraScript();
        }
};

/// Defile - 152280
/// Updated as of 7.0.3 - 22522
class spell_dk_defile : public SpellScriptLoader
{
    public:
        spell_dk_defile() : SpellScriptLoader("spell_dk_defile") { }

        class spell_dk_defile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_defile_SpellScript);

            enum eSpells
            {
                SpawnDefileVisual   = 169018
            };

            void HandleSpawn(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Loc = GetExplTargetDest();
                if (!l_Caster || !l_Loc)
                    return;

                l_Caster->CastSpell(l_Loc, eSpells::SpawnDefileVisual, true);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_dk_defile_SpellScript::HandleSpawn, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_defile_SpellScript;
        }
};

/// last update : 6.2.3
/// Periodic Taunt - 43264
class spell_dk_army_of_the_death_taunt : public SpellScriptLoader
{
    public:
        spell_dk_army_of_the_death_taunt() : SpellScriptLoader("spell_dk_army_of_the_death_taunt") { }

        class spell_dk_army_of_the_death_taunt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_army_of_the_death_taunt_AuraScript);

            enum eSpells
            {
                GlyphofArmyoftheDead = 58669
            };

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Owner = l_Target->GetOwner();

                if (l_Owner == nullptr)
                    return;

                if (l_Owner->HasAura(eSpells::GlyphofArmyoftheDead))
                    l_Target->RemoveAura(GetSpellInfo()->Id);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_army_of_the_death_taunt_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_army_of_the_death_taunt_AuraScript();
        }
};

/// Last Update 7.0.3
/// Icy talons - 194878
/// Call By Frost Strike : 49143
class spell_dk_icy_talons: public SpellScriptLoader
{
    public:
        spell_dk_icy_talons() : SpellScriptLoader("spell_dk_icy_talons") { }

        class spell_dk_icy_talons_SpellScript : public SpellScript
        {
            enum eSpells
            {
                IcyTalons     = 194878,
                IcyTalonsBuff = 194879
            };

            PrepareSpellScript(spell_dk_icy_talons_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasSpell(eSpells::IcyTalons))
                        l_Caster->CastSpell(l_Caster, eSpells::IcyTalonsBuff, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dk_icy_talons_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_icy_talons_SpellScript();
        }
};

struct DamageHistoryEntry
{
    DamageHistoryEntry(uint32 p_Damage, time_t p_Timestamp)
    {
        Damage    = p_Damage;
        Timestamp = p_Timestamp;
    }

    uint32           Damage;
    uint32           Timestamp;
};

typedef std::deque<DamageHistoryEntry>                              DamagesHistory;

typedef ACE_Based::LockedMap<uint64/*PlayerGUID*/, DamagesHistory>  DamagesHistoryMap;

class PlayerScript_dk_death_strike : public PlayerScript
{
    private:
        static int const k_CycleInSecs;
        static DamagesHistoryMap m_HistoryDamagesPlayers;
        uint32 m_Timelapse = 0;

        enum eSpells
        {
            DeathStrike = 49998
        };

    public:
        PlayerScript_dk_death_strike() : PlayerScript("PlayerScript_Death_Strike")
        {
            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeathStrike))
                m_Timelapse = l_SpellInfo->Effects[EFFECT_3].BasePoints;
        }

        void OnLogin(Player* p_Player) override
        {
            m_HistoryDamagesPlayers.erase(p_Player->GetGUID());
        }

        void OnTakeDamage(Player* p_Player, Unit*, DamageEffectType p_DamageEffectType, uint32 p_Damage, SpellSchoolMask p_SchoolMask, CleanDamage const* p_CleanDamage) override
        {
            if (p_Player->getClass() != CLASS_DEATH_KNIGHT)
                return;

            if (p_DamageEffectType == DamageEffectType::SELF_DAMAGE
                || p_DamageEffectType == DamageEffectType::HEAL)
                return;

            time_t l_time = time(NULL);
            DamageHistoryEntry l_Entry = { p_Damage, l_time };

            if (m_HistoryDamagesPlayers.find(p_Player->GetGUID()) != m_HistoryDamagesPlayers.end())
                m_HistoryDamagesPlayers[p_Player->GetGUID()].push_back(l_Entry);
            else
            {
                DamagesHistory l_History;

                l_History.push_back(l_Entry);
                m_HistoryDamagesPlayers[p_Player->GetGUID()] = l_History;
            }
        }

        void OnUpdate(Player* p_Player, uint32 p_Diff) override
        {
            if (p_Player->getClass() != CLASS_DEATH_KNIGHT)
                return;

            if (m_HistoryDamagesPlayers.find(p_Player->GetGUID()) == m_HistoryDamagesPlayers.end())
                return;

            DamagesHistory& l_History = m_HistoryDamagesPlayers[p_Player->GetGUID()];
            DamagesHistory l_CleanHistory;

            for (DamagesHistory::iterator l_Itr = l_History.begin(); l_Itr != l_History.end(); l_Itr++)
            {
                if ((*l_Itr).Timestamp + m_Timelapse > time(NULL))
                    l_CleanHistory.push_back(*l_Itr);
            }

            l_History = l_CleanHistory;
        }

        static uint32 GetDamagesTakenInLastPeriod(uint64 p_PlayerGUID)
        {
            DamagesHistory& l_DamagesHistory = m_HistoryDamagesPlayers[p_PlayerGUID];
            uint32 l_Damages = 0;

            for (DamageHistoryEntry l_Entry : l_DamagesHistory)
                l_Damages += l_Entry.Damage;

            return l_Damages;
        }
};

void SouldrinkerHeal(Player* p_Player, int32 p_Heal)
{
    enum eSpells
    {
        Souldrinker         = 238114,
        SouldrinkerBuff     = 240558
    };

    if (!p_Player->ToUnit())
        return;

    if (p_Player->HasAura(eSpells::Souldrinker))
    {
        int32 l_OverHeal = (p_Player->GetHealth() + p_Heal) - p_Player->GetMaxHealth();
        if (l_OverHeal > 0)
        {
            if (SpellInfo const* l_SDSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Souldrinker))
            {
                int32 l_SouldrinkerHeal = CalculatePct(l_OverHeal, l_SDSpellInfo->Effects[EFFECT_0].BasePoints);
                int32 l_SouldrinkerHealPct = l_SouldrinkerHeal * 100 / p_Player->GetMaxHealth();

                if (p_Player->ToUnit()->m_SpellHelper.GetUint32(eSpellHelpers::SouldrinkerHealthPct) > 0)
                    l_SouldrinkerHealPct += p_Player->ToUnit()->m_SpellHelper.GetUint32(eSpellHelpers::SouldrinkerHealthPct);

                p_Player->ToUnit()->m_SpellHelper.GetUint32(eSpellHelpers::SouldrinkerHealthPct) = l_SouldrinkerHealPct;

                if (l_SouldrinkerHealPct > l_SDSpellInfo->Effects[EFFECT_1].BasePoints)
                    l_SouldrinkerHealPct = l_SDSpellInfo->Effects[EFFECT_1].BasePoints;

                p_Player->CastCustomSpell(p_Player, eSpells::SouldrinkerBuff, &l_SouldrinkerHealPct, nullptr, nullptr, true);
            }
        }
    }
}

DamagesHistoryMap PlayerScript_dk_death_strike::m_HistoryDamagesPlayers;

/// last update : 7.3.2 Build 25549
/// Death Strike - 49998
class spell_dk_death_strike: public SpellScriptLoader
{
    public:
        spell_dk_death_strike() : SpellScriptLoader("spell_dk_death_strike") { }

        class spell_dk_death_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_strike_SpellScript);

            enum eSpells
            {
                DeathStrike         = 49998,
                DeathStrikeHeal     = 45470,
                Defile              = 152280,
                DeathMarch          = 235556,
                ShacklesOfBryndaor  = 209228,
                Haemostasis         = 235559,
                DeathAndDecay       = 43265,
                HeartOfIce          = 246426,
                IceboundFortitude   = 48792,
                DeathStrikeOff      = 66188,
                DarkSuccor          = 101568
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeathMarch);
                if (!l_Caster || !l_SpellInfo)
                    return;

                /// The spell can be casted by Dancing Rune Weapon
                if (Player* l_Player = l_Caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    SpellInfo const* l_DeathStrikeSpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeathStrike);
                    SpellInfo const* l_DeathStrikeHealSpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeathStrikeHeal);
                    SpellInfo const* l_ShacklesOfBryndaorSpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShacklesOfBryndaor);

                    if (l_DeathStrikeSpellInfo && l_ShacklesOfBryndaorSpellInfo && l_DeathStrikeHealSpellInfo)
                    {
                        uint32 l_DamagesTaken = PlayerScript_dk_death_strike::GetDamagesTakenInLastPeriod(l_Player->GetGUID());
                        uint32 l_Damages = CalculatePct(l_DamagesTaken, l_DeathStrikeSpellInfo->Effects[EFFECT_2].BasePoints);

                        if (l_Player->CanApplyPvPSpellModifiers())
                            l_Damages *= 0.50f;

                        uint32 l_PlayerHealth = CalculatePct(l_Player->GetMaxHealth(), l_DeathStrikeSpellInfo->Effects[EFFECT_4].BasePoints);
                        int32 l_FinalHealth = std::max(l_Damages, l_PlayerHealth);

                        SouldrinkerHeal(l_Player, l_FinalHealth);

                        l_Player->CastCustomSpell(GetCaster(), eSpells::DeathStrikeHeal, &l_FinalHealth, nullptr, nullptr, true);

                        if (l_Player->HasAura(eSpells::ShacklesOfBryndaor))
                        {
                            int32 l_TotalHealth = l_Player->SpellHealingBonusDone(l_Player, l_DeathStrikeHealSpellInfo, l_FinalHealth, SpellEffIndex::EFFECT_0, DamageEffectType::HEAL);
                            l_TotalHealth = l_Player->SpellHealingBonusTaken(l_Player, l_DeathStrikeHealSpellInfo, l_TotalHealth, DamageEffectType::HEAL);
                            float l_FinalHealthPercent = (float)l_TotalHealth * 100 / l_Player->GetMaxHealth();

                            if (l_FinalHealthPercent > (float)l_ShacklesOfBryndaorSpellInfo->Effects[EFFECT_1].BasePoints)
                            {
                                int32 l_PowerCost = GetSpell()->GetPowerCost(Powers::POWER_RUNIC_POWER);
                                l_Player->ModifyPower(POWER_RUNIC_POWER, CalculatePct(l_PowerCost, l_ShacklesOfBryndaorSpellInfo->Effects[EFFECT_0].BasePoints));
                            }
                        }

                        if (l_Caster->HasAura(eSpells::HeartOfIce))
                        {
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HeartOfIce))
                            {
                                if (Aura* l_Aura = l_Caster->GetAura(eSpells::IceboundFortitude))
                                    l_Aura->SetDuration(l_Aura->GetDuration() + ((l_SpellInfo->Effects[EFFECT_0].BasePoints / 10) * IN_MILLISECONDS));
                            }
                        }
                    }
                }

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::DeathMarch))
                {
                    l_Player->ReduceSpellCooldown(eSpells::Defile, l_SpellInfo->Effects[EFFECT_0].BasePoints / 10 * IN_MILLISECONDS);
                    l_Player->ReduceSpellCooldown(eSpells::DeathAndDecay, l_SpellInfo->Effects[EFFECT_0].BasePoints / 10 * IN_MILLISECONDS);
                }

                if (l_Caster->HasAura(eSpells::Haemostasis))
                    l_Caster->RemoveAura(eSpells::Haemostasis);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// The spell can be casted by Dancing Rune Weapon
                Player* l_Player = l_Caster->GetCharmerOrOwnerPlayerOrPlayerItself();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_DK_FROST)
                    if (Unit* l_Target = GetHitUnit())
                        l_Caster->CastSpell(l_Target, eSpells::DeathStrikeOff, true);
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::DarkSuccor) = false;
                if (AuraEffect* l_AuraEff = l_Caster->GetAuraEffect(eSpells::DarkSuccor, EFFECT_0))
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::DarkSuccor) = true;
            }

            void Register()
            {
                OnCast += SpellCastFn(spell_dk_death_strike_SpellScript::HandleOnCast);
                AfterCast += SpellCastFn(spell_dk_death_strike_SpellScript::HandleAfterCast);
                OnHit += SpellHitFn(spell_dk_death_strike_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_strike_SpellScript();
        }
};

/// last update : 7.0.3
/// Avalanche - 207142
class spell_dk_avalanche_aura : public SpellScriptLoader
{
    public:
        spell_dk_avalanche_aura() : SpellScriptLoader("spell_dk_avalanche_aura") { }

        class spell_dk_avalanche_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_avalanche_aura_AuraScript);

            enum eSpell
            {
                PillarOfFrost = 51271,
                Avalanche     = 207150
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (l_Caster == nullptr || l_Target == nullptr || !l_SpellInfo)
                    return;

                std::chrono::milliseconds l_Timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch());

                if (l_Timestamp.count() - l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::Avalanche) < 100)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::Avalanche) = l_Timestamp.count();

                if (l_Caster->HasAura(eSpell::PillarOfFrost))
                {
                    if (p_EventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
                        l_Caster->CastSpell(l_Target, eSpell::Avalanche, true);
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_avalanche_aura_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_avalanche_aura_AuraScript();
        }
};

/// Last Update 7.0.3
/// Bloodworms - 195679
class spell_dk_bloodworms : public SpellScriptLoader
{
    public:
        spell_dk_bloodworms() : SpellScriptLoader("spell_dk_bloodworms") { }

        class spell_dk_bloodworms_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_bloodworms_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (!p_EventInfo.GetDamageInfo())
                    return;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo())
                    return;

                if (l_Caster == nullptr)
                    return;

                if (!(p_EventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                    return;

                l_Caster->CastSpell(l_Caster, p_AurEff->GetTriggerSpell(), true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_bloodworms_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_bloodworms_AuraScript();
        }
};

/// Last Update 7.0.3
/// Heart Strike - 206930
class spell_dk_heart_strike : public SpellScriptLoader
{
    public:
        spell_dk_heart_strike() : SpellScriptLoader("spell_dk_heart_strike") { }

        class spell_dk_heart_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_heart_strike_SpellScript);

            enum eSpells
            {
                Heartbreaker            = 221536,
                HeartbreakerEnergize    = 210738,
                BloodForBlood           = 202846,
                ServiceOfGorefiend      = 208706,
                VampiricBlood           = 55233,
                DancingRuneWeapon       = 49028,
                MouthOfHell             = 192570
            };

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster && l_Caster->HasAura(eSpells::Heartbreaker))
                    l_Caster->CastSpell(l_Caster, eSpells::HeartbreakerEnergize, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ServiceOfGorefiend);

                if (!l_Caster || !l_Caster->HasAura(eSpells::ServiceOfGorefiend) || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                SpellInfo const* l_VampiricBloodInfo = sSpellMgr->GetSpellInfo(eSpells::VampiricBlood);
                if (!l_VampiricBloodInfo)
                    return;

                int32 l_ReductionTime = l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS;

                uint8 l_Multiplier = 1;
                if (l_Player->HasAura(eSpells::DancingRuneWeapon))
                {
                    l_Multiplier++;
                    if (l_Player->HasAura(eSpells::MouthOfHell))
                        l_Multiplier++;
                }

                if (SpellCategoryEntry const* l_Entry = l_VampiricBloodInfo->CategoryEntry)
                    l_Player->ReduceChargeCooldown(l_Entry, l_ReductionTime * l_Multiplier, false);

                l_Player->ReduceSpellCooldown(eSpells::VampiricBlood, l_ReductionTime * l_Multiplier);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_heart_strike_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                AfterCast += SpellCastFn(spell_dk_heart_strike_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_heart_strike_SpellScript();
        }
};

/// Last update 7.0.3 Build 22293
/// Soulgorge - 212744
class spell_dk_soulgorge : public SpellScriptLoader
{
    public:
        spell_dk_soulgorge() : SpellScriptLoader("spell_dk_soulgorge") { }

        class spell_dk_soulgorge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_soulgorge_SpellScript);

            enum eSpells
            {
                BloodPlague = 55078,
                SoulgorgeAura = 213003
            };

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (!p_Object->ToUnit()->HasAura(eSpells::BloodPlague, l_Caster->GetGUID()))
                        return true;

                    return false;
                });
            }

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                Aura* l_Aura = l_Target->GetAura(eSpells::BloodPlague, l_Caster->GetGUID());
                if (l_Aura == nullptr)
                    return;

                uint32 l_Pct = l_Aura->GetDuration() / (l_Aura->GetMaxDuration() / 100.0f);
                int32 l_Bp = CalculatePct(GetSpellInfo()->Effects[EFFECT_1].BasePoints, l_Pct);
                l_Target->RemoveAura(eSpells::BloodPlague, l_Caster->GetGUID());
                l_Caster->CastSpell(l_Caster, eSpells::SoulgorgeAura);

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::SoulgorgeAura, EFFECT_0);
                if (l_AuraEffect == nullptr)
                    return;

                l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() + l_Bp);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_soulgorge_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_dk_soulgorge_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_soulgorge_SpellScript();
        }
};

/// Last update 7.3.5 Build 26365
/// Bone Shield - 195181
class spell_dk_bone_shield : public SpellScriptLoader
{
    public:
        spell_dk_bone_shield() : SpellScriptLoader("spell_dk_bone_shield") { }

        class spell_dk_bone_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_bone_shield_AuraScript);

            enum eSpells
            {
                BloodTap                    = 221699,
                OssuaryAura                 = 219786,
                OssuaryBuff                 = 219788,
                SpectralDeflection          = 211078,
                SkeletalShattering          = 192558,
                RattlegoreBoneLegplatesAura = 205816,
                RattlegoreBoneLegplatesBuff = 216140,
                T21Blood2PBonus             = 251876,
                DanceRuneWeapon             = 49028

            };

            time_t m_Time = 0;

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* /*p_AurEff*/, DamageInfo & p_DmgInfo, uint32 & p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_Player = l_Target->ToPlayer();
                if (!l_Player)
                    return;

                bool l_CanDamageConsumeStack = p_DmgInfo.GetSchoolMask() == SpellSchoolMask::SPELL_SCHOOL_MASK_NORMAL && p_DmgInfo.GetDamageType() != DamageEffectType::DOT;

                int32 l_AbsorbPerc = GetSpellInfo()->Effects[EFFECT_4].CalcValue(l_Target);
                int32 l_AbsorbStack = 1;
                if (AuraEffect* l_SpectralDeflectionAuraEffect = l_Target->GetAuraEffect(eSpells::SpectralDeflection, EFFECT_0))
                {
                    if (l_Target->CountPctFromMaxHealth(l_SpectralDeflectionAuraEffect->GetAmount()) < p_DmgInfo.GetAmount())
                    {
                        l_AbsorbPerc *= 2;
                        l_AbsorbStack *= 2;
                        if (l_CanDamageConsumeStack)
                        {
                            ModStackAmount(-1);
                            if (l_Target->HasAura(eSpells::T21Blood2PBonus))
                                if (SpellInfo const* l_T21SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Blood2PBonus))
                                    l_Player->ReduceSpellCooldown(eSpells::DanceRuneWeapon, std::abs(l_T21SpellInfo->Effects[EFFECT_0].BasePoints));
                        }
                    }
                }

                AuraEffect* l_SkeletalShatteringAuraEffect = l_Target->GetAuraEffect(eSpells::SkeletalShattering, EFFECT_0);
                if (l_SkeletalShatteringAuraEffect && roll_chance_f(l_Player->GetFloatValue(PLAYER_FIELD_CRIT_PERCENTAGE)))
                    l_AbsorbPerc += l_SkeletalShatteringAuraEffect->GetAmount();

                p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_AbsorbPerc);

                /// Can loose one stack of bone shield every 2 second.
                if (l_CanDamageConsumeStack && (m_Time == 0 || m_Time + 2 <= time(nullptr)))
                {
                    m_Time = time(nullptr);
                    ModStackAmount(-1);
                    if (l_Target->HasAura(eSpells::T21Blood2PBonus))
                        if (SpellInfo const* l_T21SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Blood2PBonus))
                            l_Player->ReduceSpellCooldown(eSpells::DanceRuneWeapon, std::abs(l_T21SpellInfo->Effects[EFFECT_0].BasePoints));
                }

                if (l_Player && l_Player->HasSpell(eSpells::BloodTap))
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BloodTap);
                    if (l_SpellInfo)
                        l_Player->ReduceChargeCooldown(l_SpellInfo->ChargeCategoryEntry, IN_MILLISECONDS * l_SpellInfo->Effects[EFFECT_1].CalcValue(l_Target) * l_AbsorbStack);
                }
            }

            void AfterStackApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                HandleStackChange();
            }

            void AfterStackRemove(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                HandleStackChange();

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (l_Target->HasAura(eSpells::RattlegoreBoneLegplatesBuff))
                    l_Target->RemoveAurasDueToSpell(eSpells::RattlegoreBoneLegplatesBuff);
            }

            void HandleStackChange()
            {
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::OssuaryAura);
                if (!l_Target || !l_SpellInfo)
                    return;

                if (GetStackAmount() >= l_SpellInfo->Effects[EFFECT_0].BasePoints && l_Target->HasAura(m_scriptSpellId))
                {
                    if (l_Target->HasAura(eSpells::OssuaryAura))
                        l_Target->CastSpell(l_Target, eSpells::OssuaryBuff, true);
                }
                else
                    l_Target->RemoveAurasDueToSpell(eSpells::OssuaryBuff);

                if (l_Target->HasAura(eSpells::RattlegoreBoneLegplatesAura) && !l_Target->HasAura(eSpells::RattlegoreBoneLegplatesBuff))
                    l_Target->CastSpell(l_Target, eSpells::RattlegoreBoneLegplatesBuff, true);
            }

            bool CanRefreshProcDummy(bool p_Decrease)
            {
                return !p_Decrease;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_bone_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_bone_shield_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_dk_bone_shield_AuraScript::AfterStackApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_bone_shield_AuraScript::AfterStackRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                CanRefreshProc += AuraCanRefreshProcFn(spell_dk_bone_shield_AuraScript::CanRefreshProcDummy);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_bone_shield_AuraScript();
        }
};

/// Last Update 7.0.3
/// Mark of Blood - 206940
class spell_dk_mark_of_blood : public SpellScriptLoader
{
    public:
        spell_dk_mark_of_blood() : SpellScriptLoader("spell_dk_mark_of_blood") { }

        class spell_dk_mark_of_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_mark_of_blood_AuraScript);

            enum eSpells
            {
                MarkOfBloodHeal = 206945
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (l_DamageInfo == nullptr)
                    return;

                if (l_DamageInfo->GetSpellInfo() != nullptr) ///< Only auto attacks
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MarkOfBloodHeal, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_mark_of_blood_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_mark_of_blood_AuraScript();
        }
};

/// Last Update 7.0.3
/// Red Thirst - 205723
class PlayerScript_dk_red_thirst : public PlayerScript
{
    public:
        PlayerScript_dk_red_thirst() :PlayerScript("PlayerScript_red_thirst") {}

        int32 m_RunicPower = 0;

        enum eSpells
        {
            RedThirst       = 205723,
            VampiricBlood   = 55233
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After)
                return;

            if (p_Player->getClass() != CLASS_DEATH_KNIGHT || p_Power != POWER_RUNIC_POWER || !p_Player->HasSpell(eSpells::RedThirst) || p_Regen)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::RedThirst);
            if (l_SpellInfo == nullptr)
                return;

            int32 l_Old = p_OldValue;
            int32 l_New = p_NewValue;

            l_Old /= 10;
            l_New /= 10;

            /// Get the power earn (if > 0 ) or consum (if < 0)
            int32 l_diffValue = l_New - l_Old;

            if (l_diffValue > 0)
                return;

            m_RunicPower = std::abs(l_diffValue);

            uint8 l_TotalSec = 0;

            while (m_RunicPower >= 6)
            {
                m_RunicPower -= 6;
                l_TotalSec++;
            }

            SpellInfo const* l_Vampiric = sSpellMgr->GetSpellInfo(eSpells::VampiricBlood);
            if (!l_Vampiric)
                return;

            if (SpellCategoryEntry const* l_Entry = l_Vampiric->CategoryEntry)
                p_Player->ReduceChargeCooldown(l_Entry, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS * l_TotalSec, false);

            p_Player->ReduceSpellCooldown(eSpells::VampiricBlood, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS * l_TotalSec);
        }
};

/// Last Update 7.1.5 - 23420
/// Will of the Necropolis (Absorb) - 206967
class spell_dk_will_of_the_necropolis : public SpellScriptLoader
{
    public:
        spell_dk_will_of_the_necropolis() : SpellScriptLoader("spell_dk_will_of_the_necropolis") { }

        class spell_dk_will_of_the_necropolis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_will_of_the_necropolis_AuraScript);

            enum eSpells
            {
                FieryBrand = 204021,
                BurningAliveDot = 207771,
                FieryBrandMarker = 207744
            };

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Victim = p_DmgInfo.GetTarget();
                Unit* l_Attacker = p_DmgInfo.GetActor();

                if (l_Victim == nullptr || l_Attacker == nullptr)
                    return;

                if (l_Victim->GetHealthPct() < GetSpellInfo()->Effects[EFFECT_2].BasePoints)
                    p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), GetSpellInfo()->Effects[EFFECT_1].BasePoints);
                else
                    p_AbsorbAmount = 0;
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1; ///< Initialize to infinite Absorb
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_will_of_the_necropolis_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_will_of_the_necropolis_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_will_of_the_necropolis_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Tombstone - 219809
class spell_dk_tombstone : public SpellScriptLoader
{
    public:
        spell_dk_tombstone() : SpellScriptLoader("spell_dk_tombstone") { }

        class spell_dk_tombstone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_tombstone_AuraScript);

            enum eSpells
            {
                BoneShield          = 195181,
                T21Blood2PBonus     = 251876,
                DanceRuneWeapon     = 49028
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::BoneShield);
                if (!l_Aura)
                    return;

                uint8 l_MaxCharge = l_SpellInfo->Effects[EFFECT_4].BasePoints;
                uint8 l_ChargeNumber = l_Aura->GetStackAmount() > l_MaxCharge ? l_MaxCharge : l_Aura->GetStackAmount();
                p_Amount = CalculatePct(l_Caster->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_3].BasePoints * l_ChargeNumber);
            }

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_SpellInfo)
                    return;

                Aura* l_Aura = l_Target->GetAura(eSpells::BoneShield);
                if (!l_Aura)
                    return;

                uint8 l_MaxCharge = l_SpellInfo->Effects[EFFECT_4].BasePoints;
                uint8 l_ChargeNumber = l_Aura->GetStackAmount() > l_MaxCharge ? l_MaxCharge : l_Aura->GetStackAmount();
                l_Aura->ModStackAmount(-l_ChargeNumber);

                if (l_Target->HasAura(eSpells::T21Blood2PBonus))
                    if (SpellInfo const* l_T21SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Blood2PBonus))
                        if (Player* l_Player = l_Target->ToPlayer())
                            l_Player->ReduceSpellCooldown(eSpells::DanceRuneWeapon, std::abs(l_T21SpellInfo->Effects[EFFECT_0].BasePoints) * l_ChargeNumber);

                int32 l_Bp = l_ChargeNumber * l_SpellInfo->Effects[EFFECT_2].BasePoints;
                l_Target->EnergizeBySpell(l_Target, l_SpellInfo->Id, l_Bp * 10, POWER_RUNIC_POWER);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_tombstone_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_dk_tombstone_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_tombstone_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Bonestorm - 194844
class spell_dk_bonestorm : public SpellScriptLoader
{
    public:
        spell_dk_bonestorm() : SpellScriptLoader("spell_dk_bonestorm") { }

        class spell_dk_bonestorm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_bonestorm_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_Power = l_Caster->GetPower(POWER_RUNIC_POWER) / l_Caster->GetPowerCoeff(POWER_RUNIC_POWER);
                l_Power = std::min(l_Power, (uint32)90);
                Aura* l_Aura = l_Caster->GetAura(GetSpellInfo()->Id);
                if (!l_Aura)
                    return;

                l_Aura->SetDuration(l_Aura->GetDuration() + (IN_MILLISECONDS * (l_Power / 10)));
                l_Caster->SetPower(POWER_RUNIC_POWER, l_Caster->GetPower(POWER_RUNIC_POWER) - (l_Power / 10) * 10 * l_Caster->GetPowerCoeff(POWER_RUNIC_POWER));
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_dk_bonestorm_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_bonestorm_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Bonestorm - 196528
class spell_dk_bonestorm_damage : public SpellScriptLoader
{
    public:
        spell_dk_bonestorm_damage() : SpellScriptLoader("spell_dk_bonestorm_damage") { }

        class spell_dk_bonestorm_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_bonestorm_damage_SpellScript);

            enum eSpells
            {
                Bonestorm = 196545
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Bonestorm, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dk_bonestorm_damage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_bonestorm_damage_SpellScript();
        }
};

/// Last Update 7.0.3
/// Blood Mirror - 206977
class spell_dk_blood_mirror : public SpellScriptLoader
{
    public:
        spell_dk_blood_mirror() : SpellScriptLoader("spell_dk_blood_mirror") { }

        class spell_dk_blood_mirror_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_blood_mirror_AuraScript);

            enum eSpells
            {
                BloodMirror = 221847,
                RageoftheSleeperDamage = 219432,
            };

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;

                Unit* l_Victim = p_DmgInfo.GetTarget();
                Unit* l_Attacker = p_DmgInfo.GetActor();

                if (l_Victim == nullptr || l_Attacker == nullptr)
                    return;

                if (!l_Attacker->HasAura(GetSpellInfo()->Id, l_Victim->GetGUID()))
                    return;

                /// Prevent infinite loop
                SpellInfo const* l_ProcSpell = p_DmgInfo.GetSpellInfo();
                if (l_ProcSpell)
                {
                    switch (l_ProcSpell->Id)
                    {
                        case eSpells::RageoftheSleeperDamage:
                        case eSpells::BloodMirror:
                            return;
                        default:
                            break;
                    }
                }

                p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), GetSpellInfo()->Effects[EFFECT_1].BasePoints);
                int32 l_Bp = p_AbsorbAmount;
                l_Victim->CastCustomSpell(l_Attacker, eSpells::BloodMirror, &l_Bp, NULL, NULL, true);
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1; ///< Initialize to infinite Absorb
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_blood_mirror_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_blood_mirror_AuraScript::OnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_blood_mirror_AuraScript();
        }
};

/// Last Update 7.0.3
/// Permafrost - 61606
class spell_dk_permafrost : public SpellScriptLoader
{
    public:
        spell_dk_permafrost() : SpellScriptLoader("spell_dk_permafrost") { }

        class spell_dk_permafrost_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_permafrost_AuraScript);

            enum eSpells
            {
                FrostShieldAura = 207203
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (l_DamageInfo == nullptr)
                    return;

                if (l_DamageInfo->GetSpellInfo() != nullptr) ///< Only auto attacks
                    return;

                int32 l_Amount = CalculatePct(l_DamageInfo->GetAmount(), p_AurEff->GetAmount());
                AuraEffect* l_PreviousAuraEffect = l_Caster->GetAuraEffect(eSpells::FrostShieldAura, EFFECT_0);
                if (l_PreviousAuraEffect != nullptr)
                    l_Amount += l_PreviousAuraEffect->GetAmount();

                l_Amount = std::min(l_Amount, (int32)CalculatePct(l_Caster->GetMaxHealth(), 20)); ///< Maximum 20% of max Health
                l_Caster->CastSpell(l_Caster, eSpells::FrostShieldAura, true);

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::FrostShieldAura, EFFECT_0))
                    l_AuraEffect->SetAmount(l_Amount);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_permafrost_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_permafrost_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Winter Is Coming - 211794
class spell_dk_winter_is_coming : public SpellScriptLoader
{
    public:
        spell_dk_winter_is_coming() : SpellScriptLoader("spell_dk_winter_is_coming") { }

        class spell_dk_winter_is_coming_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_winter_is_coming_SpellScript);

            enum eSpells
            {
                PillarOfFrost           = 51271,
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::PillarOfFrost))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_winter_is_coming_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_winter_is_coming_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Sindragosa's Fury - 190778
class spell_dk_sindragosa_fury : public SpellScriptLoader
{
    public:
        spell_dk_sindragosa_fury() : SpellScriptLoader("spell_dk_sindragosa_fury") { }

        class spell_dk_sindragosa_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_sindragosa_fury_SpellScript);

            enum eEntry
            {
                Sindragosa = 92870
            };

            enum eSpells
            {
                BreathVisual = 171686
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                float l_X = l_Caster->m_positionX - 10.0f * (std::cos(l_Caster->GetOrientation()));
                float l_Y = l_Caster->m_positionY - 10.0f * (std::sin(l_Caster->GetOrientation()));
                SpellDestination* l_SpawnPosition = new SpellDestination(l_X, l_Y, l_Caster->m_positionZ + 10.0f, l_Caster->GetOrientation(), l_Caster->GetMapId());
                Creature* l_Sindragosa = l_Caster->SummonCreature(eEntry::Sindragosa, l_SpawnPosition, TEMPSUMMON_TIMED_DESPAWN, 1500);
                if (!l_Sindragosa)
                    return;

                l_Caster->CastSpell(l_Sindragosa, eSpells::BreathVisual, true);

                float l_DestX = l_Caster->m_positionX + 40.0f * (std::cos(l_Caster->GetOrientation()));
                float l_DestY = l_Caster->m_positionY + 40.0f * (std::sin(l_Caster->GetOrientation()));
                l_Sindragosa->GetMotionMaster()->MovePoint(0, l_DestX, l_DestY, l_Sindragosa->m_positionZ, false);

                delete l_SpawnPosition;
            }

            void Register() override
            {
                AfterCast+= SpellCastFn(spell_dk_sindragosa_fury_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_sindragosa_fury_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Crystalline Sword - 189186
class spell_dk_crystalline_sword : public SpellScriptLoader
{
    public:
        spell_dk_crystalline_sword() : SpellScriptLoader("spell_dk_crystalline_sword") { }

        class spell_dk_crystalline_sword_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_crystalline_sword_AuraScript);

            enum eSpells
            {
                CrystallineSwordLaunchLeft      = 205165,
                CrystallineSwordLaunchRight     = 205164,
                CrystallineSwordOrbitingLeft    = 205160,
                CrystallineSwordOrbitingRight   = 205159
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::CrystallineSwordsCounter) += 1;

                if (!l_Caster->HasAura(eSpells::CrystallineSwordOrbitingRight))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::CrystallineSwordOrbitingRight, true);
                    return;
                }

                if (!l_Caster->HasAura(eSpells::CrystallineSwordOrbitingLeft))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::CrystallineSwordOrbitingLeft, true);
                    return;
                }

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::CrystallineSwordsCounter) < 9) ///< got information from wowhead
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::CrystallineSwordsCounter) = 0;

                l_Caster->RemoveAurasDueToSpell(eSpells::CrystallineSwordOrbitingRight);
                l_Caster->RemoveAurasDueToSpell(eSpells::CrystallineSwordOrbitingLeft);
                l_Caster->CastSpell(l_Target, eSpells::CrystallineSwordLaunchRight, true);
                l_Caster->CastSpell(l_Target, eSpells::CrystallineSwordLaunchLeft, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_crystalline_sword_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_crystalline_sword_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Crystalline Sword Launch - 205165, 205164
class spell_dk_crystalline_sword_launch : public SpellScriptLoader
{
public:
    spell_dk_crystalline_sword_launch() : SpellScriptLoader("spell_dk_crystalline_sword_launch") { }

    class spell_dk_crystalline_sword_launch_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_crystalline_sword_launch_SpellScript);

        enum eSpells
        {
            RunicChills = 238079,
            SindragosasFury = 190778
        };

        void HandleOnHitTarget(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::RunicChills, EFFECT_0))
                l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::SindragosasFury, -l_AuraEffect->GetAmount());
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dk_crystalline_sword_launch_SpellScript::HandleOnHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_crystalline_sword_launch_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Remorseless Winter (damage) - 196771
class spell_dk_remorseless_winter : public SpellScriptLoader
{
    public:
        spell_dk_remorseless_winter() : SpellScriptLoader("spell_dk_remorseless_winter") { }

        class spell_dk_remorseless_winter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_remorseless_winter_SpellScript);

            enum eSpells
            {
                RemorselessWinterAura   = 211793,
                WinterIsComing          = 207170,
                WinterIsComingStack     = 211794,
                WinterIsComingStun      = 207171,
                FrozenSoul              = 189184,
                FrozenSoulStack         = 204957
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::RemorselessWinterAura, true);

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RemorlessWinterTargets].insert(l_Target->GetGUID());

                Aura* l_FrozenSoulAura = l_Caster->GetAura(eSpells::FrozenSoul);
                std::set<uint64>& l_FrozenSoulTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::FrozenSoulTargets];
                if (l_FrozenSoulAura && l_FrozenSoulTargets.find(l_Target->GetGUID()) == l_FrozenSoulTargets.end())
                {
                    l_FrozenSoulTargets.insert(l_Target->GetGUID());
                    l_Caster->CastSpell(l_Caster, eSpells::FrozenSoulStack, true);
                }

                Aura* l_DebuffStacking = l_Target->GetAura(eSpells::WinterIsComingStack);
                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::WinterIsComing, EFFECT_0);
                if (!l_DebuffStacking || !l_AuraEffect)
                    return;

                if (l_DebuffStacking->GetStackAmount() >= l_AuraEffect->GetAmount())
                {
                    std::set<uint64>& l_StunnedsForCaster = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RemorselessWinterStunnedTargets];

                    if (l_StunnedsForCaster.find(l_Target->GetGUID()) == l_StunnedsForCaster.end())
                    {
                        l_StunnedsForCaster.insert(l_Target->GetGUID());
                        l_Caster->CastSpell(l_Target, eSpells::WinterIsComingStun, true);
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RemorlessWinterTargets].clear();
                }, 15 * IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_dk_remorseless_winter_SpellScript::HandleOnHit);
                AfterCast += SpellCastFn(spell_dk_remorseless_winter_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_remorseless_winter_SpellScript();
        }
};

/// last update : 6.2.3
/// Remorseless Winter - 196770
class spell_dk_remorseless_winter_aura : public SpellScriptLoader
{
    public:
        spell_dk_remorseless_winter_aura() : SpellScriptLoader("spell_dk_remorseless_winter_aura") { }

        class spell_dk_remorseless_winter_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_remorseless_winter_aura_AuraScript);

            enum eSpells
            {
                FrozenSoul          = 189184,
                FrozenSoulDamages   = 204959,
                GatheringStormBuff  = 211805
            };

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RemorselessWinterStunnedTargets].clear();
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::FrozenSoul))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FrozenSoulDamages, true);
                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::FrozenSoulTargets].clear();
                l_Caster->RemoveAura(eSpells::GatheringStormBuff);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_remorseless_winter_aura_AuraScript::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_remorseless_winter_aura_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_remorseless_winter_aura_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Wraith Walk - 212552
class spell_dk_wraith_walk : public SpellScriptLoader
{
    public:
        spell_dk_wraith_walk() : SpellScriptLoader("spell_dk_wraith_walk") { }

        class spell_dk_spell_dk_wraith_walk_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_spell_dk_wraith_walk_SpellScript);

            enum eSpells
            {
                SongOfChiJi = 198909
            };

            void SendCastResult(Player* p_Caster, SpellInfo const* p_SpellInfo, Guid128 p_CastGuid)
            {
                WorldPacket l_Data(SMSG_CAST_FAILED, 4 + 4 + 4 + 4 + 1);
                l_Data.appendGuid128(p_CastGuid);
                l_Data << uint32(p_SpellInfo->Id);                                  ///< spellId
                l_Data << uint32(p_SpellInfo->GetSpellXSpellVisualId(p_Caster));      ///< SpellXSpellVisualID
                l_Data << uint32(SPELL_FAILED_PREVENTED_BY_MECHANIC);
                l_Data << uint32(MECHANIC_DISORIENTED);
                l_Data << uint32(0);
                p_Caster->GetSession()->SendPacket(&l_Data);
            }

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer() || !GetSpellInfo() || !GetSpell())
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->HasAura(eSpells::SongOfChiJi))
                {
                    SendCastResult(l_Caster->ToPlayer(), GetSpellInfo(), GetSpell()->GetCastGuid());
                    return SPELL_FAILED_DONT_REPORT;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_spell_dk_wraith_walk_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_spell_dk_wraith_walk_SpellScript();
        }

        class spell_dk_wraith_walk_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_wraith_walk_AuraScript);

            enum eSpells
            {
                WhiteWalker         = 212765,
                WhiteWalkerAura     = 212764,
                MarchOfTheDamned    = 219779,
                WraithWalkImmunity  = 212654
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Aura* l_BaseAura = GetAura();
                if (!l_BaseAura)
                    return;

                if (l_Target->HasAura(eSpells::WhiteWalker))
                    l_Target->CastSpell(l_Target, eSpells::WhiteWalkerAura, true);

                if (l_Target->HasAura(eSpells::MarchOfTheDamned))
                {
                    if (Aura* l_WrathWalkImmunity = l_Target->GetAura(eSpells::WraithWalkImmunity))
                    {
                        l_WrathWalkImmunity->SetDuration(l_BaseAura->GetDuration());
                        l_WrathWalkImmunity->SetMaxDuration(l_BaseAura->GetDuration());
                    }

                    std::list<AuraEffect*> l_EffectList = l_Target->GetAuraEffectsByMechanic((1 << Mechanics::MECHANIC_SNARE) | (1 << Mechanics::MECHANIC_ROOT) | (1 << Mechanics::MECHANIC_SLEEP) | (1 << Mechanics::MECHANIC_STUN) | (1 << Mechanics::MECHANIC_FREEZE) | (1 << Mechanics::MECHANIC_DAZE) | (1 << Mechanics::MECHANIC_SAPPED));

                    for (AuraEffect* l_Effect : l_EffectList)
                    {
                        Aura* l_Aura = l_Effect->GetBase();
                        if (!l_Aura)
                            return;

                        if (!l_Aura->GetSpellInfo())
                            return;

                        if (!l_Aura->GetSpellInfo()->IsPositive())
                            l_Aura->Remove();
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();

                if (l_Target->HasAura(eSpells::WhiteWalker))
                    l_Target->CastSpell(l_Target, eSpells::WhiteWalkerAura, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_wraith_walk_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_wraith_walk_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_wraith_walk_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Obliterate - 49020, Obliterate (Main Hand) - 222024, Obliterate (Off Hand) - 66198, Frostscythe - 207230, Frost Strike - 49143, Frost Strike (Main Hand) - 222026, Frost Strike (Off Hand) - 66196
class spell_dk_icecap_effect : public SpellScriptLoader
{
    public:
        spell_dk_icecap_effect() : SpellScriptLoader("spell_dk_icecap_effect") { }

        class spell_dk_icecap_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_icecap_effect_SpellScript);

            enum eSpells
            {
                ObliterateCast = 49020,
                FrostStrikeCast = 49143,
                Frostscythe = 207230,
                PilarofFrost = 51271,
                Icecap = 207126
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IcecapFirstTarget) = false;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IcecapFirstTarget))
                    return;

                if (l_Caster->IsPlayer() && GetSpell()->IsCritForTarget(l_Target) && l_Caster->HasAura(eSpells::Icecap))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Icecap))
                    {
                        l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::PilarofFrost, (l_SpellInfo->Effects[EFFECT_0].BasePoints / 10) * IN_MILLISECONDS);

                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::IcecapFirstTarget) = true;
                    }
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::ObliterateCast:
                    case eSpells::FrostStrikeCast:
                        OnPrepare += SpellOnPrepareFn(spell_dk_icecap_effect_SpellScript::HandleOnPrepare);
                        break;
                    case eSpells::Frostscythe:
                        OnPrepare += SpellOnPrepareFn(spell_dk_icecap_effect_SpellScript::HandleOnPrepare);
                        OnHit += SpellHitFn(spell_dk_icecap_effect_SpellScript::HandleOnHit);
                        break;
                    default:
                        OnHit += SpellHitFn(spell_dk_icecap_effect_SpellScript::HandleOnHit);
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_icecap_effect_SpellScript();
        }
};

class PlayerScript_dk_gathering_storm : public PlayerScript
{
    public:
        PlayerScript_dk_gathering_storm() : PlayerScript("PlayerScript_gathering_storm") {}

        enum eSpells
        {
            RemorselessWinter   = 196770,
            GatheringStormAura  = 211805,
            GatheringStorm      = 194912
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After)
                return;

            if (p_Player->getClass() != CLASS_DEATH_KNIGHT || p_Power != POWER_RUNES || p_Regen || p_NewValue >= p_OldValue)
                return;

            int32 l_PowerSpent = p_OldValue - p_NewValue;

            Aura* l_Aura = p_Player->GetAura(eSpells::RemorselessWinter);

            if (l_Aura == nullptr)
                return;

            if (AuraEffect* l_GatheringStorm = p_Player->GetAuraEffect(eSpells::GatheringStorm, EFFECT_0))
            {
                l_Aura->SetDuration(l_Aura->GetDuration() + (l_PowerSpent * l_GatheringStorm->GetAmount() * 100));
                for (int8 l_I = 0; l_I < l_PowerSpent; ++l_I)
                    p_Player->CastSpell(p_Player, eSpells::GatheringStormAura, true);
            }
        }
};

class UnholyFrenzyUpdater
{
    public:

        enum eSpells
        {
            UnholyFrenzyTalent      = 207289,
            UnholyFrenzyTalentSpeed = 207290
        };

        UnholyFrenzyUpdater(Unit* p_Owner) : m_Owner(p_Owner)
        {
            m_DefaultDuration = sSpellMgr->GetSpellInfo(eSpells::UnholyFrenzyTalentSpeed)->GetDuration();
        }

        void Update(int32 p_Stacks)
        {
            if (!m_Owner->HasAura(eSpells::UnholyFrenzyTalent))
                return;

            int32 l_Duration = std::max(int32(1), p_Stacks) * m_DefaultDuration;

            Aura* l_Aura = nullptr;
            if ((l_Aura = m_Owner->GetAura(eSpells::UnholyFrenzyTalentSpeed)))
            {
                int32 l_NewDuration = l_Aura->GetDuration() + l_Duration;
                l_Aura->SetMaxDuration(l_NewDuration);
                l_Aura->SetDuration(l_NewDuration);
            }
            else if ((l_Aura = m_Owner->AddAura(eSpells::UnholyFrenzyTalentSpeed, m_Owner)))
            {
                l_Aura->SetMaxDuration(l_Duration);
                l_Aura->SetDuration(l_Duration);
            }
        }

    private:

        Unit* m_Owner;
        uint32 m_DefaultDuration;
};

/// last update : 7.1.5 Build 23420
/// Called by Unholy Talent 'Unholy Frenzy' proc Aura: 207290 to prevent stacking the proc and then changing talent (aura can stack up to 1 hour+)
class spell_dk_unholy_frenzy_remove : public SpellScriptLoader
{
    public:
        spell_dk_unholy_frenzy_remove() : SpellScriptLoader("spell_dk_unholy_frenzy_remove") { }

        class spell_dk_unholy_frenzy_remove_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_unholy_frenzy_remove_AuraScript);

            enum eSpells
            {
                UnholyFrenzyPassiveAura = 207289
            };

            uint32 m_Timer = 0;

            void OnUpdate(uint32 p_Diff)
            {
                if (m_Timer < 1000)
                {
                    m_Timer += p_Diff;
                    return;
                }

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::UnholyFrenzyPassiveAura))
                    l_Caster->RemoveAura(m_scriptSpellId);

                m_Timer = 0;
            }

            void Register()
            {
                OnAuraUpdate += AuraUpdateFn(spell_dk_unholy_frenzy_remove_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_unholy_frenzy_remove_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Festering Wound damage - 194311
class spell_dk_festering_wound : public SpellScriptLoader
{
    public:
        spell_dk_festering_wound() : SpellScriptLoader("spell_dk_festering_wound") { }

        class spell_dk_festering_wound_SpellScript : public SpellScript
        {
            enum eSpells
            {
                FesteringWoundEnergize  = 195757,
                BurstingSores           = 207264,
                BurstingSoresDamage     = 207267,
                SoulReaperAura          = 130736,
                SoulReaperSpeed         = 215711,
                T19Unholy2P             = 211047,
                RunicCorruption         = 51460
            };

            PrepareSpellScript(spell_dk_festering_wound_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                int32 l_Stacks = 1;
                if (Spell const* l_Spell = GetSpell())
                    l_Stacks = l_Spell->GetSpellValue(SpellValueMod::SPELLVALUE_BASE_POINT2);

                UnholyFrenzyUpdater l_UnholyFrenzyUpdater(l_Caster);
                l_UnholyFrenzyUpdater.Update(l_Stacks);

                l_Caster->CastSpell(l_Caster, eSpells::FesteringWoundEnergize, true);

                if (l_Caster->HasAura(eSpells::BurstingSores))
                    l_Caster->CastSpell(l_Target, eSpells::BurstingSoresDamage, true);

                if (l_Target->HasAura(eSpells::SoulReaperAura, l_Caster->GetGUID()))
                    l_Caster->CastSpell(l_Caster, eSpells::SoulReaperSpeed, true);

                /// Item - Death Knight T19 Unholy 2P Bonus
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Unholy2P, EFFECT_0))
                    if (roll_chance_i(l_AuraEffect->GetAmount()))
                        l_Caster->CastSpell(l_Caster, eSpells::RunicCorruption, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_festering_wound_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_festering_wound_SpellScript();
        }
};

/// last update : 7.0.3
/// Festering Wound - 194310 (on expire)
class spell_dk_festering_wound_aura : public SpellScriptLoader
{
    public:
        spell_dk_festering_wound_aura() : SpellScriptLoader("spell_dk_festering_wound_aura") { }

        class spell_dk_festering_wound_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_festering_wound_aura_AuraScript);

            enum eSpells
            {
                FesteringWoundDamage = 194311
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                std::set<uint64>& l_FesteringWoundTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::FesteringWound];
                l_FesteringWoundTargets.insert(l_Target->GetGUID());
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                std::set<uint64>& l_FesteringWoundTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::FesteringWound];
                l_FesteringWoundTargets.erase(l_Target->GetGUID());
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_festering_wound_aura_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_festering_wound_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_festering_wound_aura_AuraScript();
        }
};

/// last update : 7.0.3
/// Festering Wound - 194310 (on expire)
class spell_dk_pestilent_pustules : public SpellScriptLoader
{
    public:
        spell_dk_pestilent_pustules() : SpellScriptLoader("spell_dk_pestilent_pustules") { }

        class spell_dk_pestilent_pustules_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_pestilent_pustules_AuraScript);

            enum eSpells
            {
                PestilentPustulesEnergize   = 220211
            };

            void SetData(uint32, uint32 p_Amount) override
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect* l_Eff = GetEffect(EFFECT_0))
                {
                    l_Eff->SetAmount(l_Eff->GetAmount() - p_Amount);
                    /// reached amount of Festering Wounds, reset counter and give rune
                    if (l_Eff->GetAmount() <= 0)
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::PestilentPustulesEnergize, true);
                        l_Eff->SetAmount(l_Eff->GetBaseAmount());
                    }
                }
            }

            void Register() override
            {
                /// nothing here, only overrider SetData
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_pestilent_pustules_AuraScript();
        }
};

/// Blighted Rune Weapon Proc Trigger - 195758
/// Updated as of 7.0.3 - 22522
class spell_dk_blighted_rune_weapon : public SpellScriptLoader
{
    public:
        spell_dk_blighted_rune_weapon() : SpellScriptLoader("spell_dk_blighted_rune_weapon") { }

        class spell_dk_blighted_rune_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_blighted_rune_weapon_SpellScript);

            enum eSpellIds
            {
                FesteringWound          = 194310
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                for (uint8 l_Itr = 0; l_Itr < 2; l_Itr++)
                    l_Caster->CastSpell(l_Target, eSpellIds::FesteringWound, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_blighted_rune_weapon_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_blighted_rune_weapon_SpellScript();
        }
};

/// Last Update 7.1.5
/// Corpse Shield - 207319
class spell_dk_corpse_shield : public SpellScriptLoader
{
    public:
        spell_dk_corpse_shield() : SpellScriptLoader("spell_dk_corpse_shield") { }

        class spell_dk_corpse_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_corpse_shield_AuraScript);

            enum eSpells
            {
                IgnobleSacrifice = 212756,
                CorpseShield = 212753
            };

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & p_DmgInfo, uint32 & p_AbsorbAmount)
            {
                Player* target = GetTarget()->ToPlayer();
                if (!target || p_DmgInfo.GetAmount() == 0)
                    return;

                p_AbsorbAmount = 0;

                if (Pet* pet = target->GetPet())
                {
                    if (!pet->isAlive())
                        return;

                    int32 l_AbosrbPct = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                    p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_AbosrbPct);
                    int32 l_Bp = p_AbsorbAmount;
                    if (pet->GetHealth() <= p_AbsorbAmount)
                        target->CastSpell(target, eSpells::IgnobleSacrifice, true);

                    target->CastCustomSpell(pet, eSpells::CorpseShield, &l_Bp, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_corpse_shield_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_corpse_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        class spell_dk_corpse_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_corpse_shield_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                    if (Player* l_Player = l_Caster->ToPlayer())
                        if (Pet* l_Pet = l_Player->GetPet())
                            if (l_Pet->GetEntry() == DeathKnightPet::Ghoul || l_Pet->GetEntry() == DeathKnightPet::Abomination)
                                return SPELL_CAST_OK;

                return SPELL_FAILED_NO_PET;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_corpse_shield_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_corpse_shield_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_corpse_shield_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Scourge Strike - 55090
class spell_dk_scourge_strike : public SpellScriptLoader
{
    public:
        spell_dk_scourge_strike() : SpellScriptLoader("spell_dk_scourge_strike") { }

        class spell_dk_scourge_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_scourge_strike_SpellScript);

            enum eSpells
            {
                FesteringWound          = 194310,
                FesteringWoundDamage    = 194311,
                Castigator              = 207305,
                LessonOfRazuvious       = 208713,
                NecrosisBuff            = 216974,
                DeathAndDecayEffect     = 188290,
                Defile = 152280
            };

            void FilterTargets(std::list<WorldObject*> &p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!HasAoEEffect(l_Caster))
                {
                    p_Targets.remove_if([l_Target](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == l_Target)
                            return false;

                        return true;
                    });
                }
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                Aura* l_FesteringWoundAura = l_Target->GetAura(eSpells::FesteringWound, l_Caster->GetGUID());
                if (!l_FesteringWoundAura)
                    return;

                if (l_FesteringWoundAura->GetStackAmount() > 1 && l_Caster->HasAura(eSpells::Castigator) && GetSpell()->IsCritForTarget(l_Target))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Castigator))
                    {
                        int32 l_StackAmount = l_SpellInfo->Effects[EFFECT_1].BasePoints;

                        for (int32 l_Itr = 0; l_Itr < l_StackAmount; l_Itr++)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::FesteringWoundDamage, true);
                            l_FesteringWoundAura->ModStackAmount(-1);

                            if (Aura* l_PestilentPustules = l_Caster->GetAura(DeathKnightSpells::PestilentPustules))
                                l_PestilentPustules->SetScriptData(0, 1);
                        }
                    }
                }

                int32 l_NbIter = 1;
                if (AuraEffect* l_LessonOfRazuvious = l_Caster->GetAuraEffect(eSpells::LessonOfRazuvious, EFFECT_0))
                {
                    /// http://www.askmrrobot.com/wow/theory/mechanic/spell/festeringwound?spec=DeathKnightUnholy&version=7_1_5_23420
                    int32 l_Rand = urand(1, 21);
                    if (l_Rand > 4 && l_Rand <= 12)
                        l_NbIter += 1;
                    else if (l_Rand > 12 && l_Rand <= 17)
                        l_NbIter += 2;
                    else if (l_Rand > 17 && l_Rand <= 20)
                        l_NbIter += 3;
                }

                if (l_FesteringWoundAura->GetStackAmount() < l_NbIter)
                    l_NbIter = l_FesteringWoundAura->GetStackAmount();

                for (int32 l_Itr = 0; l_Itr < l_NbIter; l_Itr++)
                {
                    l_Caster->CastSpell(l_Target, eSpells::FesteringWoundDamage, true);

                    l_FesteringWoundAura->ModStackAmount(-1);

                    if (Aura* l_PestilentPustules = l_Caster->GetAura(DeathKnightSpells::PestilentPustules))
                        l_PestilentPustules->SetScriptData(0, 1);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::NecrosisBuff);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_scourge_strike_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_scourge_strike_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_dk_scourge_strike_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
                AfterCast += SpellCastFn(spell_dk_scourge_strike_SpellScript::HandleAfterCast);
            }

        private:

            bool HasAoEEffect(Unit* p_Caster)
            {
                if (p_Caster->HasAura(DeathAndDecayEffect))
                    return true;

                if (AreaTrigger* l_AreaTrigger = p_Caster->GetAreaTrigger(eSpells::Defile))
                    if (l_AreaTrigger->GetDistance(p_Caster) <= l_AreaTrigger->GetRadius())
                        return true;

                return false;
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_scourge_strike_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Scourge Strike - 70890
class spell_dk_scourge_strike_shadow : public SpellScriptLoader
{
    public:
        spell_dk_scourge_strike_shadow() : SpellScriptLoader("spell_dk_scourge_strike_shadow") { }

        class spell_dk_scourge_strike_shadow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_scourge_strike_shadow_SpellScript);

            enum eSpells
            {
                DeathAndDecayEffect = 188290,
                Defile = 152280
            };

            void FilterTargets(std::list<WorldObject*> &p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!HasAoEEffect(l_Caster))
                {
                    p_Targets.remove_if([l_Target](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == l_Target)
                            return false;

                        return true;
                    });
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_scourge_strike_shadow_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }

        private:

            bool HasAoEEffect(Unit* p_Caster)
            {
                if (p_Caster->HasAura(DeathAndDecayEffect))
                    return true;

                if (AreaTrigger* l_AreaTrigger = p_Caster->GetAreaTrigger(eSpells::Defile))
                    if (l_AreaTrigger->GetDistance(p_Caster) <= l_AreaTrigger->GetRadius())
                        return true;

                return false;
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_scourge_strike_shadow_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Clawing Shadows - 207311
class spell_dk_clawing_shadows : public SpellScriptLoader
{
    public:
        spell_dk_clawing_shadows() : SpellScriptLoader("spell_dk_clawing_shadows") { }

        class spell_dk_clawing_shadows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_clawing_shadows_SpellScript);

            enum eSpells
            {
                FesteringWound          = 194310,
                FesteringWoundDamage    = 194311,
                Castigator              = 207305,
                DeathAndDecayEffect     = 188290,
                Defile                  = 152280,
                NecrosisBuff            = 216974,
                LessonOfRazuvious       = 208713
            };

            void FilterTargets(std::list<WorldObject*> &p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!HasAoEEffect(l_Caster))
                {
                    p_Targets.remove_if([l_Target](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == l_Target)
                            return false;

                        return true;
                    });
                }

                l_Caster->GetAreaTrigger(eSpells::Defile);
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                Aura* l_FesteringWoundAura = l_Target->GetAura(eSpells::FesteringWound, l_Caster->GetGUID());
                if (!l_FesteringWoundAura)
                    return;

                int32 l_NbIter = 1;
                if (AuraEffect* l_LessonOfRazuvious = l_Caster->GetAuraEffect(eSpells::LessonOfRazuvious, EFFECT_0))
                {
                    /// 20% - 1 stack, 40% - 2 stacks, 20% - 3 stacks
                    /// http://ru.wowhead.com/item=132448#english-comments
                    int32 l_Rand = urand(0, 100);
                    if (l_Rand <= 20)
                        l_NbIter += 1;
                    else if (l_Rand <= 60)
                        l_NbIter += 2;
                    else if (l_Rand <= 80)
                        l_NbIter += l_LessonOfRazuvious->GetAmount();
                }

                if (l_FesteringWoundAura->GetStackAmount() < l_NbIter)
                    l_NbIter = l_FesteringWoundAura->GetStackAmount();

                for (int32 l_Itr = 0; l_Itr < l_NbIter; l_Itr++)
                {
                    l_Caster->CastSpell(l_Target, eSpells::FesteringWoundDamage, true);

                    l_FesteringWoundAura->ModStackAmount(-1);

                    if (Aura* l_PestilentPustules = l_Caster->GetAura(DeathKnightSpells::PestilentPustules))
                        l_PestilentPustules->SetScriptData(0, 1);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::NecrosisBuff);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_clawing_shadows_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_clawing_shadows_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_dk_clawing_shadows_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                AfterCast += SpellCastFn(spell_dk_clawing_shadows_SpellScript::HandleAfterCast);
            }

        private:

            bool HasAoEEffect(Unit* p_Caster)
            {
                if (p_Caster->HasAura(DeathAndDecayEffect))
                    return true;

                if (AreaTrigger* l_AreaTrigger = p_Caster->GetAreaTrigger(eSpells::Defile))
                    if (l_AreaTrigger->GetDistance(p_Caster) <= l_AreaTrigger->GetRadius())
                        return true;

                return false;
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_clawing_shadows_SpellScript();
        }
};

/// Marrowrend - 195182
class spell_dk_marrowrend : public SpellScriptLoader
{
    public:
        spell_dk_marrowrend() : SpellScriptLoader("spell_dk_marrowrend") { }

        class spell_dk_marrowrend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_marrowrend_SpellScript);

            enum eSpells
            {
                BoneShield  = 195181
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                /// The spell can be casted by Dancing Rune Weapon
                Player* l_Player = l_Caster->GetCharmerOrOwnerPlayerOrPlayerItself();
                if (!l_Player)
                    return;

                int32 l_StackAmount = l_SpellInfo->Effects[EFFECT_2].BasePoints;

                for (int32 l_Itr = 0; l_Itr < l_StackAmount; l_Itr++)
                    l_Player->CastSpell(l_Caster, eSpells::BoneShield, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_marrowrend_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_marrowrend_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Crimson Scourge - 81136
class spell_dk_crimson_scourge_aura : public SpellScriptLoader
{
    public:
        spell_dk_crimson_scourge_aura() : SpellScriptLoader("spell_dk_crimson_scourge_aura") { }

        class spell_dk_crimson_scourge_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_crimson_scourge_aura_AuraScript);

            enum eSpells
            {
                CrimsonScourge      = 81141,
                BloodPlague         = 55078,
                DeathAndDecay       = 43265,
                DeathAndDecayBuff   = 188290
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Target = p_EventInfo.GetActionTarget();
                Unit* l_Caster = GetCaster();

                if (l_Target == nullptr || l_Caster == nullptr)
                    return;

                if (!l_Target->HasAura(eSpells::BloodPlague) || l_Caster->HasAura(eSpells::DeathAndDecay))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CrimsonScourge, true);
                if (Player* l_Player = l_Caster->ToPlayer())
                    l_Player->RemoveSpellCooldown(eSpells::DeathAndDecay, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_crimson_scourge_aura_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_crimson_scourge_aura_AuraScript();
        }
};

/// Last Update 7.3.5 - build 26365
/// Called by Death Grip - 49576
class spell_dk_death_grip_dummy : public SpellScriptLoader
{
    public:
        spell_dk_death_grip_dummy() : SpellScriptLoader("spell_dk_death_grip_dummy") { }

        class spell_dk_death_grip_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_grip_dummy_SpellScript);

            enum eSpells
            {
                DeathGripEffect     = 49575,
                BloodDeathKnight    = 137008,
                DeathGripTaunt      = 51399,
                DeathChill          = 204080,
                ChainsOfIce         = 45524,
                WarlocksNetherWard  = 212295
            };

            SpellCastResult CheckCast()
            {
                if (Unit* caster = GetCaster())
                    if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
                        return SPELL_FAILED_MOVING;

                return SPELL_CAST_OK;
            }

            void HandleDummy()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetExplTargetUnit())
                    {
                        ///< Death grip effect reflecting
                        int32 l_ReflectChance = l_Target->GetTotalAuraModifier(SPELL_AURA_REFLECT_SPELLS);
                        Unit::AuraEffectList const& l_MReflectSpellsSchool = l_Target->GetAuraEffectsByType(SPELL_AURA_REFLECT_SPELLS_SCHOOL);
                        for (Unit::AuraEffectList::const_iterator l_Itr = l_MReflectSpellsSchool.begin(); l_Itr != l_MReflectSpellsSchool.end(); ++l_Itr)
                            if ((*l_Itr)->GetMiscValue() & GetSpellInfo()->GetSchoolMask())
                                l_ReflectChance += (*l_Itr)->GetAmount();

                        if (roll_chance_i(l_ReflectChance))
                            return;

                        if (!l_Target->IsImmunedToSpell(GetSpellInfo()))
                        {
                            if (l_Target->ToCreature() && !l_Target->ToCreature()->IsDungeonBoss())
                            {
                                Spell* l_CurrentSpell = l_Target->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
                                if (!l_CurrentSpell)
                                {
                                    l_CurrentSpell = l_Target->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL);
                                    if (!l_CurrentSpell)
                                    {
                                        l_CurrentSpell = l_Target->GetCurrentSpell(CURRENT_GENERIC_SPELL);
                                        if (!l_CurrentSpell)
                                            l_CurrentSpell = l_Target->GetCurrentSpell(CURRENT_MELEE_SPELL);
                                    }
                                }

                                if (l_CurrentSpell)
                                {
                                    if (l_CurrentSpell->GetSpellInfo()->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT ||
                                        l_CurrentSpell->GetSpellInfo()->InterruptFlags & SPELL_INTERRUPT_FLAG_PUSH_BACK)
                                        l_CurrentSpell->cancel();
                                }

                                if (!l_Target->HasUnitState(UNIT_STATE_STUNNED))
                                {
                                    uint32 l_Distance = l_Target->GetExactDist2d(l_Caster);
                                    uint32 l_StunDuration = 300 + (l_Distance * l_Distance);
                                    l_Target->AddUnitState(UNIT_STATE_STUNNED);
                                    l_Target->AddDelayedEvent([l_Target]() -> void
                                    {
                                        l_Target->ClearUnitState(UNIT_STATE_STUNNED);
                                    }, l_StunDuration);
                                }
                            }

                            l_Target->CastSpell(l_Caster, eSpells::DeathGripEffect, false);
                        }

                        if (l_Caster->HasAura(eSpells::BloodDeathKnight))
                            l_Caster->CastSpell(l_Target, eSpells::DeathGripTaunt, false);

                        if (l_Caster->HasAura(eSpells::DeathChill))
                            l_Caster->CastSpell(l_Target, eSpells::ChainsOfIce, true);
                    }
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_grip_dummy_SpellScript::CheckCast);
                OnCast += SpellCastFn(spell_dk_death_grip_dummy_SpellScript::HandleDummy);
            }

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_grip_dummy_SpellScript();
        }
};

/// Called by Killing Machine - 51124
/// Murderous Efficiency - 207061 / 207062
class spell_dk_murderous_efficiency : public SpellScriptLoader
{
    public:
        spell_dk_murderous_efficiency() : SpellScriptLoader("spell_dk_murderous_efficiency") { }

        class spell_dk_murderous_efficiency_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_murderous_efficiency_AuraScript);

            enum eSpells
            {
                MurderousEfficiency         = 207061,
                MurderousEfficiencyEnergize = 207062
            };

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->HasSpell(eSpells::MurderousEfficiency))
                        return;

                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MurderousEfficiency))
                    {
                        uint32 l_Rand = urand(0, 100);

                        if (l_Rand >= l_SpellInfo->Effects[EFFECT_0].BasePoints)
                            l_Caster->CastSpell(l_Caster, eSpells::MurderousEfficiencyEnergize, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_dk_murderous_efficiency_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_murderous_efficiency_AuraScript();
        }
};

/// Last update 7.1.5
/// Called by Glacial Advance - 194913
class spell_dk_glacial_advance : public SpellScriptLoader
{
    public:
        spell_dk_glacial_advance() : SpellScriptLoader("spell_dk_glacial_advance") { }

        class spell_dk_glacial_advance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_glacial_advance_SpellScript);

            enum eSpells
            {
                GlacialAdvanceDamage = 195975
            };

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GlacialAdvanceDamage);
                    if (!l_SpellInfo)
                        return;

                    CustomSpellValues l_Values;
                    SpellCastTargets l_SpellTarget;
                    l_SpellTarget.SetCaster(l_Caster);

                    Position const* l_PositionTarget = GetExplTargetDest();
                    float l_Angle = l_Caster->GetAngle(l_PositionTarget);

                    l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::GlacialAdvance].clear();

                    for (int l_Itr = 1; l_Itr < 9; l_Itr++)
                    {
                        float x = l_Caster->GetPositionX() + 3.0f * std::cos(l_Angle) * l_Itr;
                        float y = l_Caster->GetPositionY() + 3.0f * std::sin(l_Angle) * l_Itr;
                        float z = l_Caster->GetPositionZ();

                        l_Caster->UpdateGroundPositionZ(x, y, z);
                        Position l_Position;
                        l_Position.Relocate(x, y, z, 0.0f);
                        l_SpellTarget.SetDst(l_Position);
                        l_Caster->CastSpell(l_SpellTarget, l_SpellInfo, &l_Values);
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_dk_glacial_advance_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_glacial_advance_SpellScript();
        }
};

/// Last update 7.1.5
/// Called by Glacial Advance - 195975
class spell_dk_glacial_advance_dmg : public SpellScriptLoader
{
public:
    spell_dk_glacial_advance_dmg() : SpellScriptLoader("spell_dk_glacial_advance_dmg") { }

    class spell_dk_glacial_advance_dmg_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_glacial_advance_dmg_SpellScript);

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();

            if (!l_Caster || !l_Target)
                return;

            std::set<uint64>& l_HitTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::GlacialAdvance];

            if (l_HitTargets.find(l_Target->GetGUID()) == l_HitTargets.end())
            {
                l_HitTargets.insert(l_Target->GetGUID());
                return;
            }

            PreventHitDamage();
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_dk_glacial_advance_dmg_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dk_glacial_advance_dmg_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Apocalypse - 220143
class spell_dk_apocalypse : public SpellScriptLoader
{
    public:
        spell_dk_apocalypse() : SpellScriptLoader("spell_dk_apocalypse") { }

        class spell_dk_apocalypse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_apocalypse_SpellScript);

            enum eSpells
            {
                FesteringWound       = 194310,
                FesteringWoundDamage = 194311,
                ArmyoftheDead        = 205491,  /// Not sure about this id
                DeathsHarbinger      = 238080
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Aura* l_FesteringWoundAura = l_Target->GetAura(eSpells::FesteringWound, l_Caster->GetGUID());
                if (!l_FesteringWoundAura)
                    return;

                uint8 l_MaxCount = (uint8)l_SpellInfo->Effects[EFFECT_2].BasePoints;

                if (l_Player->CanApplyPvPSpellModifiers())
                    l_MaxCount = 4;

                uint8 l_StackFesteringWound = std::min(l_MaxCount, l_FesteringWoundAura->GetStackAmount());

                for (uint8 l_Itr = 0; l_Itr < l_StackFesteringWound && l_Itr < l_MaxCount; l_Itr++)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::ArmyoftheDead, true);
                    l_Caster->CastSpell(l_Target, eSpells::FesteringWoundDamage, true);
                    l_FesteringWoundAura->ModStackAmount(-1);

                    if (Aura* l_PestilentPustules = l_Caster->GetAura(DeathKnightSpells::PestilentPustules))
                        l_PestilentPustules->SetScriptData(0, 1);
                }
            }

            void HandleEnergize(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::DeathsHarbinger))
                    PreventHitDefaultEffect(p_EffIndex);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dk_apocalypse_SpellScript::HandleOnHit);
                OnEffectHitTarget += SpellEffectFn(spell_dk_apocalypse_SpellScript::HandleEnergize, EFFECT_3, SPELL_EFFECT_ENERGIZE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_apocalypse_SpellScript();
        }
};

/// Scourge the Unbeliever - 191494, called by itself
/// Updated as of 7.3.5 build 26365
class spell_dk_scourge_the_unbeliever : public SpellScriptLoader
{
    public:
        spell_dk_scourge_the_unbeliever() : SpellScriptLoader("spell_dk_scourge_the_unbeliever") { }

        class spell_dk_scourge_the_unbeliever_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_scourge_the_unbeliever_AuraScript);

            enum eArtifactPowerIds
            {
                ScourgeTheUnbeliever = 265
            };

            bool OnCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (!GetCaster() || !p_EventInfo.GetDamageInfo())
                    return false;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return false;

                SpellInfo const* l_DamageSpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_DamageSpellInfo || !l_DamageSpellInfo->GetSpellClassOptions())
                    return false;

                /// proc only on Source Strike
                if (!(l_DamageSpellInfo->GetSpellClassOptions()->SpellFamilyFlags & flag128(0x0, 0x8000000, 0x0, 0x80000000)))
                    return false;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return false;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowerIds::ScourgeTheUnbeliever, l_Manager->GetCurrentRankWithBonus(eArtifactPowerIds::ScourgeTheUnbeliever) - 1);
                if (!l_RankEntry)
                    return false;

                /// determine chance to proc
                if (l_RankEntry->AuraPointsOverride > 0)
                {
                    if (roll_chance_f(l_RankEntry->AuraPointsOverride))
                        return true; ///< so that it can carry on in HandleDummyAuraProc
                }

                return false;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_scourge_the_unbeliever_AuraScript::OnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_scourge_the_unbeliever_AuraScript();
        }
};

/// Last Update 7.0.3
/// Pandemic - 199724
class spell_dk_pandemic : public SpellScriptLoader
{
    public:
        spell_dk_pandemic() : SpellScriptLoader("spell_dk_pandemic") { }

        class spell_dk_pandemic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_pandemic_AuraScript);

            enum eSpell
            {
                Outbreak = 77575
            };

            void OnProc(AuraEffect const* p_AuraEffect, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpell::Outbreak)
                    return;

                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, p_AuraEffect->GetTriggerSpell(), true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_pandemic_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_pandemic_AuraScript();
        }
};

/// Last Update 7.0.3
/// Abomination's Might - 207161
class spell_dk_abominations_might : public SpellScriptLoader
{
    public:
        spell_dk_abominations_might() : SpellScriptLoader("spell_dk_abominations_might") { }

        class spell_dk_abominations_might_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_abominations_might_AuraScript);

            enum eSpells
            {
                Obliterate                = 49020,
                Obliterate2               = 222024,
                AbominationsMightPlayer   = 211831,
                AbominationsMightCreature = 207165
            };

            void OnProc(AuraEffect const* p_AuraEffect, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (Unit* l_Caster = GetCaster())
                {

                    if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::Obliterate && p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::Obliterate2)
                        return;

                    if (!(p_EventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                        return;

                    if (Unit* l_Target = p_EventInfo.GetProcTarget())
                    {
                        if (l_Target->GetTypeId() == TYPEID_PLAYER)
                            l_Caster->CastSpell(l_Target, eSpells::AbominationsMightPlayer, true);
                        else
                            l_Caster->CastSpell(l_Target, eSpells::AbominationsMightCreature, true);
                    }
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_abominations_might_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_abominations_might_AuraScript();
        }
};

class PlayerScript_dk_death_gate : public PlayerScript
{
    public:
        PlayerScript_dk_death_gate() :PlayerScript("PlayerScript_Death_Gate") {}

        enum eSpells
        {
            DeathGate    = 50977,
            DeathCharger = 48778,
            RuneForge    = 53428
        };

        enum eMaps
        {
            StartingMap = 609
        };

        enum eQuests
        {
            PrevQuestDKAlly         = 13188,
            PrevQuestDKHorde        = 13189,
            TakingBackArcherus      = 13165,
            TheBattleForTheEbonHold = 13166
        };

        void OnTeleport(Player* p_Player, SpellInfo const*, bool) override
        {
            if (!(p_Player->getClass() == CLASS_DEATH_KNIGHT) && p_Player->HasSpell(eSpells::DeathGate))
            {
                p_Player->removeSpell(eSpells::DeathGate);
                return;
            }

            if (!(p_Player->getClass() == CLASS_DEATH_KNIGHT))
                return;

            if (p_Player->GetMapId() != eMaps::StartingMap)
            {
                if (!p_Player->HasSpell(eSpells::DeathGate))
                    p_Player->learnSpell(eSpells::DeathGate, false);

                if (!p_Player->HasSpell(eSpells::DeathCharger))
                    p_Player->learnSpell(eSpells::DeathCharger, false);

                if (!p_Player->HasSpell(eSpells::RuneForge))
                    p_Player->learnSpell(eSpells::RuneForge, false);

                uint32 l_QuestId = p_Player->getFaction() == TeamId::TEAM_ALLIANCE ? eQuests::PrevQuestDKAlly : eQuests::PrevQuestDKHorde;

                /// Quest in DK Starting Questline takes place in Map 0
                if (p_Player->HasQuest(TakingBackArcherus))
                    return;

                /// Quest in DK Starting Questline takes place in Map 0
                if (p_Player->HasQuest(TheBattleForTheEbonHold))
                    return;

                if (p_Player->HasQuest(l_QuestId))
                    return;

                Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_QuestId);

                uint32 l_QuestBit = GetQuestUniqueBitFlag(l_QuestId);
                if (!p_Player->IsQuestBitFlaged(l_QuestBit) && l_Quest)
                {
                    p_Player->CompleteQuest(l_QuestId);
                    p_Player->RewardQuest(l_Quest, 0, p_Player, false);
                }
            }
        }
};

/// Last Update 7.3.5 Build 26124
/// Consumption - 205223
class spell_dk_consumption : public SpellScriptLoader
{
    public:
        spell_dk_consumption() : SpellScriptLoader("spell_dk_consumption") { }

        class spell_dk_consumption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_consumption_SpellScript);

            enum eSpells
            {
                ConsumptionHeal = 205224
            };

            uint32 m_TotalHealing = 0;

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                uint32 l_Damage = GetHitDamage();
                if (!l_Caster || !l_Damage || !l_Target)
                    return;

                Player* l_PlayerOwner = l_Caster->GetCharmerOrOwnerPlayerOrPlayerItself();
                if (!l_PlayerOwner)
                    return;

                DamageInfo l_DamageInfo(l_Caster, l_Target, l_Damage, GetSpellInfo()->GetSchoolMask(), DamageEffectType::SPELL_DIRECT_DAMAGE);
                l_Caster->CalcArmorReducedDamage(l_DamageInfo);
                int32 l_ReducedDamage = l_DamageInfo.GetAmount();

                m_TotalHealing += l_ReducedDamage;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ConsumptionHeal);
                if (!l_Caster || !l_SpellInfo)
                    return;

                /// The spell can be casted by Dancing Rune Weapon
                if (Player* l_OriginalCaster = l_Caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    SouldrinkerHeal(l_OriginalCaster, m_TotalHealing);

                    l_OriginalCaster->HealBySpell(l_OriginalCaster, l_SpellInfo, m_TotalHealing);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_consumption_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                AfterCast += SpellCastFn(spell_dk_consumption_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_consumption_SpellScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Called by Dark Arbiter - 207349
class spell_dk_dark_arbiter : public SpellScriptLoader
{
public:
    spell_dk_dark_arbiter() : SpellScriptLoader("spell_dk_dark_arbiter") { }

    class spell_dk_dark_arbiter_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_dark_arbiter_SpellScript);

        void ChangeSummonPos(SpellEffIndex /*effIndex*/)
        {
            // Adjust effect summon position
            WorldLocation summonPos = *GetExplTargetDest();
            Position offset = { 0.0f, 0.0f, 8.2f, 0.0f };
            summonPos.RelocateOffset(offset);
            SetExplTargetDest(summonPos);
            GetHitDest()->RelocateOffset(offset);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_dk_dark_arbiter_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_dark_arbiter_SpellScript();
    }
};

/// Dark Arbiter - 207349
class PlayerScript_dk_dark_arbiter : public PlayerScript
{
    public:
        PlayerScript_dk_dark_arbiter() : PlayerScript("PlayerScript_dark_arbiter") {}

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After)
                return;

            if (p_Player->getClass() != CLASS_DEATH_KNIGHT || p_Power != POWER_RUNIC_POWER || p_Regen || p_NewValue > p_OldValue)
                return;

            int32 l_PowerSpent = p_OldValue - p_NewValue;
            DarkArbiterEmpowerment l_DarkArbiterEmpower;
            l_DarkArbiterEmpower.Empower(p_Player, l_PowerSpent);
        }
};

/// Last update 7.1.5 Build 23420
/// Infected Claws - 207272
/// Called by Claw - 91776, Sweeping Claws- 91778, Vile Gas - 212335, Cleaver - 212338
class spell_dk_infected_claws : public SpellScriptLoader
{
public:
    spell_dk_infected_claws() : SpellScriptLoader("spell_dk_infected_claws") { }

    class spell_dk_infected_claws_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_infected_claws_SpellScript);

        enum eSpellIds
        {
            /// Infected Claws
            InfectedClaws = 207272,
            FesteringWound = 194310
        };

        void HandleDummy(SpellEffIndex)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target || !l_Caster->ToPet() || !m_Cast)
                return;

            Unit* l_Owner = l_Caster->ToPet()->GetOwner();
            if (!l_Owner)
                return;

            l_Owner->CastSpell(l_Target, eSpellIds::FesteringWound, true);
        }

        void HandleBeforeCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->ToPet())
                return;

            Unit* l_Owner = l_Caster->ToPet()->GetOwner();
            if (!l_Owner)
                return;

            /// Infected Claws - 207272
            if (l_Owner->HasAura(eSpellIds::InfectedClaws))
            {
                if (SpellInfo const* l_InfectedClaws = sSpellMgr->GetSpellInfo(eSpellIds::InfectedClaws))
                    if (roll_chance_i(l_InfectedClaws->Effects[EFFECT_0].BasePoints))
                        m_Cast = true;
            }
        }

        void Register() override
        {
            BeforeCast += SpellCastFn(spell_dk_infected_claws_SpellScript::HandleBeforeCast);
            OnEffectHitTarget += SpellEffectFn(spell_dk_infected_claws_SpellScript::HandleDummy, m_scriptSpellId == 91778 ? EFFECT_1 : EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
        }

    private:
        bool m_Cast = false;
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_infected_claws_SpellScript();
    }
};

/// Called by Claw - 47468, Gnaw - 47481, Huddle - 47484, Leap - 47482; Cleaver - 212333, Smash - 212336, Hook - 212468, Protective Bite - 212384
/// Updated as of 7.0.3 - 22522
class spell_dk_pet_abilites : public SpellScriptLoader
{
    public:
        spell_dk_pet_abilites() : SpellScriptLoader("spell_dk_pet_abilites") { }

        class spell_dk_pet_abilites_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_pet_abilites_SpellScript);

            enum eSpellIds
            {
                /// casted - Ghoul
                Claw                = 47468,
                Gnaw                = 47481,
                Huddle              = 47484,
                Leap                = 47482,

                /// casted - Abomination
                Cleaver             = 212333,
                Smash               = 212336,
                Hook                = 212468,
                ProtectiveBile      = 212384,

                /// regular with effects - Ghoul
                RealClaw            = 91776,
                RealGnaw            = 91800,
                RealHuddle          = 91838,
                RealLeap            = 91809,

                /// regular with effects - Abomination
                RealCleaver         = 212335,
                RealSmash           = 212332,
                RealHook            = 212469,
                RealProtectiveBile  = 212385,

                /// Dark transformation - Ghoul
                SweepingClaws       = 91778,
                MonstrousBlow       = 91797,
                PutridBulwark       = 91378,
                ShamblingRush       = 91802,

                /// Dark transformation - Abomination
                VileGas             = 212338,
                GastricBoat         = 212383,
                PowerfulSmash       = 212337,
                FleshHook           = 212470,

                DarkTransformation  = 63560,
                SledgeButcher       = 207313,
                BlackClaws          = 238116,
                FesteringWound      = 194310,
                FesteringWoundDamage = 194311
            };

            struct s_OverriderData
            {
                uint32 m_Regular;
                uint32 m_DarkTrans;
            };

            std::map<uint32, s_OverriderData> m_OverriderEntries =
            {
                { eSpellIds::Claw,              { eSpellIds::RealClaw,              eSpellIds::SweepingClaws    } },
                { eSpellIds::Gnaw,              { eSpellIds::RealGnaw,              eSpellIds::MonstrousBlow    } },
                { eSpellIds::Huddle,            { eSpellIds::RealHuddle,            eSpellIds::PutridBulwark    } },
                { eSpellIds::Leap,              { eSpellIds::RealLeap,              eSpellIds::ShamblingRush    } },
                { eSpellIds::Cleaver,           { eSpellIds::RealCleaver,           eSpellIds::VileGas          } },
                { eSpellIds::Smash,             { eSpellIds::RealSmash,             eSpellIds::PowerfulSmash    } },
                { eSpellIds::Hook,              { eSpellIds::RealHook,              eSpellIds::FleshHook        } },
                { eSpellIds::ProtectiveBile,    { eSpellIds::RealProtectiveBile,    eSpellIds::GastricBoat      } }
            };

            SpellCastResult CheckLeapCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->GetExactDist2d(l_Target) > 5.0f && !l_Caster->HasAura(eSpellIds::DarkTransformation)) /// If Player has Dark Transformation, the interrupt must still go through even if the pet is less than 5yds away from target.
                    return SPELL_FAILED_OUT_OF_RANGE;                                                               /// I added minRange of 0 for the spell, but check here if player has dark transfo, otherwise spell shouldn't be casted if dist < 5 yards.

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_Caster->ToPet())
                    return;

                Unit* l_Owner = l_Caster->ToPet()->GetOwner();
                if (!l_Owner)
                    return;

                /// replace spells
                uint32 l_SelectedSpellId = l_SpellInfo->Id;
                if (l_Caster->HasAura(eSpellIds::DarkTransformation))
                {
                    if (m_OverriderEntries.find(l_SelectedSpellId) != m_OverriderEntries.end())
                        l_SelectedSpellId = m_OverriderEntries[l_SelectedSpellId].m_DarkTrans;
                }
                else ///< regular spell
                {
                    if (m_OverriderEntries.find(l_SelectedSpellId) != m_OverriderEntries.end())
                        l_SelectedSpellId = m_OverriderEntries[l_SelectedSpellId].m_Regular;
                }

                l_Caster->CastSpell(l_Target, l_SelectedSpellId, true);

                if ((l_SelectedSpellId == eSpellIds::SweepingClaws || l_SelectedSpellId == eSpellIds::VileGas)
                    && l_Owner->HasAura(eSpellIds::BlackClaws) && roll_chance_i(50))
                {
                    std::set<uint64>& l_FesteringWoundTargets = l_Owner->m_SpellHelper.GetUint64Set()[eSpellHelpers::FesteringWound];
                    if (l_FesteringWoundTargets.empty())
                        return;

                    std::set<uint64>::const_iterator it(l_FesteringWoundTargets.begin());
                    std::advance(it, urand(0, l_FesteringWoundTargets.size() - 1));
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Owner, *it);
                    if (!l_Target)
                        return;

                    Aura* l_FesteringWoundAura = l_Target->GetAura(eSpellIds::FesteringWound);
                    if (!l_FesteringWoundAura)
                        return;

                    if (l_FesteringWoundAura->GetStackAmount() > 0)
                    {
                        l_Owner->CastSpell(l_Target, eSpellIds::FesteringWoundDamage, true);
                        l_FesteringWoundAura->ModStackAmount(-1);
                    }
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpellIds::Leap)
                    OnCheckCast += SpellCheckCastFn(spell_dk_pet_abilites_SpellScript::CheckLeapCast);

                OnEffectHitTarget += SpellEffectFn(spell_dk_pet_abilites_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_pet_abilites_SpellScript();
        }
};

/// Hook - 212469, Flesh Hook - 212470
/// Called by same
/// Updated as of 7.0.3 - 22522
class spell_dk_pet_hook : public SpellScriptLoader
{
    public:
        spell_dk_pet_hook() : SpellScriptLoader("spell_dk_pet_hook") { }

        class spell_dk_pet_hook_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_pet_hook_SpellScript);

            enum eSpellIds
            {
                FleshHookRoot       = 212540,
                FleshHookMissile    = 212528
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Caster->GetExactDist2d(l_Target) > 30.0f)
                    return SPELL_FAILED_OUT_OF_RANGE;

                return SPELL_CAST_OK;
            }

            void HandleEffectHit(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_SpellMissileInfo = sSpellMgr->GetSpellInfo(l_SpellInfo->Effects[EFFECT_0].TriggerSpell);
                if (!l_Caster || !l_SpellInfo || !l_SpellMissileInfo)
                    return;

                if (Unit* l_Target = GetHitUnit())
                {
                    if (l_Target->IsImmunedToSpell(l_SpellMissileInfo))
                        l_Caster->JumpTo(l_Target, 5.0f);
                    else
                        l_Target->JumpTo(l_Caster, 5.0f);

                    if (l_SpellMissileInfo->Id == eSpellIds::FleshHookMissile)
                        l_Caster->CastSpell(l_Target, eSpellIds::FleshHookRoot, true);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_pet_hook_SpellScript::HandleCheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_dk_pet_hook_SpellScript::HandleEffectHit, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_pet_hook_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Zombie Explosion - 210141
class spell_dk_reanimation_explosion : public SpellScriptLoader
{
    public:
        spell_dk_reanimation_explosion() : SpellScriptLoader("spell_dk_reanimation_explosion") { }

        class spell_dk_reanimation_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_reanimation_explosion_SpellScript);

            void HandleEffectHit(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                bool l_HasDespawned = false;

                if (Unit* l_Target = GetHitUnit())
                    SetHitDamage(CalculatePct(l_Target->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_0].BasePoints));

                if (!l_HasDespawned)
                {
                    l_Caster->AddDelayedEvent([l_Caster]() -> void
                    {
                        if (l_Caster->ToCreature())
                        {
                            l_Caster->Kill(l_Caster);
                            l_Caster->ToCreature()->DespawnOrUnsummon(0);
                        }
                    },1);

                    l_HasDespawned = true;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_reanimation_explosion_SpellScript::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_reanimation_explosion_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Unending thirst - 192567 - Blood Shield - 77535
/// Called for Unending thirst - 192567
class spell_dk_unending_thirst_trigger : public SpellScriptLoader
{
    public:
        spell_dk_unending_thirst_trigger() : SpellScriptLoader("spell_dk_unending_thirst_trigger") { }

        class spell_dk_unending_thirst_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_unending_thirst_trigger_AuraScript);

            enum eSpells
            {
                UnendingThirst          = 192567,
                UnendingThristEffect    = 216019,
                BloodShield             = 77535
            };

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::BloodShield) || !l_Caster->HasAura(eSpells::UnendingThirst))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::UnendingThristEffect, true);
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::UnendingThristEffect, l_Caster->GetGUID());
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::BloodShield:
                        /// Blood Shield - 77535
                        AfterEffectApply += AuraEffectApplyFn(spell_dk_unending_thirst_trigger_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_dk_unending_thirst_trigger_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                        break;

                    case eSpells::UnendingThirst:
                        /// For Unending Thirst - 192567
                        AfterEffectApply += AuraEffectApplyFn(spell_dk_unending_thirst_trigger_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_dk_unending_thirst_trigger_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_unending_thirst_trigger_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Unending Thirst - 216019
class spell_dk_unending_thirst : public SpellScriptLoader
{
    public:
        spell_dk_unending_thirst() : SpellScriptLoader("spell_dk_unending_thirst") { }

        class spell_dk_unending_thirst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_unending_thirst_AuraScript);

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amout, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                p_Amout = p_AuraEffect->GetBaseAmount() * 0.60f; ///< Unending thirst effect is reduced by 40% in PvP.
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_unending_thirst_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_LEECH_PCT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_unending_thirst_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_unending_thirst_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Blood Feast - 192548
class spell_dk_blood_feast : public SpellScriptLoader
{
    public:
        spell_dk_blood_feast() : SpellScriptLoader("spell_dk_blood_feast") { }

        class spell_dk_blood_feast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_blood_feast_AuraScript);

            enum eSpells
            {
                HeartStrike = 206930
            };

            void HandleHeal(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_DamageInfo || !l_SpellInfo || !l_Caster)
                    return;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                uint32 l_HealPct = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (!l_ProcSpellInfo || !l_HealPct)
                    return;

                if (l_ProcSpellInfo->Id != eSpells::HeartStrike)
                    return;

                uint32 l_HitDamage = l_DamageInfo->GetAmount();
                if (!l_HitDamage)
                    return;

                int32 l_HealAmount = CalculatePct(l_HitDamage, l_HealPct);

                l_Caster->HealBySpell(l_Caster, l_SpellInfo, l_HealAmount);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dk_blood_feast_AuraScript::HandleHeal);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_blood_feast_AuraScript();
        }
};

/// last update : 7.0.3
/// Frost Strike - 222026 - Frost Strike - 66196
/// Called for Shattering Strikes - 207057
class spell_dk_shaterring_strikes : public SpellScriptLoader
{
    public:
        spell_dk_shaterring_strikes() : SpellScriptLoader("spell_dk_shaterring_strikes") { }

        class spell_dk_shaterring_strikes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_shaterring_strikes_SpellScript);

            enum eSpell
            {
                ShatteringStrikes   = 207057,
                Razorice            = 51714
            };

            void HandleStackRemove()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::ShatteringStrikes);

                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                Aura* l_Aura = l_Target->GetAura(eSpell::Razorice, l_Caster->GetGUID());

                if (!l_Aura || !l_Caster->HasAura(eSpell::ShatteringStrikes))
                    return;

                uint8 l_StackAmount = l_Aura->GetStackAmount();

                if (l_StackAmount != 5)
                    return;

                int32 l_AmountPCT = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                int32 l_Amount = GetHitDamage();
                l_Amount = AddPct(l_Amount, l_AmountPCT);

                SetHitDamage(l_Amount);

                if (!l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ShatteringStrikesRemoving))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ShatteringStrikesRemoving) = true;
                    uint64 l_TargetGUID = l_Target->GetGUID();
                    l_Caster->AddDelayedEvent([l_Caster, l_TargetGUID]() -> void
                    {
                        if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGUID))
                            l_Target->RemoveAurasDueToSpell(eSpell::Razorice, l_Caster->GetGUID());
                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ShatteringStrikesRemoving) = false;
                    }, 100);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dk_shaterring_strikes_SpellScript::HandleStackRemove);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_shaterring_strikes_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Frozen Pulse - 194909
class spell_dk_frozen_pulse : public SpellScriptLoader
{
public:
    spell_dk_frozen_pulse() : SpellScriptLoader("spell_dk_frozen_pulse") { }

    class spell_dk_frozen_pulse_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_frozen_pulse_AuraScript);

        void HandleProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            Player* l_Player = l_Caster->ToPlayer();
            AuraEffect* l_AuraEffect = GetEffect(EFFECT_1);

            if (!l_Caster || !l_Player || !l_AuraEffect)
                return;

            uint8 l_Runes = 0;
            for (uint8 i = 0; i < MAX_RUNES; ++i)
            {
                if (!l_Player->GetRuneCooldown(i))
                    l_Runes++;
            }

            if (l_Runes >= l_AuraEffect->GetAmount())
                PreventDefaultAction();
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dk_frozen_pulse_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_frozen_pulse_AuraScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Called by Death Grip - 49576
/// Called for Walking Dead - 202731
class spell_dk_walking_dead : public SpellScriptLoader
{
    public:
        spell_dk_walking_dead() : SpellScriptLoader("spell_dk_walking_dead") { }

        class spell_dk_walking_dead_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_walking_dead_SpellScript);

            enum eSpells
            {
                WalkingDead         = 202731,
                WalkingDeadEffect   = 212610
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::WalkingDead))
                    l_Caster->CastSpell(l_Target, eSpells::WalkingDeadEffect, true);
            }

            void Register()
            {
                OnCast += SpellCastFn(spell_dk_walking_dead_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_walking_dead_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Chains of Ice - 45524
/// Called for Deathchill - 204080
class spell_dk_deathchill : public SpellScriptLoader
{
    public:
        spell_dk_deathchill() : SpellScriptLoader("spell_dk_deathchill") { }

        class spell_dk_deathchill_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_deathchill_SpellScript);

            enum eSpells
            {
                Deathchill          = 204080,
                DeathchillEffect    = 204085,
                ChainsOfIce         = 45524
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::Deathchill) && l_Target->HasAura(eSpells::ChainsOfIce))
                    l_Caster->CastSpell(l_Target, eSpells::DeathchillEffect, true);
            }

            void Register()
            {
                OnCast += SpellCastFn(spell_dk_deathchill_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_deathchill_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Outbreak - 77575
/// Called for Wandering Plague - 199725
class spell_dk_wandering_plague : public SpellScriptLoader
{
    public:
        spell_dk_wandering_plague() : SpellScriptLoader("spell_dk_wandering_plague") { }

        class spell_dk_wandering_plague_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_wandering_plague_SpellScript);

            enum eSpells
            {
                WanderingPlague             = 199725,
                WanderingPlagueVisualMissle = 199986,
                WanderingPlagueDebuff       = 199969,
                VirulentPlague              = 191587
            };

            void HandleOnHit(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::WanderingPlague))
                {
                    l_Caster->CastSpell(l_Target, eSpells::WanderingPlagueVisualMissle, true);
                    if (Aura* l_Wandering = l_Caster->AddAura(eSpells::WanderingPlagueDebuff, l_Target))
                        l_Wandering->SetStackAmount(2);

                    l_Caster->CastSpell(l_Target, eSpells::VirulentPlague, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_wandering_plague_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_wandering_plague_SpellScript();
        };
};

/// Wandering Plague - 200093
/// Updated as of 7.0.3 - 22522
class spell_dk_wandering_plague_jump : public SpellScriptLoader
{
    public:
        spell_dk_wandering_plague_jump() : SpellScriptLoader("spell_dk_wandering_plague_jump") { }

        class spell_dk_wandering_plague_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_wandering_plague_jump_SpellScript);

            enum eSpells
            {
                WanderingPlagueVisualMissle = 199986,
                WanderingPlagueDebuff       = 199969,
                VirulentPlague              = 191587
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo || p_Targets.empty())
                    return;

                /// remove targets without disease from us
                p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                {
                    if (Unit* l_Target = p_Target->ToUnit())
                    {
                        DispelChargesList l_Diseases;
                        l_Target->GetDispellableAuraList(l_Target, DispelType::DISPEL_DISEASE, l_Diseases);
                        for (auto l_Itr : l_Diseases)
                        {
                            if (Aura* l_Aura = l_Itr.first)
                            {
                                /// if target has disease from caster
                                if (l_Aura->GetCasterGUID() == l_Caster->GetGUID())
                                    return false;
                            }
                        }
                    }

                    return true;
                });

                if (!p_Targets.empty())
                    p_Targets.resize(l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void HandleOnHit(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::WanderingPlagueVisualMissle, true);
                l_Caster->CastSpell(l_Target, eSpells::WanderingPlagueDebuff, true);
                l_Caster->CastSpell(l_Target, eSpells::VirulentPlague, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_wandering_plague_jump_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_dk_wandering_plague_jump_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_wandering_plague_jump_SpellScript();
        };
};

/// Wandering Plague - 199969
/// Updated as of 7.0.3 - 22522
class spell_dk_wandering_plague_debuff : public SpellScriptLoader
{
    public:
        spell_dk_wandering_plague_debuff() : SpellScriptLoader("spell_dk_wandering_plague_debuff") { }

        class spell_dk_wandering_plague_debuff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_wandering_plague_debuff_AuraScript);

            enum eSpells
            {
                WanderingPlagueJump         = 200093
            };

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                Aura* l_Aura = p_AurEff->GetBase();
                if (!l_Caster || !l_Target || !l_Aura)
                    return;

                AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                if ((removeMode == AURA_REMOVE_BY_EXPIRE || removeMode == AURA_REMOVE_BY_ENEMY_SPELL) && l_Aura->GetStackAmount() > 1)
                    l_Caster->CastSpell(l_Target, eSpells::WanderingPlagueJump, true); ///< cast new target picker
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_wandering_plague_debuff_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_wandering_plague_debuff_AuraScript();
        }
};

/// Called by Festering Wound - 194310
/// Called for Crypt Fever - 199722
/// Updated as of 7.0.3 - 22522
class spell_dk_crypt_fever : public SpellScriptLoader
{
    public:
        spell_dk_crypt_fever() : SpellScriptLoader("spell_dk_crypt_fever") { }

        class spell_dk_crypt_fever_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_crypt_fever_AuraScript);

            enum eSpells
            {
                CryptFever      = 199722,
                CryptFeverProc  = 199723
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::CryptFever))
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::CryptFever) = time(nullptr);

                l_Caster->CastSpell(l_Target, eSpells::CryptFeverProc, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dk_crypt_fever_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_crypt_fever_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Dark Command - 56222
/// Called for Murderous Intend - 207018
class spell_dk_murderous_intend : public SpellScriptLoader
{
    public:
        spell_dk_murderous_intend() : SpellScriptLoader("spell_dk_murderous_intend") { }

        class spell_dk_murderous_intend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_murderous_intend_SpellScript);

            enum eSpells
            {
                MurderousIntend = 207018,
                Intimidated     = 206891,
                DarkCommand     = 56222
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::MurderousIntend))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Intimidated, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dk_murderous_intend_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_murderous_intend_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Necrotic Wound - 223929
/// Called for Necrotic Strike - 223829
class spell_dk_necrotic_strike : public SpellScriptLoader
{
    public:
        spell_dk_necrotic_strike() : SpellScriptLoader("spell_dk_necrotic_strike") { }

        class spell_dk_necrotic_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_necrotic_strike_SpellScript);

            enum eSpells
            {
                FesteringWound  = 194310,
                SoulReaperAura  = 130736,
                SoulReaperSpeed = 215711,
                NectroticStrike = 223829
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (AuraEffect const* l_aurNecroticWoundEffect = l_Target->GetAuraEffect(m_scriptSpellId, EFFECT_0, l_Caster->GetGUID()))
                    m_PrevNecroticWoundAmount = l_aurNecroticWoundEffect->GetAmount();
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::NectroticStrike);
                if (!l_Target || !l_Caster || !l_SpellInfo)
                    return;

                Aura* l_NecroticWound = l_Target->GetAura(m_scriptSpellId, l_Caster->GetGUID());
                Aura* l_FesteringWound = l_Target->GetAura(eSpells::FesteringWound, l_Caster->GetGUID());
                if (!l_NecroticWound || !l_FesteringWound)
                    return;

                l_FesteringWound->DropStack();

                UnholyFrenzyUpdater l_UnholyFrenzyUpdater(l_Caster);
                l_UnholyFrenzyUpdater.Update(1);

                if (l_Target->HasAura(eSpells::SoulReaperAura))
                    l_Caster->CastSpell(l_Caster, eSpells::SoulReaperSpeed, true);

                AuraEffect* l_NecroticWoundEffect = l_NecroticWound->GetEffect(EFFECT_0);
                if (!l_NecroticWoundEffect)
                    return;

                int32 l_Amount = (float(l_SpellInfo->Effects[EFFECT_0].BasePoints) / 100.0f) * float(l_Caster->GetUInt32Value(UNIT_FIELD_ATTACK_POWER));
                l_NecroticWoundEffect->SetAmount(l_Amount + m_PrevNecroticWoundAmount);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_dk_necrotic_strike_SpellScript::HandleBeforeCast);
                AfterHit += SpellHitFn(spell_dk_necrotic_strike_SpellScript::HandleAfterHit);
            }

        private:

            int32 m_PrevNecroticWoundAmount = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_necrotic_strike_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Chill Streak - 204160
class spell_dk_chill_streak : public SpellScriptLoader
{
    public:
        spell_dk_chill_streak() : SpellScriptLoader("spell_dk_chill_streak") { }

        class spell_dk_chill_streak_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_chill_streak_SpellScript);

            enum eSpells
            {
                ChillStreakVisual = 204199,
                ChillStreakDamages = 204167
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                uint32& l_BouncesAmount = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ChillStreakTargetsAmount);
                l_BouncesAmount = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                l_Caster->CastSpell(l_Target, eSpells::ChillStreakDamages, true);
                l_Caster->CastSpell(l_Target, eSpells::ChillStreakVisual, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dk_chill_streak_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_chill_streak_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Chill Streak (Visual) - 204199
class spell_dk_chill_streak_visual : public SpellScriptLoader
{
    public:
        spell_dk_chill_streak_visual() : SpellScriptLoader("spell_dk_chill_streak_visual") { }

        class spell_dk_chill_streak_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_chill_streak_visual_SpellScript);

            enum eSpells
            {
                ChillStreakSlow     = 204206,
                ChillStreakDamages  = 204167,
                ChillStreakRadius   = 204165
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetOriginalCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_RadiusSpellInfo = sSpellMgr->GetSpellInfo(eSpells::ChillStreakRadius);
                if (!l_Caster || !l_Target || !l_RadiusSpellInfo)
                    return;

                uint32& l_BouncesAmount = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ChillStreakTargetsAmount);
                l_BouncesAmount--;
                if (!l_BouncesAmount)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ChillStreakSlow, true);

                SpellRadiusEntry const* l_RadiusEntry = l_RadiusSpellInfo->Effects[EFFECT_0].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                float l_Radius = l_RadiusEntry->RadiusMax;
                std::list<Unit*> l_TargetList;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Ucheck(l_Target, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Target, l_TargetList, l_Ucheck);
                l_Target->VisitNearbyObject(l_Radius, l_Searcher);

                if (l_TargetList.empty())
                    return;

                l_TargetList.remove_if([=](Unit* l_TargetInList) -> bool
                {
                    return (l_TargetInList == l_Target);
                });
                l_TargetList.sort(JadeCore::WorldObjectDistanceCompareOrderPred(l_Target));

                if (l_TargetList.empty())
                    return;

                Unit* l_NewTarget = l_TargetList.front();
                l_Target->CastSpell(l_NewTarget, m_scriptSpellId, true, nullptr, nullptr, l_Caster->GetGUID());
                l_Caster->CastSpell(l_NewTarget, eSpells::ChillStreakDamages, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_dk_chill_streak_visual_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_chill_streak_visual_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Chill Streak (Damages) - 204167
class spell_dk_chill_streak_damages : public SpellScriptLoader
{
    public:
        spell_dk_chill_streak_damages() : SpellScriptLoader("spell_dk_chill_streak_damages") { }

        class spell_dk_chill_streak_damages_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_chill_streak_damages_SpellScript);

            void HandleDamages(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Target = GetHitUnit();
                if (!l_SpellInfo || !l_Target)
                    return;

                int32 l_Damages = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                l_Damages = CalculatePct(l_Target->GetMaxHealth(), l_Damages);
                SetHitDamage(l_Damages);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_chill_streak_damages_SpellScript::HandleDamages, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_chill_streak_damages_SpellScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Cadaverous Pallor - 201995
class spell_dk_cadaverous_pallor : public SpellScriptLoader
{
    public:
        spell_dk_cadaverous_pallor() : SpellScriptLoader("spell_dk_cadaverous_pallor") { }

        class spell_dk_cadaverous_pallor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_cadaverous_pallor_AuraScript);

            enum eSpells
            {
                CadaverousPallorAura    = 201995,
                CadaverousPallorDisease = 213726
            };

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = p_DmgInfo.GetTarget();

                if (l_Target == nullptr)
                    return;

                if (p_DmgInfo.GetSchoolMask() & SPELL_SCHOOL_MASK_MAGIC)
                {
                    uint32 l_PercentProc = GetSpellInfo()->Effects[EFFECT_1].BasePoints;
                    uint32 l_Rand        = urand(0, 100);
                    int32 l_Bp           = p_DmgInfo.GetAmount() / 6;

                    if (l_Rand <= l_PercentProc)
                    {
                        p_AbsorbAmount = p_DmgInfo.GetAmount();
                        l_Target->CastCustomSpell(l_Target, eSpells::CadaverousPallorDisease, &l_Bp, NULL, NULL, true);
                    }
                    else
                        p_AbsorbAmount = 0;
                }
                else
                    p_AbsorbAmount = 0;
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1; ///< Initialize to infinite Absorb
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_cadaverous_pallor_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_cadaverous_pallor_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_cadaverous_pallor_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Death Grip - 49576
/// Called for Gravitational Pull - 191721
class spell_dk_gravitation_pull : public SpellScriptLoader
{
    public:
        spell_dk_gravitation_pull() : SpellScriptLoader("spell_dk_gravitation_pull") { }

        class spell_dk_gravitation_pull_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_gravitation_pull_SpellScript);

            enum eSpells
            {
                GravitationalPull       = 191721,
                GravitationalPullEffect = 191719
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::GravitationalPull))
                    return;

                if (!l_Target->ToCreature() || !l_Target->ToCreature()->IsDungeonBoss())
                    l_Caster->CastSpell(l_Target, eSpells::GravitationalPullEffect, true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_dk_gravitation_pull_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_gravitation_pull_SpellScript();
        };
};

/// Last Update 7.3.5 Build 26365
/// Called by Gravitational Pull - 191721
class spell_dk_gravitation_pull_aura : public SpellScriptLoader
{
    public:
        spell_dk_gravitation_pull_aura() : SpellScriptLoader("spell_dk_gravitation_pull_aura") { }

        class spell_dk_gravitation_pull_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_gravitation_pull_aura_AuraScript);

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                PreventDefaultAction();
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_gravitation_pull_aura_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_gravitation_pull_aura_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Blight - 203157
class spell_dk_blight : public SpellScriptLoader
{
public:
    spell_dk_blight() : SpellScriptLoader("spell_dk_blight") { }

    class spell_dk_blight_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_blight_SpellScript);

        enum eSpells
        {
            DeathAndDecay   = 203128
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_SpellInfo)
                return;

            SpellRadiusEntry const* l_RadiusEntry = l_SpellInfo->Effects[EFFECT_0].RadiusEntry;
            uint32 l_TargetNumber = l_SpellInfo->MaxAffectedTargets;
            if (!l_RadiusEntry || !l_TargetNumber)
                return;

            float l_Radius = l_RadiusEntry->RadiusMax;
            std::list<Unit*> l_TargetList;

            JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Ucheck(l_Caster, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Ucheck);
            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);
            if (l_TargetList.empty())
                return;

            l_TargetList.remove_if([=](Unit* l_TargetInList) -> bool
            {
                return (!l_TargetInList || l_TargetInList == l_Caster);
            });
            l_TargetList.sort(JadeCore::WorldObjectDistanceCompareOrderPred(l_Caster));
            for (auto itr : l_TargetList)
            {
                if (!l_TargetNumber)
                    return;
                l_Caster->CastSpell(itr, eSpells::DeathAndDecay, true);
                l_TargetNumber--;
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_dk_blight_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_blight_SpellScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Called by Frost Strike - 222026 - Frost Strike (left hand) - 66196
/// Called for Tundra Stalker - 204132
class spell_dk_tundra_stalker : public SpellScriptLoader
{
    public:
        spell_dk_tundra_stalker() : SpellScriptLoader("spell_dk_tundra_stalker") { }

        class spell_dk_tundra_stalker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_tundra_stalker_SpellScript);

            enum eSpells
            {
                TundraStalker = 204132
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                int32 l_AmountPct = (sSpellMgr->GetSpellInfo(eSpells::TundraStalker))->Effects[EFFECT_0].BasePoints;
                int32 l_Amount = GetHitDamage();
                if (!l_Caster->HasAura(eSpells::TundraStalker) || !l_Target->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || !l_Amount || !l_AmountPct)
                    return;

                l_Amount = AddPct(l_Amount, l_AmountPct);
                SetHitDamage(l_Amount);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_dk_tundra_stalker_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_tundra_stalker_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Last Dance - 233412
class spell_dk_last_dance : public SpellScriptLoader
{
public:
    spell_dk_last_dance() : SpellScriptLoader("spell_dk_last_dance") { }

    class spell_dk_last_dance_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_last_dance_AuraScript);

        enum eSpells
        {
            LastDance       = 233412,
            DanceRuneWeapon = 49028
        };

        void HandleProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_LastDance = sSpellMgr->GetSpellInfo(eSpells::LastDance);
            SpellInfo const* l_DanceRuneWeapon = sSpellMgr->GetSpellInfo(eSpells::DanceRuneWeapon);
            if (!l_Caster || !l_LastDance || !l_DanceRuneWeapon)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            SpellCooldownsEntry const* l_Cooldown = l_DanceRuneWeapon->GetSpellCooldowns();
            if (!l_Player || !l_Cooldown)
                return;

            l_Player->ModifySpellCooldown(eSpells::DanceRuneWeapon, CalculatePct(l_Cooldown->RecoveryTime, l_LastDance->Effects[EFFECT_0].BasePoints));
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dk_last_dance_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_last_dance_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called for Delirium - 233396
class spell_dk_delirium : public SpellScriptLoader
{
public:
    spell_dk_delirium() : SpellScriptLoader("spell_dk_delirium") { }

    class spell_dk_delirium_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_delirium_AuraScript);

        enum eSpells
        {
            FrostStrike     = 49143,
            HowlingBlast    = 49184,
            Delirium        = 233397
        };

        void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
        {
            PreventDefaultAction();

            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_EventInfo.GetActionTarget();
            if (!l_Caster || !l_Target || l_Caster == l_Target || !p_EventInfo.GetDamageInfo())
                return;

            SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
            if (!l_SpellInfo)
                return;

            if (l_SpellInfo->Id == eSpells::FrostStrike || l_SpellInfo->Id == eSpells::HowlingBlast)
                l_Caster->CastSpell(l_Target, eSpells::Delirium, true);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dk_delirium_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_delirium_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Death and Decay - 43265
class spell_dk_death_and_decay : public SpellScriptLoader
{
public:
    spell_dk_death_and_decay() : SpellScriptLoader("spell_dk_death_and_decay") { }

    class spell_dk_death_and_decay_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_death_and_decay_SpellScript);

        enum eSpells
        {
            TighteningGrasp     = 143375,
            TighteningGraspAura = 206970,
            CrimsonScourgeAura  = 81141,
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            WorldLocation const* l_TargetDest = GetExplTargetDest();
            if (!l_Caster || !l_TargetDest)
                return;

            if (l_Caster->HasAura(eSpells::TighteningGraspAura))
                l_Caster->CastSpell(l_TargetDest->GetPositionX(), l_TargetDest->GetPositionY(), l_TargetDest->GetPositionZ(), eSpells::TighteningGrasp, true);

            l_Caster->RemoveAura(eSpells::CrimsonScourgeAura);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_dk_death_and_decay_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_death_and_decay_SpellScript();
    }

    class spell_dk_death_and_decay_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_death_and_decay_AuraScript);

        enum eSpells
        {
            DeathAndDecay               = 43265,
            RapidDecompositionEnergize  = 255203,
            RapidDecomposition          = 194662
        };

        void OnTick(AuraEffect const* /*aurEff*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::RapidDecomposition))
                return;

            if (AreaTrigger* l_AreaTrigger = l_Caster->GetAreaTrigger(eSpells::DeathAndDecay))
                if (l_AreaTrigger->GetDistance(l_Caster) <= l_AreaTrigger->GetRadius())
                    l_Caster->CastSpell(l_Caster, eSpells::RapidDecompositionEnergize, true);
        }

        void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_RDSpellInfo = sSpellMgr->GetSpellInfo(eSpells::RapidDecomposition);
            if (!l_Caster || !l_RDSpellInfo)
                return;

            if (!l_Caster->HasAura(eSpells::RapidDecomposition))
                return;

            AuraEffect* l_AuraEffect = GetAura()->GetEffect(EFFECT_1);
            if (!l_AuraEffect)
                return;

            l_AuraEffect->SetPeriodicTimer(l_AuraEffect->GetPeriodicTimer() - CalculatePct(l_AuraEffect->GetPeriodicTimer(), l_RDSpellInfo->Effects[EFFECT_1].BasePoints));
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_death_and_decay_AuraScript::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
			AfterEffectApply += AuraEffectApplyFn(spell_dk_death_and_decay_AuraScript::HandleAfterApply, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_death_and_decay_AuraScript();
    }
};

/// Last Update 7.3.5 - Build 26365
/// Lanathiel's lamentation - 212975 / Item - 133974
class Playerscript_dk_lanathiels_lamentation : public PlayerScript
{
    public:
        Playerscript_dk_lanathiels_lamentation() : PlayerScript("Playerscript_dk_lanathiels_lamentation") { }

        enum eItems
        {
            LanathielsLamentation = 133974
        };

        enum eSpells
        {
            LanathielsLamentationSpell = 212975
        };

        void OnAfterUnequipItem(Player* p_Player, Item const* p_Item) override
        {
            if (!p_Player || !p_Item || p_Item->GetEntry() != eItems::LanathielsLamentation)
                return;

            p_Player->RemoveAura(eSpells::LanathielsLamentationSpell);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for The Shambler - 191760
class spell_dk_the_shambler : public SpellScriptLoader
{
    public:
        spell_dk_the_shambler() : SpellScriptLoader("spell_dk_the_shambler") { }

        class spell_dk_the_shambler_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_the_shambler_AuraScript);

            enum eSpells
            {
                SummonShamblingHorror = 191759,
            };

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonShamblingHorror, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dk_the_shambler_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_the_shambler_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Hypothermia - 189185
class spell_dk_hypothermia : public SpellScriptLoader
{
    public:
        spell_dk_hypothermia() : SpellScriptLoader("spell_dk_hypothermia") { }

        class spell_dk_hypothermia_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_hypothermia_AuraScript);

            enum eSpells
            {
                HypothermiaDamages = 228322,
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::HypothermiaDamages, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dk_hypothermia_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_hypothermia_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Scourge of Worlds - 191747
class spell_dk_scourge_of_worlds : public SpellScriptLoader
{
    public:
        spell_dk_scourge_of_worlds() : SpellScriptLoader("spell_dk_scourge_of_worlds") { }

        class spell_dk_scourge_of_worlds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_scourge_of_worlds_AuraScript);

            enum eSpells
            {
                DeathCoil = 47541,
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::DeathCoil)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_scourge_of_worlds_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_scourge_of_worlds_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23240
/// Called by Frozen Soul - 204959
class spell_dk_frozen_soul : public SpellScriptLoader
{
    public:
        spell_dk_frozen_soul() : SpellScriptLoader("spell_dk_frozen_soul") { }

        class spell_dk_frozen_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_frozen_soul_SpellScript);

            void HandleOnHitTarget(SpellEffIndex /*p_effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SetHitDamage(GetHitDamage() * (1 + l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::FrozenSoulTargets].size()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_frozen_soul_SpellScript::HandleOnHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_frozen_soul_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Umbilicus Eternus - 193249
class spell_dk_umbilicus_eternus : public SpellScriptLoader
{
    public:
        spell_dk_umbilicus_eternus() : SpellScriptLoader("spell_dk_umbilicus_eternus") { }

        class spell_dk_umbilicus_eternus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_umbilicus_eternus_AuraScript);

            enum eSpells
            {
                BloodPlague             = 55078,
                UmbilicusEternusAbsorb  = 193320,
                UmbilicusEternus        = 193213
            };

            void HandleOnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::BloodPlague)
                    return;

                Aura* l_Aura = p_AurEff->GetBase();
                if (!l_Aura)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                uint32 l_absorb = std::max((uint32)l_AuraEffect->GetAmount(), (uint32)l_AuraEffect->GetAmount() + l_DamageInfo->GetAmount());
                l_AuraEffect->SetAmount(l_absorb);
            }

            void HandleOnRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::UmbilicusEternus);
                if (!l_Caster || !l_SpellInfo)
                    return;

                uint64 l_Absorb = p_AurEff->GetAmount() * l_SpellInfo->Effects[EFFECT_0].BasePoints;
                l_Caster->CastCustomSpell(eSpells::UmbilicusEternusAbsorb, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Absorb, l_Caster, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_dk_umbilicus_eternus_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_umbilicus_eternus_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_umbilicus_eternus_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Killing Obliteration - 207256
class spell_dk_obliteration : public SpellScriptLoader
{
    public:
        spell_dk_obliteration() : SpellScriptLoader("spell_dk_obliteration") { }

        class spell_dk_obliteration_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_obliteration_AuraScript);

            enum eSpells
            {
                FrostStrike = 222026,
                HowlingBlast = 49184
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::FrostStrike && l_SpellInfo->Id != eSpells::HowlingBlast))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_obliteration_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_obliteration_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Calle by Unholy mutation - 201934
class spell_dk_unholy_mutation: public SpellScriptLoader
{
    public:
        spell_dk_unholy_mutation() : SpellScriptLoader("spell_dk_unholy_mutation") { }

        class spell_dk_unholy_mutation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_unholy_mutation_AuraScript);

            enum eSpells
            {
                UnholyMutationAura     = 201934,
                UnholyMutationSlimeNPC = 222393
            };

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::UnholyMutationAura))
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AURA_REMOVE_BY_EXPIRE || l_RemoveMode == AURA_REMOVE_BY_ENEMY_SPELL)
                    l_Caster->CastSpell(l_Target, eSpells::UnholyMutationSlimeNPC, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_unholy_mutation_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_unholy_mutation_AuraScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Called by Remorseless Winter - 196770
class spell_dk_unholy_center : public SpellScriptLoader
{
public:
    spell_dk_unholy_center() : SpellScriptLoader("spell_dk_unholy_center") { }

    class spell_dk_frozen_center_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_frozen_center_SpellScript);

        enum eSpells
        {
            FrozenCenter = 204135,
            FrozenCenterRoot = 233395,
        };

        void HandleOnCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (l_Caster->HasAura(eSpells::FrozenCenter))
                l_Caster->CastSpell(l_Caster, eSpells::FrozenCenterRoot, true);
        }

        void Register()
        {
            OnCast += SpellCastFn(spell_dk_frozen_center_SpellScript::HandleOnCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dk_frozen_center_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Heartstop Aura - 214975 - Necrotic Aura - 214968 - Decomposing Aura - 232352 , 199721
class spell_dk_honor_aura_check : public SpellScriptLoader
{
    public:
        spell_dk_honor_aura_check() : SpellScriptLoader("spell_dk_honor_aura_check") { }

        class spell_dk_honor_aura_check_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_honor_aura_check_AuraScript);

            enum eSpells
            {
                HeartstopAura           = 199719,
                HeartstopAuraDebuff     = 214975,
                DecomposingAura         = 199720,
                DecomposingAuraDebuff   = 232352,
                NecroticAura            = 199642,
                NecroticAuraDebuff      = 214968,
                PvPRulesAura            = 134735,
                DecomposingAuraDebuff2  = 199721
            };

            bool Load() override
            {
                m_AuraSpell = 0;

                switch (m_scriptSpellId)
                {
                    case eSpells::HeartstopAuraDebuff:
                        m_AuraSpell = eSpells::HeartstopAura;
                        break;
                    case eSpells::DecomposingAuraDebuff:
                        m_AuraSpell = eSpells::DecomposingAura;
                        break;
                    case eSpells::NecroticAuraDebuff:
                        m_AuraSpell = eSpells::NecroticAura;
                        break;
                }

                if (m_AuraSpell)
                    if (SpellInfo const* l_spellInfo = sSpellMgr->GetSpellInfo(m_AuraSpell))
                        m_Radius = l_spellInfo->Effects[EFFECT_1].CalcRadius();

                return true;
            }

            void HandleUpdate(uint32 p_Diff)
            {
                if (m_UpdateTimer <= p_Diff)
                {
                    m_UpdateTimer = 1 * IN_MILLISECONDS;

                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetUnitOwner();

                    if (!m_AuraSpell)
                        return;

                    if (!l_Target)
                        return;

                    if (!l_Caster)
                    {
                        Remove();
                        return;
                    }

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player || l_Player->GetMap()->IsRaid() || l_Player->GetMap()->IsDungeon())
                    {
                        Remove();
                        return;
                    }

                    if (!l_Caster->HasAura(m_AuraSpell) || !l_Caster->IsWithinDistInMap(l_Target, m_Radius) || !l_Caster->IsValidAttackTarget(l_Target) || (!l_Player->CanApplyPvPSpellModifiers() && !l_Caster->HasAura(eSpells::PvPRulesAura)))
                    {
                        if (AreaTrigger* l_AreaTrigger = l_Caster->GetAreaTrigger(m_AuraSpell))
                            l_AreaTrigger->GetAffectedPlayers()->erase(l_Target->GetGUID());

                        Remove();
                    }
                }
                else
                {
                    m_UpdateTimer -= p_Diff;
                }
            }

            void HandleUpdateForDecomposing(uint32 p_Diff)
            {
                if (m_UpdateDecomposingTimer <= p_Diff)
                {
                    m_UpdateDecomposingTimer = 2 * IN_MILLISECONDS;

                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetUnitOwner();
                    Aura* l_Aura = GetAura();
                    if (!l_Aura)
                        return;

                    if (!l_Target)
                        return;

                    if (!l_Caster || !l_Caster->HasAura(eSpells::DecomposingAura))
                    {
                        Remove();
                        return;
                    }

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                    {
                        Remove();
                        return;
                    }

                    float l_Radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius();

                    if (!l_Caster->IsWithinDistInMap(l_Target, l_Radius))
                    {
                        std::list<Player*> l_NearDks;
                        l_Target->GetPlayerListInGrid(l_NearDks, l_Radius, false, true);

                        for (Player* dkPlayer : l_NearDks)
                        {
                            if (!dkPlayer)
                                continue;

                            if (dkPlayer->getClass() != CLASS_DEATH_KNIGHT || !dkPlayer->HasAura(eSpells::DecomposingAura) ||
                                !dkPlayer->IsValidAttackTarget(l_Target) || (!dkPlayer->CanApplyPvPSpellModifiers() && !dkPlayer->HasAura(eSpells::PvPRulesAura)))
                                continue;

                            uint32 l_stack = l_Aura->GetStackAmount();
                            l_Aura->Remove();
                            dkPlayer->SetAuraStack(eSpells::DecomposingAuraDebuff2, l_Target, l_stack);
                            return;
                        }

                        if (l_Aura->GetStackAmount() > 1)
                            l_Aura->DropStack();
                        else
                            l_Aura->Remove();
                    }
                }
                else
                {
                    m_UpdateDecomposingTimer -= p_Diff;
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::DecomposingAuraDebuff2)
                    OnAuraUpdate += AuraUpdateFn(spell_dk_honor_aura_check_AuraScript::HandleUpdateForDecomposing);
                else
                    OnAuraUpdate += AuraUpdateFn(spell_dk_honor_aura_check_AuraScript::HandleUpdate);
            }

        private:

            float m_Radius = 8.f;
            uint32 m_AuraSpell = 0;
            uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
            uint32 m_UpdateDecomposingTimer = 2 * IN_MILLISECONDS;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_honor_aura_check_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by all 3 Tier 4 honor talent Auras: Decomposing Aura 199720 / Heartstop Aura 199719 / Necrotic Aura 199642
class spell_dk_honor_auras_removal : public SpellScriptLoader
{
    public:
        spell_dk_honor_auras_removal() : SpellScriptLoader("spell_dk_honor_auras_removal") { }

        class spell_dk_honor_auras_removal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_honor_auras_removal_AuraScript);

            enum eSpells
            {
                HeartstopAura       = 199719,
                NecroticAura        = 199642,
                DecomposingAura     = 199720
            };

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::DecomposingAura:
                        l_Caster->DelayedRemoveAura(eSpells::NecroticAura, 1);
                        l_Caster->DelayedRemoveAura(eSpells::HeartstopAura, 1);
                        break;
                    case eSpells::NecroticAura:
                        l_Caster->DelayedRemoveAura(eSpells::DecomposingAura, 1);
                        l_Caster->DelayedRemoveAura(eSpells::HeartstopAura, 1);
                        break;
                    case eSpells::HeartstopAura:
                        l_Caster->DelayedRemoveAura(eSpells::NecroticAura, 1);
                        l_Caster->DelayedRemoveAura(eSpells::DecomposingAura, 1);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_honor_auras_removal_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_honor_auras_removal_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Frost Breath - 190780
class spell_dk_frost_breath : public SpellScriptLoader
{
    public:
        spell_dk_frost_breath() : SpellScriptLoader("spell_dk_frost_breath") { }

        class spell_dk_frost_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_frost_breath_SpellScript);

            enum eSpells
            {
                FrostBreath         = 235612,
                ConsortsColdCore    = 235605
            };

            void HandleOnHitTarget(SpellEffIndex /*p_effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (l_Caster && l_Caster->HasAura(eSpells::ConsortsColdCore) && l_Target)
                    l_Caster->CastSpell(l_Target, eSpells::FrostBreath, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_frost_breath_SpellScript::HandleOnHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_frost_breath_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Tak'theritrix's Command - 215074
class spell_dk_taktheritrixs_command : public SpellScriptLoader
{
    public:
        spell_dk_taktheritrixs_command() : SpellScriptLoader("spell_dk_taktheritrixs_command") { }

        class spell_dk_taktheritrixs_command_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_taktheritrixs_command_SpellScript);

            enum eNPCs
            {
                DarkArbiter     = 100876,
                ArmyOfTheDead   = 24207
            };

            enum eSpells
            {
                TaktheritrixsCommandBuff = 215069
            };

            void HandleOnHitTarget(SpellEffIndex /*p_effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                switch (l_Target->GetEntry())
                {
                    case eNPCs::DarkArbiter:
                    case eNPCs::ArmyOfTheDead:
                    case DeathKnightPet::Gargoyle:
                        break;
                    default:
                        return;
                }

                l_Caster->CastSpell(l_Target, eSpells::TaktheritrixsCommandBuff, true);
            }

            void Register() override
            {
                    OnEffectHitTarget += SpellEffectFn(spell_dk_taktheritrixs_command_SpellScript::HandleOnHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_taktheritrixs_command_SpellScript();
        }
};

/// Called for Skullflower's Haemostasis - 235558
class spell_dk_skullflowers_haemostasis : public SpellScriptLoader
{
    public:
        spell_dk_skullflowers_haemostasis() : SpellScriptLoader("spell_dk_skullflowers_haemostasis") { }

        class spell_dk_skullflowers_haemostasis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_skullflowers_haemostasis_AuraScript);

            enum eSpells
            {
                BloodBoil = 50842,
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::BloodBoil)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_skullflowers_haemostasis_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_skullflowers_haemostasis_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Uvanimor, the Unbeautiful - 208786
class spell_dk_uvanimor_the_unbeautiful : public SpellScriptLoader
{
    public:
        spell_dk_uvanimor_the_unbeautiful() : SpellScriptLoader("spell_dk_uvanimor_the_unbeautiful") { }

        class spell_dk_uvanimor_the_unbeautiful_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_uvanimor_the_unbeautiful_AuraScript);

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->RestoreRune();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_dk_uvanimor_the_unbeautiful_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_uvanimor_the_unbeautiful_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called for Uvanimor, the Unbeautiful - 137037
class PlayerScript_dk_uvanimor_the_unbeautiful : public PlayerScript
{
    public:
        PlayerScript_dk_uvanimor_the_unbeautiful() : PlayerScript("PlayerScript_uvanimor_the_unbeautiful") {}

        enum eSpells
        {
            UvanimorTheUnbeautiful  = 208786,
            SludgeBelcherAura       = 207313
        };

        enum eItems
        {
            UvanimorTheUnbeautifulItem = 137037
        };

        std::vector<uint32> NPCs =
        {
            DeathKnightPet::Abomination,
            DeathKnightPet::Ghoul
        };

        void OnSummonPet(Player* p_Player)
        {
            if (!p_Player)
                return;

            Pet* l_Pet = p_Player->GetPet();
            if (!l_Pet)
                return;

            if (p_Player->HasItemOrGemWithIdEquipped(eItems::UvanimorTheUnbeautifulItem, 1))
            {
                if (std::find(NPCs.begin(), NPCs.end(), l_Pet->GetEntry()) != NPCs.end())
                    p_Player->AddAura(eSpells::UvanimorTheUnbeautiful, l_Pet);
            }

            if (l_Pet->GetEntry() == 106848 && !p_Player->HasAura(eSpells::SludgeBelcherAura))
                l_Pet->DisappearAndDie();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Shadow Infusion - 198943
class spell_dk_shadow_infusion : public SpellScriptLoader
{
    public:
        spell_dk_shadow_infusion() : SpellScriptLoader("spell_dk_shadow_infusion") { }

        class spell_dk_shadow_infusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_shadow_infusion_AuraScript);

            enum eSpells
            {
                ShadowInfusion      = 198943,
                DarkTransformation  = 63560,
                DeathCoil           = 47632
            };

            std::vector<uint32> m_NPCs =
            {
                DeathKnightPet::Ghoul,
                DeathKnightPet::Abomination
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::DeathCoil)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Pet* l_Pet = l_Player->GetPet();
                SpellInfo const* l_ShadowInfusion = sSpellMgr->GetSpellInfo(eSpells::ShadowInfusion);
                if (!l_Pet || !l_ShadowInfusion || !l_Player->HasAura(eSpells::ShadowInfusion))
                    return;

                if (!l_Pet->HasAura(eSpells::DarkTransformation) && std::find(m_NPCs.begin(), m_NPCs.end(), l_Pet->GetEntry()) != m_NPCs.end())
                    l_Player->ReduceSpellCooldown(eSpells::DarkTransformation, l_ShadowInfusion->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_shadow_infusion_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_dk_shadow_infusion_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_shadow_infusion_AuraScript();
        }
};

/// Last update 7.1.5
/// Epidemic - 207317
class spell_dk_epidemic : public SpellScriptLoader
{
    public:
        spell_dk_epidemic() : SpellScriptLoader("spell_dk_epidemic") { }

        class spell_dk_epidemic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_epidemic_SpellScript);

            enum eSpells
            {
                VirulentPlague  = 191587,
                EpidemicDmg     = 212739,
                EpidemicAOE     = 215969
            };

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (!p_Object->ToUnit()->HasAura(eSpells::VirulentPlague, l_Caster->GetGUID()))
                        return true;

                    return false;
                });
            }

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                Aura* l_Aura = l_Target->GetAura(eSpells::VirulentPlague, l_Caster->GetGUID());
                if (l_Aura == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EpidemicDmg, true);
                l_Caster->CastSpell(l_Target, eSpells::EpidemicAOE, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_epidemic_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_dk_epidemic_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_epidemic_SpellScript();
        }
};

/// last update : 7.1.5
/// Epidemic - 212739, 215969
class spell_dk_epidemic_damage : public SpellScriptLoader
{
    public:
        spell_dk_epidemic_damage() : SpellScriptLoader("spell_dk_epidemic_damage") { }

        class spell_dk_epidemic_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_epidemic_damage_SpellScript);

            enum eSpells
            {
                EpidemicDmg = 212739,
                EpidemicAOE = 215969
            };

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    Unit* l_MainTarget = GetExplTargetUnit();

                    if (l_MainTarget == GetHitUnit() && m_scriptSpellId == eSpells::EpidemicAOE)
                    {
                        PreventHitDamage();
                        return;
                    }

                    float l_AP = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
                    if (l_Caster->CanApplyPvPSpellModifiers())
                        l_AP *= 0.75;

                    int32 l_Damage = 0;

                    if (m_scriptSpellId == eSpells::EpidemicDmg)
                        l_Damage = static_cast<int32>(l_AP * 1.5f);
                    else if (m_scriptSpellId == eSpells::EpidemicAOE)
                        l_Damage = static_cast<int32>(l_AP *  0.275f);

                    if (Unit* l_Target = GetHitUnit())
                        l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);

                    SetHitDamage(l_Damage);
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_dk_epidemic_damage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_epidemic_damage_SpellScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Called by Dancing Rune Weapon - 49028
/// Called for Mouth of Hell - 192570
class spell_dk_mouth_of_hell : public SpellScriptLoader
{
    public:
        spell_dk_mouth_of_hell() : SpellScriptLoader("spell_dk_mouth_of_hell") { }

        class spell_dk_mouth_of_hell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_mouth_of_hell_SpellScript);

            enum eSpells
            {
                MouthOfHell                 = 192570,
                AdditionalDancingRuneWeapon = 192566
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::MouthOfHell))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::AdditionalDancingRuneWeapon, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dk_mouth_of_hell_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_mouth_of_hell_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mirror Ball - 189180
class spell_dk_mirror_ball : public SpellScriptLoader
{
    public:
        spell_dk_mirror_ball() : SpellScriptLoader("spell_dk_mirror_ball") { }

        class spell_dk_mirror_ball_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_mirror_ball_AuraScript);

            enum eSpells
            {
                AntiMagicShell = 48707
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::AntiMagicShell)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_mirror_ball_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_mirror_ball_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Runic Tattoos - 191592 191593 191594
/// Need to recalculate amounts because ArtifactPowerRankEntry has RankBasePoints=0.
class spell_dk_runic_tattoos : public SpellScriptLoader
{
    public:
        spell_dk_runic_tattoos() : SpellScriptLoader("spell_dk_runic_tattoos") { }

        class spell_dk_runic_tattoos_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_runic_tattoos_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, int32 & amount, bool & /*canBeRecalculated*/)
            {
                amount = GetSpellInfo()->Effects[aurEff->GetEffIndex()].BasePoints;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_runic_tattoos_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_POWER_GAIN_PCT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_runic_tattoos_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_MAX_POWER);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_runic_tattoos_AuraScript();
        }
};

/// Runic Power - 191592 - 191593 - 191594 - 224466 - 224467
class spell_dk_runic_power_tattoos_substitute : public SpellScriptLoader
{
public:
    spell_dk_runic_power_tattoos_substitute() : SpellScriptLoader("spell_dk_runic_power_tattoos_substitute") { }

    class spell_dk_runic_power_tattoos_substitute_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_runic_power_tattoos_substitute_SpellScript);

        uint32 m_RunicPowerSpells[5] = { 191592, 191593, 191594, 224466, 224467};

        void HandleOnCast()
        {
            Unit* l_Caster = GetCaster();

            if (!l_Caster)
                return;

            for (size_t i = 1; i < 5; i++)
                if (m_scriptSpellId == m_RunicPowerSpells[i])
                    l_Caster->RemoveAura(m_RunicPowerSpells[i - 1]);
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_dk_runic_power_tattoos_substitute_SpellScript::HandleOnCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_runic_power_tattoos_substitute_SpellScript();
    }
};

/// Last Update 7.1.5
/// Over Powered - 189097
class spell_dk_over_powered : public SpellScriptLoader
{
public:
    spell_dk_over_powered() : SpellScriptLoader("spell_dk_over_powered") { }

    class spell_dk_over_powered_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_over_powered_AuraScript);

        enum eSpells
        {
            RunicPower = 189096,
            Obliterate = 49020
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Obliterate)
                return false;

            return true;
        }

        void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& /*p_EventInfo*/)
        {
            PreventDefaultAction();
            int32 l_bp = p_AurEff->GetAmount();
            if (Unit* l_Caster = GetCaster())
                l_Caster->CastCustomSpell(l_Caster, eSpells::RunicPower, &l_bp, NULL, NULL, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_dk_over_powered_AuraScript::HandleCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_dk_over_powered_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_over_powered_AuraScript();
    }
};

/// Last update 7.3.5 Build 26365
/// Called by Dark Transformation - 63560
class spell_dk_dark_transformation : public SpellScriptLoader
{
    public:
        spell_dk_dark_transformation() : SpellScriptLoader("spell_dk_dark_transformation") { }

        class spell_dk_dark_transformation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_dark_transformation_SpellScript);

            enum eSpells
            {
                SludgeBelcher            = 207313,
                FleshGolem               = 212298,
                TaktheritrixsCommand     = 215068,
                TaktheritrixsCommandBuff = 215069
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::TaktheritrixsCommand))
                    for (Unit* l_Summon : l_Caster->m_Controlled)
                        if (l_Summon->GetEntry() == DeathKnightPet::Gargoyle ||
                            l_Summon->GetEntry() == DeathKnightPet::DarkArbiter ||
                            l_Summon->GetEntry() == DeathKnightPet::ArmyOfTheDead)
                            l_Caster->CastSpell(l_Summon, eSpells::TaktheritrixsCommandBuff, true);

                if (!l_Caster->HasAura(eSpells::SludgeBelcher))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FleshGolem, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_dk_dark_transformation_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_dark_transformation_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Tremble Before Me - 206960
class spell_dk_tremble_before_me : public SpellScriptLoader
{
    public:
        spell_dk_tremble_before_me() : SpellScriptLoader("spell_dk_tremble_before_me") { }

        class spell_dk_tremble_before_me_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_tremble_before_me_AuraScript);

            enum eSpells
            {
                DeathAndDecay       = 52212,
                TrembleBeforeMe     = 206961,
                TrembleBeforeMeAura = 206962
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::DeathAndDecay)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target || l_Target->HasAura(eSpells::TrembleBeforeMeAura))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::TrembleBeforeMeAura, true);
                l_Caster->CastSpell(l_Target, eSpells::TrembleBeforeMe, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_tremble_before_me_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_dk_tremble_before_me_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_tremble_before_me_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Rattling Bones - 192557
class spell_dk_rattling_bones : public SpellScriptLoader
{
    public:
        spell_dk_rattling_bones() : SpellScriptLoader("spell_dk_rattling_bones") { }

        class spell_dk_rattling_bones_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_rattling_bones_AuraScript);

            enum eSpells
            {
                Marrowrend = 195182
            };

            bool OnCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (!p_EventInfo.GetDamageInfo())
                    return false;

                SpellInfo const* l_DamageSpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_DamageSpellInfo || l_DamageSpellInfo->Id != eSpells::Marrowrend)
                    return false;

                return true;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_rattling_bones_AuraScript::OnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_rattling_bones_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
class spell_dk_dancing_rune_weapon_copy : public SpellScriptLoader
{
    public:
        spell_dk_dancing_rune_weapon_copy() : SpellScriptLoader("spell_dk_dancing_rune_weapon_copy") { }

        enum eSpells
        {
            DancingRuneWeapon1 = 49028,
            DancingRuneWeapon2 = 192566,

            DeathStrike     = 49998,
            BloodBoil       = 50842,
            Marrowrend      = 195182,
            DeathsCaress    = 195292,
            Consumption     = 205223,
            HeartStrike     = 206930,

            SkullflowersHaemostasis = 235558,
            Haemostasis             = 235559
        };

        class spell_dk_dancing_rune_weapon_copy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_dancing_rune_weapon_copy_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!HasRuneWeapons(l_Caster))
                    return;

                std::vector<Unit*> l_Weapons;
                GetWeapons(l_Weapons, l_Caster);

                uint32 l_SpellId = GetSpellInfo()->Id;

                for (auto l_Unit : l_Weapons)
                {
                    if (!l_Unit)
                        continue;

                    if (l_Unit->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS) == 0)
                        continue;

                    if (Creature* l_Weapon = l_Unit->ToCreature())
                    {
                        Unit* l_Victim = l_Weapon->getVictim() ? l_Weapon->getVictim() : l_Caster->getVictim();
                        if (!l_Weapon->HasSpellCooldown(l_SpellId))
                        {
                            l_Weapon->CastSpell(l_Victim, l_SpellId, true);

                            /// Generating extra 5 runic power for each heart strike hit via dancing rune weapon
                            if (l_SpellId == eSpells::HeartStrike)
                            {
                                int32 l_RunicPower = GetSpellInfo()->Effects[EFFECT_2].BasePoints * l_Caster->GetPowerCoeff(POWER_RUNIC_POWER);
                                l_Caster->ModifyPower(POWER_RUNIC_POWER, l_RunicPower);
                            }

                            HandleCasterEffects(l_Caster, l_SpellId);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dk_dancing_rune_weapon_copy_SpellScript::HandleAfterCast);
            }

        private:

            bool HasRuneWeapons(Unit* p_Owner) const
            {
                return p_Owner->HasAura(eSpells::DancingRuneWeapon1) || p_Owner->HasAura(eSpells::DancingRuneWeapon2);
            }

            void GetWeapons(std::vector<Unit*>& p_Weapons, Unit* p_Owner)
            {
                for (auto l_Unit : p_Owner->m_Controlled)
                {
                    if (l_Unit->GetEntry() == DeathKnightPet::DancingRuneWeapon)
                    {
                        p_Weapons.push_back(l_Unit);
                    }
                }
            }

            void HandleCasterEffects(Unit* p_Caster, uint32 p_SpellId)
            {
                if (p_SpellId == eSpells::BloodBoil)
                {
                    if (p_Caster->GetAura(eSpells::SkullflowersHaemostasis))
                    {
                        /// Don't use 'CastSpell', it doesn't work
                        if (Aura* l_Aura = p_Caster->GetAura(eSpells::Haemostasis))
                            l_Aura->ModStackAmount(1);
                        else
                            p_Caster->AddAura(eSpells::Haemostasis, p_Caster);
                    }
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_dancing_rune_weapon_copy_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Virulent Eruption - 191685
class spell_dk_virulent_eruption : public SpellScriptLoader
{
    public:
        spell_dk_virulent_eruption() : SpellScriptLoader("spell_dk_virulent_eruption") { }

        class spell_dk_virulent_eruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_virulent_eruption_SpellScript);

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->ToPlayer()->CanApplyPvPSpellModifiers())
                    SetHitDamage(GetHitDamage() * 0.45); ///< Virulent Eruption deals 45% damage in pvp.
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_virulent_eruption_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_virulent_eruption_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Virulent Plague - 191587
class spell_dk_virulent_plague : public SpellScriptLoader
{
    public:
        spell_dk_virulent_plague() : SpellScriptLoader("spell_dk_virulent_plague") { }

        class spell_dk_virulent_plague_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_virulent_plague_AuraScript);

            enum eSpells
            {
                VirulentEruption    = 191685
            };

            void HandlePeriodic(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                if (!roll_chance_i(l_SpellInfo->Effects[EFFECT_1].BasePoints))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::VirulentEruption, true);
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->isAlive())
                    return;

                l_Caster->CastSpell(l_Target, eSpells::VirulentEruption, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_virulent_plague_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_virulent_plague_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_virulent_plague_AuraScript();
        }
};

/// last update : 7.1.5
/// Bursting Sores - 207267
class spell_dk_bursting_sores : public SpellScriptLoader
{
    public:
        spell_dk_bursting_sores() : SpellScriptLoader("spell_dk_bursting_sores") { }

        class spell_dk_bursting_sores_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_bursting_sores_SpellScript);


            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_MainTarget = GetExplTargetUnit();

                p_Targets.remove_if([l_MainTarget](WorldObject* p_Object) -> bool
                {
                    if (p_Object == l_MainTarget)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_bursting_sores_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_bursting_sores_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Rime - 59057
class spell_dk_rime : public SpellScriptLoader
{
    public:
        spell_dk_rime() : SpellScriptLoader("spell_dk_rime") { }

        class spell_dk_rime_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_rime_AuraScript);

            enum eSpells
            {
                Obliterate = 222024,
                Frostscythe = 207230
            };

            Guid128 m_LastProcGUID;

            bool OnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Aura* l_Aura = GetAura();
                if (!l_Aura || !p_EventInfo.GetDamageInfo())
                    return false;

                Spell const* l_Spell = p_EventInfo.GetDamageInfo()->GetSpell();
                if (l_Spell == nullptr)
                    return false;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_1);
                if (!l_AuraEffect)
                    return false;

                SpellInfo const* l_DamageSpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_DamageSpellInfo)
                    return false;

                Guid128 l_CastGUID = l_Spell->GetCastGuid();
                if (l_CastGUID == m_LastProcGUID)
                    return false;

                m_LastProcGUID = l_CastGUID;

                float l_Chance = l_AuraEffect->GetAmount();

                if ((l_DamageSpellInfo->Id == eSpells::Obliterate && roll_chance_f(l_Chance)) ||
                    (l_DamageSpellInfo->Id == eSpells::Frostscythe && roll_chance_f(l_Chance / 2.0f)))
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_rime_AuraScript::OnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_rime_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called By Vampiric Blood - 55233
class spell_dk_vampiric_blood : public SpellScriptLoader
{
    public:
        spell_dk_vampiric_blood() : SpellScriptLoader("spell_dk_vampiric_blood")
        {}

        class spell_dk_vampiric_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_vampiric_blood_SpellScript);

            void HandleOnCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Player* l_Caster = GetCaster()->ToPlayer();
                SpellInfo const* l_Spell = GetSpellInfo();

                if (l_Caster == nullptr || l_Spell == nullptr)
                    return;

                l_Caster->AddSpellCooldown(l_Spell ->Id, 0, l_Spell->RecoveryTime, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_dk_vampiric_blood_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_vampiric_blood_SpellScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called By Vampiric Aura - 238078
class spell_dk_vampiric_aura : public SpellScriptLoader
{
    public:
        spell_dk_vampiric_aura() : SpellScriptLoader("spell_dk_vampiric_aura") { }

        class spell_dk_vampiric_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_vampiric_aura_AuraScript);

            enum eSpells
            {
                Consumption = 205223
            };

            bool OnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();

                if (!l_ProcSpellInfo)
                    return false;

                if (l_ProcSpellInfo->Id == eSpells::Consumption)
                    return true;

                return false;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_vampiric_aura_AuraScript::OnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_vampiric_aura_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Vampiric Aura - 238698
class spell_dk_vampiric_aura_buff : public SpellScriptLoader
{
    public:
        spell_dk_vampiric_aura_buff() : SpellScriptLoader("spell_dk_vampiric_aura_buff") { }

        enum eSpells
        {
            VampiricAura = 238078
        };

        class spell_dk_vampiric_aura_buff_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_vampiric_aura_buff_SpellScript);

            void FilterTargets(std::list<WorldObject*> &p_Targets)
            {
                Unit * l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                int32 l_MaxTargets = 4;
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VampiricAura))
                    l_MaxTargets = l_SpellInfo->Effects[SpellEffIndex::EFFECT_1].BasePoints;

                p_Targets.remove(l_Caster);

                if (p_Targets.size() > l_MaxTargets)
                    JadeCore::Containers::RandomResizeList(p_Targets, l_MaxTargets);

                p_Targets.push_back(l_Caster);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_vampiric_aura_buff_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_ALLY_OR_RAID);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_vampiric_aura_buff_SpellScript();
        }

        class spell_dk_vampiric_aura_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_vampiric_aura_buff_AuraScript);

            void HandlePvPModifier(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.4f;   ///< Vampiric Aura is reduced by 60% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_vampiric_aura_buff_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_LEECH_PCT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_vampiric_aura_buff_AuraScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Legendary: Soul of the Deathlord - 247518
class spell_dk_soul_of_the_deathlord : public SpellScriptLoader
{
    public:
        spell_dk_soul_of_the_deathlord() : SpellScriptLoader("spell_dk_soul_of_the_deathlord") { }

        class spell_dk_soul_of_the_deathlord_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_soul_of_the_deathlord_AuraScript);

            enum eSpells
            {
                FoulBulwark         = 206974,
                GatheringStorm      = 194912,
                BurstingSore        = 207264
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
                case SPEC_DK_BLOOD:
                    l_Player->CastSpell(l_Player, eSpells::FoulBulwark, true);
                    break;
                case SPEC_DK_FROST:
                    l_Player->CastSpell(l_Player, eSpells::GatheringStorm, true);
                    break;
                default:
                    l_Player->CastSpell(l_Player, eSpells::BurstingSore, true);
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
                case SPEC_DK_BLOOD:
                    l_Player->RemoveAura(eSpells::FoulBulwark);
                    break;
                case SPEC_DK_FROST:
                    l_Player->RemoveAura(eSpells::GatheringStorm);
                    break;
                default:
                    l_Player->RemoveAura(eSpells::BurstingSore);
                    break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_soul_of_the_deathlord_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_soul_of_the_deathlord_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_soul_of_the_deathlord_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Chains of Ice - 45524
class spell_dk_chains_of_ice : public SpellScriptLoader
{
public:
    spell_dk_chains_of_ice() : SpellScriptLoader("spell_dk_chains_of_ice") { }

    class spell_dk_chains_of_ice_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_chains_of_ice_AuraScript);

        void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount *= 0.71428f;   ///< Chains of Ice is reduced by 28,572% in PvP
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_chains_of_ice_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_chains_of_ice_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Pillar of Frost - 51271
class spell_dk_pillar_of_frost : public SpellScriptLoader
{
public:
    spell_dk_pillar_of_frost() : SpellScriptLoader("spell_dk_pillar_of_frost") { }

    class spell_dk_pillar_of_frost_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_pillar_of_frost_AuraScript);

        enum eSpells
        {
            IcyEdge = 247240
        };

        void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            int32 l_Amount = p_Amount;
            if (Aura* l_IcyEdgeAura = l_Caster->GetAura(eSpells::IcyEdge))
            {
                l_Amount += l_IcyEdgeAura->GetStackAmount();
                l_Caster->DelayedRemoveAura(eSpells::IcyEdge, 0);
            }

            if (l_Amount > (p_Amount * 2))
                l_Amount = p_Amount * 2;

            p_Amount = l_Amount;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount *= 0.60f;   ///< Pillar of Frost is reduced by 40% in PvP
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_pillar_of_frost_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_pillar_of_frost_AuraScript();
    }

    class spell_dk_pillar_of_frost_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_pillar_of_frost_SpellScript);

        enum eSpells
        {
            IcyEdge = 247240
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::IcyEdge);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_dk_pillar_of_frost_SpellScript::HandleAfterCast);
        }
    };
};

/// Last Update 7.3.2 build 25549
/// Called by Frozen Core - 189179
class spell_dk_frozen_core : public SpellScriptLoader
{
public:
    spell_dk_frozen_core() : SpellScriptLoader("spell_dk_frozen_core") { }

    class spell_dk_frozen_core_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_frozen_core_AuraScript);

        void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount *= 0.50f;   ///< Frozen Core is reduced by 50% in PvP
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_frozen_core_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_frozen_core_AuraScript();
    }
};

/// Last Update 7.3.2 build 25549
/// Souldrinker 240558
class spell_dk_souldrinker_aura : public SpellScriptLoader
{
public:
    spell_dk_souldrinker_aura() : SpellScriptLoader("spell_dk_souldrinker_aura") { }

    class spell_dk_souldrinker_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_souldrinker_aura_AuraScript);

        void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::SouldrinkerHealthPct) = 0;
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_dk_souldrinker_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_souldrinker_aura_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Inexorable Assault - 253594
class spell_dk_inexorable_assault : public SpellScriptLoader
{
    public:
        spell_dk_inexorable_assault() : SpellScriptLoader("spell_dk_inexorable_assault") { }

        class spell_dk_inexorable_assault_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_inexorable_assault_AuraScript);

            enum eSpells
            {
                InexorableAssaultBuff    = 253595,
                InexorableAssault        = 253593
            };

            bool Load() override
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::InexorableAssault);
                if (l_SpellInfo == nullptr)
                    return false;

                m_Radius = (float)l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints;

                return true;
            }

            void HandlePeriodic(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SPEC_DK_FROST)
                    return;

                if (GetEnemiesCount(l_Caster) == 0)
                    l_Caster->CastSpell(l_Caster, eSpells::InexorableAssaultBuff, true);
            }

        private:

            uint32 GetEnemiesCount(Unit* const p_Owner)
            {
                std::list<Unit*> l_TargetList;
                JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck l_Check(p_Owner, m_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck> l_Searcher(p_Owner, l_TargetList, l_Check);
                p_Owner->VisitNearbyObject(m_Radius, l_Searcher);
                return l_TargetList.size();
            }

        private:

            float m_Radius = 8.0f;

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_inexorable_assault_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_inexorable_assault_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Death Knight T20 Blood 2P Bonus - 242001
class spell_dk_t20_blood_2p_bonus : public SpellScriptLoader
{
public:
    spell_dk_t20_blood_2p_bonus() : SpellScriptLoader("spell_dk_t20_blood_2p_bonus") { }

    class spell_dk_t20_blood_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_t20_blood_2p_bonus_AuraScript);

        enum eSpells
        {
            BloodBoil = 50842
        };

        bool OnCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

            if (!l_Caster || !l_DamageInfo)
                return false;

            SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();

            if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::BloodBoil)
                return false;

            return true;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_dk_t20_blood_2p_bonus_AuraScript::OnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_t20_blood_2p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Obliterate - 49020, Frostscythe - 207230
/// Called for Thronebreaker - 238115
class spell_dk_thronebreaker : public SpellScriptLoader
{
    public:
        spell_dk_thronebreaker() : SpellScriptLoader("spell_dk_thronebreaker") { }

        class spell_dk_thronebreaker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_thronebreaker_SpellScript);

            enum eSpells
            {
                Thronebreaker       = 238115,
                Frostscythe         = 207230,
                CrystallineSwords   = 243122,
                RunicChills         = 238079,
                SindragosasFury     = 190778,
                Obliterate          = 49020,
                Obliteration        = 207256,
                RemorselessWinter   = 196770,
                GatheringStormAura  = 211805,
                GatheringStorm      = 194912
            };

            void HandleGatheringStorm()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::Obliteration))
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::RemorselessWinter);
                if (!l_Aura)
                    return;

                if (AuraEffect* l_GatheringStorm = l_Caster->GetAuraEffect(eSpells::GatheringStorm, EFFECT_0))
                {
                    l_Aura->SetDuration(l_Aura->GetDuration() + 500);
                    l_Caster->CastSpell(l_Caster, eSpells::GatheringStormAura, true);
                }
            }

            void HandleAfterHit(SpellEffIndex effIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Thronebreaker);
                if (!l_Caster || !l_Caster->IsPlayer() || !l_SpellInfo)
                    return;

                if (l_Target != l_Caster && l_Caster->IsValidAttackTarget(l_Target))
                {
                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Thronebreaker, EFFECT_1))
                    {
                        if ((m_scriptSpellId == eSpells::Frostscythe && roll_chance_i(l_AuraEffect->GetAmount())) || m_scriptSpellId != eSpells::Frostscythe)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::CrystallineSwords, true);
                            // l_Caster->ToPlayer()->AddSpellCooldown(eSpells::CrystallineSwords, 0, l_SpellInfo->ProcCooldown, false);
                        }
                    }

                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::RunicChills, EFFECT_0))
                        l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::SindragosasFury, -l_AuraEffect->GetAmount());
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Obliterate)
                {
                    OnCast += SpellCastFn(spell_dk_thronebreaker_SpellScript::HandleGatheringStorm);
                    OnEffectHitTarget += SpellEffectFn(spell_dk_thronebreaker_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                }
                else
                    OnEffectHitTarget += SpellEffectFn(spell_dk_thronebreaker_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_thronebreaker_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dark Transformation - 63560
class spell_dk_dark_transformation_check_target : public SpellScriptLoader
{
    public:
        spell_dk_dark_transformation_check_target() : SpellScriptLoader("spell_dk_dark_transformation_check_target") { }

        class spell_dk_dark_transformation_check_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_dark_transformation_check_target_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Spell* l_Spell = GetSpell();
                if (!l_Spell)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->GetPet())
                    return;

                TriggerCastFlags l_TriggerFlags = TriggerCastFlags(TriggerCastFlags::TRIGGERED_FULL_MASK & ~TriggerCastFlags::TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD);

                l_Spell->setTriggerCastFlags(l_TriggerFlags);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_dk_dark_transformation_check_target_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_dark_transformation_check_target_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Valkyr Strike - 198715
class spell_dk_valkyr_strike : public SpellScriptLoader
{
public:
    spell_dk_valkyr_strike() : SpellScriptLoader("spell_dk_valkyr_strike") { }

    class spell_dk_valkyr_strike_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_valkyr_strike_SpellScript);

        enum eSpells
        {
            ShadowEmpowerment           = 211947,
            TaktheritrixsCommandBuff    = 215069,
            Fleshsearer                 = 214906,
            CunningOfTheEbonBlade       = 241050,
            UnholyDeathKnight           = 137007
        };

        enum eArtifactPowerIds
        {
            FleshsearerArtifact = 1361
        };

        void HandleOnEffectHitTarget(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner || !l_Owner->IsPlayer())
                return;

            Player* l_Player = l_Owner->ToPlayer();
            if (!l_Player)
                return;

            uint32 l_Damage = l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 2.2f;

            float l_Mastery = (l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * 2.25f / 100.0f) + 1.0f;

            if (l_Mastery > 0)
                l_Damage *= l_Mastery;

            float l_Versatility = (1.0f + l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f);
            l_Damage *= l_Versatility;

            if (l_Player->HasAura(eSpells::Fleshsearer))
            {
                if (MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact())
                {
                    if (ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowerIds::FleshsearerArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowerIds::FleshsearerArtifact) - 1))
                    {
                        if (l_RankEntry->AuraPointsOverride > 0)
                            AddPct(l_Damage, l_RankEntry->AuraPointsOverride);
                    }
                }
            }

            if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::CunningOfTheEbonBlade, EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount());

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ShadowEmpowerment, EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount());

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::TaktheritrixsCommandBuff, SpellEffIndex::EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount());

            if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::UnholyDeathKnight, EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount());

            l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

            SetHitDamage(l_Damage);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dk_valkyr_strike_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dk_valkyr_strike_SpellScript();
    }
};

/// Last Update 7.3.5
/// Item - Death Knight T20 Frost 2P Bonus
class PlayerScript_dk_t20_frost_2p_bonus : public PlayerScript
{
public:
    PlayerScript_dk_t20_frost_2p_bonus() : PlayerScript("PlayerScript_dk_t20_frost_2p_bonus") {}

    enum eSpells
    {
        PillarOfFrost = 51271,
        T20Frost2PBonus = 242058
    };

    void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
    {
        if (p_After)
            return;

        if (p_Player->getClass() != CLASS_DEATH_KNIGHT || p_Power != POWER_RUNIC_POWER || p_Regen || p_NewValue >= p_OldValue)
            return;

        Aura* l_Frost2PBonus = p_Player->GetAura(eSpells::T20Frost2PBonus);
        Aura* l_PillarOfFrost = p_Player->GetAura(eSpells::PillarOfFrost);
        if (!l_Frost2PBonus || !l_PillarOfFrost)
            return;

        int32 l_PowerSpent = p_OldValue - p_NewValue;
        p_Player->m_SpellHelper.GetUint32(eSpellHelpers::T20Frost2PBonus) += l_PowerSpent;

        if (AuraEffect* l_AurEff0 = l_Frost2PBonus->GetEffect(EFFECT_0))
        {
            if (p_Player->m_SpellHelper.GetUint32(eSpellHelpers::T20Frost2PBonus) >= (l_AurEff0->GetAmount() * 10))
            {
                if (AuraEffect* l_AurEff1 = l_Frost2PBonus->GetEffect(EFFECT_1))
                {
                    p_Player->m_SpellHelper.GetUint32(eSpellHelpers::T20Frost2PBonus) -= l_AurEff0->GetAmount() * 10;
                    l_PillarOfFrost->SetDuration(l_PillarOfFrost->GetDuration() + (l_AurEff1->GetAmount() * 100));
                }
            }
        }
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Army of the Dead - 42651 , 205491
/// Called for 242064 Item - Death Knight T20 Unholy 2P Bonus
class spell_dk_army_of_the_dead : public SpellScriptLoader
{
public:
    spell_dk_army_of_the_dead() : SpellScriptLoader("spell_dk_army_of_the_dead") { }

    class spell_dk_army_of_the_dead_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_army_of_the_dead_SpellScript);

        enum eSpells
        {
            T20Unholy2PBonus = 242064,
            MasterOfGhouls = 246995
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            SpellInfo const* l_AuraInfo = sSpellMgr->GetSpellInfo(eSpells::MasterOfGhouls);
            if (!l_AuraInfo)
                return;

            if (!l_Caster->HasAura(eSpells::T20Unholy2PBonus))
                return;

            if (l_Caster->HasAura(eSpells::MasterOfGhouls) && !l_Caster->HasAura(eSpells::T20Unholy2PBonus))
                l_Caster->RemoveAura(eSpells::MasterOfGhouls);

            if (Aura* l_Aura = l_Caster->GetAura(eSpells::MasterOfGhouls))
            {
                l_Aura->SetDuration(l_Aura->GetDuration() + l_AuraInfo->GetMaxDuration());
                l_Aura->SetMaxDuration(l_Aura->GetDuration());
            }
            else
                l_Caster->CastSpell(l_Caster, eSpells::MasterOfGhouls, true);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_dk_army_of_the_dead_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dk_army_of_the_dead_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Gargoyle Strike - 51963
class spell_dk_gargoyle_strike : public SpellScriptLoader
{
public:
    spell_dk_gargoyle_strike() : SpellScriptLoader("spell_dk_gargoyle_strike") { }

    class spell_dk_gargoyle_strike_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_gargoyle_strike_SpellScript);

        enum eSpells
        {
            DeathDebuff     = 191730,
            MasterOfGhouls  = 246995
        };

        void HandleOnEffectHitTarget(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner || !l_Owner->IsPlayer())
                return;

            uint32 l_Damage = GetHitDamage();

            if (Aura* l_DeathDebuff = l_Target->GetAura(eSpells::DeathDebuff, l_Owner->GetGUID()))
                if (SpellInfo const* l_DebuffInfo = sSpellMgr->GetSpellInfo(eSpells::DeathDebuff))
                    AddPct(l_Damage, l_DebuffInfo->Effects[EFFECT_0].BasePoints * l_DeathDebuff->GetStackAmount());

            if (l_Owner->HasAura(eSpells::MasterOfGhouls))
                if (SpellInfo const* l_BuffInfo = sSpellMgr->GetSpellInfo(eSpells::MasterOfGhouls))
                    AddPct(l_Damage, l_BuffInfo->Effects[EFFECT_0].BasePoints);

            SetHitDamage(l_Damage);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dk_gargoyle_strike_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dk_gargoyle_strike_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dragged to Helheim 218321
class spell_dk_dragged_to_helheim_damage : public SpellScriptLoader
{
    public:
        spell_dk_dragged_to_helheim_damage() : SpellScriptLoader("spell_dk_dragged_to_helheim_damage") { }

        class spell_dk_dragged_to_helheim_damage_SpellScript : public SpellScript
        {
            enum eSpells
            {
                CunningOfTheEbonBlade   = 241050,
                Fleshsearer             = 214906,
                UnholyPassive           = 137007,
                DeathDebuff             = 191730,
                MasterOfGhouls          = 246995
            };

            enum eArtifactPowerIds
            {
                FleshsearerArtifact     = 1361
            };

            PrepareSpellScript(spell_dk_dragged_to_helheim_damage_SpellScript);

            void HandleOnHitTarget(SpellEffIndex /*p_effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                int32 l_Damage = int32(l_Player->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * GetSpellInfo()->Effects[EFFECT_0].AttackPowerMultiplier);

                if (l_Player->HasAura(eSpells::CunningOfTheEbonBlade))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CunningOfTheEbonBlade))
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                if (l_Player->HasAura(eSpells::Fleshsearer))
                {
                    MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                    if (!l_Manager)
                    {
                        SetHitDamage(l_Damage);
                        return;
                    }

                    ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowerIds::FleshsearerArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowerIds::FleshsearerArtifact) - 1);
                    if (!l_RankEntry)
                    {
                        SetHitDamage(l_Damage);
                        return;
                    }

                    if (l_RankEntry->AuraPointsOverride > 0)
                        AddPct(l_Damage, l_RankEntry->AuraPointsOverride);
                }

                float l_Versatility = (1.0f + l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f);
                l_Damage *= l_Versatility;

                float l_Mastery = (l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * 2.25f / 100.0f) + 1.0f;
                l_Damage *= l_Mastery;

                if (l_Player->HasAura(eSpells::UnholyPassive))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::UnholyPassive))
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                if (Aura* l_DeathDebuff = l_Target->GetAura(eSpells::DeathDebuff, l_Owner->GetGUID()))
                    if (SpellInfo const* l_DebuffInfo = sSpellMgr->GetSpellInfo(eSpells::DeathDebuff))
                        AddPct(l_Damage, l_DebuffInfo->Effects[EFFECT_0].BasePoints * l_DeathDebuff->GetStackAmount());

                if (l_Owner->HasAura(eSpells::MasterOfGhouls))
                    if (SpellInfo const* l_BuffInfo = sSpellMgr->GetSpellInfo(eSpells::MasterOfGhouls))
                        AddPct(l_Damage, l_BuffInfo->Effects[EFFECT_0].BasePoints);

                if (l_Player->CanApplyPvPSpellModifiers())
                    l_Damage *= 0.8f; ///< 80% in PvP

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_dragged_to_helheim_damage_SpellScript::HandleOnHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_dragged_to_helheim_damage_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Shambling Rush - 91802
class spell_dk_shambling_rush : public SpellScriptLoader
{
    public:
        spell_dk_shambling_rush() : SpellScriptLoader("spell_dk_shambling_rush") { }

        class spell_dk_shambling_rush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_shambling_rush_SpellScript);

            enum eSpells
            {
                InterruptSpell  = 91807
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                uint32 l_Delay = l_Caster->GetDistance(l_Target) * 33;

                l_Caster->DelayedCastSpell(l_Target, eSpells::InterruptSpell, true, l_Delay);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dk_shambling_rush_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_shambling_rush_SpellScript();
        }
};

/// Last Update 7.3.0 - Build 24920
/// Obliterate Off Hand - 66198 (so that both hands benefit from the crit, we remove Killing machine when the off hands hit.)
/// Frostscythe - 207230
class spell_dk_killing_machine_buff: public SpellScriptLoader
{
    public:
        spell_dk_killing_machine_buff() : SpellScriptLoader("spell_dk_killing_machine_buff")
        {}

        enum eSpells
        {
            SpellKillingMachineBuff = 51124,
        };

        class spell_dk_killing_machine_buff_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_killing_machine_buff_SpellScript);

            void HandleAfterHit()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->RemoveAura(eSpells::SpellKillingMachineBuff);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_dk_killing_machine_buff_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_killing_machine_buff_SpellScript();
        }
};

/// Death Knight T20 Unholy 4P Bonus - 242225
class PlayerScript_dk_t20_4p_bonus : public PlayerScript
{
public:
    PlayerScript_dk_t20_4p_bonus() : PlayerScript("PlayerScript_dk_t20_4p_bonus") {}

    enum eSpells
    {
        T20Unholy4PBonus    = 242225,
        ArmyOfTheDead       = 42650,
    };

    void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
    {
        if (p_After)
            return;

        if (p_Player->getClass() != CLASS_DEATH_KNIGHT || p_Power != POWER_RUNES || p_Regen || p_NewValue > p_OldValue)
            return;

        int32 l_PowerSpent = p_OldValue - p_NewValue;

        if (AuraEffect* l_AuraEffect = p_Player->GetAuraEffect(eSpells::T20Unholy4PBonus, EFFECT_0))
        {
            for (int32 l_I = 0; l_I < l_PowerSpent; l_I++)
            {
                p_Player->ReduceSpellCooldown(eSpells::ArmyOfTheDead, uint32((l_AuraEffect->GetAmount() / 10) * IN_MILLISECONDS));
            }
        }
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Smash - 212336
class spell_dk_smash : public SpellScriptLoader
{
public:
    spell_dk_smash() : SpellScriptLoader("spell_dk_smash") { }

    class spell_dk_smash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_smash_SpellScript);

        enum eSpells
        {
            Gnaw                = 91800,
            DarkTransformation  = 63560
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Caster || !l_Target)
                return;

            if (l_Caster->HasAura(eSpells::DarkTransformation))
                l_Caster->CastSpell(l_Target, eSpells::Gnaw, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_dk_smash_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_smash_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Gravewarden - 242010
class spell_dk_gravewarden : public SpellScriptLoader
{
public:
    spell_dk_gravewarden() : SpellScriptLoader("spell_dk_gravewarden") { }

    class spell_dk_gravewarden_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_gravewarden_AuraScript);

        enum eSpells
        {
            T20Blood4PBonus = 242009
        };

        void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T20Blood4PBonus, EFFECT_0))
                p_Amount = -int32(l_AuraEffect->GetAmount() * 10);
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_gravewarden_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_gravewarden_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Dark Succor - 178819
class spell_dk_dark_succor_proc : public SpellScriptLoader
{
    public:
        spell_dk_dark_succor_proc() : SpellScriptLoader("spell_dk_dark_succor_proc") { }

        class spell_dk_dark_succor_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_dark_succor_proc_AuraScript);

            bool OnCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return false;

                Unit* l_Target = p_ProcEventInfo.GetActionTarget();
                if (!l_Target)
                    return false;

                if (!l_Player->isHonorOrXPTarget((l_Target)))
                    return false;

                if (l_Target->isPet() || l_Target->isGuardian())
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_dark_succor_proc_AuraScript::OnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_dark_succor_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Dancing Rune Weapon - 81256
/// Called for Item - Death Knight T21 Blood 4P Bonus - 251877
class spell_dk_t21_blood_4p_bonus : public SpellScriptLoader
{
    public:
        spell_dk_t21_blood_4p_bonus() : SpellScriptLoader("spell_dk_t21_blood_4p_bonus") { }

        class spell_dk_t21_blood_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_t21_blood_4p_bonus_AuraScript);

            enum eSpells
            {
                T21Blood4P      = 251877,
                RuneMaster      = 253381
            };

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::T21Blood4P))
                    return;

                l_Caster->DelayedCastSpell(l_Caster, eSpells::RuneMaster, true, 1);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_t21_blood_4p_bonus_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_PARRY_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_t21_blood_4p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Death Knight T21 Frost 4P Bonus - 251875
class spell_dk_t21_frost_4p_bonus : public SpellScriptLoader
{
    public:
        spell_dk_t21_frost_4p_bonus() : SpellScriptLoader("spell_dk_t21_frost_4p_bonus") { }

        class spell_dk_t21_frost_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_t21_frost_4p_bonus_AuraScript);

            enum eSpells
            {
                FreezingDeath   = 253590
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                if (!(l_DamageInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FROST))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetProcTarget();;

                if (!l_Caster || !l_Target)
                    return;

                uint8 IcyclesAmount = 3; /// Got this value from spell description  ${$253590sw1*3} and video from retail
                for (uint8 i = 0; i < IcyclesAmount; ++i)
                    l_Caster->CastSpell(l_Target, eSpells::FreezingDeath, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_t21_frost_4p_bonus_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_dk_t21_frost_4p_bonus_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_t21_frost_4p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Death Knight T20 Frost 4P Bonus - 242063
class spell_dk_t20_frost_4p_bonus : public SpellScriptLoader
{
    public:
        spell_dk_t20_frost_4p_bonus() : SpellScriptLoader("spell_dk_t20_frost_4p_bonus") { }

        enum eSpells
        {
            IcyEdge = 247240
        };

        class spell_dk_t20_frost_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_t20_frost_4p_bonus_AuraScript);

            Guid128 m_CastId;
            int32 m_RuneCounter = 0;
            int32 m_RunesNeed = 0;
            int32 m_BonusToAdd = 0;

            bool Load() override
            {
                m_RunesNeed = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].BasePoints;
                m_BonusToAdd = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints;
                return true;
            }

            bool OnCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                if (GetUnitOwner() == nullptr || !GetUnitOwner()->IsPlayer() || p_ProcEventInfo.GetDamageInfo()->GetSpell() == nullptr)
                    return false;

                if (m_CastId == p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid())
                    return false;

                Player* l_Owner = GetUnitOwner()->ToPlayer();
                if (l_Owner->getClass() != Classes::CLASS_DEATH_KNIGHT)
                    return false;

                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (CalcRuneCost(l_SpellInfo) <= 0)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                if (!GetUnitOwner() || !GetUnitOwner()->IsPlayer() || !p_ProcEventInfo.GetDamageInfo()->GetSpell())
                    return;

                if (m_CastId == p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid())
                    return;

                m_CastId = p_ProcEventInfo.GetDamageInfo()->GetSpell()->GetCastGuid();
                Player* l_Owner = GetUnitOwner()->ToPlayer();

                if (l_Owner->getClass() != Classes::CLASS_DEATH_KNIGHT)
                    return;

                SpellInfo const* l_triggerSpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (l_triggerSpellInfo == nullptr)
                    return;

                int32 l_RuneCost = CalcRuneCost(l_triggerSpellInfo);
                if (l_RuneCost <= 0)
                    return;

                m_RuneCounter += l_RuneCost;

                int32 l_NextRunesCount = m_RuneCounter / m_RunesNeed;
                if (l_NextRunesCount <= 0)
                    return;

                m_RuneCounter -= (l_NextRunesCount * m_RunesNeed);

                int32 l_NextBonus = l_NextRunesCount * m_BonusToAdd;

                if (l_NextBonus <= 0)
                    return;

                ApplyBonus(l_Owner, l_NextBonus);
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Owner = GetUnitOwner())
                    l_Owner->RemoveAura(eSpells::IcyEdge);
            }

            int32 CalcRuneCost(SpellInfo const* p_SpellInfo)
            {
                if (p_SpellInfo->SpellPowers.empty())
                    return 0;

                int32 l_RuneCost = 0;
                for (auto spellPower : p_SpellInfo->SpellPowers)
                {
                    if (spellPower->PowerType == Powers::POWER_RUNES)
                        l_RuneCost += spellPower->Cost;
                }

                return l_RuneCost;
            }

            void ApplyBonus(Unit* p_Owner, int32 p_Bonus)
            {
                if (Aura* l_IcyEdgeAura = p_Owner->GetAura(eSpells::IcyEdge))
                {
                    l_IcyEdgeAura->ModStackAmount(p_Bonus);
                    l_IcyEdgeAura->SetDuration(l_IcyEdgeAura->GetMaxDuration());
                }
                else
                    p_Owner->CastCustomSpell(eSpells::IcyEdge, SpellValueMod::SPELLVALUE_AURA_STACK, p_Bonus, p_Owner, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_t20_frost_4p_bonus_AuraScript::OnCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_dk_t20_frost_4p_bonus_AuraScript::OnProc, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_DUMMY);
                AfterEffectRemove += AuraEffectApplyFn(spell_dk_t20_frost_4p_bonus_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_t20_frost_4p_bonus_AuraScript();
        }
};

/// Death Gate - 50977
class spell_dk_death_gate_check : public SpellScriptLoader
{
    public:
        spell_dk_death_gate_check() : SpellScriptLoader("spell_dk_death_gate_check") { }

        enum eMapIDs
        {
            TheMageHunter           = 1616,
            TheArchmagesReckoning   = 1673,
            ThwartingTheTwins       = 1684,
            HighlordsReturn         = 1698,
            FeltotemsFall           = 1702,
            TheGodQueensFury        = 1703,
            EndOfTheRisenThreat     = 1710
        };

        class spell_dk_death_gate_check_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_gate_check_SpellScript);

            std::set<uint32> m_MapsToCheck =
            {
                eMapIDs::TheMageHunter,
                eMapIDs::TheArchmagesReckoning,
                eMapIDs::ThwartingTheTwins,
                eMapIDs::HighlordsReturn,
                eMapIDs::FeltotemsFall,
                eMapIDs::TheGodQueensFury,
                eMapIDs::EndOfTheRisenThreat
            };

            SpellCastResult CheckMap()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (m_MapsToCheck.find(l_Caster->GetMapId()) != m_MapsToCheck.end())
                        return SpellCastResult::SPELL_FAILED_NOT_HERE;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_check_SpellScript::CheckMap);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_gate_check_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_deathknight_spell_scripts()
{
    new spell_dk_rime();
    new spell_dk_bursting_sores();
    new spell_dk_epidemic_damage();
    new spell_dk_death_coil_damage();
    new spell_dk_virulent_plague();
    new spell_dk_virulent_eruption();
    new spell_dk_outbreak_periodic();
    new spell_dk_glacial_advance_dmg();
    new spell_dk_epidemic();
    new spell_dk_consumption();
    new spell_dk_scourge_strike();
    new spell_dk_scourge_strike_shadow();
    new spell_dk_corpse_shield();
    new spell_dk_festering_wound();
    new spell_dk_icecap_effect();
    new spell_dk_wraith_walk();
    new spell_dk_remorseless_winter_aura();
    new spell_dk_remorseless_winter();
    new spell_dk_permafrost();
    new spell_dk_blood_mirror();
    new spell_dk_bonestorm();
    new spell_dk_will_of_the_necropolis();
    new spell_dk_tombstone();
    new spell_dk_mark_of_blood();
    new spell_dk_bone_shield();
    new spell_dk_soulgorge();
    new spell_dk_heart_strike();
    new spell_dk_bloodworms();
    new spell_dk_army_of_the_death_taunt();
    new spell_dk_defile();
    new spell_dk_death_coil();
    new spell_dk_gorefiends_grasp();
    new spell_dk_desecrated_ground();
    new spell_dk_death_strike_heal();
    new spell_dk_howling_blast();
    new spell_dk_howling_blast_aoe();
    new spell_dk_conversion();
    new spell_dk_soul_reaper();
    new spell_dk_death_strike();
    new spell_dk_purgatory();
    new spell_dk_purgatory_absorb();
    new spell_dk_plague_leech();
    new spell_dk_unholy_blight();
    new spell_dk_anti_magic_shell_raid();
    new spell_dk_anti_magic_shell_self();
    new spell_dk_anti_magic_zone();
    new spell_dk_death_gate_teleport();
    new spell_dk_death_gate();
    new spell_dk_blood_boil();
    new spell_dk_corpse_explosion();
    new spell_dk_runic_empowerment();
    new spell_dk_runic_corruption();
    new spell_dk_death_pact();
    new spell_dk_chain_of_ice();
    new spell_dk_chilblains_aura();
    new spell_dk_glyph_of_the_geist();
    new spell_dk_glyph_of_the_skeleton();
    new spell_dk_death_grip_dummy();
    new spell_dk_control_undead();
    new spell_dk_presences();
    new spell_dk_breath_of_sindragosa();
    new spell_dk_blood_shield();
    new spell_dk_item_t17_frost_4p_driver();
    new spell_dk_item_t17_frost_4p_driver_periodic();
    new spell_dk_frozen_soul();
    new spell_dk_soul_of_the_deathlord();
    new spell_dk_t20_blood_2p_bonus();
    new PlayerScript_dk_t20_frost_2p_bonus();
    new spell_dk_army_of_the_dead();
    new PlayerScript_dk_t20_4p_bonus();
    new spell_dk_gravewarden();

    /// Frost
    new spell_dk_killing_machine_buff();
    new spell_dk_icy_talons();
    new spell_dk_avalanche_aura();
    new spell_dk_murderous_efficiency();
    new spell_dk_glacial_advance();
    new spell_dk_abominations_might();
    new spell_dk_winter_is_coming();
    new spell_dk_sindragosa_fury();
    new spell_dk_crystalline_sword();
    new spell_dk_crystalline_sword_launch();
    new spell_dk_shaterring_strikes();
    new spell_dk_frozen_pulse();
    new spell_dk_obliteration();
    new spell_dk_frost_breath();
    new spell_dk_breath_of_sindragosa_damage();
    new spell_dk_inexorable_assault();
    new spell_dk_t21_frost_4p_bonus();
    new spell_dk_t20_frost_4p_bonus();

    /// Unholy
    new spell_outbreak_aura();
    new spell_dk_outbreak();
    new spell_dk_festering_strike();
    new spell_dk_festering_wound_aura();
    new spell_dk_marrowrend();
    new spell_dk_raise_dead();
    new spell_dk_clawing_shadows();
    new spell_dk_pandemic();
    new spell_dk_pestilent_pustules();
    new spell_dk_blighted_rune_weapon();
    new spell_dk_taktheritrixs_command();
    new spell_dk_uvanimor_the_unbeautiful();
    new spell_dk_shadow_infusion();
    new spell_dk_sludge_belcher();
    new spell_dk_all_will_serve();
    new spell_dk_dark_transformation();
    new spell_dk_dark_arbiter();
    new spell_dk_infected_claws();
    new spell_dk_runic_power_tattoos_substitute();
    new spell_dk_dark_transformation_check_target();
    new spell_dk_valkyr_strike();
    new spell_dk_gargoyle_strike();
    new spell_dk_dragged_to_helheim_damage();
    new spell_dk_shambling_rush();
    new spell_dk_smash();
    new spell_dk_dark_succor_proc();
    new spell_dk_item_t21_unholy_4p_proc();

    /// Blood
    new spell_dk_crimson_scourge_aura();
    new spell_dk_vampiric_blood();
    new spell_dk_unending_thirst_trigger();
    new spell_dk_unending_thirst();
    new spell_dk_blood_feast();
    new spell_dk_skullflowers_haemostasis();
    new spell_dk_bonestorm_damage();
    new spell_dk_tremble_before_me();
    new spell_dk_rattling_bones();
    new spell_dk_dancing_rune_weapon_copy();
    new spell_dk_souldrinker_aura();
    new spell_dk_t19_blood_4p_proc();
    new spell_dk_t21_blood_4p_bonus();

    /// PlayerScript
    new PlayerScript_dk_red_thirst();
    new PlayerScript_dk_death_strike();
    new PlayerScript_dk_gathering_storm();
    new PlayerScript_dk_death_gate();
    new PlayerScript_dk_dark_arbiter();
    new PlayerScript_dk_uvanimor_the_unbeautiful();
    new Playerscript_dk_lanathiels_lamentation();

    /// Pet Abiltiies
    new spell_dk_pet_abilites();
    new spell_dk_pet_hook();
    new spell_dk_reanimation_explosion();

    /// Artifacts
    new spell_dk_apocalypse();
    new spell_dk_scourge_the_unbeliever();
    new spell_dk_gravitation_pull();
    new spell_dk_gravitation_pull_aura();
    new spell_dk_the_shambler();
    new spell_dk_hypothermia();
    new spell_dk_scourge_of_worlds();
    new spell_dk_umbilicus_eternus();
    new spell_dk_mouth_of_hell();
    new spell_dk_mirror_ball();
    new spell_dk_runic_tattoos();
    new spell_dk_over_powered();
    new spell_dk_vampiric_aura();
    new spell_dk_vampiric_aura_buff();
    new spell_dk_thronebreaker();

    /// Honor Talents
    new spell_dk_walking_dead();
    new spell_dk_deathchill();
    new spell_dk_wandering_plague();
    new spell_dk_wandering_plague_jump();
    new spell_dk_wandering_plague_debuff();
    new spell_dk_murderous_intend();
    new spell_dk_necrotic_strike();
    new spell_dk_chill_streak();
    new spell_dk_chill_streak_visual();
    new spell_dk_chill_streak_damages();
    new spell_dk_crypt_fever();
    new spell_dk_cadaverous_pallor();
    new spell_dk_blight();
    new spell_dk_tundra_stalker();
    new spell_dk_last_dance();
    new spell_dk_delirium();
    new spell_dk_death_and_decay();
    new spell_dk_unholy_mutation();
    new spell_dk_unholy_center();
    new spell_dk_honor_aura_check();
    new spell_dk_honor_auras_removal();

    /// PvP Modifiers
    new spell_dk_chains_of_ice();
    new spell_dk_pillar_of_frost();
    new spell_dk_frozen_core();
    new spell_dk_unholy_frenzy_remove();

    new spell_dk_death_gate_check();
}
#endif
