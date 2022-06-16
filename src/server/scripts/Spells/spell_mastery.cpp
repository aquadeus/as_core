////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with MASTERY.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mastery_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "HelperDefines.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

enum MasterySpells
{
    MASTERY_SPELL_LIGHTNING_BOLT        = 45284,
    MASTERY_SPELL_CHAIN_LIGHTNING       = 45297,
    MASTERY_SPELL_LAVA_BURST            = 77451,
    MASTERY_SPELL_ELEMENTAL_BLAST       = 120588,
    MASTERY_SPELL_HAND_OF_LIGHT         = 96172,
    MASTERY_SPELL_IGNITE_AURA           = 12654,
    MASTERY_SPELL_BLOOD_SHIELD          = 77535,
    SPELL_DK_SCENT_OF_BLOOD             = 50421,
    SPELL_MAGE_MASTERY_ICICLES          = 76613,
    SPELL_MAGE_ICICLE_DAMAGE            = 148022,
    SPELL_MAGE_ICICLE_PERIODIC_TRIGGER  = 148023,
    SPELL_PRIEST_ECHO_OF_LIGHT          = 77489,
    SPELL_WARRIOR_WEAPONS_MASTER        = 76838,
    SPELL_WARLOCK_MASTER_DEMONOLOGIST   = 77219,
    SPELL_PRIEST_MENTAL_ANGUISH         = 77486,
    MASTERY_SPELL_IGNITE                = 12846
};

/// last update : 6.2.3
/// Mastery: Echo of Light - 77485
class spell_mastery_echo_of_light: public SpellScriptLoader
{
    public:
        spell_mastery_echo_of_light() : SpellScriptLoader("spell_mastery_echo_of_light") { }

        class spell_mastery_echo_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_echo_of_light_AuraScript);

            enum eSpells
            {
                T19Holy2P           = 211638,
                HolyWordSanctify    = 34861,
                LifeSteal           = 146347
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id == eSpells::LifeSteal)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                uint32 l_HealingValue = l_DamageInfo->GetAmount();
                if (!l_HealingValue)
                    return;

                Unit* l_UnitTarget = p_EventInfo.GetActionTarget();
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_UnitTarget || !l_Player)
                    return;

                if (l_Player->HasAura(eSpells::T19Holy2P) && l_SpellInfo->Id == eSpells::HolyWordSanctify)
                    l_HealingValue *= 2;

                float l_Mastery = l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * 5.0f / 4.0f / 100.0f;
                int32 l_Bp = (l_Mastery * l_HealingValue);

                if (Aura* l_HealAura = l_UnitTarget->GetAura(SPELL_PRIEST_ECHO_OF_LIGHT, l_Player->GetGUID()))
                {
                    uint32 l_RemainingTime = l_HealAura->GetDuration();
                    uint32 l_MaxDuration = l_HealAura->GetMaxDuration();
                    AuraEffect* l_AuraEffect = l_HealAura->GetEffect(EFFECT_0);
                    if (!l_AuraEffect)
                        return;

                    uint32 l_Amplitude = l_AuraEffect->GetAmplitude();
                    uint32 l_RemainingTick = (l_RemainingTime / l_Amplitude) + 1;
                    l_Bp += (l_AuraEffect->GetAmount() * l_RemainingTick);
                    if (l_RemainingTime < l_MaxDuration)
                    {
                        l_RemainingTime += l_Amplitude;
                        ++l_RemainingTick;
                        l_HealAura->SetDuration(l_RemainingTime);
                    }
                    l_Bp /= l_RemainingTick;
                    l_AuraEffect->SetAmount(l_Bp);
                }
                else
                {
                    l_Bp /= 2;
                    l_Player->CastCustomSpell(l_UnitTarget, SPELL_PRIEST_ECHO_OF_LIGHT, &l_Bp, NULL, NULL, true);
                }
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_mastery_echo_of_light_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_mastery_echo_of_light_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mastery_echo_of_light_AuraScript();
        }
};

#define MAX_ICICLES 5
const int IcicleAuras[MAX_ICICLES] = { 214130, 214127, 214126, 214125, 214124 };
const int IcicleHits[MAX_ICICLES] = { 214756, 214755, 214754, 214753, 214752 };
const int IcicleNormalHits[MAX_ICICLES] = {148021, 148020, 148019, 148018, 148017 };

/// Last Update : 7.3.2 Build 25549
/// Mastery: Icicles - 76613
class spell_mastery_icicles_proc : public SpellScriptLoader
{
    public:
        spell_mastery_icicles_proc() : SpellScriptLoader("spell_mastery_icicles_proc") { }

        class spell_mastery_icicles_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_icicles_proc_AuraScript);

            enum eSpells
            {
                SplittingIce        = 56377,
                FrosBolt            = 228597,
                GlacialSpikeTalent  = 199786,
                GlacialSpikeUsable  = 199844,
                BlackIce            = 195615,
                IcicleDamage        = 148022,
                BlackIcicleDamage   = 195609,
                BlackIcicleVisual   = 195611,
                IceNine             = 214664
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_SpellInfo == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::FrosBolt)
                    return;

                Unit* l_Target = p_EventInfo.GetDamageInfo()->GetTarget();
                Unit* l_Caster = p_EventInfo.GetDamageInfo()->GetActor();

                if (l_Target == nullptr || l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                if (l_Player == nullptr)
                    return;

                /// Calculate damage
                int32 l_HitDamage = p_EventInfo.GetDamageInfo()->GetAmount() + p_EventInfo.GetDamageInfo()->GetAbsorb();

                /// if l_HitDamage == 0 we have a miss, so we need to except this variant
                if (l_HitDamage != 0)
                {
                    uint8 l_NewIcicles = 1;
                    if (l_Caster->HasAura(eSpells::IceNine) && roll_chance_i(20))
                        l_NewIcicles += 1;

                    for (uint8 l_Itr = 0; l_Itr < l_NewIcicles; ++l_Itr)
                    {
                        double l_IcicleDamage = l_HitDamage;
                        l_IcicleDamage *= (l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * 2.25f) / 100.0f;

                        if (AuraEffect const* l_SplittingIceEffect = l_Player->GetAuraEffect(eSpells::SplittingIce, SpellEffIndex::EFFECT_2))
                            AddPct(l_IcicleDamage, l_SplittingIceEffect->GetAmount());

                        /// Prevent huge hits on player after hitting low level creatures
                        if (l_Player->getLevel() > l_Target->getLevel())
                            l_IcicleDamage = std::min(uint64(l_IcicleDamage), l_Target->GetMaxHealth());

                        /// We need to get the first free icicle slot
                        int8 l_IcicleFreeSlot = -1; ///< -1 means no free slot
                        for (int8 l_I = 0; l_I < MAX_ICICLES; ++l_I)
                        {
                            if (!l_Player->HasAura(IcicleAuras[l_I]))
                            {
                                l_IcicleFreeSlot = l_I;
                                break;
                            }
                        }

                        switch (l_IcicleFreeSlot)
                        {
                            case -1:
                            {
                                // We need to find the icicle with the smallest duration.
                                int8 l_SmallestIcicle = 0;
                                int32 l_MinDuration = 0xFFFFFF;
                                for (int8 i = 0; i < MAX_ICICLES; i++)
                                {
                                    if (Aura* l_TmpCurrentAura = l_Player->GetAura(IcicleAuras[i]))
                                    {
                                        if (l_MinDuration > l_TmpCurrentAura->GetDuration())
                                        {
                                            l_MinDuration = l_TmpCurrentAura->GetDuration();
                                            l_SmallestIcicle = i;
                                        }
                                    }
                                }

                                /// Launch the icicle with the smallest duration
                                if (AuraEffect* l_CurrentIcicleAuraEffect = l_Player->GetAuraEffect(IcicleAuras[l_SmallestIcicle], EFFECT_0))
                                {
                                    bool l_IsBlackIcicle = l_CurrentIcicleAuraEffect->GetAmplitude() == 1;

                                    int32 l_BasePoints = l_CurrentIcicleAuraEffect->GetAmount();
                                    uint32 l_DamagingSpell = eSpells::IcicleDamage;
                                    uint32 l_VisualSpell = IcicleNormalHits[l_SmallestIcicle];

                                    if (l_IsBlackIcicle)
                                    {
                                        l_BasePoints *= 2;
                                        l_DamagingSpell = eSpells::BlackIcicleDamage;
                                        l_VisualSpell = IcicleHits[l_SmallestIcicle];
                                    }

                                    l_Player->CastSpell(l_Target, l_VisualSpell, true);
                                    l_Player->CastCustomSpell(l_Target, l_DamagingSpell, &l_BasePoints, NULL, NULL, true);

                                    if (l_Caster->HasAura(eSpells::SplittingIce))
                                    {
                                        std::list<Unit*> l_TargetList;
                                        l_Target->GetAttackableUnitListInRange(l_TargetList, 10.0f);

                                        std::pair < Unit*, float> l_TargetDistance = { nullptr, 0 };
                                        for (Unit* l_TargetItr : l_TargetList)
                                        {
                                            if (!l_Caster->IsValidAttackTarget(l_TargetItr))
                                                continue;

                                            float l_Distance = l_Caster->GetDistance(l_TargetItr);
                                            if (l_TargetItr != l_Target)
                                            {
                                                if (l_TargetDistance.first == nullptr || l_Distance < l_TargetDistance.second)
                                                    l_TargetDistance = { l_TargetItr, l_Distance };
                                            }
                                        }

                                        SpellInfo const* l_SplittingIceInfo = sSpellMgr->GetSpellInfo(eSpells::SplittingIce);

                                        if (l_SplittingIceInfo)
                                            l_BasePoints = CalculatePct(l_BasePoints, l_SplittingIceInfo->Effects[EFFECT_1].BasePoints);

                                        if (l_TargetDistance.first != nullptr)
                                            l_Caster->CastCustomSpell(l_TargetDistance.first, l_DamagingSpell, &l_BasePoints, NULL, NULL, true);
                                    }

                                    l_Player->RemoveAura(IcicleAuras[l_SmallestIcicle]);
                                }

                                l_IcicleFreeSlot = l_SmallestIcicle;
                                /// No break because we'll add the icicle in the next case
                            }
                            case 0:
                            case 1:
                            case 2:
                            case 3:
                            case 4:
                            {
                                if (Aura* l_CurrentIcicleAura = l_Player->AddAura(IcicleAuras[l_IcicleFreeSlot], l_Player))
                                {
                                    if (AuraEffect* l_Effect = l_CurrentIcicleAura->GetEffect(EFFECT_0))
                                    {
                                        if (l_Caster->HasAura(eSpells::BlackIce) && roll_chance_i(20))
                                            l_Effect->SetAmplitude(1);

                                        l_Effect->SetAmount(l_IcicleDamage);
                                    }
                                }

                                break;
                            }
                        }

                        /// We need to get if we have all the Icicle Glacial Spike
                        int8 l_IcicleCount = 0;
                        for (int8 l_I = 0; l_I < MAX_ICICLES; ++l_I)
                        {
                            if (l_Player->HasAura(IcicleAuras[l_I]))
                                ++l_IcicleCount;
                        }

                        if (l_IcicleCount >= MAX_ICICLES && l_Player->HasSpell(eSpells::GlacialSpikeTalent))
                            l_Player->CastSpell(l_Player, eSpells::GlacialSpikeUsable, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mastery_icicles_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_icicles_proc_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Ice Lance - 30455
class spell_mastery_icicles_trigger : public SpellScriptLoader
{
    public:
        spell_mastery_icicles_trigger() : SpellScriptLoader("spell_mastery_icicles_trigger") { }

        class spell_mastery_icicles_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_icicles_trigger_SpellScript);

            enum eSpells
            {
                IceLanceDamage  = 228598,
                GlacialSpike    = 199786
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_MainTarget = GetExplTargetUnit();
                if (!l_Caster || !l_MainTarget)
                    return;

                if (l_MainTarget->isAlive())
                    l_Caster->m_SpellHelper.GetUint64Pair()[eSpellHelpers::IciclesTarget].first = l_MainTarget->GetGUID();

                if (!l_Caster->HasSpell(eSpells::GlacialSpike))
                    l_Caster->CastSpell(l_Caster, SPELL_MAGE_ICICLE_PERIODIC_TRIGGER, true);
            }

            void RegisterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                WorldObject* l_Target = p_Targets.front();
                if (!l_Target->ToUnit())
                    return;

                Unit* l_SplittingIceTarget = l_Target->ToUnit();
                if (l_SplittingIceTarget->isAlive())
                    l_Caster->m_SpellHelper.GetUint64Pair()[eSpellHelpers::IciclesTarget].second = l_SplittingIceTarget->GetGUID();
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_HitUnit = GetHitUnit();
                if (!l_Caster || !l_HitUnit)
                    return;

                if (l_HitUnit == GetExplTargetUnit())
                    l_Caster->CastSpell(l_HitUnit, eSpells::IceLanceDamage, true);
                else
                {
                    int32 l_Damage = 99;
                    l_Caster->CastCustomSpell(l_HitUnit, eSpells::IceLanceDamage, NULL, &l_Damage, NULL,true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mastery_icicles_trigger_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mastery_icicles_trigger_SpellScript::RegisterTarget, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
                AfterHit += SpellHitFn(spell_mastery_icicles_trigger_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mastery_icicles_trigger_SpellScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Icicles (periodic) - 148023
class spell_mastery_icicles_periodic : public SpellScriptLoader
{
    public:
        spell_mastery_icicles_periodic() : SpellScriptLoader("spell_mastery_icicles_periodic") { }

        class spell_mastery_icicles_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_icicles_periodic_AuraScript);

            enum eSpells
            {
                IcicleDamage        = 148022,
                BlackIcicleDamage   = 195609,
                BlackIcicleVisual   = 195611
            };

            uint32 m_Icicles[MAX_ICICLES];
            int32 m_IcicleCount;

            void OnApply(AuraEffect const* /*l_AurEff*/, AuraEffectHandleModes /*l_Mode*/)
            {
                m_IcicleCount = 0;
                if (Unit* l_Caster = GetCaster())
                {
                    for (uint32 l_I = 0; l_I < MAX_ICICLES; ++l_I)
                    {
                        if (Aura* l_Icicle = l_Caster->GetAura(IcicleAuras[l_I]))
                            m_Icicles[m_IcicleCount++] = IcicleAuras[l_I];
                    }
                }
            }

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (AuraEffect* l_Aura = l_Caster->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                    {
                        // Maybe not the good target selection ...
                        if (Unit* l_MainTarget = ObjectAccessor::FindUnit(l_Caster->m_SpellHelper.GetUint64Pair()[eSpellHelpers::IciclesTarget].first))
                        {
                            Unit* l_SecondTarget = ObjectAccessor::FindUnit(l_Caster->m_SpellHelper.GetUint64Pair()[eSpellHelpers::IciclesTarget].second);

                            if (l_MainTarget->isAlive())
                            {
                                int32 l_Amount = l_Aura->GetAmount();
                                if (Aura* l_CurrentIcicleAura = l_Caster->GetAura(m_Icicles[l_Amount]))
                                {
                                    AuraEffect const* l_IcicleEffect = l_CurrentIcicleAura->GetEffect(EFFECT_0);
                                    if (!l_IcicleEffect)
                                        return;

                                    bool l_IsBlackIcicle = l_IcicleEffect->GetAmplitude() == 1;

                                    int32 l_BasePoints = l_IcicleEffect->GetAmount();
                                    uint32 l_DamagingSpell = eSpells::IcicleDamage;
                                    uint32 l_VisualSpell = IcicleNormalHits[l_Amount];

                                    if (l_IsBlackIcicle)
                                    {
                                        l_BasePoints *= 2;
                                        l_DamagingSpell = eSpells::BlackIcicleDamage;
                                        l_VisualSpell = IcicleHits[l_Amount];
                                    }

                                    l_Caster->CastSpell(l_MainTarget, l_VisualSpell, true);
                                    l_Caster->CastCustomSpell(l_MainTarget, l_DamagingSpell, &l_BasePoints, NULL, NULL, true);

                                    if (l_SecondTarget && l_SecondTarget->isAlive())
                                    {
                                        l_BasePoints = 0.8f * l_BasePoints;
                                        l_Caster->CastSpell(l_SecondTarget, l_VisualSpell, true);
                                        l_Caster->CastCustomSpell(l_SecondTarget, l_DamagingSpell, &l_BasePoints, NULL, NULL, true);
                                    }

                                    l_Caster->RemoveAura(IcicleAuras[l_Amount]);
                                }

                                if (++l_Amount >= m_IcicleCount)
                                    l_Caster->RemoveAura(l_Aura->GetBase());
                                else
                                    l_Aura->SetAmount(l_Amount);
                            }
                            else
                                l_Caster->RemoveAura(l_Aura->GetBase());
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mastery_icicles_periodic_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mastery_icicles_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_icicles_periodic_AuraScript();
        }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Icicle 214130, 214127, 214126, 214125, 214124
class spell_mastery_icicle_charge : public SpellScriptLoader
{
    public:
        spell_mastery_icicle_charge() : SpellScriptLoader("spell_mastery_icicle_charge") { }

        class spell_mastery_icicle_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_icicle_charge_AuraScript);

            enum eSpells
            {
                IcicleCounter = 205473
            };

            void HandleApply(AuraEffect const* /*l_AurEff*/, AuraEffectHandleModes /*l_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                UpdateIcicleCounter(l_Owner, 1);
            }

            void HandleRemove(AuraEffect const* /*l_AurEff*/, AuraEffectHandleModes /*l_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                UpdateIcicleCounter(l_Owner, -1);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mastery_icicle_charge_AuraScript::HandleApply, EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mastery_icicle_charge_AuraScript::HandleRemove, EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }

        private:

            void UpdateIcicleCounter(Unit* p_Owner, int32 p_Change)
            {
                if (p_Change == 0)
                    return;

                if (Aura* l_AuraCounter = p_Owner->GetAura(eSpells::IcicleCounter))
                {
                    int32 l_newAmount = l_AuraCounter->GetEffect(SpellEffIndex::EFFECT_0)->GetAmount() + p_Change;
                    if (l_newAmount > 0)
                    {
                        l_AuraCounter->GetEffect(SpellEffIndex::EFFECT_0)->SetAmount(std::min(l_newAmount, (int32)MAX_ICICLES));

                        if (p_Change > 0)
                            l_AuraCounter->RefreshDuration();
                    }
                    else
                    {
                        l_AuraCounter->Remove();
                    }
                }
                else if (p_Change > 0)
                {
                    if (Aura* l_AuraCounter = p_Owner->AddAura(eSpells::IcicleCounter, p_Owner))
                    {
                        l_AuraCounter->GetEffect(SpellEffIndex::EFFECT_0)->SetAmount(std::min(p_Change, (int32)MAX_ICICLES));
                    }
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_icicle_charge_AuraScript();
        }
};

/// last update : 6.1.2 19802
// Called by 45470 - Death Strike (Heal)
// 77513 - Mastery : Blood Shield
class spell_mastery_blood_shield: public SpellScriptLoader
{
    public:
        spell_mastery_blood_shield() : SpellScriptLoader("spell_mastery_blood_shield") { }

        class spell_mastery_blood_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_blood_shield_SpellScript);

            enum eSpells
            {
                BloodPresence = 48263,
                MasteryAura = 77513
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::BloodPresence))
                    return;

                if (l_Caster->GetGUID() != l_Target->GetGUID()) ///< Only when you heal yourself
                    return;

                if (AuraEffect* l_MasteryBloodShield = l_Caster->GetAuraEffect(eSpells::MasteryAura, EFFECT_0))
                {
                    float l_Mastery = l_MasteryBloodShield->GetAmount();

                    int32 l_Bp = -int32(GetHitDamage() * (l_Mastery / 100));

                    if (AuraEffect* l_ActualBloodShield = l_Caster->GetAuraEffect(MASTERY_SPELL_BLOOD_SHIELD, EFFECT_0))
                        l_Bp += l_ActualBloodShield->GetAmount();

                    l_Bp = std::min(uint64(l_Bp), l_Caster->GetMaxHealth());

                    l_Caster->CastCustomSpell(l_Caster, MASTERY_SPELL_BLOOD_SHIELD, &l_Bp, NULL, NULL, true);
                }
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_mastery_blood_shield_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mastery_blood_shield_SpellScript();
        }
};

/// Last update 7.0.3 Build 22045
/// Called by 133 - Fireball, 108853 - Fire Blast, 2948 - Scorch, 11366 - Pyroblast, 2120 - Flamestrike, 153564 - Meteor and Pheonix Flames - 194466, 224637
/// 12846 - Mastery : Ignite
class spell_mastery_ignite: public SpellScriptLoader
{
    public:
        spell_mastery_ignite() : SpellScriptLoader("spell_mastery_ignite") { }

        class spell_mastery_ignite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_ignite_SpellScript);

            bool m_HotStreak = false;

            enum eSpell
            {
                Pyroblast           = 11366,
                Flamestrike         = 2120,
                HotStreak           = 48108,
                Everburning         = 194314
            };

            enum  eArtifactPower
            {
                EverburningTrait = 755
            };

            void HandleBeforeCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    SpellInfo const* l_SpellInfo = GetSpellInfo();
                    if (l_SpellInfo && (l_SpellInfo->Id == eSpell::Pyroblast || l_SpellInfo->Id == eSpell::Flamestrike))
                    {
                        if (l_Caster->HasAura(eSpell::HotStreak))
                            m_HotStreak = true;
                    }
                }
            }

            void HandleAfterHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        if (l_Caster->IsPlayer() && l_Caster->HasAura(MASTERY_SPELL_IGNITE) && l_Caster->getLevel() >= 80)
                        {
                            const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(MASTERY_SPELL_IGNITE_AURA);
                            if (GetSpellInfo()->Id != MASTERY_SPELL_IGNITE_AURA && l_SpellInfo != nullptr)
                            {
                                float l_Value = l_Caster->GetFloatValue(PLAYER_FIELD_MASTERY) * (m_HotStreak ? 1.5f : 0.75f);
                                m_HotStreak = false;

                                int64 l_Bp = GetHitDamage() + GetAbsorbedDamage();

                                if (l_Bp > 0)
                                {
                                    l_Bp = static_cast<int32>(CalculatePct(l_Bp, l_Value));

                                    if (l_SpellInfo->Effects[EFFECT_0].Amplitude > 0)
                                        l_Bp = l_Bp / (l_SpellInfo->GetMaxDuration() / l_SpellInfo->Effects[EFFECT_0].Amplitude);

                                    if (Player* l_Player = l_Caster->ToPlayer())
                                    {
                                        if (AuraEffect* l_Everburning = l_Player->GetAuraEffect(eSpell::Everburning, EFFECT_0))
                                            l_Bp = AddPct(l_Bp, l_Everburning->GetAmount());
                                    }

                                    if (Aura* l_PreviousIgnite = l_Target->GetAura(MASTERY_SPELL_IGNITE_AURA, l_Caster->GetGUID()))
                                    {
                                        AuraEffect* l_Effect = l_PreviousIgnite->GetEffect(EFFECT_0);
                                        if (!l_Effect)
                                            return;

                                        int64 l_PreviousRemainingDamage = (int64)l_Effect->GetAmount() * (int64)l_PreviousIgnite->GetDuration() / (uint64)l_SpellInfo->Effects[EFFECT_0].Amplitude;
                                        l_Bp += (l_PreviousRemainingDamage / ((int64)l_SpellInfo->GetMaxDuration() / (uint64)l_SpellInfo->Effects[EFFECT_0].Amplitude));
                                        l_PreviousIgnite->SetDuration(l_SpellInfo->GetMaxDuration());               ///< Ignite gets "refreshed" with new amount
                                        l_Effect->SetAmount(l_Bp);
                                    }
                                    else
                                    {
                                        int32 l_Value = l_Bp;
                                        l_Caster->CastCustomSpell(l_Target, MASTERY_SPELL_IGNITE_AURA, &l_Value, nullptr, nullptr, true);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            static void DropExpiredIgnite(Unit* l_Caster, uint64 p_TargetGUID, int32 l_Damage, uint64 l_Igniteintance)
            {
                Unit* l_Target = nullptr;

                if (!l_Caster || !(l_Target = sObjectAccessor->GetUnit(*l_Caster, p_TargetGUID)))
                    return;

                std::map<uint64, uint64>& l_IgniteintanceHelper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::IgniteInstance];
                if (l_IgniteintanceHelper.find(p_TargetGUID) == l_IgniteintanceHelper.end())
                    return;

                if (l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::IgniteInstance][p_TargetGUID] != l_Igniteintance)  ///< in case ignite has been removed from target, then reapplied while some event was still in queue, ignore this events
                    return;

                if (Aura* l_PreviousIgnite = l_Target->GetAura(MASTERY_SPELL_IGNITE_AURA, l_Caster->GetGUID()))
                {
                    if (AuraEffect* l_Effect = l_PreviousIgnite->GetEffect(EFFECT_0))
                    {
                        int32 l_Amount = l_Effect->GetAmount() - l_Damage;
                        if (l_Amount < 0)
                            l_Amount = 0;

                        l_Effect->SetAmount(l_Amount);
                    }
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_mastery_ignite_SpellScript::HandleAfterHit);
                BeforeCast += SpellCastFn(spell_mastery_ignite_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mastery_ignite_SpellScript();
        }
};

/// Last update 7.0.3 Build 22045
/// Mastery : Ignite 12654
class spell_mastery_ignite_spread : public SpellScriptLoader
{
    public:
        spell_mastery_ignite_spread() : SpellScriptLoader("spell_mastery_ignite_spread") { }

        class spell_mastery_ignite_spread_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_ignite_spread_AuraScript);

            bool m_ShouldSpread = false;

            enum eSpells
            {
                Ignite      = 12654
            };

            void HandleOnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                if (m_ShouldSpread)
                {
                    m_ShouldSpread = false;
                    Unit* l_Target = GetTarget();       ///< Player with ignite Dot ticking
                    Unit* l_Caster = GetCaster();       ///< the Mage
                    const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(MASTERY_SPELL_IGNITE_AURA);
                    if (!l_Caster || !l_Target || !l_SpellInfo)
                        return;
                    Aura* l_Ignite = l_Target->GetAura(eSpells::Ignite, l_Caster->GetGUID());
                    if (!l_Ignite)
                        return;

                    AuraEffect* l_IgniteDamage = l_Ignite->GetEffect(EFFECT_0);
                    if (!l_IgniteDamage)
                        return;

                    std::list<Unit*> l_TargetList;
                    l_Target->GetAttackableUnitListInRange(l_TargetList, 10.0f);

                    bool l_EveryoneIgnited = true;
                    for (auto l_It = l_TargetList.begin(); l_It != l_TargetList.end();)
                    {
                        bool l_ItErased = false;
                        Unit* l_Unit = *l_It;
                        if (!l_Caster->IsValidAttackTarget(l_Unit) || !l_Unit->IsHostileTo(l_Caster) || !l_Caster->IsWithinLOSInMap(l_Unit))
                        {
                            l_It = l_TargetList.erase(l_It);
                            l_ItErased = true;
                        }
                        else if (Aura* l_AlreadyIgnited = l_Unit->GetAura(eSpells::Ignite, l_Caster->GetGUID()))
                        {
                            if (l_AlreadyIgnited->GetCasterGUID() == l_Caster->GetGUID())
                            {
                                if (AuraEffect* l_AlreadyIgnitedDamage = l_AlreadyIgnited->GetEffect(EFFECT_0))
                                {
                                    if ((l_AlreadyIgnitedDamage->GetAmount() * (l_AlreadyIgnited->GetDuration() / l_AlreadyIgnitedDamage->GetAmplitude())) >= (l_IgniteDamage->GetAmount() * (l_Ignite->GetDuration() / l_IgniteDamage->GetAmplitude())))
                                    {
                                        l_It = l_TargetList.erase(l_It);
                                        l_ItErased = true;
                                    }
                                }
                            }
                        }
                        else
                            l_EveryoneIgnited = false;

                        if (!l_ItErased)
                            ++l_It;
                    }

                    if (l_TargetList.empty())
                        return;

                    if (!l_EveryoneIgnited)
                    {
                        for (auto l_Itr = l_TargetList.begin(); l_Itr != l_TargetList.end();)
                        {
                            if ((*l_Itr)->HasAura(eSpells::Ignite, l_Caster->GetGUID()))
                                l_Itr = l_TargetList.erase(l_Itr);
                            else
                                ++l_Itr;
                        }
                    }

                    if (l_TargetList.empty())
                        return;

                    Unit* l_NewTarget = JadeCore::Containers::SelectRandomContainerElement(l_TargetList);
                    int32 l_Damage = l_IgniteDamage->GetAmount();

                    if (!l_NewTarget->IsPolymorphed())
                    {
                        l_NewTarget->RemoveAura(eSpells::Ignite, l_Caster->GetGUID());                          ///< In case NewTarget has an ignite from the same caster already, remove it and set the new one
                        l_Caster->CastCustomSpell(l_NewTarget, MASTERY_SPELL_IGNITE_AURA, &l_Damage, NULL, NULL, true);
                    }
                    Aura* l_NewIgnite = l_NewTarget->GetAura(eSpells::Ignite, l_Caster->GetGUID());
                    if (!l_NewIgnite)
                        return;

                    l_NewIgnite->SetMaxDuration(l_Ignite->GetDuration());
                    l_NewIgnite->SetDuration(l_Ignite->GetDuration());
                }
                else
                    m_ShouldSpread = true;
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mastery_ignite_spread_AuraScript::HandleOnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_ignite_spread_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Mastery: Elemental Overload - 168534
/// Static Overload - 191602
/// Called By Lightning Bolt - 188196, Chain Lightning - 188443, and Lava Burst - 51505, and Lava Beam - 114074
/// Called By Elemental Blast - 117014, Icefury - 210714, Earth Shock - 8042, Frost Shock - 196840
class spell_mastery_elemental_overload : public SpellScriptLoader
{
    public:
        spell_mastery_elemental_overload() : SpellScriptLoader("spell_mastery_elemental_overload") { }

        class spell_mastery_elemental_overload_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_elemental_overload_SpellScript);

            enum eSpells
            {
                LightningBolt               = 188196,
                ChainLightning              = 188443,
                LavaBurst                   = 51505,
                ElementalBlast              = 117014,
                ElementalOverload           = 168534,
                StormTotem                  = 210652,
                Icefury                     = 210714,
                LavaBeam                    = 114074,
                StaticOverload              = 191634,
                EarthShock                  = 8042,
                FrostShock                  = 196840,

                OverloadedLavaBurst         = 77451,
                OverloadedChainLightning    = 45297,
                OverloadedLightningBolt     = 45284,
                OverloadedElementalBlast    = 120588,
                OverloadedIcefury           = 219271,
                OverloadedLavaBeam          = 114738,
                OverloadedEarthShock        = 252143,
                OverloadedFrostShock        = 256561,
                MasterOfTheElements         = 191647,

                T21Elemental4PBonus         = 251758
            };

            bool m_AlreadyTriggered = false;

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::StaticOverload) = false;

                if (l_Caster->HasAura(eSpells::StaticOverload))
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::StaticOverload) = true;
            }

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                HandleOverload();
            }

            void HandleAfterHit()
            {
                HandleOverload();
            }

            void HandleOverload()
            {
                int32 l_Damage = GetBaseHitDamage();
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_ElementalOverload = sSpellMgr->GetSpellInfo(eSpells::ElementalOverload);
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_ElementalOverload)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T21Elemental4PBonus, EFFECT_0);

                if (!l_AuraEffect && (l_SpellInfo->Id == eSpells::EarthShock || l_SpellInfo->Id == eSpells::FrostShock))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SpecIndex::SPEC_SHAMAN_ELEMENTAL)
                    return;

                float l_Chance = l_Player->GetFloatValue(EPlayerFields::PLAYER_FIELD_MASTERY) * 2.0f;

                if (l_SpellInfo->Id == eSpells::LightningBolt)
                {
                    AuraEffect* l_MasteryEffect = l_Caster->GetAuraEffect(eSpells::ElementalOverload, EFFECT_0);

                    l_Chance = (l_MasteryEffect ? l_MasteryEffect->GetAmount() : 0);
                }

                /// Storm Totem increases the chance for Lightning Bolt and Chain Lightning to trigger Elemental Overload by 10%
                if (l_SpellInfo->Id == eSpells::LightningBolt || l_SpellInfo->Id == eSpells::ChainLightning)
                {
                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::StormTotem, EFFECT_0))
                        l_Chance += l_AuraEffect->GetAmount();

                    if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::StaticOverload))
                    {
                        l_Chance = 100.0f;
                        l_Caster->RemoveAura(eSpells::StaticOverload);
                    }
                }

                ///< Chain Lightning and Lava Beam should have chance /= 5 from each target.
                if (!l_Caster->m_SpellHelper.GetBool(eSpellHelpers::StaticOverload))
                {
                    if (l_SpellInfo->Id == eSpells::ChainLightning || l_SpellInfo->Id == eSpells::LavaBeam)
                        l_Chance /= 5;
                }

                if (l_SpellInfo->Id == eSpells::Icefury && l_Player->CanApplyPvPSpellModifiers())
                {
                    l_Damage *= 0.65f; ///< Icefury damage is reduced by 35% in PvP
                    SetHitDamage(l_Damage);
                }

                if (l_AuraEffect && (l_SpellInfo->Id == eSpells::EarthShock || l_SpellInfo->Id == eSpells::FrostShock))
                {
                    l_Chance = l_AuraEffect->GetAmount();
                    if (GetSpell() && GetSpell()->IsCritForTarget(l_Target)) /// Earth Shock overload and Frost Shock overload from the T21 Elemental 4P cannot crit, but will base their damage on the initial spell (including crit) 7.3.5 - Build 26365
                        l_Damage *= (l_Player->CanApplyPvPSpellModifiers() ? 1.85f : 2.7f); /// taking into account 'Elemental Fury' 60188 (+70% crit damage in pve, +35% crit damage in pvp)
                }

                if (!roll_chance_f(l_Chance))
                    return;

                uint32 l_SpellId = eSpells::OverloadedChainLightning;
                int32 l_ElementalOverloadPct = l_ElementalOverload->Effects[EFFECT_1].BasePoints * (l_Player->CanApplyPvPSpellModifiers() ? 0.75f : 1.0f); ///< Elemental Overload is reduced by 25% in PvP

                switch (l_SpellInfo->Id)
                {
                    case eSpells::LavaBurst:
                    {
                        l_SpellId = eSpells::OverloadedLavaBurst;
                        break;
                    }
                    case eSpells::LightningBolt:
                    {
                        l_SpellId = eSpells::OverloadedLightningBolt;
                        break;
                    }
                    case eSpells::ElementalBlast:
                    {
                        l_SpellId = eSpells::OverloadedElementalBlast;
                        break;
                    }
                    case eSpells::Icefury:
                    {
                        l_SpellId = eSpells::OverloadedIcefury;
                        break;
                    }
                    case eSpells::LavaBeam:
                    {
                        l_SpellId = eSpells::OverloadedLavaBeam;
                        break;
                    }
                    case eSpells::EarthShock:
                    {
                        l_SpellId = eSpells::OverloadedEarthShock;
                        break;
                    }
                    case eSpells::FrostShock:
                    {
                        l_SpellId = eSpells::OverloadedFrostShock;
                        break;
                    }
                    default:
                        break;
                }

                if (AuraEffect* l_MasterOfTheElements = l_Caster->GetAuraEffect(eSpells::MasterOfTheElements, EFFECT_0))
                    l_ElementalOverloadPct += l_MasterOfTheElements->GetAmount();

                l_Damage = CalculatePct(l_Damage, l_ElementalOverloadPct);

                bool l_NeedsDelay = false;
                if (l_SpellInfo->Id == eSpells::LightningBolt || l_SpellInfo->Id == eSpells::ChainLightning)
                    l_NeedsDelay = true;

                /// Add ~500 ms delay for Lightning Bolt and Chain Lightning Overload, for visual
                l_Player->CastCustomSpell(l_Target, l_SpellId, &l_Damage, nullptr, nullptr, true, nullptr, nullptr, 0, l_NeedsDelay ? 500 : 0);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::LavaBurst:
                    case eSpells::Icefury:
                    case eSpells::LavaBeam:
                    case eSpells::ChainLightning:
                    case eSpells::EarthShock:
                    case eSpells::FrostShock:
                        OnEffectHitTarget += SpellEffectFn(spell_mastery_elemental_overload_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                        break;
                    case eSpells::LightningBolt:
                        OnCast += SpellCastFn(spell_mastery_elemental_overload_SpellScript::HandleOnCast);
                        /// No break here
                    default:
                        AfterHit += SpellHitFn(spell_mastery_elemental_overload_SpellScript::HandleAfterHit);
                        break;
                }

                if (m_scriptSpellId == eSpells::ChainLightning)
                    OnCast += SpellCastFn(spell_mastery_elemental_overload_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mastery_elemental_overload_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Mastery: Chaotic Energies - 77220
/// Called by Conflagrate - 17962 - Immolate - 348 - Incinerate - 29722 - Rain of Fire - 42223 - Immolate Dot - 157736
/// Called by Shadowburn - 17877 - Cataclysm - 152108 - Chaos Bolt - 116858
class spell_mastery_chaotic_energies : public SpellScriptLoader
{
    public:
        spell_mastery_chaotic_energies() : SpellScriptLoader("spell_mastery_chaotic_energies") { }

        class spell_mastery_chaotic_energies_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_chaotic_energies_SpellScript);

            enum eSpells
            {
                ChaoticEnergies     = 77220,
                RainOfFire          = 42223,
                Incinerate          = 29722,
                ChaosBolt           = 116858,
                Conflagrate         = 17962,
                Immolate            = 348,
                Shadowburn          = 17877,
                Cataclysm           = 152108,
                ChaosBarrage        = 187394,
                ShadowBolt          = 196657,
                DemonicPower        = 196100,
                T20Destro4P         = 242296
            };

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                if (!l_Player || !l_Player->HasAura(eSpells::ChaoticEnergies))
                    return;

                int32 l_Damage = (int32)GetHitDamage();
                float l_Mastery = l_Player->GetFloatValue(PLAYER_FIELD_MASTERY);

                float l_Additional = frand(0, l_Mastery);

                if (GetSpellInfo()->Id == eSpells::ChaosBolt && l_Caster->HasAura(eSpells::T20Destro4P))
                    l_Additional = l_Mastery;

                l_Mastery = l_Mastery + l_Additional;

                if (GetSpell()->m_ShoudLogDamage && GetSpell()->m_CurrentLogDamageStream)
                    *GetSpell()->m_CurrentLogDamageStream << "spell_mastery_chaotic_energies: mastery " << l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) << " final mastery boost " << l_Mastery << std::endl;

                l_Damage += CalculatePct(l_Damage, l_Mastery);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::Incinerate:
                        OnEffectHitTarget += SpellEffectFn(spell_mastery_chaotic_energies_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                        break;
                    case eSpells::RainOfFire:
                    case eSpells::ChaosBolt:
                    case eSpells::Conflagrate:
                    case eSpells::Immolate:
                    case eSpells::Shadowburn:
                    case eSpells::Cataclysm:
                    case eSpells::ChaosBarrage:
                    case eSpells::ShadowBolt:
                    case eSpells::DemonicPower:
                        OnEffectHitTarget += SpellEffectFn(spell_mastery_chaotic_energies_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mastery_chaotic_energies_SpellScript();
        }

        class spell_mastery_chaotic_energies_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_chaotic_energies_AuraScript);

            enum eSpells
            {
                ChaoticEnergies     = 77220,
                ImmolateDot         = 157736
            };

            int32 m_Amount = 0;

            void HandleSaveAmount(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                m_Amount = p_AuraEffect->GetAmount();
            }

            void HandleDamages(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->HasAura(eSpells::ChaoticEnergies))
                    return;

                int32 l_Damage = m_Amount;
                float l_Mastery = l_Player->GetFloatValue(PLAYER_FIELD_MASTERY);
                l_Mastery = l_Mastery + frand(0, l_Mastery);

                l_Damage += CalculatePct(l_Damage, l_Mastery);
                const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(l_Damage);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::ImmolateDot:
                        AfterEffectApply += AuraEffectApplyFn(spell_mastery_chaotic_energies_AuraScript::HandleSaveAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mastery_chaotic_energies_AuraScript::HandleDamages, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_chaotic_energies_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Mastery: Chaotic Energies - 77220
class spell_mastery_chaotic_energies_absorb : public SpellScriptLoader
{
    public:
        spell_mastery_chaotic_energies_absorb() : SpellScriptLoader("spell_mastery_chaotic_energies_absorb") { }

        class  spell_mastery_chaotic_energies_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_chaotic_energies_absorb_AuraScript);

            void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo || !l_Caster)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                float l_MaxPct = l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * l_SpellInfo->Effects[EFFECT_1].BonusMultiplier / 3;
                p_AbsorbAmount = uint32(CalculatePct(float(p_DamageInfo.GetAmount()), l_MaxPct + frand(0.0f, l_MaxPct)));
                p_AuraEffect->SetAmount(p_AbsorbAmount + 1); ///< int32 max value so it never fades off
                return;
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mastery_chaotic_energies_absorb_AuraScript::HandleOnAbsorb, EFFECT_2 ,SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_chaotic_energies_absorb_AuraScript();
        }
};

/// last update : 7.3.5 build 26365
/// Mastery: Gust of Mists - 117907
/// Look spell_proc_event table
class spell_mastery_gust_of_mists : public SpellScriptLoader
{
    public:
        spell_mastery_gust_of_mists() : SpellScriptLoader("spell_mastery_gust_of_mists") { }

        class spell_mastery_gust_of_mists_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_gust_of_mists_AuraScript);

            enum eSpells
            {
                GustofMists         = 191894,
                RenewingMistDummy   = 115151,
                Vivify              = 116670,
                EssenceFontBuff     = 191840,
                T21Mistweaver2P     = 251825,
                TranquilMist        = 253448
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcInfo)
            {
                PreventDefaultAction();

                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id == eSpells::RenewingMistDummy)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Caster|| !l_Target)
                    return;

                /// Vivify can triggers Gust of Mists on main target only
                if (l_SpellInfo->Id == eSpells::Vivify)
                {
                    if (l_DamageInfo->GetSpell() && l_DamageInfo->GetSpell()->m_targets.GetUnitTarget() != l_Target)
                        return;
                }

                l_Caster->CastSpell(l_Target, eSpells::GustofMists, true);

                /// Need to script T21 2P Here, because Renewing Mist's Gust of Mist can't proc it (because of some retarded stuff, don't ask pls)
                if (l_Caster->HasAura(eSpells::T21Mistweaver2P))
                    l_Caster->CastSpell(l_Target, eSpells::TranquilMist, true);

                if (l_Target->HasAura(eSpells::EssenceFontBuff, l_Caster->GetGUID()))
                    l_Caster->CastSpell(l_Target, eSpells::GustofMists, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mastery_gust_of_mists_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_gust_of_mists_AuraScript();
        }
};

/// last update : 7.3.5 build 26365
/// Mastery: Gust of Mists (heal) - 191894
class spell_mastery_gust_of_mists_heal : public SpellScriptLoader
{
    public:
        spell_mastery_gust_of_mists_heal() : SpellScriptLoader("spell_mastery_gust_of_mists_heal") { }

        class spell_mastery_gust_of_mists_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_gust_of_mists_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(117907, EFFECT_0);
                if (l_AuraEffect == nullptr)
                    return;

                int32 l_Heal = (l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL) * ((float)l_AuraEffect->GetAmount() / 100.0f));
                l_Heal = l_Caster->SpellHealingBonusDone(l_Target, GetSpellInfo(), l_Heal, EFFECT_0, HEAL);
                l_Heal = l_Target->SpellHealingBonusTaken(l_Caster, GetSpellInfo(), l_Heal, HEAL);

                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mastery_gust_of_mists_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mastery_gust_of_mists_heal_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Path of Flame - 210621
class spell_mastery_path_of_flame : public SpellScriptLoader
{
public:
    spell_mastery_path_of_flame() : SpellScriptLoader("spell_mastery_path_of_flame") { }

    class spell_mastery_path_of_flame_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mastery_path_of_flame_SpellScript);

        enum eSpells
        {
            FlameShock = 188389,
            PathOfFlame = 210621
        };

        void HandleOnEffect(SpellEffIndex p_index)
        {
            if (m_alreadyHit)
                return;
            m_alreadyHit = true;
            Unit* l_Caster = GetCaster();
            Unit* l_LavaBurstTarget = GetExplTargetUnit();
            if (!l_Caster || !l_LavaBurstTarget)
                return;

            SpellInfo const* l_pathOfFlame = sSpellMgr->GetSpellInfo(eSpells::PathOfFlame);
            if (!l_pathOfFlame)
                return;
            uint32 l_Radius = l_pathOfFlame->Effects[EFFECT_1].RadiusEntry->RadiusMax;

            std::list<Unit*> l_UnitList;
            JadeCore::AnyUnitInObjectRangeCheck l_Check(l_LavaBurstTarget, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyUnitInObjectRangeCheck> l_Searcher(l_LavaBurstTarget, l_UnitList, l_Check);
            l_LavaBurstTarget->VisitNearbyObject(l_Radius, l_Searcher);

            std::list<Unit*> l_EnemiesList;
            for (auto l_unit = l_UnitList.begin(); l_unit != l_UnitList.end(); ++l_unit)
                if (l_Caster->IsValidAttackTarget(*l_unit))
                    l_EnemiesList.push_back(*l_unit);

            l_EnemiesList.remove(l_LavaBurstTarget);
            if (!l_EnemiesList.size())
                return;
            Unit* l_Target = JadeCore::Containers::SelectRandomContainerElement(l_EnemiesList);

            l_Caster->CastSpell(l_Target, eSpells::FlameShock, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_mastery_path_of_flame_SpellScript::HandleOnEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
        }

        bool m_alreadyHit;

    public:
        spell_mastery_path_of_flame_SpellScript() : m_alreadyHit(false) {}
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mastery_path_of_flame_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Mastery: Nature's Guardian - 155783
class spell_mastery_natures_guardian : public SpellScriptLoader
{
    public:
        spell_mastery_natures_guardian() : SpellScriptLoader("spell_mastery_natures_guardian") { }

        class spell_mastery_natures_guardian_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_natures_guardian_AuraScript);

            enum eSpells
            {
                SpellNaturesGuardianProc            = 227034,
                SpellFrenziedRegeneration           = 22842,
                SpellFrenziedRegenerationInstant    = 122307,
                SpellFuryOfNatureHeal               = 248522
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfo)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                Player* l_Caster = GetCaster()->ToPlayer();
                if (!l_Caster)
                    return;

                if (p_ProcInfo.GetActor() == p_ProcInfo.GetActionTarget() && l_Caster->HasSpellCooldown(eSpells::SpellNaturesGuardianProc))
                    return;

                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_DamageInfo->GetAmount())
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                /// Don't proc from self, neither from Frenzied Regeneration
                switch (l_SpellInfo->Id)
                {
                    case eSpells::SpellNaturesGuardianProc:
                    case eSpells::SpellFrenziedRegeneration:
                    case eSpells::SpellFrenziedRegenerationInstant:
                    case eSpells::SpellFuryOfNatureHeal: ///< Handled manually
                        return;
                    default:
                        break;
                }

                if (!l_SpellInfo->IsPositive())
                    return;

                int32 l_Heal = CalculatePct(l_DamageInfo->GetAmount(), p_AurEff->GetAmount());

                l_Caster->CastCustomSpell(l_Caster, eSpells::SpellNaturesGuardianProc, &l_Heal, nullptr, nullptr, true);

                /// Add 50ms cooldown to prevent double procs from same self heal (healer == target)
                if (p_ProcInfo.GetActor() == p_ProcInfo.GetActionTarget())
                    l_Caster->AddSpellCooldown(eSpells::SpellNaturesGuardianProc, 0, 50);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_mastery_natures_guardian_AuraScript::OnProc, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mastery_natures_guardian_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_mastery_spell_scripts()
{
    new spell_mastery_blood_shield();

    ////// Legion Reworked //////
    /// Mage
    new spell_mastery_icicles_proc();
    new spell_mastery_icicles_trigger();
    new spell_mastery_icicles_periodic();
    new spell_mastery_icicle_charge();
    new spell_mastery_ignite();
    new spell_mastery_ignite_spread();

    /// Shaman
    new spell_mastery_elemental_overload();
    new spell_mastery_path_of_flame();

    /// Priest
    new spell_mastery_echo_of_light();

    /// Warlock
    new spell_mastery_chaotic_energies();
    new spell_mastery_chaotic_energies_absorb();

    /// Monk
    new spell_mastery_gust_of_mists();
    new spell_mastery_gust_of_mists_heal();

    /// Druid
    new spell_mastery_natures_guardian();
}
#endif
