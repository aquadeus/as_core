////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_ROGUE and SPELLFAMILY_GENERIC spells used by rogue players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_rog_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "Object.h"
#include "HelperDefines.h"
#include "CellImpl.h"

/// Shadow Reflection (aura proc) - 152151
class spell_rog_shadow_reflection_proc : public SpellScriptLoader
{
    public:
        spell_rog_shadow_reflection_proc() : SpellScriptLoader("spell_rog_shadow_reflection_proc") { }

        class spell_rog_shadow_reflection_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shadow_reflection_proc_AuraScript);

            enum eDatas
            {
                NpcShadowReflection = 77726,
                AddSpellToQueue     = 0,
                FinishFirstPhase    = 1
            };

            int32 m_OldDataTimeSpell = 0;

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() == nullptr)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = p_EventInfo.GetActionTarget())
                    {
                        for (Unit::ControlList::const_iterator l_Iter = l_Caster->m_Controlled.begin(); l_Iter != l_Caster->m_Controlled.end(); ++l_Iter)
                        {
                            if ((*l_Iter)->GetEntry() != eDatas::NpcShadowReflection)
                                continue;

                            if (Creature* l_Creature = (*l_Iter)->ToCreature())
                            {
                                if (!l_Creature->IsAIEnabled)
                                    break;

                                uint32 l_Data[3];
                                l_Data[0] = p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id;
                                l_Data[1] = p_AurEff->GetBase()->GetMaxDuration() - p_AurEff->GetBase()->GetDuration() - m_OldDataTimeSpell;
                                l_Data[2] = l_Caster->GetPower(Powers::POWER_COMBO_POINT);
                                m_OldDataTimeSpell = p_AurEff->GetBase()->GetMaxDuration() - p_AurEff->GetBase()->GetDuration();
                                l_Creature->AI()->AddHitQueue(l_Data, eDatas::AddSpellToQueue);
                                break;
                            }
                        }
                    }
                }
            }

            void OnUpdate(uint32 p_Diff)
            {
                if ((GetDuration() - p_Diff) <= 8000)
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        for (Unit::ControlList::const_iterator l_Iter = l_Caster->m_Controlled.begin(); l_Iter != l_Caster->m_Controlled.end(); ++l_Iter)
                        {
                            if ((*l_Iter)->GetEntry() != eDatas::NpcShadowReflection)
                                continue;

                            if (Creature* l_Creature = (*l_Iter)->ToCreature())
                            {
                                if (!l_Creature->IsAIEnabled)
                                    break;

                                l_Creature->AI()->AddHitQueue(nullptr, eDatas::FinishFirstPhase);
                                break;
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_shadow_reflection_proc_AuraScript::OnProc, EFFECT_1, SPELL_AURA_DUMMY);
                OnAuraUpdate += AuraUpdateFn(spell_rog_shadow_reflection_proc_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shadow_reflection_proc_AuraScript();
        }
};

/// Shadow Reflection - 156744
class spell_rog_shadow_reflection : public SpellScriptLoader
{
    public:
        spell_rog_shadow_reflection() : SpellScriptLoader("spell_rog_shadow_reflection") { }

        class spell_rog_shadow_reflection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shadow_reflection_SpellScript);

            enum eSpells
            {
                ShadowReflectionClone = 168691
            };

            void HandleOnHit(SpellEffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                        l_Target->CastSpell(l_Caster, eSpells::ShadowReflectionClone, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_shadow_reflection_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shadow_reflection_SpellScript();
        }
};

/// Last Update 6.2.3
/// Killing Spree - 51690
class spell_rog_killing_spree: public SpellScriptLoader
{
    public:
        spell_rog_killing_spree() : SpellScriptLoader("spell_rog_killing_spree") { }

        static SpellCastResult CheckCanAttack(Unit* p_Caster)
        {
            if (!p_Caster)
                return SpellCastResult::SPELL_FAILED_ERROR;

            if (p_Caster->isInStun())
                return SpellCastResult::SPELL_FAILED_STUNNED;

            if (p_Caster->isInRoots())
                return SpellCastResult::SPELL_FAILED_ROOTED;

            if (p_Caster->isConfused())
                return SpellCastResult::SPELL_FAILED_CONFUSED;

            if (p_Caster->IsPolymorphed())
                return SpellCastResult::SPELL_FAILED_NOT_IN_CONTROL;

            return SpellCastResult::SPELL_CAST_OK;
        }

        class spell_rog_killing_spree_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_killing_spree_AuraScript);

            enum eSpells
            {
                KillingSpreeDeselect = 61851,
                KillingSpreeTeleport = 57840,
                KillingSpreeDamages = 57841,
                BladeFlurryAura = 13877
            };

            uint64 m_TargetGUID = 0;
            float m_X = 0.0f;
            float m_Y = 0.0f;
            float m_Z = 0.0f;
            float m_Orientation = 0.0f;

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->CastSpell(l_Caster, eSpells::KillingSpreeDeselect, true);

                    Unit* l_Target = Unit::GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::KillingSpree));
                    if (!l_Target)
                        return;

                    m_TargetGUID = l_Target->GetGUID();
                    m_X = l_Caster->GetPositionX();
                    m_Y = l_Caster->GetPositionY();
                    m_Z = l_Caster->GetPositionZ();
                    m_Orientation = l_Caster->GetOrientation();
                }
            }

            void OnTick(AuraEffect const*)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// From 6.2.0 stun can interrupt Killing Spree
                    if (CheckCanAttack(l_Caster) != SPELL_CAST_OK)
                    {
                        SetDuration(0);
                        return;
                    }

                    if (!l_Caster->HasAura(eSpells::BladeFlurryAura))
                    {
                        Unit* l_Target = ObjectAccessor::FindUnit(m_TargetGUID);

                        if (l_Target != nullptr)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::KillingSpreeTeleport, true);
                            l_Caster->CastSpell(l_Target, eSpells::KillingSpreeDamages, true);
                        }
                    }
                    else
                    {
                        std::list<Unit*> l_TargetList;
                        float l_Radius = 10.0f;

                        JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                        JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                        l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                        l_TargetList.remove_if([this, l_Caster](Unit* p_Unit) -> bool
                        {
                            if (p_Unit == nullptr || p_Unit->HasBreakableByDamageCrowdControlAura() || !l_Caster->IsValidAttackTarget(p_Unit))
                                return true;

                            return false;
                        });

                        if (l_TargetList.empty())
                            return;

                        Unit* l_Target = JadeCore::Containers::SelectRandomContainerElement(l_TargetList);
                        if (!l_Target)
                            return;

                        l_Caster->CastSpell(l_Target, eSpells::KillingSpreeTeleport, true);
                        l_Caster->CastSpell(l_Target, eSpells::KillingSpreeDamages, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_killing_spree_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_killing_spree_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        class spell_rog_killing_spree_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_killing_spree_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                return CheckCanAttack(GetCaster());
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                uint64& l_TargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::KillingSpree);
                l_TargetGUID = l_Target->GetGUID();
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_killing_spree_SpellScript::HandleCheckCast);
                OnCast += SpellCastFn(spell_rog_killing_spree_SpellScript::HandleOnCast);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_killing_spree_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_killing_spree_SpellScript();
        }
};

/// Killing Spree (teleport) - 57840
class spell_rog_killing_spree_teleport : public SpellScriptLoader
{
    public:
        spell_rog_killing_spree_teleport() : SpellScriptLoader("spell_rog_killing_spree_teleport") { }

        class spell_rog_killing_spree_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_killing_spree_teleport_SpellScript);

            enum eCreature
            {
                BossKromog = 77692
            };

            void HandleTeleport(SpellEffIndex p_EffIndex)
            {
                if (GetExplTargetUnit() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                /// Players can't be teleported in Kromog's back
                if (Creature* l_Target = GetExplTargetUnit()->ToCreature())
                {
                    if (l_Target->GetEntry() == eCreature::BossKromog)
                    {
                        PreventHitEffect(p_EffIndex);

                        Position l_Dest;
                        l_Target->GetContactPoint(l_Caster, l_Dest.m_positionX, l_Dest.m_positionY, l_Dest.m_positionZ);

                        if (!l_Caster->IsWithinLOS(l_Dest.GetPositionX(), l_Dest.GetPositionY(), l_Dest.GetPositionZ()))
                        {
                            float l_Angle = l_Target->GetRelativeAngle(l_Caster);
                            float l_Dist = l_Caster->GetDistance(l_Dest);
                            l_Target->GetFirstCollisionPosition(l_Dest, l_Dist, l_Angle);
                        }

                        /// Calculated as an EffectCharge
                        l_Caster->NearTeleportTo(l_Dest, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_killing_spree_teleport_SpellScript::HandleTeleport, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_killing_spree_teleport_SpellScript();
        }
};

/// Called by Marked for Death (check caster) - 140149
/// Marked for Death - 137619
class spell_rog_marked_for_death: public SpellScriptLoader
{
    public:
        spell_rog_marked_for_death() : SpellScriptLoader("spell_rog_marked_for_death") { }

        class spell_rog_marked_for_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_marked_for_death_AuraScript);

            enum eSpells
            {
                MarkedForDeath = 137619
            };

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                if (!GetTargetApplication())
                    return;

                if (!GetTargetApplication()->GetBase())
                    return;

                if (!GetTarget())
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AURA_REMOVE_BY_DEATH)
                {
                    if (l_Player->HasSpellCooldown(eSpells::MarkedForDeath))
                        l_Player->RemoveSpellCooldown(eSpells::MarkedForDeath, true);

                    GetTarget()->RemoveAura(eSpells::MarkedForDeath);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_marked_for_death_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_marked_for_death_AuraScript();
        }
};

/// Called by Ambush - 8676, Garrote - 703 and Cheap Shot - 1833
class spell_rog_cloak_and_dagger: public SpellScriptLoader
{
    public:
        spell_rog_cloak_and_dagger() : SpellScriptLoader("spell_rog_cloak_and_dagger") { }

        class spell_rog_cloak_and_dagger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_cloak_and_dagger_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                float l_BasicRadius = 5.0f;

                if (l_Target == nullptr)
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Caster->HasUnitState(UNIT_STATE_ROOT) && l_Target->GetDistance(l_Caster) > l_BasicRadius)
                    return SPELL_FAILED_ROOTED;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_cloak_and_dagger_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_cloak_and_dagger_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Cheat Death - 31230
class spell_rog_cheat_death: public SpellScriptLoader
{
    public:
        spell_rog_cheat_death() : SpellScriptLoader("spell_rog_cheat_death") { }

        class spell_rog_cheat_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_cheat_death_AuraScript);

            enum eSpells
            {
                CheatDeathReduceDamage  = 45182,
                CheatDeathMarker        = 45181
            };

            void Absorb(AuraEffect* p_AuraEffect, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (!GetTarget())
                    return;

                if (Player* l_Target = GetTarget()->ToPlayer())
                {
                    if (p_DmgInfo.GetAmount() > l_Target->GetHealth() && !l_Target->HasAura(eSpells::CheatDeathMarker))
                    {
                        l_Target->CastSpell(l_Target, eSpells::CheatDeathReduceDamage, true);
                        l_Target->CastSpell(l_Target, eSpells::CheatDeathMarker, true);
                        p_AbsorbAmount = p_DmgInfo.GetAmount() + 1;
                        l_Target->SetHealth(l_Target->CountPctFromMaxHealth(7));
                    }
                    else if (l_Target->HasAura(eSpells::CheatDeathReduceDamage))
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CheatDeathReduceDamage))
                        {
                            p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), -l_SpellInfo->Effects[EFFECT_0].BasePoints);
                            p_AuraEffect->ChangeAmount(p_AbsorbAmount + 1); ///< int32 max value so it never fades off
                        }
                    }
                    else
                        p_AbsorbAmount = 0;
                }
            }

            void AfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                const_cast<AuraEffect*>(p_AuraEffect)->ChangeAmount(1);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_cheat_death_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_rog_cheat_death_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_cheat_death_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Blade Flurry - 13877
class spell_rog_blade_flurry: public SpellScriptLoader
{
    public:
        spell_rog_blade_flurry() : SpellScriptLoader("spell_rog_blade_flurry") { }

        class spell_rog_blade_flurry_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_blade_flurry_AuraScript);

            enum eSpells
            {
                BladeFlurryDamage = 22482,
                ShivarranSymmetry = 226318
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (!GetTarget())
                    return;

                if (p_EventInfo.GetActor()->GetGUID() != GetTarget()->GetGUID())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                    SpellInfo const* l_SpellInfo = GetSpellInfo();
                    if (p_EventInfo.GetDamageInfo()->GetDamageType() == DOT || (l_ProcSpell && !l_ProcSpell->CanTriggerBladeFlurry()))
                        return;

                    int32 l_DamagePCT = l_SpellInfo->Effects[EFFECT_2].BasePoints;
                    if (l_Caster->HasAura(eSpells::ShivarranSymmetry))
                    {
                        if (SpellInfo const* l_ShivarranSymmetry = sSpellMgr->GetSpellInfo(eSpells::ShivarranSymmetry))
                            l_DamagePCT += l_ShivarranSymmetry->Effects[EFFECT_0].BasePoints;
                    }

                    int32 l_Damage = CalculatePct(p_EventInfo.GetDamageInfo()->GetUnmitigatedAmount(), l_DamagePCT);

                    if (Unit* l_MainTarget = p_EventInfo.GetDamageInfo()->GetTarget())
                    {
                        l_Caster->CastCustomSpell(l_MainTarget, eSpells::BladeFlurryDamage, &l_Damage, NULL, NULL, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_blade_flurry_AuraScript::OnProc, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_blade_flurry_AuraScript();
        }

        class spell_rog_blade_flurry_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_blade_flurry_SpellScript);

            enum eSpells
            {
                ShivarranSymmetryAura = 226045,
                ShivarranSymmetryProc = 226318
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::ShivarranSymmetryAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ShivarranSymmetryProc, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_blade_flurry_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_blade_flurry_SpellScript();
        }
};

/// Last Update 6.1.2
/// Blade Flurry (damage) - 22482
class spell_rog_blade_flurry_damage : public SpellScriptLoader
{
    public:
        spell_rog_blade_flurry_damage() : SpellScriptLoader("spell_rog_blade_flurry_damage") { }

        class spell_rog_blade_flurry_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_blade_flurry_damage_SpellScript);

            uint64 m_MainTargetGUID = 0;

            void HandleOnCast()
            {
                Unit* l_MainTarget = GetExplTargetUnit();

                if (l_MainTarget == nullptr)
                    return;

                m_MainTargetGUID = l_MainTarget->GetGUID();
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Target->GetGUID() == m_MainTargetGUID)
                    PreventHitDamage();
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_rog_blade_flurry_damage_SpellScript::HandleOnCast);
                OnEffectHitTarget += SpellEffectFn(spell_rog_blade_flurry_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_blade_flurry_damage_SpellScript();
        }
};

/// Cloak of Shadows - 31224
class spell_rog_cloak_of_shadows: public SpellScriptLoader
{
    public:
        spell_rog_cloak_of_shadows() : SpellScriptLoader("spell_rog_cloak_of_shadows") { }

        class spell_rog_cloak_of_shadows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_cloak_of_shadows_SpellScript);

            enum eSpells
            {
                GhostlyShell        = 202533,
                GhostlyShellBuff    = 202534,
                GhostlyShellHeal    = 202536
            };

            enum eArtifactPower
            {
                GhostlyShellArtifact = 1066
            };

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    int l_Count = 0;

                    uint32 l_DispelMask = SpellInfo::GetDispelMask(DISPEL_ALL);
                    Unit::AuraApplicationMap& l_Auras = l_Caster->GetAppliedAuras();
                    for (Unit::AuraApplicationMap::iterator l_Iter = l_Auras.begin(); l_Iter != l_Auras.end();)
                    {
                        AuraApplication* l_Application = l_Iter->second;
                        Aura* l_Aura = l_Application->GetBase();
                        SpellInfo const* l_SpellInfo = l_Aura->GetSpellInfo();

                        if (l_SpellInfo->Id == 197963 || l_SpellInfo->Id == 197996)    ///< Runic Brand
                        {
                            ++l_Iter;
                            continue;
                        }

                        // remove all harmful spells on you...
                        if ((l_SpellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC // only affect magic spells
                            || (l_SpellInfo->GetDispelMask() & l_DispelMask) || (l_SpellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_MAGIC))
                            // ignore positive and passive auras
                            && !l_Application->IsPositive() && !l_Aura->IsPassive() && GetSpellInfo()->CanDispelAura(l_SpellInfo))
                            //&& l_Aura->GetCasterGUID() != l_Caster->GetGUID())
                        {
                            l_Caster->RemoveAura(l_Iter);
                            ++l_Count;
                        }
                        else
                            ++l_Iter;
                    }

                    if (l_Caster->IsPlayer() && l_Caster->HasAura(eSpells::GhostlyShell))
                    {
                        Player* l_Player = l_Caster->ToPlayer();
                        int8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPower::GhostlyShellArtifact);
                        SpellInfo const* l_GhostlyShellHealInfo = sSpellMgr->GetSpellInfo(eSpells::GhostlyShellHeal);

                        if (!l_GhostlyShellHealInfo)
                            return;

                        int32 l_HealPct = l_Rank * l_GhostlyShellHealInfo->Effects[EFFECT_0].BasePoints * l_Count;

                        l_Player->CastCustomSpell(l_Caster, eSpells::GhostlyShellHeal, &l_HealPct, nullptr, nullptr, true);
                        l_Player->CastSpell(l_Player, eSpells::GhostlyShellBuff, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_cloak_of_shadows_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_cloak_of_shadows_SpellScript();
        }
};

/// Combat Readiness - 74001
class spell_rog_combat_readiness: public SpellScriptLoader
{
    public:
        spell_rog_combat_readiness() : SpellScriptLoader("spell_rog_combat_readiness") { }

        class spell_rog_combat_readiness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_combat_readiness_AuraScript);

            enum eSpells
            {
                CombatReadiness = 74001,
                CombatInsight   = 74002
            };

            uint32 m_Update;
            bool m_Hit;

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                {
                    m_Update = 10000;
                    m_Hit = false;
                }
            }

            void OnUpdate(uint32 p_Diff)
            {
                m_Update -= p_Diff;

                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::CombatInsight))
                        m_Hit = true;

                    if (m_Update <= 0)
                        if (!m_Hit)
                            l_Caster->RemoveAura(eSpells::CombatReadiness);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_rog_combat_readiness_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnAuraUpdate += AuraUpdateFn(spell_rog_combat_readiness_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_combat_readiness_AuraScript();
        }
};

/// Called by Kidney Shot - 408 and Cheap Shot - 1833
/// Nerve Strike - 108210
class spell_rog_nerve_strike: public SpellScriptLoader
{
    public:
        spell_rog_nerve_strike() : SpellScriptLoader("spell_rog_nerve_strike") { }

        enum eSpells
        {
            NerveStrikeAura             = 108210,
            NerveStrikeReduceDamageDone = 112947
        };

        class spell_rog_combat_readiness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_combat_readiness_AuraScript);

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                    if (Unit* l_Target = GetTarget())
                        if (l_Caster->HasAura(eSpells::NerveStrikeAura))
                            l_Caster->CastSpell(l_Target, eSpells::NerveStrikeReduceDamageDone, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_combat_readiness_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_combat_readiness_AuraScript();
        }
};

/// Last Update 7.0.3 - 22522
/// Called by Envenom - 32645
class spell_rog_envenom: public SpellScriptLoader
{
    public:
        spell_rog_envenom() : SpellScriptLoader("spell_rog_envenom") { }

        class spell_rog_envenom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_envenom_SpellScript);

            enum eSpells
            {
                SliceAndDice        = 5171,
                Tier5Bonus2P        = 37169,
                T17Assassination4P  = 166886,
                EnvenomComboPoint   = 167106,
                T19Assassination4P  = 211673,
                MutilatedFlesh      = 211672,
                Rupture             = 1943,
                CruelGarrote        = 230011
            };

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Player* l_Owner = l_Caster->GetSpellModOwner();

                if (l_Target == nullptr || l_Owner == nullptr)
                    return;

                int32 l_ComboPoint = l_Caster->GetPower(Powers::POWER_COMBO_POINT);
                int32 l_Damage = GetHitDamage();

                if (l_ComboPoint)
                {
                    /// Tier 5 Bonus 2 pieces
                    if (AuraEffect* l_Tier5Bonus2P = l_Caster->GetAuraEffect(eSpells::Tier5Bonus2P, EFFECT_0))
                        l_Damage += l_ComboPoint * l_Tier5Bonus2P->GetAmount();
                }

                /// Item - Rogue T19 Assassination 4P Bonus
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Assassination4P, EFFECT_0))
                {
                    std::list<AuraEffect*> l_BleedAuraEffects = l_Target->GetAuraEffectsByMechanic(1LL << MECHANIC_BLEED);
                    for (AuraEffect* l_AuraEffect2 : l_BleedAuraEffects)
                    {
                        if (l_AuraEffect2->GetAuraType() != SPELL_AURA_PERIODIC_DAMAGE && l_AuraEffect2->GetAuraType() != SPELL_AURA_PERIODIC_DUMMY && l_AuraEffect2->GetAuraType() != SPELL_AURA_PERIODIC_DAMAGE_PERCENT)
                            continue;

                        /// T19 2P bonus doesn't work with T19 4P bonus, neither does Bloodstained Handkerchief
                        if (l_AuraEffect2->GetId() == eSpells::MutilatedFlesh || l_AuraEffect2->GetId() == eSpells::CruelGarrote)
                            continue;

                        /// Rupture has 2 periodic effects with MECHANIC_BLEED
                        if (l_AuraEffect2->GetId() == eSpells::Rupture && l_AuraEffect2->GetAuraType() == SPELL_AURA_PERIODIC_DUMMY)
                            continue;

                        Unit* l_AuraCaster = l_AuraEffect2->GetCaster();
                        if (!l_AuraCaster)
                            continue;

                        if (l_Caster->GetGUID() == l_AuraCaster->GetGUID())
                            AddPct(l_Damage, l_AuraEffect->GetAmount());
                    }
                }

                SetHitDamage(l_Damage);

                /// Envenom refunds 1 Combo Point.
                if (l_Caster->HasAura(eSpells::T17Assassination4P))
                    l_Caster->CastSpell(l_Caster, eSpells::EnvenomComboPoint, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_envenom_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_envenom_SpellScript();
        }
};

/// Called by Envenom - 32645 and Run Through - 2098
/// Cut to the Chase - 51667
class spell_rog_cut_to_the_chase: public SpellScriptLoader
{
    public:
        spell_rog_cut_to_the_chase() : SpellScriptLoader("spell_rog_cut_to_the_chase") { }

        class spell_rog_cut_to_the_chase_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_cut_to_the_chase_SpellScript);

            enum eSpells
            {
                SliceAndDice        = 5171,
                CutToTheChaseAura   = 51667
            };

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::CutToTheChaseAura))
                    {
                        if (Aura* l_SliceAndDice = l_Caster->GetAura(eSpells::SliceAndDice, l_Caster->GetGUID()))
                        {
                            l_SliceAndDice->SetDuration(36 * IN_MILLISECONDS);
                            l_SliceAndDice->SetMaxDuration(36 * IN_MILLISECONDS);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_cut_to_the_chase_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_cut_to_the_chase_SpellScript();
        }
};

/// Shadowstep - 36554
class spell_rog_shadowstep: public SpellScriptLoader
{
    public:
        spell_rog_shadowstep() : SpellScriptLoader("spell_rog_shadowstep") { }

        class spell_rog_shadowstep_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shadowstep_SpellScript);

            enum eSpells
            {
                ShadowSwiftness     = 192422,
                Evasion             = 226364
            };

            SpellCastResult CheckCast()
            {
                if (GetCaster()->HasUnitState(UNIT_STATE_ROOT))
                    return SPELL_FAILED_ROOTED;
                if (GetExplTargetUnit() && GetExplTargetUnit() == GetCaster())
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ShadowSwiftness, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                if (Aura* l_Aura = l_Caster->AddAura(eSpells::Evasion, l_Caster))
                    l_Aura->SetDuration(l_AuraEffect->GetAmount() * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_shadowstep_SpellScript::CheckCast);
                AfterCast += SpellCastFn(spell_rog_shadowstep_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shadowstep_SpellScript();
        }
};

class EventRemoveShadowstrike : public BasicEvent
{
    public:
        explicit EventRemoveShadowstrike(uint64 p_PlayerGuid, uint32 p_SpellId) : m_PlayerGuid(p_PlayerGuid), m_SpellId(p_SpellId) { }

        enum eSpells
        {
            Stealth                 = 1784,
            StealthSubterfuge       = 115191,
            StealthSubterfugeEffect = 115192,
            ShadowDance             = 185422
        };

        bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
        {
            Player* l_Player = sObjectAccessor->FindPlayer(m_PlayerGuid);
            if (!l_Player)
                return true;

            constexpr uint32 l_AuraIds[] = { eSpells::Stealth, eSpells::StealthSubterfuge, eSpells::StealthSubterfugeEffect, eSpells::ShadowDance };

            for (auto l_AuraId : l_AuraIds)
                if (l_Player->HasAura(l_AuraId))
                    return true;

            l_Player->RemoveAura(m_SpellId);
            return true;
        }

    private:
        uint64 m_PlayerGuid;
        uint32 m_SpellId;
};

/// Last Update Legion: 7.1.5 build 23420
/// Stealth - 1784, Subterfuge - 115191 and Subterfuge effect - 115192 - Shadow Dance - 185422
class spell_rog_stealth: public SpellScriptLoader
{
    public:
        spell_rog_stealth() : SpellScriptLoader("spell_rog_stealth") { }

        enum eSpells
        {
            Stealth                     = 1784,
            Shadowstrike                = 245623,
            StealthSubterfuge           = 115191,
            StealthSubterfugeEffect     = 115192,
            StealthTriggered1           = 158188,
            StealthTriggered2           = 158185,
            NightStalkerAura            = 14062,
            NightStalkerDamageDone      = 130493,
            ShadowFocusAura             = 108209,
            ShadowFocusCostPct          = 112942,
            ShadowDance                 = 185422,
            MasterAssassinsInitiative   = 235022,
            MasterAssassinsCritical     = 235027,
            FromTheShadows              = 192434,
            LifeSteal                   = 146347,
            TouchOfGrave                = 127802,
            Vanish                      = 131361,
        };

        class spell_rog_stealth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_stealth_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                if (DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo())
                {
                    SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                    if (l_SpellInfo && l_SpellInfo->Id == eSpells::FromTheShadows)
                    {
                        Unit* l_Caster = GetCaster();
                        Unit* l_Actor = l_DamageInfo->GetActor();
                        if (l_Caster == l_Actor)
                            return false;
                    }
                }

                return true;
            }

            void HandlePrepareProc(ProcEventInfo & p_EventInfo)
            {
                Unit* l_Owner = GetUnitOwner();

                // Fix - Vanish :  If rogue has vanish aura stealth is not removed on periodic damage
                if (l_Owner != nullptr && l_Owner->HasAura(eSpells::Vanish))
                {
                    PreventDefaultAction();
                    return;
                }

                if (p_EventInfo.GetDamageInfo())
                {
                    SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                    if (l_ProcSpell == nullptr)
                        return;

                    /// Fix for Subterfuge with Drain Life, Touch of the Grave and Hashshashin's trait "From the Shadows" DoT (added 7.1.5 - 23420)
                    if (l_ProcSpell->Id == FromTheShadows ||
                        l_ProcSpell->Id == LifeSteal ||
                        l_ProcSpell->Id == TouchOfGrave)
                        PreventDefaultAction();
                }
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (GetSpellInfo()->Id != eSpells::ShadowDance)
                        l_Caster->CastSpell(l_Caster, eSpells::Shadowstrike, true);

                    if ((GetSpellInfo()->Id == eSpells::Stealth || GetSpellInfo()->Id == eSpells::StealthSubterfuge) && (l_Caster->HasAura(eSpells::StealthSubterfugeEffect) || l_Caster->HasAura(eSpells::ShadowDance)))
                    {
                        l_Caster->RemoveAura(eSpells::StealthSubterfugeEffect);
                        l_Caster->RemoveAura(eSpells::ShadowDance);
                    }

                    if (GetSpellInfo()->Id != eSpells::StealthSubterfugeEffect && GetSpellInfo()->Id != eSpells::ShadowDance)
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::StealthTriggered1, true);
                        l_Caster->CastSpell(l_Caster, eSpells::StealthTriggered2, true);

                        if (l_Caster->HasAura(eSpells::NightStalkerAura))
                            l_Caster->CastSpell(l_Caster, eSpells::NightStalkerDamageDone, true);

                        if (l_Caster->HasAura(eSpells::ShadowFocusAura))
                            l_Caster->CastSpell(l_Caster, eSpells::ShadowFocusCostPct, true);

                        if (l_Caster->HasAura(eSpells::MasterAssassinsInitiative))
                            l_Caster->CastSpell(l_Caster, eSpells::MasterAssassinsCritical, true);
                    }
                    else if (GetSpellInfo()->Id == eSpells::ShadowDance)
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::StealthTriggered1, true);

                        if (l_Caster->HasAura(eSpells::ShadowFocusAura))
                            l_Caster->CastSpell(l_Caster, eSpells::ShadowFocusCostPct, true);

                        if (l_Caster->HasAura(eSpells::NightStalkerAura))
                            l_Caster->CastSpell(l_Caster, eSpells::NightStalkerDamageDone, true);
                    }
                    else
                    {
                        if (l_Caster->HasAura(eSpells::StealthSubterfuge))
                            l_Caster->RemoveAura(eSpells::StealthSubterfuge);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->m_Events.AddEvent(new EventRemoveShadowstrike(l_Caster->GetGUID(), eSpells::Shadowstrike), l_Caster->m_Events.CalculateTime(100));
                    bool hasInitiative = l_Caster->HasAura(eSpells::MasterAssassinsInitiative);
                    if (hasInitiative && GetSpellInfo()->Id != eSpells::StealthSubterfugeEffect)
                    {
                        SpellInfo const * l_Spellinfo = sSpellMgr->GetSpellInfo(eSpells::MasterAssassinsInitiative);
                        if (!l_Spellinfo)
                            return;

                        if (Aura* l_Aura = l_Caster->GetAura(eSpells::MasterAssassinsCritical))
                        {
                            l_Aura->SetAuraTimer(l_Spellinfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                            l_Aura->SetDuration(l_Spellinfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                        }
                    }
                    else if (!hasInitiative)
                    {
                        l_Caster->RemoveAura(eSpells::MasterAssassinsCritical);
                    }

                    AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                    if (GetSpellInfo()->Id != eSpells::StealthSubterfuge || l_RemoveMode == AURA_REMOVE_BY_CANCEL)
                    {
                        l_Caster->RemoveAura(eSpells::StealthTriggered1);
                        if (Aura* l_Nightstalker = l_Caster->GetAura(eSpells::NightStalkerDamageDone))
                            l_Nightstalker->SetDuration(200);   ///< We can't remove it now

                        l_Caster->RemoveAura(eSpells::ShadowFocusCostPct);
                    }

                    if (GetSpellInfo()->Id == eSpells::StealthSubterfuge)
                    {
                        if (!l_Caster->HasAura(eSpells::StealthSubterfugeEffect))
                            l_Caster->CastSpell(l_Caster, eSpells::StealthSubterfugeEffect, true);
                    }

                    l_Caster->RemoveAurasDueToSpell(eSpells::StealthTriggered2);
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_stealth_AuraScript::HandleCheckProc);
                DoPrepareProc += AuraProcFn(spell_rog_stealth_AuraScript::HandlePrepareProc);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_stealth_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_stealth_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_stealth_AuraScript();
    }
};

/// Last Update 7.3.5 - Build 26365
/// Vanish - 1856
/// Called by Vanish triggered spell - 131361
class spell_rog_vanish : public SpellScriptLoader
{
    public:
        spell_rog_vanish() : SpellScriptLoader("spell_rog_vanish") { }

        enum eSpells
        {
            WodPvpCombat4pBonus                 = 182303,
            WodPvpCombat4pBonusTrigger          = 182304,
            WodPvpAssassination4pBonus          = 170883,
            WodPvpAssassination4pBonusTrigger   = 170882,
            Stealth                             = 1784,
            StealthShapeshift                   = 158188,
            StealthModifier                     = 158185,
            Subterfuge                          = 108208,
            StealthSubterfuge                   = 115191,
            StealthSubterfugeEffect             = 115192,
            MasterAssassinsInitiative           = 235022,
            MasterAssassinsCritical             = 235027,
            Vanish                              = 1856,
            VanishAura                          = 131361,
            Shadowstrike                        = 245623,
            ShadowSightDebuff                   = 34709,
            Vendetta                            = 79140
        };

        class spell_rog_vanish_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_vanish_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*canBeRecalculated*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::StealthSubterfugeEffect))
                        p_Amount = 1;
                }
            }

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    l_Player->RemoveMovementImpairingAuras();
                    l_Player->RemoveAurasByType(SPELL_AURA_MOD_STALKED, 0, nullptr, eSpells::Vendetta); ///< Except Vendetta
                    l_Player->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                    l_Player->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_ROOT, true);

                    l_Player->CastSpell(l_Player, eSpells::StealthShapeshift, true);

                    if (l_Player->HasAura(eSpells::MasterAssassinsInitiative))
                        l_Player->CastSpell(l_Player, eSpells::MasterAssassinsCritical, true);

                    l_Player->CastSpell(l_Player, eSpells::Shadowstrike, true);

                    /// Item - Rogue WoD PvP Assassination 4P Bonus and Item - Rogue WoD PvP Combat 4P Bonus
                    if (l_Player->getLevel() == 100)
                    {
                        /// Assasination
                        if (l_Player->HasAura(eSpells::WodPvpAssassination4pBonus))
                            l_Player->CastSpell(l_Player, eSpells::WodPvpAssassination4pBonusTrigger, true);
                        /// Combat
                        else if (l_Player->HasAura(eSpells::WodPvpCombat4pBonus))
                            l_Player->CastSpell(l_Player, eSpells::WodPvpCombat4pBonusTrigger, true);
                    }
                }
            }

            void OnRemove(AuraEffect const* p_aurEff, AuraEffectHandleModes /*mode*/)
            {
                uint32 l_RemainingDuration = 0;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    Aura* l_Aura = p_aurEff->GetBase();
                    if (!l_Aura)
                        return;
                    bool l_CheckSpell = false;                                              /// Checking if the Rogue is currently casting a spell

                    Spell const* l_CurrentSpellCast = l_Player->GetCurrentSpell(CurrentSpellTypes::CURRENT_MELEE_SPELL);
                    if (l_CurrentSpellCast)
                        l_CheckSpell = true;

                    if (!l_CheckSpell)
                        l_CurrentSpellCast = l_Player->GetCurrentSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                    if (l_CurrentSpellCast)
                        l_CheckSpell = true;

                    if (!l_CheckSpell)
                        l_CurrentSpellCast = l_Player->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
                    if (l_CurrentSpellCast)
                        l_CheckSpell = true;

                    if (!l_CheckSpell)
                        l_CurrentSpellCast = l_Player->GetCurrentSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL);
                    if (l_CurrentSpellCast)
                        l_CheckSpell = true;

                    AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                    if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE || l_Player->getVictim() || l_CheckSpell)       /// Need to check if the remove was due to buff expiring or casting spell / auto (intended) or by taking damage (not intended)
                    {
                        l_Player->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SNARE, false);
                        l_Player->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_ROOT, false);
                        bool hasInitiative = l_Player->HasAura(eSpells::MasterAssassinsInitiative);
                        if (hasInitiative && GetSpellInfo()->Id != eSpells::StealthSubterfugeEffect)
                        {
                            SpellInfo const * l_Spellinfo = sSpellMgr->GetSpellInfo(eSpells::MasterAssassinsInitiative);
                            if (!l_Spellinfo)
                                return;

                            if (Aura* l_Aura = l_Player->GetAura(eSpells::MasterAssassinsCritical))
                            {
                                l_Aura->SetAuraTimer(l_Spellinfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                                l_Aura->SetDuration(l_Spellinfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                            }
                        }
                        else if (!hasInitiative)
                        {
                            l_Player->RemoveAura(eSpells::MasterAssassinsCritical);
                        }

                        /// Stealth should be applied just after Vanish buff remove
                        int32 l_CurrentStealthId = l_Player->HasAura(eSpells::Subterfuge) ? eSpells::StealthSubterfuge : eSpells::Stealth;

                        l_Player->RemoveSpellCooldown(l_CurrentStealthId, true);

                        l_Player->m_SpellHelper.GetBool(eSpellHelpers::MasterOfShadowsVanish) = true;
                        l_Player->CastSpell(l_Player, eSpells::Stealth, true);
                    }
                    else if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_DEFAULT) ///< Vanish was removed but shouldnt have (taking damage for example)
                    {
                        l_RemainingDuration = l_Aura->GetDuration();

                        if (!l_Player)
                            return;

                        l_Player->AddDelayedEvent([l_Player, l_RemainingDuration]() -> void /// Casting Vanish again as soon as it gets removed, with the duration altered
                        {
                            l_Player->CastSpell(l_Player, eSpells::Vanish, true);
                            Aura* l_Aura = l_Player->GetAura(eSpells::VanishAura);
                            if (!l_Aura)
                                return;

                            l_Aura->SetDuration(l_RemainingDuration);
                        }, 1);

                        if (!l_Player->HasAura(eSpells::StealthModifier))       ///< This happen when vanish was cancelled (right click on aura bar)
                            l_Player->RemoveAura(eSpells::StealthShapeshift);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_vanish_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_DUMMY);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_vanish_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_vanish_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_vanish_AuraScript();
        }

        class spell_rog_vanish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_vanish_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::ShadowSightDebuff))
                        return SpellCastResult::SPELL_FAILED_CASTER_AURASTATE;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_vanish_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_vanish_SpellScript();
        }
};

/// Burst of Speed - 108212
class spell_rog_burst_of_speed: public SpellScriptLoader
{
public:
    spell_rog_burst_of_speed() : SpellScriptLoader("spell_rog_burst_of_speed") { }

    class spell_rog_burst_of_speed_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rog_burst_of_speed_SpellScript);

        enum eSpells
        {
            BurstOfSpeed = 137573
        };

        void HandleOnHit()
        {
            if (Unit* l_Caster = GetCaster())
                l_Caster->CastSpell(l_Caster, eSpells::BurstOfSpeed, true);
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_rog_burst_of_speed_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rog_burst_of_speed_SpellScript();
    }
};

/// Last Upadate Legion 7.1.5
/// Fan of Knives - 51723
class spell_rog_fan_of_knives: public SpellScriptLoader
{
    public:
        spell_rog_fan_of_knives() : SpellScriptLoader("spell_rog_fan_of_knives") { }

        class spell_rog_fan_of_knives_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_fan_of_knives_SpellScript);

            enum eSpells
            {
                SealFateAura                    = 14190,
                SealFate                        = 14189,
                FlyingDaggers                   = 198128,
                DreadlordsDeceitAssassination   = 208693,
                PoisonKnives                    = 192376,
                PoisonKnivesDamages             = 192380,
                DeadlyPoison                    = 2818,
                AgonizingPoison                 = 200803
            };

            enum eArtifactTraits
            {
                PoisonKnifesTraits              = 331
            };

            bool m_HasAlredyBenefitOfBonus = false;

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Spell const* l_Spell = GetSpell();
                Unit* l_Target = GetHitUnit();
                if (l_Caster == nullptr || l_Spell == nullptr || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::FlyingDaggers))
                {
                    if (l_Spell->GetUnitTargetCount() >= 3)
                        SetHitDamage((GetHitDamage() * 150) / 60);
                }

                if (l_Spell->IsCritForTarget(l_Target) && l_Caster->HasAura(eSpells::SealFateAura))
                    l_Caster->CastSpell(l_Caster, eSpells::SealFate, true);

                if (!l_Caster->HasAura(eSpells::PoisonKnives))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::PoisonKnives);
                Aura* l_Aura = l_Target->GetAura(eSpells::DeadlyPoison);
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Aura)
                    l_Aura = l_Target->GetAura(eSpells::AgonizingPoison);

                if (!l_Aura || !l_SpellInfo || !l_Player)
                    return;

                if (l_Aura->GetId() == eSpells::DeadlyPoison)
                {
                    AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                    if (!l_AuraEffect)
                        return;

                    int32 l_Bp = CalculatePct(l_AuraEffect->GetAmount() * (l_Aura->GetDuration() / l_AuraEffect->GetAmplitude() + 1), l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eArtifactTraits::PoisonKnifesTraits));
                    l_Caster->CastCustomSpell(l_Target, eSpells::PoisonKnivesDamages, &l_Bp, nullptr, nullptr, true);
                }
                else
                {
                    int32 l_Bp = l_SpellInfo->Effects[EFFECT_0].BasePoints / 2 * l_Player->GetRankOfArtifactPowerId(eArtifactTraits::PoisonKnifesTraits);
                    int32 l_Damage = GetHitDamage();

                    SetHitDamage(AddPct(l_Damage, l_Bp));
                }
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();

                if (m_HasAlredyBenefitOfBonus)
                    return;

                l_Caster->AddComboPoints(GetSpellInfo()->Effects[EFFECT_1].BasePoints);
                m_HasAlredyBenefitOfBonus = true;
                Aura* l_Aura = l_Caster->GetAura(eSpells::DreadlordsDeceitAssassination);
                if (!l_Aura)
                    return;
                l_Aura->Remove();
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_fan_of_knives_SpellScript::HandleOnHit);
                AfterHit += SpellHitFn(spell_rog_fan_of_knives_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_fan_of_knives_SpellScript();
        }
};

/// Combo Point Delayed - 139569
class spell_rog_combo_point_delayed: public SpellScriptLoader
{
    public:
        spell_rog_combo_point_delayed() : SpellScriptLoader("spell_rog_combo_point_delayed") { }

        class spell_rog_combo_point_delayed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_combo_point_delayed_SpellScript);

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                uint64 l_PlayerGUID = l_Player->GetGUID();
                uint32 l_SpellID = GetSpellInfo()->Id;
                uint32 l_Value = GetSpellInfo()->Effects[EFFECT_0].BasePoints;

                GetCaster()->ToPlayer()->AddCriticalOperation([l_PlayerGUID, l_SpellID, l_Value]() -> void
                {
                    Player * l_LambdaPlayer = HashMapHolder<Player>::Find(l_PlayerGUID);

                    if (l_LambdaPlayer && l_LambdaPlayer->IsInWorld())
                        l_LambdaPlayer->EnergizeBySpell(l_LambdaPlayer, l_SpellID, l_Value, POWER_COMBO_POINT);
                });
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_combo_point_delayed_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_combo_point_delayed_SpellScript();
        }
};

/// Deadly Throw - 26679
class spell_rog_deadly_throw : public SpellScriptLoader
{
    public:
        spell_rog_deadly_throw() : SpellScriptLoader("spell_rog_deadly_throw") { }

        class spell_rog_deadly_throw_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_deadly_throw_SpellScript);

            enum eSpells
            {
                DeadlyThrowInterrupt = 137576
            };

            void HandleDamage(SpellEffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    uint8 l_ComboPoint = l_Caster->GetPower(Powers::POWER_COMBO_POINT);
                    int32 l_Damage = 0;

                    if (l_ComboPoint)
                    {
                        float l_Ap = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                        l_Damage += int32((l_Ap * 0.178f) * 1 * l_ComboPoint);
                    }

                    if (l_ComboPoint >= 5)
                    {
                        if (Unit* l_Target = GetHitUnit())
                            l_Caster->CastSpell(l_Target, eSpells::DeadlyThrowInterrupt, true);
                    }

                    SetHitDamage(l_Damage);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_deadly_throw_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_deadly_throw_SpellScript();
        }
};

class PlayerScript_ruthlessness : public PlayerScript
{
    public:
        PlayerScript_ruthlessness() : PlayerScript("PlayerScript_ruthlessness") { }

        enum eSpells
        {
            Ruthlessness        = 14161,
            T17Combat4P         = 165478,
            Deceit              = 166878,
            ShadowStrikesAura   = 166881,
            ShadowStrikesProc   = 170107,
            ComboPointDelayed   = 139569,
            DeeperStratagem     = 193531
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_After)
                return;

            if (p_Regen || p_Power != POWER_COMBO_POINT || p_Player->getClass() != CLASS_ROGUE)
                return;

            /// Get the power earn (if > 0 ) or consum (if < 0)
            int32 l_DiffVal = p_NewValue - p_OldValue;

            if (l_DiffVal < 0)
            {
                if (p_Player->HasAura(eSpells::Ruthlessness))
                {
                    SpellInfo const * l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Ruthlessness);
                    if (!l_SpellInfo)
                        return;

                    l_DiffVal *= -1;

                    if (AuraEffect* l_AuraEffect = p_Player->GetAuraEffect(eSpells::DeeperStratagem, EFFECT_2))
                    {
                        if (l_DiffVal >= l_AuraEffect->GetAmount())
                        {
                            if (roll_chance_i(l_SpellInfo->Effects[0].PointsPerComboPoint))
                                p_Player->CastSpell(p_Player, eSpells::ComboPointDelayed, true);
                        }
                    }

                    if (roll_chance_i(l_SpellInfo->Effects[0].PointsPerComboPoint * l_DiffVal))
                        p_Player->CastSpell(p_Player, eSpells::ComboPointDelayed, true);
                }

                if (p_Player->HasAura(eSpells::T17Combat4P))
                {
                    /// Your finishing moves have a 4% chance per combo point to generate 5 combo points and cause your next Eviscerate to consume no Energy.
                    if (roll_chance_i(4 * -l_DiffVal))
                        p_Player->CastSpell(p_Player, eSpells::Deceit, true);
                }

                /// When Shadow Dance expires, your next finishing move refunds 5 combo points.
                if (p_Player->HasAura(eSpells::ShadowStrikesAura))
                {
                    p_Player->RemoveAura(eSpells::ShadowStrikesAura);
                    p_Player->CastSpell(p_Player, eSpells::ShadowStrikesProc, true);
                }
            }
        }
};

/// Last Update 6.2.3
class PlayerScript_stealth : public PlayerScript
{
    public:
        PlayerScript_stealth() : PlayerScript("PlayerScript_stealth") { }

        enum eSpells
        {
            Stealth                 = 1784,
            StealthSubterfuge       = 115191,
            StealthShapeshift       = 158188,
            StealthSubterfugeEffect = 115192,
            Subterfuge              = 108208,
            NightStalker            = 14062,
            StealthStalker          = 1784
        };

        void OnEnterBG(Player* p_Player, uint32 p_MapID) override
        {
            if (p_Player->getClass() != CLASS_ROGUE)
                return;

            /// We should remove the resting stealth aura
            p_Player->RemoveAura(eSpells::Stealth);
            p_Player->RemoveAura(eSpells::StealthSubterfuge);
            p_Player->RemoveAura(eSpells::StealthShapeshift);
            p_Player->RemoveAura(eSpells::StealthSubterfugeEffect);
        }

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID) override
        {
            switch (p_SpellID)
            {
            case eSpells::Subterfuge:
                p_Player->RemoveAura(eSpells::StealthStalker);
                p_Player->RemoveAura(eSpells::StealthSubterfuge);
                break;
            case eSpells::NightStalker:
                p_Player->RemoveAura(eSpells::StealthSubterfuge);
                p_Player->RemoveAura(eSpells::StealthStalker);
                break;
            default:
                break;
            }
        }
};

/// Last Update: 7.1.5 Build 23420
/// Feint - 1966
class spell_rog_feint : public SpellScriptLoader
{
    public:
        spell_rog_feint() : SpellScriptLoader("spell_rog_feint") { }

        class spell_rog_feint_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_feint_SpellScript);

            enum eSpells
            {
                WodPvPSubtlety4p        = 170877,
                WodPvPSubtlety4pEffect  = 170879,
                WillofValeeraHeal       = 208403,
                WillofValeera           = 208402
            };

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->GetActiveSpecializationID() == SPEC_ROGUE_SUBTLETY && l_Player->HasAura(eSpells::WodPvPSubtlety4p))
                    {
                        l_Player->CastSpell(l_Player, eSpells::WodPvPSubtlety4pEffect, true);
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::WillofValeera))
                    return;
                l_Caster->CastSpell(l_Caster, eSpells::WillofValeeraHeal);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_feint_SpellScript::HandleOnHit);
                AfterCast += SpellCastFn(spell_rog_feint_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_feint_SpellScript();
        }

        class spell_rog_feint_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_feint_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.60f;   ///< Feint effect is reduced by 40% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_feint_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_feint_AuraScript();
        }
};

/// Increased 40% if a dagger is equipped
/// Call by Ambush - 8676, Hemorrhage - 16511
class spell_rog_dagger_bonus : public SpellScriptLoader
{
    public:
        spell_rog_dagger_bonus() : SpellScriptLoader("spell_rog_dagger_bonus") { }

        class spell_rog_dagger_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_dagger_bonus_SpellScript);

            void HandleDamage(SpellEffIndex)
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                Item* l_MainItem    = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                Item* l_OffItem     = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

                if (((l_MainItem && l_MainItem->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER) || (l_OffItem && l_OffItem->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)))
                {
                    int32 l_Damage = GetHitDamage();
                    SetHitDamage(l_Damage + CalculatePct(l_Damage, 40));
                }

            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_dagger_bonus_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_dagger_bonus_SpellScript();
        }
};

/// Item - Rogue T17 Assassination 2P Bonus - 165516
class spell_rog_item_t17_assassination_2p_bonus : public SpellScriptLoader
{
    public:
        spell_rog_item_t17_assassination_2p_bonus() : SpellScriptLoader("spell_rog_item_t17_assassination_2p_bonus") { }

        class spell_rog_item_t17_assassination_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_item_t17_assassination_2p_bonus_AuraScript);

            enum eSpells
            {
                MutilateRightHand   = 5374,
                MutilateLeftHand    = 27576,
                MutilateEnergizer   = 166885
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_ProcSpell)
                    return;

                uint32 const l_AllowedSpells[2] = {eSpells::MutilateRightHand, eSpells::MutilateLeftHand };

                bool l_Found = false;
                for (uint8 l_I = 0; l_I < 2; ++l_I)
                {
                    if (l_ProcSpell->Id == l_AllowedSpells[l_I])
                    {
                        l_Found = true;
                        break;
                    }
                }

                if (!l_Found)
                    return;

                if (!(p_EventInfo.GetHitMask() & ProcFlagsExLegacy::PROC_EX_CRITICAL_HIT))
                    return;

                /// Mutilate and Dispatch critical strikes restore 7 energy.
                l_Caster->CastSpell(l_Caster, eSpells::MutilateEnergizer, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_item_t17_assassination_2p_bonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_item_t17_assassination_2p_bonus_AuraScript();
        }
};

/// Called by Envenom 32645
/// Ruthlessness - 14161
class spell_rog_ruthlessness_and_relentless_strikes : public SpellScriptLoader
{
    public:
        spell_rog_ruthlessness_and_relentless_strikes() : SpellScriptLoader("spell_rog_ruthlessness_and_relentless_strikes") { }

        class spell_rog_ruthlessness_and_relentless_strikes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_ruthlessness_and_relentless_strikes_SpellScript);

            enum eSpells
            {
                Ruthlessness        = 14161,
                ComboPointDelayed   = 139569
            };

            uint8 m_ComboPoints = 0;

            void HandleBeforeHit()
            {
                if (Unit* l_Caster = GetCaster())
                    m_ComboPoints = l_Caster->GetPower(Powers::POWER_COMBO_POINT);
            }

            void HandleAfterHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    /// 20% chance for every spent combo point
                    /// 1 cp - 20%, 2 cp - 40%, 3 cp - 60%, 4 cp - 80%, 5 cp - 100%
                    if (l_Player->HasAura(eSpells::Ruthlessness))
                        if (SpellInfo const* l_AuraSpellinfo = sSpellMgr->GetSpellInfo(eSpells::Ruthlessness))
                            if (roll_chance_i(l_AuraSpellinfo->Effects[0].PointsPerComboPoint * m_ComboPoints))
                                l_Player->CastSpell(l_Player, eSpells::ComboPointDelayed, true);
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_rog_ruthlessness_and_relentless_strikes_SpellScript::HandleBeforeHit);
                AfterHit += SpellHitFn(spell_rog_ruthlessness_and_relentless_strikes_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_ruthlessness_and_relentless_strikes_SpellScript();
        }
};

/// Last Update 7.1.0 - Build 22578
/// Ruthlessness - 14161
class spell_rog_ruthlessness : public SpellScriptLoader
{
    public:
        spell_rog_ruthlessness() : SpellScriptLoader("spell_rog_ruthlessness")
        {}

        enum eSpells
        {
            SpellRunThrough     = 2098,
            SpellKidneyShot     = 408,
            SpellDeadlyThrow    = 26679,
            SpellRupture        = 1943
        };

        class spell_rog_ruthlessness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_ruthlessness_AuraScript);

            bool HandleCheckProc(ProcEventInfo & p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr)
                    return false;

                SpellInfo const* l_ProcSpell = p_Proc.GetDamageInfo()->GetSpellInfo();

                if (l_ProcSpell == nullptr)
                    return false;

                switch (l_ProcSpell->Id)
                {
                    case eSpells::SpellRunThrough:
                    case eSpells::SpellKidneyShot:
                    case eSpells::SpellRupture:
                    case eSpells::SpellDeadlyThrow:
                        return true;

                    default:
                        return false;
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_ruthlessness_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_ruthlessness_AuraScript();
        }
};

/// Last Update 6.2.3
/// Mastery: Main Gauche - 76806
class spell_rog_main_gauche: public SpellScriptLoader
{
    public:

        spell_rog_main_gauche() : SpellScriptLoader("spell_rog_main_gauche") { }

        class spell_rog_main_gauche_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_main_gauche_AuraScript);

            enum eSpells
            {
                MainGauche      = 86392,
                AutoAttack      = 6603,
                SaberSlash      = 193315,
                SaberSlashExtra = 197834,
                KillingSpree    = 57841,
                KillingSpreeOff = 57842,
                DeathFromAbove  = 152150,
                T19Outlaw2PBonus= 211667
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                Unit* l_Target = GetTarget();

                if (l_DamageInfo == nullptr || l_Target == nullptr)
                    return;

                Unit* l_Victim = p_EventInfo.GetDamageInfo()->GetTarget();

                if (l_Victim == nullptr)
                    return;

                /// Poison is considerate like a Mainhand attack, and Main Gauche should not proc from poison
                if (p_EventInfo.GetDamageInfo()->GetSpellInfo() && (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Dispel == DISPEL_POISON))
                    return;

                if (!(p_EventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK))
                    return;

                std::vector<uint32> l_Spells =
                {
                    eSpells::AutoAttack,
                    eSpells::SaberSlash,
                    eSpells::SaberSlashExtra,
                    eSpells::KillingSpree,
                    eSpells::KillingSpreeOff,
                    eSpells::DeathFromAbove
                };

                if (SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo())
                {
                    if (std::find(l_Spells.begin(), l_Spells.end(), l_SpellInfo->Id) == l_Spells.end())
                        return;

                    if (l_SpellInfo->Id == eSpells::DeathFromAbove)
                    {
                        if (Spell const* l_Spell = p_EventInfo.GetDamageInfo()->GetSpell())
                            if (l_Spell->m_targets.GetUnitTarget() != p_EventInfo.GetProcTarget())
                                return;
                    }
                }

                int32 l_ProcChance = p_AurEff->GetAmount();
                /// Item - Rogue T19 Outlaw 2P Bonus
                if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::T19Outlaw2PBonus, EFFECT_0))
                    l_ProcChance += l_AuraEffect->GetAmount();

                if (roll_chance_i(l_ProcChance))
                    l_Target->CastSpell(l_Victim, eSpells::MainGauche, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_main_gauche_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_main_gauche_AuraScript();
        }
};

//////////////////////////////////////////////
////////////////              ////////////////
////////////////    LEGION    ////////////////
////////////////              ////////////////
//////////////////////////////////////////////

/// Called by Internal Bleeding 154953
class spell_rog_internal_bleeding : public SpellScriptLoader
{
    public:
        spell_rog_internal_bleeding() : SpellScriptLoader("spell_rog_internal_bleeding") { }

        class spell_rog_internal_bleeding_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_internal_bleeding_AuraScript);

            uint32 m_ComboPointsUsed = 0;

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !m_ComboPointsUsed)
                    return;

                p_Amount = m_ComboPointsUsed * ((l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 1.44f) / p_AurEff->GetTotalTicks());
            }

            void SetGuid(uint32 p_Index, uint64 p_Value) override
            {
                if (p_Index != 1)
                    return;

                m_ComboPointsUsed = p_Value;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_internal_bleeding_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_internal_bleeding_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Envenom - 32645 - Rupture - 1943 - Kidney shot - 408 - Eviscerate - 196819 - NightBlade - 195452 - Run Through - 2098 - Slice And Dice - 5171 - FinalyEviscerate - 197393 - Between The Eyes - 199804 - Death From Above - 152150 - Roll the Bones - 193316
class spell_rog_finishing_moves : public SpellScriptLoader
{
    public:
        spell_rog_finishing_moves() : SpellScriptLoader("spell_rog_finishing_moves") { }

        class spell_rog_finishing_moves_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_finishing_moves_SpellScript);

            enum eSpells
            {
                TraxisTricksyTreads     = 212539,
                NightBlade              = 195452,
                Envenom                 = 32645,
                Eviscerate              = 196819,
                EviscerateAura          = 197496,
                RunThrough              = 2098,
                SliceAndDice            = 5171,
                KidneyShot              = 408,
                InternalBleedingAura    = 154904,
                InternalBleeding        = 154953,
                DeeperStratagem         = 193531,
                DreadbladesAura         = 202665,
                DreadbladesDamage       = 202669,
                TrueBearing             = 193359,
                DeepeningShadows        = 185314,
                ShadowDance             = 185313,
                SystemShockPassive      = 198145,
                SystemShockDamage       = 198222,
                Garrote                 = 703,
                Rupture                 = 1943,
                AgonizingPoison         = 200803,
                DeadlyPoison            = 2818,
                WoundPoison             = 8680,
                FinalyEviscerate        = 197393,
                FinalyNightBlade        = 195452,
                EnvelopingShadows       = 238104,
                FeedingFrenzyProc       = 242705,
                DeathFromAbove          = 152150,
                Alacrity                = 193539,
                AlacrityBuff            = 193538,
                BetweenTheEyes          = 199804,
                RollTheBones            = 193316
            };

            enum eAffectedSpells
            {
                Vanish = 1856,
                AdrenalineRush = 13750,
                Sprint = 231691,
                GrapplingHook = 195457,
                CannonballBarrage = 185767,
                KillingSpree = 51690,
                MarkedForDeath = 137619,
                BetweenTheEyes_Aff = 199804,
                DeathFromAbove_Aff = 152150
            };

            std::array<uint32, 9> m_AffectedSpellsIds =
            {
                {
                    eAffectedSpells::Vanish,
                    eAffectedSpells::AdrenalineRush,
                    eAffectedSpells::Sprint,
                    eAffectedSpells::GrapplingHook,
                    eAffectedSpells::CannonballBarrage,
                    eAffectedSpells::KillingSpree,
                    eAffectedSpells::MarkedForDeath,
                    eAffectedSpells::BetweenTheEyes_Aff,
                    eAffectedSpells::DeathFromAbove_Aff
                }
            };

            int32 m_PowerCost = 0;

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (p_PowerType == Powers::POWER_COMBO_POINT)
                {
                    int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);

                    l_MaxComboPoint = l_Caster->HasAura(eSpells::DeeperStratagem) && l_MaxComboPoint >= 6 ? 6 : 5;
                    p_PowerCost = std::min(l_Caster->GetPower(Powers::POWER_COMBO_POINT), l_MaxComboPoint); ///< override power cost with our current

                    Player* l_Player = l_Caster->ToPlayer();
                    if (l_Player)
                        l_Player->m_SpellHelper.GetUint32(eSpellHelpers::LastFinishingMoveCP) = p_PowerCost;

                    Spell const* l_Spell = GetSpell();
                    if (!l_Spell)
                        return;

                    std::list<Spell::TargetInfo> const& l_TargetInfos = l_Spell->GetTargetInfos();

                    bool l_AllMissed = true;
                    for (Spell::TargetInfo const& l_TargetInfo : l_TargetInfos)
                    {
                        switch (l_TargetInfo.missCondition)
                        {
                            case SpellMissInfo::SPELL_MISS_MISS:
                            case SpellMissInfo::SPELL_MISS_DODGE:
                            case SpellMissInfo::SPELL_MISS_PARRY:
                                break;
                            default:
                                l_AllMissed = false;
                        }
                    }

                    Unit* l_Target = GetExplTargetUnit();
                    if (l_Target && roll_chance_i(l_Target->GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT)))
                        l_AllMissed = true;

                    if (l_AllMissed)
                        p_PowerCost = 0;

                    m_PowerCost = p_PowerCost;

                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::TrueBearing))
                        l_Aura->SetScriptGuid(1, m_PowerCost);

                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::DeepeningShadows, EFFECT_0))
                    {
                        if (SpellInfo const* l_ShadowDance = sSpellMgr->GetSpellInfo(eSpells::ShadowDance))
                        {
                            if (Player* l_Player = l_Caster->ToPlayer())
                            {
                                uint64 reducedtime = ((float)m_PowerCost * ((float)l_AuraEffect->GetAmount() / 10.0f)) * IN_MILLISECONDS;
                                l_Player->ReduceChargeCooldown(l_ShadowDance->ChargeCategoryEntry, reducedtime);
                            }
                        }
                    }

                    if (Aura* l_Alacrity = l_Player->GetAura(eSpells::Alacrity))
                    {
                        if(roll_chance_i(p_PowerCost * 20))
                            l_Player->CastSpell(l_Player, eSpells::AlacrityBuff);

                        if (p_PowerCost == 6 && roll_chance_i(20))
                            l_Player->CastSpell(l_Player, eSpells::AlacrityBuff);
                    }
                }
            }

            void HandleTakePowers()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// Feeding Frenzy
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::FeedingFrenzyProc))
                    l_Aura->DropCharge();

                /// Dreadblades
                if (l_Caster->HasAura(eSpells::DreadbladesAura))
                {
                    int32 l_Damage = l_Caster->GetHealth() * 0.05f;
                    l_Caster->CastCustomSpell(l_Caster, eSpells::DreadbladesDamage, &l_Damage, NULL, NULL, true);
                }
            }

            void HandleDamages(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!m_PowerCost)
                {
                    int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);
                    m_PowerCost = std::min(l_Caster->GetPower(Powers::POWER_COMBO_POINT), l_MaxComboPoint);
                    m_PowerCost = std::min(m_PowerCost, l_Caster->HasAura(eSpells::DeeperStratagem) ? 6 : 5);
                    if (!l_Player->GetCommandStatus(CHEAT_POWER))
                        l_Caster->ModifyPower(Powers::POWER_COMBO_POINT, -m_PowerCost);
                }

                switch (l_SpellInfo->Id)
                {
                    case eSpells::FinalyEviscerate:
                    case eSpells::Eviscerate:
                    {
                        Player* l_Player = l_Caster->ToPlayer();
                        if (l_Player)
                            SetHitDamage(GetHitDamage() * l_Player->m_SpellHelper.GetUint32(eSpellHelpers::LastFinishingMoveCP));

                        break;
                    }
                    case eSpells::Envenom:
                    {
                        Player* l_Player = l_Caster->ToPlayer();
                        if (l_Player)
                            SetHitDamage(GetHitDamage() * l_Player->m_SpellHelper.GetUint32(eSpellHelpers::LastFinishingMoveCP));

                        break;
                    }
                    case eSpells::RunThrough:
                    {
                        int32 l_Damage = GetHitDamage();
                        Player* l_Player = l_Caster->ToPlayer();
                        if (l_Player)
                            l_Damage *= l_Player->m_SpellHelper.GetUint32(eSpellHelpers::LastFinishingMoveCP);

                        float m_Speed = l_Caster->GetSpeed(MOVE_RUN); // is Total (base + bonus) movement speed; 7 is 100% (base) of movement speed.
                        m_Speed = (m_Speed - 7) / 7; // We want the bonus speed in pct % here, refer to the Legendary Traxis.
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TraxisTricksyTreads);
                        if (l_Caster->HasAura(eSpells::TraxisTricksyTreads) && l_SpellInfo)
                            SetHitDamage(AddPct(l_Damage, 100 * CalculatePct(m_Speed, l_SpellInfo->Effects[EFFECT_0].BasePoints)));
                        else
                            SetHitDamage(l_Damage);
                        break;
                    }
                    default:
                        break;
                }
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::KidneyShot:
                    {
                        if (!l_Caster->HasAura(eSpells::InternalBleedingAura))
                            break;

                        Unit* l_Target = GetExplTargetUnit();
                        if (!l_Target)
                            break;

                        l_Caster->CastSpell(l_Target, eSpells::InternalBleeding, true);
                        Aura* l_Aura = l_Target->GetAura(eSpells::InternalBleeding, l_Caster->GetGUID());
                        if (!l_Aura)
                            break;

                        AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                        if (!l_AuraEffect)
                            return;

                        int32 I_Bp = m_PowerCost * ((l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 1.44f) / l_AuraEffect->GetTotalTicks());
                        l_AuraEffect->SetAmount(I_Bp);

                        l_Aura->SetScriptGuid(1, m_PowerCost);
                    }
                    case eSpells::NightBlade:
                    {
                        Unit* l_Target = GetExplTargetUnit();
                        if (!l_Target)
                            break;

                        Aura* l_Aura = l_Target->GetAura(eSpells::NightBlade, l_Caster->GetGUID());
                        if (!l_Aura)
                            break;

                        l_Aura->SetScriptGuid(1, m_PowerCost);
                        break;
                    }
                    default:
                        break;
                }
            }

            void HandleSystemShock()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SystemShockPassive);
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_Caster->HasAura(eSpells::SystemShockPassive))
                    return;

                if ((m_PowerCost >= l_SpellInfo->Effects[EFFECT_0].BasePoints && l_Target->HasAura(eSpells::Garrote) && l_Target->HasAura(eSpells::Rupture)) &&
                    (l_Target->HasAura(eSpells::AgonizingPoison) || l_Target->HasAura(eSpells::DeadlyPoison) || l_Target->HasAura(eSpells::WoundPoison)))
                    l_Caster->CastSpell(l_Target, eSpells::SystemShockDamage, true);
            }

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_PowerCost = l_Caster->GetPower(Powers::POWER_COMBO_POINT);
            }

            void ApplyRestlessBladeEffect()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;


                uint32 l_CurComboPoints = l_Caster->GetPower(POWER_COMBO_POINT);

                for (uint32 l_AffectedSpells : m_AffectedSpellsIds)
                    l_Player->ReduceSpellCooldown(l_AffectedSpells, 0.5f * IN_MILLISECONDS * l_CurComboPoints);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_finishing_moves_SpellScript::HandleTakePowers);

                switch (m_scriptSpellId)
                {
                    case eSpells::Envenom:
                    {
                        OnPrepare += SpellOnPrepareFn(spell_rog_finishing_moves_SpellScript::HandleOnPrepare);
                        AfterCast += SpellCastFn(spell_rog_finishing_moves_SpellScript::HandleSystemShock);
                    }
                    case eSpells::Eviscerate:
                    case eSpells::FinalyEviscerate:
                    case eSpells::RunThrough:
                    {
                        OnTakePowers += SpellTakePowersFn(spell_rog_finishing_moves_SpellScript::HandlePowerCost);
                        OnEffectHitTarget += SpellEffectFn(spell_rog_finishing_moves_SpellScript::HandleDamages, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                        break;
                    }
                    case eSpells::Rupture:
                    {
                        OnTakePowers += SpellTakePowersFn(spell_rog_finishing_moves_SpellScript::HandlePowerCost);
                        OnEffectHitTarget += SpellEffectFn(spell_rog_finishing_moves_SpellScript::HandleDamages, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                        break;
                    }
                    case eSpells::NightBlade:
                    {
                        OnTakePowers += SpellTakePowersFn(spell_rog_finishing_moves_SpellScript::HandlePowerCost);
                        OnEffectHitTarget += SpellEffectFn(spell_rog_finishing_moves_SpellScript::HandleDamages, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                        AfterHit += SpellHitFn(spell_rog_finishing_moves_SpellScript::HandleAfterHit);
                        break;
                    }
                    case eSpells::KidneyShot:
                    {
                        OnTakePowers += SpellTakePowersFn(spell_rog_finishing_moves_SpellScript::HandlePowerCost);
                        AfterHit += SpellHitFn(spell_rog_finishing_moves_SpellScript::HandleAfterHit);
                        break;
                    }
                    case SliceAndDice:
                        OnTakePowers += SpellTakePowersFn(spell_rog_finishing_moves_SpellScript::HandlePowerCost);
                        AfterCast += SpellCastFn(spell_rog_finishing_moves_SpellScript::ApplyRestlessBladeEffect);
                        OnEffectHitTarget += SpellEffectFn(spell_rog_finishing_moves_SpellScript::HandleDamages, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                        break;
                    case eSpells::DeathFromAbove:
                    case eSpells::BetweenTheEyes:
                    case eSpells::RollTheBones:
                        OnTakePowers += SpellTakePowersFn(spell_rog_finishing_moves_SpellScript::HandlePowerCost);
                        break;
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_finishing_moves_SpellScript();
        }
};

/// Last Update Legion 7.3.5 Build 26365
/// Called by Night Blade - 195452, Kidney Shot - 408, Eviscerate - 196819, Death From Above - 152150
/// Called for Relentless Strikes - 58423
class spell_rog_relentless_strikes : public SpellScriptLoader
{
public:
    spell_rog_relentless_strikes() : SpellScriptLoader("spell_rog_relentless_strikes") { }

    class spell_rog_relentless_strikes_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rog_relentless_strikes_SpellScript);

        enum eSpells
        {
            RelentlessStrikes           = 58423,
            RelentlessStrikesTrigger    = 98440,
            ShurikenStorm               = 245640,
            Eviscerate                  = 196819,
            DeathFromAbove              = 152150
        };

        void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::RelentlessStrikes))
                return;

            m_CurComboPoints = l_Caster->GetPower(POWER_COMBO_POINT);
        }

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::RelentlessStrikes))
                return;

            int32 l_EnergyGained = 6 * m_CurComboPoints;     /// gain 6 per combo point spent

            if (m_scriptSpellId == eSpells::DeathFromAbove)
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::DeathFromAboveEnergyBack) = l_EnergyGained;
            else
                l_Caster->CastCustomSpell(l_Caster, eSpells::RelentlessStrikesTrigger, &l_EnergyGained, NULL, NULL, true);
        }

        void HandleAfterHit()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->HasAura(eSpells::ShurikenStorm))
                return;

            l_Caster->RemoveAura(eSpells::ShurikenStorm);
        }

        void Register() override
        {
            OnTakePowers += SpellTakePowersFn(spell_rog_relentless_strikes_SpellScript::HandlePowerCost);
            AfterCast += SpellCastFn(spell_rog_relentless_strikes_SpellScript::HandleAfterCast);
            switch (m_scriptSpellId)
            {
            case eSpells::Eviscerate:
            {
                AfterHit += SpellHitFn(spell_rog_relentless_strikes_SpellScript::HandleAfterHit);
            }
            default:
                break;
            }
        }

    private:
        int32 m_CurComboPoints = 0;
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rog_relentless_strikes_SpellScript();
    }
};

/// Called by Envenom - 32645, Rupture - 1943, Eviscerate - 196819, Night Blade - 195452, Run Through - 2098, Slice And Dice - 5171, Finaly Eviscerate - 197393, Between The Eyes - 199804, Death From Above - 152150, Roll the Bones - 193316
/// Called for Restless Blades - 79096
class spell_rog_restless_blades : public SpellScriptLoader
{
public:
    spell_rog_restless_blades() : SpellScriptLoader("spell_rog_restless_blades") { }

    class spell_rog_restless_blades_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rog_restless_blades_SpellScript);

        enum eSpells
        {
            Envenom = 32645,
            Rupture = 1943,
            KidneyShot = 408,
            Eviscerate = 196819,
            NightBlade = 195452,
            RunThrough = 2098,
            SliceAndDice = 5171,
            FinalyEviscerate = 197393,
            BetweenTheEyes = 199804,
            DeathFromAbove = 152150,
            RollTheBones = 193316,
            TheCurseOfRestlessness = 248107,
            RestlessBlades = 79096,
            TheCurseOftheDreadblades = 202665
        };

        enum eAffectedSpells
        {
            Vanish = 1856,
            AdrenalineRush = 13750,
            Sprint = 231691,
            GrapplingHook = 195457,
            CannonballBarrage = 185767,
            KillingSpree = 51690,
            MarkedForDeath = 137619,
            BetweenTheEyes_Aff = 199804,
            DeathFromAbove_Aff = 152150,
            Sprint2 = 2983
        };

        std::array<uint32, 11> m_FinishingMovesIds =
        {
            {
                eSpells::Envenom,
                eSpells::Rupture,
                eSpells::KidneyShot,
                eSpells::Eviscerate,
                eSpells::NightBlade,
                eSpells::RunThrough,
                eSpells::SliceAndDice,
                eSpells::FinalyEviscerate,
                eSpells::BetweenTheEyes,
                eSpells::DeathFromAbove,
                eSpells::RollTheBones
            }
        };

        std::array<uint32, 10> m_AffectedSpellsIds =
        {
            {
                eAffectedSpells::Vanish,
                eAffectedSpells::AdrenalineRush,
                eAffectedSpells::Sprint,
                eAffectedSpells::GrapplingHook,
                eAffectedSpells::CannonballBarrage,
                eAffectedSpells::KillingSpree,
                eAffectedSpells::MarkedForDeath,
                eAffectedSpells::BetweenTheEyes_Aff,
                eAffectedSpells::DeathFromAbove_Aff,
                eAffectedSpells::Sprint2
            }
        };

        void HandleOnPrepare()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::RestlessBlades))
                return;

            m_CurComboPoints = l_Caster->GetPower(POWER_COMBO_POINT);
        }

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::RestlessBlades, EFFECT_0);
            if (!l_AuraEffect)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            uint32 l_ReduceTime = uint32(((float)l_AuraEffect->GetAmount() / 10.0f) * m_CurComboPoints * IN_MILLISECONDS);

            for (uint32 l_FinishingMove : m_FinishingMovesIds)
            {
                if (GetSpellInfo()->Id == l_FinishingMove)
                {
                    for (uint32 l_AffectedSpells : m_AffectedSpellsIds)
                        l_Player->ReduceSpellCooldown(l_AffectedSpells, l_ReduceTime);

                    break;
                }
            }

            /// And if target has The Curse Of Restlessness aura
            if (AuraEffect* l_AuraEffectLeg = l_Player->GetAuraEffect(eSpells::TheCurseOfRestlessness, EFFECT_0))
            {
                l_ReduceTime = uint32(((float)l_AuraEffectLeg->GetAmount() / 100.0f) * m_CurComboPoints * IN_MILLISECONDS);
                l_Player->ReduceSpellCooldown(eSpells::TheCurseOftheDreadblades, l_ReduceTime);
            }
        }

        void Register() override
        {
            OnPrepare += SpellOnPrepareFn(spell_rog_restless_blades_SpellScript::HandleOnPrepare);
            AfterCast += SpellCastFn(spell_rog_restless_blades_SpellScript::HandleAfterCast);
        }

    private:
        int32 m_CurComboPoints = 0;
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rog_restless_blades_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Slice and Dice - 5171
class spell_rog_slice_and_dice : public SpellScriptLoader
{
    public:

        spell_rog_slice_and_dice() : SpellScriptLoader("spell_rog_slice_and_dice") { }

        class spell_rog_slice_and_dice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_slice_and_dice_AuraScript);

            enum eSpells
            {
                LoadedDiceBuff      = 240837
            };

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (AuraEffect const* l_LoadedDiceEffect = l_Owner->GetAuraEffect(eSpells::LoadedDiceBuff, SpellEffIndex::EFFECT_0))
                    AddPct(p_Amount, l_LoadedDiceEffect->GetAmount());
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                l_Owner->RemoveAura(eSpells::LoadedDiceBuff);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_slice_and_dice_AuraScript::CalculateAmount, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_MELEE_RANGED_HASTE_2);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_slice_and_dice_AuraScript::CalculateAmount, SpellEffIndex::EFFECT_2, AuraType::SPELL_AURA_MOD_POWER_REGEN_PERCENT);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_slice_and_dice_AuraScript::HandleAfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_MELEE_RANGED_HASTE_2, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_slice_and_dice_AuraScript();
        }

        class spell_rog_slice_and_dice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_slice_and_dice_SpellScript);

            enum eSpells
            {
                DeeperStratagem = 193531
            };

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_PowerType == Powers::POWER_COMBO_POINT)
                {
                    int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);

                    l_MaxComboPoint = l_Caster->HasAura(eSpells::DeeperStratagem) && l_MaxComboPoint >= 6 ? 6 : 5;
                    m_PowerCost = std::min(l_Caster->GetPower(Powers::POWER_COMBO_POINT), l_MaxComboPoint); ///< override power cost with our current
                }
            }

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(GetSpellInfo()->Id))
                    m_Duration = l_Aura->GetDuration();
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(GetSpellInfo()->Id))
                {
                    uint32 l_DurationToAdd = (12 + ((m_PowerCost - 1) * 6)) * IN_MILLISECONDS;
                    if (m_Duration)
                    {
                        if (m_Duration >= CalculatePct(l_DurationToAdd, 30))
                            l_DurationToAdd += CalculatePct(l_DurationToAdd, 30);
                        else
                            l_DurationToAdd += m_Duration;
                    }

                    l_Aura->SetDuration(l_DurationToAdd);
                    l_Aura->SetMaxDuration(l_DurationToAdd);
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_rog_slice_and_dice_SpellScript::HandleOnPrepare);
                OnTakePowers += SpellTakePowersFn(spell_rog_slice_and_dice_SpellScript::HandlePowerCost);
                AfterCast += SpellCastFn(spell_rog_slice_and_dice_SpellScript::HandleAfterCast);
            }

        private:
            int32 m_PowerCost;
            int32 m_Duration = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_slice_and_dice_SpellScript();
        }
};

/// Last Update Legion 7.3.2 Build 25549
/// Called by Shuriken storm - 197835
class spell_rog_shuriken_storm : public SpellScriptLoader
{
    public:
        spell_rog_shuriken_storm() : SpellScriptLoader("spell_rog_shuriken_storm") { }

        class spell_rog_shuriken_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shuriken_storm_SpellScript);

            enum eSpells
            {
                DreadlordsDeceitSubtlety    = 228224,
                ShurikenCombo               = 245639,
                ShurikenBonus               = 245640,
                Subterfuge                  = 115192,
                ShadowDance                 = 185422,
                ShurikenStormGain           = 212743,
                ShadowBlades                = 121471,
                ShadowBladesDmg             = 121473,
                ShadowBladesDmgOff          = 121474
            };

            bool m_IsStealth = false;
            uint8 m_Cast = 0;
            bool m_ShadowBlades = false;

            bool Load() override
            {
                if (Unit* l_Caster = GetCaster())
                    m_ShadowBlades = l_Caster->HasAura(eSpells::ShadowBlades);

                return true;
            }

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_STEALTH) || l_Caster->HasAura(eSpells::Subterfuge))
                    m_IsStealth = true;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (m_IsStealth || l_Caster->HasAura(eSpells::ShadowDance))
                    SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), l_SpellInfo->Effects[EFFECT_2].BasePoints));

                int32 l_ComboPoints = m_ShadowBlades ? 2 : 1;
                l_Caster->CastCustomSpell(eSpells::ShurikenStormGain, SpellValueMod::SPELLVALUE_BASE_POINT0, l_ComboPoints, l_Caster, true);

                if (m_ShadowBlades && l_Target != nullptr)
                    l_Caster->CastSpell(l_Target, eSpells::ShadowBladesDmg, true);

                if (l_Caster->HasAura(eSpells::ShurikenCombo) && m_Cast != 0)
                    l_Caster->CastSpell(l_Caster, eSpells::ShurikenBonus, true);

                m_Cast++;
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::DreadlordsDeceitSubtlety);
                if (!l_Aura)
                    return;
                l_Aura->Remove();
            }

            void HandleAfterCast()
            {
                m_Cast = 0;
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_rog_shuriken_storm_SpellScript::HandleOnPrepare);
                OnEffectHitTarget += SpellEffectFn(spell_rog_shuriken_storm_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterHit += SpellHitFn(spell_rog_shuriken_storm_SpellScript::HandleAfterHit);
                AfterCast += SpellCastFn(spell_rog_shuriken_storm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shuriken_storm_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Shuriken Combo - 245640
class spell_rog_shuriken_combo_pvp : public SpellScriptLoader
{
    public:
        spell_rog_shuriken_combo_pvp() : SpellScriptLoader("spell_rog_shuriken_combo_pvp") { }

        class spell_rog_shuriken_combo_pvp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shuriken_combo_pvp_AuraScript);

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

                p_Amount /= 5;  ///< Shuriken Combo deals 2% increased damage per stack on 7.3.5 (Equiinoxx remembered that from 7.3.5 retail)
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_shuriken_combo_pvp_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shuriken_combo_pvp_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Nightstalker damage buff - 130493
class spell_druid_nightstalker_pvp_mod : public SpellScriptLoader
{
    public:
        spell_druid_nightstalker_pvp_mod() : SpellScriptLoader("spell_druid_nightstalker_pvp_mod") { }

        class spell_druid_nightstalker_pvp_mod_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_druid_nightstalker_pvp_mod_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                if (GetCaster() && GetCaster()->IsPlayer() && GetCaster()->ToPlayer()->CanApplyPvPSpellModifiers())
                    p_Amount *= 0.50f;   ///< Nightstalker buff is halved in PVP situations
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_druid_nightstalker_pvp_mod_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_druid_nightstalker_pvp_mod_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Death from Above - 152150
class spell_rog_death_from_above : public SpellScriptLoader
{
    public:
        spell_rog_death_from_above() : SpellScriptLoader("spell_rog_death_from_above") { }

        class spell_rog_death_from_above_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_death_from_above_SpellScript);

            enum eSpells
            {
                DeathFromAboveBonus = 163786,
                DeathFromAboveJump  = 178236,
                DeepeningShadows    = 185314,
                ShadowDance         = 185313,
                EnvelopingShadows   = 238104
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();


                if (l_Target == nullptr)
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (!l_Caster->IsWithinLOSInMap(l_Target))
                    return SPELL_FAILED_LINE_OF_SIGHT;

                return SPELL_CAST_OK;
            }

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (p_PowerType == Powers::POWER_COMBO_POINT)
                {
                    if (l_Caster && l_Caster->HasAura(eSpells::DeepeningShadows))
                    {
                        if (SpellInfo const* l_DeepeningShadows = sSpellMgr->GetSpellInfo(eSpells::DeepeningShadows))
                        {
                            if (SpellInfo const* l_ShadowDance = sSpellMgr->GetSpellInfo(eSpells::ShadowDance))
                            {
                                if (Player* l_Player = l_Caster->ToPlayer())
                                {
                                    uint32 l_PowerCost = l_Player->GetPower(POWER_COMBO_POINT);
                                    uint32 reducedtime = (l_PowerCost * (l_DeepeningShadows->Effects[EFFECT_0].BasePoints / 10));
                                    if (AuraEffect* I_AuraEffect = l_Caster->GetAuraEffect(eSpells::EnvelopingShadows, EFFECT_0))
                                        reducedtime += (I_AuraEffect->GetAmount() / 10) * l_PowerCost;
                                    l_Player->ReduceChargeCooldown(l_ShadowDance->ChargeCategoryEntry, reducedtime * IN_MILLISECONDS);
                                }
                            }
                        }
                    }
                }

            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                l_Caster->CastSpell(l_Caster, eSpells::DeathFromAboveJump, true);
                l_Caster->CastSpell(l_Caster, eSpells::DeathFromAboveBonus, true);
            }

            void HandleRegisterTargetGUID()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster || !l_Caster->ToPlayer())
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                {
                    l_Aura->SetScriptGuid(0, l_Target->GetGUID());
                    l_Caster->ToPlayer()->m_SpellHelper.GetUint64(eSpellHelpers::DeathFromAboveTargetGuid) = l_Target->GetGUID();
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_death_from_above_SpellScript::CheckCast);
                OnTakePowers += SpellTakePowersFn(spell_rog_death_from_above_SpellScript::HandlePowerCost);
                AfterCast += SpellCastFn(spell_rog_death_from_above_SpellScript::HandleAfterCast);
                AfterHit += SpellHitFn(spell_rog_death_from_above_SpellScript::HandleRegisterTargetGUID);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_death_from_above_SpellScript();
        }

        class spell_rog_death_from_above_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_death_from_above_AuraScript);

            enum eSpells
            {
                DeathFromAboveJumpToTarget  = 156327,
                Envenom                     = 32645,
                RunThrough                  = 2098,
                Eviscerate                  = 196819,
                EviscerateAura              = 197496,
                RelentlessStrikesTrigger    = 98440
            };

            uint64 m_guid = 0;
            bool m_Applied = false;
            bool m_SecondApplied = false;

            void SetGuid(uint32 index, uint64 guid) override
            {
                switch (index)
                {
                    case 0:
                    {
                        m_guid = guid;
                        break;
                    }
                    case 1:
                    {
                        if (m_Applied)
                            break;

                        m_Applied = true;
                        if (Unit* l_Caster = GetUnitOwner())
                        {
                            if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, m_guid))
                                l_Caster->CastSpell(l_Target, eSpells::DeathFromAboveJumpToTarget, true);
                        }

                        break;
                    }
                    case 2:
                    {
                        if (m_SecondApplied)
                            break;

                        m_SecondApplied = true;
                        Unit* l_Caster = GetUnitOwner();
                        Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, m_guid);
                        if (!l_Caster || !l_Target)
                            return;

                        Player* l_Player = l_Caster->ToPlayer();
                        if (!l_Player)
                            return;

                        switch (l_Player->GetActiveSpecializationID())
                        {
                            case SpecIndex::SPEC_ROGUE_ASSASSINATION:
                            {
                                uint64 l_Guid = m_guid;
                                l_Caster->AddDelayedEvent([l_Caster, l_Guid]() -> void
                                {
                                    if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Guid))
                                    {
                                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::BlockCPGain) = true;
                                        l_Caster->CastSpell(l_Target, eSpells::Envenom, true);
                                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::BlockCPGain) = false;
                                    }
                                }, 0.4 * IN_MILLISECONDS);
                                break;
                            }
                            case SpecIndex::SPEC_ROGUE_SUBTLETY:
                            {
                                uint64 l_Guid = m_guid;
                                l_Caster->AddDelayedEvent([l_Caster, l_Guid]() -> void
                                {
                                    if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Guid))
                                    {
                                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::BlockCPGain) = true;
                                        l_Caster->CastSpell(l_Target, eSpells::Eviscerate, true);
                                        if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::DeathFromAboveEnergyBack) > 0)
                                        {
                                            int32 l_Bp0 = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::DeathFromAboveEnergyBack);
                                            l_Caster->CastCustomSpell(l_Caster, eSpells::RelentlessStrikesTrigger, &l_Bp0, NULL, NULL, true);
                                        }
                                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::BlockCPGain) = false;
                                    }
                                }, 1 * IN_MILLISECONDS);
                                break;
                            }
                            case SpecIndex::SPEC_ROGUE_OUTLAW:
                            {
                                uint64 l_Guid = m_guid;
                                l_Caster->AddDelayedEvent([l_Caster, l_Guid]() -> void
                                {
                                    if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Guid))
                                    {
                                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::BlockCPGain) = true;
                                        l_Caster->CastSpell(l_Target, eSpells::RunThrough, true);
                                        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::BlockCPGain) = false;
                                    }
                                }, 0.4 * IN_MILLISECONDS);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override { }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_death_from_above_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Death from Above (Leap) - 178236
class spell_rog_death_from_above_leap : public SpellScriptLoader
{
    public:
        spell_rog_death_from_above_leap() : SpellScriptLoader("spell_rog_death_from_above_leap") { }

        class spell_rog_death_from_above_leap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_death_from_above_leap_SpellScript);

            void ChangeJumpDestination(SpellEffIndex p_EffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    PreventHitDefaultEffect(p_EffIndex);

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;

                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Player->m_SpellHelper.GetUint64(eSpellHelpers::DeathFromAboveTargetGuid));
                    if (!l_Target)
                        return;

                    if (!l_Caster->IsValidAttackTarget(l_Target))
                        return;

                    if (l_Caster->GetDistance(l_Target) > (MELEE_RANGE - 0.5f) && l_Caster->GetDistance(l_Target) < (MELEE_RANGE + 0.5f))
                    {
                        l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionX) = l_Caster->GetPositionX();
                        l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionY) = l_Caster->GetPositionY();
                        l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionZ) = l_Caster->GetPositionZ();
                        l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAboveAngle) = l_Caster->GetAngle(l_Target);
                    }
                    else
                    {
                        if (l_Target->HasInArc(M_PI, l_Caster))
                            l_Caster->m_SpellHelper.GetBool(eSpellHelpers::DeathFromAboveInFront) = true;
                    }

                    SpellInfo const* l_SpellInfo = GetSpellInfo();
                    float l_X = l_Caster->GetPositionX() + (5.0f * cos(l_Caster->GetOrientation() + M_PI));
                    float l_Y = l_Caster->GetPositionY() + (5.0f * sin(l_Caster->GetOrientation() + M_PI));
                    float l_Z = l_Caster->GetPositionZ() + 15.0f;

                    float l_SpeedXY, l_SpeedZ;

                    if (l_SpellInfo->Effects[p_EffIndex].MiscValue)
                        l_SpeedZ = float(l_SpellInfo->Effects[p_EffIndex].MiscValue) / 10;
                    else if (l_SpellInfo->Effects[p_EffIndex].MiscValueB)
                        l_SpeedZ = float(l_SpellInfo->Effects[p_EffIndex].MiscValueB) / 10;
                    else
                        l_SpeedZ = 10.0f;

                    if (l_SpellInfo->Effects[p_EffIndex].ValueMultiplier)
                        l_SpeedXY = l_SpellInfo->Effects[p_EffIndex].ValueMultiplier;
                    else
                        l_SpeedXY = l_Caster->GetExactDist2d(l_X, l_Y) * 10.0f / l_SpeedZ;

                    l_Caster->GetMotionMaster()->MoveJump(l_X, l_Y, l_Z, l_SpeedXY, l_SpeedZ, l_Caster->GetOrientation(), l_SpellInfo->Id);
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_rog_death_from_above_leap_SpellScript::ChangeJumpDestination, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_death_from_above_leap_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Death From Above (Return) - 156327
class spell_rog_death_from_above_return : public SpellScriptLoader
{
    public:
        spell_rog_death_from_above_return() : SpellScriptLoader("spell_rog_death_from_above_return") { }

        class spell_rog_death_from_above_return_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_death_from_above_return_SpellScript);

            enum eSpells
            {
                DeathFromAbove      = 152150,
                DeathFromAboveJump  = 178236,

                SurgingWaters       = 192633
            };

            enum eNPC
            {
                WrathOfAzshara      = 96028,
                Naraxas             = 91005,
                FallenAvatar        = 116939,
                KinGaroth           = 122578
            };

            void HandleAfterHit(SpellEffIndex p_SpellEffectIndex)
            {
                PreventHitDefaultEffect(p_SpellEffectIndex);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeathFromAboveJump);
                Unit* l_Caster = GetCaster();
                if (!l_Caster->ToPlayer())
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->ToPlayer()->m_SpellHelper.GetUint64(eSpellHelpers::DeathFromAboveTargetGuid));
                if (!l_Caster || !l_SpellInfo || !l_Target || !l_Caster->IsValidAttackTarget(l_Target))
                    return;

                float l_Angle = l_Caster->GetAngle(l_Target);
                if (l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAboveAngle) != 0)
                    l_Angle = l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAboveAngle);

                float l_PositionX = l_Target->GetPositionX() - ((l_Target->GetCombatReach() / 2) * cos(l_Target->GetOrientation()));
                float l_PositionY = l_Target->GetPositionY() - ((l_Target->GetCombatReach() / 2) * sin(l_Target->GetOrientation()));

                if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::DeathFromAboveInFront))
                {
                    l_PositionX = l_Target->GetPositionX() + ((l_Target->GetCombatReach() / 2) * cos(l_Target->GetOrientation()));
                    l_PositionY = l_Target->GetPositionY() + ((l_Target->GetCombatReach() / 2) * sin(l_Target->GetOrientation()));
                    l_Angle -= M_PI;
                }

                float l_PositionZ = l_Target->GetPositionZ();
                l_Caster->UpdateAllowedPositionZ(l_PositionX, l_PositionY, l_PositionZ);
                Position l_Pos = Position(l_PositionX, l_PositionY, l_PositionZ);

                if (JadeCore::IsValidMapCoord(l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionX), l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionY), l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionZ)))
                {
                    Position l_TempPos = Position(l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionX), l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionY), l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionZ));
                    if (l_Caster->GetDistance(l_TempPos) <= 50.0f)
                        l_Pos = l_TempPos;
                }

                if (l_Target->ToCreature())
                {
                    switch (l_Target->ToCreature()->GetEntry())
                    {
                        case eNPC::WrathOfAzshara:
                            l_Pos = l_Target->GetPosition();
                            l_Target->DelayedCastSpell(l_Caster, eSpells::SurgingWaters, true, 1700);
                            break;
                        case eNPC::Naraxas:
                            l_Pos = Position(3001.2275f, 1821.2341f, -60.033695f);
                            break;
                        case eNPC::FallenAvatar:
                            l_Pos = Position(6590.0039f, -795.469177f, 1663.58f);
                            break;
                        case eNPC::KinGaroth:
                            l_Pos = l_Caster->GetPosition();
                        default:
                            break;
                    }
                }

                l_Pos.m_positionZ += 1.0f; ///< Try to prevent possible fallings under ground

                l_Caster->GetMotionMaster()->MoveJump(l_Pos, l_SpellInfo->Effects[EFFECT_0].MiscValue, l_SpellInfo->Effects[EFFECT_0].MiscValue / 10, l_Angle);

                l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAboveAngle) = 0.0f;
                l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionX) = 0.0f;
                l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionY) = 0.0f;
                l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::DeathFromAbovePositionZ) = 0.0f;
                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::DeathFromAboveInFront) = false;

                if (Aura* l_DeathFromAbove = l_Caster->GetAura(eSpells::DeathFromAbove))
                {
                    l_DeathFromAbove->SetScriptGuid(2, 0);
                    l_Caster->ToPlayer()->m_SpellHelper.GetUint64(eSpellHelpers::DeathFromAboveTargetGuid) = 0;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_death_from_above_return_SpellScript::HandleAfterHit, EFFECT_1, SPELL_EFFECT_JUMP);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_death_from_above_return_SpellScript();
        }
};

class PlayerScript_Death_From_Above : public PlayerScript
{
    public:
        PlayerScript_Death_From_Above() : PlayerScript("PlayerScript_Death_From_Above") { }

        enum eSpells
        {
            DeathFromAbove = 152150,
        };

        void OnMoveSplineDone(Player* p_Player)
        {
            if (p_Player->getClass() != CLASS_ROGUE || !p_Player->HasAura(eSpells::DeathFromAbove))
                return;

            if (Aura* l_DeathFromAbove = p_Player->GetAura(eSpells::DeathFromAbove))
            {
                l_DeathFromAbove->SetScriptGuid(1, 0);
            }
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Death From Above (Bonus) - 163786
class spell_rog_death_from_above_bonus : public SpellScriptLoader
{
    public:
        spell_rog_death_from_above_bonus() : SpellScriptLoader("spell_rog_death_from_above_bonus") { }

        class spell_rog_death_from_above_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_death_from_above_bonus_AuraScript);

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

                /// Death from Above increases the effect of Envenom, Run Through and Eviscerate by 25 % (down from 50 % )
                l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() / 2);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_death_from_above_bonus_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_death_from_above_bonus_AuraScript::HandleAfterApply, EFFECT_2, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_death_from_above_bonus_AuraScript::HandleAfterApply, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_death_from_above_bonus_AuraScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Called by Garrote - 703
class spell_rog_garrote : public SpellScriptLoader
{
    public:
        spell_rog_garrote() : SpellScriptLoader("spell_rog_garrote") { }

        class spell_rog_garrote_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_garrote_SpellScript);

            enum eSpells
            {
                GarroteStun = 1330,
                Subterfuge = 115192
            };

            bool m_IsStealth = false;

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_STEALTH) || l_Caster->HasAura(Subterfuge))
                    m_IsStealth = true;

            }

            void HandleOnHit(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (m_IsStealth) ///< Silences the target for 3 sec when used from Stealth.
                    l_Caster->CastSpell(l_Target, eSpells::GarroteStun, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_rog_garrote_SpellScript::HandleOnPrepare);
                OnEffectHitTarget += SpellEffectFn(spell_rog_garrote_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_garrote_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Blood Of The Assassinated - 192925
class spell_rog_blood_of_the_assassinated : public SpellScriptLoader
{
    public:
        spell_rog_blood_of_the_assassinated() : SpellScriptLoader("spell_rog_blood_of_the_assassinated") { }

        class spell_rog_blood_of_the_assassinated_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_blood_of_the_assassinated_AuraScript);

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                AuraEffect* l_AuraEffect = const_cast<AuraEffect*>(p_AuraEffect);

                /// Blood of the Assassinated increases rupture damage by 30 % in PvP(down from 100 % )
                l_AuraEffect->ChangeAmount(30);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_blood_of_the_assassinated_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_blood_of_the_assassinated_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Called by Exsanguinate - 703
class spell_rog_exsanguinate : public SpellScriptLoader
{
    public:
        spell_rog_exsanguinate() : SpellScriptLoader("spell_rog_exsanguinate") { }

        class spell_rog_exsanguinate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_exsanguinate_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                std::list<AuraEffect*> l_BleedAuraEffects = l_Target->GetAuraEffectsByMechanic(1LL << MECHANIC_BLEED);
                std::set<uint32> l_AuraIds;
                for (AuraEffect* l_AuraEffect : l_BleedAuraEffects)
                {
                    if (l_AuraEffect->GetAuraType() != SPELL_AURA_PERIODIC_DAMAGE && l_AuraEffect->GetAuraType() != SPELL_AURA_PERIODIC_DUMMY && l_AuraEffect->GetAuraType() != SPELL_AURA_PERIODIC_DAMAGE_PERCENT)
                        continue;

                    Unit* l_AuraCaster = l_AuraEffect->GetCaster();
                    if (!l_AuraCaster)
                        continue;

                    if (l_Caster->GetGUID() == l_AuraCaster->GetGUID())
                    {
                        Aura* l_Aura = l_AuraEffect->GetBase();
                        if (!l_Aura)
                            continue;

                        SpellInfo const* l_SpellInfo = l_Aura->GetSpellInfo();
                        if (!l_SpellInfo)
                            continue;

                        l_AuraIds.insert(l_SpellInfo->Id);
                        l_AuraEffect->SetAmplitude(l_AuraEffect->GetAmplitude() / 2);
                    }
                }

                for (uint32 l_AuraId : l_AuraIds)
                {
                    Aura* l_Aura = l_Target->GetAura(l_AuraId, l_Caster->GetGUID());
                    if (!l_Aura)
                        continue;

                    l_Aura->SetDuration(l_Aura->GetDuration() / 2);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_exsanguinate_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_exsanguinate_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Elaborate Planning - 193640
class spell_rog_elaborate_planning : public SpellScriptLoader
{
    public:
        spell_rog_elaborate_planning() : SpellScriptLoader("spell_rog_elaborate_planning") { }

        class spell_rog_elaborate_planning_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_elaborate_planning_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                bool l_PreventProc = true;
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();

                if (!l_DamageInfo || !l_SpellInfo)
                    return;

                if (l_SpellInfo->IsFinishingMove())
                    l_PreventProc = false;

                if (l_PreventProc)
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_elaborate_planning_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_elaborate_planning_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Kingsbane - 192759
class spell_rog_kingsbane : public SpellScriptLoader
{
    public:
        spell_rog_kingsbane() : SpellScriptLoader("spell_rog_kingsbane") { }

        class spell_rog_kingsbane_AuraScript : public AuraScript
        {
            enum eSpells
            {
                KingsbaneIncreaseDmg    = 192853
            };

            PrepareAuraScript(spell_rog_kingsbane_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Target = l_DamageInfo->GetTarget();

                /// ProcSpell need to be a lethal poison, and target needs to have Kingsbane applied on him, otherwise you could apply poison on other targets and still increase Kingsbane's Damage.
                if (!l_SpellInfo || !l_SpellInfo->IsLethalPoison() || l_SpellInfo->Id == 2818 || !l_Target || !l_Target->HasAura(m_scriptSpellId, l_Caster->GetGUID())) ///< Deadly Poison application is 113780, NOT 2818
                    return false;

                return true;
            }


            void AfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                /// Damage per tick are calculated for 6 ticks instead of 7 for some reason
                Aura* l_Aura = p_AuraEffect->GetBase();
                if (!l_Aura)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_3);
                if (!l_AuraEffect)
                    return;

                l_AuraEffect->SetAmount(int32((float)l_AuraEffect->GetAmount() * (6.0f / 7.0f)));
            }

            void HandleAfterRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::KingsbaneIncreaseDmg))
                    l_Caster->RemoveAura(eSpells::KingsbaneIncreaseDmg);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_kingsbane_AuraScript::HandleAfterRemove, EFFECT_3, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                DoCheckProc += AuraCheckProcFn(spell_rog_kingsbane_AuraScript::HandleCheckProc);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_kingsbane_AuraScript::AfterApply, EFFECT_3, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_kingsbane_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by KingsbaneMainHand - 222062 & KingsbaneOffHand - 192760
class spell_rog_kingsbane_damage : public SpellScriptLoader
{
    public:
        spell_rog_kingsbane_damage() : SpellScriptLoader("spell_rog_kingsbane_damage") { }

        class spell_rog_kingsbane_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_kingsbane_damage_SpellScript);

            enum eSpells
            {
                SealFateAura    = 14190,
                SealFate        = 14189
            };

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::SealFateAura))
                    return;

                bool l_Crit = false;
                if (!GetSpell())
                    return;

                for (auto l_TargetInfo : GetSpell()->GetTargetInfos())
                {
                    if (l_TargetInfo.crit)
                        l_Crit = true;
                }

                if (!l_Crit)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SealFate, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_kingsbane_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_kingsbane_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Garrote - 703
/// Called for Thuggee - 196861
class spell_rog_thuggee : public SpellScriptLoader
{
    public:
        spell_rog_thuggee() : SpellScriptLoader("spell_rog_thuggee") { }

        enum eSpells
        {
            Thugee = 196861
        };

        class spell_rog_thuggee_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_thuggee_AuraScript);

            void HandleAfterRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Caster || !l_SpellInfo || !l_Owner || l_Owner->isAlive())
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (l_Player->HasAura(eSpells::Thugee))
                        l_Player->RemoveSpellCooldown(m_scriptSpellId, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_thuggee_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_thuggee_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Deadly Poison - 2818 - Crippling Poison - 3409 - Wound Poison - 8680 -  Leeching Poison - 112961 - Agonizing Poison - 200803
/// Called For Venomous Wounds - 79134
class spell_rog_venom_rush : public SpellScriptLoader
{
    public:
        spell_rog_venom_rush() : SpellScriptLoader("spell_rog_venom_rush") { }

        enum eSpells
        {
            WoundPoison             = 8680,
            VenomRushAura           = 152152,
            DeadlyPoisonDot         = 2818,
            CreeplingPoisonDebuff   = 3409,
            LeechingPoisonDebuff    = 112961,
            SinisterCirculation     = 238138,
            Kingsbane               = 192759,
            MasterPoisoner         = 196864
        };

        class spell_rog_venom_rush_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_venom_rush_AuraScript);

            void OnApply(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    Player* l_Player = l_Caster->ToPlayer();

                    if (l_Player == nullptr)
                        return;

                    if (GetSpellInfo()->Id == 3409 && l_Player->GetActiveSpecializationID() == SPEC_ROGUE_OUTLAW)
                        return;

                    if (Unit* l_Target = GetTarget())
                    {
                        l_Caster->AddPoisonTarget(GetSpellInfo()->Id, l_Target->GetGUIDLow());
                    }

                    if (l_Player->HasAura(eSpells::SinisterCirculation))
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SinisterCirculation);
                        if (!l_SpellInfo)
                            return;

                        l_Player->ReduceSpellCooldown(eSpells::Kingsbane, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                    }
                }
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Caster = GetCaster())
                {

                    if (Unit* l_Target = GetTarget())
                    {
                        l_Caster->RemovePoisonTarget(l_Target->GetGUIDLow(), GetSpellInfo()->Id);
                    }
                }
            }

            void HandleCreeplingPvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.60f;   ///< Creepling Poison snare is reduced by 40% in PvP
            }

            void HandleWoundPvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.80f;   ///< Wound Poison heal reduction is reduced by 20% in PvP

                if (l_Player->HasAura(eSpells::MasterPoisoner))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MasterPoisoner))
                        AddPct(p_Amount, l_SpellInfo->Effects[EFFECT_3].BasePoints);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::DeadlyPoisonDot:
                        OnEffectApply += AuraEffectApplyFn(spell_rog_venom_rush_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                        OnEffectRemove += AuraEffectRemoveFn(spell_rog_venom_rush_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                        break;
                    case eSpells::CreeplingPoisonDebuff:
                        OnEffectApply += AuraEffectApplyFn(spell_rog_venom_rush_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                        OnEffectRemove += AuraEffectRemoveFn(spell_rog_venom_rush_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_venom_rush_AuraScript::HandleCreeplingPvPModifier, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED);
                        break;
                    case eSpells::WoundPoison:
                        OnEffectApply += AuraEffectApplyFn(spell_rog_venom_rush_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_HEALING_PCT, AURA_EFFECT_HANDLE_REAL);
                        OnEffectRemove += AuraEffectRemoveFn(spell_rog_venom_rush_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_HEALING_PCT, AURA_EFFECT_HANDLE_REAL);
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_venom_rush_AuraScript::HandleWoundPvPModifier, EFFECT_1, SPELL_AURA_MOD_HEALING_PCT);
                        break;
                    case eSpells::LeechingPoisonDebuff:
                        OnEffectApply += AuraEffectApplyFn(spell_rog_venom_rush_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        OnEffectRemove += AuraEffectRemoveFn(spell_rog_venom_rush_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_venom_rush_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Called By Venomous Wounds - 79134
class spell_rog_venomous_wounds_proc : public SpellScriptLoader
{
    public:
        spell_rog_venomous_wounds_proc() : SpellScriptLoader("spell_rog_venomous_wounds_proc") { }

        enum eSpells
        {
            VenomousVimEnergize = 51637,
            VenomRush           = 152152,
            CruelGarrote        = 230011,
            MutilatedFlesh      = 211672
        };

        class spell_rog_venomous_wounds_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_venomous_wounds_proc_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActionTarget();
                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                if (l_SpellInfo == nullptr || l_Caster == nullptr || l_Target == nullptr)
                    return;

                if (!l_SpellInfo->HasEffectMechanic(Mechanics::MECHANIC_BLEED))
                    return;

                if (l_SpellInfo->Id == eSpells::CruelGarrote || l_SpellInfo->Id == eSpells::MutilatedFlesh)
                    return;

                if (!l_Target->HasAuraWithDispelFlagsFromCaster(l_Caster, DispelType::DISPEL_POISON))
                    return;

                if (AuraEffect const* l_AuraEffect = p_AurEff->GetBase()->GetEffect(EFFECT_1))
                {
                    int32 l_Amount = l_AuraEffect->GetAmount();
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::VenomRush))
                    {
                        if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0))
                            l_Amount += l_AuraEffect->GetAmount();
                    }

                    l_Caster->EnergizeBySpell(l_Caster, eSpells::VenomousVimEnergize, l_Amount, POWER_ENERGY);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_venomous_wounds_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_venomous_wounds_proc_AuraScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Called by Saber Slash - 193315
class spell_rog_saber_slash : public SpellScriptLoader
{
    public:
        spell_rog_saber_slash() : SpellScriptLoader("spell_rog_saber_slash") { }

        class spell_rog_saber_slash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_saber_slash_SpellScript);

            enum eSpells
            {
                ExtraSaberSlash = 197834,
                Swordmaster     = 200733,
                FreePistolAura  = 195627,
                BlunderbussProc = 202897,
                BlunderbussAura = 202848,
                HiddenBlade     = 202754,
                JollyRoger      = 199603,
                T21Outlaw2PBonus = 251778,
                SharpenedSabers = 252285
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return;

                uint8 l_ChancePCt = GetSpellInfo()->Effects[EFFECT_4].BasePoints;
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Swordmaster, EFFECT_0))
                    l_ChancePCt += l_AuraEffect->GetAmount();

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::JollyRoger, EFFECT_0))
                    l_ChancePCt += l_AuraEffect->GetAmount();

                if (l_Caster->HasAura(eSpells::HiddenBlade) || roll_chance_i(l_ChancePCt))
                {
                    l_Caster->RemoveAurasDueToSpell(eSpells::HiddenBlade);
                    l_Caster->CastSpell(l_Caster, eSpells::FreePistolAura, true);
                    l_Caster->CastSpell(l_Target, eSpells::ExtraSaberSlash, true);

                    if (l_Caster->HasAura(eSpells::T21Outlaw2PBonus))
                        l_Caster->CastSpell(l_Caster, eSpells::SharpenedSabers, true);

                    SpellInfo const* l_SpellInfoBlunderbussProc = sSpellMgr->GetSpellInfo(eSpells::BlunderbussProc);
                    if (!l_SpellInfoBlunderbussProc || !l_Caster->HasAura(eSpells::BlunderbussProc))
                        return;

                    uint8 l_ChancePctBlunderbussProc = l_SpellInfoBlunderbussProc->Effects[EFFECT_1].BasePoints;
                    if (roll_chance_i(l_ChancePctBlunderbussProc))
                        l_Caster->CastSpell(l_Caster, eSpells::BlunderbussAura, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_saber_slash_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_saber_slash_SpellScript();
        }
};

class TrueBearingWorker
{
    public:

        TrueBearingWorker()
        {
            m_ReduceTimePerCombo = 0;

            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TrueBearing))
                m_ReduceTimePerCombo = 1 * IN_MILLISECONDS;
        }

        void ReduceCooldowns(Player* l_Player, uint32 comboPoints)
        {
            if (!l_Player)
                return;

            for (uint32 l_SpellId : m_SpellArray)
                l_Player->ReduceSpellCooldown(l_SpellId, m_ReduceTimePerCombo * comboPoints);
        }

    private:

        enum eSpells
        {
            AdrenalineRush = 13750,
            Sprint = 2983,
            BetweenTheEyes = 199804,
            Vanish = 1856,
            Blind = 2094,
            CloakOfShadows = 31224,
            Riposte = 199754,
            GrapplingHook = 195457,
            CannonballBarrage = 185767,
            KillingSpree = 51690,
            MarkedForDeath = 137619,
            DeathFromAbove = 152150,
            RunThrough = 2098,
            TrueBearing = 193359
        };

        enum eDatas
        {
            MaxSpell = 10
        };

        std::array<uint32, MaxSpell> const m_SpellArray =
        {
            {
                AdrenalineRush,
                Sprint,
                BetweenTheEyes,
                Vanish,
                GrapplingHook,
                CannonballBarrage,
                KillingSpree,
                MarkedForDeath,
                DeathFromAbove,
                RunThrough
            }
        };

        uint32 m_ReduceTimePerCombo;
};

/// Last Update 7.3.5 Build 26365
/// Called by Roll the Bones - 193316
class spell_rog_roll_the_bones : public SpellScriptLoader
{
    public:
        spell_rog_roll_the_bones() : SpellScriptLoader("spell_rog_roll_the_bones") { }

        class spell_rog_roll_the_bones_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_roll_the_bones_SpellScript);

            struct DiceData
            {
                DiceData(uint32 p_SpellId, uint32 p_VisualActivated, uint32 p_VisualUnactivated) :
                    spellId(p_SpellId), VisualActivated(p_VisualActivated), VisualUnactivated(p_VisualUnactivated) {}

                uint32 spellId;
                int32 VisualActivated;
                int32 VisualUnactivated;
            };

            enum eVisuals
            {
                ActivatedCannonBalls   = 56595,
                ActivatedTreasure      = 56587,
                ActivatedSwords        = 56600,
                ActivatedShark         = 56598,
                ActivatedSkullhead     = 56596,
                ActivatedCompass       = 56597,
                UnactivatedCannonBalls = 56826,
                UnactivatedTreasure    = 56825,
                UnactivatedSwords      = 56834,
                UnactivatedShark       = 56831,
                UnactivatedSkullhead   = 56827,
                UnactivatedCompass     = 56830,
            };

            enum eSpells
            {
                Broadsides          = 193356,
                BuriedTreasure      = 199600,
                JollyRoger          = 199603,
                GrandMelee          = 193358,
                SharkInfestedWaters = 193357,
                TrueBearing         = 193359,
                DeeperStratagem     = 193531,
                DeepeningShadows    = 185314,
                ShadowDance         = 185313,
                EnvelopingShadows   = 238104,
                LoadedDice          = 240837,
                AdrenalineRush      = 13750
            };

            std::array<DiceData, 6> m_DiceData =
            {
                {
                    DiceData(eSpells::Broadsides,            eVisuals::ActivatedCannonBalls, eVisuals::UnactivatedCannonBalls),
                    DiceData(eSpells::BuriedTreasure,        eVisuals::ActivatedTreasure,    eVisuals::UnactivatedTreasure),
                    DiceData(eSpells::GrandMelee,            eVisuals::ActivatedSwords,      eVisuals::UnactivatedSwords),
                    DiceData(eSpells::SharkInfestedWaters,   eVisuals::ActivatedShark,       eVisuals::UnactivatedShark),
                    DiceData(eSpells::JollyRoger,            eVisuals::ActivatedSkullhead,   eVisuals::UnactivatedSkullhead),
                    DiceData(eSpells::TrueBearing,           eVisuals::ActivatedCompass,     eVisuals::UnactivatedCompass)
                }
            };

            int32 m_PowerCost = 0;

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_PowerType == POWER_COMBO_POINT)
                {
                    int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);
                    l_MaxComboPoint = l_Caster->HasAura(eSpells::DeeperStratagem) && l_MaxComboPoint >= 6 ? 6 : 5;

                    p_PowerCost = std::min(l_Caster->GetPower(POWER_COMBO_POINT), l_MaxComboPoint);
                    m_PowerCost = p_PowerCost;

                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::TrueBearing))
                        l_Aura->SetScriptGuid(1, m_PowerCost);

                    if (l_Caster->HasAura(eSpells::DeepeningShadows))
                    {
                        if (SpellInfo const* l_DeepeningShadows = sSpellMgr->GetSpellInfo(eSpells::DeepeningShadows))
                        {
                            if (SpellInfo const* l_ShadowDance = sSpellMgr->GetSpellInfo(eSpells::ShadowDance))
                            {
                                if (Player* l_Player = l_Caster->ToPlayer())
                                {
                                    uint32 reducedtime = (m_PowerCost * l_DeepeningShadows->Effects[EFFECT_1].BasePoints);
                                    if (AuraEffect* I_AuraEffect = l_Caster->GetAuraEffect(eSpells::EnvelopingShadows, EFFECT_0))
                                        reducedtime += (I_AuraEffect->GetAmount() / 10) * m_PowerCost;
                                    l_Player->ReduceChargeCooldown(l_ShadowDance->ChargeCategoryEntry, reducedtime * IN_MILLISECONDS);
                                }
                            }
                        }
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_Duration = 0;
                std::vector<int8> l_Buffs;
                std::vector<int8> l_OldBuffs;
                int8 l_I = 0;
                for (auto l_Itr : m_DiceData)
                {
                    if (!l_Duration)
                    {
                        if (Aura* l_Aura = l_Caster->GetAura(l_Itr.spellId))
                            l_Duration = l_Aura->GetDuration();
                    }

                    if (l_Caster->HasAura(l_Itr.spellId))
                        l_OldBuffs.push_back(l_I);

                    l_Caster->RemoveAurasDueToSpell(l_Itr.spellId);
                    l_I++;
                }

                int8 l_MaxBuffs = 0;
                if (roll_chance_i(1))
                    l_MaxBuffs = 5;
                else if (roll_chance_i(20))
                    l_MaxBuffs = 2;
                else
                    l_MaxBuffs = l_Caster->HasAura(eSpells::LoadedDice) ? 2 : 1;

                for (int8 l_Itr = 0; l_Itr < l_MaxBuffs; ++l_Itr)
                {
                    int8 l_RolledSide = urand(0, 5);
                    for (;;)
                    {
                        if (std::find(l_Buffs.begin(), l_Buffs.end(), l_RolledSide) == l_Buffs.end())
                        {
                            l_Buffs.push_back(l_RolledSide);
                            break;
                        }
                        else
                            l_RolledSide = urand(0, 5);
                    }
                }

                for (int8 l_Itr = 0; l_Itr < 6; ++l_Itr)
                {
                    bool l_Activated = false;
                    if (std::find(l_Buffs.begin(), l_Buffs.end(), l_Itr) != l_Buffs.end())
                        l_Activated = true;

                    if (!l_Activated)
                    {
                        if (std::find(l_OldBuffs.begin(), l_OldBuffs.end(), l_Itr) == l_OldBuffs.end())
                            continue;
                    }

                    int32 l_Visual = l_Activated ? m_DiceData[l_Itr].VisualActivated : m_DiceData[l_Itr].VisualUnactivated;

                    l_Caster->PlayOrphanSpellVisual(l_Caster->AsVector3(), G3D::Vector3(0, 0, (M_PI / 3.0f) * l_Itr), l_Caster->AsVector3(), l_Visual, 1.0f, 0, true, 0.0f);
                }

                for (auto l_Itr : l_Buffs)
                {
                    uint32 l_SpellId = m_DiceData[l_Itr].spellId;
                    l_Caster->CastSpell(l_Caster, l_SpellId, true);
                    if (Aura* l_Aura = l_Caster->GetAura(l_SpellId))
                    {
                        uint32 l_DurationToAdd = (1 + m_PowerCost) * 6 * IN_MILLISECONDS;
                        if (l_Duration)
                        {
                            if (l_Duration >= CalculatePct(l_DurationToAdd, 30))
                                l_DurationToAdd += CalculatePct(l_DurationToAdd, 30);
                            else
                                l_DurationToAdd += l_Duration;
                        }

                        l_Aura->SetAuraTimer(l_DurationToAdd);
                    }

                    if (l_SpellId == eSpells::TrueBearing)
                        m_TrueBearingWorker.ReduceCooldowns(l_Caster->ToPlayer(), m_PowerCost);
                }

                l_Caster->RemoveAura(eSpells::LoadedDice);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_rog_roll_the_bones_SpellScript::HandlePowerCost);
                AfterCast += SpellCastFn(spell_rog_roll_the_bones_SpellScript::HandleAfterCast);
            }

        private:

            TrueBearingWorker m_TrueBearingWorker;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_roll_the_bones_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by True Bearing - 193359
class spell_rog_true_bearing : public SpellScriptLoader
{
    public:
        spell_rog_true_bearing() : SpellScriptLoader("spell_rog_true_bearing") { }

        class spell_rog_true_bearing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_true_bearing_AuraScript);

            uint8 m_ComboPointUsed = 0;
            bool m_LastCall = false;

            enum eSpells
            {
                DeathFromAbove = 152150
            };

            void OnAuraProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_ProcSpellInfo || !l_ProcSpellInfo->IsFinishingMove())
                    return;

                Spell const* l_ProcSpell = l_DamageInfo->GetSpell();
                if (!l_ProcSpell)
                    return;

                if (!m_LastCall)
                    m_ComboPointUsed = l_Caster->GetPower(POWER_COMBO_POINT);

                if (m_ComboPointUsed > 5 && !l_Player->HasAura(193531))
                {
                    m_ComboPointUsed = 5;
                }

                if (l_ProcSpellInfo->Id == eSpells::DeathFromAbove)
                {
                    m_LastCall = true;
                    return;
                }

                m_LastCall = false;

                m_TrueBearingWorker.ReduceCooldowns(l_Player, m_ComboPointUsed);
            }

            void SetGuid(uint32 p_Index, uint64 p_Value) override
            {
                if (p_Index != 1)
                    return;

                m_ComboPointUsed = p_Value;
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_rog_true_bearing_AuraScript::OnAuraProc);
            }

        private:

            TrueBearingWorker m_TrueBearingWorker;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_true_bearing_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Grapplin Hook - 195457
class spell_rog_grappling_hook : public SpellScriptLoader
{
    public:
        spell_rog_grappling_hook() : SpellScriptLoader("spell_rog_grappling_hook") { }

        class spell_rog_grappling_hook_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_grappling_hook_SpellScript);

            enum eSpells
            {
                GrapplinHookJump = 227180
            };

            void HandleJump()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::GrapplinHookJump, true);
            }

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                if (l_Caster->isInStun())
                    return SpellCastResult::SPELL_FAILED_STUNNED;

                if (l_Caster->isInRoots())
                    return SpellCastResult::SPELL_FAILED_ROOTED;

                if (l_Caster->isConfused())
                    return SpellCastResult::SPELL_FAILED_CONFUSED;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_grappling_hook_SpellScript::HandleCheckCast);
                AfterCast += SpellCastFn(spell_rog_grappling_hook_SpellScript::HandleJump);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_grappling_hook_SpellScript();
        }
};

/// Last Update 7.1.5 Build 22522
/// Called by Curse Of The Dreadblades - 202665
class spell_rog_curse_of_the_dreadblades : public SpellScriptLoader
{
    public:
        spell_rog_curse_of_the_dreadblades() : SpellScriptLoader("spell_rog_curse_of_the_dreadblades") { }

        class spell_rog_curse_of_the_dreadblades_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_curse_of_the_dreadblades_AuraScript);

            enum eSpells
            {
                DreadbladesFinishingAura    = 202669,

                SaberSlash                  = 193315,
                PistolShot                  = 185763,
                Blunderbuss                 = 202895,
                Ambush                      = 8676,
                GhostlyStrike               = 196937,
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();

                if (l_SpellInfo == nullptr)
                    return false;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::SaberSlash:
                    case eSpells::PistolShot:
                    case eSpells::Blunderbuss:
                    case eSpells::GhostlyStrike:
                    case eSpells::Ambush:
                        return true;

                    default: return false;
                }

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_curse_of_the_dreadblades_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_curse_of_the_dreadblades_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Take Your Cut - 198265
class spell_rog_take_your_cut : public SpellScriptLoader
{
    public:
        spell_rog_take_your_cut() : SpellScriptLoader("spell_rog_take_your_cut") { }

        class spell_rog_take_your_cut_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_take_your_cut_AuraScript);

            enum eSpells
            {
                TakeYourCut = 198368
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::TakeYourCut, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_take_your_cut_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_take_your_cut_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Backstab - 53
class spell_rog_backstab : public SpellScriptLoader
{
    public:
        spell_rog_backstab() : SpellScriptLoader("spell_rog_backstab") { }

        class spell_rog_backstab_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_backstab_SpellScript);

            void HandleDamage(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (!l_Target->isInFront(l_Caster))
                    SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), GetSpellInfo()->Effects[EFFECT_3].BasePoints));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_backstab_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_backstab_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Rogue T19 Subtlety 4P Bonus - 211663
class spell_rog_t19_subtlety_4p : public SpellScriptLoader
{
    public:
        spell_rog_t19_subtlety_4p() : SpellScriptLoader("spell_rog_t19_subtlety_4p") { }

        class spell_rog_t19_subtlety_4p_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_t19_subtlety_4p_AuraScript);

            enum eSpells
            {
                ShadowStrike = 185438,
                NightBlade   = 195452
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                bool l_PreventProc = true;

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (l_DamageInfo)
                {
                    Unit* l_Target = l_DamageInfo->GetTarget();
                    SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                    if (l_Target && l_SpellInfo)
                    {
                        if (l_SpellInfo->Id == eSpells::ShadowStrike &&
                            l_Target->HasAura(eSpells::NightBlade))
                        {
                            l_PreventProc = false;
                        }
                    }
                }
                if (l_PreventProc)
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_t19_subtlety_4p_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_t19_subtlety_4p_AuraScript();
        }
};

/// Last Update 7.3.5 - 25996
/// Called By Shadow Dance - 185313 - Vanish - 1856 - Stealth - 1784, 115191
class spell_rog_master_of_shadows : public SpellScriptLoader
{
    public:
        spell_rog_master_of_shadows() : SpellScriptLoader("spell_rog_master_of_shadows") { }

        class spell_rog_master_of_shadows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_master_of_shadows_SpellScript);

            enum eSpells
            {
                MasterOfShadows = 196976,
                MasterOfShadowsTrigger = 196980,
                Vanish          = 1856,
                VanishAura      = 131361,
                Stealth         = 1784,
                Stealth2        = 115191
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (!l_Caster->HasAura(eSpells::MasterOfShadows))
                    return;

                bool l_VanishAura = l_Caster->m_SpellHelper.GetBool(eSpellHelpers::MasterOfShadowsVanish);

                if ((l_SpellInfo->Id == eSpells::Vanish && (l_Caster->HasAura(eSpells::Stealth) || l_Caster->HasAura(eSpells::Stealth2))) || (l_SpellInfo->Id == eSpells::Stealth && l_VanishAura)
                    || (l_SpellInfo->Id == eSpells::Stealth2 && l_VanishAura))
                {
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::MasterOfShadowsVanish) = false;
                    return;
                }

                l_Caster->CastSpell(l_Caster, eSpells::MasterOfShadowsTrigger, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_master_of_shadows_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_master_of_shadows_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by NightBlade - 195452
class spell_rog_nightblade : public SpellScriptLoader
{
    public:
        spell_rog_nightblade() : SpellScriptLoader("spell_rog_nightblade") { }

        class spell_rog_nightblade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_nightblade_AuraScript);

            enum eSpells
            {
                NightBladeFinality = 197395,
                NigthTerrors = 206760
            };

            uint32 m_ComboPointsUsed = 0;

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !m_ComboPointsUsed)
                    return;

                p_Amount += (p_Amount * 0, 25) * (m_ComboPointsUsed - 1); // 25% per CP

                if (l_Caster->CanApplyPvPSpellModifiers() || (l_Caster->ToPlayer() && l_Caster->ToPlayer()->IsInPvPCombat()))
                    p_Amount = CalculatePct(p_Amount, 85); // 85% in pvp
            }

            void SetGuid(uint32 p_Index, uint64 p_Value) override
            {
                if (p_Index != 1)
                    return;

                m_ComboPointsUsed = p_Value;
            }

            void OnAuraProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(eSpells::NightBladeFinality))
                    l_Target->RemoveAura(eSpells::NightBladeFinality);

                l_Caster->CastSpell(l_Target, eSpells::NigthTerrors, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_nightblade_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                OnProc += AuraProcFn(spell_rog_nightblade_AuraScript::OnAuraProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_nightblade_AuraScript();
        }

        class spell_rog_nightblade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_nightblade_SpellScript);

            enum eSpells
            {
                NightBladeFinality = 197395,
                NigthTerrors = 206760
            };

            void HandlePVPModifier()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->CanApplyPvPSpellModifiers())
                    return;

                int32 l_Damage = GetHitDamage();

                l_Damage = CalculatePct(l_Damage, 85);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_nightblade_SpellScript::HandlePVPModifier);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_nightblade_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Shuriken Toss - 114014
/// Called for Night Terror - 206760
class spell_rog_nightterror_refresh : public SpellScriptLoader
{
    public:
        spell_rog_nightterror_refresh() : SpellScriptLoader("spell_rog_nightterror_refresh") { }

        class spell_rog_nightterror_refresh_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_nightterror_refresh_SpellScript);

            enum eSpells
            {
                NightTerror = 206760
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                Aura* l_Aura = l_Target->GetAura(eSpells::NightTerror);
                if (l_Aura)
                    l_Aura->RefreshDuration();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_nightterror_refresh_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_nightterror_refresh_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Shadow Techniques - 196912
class spell_rog_shadow_techniques : public SpellScriptLoader
{
    public:
        spell_rog_shadow_techniques() : SpellScriptLoader("spell_rog_shadow_techniques") { }

        class spell_rog_shadow_techniques_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shadow_techniques_AuraScript);

            uint32 m_ElapsedAttacks = 0;

            enum eSpells
            {
                FortunesBite = 197369,
                ShadowsWhisper = 242707
            };

            void OnAuraProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                ++l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ShadowTechniqueCount);
                uint32 l_Chance = 0;
                int32 l_ResourceAmount = 1;

                switch (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ShadowTechniqueCount))
                {
                    case 4:
                        l_Chance = 50;
                        break;
                    case 5:
                        l_Chance = 100;
                        break;
                    default:
                        break;
                }

                /// Fortune's Bite (Artifact Trait)
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::FortunesBite, EFFECT_0))
                    if (roll_chance_i(l_AuraEffect->GetAmount()))
                        l_ResourceAmount = 2;

                if (roll_chance_i(l_Chance))
                {
                    l_Caster->EnergizeBySpell(l_Caster, l_SpellInfo->Id, l_ResourceAmount, POWER_COMBO_POINT);
                    if (AuraEffect* l_ShadowsWhisperEffect = l_Caster->GetAuraEffect(eSpells::ShadowsWhisper, EFFECT_0))
                        l_Caster->EnergizeBySpell(l_Caster, l_SpellInfo->Id, l_ShadowsWhisperEffect->GetAmount(), POWER_ENERGY);

                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ShadowTechniqueCount) = 0;
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_rog_shadow_techniques_AuraScript::OnAuraProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shadow_techniques_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by WeaponMaster - 193537
class spell_rog_weaponmaster : public SpellScriptLoader
{
    public:
        spell_rog_weaponmaster() : SpellScriptLoader("spell_rog_weaponmaster") { }

        class spell_rog_weaponmaster_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_weaponmaster_AuraScript);

            enum eSpells
            {
                WeaponMasterTriggered = 193536
            };

            void OnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                SpellInfo const * l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_ProcSpellInfo || !l_Spell || l_Spell->IsTriggered())
                    return;

                l_Caster->CastSpell(l_Target, l_ProcSpellInfo->Id, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_weaponmaster_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL_COPY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_weaponmaster_AuraScript();
        }
};

/// Bribe - 199740
class spell_rog_bribe : public SpellScriptLoader
{
    public:
        spell_rog_bribe() : SpellScriptLoader("spell_rog_bribe") { }

        class spell_rog_bribe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_bribe_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr || l_Target->IsPlayer())
                    return SPELL_FAILED_BAD_TARGETS;

                /// Can't be used on Iron Dockworker (Blackrock Foundry)
                if (l_Target->GetEntry() == 84841)
                    return SPELL_FAILED_BAD_TARGETS;

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature)
                    return SPELL_FAILED_BAD_TARGETS;

                CreatureTemplate const* l_CreatureTemplate = l_Creature->GetCreatureTemplate();
                if (!l_CreatureTemplate)
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_CreatureTemplate->rank > CreatureEliteType::CREATURE_ELITE_NORMAL)
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_bribe_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_bribe_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Shadow Dance - 185313
class spell_rog_shadow_dance : public SpellScriptLoader
{
    public:
        spell_rog_shadow_dance() : SpellScriptLoader("spell_rog_shadow_dance") { }

        class spell_rog_shadow_dance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shadow_dance_SpellScript);

            enum eSpells
            {
                ShadowDance = 185422
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::ShadowDance))
                {
                    l_Aura->RefreshDuration();
                    return;
                }

                l_Caster->CastSpell(l_Caster, eSpells::ShadowDance, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_shadow_dance_SpellScript::HandleAfterCast);

            }
        };

        class spell_rog_shadow_dance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shadow_dance_AuraScript);

            enum eSpells
            {
                SoothingDarkness = 200759
            };

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::SoothingDarkness))
                    return;

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                /// Soothing Darkness should be at 1% healing by s in PVP
                p_Amount = 1.0f;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_shadow_dance_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_OBS_MOD_HEALTH);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shadow_dance_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shadow_dance_AuraScript();
        }
};

/// Last Update 7.0.3
/// Shadow Dance - 212035
/// Call by Parley - 199743, Blind - 2094
class spell_rog_cheap_tricks : public SpellScriptLoader
{
    public:
        spell_rog_cheap_tricks() : SpellScriptLoader("spell_rog_cheap_tricks") { }

        class spell_rog_cheap_tricks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_cheap_tricks_AuraScript);

            enum eSpells
            {
                Blind           = 2094,
                Parley          = 199743,
                CheapTricks     = 212035,
                CheapTricksMiss = 212150
            };

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::CheapTricks))
                    return;

                SpellInfo const * l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CheapTricks);
                if (!l_SpellInfo)
                    return;

                l_Caster->CastSpell(l_Caster, l_SpellInfo->Effects[EFFECT_0].TriggerSpell, true);
                l_Caster->CastSpell(l_Target, eSpells::CheapTricksMiss, true);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                case eSpells::Parley:
                    AfterEffectRemove += AuraEffectRemoveFn(spell_rog_cheap_tricks_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_PACIFY, AURA_EFFECT_HANDLE_REAL);
                    break;
                case eSpells::Blind:
                    AfterEffectRemove += AuraEffectRemoveFn(spell_rog_cheap_tricks_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                    break;
                default:
                    break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_cheap_tricks_AuraScript();
        }
};

/// Rupture - 1943
/// Updated as of 7.3.5 build 26365
class spell_rog_rupture : public SpellScriptLoader
{
    public:
        spell_rog_rupture() : SpellScriptLoader("spell_rog_rupture") { }

        class spell_rog_rupture_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_rupture_AuraScript);

            enum eSpells
            {
                DeeperStratagem = 193531,
                VenomousWounds = 79134,
                VenomousVimEnergize = 51637
            };

            void HandleAfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_ComboPoints = std::min(l_Caster->GetPower(POWER_COMBO_POINT), l_Caster->HasAura(eSpells::DeeperStratagem) ? 6 : 5);
                p_AurEff->GetBase()->SetDuration((8 + ((l_ComboPoints - 1) * 4)) * IN_MILLISECONDS);
            }

            void HandleRuptureDamage(AuraEffect const* p_AurEff, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                // int32 l_ComboPoints = std::min(l_Caster->GetPower(POWER_COMBO_POINT), l_Caster->HasAura(eSpells::DeeperStratagem) ? 6 : 5);

                /*
                       1 point  : ${$s1*8/2} over 8 sec
                       2 points: ${$s1*12/2} over 12 sec
                       3 points: ${$s1*16/2} over 16 sec
                       4 points: ${$s1*20/2} over 20 sec
                       5 points: ${$s1*24/2} over 24 sec$?s193531[
                       6 points: ${$s1*28/2} over 28 sec][]
                */

                uint32 l_AttackPower = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
                /// Calculate only bonus from AP
                p_Amount = (((l_SpellInfo->Effects[EFFECT_1].BasePoints + 0.5) * l_AttackPower));

                if (l_Caster->CanApplyPvPSpellModifiers() || (l_Caster->ToPlayer() && l_Caster->ToPlayer()->IsInPvPCombat()))
                    p_Amount = CalculatePct(p_Amount, 75); // 75% in pvp

                p_CanBeRecalculated = false;
            }

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes mode)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::VenomousWounds))
                    {
                        const uint32 energyPerSec = 5;

                        uint32 energyRegen = energyPerSec * GetDuration() / IN_MILLISECONDS;

                        l_Caster->EnergizeBySpell(l_Caster, eSpells::VenomousVimEnergize, energyRegen, POWER_ENERGY);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_rupture_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_rupture_AuraScript::HandleRuptureDamage, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_rupture_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }

            private:
                int32 m_ComboPoints = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_rupture_AuraScript();
        }

        class spell_rog_rupture_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_rupture_SpellScript);

            enum eSpells
            {
                DeeperStratagem = 193531,
            };

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_PowerType == Powers::POWER_COMBO_POINT)
                {
                    int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);

                    l_MaxComboPoint = l_Caster->HasAura(eSpells::DeeperStratagem) && l_MaxComboPoint >= 6 ? 6 : 5;
                    m_PowerCost = std::min(l_Caster->GetPower(Powers::POWER_COMBO_POINT), l_MaxComboPoint); ///< override power cost with our current
                }
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                    m_NeedPandemic = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Aura* l_Aura = l_Target->GetAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                {
                    l_Aura->SetDuration((8 + ((m_PowerCost - 1) * 4)) * IN_MILLISECONDS);

                    if (m_NeedPandemic)
                        l_Aura->SetDuration(l_Aura->GetDuration() + CalculatePct(l_Aura->GetDuration(), 30)); // Pandemic addition

                    l_Aura->SetMaxDuration(l_Aura->GetDuration());
                }
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_rog_rupture_SpellScript::HandlePowerCost);
                AfterCast += SpellCastFn(spell_rog_rupture_SpellScript::HandleAfterCast);
                OnPrepare += SpellOnPrepareFn(spell_rog_rupture_SpellScript::HandleBeforeCast);
            }

        private:
            int32 m_PowerCost;
            bool m_NeedPandemic;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_rupture_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Envenom - 32645
/// Called For Creeping Venom - 198092
class spell_rog_creeping_venom_application : public SpellScriptLoader
{
    public:
        spell_rog_creeping_venom_application() : SpellScriptLoader("spell_rog_creeping_venom_application") { }

        class spell_rog_creeping_venom_application_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_creeping_venom_application_SpellScript);

            enum eSpells
            {
                CreepingVenomPassive    = 198092,
                CreepingVenom           = 198097
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::CreepingVenomPassive))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::CreepingVenom, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_creeping_venom_application_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_creeping_venom_application_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Creeping Venom - 198097
class  spell_rog_creeping_venom : public SpellScriptLoader
{
    public:
        spell_rog_creeping_venom() : SpellScriptLoader("spell_rog_creeping_venom") {}

        class spell_rog_creeping_venom_AuraScript : public AuraScript
        {
            enum eSpells
            {
                MasterPoisoner     = 196864
            };

            PrepareAuraScript(spell_rog_creeping_venom_AuraScript);

            Position m_Position = Position();

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Position l_NewPos = Position(*l_Target);
                if (l_NewPos.GetExactDist(&m_Position))
                {
                    m_Position = l_NewPos;
                    Aura* l_Aura = p_AuraEffect->GetBase();
                    if (!l_Aura)
                        return;

                    l_Aura->RefreshTimers();
                }
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                m_Position = Position(*l_Target);
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::MasterPoisoner, EFFECT_0))
                    AddPct(p_Amount, l_AuraEffect->GetAmount());
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_creeping_venom_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_creeping_venom_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_rog_creeping_venom_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_creeping_venom_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Sprint - 2983
/// Called For Maneuverability - 197000
class spell_rog_maneuverability : public SpellScriptLoader
{
    public:
        spell_rog_maneuverability() : SpellScriptLoader("spell_rog_maneuverability") { }

        class spell_rog_maneuverability_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_maneuverability_SpellScript);

            enum eSpells
            {
                Maneuverability     = 197000,
                ManeuverabilityBuff = 197003
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::Maneuverability))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ManeuverabilityBuff, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_maneuverability_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_maneuverability_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Honor Among Thieves - 198031
class spell_rog_honor_among_thieves_ally : public SpellScriptLoader
{
    public:
        spell_rog_honor_among_thieves_ally() : SpellScriptLoader("spell_rog_honor_among_thieves_ally") { }

        class spell_rog_honor_among_thieves_ally_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_honor_among_thieves_ally_AuraScript);

            enum eSpells
            {
                HonorAmongThieves = 198032
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                if (!(p_ProcEventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                    return false;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Aura* l_Aura = l_Caster->GetAura(eSpells::HonorAmongThieves);
                if (!l_Aura)
                    return false;

                if (p_ProcEventInfo.GetActor() == l_Caster)
                    return false;

                l_Aura->SetScriptGuid(0, 0);

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_honor_among_thieves_ally_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_honor_among_thieves_ally_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Honor Among Thieves - 198032
class spell_rog_honor_among_thieves : public SpellScriptLoader
{
    public:
        spell_rog_honor_among_thieves() : SpellScriptLoader("spell_rog_honor_among_thieves") { }

        class spell_rog_honor_among_thieves_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_honor_among_thieves_AuraScript);

            uint32 m_LastProcTime = 0;

            void SetGuid(uint32 p_Index, uint64 p_GUID) override
            {
                uint32 l_Diff = GetMSTimeDiffToNow(m_LastProcTime);
                if (l_Diff < 2 * IN_MILLISECONDS)
                    return;

                m_LastProcTime += l_Diff;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->EnergizeBySpell(l_Caster, m_scriptSpellId, 1, POWER_COMBO_POINT);
            }

            void Register() override {}
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_honor_among_thieves_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Turn The Tables - 198020
class spell_rog_turn_the_tables : public SpellScriptLoader
{
    public:
        spell_rog_turn_the_tables() : SpellScriptLoader("spell_rog_turn_the_tables") { }

        class spell_rog_turn_the_tables_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_turn_the_tables_AuraScript);

            enum eSpells
            {
                TurnTheTablesPeriodic = 198023
            };

            bool HandleProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Spell const* l_ProcSpell = l_DamageInfo->GetSpell();
                Unit* l_Attacker = l_DamageInfo->GetActor();
                if (!l_ProcSpellInfo || !l_Attacker || !l_ProcSpell)
                    return false;

                DiminishingGroup l_DiminishingGroup = GetDiminishingReturnsGroupForSpell(l_ProcSpellInfo, l_Attacker);
                if (l_DiminishingGroup != DIMINISHING_STUN)
                    return false;

                DiminishingLevels l_DiminishingLevel = l_Caster->GetDiminishing(l_DiminishingGroup);
                l_DiminishingLevel = l_DiminishingLevel ? DiminishingLevels(l_DiminishingLevel - 1) : l_DiminishingLevel;
                int32 l_Duration = l_ProcSpellInfo->GetMaxDuration();
                int32 l_LimitDuration = GetDiminishingReturnsLimitDuration(l_ProcSpellInfo, l_ProcSpell->GetOriginalCaster());
                l_Caster->ApplyDiminishingToDuration(l_DiminishingGroup, l_Duration, l_ProcSpell->GetOriginalCaster(), l_DiminishingLevel, l_LimitDuration);

                if (l_Duration == 0)
                    return false;

                l_Caster->DelayedCastSpell(l_Caster, eSpells::TurnTheTablesPeriodic, true, 1);

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_turn_the_tables_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_turn_the_tables_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Turn The Tables - 198023
class spell_rog_turn_the_tables_periodic : public SpellScriptLoader
{
    public:
        spell_rog_turn_the_tables_periodic() : SpellScriptLoader("spell_rog_turn_the_tables_periodic") { }

        class spell_rog_turn_the_tables_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_turn_the_tables_periodic_AuraScript);

            enum eSpells
            {
                TurnTheTablesBuff = 198027
            };

            bool m_WasStun = false;

            void HandlePeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasUnitState(UNIT_STATE_STUNNED))
                {
                    if (!m_WasStun)
                        m_WasStun = true;

                    return;
                }
                else
                {
                    if (m_WasStun)
                        l_Caster->CastSpell(l_Caster, eSpells::TurnTheTablesBuff, true);

                    Aura* l_Aura = l_Caster->GetAura(m_scriptSpellId);
                    if (l_Aura)
                        l_Aura->SetDuration(0);

                    return;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_turn_the_tables_periodic_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_turn_the_tables_periodic_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Crimson Vial - 185311 - Created Crimson Vial - 212198
/// Called For Drink Me Up Hearties - 212210
class spell_rog_drink_me_up_hearties : public SpellScriptLoader
{
    public:
        spell_rog_drink_me_up_hearties() : SpellScriptLoader("spell_rog_drink_me_up_hearties") { }

        class spell_rog_drink_me_up_hearties_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_drink_me_up_hearties_SpellScript);

            enum eSpells
            {
                CrimsonVial         = 185311,
                CreatedCrimsonVial  = 212198
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::CrimsonVial:
                        l_Player->AddSpellCooldown(eSpells::CreatedCrimsonVial, 137222, 30 * IN_MILLISECONDS, true);
                        break;
                    case eSpells::CreatedCrimsonVial:
                        l_Player->AddSpellCooldown(eSpells::CrimsonVial, 0, 60 * IN_MILLISECONDS, true);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_drink_me_up_hearties_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_drink_me_up_hearties_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Boarding Party - 209752
class spell_rog_boarding_party : public SpellScriptLoader
{
    public:
        spell_rog_boarding_party() : SpellScriptLoader("spell_rog_boarding_party") { }

        class spell_rog_boarding_party_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_boarding_party_AuraScript);

            enum eSpells
            {
                BetweenTheEyes  = 199804,
                BoardingParty   = 209754
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo)
                    return false;

                if (l_ProcSpellInfo->Id != eSpells::BetweenTheEyes)
                    return false;

                l_Caster->DelayedCastSpell(l_Caster, eSpells::BoardingParty, true, 1);

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_boarding_party_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_boarding_party_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Shadowstep - 36554
/// Called for Silhouette - 197899
class spell_rog_intent_to_kill : public SpellScriptLoader
{
    public:
        spell_rog_intent_to_kill() : SpellScriptLoader("spell_rog_intent_to_kill") { }

        class spell_rog_intent_to_kill_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_intent_to_kill_SpellScript);

            enum eSpells
            {
                IntentToKill    = 197007,
                Vendetta        = 79140
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_IntentToKill = sSpellMgr->GetSpellInfo(eSpells::IntentToKill);
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_IntentToKill)
                    return;

                if (!l_Caster->HasAura(eSpells::IntentToKill) || !l_Target->HasAura(eSpells::Vendetta, l_Caster->GetGUID()))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellCategoryEntry const* l_SpellCategoryEntry = l_SpellInfo->ChargeCategoryEntry;
                if (!l_Player || !l_SpellCategoryEntry)
                    return;

                uint32 l_CooldownReduction = CalculatePct(l_SpellCategoryEntry->ChargeRecoveryTime, l_IntentToKill->Effects[EFFECT_0].BasePoints);
                l_Player->ReduceChargeCooldown(l_SpellCategoryEntry, l_CooldownReduction);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_intent_to_kill_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_intent_to_kill_SpellScript();
        }
};

class PlayerScript_drink_me_up_hearties : public PlayerScript
{
    public:
        PlayerScript_drink_me_up_hearties() : PlayerScript("PlayerScript_drink_me_up_hearties") { }

        enum eSpells
        {
            DrinkMeUpHearties           = 212205,
            DrinkMeUpHeartiesPassive    = 212210
        };

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID)
        {
            if (p_SpellID == eSpells::DrinkMeUpHeartiesPassive)
                p_Player->learnSpell(eSpells::DrinkMeUpHearties, false, false, false, 0, true);
        }

        void OnSpellRemoved(Player* p_Player, uint32 p_SpellID)
        {
            if (p_SpellID == eSpells::DrinkMeUpHeartiesPassive)
                p_Player->removeSpell(eSpells::DrinkMeUpHearties, false, false);
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Unfair Advantage - 206317
class spell_rog_unfair_advantage : public SpellScriptLoader
{
    public:
        spell_rog_unfair_advantage() : SpellScriptLoader("spell_rog_unfair_advantage") { }

        class spell_rog_unfair_advantage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_unfair_advantage_AuraScript);

            enum eSpells
            {
                UnfairAdvantage = 209417
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                uint32 l_HitMask = p_ProcEventInfo.GetHitMask();
                if (!l_DamageInfo || !l_Caster || !(l_HitMask & PROC_EX_CRITICAL_HIT))
                    return false;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return false;

                if (l_Target->GetHealthPct() >= l_Caster->GetHealthPct())
                    return false;

                l_Caster->DelayedCastSpell(l_Caster, eSpells::UnfairAdvantage, true, 1);

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_unfair_advantage_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_unfair_advantage_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Agonizing Poison - 200803 - Deadly Poison - 2818 - Wound Poison - 8680
/// Called For Deadly Brew - 197044
class spell_rog_deadly_brew : public SpellScriptLoader
{
    public:
        spell_rog_deadly_brew() : SpellScriptLoader("spell_rog_deadly_brew") { }

        class spell_rog_deadly_brew_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_deadly_brew_AuraScript);

            enum eSpells
            {
                AgonizingPoison = 200803,
                DeadlyPoison    = 2818,
                DeadlyBrew      = 197044,
                WoundPoison     = 8680
            };

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::DeadlyBrew))
                    return;

                int32 l_NoDamages = 0;
                l_Caster->CastCustomSpell(l_Target, eSpells::WoundPoison, &l_NoDamages, NULL, NULL, true);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::AgonizingPoison:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_rog_deadly_brew_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        break;
                    }
                    case eSpells::DeadlyPoison:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_rog_deadly_brew_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_deadly_brew_AuraScript();
        }

        class spell_rog_deadly_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_deadly_brew_SpellScript);

            enum eSpells
            {
                WoundPoison     = 8680
            };

            void HandleNoDamages(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Spell const* l_Spell = GetSpell();
                if (!l_Spell)
                    return;

                if (l_Spell->GetSpellValue(SPELLVALUE_BASE_POINT0) == -1)
                    SetHitDamage(0);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::WoundPoison)
                    OnEffectHitTarget += SpellEffectFn(spell_rog_deadly_brew_SpellScript::HandleNoDamages, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_deadly_brew_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Agonizing Poison - 200803 - Deadly Poison - 2818
/// Called For Mind-Numbing Poison - 197050
class spell_rog_mind_numbing_poison : public SpellScriptLoader
{
    public:
        spell_rog_mind_numbing_poison() : SpellScriptLoader("spell_rog_mind_numbing_poison") { }

        class spell_rog_mind_numbing_poison_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_mind_numbing_poison_AuraScript);

            enum eSpells
            {
                AgonizingPoison     = 200803,
                DeadlyPoison        = 2818,
                MindNumbingPassive  = 197050,
                MindNumbingPoison   = 197051
            };

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::MindNumbingPassive))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::MindNumbingPoison, true);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::AgonizingPoison:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_rog_mind_numbing_poison_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        break;
                    }
                    case eSpells::DeadlyPoison:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_rog_mind_numbing_poison_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_mind_numbing_poison_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Mind-Numbing Poison - 197051
class spell_rog_mind_numbing_poison_debuff : public SpellScriptLoader
{
    public:
        spell_rog_mind_numbing_poison_debuff() : SpellScriptLoader("spell_rog_mind_numbing_poison_debuff") { }

        class spell_rog_mind_numbing_poison_debuff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_mind_numbing_poison_debuff_AuraScript);

            enum eSpells
            {
                MindNumbingPoison   = 197062
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return false;

                l_Caster->DelayedCastSpell(l_Target, eSpells::MindNumbingPoison, true, 1);
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_mind_numbing_poison_debuff_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_mind_numbing_poison_debuff_AuraScript();
        }
};

/// Called by Shadowstep - 36554
/// Called for Silhouette - 197899
class spell_rog_silhouette : public SpellScriptLoader
{
    public:
        spell_rog_silhouette() : SpellScriptLoader("spell_rog_silhouette") { }

        class spell_rog_silhouette_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_silhouette_SpellScript);

            enum eSpells
            {
                Silhouette = 197899
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_Silhouette = sSpellMgr->GetSpellInfo(eSpells::Silhouette);
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_Silhouette)
                    return;

                if (!l_Caster->HasAura(eSpells::Silhouette) || !l_Target->IsFriendlyTo(l_Caster))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellCategoryEntry const* l_SpellCategoryEntry = l_SpellInfo->ChargeCategoryEntry;
                if (!l_Player || !l_SpellCategoryEntry)
                    return;

                uint32 l_CooldownReduction = CalculatePct(l_SpellCategoryEntry->ChargeRecoveryTime, l_Silhouette->Effects[EFFECT_0].BasePoints);
                l_Player->ReduceChargeCooldown(l_SpellCategoryEntry, l_CooldownReduction);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_silhouette_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_silhouette_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called By Cold Blood - 213981
class spell_rog_cold_blood : public SpellScriptLoader
{
    public:
        spell_rog_cold_blood() : SpellScriptLoader("spell_rog_cold_blood") { }

        class spell_rog_cold_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_cold_blood_AuraScript);

            enum eSpells
            {
                ShadowStrike    = 185438,
                CheapShot       = 1833,
                ColdBlood       = 213983
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_ColdBlood = sSpellMgr->GetSpellInfo(eSpells::ColdBlood);
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_DamageInfo || !l_ColdBlood || !l_SpellInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Caster = l_DamageInfo->GetActor();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (l_ProcSpellInfo == nullptr || (l_ProcSpellInfo->Id != eSpells::CheapShot && l_ProcSpellInfo->Id != eSpells::ShadowStrike) ||
                    l_Target == nullptr || l_Caster == nullptr || GetCasterGUID() != l_Caster->GetGUID())
                    return false;

                int32 l_Amount = CalculatePct(l_Target->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                l_Caster->CastCustomSpell(l_Target, eSpells::ColdBlood, &l_Amount, nullptr, nullptr, true, nullptr, nullptr, 0, 1);

                Aura* l_Aura = GetAura();
                if (l_Aura)
                    l_Aura->SetDuration(0);

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_cold_blood_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_cold_blood_AuraScript();
        }
};

/// Called By Vanish - 131361 - Stealth - 1784 - Stealth (Subterfuge) 115191 - Shadow Dance - 185422 - Cold Blood - 213981
/// Caleed For Shadow's Caress - 198665
class spell_rog_shadows_caress : public SpellScriptLoader
{
    public:
        spell_rog_shadows_caress() : SpellScriptLoader("spell_rog_shadows_caress") { }

        class spell_rog_shadows_caress_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shadows_caress_AuraScript);

            enum eSpells
            {
                Vanish              = 131361,
                Stealth             = 1784,
                SubterfugeStealth   = 115191,
                ShadowDance         = 185422,
                ColdBlood           = 213981,
                ShadowsCaress       = 198665,
                ShadowsCaressBuff   = 209427
            };

            bool l_HasAuraShadowsCaress = false;

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::ShadowsCaress))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ShadowsCaressBuff, true);
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::ShadowsCaressBuff);
            }

            /// For Cold Blood's cooldown which wasn't applied correctly
            void ColdBloodOnEffectApply(AuraEffect const* /*p_aurEff*/, AuraEffectHandleModes /*p_mode*/)
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player || !l_Player->HasAura(eSpells::ShadowsCaressBuff))
                    return;

                l_HasAuraShadowsCaress = true;
            }

            void ColdBloodOnEffectRemove(AuraEffect const* /*p_aurEff*/, AuraEffectHandleModes /*p_mode*/)
            {
                Player* l_Player = GetCaster()->ToPlayer();
                int32 l_Cooldown = (sSpellMgr->GetSpellInfo(eSpells::ColdBlood))->RecoveryTime;
                int32 l_AmountPct = (sSpellMgr->GetSpellInfo(eSpells::ShadowsCaressBuff))->Effects[EFFECT_0].BasePoints;
                if (!l_Player || !l_HasAuraShadowsCaress || !l_Cooldown || !l_AmountPct)
                    return;

                l_Player->AddSpellCooldown(eSpells::ColdBlood, CalculatePct(l_Cooldown, l_AmountPct), true);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::Vanish:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_rog_shadows_caress_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_shadows_caress_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        break;
                    }
                    case eSpells::Stealth:
                    case eSpells::SubterfugeStealth:
                    case eSpells::ShadowDance:
                    {
                        AfterEffectApply += AuraEffectApplyFn(spell_rog_shadows_caress_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_shadows_caress_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                        break;
                    }
                    case eSpells::ColdBlood:
                    {
                        OnEffectApply += AuraEffectApplyFn(spell_rog_shadows_caress_AuraScript::ColdBloodOnEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        OnEffectRemove += AuraEffectRemoveFn(spell_rog_shadows_caress_AuraScript::ColdBloodOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        break;
                    }
                    default:
                        break;
                }

            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shadows_caress_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Caleed By Dagger In The Dark - 198675
class spell_rog_dagger_in_the_dark : public SpellScriptLoader
{
    public:
        spell_rog_dagger_in_the_dark() : SpellScriptLoader("spell_rog_dagger_in_the_dark") { }

        class spell_rog_dagger_in_the_dark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_dagger_in_the_dark_AuraScript);

            enum eSpells
            {
                Vanish              = 131361,
                Stealth             = 1784,
                DaggerInTheDark     = 198688,
                SubterfugeStealth   = 115191,
                ShadowDance         = 185422
            };

            void HandleOnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::Stealth) && !l_Caster->HasAura(eSpells::Vanish) && !l_Caster->HasAura(eSpells::SubterfugeStealth) && !l_Caster->HasAura(eSpells::ShadowDance))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DaggerInTheDark);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_dagger_in_the_dark_AuraScript::HandleOnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_dagger_in_the_dark_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// SpellId 185438
class spell_rog_shadowstrike : public SpellScriptLoader
{
    public:
        spell_rog_shadowstrike() : SpellScriptLoader("spell_rog_shadowstrike") { }

        class spell_rog_shadowstrike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shadowstrike_SpellScript);

            enum eSpells
            {
                Stealth                     = 115191,
                StrikeFromTheShadows        = 196951,
                StrikeFromTheShadowsControl = 196958,
                StrikeFromTheShadowsSlow    = 222775,
                Shadowstrike                = 245623
            };

            SpellCastResult HandleCheckCast()
            {
                Unit *l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster)
                    return SPELL_FAILED_ERROR;

                if (!l_Target)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                if (l_Caster->HasAuraType(SPELL_AURA_MOD_ROOT) || l_Caster->HasAuraType(SPELL_AURA_MOD_ROOT_2))
                {
                    if (!l_Target->IsWithinMeleeRange(l_Caster))
                        return SPELL_FAILED_ROOTED;
                }

                return SPELL_CAST_OK;
            }

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Stealth))
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::StealthOnCast) = true;
                else
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::StealthOnCast) = false;
            }

            void HandleOnTeleport(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->IsWithinMeleeRange(l_Caster))
                    PreventHitDefaultEffect(EFFECT_3);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::StrikeFromTheShadows))
                {
                    if (l_Target->IsPlayer())
                        l_Caster->CastSpell(l_Target, eSpells::StrikeFromTheShadowsSlow, true);
                    else
                        l_Caster->CastSpell(l_Target, eSpells::StrikeFromTheShadowsControl, true);
                }

                /// Since Shadowstrike aura is removed 100ms after Stealth is removed (idk why, but that's how the dev scripted it, and i don't wanna mess with it) we have to check if the caster was stealthed when he casted this spell to know if we need to apply the 25% damage bonus
                if (!l_Caster->m_SpellHelper.GetBool(eSpellHelpers::StealthOnCast))
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Shadowstrike, EFFECT_1))
                {
                    int32 l_Damage = GetHitDamage();
                    AddPct(l_Damage, l_AuraEffect->GetAmount());
                    SetHitDamage(l_Damage);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_shadowstrike_SpellScript::HandleCheckCast);
                OnPrepare += SpellOnPrepareFn(spell_rog_shadowstrike_SpellScript::HandleOnPrepare);
                OnEffectHitTarget += SpellEffectFn(spell_rog_shadowstrike_SpellScript::HandleOnTeleport, EFFECT_3, SPELL_EFFECT_TELEPORT_UNITS);
                OnHit += SpellHitFn(spell_rog_shadowstrike_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shadowstrike_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Shadowstrike - 185438 and Shadowstep - 36563
class spell_rog_shadowstrike_shadowstep_tp : public SpellScriptLoader
{
    public:
        spell_rog_shadowstrike_shadowstep_tp() : SpellScriptLoader("spell_rog_shadowstrike_shadowstep_tp") { }

        class spell_rog_shadowstrike_shadowstep_tp_SpellScript : public SpellScript
        {
            enum eSpells
            {
                Shadowstep = 36563
            };

            enum eNPC
            {
                Naraxas             = 91005,
                FallenAvatar        = 116939
            };

            PrepareSpellScript(spell_rog_shadowstrike_shadowstep_tp_SpellScript);

            void HandleOnTeleport(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->IsWithinMeleeRange(l_Caster))
                    PreventHitDefaultEffect(EFFECT_3);

                Spell* l_Spell = GetSpell();
                if (!l_Spell)
                    return;

                if (WorldLocation const* l_Dst = l_Spell->GetDstTarget())
                {
                    Position l_Pos = l_Dst->GetPosition();
                    if (!l_Pos.IsPositionValid())
                        return;

                    l_Caster->UpdateAllowedPositionZ(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                    if (l_Target->GetDistance(l_Pos) > MELEE_RANGE || l_Target->GetExactDist2d(l_Pos.m_positionX, l_Pos.m_positionY) < 2.0f)
                    {
                        l_Pos.m_positionX = l_Target->GetPositionX() + ((l_Target->GetCombatReach() / 2) * cos(l_Target->GetOrientation()));
                        l_Pos.m_positionY = l_Target->GetPositionY() + ((l_Target->GetCombatReach() / 2) * sin(l_Target->GetOrientation()));
                        l_Pos.m_positionZ = l_Target->GetPositionZ();
                        l_Pos.m_orientation = l_Target->GetOrientation() - M_PI;
                        l_Caster->UpdateAllowedPositionZ(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

                        if (l_Target->ToCreature())
                        {
                            switch (l_Target->ToCreature()->GetEntry())
                            {
                                case eNPC::Naraxas:
                                    l_Pos = Position(3001.2275f, 1821.2341f, -60.033695f);
                                    break;
                                case eNPC::FallenAvatar:
                                    l_Pos = Position(6590.0039f, -795.469177f, 1663.58f);
                                    break;
                                default:
                                    break;
                            }
                        }

                        l_Pos.m_positionZ += 1.0f;

                        l_Spell->destTarget = new WorldLocation(l_Caster->GetMapId(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ, l_Pos.m_orientation);
                    }
                }
            }

            void Register() override
            {
                SpellEffIndex l_EffectIndex = EFFECT_3;
                if (m_scriptSpellId == eSpells::Shadowstep)
                    l_EffectIndex = EFFECT_0;

                OnEffectHitTarget += SpellEffectFn(spell_rog_shadowstrike_shadowstep_tp_SpellScript::HandleOnTeleport, l_EffectIndex, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shadowstrike_shadowstep_tp_SpellScript();
        }
};

/// Last Update 7.1.5 Build 22522
/// Called by Rupture - 1943 - Envenom - 32645 - Kidney Shot - 408 - Deadly Throw - 26679 - Death from Above - 152150
/// Called for Surge of Toxins - 192424
class spell_rog_surge_of_toxins : public SpellScriptLoader
{
    public:
        spell_rog_surge_of_toxins() : SpellScriptLoader("spell_rog_surge_of_toxins") { }

        class spell_rog_surge_of_toxins_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_surge_of_toxins_SpellScript);

            enum eSpells
            {
                SurgeOfToxins       = 192424,
                SurgeOfToxinsEffect = 192425,
                AgonizingPoison     = 200803,
                AgonizingPoisonBuff = 200802

            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SurgeOfToxinsEffect);

                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::SurgeOfToxins) || !l_SpellInfo)
                    return;

                int32 l_Bp = 10;

                if (!l_Caster->HasAura(eSpells::AgonizingPoisonBuff))
                    l_Bp = l_SpellInfo->Effects[EFFECT_0].BasePoints + (l_Target->GetAuraCount(eSpells::AgonizingPoison) * 10);

                l_Caster->CastCustomSpell(l_Target, eSpells::SurgeOfToxinsEffect, &l_Bp, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_surge_of_toxins_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_surge_of_toxins_SpellScript();
        }
};

/// Last Update 7.1.5 Build 22522
/// Called for Agonizing Poison - 200803
class spell_rog_surge_of_toxins_agonizing : public SpellScriptLoader
{
    public:
        spell_rog_surge_of_toxins_agonizing() : SpellScriptLoader("spell_rog_surge_of_toxins_agonizing") { }

        class spell_rog_surge_of_toxins_agonizing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_surge_of_toxins_agonizing_AuraScript);

            enum eSpells
            {
                SurgeOfToxin            = 192424,
                AgonizingPoisonDebuff   = 200803
            };

            void HandleEffectApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SurgeOfToxin);
                if (!l_Caster || !l_SpellInfo || !l_Caster->HasAura(eSpells::SurgeOfToxin) || !p_AuraEffect)
                    return;

                Aura* l_Aura = p_AuraEffect->GetBase();
                if (!l_Aura)
                    return;

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(p_AuraEffect->GetAmount() + (l_SpellInfo->Effects[EFFECT_0].BasePoints / 10) * l_Aura->GetStackAmount());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_surge_of_toxins_agonizing_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_surge_of_toxins_agonizing_AuraScript();
        }
};

/// Last Update 7.1.5 Build 22522
/// Called for Agonizing Poison - 200802
class spell_rog_agonizing_poison : public SpellScriptLoader
{
public:
    spell_rog_agonizing_poison() : SpellScriptLoader("spell_rog_agonizing_poison") { }

    class spell_rog_agonizing_poison_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_agonizing_poison_AuraScript);

        enum eSpells
        {
            Envenom             = 32645,
            AgonizingPoisonDmg  = 200803
        };

        bool HandleDoCheckProc(ProcEventInfo& /*p_EventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();

            if (!l_Caster || !l_Target || l_Target == l_Caster) /// Build 23420: Prevent Rogue from applying the poison to himself
                return false;

            if (l_Caster->HasAura(eSpells::Envenom) ? roll_chance_i(60) : roll_chance_i(30))
                return true;

            return false;
        }

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();

            if (!l_Caster || !l_Target)
                return;

            l_Caster->CastSpell(l_Target, eSpells::AgonizingPoisonDmg, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_rog_agonizing_poison_AuraScript::HandleDoCheckProc);
            OnProc += AuraProcFn(spell_rog_agonizing_poison_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_agonizing_poison_AuraScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Greed - 202820
class spell_rogue_greed_proc : public SpellScriptLoader
{
    public:
        spell_rogue_greed_proc() : SpellScriptLoader("spell_rogue_greed_proc") { }

        class spell_rogue_greed_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rogue_greed_proc_AuraScript);

            enum eSpells
            {
                RunThrough = 2098,
                GreedProc  = 202822
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_ProcSpell || l_ProcSpell->Id != eSpells::RunThrough)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::GreedProc, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rogue_greed_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rogue_greed_proc_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Greed damage & heal - 202822 (trigered by aura 202820)
class spell_rog_greed : public SpellScriptLoader
{
    public:
        spell_rog_greed() : SpellScriptLoader("spell_rog_greed") { }

        class spell_rog_greed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_greed_SpellScript);

            enum eSpells
            {
                GreedAura = 202820,
                GreedHeal = 202824
            };

            uint32 m_TargetHit = 0;

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                m_TargetHit++;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::GreedAura))
                {
                    if (Aura* l_GreedAura = l_Caster->GetAura(eSpells::GreedAura, l_Caster->GetGUID()))
                    {
                        int32 l_HealAmount = CalculatePct(l_Caster->GetMaxHealth(), l_GreedAura->GetEffect(EFFECT_0)->GetBaseAmount()) * m_TargetHit;
                        l_Caster->CastCustomSpell(l_Caster, eSpells::GreedHeal, &l_HealAmount, NULL, NULL, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_greed_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
                AfterCast += SpellCastFn(spell_rog_greed_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_greed_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Blind - 2094
class spell_rog_blind : public SpellScriptLoader
{
    public:
        spell_rog_blind() : SpellScriptLoader("spell_rog_blind") { }

        enum eSpell
        {
            SpellFromTheShadows = 192432
        };

        class spell_rog_blind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_blind_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                /// Blind removes all the DoTs from the target
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_LEECH);
                l_Target->RemoveAllAurasByType(SPELL_AURA_PERIODIC_HEALTH_FUNNEL);

                l_Target->RemoveAura(eSpell::SpellFromTheShadows);

                /// Pet should stop combat when they're blind
                if (l_Target->isPet())
                    l_Target->CombatStop(true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_blind_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_blind_SpellScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Called by Garrote - 703
/// Called for Subterfuge - 108208
class spell_rog_subterfuge_assassination : public SpellScriptLoader
{
    public:
        spell_rog_subterfuge_assassination() : SpellScriptLoader("spell_rog_subterfuge_assassination") { }

        class spell_rog_subterfuge_assassination_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_subterfuge_assassination_SpellScript);

            enum eSpells
            {
                GarroteEffect       = 703,
                SubterfugeEffect    = 115192
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::SubterfugeEffect))
                    return;

                SpellInfo const* l_Subterfuge = sSpellMgr->GetSpellInfo(eSpells::SubterfugeEffect);
                if (l_Subterfuge == nullptr)
                    return;

                if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::GarroteEffect, EFFECT_0, l_Caster->GetGUID()))
                {
                    int32 l_AmountPct = l_Subterfuge->Effects[EFFECT_1].BasePoints;
                    int32 l_HitAmount = l_AuraEffect->GetAmount();
                    l_AuraEffect->SetAmount(AddPct(l_HitAmount, l_AmountPct));
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rog_subterfuge_assassination_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_subterfuge_assassination_SpellScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Called by Garrote - 703 - Rupture - 1943
/// Called for Nightstalker - 14062
class spell_rog_nightstalker : public SpellScriptLoader
{
    public:
        spell_rog_nightstalker() : SpellScriptLoader("spell_rog_nightstalker") { }

        class spell_rog_nightstalker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_nightstalker_SpellScript);

            enum eSpells
            {
                GarroteEffect       = 703,
                RuptureEffect       = 1943,
                NightstalkerEffect  = 130493
            };

            void HandleBeforeHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::NightstalkerEffect))
                    return;

                SpellInfo const* l_Nightstalker = sSpellMgr->GetSpellInfo(eSpells::NightstalkerEffect);
                if (!l_Nightstalker)
                    return;

                int32 l_Damages = GetHitDamage();
                int32 l_AmountPct = l_Nightstalker->Effects[EFFECT_0].BasePoints;
                SetHitDamage(AddPct(l_Damages, l_AmountPct));
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::NightstalkerEffect))
                    return;

                SpellInfo const* l_Nightstalker = sSpellMgr->GetSpellInfo(eSpells::NightstalkerEffect);
                if (!l_Nightstalker)
                    return;

                /// Fix for Garrote and Rupture's damage over time effect
                switch (m_scriptSpellId)
                {
                    case eSpells::GarroteEffect:
                    {
                        if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::GarroteEffect, EFFECT_0, l_Caster->GetGUID()))
                        {
                            int32 l_AmountPct = l_Nightstalker->Effects[EFFECT_0].BasePoints;
                            int32 l_HitAmount = l_AuraEffect->GetAmount();
                            if (l_Caster->CanApplyPvPSpellModifiers())
                                l_AmountPct *= 0.50f; ///< Nightstalker effect is reduced by 50% in PvP
                            l_AuraEffect->SetAmount(AddPct(l_HitAmount, l_AmountPct));
                        }
                        break;
                    }
                    case eSpells::RuptureEffect:
                    {
                        if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::RuptureEffect, EFFECT_0, l_Caster->GetGUID()))
                        {
                            int32 l_AmountPct = l_Nightstalker->Effects[EFFECT_0].BasePoints;
                            int32 l_HitAmount = l_AuraEffect->GetAmount();
                            if (l_Caster->CanApplyPvPSpellModifiers())
                                l_AmountPct *= 0.50f; ///< Nightstalker effect is reduced by 50% in PvP
                            l_AuraEffect->SetAmount(AddPct(l_HitAmount, l_AmountPct));
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rog_nightstalker_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_nightstalker_SpellScript();
        }
};

/// Called by Shadowy Duel - 207736
class spell_rog_shadowy_duel : public SpellScriptLoader
{
    public:
        spell_rog_shadowy_duel() : SpellScriptLoader("spell_rog_shadowy_duel") { }

        class spell_rog_shadowy_duel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shadowy_duel_SpellScript);

            enum eSpells
            {
                ShadowyDuel = 210558,
                ShadowyDuelInvisible = 207756
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ShadowyDuel, true);
                l_Target->CastSpell(l_Caster, eSpells::ShadowyDuel, true);
                l_Caster->CastSpell(l_Caster, eSpells::ShadowyDuelInvisible, true);
                l_Target->CastSpell(l_Target, eSpells::ShadowyDuelInvisible, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_shadowy_duel_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shadowy_duel_SpellScript();
        }
};

/// last update 7.1.5 build 23420
/// Called by From the Shadows - 192428
class spell_rog_from_the_shadows : public SpellScriptLoader
{
    public:
        spell_rog_from_the_shadows() : SpellScriptLoader("spell_rog_from_the_shadows") { }

        class spell_rog_from_the_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_from_the_shadows_AuraScript);

            enum eSpells
            {
                Vendetta                = 79140,
                FromTheShadowsPeriodic  = 192432
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Vendetta || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FromTheShadowsPeriodic, true, 0, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_rog_from_the_shadows_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_from_the_shadows_AuraScript();
        }
};

/// last update 7.1.5 build 23420
/// Called by From the Shadows - 192432
class spell_rog_from_the_shadows_periodic : public SpellScriptLoader
{
    public:
        spell_rog_from_the_shadows_periodic() : SpellScriptLoader("spell_rog_from_the_shadows_periodic") { }

        class spell_rog_from_the_shadows_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_from_the_shadows_periodic_AuraScript);

            enum eSpells
            {
                FromTheShadowsDamage  = 192434,
                FromTheShadowsAnim = 192431
            };

            void CastSpell(Unit* p_Caster, Unit* p_Target, uint32 p_SpellId)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellId);
                if (!l_SpellInfo)
                    return;

                float l_ObjSize = p_Target->GetObjectSize();
                float l_Dist = l_SpellInfo->Effects[EFFECT_0].CalcRadius(p_Caster);

                if (l_Dist < l_ObjSize)
                    l_Dist = l_ObjSize;

                l_Dist = l_ObjSize + (l_Dist - l_ObjSize) * (float)rand_norm();

                float l_Angle = float(rand_norm())*static_cast<float>(2 * M_PI);

                Position l_Pos;
                p_Target->GetNearPosition(l_Pos, l_Dist, l_Angle);

                Creature* l_Trigger = p_Caster->SummonCreature(WORLD_TRIGGER, l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, l_SpellInfo->CalcCastTime() + 100);
                if (!l_Trigger)
                    return;

                l_Trigger->setFaction(p_Caster->getFaction());
                l_Trigger->SetGuidValue(UNIT_FIELD_SUMMONED_BY, p_Caster->GetGUID());
                l_Trigger->CastSpell(p_Target, l_SpellInfo, true, 0, nullptr, p_Caster->GetGUID());
            }

            void OnTick(const AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                CastSpell(l_Caster, l_Target, eSpells::FromTheShadowsAnim);
                l_Caster->CastSpell(l_Target, eSpells::FromTheShadowsDamage, true, 0, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_from_the_shadows_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_from_the_shadows_periodic_AuraScript();
        }
};

/// last update 7.3.5
/// Called by From the Shadows - 192434
class spell_rog_from_the_shadows_damage : public SpellScriptLoader
{
public:
    spell_rog_from_the_shadows_damage() : SpellScriptLoader("spell_rog_from_the_shadows_damage") { }

    class spell_rog_from_the_shadows_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rog_from_the_shadows_damage_SpellScript);

        enum eSpells
        {
            PotentPoisons = 76803
        };

        void ChangeDamage(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetOriginalCaster();
            if (!l_Caster)
                return;

            int32 l_Damage = GetHitDamage();
            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::PotentPoisons, EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount());
            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_rog_from_the_shadows_damage_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rog_from_the_shadows_damage_SpellScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Called By Control is King - 212219
class spell_rog_control_is_king_ennemy : public SpellScriptLoader
{
    public:
        spell_rog_control_is_king_ennemy() : SpellScriptLoader("spell_rog_control_is_king_ennemy") { }

        class spell_rog_control_is_king_ennemy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_control_is_king_ennemy_AuraScript);

            enum eSpells
            {
                ControlIsKing = 212217
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Unit* l_Target = GetTarget();
                if (!l_Target || l_Caster->IsFriendlyTo(l_Target))
                    return false;

                Aura* l_Aura = l_Caster->GetAura(eSpells::ControlIsKing);
                if (!l_Aura)
                    return false;

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                bool l_Match = false;

                /// Check global spell mechanic first
                switch (l_SpellInfo->Mechanic)
                {
                    case Mechanics::MECHANIC_SILENCE:
                    case Mechanics::MECHANIC_POLYMORPH:
                    case Mechanics::MECHANIC_STUN:
                        l_Match = true;
                        break;
                }

                /// Then check effect mechanic if needed
                if (!l_Match)
                {
                    for (uint8 l_I = 0; l_I < l_SpellInfo->EffectCount; ++l_I)
                    {
                        switch (l_SpellInfo->Effects[l_I].Mechanic)
                        {
                            case Mechanics::MECHANIC_SILENCE:
                            case Mechanics::MECHANIC_POLYMORPH:
                            case Mechanics::MECHANIC_STUN:
                                l_Match = true;
                                break;
                            default:
                                break;
                        }
                    }
                }

                if (!l_Match)
                    return false;

                l_Caster->AddDelayedEvent([l_Caster]() -> void {
                    Aura* l_Aura = l_Caster->GetAura(eSpells::ControlIsKing);
                    if (!l_Aura)
                        return;

                    l_Aura->SetScriptGuid(0, 0);
                }, 10);

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_control_is_king_ennemy_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_control_is_king_ennemy_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Control is King - 212217
class spell_rog_control_is_king : public SpellScriptLoader
{
    public:
        spell_rog_control_is_king() : SpellScriptLoader("spell_rog_control_is_king") { }

        class spell_rog_control_is_king_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_control_is_king_AuraScript);

            enum eSpells
            {
                AdrenalineRush  = 202033,
                ControlIsKing   = 212217,
                BlurredTime     = 202776
            };

            void SetGuid(uint32 p_Index, uint64 p_GUID) override
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_Duration = 0;
                if (!l_Caster->HasAura(eSpells::AdrenalineRush))
                    l_Caster->CastSpell(l_Caster, eSpells::AdrenalineRush, true);
                else
                    l_Duration = l_Caster->GetAura(eSpells::AdrenalineRush)->GetDuration();

                Aura* l_Aura = l_Caster->GetAura(eSpells::AdrenalineRush);
                if (!l_Aura)
                    return;

                SpellInfo const* l_ControlIsKing = sSpellMgr->GetSpellInfo(eSpells::ControlIsKing);
                if (l_ControlIsKing)
                {
                    l_Aura->SetDuration(l_Duration + l_ControlIsKing->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);

                    if (Aura* l_BlurredTimeAura = l_Caster->GetAura(eSpells::BlurredTime))
                    {
                        l_BlurredTimeAura->SetDuration(l_BlurredTimeAura->GetDuration() + l_ControlIsKing->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                    }
                    else
                        if (Aura* l_BlurredTimeAura = l_Caster->AddAura(eSpells::BlurredTime, l_Caster))
                            l_BlurredTimeAura->SetDuration(l_ControlIsKing->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                }
            }

            void Register() override {}
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_control_is_king_AuraScript();
        }
};

/// last update 7.1.5 build 23420
/// Called by blunderbuss - 202895
class spell_rog_blunderbuss : public SpellScriptLoader
{
    public:
        spell_rog_blunderbuss() : SpellScriptLoader("spell_rog_blunderbuss") { }

        class spell_rog_blunderbuss_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_blunderbuss_SpellScript);

            enum eSpells
            {
                BlunderbussAura                         = 202848,
                GreenskinsWaterloggedWristcuffsBonus    = 209423,
                T20Outlaw2PBonus                        = 242276,
                Headshot                                = 242277
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::T20Outlaw2PBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::Headshot, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::BlunderbussAura))
                    l_Caster->RemoveAura(eSpells::BlunderbussAura);
                if (l_Caster->HasAura(eSpells::GreenskinsWaterloggedWristcuffsBonus))
                    l_Caster->RemoveAura(eSpells::GreenskinsWaterloggedWristcuffsBonus);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rog_blunderbuss_SpellScript::HandleAfterHit);
                AfterCast += SpellCastFn(spell_rog_blunderbuss_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_blunderbuss_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Stealth - 1784 & Shadow Dance 185422 & Subterfuge 115192
/// Call Veil Of Midnight - 198952
class spell_rog_veil_of_midnight : public SpellScriptLoader
{
    public:
        spell_rog_veil_of_midnight() : SpellScriptLoader("spell_rog_veil_of_midnight") { }

        class spell_rog_veil_of_midnight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_veil_of_midnight_AuraScript);

            enum eSpells
            {
                Subterfuge          = 115192,
                Vanish              = 131361,
                Stealth             = 1784,
                VeilOfMidnight      = 198952,
                VeilOfMidnightBuff  = 199027
            };

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::VeilOfMidnight))
                    return;
                if (l_Caster->HasAura(eSpells::Subterfuge) && GetAura()->GetId() == eSpells::Stealth)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::VeilOfMidnightBuff, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_veil_of_midnight_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_veil_of_midnight_AuraScript();
        }
};

/// last update 7.3.5 build 26365
/// Called by Nightblade - 195452
class spell_rog_finality_nightblade : public SpellScriptLoader
{
    public:
        spell_rog_finality_nightblade() : SpellScriptLoader("spell_rog_finality_nightblade") { }

        class spell_rog_finality_nightblade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_finality_nightblade_SpellScript);

            enum eSpells
            {
                DeeperStratagem = 193531,
                T19Subtlety2PBonus = 211661,
                NightBladeAura  = 197498
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                    m_NeedPandemic = true;
            }

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_PowerType == Powers::POWER_COMBO_POINT)
                {
                    int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);

                    l_MaxComboPoint = l_Caster->HasAura(eSpells::DeeperStratagem) && l_MaxComboPoint >= 6 ? 6 : 5;
                    m_PowerCost = std::min(l_Caster->GetPower(Powers::POWER_COMBO_POINT), l_MaxComboPoint); ///< override power cost with our current
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

                if (Aura* l_Aura = l_Target->GetAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                {
                    l_Aura->SetDuration((8 + ((m_PowerCost - 1) * 2)) * IN_MILLISECONDS);

                    if (m_NeedPandemic)
                        l_Aura->SetDuration(l_Aura->GetDuration() + CalculatePct(l_Aura->GetDuration(), 30)); // Pandemic addition

                    l_Aura->SetMaxDuration(l_Aura->GetDuration());
                }

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Subtlety2PBonus, EFFECT_0))
                    if (Aura* l_Aura = l_Target->GetAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                        l_Aura->SetDuration(l_Aura->GetDuration() + ((l_AuraEffect->GetAmount() * IN_MILLISECONDS) * m_PowerCost));

                int32 l_DamageBonus = m_PowerCost * 4;

                if (l_Player->CanApplyPvPSpellModifiers())
                    l_DamageBonus *= 0.6f; ///< Finalty: Nightblade is reduced by 40% in PvP

                if (l_DamageBonus <= 0)
                    return;

                if (l_Caster->HasAura(eSpells::NightBladeAura))
                    l_Caster->RemoveAura(eSpells::NightBladeAura);
                else
                    l_Caster->CastCustomSpell(l_Caster, eSpells::NightBladeAura, &l_DamageBonus, NULL, NULL, true);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_rog_finality_nightblade_SpellScript::HandlePowerCost);
                AfterCast += SpellCastFn(spell_rog_finality_nightblade_SpellScript::HandleAfterCast);
                OnPrepare += SpellOnPrepareFn(spell_rog_finality_nightblade_SpellScript::HandleBeforeCast);
            }

        private:
            int32 m_PowerCost;
            bool m_NeedPandemic;
        };

        class spell_rog_finality_nightblade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_finality_nightblade_AuraScript);

            enum eSpells
            {
                NightbladeAura          = 197498,
                NigthTerrors            = 206760,

            };

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::NigthTerrors, true);
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::NightbladeAura);
                if (!l_Aura)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                int32 l_Amount = p_AuraEffect->GetAmount();
                AddPct(l_Amount, l_AuraEffect->GetAmount());

                const_cast<AuraEffect*>(p_AuraEffect)->ChangeAmount(l_Amount);

                l_Aura->SetDuration(1);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_rog_finality_nightblade_AuraScript::HandleOnProc);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_finality_nightblade_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_finality_nightblade_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_finality_nightblade_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Rupture - 1943 - Envenom - 32645 Called for Bag of Tricks - 192657
class spell_rog_bag_of_tricks : public SpellScriptLoader
{
    public:
        spell_rog_bag_of_tricks() : SpellScriptLoader("spell_rog_bag_of_tricks") { }

        class spell_rog_bag_of_tricks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_bag_of_tricks_SpellScript);

            enum eSpells
            {
                Envenom         = 32645,
                Rupture         = 1943,
                PoisonBomb      = 192661,
                BagsOfTricks    = 192657,
                DeeperStrat     = 193531
            };

            uint8 m_ComboPoints = 0;

            void HandleBeforeHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::BagsOfTricks))
                    return;

                int32 l_MaxCPUsable = l_Caster->HasAura(eSpells::DeeperStrat) ? 6 : 5;

                m_ComboPoints = std::min(l_Caster->GetPower(Powers::POWER_COMBO_POINT), l_MaxCPUsable);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo || l_Caster == l_Target)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::BagsOfTricks, SpellEffIndex::EFFECT_0);
                if (!l_AuraEffect)
                    return;

                float l_ProcChance = (l_AuraEffect->GetAmount() / 10.f * m_ComboPoints);
                if (roll_chance_f(l_ProcChance))
                    l_Caster->CastSpell(l_Target, eSpells::PoisonBomb, true);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Envenom || m_scriptSpellId == eSpells::Rupture)
                {
                    BeforeHit += SpellHitFn(spell_rog_bag_of_tricks_SpellScript::HandleBeforeHit);
                    AfterHit += SpellHitFn(spell_rog_bag_of_tricks_SpellScript::HandleAfterHit);
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_bag_of_tricks_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Energetic Stabbing - 197239
class spell_rog_energetic_stabbing : public SpellScriptLoader
{
    public:
        spell_rog_energetic_stabbing() : SpellScriptLoader("spell_rog_energetic_stabbing") { }

        class spell_rog_energetic_stabbing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_energetic_stabbing_AuraScript);

            enum eSpells
            {
                ShadowStrike    = 185438,
                Gloomblade      = 200758,
                Backstab        = 53
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                return (l_SpellInfo->Id == eSpells::ShadowStrike || l_SpellInfo->Id == eSpells::Gloomblade || l_SpellInfo->Id == eSpells::Backstab);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_energetic_stabbing_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_energetic_stabbing_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Cheap Shot - 1833
/// Called for Energetic Stabbing - 197239
class spell_rog_cheap_shot : public SpellScriptLoader
{
public:
    spell_rog_cheap_shot() : SpellScriptLoader("spell_rog_cheap_shot") { }

    class spell_rog_cheap_shot_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rog_cheap_shot_SpellScript);

        enum eSpells
        {
            EnergeticStabbing = 197239,
            EnergeticStabbingTrigger = 197237
        };

        void HandleScript(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::EnergeticStabbing, EFFECT_0))
            {
                int32 l_Bp = l_AuraEffect->GetAmount();

                l_Caster->CastCustomSpell(l_Caster, eSpells::EnergeticStabbingTrigger, &l_Bp, NULL, NULL, true);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_rog_cheap_shot_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rog_cheap_shot_SpellScript();
    }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Pistol Shot - 185763
class spell_rogue_pistol_shot : public SpellScriptLoader
{
    public:
        spell_rogue_pistol_shot() : SpellScriptLoader("spell_rogue_pistol_shot") { }

        class spell_rogue_pistol_shot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rogue_pistol_shot_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.60f;   ///< Pistol Shot Snare is reduced by 40% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rogue_pistol_shot_AuraScript::HandlePvPModifier, EFFECT_2, SPELL_AURA_MOD_DECREASE_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rogue_pistol_shot_AuraScript();
        }

        class spell_rogue_pistol_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rogue_pistol_shot_SpellScript);

            enum eSpells
            {
                GreenskinsWaterloggedWristcuffsBonus = 209423,
                Opportunity = 195627,
                T20Outlaw2PBonus = 242276,
                Headshot = 242277
            };

            void HandleOnPrepare()
            {
                if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::Opportunity))
                        m_Free = true;
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::GreenskinsWaterloggedWristcuffsBonus))
                    l_Caster->RemoveAura(eSpells::GreenskinsWaterloggedWristcuffsBonus);

                if (l_Caster->HasAura(eSpells::T20Outlaw2PBonus) && m_Free)
                    l_Caster->CastSpell(l_Caster, eSpells::Headshot, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_rogue_pistol_shot_SpellScript::HandleOnPrepare);
                AfterHit += SpellHitFn(spell_rogue_pistol_shot_SpellScript::HandleAfterHit);
            }

        private:
            bool m_Free = false;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rogue_pistol_shot_SpellScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Night Terrors - 206760
class spell_rogue_night_terrors : public SpellScriptLoader
{
    public:
        spell_rogue_night_terrors() : SpellScriptLoader("spell_rogue_night_terrors") { }

        class spell_rogue_night_terrors_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rogue_night_terrors_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.60f;   ///< Pistol Shot Snare is reduced by 40% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rogue_night_terrors_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rogue_night_terrors_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called for Duskwalker Footpads - 208895
class PlayerScript_duskwalker : public PlayerScript
{
    public:
        PlayerScript_duskwalker() : PlayerScript("PlayerScript_duskwalker") { }

        enum eSpells
        {
            Vendetta        = 79140,
            Duskwalker      = 208895
        };

        int32 m_PowerCost = 0;

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_After || p_Player->getClass() != CLASS_ROGUE || !p_Player->HasAura(eSpells::Duskwalker))
                return;

            /// Get the power earn (if > 0 ) or consum (if < 0)
            int32 l_DiffVal = p_NewValue - p_OldValue;
            if (l_DiffVal > 0)
                return;

            SpellInfo const * l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Duskwalker);
            if (!l_SpellInfo)
                return;

            m_PowerCost += -(l_DiffVal);
            if (m_PowerCost < l_SpellInfo->Effects[EFFECT_1].BasePoints)
                return;

            p_Player->ReduceSpellCooldown(eSpells::Vendetta, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            m_PowerCost -= l_SpellInfo->Effects[EFFECT_1].BasePoints;
        }
};

/// Update to Legion 7.1.5 - build 23420
/// Called by The Dreadlord's Deceit - 208692
class spell_rogue_dreadlords_deceit : public SpellScriptLoader
{
    public:
        spell_rogue_dreadlords_deceit() : SpellScriptLoader("spell_rogue_dreadlords_deceit") { }

        class spell_rogue_dreadlords_deceit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rogue_dreadlords_deceit_AuraScript);

            enum eSpells
            {
                DreadlordsDeceitAssassination   = 208693,
                DreadlordsDeceitSubtlety        = 228224
            };

            void HandleAfterEffectPediodic(AuraEffect const* /* p_AurEff */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_ROGUE_ASSASSINATION)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::DreadlordsDeceitAssassination, true);
                    l_Caster->RemoveAura(eSpells::DreadlordsDeceitSubtlety);
                }

                else if (l_Player->GetActiveSpecializationID() == SPEC_ROGUE_SUBTLETY)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::DreadlordsDeceitSubtlety, true);
                    l_Caster->RemoveAura(eSpells::DreadlordsDeceitAssassination);
                }
            }

            void HandleAfterEffectRemove(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::DreadlordsDeceitSubtlety);
                l_Caster->RemoveAura(eSpells::DreadlordsDeceitAssassination);
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_rogue_dreadlords_deceit_AuraScript::HandleAfterEffectPediodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_rogue_dreadlords_deceit_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rogue_dreadlords_deceit_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Denial of the Half-Giants - 208892.
class spell_rogue_denial_of_the_half_giants : public SpellScriptLoader
{
    public:
        spell_rogue_denial_of_the_half_giants() : SpellScriptLoader("spell_rogue_denial_of_the_half_giants") { }

        class spell_rogue_denial_of_the_half_giants_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rogue_denial_of_the_half_giants_AuraScript);

            enum eSpells
            {
               ShadowBlades = 121471,
               DeeperStratagem = 193531
            };

            void HandleOnProc(ProcEventInfo&  p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellProcInfo = GetSpellInfo();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellProcInfo)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Aura* l_Aura = l_Caster->GetAura(eSpells::ShadowBlades);
                if (!l_SpellInfo || !l_Aura)
                    return;

                if (l_SpellInfo->IsFinishingMove())
                {
                    int32 power = l_Caster->GetPower(Powers::POWER_COMBO_POINT);
                    int32 time = l_Aura->GetDuration() + l_SpellProcInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS / 10 * power;
                    l_Aura->SetDuration(time);
                }

            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_rogue_denial_of_the_half_giants_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rogue_denial_of_the_half_giants_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called by Greenskin's Waterlogged Wristcuffs - 209420
/// Called for Greenskin's Waterlogged Wristcuffs (item) - 137099
class spell_rogue_greenskins_waterlogged_wristcuffs : public SpellScriptLoader
{
    public:
        spell_rogue_greenskins_waterlogged_wristcuffs() : SpellScriptLoader("spell_rogue_greenskins_waterlogged_wristcuffs") { }

        class spell_rogue_greenskins_waterlogged_wristcuffs_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rogue_greenskins_waterlogged_wristcuffs_AuraScript);

            enum eSpells
            {
                BetweenTheEyes                          = 199804,
                GreenskinsWaterloggedWristcuffsAura     = 209420,
                GreenskinsWaterloggedWristcuffsBonus    = 209423
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::BetweenTheEyes)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GreenskinsWaterloggedWristcuffsAura);
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_Chance = l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Caster->GetPower(Powers::POWER_COMBO_POINT);
                if (roll_chance_i(l_Chance))
                    l_Caster->CastSpell(l_Caster, eSpells::GreenskinsWaterloggedWristcuffsBonus);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rogue_greenskins_waterlogged_wristcuffs_AuraScript::HandleOnCheckProc);
                AfterProc += AuraProcFn(spell_rogue_greenskins_waterlogged_wristcuffs_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rogue_greenskins_waterlogged_wristcuffs_AuraScript();
        }
};

/// Last Update: 7.3.2 Build 25549
/// Called for Shadow Satyr's Walk - 208436
/// Called by Shadow Strike -  185438
class spell_rogue_shadow_satyrs_walk : public SpellScriptLoader
{
    public:
        spell_rogue_shadow_satyrs_walk() : SpellScriptLoader("spell_rogue_shadow_satyrs_walk") { }

        class spell_rogue_shadow_satyrs_walk_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rogue_shadow_satyrs_walk_SpellScript);

            enum eSpells
            {
                ShadowSatyrsWalkPower   = 224914,
                ShadowSatyrsWalkAura    = 208436
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowSatyrsWalkPower);
                if (!l_Caster || !l_SpellInfo || !l_Target || !l_Caster->HasAura(eSpells::ShadowSatyrsWalkAura))
                    return;

                float l_Distance = l_Caster->GetDistance(l_Target);
                l_Caster->EnergizeBySpell(l_Caster, eSpells::ShadowSatyrsWalkPower, (l_SpellInfo->Effects[EFFECT_0].BasePoints + (l_Distance / 3)) * l_Caster->GetPowerCoeff(Powers::POWER_ENERGY), Powers::POWER_ENERGY);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rogue_shadow_satyrs_walk_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rogue_shadow_satyrs_walk_SpellScript();
        }
};

/// Update to Legion 7.1.5 - build 23420
/// Called by Insignia of Ravenholdt - 209041
class spell_rogue_insignia_of_ravenholdt : public SpellScriptLoader
{
    public:
        spell_rogue_insignia_of_ravenholdt() : SpellScriptLoader("spell_rogue_insignia_of_ravenholdt") { }

        class spell_rogue_insignia_of_ravenholdt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rogue_insignia_of_ravenholdt_AuraScript);

            enum eSpells
            {
                InsigniaOfRavenholdt    = 209043,
                MutilateMainHand        = 5374,
                MutilateOffHand         = 27576,
                PistolShot              = 185763,
                PoisonedKnife           = 185565,
                ShurikenToss            = 114014,
                BlunderBuss             = 202895
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_ProcSpellInfo->Id == eSpells::PoisonedKnife || l_ProcSpellInfo->Id == eSpells::ShurikenToss)
                    return false;

                return true;
            }

            void HandleAfterProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                int32 l_Damage = l_DamageInfo->GetAmount();
                Unit* l_Target = l_DamageInfo->GetTarget();
                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo || !l_Target || !l_ProcSpellInfo)
                    return;

                l_Damage = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                /// Damaging spells of mutilate are not the sames as the one generating combo points.
                switch (l_ProcSpellInfo->Id)
                {
                    case eSpells::MutilateMainHand:
                    case eSpells::MutilateOffHand:
                        l_Caster->CastCustomSpell(l_Target, eSpells::InsigniaOfRavenholdt, &l_Damage, nullptr, nullptr, true);
                        return;
                    default:
                        break;
                }

                if (!l_ProcSpellInfo->HasEffect(SPELL_EFFECT_ENERGIZE))
                    return;

                for (uint8 l_I = 0; l_I < l_ProcSpellInfo->EffectCount; l_I++)
                {
                    if (l_ProcSpellInfo->Effects[l_I].MiscValue == POWER_COMBO_POINT && l_ProcSpellInfo->Effects[l_I].IsEffect(SPELL_EFFECT_ENERGIZE))
                    {
                        l_Caster->CastCustomSpell(l_Target, eSpells::InsigniaOfRavenholdt, &l_Damage, nullptr, nullptr, true);
                        return;
                    }
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rogue_insignia_of_ravenholdt_AuraScript::HandleOnCheckProc);
                AfterProc += AuraProcFn(spell_rogue_insignia_of_ravenholdt_AuraScript::HandleAfterProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rogue_insignia_of_ravenholdt_AuraScript();
        }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Between The Eyes - 199804
class spell_rog_between_the_eyes : public SpellScriptLoader
{
    public:
        spell_rog_between_the_eyes() : SpellScriptLoader("spell_rog_between_the_eyes") { }

        class spell_rog_between_the_eyes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_between_the_eyes_SpellScript);

            enum eSpells
            {
                DeeperStratagem = 193531,
                CheapTricksAura = 212035,
                CheapTricks     = 213995
            };

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_PowerType == POWER_COMBO_POINT)
                {
                    int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);
                    l_MaxComboPoint = l_Caster->HasAura(eSpells::DeeperStratagem) && l_MaxComboPoint >= 6 ? 6 : 5;

                    p_PowerCost = std::min(l_Caster->GetPower(POWER_COMBO_POINT), l_MaxComboPoint);
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::BetweenTheEyesDuration) = p_PowerCost;
                }
            }

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_MaxComboPoint = l_Caster->GetMaxPower(Powers::POWER_COMBO_POINT);
                l_MaxComboPoint = l_Caster->HasAura(eSpells::DeeperStratagem) && l_MaxComboPoint >= 6 ? 6 : 5;

                SetHitDamage(GetHitDamage() * std::min(l_Caster->GetPower(POWER_COMBO_POINT), l_MaxComboPoint));

                if (l_Caster->HasAura(eSpells::CheapTricksAura))
                    l_Caster->CastSpell(l_Caster, eSpells::CheapTricks, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_between_the_eyes_SpellScript::HandleHit);
                OnTakePowers += SpellTakePowersFn(spell_rog_between_the_eyes_SpellScript::HandlePowerCost);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_between_the_eyes_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Between the Eyes Crit modifier aura : 235484
class spell_between_the_eyes_crit_multiplier : public SpellScriptLoader
{
    public:
        spell_between_the_eyes_crit_multiplier() : SpellScriptLoader("spell_between_the_eyes_crit_multiplier") { }

        class spell_between_the_eyes_crit_multiplier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_between_the_eyes_crit_multiplier_AuraScript);

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amout, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                p_Amout *= 0.50f; ///< Between the eyes crit bonus in pvp down to 100% from 200%: https://image.prntscr.com/image/xJu1klOnSbif-5IEB-ypAw.png
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_between_the_eyes_crit_multiplier_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_between_the_eyes_crit_multiplier_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Ghostly Strike - 196937
class spell_rog_ghostly_strike : public SpellScriptLoader
{
    public:
        spell_rog_ghostly_strike() : SpellScriptLoader("spell_rog_ghostly_strike") { }

        class spell_rog_ghostly_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_ghostly_strike_SpellScript);

            void HandleEnergize(SpellEffIndex p_Index)
            {
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target && (l_Target->GetSchoolImmunityMask() & IMMUNITY_DAMAGE))
                    PreventHitDefaultEffect(p_Index);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_ghostly_strike_SpellScript::HandleEnergize, EFFECT_0, SPELL_EFFECT_ENERGIZE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_ghostly_strike_SpellScript();
        }
};


/// Last Update 7.1.5 Build 23420
/// Called by Cheap Shot
class spell_rog_shadow_cheap_shot : public SpellScriptLoader
{
public:
    spell_rog_shadow_cheap_shot() : SpellScriptLoader("spell_rog_shadow_cheap_shot") { }

    class spell_rog_shadow_cheap_shot_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rog_shadow_cheap_shot_SpellScript);

        enum eSpells
        {
            AkaariSoul      = 209835,
            AkaariSoulNPC   = 209837
        };

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            if (l_Caster->HasAura(eSpells::AkaariSoul))
            {
                l_Caster->DelayedCastSpell(l_Target, eSpells::AkaariSoulNPC, true, 2000);
                std::map<uint64, uint64>& l_Helper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::AkaariSoul];
                uint64 l_LastUsedID = 0;
                for (std::map<uint64, uint64>::const_iterator l_Itr = l_Helper.begin(); l_Itr != l_Helper.end(); ++l_Itr)
                {
                    if (l_Itr->first > l_LastUsedID)
                        l_LastUsedID = l_Itr->first;
                }

                l_Helper[l_LastUsedID + 1] = l_Target->GetGUID();
            }

            return;
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_rog_shadow_cheap_shot_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rog_shadow_cheap_shot_SpellScript();
    }
};


/// Last Update 7.1.5 Build 23420
/// Called by Akaari's Soul - 209835
class spell_rog_akaari_soul : public SpellScriptLoader
{
    public:
        spell_rog_akaari_soul() : SpellScriptLoader("spell_rog_akaari_soul") { }

        class spell_rog_akaari_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_akaari_soul_AuraScript);

            enum eSpells
            {
                AkaariSoulNPC   = 209837,
                CheapShot       = 1833,
                ShadowStrike    = 185438
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Target || !l_ProcSpellInfo || (l_ProcSpellInfo->Id != eSpells::CheapShot && l_ProcSpellInfo->Id != eSpells::ShadowStrike))
                    return;

                l_Caster->DelayedCastSpell(l_Target, eSpells::AkaariSoulNPC, true, 2000);
                std::map<uint64, uint64>& l_Helper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::AkaariSoul];
                uint64 l_LastUsedID = 0;
                for (std::map<uint64, uint64>::const_iterator l_Itr = l_Helper.begin(); l_Itr != l_Helper.end(); ++l_Itr)
                {
                    if (l_Itr->first > l_LastUsedID)
                        l_LastUsedID = l_Itr->first;
                }

                l_Helper[l_LastUsedID + 1] = l_Target->GetGUID();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_rog_akaari_soul_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_akaari_soul_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Shadow Nova - 209781
class spell_rog_shadow_nova : public SpellScriptLoader
{
    public:
        spell_rog_shadow_nova() : SpellScriptLoader("spell_rog_shadow_nova") { }

        class spell_rog_shadow_nova_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shadow_nova_AuraScript);

            enum eSpells
            {
                ShadowStrike    = 185438,
                CheapShot       = 1833,
                ShurikenStorm   = 197835,
                ShadowDance     = 185422
            };

            std::vector<uint32> m_Spells
            {
                eSpells::ShadowStrike,
                eSpells::CheapShot,
                eSpells::ShurikenStorm
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

                if (!l_Caster->HasAura(eSpells::ShadowDance) || std::find(m_Spells.begin(), m_Spells.end(), l_SpellInfo->Id) == m_Spells.end())
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_shadow_nova_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shadow_nova_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Blurred Time - 202769
class spell_rog_blurred_time : public SpellScriptLoader
{
    public:
        spell_rog_blurred_time() : SpellScriptLoader("spell_rog_blurred_time") { }

        class spell_rog_blurred_time_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_blurred_time_AuraScript);

            enum eSpells
            {
                AdrenalineRush = 13750
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::AdrenalineRush)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_blurred_time_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_blurred_time_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Master Assassin's Initiative - 235027
class spell_rog_master_assassin : public SpellScriptLoader
{
public:
    spell_rog_master_assassin() : SpellScriptLoader("spell_rog_master_assassin") { }

    class spell_rog_master_assassin_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_master_assassin_AuraScript);

        enum eSpells
        {
            VanishStealth               = 131361,
            Stealth                     = 1784,
            SubterfugeStealth           = 115191,
            MasterAssassinsInitiative   = 235022
        };

        void OnUpdate(uint32 p_Diff)
        {
            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Aura)
                return;

            if (!l_Caster->HasAura(eSpells::MasterAssassinsInitiative))
            {
                l_Aura->Remove();
                return;
            }

            if (l_Caster->HasAura(eSpells::VanishStealth) || l_Caster->HasAura(eSpells::Stealth) || l_Caster->HasAura(eSpells::SubterfugeStealth))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MasterAssassinsInitiative);
            if (!l_SpellInfo)
                return;

            if (l_Aura->GetDuration() == -1)
            {
                l_Aura->SetAuraTimer(l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_rog_master_assassin_AuraScript::OnUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_master_assassin_AuraScript();
    }
};

/// Update to Legion 7.1.5 build 23420
/// Called by Faster Than Light Trigger - 197270
class spell_rog_flickering_shadows : public SpellScriptLoader
{
    public:
        spell_rog_flickering_shadows() : SpellScriptLoader("spell_rog_flickering_shadows") { }

        class spell_rog_flickering_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_flickering_shadows_AuraScript);

            enum eSpells
            {
                Vanish  = 1856
            };

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (GetTargetApplication() && GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                {
                    l_Owner->CastCustomSpell(eSpells::Vanish, SpellValueMod::SPELLVALUE_IGNORE_CD, 1, l_Owner, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_flickering_shadows_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_flickering_shadows_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Stealth - 1784 158188
class spell_rog_embrace_of_darkness : public SpellScriptLoader
{
    public:
        spell_rog_embrace_of_darkness() : SpellScriptLoader("spell_rog_embrace_of_darkness") { }

        class spell_rog_embrace_of_darkness_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_embrace_of_darkness_SpellScript);

            enum eSpells
            {
                EmbraceOfDarkness       = 197604,
                EmbraceOfDarknessBuff   = 197603,

                 ShadowSightDebuff       = 34709
            };

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::ShadowSightDebuff))
                        return SpellCastResult::SPELL_FAILED_CASTER_AURASTATE;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::EmbraceOfDarkness))
                    return;

                int32 l_Amount = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                l_Caster->RemoveAura(eSpells::EmbraceOfDarknessBuff);
                l_Caster->CastCustomSpell(eSpells::EmbraceOfDarknessBuff, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Amount, l_Caster, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_rog_embrace_of_darkness_SpellScript::HandleCheckCast);
                AfterCast += SpellCastFn(spell_rog_embrace_of_darkness_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_embrace_of_darkness_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Stealth - 158188
class spell_rog_stealth_heal : public SpellScriptLoader
{
    public:
        spell_rog_stealth_heal() : SpellScriptLoader("spell_rog_stealth_heal") { }

        class spell_rog_stealth_heal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_stealth_heal_AuraScript);

            enum eSpells
            {
                SoothingDarkness = 200759
            };

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::SoothingDarkness))
                    return;

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                /// Soothing Darkness should be at 1% healing by s in PVP
                p_Amount = 1.0f;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_stealth_heal_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_OBS_MOD_HEALTH);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_stealth_heal_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Tricks of the Trade - 57934
class spell_rog_thick_as_thieves : public SpellScriptLoader
{
    public:
        spell_rog_thick_as_thieves() : SpellScriptLoader("spell_rog_thick_as_thieves") { }

        class spell_rog_thick_as_thieves_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_thick_as_thieves_SpellScript);

            enum eSpells
            {
                ThickAsThieves              = 221622,
                ThickAsThievesHonorEffect   = 212155,
                ThievesGambit               = 248762
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::ThickAsThieves))
                {
                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ThievesGambit) = l_Target->GetGUID();
                    return;
                }

                l_Caster->CastSpell(l_Target, eSpells::ThickAsThievesHonorEffect, true);
                l_Caster->CastSpell(l_Caster, eSpells::ThickAsThievesHonorEffect, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_rog_thick_as_thieves_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_thick_as_thieves_SpellScript();
        }

        class spell_rog_thick_as_thieves_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_thick_as_thieves_AuraScript);

            void HandleEffectRemove(AuraEffect const* , AuraEffectHandleModes )
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ThievesGambit) = 0;
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_thick_as_thieves_AuraScript::HandleEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_thick_as_thieves_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Stealth - 158185
class spell_rog_triggerred_stealth : public SpellScriptLoader
{
    public:
        spell_rog_triggerred_stealth() : SpellScriptLoader("spell_rog_triggerred_stealth") { }

        class spell_rog_triggerred_stealth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_triggerred_stealth_AuraScript);

            enum eSpells
            {
                Stealth     = 1784,
                StealthSub  = 115191
            };

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::Stealth);
                l_Caster->RemoveAurasDueToSpell(eSpells::StealthSub);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_rog_triggerred_stealth_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_triggerred_stealth_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Ambush - 8676
class spell_rog_hidden_blade : public SpellScriptLoader
{
    public:
        spell_rog_hidden_blade() : SpellScriptLoader("spell_rog_hidden_blade") { }

        class spell_rog_hidden_blade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_hidden_blade_SpellScript);

            enum eSpells
            {
                HiddenBladeAura = 202753,
                HiddenBladeBuff = 202754
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Caster->HasAura(eSpells::HiddenBladeAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::HiddenBladeBuff, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_rog_hidden_blade_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_hidden_blade_SpellScript();
        }
};

/// Last Update to 7.3.5 build 26365
/// Called by Riposte 199754
class spell_rog_riposte : public SpellScriptLoader
{
    public:
        spell_rog_riposte() : SpellScriptLoader("spell_rog_riposte") { }

        class spell_rog_riposte_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_riposte_AuraScript);

            enum eSpells
            {
                RiposteDamages = 199753,
                Blademaster = 202628,
                BlademasterTrigger = 202631
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = l_Caster->SelectNearbyTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::RiposteDamages, true);
                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Player || !l_SpellInfo)
                    return;

                l_Player->AddSpellCooldown(eSpells::RiposteDamages, 0, l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
            }

            void HandleApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Blademaster))
                    l_Caster->CastSpell(l_Caster, eSpells::BlademasterTrigger, true);
            }

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::BlademasterTrigger);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_rog_riposte_AuraScript::HandleOnProc);
                AfterEffectApply += AuraEffectApplyFn(spell_rog_riposte_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_MOD_PARRY_PERCENT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_riposte_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_PARRY_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_riposte_AuraScript();
        }
};

/// Called by Deadly Poison - 2818
/// Deadly Poison : Instant damage - 113780
class spell_rog_deadly_poison_instant_damage : public SpellScriptLoader
{
    public:
        spell_rog_deadly_poison_instant_damage() : SpellScriptLoader("spell_rog_deadly_poison_instant_damage") { }

        class spell_rog_deadly_poison_instant_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_deadly_poison_instant_damage_SpellScript);

            enum eSpells
            {
                DeadlyPoisonDot             = 2818,
                DeadlyPoisonInstantDamage   = 113780,
                SinisterCirculation         = 238138,
                Kingsbane                   = 192759
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster || !l_Caster->IsPlayer() || GetMissInfo() != SpellMissInfo::SPELL_MISS_NONE)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::SinisterCirculation, EFFECT_0))
                {
                    uint32 l_ModifyTime = uint32(((float)l_AuraEffect->GetAmount() / 100.0f) * IN_MILLISECONDS);
                    l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::Kingsbane, l_ModifyTime);
                }

                if (l_Target->HasAura(eSpells::DeadlyPoisonDot, l_Caster->GetGUID()))
                    l_Caster->CastSpell(l_Target, eSpells::DeadlyPoisonInstantDamage, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_deadly_poison_instant_damage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_deadly_poison_instant_damage_SpellScript();
        }
};

/// Last Update to 7.3.5 build 26365
/// Called by Sprint - 2983
/// Called For Deception - 202755
class spell_rog_deception : public SpellScriptLoader
{
    public:
        spell_rog_deception() : SpellScriptLoader("spell_rog_deception") { }

        class spell_rog_deception_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_deception_SpellScript);

            enum eSpells
            {
                Feint     = 1966,
                Deception = 202755
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::Deception))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Feint, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_deception_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_deception_SpellScript();
        }
};

/// Last Update to 7.3.5 build 26365
/// Called by Wound Poison 8680 - Deadly Poison initial damage 113780 - Deadly Poison DOT 2818
/// Called For T21 Assassination 4P - 251777
class spell_rog_t21_assassination_4p : public SpellScriptLoader
{
    public:
        spell_rog_t21_assassination_4p() : SpellScriptLoader("spell_rog_t21_assassination_4p") { }

        class spell_rog_t21_assassination_4p_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_t21_assassination_4p_SpellScript);

            enum eSpells
            {
                ToxicRush           = 252284,
                T21Assassination4P  = 251777
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Spell* l_Spell = GetSpell();
                Unit* l_Target = GetHitUnit();
                if (l_Caster && l_Caster->HasAura(eSpells::T21Assassination4P) && l_Spell && l_Target && l_Spell->IsCritForTarget(l_Target))
                    l_Caster->CastSpell(l_Caster, eSpells::ToxicRush, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_t21_assassination_4p_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_t21_assassination_4p_SpellScript();
        }
};

/// Last Update Legion 7.3.2 Build 25549
/// Called by Second Shuriken - 197611
class spell_rog_second_shuriken : public SpellScriptLoader
{
    public:
        spell_rog_second_shuriken() : SpellScriptLoader("spell_rog_second_shuriken") { }

        class spell_rog_second_shuriken_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_second_shuriken_SpellScript);

            enum eSpells
            {
                ShadowDance = 185422,
                Stealth     = 1784
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || (!l_Caster->HasAura(eSpells::ShadowDance) && !l_Caster->HasAura(eSpells::ShadowDance)))
                    return;

                int32 l_Damage = GetHitDamage();
                SetHitDamage(l_Damage * 2);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_second_shuriken_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_second_shuriken_SpellScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Soul of the Shadowblade - 247509
class spell_rog_soul_of_the_shadowblade : public SpellScriptLoader
{
    public:
        spell_rog_soul_of_the_shadowblade() : SpellScriptLoader("spell_rog_soul_of_the_shadowblade") { }

        class spell_rog_soul_of_the_shadowblade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_soul_of_the_shadowblade_AuraScript);

            enum eSpells
            {
                Vigor = 14983
            };

            void HandleApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Vigor, true);
            }

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::Vigor);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_soul_of_the_shadowblade_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_soul_of_the_shadowblade_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_soul_of_the_shadowblade_AuraScript();
        }
};

/// Last Update to 7.3.2 build 25549
/// Called by Sprint - 2983
class spell_rog_sprint : public SpellScriptLoader
{
    public:
        spell_rog_sprint() : SpellScriptLoader("spell_rog_sprint") { }

        class spell_rog_sprint_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_sprint_AuraScript);

            enum eSpells
            {
                Sprint = 245751,
                SprintWaterWalk = 245756
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Sprint))
                    l_Caster->CastSpell(l_Caster, eSpells::SprintWaterWalk, true);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::SprintWaterWalk);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_sprint_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_rog_sprint_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_sprint_AuraScript();
        }
};

/// Last Update to 7.3.2 build 25549
/// Called by Symbols of Death - 212283
class spell_rog_symbols_of_death : public SpellScriptLoader
{
public:
    spell_rog_symbols_of_death() : SpellScriptLoader("spell_rog_symbols_of_death") { }

    class spell_rog_symbols_of_death_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_symbols_of_death_AuraScript);

        enum eSpells
        {
            TheFirstOfDead = 248110,
            TheFirstOfDeadTriggered = 248210
        };

        void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->HasAura(eSpells::TheFirstOfDead))
                return;

            l_Caster->CastSpell(l_Caster, eSpells::TheFirstOfDeadTriggered, true);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_rog_symbols_of_death_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_symbols_of_death_AuraScript();
    }
};

/// Update to Legion 7.1.5 build: 23420
/// Called By Item - Rogue T19 Assassination 2P Bonus 211671
class spell_rog_t19_assassination_2p_bonus: public SpellScriptLoader
{
public:
    spell_rog_t19_assassination_2p_bonus() : SpellScriptLoader("spell_rog_t19_assassination_2p_bonus") { }

    class spell_rog_t19_assassination_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_t19_assassination_2p_bonus_AuraScript);

        enum eSpells
        {
            Mutilate = 5374,
            MutilatedFlesh = 211672
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEvent)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_ProcEvent.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            SpellInfo const * l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Mutilate)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& p_ProcEvent)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_ProcEvent.GetActionTarget();
            DamageInfo* l_DamageInfo = p_ProcEvent.GetDamageInfo();
            if (!l_Caster || !l_Target || !l_DamageInfo)
                return;

            int32 l_Damage = CalculatePct(l_DamageInfo->GetAmount(), GetEffect(EFFECT_0)->GetAmount());
            l_Caster->CastCustomSpell(l_Target, eSpells::MutilatedFlesh, &l_Damage, NULL, NULL, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_rog_t19_assassination_2p_bonus_AuraScript::HandleCheckProc);
            OnProc += AuraProcFn(spell_rog_t19_assassination_2p_bonus_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_t19_assassination_2p_bonus_AuraScript();
    }
};

/// Update to Legion 7.1.5 build: 23420
/// Called By Item - Rogue T19 Outlaw 4P Bonus 211668
class spell_rog_t19_outlaw_4p_bonus : public SpellScriptLoader
{
public:
    spell_rog_t19_outlaw_4p_bonus() : SpellScriptLoader("spell_rog_t19_outlaw_4p_bonus") { }

    class spell_rog_t19_outlaw_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_t19_outlaw_4p_bonus_AuraScript);

        enum eSpells
        {
            RunThrough = 2098
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEvent)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_ProcEvent.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            SpellInfo const * l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::RunThrough)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_rog_t19_outlaw_4p_bonus_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_t19_outlaw_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.2 build 25549
/// Called by Symbols of Death - 212283
class spell_rog_symbols_of_death_bonus : public SpellScriptLoader
{
    public:
        spell_rog_symbols_of_death_bonus() : SpellScriptLoader("spell_rog_symbols_of_death_bonus") { }

        class spell_rog_symbols_of_death_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_symbols_of_death_bonus_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount -= p_Amount / 3; ///< Lash of Insanity is reduced by 33,3333..% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_symbols_of_death_bonus_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_symbols_of_death_bonus_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Catlike Reflexes - 210146
class spell_rog_catlike_reflexes : public SpellScriptLoader
{
    public:
        spell_rog_catlike_reflexes() : SpellScriptLoader("spell_rog_catlike_reflexes") { }

        class spell_rog_catlike_reflexes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_catlike_reflexes_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.25f;   ///< Catlike Reflexes is reduced by 75% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_catlike_reflexes_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DODGE_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_catlike_reflexes_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Crimson Vial - 185311
class spell_rog_crimson_vial : public SpellScriptLoader
{
    public:
        spell_rog_crimson_vial() : SpellScriptLoader("spell_rog_crimson_vial") { }

        class spell_rog_crimson_vial_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_crimson_vial_SpellScript);

            enum eSpells
            {
                DenseConcoction = 238102
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::DenseConcoction))
                    return;

                l_Caster->CastSpell(l_Caster, 240523, true);
            }

            void Register() override
            {
               OnHit += SpellHitFn(spell_rog_crimson_vial_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_crimson_vial_SpellScript();
        }
};

/// Last Update Legion 7.3.2 Build 25549
/// Called by Goremaw's Bite - 209783
/// Called for Feeding Frenzy - 238140
class spell_rog_feeding_frenzy : public SpellScriptLoader
{
    public:
        spell_rog_feeding_frenzy() : SpellScriptLoader("spell_rog_feeding_frenzy") { }

        class spell_rog_feeding_frenzy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_feeding_frenzy_SpellScript);

            enum eSpells
            {
                FeedingFrenzyAura = 238140,
                FeedingFrenzySpell = 242705
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::FeedingFrenzyAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FeedingFrenzySpell, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_feeding_frenzy_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_feeding_frenzy_SpellScript();
        }
};

/// Last Update Legion 7.3.2 Build 25549
/// Called for Feeding Frenzy - 238140
class spell_rog_feeding_frenzy_proc : public SpellScriptLoader
{
    public:
        spell_rog_feeding_frenzy_proc() : SpellScriptLoader("spell_rog_feeding_frenzy_proc") { }

        class spell_rog_feeding_frenzy_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_feeding_frenzy_proc_AuraScript);

            bool HandleDoCheckProc(ProcEventInfo& /*p_EventInfo*/)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_feeding_frenzy_proc_AuraScript::HandleDoCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_feeding_frenzy_proc_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Tricks of the Trade - 248762
/// 185311 - 31224 - 5277 - 131361
class spell_rog_thieves_gambit : public SpellScriptLoader
{
    public:
        spell_rog_thieves_gambit() : SpellScriptLoader("spell_rog_thieves_gambit") { }

        class spell_rog_thieves_gambit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_thieves_gambit_AuraScript);

            enum eSpells
            {
                CrimsonVial     = 185311,
                CloakOfShadows  = 31224,
                Evasion         = 5277,
                Vanish          = 131361
            };

            void HandleEffectApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ThievesGambit));
                if (!l_Target)
                    return;

                l_Caster->AddAura(GetId(), l_Target);
                if (Aura* l_Aura = l_Target->GetAura(GetId()))
                    l_Aura->SetDuration(l_Aura->GetDuration() / 2);
            }

            void Register() override
            {
              switch (m_scriptSpellId)
              {
                case CrimsonVial:
                    AfterEffectApply += AuraEffectApplyFn(spell_rog_thieves_gambit_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
                    break;
                case CloakOfShadows:
                    AfterEffectApply += AuraEffectApplyFn(spell_rog_thieves_gambit_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE, AURA_EFFECT_HANDLE_REAL);
                    break;
                case Evasion:
                    AfterEffectApply += AuraEffectApplyFn(spell_rog_thieves_gambit_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_DODGE_PERCENT, AURA_EFFECT_HANDLE_REAL);
                    break;
                case Vanish:
                    AfterEffectApply += AuraEffectApplyFn(spell_rog_thieves_gambit_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
                    break;
                default:
                    break;
              }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_thieves_gambit_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Second Shuriken - 197610
class spell_rog_second_shuriken_aura : public SpellScriptLoader
{
    public:
        spell_rog_second_shuriken_aura() : SpellScriptLoader("spell_rog_second_shuriken_aura") { }

        class spell_rog_second_shuriken_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_second_shuriken_AuraScript);

            enum eSpells
            {
                ShurikenStorm = 197835
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::ShurikenStorm)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_second_shuriken_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_second_shuriken_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Fate and Fortune Blood Drinking Aura - 213721
class spell_rog_fate_and_fortune_blood_drinking : public SpellScriptLoader
{
    public:
        spell_rog_fate_and_fortune_blood_drinking() : SpellScriptLoader("spell_rog_fate_and_fortune_blood_drinking") { }

        class spell_rog_fate_and_fortune_blood_drinking_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_fate_and_fortune_blood_drinking_AuraScript);

            enum eSpells
            {
                TasteOfBlood = 213738
            };

            void HandleOnEffectProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActionTarget();
                if (l_Caster == nullptr || l_Target == nullptr || l_Target->GetCreatureType() != CreatureType::CREATURE_TYPE_HUMANOID)
                    return;

                l_Target->CastSpell(l_Caster, eSpells::TasteOfBlood, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_rog_fate_and_fortune_blood_drinking_AuraScript::HandleOnEffectProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_fate_and_fortune_blood_drinking_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Adrenaline Rush - 13750
class spell_rog_adrenaline_rush: public SpellScriptLoader
{
    public:
        spell_rog_adrenaline_rush() : SpellScriptLoader("spell_rog_adrenaline_rush") { }

        class spell_rog_adrenaline_rush_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_adrenaline_rush_AuraScript);

            enum eSpells
            {
                LoadedDice     = 238139,
                LoadedDiceBuff = 240837,
                T20Outlaw4PBonus = 242278,
                LesserAdrenalineRush = 246558
            };

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::LoadedDice))
                    l_Caster->CastSpell(l_Caster, eSpells::LoadedDiceBuff, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !IsExpired())
                    return;

                if (l_Caster->HasAura(eSpells::T20Outlaw4PBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::LesserAdrenalineRush, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_adrenaline_rush_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_adrenaline_rush_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_adrenaline_rush_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Shadowstrike - 245623
class spell_rog_shadowstrike_buff : public SpellScriptLoader
{
    public:
        spell_rog_shadowstrike_buff() : SpellScriptLoader("spell_rog_shadowstrike_buff") { }

        class spell_rog_shadowstrike_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shadowstrike_buff_AuraScript);

            void HandleEffect(AuraEffect const* /* l_AuraEffect */, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                p_CanBeRecalculated = true;
                p_Amount = 25;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_shadowstrike_buff_AuraScript::HandleEffect, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_shadowstrike_buff_AuraScript::HandleEffect, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shadowstrike_buff_AuraScript();
        }
};

/// Last Update 7.3.5
/// Distract - 1725
class spell_rog_distract : public SpellScriptLoader
{
    public:
        spell_rog_distract() : SpellScriptLoader("spell_rog_distract") { }

        class spell_rog_distract_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_distract_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        std::list<Aura*> l_AurasToRemove;
                        for (Unit::AuraApplicationMap::const_iterator l_Iter = l_Target->GetAppliedAuras().begin(); l_Iter != l_Target->GetAppliedAuras().end(); ++l_Iter)
                        {
                            Aura* l_Aura = l_Iter->second->GetBase();
                            if (!l_Aura)
                                continue;

                            SpellInfo const* l_SpellInfo = l_Aura->GetSpellInfo();
                            if (l_SpellInfo->GetSpellSpecific() == SpellSpecificType::SpellSpecificFoodAndDrink ||
                                l_SpellInfo->GetSpellSpecific() == SpellSpecificType::SpellSpecificFood ||
                                l_SpellInfo->GetSpellSpecific() == SpellSpecificType::SpellSpecificDrink)
                                l_AurasToRemove.push_back(l_Aura);
                        }

                        for (Aura* l_Aura : l_AurasToRemove)
                            l_Aura->Remove();

                        if (l_AurasToRemove.size())
                            l_Target->SetStandState(UNIT_STAND_STATE_STAND);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_distract_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DISTRACT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_distract_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Called by Eviscerate - 196819
class spell_rog_finality_eviscerate_drop : public SpellScriptLoader
{
    public:
        spell_rog_finality_eviscerate_drop() : SpellScriptLoader("spell_rog_finality_eviscerate_drop") { }

        class spell_rog_finality_eviscerate_drop_SpellScript : public SpellScript
        {
            enum eSpells
            {
                EviscerateAura  = 197496,
                Finality        = 197406
            };

            PrepareSpellScript(spell_rog_finality_eviscerate_drop_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                int32 l_DamageBonus = l_Player->m_SpellHelper.GetUint32(eSpellHelpers::LastFinishingMoveCP) * 4;

                if (l_Player->CanApplyPvPSpellModifiers())
                    l_DamageBonus *= 0.6f; ///< Finalty: Eviscerate is reduced by 40% in PvP

                if (l_DamageBonus <= 0)
                    return;

                ///< Finality proc should only happen if you didnt have it already. in essence, only 1 cast out of 2 will benefit (1 puts aura, 2 consumes, 3 puts aura, 4 consumes etc)
                if (l_Caster->HasAura(eSpells::EviscerateAura))
                    l_Caster->RemoveAura(eSpells::EviscerateAura);

                else if (l_Caster->HasAura(eSpells::Finality))
                    l_Caster->CastCustomSpell(l_Caster, eSpells::EviscerateAura, &l_DamageBonus, NULL, NULL, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_finality_eviscerate_drop_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_finality_eviscerate_drop_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by The Empty Crown - 248106
class spell_rog_the_empty_crown : public SpellScriptLoader
{
public:
    spell_rog_the_empty_crown() : SpellScriptLoader("spell_rog_the_empty_crown") { }

    class spell_rog_the_empty_crown_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_the_empty_crown_AuraScript);

        enum eSpells
        {
            Kingsbane = 192759
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo)
                return false;

            if (l_SpellInfo->Id != eSpells::Kingsbane)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_rog_the_empty_crown_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_the_empty_crown_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Toxic Blade - 245388
/// Called for Steal Fate - 14190
class spell_rog_toxic_blade_for_seal_fate : public SpellScriptLoader
{
    public:
        spell_rog_toxic_blade_for_seal_fate() : SpellScriptLoader("spell_rog_toxic_blade_for_seal_fate") { }

        class spell_rog_toxic_blade_for_seal_fate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_toxic_blade_for_seal_fate_SpellScript);

            enum eSpells
            {
                SealFateAura    = 14190,
                SealFate        = 14189
            };

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::SealFateAura))
                    return;

                bool l_Crit = false;
                if (!GetSpell())
                    return;

                for (auto l_TargetInfo : GetSpell()->GetTargetInfos())
                {
                    if (l_TargetInfo.crit)
                        l_Crit = true;
                }

                if (!l_Crit)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SealFate, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_toxic_blade_for_seal_fate_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_toxic_blade_for_seal_fate_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Sap - 6770
/// Called For Mage's invisibility - 66
class spell_rog_sap_break_invisibility : public SpellScriptLoader
{
    public:
        spell_rog_sap_break_invisibility() : SpellScriptLoader("spell_rog_sap_break_invisibility") { }

        class spell_rog_sap_break_invisibility_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_sap_break_invisibility_AuraScript);

            enum eSpells
            {
                InvisibilityJoining = 66
            };

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(eSpells::InvisibilityJoining))
                    l_Target->RemoveAura(eSpells::InvisibilityJoining);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_rog_sap_break_invisibility_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_sap_break_invisibility_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Shadow Dance - Stealth - 1784, 115191
class spell_rog_shroud_of_concealment: public SpellScriptLoader
{
    public:
        spell_rog_shroud_of_concealment() : SpellScriptLoader("spell_rog_shroud_of_concealment") { }

        class spell_rog_shroud_of_concealment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shroud_of_concealment_AuraScript);

            enum eSpells
            {
                ShroudOfConcealment = 114018,
                ShadowDance         = 185422
            };

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->HasAura(eSpells::ShroudOfConcealment))
                    GetCaster()->RemoveAura(eSpells::ShroudOfConcealment);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_shroud_of_concealment_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shroud_of_concealment_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Subterfuge - 115192
class spell_rog_shadow_dance_vs_subterfuge : public SpellScriptLoader
{
    public:
        spell_rog_shadow_dance_vs_subterfuge() : SpellScriptLoader("spell_rog_shadow_dance_vs_subterfuge") { }

        class spell_rog_shadow_dance_vs_subterfuge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_shadow_dance_vs_subterfuge_AuraScript);

            enum eSpells
            {
                ShadowDanceBuff     = 185422
            };

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::ShadowDanceBuff))
                {
                    uint32 l_RemainingDuration = l_Aura->GetDuration() - 10;
                    l_Aura->Remove();

                    l_Caster->AddDelayedEvent([l_Caster, l_RemainingDuration]() -> void
                    {
                        if (l_Caster && l_RemainingDuration > 0)
                            if (Aura* l_NewAura = l_Caster->AddAura(eSpells::ShadowDanceBuff, l_Caster))
                                l_NewAura->SetDuration(l_RemainingDuration);
                    }, 10);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_rog_shadow_dance_vs_subterfuge_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_shadow_dance_vs_subterfuge_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Plunder Armor - 198529
class spell_rog_plunder_armor : public SpellScriptLoader
{
public:
    spell_rog_plunder_armor() : SpellScriptLoader("spell_rog_plunder_armor") { }

    class spell_rog_plunder_armor_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_plunder_armor_AuraScript);

        enum eSpells
        {
            PlunderArmorTransmo = 208535
        };

        void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();
            if (!l_Caster || !l_Target)
                return;

            l_Target->CastSpell(l_Caster, eSpells::PlunderArmorTransmo, true);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_rog_plunder_armor_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_plunder_armor_AuraScript();
    }

    class spell_rog_plunder_armor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rog_plunder_armor_SpellScript);

        enum eSpells
        {
            PlunderArmorTransmo = 208535
        };

        SpellCastResult HandleCheckCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Caster || !l_Target)
                return SPELL_FAILED_DONT_REPORT;

            if (!l_Target->IsPlayer())
                return SPELL_FAILED_TARGET_NOT_PLAYER;

            return SPELL_CAST_OK;

        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_rog_plunder_armor_SpellScript::HandleCheckCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rog_plunder_armor_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Item - Rogue T21 Assassination 2P Bonus - 251776
class spell_rog_t21_assas_2p_bonus : public SpellScriptLoader
{
    public:
        spell_rog_t21_assas_2p_bonus() : SpellScriptLoader("spell_rog_t21_assas_2p_bonus") { }

        class spell_rog_t21_assas_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_t21_assas_2p_bonus_AuraScript);

            enum eSpells
            {
                Envenom   = 32645
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Envenom)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_rog_t21_assas_2p_bonus_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_t21_assas_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// For Item - Rogue T21 Subtlety 2P Bonus - 251785
class PlayerScript_T21Subtlety2P : public PlayerScript
{
    public:
        PlayerScript_T21Subtlety2P() : PlayerScript("PlayerScript_T21Subtlety2P") { }

        enum eSpells
        {
            T21Subtlety2P   = 251785,
            SymbolsOfDeath  = 212283
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_COMBO_POINT || p_OldValue <= p_NewValue || p_Regen || !p_After)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Subtlety2P);
            if (!l_SpellInfo)
                return;

            SpellInfo const* l_SODSpellInfo = sSpellMgr->GetSpellInfo(eSpells::SymbolsOfDeath);
            if (!l_SODSpellInfo)
                return;

            if (!p_Player->HasAura(eSpells::T21Subtlety2P) || !l_SpellInfo)
                return;

            uint32 l_CpSpent = p_OldValue - p_NewValue;

            p_Player->ReduceChargeCooldown(l_SODSpellInfo->ChargeCategoryEntry, 0.1f * l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS * l_CpSpent);
        }
};

/// Last Update 7.3.5 build 26365
/// Item - Rogue T21 Subtlety 4P Bonus - 251788
class spell_rog_t21_subtlety_4p_bonus : public SpellScriptLoader
{
public:
    spell_rog_t21_subtlety_4p_bonus() : SpellScriptLoader("spell_rog_t21_subtlety_4p_bonus") { }

    class spell_rog_t21_subtlety_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_t21_subtlety_4p_bonus_AuraScript);

        enum eSpells
        {
            Backstab = 53,
            Shadowstrike = 185438
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::Backstab && l_SpellInfo->Id != Shadowstrike))
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_rog_t21_subtlety_4p_bonus_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_t21_subtlety_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Shadow Gestures - 257945
class PlayerScript_rog_shadow_gestures : public PlayerScript
{
public:
    PlayerScript_rog_shadow_gestures() : PlayerScript("PlayerScript_rog_shadow_gestures") { }

    enum eSpells
    {
        ShadowGestures = 257945,
        ShadowGesturesEnergize = 252732
    };

    void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
    {
        if (!p_Player || p_Power != POWER_COMBO_POINT || p_OldValue <= p_NewValue || p_Regen || !p_After)
            return;

        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowGestures);
        if (!l_SpellInfo)
            return;

        SpellInfo const* l_SODSpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowGestures);
        if (!l_SODSpellInfo)
            return;

        if (!p_Player->HasAura(eSpells::ShadowGestures) || !l_SpellInfo)
            return;

        int32 l_CpSpent = p_OldValue - p_NewValue;

        p_Player->RemoveAura(eSpells::ShadowGestures);
        p_Player->CastCustomSpell(p_Player, eSpells::ShadowGesturesEnergize, &l_CpSpent, NULL, NULL, true);
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Run Through - 2098
class spell_rog_t21_outlaw_4p_bonus : public SpellScriptLoader
{
    public:
        spell_rog_t21_outlaw_4p_bonus() : SpellScriptLoader("spell_rog_t21_outlaw_4p_bonus") { }

        class spell_rog_t21_outlaw_4p_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_t21_outlaw_4p_bonus_SpellScript);

            enum eSpells
            {
                T21Outlaw4P         = 251783,
                SharpenedSabers     = 252285, ///< T21 2P outlaw buff

                Broadsides          = 193356,
                BuriedTreasure      = 199600,
                JollyRoger          = 199603,
                GrandMelee          = 193358,
                TrueBearing         = 193359,
                SharkInfestedWaters = 193357
            };

            std::vector<uint32> m_Buffs =
            {
                eSpells::Broadsides ,
                eSpells::BuriedTreasure,
                eSpells::JollyRoger,
                eSpells::GrandMelee,
                eSpells::TrueBearing,
                eSpells::SharkInfestedWaters
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::SharpenedSabers);
                if (!l_Caster->HasAura(eSpells::T21Outlaw4P))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Outlaw4P);
                if (!l_SpellInfo)
                    return;

                if (!roll_chance_i(l_SpellInfo->Effects[EFFECT_1].BasePoints))
                    return;

                std::vector<uint32> l_SelectedBuffs;
                for (uint32 l_Buff : m_Buffs)
                {
                    if (!l_Caster->HasAura(l_Buff))
                        l_SelectedBuffs.push_back(l_Buff);
                }

                if (l_SelectedBuffs.empty())
                    return;

                uint32 l_SpellId = l_SelectedBuffs[urand(0, l_SelectedBuffs.size() - 1)];

                if (Aura* l_Buff = l_Caster->AddAura(l_SpellId, l_Caster))
                {
                    l_Buff->SetMaxDuration(l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
                    l_Buff->SetDuration(l_Buff->GetMaxDuration());
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_rog_t21_outlaw_4p_bonus_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_t21_outlaw_4p_bonus_SpellScript();
        }
};

/// Update 7.3.5 Build 26365
/// Called by Cannonball Barrage - 185779
/// Called for Shellshocked - 185778
class spell_rog_shellshocked : public SpellScriptLoader
{
    public:
        spell_rog_shellshocked() : SpellScriptLoader("spell_rog_shellshocked") { }

        class spell_rog_shellshocked_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shellshocked_SpellScript);

            enum eSpells
            {
                CannonballBarrage   = 185767,
                Shellshocked        = 185778
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Shellshocked, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rog_shellshocked_SpellScript::HandleDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shellshocked_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Deadly Poison - 2823, 129410
/// Crippiling Poison - 3408
/// Wound Poison - 8679, 129412
/// Agonizing Poison - 200802
class spell_rog_poisons_poisoned_knife : public SpellScriptLoader
{
    public:
        spell_rog_poisons_poisoned_knife() : SpellScriptLoader("spell_rog_poisons_poisoned_knife")
        {}

        enum eSpells
        {
            PoisonedKnife  = 185565,
        };

        class spell_rog_poisons_poisoned_knife_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_poisons_poisoned_knife_AuraScript);

            void HandleProcChance(float & p_Chance, ProcEventInfo & p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr || p_Proc.GetDamageInfo()->GetSpellInfo() == nullptr)
                    return;

                if (p_Proc.GetDamageInfo()->GetSpellInfo()->Id == PoisonedKnife)
                    p_Chance = 100.f;
            }

            void Register() override
            {
                DoCalcProcChance += AuraProcChanceFn(spell_rog_poisons_poisoned_knife_AuraScript::HandleProcChance);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_poisons_poisoned_knife_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_rogue_spell_scripts()
{
    new spell_rog_thuggee();
    new spell_rog_deception();
    new spell_rog_hidden_blade();
    new spell_rog_thick_as_thieves();
    new spell_rog_cheap_tricks();
    new spell_rog_bribe();
    new spell_rog_main_gauche();
    new spell_rog_dagger_bonus();
    new spell_rog_shadow_reflection_proc();
    new spell_rog_shadow_reflection();
    new spell_rog_deadly_throw();
    new spell_rog_combo_point_delayed();
    new spell_rog_burst_of_speed();
    new spell_rog_killing_spree();
    new spell_rog_killing_spree_teleport();
    new spell_rog_marked_for_death();
    new spell_rog_cloak_and_dagger();
    new spell_rog_cheat_death();
    new spell_rog_cloak_of_shadows();
    new spell_rog_combat_readiness();
    new spell_rog_nerve_strike();
    new spell_rog_cut_to_the_chase();
    new spell_rog_shadowstep();
    new spell_rog_stealth();
    new spell_rog_vanish();
    new spell_rog_feint();
    new spell_rog_item_t17_assassination_2p_bonus();
    new spell_rog_ruthlessness_and_relentless_strikes();
    new spell_rog_blind();
    new spell_rog_nightstalker();
    new spell_rog_triggerred_stealth();
    new spell_rog_soul_of_the_shadowblade();
    new spell_rog_symbols_of_death();
    new spell_rog_cheap_shot();
    new spell_rog_distract();
    new spell_rog_adrenaline_rush();
    new spell_rog_t21_subtlety_4p_bonus();
    new PlayerScript_rog_shadow_gestures();
    new spell_druid_nightstalker_pvp_mod();

    /// Generic
    new spell_rog_internal_bleeding();
    new spell_rog_finishing_moves();
    new spell_rog_shuriken_storm();
    new spell_rog_death_from_above();
    new spell_rog_death_from_above_leap();
    new spell_rog_death_from_above_return();
    new PlayerScript_Death_From_Above();
    new spell_rog_death_from_above_bonus();
    new spell_rog_sap_break_invisibility();
    new spell_rog_shroud_of_concealment();

    /// Assasination
    new spell_rog_poisons_poisoned_knife();
    new spell_rog_fan_of_knives();
    new spell_rog_venomous_wounds_proc();
    new spell_rog_garrote();
    new spell_rog_exsanguinate();
    new spell_rog_elaborate_planning();
    new spell_rog_kingsbane();
    new spell_rog_kingsbane_damage();
    new spell_rog_venom_rush();
    new spell_rog_blood_of_the_assassinated();
    new spell_rog_envenom();
    new spell_rog_rupture();
    new spell_rog_creeping_venom_application();
    new spell_rog_creeping_venom();
    new spell_rog_subterfuge_assassination();
    new spell_rog_deadly_poison_instant_damage();
    new spell_rog_t19_assassination_2p_bonus();
    new spell_rog_agonizing_poison();
    new spell_rog_crimson_vial();
    new spell_rog_toxic_blade_for_seal_fate();
    new spell_rog_t21_assas_2p_bonus();
    new spell_rog_t21_assassination_4p();

    /// Outlaw
    new spell_rog_ghostly_strike();
    new spell_rog_blade_flurry();
    new spell_rog_blade_flurry_damage();
    new spell_rog_between_the_eyes();
    new spell_rog_roll_the_bones();
    new spell_rog_true_bearing();
    new spell_rog_saber_slash();
    new spell_rog_grappling_hook();
    new spell_rog_curse_of_the_dreadblades();
    new spell_rog_take_your_cut();
    new spell_rogue_pistol_shot();
    new spell_rogue_greenskins_waterlogged_wristcuffs();
    new spell_rog_riposte();
    new spell_rog_t19_outlaw_4p_bonus();
    new spell_rog_restless_blades();
    new spell_rog_slice_and_dice();
    new spell_between_the_eyes_crit_multiplier();
    new spell_rog_t21_outlaw_4p_bonus();
    new spell_rog_shellshocked();
    new spell_rog_ruthlessness();

    /// Subtlety
    new spell_rog_shadow_dance();
    new spell_rog_shadowstrike();
    new spell_rog_backstab();
    new spell_rog_t19_subtlety_4p();
    new spell_rog_master_of_shadows();
    new spell_rog_nightblade();
    new spell_rog_nightterror_refresh();
    new spell_rog_shadow_techniques();
    new spell_rog_weaponmaster();
    new spell_rog_veil_of_midnight();
    new spell_rogue_night_terrors();
    new spell_rog_stealth_heal();
    new spell_rog_shadow_cheap_shot();
    new spell_rog_second_shuriken();
    new spell_rog_sprint();
    new spell_rog_relentless_strikes();
    new spell_rog_feeding_frenzy();
    new spell_rog_feeding_frenzy_proc();
    new spell_rog_shadowstrike_buff();
    new spell_rog_finality_eviscerate_drop();
    new spell_rog_shadow_dance_vs_subterfuge();
    new spell_rog_shadowstrike_shadowstep_tp();

    /// Honor talents
    new spell_rog_maneuverability();
    new spell_rog_honor_among_thieves_ally();
    new spell_rog_honor_among_thieves();
    new spell_rog_turn_the_tables();
    new spell_rog_turn_the_tables_periodic();
    new PlayerScript_drink_me_up_hearties();
    new spell_rog_drink_me_up_hearties();
    new spell_rog_cold_blood();
    new spell_rog_boarding_party();
    new spell_rog_intent_to_kill();
    new PlayerScript_drink_me_up_hearties();
    new spell_rog_unfair_advantage();
    new spell_rog_deadly_brew();
    new spell_rog_mind_numbing_poison();
    new spell_rog_mind_numbing_poison_debuff();
    new spell_rog_silhouette();
    new spell_rog_shadows_caress();
    new spell_rog_dagger_in_the_dark();
    new spell_rog_shadowy_duel();
    new spell_rog_control_is_king();
    new spell_rog_control_is_king_ennemy();
    new spell_rog_thieves_gambit();
    new spell_rog_plunder_armor();

    /// Player Scripts
    new PlayerScript_ruthlessness();
    new PlayerScript_stealth();
    new PlayerScript_T21Subtlety2P();

    /// Artifact Traits
    new spell_rogue_greed_proc();
    new spell_rog_greed();
    new spell_rog_surge_of_toxins();
    new spell_rog_surge_of_toxins_agonizing();
    new spell_rog_from_the_shadows();
    new spell_rog_from_the_shadows_periodic();
    new spell_rog_from_the_shadows_damage();
    new spell_rog_blunderbuss();
    new spell_rog_finality_nightblade();
    new spell_rog_bag_of_tricks();
    new spell_rog_energetic_stabbing();
    new spell_rog_akaari_soul();
    new spell_rog_shadow_nova();
    new spell_rog_blurred_time();
    new spell_rog_flickering_shadows();
    new spell_rog_embrace_of_darkness();
    new spell_rog_master_assassin();
    new spell_rog_second_shuriken_aura();
    new spell_rog_fate_and_fortune_blood_drinking();

    ///Legendary Items
    new PlayerScript_duskwalker();
    new spell_rogue_shadow_satyrs_walk();
    new spell_rogue_dreadlords_deceit();
    new spell_rogue_denial_of_the_half_giants();
    new spell_rogue_insignia_of_ravenholdt();
    new spell_rog_the_empty_crown();

    /// PvP Modifiers
    new spell_rog_symbols_of_death_bonus();
    new spell_rog_catlike_reflexes();
    new spell_rog_shuriken_combo_pvp();
}
#endif
