////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_MONK and SPELLFAMILY_GENERIC spells used by monk players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_monk_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "HelperDefines.h"
#include "Unit.h"

/// Last Update 7.3.5
/// Called for Serenity - 152173
class PlayerScript_Serenity: public PlayerScript
{
    public:
        PlayerScript_Serenity() :PlayerScript("PlayerScript_Serenity") {}

        enum eSpells
        {
            Serenity          = 152173,
            DrinkingHornCover = 209256,
            StormEarthAndFire = 137639
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool /*p_Regen*/, bool p_After)
        {
            if (p_After)
                return;

            // Get the power earn (if > 0 ) or consum (if < 0)
            int32 l_DiffVal = (p_OldValue - p_NewValue) / p_Player->GetPowerCoeff(p_Power);

            if (p_Power == POWER_CHI && l_DiffVal > 0)
                if (p_Player->HasAura(eSpells::Serenity))
                    p_NewValue = p_OldValue;
        }
};

class PlayerScript_Monk_T19_Windwalker_4P_Bonus : public PlayerScript
{
public:
    PlayerScript_Monk_T19_Windwalker_4P_Bonus() :PlayerScript("PlayerScript_Monk_T19_Windwalker_4P_Bonus") {}

    enum eSpells
    {
        SetActiveBonus = 211432
    };
    virtual void OnModifySpec(Player* p_Player, int32 /* p_OldSpec */, int32 /* p_NewSpec */)
    {
        std::list<uint64>& l_List = p_Player->m_SpellHelper.GetUint64List()[eSpellHelpers::TwoLastCastedSpellsIDs];
        l_List.clear();

        p_Player->AddDelayedEvent([p_Player]()->void
        {
            std::list<uint64>& l_List = p_Player->m_SpellHelper.GetUint64List()[eSpellHelpers::TwoLastCastedSpellsIDs];
            l_List.clear();
        }, 0);

        p_Player->RemoveAura(eSpells::SetActiveBonus);
    }
};

/// Last Update 7.3.5 build 26365
/// 195267 Strength of Xuen
/// Called By 137384 Combo Breaker
class spell_monk_strength_of_xuen_addition : public SpellScriptLoader
{
    public:
        spell_monk_strength_of_xuen_addition() : SpellScriptLoader("spell_monk_strength_of_xuen_addition") { }

        class spell_monk_strength_of_xuen_addition_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_strength_of_xuen_addition_AuraScript);

            enum eSpells
            {
                StrengthOfXuen  = 195267
            };

            enum eTraits
            {
                StrengthOfXuenTrait = 1094
            };

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !GetSpellInfo())
                    return;

                if (!l_Caster->HasAura(eSpells::StrengthOfXuen))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::StrengthOfXuen);
                if (!l_SpellInfo)
                    return;

                int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eTraits::StrengthOfXuenTrait);
                p_Amount = GetSpellInfo()->Effects[EFFECT_0].BasePoints + l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_strength_of_xuen_addition_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_strength_of_xuen_addition_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365 Called by 100780 - Jab
/// Called by Tiger Palm - 100780
/// Called for: Blackout Kick! - 116768
class spell_monk_combo_breaker: public SpellScriptLoader
{
    public:
        spell_monk_combo_breaker() : SpellScriptLoader("spell_monk_combo_breaker") { }

        class spell_monk_combo_breaker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_combo_breaker_SpellScript);

            enum eSpells
            {
                ComboBreakerAura            = 137384,
                ComboBreakerBlackoutKick    = 116768,
                StrengthOfXuen              = 195267
            };

            enum eTraits
            {
                StrengthOfXuenTrait = 1094
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                const SpellInfo* l_CombotBeaker = sSpellMgr->GetSpellInfo(eSpells::ComboBreakerAura);
                const SpellInfo* l_StrengthOfXuen = sSpellMgr->GetSpellInfo(eSpells::StrengthOfXuen);
                if (!l_CombotBeaker || !l_StrengthOfXuen)
                    return;

                if (l_Caster->HasAura(eSpells::ComboBreakerAura))
                {
                    uint32 l_Chance = l_CombotBeaker->Effects[EFFECT_0].BasePoints;

                    if (l_Caster->HasAura(eSpells::StrengthOfXuen))
                    {
                        int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eTraits::StrengthOfXuenTrait);
                        l_Chance += l_StrengthOfXuen->Effects[EFFECT_0].BasePoints * l_Rank;
                    }

                    if (roll_chance_i(l_Chance))
                        l_Caster->CastSpell(l_Caster, eSpells::ComboBreakerBlackoutKick, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_combo_breaker_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_combo_breaker_SpellScript();
        }
};

/// last update : 6.2.3
/// Chi Brew - 115399
class spell_monk_chi_brew: public SpellScriptLoader
{
    public:
        spell_monk_chi_brew() : SpellScriptLoader("spell_monk_chi_brew") { }

        enum eSpells
        {
            SoothingMist        = 115175,
            ChiBrew             = 115399,
            MasteryBottledFury  = 115636
        };

        class spell_monk_chi_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_chi_brew_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::ChiBrew))
                    return false;
                return true;
            }

            void HandleOnPrepare()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->GetCurrentSpell(CURRENT_CHANNELED_SPELL) && l_Player->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->GetSpellInfo()->Id == eSpells::SoothingMist)
                    {
                        TriggerCastFlags l_Flags = TriggerCastFlags(GetSpell()->getTriggerCastFlags() | TRIGGERED_CAST_DIRECTLY);
                        GetSpell()->setTriggerCastFlags(l_Flags);
                    }
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_monk_chi_brew_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_chi_brew_SpellScript();
        }
};

/// Last Update 7.3.2 - Build 25549: Here is handled Damage or Healing, depending on target hit
/// Chi Wave (healing bolt) - 173545
class spell_monk_chi_wave_healing_bolt: public SpellScriptLoader
{
    public:
        spell_monk_chi_wave_healing_bolt() : SpellScriptLoader("spell_monk_chi_wave_healing_bolt") { }

        class spell_monk_chi_wave_healing_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_chi_wave_healing_bolt_SpellScript);

            enum eSpells
            {
                ChiWaveHeal     = 132463,
                ChiWaveDamage   = 132467
            };

            void HandleOnHit()
            {
                Unit* l_OGCaster = GetOriginalCaster();
                if (!l_OGCaster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (Player* l_Player = l_OGCaster->GetSpellModOwner())
                {
                    if (l_Player->IsValidAttackTarget(l_Target))
                        l_Player->CastSpell(l_Target, eSpells::ChiWaveDamage, true, nullptr, nullptr, l_OGCaster->GetGUID());
                    else
                        l_Player->CastSpell(l_Target, eSpells::ChiWaveHeal, true, nullptr, nullptr, l_OGCaster->GetGUID());
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_chi_wave_healing_bolt_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_chi_wave_healing_bolt_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 25996
/// Chi Wave (damage) - 132467 and Chi Wave (heal) - 132463
class spell_monk_chi_wave_bolt: public SpellScriptLoader
{
    public:
        spell_monk_chi_wave_bolt() : SpellScriptLoader("spell_monk_chi_wave_bolt") { }

        class spell_monk_chi_wave_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_chi_wave_bolt_SpellScript);

            enum eSpells
            {
                ChiWaveBolt         = 173545, /// I'm not actually sure if it's just the healing bolt, or the full bolt, that then either damages or heals. Couldnt find the visual for the "red bolt" that should be the damaging one. 7.3.2 - Build 25549
                ChiWaveTalentAura   = 115098
            };

            void HandleOnHit()
            {
                Unit* l_OriginalCaster = GetOriginalCaster();
                Unit* l_Target = GetHitUnit();

                if (l_OriginalCaster == nullptr || l_Target == nullptr)
                    return;

                uint8 l_Count = 0;
                std::list<Unit*> l_TargetList;
                std::vector<uint64> l_ValidTargets;

                if (AuraEffect* l_ChiWave = l_OriginalCaster->GetAuraEffect(eSpells::ChiWaveTalentAura, EFFECT_1))
                {
                    l_Count = l_ChiWave->GetAmount();

                    if (l_Count >= 7)
                    {
                        l_OriginalCaster->RemoveAura(eSpells::ChiWaveTalentAura);
                        return;
                    }
                    else
                    {
                        if (Aura* l_ChiAura = l_OriginalCaster->GetAura(eSpells::ChiWaveTalentAura))
                            l_ChiAura->SetDuration(l_ChiAura->GetMaxDuration());
                    }

                    l_Count++;
                    l_ChiWave->SetAmount(l_Count);
                }
                else
                    return;

                CellCoord l_P(JadeCore::ComputeCellCoord(l_Target->GetPositionX(), l_Target->GetPositionY()));
                Cell l_Cell(l_P);
                l_Cell.SetNoCreate();

                JadeCore::AnyUnitInObjectRangeCheck u_check(l_OriginalCaster, 20.0f);
                JadeCore::UnitListSearcher<JadeCore::AnyUnitInObjectRangeCheck> l_Searcher(l_OriginalCaster, l_TargetList, u_check);

                TypeContainerVisitor<JadeCore::UnitListSearcher<JadeCore::AnyUnitInObjectRangeCheck>, WorldTypeMapContainer> world_unit_searcher(l_Searcher);
                TypeContainerVisitor<JadeCore::UnitListSearcher<JadeCore::AnyUnitInObjectRangeCheck>, GridTypeMapContainer>  grid_unit_searcher(l_Searcher);

                l_Cell.Visit(l_P, world_unit_searcher, *l_OriginalCaster->GetMap(), *l_OriginalCaster, 20.0f);
                l_Cell.Visit(l_P, grid_unit_searcher, *l_OriginalCaster->GetMap(), *l_OriginalCaster, 20.0f);

                for (auto l_Itr : l_TargetList)
                {
                    if (!l_Itr->IsWithinLOSInMap(l_OriginalCaster))
                        continue;

                    if (l_Itr == l_Target)
                        continue;

                    l_ValidTargets.push_back(l_Itr->GetGUID());
                }

                if (l_ValidTargets.empty())
                {
                    l_OriginalCaster->RemoveAurasDueToSpell(eSpells::ChiWaveTalentAura);
                    return;
                }

                std::random_shuffle(l_ValidTargets.begin(), l_ValidTargets.end());

                if (Unit* l_NewTarget = sObjectAccessor->FindUnit(l_ValidTargets.front()))
                {
                    if (l_OriginalCaster->IsValidAttackTarget(l_NewTarget))
                        l_Target->CastSpell(l_NewTarget, eSpells::ChiWaveBolt, true, NULL, nullptr, l_OriginalCaster->GetGUID());
                    else
                    {
                        std::list<Unit*> l_AlliesList;
                        l_ValidTargets.push_back(l_OriginalCaster->GetGUID());

                        for (auto l_Itr : l_ValidTargets)
                        {
                            Unit* l_AllyTarget = sObjectAccessor->FindUnit(l_Itr);
                            if (!l_AllyTarget)
                                continue;

                            if (!l_OriginalCaster->IsValidAssistTarget(l_AllyTarget))
                                continue;

                            l_AlliesList.push_back(sObjectAccessor->FindUnit(l_Itr));
                        }

                        if (l_AlliesList.empty())
                            return;

                        l_AlliesList.sort(JadeCore::HealthPctOrderPred());

                        l_Target->CastSpell(l_AlliesList.front(), eSpells::ChiWaveBolt, true, NULL, nullptr, l_OriginalCaster->GetGUID());
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_chi_wave_bolt_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_chi_wave_bolt_SpellScript();
        }
};

// Chi Wave (talent) - 115098
class spell_monk_chi_wave : public SpellScriptLoader
{
    public:
        spell_monk_chi_wave() : SpellScriptLoader("spell_monk_chi_wave") { }

        class spell_monk_chi_wave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_chi_wave_SpellScript);

            uint64 m_TargetGUID;
            bool m_Done = false;

            enum eSpells
            {
                ChiWaveBolt = 132464,
                ChiWaveHeal = 132463,
                ChiWaveDamage = 132467,
                HitCombo = 196741,
                HitComboTal = 196740
            };

            enum eVisuals
            {
                Damage = 24491,
                Heal = 38379
            };

            bool Load() override
            {
                m_TargetGUID = 0;
                m_Done = false;
                GetCaster()->m_SpellHelper.GetUint32(eSpellHelpers::ChiWaveState) = 0;
                return true;
            }

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                Player* l_TargetPlayer = l_Target->ToPlayer();
                if (l_Player && l_TargetPlayer)
                    if (l_Player->IsPvP() && !l_TargetPlayer->IsPvP())
                        return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || !l_Caster->IsPlayer())
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                l_Player->m_SpellHelper.GetBool(eSpellHelpers::ChiWaveMastery) = (l_Player->GetLastCastedSpell() != m_scriptSpellId);
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ChiWaveCounter) = (GetSpellInfo()->Effects[EFFECT_0].BasePoints - 1);

                if (l_Player->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) != m_scriptSpellId)
                    if (l_Caster->HasAura(eSpells::HitComboTal))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::HitCombo, true);
                        l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::MasteryPrevSpellCasted) = m_scriptSpellId;
                        l_Player->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) = m_scriptSpellId;
                    }
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                m_TargetGUID = l_Target->GetGUID();

                if (l_Caster->HasAura(eSpells::HitComboTal))
                    if (l_Target->IsFriendlyTo(l_Caster))
                        l_Caster->CastSpell(l_Caster, eSpells::HitCombo, true);
            }

            void HandleApplyAura()
            {
                if (!m_TargetGUID || m_Done)
                    return;

                Unit* l_Caster = GetCaster();

                if (Player* l_Player = l_Caster->GetSpellModOwner())
                {
                    if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Player, m_TargetGUID))
                    {
                        if (l_Player->IsFriendlyTo(l_Target))
                        {
                            l_Caster->SendPlaySpellVisual(eVisuals::Heal, l_Target, 20.0f, l_Target->GetPosition(), false, false);
                            l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ChiWaveState) = 1;
                            l_Caster->CastSpell(l_Target, eSpells::ChiWaveHeal, true);
                        }
                        else
                        {
                            l_Caster->CombatStart(l_Target, true);
                            l_Caster->SendPlaySpellVisual(eVisuals::Damage, l_Target, 20.0f, l_Target->GetPosition(), false, false);
                            l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ChiWaveState) = 2;
                            l_Caster->CastSpell(l_Target, eSpells::ChiWaveDamage, true);
                        }
                        m_Done = true;
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_chi_wave_SpellScript::HandleCheckCast);
                OnCast += SpellCastFn(spell_monk_chi_wave_SpellScript::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_monk_chi_wave_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                AfterHit += SpellHitFn(spell_monk_chi_wave_SpellScript::HandleApplyAura);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_chi_wave_SpellScript();
        }
};

/// Last Update 7.0.3
/// Dampen Harm - 122278
class spell_monk_dampen_harm: public SpellScriptLoader
{
    public:
        spell_monk_dampen_harm() : SpellScriptLoader("spell_monk_dampen_harm") { }

        class spell_monk_dampen_harm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_dampen_harm_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* p_AuraEffect, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = GetTarget();

                uint32 l_Health = l_Target->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_0].BasePoints);

                if (p_DmgInfo.GetAmount() < l_Health)
                    return;

                /// The next 3 attacks within 45 sec that deal damage equal to 10% or more of your total health are reduced by half
                AuraEffect* l_AuraEffect = p_AuraEffect->GetBase()->GetEffect(EFFECT_1);

                if (l_AuraEffect == nullptr)
                    return;

                p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_AuraEffect->GetAmount());
                p_AuraEffect->GetBase()->DropCharge();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_dampen_harm_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_dampen_harm_AuraScript::CalculateAmount, EFFECT_3, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_monk_dampen_harm_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_monk_dampen_harm_AuraScript::Absorb, EFFECT_3, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_dampen_harm_AuraScript();
        }
};

/// Last Update 6.2.3
/// Called by Thunder Focus Tea - 116680
/// Item S12 4P - Mistweaver - 124487
class spell_monk_item_s12_4p_mistweaver: public SpellScriptLoader
{
    public:
        spell_monk_item_s12_4p_mistweaver() : SpellScriptLoader("spell_monk_item_s12_4p_mistweaver") { }

        class spell_monk_item_s12_4p_mistweaver_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_item_s12_4p_mistweaver_SpellScript);

            enum eSpells
            {
                T17Mistweaver4P     = 167717,
                SoothingMist        = 115175,
                S124ItemMistweaver  = 124487,
                ZenFocus            = 124488
            };

            void HandleOnPrepare()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->GetCurrentSpell(CURRENT_CHANNELED_SPELL) && l_Player->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->GetSpellInfo()->Id == eSpells::SoothingMist)
                    {
                        TriggerCastFlags l_Flags = TriggerCastFlags(GetSpell()->getTriggerCastFlags() | TRIGGERED_CAST_DIRECTLY);
                        GetSpell()->setTriggerCastFlags(l_Flags);
                    }
                }
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::S124ItemMistweaver))
                    l_Caster->CastSpell(l_Caster, eSpells::ZenFocus, true);

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::T17Mistweaver4P))
                {
                    l_Caster->RemoveAura(l_Aura);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_item_s12_4p_mistweaver_SpellScript::HandleOnHit);
                OnPrepare += SpellOnPrepareFn(spell_monk_item_s12_4p_mistweaver_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_item_s12_4p_mistweaver_SpellScript();
        }
};

struct auraData
{
    auraData(uint32 id, uint64 casterGUID) : m_id(id), m_casterGuid(casterGUID) {}
    uint32 m_id;
    uint64 m_casterGuid;
};

// Diffuse Magic - 122783
class spell_monk_diffuse_magic: public SpellScriptLoader
{
    public:
        spell_monk_diffuse_magic() : SpellScriptLoader("spell_monk_diffuse_magic") { }

        class spell_monk_diffuse_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_diffuse_magic_SpellScript);

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    std::set<auraData*> l_AuraListToRemove;
                    Unit::AuraApplicationMap l_AuraList = l_Player->GetAppliedAuras();
                    for (Unit::AuraApplicationMap::iterator l_Itr = l_AuraList.begin(); l_Itr != l_AuraList.end(); ++l_Itr)
                    {
                        Aura* l_Aura = l_Itr->second->GetBase();
                        if (!l_Aura)
                            continue;

                        Unit* l_Caster = l_Aura->GetCaster();
                        if (!l_Caster || l_Caster->GetGUID() == l_Player->GetGUID())
                            continue;

                        /// Don't transfer magical harmful effects of bosses
                        if (l_Caster->ToCreature() && l_Caster->ToCreature()->IsDungeonBoss())
                            continue;

                        if (!l_Caster->IsWithinDist(l_Player, 40.0f))
                            continue;

                        if (l_Aura->GetSpellInfo()->IsPositive())
                            continue;

                        if (!(l_Aura->GetSpellInfo()->GetSchoolMask() & SPELL_SCHOOL_MASK_MAGIC))
                            continue;

                        if (l_Aura->GetSpellInfo()->AttributesEx & SPELL_ATTR1_CANT_BE_REFLECTED)
                            continue;

                        l_Player->AddAura(l_Aura->GetSpellInfo()->Id, l_Caster);

                        if (Aura* l_TargetAura = l_Caster->GetAura(l_Aura->GetSpellInfo()->Id, l_Player->GetGUID()))
                        {
                            for (uint8 i = 0; i < l_Aura->GetEffectCount(); ++i)
                            {
                                if (l_TargetAura->GetEffect(i) && l_Aura->GetEffect(i))
                                {
                                    AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(l_Aura->GetSpellInfo()->Id, i);
                                    if (!l_AuraEffect)
                                        continue;

                                    int32 l_Damage = l_AuraEffect->GetAmount();

                                    // need to check auraEffect pointer here !!
                                    if (l_AuraEffect->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE ||
                                        l_AuraEffect->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE_PERCENT)
                                        l_Damage = l_Caster->SpellDamageBonusDone(l_Player, l_Aura->GetSpellInfo(), l_Damage, l_AuraEffect->GetEffIndex(), DOT, l_AuraEffect->GetBase()->GetStackAmount());

                                    l_TargetAura->GetEffect(i)->SetAmount(l_Damage);
                                }
                            }
                        }

                        l_AuraListToRemove.insert(new auraData(l_Aura->GetSpellInfo()->Id, l_Caster->GetGUID()));
                    }

                    for (std::set<auraData*>::iterator l_Itr = l_AuraListToRemove.begin(); l_Itr != l_AuraListToRemove.end(); ++l_Itr)
                    {
                        l_Player->RemoveAura((*l_Itr)->m_id, (*l_Itr)->m_casterGuid);
                        delete (*l_Itr);
                    }

                    l_AuraListToRemove.clear();
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_diffuse_magic_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_diffuse_magic_SpellScript();
        }
};

// Summon Black Ox Statue - 115315
class spell_monk_black_ox_statue: public SpellScriptLoader
{
    public:
        spell_monk_black_ox_statue() : SpellScriptLoader("spell_monk_black_ox_statue") { }

        class spell_monk_black_ox_statue_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_black_ox_statue_SpellScript);

            enum eEntry
            {
                BlackOxStatue = 61146
            };

            void HandleSummon(SpellEffIndex p_EffectIndex)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    PreventHitDefaultEffect(p_EffectIndex);

                    const SpellInfo* l_Spell = GetSpellInfo();
                    std::list<Creature*> l_TempList;
                    std::list<Creature*> l_BlackOxList;

                    l_Player->GetCreatureListWithEntryInGrid(l_TempList, eEntry::BlackOxStatue, 200.0f);

                    for (auto l_Itr : l_TempList)
                        l_BlackOxList.push_back(l_Itr);

                    // Remove other players jade statue
                    for (std::list<Creature*>::iterator l_Itr = l_TempList.begin(); l_Itr != l_TempList.end(); ++l_Itr)
                    {
                        Unit* l_Owner = (*l_Itr)->GetOwner();
                        if (l_Owner && l_Owner == l_Player && (*l_Itr)->isSummon())
                            continue;

                        l_BlackOxList.remove((*l_Itr));
                    }

                    // 1 statue max
                    if ((int32)l_BlackOxList.size() >= l_Spell->Effects[p_EffectIndex].BasePoints)
                        l_BlackOxList.back()->ToTempSummon()->UnSummon();

                    Position l_Pos;
                    GetExplTargetDest()->GetPosition(&l_Pos);
                    TempSummon* l_Summon = l_Player->SummonCreature(l_Spell->Effects[p_EffectIndex].MiscValue, l_Pos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, l_Spell->GetDuration());
                    if (!l_Summon)
                        return;

                    l_Summon->SetGuidValue(UNIT_FIELD_SUMMONED_BY, l_Player->GetGUID());
                    l_Summon->setFaction(l_Player->getFaction());
                    l_Summon->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, GetSpellInfo()->Id);
                    l_Summon->SetMaxHealth(l_Player->CountPctFromMaxHealth(50));
                    l_Summon->SetHealth(l_Summon->GetMaxHealth());
                    l_Summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
                    l_Summon->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, true);
                    l_Summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
                    l_Player->SendTempSummonUITimer(l_Summon);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_monk_black_ox_statue_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_black_ox_statue_SpellScript();
        }

};

// Zen Flight - 125883
class spell_monk_zen_flight_check: public SpellScriptLoader
{
    public:
        spell_monk_zen_flight_check() : SpellScriptLoader("spell_monk_zen_flight_check") { }

        class spell_monk_zen_flight_check_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_zen_flight_check_SpellScript);

            SpellCastResult CheckTarget()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->GetMap()->IsBattlegroundOrArena())
                        return SPELL_FAILED_NOT_IN_BATTLEGROUND;

                    // In Kalimdor or Eastern Kingdom with Expert Riding
                    if (!l_Player->HasSpell(34090) && (l_Player->GetMapId() == 1 || l_Player->GetMapId() == 0))
                        return SPELL_FAILED_NOT_HERE;

                    // In Pandaria with Expert Riding
                    if (!l_Player->HasSpell(34090) && (l_Player->GetMapId() == 870))
                        return SPELL_FAILED_NOT_HERE;

                    /// Disable Zen Flight in Gurubashi arena
                    if (l_Player->GetAreaId() == 2177 && sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                        return SPELL_FAILED_NOT_HERE;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_zen_flight_check_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_zen_flight_check_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Stance of the Fierce Tiger - 103985
class spell_monk_stance_of_the_fierce_tiger : public SpellScriptLoader
{
    public:
        spell_monk_stance_of_the_fierce_tiger() : SpellScriptLoader("spell_monk_stance_of_the_fierce_tiger") { }

        class spell_monk_stance_of_the_fierce_tiger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_stance_of_the_fierce_tiger_SpellScript);

            SpellCastResult CheckSpec() /// Mistweaver had this spell on Cross, which made them insanely powerfull (500 less GCD + Way of the Crane honor talent = RIP
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player * l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_stance_of_the_fierce_tiger_SpellScript::CheckSpec);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_stance_of_the_fierce_tiger_SpellScript();
        }

        class spell_monk_stance_of_the_fierce_tiger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_stance_of_the_fierce_tiger_AuraScript);

            void HandleAfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player * l_Player = l_Caster->ToPlayer();
                if (l_Player && l_Player->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                {
                    uint32 l_SpellId = GetSpellInfo()->Id;
                    l_Player->AddDelayedEvent([l_Player, l_SpellId]() -> void
                    {
                        l_Player->RemoveAura(l_SpellId);
                    }, 50);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_stance_of_the_fierce_tiger_AuraScript::HandleAfterApply, EFFECT_5, SPELL_AURA_MOD_INCREASE_ENERGY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };
};

/// Tiger Palm - 100780
class spell_monk_tiger_palm: public SpellScriptLoader
{
    public:
        spell_monk_tiger_palm() : SpellScriptLoader("spell_monk_tiger_palm") { }

        class spell_monk_tiger_palm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_tiger_palm_SpellScript);

            enum eSpells
            {
                PowerStrikesAura        = 129914,
                FortifyingBrew          = 115203,
                IronskinBrew            = 115308,
                PurifyingBrew           = 119582,
                BlackOxBrew             = 115399,
                T19Brewmaster4P         = 211417
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::PowerStrikesAura))
                {
                    l_Caster->ModifyPower(POWER_CHI, 1);
                    l_Caster->RemoveAura(eSpells::PowerStrikesAura);
                }

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_MONK_BREWMASTER)
                {
                    SpellInfo const* l_SpellInfo = GetSpellInfo();
                    if (!l_SpellInfo)
                        return;

                    uint32 l_Amount = l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS;
                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Brewmaster4P, EFFECT_0))
                        l_Amount += l_AuraEffect->GetAmount() * IN_MILLISECONDS;
                    l_Player->ReduceSpellCooldown(eSpells::FortifyingBrew, l_Amount);
                    l_Player->ReduceSpellCooldown(eSpells::BlackOxBrew, l_Amount);

                    l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::IronskinBrew);
                    SpellCategoryEntry const* l_CommonCategoryEntry = l_SpellInfo->ChargeCategoryEntry;
                    if (!l_SpellInfo || !l_CommonCategoryEntry)
                        return;

                    l_Player->ReduceChargeCooldown(l_CommonCategoryEntry, (uint64)l_Amount);
                }
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (l_Caster == nullptr || l_Target == nullptr)
                    return;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_tiger_palm_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_monk_tiger_palm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_monk_tiger_palm_SpellScript();
        }
};

/// Crackling Jade Lightning - 117952
class spell_monk_crackling_jade_lightning: public SpellScriptLoader
{
    public:
        spell_monk_crackling_jade_lightning() : SpellScriptLoader("spell_monk_crackling_jade_lightning") { }

        class spell_monk_crackling_jade_lightning_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_crackling_jade_lightning_AuraScript);

            enum eSpells
            {
                JadeLightningEnergize   = 123333,
                CracklingJadeShockBump  = 117962,
                TheEmperorCapacitor     = 235054
            };

            enum eEntry
            {
                SEFfireSpirit           = 69791,
                SEFearthSpirit          = 69792
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->GetActiveSpecializationID() == SPEC_MONK_MISTWEAVER)
                        l_Player->CastSpell(l_Player, eSpells::JadeLightningEnergize, true);
                }

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (GetTarget()->HasAura(p_AurEff->GetSpellInfo()->Id, l_Player->GetGUID()))
                    {
                        const SpellInfo *l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CracklingJadeShockBump);

                        if (!l_Player->HasSpellCooldown(eSpells::CracklingJadeShockBump) && l_SpellInfo != nullptr)
                        {
                            l_Player->CastSpell(GetTarget(), eSpells::CracklingJadeShockBump, true);
                            l_Player->AddSpellCooldown(eSpells::CracklingJadeShockBump, 0, l_SpellInfo->RecoveryTime);
                        }
                    }
                }
            }

            void CalculateMaxDuration(int32& p_Duration)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->GetEntry() == eEntry::SEFearthSpirit || l_Caster->GetEntry() == eEntry::SEFfireSpirit) ///< Storm, Earth and Fire Spirits should cast Crackling Jade Lightning 20% faster - 7.3.5 - Build 26365
                    p_Duration *= 0.8f;
            }

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Modes*/)
            {
                if (!GetCaster())
                    return;

                if (Aura* l_EmperorCapacitorAura = GetCaster()->GetAura(eSpells::TheEmperorCapacitor))
                    l_EmperorCapacitorAura->Remove();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_crackling_jade_lightning_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_monk_crackling_jade_lightning_AuraScript::CalculateMaxDuration);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_crackling_jade_lightning_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_crackling_jade_lightning_AuraScript();
        }

        class spell_monk_crackling_jade_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_crackling_jade_lightning_SpellScript);

            enum eSpells
            {
                HitComboTalent  = 196740,
                HitComboAura    = 196741
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Monk = l_Caster->GetSpellModOwner();
                if (!l_Monk)
                    return;

                if (l_Monk->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) == m_scriptSpellId && l_Caster == l_Monk)
                {
                    l_Caster->RemoveAura(eSpells::HitComboAura);
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::CJLBenefitFromMastery) = false;
                    return;
                }
                else if (l_Monk->HasAura(eSpells::HitComboTalent))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::HitComboAura, true);
                }

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) = m_scriptSpellId;
                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::CJLBenefitFromMastery) = true;
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_monk_crackling_jade_lightning_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_crackling_jade_lightning_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Touch of Karma - 122470
class spell_monk_touch_of_karma: public SpellScriptLoader
{
    public:
        spell_monk_touch_of_karma() : SpellScriptLoader("spell_monk_touch_of_karma") { }

        class spell_monk_touch_of_karma_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_touch_of_karma_AuraScript);

            uint64 m_TargetGUID;

            enum eSpells
            {
                WoDPvPWindwalker2PBonus    = 180743,
                TouchOfKarmaRedirectDamage = 124280,
                TouchOfKarmaAura           = 122470,
                CenedrilReflectorOfHatred  = 208842
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (l_Target != nullptr && l_Caster != nullptr)
                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::TouchOfKarmaTarget) = l_Target->GetGUID();
            }

            int32 m_DamageCap = 0;

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WoDPvPWindwalker2PBonus);

                if (l_Caster == nullptr || l_SpellInfo == nullptr)
                    return;

                int32 l_HealthPct = GetSpellInfo()->Effects[EFFECT_2].BasePoints;

                if (l_Caster->HasAura(eSpells::WoDPvPWindwalker2PBonus))
                    l_HealthPct += l_SpellInfo->Effects[EFFECT_0].BasePoints;

                p_Amount    = l_Caster->CountPctFromMaxHealth(l_HealthPct);
                m_DamageCap = p_Amount;
            }

            void OnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Owner  = GetCaster();
                Unit* l_Caster = p_DmgInfo.GetTarget();

                SpellInfo const* l_ProcSpellInfo = p_DmgInfo.GetSpellInfo();
                if (l_ProcSpellInfo && l_ProcSpellInfo->Id == eSpells::TouchOfKarmaRedirectDamage)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                if (l_Caster == nullptr || l_Owner == nullptr)
                    return;

                if (l_Owner->GetGUID() != l_Caster->GetGUID())
                    return;

                Unit* l_Target = nullptr;
                uint64& l_TargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::TouchOfKarmaTarget);

                if (Unit* l_Target = sObjectAccessor->FindUnit(l_TargetGUID))
                {
                    int32 l_Damage = p_DmgInfo.GetAmount();

                    if (l_Damage > (int32)p_AbsorbAmount)
                        l_Damage = p_AbsorbAmount;

                    p_AbsorbAmount = l_Damage;

                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::CenedrilReflectorOfHatred, EFFECT_0))
                        AddPct(l_Damage, l_AuraEffect->GetAmount());

                    /// If target has already this aura (old damage redirected)
                    if (AuraEffect* l_PreviousAura = l_Target->GetAuraEffect(eSpells::TouchOfKarmaRedirectDamage, EFFECT_0, l_Caster->GetGUID()))
                    {
                        l_Damage += l_PreviousAura->GetAmount() * (l_PreviousAura->GetTotalTicks() - l_PreviousAura->GetTickNumber());
                        l_Target->RemoveAura(eSpells::TouchOfKarmaRedirectDamage, l_Caster->GetGUID());
                    }

                    l_Damage = std::min(l_Damage, m_DamageCap);

                    uint32 l_TicksCount = 1;
                    if (SpellInfo const* l_DamageSpellInfo = sSpellMgr->GetSpellInfo(eSpells::TouchOfKarmaRedirectDamage))
                        l_TicksCount = l_DamageSpellInfo->GetMaxDuration() / l_DamageSpellInfo->Effects[EFFECT_0].Amplitude;

                    if (l_TicksCount)
                        l_Damage /= l_TicksCount;

                    l_Caster->CastCustomSpell(eSpells::TouchOfKarmaRedirectDamage, SPELLVALUE_BASE_POINT0, l_Damage, l_Target);
                }
                else
                {
                    p_AbsorbAmount = 0;
                    l_Caster->RemoveAura(eSpells::TouchOfKarmaAura, l_Caster->GetGUID());
                }
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(GetId());
                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::TouchOfKarmaTarget) = 0;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_touch_of_karma_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_touch_of_karma_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_monk_touch_of_karma_AuraScript::OnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_touch_of_karma_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_touch_of_karma_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Vivify - 116670 - Renewing Mist - 115151 - Effuse - 116694 - Envelopping Mist - 124682 - Essence Font - 191837 - Effuse AMA - 227344 - Envelopping Mist AMA - 227345
/// Called for Thunder Focus Tea - 116680
class spell_monk_thunder_focus_tea: public SpellScriptLoader
{
    public:
        spell_monk_thunder_focus_tea() : SpellScriptLoader("spell_monk_thunder_focus_tea") { }

        enum eSpells
        {
            ThunderFocusTeaWalk = 197218,
            RenewingMistHot     = 119611,
            ThunderFocusTea     = 116680,
            Vivify              = 116670,
            RenewingMist        = 115151,
            Effuse              = 116694,
            EnveloppingMist     = 124682,
            EssenceFont         = 191837,

            RenewingMistAura    = 216509,
            EffuseAura          = 216992,
            EnveloppingMistAura = 216995,
            EssenceFontAura     = 217000,
            VivifyAura          = 217006,

            EffuseAMA           = 227344,
            EnveloppingMistAMA  = 227345,
            SurgeOfMist         = 216074,
            SoothingMistAMA     = 209525
        };

        class spell_monk_thunder_focus_tea_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_thunder_focus_tea_SpellScript);

            std::vector<eSpells> m_Spells =
            {
                eSpells::VivifyAura,
                eSpells::RenewingMistAura,
                eSpells::EnveloppingMistAura,
                eSpells::EssenceFontAura
            };

            void HandleOnTakePower(Powers /*p_PowerType*/, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::ThunderFocusTea))
                    return;

                p_PowerCost = 0;

                RemoveThunderFocusAura(l_Caster);
            }

            void HandleRemoval()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::ThunderFocusTea))
                    return;

                for (eSpells l_Spell : m_Spells)
                    if (l_Caster->HasAura(l_Spell))
                    {
                        l_Caster->RemoveAura(l_Spell);
                        return;
                    }

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id == eSpells::EnveloppingMist && l_Caster->HasAura(eSpells::SurgeOfMist)) || (l_SpellInfo->Id == eSpells::EnveloppingMistAMA && l_Caster->HasAura(eSpells::SoothingMistAMA)))
                    return;

                RemoveThunderFocusAura(l_Caster);
            }

            void HandleBeforeHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::ThunderFocusTea))
                    return;

                if (l_Caster->HasAura(eSpells::EffuseAura))
                    return;

                RemoveThunderFocusAura(l_Caster);
            }
            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::ThunderFocusTea) || !l_Caster->IsPlayer())
                    return;

                l_Caster->ToPlayer()->RemoveSpellCooldown(eSpells::RenewingMist);
                RemoveThunderFocusAura(l_Caster);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::Vivify:
                        OnTakePowers += SpellTakePowersFn(spell_monk_thunder_focus_tea_SpellScript::HandleOnTakePower);
                        break;
                    case eSpells::Effuse:
                    case eSpells::EffuseAMA:
                        BeforeHit += SpellHitFn(spell_monk_thunder_focus_tea_SpellScript::HandleBeforeHit);
                        break;
                    case eSpells::EnveloppingMist:
                    case eSpells::EssenceFont:
                    case eSpells::EnveloppingMistAMA:
                        AfterHit += SpellHitFn(spell_monk_thunder_focus_tea_SpellScript::HandleRemoval);
                        break;
                    case eSpells::RenewingMist:
                        AfterCast += SpellCastFn(spell_monk_thunder_focus_tea_SpellScript::HandleCast);
                    default:
                        break;
                }
            }

        private:

            void RemoveThunderFocusAura(Unit* l_Owner)
            {
                l_Owner->RemoveAuraFromStack(eSpells::ThunderFocusTea);
                if (GetSpellInfo()->Id != eSpells::EssenceFont && !l_Owner->HasAura(eSpells::ThunderFocusTea))
                    l_Owner->RemoveAura(eSpells::ThunderFocusTeaWalk);
            }
        };

        class spell_monk_thunder_focus_tea_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_thunder_focus_tea_AuraScript);

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Owner = GetUnitOwner())
                {
                    if (!l_Owner->HasAura(eSpells::ThunderFocusTea))
                        l_Owner->RemoveAura(eSpells::ThunderFocusTeaWalk);
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::EssenceFont:
                        AfterEffectRemove += AuraEffectApplyFn(spell_monk_thunder_focus_tea_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                        break;
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_thunder_focus_tea_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_thunder_focus_tea_AuraScript();
        }
};

/// Update Legion 7.3.5 build 26365
/// Summon Jade Serpent Statue - 115313
class spell_monk_jade_serpent_statue : public SpellScriptLoader
{
public:
    spell_monk_jade_serpent_statue() : SpellScriptLoader("spell_monk_jade_serpent_statue") { }

    class spell_monk_jade_serpent_statue_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_jade_serpent_statue_SpellScript);

        enum eEntry
        {
            JadeSerpentStatue = 60849
        };

        void HandleSummon(SpellEffIndex p_EffectIndex)
        {
            Player* l_Player = GetCaster()->ToPlayer();
            if (!l_Player)
                return;

            PreventHitDefaultEffect(p_EffectIndex);

            const SpellInfo* l_Spell = GetSpellInfo();
            std::list<Creature*> l_TempList;
            std::list<Creature*> l_JadeSerpentList;

            l_Player->GetCreatureListWithEntryInGrid(l_TempList, eEntry::JadeSerpentStatue, 200.0f);
            l_Player->GetCreatureListWithEntryInGrid(l_JadeSerpentList, eEntry::JadeSerpentStatue, 200.0f);

            /// Remove other players jade statue
            for (std::list<Creature*>::iterator l_Itr = l_TempList.begin(); l_Itr != l_TempList.end(); ++l_Itr)
            {
                Unit* l_Owner = (*l_Itr)->GetOwner();
                if (l_Owner && l_Owner == l_Player && (*l_Itr)->isSummon())
                    continue;

                l_JadeSerpentList.remove((*l_Itr));
            }

            /// 1 statue max
            if ((int32)l_JadeSerpentList.size() >= l_Spell->Effects[p_EffectIndex].BasePoints)
                l_JadeSerpentList.back()->ToTempSummon()->UnSummon();

            Position l_Pos;
            GetExplTargetDest()->GetPosition(&l_Pos);
            TempSummon* l_Summon = l_Player->SummonCreature(l_Spell->Effects[p_EffectIndex].MiscValue, l_Pos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, l_Spell->GetDuration());
            if (!l_Summon)
                return;

            l_Summon->SetGuidValue(UNIT_FIELD_SUMMONED_BY, l_Player->GetGUID());
            l_Summon->setFaction(l_Player->getFaction());
            l_Summon->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, GetSpellInfo()->Id);
            l_Summon->SetMaxHealth(l_Player->CountPctFromMaxHealth(50));
            l_Summon->SetHealth(l_Summon->GetMaxHealth());
            l_Player->SendTempSummonUITimer(l_Summon);
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_monk_jade_serpent_statue_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_monk_jade_serpent_statue_SpellScript();
    }
};

/// Update Legion 7.3.5 build 26365
/// Enveloping Mist - 124682 - 227345
class spell_monk_enveloping_mist: public SpellScriptLoader
{
    public:
        spell_monk_enveloping_mist() : SpellScriptLoader("spell_monk_enveloping_mist") { }

        class spell_monk_enveloping_mist_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_enveloping_mist_SpellScript);

            enum eSpells
            {
                MonkWoDPvPMistweaver2PBonus       = 170799,
                MonkWoDPvPMistweaver2PBonusEffect = 170808,
                LifecyclesAura                    = 197915,
                LifecyclesVivify                  = 197916,
                EnvelopingMistHealingBonusMod     = 231605
            };

            void HandleBeforeHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::EnvelopingMistHealingBonusMod, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::MonkWoDPvPMistweaver2PBonus))
                    l_Caster->CastSpell(l_Target, eSpells::MonkWoDPvPMistweaver2PBonusEffect, true);

                if (l_Caster->HasAura(eSpells::LifecyclesAura))
                    l_Caster->CastSpell(l_Caster, eSpells::LifecyclesVivify, true);
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_monk_enveloping_mist_SpellScript::HandleBeforeHit);
                AfterCast += SpellCastFn(spell_monk_enveloping_mist_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_enveloping_mist_SpellScript();
        }

        class spell_monk_enveloping_mist_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_enveloping_mist_AuraScript);

            bool m_FullLife = false;

            enum eSpells
            {
                OvydsWinterWrap     = 217634,
                OvydsWinterWrapArea = 217647,
                DomeOfMistAura      = 202577,
                DomeOfMist          = 205655
            };

            int32 l_RemainingHeal = 0;

            void HandleApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetUnitOwner();

                m_FullLife = l_Target && l_Target->IsFullHealth();

                l_RemainingHeal = CalcRemainingHeal(p_AurEff);
            }

            void HandlePeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Target = GetUnitOwner();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::OvydsWinterWrap);
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                if (!m_FullLife)
                    m_FullLife = l_Target->IsFullHealth();

                if (m_FullLife && roll_chance_i(l_SpellInfo->ProcChance) && l_Caster->HasAura(eSpells::OvydsWinterWrap))
                    l_Caster->CastSpell(l_Target, eSpells::OvydsWinterWrapArea, true);

                l_RemainingHeal = CalcRemainingHeal(p_AurEff);
            }

            void HandleDispel(DispelInfo* p_DispelInfo)
            {
                Unit* l_Caster = GetCaster();
                WorldObject* l_Target = GetOwner();
                if (!l_Caster || !l_Caster->HasAura(eSpells::DomeOfMistAura) || !l_Target)
                    return;

                if (l_RemainingHeal > 0)
                    l_Caster->CastCustomSpell(static_cast<Unit*>(l_Target), eSpells::DomeOfMist, &l_RemainingHeal, nullptr, nullptr, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_monk_enveloping_mist_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_monk_enveloping_mist_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
                AfterDispel += AuraDispelFn(spell_monk_enveloping_mist_AuraScript::HandleDispel);
            }

        private:

            int32 CalcRemainingHeal(AuraEffect const* p_AurEff)
            {
                return p_AurEff->GetAmount() * (p_AurEff->GetTotalTicks() - p_AurEff->GetTickNumber());
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_enveloping_mist_AuraScript();
        }
};

/// last update : 7.0.3
/// Renewing Mist (heal) - 115151
class spell_monk_renewing_mist_hot: public SpellScriptLoader
{
    public:
        spell_monk_renewing_mist_hot() : SpellScriptLoader("spell_monk_renewing_mist_hot") { }

        class spell_monk_renewing_mist_hot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_renewing_mist_hot_SpellScript);

            enum eSpells
            {
                RenewingMist    = 115151,
                PoolOfMists     = 173841,
                ThunderFocusTea = 116680,
                RenewingMistHot = 119611,
                T21Mistweaver4PBonus = 251826,
                ChiBolt         = 253581
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::T21Mistweaver4PBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::ChiBolt, true);

                l_Caster->CastSpell(l_Target, eSpells::RenewingMistHot, true);

                if (Aura* l_ThunderFocusTea = l_Caster->GetAura(eSpells::ThunderFocusTea, l_Caster->GetGUID()))
                {
                    if (Aura* l_RenewingMistHot = l_Target->GetAura(eSpells::RenewingMistHot, l_Caster->GetGUID()))
                    {
                        auto l_RenewingMistHotEffect = l_RenewingMistHot->GetEffect(EFFECT_0);
                        auto l_ThunderFocusTeaEffect = l_ThunderFocusTea->GetEffect(EFFECT_1);

                        if (l_RenewingMistHotEffect && l_ThunderFocusTeaEffect)
                            l_RenewingMistHotEffect->SetAmount(l_RenewingMistHotEffect->GetAmount() + l_ThunderFocusTeaEffect->GetAmount());
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_renewing_mist_hot_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_renewing_mist_hot_SpellScript();
        }
};

/// Renewing Mist - 119611
/// Updated as of 7.0.3 - 22522
class spell_monk_renewing_mist: public SpellScriptLoader
{
    public:
        spell_monk_renewing_mist() : SpellScriptLoader("spell_monk_renewing_mist") { }

        class spell_monk_renewing_mist_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_renewing_mist_AuraScript);

            enum eSpells
            {
                SPELL_MONK_RENEWING_MIST_HOT    = 119611,
                SPELL_MONK_RENEWING_MIST        = 115151,
                SPELL_MONK_UPLIFTING_TRANCE     = 197206,
                DancingMists                    = 199573,
                T19Mistweaver2p                 = 211418
            };

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::SPELL_MONK_RENEWING_MIST_HOT) || !sSpellMgr->GetSpellInfo(eSpells::SPELL_MONK_RENEWING_MIST))
                    return false;

                return true;
            }

            Unit* SelectNextFittingTarget(Unit* p_Caster, Unit* p_Target)
            {
                float l_Radius = 20.0f;

                /// Get friendly unit on range
                std::list<Unit*> l_FriendlyUnitList;
                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(p_Target, p_Target, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_Target, l_FriendlyUnitList, l_Check);
                p_Target->VisitNearbyObject(l_Radius, l_Searcher);

                /// Remove friendly unit with already renewing mist apply
                l_FriendlyUnitList.remove_if(JadeCore::UnitAuraCheck(true, GetSpellInfo()->Id));

                l_FriendlyUnitList.remove_if([this, p_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (!p_Caster->IsValidAssistTarget(p_Object->ToUnit()))
                        return true;

                    return false;
                });

                /// Sort friendly unit by pourcentage of health and get the most injured
                if (l_FriendlyUnitList.size() > 0)
                {
                    l_FriendlyUnitList.sort(JadeCore::HealthPctOrderPred());
                    l_FriendlyUnitList.resize(1);
                    Unit* unitToHeal = l_FriendlyUnitList.front();
                    if (unitToHeal->IsFullHealth())
                        return nullptr;
                    return unitToHeal;
                }

                return nullptr;
            }

            void AfterTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster == nullptr || l_Target == nullptr || !p_AuraEffect || !p_AuraEffect->GetBase())
                    return;

                /// Each time Renewing Mist heals, it has a 4% chance to increase the healing of your next Vivify by 40%
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SPELL_MONK_RENEWING_MIST))
                {
                    int32 l_Chance = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Mistweaver2p, EFFECT_0))
                        l_Chance += l_AuraEffect->GetAmount();
                    if (roll_chance_i(l_Chance))
                        l_Caster->CastSpell(l_Caster, eSpells::SPELL_MONK_UPLIFTING_TRANCE, true);
                }

                if (l_Target->IsFullHealth())
                {
                    /// we've healed our target, try to move to another
                    if (Spread(l_Caster, l_Target))
                        Remove();
                }
            }

            bool Spread(Unit* p_Caster, Unit* p_Target)
            {
                if (AuraEffect* l_AuraEffect = p_Target->GetAuraEffect(GetId(), EFFECT_0, p_Caster->GetGUID()))
                {
                    if (Unit* l_NewTarget = SelectNextFittingTarget(p_Caster, p_Target))
                    {
                        if (Aura* l_NewAura = p_Caster->AddAura(GetId(), l_NewTarget))
                        {
                            l_NewAura->SetDuration(GetDuration());

                            /// port healing
                            if (AuraEffect* const l_NewAurEffect = l_NewAura->GetEffect(EFFECT_0))
                            {
                                l_NewAurEffect->SetAmount(l_AuraEffect->GetAmount());
                                l_NewAurEffect->SetPeriodicTimer(l_AuraEffect->GetPeriodicTimer());

                                return true;
                            }
                        }
                    }
                }

                return false;
            }

            void HandleAfterApply(AuraEffect const* /*p_AuraEffecty*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::DancingMists))
                {
                    SpellInfo const* l_DancingMistsInfo = sSpellMgr->GetSpellInfo(eSpells::DancingMists);

                    if (l_DancingMistsInfo && roll_chance_i(l_DancingMistsInfo->Effects[EFFECT_0].BasePoints))
                        Spread(l_Caster, l_Target);
                }

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RenewingMist].insert(l_Target->GetGUID());
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffecty*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RenewingMist].erase(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_monk_renewing_mist_AuraScript::AfterTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
                AfterEffectApply += AuraEffectApplyFn(spell_monk_renewing_mist_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_renewing_mist_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_renewing_mist_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Healing Elixirs - 122280
class spell_monk_healing_elixirs: public SpellScriptLoader
{
    public:
        spell_monk_healing_elixirs() : SpellScriptLoader("spell_monk_healing_elixirs") { }

        class spell_monk_healing_elixirs_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_healing_elixirs_AuraScript);

            enum eSpells
            {
                HealingElixirsHeal = 122281
            };

            void OnUpdate(uint32 /*diff*/)
            {
                if (!GetCaster())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->isAlive())
                    return;

                if (l_Player->IsMounted())
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HealingElixirsHeal);
                if (!l_SpellInfo)
                    return;

                if ((int32)l_Player->GetHealthPct() < GetSpellInfo()->Effects[EFFECT_0].BasePoints)
                    if (l_Player->HasCharge(l_SpellInfo->ChargeCategoryEntry))
                        l_Player->CastSpell(l_Player, eSpells::HealingElixirsHeal, false);
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_monk_healing_elixirs_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_healing_elixirs_AuraScript();
        }
};

// Spear Hand Strike - 116705
class spell_monk_spear_hand_strike: public SpellScriptLoader
{
    public:
        spell_monk_spear_hand_strike() : SpellScriptLoader("spell_monk_spear_hand_strike") { }

        class spell_monk_spear_hand_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_spear_hand_strike_SpellScript);

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Target->isInFront(l_Player))
                        {
                            l_Player->AddSpellCooldown(116705, 0, 15 * IN_MILLISECONDS);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_spear_hand_strike_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_spear_hand_strike_SpellScript();
        }
};

// Tiger's Lust - 116841
class spell_monk_tigers_lust: public SpellScriptLoader
{
    public:
        spell_monk_tigers_lust() : SpellScriptLoader("spell_monk_tigers_lust") { }

        class spell_monk_tigers_lust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_tigers_lust_SpellScript);

            enum eSpells
            {
                FlyingSerpentKickNew = 115057
            };

            bool Validate(SpellInfo const* /*spellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::FlyingSerpentKickNew))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                    if (Unit* l_target = GetHitUnit())
                        l_target->RemoveMovementImpairingAuras();
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_tigers_lust_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_tigers_lust_SpellScript();
        }
};

/// Flying Serpent Kick - 115057
class spell_monk_flying_serpent_kick: public SpellScriptLoader
{
    public:
        spell_monk_flying_serpent_kick() : SpellScriptLoader("spell_monk_flying_serpent_kick") { }

        class spell_monk_flying_serpent_kick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_flying_serpent_kick_SpellScript);

            enum eSpells
            {
                FlyingSerpentKickNew = 115057,
                FlyingSerpentKick    = 101545,
                ItemPvPGlovesBonus   = 124489
            };

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::FlyingSerpentKickNew))
                    return false;
                return true;
            }

            void HandleBeforeCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (l_Player->HasAura(eSpells::FlyingSerpentKick))
                            l_Player->RemoveAura(eSpells::FlyingSerpentKick);

                        if (l_Caster->HasAura(eSpells::ItemPvPGlovesBonus))
                            l_Caster->RemoveAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
                    }
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_monk_flying_serpent_kick_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_flying_serpent_kick_SpellScript();
        }

        class spell_monk_flying_serpent_kick_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_flying_serpent_kick_AuraScript);

            enum eSpells
            {
                FlyingSerpentKickAoe = 123586
            };

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), eSpells::FlyingSerpentKickAoe, true);
                Remove();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_flying_serpent_kick_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_flying_serpent_kick_AuraScript();
        }
};

/// Flying Serpent Kick - 101545
class spell_monk_tiger_style: public SpellScriptLoader
{
    public:
        spell_monk_tiger_style() : SpellScriptLoader("spell_monk_tiger_style") { }

        class spell_monk_tiger_style_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_tiger_style_SpellScript);

            enum eSpells
            {
                TigerStyle  = 206743,
                HitCombo    = 196741,
                HitComboTal = 196740
            };

            void HandleApply(SpellEffIndex p_Index)
            {
                Unit* l_Owner = GetHitUnit();

                if (l_Owner && l_Owner->HasAura(eSpells::TigerStyle))
                    PreventHitEffect(p_Index);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster && l_Caster->HasAura(eSpells::HitComboTal) && l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) != m_scriptSpellId)
                    l_Caster->CastSpell(l_Caster, eSpells::HitCombo, true);

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) = m_scriptSpellId;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_tiger_style_SpellScript::HandleApply, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                AfterCast += SpellCastFn(spell_monk_tiger_style_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_tiger_style_SpellScript();
        }
};

/// Chi Torpedo - 115008
class spell_monk_chi_torpedo: public SpellScriptLoader
{
    public:
        spell_monk_chi_torpedo() : SpellScriptLoader("spell_monk_chi_torpedo") { }

        class spell_monk_chi_torpedo_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_chi_torpedo_SpellScript);

            enum eSpells
            {
                MonkWoDPvPBrewmaster2PBonus = 165691,
                MonkWoDPvPBrewmasterAura    = 165692,
                ItemPvPGlovesBonus          = 124489
            };

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (l_Player->HasAura(eSpells::ItemPvPGlovesBonus))
                    l_Player->RemoveAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);

                if (l_Player->HasAura(eSpells::MonkWoDPvPBrewmaster2PBonus))
                    l_Player->CastSpell(l_Player, eSpells::MonkWoDPvPBrewmasterAura, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_chi_torpedo_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_chi_torpedo_SpellScript();
        }
};

/// Last Update 7.0.3
/// Purifying Brew - 119582
class spell_monk_purifying_brew: public SpellScriptLoader
{
    public:
        spell_monk_purifying_brew() : SpellScriptLoader("spell_monk_purifying_brew") { }

        class spell_monk_purifying_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_purifying_brew_SpellScript);

            enum eSpells
            {
                T17Brewmaster4P             = 165352,
                ElusiveBrew                 = 128939,
                ElusiveDance                = 196738,
                ElusiveDanceAura            = 196739,
                LightStagger                = 124275,
                ModerateStagger             = 124274,
                HeavyStagger                = 124273,
                GaiPlinsImperialBrew        = 208837,
                GaiPlinsImperialBrewHeal    = 208840,
                StaggeringAroundAura        = 213055,
                HotTrubAura                 = 202126,
                HotTrub                     = 202127,
                IronskinBrewAura            = 215479,
                QuickSip                    = 238129,
                NiuzaosEssence              = 232876
            };

            enum eTraits
            {
                StaggeringAround = 1282
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// Drinking a Purifying Brew will dispel all snares affecting you.
                if (l_Caster->HasAura(eSpells::NiuzaosEssence) && l_Caster->CanApplyPvPSpellModifiers())
                    l_Caster->RemoveAurasWithMechanic(1LL << Mechanics::MECHANIC_SNARE);

                if (AuraEffect* l_QuickSipEffect = l_Caster->GetAuraEffect(eSpells::QuickSip, EFFECT_2))
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::IronskinBrewAura))
                    {
                        if (AuraEffect* I_AuraEffect = l_Aura->GetEffect(EFFECT_1))
                            l_Aura->SetDuration(std::min<int32>(l_Aura->GetDuration() + l_QuickSipEffect->GetAmount() * IN_MILLISECONDS, I_AuraEffect->GetAmount() * l_Aura->GetMaxDuration()));
                    }
                    else
                    {
                        if (Aura* l_Aura = l_Caster->AddAura(eSpells::IronskinBrewAura, l_Caster))
                            l_Aura->SetDuration(l_QuickSipEffect->GetAmount() * IN_MILLISECONDS);
                    }
                }

                AuraEffect* l_Stagger = l_Caster->GetAuraEffect(eSpells::LightStagger, EFFECT_0);

                if (l_Stagger == nullptr)
                {
                    l_Stagger = l_Caster->GetAuraEffect(eSpells::ModerateStagger, EFFECT_0);

                    /// When clearing Moderate Stagger with Purifying Brew, you generate 1 stack of Elusive Brew.
                    if (l_Caster->HasAura(eSpells::T17Brewmaster4P))
                        l_Caster->CastSpell(l_Caster, eSpells::ElusiveBrew, true);
                }

                if (!l_Stagger)
                    l_Stagger = l_Caster->GetAuraEffect(eSpells::HeavyStagger, EFFECT_0);

                if (!l_Stagger)
                    return;

                if (l_Caster->HasAura(eSpells::GaiPlinsImperialBrew))
                    l_Caster->CastSpell(l_Caster, eSpells::GaiPlinsImperialBrewHeal, true);

                uint32 l_ClearAmount = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ElusiveDance, EFFECT_1))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::ElusiveDanceAura, true);
                    l_ClearAmount += l_AuraEffect->GetAmount();
                }

                if (l_Caster->HasAura(eSpells::StaggeringAroundAura))
                {
                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;

                    l_ClearAmount += l_Player->GetRankOfArtifactPowerId(eTraits::StaggeringAround);
                }

                if (AuraEffect const* l_AurEff = l_Caster->GetAuraEffect(eSpells::HotTrubAura, SpellEffIndex::EFFECT_0))
                {
                    int32 l_Bp = CalculatePct(CalculatePct(l_Stagger->GetAmount(), l_ClearAmount), l_AurEff->GetAmount());
                    l_Caster->CastCustomSpell(l_Caster, eSpells::HotTrub, &l_Bp, nullptr, nullptr, true);
                }

                l_Stagger->SetAmount(CalculatePct(l_Stagger->GetAmount(), (100 - l_ClearAmount)));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_purifying_brew_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_purifying_brew_SpellScript();
        }
};

/// Update to Legion 7.3.5 build 26365
/// Called by Keg Smash - 121253
class spell_monk_keg_smash: public SpellScriptLoader
{
    public:
        spell_monk_keg_smash() : SpellScriptLoader("spell_monk_keg_smash") { }

        class spell_monk_keg_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_keg_smash_SpellScript);

            enum eSpells
            {
                KegSmashVisual   = 123662,
                KegSmash         = 121253,
                IronskinBrew     = 115308,
                FortifyingBrew   = 115203,
                BlackOxBrew      = 115399,
                DoubleBarrelAura = 202335,
                DoubleBarrelStun = 202346,
                StaveOff         = 238093
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::KegSmashVisual, true);

                if (l_Caster->HasAura(eSpells::DoubleBarrelAura))
                {
                    l_Caster->CastSpell(l_Target, eSpells::DoubleBarrelStun, true);
                    l_Caster->RemoveAurasDueToSpell(eSpells::DoubleBarrelAura);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                SpellInfo const* l_KegsmashInfos = sSpellMgr->GetSpellInfo(eSpells::KegSmash);
                SpellInfo const* l_SpellInfoIronskinBrew = sSpellMgr->GetSpellInfo(eSpells::IronskinBrew);
                SpellInfo const* l_StaveOffInfo = sSpellMgr->GetSpellInfo(eSpells::StaveOff);
                if (!l_KegsmashInfos || !l_SpellInfoIronskinBrew || !l_StaveOffInfo)
                    return;

                if (l_Caster->HasAura(eSpells::StaveOff))
                {
                    int32 l_Chance = l_StaveOffInfo->Effects[EFFECT_0].BasePoints;
                    if (roll_chance_i(l_Chance))
                    {
                        l_Player->RestoreCharge(l_KegsmashInfos->ChargeCategoryEntry);
                        l_Caster->CastSpell(l_Target, eSpells::KegSmash, true);
                    }
                }

                l_Player->ReduceChargeCooldown(l_SpellInfoIronskinBrew->ChargeCategoryEntry, l_KegsmashInfos->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eSpells::FortifyingBrew, l_KegsmashInfos->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eSpells::BlackOxBrew, l_KegsmashInfos->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_keg_smash_SpellScript::HandleOnHit);
                AfterCast += SpellCastFn(spell_monk_keg_smash_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_keg_smash_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Ironskin Brew - 115308
class spell_monk_ironskin_brew: public SpellScriptLoader
{
    public:
        spell_monk_ironskin_brew() : SpellScriptLoader("spell_monk_ironskin_brew") { }

        class spell_monk_ironskin_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_ironskin_brew_SpellScript);

            enum eSpells
            {
                IronskinBrewAura        = 215479,
                QuickSip                = 238129,
                LightStagger            = 124275,
                ModerateStagger         = 124274,
                HeavyStagger            = 124273
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::IronskinBrewAura))
                {
                    if (AuraEffect* I_AuraEffect = l_Aura->GetEffect(EFFECT_1))
                        l_Aura->SetDuration(std::min<int32>(l_Aura->GetDuration() + l_Aura->GetMaxDuration(), I_AuraEffect->GetAmount() * l_Aura->GetMaxDuration()));
                }
                else
                    l_Caster->CastSpell(l_Caster, eSpells::IronskinBrewAura, true);

                /// Here we'll handle Quicksip (238129) stagger cleaning
                if (!l_Caster->HasAura(eSpells::QuickSip))
                    return;

                AuraEffect* l_Stagger = l_Caster->GetAuraEffect(eSpells::LightStagger, EFFECT_0);

                if (!l_Stagger)
                    l_Stagger = l_Caster->GetAuraEffect(eSpells::ModerateStagger, EFFECT_0);

                if (!l_Stagger)
                    l_Stagger = l_Caster->GetAuraEffect(eSpells::HeavyStagger, EFFECT_0);

                if (!l_Stagger)
                    return;

                SpellInfo const* l_QuickSipInfo = sSpellMgr->GetSpellInfo(eSpells::QuickSip);
                if (!l_QuickSipInfo)
                    return;

                uint8 l_ClearAmount = l_QuickSipInfo->Effects[EFFECT_1].BasePoints;

                l_Stagger->SetAmount(CalculatePct(l_Stagger->GetAmount(), (100 - l_ClearAmount)));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_ironskin_brew_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_ironskin_brew_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Disable - 116095
class spell_monk_disable : public SpellScriptLoader
{
    public:
        spell_monk_disable() : SpellScriptLoader("spell_monk_disable") { }

        class spell_monk_disable_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_disable_SpellScript);

            enum eSpells
            {
                DisableRootAura = 116706
            };

            bool m_SnaredOnHit = false;

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetCaster()->getVictim();

                if (l_Target == nullptr)
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Target->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED))
                    m_SnaredOnHit = true;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (m_SnaredOnHit)
                {
                    l_Caster->CastSpell(l_Target, eSpells::DisableRootAura, true);

                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (l_Player->CanApplyPvPSpellModifiers())
                        {
                            if (Aura* l_RootDebuff = l_Target->GetAura(eSpells::DisableRootAura, l_Caster->GetGUID()))
                            {
                                l_RootDebuff->SetMaxDuration(l_RootDebuff->GetMaxDuration() / 2);
                                l_RootDebuff->SetDuration(l_RootDebuff->GetMaxDuration());
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_disable_SpellScript::CheckCast);
                OnHit += SpellHitFn(spell_monk_disable_SpellScript::HandleOnHit);
            }
        };

        class spell_monk_disable_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_disable_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& /*p_ProcInfo*/)
            {
                p_AurEff->GetBase()->RefreshDuration();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_disable_AuraScript::OnProc, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_disable_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_disable_SpellScript();
        }
};

/// Last update 7.3.2
/// Zen Pilgrimage - 126892 and Zen Pilgrimage : Return - 126895
class spell_monk_zen_pilgrimage: public SpellScriptLoader
{
    public:
        spell_monk_zen_pilgrimage() : SpellScriptLoader("spell_monk_zen_pilgrimage") { }

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

        class spell_monk_zen_pilgrimage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_zen_pilgrimage_SpellScript);

            enum eSpells
            {
                Pilgrimage           = 126892,
                PilgrimageReturn     = 126895,
                PilgrimageReturnAura = 126896
            };

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

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::Pilgrimage) || !sSpellMgr->GetSpellInfo(eSpells::PilgrimageReturn))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (GetSpellInfo()->Id == eSpells::Pilgrimage)
                        {
                            l_Player->SaveRecallPosition();
                            if (l_Player->getLevel() >= 98)
                                l_Player->TeleportTo(1514, 917.6382f, 3605.0444f, 196.5522f, 6.2603f);
                            else
                                l_Player->TeleportTo(870, 3818.55f, 1793.18f, 950.35f, l_Player->GetOrientation());

                           l_Caster->DelayedCastSpell(l_Caster, eSpells::PilgrimageReturnAura, true, 1000);
                        }
                        else if (GetSpellInfo()->Id == eSpells::PilgrimageReturn)
                        {
                            l_Player->TeleportTo(l_Player->m_recallMap, l_Player->m_recallX, l_Player->m_recallY, l_Player->m_recallZ, l_Player->m_recallO);
                            l_Player->RemoveAura(eSpells::PilgrimageReturnAura);
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_zen_pilgrimage_SpellScript::CheckMap);
                OnEffectHitTarget += SpellEffectFn(spell_monk_zen_pilgrimage_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_zen_pilgrimage_SpellScript();
        }
};


/// last update : 6.1.2 19802
/// Provoke - 115546
class spell_monk_provoke: public SpellScriptLoader
{
    public:
        spell_monk_provoke() : SpellScriptLoader("spell_monk_provoke") { }

        class spell_monk_provoke_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_provoke_SpellScript);

            bool m_IsMultiTarget = false;

            enum eSpells
            {
                ProvokeSingleTarget = 116189,
                ProvokeMultiTarget  = 118635,
                Admonishment        = 207025
            };

            enum eNpcs
            {
                BlackOxStatue = 61146
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster)
                {
                    Unit* l_Target = GetExplTargetUnit();
                    if (!l_Target)
                        return SPELL_FAILED_NO_VALID_TARGETS;
                    else if (l_Target->IsPlayer() && !l_Caster->HasAura(eSpells::Admonishment))
                        return SPELL_FAILED_BAD_TARGETS;
                    else if (!l_Target->IsWithinLOSInMap(GetCaster()))
                        return SPELL_FAILED_LINE_OF_SIGHT;

                    if (l_Target->GetEntry() == eNpcs::BlackOxStatue && l_Target->GetOwnerGUID() == l_Caster->GetGUID())
                        m_IsMultiTarget = true;
                    else if (!l_Caster->IsValidAttackTarget(l_Target))
                        return SPELL_FAILED_TARGET_FRIENDLY;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (m_IsMultiTarget)
                            l_Caster->CastSpell(l_Target, eSpells::ProvokeMultiTarget, true);
                        else
                            l_Caster->CastSpell(l_Target, eSpells::ProvokeSingleTarget, true);
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_provoke_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_monk_provoke_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_provoke_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Provoke (launch) - 118635
class spell_monk_provoke_launch : public SpellScriptLoader
{
    public:
        spell_monk_provoke_launch() : SpellScriptLoader("spell_monk_provoke_launch") { }

        class spell_monk_provoke_launch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_provoke_launch_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    /// Unusable on pvp
                    if (p_Object->IsPlayer())
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_provoke_launch_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_provoke_launch_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_provoke_launch_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_provoke_launch_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Touch of Death - 115080
class spell_monk_touch_of_death : public SpellScriptLoader
{
    public:
        spell_monk_touch_of_death() : SpellScriptLoader("spell_monk_touch_of_death") { }

        class spell_monk_touch_of_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_touch_of_death_AuraScript);

            enum eSpells
            {
                TouchOfDeathDamages         = 229980,
                GaleBurstTrait              = 195399,
                GaleBurstAura               = 195403,
                DeathArt                    = 195266,
                Serenity                    = 152173,
                MasteryComboStrikes         = 115636,
                HiddenMastersForbiddenTouch = 213112
            };

            enum eArtifactPowers
            {
                DeathArtArtifact = 822
            };

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::GaleBurstTrait))
                    l_Caster->CastSpell(l_Target, eSpells::GaleBurstAura, true);
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return;

                if (!l_Target->isAlive() && l_Caster->HasAura(eSpells::DeathArt))
                {
                    SpellInfo const* l_DeathArtInfo = sSpellMgr->GetSpellInfo(eSpells::DeathArt);
                    Player* l_Player = l_Caster->ToPlayer();

                    if (l_Player && l_DeathArtInfo)
                    {
                        if (l_Player->HasSpellCooldown(m_scriptSpellId))
                        {
                            int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::DeathArtArtifact);
                            int32 l_RemainingCD = l_Player->GetSpellCooldownDelay(m_scriptSpellId);
                            int32 l_ToRemoveCD = CalculatePct(l_RemainingCD, l_DeathArtInfo->Effects[EFFECT_0].BasePoints * l_Rank);

                            l_Player->ReduceSpellCooldown(m_scriptSpellId, l_ToRemoveCD);
                        }
                    }
                }
            }

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                /// If the target is a player, only deal 25% of HP
                /// Percentage amount taken from youtube video: https://youtu.be/hHotc4wjub8?t=25s
                /// Player health: 1.9M; Damage dealt: 447k; So 447,000 / 1,900,000 = ~0.235, with rounded HP
                if (l_Target->GetTypeId() == TYPEID_PLAYER)
                    p_Amount = l_Caster->CountPctFromMaxHealth(15); /// https://www.freakz.ro/forum/FIXED-Monk-Windwalker-Touch-of-Death-damage-in-PvP-t515921.html
                else
                    p_Amount = l_Caster->CountPctFromMaxHealth(l_SpellInfo->Effects[EFFECT_1].BasePoints);
            }

            void HandleAfterEffectPeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                SpellInfo const* l_SpellInfoTouchOfDeathDamages = sSpellMgr->GetSpellInfo(eSpells::TouchOfDeathDamages);

                int32 l_Amount = p_AurEff->GetAmount();     ///< Amount of Effect 0: 1

                if (!l_Caster || !l_Target || !l_SpellInfoTouchOfDeathDamages || !l_Amount)
                    return;

                if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::GaleBurstAura, EFFECT_0, l_Caster->GetGUID()))
                    if (SpellInfo const* l_SpellInfoGaleBurst = sSpellMgr->GetSpellInfo(eSpells::GaleBurstTrait))
                        l_Amount += CalculatePct(l_AuraEffect->GetAmount(), l_SpellInfoGaleBurst->Effects[EFFECT_0].BasePoints);

                if (AuraEffect* l_SerenityEffect = l_Caster->GetAuraEffect(eSpells::Serenity, EFFECT_1))
                    AddPct(l_Amount, l_SerenityEffect->GetAmount());

                if (AuraEffect* l_MasteryComboEffect = l_Caster->GetAuraEffect(eSpells::MasteryComboStrikes, EFFECT_0))
                    AddPct(l_Amount, l_MasteryComboEffect->GetAmount());

                if (AuraEffect* l_HiddenMasters = l_Caster->GetAuraEffect(eSpells::HiddenMastersForbiddenTouch, EFFECT_1))
                    AddPct(l_Amount, l_HiddenMasters->GetAmount());

                l_Caster->CastCustomSpell(l_Target, eSpells::TouchOfDeathDamages, &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_touch_of_death_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_monk_touch_of_death_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_touch_of_death_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_monk_touch_of_death_AuraScript::HandleAfterEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_touch_of_death_AuraScript();
        }

        class spell_monk_touch_of_death_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_touch_of_death_SpellScript);

            enum eSpells
            {
                HiddenMastersForbiddenTouch = 213112,
                HiddenMastersForbiddenTouchCD = 213114,
                TouchOfDeath = 115080,
                HitComboBuff = 196741
            };

            /// Prevents casting 2 death mark on the same target with the lego, tested on retail 7.3.5 and you CANNOT do that
            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                if (l_Target->HasAura(m_scriptSpellId, l_Caster->GetGUID()))
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) != m_scriptSpellId)
                    l_Caster->CastSpell(l_Caster, eSpells::HitComboBuff, true);

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) = m_scriptSpellId;

                if (!l_Caster->HasAura(eSpells::HiddenMastersForbiddenTouch))
                    return;

                if (l_Caster->HasAura(eSpells::HiddenMastersForbiddenTouchCD))
                    l_Caster->RemoveAura(eSpells::HiddenMastersForbiddenTouchCD);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::HiddenMastersForbiddenTouchCD, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_touch_of_death_SpellScript::CheckCast);
                AfterCast += SpellCastFn(spell_monk_touch_of_death_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_touch_of_death_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Call by Flying Serpent Kick - 101545, Roll - 109132, and Chi Torpedo - 115008
class spell_monk_roll: public SpellScriptLoader
{
    public:
        spell_monk_roll() : SpellScriptLoader("spell_monk_roll") { }

        class spell_monk_roll_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_roll_SpellScript);

            enum eSpells
            {
                MonkWoDPvPBrewmaster2PBonus     = 165691,
                MonkWoDPvPBrewmasterAura        = 165692,
                YulonsGiftMistWeaver            = 159534,
                YulonsGiftWindWalker            = 232879,
                RollJump                        = 194427,
                Roll                            = 109132,
                RollTrigger                     = 107427,
                ItemPvPGlovesBonus              = 124489,
                FlyingSerpentKick               = 101545,
                ChiTorpedo                      = 115008
            };

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::Roll))
                    return false;
                return true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                l_Caster->AddDelayedEvent([=]() -> void
                {
                    l_Caster->CastSpell(l_Caster, eSpells::RollTrigger);
                    Aura* l_Aura = l_Caster->GetAura(eSpells::RollTrigger);
                    if (!l_Aura)
                        return;

                    l_Aura->SetDuration(800);

                    AuraApplication* l_AuraApplication = l_Aura->GetApplicationOfTarget(GetCaster()->GetGUID());
                    if (!l_AuraApplication)
                        return;

                    l_AuraApplication->ClientUpdate();

                    if (!l_Caster || l_Caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (l_Caster->HasAura(eSpells::MonkWoDPvPBrewmaster2PBonus))
                        l_Caster->CastSpell(l_Caster, eSpells::MonkWoDPvPBrewmasterAura, true);
                }, 0);

                l_Caster->AddDelayedEvent([=]() -> void
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::RollSpell) = false;
                }, 500);
            }

            void HandleRemoveSnare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::ItemPvPGlovesBonus) ||
                    ((GetSpellInfo()->Id == eSpells::Roll || GetSpellInfo()->Id == eSpells::ChiTorpedo) && l_Caster->HasAura(eSpells::YulonsGiftMistWeaver)) ||
                    (GetSpellInfo()->Id == eSpells::FlyingSerpentKick && l_Caster->HasAura(eSpells::YulonsGiftWindWalker)))
                    l_Caster->RemoveAurasWithMechanic(1LL << Mechanics::MECHANIC_SNARE);
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RollJump, true);
                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::RollSpell) = true;
            }

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                if (m_scriptSpellId == eSpells::ChiTorpedo && l_Caster->HasAura(m_scriptSpellId))
                    return SPELL_FAILED_DONT_REPORT;

                if (m_scriptSpellId == eSpells::Roll && l_Caster->m_SpellHelper.GetBool(eSpellHelpers::RollSpell))
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->HasUnitState(UNIT_STATE_ROOT))
                    return SPELL_FAILED_ROOTED;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Roll)
                {
                    BeforeCast += SpellCastFn(spell_monk_roll_SpellScript::HandleBeforeCast);
                    AfterCast += SpellCastFn(spell_monk_roll_SpellScript::HandleAfterCast);
                }
                if (m_scriptSpellId == eSpells::Roll || m_scriptSpellId == eSpells::ChiTorpedo)
                    OnCheckCast += SpellCheckCastFn(spell_monk_roll_SpellScript::CheckCast);

                AfterCast += SpellCastFn(spell_monk_roll_SpellScript::HandleRemoveSnare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_roll_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Fists of Fury - 113656
class spell_monk_fists_of_fury : public SpellScriptLoader
{
    public:
        spell_monk_fists_of_fury() : SpellScriptLoader("spell_monk_fists_of_fury") { }

        enum eNpc
        {
            NPC_FireSpirit = 69791,
            NPC_EarthSpirit = 69792
        };

        enum eSpells
        {
            HeavyHandedStrikes       = 205003,
            FistOfFuryStun           = 120086,
            TransferThePower         = 195321,
            MonkT20Windwalker2PBonus = 242259,
            PressurePoint            = 247255,
        };

        class spell_monk_fists_of_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_fists_of_fury_AuraScript);

            bool m_StunProc = false;

            void OnTickStun(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || m_StunProc)
                    return;

                if (l_Caster->HasAura(eSpells::HeavyHandedStrikes))
                {
                    m_StunProc = true;

                    l_Caster->CastSpell(l_Caster, eSpells::FistOfFuryStun, true);
                }
            }

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::FistsOfFuryStunnedTargets].clear();
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::TransferThePower) = 0;

                if (l_Caster->HasAura(eSpells::MonkT20Windwalker2PBonus))
                    l_Caster->AddAura(eSpells::PressurePoint, l_Caster);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_fists_of_fury_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_fists_of_fury_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_fists_of_fury_AuraScript::OnTickStun, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_fists_of_fury_AuraScript();
        }

        class spell_monk_fists_of_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_fists_of_fury_SpellScript);

            void HandleOnCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::TransferThePower))
                    {
                        l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::TransferThePower) = l_Aura->GetStackAmount();
                        l_Caster->RemoveAura(eSpells::TransferThePower);
                    }

                    // Storm, Earth and Fire channel 25% quicker
                    if (!l_Caster->ToCreature())
                        return;

                    Spell* l_Spell = GetSpell();
                    Creature* l_Creature = l_Caster->ToCreature();

                    if (l_Creature && l_Spell && (l_Creature->GetEntry() == eNpc::NPC_EarthSpirit || l_Creature->GetEntry() == eNpc::NPC_FireSpirit))
                    {
                        int32 l_duration = (l_Spell->GetChanneledDuration() ? l_Spell->GetChanneledDuration() : GetSpellInfo()->GetDuration()) * 0.75;
                        l_Creature->ModSpellCastTime(l_Spell->GetSpellInfo(), l_duration, l_Spell);
                        l_Spell->SetChanneledDuration(l_duration);
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_monk_fists_of_fury_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_fists_of_fury_SpellScript();
        }
};

/// last update : 7.3.5 build 26365
/// Fists of Fury (damage) - 117418
class spell_monk_fists_of_fury_damage : public SpellScriptLoader
{
    public:
        spell_monk_fists_of_fury_damage() : SpellScriptLoader("spell_monk_fists_of_fury_damage") { }

        class spell_monk_fists_of_fury_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_fists_of_fury_damage_SpellScript);

            enum eSpells
            {
                FistOfFuryAura      = 113656,
                TransferThePower    = 195321
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TransferThePower);
                        if (l_SpellInfo == nullptr)
                            return;

                        int32 l_Damage = GetHitDamage();
                        Player* l_Monk = l_Caster->GetSpellModOwner();
                        if (!l_Monk)
                            return;

                        if (uint32& l_Stacks = l_Monk->m_SpellHelper.GetUint32(eSpellHelpers::TransferThePower))
                        {
                            uint32 l_Amount = l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Stacks;

                            if (l_Monk->CanApplyPvPSpellModifiers())
                                l_Amount *= 0.6f; ///< 60% in PvP

                            AddPct(l_Damage, l_Amount);
                        }

                        if (l_Monk->CanApplyPvPSpellModifiers())
                            l_Damage *= 0.8f; ///< 80% in PvP

                        SetHitDamage(l_Damage);
                    }
                }
            }

            void SelectTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::CrosswindsAvailableTargets].clear();

                for (WorldObject* l_Target : p_Targets)
                {
                    if (Unit* l_UnitTarget = l_Target->ToUnit())
                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::CrosswindsAvailableTargets].insert(l_UnitTarget->GetGUID());
                }

            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_fists_of_fury_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_fists_of_fury_damage_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_fists_of_fury_damage_SpellScript();
        }
};

/// Last Update 7.0.3
/// Fists of Fury (Stun) - 120086
class spell_monk_fists_of_fury_stun: public SpellScriptLoader
{
    public:
        spell_monk_fists_of_fury_stun() : SpellScriptLoader("spell_monk_fists_of_fury_stun") { }

        class spell_monk_fists_of_fury_stun_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_fists_of_fury_stun_SpellScript);

            void RemoveInvalidTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::CrosswindsAvailableTargets].clear();

                p_Targets.remove_if([=](WorldObject* l_Target) -> bool
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    SpellInfo const* l_SpellInfo = GetSpellInfo();
                    if (!l_UnitTarget || !l_SpellInfo)
                        return true;

                    if (l_UnitTarget->HasAura(l_SpellInfo->Id, l_Caster->GetGUID()))
                        return true;

                    l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::CrosswindsAvailableTargets].insert(l_UnitTarget->GetGUID());

                    std::set<uint64>& l_StunnedsForCaster = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::FistsOfFuryStunnedTargets];
                    if (l_StunnedsForCaster.find(l_Target->GetGUID()) == l_StunnedsForCaster.end())
                        l_StunnedsForCaster.insert(l_Target->GetGUID());
                    else
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_fists_of_fury_stun_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_fists_of_fury_stun_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Expel Harm - 115072
class spell_monk_expel_harm: public SpellScriptLoader
{
    public:
        spell_monk_expel_harm() : SpellScriptLoader("spell_monk_expel_harm") { }

        class spell_monk_expel_harm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_expel_harm_SpellScript);

            enum eSpells
            {
                ExpelHarmDamage     = 115129,
                GiftOfTheOxRight    = 124503,
                GiftOfTheOxLeft     = 124506,
                GiftOfTheOxHeal     = 124507,
                GiftOfTheOxHealBig  = 214416,
                GiftOfTheOxLeftBig  = 214420,
                GiftOfTheOxRightBig = 214418,
                HealingSphereActive = 224863
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                std::list<AreaTrigger*> l_AreatriggerList;
                l_Caster->GetAreaTriggerList(l_AreatriggerList, eSpells::GiftOfTheOxLeft);
                l_Caster->GetAreaTriggerList(l_AreatriggerList, eSpells::GiftOfTheOxRight);
                l_Caster->GetAreaTriggerList(l_AreatriggerList, eSpells::GiftOfTheOxLeftBig);
                l_Caster->GetAreaTriggerList(l_AreatriggerList, eSpells::GiftOfTheOxRightBig);

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ExpelHarm) = 0;

                for (AreaTrigger* l_Areatrigger : l_AreatriggerList)
                {
                    l_Caster->CastSpell(l_Caster, l_Areatrigger->GetEntry() == 373 ? eSpells::GiftOfTheOxHeal : eSpells::GiftOfTheOxHealBig, true);
                    l_Areatrigger->Despawn();
                }

                int32 l_Damage = CalculatePct(l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ExpelHarm), l_SpellInfo->Effects[EFFECT_1].BasePoints);

                l_Caster->CastCustomSpell(l_Caster, eSpells::ExpelHarmDamage, &l_Damage, nullptr, nullptr, true);

                l_Caster->RemoveAurasDueToSpell(eSpells::HealingSphereActive);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_expel_harm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_expel_harm_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Gift Of The Ox (Heal) - 124507, 214416
class spell_monk_gift_of_the_ox_heal : public SpellScriptLoader
{
    public:
        spell_monk_gift_of_the_ox_heal() : SpellScriptLoader("spell_monk_gift_of_the_ox_heal") { }

        class spell_monk_gift_of_the_ox_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_gift_of_the_ox_heal_SpellScript);

            enum eSpells
            {
                GiftOfTheOxBig       = 214416,
                T20Brewmaster4P      = 242256,
                LightStagger         = 124275,
                ModerateStagger      = 124274,
                HeavyStagger         = 124273,
                GiftedStudent        = 213136,
                GiftetStudentProc    = 227678
            };

            void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                uint32 l_Heal = GetHitHeal();
                if (!l_Caster)
                    return;

                uint32 l_Id = GetSpellInfo()->Id;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ExpelHarm) += l_Heal;
                SetHitHeal(l_Heal);

                if (AuraEffect* l_GiftedStudent = l_Caster->GetAuraEffect(eSpells::GiftedStudent, EFFECT_0))
                {
                    int32 l_Amount = l_GiftedStudent->GetAmount();
                    l_Caster->CastCustomSpell(l_Caster, eSpells::GiftetStudentProc, &l_Amount, nullptr, nullptr, nullptr, nullptr, nullptr, true);
                }

                if (l_Caster->HasAura(eSpells::T20Brewmaster4P))
                {
                    AuraEffect* l_Stagger = l_Caster->GetAuraEffect(eSpells::LightStagger, EFFECT_0);
                    if (!l_Stagger)
                        l_Stagger = l_Caster->GetAuraEffect(eSpells::ModerateStagger, EFFECT_0);

                    if (!l_Stagger)
                        l_Stagger = l_Caster->GetAuraEffect(eSpells::HeavyStagger, EFFECT_0);

                    if (!l_Stagger)
                        return;

                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T20Brewmaster4P))
                        l_Stagger->SetAmount(CalculatePct(l_Stagger->GetAmount(), (100 - l_SpellInfo->Effects[EFFECT_0].BasePoints)));
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_gift_of_the_ox_heal_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_gift_of_the_ox_heal_SpellScript();
        }
};


// Detox - 115450
class spell_monk_detox: public SpellScriptLoader
{
    public:
        spell_monk_detox() : SpellScriptLoader("spell_monk_detox") { }

        class spell_monk_detox_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_detox_SpellScript);

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr || l_Player == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                DispelChargesList l_DispelList;
                uint32 l_DispelMask = 0;

                /// Create dispel mask by dispel type
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                for (uint8 l_Itr = 0; l_Itr < l_SpellInfo->EffectCount; ++l_Itr)
                {
                    if (l_Player->GetActiveSpecializationID() != SPEC_MONK_MISTWEAVER && l_Itr == EFFECT_2)
                        continue;

                    if (l_SpellInfo->Effects[l_Itr].IsEffect())
                    {
                        uint32 l_Dispel_type = l_SpellInfo->Effects[l_Itr].MiscValue;
                        l_DispelMask = l_SpellInfo->GetDispelMask(DispelType(l_Dispel_type));
                        l_Target->GetDispellableAuraList(l_Player, l_DispelMask, l_DispelList);
                    }
                }

                DispelChargesList l_TempDispelList = l_DispelList;

                for (auto l_Itr : l_TempDispelList)
                {
                    if (Aura* l_Aura = l_Itr.first)
                    {
                        AuraApplication * l_AuraApplication = l_Aura->GetApplicationOfTarget(l_Target->GetGUID());
                        if (l_AuraApplication->IsPositive())
                            l_DispelList.remove(l_Itr);
                    }
                }

                if (l_DispelList.empty())
                    return SPELL_FAILED_NOTHING_TO_DISPEL;

                return SPELL_CAST_OK;
            }

            void HandleDispel(SpellEffIndex p_EffectIndex)
            {
                PreventHitDefaultEffect(p_EffectIndex);

                Player* l_Player = GetCaster()->ToPlayer();
                if (l_Player && l_Player->GetActiveSpecializationID() != SPEC_MONK_MISTWEAVER)
                    return;

                GetSpell()->EffectDispel(p_EffectIndex);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_detox_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_monk_detox_SpellScript::HandleDispel, EFFECT_2, SPELL_EFFECT_DISPEL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_detox_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Windwalking - 137025
class spell_monk_windwalker_monk : public SpellScriptLoader
{
    public:
        spell_monk_windwalker_monk() : SpellScriptLoader("spell_monk_windwalker_monk") { }

        class spell_monk_stance_of_tiger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_stance_of_tiger_AuraScript);

            enum eSpells
            {
                WindWalking = 166646
            };

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->RemoveAura(eSpells::WindWalking);
            }

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSpells::WindWalking, true);
            }

            void OnUpdate(uint32 /*diff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->HasAura(eSpells::WindWalking))
                        l_Caster->CastSpell(l_Caster, eSpells::WindWalking, true);

                    Player * l_Player = l_Caster->ToPlayer();
                    if (l_Player && l_Player->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER) ///< 7.1.5 - Build 23420: fixes issue causing non-windwalking monks to have the aura in BG and arenas.
                    {
                        uint32 l_SpellId = GetSpellInfo()->Id;
                        l_Player->AddDelayedEvent([l_Player, l_SpellId]() -> void
                        {
                            l_Player->RemoveAura(l_SpellId);
                        }, 0);
                    }
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_monk_stance_of_tiger_AuraScript::OnUpdate);
                AfterEffectApply += AuraEffectApplyFn(spell_monk_stance_of_tiger_AuraScript::AfterApply, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_stance_of_tiger_AuraScript::AfterRemove, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_stance_of_tiger_AuraScript();
        }
};

/// 7.1.5 - Build 23420
/// Called by Windwalking 166646
class spell_monk_windwalking : public SpellScriptLoader
{
public:
    spell_monk_windwalking() : SpellScriptLoader("spell_monk_windwalking") { }

    class spell_monk_windwalking_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_windwalking_AuraScript);

        enum eSpells
        {
            Windwalking = 166646
        };

        uint32 m_timer = 0;

        void OnUpdate(uint32 p_Diff)
        {
            m_timer += p_Diff;
            if (m_timer < 500)
                return;

            m_timer = 0;

            Unit* l_Monk = GetCaster();
            Unit* l_Target = GetUnitOwner();
            if (!l_Monk)
            {
                l_Target->RemoveAura(eSpells::Windwalking);
                return;
            }

            if (!l_Target || l_Monk->GetGUID() == l_Target->GetGUID())
                return;

            Player* l_MonkPlayer = l_Monk->ToPlayer();
            Player* l_TargetPlayer = l_Target->ToPlayer();

            if (!l_MonkPlayer || !l_TargetPlayer)
            {
                l_Target->RemoveAura(GetAura());
                return;
            }

            if (!l_TargetPlayer->IsInSameGroupWith(l_MonkPlayer))
            {
                Aura* l_Aura = l_TargetPlayer->GetAura(eSpells::Windwalking, l_Monk->GetGUID());
                if (l_Aura->GetMaxDuration() == -1)
                {
                    l_Aura->SetMaxDuration(10 * IN_MILLISECONDS);
                    l_Aura->SetDuration(10 * IN_MILLISECONDS);
                }
            }
        }

        void Register() override
        {
                OnAuraUpdate += AuraUpdateFn(spell_monk_windwalking_AuraScript::OnUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_monk_windwalking_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Vivify - 116670
class spell_monk_vivify : public SpellScriptLoader
{
    public:
        spell_monk_vivify() : SpellScriptLoader("spell_monk_vivify") { }

        class spell_monk_vivify_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_vivify_SpellScript);

            enum eSpells
            {
                LifecyclesAura = 197915,
                LifecyclesEnvelopingMist = 197919,
                T19Mistweaver4P = 211423
            };

            void CorrectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (Unit* l_RealTarget = GetExplTargetUnit())
                {
                    uint32 l_TargetCount = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                    m_MainTarget = l_RealTarget->GetGUID();

                    p_Targets.remove_if([this, l_RealTarget](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == nullptr)
                            return true;

                        Unit* l_Target = p_Object->ToUnit();
                        if (l_Target == nullptr)
                            return true;

                        if (l_Target->GetGUID() == l_RealTarget->GetGUID())
                            return true;
                        return false;
                    });

                    p_Targets.sort(JadeCore::ObjectDistanceOrderPred(l_RealTarget));
                    p_Targets.sort(JadeCore::HealthPctOrderPred());
                    p_Targets.resize(l_TargetCount);

                    ///< Add primary target at the top for Smoothing Mist
                    p_Targets.push_front(l_RealTarget);
                    if (GetCaster())
                        GetCaster()->m_SpellHelper.GetUint64(eSpellHelpers::SoothingMistTarget) = l_RealTarget->GetGUID();
                }
            }

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster && l_Caster->HasAura(eSpells::LifecyclesAura))
                    l_Caster->CastSpell(l_Caster, eSpells::LifecyclesEnvelopingMist, true);
            }

            void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || l_Target->GetGUID() != m_MainTarget)
                    return;

                uint32 l_Heal = GetHitHeal();

                /// Item - Monk T19 Mistweaver 4P Bonus
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Mistweaver4P, EFFECT_0))
                    AddPct(l_Heal, l_AuraEffect->GetAmount());

                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_vivify_SpellScript::CorrectTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_monk_vivify_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_HEAL);
                AfterCast += SpellCastFn(spell_monk_vivify_SpellScript::HandleCast);
            }

        private:
            uint64 m_MainTarget;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_vivify_SpellScript();
        }
};

// Afterlife - 116092
class spell_monk_afterlife: public SpellScriptLoader
{
    public:
        spell_monk_afterlife() : SpellScriptLoader("spell_monk_afterlife") { }

        class spell_monk_afterlife_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_afterlife_AuraScript);

            enum eSpells
            {
                BlackoutKick        = 100784,
                SummonHealingSphere = 117032,
                SummonChiSphere     = 163271
            };

            void OnProcHealingSphere(AuraEffect const* /*aurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() && p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id == eSpells::BlackoutKick)
                    return;

                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSpells::SummonHealingSphere, true);
            }

            void OnProcChiSphere(AuraEffect const* p_AuraEffect, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() && p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::BlackoutKick)
                    return;

                if (!roll_chance_i(p_AuraEffect->GetAmount()))
                    return;

                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSpells::SummonChiSphere, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_afterlife_AuraScript::OnProcHealingSphere, EFFECT_0, SPELL_AURA_DUMMY);
                OnEffectProc += AuraEffectProcFn(spell_monk_afterlife_AuraScript::OnProcChiSphere, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_afterlife_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Monk WoD PvP Brewmaster 2P Bonus - 165691
class spell_monk_WoDPvPBrewmaster2PBonus : public SpellScriptLoader
{
    public:
        spell_monk_WoDPvPBrewmaster2PBonus() : SpellScriptLoader("spell_monk_WoDPvPBrewmaster2PBonus") { }

        class spell_monk_WoDPvPBrewmaster2PBonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_WoDPvPBrewmaster2PBonus_AuraScript);

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_ProcInfo*/)
            {
                PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_WoDPvPBrewmaster2PBonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_WoDPvPBrewmaster2PBonus_AuraScript();
        }
};

/// Crackling Tiger Lightning - 123996
class spell_monk_crackling_tiger_lightning : public SpellScriptLoader
{
    public:
        spell_monk_crackling_tiger_lightning() : SpellScriptLoader("spell_monk_crackling_tiger_lightning") { }

        class spell_monk_crackling_tiger_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_crackling_tiger_lightning_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = l_Caster->GetOwner();
                Unit* l_Target = GetHitUnit();

                if (l_Owner == nullptr)
                    return;

                int32 l_Damage = 0.67f * l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);
                l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_crackling_tiger_lightning_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_crackling_tiger_lightning_SpellScript();
        }
};

/// Item - Monk T17 Brewmaster 2P Bonus - 165353
class spell_monk_item_t17_brewmaster_2p_bonus : public SpellScriptLoader
{
    public:
        spell_monk_item_t17_brewmaster_2p_bonus() : SpellScriptLoader("spell_monk_item_t17_brewmaster_2p_bonus") { }

        class spell_monk_item_t17_brewmaster_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_item_t17_brewmaster_2p_bonus_AuraScript);

            enum eSpell
            {
                SwiftReflexes = 165356
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!(p_EventInfo.GetHitMask() & ProcFlagsExLegacy::PROC_EX_DODGE))
                    return;

                /// Dodging an attack generates 2 Energy.
                l_Caster->CastSpell(l_Caster, eSpell::SwiftReflexes, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_item_t17_brewmaster_2p_bonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_item_t17_brewmaster_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 23420
/// Breath of Fire - 115181
class spell_monk_breath_of_fire : public SpellScriptLoader
{
    public:
        spell_monk_breath_of_fire() : SpellScriptLoader("spell_monk_breath_of_fire") { }

        class spell_monk_breath_of_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_breath_of_fire_SpellScript);

            enum eSpells
            {
                KegSmash             = 121253,
                BreathofFireDoT      = 123725,
                FirestoneWalkersAura = 224489,
                FortifyingBrew       = 115203,
                DragonFireBrewTrait  = 213183,
                DragonFireBrewSpell  = 227681,
                IncendiaryBreathAura = 202272,
                IncendiaryBreath     = 202274,
                BlackoutComboBuff    = 228563,
                BlackoutCombo        = 196736
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::FirestoneWalkersAura))
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::FirestoneWalkersCooldownReduction) = 0;

                if (l_Caster->HasAura(eSpells::DragonFireBrewTrait))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::DragonFireBrewSpell, true);
                    l_Caster->CastSpell(l_Caster, eSpells::DragonFireBrewSpell, true);
                }

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::BlackoutComboBuff))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BlackoutCombo))
                        l_Aura->Remove();
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(eSpells::KegSmash))
                    l_Caster->CastSpell(l_Target, eSpells::BreathofFireDoT, true);

                if (l_Caster->HasAura(eSpells::IncendiaryBreathAura))
                    l_Caster->CastSpell(l_Target, eSpells::IncendiaryBreath, true);

                if (l_Caster->HasAura(eSpells::FirestoneWalkersAura))
                {
                    Player* l_Player = l_Caster->ToPlayer();
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FirestoneWalkersAura);
                    int32 l_CooldownReduction = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                    if (!l_SpellInfo || !l_Player || !l_CooldownReduction ||  l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::FirestoneWalkersCooldownReduction) >= l_SpellInfo->Effects[EFFECT_1].BasePoints)
                        return;

                    l_Player->ReduceSpellCooldown(eSpells::FortifyingBrew, l_CooldownReduction * IN_MILLISECONDS);
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::FirestoneWalkersCooldownReduction) += l_CooldownReduction;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_breath_of_fire_SpellScript::HandleAfterCast);
                AfterHit += SpellHitFn(spell_monk_breath_of_fire_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_breath_of_fire_SpellScript();
        }
};

/// Last Update 7.1.5
/// Fortifying Brew - 115203
class spell_monk_fortifying_brew : public SpellScriptLoader
{
    public:
        spell_monk_fortifying_brew() : SpellScriptLoader("spell_monk_fortifying_brew") { }

        class spell_monk_fortifying_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_fortifying_brew_SpellScript);

            enum eSpells
            {
                FortifyingBrewAura  = 120954,
                FortificationAura   = 213340,
                FortificationProc   = 213341
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FortifyingBrewAura, true);

                if (l_Caster->HasAura(eSpells::FortificationAura))
                    l_Caster->CastSpell(l_Caster, eSpells::FortificationProc, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_fortifying_brew_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_fortifying_brew_SpellScript();
        }
};

/// last update : 7.0.2
/// Eye of the Tiger - 196607
class spell_monk_eye_of_the_tiger : public SpellScriptLoader
{
    public:
        spell_monk_eye_of_the_tiger() : SpellScriptLoader("spell_monk_eye_of_the_tiger") { }

        class spell_monk_eye_of_the_tiger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_eye_of_the_tiger_AuraScript);

            enum eSpells
            {
                EyeoftheTigerTrigger    = 196608,
                TiggerPalm              = 100780
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcInfo)
            {
                PreventDefaultAction();

                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::TiggerPalm)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcInfo.GetDamageInfo()->GetTarget();

                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EyeoftheTigerTrigger, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_eye_of_the_tiger_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_eye_of_the_tiger_AuraScript();
        }
};

/// last update : 7.0.3
/// Celestial Fortune - 216519
class spell_monk_celestial_fortune : public SpellScriptLoader
{
    public:
        spell_monk_celestial_fortune() : SpellScriptLoader("spell_monk_celestial_fortune") { }

        class spell_monk_celestial_fortune_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_celestial_fortune_AuraScript);

            enum eSpells
            {
                CelestialFortuneHealEffect = 216521
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                float l_Crit_chance = l_Caster->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE);
                if (roll_chance_f(l_Crit_chance))
                {
                    int32 l_HealAmount = CalculatePct(l_DamageInfo->GetAmount(), p_AurEff->GetAmount());
                    l_Caster->CastCustomSpell(l_Caster, eSpells::CelestialFortuneHealEffect, &l_HealAmount, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_celestial_fortune_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_celestial_fortune_AuraScript();
        }
};

/// Last Update 7.0.3
/// Black Ox Brew - 115399
class spell_monk_black_ox_brew : public SpellScriptLoader
{
    public:
        spell_monk_black_ox_brew() : SpellScriptLoader("spell_monk_black_ox_brew") { }

        class spell_monk_black_ox_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_black_ox_brew_SpellScript);

            enum eSpells
            {
                IronSkinBrew = 115308
            };

            void HandleOnCast()
            {
                Player* l_Player = (GetCaster()) ? GetCaster()->ToPlayer() : nullptr;

                if (!l_Player)
                    return;

                if (SpellInfo const* IronSkinBrew = sSpellMgr->GetSpellInfo(eSpells::IronSkinBrew))
                {
                    while (l_Player->GetConsumedCharges(IronSkinBrew->ChargeCategoryEntry))
                        l_Player->RestoreCharge(IronSkinBrew->ChargeCategoryEntry);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_monk_black_ox_brew_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_black_ox_brew_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Special Delivery - 196734
class spell_monk_special_delivery_damage : public SpellScriptLoader
{
    public:
        spell_monk_special_delivery_damage() : SpellScriptLoader("spell_monk_special_delivery_damage") { }

        class spell_monk_special_delivery_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_special_delivery_damage_SpellScript);

            enum eSpells
            {
                SpecialDelivery = 196732
            };

            bool m_Hit = false;

            void HandleOnHit()
            {
                if (m_Hit)
                    return;

                m_Hit = true;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SpecialDelivery, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_special_delivery_damage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_special_delivery_damage_SpellScript();
        }
};

/// last update : 7.0.2
/// Blackout Combo - 196736
class spell_monk_blackout_combo : public SpellScriptLoader
{
    public:
        spell_monk_blackout_combo() : SpellScriptLoader("spell_monk_blackout_combo") { }

        class spell_monk_blackout_combo_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_blackout_combo_AuraScript);

            enum eSpells
            {
                BlackoutStrike = 205523
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfo)
            {
                PreventDefaultAction();

                if (p_ProcInfo.GetDamageInfo()->GetSpellInfo() && p_ProcInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::BlackoutStrike)
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, p_AurEff->GetTriggerSpell(), true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_blackout_combo_AuraScript::OnProc, EFFECT_4, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_blackout_combo_AuraScript();
        }
};

/// last update : 7.3.5
/// Zen Pulse - 124081
class spell_monk_zen_pulse : public SpellScriptLoader
{
    public:
        spell_monk_zen_pulse() : SpellScriptLoader("spell_monk_zen_pulse") { }

        class spell_monk_zen_pulse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_zen_pulse_SpellScript);

            enum eSpells
            {
                ZenPulseHeal = 198487
            };

            int32 m_HealAmount = 0;
            uint32 m_TargetCount = 0;
            uint32 m_DamageItr = 0;

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->IsValidAttackTarget(l_Target))
                {
                    PreventHitDamage();
                    return;
                }

                ++m_DamageItr;

                m_HealAmount += GetHitDamage();

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_HealTarget = l_Player->GetSelectedUnit();
                if (!l_HealTarget)
                    l_HealTarget = l_Caster;

                if (!l_Caster->IsValidAssistTarget(l_HealTarget))
                    l_HealTarget = l_Caster;

                if (!l_HealTarget)
                    return;

                if (m_DamageItr == m_TargetCount && m_HealAmount > 0)
                    l_Caster->CastCustomSpell(l_HealTarget, eSpells::ZenPulseHeal, &m_HealAmount, nullptr, nullptr, true);
            }

            void HandleTargetSelect(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || !p_Object->ToUnit() || !l_Caster->IsValidAttackTarget(p_Object->ToUnit()))
                        return true;

                    return false;
                });

                m_TargetCount = p_Targets.size();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_zen_pulse_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_zen_pulse_SpellScript::HandleTargetSelect, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_zen_pulse_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Zen Pulse - 198487
class spell_monk_zen_pulse_heal : public SpellScriptLoader
{
    public:
        spell_monk_zen_pulse_heal() : SpellScriptLoader("spell_monk_zen_pulse_heal") { }

        class spell_monk_zen_pulse_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_zen_pulse_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (GetSpell())
                    SetHitHeal(GetSpell()->GetSpellValue(SPELLVALUE_BASE_POINT0));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_zen_pulse_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_zen_pulse_heal_SpellScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Essence font - 191837
class spell_monk_essence_font_ticks : public SpellScriptLoader
{
    public:
        spell_monk_essence_font_ticks() : SpellScriptLoader("spell_monk_essence_font_ticks") { }

        class spell_monk_essence_font_ticks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_essence_font_ticks_AuraScript);

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!GetSpellInfo())
                    return;

                if (p_AuraEffect->GetTickNumber() % GetSpellInfo()->Effects[EFFECT_0].BasePoints == 1)
                    l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::EssenceFont].clear();
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::EssenceFont].clear();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_essence_font_ticks_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_essence_font_ticks_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_essence_font_ticks_AuraScript();
        }
};

/// Last update : 7.3.5 build 26365
/// Essence Font - 191840
class spell_monk_essence_font : public SpellScriptLoader
{
    public:
        spell_monk_essence_font() : SpellScriptLoader("spell_monk_essence_font") { }

        class spell_monk_essence_font_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_essence_font_SpellScript);

            enum eSpells
            {
                EssenceFontAura = 191837
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                std::list<uint64>& l_Targets = l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::EssenceFont];

                for (std::list<WorldObject*>::iterator l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    Unit* l_UnitTarget = (*l_Itr)->ToUnit();
                    if (!l_UnitTarget)
                    {
                        l_Itr = p_Targets.erase(l_Itr);
                        continue;
                    }

                    bool l_Erased = false;
                    for (uint64 l_HealedTargetGUID : l_Targets)
                    {
                        if (l_HealedTargetGUID == (*l_Itr)->GetGUID())
                        {
                            l_Itr = p_Targets.erase(l_Itr);
                            l_Erased = true;
                            break;
                        }
                    }

                    if (!l_Erased)
                        l_Itr++;
                }

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::HealthPctExcludeAuraOrderPredPlayer(GetSpellInfo()->Id, l_Caster->GetGUID()));
                p_Targets.resize(1);

                l_Targets.push_back(p_Targets.front()->GetGUID());
                if (l_Targets.size() > 5)
                    l_Targets.pop_front();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_essence_font_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_essence_font_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_essence_font_SpellScript();
        }

        class spell_monk_essence_font_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_essence_font_AuraScript);

            void OnUpdate(uint32 p_Diff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_MaxDurationWithPandemic = 10400; // 8sec + 30%

                if (GetDuration() > l_MaxDurationWithPandemic)
                    SetDuration(l_MaxDurationWithPandemic);
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                uint32 l_MaxDurationWithPandemic = 10400; // 8sec + 30%

                if (l_Aura->GetDuration() > l_MaxDurationWithPandemic)
                    l_Aura->SetDuration(l_MaxDurationWithPandemic);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_essence_font_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAPPLY);
                OnAuraUpdate += AuraUpdateFn(spell_monk_essence_font_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_essence_font_AuraScript();
        }
};

/// Last Update 7.0.3
/// Whirling Dragon Punch
class PlayerScript_whirling_dragon_punch : public PlayerScript
{
    public:
        PlayerScript_whirling_dragon_punch() :PlayerScript("PlayerScript_whirling_dragon_punch") {}

        uint32 m_Timer = 0;

        enum eSpells
        {
            FistsofFury = 113656,
            RisingSunKick = 107428,
            WirlingDragonActivate = 196742,
            WhirlingDragonPunch = 152175
        };

        void OnUpdate(Player* p_Player, uint32 p_Time)
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                return;

            if (!p_Player->HasSpell(eSpells::WhirlingDragonPunch))
                return;

            m_Timer += p_Time;

            if (m_Timer > 500)
            {

                SpellInfo const* l_RisingSunKick = sSpellMgr->GetSpellInfo(eSpells::RisingSunKick);
                if (l_RisingSunKick == nullptr)
                    return;
                if (p_Player->HasSpellCooldown(eSpells::FistsofFury) && p_Player->GetConsumedCharges(l_RisingSunKick->ChargeCategoryEntry))
                    p_Player->CastSpell(p_Player, eSpells::WirlingDragonActivate, true);
                m_Timer = 0;
            }
        }
};

/// Last Update 7.3.5 build 26365
/// Smoothing Mist - 193884
class spell_monk_soothing_mist : public SpellScriptLoader
{
    public:
        spell_monk_soothing_mist() : SpellScriptLoader("spell_monk_soothing_mist") { }

        class spell_monk_soothing_mist_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_soothing_mist_AuraScript);

            enum eSpells
            {
                LifeCooon               = 116849,
                Vivify                  = 116670,
                EnvelopingMist          = 124682,
                Effuse                  = 116694,
                SmoothingMistHeal       = 115175,
                AncientMistweaverArts   = 209520,
                SheilunsGift            = 205406,
                ShroudOfMist            = 199365,
                ShroudOfMistProc        = 214478
            };

            enum eArtifactPowers
            {
                ShroudOfMistArtifact    = 939
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Target = p_EventInfo.GetDamageInfo()->GetTarget();
                if (l_Target == nullptr)
                    return;

                Unit* l_Actor = p_EventInfo.GetActor();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Actor || l_Actor != l_Caster)
                    return;

                if (SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo())
                {
                    if ((l_ProcSpell->Id == eSpells::LifeCooon || l_ProcSpell->Id == eSpells::EnvelopingMist
                        || l_ProcSpell->Id == eSpells::Effuse || l_ProcSpell->Id == eSpells::SheilunsGift)
                        || (l_ProcSpell->Id == eSpells::Vivify && !l_Caster->HasAura(eSpells::AncientMistweaverArts)))
                    {
                        if (l_ProcSpell->Id == eSpells::Vivify)
                        {
                            uint64 l_TargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::SoothingMistTarget);
                            if (Unit* l_MainTarget = sObjectAccessor->FindUnit(l_TargetGUID))
                                l_Target = l_MainTarget;
                        }

                        l_Caster->CastSpell(l_Target, eSpells::SmoothingMistHeal, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_soothing_mist_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_soothing_mist_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Soothing Mist - 115175 - Soothing Mist(HonorTalent) 209525
class spell_monk_soothing_mist_heal : public SpellScriptLoader
{
    public:
        spell_monk_soothing_mist_heal() : SpellScriptLoader("spell_monk_soothing_mist_heal") { }

        class sspell_monk_soothing_mist_heal_AuraScript : public AuraScript
        {
            PrepareAuraScript(sspell_monk_soothing_mist_heal_AuraScript);

            enum eNpCs
            {
                JadeStatue = 60849
            };

            enum eSpells
            {
                SoothingMist        = 115175,
                SummonJadeStatue    = 115313,
                ShroudOfMist        = 199365,
                ShroudOfMistProc    = 214478,
                EnveloppingMist     = 124682,
            };

            enum eVisuals
            {
                ParticlesToTarget = 24208
            };

            enum eArtifactPowers
            {
                ShroudOfMistArtifact = 939
            };

            int32 m_BaseAmount = 0;

            Unit* GetStatueOfUnit(Unit* p_Caster) const
            {
                if (p_Caster == nullptr)
                    return nullptr;

                std::list<Creature*> l_TempList;
                std::list<Creature*> l_StatueList;
                Creature* l_Statue = nullptr;

                p_Caster->GetCreatureListWithEntryInGrid(l_TempList, eNpCs::JadeStatue, 100.0f);
                p_Caster->GetCreatureListWithEntryInGrid(l_StatueList, eNpCs::JadeStatue, 100.0f);

                /// Remove other players jade statue
                for (std::list<Creature*>::iterator i = l_TempList.begin(); i != l_TempList.end(); ++i)
                {
                    Unit* l_Owner = (*i)->GetOwner();
                    if (l_Owner && l_Owner->GetGUID() == p_Caster->GetGUID() && (*i)->isSummon())
                        continue;

                    l_StatueList.remove((*i));
                }

                if (l_StatueList.empty())
                    return nullptr;

                return l_StatueList.front();
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Target == nullptr || l_Caster == nullptr)
                    return;

                Unit* l_JadeStatue = GetStatueOfUnit(l_Caster);
                if (l_JadeStatue)
                {
                    l_JadeStatue->CastStop();
                    l_JadeStatue->CastSpell(l_Target, eSpells::SoothingMist, false);
                }

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::ShroudOfMist))
                {
                    SpellInfo const* l_ShroudOfMistInfo = sSpellMgr->GetSpellInfo(eSpells::ShroudOfMistProc);

                    if (l_ShroudOfMistInfo)
                    {
                        int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::ShroudOfMistArtifact);
                        int32 l_BP = l_Rank * l_ShroudOfMistInfo->Effects[EFFECT_0].BasePoints;

                        if (l_Rank == 3)
                            l_BP = -10;

                        l_Player->CastCustomSpell(l_Player, eSpells::ShroudOfMistProc, &l_BP, nullptr, nullptr, true);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAurasDueToSpell(214478);
                if (l_Caster->HasAura(eSpells::ShroudOfMistProc))
                    l_Caster->RemoveAura(eSpells::ShroudOfMistProc);
            }

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                m_BaseAmount = p_Amount;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                /// Will also channel Soothing Mist on your target, healing for 50% as much.
                if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::SummonJadeStatue, EFFECT_1))
                {
                    p_Amount = CalculatePct(p_Amount, l_AuraEffect->GetAmount());
                    m_BaseAmount = p_Amount;
                }
            }

            void HandleOnEffectPeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                {
                    SetDuration(1);
                    return;
                }

                float l_MaxRange = GetSpellInfo()->GetMaxRange();

                /// https://blue.mmo-champion.com/topic/766586-hotfixes-september-15/
                /// Soothing Mist cast from the Monk’s Jade Serpent Statue should cancel on targets farther than 60 yards from the statue.
                if (l_Caster->isStatue())
                    AddPct(l_MaxRange, 50.0f);

                if (l_Caster->GetDistance2d(l_Target) >= l_MaxRange || l_Caster->GetMap() != l_Target->GetMap())
                {
                    SetDuration(1);
                    return;
                }

                l_Caster->SendPlaySpellVisual(eVisuals::ParticlesToTarget, l_Target, 15.0f, l_Target->GetPosition(), false, false);

                Unit* l_Owner = l_Caster->GetCharmerOrOwnerOrSelf();
                if (!l_Owner)
                    return;

                if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::EnveloppingMist, EFFECT_1))
                    const_cast<AuraEffect*>(p_AurEff)->SetAmount(m_BaseAmount +  CalculatePct(m_BaseAmount, l_AuraEffect->GetAmount()));
                else
                    const_cast<AuraEffect*>(p_AurEff)->SetAmount(m_BaseAmount);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(sspell_monk_soothing_mist_heal_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
                AfterEffectApply += AuraEffectApplyFn(sspell_monk_soothing_mist_heal_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(sspell_monk_soothing_mist_heal_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(sspell_monk_soothing_mist_heal_AuraScript::HandleOnEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new sspell_monk_soothing_mist_heal_AuraScript();
        }
};

/// Last Update 7.0.3
/// Sheilun's Gift - 214483
class spell_monk_sheiluns_gift : public SpellScriptLoader
{
    public:
        spell_monk_sheiluns_gift() : SpellScriptLoader("spell_monk_sheiluns_gift") { }

        class spell_monk_sheiluns_gift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_sheiluns_gift_AuraScript);

            enum eSpells
            {
                SheilunsGift        = 214502,
                SheilunsGiftCloud   = 214501
            };

            void OnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (l_Caster->isInCombat())
                {
                    l_Caster->CastSpell(l_Caster, eSpells::SheilunsGift, true);
                    l_Caster->CastSpell(l_Caster, eSpells::SheilunsGiftCloud, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_sheiluns_gift_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_sheiluns_gift_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Sheilun's Gift - 205406
class spell_monk_sheiluns_gift_active : public SpellScriptLoader
{
    public:
        spell_monk_sheiluns_gift_active() : SpellScriptLoader("spell_monk_sheiluns_gift_active") { }

        class spell_impl : public SpellScript
        {
            PrepareSpellScript(spell_impl);

            enum eSpells
            {
                SheilunArtifactTracker  = 214502,
                SheilunsGiftCloud       = 214501,
                WhispersofShaohaoAura   = 238130,
                WhispersofShaohaoSpell  = 242400
            };

            enum eData
            {
                VisualId                = 56861
            };

            void HandleVisual()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<AreaTrigger*> l_Clouds;
                l_Caster->GetAreaTriggerList(l_Clouds, eSpells::SheilunsGiftCloud);

                for (AreaTrigger* l_Areatrigger : l_Clouds)
                    l_Caster->PlayOrphanSpellVisual(l_Areatrigger->AsVector3(), G3D::Vector3(180, 0, 0), G3D::Vector3(0, 0, 0), eData::VisualId, 1.5f, l_Caster->GetGUID(), true, 0.0f);
            }

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                int32 l_Heal = GetHitHeal();
                if (!l_Caster)
                    return;

                /// handle stacks of clouds
                if (Aura* const l_Aura = l_Caster->GetAura(eSpells::SheilunArtifactTracker))
                {
                    ///< Whispers of Shaohao
                    if (l_Caster->HasAura(eSpells::WhispersofShaohaoAura))
                    {
                        uint32 l_StackAmount = l_Aura->GetStackAmount();
                        for (uint32 l_Itr = 0; l_Itr < l_StackAmount; l_Itr++)
                             l_Caster->CastSpell(l_Caster, eSpells::WhispersofShaohaoSpell, true);
                    }

                    SetHitHeal(l_Heal * l_Aura->GetStackAmount());
                    l_Aura->Remove();
                }

                std::list<AreaTrigger*> l_Clouds;
                l_Caster->GetAreaTriggerList(l_Clouds, eSpells::SheilunsGiftCloud);

                for (AreaTrigger* l_Areatrigger : l_Clouds)
                    l_Areatrigger->Despawn();
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_impl::HandleVisual);
                OnEffectHitTarget += SpellEffectFn(spell_impl::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_impl();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Serenity - 152173
class spell_monk_serenity : public SpellScriptLoader
{
    public:
        spell_monk_serenity() : SpellScriptLoader("spell_monk_serenity") { }

        class spell_monk_serenity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_serenity_AuraScript);

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

                l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() * 0.40f); ///< Serenity is reduced by 60% in PvP
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_serenity_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_monk_serenity_AuraScript::HandleAfterApply, EFFECT_2, SPELL_AURA_MOD_HEALING_DONE_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_serenity_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Tansfer the power  - 195321
class spell_monk_transfer_the_power : public SpellScriptLoader
{
    public:
        spell_monk_transfer_the_power() : SpellScriptLoader("spell_monk_transfer_the_power") { }

        class spell_monk_transfer_the_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_transfer_the_power_AuraScript);

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

                /// Transfer the Power reduced by 40 % in PvP.
                int32 l_NewAmount = l_SpellInfo->Effects[EFFECT_0].BasePoints * GetStackAmount() * 0.60f;
                l_AuraEffect->ChangeAmount(l_NewAmount);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_transfer_the_power_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_transfer_the_power_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Transcendence : Transfer - 119996
class spell_monk_transcendence_transfer: public SpellScriptLoader
{
    public:
        spell_monk_transcendence_transfer() : SpellScriptLoader("spell_monk_transcendence_transfer") { }

        class spell_monk_transcendence_transfer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_transcendence_transfer_SpellScript);

            SpellCastResult CheckSpiritRange()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    for (Unit::ControlList::const_iterator l_Itr = l_Caster->m_Controlled.begin(); l_Itr != l_Caster->m_Controlled.end(); ++l_Itr)
                    {
                        if ((*l_Itr)->GetEntry() == 54569)
                        {
                            if ((*l_Itr)->GetDistance(l_Caster) > 40.0f)
                                return SPELL_FAILED_DONT_REPORT;
                        }
                    }
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    for (Unit::ControlList::const_iterator l_Itr = l_Caster->m_Controlled.begin(); l_Itr != l_Caster->m_Controlled.end(); ++l_Itr)
                    {
                        if ((*l_Itr)->GetEntry() == 54569)
                        {
                            Creature* l_Clone = (*l_Itr)->ToCreature();
                            if (l_Clone && l_Clone->AI())
                                l_Clone->AI()->DoAction(1);
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_transcendence_transfer_SpellScript::CheckSpiritRange);
                OnHit += SpellHitFn(spell_monk_transcendence_transfer_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_transcendence_transfer_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Transcendence - 101643
class spell_monk_transcendence : public SpellScriptLoader
{
    public:
        spell_monk_transcendence() : SpellScriptLoader("spell_monk_transcendence") { }

        enum eData
        {
            TranscendenceSpirit = 54569,
            DisableCast         = 62388
        };

        class spell_monk_transcendence_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_transcendence_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Player* l_Player = GetCaster()->ToPlayer();
                if (l_Player == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;;

                return SPELL_CAST_OK;
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon && l_Summon->GetEntry() == eData::TranscendenceSpirit)
                    {
                        if (Creature* l_Creature = l_Summon->ToCreature())
                        {
                            l_Creature->DespawnOrUnsummon();
                            break;
                        }
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                /// Glyph of fighting pose 125872
                if (l_Caster->HasAura(125872))
                {
                    for (Unit* l_Summon : l_Caster->m_Controlled)
                    {
                        if (l_Summon && l_Summon->GetEntry() == eData::TranscendenceSpirit)
                        {
                            if (Creature* l_Creature = l_Summon->ToCreature())
                            {
                                l_Creature->HandleEmoteCommand(27);
                                break;
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_transcendence_SpellScript::CheckCast);
                BeforeCast += SpellCastFn(spell_monk_transcendence_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_monk_transcendence_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_transcendence_SpellScript();
        }

        class spell_monk_transcendence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_transcendence_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (Unit::ControlList::const_iterator l_Itr = l_Caster->m_Controlled.begin(); l_Itr != l_Caster->m_Controlled.end(); ++l_Itr)
                {
                    if ((*l_Itr)->GetEntry() == eData::TranscendenceSpirit)
                    {
                        if (AuraApplication* l_Transcendence = l_Caster->GetAuraApplication(GetSpellInfo()->Id, l_Caster->GetGUID()))
                        {
                            if ((*l_Itr)->GetDistance(l_Caster) > 40.0f)
                                l_Transcendence->SendFakeAuraUpdate(eData::DisableCast, false);
                            else
                                l_Transcendence->SendFakeAuraUpdate(eData::DisableCast, true);

                            break;
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_transcendence_AuraScript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_transcendence_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Effuse - 116694 227344
class spell_monk_effuse : public SpellScriptLoader
{
    public:
        spell_monk_effuse() : SpellScriptLoader("spell_monk_effuse") { }

        class spell_monk_effuse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_effuse_SpellScript);

            enum eSpells
            {
                ControlTheMistsAura = 233954,
                ControlTheMistsCD   = 233766,
                SheilunsGift        = 214502,
                SheilunsGiftCloud   = 214501,
                EffusiveMists       = 238094
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::ControlTheMistsAura))
                {
                    l_Caster->RemoveAura(eSpells::ControlTheMistsAura);
                    l_Caster->CastSpell(l_Caster, eSpells::ControlTheMistsCD, true);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::EffusiveMists))
                {
                    if (l_Caster->isInCombat())
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::SheilunsGift, true);
                        l_Caster->CastSpell(l_Caster, eSpells::SheilunsGiftCloud, true);
                    }
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_monk_effuse_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_monk_effuse_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_effuse_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Healing Winds - 195380
class spell_monk_healing_winds : public SpellScriptLoader
{
    public:
        spell_monk_healing_winds() : SpellScriptLoader("spell_monk_healing_winds") { }

        class spell_monk_healing_winds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_healing_winds_AuraScript);

            enum eSpells
            {
                TranscendanceTransfer   = 119996,
                HealingWindsHeal        = 195381
            };

            enum eArtifactPowerId
            {
                HealingWinds            = 829
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Player || !l_SpellInfo || l_SpellInfo->Id != eSpells::TranscendanceTransfer)
                    return;

                uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowerId::HealingWinds);
                float l_Pct = p_AurEff->GetBaseAmount();
                l_Pct += (float(l_Rank) - 1.0f) * 5.0f;

                if (l_Player->CanApplyPvPSpellModifiers())
                    l_Pct *= 0.5f; ///< 50% in PvP situations

                int32 l_HealingAmount = CalculatePct(l_Player->GetMaxHealth(), l_Pct);

                l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HealingWindsHeal);
                if (!l_SpellInfo)
                    return;

                l_HealingAmount /= l_SpellInfo->GetMaxTicks();
                l_Player->CastCustomSpell(l_Player, eSpells::HealingWindsHeal, &l_HealingAmount, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_healing_winds_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_healing_winds_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Rushing Jade Wind - 116847
/// Called for Storm Earth and Fire Spirits (SEF id: 137639)
class spell_monk_rushing_jade_wind_spirits_copy : public SpellScriptLoader
{
    public:
        spell_monk_rushing_jade_wind_spirits_copy() : SpellScriptLoader("spell_monk_rushing_jade_wind_spirits_copy") { }

        class spell_monk_rushing_jade_wind_spirits_copy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_rushing_jade_wind_spirits_copy_SpellScript);

            enum eEntry
            {
                SEFfireSpirit = 69791,
                SEFearthSpirit = 69792
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->getClass() == Classes::CLASS_MONK)
                {
                    for (Unit* l_Summon : l_Player->m_Controlled)
                    {
                        if (!l_Summon || (l_Summon->GetEntry() != eEntry::SEFearthSpirit && l_Summon->GetEntry() != eEntry::SEFfireSpirit))
                            continue;

                        l_Summon->CastSpell(l_Summon, m_scriptSpellId, true);
                    }
                }

            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_monk_rushing_jade_wind_spirits_copy_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_rushing_jade_wind_spirits_copy_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Chi Burst - 123986
/// Called for Storm Earth and Fire Spirits (SEF id: 137639)
class spell_monk_chi_burst_spirits_copy : public SpellScriptLoader
{
    public:
        spell_monk_chi_burst_spirits_copy() : SpellScriptLoader("spell_monk_chi_burst_spirits_copy") { }

        class spell_monk_chi_burst_spirits_copy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_chi_burst_spirits_copy_SpellScript);

            enum eEntry
            {
                SEFfireSpirit       = 69791,
                SEFearthSpirit      = 69792,
            };

            enum eSpells
            {
                HitComboAura = 196740,
                HitCombo     = 196741
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = GetExplTargetUnit();

                if (l_Player->getClass() == Classes::CLASS_MONK)
                {
                    for (Unit* l_Summon : l_Player->m_Controlled)
                    {
                        if (!l_Summon || (l_Summon->GetEntry() != eEntry::SEFearthSpirit && l_Summon->GetEntry() != eEntry::SEFfireSpirit))
                            continue;

                        l_Summon->CastSpell(l_Target ? l_Target : l_Summon, m_scriptSpellId, true);
                    }
                }

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) != m_scriptSpellId)
                    l_Caster->CastSpell(l_Caster, eSpells::HitCombo, true);

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) = m_scriptSpellId;
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_monk_chi_burst_spirits_copy_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_chi_burst_spirits_copy_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Spinning Crane Kick - 101546
/// Called for Storm Earth and Fire Spirits (SEF id: 137639)
class spell_monk_spinning_crane_kick_spirits_copy : public SpellScriptLoader
{
    public:
        spell_monk_spinning_crane_kick_spirits_copy() : SpellScriptLoader("spell_monk_spinning_crane_kick_spirits_copy") { }

        class spell_monk_spinning_crane_kick_spirits_copy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_spinning_crane_kick_spirits_copy_SpellScript);

            enum eEntry
            {
                SEFfireSpirit   = 69791,
                SEFearthSpirit  = 69792
            };

            enum eSpells
            {
                HitComboTalent  = 196740,
                HitComboBuff    = 196741
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->getClass() == Classes::CLASS_MONK)
                {
                    for (Unit* l_Summon : l_Player->m_Controlled)
                    {
                        if (!l_Summon || (l_Summon->GetEntry() != eEntry::SEFearthSpirit && l_Summon->GetEntry() != eEntry::SEFfireSpirit))
                            continue;

                        l_Summon->CastSpell(l_Summon, m_scriptSpellId, true);
                    }
                }

                if (l_Player->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) != m_scriptSpellId)
                {
                    if (l_Caster->HasAura(eSpells::HitComboTalent))
                        l_Caster->CastSpell(l_Caster, eSpells::HitComboBuff, true);

                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::SpinningCraneKickMastery) = true;
                }
                else
                {
                    l_Caster->RemoveAura(eSpells::HitComboBuff);
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::SpinningCraneKickMastery) = false;
                }

                l_Player->m_SpellHelper.GetUint32(eSpellHelpers::HitComboLastSpellId) = m_scriptSpellId;
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_monk_spinning_crane_kick_spirits_copy_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_spinning_crane_kick_spirits_copy_SpellScript();
        }
};


/// Last Update 7.0.3 Build 22522
/// Called by Storm, Earth, and Fire - 137639
class spell_monk_storm_earth_and_fire : public SpellScriptLoader
{
    public:
        spell_monk_storm_earth_and_fire() : SpellScriptLoader("spell_monk_storm_earth_and_fire") { }

        class spell_monk_storm_earth_and_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_storm_earth_and_fire_SpellScript);

            enum eSpells
            {
                StormSpiritVisual   = 138080,
                SummonEarthSpirit   = 138121,
                SummonFireSpirit    = 138123,
                StormEarthAndFire   = 137639
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Victim = l_Caster->getVictim();

                l_Caster->CastSpell(l_Caster, eSpells::SummonFireSpirit, true);
                l_Caster->CastSpell(l_Caster, eSpells::SummonEarthSpirit, true);
                l_Caster->CastSpell(l_Caster, eSpells::StormSpiritVisual, true);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->IsValidAttackTarget(l_Target))
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::StormEarthAndFire);
                if (!l_Aura)
                    return;

                l_Aura->SetScriptGuid(0, l_Target->GetGUID());
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_storm_earth_and_fire_SpellScript::HandleAfterCast);
                AfterHit += SpellHitFn(spell_monk_storm_earth_and_fire_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_storm_earth_and_fire_SpellScript();
        }

        class spell_monk_storm_earth_and_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_storm_earth_and_fire_AuraScript);

            enum eSpells
            {
                StormEarthAndFire           = 137639,
                StormSpiritVisual           = 138080,
                TigerPalm                   = 100780,
                BlackoutKick                = 100784,
                BlackoutKickBuff            = 116768,
                FistsOfFury                 = 113656,
                FistsOfFuryDamage           = 117418,
                FistsOfFuryVisual           = 123154,
                RisingSunKick               = 107428,
                RisingSunKickDamage         = 185099,
                StrikeOfTheWindlord         = 205320,
                StrikeOfTheWindlordMain     = 222029,
                StrikeOfTheWindlordSecond   = 205414,
                CracklingJadeLightning      = 117952,
                WhirlingDragonPunch         = 152175,
                WhirlingDragonPunchDamage   = 158221,
                ChiWave                     = 115098,
                T21Windwalker4P             = 251821
            };

            enum eDataType
            {
                DataSpellId = 0
            };

            uint64 m_SpiritTargetGuid   = 0;
            uint32 m_SpellId            = 0;
            uint64 m_TimeofLastStrike   = 0;

            std::map<uint32, std::pair<bool, bool>> m_CastedSpells;

            void SetGuid(uint32 /*p_Index*/, uint64 p_GUID) override
            {
                if (!p_GUID)
                    return;

                m_SpiritTargetGuid = p_GUID;
            }

            void SetData(uint32 p_Type, uint32 p_SpellId) override
            {
                switch (static_cast<eDataType>(p_Type))
                {
                    case eDataType::DataSpellId:
                        m_SpellId = p_SpellId;
                        break;
                    default:
                        m_CastedSpells[p_Type] = { false, false };
                        break;
                }
            }

            void HandleDamageCopy(int32 p_Damage, int32 p_SpellID)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /*if (l_Caster->CanApplyPvPSpellModifiers())
                    p_Damage *= 0.90f; ///< Storm, Earth and Fire damage is reduced by 10% in PvP*/

                uint32 l_SpellID = p_SpellID;
                uint64 l_SpiritTargetGUID = m_SpiritTargetGuid;
                std::map<uint32, std::pair<bool, bool>> l_CastedSpells = m_CastedSpells;
                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                Guid128 l_CastGuid = l_Aura->GetCastGUID();
                uint32 l_ScriptId = m_scriptSpellId;

                uint64 l_CasterGuid = l_Caster->GetGUID();
                if (!l_Caster->IsPlayer())
                    return;

                l_Caster->AddDelayedEvent([l_CasterGuid, l_SpellID, l_SpiritTargetGUID, p_Damage, this, l_CastedSpells, l_CastGuid, l_ScriptId]() -> void
                {
                    Player* l_Player = sObjectAccessor->FindPlayer(l_CasterGuid);
                    if (!l_Player)
                        return;

                    Aura* l_Aura = l_Player->GetAura(l_ScriptId);
                    if (!l_Aura || l_Aura->GetCastGUID() != l_CastGuid)
                        return;

                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_SpellID);
                    if (!l_Player || !l_SpellInfo)
                        return;

                    Unit* l_Target = nullptr;
                    if (l_SpellInfo->IsPositive())
                    {
                        Spell const* l_Spell = l_Player->GetCurrentSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                        if (l_Spell)
                            l_Target = l_Spell->GetUnitTarget();
                    }
                    else
                        l_Target = sObjectAccessor->GetUnit(*l_Player, l_SpiritTargetGUID);

                    if (!l_Target)
                    {
                        switch (l_SpellID)
                        {
                            case eSpells::WhirlingDragonPunch:
                                break;
                            default:
                                return;
                        }
                    }

                    std::set<Unit*> l_Spirits;

                    std::set<uint64>& l_EarthAndFireSpirits = l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::EarthAndFireSpirit];
                    for (uint64 l_SpiritGUID : l_EarthAndFireSpirits)
                    {
                        Unit* l_Spirit = sObjectAccessor->GetUnit(*l_Player, l_SpiritGUID);
                        if (!l_Spirit)
                            continue;

                        l_Spirits.insert(l_Spirit);
                    }

                    if (l_SpellID == eSpells::FistsOfFury)
                    {
                        if (Aura* l_FistOfFury = l_Target->GetAura(l_SpellID))
                        {
                            if (l_FistOfFury->GetCaster() && (l_FistOfFury->GetCaster()->GetOwner() == l_Player))
                                return;
                        }
                    }

                    for (Unit* l_Spirit : l_Spirits)
                    {
                        switch (l_SpellID)
                        {
                            case eSpells::WhirlingDragonPunch:
                                l_Target = l_Spirit;
                                break;
                            default:
                                break;
                        }

                        if (!l_Target)
                            continue;

                        Aura* l_StormEarthAndFire = l_Player->GetAura(eSpells::StormEarthAndFire);
                        if (!l_StormEarthAndFire)
                            continue;

                        auto l_Itr = l_CastedSpells.find(l_SpellID);
                        if (l_Itr != l_CastedSpells.end())
                        {
                            switch (l_Spirit->GetEntry())
                            {
                                case eMonkPets::Earth:
                                {
                                    if ((*l_Itr).second.first == true)
                                        continue;

                                    m_CastedSpells[l_SpellID].first = true;
                                    break;
                                }
                                case eMonkPets::Fire:
                                {
                                    if ((*l_Itr).second.second == true)
                                        continue;

                                    m_CastedSpells[l_SpellID].second = true;
                                    break;
                                }
                                default:
                                    break;
                            }
                        }

                        l_Spirit->InterruptNonMeleeSpells(true);
                        l_Spirit->CastSpell(l_Target, l_SpellID, true);

                        if (!l_Spirit->ToCreature())
                            continue;

                        l_Spirit->ToCreature()->m_CreatureCategoryCooldowns.clear();
                        l_Spirit->ToCreature()->m_CreatureSpellCooldowns.clear();
                    }
                }, 10);
            }

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                Unit* l_Caster = GetCaster();
                int32 l_Damage = l_DamageInfo->GetAmount();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Target = GetTarget();
                if (!l_SpellInfo || l_SpellInfo->SpellFamilyName != SpellFamilyNames::SPELLFAMILY_MONK || l_SpellInfo->IsPassive())
                    return false;

                int32 l_SpellID = l_SpellInfo->Id;

                switch (l_SpellID)
                {
                    case eSpells::TigerPalm:
                    case eSpells::FistsOfFury:
                    case eSpells::RisingSunKick:
                    case eSpells::CracklingJadeLightning:
                    case eSpells::WhirlingDragonPunch:
                    case eSpells::ChiWave:
                        HandleDamageCopy(l_Damage, l_SpellID);
                        break;
                    case eSpells::StrikeOfTheWindlord:
                        if (getMSTime() - m_TimeofLastStrike > 5000)
                        {
                            HandleDamageCopy(l_Damage, l_SpellID);
                            m_TimeofLastStrike = getMSTime();
                        }
                        break;
                    case eSpells::BlackoutKick:
                        HandleDamageCopy(l_Damage, l_SpellID);
                        if (l_Caster && l_Caster->HasAura(eSpells::BlackoutKickBuff) && l_Caster->HasAura(eSpells::T21Windwalker4P)) ///< Since the buff is consumed, need to save it here.
                            l_Caster->m_SpellHelper.GetBool(eSpellHelpers::T21Windwalker4P) = true;
                        else
                            l_Caster->m_SpellHelper.GetBool(eSpellHelpers::T21Windwalker4P) = false;
                    default:
                        break;
                }

                return false;
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::StormSpiritVisual);

                std::set<uint64>& l_EarthAndFireSpirits = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::EarthAndFireSpirit];
                for (uint64 l_SpiritGUID : l_EarthAndFireSpirits)
                {
                    Unit* l_Spirit = sObjectAccessor->GetUnit(*l_Caster, l_SpiritGUID);
                    if (!l_Spirit)
                        continue;

                    Creature* l_CreatureSpirit = l_Spirit->ToCreature();
                    if (!l_CreatureSpirit)
                        continue;

                    l_CreatureSpirit->DespawnOrUnsummon(1);
                }

                l_EarthAndFireSpirits.clear();
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_storm_earth_and_fire_AuraScript::HandleOnProc);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_storm_earth_and_fire_AuraScript::HandleOnRemove, EFFECT_4, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_storm_earth_and_fire_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called for Storm, Earth, and Fire - 137639
class PlayerScript_Storm_Earth_And_Fire: public PlayerScript
{
    public:
        PlayerScript_Storm_Earth_And_Fire() :PlayerScript("PlayerScript_Storm_Earth_And_Fire") {}

        enum eSpells
        {
            StormEarthAndFire       = 137639,
        };

        void OnSpellCast(Player* p_Player, Spell* p_Spell, bool p_SkipCheck)
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                return;

            if (p_Spell->getTriggerCastFlags() & TRIGGERED_FULL_MASK)
                return;

            Aura* l_Aura = p_Player->GetAura(eSpells::StormEarthAndFire);
            SpellInfo const* l_SpellInfo = p_Spell->GetSpellInfo();
            if (!l_Aura)
                return;

            l_Aura->SetScriptData(0, l_SpellInfo->Id);
            l_Aura->SetScriptData(l_SpellInfo->Id, static_cast<uint32>(true));
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Storm, Earth, and Fire (Fixate) - 221771
class spell_monk_storm_earth_and_fire_fixate : public SpellScriptLoader
{
    public:
        spell_monk_storm_earth_and_fire_fixate() : SpellScriptLoader("spell_monk_storm_earth_and_fire_fixate") { }

        class spell_monk_storm_earth_and_fire_fixate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_storm_earth_and_fire_fixate_SpellScript);

            enum eSpells
            {
                StormEarthAndFire = 137639
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->IsValidAttackTarget(l_Target))
                    return;

                std::set<uint64>& l_EarthAndFireSpirits = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::EarthAndFireSpirit];
                for (uint64 l_SpritGUID : l_EarthAndFireSpirits)
                {
                    Unit* l_Spirit = sObjectAccessor->GetUnit(*l_Caster, l_SpritGUID);
                    if (!l_Spirit)
                        continue;

                    Creature* l_SpiritCreature = l_Spirit->ToCreature();
                    if (!l_SpiritCreature)
                        continue;

                    UnitAI* l_SpiritAI = l_SpiritCreature->GetAI();
                    if (!l_SpiritAI)
                        continue;

                    l_SpiritAI->SetGUID(l_Target->GetGUID());
                }

                Aura* l_Aura = l_Caster->GetAura(eSpells::StormEarthAndFire);
                if (!l_Aura)
                    return;

                l_Aura->SetScriptGuid(0, l_Target->GetGUID());
            }

            void Register() override
            {
                AfterCast += SpellCastFn (spell_monk_storm_earth_and_fire_fixate_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_storm_earth_and_fire_fixate_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Thunder Focus Tea - 116680
/// Called for Zen Focus - 209583
class spell_monk_zen_focus : public SpellScriptLoader
{
    public:
        spell_monk_zen_focus() : SpellScriptLoader("spell_monk_zen_focus") { }

        class spell_monk_zen_focus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_zen_focus_AuraScript);

            enum eSpells
            {
                ZenFocus                    = 209583,
                ZenFocusEffect              = 209584,
                TheBlackFlamesGamble        = 216506,

                SpellsNumber                = 5,
                Vivify                      = 116670,
                RenewingMist                = 115151,
                Effuse                      = 116694,
                EnveloppingMist             = 124682,
                EssenceFont                 = 191837
            };

            std::array<uint32, eSpells::SpellsNumber> Spells =
            { {
                    eSpells::Vivify,
                    eSpells::RenewingMist,
                    eSpells::Effuse,
                    eSpells::EnveloppingMist,
                    eSpells::EssenceFont
                } };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::ZenFocus))
                    l_Caster->CastSpell(l_Caster, eSpells::ZenFocusEffect, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_zen_focus_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_zen_focus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Healing Sphere - 205234
class spell_monk_healing_sphere : public SpellScriptLoader
{
    public:
        spell_monk_healing_sphere() : SpellScriptLoader("spell_monk_healing_sphere") { }

        class spell_monk_healing_sphere_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_healing_sphere_SpellScript);

            enum eSpells
            {
                HealingSphereAreatrigger = 202531,
                HealingSphere            = 205234
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Caster || !l_Dest)
                    return;

                SpellInfo const* l_HealingSphere = sSpellMgr->GetSpellInfo(eSpells::HealingSphere);
                if (!l_HealingSphere)
                    return;

                l_Caster->DelayedCastSpell(l_Dest, eSpells::HealingSphereAreatrigger, true, 1.5 * IN_MILLISECONDS);

                AreaTrigger* l_SphereToRemove = nullptr;
                uint32 l_CurDuration = 0;

                std::list<AreaTrigger*> l_Spheres;
                l_Caster->GetAreaTriggerList(l_Spheres, eSpells::HealingSphereAreatrigger);

                if (l_Spheres.size() < l_HealingSphere->Effects[EFFECT_0].BasePoints)
                    return;

                for (AreaTrigger* l_Areatrigger : l_Spheres)
                {
                    if (l_CurDuration == 0 || l_Areatrigger->GetDuration() <  l_CurDuration)
                    {
                        l_SphereToRemove = l_Areatrigger;
                        l_CurDuration = l_Areatrigger->GetDuration();
                    }
                }

                if (l_SphereToRemove)
                    l_SphereToRemove->Remove(0);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_healing_sphere_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_healing_sphere_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Healing Sphere - 115464
class spell_monk_healing_sphere_cleanse : public SpellScriptLoader
{
    public:
        spell_monk_healing_sphere_cleanse() : SpellScriptLoader("spell_monk_healing_sphere_cleanse") { }

        class spell_monk_healing_sphere_cleanse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_healing_sphere_cleanse_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                Unit::AuraApplicationMap& l_AppliedAuras = l_Target->GetAppliedAuras();
                for (Unit::AuraApplicationMap::iterator l_Itr = l_AppliedAuras.begin(); l_Itr != l_AppliedAuras.end();)
                {
                    if (Aura* l_Aura = l_Itr->second->GetBase())
                    {
                        if (SpellInfo const* l_SpellInfo = l_Aura->GetSpellInfo())
                        {
                            if (SpellCategoriesEntry const* l_SpellCategoriesEntry = l_SpellInfo->GetSpellCategories())
                            {
                                if (l_SpellInfo->IsPeriodic() && !l_SpellInfo->IsPositive() && l_SpellCategoriesEntry->DmgClass == SpellDmgClass::SPELL_DAMAGE_CLASS_MAGIC)
                                {
                                    l_Target->RemoveAura(l_Itr);
                                    continue;
                                }
                            }
                        }
                    }

                    ++l_Itr;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_healing_sphere_cleanse_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_healing_sphere_cleanse_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Called by Guard - 202162
class spell_monk_guard : public SpellScriptLoader
{
    public:
        spell_monk_guard() : SpellScriptLoader("spell_monk_guard") { }

        class spell_monk_guard_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_guard_AuraScript);

            void OnCalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                /// Redirect 30% of damages to the caster with Stagger
                int32 l_Damage  = p_DmgInfo.GetAmount();
                int32 l_Stagger = 0;

                l_Stagger = CalculatePct(l_Damage, GetSpellInfo()->Effects[EFFECT_1].BasePoints);

                p_AbsorbAmount = l_Stagger;

                l_Caster->CalcStaggerDamage(l_Caster->ToPlayer(), l_Stagger, SpellSchoolMask::SPELL_SCHOOL_MASK_NONE, nullptr, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_guard_AuraScript::OnCalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_monk_guard_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_guard_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Tiger Palm - 100780 - Blackout Kick - 100784, 228649
/// Called for Teachings of the Monastery - 116645
class spell_monk_teachings_of_the_monastery : public SpellScriptLoader
{
    public:
        spell_monk_teachings_of_the_monastery() : SpellScriptLoader("spell_monk_teachings_of_the_monastery") { }

        class spell_monk_teachings_of_the_monastery_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_teachings_of_the_monastery_SpellScript);

            enum eSpells
            {
                TeachingsOfTheMonastery         = 116645,
                TeachingsOfTheMonasteryStack    = 202090,
                TigerPalm                       = 100780,
                BlackoutKick                    = 100784,
                AdditionnalBlackoutKick         = 228649,
                RisingSunKick                   = 107428,
                SpiritOfTheCrane                = 210802,
                SpiritOfTheCraneProc            = 210803

            };

            void HandleTigerPalm()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::TeachingsOfTheMonastery))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::TeachingsOfTheMonasteryStack, true);
            }

            void HandleBlackoutKick()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Player || !l_Target || !l_Player->HasAura(eSpells::TeachingsOfTheMonastery))
                    return;

                int32 l_Chance = (sSpellMgr->GetSpellInfo(eSpells::TeachingsOfTheMonastery))->Effects[EFFECT_0].BasePoints;
                SpellInfo const* l_RisingSunKick = sSpellMgr->GetSpellInfo(eSpells::RisingSunKick);
                if (!l_Chance || !l_RisingSunKick)
                    return;

                if (roll_chance_i(l_Chance))
                    l_Player->RestoreCharge(l_RisingSunKick->ChargeCategoryEntry);

                if (m_scriptSpellId != eSpells::BlackoutKick)
                    return;

                Aura* l_Aura = l_Player->GetAura(eSpells::TeachingsOfTheMonasteryStack, l_Player->GetGUID());
                if (l_Aura)
                {
                    uint8 l_StackAmount = l_Aura->GetStackAmount();
                    if (l_StackAmount != 0)
                    {
                        for (uint8 i = 0; i < l_StackAmount; ++i)
                        {
                            l_Player->CastSpell(l_Target, eSpells::AdditionnalBlackoutKick);
                            if (l_Player->HasAura(eSpells::SpiritOfTheCrane))
                                l_Player->CastSpell(l_Player, eSpells::SpiritOfTheCraneProc, true);
                        }

                        l_Player->RemoveAurasDueToSpell(eSpells::TeachingsOfTheMonasteryStack);
                    }
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::TigerPalm:
                        AfterCast += SpellCastFn(spell_monk_teachings_of_the_monastery_SpellScript::HandleTigerPalm);
                        break;
                    case eSpells::BlackoutKick:
                    case eSpells::AdditionnalBlackoutKick:
                        AfterCast += SpellCastFn(spell_monk_teachings_of_the_monastery_SpellScript::HandleBlackoutKick);
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_teachings_of_the_monastery_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26124
/// Strike of the Windlord - 205414, 222029
class spell_monk_strike_of_the_windlord : public SpellScript
{
    PrepareSpellScript(spell_monk_strike_of_the_windlord);

    enum eSpells
    {
        StrikeOfTheWindLord = 205320,
        StrikeOfTheWindlordMain = 222029,
        StrikeOfTheWindLordSecond = 205414,
        ComboStrikes = 115636,
        TheWindBlows = 248101,
        BlackoutKickFree = 116768,
        PvPRulesEnables = 134735,
        ThunderfistTrait = 238131,
        ThunderfistBuff = 242387
    };

    enum eEntry
    {
        SEFfireSpirit = 69791,
        SEFearthSpirit = 69792
    };

    void FilterTargets(std::list<WorldObject*>& p_Targets)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        float l_Radius = 2.0f;
        std::list<Unit*> l_Targets;
        JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_Targets, l_Check);
        l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

        for (auto l_Target : l_Targets)
        {
            if (l_Target && std::find(p_Targets.begin(), p_Targets.end(), l_Target) == p_Targets.end() && l_Caster->IsValidAttackTarget(l_Target))
                p_Targets.push_back(l_Target);
        }

        m_Targets = p_Targets.size();
    }

    void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
    {
        Unit* l_Caster = GetCaster();
        Unit* l_Target = GetHitUnit();

        if (!l_Caster || !l_Target)
            return;

        Player* l_Monk = l_Caster->GetSpellModOwner();
        if (!l_Monk)
            return;

        if (l_Monk->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
            return;

        Unit* l_MainTarget = l_Monk->GetSelectedUnit();

        if (m_scriptSpellId == eSpells::StrikeOfTheWindlordMain)
        {
            if (l_Caster->HasAura(eSpells::ThunderfistTrait))
            {
                if (Aura* l_ThunderfistAura = l_Caster->AddAura(eSpells::ThunderfistBuff, l_Caster))
                {
                    if (l_MainTarget == l_Target)
                        l_ThunderfistAura = l_Caster->AddAura(eSpells::ThunderfistBuff, l_Caster);

                    l_ThunderfistAura->SetMaxDuration(15 * IN_MILLISECONDS);
                    l_ThunderfistAura->SetDuration(15 * IN_MILLISECONDS);
                }
            }
        }

        if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ComboStrikes, EFFECT_0))
        {
            uint32 l_PrevSpell = l_Caster->ToPlayer()->GetLastCastedSpell();
            int32 p_Damage = GetHitDamage();
            if (l_Caster->ToPlayer()->IsInDuel() && l_Target->IsPlayer() && !l_Target->HasAura(eSpells::PvPRulesEnables))
                p_Damage = 0;

            p_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), p_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TheWindBlows);
            if (!l_SpellInfo)
                return;

            if (m_scriptSpellId == eSpells::StrikeOfTheWindlordMain && l_Caster->HasAura(eSpells::TheWindBlows))
                l_Caster->CastSpell(l_Caster, eSpells::BlackoutKickFree, true);

            if (!l_Caster->IsValidAttackTarget(l_Target))
                p_Damage = 0;

            if (l_MainTarget != l_Target && m_Targets)
                p_Damage /= m_Targets;

            SetHitDamage(p_Damage);

            if (l_Caster->GetEntry() == eEntry::SEFearthSpirit || l_Caster->GetEntry() == eEntry::SEFfireSpirit)
            {
                int32 p_Damage = GetHitDamage();
                AddPct(p_Damage, l_AuraEffect->GetAmount());
                SetHitDamage(p_Damage);
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_strike_of_the_windlord::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST);
        OnEffectHitTarget += SpellEffectFn(spell_monk_strike_of_the_windlord::HandleOnHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }

private:
    int32 m_Targets = 0;
};

/// Last Update 7.0.3 Build 22522
/// Called by Provoke - 115546
/// Called for Admonishment - 207025
class spell_monk_admonishment : public SpellScriptLoader
{
    public:
        spell_monk_admonishment() : SpellScriptLoader("spell_monk_admonishment") { }

        class spell_monk_admonishment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_admonishment_SpellScript);

            enum eSpells
            {
                Admonishment    = 207025,
                Intimidated     = 206891
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::Admonishment))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Intimidated, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_admonishment_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_admonishment_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Spinning Crane Kick - 107270
/// Called For Whirling Kicks - 201211
class spell_monk_whirling_kicks : public SpellScriptLoader
{
    public:
        spell_monk_whirling_kicks() : SpellScriptLoader("spell_monk_whirling_kicks") { }

        class spell_monk_whirling_kicks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_whirling_kicks_SpellScript);

            enum eSpells
            {
                WhirlingKicks       = 201211,
                WhirlingKicksBuff   = 201233
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster || !l_Caster->HasAura(eSpells::WhirlingKicks))
                    return;

                std::set<uint64>& l_TargetsGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::WhirlingKicksTargets];
                if (l_TargetsGUIDs.find(l_Target->GetGUID()) == l_TargetsGUIDs.end())
                {
                    l_TargetsGUIDs.insert(l_Target->GetGUID());
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::WhirlingKicksBuff))
                    {
                        if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0))
                        {
                            l_AuraEffect->SetAmount(l_AuraEffect->GetAmount() >= 80 ? 100 : l_AuraEffect->GetAmount() + 20);
                        }
                    }
                    else
                        l_Caster->CastSpell(l_Caster, eSpells::WhirlingKicksBuff, true);
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_monk_whirling_kicks_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_whirling_kicks_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Spinning Crane Kick - 101546
/// Called For Whirling Kicks - 201211
class spell_monk_whirling_kicks_buff : public SpellScriptLoader
{
    public:
        spell_monk_whirling_kicks_buff() : SpellScriptLoader("spell_monk_whirling_kicks_buff") { }

        class spell_monk_whirling_kicks_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_whirling_kicks_buff_AuraScript);

            void HandleCleanTargets(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::WhirlingKicksTargets].clear();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_whirling_kicks_buff_AuraScript::HandleCleanTargets, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_whirling_kicks_buff_AuraScript::HandleCleanTargets, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_whirling_kicks_buff_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Touch of Death - 122470
/// Called for Good Karma - 195295
class spell_monk_good_karma : public SpellScriptLoader
{
    public:
        spell_monk_good_karma() : SpellScriptLoader("spell_monk_good_karma") { }

        class spell_monk_good_karma_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_good_karma_AuraScript);

            enum eSpells
            {
                GoodKarmaAura = 195295,
                GoodKarmaHeal = 195318
            };

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& /*p_AbsorbAmount*/)
            {
                Unit* l_Caster = GetCaster();
                int32 l_Amount = p_DmgInfo.GetAmount();
                if (!l_Caster || !l_Amount)
                    return;

                if (l_Caster->HasAura(eSpells::GoodKarmaAura))
                    l_Caster->CastCustomSpell(l_Caster, eSpells::GoodKarmaHeal, &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_monk_good_karma_AuraScript::OnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_good_karma_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Whirling Dragon Punch - 152175
class spell_monk_whirling_dragon_punch : public SpellScriptLoader
{
    public:
        spell_monk_whirling_dragon_punch() : SpellScriptLoader("spell_monk_whirling_dragon_punch") { }

        class spell_monk_whirling_dragon_punch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_whirling_dragon_punch_AuraScript);

            enum eSpells
            {
                WhirlingDragonPunchDamages = 158221
            };

            void HandleAfterEffectPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::WhirlingDragonPunchDamages, true);
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_monk_whirling_dragon_punch_AuraScript::HandleAfterEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_whirling_dragon_punch_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Fists of Fury - 113656
/// Called for Crosswinds - 195650
class spell_monk_crosswinds : public SpellScriptLoader
{
    public:
        spell_monk_crosswinds() : SpellScriptLoader("spell_monk_crosswinds") { }

        class spell_monk_crosswinds_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_crosswinds_SpellScript);

            enum eSpells
            {
                CrosswindsAura    = 195650,
                CrosswindsTrigger = 195651
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::CrosswindsAura))
                    l_Caster->CastSpell(l_Caster, eSpells::CrosswindsTrigger, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_crosswinds_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_crosswinds_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Crane Heal - 198764
class spell_monk_crane_heal : public SpellScriptLoader
{
    public:
        spell_monk_crane_heal() : SpellScriptLoader("spell_monk_crane_heal") { }

        class spell_monk_crane_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_crane_heal_SpellScript);

            enum eSpells
            {
                CraneHealTarget = 198766
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->m_SpellHelper.GetBool(eSpellHelpers::CraneHealLastTarget))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CraneHealTarget, true);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_monk_crane_heal_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_crane_heal_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Crane Heal - 198766
class spell_monk_crane_heal_target : public SpellScriptLoader
{
    public:
        spell_monk_crane_heal_target() : SpellScriptLoader("spell_monk_crane_heal_target") { }

        class spell_monk_crane_heal_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_crane_heal_target_SpellScript);

            enum eSpells
            {
                CraneHealEffect = 198756
            };

            void CheckTarget(std::list<WorldObject*> &p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = nullptr;
                if (!l_Caster || !(l_Owner = l_Caster->GetOwner()))
                    return;

                Unit* l_Target = sObjectAccessor->FindUnit(l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::CraneHealLastTarget));

                if (!l_Target && !p_Targets.empty())
                    l_Target = (p_Targets.front() ) ? p_Targets.front()->ToUnit() : nullptr;

                if (l_Target)
                {
                    int32 l_HealAmount = static_cast<int32>(l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 1.35f);
                    l_HealAmount *= (1.0f + l_Owner->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f);

                    l_Caster->CastCustomSpell(l_Target, eSpells::CraneHealEffect, nullptr, &l_HealAmount, nullptr, true);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_crane_heal_target_SpellScript::CheckTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_crane_heal_target_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by The Mists of Sheilun - 199888
class spell_monk_the_mists_of_sheilun : public SpellScriptLoader
{
    public:
        spell_monk_the_mists_of_sheilun() : SpellScriptLoader("spell_monk_the_mists_of_sheilun") { }

        class spell_monk_the_mists_of_sheilun_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_the_mists_of_sheilun_AuraScript);

            enum eSpells
            {
                TheMistsOfSheilunFinal = 199894
            };

            void HandleOnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::TheMistsOfSheilunFinal, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_the_mists_of_sheilun_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_the_mists_of_sheilun_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Tiger Palm - 100780
/// Called for Face Palm - 213116
class spell_monk_face_palm : public SpellScriptLoader
{
    public:
        spell_monk_face_palm() : SpellScriptLoader("spell_monk_face_palm") { }

        class spell_monk_face_palm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_face_palm_SpellScript);

            enum eArtifactPowerId
            {
                FacePalm = 1286
            };

            enum eSpells
            {
                FacePalmAura    = 213116,
                FacePalmBonus   = 227679,
                FortifyingBrew  = 115203,
                PurifyingBrew   = 119582,
                IronskinBrew    = 115308,
                BlackOxBrew     = 115399
            };

            void HandleOnEffectHitTarget(SpellEffIndex effIndex)
            {
                Unit* l_Caster = GetCaster();
                int32 l_Damage = GetHitDamage();
                SpellInfo const* l_SpellInfoFacePalmAura = sSpellMgr->GetSpellInfo(eSpells::FacePalmAura);
                SpellInfo const* l_SpellInfoFacePalmBonus = sSpellMgr->GetSpellInfo(eSpells::FacePalmBonus);
                if (!l_Caster || !l_Damage || !l_SpellInfoFacePalmAura || !l_SpellInfoFacePalmBonus)
                    return;

                if (!l_Caster->HasAura(eSpells::FacePalmAura))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowerId::FacePalm);
                int32 l_PctProc = l_SpellInfoFacePalmAura->Effects[EFFECT_0].BasePoints * l_Rank;
                if (!roll_chance_i(l_PctProc))
                    return;

                SetHitDamage(CalculatePct(l_Damage, l_SpellInfoFacePalmBonus->Effects[EFFECT_0].BasePoints));

                uint32 l_AmountReduceCool = l_SpellInfoFacePalmBonus->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS;

                l_Player->ReduceSpellCooldown(eSpells::FortifyingBrew, l_AmountReduceCool);
                l_Player->ReduceSpellCooldown(eSpells::BlackOxBrew, l_AmountReduceCool);

                SpellInfo const* l_SpellInfoIronskinBrew = sSpellMgr->GetSpellInfo(eSpells::IronskinBrew);
                if (!l_SpellInfoIronskinBrew)
                    return;

                l_Player->ReduceChargeCooldown(l_SpellInfoIronskinBrew->ChargeCategoryEntry, l_AmountReduceCool);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_face_palm_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_face_palm_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Celestial Breath - 199640
class spell_monk_celestial_breath : public SpellScriptLoader
{
    public:
        spell_monk_celestial_breath() : SpellScriptLoader("spell_monk_celestial_breath") { }

        class spell_monk_celestial_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_celestial_breath_AuraScript);

            enum eSpells
            {
                ThunderFocusTea         = 116680,
                CelestialBreathCooldown = 214411,
                CelestialBreath         = 199641
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo || !l_Caster->IsPlayer())
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ThunderFocusTea)
                    return false;

                if (l_Caster->ToPlayer()->HasSpellCooldown(CelestialBreath))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                l_Caster->ToPlayer()->AddSpellCooldown(CelestialBreath, 0, 30 * IN_MILLISECONDS);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_celestial_breath_AuraScript::HandleOnCheckProc);
                AfterProc += AuraProcFn(spell_monk_celestial_breath_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_celestial_breath_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Celestial Breath (Heal) - 199656
/// Called for Celestial Breath - 199640
class spell_monk_celestial_breath_heal : public SpellScriptLoader
{
    public:
        spell_monk_celestial_breath_heal() : SpellScriptLoader("spell_monk_celestial_breath_heal") { }

        class spell_monk_celestial_breath_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_celestial_breath_heal_SpellScript);

            enum eSpells
            {
                CelestialBreathAura = 199640
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    return false;
                });

                SpellInfo const* l_SpellInfoCelestialBreathAura = sSpellMgr->GetSpellInfo(eSpells::CelestialBreathAura);
                if (!l_SpellInfoCelestialBreathAura)
                    return;

                uint8 l_MaxTargets = l_SpellInfoCelestialBreathAura->Effects[EFFECT_0].BasePoints;
                if (p_Targets.size() > l_MaxTargets)
                    p_Targets.resize(l_MaxTargets);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_celestial_breath_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ALLY_CONE_CASTER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_celestial_breath_heal_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Ovyd's Winter Wrap 217647
class spell_monk_ovyds_winter_wrap : public SpellScriptLoader
{
public:
    spell_monk_ovyds_winter_wrap() : SpellScriptLoader("spell_monk_ovyds_winter_wrap") { }

    class spell_monk_ovyds_winter_wrap_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_ovyds_winter_wrap_SpellScript);

        enum eSpells
        {
            OvydsWinterWrapProc = 217642
        };

        void HandleSelectTarget(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            int8 l_Hit = 0;
            if (!l_Caster)
                return;

            for (auto l_Target : p_Targets)
            {
                Unit* l_Unit = l_Target->ToUnit();

                if (l_Unit && l_Unit != GetExplTargetUnit())
                {
                    l_Hit++;
                    l_Caster->CastSpell(l_Unit, eSpells::OvydsWinterWrapProc, true);
                }
                if (l_Hit >= 2)
                    return;
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_ovyds_winter_wrap_SpellScript::HandleSelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_monk_ovyds_winter_wrap_SpellScript();
    }
};

class PlayerScript_DrinkingHornCover : public PlayerScript
{
    public:
        PlayerScript_DrinkingHornCover() : PlayerScript("PlayerScript_DrinkingHornCover") { }

        enum eSpells
        {
            DrinkingHornCover = 209256,
            StormEarthAndFire = 137639
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After) override
        {
            if (!p_After)
                return;

            if (p_Regen || p_Power != POWER_CHI || p_Player->getClass() != CLASS_MONK || !p_Player->HasAura(eSpells::DrinkingHornCover))
                return;

            int32 l_DiffVal = (p_OldValue - p_NewValue) / p_Player->GetPowerCoeff(p_Power);

            if (AuraEffect* l_Effect = p_Player->GetAuraEffect(eSpells::DrinkingHornCover, EFFECT_0))
            {
                if (Aura* l_Aura = p_Player->GetAura(eSpells::StormEarthAndFire))
                {
                    if (l_DiffVal > 0)
                    {
                        int32 l_AddDuration = l_DiffVal * (l_Effect->GetAmount() / 10.0f) * TimeConstants::IN_MILLISECONDS;

                        l_Aura->SetDuration(l_Aura->GetDuration() + l_AddDuration);
                    }
                }
            }
        }
};

/// Last Update: 7.1.5 23420
/// Called by Hit Combo - 196741
class spell_monk_hit_combo : public SpellScriptLoader
{
    public:
        spell_monk_hit_combo() : SpellScriptLoader("spell_monk_hit_combo") { }

        class spell_monk_hit_combo_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_hit_combo_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.50f;   ///< Hit Combo bonus is reduced by 50% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_hit_combo_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_hit_combo_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Petrichor Lagniappe - 206902
class spell_monk_petrichor_lagniappe : public SpellScriptLoader
{
    public:
        spell_monk_petrichor_lagniappe() : SpellScriptLoader("spell_monk_petrichor_lagniappe") { }

        class spell_monk_petrichor_lagniappe_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_petrichor_lagniappe_AuraScript);

            enum eSpells
            {
                RenewingMist = 115151,
                Revival      = 115310
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::RenewingMist)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Player || !l_SpellInfo)
                    return;

                int32 l_Amount = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (l_Amount < 0)
                    l_Amount = -l_Amount;

                l_Player->ReduceSpellCooldown(eSpells::Revival, l_Amount);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_petrichor_lagniappe_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_monk_petrichor_lagniappe_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_petrichor_lagniappe_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Sal'salabim's Strength - 212935
/// Called for Sal'salabim's Lost Tunic (item) - 137016
class spell_monk_salsalabims_strength : public SpellScriptLoader
{
    public:
        spell_monk_salsalabims_strength() : SpellScriptLoader("spell_monk_salsalabims_strength") { }

        class spell_monk_salsalabims_strength_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_salsalabims_strength_AuraScript);

            enum eSpells
            {
                KegSmash     = 121253,
                BreathOfFire = 115181
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::KegSmash)
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

                l_Player->RemoveSpellCooldown(eSpells::BreathOfFire, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_salsalabims_strength_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_monk_salsalabims_strength_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_salsalabims_strength_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Anvil & Stave - 235300
/// Called for Anvil-Hardened Wristwraps (item) - 144277
class spell_monk_anvil_and_stave : public SpellScriptLoader
{
    public:
        spell_monk_anvil_and_stave() : SpellScriptLoader("spell_monk_anvil_and_stave") { }

        class spell_monk_anvil_and_stave_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_anvil_and_stave_AuraScript);

            enum eSpells
            {
                FortifyingBrew = 115203,
                IronskinBrew   = 115308,
                PurifyingBrew  = 119582,
                BlackOxBrew    = 115399
            };

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_ProcInfos*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Player || !l_SpellInfo)
                    return;

                float l_Amount = (l_SpellInfo->Effects[EFFECT_0].BasePoints / 10.0f) * IN_MILLISECONDS;
                l_Player->ReduceSpellCooldown(eSpells::FortifyingBrew, l_Amount);
                l_Player->ReduceSpellCooldown(eSpells::BlackOxBrew, l_Amount);

                l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::IronskinBrew);
                SpellCategoryEntry const* l_CommonCategoryEntry = l_SpellInfo->ChargeCategoryEntry;
                if (!l_SpellInfo || !l_CommonCategoryEntry)
                    return;

                l_Player->ReduceChargeCooldown(l_CommonCategoryEntry, l_Amount);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_monk_anvil_and_stave_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_anvil_and_stave_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Shelter Of Rin - 235719
/// Called for Shelter Of Rin (item) - 144340
class spell_monk_shelter_of_rin : public SpellScriptLoader
{
    public:
        spell_monk_shelter_of_rin() : SpellScriptLoader("spell_monk_shelter_of_rin") { }

        class spell_monk_shelter_of_rin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_shelter_of_rin_AuraScript);

            enum eSpells
            {
                SheilunsGiftHeal    = 205406,
                ShelterOfRinHeal    = 235750,
                RenewingMist        = 119611,
                ShelterOfRin        = 235719

            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::SheilunsGiftHeal)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShelterOfRin);
                if (!l_DamageInfo || !l_Caster || !l_SpellInfo)
                    return;

                int32 l_HealAmount = l_DamageInfo->GetAmount();
                l_HealAmount = CalculatePct(l_HealAmount, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                for (uint64 l_TargetGUID : l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::RenewingMist])
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGUID);
                    if (!l_Target)
                        continue;

                    l_Caster->CastCustomSpell(l_Target, eSpells::ShelterOfRinHeal, &l_HealAmount, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_shelter_of_rin_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_monk_shelter_of_rin_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_shelter_of_rin_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Gai Plins Imperial Brew - 208840
/// Called for Gai Plin's Soothing Sash (item) - 137079
class spell_monk_gai_plins_imperial_brew : public SpellScriptLoader
{
    public:
        spell_monk_gai_plins_imperial_brew() : SpellScriptLoader("spell_monk_gai_plins_imperial_brew") { }

        class spell_monk_gai_plins_imperial_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_gai_plins_imperial_brew_SpellScript);

            enum eSpells
            {
                GaiPlinsImperialBrew = 208837,
                LightStagger         = 124275,
                ModerateStagger      = 124274,
                HeavyStagger         = 124273
            };

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GaiPlinsImperialBrew);
                if (!l_Caster || !l_SpellInfo)
                    return;

                AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::LightStagger, 0, l_Caster->GetGUID());
                if (!l_AurEff)
                    l_AurEff = l_Caster->GetAuraEffect(eSpells::ModerateStagger, 0, l_Caster->GetGUID());
                if (!l_AurEff)
                    l_AurEff = l_Caster->GetAuraEffect(eSpells::HeavyStagger, 0, l_Caster->GetGUID());
                if (!l_AurEff)
                    return;

                int32 l_Duration = l_AurEff->GetBase()->GetDuration();
                int32 l_Amplitude = l_AurEff->GetAmplitude();
                if (!l_Duration || !l_Amplitude)
                    return;

                int32 l_HealAmount = l_AurEff->GetAmount() * (l_Duration / l_Amplitude);
                l_HealAmount = CalculatePct(l_HealAmount, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                SetHitHeal(l_HealAmount);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_gai_plins_imperial_brew_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_gai_plins_imperial_brew_SpellScript();
        }
};

/// Update to Legion 7.1.5 buid 23420
/// Called for The Black Flame's Gamble - 216506
/// Called by ThunderFocusTea - 1166840
class spell_monk_the_black_flames_gamblee : public SpellScriptLoader
{
    public:
        spell_monk_the_black_flames_gamblee() : SpellScriptLoader("spell_monk_the_black_flames_gamblee") { }

        class spell_monk_the_black_flames_gamblee_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_the_black_flames_gamblee_SpellScript);

            enum eSpells
            {
                SpellsNumber            = 5,
                RenewingMist            = 216509,
                Effuse                  = 216992,
                EnvelopingMist          = 216995,
                EssenceFont             = 217000,
                Vivify                  = 217006,
                TheBlackFlamesGamble    = 216506
            };

            std::array<uint32, eSpells::SpellsNumber> m_Spells =
            {
                {
                    eSpells::RenewingMist,
                    eSpells::Effuse,
                    eSpells::EnvelopingMist,
                    eSpells::EssenceFont,
                    eSpells::Vivify
                }
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::TheBlackFlamesGamble))
                    return;

                for (auto l_eSpells : m_Spells)
                {
                    if (l_Caster->HasAura(l_eSpells))
                        return;
                }

                l_Caster->CastSpell(l_Caster, m_Spells[urand(0, 4)], true);
            }


            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_the_black_flames_gamblee_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_the_black_flames_gamblee_SpellScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Soothing Mist (periodic heal) - 115175
/// Called for Unison - 212123
/// Called for Unison Spaulders (item) - 137073
class spell_monk_unison : public SpellScriptLoader
{
    public:
        spell_monk_unison() : SpellScriptLoader("spell_monk_unison") { }

        class spell_monk_unison_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_unison_AuraScript);

            enum eSpells
            {
                UnisonAura   = 212123,
                UnisonTarget = 213804
            };

            void HandleOnEffectPeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->m_SpellHelper.GetUint32(eSpellHelpers::SoothingMistHealAmount) = p_AurEff->GetAmount();

                if (!l_Caster->HasAura(eSpells::UnisonAura))
                    return;

                l_Target->CastSpell(l_Target, eSpells::UnisonTarget, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_unison_AuraScript::HandleOnEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_unison_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Unison - 213804
/// Called for Unison Spaulders (item) - 137073
class spell_monk_unison_target : public SpellScriptLoader
{
    public:
        spell_monk_unison_target() : SpellScriptLoader("spell_monk_unison_target") { }

        class spell_monk_unison_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_unison_target_SpellScript);

            enum eSpells
            {
                UnisonPeriodic = 213872
            };

            void FindTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !p_Targets.size())
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());

                Unit* l_Target = p_Targets.front()->ToUnit();
                if (!l_Target || l_Target->GetHealthPct() == 100.0f)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::UnisonPeriodic, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_unison_target_SpellScript::FindTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_unison_target_SpellScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Unison - 213872
/// Called for Unison Spaulders (item) - 137073
class spell_monk_unison_periodic : public SpellScriptLoader
{
    public:
        spell_monk_unison_periodic() : SpellScriptLoader("spell_monk_unison_periodic") { }

        class spell_monk_unison_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_unison_periodic_AuraScript);

            enum eSpells
            {
                UnisonAura = 212123,
                UnisonHeal = 213884
            };

            void HandleAfterEffectPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::UnisonAura);
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                int32 l_HealAmount = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::SoothingMistHealAmount);
                l_HealAmount = CalculatePct(l_HealAmount, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                l_Caster->CastCustomSpell(l_Target, eSpells::UnisonHeal, &l_HealAmount, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_monk_unison_periodic_AuraScript::HandleAfterEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_unison_periodic_AuraScript();
        }
};

/// Legion 7.1.5 - build 23420
/// Called by Thunder Focus Tea 116680
class spell_monk_focused_thunder : public SpellScriptLoader
{
    public:
        spell_monk_focused_thunder() : SpellScriptLoader("spell_monk_focused_thunder") { }

        enum eSpells
        {
            FocusingThunder = 197895
        };

        class spell_monk_focused_thunder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_focused_thunder_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::FocusingThunder))
                {
                    GetSpell()->SetSpellValue(SpellValueMod::SPELLVALUE_AURA_STACK, 2);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_monk_focused_thunder_SpellScript::HandleBeforeCast);
            }
        };

        class spell_monk_focused_thunder_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_focused_thunder_AuraScript);

            void HandleCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
            {
                // HACK: spellmods should have value with 1 stack if the aura has more than 1 stacks
                if (GetStackAmount() > 1 && spellMod->value != 0)
                {
                    spellMod->value /= GetStackAmount();
                }
            }

            void Register() override
            {
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_monk_focused_thunder_AuraScript::HandleCalcSpellMod, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_monk_focused_thunder_AuraScript::HandleCalcSpellMod, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_monk_focused_thunder_AuraScript::HandleCalcSpellMod, EFFECT_2, SPELL_AURA_ADD_PCT_MODIFIER);
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_monk_focused_thunder_AuraScript::HandleCalcSpellMod, EFFECT_4, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_focused_thunder_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_focused_thunder_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Life Cocoon - 116849
class spell_monk_mists_of_life : public SpellScriptLoader
{
    public:
        spell_monk_mists_of_life() : SpellScriptLoader("spell_monk_mists_of_life") { }

        class spell_monk_mists_of_life_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_mists_of_life_SpellScript);

            enum eSpells
            {
                MistsOfLife = 199563,
                RenewingMist = 119611,
                EnvelopingMist = 124682
            };

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::MistsOfLife))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EnvelopingMist, true);
                l_Caster->CastSpell(l_Target, eSpells::RenewingMist, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_mists_of_life_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_mists_of_life_SpellScript();
        }
};

/// Called by Ancient Mistweaver Arts - 209520
class spell_monk_ancient_mistweaver_arts : public SpellScriptLoader
{
    public:
        spell_monk_ancient_mistweaver_arts() : SpellScriptLoader("spell_monk_ancient_mistweaver_arts") { }

        class spell_monk_ancient_mistweaver_arts_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_ancient_mistweaver_arts_AuraScript);

            enum eSpells
            {
                SoothingMistActive = 209525
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->learnSpell(eSpells::SoothingMistActive, false, false, false, 0, true);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->removeSpell(eSpells::SoothingMistActive, false, false);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_ancient_mistweaver_arts_AuraScript::HandleAfterEffectApply, EFFECT_2, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_ancient_mistweaver_arts_AuraScript::HandleAfterEffectRemove, EFFECT_2, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_ancient_mistweaver_arts_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Control the Mists - 233765
class spell_monk_control_the_mists : public SpellScriptLoader
{
    public:
        spell_monk_control_the_mists() : SpellScriptLoader("spell_monk_control_the_mists") { }

        class spell_monk_control_the_mists_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_control_the_mists_AuraScript);

            enum eSpells
            {
                ControlTheMistsAura = 233954,
                ControlTheMistsCD   = 233766
            };

            void HandleAfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (!l_Owner->HasAura(eSpells::ControlTheMistsCD))
                    l_Owner->CastSpell(l_Owner, eSpells::ControlTheMistsAura, true);
            }

            void HandleAfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                l_Owner->RemoveAura(eSpells::ControlTheMistsAura);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_control_the_mists_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_control_the_mists_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_control_the_mists_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Control the Mists - 233766
class spell_monk_control_the_mists_cd : public SpellScriptLoader
{
    public:
        spell_monk_control_the_mists_cd() : SpellScriptLoader("spell_monk_control_the_mists_cd") { }

        class spell_monk_control_the_mists_cd_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_control_the_mists_cd_AuraScript);

            enum eSpells
            {
                ControlTheMists     = 233765,
                ControlTheMistsAura = 233954
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (!p_EventInfo.GetDamageInfo())
                    return false;

                SpellInfo const* triggerSpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!triggerSpellInfo)
                    return false;

                if (triggerSpellInfo->SpellPowers.empty())
                    return false;

                for (auto spellPower : triggerSpellInfo->SpellPowers)
                {
                    if (spellPower->PowerType == POWER_CHI)
                        return true;
                }

                return false;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (!p_ProcEventInfo.GetDamageInfo())
                    return;

                SpellInfo const* triggerSpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (!triggerSpellInfo)
                    return;

                if (triggerSpellInfo->SpellPowers.empty())
                    return;

                if (AuraEffect const* l_MainEffect = l_Owner->GetAuraEffect(eSpells::ControlTheMists, EFFECT_0))
                {
                    int chiCost = 0;
                    for (auto spellPower : triggerSpellInfo->SpellPowers)
                    {
                        if (spellPower->PowerType == POWER_CHI && (!spellPower->RequiredAuraSpellId || l_Owner->HasAura(spellPower->RequiredAuraSpellId)))
                            chiCost += spellPower->Cost;
                    }

                    int32 reduceDuration = chiCost * l_MainEffect->GetAmount() * IN_MILLISECONDS;
                    if (reduceDuration <= 0)
                        return;

                    if (GetDuration() >= reduceDuration)
                        SetDuration(GetDuration() - reduceDuration);
                }
            }

            void HandleAfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (l_Owner->HasAura(eSpells::ControlTheMists))
                    l_Owner->CastSpell(l_Owner, eSpells::ControlTheMistsAura, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_control_the_mists_cd_AuraScript::HandleOnCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_monk_control_the_mists_cd_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_control_the_mists_cd_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_control_the_mists_cd_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Gale Burst - 195403
class spell_monk_gale_burst : public SpellScriptLoader
{
    public:
        spell_monk_gale_burst() : SpellScriptLoader("spell_monk_gale_burst") { }

        class spell_monk_gale_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_gale_burst_AuraScript);

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_DamageInfo || !l_Aura)
                    return;

                Unit* l_Actor = l_DamageInfo->GetActor();
                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);

                if (!l_Actor || !l_AuraEffect)
                    return;

                Player* l_Monk = l_Caster->ToPlayer();
                Player* l_ModOwner = l_Actor->GetSpellModOwner();
                if (!l_Monk || !l_ModOwner || l_Monk != l_ModOwner) ///< Damage dealt by the monk's Earth and Fire spirit from Storm Earth and Fire (137639) should also increase Gale Burst's damage 7.3.5 - Build 26365
                    return;

                l_AuraEffect->SetAmount(l_AuraEffect->GetAmount() + l_DamageInfo->GetAmount());
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_monk_gale_burst_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_gale_burst_AuraScript();
        }
};

/// Last Update: 7.3.5 build 26365
/// Called by Rising Sun Kick - 185099
class spell_monk_tornado_kicks : public SpellScriptLoader
{
    public:
        spell_monk_tornado_kicks() : SpellScriptLoader("spell_monk_tornado_kicks") { }

        class spell_monk_tornado_kicks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_tornado_kicks_SpellScript);

            enum eSpells
            {
                TornadoKicks            = 196082,
                T20Windwalker4PBonus    = 242260,
                FistsOfFury             = 113656
            };

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                    if (l_Player->HasAura(eSpells::T20Windwalker4PBonus) && l_Target)
                        if (GetSpell()->IsCritForTarget(l_Target))
                            if (SpellInfo const* l_BonusSpellInfo = sSpellMgr->GetSpellInfo(eSpells::T20Windwalker4PBonus))
                                l_Player->ReduceSpellCooldown(eSpells::FistsOfFury, l_BonusSpellInfo->Effects[EFFECT_0].BasePoints);

                if (!l_Caster->GetSpellModOwner())
                    return;

                if (AuraEffect *l_Effect = l_Caster->GetSpellModOwner()->GetAuraEffect(eSpells::TornadoKicks, EFFECT_0))
                {
                    bool& l_TornadoKicks = l_Caster->m_SpellHelper.GetBool(eSpellHelpers::TornadoKicks);
                    uint32& l_PrevDamage = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::TornadoKicks);

                    if (l_TornadoKicks)
                    {
                        l_TornadoKicks = false;
                        int32 l_Damage = l_PrevDamage;

                        l_Damage = CalculatePct(l_Damage, l_Effect->GetBaseAmount());
                        SetHitDamage(l_Damage);
                    }
                    else
                    {
                        if (!l_Target)
                            return;

                        int64 l_TargetGUID = l_Target->GetGUID();
                        int32 l_SpellId = m_scriptSpellId;

                        l_TornadoKicks = true;
                        l_Caster->AddDelayedEvent([l_Caster]() -> void
                        {
                            l_Caster->m_SpellHelper.GetBool(eSpellHelpers::TornadoKicks) = false; ///< In case the original Rising sun kick kills the target, if we don't reset the helper to false, the spell becomes fucked until DC / RC
                        }, 1000);

                        l_PrevDamage = GetHitDamage();
                        if (GetSpell() && GetSpell()->IsCritForTarget(l_Target))
                            l_PrevDamage *= 2;

                        if (l_Target)
                            l_Caster->DelayedCastSpell(l_Target, l_SpellId, true, 300);
                        else
                            l_Caster->m_SpellHelper.GetBool(eSpellHelpers::TornadoKicks) = false;
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_tornado_kicks_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_tornado_kicks_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blessings of Yu'lon - 199665
class spell_monk_blessings_of_yulon : public SpellScriptLoader
{
    public:
        spell_monk_blessings_of_yulon() : SpellScriptLoader("spell_monk_blessings_of_yulon") { }

        class spell_monk_blessings_of_yulon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_blessings_of_yulon_AuraScript);

            enum eSpells
            {
                BlessingsOfYulonHeal    = 199668,
                BlessingsOfYulonSummon  = 199671,
                BlessingsOfYulon        = 210110,
                Revival                 = 115310
            };

            Guid128 m_LastProcGUID;

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_SpellInfo || !l_Spell || l_SpellInfo->Id != eSpells::Revival)
                    return false;

                Guid128 l_CastGUID = l_Spell->GetCastGuid();
                if (l_CastGUID == m_LastProcGUID)
                    return false;

                m_LastProcGUID = l_CastGUID;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_HealSpellInfo = sSpellMgr->GetSpellInfo(eSpells::BlessingsOfYulonHeal);
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo || !l_HealSpellInfo)
                    return;

                int32 l_Amount = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints) / l_HealSpellInfo->GetMaxTicks();

                std::list<Unit*> l_UnitList;
                JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(l_Caster, l_Caster, 30.0f);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(l_Caster, l_UnitList, u_check);
                l_Caster->VisitNearbyObject(30.0f, searcher);

                l_UnitList.remove_if([l_Caster](Unit* p_Unit) -> bool
                {
                    if (p_Unit == nullptr)
                        return true;

                    if (!l_Caster->IsInPartyWith(p_Unit) && !l_Caster->IsInRaidWith(p_Unit))
                        return true;

                    return false;
                });

                for (Unit* l_Unit : l_UnitList)
                    l_Caster->CastCustomSpell(l_Unit, eSpells::BlessingsOfYulonHeal, &l_Amount, nullptr, nullptr, true);

                l_Caster->CastSpell(l_Caster, eSpells::BlessingsOfYulonSummon, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_blessings_of_yulon_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_monk_blessings_of_yulon_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_blessings_of_yulon_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Light Stagger - 124275 - Moderate Stagger - 124274 - Heavy Stagger - 124273
/// Called for High Tolerance - 196737
class spell_monk_high_tolerance : public SpellScriptLoader
{
    public:
        spell_monk_high_tolerance() : SpellScriptLoader("spell_monk_high_tolerance") { }

        class spell_monk_high_tolerance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_high_tolerance_AuraScript);

            enum eSpells
            {
                LightStagger        = 124275,
                ModerateStagger     = 124274,
                HeavyStagger        = 124273,
                HighTolerance       = 196737
            };

            void HandleAfterApplyHaste(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HighTolerance);
                if (!l_SpellInfo)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::LightStagger:
                        const_cast<AuraEffect*>(p_AuraEffect)->ChangeAmount(l_SpellInfo->Effects[EFFECT_1].BasePoints - 1);
                        break;
                    case eSpells::ModerateStagger:
                        const_cast<AuraEffect*>(p_AuraEffect)->ChangeAmount(l_SpellInfo->Effects[EFFECT_2].BasePoints - 2);
                        break;
                    case eSpells::HeavyStagger:
                        const_cast<AuraEffect*>(p_AuraEffect)->ChangeAmount(l_SpellInfo->Effects[EFFECT_3].BasePoints - 3);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_high_tolerance_AuraScript::HandleAfterApplyHaste, EFFECT_2, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_high_tolerance_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Light on your feet - 195244
class spell_monk_light_on_your_feet : public SpellScriptLoader
{
    public:
        spell_monk_light_on_your_feet() : SpellScriptLoader("spell_monk_light_on_your_feet") { }

        class spell_monk_light_on_your_feet_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_light_on_your_feet_AuraScript);

            enum eArtifactPowers
            {
                LightOnYourFeetArtifact = 801
            };

            void HandleCalcAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Owner = GetUnitOwner();

                if (!l_SpellInfo || !l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::LightOnYourFeetArtifact);
                    amount = l_Rank * l_SpellInfo->Effects[0].BasePoints;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_light_on_your_feet_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_MOD_DODGE_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_light_on_your_feet_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Protection of Shaohao - 199367
class spell_monk_protection_of_shaohao : public SpellScriptLoader
{
    public:
        spell_monk_protection_of_shaohao() : SpellScriptLoader("spell_monk_protection_of_shaohao") { }

        class spell_monk_protection_of_shaohao_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_protection_of_shaohao_AuraScript);

            enum eArtifactPowers
            {
                ProtectionOfShaohaoArtifact = 941
            };

            void HandleCalcAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Owner = GetUnitOwner();

                if (!l_SpellInfo || !l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::ProtectionOfShaohaoArtifact);
                    p_Amount = l_Rank * l_SpellInfo->Effects[0].BasePoints;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_protection_of_shaohao_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_protection_of_shaohao_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Life Cocoon - 116849
class spell_monk_life_cocoon : public SpellScriptLoader
{
    public:
        spell_monk_life_cocoon() : SpellScriptLoader("spell_monk_life_cocoon") { }

        enum eSpells
        {
            ProtectionOfShaohao = 199367
        };

        class spell_monk_life_cocoon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_life_cocoon_AuraScript);


            void HandleCalcAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                Unit* l_Owner = GetUnitOwner();

                if (!l_Owner)
                    return;

                if (AuraEffect* l_ProtectionOfShaohao = l_Owner->GetAuraEffect(eSpells::ProtectionOfShaohao, EFFECT_0))
                    p_Amount = AddPct(p_Amount, l_ProtectionOfShaohao->GetAmount());
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_life_cocoon_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_life_cocoon_AuraScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Blackout Combo - 228563
class spell_monk_blackout_combo_proc : public SpellScriptLoader
{
    public:
        spell_monk_blackout_combo_proc() : SpellScriptLoader("spell_monk_blackout_combo_proc") { }

        class spell_monk_blackout_combo_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_blackout_combo_proc_AuraScript);

            enum eSpells
            {
                ElusiveBrawler  = 195630,

                TigerPalm       = 100780,
                KegSmash        = 121253,
                IronSkinBrew    = 115308,
                PurifyingBrew   = 119582
            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::TigerPalm,
                eSpells::KegSmash,
                eSpells::IronSkinBrew,
                eSpells::PurifyingBrew
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || !(std::find(m_Spells.begin(), m_Spells.end(), l_SpellInfo->Id) != m_Spells.end()))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Aura* l_Aura = GetAura();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_SpellInfo || !l_Aura || !l_Player)
                    return;

                SpellInfo const* l_BlackoutCombo = GetSpellInfo();
                if (!l_BlackoutCombo)
                    return;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::KegSmash:
                    {
                        SpellInfo const* l_BrewInfo = sSpellMgr->GetSpellInfo(eSpells::PurifyingBrew);
                        if (!l_SpellInfo)
                            return;

                        l_Player->ReduceChargeCooldown(l_BrewInfo->ChargeCategoryEntry, l_BlackoutCombo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
                        l_Aura->Remove();
                        break;
                    }
                    case eSpells::PurifyingBrew:
                        l_Caster->CastSpell(l_Caster, eSpells::ElusiveBrawler, true);
                        l_Aura->Remove();
                        break;
                    case eSpells::IronSkinBrew:
                        l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IronSkin) = 2 * (1 + l_BlackoutCombo->Effects[EFFECT_3].BasePoints); ///< 2 times cuz each tick is applied every 0.5sec instead of every 1 sec
                        l_Aura->Remove();
                        break;
                    default:
                        l_Aura->Remove();
                        break;
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_blackout_combo_proc_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_monk_blackout_combo_proc_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_blackout_combo_proc_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Stagger 124273 - 124274 - 124275
/// Called for Blackout Combo 228563
class spell_monk_stagger : public SpellScriptLoader
{
    public:
        spell_monk_stagger() : SpellScriptLoader("spell_monk_stagger") { }

        class spell_monk_stagger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_stagger_AuraScript);

            enum eSpells
            {
                BlackOutCombo = 228563
            };

            uint32 m_Amout = 0;
            void HandleCalc(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BlackOutCombo);
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IronSkin) == 0)
                    return;

                if (m_Amout == 0)
                    m_Amout = p_AuraEffect->GetAmount();

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IronSkin)--;
                const_cast<AuraEffect *>(p_AuraEffect)->SetAmount(0);

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::IronSkin) == 0)
                {
                    const_cast<AuraEffect *>(p_AuraEffect)->SetAmount(m_Amout);
                    m_Amout = 0;
                }
            }

            void OnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                AuraEffect const* l_Stagger = l_Aura->GetEffect(EFFECT_0);
                if (!l_Stagger)
                    return;

                /// Update Stagger Aura visual and damages
                int32 l_Time = l_Aura->GetDuration();
                uint32 l_TicksNumber = l_Time / l_Stagger->GetAmplitude();
                int32 l_Bp = l_Stagger->GetAmount();
                int32 l_RemainingStagger = l_Bp * l_TicksNumber;
                uint32 l_SpellId = 0;
                if (int64(l_RemainingStagger) < l_Caster->CountPctFromMaxHealth(30))
                    l_SpellId = LIGHT_STAGGER;
                else if (int64(l_RemainingStagger) < l_Caster->CountPctFromMaxHealth(60))
                    l_SpellId = MODERATE_STAGGER;
                else
                    l_SpellId = HEAVY_STAGGER;

                l_Caster->RemoveAura(LIGHT_STAGGER);
                l_Caster->RemoveAura(MODERATE_STAGGER);
                l_Caster->RemoveAura(HEAVY_STAGGER);
                l_Caster->CastCustomSpell(l_Caster, l_SpellId, &l_Bp, &l_RemainingStagger, nullptr, true);
                l_Caster->CastCustomSpell(l_Caster, 124255, nullptr, &l_RemainingStagger, nullptr, true); ///< Update Stagger Bar on Stagger Tick

                Aura* l_SecondAura = l_Caster->GetAura(l_SpellId);
                if (!l_SecondAura)
                    return;

                l_SecondAura->SetDuration(l_Time);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_stagger_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_stagger_AuraScript::HandleCalc, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_stagger_AuraScript();
        }
};

/// Last Update: 7.3.5 build 26365
/// Called by Tiger Palm - 100780, BlackoutKick 100784, RisingSunKick 107428
class spell_monk_spinning_crane_kick : public SpellScriptLoader
{
    public:
        spell_monk_spinning_crane_kick() : SpellScriptLoader("spell_monk_spinning_crane_kick") { }

        class spell_monk_spinning_crane_kick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_spinning_crane_kick_SpellScript);

            enum eSpells
            {
                MarkOfTheCrane  = 228287,
                CycloneStrikes = 220357
            };

            uint32 m_TargetCount = 0;

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || l_Caster == l_Target)
                    return;

                /*if (TempSummon* l_TempSummon = l_Caster->ToTempSummon())
                {
                    if (Unit* l_Owner = l_TempSummon->GetSummoner())
                    {
                        if (!l_Owner->IsPlayer() || l_Owner->ToPlayer()->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                            return;

                        l_TempSummon->CastSpell(l_Target, eSpells::MarkOfTheCrane, true);
                        return;
                    }
                }

                if (!l_Caster->IsPlayer() || l_Caster->ToPlayer()->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                    return;

                if (l_Target != l_Caster)
                    l_Caster->CastSpell(l_Target, eSpells::MarkOfTheCrane, true);*/

                if (Player* l_Monk = l_Caster->GetSpellModOwner())
                    if (l_Monk->HasAura(eSpells::CycloneStrikes))
                        l_Monk->CastSpell(l_Target, eSpells::MarkOfTheCrane, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_monk_spinning_crane_kick_SpellScript::HandleAfterHit);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_spinning_crane_kick_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Called by Mark of the crane - 228287
class spell_monk_mark_of_the_crane : public SpellScriptLoader
{
    public:
        spell_monk_mark_of_the_crane() : SpellScriptLoader("spell_monk_mark_of_the_crane") { }

        class spell_monk_mark_of_the_crane_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_mark_of_the_crane_AuraScript);

            enum eSpells
            {
                MarkOfTheCrane = 228287,
                CycloneStrikesTrigger = 220358
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                if (l_Caster->GetGUID() != l_Target->GetGUID())
                {
                    if (l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkOfTheCrane].find(l_Target->GetGUID()) == l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkOfTheCrane].end())
                    {
                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkOfTheCrane].insert(l_Target->GetGUID());
                        l_Caster->CastSpell(l_Caster, eSpells::CycloneStrikesTrigger, true);
                    }
                    else
                    {
                        if (Aura* l_Aura = l_Caster->GetAura(eSpells::CycloneStrikesTrigger))
                            l_Aura->RefreshDuration();
                    }
                }
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::MarkOfTheCrane].erase(l_Target->GetGUID());
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::CycloneStrikesTrigger))
                    l_Aura->DropStack();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_mark_of_the_crane_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_mark_of_the_crane_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_mark_of_the_crane_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by The Emperor's Capacitor - 235053
/// Called for The Emperor's Capacitor (item) - 144239
class spell_monk_the_emperors_capacitor : public SpellScriptLoader
{
    public:
        spell_monk_the_emperors_capacitor() : SpellScriptLoader("spell_monk_the_emperors_capacitor") { }

        class spell_monk_the_emperors_capacitor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_the_emperors_capacitor_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_the_emperors_capacitor_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_the_emperors_capacitor_AuraScript();
        }
};

class PlayerScript_monk_the_emperors_capacitor : public PlayerScript
{
    public:
        PlayerScript_monk_the_emperors_capacitor() : PlayerScript("PlayerScript_monk_the_emperors_capacitor") {}

        enum eSpells
        {
            BlackoutKick         = 100784,
            RisingSunKick        = 107428,
            FistsOfFury          = 113656,
            SpinningCraneKick    = 101546,
            RushingJadeWind      = 116847,
            StrikeOfTheWindlord  = 205320,
            TheEmperorsCapacitor = 235053,
            EmperorCapacitorProc = 235054,
        };

        void OnSpellCast(Player* p_Player, Spell* p_Spell, bool p_SkipCheck)
        {
            if (!p_Spell->m_FromClient || p_Player->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                return;

            SpellInfo const* l_SpellInfo = p_Spell->GetSpellInfo();
            if (!l_SpellInfo)
                return;

            switch (l_SpellInfo->Id)
            {
                case eSpells::BlackoutKick:
                case eSpells::RisingSunKick:
                case eSpells::FistsOfFury:
                case eSpells::SpinningCraneKick:
                case eSpells::RushingJadeWind:
                case eSpells::StrikeOfTheWindlord:
                {
                    if (p_Player->HasAura(eSpells::TheEmperorsCapacitor))
                        p_Player->DelayedCastSpell(p_Player, eSpells::EmperorCapacitorProc, true, 50);
                    break;
                }
             }
        }
};


/// Last Update: 7.1.5 build 23420
/// Called by Mighty Ox Kick (Honor Talent) - 202370
class spell_monk_mighty_ox_kick : public SpellScriptLoader
{
    public:
        spell_monk_mighty_ox_kick() : SpellScriptLoader("spell_monk_mighty_ox_kick") { }

        class spell_monk_mighty_ox_kick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_mighty_ox_kick_SpellScript);

            enum eSpells
            {
                MightyOxKick = 202372
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MightyOxKick);
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                /// Special Leap Back for this spell, the target always go behind the caster
                float l_speedXY = l_SpellInfo->Effects[EFFECT_0].MiscValue / 10.f;
                float l_speedZ = l_SpellInfo->Effects[EFFECT_0].BasePoints / 10.f;

                float l_RelativeTargetX = l_Target->m_positionX - l_Caster->m_positionX;
                float l_RelativeTargetY = l_Target->m_positionY - l_Caster->m_positionY;

                float l_TempTargetX = l_RelativeTargetX * std::cos(-l_Caster->m_orientation) - l_RelativeTargetY * std::sin(-l_Caster->m_orientation);
                float l_TempTargetY = l_RelativeTargetY * std::cos(-l_Caster->m_orientation) + l_RelativeTargetX * std::sin(-l_Caster->m_orientation);

                float l_Alpha = std::asin((std::abs(l_RelativeTargetX) / (2 * l_speedZ * l_speedXY / Movement::gravity)));///< dist = 2 * speedZ * speedXY / Gravity;
                float l_AngleFromTarget = M_PI - (l_Target->m_orientation - l_Caster->m_orientation) + l_Alpha;

                if (l_Target->GetTypeId() == TYPEID_UNIT)
                    l_Target->GetMotionMaster()->MoveJumpTo(l_AngleFromTarget, l_speedXY, l_speedZ);
                else
                {
                    float vcos = std::cos(l_AngleFromTarget + l_Target->GetOrientation());
                    float vsin = std::sin(l_AngleFromTarget + l_Target->GetOrientation());
                    l_Target->SendMoveKnockBack(l_Target->ToPlayer(), l_speedXY, -l_speedZ, vcos, vsin);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_mighty_ox_kick_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_mighty_ox_kick_SpellScript();
        }
};

/// Last Update: 7.3.5 26356
/// Called by Renewing Mist - 119611
class spell_monk_counteract_magic : public SpellScriptLoader
{
    public:
        spell_monk_counteract_magic() : SpellScriptLoader("spell_monk_counteract_magic") { }

        class spell_monk_counteract_magic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_counteract_magic_AuraScript);

            enum eSpells
            {
                CounteractMagic = 202428
            };

            int32 m_BaseAmount = 0;
            bool m_Applied = false;

            void HandleOnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                m_Applied = false;
            }

            void HandleOnHit(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->HasAura(eSpells::CounteractMagic))
                    return;

                if (AuraEffect* l_AuraEffect = const_cast<AuraEffect*>(p_AuraEffect))
                {
                    if (!l_Caster->GetAuraEffectsByType(AuraType::SPELL_AURA_PERIODIC_DAMAGE).empty() || !l_Caster->GetAuraEffectsByType(AuraType::SPELL_AURA_PERIODIC_DAMAGE_PERCENT).empty())
                    {
                        if (m_Applied)
                            return;

                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CounteractMagic);

                        if (!l_SpellInfo)
                            return;

                        int32 l_Heal = l_AuraEffect->GetAmount();

                        m_Applied = true;
                        m_BaseAmount = l_Heal;
                        l_Heal = AddPct(l_Heal, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                        l_AuraEffect->SetAmount(l_Heal);
                    }
                    else if (m_Applied)
                    {
                        m_Applied = false;
                        l_AuraEffect->SetAmount(m_BaseAmount);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_counteract_magic_AuraScript::HandleOnApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_HEAL, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_counteract_magic_AuraScript::HandleOnHit, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_counteract_magic_AuraScript();
        }
};

/// Last Update: 7.3.5 build 26365
/// Called for Refreshing Breeze - 202523
/// Called by Vivify - 116670
class spell_monk_refreshing_breeze : public SpellScriptLoader
{
    public:
        spell_monk_refreshing_breeze() : SpellScriptLoader("spell_monk_refreshing_breeze") { }

        class spell_monk_refreshing_breeze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_refreshing_breeze_SpellScript);

            enum eSpells
            {
                RefreshingBreezeAura = 202523,
                EssenceFont = 191840
            };

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::RefreshingBreezeAura))
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (Aura* l_Aura = l_Target->GetAura(eSpells::EssenceFont, l_Caster->GetGUID()))
                    l_Aura->SetDuration(l_Aura->GetMaxDuration());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_refreshing_breeze_SpellScript::HandleHeal, EFFECT_1, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_refreshing_breeze_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Spirit of the Crane - 210802
class spell_monk_spirit_of_the_crane : public SpellScriptLoader
{
    public:
        spell_monk_spirit_of_the_crane() : SpellScriptLoader("spell_monk_spirit_of_the_crane") { }

        class spell_monk_spirit_of_the_crane_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_spirit_of_the_crane_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                PreventHitEffect(EFFECT_0);
                int32 l_CustomAmount = GetSpell()->GetSpellValue(SPELLVALUE_BASE_POINT0);
                float l_PctToRestore = (float)l_CustomAmount / 100.0f;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                float l_MaxMana = (float)l_Player->GetMaxPower(POWER_MANA);
                float l_Amount = l_MaxMana;

                ApplyPercentModFloatVar(l_Amount, l_PctToRestore, true);
                l_Player->EnergizeBySpell(l_Player, m_scriptSpellId, l_Amount - l_MaxMana, POWER_MANA);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_spirit_of_the_crane_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_ENERGIZE_PCT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_spirit_of_the_crane_SpellScript();
        }
};

/// Obstinate Determination - 216424
class PlayerScript_ObstinateDetermination : public PlayerScript
{
    public:
        PlayerScript_ObstinateDetermination() : PlayerScript("PlayerScript_ObstinateDetermination") { }

        enum eSpells
        {
            ObstinateDetermination  = 216424,
            GiftOfTheOxLeft         = 124503,
            GiftOfTheOxRight        = 124506,
            HealingSphereCounter    = 224863
        };

        void OnModifyHealth(Player* p_Player, int64 p_Value, int64 p_OldValue) override
        {
            if (p_Player->getClass() == CLASS_MONK)
            {
                int32 l_OldPct = (p_OldValue * 100) / p_Player->GetMaxHealth();
                int32 l_NewPct = (p_Value * 100) / p_Player->GetMaxHealth();

                if (l_OldPct >= 35.0f && l_NewPct < 35.0f)
                {
                    if (p_Player->HasAura(eSpells::ObstinateDetermination))
                    {
                       /// Cast Spell Healing Sphere Active, required for some caster aura spell (115072) Expel Harm
                       p_Player->CastSpell(p_Player, eSpells::HealingSphereCounter, true);
/*
                       if (roll_chance_i(50))
                           p_Player->CastSpell(p_Player, eSpells::GiftOfTheOxLeft, true);
                       else
                           p_Player->CastSpell(p_Player, eSpells::GiftOfTheOxRight, true);*/
                    }
                }
            }
        }
};

/// Last Update: 7.1.5 23420
/// Gift of the Ox - 124502
class PlayerScript_GiftOfTheOx : public PlayerScript
{
public:
    PlayerScript_GiftOfTheOx() : PlayerScript("PlayerScript_GiftOfTheOx") { }

    enum eSpells
    {
        ObstinateDetermination = 216424,
        GiftOfTheOx = 124502,
        GiftOfTheMists = 196719,
        GiftOfTheOxLeft = 124503,
        GiftOfTheOxRight = 124506,
        GiftOfTheOxLeftBig = 214420,
        GiftOfTheOxRightBig = 214418,
        HealingSphereCounter = 224863,
        Overflow = 213180
    };

    enum eArtifactPowerID
    {
        OverflowArtifactPowerID = 1280
    };

    void OnTakeDamage(Player* p_Player, Unit* p_Origin, DamageEffectType p_DamageEffectType, uint32 p_Damage, SpellSchoolMask p_SchoolMask, CleanDamage const* p_CleanDamage) override
    {
        if (p_Player->GetActiveSpecializationID() != SPEC_MONK_BREWMASTER || !p_Origin || !p_CleanDamage)
            return;

        if (!p_Player->HasSpellCooldown(eSpells::GiftOfTheOx))
        {
            /// Gift of the Ox is no longer a random chance, under the hood. When you are hit, it increments a counter by (DamageTakenBeforeAbsorbsOrStagger / MaxHealth).
            /// It now drops an orb whenever that reaches 1.0, and decrements it by 1.0. The tooltip still says chance, to keep it understandable.
            /// Gift of the Mists multiplies that counter increment by (2 - (HealthBeforeDamage - DamageTakenBeforeAbsorbsOrStagger) / MaxHealth);

            float l_DamageTakenBeforeAbsorbsOrStagger = p_Damage + p_CleanDamage->absorbed_damage;
            if (l_DamageTakenBeforeAbsorbsOrStagger > p_Player->GetHealth())
                l_DamageTakenBeforeAbsorbsOrStagger = p_Player->GetHealth();

            float l_AddCounter = l_DamageTakenBeforeAbsorbsOrStagger / p_Player->GetMaxHealth();

            /// Blue post: TalentedOrbChance = BaseOrbChance * (1 + 0.75 * (1 - (HealthBeforeDamage - DamageTakenBeforeAbosorbsOrStagger) / MaxHealth))
            if (AuraEffect const* l_GiftOfTheMists = p_Player->GetAuraEffect(eSpells::GiftOfTheMists, EFFECT_0)) ///< Gift of the Mists
                l_AddCounter *= (1.0f + ((float)l_GiftOfTheMists->GetAmount() / 100.0f) * (1.0f - (p_Player->GetHealth() - l_DamageTakenBeforeAbsorbsOrStagger) / p_Player->GetMaxHealth()));

            /// Prevent some exploits and bugs
            if (l_AddCounter > 1.0f)
                l_AddCounter = 1.0f;

            float& l_CurrentCounter = p_Player->m_SpellHelper.GetFloat(eSpellHelpers::GiftOfTheOxCounter);

            l_CurrentCounter += l_AddCounter;

            if (l_CurrentCounter >= 1.0f)
            {
                l_CurrentCounter -= 1.0f;

                if (roll_chance_i(50))
                    p_Player->CastSpell(p_Player, eSpells::GiftOfTheOxLeft, true);
                else
                    p_Player->CastSpell(p_Player, eSpells::GiftOfTheOxRight, true);

                /// Cast Spell Healing Sphere Active, required for some caster aura spell (115072) Expel Harm
                p_Player->CastSpell(p_Player, eSpells::HealingSphereCounter, true);
                p_Player->AddSpellCooldown(eSpells::GiftOfTheOx, 0, 3 * IN_MILLISECONDS); ///< internal cooldown, i can't find any information to get the real value so it's a guessed value ...
            }
        }
    }
 };

/// Last Update: 7.1.5 23420
/// Called by Spirit Tether - 199387
class spell_monk_spirit_tether : public SpellScriptLoader
{
    public:
        spell_monk_spirit_tether() : SpellScriptLoader("spell_monk_spirit_tether") { }

        class spell_monk_spirit_tether_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_spirit_tether_AuraScript);

            enum eArtifactPowers
            {
                SpiritTetherArtifact = 945
            };

            enum eSpells
            {
                SpiritTether = 199384
            };

            void HandleCalcAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpiritTether);
                Unit* l_Caster = GetCaster();

                if (!l_SpellInfo || !l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::SpiritTetherArtifact);
                    amount =  l_SpellInfo->Effects[0].BasePoints + ((l_Rank - 1) * ( l_SpellInfo->Effects[0].BasePoints / 2));
                    amount *= -1;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_spirit_tether_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_spirit_tether_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Collidus the Warp-Watcher's Gaze - 217473
class spell_monk_collidus_the_warp_watchers_gaze : public SpellScriptLoader
{
    public:
        spell_monk_collidus_the_warp_watchers_gaze() : SpellScriptLoader("spell_monk_collidus_the_warp_watchers_gaze") { }

        class spell_monk_collidus_the_warp_watchers_gaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_collidus_the_warp_watchers_gaze_AuraScript);

            enum eSpells
            {
                SoothingMist = 115175
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::SoothingMist)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_collidus_the_warp_watchers_gaze_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_collidus_the_warp_watchers_gaze_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dark Side of the Moon Rank 1 - 213062 / Rank 2 - 227688 / Rank 3 - 227689 / Rank 4 - 227690 / Rank 5 - 227691 / Rank 6 - 227692
class spell_monk_dark_side_of_the_moon : public SpellScriptLoader
{
public:
    spell_monk_dark_side_of_the_moon() : SpellScriptLoader("spell_monk_dark_side_of_the_moon") { }

    class spell_monk_dark_side_of_the_moon_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_dark_side_of_the_moon_AuraScript);

        enum eSpells
        {
            BlackoutStrike = 205523
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::BlackoutStrike)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_monk_dark_side_of_the_moon_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_monk_dark_side_of_the_moon_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dark Side of the Moon - 213063
class spell_monk_dark_side_of_the_moon_debuff : public SpellScriptLoader
{
public:
    spell_monk_dark_side_of_the_moon_debuff() : SpellScriptLoader("spell_monk_dark_side_of_the_moon_debuff") { }

    class spell_monk_dark_side_of_the_moon_debuff_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_dark_side_of_the_moon_debuff_AuraScript);

        enum eSpells
        {
            BlackoutStrike = 205523
        };

        void HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Target = (Unit*)GetOwner();
            if (!l_Target)
                return;

            l_Target->RemoveAura(GetId());
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_monk_dark_side_of_the_moon_debuff_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_monk_dark_side_of_the_moon_debuff_AuraScript();
    }
};

/// Last Update: 7.1.5 23420
/// Called by Chi Orbit - 196743
class spell_monk_chi_orbit : public SpellScriptLoader
{
    public:
        spell_monk_chi_orbit() : SpellScriptLoader("spell_monk_chi_orbit") { }

        class spell_monk_chi_orbit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_chi_orbit_AuraScript);

            enum eSpells
            {
                FirstOrb = 196744,
                SecondOrb = 196745,
                ThirdOrb = 196746,
                FourthOrb = 196747,

                MaxOrbs = 4
            };

            std::array<uint32, eSpells::MaxOrbs> m_Orbs =
            {
                {
                    eSpells::FirstOrb,
                    eSpells::SecondOrb,
                    eSpells::ThirdOrb,
                    eSpells::FourthOrb
                }
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint32 m_OrbSpell : m_Orbs)
                {
                    if (l_Caster->HasAura(m_OrbSpell))
                        continue;

                    l_Caster->CastSpell(l_Caster, m_OrbSpell, true);

                    AuraEffect* l_AuraEff = l_Caster->GetAuraEffect(m_OrbSpell, EFFECT_0);
                    if (!l_AuraEff)
                        return;

                    l_AuraEff->SetCanBeRecalculated(false);

                    break;
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::FirstOrb);
                l_Caster->RemoveAura(eSpells::SecondOrb);
                l_Caster->RemoveAura(eSpells::ThirdOrb);
                l_Caster->RemoveAura(eSpells::FourthOrb);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_chi_orbit_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_chi_orbit_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_chi_orbit_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Blackout Strike - 205523 - Breath of Fire - 115181
/// Called for Mastery: Elusive Brawler - 117906
class spell_monk_elusive_brawler : public SpellScriptLoader
{
public:
    spell_monk_elusive_brawler() : SpellScriptLoader("spell_monk_elusive_brawler") { }

    class spell_monk_elusive_brawler_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_elusive_brawler_SpellScript);

        enum eSpells
        {
            ElusiveBrawlerAura      = 117906,
            ElusiveBrawlerSpell     = 195630,
            T21Brewmaster2PBonus    = 251829,
            BreathOfFire            = 115181
        };

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            if (!l_Caster->HasAura(eSpells::ElusiveBrawlerAura))
                return;

            if (m_scriptSpellId == eSpells::BreathOfFire)
            {
                if (m_OnHit && !l_Caster->HasAura(eSpells::T21Brewmaster2PBonus))
                    return;
                else m_OnHit = true;
            }

            l_Caster->CastSpell(l_Caster, eSpells::ElusiveBrawlerSpell, true);
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_monk_elusive_brawler_SpellScript::HandleOnHit);
        }

    private:
        bool m_OnHit = false;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_monk_elusive_brawler_SpellScript();
    }
};

/// Last update 7.3.5
/// Called by Legendary: Soul of the Grandmaster - 247558
class spell_monk_soul_of_the_grandmaster : public SpellScriptLoader
{
    public:
        spell_monk_soul_of_the_grandmaster() : SpellScriptLoader("spell_monk_soul_of_the_grandmaster") { }

        class spell_monk_soul_of_the_grandmaster_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_soul_of_the_grandmaster_AuraScript);

            enum eSpells
            {
                ChiOrbit            = 196743,
                MisticVitality      = 237076,
                MistWrap            = 197900
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
                    case SPEC_MONK_BREWMASTER:
                        l_Player->CastSpell(l_Player, eSpells::MisticVitality, true);
                        break;
                    case SPEC_MONK_WINDWALKER:
                        l_Player->CastSpell(l_Player, eSpells::ChiOrbit, true);
                        break;
                    default:
                        l_Player->CastSpell(l_Player, eSpells::MistWrap, true);
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
                    case SPEC_MONK_BREWMASTER:
                        l_Player->RemoveAura(eSpells::MisticVitality);
                        break;
                    case SPEC_MONK_WINDWALKER:
                        l_Player->RemoveAura(eSpells::ChiOrbit);
                        break;
                    default:
                        l_Player->RemoveAura(eSpells::MistWrap);
                        break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_soul_of_the_grandmaster_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_soul_of_the_grandmaster_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_soul_of_the_grandmaster_AuraScript();
        }
};

/// Last Update 7.3.5
/// Soul of the Grandmaster - 247558
class PlayerScript_monk_legendary_soul_of_the_grandmaster : public PlayerScript
{
public:
    PlayerScript_monk_legendary_soul_of_the_grandmaster() : PlayerScript("PlayerScript_monk_legendary_soul_of_the_grandmaster") { }

    enum eSpells
    {
        ChiOrbit = 196743,
        MisticVitality = 237076,
        MistWrap = 197900,
        SoulOfTheGrandmaster = 247558
    };

    void ApplyLegendaryTalent(Player* p_Player)
    {
        if (!p_Player->HasAura(eSpells::SoulOfTheGrandmaster))
            return;

        switch (p_Player->GetActiveSpecializationID())
        {
            case SPEC_MONK_BREWMASTER:
                p_Player->CastSpell(p_Player, eSpells::MisticVitality, true);
                break;
            case SPEC_MONK_WINDWALKER:
                p_Player->CastSpell(p_Player, eSpells::ChiOrbit, true);
                break;
            default:
                p_Player->CastSpell(p_Player, eSpells::MistWrap, true);
                break;
        }
    }

    void OnSpellLearned(Player* p_Player, uint32) override
    {
        ApplyLegendaryTalent(p_Player);
    }

    void OnSpellRemoved(Player* p_Player, uint32) override
    {
        ApplyLegendaryTalent(p_Player);
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Tigereye Brew - 247483, 248643, 248689
class spell_monk_tigereyebrew: public SpellScriptLoader
{
public:
    spell_monk_tigereyebrew() : SpellScriptLoader("spell_monk_tigereyebrew") { }

    enum eSpells
    {
        TigereyeBrewTalent      = 247483,
        TigereyeBrewProcAura    = 248643,
        TigereyeBrewTrigger     = 248689,
        TigereyeBrewVisual      = 248648,
        TigereyeBrewAura        = 248646
    };

    class spell_monk_tigereyebrew_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_tigereyebrew_SpellScript);

        void HandleDummy(SpellEffIndex /*p_EffectIndex*/)
        {
            if (Unit* l_Caster = GetCaster())
            {
                l_Caster->CastSpell(l_Caster, eSpells::TigereyeBrewVisual, true);
                l_Caster->CastSpell(l_Caster, eSpells::TigereyeBrewAura, true);
            }
        }

        void Register() override
        {
            if (m_scriptSpellId == eSpells::TigereyeBrewTrigger)
                OnEffectHitTarget += SpellEffectFn(spell_monk_tigereyebrew_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    class spell_monk_tigereyebrew_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_tigereyebrew_AuraScript);

        void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::TigereyeBrewTrigger, true);
        }

        void HandleOnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Mode*/)
        {
            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            if (!l_Caster || !l_Aura)
                return;

            int32 l_Duration = l_Aura->GetDuration();
            int32 l_Stacks = 0;
            int32 l_TempStacks = 0;
            if (Aura* l_Aura2 = l_Caster->GetAura(eSpells::TigereyeBrewAura))
            {
                l_Stacks = l_Aura2->GetStackAmount();
                l_TempStacks = l_Aura2->GetStackAmount();
                if (l_TempStacks > p_AuraEffect->GetAmount())
                {
                    l_Stacks = p_AuraEffect->GetAmount();
                    l_Aura2->SetStackAmount(l_TempStacks - p_AuraEffect->GetAmount());
                }
            }

            l_Aura->SetDuration(l_Duration * l_Stacks);

            if (l_TempStacks <= p_AuraEffect->GetAmount())
            {
                l_Caster->RemoveAura(eSpells::TigereyeBrewVisual);
                l_Caster->RemoveAura(eSpells::TigereyeBrewAura);
            }
        }

        void Register() override
        {
            if (m_scriptSpellId == eSpells::TigereyeBrewTalent)
                AfterEffectApply += AuraEffectApplyFn(spell_monk_tigereyebrew_AuraScript::HandleOnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            if (m_scriptSpellId == eSpells::TigereyeBrewProcAura)
                OnEffectProc += AuraEffectProcFn(spell_monk_tigereyebrew_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_monk_tigereyebrew_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_monk_tigereyebrew_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Tigereye Brew - 247483
class PlayerScript_TigereyeBrew : public PlayerScript
{
public:
    PlayerScript_TigereyeBrew() : PlayerScript("PlayerScript_TigereyeBrew") { }

    enum eSpells
    {
        TigereyeBrewTalent = 247483,
        TigereyeBrewProcAura = 248643,
        TigereyeBrewTrigger = 248689
    };

    /*void OnPlayerSpellLearned(Player* p_Player, uint32 p_SpellID)
    {
        if (p_SpellID == eSpells::TigereyeBrewTalent)
            p_Player->CastSpell(p_Player, eSpells::TigereyeBrewProcAura, true);
    }

    void OnPlayerSpellRemoved(Player* p_Player, uint32 p_SpellID)
    {
        if (p_SpellID == eSpells::TigereyeBrewTalent)
            p_Player->RemoveAura(eSpells::TigereyeBrewProcAura);
    }*/

    void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool /*p_Regen*/, bool p_After)
    {
        if (p_After)
            return;

        if (p_Power != POWER_CHI)
            return;

        if (!p_Player->HasSpell(eSpells::TigereyeBrewTalent))
            return;

        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TigereyeBrewTalent);
        if (!l_SpellInfo)
            return;

        if ((p_OldValue - p_NewValue) > 0)
            m_SpendValue += p_OldValue - p_NewValue;
        if (m_SpendValue >= l_SpellInfo->Effects[EFFECT_2].BasePoints)
        {
            p_Player->CastSpell(p_Player, eSpells::TigereyeBrewTrigger, true);
            m_SpendValue = 0;
        }
    }

private:
    int32 m_SpendValue = 0;
};

/// Last Update 7.3.2 Build 25549
/// Paralysis - 115078
class spell_monk_paralysis : public SpellScriptLoader
{
    public:
        spell_monk_paralysis() : SpellScriptLoader("spell_monk_paralysis") { }

        class spell_monk_paralysis_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_paralysis_SpellScript);

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
                OnHit += SpellHitFn(spell_monk_paralysis_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_paralysis_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Way of the Crane - 216113, 216161
class spell_monk_way_of_the_crane : public SpellScriptLoader
{
public:
    spell_monk_way_of_the_crane() : SpellScriptLoader("spell_monk_way_of_the_crane") { }

    enum eSpells
    {
        Passive = 216113,
        Heal = 216161
    };

    class spell_monk_way_of_the_crane_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_way_of_the_crane_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Passive);
            if (!l_SpellInfo)
                return;

            int32 l_NbTargets = l_SpellInfo->Effects[EFFECT_4].BasePoints;

            if (p_Targets.size() > l_NbTargets)
            {
                p_Targets.sort(JadeCore::HealthPctOrderPred());
                p_Targets.resize(l_NbTargets);
            }
            else
                p_Targets.sort(JadeCore::HealthPctOrderPred());
        }

        void Register()
        {
            if (m_scriptSpellId == eSpells::Heal)
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_way_of_the_crane_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        }
    };

    class spell_monk_way_of_the_crane_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_way_of_the_crane_AuraScript);

        void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            PreventDefaultAction();

            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return;

            int32 l_Absorb = l_DamageInfo->GetAbsorb();
            int32 l_Damage = l_DamageInfo->GetAmount();
            int32 l_Amount = l_Absorb + l_Damage;
            l_Amount = CalculatePct(l_Amount, GetSpellInfo()->Effects[EFFECT_3].BasePoints + 10); ///< +10 from May hotfix
            l_Caster->CastCustomSpell(l_Caster, eSpells::Heal, &l_Amount, NULL, NULL, true);
        }

        void HandleDamagePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if (!l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount = l_AuraEffect->GetBaseAmount() * 0.60f;
        }

        void Register() override
        {
            if (m_scriptSpellId == eSpells::Passive)
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_way_of_the_crane_AuraScript::HandleDamagePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                OnEffectProc += AuraEffectProcFn(spell_monk_way_of_the_crane_AuraScript::OnProc, EFFECT_3, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_monk_way_of_the_crane_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_monk_way_of_the_crane_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Essence Font - 191837
/// Called for Doorway to Nowhere - 116680
class spell_monk_doorway_to_nowhere: public SpellScriptLoader
{
    public:
        spell_monk_doorway_to_nowhere() : SpellScriptLoader("spell_monk_doorway_to_nowhere") { }

        enum eSpells
        {
            DoorwaytoNowhereAura   = 248035,
            DoorwaytoNowhereSummon = 248293
        };

        class spell_monk_doorway_to_nowhere_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_doorway_to_nowhere_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::DoorwaytoNowhereAura, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                if (!roll_chance_i(l_AuraEffect->GetAmount()))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DoorwaytoNowhereSummon, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_monk_doorway_to_nowhere_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_doorway_to_nowhere_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Whispers of Shaohao - 242400
class spell_monk_whispers_of_shaohao : public SpellScriptLoader
{
    public:
        spell_monk_whispers_of_shaohao() : SpellScriptLoader("spell_monk_whispers_of_shaohao") { }

        class spell_monk_whispers_of_shaohao_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_whispers_of_shaohao_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (GetCaster())
                    p_Targets.remove(GetCaster());

                if (p_Targets.size() > 1)
                {
                    p_Targets.sort(JadeCore::HealthPctOrderPred());
                    p_Targets.resize(1);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_whispers_of_shaohao_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_whispers_of_shaohao_SpellScript();
        }
};

/// Last update 7.1.5
/// Zen Pilgrimage : Return - 126896
class spell_monk_zen_pilgrimage_return : public SpellScriptLoader
{
    public:
        spell_monk_zen_pilgrimage_return() : SpellScriptLoader("spell_monk_zen_pilgrimage_return") { }

        class spell_monk_zen_pilgrimage_return_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_zen_pilgrimage_return_AuraScript);

            void OnUpdate(uint32 p_Diff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                if (l_Caster->GetMapId() != 1514 && l_Caster->GetMapId() != 870)
                    Remove();
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_monk_zen_pilgrimage_return_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_zen_pilgrimage_return_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Rushing Jade Wind - 116847
class spell_monk_rushing_jade_visual: public SpellScriptLoader
{
    public:
        spell_monk_rushing_jade_visual() : SpellScriptLoader("spell_monk_rushing_jade_visual") { }

        class spell_monk_rushing_jade_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_rushing_jade_visual_AuraScript);

            enum eSpells
            {
                RusingJadeGeneric     = 163317,
                TransferThePowerTrait = 195300,
                TransferThePower      = 195321,
                MarkOfTheCrane        = 228287,
                CycloneStrikes        = 220358
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RusingJadeGeneric, true);
                l_Aura->Remove();

                if (l_Caster->HasAura(eSpells::TransferThePowerTrait))
                    l_Caster->CastSpell(l_Caster, eSpells::TransferThePower, true);

                Player* l_Player = l_Caster->GetSpellModOwner();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() != SPEC_MONK_WINDWALKER)
                    return;

                std::list<Unit*> l_UnitList;
                l_Caster->GetAttackableUnitListInRange(l_UnitList, 8.0f);

                uint8 l_MaxTargets = 2;
                uint8 l_CurTarget = 0;
                for (std::list<Unit*>::iterator l_Itr = l_UnitList.begin(); l_Itr != l_UnitList.end();)
                {
                    if (l_Player->IsValidAttackTarget(*l_Itr) && !(*l_Itr)->HasAura(eSpells::MarkOfTheCrane))
                    {
                        l_Player->CastSpell(*l_Itr, eSpells::MarkOfTheCrane, true);
                        ++l_CurTarget;

                        l_Itr = l_UnitList.erase(l_Itr);

                        if (l_CurTarget == l_MaxTargets)
                            break;
                    }
                    else
                    {
                        l_Itr++;
                    }
                }

                if (l_CurTarget >= l_MaxTargets || l_UnitList.empty())
                    return;

                for (Unit* l_Unit : l_UnitList)
                {
                    if (l_Player->IsValidAttackTarget(l_Unit))
                    {
                        l_Player->CastSpell(l_Unit, eSpells::MarkOfTheCrane, true);
                        ++l_CurTarget;

                        if (l_CurTarget == l_MaxTargets)
                            break;
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_rushing_jade_visual_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_rushing_jade_visual_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Rushing Jade Wind - 163317
class spell_monk_rushing_jade_amplitude : public SpellScriptLoader
{
    public:
        spell_monk_rushing_jade_amplitude() : SpellScriptLoader("spell_monk_rushing_jade_amplitude") { }

        class spell_monk_rushing_jade_amplitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_rushing_jade_amplitude_AuraScript);

            void CalcValues(Unit* l_Caster, int32& p_Duration, int32& p_Amplitude)
            {
                float l_Haste = 1.0f / l_Caster->GetFloatValue(UNIT_FIELD_MOD_HASTE);
                p_Duration = (6 * IN_MILLISECONDS);
                p_Amplitude = 750;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Caster->ToPlayer() && l_Caster->ToPlayer()->GetActiveSpecializationID() == SPEC_MONK_BREWMASTER)
                {
                    constexpr uint32 l_Pct = 50;
                    AddPct(p_Duration, l_Pct);
                    AddPct(p_Amplitude, l_Pct);
                }

                p_Duration /= l_Haste;
                p_Amplitude /= l_Haste;
            }

            void HandleOnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                int32 l_Duration = 0;
                int32 l_Amplitude = 0;

                CalcValues(l_Target, l_Duration, l_Amplitude);

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmplitude(l_Amplitude);
            }

            void CalculateMaxDuration(int32& p_Duration)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Duration = 0;
                int32 l_Amplitude = 0;

                CalcValues(l_Caster, l_Duration, l_Amplitude);

                p_Duration = l_Duration;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_monk_rushing_jade_amplitude_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_monk_rushing_jade_amplitude_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_rushing_jade_amplitude_AuraScript();
        }
};

/// Last update 7.3.5 build 26124
/// Called By Thunderfist - 242390
class spell_monk_thunderfist_damage : public SpellScriptLoader
{
    public:
        spell_monk_thunderfist_damage() : SpellScriptLoader("spell_monk_thunderfist_damage") { }

        class spell_monk_thunderfist_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_thunderfist_damage_SpellScript);

            enum eSpells
            {
                ThunderfistTrait            = 238131,
                ThunderfistBuff             = 242387
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::ThunderfistTrait))
                    if (Aura* l_ThunderfistAura = l_Caster->GetAura(eSpells::ThunderfistBuff))
                        l_ThunderfistAura->DropStack();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monk_thunderfist_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_monk_thunderfist_damage_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Master Of Combinations - 238095
class spell_monk_master_of_combination : public SpellScriptLoader
{
    public:
        spell_monk_master_of_combination() : SpellScriptLoader("spell_monk_master_of_combination") { }

        class spell_monk_master_of_combination_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_master_of_combination_AuraScript);

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return false;

                uint32 l_PrecursorSpell = 0;
                uint32 l_PrevSpell = l_Player->GetLastCastedSpell();
                bool FistOfFury = false;

                switch (l_SpellInfo->Id)
                {
                    case 185099:
                        l_PrecursorSpell = 107428;
                        break;
                    case 117418:
                        l_PrecursorSpell = 113656;
                        FistOfFury = true;
                        break;
                    case 100784:
                    {
                        if (l_Caster->HasAura(116768))
                            l_PrecursorSpell = 100780;
                        else
                            l_PrecursorSpell = l_SpellInfo->Id;
                        break;
                    }
                    case 123586:
                        l_PrecursorSpell = 115057;
                        break;
                    default:
                        l_PrecursorSpell = l_SpellInfo->Id;
                        break;
                }

                if (l_PrecursorSpell != l_PrevSpell || FistOfFury)
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_master_of_combination_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_master_of_combination_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Master Of Combinations - 240672
class spell_monk_master_of_combination_targets : public SpellScriptLoader
{
public:
    spell_monk_master_of_combination_targets() : SpellScriptLoader("spell_monk_master_of_combination_targets") { }

    enum eSpells
    {
        MasterOfCombinations = 238095
    };

    class spell_monk_master_of_combination_targets_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_master_of_combination_targets_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MasterOfCombinations);
            if (!l_SpellInfo)
                return;

            uint8 l_MaxTargets = l_SpellInfo->Effects[EFFECT_1].BasePoints + 1; /// + 1 caster

            if (p_Targets.size() > l_MaxTargets)
                p_Targets.resize(l_MaxTargets);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_master_of_combination_targets_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ALLY_OR_RAID);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_monk_master_of_combination_targets_SpellScript();
    }
};

/// Last Update 7.3.5
/// Fortune Turned - 216913
class spell_monk_fortune_turned : public SpellScriptLoader
{
public:
    spell_monk_fortune_turned() : SpellScriptLoader("spell_monk_fortune_turned") { }

    class spell_monk_fortune_turned_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_fortune_turned_AuraScript);

        enum eSpells
        {
            Effuse = 116694,
            Effuce2 = 227344
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::Effuse && l_SpellInfo->Id != eSpells::Effuce2))
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_monk_fortune_turned_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_monk_fortune_turned_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Flying Serpent Kick - 101545
/// Called for Ride the Wind - 201372
class spell_monk_ride_the_wind: public SpellScriptLoader
{
    public:
        spell_monk_ride_the_wind() : SpellScriptLoader("spell_monk_ride_the_wind") { }

        class spell_monk_ride_the_wind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_ride_the_wind_SpellScript);

            enum eSpells
            {
                RideTheWind         = 201372,
                RideTheWindPeriodic = 201376
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::RideTheWind))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RideTheWindPeriodic, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_ride_the_wind_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_ride_the_wind_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
class spell_monk_drinking_horn_cover_serenity: public SpellScriptLoader
{
    public:
        spell_monk_drinking_horn_cover_serenity() : SpellScriptLoader("spell_monk_drinking_horn_cover_serenity") { }

        class spell_monk_drinking_horn_cover_serenity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_drinking_horn_cover_serenity_SpellScript);

            enum eSpells
            {
                Serenity          = 152173,
                DrinkingHornCover = 209256,

                /// Called by:
                BlackoutKick            = 100784,
                SpinningCraneKick       = 101546,
                RisingSunKick           = 107428,
                FistsOfFury             = 113656,
                RushingJadeWind         = 116847,
                StrikeOfTheWindlord     = 205320
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Serenity))
                {
                    if (l_Caster->HasAura(eSpells::DrinkingHornCover))
                    {
                        if (AuraEffect* l_Effect = l_Caster->GetAuraEffect(eSpells::DrinkingHornCover, EFFECT_1))
                        {
                            int32 l_AddDuration = (l_Effect->GetAmount() / 10.0f) * TimeConstants::IN_MILLISECONDS;

                            l_Aura->SetDuration(l_Aura->GetDuration() + l_AddDuration);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_drinking_horn_cover_serenity_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_monk_drinking_horn_cover_serenity_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Monk T20 Brewmaster 2P Bonus - 242255
/// Called by Fortifying Brew - 115203, Ironskin Brew - 115308, Purifying Brew - 119582, Black Ox Brew - 115399
class spell_monk_T20Brewmaster2PBonus : public SpellScriptLoader
{
    public:
        spell_monk_T20Brewmaster2PBonus() : SpellScriptLoader("spell_monk_T20Brewmaster2PBonus") { }

        enum eSpells
        {
            Brewmaster2PBonus       = 242255,
            GiftOfTheOxLeft         = 124503,
            GiftOfTheOxRight        = 124506,
            HealingSphereCounter    = 224863
        };

        class spell_monk_T20Brewmaster2PBonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_T20Brewmaster2PBonus_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::Brewmaster2PBonus))
                    return;

                const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Brewmaster2PBonus);
                if (!l_SpellInfo)
                    return;

                if (roll_chance_i(l_SpellInfo->ProcChance))
                {
                    if (roll_chance_i(50))
                        l_Caster->CastSpell(l_Caster, eSpells::GiftOfTheOxLeft, true);
                    else
                        l_Caster->CastSpell(l_Caster, eSpells::GiftOfTheOxRight, true);

                    /// Cast Spell Healing Sphere Active, required for some caster aura spell (115072) Expel Harm
                    l_Caster->CastSpell(l_Caster, eSpells::HealingSphereCounter, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_T20Brewmaster2PBonus_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_T20Brewmaster2PBonus_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Rising Sun Kick - 107428
/// Called for Rising Thunder - 210804
class spell_monk_rising_thunder: public SpellScriptLoader
{
    public:
        spell_monk_rising_thunder() : SpellScriptLoader("spell_monk_rising_thunder") { }

        class spell_monk_rising_thunder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_rising_thunder_SpellScript);

            enum eSpells
            {
                RisingThunder   = 210804,
                ThunderFocusTea = 116680
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::RisingThunder))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->RemoveSpellCooldown(eSpells::ThunderFocusTea, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_monk_rising_thunder_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_rising_thunder_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Surge of Mists - 246328
/// Called for Item - Monk T20 Mistweaver 4P Bonus - 242258
class spell_monk_t20_mistweaver_4p_bonus : public SpellScriptLoader
{
    public:
        spell_monk_t20_mistweaver_4p_bonus() : SpellScriptLoader("spell_monk_t20_mistweaver_4p_bonus") { }

        class spell_monk_t20_mistweaver_4p_bonus_AuraScript : public AuraScript
        {
            enum eSpells
            {
                T20Mistweaver4P                   = 242258,
                DanceOfMists                      = 247891
            };

            PrepareAuraScript(spell_monk_t20_mistweaver_4p_bonus_AuraScript);

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();

                /// Consumed spell mods are removed with AURA_REMOVE_BY_EXPIRE
                if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE && GetAura() && !GetAura()->GetDuration())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::T20Mistweaver4P))
                    l_Caster->DelayedCastSpell(l_Caster, eSpells::DanceOfMists, true, 1);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_monk_t20_mistweaver_4p_bonus_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_t20_mistweaver_4p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Heal elexir - 122281
class spell_monk_healing_elixir : public SpellScriptLoader
{
    public:
        spell_monk_healing_elixir() : SpellScriptLoader("spell_monk_healing_elixir") { }

        class spell_monk_healing_elixir_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_healing_elixir_SpellScript);

            enum eSpells
            {
                Serenity = 152173
            };

            void HandleHeal()
            {
                Unit * l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (AuraEffect const* l_SerenityEffect = l_Caster->GetAuraEffect(eSpells::Serenity, SpellEffIndex::EFFECT_2))
                {
                    int32 l_Heal = GetHitHeal();
                    AddPct(l_Heal, l_SerenityEffect->GetAmount());
                    SetHitHeal(l_Heal);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_healing_elixir_SpellScript::HandleHeal);
            }
        };

        SpellScript * GetSpellScript() const override
        {
            return new spell_monk_healing_elixir_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Item - Monk T21 Mistweaver 2P Bonus - 251825
class spell_monk_t21_mistweaver_2p_bonus : public SpellScriptLoader
{
    public:
        spell_monk_t21_mistweaver_2p_bonus() : SpellScriptLoader("spell_monk_t21_mistweaver_2p_bonus") { }

        class spell_monk_t21_mistweaver_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_t21_mistweaver_2p_bonus_AuraScript);

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_t21_mistweaver_2p_bonus_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_t21_mistweaver_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Item - Monk T21 Mistweaver 4P Bonus - 251826
class spell_monk_t21_mistweaver_4p_bonus : public SpellScriptLoader
{
    public:
        spell_monk_t21_mistweaver_4p_bonus() : SpellScriptLoader("spell_monk_t21_mistweaver_4p_bonus") { }

        class spell_monk_t21_mistweaver_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_t21_mistweaver_4p_bonus_AuraScript);

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_monk_t21_mistweaver_4p_bonus_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_t21_mistweaver_4p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Chi Bolt - 253581
class spell_monk_chi_bolt : public SpellScriptLoader
{
public:
    spell_monk_chi_bolt() : SpellScriptLoader("spell_monk_chi_bolt") { }

    class spell_monk_chi_bolt_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_chi_bolt_SpellScript);

        enum eSpells
        {
            TranquilMist = 253448
        };

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            p_Targets.remove_if(JadeCore::UnitAuraCheck(false, eSpells::TranquilMist, l_Caster->GetGUID()));
            p_Targets.sort(JadeCore::HealthPctOrderPred());
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_chi_bolt_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_monk_chi_bolt_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Blackout Kick! - 116768
class spell_monk_blackout_kick : public SpellScriptLoader
{
public:
    spell_monk_blackout_kick() : SpellScriptLoader("spell_monk_blackout_kick") { }

    class spell_monk_blackout_kick_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_blackout_kick_AuraScript);

        enum eSpells
        {
            T21Windwalker4PBonus = 251823,
            BlackoutKick = 116768
        };

        void HandleAfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (Aura* l_Aura = l_Caster->GetAura(eSpells::T21Windwalker4PBonus))
            {
                if (AuraEffect* l_AuraEffect0 = l_Aura->GetEffect(EFFECT_0))
                {
                    if (roll_chance_i(l_AuraEffect0->GetAmount()))
                    {
                        if (AuraEffect* l_AuraEffect1 = l_Aura->GetEffect(EFFECT_1))
                        {
                            l_Caster->AddDelayedEvent([l_Caster, l_Amount = l_AuraEffect1->GetAmount()]()
                            {
                                l_Caster->EnergizeBySpell(l_Caster, eSpells::BlackoutKick, l_Amount, Powers::POWER_CHI);
                            }, 100);
                        }
                    }
                }
            }
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_monk_blackout_kick_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_monk_blackout_kick_AuraScript();
    }
};

// 213136 - Gifted Student
class freakz_spell_gifted_student : public SpellScriptLoader
{
    public:
        freakz_spell_gifted_student() : SpellScriptLoader("freakz_spell_gifted_student") { }

        class freakz_spell_gifted_student_AuraScript : public AuraScript
        {
            PrepareAuraScript(freakz_spell_gifted_student_AuraScript);

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (!(eventInfo.GetHitMask() & ProcFlagsExLegacy::PROC_EX_DODGE))
                    return;

                int32 amount = aurEff->GetAmount();
                caster->CastCustomSpell(caster, aurEff->GetSpellEffectInfo()->TriggerSpell, &amount, nullptr, nullptr, nullptr, nullptr, nullptr, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(freakz_spell_gifted_student_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new freakz_spell_gifted_student_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_monk_spell_scripts()
{
    new PlayerScript_ObstinateDetermination();
    new PlayerScript_GiftOfTheOx();
    new spell_monk_spirit_of_the_crane();
    new spell_monk_spirit_tether();
    new spell_monk_counteract_magic();
    new spell_monk_life_cocoon();
    new spell_monk_protection_of_shaohao();
    new spell_monk_light_on_your_feet();
    new spell_monk_high_tolerance();
    new spell_monk_tiger_style();
    new spell_monk_tornado_kicks();
    new spell_monk_mists_of_life();
    new spell_monk_ovyds_winter_wrap();
    new spell_monk_the_mists_of_sheilun();
    new spell_monk_crane_heal_target();
    new spell_monk_crane_heal();
    new spell_monk_sheiluns_gift();
    new spell_monk_soothing_mist_heal();
    new spell_monk_essence_font_ticks();
    new spell_monk_essence_font();
    new spell_monk_zen_pulse();
    new spell_monk_zen_pulse_heal();
    new spell_monk_vivify();
    new spell_monk_blackout_combo();
    new spell_monk_special_delivery_damage();
    new spell_monk_black_ox_brew();
    new spell_monk_celestial_fortune();
    new spell_monk_eye_of_the_tiger();
    new spell_monk_fortifying_brew();
    new spell_monk_windwalker_monk();
    new spell_monk_combo_breaker();
    new spell_monk_chi_brew();
    new spell_monk_chi_wave_healing_bolt();
    new spell_monk_chi_wave_bolt();
    new spell_monk_chi_wave();
    new spell_monk_dampen_harm();
    new spell_monk_item_s12_4p_mistweaver();
    new spell_monk_diffuse_magic();
    new spell_monk_black_ox_statue();
    new spell_monk_zen_flight_check();
    new spell_monk_tiger_palm();
    new spell_monk_crackling_jade_lightning();
    new spell_monk_touch_of_karma();
    new spell_monk_thunder_focus_tea();
    new spell_monk_jade_serpent_statue();
    new spell_monk_enveloping_mist();
    new spell_monk_renewing_mist_hot();
    new spell_monk_renewing_mist();
    new spell_monk_healing_elixirs();
    new spell_monk_spear_hand_strike();
    new spell_monk_tigers_lust();
    new spell_monk_flying_serpent_kick();
    new spell_monk_chi_torpedo();
    new spell_monk_purifying_brew();
    new spell_monk_keg_smash();
    new spell_monk_ironskin_brew();
    new spell_monk_breath_of_fire();
    new spell_monk_disable();
    new spell_monk_zen_pilgrimage();
    new spell_monk_touch_of_death();
    new spell_monk_provoke();
    new spell_monk_provoke_launch();
    new spell_monk_roll();
    new spell_monk_expel_harm();
    new spell_monk_gift_of_the_ox_heal();
    new spell_monk_detox();
    new spell_monk_afterlife();
    new spell_monk_fists_of_fury();
    new spell_monk_fists_of_fury_damage();
    new spell_monk_fists_of_fury_stun();
    new spell_monk_WoDPvPBrewmaster2PBonus();
    new spell_monk_crackling_tiger_lightning();
    new spell_monk_item_t17_brewmaster_2p_bonus();
    new spell_monk_serenity();
    new spell_monk_transfer_the_power();
    new spell_monk_teachings_of_the_monastery();
    new spell_monk_elusive_brawler();
    new spell_monk_paralysis();
    new spell_monk_doorway_to_nowhere();
    new spell_monk_whispers_of_shaohao();
    new spell_monk_zen_pilgrimage_return();
    new spell_monk_t21_mistweaver_2p_bonus();
    new spell_monk_t21_mistweaver_4p_bonus();
    new spell_monk_chi_bolt();
    new spell_monk_blackout_kick();

    /// All Spec Spells
    new spell_monk_transcendence_transfer();
    new spell_monk_transcendence();
    new spell_monk_effuse();
    new spell_monk_soul_of_the_grandmaster();
    new PlayerScript_monk_legendary_soul_of_the_grandmaster();

    /// MisteWeaver
    new spell_monk_soothing_mist();
    new spell_monk_sheiluns_gift_active();
    new spell_monk_petrichor_lagniappe();
    new spell_monk_shelter_of_rin();
    new spell_monk_the_black_flames_gamblee();
    new spell_monk_unison();
    new spell_monk_unison_target();
    new spell_monk_unison_periodic();
    new spell_monk_focused_thunder();
    new spell_monk_collidus_the_warp_watchers_gaze();
    new spell_monk_rising_thunder();
    new spell_monk_t20_mistweaver_4p_bonus();
    new spell_monk_healing_elixir();

    /// Windwalker
    new spell_monk_healing_winds();
    new spell_monk_storm_earth_and_fire();
    new PlayerScript_Storm_Earth_And_Fire();
    new spell_monk_storm_earth_and_fire_fixate();
    new spell_monk_whirling_dragon_punch();
    new spell_monk_hit_combo();
    new spell_monk_spinning_crane_kick();
    new spell_monk_mark_of_the_crane();
    new spell_monk_the_emperors_capacitor();
    new spell_monk_chi_orbit();
    new spell_monk_windwalking();
    new spell_monk_rushing_jade_visual();
    new spell_monk_rushing_jade_amplitude();
    new spell_monk_chi_burst_spirits_copy();
    new spell_monk_rushing_jade_wind_spirits_copy();
    new spell_monk_spinning_crane_kick_spirits_copy();
    new spell_monk_drinking_horn_cover_serenity();
    new spell_monk_stance_of_the_fierce_tiger();

    /// Brewmaster
    new spell_monk_salsalabims_strength();
    new spell_monk_anvil_and_stave();
    new spell_monk_gai_plins_imperial_brew();
    new spell_monk_blackout_combo_proc();
    new spell_monk_stagger();
    new spell_monk_mighty_ox_kick();
    new spell_monk_dark_side_of_the_moon();
    new spell_monk_dark_side_of_the_moon_debuff();
    new spell_monk_T20Brewmaster2PBonus();

    /// Player Script
    new PlayerScript_Serenity();
    new PlayerScript_whirling_dragon_punch();
    new PlayerScript_DrinkingHornCover();
    new PlayerScript_Monk_T19_Windwalker_4P_Bonus();

    /// Honor Talents
    new spell_monk_zen_focus();
    new spell_monk_healing_sphere();
    new spell_monk_healing_sphere_cleanse();
    new spell_monk_guard();
    new spell_monk_admonishment();
    new spell_monk_whirling_kicks();
    new spell_monk_whirling_kicks_buff();
    new spell_monk_ancient_mistweaver_arts();
    new spell_monk_control_the_mists();
    new spell_monk_control_the_mists_cd();
    new spell_monk_refreshing_breeze();
    new spell_monk_tigereyebrew();
    new PlayerScript_TigereyeBrew();
    new spell_monk_way_of_the_crane();
    new spell_monk_fortune_turned();
    new spell_monk_ride_the_wind();

    /// Artifact Traits
    new spell_monk_good_karma();
    new spell_monk_crosswinds();
    new spell_monk_face_palm();
    new spell_monk_celestial_breath();
    new spell_monk_celestial_breath_heal();
    new spell_monk_gale_burst();
    new spell_monk_blessings_of_yulon();
    RegisterSpellScript(spell_monk_strike_of_the_windlord);
    new spell_monk_thunderfist_damage();
    new spell_monk_strength_of_xuen_addition();
    new spell_monk_master_of_combination();
    new spell_monk_master_of_combination_targets();

    new PlayerScript_monk_the_emperors_capacitor();

    new freakz_spell_gifted_student();
}
#endif
