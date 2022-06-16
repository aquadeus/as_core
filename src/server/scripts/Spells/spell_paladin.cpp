////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_PALADIN and SPELLFAMILY_GENERIC spells used by paladin players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pal_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "HelperDefines.h"
#include "CellImpl.h"
#include "Map.h"

enum PaladinSpells
{
    PALADIN_SPELL_JUDGMENT                      = 20271,
    PALADIN_SPELL_JUDGMENTS_OF_THE_WISE         = 105424,
    PALADIN_SPELL_TEMPLARS_VERDICT              = 85256,
    PALADIN_SPELL_FINAL_VERDICT                 = 157048,
    PALADIN_SPELL_PHYSICAL_VULNERABILITY        = 81326,
    PALADIN_SPELL_LONG_ARM_OF_THE_LAW           = 87172,
    PALADIN_SPELL_LONG_ARM_OF_THE_LAW_RUN_SPEED = 87173,
    PALADIN_SPELL_HOLY_SHOCK_R1                 = 20473,
    PALADIN_SPELL_HOLY_SHOCK_R1_DAMAGE          = 25912,
    PALADIN_SPELL_HOLY_SHOCK_R1_HEALING         = 25914,
    PALADIN_SPELL_HOLY_SHOCK_ENERGIZE           = 148976,
    SPELL_BLESSING_OF_LOWER_CITY_DRUID          = 37878,
    SPELL_BLESSING_OF_LOWER_CITY_PALADIN        = 37879,
    SPELL_BLESSING_OF_LOWER_CITY_PRIEST         = 37880,
    SPELL_BLESSING_OF_LOWER_CITY_SHAMAN         = 37881,
    SPELL_DIVINE_STORM                          = 53385,
    SPELL_DIVINE_STORM_DUMMY                    = 54171,
    SPELL_DIVINE_STORM_HEAL                     = 54172,
    SPELL_FORBEARANCE                           = 25771,
    PALADIN_SPELL_WORD_OF_GLORY                 = 85673, ///< 7.0.3 deprecated
    PALADIN_SPELL_WORD_OF_GLORY_HEAL            = 130551,
    PALADIN_SPELL_GLYPH_OF_WORD_OF_GLORY        = 54936,
    PALADIN_SPELL_GLYPH_OF_WORD_OF_GLORY_DAMAGE = 115522,
    PALADIN_SPELL_GLYPH_OF_HARSH_WORDS          = 54938,
    PALADIN_SPELL_HARSH_WORDS_DAMAGE            = 130552,
    PALADIN_SPELL_HOLY_PRISM_ALLIES             = 114871,
    PALADIN_SPELL_HOLY_PRISM_ENEMIES            = 114852,
    PALADIN_SPELL_HOLY_PRISM_DAMAGE_VISUAL      = 114862,
    PALADIN_SPELL_HOLY_PRISM_DAMAGE_VISUAL_2    = 114870,
    PALADIN_SPELL_HOLY_PRISM_HEAL_VISUAL        = 121551,
    PALADIN_SPELL_HOLY_PRISM_HEAL_VISUAL_2      = 121552,
    PALADIN_SPELL_ARCING_LIGHT_HEAL             = 119952,
    PALADIN_SPELL_ARCING_LIGHT_DAMAGE           = 114919,
    PALADIN_SPELL_EXECUTION_SENTENCE            = 114916,
    PALADIN_SPELL_STAY_OF_EXECUTION             = 114917,
    PALADIN_SPELL_EXECUTION_DISPEL_DAMAGE       = 146585,
    PALADIN_SPELL_EXECUTION_DISPEL_HEAL         = 146586,
    PALADIN_SPELL_GLYPH_OF_BLINDING_LIGHT       = 54934,
    PALADIN_SPELL_BLINDING_LIGHT_CONFUSE        = 105421,
    PALADIN_SPELL_BLINDING_LIGHT_STUN           = 115752,
    PALADIN_SPELL_MASS_EXORCISM                 = 122032,
    PALADIN_SPELL_SACRED_SHIELD                 = 65148,
    PALADIN_SPELL_ARDENT_DEFENDER_HEAL          = 66235,
    PALADIN_SPELL_BEACON_OF_LIGHT               = 53563,
    PALADIN_SPELL_BEACON_OF_FAITH               = 156910,
    PALADIN_SPELL_SELFLESS_HEALER_VISUAL        = 128863,
    PALADIN_SPELL_SELFLESS_HEALER_STACK         = 114250,
    PALADIN_SPELL_SELFLESS_HEALER               = 85804,
    PALADIN_SPELL_SHIELD_OF_THE_RIGHTEOUS_PROC  = 132403,
    PALADIN_SPELL_BASTION_OF_GLORY              = 114637,
    PALADIN_SPELL_DIVINE_PURPOSE_AURA           = 90174,
    PALADIN_SPELL_DIVINE_SHIELD                 = 642,
    PALADIN_SPELL_LAY_ON_HANDS                  = 633,
    PALADIN_SPELL_DIVINE_PROTECTION             = 498,
    PALADIN_SPELL_GLYPH_OF_AVENGING_WRATH       = 54927,
    PALADIN_SPELL_AVENGING_WRATH_REGEN_BY_GLYPH = 115547,
    PALADIN_SPELL_SACRED_CLEANSING              = 53551,
    PALADIN_SPELL_DAYBREAK_AURA                 = 88821,
    PALADIN_SPELL_DAYBREAK_PROC                 = 88819,
    PALADIN_SPELL_DAYBREAK_HEAL                 = 121129,
    PALADIN_SPELL_HOLY_AVENGER                  = 105809,
    PALADIN_ITEM_PVP_HOLY_4P_BONUS              = 131665,
    PALADIN_SPELL_GLYPH_OF_DENOUNCE             = 56420,
    PALADIN_SPELL_GLYPH_OF_DENOUNCE_PROC        = 115654,
    PALADIN_SPELL_GLYPH_OF_DEVOTION_AURA        = 146955,
    PALADIN_SPELL_AVENGING_WRATH                = 31884,
    PALADIN_SPELL_AVENGING_WRATH_HEAL           = 31842,
    PALADIN_SPELL_EXORCISM_ENERGIZE             = 147715,
    PALADIN_SPELL_GLYPH_OF_BURDEN_OF_GUILT      = 54931,
    PALADIN_SPELL_BURDEN_OF_GUILD               = 110300,
    PALADIN_SPELL_SEAL_OF_JUSTICE               = 20164,
    PALADIN_SPELL_SEAL_OF_INSIHT                = 20165,
    PALADIN_SPELL_SEAL_OF_RIGHTEOUSNESS         = 20154,
    PALADIN_SPELL_SEAL_OF_TRUTH                 = 31801,
    PALADIN_SPELL_EMPOWERED_SEALS               = 152263,
    PALADIN_SPELL_TURALYONS_JUSTICE             = 156987,
    PALADIN_SPELL_UTHERS_INSIGHT                = 156988,
    PALADIN_SPELL_LIADRINS_RIGHTEOUSNESS        = 156989,
    PALADIN_SPELL_MARAADS_TRUTH                 = 156990,
    PALADIN_SPELL_ETERNAL_FLAME_PERIODIC_HEAL   = 156322,
    PALADIN_SPELL_ETERNAL_FLAME                 = 114163,
    PALADIN_SPELL_HAMMER_OF_WRATH               = 24275,
    PALADIN_SPELL_SANCTIFIED_WRATH_PROTECTION   = 171648,
    PALADIN_SPELL_EMPOWERED_DIVINE_STORM        = 174718,
    PALADIN_SPELL_DIVINE_CRUSADER               = 144595,
    PALADIN_ENHANCED_HOLY_SHOCK_PROC            = 160002,
    PALADIN_SPELL_SHIELD_OF_THE_RIGHTEOUS       = 53600,
    PALADIN_SPELL_LIGHT_OF_DAWN                 = 85222,
    PALADIN_SPELL_DIVINE_PURPOSE                = 86172,
    PALADIN_SPELL_HAMMER_OF_WRATH_POWER         = 141459,
    PALADIN_SPELL_GLYPH_OF_TEMPLAR_VERDICT      = 54926,
    PALADIN_SPELL_GLYPH_OF_TEMPLAR_VERDICT_PROC = 115668,
    PALADIN_SPELL_GLYPH_OF_DIVINE_SHIELD        = 146956,
    PALADIN_SPELL_IMPROVED_DAYBREAK             = 157455,
    PALADIN_SPELL_FLASH_OF_LIGHT                = 19750,
    PALADIN_SPELL_HOLY_LIGHT                    = 13952,
    PALADIN_NPC_LIGHTS_HAMMER                   = 59738,
    PALADIN_SPELL_LIGHTS_HAMMER_TICK            = 114918,
    PALADIN_PVP_RETRIBUTION_2P_BONUS            = 165886,
    PALADIN_SPELL_RIGHTEOUS_DETERMINATION       = 165889,
    PALADIN_PVP_RETRIBUTION_4P_BONUS            = 165895,
    PALADIN_VINDICATORS_FURY                    = 165903,
    PALADIN_HOLY_POWER_ENERGIZE                 = 138248
};

/// Hammer of Wrath - 24275 - 158392 - 157496
class spell_pal_hammer_of_wrath: public SpellScriptLoader
{
    public:
        spell_pal_hammer_of_wrath() : SpellScriptLoader("spell_pal_hammer_of_wrath") { }

        class spell_pal_hammer_of_wrath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hammer_of_wrath_SpellScript);

            enum eSpells
            {
                T17Retribution4P    = 165439,
                BlazingContempt     = 166831
            };

            bool m_EnergizeAlreadyApply = false;

            void HandleAfterHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (GetHitDamage() + GetAbsorbedDamage() <= 0 || m_EnergizeAlreadyApply)
                    return;

                if (l_Player->GetActiveSpecializationID() != SPEC_PALADIN_RETRIBUTION)
                    return;

                l_Player->CastSpell(l_Player, PALADIN_SPELL_HAMMER_OF_WRATH_POWER, true);
                m_EnergizeAlreadyApply = true;

                if (l_Player->HasAura(eSpells::T17Retribution4P))
                    l_Player->CastSpell(l_Player, eSpells::BlazingContempt, true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_pal_hammer_of_wrath_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_hammer_of_wrath_SpellScript();
        }
};

/// Last Update 7.0.3
/// Shield of the Righteous - 53600
class spell_pal_shield_of_the_righteous: public SpellScriptLoader
{
    public:
        spell_pal_shield_of_the_righteous() : SpellScriptLoader("spell_pal_shield_of_the_righteous") { }

        class spell_pal_shield_of_the_righteous_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_shield_of_the_righteous_SpellScript);

            enum eSpells
            {
                RighteousProtector  = 204074,
                LightoftheProtector = 184092,
                HandOfTheProtector  = 213652,
                AvengingWrath       = 31884
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                int32 l_PreviousDuration = 0;

                if (Aura* l_Aura = l_Caster->GetAura(PALADIN_SPELL_SHIELD_OF_THE_RIGHTEOUS_PROC))
                    l_PreviousDuration = l_Aura->GetDuration();

                int32 l_StartPercent = 0;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(PALADIN_SPELL_SHIELD_OF_THE_RIGHTEOUS_PROC))
                    l_StartPercent = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                int32 l_Bp = G3D::iClamp(l_StartPercent, -80, -25);

                l_Caster->CastCustomSpell(l_Caster, PALADIN_SPELL_SHIELD_OF_THE_RIGHTEOUS_PROC, &l_Bp, NULL, NULL, true);

                if (Aura* l_Aura = l_Caster->GetAura(PALADIN_SPELL_SHIELD_OF_THE_RIGHTEOUS_PROC))
                    l_Aura->SetDuration(l_Aura->GetDuration() + l_PreviousDuration);

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::RighteousProtector, EFFECT_0))
                {
                    Player* l_Player = l_Caster->ToPlayer();
                    SpellInfo const* l_SpellInfoLOTP = sSpellMgr->GetSpellInfo(eSpells::LightoftheProtector);
                    SpellInfo const* l_SpellInfoAW = sSpellMgr->GetSpellInfo(eSpells::AvengingWrath);
                    SpellInfo const* l_SpellInfoHOTP = sSpellMgr->GetSpellInfo(eSpells::HandOfTheProtector);
                    if (!l_Player || !l_SpellInfoLOTP || !l_SpellInfoAW || !l_SpellInfoHOTP)
                        return;

                    SpellCategoryEntry const* l_ChargeCategoryEntryLOTP = l_SpellInfoLOTP->ChargeCategoryEntry;
                    SpellCategoryEntry const* l_ChargeCategoryEntryAW = l_SpellInfoAW->ChargeCategoryEntry;
                    SpellCategoryEntry const* l_ChargeCategoryEntryHOTP = l_SpellInfoHOTP->ChargeCategoryEntry;
                    if (!l_ChargeCategoryEntryLOTP || !l_ChargeCategoryEntryAW || !l_ChargeCategoryEntryHOTP)
                        return;

                    l_Player->ReduceChargeCooldown(l_ChargeCategoryEntryLOTP, l_AuraEffect->GetAmount() * IN_MILLISECONDS);
                    l_Player->ReduceChargeCooldown(l_ChargeCategoryEntryAW, l_AuraEffect->GetAmount() * IN_MILLISECONDS);
                    l_Player->ReduceChargeCooldown(l_ChargeCategoryEntryHOTP, l_AuraEffect->GetAmount() * IN_MILLISECONDS);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_shield_of_the_righteous_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_shield_of_the_righteous_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Selfless healer - 85804
/// Called by flash of light - 19750
class spell_pal_selfless_healer: public SpellScriptLoader
{
    public:
        spell_pal_selfless_healer() : SpellScriptLoader("spell_pal_selfless_healer") { }

        class spell_pal_selfless_healer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_selfless_healer_SpellScript);

            void HandleHeal(SpellEffIndex /*l_EffIndex*/)
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Player == nullptr || l_Target == nullptr)
                    return;

                if (l_Player->HasAura(PALADIN_SPELL_SELFLESS_HEALER_STACK))
                {
                    int32 l_Charges = l_Player->GetAura(PALADIN_SPELL_SELFLESS_HEALER_STACK)->GetStackAmount();

                    if (l_Player->IsValidAssistTarget(l_Target) && l_Target != l_Player)
                    {
                        uint32 l_Heal = 0;

                        if (l_Player->GetActiveSpecializationID() == SPEC_PALADIN_HOLY)
                            l_Heal = int32(GetHitHeal() + ((GetHitHeal() * sSpellMgr->GetSpellInfo(PALADIN_SPELL_SELFLESS_HEALER_STACK)->Effects[EFFECT_3].BasePoints / 100) * l_Charges));
                        else
                            l_Heal = int32(GetHitHeal() + ((GetHitHeal() * sSpellMgr->GetSpellInfo(PALADIN_SPELL_SELFLESS_HEALER_STACK)->Effects[EFFECT_1].BasePoints / 100) * l_Charges));

                        SetHitHeal(l_Heal);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_selfless_healer_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_selfless_healer_SpellScript();
        }
};

/// 7.1.5 - Build 23420
/// Called by Blinding Light - 115750
class spell_pal_blinding_light: public SpellScriptLoader
{
    public:
        spell_pal_blinding_light() : SpellScriptLoader("spell_pal_blinding_light") { }

        class spell_pal_blinding_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blinding_light_SpellScript);

            enum eSpells
            {
                BlindingLightProc   = 105421
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->CastSpell(l_Target, eSpells::BlindingLightProc, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pal_blinding_light_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_blinding_light_SpellScript();
        }
};

/// Hand of Protection - 1022
class spell_pal_hand_of_protection: public SpellScriptLoader
{
    public:
        spell_pal_hand_of_protection() : SpellScriptLoader("spell_pal_hand_of_protection") { }

        enum eSpells
        {
            Asphyxiate = 108194,
            AsphyxiateBlood = 221562
        };

        class spell_pal_hand_of_protection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hand_of_protection_SpellScript);

            SpellCastResult CheckForbearance()
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetExplTargetUnit())
                    if (target->HasAura(SPELL_FORBEARANCE))
                        return SPELL_FAILED_TARGET_AURASTATE;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        l_Player->CastSpell(l_Target, SPELL_FORBEARANCE, true);

                        if (l_Target->HasAura(eSpells::Asphyxiate) || l_Target->HasAura(eSpells::AsphyxiateBlood))
                        {
                            l_Target->RemoveAura(eSpells::Asphyxiate);
                            l_Target->RemoveAura(eSpells::AsphyxiateBlood);
                        }
                    }
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_hand_of_protection_SpellScript::CheckForbearance);
                OnHit += SpellHitFn(spell_pal_hand_of_protection_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_hand_of_protection_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Cleanse - 4987
class spell_pal_cleanse: public SpellScriptLoader
{
    public:
        spell_pal_cleanse() : SpellScriptLoader("spell_pal_cleanse") { }

        class spell_pal_cleanse_SpellScript : public SpellScript
        {
            enum eSpells
            {
                CleanseTheWeak          = 199330,
                CleanseTheWeakDispel    = 199360
            };

            PrepareSpellScript(spell_pal_cleanse_SpellScript);

            SpellCastResult CheckCleansing()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetExplTargetUnit())
                    {
                        DispelChargesList dispelList;

                        // Create dispel mask by dispel type
                        SpellInfo const* l_SpellInfo = GetSpellInfo();
                        for (uint8 i = 0; i < l_SpellInfo->EffectCount; ++i)
                        {
                            if (l_SpellInfo->Effects[i].IsEffect())
                            {
                                uint32 dispel_type = l_SpellInfo->Effects[i].MiscValue;
                                uint32 dispelMask = l_SpellInfo->GetDispelMask(DispelType(dispel_type));

                                // Epuration can dispell Magic with Sacred Cleansing
                                if (dispelMask == DISPEL_MAGIC && !caster->HasAura(PALADIN_SPELL_SACRED_CLEANSING) && GetSpellInfo()->Id == 4987)
                                    continue;

                                target->GetDispellableAuraList(caster, dispelMask, dispelList);
                            }
                        }
                        if (dispelList.empty())
                            return SPELL_FAILED_NOTHING_TO_DISPEL;

                        return SPELL_CAST_OK;
                    }
                }

                return SPELL_CAST_OK;
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Spell* l_Spell = GetSpell();
                if (!l_Caster || !l_Spell)
                    return;

                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    l_Target = l_Caster;

                if (l_Caster->HasAura(eSpells::CleanseTheWeak))
                {
                    l_Spell->cancel();
                    l_Caster->CastSpell(l_Target, eSpells::CleanseTheWeakDispel, false);
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_cleanse_SpellScript::CheckCleansing);
                BeforeCast += SpellCastFn(spell_pal_cleanse_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_cleanse_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Divine Shield - 642 and Divine Shield - 110700
class spell_pal_divine_shield: public SpellScriptLoader
{
    public:
        spell_pal_divine_shield() : SpellScriptLoader("spell_pal_divine_shield") { }

        enum eSpells
        {
            Cyclone     = 33786,
            Imprison    = 221527,
            DiamondIce  = 203337
        };

        class spell_pal_divine_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_shield_SpellScript);

            uint8 m_CoutNegativeAura = 0;

            SpellCastResult CheckForbearance()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(SPELL_FORBEARANCE))
                        return SpellCastResult::SPELL_FAILED_TARGET_AURASTATE;

                    if (l_Caster->HasAura(eSpells::Cyclone))
                    {
                        SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_DO_WHILE_CYCYLONED);
                        return SPELL_FAILED_CUSTOM_ERROR;
                    }

                    l_Caster->RemoveAura(eSpells::Imprison);
                    l_Caster->RemoveAura(eSpells::DiamondIce);
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, SPELL_FORBEARANCE, true);
            }

            void HandleBeforeHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(PALADIN_SPELL_GLYPH_OF_DIVINE_SHIELD))
                    {
                        Unit::AuraApplicationMap l_AppliedAuras = l_Caster->GetAppliedAuras();
                        for (Unit::AuraApplicationMap::iterator itr = l_AppliedAuras.begin(); itr != l_AppliedAuras.end(); itr++)
                        {
                            AuraApplication const* l_AurApp = itr->second;
                            Aura const* aura = l_AurApp->GetBase();
                            if (!l_AurApp->IsPositive()
                                && !aura->IsPassive()
                                && !aura->IsDeathPersistent())
                                m_CoutNegativeAura++;
                        }
                    }

                }
            }

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(PALADIN_SPELL_GLYPH_OF_DIVINE_SHIELD))
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(PALADIN_SPELL_GLYPH_OF_DIVINE_SHIELD);

                        if (l_SpellInfo)
                        {
                            if (m_CoutNegativeAura > l_SpellInfo->Effects[EFFECT_1].BasePoints)
                                m_CoutNegativeAura = l_SpellInfo->Effects[EFFECT_1].BasePoints;

                            SetHitHeal((l_Caster->GetMaxHealth() * (m_CoutNegativeAura * l_SpellInfo->Effects[EFFECT_0].BasePoints)) / 100);
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_divine_shield_SpellScript::CheckForbearance);
                BeforeHit += SpellHitFn(spell_pal_divine_shield_SpellScript::HandleBeforeHit);
                OnEffectHitTarget += SpellEffectFn(spell_pal_divine_shield_SpellScript::HandleHeal, EFFECT_3, SPELL_EFFECT_HEAL_PCT);
                OnHit += SpellHitFn(spell_pal_divine_shield_SpellScript::HandleOnHit);
            }
        };

        class spell_pal_divine_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_divine_shield_AuraScript);

            enum eSpells
            {
                FinalStand      = 204077,
                FinalStandTaunt = 204079
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(eSpells::FinalStand))
                    l_Target->CastSpell(l_Target, eSpells::FinalStandTaunt, true);


                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, true);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_STUN, true);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_KNOCKOUT, true);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, true);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE, true);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE, true);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, false);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, false);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, false);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_STUN, false);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_KNOCKOUT, false);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, false);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE, false);
                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE, false);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_divine_shield_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_divine_shield_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_divine_shield_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_divine_shield_SpellScript();
        }
};

/// Light's Hammer (periodic dummy for npc) - 114918
class spell_pal_lights_hammer_tick: public SpellScriptLoader
{
    public:
        spell_pal_lights_hammer_tick() : SpellScriptLoader("spell_pal_lights_hammer_tick") { }

        class spell_pal_lights_hammer_tick_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_lights_hammer_tick_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit *l_Caster = GetCaster())
                {
                    if (l_Caster->GetOwner())
                    {
                        l_Caster->CastSpell(l_Caster->GetPositionX(), l_Caster->GetPositionY(), l_Caster->GetPositionZ(), PALADIN_SPELL_ARCING_LIGHT_HEAL, true, 0, nullptr, l_Caster->GetOwner()->GetGUID());
                        l_Caster->CastSpell(l_Caster->GetPositionX(), l_Caster->GetPositionY(), l_Caster->GetPositionZ(), PALADIN_SPELL_ARCING_LIGHT_DAMAGE, true, 0, nullptr, l_Caster->GetOwner()->GetGUID());
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_lights_hammer_tick_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_lights_hammer_tick_AuraScript();
        }
};

/// Light's Hammer - 122773
class spell_pal_lights_hammer : public SpellScriptLoader
{
public:
    spell_pal_lights_hammer() : SpellScriptLoader("spell_pal_lights_hammer") { }

    class spell_pal_lights_hammer_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_lights_hammer_SpellScript);

        void HandleAfterCast()
        {
            if (Unit* l_Caster = GetCaster()->ToPlayer())
            {
                std::list<Creature*> l_TempList;
                std::list<Creature*> l_LightsHammerlist;

                l_Caster->GetCreatureListWithEntryInGrid(l_LightsHammerlist, PALADIN_NPC_LIGHTS_HAMMER, 200.0f);

                l_TempList = l_LightsHammerlist;

                for (std::list<Creature*>::iterator i = l_TempList.begin(); i != l_TempList.end(); ++i)
                {
                    Unit* l_Owner = (*i)->GetOwner();
                    if (l_Owner != nullptr && l_Owner->GetGUID() == l_Caster->GetGUID() && (*i)->isSummon())
                        continue;

                    l_LightsHammerlist.remove((*i));
                }

                for (std::list<Creature*>::iterator itr = l_LightsHammerlist.begin(); itr != l_LightsHammerlist.end(); ++itr)
                {
                    (*itr)->CastSpell((*itr), PALADIN_SPELL_LIGHTS_HAMMER_TICK, true);
                    (*itr)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    (*itr)->setFaction(l_Caster->getFaction());
                    (*itr)->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, GetSpellInfo()->Id);
                }
            }
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_pal_lights_hammer_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_lights_hammer_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Holy Prism - 114165
class spell_pal_holy_prism : public SpellScriptLoader
{
public:
    spell_pal_holy_prism() : SpellScriptLoader("spell_pal_holy_prism") { }

    class spell_pal_holy_prism_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_holy_prism_SpellScript);

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();

            if (l_Target == nullptr)
                return;

            if (l_Caster->IsValidAttackTarget(l_Target))
            {
                l_Caster->CastSpell(l_Target, PALADIN_SPELL_HOLY_PRISM_DAMAGE_VISUAL_2, true);
            }
            else
            {
                if (!l_Caster->IsValidAssistTarget(l_Target))
                    l_Target = l_Caster;

                l_Caster->CastSpell(l_Target, PALADIN_SPELL_HOLY_PRISM_HEAL_VISUAL_2, true);
            }
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_pal_holy_prism_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_holy_prism_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Holy Prism - Casted by 114870 / 121552
class spell_pal_holy_prism_visual_effect : public SpellScriptLoader
{
public:
    spell_pal_holy_prism_visual_effect() : SpellScriptLoader("spell_pal_holy_prism_visual_effect") { }

    class spell_pal_holy_prism_visual_effect_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_holy_prism_visual_effect_SpellScript);

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();

            if (l_Target == nullptr)
                return;

            if (GetSpellInfo()->Id == PALADIN_SPELL_HOLY_PRISM_DAMAGE_VISUAL_2)
                l_Caster->CastSpell(l_Target, PALADIN_SPELL_HOLY_PRISM_ENEMIES, true);
            else if (GetSpellInfo()->Id == PALADIN_SPELL_HOLY_PRISM_HEAL_VISUAL_2)
                l_Caster->CastSpell(l_Target, PALADIN_SPELL_HOLY_PRISM_ALLIES, true);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_pal_holy_prism_visual_effect_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_holy_prism_visual_effect_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// called by Holy Prism (damage) - 114852 or Holy Prism (heal) - 114871
/// Holy Prism visual for other targets
class spell_pal_holy_prism_visual: public SpellScriptLoader
{
    public:
        spell_pal_holy_prism_visual() : SpellScriptLoader("spell_pal_holy_prism_visual") { }

        class spell_pal_holy_prism_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_holy_prism_visual_SpellScript);

            enum eSpells
            {
                HolyPrismHeal = 114871,
                HolyPrismDamage = 114852,
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster      = GetCaster();
                if (l_Caster && GetSpellInfo()->Id == eSpells::HolyPrismHeal)
                {
                    p_Targets.remove_if([l_Caster](WorldObject* p_WorldObject) -> bool
                    {
                        if (p_WorldObject == nullptr)
                            return true;

                        if (!l_Caster->IsValidAttackTarget(static_cast<Unit*>(p_WorldObject)))
                            return true;

                        return false;
                    });
                }

                /// Select up to 5 allies or enemies
                if (p_Targets.size() > 5)
                    JadeCore::RandomResizeList(p_Targets, 5);
            }

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Player->IsValidAttackTarget(l_Target))
                            l_Player->CastSpell(l_Target, PALADIN_SPELL_HOLY_PRISM_DAMAGE_VISUAL, true);
                        else if (l_Target != l_Player)
                            l_Player->CastSpell(l_Target, PALADIN_SPELL_HOLY_PRISM_HEAL_VISUAL, true);
                    }
                }
            }

            void Register()
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::HolyPrismHeal:
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_prism_visual_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                        break;
                    case eSpells::HolyPrismDamage:
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_prism_visual_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
                        break;
                    default:
                        break;
                }
                OnHit += SpellHitFn(spell_pal_holy_prism_visual_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_holy_prism_visual_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Judgment - 20271
class spell_pal_judgment: public SpellScriptLoader
{
    public:
        spell_pal_judgment() : SpellScriptLoader("spell_pal_judgment") { }

        class spell_pal_judgment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_judgment_SpellScript);

            enum eSpells
            {
                ShieldoftheRighteous        = 53600,
                JudgmentMasteryDebuff       = 197277,
                MasteryDivineJudgment       = 76672,
                KnightOfTheSilverHandAura   = 200302,
                KnightOfTheSilverHandProc   = 211422,
                JudgmentProtection          = 231657,
                JudgmentHoly                = 214222,
                JudgmentRetribution         = 197277,
                JudgementRetributionTarget  = 231661,
                JudgmentAdditional          = 228288,
                GreaterJudgment             = 218178,
                LawbringerHonorTalentAura   = 246806,
                LawbringerHonorTalentSpell  = 246807,
                LawbringerHonorTalentDamage = 246867,
                Tier21PRetributionBonus     = 251868,
                SacredJudgement             = 253806,
            };

            enum eArtifactPowers
            {
                PowerOfTheSilverHand = 974
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                Spell* l_Spell = GetSpell();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GreaterJudgment);
                if (!l_Caster || !l_Target || !l_Spell || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::GreaterJudgment) && l_Target->GetHealthPct() > l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    l_Spell->SetCustomCritChance(100);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(PALADIN_SPELL_LONG_ARM_OF_THE_LAW))
                    l_Caster->CastSpell(l_Caster, PALADIN_SPELL_LONG_ARM_OF_THE_LAW_RUN_SPEED, true);
                if (l_Caster->HasAura(PALADIN_SPELL_GLYPH_OF_BURDEN_OF_GUILT))
                    l_Caster->CastSpell(l_Target, PALADIN_SPELL_BURDEN_OF_GUILD, true);

                if (l_Caster->HasAura(PALADIN_SPELL_EMPOWERED_SEALS))
                {
                    if (l_Caster->HasAura(PALADIN_SPELL_SEAL_OF_JUSTICE))
                        l_Caster->CastSpell(l_Caster, PALADIN_SPELL_TURALYONS_JUSTICE, true);
                    else if (l_Caster->HasAura(PALADIN_SPELL_SEAL_OF_INSIHT))
                        l_Caster->CastSpell(l_Caster, PALADIN_SPELL_UTHERS_INSIGHT, true);
                    else if (l_Caster->HasAura(PALADIN_SPELL_SEAL_OF_RIGHTEOUSNESS))
                        l_Caster->CastSpell(l_Caster, PALADIN_SPELL_LIADRINS_RIGHTEOUSNESS, true);
                    else if (l_Caster->HasAura(PALADIN_SPELL_SEAL_OF_TRUTH))
                        l_Caster->CastSpell(l_Caster, PALADIN_SPELL_MARAADS_TRUTH, true);
                }

                if (l_Caster->HasAura(eSpells::MasteryDivineJudgment))
                {
                    if (l_Target->HasAura(eSpells::JudgmentMasteryDebuff))
                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::Lawbringer].insert(l_Target->GetGUID());

                    l_Caster->CastSpell(l_Target, eSpells::JudgmentMasteryDebuff, true);
                }

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::KnightOfTheSilverHandAura))
                    {
                        SpellInfo const* l_SilverHandInfo = sSpellMgr->GetSpellInfo(eSpells::KnightOfTheSilverHandProc);

                        if (l_SilverHandInfo)
                        {
                            uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::PowerOfTheSilverHand);
                            int32 l_DamageReduction = l_SilverHandInfo->Effects[0].BasePoints * l_Rank;

                            l_Player->CastCustomSpell(l_Player, eSpells::KnightOfTheSilverHandProc, &l_DamageReduction, nullptr, nullptr, true);
                        }
                    }
                }

                if (l_Caster->HasAura(eSpells::LawbringerHonorTalentAura))
                {
                    if (l_Target->HasAura(eSpells::LawbringerHonorTalentSpell, l_Caster->GetGUID()))
                        l_Caster->CastSpell(l_Target, eSpells::LawbringerHonorTalentDamage, true);
                    else
                        l_Caster->CastSpell(l_Target, eSpells::LawbringerHonorTalentSpell, true);
                }
            }

            void HandleSpecializationOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                switch (l_Player->GetActiveSpecializationID())
                {
                    /// And reducing the remaining cooldown on Shield of the Righteous by 2 sec, or 4 sec on a critical strike
                    case SPEC_PALADIN_PROTECTION:
                    {
                        SpellInfo const* l_SpellInfoShieldoftheRighteous = sSpellMgr->GetSpellInfo(eSpells::ShieldoftheRighteous);
                        SpellInfo const* l_SpellInfoShieldoftheRighteousCool = sSpellMgr->GetSpellInfo(eSpells::JudgmentProtection);
                        if (!l_SpellInfoShieldoftheRighteous || !l_SpellInfoShieldoftheRighteousCool)
                            return;

                        SpellCategoryEntry const* l_CategoryEntry = l_SpellInfoShieldoftheRighteous->ChargeCategoryEntry;
                        if (!l_CategoryEntry)
                            return;

                        uint8 l_ReduceBp = l_SpellInfoShieldoftheRighteousCool->Effects[EFFECT_0].BasePoints;

                        Spell* l_Spell = GetSpell();
                        if (!l_Spell)
                            return;

                        if (l_Spell->IsCritForTarget(l_Target))
                            l_ReduceBp *= 2;

                        l_Player->ReduceChargeCooldown(l_CategoryEntry, l_ReduceBp * IN_MILLISECONDS);
                        break;
                    }
                    case SPEC_PALADIN_HOLY: ///< and causing the target to take 30% increased damage from your Crusader Strike and Holy Shock for 6 sec
                        l_Player->CastSpell(l_Target, eSpells::JudgmentHoly, true);
                        break;
                    case SPEC_PALADIN_RETRIBUTION:
                    {
                        l_Caster->CastSpell(l_Target, eSpells::JudgmentRetribution, true);

                        SpellInfo const* l_SpellInfoJudgementRetributionTarget = sSpellMgr->GetSpellInfo(eSpells::JudgementRetributionTarget);
                        SpellInfo const* l_SpellInfoGreaterJudgment = sSpellMgr->GetSpellInfo(eSpells::GreaterJudgment);
                        if (!l_SpellInfoJudgementRetributionTarget || !l_SpellInfoGreaterJudgment)
                            return;

                        uint8 l_NbAdditionalsTargets = 0;
                        if (!l_Caster->HasAura(eSpells::GreaterJudgment))
                            l_NbAdditionalsTargets = l_SpellInfoJudgementRetributionTarget->Effects[EFFECT_0].BasePoints;
                        else
                            l_NbAdditionalsTargets = l_SpellInfoJudgementRetributionTarget->Effects[EFFECT_0].BasePoints + l_SpellInfoGreaterJudgment->Effects[EFFECT_1].BasePoints;

                        float l_Radius = 8.0f;
                        std::list<Unit*> l_AdditionalsTargets;
                        JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(l_Target, l_Caster, l_Radius);
                        JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(l_Caster, l_AdditionalsTargets, u_check);
                        l_Caster->VisitNearbyObject(l_Radius, searcher);
                        if (l_AdditionalsTargets.empty())
                            return;

                        uint64 l_TargetGUID = l_Target->GetGUID();
                        l_AdditionalsTargets.remove_if([=](Unit* l_TargetInList) -> bool
                        {
                            if (!l_TargetInList)
                                return true;

                            if (l_TargetInList->GetGUID() == l_TargetGUID)
                                return true;

                            if (!l_Caster->IsValidAttackTarget(l_TargetInList))
                                return true;

                            return false;
                        });

                        if (l_AdditionalsTargets.empty())
                            return;

                        l_AdditionalsTargets.sort(JadeCore::WorldObjectDistanceCompareOrderPred(l_Target));
                        if (l_AdditionalsTargets.size() > l_NbAdditionalsTargets)
                            l_AdditionalsTargets.resize(l_NbAdditionalsTargets);

                        bool l_hasLawbringerTalent = l_Caster->HasAura(eSpells::LawbringerHonorTalentAura);
                        for (auto l_AdditionalTarget : l_AdditionalsTargets)
                        {
                            l_Caster->CastSpell(l_AdditionalTarget, eSpells::JudgmentAdditional, true);
                            l_Caster->CastSpell(l_AdditionalTarget, eSpells::JudgmentRetribution, true);

                            if (l_hasLawbringerTalent)
                            {
                                if (l_AdditionalTarget->HasAura(eSpells::LawbringerHonorTalentSpell, l_Caster->GetGUID()))
                                    l_Caster->CastSpell(l_AdditionalTarget, eSpells::LawbringerHonorTalentDamage, true);
                                else
                                    l_Caster->CastSpell(l_AdditionalTarget, eSpells::LawbringerHonorTalentSpell, true);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Tier21PRetributionBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::SacredJudgement, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_pal_judgment_SpellScript::HandleOnPrepare);
                OnHit += SpellHitFn(spell_pal_judgment_SpellScript::HandleOnHit);
                OnHit += SpellHitFn(spell_pal_judgment_SpellScript::HandleSpecializationOnHit);
                AfterCast += SpellCastFn(spell_pal_judgment_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_judgment_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Lawbringer Honor Talent - 246867
class spell_pal_lawbringer_honor : public SpellScriptLoader
{
public:
    spell_pal_lawbringer_honor() : SpellScriptLoader("spell_pal_lawbringer_honor") { }

    class spell_pal_lawbringer_honor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_lawbringer_honor_SpellScript);

        enum eSpells
        {
            LawbringerDebuff = 246807
        };

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            SpellInfo const* l_SpellInfo = GetSpellInfo();

            if (!l_Caster || !l_SpellInfo || !l_Target)
                return;

            if (Creature* l_Creature = l_Target->ToCreature())
            {
                if (CreatureTemplate const* l_CreatureTemplate = l_Creature->GetCreatureTemplate())
                {
                    if (l_CreatureTemplate->rank > CreatureEliteType::CREATURE_ELITE_NORMAL)
                    {
                        SetHitDamage(int32((float)l_Caster->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints) / 100.0f));
                    }
                    else
                    {
                        SetHitDamage(int32((float)l_Target->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints) / 100.0f));
                    }
                }
            }
            else
                SetHitDamage(int32((float)l_Target->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints) / 100.0f));
        }

        void HandleSelectTarget(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const *l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_SpellInfo || !l_SpellInfo->RangeEntry)
                return;

            p_Targets.remove_if([l_Caster, l_SpellInfo](WorldObject* l_Target) -> bool
            {
                Unit* l_UnitTarget = l_Target->ToUnit();
                if (!l_UnitTarget)
                    return true;

                if (l_UnitTarget->GetDistance(l_Caster) > l_SpellInfo->RangeEntry->maxRangeHostile)
                    return true;

                if (l_UnitTarget->HasBreakOnDamageCrowdControlAura() || l_UnitTarget->isInRoots())
                    return true;

                if (l_UnitTarget->HasAura(eSpells::LawbringerDebuff, l_Caster->GetGUID()))
                    return false;

                return true;
            });
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_lawbringer_honor_SpellScript::HandleSelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnHit += SpellHitFn(spell_pal_lawbringer_honor_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pal_lawbringer_honor_SpellScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Called by Judgment (Additional) - 228288
/// Called for Judgment - 20271
class spell_pal_judgment_additional: public SpellScriptLoader
{
    public:
        spell_pal_judgment_additional() : SpellScriptLoader("spell_pal_judgment_additional") { }

        class spell_pal_judgment_additional_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_judgment_additional_SpellScript);

            enum eSpells
            {
                GreaterJudgment = 218178
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                Spell* l_Spell = GetSpell();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GreaterJudgment);
                if (!l_Caster || !l_Target || !l_Spell || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::GreaterJudgment) && l_Target->GetHealthPct() > l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    l_Spell->SetCustomCritChance(100);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_pal_judgment_additional_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_judgment_additional_SpellScript();
        }
};

/// Last Update 6.2.3
/// Empowered Seals - 152263
/// Call by Turalyons Justice - 156987, Uthers Insight - 156988, Liadrins Righteousness - 156989, Maraads Truth - 156990
class spell_pal_empowered_seals : public SpellScriptLoader
{
    public:
        spell_pal_empowered_seals() : SpellScriptLoader("spell_pal_empowered_seals") { }

        enum eSpells
        {
            TuralyonsJustice        = 156987,
            UthersInsight           = 156988,
            LiadrinsRighteousness   = 156989,
            MaraadsTruth            = 156990,
            EmpowredSealsVisual     = 172319
        };

        class spell_pal_empowered_seals_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_empowered_seals_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();

                l_Target->CastSpell(l_Target, eSpells::EmpowredSealsVisual, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();
                bool m_HasStillAura = false;

                for (uint32 l_I = eSpells::TuralyonsJustice; l_I <= eSpells::MaraadsTruth; ++l_I)
                {
                    if (l_Target->HasAura(l_I))
                        m_HasStillAura = true;
                }
                if (!m_HasStillAura)
                    l_Target->RemoveAura(eSpells::EmpowredSealsVisual);
            }

            void Register()
            {
                switch (m_scriptSpellId)
                {
                case eSpells::TuralyonsJustice:
                    AfterEffectApply += AuraEffectApplyFn(spell_pal_empowered_seals_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_SPEED_ALWAYS, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_pal_empowered_seals_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_SPEED_ALWAYS, AURA_EFFECT_HANDLE_REAL);
                    break;
                case eSpells::UthersInsight:
                    AfterEffectApply += AuraEffectApplyFn(spell_pal_empowered_seals_AuraScript::OnApply, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_pal_empowered_seals_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
                    break;
                case eSpells::LiadrinsRighteousness:
                    AfterEffectApply += AuraEffectApplyFn(spell_pal_empowered_seals_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_pal_empowered_seals_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
                    break;
                case eSpells::MaraadsTruth:
                    AfterEffectApply += AuraEffectApplyFn(spell_pal_empowered_seals_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_ATTACK_POWER_PCT, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_pal_empowered_seals_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_ATTACK_POWER_PCT, AURA_EFFECT_HANDLE_REAL);
                    break;
                default:
                    break;
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_empowered_seals_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Ardent Defender - 31850
class spell_pal_ardent_defender: public SpellScriptLoader
{
    public:
        spell_pal_ardent_defender() : SpellScriptLoader("spell_pal_ardent_defender") { }

        class spell_pal_ardent_defender_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_ardent_defender_AuraScript);

            enum eSpells
            {
                DefenderOfTruthAura  = 238097,
                DefenderOfTruth      = 240059
            };

            int32 m_AbsorbPct, m_HealPct;

            bool Load() override
            {
                m_HealPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                m_AbsorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
                return GetUnitOwner()->IsPlayer();
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & canBeRecalculate)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* p_AurEff, DamageInfo & p_DmgInfo, uint32 & p_AbsorbAmount)
            {
                Unit* l_Victim = GetTarget();

                if (l_Victim == nullptr)
                    return;

                int32 l_RemainingHealth =  l_Victim->GetHealth() - p_DmgInfo.GetAmount();
                // If damage kills us
                if (l_RemainingHealth <= 0 && !l_Victim->ToPlayer()->HasSpellCooldown(PALADIN_SPELL_ARDENT_DEFENDER_HEAL))
                {
                    // Cast healing spell, completely avoid damage
                    p_AbsorbAmount = p_DmgInfo.GetAmount();

                    int32 l_HealAmount = int32(l_Victim->CountPctFromMaxHealth(m_HealPct));
                    l_Victim->CastCustomSpell(l_Victim, PALADIN_SPELL_ARDENT_DEFENDER_HEAL, &l_HealAmount, NULL, NULL, true, NULL, p_AurEff);
                    l_Victim->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                }
                else
                    p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), m_AbsorbPct);
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DefenderOfTruthAura);
                if (!l_Caster || !l_Caster->HasAura(eSpells::DefenderOfTruthAura) ||!l_SpellInfo)
                    return;

                int32 l_Shield = l_Caster->GetMaxHealth(l_Caster);
                l_Shield = CalculatePct(l_Shield, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                l_Caster->CastCustomSpell(l_Caster, eSpells::DefenderOfTruth, &l_Shield, nullptr, nullptr, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_ardent_defender_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_ardent_defender_AuraScript::Absorb, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_ardent_defender_AuraScript::HandleRemove, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_ardent_defender_AuraScript();
        }
};

/// Blessing of faith - 37877
class spell_pal_blessing_of_faith: public SpellScriptLoader
{
    public:
        spell_pal_blessing_of_faith() : SpellScriptLoader("spell_pal_blessing_of_faith") { }

        class spell_pal_blessing_of_faith_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blessing_of_faith_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_DRUID) || !sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_PALADIN) || !sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_PRIEST) || !sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_SHAMAN))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    uint32 spell_id = 0;
                    switch (unitTarget->getClass())
                    {
                        case CLASS_DRUID:   spell_id = SPELL_BLESSING_OF_LOWER_CITY_DRUID; break;
                        case CLASS_PALADIN: spell_id = SPELL_BLESSING_OF_LOWER_CITY_PALADIN; break;
                        case CLASS_PRIEST:  spell_id = SPELL_BLESSING_OF_LOWER_CITY_PRIEST; break;
                        case CLASS_SHAMAN:  spell_id = SPELL_BLESSING_OF_LOWER_CITY_SHAMAN; break;
                        default: return;                    // ignore for non-healing classes
                    }
                    Unit* caster = GetCaster();
                    caster->CastSpell(caster, spell_id, true);
                }
            }

            void Register()
            {
                // add dummy effect spell handler to Blessing of Faith
                OnEffectHitTarget += SpellEffectFn(spell_pal_blessing_of_faith_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_blessing_of_faith_SpellScript();
        }
};

/// Update to Legion 7.3.5 build 26365
/// Called by Holy Shock - 20473
class spell_pal_holy_shock: public SpellScriptLoader
{
    public:
        spell_pal_holy_shock() : SpellScriptLoader("spell_pal_holy_shock") { }

        class spell_pal_holy_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_holy_shock_SpellScript);

            enum eSpells
            {
                HolyShockDamages        = 25912,
                HolyShockHeal           = 25914,
                PowerOfTheSilverHand    = 200657
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->IsFriendlyTo(l_Target))
                    l_Caster->CastSpell(l_Caster->IsValidAssistTarget(l_Target) ? l_Target : l_Caster, eSpells::HolyShockHeal, true);
                else
                    l_Caster->CastSpell(l_Target, eSpells::HolyShockDamages, true);

                if (l_Caster->HasAura(eSpells::PowerOfTheSilverHand))
                    l_Caster->RemoveAura(eSpells::PowerOfTheSilverHand);
            }

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Caster->IsFriendlyTo(l_Target))
                {
                    if (!l_Caster->IsValidAttackTarget(l_Target))
                        return SPELL_FAILED_BAD_TARGETS;
                    if (!l_Caster->isInFront(l_Target))
                        return SPELL_FAILED_UNIT_NOT_INFRONT;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_holy_shock_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_pal_holy_shock_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_holy_shock_SpellScript();
        }
};

/// Lay on Hands - 633
class spell_pal_lay_on_hands: public SpellScriptLoader
{
    public:
        spell_pal_lay_on_hands() : SpellScriptLoader("spell_pal_lay_on_hands") { }

        class spell_pal_lay_on_hands_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_lay_on_hands_SpellScript);

            SpellCastResult CheckForbearance()
            {
                if (Unit* target = GetExplTargetUnit())
                    if (target->HasAura(SPELL_FORBEARANCE))
                        return SPELL_FAILED_TARGET_AURASTATE;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        _player->CastSpell(target, SPELL_FORBEARANCE, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_lay_on_hands_SpellScript::CheckForbearance);
                OnHit += SpellHitFn(spell_pal_lay_on_hands_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_lay_on_hands_SpellScript();
        }
};

/// Righteous defense - 31789
class spell_pal_righteous_defense: public SpellScriptLoader
{
    public:
        spell_pal_righteous_defense() : SpellScriptLoader("spell_pal_righteous_defense") { }

        class spell_pal_righteous_defense_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_righteous_defense_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_DONT_REPORT;

                if (Unit* target = GetExplTargetUnit())
                {
                    if (!target->IsFriendlyTo(caster) || target->getAttackers().empty())
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_righteous_defense_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_righteous_defense_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Eternal Flame - 114163
class spell_pal_eternal_flame : public SpellScriptLoader
{
    public:
        spell_pal_eternal_flame() : SpellScriptLoader("spell_pal_eternal_flame") { }

        class spell_pal_eternal_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_eternal_flame_SpellScript);

            enum eSpells
            {
                T17Holy2P   = 165438,
                T17Holy4P   = 167697,
                LawfulWords = 166780,
                LightsFavor = 166781,
                WoDPvPHoly4PBonusAura   = 180766,
                WoDPvPHoly4PBonus       = 180767,
            };

            int32 m_PowerUsed = 0;

            SpellCastResult CheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetPower(POWER_HOLY_POWER) < 1)
                    {
                        if (!l_Caster->HasAura(PALADIN_SPELL_DIVINE_PURPOSE_AURA) && !l_Caster->HasAura(eSpells::T17Holy2P))
                            return SPELL_FAILED_NO_POWER;
                    }
                }

                return SPELL_CAST_OK;
            }

            void HandleOnCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    m_PowerUsed = l_Caster->GetPower(POWER_HOLY_POWER);

                    if (AuraEffect* l_T17Holy = l_Caster->GetAuraEffect(eSpells::T17Holy2P, EFFECT_1))
                        m_PowerUsed = l_T17Holy->GetAmount();
                }
            }

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (AuraEffect* l_T17Holy = l_Caster->GetAuraEffect(eSpells::T17Holy4P, EFFECT_0))
                    {
                        /// When you cast Word of Glory, you have a 20% chance to be able to cast a free 3 Holy Power Light of Dawn.
                        if (!roll_chance_i(l_T17Holy->GetAmount()))
                            return;

                        l_Caster->CastSpell(l_Caster, eSpells::LightsFavor, true);
                    }

                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (!l_Caster->HasAura(eSpells::LawfulWords))
                            l_Caster->SetPower(POWER_HOLY_POWER, m_PowerUsed);

                        if (m_PowerUsed > 3 || l_Caster->HasAura(PALADIN_SPELL_DIVINE_PURPOSE_AURA))
                            m_PowerUsed = 3;

                        SetHitHeal((GetHitHeal() / 3) * m_PowerUsed);

                        if (l_Target->GetGUID() == l_Caster->GetGUID() && l_Caster->HasAura(PALADIN_SPELL_BASTION_OF_GLORY))
                        {
                            if (Aura* l_Aura = l_Caster->GetAura(PALADIN_SPELL_BASTION_OF_GLORY))
                            {
                                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(PALADIN_SPELL_BASTION_OF_GLORY))
                                {
                                    SetHitHeal(GetHitHeal() + CalculatePct(GetHitHeal(), l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Aura->GetStackAmount()));
                                    l_Caster->RemoveAurasDueToSpell(PALADIN_SPELL_BASTION_OF_GLORY);
                                }
                            }
                        }

                        l_Caster->CastSpell(l_Target, PALADIN_SPELL_ETERNAL_FLAME_PERIODIC_HEAL, true);

                        if (Aura* l_PeriodicHeal = l_Target->GetAura(PALADIN_SPELL_ETERNAL_FLAME_PERIODIC_HEAL))
                        {
                            int32 l_Duration = (GetSpellInfo()->Effects[EFFECT_2].BasePoints / 3) * m_PowerUsed;
                            l_PeriodicHeal->SetDuration(l_Duration * IN_MILLISECONDS);
                            if (l_Caster->GetGUID() == l_Target->GetGUID())
                            {
                                int32 l_Amount = l_PeriodicHeal->GetEffect(0)->GetAmount();
                                l_Amount += CalculatePct(l_Amount, GetSpellInfo()->Effects[1].BasePoints);
                                l_PeriodicHeal->GetEffect(0)->SetAmount(l_Amount);
                            }
                        }

                        if (!l_Caster->HasAura(PALADIN_SPELL_DIVINE_PURPOSE_AURA) && !l_Caster->HasAura(eSpells::LawfulWords))
                            l_Caster->ModifyPower(POWER_HOLY_POWER, -m_PowerUsed);

                        if (l_Caster->HasAura(PALADIN_SPELL_GLYPH_OF_WORD_OF_GLORY) && l_Target->IsFriendlyTo(l_Caster))
                        {
                            if (Aura* l_Aura = l_Caster->AddAura(PALADIN_SPELL_GLYPH_OF_WORD_OF_GLORY_DAMAGE, l_Caster))
                            {
                                if (m_PowerUsed > 3 || l_Caster->HasAura(PALADIN_SPELL_DIVINE_PURPOSE_AURA))
                                    m_PowerUsed = 3;

                                l_Aura->GetEffect(0)->ChangeAmount(l_Aura->GetEffect(0)->GetAmount() * m_PowerUsed);
                                l_Aura->SetNeedClientUpdateForTargets();
                            }
                        }

                        if (l_Caster->HasAura(eSpells::WoDPvPHoly4PBonusAura))
                        {
                            l_Caster->CastSpell(l_Target, eSpells::WoDPvPHoly4PBonus, true);

                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WoDPvPHoly4PBonusAura))
                            {
                                if (Aura* l_Aura = l_Target->GetAura(eSpells::WoDPvPHoly4PBonus))
                                    l_Aura->GetEffect(EFFECT_0)->SetAmount(l_SpellInfo->Effects[EFFECT_0].BasePoints * -m_PowerUsed);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_eternal_flame_SpellScript::CheckCast);
                OnCast += SpellCastFn(spell_pal_eternal_flame_SpellScript::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_pal_eternal_flame_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_eternal_flame_SpellScript();
        }
};

/// The Light Saves - 200421
class PlayerScript_the_light_saves : public PlayerScript
{
    public:
        PlayerScript_the_light_saves() :PlayerScript("PlayerScript_the_light_saves") {}

        enum eSpells
        {
            BeaconOfFaith       = 156910,
            BeaconOfLight       = 53563,
            BeaconOfVirtue      = 200025,
            TheLightSaves       = 200421,
            TheLightSavesBonus  = 200423,
            TheLightSavesMalus  = 211426,
        };

        void OnModifyHealth(Player* p_Player, int64 p_Value, int64)
        {
            Aura* l_BeaconOfFaith = p_Player->GetAura(eSpells::BeaconOfFaith);
            Aura* l_BeaconOfLight = p_Player->GetAura(eSpells::BeaconOfLight);
            Aura* l_BeaconOfVirtue = p_Player->GetAura(eSpells::BeaconOfVirtue);
            Unit* l_Caster = nullptr;

            if (l_BeaconOfFaith)
                l_Caster = l_BeaconOfFaith->GetCaster();
            else if (l_BeaconOfLight)
                l_Caster = l_BeaconOfLight->GetCaster();
            else if (l_BeaconOfVirtue)
                l_Caster = l_BeaconOfVirtue->GetCaster();
            else
                return;

            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::TheLightSaves) || l_Caster->HasAura(eSpells::TheLightSavesBonus) || l_Caster->HasAura(eSpells::TheLightSavesMalus))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TheLightSaves);
            if (!l_SpellInfo)
                return;

            if (p_Player->HealthBelowPct(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                l_Caster->CastSpell(l_Caster, eSpells::TheLightSavesBonus, true);
        }
};

enum SealOfJusticeSpells
{
    SpellSealOfJusticeProc = 20170
};

/// Last Update 6.2.3
/// Holy Shield - 152261
class spell_pal_holy_shield: public SpellScriptLoader
{
    public:
        spell_pal_holy_shield() : SpellScriptLoader("spell_pal_holy_shield") { }

        class spell_pal_holy_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_holy_shield_AuraScript);

            enum eSpells
            {
                HolyShieldDamage = 157122
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Victim = p_EventInfo.GetDamageInfo()->GetTarget();
                Unit* l_Attacker = p_EventInfo.GetDamageInfo()->GetActor();

                if (l_Victim == nullptr || l_Attacker == nullptr)
                    return;

                l_Victim->CastSpell(l_Attacker, eSpells::HolyShieldDamage, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_holy_shield_AuraScript::OnProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_holy_shield_AuraScript();
        }
};

/// Sanctified Wrath - 53376
/// Called by Avenging Wrath (holy) - 31842 and Avenging Wrath (ret) - 31884
/// last update : 6.1.2 19802
class spell_pal_sanctified_wrath : public SpellScriptLoader
{
    public:
        spell_pal_sanctified_wrath() : SpellScriptLoader("spell_pal_sanctified_wrath") { }

        enum eSpells
        {
            AvengingWrathHoly    = 31842,
            AvengingWrathRet     = 31884,
            SanctifiedWrath      = 53376,
            SanctifiedWrathBonus = 114232
        };

        class spell_pal_sanctified_wrath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_sanctified_wrath_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::SanctifiedWrath))
                    l_Caster->CastSpell(l_Caster, eSpells::SanctifiedWrathBonus, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::SanctifiedWrathBonus))
                    l_Caster->RemoveAurasDueToSpell(eSpells::SanctifiedWrathBonus);
            }

            void Register()
            {
                switch (m_scriptSpellId)
                {
                case eSpells::AvengingWrathHoly:
                    AfterEffectApply += AuraEffectApplyFn(spell_pal_sanctified_wrath_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_HEALING_DONE_PERCENT, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_pal_sanctified_wrath_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_HEALING_DONE_PERCENT, AURA_EFFECT_HANDLE_REAL);
                    break;
                case eSpells::AvengingWrathRet:
                    AfterEffectApply += AuraEffectApplyFn(spell_pal_sanctified_wrath_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_pal_sanctified_wrath_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                    break;
                default:
                    break;
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_sanctified_wrath_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Selfless Healer - 85804
class spell_pal_selfless_healer_proc : public SpellScriptLoader
{
    public:
        spell_pal_selfless_healer_proc() : SpellScriptLoader("spell_pal_selfless_healer_proc") { }

        class spell_pal_selfless_healer_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_selfless_healer_proc_AuraScript);

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != PALADIN_SPELL_JUDGMENT)
                    return;

                l_Caster->CastSpell(l_Caster, PALADIN_SPELL_SELFLESS_HEALER_STACK, true);

                if (Aura* l_SelflessHealer = l_Caster->GetAura(PALADIN_SPELL_SELFLESS_HEALER_STACK))
                    if (l_SelflessHealer->GetStackAmount() == 3)
                        l_Caster->CastSpell(l_Caster, PALADIN_SPELL_SELFLESS_HEALER_VISUAL, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_selfless_healer_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_selfless_healer_proc_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Turn Evil - 145067
class spell_pal_turn_evil : public SpellScriptLoader
{
    public:
        spell_pal_turn_evil() : SpellScriptLoader("spell_pal_turn_evil") { }

        class spell_pal_turn_evil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_turn_evil_SpellScript);

            SpellCastResult CheckCast()
            {
                if (Unit* l_Target = GetExplTargetUnit())
                {
                    if (l_Target->IsPlayer())
                        return SPELL_FAILED_BAD_TARGETS;
                    return SPELL_CAST_OK;
                }
                return SPELL_FAILED_NO_VALID_TARGETS;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_turn_evil_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_turn_evil_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Call by Hand of Freedom - 1044
/// Glyph of the Liberator - 159573
/// Glyph of Hand of Freedom - 159583
class spell_pal_glyph_of_the_liberator : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_the_liberator() : SpellScriptLoader("spell_pal_glyph_of_the_liberator") { }

        class spell_pal_glyph_of_the_liberator_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_glyph_of_the_liberator_SpellScript);

            enum eSpells
            {
                GlyphoftheLiberator         = 159573,
                GlyphofHandofFreedom        = 159579,
                GlyphofHandofFreedomEffect  = 159583
            };

            void HandleAfterHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || l_Player == nullptr)
                    return;

                if (l_Player->HasAura(eSpells::GlyphoftheLiberator) && l_Target->GetGUID() != l_Player->GetGUID())
                {
                    if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::GlyphoftheLiberator, EFFECT_0))
                    {
                        if (l_Player->HasSpellCooldown(GetSpellInfo()->Id))
                            l_Player->ReduceSpellCooldown(GetSpellInfo()->Id, l_AuraEffect->GetAmount() * IN_MILLISECONDS);

                        if (SpellCategoryEntry const* l_HandofFreedomCategory = GetSpellInfo()->ChargeCategoryEntry)
                            l_Player->ReduceChargeCooldown(l_HandofFreedomCategory, l_AuraEffect->GetAmount() * IN_MILLISECONDS);
                    }
                }

                if (l_Player->HasAura(eSpells::GlyphofHandofFreedom))
                    l_Player->CastSpell(l_Target, eSpells::GlyphofHandofFreedomEffect, true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_pal_glyph_of_the_liberator_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_glyph_of_the_liberator_SpellScript();
        }
};

/// last update : 7.3.5 - Build 26365
/// Beacon of Light - 53563, Beacon of Faith - 156910
class spell_pal_beacon_of_light : public SpellScriptLoader
{
    public:
        spell_pal_beacon_of_light() : SpellScriptLoader("spell_pal_beacon_of_light") { }

        class spell_pal_beacon_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_beacon_of_light_AuraScript);

            enum eSpells
            {
                BeaconOfLight           = 53563,
                BeaconOfLightProcAura   = 53651,

                BeaconOfFaith           = 156910,
                BeaconOfFaithProcAura   = 177173
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                if (l_SpellInfo->Id == eSpells::BeaconOfLight)
                    l_Caster->CastCustomSpell(l_Caster, eSpells::BeaconOfLightProcAura, 0, nullptr, nullptr, true, nullptr, nullptr, l_Target->GetGUID());
                else
                    l_Caster->CastCustomSpell(l_Caster, eSpells::BeaconOfFaithProcAura, 0, nullptr, nullptr, true, nullptr, nullptr, l_Target->GetGUID());

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::BeaconOfLight:
                    {
                        uint64 l_PreviousBeaconId = l_Player->GetBeaconOfLightTarget();
                        Unit* l_PreviousBeaconTarget = sObjectAccessor->FindPlayer(l_PreviousBeaconId);
                        if (l_PreviousBeaconTarget)
                            l_PreviousBeaconTarget->RemoveAura(eSpells::BeaconOfLight);

                        l_Player->SetBeaconOfLightTarget(l_Target->GetGUID());
                        break;
                    }
                    case eSpells::BeaconOfFaith:
                    {
                        uint64 l_PreviousBeaconId = l_Player->GetBeaconOfFaithTarget();
                        Unit* l_PreviousBeaconTarget = sObjectAccessor->FindPlayer(l_PreviousBeaconId);
                        if (l_PreviousBeaconTarget)
                            l_PreviousBeaconTarget->RemoveAura(eSpells::BeaconOfFaith);

                        l_Player->SetBeaconOfFaithTarget(l_Target->GetGUID());
                        break;
                    }
                    default:
                        break;
                }

                l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets].insert(l_Target->GetGUID());
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_SpellInfo->Id == eSpells::BeaconOfLight)
                {
                    l_Caster->RemoveAura(eSpells::BeaconOfLightProcAura, l_Target->GetGUID());
                    l_Player->SetBeaconOfLightTarget(0);
                }
                else
                {
                    l_Caster->RemoveAura(eSpells::BeaconOfFaithProcAura, l_Target->GetGUID());
                    l_Player->SetBeaconOfFaithTarget(0);
                }

                if ((l_SpellInfo->Id == eSpells::BeaconOfLight && l_Caster->HasAura(eSpells::BeaconOfFaithProcAura)) ||
                    (l_SpellInfo->Id == eSpells::BeaconOfFaith && l_Caster->HasAura(eSpells::BeaconOfLightProcAura)))
                    return;

                l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets].erase(l_Target->GetGUID());
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_beacon_of_light_AuraScript::OnApply, EFFECT_2, SPELL_AURA_MOD_HEALING_RECEIVED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_beacon_of_light_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_MOD_HEALING_RECEIVED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_beacon_of_light_AuraScript();
        }

        class spell_pal_beacon_of_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_beacon_of_light_SpellScript);

            enum eSpells
            {
                BeaconOfLight = 53563,
                BeaconOfFaith = 156910,
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Target->HasAura(eSpells::BeaconOfLight, l_Caster->GetGUID()) || l_Target->HasAura(eSpells::BeaconOfFaith, l_Caster->GetGUID()))
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_beacon_of_light_SpellScript::CheckCast);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_beacon_of_light_SpellScript();
        }
};

/// Last update : 7.3.5 Build 26124
/// Light's Beacon - 53652
class spell_pal_lights_beacon_heal : public SpellScriptLoader
{
    public:
        spell_pal_lights_beacon_heal() : SpellScriptLoader("spell_pal_lights_beacon_heal") { }

        class spell_pal_lights_beacon_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_lights_beacon_heal_SpellScript);

            enum eSpells
            {
                NecroticRot = 209858
            };

            void HandleHeal(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->IsValidAssistTarget(l_Target))
                    SetHitHeal(0);

                if (Aura* l_Aura = l_Target->GetAura(eSpells::NecroticRot))
                    if (AuraEffect* l_NecroticEff = l_Aura->GetEffect(EFFECT_1))
                    {
                        int32 l_healAmount = GetHitHeal();
                        AddPct(l_healAmount, l_NecroticEff->GetAmount());
                        SetHitHeal(l_healAmount);
                    }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_lights_beacon_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_lights_beacon_heal_SpellScript();
        }
};

/// last update : 7.1.5 Build 23420
/// Beacon of Light (proc aura) - 53651, Beacon of Faith (proc aura) - 177173
class spell_pal_beacon_of_light_proc : public SpellScriptLoader
{
    public:
        spell_pal_beacon_of_light_proc() : SpellScriptLoader("spell_pal_beacon_of_light_proc") { }

        class spell_pal_beacon_of_light_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_beacon_of_light_proc_AuraScript);

            enum eAreaID
            {
                GurubashiRingBattle = 2177
            };

            enum eSpells
            {
                BeaconOfLight           = 53563,
                BeaconOfLightProcAura   = 53651,
                BeaconOfLightHeal       = 53652,
                BeaconOfLightMana       = 88852,

                BeaconOfFaith           = 156910,
                BeaconOfFaithProcAura   = 177173,
                BeaconOfFaithHeal       = 177174,

                BeaconOfVirtue          = 200025,

                HolyLight               = 82326,
                FlashofLight            = 19750,
                BestowFaith             = 223306,

                LightsHammer            = 119952,      ///< Healing Spell from Light's Hammer: Arcing Light
                HolyPrism               = 114852,
                LightOfDawn             = 225311,

                /// Does not cause your Beacon of Light to be healed
                LightOfTheMartyr        = 183998,
                LayOnHands              = 633,
                JudgmentOfLight         = 183811,
                AvengingCrusader        = 216372,
                AvengingCrusader2       = 216371,

                /// Avoid infinite proc loop with some others class spell
                ShamanRestorativeMists = 114083,

                T21Holy2PBonus         = 251863,

                LightsEmbrace          = 247237
            };

            int32 GetPctBySpell(uint32 l_SpellID, Unit* p_Paladin) const
            {
                int32 l_Percent = 0;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BeaconOfLightProcAura);
                if (!l_SpellInfo)
                    return l_Percent;

                l_Percent = l_SpellInfo->Effects[EFFECT_0].BasePoints; ///< 40% heal from all other heals

                if (p_Paladin->HasSpell(eSpells::BeaconOfFaith))
                {
                    if ((l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BeaconOfFaith)))
                        l_Percent -= CalculatePct(l_Percent, l_SpellInfo->Effects[EFFECT_3].BasePoints); ///< Beacon of faith should heal at 20% reduced effectiveness (80% of 40%)
                }

                if (AuraEffect const* l_LightsEmbraceEffect = p_Paladin->GetAuraEffect(eSpells::LightsEmbrace, SpellEffIndex::EFFECT_0))
                    l_Percent += l_LightsEmbraceEffect->GetAmount();

                switch (l_SpellID)
                {
                    case eSpells::LightsHammer:
                    case eSpells::HolyPrism:
                    case eSpells::LightOfDawn:
                        l_Percent /= 2;                                  ///< AOE Heals trigger the beacon heal for half the normal amount
                        break;
                    case HolyLight:
                    case FlashofLight:
                        if (AuraEffect* l_AuraEffect = p_Paladin->GetAuraEffect(eSpells::T21Holy2PBonus, EFFECT_0))
                            l_Percent += l_AuraEffect->GetAmount();
                        break;
                    default:
                        break;
                }

                return l_Percent;
            }

            /// 7.1.5 - Build 23420 - Added to prevent the abuse on Greymane's Gurubashi Arena where the pala stays out of the battle ring, heals himself while having his beacon on an ally inside the battle ring, thus healing him
            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_OwnerOfBeacon = GetTarget();  ///< The Paladin
                Unit* l_TargetOfBeacon = GetCaster();

                if (!l_OwnerOfBeacon || !l_TargetOfBeacon)
                    return false;

                if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) && l_OwnerOfBeacon->GetAreaId() != eAreaID::GurubashiRingBattle && l_TargetOfBeacon->GetAreaId() == eAreaID::GurubashiRingBattle)
                    return false;

                /// 7.3.5 - Build 26365: Need to prevent trinkets from proccing Beacons: only Paladin spells should proc them.
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_DamageInfo->GetSpellInfo() || l_DamageInfo->GetSpellInfo()->SpellFamilyName != SPELLFAMILY_PALADIN)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_OwnerOfBeacon = GetTarget();  ///< The Paladin
                Unit* l_TargetOfBeacon = GetCaster();
                SpellInfo const * l_SpellInfo = GetSpellInfo();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_OwnerOfBeacon || !l_TargetOfBeacon || !l_SpellInfo || !l_DamageInfo)
                    return;

                // Fix for Imprison and Cyclone
                if (l_TargetOfBeacon->HasAura(217832) || l_TargetOfBeacon->HasAura(33786))
                    return;

                SpellInfo const* l_SpellInfoProc = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfoProc)
                    return;

                switch (l_SpellInfoProc->Id)
                {
                    case eSpells::BeaconOfLightHeal:
                    case eSpells::BeaconOfFaithHeal:
                    case eSpells::LightOfTheMartyr:
                    case eSpells::BeaconOfVirtue:
                    case eSpells::LayOnHands:
                    case eSpells::JudgmentOfLight:
                    case eSpells::AvengingCrusader:
                    case eSpells::AvengingCrusader2:
                    case eSpells::ShamanRestorativeMists:
                        return;
                    default:
                        break;
                }

                Unit* l_TargetOfHeal = l_DamageInfo->GetTarget();
                if (!l_TargetOfHeal)
                    return;

                int32 l_Bp = CalculatePct(l_DamageInfo->GetAmount(), GetPctBySpell(l_SpellInfoProc->Id, l_OwnerOfBeacon));

                std::set<uint64> &l_BeaconsTargets = l_OwnerOfBeacon->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets];

                if (l_SpellInfo->Id == eSpells::BeaconOfLightProcAura && l_TargetOfBeacon->HasAura(eSpells::BeaconOfLight) && l_TargetOfHeal->GetGUID() != l_TargetOfBeacon->GetGUID())
                    l_OwnerOfBeacon->CastCustomSpell(l_TargetOfBeacon, eSpells::BeaconOfLightHeal, &l_Bp, nullptr, nullptr, true);
                else if (l_SpellInfo->Id == eSpells::BeaconOfFaithProcAura && l_TargetOfBeacon->HasAura(eSpells::BeaconOfFaith) && l_TargetOfHeal->GetGUID() != l_TargetOfBeacon->GetGUID())
                    l_OwnerOfBeacon->CastCustomSpell(l_TargetOfBeacon, eSpells::BeaconOfFaithHeal, &l_Bp, nullptr, nullptr, true);
                else if (l_SpellInfo->Id == eSpells::BeaconOfLightProcAura && l_TargetOfBeacon->HasAura(eSpells::BeaconOfVirtue))
                {
                    for (uint64 l_TargetGUID : l_BeaconsTargets)
                    {
                        Unit* l_Target = sObjectAccessor->GetUnit(*l_OwnerOfBeacon, l_TargetGUID);
                        if (!l_Target || l_Target->GetGUID() == l_TargetOfHeal->GetGUID())
                            continue;

                        l_OwnerOfBeacon->CastCustomSpell(l_Target, eSpells::BeaconOfLightHeal, &l_Bp, nullptr, nullptr, true);
                    }
                }

                if (l_TargetOfHeal->GetGUID() == l_TargetOfBeacon->GetGUID() && (l_SpellInfoProc->Id == eSpells::FlashofLight || l_SpellInfoProc->Id == eSpells::HolyLight))
                {
                    if (l_BeaconsTargets.find(l_TargetOfHeal->GetGUID()) != l_BeaconsTargets.end())
                    {
                        Spell const* l_Spell = l_DamageInfo->GetSpell();
                        l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BeaconOfLightMana);
                        if (!l_Spell || !l_SpellInfo)
                            return;

                        int32 l_ManaAmount = CalculatePct(l_Spell->GetPowerCost(Powers::POWER_MANA), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                        l_OwnerOfBeacon->CastCustomSpell(l_OwnerOfBeacon, eSpells::BeaconOfLightMana, &l_ManaAmount, nullptr, nullptr, true);
                    }
                }
            }

            void HandleAfterEffectApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (p_AurEff->GetId() != eSpells::BeaconOfLightProcAura)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->GetGUID() == l_Target->GetGUID() && !l_Caster->HasAura(eSpells::BeaconOfLight) && !l_Caster->HasAura(eSpells::BeaconOfVirtue))
                    l_Caster->RemoveAura(eSpells::BeaconOfLightProcAura, l_Caster->GetGUID());
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_beacon_of_light_proc_AuraScript::HandleOnCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_beacon_of_light_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                AfterEffectApply += AuraEffectApplyFn(spell_pal_beacon_of_light_proc_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_beacon_of_light_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Avenger's Shield - 31935
class spell_pal_avengers_shield : public SpellScriptLoader
{
    public:
        spell_pal_avengers_shield() : SpellScriptLoader("spell_pal_avengers_shield") { }

        class spell_impl : public SpellScript
        {
            PrepareSpellScript(spell_impl);

            enum eSpells
            {
                FaithBarricade          = 165447,
                T17Protection2P         = 165446,
                GlyphofDazingShield     = 56414,
                GlyphofDazingShieldDaz  = 63529,
                FirstAvenger            = 203776,
                BulwarkOfOrderAura      = 209389,
                BulwarkOfOrderProc      = 209388,
                GiftOfTheGoldenValkyr   = 207628,
                GuardianOfAncientKings  = 86659
            };

            bool Load() override
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster)
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::AvengersShieldDamage) = 0;
                return (SpellScript::Load());
            }

            void HandleOnCast()
            {
                /// get main target for silence eff
                if (Unit* l_Target = GetExplTargetUnit())
                    m_MainTargetGuid = l_Target->GetGUID();

                m_TargetCount = GetSpell()->GetUnitTargetCount();
            }

            void HandleSilence(SpellEffIndex p_EffIndex)
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    if (l_Target->GetGUID() != m_MainTargetGuid || ///< work only on main-target
                        l_Target->GetTypeId() == TYPEID_PLAYER) ///< and on non-player targets
                    {
                        PreventHitAura();
                        PreventHitDefaultEffect(p_EffIndex);
                    }
                }
            }

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Player* l_Player = GetCaster()->ToPlayer();

                if (!l_Player)
                    SetHitDamage(0);

                if (!l_Target || !l_Caster || !l_Player)
                    return;

                /// Gift of the Golden Valkyr : reduces by 3s cooldown of Guardian of Ancients kings for each hit
                if (l_Caster->HasAura(eSpells::GiftOfTheGoldenValkyr))
                    l_Player->ReduceSpellCooldown(eSpells::GuardianOfAncientKings, 3000);

                ++m_CurrentTarget;

                if (m_CurrentTarget == m_TargetCount)
                    l_Target->CastCustomSpell(l_Caster, GetSpellInfo()->Id, 0, 0, 0, true);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                /// When you use Avenger's Shield, your block chance is increased by 12% for 5 sec.
                if (l_Caster->HasAura(eSpells::T17Protection2P))
                    l_Caster->CastSpell(l_Caster, eSpells::FaithBarricade, true);

                /// Your Avenger's Shield now also dazes targets for 10 sec.
                if (l_Caster->HasAura(eSpells::GlyphofDazingShield))
                    l_Caster->CastSpell(l_Target, eSpells::GlyphofDazingShieldDaz, true);

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::AvengersShieldDamage) += GetHitDamage();
                if (Aura* l_BulwarkOfOrderAura = l_Caster->GetAura(eSpells::BulwarkOfOrderProc))
                {
                    if (AuraEffect* l_Effect = l_BulwarkOfOrderAura->GetEffect(EFFECT_0))
                    {
                        SpellInfo const* l_BulwarkOfOrderInfo = sSpellMgr->GetSpellInfo(eSpells::BulwarkOfOrderAura);

                        if (l_BulwarkOfOrderInfo)
                            l_Effect->SetAmount(CalculatePct(l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::AvengersShieldDamage), l_BulwarkOfOrderInfo->Effects[0].BasePoints));
                    }
                }
            }

            void HandleHitFirstAvenger()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();


                if (!l_Target || l_Target->GetGUID() != m_MainTargetGuid || !l_Caster || !l_Caster->HasAura(eSpells::FirstAvenger))
                    return;

                SpellInfo const* l_FirstAvengerInfo = sSpellMgr->GetSpellInfo(eSpells::FirstAvenger);
                if (!l_FirstAvengerInfo || !l_Caster->HasAura(eSpells::FirstAvenger))
                    return;

                int32 l_Damage = GetHitDamage();
                SetHitDamage(AddPct(l_Damage, l_FirstAvengerInfo->Effects[0].BasePoints));
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_impl::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_impl::HandleSilence, EFFECT_1, SPELL_EFFECT_INTERRUPT_CAST);
                OnEffectHitTarget += SpellEffectFn(spell_impl::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterHit += SpellHitFn(spell_impl::HandleAfterHit);
                OnHit += SpellHitFn(spell_impl::HandleHitFirstAvenger);
            }

            private:
                uint64 m_MainTargetGuid = 0;
                uint32 m_TargetCount = 0;
                uint32 m_CurrentTarget = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_impl();
        }
};

/// Item - Paladin T17 Protection 4P Bonus - 167740
class spell_pal_t17_protection_4p : public SpellScriptLoader
{
    public:
        spell_pal_t17_protection_4p() : SpellScriptLoader("spell_pal_t17_protection_4p") { }

        class spell_pal_t17_protection_4p_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_t17_protection_4p_AuraScript);

            enum eSpell
            {
                DefenderOfTheLight = 167742
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_EventInfo.GetHitMask() & ProcFlagsExLegacy::PROC_EX_BLOCK)
                    l_Caster->CastSpell(l_Caster, eSpell::DefenderOfTheLight, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_t17_protection_4p_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_t17_protection_4p_AuraScript();
        }
};

/// Item - Paladin WoD PvP Retribution 4P Bonus - 165895
class PlayerScript_paladin_wod_pvp_4p_bonus : public PlayerScript
{
    public:
        PlayerScript_paladin_wod_pvp_4p_bonus() :PlayerScript("PlayerScript_paladin_wod_pvp_4p_bonus") { }

        enum eSpells
        {
            T17Retribution2P    = 165440,
            CrusadersFury       = 165442
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After)
                return;

            if (p_Regen)
                return;

            if (p_OldValue > p_NewValue && p_Player->getClass() == CLASS_PALADIN && p_Player->GetActiveSpecializationID() == SPEC_PALADIN_RETRIBUTION && p_Power == POWER_HOLY_POWER)
            {
                /// Get the power earned (if > 0) or consumed (if < 0)
                int32 l_SpentPower = p_OldValue - p_NewValue;

                if (p_Player->HasAura(PALADIN_PVP_RETRIBUTION_4P_BONUS))
                {
                    /// Spending Holy Power increases your damage and healing by 2 % per Holy Power
                    int32 l_EffectValue = l_SpentPower * sSpellMgr->GetSpellInfo(PALADIN_VINDICATORS_FURY)->Effects[EFFECT_0].BasePoints;
                    p_Player->CastCustomSpell(p_Player, PALADIN_VINDICATORS_FURY, &l_EffectValue, &l_EffectValue, &l_EffectValue, true);
                }

                /// Spending Holy Power has a 20% chance to allow the use of Hammer of Wrath regardless of your target's current health.
                if (p_Player->HasAura(eSpells::T17Retribution2P) && roll_chance_i(20))
                    p_Player->CastSpell(p_Player, eSpells::CrusadersFury, true);
            }
        }
};

/// Grand Crusader - 85416
/// Hammer of the Righteous - 53595, Blessed Hammer - 204019
class spell_pal_grand_crusader : public SpellScriptLoader
{
    public:
        spell_pal_grand_crusader() : SpellScriptLoader("spell_pal_grand_crusader") { }

        class spell_pal_grand_crusader_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_grand_crusader_SpellScript);

            enum eSpells
            {
                GrandCrusader       = 85043,
                GrandCrusaderEffect = 85416,

                CrusadersJudgment   = 204023,
                Judgment            = 20271,
                FirstAvenger        = 203776
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GrandCrusader);

                if (l_SpellInfo == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::GrandCrusader))
                    return;

                int32 l_ProcChance = l_SpellInfo->ProcChance;

                if (AuraEffect* l_FirstAvengerEffect = l_Caster->GetAuraEffect(eSpells::FirstAvenger, EFFECT_1))
                    l_ProcChance += l_FirstAvengerEffect->GetAmount();

                if (!roll_chance_i(l_ProcChance))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::GrandCrusaderEffect, true);

                if (!l_Caster->HasAura(eSpells::CrusadersJudgment))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Judgment);
                if (!l_Player || !l_SpellInfo)
                    return;

                SpellCategoryEntry const* l_JudgmentCategoryEntry = l_SpellInfo->ChargeCategoryEntry;
                if (!l_JudgmentCategoryEntry)
                    return;

                l_Player->RestoreCharge(l_JudgmentCategoryEntry);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_grand_crusader_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_grand_crusader_SpellScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Grand Crusader - 85043
class spell_pal_grand_crusader_proc : public SpellScriptLoader
{
    public:
        spell_pal_grand_crusader_proc() : SpellScriptLoader("spell_pal_grand_crusader_proc") { }

        class spell_pal_grand_crusader_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_grand_crusader_proc_AuraScript);

            enum eSpells
            {
                GrandCrusaderEffect = 85416,

                CrusadersJudgment   = 204023,
                Judgment            = 20271
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (p_EventInfo.GetHitMask() & (ProcFlagsExLegacy::PROC_EX_DODGE | ProcFlagsExLegacy::PROC_EX_PARRY))
                    return true;

                return false;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::GrandCrusaderEffect, true);

                if (!l_Caster->HasAura(eSpells::CrusadersJudgment))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Judgment);
                if (!l_Player || !l_SpellInfo)
                    return;

                SpellCategoryEntry const* l_JudgmentCategoryEntry = l_SpellInfo->ChargeCategoryEntry;
                if (!l_JudgmentCategoryEntry)
                    return;

                l_Player->RestoreCharge(l_JudgmentCategoryEntry);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_grand_crusader_proc_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_pal_grand_crusader_proc_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_grand_crusader_proc_AuraScript();
        }
};

/// Last Update : 6.2.3
/// Glyph of Pillar of Light - 146959
class spell_pal_glyph_of_pillar_of_light : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_pillar_of_light() : SpellScriptLoader("spell_pal_glyph_of_pillar_of_light") { }

        class spell_pal_glyph_of_pillar_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_glyph_of_pillar_of_light_AuraScript);

            enum eSpells
            {
                GlyphOfPillarofLightVisual = 148064
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() == nullptr)
                    return;

                Unit* l_Target = p_EventInfo.GetDamageInfo()->GetTarget();

                if (l_Target == nullptr)
                    return;

                if (p_EventInfo.GetHitMask() & ProcFlagsExLegacy::PROC_EX_CRITICAL_HIT)
                    l_Caster->CastSpell(l_Target, eSpells::GlyphOfPillarofLightVisual, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_glyph_of_pillar_of_light_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_glyph_of_pillar_of_light_AuraScript();
        }
};

/// Glyph of the Luminous Charger - 89401
/// Called by: Summon Warhorse - 13819, Summon Sunwalker Kodo - 69820, Summon Thalassian Warhorse - 34769, Summon Exarch's Elekk - 73629, Summon Great Sunwalker Kodo - 69826.
/// Called by: Summon Charger - 23214, Summon Thalassian Charger - 34767, Summon Great Exarch's Elekk - 73630.
class spell_pal_glyph_of_the_luminous_charger : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_the_luminous_charger() : SpellScriptLoader("spell_pal_glyph_of_the_luminous_charger") { }

        class spell_pal_glyph_of_the_luminous_charger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_glyph_of_the_luminous_charger_AuraScript);

            enum eSpells
            {
                SpellPaladinLuminousCharger = 126666,
                SpellPaladinGlyphOfTheLuminousCharger = 89401,
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::SpellPaladinGlyphOfTheLuminousCharger))
                        l_Caster->CastSpell(l_Caster, eSpells::SpellPaladinLuminousCharger, true);
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::SpellPaladinLuminousCharger))
                        l_Caster->RemoveAura(eSpells::SpellPaladinLuminousCharger);
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_glyph_of_the_luminous_charger_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_glyph_of_the_luminous_charger_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_glyph_of_the_luminous_charger_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Hand of sacrifice - 6940
class spell_pal_hand_of_sacrifice : public SpellScriptLoader
{
    public:
        spell_pal_hand_of_sacrifice() : SpellScriptLoader("spell_pal_hand_of_sacrifice") { }

        class spell_pal_hand_of_sacrifice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_hand_of_sacrifice_AuraScript);

            enum eSpells
            {
                ShareTheBurden = 200327
            };

            void Absorb(AuraEffect* p_AurEff, DamageInfo & /*p_DmgInfo*/, uint32 & p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
            }

            void SplitDamage(AuraEffect* /*p_AurEff*/, DamageInfo & /*p_DmgInfo*/, uint32 & p_SplitAmount)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_healthPct = GetSpellInfo()->Effects[EFFECT_2].BasePoints;
                if (l_Caster->HealthBelowPct(l_healthPct))
                {
                    p_SplitAmount = 0;
                    Remove(AURA_REMOVE_BY_DEFAULT);
                    l_Caster->RemoveAurasDueToSpell(GetId(), l_Caster->GetGUID());
                }
                else if (Player* l_ModOwner = (l_Caster ? l_Caster->GetSpellModOwner() : NULL))
                    l_ModOwner->ApplySpellMod(GetId(), SPELLMOD_VALUE_MULTIPLIER, p_SplitAmount);
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_hand_of_sacrifice_AuraScript::Absorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectSplitDamage += AuraEffectSplitDamageFn(spell_pal_hand_of_sacrifice_AuraScript::SplitDamage, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_hand_of_sacrifice_AuraScript();
        }
};

/// Last Update 6.2.3
/// Hammer of Justice - 853
class spell_pal_hammer_of_justice : public SpellScriptLoader
{
    public:
        spell_pal_hammer_of_justice() : SpellScriptLoader("spell_pal_hammer_of_justice") { }

        class spell_pal_hammer_of_justice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hammer_of_justice_SpellScript);

            enum eSpells
            {
                WoDPvPProtection2PBonus = 165905,
                Lawbringer              = 165909
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::WoDPvPProtection2PBonus))
                    l_Caster->CastSpell(l_Target, eSpells::Lawbringer, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pal_hammer_of_justice_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_hammer_of_justice_SpellScript();
        }
};

/// Last Update 7.0.3
/// Crusader's Might - 196926
class spell_pal_crusader_might : public SpellScriptLoader
{
    public:
        spell_pal_crusader_might() : SpellScriptLoader("spell_pal_crusader_might") { }

        class spell_pal_crusader_might_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_crusader_might_AuraScript);

            enum eSpells
            {
                CrusaderStrike  = 35395,
                HolyShock       = 20473,
                LightofDawn     = 85222
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (l_Caster == nullptr || l_DamageInfo == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_SpellInfo == nullptr || l_Player == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::CrusaderStrike)
                    return;

                l_Player->ReduceSpellCooldown(eSpells::LightofDawn, p_AurEff->GetAmount() * -1);
                l_Player->ReduceSpellCooldown(eSpells::HolyShock, p_AurEff->GetAmount() * -1);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_crusader_might_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_crusader_might_AuraScript();
        }
};

/// Last Update 7.0.3
/// Divine Steed - 205656, Divine Steed - 190784
class spell_pal_divine_steed : public SpellScriptLoader
{
    public:
        spell_pal_divine_steed() : SpellScriptLoader("spell_pal_divine_steed") { }

        class spell_pal_divine_steed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_steed_SpellScript);

            enum eSpells
            {
                HumanMount      = 221883,
                TaurenMount     = 221885,
                BloodElfeMount  = 221886,
                DraneiMount     = 221887
            };

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                uint32 l_SpellId = 0;
                switch (l_Player->getRace())
                {
                    case RACE_HUMAN:
                    case RACE_DWARF:
                        l_SpellId = eSpells::HumanMount;
                        break;
                    case RACE_TAUREN:
                        l_SpellId = eSpells::TaurenMount;
                        break;
                    case RACE_BLOODELF:
                        l_SpellId = eSpells::BloodElfeMount;
                        break;
                    case RACE_DRAENEI:
                    case RACE_LIGHTFORGED_DRAENEI:
                        l_SpellId = eSpells::DraneiMount;
                        break;
                }
                if (l_SpellId)
                    l_Player->CastSpell(l_Player, l_SpellId, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_divine_steed_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_divine_steed_SpellScript();
        }
};


/// Last Update 7.1.5 Build 23420
/// Call by Divine Steed Mounts - 221883 - 221885 - 221886 - 221887
class spell_pal_divine_steed_mount : public SpellScriptLoader
{
    public:
        spell_pal_divine_steed_mount() : SpellScriptLoader("spell_pal_divine_steed_mount") { }

        class spell_pal_divine_steed_mount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_divine_steed_mount_AuraScript);

            void OnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_CAN_FIGHT_WITHOUT_DISMOUNT);
            }

            void OnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_3, eUnitFlags3::UNIT_FLAG3_CAN_FIGHT_WITHOUT_DISMOUNT);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_pal_divine_steed_mount_AuraScript::OnApply, EFFECT_2, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_pal_divine_steed_mount_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_divine_steed_mount_AuraScript();
        }
};

/// Last Update 7.0.3
/// Aura of Sacrifice - 210372
class spell_pal_aura_of_sacrifice : public SpellScriptLoader
{
    public:
        spell_pal_aura_of_sacrifice() : SpellScriptLoader("spell_pal_aura_of_sacrifice") { }

        class spell_pal_aura_of_sacrifice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_aura_of_sacrifice_AuraScript);

            enum eSpells
            {
                AuraofSacrifice     = 210380,
                AuraofSacrificeAura = 183416
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* p_AurEff, DamageInfo & p_DmgInfo, uint32 & p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                p_AbsorbAmount = 0;

                if (l_Caster == nullptr)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::AuraofSacrificeAura);
                if (l_Aura == nullptr)
                    return;

                AuraEffect* l_AuraEffectPctAbove = l_Aura->GetEffect(EFFECT_2);
                AuraEffect* l_AuraEffectPct = l_Aura->GetEffect(EFFECT_0);

                if (l_AuraEffectPctAbove == nullptr || l_AuraEffectPct == nullptr)
                    return;

                int32 l_PctAbove = l_AuraEffectPctAbove->GetAmount();
                int32 l_Pct = l_AuraEffectPct->GetAmount();

                if (l_Caster->GetHealthPct() > l_PctAbove)
                {
                    l_Pct = std::min(l_Pct, 100);
                    l_Pct = std::max(l_Pct, 0);
                    p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_Pct);
                    int32 l_DirectDamage = p_AbsorbAmount;
                    l_Target->CastCustomSpell(l_Caster, eSpells::AuraofSacrifice, &l_DirectDamage, nullptr, nullptr, true, nullptr, p_AurEff);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_aura_of_sacrifice_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_aura_of_sacrifice_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_aura_of_sacrifice_AuraScript();
        }
};

/// Aura of Sacrifice - 183416
class spell_pal_aura_of_sacrifice_area : public SpellScriptLoader
{
    public:
        spell_pal_aura_of_sacrifice_area() : SpellScriptLoader("spell_pal_aura_of_sacrifice_area") { }

        class spell_pal_aura_of_sacrifice_areaAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_aura_of_sacrifice_areaAuraScript);

            enum eSpells
            {
                AuraOfSacrifice = 210372,
                DivineVision    = 199324,
                AuraMastery     = 31821
            };

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_Distance = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                if (AuraEffect const* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::DivineVision, EFFECT_0))
                    l_Distance = AddPct(l_Distance, l_AuraEffect->GetAmount());

                if (AuraEffect const* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::AuraMastery, EFFECT_0))
                    l_Distance += CalculatePct(l_SpellInfo->Effects[EFFECT_1].BasePoints, l_AuraEffect->GetAmount());

                std::list<Unit*> l_GroupList;
                l_Caster->GetRaidMembers(l_GroupList);

                l_GroupList.remove_if([this, l_Caster, l_Distance](Unit* p_Unit) -> bool
                {
                    if (p_Unit == nullptr)
                        return true;

                    if (!l_Caster->IsValidAssistTarget(p_Unit))
                        return true;

                    if (l_Caster->GetGUID() == p_Unit->GetGUID())
                        return true;

                    if (l_Caster->GetDistance(p_Unit) > l_Distance)
                        return true;

                    return false;
                });

                std::set<uint64>& l_GroupInZone = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AuraOfSacrifice];
                /// Check the new in party member in Range and add aura
                for (auto l_Unit : l_GroupList)
                {
                    if (std::find(l_GroupInZone.begin(), l_GroupInZone.end(), l_Unit->GetGUID()) == l_GroupInZone.end())
                    {
                        l_GroupInZone.insert(l_Unit->GetGUID());
                        l_Caster->CastSpell(l_Unit, eSpells::AuraOfSacrifice, true);
                    }
                }

                /// Remove the party member no more in Range, or no more group member
                for (auto l_It = l_GroupInZone.begin();  l_It != l_GroupInZone.end();)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(*l_It);

                    if (l_Target == nullptr || std::find(l_GroupList.begin(), l_GroupList.end(), l_Target) == l_GroupList.end())
                    {
                        l_It = l_GroupInZone.erase(l_It);
                        if (l_Target != nullptr)
                            l_Target->RemoveAura(eSpells::AuraOfSacrifice, l_Caster->GetGUID());
                    }
                    else
                        l_It++;
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::set<uint64>& l_GroupInZone = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AuraOfSacrifice];

                /// Remove all the aura in Party member from caster
                for (auto l_UnitGUID : l_GroupInZone)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(l_UnitGUID);

                    if (l_Target != nullptr)
                        l_Target->RemoveAura(eSpells::AuraOfSacrifice, l_Caster->GetGUID());
                }
            }

            void Absorb(AuraEffect* /*p_AurEff*/, DamageInfo& /*p_DmgInfo*/, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_pal_aura_of_sacrifice_areaAuraScript::OnUpdate, EFFECT_0, SPELL_AURA_AREATRIGGER);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_aura_of_sacrifice_areaAuraScript::OnRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_aura_of_sacrifice_areaAuraScript::Absorb, EFFECT_4, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_aura_of_sacrifice_areaAuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Aura of Devotion - 183425
class spell_pal_aura_of_devoltion : public SpellScriptLoader
{
    public:
        spell_pal_aura_of_devoltion() : SpellScriptLoader("spell_pal_aura_of_devoltion") { }

        class spell_pal_aura_of_devoltion_areaAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_aura_of_devoltion_areaAuraScript);

            std::vector<uint64> l_GroupInZone;

            enum eSpells
            {
                AuraOfSacrifice = 210320,
                AuraMastery     = 31821,
                DivineVision    = 199324
            };

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_SpellInfoAuraMastery = sSpellMgr->GetSpellInfo(eSpells::AuraMastery);
                if (!l_Caster || !l_SpellInfo || !l_SpellInfoAuraMastery)
                    return;

                bool l_HasAuraMastery = false;
                int32 l_Distance = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                if (AuraEffect const* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::DivineVision, EFFECT_0))
                    l_Distance = AddPct(l_Distance, l_AuraEffect->GetAmount());

                if ((l_HasAuraMastery = l_Caster->HasAura(eSpells::AuraMastery)))
                    l_Distance *= 1 + l_SpellInfoAuraMastery->Effects[EFFECT_0].BasePoints / 100;

                std::list<Unit*> l_GroupList;
                l_Caster->GetRaidMembers(l_GroupList);

                l_GroupList.remove_if([this, l_Caster, l_Distance](Unit* p_Unit) -> bool
                {
                    if (p_Unit == nullptr)
                        return true;

                    if (!l_Caster->IsValidAssistTarget(p_Unit))
                        return true;

                   if (p_Unit->isPet())
                        return true;

                    if (l_Caster->GetDistance(p_Unit) > l_Distance)
                        return true;

                    if (l_Caster->isDead())     ///< prevent the dead paladin from giving his allies the buff in arenas :^)
                        return true;

                    return false;
                });
                /// Check the new in party member in Range and add aura, set the amount
                for (auto l_Unit : l_GroupList)
                {
                    if (std::find(l_GroupInZone.begin(), l_GroupInZone.end(), l_Unit->GetGUID()) == l_GroupInZone.end())
                        l_GroupInZone.push_back(l_Unit->GetGUID());

                    if (!l_Unit->HasAura(eSpells::AuraOfSacrifice))
                        l_Caster->CastSpell(l_Unit, eSpells::AuraOfSacrifice, true);

                    AuraEffect* l_AuraEffect = l_Unit->GetAuraEffect(eSpells::AuraOfSacrifice, EFFECT_0);
                    if (l_AuraEffect == nullptr)
                        continue;

                    int8 l_InititalAmount = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                    if (l_GroupList.size() == 0)
                        l_AuraEffect->SetAmount(l_InititalAmount);;

                    int8 l_Amount = l_InititalAmount * -1;
                    if ((l_GroupList.size() > (l_InititalAmount * -1)) && !l_HasAuraMastery)
                        l_Amount = 1;
                    else if (!l_HasAuraMastery)
                        l_Amount /= l_GroupList.size();
                    l_Amount = std::min(l_Amount, (int8)(l_InititalAmount * -1));
                    l_Amount = std::max(l_Amount, (int8)0);

                    l_AuraEffect->SetAmount(l_Amount * -1);
                }

                /// Remove the party member no more in Range, or no more group member
                for (std::vector<uint64>::iterator l_It = l_GroupInZone.begin(); l_It != l_GroupInZone.end();)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(*l_It);

                    if (l_Target == nullptr || std::find(l_GroupList.begin(), l_GroupList.end(), l_Target) == l_GroupList.end())
                    {
                        l_It = l_GroupInZone.erase(l_It);
                        if (l_Target != nullptr)
                            l_Target->RemoveAura(eSpells::AuraOfSacrifice, l_Caster->GetGUID());
                    }
                    else
                        l_It++;
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                /// Remove all the aura in Party member from caster
                for (auto l_UnitGUID : l_GroupInZone)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(l_UnitGUID);

                    if (l_Target != nullptr)
                        l_Target->RemoveAura(eSpells::AuraOfSacrifice, l_Caster->GetGUID());
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_pal_aura_of_devoltion_areaAuraScript::OnUpdate, EFFECT_0, SPELL_AURA_AREATRIGGER);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_aura_of_devoltion_areaAuraScript::OnRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_aura_of_devoltion_areaAuraScript();
        }
};

/// Last Update 7.0.3
/// Judgment of Light - 183778
class spell_pal_judgment_of_light : public SpellScriptLoader
{
    public:
        spell_pal_judgment_of_light() : SpellScriptLoader("spell_pal_judgment_of_light") { }

        class spell_pal_judgment_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_judgment_of_light_AuraScript);

            enum eSpells
            {
                Jugement = 20271,
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Target = GetTarget();
                Unit* l_Victim = p_EventInfo.GetDamageInfo()->GetTarget();

                if (l_Victim == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_SpellInfo == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::Jugement)
                    return;

                l_Target->CastSpell(l_Victim, p_AurEff->GetTriggerSpell(), true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_judgment_of_light_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_judgment_of_light_AuraScript();
        }
};

/// Last Update 7.0.3
/// Judgment of Light - 196941
class spell_pal_judgment_of_light_aura : public SpellScriptLoader
{
    public:
        spell_pal_judgment_of_light_aura() : SpellScriptLoader("spell_pal_judgment_of_light_aura") { }

        class spell_pal_judgment_of_light_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_judgment_of_light_aura_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (!p_EventInfo.GetDamageInfo())
                    return false;

                Unit* l_Attacker = p_EventInfo.GetDamageInfo()->GetActor();
                if (!l_Attacker)
                    return false;

                if (l_Attacker->IsFullHealth())
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Attacker = p_EventInfo.GetDamageInfo()->GetActor();

                if (l_Attacker == nullptr || l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Attacker, p_AurEff->GetTriggerSpell(), true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_judgment_of_light_aura_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_judgment_of_light_aura_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_judgment_of_light_aura_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Light of the Martyr - 219562
class spell_pal_light_of_the_martyr : public SpellScriptLoader
{
    public:
        spell_pal_light_of_the_martyr() : SpellScriptLoader("spell_pal_light_of_the_martyr") { }

        class spell_pal_light_of_the_martyr_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_light_of_the_martyr_AuraScript);

            enum eSpells
            {
                LightOfTheMartyrHeal    = 183998,
                LightOfTheMartyrDamage  = 196917,
                DivineShield            = 642,
                MaraadsDyingBreath      = 234848
            };

            uint32 m_NumberHealed = 0;                                              ///< Allows us to get the number of targets healed by Light of Dawn to get the % Augmented heal, that shouldnt be dealt to the paladin when casting Light of the Martyr

            void SetGuid(uint32 p_Type, uint64 p_Data) override
            {
                if (p_Type == 0)
                {
                    if (m_NumberHealed < 5)
                        m_NumberHealed++;                                               ///< SetGuid will be called each time the paladin gets the Aura from Light of Dawn, incrementing
                }
                else
                    m_NumberHealed = 0;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_SpellInfo == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::LightOfTheMartyrHeal || !p_EventInfo.GetDamageInfo()->GetAmount())
                    return;

                if (l_Target->HasAura(eSpells::DivineShield))
                    return;

                int32 l_HealingFactor = 0;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MaraadsDyingBreath))
                    l_HealingFactor = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                int32 l_BaseHeal = p_EventInfo.GetDamageInfo()->GetAmount() * 100 / (100 + m_NumberHealed * l_HealingFactor);
                int32 l_Damage = CalculatePct(l_BaseHeal, p_AurEff->GetAmount());
                l_Target->CastCustomSpell(l_Target, eSpells::LightOfTheMartyrDamage, &l_Damage, NULL, NULL, true);
                m_NumberHealed = 0;
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_light_of_the_martyr_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_light_of_the_martyr_AuraScript();
        }
};

/// Last Update 7.0.3
/// Light of the Martyr (heal) - 183998
class spell_pal_light_of_martyr_heal : public SpellScriptLoader
{
    public:
        spell_pal_light_of_martyr_heal() : SpellScriptLoader("spell_pal_light_of_martyr_heal") { }

        class spell_pal_light_of_martyr_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_light_of_martyr_heal_SpellScript);

            enum eSpells
            {
                FerventMartyr          = 223316,
                MaraadsDyingBreathBuff = 234862
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::FerventMartyr);
                l_Caster->RemoveAura(eSpells::MaraadsDyingBreathBuff);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pal_light_of_martyr_heal_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_light_of_martyr_heal_SpellScript();
        }
};

/// last update : 7.3.5 Build 26365
/// Beacon of Virtue - 200025
class spell_pal_beacon_of_virtue : public SpellScriptLoader
{
    public:
        spell_pal_beacon_of_virtue() : SpellScriptLoader("spell_pal_beacon_of_virtue") { }

        class spell_pal_beacon_of_virtue_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_beacon_of_virtue_AuraScript);

            enum eSpells
            {
                BeaconOfLight           = 53563,
                BeaconOfLightProcAura   = 53651
            };

            void OnEffectApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets].insert(l_Target->GetGUID());

                if (p_AurEff->GetEffIndex() == EFFECT_0)
                    l_Caster->CastCustomSpell(l_Caster, eSpells::BeaconOfLightProcAura, 0, nullptr, nullptr, true, nullptr, nullptr, l_Target->GetGUID());
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets].erase(l_Target->GetGUID());
                l_Caster->RemoveAura(eSpells::BeaconOfLightProcAura, l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_beacon_of_virtue_AuraScript::OnEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_beacon_of_virtue_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_pal_beacon_of_virtue_AuraScript::OnEffectApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_beacon_of_virtue_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_pal_beacon_of_virtue_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_beacon_of_virtue_SpellScript);

            std::unordered_set<uint64> m_Targets;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!m_Targets.empty())
                {
                    p_Targets.clear();

                    for (uint64 const& l_Guid : m_Targets)
                    {
                        if (Unit* l_Unit = Unit::GetUnit(*l_Caster, l_Guid))
                            p_Targets.push_back(l_Unit);
                    }

                    m_Targets.clear();
                    return;
                }

                Unit* l_MainTarget = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_SpellInfo)
                    return;

                if (!l_MainTarget)
                    l_MainTarget = l_Caster;

                if (l_MainTarget != l_Caster && !l_Caster->IsFriendlyTo(l_MainTarget))
                    l_MainTarget = l_Caster;

                p_Targets.remove_if([this, l_MainTarget](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || p_Object->GetGUID() == l_MainTarget->GetGUID())
                        return true;

                    return false;
                });                ///< Max 3 targets and should be the most injured

                int32 l_NbTargets = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                if (!p_Targets.empty())
                {
                    p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                    if (p_Targets.size() > l_NbTargets)
                        p_Targets.resize(l_NbTargets);
                }

                p_Targets.push_back(l_MainTarget);

                if (m_Targets.empty())
                {
                    for (auto const l_Iter : p_Targets)
                        m_Targets.insert(l_Iter->GetGUID());
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_beacon_of_virtue_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_beacon_of_virtue_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_beacon_of_virtue_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_beacon_of_virtue_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Hammer of the Righteous - 53595
class spell_pal_hammer_of_the_righteous: public SpellScriptLoader
{
    public:
        spell_pal_hammer_of_the_righteous() : SpellScriptLoader("spell_pal_hammer_of_the_righteous") { }

        class spell_pal_hammer_of_the_righteous_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hammer_of_the_righteous_SpellScript);

            enum eSpells
            {
                WaveDamage          = 88263,
                Consecration        = 26573,
                ConsecratedHammer   = 203785
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                AreaTrigger* l_ConsecrationTrigger = l_Caster->GetAreaTrigger(eSpells::Consecration);

                bool l_Consecration = l_ConsecrationTrigger != nullptr && l_Caster->GetDistance(l_ConsecrationTrigger) <= 7.0f;
                bool l_ConsecratedHammer = l_Caster->HasAura(eSpells::ConsecratedHammer);

                /// While you are standing in your Consecration, Hammer of the Righteous also causes a wave of light
                if (l_Consecration || l_ConsecratedHammer)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::WaveDamage, true);
                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HammerOfTheRighteousTarget) = l_Target->GetGUID();
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_hammer_of_the_righteous_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_hammer_of_the_righteous_SpellScript;
        }
};

/// Last Update 7.1.3 Build 23420
/// Called by Hammer of the Righteous (aoe damage) - 88263
class spell_pal_hammer_of_the_righteous_aoe : public SpellScriptLoader
{
    public:
        spell_pal_hammer_of_the_righteous_aoe() : SpellScriptLoader("spell_pal_hammer_of_the_righteous_aoe") { }

        class spell_pal_hammer_of_the_righteous_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hammer_of_the_righteous_aoe_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint64& l_MainTargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HammerOfTheRighteousTarget);

                p_Targets.remove_if([l_MainTargetGUID](WorldObject* p_Unit) -> bool
                {
                    if (p_Unit == nullptr)
                        return true;

                    if (l_MainTargetGUID == p_Unit->GetGUID())
                        return true;

                    return false;
                });

                l_MainTargetGUID = 0;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_hammer_of_the_righteous_aoe_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_hammer_of_the_righteous_aoe_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Eye of Tyr - 209202
class spell_pal_eye_of_tyr : public SpellScriptLoader
{
public:
    spell_pal_eye_of_tyr() : SpellScriptLoader("spell_pal_eye_of_tyr") { }

    class spell_pal_eye_of_tyr_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_eye_of_tyr_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
            {
                if (!p_Target || p_Target->GetExactDist2d(l_Caster) > 8.0)
                    return true;

                return false;
            });

        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_eye_of_tyr_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pal_eye_of_tyr_SpellScript();
    }
};


/// Last Update 7.0.3
/// Retribution Aura - 203797
class spell_pal_retribution_aura : public SpellScriptLoader
{
    public:
        spell_pal_retribution_aura() : SpellScriptLoader("spell_pal_retribution_aura") { }

        class spell_pal_retribution_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_retribution_aura_AuraScript);

            enum eSpells
            {
                RetributionDamage = 204011
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Victim = p_EventInfo.GetDamageInfo()->GetTarget();
                Unit* l_Attacker = p_EventInfo.GetDamageInfo()->GetActor();

                if (l_Caster == nullptr || l_Attacker == nullptr || l_Victim == nullptr)
                    return;

                l_Caster->CastSpell(l_Attacker, eSpells::RetributionDamage, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_retribution_aura_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_retribution_aura_AuraScript();
        }
};

/// last update : 7.1.5 Build 23420
/// Hand of the Protector - 213652
class spell_pal_hand_of_the_protector : public SpellScriptLoader
{
    public:
        spell_pal_hand_of_the_protector() : SpellScriptLoader("spell_pal_hand_of_the_protector") { }

        class spell_pal_hand_of_the_protector_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hand_of_the_protector_SpellScript);

            enum eSpells
            {
                ScatterTheShadowsAura = 209223
            };

            void HandleHeal(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                uint32 l_MissingLife = l_Target->GetMaxHealth() - l_Target->GetHealth();
                int32 l_Heal = CalculatePct(l_MissingLife, l_SpellInfo->Effects[p_EffIndex].BasePoints);
                l_Heal = l_Caster->SpellHealingBonusDone(l_Target, l_SpellInfo, l_Heal, p_EffIndex, DamageEffectType::HEAL);
                l_Heal = l_Target->SpellHealingBonusTaken(l_Caster, l_SpellInfo, l_Heal, DamageEffectType::HEAL);

                AuraEffect* l_AuraEffectScatter = l_Caster->GetAuraEffect(eSpells::ScatterTheShadowsAura, EFFECT_0);
                if (l_AuraEffectScatter)
                    AddPct(l_Heal, l_AuraEffectScatter->GetAmount());

                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_hand_of_the_protector_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_hand_of_the_protector_SpellScript();
        }
};

/// Last Update 7.0.3
/// Aegis of Light - 204150
class spell_pal_aegis_of_light : public SpellScriptLoader
{
    public:
        spell_pal_aegis_of_light() : SpellScriptLoader("spell_pal_aegis_of_light") { }

        class spell_pal_aegis_of_light_areaAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_aegis_of_light_areaAuraScript);

            std::vector<uint64> l_GroupInZone;

            enum eSpells
            {
                AegisOfLight = 204335
            };

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                float l_Radius = 10.0f;
                float l_ConeAngle = (M_PI * (2.0f/ 3.0f));

                ConditionContainer* l_ConditionsList = GetSpellInfo()->Effects[EFFECT_0].ImplicitTargetConditions;

                std::list<WorldObject*> l_Targets;
                JadeCore::WorldObjectSpellConeTargetCheck l_Check(l_ConeAngle, l_Radius, l_Caster, GetSpellInfo(), SpellTargetCheckTypes::TARGET_CHECK_ALLY, l_ConditionsList);
                JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellConeTargetCheck> l_Searcher(l_Caster, l_Targets, l_Check);
                l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                l_Targets.remove_if([this, l_Caster](WorldObject* p_Unit) -> bool
                {
                    if (p_Unit == nullptr || p_Unit->ToUnit() == nullptr)
                        return true;

                    if (!l_Caster->IsValidAssistTarget(p_Unit->ToUnit()))
                        return true;

                    if (l_Caster->GetGUID() == p_Unit->ToUnit()->GetGUID())
                        return true;

                    if (l_Caster->GetDistance(p_Unit) > 10.0f)
                        return true;

                    return false;
                });
                /// Check the new in party member in Range and add aura
                for (auto l_WorldObject : l_Targets)
                {
                    Unit* l_Unit = l_WorldObject->ToUnit();
                    if (l_Unit == nullptr)
                        continue;

                    if (std::find(l_GroupInZone.begin(), l_GroupInZone.end(), l_Unit->GetGUID()) == l_GroupInZone.end())
                    {
                        l_GroupInZone.push_back(l_Unit->ToUnit()->GetGUID());
                        l_Caster->CastSpell(l_Unit, eSpells::AegisOfLight, true);
                    }
                }

                /// Remove the party member no more in Range, or no more group member
                for (std::vector<uint64>::iterator l_It = l_GroupInZone.begin(); l_It != l_GroupInZone.end();)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(*l_It);

                    if (l_Target == nullptr || std::find(l_Targets.begin(), l_Targets.end(), l_Target) == l_Targets.end())
                    {
                        l_It = l_GroupInZone.erase(l_It);
                        if (l_Target != nullptr)
                            l_Target->RemoveAura(eSpells::AegisOfLight, l_Caster->GetGUID());
                    }
                    else
                        l_It++;
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                /// Remove all the aura in Party member from caster
                for (auto l_UnitGUID : l_GroupInZone)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(l_UnitGUID);

                    if (l_Target != nullptr)
                        l_Target->RemoveAura(eSpells::AegisOfLight, l_Caster->GetGUID());
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_pal_aegis_of_light_areaAuraScript::OnUpdate, EFFECT_0, SPELL_AURA_AREATRIGGER);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_aegis_of_light_areaAuraScript::OnRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_aegis_of_light_areaAuraScript();
        }
};

/// Last Update 7.0.3
/// Consecrated Ground - 204054
class spell_pal_consecrated_ground : public SpellScriptLoader
{
    public:
        spell_pal_consecrated_ground() : SpellScriptLoader("spell_pal_consecrated_ground") { }

        class spell_pal_consecrated_ground_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_consecrated_ground_AuraScript);

            enum eSpells
            {
                ConcecrationDamage          = 81297,
                ConcecrationHeal            = 204241,
                ConsecrationAreatrigger     = 26573
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_Caster == nullptr || l_SpellInfo == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::ConcecrationDamage)
                    return;

                AreaTrigger* l_AreaTrigger = l_Caster->GetAreaTrigger(eSpells::ConsecrationAreatrigger);

                if (l_AreaTrigger == nullptr)
                    return;

                l_Caster->CastSpell(l_AreaTrigger->GetPositionX(), l_AreaTrigger->GetPositionY(), l_AreaTrigger->GetPositionZ(), eSpells::ConcecrationHeal, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_consecrated_ground_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_consecrated_ground_AuraScript();
        }
};

/// Last Update 7.0.3
/// Consecrated Ground (heal) - 204241
class spell_pal_consecrated_ground_heal : public SpellScriptLoader
{
    public:
        spell_pal_consecrated_ground_heal() : SpellScriptLoader("spell_pal_consecrated_ground_heal") { }

        class spell_pal_consecrated_ground_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_consecrated_ground_heal_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                /// Up to 6 allies standing within your Consecration
                if (p_Targets.size() > 6)
                {
                    p_Targets.sort(JadeCore::HealthPctOrderPred());
                    p_Targets.resize(6);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_consecrated_ground_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_consecrated_ground_heal_SpellScript();
        }
};

/// Last Update - 7.3.5
/// Called by Healthcliff Immortality - 207599
class spell_pal_healthcliff_immortality : public SpellScriptLoader
{
    public:
        spell_pal_healthcliff_immortality() : SpellScriptLoader("spell_pal_healthcliff_immortality") { }

        enum eSpells
        {
            HealthcliffImmortality = 207599
        };

        class spell_pal_healthcliff_immortality_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_healthcliff_immortality_SpellScript);

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player || (l_Player->GetActiveSpecializationID() != SpecIndex::SPEC_PALADIN_HOLY && l_Player->GetActiveSpecializationID() != SpecIndex::SPEC_PALADIN_PROTECTION))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_healthcliff_immortality_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_healthcliff_immortality_SpellScript();
        }
};

/// Last Update - 7.3.5
/// Healthcliff Immortality - 207599 / Item - 137047
class PlayerScript_pal_healthcliff_immortality : public PlayerScript
{
    public:
        PlayerScript_pal_healthcliff_immortality() : PlayerScript("PlayerScript_pal_healthcliff_immortality") { }

        enum eItems
        {
            HealthcliffImmortality = 137047
        };

        enum eSpells
        {
            HealthcliffImmortalitySpell = 207599
        };

        void OnAfterUnequipItem(Player* p_Player, Item const* p_Item) override
        {
            if (!p_Player || !p_Item || p_Item->GetEntry() != eItems::HealthcliffImmortality)
                return;

            p_Player->RemoveAura(eSpells::HealthcliffImmortalitySpell);
        }

        void OnLogout(Player* p_Player) override
        {
            if (!p_Player || !p_Player->HasAura(eSpells::HealthcliffImmortalitySpell))
                return;

            p_Player->RemoveAura(eSpells::HealthcliffImmortalitySpell);
        }
};

/// Seraphim - 152262
class spell_pal_seraphim : public SpellScriptLoader
{
    public:
        spell_pal_seraphim() : SpellScriptLoader("spell_pal_seraphim") { }

        enum eSpells
        {
            ShieldOfTheRighteous = 53600
        };

        class spell_pal_seraphim_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_seraphim_SpellScript);

            SpellCastResult CheckCharge()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShieldOfTheRighteous);

                if (l_Player == nullptr || l_SpellInfo == nullptr)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Player->HasCharge(l_SpellInfo->ChargeCategoryEntry))
                    return SPELL_FAILED_NO_CHARGES_REMAIN;

                return SPELL_CAST_OK;
            }

            void HandleAfterHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShieldOfTheRighteous);

                if (l_Player == nullptr || l_SpellInfo == nullptr)
                    return;

                l_Player->ConsumeCharge(l_SpellInfo->ChargeCategoryEntry, l_SpellInfo->Id);
                if (l_Player->HasCharge(l_SpellInfo->ChargeCategoryEntry))
                {
                    if (Aura* l_Aura = l_Player->GetAura(GetSpellInfo()->Id))
                    {
                        l_Aura->SetDuration(l_Aura->GetDuration() + l_Aura->GetMaxDuration());
                        l_Player->ConsumeCharge(l_SpellInfo->ChargeCategoryEntry, l_SpellInfo->Id);
                    }
                }
                l_Player->SendSetSpellCharges(l_SpellInfo->ChargeCategoryEntry);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_seraphim_SpellScript::CheckCharge);
                AfterHit += SpellHitFn(spell_pal_seraphim_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_seraphim_SpellScript();
        }
};

/// Last Update 7.0.3
/// Last Defender - 203791
class spell_pal_last_defender : public SpellScriptLoader
{
    public:
        spell_pal_last_defender() : SpellScriptLoader("spell_pal_last_defender") { }

        class spell_pal_last_defender_areaAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_last_defender_areaAuraScript);

            std::vector<uint64> l_GroupInZone;

            enum eSpells
            {
                AuraOfSacrifice = 210372
            };

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                AuraEffect* l_AuraEffectRadius = p_AurEff->GetBase()->GetEffect(EFFECT_0);
                AuraEffect* l_AuraEffectDamage = p_AurEff->GetBase()->GetEffect(EFFECT_4);
                AuraEffect* l_AuraEffectMultiplier = p_AurEff->GetBase()->GetEffect(EFFECT_1);

                if (l_AuraEffectRadius == nullptr || l_AuraEffectDamage == nullptr || l_AuraEffectMultiplier == nullptr)
                    return;

                std::list<Unit*> l_TargetList;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(l_Caster, l_Caster, l_AuraEffectRadius->GetAmount());
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(l_Caster, l_TargetList, u_check);
                l_Caster->VisitNearbyObject(l_AuraEffectRadius->GetAmount(), searcher);

                uint32 l_NumberOfEnnemies = l_TargetList.size();
                int32 l_Amount = (1 - powf(1 - l_AuraEffectMultiplier->GetAmount() / 100.f, l_NumberOfEnnemies)) * 100;

                if (l_NumberOfEnnemies == 0) // Preventing a perma 1% value because x^0 = 1
                    l_Amount = 0;

                if (l_AuraEffectDamage->GetAmount() != l_Amount)
                    l_AuraEffectDamage->SetAmount(l_Amount);
            }

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* p_AuraEffect, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;

                Unit* l_Target = GetTarget();
                AuraEffect* l_AuraEffect = p_AuraEffect->GetBase()->GetEffect(EFFECT_4);

                if (l_AuraEffect == nullptr)
                    return;

                p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_AuraEffect->GetAmount());
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_pal_last_defender_areaAuraScript::OnUpdate, EFFECT_2, SPELL_AURA_AREATRIGGER);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_last_defender_areaAuraScript::CalculateAmount, EFFECT_3, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_last_defender_areaAuraScript::Absorb, EFFECT_3, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_last_defender_areaAuraScript();
        }
};

/// Last Update 7.0.3
/// Zeal - 217020
class spell_pal_zeal : public SpellScriptLoader
{
    public:
        spell_pal_zeal() : SpellScriptLoader("spell_pal_zeal") { }

        class spell_pal_zealr_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_zealr_SpellScript);

            uint8 m_NbTarget = 0;

            void HandleDamage(SpellEffIndex /*l_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                int32 l_Damage = GetHitDamage();
                for (uint8 i = 0; i < m_NbTarget; ++i)
                    l_Damage -= CalculatePct(l_Damage, GetSpellInfo()->Effects[EFFECT_5].BasePoints);
                SetHitDamage(l_Damage);
                m_NbTarget++;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_zealr_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_zealr_SpellScript();
        }
};

/// last update : 7.1.5 Build 23420
/// Called by Avenging Wrath - 31884 and Crusade - 231895 Called for Liadrin's Fury Unleashed - 208408
class spell_pal_liadrin_fury_unleashed : public SpellScriptLoader
{
    public:
        spell_pal_liadrin_fury_unleashed() : SpellScriptLoader("spell_pal_liadrin_fury_unleashed") { }

        class spell_pal_liadrin_fury_unleashed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_liadrin_fury_unleashed_AuraScript);

            enum eSpells
            {
                LiadrinFuryUnleashedPassive = 208408,
                LiadrinFuryUnleashed        = 208410
            };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::LiadrinFuryUnleashedPassive))
                    l_Caster->CastSpell(l_Caster, eSpells::LiadrinFuryUnleashed, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::LiadrinFuryUnleashed);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_liadrin_fury_unleashed_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_liadrin_fury_unleashed_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_liadrin_fury_unleashed_AuraScript();
        }
};

/// last update : 7.0.3
/// Eye for an Eye - 85804
class spell_pal_eye_for_an_eye : public SpellScriptLoader
{
    public:
        spell_pal_eye_for_an_eye() : SpellScriptLoader("spell_pal_eye_for_an_eye") { }

        class spell_pal_eye_for_an_eye_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_eye_for_an_eye_AuraScript);

            enum eSpells
            {
                EyeForAnEyeDamage = 205202
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                if (p_EventInfo.GetDamageInfo() == nullptr)
                    return;

                Unit* l_Victim = p_EventInfo.GetDamageInfo()->GetTarget();
                Unit* l_Attacker = p_EventInfo.GetDamageInfo()->GetActor();

                if (l_Victim == nullptr || l_Attacker == nullptr)
                    return;

                l_Victim->CastSpell(l_Attacker, eSpells::EyeForAnEyeDamage, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_eye_for_an_eye_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_eye_for_an_eye_AuraScript();
        }
};

/// Last Update 7.0.3
/// Word of Glory - 210191
class spell_pal_word_of_glory : public SpellScriptLoader
{
    public:
        spell_pal_word_of_glory() : SpellScriptLoader("spell_pal_word_of_glory") { }

        class spell_pal_word_of_glory_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_word_of_glory_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.push_back(l_Caster);

                uint8 l_MaxTarget = GetSpellInfo()->Effects[EFFECT_1].BasePoints;
                /// Max 3 targets and should be the more injured
                if (p_Targets.size() > l_MaxTarget)
                {
                    p_Targets.sort(JadeCore::HealthPctOrderPred());
                    p_Targets.resize(l_MaxTarget);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_word_of_glory_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_word_of_glory_SpellScript();
        }
};

/// Last Update 7.1.5
/// Divine Intervention - 213313
class spell_pal_divine_intervention : public SpellScriptLoader
{
    public:
        spell_pal_divine_intervention() : SpellScriptLoader("spell_pal_divine_intervention") { }

        class spell_pal_divine_intervention_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_divine_intervention_AuraScript);

            std::vector<uint64> l_GroupInZone;

            enum eSpells
            {
                AuraOfSacrifice     = 210372,
                Forbearance         = 25771,
                DivineShield        = 642
            };

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* p_AuraEffect, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfoDivineShield = sSpellMgr->GetSpellInfo(eSpells::DivineShield);
                AuraEffect* l_AuraEffectHealPct = p_AuraEffect->GetBase()->GetEffect(EFFECT_1);

                if (l_AuraEffectHealPct == nullptr || !l_SpellInfoDivineShield)
                    return;

                Player* l_Player = l_Target->ToPlayer();
                SpellCooldownsEntry const* l_SpellCooldownsEntry = l_SpellInfoDivineShield->GetSpellCooldowns();

                if (l_Player == nullptr || !l_SpellCooldownsEntry)
                    return;

                /// Cannot occur while Divine Shield is on cooldown or Forbearance is active.
                if (l_Player->HasAura(eSpells::Forbearance) || l_Player->HasSpellCooldown(eSpells::DivineShield))
                    return;

                /// Any attack which would kill you instead reduces you to 20% of your maximum health and triggers Divine Shield.
                if (p_DmgInfo.GetAmount() >= l_Player->GetHealth())
                {
                    p_AbsorbAmount = p_DmgInfo.GetAmount();
                    l_Target->SetHealth(l_Player->GetMaxHealth() * (l_AuraEffectHealPct->GetAmount() / 100.0f));
                    l_Target->CastSpell(l_Target, eSpells::DivineShield, true);
                    int32 l_RecoveryTime = l_SpellCooldownsEntry->RecoveryTime;
                    if (AuraEffect* l_AuraEffect = GetEffect(EFFECT_2))
                        AddPct(l_RecoveryTime, l_AuraEffect->GetAmount());
                    l_Player->AddSpellCooldown(eSpells::DivineShield, 0, l_RecoveryTime, true);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_divine_intervention_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_divine_intervention_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_divine_intervention_AuraScript();
        }
};

/// Last Update 7.1.5
/// Shield of Vengeance - 184662
class spell_pal_shield_of_vengeance : public SpellScriptLoader
{
    public:
        spell_pal_shield_of_vengeance() : SpellScriptLoader("spell_pal_shield_of_vengeance") { }

        class spell_pal_shield_of_vengeance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_shield_of_vengeance_AuraScript);

            enum eSpells
            {
                ShieldOfVengeanceDamage = 184689,
                AegisjalmurAura         = 225036,
                AegisjalmurDelay        = 225056
            };

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                /// absorbs ${$AP*$s2(20)}
                p_Amount = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * l_SpellInfo->Effects[EFFECT_1].BasePoints;
            }

            void Absorb(AuraEffect* p_AuraEffect, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!p_AbsorbAmount)
                {
                    if (Aura* l_Aura = p_AuraEffect->GetBase())
                        l_Caster->RemoveAura(l_Aura, AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);
                }
            }

            void OnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = p_AuraEffect->GetBase();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Aura || !l_SpellInfo)
                    return;

                int32 l_Damage = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * l_SpellInfo->Effects[EFFECT_1].BasePoints;
                if (p_AuraEffect->GetAmount() == 0)
                    l_Caster->CastCustomSpell(l_Caster, eSpells::ShieldOfVengeanceDamage, &l_Damage, NULL, NULL, true);

                if (l_Caster->HasAura(eSpells::AegisjalmurDelay))
                {
                    SpellInfo const* l_SpellInfoAegisjalmurAura = sSpellMgr->GetSpellInfo(eSpells::AegisjalmurAura);
                    if (!l_SpellInfoAegisjalmurAura)
                        return;

                    l_Caster->SetHealth(l_SpellInfoAegisjalmurAura->Effects[EFFECT_1].BasePoints);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_shield_of_vengeance_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_shield_of_vengeance_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_shield_of_vengeance_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_shield_of_vengeance_AuraScript();
        }
};

/// Last Update 7.0.3
/// Shield of Vengeance (damage) - 184689
class spell_pal_shield_of_vengeance_damage : public SpellScriptLoader
{
    public:
        spell_pal_shield_of_vengeance_damage() : SpellScriptLoader("spell_pal_shield_of_vengeance_damage") { }

        class spell_pal_shield_of_vengeance_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_shield_of_vengeance_damage_SpellScript);

            int32 m_TotalTargets = 0;

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                m_TotalTargets = p_Targets.size();
            }


            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (m_TotalTargets)
                    SetHitDamage(GetHitDamage() / m_TotalTargets);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_shield_of_vengeance_damage_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_pal_shield_of_vengeance_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_shield_of_vengeance_damage_SpellScript();
        }
};

/// Crusade - 231895
class PlayerScript_crusade : public PlayerScript
{
    public:
        PlayerScript_crusade() :PlayerScript("PlayerScript_crusade") {}

        enum eSpells
        {
            CrusadeAura = 231895
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (p_After)
                return;

            if (p_Regen)
                return;

            int32 l_DiffValue = p_NewValue - p_OldValue;

            if (l_DiffValue >= 0)
                return;

            if (p_Player->getClass() == CLASS_PALADIN && p_Player->GetActiveSpecializationID() == SPEC_PALADIN_RETRIBUTION && p_Power == POWER_HOLY_POWER && p_Player->HasAura(eSpells::CrusadeAura))
            {
                /// Get the power earn (if > 0 ) or consum (if < 0)

                Aura* l_Aura = p_Player->GetAura(eSpells::CrusadeAura, EFFECT_0);
                l_DiffValue *= -1;

                if (l_Aura == nullptr)
                    return;

                uint32 l_Duration = l_Aura->GetDuration();
                for (uint8 i = 0; i < l_DiffValue; ++i)
                {
                    l_Aura->ModStackAmount(1);
                    l_Aura->SetDuration(l_Duration);
                }
            }
        }
};

/// Last Update 7.0.3
/// Holy Wrath - 210220
class spell_pal_holy_wrath : public SpellScriptLoader
{
    public:
        spell_pal_holy_wrath() : SpellScriptLoader("spell_pal_holy_wrath") { }

        class spell_pal_holy_wrath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_holy_wrath_SpellScript);

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                uint8 l_MaxTarget = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                if (p_Targets.size() > l_MaxTarget)
                    p_Targets.resize(l_MaxTarget);
            }


            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                int32 l_MissingHealth = l_Caster->GetMaxHealth() - l_Caster->GetHealth();
                int32 l_Damage = 0;

                if (l_Caster->CanApplyPvPSpellModifiers() || l_Target->IsPlayer()) ///< Deals 35% of missing health against enemy players and pets.
                    l_Damage = CalculatePct(l_MissingHealth, l_SpellInfo->Effects[EFFECT_3].BasePoints);
                else ///< Deals 200% of missing health against creatures.
                    l_Damage = CalculatePct(l_MissingHealth, l_SpellInfo->Effects[EFFECT_2].BasePoints);

                l_Damage = uint32(std::min(CalculatePct(l_Caster->GetMaxHealth(), 120), (uint64)l_Damage));

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_wrath_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_pal_holy_wrath_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_holy_wrath_SpellScript();
        }
};

/// Last Update 7.0.3
/// Bastion of Light - 204035
class spell_pal_bastion_of_light : public SpellScriptLoader
{
    public:
        spell_pal_bastion_of_light() : SpellScriptLoader("spell_pal_bastion_of_light") { }

        class spell_pal_bastion_of_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_bastion_of_light_SpellScript);

            enum eSpells
            {
                ShieldoftheRighteous = 53600
            };

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                /// Immediately grants 3 charges of Shield of the Righteous.
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShieldoftheRighteous);
                if (SpellCategoryEntry const* l_CastegoryEntry = l_SpellInfo->ChargeCategoryEntry)
                {
                    for (uint8 i = 0; i < GetSpellInfo()->Effects[EFFECT_0].BasePoints; ++i)
                        l_Player->RestoreCharge(l_CastegoryEntry);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_bastion_of_light_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_bastion_of_light_SpellScript();
        }
};

/// Last Update 7.0.3
/// Divine Storm - 53385
class spell_pal_divine_storm : public SpellScriptLoader
{
    public:
        spell_pal_divine_storm() : SpellScriptLoader("spell_pal_divine_storm") { }

        class spell_pal_divine_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_storm_SpellScript);

            enum eSpells
            {
                DivineStormDamage   = 224239,
                DivineTempestAura   = 186773,
                DivineTempestArea   = 186775,
                HealingStormAura    = 193058,
                HealingStormHeal    = 215257,
                DSVisuals           = 174333
            };

            void HandleDummy(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::DivineTempestAura)  ///< Regular cast (no divine tempest trait)
                    || GetSpell()->IsTriggered())                   ///< From divine tempest areatrigger
                    l_Caster->CastSpell(l_Target, eSpells::DivineStormDamage, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::DivineTempestAura) && !GetSpell()->IsTriggered())
                    l_Caster->CastSpell(l_Caster, eSpells::DivineTempestArea, true);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::DSVisuals, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HealingStormAura);
                if (!l_Caster || !l_SpellInfo || !l_Caster->HasAura(eSpells::HealingStormAura))
                    return;

                /// Healing up to 6 allied players
                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target)
                        return true;

                    Unit* l_Target = p_Target->ToUnit();
                    if (!l_Target || !l_Target->IsPlayer())
                        return true;

                    return false;
                });

                int32 l_NbTarget = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                if (p_Targets.size() > l_NbTarget)
                    p_Targets.resize(l_NbTarget);
            }

            void HandleOnEffectHealHitTarget(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::HealingStormAura))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::HealingStormHeal, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_divine_storm_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                AfterCast += SpellCastFn(spell_pal_divine_storm_SpellScript::HandleAfterCast);

                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_divine_storm_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_pal_divine_storm_SpellScript::HandleOnEffectHealHitTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_divine_storm_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Divine Storm - 224239, 213843
class spell_pal_divine_storm_damage : public SpellScriptLoader
{
public:
    spell_pal_divine_storm_damage() : SpellScriptLoader("spell_pal_divine_storm_damage") { }

    class spell_pal_divine_storm_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_divine_storm_damage_SpellScript);

        enum eSpells
        {
            ScarletInquisitorsExpurgation = 248289
        };

        void HandleAfterCast()
        {
            if (Unit* l_Caster = GetCaster())
                l_Caster->RemoveAura(eSpells::ScarletInquisitorsExpurgation);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_pal_divine_storm_damage_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pal_divine_storm_damage_SpellScript();
    }
};


/// Last Update 7.0.3
/// Templar's Verdict - 85256
class spell_pal_templar_verdict : public SpellScriptLoader
{
    public:
        spell_pal_templar_verdict() : SpellScriptLoader("spell_pal_templar_verdict") { }

        class spell_pal_templar_verdict_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_templar_verdict_SpellScript);

            enum eSpells
            {
                TemplarsVerdictmDamage = 224266
            };

            void HandleDummy(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::TemplarsVerdictmDamage, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_templar_verdict_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_templar_verdict_SpellScript();
        }
};

/// Last Update 7.1.5
/// Light of the Protector - 184092
class spell_pal_light_of_the_protector : public SpellScriptLoader
{
    public:
        spell_pal_light_of_the_protector() : SpellScriptLoader("spell_pal_light_of_the_protector") { }

        class spell_pal_light_of_the_protector_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_light_of_the_protector_SpellScript);

            enum eSpells
            {
                ScatterTheShadowsAura = 209223
            };

            void HandleHeal()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                uint32 l_MissingLife = l_Caster->GetMaxHealth() - l_Caster->GetHealth();
                int32 l_Heal = CalculatePct(l_MissingLife, l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints);
                l_Heal = l_Caster->SpellHealingBonusDone(l_Caster, l_SpellInfo, l_Heal, SpellEffIndex::EFFECT_0, DamageEffectType::HEAL);
                l_Heal = l_Caster->SpellHealingBonusTaken(l_Caster, l_SpellInfo, l_Heal, DamageEffectType::HEAL);

                AuraEffect* l_AuraEffectScatter = l_Caster->GetAuraEffect(eSpells::ScatterTheShadowsAura, EFFECT_0);
                if (l_AuraEffectScatter)
                    AddPct(l_Heal, l_AuraEffectScatter->GetAmount());

                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_light_of_the_protector_SpellScript::HandleHeal);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_light_of_the_protector_SpellScript();
        }
};

/// Last Update 7.1.5
class PlayerScript_pal_after_cast : public PlayerScript
{
    public:
        PlayerScript_pal_after_cast() :PlayerScript("PlayerScript_pal_after_cast") { }

        enum eSpells
        {
            DivinePurposeAura       = 223819,
            DivinePurposePassive    = 223817,
            FiresJustice            = 209785,
            Crusade                 = 231895
        };

        void AfterSpellCast(Player* p_Player, Spell* p_Spell, bool p_SkipCheck)
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_PALADIN_RETRIBUTION)
                return;

            /// Divine Purpose - The Fires of Justice
            if (p_Spell->GetSpellInfo() != nullptr)
            {
                AuraEffect* l_AuraEffectFiresofJustice  = p_Player->GetAuraEffect(eSpells::FiresJustice, EFFECT_0);
                AuraEffect* l_AuraEffectDivinePurpose   = p_Player->GetAuraEffect(eSpells::DivinePurposeAura , EFFECT_0);
                Aura*       l_AuraDivinePurposePassive  = p_Player->GetAura(eSpells::DivinePurposePassive);

                if (l_AuraEffectFiresofJustice != nullptr)
                {
                    if (l_AuraEffectFiresofJustice->IsAffectingSpell(p_Spell->GetSpellInfo()))
                        p_Player->RemoveAura(eSpells::FiresJustice);
                }

                if (l_AuraEffectDivinePurpose != nullptr)
                {
                    if (l_AuraEffectDivinePurpose->IsAffectingSpell(p_Spell->GetSpellInfo())) {
                        p_Player->RemoveAura(eSpells::DivinePurposeAura);
                        uint32 l_cost = 0;
                        for (auto l_power : p_Spell->GetSpellInfo()->SpellPowers)
                        {
                            if (l_power->PowerType == Powers::POWER_HOLY_POWER)
                            {
                                l_cost = l_power->Cost;
                                break;
                            }
                        }

                        if (Aura* l_Aura = p_Player->GetAura(eSpells::Crusade, EFFECT_0)) {
                            uint32 l_amount = l_Aura->GetStackAmount() + l_cost > 15 ? 15 : l_Aura->GetStackAmount() + l_cost;
                            l_Aura->SetStackAmount(l_amount);
                        }
                    }
                }

                if (l_AuraDivinePurposePassive && p_Spell->GetSpellInfo()->GetSpellPower(Powers::POWER_HOLY_POWER))
                {
                    if (SpellInfo const* l_SpellInfo = l_AuraDivinePurposePassive->GetSpellInfo())
                    {
                        if (roll_chance_i(l_SpellInfo->ProcChance))
                        {
                            p_Player->AddDelayedEvent([p_Player]() -> void
                            {
                                p_Player->CastSpell(p_Player, eSpells::DivinePurposeAura, true);
                            }, 10);
                        }
                    }
                }
            }
        }
};

/// Last Update 7.1.5
/// Divine Hammer - 198034
class spell_pal_divine_hammer : public SpellScriptLoader
{
    public:
        spell_pal_divine_hammer() : SpellScriptLoader("spell_pal_divine_hammer") { }

        enum eSpells
        {
            DivineHammer            = 198034,
            T19Retribution4PBonus   = 211448,
            PowerOfTheHighlord      = 231372,
            RighteousVerdict        = 238996,
            T20Retribution2PBonus   = 242266
        };

        class spell_pal_divine_hammer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_hammer_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// Item - Paladin T19 Retribution 4P Bonus
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Retribution4PBonus, EFFECT_0))
                    if (roll_chance_i(l_AuraEffect->GetSpellInfo()->ProcChance))
                        l_Caster->CastSpell(l_Caster, eSpells::PowerOfTheHighlord, true);

                l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, GetSpellInfo()->Effects[EFFECT_1].BasePoints, POWER_HOLY_POWER);
                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::DivineHammerTargets].clear();

                /// Correct the duration because the aura is affected by haste and the amplitude is changed.
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::DivineHammer))
                {
                    int32 l_BaseMaxDuration = l_Aura->GetSpellInfo()->GetMaxDuration();
                    int32 l_BaseTicksCount = l_BaseMaxDuration / l_Aura->GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].Amplitude;
                    int32 l_Timer = l_Aura->GetEffect(EFFECT_0)->GetAmplitude();

                    l_Aura->SetDuration(l_BaseTicksCount * l_Timer);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_divine_hammer_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_divine_hammer_SpellScript();
        }

        class spell_pal_divine_hammer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_divine_hammer_AuraScript);

            bool m_GeneratePower = false;

            void AfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                m_GeneratePower = false;
            }

            void OnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || m_GeneratePower)
                    return;

                if (!l_Caster->SelectNearbyTarget(l_Caster, 8.0f))
                    return;

                int32 l_Amount = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T20Retribution2PBonus, SpellEffIndex::EFFECT_1))
                    l_Amount += l_AuraEffect->GetAmount();

                l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, l_Amount, Powers::POWER_HOLY_POWER);

                m_GeneratePower = true;
            }

            void AfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::RighteousVerdict);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_divine_hammer_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_divine_hammer_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_divine_hammer_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_divine_hammer_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Light of Dawn - 85222
class spell_pal_light_of_dawn : public SpellScriptLoader
{
    public:
        spell_pal_light_of_dawn() : SpellScriptLoader("spell_pal_light_of_dawn") { }

        class spell_pal_light_of_dawn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_light_of_dawn_SpellScript);

            enum eSpells
            {
                LightOfDawnHeal          = 225311,
                MaraadsDyingBreathAura   = 234848,
                MaraadsDyingBreathBonus  = 234862,
                LightOfTheMartyr         = 219562,
                TheToplessTower          = 248033,
                AvengingWrath            = 31842,
                SacredDawnAura           = 238132,
                SacredDawn               = 243174
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::SacredDawnAura))
                    for (auto l_Target : p_Targets)
                        if (l_Target)
                            l_Caster->CastSpell(static_cast<Unit*>(l_Target), eSpells::SacredDawn, true);


                /// Healing up to 5 allies
                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                if (p_Targets.size() > GetSpellInfo()->Effects[EFFECT_0].BasePoints)
                    p_Targets.resize(GetSpellInfo()->Effects[EFFECT_0].BasePoints);
            }

            void HandlDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::LightOfDawnHeal, true);

                if (l_Caster->HasAura(eSpells::MaraadsDyingBreathAura))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::MaraadsDyingBreathBonus, true);
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::LightOfTheMartyr))
                        l_Aura->SetScriptGuid(0, 0);
                }

                /// Legendary: The Topless Tower
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::TheToplessTower))
                {
                    if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0))
                    {
                        if (roll_chance_i(l_AuraEffect->GetAmount()))
                        {
                            if (AuraEffect* l_AuraEffect2 = l_Aura->GetEffect(EFFECT_1))
                            {
                                int32 l_Duration = l_AuraEffect2->GetAmount() * IN_MILLISECONDS;
                                if (Aura* l_AvengingWrath = l_Caster->GetAura(eSpells::AvengingWrath))
                                {
                                    if (l_AvengingWrath->GetDuration() < l_Duration)
                                        l_AvengingWrath->SetDuration(l_Duration);
                                }
                                else if (Aura* l_AvengingWrath = l_Caster->AddAura(eSpells::AvengingWrath, l_Caster))
                                    l_AvengingWrath->SetDuration(l_Duration);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_light_of_dawn_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_pal_light_of_dawn_SpellScript::HandlDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_light_of_dawn_SpellScript();
        }
};

class PlayerScript_Aura_of_devotion : public PlayerScript
{
    public:
        PlayerScript_Aura_of_devotion() :PlayerScript("PlayerScript_Aura_of_devotion") { }

        enum eSpells
        {
            AuraOfDevotion = 210320
        };

        void OnLogout(Player* p_Player)
        {
            p_Player->RemoveAura(eSpells::AuraOfDevotion);
        }
};

/// Last Update 7.0.3
/// Wake of Ashes- 205273
class spell_pal_wake_of_ashes : public SpellScriptLoader
{
    public:
        spell_pal_wake_of_ashes() : SpellScriptLoader("spell_pal_wake_of_ashes") { }

        class spell_pal_wake_of_ashes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_wake_of_ashes_AuraScript);

            enum eSpells
            {
                AshestoAshes        = 179546
            };

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::AshestoAshes))
                    p_Amount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_wake_of_ashes_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_wake_of_ashes_AuraScript();
        }

        class spell_pal_wake_of_ashes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_wake_of_ashes_SpellScript);

            enum eSpells
            {
                AshestoAshes = 179546,
                HolyPowerGenerate = 218001,
                WakeOfAshesStun = 205290
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::AshestoAshes))
                    l_Caster->CastSpell(l_Caster, eSpells::HolyPowerGenerate, true);
            }

            void HandlDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->GetCreatureType() == CREATURE_TYPE_DEMON || l_Target->GetCreatureType() == CREATURE_TYPE_UNDEAD)
                    l_Caster->CastSpell(l_Target, eSpells::WakeOfAshesStun, true);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pal_wake_of_ashes_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_pal_wake_of_ashes_SpellScript::HandlDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_wake_of_ashes_SpellScript();
        }
};

/// Last Update 7.0.3
/// Echo of the Highlord - 186788
class spell_pal_echo_of_the_highlord : public SpellScriptLoader
{
    public:
        spell_pal_echo_of_the_highlord() : SpellScriptLoader("spell_pal_echo_of_the_highlord") { }

        class spell_pal_echo_of_the_highlord_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_echo_of_the_highlord_AuraScript);

            enum eSpells
            {
                TemplarVerdict  = 224266,
                DivineStorm     = 224239,
                MiniAshbringer  = 186811,
                EchoDSVisual    = 186878,
                EchoTVVisual    = 186802
            };

            enum eData
            {
                MiniAshbringerEntry = 94358
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->m_SpellHelper.GetBool(eSpellHelpers::EchoOfTheHighlord))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::EchoOfTheHighlord) = true;
                    return false;
                }

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::TemplarVerdict && l_SpellInfo->Id != eSpells::DivineStorm))
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                /// Basically every time you use TV or DS a little mini ashbringer pops up and does another 10% dmg hi
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                SpellInfo const* l_SpellInfoProc = l_DamageInfo->GetSpellInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                Unit* l_Victim = l_DamageInfo->GetTarget();

                Spell const* l_Spell = l_DamageInfo->GetSpell();

                if (!l_SpellInfoProc || !l_SpellInfo || !l_Player || !l_Victim || !l_Spell)
                    return;

                Unit* l_Actor = l_Spell->GetCaster();
                if (!l_Actor || l_Actor->GetEntry() == eData::MiniAshbringerEntry)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MiniAshbringer, true);

                for (Unit* l_Control : l_Caster->m_Controlled)
                {
                    if (l_Control->GetEntry() == eData::MiniAshbringerEntry)
                    {
                        if (l_SpellInfoProc->Id == eSpells::DivineStorm)
                            l_Control->CastSpell(l_Victim, eSpells::EchoDSVisual, false);
                        else
                            l_Control->CastSpell(l_Victim, eSpells::EchoTVVisual, false);

                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::EchoOfTheHighlord) = false;

                        float l_Amount = CalculatePct((float)l_SpellInfoProc->Effects[EFFECT_0].BasePoints, (float)l_SpellInfo->Effects[EFFECT_0].BasePoints);
                        int32 l_AmountCeil = (int32)ceil(l_Amount);

                        l_Caster->CastCustomSpell(l_Victim, l_SpellInfoProc->Id, &l_AmountCeil, nullptr, nullptr, true);

                        l_Control->ToCreature()->DespawnOrUnsummon();

                        break;
                    }
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_echo_of_the_highlord_AuraScript::HandleOnCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_echo_of_the_highlord_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_echo_of_the_highlord_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Tyr's Deliverance - 200653
class spell_pal_tyrs_deliverance : public SpellScriptLoader
{
    public:
        spell_pal_tyrs_deliverance() : SpellScriptLoader("spell_pal_tyrs_deliverance") { }

        class spell_pal_tyrs_deliverance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_tyrs_deliverance_SpellScript);

            enum eSpells
            {
                TyrsDeliveranceHeal = 200654
            };

            uint64 m_TargetGuid = 0;

            void FilterTarget(std::list<WorldObject*>& l_TargetList)
            {
                l_TargetList.remove_if([](WorldObject* l_Target) -> bool
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        return true;

                    return false;
                });

                if (l_TargetList.empty())
                    return;

                l_TargetList.sort(JadeCore::HealthPctOrderPredPlayer());

                m_TargetGuid = l_TargetList.front()->GetGUID();
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, m_TargetGuid);
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::TyrsDeliveranceHeal, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_tyrs_deliverance_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                AfterCast += SpellCastFn(spell_pal_tyrs_deliverance_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_tyrs_deliverance_SpellScript();
        }
};

/// Crusader's Judgment - 204023
/// Updated as of 7.0.3 - 22522
class spell_pal_crusaders_judgment : public SpellScriptLoader
{
    public:
        spell_pal_crusaders_judgment() : SpellScriptLoader("spell_pal_crusaders_judgment")
        {
        }

        class aura_impl : public AuraScript
        {
            PrepareAuraScript(aura_impl);

            enum eSpells
            {
                AvengersShield = 31935
            };

            void HandleProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                if (!GetCaster())
                    return;

                /// Judgment can also trigger Grand Crusader.
                /// you have a 15% chance to reset the remaining cooldown on Avenger's Shield.
                if (Player* l_Player = GetCaster()->ToPlayer())
                    l_Player->RemoveSpellCooldown(eSpells::AvengersShield, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(aura_impl::HandleProc, EFFECT_0, SPELL_AURA_MOD_MAX_CHARGES);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new aura_impl();
        };
};

/// Last Update 7.1.5 Build 23420
/// Called by Faith's Armor - 211903
class spell_pal_faiths_armor : public SpellScriptLoader
{
    public:
        spell_pal_faiths_armor() : SpellScriptLoader("spell_pal_faiths_armor") { }

        class spell_pal_faiths_armor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_faiths_armor_AuraScript);

            enum eSpells
            {
                FaithsArmorAuraRank1    = 209225,
                FaithsArmorAuraRank2    = 211912,
                FaithsArmorAuraRank3    = 211913,
            };

            void HandleAfterEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::array<uint32, 3> m_spellIds =
                { {
                    eSpells::FaithsArmorAuraRank1,
                    eSpells::FaithsArmorAuraRank2,
                    eSpells::FaithsArmorAuraRank3
                } };

                int32 l_Armor = 0;
                for (uint8 l_I = 0; l_I < 3; ++l_I)
                {
                    if (Aura* l_Aura = l_Caster->GetAura(m_spellIds[l_I]))
                    {
                        if (SpellInfo const* l_SpellInfoFaithsArmorRank = l_Aura->GetSpellInfo())
                            l_Armor = l_SpellInfoFaithsArmorRank->Effects[1].BasePoints;
                        break;
                    }
                }

                if (!l_Armor)
                    return;

                const_cast<AuraEffect*>(aurEff)->ChangeAmount(l_Armor);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_faiths_armor_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_MOD_RESISTANCE_PCT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_faiths_armor_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Protection Of Tyr - 200430
class spell_pal_protection_of_tyr : public SpellScriptLoader
{
    public:
        spell_pal_protection_of_tyr() : SpellScriptLoader("spell_pal_protection_of_tyr") { }

        class spell_pal_protection_of_tyr_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_protection_of_tyr_AuraScript);

            enum eSpells
            {
                AuraMastery = 31821
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::AuraMastery)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_protection_of_tyr_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_protection_of_tyr_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Vindicator - 200373
class spell_pal_vindicator : public SpellScriptLoader
{
    public:
        spell_pal_vindicator() : SpellScriptLoader("spell_pal_vindicator") { }

        class spell_pal_vindicator_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_vindicator_AuraScript);

            enum eSpells
            {
                AvengingWrath = 31842,
                AvengingCrusader = 216331
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::AvengingWrath && l_SpellInfo->Id != eSpells::AvengingCrusader))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_vindicator_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_vindicator_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blade of Wrath - 231843
class spell_pal_blade_of_wrath : public SpellScriptLoader
{
    public:
        spell_pal_blade_of_wrath() : SpellScriptLoader("spell_pal_blade_of_wrath") { }

        class spell_pal_blade_of_wrath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_blade_of_wrath_AuraScript);

            enum eSpells
            {
                BladeOfJustice = 184575
            };

            void HandleAfterEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->RemoveSpellCooldown(eSpells::BladeOfJustice, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_blade_of_wrath_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_blade_of_wrath_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Flash of Light - 19750
/// Called by Holy Light - 82326
/// Called for The Light Saves - 200421
class spell_pal_the_light_saves : public SpellScriptLoader
{
    public:
        spell_pal_the_light_saves() : SpellScriptLoader("spell_pal_the_light_saves") { }

        class spell_pal_the_light_saves_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_the_light_saves_SpellScript);

            enum eSpells
            {
                TheLightSavesAura   = 200421,
                TheLightSavesBonus  = 200423,
                TheLightSavesMalus  = 211426,
                BeaconOfLight       = 53563,
                BeaconOfFaith       = 156910,
                BeaconOfVirtue      = 200025
            };

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                int32 l_Heal = GetHitHeal();
                if (!l_Caster || !l_Target || !l_Heal)
                    return;

                if (AuraEffect* l_Effect = l_Caster->GetAuraEffect(eSpells::TheLightSavesBonus, EFFECT_0))
                {
                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::TheLightSavesAura, EFFECT_0))
                    {
                        if (l_Target->HealthBelowPct(l_AuraEffect->GetAmount()) && (
                            l_Target->HasAura(eSpells::BeaconOfLight, l_Caster->GetGUID()) ||
                            l_Target->HasAura(eSpells::BeaconOfFaith, l_Caster->GetGUID()) ||
                            l_Target->HasAura(eSpells::BeaconOfVirtue, l_Caster->GetGUID())
                            ))
                        {
                            SetHitHeal(l_Heal + CalculatePct(l_Heal, l_Effect->GetAmount()));
                            l_Caster->CastSpell(l_Caster, eSpells::TheLightSavesMalus, true);
                            l_Caster->RemoveAurasDueToSpell(eSpells::TheLightSavesBonus);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_the_light_saves_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_the_light_saves_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Pöwer of the Silver Hand - 200656
class spell_pal_power_of_the_silver_hand_damages : public SpellScriptLoader
{
    public:
        spell_pal_power_of_the_silver_hand_damages() : SpellScriptLoader("spell_pal_power_of_the_silver_hand_damages") { }

        class spell_pal_power_of_the_silver_hand_damages_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_power_of_the_silver_hand_damages_AuraScript);

            enum eSpells
            {
                PowerOfTheSilverHand = 200657
            };

            void HandleAfterEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::PowerOfTheSilverHand) = 0;
                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PowerOfTheSilverHand) = true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                int32 l_Amount = l_DamageInfo->GetAmount();
                int32 l_Difference = std::max(0, l_Amount - l_DamageInfo->GetOverHeal());

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::PowerOfTheSilverHand) += CalculatePct(10, l_Difference);

                int32 l_Bonus = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::PowerOfTheSilverHand);

                l_Caster->CastCustomSpell(l_Caster, eSpells::PowerOfTheSilverHand, &l_Bonus, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_power_of_the_silver_hand_damages_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnProc += AuraProcFn(spell_pal_power_of_the_silver_hand_damages_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_power_of_the_silver_hand_damages_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Holy Shock (heal) - 25914
/// Called for Power of the Silver Hand - 200474
class spell_pal_power_of_the_silver_hand_bonus : public SpellScriptLoader
{
    public:
        spell_pal_power_of_the_silver_hand_bonus() : SpellScriptLoader("spell_pal_power_of_the_silver_hand_bonus") { }

        class spell_pal_power_of_the_silver_hand_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_power_of_the_silver_hand_bonus_SpellScript);

            enum eSpells
            {
                PowerOfTheSilverHandAura = 200474,
                PowerOfTheSilverHandProc = 200656,
                PowerOfTheSilverHand     = 200657
            };

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                int32 l_Heal = GetHitHeal();
                if (!l_Caster || !l_Heal)
                    return;

                if (AuraEffect const* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::PowerOfTheSilverHand, EFFECT_0))
                    SetHitHeal(l_Heal + l_AuraEffect->GetAmount());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_power_of_the_silver_hand_bonus_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_power_of_the_silver_hand_bonus_SpellScript();
        }
};

/// Last Update 7.1.5
/// Divine Purpose - 197646
/// Called by Holy Shock - 20473, Light Of Dawn - 85222
class spell_pal_divine_purpose : public SpellScriptLoader
{
    public:
        spell_pal_divine_purpose() : SpellScriptLoader("spell_pal_divine_purpose") { }

        class spell_pal_divine_purpose_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_purpose_SpellScript);

            enum eSpells
            {
                DivinePurpose = 197646,
                HolyShock = 20473,
                LightofDawn = 85222,
                DivinePurposeLightOfDawn = 216413,
                DivinePurposeHolyShock = 216411
            };

            void HandleDummy()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;

                if (!l_Caster || !(l_Player = l_Caster->ToPlayer()) || !l_Player->HasAura(eSpells::DivinePurpose))
                    return;

                SpellInfo const* l_DivinePurposeInfo = sSpellMgr->GetSpellInfo(eSpells::DivinePurpose);
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo || !l_DivinePurposeInfo)
                    return;

                if (!roll_chance_i(l_DivinePurposeInfo->Effects[0].BasePoints))
                    return;

                switch (l_SpellInfo->Id)
                {
                case eSpells::LightofDawn:
                    l_Caster->CastSpell(l_Caster, eSpells::DivinePurposeLightOfDawn, true);
                    break;
                case eSpells::HolyShock:
                    l_Caster->CastSpell(l_Caster, eSpells::DivinePurposeHolyShock, true);
                    break;
                default:
                    return;
                }

                l_Player->RemoveSpellCooldown(l_SpellInfo->Id, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_divine_purpose_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_divine_purpose_SpellScript();
        }
};

/// Last Update 7.3.5
/// Called Divine Purpose Light Of Dawn -216413
class spell_pal_divine_purpose_lod : public SpellScriptLoader
{
    public:
        spell_pal_divine_purpose_lod() : SpellScriptLoader("spell_pal_divine_purpose_lod") { }

        class spell_pal_divine_purpose_lod_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_divine_purpose_lod_AuraScript);

            enum eSpells
            {
                LightofDawn = 85222,
                DivinePurposeLightOfDawn = 216413
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (l_Spell->IsTriggered())
                    l_Caster->CastSpell(l_Caster, eSpells::DivinePurposeLightOfDawn, true);

            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pal_divine_purpose_lod_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_divine_purpose_lod_AuraScript();
        }
};

/// Last Update 7.1.5
/// Greater Blessing of King - 203538
class spell_pal_greater_blessing_of_king : public SpellScriptLoader
{
    public:
        spell_pal_greater_blessing_of_king() : SpellScriptLoader("spell_pal_greater_blessing_of_king") { }

        class spell_pal_greater_blessing_of_king_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_greater_blessing_of_king_AuraScript);

            void CalculateAmount(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;
                AuraEffect* l_Effect = const_cast<AuraEffect *>(p_AuraEffect);

                if (!l_Caster || !(l_Player = l_Caster->ToPlayer()) || !l_Effect)
                    return;

                Aura* l_Base = l_Effect->GetBase();
                AuraEffect* l_AbsorbEffect = nullptr;
                int32 l_AbsorbAmount = static_cast<int32>(l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL) * 2.7f);

                if (!l_Base || !(l_AbsorbEffect = l_Base->GetEffect(EFFECT_1)))
                    return;
                l_Effect->SetAmount(l_AbsorbAmount);
                l_AbsorbEffect->SetAmount(l_AbsorbAmount);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_greater_blessing_of_king_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_greater_blessing_of_king_AuraScript();
        }
};

/// Last Update 7.1.5
/// Hand Of Hindrance - 183218
class spell_pal_hand_of_hindrance : public SpellScriptLoader
{
    public:
        spell_pal_hand_of_hindrance() : SpellScriptLoader("spell_pal_hand_of_hindrance") { }

        class spell_pal_hand_of_hindrance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_hand_of_hindrance_AuraScript);

            enum eSpells
            {
                LawAndOrder = 204934
            };

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;

                if (!l_Caster || !(l_Player = l_Caster->ToPlayer()))
                    return;

                if (!l_Player->HasAura(eSpells::LawAndOrder) || !l_Player->HasSpellCooldown(m_scriptSpellId))
                    return;

                SpellInfo const* l_LawAndOrderInfo = sSpellMgr->GetSpellInfo(eSpells::LawAndOrder);

                if (GetDuration() > 0 && l_LawAndOrderInfo)
                    l_Player->ReduceSpellCooldown(m_scriptSpellId, l_LawAndOrderInfo->Effects[0].BasePoints * TimeConstants::IN_MILLISECONDS);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_hand_of_hindrance_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_hand_of_hindrance_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Blade Of Justice - 184575, Divine Hammer - 198034 and Divine Hammer's damaging spell - 198137
class spell_pal_blade_of_justice : public SpellScriptLoader
{
    public:
        spell_pal_blade_of_justice() : SpellScriptLoader("spell_pal_blade_of_justice") { }

        class spell_pal_blade_of_justice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blade_of_justice_SpellScript);

            enum eSpells
            {
                DivineHammerDmg         = 198137,
                HandOfHindrance         = 183218,
                LawAndOrder             = 204934,
                RighteousVerdict        = 238062,
                RighteousVerdictProc    = 238996
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->m_SpellHelper.GetBool(eSpellHelpers::RighteousVerdict))
                    return;

                AuraEffect* l_RighteousEffect = l_Caster->GetAuraEffect(eSpells::RighteousVerdict, EFFECT_0);
                if (!l_RighteousEffect)
                    return;

                /// Someone tried to fix this generically by adding the spellschoolmask of Divine Hammer to Righteous Verdict, it made it work on EVERY pala spell, so i'd rather fix it there with a Helper.
                uint32 l_PctBonus = l_RighteousEffect->GetAmount();
                uint32 l_Damage = GetHitDamage();
                AddPct(l_Damage, l_PctBonus);
                SetHitDamage(l_Damage);
            }

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || l_Caster == l_Target || !l_Caster->HasAura(eSpells::LawAndOrder))
                    return;

                if (GetSpellInfo()->Id == eSpells::DivineHammerDmg)
                {
                    std::set<uint64>& l_Targets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::DivineHammerTargets];
                    if (l_Targets.find(l_Target->GetGUID()) != l_Targets.end())
                        return;

                    l_Targets.insert(l_Target->GetGUID());
                }

                SpellInfo const* l_LawAndOrderInfo = sSpellMgr->GetSpellInfo(eSpells::LawAndOrder);
                if (!l_LawAndOrderInfo)
                    return;

                int32 l_Duration = l_LawAndOrderInfo->Effects[1].BasePoints * TimeConstants::IN_MILLISECONDS;
                if (Aura *l_Aura = l_Target->GetAura(eSpells::HandOfHindrance, l_Caster->GetGUID()))
                {
                    int32 l_NewDuration = l_Aura->GetDuration() + l_Duration;
                    l_Aura->SetMaxDuration(std::max(l_Aura->GetMaxDuration(), l_NewDuration));
                    l_Aura->SetDuration(l_NewDuration);
                }
                else
                {
                    if (Aura *l_Aura = l_Caster->AddAura(eSpells::HandOfHindrance, l_Target))
                    {
                        l_Aura->SetMaxDuration(l_Duration);
                        l_Aura->SetDuration(l_Duration);
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::RighteousVerdictProc))
                {
                    l_Caster->RemoveAura(eSpells::RighteousVerdictProc);
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::RighteousVerdict) = true;
                }
                else
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::RighteousVerdict) = false;
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::DivineHammerDmg)
                    OnHit += SpellHitFn(spell_pal_blade_of_justice_SpellScript::HandleOnHit);

                AfterHit += SpellHitFn(spell_pal_blade_of_justice_SpellScript::HandleHit);

                if (m_scriptSpellId != eSpells::DivineHammerDmg)
                    AfterCast += SpellCastFn(spell_pal_blade_of_justice_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_blade_of_justice_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Ilterendi, Crown Jewel of Silvermoon - 207587
/// Called for Ilterendi, Crown Jewel of Silvermoon (item) - 137046
class spell_pal_ilterendi_crown_jewel_of_silvermoon : public SpellScriptLoader
{
    public:
        spell_pal_ilterendi_crown_jewel_of_silvermoon() : SpellScriptLoader("spell_pal_ilterendi_crown_jewel_of_silvermoon") { }

        class spell_pal_ilterendi_crown_jewel_of_silvermoon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_ilterendi_crown_jewel_of_silvermoon_AuraScript);

            enum eSpells
            {
                Judgment = 20271
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Judgment)
                    return false;

                return true;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_ilterendi_crown_jewel_of_silvermoon_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_ilterendi_crown_jewel_of_silvermoon_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Whisper of the Nathrezim - 207633
/// Called for Whisper of the Nathrezim (item) - 137020
class spell_pal_whisper_of_the_nathrezim : public SpellScriptLoader
{
    public:
        spell_pal_whisper_of_the_nathrezim() : SpellScriptLoader("spell_pal_whisper_of_the_nathrezim") { }

        class spell_pal_whisper_of_the_nathrezim_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_whisper_of_the_nathrezim_AuraScript);

            enum eSpells
            {
                TemplarsVerdict       = 224266,
                DivineStorm           = 224239,
                WhisperOfTheNathrezim = 207635
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::TemplarsVerdict && l_SpellInfo->Id != eSpells::DivineStorm))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::WhisperOfTheNathrezim, true);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_whisper_of_the_nathrezim_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_whisper_of_the_nathrezim_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_whisper_of_the_nathrezim_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Obsidian Stone Spaulders - 210992
/// Called for Obsidian Stone Spaulders (item) - 210992
class spell_pal_obsidian_stone_spaulders : public SpellScriptLoader
{
    public:
        spell_pal_obsidian_stone_spaulders() : SpellScriptLoader("spell_pal_obsidian_stone_spaulders") { }

        class spell_pal_obsidian_stone_spaulders_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_obsidian_stone_spaulders_AuraScript);

            enum eSpells
            {
                ObsidianStoneSpauldersHeal = 210999
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_SpellInfo || !l_DamageInfo)
                    return;

                int32 l_Amount = l_DamageInfo->GetAmount();
                if (!l_Amount)
                    return;

                std::set<uint64> l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets];
                for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                {
                    Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, *l_Itr);

                    if (!l_Target)
                        l_TargetGUIDs.erase(*l_Itr);
                }

                if (!l_TargetGUIDs.size())
                    return;

                l_Amount = CalculatePct(l_Amount, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                l_Amount /= l_TargetGUIDs.size();

                for (auto l_Itr2 = l_TargetGUIDs.begin(); l_Itr2 != l_TargetGUIDs.end(); ++l_Itr2)
                {
                    Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, *l_Itr2);

                    if (!l_Target)
                        continue;

                    l_Caster->CastCustomSpell(l_Target, eSpells::ObsidianStoneSpauldersHeal, &l_Amount, nullptr, nullptr, true);
                }
            }

            void Register()
            {
                OnProc += AuraProcFn(spell_pal_obsidian_stone_spaulders_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_obsidian_stone_spaulders_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Aegisjalmur, the Armguards of Awe - 225036
/// Called for Aegisjalmur, the Armguards of Awe (item) - 140846
class spell_pal_aegisjalmure_the_armguards_of_awe : public SpellScriptLoader
{
    public:
        spell_pal_aegisjalmure_the_armguards_of_awe() : SpellScriptLoader("spell_pal_aegisjalmure_the_armguards_of_awe") { }

        class spell_pal_aegisjalmure_the_armguards_of_awe_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_aegisjalmure_the_armguards_of_awe_AuraScript);

            enum eSpells
            {
                AegisjalmurDelay  = 225056,
                ShieldOfVengeance = 184662
            };

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* p_AuraEffect, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = 0;
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_SpellInfoShieldOfVengeance = sSpellMgr->GetSpellInfo(eSpells::ShieldOfVengeance);
                if (!l_Target || !l_SpellInfo || !l_SpellInfoShieldOfVengeance)
                    return;

                /// Cannot occur more often than once per 3 min.
                if (l_Target->HasAura(eSpells::AegisjalmurDelay))
                    return;

                /// Any attack which would kill you instead triggers a Shield of Vengeance at 200% of the normal amount then reduces you to 1 health.
                if (p_DmgInfo.GetAmount() >= l_Target->GetHealth())
                {
                    p_AbsorbAmount = p_DmgInfo.GetAmount();

                    int32 l_AmountShield = CalculatePct(l_SpellInfoShieldOfVengeance->Effects[EFFECT_1].BasePoints, l_SpellInfo->Effects[EFFECT_2].BasePoints);
                    l_Target->CastSpell(l_Target, eSpells::AegisjalmurDelay, true);
                    l_Target->CastCustomSpell(l_Target, eSpells::ShieldOfVengeance, nullptr, &l_AmountShield, nullptr, true);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_aegisjalmure_the_armguards_of_awe_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_aegisjalmure_the_armguards_of_awe_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_aegisjalmure_the_armguards_of_awe_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Light of Martyr (heal) - 183998
/// Called for Maraad's Dying Breath (item) - 144273
class spell_pal_maraads_dying_breath : public SpellScriptLoader
{
    public:
        spell_pal_maraads_dying_breath() : SpellScriptLoader("spell_pal_maraads_dying_breath") { }

        class spell_pal_maraads_dying_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_maraads_dying_breath_SpellScript);

            enum eSpells
            {
                MaraadsDyingBreathAura  = 234848,
                MaraadsDyingBreathBuff  = 234862,
                BeaconOfLight           = 53563,
                BeaconOfFaith           = 156910,
                BeaconOfVirtue          = 200025
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                int32 l_HealAmount = GetHitHeal();
                SpellInfo const* l_SpellInfoMaraadsDyingBreath = sSpellMgr->GetSpellInfo(eSpells::MaraadsDyingBreathAura);
                if (!l_Caster || !l_Target || !l_HealAmount || !l_SpellInfoMaraadsDyingBreath)
                    return;

                if (!l_Caster->HasAura(eSpells::MaraadsDyingBreathBuff))
                    return;

                std::set<uint64> l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets];
                for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                {
                    Unit* l_TargetHeal = ObjectAccessor::GetUnit(*l_Caster, *l_Itr);

                    if (!l_TargetHeal || l_TargetHeal == l_Target)                                                  ///< Check if Target of Light of the Martyr isnt beacon
                        continue;

                    uint32 l_HealingFactor = 0;                                                                     ///< Healing factor of said Beacon

                    Aura* l_Aura = l_TargetHeal->GetAura(eSpells::BeaconOfLight, l_Caster->GetGUID());
                    if (!l_Aura)
                        l_Aura = l_TargetHeal->GetAura(eSpells::BeaconOfFaith, l_Caster->GetGUID());

                    if (l_Aura)                                                                                     ///< Case : Beacon of Light or Beacon of Faith
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BeaconOfLight))
                            l_HealingFactor = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                        if (l_Caster->ToPlayer() && l_Caster->ToPlayer()->HasSpell(eSpells::BeaconOfFaith))         ///< Case: caster has talent Beacon of Faith, lowering the value of the heal (obvious if target has aura Beacon of faith :D)
                        {
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BeaconOfFaith))
                                AddPct(l_HealingFactor, -l_SpellInfo->Effects[EFFECT_3].BasePoints);
                        }
                    }

                    if ((l_Aura = l_TargetHeal->GetAura(eSpells::BeaconOfVirtue, l_Caster->GetGUID())))               ///< Case : Beacon of Virtue
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BeaconOfVirtue))
                            l_HealingFactor = l_SpellInfo->Effects[EFFECT_2].BasePoints;
                    }

                    l_Caster->HealBySpell(l_TargetHeal, l_SpellInfoMaraadsDyingBreath, CalculatePct(l_HealAmount, l_HealingFactor));
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_maraads_dying_breath_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_maraads_dying_breath_SpellScript();
        }
};


/// Last Update 7.1.5 Build 23420
/// Called by Lawbringer - 216527
class spell_pal_lawbringer : public SpellScriptLoader
{
    public:
        spell_pal_lawbringer() : SpellScriptLoader("spell_pal_lawbringer") { }

        class spell_pal_lawbringer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_lawbringer_SpellScript);

            enum eSpells
            {
                JudgmentDebuff = 197277
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Caster || !l_SpellInfo || !l_Target)
                    return;

                if (Creature* l_Creature = l_Target->ToCreature())
                {
                    if (CreatureTemplate const* l_CreatureTemplate = l_Creature->GetCreatureTemplate())
                    {
                        if (l_CreatureTemplate->rank >= CreatureEliteType::CREATURE_ELITE_NORMAL)
                        {
                            SetHitDamage(int32((float)l_Caster->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints) / 100.0f));
                        }
                        else
                        {
                            SetHitDamage(int32((float)l_Target->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints) / 100.0f));
                        }
                    }
                }
                else
                    SetHitDamage(int32((float)l_Target->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints) / 100.0f));
            }

            void HandleSelectTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const *l_SpellInfo = GetSpellInfo();

                if (!l_Caster || !l_SpellInfo || !l_SpellInfo->RangeEntry)
                    return;

                p_Targets.remove_if([l_Caster, l_SpellInfo](WorldObject* l_Target) -> bool
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        return true;

                    if (l_UnitTarget->GetDistance(l_Caster) > l_SpellInfo->RangeEntry->maxRangeHostile)
                        return true;

                    if (l_UnitTarget->HasAura(eSpells::JudgmentDebuff, l_Caster->GetGUID()))
                        return false;

                    return true;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_lawbringer_SpellScript::HandleSelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_pal_lawbringer_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_lawbringer_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Judgement - 197277
/// Called for Lawbringer - 216527
class spell_pal_lawbringer_judgement : public SpellScriptLoader
{
    public:
        spell_pal_lawbringer_judgement() : SpellScriptLoader("spell_pal_lawbringer_judgement") { }

        class spell_pal_lawbringer_judgement_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_lawbringer_judgement_AuraScript);

            void HandleAfterEffectRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::Lawbringer].erase(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_lawbringer_judgement_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_lawbringer_judgement_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Hand Of Reckoning - 62124
/// Called for Inquisition - 207028
class spell_pal_inquisition : public SpellScriptLoader
{
    public:
        spell_pal_inquisition() : SpellScriptLoader("spell_pal_inquisition") { }

        class spell_pal_inquisition_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_inquisition_SpellScript);

            enum eSpells
            {
                Intimidated = 206891,
                Inquisition = 207028
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasSpell(eSpells::Inquisition))
                    return;

                l_Target->RemoveAurasDueToSpell(eSpells::Intimidated);
                l_Caster->CastSpell(l_Target, eSpells::Intimidated, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_inquisition_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_inquisition_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Uther's Guard - 207558
/// Called for Uther's Guard (item) - 137105
class spell_pal_uthers_guard : public SpellScriptLoader
{
    public:
        spell_pal_uthers_guard() : SpellScriptLoader("spell_pal_uthers_guard") { }

        class spell_pal_uthers_guard_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_uthers_guard_AuraScript);

            enum eSpells
            {
                BlessingOfFreedom      = 1044,
                BlessingOfSacrifice    = 6940,
                BlessingOfSpellwarding = 204018,
                BlessingOfProtection   = 1022,
                MaxSpell               = 4,
                ThraynsSacrifice       = 207559
            };

            std::array<uint32, eSpells::MaxSpell> const m_SpellArray =
            {
                {
                    eSpells::BlessingOfFreedom,
                    eSpells::BlessingOfSacrifice,
                    eSpells::BlessingOfSpellwarding,
                    eSpells::BlessingOfProtection
                }
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                if (!l_Caster || !l_Target)
                    return false;

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_DamageInfo || !l_SpellInfo)
                    return false;

                SpellInfo const* l_SpellInfoProc = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfoProc)
                    return false;

                if (l_SpellInfoProc->Id == eSpells::BlessingOfSacrifice)
                {
                    if (l_Caster == l_Target)
                        return false;
                }

                for (uint32 l_SpellId : m_SpellArray)
                {
                    if (l_SpellId == l_SpellInfoProc->Id)
                        return true;
                }

                return false;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ThraynsSacrifice, true);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_uthers_guard_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_uthers_guard_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_uthers_guard_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Justice Gaze - 211557
/// Called for Justice Gaze (item) - 211557
class spell_pal_justice_gaze : public SpellScriptLoader
{
    public:
        spell_pal_justice_gaze() : SpellScriptLoader("spell_pal_justice_gaze") { }

        class spell_pal_justice_gaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_justice_gaze_AuraScript);

            enum eSpells
            {
                HammerOfJustice        = 853,
                HammerOfJusticeDamages = 211561
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::HammerOfJustice)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_SpellInfoHammerOfJustice = sSpellMgr->GetSpellInfo(eSpells::HammerOfJustice);
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_SpellInfoHammerOfJustice)
                    return;

                if (l_Target->GetHealthPct() <= l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Caster->EnergizeBySpell(l_Caster, l_SpellInfo->Id, l_SpellInfo->Effects[EFFECT_2].BasePoints, Powers::POWER_HOLY_POWER);

                l_Player->ReduceSpellCooldown(eSpells::HammerOfJustice, CalculatePct(l_SpellInfoHammerOfJustice->GetRecoveryTime(), l_SpellInfo->Effects[EFFECT_1].BasePoints));

                l_Caster->CastSpell(l_Target, eSpells::HammerOfJusticeDamages, true);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_justice_gaze_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_justice_gaze_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_justice_gaze_AuraScript();
        }
};


/// Last Update 7.1.5
/// Darkest before the Dawn - 210378
class spell_pal_darkest_before_the_dawn : public SpellScriptLoader
{
    public:
        spell_pal_darkest_before_the_dawn() : SpellScriptLoader("spell_pal_darkest_before_the_dawn") { }

        class spell_pal_darkest_before_the_dawn_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_darkest_before_the_dawn_AuraScript);

            enum eSpells
            {
                DarkestBeforeTheDawnProc = 210391,
                LightOfDawn = 85222
            };

            void HandlePeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;
                AuraEffect* l_Periodic = GetEffect(EFFECT_0);
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Periodic || !l_SpellInfo)
                    return;

                if (!l_Caster || !(l_Player = l_Caster->ToPlayer()))
                    return;

                if (l_Player->HasSpellCooldown(eSpells::LightOfDawn))
                {
                    l_Periodic->SetAmplitude(l_Player->GetSpellCooldownDelay(eSpells::LightOfDawn));
                    return;
                }
                else
                    l_Periodic->SetAmplitude(l_SpellInfo->Effects[0].Amplitude);

                l_Player->CastSpell(l_Player, eSpells::DarkestBeforeTheDawnProc, true);
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::DarkestBeforeTheDawnProc);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_darkest_before_the_dawn_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_darkest_before_the_dawn_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_darkest_before_the_dawn_AuraScript();
        }
};

/// Last Update 7.1.5
/// Light of Dawn Heal - 225311
class spell_pal_light_of_dawn_heal : public SpellScriptLoader
{
    public:
        spell_pal_light_of_dawn_heal() : SpellScriptLoader("spell_pal_light_of_dawn_heal") { }

        class spell_pal_light_of_dawn_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_light_of_dawn_heal_SpellScript);

            enum eSpells
            {
                DarkestBeforeTheDawnProc = 210391
            };

            void HandleHeal()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect *l_DarkestBeforeTheDawn = l_Caster->GetAuraEffect(eSpells::DarkestBeforeTheDawnProc, EFFECT_0))
                {
                    int32 l_Heal = GetHitHeal();

                    l_Heal = AddPct(l_Heal, l_DarkestBeforeTheDawn->GetAmount());
                    SetHitHeal(l_Heal);
                    l_Caster->RemoveAurasDueToSpell(eSpells::DarkestBeforeTheDawnProc);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_light_of_dawn_heal_SpellScript::HandleHeal);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_light_of_dawn_heal_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Retribution - 183435
class spell_pal_retribution : public SpellScriptLoader
{
    public:
        spell_pal_retribution() : SpellScriptLoader("spell_pal_retribution") { }

        class spell_pal_retribution_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_retribution_AuraScript);

            enum eSpells
            {
                RetributionBuff = 183436
            };

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RetributionBuff, true);
            }

            void Register()
            {
                OnProc += AuraProcFn(spell_pal_retribution_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_retribution_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blessing of Freedom - 1044
/// Called by Blessing of Protection - 1022
/// Called by Blessing of Sacrifice - 6940
/// Called by Blessing of Spellwarding - 204018
/// Called by Blessing of Sanctuary - 210256
/// Called for Holy Ritual - 199422
class spell_pal_holy_ritual : public SpellScriptLoader
{
    public:
        spell_pal_holy_ritual() : SpellScriptLoader("spell_pal_holy_ritual") { }

        class spell_pal_holy_ritual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_holy_ritual_AuraScript);

            enum eSpells
            {
                BlessingOfProtection    = 1022,
                BlessingOfFreedom       = 1044,
                BlessingOfSacrifice     = 6940,
                BlessingOfSpellwarding  = 204018,
                BlessingOfSanctuary     = 210256,
                HolyRitualAura          = 199422,
                HolyRitualHeal          = 199423
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::HolyRitualAura))
                    l_Caster->CastSpell(l_Target, eSpells::HolyRitualHeal, true);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::HolyRitualAura))
                    l_Caster->CastSpell(l_Target, eSpells::HolyRitualHeal, true);
            }

            void Register()
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::BlessingOfProtection:
                    case eSpells::BlessingOfSpellwarding:
                        AfterEffectApply += AuraEffectApplyFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_SCHOOL_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_SCHOOL_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
                        break;
                    case eSpells::BlessingOfFreedom:
                        AfterEffectApply += AuraEffectApplyFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
                        break;
                    case eSpells::BlessingOfSanctuary:
                        AfterEffectApply += AuraEffectApplyFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectApply, EFFECT_1, SPELL_AURA_MECHANIC_DURATION_MOD, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectRemove, EFFECT_1, SPELL_AURA_MECHANIC_DURATION_MOD, AURA_EFFECT_HANDLE_REAL);
                        break;
                    default:
                        AfterEffectApply += AuraEffectApplyFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_SPLIT_DAMAGE_PCT, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_holy_ritual_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_SPLIT_DAMAGE_PCT, AURA_EFFECT_HANDLE_REAL);
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_holy_ritual_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blinding Light - 105421
class spell_pal_blinding_light_confuse : public SpellScriptLoader
{
public:
    spell_pal_blinding_light_confuse() : SpellScriptLoader("spell_pal_blinding_light_confuse") { }

    class spell_pal_blinding_light_confuse_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_blinding_light_confuse_SpellScript);

        void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    l_Target->RemoveAurasWithMechanic((1LL << MECHANIC_BLEED) | (1LL << MECHANIC_INFECTED));
                    l_Target->RemoveAurasByType(AuraType::SPELL_AURA_PERIODIC_DAMAGE);
                    l_Target->RemoveAurasByType(AuraType::SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_blinding_light_confuse_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    class spell_pal_blinding_light_confuse_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_blinding_light_confuse_AuraScript);

        enum eSpells
        {
            BlindingLightConfuse = 105421
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            if (l_DamageInfo->GetSchoolMask() & SpellSchoolMask::SPELL_SCHOOL_MASK_HOLY)
                return false;

            return true;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_blinding_light_confuse_AuraScript::HandleCheckProc);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_blinding_light_confuse_SpellScript();
    }

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_blinding_light_confuse_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Guardian of the Forgotten Queen - 228049
class spell_pal_guardian_of_the_forgotten_queen : public SpellScriptLoader
{
    public:
        spell_pal_guardian_of_the_forgotten_queen() : SpellScriptLoader("spell_pal_guardian_of_the_forgotten_queen") { }

        class spell_pal_guardian_of_the_forgotten_queen_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_guardian_of_the_forgotten_queen_SpellScript);

            enum eCreatures
            {
                GuardianOfTheForgottenQueen = 114565
            };

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                Unit* l_ExplTarget = GetExplTargetUnit();
                if (!l_Target || !l_ExplTarget || l_Target != l_Caster)
                    return;

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon->GetEntry() == eCreatures::GuardianOfTheForgottenQueen)
                    {
                        if (UnitAI* l_SummonAI = l_Summon->GetAI())
                            l_SummonAI->SetGUID(l_ExplTarget->GetGUID());
                        break;
                    }
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pal_guardian_of_the_forgotten_queen_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_guardian_of_the_forgotten_queen_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Flash of Light - 19750, Holy Light - 82326
class spell_pal_fervent_martyr : public SpellScriptLoader
{
    public:
        spell_pal_fervent_martyr() : SpellScriptLoader("spell_pal_fervent_martyr") { }

        class spell_pal_fervent_martyr_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_fervent_martyr_SpellScript);

            enum eSpells
            {
                FerventMartyr       = 196923,
                FerventMartyrProc   = 223316
            };

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::FerventMartyr))
                    return;

                bool l_HasFerventMartyr = l_Caster->HasAura(eSpells::FerventMartyrProc);

                l_Caster->CastSpell(l_Caster, eSpells::FerventMartyrProc, true);
                if (!l_HasFerventMartyr)
                    l_Caster->SetAuraStack(eSpells::FerventMartyrProc, l_Caster, 1);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_fervent_martyr_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_fervent_martyr_SpellScript();
        }
};

/// Last Update: 7.3.5 build 26365
/// Called by Execution Sentence - 213757
class spell_pal_execution_sentence : public SpellScriptLoader
{
    public:
        spell_pal_execution_sentence() : SpellScriptLoader("spell_pal_execution_sentence") { }

        class spell_pal_execution_sentence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_execution_sentence_AuraScript);

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

                p_Amount *= 0.80f;   ///< Execution Sentence Damage in PVP is reduced by 20%
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_execution_sentence_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_execution_sentence_AuraScript();
        }

        class spell_pal_execution_sentence_SpellScript : public SpellScript
        {
            enum eSpells
            {
                DivineStorm = 53385,
                WorldOfGlory = 210191,
                HammerOfJustice = 853
            };

            PrepareSpellScript(spell_pal_execution_sentence_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() != SPEC_PALADIN_RETRIBUTION)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                if (l_SpellInfo->SpellPowers.empty())
                    return;

                uint32 l_HolyPowerCost = 0;

                for (auto spellPower : l_SpellInfo->SpellPowers)
                    if (spellPower->PowerType == POWER_HOLY_POWER)
                        l_HolyPowerCost += spellPower->Cost;

                if (l_HolyPowerCost > 0)
                    l_Player->ReduceSpellCooldown(eSpells::HammerOfJustice, 2500 * l_HolyPowerCost);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_execution_sentence_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_execution_sentence_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Avenging Crusader - 216331
class spell_pal_avenging_crusader : public SpellScriptLoader
{
    public:
        spell_pal_avenging_crusader() : SpellScriptLoader("spell_pal_avenging_crusader") { }

        class spell_pal_avenging_crusader_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_avenging_crusader_SpellScript);

            enum eSpells
            {
                AvengingWrathHoly = 31842,
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::AvengingWrathHoly))
                    l_Caster->RemoveAura(eSpells::AvengingWrathHoly);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_pal_avenging_crusader_SpellScript::HandleBeforeCast);
            }
        };

        class spell_pal_avenging_crusader_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_avenging_crusader_AuraScript);

            enum eSpells
            {
                CrusaderStrike              = 35395,
                Judgment                    = 20271,
                AvengingCrusader            = 216331,
                AvengingCrusaderHealAllies  = 216371,
                AvengingCrusaderHealCaster  = 216372
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::CrusaderStrike && l_SpellInfo->Id != eSpells::Judgment))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AvengingCrusader);
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return;

                int32 l_Amount = l_DamageInfo->GetAmount();
                int32 l_Absorb = l_DamageInfo->GetAbsorb();
                if (!l_Amount && !l_Absorb)
                    return;

                l_Amount = (l_Amount + l_Absorb) * float(l_SpellInfo->Effects[EFFECT_0].BasePoints / 100.0f);

                l_Caster->CastCustomSpell(l_Caster, eSpells::AvengingCrusaderHealAllies, &l_Amount, nullptr, nullptr, true);
                l_Caster->CastCustomSpell(l_Caster, eSpells::AvengingCrusaderHealCaster, &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_avenging_crusader_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_avenging_crusader_AuraScript::HandleOnProc);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_avenging_crusader_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_avenging_crusader_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Justicar's vengeance - 215661
class spell_pal_justicar_vengeance : public SpellScriptLoader
{
    public:
        spell_pal_justicar_vengeance() : SpellScriptLoader("spell_pal_justicar_vengeance") { }

        class spell_pal_justicar_vengeance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_justicar_vengeance_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_Target->isInStun() || !l_SpellInfo)
                    return;

                int32 l_Damage = GetHitDamageForLeechEffect();
                float l_PctModifier = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                if (l_Caster->CanApplyPvPSpellModifiers())
                    l_PctModifier *= 0.25; ///< bonus damage on stunned target is reduced by 75% in pvp

                AddPct(l_Damage, l_PctModifier);
                SetHitDamageForLeechEffect(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_justicar_vengeance_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_HEALTH_LEECH);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_justicar_vengeance_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Unbreakable Will - 182234
class spell_pal_unbreakable_will : public SpellScriptLoader
{
    public:
        spell_pal_unbreakable_will() : SpellScriptLoader("spell_pal_unbreakable_will") { }

        class spell_pal_unbreakable_will_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_unbreakable_will_AuraScript);

            enum eSpells
            {
                UnbreakableWillCrowedControlled = 182497
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                Unit* l_Actor = p_EventInfo.GetActor();
                if (!l_Actor)
                    return false;

                return CheckCrowdControlSpell(l_SpellInfo);
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Actor = p_EventInfo.GetActor();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster->isInCombat() || !l_Actor || !l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfoProc = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfoProc)
                    return;

                Aura* l_AuraUnbreakableWill = l_Caster->AddAura(eSpells::UnbreakableWillCrowedControlled, l_Caster);
                if (!l_AuraUnbreakableWill)
                    return;

                l_AuraUnbreakableWill->SetScriptGuid(l_SpellInfoProc->Id, l_Actor->GetGUID());
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_unbreakable_will_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_unbreakable_will_AuraScript::HandleOnProc);
            }

        private:

            bool CheckCrowdControlSpell(SpellInfo const* p_SpellInfo)
            {
                switch (p_SpellInfo->Id)
                {
                case 33786:
                case 108194:
                case 221562:
                    return true;
                default:
                    break;
                }
                for (uint32 l_EffIndex = 0; l_EffIndex < p_SpellInfo->EffectCount; ++l_EffIndex)
                {
                    switch (p_SpellInfo->Effects[l_EffIndex].ApplyAuraName)
                    {
                        case SPELL_AURA_MOD_STUN:
                        case SPELL_AURA_MOD_CONFUSE:
                        case SPELL_AURA_MOD_FEAR:
                        case SPELL_AURA_MOD_FEAR_2:
                        case SPELL_AURA_TRANSFORM:
                        case SPELL_AURA_STRANGULATE:
                        {
                            if (!p_SpellInfo->IsPositive())
                                return true;
                            break;
                        }
                        default:
                            break;
                    }
                }

                return false;
            }

        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_unbreakable_will_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Unbreakable Will (crowed controlled) - 182497
class spell_pal_unbreakable_will_crowed_controlled : public SpellScriptLoader
{
    public:
        spell_pal_unbreakable_will_crowed_controlled() : SpellScriptLoader("spell_pal_unbreakable_will_crowed_controlled") { }

        class spell_pal_unbreakable_will_crowed_controlled_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_unbreakable_will_crowed_controlled_AuraScript);

            enum eSpells
            {
                UnbreakableWillBreakFree    = 182531,
                UnbreakableWillTimer        = 182496,
                Cyclone                     = 33786
            };

            uint32 m_SpellId = 0;
            uint64 m_CasterGUID = 0;
            Guid128 m_CastGUID = Guid128(0, 0);

            void HandleAfterEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                Aura* l_AuraUnbreakableWill = p_AurEff->GetBase();
                if (!l_Owner || !l_AuraUnbreakableWill)
                    return;

                Aura* l_AuraCrowedControl = l_Owner->GetAura(m_SpellId, m_CasterGUID);
                if (!l_AuraCrowedControl)
                    return;

                if (l_Owner->HasAura(eSpells::UnbreakableWillTimer) || l_AuraUnbreakableWill->GetDuration() != 0)
                    return;

                if (m_CastGUID != l_AuraCrowedControl->GetCastGUID())
                    return;

                l_Owner->CastSpell(l_Owner, eSpells::UnbreakableWillBreakFree, true);
                if (l_Owner->HasAura(eSpells::Cyclone))
                    l_Owner->RemoveAurasDueToSpell(eSpells::Cyclone);
                l_Owner->CastSpell(l_Owner, eSpells::UnbreakableWillTimer, true);
            }

            void SetGuid(uint32 p_Type, uint64 p_Data) override
            {
                m_SpellId = p_Type;
                m_CasterGUID = p_Data;
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                Aura* l_Aura = l_Owner->GetAura(m_SpellId, m_CasterGUID);
                if (!l_Aura)
                    return;

                m_CastGUID = l_Aura->GetCastGUID();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_unbreakable_will_crowed_controlled_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_unbreakable_will_crowed_controlled_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blessing of Sacrifice - 199448
/// Called for Ultimate Sacrifice - 199452
class spell_pal_ultimate_sacrifice : public SpellScriptLoader
{
    public:
        spell_pal_ultimate_sacrifice() : SpellScriptLoader("spell_pal_ultimate_sacrifice") { }

        class spell_pal_ultimate_sacrifice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_ultimate_sacrifice_AuraScript);

            enum eSpells
            {
                UltimateSacrificePeriodic = 199450
            };

            void HandleOnEffectAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void HandleOnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::UltimateSacrificePeriodic);
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_Caster->IsPlayer() || !l_DamageInfo || !l_SpellInfo)
                    return;

                int32 l_Amount = l_DamageInfo->GetAbsorb();
                SpellDurationEntry const* l_DurationEntry = l_SpellInfo->DurationEntry;
                if (!l_Amount || !l_DurationEntry)
                    return;

                l_Amount /= l_DurationEntry->Duration / l_SpellInfo->Effects[EFFECT_0].Amplitude;
                if (Player* l_ModOwner = (l_Caster ? l_Caster->GetSpellModOwner() : NULL))
                    l_ModOwner->ApplySpellMod(GetId(), SPELLMOD_VALUE_MULTIPLIER, l_Amount);

                Aura* l_SacrificeDOT = l_Caster->GetAura(eSpells::UltimateSacrificePeriodic);
                if (!l_SacrificeDOT)
                    l_Caster->CastCustomSpell(l_Caster, eSpells::UltimateSacrificePeriodic, &l_Amount, nullptr, nullptr, true);
                else
                {
                    AuraEffect* l_SacrificeEffect = l_SacrificeDOT->GetEffect(SpellEffIndex::EFFECT_0);
                    if (!l_SacrificeDOT)
                        return;

                    uint32 l_OldAmount = l_SacrificeEffect->GetAmount();
                    uint32 l_RemainingTicks = (l_SacrificeDOT->GetDuration() / l_SacrificeEffect->GetAmplitude()) + 1;
                    uint32 l_MaxTicks = l_SacrificeDOT->GetMaxDuration() / l_SacrificeEffect->GetAmplitude();

                    /// In case the sacrificed target gets hit several times in 1 update (lots of people hitting), it's possible that l_RemainingTicks is superior to l_Maxticks (4000 / 1000) + 1 = 5, when maxtick = 4
                    if (l_RemainingTicks > l_MaxTicks)
                        l_RemainingTicks = l_MaxTicks;

                    l_Amount += (l_OldAmount * l_RemainingTicks / l_MaxTicks);

                    l_SacrificeEffect->ChangeAmount(l_Amount);
                    l_SacrificeDOT->RefreshDuration(false);
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_ultimate_sacrifice_AuraScript::HandleOnEffectAbsorb, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectProc += AuraEffectProcFn(spell_pal_ultimate_sacrifice_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_ultimate_sacrifice_AuraScript();
        }
};

class PlayerScript_boundless_conviction : public PlayerScript
{
    public:
        PlayerScript_boundless_conviction() : PlayerScript("PlayerScript_boundless_conviction") { }

        enum eSpells
        {
            BoundlessConviction = 115675
        };

        void OnModifySpec(Player* p_Player, int32 p_OldSpec, int32 p_NewSpec) override
        {
            if (p_NewSpec == SPEC_PALADIN_RETRIBUTION)
                p_Player->learnSpell(eSpells::BoundlessConviction, false, false, false, 0, true);
            else if (p_Player->getClass() == CLASS_PALADIN)
                p_Player->removeSpell(eSpells::BoundlessConviction, false, false);
        }
};

/// Last Update 7.35 Build 26365
/// Called for Forbearant Faithful - 209376
/// Called by Forbearance - 25771
class spell_pal_forbearant_faithful : public SpellScriptLoader
{
    public:
        spell_pal_forbearant_faithful() : SpellScriptLoader("spell_pal_forbearant_faithful") { }

        class spell_pal_forbearant_faithful_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_forbearant_faithful_AuraScript);

            enum eSpell
            {
                ForbearantFaithful = 209376
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpell::ForbearantFaithful, SpellEffIndex::EFFECT_0))
                    {
                        if (AuraEffect* l_BP = l_AurEff->GetBase()->GetEffect(SpellEffIndex::EFFECT_1))
                            l_AurEff->ChangeAmount(l_AurEff->GetAmount() + l_BP->GetAmount());
                    }
                }
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpell::ForbearantFaithful, SpellEffIndex::EFFECT_0))
                    {
                        if (AuraEffect* l_BP = l_AurEff->GetBase()->GetEffect(SpellEffIndex::EFFECT_1))
                            l_AurEff->ChangeAmount((l_AurEff->GetAmount() > l_BP->GetAmount()) ? l_AurEff->GetAmount() - l_BP->GetAmount() : 0);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_forbearant_faithful_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MECHANIC_IMMUNITY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_forbearant_faithful_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MECHANIC_IMMUNITY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_forbearant_faithful_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Light of the Titans - 209539
class spell_pal_light_of_the_titans : public SpellScriptLoader
{
    public:
        spell_pal_light_of_the_titans() : SpellScriptLoader("spell_pal_light_of_the_titans") { }

        class spell_pal_light_of_the_titans_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_light_of_the_titans_AuraScript);

            enum eSpells
            {
                LightOfTheProtector = 184092,
                HandOfTheProtector  = 213652
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (!p_EventInfo.GetDamageInfo())
                    return false;

                SpellInfo const* l_ProcSpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_ProcSpellInfo)
                    return false;

                switch (l_ProcSpellInfo->Id)
                {
                    case eSpells::LightOfTheProtector:
                        return true;
                    case eSpells::HandOfTheProtector:
                        // The spell has 2 effects, but it should proc by the first only.
                        if (auto l_Spell = p_EventInfo.GetDamageInfo()->GetSpell())
                            if (l_Spell->m_targets.GetUnitTarget() != p_EventInfo.GetActionTarget())
                                return false;
                        break;
                    default:
                        return false;
                }

                return true;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_light_of_the_titans_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_light_of_the_titans_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Beacon of Light (Light's Beacon) - 53651
class PlayerScript_beacon_of_light : public PlayerScript
{
    public:
        PlayerScript_beacon_of_light() : PlayerScript("PlayerScript_beacon_of_light") { }

        enum eSpells
        {
            BeaconOfLight = 53563
        };

        void OnModifySpec(Player* p_Player, int32 p_OldSpec, int32 p_NewSpec) override
        {
            if (p_OldSpec == SPEC_PALADIN_HOLY)
            {
                auto l_BeaconTargets = p_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::BeaconsTargets];
                for (uint64 l_TargetGUID : l_BeaconTargets)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*p_Player, l_TargetGUID);
                    if (!l_Target)
                        continue;

                    l_Target->RemoveAura(eSpells::BeaconOfLight, p_Player->GetGUID());
                }
            }
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Second Sunrise - 200482
class spell_pal_second_sunrise : public SpellScriptLoader
{
    public:
        spell_pal_second_sunrise() : SpellScriptLoader("spell_pal_second_sunrise") { }

        class spell_pal_second_sunrise_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_second_sunrise_AuraScript);

            enum eSpells
            {
                LightOfDawn                 = 85222,
                DivinePurposeLightOfDawn    = 216413
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                Aura* l_Aura = GetAura();

                if (!l_DamageInfo || !l_Aura || !l_Player)
                    return false;

                if (const Spell* l_Spell = l_DamageInfo->GetSpell())
                    if (!l_Spell->m_FromClient)
                        return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::LightOfDawn || !l_AuraEffect || l_Target != l_Caster) ///< Check caster == target to avoid multiple procs, since light of dawn passes the check proc for EACH TARGET.
                    return false;

                if (!roll_chance_i(l_AuraEffect->GetAmount()))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_cooldown = 0;
                if (l_Player->HasSpellCooldown(eSpells::LightOfDawn))
                    l_cooldown = l_Player->GetSpellCooldownDelay(eSpells::LightOfDawn);

                l_Player->RemoveSpellCooldown(eSpells::LightOfDawn);
                l_Player->CastSpell(l_Player, eSpells::LightOfDawn, true); ///< Won't make it proc again since it's triggered.
                if (l_cooldown > 0)
                    l_Player->AddSpellCooldown(eSpells::LightOfDawn, 0, l_cooldown, true);

                l_Player->m_Functions.AddFunction([=]() -> void
                {
                    if (l_Caster->HasAura(eSpells::DivinePurposeLightOfDawn))
                    {
                        l_Player->RemoveSpellCooldown(eSpells::LightOfDawn, true);
                        l_Player->SendClearCooldown(eSpells::LightOfDawn, l_Player, true);
                    }
                }, l_Player->m_Functions.CalculateTime(100));
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_second_sunrise_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_second_sunrise_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_second_sunrise_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Painful Truths - 209341
class spell_pal_painful_truths : public SpellScriptLoader
{
    public:
        spell_pal_painful_truths() : SpellScriptLoader("spell_pal_painful_truths") { }

        class spell_pal_painful_truths_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_painful_truths_AuraScript);

            enum eSpells
            {
                ArdentDefender = 31850
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ArdentDefender)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_painful_truths_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_painful_truths_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Luminescence - 199428
class spell_pal_luminescence : public SpellScriptLoader
{
    public:
        spell_pal_luminescence() : SpellScriptLoader("spell_pal_luminescence") { }

        class spell_pal_luminescence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_luminescence_AuraScript);

            enum eSpells
            {
                LuminescenceHeal = 199435
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return;

                int32 l_HealAmount = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                if (l_HealAmount <= 0)
                    return;

                l_Caster->CastCustomSpell(l_Caster, eSpells::LuminescenceHeal, &l_HealAmount, nullptr, nullptr, true);
            }

            void Register()
            {
                OnProc += AuraProcFn(spell_pal_luminescence_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_luminescence_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Vengeance Aura - 210323
class spell_pal_vengeance_aura : public SpellScriptLoader
{
    public:
        spell_pal_vengeance_aura() : SpellScriptLoader("spell_pal_vengeance_aura") { }

        class spell_pal_vengeance_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_vengeance_aura_AuraScript);

            enum eSpells
            {
                GreaterBlessingOfKings  = 203538,
                GreaterBlessingOfWisdom = 203539,
                VengeanceAuraBuff       = 210324
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_Target || !l_DamageInfo)
                    return false;

                if (!l_Target->HasAura(eSpells::GreaterBlessingOfKings) && !l_Target->HasAura(eSpells::GreaterBlessingOfWisdom))
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || !l_SpellInfo->_IsLossOfControl())
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::VengeanceAuraBuff, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_vengeance_aura_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_pal_vengeance_aura_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_vengeance_aura_AuraScript();

        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Divine Steed - 220509, 221883, 221885, 221886, 221887
class spell_pal_steed_of_glory : public SpellScriptLoader
{
    public:
        spell_pal_steed_of_glory() : SpellScriptLoader("spell_pal_steed_of_glory") { }

        class spell_pal_steed_of_glory_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_steed_of_glory_AuraScript);

            enum eSpells
            {
                SteedOfGloryAura = 199542,
                SteedOfGloryAreaTrigger = 199545
            };

            void HandleAfterApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Owner = GetUnitOwner();

                if (!l_Owner || !l_Owner->HasAura(eSpells::SteedOfGloryAura))
                    return;

                l_Owner->CastSpell(l_Owner, eSpells::SteedOfGloryAreaTrigger, true);

                if (Aura* l_Aura = l_Owner->GetAura(eSpells::SteedOfGloryAreaTrigger))
                    l_Aura->SetDuration(GetDuration());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_steed_of_glory_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_steed_of_glory_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blessing of Spellwarding - 204018
class spell_pal_blessing_of_spellwarding : public SpellScriptLoader
{
    public:
        spell_pal_blessing_of_spellwarding() : SpellScriptLoader("spell_pal_blessing_of_spellwarding") { }

        class spell_pal_blessing_of_spellwarding_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blessing_of_spellwarding_SpellScript);

            enum eSpells
            {
                Forbearance = 25771
            };

            SpellCastResult CheckForbearance()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                if (l_Target->HasAura(eSpells::Forbearance))
                        return SPELL_FAILED_TARGET_AURASTATE;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Forbearance, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_blessing_of_spellwarding_SpellScript::CheckForbearance);
                OnHit += SpellHitFn(spell_pal_blessing_of_spellwarding_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_blessing_of_spellwarding_SpellScript();
        }
};

/// Last Update : 7.3.5 Build 26365
/// Called by Fist of Justice - 198054
class spell_pal_fist_of_justice : public SpellScriptLoader
{
    public:
        spell_pal_fist_of_justice() : SpellScriptLoader("spell_pal_fist_of_justice") { }

        class spell_pal_fist_of_justice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_fist_of_justice_AuraScript);

            enum eSpells
            {
                Judgment        = 20271,
                HammerOfJustice = 853
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Judgment)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Player || !l_SpellInfo)
                    return;

                int32 l_ReducedAmount = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                /// https://worldofwarcraft.com/en-us/news/21365424
                if (Unit* l_Target = p_EventInfo.GetActionTarget())
                {
                    if (l_Player->GetActiveSpecializationID() == SpecIndex::SPEC_PALADIN_PROTECTION &&
                        (l_Player->CanApplyPvPSpellModifiers() || l_Target->IsPlayer()))
                        l_ReducedAmount /= 2;
                }

                l_Player->ReduceSpellCooldown(eSpells::HammerOfJustice, l_ReducedAmount * IN_MILLISECONDS);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_fist_of_justice_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_pal_fist_of_justice_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_fist_of_justice_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Fist of Justice - 234299
class spell_pal_fist_of_justice_retri : public SpellScriptLoader
{
    public:
        spell_pal_fist_of_justice_retri() : SpellScriptLoader("spell_pal_fist_of_justice_retri") { }

        enum eSpells
        {
            DivineStorm = 53385,
            WorldOfGlory = 210191,
            HammerOfJustice = 853
        };

        class spell_pal_fist_of_justice_retri_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_fist_of_justice_retri_AuraScript);

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (!GetUnitOwner() || !GetUnitOwner()->IsPlayer())
                    return false;

                Player* l_Owner = GetUnitOwner()->ToPlayer();

                if (l_Owner->GetActiveSpecializationID() != SPEC_PALADIN_RETRIBUTION)
                    return false;

                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_SpellInfo == nullptr)
                    return false;

                auto l_HolyPower = CalcUsedHolyPower(p_EventInfo);

                if (l_HolyPower == 0)
                    return false;

                /// Use cooldown to prevent multiply procs (Divine Storm and Word of Glory)
                if ((l_SpellInfo->Id == WorldOfGlory || l_SpellInfo->Id == DivineStorm) && !CheckAndUpdateCooldown())
                    return false;

                /// Leave this sentence here 'cause in OnEffectProc doesn't proc if caster have
                /// too much hasteeven with proc cd on 0
                l_Owner->ReduceSpellCooldown(eSpells::HammerOfJustice, 2500 * l_HolyPower);

                return true;
            }

            void HandleEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_fist_of_justice_retri_AuraScript::HandleOnCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_fist_of_justice_retri_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
            }

        private:

            uint32 CalcUsedHolyPower(ProcEventInfo& p_EventInfo)
            {
                if (!p_EventInfo.GetDamageInfo())
                    return 0;

                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (!l_SpellInfo)
                    return 0;

                if (l_SpellInfo->SpellPowers.empty())
                    return 0;

                uint32 l_HolyPowerCost = 0;

                for (auto spellPower : l_SpellInfo->SpellPowers)
                    if (spellPower->PowerType == POWER_HOLY_POWER)
                        l_HolyPowerCost += spellPower->Cost;

                return l_HolyPowerCost;
            }

            bool CheckAndUpdateCooldown()
            {
                if (m_Cooldown > time(NULL))
                    return false;

                m_Cooldown = time(NULL) + 1;

                return true;
            }

        private:

            time_t m_Cooldown;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_fist_of_justice_retri_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by The Fires of Justice - 203316
class spell_pal_the_fires_of_justice : public SpellScriptLoader
{
    public:
        spell_pal_the_fires_of_justice() : SpellScriptLoader("spell_pal_the_fires_of_justice") { }

        class spell_pal_the_fires_of_justice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_the_fires_of_justice_AuraScript);

            enum eSpells
            {
                CrusaderStrike = 35395
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::CrusaderStrike)
                    return false;

                return true;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_the_fires_of_justice_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_the_fires_of_justice_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blessing of Sanctuary - 210256
class spell_pal_blessing_of_sanctuary : public SpellScriptLoader
{
    public:
        spell_pal_blessing_of_sanctuary() : SpellScriptLoader("spell_pal_blessing_of_sanctuary") { }

        class spell_pal_blessing_of_sanctuary_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blessing_of_sanctuary_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                l_Target->RemoveAurasWithMechanic(((1 << MECHANIC_STUN) | (1 << MECHANIC_SILENCE) | (1 << MECHANIC_FEAR) | (1 << MECHANIC_HORROR)));
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_blessing_of_sanctuary_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_blessing_of_sanctuary_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Shield of Virtue - 215652
class spell_pal_shield_of_virtue : public SpellScriptLoader
{
    public:
        spell_pal_shield_of_virtue() : SpellScriptLoader("spell_pal_shield_of_virtue") { }

        class spell_pal_shield_of_virtue_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_shield_of_virtue_AuraScript);

            enum eSpells
            {
                AvengersShield = 31935
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::AvengersShield)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_shield_of_virtue_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_shield_of_virtue_AuraScript();
        }
};
/// Last Update : 7.1.5 Build 23420
/// Called by Seraphim's Blessing - 204927
class spell_pal_seraphims_blessing : public SpellScriptLoader
{
    public:
        spell_pal_seraphims_blessing() : SpellScriptLoader("spell_pal_seraphims_blessing") { }

        class spell_pal_seraphims_blessing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_seraphims_blessing_AuraScript);

            enum eSpells
            {
                SeraphimsBlessingAura = 210532
            };

            std::list<uint64> m_TargetList;

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                float l_Radius = (float)p_AurEff->GetAmount();

                std::list<Unit*> l_FriendlyList;
                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_FriendlyList, l_Check);
                l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                m_TargetList.clear();
                for (auto l_Target : l_FriendlyList)
                {
                    l_Caster->CastSpell(l_Target, eSpells::SeraphimsBlessingAura, true);
                    m_TargetList.push_back(l_Target->GetGUID());
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// Remove all the aura in Party member from caster
                for (auto l_UnitGUID : m_TargetList)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(l_UnitGUID);
                    if (l_Target)
                        l_Target->RemoveAura(eSpells::SeraphimsBlessingAura, l_Caster->GetGUID());
                }

                m_TargetList.clear();
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_pal_seraphims_blessing_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_AREATRIGGER);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_seraphims_blessing_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_seraphims_blessing_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Seraphim's Blessing - 210532
class spell_pal_seraphims_blessing_buff : public SpellScriptLoader
{
    public:
        spell_pal_seraphims_blessing_buff() : SpellScriptLoader("spell_pal_seraphims_blessing_buff") { }

        class spell_pal_seraphims_blessing_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_seraphims_blessing_buff_AuraScript);

            enum eSpells
            {
                SeraphimsBlessing       = 204927,
                SeraphimsBlessingTimer  = 205069,
                AvengingWrath           = 31884,
                Crusade                 = 231895,
                ChainOfThraynAura       = 206338,
                ChainOfThraynBuff       = 236328
            };

            bool HandleOnCheckProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Owner || !l_SpellInfo || l_Caster->HasAura(eSpells::SeraphimsBlessingTimer) || l_Owner->GetHealthPct() >= l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SeraphimsBlessing);
                SpellInfo const* l_SpellInfoAW = sSpellMgr->GetSpellInfo(eSpells::AvengingWrath);
                if (!l_Caster || !l_SpellInfo || !l_SpellInfoAW)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                SpellCategoryEntry const* l_ChargeCategoryEntry = l_SpellInfoAW->ChargeCategoryEntry;
                if (!l_ChargeCategoryEntry)
                    return;

                bool l_ShouldRestoreCharge = true;
                int32 l_ChargeRecoveryTime = 0;
                if (l_Player->m_CategoryCharges[l_ChargeCategoryEntry->Id].size())
                {
                    ChargeEntry l_ChargeEntry = l_Player->m_CategoryCharges[l_ChargeCategoryEntry->Id].back();
                    l_ChargeRecoveryTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - l_ChargeEntry.RechargeStart).count();
                }

                if (!l_Player->HasCharge(l_ChargeCategoryEntry))
                {
                    l_Player->RestoreCharge(l_ChargeCategoryEntry);
                    l_ShouldRestoreCharge = false;
                }

                uint32 l_SpellId = 0;
                if (l_Player->HasSpell(eSpells::Crusade))
                    l_SpellId = eSpells::Crusade;
                else
                    l_SpellId = eSpells::AvengingWrath;

                int32 l_Duration = l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS;
                Aura* l_Aura = l_Player->GetAura(l_SpellId);
                if (l_Aura)
                    l_Duration += l_Aura->GetDuration();

                l_Player->CastSpell(l_Caster, l_SpellId, true);

                if (l_Player->HasAura(eSpells::ChainOfThraynAura))
                    l_Player->CastSpell(l_Caster, eSpells::ChainOfThraynBuff, true);

                if (l_ShouldRestoreCharge)
                    l_Player->RestoreCharge(l_ChargeCategoryEntry);
                else if (l_ChargeRecoveryTime)
                    l_Player->ReduceChargeCooldown(l_ChargeCategoryEntry, l_ChargeRecoveryTime);

                l_Player->SendClearCooldown(l_SpellId, l_Player);

                if (!l_Aura)
                    l_Aura = l_Player->GetAura(l_SpellId);
                if (l_Aura)
                    l_Aura->SetDuration(l_Duration);

                l_Player->CastSpell(l_Player, eSpells::SeraphimsBlessingTimer, true);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_seraphims_blessing_buff_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_pal_seraphims_blessing_buff_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_seraphims_blessing_buff_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Greater Blessing of Kings - 203538, Greater Blessing of Wisdom - 203539
class spell_pal_greater_blessing : public SpellScriptLoader
{
    public:
        spell_pal_greater_blessing() : SpellScriptLoader("spell_pal_greater_blessing") { }

        class spell_pal_greater_blessing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_greater_blessing_AuraScript);

            enum eSpells
            {

                HammerOfReckoning           = 247675,
                ReckoningActivate           = 247676,
                ReckoningStacks             = 247677,
                GreaterBlessingOfKings      = 203538,
                GreaterBlessingOfWisdom     = 203539
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                if (l_DamageInfo->GetDamageType() != DamageEffectType::DIRECT_DAMAGE
                    && l_DamageInfo->GetDamageType() != DamageEffectType::SPELL_DIRECT_DAMAGE
                    && l_DamageInfo->GetDamageType() != DamageEffectType::DOT)
                    return;

                if (l_Caster->HasSpell(eSpells::HammerOfReckoning))
                    l_Caster->CastSpell(l_Caster, eSpells::ReckoningStacks, true);

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::GreaterBlessingOfWisdom))
                    l_Caster->SendSpellCooldown(eSpells::GreaterBlessingOfWisdom, 10 * IN_MILLISECONDS);
            }

            void OnUpdate(uint32 p_Diff, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                /// Prevent paladin from casting retribution blessings, then change spec and keep his blessings active
                if (l_Player->GetRoleForGroup() != Roles::ROLE_DAMAGE)
                    l_Target->RemoveAura(p_AurEff->GetId(), l_Caster->GetGUID());
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pal_greater_blessing_AuraScript::HandleOnProc);

                if (m_scriptSpellId == eSpells::GreaterBlessingOfKings)
                    OnEffectUpdate += AuraEffectUpdateFn(spell_pal_greater_blessing_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                else
                {
                    OnEffectUpdate += AuraEffectUpdateFn(spell_pal_greater_blessing_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_OBS_MOD_POWER);

                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_greater_blessing_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Hammer of Reckoning - 247675
class spell_pal_hammer_of_reckoning : public SpellScriptLoader
{
    public:
        spell_pal_hammer_of_reckoning() : SpellScriptLoader("spell_pal_hammer_of_reckoning") { }

        class spell_pal_hammer_of_reckoning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hammer_of_reckoning_SpellScript);

            enum eSpells
            {
                ReckoningActivate = 247676,
                ReckoningStacks = 247677,
                Crusade = 231895,
                AvengingWrath = 31884
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Caster->IsPlayer() || !l_SpellInfo)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::ReckoningStacks))
                {
                    if (l_Aura->GetStackAmount() >= l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    {
                        if (l_Caster->HasSpell(eSpells::AvengingWrath))
                        {
                            l_Caster->AddAura(eSpells::AvengingWrath, l_Caster);
                            if (Aura* l_Crusade = l_Caster->GetAura(eSpells::AvengingWrath))
                                l_Crusade->SetDuration(l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
                        }
                    }
                    l_Aura->Remove();
                }

                l_Caster->RemoveAura(eSpells::ReckoningActivate);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_hammer_of_reckoning_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_hammer_of_reckoning_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Reckoning - 247677
class spell_pal_reckoning : public SpellScriptLoader
{
public:
    spell_pal_reckoning() : SpellScriptLoader("spell_pal_reckoning") { }

    class spell_pal_reckoning_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_reckoning_AuraScript);

        enum eSpells
        {
            ReckoningActivate = 247676
        };

        void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::ReckoningActivate))
                l_Caster->CastSpell(l_Caster, eSpells::ReckoningActivate, true);
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::ReckoningActivate);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_pal_reckoning_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pal_reckoning_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pal_reckoning_AuraScript();
    }
};

/// Update to Legion 7.1.5 build: 23420
/// Called By Avenging Light (Honor Talent) 199441
class spell_pal_avenging_light : public SpellScriptLoader
{
    public:
        spell_pal_avenging_light() : SpellScriptLoader("spell_pal_avenging_light") { }

        class spell_pal_avenging_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_avenging_light_AuraScript);

            enum eSpells
            {
                HolyLight            = 82326,
                AvengingLightDamages = 199443
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::HolyLight)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_AvengingLightInfo = GetSpellInfo();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_AvengingLightInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                int32 l_HealAmount = l_DamageInfo->GetAmount() - l_DamageInfo->GetOverHeal();
                l_HealAmount = CalculatePct(l_HealAmount, l_AvengingLightInfo->Effects[EFFECT_0].BasePoints);
                l_Caster->CastCustomSpell(l_Target, eSpells::AvengingLightDamages, &l_HealAmount, nullptr, nullptr, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_avenging_light_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_avenging_light_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_avenging_light_AuraScript();
        }
};

/// Update to Legion 7.1.5 build: 23420
/// Called by Aura of Mastery - 31821
/// Called for Aura of Sacrifice (heal) - 210383
class spell_pal_aura_of_sacrifice_heal : public SpellScriptLoader
{
    public:
        spell_pal_aura_of_sacrifice_heal() : SpellScriptLoader("spell_pal_aura_of_sacrifice_heal") { }

        class spell_pal_aura_of_sacrifice_heal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_aura_of_sacrifice_heal_AuraScript);

            enum eSpells
            {
                AuraOfSacrifice     = 183416,
                AuraOfSacrificeHeal = 210383,
                AggramarBoon        = 255200
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo || !l_Caster->HasAura(eSpells::AuraOfSacrifice))
                    return false;

                if (l_Caster->HasAura(eSpells::AggramarBoon)) ///< Aura of Sacrifice can't proc if tank has Avatar of Aggramar in Antorus and Hpal is near of him
                    return false;

                if (l_DamageInfo->GetDamageType() != DamageEffectType::HEAL)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_ActionTarget = p_EventInfo.GetActionTarget();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AuraOfSacrifice);
                if (!l_Caster || !l_ActionTarget || !l_DamageInfo || !l_SpellInfo)
                    return;

                int32 l_Amount = CalculatePct(l_DamageInfo->GetAmount() - l_DamageInfo->GetOverHeal(), l_SpellInfo->Effects[EFFECT_3].BasePoints);
                std::set<uint64>& l_AuraOfSacrificeTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AuraOfSacrifice];

                for (uint64 l_TargetGUID : l_AuraOfSacrificeTargets)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGUID);
                    if (!l_Target || l_Target == l_ActionTarget)
                        continue;

                    l_Caster->CastCustomSpell(l_Target, eSpells::AuraOfSacrificeHeal, &l_Amount, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_aura_of_sacrifice_heal_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_aura_of_sacrifice_heal_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_aura_of_sacrifice_heal_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Divine Punisher - 204914
class spell_pal_divine_punisher : public SpellScriptLoader
{
    public:
        spell_pal_divine_punisher() : SpellScriptLoader("spell_pal_divine_punisher") { }

        class spell_pal_divine_punisher_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_divine_punisher_AuraScript);

            enum eSpells
            {
                Judgement       = 20271,
                DivinePunisher  = 216762
            };

            uint64 m_TargetGUID = 0;

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                Unit* l_Target = l_DamageInfo->GetTarget();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Target || !l_SpellInfo || l_SpellInfo->Id != eSpells::Judgement)
                    return false;

                if (m_TargetGUID != l_Target->GetGUID())
                {
                    m_TargetGUID = l_Target->GetGUID();
                    return false;
                }

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DivinePunisher, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_divine_punisher_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_divine_punisher_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_divine_punisher_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Shock Treatment - 200315
class spell_pal_shock_treatment : public SpellScriptLoader
{
    public:
        spell_pal_shock_treatment() : SpellScriptLoader("spell_pal_shock_treatment") { }

        class spell_pal_shock_treatment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_shock_treatment_AuraScript);

            enum eArtifactPowers
            {
                ShockTreatmentArtifact = 972
            };

            int32 CalcValue()
            {
                int32 l_Value = 0;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Owner = GetUnitOwner();

                if (!l_SpellInfo || !l_Owner)
                    return 0;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::ShockTreatmentArtifact);
                    l_Value = l_Rank * l_SpellInfo->Effects[0].BasePoints;
                }

                return l_Value;
            }

            void HandleCalcAmount(AuraEffect const*, int32& p_Amount, bool& )
            {
                p_Amount = CalcValue() / 2;
            }

            void HandleCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
            {
                spellMod->value = CalcValue();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_shock_treatment_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pal_shock_treatment_AuraScript::HandleCalcSpellMod, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_shock_treatment_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Maraad's Dying Breath - 234862
class spell_pal_maarads_dying_breath : public SpellScriptLoader
{
    public:
        spell_pal_maarads_dying_breath() : SpellScriptLoader("spell_pal_maarads_dying_breath") { }

        class spell_pal_maarads_dying_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_maarads_dying_breath_AuraScript);

            enum eSpells
            {
                LightoftheMartyr = 219562
            };

            void HandleOnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::LightoftheMartyr, l_Caster->GetGUID());
                if (!l_Aura)
                    return;

                l_Aura->SetScriptGuid(1, 0);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_maarads_dying_breath_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_maarads_dying_breath_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Arcing Light - 119952
class spell_pal_arcing_light : public SpellScriptLoader
{
    public:
        spell_pal_arcing_light() : SpellScriptLoader("spell_pal_arcing_light") { }

        class spell_pal_arcing_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_arcing_light_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                /// Healing up to 6 allies
                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                if (p_Targets.size() > 6)
                    p_Targets.resize(6);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_arcing_light_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_arcing_light_SpellScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Aura Of Mercy - 210291
class spell_pal_aura_of_mercy : public SpellScriptLoader
{
    public:
        spell_pal_aura_of_mercy() : SpellScriptLoader("spell_pal_aura_of_mercy") { }

        class spell_pal_aura_of_mercy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_aura_of_mercy_SpellScript);

            enum eSpells
            {
                AuraMastery = 31821
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());

                if (!l_Caster->HasAura(eSpells::AuraMastery))
                {
                    if (p_Targets.size() > 3)
                        p_Targets.resize(3);
                }

                for (std::list<WorldObject*>::const_iterator l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    if (Unit* l_Target = (*l_Itr)->ToUnit())
                    {
                        if (G3D::fuzzyEq(l_Target->GetHealthPct(), 100.0f))
                        {
                            l_Itr = p_Targets.erase(l_Itr);
                            continue;
                        }
                    }

                    l_Itr++;
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_aura_of_mercy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_aura_of_mercy_SpellScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Legendary: Soul of the Highlord - 247566
class spell_pal_soul_of_the_highlord : public SpellScriptLoader
{
    public:
        spell_pal_soul_of_the_highlord() : SpellScriptLoader("spell_pal_soul_of_the_highlord") { }

        class spell_pal_soul_of_the_highlord_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_soul_of_the_highlord_AuraScript);

            enum eSpells
            {
                HolyShield          = 152261,
                DivinePurposeHeal   = 197646,
                DivinePurpose       = 223817
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
                case SPEC_PALADIN_HOLY:
                    l_Player->CastSpell(l_Player, eSpells::DivinePurposeHeal, true);
                    break;
                case SPEC_PALADIN_PROTECTION:
                    l_Player->CastSpell(l_Player, eSpells::HolyShield, true);
                    break;
                default:
                    l_Player->CastSpell(l_Player, eSpells::DivinePurpose, true);
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
                case SPEC_PALADIN_HOLY:
                    l_Player->RemoveAura(eSpells::DivinePurposeHeal);
                    break;
                case SPEC_PALADIN_PROTECTION:
                    l_Player->RemoveAura(eSpells::HolyShield);
                    break;
                default:
                    l_Player->RemoveAura(eSpells::DivinePurpose);
                    break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_soul_of_the_highlord_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_soul_of_the_highlord_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_soul_of_the_highlord_AuraScript();
        }
};

/// Last Update 7.1.5 build 23420
/// Called By Paladin T19 Retribution 4P Bonus - 211448
class spell_pal_t19_retribution_4p_bonus : public SpellScriptLoader
{
public:
    spell_pal_t19_retribution_4p_bonus() : SpellScriptLoader("spell_pal_t19_retribution_4p_bonus") { }

    class spell_pal_t19_retribution_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_t19_retribution_4p_bonus_AuraScript);

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo)
                return false;

            for (uint8 l_I = 0; l_I < l_SpellInfo->EffectCount; ++l_I)
            {
                if (l_SpellInfo->Effects[l_I].Effect == SPELL_EFFECT_ENERGIZE && l_SpellInfo->Effects[l_I].MiscValue == POWER_HOLY_POWER)
                    return true;
            }

            return false;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_t19_retribution_4p_bonus_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pal_t19_retribution_4p_bonus_AuraScript();
    }
};

/// Last Update 7.1.5 build 23420
/// Called By Paladin T19 Protection 4P Bonus - 211554
class spell_pal_t19_protection_4p_bonus : public SpellScriptLoader
{
public:
    spell_pal_t19_protection_4p_bonus() : SpellScriptLoader("spell_pal_t19_protection_4p_bonus") { }

    class spell_pal_t19_protection_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_t19_protection_4p_bonus_AuraScript);

        enum eSpells
        {
            ShieldOfTheRighteous    = 53600,
            LightOfTheProtector     = 184092,
            HandOfTheProtector      = 213652
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_ProcDamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_ProcDamageInfo)
                return false;

            SpellInfo const* l_ProcSpellInfo = l_ProcDamageInfo->GetSpellInfo();
            if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::ShieldOfTheRighteous)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Player->HasSpell(eSpells::HandOfTheProtector) ? eSpells::HandOfTheProtector : eSpells::LightOfTheProtector);
            if (!l_SpellInfo)
                return;

            SpellCategoryEntry const* l_JudgmentCategoryEntry = l_SpellInfo->ChargeCategoryEntry;
            if (!l_JudgmentCategoryEntry)
                return;

            l_Player->RestoreCharge(l_JudgmentCategoryEntry);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_t19_protection_4p_bonus_AuraScript::HandleCheckProc);
            OnProc += AuraProcFn(spell_pal_t19_protection_4p_bonus_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pal_t19_protection_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called for Righteous Verdict - 238062
/// Called By DivinStorm - 53385, Justiciar's Vengeance - 215661, Templar Verdict - 85256, Execution Sentence - 213757
class spell_pal_righteous_verdict : public SpellScriptLoader
{
    public:
        spell_pal_righteous_verdict() : SpellScriptLoader("spell_pal_righteous_verdict") { }

        class spell_pal_righteous_verdict_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_righteous_verdict_SpellScript);

            enum eSpells
            {
                RighteousVerdictAura    = 238062,
                RighteousVerdict        = 238996
            };

            enum eArtifact
            {
                RighteousVerdictArtifact = 1561
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::RighteousVerdictAura) || !l_Caster->IsPlayer())
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::RighteousVerdictAura);
                if (!l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                int32 l_Bonus = l_Player->GetRankOfArtifactPowerId(RighteousVerdictArtifact) * l_SpellInfo->Effects[EFFECT_0].BasePoints;
                l_Caster->CastCustomSpell(l_Caster, eSpells::RighteousVerdict, &l_Bonus, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_righteous_verdict_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_righteous_verdict_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called for Blessing of the Ashbringer - 238098
/// Called By Greater Blessing of Wisdom - 203539, Greater Blessing of Kings - 203538
class spell_pal_blessing_of_the_ashbringer : public SpellScriptLoader
{
    public:
        spell_pal_blessing_of_the_ashbringer() : SpellScriptLoader("spell_pal_blessing_of_the_ashbringer") { }

        class spell_pal_blessing_of_the_ashbringer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_blessing_of_the_ashbringer_AuraScript);

            enum eSpells
            {
                GreaterBlessingOfKings      = 203538,
                GreaterBlessingOfWisdom     = 203539,
                BlessingOfTheAshbringer     = 242981,
                BlessingOfTheAshbringerAura = 238098
            };

            void HandleEffectApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                Aura* l_Aura = p_AuraEffect->GetBase();
                if (!l_Caster || !l_Target || !l_Aura)
                    return;

                if (l_Aura->GetId() == eSpells::GreaterBlessingOfKings)
                {
                    Unit* l_OldTarget = sObjectAccessor->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::GreaterBlessingOfKings));
                    if (l_OldTarget && l_OldTarget != l_Target)
                        l_OldTarget->RemoveAura(eSpells::GreaterBlessingOfKings);

                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::GreaterBlessingOfKings) = l_Target->GetGUID();
                }
                if (l_Aura->GetId() == eSpells::GreaterBlessingOfWisdom)
                {
                    Unit* l_OldTarget = sObjectAccessor->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::GreaterBlessingOfWisdom));
                    if (l_OldTarget && l_OldTarget != l_Target)
                        l_OldTarget->RemoveAura(eSpells::GreaterBlessingOfWisdom);

                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::GreaterBlessingOfWisdom) = l_Target->GetGUID();
                }

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BlessingOfTheAshbringer) += 1;
            }

            void HandleEffectRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BlessingOfTheAshbringer) -= 1;
                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BlessingOfTheAshbringer) < 2)
                    l_Caster->RemoveAura(eSpells::BlessingOfTheAshbringer);
            }

            void OnUpdate(uint32 /*p_Diff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BlessingOfTheAshbringer) == 2)
                        if (l_Caster->HasAura(eSpells::BlessingOfTheAshbringerAura) && !l_Caster->HasAura(eSpells::BlessingOfTheAshbringer) && l_Caster->isAlive())
                            l_Caster->CastSpell(l_Caster, eSpells::BlessingOfTheAshbringer, true);
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::GreaterBlessingOfKings:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_pal_blessing_of_the_ashbringer_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_blessing_of_the_ashbringer_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        break;
                    }
                    case eSpells::GreaterBlessingOfWisdom:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_pal_blessing_of_the_ashbringer_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_OBS_MOD_POWER, AURA_EFFECT_HANDLE_REAL);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_blessing_of_the_ashbringer_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_OBS_MOD_POWER, AURA_EFFECT_HANDLE_REAL);
                        break;
                    }
                    default:
                        break;
                }

                OnAuraUpdate += AuraUpdateFn(spell_pal_blessing_of_the_ashbringer_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_blessing_of_the_ashbringer_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Paladin T19 Protection 4P Bonus - 211554
class spell_pal_blessed_stalwart : public SpellScriptLoader
{
    public:
        spell_pal_blessed_stalwart() : SpellScriptLoader("spell_pal_blessed_stalwart") { }

        class spell_pal_blessed_stalwart_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_blessed_stalwart_AuraScript);

            enum eSpells
            {
                Judgment = 20271
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_ProcDamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_ProcDamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_ProcDamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::Judgment)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_blessed_stalwart_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_blessed_stalwart_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By  Judgment - 197277
class spell_pal_judgment_unworthy : public SpellScriptLoader
{
    public:
        spell_pal_judgment_unworthy() : SpellScriptLoader("spell_pal_judgment_unworthy") { }

        class spell_pal_judgment_unworthy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_judgment_unworthy_AuraScript);

            enum eSpells
            {
                JudgeUnworthy = 238134,
                Judgment      = 197277
            };

            bool HandleCheckProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::JudgeUnworthy))
                    return false;

                return true;
            }

            void HandleProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_MainTarget = GetTarget();
                if (!l_Caster || !l_MainTarget)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::JudgeUnworthy);
                Unit* l_Target = l_Caster->SelectNearbyTarget(nullptr, 5.0f, eSpells::Judgment);
                if (!l_Target || !l_SpellInfo || !roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                    return;

                l_Caster->AddAura(eSpells::Judgment, l_Target);
                Aura* l_MainAura = l_MainTarget->GetAura(eSpells::Judgment);
                Aura* l_Aura = l_Target->GetAura(eSpells::Judgment);
                if (l_Aura && l_MainAura)
                    l_Aura->SetDuration(l_MainAura->GetDuration());
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_judgment_unworthy_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_judgment_unworthy_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_judgment_unworthy_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called By Power of the Silver Hand - 200474
class spell_pal_power_of_the_silver_hand : public SpellScriptLoader
{
public:
    spell_pal_power_of_the_silver_hand() : SpellScriptLoader("spell_pal_power_of_the_silver_hand") { }

    class spell_pal_power_of_the_silver_hand_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_power_of_the_silver_hand_AuraScript);

        enum eSpells
        {
            HolyLight = 82326,
            FlashOfLight = 19750
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::HolyLight && l_SpellInfo->Id != eSpells::FlashOfLight))
                return false;

            return true;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_power_of_the_silver_hand_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_power_of_the_silver_hand_AuraScript();
    }
};

/// Last Update 7.3.5
/// Repentance - 20066
class spell_pal_repentance : public SpellScriptLoader
{
public:
    spell_pal_repentance() : SpellScriptLoader("spell_pal_repentance") { }

    class spell_pal_repentance_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_repentance_SpellScript);

        void HandleAfterCast()
        {
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Target)
                return;

            l_Target->RemoveAurasWithMechanic((1LL << MECHANIC_BLEED) | (1LL << MECHANIC_INFECTED));
            l_Target->RemoveAurasByType(AuraType::SPELL_AURA_PERIODIC_DAMAGE);
            l_Target->RemoveAurasByType(AuraType::SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_pal_repentance_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pal_repentance_SpellScript();
    }
};

/// Last Update : 7.3.5 Build 26365
/// Called by Item - Paladin T20 Protection 2P Bonus - 242263
class spell_pal_t20_protection_2p_bonus_proc : public SpellScriptLoader
{
    public:
        spell_pal_t20_protection_2p_bonus_proc() : SpellScriptLoader("spell_pal_t20_protection_2p_bonus_proc") { }

        class spell_pal_t20_protection_2p_bonus_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_t20_protection_2p_bonus_proc_AuraScript);

            enum eSpells
            {
                AvengersShield      = 31935,
                Judgment            = 20271
            };

            bool HandleOnCheckProc(ProcEventInfo&  p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo)
                    return false;

                if (l_ProcSpellInfo->Id == eSpells::Judgment)
                    return true;

                return false;
            }

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasSpellCooldown(eSpells::AvengersShield))
                    l_Player->RemoveSpellCooldown(eSpells::AvengersShield, true);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_t20_protection_2p_bonus_proc_AuraScript::HandleOnCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_t20_protection_2p_bonus_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_t20_protection_2p_bonus_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Paladin T20 Protection 4P Bonus - 242264
class spell_pal_t20_protection_4p_bonus_proc : public SpellScriptLoader
{
    public:
        spell_pal_t20_protection_4p_bonus_proc() : SpellScriptLoader("spell_pal_t20_protection_4p_bonus_proc") { }

        class spell_pal_t20_protection_4p_bonus_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_t20_protection_4p_bonus_proc_AuraScript);

            enum eSpells
            {
                AvengersShield = 31935
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::AvengersShield)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_t20_protection_4p_bonus_proc_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_t20_protection_4p_bonus_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Pure of Heart - 210540
class spell_pal_pure_of_heart_proc: public SpellScriptLoader
{
    public:
        spell_pal_pure_of_heart_proc() : SpellScriptLoader("spell_pal_pure_of_heart_proc") { }

        class spell_pal_pure_of_heart_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_pure_of_heart_proc_AuraScript);

            enum eSpells
            {
                PureOfHeartDispel   = 199427
            };

            bool HandleCheckProc(ProcEventInfo & p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr)
                    return false;

                if (p_Proc.GetDamageInfo()->GetSpellInfo() == nullptr)
                    return false;

                return p_Proc.GetDamageInfo()->GetSpellInfo()->IsHealingSpell();
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Target = p_EventInfo.GetActionTarget();

                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::PureOfHeartDispel, true);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_pure_of_heart_proc_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pal_pure_of_heart_proc_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_pure_of_heart_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Avenging Crusader - 216371
class spell_pal_avenging_crusader_vs_cyclone : public SpellScriptLoader
{
    public:
        spell_pal_avenging_crusader_vs_cyclone() : SpellScriptLoader("spell_pal_avenging_crusader_vs_cyclone") { }

        class spell_pal_avenging_crusader_vs_cyclone_SpellScript : public SpellScript
        {
            enum eSpells
            {
                Cyclone                     = 33786,
                AvengingCrusaderOriginal    = 216331
            };

            PrepareSpellScript(spell_pal_avenging_crusader_vs_cyclone_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());

                p_Targets.remove_if([=](WorldObject* l_Target) -> bool
                {
                    if (!l_Target)
                        return true;

                    if (!l_Target->ToUnit())
                        return true;

                    if (l_Target->ToUnit() == l_Caster)
                        return true;

                    if (!l_Caster->IsFriendlyTo(l_Target->ToUnit()))
                        return true;

                    if (l_Target->ToUnit()->HasAura(eSpells::Cyclone))
                        return true;

                    return false;
                });

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AvengingCrusaderOriginal);
                if (!l_SpellInfo)
                    return;

                if (p_Targets.size() > l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    p_Targets.resize(l_SpellInfo->Effects[EFFECT_1].BasePoints);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_avenging_crusader_vs_cyclone_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_avenging_crusader_vs_cyclone_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Judgment - 197277
class spell_pal_judgment_amount : public SpellScriptLoader
{
    public:
        spell_pal_judgment_amount() : SpellScriptLoader("spell_pal_judgment_amount") { }

        class spell_pal_judgment_amount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_judgment_amount_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                float l_Mastery = (l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * 1.75f);
                p_Amount = (int32)l_Mastery + 10;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_judgment_amount_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_judgment_amount_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Tyr's Munificence - 238060
class spell_pal_tyrs_munificence : public SpellScriptLoader
{
public:
    spell_pal_tyrs_munificence() : SpellScriptLoader("spell_pal_tyrs_munificence") { }

    class spell_pal_tyrs_munificence_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_tyrs_munificence_AuraScript);

        enum eArtifactTraits
        {
            TyrsMunificenceTraits = 1553
        };

        void HandleModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_AuraEffect->GetId());
            if (!l_Caster || !l_SpellInfo)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactTraits::TyrsMunificenceTraits);

            /// At rank 4 must be 20%,  Tyr's Munificence - 238060. In SpellWorks it's 2%
            p_Amount = 5 * l_Rank;
            p_CanBeRecalculated = false;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_tyrs_munificence_AuraScript::HandleModifier, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pal_tyrs_munificence_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Item - Paladin T21 Holy 4P Bonus - 251865
class spell_pal_t21_holy_4p_bonus : public SpellScriptLoader
{
public:
    spell_pal_t21_holy_4p_bonus() : SpellScriptLoader("spell_pal_t21_holy_4p_bonus") { }

    class spell_pal_t21_holy_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_t21_holy_4p_bonus_AuraScript);

        enum eSpells
        {
            HolyShockDmg    = 25912,
            HolyShockHeal   = 25914
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::HolyShockDmg && l_SpellInfo->Id != eSpells::HolyShockHeal))
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_t21_holy_4p_bonus_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pal_t21_holy_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Item - Paladin T21 Protection 2P Bonus - 251869
/// Called for Item - Paladin T21 Protection 4P Bonus - 251870
class spell_pal_t21_protection_2p_bonus : public SpellScriptLoader
{
public:
    spell_pal_t21_protection_2p_bonus() : SpellScriptLoader("spell_pal_t21_protection_2p_bonus") { }

    class spell_pal_t21_protection_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_t21_protection_2p_bonus_AuraScript);

        enum eSpells
        {
            VindicatorsShield = 253331,
            T21Protection4PBonus = 251870,
            ShieldOfTheRighteous = 132403
        };

        void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
        {
            if (Unit* l_Caster = GetCaster())
                l_Caster->CastSpell(l_Caster, eSpells::VindicatorsShield, true);
        }

        void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (l_Caster->HasAura(eSpells::VindicatorsShield))
                    l_Caster->RemoveAura(eSpells::VindicatorsShield);
            }
        }

        void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (AuraEffect* l_AuraEffect4p = l_Caster->GetAuraEffect(eSpells::T21Protection4PBonus, EFFECT_0))
            {
                if (AuraEffect* l_AuraEffectVS = l_Caster->GetAuraEffect(eSpells::VindicatorsShield, EFFECT_0))
                {
                    if (!l_Caster->HasAura(eSpells::ShieldOfTheRighteous))
                    {
                        int32 l_Amount = l_AuraEffectVS->GetBaseAmount();
                        AddPct(l_Amount, l_AuraEffect4p->GetAmount());
                        l_AuraEffectVS->ChangeAmount(l_Amount);
                    }
                    else
                        l_AuraEffectVS->ChangeAmount(l_AuraEffectVS->GetBaseAmount());
                }
            }

            else if (AuraEffect* l_AuraEffectVS = l_Caster->GetAuraEffect(eSpells::VindicatorsShield, EFFECT_0))
                l_AuraEffectVS->ChangeAmount(l_AuraEffectVS->GetBaseAmount());
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_pal_t21_protection_2p_bonus_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pal_t21_protection_2p_bonus_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectUpdate += AuraEffectUpdateFn(spell_pal_t21_protection_2p_bonus_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_t21_protection_2p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Item - Paladin T21 Retribution 4P Bonus - 251868
class spell_pal_t21_retribution_4p_bonus : public SpellScriptLoader
{
public:
    spell_pal_t21_retribution_4p_bonus() : SpellScriptLoader("spell_pal_t21_retribution_4p_bonus") { }

    class spell_pal_t21_retribution_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_t21_retribution_4p_bonus_AuraScript);

        enum eSpells
        {
            Judgment = 20271
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Judgment)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_t21_retribution_4p_bonus_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pal_t21_retribution_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Sacred Judgment - 253806
class PlayerScript_sacred_judgment : public PlayerScript
{
public:
    PlayerScript_sacred_judgment() :PlayerScript("PlayerScript_sacred_judgment") {}

    enum eSpells
    {
        T21Retribution4PBonus = 251868,
        SacredJudgment = 253806
    };

    void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
    {
        if (p_After)
            return;

        if (p_Regen)
            return;

        if (p_Power != POWER_HOLY_POWER)
            return;

        if (p_OldValue <= p_NewValue)
            return;

        if (p_Player->HasAura(eSpells::T21Retribution4PBonus) && p_Player->HasAura(eSpells::SacredJudgment))
            p_Player->RemoveAura(eSpells::SacredJudgment);
    }
};

class PlayerScript_pal_scarlet_inquisitor_expurgation : public PlayerScript
{
    public:
        PlayerScript_pal_scarlet_inquisitor_expurgation() : PlayerScript("PlayerScript_pal_scarlet_inquisitor_expurgation") { }

        enum eSpells
        {
            ScarletInquisitorAura = 248289
        };

        void OnUpdateZone(Player* p_Player, uint32 /*p_NewZoneID*/, uint32 /*p_OldZoneID*/, uint32 /*p_NewAreaID*/)
        {
            if (p_Player->GetMap()->IsBattlegroundOrArena() && p_Player->HasAura(eSpells::ScarletInquisitorAura)) ///< Can be stack up before entering in BG/Arena
            {
                p_Player->RemoveAura(eSpells::ScarletInquisitorAura);
            }
        }
};

class PlayerScript_pal_soul_of_the_highlord : public PlayerScript
{
    public:
        PlayerScript_pal_soul_of_the_highlord() : PlayerScript("PlayerScript_pal_soul_of_the_highlord") { }

        enum eSpells
        {
            SoulOfTheHighlord  = 247566,
            SoulOfTheHighlord2 = 247578,
            SoulOfTheHighlord3 = 247579,
            SoulOfTheHighlord4 = 247580
        };

        void OnUpdateZone(Player* p_Player, uint32 /*p_NewZoneID*/, uint32 /*p_OldZoneID*/, uint32 /*p_NewAreaID*/)
        {
            if (p_Player->GetMap()->IsBattlegroundOrArena() && p_Player->HasAura(eSpells::SoulOfTheHighlord))
            {
                p_Player->RemoveAura(eSpells::SoulOfTheHighlord);
                p_Player->RemoveAura(eSpells::SoulOfTheHighlord2);
                p_Player->RemoveAura(eSpells::SoulOfTheHighlord3);
                p_Player->RemoveAura(eSpells::SoulOfTheHighlord4);
            }
        }
};


#ifndef __clang_analyzer__
void AddSC_paladin_spell_scripts()
{
    new spell_pal_shock_treatment();
    new spell_pal_steed_of_glory();
    new spell_pal_fervent_martyr();
    new spell_pal_light_of_dawn_heal();
    new spell_pal_darkest_before_the_dawn();
    new spell_pal_lawbringer();
    new spell_pal_lawbringer_judgement();
    new spell_pal_blade_of_justice();
    new spell_pal_hand_of_hindrance();
    new spell_pal_greater_blessing_of_king();
    new spell_pal_echo_of_the_highlord();
    new spell_pal_wake_of_ashes();
    new spell_pal_divine_hammer();
    new spell_pal_light_of_the_protector();
    new spell_pal_templar_verdict();
    new spell_pal_bastion_of_light();
    new spell_pal_holy_wrath();
    new spell_pal_shield_of_vengeance();
    new spell_pal_shield_of_vengeance_damage();
    new spell_pal_divine_intervention();
    new spell_pal_word_of_glory();
    new spell_pal_eye_for_an_eye();
    new spell_pal_zeal();
    new spell_pal_last_defender();
    new spell_pal_seraphim();
    new spell_pal_consecrated_ground_heal();
    new spell_pal_consecrated_ground();
    new spell_pal_aegis_of_light();
    new spell_pal_hand_of_the_protector();
    new spell_pal_retribution_aura();
    new spell_pal_hammer_of_the_righteous();
    new spell_pal_hammer_of_the_righteous_aoe();
    new spell_pal_beacon_of_virtue();
    new spell_pal_light_of_martyr_heal();
    new spell_pal_light_of_the_martyr();
    new spell_pal_judgment_of_light_aura();
    new spell_pal_judgment_of_light();
    new spell_pal_aura_of_devoltion();
    new spell_pal_aura_of_sacrifice_area();
    new spell_pal_aura_of_sacrifice();
    new spell_pal_divine_steed();
    new spell_pal_divine_steed_mount();
    new spell_pal_crusader_might();
    new spell_pal_hammer_of_justice();
    new spell_pal_hand_of_sacrifice();
    new spell_pal_glyph_of_pillar_of_light();
    new spell_pal_empowered_seals();
    new spell_pal_lights_beacon_heal();
    new spell_pal_beacon_of_light();
    new spell_pal_beacon_of_light_proc();
    new spell_pal_turn_evil();
    new spell_pal_holy_shield();
    new spell_pal_divine_purpose();
    new spell_pal_hammer_of_wrath();
    new spell_pal_eternal_flame();
    new spell_pal_divine_storm();
    new spell_pal_divine_storm_damage();
    new spell_pal_shield_of_the_righteous();
    new spell_pal_selfless_healer();
    new spell_pal_blinding_light();
    new spell_pal_hand_of_protection();
    new spell_pal_cleanse();
    new spell_pal_divine_shield();
    new spell_pal_lights_hammer_tick();
    new spell_pal_lights_hammer();
    new spell_pal_holy_prism();
    new spell_pal_holy_prism_visual();
    new spell_pal_holy_prism_visual_effect();
    new spell_pal_judgment();
    new spell_pal_judgment_additional();
    new spell_pal_ardent_defender();
    new spell_pal_blessing_of_faith();
    new spell_pal_lay_on_hands();
    new spell_pal_righteous_defense();
    new spell_pal_sanctified_wrath();
    new spell_pal_selfless_healer_proc();
    new spell_pal_glyph_of_the_liberator();
    new spell_pal_avengers_shield();
    new spell_pal_grand_crusader();
    new spell_pal_grand_crusader_proc();
    new spell_pal_glyph_of_the_luminous_charger();
    new spell_pal_tyrs_deliverance();
    new spell_pal_crusaders_judgment();
    new spell_pal_inquisition();
    new spell_pal_retribution();
    new spell_pal_blinding_light_confuse();
    new spell_pal_liadrin_fury_unleashed();
    new spell_pal_repentance();
    new spell_pal_t21_protection_2p_bonus();
    new spell_pal_t21_retribution_4p_bonus();
    new PlayerScript_sacred_judgment();

    /// Holy
    new spell_pal_ilterendi_crown_jewel_of_silvermoon();
    new spell_pal_obsidian_stone_spaulders();
    new spell_pal_maraads_dying_breath();
    new spell_pal_holy_shock();
    new spell_pal_light_of_dawn();
    new spell_pal_aura_of_sacrifice_heal();
    new spell_pal_arcing_light();
    new spell_pal_aura_of_mercy();
    new spell_pal_divine_purpose_lod();
    new spell_pal_t21_holy_4p_bonus();

    /// Retribution
    new spell_pal_whisper_of_the_nathrezim();
    new spell_pal_aegisjalmure_the_armguards_of_awe();
    new spell_pal_execution_sentence();
    new spell_pal_justicar_vengeance();
    new spell_pal_the_fires_of_justice();
    new spell_pal_t19_retribution_4p_bonus();
    new spell_pal_righteous_verdict();
    new spell_pal_blessing_of_the_ashbringer();
    new spell_pal_judgment_unworthy();
    new spell_pal_judgment_amount();

    /// Protection
    new spell_pal_forbearant_faithful();
    new spell_pal_light_of_the_titans();
    new spell_pal_blessing_of_spellwarding();
    new spell_pal_t19_protection_4p_bonus();
    new spell_pal_t17_protection_4p();
    new spell_pal_blessed_stalwart();
    new spell_pal_t20_protection_2p_bonus_proc();
    new spell_pal_t20_protection_4p_bonus_proc();

    /// Talent
    new spell_pal_blade_of_wrath();
    new spell_pal_fist_of_justice();
    new spell_pal_fist_of_justice_retri();

    /// Legendary Items
    new spell_pal_uthers_guard();
    new spell_pal_justice_gaze();
    new spell_pal_maarads_dying_breath();
    new spell_pal_soul_of_the_highlord();
    new spell_pal_healthcliff_immortality();

    /// Artifact Traits
    new spell_pal_faiths_armor();
    new spell_pal_protection_of_tyr();
    new spell_pal_vindicator();
    new spell_pal_the_light_saves();
    new spell_pal_power_of_the_silver_hand_damages();
    new spell_pal_power_of_the_silver_hand_bonus();
    new spell_pal_unbreakable_will();
    new spell_pal_unbreakable_will_crowed_controlled();
    new spell_pal_second_sunrise();
    new spell_pal_painful_truths();
    new spell_pal_eye_of_tyr();
    new spell_pal_power_of_the_silver_hand();
    new spell_pal_tyrs_munificence();

    /// Honor Talents
    new spell_pal_holy_ritual();
    new spell_pal_guardian_of_the_forgotten_queen();
    new spell_pal_avenging_crusader();
    new spell_pal_ultimate_sacrifice();
    new spell_pal_luminescence();
    new spell_pal_vengeance_aura();
    new spell_pal_blessing_of_sanctuary();
    new spell_pal_shield_of_virtue();
    new spell_pal_seraphims_blessing();
    new spell_pal_seraphims_blessing_buff();
    new spell_pal_greater_blessing();
    new spell_pal_hammer_of_reckoning();
    new spell_pal_reckoning();
    new spell_pal_avenging_light();
    new spell_pal_divine_punisher();
    new spell_pal_lawbringer_honor();
    new spell_pal_pure_of_heart_proc();
    new spell_pal_avenging_crusader_vs_cyclone();

    /// PlayerScripts
    new PlayerScript_crusade();
    new PlayerScript_the_light_saves();
    new PlayerScript_paladin_wod_pvp_4p_bonus();
    new PlayerScript_pal_after_cast();
    new PlayerScript_Aura_of_devotion();
    new PlayerScript_boundless_conviction();
    new PlayerScript_beacon_of_light();
    new PlayerScript_pal_healthcliff_immortality();
    new PlayerScript_pal_scarlet_inquisitor_expurgation();
    new PlayerScript_pal_soul_of_the_highlord();
}
#endif
