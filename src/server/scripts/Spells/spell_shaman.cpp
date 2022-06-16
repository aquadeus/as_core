////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 */

#include "ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiersImpl.h"
#include "GridNotifiers.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "HelperDefines.h"

/// Ascendance (Flame) - 114050
/// last update: 7.3.5. 26124
class spell_sha_ascendance_flame : public SpellScriptLoader
{
    public:
        spell_sha_ascendance_flame() : SpellScriptLoader("spell_sha_ascendance_flame") { }

        class spell_sha_ascendance_flame_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ascendance_flame_AuraScript);

            enum eSpells
            {
                LavaBurst = 51505
            };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (SpellInfo const* l_LavaBurst = sSpellMgr->GetSpellInfo(eSpells::LavaBurst))
                    l_Player->ResetCharges(l_LavaBurst->ChargeCategoryEntry);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sha_ascendance_flame_AuraScript::AfterApply, EFFECT_2, SPELL_AURA_CHARGE_RECOVERY_MOD, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_ascendance_flame_AuraScript();
        }
};

/// Ascendance (Air) - 114051
/// last update: 7.3.5 26124
class spell_sha_ascendance_air : public SpellScriptLoader
{
    public:
        spell_sha_ascendance_air() : SpellScriptLoader("spell_sha_ascendance_air") { }

        class spell_sha_ascendance_air_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ascendance_air_AuraScript);

            enum eSpells
            {
                Stormstrike = 17364,
                Windstrike  = 115356
            };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (SpellInfo const* l_Stormstrike = sSpellMgr->GetSpellInfo(eSpells::Stormstrike))
                    l_Player->RestoreCharge(l_Stormstrike->ChargeCategoryEntry);

                if (SpellInfo const* l_Windstrike = sSpellMgr->GetSpellInfo(eSpells::Windstrike))
                    l_Player->RestoreCharge(l_Windstrike->ChargeCategoryEntry);

                l_Player->RemoveSpellCooldown(eSpells::Stormstrike, true);
                l_Player->RemoveSpellCooldown(eSpells::Windstrike, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sha_ascendance_air_AuraScript::AfterApply, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_ascendance_air_AuraScript();
        }
};

/// Ascendance (Water) - 114052
class spell_sha_ascendance_water : public SpellScriptLoader
{
    public:
        spell_sha_ascendance_water() : SpellScriptLoader("spell_sha_ascendance_water") { }

        class spell_sha_ascendance_water_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ascendance_water_AuraScript);

            enum eSpells
            {
                RestorativeMists = 114083,
                BeaconOfFaithHeal = 177174,

                /// Prevent some others class spell to infinite proc loop
                PaladinBeaconOfLightHeal = 53652,
                PaladinBeaconOfLightMana = 88852,
                PaladinBeaconOfFaithHeal = 177174,
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                if (p_EventInfo.GetActor()->GetGUID() != l_Player->GetGUID())
                    return;

                SpellInfo const* l_ProcSpell = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_ProcSpell)
                    return;

                switch (l_ProcSpell->Id)
                {
                    case PaladinBeaconOfLightHeal:
                    case PaladinBeaconOfLightMana:
                    case PaladinBeaconOfFaithHeal:
                    case RestorativeMists:
                        return;
                    default:
                        break;
                }

                if (!(p_EventInfo.GetDamageInfo()->GetAmount()))
                    return;

                if (!(l_ProcSpell->IsPositive()))
                    return;

                if (Unit* l_Target = p_EventInfo.GetActionTarget())
                {
                    int32 l_Bp = p_EventInfo.GetDamageInfo()->GetAmount();

                    if (l_Bp > 0)
                        l_Player->CastCustomSpell(l_Target, eSpells::RestorativeMists, &l_Bp, nullptr, nullptr, true); ///< Restorative Mists
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_ascendance_water_AuraScript::OnProc, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_ascendance_water_AuraScript();
        }
};

/// Ascendance (Water)(heal) - 114083
class spell_sha_ascendance_water_heal : public SpellScriptLoader
{
    public:
        spell_sha_ascendance_water_heal() : SpellScriptLoader("spell_sha_ascendance_water_heal") { }

        class spell_sha_ascendance_water_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_ascendance_water_heal_SpellScript);

            uint32 m_TargetSize = 1;

            void OnEffectHeal(SpellEffIndex)
            {
                SetHitHeal(GetHitHeal() / m_TargetSize);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                m_TargetSize = p_Targets.size();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_ascendance_water_heal_SpellScript::OnEffectHeal, EFFECT_0, SPELL_EFFECT_HEAL);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_ascendance_water_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_ascendance_water_heal_SpellScript();
        }
};

/// 108285 - Call of the Elements
class spell_sha_call_of_the_elements: public SpellScriptLoader
{
    public:
        spell_sha_call_of_the_elements() : SpellScriptLoader("spell_sha_call_of_the_elements") { }

        class spell_sha_call_of_the_elements_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_call_of_the_elements_SpellScript);

            enum eDatas
            {
                MaxTotem = 5
            };

            std::array<uint32, eDatas::MaxTotem> m_resetTotemCdSpells =
            {
                {
                    2484,   ///< Earthbind Totem
                    5394,   ///< Healing Stream Totem
                    108270, ///< Stone Bulwark Totem
                    51485,  ///< Earthgrab Totem
                    108273  ///< Windwalk Totem
                }
            };

            void OnSpellHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (!l_Player)
                    return;

                for (int l_I = 0; l_I < MaxTotem; l_I++)
                    l_Player->RemoveSpellCooldown(m_resetTotemCdSpells[l_I], true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_sha_call_of_the_elements_SpellScript::OnSpellHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_call_of_the_elements_SpellScript();
        }
};

/// Last Update 7.3.5
/// Ancestral Guidance - 108281
class spell_sha_ancestral_guidance: public SpellScriptLoader
{
    public:
        spell_sha_ancestral_guidance() : SpellScriptLoader("spell_sha_ancestral_guidance") { }

        class spell_sha_ancestral_guidance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ancestral_guidance_AuraScript);

            enum eSpells
            {
                AncestalGuidance = 114911,
                RestorativeMists = 114083
            };

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                // Prevent inifinite loop
                if (eventInfo.GetDamageInfo()->GetSpellInfo() &&
                    (eventInfo.GetDamageInfo()->GetSpellInfo()->Id == GetSpellInfo()->Id ||
                    eventInfo.GetDamageInfo()->GetSpellInfo()->Id == eSpells::RestorativeMists ||
                    eventInfo.GetDamageInfo()->GetSpellInfo()->Id == eSpells::AncestalGuidance))
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (l_Player == nullptr)
                    return;

                if (Unit* l_Target = eventInfo.GetActionTarget())
                {
                    int32 l_Bp = eventInfo.GetDamageInfo()->GetAmount();

                    if (!l_Bp)
                        return;

                    l_Bp = CalculatePct(l_Bp, aurEff->GetAmount());

                    l_Player->CastCustomSpell(l_Target, eSpells::AncestalGuidance, &l_Bp, NULL, NULL, true);
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_guidance_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_ancestral_guidance_AuraScript();
        }
};

/// Last update 7.3.5
/// Ancestral Guidance (heal) - 114911
class spell_sha_ancestral_guidance_heal : public SpellScriptLoader
{
    public:
        spell_sha_ancestral_guidance_heal() : SpellScriptLoader("spell_sha_ancestral_guidance_heal") { }

        class spell_sha_ancestral_guidance_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_ancestral_guidance_heal_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() > 3)
                {
                    p_Targets.sort(JadeCore::HealthPctOrderPred());
                    p_Targets.resize(3);
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_ancestral_guidance_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_ancestral_guidance_heal_SpellScript();
        }
};

/// 64695 - Earthgrab
class spell_sha_earthgrab: public SpellScriptLoader
{
    public:
        spell_sha_earthgrab() : SpellScriptLoader("spell_sha_earthgrab") { }

        enum eSpells
        {
            EarthgrabImmunity = 116946,
            Earthbind = 116947,
            Earthgrab = 64695
        };

        class spell_sha_earthgrab_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earthgrab_SpellScript);

            void HitTarget(SpellEffIndex)
            {
                Unit* l_Target = GetHitUnit();

                if (!l_Target)
                    return;

                GetCaster()->CastSpell(l_Target, eSpells::EarthgrabImmunity);
                l_Target->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                l_Target->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
            }

            void FilterTargets(std::list<WorldObject*>& p_UnitList)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_UnitList.remove_if([l_Caster](WorldObject* p_Object) -> bool
                {
                    if (!l_Caster)
                        return true;

                    Unit* l_Target = p_Object->ToUnit();
                    if (!l_Target)
                        return true;

                    if (l_Target->HasAura(eSpells::Earthgrab))
                        return true;

                    if (!l_Caster->IsWithinLOSInMap(l_Target)) ///< check done in Spell::SelectImplicitAreaTargets but needs to be done again here Q.Q PIGPIGPIG
                        return true;

                    if (l_Target->HasAura(eSpells::EarthgrabImmunity))
                    {
                        l_Caster->CastSpell(l_Target, eSpells::Earthbind, true);
                        return true;
                    }

                    return false;
                });
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_earthgrab_SpellScript::HitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_earthgrab_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_earthgrab_SpellScript();
        }

        class spell_sha_earthgrab_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthgrab_AuraScript);

            void HandleDispel(DispelInfo* /*p_DispelInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EarthgrabImmunity);
                l_Target->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                l_Target->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
            }

            void Register() override
            {
                AfterDispel += AuraDispelFn(spell_sha_earthgrab_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_earthgrab_AuraScript();
        }
};

/// Earthgrab - 8377
class spell_sha_earthgrab_totem_root : public SpellScriptLoader
{
public:
    spell_sha_earthgrab_totem_root() : SpellScriptLoader("spell_sha_earthgrab_totem_root") { }

    class spell_sha_earthgrab_totem_root_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_earthgrab_totem_root_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_UnitList)
        {
            Unit* l_Caster = GetOriginalCaster();

            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();

            if (!l_Owner)
                return;

            Player* l_Player = l_Owner->ToPlayer();

            if (!l_Player)
                return;

            for (auto l_It = p_UnitList.begin(); l_It != p_UnitList.end();)
            {
                Unit* l_UnitTarget = (*l_It)->ToUnit();

                if (!l_UnitTarget || !l_UnitTarget->IsValidAttackTarget(l_Owner))
                {
                    l_It = p_UnitList.erase(l_It);
                    continue;
                }

                ++l_It;
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_earthgrab_totem_root_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_sha_earthgrab_totem_root_SpellScript();
    }
};

/// last update : 6.1.2 19802
/// Stone Bulwark Totem - 114889
class spell_sha_stone_bulwark: public SpellScriptLoader
{
    public:
        spell_sha_stone_bulwark() : SpellScriptLoader("spell_sha_stone_bulwark") { }

        class spell_sha_stone_bulwark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_stone_bulwark_AuraScript);

            enum eSpells
            {
                StoneBulwark = 114893
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Owner = l_Caster->GetOwner();

                if (l_Caster == nullptr || l_Owner == nullptr)
                    return;

                Player* l_Player = l_Owner->ToPlayer();

                if (l_Player == nullptr)
                    return;

                float spellPower = spellPower = l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                int32 l_Amount = 0.875f * spellPower;

                if (Aura* aura = l_Player->GetAura(eSpells::StoneBulwark))
                {
                    aura->GetEffect(EFFECT_0)->SetAmount(aura->GetEffect(EFFECT_0)->GetAmount() + l_Amount);
                    aura->RefreshDuration();
                }
                else if (p_AurEff->GetTickNumber() == 1)
                {
                    l_Amount *= 4.0f;
                    if (Aura* aura = l_Caster->AddAura(eSpells::StoneBulwark, l_Player))
                        aura->GetEffect(EFFECT_0)->SetAmount(l_Amount);
                }
                else
                {
                    if (Aura* aura = l_Caster->AddAura(eSpells::StoneBulwark, l_Player))
                        aura->GetEffect(EFFECT_0)->SetAmount(l_Amount);
                }

            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_stone_bulwark_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_stone_bulwark_AuraScript();
        }
};

/// Spirit Link - 98020 : triggered by 98017
/// Spirit Link Totem
class spell_sha_spirit_link: public SpellScriptLoader
{
    public:
        spell_sha_spirit_link() : SpellScriptLoader("spell_sha_spirit_link") { }

        class spell_sha_spirit_link_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_spirit_link_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetEntry() == 53006)
                    {
                        if (l_Caster->GetOwner())
                        {
                            if (Player* _player = l_Caster->GetOwner()->ToPlayer())
                            {
                                std::list<Unit*> l_MemberList;
                                _player->GetRaidMembers(l_MemberList);

                                float l_Radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(l_Caster, GetSpell());

                                l_MemberList.remove_if([l_Caster, l_Radius](Unit const* p_Unit) -> bool
                                {
                                    if (p_Unit->GetDistance(*l_Caster) > l_Radius)
                                        return true;

                                    return false;
                                });

                                float l_TotalRaidHealthPct = 0;

                                for (auto l_Itr : l_MemberList)
                                    l_TotalRaidHealthPct += l_Itr->GetHealthPct();

                                l_TotalRaidHealthPct /= l_MemberList.size() * 100.0f;

                                for (auto l_Itr : l_MemberList)
                                    l_Itr->SetHealth(uint32(l_TotalRaidHealthPct * l_Itr->GetMaxHealth()));
                            }
                        }
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_sha_spirit_link_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_spirit_link_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Healing Stream - 52042
class spell_sha_healing_stream: public SpellScriptLoader
{
    public:
        spell_sha_healing_stream() : SpellScriptLoader("spell_sha_healing_stream") { }

        class spell_sha_healing_stream_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_healing_stream_SpellScript);

            enum eSpells
            {
                RushingStreams = 147074,
                QueensDecreeAura = 207360,
                QueensDecreeProc = 208899
            };

            class QueenDecreeOrderPred
            {
            public:
                bool operator() (const WorldObject* a, const WorldObject* b) const
                {
                    if (!a->isType(TYPEMASK_UNIT) || !b->isType(TYPEMASK_UNIT))
                        return false;

                    double rA = a->ToUnit()->GetMaxHealth() ? double(a->ToUnit()->GetHealth()) / double(a->ToUnit()->GetMaxHealth()) : 0.0;
                    double rB = b->ToUnit()->GetMaxHealth() ? double(b->ToUnit()->GetHealth()) / double(b->ToUnit()->GetMaxHealth()) : 0.0;

                    if (!a->ToUnit()->HasAura(eSpells::QueensDecreeProc) && b->ToUnit()->HasAura(eSpells::QueensDecreeProc))
                        return true;
                    else if (!b->ToUnit()->HasAura(eSpells::QueensDecreeProc) && a->ToUnit()->HasAura(eSpells::QueensDecreeProc))
                        return false;

                    return rA < rB;
                }
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();

                if (Unit* l_Owner = l_Caster->GetOwner())
                    l_Caster = l_Owner;

                if (p_Targets.size() > 1)
                {
                    if (l_Caster->HasAura(eSpells::QueensDecreeAura))
                        p_Targets.sort(QueenDecreeOrderPred());
                    else
                        p_Targets.sort(JadeCore::HealthPctOrderPred());

                    if (l_Caster->HasAura(eSpells::RushingStreams)) ///< Your Healing Stream Totem now heals two targets
                        p_Targets.resize(2);
                    else
                        p_Targets.resize(1);
                }
            }

            void HandleHit()
            {
                Unit* l_Totem = GetCaster();
                Unit* l_Caster = nullptr;
                Unit* l_Target = GetHitUnit();

                if (l_Target && l_Totem && (l_Caster = l_Totem->GetOwner()))
                {
                    if (l_Caster->HasAura(eSpells::QueensDecreeAura))
                    {
                        int32 l_Heal = l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL) * 0.24f;

                        l_Caster->CastCustomSpell(l_Target, eSpells::QueensDecreeProc, &l_Heal, nullptr, nullptr, true);
                    }
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_healing_stream_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
                OnHit += SpellHitFn(spell_sha_healing_stream_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_healing_stream_SpellScript();
        }
};

/// Earthquake - 61882
/// last update : 7.3.5 26365
class spell_sha_earthquake: public SpellScriptLoader
{
    public:
        spell_sha_earthquake() : SpellScriptLoader("spell_sha_earthquake") { }

        class spell_sha_earthquake_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earthquake_SpellScript);

            enum eSpells
            {
                Earthquake                    = 61882,
                EchoesOfTheGreatSunderingBuff = 208723,
                EarthenStrength               = 252141
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Bonus = 0;
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::EarthenStrength, EFFECT_0))
                {
                    l_Bonus = l_AuraEffect->GetAmount();
                    l_Caster->RemoveAura(eSpells::EarthenStrength);
                }

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::EarthenStrength) = l_Bonus;

                std::list<AreaTrigger*> l_AreaTriggerList;
                l_Caster->GetAreaTriggerList(l_AreaTriggerList, eSpells::Earthquake);
                if (l_AreaTriggerList.empty())
                    return;

                if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::EarthShockPowerSpentAndEarthquakes) == true)
                {
                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::EarthShockPowerSpentAndEarthquakes) = l_AreaTriggerList.back()->GetGUID();
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::EarthShockPowerSpentAndEarthquakes) = false;
                    l_Caster->RemoveAura(eSpells::EchoesOfTheGreatSunderingBuff);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_earthquake_SpellScript::HandleAfterCast);
            }
        };

        class spell_sha_earthquake_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthquake_AuraScript);

            enum eSpells
            {
                Earthquake             = 61882,
                EarthquakeTick         = 77478
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<AreaTrigger*> l_AreaTriggerList;
                l_Caster->GetAreaTriggerList(l_AreaTriggerList, eSpells::Earthquake);

                for (AreaTrigger* l_AreaTrigger : l_AreaTriggerList)
                {
                    /// Dealing ${$SPN * 0.92 * 6 * (1 + $170374m3 / 100)} Physical damage over $d
                    int32 l_Bp0 = float(l_Caster->SpellBaseDamageBonusDone(SpellSchoolMask::SPELL_SCHOOL_MASK_SPELL)) * 0.92f * 6.0f;

                    l_Bp0 /= GetSpellInfo()->GetDuration() / TimeConstants::IN_MILLISECONDS;

                    if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::EarthenStrength) != 0)
                        AddPct(l_Bp0, l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::EarthenStrength));

                    l_Caster->CastCustomSpell(l_AreaTrigger->GetPositionX(), l_AreaTrigger->GetPositionY(), l_AreaTrigger->GetPositionZ(), eSpells::EarthquakeTick, &l_Bp0, nullptr, nullptr, true, nullptr, nullptr, 0, l_AreaTrigger->GetGUID());
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthquake_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_earthquake_AuraScript();
        }

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_earthquake_SpellScript();
        }
};

/// Earthquake : Ticks - 77478
/// last update : 7.1.5 Build 23420
class spell_sha_earthquake_tick: public SpellScriptLoader
{
    public:
        spell_sha_earthquake_tick() : SpellScriptLoader("spell_sha_earthquake_tick") { }

        class spell_sha_earthquake_tick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earthquake_tick_SpellScript);

            enum eSpells
            {
                EarthquakeSlow                = 182387,
                EarthquakeKnockDown           = 77505,
                EchoesOfTheGreatSunderingBuff = 208723,
                Earthquake                    = 61882
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Spell* l_Spell = GetSpell();
                if (!l_Caster || !l_Target || !l_Spell)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EarthquakeSlow, true);

                /// 10% chance of knocking down affected targets
                if (roll_chance_i(GetSpellInfo()->Effects[EFFECT_1].BasePoints))
                    l_Caster->CastSpell(l_Target, eSpells::EarthquakeKnockDown, true);

                if (l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::EarthShockPowerSpentAndEarthquakes) == l_Spell->m_AreaTriggerGUID)
                {
                    int32 l_Amount = GetHitDamage();
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EchoesOfTheGreatSunderingBuff);
                    if (!l_Amount || !l_SpellInfo)
                        return;

                    int32 l_NewAmount = CalculatePct(l_Amount, 100.0f + l_SpellInfo->Effects[EFFECT_1].BasePoints);
                    SetHitDamage(l_NewAmount);
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_sha_earthquake_tick_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_earthquake_tick_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Rainfall (enhancement) - 215871
class spell_sha_rainfall_heal : public SpellScriptLoader
{
    public:
        spell_sha_rainfall_heal() : SpellScriptLoader("spell_sha_rainfall_heal") { }

        class spell_sha_rainfall_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_rainfall_heal_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                /// Healing up to 6 allies
                if (p_Targets.size() > 6)
                    JadeCore::RandomResizeList(p_Targets, 6);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_rainfall_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_rainfall_heal_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Bloodlust - 2825
class spell_sha_bloodlust : public SpellScriptLoader
{
    public:
        spell_sha_bloodlust() : SpellScriptLoader("spell_sha_bloodlust") { }

        class spell_sha_bloodlust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_bloodlust_SpellScript);

            enum eSpells
            {
                Sated                   = 57724,
                Exhaustion              = 57723,
                Insanity                = 95809,
                TemporalDisplacement    = 80354,
                Fatigued                = 160455
            };

            void ApplyDebuff()
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    if (GetSpellInfo() && l_Target->HasAura(GetSpellInfo()->Id))
                        l_Target->CastSpell(l_Target, eSpells::Sated,  true);
                }
            }

            void HandleImmunity(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eSpells::Exhaustion) || l_Target->HasAura(eSpells::Insanity) ||
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
                AfterHit += SpellHitFn(spell_sha_bloodlust_SpellScript::ApplyDebuff);
                OnEffectHitTarget += SpellEffectFn(spell_sha_bloodlust_SpellScript::HandleImmunity, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                OnEffectHitTarget += SpellEffectFn(spell_sha_bloodlust_SpellScript::HandleImmunity, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_bloodlust_SpellScript();
        }
};

/// Heroism - 32182 - last update: 6.1.2 19802
class spell_sha_heroism: public SpellScriptLoader
{
    public:
        spell_sha_heroism() : SpellScriptLoader("spell_sha_heroism") { }

        class spell_sha_heroism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_heroism_SpellScript);

            enum eSpells
            {
                Sated                   = 57724,
                Exhaustion              = 57723,
                Insanity                = 95809,
                TemporalDisplacement    = 80354,
                Fatigued                = 160455
            };

            void ApplyDebuff()
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    if (GetSpellInfo() && l_Target->HasAura(GetSpellInfo()->Id))
                        l_Target->CastSpell(l_Target, eSpells::Exhaustion, true);
                }
            }

            void HandleImmunity(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eSpells::Exhaustion) || l_Target->HasAura(eSpells::Insanity) ||
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
                AfterHit += SpellHitFn(spell_sha_heroism_SpellScript::ApplyDebuff);
                OnEffectHitTarget += SpellEffectFn(spell_sha_heroism_SpellScript::HandleImmunity, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                OnEffectHitTarget += SpellEffectFn(spell_sha_heroism_SpellScript::HandleImmunity, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_heroism_SpellScript();
        }
};

/// 105792 - Lava Shock Spread
class spell_sha_lava_lash_spread: public SpellScriptLoader
{
    public:
        spell_sha_lava_lash_spread() : SpellScriptLoader("spell_sha_lava_lash_spread") { }

        class spell_sha_lava_lash_spread_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lava_lash_spread_SpellScript);

            enum eSpells
            {
                LavaLash = 60103
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                uint32 l_MaxTargets = sSpellMgr->GetSpellInfo(eSpells::LavaLash)->Effects[EFFECT_3].BasePoints;
                Unit* l_MainTarget = GetExplTargetUnit();

                if (l_MainTarget == nullptr)
                    return;

                p_Targets.remove_if([this, l_MainTarget](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr || p_Object->ToUnit()->GetGUID() == l_MainTarget->GetGUID())
                        return true;

                    return false;
                });

                if (p_Targets.size() > l_MaxTargets)
                    JadeCore::RandomResizeList(p_Targets, l_MaxTargets);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_lava_lash_spread_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_lava_lash_spread_SpellScript();
        }
};

/// Update to Legion 7.3.5 build 263635
/// 33757 - Windfury, Feral Spirit Windfury Driver - 170523
class spell_sha_windfury: public SpellScriptLoader
{
    public:
        spell_sha_windfury() : SpellScriptLoader("spell_sha_windfury") { }

        class spell_sha_windfury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_windfury_AuraScript);

            enum eSpellIds
            {
                Mastery             = 77223,
                WindfuryAttack      = 25504,
                Windfury            = 33757,
                PvPBonusWod4        = 171121,
                HailstromDamage     = 210854,
                DoomWindsActive     = 204945,
                WindfuryTotem       = 204332,
                WindfuryTotemBuff   = 209385,
                WindfuryAttackTotem = 232056,
                WindfuryAttackOff   = 33750,

                WindlashMainHand    = 114089,
                WindlashOffHand     = 114093,

                Stormstrike         = 17364,
                StormstrikeAttack   = 32175,
                StormstrikeAttack2  = 32176
            };

            void HandleProcChance(float & p_Chance, ProcEventInfo & p_Proc)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (GetUnitOwner()->HasAura(eSpellIds::DoomWindsActive))
                    p_Chance = 100.f;
            }

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (l_SpellInfo && (l_SpellInfo->Id == eSpellIds::WindfuryAttack || l_SpellInfo->Id == eSpellIds::WindfuryAttackTotem))
                    return;

                if (l_SpellInfo && (l_SpellInfo->Id == eSpellIds::Stormstrike || l_SpellInfo->Id == eSpellIds::StormstrikeAttack || l_SpellInfo->Id == eSpellIds::StormstrikeAttack2))
                    return;

                uint32 l_ProcSpell = eSpellIds::WindfuryAttack;

                /// Doom Winds makes 100% proc but it's for autoattacks only
                /// Spell attacks still has default proc chance
                if (l_Caster->HasAura(eSpellIds::DoomWindsActive))
                {
                    if (l_SpellInfo && l_SpellInfo->Id != eSpellIds::WindlashOffHand && !roll_chance_i(GetSpellInfo()->ProcChance))
                        return;

                    if ((p_EventInfo.GetTypeMask() & ProcFlags::PROC_FLAG_DONE_OFFHAND_ATTACK) && (!l_SpellInfo || (l_SpellInfo && l_SpellInfo->Id == eSpellIds::WindlashOffHand)))
                        l_ProcSpell = eSpellIds::WindfuryAttackOff;
                }
                /// Offhand attacks cannot trigger proc without Doom Winds
                else
                {
                    /// Offhand autoattacks and windlash can proc if player has DoomWinds
                    if (p_EventInfo.GetTypeMask() & ProcFlags::PROC_FLAG_DONE_OFFHAND_ATTACK)
                        return;
                }

                Unit* l_Victim = l_DamageInfo->GetTarget();
                if (!l_Victim)
                    return;

                for (uint8 l_I = 0; l_I < 2; l_I++)
                    l_Caster->CastSpell(l_Victim, l_ProcSpell, true);
            }

            void Register()
            {
                DoCalcProcChance += AuraProcChanceFn(spell_sha_windfury_AuraScript::HandleProcChance);
                OnEffectProc += AuraEffectProcFn(spell_sha_windfury_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_windfury_AuraScript();
        }
};

/// Update to Legion 7.3.2 build 25549
/// Windfury Totem - 209385
class spell_sha_windfury_totem_buff : public SpellScriptLoader
{
    public:
        spell_sha_windfury_totem_buff() : SpellScriptLoader("spell_sha_windfury_totem_buff") { }

        class spell_sha_windfury_totem_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_windfury_totem_buff_AuraScript);

            enum eSpellIds
            {
                WindfuryAttack          = 25504,
                Windfury                = 33757,
                WindfuryTotem           = 204332,
                WindfuryAttackTotem     = 232056
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();
                Unit* l_Target = GetTarget();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Target)
                    return;

                SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();
                if (l_SpellInfo && (l_SpellInfo->Id == eSpellIds::WindfuryAttack || l_SpellInfo->Id == eSpellIds::WindfuryAttackTotem))
                    return;

                Unit* l_Victim = l_DamageInfo->GetTarget();
                if (!l_Victim)
                    return;

                SpellInfo const* l_TotemInfo = sSpellMgr->GetSpellInfo(eSpellIds::WindfuryTotem);
                if (!l_Victim || !l_TotemInfo)
                    return;

                for (uint8 l_I = 0; l_I < l_TotemInfo->Effects[EFFECT_3].BasePoints; l_I++)
                    l_Target->CastSpell(l_Victim, eSpellIds::WindfuryAttackTotem, true);
            }

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Target = GetUnitOwner();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_WindfuryTotemInfo = sSpellMgr->GetSpellInfo(eSpellIds::WindfuryTotem);
                if (!l_Caster || !l_WindfuryTotemInfo || !l_Target)
                    return;

                Creature* l_TotemNPC = l_Target->FindNearestCreature(l_WindfuryTotemInfo->Effects[EFFECT_0].MiscValue, (float)l_WindfuryTotemInfo->Effects[EFFECT_1].BasePoints + 1.0f);
                if (l_TotemNPC && l_TotemNPC->GetOwner() == l_Caster)
                    return;

                if (Aura* l_WindfuryTotemAura = l_Target->GetAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                    l_WindfuryTotemAura->Remove();
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_windfury_totem_buff_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                OnEffectUpdate += AuraEffectUpdateFn(spell_sha_windfury_totem_buff_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_windfury_totem_buff_AuraScript();
        }
};

/// 51533 - Feral Spirit
class spell_sha_feral_spirit: public SpellScriptLoader
{
    public:
        spell_sha_feral_spirit() : SpellScriptLoader("spell_sha_feral_spirit") { }

        class spell_sha_feral_spirit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_feral_spirit_SpellScript);

            enum eSpells
            {
                FeralSpiritWolf         = 228562,
                RaptorTransform         = 172753,
                GlyphOfSpiritRaptors    = 147783,
                DoomWolvesAura          = 198505,
                DoomWolves              = 198506
            };

            void OnLaunch(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /* if (l_Caster->HasAura(eSpells::GlyphOfSpiritRaptors))
                        l_Caster->CastSpell(l_Caster, eSpells::RaptorTransform, true);
                    else */
                    if (l_Caster->HasAura(eSpells::DoomWolvesAura))
                        l_Caster->CastSpell(l_Caster, eSpells::DoomWolves, true);
                    else
                        l_Caster->CastSpell(l_Caster, eSpells::FeralSpiritWolf, true);
                }
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_sha_feral_spirit_SpellScript::OnLaunch, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_feral_spirit_SpellScript();
        }
};

/// Spirit Hunt - 58877
class spell_sha_pet_spirit_hunt: public SpellScriptLoader
{
    public:
        spell_sha_pet_spirit_hunt() : SpellScriptLoader("spell_sha_pet_spirit_hunt") { }

        enum eSpells
        {
            SpiritHuntHeal = 58879,
        };

        class spell_sha_pet_spirit_hunt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_pet_spirit_hunt_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                if (p_EventInfo.GetDamageInfo() == nullptr)
                    return;

                int32 l_TakenDamage = p_EventInfo.GetDamageInfo()->GetAmount();
                if (!l_TakenDamage)
                    return;

                int32 l_HealAmount = CalculatePct(l_TakenDamage, p_AurEff->GetAmount());

                l_Caster->CastCustomSpell(l_Owner, eSpells::SpiritHuntHeal, &l_HealAmount, nullptr, nullptr, true);
                l_Caster->CastCustomSpell(l_Caster, eSpells::SpiritHuntHeal, &l_HealAmount, nullptr, nullptr, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_pet_spirit_hunt_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_pet_spirit_hunt_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Elemental Fury - 60188 (will also work with Elemental Destabilization 238069 since it buffs Elemental Fury)
class spell_shaman_elemental_fury_pvp : public SpellScriptLoader
{
    public:
        spell_shaman_elemental_fury_pvp() : SpellScriptLoader("spell_shaman_elemental_fury_pvp") { }

        class spell_shaman_elemental_fury_pvp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shaman_elemental_fury_pvp_AuraScript);

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amout, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->CanApplyPvPSpellModifiers())
                    return;

                p_Amout *= 0.50f; ///< Elemental Fury (and Elemental Destabilization) crit damage bonuses are halved in PVP situations. Only need to check for Elemental Fury tho
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_shaman_elemental_fury_pvp_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shaman_elemental_fury_pvp_AuraScript();
        }
};

/// Call by Chain Heal - 1064, Riptide - 61295, Chain Heal (T17 Proc) - 177972, Healing Wave - 77472 and Healing Surge - 8004
/// Tidal Waves - 51564
class spell_sha_tidal_waves : public SpellScriptLoader
{
    public:
        spell_sha_tidal_waves() : SpellScriptLoader("spell_sha_tidal_waves") { }

        class spell_sha_tidal_waves_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_tidal_waves_SpellScript);

            enum eSpells
            {
                T17Restoration4P        = 167702,
                HarmonyOfTheElements    = 167703,
                TidalWaves              = 51564,
                TidalWavesProc          = 53390,
                CrashingWaves           = 197464,
                HealingWave             = 77472,
                HealingSurge            = 8004,
                Riptide                 = 61295,
                ChainHeal               = 1064,
                ChainHeal2              = 177972,
                EchoOfElements          = 108283
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::TidalWaves) && (l_SpellInfo->Id == eSpells::ChainHeal || l_SpellInfo->Id == eSpells::Riptide || l_SpellInfo->Id == eSpells::ChainHeal2))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::TidalWavesProc, true);

                    if (l_Caster->HasAura(eSpells::CrashingWaves) && m_scriptSpellId == eSpells::Riptide)
                        l_Caster->CastSpell(l_Caster, eSpells::TidalWavesProc, true);

                    /// When you gain Tidal Waves, you have a 8% chance to reduce the mana cost of Chain Heal by 50% for 8 sec.
                    if (l_Caster->HasAura(eSpells::T17Restoration4P) && roll_chance_i(8))
                        l_Caster->CastSpell(l_Caster, eSpells::HarmonyOfTheElements, true);
                }
            }

            void HandleStackRemove()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Caster->HasAura(eSpells::TidalWaves) && (l_SpellInfo->Id == 77472 || l_SpellInfo->Id == 8004))
                {
                    Aura* l_Aura = l_Caster->GetAura(eSpells::TidalWavesProc);
                    if (l_Aura != nullptr)
                        l_Aura->ModStackAmount(-1);

                    if (Aura* l_Aura = l_Caster->GetAura(EchoOfElements))
                    {
                        SpellInfo const* l_Riptide = sSpellMgr->GetSpellInfo(eSpells::Riptide);
                        if (!roll_chance_i(l_Aura->GetEffect(EFFECT_0)->GetAmount()) || !l_Riptide)
                            return;

                        l_Player->RestoreCharge(l_Riptide->ChargeCategoryEntry);
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_sha_tidal_waves_SpellScript::HandleAfterCast);
                OnHit += SpellHitFn(spell_sha_tidal_waves_SpellScript::HandleStackRemove);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_tidal_waves_SpellScript();
        }
};

/// Last updated : 7.0.3
/// Chain Heal - 1064
class spell_sha_chain_heal : public SpellScriptLoader
{
    public:
        spell_sha_chain_heal() : SpellScriptLoader("spell_sha_chain_heal") { }

        class spell_sha_chain_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_chain_heal_SpellScript);

            enum eSpells
            {
                T17Restoration2P    = 165576,
                ChainHealTriggered  = 177972
            };

            uint64 m_ProcTarget;

            bool Load() override
            {
                m_ProcTarget = 0;
                return true;
            }

            void CheckTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (AuraEffect* l_T17Restoration = l_Caster->GetAuraEffect(eSpells::T17Restoration2P, EFFECT_0))
                    {
                        if (!roll_chance_i(l_T17Restoration->GetAmount()))
                            return;

                        m_ProcTarget = JadeCore::Containers::SelectRandomContainerElement(p_Targets)->GetGUID();
                    }
                }
            }

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                uint32 l_Heal = GetHitHeal();

                if (Unit* l_Target = GetHitUnit())
                {
                    /// You have a chance when you cast Chain Heal to cast a second Chain Heal at one of the targets healed.
                    if (l_Target->GetGUID() == m_ProcTarget)
                        GetCaster()->CastSpell(l_Target, eSpells::ChainHealTriggered, true);
                }

                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_chain_heal_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_TARGET_CHAINHEAL_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_sha_chain_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_chain_heal_SpellScript;
        }
};

/// Las Update 7.3.5 build 26365
/// Riptide - 61295
class spell_sha_riptide : public SpellScriptLoader
{
    public:
        spell_sha_riptide() : SpellScriptLoader("spell_sha_riptide") { }

        enum eSpells
        {
            UnleashLife                 = 73685,
            Cloudburst                  = 157503,
            CloudburstAura              = 157504,
            TidalForce                  = 246729,
            ConcordanceOfTheLegionfall  = 242586
        };

        class spell_sha_riptide_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_riptide_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_UnleashLife = sSpellMgr->GetSpellInfo(eSpells::UnleashLife);

                if (l_UnleashLife == nullptr)
                    return;

                int32 l_BonusHealPct = 0;

                if (l_Caster->HasAura(eSpells::UnleashLife))
                    l_BonusHealPct += l_UnleashLife->Effects[EFFECT_2].BasePoints;

                SetHitHeal(GetHitHeal() + CalculatePct(GetHitHeal(), l_BonusHealPct));
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::UnleashLife))
                    l_Caster->RemoveAurasDueToSpell(eSpells::UnleashLife);

                if (l_Caster->HasAura(eSpells::TidalForce))
                    l_Caster->RemoveAura(eSpells::TidalForce);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_riptide_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
                AfterHit += SpellHitFn(spell_sha_riptide_SpellScript::HandleAfterHit);
            }
        };

        class spell_sha_riptide_AuraScript : public AuraScript
        {
            int32 m_BaseAmount = 0;

            PrepareAuraScript(spell_sha_riptide_AuraScript);

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                m_BaseAmount = p_Amount;
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::UnleashLife);

                if (l_Caster == nullptr || l_SpellInfo == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::UnleashLife))
                    p_Amount += CalculatePct(p_Amount, l_SpellInfo->Effects[EFFECT_2].BasePoints);
            }

            void HandleAfterEffectPeriodic(AuraEffect const* p_AuraEff)
            {
                if (!GetCaster())
                    return;

                Aura* l_Aura = GetCaster()->GetAura(eSpells::CloudburstAura);

                if (!l_Aura || !p_AuraEff)
                    return;

                AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0);

                if (!l_Effect)
                    return;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Cloudburst))
                    l_Effect->SetAmount(l_Effect->GetAmount() + CalculatePct(p_AuraEff->GetAmount(), l_SpellInfo->Effects[EFFECT_1].BasePoints));
            }

            void HandleOnEffectPeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ConcordanceOfTheLegionfall, EFFECT_0))
                    const_cast<AuraEffect*>(p_AurEff)->SetAmount(m_BaseAmount + l_AuraEffect->GetAmount() * GetSpellInfo()->Effects[EFFECT_1].BonusMultiplier);
                else
                    const_cast<AuraEffect*>(p_AurEff)->SetAmount(m_BaseAmount);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_riptide_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_riptide_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_sha_riptide_AuraScript::HandleAfterEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_riptide_AuraScript();
        }

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_riptide_SpellScript;
        }
};

/// Queen's Decree - 208899
class spell_sha_queens_decree : public SpellScriptLoader
{
    public:
        spell_sha_queens_decree() : SpellScriptLoader("spell_sha_queens_decree") { }

        enum eSpells
        {
            Cloudburst = 157503,
            CloudburstAura = 157504
        };


        class spell_sha_queens_decree_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_queens_decree_AuraScript);


            void HandleAfterEffectPeriodic(AuraEffect const* p_AuraEff)
            {
                if (!GetCaster())
                    return;

                Aura* l_Aura = GetCaster()->GetAura(eSpells::CloudburstAura);

                if (!l_Aura || !p_AuraEff)
                    return;

                AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0);

                if (!l_Effect)
                    return;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Cloudburst))
                    l_Effect->SetAmount(l_Effect->GetAmount() + CalculatePct(p_AuraEff->GetAmount(), l_SpellInfo->Effects[EFFECT_1].BasePoints));
            }

            void Register()
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_sha_queens_decree_AuraScript::HandleAfterEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_queens_decree_AuraScript();
        }
};

/// Cloudburst Totem - 157504
/// Updated as of 7.0.3 - 22522
class spell_sha_cloudburst_totem : public SpellScriptLoader
{
    public:
        spell_sha_cloudburst_totem() : SpellScriptLoader("spell_sha_cloudburst_totem") { }

        class spell_sha_cloudburst_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_cloudburst_totem_AuraScript);

            enum eSpells
            {
                Cloudburst      = 157503,
                Shaladrassil    = 208981
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                DamageInfo* l_HealInfo = p_EventInfo.GetDamageInfo();

                if (!l_HealInfo)
                    return;

                /// For Some reason, Shaladrassil's Heal procs twice with each heal, doubling the healing effect
                bool IsShaladrassilHeal = false;
                if (l_HealInfo->GetSpellInfo())
                {
                    if (l_HealInfo->GetSpellInfo()->Id == eSpells::Shaladrassil)
                        IsShaladrassilHeal = true;
                }

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Cloudburst))
                {
                    uint32 l_HealPct = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                    GetEffect(p_AurEff->GetEffIndex())->SetAmount(p_AurEff->GetAmount() + CalculatePct(l_HealInfo->GetAmount(), IsShaladrassilHeal ? l_HealPct / 2 : l_HealPct));
                }
            }

            void OnRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /* p_Mode */)
            {
                if (Unit* l_Owner = GetOwner()->ToUnit())
                {
                    if (int32 l_Amount = p_AurEff->GetAmount())
                    {
                        l_Owner->CastCustomSpell(l_Owner, eSpells::Cloudburst, &l_Amount, nullptr, nullptr, true);
                        GetEffect(p_AurEff->GetEffIndex())->SetAmount(0);
                    }
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_cloudburst_totem_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_cloudburst_totem_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_cloudburst_totem_AuraScript();
        }
};

/// Last updated : 6.1.2 19802
/// Clodburst - 157503
class spell_sha_cloudburst: public SpellScriptLoader
{
    public:
        spell_sha_cloudburst() : SpellScriptLoader("spell_sha_cloudburst") { }

        class spell_sha_cloudburst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_cloudburst_SpellScript);

            bool Load()
            {
                l_TargetCount = 0;
                return true;
            }

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                if (l_TargetCount)
                    SetHitHeal(GetHitHeal() / l_TargetCount);
            }

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                p_Targets.remove_if([](WorldObject* p_Object) -> bool
                {
                    Unit* l_Target = p_Object->ToUnit();
                    if (!l_Target)
                        return true;

                    if (l_Target->IsFullHealth())
                        return true;

                    return false;
                });
                l_TargetCount = p_Targets.size();
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_cloudburst_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_sha_cloudburst_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }

            uint8 l_TargetCount;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_cloudburst_SpellScript;
        }
};

/// Recall Cloudburst Totem - 201764
/// Updated as of 7.0.3 - 22522
class spell_sha_recall_cloudburst_totem : public SpellScriptLoader
{
    public:
        spell_sha_recall_cloudburst_totem() : SpellScriptLoader("spell_sha_recall_cloudburst_totem") { }

        class spell_sha_recall_cloudburst_totem_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_recall_cloudburst_totem_SpellScript);

            enum eSpells
            {
                CloudburstTotem = 157504
            };

            enum eNpcEntries
            {
                CloudburstEntry    = 78001
            };

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* l_Cloudburst = l_Caster->GetFirstMinionByEntry(eNpcEntries::CloudburstEntry))
                {
                    l_Cloudburst->RemoveAurasDueToSpell(eSpells::CloudburstTotem);
                    l_Cloudburst->ToTempSummon()->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_recall_cloudburst_totem_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_recall_cloudburst_totem_SpellScript();
        }
};

/// Last Update 7.3.5
/// Call Wind Gust - 157331
class spell_sha_wind_gust : public SpellScriptLoader
{
    public:
        spell_sha_wind_gust() : SpellScriptLoader("spell_sha_wind_gust") { }

        class spell_sha_wind_gust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_wind_gust_SpellScript);

            enum eSpells
            {
                WindGust = 157331,
                WindGustEnergize = 226180,
                PrimalElementalist = 117013,
                CallLightning = 157348
            };

            void HitTarget(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = l_Caster->GetOwner();

                if (l_Owner == nullptr || !l_Owner->IsPlayer())
                    return;

                l_Caster->CastSpell(l_Owner, eSpells::WindGustEnergize, true);

                int32 l_Damage = 1 + (0.6f * l_Owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL));

                if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::PrimalElementalist, EFFECT_0))
                    AddPct(l_Damage, l_AuraEffect->GetAmount());

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::CallLightning, EFFECT_1))
                    AddPct(l_Damage, l_AuraEffect->GetAmount());

                SetHitDamage(l_Damage);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_wind_gust_SpellScript::HitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_wind_gust_SpellScript();
        }
};

/// Last Update 7.3.5
/// Call Lightning - 157348
class spell_sha_call_lightning : public SpellScriptLoader
{
public:
    spell_sha_call_lightning() : SpellScriptLoader("spell_sha_call_lightning") { }

    class spell_sha_call_lightning_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_call_lightning_SpellScript);

        enum eSpells
        {
            CallLightning = 157348,
            PrimalElementalist = 117013,
        };

        void HitTarget(SpellEffIndex)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Owner = l_Caster->GetOwner();

            if (l_Owner == nullptr || !l_Owner->IsPlayer())
                return;

            int32 l_Damage = 1 + (1.2f * l_Owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL));

            if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::PrimalElementalist, EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount());

            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::CallLightning, EFFECT_1))
                AddPct(l_Damage, l_AuraEffect->GetAmount());

            SetHitDamage(l_Damage);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_sha_call_lightning_SpellScript::HitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_sha_call_lightning_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Call Fire Blast - 57984
class spell_sha_fire_blast : public SpellScriptLoader
{
public:
    spell_sha_fire_blast() : SpellScriptLoader("spell_sha_fire_blast") { }

    class spell_sha_fire_blast_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_fire_blast_SpellScript);

        enum eSpells
        {
            PrimalElementalist = 117013
        };

        void HitTarget(SpellEffIndex)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Owner = l_Caster->GetOwner();

            if (l_Owner == nullptr || !l_Owner->IsPlayer())
                return;

            if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::PrimalElementalist, EFFECT_0))
            {
                int32 l_Damage = GetHitDamage();
                AddPct(l_Damage, l_AuraEffect->GetAmount());
                SetHitDamage(l_Damage);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_sha_fire_blast_SpellScript::HitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_sha_fire_blast_SpellScript();
    }
};

/// WoD PvP Enhancement 2P Bonus - 166103
class spell_sha_WoDPvPEnhancement2PBonus : public SpellScriptLoader
{
    public:
        spell_sha_WoDPvPEnhancement2PBonus() : SpellScriptLoader("spell_sha_WoDPvPEnhancement2PBonus") { }

        class spell_sha_WoDPvPEnhancement2PBonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_WoDPvPEnhancement2PBonus_AuraScript);

            void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_WoDPvPEnhancement2PBonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_WoDPvPEnhancement2PBonus_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Earthen Shield Totem Absorb - 201633
class spell_sha_earthen_shield_absorb : public SpellScriptLoader
{
    public:
        spell_sha_earthen_shield_absorb() : SpellScriptLoader("spell_sha_earthen_shield_absorb") { }

        class spell_sha_earthen_shield_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthen_shield_absorb_AuraScript);

            enum eSpellIds
            {
                EarthenShieldDamage  = 201657,
                Dampening            = 110310
            };

            void CalculateAbsorb(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                if (!GetCaster())
                    return;

                Unit* l_Owner = GetCaster()->GetCharmerOrOwner();
                if (!l_Owner)
                    return;

                p_Amount = l_Owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_NATURE);

                uint32 l_DampeningValue = 0;
                if (AuraEffect* l_DampeningEffect = l_Owner->GetAuraEffect(eSpellIds::Dampening, EFFECT_0))
                    l_DampeningValue = l_DampeningEffect->GetAmount();

                if (l_DampeningValue > 0)
                    p_Amount = CalculatePct(p_Amount, 100 - l_DampeningValue);

                p_CanBeRecalculated = false;
            }

            void OnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Totem = p_AurEff->GetCaster();
                Unit* l_Attacker = p_DmgInfo.GetActor();
                if (!l_Totem || !l_Attacker)
                    return;

                if (Unit* l_Victim = p_DmgInfo.GetTarget())
                {
                    /// deal damage to totem
                    int32 l_Damage = (p_DmgInfo.GetAmount() > p_AbsorbAmount) ? p_AbsorbAmount : p_DmgInfo.GetAmount();
                    l_Victim->CastCustomSpell(l_Totem, eSpellIds::EarthenShieldDamage, &l_Damage, nullptr, nullptr, true, 0, nullptr, l_Attacker->GetGUID());
                    p_DmgInfo.SetAmount((p_DmgInfo.GetAmount() > p_AbsorbAmount) ? p_DmgInfo.GetAmount() - p_AbsorbAmount : 0);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_earthen_shield_absorb_AuraScript::CalculateAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_earthen_shield_absorb_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_earthen_shield_absorb_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Eye of the Storm - 157382
class spell_sha_eye_of_the_storm : public SpellScriptLoader
{
    public:
        spell_sha_eye_of_the_storm() : SpellScriptLoader("spell_sha_eye_of_the_storm") { }

        class spell_sha_eye_of_the_storm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_eye_of_the_storm_AuraScript);

            enum eSpells
            {
                EyeOfTheStormSpeedEffect = 157384
            };

            std::list<uint64> m_TargetList;

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                float l_Radius = 6.0f;

                std::list<Unit*> l_AlliesList;
                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_AlliesList, l_Check);
                l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                for (auto l_Target : l_AlliesList)
                {
                    if (l_Caster->IsValidAssistTarget(l_Target) && std::find(m_TargetList.begin(), m_TargetList.end(), l_Target->GetGUID()) == m_TargetList.end())
                    {
                        l_Target->CastSpell(l_Target, eSpells::EyeOfTheStormSpeedEffect, true);
                        m_TargetList.push_back(l_Target->GetGUID());
                    }
                }

                for (auto l_It = m_TargetList.begin(); l_It != m_TargetList.end();)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(*l_It);
                    if (!l_Target || (std::find(l_AlliesList.begin(), l_AlliesList.end(), l_Target) == l_AlliesList.end()))
                    {
                        if (l_Target)
                            l_Target->RemoveAura(eSpells::EyeOfTheStormSpeedEffect);

                        l_It = m_TargetList.erase(l_It);
                    }
                    else
                        ++l_It;
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                for (uint64 l_TargetGUID : m_TargetList)
                {
                    Unit* l_Target = ObjectAccessor::FindUnit(l_TargetGUID);
                    if (l_Target)
                        l_Target->RemoveAura(eSpells::EyeOfTheStormSpeedEffect);
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_sha_eye_of_the_storm_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_AREATRIGGER);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_eye_of_the_storm_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_AREATRIGGER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_eye_of_the_storm_AuraScript();
        }
};

//////////////////////////////////////////////
////////////////              ////////////////
////////////////    LEGION    ////////////////
////////////////              ////////////////
//////////////////////////////////////////////

/// Last Update 7.3.2 Build 25549
/// Called By Chain Lightning 188443, Chain Lightning Overload - 45297
class spell_sha_chain_lightning : public SpellScriptLoader
{
    public:
        spell_sha_chain_lightning() : SpellScriptLoader("spell_sha_chain_lightning") { }

        enum eSpells
        {
            ChainLightning              = 188443,
            ChainLightningOverload      = 45297,
            MasteryElementalOverload    = 168534
        };

        class spell_sha_chain_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_chain_lightning_SpellScript);

            void HandleOnHit(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_CustomAmount = GetSpell()->GetSpellValue(SPELLVALUE_BASE_POINT1);

                if (l_SpellInfo->Id == eSpells::ChainLightningOverload)
                    if (SpellInfo const* l_OverloadInfo = sSpellMgr->GetSpellInfo(eSpells::MasteryElementalOverload))
                        if (SpellInfo const* l_ChainLightningInfo = sSpellMgr->GetSpellInfo(eSpells::ChainLightning))
                            l_CustomAmount = CalculatePct(l_ChainLightningInfo->Effects[EFFECT_1].BasePoints, l_OverloadInfo->Effects[EFFECT_2].BasePoints);

                l_Caster->ModifyPower(POWER_MAELSTROM, l_CustomAmount);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_chain_lightning_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_chain_lightning_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Earth Shock 8042
class spell_sha_earth_shock : public SpellScriptLoader
{
    public:
        spell_sha_earth_shock() : SpellScriptLoader("spell_sha_earth_shock") { }

        enum eSpells
        {
            TheDeceiversBloodPactAura = 214131,
            TheDeceiversBloodPactProc = 214134,
            EarthenStrength = 252141
        };

        enum eVisuals
        {
            EarthShock = 70001
        };

        class spell_sha_earth_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earth_shock_SpellScript);

            int32 m_PowerSpent = 0;

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != POWER_MAELSTROM)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_PowerCost = l_Caster->GetPower(POWER_MAELSTROM);
                m_PowerSpent = p_PowerCost;
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::EarthShockPowerSpentAndEarthquakes) = m_PowerSpent;
            }

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->SendPlaySpellVisual(eVisuals::EarthShock, l_Target, 0.0, { 0.0f, 0.0f, 0.0f, 0.0f });
            }

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                int32 l_MaxDamage = GetHitDamage();
                if (l_MaxDamage)
                    SetHitDamage((l_MaxDamage * m_PowerSpent) / 100);

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !m_PowerSpent)
                    return;

                if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::TheDeceiversBloodPactAura, EFFECT_0))
                {
                    if (roll_chance_i(l_AurEff->GetAmount()))
                        l_Caster->CastCustomSpell(l_Caster, eSpells::TheDeceiversBloodPactProc, &m_PowerSpent, nullptr, nullptr, true);
                }
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::EarthenStrength);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_sha_earth_shock_SpellScript::HandlePowerCost);
                OnPrepare += SpellOnPrepareFn(spell_sha_earth_shock_SpellScript::HandleOnPrepare);
                OnEffectHitTarget += SpellEffectFn(spell_sha_earth_shock_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterHit += SpellHitFn(spell_sha_earth_shock_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_earth_shock_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called By Flame Shock 188389
class spell_sha_flame_shock : public SpellScriptLoader
{
    public:
        spell_sha_flame_shock() : SpellScriptLoader("spell_sha_flame_shock") { }

        enum eSpells
        {
            FireAndIce = 247526,
            ElementalFocus = 16246
        };

        class spell_sha_flame_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_flame_shock_SpellScript);

            int32 m_PowerSpent = 0;

            void HandeOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target)
                    return;

                if (Aura* l_FlameShock = l_Target->GetAura(l_SpellInfo->Id, l_Caster->GetGUID()))
                    m_PrevDuration = l_FlameShock->GetDuration();

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ElementalFocus, EFFECT_0))
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ElementalFocus) = l_AuraEffect->GetAmount();
            }

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != Powers::POWER_MAELSTROM)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_PowerCost = std::min(l_Caster->GetPower(Powers::POWER_MAELSTROM), 20);

                if (l_Caster->HasAura(eSpells::FireAndIce))
                    p_PowerCost = 0;

                m_PowerSpent = p_PowerCost;
            }

            void HandleDuration()
            {
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Duration = 15 * TimeConstants::IN_MILLISECONDS;

                if (Aura* l_FlameShock = l_Target->GetAura(l_SpellInfo->Id, l_Caster->GetGUID()))
                {
                    if (AuraEffect const* l_FireAndIce = l_Caster->GetAuraEffect(eSpells::FireAndIce, SpellEffIndex::EFFECT_2))
                        AddPct(l_Duration, l_FireAndIce->GetAmount());

                    int32 l_AddDuration = CalculatePct(l_Duration, (((float)m_PowerSpent / 20.0f) * 100.0f));
                    int32 l_TotalDuration = l_Duration + l_AddDuration + m_PrevDuration;

                    if (l_TotalDuration >= 40 * IN_MILLISECONDS)
                        l_TotalDuration = 40 * IN_MILLISECONDS;

                    l_FlameShock->SetDuration(l_TotalDuration);
                    l_FlameShock->SetMaxDuration(l_TotalDuration);
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_sha_flame_shock_SpellScript::HandeOnPrepare);
                OnTakePowers += SpellTakePowersFn(spell_sha_flame_shock_SpellScript::HandlePowerCost);
                AfterHit += SpellHitFn(spell_sha_flame_shock_SpellScript::HandleDuration);
            }

            private:
                int32 m_PrevDuration = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_flame_shock_SpellScript();
        }

        class spell_sha_flame_shock_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_flame_shock_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_BonusAmount > 0)
                    AddPct(p_Amount, m_BonusAmount);
            }

            void AfterApply(AuraEffect const* /*p_aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_BonusAmount = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ElementalFocus);
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ElementalFocus) = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_flame_shock_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectApply += AuraEffectApplyFn(spell_sha_flame_shock_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        private:
            int32 m_BonusAmount = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_flame_shock_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called By Frost Shock 196840
class spell_sha_frost_shock : public SpellScriptLoader
{
    public:
        spell_sha_frost_shock() : SpellScriptLoader("spell_sha_frost_shock") { }

        enum eSpells
        {
            FireAndIce = 247526
        };

        class spell_sha_frost_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_frost_shock_SpellScript);

            int32 m_PowerSpent = 0;

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != Powers::POWER_MAELSTROM)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_PowerCost = std::min(l_Caster->GetPower(Powers::POWER_MAELSTROM), 20);

                if (l_Caster->HasAura(eSpells::FireAndIce))
                    p_PowerCost = 0;

                m_PowerSpent = p_PowerCost;
            }

            void HandleDuration()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_Caster)
                    return;

                if (Aura* l_FrostShock = l_Target->GetAura(l_SpellInfo->Id))
                {
                    int32 l_Duration = l_SpellInfo->GetDuration() * (1.0f + ((float)m_PowerSpent / 20.0f));

                    if (AuraEffect const* l_FireAndIce = l_Caster->GetAuraEffect(eSpells::FireAndIce, SpellEffIndex::EFFECT_1))
                        AddPct(l_Duration, l_FireAndIce->GetAmount());

                    l_FrostShock->SetDuration(l_Duration);
                    l_FrostShock->SetMaxDuration(l_Duration);
                }
            }

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                int32 l_MaxDamage = GetHitDamage();
                if (l_MaxDamage)
                    SetHitDamage(l_MaxDamage * (1.0f + ((float)m_PowerSpent) / 20.0f));
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_sha_frost_shock_SpellScript::HandlePowerCost);
                AfterHit += SpellHitFn(spell_sha_frost_shock_SpellScript::HandleDuration);
                OnEffectHitTarget += SpellEffectFn(spell_sha_frost_shock_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_frost_shock_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called By Lightning Bolt - 188196, and Lightning Bolt Overloaded - 45284
class spell_sha_lightning_bolt : public SpellScriptLoader
{
    public:
        spell_sha_lightning_bolt() : SpellScriptLoader("spell_sha_lightning_bolt") { }

        class spell_sha_lightning_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lightning_bolt_SpellScript);

            enum eSpells
            {
                LightningBolt                   = 188196,
                LightningBoltOverloaded         = 45284,

                LightningBoltEnergize           = 214815,
                LightningBoltOverloadedEnergize = 214816,

                PowerOfTheMaelstrom             = 191877,
                MasteryElementalOverload        = 168534
            };

            uint32 m_PowerOfTheMaelstromCharges = 0;

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, GetSpellInfo()->Id == eSpells::LightningBolt ? eSpells::LightningBoltEnergize : eSpells::LightningBoltOverloadedEnergize, true);
            }

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (m_PowerOfTheMaelstromCharges > 0 && GetSpellInfo()->Id == eSpells::LightningBolt)
                {
                    int32 l_Damage = static_cast<uint32>(GetBaseHitDamage() * 0.75f);
                    l_Caster->CastCustomSpell(l_Target, eSpells::LightningBoltOverloaded, &l_Damage, nullptr, nullptr, true, nullptr, nullptr, 0, 500);
                }
            }

            void HandleBeforeCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::PowerOfTheMaelstrom))
                    {
                        m_PowerOfTheMaelstromCharges = l_Aura->GetStackAmount();
                    }
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_sha_lightning_bolt_SpellScript::HandleBeforeCast);
                AfterHit += SpellHitFn(spell_sha_lightning_bolt_SpellScript::HandleHit);
                AfterCast += SpellCastFn(spell_sha_lightning_bolt_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_lightning_bolt_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called By Elemental Focus - 16164
class spell_sha_elemental_focus : public SpellScriptLoader
{
    public:
        spell_sha_elemental_focus() : SpellScriptLoader("spell_sha_elemental_focus") { }

        class spell_sha_elemental_focus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_elemental_focus_AuraScript);

            enum eSpells
            {
                LavaBurst       = 51505,
                ElementalFocus  = 16246
            };

            bool HandleOnCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (!(p_ProcEventInfo.GetHitMask() & PROC_HIT_CRITICAL))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ElementalFocus, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_elemental_focus_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_sha_elemental_focus_AuraScript::HandleOnProc);

            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_elemental_focus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Fire Elemental - 198067
class spell_sha_fire_elemental : public SpellScriptLoader
{
    public:
        spell_sha_fire_elemental() : SpellScriptLoader("spell_sha_fire_elemental") { }

        class spell_sha_fire_elemental_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_fire_elemental_SpellScript);

            enum eSpells
            {
                PrimalElementalist  = 117013,
                PrimalFireElemental = 118291,
                FireElemental       = 188592,
                T20Elemental2PBonus = 242281,
                FocusedElements     = 246594
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::PrimalElementalist))
                    l_Caster->CastSpell(l_Caster, eSpells::PrimalFireElemental, true);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::FireElemental, true);

                if (l_Caster->HasAura(eSpells::T20Elemental2PBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::FocusedElements, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_sha_fire_elemental_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_fire_elemental_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Earth Elemental - 198103
class spell_sha_earth_elemental : public SpellScriptLoader
{
    public:
        spell_sha_earth_elemental() : SpellScriptLoader("spell_sha_earth_elemental") { }

        class spell_sha_earth_elemental_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earth_elemental_SpellScript);

            enum eSpells
            {
                PrimalElementalist   = 117013,
                PrimalEarthElemental = 118323,
                EarthElemental       = 188616
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::PrimalElementalist))
                    l_Caster->CastSpell(l_Caster, eSpells::PrimalEarthElemental, true);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::EarthElemental, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_sha_earth_elemental_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_earth_elemental_SpellScript();
        }
};

/// Last Update 7.3.3 Build 26365
/// Called By Storm Elemental - 192249
class spell_sha_storm_elemental : public SpellScriptLoader
{
    public:
        spell_sha_storm_elemental() : SpellScriptLoader("spell_sha_storm_elemental") { }

        class spell_sha_storm_elemental_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_storm_elemental_SpellScript);

            enum eSpells
            {
                PrimalElementalist   = 117013,
                PrimalStormElemental = 157319,
                StormElemental       = 157299,
                T20Elemental2PBonus  = 242281,
                FocusedElements      = 246594
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::PrimalElementalist))
                    l_Caster->CastSpell(l_Caster, eSpells::PrimalStormElemental, true);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::StormElemental, true);

                if (l_Caster->HasAura(eSpells::T20Elemental2PBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::FocusedElements, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_sha_storm_elemental_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_storm_elemental_SpellScript();
        }
};

/// last update 7.0.3 Build 22522
/// Called By Elemental Blast - 117014 and Elemental Blast Overload - 120588
class spell_sha_elemental_blast: public SpellScriptLoader
{
    public:
        spell_sha_elemental_blast() : SpellScriptLoader("spell_sha_elemental_blast") { }

        class spell_sha_elemental_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_elemental_blast_SpellScript);

            enum eSpells
            {
                ElementalBlastCrit = 118522,
                ElementalBlastHaste = 173183,
                ElementalBlastMastery = 173184
            };

            void HandleOnHit()
            {
                std::vector<uint32> l_SpellsToCast =
                {
                    eSpells::ElementalBlastCrit ,
                    eSpells::ElementalBlastHaste,
                    eSpells::ElementalBlastMastery,
                };

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->CastSpell(GetCaster(), l_SpellsToCast[urand(0, l_SpellsToCast.size() - 1)], true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sha_elemental_blast_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_elemental_blast_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called by Healing Surge - 188070
class spell_sha_healing_surge : public SpellScriptLoader
{
    public:
        spell_sha_healing_surge() : SpellScriptLoader("spell_sha_healing_surge") { }

        class spell_sha_healing_surge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_healing_surge_SpellScript);

            enum eSpells
            {
                HealingSurgeCastMoving = 190900
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Player || !l_SpellInfo)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_SHAMAN_ENHANCEMENT && l_Player->GetPower(POWER_MAELSTROM) >= l_SpellInfo->Effects[EFFECT_2].BasePoints)
                {
                    l_Player->CastSpell(l_Caster, eSpells::HealingSurgeCastMoving, true);
                    l_Player->ModifyPower(POWER_MAELSTROM, -l_SpellInfo->Effects[EFFECT_2].BasePoints);
                    GetSpell()->SetCastTime(0);
                    return;
                }

                if (l_Player->HasAura(eSpells::HealingSurgeCastMoving))
                    l_Player->RemoveAura(eSpells::HealingSurgeCastMoving);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_sha_healing_surge_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_healing_surge_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called by Crash Lightning - 187874
class spell_sha_crash_lightning : public SpellScriptLoader
{
    public:
        spell_sha_crash_lightning() : SpellScriptLoader("spell_sha_crash_lightning") { }

        class spell_sha_crash_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_crash_lightning_SpellScript);

            enum eSpells
            {
                CrashLightning          = 187878,
                CrashingStorm           = 192246,
                CrashingStormAT         = 210797,
                GatheringStorms         = 198299,
                GatheringStormsProc     = 198300,
                EmalonsChargedCore      = 208741,
                EmalonsChargedCoreProc  = 208742,
                AlphaWolfAura           = 198434,
                AlphaWolfProc           = 198486,
                CrashingLightning       = 242286
            };

            enum eNPC
            {
                DoomWolves = 100820
            };

            enum eArtifactPowers
            {
                GatheringStormsArtifact = 912
            };

            uint32 m_HitTarget = 0;
            int32 m_Bp0Sum = 0;

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                ++m_HitTarget;
                if (m_HitTarget == 2)
                    l_Caster->CastSpell(l_Caster, eSpells::CrashLightning, true);

                if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::EmalonsChargedCore, EFFECT_0))
                {
                    if (m_HitTarget == l_AurEff->GetAmount())
                        l_Caster->CastSpell(l_Caster, eSpells::EmalonsChargedCoreProc, true);
                }

                if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::GatheringStorms, EFFECT_0))
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        int8 l_TraitRank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::GatheringStormsArtifact);

                        if(l_TraitRank > 0 && m_HitTarget <= 50)
                            m_Bp0Sum += l_AurEff->GetAmount();

                        l_Player->CastCustomSpell(l_Player, eSpells::GatheringStormsProc, &m_Bp0Sum, nullptr, nullptr, true);
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::AlphaWolfAura))
                {
                    GuidList *l_Wolves = l_Caster->GetSummonList(eNPC::DoomWolves);

                    for (int64 l_WolfId : *l_Wolves)
                    {
                        if (Unit* l_Wolf = sObjectAccessor->GetUnit(*l_Caster, l_WolfId))
                            l_Caster->CastSpell(l_Wolf, eSpells::AlphaWolfProc, true);
                    }
                }

                l_Caster->RemoveAura(eSpells::CrashingLightning);

                if (!l_Caster->HasAura(eSpells::CrashingStorm))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CrashingStormAT, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_crash_lightning_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                AfterCast += SpellCastFn(spell_sha_crash_lightning_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_crash_lightning_SpellScript();
        }
};

/// Last 7.0.3 Build 22293
/// Called By Crash Lightning - 187878
class spell_sha_crash_lightning_aura : public SpellScriptLoader
{
    public:
        spell_sha_crash_lightning_aura() : SpellScriptLoader("spell_sha_crash_lightning_aura") { }

        class spell_sha_crash_lightning_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_crash_lightning_aura_AuraScript);

            enum eSpells
            {
                StormstrikeMH   = 32175,
                WindstrikeMH    = 115357,
                LavaLash        = 60103
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::StormstrikeMH && l_SpellInfo->Id != eSpells::LavaLash && l_SpellInfo->Id != eSpells::WindstrikeMH))
                    return false;

                return true;
            }


            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_crash_lightning_aura_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_crash_lightning_aura_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Stormbringer - 201845
class spell_sha_stormbinger : public SpellScriptLoader
{
    public:
        spell_sha_stormbinger() : SpellScriptLoader("spell_sha_stormbinger") { }

        class spell_sha_stormbinger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_stormbinger_AuraScript);

            enum eSpells
            {
                Stormbringer        = 201846,
                Stormstrike         = 17364,
                StormstrikeMainHand = 32175,
                StormstrikeOffHand  = 32176,
                Windlash            = 114089,
                Windstrike          = 115356,
                WindstrikeMainHand  = 115357,
                WindstrikeOffHand   = 115360,
                WindfuryAttack      = 25504,
                WindfuryAttackOff   = 33750,

                WindStrikesTrait    = 198292,
                WindStrikesProc     = 198293,

                Rockbiter           = 193786,
                Flametongue         = 193796,
                Frostbrand          = 196834
            };

            enum eArtifactPowers
            {
                WindStrikeArtifact = 910
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Spell const* l_Spell = p_ProcEventInfo.GetDamageInfo()->GetSpell();
                if (l_Spell != nullptr && l_SpellInfo != nullptr)
                {
                    /// Stormstrike and Windstrike are handled on dealing damage by triggered spells
                    if (l_SpellInfo->Id == eSpells::Stormstrike || l_SpellInfo->Id == eSpells::Windstrike)
                        return;

                    /// don't proc from triggered spells excepted windfury, stormstrike, windlash and windstrike
                    if (l_Spell->IsTriggered() && l_SpellInfo != nullptr)
                    {
                        switch (l_SpellInfo->Id)
                        {
                            case eSpells::WindfuryAttack:
                            case eSpells::WindfuryAttackOff:
                            case eSpells::StormstrikeMainHand:
                            case eSpells::StormstrikeOffHand:
                            case eSpells::WindstrikeMainHand:
                            case eSpells::WindstrikeOffHand:
                            case eSpells::Windlash:
                                break;
                            default:
                                return;
                        }
                    }
                    else if (p_ProcEventInfo.GetTypeMask() & ProcFlags::PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG)
                    {
                        switch (l_SpellInfo->Id)
                        {
                            case eSpells::Rockbiter:
                            case eSpells::Flametongue:
                            case eSpells::Frostbrand:
                                break;
                            default:
                                return;
                        }
                    }
                }

                l_Player->RemoveSpellCooldown(eSpells::Stormstrike, true);
                l_Player->RemoveSpellCooldown(eSpells::Windstrike, true);

                l_Player->CastSpell(l_Player, eSpells::Stormbringer, true);

                if (l_Caster->HasAura(eSpells::WindStrikesTrait))
                {
                    SpellInfo const* l_WindStrikesInfo = sSpellMgr->GetSpellInfo(eSpells::WindStrikesTrait);

                    if (l_WindStrikesInfo)
                    {
                        uint32 l_TraitRank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::WindStrikeArtifact);
                        int32 l_HastPct = l_WindStrikesInfo->Effects[0].BasePoints * l_TraitRank;

                        l_Caster->CastCustomSpell(l_Caster, eSpells::WindStrikesProc, &l_HastPct, nullptr, nullptr, true);
                    }
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_sha_stormbinger_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_stormbinger_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by T19 Enhancement 4P Bonus - 211988
class spell_sha_t19_enhancement_4p_bonus : public SpellScriptLoader
{
public:
    spell_sha_t19_enhancement_4p_bonus() : SpellScriptLoader("spell_sha_t19_enhancement_4p_bonus") { }

    class spell_sha_t19_enhancement_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_t19_enhancement_4p_bonus_AuraScript);

        enum eSpells
        {
            LavaLash = 60103,
            Stormbringer = 201846,
            Stormstrike = 17364,
            Windstrike = 115356,

            WindStrikesTrait = 198292,
            WindStrikesProc = 198293
        };

        enum eArtifactPowers
        {
            WindStrikeArtifact = 910
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            Unit* l_Target = l_DamageInfo->GetTarget();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::LavaLash)
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

            l_Player->RemoveSpellCooldown(eSpells::Stormstrike, true);
            l_Player->RemoveSpellCooldown(eSpells::Windstrike, true);

            l_Player->CastSpell(l_Player, eSpells::Stormbringer, true);

            if (l_Caster->HasAura(eSpells::WindStrikesTrait))
            {
                SpellInfo const* l_WindStrikesInfo = sSpellMgr->GetSpellInfo(eSpells::WindStrikesTrait);

                if (l_WindStrikesInfo)
                {
                    uint32 l_TraitRank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::WindStrikeArtifact);
                    int32 l_HastPct = l_WindStrikesInfo->Effects[0].BasePoints * l_TraitRank;

                    l_Caster->CastCustomSpell(l_Caster, eSpells::WindStrikesProc, &l_HastPct, nullptr, nullptr, true);
                }
            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_t19_enhancement_4p_bonus_AuraScript::HandleCheckProc);
            OnProc += AuraProcFn(spell_sha_t19_enhancement_4p_bonus_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_t19_enhancement_4p_bonus_AuraScript();
    }
};

/// Last Update 7.0.3 Build 22293
/// Called by Resurgence - 16196
class spell_sha_resurgence : public SpellScriptLoader
{
    public:
        spell_sha_resurgence() : SpellScriptLoader("spell_sha_resurgence") { }

        class spell_sha_resurgence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_resurgence_AuraScript);

            enum eSpells
            {
                HealingWave         = 77472,
                HealingSurge        = 8004,
                Riptide             = 61295,
                ChainHeal           = 1064,
                Resurgence          = 101033,
                RefreshingCurrents  = 207356,
                BottomlessDepths    = 197467
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (!(p_ProcEventInfo.GetHitMask() & PROC_HIT_CRITICAL))
                    return;

                if (DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo())
                {
                    if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                    {
                        int32 l_Pct;
                        switch (l_SpellInfo->Id)
                        {
                            case eSpells::HealingWave:
                                l_Pct = 100;
                                break;
                            case eSpells::HealingSurge:
                            case eSpells::Riptide:
                                l_Pct = 60;
                                break;
                            case eSpells::ChainHeal:
                            {
                                l_Pct = 25;

                                if (AuraEffect* const p_AuraEffect = l_Caster->GetAuraEffect(eSpells::RefreshingCurrents, EFFECT_0))
                                    l_Pct = AddPct(l_Pct, p_AuraEffect->GetBaseAmount());

                                break;
                            }
                            default:
                                l_Pct = 0;
                                break;
                        }

                        l_Caster->CastCustomSpell(l_Caster, eSpells::Resurgence, &l_Pct, NULL, NULL, true);
                    }
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_sha_resurgence_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_resurgence_AuraScript();
        }
};

/// Called by Bottomless Depths - 197467
class spell_sha_bottomless_depths : public SpellScriptLoader
{
public:
    spell_sha_bottomless_depths() : SpellScriptLoader("spell_sha_bottomless_depths") { }

    class spell_sha_bottomless_depths_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_bottomless_depths_AuraScript);

        enum eSpells
        {
            HealingWave = 77472,
            HealingSurge = 8004,
            Riptide = 61295,
            ChainHeal = 1064,
            Resurgence = 101033,
            RefreshingCurrents = 207356,
            BottomlessDepths = 197467
        };

        int32 m_Pct;

        bool HandleDoCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            Unit* l_Caster = GetCaster();

            if (!l_Caster)
                return false;

            if (l_Caster->GetHealthPct() < 60)
            {
                if (DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo())
                {
                    if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                    {
                        switch (l_SpellInfo->Id)
                        {
                            case eSpells::HealingWave:
                                m_Pct = 100;
                                break;
                            case eSpells::HealingSurge:
                            case eSpells::Riptide:
                                m_Pct = 60;
                                break;
                            case eSpells::ChainHeal:
                            {
                                m_Pct = 25;

                                if (AuraEffect* const p_AuraEffect = l_Caster->GetAuraEffect(eSpells::RefreshingCurrents, EFFECT_0))
                                    m_Pct = AddPct(m_Pct, p_AuraEffect->GetBaseAmount());

                                break;
                            }
                            default:
                                m_Pct = 0;
                                break;
                        }

                        return true;
                    }
                }
            }

            return false;
        }

        void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster)
                l_Caster->CastCustomSpell(l_Caster, eSpells::Resurgence, &m_Pct, NULL, NULL, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_bottomless_depths_AuraScript::HandleDoCheckProc);
            OnProc += AuraProcFn(spell_sha_bottomless_depths_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_bottomless_depths_AuraScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Called by Maelstrom Weapon - 187880
class spell_sha_maelstrom_weapon : public SpellScriptLoader
{
    public:
        spell_sha_maelstrom_weapon() : SpellScriptLoader("spell_sha_maelstrom_weapon") { }

        class spell_sha_maelstrom_weapon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_maelstrom_weapon_AuraScript);

            enum eSpells
            {
                MaelstromWeaponEnergize = 187890,
                WindfuryAttack          = 25504,
                WindlashMainHand        = 114089,
                WindlashOffHand         = 114093
            };

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || l_DamageInfo == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();

                if (!l_SpellInfo || l_SpellInfo->Id == eSpells::WindfuryAttack || l_SpellInfo->Id == WindlashMainHand || l_SpellInfo->Id == WindlashOffHand)
                    l_Caster->CastSpell(l_Caster, eSpells::MaelstromWeaponEnergize, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_maelstrom_weapon_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_maelstrom_weapon_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Flametongue - 194084
class spell_sha_flametongue : public SpellScriptLoader
{
    public:
        spell_sha_flametongue() : SpellScriptLoader("spell_sha_flametongue") { }

        class spell_sha_flametongue_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_flametongue_AuraScript);

            enum eSpells
            {
                FlametongueProcDamage = 10444,
                HailstormDamage = 210854
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                /// don't proc from Hailstorm
                if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                {
                    if (l_SpellInfo->Id == eSpells::HailstormDamage)
                        return;
                }

                l_Caster->CastSpell(l_Target, eSpells::FlametongueProcDamage, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_sha_flametongue_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_flametongue_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Healing Stream - 5672 (Healing Stream Totem - 5394)
/// Called for Legendary Praetorian's Tidecaller - 137058
class spell_sha_healing_stream_lego : public SpellScriptLoader
{
    public:
        spell_sha_healing_stream_lego() : SpellScriptLoader("spell_sha_healing_stream_lego") { }

        class spell_sha_healing_stream_lego_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_healing_stream_lego_AuraScript);

            enum eSpells
            {
                PraetorianTidecallerAura = 210604
            };

            void AfterApply(AuraEffect const* p_aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (!p_aurEff)
                    return;

                Aura* l_Aura = p_aurEff->GetBase();
                SpellInfo const* l_PraetorInfo = sSpellMgr->GetSpellInfo(eSpells::PraetorianTidecallerAura);
                if (!l_Aura || !l_PraetorInfo)
                    return;

                uint32 l_PercentIncrease = l_PraetorInfo->Effects[EFFECT_0].BasePoints;
                uint32 l_MaxDur = l_Aura->GetMaxDuration();

                l_Aura->SetMaxDuration(AddPct(l_MaxDur, l_PercentIncrease));
                l_Aura->SetDuration(l_Aura->GetMaxDuration());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sha_healing_stream_lego_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_healing_stream_lego_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Lightning Bolt - 187837
class spell_sha_enh_lightning_bolt : public SpellScriptLoader
{
    public:
        spell_sha_enh_lightning_bolt() : SpellScriptLoader("spell_sha_enh_lightning_bolt") { }

        class spell_sha_enh_lightning_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_enh_lightning_bolt_SpellScript);

            enum eSpells
            {
                Overcharge = 210727
            };

            enum eDatas
            {
                MaxPowerCost = 40
            };

            int32 m_PowerSpent = 0;

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != POWER_MAELSTROM)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::Overcharge))
                    return;

                p_PowerCost = std::min(l_Caster->GetPower(POWER_MAELSTROM), int32(eDatas::MaxPowerCost));
                m_PowerSpent = p_PowerCost;
            }

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                SpellInfo const* l_Overcharge = sSpellMgr->GetSpellInfo(eSpells::Overcharge);
                int32 l_BaseDamage = GetHitDamage();
                if (!l_Overcharge || !l_BaseDamage)
                    return;

                float l_Modifier = (((float)m_PowerSpent / (float)eDatas::MaxPowerCost) * (float(l_Overcharge->Effects[EFFECT_1].BasePoints) / 100.0f));

                if (!GetCaster()->HasAura(eSpells::Overcharge))
                    l_Modifier = 1.0f;

                SetHitDamage(int32(l_BaseDamage * l_Modifier));
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_sha_enh_lightning_bolt_SpellScript::HandlePowerCost);
                OnEffectHitTarget += SpellEffectFn(spell_sha_enh_lightning_bolt_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_enh_lightning_bolt_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called By Feral Lunge - 196884
class spell_sha_feral_lunge : public SpellScriptLoader
{
    public:
        spell_sha_feral_lunge() : SpellScriptLoader("spell_sha_feral_lunge") { }

        class spell_sha_feral_lunge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_feral_lunge_SpellScript);

            enum eSpells
            {
                GhostWolf   = 202499,
                WolfJump    = 202498
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::GhostWolf, true);
                l_Caster->CastSpell(l_Target, eSpells::WolfJump, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_feral_lunge_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_feral_lunge_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called By Feral Lunge - 202498
class spell_sha_feral_lunge_jump : public SpellScriptLoader
{
    public:
        spell_sha_feral_lunge_jump() : SpellScriptLoader("spell_sha_feral_lunge_jump") { }

        class spell_sha_feral_lunge_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_feral_lunge_jump_SpellScript);

            enum eSpells
            {
                RemoveWolf  = 202505
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RemoveWolf, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_feral_lunge_jump_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_feral_lunge_jump_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called by Resurgence - 101033
class spell_sha_resurgence_heal : public SpellScriptLoader
{
    public:
        spell_sha_resurgence_heal() : SpellScriptLoader("spell_sha_resurgence_heal") { }

        class spell_sha_resurgence_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_resurgence_heal_SpellScript);

            uint32 m_HitTarget = 0;

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
                l_Player->EnergizeBySpell(l_Player, m_scriptSpellId, l_Amount- l_MaxMana, POWER_MANA);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_resurgence_heal_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_ENERGIZE_PCT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_resurgence_heal_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Elemental Healing - 198248
class spell_sha_elemental_healing : public SpellScriptLoader
{
    public:
        spell_sha_elemental_healing() : SpellScriptLoader("spell_sha_elemental_healing") { }

        class spell_sha_elemental_healing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_elemental_healing_AuraScript);

            enum Spells : uint32
            {
                ElementalHealingHeal = 198249
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SPEC_SHAMAN_ENHANCEMENT)
                    return;

                int32 l_Bp0 = p_AurEff->GetAmount();
                l_Player->CastCustomSpell(l_Player, Spells::ElementalHealingHeal, &l_Bp0, nullptr, nullptr, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_elemental_healing_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_elemental_healing_AuraScript();
        }
};

/// Aftershock 210707
class PlayerScript_aftershock : public PlayerScript
{
    public:
        PlayerScript_aftershock() :PlayerScript("PlayerScript_aftershock") {}

        enum Spells : uint32
        {
            AfterShock          = 210707,
            AftershockEnergize  = 210712
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            AuraEffect* p_AurEff = p_Player->GetAuraEffect(Spells::AfterShock, EFFECT_0);
            int32 l_Diff = p_OldValue - p_NewValue;

            if (!p_AurEff || !p_After || p_Regen || l_Diff < 0 || p_Power != Powers::POWER_MAELSTROM)
                return;

            l_Diff = CalculatePct(l_Diff, p_AurEff->GetAmount());
            p_Player->CastCustomSpell(p_Player, Spells::AftershockEnergize, &l_Diff, nullptr, nullptr, true);
       }
};

// Hailstorm - 210853
class spell_sha_hailstorm : public SpellScriptLoader
{
    public:
        spell_sha_hailstorm() : SpellScriptLoader("spell_sha_hailstorm") { }

        class spell_sha_hailstorm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_hailstorm_AuraScript);

            enum eSpellIds : uint32
            {
                Frostbrand              = 196834,
                HailstormDamage         = 210854,
                FlametongueProcDamage   = 10444
            };

            void HandleOnProc(AuraEffect const* /*p_AuraEffect*/,  ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpellIds::Frostbrand))
                    return;

                /// don't proc from Flametongue damage
                if (SpellInfo const* l_SpellInfo = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo())
                {
                    if (l_SpellInfo->Id == eSpellIds::FlametongueProcDamage)
                        return;
                }

                l_Caster->CastSpell(p_ProcEventInfo.GetActionTarget(), eSpellIds::HailstormDamage, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_hailstorm_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_hailstorm_AuraScript();
        }
};

/// last update : 7.3.5 build 26365
/// Healing Rain - 73920
class spell_sha_healing_rain : public SpellScriptLoader
{
    public:
        spell_sha_healing_rain() : SpellScriptLoader("spell_sha_healing_rain") { }

        enum eSpells
        {
            HealingRainHeal         = 73921,
            HealingRainVisual       = 147490,
            ElementalRebalancerAura = 208776,
            ElementalRebalancerProc = 208777,
            FireInTheDeep           = 248036,
            AscendanceRestore       = 114052,
            SpiritRain              = 246771,
            T21Restoration2P        = 251764,
            Rainfall                = 252154
        };

        enum eNPC
        {
            HealingRainTrigger = 73400
        };

        class spell_sha_healing_rain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_healing_rain_SpellScript);

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();

                if (WorldLocation const* l_Loc = GetExplTargetDest())
                {
                    if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                        l_Trigger->DespawnOrUnsummon();

                    if (TempSummon* l_Summon = l_Caster->SummonCreature(eNPC::HealingRainTrigger, *l_Loc, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        l_Summon->SetOwnerGUID(l_Caster->GetGUID());
                        l_Caster->SetHealingRainTrigger(l_Summon->GetGUID());
                        l_Summon->CastSpell(l_Summon, eSpells::HealingRainVisual, true);

                        if (l_Caster->HasAura(eSpells::T21Restoration2P))
                            l_Caster->CastSpell(l_Summon, eSpells::Rainfall, true);
                    }
                }

                if (l_Caster->HasAura(eSpells::FireInTheDeep))
                {
                    if (SpellInfo const* l_FITDInfo = sSpellMgr->GetSpellInfo(eSpells::FireInTheDeep))
                    {
                        if (roll_chance_i(l_FITDInfo->Effects[EFFECT_0].BasePoints))
                        {
                            if (Aura* AscendanceAura = l_Caster->AddAura(eSpells::AscendanceRestore, l_Caster))
                            {
                                uint32 durationTime = l_FITDInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS;
                                AscendanceAura->SetDuration(durationTime);
                                AscendanceAura->SetMaxDuration(durationTime);
                            }
                        }
                    }
                }

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::SpiritRain, EFFECT_0))
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::T20Restoration4PBonus) = l_AuraEffect->GetAmount();

                l_Caster->RemoveAura(eSpells::SpiritRain);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_sha_healing_rain_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_healing_rain_SpellScript();
        }

        class spell_sha_healing_rain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_healing_rain_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                {
                    l_Caster->CastSpell(l_Trigger, eSpells::HealingRainHeal, true);

                    if (l_Caster->HasAura(eSpells::ElementalRebalancerAura))
                        l_Caster->CastSpell(l_Trigger, eSpells::ElementalRebalancerProc, true);
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                    l_Trigger->DespawnOrUnsummon();

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::T20Restoration4PBonus) = 0;
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_healing_rain_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_healing_rain_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_healing_rain_AuraScript();
        }
};

/// last update : 7.1.5
/// Healing Rain (Heal) - 73921
class spell_sha_healing_rain_heal : public SpellScriptLoader
{
    public:
        spell_sha_healing_rain_heal() : SpellScriptLoader("spell_sha_healing_rain_heal") { }

        enum eSpells
        {
            HealingRain         = 73920,
            HealingRainBonus    = 142923
        };

        class spell_sha_healing_rain_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_healing_rain_heal_SpellScript);

            uint32 m_TargetAmount = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                m_TargetAmount = p_Targets.size();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HealingRain);
                if (!l_SpellInfo)
                    return;

                if (m_TargetAmount <= l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                p_Targets.resize(l_SpellInfo->Effects[EFFECT_1].BasePoints);
            }

            void HandleHeal(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_Heal = GetHitHeal();

                /// Increase the effectiveness by 30% with Unleashed Life
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::HealingRainBonus, EFFECT_1))
                    AddPct(l_Heal, l_AuraEffect->GetAmount());

                if (l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::T20Restoration4PBonus) > 0)
                    AddPct(l_Heal, l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::T20Restoration4PBonus));

                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_healing_rain_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_healing_rain_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_healing_rain_heal_SpellScript();
        }
};

/// Last Update 7.0.3
/// Tidal Pools - 207358
class spell_sha_tidal_pools : public SpellScriptLoader
{
    public:
        spell_sha_tidal_pools() : SpellScriptLoader("spell_sha_tidal_pools") { }

        class spell_sha_tidal_pools_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_tidal_pools_AuraScript);

            enum eSpells
            {
                Riptide = 61295,
                TidalPools = 208932
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Victim = p_EventInfo.GetActionTarget();

                if (l_Caster == nullptr || l_Victim == nullptr)
                    return;

                if (!p_EventInfo.GetDamageInfo()->GetSpellInfo())
                    return;

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id != eSpells::Riptide)
                    return;

                l_Caster->CastSpell(l_Victim, eSpells::TidalPools, true);
            }

            void Register() override
            {
                 OnEffectProc += AuraEffectProcFn(spell_sha_tidal_pools_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_tidal_pools_AuraScript();
        }
};

/// Earth Shield - 204288
class spell_sha_earth_shield : public SpellScriptLoader
{
    public:
        spell_sha_earth_shield() : SpellScriptLoader("spell_sha_earth_shield") { }

        class spell_sha_earth_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earth_shield_AuraScript);

            enum eSpells
            {
                EarthShieldHeal = 204290
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();

                if (!l_DamageInfo || !l_Caster)
                    return false;

                if (l_DamageInfo->GetDamageType() == DamageEffectType::HEAL)
                    return false;

                AuraEffect* l_EarthShieldEffect = l_Caster->GetAuraEffect(m_scriptSpellId, EFFECT_1);
                if (!l_EarthShieldEffect)
                    return false;

                if (l_DamageInfo->GetAmount() > CalculatePct(l_Caster->GetMaxHealth(), l_EarthShieldEffect->GetAmount()))
                    return true;

                return false;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(p_EventInfo.GetActionTarget(), eSpells::EarthShieldHeal, true);
                DropCharge();
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_earth_shield_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_sha_earth_shield_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_earth_shield_AuraScript();
        }
};

/// last update : 7.3.5 build 26124
/// Healing Tide Totem - 108280
class spell_sha_healing_tide_totem : public SpellScriptLoader
{
    public:
        spell_sha_healing_tide_totem() : SpellScriptLoader("spell_sha_healing_tide_totem") { }

        class spell_sha_healing_tide_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_healing_tide_totem_AuraScript);

            enum eNPC
            {
                HealingTotem = 59764
            };

            enum eSpells
            {
                HealingTide = 114942
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();

                for (Unit* l_Controlled : l_Caster->m_Controlled)
                {
                    if (l_Controlled->GetEntry() == eNPC::HealingTotem)
                        l_Controlled->CastSpell(l_Controlled, eSpells::HealingTide, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_healing_tide_totem_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_healing_tide_totem_AuraScript();
        }
};

/// last update : 7.1.5 - Build 23420
/// Healing Tide - 114942
class spell_sha_healing_tide_heal : public SpellScriptLoader
{
    public:
        spell_sha_healing_tide_heal() : SpellScriptLoader("spell_sha_healing_tide_heal") { }

        class spell_sha_healing_tide_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_healing_tide_heal_SpellScript);

            uint32 m_TargetSize = 0;

            enum eSpells
            {
                CumulativeUpkeepAura  = 207362,
                CumulativeUpkeepProc  = 208205,
                VelensFutureSight     = 235966,
                VelensFutureSightHeal = 235967
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Totem = GetCaster();
                if (!l_Totem)
                    return;

                Unit* l_Owner = l_Totem->GetOwner();

                if (l_Owner == nullptr)
                    return;

                for (std::list<WorldObject*>::iterator l_It = p_Targets.begin(); l_It != p_Targets.end();)
                {
                    Unit* l_Unit = (*l_It)->ToUnit();
                    if (l_Unit == nullptr)
                        l_It = p_Targets.erase(l_It);
                    else if (!l_Owner->IsInRaidWith(l_Unit))
                        l_It = p_Targets.erase(l_It);
                    else
                        l_It++;
                }

                /// Cumulative upkeep targets ALL allies in an area, so need to also filter targets in spell_sha_cumulative_upkeep to see which one is in raid with the Shaman
                if (l_Owner->HasAura(eSpells::CumulativeUpkeepAura))
                {
                    l_Totem->AddDelayedEvent([l_Totem]()
                    {
                        l_Totem->CastSpell(l_Totem, eSpells::CumulativeUpkeepProc, true);
                    }, 1);
                }
            }

            void HandleHeal(SpellEffIndex)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Totem = GetCaster();

                if (!l_Totem || !l_Target)
                    return;

                Unit* l_Owner = l_Totem->GetOwner();
                if (!l_Owner)
                    return;

                int32 l_Damage = GetHitHeal();

                AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpells::CumulativeUpkeepProc, 0, l_Totem->GetGUID());
                if (l_AurEff)
                {
                    l_Damage = AddPct(l_Damage, l_AurEff->GetAmount());
                    SetHitHeal(l_Damage);
                }

                int32 l_OverHeal = (l_Target->GetHealth() + l_Damage) - l_Target->GetMaxHealth();
                if (l_OverHeal > 0)
                {
                    if (l_Owner->HasAura(eSpells::VelensFutureSight))
                    {
                        if (SpellInfo const* l_VelensFSInfo = sSpellMgr->GetSpellInfo(eSpells::VelensFutureSight))
                        {
                            int32 l_Amount = CalculatePct(l_OverHeal, l_VelensFSInfo->Effects[EFFECT_3].BasePoints);

                            l_Totem->CastCustomSpell(l_Totem, eSpells::VelensFutureSightHeal, &l_Amount, nullptr, nullptr, true);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_healing_tide_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_healing_tide_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_healing_tide_heal_SpellScript();
        }
};

/// last update : 7.1.5 - Build 23420
/// Called by Cumulative Upkeep - 208205
class spell_sha_cumulative_upkeep : public SpellScriptLoader
{
public:
    spell_sha_cumulative_upkeep() : SpellScriptLoader("spell_sha_cumulative_upkeep") { }

    class spell_sha_cumulative_upkeep_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_cumulative_upkeep_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Totem = GetCaster();
            if (!l_Totem)
                return;

            Unit* l_Owner = GetCaster()->GetOwner();

            if (l_Owner == nullptr)
                return;

            for (std::list<WorldObject*>::iterator l_It = p_Targets.begin(); l_It != p_Targets.end();)
            {
                Unit* l_Unit = (*l_It)->ToUnit();
                if (l_Unit == nullptr)
                    l_It = p_Targets.erase(l_It);
                else if (!l_Owner->IsInRaidWith(l_Unit))
                    l_It = p_Targets.erase(l_It);
                else
                    l_It++;
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_cumulative_upkeep_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_sha_cumulative_upkeep_SpellScript();
    }
};

/// last update : 7.0.3
///  Wellspring - 197995
class spell_sha_wellspring : public SpellScriptLoader
{
public:
    spell_sha_wellspring() : SpellScriptLoader("spell_sha_wellspring") { }

    class spell_sha_wellspring_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_wellspring_SpellScript);

        enum eSpells
        {
            WellspringHeal = 198117
        };

        void OnEffectDummy(SpellEffIndex)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();

            if (l_Target == nullptr)
                return;

            l_Caster->CastSpell(l_Target, eSpells::WellspringHeal, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_sha_wellspring_SpellScript::OnEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sha_wellspring_SpellScript();
    }
};

/// Las Update 7.0.3
/// Shamanism - 193876
class spell_sha_shamanism : public SpellScriptLoader
{
    public:
        spell_sha_shamanism() : SpellScriptLoader("spell_sha_shamanism") { }

        enum eSpells
        {
            BloodLust       = 73685,
            BloodLustHonor  = 204361,
            Heroism         = 32182,
            HeroismHonor    = 204362
        };

        class spell_sha_shamanism_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_shamanism_AuraScript);

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                if (l_Player->HasSpell(eSpells::BloodLust))
                    p_Amount = eSpells::BloodLustHonor;
                else if (l_Player->HasSpell(eSpells::Heroism))
                    p_Amount = 204362;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_shamanism_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_shamanism_AuraScript();
        }
};

/// Las Update 7.0.3 Build 22522
/// Called by Lightning Rod - 210689
class spell_sha_lightning_rod : public SpellScriptLoader
{
    public:
        spell_sha_lightning_rod() : SpellScriptLoader("spell_sha_lightning_rod") { }

        class spell_sha_lightning_rod_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_lightning_rod_AuraScript);

            enum eSpells
            {
                LightningRod            = 197209,
                LightningBolt           = 188196,
                LightningBoltOverloaded = 45284,
                ChainLightning          = 188443
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo ||!l_SpellInfo || !l_Caster)
                    return;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Victim = l_DamageInfo->GetTarget();
                if (!l_Spell || !l_Victim || !l_ProcSpellInfo || (l_ProcSpellInfo->Id != eSpells::LightningBolt && l_ProcSpellInfo->Id != eSpells::ChainLightning && l_ProcSpellInfo->Id != eSpells::LightningBoltOverloaded))
                    return;

                Unit* l_UnitTarget = l_Spell->m_targets.GetUnitTarget();
                if (!l_UnitTarget || l_UnitTarget != l_Victim)
                    return;

                if (!roll_chance_f(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                    return;

                int32 l_Damage = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_1].BasePoints);

                l_Caster->CastCustomSpell(l_UnitTarget, eSpells::LightningRod, &l_Damage, nullptr, nullptr, true);
                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::LightningRods].insert(l_UnitTarget->GetGUID());
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_sha_lightning_rod_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_lightning_rod_AuraScript();
        }
};

/// Las Update 7.0.3 Build 22522
/// Called by Lightning Rod - 197209
class spell_sha_lightning_rod_target : public SpellScriptLoader
{
    public:
        spell_sha_lightning_rod_target() : SpellScriptLoader("spell_sha_lightning_rod_target") { }

        class spell_sha_lightning_rod_target_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_lightning_rod_target_AuraScript);

            void HandleOnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::LightningRods].erase(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_lightning_rod_target_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_lightning_rod_target_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Chain Lightning - 188443 - Lightning Bolt - 188196, Lightning bolt overloaded - 45284
/// Called For Lightning Rod - 210689 and Stormkeeper 205495
class spell_sha_lightning_rod_damages : public SpellScriptLoader
{
    public:
        spell_sha_lightning_rod_damages() : SpellScriptLoader("spell_sha_lightning_rod_damages") { }

        class spell_sha_lightning_rod_damages_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lightning_rod_damages_SpellScript);

            enum eSpells
            {
                ElementalFury       = 60188,
                LightningRod        = 210689,
                LightningRodDamages = 197568,

                Stormkeeper         = 205495,
                ChainLightning      = 188443,
                PetLighningChain    = 191732,
                PetLighningBlast    = 191726
            };

            enum eNpc
            {
                LightningElemental  = 97022
            };

            void HandleBeforeCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        if (Unit* l_Target = l_Player->GetSelectedUnit())
                        {
                            for (Unit* l_Summon : l_Caster->m_Controlled)
                            {
                                if (l_Summon->HasUnitState(UnitState::UNIT_STATE_CASTING))
                                    return;

                                if (l_Summon->m_SpellHelper.GetUint32(eSpellHelpers::LightningElementalSpell) != 0)
                                    return;

                                if (l_Summon->GetEntry() == eNpc::LightningElemental)
                                {
                                    if (GetSpellInfo()->Id == eSpells::ChainLightning)
                                    {
                                        l_Summon->CastSpell(l_Target, eSpells::PetLighningChain, false);
                                        l_Summon->m_SpellHelper.GetUint32(eSpellHelpers::LightningElementalSpell) = eSpells::PetLighningChain;
                                    }
                                    else
                                    {
                                        l_Summon->CastSpell(l_Target, eSpells::PetLighningBlast, false);
                                        l_Summon->m_SpellHelper.GetUint32(eSpellHelpers::LightningElementalSpell) = eSpells::PetLighningBlast;
                                    }
                                    l_Summon->m_SpellHelper.GetUint64(eSpellHelpers::LightningElementalTarget) = l_Target->GetGUID();
                                }
                            }
                        }
                    }
                }
            }

            void HandleOnHit(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_LightningRodSpellInfo = sSpellMgr->GetSpellInfo(eSpells::LightningRod);
                if (!l_Caster || !l_LightningRodSpellInfo)
                    return;

                if (!l_Caster->HasAura(eSpells::LightningRod))
                    return;

                std::set<uint64> l_LightningRodTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::LightningRods];
                for (uint64 l_LightningRodGUID : l_LightningRodTargets)
                {
                    Unit* l_LightningRod = sObjectAccessor->GetUnit(*l_Caster, l_LightningRodGUID);
                    if (!l_LightningRod)
                        continue;

                    int32 l_Bp = int32(GetHitDamage() * (float(l_LightningRodSpellInfo->Effects[EFFECT_1].BasePoints) / 100.0f));

                    if (GetSpell()->IsCritForTarget(l_LightningRod))
                    {
                        float l_CritMultiplier = l_Caster->HasAura(eSpells::ElementalFury) ? 2.5f : 2.0f;

                        l_Bp = int32(GetHitDamage() * l_CritMultiplier * 0.4f);
                    }

                    l_Caster->CastCustomSpell(l_LightningRod, eSpells::LightningRodDamages, &l_Bp, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_lightning_rod_damages_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                BeforeCast += SpellCastFn(spell_sha_lightning_rod_damages_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_lightning_rod_damages_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Lava Burst - 51505
/// Called For Lava Surge - 77756, Path of Flame - 201909
class spell_sha_lava_burst_reset : public SpellScriptLoader
{
    public:
        spell_sha_lava_burst_reset() : SpellScriptLoader("spell_sha_lava_burst_reset") { }

        enum eSpells
        {
            LavaBurst               = 51505,
            PathOfFlameAura         = 201909,
            FlameShock              = 188389,
            PathOfFlameBuff         = 210621,
            LavaSurgeBuff           = 77762,
            LavaSurgeVisual         = 174928,
            ControlOfLava           = 236746,
            EchoOfElements          = 108283,
            PowerOfMaelstrom        = 191861,
            PowerOfMaelstromBuff    = 191877
        };

        class LavaBurstCDReset
        {
            public:

                LavaBurstCDReset(Player* p_Player) : m_Player(p_Player) { }

                void ResetCooldown()
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LavaBurst);
                    if (!m_Player || !l_SpellInfo)
                        return;

                    SpellCategoryEntry const* l_SpellCategory = l_SpellInfo->ChargeCategoryEntry;
                    if (!l_SpellCategory)
                        return;

                    if (m_Player->HasAura(eSpells::EchoOfElements))
                        m_Player->RestoreCharge(l_SpellCategory);
                    else
                        m_Player->ReduceChargeCooldown(l_SpellCategory, l_SpellCategory->ChargeRecoveryTime);
                }

            private:

                Player* m_Player;
        };

        class spell_sha_lava_burst_reset_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lava_burst_reset_SpellScript);

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (auto l_AuraLavaSurge = l_Caster->GetAura(eSpells::LavaSurgeBuff))
                {
                    m_HasLavaSurge = true;
                    l_AuraLavaSurge->Remove();
                    l_Caster->RemoveAura(eSpells::LavaSurgeVisual);
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::PathOfFlameAura) && l_Target->HasAura(eSpells::FlameShock))
                    l_Caster->CastSpell(l_Target, eSpells::PathOfFlameBuff, true);

                l_Caster->RemoveAura(eSpells::ControlOfLava);

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (!m_HasLavaSurge && l_Player->HasAura(eSpells::LavaSurgeBuff))
                    {
                        LavaBurstCDReset l_CDReset(l_Player);
                        l_CDReset.ResetCooldown();
                    }
                }

                if (auto l_AuraPowerOfMaelstorm = l_Caster->GetAura(eSpells::PowerOfMaelstrom))
                {
                    if (roll_chance_i(l_AuraPowerOfMaelstorm->GetSpellInfo()->ProcChance))
                        l_Caster->CastSpell(l_Caster, eSpells::PowerOfMaelstromBuff, true);
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_sha_lava_burst_reset_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_sha_lava_burst_reset_SpellScript::HandleAfterCast);
            }

        private:

            bool m_HasLavaSurge = false;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_lava_burst_reset_SpellScript();
        }
};

/// Ascendance - 114050
/// Called by Lava Burst - 51505
/// Updated as of 7.0.3 - 22522
class spell_sha_ascendance : public SpellScriptLoader
{
    public:
        spell_sha_ascendance() : SpellScriptLoader("spell_sha_ascendance") { }

        class spell_sha_ascendance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_ascendance_SpellScript);

            enum eSpell
            {
                Ascendance  = 114050
            };

            void HandleDamage(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpell::Ascendance, l_Caster->GetGUID()))
                    return;

                SetHitDamage(CalculatePct(GetHitDamage(), 100.0f + l_Caster->GetUnitCriticalChance(WeaponAttackType::BaseAttack, l_Target)));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_ascendance_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_ascendance_SpellScript();
        }
};

/// Doom Winds - 204945
/// Updated as of 7.0.3 - 22522
class spell_sha_doom_winds : public SpellScriptLoader
{
    public:
        spell_sha_doom_winds() : SpellScriptLoader("spell_sha_doom_winds") { }

        class aura_impl : public AuraScript
        {
            PrepareAuraScript(aura_impl);

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& p_Recalculate)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (l_Player->CanApplyPvPSpellModifiers())
                        p_Amount = p_AuraEffect->GetBaseAmount() * 0.60f; ///< Doomwinds effect is reduced by 40% in PvP
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(aura_impl::CalculateAmount, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new aura_impl();
        }
};

/// Rainfall - 215864
/// Updated as of 7.0.3 22522
class spell_sha_rainfall : public SpellScriptLoader
{
    public:
        spell_sha_rainfall() : SpellScriptLoader("spell_sha_rainfall") { }

        enum eSpells
        {
            HealingRainVisual   = 147490,
            RainfallHeal        = 215871,

            LavaLash            = 60103,
            Stormstrike         = 32175
        };

        enum eNPC
        {
            HealingRainTrigger = 73400
        };

        class spell_sha_rainfall_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_rainfall_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                WorldLocation const* l_Dest = GetExplTargetDest();
                if (l_Dest == nullptr)
                    return;

                if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                    l_Trigger->DespawnOrUnsummon();

                if (TempSummon* l_Summon = l_Caster->SummonCreature(eNPC::HealingRainTrigger, *l_Dest, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    l_Summon->SetOwnerGUID(l_Caster->GetGUID());
                    l_Caster->SetHealingRainTrigger(l_Summon->GetGUID());
                    l_Summon->CastSpell(l_Summon, eSpells::HealingRainVisual, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_rainfall_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_rainfall_SpellScript();
        }

        class spell_sha_rainfall_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_rainfall_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                    l_Caster->CastSpell(l_Trigger, eSpells::RainfallHeal, true);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                    l_Trigger->DespawnOrUnsummon();
            }

            void HandleOnProc(AuraEffect const* p_AuraEffect, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                SpellInfo const* l_ProcSpell = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (l_ProcSpell == nullptr || (l_ProcSpell->Id != eSpells::LavaLash && l_ProcSpell->Id != eSpells::Stormstrike))
                    return;

                Aura* l_Aura = p_AuraEffect->GetBase();

                int32 l_Duration        = p_AuraEffect->GetAmount() * TimeConstants::IN_MILLISECONDS;
                int32 l_MaxDuration     = l_Aura->GetEffect(EFFECT_3)->GetAmount() * TimeConstants::IN_MILLISECONDS;
                int32 l_NewDuration     = std::min(l_Aura->GetDuration() + l_Duration, l_MaxDuration);
                int32 l_NewMaxDuration  = std::min(l_Aura->GetMaxDuration() + l_Duration, l_MaxDuration);

                l_Aura->SetMaxDuration(l_NewMaxDuration);
                l_Aura->SetDuration(l_NewDuration);
                l_Aura->SetNeedClientUpdateForTargets();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_rainfall_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_rainfall_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectProc += AuraEffectProcFn(spell_sha_rainfall_AuraScript::HandleOnProc, EFFECT_2, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_rainfall_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Ghost Wolf - 2645
/// Called for Spectral Recovery - 204261
class spell_sha_spectral_recovery : public SpellScriptLoader
{
    public:
        spell_sha_spectral_recovery() : SpellScriptLoader("spell_sha_spectral_recovery") { }

        class spell_sha_spectral_recovery_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_sha_spectral_recovery_Aurascript);

            enum eSpells
            {
                SpectralRecovery        = 204261,
                SpectralRecoveryEffect  = 204262,
                SpiritualJourney        = 214170,
            };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::SpectralRecovery))
                    return;

                if (!l_Caster->HasAura(eSpells::SpectralRecoveryEffect))
                    l_Caster->CastSpell(l_Caster, eSpells::SpectralRecoveryEffect);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SpectralRecoveryEffect);
                l_Caster->RemoveAura(eSpells::SpiritualJourney);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sha_spectral_recovery_Aurascript::AfterApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_spectral_recovery_Aurascript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_spectral_recovery_Aurascript();
        }
};

/// Spectral Recovery - 204261
class PlayerScript_spectral_recovery : public PlayerScript
{
    public:
        PlayerScript_spectral_recovery() : PlayerScript("PlayerScript_spectral_recovery") {}

        enum eSpells
        {
            SpectralRecoveryEffect = 204262,
            SpiritualJourney = 214170,
        };

        void OnUpdateZone(Player* p_Player, uint32 /*p_NewZoneID*/, uint32 /*p_OldZoneID*/, uint32 /*p_NewAreaID*/)
        {
            if (p_Player->IsInWorld() && !p_Player->GetMap()->IsBattlegroundOrArena())
            {
                p_Player->RemoveAurasDueToSpell(eSpells::SpectralRecoveryEffect);
                p_Player->RemoveAura(eSpells::SpiritualJourney);
            }
        }

        void OnLeaveCombat(Player* p_Player)
        {
            if (p_Player->IsInWorld() && !p_Player->IsInPvPCombat() && !p_Player->GetMap()->IsBattlegroundOrArena())
            {
                p_Player->RemoveAurasDueToSpell(eSpells::SpectralRecoveryEffect);
                p_Player->RemoveAura(eSpells::SpiritualJourney);
            }
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Healing Wave - 77472, Healing Surge - 8004
/// Called For Undulation - 200071
class spell_sha_undulation : public SpellScriptLoader
{
    public:
        spell_sha_undulation() : SpellScriptLoader("spell_sha_undulation") { }

        class spell_sha_undulation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_undulation_SpellScript);

            enum eSpells
            {
                Undulation       = 200071,
                UndulationHeal   = 216251,
                T21Restoration4P = 251765,
                HealingRain      = 73920,
                Downpour         = 252159
            };

            enum eNPC
            {
                HealingRainTrigger = 73400
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::Undulation);
                if (!l_Aura)
                    return;

                uint32& l_Counter = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::UndulationCounter);
                l_Counter++;

                if (l_Counter >= 3)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::UndulationHeal, true);
                    l_Counter = 0;
                }
            }

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::T21Restoration4P))
                    return;

                SpellInfo const* l_4PSpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Restoration4P);
                if (!l_4PSpellInfo)
                    return;

                int32 l_Heal = CalculatePct(GetHitHeal(), l_4PSpellInfo->Effects[EFFECT_0].BasePoints);

                if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                    l_Caster->CastCustomSpell(l_Trigger, eSpells::Downpour, &l_Heal, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_undulation_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_sha_undulation_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_undulation_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Counterstrike Totem - 208997
class spell_sha_counterstrike_totem : public SpellScriptLoader
{
    public:
        spell_sha_counterstrike_totem() : SpellScriptLoader("spell_sha_counterstrike_totem") { }

        class spell_sha_counterstrike_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_counterstrike_totem_AuraScript);

            enum eSpells
            {
                CounterstrikeTotem              = 204331,
                CounterstrikeTotemDamage        = 209031,
                CounterstrikeTotemAreatrigger   = 208990
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                SpellInfo const* l_CounterstrikeTotem = sSpellMgr->GetSpellInfo(eSpells::CounterstrikeTotem);
                if (!l_DamageInfo || !l_Caster || !l_Target || !l_CounterstrikeTotem)
                    return;

                int32 l_Damage = l_DamageInfo->GetAmount();
                if (!l_Damage)
                    return;

                uint32 l_DamageCup = 1000000; // 1M cup

                if (l_Damage > l_DamageCup)
                    l_Damage = l_DamageCup;

                if (l_Damage + l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::CounterstrikeStoredDamage) > l_DamageCup)
                    l_Damage = l_DamageCup - l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::CounterstrikeStoredDamage);

                if (l_Damage > 0)
                {
                    l_Caster->CastCustomSpell(l_Target, eSpells::CounterstrikeTotemDamage, &l_Damage, nullptr, nullptr, true);

                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::CounterstrikeStoredDamage) += l_Damage;
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_sha_counterstrike_totem_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_counterstrike_totem_AuraScript();
        }
};

/// Update Legion 7.1.5 build 23420
/// Called by Control Of Lava - 204393
class spell_sha_control_of_lava : public SpellScriptLoader
{
    public:
        spell_sha_control_of_lava() : SpellScriptLoader("spell_sha_control_of_lava") { }

        class spell_sha_control_of_lava_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_control_of_lava_AuraScript);

            enum eSpells
            {
                FlameShock    = 188389,
                ControlOfLava = 236746,
                LavaBurst     = 51505
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::FlameShock)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                l_Owner->CastSpell(l_Owner, eSpells::ControlOfLava, true);
                Player* l_Player = l_Owner->ToPlayer();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LavaBurst);
                if (!l_Player || !l_SpellInfo)
                    return;

                l_Player->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_control_of_lava_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_sha_control_of_lava_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_control_of_lava_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Healing Stream Totem - 5394
/// Called for Caress of the Tidemother - 207354
class spell_sha_caress_of_the_tidemother : public SpellScriptLoader
{
    public:
        spell_sha_caress_of_the_tidemother() : SpellScriptLoader("spell_sha_caress_of_the_tidemother") { }

        class spell_sha_caress_of_the_tidemother_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_caress_of_the_tidemother_SpellScript);

            enum eSpells
            {
                CaressOfTheTidemother       = 207354,
                CaressOfTheTidemotherBuff   = 209950
            };

            enum eArtifactPowers
            {
                CaressOfTheTidemotherArtifact = 1111
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CaressOfTheTidemotherBuff);
                if (!l_Caster || !l_SpellInfo || !l_Caster->HasAura(eSpells::CaressOfTheTidemother))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::CaressOfTheTidemotherArtifact);
                if (!l_Rank)
                    return;

                int32 l_Amount = 0;

                switch (l_Rank)
                {
                    case 1:
                        l_Amount = -3;
                        break;
                    case 2:
                        l_Amount = -6;
                        break;
                    case 3:
                        l_Amount = -10;
                        break;
                    default:
                        l_Amount = 0;
                }

                l_Caster->CastCustomSpell(l_Caster, eSpells::CaressOfTheTidemotherBuff, &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_sha_caress_of_the_tidemother_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_caress_of_the_tidemother_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Caress of the Tidemother - 207354
class spell_sha_caress_of_the_tidemother_checkproc : public SpellScriptLoader
{
public:
    spell_sha_caress_of_the_tidemother_checkproc() : SpellScriptLoader("spell_sha_caress_of_the_tidemother_checkproc") { }

    class spell_sha_caress_of_the_tidemother_checkproc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_caress_of_the_tidemother_checkproc_AuraScript);

        enum eSpells
        {
            HealingStreamTotem = 5394
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_DamageInfo || !l_SpellInfo || l_SpellInfo->Id != eSpells::HealingStreamTotem)
                return false;
            else
                return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_caress_of_the_tidemother_checkproc_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_caress_of_the_tidemother_checkproc_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Liquid Magma Totem - 192223
class spell_sha_liquid_magma_totem : public SpellScriptLoader
{
public:
    spell_sha_liquid_magma_totem() : SpellScriptLoader("spell_sha_liquid_magma_totem") { }

    class spell_sha_liquid_magma_totem_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_liquid_magma_totem_SpellScript);

        enum eSpells
        {
            LiquidMagmaDamage = 192231
        };

        void HandleOnEffect(SpellEffIndex)
        {
            if (m_alreadyHit)
                return;
            m_alreadyHit = true;
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Target = GetHitUnit();

            l_Caster->CastSpell(l_Target, eSpells::LiquidMagmaDamage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_sha_liquid_magma_totem_SpellScript::HandleOnEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }

        bool m_alreadyHit;

    public:
        spell_sha_liquid_magma_totem_SpellScript() : m_alreadyHit(false) {}
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sha_liquid_magma_totem_SpellScript();
    }
};

/// Last Update 7.0.3 Build 22522
/// Called by Swelling Waves - 204264
class spell_sha_swelling_waves : public SpellScriptLoader
{
    public:
        spell_sha_swelling_waves() : SpellScriptLoader("spell_sha_swelling_waves") { }

        class spell_sha_swelling_waves_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_swelling_waves_AuraScript);

            enum eSpells
            {
                HealingSurge        = 8004,
                SwellingWavesHeal   = 204266
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_DamageInfo || !l_SpellInfo)
                    return;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Actor = l_DamageInfo->GetActor();
                Unit* l_Target = l_DamageInfo->GetTarget();
                int32 l_Amount = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_1].BasePoints);
                if (!l_ProcSpellInfo || !l_Actor || !l_Target || l_ProcSpellInfo->Id != eSpells::HealingSurge || l_Actor != l_Target)
                    return;

                l_Actor->CastCustomSpell(l_Actor, eSpells::SwellingWavesHeal, &l_Amount, nullptr, nullptr, true, nullptr, nullptr, l_Actor->GetGUID(), l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_sha_swelling_waves_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_swelling_waves_AuraScript();
        }
};

enum eEarthenRageSpells
{
    EarthenRageDamage   = 170379,
    EarthenRagePeriodic = 170377,
    EarthenRageAura     = 170374
};

/// Earthen Rage - 170374
/// Updated as of 7.0.3 - 22522
class spell_sha_earthen_rage : public SpellScriptLoader
{
    public:
        spell_sha_earthen_rage() : SpellScriptLoader("spell_sha_earthen_rage") { }

        class spell_sha_earthen_rage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthen_rage_AuraScript);

            void OnProc(AuraEffect const*, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                if (p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id == eEarthenRageSpells::EarthenRageDamage)
                    return;

                /// If spell doesn't deal damage it can't trigger Earthen Shield
                if (p_EventInfo.GetDamageInfo()->GetAmount() == 0 && p_EventInfo.GetDamageInfo()->GetAbsorb() == 0)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetDamageInfo()->GetTarget();
                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eEarthenRageSpells::EarthenRagePeriodic, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_earthen_rage_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_earthen_rage_AuraScript();
        }
};

/// Earthen Rage Periodic - 170377
/// Updated as of 7.0.3 - 22522
class spell_sha_earthen_rage_periodic : public SpellScriptLoader
{
    public:
        spell_sha_earthen_rage_periodic() : SpellScriptLoader("spell_sha_earthen_rage_periodic") { }

        class spell_sha_earthen_rage_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthen_rage_periodic_AuraScript);

            void HandleEffectPeriodic(AuraEffect const*)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint64 l_TargetGuid = l_Caster->GetLastDamagedGuid();
                if (Unit* l_Target = sObjectAccessor->FindUnit(l_TargetGuid))
                    l_Caster->CastSpell(l_Target, eEarthenRageSpells::EarthenRageDamage, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthen_rage_periodic_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_earthen_rage_periodic_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Chain Heal - 1064
/// Called for Deluge - 200076
class spell_sha_deluge : public SpellScriptLoader
{
    public:
        spell_sha_deluge() : SpellScriptLoader("spell_sha_deluge") { }

        class spell_sha_deluge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_deluge_SpellScript);

            enum eSpells
            {
                Riptide = 61295,
                DelugeTalent = 200076,
                DelugeAura = 200075
            };

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::DelugeTalent))
                    return;

                uint32 l_Heal = GetHitHeal();
                if (AuraEffect const* l_AuraDeluge = l_Caster->GetAuraEffect(eSpells::DelugeTalent, EFFECT_0))
                {
                    /// If the target is affected by Riptide - 61295
                    if (l_Target->HasAura(eSpells::DelugeAura) || l_Target->HasAura(eSpells::Riptide))
                        l_Heal += CalculatePct(l_Heal, l_AuraDeluge->GetAmount());
                    /// If the target is affected by Healing Rain - 73920
                    if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                        l_Heal += CalculatePct(l_Heal, l_AuraDeluge->GetAmount());
                }
                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_deluge_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_deluge_SpellScript;
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Chain Heal - 1064, Riptide - 61295, HealingWave - 77472, HealingSurge - 8004, UnleashLife - 73685
class spell_sha_queen_ascendant : public SpellScriptLoader
{
    public:
        spell_sha_queen_ascendant() : SpellScriptLoader("spell_sha_queen_ascendant") { }

        class spell_sha_queen_ascendant_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_queen_ascendant_SpellScript);

            enum eSpells
            {
                QueenAscendantProc = 207288
            };

            enum eArtifactPowers
            {
                QueenAscendantArtifact = 1108
            };

            void HandleHealHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                if (!GetSpell()->IsCritForTarget(l_Target))
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    uint32 l_TraitRank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::QueenAscendantArtifact);
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::QueenAscendantProc);

                    if (!l_SpellInfo)
                        return;

                    int32 l_BasePoint = l_SpellInfo->Effects[0].BasePoints * l_TraitRank;

                    l_Caster->CastCustomSpell(l_Caster, eSpells::QueenAscendantProc, &l_BasePoint, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sha_queen_ascendant_SpellScript::HandleHealHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_queen_ascendant_SpellScript;
        }
};

/// Last Update 7.3.5 Build 26124
/// Called by Volcanic Inferno - 192630
class spell_sha_volcanic_inferno : public SpellScriptLoader
{
    public:
        spell_sha_volcanic_inferno() : SpellScriptLoader("spell_sha_volcanic_inferno") { }

        class spell_sha_volcanic_inferno_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_volcanic_inferno_AuraScript);

            enum eSpells
            {
                VolcanicInfernoAreaTrigger = 205532,
                LavaBurst                  = 51505
            };

            void HandleProc(AuraEffect const*, ProcEventInfo& p_Info)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_Info.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::VolcanicInfernoAreaTrigger, true);
            }

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();

                if (!l_ProcSpellInfo)
                    return false;

                if (l_ProcSpellInfo->Id != eSpells::LavaBurst)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_volcanic_inferno_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_sha_volcanic_inferno_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_volcanic_inferno_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Unleash Doom - 199055
class spell_sha_unleash_doom : public SpellScriptLoader
{
    public:
        spell_sha_unleash_doom() : SpellScriptLoader("spell_sha_unleash_doom") { }

        class spell_sha_unleash_doom_AuraScript : public AuraScript

        {
            PrepareAuraScript(spell_sha_unleash_doom_AuraScript);

            enum eSpells
            {
                UnleashLava         = 199053,
                UnleashLightning    = 199054
            };

            enum eWhitelistSpells
            {
                BoulderFist             = 201897,
                CrashLightning          = 187874,
                CrashLightningSecondary = 195592,
                Flametongue             = 193796,
                Frostband               = 196834,
                Rockbiter               = 193786,
                Windsong                = 201898,
                Sundering               = 197214,
                EarthenSpike            = 188089,
                LavaLeash               = 60103,
                StormStrike             = 32175,
                StormStrikeOff          = 32176,
                WindstrikeMain          = 115357,
                WindstrikeOff           = 115360
            };

            void HandleProc(AuraEffect const*, ProcEventInfo& p_Info)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_Info.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, urand(0, 1) ? eSpells::UnleashLava : eSpells::UnleashLightning, true);
            }

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
                    case eWhitelistSpells::BoulderFist:
                    case eWhitelistSpells::CrashLightning:
                    case eWhitelistSpells::CrashLightningSecondary:
                    case eWhitelistSpells::Flametongue:
                    case eWhitelistSpells::Frostband:
                    case eWhitelistSpells::Rockbiter:
                    case eWhitelistSpells::Windsong:
                    case eWhitelistSpells::Sundering:
                    case eWhitelistSpells::EarthenSpike:
                    case eWhitelistSpells::LavaLeash:
                    case eWhitelistSpells::StormStrike:
                    case eWhitelistSpells::StormStrikeOff:
                    case eWhitelistSpells::WindstrikeMain:
                    case eWhitelistSpells::WindstrikeOff:
                        return true;
                    default:
                        return false;
                }

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_unleash_doom_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_sha_unleash_doom_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_unleash_doom_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Unleash Doom - 198736
class spell_sha_unleash_doom_proc : public SpellScriptLoader
{
    public:
        spell_sha_unleash_doom_proc() : SpellScriptLoader("spell_sha_unleash_doom_proc") { }

        class spell_sha_unleash_doom_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_unleash_doom_proc_AuraScript);

            enum eWhitelistSpells
            {
                StormStrike             = 32175,
                StormStrikeOff          = 32176,
                WindStrike              = 115357,
                WindStrikeOff           = 115360
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

                if (l_ProcSpellInfo->Id == eWhitelistSpells::StormStrike || l_ProcSpellInfo->Id == eWhitelistSpells::StormStrikeOff
                    || l_ProcSpellInfo->Id == eWhitelistSpells::WindStrike || l_ProcSpellInfo->Id == eWhitelistSpells::WindStrikeOff)
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_unleash_doom_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_unleash_doom_proc_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Storkeeper - 205495
class spell_sha_stormkeeper : public SpellScriptLoader
{
    public:
        spell_sha_stormkeeper() : SpellScriptLoader("spell_sha_stormkeeper") { }

        class spell_sha_stormkeeper_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_stormkeeper_AuraScript);

            enum eSpells
            {
                LightningBolt = 188196,
                ChainLightning = 188443
            };

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.50f;   ///< Storkeeper bonus damage is reduced by 50% in PvP
            }



            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_stormkeeper_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        class spell_sha_stormkeeper_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_stormkeeper_SpellScript);

            enum eSpells
            {
                StaticOverload = 191602,
                StaticOverloadTrigger = 191634
            };

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::StaticOverload))
                        l_Caster->CastSpell(l_Caster, eSpells::StaticOverloadTrigger, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_stormkeeper_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_stormkeeper_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_stormkeeper_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Doom Vortex - 199107
class spell_sha_doom_vortex : public SpellScriptLoader
{
    public:
        spell_sha_doom_vortex() : SpellScriptLoader("spell_sha_doom_vortex") { }

        class spell_sha_doom_vortex_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_doom_vortex_AuraScript);

            enum eSpells
            {
                DoomVortexAreaTrigger   = 199121,
                LightningBolt           = 187837,
                Overcharge              = 210727
            };

            void HandleOnProc(AuraEffect const*, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetActionTarget();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();

                if (!l_Caster || !l_Target || !l_DamageInfo)
                    return;

                if (l_DamageInfo->GetSpellInfo() && l_DamageInfo->GetSpellInfo()->Id == eSpells::LightningBolt)
                {
                    if (!l_Caster->HasAura(eSpells::Overcharge))
                        return;
                }

                l_Caster->CastSpell(l_Target, eSpells::DoomVortexAreaTrigger, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_doom_vortex_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_doom_vortex_AuraScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by IceFury - 210714
class spell_sha_icefury : public SpellScriptLoader
{
    public:
        spell_sha_icefury() : SpellScriptLoader("spell_sha_icefury") { }

        class spell_sha_icefury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_icefury_SpellScript);

            enum eSpells
            {
                EarthenStrength = 252141
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::EarthenStrength);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_sha_icefury_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_icefury_SpellScript();
        }

        class spell_sha_icefury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_icefury_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.625f;   ///< Icefury bonus damage for frost shock is reduced by 37.5% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_icefury_AuraScript::HandlePvPModifier, EFFECT_2, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_icefury_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Calming Waters - 221678
class PlayerScript_Calming_Waters : public PlayerScript
{
    public:
        PlayerScript_Calming_Waters() :PlayerScript("PlayerScript_Calming_Waters") { }

        enum eSpell
        {
            CalmingWaters       = 221678,
            CalmingWatersBuff   = 221677
        };

        void OnSpellInterrupt(Player* p_Player, Spell* p_Spell, Spell* p_InterruptingSpell, uint32 /*p_InterruptingSpellId*/)
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_SHAMAN_RESTORATION)
                return;

            if (!p_Player->HasAura(eSpell::CalmingWaters))
                return;

            if (!p_InterruptingSpell || !p_InterruptingSpell->GetSpellInfo()->HasEffectMechanic(Mechanics::MECHANIC_INTERRUPT))
                return;

            if (!p_Player->HasAura(eSpell::CalmingWatersBuff))
                p_Player->CastSpell(p_Player, eSpell::CalmingWatersBuff, true);
        }
};

/// Last Update: 7.1.5 23420
/// Called by Hex - 196942
/// Called for Voodoo Totem - 196932
class spell_sha_voodoo_hex : public SpellScriptLoader
{
    public:
        spell_sha_voodoo_hex() : SpellScriptLoader("spell_sha_voodoo_hex") { }

        enum eSpell
        {
            SpellHexhaustion = 202318
        };

        class spell_sha_voodoo_hex_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_voodoo_hex_AuraScript);

            void HandleOnRemove(AuraEffect const* l_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpell::SpellHexhaustion, true);
                l_Target->IncrDiminishing(DiminishingGroup::DIMINISHING_INCAPACITATE);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_voodoo_hex_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_voodoo_hex_AuraScript();
        }

        class spell_sha_voodoo_hex_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_voodoo_hex_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                if (l_Target->HasAura(eSpell::SpellHexhaustion))
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_sha_voodoo_hex_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_voodoo_hex_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Spiritual Journey - 214147
class spell_sha_spiritual_journey : public SpellScriptLoader
{
    public:
        spell_sha_spiritual_journey() : SpellScriptLoader("spell_sha_spiritual_journey") { }

        class spell_sha_spiritual_journey_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_spiritual_journey_AuraScript);

            enum eSpells
            {
                Wolf = 2645,
                SpiritualJourney = 214170
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo)
                    return false;

                if (l_ProcSpellInfo->Id != eSpells::Wolf)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SpiritualJourney, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_spiritual_journey_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_sha_spiritual_journey_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_spiritual_journey_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Lava Beam - 114074, and Lava Beam Overloaded - 114738
class spell_sha_lava_beam : public SpellScriptLoader
{
    public:
        spell_sha_lava_beam() : SpellScriptLoader("spell_sha_lava_beam") { }

        class spell_sha_lava_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lava_beam_SpellScript);

            void HandleOnHit(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_CustomAmount = GetSpell()->GetSpellValue(SPELLVALUE_BASE_POINT2);
                l_Caster->ModifyPower(POWER_MAELSTROM, l_CustomAmount);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_lava_beam_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_lava_beam_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Storm Tempests - 214260
class spell_sha_storm_tempests : public SpellScriptLoader
{
    public:
        spell_sha_storm_tempests() : SpellScriptLoader("spell_sha_storm_tempests") { }

        class spell_sha_storm_tempests_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_storm_tempests_AuraScript);

            enum eSpells
            {
                StormTempests   = 214265,
                StormStrike     = 32175
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::StormStrike)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_ProcTarget = l_DamageInfo->GetTarget();
                if (!l_ProcTarget)
                    return;

                l_Caster->CastSpell(l_ProcTarget, eSpells::StormTempests, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_storm_tempests_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_sha_storm_tempests_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_storm_tempests_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Storm Tempests_periodic - 214265
class spell_sha_storm_tempests_periodic : public SpellScriptLoader
{
    public:
        spell_sha_storm_tempests_periodic() : SpellScriptLoader("spell_sha_storm_tempests_periodic") { }

        class spell_sha_storm_tempests_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_storm_tempests_periodic_AuraScript);

            enum eSpells
            {
                StormTempestsDamage = 214452,
                StormTempestArea    = 214273
            };

            void HandleOnPeriodic(AuraEffect const*  /* aurEff */)
            {
                Unit* l_MainTarget = GetTarget();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_StormTempestInfo = sSpellMgr->GetSpellInfo(eSpells::StormTempestArea);
                if (!l_Caster || !l_MainTarget || !l_StormTempestInfo)
                    return;

                SpellRadiusEntry const* l_RadiusEntry = l_StormTempestInfo->Effects[EFFECT_1].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                float l_Radius = l_RadiusEntry->RadiusMax;
                std::list<Unit*> l_StormTempestAllies;
                JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(l_MainTarget, l_MainTarget, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(l_MainTarget, l_StormTempestAllies, u_check);
                l_MainTarget->VisitNearbyObject(l_Radius, searcher);
                for (auto l_Target : l_StormTempestAllies)
                {
                    if ((l_MainTarget != l_Target && !l_Target->IsFriendlyTo(l_Caster)) ||
                        (l_MainTarget->IsPvP() && l_Target->IsPvP() && l_MainTarget != l_Target))
                    {
                        l_Caster->CastSpell(l_Target, StormTempestsDamage, true);
                        return;
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_storm_tempests_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_storm_tempests_periodic_AuraScript();
        }
};

/// Called by Echoes of the Great Sundering - 208722
/// Called for Echoes of the Great Sundering (item) - 137074
class spell_sha_echoes_of_the_great_sundering : public SpellScriptLoader
{
    public:
        spell_sha_echoes_of_the_great_sundering() : SpellScriptLoader("spell_sha_echoes_of_the_great_sundering") { }

        class spell_sha_echoes_of_the_great_sundering_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_echoes_of_the_great_sundering_AuraScript);

            enum eSpells
            {
                EchoesOfTheGreatSunderingAura = 208722,
                EchoesOfTheGreatSunderingBuff = 208723,
                EarthShock                    = 8042
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfoEchoesOfTheGreatSunderingAura = sSpellMgr->GetSpellInfo(eSpells::EchoesOfTheGreatSunderingAura);
                if (!l_Caster || !l_DamageInfo || !l_SpellInfoEchoesOfTheGreatSunderingAura)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::EarthShock)
                    return false;

                float l_Chance = (l_SpellInfoEchoesOfTheGreatSunderingAura->Effects[EFFECT_0].BasePoints / 100.0f) * l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::EarthShockPowerSpentAndEarthquakes);
                if (!roll_chance_f(l_Chance))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::EchoesOfTheGreatSunderingBuff, true);
                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::EarthShockPowerSpentAndEarthquakes) = true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_echoes_of_the_great_sundering_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_sha_echoes_of_the_great_sundering_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_echoes_of_the_great_sundering_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Hex - 51514 and Hex (Voodoo Totem) - 196942
/// Called by Hex Variants - 210873, 211015, 211004, 211010
class spell_sha_hex : public SpellScriptLoader
{
    public:
        spell_sha_hex() : SpellScriptLoader("spell_sha_hex") { }

        class spell_sha_hex_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_hex_AuraScript);

            enum eSpells
            {
                HexNoActionAura = 93958,
                HexVoodooTotem  = 196942
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        int32 l_Duration = p_AurEff->GetBase()->GetDuration();

                        l_Caster->CastSpell(l_Target, eSpells::HexNoActionAura, true);

                        if (Aura* l_Aura = l_Target->GetAura(eSpells::HexNoActionAura, l_Caster->GetGUID()))
                        {
                            l_Aura->SetDuration(l_Duration);
                            l_Aura->SetMaxDuration(l_Duration);
                        }
                    }
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpells::HexNoActionAura);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::HexVoodooTotem)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_sha_hex_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_sha_hex_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                }
                else
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_sha_hex_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_sha_hex_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_hex_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Spirit Link (PvP Talent) - 204293
class spell_sha_spirit_link_aura : public SpellScriptLoader
{
    public:
        spell_sha_spirit_link_aura() : SpellScriptLoader("spell_sha_spirit_link_aura") { }

        class spell_sha_spirit_link_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_spirit_link_aura_AuraScript);

            enum eSpell
            {
                SpiritLinkDamage = 204314
            };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        if (l_Caster == l_Target)
                            return;

                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::SpiritLinkPvPTalent].insert(l_Target->GetGUID());
                    }
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        if (l_Caster == l_Target)
                            return;

                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::SpiritLinkPvPTalent].erase(l_Target->GetGUID());
                    }
                }
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        if (p_DmgInfo.GetSpellInfo() && p_DmgInfo.GetSpellInfo()->Id == eSpell::SpiritLinkDamage)
                            return;

                        /// Redirects 50% of the damage on linked targets
                        int32 l_Amount = CalculatePct(p_DmgInfo.GetAmount(), GetSpellInfo()->Effects[EFFECT_1].BasePoints);

                        p_AbsorbAmount = l_Amount;

                        std::set<uint64> l_RedirectTargets;
                        for (uint64 l_Guid : l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::SpiritLinkPvPTalent])
                        {
                            if (l_Target->GetGUID() == l_Guid)
                                continue;

                            l_RedirectTargets.insert(l_Guid);
                        }

                        if (l_Target != l_Caster)
                            l_RedirectTargets.insert(l_Caster->GetGUID());

                        if (!l_RedirectTargets.empty())
                            l_Amount /= int32(l_RedirectTargets.size());

                        for (uint64 l_Guid : l_RedirectTargets)
                        {
                            if (Unit* l_Unit = Unit::GetUnit(*l_Target, l_Guid))
                                l_Target->CastCustomSpell(l_Unit, eSpell::SpiritLinkDamage, &l_Amount, nullptr, nullptr, true);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sha_spirit_link_aura_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_spirit_link_aura_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_spirit_link_aura_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_spirit_link_aura_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_spirit_link_aura_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Stormstrike Mainhand - 32175, Stormstrike Mainhand Offhand - 32176, WindstrikeMainHand - 115357
class spell_sha_stormstrike_damage: public SpellScriptLoader
{
    public:
        spell_sha_stormstrike_damage() : SpellScriptLoader("spell_sha_stormstrike_damage") { }

        enum eSpells
        {
            Stormflurry         = 198367,
            StormStrike         = 17364,
            StormStrikeMain     = 32175,
            StormStrikeOff      = 32176,
            StormStrikeDummy    = 195573,
            Windstrike          = 115356,
            WindstrikeMain      = 115357,
            WindstrikeOff       = 115360,
            WindfuryProc        = 33757,
            WindfuryAttack      = 25504
        };

        class spell_sha_stormstrike_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_stormstrike_damage_SpellScript);

            void WindfuryProc(Unit* p_Caster, Unit* p_Target)
            {
                if (m_scriptSpellId == eSpells::StormStrikeOff)
                    return;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WindfuryProc))
                {
                    uint32 l_ProcChance = l_SpellInfo->ProcChance;
                    float l_AddProc = p_Caster->ToPlayer()->GetFloatValue(PLAYER_FIELD_MASTERY) * 2.0f / 25.0f;
                    l_ProcChance += (uint32)l_AddProc;

                    if (roll_chance_i(l_ProcChance))
                        for (uint8 l_I = 0; l_I < 2; l_I++)
                            p_Caster->CastSpell(p_Target, eSpells::WindfuryAttack, true);
                }
            }

            void StormflurryProc(Unit* p_Caster, Unit* p_Target)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Stormflurry);

                if (l_SpellInfo == nullptr || !p_Caster->HasAura(eSpells::Stormflurry))
                    return;

                if (roll_chance_i(l_SpellInfo->Effects[0].BasePoints))
                {
                    if (m_scriptSpellId == eSpells::StormStrikeMain)
                    {
                        SpellInfo const* l_StormStrikeInfo = sSpellMgr->GetSpellInfo(eSpells::StormStrikeMain);
                        SpellInfo const* l_StormStrikeOffInfo = sSpellMgr->GetSpellInfo(eSpells::StormStrikeOff);
                        if (!l_StormStrikeInfo || !l_StormStrikeOffInfo)
                            return;

                        int32 l_Bp0 = CalculatePct(l_StormStrikeInfo->Effects[0].BasePoints, l_SpellInfo->Effects[1].BasePoints);
                        int32 l_Bp0Off = CalculatePct(l_StormStrikeOffInfo->Effects[0].BasePoints, l_SpellInfo->Effects[1].BasePoints);

                        /// Marked that Storm Strike is triggered by Stormflurry
                        constexpr int32 l_Bp5 = 1;

                        p_Caster->CastCustomSpell(p_Target, eSpells::StormStrikeMain, &l_Bp0, nullptr, nullptr, nullptr, nullptr, &l_Bp5, true);
                        p_Caster->CastCustomSpell(p_Target, eSpells::StormStrikeOff, &l_Bp0Off, nullptr, nullptr, nullptr, nullptr, &l_Bp5, true);
                        p_Caster->CastSpell(p_Caster, eSpells::StormStrikeDummy, true);
                    }
                    else if (m_scriptSpellId == eSpells::WindstrikeMain)
                    {
                        SpellInfo const* l_WindStrikeInfo = sSpellMgr->GetSpellInfo(eSpells::WindstrikeMain);
                        SpellInfo const* l_WindStrikeOffInfo = sSpellMgr->GetSpellInfo(eSpells::WindstrikeOff);
                        if (!l_WindStrikeInfo || !l_WindStrikeOffInfo)
                            return;

                        int32 l_Bp0 = CalculatePct(l_WindStrikeInfo->Effects[0].BasePoints, l_SpellInfo->Effects[1].BasePoints);
                        int32 l_Bp0Off = CalculatePct(l_WindStrikeOffInfo->Effects[0].BasePoints, l_SpellInfo->Effects[1].BasePoints);

                        p_Caster->CastCustomSpell(p_Target, eSpells::WindstrikeMain, &l_Bp0, nullptr, nullptr, true);
                        p_Caster->CastCustomSpell(p_Target, eSpells::WindstrikeOff, &l_Bp0Off, nullptr, nullptr, true);
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->ToPlayer())
                    return;

                WindfuryProc(l_Caster, l_Target);
                StormflurryProc(l_Caster, l_Target);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_stormstrike_damage_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_stormstrike_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Stormstrike - 17364
class spell_sha_stormstrike : public SpellScriptLoader
{
    public:
        spell_sha_stormstrike() : SpellScriptLoader("spell_sha_stormstrike") { }

        enum eSpells
        {
            GatheringStormsProc = 198300,
            ForkedLightning     = 204349,
            ForkedLightningDmg  = 204350
        };

        class spell_sha_stormstrike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_stormstrike_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster)
                    l_Caster->RemoveAurasDueToSpell(eSpells::GatheringStormsProc);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::ForkedLightning))
                    l_Caster->CastSpell(l_Caster, eSpells::ForkedLightningDmg, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_sha_stormstrike_SpellScript::HandleAfterHit);
                AfterCast += SpellCastFn(spell_sha_stormstrike_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_stormstrike_SpellScript();
        }
};

/// Called by Ghost Wolf - 2645
class spell_sha_ghost_wolf : public SpellScriptLoader
{
    public:
        spell_sha_ghost_wolf() : SpellScriptLoader("spell_sha_ghost_wolf") { }

        class spell_sha_ghost_wolf_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ghost_wolf_AuraScript);

            enum eSpells
            {
                GhostInTheMistTrait             = 207351,
                GhostInTheMistPeriodic          = 207524,
                GhostInTheMistBuff              = 207527,
                SpiritOfTheMaelstorm            = 198238,
                SpiritOfTheMaelstormAura        = 198240,
                GlyphOfTheSpectralWolf          = 58135
            };

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::GhostInTheMistTrait))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::GhostInTheMistPeriodic, true);
                }

                if (l_Caster->HasAura(eSpells::SpiritOfTheMaelstorm))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::SpiritOfTheMaelstormAura, true);
                }

                if (l_Caster->IsPlayer() && l_Caster->HasAura(eSpells::GlyphOfTheSpectralWolf))
                {
                    l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, nullptr);
                    l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, nullptr);
                }
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::GhostInTheMistPeriodic))
                {
                    l_Caster->RemoveAurasDueToSpell(eSpells::GhostInTheMistPeriodic);
                    l_Caster->RemoveAurasDueToSpell(eSpells::GhostInTheMistBuff);
                }

                if (l_Caster->IsPlayer() && l_Caster->HasAura(eSpells::GlyphOfTheSpectralWolf))
                {
                    if (Item* l_Item = l_Caster->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                        l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, l_Item);
                    else
                        l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, nullptr);
                    if (Item* l_Item = l_Caster->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                        l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, l_Item);
                    else
                        l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, nullptr);
                }

                l_Caster->RemoveAura(eSpells::SpiritOfTheMaelstormAura);
            }

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                if (l_Caster->ToPlayer()->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + EQUIPMENT_SLOT_MAINHAND * 2) == 0 &&
                    l_Caster->ToPlayer()->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + EQUIPMENT_SLOT_OFFHAND * 2) == 0)
                    return;

                if (l_Caster->HasAura(eSpells::GlyphOfTheSpectralWolf))
                {
                    l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, nullptr);
                    l_Caster->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, nullptr);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sha_ghost_wolf_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_ghost_wolf_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
                OnEffectUpdate += AuraEffectUpdateFn(spell_sha_ghost_wolf_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_ghost_wolf_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Stormlash - 195255
class spell_sha_stormlash : public SpellScriptLoader
{
    public:
        spell_sha_stormlash() : SpellScriptLoader("spell_sha_stormlash") { }

        class spell_sha_stormlash_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_stormlash_AuraScript);

            enum eSpells
            {
                Flametongue         = 194084,
                Frostband           = 196834,
                StormlashDmg        = 213307,
                StormlashEmpowered  = 210731,
                StormlashBuff       = 195222,
                StormlashVisual     = 213430
            };

            void HandleProc(AuraEffect const*, ProcEventInfo&)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || (!l_Caster->HasAura(eSpells::Flametongue) && !l_Caster->HasAura(eSpells::Frostband)))
                    return;

                SpellInfo const* l_StormlashDmgInfo = sSpellMgr->GetSpellInfo(eSpells::StormlashDmg);
                AuraEffect* l_Effect = GetEffect(EFFECT_0);

                if (!l_StormlashDmgInfo || !l_Effect)
                    return;

                int32 l_NbTargets = l_Effect->GetAmount();
                int32 l_Damage = l_StormlashDmgInfo->Effects[EFFECT_0].CalcValue(l_Caster);
                l_Damage = static_cast<int32>(l_Caster->SpellDamageBonusDone(l_Caster, l_StormlashDmgInfo, l_Damage, EFFECT_0, DamageEffectType::SPELL_DIRECT_DAMAGE));

                std::list<Unit*> l_TargetList = SelectTargets(l_Caster, l_NbTargets);

                if (l_TargetList.size() > l_NbTargets)
                    l_TargetList.resize(l_NbTargets);

                for (Unit* l_Target : l_TargetList)
                {
                    l_Caster->CastCustomSpell(l_Target, eSpells::StormlashBuff, &l_Damage, &l_Damage, nullptr, true);
                    l_Caster->CastSpell(l_Target, eSpells::StormlashVisual, true);
                }
            }

            std::list<Unit*> SelectTargets(Unit* p_Caster, int32 l_NbTargets)
            {
                int32 l_Radius = 100;
                std::list<Unit*> l_AlliesList;
                JadeCore::AnyGroupedPlayerInObjectRangeCheck l_Check(p_Caster, p_Caster, l_Radius, false);
                JadeCore::UnitListSearcher<JadeCore::AnyGroupedPlayerInObjectRangeCheck> l_Searcher(p_Caster, l_AlliesList, l_Check);
                p_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                std::list<Unit*> l_NotHealerTargets;
                std::list<Unit*> l_SelectedTargets;

                SelectNotHealer(l_NotHealerTargets, l_AlliesList);
                SelectWithoutStormslash(l_SelectedTargets, l_NotHealerTargets, false);
                if (l_SelectedTargets.size() >= l_NbTargets)
                    return l_SelectedTargets;

                SelectWithoutStormslash(l_SelectedTargets, l_NotHealerTargets, true);
                if (l_SelectedTargets.size() >= l_NbTargets)
                    return l_SelectedTargets;

                SelectWithoutStormslashFromThisShaman(l_SelectedTargets, l_NotHealerTargets, p_Caster->GetGUID());
                if (l_SelectedTargets.size() >= l_NbTargets)
                    return l_SelectedTargets;

                for (Unit* l_Target : l_NotHealerTargets)
                {
                    l_SelectedTargets.push_back(l_Target);
                    if (l_SelectedTargets.size() >= l_NbTargets)
                        return l_SelectedTargets;
                }

                for (Unit* l_Target : l_AlliesList)
                {
                    l_SelectedTargets.push_back(l_Target);
                    if (l_SelectedTargets.size() >= l_NbTargets)
                        return l_SelectedTargets;
                }

                return l_SelectedTargets;
            }

            void SelectNotHealer(std::list<Unit*>& p_SelectTargets, std::list<Unit*>& p_TargetList)
            {
                for (Unit* l_Target : p_TargetList)
                {
                    if (Player* l_Player = l_Target->ToPlayer())
                    {
                        int32 l_Specialization = l_Player->GetActiveSpecializationID();

                        if (l_Player->GetRoleBySpecializationId(l_Specialization) != Roles::ROLE_HEALER)
                            p_SelectTargets.push_back(l_Target);
                    }
                }

                for (Unit* l_Target : p_SelectTargets)
                    p_TargetList.remove(l_Target);
            }

            void SelectWithoutStormslash(std::list<Unit*>& p_SelectTargets, std::list<Unit*>& p_TargetList, bool p_Shaman)
            {
                for (Unit* l_Target : p_TargetList)
                {
                    if (l_Target->HasAura(eSpells::StormlashBuff))
                        continue;

                    if (Player* l_Player = l_Target->ToPlayer())
                    {
                        if ((l_Player->getClass() == Classes::CLASS_SHAMAN) != p_Shaman)
                            continue;

                        p_SelectTargets.push_back(l_Target);
                    }
                }

                for (Unit* l_Target : p_SelectTargets)
                    p_TargetList.remove(l_Target);
            }

            void SelectWithoutStormslashFromThisShaman(std::list<Unit*>& p_SelectTargets, std::list<Unit*>& p_TargetList, uint64 p_GUID)
            {
                for (Unit* l_Target : p_TargetList)
                {
                    if (l_Target->HasAura(eSpells::StormlashBuff, p_GUID))
                        continue;

                    if (Player* l_Player = l_Target->ToPlayer())
                        p_SelectTargets.push_back(l_Target);
                }

                for (Unit* l_Target : p_SelectTargets)
                    p_TargetList.remove(l_Target);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_stormlash_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_stormlash_AuraScript();
        }
};

/// Called by Ghost In The Mist - 207524
class spell_sha_ghost_in_the_mist : public SpellScriptLoader
{
    public:
        spell_sha_ghost_in_the_mist() : SpellScriptLoader("spell_sha_ghost_in_the_mist") { }

        class spell_sha_ghost_in_the_mist_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ghost_in_the_mist_AuraScript);

            enum eSpells
            {
                GhostInTheMistBuff = 207527
            };

            enum eArtifactPowers
            {
                GhostInTheMistArtifact = 1110
            };

            void HandlePeriodic(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    int32 l_TraitRank = -l_Player->GetRankOfArtifactPowerId(eArtifactPowers::GhostInTheMistArtifact);

                    l_Player->CastCustomSpell(l_Player, eSpells::GhostInTheMistBuff, &l_TraitRank, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_ghost_in_the_mist_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_ghost_in_the_mist_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Stormlash - 195222
class spell_sha_stormlash_buff : public SpellScriptLoader
{
    public:
        spell_sha_stormlash_buff() : SpellScriptLoader("spell_sha_stormlash_buff") { }

        class spell_sha_stormlash_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_stormlash_buff_AuraScript);

            int32 m_PreviousExpiredPct = 0;
            int32 m_UsedPotential = 0;

            enum eSpells
            {
                StormlashDmg    = 195256,
                StormlashVisual = 213430
            };

            void HandleProc(AuraEffect const* p_Effect, ProcEventInfo& p_EventInfo)
            {
                if (DamageInfo *l_DmgInfo = p_EventInfo.GetDamageInfo())
                    if (l_DmgInfo->GetSpellInfo() && l_DmgInfo->GetSpellInfo()->Id == eSpells::StormlashDmg)
                        return;

                Aura* l_Aura = p_Effect->GetBase();
                Unit* l_Owner = GetUnitOwner();
                Unit* l_Target = nullptr;

                if (!l_Aura || !l_Owner)
                    return;

                if (!(l_Target = sObjectAccessor->GetUnit(*l_Owner, l_Owner->GetTargetGUID())))
                    return;

                int32 l_ExpiredPct = 100 - (l_Aura->GetDuration() / static_cast<float>(l_Aura->GetMaxDuration())) * 100;
                int32 l_DamagePct = l_ExpiredPct - m_PreviousExpiredPct;
                int32 l_Potential = p_Effect->GetAmount() - m_UsedPotential;
                int32 l_Damage = CalculatePct(l_Potential, l_DamagePct);

                m_UsedPotential += l_Damage;
                m_PreviousExpiredPct = l_ExpiredPct;

                l_Owner->CastCustomSpell(l_Target, eSpells::StormlashDmg, &l_Damage, nullptr, nullptr, true);

                if (Unit* l_Caster = GetCaster()) 
                {
                    if (l_Caster != l_Owner) 
                    {
                        l_Caster->CastSpell(l_Owner, eSpells::StormlashVisual, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_stormlash_buff_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_stormlash_buff_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Ethereal Form : return - 211060
class spell_sha_ethereal_form_return : public SpellScriptLoader
{
    public:
        spell_sha_ethereal_form_return() : SpellScriptLoader("spell_sha_ethereal_form_return") { }

        enum eSpells
        {
            EtherealForm = 210918
        };

        class spell_sha_ethereal_form_return_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_ethereal_form_return_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::EtherealForm);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_ethereal_form_return_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_ethereal_form_return_SpellScript();
        }
};

/// Called by Fury of the Storms - 191717
class spell_sha_fury_of_the_storms : public SpellScriptLoader
{
    public:
        spell_sha_fury_of_the_storms() : SpellScriptLoader("spell_sha_fury_of_the_storms") { }

        class spell_sha_fury_of_the_storms_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_fury_of_the_storms_AuraScript);

            enum eSpells
            {
                Stormkeeper         = 205495,
                LightningElemental  = 191716
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Stormkeeper)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::LightningElemental, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_fury_of_the_storms_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_sha_fury_of_the_storms_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_fury_of_the_storms_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Fiery Jaws - 224125, Frozen Bite - 224126, Fire Nova - 198480, Snowstorm - 198483, Thunder Bite - 198485
class spell_sha_doom_wolves_attacks : public SpellScriptLoader
{
    public:
        spell_sha_doom_wolves_attacks() : SpellScriptLoader("spell_sha_doom_wolves_attacks") { }

        enum eSpells
        {
            FieryJaws           = 224125,
            FrozenBite          = 224126,
            FireNova            = 198480,
            Snowstorm           = 198483,
            ThunderBite         = 198485,

            EnhancementMastery  = 77223
        };

        class spell_sha_doom_wolves_attacks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_doom_wolves_attacks_AuraScript);

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Wolf = GetCaster();
                Unit* l_Owner = nullptr;

                if (!l_Wolf || !(l_Owner = l_Wolf->GetOwner()))
                    return;

                AuraEffect* l_MasteryEffect = l_Owner->GetAuraEffect(eSpells::EnhancementMastery, EFFECT_0);

                int32 l_Mastery = (l_MasteryEffect ? l_MasteryEffect->GetFloatAmount() : 0);
                float l_AP = l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                int32 l_Damage = 0.8f * l_AP;

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(AddPct(l_Damage, l_Mastery));
            }

            void Register() override
            {
                if (m_scriptSpellId != eSpells::FieryJaws)
                    return;

                AfterEffectApply += AuraEffectApplyFn(spell_sha_doom_wolves_attacks_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_doom_wolves_attacks_AuraScript();
        }

        class spell_sha_doom_wolves_attacks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_doom_wolves_attacks_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Wolf = GetCaster();
                Unit* l_Owner = nullptr;

                if (!l_Wolf || !(l_Owner = l_Wolf->GetOwner()))
                    return;

                AuraEffect* l_MasteryEffect = l_Owner->GetAuraEffect(eSpells::EnhancementMastery, EFFECT_0);

                int32 l_Mastery = (l_MasteryEffect ? l_MasteryEffect->GetAmount() : 0);
                int32 l_Damage = GetHitDamage();
                float l_AP = l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                switch (static_cast<eSpells>(m_scriptSpellId))
                {
                    case FieryJaws:
                        l_Damage = 2 * l_AP;
                        break;
                    case FrozenBite:
                        l_Damage = 2 * l_AP * 1.5f;
                        break;
                    case FireNova:
                    case Snowstorm:
                    case ThunderBite:
                        l_Damage = l_AP * 0.8f;
                        break;
                    default:
                        return;
                }

                SetHitDamage(AddPct(l_Damage, l_Mastery));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sha_doom_wolves_attacks_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_doom_wolves_attacks_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Alpha Wolf - 198486
class spell_sha_alpha_wolf : public SpellScriptLoader
{
public:
    spell_sha_alpha_wolf() : SpellScriptLoader("spell_sha_alpha_wolf") { }

    class spell_sha_alpha_wolf_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_alpha_wolf_AuraScript);

        enum eSpells
        {
            FireNova = 198480,
            Snowstorm = 198483,
            ThunderBite = 198485
        };

        enum eDisplayId
        {

            DoomWolvesFire = 66843,
            DoomWolvesFrost = 66844,
            DoomWolvesNature = 66845
        };

        void OnTick(AuraEffect const* p_AurEff)
        {
            Unit* l_Wolf = GetUnitOwner();
            Unit* l_Target = nullptr;

            if (!l_Wolf || !(l_Target = l_Wolf->getVictim()))
                return;

            switch (l_Wolf->GetDisplayId())
            {
            case eDisplayId::DoomWolvesFire:
                l_Wolf->CastSpell(l_Target, eSpells::FireNova, true, (Item*)nullptr, (AuraEffect*)nullptr, l_Wolf->GetOwnerGUID());
                break;
            case eDisplayId::DoomWolvesFrost:
                l_Wolf->CastSpell(l_Target, eSpells::Snowstorm, true, (Item*)nullptr, (AuraEffect*)nullptr, l_Wolf->GetOwnerGUID());
                break;
            case eDisplayId::DoomWolvesNature:
                l_Wolf->CastSpell(l_Target, eSpells::ThunderBite, true, (Item*)nullptr, (AuraEffect*)nullptr, l_Wolf->GetOwnerGUID());
                break;
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_alpha_wolf_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_sha_alpha_wolf_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by sundering - 197214
class spell_sha_sundering : public SpellScriptLoader
{
    public:
        spell_sha_sundering() : SpellScriptLoader("spell_sha_sundering") { }

        class spell_sha_sundering_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_sundering_SpellScript);

            enum eSpells
            {
                SunderingKnockBack = 197619,
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SunderingKnockBack, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_sundering_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_sundering_SpellScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Spirit of the Maelstorm - 198240
class spell_sha_spirit_of_the_maelstorm : public SpellScriptLoader
{
    public:
        spell_sha_spirit_of_the_maelstorm() : SpellScriptLoader("spell_sha_spirit_of_the_maelstorm") { }

        class spell_sha_spirit_of_the_maelstorm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_spirit_of_the_maelstorm_AuraScript);

            enum eSpells
            {
                SpiritOfTheMaelstorm            = 198238,
                SpiritOfTheMaelstormEnergize    = 204880
            };

            enum eArtifactPowers
            {
                SpiritOfTheMaelstormArt = 907
            };

            void HandlePeriodic(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->isInCombat())
                {
                    if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::SpiritOfTheMaelstorm, EFFECT_0))
                    {
                        if (Player* l_Player = l_Caster->ToPlayer())
                        {
                            int8 l_TraitRank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::SpiritOfTheMaelstormArt);
                            int32 l_Amount = l_AurEff->GetSpellEffectInfo()->BasePoints * l_TraitRank;
                            l_Caster->CastCustomSpell(eSpells::SpiritOfTheMaelstormEnergize, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Amount, l_Caster, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_spirit_of_the_maelstorm_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_spirit_of_the_maelstorm_AuraScript();
        }
};

/// Last Update Legion 7.1.5
/// Called by Gost In the Mist - 207351
class spell_sha_ghost_in_the_mist_proc : public SpellScriptLoader
{
    public:
        spell_sha_ghost_in_the_mist_proc() : SpellScriptLoader("spell_sha_ghost_in_the_mist_proc") { }

        class spell_sha_ghost_in_the_mist_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ghost_in_the_mist_proc_AuraScript);

            enum eSpells
            {
                GhostWolf = 2645
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::GhostWolf)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_ghost_in_the_mist_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_ghost_in_the_mist_proc_AuraScript();
        }
};

/// Last Update Legion 7.1.5 Build 23420
/// Called by Elementalist - 191512
class spell_sha_elementalist : public SpellScriptLoader
{
    public:
        spell_sha_elementalist() : SpellScriptLoader("spell_sha_elementalist") { }

        class spell_sha_elementalist_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_elementalist_AuraScript);

            enum eSpells
            {
                LavaBurst       = 51505,
                StormElemental  = 192249,
                FireElemental   = 198067
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::LavaBurst)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                AuraEffect* l_AuraEffect = GetEffect(EFFECT_0);
                if (!l_Caster || !l_AuraEffect)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_SpellId;
                if (l_Player->HasSpell(eSpells::StormElemental))
                    l_SpellId = eSpells::StormElemental;
                else
                    l_SpellId = eSpells::FireElemental;

                l_Player->ReduceSpellCooldown(l_SpellId, -l_AuraEffect->GetAmount());
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_elementalist_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_sha_elementalist_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_elementalist_AuraScript();
        }
};

/// Last Update Legion 7.1.5
/// Called by Purge 370 For Electrocute (Honor Talent) 206642
class spell_sha_electrocute : public SpellScriptLoader
{
    public:
        spell_sha_electrocute() : SpellScriptLoader("spell_sha_electrocute") { }

        class spell_sha_electrocute_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_electrocute_SpellScript);

            enum eSpells
            {
                PvPAura             = 134735,
                ElectrocuteAura     = 206642,
                Electrocute         = 206647,
            };

            void HandleDispel(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::PvPAura) || !l_Caster->HasAura(eSpells::ElectrocuteAura))
                    return;

                DispelChargesList l_DispelChargeList;
                uint32 l_DispelMask = 1 << DispelType::DISPEL_MAGIC;
                l_Target->GetDispellableAuraList(l_Caster, l_DispelMask, l_DispelChargeList);
                if (l_DispelChargeList.size() > 0)
                    l_Caster->CastSpell(l_Target, eSpells::Electrocute, true);
            }

            void Register() override
            {
                 OnEffectHitTarget += SpellEffectFn(spell_sha_electrocute_SpellScript::HandleDispel, EFFECT_0, SPELL_EFFECT_DISPEL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_electrocute_SpellScript();
        }
};

/// Last Update Legion 7.1.5
/// Called by Shamanistic Healing 191582
class spell_sha_shamanistic_healing : public SpellScriptLoader
{
    public:
        spell_sha_shamanistic_healing() : SpellScriptLoader("spell_sha_shamanistic_healing") { }

        class spell_sha_shamanistic_healing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_shamanistic_healing_AuraScript);

            enum eSpells
            {
                NatureEssence       = 191580
            };

            void HandleOnProc(ProcEventInfo&  p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_HealthPct = l_Caster->GetHealth() - l_DamageInfo->GetAmount();
                l_HealthPct = l_HealthPct * 100 / l_Caster->GetMaxHealth();

                if (l_HealthPct > l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    return;

                l_Player->CastSpell(l_Player, eSpells::NatureEssence, true);
                l_Player->AddSpellCooldown(eSpells::NatureEssence, 0, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                 OnProc += AuraProcFn(spell_sha_shamanistic_healing_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_shamanistic_healing_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Bloodlust - 2825 204361  Heroism - 32182 204362  Time Warp - 80353  Ancient Hysteria - 90355  Netherwinds - 160452
/// Called For Sence of Urgency - 207355
class spell_sha_sence_of_urgency : public SpellScriptLoader
{
    public:
        spell_sha_sence_of_urgency() : SpellScriptLoader("spell_sha_sence_of_urgency") { }

        class spell_sha_sence_of_urgency_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_sence_of_urgency_SpellScript);

            enum eSpells
            {
                SenceOfUrgency      = 207355,
                SenceOfUrgencyBuff  = 208416,
                BloodLust           = 2825,
                BloodlustTalent     = 204361,
                Heroism             = 32182,
                HeroismTalent       = 204362,
                TimeWarp            = 80353,
                AncientHysteria     = 90355,
                Netherwinds         = 160452
            };

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                /// Update 7.1.5 build 23420: before this fix, shaman would benefit from sense of urgency if a BL effect was casted by an ally, even if the shaman wasn't affected by it due to exhaust/similar effects
                if (l_Target->HasAura(eSpells::SenceOfUrgency) && !l_Target->HasAura(eSpells::SenceOfUrgencyBuff)
                    && (l_Target->HasAura(eSpells::BloodLust)
                    || l_Target->HasAura(eSpells::Heroism)
                    || l_Target->HasAura(eSpells::BloodlustTalent)
                    || l_Target->HasAura(eSpells::HeroismTalent)
                    || l_Target->HasAura(eSpells::TimeWarp)
                    || l_Target->HasAura(eSpells::AncientHysteria)
                    || l_Target->HasAura(eSpells::Netherwinds)))
                {
                    l_Target->CastSpell(l_Target, eSpells::SenceOfUrgencyBuff, true);
                }
                else
                    return;

                SpellInfo const* l_Spellinfo = GetSpellInfo();
                if (!l_Spellinfo)
                    return;

                int32 l_BLDuration = l_Spellinfo->GetMaxDuration();
                if (l_BLDuration == 0)
                    return;

                Aura* l_SenseOfUrgency = l_Target->GetAura(eSpells::SenceOfUrgencyBuff);
                if (l_SenseOfUrgency)
                    l_SenseOfUrgency->SetDuration(l_BLDuration);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_sha_sence_of_urgency_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_sence_of_urgency_SpellScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Gift of the Queen - 207778
class spell_sha_gift_of_the_queen : public SpellScriptLoader
{
    public:
        spell_sha_gift_of_the_queen() : SpellScriptLoader("spell_sha_gift_of_the_queen") { }

        class spell_sha_gift_of_the_queen_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_gift_of_the_queen_SpellScript);

            enum eSpells
            {
                DeepWaters      = 238143,
                GiftOfTheQueen  = 255227
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() <= 6)
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                p_Targets.resize(6);
            }

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();

                if (!l_Caster || !l_Dest)
                    return;

                if (l_Caster->IsPlayer() && l_Caster->HasAura(eSpells::DeepWaters))
                {
                    WorldLocation l_Location = *l_Dest;

                    l_Caster->AddDelayedEvent([l_Caster, l_Location]() -> void
                    {
                        l_Caster->CastSpell(l_Location.GetPositionX(), l_Location.GetPositionY(), l_Location.GetPositionZ(), eSpells::GiftOfTheQueen, true);
                    }, 3 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_gift_of_the_queen_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_gift_of_the_queen_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
                OnCast += SpellCastFn(spell_sha_gift_of_the_queen_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_gift_of_the_queen_SpellScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Thunderstorm - 204406
class spell_sha_thunderstorm_honor : public SpellScriptLoader
{
    public:
        spell_sha_thunderstorm_honor() : SpellScriptLoader("spell_sha_thunderstorm_honor") { }

        class spell_sha_thunderstorm_honor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_thunderstorm_honor_SpellScript);

            enum eSpells
            {
                ThunderstormKnockback   = 228947,
                Thunderstorm            = 204408
            };

            void HandleOnHit()
            {
                Unit* l_Target = GetHitUnit();
                Spell* l_Spell = GetSpell();

                if (!l_Spell || !l_Spell->m_FromClient || (!l_Target && !(l_Target = GetCaster())))
                    return;

                l_Target->CastSpell(l_Target, eSpells::Thunderstorm, true);
                l_Target->CastSpell(l_Target, eSpells::ThunderstormKnockback, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sha_thunderstorm_honor_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_thunderstorm_honor_SpellScript();
        }
};

/// Last Update Legion 7.1.5 Build 23420
/// Called by Lava Surge - 77756
class spell_sha_lava_surge : public SpellScriptLoader
{
    public:
        spell_sha_lava_surge() : SpellScriptLoader("spell_sha_lava_surge") { }

        enum eSpells
        {
            FlameShock          = 188389,
            FlameShockResto     = 188838,
            InstantCastBuff     = 77762,
            LavaBurst           = 51505,
            EchoOfElements      = 108283,
            ControlOfLava       = 204393
        };

        class spell_sha_lava_surge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_lava_surge_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::FlameShock && l_SpellInfo->Id != eSpells::FlameShockResto))
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

                spell_sha_lava_burst_reset::LavaBurstCDReset l_CdReset(l_Player);
                l_CdReset.ResetCooldown();

                if (l_Caster->HasAura(eSpells::ControlOfLava))
                    return;

                l_Player->CastSpell(l_Caster, eSpells::InstantCastBuff, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_lava_surge_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_sha_lava_surge_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_lava_surge_AuraScript();
        }
};

/// Last Update Legion 7.1.5 Build 23420
/// Called by Static Cling - 211062
class spell_sha_static_cling : public SpellScriptLoader
{
    public:
        spell_sha_static_cling() : SpellScriptLoader("spell_sha_static_cling") { }

        class spell_sha_static_cling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_static_cling_AuraScript);

            enum eSpells
            {
                Stormstrike         = 32175,
                StaticClingDamage   = 210927,
                StaticClingSpeed    = 211400
            };

            uint8 m_CastCount = 0;

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Spell || !l_SpellInfo || (l_Spell->IsTriggered() && l_SpellInfo->Id != eSpells::Stormstrike))
                    return false;

                m_CastCount = (l_SpellInfo->Id == eSpells::Stormstrike) ? m_CastCount + 1 : 0;
                if (m_CastCount != 2)
                    return false;

                m_CastCount = 0;
                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::StaticClingDamage, true);
                l_Caster->CastSpell(l_Caster, eSpells::StaticClingSpeed, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_static_cling_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_sha_static_cling_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_static_cling_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Static Cling - 210927
class spell_sha_static_cling_damage : public SpellScriptLoader
{
    public:
        spell_sha_static_cling_damage() : SpellScriptLoader("spell_sha_static_cling_damage") { }

        class spell_sha_static_cling_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_static_cling_damage_AuraScript);

            enum eSpells
            {
                StaticClingSpeed = 211400
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_SpellInfo->RangeEntry)
                    return;

                if (l_Caster->isInCombat() && l_Target->isInCombat() && l_Caster->GetDistance(l_Target) < l_SpellInfo->RangeEntry->maxRangeHostile)
                    return;

                l_Caster->RemoveAura(eSpells::StaticClingSpeed);
                SetDuration(0);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_static_cling_damage_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_static_cling_damage_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Earth Shock - 8042
/// Called for Earthfury- 204398
class spell_sha_earthfury : public SpellScriptLoader
{
    public:
        spell_sha_earthfury() : SpellScriptLoader("spell_sha_earthfury") { }

        class spell_sha_earthfury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earthfury_SpellScript);

            enum eSpells
            {
                EarthfuryAura   = 204398,
                EarthfuryStun   = 204399
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Spell* l_Spell = GetSpell();
                if (!l_Caster || !l_Caster->HasAura(eSpells::EarthfuryAura) || !l_Target || !l_Spell || l_Spell->GetPowerCost(Powers::POWER_MAELSTROM) < 100)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::EarthfuryStun, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_sha_earthfury_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_earthfury_SpellScript();
        }
};

/// Update 7.1.5 Build 23420
/// Called by Hot Hand - 201900
class spell_sha_hot_hand : public SpellScriptLoader
{
    public:
        spell_sha_hot_hand() : SpellScriptLoader("spell_sha_hot_hand") { }

        class spell_sha_hot_hand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_hot_hand_AuraScript);

            enum eSpells
            {
                Flametongue = 194084
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                if (GetCaster() && GetCaster()->HasAura(eSpells::Flametongue))
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_hot_hand_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_hot_hand_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Ride the Lightning - 204357
class spell_sha_ride_the_lightning_proc : public SpellScriptLoader
{
    public:
        spell_sha_ride_the_lightning_proc() : SpellScriptLoader("spell_sha_ride_the_lightning_proc") { }

        class spell_sha_ride_the_lightning_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ride_the_lightning_proc_AuraScript);

            enum eWhitelistSpells
            {
                StormStrike             = 17364,
                Windstrike              = 115356
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (!l_Caster || !l_DamageInfo)
                    return false;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();

                if (l_ProcSpellInfo == nullptr || l_Spell == nullptr)
                    return false;

                if ((l_ProcSpellInfo->Id != eWhitelistSpells::StormStrike && l_ProcSpellInfo->Id != eWhitelistSpells::Windstrike) || l_Spell->IsTriggered())
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_ride_the_lightning_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_ride_the_lightning_proc_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Tidal Waves - 53390
class spell_sha_tidal_waves_for_t19 : public SpellScriptLoader
{
public:
    spell_sha_tidal_waves_for_t19() : SpellScriptLoader("spell_sha_tidal_waves_for_t19") { }

    class spell_sha_tidal_waves_for_t19_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_tidal_waves_for_t19_AuraScript);

        enum eSpells
        {
            T19Restoration2P = 211992,
            T19Restoration4P = 211993,
            TidalWaves = 211991,
            HealingStreamTotem = 5394
        };

        void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            /// Item - Shaman T19 Restoration 2P Bonus
            if (l_Caster->HasAura(eSpells::T19Restoration2P))
                l_Caster->CastSpell(l_Caster, eSpells::TidalWaves, true);
        }

        void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::TidalWaves);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_sha_tidal_waves_for_t19_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_sha_tidal_waves_for_t19_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        }
    };

    class spell_sha_tidal_waves_for_t19_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_tidal_waves_for_t19_SpellScript);

        enum eSpells
        {
            T19Restoration4P = 211993,
            HealingStreamTotem = 5394
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            /// Item - Shaman T19 Restoration 4P Bonus
            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Restoration4P, EFFECT_0))
            {
                if (SpellInfo const* l_Totem = sSpellMgr->GetSpellInfo(eSpells::HealingStreamTotem))
                    l_Player->ReduceChargeCooldown(l_Totem->ChargeCategoryEntry, l_AuraEffect->GetAmount() * IN_MILLISECONDS);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_sha_tidal_waves_for_t19_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sha_tidal_waves_for_t19_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_tidal_waves_for_t19_AuraScript();
    }
};

/// Last Update 7.3.2 build 25549
/// Called by Ascendance - 114051
class spell_sha_ascendance_pct : public SpellScriptLoader
{
    public:
        spell_sha_ascendance_pct() : SpellScriptLoader("spell_sha_ascendance_pct") { }

        class spell_sha_ascendance_pct_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_ascendance_pct_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.625f;   ///< Ascendance is reduced by 37.5% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_ascendance_pct_AuraScript::HandlePvPModifier, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_ascendance_pct_AuraScript::HandlePvPModifier, EFFECT_4, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_ascendance_pct_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Earthen Spike - 188089
class spell_sha_earthen_spike_bonus : public SpellScriptLoader
{
    public:
        spell_sha_earthen_spike_bonus() : SpellScriptLoader("spell_sha_earthen_spike_bonus") { }

        class spell_sha_earthen_spike_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthen_spike_bonus_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.66666f;   ///< Earthen Spike is reduced by 33.334% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_earthen_spike_bonus_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_earthen_spike_bonus_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Shamanistic Healing - 191582
class spell_sha_shamanic_healing : public SpellScriptLoader
{
    public:
        spell_sha_shamanic_healing() : SpellScriptLoader("spell_sha_shamanic_healing") { }

        class spell_sha_shamanic_healing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_shamanic_healing_AuraScript);

            enum eSpells
            {
                ShamanicHealingAura     = 191582,
                ShamanicHealingCooldown = 191591,
                ShamanicHealingSpell    = 191580
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_ShamanicCooldown = sSpellMgr->GetSpellInfo(eSpells::ShamanicHealingCooldown);
                if (!l_Caster || !l_DamageInfo || !l_ShamanicCooldown)
                    return false;

                if (l_Caster->HasAura(eSpells::ShamanicHealingCooldown) || l_Caster->GetHealthPct() > l_ShamanicCooldown->Effects[EFFECT_0].BasePoints)
                    return false;

                return true;
            }

            void HandleProc(AuraEffect const*, ProcEventInfo& p_Info)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ShamanicHealingCooldown, true);
                l_Caster->CastSpell(l_Caster, eSpells::ShamanicHealingSpell, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_shamanic_healing_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_sha_shamanic_healing_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_shamanic_healing_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Shamanistic Healing - 191580
class spell_sha_shamanic_healing_triggered: public SpellScriptLoader
{
    public:
        spell_sha_shamanic_healing_triggered() : SpellScriptLoader("spell_sha_shamanic_healing_triggered") { }

        class spell_sha_shamanic_healing_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_shamanic_healing_triggered_SpellScript);

            enum eSpells
            {
                ShamanicHealingAura     = 191582
            };

            void HandleHeal(SpellEffIndex /*l_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_Heal = GetHitHeal();
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ShamanicHealingAura, EFFECT_0))
                    l_Heal *= l_AuraEffect->GetAmount();

                SetHitHeal(l_Heal);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_shamanic_healing_triggered_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_shamanic_healing_triggered_SpellScript();
        }
};

/// Last 7.3.2 Build 25549
/// Called By Static Overload - 191602
class spell_sha_static_overload : public SpellScriptLoader
{
public:
    spell_sha_static_overload() : SpellScriptLoader("spell_sha_static_overload") { }

    class spell_sha_static_overload_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_static_overload_AuraScript);

        bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
        {
            return false;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_static_overload_AuraScript::HandleDoCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_static_overload_AuraScript();
    }
};

/// Lashing Flames - 238142
class spell_sha_lashing_flames : public SpellScriptLoader
{
public:
    spell_sha_lashing_flames() : SpellScriptLoader("spell_sha_lashing_flames") { }

    class spell_sha_lashing_flames_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_lashing_flames_AuraScript);

        enum eSpells
        {
            Flametongue         = 193796,
            FlametongueAttack   = 10444,
        };

        bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

            if(l_DamageInfo && l_DamageInfo->GetSpellInfo())
                if (l_DamageInfo->GetSpellInfo()->Id ==eSpells::Flametongue || l_DamageInfo->GetSpellInfo()->Id == eSpells::FlametongueAttack)
                    return true;

            return false;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_lashing_flames_AuraScript::HandleDoCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_lashing_flames_AuraScript();
    }
};

/// Lava Lash - 60103
class spell_sha_lava_lash : public SpellScriptLoader
{
    public:
        spell_sha_lava_lash() : SpellScriptLoader("spell_sha_lava_lash") { }

        class spell_sha_lava_lash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lava_lash_SpellScript);

            enum eSpells
            {
                Flametongue     = 193796,
                LashingFlames   = 240842
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->HasAura(eSpells::LashingFlames, l_Caster->GetGUID()))
                    l_Target->RemoveAura(LashingFlames, l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_lava_lash_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_lava_lash_SpellScript();
        }
};

/// 7.3.5 build 25996
/// Purify Spirit - 77130
class spell_sha_purify_spirit : public SpellScriptLoader
{
public:
    spell_sha_purify_spirit() : SpellScriptLoader("spell_sha_purify_spirit") { }

    class spell_sha_purify_spirit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_purify_spirit_SpellScript);

        SpellCastResult HandleCheckCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();

            if (!l_Target || !l_Caster)
                return SPELL_FAILED_DONT_REPORT;

            DispelChargesList l_DispelChargeList;
            uint32 l_DispelMask = 1 << DispelType::DISPEL_MAGIC;
            l_DispelMask |= 1 << DispelType::DISPEL_CURSE;

            l_Target->GetDispellableAuraList(l_Caster, l_DispelMask, l_DispelChargeList);

            if(l_DispelChargeList.size() == 0)
                return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_sha_purify_spirit_SpellScript::HandleCheckCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sha_purify_spirit_SpellScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Seismic Storm - 238141
class spell_sha_seismic_storm : public SpellScriptLoader
{
public:
    spell_sha_seismic_storm() : SpellScriptLoader("spell_sha_seismic_storm") { }

    class spell_sha_seismic_storm_Aurascript : public AuraScript
    {
        PrepareAuraScript(spell_sha_seismic_storm_Aurascript);

        enum eSpells
        {
            Earthquake = 77478
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Earthquake)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_seismic_storm_Aurascript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_seismic_storm_Aurascript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Chilled By Riptide - 61295
class spell_sha_rippling_waters_heal : public SpellScriptLoader
{
    public:
        spell_sha_rippling_waters_heal() : SpellScriptLoader("spell_sha_rippling_waters_heal") { }

        class spell_sha_rippling_waters_heal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_rippling_waters_heal_AuraScript);

            enum eSpells
            {
                RipplingWaters      = 204269,
                RipplingWatersHeal  = 204281
            };

            void OnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::RipplingWaters))
                    l_Caster->CastSpell(l_Target, eSpells::RipplingWatersHeal, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_rippling_waters_heal_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
           }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_rippling_waters_heal_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Earth Shock (8042) visual icon
class PlayerScript_Earthfury_visual : public PlayerScript
{
    public:
        PlayerScript_Earthfury_visual() :PlayerScript("PlayerScript_Earthfury_visual") { }

        enum eSpells
        {
            FulminationVisual = 190494,
            SwellingMaelstrom = 238105
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_MAELSTROM || !p_After || p_Player->GetActiveSpecializationID() != SPEC_SHAMAN_ELEMENTAL)
                return;

            uint32 l_ReqMaelstrom = 100;

            if (p_Player->HasAura(eSpells::SwellingMaelstrom))
                if (SpellInfo const* l_SwellingMaelstrom = sSpellMgr->GetSpellInfo(eSpells::SwellingMaelstrom))
                    l_ReqMaelstrom += l_SwellingMaelstrom->Effects[EFFECT_0].BasePoints;

            if (p_NewValue < l_ReqMaelstrom && p_Player->HasAura(eSpells::FulminationVisual))
                p_Player->RemoveAura(eSpells::FulminationVisual);

            if (p_NewValue >= l_ReqMaelstrom && !p_Player->HasAura(eSpells::FulminationVisual))
                p_Player->CastSpell(p_Player, eSpells::FulminationVisual, true);
        }
};

/// Last Update 7.3.5
/// Item - Shaman T20 Enhancement 2P Bonus - 242283
class spell_sha_t20_enhancement_2p_bonus : public SpellScriptLoader
{
public:
    spell_sha_t20_enhancement_2p_bonus() : SpellScriptLoader("spell_sha_t20_enhancement_2p_bonus") { }

    class spell_sha_t20_enhancement_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_t20_enhancement_2p_bonus_AuraScript);

        enum eSpells
        {
            CrashLightning = 187874
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::CrashLightning)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_t20_enhancement_2p_bonus_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_t20_enhancement_2p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Item - Shaman T20 Elemental 4P Bonus - 242282
class spell_sha_t20_elemental_2p_proc : public SpellScriptLoader
{
    public:
        spell_sha_t20_elemental_2p_proc() : SpellScriptLoader("spell_sha_t20_elemental_2p_proc") { }

        class spell_sha_t20_elemental_2p_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_t20_elemental_2p_proc_AuraScript);

            enum eSpells
            {
                FireElemental   = 198067,
                StormElemental  = 192249
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasSpellCooldown(eSpells::FireElemental))
                    l_Player->ReduceSpellCooldown(eSpells::FireElemental, GetSpellInfo()->Effects[EFFECT_0].BasePoints * 100);

                if (l_Player->HasSpellCooldown(eSpells::StormElemental))
                    l_Player->ReduceSpellCooldown(eSpells::StormElemental, GetSpellInfo()->Effects[EFFECT_1].BasePoints * 100);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_sha_t20_elemental_2p_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_t20_elemental_2p_proc_AuraScript();
        }
};

/// Cleanse Spirit - 51886
class spell_sha_cleanse_spirit : public SpellScriptLoader
{
public:
    spell_sha_cleanse_spirit() : SpellScriptLoader("spell_sha_cleanse_spirit") { }

    class spell_sha_cleanse_spirit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_cleanse_spirit_SpellScript);

        SpellCastResult CheckCleansing()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Caster || !l_Target)
                return SPELL_FAILED_ERROR;

            DispelChargesList l_DispelList;
            SpellInfo const* l_SpellInfo = GetSpellInfo();

            for (uint8 i = 0; i < l_SpellInfo->EffectCount; ++i)
            {
                if (!l_SpellInfo->Effects[i].IsEffect())
                    break;

                uint32 l_DispelType = l_SpellInfo->Effects[i].MiscValue;
                uint32 l_DispelMask = l_SpellInfo->GetDispelMask(DispelType(l_DispelType));

                l_Target->GetDispellableAuraList(l_Caster, l_DispelMask, l_DispelList);
            }

            if (l_DispelList.empty())
                return SPELL_FAILED_NOTHING_TO_DISPEL;

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_sha_cleanse_spirit_SpellScript::CheckCleansing);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sha_cleanse_spirit_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Skyfury Totem - 208963
class spell_sha_skyfury_totem : public SpellScriptLoader
{
    public:
        spell_sha_skyfury_totem() : SpellScriptLoader("spell_sha_skyfury_totem") { }

        class spell_sha_skyfury_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_skyfury_totem_AuraScript);

            void HandlePvPModifierDamage(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                if (!GetCaster())
                    return;

                Unit* l_Caster = GetCaster()->GetCharmerOrOwnerOrSelf();;
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->CanApplyPvPSpellModifiers())
                    p_Amount *= 2;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_skyfury_totem_AuraScript::HandlePvPModifierDamage, EFFECT_0, SPELL_AURA_MOD_CRIT_DAMAGE_BONUS);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_skyfury_totem_AuraScript();
        }
};

/// Update 7.3.5 Build 26365
/// Called by Forked Lightning - 204350
class spell_sha_forked_lightning : public SpellScriptLoader
{
    public:
        spell_sha_forked_lightning() : SpellScriptLoader("spell_sha_forked_lightning") { }

        class spell_sha_forked_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_forked_lightning_SpellScript);

            enum eSpells
            {
                DeepWaters = 238143,
                GiftOfTheQueen = 255227
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster, false));
                p_Targets.resize(2);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_forked_lightning_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_forked_lightning_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Landslide - 197992
class spell_sha_landslide : public SpellScriptLoader
{
public:
    spell_sha_landslide() : SpellScriptLoader("spell_sha_landslide") { }

    class spell_sha_landslide_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_landslide_AuraScript);

        enum eSpells
        {
            Rockbiter = 193786
        };

        bool HandleOnCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Rockbiter)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_landslide_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_landslide_AuraScript();
    }
};

/// Last Update 7.3.5 build 25996
/// Called by Shaman T20 Restoration 4P Bonus - 242288
class spell_sha_t20_restoration_4p_bonus : public SpellScriptLoader
{
public:
    spell_sha_t20_restoration_4p_bonus() : SpellScriptLoader("spell_sha_t20_restoration_4p_bonus") { }

    class spell_sha_t20_restoration_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_t20_restoration_4p_bonus_AuraScript);

        enum eSpells
        {
            Riptide = 61295
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Riptide)
                return false;

            if (!(p_ProcEventInfo.GetHitMask() & PROC_HIT_CRITICAL))
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_t20_restoration_4p_bonus_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_t20_restoration_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 25996
/// Called by Shaman T20 Enhancement 4P Bonus - 242285
class spell_sha_t20_enhancement_4p_bonus : public SpellScriptLoader
{
public:
    spell_sha_t20_enhancement_4p_bonus() : SpellScriptLoader("spell_sha_t20_enhancement_4p_bonus") { }

    class spell_sha_t20_enhancement_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_t20_enhancement_4p_bonus_AuraScript);

        enum eSpells
        {
            Stormstrike = 32175,
            StormstrikeOfHand = 32176,
            Windstrike = 115357,
            WindstrikeOffHand = 115360
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::Stormstrike && l_SpellInfo->Id != eSpells::StormstrikeOfHand &&
                l_SpellInfo->Id != eSpells::Windstrike && l_SpellInfo->Id != eSpells::WindstrikeOffHand))
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_t20_enhancement_4p_bonus_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_t20_enhancement_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Windfury Attack - 25504 and 33750
/// Called for Winds of Change - 238106
class spell_sha_winds_of_change : public SpellScriptLoader
{
    public:
        spell_sha_winds_of_change() : SpellScriptLoader("spell_sha_winds_of_change") { }

        class spell_sha_winds_of_change_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_winds_of_change_SpellScript);

            enum eSpells
            {
                WindsOfChange   = 238106
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WindsOfChange);
                if (!l_SpellInfo)
                    return;

                l_Caster->EnergizeBySpell(l_Caster, eSpells::WindsOfChange, l_SpellInfo->Effects[EFFECT_0].BasePoints, Powers::POWER_MAELSTROM);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_winds_of_change_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_winds_of_change_SpellScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Elemental Focus - 16246
class spell_sha_elemental_focus_buff : public SpellScriptLoader
{
public:
    spell_sha_elemental_focus_buff() : SpellScriptLoader("spell_sha_elemental_focus_buff") { }

    class spell_sha_elemental_focus_buff_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sha_elemental_focus_buff_AuraScript);

        enum eSpells
        {
            Earthquake      = 77478,
            EarthenRage     = 170379,
            VolcanicInferno = 205533
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id == eSpells::Earthquake
                || l_SpellInfo->Id == eSpells::EarthenRage || l_SpellInfo->Id == eSpells::VolcanicInferno)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_sha_elemental_focus_buff_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_sha_elemental_focus_buff_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Item - Shaman T21 Elemental 2P Bonus - 251757
class spell_sha_t21_elemental_2p_bonus : public SpellScriptLoader
{
    public:
        spell_sha_t21_elemental_2p_bonus() : SpellScriptLoader("spell_sha_t21_elemental_2p_bonus") { }

        class spell_sha_t21_elemental_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_t21_elemental_2p_bonus_AuraScript);

            enum eSpells
            {
                LavaBurst = 51505
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::LavaBurst)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_t21_elemental_2p_bonus_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_t21_elemental_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Item - Shaman T21 Enhancement 2P Bonus  - 251762
class spell_sha_t21_enhancement_2p_bonus : public SpellScriptLoader
{
    public:
        spell_sha_t21_enhancement_2p_bonus() : SpellScriptLoader("spell_sha_t21_enhancement_2p_bonus") { }

        class spell_sha_t21_enhancement_2p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_t21_enhancement_2p_bonus_AuraScript);

            enum eSpells
            {
                Stormstrike = 32175,
                Windstrike  = 115357
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::Stormstrike && l_SpellInfo->Id != eSpells::Windstrike))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_t21_enhancement_2p_bonus_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_t21_enhancement_2p_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Item - Shaman T21 Enhancement 4P Bonus - 251761
class spell_sha_t21_enhancement_4p_bonus : public SpellScriptLoader
{
    public:
        spell_sha_t21_enhancement_4p_bonus() : SpellScriptLoader("spell_sha_t21_enhancement_4p_bonus") { }

        class spell_sha_t21_enhancement_4p_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_t21_enhancement_4p_bonus_AuraScript);

            enum eSpells
            {
                Rockbiter = 193786
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Rockbiter)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_sha_t21_enhancement_4p_bonus_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_t21_enhancement_4p_bonus_AuraScript();
        }
};

/// Last Update : 7.3.5 build 26365
/// Called by Rainfall - 252154
class spell_sha_rainfall_targets : public SpellScriptLoader
{
    public:
        spell_sha_rainfall_targets() : SpellScriptLoader("spell_sha_rainfall_targets") { }

        enum eSpells
        {
            T21Restoration2P    = 251764
        };

        class spell_sha_rainfall_targets_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_rainfall_targets_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                uint32 l_TargetAmount = p_Targets.size();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T21Restoration2P);
                if (!l_SpellInfo)
                    return;

                if (l_TargetAmount <= l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                p_Targets.resize(l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_rainfall_targets_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_rainfall_targets_SpellScript();
        }
};

/// Last Update : 7.3.5 build 26365
/// Called by Downpour - 252159
class spell_sha_downpour_targets : public SpellScriptLoader
{
    public:
        spell_sha_downpour_targets() : SpellScriptLoader("spell_sha_downpour_targets") { }

        class spell_sha_downpour_targets_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_downpour_targets_SpellScript);

            enum eSpells
            {
                HealingRain      = 73920
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_HealingRainSpellInfo = sSpellMgr->GetSpellInfo(eSpells::HealingRain);
                if (!l_HealingRainSpellInfo)
                    return;

                if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, l_Caster->GetHealingRainTrigger()))
                {
                    float l_Radius = l_HealingRainSpellInfo->Effects[EFFECT_0].CalcRadius(l_Caster);

                    p_Targets.remove_if([l_Trigger, l_Radius](WorldObject* p_Object) -> bool
                    {
                        Unit* l_Target = p_Object->ToUnit();
                        if (!l_Target || l_Target->IsFullHealth())
                            return true;

                        if (l_Trigger->GetDistance(l_Target) > l_Radius)
                            return true;

                        return false;
                    });
                }

                JadeCore::Containers::RandomResizeList(p_Targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_downpour_targets_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_downpour_targets_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_shaman_spell_scripts()
{
    new spell_sha_hot_hand();
    new PlayerScript_spectral_recovery();
    new spell_sha_thunderstorm_honor();
    new spell_sha_gift_of_the_queen();
    new spell_sha_stormstrike();
    new spell_sha_alpha_wolf();
    new spell_sha_doom_wolves_attacks();
    new spell_sha_stormlash_buff();
    new spell_sha_stormlash();
    new spell_sha_stormstrike_damage();
    new spell_sha_ghost_in_the_mist();
    new spell_sha_ghost_wolf();
    new spell_sha_doom_vortex();
    new spell_sha_stormkeeper();
    new spell_sha_unleash_doom();
    new spell_sha_unleash_doom_proc();
    new spell_sha_volcanic_inferno();
    new spell_sha_queen_ascendant();
    new spell_sha_lava_burst_reset();
    new spell_sha_lightning_rod_damages();
    new spell_sha_lightning_rod_target();
    new spell_sha_lightning_rod();
    new spell_sha_wellspring();
    new spell_sha_healing_tide_heal();
    new spell_sha_healing_tide_totem();
    new spell_sha_shamanism();
    new spell_sha_tidal_pools();
    new spell_sha_ancestral_guidance_heal();
    new spell_sha_eye_of_the_storm();
    new spell_sha_tidal_waves();
    new spell_sha_ascendance_flame();
    new spell_sha_ascendance_air();
    new spell_sha_ascendance_water();
    new spell_sha_ascendance_water_heal();
    new spell_sha_call_of_the_elements();
    new spell_sha_ancestral_guidance();
    new spell_sha_earthgrab();
    new spell_sha_stone_bulwark();
    new spell_sha_spirit_link();
    new spell_sha_lava_surge();
    new spell_sha_healing_stream();
    new spell_sha_earthquake_tick();
    new spell_sha_earthquake();
    new spell_sha_rainfall_heal();
    new spell_sha_bloodlust();
    new spell_sha_heroism();
    new spell_sha_lava_lash_spread();
    new spell_sha_windfury();
    new spell_sha_windfury_totem_buff();
    new spell_sha_feral_spirit();
    new spell_sha_pet_spirit_hunt();
    new spell_sha_chain_heal();
    new spell_sha_riptide();
    new spell_sha_cloudburst_totem();
    new spell_sha_cloudburst();
    new spell_sha_recall_cloudburst_totem();
    new spell_sha_wind_gust();
    new spell_sha_fire_blast();
    new spell_sha_WoDPvPEnhancement2PBonus();
    new spell_sha_earthen_shield_absorb();
    new spell_sha_spectral_recovery();
    new spell_sha_voodoo_hex();
    new spell_sha_hex();
    new spell_sha_seismic_storm();
    new spell_sha_t20_enhancement_2p_bonus();
    new spell_sha_cleanse_spirit();
    new spell_sha_landslide();
    new spell_sha_t20_restoration_4p_bonus();
    new spell_sha_t20_enhancement_4p_bonus();
    new spell_sha_call_lightning();
    new spell_sha_t21_elemental_2p_bonus();
    new spell_sha_t21_enhancement_2p_bonus();
    new spell_sha_t21_enhancement_4p_bonus();

    /// Elemental
    new spell_sha_chain_lightning();
    new spell_sha_earth_shock();
    new spell_sha_flame_shock();
    new spell_sha_frost_shock();
    new spell_sha_lightning_bolt();
    new spell_sha_elemental_focus();
    new spell_sha_fire_elemental();
    new spell_sha_earth_elemental();
    new spell_sha_storm_elemental();
    new spell_sha_elemental_blast();
    new spell_sha_ascendance();
    new spell_sha_earthen_rage();
    new spell_sha_earthen_rage_periodic();
    new spell_sha_liquid_magma_totem();
    new spell_sha_icefury();
    new spell_sha_lava_beam();
    new spell_sha_echoes_of_the_great_sundering();
    new spell_sha_fury_of_the_storms();
    new spell_sha_elementalist();
    new spell_sha_earthgrab_totem_root();
    new spell_sha_shamanic_healing();
    new spell_sha_shamanic_healing_triggered();
    new spell_sha_static_overload();
    new spell_sha_t20_elemental_2p_proc();
    new spell_shaman_elemental_fury_pvp();
    new spell_sha_elemental_focus_buff();

    /// Enhancement
    new spell_sha_healing_surge();
    new spell_sha_crash_lightning();
    new spell_sha_crash_lightning_aura();
    new spell_sha_stormbinger();
    new spell_sha_maelstrom_weapon();
    new spell_sha_flametongue();
    new spell_sha_enh_lightning_bolt();
    new spell_sha_feral_lunge();
    new spell_sha_feral_lunge_jump();
    new spell_sha_elemental_healing();
    new PlayerScript_aftershock();
    new spell_sha_hailstorm();
    new spell_sha_doom_winds();
    new spell_sha_rainfall();
    new spell_sha_spiritual_journey();
    new spell_sha_storm_tempests();
    new spell_sha_storm_tempests_periodic();
    new spell_sha_ethereal_form_return();
    new spell_sha_sundering();
    new spell_sha_spirit_of_the_maelstorm();
    new spell_sha_t19_enhancement_4p_bonus();
    new spell_sha_lashing_flames();
    new spell_sha_lava_lash();
    new spell_sha_winds_of_change();

    /// Restoration
    new spell_sha_resurgence();
    new spell_sha_resurgence_heal();
    new spell_sha_bottomless_depths();
    new spell_sha_healing_rain();
    new spell_sha_healing_rain_heal();
    new spell_sha_earth_shield();
    new spell_sha_undulation();
    new spell_sha_caress_of_the_tidemother();
    new spell_sha_caress_of_the_tidemother_checkproc();
    new spell_sha_deluge();
    new spell_sha_ghost_in_the_mist_proc();
    new spell_sha_electrocute();
    new spell_sha_shamanistic_healing();
    new spell_sha_sence_of_urgency();
    new spell_sha_cumulative_upkeep();
    new spell_sha_queens_decree();
    new spell_sha_tidal_waves_for_t19();
    new spell_sha_healing_stream_lego();
    new spell_sha_purify_spirit();
    new spell_sha_rainfall_targets();
    new spell_sha_downpour_targets();

    ///Honor Talent
    new PlayerScript_Calming_Waters();
    new spell_sha_counterstrike_totem();
    new spell_sha_control_of_lava();
    new spell_sha_swelling_waves();
    new spell_sha_spirit_link_aura();
    new spell_sha_static_cling();
    new spell_sha_static_cling_damage();
    new spell_sha_earthfury();
    new spell_sha_ride_the_lightning_proc();
    new spell_sha_rippling_waters_heal();
    new PlayerScript_Earthfury_visual();
    new spell_sha_skyfury_totem();
    new spell_sha_forked_lightning();

    /// PvP Modifiers
    new spell_sha_ascendance_pct();
    new spell_sha_earthen_spike_bonus();
}
#endif
