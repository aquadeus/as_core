////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_WARLOCK and SPELLFAMILY_GENERIC spells used by warlock players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warl_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "CreatureAI.h"
#include "HelperDefines.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

// Haunt (dispel effect) - 48181
class spell_warl_haunt_dispel: public SpellScriptLoader
{
    public:
        spell_warl_haunt_dispel() : SpellScriptLoader("spell_warl_haunt_dispel") { }

        class spell_warl_haunt_dispel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_haunt_dispel_AuraScript);

            void HandleDispel(DispelInfo* /*dispelInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, 100, POWER_SOUL_SHARDS);
            }

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->getDeathState() == DeathState::JUST_DIED)
                    ResetCooldown(l_Caster);
            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_warl_haunt_dispel_AuraScript::HandleDispel);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_haunt_dispel_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            void ResetCooldown(Unit* p_Caster)
            {
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    if (GetTargetApplication() && GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                    {
                        l_Player->RemoveSpellCooldown(GetSpellInfo()->Id, true);
                    }
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_haunt_dispel_AuraScript();
        }
};

// Dark Regeneration - 108359
class spell_warl_dark_regeneration: public SpellScriptLoader
{
    public:
        spell_warl_dark_regeneration() : SpellScriptLoader("spell_warl_dark_regeneration") { }

        class spell_warl_dark_regeneration_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_dark_regeneration_AuraScript);

            enum eSpells
            {
                DarkRegeneration = 108359
            };

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget())
                    if (Guardian* pet = GetTarget()->GetGuardianPet())
                        pet->CastSpell(pet, eSpells::DarkRegeneration, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_dark_regeneration_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_dark_regeneration_AuraScript();
        }
};

/// Dark Pact - 108416
class spell_warl_sacrificial_pact: public SpellScriptLoader
{
    public:
        spell_warl_sacrificial_pact() : SpellScriptLoader("spell_warl_sacrificial_pact") { }

        class spell_warl_sacrificial_pact_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_sacrificial_pact_AuraScript);

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    int32 l_SacrifiedHealth = 0;

                    if (Guardian* l_Guardian = l_Caster->GetGuardianPet())
                    {
                        l_SacrifiedHealth = l_Guardian->CountPctFromCurHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints);

                        l_Caster->AddDelayedEvent([=]() -> void
                        {
                            if (Guardian* l_Guardian = l_Caster->GetGuardianPet())
                                l_Guardian->ModifyHealth(-l_SacrifiedHealth);
                        }, 10);
                    }
                    else
                    {
                        l_SacrifiedHealth = l_Caster->CountPctFromCurHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints);

                        l_Caster->AddDelayedEvent([=]() -> void { l_Caster->ModifyHealth(-l_SacrifiedHealth); }, 10);
                    }

                    p_Amount = CalculatePct(l_SacrifiedHealth, p_AuraEffect->GetBaseAmount());
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_sacrificial_pact_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_sacrificial_pact_AuraScript();
        }
};

// Soul Swap - 86121 or Soul Swap : Exhale - 86213
class spell_warl_soul_swap: public SpellScriptLoader
{
    public:
        spell_warl_soul_swap() : SpellScriptLoader("spell_warl_soul_swap") { }

        class spell_warl_soul_swap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soul_swap_SpellScript);

            enum eSpells
            {
                Agony = 980,
                UnstableAffliction = 30108,
                SoulSwapAura = 86211,
                SoulSwapVisual = 92795,
                SoulSwap = 86121,
                SoulSwapExhale = 86213,
                CorruptionDot = 146739
            };

            SpellCastResult CheckConditions()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                /// Target should have at least one DOT from the list
                if (GetSpellInfo()->Id == eSpells::SoulSwap)
                {
                    if (!l_Target->HasAura(eSpells::Agony, l_Caster->GetGUID()) && !l_Target->HasAura(eSpells::CorruptionDot, l_Caster->GetGUID()) &&
                        !l_Target->HasAura(eSpells::UnstableAffliction, l_Caster->GetGUID()))
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }

                /// Can't copy DOTs to the same target
                if (GetSpellInfo()->Id == eSpells::SoulSwapExhale)
                {
                    if (Unit* l_SavedTarget = l_Caster->GetSoulSwapDotTarget())
                    {
                        if (l_SavedTarget == l_Target)
                            return SPELL_FAILED_BAD_TARGETS;
                    }
                }


                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                /// Soul Swap launch spell
                if (GetSpellInfo()->Id == eSpells::SoulSwap)
                {
                    /// Simple cast of Soul Swap, without Soulburn
                    l_Target->CastSpell(l_Caster, eSpells::SoulSwapVisual, true);
                    // Soul Swap override spell
                    l_Caster->CastSpell(l_Caster, eSpells::SoulSwapAura, true);
                    l_Caster->RemoveSoulSwapDOT(l_Target);

                    /// Store Soul Swap target GUID
                    l_Caster->SetSoulSwapDotTarget(l_Target->GetGUID());
                    /// Save that at the moment we don't need refresh duration
                    l_Caster->SetSoulSwapRefreshDuration(false);
                }
                /// Soul Swap finish (exhale) spell
                else if (GetSpellInfo()->Id == eSpells::SoulSwapExhale)
                {
                    l_Caster->ApplySoulSwapDOT(l_Caster, l_Target);
                    l_Caster->RemoveAurasDueToSpell(eSpells::SoulSwapAura);

                    /// Set Soul Swap target GUID to NULL
                    l_Caster->RemoveSoulSwapDotTarget();
                }
            }

            void HandleBeforeCast()
            {
                if (GetSpellInfo()->Id != eSpells::SoulSwapExhale)
                    return;
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                SpellInfo const* l_EffectInfo = sSpellMgr->GetSpellInfo(eSpells::SoulSwapExhale);
                if (!l_EffectInfo)
                    return;

                int32_t l_damage = l_Target->GetMaxHealth() * l_EffectInfo->Effects[0].BasePoints / 100;

                GetSpell()->SetSpellValue(SpellValueMod::SPELLVALUE_BASE_POINT0, l_damage);
            }

            void Register()
            {
                BeforeCast += SpellCastFn(spell_warl_soul_swap_SpellScript::HandleBeforeCast);
                OnCheckCast += SpellCheckCastFn(spell_warl_soul_swap_SpellScript::CheckConditions);
                OnHit += SpellHitFn(spell_warl_soul_swap_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_soul_swap_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Demonic Gateway - 111771
class spell_warl_demonic_gateway: public SpellScriptLoader
{
    public:
        spell_warl_demonic_gateway() : SpellScriptLoader("spell_warl_demonic_gateway") { }

        class spell_warl_demonic_gateway_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_demonic_gateway_SpellScript);

            enum eSpells
            {
                SpawnPurpleDemonicGateway   = 113890,
                SpawnGreenDemonicGateway    = 113886,
                ArenaPreparation            = 44521
            };

            enum eAreaID
            {
                GurubashiRingBattle = 2177
            };

            SpellCastResult CheckElevation()
            {
                if (!GetCaster() || !GetCaster()->ToPlayer())
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = GetCaster()->ToPlayer();

                WorldLocation* l_SpellDest = const_cast<WorldLocation*>(GetExplTargetDest());
                if (!l_SpellDest)
                    return SPELL_FAILED_DONT_REPORT;

                Map* l_Map = l_Player->GetMap();
                if (l_Map == nullptr)
                    return SPELL_FAILED_SUCCESS;

                if (l_Player->HasAura(eSpells::ArenaPreparation))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                PathGenerator l_Path(l_Player);
                l_Path.SetPathLengthLimit(80.0f);
                bool l_Result = l_Path.CalculatePath(l_SpellDest->GetPositionX(), l_SpellDest->GetPositionY(), l_SpellDest->GetPositionZ());

                if (!l_Result || !(l_Path.GetPathType() & PATHFIND_NORMAL))
                    return SPELL_FAILED_NOPATH;

                if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE)) ///< For Greymane (7.1.5 - 23420) don't allow cast from outside of the Gurubashi ring circle to the inside, and from inside to outside, but allow it inside to inside and outside to outside
                {
                    Map* l_Map = l_Player->GetMap();
                    if (!l_Map)
                        return SPELL_FAILED_DONT_REPORT;

                    uint32 l_TargetAreaId = l_Map->GetAreaId(l_SpellDest->m_positionX, l_SpellDest->m_positionY, l_SpellDest->m_positionZ);
                    if ((l_Player->GetAreaId() == eAreaID::GurubashiRingBattle) ^ (l_TargetAreaId == eAreaID::GurubashiRingBattle)) ///< this operator is an exclusive 'OR'
                        return SPELL_FAILED_NOT_HERE;
                }

                return SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->CastSpell(l_Caster, eSpells::SpawnPurpleDemonicGateway, true);

                    if (WorldLocation const* l_Dest = GetExplTargetDest())
                        l_Caster->CastSpell(l_Dest, eSpells::SpawnGreenDemonicGateway, true);
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_demonic_gateway_SpellScript::CheckElevation);
                AfterCast += SpellCastFn(spell_warl_demonic_gateway_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_demonic_gateway_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Enslave Demon - 1098
class spell_warl_enslave_demon : public SpellScriptLoader
{
    public:
        spell_warl_enslave_demon() : SpellScriptLoader("spell_warl_enslave_demon") { }

        class spell_warl_enslave_demon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_enslave_demon_SpellScript);

            SpellCastResult CheckTarget()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return SPELL_FAILED_SUCCESS;

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature)
                    return SPELL_FAILED_BAD_TARGETS;

                if (l_Creature->getLevelForTarget(l_Caster) > (l_Caster->getLevel()) || !l_Creature->GetCreatureTemplate() || l_Creature->GetCreatureTemplate()->rank > CreatureEliteType::CREATURE_ELITE_NORMAL)
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_enslave_demon_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_enslave_demon_SpellScript();
        }
};

// Fear - 5782
class spell_warl_fear: public SpellScriptLoader
{
    public:
        spell_warl_fear() : SpellScriptLoader("spell_warl_fear") { }

        class spell_warl_fear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_fear_SpellScript);

            enum eSpells
            {
                FearEffect        = 118699,
                GlyphOfFearEffect = 130616,
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FearEffect, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_warl_fear_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_fear_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Banish - 710
class spell_warl_banish: public SpellScriptLoader
{
    public:
        spell_warl_banish() : SpellScriptLoader("spell_warl_banish") { }

        class spell_warl_banish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_banish_SpellScript);

            enum eSpells
            {
                Banish = 710
            };

            void HandleBanish()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                if (l_Target->HasAura(eSpells::Banish))
                    l_Target->RemoveAura(eSpells::Banish);
            }

            void Register()
            {
                OnCast += SpellCastFn(spell_warl_banish_SpellScript::HandleBanish);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_banish_SpellScript();
        }
};

// Soulshatter - 29858
class spell_warl_soulshatter: public SpellScriptLoader
{
    public:
        spell_warl_soulshatter() : SpellScriptLoader("spell_warl_soulshatter") { }

        class spell_warl_soulshatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soulshatter_SpellScript);

            enum eSpells
            {
                Soulshatter = 32835
            };

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::Soulshatter))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (target->CanHaveThreatList() && target->getThreatManager().getThreat(caster) > 0.0f)
                            caster->CastSpell(target, eSpells::Soulshatter, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_soulshatter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_soulshatter_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Demonic Circle : Summon - 48018
class spell_warl_demonic_circle_summon: public SpellScriptLoader
{
    public:
        spell_warl_demonic_circle_summon() : SpellScriptLoader("spell_warl_demonic_circle_summon") { }

        class spell_warl_demonic_circle_summon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_circle_summon_AuraScript);

            enum eSpells
            {
                DemonicCircleSummon     = 48018,
                DemonicCircleTeleport   = 48020,
                DemonicCircleAllowCast  = 62388
            };

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes p_Mode)
            {
                if (Unit* l_Target = GetTarget())
                {
                    // If effect is removed by expire remove the summoned demonic circle too.
                    if (!(p_Mode & AURA_EFFECT_HANDLE_REAPPLY))
                        l_Target->RemoveGameObject(eSpells::DemonicCircleSummon, true);

                    if (AuraApplication* l_Circle = l_Target->GetAuraApplication(GetSpellInfo()->Id, l_Target->GetGUID()))
                        l_Circle->SendFakeAuraUpdate(eSpells::DemonicCircleAllowCast, true);
                }
            }

            void HandleDummyTick(AuraEffect const* /*aurEff*/)
            {
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DemonicCircleTeleport);

                if (l_Target == nullptr || l_SpellInfo == nullptr)
                    return;

                GameObject* l_Circle = l_Target->GetGameObject(GetId());

                if (l_Circle == nullptr)
                    return;

                // Here we check if player is in demonic circle teleport range, if so add
                // WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST; allowing him to cast the WARLOCK_DEMONIC_CIRCLE_TELEPORT.
                // If not in range remove the WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST.
                if (AuraApplication* l_CircleSummon = l_Target->GetAuraApplication(GetSpellInfo()->Id, l_Target->GetGUID()))
                {
                    if (l_Target->IsWithinDist(l_Circle, l_SpellInfo->GetMaxRange(true)))
                        l_CircleSummon->SendFakeAuraUpdate(eSpells::DemonicCircleAllowCast, false);
                    else
                        l_CircleSummon->SendFakeAuraUpdate(eSpells::DemonicCircleAllowCast, true);
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectApplyFn(spell_warl_demonic_circle_summon_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_warl_demonic_circle_summon_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_demonic_circle_summon_AuraScript();
        }
};

// Demonic Circle : Teleport - 48020
class spell_warl_demonic_circle_teleport: public SpellScriptLoader
{
    public:
        spell_warl_demonic_circle_teleport() : SpellScriptLoader("spell_warl_demonic_circle_teleport") { }

        class spell_warl_demonic_circle_teleport_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_circle_teleport_AuraScript);

            enum eSpells
            {
                DemonicCircleSummon = 48018,
                PvPAffliction4pBonus = 171379,
                PvPAffliction4pBonusEffect = 171380
            };

            void HandleTeleport(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    if (GameObject* circle = player->GetGameObject(eSpells::DemonicCircleSummon))
                    {
                        /// Item - Warlock WoD PvP Affliction 4P Bonus
                        if (player->getLevel() == 100 && player->HasAura(eSpells::PvPAffliction4pBonus))
                            player->CastSpell(player, eSpells::PvPAffliction4pBonusEffect, true);
                        player->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation());
                        player->RemoveMovementImpairingAuras();
                        player->Relocate(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation());

                        WorldPacket data(SMSG_MOVE_UPDATE);
                        player->m_movementInfo.pos.m_positionX = player->GetPositionX();
                        player->m_movementInfo.pos.m_positionY = player->GetPositionY();
                        player->m_movementInfo.pos.m_positionZ = player->GetPositionZ();
                        player->m_movementInfo.pos.m_orientation = player->GetOrientation();
                        player->m_movementInfo.time = getMSTime();
                        WorldSession::WriteMovementInfo(data, &player->m_movementInfo);
                        player->SendMessageToSet(&data, false);
                    }
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_demonic_circle_teleport_AuraScript::HandleTeleport, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_demonic_circle_teleport_AuraScript();
        }
};

// Call by Soulburn : Immolate (Periodic damage) - 157736
// Corruption (Periodic Damage) - 146739
// Siphon Life
class spell_warl_siphon_life : public SpellScriptLoader
{
    public:
        spell_warl_siphon_life() : SpellScriptLoader("spell_warl_siphon_life") { }

        class spell_warl_siphon_life_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_siphon_life_AuraScript);

            enum eSpells
            {
                T17Demonology4P = 165451,
                CorruptionDoT   = 146739,
                HandOfGuldan    = 105174,
                SpellImmolateMarker = 16003,
                Immolate        = 157736
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    /// Hand of Gul'dan now has 1 additional maximum charge, and Corruption has a 2% chance to generate 1 charge of Hand of Gul'dan when dealing damage.
                    if (GetSpellInfo()->Id == eSpells::CorruptionDoT && l_Caster->HasAura(eSpells::T17Demonology4P) && roll_chance_i(2))
                    {
                        if (Player* l_Player = l_Caster->ToPlayer())
                        {
                            if (SpellInfo const* l_HandOfGuldan = sSpellMgr->GetSpellInfo(eSpells::HandOfGuldan))
                                l_Player->RestoreCharge(l_HandOfGuldan->ChargeCategoryEntry);
                        }
                    }
                }
            }

            void OnRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();

                if (l_Target == nullptr)
                    return;

                /// Just for Immolation
                if (!p_AurEff->GetBase() || p_AurEff->GetBase()->GetId() != eSpells::Immolate)
                    return;

                if (l_Target->HasAura(eSpells::SpellImmolateMarker))
                    l_Target->RemoveAura(eSpells::SpellImmolateMarker);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_siphon_life_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_siphon_life_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_siphon_life_AuraScript();
        }
};

// Siphon Life - 63106
class spell_warl_siphon_life_heal : public SpellScriptLoader
{
public:
    spell_warl_siphon_life_heal() : SpellScriptLoader("spell_warl_siphon_life_heal") { }

    class spell_warl_siphon_life_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_siphon_life_heal_SpellScript);

        void HandleOnHit()
        {
            SetHitHeal(GetHitHeal() / 100);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_warl_siphon_life_heal_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_siphon_life_heal_SpellScript();
    }
};

/// last update : 6.1.2 19802
/// Call by Corruption - 146739 and Doom - 603
/// Item - Warlock WoD PvP Demonology 4P Bonus
class spell_warl_WodPvPDemonology4PBonus : public SpellScriptLoader
{
    public:
        spell_warl_WodPvPDemonology4PBonus() : SpellScriptLoader("spell_warl_WodPvPDemonology4PBonus") { }

        class spell_warl_WodPvPDemonology4PBonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_WodPvPDemonology4PBonus_AuraScript);

            enum eSpells
            {
                WoDPvPDemonology4PBonusAura = 165967,
                WoDPvPDemonology4PBonus     = 165968
            };

            void HandleDispel(DispelInfo* /*dispelInfo*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::WoDPvPDemonology4PBonusAura))
                    l_Caster->CastSpell(l_Caster, eSpells::WoDPvPDemonology4PBonus, true);
            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_warl_WodPvPDemonology4PBonus_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_WodPvPDemonology4PBonus_AuraScript();
        }
};


/// last update : 6.1.2 19802
/// Cataclysm - 152108
class spell_warl_cataclysm : public SpellScriptLoader
{
    public:
        spell_warl_cataclysm() : SpellScriptLoader("spell_warl_cataclysm") { }

        class spell_warl_cataclysm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_cataclysm_SpellScript);

            enum eSpells
            {
                Immolate = 157736,
                Agony = 980,
                Corrupation = 172,
                UnstableAffliction = 30108
            };

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Player == nullptr || l_Target == nullptr)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_WARLOCK_DESTRUCTION)
                    l_Player->CastSpell(l_Target, eSpells::Immolate, true);
                else if (l_Player->GetActiveSpecializationID() == SPEC_WARLOCK_AFFLICTION)
                {
                    l_Player->CastSpell(l_Target, eSpells::Agony, true);
                    l_Player->CastSpell(l_Target, eSpells::UnstableAffliction, true);
                }
                else if (l_Player->GetActiveSpecializationID() == SPEC_WARLOCK_DEMONOLOGY)
                    l_Player->CastSpell(l_Target, eSpells::Corrupation, true);
            }


            void Register()
            {
                OnHit += SpellHitFn(spell_warl_cataclysm_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_cataclysm_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Fel Firebolt - 104318, Firebolt - 3110
class spell_warl_fel_firebolt : public SpellScriptLoader
{
    public:
        spell_warl_fel_firebolt() : SpellScriptLoader("spell_warl_fel_firebolt") { }

        class spell_warl_fel_firebolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_fel_firebolt_SpellScript);

            enum eSpells
            {
                Firebolt                    = 104318,
                Immolation                  = 157736,
                FireboltDestructionPassive  = 231795,
                StolenPowerPassive          = 211530,
                StolenPowerStack            = 211529,
                JawsOfShadow                = 238109,
                JawsOfShadowAura            = 242922,
                Firebolt2                   = 3110,
                DemonicEmpowerment          = 193396,
                TheExpendables              = 211218,
                GrimoireOfService           = 216187,
                BreathOfThalkiel            = 221882,
                DestructionWarlockAura      = 137046,
                GrimoireOfSynergy           = 171982,
                EmpoweredLifeTap            = 235156
            };

            enum eDatas
            {
                WildImp     = 55659
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = l_Caster->GetOwner();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || !l_Owner)
                    return;

                if (l_Owner->HasAura(eSpells::StolenPowerPassive))
                    l_Owner->CastSpell(l_Owner, eSpells::StolenPowerStack, true);

                int32 l_Damage = GetSpellInfo()->Effects[EFFECT_0].BonusMultiplier * l_Owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL);

                l_Damage = l_Owner->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);

                /// Grimoire of Synergy mustn't be applied
                if (l_Owner->HasAura(eSpells::GrimoireOfSynergy))
                    l_Damage /= 1.25;

                /// Empowered life tap musn't be applied
                if (l_Owner->HasAura(eSpells::EmpoweredLifeTap))
                    l_Damage /= 1.10;

                l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FireboltDestructionPassive))
                {
                    if (l_Owner->HasAura(eSpells::FireboltDestructionPassive) && l_Target->HasAura(eSpells::Immolation, l_Owner->GetGUID()))
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                }

                if (SpellInfo const* l_JawsOfShadowInfo = sSpellMgr->GetSpellInfo(eSpells::JawsOfShadowAura))
                    if (l_Owner->HasAura(eSpells::JawsOfShadow) && l_Target->HasAura(eSpells::JawsOfShadowAura))
                        AddPct(l_Damage, l_JawsOfShadowInfo->Effects[EFFECT_0].BasePoints);

                if (AuraEffect* l_DemonicEffect = l_Caster->GetAuraEffect(eSpells::DemonicEmpowerment, EFFECT_3))
                    AddPct(l_Damage, l_DemonicEffect->GetAmount());

                if (AuraEffect* l_TheExpendablesEffect = l_Caster->GetAuraEffect(eSpells::TheExpendables, EFFECT_0))
                    AddPct(l_Damage, l_TheExpendablesEffect->GetAmount());

                if (SpellInfo const* l_GrimoireOfServiceInfo = sSpellMgr->GetSpellInfo(eSpells::GrimoireOfService))
                    if (l_Caster->HasAura(eSpells::GrimoireOfService))
                        AddPct(l_Damage, l_GrimoireOfServiceInfo->Effects[EFFECT_0].BasePoints);

                if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::BreathOfThalkiel, EFFECT_0))
                    AddPct(l_Damage, l_AuraEffect->GetAmount());

                if (l_Owner->HasAura(eSpells::DestructionWarlockAura))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DestructionWarlockAura))
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                }

                SetHitDamage(l_Damage);

                if (m_scriptSpellId == eSpells::Firebolt)
                {
                    Creature* l_Creature = l_Caster->ToCreature();

                    if (l_Creature == nullptr)
                        return;

                    if (l_Creature->GetEntry() == eDatas::WildImp)
                    {
                        l_Creature->AI()->DropCharge();
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_fel_firebolt_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_fel_firebolt_SpellScript();
        }
};

/// Last Udapte 7.1.5 Build 23420
/// Called by Shadow Bite - 54049
class spell_warl_shadow_bite : public SpellScriptLoader
{
    public:
        spell_warl_shadow_bite() : SpellScriptLoader("spell_warl_shadow_bite") { }

        class spell_warl_shadow_bite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_shadow_bite_SpellScript);

            enum eSpells
            {
                ShadowBiteAfflictionPassive = 231799
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = l_Caster->GetOwner();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowBiteAfflictionPassive);
                if (!l_Owner || !l_Target || !l_SpellInfo)
                    return;

                int32 l_Damage = GetHitDamage();

                if (l_Owner->HasAura(eSpells::ShadowBiteAfflictionPassive))
                {
                    float l_PctToAdd = 0.0f;
                    std::list<AuraEffect*> l_TargetDots = l_Target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (AuraEffect* l_AuraEffect : l_TargetDots)
                    {
                        if (l_AuraEffect->GetCaster() != l_Owner)
                            continue;

                        SpellInfo const* l_DotInfo = l_AuraEffect->GetSpellInfo();
                        if (!l_SpellInfo)
                            continue;

                        if (l_SpellInfo->SpellFamilyName != SPELLFAMILY_WARLOCK)
                            continue;

                        l_PctToAdd += l_SpellInfo->Effects[EFFECT_0].BasePoints;
                    }

                    AddPct(l_Damage, l_PctToAdd);
                }

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_shadow_bite_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_shadow_bite_SpellScript();
        }
};

/// Last Udapte 7.1.5 Build 23420
/// Called by Doom Bolt - 85692
class spell_warl_doom_bolt : public SpellScriptLoader
{
    public:
        spell_warl_doom_bolt() : SpellScriptLoader("spell_warl_doom_bolt") { }

        class spell_warl_doom_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_doom_bolt_SpellScript);

            enum eSpells
            {
                GrimoireOfSupremacy             = 152107,
                LeftHandOfDarkness              = 238073,
                DemonicEmpowerment              = 193396,
                MasterDemonologist              = 77219,
                GrimoireOfSynergy               = 171982,
                EmpoweredLifeTap                = 235156,
                ShadowyIncantation              = 199163,
                AfflictionWarlock               = 137043,
                ImpishIncineration              = 215273,
                TheExpendables                  = 211218,
                DestructionWarlock              = 137046
            };

            enum eArtifactPowers
            {
                LeftHandOfDarknessArtifact = 1605,
                ShadowyIncantationArtifact = 921
            };

            void HandleDamage()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                if (!GetSpellInfo())
                    return;

                int32 l_SpellPower = l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW);
                float l_Modifier = GetSpellInfo()->Effects[EFFECT_0].BonusMultiplier;
                int32 l_Damage = l_SpellPower * l_Modifier;

                l_Damage = l_Player->SpellDamageBonusDone(l_Target, GetSpellInfo(), (uint32)l_Damage, SpellEffIndex::EFFECT_0, DamageEffectType::SPELL_DIRECT_DAMAGE);

                /// Grimoire of Synergy mustn't be applied
                if (l_Owner->HasAura(eSpells::GrimoireOfSynergy))
                    l_Damage /= 1.25;

                /// Impish Incineration mustn't be applied
                if (l_Owner->HasAura(eSpells::ImpishIncineration)  && !l_Owner->HasAura(eSpells::GrimoireOfSupremacy))
                    l_Damage /= 1.18;

                /// Empowered life tap musn't be applied
                if (l_Owner->HasAura(eSpells::EmpoweredLifeTap))
                    l_Damage /= 1.10;

                /// Shadowy Incantation musn't be applied
                if (l_Player->HasAura(eSpells::ShadowyIncantation))
                    l_Damage /= 1 + (l_Player->GetRankOfArtifactPowerId(eArtifactPowers::ShadowyIncantationArtifact) / 100.0);

                /// Affliction Warlocke MUST be applied
                if (l_Owner->HasAura(eSpells::AfflictionWarlock))
                    l_Damage *= 1.14;

                /// Destruction Warlocke MUST be applied
                if (l_Owner->HasAura(eSpells::DestructionWarlock))
                    l_Damage *= 1.15;

                ///< Warlock's pets the expendables - 211218
                if (AuraEffect* l_TheExpendablesEffect = l_Caster->GetAuraEffect(eSpells::TheExpendables, EFFECT_0))
                    AddPct(l_Damage, l_TheExpendablesEffect->GetAmount());

                l_Damage = l_Target->SpellDamageBonusTaken(l_Player, GetSpellInfo(), (uint32)l_Damage, DamageEffectType::SPELL_DIRECT_DAMAGE, EFFECT_0);

                if (AuraEffect* l_DemonicEffect = l_Caster->GetAuraEffect(eSpells::DemonicEmpowerment, EFFECT_3))
                    l_Damage = AddPct(l_Damage, l_DemonicEffect->GetAmount());

                /// If target has less then 20% damage we should increase damage by 20%
                if (l_Target->GetHealthPct() <= (float)GetSpellInfo()->Effects[EFFECT_1].BasePoints)
                    AddPct(l_Damage, GetSpellInfo()->Effects[EFFECT_1].BasePoints);

                if (l_Caster->isPet() && l_Player->HasAura(eSpells::LeftHandOfDarkness))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LeftHandOfDarkness))
                    {
                        uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::LeftHandOfDarknessArtifact);
                        float l_Amount = l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank;

                        if (l_Owner->HasAura(eSpells::GrimoireOfSupremacy))
                            l_Amount /= 2;

                        if (l_Amount > 0)
                            l_Damage = AddPct(l_Damage, l_Amount);
                    }
                }

                SetHitDamage(l_Damage);
            }

            void Register()
            {
                BeforeHit += SpellHitFn(spell_warl_doom_bolt_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_doom_bolt_SpellScript();
        }
};

/// last update : 6.2.3
/// Demonic Servitude - 152107
class spell_warl_demonic_servitude : public SpellScriptLoader
{
    public:
        spell_warl_demonic_servitude() : SpellScriptLoader("spell_warl_demonic_servitude") { }

        class spell_warl_demonic_servitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_servitude_AuraScript);

            enum eSpells
            {
                GrimoireOfService   = 108501,
                SummonAbyssal       = 157899
            };

            enum eNPCs
            {
                Infernal    = 78217,
                Abyssal     = 78216,
                Terrorguard = 78215,
                DoomGuard   = 78158
            };

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (l_Player->HasSpell(eSpells::SummonAbyssal))
                    l_Player->removeSpell(eSpells::SummonAbyssal, false, false);

                Pet* l_Pet = l_Player->GetPet();

                if (l_Pet == nullptr)
                    return;

                /// Prevent Doomguard/Infernal/Abyssal to keep summon after removing the talent
                if (l_Pet->GetEntry() == eNPCs::Abyssal || l_Pet->GetEntry() == eNPCs::Infernal || l_Pet->GetEntry() == eNPCs::Terrorguard || l_Pet->GetEntry() == eNPCs::DoomGuard)
                    l_Player->GetPet()->UnSummon();
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_demonic_servitude_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_demonic_servitude_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Grimoire of Synergy - 171975
class spell_warl_grimoire_of_synergy : public SpellScriptLoader
{
    public:
        spell_warl_grimoire_of_synergy() : SpellScriptLoader("spell_warl_grimoire_of_synergy") { }

        class spell_warl_grimoire_of_synergy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_grimoire_of_synergy_AuraScript);

            enum eSpells
            {
                GrimoireofSynergyBuff = 171982
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Target = GetTarget();

                /// If procs on warlock, we should add buff on pet
                if (l_Target->IsPlayer() && l_Target->ToPlayer())
                {
                    if (Pet* l_Pet = l_Target->ToPlayer()->GetPet())
                    {
                        if (!l_Pet)
                            return;

                        l_Target->CastSpell(l_Pet, eSpells::GrimoireofSynergyBuff, true);
                    }
                }
                /// If procs on pet, we should add buff on warlock
                else if (l_Target->GetTypeId() == TYPEID_UNIT)
                {
                    if (Unit* l_Owner = l_Target->GetOwner())
                    {
                        if (!l_Owner)
                            return;

                        l_Target->CastSpell(l_Owner, eSpells::GrimoireofSynergyBuff, true);
                    }
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_warl_grimoire_of_synergy_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_grimoire_of_synergy_AuraScript();
        }
};

/// WoD PvP Demonology 2P Bonus - 171393
class PlayerScript_WoDPvPDemonology2PBonus : public PlayerScript
{
    public:
        PlayerScript_WoDPvPDemonology2PBonus() :PlayerScript("PlayerScript_WoDPvPDemonology2PBonus") { }

        enum eSpells
        {
            T17Destruction4P    = 165452,
            WoDPvPDemonology2PBonusAura = 171393,
            WoDPvPDemonology2PBonus = 171397
        };

        void OnModifyHealth(Player* p_Player, int64 /*p_Value*/, int64) override
        {
            if (p_Player->getClass() == CLASS_WARLOCK && p_Player->HasAura(eSpells::WoDPvPDemonology2PBonusAura))
            {
                if (p_Player->GetHealthPct() < 20.0f && !p_Player->HasAura(eSpells::WoDPvPDemonology2PBonus))
                    p_Player->CastSpell(p_Player, eSpells::WoDPvPDemonology2PBonus, true);

                /// Remove aura if player has more than 20% life
                if (p_Player->GetHealthPct() >= 20.0f && !(p_Player->GetPet() && p_Player->GetPet()->GetHealthPct() < 20.0f))
                    p_Player->RemoveAura(eSpells::WoDPvPDemonology2PBonus);
            }
        }
};

/// Create Healthstone - 6201
class spell_warl_create_healthstone: public SpellScriptLoader
{
    public:
        spell_warl_create_healthstone() : SpellScriptLoader("spell_warl_create_healthstone") { }

        class spell_warl_create_healthstone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_create_healthstone_SpellScript);

            enum eSpells
            {
                CreateHealthstone = 23517
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                l_Caster->CastSpell(l_Caster, CreateHealthstone, true);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_warl_create_healthstone_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_create_healthstone_SpellScript();
        }
};

/// Command Demon
/// 119905 - (Cauterize Master), 119907 - Suffering (Command Demon), 119909 - (Whiplash), 119913 - (Fellash), 119910 - (Spell Lock), 119911 - (Optical Blast), 119914 - (Felstorm), 119915 - (Wrathstorm), 171140 - (Shadow Lock), 171152 - (Meteor Strike) (Infernal), 171154 - (Meteor Strike) (Abyssal)
class spell_warl_command_demon_spells : public SpellScriptLoader
{
    public:
        spell_warl_command_demon_spells() : SpellScriptLoader("spell_warl_command_demon_spells") { }

        class spell_warl_command_demon_spells_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_command_demon_spells_SpellScript);

            enum eSpells
            {
                CauterizeMaster = 119905,
                CauterizeMasterTrigger = 119899,
                Suffering = 119907,
                SufferingTrigger = 17735,
                Whiplash = 119909,
                Fellash = 119913,
                SpellLock = 119910,
                OpticalBlast = 119911,
                Felstorm = 119914,
                Wrathstorm = 119915,
                ShadowLock = 171140,
                MeteorStrikeI = 171152,
                MeteorStrikeA = 171154
            };

            uint32 GetTriggerSpell(uint32 p_SpellId)
            {
                switch (p_SpellId)
                {
                case eSpells::CauterizeMaster:
                    return eSpells::CauterizeMasterTrigger;
                case eSpells::Suffering:
                    return eSpells::SufferingTrigger;
                case eSpells::Felstorm:
                case eSpells::Wrathstorm:
                    return GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;
                default:
                    return GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                }
            }

            SpellCastResult CheckConditions()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr || !l_Caster->IsPlayer())
                    return SPELL_FAILED_DONT_REPORT;

                Pet* l_Pet = l_Caster->ToPlayer()->GetPet();

                if (l_Pet == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Pet->HasSpellCooldown(GetTriggerSpell(GetSpellInfo()->Id)))
                {
                    l_Caster->ToPlayer()->AddSpellCooldown(m_scriptSpellId, 0, l_Pet->GetCreatureSpellCooldownDelay(GetTriggerSpell(GetSpellInfo()->Id)) * IN_MILLISECONDS, true);
                    return SPELL_FAILED_NOT_READY;
                }

                if (l_Pet->HasAurasPreventCasting())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Pet* l_Pet = l_Caster->ToPlayer()->GetPet();

                int32 l_Damage = GetEffectValue();

                switch (m_scriptSpellId)
                {
                case eSpells::CauterizeMaster: // Cauterize Master (Command Demon)
                    if (l_Target)
                        l_Pet->CastSpell(l_Target, eSpells::CauterizeMasterTrigger, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                    break;
                case eSpells::Suffering: // Suffering (Command Demon)
                    if (l_Target && !l_Caster->IsFriendlyTo(l_Target))
                        l_Pet->CastSpell(l_Target, eSpells::SufferingTrigger, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                    break;
                case eSpells::SpellLock:// Spell Lock (Command Demon)
                    if (l_Target && !l_Caster->IsFriendlyTo(l_Target))
                        l_Pet->CastSpell(l_Target, l_Damage, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                    break;
                case eSpells::OpticalBlast: // Optical Blast (Command Demon)
                    if (l_Target && !l_Caster->IsFriendlyTo(l_Target))
                        l_Pet->CastSpell(l_Target, l_Damage, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                    break;
                case eSpells::Whiplash: // Whiplash (Command Demon)
                    if (WorldLocation* pos = GetHitDest())
                        l_Pet->CastSpell(pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), l_Damage, true);
                    break;
                case eSpells::Fellash: // Fellash (Command Demon)
                    if (WorldLocation* pos = GetHitDest())
                        l_Pet->CastSpell(pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), l_Damage, true);
                    break;
                case eSpells::ShadowLock: // Shadow Lock (Command Demon)
                    if (l_Target && !l_Caster->IsFriendlyTo(l_Target))
                        l_Pet->CastSpell(l_Target, l_Damage, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
                    break;
                case eSpells::MeteorStrikeI: // Meteor Strike (Infernal) (Command Demon)
                    if (l_Target)
                        l_Pet->CastSpell(l_Target, l_Damage, true);

                    break;
                case eSpells::MeteorStrikeA: // Meteor Strike (Abyssal) (Command Demon)
                    if (l_Target)
                        l_Pet->CastSpell(l_Target, l_Damage, true);
                    break;
                default:
                    break;
                }
            }

            void HandleForceCast(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);
                Unit* l_Caster = GetCaster();
                Unit* l_Pet = GetHitUnit();
                if (!l_Pet || !l_Pet->ToPet())
                    return;

                l_Pet->CastSpell(l_Pet, GetSpellInfo()->Effects[p_EffIndex].TriggerSpell, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Pet* l_Pet = l_Caster->ToPlayer()->GetPet();

                if (l_Pet->HasSpellCooldown(GetTriggerSpell(GetSpellInfo()->Id)))
                    l_Caster->ToPlayer()->AddSpellCooldown(m_scriptSpellId, 0, l_Pet->GetCreatureSpellCooldownDelay(GetTriggerSpell(GetSpellInfo()->Id)) * IN_MILLISECONDS, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_command_demon_spells_SpellScript::CheckConditions);
                if (m_scriptSpellId != eSpells::Felstorm && m_scriptSpellId != eSpells::Wrathstorm)
                {
                    if (m_scriptSpellId == eSpells::Whiplash || m_scriptSpellId == eSpells::Fellash)
                        OnEffectHit += SpellEffectFn(spell_warl_command_demon_spells_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                    else
                        OnEffectHitTarget += SpellEffectFn(spell_warl_command_demon_spells_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                }
                else
                    OnEffectHitTarget += SpellEffectFn(spell_warl_command_demon_spells_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
                AfterCast += SpellCastFn(spell_warl_command_demon_spells_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_command_demon_spells_SpellScript();
        }
};

/// Cripple - 170995
class spell_warl_cripple_doomguard : public SpellScriptLoader
{
public:
    spell_warl_cripple_doomguard() : SpellScriptLoader("spell_warl_cripple_doomguard") { }

    class spell_warl_cripple_doomguard_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_cripple_doomguard_SpellScript);

        void HandleAfterHit()
        {
            if (!GetCaster() || !GetCaster()->ToPet())
                return;

            if (Pet* l_Pet = GetCaster()->ToPet())
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    /// 4 seconds duration in PVP
                    if (l_Target->IsPlayer())
                        if (Aura* l_Cripple = l_Target->GetAura(GetSpellInfo()->Id))
                            l_Cripple->SetDuration(4000);
                }
            }
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_warl_cripple_doomguard_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_cripple_doomguard_SpellScript();
    }
};

/// Last Update 6.2.3
/// Drain Fel Energy - 139200
class spell_warl_drain_fel_energy : public SpellScriptLoader
{
public:
    spell_warl_drain_fel_energy() : SpellScriptLoader("spell_warl_drain_fel_energy") { }

    class spell_warl_drain_fel_energy_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_drain_fel_energy_AuraScript);

        enum eSpells
        {
            CodexOfXerrath1 = 101508
        };

        void OnRemoveAura(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                return;

            l_Player->learnSpell(eSpells::CodexOfXerrath1, false);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_warl_drain_fel_energy_AuraScript::OnRemoveAura, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warl_drain_fel_energy_AuraScript();
    }
};

//////////////////////////////////////////////
////////////////              ////////////////
////////////////    LEGION    ////////////////
////////////////              ////////////////
//////////////////////////////////////////////

/// Last Update 7.0.3 Build 22293
/// Called by Immolate - 193541
class spell_warl_immolate_energize : public SpellScriptLoader
{
    public:
        spell_warl_immolate_energize() : SpellScriptLoader("spell_warl_immolate_energize") { }

        class spell_warl_immolate_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_immolate_energize_AuraScript);

            enum eSpells
            {
                Immolate            = 348,
                ImmolateDot         = 157736,
                ImmolateEnergize    = 193540
            };

            void HandleOnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                PreventDefaultAction();
                if (l_SpellInfo->Id != eSpells::Immolate && l_SpellInfo->Id != eSpells::ImmolateDot)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ImmolateEnergize, true);
                int32 l_EnergizeChance = 50;

                if (p_ProcEventInfo.GetHitMask() & ProcFlagsHit::PROC_HIT_CRITICAL)
                    if (roll_chance_i(l_EnergizeChance))
                        l_Caster->CastSpell(l_Caster, eSpells::ImmolateEnergize, true);

            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warl_immolate_energize_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_immolate_energize_AuraScript();
        }
};

/// last update : 7.0.3 Build 22522
/// Called by Immolate - 348
class spell_warl_immolate : public SpellScriptLoader
{
    public:
        spell_warl_immolate() : SpellScriptLoader("spell_warl_immolate") { }

        class spell_warl_immolate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_immolate_SpellScript);

            enum eSpells
            {
                Immolate = 157736
            };

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || l_Player == nullptr)
                    return;

                l_Player->CastSpell(l_Target, eSpells::Immolate, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_warl_immolate_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_immolate_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Immolate - 157736
/// For Channel Demonfire - 196447
class spell_warl_immolate_marker : public SpellScriptLoader
{
    public:
        spell_warl_immolate_marker() : SpellScriptLoader("spell_warl_immolate_marker") { }

        class spell_warl_immolate_marker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_immolate_marker_AuraScript);

            enum eSpells
            {
                ImmolateMarker      = 228312,
                FetorityOfSouls     = 205702,
                FierySoulsEnergize  = 205704
            };

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::ChannelDemonfireImmolatedTargets].insert(l_Target->GetGUID());
                l_Caster->CastSpell(l_Caster, eSpells::ImmolateMarker, true);

                Aura* l_Aura = l_Caster->GetAura(eSpells::ImmolateMarker);
                if (!l_Aura)
                    return;

                l_Aura->SetDuration(30 * IN_MILLISECONDS);
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                std::set<uint64>& l_ImmolatedTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::ChannelDemonfireImmolatedTargets];
                l_ImmolatedTargets.erase(l_Target->GetGUID());

                if (l_ImmolatedTargets.size())
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::ImmolateMarker);
            }

            void OnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::FetorityOfSouls))
                    if (roll_chance_i(6))
                        l_Caster->CastSpell(l_Caster, eSpells::FierySoulsEnergize, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_immolate_marker_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_immolate_marker_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_immolate_marker_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_immolate_marker_AuraScript();
        }

        class spell_warl_immolate_marker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_immolate_marker_SpellScript);

            enum eSpells
            {
            };

            void HandleHit()
            {
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_immolate_marker_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_immolate_marker_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26124
/// Called by Channel Demonfire - 196447
class spell_warl_channel_demonfire : public SpellScriptLoader
{
    public:
        spell_warl_channel_demonfire() : SpellScriptLoader("spell_warl_channel_demonfire") { }

        class spell_warl_channel_demonfire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_channel_demonfire_SpellScript);

            enum eSpells
            {
                ChannelDemonfireRange = 196449
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_RangeSpell = sSpellMgr->GetSpellInfo(eSpells::ChannelDemonfireRange);
                if (!l_Caster || !l_RangeSpell)
                    return SPELL_FAILED_DONT_REPORT;

                SpellRangeEntry const* l_Range = l_RangeSpell->RangeEntry;
                if (!l_Range)
                    return SPELL_FAILED_DONT_REPORT;

                std::set<uint64> l_Targets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::ChannelDemonfireImmolatedTargets];
                if (l_Targets.empty())
                    return SPELL_FAILED_DONT_REPORT;

                for (uint64 l_TargetGUID : l_Targets)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGUID);
                    if (!l_Target)
                        continue;

                    if (l_Caster->GetDistance(l_Target) > l_Range->maxRangeHostile)
                        continue;

                    return SPELL_CAST_OK;
                }

                return SPELL_FAILED_OUT_OF_RANGE;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_channel_demonfire_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_channel_demonfire_SpellScript();
        }

        class spell_warl_channel_demonfire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_channel_demonfire_AuraScript);

            enum eSpells
            {
                ChannelDemonfireDamage = 196448,
                ChannelDemonfireRange = 196449
            };

            void OnTick(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_RangeSpell = sSpellMgr->GetSpellInfo(eSpells::ChannelDemonfireRange);
                if (!l_Caster || !l_RangeSpell)
                    return;

                SpellRangeEntry const* l_Range = l_RangeSpell->RangeEntry;
                if (!l_Range)
                    return;

                std::set<uint64>& l_Targets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::ChannelDemonfireImmolatedTargets];
                if (l_Targets.empty())
                    return;

                std::set<uint64>::iterator l_TargetsItr = l_Targets.begin();
                std::advance(l_TargetsItr, urand(0, l_Targets.size() - 1));
                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, *l_TargetsItr);
                if (!l_Target)
                    return;

                if (l_Caster->IsWithinLOSInMap(l_Target))
                {
                    l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ChannelDemonfireCurrentTargetGUID) = l_Target->GetGUID();
                    l_Caster->CastSpell(l_Target, eSpells::ChannelDemonfireDamage, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_channel_demonfire_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_channel_demonfire_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Immolate - 157736
/// For Roaring Blaze - 205184
class spell_warl_unaura_roaring_blaze : public SpellScriptLoader
{
    public:
        spell_warl_unaura_roaring_blaze() : SpellScriptLoader("spell_warl_unaura_roaring_blaze") { }

        class spell_warl_unaura_roaring_blaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_unaura_roaring_blaze_AuraScript);

            enum eSpells
            {
                RoaringBlaze = 205690
            };

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->RemoveAurasDueToSpell(eSpells::RoaringBlaze, l_Caster->GetGUID());
                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::RoaringBlaze) = 0;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_unaura_roaring_blaze_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_unaura_roaring_blaze_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Roaring Blaze - 205184
class spell_warl_roaring_blaze : public SpellScriptLoader
{
    public:
        spell_warl_roaring_blaze() : SpellScriptLoader("spell_warl_roaring_blaze") { }

        class spell_warl_roaring_blaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_roaring_blaze_AuraScript);

            enum eSpells
            {
                Conflagrate     = 17962,
                RoaringBlaze    = 205690
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = l_DamageInfo->GetActor();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_DamageInfo || !l_Caster || !l_Target)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                if (l_SpellInfo->Id == Conflagrate)
                    l_Caster->CastSpell(l_Target, eSpells::RoaringBlaze, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warl_roaring_blaze_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_roaring_blaze_AuraScript();
        }
};


/// Last Update 7.0.3 Build 22293
/// Called by Soul Leech - 108366, 108370
class spell_warl_soul_leech : public SpellScriptLoader
{
    public:
        spell_warl_soul_leech() : SpellScriptLoader("spell_warl_soul_leech") { }

        class spell_warl_soul_leech_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_soul_leech_AuraScript);

            enum eSpells
            {
                SoulLeechShield     = 108366,
                SoulLinkBuff        = 108446,
                SoulLinkAura        = 108415,
                SoulLinkHeal        = 108447,
                DemonSkin           = 219272,
                LifeTap             = 1454,
                DemonBolt           = 157695,
                Dampening           = 110310
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Pet = nullptr;
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                SpellInfo const* l_ShieldSpellInfo = sSpellMgr->GetSpellInfo(eSpells::SoulLeechShield);
                Aura* l_Aura = GetAura();

                if (!l_Caster || !l_DamageInfo || !l_SpellInfo || !l_ShieldSpellInfo || !l_Aura)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                if ((l_DamageInfo->GetSpell() && !l_DamageInfo->GetSpell()->IsSingleEnnemyTargetSpell() && l_DamageInfo->GetSpellInfo()->Id != eSpells::DemonBolt))
                    return;

                int32 l_Damage = l_DamageInfo->GetAmount();
                if (!l_Damage)
                    return;

                std::vector<Unit*> l_Targets;
                if (l_Caster->IsPlayer())
                {
                    l_Targets.push_back(l_Caster);
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        l_Pet = l_Player->GetPet();
                        if (l_Pet)
                            l_Targets.push_back(l_Pet);
                    }
                }
                else if (Unit* l_Owner = l_Caster->GetOwner())
                {
                    l_Targets.push_back(l_Owner);
                    l_Targets.push_back(l_Caster);
                }

                int32 l_Shield = CalculatePct(l_Damage, l_AuraEffect->GetAmount());

                /// Hackfix to make Soul Leech affected by Dampening (7.1.5 build 23420)
                if (AuraEffect* l_DampEffect = l_Caster->GetAuraEffect(eSpells::Dampening, 0))
                {
                    uint32 l_DampeningAmount = l_DampEffect->GetAmount();
                    l_Shield = CalculatePct(l_Shield, 100 - l_DampeningAmount);
                }

                for (Unit* l_Target : l_Targets)
                {
                    Aura* l_ShieldAura = l_Target->GetAura(eSpells::SoulLeechShield);
                    if (!l_ShieldAura/* && !l_Caster->HasAura(eSpells::DemonSkin)*/)
                        l_ShieldAura = l_Target->AddAura(eSpells::SoulLeechShield, l_Target);

                    if (!l_ShieldAura)
                        continue;

                    int32 l_bp = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                    if (Aura* l_DemonSkinAura = l_Caster->GetAura(eSpells::DemonSkin))
                        l_bp = l_DemonSkinAura->GetEffect(EFFECT_1)->GetAmount();

                    int32 l_MaxShield = CalculatePct(l_Target->GetMaxHealth(), l_bp);
                    if (AuraEffect* l_ShieldEffect = l_ShieldAura->GetEffect(EFFECT_0))
                    {
                        int32 l_ActualShield = l_ShieldEffect->GetAmount();

                        l_ShieldEffect->SetAmount(std::min(l_ActualShield + l_Shield, l_MaxShield));
                        l_ShieldAura->SetDuration(l_ShieldAura->GetMaxDuration());
                    }
                }

                if (l_Caster->HasAura(eSpells::SoulLinkAura) && l_Pet)
                {
                    if (SpellInfo const* l_SoulLinkInfo = sSpellMgr->GetSpellInfo(eSpells::SoulLinkBuff))
                    {
                        int32 l_CasterHeal = CalculatePct(l_Shield, l_SoulLinkInfo->Effects[1].BasePoints);
                        int32 l_PetHeal = CalculatePct(l_Shield, l_SoulLinkInfo->Effects[2].BasePoints);

                        l_Caster->CastCustomSpell(l_Caster, eSpells::SoulLinkHeal, &l_CasterHeal, &l_PetHeal, nullptr, true);
                    }
                }
            }

            void HandleOnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
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

                l_Pet->CastSpell(l_Pet, GetSpellInfo()->Id, true);
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
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

                l_Pet->RemoveAurasDueToSpell(GetSpellInfo()->Id);
            }

            void OnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (p_DmgInfo.GetSpellInfo() && p_DmgInfo.GetSpellInfo()->Id == eSpells::LifeTap)
                    p_AbsorbAmount = 0;
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::SoulLeechShield)
                    OnEffectAbsorb += AuraEffectAbsorbFn(spell_warl_soul_leech_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                else
                {
                    OnProc += AuraProcFn(spell_warl_soul_leech_AuraScript::HandleOnProc);
                    AfterEffectApply += AuraEffectApplyFn(spell_warl_soul_leech_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_warl_soul_leech_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_soul_leech_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Soul Leech Pet - 228974
class spell_warl_soul_leech_pet : public SpellScriptLoader
{
public:
    spell_warl_soul_leech_pet() : SpellScriptLoader("spell_warl_soul_leech_pet") { }

    class spell_warl_soul_leech_pet_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_soul_leech_pet_AuraScript);

        enum eSpells
        {
            SoulLeechShield = 108366,
            DemonSkin = 219272,
            Dampening = 110310
        };

        void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
        {
            Unit* l_Pet = GetCaster();
            Unit* l_Owner = nullptr;
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            SpellInfo const* l_ShieldSpellInfo = sSpellMgr->GetSpellInfo(eSpells::SoulLeechShield);
            Aura* l_Aura = GetAura();

            if (!l_Pet || !l_Pet->isPet() || l_Pet->GetOwner() == nullptr || !l_DamageInfo || !l_SpellInfo || !l_ShieldSpellInfo || !l_Aura)
                return;

            l_Owner = l_Pet->GetOwner();

            AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
            if (/*l_Owner->HasAura(eSpells::DemonSkin) || */!l_AuraEffect)
                return;

            if ((l_DamageInfo->GetSpell() && !l_DamageInfo->GetSpell()->IsSingleEnnemyTargetSpell()))
                return;

            int32 l_Damage = l_DamageInfo->GetAmount();
            if (!l_Damage)
                return;

            std::vector<Unit*> l_Targets = { l_Owner, l_Pet };

            int32 l_Shield = CalculatePct(l_Damage, l_AuraEffect->GetAmount());

            if (l_Owner)
            {
                if (AuraEffect* l_DampEffect = l_Owner->GetAuraEffect(eSpells::Dampening, 0))
                {
                    uint32 l_DampeningAmount = l_DampEffect->GetAmount();
                    l_Shield = CalculatePct(l_Shield, 100 - l_DampeningAmount);
                }
            }

            for (Unit* l_Target : l_Targets)
            {
                Aura* l_ShieldAura = l_Target->GetAura(eSpells::SoulLeechShield);
                if (!l_ShieldAura)
                    l_ShieldAura = l_Target->AddAura(eSpells::SoulLeechShield, l_Target);

                if (!l_ShieldAura)
                    continue;

                int32 l_bp = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                if (Aura* l_DemonSkinAura = l_Owner->GetAura(eSpells::DemonSkin))
                    l_bp = l_DemonSkinAura->GetEffect(EFFECT_1)->GetAmount();

                int32 l_MaxShield = CalculatePct(l_Target->GetMaxHealth(), l_bp);
                if (AuraEffect* l_ShieldEffect = l_ShieldAura->GetEffect(EFFECT_0))
                {
                    int32 l_ActualShield = l_ShieldEffect->GetAmount();

                    l_ShieldEffect->SetAmount(std::min(l_ActualShield + l_Shield, l_MaxShield));
                    l_ShieldAura->SetDuration(l_ShieldAura->GetMaxDuration());
                }
            }
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_warl_soul_leech_pet_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warl_soul_leech_pet_AuraScript();
    }
};

/// Soul Leech - 228974
class PlayerScript_Soul_Leech : public PlayerScript
{
    public:
        PlayerScript_Soul_Leech() :PlayerScript("PlayerScript_Soul_Leech") { }

        enum eSpells
        {
            SoulLeechPlayer     = 108370,
            SoulLeechPet        = 228974
        };

        void OnSummonPet(Player* p_Player)
        {
            if (!p_Player || p_Player->getClass() != CLASS_WARLOCK)
                return;

            if (!p_Player->HasAura(eSpells::SoulLeechPlayer))
                return;

            Pet* l_Pet = p_Player->GetPet();
            if (!l_Pet)
                return;

            l_Pet->CastSpell(l_Pet, eSpells::SoulLeechPet, true);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Demon Skin - 219272
class spell_warl_demon_skin : public SpellScriptLoader
{
    public:
        spell_warl_demon_skin() : SpellScriptLoader("spell_warl_demon_skin") { }

        class spell_warl_demon_skin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demon_skin_AuraScript);

            enum eSpells
            {
                Dampening       = 110310,
                SoulLeechShield = 108366,
                SoulLinkBuff    = 108446,
                SoulLinkHeal    = 108447
            };

            bool Load() override
            {
                if (SpellInfo const* l_SpellInfo = GetSpellInfo())
                {
                    m_MaxShield = l_SpellInfo->Effects[SpellEffIndex::EFFECT_1].BasePoints;
                    m_HealthPct = static_cast<float>(l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints);
                }

                if (SpellInfo const* l_ShieldSpellInfo = sSpellMgr->GetSpellInfo(eSpells::SoulLeechShield))
                    m_MaxDuration = l_ShieldSpellInfo->GetMaxDuration();

                if (SpellInfo const* l_SoulLinkInfo = sSpellMgr->GetSpellInfo(eSpells::SoulLinkBuff))
                {
                    m_HealPctPlayer = l_SoulLinkInfo->Effects[SpellEffIndex::EFFECT_1].BasePoints;
                    m_HealPctPet = l_SoulLinkInfo->Effects[SpellEffIndex::EFFECT_2].BasePoints;
                }

                return true;
            }

            void OnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Pet = nullptr;
                if (!l_Caster)
                    return;

                int32 l_AddedForPlayer = RegenerateShield(l_Caster);
                int32 l_AddedForPet = 0;
                if (Player* l_Player = l_Caster->ToPlayer())
                    if (Pet* l_Pet = l_Player->GetPet())
                        l_AddedForPet = RegenerateShield(l_Pet);

                if (l_Caster->HasAura(eSpells::SoulLinkBuff))
                {
                    int32 l_CasterHeal = CalculatePct(l_AddedForPlayer, m_HealPctPlayer);
                    int32 l_PetHeal = CalculatePct(l_AddedForPet, m_HealPctPet);

                    l_Caster->CastCustomSpell(l_Caster, eSpells::SoulLinkHeal, &l_CasterHeal, &l_PetHeal, nullptr, true);
                }
            }

            void OnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SoulLeechShield);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_demon_skin_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_demon_skin_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        private:

            int32 RegenerateShield(Unit* p_Target)
            {
                Aura* l_Aura = p_Target->GetAura(eSpells::SoulLeechShield);
                if (!l_Aura)
                    l_Aura = p_Target->AddAura(eSpells::SoulLeechShield, p_Target);

                if (!l_Aura)
                    return 0;

                l_Aura->SetDuration(m_MaxDuration);

                int32 l_MaxShieldForTarget = CalculatePct(p_Target->GetMaxHealth(), m_MaxShield);
                AuraEffect* l_AuraEffect = l_Aura->GetEffect(SpellEffIndex::EFFECT_0);
                if (l_AuraEffect == nullptr)
                    return 0;

                int32 l_ActualShield = l_AuraEffect->GetAmount();
                if (l_ActualShield >= l_MaxShieldForTarget)
                    return 0;

                float l_HealthPctForTarget = m_HealthPct;
                Unit* l_Owner = p_Target->GetOwner();
                if (Player* l_Player = l_Owner ? l_Owner->ToPlayer() : p_Target->ToPlayer())
                {
                    if (l_Player->IsInPvPCombat() || l_Player->CanApplyPvPSpellModifiers())
                        AddPct(l_HealthPctForTarget, -50.0f); ///< Demon Skin now regenerates 0.5% health (down from 1.0%) per second in PvP
                }

                int32 l_Added = std::min(l_MaxShieldForTarget - l_ActualShield, (int32)CalculatePct(p_Target->GetMaxHealth(), l_HealthPctForTarget));

                if (AuraEffect* l_DampEffect = l_Owner ? l_Owner->GetAuraEffect(eSpells::Dampening, 0) : p_Target->GetAuraEffect(eSpells::Dampening, 0))
                {
                    uint32 l_DampeningAmount = l_DampEffect->GetAmount();
                    l_Added = CalculatePct(l_Added, 100 - l_DampeningAmount);
                }

                l_AuraEffect->SetAmount(l_ActualShield + l_Added);

                return l_Added;
            }

        private:

            int32 m_MaxShield = 0;
            int32 m_MaxDuration = 0;
            float m_HealthPct = 0.0f;
            int32 m_HealPctPlayer = 0;
            int32 m_HealPctPet = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_demon_skin_AuraScript();
        }
};

/// Last Update 7.3.6 Build 26365
/// Called by Incinerate - 29722
class spell_warl_incinerate : public SpellScriptLoader
{
    public:
        spell_warl_incinerate() : SpellScriptLoader("spell_warl_incinerate") { }

        class spell_warl_incinerate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_incinerate_SpellScript);

            enum eSpells
            {
                WarlockWoDPvPDestruction4PBonus = 189209,
                ImmolateDamage                  = 157736,
                FireAndBrimstone                = 196408,
                Incineration                    = 200351,
                Cremation                       = 212282,
                T20Destro2PBonus                = 242295
            };

            uint32 m_TargetsCount = 1;

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::Cremation))
                {
                    l_Caster->CastSpell(l_Target, eSpells::ImmolateDamage, true);
                }
                else if (l_Caster->HasAura(eSpells::Incineration))
                {
                    if (Aura* l_Aura = l_Target->GetAura(eSpells::ImmolateDamage, l_Caster->GetGUID()))
                        l_Aura->RefreshTimers();
                }

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::WarlockWoDPvPDestruction4PBonus, EFFECT_0))
                {
                    /// Patch 6.2.3 Hotfixes: November 17 - 23 : 4-piece PvP set bonus for Destruction Warlocks now increases damage dealt by Incinerate on targets afflicted by the casting Warlock's Immolate by 100% (up from 50%)
                    if (l_Target->HasAura(eSpells::ImmolateDamage, l_Caster->GetGUID()))
                        SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), 100));
                }

                Spell const* l_Spell = GetSpell();
                if (l_Spell && l_Spell->IsCritForTarget(l_Target))
                    if (l_Caster->ToPlayer())
                        l_Caster->ToPlayer()->ModifyPower(POWER_SOUL_SHARDS, 1);

                if (l_Caster->HasAura(eSpells::T20Destro2PBonus))
                    if (l_Caster->ToPlayer())
                        if (const SpellInfo *l_BonusInfo = sSpellMgr->GetSpellInfo(eSpells::T20Destro2PBonus))
                            l_Caster->ToPlayer()->ModifyPower(POWER_SOUL_SHARDS, l_BonusInfo->Effects[EFFECT_1].BasePoints);
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_MainTarget = GetExplTargetUnit();
                Spell const* l_Spell = GetSpell();
                if (!l_Caster || !l_MainTarget || !l_Spell)
                    return;

                if (!p_Targets.empty())
                    m_TargetsCount = p_Targets.size();

                if (!l_Caster->HasAura(eSpells::FireAndBrimstone) || l_Spell->getTriggerCastFlags())
                {
                    p_Targets.clear();
                    p_Targets.push_front(l_MainTarget);
                    m_TargetsCount = 1;
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster && l_Caster->ToPlayer())
                    l_Caster->ToPlayer()->ModifyPower(POWER_SOUL_SHARDS, m_TargetsCount * 2);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_incinerate_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_incinerate_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_warl_incinerate_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_incinerate_SpellScript();
        }
};

/// Last Update 7.1.5 Build 2252
/// Called by Rain of Fire - 42223
class spell_warl_rain_of_fire: public SpellScriptLoader
{
    public:
        spell_warl_rain_of_fire() : SpellScriptLoader("spell_warl_rain_of_fire") { }

        class spell_warl_rain_of_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_rain_of_fire_SpellScript);

            enum eSpells
            {
                AlythesssPyrogenicsAura     = 205678,
                AlythesssPyrogenicsDebuf    = 205675,
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::AlythesssPyrogenicsAura))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AlythesssPyrogenicsDebuf, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_rain_of_fire_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_rain_of_fire_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called by Havoc - 80240 - Bane Of Havoc - 200548
class spell_warl_havoc : public SpellScriptLoader
{
    public:
        spell_warl_havoc() : SpellScriptLoader("spell_warl_havoc") { }

        class spell_warl_havoc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_havoc_AuraScript);

            enum eSpells
            {
                Havoc = 80240
            };

            void HandleOnApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Unit* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (m_scriptSpellId == eSpells::Havoc)
                    l_Player->m_SpellHelper.GetUint64(eSpellHelpers::HavocTarget) = l_Target->GetGUID();
                else
                    l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::HavocTarget].insert(l_Target->GetGUID());
            }

            void HandleOnRemove(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Unit* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (m_scriptSpellId == eSpells::Havoc)
                    l_Player->m_SpellHelper.GetUint64(eSpellHelpers::HavocTarget) = 0;
                else
                    l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::HavocTarget].erase(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_havoc_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_havoc_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_havoc_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Handler for Havoc 80240
/// Called by Chaos Bolt - 116858 - Conflagrate - 17962 - Immolate - 348 - Incinerate - 29722 - Mortal Coil - 6789 - Shadowburn - 17877
class spell_warl_havoc_proc : public SpellScriptLoader
{
    public:
        spell_warl_havoc_proc() : SpellScriptLoader("spell_warl_havoc_proc") { }

        class spell_warl_havoc_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_havoc_proc_SpellScript);

            enum eSpells
            {
                Conflagrate                 = 17962,
                Incinerate                  = 29722,
                FireAndBrimstone            = 196408,
                BaneOfHavoc                 = 200546,
                ShawloftheYmirjar           = 212173,
                FocusedChaos                = 233577,
                ChaosBolt                   = 116858,
                ShadowBurn                  = 17877,
                ConflagrationOfChaosAura    = 219195,
                ConflagrationOfChaos        = 196546
            };

            bool m_PreventProc = false;

            void HandleIncinerate(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::FireAndBrimstone))
                    return;

                std::set<uint64> l_HavocTargets;
                if (l_Caster->HasSpell(eSpells::BaneOfHavoc))
                    l_HavocTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::HavocTarget];
                else
                    l_HavocTargets.insert(l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HavocTarget));

                for (WorldObject* l_Target : p_Targets)
                {
                    for (uint64 l_GUID : l_HavocTargets)
                    {
                        if (l_Target->ToUnit() && l_Target->ToUnit()->GetGUID() == l_GUID)
                        {
                            m_PreventProc = true;
                            break;
                        }
                    }
                }
            }

            void HandleOnCast()
            {
                if (m_PreventProc)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Spell const* l_Spell = GetSpell();
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_Spell || l_Spell->getTriggerCastFlags() == TriggerCastFlags(TriggerCastFlags::TRIGGERED_FULL_MASK &~ (TriggerCastFlags::TRIGGERED_DISALLOW_PROC_EVENTS | TRIGGERED_IGNORE_LOS)))
                    return;

                std::set<uint64> l_HavocTargets;
                if (l_Caster->HasSpell(eSpells::BaneOfHavoc))
                    l_HavocTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::HavocTarget];
                else
                    l_HavocTargets.insert(l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HavocTarget));

                if (!l_HavocTargets.size())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                /// Chaos Bolt damage is increased by 75%, but no longer strikes additional targets afflicted by Havoc.
                if (l_Player->HasAura(eSpells::FocusedChaos) && l_SpellInfo->Id == eSpells::ChaosBolt)
                    return;

                TriggerCastFlags triggerFlags = TriggerCastFlags(TriggerCastFlags::TRIGGERED_FULL_MASK &~(TriggerCastFlags::TRIGGERED_DISALLOW_PROC_EVENTS|TRIGGERED_IGNORE_LOS));
                if (!l_Target->IsWithinLOS(l_Caster->GetPositionX(), l_Caster->GetPositionY(), l_Caster->GetPositionZ()))
                    return;

                for (uint64 l_Guid : l_HavocTargets)
                {
                    Unit* l_HavocTarget = sObjectAccessor->GetUnit(*l_Caster, l_Guid);
                    if (!l_HavocTarget || l_HavocTarget == l_Target)
                        continue;

                    if (l_SpellInfo->Id == eSpells::Conflagrate || l_SpellInfo->Id == eSpells::ShadowBurn)
                    {
                        SpellCastTargets targets;
                        targets.SetCaster(l_Player);
                        targets.SetUnitTarget(l_HavocTarget);

                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT1, 0);

                        l_Player->CastSpell(targets, l_SpellInfo, &values, triggerFlags);
                        l_Player->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
                    }
                    else
                        l_Caster->CastSpell(l_HavocTarget, l_SpellInfo->Id, triggerFlags);
                }

                m_PreventProc = true;
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                Spell* l_Spell = GetSpell();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::ShawloftheYmirjar) || !l_Target)
                    return;

                const SpellInfo *l_AuraInfo = sSpellMgr->GetSpellInfo(eSpells::ShawloftheYmirjar);
                if (!l_AuraInfo)
                    return;

                int32 l_Damage = GetHitDamage();
                if (l_Target->GetGUID() != l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HavocTarget))
                    return;

                l_Damage = l_Damage * (1 + l_AuraInfo->Effects[0].BasePoints / 100.f);
                SetHitDamage(l_Damage);
            }

            void HandleConflagrationOfChaos()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::ConflagrationOfChaos))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_Damages = GetHitDamage();
                float l_BaseCritChance = l_Player->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE);
                AddPct(l_Damages, l_Player->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE));

                SetHitDamage(l_Damages);
                l_Caster->RemoveAura(eSpells::ConflagrationOfChaos);
            }

            void Register()
            {
                if (m_scriptSpellId == eSpells::Incinerate)
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_havoc_proc_SpellScript::HandleIncinerate, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);

                OnCast += SpellCastFn(spell_warl_havoc_proc_SpellScript::HandleOnCast);
                OnHit += SpellHitFn(spell_warl_havoc_proc_SpellScript::HandleOnHit);

                if (m_scriptSpellId == eSpells::Conflagrate || m_scriptSpellId == eSpells::ShadowBurn)
                    OnHit += SpellHitFn(spell_warl_havoc_proc_SpellScript::HandleConflagrationOfChaos);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_havoc_proc_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Eradication - 196412
class spell_warl_eradication : public SpellScriptLoader
{
    public:
        spell_warl_eradication() : SpellScriptLoader("spell_warl_eradication") { }

        class spell_warl_eradication_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_eradication_AuraScript);

            enum eSpells
            {
                ChaosBolt = 116858
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                bool l_PreventProc = true;

                if (DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo())
                {
                    if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                    {
                        if (l_SpellInfo->Id == eSpells::ChaosBolt)
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
                OnEffectProc += AuraEffectProcFn(spell_warl_eradication_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_eradication_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Dimensionnal Rift 196586
class spell_warl_dimensionnal_rift : public SpellScriptLoader
{
    public:
        spell_warl_dimensionnal_rift() : SpellScriptLoader("spell_warl_dimensionnal_rift") { }

        class spell_warl_dimensionnal_rift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_dimensionnal_rift_SpellScript);

            enum eSpells
            {
                ShadowyTear             = 196639,
                ChaosTear               = 215276,
                UnstableTear            = 187370,
                LessonsOfSpaceTime      = 236174,
                LessonsOfSpaceTimeBuff  = 236176,
                FlameRiftSummon         = 242983,
                FlameRift               = 238146
            };

            void HandleSummonRift()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 l_Spell;
                uint8 l_MaxRand = 2;

                if (l_Caster->HasAura(eSpells::FlameRift))
                    l_MaxRand = 3;

                switch (urand(0, l_MaxRand))
                {
                    case 0:
                        l_Spell = eSpells::ShadowyTear;
                        break;
                    case 1:
                        l_Spell = eSpells::ChaosTear;
                        break;
                    case 2:
                        l_Spell = eSpells::UnstableTear;
                        break;
                    case 3:
                        l_Spell = eSpells::FlameRiftSummon;
                        break;
                    default:
                        return;
                }

                l_Caster->CastSpell(l_Caster, l_Spell, true);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Spell);
                if (!l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::LessonsOfSpaceTime))
                {
                    uint32 l_Duration = l_SpellInfo->GetMaxDuration();
                    if (l_Spell == eSpells::FlameRiftSummon)
                        l_Duration = 10 * IN_MILLISECONDS;

                    if (!l_Caster->HasAura(eSpells::LessonsOfSpaceTimeBuff))
                    {
                        if (Aura* l_Aura = l_Caster->AddAura(eSpells::LessonsOfSpaceTimeBuff, l_Caster))
                        {
                            l_Aura->SetMaxDuration(l_Duration);
                            l_Aura->SetDuration(l_Duration);
                        }
                    }
                    else if (Aura* l_Aura = l_Caster->GetAura(eSpells::LessonsOfSpaceTimeBuff))
                    {
                        if (l_Aura->GetDuration() < l_Duration)
                        {
                            l_Aura->SetMaxDuration(l_Duration);
                            l_Aura->SetDuration(l_Duration);
                        }
                    }

                    for (Unit* l_Summon : l_Caster->m_Controlled)
                    {
                        if (!l_Summon->HasAura(eSpells::LessonsOfSpaceTimeBuff))
                        {
                            if (Aura* l_Aura = l_Caster->AddAura(eSpells::LessonsOfSpaceTimeBuff, l_Summon))
                            {
                                l_Aura->SetMaxDuration(l_Duration);
                                l_Aura->SetDuration(l_Duration);
                            }
                        }
                        else if (Aura* l_Aura = l_Summon->GetAura(eSpells::LessonsOfSpaceTimeBuff))
                        {
                            if (l_Aura->GetDuration() < l_Duration)
                            {
                                l_Aura->SetMaxDuration(l_Duration);
                                l_Aura->SetDuration(l_Duration);
                            }
                        }
                    }

                    if (l_Caster->ToPlayer())
                    {
                        if (Pet* l_Pet = l_Caster->ToPlayer()->GetPet())
                        {
                            if (!l_Pet->HasAura(eSpells::LessonsOfSpaceTimeBuff))
                            {
                                if (Aura* l_Aura = l_Caster->AddAura(eSpells::LessonsOfSpaceTimeBuff, l_Pet))
                                {
                                    l_Aura->SetMaxDuration(l_Duration);
                                    l_Aura->SetDuration(l_Duration);
                                }
                            }
                            else if (Aura* l_Aura = l_Pet->GetAura(eSpells::LessonsOfSpaceTimeBuff))
                            {
                                if (l_Aura->GetDuration() < l_Duration)
                                {
                                    l_Aura->SetMaxDuration(l_Duration);
                                    l_Aura->SetDuration(l_Duration);
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_dimensionnal_rift_SpellScript::HandleSummonRift);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_dimensionnal_rift_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Shadow Barrage - 196659
class spell_warl_shadow_barrage : public SpellScriptLoader
{
    public:
        spell_warl_shadow_barrage() : SpellScriptLoader("spell_warl_shadow_barrage") { }

        class spell_warl_shadow_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_shadow_barrage_AuraScript);

            enum eSpells
            {
                ShadowBolt = 196657
            };

            uint64 m_TargetGUID = 0;

            void OnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_OriginalCaster = GetCaster();
                Unit* l_Caster = GetTarget();
                if (!m_TargetGUID || !l_Caster || !l_OriginalCaster)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, m_TargetGUID);
                if (!l_Target)
                    return;

                if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
                {
                    m_TargetGUID = 0;
                    return;
                }

                l_Caster->CastSpell(l_Target, eSpells::ShadowBolt, true, nullptr, nullptr, l_OriginalCaster->GetGUID());
            }

            void SetGuid(uint32 l_Index, uint64 l_Value) override
            {
                if (l_Index == 1)
                    m_TargetGUID = l_Value;
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_shadow_barrage_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_shadow_barrage_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Chaos Barrage - 187385
class spell_warl_chaos_barrage : public SpellScriptLoader
{
    public:
        spell_warl_chaos_barrage() : SpellScriptLoader("spell_warl_chaos_barrage") { }

        class spell_warl_chaos_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_chaos_barrage_AuraScript);

            enum eSpells
            {
                ChaosBarrageMissile = 187394
            };

            uint64 m_TargetGUID = 0;

            void OnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_OriginalCaster = GetCaster();
                Unit* l_Caster = GetTarget();
                if (!m_TargetGUID || !l_Caster || !l_OriginalCaster)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, m_TargetGUID);
                if (!l_Target)
                    return;

                Aura* l_Aura = p_AuraEffect->GetBase();
                if (!l_Aura || p_AuraEffect->GetAmplitude() <= 0)
                    return;

                if (p_AuraEffect->GetTickNumber() > (l_Aura->GetMaxDuration() / p_AuraEffect->GetAmplitude()))
                    return;

                if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
                {
                    m_TargetGUID = 0;
                    return;
                }

                l_Caster->CastSpell(l_Target, eSpells::ChaosBarrageMissile, true, nullptr, nullptr, l_OriginalCaster->GetGUID());
            }

            void SetGuid(uint32 l_Index, uint64 l_Value) override
            {
                if (l_Index == 1)
                    m_TargetGUID = l_Value;
            }

            void HandleOnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura* l_Aura = p_AuraEffect->GetBase();
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Unit* l_Owner = l_Target->GetOwner();
                if (!l_Aura || !l_Owner)
                    return;

                l_Aura->SetDuration(5500);
                l_Aura->SetMaxDuration(5500);

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmplitude(250 / std::min(1.0f, l_Owner->GetFloatValue(UNIT_FIELD_MOD_HASTE)));
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_chaos_barrage_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_chaos_barrage_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_chaos_barrage_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Soul Conduit - 215941
class PlayerScript_Soul_Conduit : public PlayerScript
{
    public:
        PlayerScript_Soul_Conduit() :PlayerScript("PlayerScript_Soul_Conduit") { }

        enum eSpells
        {
            SoulConduit = 215941
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_SOUL_SHARDS || p_OldValue <= p_NewValue || p_Regen || !p_After)
                return;

            if (!p_Player->HasAura(eSpells::SoulConduit))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SoulConduit);
            if (!l_SpellInfo)
                return;

            for (uint8 l_Itr = 0; l_Itr < (p_OldValue - p_NewValue)/10; ++l_Itr)
            {
                if (!roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                    continue;

                p_Player->EnergizeBySpell(p_Player, SoulConduit, 10, POWER_SOUL_SHARDS);
            }
        }
};


/// last update : 7.1.5 Build 22293
/// Life Tap - 1454
class spell_warl_life_tap : public SpellScriptLoader
{
    public:
        spell_warl_life_tap() : SpellScriptLoader("spell_warl_life_tap") { }

        class spell_warl_life_tap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_life_tap_SpellScript);

            enum eSpells
            {
                EmpoweredlifeTap        = 235157,
                EmpoweredlifeTapBonus   = 235156,
                EternalStruggleAura     = 196305,
                EternalStruggle         = 196304
            };

            enum eArtifactPowerId
            {
                EternalStrugglePower    = 812
            };

            SpellCastResult CheckLife()
            {
                if (GetCaster()->GetHealthPct() > 10.0f)
                    return SPELL_CAST_OK;
                return SPELL_FAILED_FIZZLE;
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::EmpoweredlifeTap))
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::EmpoweredlifeTapBonus))
                {
                    /// Extended duration between 7s and 20s or more, following retail test
                    if (l_Aura->GetDuration() >= 7 * IN_MILLISECONDS)
                    {
                        l_Aura->SetMaxDuration(26 * IN_MILLISECONDS);
                        l_Aura->SetDuration(26 * IN_MILLISECONDS);
                        return;
                    }
                }

               l_Caster->CastSpell(l_Caster, eSpells::EmpoweredlifeTapBonus, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::EternalStruggleAura))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EternalStruggleAura);
                if (!l_Player || !l_SpellInfo)
                    return;


                int32 l_Bp = l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eArtifactPowerId::EternalStrugglePower);
                l_Caster->CastCustomSpell(l_Caster, eSpells::EternalStruggle, &l_Bp, nullptr, nullptr, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_life_tap_SpellScript::CheckLife);
                OnCast += SpellCastFn(spell_warl_life_tap_SpellScript::HandleOnCast);
                AfterCast += SpellCastFn(spell_warl_life_tap_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_life_tap_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called by Health Funnel - 755
class spell_warl_health_funnel : public SpellScriptLoader
{
    public:
        spell_warl_health_funnel() : SpellScriptLoader("spell_warl_health_funnel") { }

        class spell_warl_health_funnel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_health_funnel_AuraScript);

            enum eSpells
            {
                HealthFunnel = 755
            };

            void HandleOnTick(AuraEffect const* /*p_Auraeffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                Pet* l_Pet = l_Target->ToPet();
                if (!l_Player || !l_Pet || l_Pet->GetOwner() != l_Player)
                    return;

                int32 l_TransferredLife = l_Caster->GetMaxHealth() * ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints / 100.0f);

                if (l_Player->GetHealth() <= l_TransferredLife || l_Pet->GetHealthPct() == 100)
                {
                    l_Pet->RemoveAurasDueToSpell(eSpells::HealthFunnel);
                    return;
                }

                l_Player->ModifyHealth(-l_TransferredLife);
                l_Pet->ModifyHealth(2 * l_TransferredLife);

                if (l_Player->GetHealth() <= l_TransferredLife || l_Pet->GetHealthPct() == 100)
                {
                    l_Pet->RemoveAurasDueToSpell(eSpells::HealthFunnel);
                    return;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_health_funnel_AuraScript::HandleOnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_health_funnel_AuraScript();
        }

        class spell_warl_health_funnel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_health_funnel_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                Player* l_Player = l_Caster->ToPlayer();
                Pet* l_Pet = l_Player->GetPet();
                if (!l_Player || !l_Pet || l_Pet->GetOwner() != l_Player)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                if (l_Player->GetHealthPct() < (float)l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    return SPELL_FAILED_NO_POWER;

                if (l_Pet->GetHealthPct() == 100)
                    return SPELL_FAILED_ALREADY_AT_FULL_HEALTH;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_health_funnel_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_health_funnel_SpellScript();
        }
};

/// Last Update 7.3.5
/// Called by Grimoire of Sacrifice - 108503
class spell_warl_grimoire_of_sacrifice : public SpellScriptLoader
{
    public:
        spell_warl_grimoire_of_sacrifice() : SpellScriptLoader("spell_warl_grimoire_of_sacrifice") { }

        class spell_warl_grimoire_of_sacrifice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_grimoire_of_sacrifice_SpellScript);

            enum eSpells
            {
                DemonicPower = 196099
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DemonicPower, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_grimoire_of_sacrifice_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_grimoire_of_sacrifice_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Grimoire of Sacrifice - 108503
class PlayerScript_Grimoire_Of_Sacrifice : public PlayerScript
{
    public:
        PlayerScript_Grimoire_Of_Sacrifice() : PlayerScript("PlayerScript_Grimoire_Of_Sacrifice") { }

        enum eSpells
        {
            GrimoireOfSacrifice = 108503,
            DemonicPower        = 196099
        };

        void OnSpellRemoved(Player* p_Player, uint32 p_SpellID)
        {
            if (p_Player && p_SpellID == eSpells::GrimoireOfSacrifice)
                p_Player->RemoveAura(eSpells::DemonicPower);
        }

        void OnSummonPet(Player* p_Player)
        {
            if (p_Player && p_Player->getClass() == CLASS_WARLOCK)
                p_Player->RemoveAura(eSpells::DemonicPower);
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Agony - 980
class spell_warl_agony : public SpellScriptLoader
{
    public:
        spell_warl_agony() : SpellScriptLoader("spell_warl_agony") { }

        class spell_warl_agony_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_agony_AuraScript);

            enum eSpells
            {
                T19Affliction4P = 212003
            };

            void HandleEffectApply(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AgonyTargetList].insert(l_Target->GetGUID());

                float l_RandomValue = frand(0.0f, 0.99f);

                /// Item - Warlock T19 Affliction 4P Bonus
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Affliction4P, EFFECT_0))
                    l_RandomValue *= ((float)l_AuraEffect->GetAmount() / 100.0f) + 1.0f;

                if (l_RandomValue > 0.99f)
                    l_RandomValue = 0.99f;

                l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonyRandomValue) = l_RandomValue;
                l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonyChance) += m_AdditionChance;
                l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonySavedValue) = l_RandomValue;
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Target)
                    return;

                bool l_ShardEnergised = false;
                uint8 l_NumberOfAffectedTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AgonyTargetList].size();
                if (l_NumberOfAffectedTargets > 4)
                    l_NumberOfAffectedTargets = 4;

                if (Aura* l_Agony = l_Target->GetAura(p_AurEff->GetSpellInfo()->Id, l_Caster->GetGUID()))
                {
                    l_Agony->ModStackAmount(p_AurEff->GetBaseAmount());

                    /// Patch 6.2.2 (2015-09-01): Now deals 10 % less damage in PvP combat.
                    if (l_Target->IsPlayer() && p_AurEff->GetTickNumber() == 1)
                        l_Agony->GetEffect(EFFECT_0)->ChangeAmount(l_Agony->GetEffect(EFFECT_0)->GetAmount() - CalculatePct(l_Agony->GetEffect(EFFECT_0)->GetAmount(), 10));
                }

                if (l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonySavedValue) >= 1.00f)
                {
                    l_Caster->ModifyPower(POWER_SOUL_SHARDS, 10);
                    l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonySavedValue) -= 1.0f;
                    l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonyChance) = 0.0f;
                    l_ShardEnergised = true;
                }

                if (l_NumberOfAffectedTargets == 1)
                    l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonySavedValue) += m_AdditionAccumulate;
                else
                    ///l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonySavedValue) += m_AdditionAccumulate + (frand(0.0f, 0.99f) * m_ConstValue / (sqrt(l_NumberOfAffectedTargets))); trying something else, cuz it's fuzzy
                    l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonySavedValue) += (m_AdditionAccumulate / sqrt(l_NumberOfAffectedTargets));

                if (l_ShardEnergised)
                    return;

                if (roll_chance_f(l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonyChance)))
                {
                    l_Caster->ModifyPower(POWER_SOUL_SHARDS, 10);
                    l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonyChance) = 0.0f;
                }
                else
                    l_Caster->m_SpellHelper.GetFloat(eSpellHelpers::AgonyChance) += m_AdditionChance;
            }

            bool CanRefreshProcDummy(bool p_Decrease)
            {
                return false;
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AgonyTargetList].erase(l_Target->GetGUID());
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_warl_agony_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_agony_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                CanRefreshProc += AuraCanRefreshProcFn(spell_warl_agony_AuraScript::CanRefreshProcDummy);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_agony_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            float m_AdditionChance = 2.5f;
            float m_AdditionAccumulate = 0.16f;
            const float m_ConstValue = 0.32f;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_agony_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Seed Of Corruption 27243
class spell_warl_seed_of_corruption : public SpellScriptLoader
{
    public:
        spell_warl_seed_of_corruption() : SpellScriptLoader("spell_warl_seed_of_corruption") { }

        class spell_warl_seed_of_corruption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_seed_of_corruption_AuraScript);

            enum eSpells
            {
                SeedOfCorruption = 27243,
                SeedOfCorruptionDetonate = 27285
            };

            int32 m_DamageTaken = 0;

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster ||!l_Target)
                    return;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                int32 l_Damage = l_DamageInfo->GetAmount();
                m_DamageTaken += l_Damage;
                if (m_DamageTaken >= l_Player->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + 5) ||
                    (l_Spell && l_Spell->GetSpellInfo() && l_Spell->GetSpellInfo()->Id == eSpells::SeedOfCorruptionDetonate && l_Spell->GetCaster() == l_Caster))
                    l_Target->RemoveAurasDueToSpell(eSpells::SeedOfCorruption, l_Player->GetGUID(), 0, AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL);
            }

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->DelayedCastSpell(l_Target, eSpells::SeedOfCorruptionDetonate, true, 1);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warl_seed_of_corruption_AuraScript::HandleOnProc);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_seed_of_corruption_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_seed_of_corruption_AuraScript();
        }

        class spell_warl_seed_of_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_seed_of_corruption_SpellScript);

            enum eSpells
            {
                SowTheSeed = 196226,
                Nightfall = 248814
            };

            enum eNpc
            {
                SoulEffigy = 103679
            };

            std::array<SpellEffIndex, 2> m_Effects =
            {
                {
                    EFFECT_1,
                    EFFECT_2
                }
            };

            bool m_SowTheSeeds = false;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target || !l_Caster)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::SowTheSeed, EFFECT_0);
                if (l_AuraEffect && l_Caster->GetPower(POWER_SOUL_SHARDS) > 0)
                {
                    p_Targets.remove_if([l_Caster, l_Target](WorldObject* l_TargetObj) -> bool
                    {
                        Unit* l_TargetUnit = l_TargetObj->ToUnit();
                        if (!l_TargetUnit || l_TargetUnit == l_Caster || l_TargetUnit == l_Target)
                            return true;

                        return false;
                    });

                    m_SowTheSeeds = true;
                    p_Targets.resize(l_AuraEffect->GetAmount());
                    p_Targets.push_back(l_Target);
                    return;
                }

                p_Targets = { l_Target };
            }

            void TakePowers(Powers p_Power, int32& p_Amount)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (p_Power == POWER_MANA && m_SowTheSeeds)
                    l_Caster->ModifyPower(POWER_SOUL_SHARDS, -10);
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::Nightfall);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_warl_seed_of_corruption_SpellScript::TakePowers);
                OnCast += SpellCastFn(spell_warl_seed_of_corruption_SpellScript::HandleOnCast);

                for (SpellEffIndex l_EffectIndex : m_Effects)
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_seed_of_corruption_SpellScript::FilterTargets, l_EffectIndex, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_seed_of_corruption_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Seed Of Corruption 27285
class spell_warl_seed_of_corruption_detonate : public SpellScriptLoader
{
    public:
        spell_warl_seed_of_corruption_detonate() : SpellScriptLoader("spell_warl_seed_of_corruption_detonate") { }

        class spell_warl_seed_of_corruption_detonate_SpellSCript : public SpellScript
        {
            PrepareSpellScript(spell_warl_seed_of_corruption_detonate_SpellSCript);

            enum eSpells
            {
                Corruption      = 172,
                DeathsEmbrace   = 234876
            };

            enum eNpc
            {
                SoulEffigy = 103679
            };

            void HandleOnHitSeed()
            {
                Creature* l_Target = GetHitCreature();
                if (!l_Target)
                    return;

                if (l_Target->GetEntry() == eNpc::SoulEffigy)
                    SetHitDamage(0);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Target->GetEntry() != eNpc::SoulEffigy)
                    l_Caster->CastSpell(l_Target, eSpells::Corruption, true);

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::DeathsEmbrace))
                {
                    if (SpellInfo const* l_DeathEmbraceInfo = sSpellMgr->GetSpellInfo(eSpells::DeathsEmbrace))
                    {
                        uint32 l_Damage = GetHitDamage();
                        float l_Threshold = (float)l_DeathEmbraceInfo->Effects[EFFECT_1].BasePoints;
                        if (l_Target->GetHealthPct() < l_Threshold)
                        {
                            l_Damage *= 1 + (((float)l_DeathEmbraceInfo->Effects[EFFECT_0].BasePoints / 100.f) * (1 - (l_Target->GetHealthPct() / 100.f) / (l_Threshold / 100.f)));
                            SetHitDamage(l_Damage);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_seed_of_corruption_detonate_SpellSCript::HandleOnHitSeed);
                OnEffectHitTarget += SpellEffectFn(spell_warl_seed_of_corruption_detonate_SpellSCript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_seed_of_corruption_detonate_SpellSCript();
        }
};

/// Last Update 7.1.5 B Build 23420
/// Called By Unstable Affliction - 30108
class spell_warl_unstable_affliction : public SpellScriptLoader
{
    public:
        spell_warl_unstable_affliction() : SpellScriptLoader("spell_warl_unstable_affliction") { }

        class spell_warl_unstable_affliction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_unstable_affliction_SpellScript);

            enum eSpells
            {
                FisrtUnstableAffliction     = 233490,
                SecondUnstableAffliction    = 233496,
                ThirdUnstableAffliction     = 233497,
                FourthUnstableAffliction    = 233498,
                FifthUnstableAffliction     = 233499,
                PowerCordOfLethrendisAura   = 205753,
                PowerCordOfLethrendis       = 205756,
                Contagion                   = 196105,
                ContagionDamages            = 233494
            };

            enum eDatas
            {
                MaxUnstableAffliction       = 5
            };

            void HandleAfterCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::PowerCordOfLethrendisAura);
                if (!l_Target || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::Contagion))
                    l_Caster->CastSpell(l_Target, eSpells::ContagionDamages, true);

                std::array<std::pair<uint32, Aura*>, eDatas::MaxUnstableAffliction> l_UnstableAfflitions =
                {
                    {std::make_pair<uint32, Aura*>(eSpells::FisrtUnstableAffliction,    nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::SecondUnstableAffliction,   nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::ThirdUnstableAffliction,    nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::FourthUnstableAffliction,   nullptr),
                    std::make_pair<uint32, Aura*>(eSpells::FifthUnstableAffliction,    nullptr)}
                };

                if (l_Caster->HasAura(eSpells::PowerCordOfLethrendisAura) && !l_Target->HasAura(eSpells::FisrtUnstableAffliction))
                    if (roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                        l_Caster->CastSpell(l_Caster, eSpells::PowerCordOfLethrendis, true);

                std::pair<uint32, int32> l_LessDurationIndex = { 0, -1};
                for (uint32 l_I = 0; l_I < l_UnstableAfflitions.size(); ++l_I)
                {
                    l_UnstableAfflitions[l_I].second = l_Target->GetAura(l_UnstableAfflitions[l_I].first, l_Caster->GetGUID());
                    if (l_UnstableAfflitions[l_I].second == nullptr)
                    {
                        l_Caster->CastSpell(l_Target, l_UnstableAfflitions[l_I].first, true);
                        return;
                    }

                    if (l_LessDurationIndex.second == -1 || l_UnstableAfflitions[l_I].second->GetDuration() < l_LessDurationIndex.second)
                        l_LessDurationIndex = { l_I, l_UnstableAfflitions[l_I].second->GetDuration() };
                }

                l_Caster->CastSpell(l_Target, l_UnstableAfflitions[l_LessDurationIndex.first].first, true);

                if (l_LessDurationIndex.second == -1)
                {
                    if (l_Caster->HasAura(eSpells::PowerCordOfLethrendisAura))
                    {
                        const SpellInfo *l_PowerCordInfo = sSpellMgr->GetSpellInfo(eSpells::PowerCordOfLethrendisAura);

                        if (l_PowerCordInfo)
                        {
                            int l_Rand = urand(0, (100 / l_PowerCordInfo->Effects[0].BasePoints) - 1);
                            if (!l_Rand)
                                l_Caster->CastSpell(l_Caster, eSpells::PowerCordOfLethrendis);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_unstable_affliction_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_unstable_affliction_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Unstable Afflictions - 233490 - 233496 - 233497 - 233498 - 233499
class spell_warl_unstable_affliction_dispell : public SpellScriptLoader
{
    public:
        spell_warl_unstable_affliction_dispell() : SpellScriptLoader("spell_warl_unstable_affliction_dispell") { }

        class spell_warl_unstable_affliction_dispell_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_unstable_affliction_dispell_AuraScript);

            enum eSpells
            {
                DispelDamage                = 196364,
                ContagionDamages            = 233494,
                FisrtUnstableAffliction     = 233490,
                SecondUnstableAffliction    = 233496,
                ThirdUnstableAffliction     = 233497,
                FourthUnstableAffliction    = 233498,
                FifthUnstableAffliction     = 233499,
                FatalEchoes                 = 199257,
                UnstableAffliction          = 30108,
                SoulRipBuff                 = 216708,
                UnstableAfflictionShard     = 231791,
                CrystallineShadows          = 221862,
                ShadowyIncantation          = 199163,
                ReapSoul                    = 216708
            };

            enum eTraits
            {
                Crystalline     = 1353,
                Shadowy         = 921
            };

            enum eDatas
            {
                RefundShardDone = 1
            };

            int32 m_TotalDamages = 0;
            std::vector<eSpells> m_UnstableAfflictions =
            {
                eSpells::FisrtUnstableAffliction,
                eSpells::SecondUnstableAffliction,
                eSpells::ThirdUnstableAffliction,
                eSpells::FourthUnstableAffliction,
                eSpells::FifthUnstableAffliction
            };

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura const* l_Aura = GetAura();
                Unit* l_Caster = GetCaster();
                if (!l_Aura || !l_Caster || !p_AuraEffect->GetAmplitude())
                    return;

                /// If dispelled, deals [(96.6% of Spell power) * 400 / 100] damage to the dispeller and silences them for 4 sec.
                m_TotalDamages = l_Caster->GetStat(STAT_INTELLECT);
                m_TotalDamages = CalculatePct(m_TotalDamages, 96.6) * 4;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_CrystallineShadowsInfo = sSpellMgr->GetSpellInfo(eSpells::CrystallineShadows);
                SpellInfo const* l_ShadowyIncantationInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowyIncantation);
                if (!l_Player || !l_ShadowyIncantationInfo || !l_CrystallineShadowsInfo)
                    return;

                uint8 l_ReapSoul = l_Player->HasAura(eSpells::ReapSoul) ? 2 : 1;

                AddPct(m_TotalDamages, l_ReapSoul == 2 ? l_Player->GetAura(eSpells::ReapSoul)->GetEffect(0)->GetAmount() : 0);
                AddPct(m_TotalDamages, l_ReapSoul * l_CrystallineShadowsInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eTraits::Crystalline));
                AddPct(m_TotalDamages, l_ReapSoul * l_ShadowyIncantationInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eTraits::Shadowy));
                AddPct(m_TotalDamages, l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * 3.16 );
            }

            void HandleDispel(DispelInfo* p_DispelInfo)
            {
                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();
                Unit* l_Dispeller = p_DispelInfo->GetDispeller();
                if (!l_Caster || !l_Dispeller)
                    return;

                uint8 l_AurasCount = 1;
                for (auto l_Aura : m_UnstableAfflictions)
                {
                    if (l_Owner->HasAura(l_Aura))
                    {
                        l_Owner->RemoveAura(l_Aura);
                        ++l_AurasCount;
                    }
                }
                m_TotalDamages *= l_AurasCount;

                l_Caster->CastCustomSpell(l_Dispeller, eSpells::DispelDamage, &m_TotalDamages, nullptr, nullptr, true);
            }

            void HandleAfterRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FatalEchoes);
                if (!l_Target || !l_Caster || !l_SpellInfo)
                    return;

                RefundSoulShard(l_Caster, l_Target);

                if (l_Caster->HasAura(eSpells::FatalEchoes) && roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints * (l_Caster->HasAura(eSpells::SoulRipBuff) ? 2 : 1)))
                    l_Caster->CastSpell(l_Target, eSpells::UnstableAffliction, true);

                if (!l_Target->HasAura(eSpells::ContagionDamages))
                    return;

                for (auto l_Aura : m_UnstableAfflictions)
                    if (l_Target->HasAura(l_Aura, l_Caster->GetGUID()))
                        return;

                l_Target->RemoveAura(eSpells::ContagionDamages);
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                if (p_Type == eDatas::RefundShardDone && p_Data == 1)
                    m_RefundShardDone = true;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_unstable_affliction_dispell_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterDispel += AuraDispelFn(spell_warl_unstable_affliction_dispell_AuraScript::HandleDispel);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_unstable_affliction_dispell_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        private:

            void RefundSoulShard(Unit* p_Caster, Unit* p_Target)
            {
                if (!m_RefundShardDone && p_Caster->HasSpell(eSpells::UnstableAfflictionShard) && GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                {
                    p_Caster->EnergizeBySpell(p_Caster, eSpells::UnstableAffliction, 10, Powers::POWER_SOUL_SHARDS);

                    SendRefundDone(p_Target);
                }
            }

            void SendRefundDone(Unit* p_Target)
            {
                Unit::AuraApplicationMap & l_Auras = p_Target->GetAppliedAuras();
                for (auto l_Aura : l_Auras)
                {
                    if (!std::any_of(m_UnstableAfflictions.begin(), m_UnstableAfflictions.end(),
                        [l_AuraId = l_Aura.second->GetBase()->GetId()](auto& p_UnstableAfflictionId) { return l_AuraId == p_UnstableAfflictionId; }))
                        continue;

                    l_Aura.second->GetBase()->SetScriptData(eDatas::RefundShardDone, 1);
                }
            }

            bool m_RefundShardDone = false;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_unstable_affliction_dispell_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called By Phantom Singularity - 205179
class spell_warl_phantom_singularity : public SpellScriptLoader
{
    public:
        spell_warl_phantom_singularity() : SpellScriptLoader("spell_warl_phantom_singularity") { }

        class spell_warl_phantom_singularity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_phantom_singularity_AuraScript);

            enum eSpells
            {
                PhantomSingularityDamage = 205246
            };

            void HandleTickDamage(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PhantomSingularityDamage, true);
            }

            void CalculateMaxDuration(int32& p_Duration)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                float l_HasteMod = 1.0f / l_Player->GetFloatValue(UNIT_FIELD_MOD_HASTE);

                p_Duration /= l_HasteMod;
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_warl_phantom_singularity_AuraScript::CalculateMaxDuration);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_phantom_singularity_AuraScript::HandleTickDamage, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_phantom_singularity_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Phantom Singularity - 205246
class spell_warl_phantom_singularity_damage : public SpellScriptLoader
{
    public:
        spell_warl_phantom_singularity_damage() : SpellScriptLoader("spell_warl_phantom_singularity_damage") { }

        class spell_warl_phantom_singularity_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_phantom_singularity_damage_SpellScript);

            enum eSpells
            {
                DeathsEmbrace = 234876
            };

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                Spell* l_Spell = GetSpell();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Target || !l_Spell || !l_Caster || !l_SpellInfo)
                    return;

                bool crit = l_Spell->IsCritForTarget(l_Target);
                uint32 bp = float(l_SpellInfo->Effects[EFFECT_1].CalcValueMultiplier(l_Caster, l_Spell)) * 100;
                uint32 heal = CalculatePct(GetHitDamageForLeechEffect(), bp);

                if (crit)
                    heal = l_Caster->SpellCriticalHealingBonus(l_SpellInfo, heal, l_Target);

                l_Caster->HealBySpell(l_Caster, l_SpellInfo, heal, crit);
            }

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                int32 l_Damage = GetHitDamage() / 5;

                Aura* l_Aura = l_Caster->GetAura(eSpells::DeathsEmbrace);
                if (!l_Aura)
                    return;

                AuraEffect* l_AuraEff1 = l_Aura->GetEffect(EFFECT_1);
                if (!l_AuraEff1)
                    return;

                float l_Threshold = l_AuraEff1->GetAmount();

                AuraEffect* l_AuraEff0 = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEff0)
                    return;

                if (l_Target->GetHealthPct() < l_Threshold)
                    l_Damage *= 1 + (((float)l_AuraEff0->GetAmount() / 100.f) * (1 - (l_Target->GetHealthPct() / 100.f) / (l_Threshold / 100.f)));

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_phantom_singularity_damage_SpellScript::HandleHit);
                OnEffectHitTarget += SpellEffectFn(spell_warl_phantom_singularity_damage_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_HEALTH_LEECH);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_phantom_singularity_damage_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Corruption - 146739
/// For Absolute Corruption - 196103
class spell_warl_absolute_corruption : public SpellScriptLoader
{
    public:
        spell_warl_absolute_corruption() : SpellScriptLoader("spell_warl_absolute_corruption") { }

        class spell_warl_absolute_corruption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_absolute_corruption_AuraScript);

            enum eSpells
            {
                AbsoluteCorruption = 196103
            };

            void HandleDuration(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_AbsoluteCorruption = sSpellMgr->GetSpellInfo(eSpells::AbsoluteCorruption);
                Aura* l_Corruption = p_AuraEffect->GetBase();
                if (!l_Caster || !l_Target || !l_AbsoluteCorruption || !l_Corruption)
                    return;

                if (!l_Caster->HasAura(eSpells::AbsoluteCorruption))
                    return;

                if (l_Target->GetSpellModOwner())
                {
                    l_Corruption->SetMaxDuration(l_AbsoluteCorruption->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                    l_Corruption->SetDuration(l_AbsoluteCorruption->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                }
                else
                {
                    l_Corruption->SetMaxDuration(-1);
                    l_Corruption->SetDuration(-1);
                }
            }

            bool CanRefresh(bool p_Decrease)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster && !l_Caster->HasAura(eSpells::AbsoluteCorruption))
                    return true;

                return false;
            }

            void Register() override
            {
                CanRefreshProc += AuraCanRefreshProcFn(spell_warl_absolute_corruption_AuraScript::CanRefresh);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_absolute_corruption_AuraScript::HandleDuration, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_absolute_corruption_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Call Dreadstalkers - 104316
class spell_warl_call_dreadstalkers : public SpellScriptLoader
{
    public:
        spell_warl_call_dreadstalkers() : SpellScriptLoader("spell_warl_call_dreadstalkers") { }

        class spell_warl_call_dreadstalkers_SpellScipt : public SpellScript
        {
            PrepareSpellScript(spell_warl_call_dreadstalkers_SpellScipt);

            enum eSpells
            {
                DreadstalkersSummonLeft     = 193331,
                DreadstalkersSummonRight    = 193332,
                ImprovedDreadstalkers       = 196272,
                SummonWildImpLeft           = 196274,
                SummonWildImpRight          = 196273,
                WakenersLoyalty             = 236199,
                WakenersLoyaltyBuff         = 236200,
                T20Demon4PBonus             = 242294
            };

            void HandleTakePowers(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != Powers::POWER_SOUL_SHARDS)
                    return;

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                const SpellPowerEntry* l_PowerEntry = nullptr;

                if (p_PowerCost > 0 || !l_Caster || !l_Caster->HasAura(eSpells::WakenersLoyalty) || !l_SpellInfo || !(l_PowerEntry = l_SpellInfo->GetSpellPower(POWER_SOUL_SHARDS)))
                    return;

                int32 l_PowerCost = 2;

                for (uint8 l_Itr = 0; l_Itr < l_PowerCost; ++l_Itr)
                    l_Caster->CastSpell(l_Caster, eSpells::WakenersLoyaltyBuff, true);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::T20Demon4PBonus))
                {
                    const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T20Demon4PBonus);
                    if (!l_SpellInfo)
                        return;

                    l_Caster->CastSpell(l_Caster, l_SpellInfo->Effects[EFFECT_0].TriggerSpell, true);
                }

                Unit* l_Target = GetExplTargetUnit();

                if (!l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::Dreadstalkers) = l_Target->GetGUID();

                l_Caster->CastSpell(l_Caster, eSpells::DreadstalkersSummonLeft, true);
                l_Caster->CastSpell(l_Caster, eSpells::DreadstalkersSummonRight, true);

                if (l_Caster->m_Controlled.empty())
                    return;

                uint64 l_MinionGUID = l_Caster->GetLastMinionSpawn();
                Unit* l_Creature = sObjectAccessor->GetUnit(*l_Caster, l_MinionGUID);

                if (l_Creature && l_Caster && l_Caster->IsPlayer())
                    l_Caster->ToPlayer()->SendTempSummonUITimer(l_Creature);

                if (!l_Caster->HasAura(eSpells::ImprovedDreadstalkers))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SummonWildImpLeft, true);
                l_Caster->CastSpell(l_Target, eSpells::SummonWildImpRight, true);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_warl_call_dreadstalkers_SpellScipt::HandleTakePowers);
                AfterCast += SpellCastFn(spell_warl_call_dreadstalkers_SpellScipt::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_call_dreadstalkers_SpellScipt();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Demonwrath - 193439
class spell_warl_demonwrath : public SpellScriptLoader
{
    public:
        spell_warl_demonwrath() : SpellScriptLoader("spell_warl_demonwrath") { }

        class spell_warl_demonwrath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_demonwrath_SpellScript);

            enum eSpells
            {
                Demonwrath = 193440,
                DemonicCalling = 205145,
                DemonicCallingTrigger = 205146
            };

            SpellInfo const* m_SpellInfo;

            bool Load() override
            {
                m_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Demonwrath);
                if (m_SpellInfo)
                    return true;
                return false;
            }

            void FilterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<WorldObject*> l_RealTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    bool l_RemoveTarget = true;
                    for (Unit* l_Summon : l_Caster->m_Controlled)
                    {
                        if (l_Summon->GetDistance(l_Target) < m_SpellInfo->Effects[EFFECT_0].BasePoints)
                            l_RemoveTarget = false;
                    }
                    if (!l_RemoveTarget)
                        l_RealTargets.push_back(l_Target);
                }
                p_Targets = l_RealTargets;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::DemonicCalling, EFFECT_1))
                    if (roll_chance_i(l_AuraEffect->GetAmount()))
                        l_Caster->CastSpell(l_Caster, eSpells::DemonicCallingTrigger, true);

                if (roll_chance_i(m_SpellInfo->Effects[EFFECT_2].BasePoints))
                    l_Caster->ModifyPower(POWER_SOUL_SHARDS, 10);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_demonwrath_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonwrath_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_demonwrath_SpellScript();
        }
};

/// Last Update 7.3.5
/// Called by Shadow Bolt - 686
class spell_warl_shadow_bolt : public SpellScriptLoader
{
    public:
        spell_warl_shadow_bolt() : SpellScriptLoader("spell_warl_shadow_bolt") { }

        class spell_warl_shadow_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_shadow_bolt_SpellScript);

            enum eSpells
            {
                ShadowyInspiration = 196606
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ModifyPower(POWER_SOUL_SHARDS, 10);
                l_Caster->RemoveAura(eSpells::ShadowyInspiration);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_shadow_bolt_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_shadow_bolt_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Doom - 603
class spell_warl_doom : public SpellScriptLoader
{
    public:
        spell_warl_doom() : SpellScriptLoader("spell_warl_doom") { }

        class spell_warl_doom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_doom_AuraScript);

            std::vector<uint32> m_SavedAmplitude;

            enum eSpells
            {
                KazaaksFinalCurse       = 214225,
                DoomDoubled             = 218572,
                DoomDoubledMultiplier   = 218571,
                ImpendinDoomAura        = 196270,
                ImpendinDoomSumm        = 196271,
                T19Demonology2PBonus    = 212005
            };

            uint32 CountNbInvocation(Unit* p_Caster)
            {
                uint32 l_Count = 0;
                for (Unit* l_Summon : p_Caster->m_Controlled)
                {
                    if (l_Summon->ToCreature() && l_Summon->ToCreature()->GetCreatureTemplate() && l_Summon->ToCreature()->GetCreatureTemplate()->type == CreatureType::CREATURE_TYPE_DEMON)
                        ++l_Count;
                }
                return l_Count;
            }

            void HandleModPower(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Aura* l_Aura = p_AuraEffect->GetBase();
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_Aura)
                    return;

                int32 l_Count = 10;

                /// Item - Warlock T19 Demonology 2P Bonus
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Demonology2PBonus, EFFECT_0))
                    if (roll_chance_i(l_AuraEffect->GetAmount()))
                        l_Count += 10;

                l_Caster->ModifyPower(POWER_SOUL_SHARDS, l_Count);

                if (l_Aura->GetDuration() > p_AuraEffect->GetAmplitude())
                    return;

                int32 l_OldAmplitude = p_AuraEffect->GetAmplitude();
                if (l_OldAmplitude <= 0)
                    return;

                AuraEffect* l_AuraEffect = const_cast<AuraEffect*>(p_AuraEffect);

                l_AuraEffect->SetAmplitude(l_Aura->GetDuration() - 100);
                l_AuraEffect->SetAmount((p_AuraEffect->GetAmount() * p_AuraEffect->GetAmplitude()) / l_OldAmplitude);
                l_AuraEffect->ResetPeriodic(true);
            }

            void HandleOnReapply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                if (!(p_Modes & AURA_EFFECT_HANDLE_REAPPLY))
                    return;

                Aura* l_Aura = p_AuraEffect->GetBase();
                Unit* l_Caster = GetCaster();
                if (!l_Aura || !l_Caster)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                int32 l_MaxDuration = l_Aura->CalcMaxDuration();
                int32 l_LeftDuration = l_Aura->GetDuration();

                /// Max duration of reapplied aura should be 130% of base max duration (Pandemic System)
                int32 l_MaxAllowedDuration = CalculatePct(l_MaxDuration, 130);

                if (l_LeftDuration + l_MaxDuration < l_MaxAllowedDuration)
                    SetMaxDuration(l_LeftDuration + l_MaxDuration);
                else
                    SetMaxDuration(l_MaxAllowedDuration);

                l_Aura->RefreshDuration();
                l_AuraEffect->CalculatePeriodic(l_Caster, false, false, false, true);
            }

            void HandleOnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                if (!(p_Modes & AURA_EFFECT_HANDLE_REAL))
                    return;

                Aura* l_Aura = p_AuraEffect->GetBase();
                if (!l_Aura)
                    return;

                l_Aura->SetDuration(p_AuraEffect->GetPeriodicTimer());
                l_Aura->SetMaxDuration(p_AuraEffect->GetPeriodicTimer());
            }

            void HandleCalcAmount(AuraEffect const* p_aurEff, int32& p_amount, bool& p_canBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                bool l_IsPVP = l_Caster->ToPlayer()->CanApplyPvPSpellModifiers();

                if (l_Caster->HasAura(eSpells::KazaaksFinalCurse))
                {
                    int32 l_NbInvoc = CountNbInvocation(l_Caster);
                    const SpellInfo *l_KazaaksInfo = sSpellMgr->GetSpellInfo(eSpells::KazaaksFinalCurse);

                    if (l_KazaaksInfo)
                        p_amount = p_amount * (1 + l_NbInvoc * (l_KazaaksInfo->Effects[0].BasePoints / 100.f));
                }

                if (l_IsPVP)
                    p_amount *= 0.85f; ///< Doom deals 85% damage in pvp

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DoomDoubled);
                SpellInfo const* l_DoomDoubled = sSpellMgr->GetSpellInfo(eSpells::DoomDoubledMultiplier);
                if (l_DoomDoubled && l_SpellInfo && l_Caster->HasAura(eSpells::DoomDoubled) && roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                {
                    uint32 l_BonusValue = l_DoomDoubled->Effects[EFFECT_0].BasePoints * (l_IsPVP ? 0.33f : 1.0f); ///< Doom, doubled deals 33% more damage instead of 100% in pvp
                    AddPct(p_amount, l_BonusValue);
                }
            }

            bool HandleDontRefresh(bool p_Decrease)
            {
                return false;
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::ImpendinDoomAura))
                {
                    l_Caster->CastSpell(l_Target, eSpells::ImpendinDoomSumm, true);
                    l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::ImpendinDoom][eSpells::ImpendinDoomSumm] = l_Target->GetGUID();
                }

				const_cast<AuraEffect*>(p_AurEff)->RecalculateAmount(false);
            }

            void Register() override
            {
                AfterEffectPeriodic += AuraEffectPeriodicFn(spell_warl_doom_AuraScript::HandleModPower, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_doom_AuraScript::HandleOnReapply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAPPLY);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_doom_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                CanRefreshProc += AuraCanRefreshProcFn(spell_warl_doom_AuraScript::HandleDontRefresh);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_doom_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_doom_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);

            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_doom_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Hand of Gul'Dan - 105174
class spell_warl_hand_of_guldan : public SpellScriptLoader
{
    public:
        spell_warl_hand_of_guldan() : SpellScriptLoader("spell_warl_hand_of_guldan") { }

        class spell_warl_hand_of_guldan_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_hand_of_guldan_SpellScript);

            enum eSpells
            {
                HandOfGuldanDamage = 86040,
                HandOfGuldanSummon = 104317,
                T21Demonology2PBonus = 251851,
                RageOfGuldan = 257926
            };

            int32 m_PowerSpent;
            int32 m_DamageMod;
            bool m_TargetHit = false;
            WorldLocation* m_TargetPosition = new WorldLocation();

            void HandleBeforeCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                m_TargetPosition = static_cast<WorldLocation*>(l_Target);
            }

            void HandlePowerCost(Powers p_Power, int32& p_PowerCost)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Power != POWER_SOUL_SHARDS)
                    return;

                if (int32 l_SoulShards = l_Caster->GetPower(POWER_SOUL_SHARDS))
                    m_PowerSpent = l_SoulShards < 40 ? l_SoulShards : 40;

                p_PowerCost = m_PowerSpent;
                m_DamageMod = m_PowerSpent/10;
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || m_TargetHit || m_TargetPosition->GetMapId() == MAPID_INVALID)
                    return;

                l_Caster->CastCustomSpell(m_TargetPosition->m_positionX, m_TargetPosition->m_positionY, m_TargetPosition->m_positionZ, eSpells::HandOfGuldanDamage, NULL, &m_DamageMod, NULL, true);

                m_TargetHit = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::T21Demonology2PBonus))
                {
                    for (int32 i = 0; i < m_PowerSpent / 10; i++)
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::RageOfGuldan, true);
                    }
                }

                if (!l_Target)
                    return;

                if (l_Caster->ToPlayer())
                    l_Caster->ToPlayer()->m_SpellHelper.GetUint64(eSpellHelpers::WildImpsTarget) = l_Target->GetGUID();

                for (int8 l_Itr = 0; l_Itr < m_PowerSpent / 10; ++l_Itr)
                    l_Caster->CastSpell(m_TargetPosition->m_positionX, m_TargetPosition->m_positionY, m_TargetPosition->m_positionZ, eSpells::HandOfGuldanSummon, true);

                if (l_Caster->m_Controlled.empty())
                    return;

                uint64 l_MinionGUID = l_Caster->GetLastMinionSpawn();
                Unit* l_Creature = sObjectAccessor->GetUnit(*l_Caster, l_MinionGUID);
                if (l_Creature && l_Caster && l_Caster->IsPlayer())
                    l_Caster->ToPlayer()->SendTempSummonUITimer(l_Creature);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_warl_hand_of_guldan_SpellScript::HandleBeforeCast);
                OnTakePowers += SpellTakePowersFn(spell_warl_hand_of_guldan_SpellScript::HandlePowerCost);
                AfterHit += SpellHitFn(spell_warl_hand_of_guldan_SpellScript::HandleAfterHit);
                AfterCast += SpellCastFn(spell_warl_hand_of_guldan_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_hand_of_guldan_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called By Hand of Gul'Dan (Damage) - 86040
class spell_warl_hand_of_guldan_damage : public SpellScriptLoader
{
    public:
        spell_warl_hand_of_guldan_damage() : SpellScriptLoader("spell_warl_hand_of_guldan_damage") { }

        class spell_warl_hand_of_guldan_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_hand_of_guldan_damage_SpellScript);

            enum eSpells
            {
                HandOfDoom  = 196283,
                Doom        = 603
            };

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::HandOfDoom))
                    l_Caster->CastSpell(l_Target, eSpells::Doom, true);

                Spell const* l_Spell = GetSpell();
                if (!l_Spell)
                    return;

                SetHitDamage(GetHitDamage() * l_Spell->GetSpellValue(SPELLVALUE_BASE_POINT1));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_hand_of_guldan_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_hand_of_guldan_damage_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Eye Laser - 205231
class spell_warl_darkglare_eye_laser : public SpellScriptLoader
{
    public:
        spell_warl_darkglare_eye_laser() : SpellScriptLoader("spell_warl_darkglare_eye_laser") { }

        class spell_warl_darkglare_eye_laser_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_darkglare_eye_laser_SpellScript);

            enum eSpells
            {
                Doom                = 603,
                EyeLaser            = 205231,
                DemonicEmpowerment  = 193396,
                BreathOfThalkiel    = 221882,
                GrimoireOfSynergy   = 171982,
                EmpoweredLifeTap    = 235156,
                TheExpendables      = 211218
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetOriginalCaster();
                Unit* l_Pet = GetCaster();
                Unit* l_HitUnit = GetHitUnit();
                SpellInfo const* l_EyeLaser = GetSpellInfo();
                if (!l_HitUnit || !l_EyeLaser || !l_Caster || !l_Pet)
                    return;

                Player* l_Player = (l_Caster->GetOwner()) ? l_Caster->GetOwner()->ToPlayer() : nullptr;
                if (!l_Player)
                    return;

                int32 l_SpellPower = l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                int32 l_Damage = l_SpellPower * l_EyeLaser->Effects[0].BonusMultiplier;

                l_Damage = l_Player->SpellDamageBonusDone(l_HitUnit, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);

                /// Grimoire of Synergy mustn't be applied
                if (l_Player->HasAura(eSpells::GrimoireOfSynergy))
                    l_Damage /= 1.25;

                /// Empowered life tap musn't be applied
                if (l_Caster->HasAura(eSpells::EmpoweredLifeTap))
                    l_Damage /= 1.10;

                ///< Warlock's pets the expendables - 211218
                if (AuraEffect* l_TheExpendablesEffect = l_Pet->GetAuraEffect(eSpells::TheExpendables, EFFECT_0))
                    AddPct(l_Damage, l_TheExpendablesEffect->GetAmount());
                else if (AuraEffect* l_TheExpendablesEffect = l_Caster->GetAuraEffect(eSpells::TheExpendables, EFFECT_0))
                    AddPct(l_Damage, l_TheExpendablesEffect->GetAmount());

                l_Damage = l_HitUnit->SpellDamageBonusTaken(l_Player, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

                if (AuraEffect* l_DemonicEffect = l_Caster->GetAuraEffect(eSpells::DemonicEmpowerment, EFFECT_3))
                    l_Damage = AddPct(l_Damage, l_DemonicEffect->GetAmount());

                SetHitDamage(l_Damage);
                SpellRangeEntry const* l_Range = l_EyeLaser->RangeEntry;
                if (!l_Range)
                    return;

                std::set<uint64>& l_HitTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::EyeLaserHitTargets];

                l_HitTargets.insert(l_HitUnit->GetGUID());

                std::list<Unit*> l_TargetList;
                JadeCore::AnyUnitHavingBuffInObjectRangeCheck l_Ucheck(l_Caster->GetOwner(), l_HitUnit, l_Range->maxRangeHostile, eSpells::Doom, true);
                JadeCore::UnitListSearcher<JadeCore::AnyUnitHavingBuffInObjectRangeCheck> l_Searcher(l_HitUnit, l_TargetList, l_Ucheck);
                l_HitUnit->VisitNearbyObject(l_Range->maxRangeHostile, l_Searcher);

                if (l_TargetList.empty())
                    return;

                Unit* l_TargetFound = nullptr;
                for (Unit* l_Target : l_TargetList)
                {
                    if (l_HitTargets.find(l_Target->GetGUID()) == l_HitTargets.end())
                    {
                        l_TargetFound = l_Target;
                        break;
                    }
                }

                if (!l_TargetFound)
                {
                    l_HitTargets.clear();
                    return;
                }

                l_HitUnit->CastSpell(l_TargetFound, eSpells::EyeLaser, true, nullptr, nullptr, l_Pet->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_darkglare_eye_laser_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_darkglare_eye_laser_SpellScript();
        }
};

/// Last Update 7.3.5
/// Called by Dreadbite - 205196
class spell_warl_dreadbite : public SpellScriptLoader
{
public:
    spell_warl_dreadbite() : SpellScriptLoader("spell_warl_dreadbite") { }

    class spell_warl_dreadbite_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_dreadbite_SpellScript);

        enum eSpells
        {
            BreathOfThalkiel    = 221882,
            T21Demonology4P     = 251852
        };

        void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetOriginalCaster();
            if (!l_Caster)
                return;

            Player* l_Player = (l_Caster->GetOwner()) ? l_Caster->GetOwner()->ToPlayer() : nullptr;
            if (!l_Player)
                return;

            int32 l_Damage = GetHitDamage();

            if (GetSpell() && GetSpell()->IsTriggered()) /// Only happening when the T21 4P gets proc'ed.
                if (AuraEffect* l_T21Bonus = l_Player->GetAuraEffect(eSpells::T21Demonology4P, EFFECT_0))
                    AddPct(l_Damage, l_T21Bonus->GetAmount());

            if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::BreathOfThalkiel, EFFECT_0))
                AddPct(l_Damage, l_AuraEffect->GetAmount());

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_warl_dreadbite_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warl_dreadbite_SpellScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Corruption - 146739
class spell_warl_corruption_damage : public SpellScriptLoader
{
    public:
        spell_warl_corruption_damage() : SpellScriptLoader("spell_warl_corruption_damage") { }

        class spell_warl_corruption_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_corruption_damage_AuraScript);

            bool m_AlreadyReducedPVP = true;

            void HandleAfterApply(AuraEffect const* /*p_AuraEFfect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                m_AlreadyReducedPVP = false;
            }

            void HandleCalcAmount(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                if (l_Caster->ToPlayer()->CanApplyPvPSpellModifiers() && !m_AlreadyReducedPVP)
                {
                    m_AlreadyReducedPVP = true;
                    AuraEffect* l_Effect = const_cast<AuraEffect*>(p_AurEff);
                    int32 l_Amount = p_AurEff->GetAmount();

                    l_Effect->ChangeAmount(l_Amount * 0.85f); ///< Corruption deals 85% damage in pvp
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_corruption_damage_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_corruption_damage_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_corruption_damage_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Grimoire of Service - 108501
class spell_warl_grimoire_of_service_learn : public SpellScriptLoader
{
    public:
        spell_warl_grimoire_of_service_learn() : SpellScriptLoader("spell_warl_grimoire_of_service_learn") { }

        class spell_warl_grimoire_of_service_learn_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_grimoire_of_service_learn_AuraScript);

            enum eSpells
            {
                GrimoireImp         = 111859,
                GrimoireVoidwalker  = 111895,
                GrimoireSuccubus     = 111896,
                GrimoireFelhunter   = 111897,
                GrimoireFelguard    = 111898,
            };

            void HandleLearn(AuraEffect const* /*p_AuraEFfect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->learnSpell(eSpells::GrimoireImp, false);
                l_Player->learnSpell(eSpells::GrimoireVoidwalker, false);
                l_Player->learnSpell(eSpells::GrimoireSuccubus, false);
                l_Player->learnSpell(eSpells::GrimoireFelhunter, false);
                if (l_Player->GetActiveSpecializationID() == SPEC_WARLOCK_DEMONOLOGY)
                    l_Player->learnSpell(eSpells::GrimoireFelguard, false);
            }

            void HandleUnlearn(AuraEffect const* /*p_AuraEFfect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->removeSpell(eSpells::GrimoireImp);
                l_Player->removeSpell(eSpells::GrimoireVoidwalker);
                l_Player->removeSpell(eSpells::GrimoireSuccubus);
                l_Player->removeSpell(eSpells::GrimoireFelhunter);
                l_Player->removeSpell(eSpells::GrimoireFelguard);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_grimoire_of_service_learn_AuraScript::HandleLearn, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_grimoire_of_service_learn_AuraScript::HandleUnlearn, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_grimoire_of_service_learn_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Grimoire: Imp - 111859, Grimoire: Voidwalker - 111895, Grimoire: Succubus - 111896
/// Grimoire: Felhunter - 111897 and Grimoire: Felguard - 111898
class spell_warl_grimoire_of_service : public SpellScriptLoader
{
    public:
        spell_warl_grimoire_of_service() : SpellScriptLoader("spell_warl_grimoire_of_service") { }

        class spell_warl_grimoire_of_service_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_grimoire_of_service_SpellScript);

            enum eSpells
            {
                SingeMagic          = 89808,
                Suffering           = 17735,
                Seduce              = 6358,
                SpellLock           = 19647,
                AxeToss             = 89766,
                DamageIncrease      = 216187,
                GrimoireImp         = 111859,
                GrimoireVoidwalker  = 111895,
                GrimoireSuccubus    = 111896,
                GrimoireFelhunter   = 111897,
                GrimoireFelguard    = 111898
            };

            uint32 GetSpellForEntry(uint32 p_Entry)
            {
                switch (p_Entry)
                {
                    case WarlockPet::Imp:
                        return eSpells::SingeMagic;
                    case WarlockPet::VoidWalker:
                        return eSpells::Suffering;
                    case WarlockPet::Succubus:
                        return eSpells::Seduce;
                    case WarlockPet::FelHunter:
                        return eSpells::SpellLock;
                    case WarlockPet::FelGuard:
                        return eSpells::AxeToss;
                    default:
                        return 0;
                }
                return 0;
            }

            uint32 GetEntryForSummoningSpell(uint32 p_Spell)
            {
                switch (p_Spell)
                {
                    case eSpells::GrimoireImp:
                        return WarlockPet::Imp;
                    case eSpells::GrimoireVoidwalker:
                        return WarlockPet::VoidWalker;
                    case eSpells::GrimoireSuccubus:
                        return WarlockPet::Succubus;
                    case eSpells::GrimoireFelhunter:
                        return WarlockPet::FelHunter;
                    case eSpells::GrimoireFelguard:
                        return WarlockPet::FelGuard;
                    default:
                        return 0;
                }
                return 0;
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                std::set<Unit*> l_Targets;

                for (Unit::ControlList::const_iterator l_Itr = l_Caster->m_Controlled.begin(); l_Itr != l_Caster->m_Controlled.end(); ++l_Itr)
                {
                    if ((*l_Itr)->isPet())
                        continue;

                    if ((*l_Itr)->GetEntry() != GetEntryForSummoningSpell(m_scriptSpellId))
                        continue;

                    /// Don't cast spell right away, the spell can alterate m_Controlled contrainer and we would get a corrupted iterator
                    l_Targets.insert(*l_Itr);
                }

                for (auto l_Itr : l_Targets)
                {
                    (l_Itr)->CastSpell((l_Itr), eSpells::DamageIncrease, true);

                    if ((l_Itr)->GetEntry() == WarlockPet::Imp)
                        (l_Itr)->CastSpell(l_Caster, GetSpellForEntry((l_Itr)->GetEntry()), true);

                    if (GetSpellForEntry((l_Itr)->GetEntry()))
                        (l_Itr)->CastSpell(l_Target, GetSpellForEntry((l_Itr)->GetEntry()), true);

                    UnitAI* l_AI = (l_Itr)->GetAI();
                    if (l_AI)
                        l_AI->SetGUID(l_Target->GetGUID(), 0);
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_warl_grimoire_of_service_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_grimoire_of_service_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Demonbolt - 157695
class spell_warl_demonbolt : public SpellScriptLoader
{
    public:
        spell_warl_demonbolt() : SpellScriptLoader("spell_warl_demonbolt") { }

        class spell_warl_demonbolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_demonbolt_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_Demonbolt = GetSpellInfo();
                if (!l_Caster || !l_Demonbolt)
                    return;

                uint8 l_ControlledDemons = 0;

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon->ToCreature() && l_Summon->ToCreature()->GetCreatureTemplate() && l_Summon->ToCreature()->GetCreatureTemplate()->type == CreatureType::CREATURE_TYPE_DEMON)
                        ++l_ControlledDemons;
                }

                SetHitDamage((int32)((float)GetHitDamage() * (1.0f + ((float)l_Demonbolt->Effects[EFFECT_2].BasePoints / 100.0f) * l_ControlledDemons)));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_demonbolt_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_demonbolt_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Demonic Empowerment - 193396
class spell_warl_demonic_empowerment : public SpellScriptLoader
{
    public:
        spell_warl_demonic_empowerment() : SpellScriptLoader("spell_warl_demonic_empowerment") { }

        class spell_warl_demonic_empowerment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_demonic_empowerment_SpellScript);

            enum eSpells
            {
                ShadowyInspirationPassive = 196269,
                ShadowyInspirationTrigger = 196606,
                PowerTrip                 = 196605,
                ThalkielsAscendance       = 238145,
                ThalkielsAscendanceDamage = 242832,
                T21Demonology4PBonus      = 251852
            };

            enum eNPCs
            {
                DemonicGateWayGreen     = 59262,
                DemonicGateWayPurple    = 59271,
                FelLord                 = 107024,
                Observer                = 107100,
                Dreadstalkers           = 98035
            };

            std::vector<eNPCs> m_ExcludedNPCs =
            {
                eNPCs::DemonicGateWayGreen,
                eNPCs::DemonicGateWayPurple,
                eNPCs::FelLord,
                eNPCs::Observer
            };

            bool l_EffectProcced = false;

            void FilterTargets(std::list<WorldObject*>& p_TargetList)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_TargetList.remove_if([this, l_Caster](WorldObject* l_Target) -> bool
                {
                    Unit* l_TargetUnit = l_Target->ToUnit();
                    if (!l_TargetUnit || l_TargetUnit == l_Caster)
                        return true;

                    if (std::find(m_ExcludedNPCs.begin(), m_ExcludedNPCs.end(), l_TargetUnit->GetEntry()) != m_ExcludedNPCs.end())
                        return true;

                    return false;
                });

                if (!l_EffectProcced)
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ThalkielsAscendance))
                    {
                        if (roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                        {
                            for (auto l_Target : p_TargetList)
                            {
                                if (Unit* l_TargetUnit = l_Target->ToUnit())
                                {
                                    if (l_Caster->ToPlayer() && l_Caster->HasAura(eSpells::ThalkielsAscendance))
                                    {
                                        if (l_TargetUnit->isInCombat() && l_TargetUnit->getVictim() && l_TargetUnit->getVictim()->isInCombat() && l_TargetUnit->getVictim()->GetDistance(l_TargetUnit) <= 40.0f)
                                            l_TargetUnit->CastSpell(l_TargetUnit->getVictim(), eSpells::ThalkielsAscendanceDamage, true);
                                        else if (l_Caster->isInCombat() && l_Caster->getVictim() && l_Caster->getVictim()->isInCombat() && l_Caster->getVictim()->GetDistance(l_TargetUnit) <= 40.0f)
                                            l_TargetUnit->CastSpell(l_Caster->getVictim(), eSpells::ThalkielsAscendanceDamage, true);
                                        else if (l_Caster->ToPlayer()->GetSelectedUnit() && l_Caster->ToPlayer()->GetSelectedUnit()->IsValidAttackTarget(l_Caster) && l_Caster->ToPlayer()->GetSelectedUnit()->isInCombat() && l_Caster->ToPlayer()->GetSelectedUnit()->GetDistance(l_TargetUnit) <= 40.0f)
                                            l_TargetUnit->CastSpell(l_Caster->ToPlayer()->GetSelectedUnit(), eSpells::ThalkielsAscendanceDamage, true);
                                    }
                                }
                            }
                        }
                    }
                }

                l_EffectProcced = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::T21Demonology4PBonus))
                {
                    for (Unit* l_Summon : l_Caster->m_Controlled)
                    {
                        if (l_Summon->GetEntry() == eNPCs::Dreadstalkers)
                        {
                            l_Summon->GetAI()->SetData(0, 1);
                        }
                    }
                }

                if (l_Caster->HasAura(eSpells::ShadowyInspirationPassive))
                    l_Caster->CastSpell(l_Caster, eSpells::ShadowyInspirationTrigger, true);

                if (l_Caster->isInCombat() && l_Caster->HasAura(eSpells::PowerTrip))
                {
                    if (AuraEffect* aurPowerTrip = l_Caster->GetAuraEffect(eSpells::PowerTrip, EFFECT_0))
                        if (roll_chance_i(aurPowerTrip->GetAmount()))
                            l_Caster->ModifyPower(POWER_SOUL_SHARDS, 10);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_demonic_empowerment_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonic_empowerment_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_SUMMON);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonic_empowerment_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_SUMMON);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonic_empowerment_SpellScript::FilterTargets, EFFECT_3, TARGET_UNIT_CASTER_AREA_SUMMON);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonic_empowerment_SpellScript::FilterTargets, EFFECT_4, TARGET_UNIT_CASTER_AREA_SUMMON);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonic_empowerment_SpellScript::FilterTargets, EFFECT_5, TARGET_UNIT_CASTER_AREA_SUMMON);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonic_empowerment_SpellScript::FilterTargets, EFFECT_6, TARGET_UNIT_CASTER_AREA_SUMMON);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_demonic_empowerment_SpellScript();
        }

        class spell_warl_demonic_empowerment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_empowerment_AuraScript);

            void HandleEffectApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                uint32 l_SpellID = GetSpellInfo()->Id;
                uint64 l_TargetGuid = l_Target->GetGUID();
                l_Caster->AddDelayedEvent([=]() -> void
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGuid);
                    if (!l_Target)
                        return;

                    /// Aura duration on reapply is not 12s but 15s.
                    if (Aura* l_Aura = l_Target->GetAura(l_SpellID))
                    {
                        l_Aura->SetMaxDuration(15 * IN_MILLISECONDS);
                        l_Aura->SetDuration(15 * IN_MILLISECONDS, true);
                    }
                }, 100);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_demonic_empowerment_AuraScript::HandleEffectApply, EFFECT_2, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAPPLY);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_demonic_empowerment_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Thal'kiel's Consumption - 211714
class spell_warl_demonic_thal_kiel_consumption : public SpellScriptLoader
{
    public:
        spell_warl_demonic_thal_kiel_consumption() : SpellScriptLoader("spell_warl_demonic_thal_kiel_consumption") { }

        class spell_warl_demonic_thal_kiel_consumption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_demonic_thal_kiel_consumption_SpellScript);

            enum eSpells
            {
                ThalkielsConsumptionDamage  = 211715,
                WakenersLoyalty             = 236200,
                BreathOfThalkiel            = 221882
            };

            enum ePets
            {
                FelLord     = 107024,
                Observer    = 107100
            };

            std::vector<ePets> m_ExcludedPets =
            {
                ePets::FelLord,
                ePets::Observer
            };

            int32 m_Damages = 0;

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                if (!l_Caster->IsWithinLOSInMap(l_Target))
                    return SPELL_FAILED_LINE_OF_SIGHT;

                return SPELL_CAST_OK;
            }

            void FilterTargets(std::list<WorldObject*>& p_TargetList)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                p_TargetList.remove_if([this, l_Caster, l_SpellInfo](WorldObject* l_Target) -> bool
                {
                    Unit* l_TargetUnit = l_Target->ToUnit();
                    if (!l_TargetUnit || l_TargetUnit == l_Caster)
                        return true;

                    if (std::find(m_ExcludedPets.begin(), m_ExcludedPets.end(), l_TargetUnit->GetEntry()) != m_ExcludedPets.end())
                        return true;

                    m_Damages += l_TargetUnit->CountPctFromMaxHealth(l_SpellInfo->Effects[EFFECT_1].BasePoints);

                    return false;
                });
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WakenersLoyalty);
                SpellInfo const* l_DamageSpellInfo = sSpellMgr->GetSpellInfo(eSpells::ThalkielsConsumptionDamage);
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_DamageSpellInfo)
                    return;

                AddPct(m_Damages, l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Caster->GetAuraCount(eSpells::WakenersLoyalty));

                if (l_Caster->CanApplyPvPSpellModifiers())
                {
                    m_Damages *= 0.66f; /// Thal'kiel's Consumption deals 66% damage in PvP

                    if (l_Caster->HasAura(eSpells::BreathOfThalkiel))
                        if (SpellInfo const* l_BreathOfThalkielInfo = sSpellMgr->GetSpellInfo(eSpells::BreathOfThalkiel))
                            AddPct(m_Damages, l_BreathOfThalkielInfo->Effects[EFFECT_0].BasePoints);
                }
                else
                    m_Damages *= l_Caster->SpellDamagePctDone(l_Target, l_DamageSpellInfo, DamageEffectType::SPELL_DIRECT_DAMAGE, EFFECT_0);

                l_Caster->CastCustomSpell(l_Target, eSpells::ThalkielsConsumptionDamage, &m_Damages, NULL, NULL, true);
                l_Caster->RemoveAura(eSpells::WakenersLoyalty);
            }

            void HandleDamagePet(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                int32 l_Damage = l_Target->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints);

                if (l_Caster->CanApplyPvPSpellModifiers())
                    l_Damage = l_Damage * 0.66f; /// Thal'kiel's Consumption deals 66% damage in PvP

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_demonic_thal_kiel_consumption_SpellScript::CheckCast);
                AfterCast += SpellCastFn(spell_warl_demonic_thal_kiel_consumption_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonic_thal_kiel_consumption_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_SUMMON);
                OnEffectHitTarget += SpellEffectFn(spell_warl_demonic_thal_kiel_consumption_SpellScript::HandleDamagePet, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_demonic_thal_kiel_consumption_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Summon Felguard - 30146 - Summon Wrathguard - 112870
class spell_warl_summon_felguard : public SpellScriptLoader
{
    public:
        spell_warl_summon_felguard() : SpellScriptLoader("spell_warl_summon_felguard") { }

        class spell_warl_summon_felguard_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_summon_felguard_SpellScript);

            SpellCastResult CheckSpec()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Player->GetActiveSpecializationID() != SPEC_WARLOCK_DEMONOLOGY)
                    return SPELL_FAILED_NO_SPEC;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_summon_felguard_SpellScript::CheckSpec);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_summon_felguard_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Eye Of The Observer - 212580
class spell_warl_eye_of_the_observer : public SpellScriptLoader
{
    public:
        spell_warl_eye_of_the_observer() : SpellScriptLoader("spell_warl_eye_of_the_observer") { }

        class spell_warl_eye_of_the_observer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_eye_of_the_observer_AuraScript);

            enum eSpells
            {
                CallObserver    = 201996,
                Laserbeam       = 212529
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Observer = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CallObserver);
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Observer || !l_DamageInfo)
                    return false;

                Spell const* l_Spell = l_DamageInfo->GetSpell();

                Unit* l_Target = l_DamageInfo->GetActor();
                Unit* l_Victim = l_DamageInfo->GetTarget();

                /// Only procs on warlock owner, or other allied player, only player anyway
                if (!l_Target || !l_Victim || !l_Victim->IsPlayer())
                    return false;

                SpellSchoolMask l_SchoolMask = l_DamageInfo->GetSchoolMask();
                if (l_SchoolMask == SPELL_SCHOOL_MASK_NORMAL || !l_Spell || l_Spell->getTriggerCastFlags() == TriggerCastFlags::TRIGGERED_FULL_MASK)
                    return false;

                int32 l_Damage = CalculatePct(l_Target->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_2].BasePoints);

                l_Observer->CastCustomSpell(l_Target, eSpells::Laserbeam, &l_Damage, nullptr, nullptr, false, nullptr, nullptr, 0, 1);

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_eye_of_the_observer_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_eye_of_the_observer_AuraScript();
        }
};

/// Last update : 7.0.3 22522
/// Called by Reap Souls (Artifact) - 216698
class spell_warl_reap_souls : public SpellScriptLoader
{
    public:
        spell_warl_reap_souls() : SpellScriptLoader("spell_warl_reap_souls") { }

        class spell_warl_reap_souls_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_reap_souls_SpellScript);

            enum eSpells
            {
                TormentedSouls          = 216695,
                DeadwindHarvester       = 216708,
                SoulOfTheDamned         = 216463,
                SoulOfTheDamnedKill     = 216721,
                TormentedSoulConsume    = 216714,
                ReapAndSow              = 236114
            };

            /*
              * wowhead spell description:
              *
              * It took me a bit to understand how this power works. The power's icon will show the stack count (1 to 12, which is the maximum number of stacks).
              * For a while, I thought the buff was actually ON me because it showed on my buffs ( Tormented Souls); however, this is not the case. (also note: the  Tormented Souls icon only will show, numerically, stacks 2-12, not 1).
              * Note also, that when you hover over the  Tormented Souls icon in your buffs, it will say it enhances the damage 10% per stack, so a stack of 10 will show 100% damage buff, and "doubling" the effects � different than what Deadwind Harvester says.
              * When you activate the power, then you get the actual buff ( Deadwind Harvester) with a count-down timer (5 seconds times the number of stacks the power icon had).
              * If you try to activate the buff AGAIN, while it's still on, it will not over-write the existing count-down buff, unless it is greater than it. For example, if you had 5 seconds left, but had a stack of 3 (15 sec), then you'd get the 15 seconds; it does not add, but replaces only when greater.
            */
            void HandleDummy(SpellEffIndex /*p_Index*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = GetCaster()->GetAura(eSpells::TormentedSouls);
                if (!l_Aura)
                    return;

                uint8 l_Souls = l_Aura->GetStackAmount();
                l_Caster->RemoveAurasDueToSpell(eSpells::TormentedSouls);

                SpellInfo const* l_DeadwindHadvester = sSpellMgr->GetSpellInfo(eSpells::DeadwindHarvester);
                if (!l_DeadwindHadvester)
                    return;

                int32 l_AddDuration = 0;
                if (!l_Caster->HasAura(eSpells::DeadwindHarvester))
                    l_Caster->CastSpell(l_Caster, eSpells::DeadwindHarvester, true);
                else if (Aura* l_DeadWindHarvester = l_Caster->GetAura(eSpells::DeadwindHarvester))
                    l_AddDuration = l_DeadWindHarvester->GetDuration();

                std::list<AreaTrigger*> l_AreaTriggerList;
                l_Caster->GetAreaTriggerList(l_AreaTriggerList, eSpells::SoulOfTheDamned);
                l_Caster->GetAreaTriggerList(l_AreaTriggerList, eSpells::SoulOfTheDamnedKill);
                for (auto l_AreaTrigger : l_AreaTriggerList)
                {
                    l_Caster->CastSpell(l_AreaTrigger->GetPositionX(), l_AreaTrigger->GetPositionY(), l_AreaTrigger->GetPositionZ(), eSpells::TormentedSoulConsume, true);
                    l_AreaTrigger->Remove();
                }

                int32 l_Duration = l_DeadwindHadvester->GetDuration() * l_Souls;

                l_Duration += l_AddDuration;

                if (l_Duration > (60 * IN_MILLISECONDS))
                    l_Duration = 60 * IN_MILLISECONDS;

                Aura* l_DeadWindHarvester = l_Caster->GetAura(eSpells::DeadwindHarvester);
                if (!l_DeadWindHarvester)
                    return;

                if (l_DeadWindHarvester->GetDuration() > l_Duration)
                    return;

                l_DeadWindHarvester->SetDuration(l_Duration, true);

                if (Aura* l_ReapAndSowAura = l_Caster->GetAura(eSpells::ReapAndSow))
                {
                    AuraEffect* l_AuraEffect = l_ReapAndSowAura->GetEffect(EFFECT_0);
                    if (!l_AuraEffect)
                        return;

                    l_Duration += l_AuraEffect->GetAmount() * l_Souls;
                    l_DeadWindHarvester->SetDuration(l_Duration, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_reap_souls_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_reap_souls_SpellScript();
        }
};

/// Implosion - 196277
class spell_warl_implosion : public SpellScriptLoader
{
    public:
        spell_warl_implosion() : SpellScriptLoader("spell_warl_implosion") { }

        class spell_warl_implosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_implosion_SpellScript);

            enum eNPCs
            {
                WildImp = 55659
            };

            uint64 m_TargetGUID = 0;

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon->GetEntry() == eNPCs::WildImp)
                        return SPELL_CAST_OK;
                }

                return SPELL_FAILED_BAD_TARGETS;
            }

            void HandleDummyEnemy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                m_TargetGUID = l_Target->GetGUID();
            }

            void HandleDummyWilImps(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr || l_Target->GetEntry() != eNPCs::WildImp)
                    return;

                Creature* l_WildImp = l_Target->ToCreature();
                Unit* l_MainTarget = Unit::GetUnit(*l_WildImp, m_TargetGUID);

                if (l_MainTarget == nullptr || l_WildImp == nullptr)
                    return;

                l_WildImp->GetAI()->AttackStart(l_MainTarget);
                l_WildImp->GetAI()->SetData(0, 1);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_implosion_SpellScript::HandleDummyEnemy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHitTarget += SpellEffectFn(spell_warl_implosion_SpellScript::HandleDummyWilImps, EFFECT_1, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_warl_implosion_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_implosion_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Harvester of Souls - 201424
class spell_warl_harvester_of_souls : public SpellScriptLoader
{
    public:
        spell_warl_harvester_of_souls() : SpellScriptLoader("spell_warl_harvester_of_souls") { }

        class spell_warl_harvester_of_souls_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_harvester_of_souls_AuraScript);

            enum eSpells
            {
                Corruption = 146739
            };

            void HandleOnProc(AuraEffect const* p_AuraEffect, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return;

                Unit* l_Victim = l_DamageInfo->GetTarget();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (l_SpellInfo == nullptr || l_Victim == nullptr)
                    return;

                if (l_SpellInfo->Id != eSpells::Corruption)
                    return;

                l_Caster->CastSpell(l_Victim, p_AuraEffect->GetTriggerSpell(), true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_warl_harvester_of_souls_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_harvester_of_souls_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Conflegrate 17962
/// Entrenched in Flame - 233581
class spell_warl_entrenched_in_flame : public SpellScriptLoader
{
    public:
        spell_warl_entrenched_in_flame() : SpellScriptLoader("spell_warl_entrenched_in_flame") { }

        class spell_warl_entrenched_in_flame_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_entrenched_in_flame_AuraScript);

            enum eSpells
            {
                Conflagrate = 17962,
                Shadowburn  = 17877
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();

                if (l_SpellInfo == nullptr)
                    return false;

                return (l_SpellInfo->Id == eSpells::Shadowburn ||
                        l_SpellInfo->Id == eSpells::Conflagrate);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_entrenched_in_flame_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_entrenched_in_flame_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Cremation - 212282
class spell_warl_cremation : public SpellScriptLoader
{
    public:
        spell_warl_cremation() : SpellScriptLoader("spell_warl_cremation") { }

        class spell_warl_cremation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_cremation_AuraScript);

            enum eSpells
            {
                Conflagrate     = 17962,
                Shadowburn      = 17877,
                ImmolateDebuff  = 157736
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::Conflagrate && l_SpellInfo->Id != eSpells::Shadowburn))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_cremation_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_cremation_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Essence Drain - 221711
class spell_warl_essence_drain : public SpellScriptLoader
{
    public:
        spell_warl_essence_drain() : SpellScriptLoader("spell_warl_essence_drain") { }

        class spell_warl_essence_drain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_essence_drain_AuraScript);

            enum eSpells
            {
                DrainLife           = 234153,
                DrainSoul           = 198590,
                EssenceDrainDebuff  = 221715
            };

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_SpellInfo || !l_Target)
                    return false;

                if (l_SpellInfo->Id != eSpells::DrainLife && l_SpellInfo->Id != eSpells::DrainSoul)
                    return false;

                l_Caster->DelayedCastSpell(l_Target, eSpells::EssenceDrainDebuff, true, 1);

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_essence_drain_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_essence_drain_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Drain Soul - 198590
class spell_warl_drain_soul : public SpellScriptLoader
{
    public:
        spell_warl_drain_soul() : SpellScriptLoader("spell_warl_drain_soul") { }

        class spell_warl_drain_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_drain_soul_AuraScript);

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                int32 l_SoulShardAmount = l_Caster->GetPower(POWER_SOUL_SHARDS);
                l_SoulShardAmount += 10;

                if (l_Target->getDeathState() == DeathState::JUST_DIED)
                    l_Caster->SetPower(POWER_SOUL_SHARDS, l_SoulShardAmount, true);
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = p_AuraEffect->GetBase();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Aura || !l_Target)
                    return;

                uint32& l_Helper = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::DrainSoul);
                uint64& l_HelperGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::DrainSoul);
                if (!l_Helper || l_HelperGUID != l_Target->GetGUID())
                    return;

                SetMaxDuration(l_Helper);
                l_Helper = 0;

                RefreshDuration();
                for (uint8 i = 0; i < l_Aura->GetEffectCount(); ++i)
                    if (HasEffect(i))
                        GetEffect(i)->CalculatePeriodic(l_Caster, true, false, false, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_drain_soul_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_LEECH, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_drain_soul_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_LEECH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_drain_soul_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Shadowburn - 17877
class spell_warl_shadowburn : public SpellScriptLoader
{
    public:
        spell_warl_shadowburn() : SpellScriptLoader("spell_warl_shadowburn") { }

        class spell_warl_shadowburn_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_shadowburn_AuraScript);

            enum eSpells
            {
                ShadowburnSoulShardsAmount = 29341
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfos = sSpellMgr->GetSpellInfo(eSpells::ShadowburnSoulShardsAmount);
                if (!l_Caster || !l_Target || !l_SpellInfos)
                    return;

                int32 l_SoulShardGain = l_SpellInfos->Effects[EFFECT_0].BasePoints;

                if (l_Target->getDeathState() == DeathState::JUST_DIED)
                    l_Caster->ModifyPower(POWER_SOUL_SHARDS, l_SoulShardGain);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_shadowburn_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_warl_shadowburn_SpellScript : public SpellScript
            {
            PrepareSpellScript(spell_warl_shadowburn_SpellScript);

            enum eSpells
            {
                ShadowburnSoulShardsAmount = 29341
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfos = sSpellMgr->GetSpellInfo(eSpells::ShadowburnSoulShardsAmount);
                if (!l_Caster || !l_SpellInfos)
                    return;

                l_Caster->ModifyPower(POWER_SOUL_SHARDS, l_SpellInfos->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_shadowburn_SpellScript::HandleAfterCast);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_shadowburn_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_shadowburn_SpellScript();
        }
};

/// Called by Fel Fissure - 200586
class spell_warl_fel_fissure : public SpellScriptLoader
{
    public:
        spell_warl_fel_fissure() : SpellScriptLoader("spell_warl_fel_fissure") { }

        class spell_warl_fel_fissure_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_fel_fissure_AuraScript);

            enum eSpells
            {
                ChaosBolt = 116858
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ChaosBolt)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_fel_fissure_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_fel_fissure_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Pleasure Through Pain - 212618
class PlayerScript_Pleasure_Through_Pain : public PlayerScript
{
    public:
        PlayerScript_Pleasure_Through_Pain() :PlayerScript("PlayerScript_Pleasure_Through_Pain") { }

        enum eSpells
        {
            PleasureThroughPain         = 212618,
            PleasureThroughPainEffect   = 212999
        };

        void OnSummonPet(Player* p_Player)
        {
            if (!p_Player || p_Player->getClass() != CLASS_WARLOCK)
                return;

            if (!p_Player->HasAura(eSpells::PleasureThroughPain))
                return;

            Pet* l_Pet = p_Player->GetPet();
            if (!l_Pet || (l_Pet->GetEntry() != WarlockPet::Succubus && l_Pet->GetEntry() != WarlockPet::Shivarra))
                return;

            p_Player->CastSpell(p_Player, eSpells::PleasureThroughPainEffect, true);
        }

        void OnMinionDespawn(Player* p_Player, Minion* p_Minion)
        {
            if (!p_Player || p_Player->getClass() != CLASS_WARLOCK)
                return;

            if (!p_Minion || (p_Minion->GetEntry() != WarlockPet::Succubus && p_Minion->GetEntry() != WarlockPet::Shivarra))
                return;

            p_Player->RemoveAurasDueToSpell(eSpells::PleasureThroughPainEffect);
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Pleasure Through Pain - 212618
class spell_warl_pleasure_through_pain_removal : public SpellScriptLoader
{
    public:
        spell_warl_pleasure_through_pain_removal() : SpellScriptLoader("spell_warl_pleasure_through_pain_removal") { }

        class spell_warl_pleasure_through_pain_removal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_pleasure_through_pain_removal_AuraScript);

            enum eSpells
            {
                PleasureThroughPain         = 212618,
                PleasureThroughPainEffect   = 212999
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::PleasureThroughPainEffect);
            }

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::PleasureThroughPain))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Pet* l_Pet = l_Player->GetPet();
                if (!l_Pet || (l_Pet->GetEntry() != WarlockPet::Succubus && l_Pet->GetEntry() != WarlockPet::Shivarra))
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::PleasureThroughPainEffect);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_pleasure_through_pain_removal_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_pleasure_through_pain_removal_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_pleasure_through_pain_removal_AuraScript();
        }
};

/// Called by Bane of Havoc - 200548
class spell_warl_bane_of_havoc : public SpellScriptLoader
{
    public:
        spell_warl_bane_of_havoc() : SpellScriptLoader("spell_warl_bane_of_havoc") { }

        class spell_warl_bane_of_havoc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_bane_of_havoc_AuraScript);

            void HandleAfterApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster ||!l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::HavocTarget].insert(l_Target->GetGUID());
            }

            void HandleAfterRemove(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::HavocTarget].erase(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_bane_of_havoc_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_bane_of_havoc_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_bane_of_havoc_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Drain Life - 689 - Drain Soul - 198590
/// Called for Rot and Decay - 212371
class spell_warl_rot_and_decay : public SpellScriptLoader
{
    public:
        spell_warl_rot_and_decay() : SpellScriptLoader("spell_warl_rot_and_decay") { }

        class spell_warl_rot_and_decay_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_rot_and_decay_AuraScript);

            enum eSpells
            {
                RotAndDecay                 = 212371,
                CorruptionAura              = 146739,
                AgonyAura                   = 980,
                FisrtUnstableAffliction     = 233490,
                SecondUnstableAffliction    = 233496,
                ThirdUnstableAffliction     = 233497,
                FourthUnstableAffliction    = 233498,
                FifthUnstableAffliction     = 233499

            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::CorruptionAura,
                eSpells::AgonyAura,
                eSpells::FisrtUnstableAffliction,
                eSpells::SecondUnstableAffliction,
                eSpells::ThirdUnstableAffliction,
                eSpells::FourthUnstableAffliction,
                eSpells::FifthUnstableAffliction
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::RotAndDecay, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                for (eSpells l_SpellId : m_Spells)
                {
                    if (Aura* l_Aura = l_Target->GetAura(l_SpellId, l_Caster->GetGUID()))
                        l_Aura->SetDuration(l_Aura->GetDuration() + l_AuraEffect->GetAmount());
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_rot_and_decay_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_LEECH);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_rot_and_decay_AuraScript();
        }
};

/// Last Update 7.1.5 Build 232420
/// Called by Conflagrate - 17962
/// Called for Backdraft - 196406
class spell_warl_backdraft : public SpellScriptLoader
{
    public:
        spell_warl_backdraft() : SpellScriptLoader("spell_warl_backdraft") { }

        class spell_warl_backdraft_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_backdraft_SpellScript);

            enum eSpells
            {
                Backdraft       = 196406,
                BackdraftBuff   = 117828,
                Conflagrate     = 17962
            };

            void HandleBuffConflag()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::Backdraft))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::BackdraftBuff, true);
                l_Caster->CastSpell(l_Caster, eSpells::BackdraftBuff, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_backdraft_SpellScript::HandleBuffConflag);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_backdraft_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called by Incinerate - 29722 - Chaos Bolt - 116858
/// Called for Backdraft - 196406
class spell_warl_backdraft_consumption : public SpellScriptLoader
{
    public:
        spell_warl_backdraft_consumption() : SpellScriptLoader("spell_warl_backdraft_consumption") { }

        class spell_warl_backdraft_consumption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_backdraft_consumption_SpellScript);

            enum eSpells
            {
                BackDraft   = 117828,
                Havoc       = 80240
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                Aura* l_Backdraft = l_Caster->GetAura(eSpells::BackDraft);
                if (!l_Backdraft)
                    return;

                Spell* l_Spell = GetSpell();
                if (l_Spell && !l_Target->HasAura(eSpells::Havoc))
                    l_Backdraft->DropStack();
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_backdraft_consumption_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_backdraft_consumption_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called Chaos bolt 116858
class spell_warl_chaos_bolt : public SpellScriptLoader
{
public:
    spell_warl_chaos_bolt() : SpellScriptLoader("spell_warl_chaos_bolt") { }

    class spell_warl_chaos_bolt_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_chaos_bolt_SpellScript);

        enum eSpells
        {
            VarothensRestraint                  = 213014,
            Havoc                               = 80240,
            CryHavoc                            = 238110,
            CryHavocDamage                      = 243011,
            Nighthold_Destru_T19_2P_SetAura     = 212018,
            Nighthold_Destru_T19_2P_SetProc     = 212019
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->HasAura(eSpells::Nighthold_Destru_T19_2P_SetAura))
                return;

            l_Caster->CastSpell(l_Caster, eSpells::Nighthold_Destru_T19_2P_SetProc, true);
        }

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            if (GetOriginalCaster() && GetOriginalCaster() != l_Caster)
            {
                if (GetOriginalCaster()->IsValidAttackTarget(l_Caster))
                        if (GetOriginalCaster()->IsPlayer() && GetOriginalCaster()->HasAura(eSpells::VarothensRestraint))
                            if (GetOriginalCaster()->m_SpellHelper.GetUint32(eSpellHelpers::ChaosBoltDamage) > 0)
                                SetHitDamage(GetOriginalCaster()->m_SpellHelper.GetUint32(eSpellHelpers::ChaosBoltDamage));
            }
            else
            {
                if (GetHitDamage() > 0)
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ChaosBoltDamage) = GetHitDamage();
            }

            if (Aura* l_Aura = l_Caster->GetAura(eSpells::VarothensRestraint))
            {
                TriggerCastFlags l_TriggerFlags = TriggerCastFlags(TRIGGERED_NONE);
                if (GetSpell()->getTriggerCastFlags() == l_TriggerFlags)
                {
                    if (roll_chance_i(20))
                    {
                        if (SpellInfo const* l_effectInfo = sSpellMgr->GetSpellInfo(eSpells::VarothensRestraint))
                        {
                            int32 l_Dist = l_effectInfo->Effects[1].BasePoints;
                            std::list<Unit*> l_TargetList;
                            JadeCore::NearestAttackableUnitInObjectRangeCheck u_check(l_Target, l_Caster, l_Dist);
                            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> l_Searcher(l_Target, l_TargetList, u_check);
                            l_Target->VisitNearbyObject(l_Dist, l_Searcher);

                            if (!l_TargetList.empty())
                            {
                                l_TargetList.remove_if([l_Target, l_Caster](WorldObject* p_Object) -> bool
                                {
                                    if (!p_Object || !p_Object->ToUnit())
                                        return true;

                                    if (p_Object->ToUnit() == l_Target)
                                        return true;

                                    if (p_Object->ToUnit() == l_Caster)
                                        return true;

                                    if (!p_Object->ToUnit()->isInCombat())
                                        return true;

                                    return false;
                                });

                                if (!l_TargetList.empty() && l_Target != l_Caster)
                                {
                                    l_TargetList.resize(1);
                                    l_Target->CastSpell(l_TargetList.front(), GetSpellInfo()->Id, true, nullptr, nullptr, l_Caster->GetGUID());
                                }
                            }
                        }
                    }
                }
                else
                    l_Aura->SetCustomData(0); // resetting
            }

            if (l_Caster->HasAura(eSpells::CryHavoc) && l_Target->HasAura(eSpells::Havoc))
            {
                SpellInfo const * l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CryHavocDamage);
                const SpellRadiusEntry* l_RadiusEntry = l_SpellInfo->Effects[EFFECT_1].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                std::list<Unit*> l_TargetList;
                float l_Radius = l_RadiusEntry->RadiusMax;
                JadeCore::AnyUnitInObjectRangeCheck l_Check(l_Target, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnitInObjectRangeCheck> l_Searcher(l_Target, l_TargetList, l_Check);
                l_Target->VisitNearbyObject(l_Radius, l_Searcher);

                bool l_HasEnemies = false;
                for (auto l_Itr : l_TargetList)
                {
                    if (l_HasEnemies)
                        break;

                    if (!l_Itr->IsValidAttackTarget(l_Caster))
                        continue;

                    if (l_Itr == l_Target)
                        continue;

                    l_HasEnemies = true;
                }

                if (l_HasEnemies)
                    l_Caster->CastSpell(l_Target, eSpells::CryHavocDamage, true);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_warl_chaos_bolt_SpellScript::HandleAfterCast);
            OnHit += SpellHitFn(spell_warl_chaos_bolt_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_chaos_bolt_SpellScript();
    }
};

/// Last Update 7.1.5 Build 22522
/// Called by Singe Magic (Honor talent) - 212623
/// SpellId 89808
class spell_warl_singe_magic : public SpellScriptLoader
{
    public:
        spell_warl_singe_magic() : SpellScriptLoader("spell_warl_singe_magic") { }

        class spell_warl_singe_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_singe_magic_SpellScript);

            enum eSpells
            {
                SingeMagicEffect = 212620,
                SingeMagic       = 89808
            };

            enum eCreatures
            {
                Imp     = 416,
                FelImp  = 58959
            };

            Unit* GetPetCaster()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return nullptr;

                if (l_Caster->IsPlayer() && l_Caster->GetAffectingPlayer() != nullptr)
                {
                    Guardian* l_pet = l_Caster->GetAffectingPlayer()->GetGuardianPet();

                    l_Caster = l_pet;
                }

                return (l_Caster);
            }

            void HandleOnEffect(SpellEffIndex)
            {
                if (m_alreadyHit)
                    return;
                m_alreadyHit = true;

                Unit* l_Caster = GetPetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster)
                    return;

                if (!l_Target)
                    l_Target = l_Caster;

                SpellInfo const* l_effectInfo = sSpellMgr->GetSpellInfo(eSpells::SingeMagicEffect);
                if (!l_effectInfo)
                    return;

                int32_t l_damage = l_Target->GetHealth() * l_effectInfo->Effects[1].BasePoints / 100;

                l_Caster->CastCustomSpell(l_Target, SingeMagicEffect, nullptr, &l_damage, nullptr, false);
            }

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetPetCaster();

                if (!l_Caster)
                    return SpellCastResult::SPELL_FAILED_NO_PET;

                if (l_Caster->GetEntry() != eCreatures::Imp && l_Caster->GetEntry() != eCreatures::FelImp)
                    return SpellCastResult::SPELL_FAILED_WRONG_BATTLE_PET_TYPE;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_singe_magic_SpellScript::HandleCheckCast);
                switch (m_scriptSpellId)
                {
                    case eSpells::SingeMagic:
                        OnEffectHitTarget += SpellEffectFn(spell_warl_singe_magic_SpellScript::HandleOnEffect, EFFECT_0, SPELL_EFFECT_DISPEL);
                        break;
                    default:
                        OnEffectHitTarget += SpellEffectFn(spell_warl_singe_magic_SpellScript::HandleOnEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
                        break;
                }
            }

            bool m_alreadyHit;

        public:
            spell_warl_singe_magic_SpellScript() : m_alreadyHit(false) {}
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_singe_magic_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Call Felhunter - 212619
class spell_warl_call_fellhunter : public SpellScriptLoader
{
    public:
        spell_warl_call_fellhunter() : SpellScriptLoader("spell_warl_call_fellhunter") { }

        class spell_warl_call_fellhunter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_call_fellhunter_SpellScript);

            enum eCreatures
            {
                CalledFelhunter = 86723
            };

            enum eSpells
            {
                SummonFelhunter = 173552
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon && (l_Summon->GetEntry() == WarlockPet::FelHunter || l_Summon->GetEntry() == WarlockPet::Observer))
                        return SpellCastResult::SPELL_FAILED_ALREADY_HAVE_PET;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonFelhunter, true);

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon && l_Summon->GetEntry() == eCreatures::CalledFelhunter)
                    {
                        l_Summon->SetTarget(l_Target->GetGUID());
                        return;
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_call_fellhunter_SpellScript::HandleCheckCast);
                AfterCast += SpellCastFn(spell_warl_call_fellhunter_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_call_fellhunter_SpellScript();
        }
};

/// Last Update 7.1.5 Build 22522
/// Called by Unstable affliction - 233490, 233496, 233497, 233498, 233499
/// Spell ID 208821
class spell_warl_stretens_insanity : public SpellScriptLoader
{
    public:
        spell_warl_stretens_insanity() : SpellScriptLoader("spell_warl_stretens_insanity") { }

        class spell_warl_stretens_insanity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_stretens_insanity_AuraScript);

            enum eSpells
            {
                StretensInsanity        = 208821,
                StretensInsanityProc    = 208822,

                UnstableAffliction      = 233490,
                UnstableAffliction1     = 233496,
                UnstableAffliction2     = 233497,
                UnstableAffliction3     = 233498,
                UnstableAffliction4     = 233499
            };

            std::vector<eSpells> m_Spells = {
                eSpells::UnstableAffliction,
                eSpells::UnstableAffliction1,
                eSpells::UnstableAffliction2,
                eSpells::UnstableAffliction3,
                eSpells::UnstableAffliction4
            };

            void HandleAfterApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Caster || !l_Caster->HasAura(eSpells::StretensInsanity))
                    return;

                uint8 l_UnstableAfflictionaCount = 0;
                for (eSpells l_Spell : m_Spells)
                    if (l_Target->HasAura(l_Spell, l_Caster->GetGUID()))
                        l_UnstableAfflictionaCount++;

                if (l_UnstableAfflictionaCount == 1)
                    l_Caster->CastSpell(l_Caster, eSpells::StretensInsanityProc, true);
            }

            void HandleAfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Caster->HasAura(eSpells::StretensInsanityProc))
                    return;

                uint8 l_UnstableAfflictionaCount = 0;
                for (eSpells l_Spell : m_Spells)
                    if (l_Target->HasAura(l_Spell, l_Caster->GetGUID()))
                        l_UnstableAfflictionaCount++;

                int32 l_AuraCount = l_Caster->GetAuraCount(eSpells::StretensInsanityProc);

                if (l_UnstableAfflictionaCount == 0)
                    l_AuraCount--;

                l_Caster->SetAuraStack(eSpells::StretensInsanityProc, l_Caster, l_AuraCount);
                if (l_UnstableAfflictionaCount == 0 && l_AuraCount == 0)
                    l_Caster->RemoveAura(eSpells::StretensInsanityProc);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_stretens_insanity_AuraScript::HandleAfterApply, EFFECT_0, SPELL_EFFECT_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_stretens_insanity_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_EFFECT_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_stretens_insanity_AuraScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Called by Corruption - 146739
/// Spell ID 207952
class spell_warl_sacrolashs_dark_strike : public SpellScriptLoader
{
    public:
        spell_warl_sacrolashs_dark_strike() : SpellScriptLoader("spell_warl_sacrolashs_dark_strike") { }

        class spell_warl_sacrolashs_dark_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_sacrolashs_dark_strike_AuraScript);

            enum eSpells
            {
                SacrolashsDarkStrikeAura = 207952,
                SacrolashsDarkStrikeProc = 207953,
            };

            void HandleAfterApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::SacrolashsDarkStrikeAura))
                    return;

                Creature * l_Creature = l_Target->ToCreature();

                if (!l_Creature)
                {
                    l_Caster->CastSpell(l_Target, eSpells::SacrolashsDarkStrikeProc, true);
                    return;
                }

                if (CreatureTemplate const* l_CreatureTemplate = l_Creature->GetCreatureTemplate())     ///< Prevents the spell from slowing down bosses, since the proc isnt a mechanic snare, but a MS debuff
                {
                    if (l_CreatureTemplate->rank >= CreatureEliteType::CREATURE_ELITE_RAREELITE)
                        return;

                    l_Caster->CastSpell(l_Target, eSpells::SacrolashsDarkStrikeProc, true);
                }
            }

            void HandleAfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Target = GetTarget();

                if (!l_Target || !l_Target->HasAura(eSpells::SacrolashsDarkStrikeProc))
                    return;

                l_Target->RemoveAura(eSpells::SacrolashsDarkStrikeProc);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_sacrolashs_dark_strike_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_sacrolashs_dark_strike_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_sacrolashs_dark_strike_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Grimoire of Synergy - 171982
class spell_warl_grimoire_of_synergy_pvp : public SpellScriptLoader
{
    public:
        spell_warl_grimoire_of_synergy_pvp() : SpellScriptLoader("spell_warl_grimoire_of_synergy_pvp") { }

        class spell_warl_grimoire_of_synergy_pvp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_grimoire_of_synergy_pvp_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.75f;   ///< Grimoire of Synergy bonus damage is reduced by 25% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_grimoire_of_synergy_pvp_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_grimoire_of_synergy_pvp_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Reverse Entropy - 205148
class spell_warl_reverse_entropy : public SpellScriptLoader
{
    public:
        spell_warl_reverse_entropy() : SpellScriptLoader("spell_warl_reverse_entropy") { }

        class spell_warl_reverse_entropy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_reverse_entropy_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.50f;   ///< Reverse entropy casting time reduction is reduced by 25% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_reverse_entropy_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_reverse_entropy_AuraScript();
        }
};

/// Last Update: 7.1.5 23420
/// Called by Backdraft - 117828
class spell_warl_backdraft_pvp : public SpellScriptLoader
{
    public:
        spell_warl_backdraft_pvp() : SpellScriptLoader("spell_warl_backdraft_pvp") { }

        class spell_warl_backdraft_pvp_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_backdraft_pvp_AuraScript);

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

                p_Amount = l_AuraEffect->GetBaseAmount() * 0.50f;   ///< Backdraft casting time reduction is reduced by 50% in PvP
            }

            bool CanRefreshProcDummy(bool p_Decrease)
            {
                GetAura()->RefreshSpellMods();
                SetCharges((GetCharges() + 3) > 6 ? 6 : GetCharges() + 3);
                return false;
            }

            void Register() override
            {
                CanRefreshProc += AuraCanRefreshProcFn(spell_warl_backdraft_pvp_AuraScript::CanRefreshProcDummy);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_backdraft_pvp_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_backdraft_pvp_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Sin'dorei Spite - 208868
class spell_warl_sindorei_spite : public SpellScriptLoader
{
    public:
        spell_warl_sindorei_spite() : SpellScriptLoader("spell_warl_sindorei_spite") { }

        class spell_warl_sindorei_spite_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_sindorei_spite_AuraScript);

            enum eSpells
            {
                SummonDoomguardGrimoire = 157757,
                SummonInfernalGrimoire = 157898,
                SummonDoomguard = 18540,
                SummonInfernal = 1122,
                SinDoreiSpite = 208871,
                Debuff = 242690
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::SummonDoomguard && l_SpellInfo->Id != eSpells::SummonInfernal
                    && l_SpellInfo->Id != eSpells::SummonDoomguardGrimoire && l_SpellInfo->Id != eSpells::SummonInfernalGrimoire))
                    return false;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                if (l_Caster->HasAura(eSpells::Debuff))
                    return false;

                return true;
            }

            void HandleProc(ProcEventInfo& /* p_ProcEventInfo */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::Debuff))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SinDoreiSpite, true);
                l_Caster->AddAura(eSpells::Debuff, l_Caster);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_sindorei_spite_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warl_sindorei_spite_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_sindorei_spite_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Wakener's Loyalty - 236199
class PlayerScript_Wakeners_Loyalty : public PlayerScript
{
    public:
        PlayerScript_Wakeners_Loyalty() : PlayerScript("PlayerScript_Wakeners_Loyalty") { }

        enum eSpells
        {
            WakenersLoyalty     = 236199,
            WakenersLoyaltyBuff = 236200
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_SOUL_SHARDS || p_OldValue <= p_NewValue || p_Regen || !p_After)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WakenersLoyalty);
            if (p_Player->GetActiveSpecializationID() != SPEC_WARLOCK_DEMONOLOGY || !p_Player->HasAura(eSpells::WakenersLoyalty) || !l_SpellInfo)
                return;

            if ((p_OldValue - p_NewValue)/10 > 5)
                return;

            for (uint8 l_Itr = 0; l_Itr < (p_OldValue - p_NewValue)/10; ++l_Itr)
                p_Player->CastSpell(p_Player, eSpells::WakenersLoyaltyBuff, true);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Lord of Flames - 224103
class spell_warl_lord_of_flames : public SpellScriptLoader
{
    public:
        spell_warl_lord_of_flames() : SpellScriptLoader("spell_warl_lord_of_flames") { }

        class spell_warl_lord_of_flames_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_lord_of_flames_AuraScript);

            enum eSpells
            {
                SummonInfernal          = 1122,
                LordOfFlamesMarquer     = 226802,
                SummonLittleInfernal    = 224105
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::SummonInfernal || l_Caster->HasAura(eSpells::LordOfFlamesMarquer))
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_Spell)
                    return;

                WorldLocation const* l_Dest = l_Spell->GetDestTarget();
                if (!l_Dest)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint8 l_LittleInfernalsCount = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (Battleground* l_Bg = l_Player->GetBattleground())
                    if (l_Bg->IsRated()) /// If battleground rated or arena not skirmish
                        l_LittleInfernalsCount = 1;

                for (uint8 l_I = 0; l_I < l_LittleInfernalsCount; ++l_I)
                    l_Caster->CastSpell(*l_Dest, eSpells::SummonLittleInfernal, true);

                l_Caster->CastSpell(l_Caster, eSpells::LordOfFlamesMarquer, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_lord_of_flames_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warl_lord_of_flames_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_lord_of_flames_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Meteor Strike - 171017
class spell_warl_meteor_strike : public SpellScriptLoader
{
    public:
        spell_warl_meteor_strike() : SpellScriptLoader("spell_warl_meteor_strike") { }

        class spell_warl_meteor_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_meteor_strike_SpellScript);

            enum eSpells
            {
                LordOfFlames            = 224103,
                LordOfFlamesMarquer     = 226802,
                SummonLittleInfernal    = 224105
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LordOfFlames);
                if (!l_Caster || !l_SpellInfo)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner || !l_Owner->HasAura(eSpells::LordOfFlames) || l_Owner->HasAura(eSpells::LordOfFlamesMarquer))
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                uint8 l_LittleInfernalsCount = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (Battleground* l_Bg = l_Player->GetBattleground())
                    if (l_Bg->IsRated()) /// If battleground rated or arena not skirmish
                        l_LittleInfernalsCount = 1;

                for (uint8 l_I = 0; l_I < l_LittleInfernalsCount; ++l_I)
                    l_Caster->CastSpell(l_Caster, eSpells::SummonLittleInfernal, true);

                l_Owner->CastSpell(l_Caster, eSpells::LordOfFlamesMarquer, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_warl_meteor_strike_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_meteor_strike_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Wrath Of Consumption - 199472 - Souls Of The Damned - 216722
class PlayerScript_Wrath_Of_Consumption_Reap_Soul : public PlayerScript
{
    public:
        PlayerScript_Wrath_Of_Consumption_Reap_Soul() :PlayerScript("PlayerScript_Wrath_Of_Consumption_Reap_Soul") { }

        enum eSpells
        {
            WrathOfConsumption      = 199472,
            WrathOfConsumptionBuff  = 199646,
            SoulsOfTheDamned        = 216722,
            ReapSoulBuff            = 216695
        };

        void OnKillReward(Unit* p_Killed, Unit* p_Attacker)
        {
            Player* l_Player = p_Attacker->ToPlayer();

            if (!l_Player)
                return;

            if (!l_Player->IsInMap(p_Killed))
                return;

            if (l_Player->GetActiveSpecializationID() != SPEC_WARLOCK_AFFLICTION)
                return;

            if (l_Player->HasAura(eSpells::WrathOfConsumption))
                l_Player->CastSpell(l_Player, eSpells::WrathOfConsumptionBuff, true);

            if (l_Player->HasAura(eSpells::SoulsOfTheDamned))
                l_Player->CastSpell(l_Player, eSpells::ReapSoulBuff, true);
        }
};

/// Last Update 7.1.5 Build 23420
/// The Expendables - 211219
class PlayerScript_The_Expendables : public PlayerScript
{
    public:
        PlayerScript_The_Expendables() :PlayerScript("PlayerScript_The_Expendables") { }

        enum eSpells
        {
            TheExpendables = 211218
        };

        enum eNPCs
        {
            WildImp         = 55659,
            WildImpImproved = 99737
        };

        std::vector<uint32> m_NPCs =
        {
            eNPCs::WildImp,
            eNPCs::WildImpImproved
        };

        void OnMinionDespawn(Player* p_Player, Minion* p_Minion)
        {
            if (!p_Player || !p_Minion || std::find(m_NPCs.begin(), m_NPCs.end(), p_Minion->GetEntry()) == m_NPCs.end())
                return;

            if (p_Player->IsDuringRemoveFromWorld())
                return;

            std::list<Unit*> l_Pets;

            for (auto l_Pet : p_Player->m_Controlled)
            {
                if (!l_Pet->IsPlayer())
                    l_Pets.push_back(l_Pet);
            }

            for (auto l_Pet : l_Pets)
                l_Pet->CastSpell(l_Pet, eSpells::TheExpendables, true);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Stolen Power - 211529
/// Called fort Stolen Power - 211583
class spell_warl_stolen_power : public SpellScriptLoader
{
    public:
        spell_warl_stolen_power() : SpellScriptLoader("spell_warl_stolen_power") { }

        class spell_warl_stolen_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_stolen_power_AuraScript);

            enum eSpells
            {
                StolenPowerBuff = 211583
            };

            void OnReapply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Mode*/)
            {
                Aura* l_Aura = p_AuraEffect->GetBase();
                Unit* l_Caster = GetCaster();
                if (!l_Aura || l_Aura->GetStackAmount() < 100)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::StolenPowerBuff, true);
                l_Caster->RemoveAurasDueToSpell(m_scriptSpellId);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_stolen_power_AuraScript::OnReapply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_stolen_power_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Corruption - 146739, Unstable Affliction - 233490, 233496, 233497, 233498, 233499, Agony - 980, Seed of Corruption - 27243, Drain Soul - 198590
class spell_warl_deaths_embrace : public SpellScriptLoader
{
    public:
        spell_warl_deaths_embrace() : SpellScriptLoader("spell_warl_deaths_embrace") { }

        class spell_warl_deaths_embrace_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_deaths_embrace_AuraScript);

            enum eSpells
            {
                DeathsEmbrace       = 234876,
                SeedOfCorruption    = 27243,
                DrainSoul           = 198590
            };

            void HandleCalcAmount(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Target || !l_Aura || !l_Caster->HasAura(eSpells::DeathsEmbrace))
                    return;

                uint32 l_Id = l_Aura->GetId();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DeathsEmbrace);
                if (!l_SpellInfo)
                    return;

                AuraEffect* l_Effect = const_cast<AuraEffect*>(p_AurEff);
                int32 l_CurAmount = p_AurEff->GetAmount();
                int32 l_NewAmount = l_CurAmount;

                float l_Threshold = l_SpellInfo->Effects[1].BasePoints;

                if (l_Target->GetHealthPct() < l_Threshold)
                    l_NewAmount *= 1 + (((float)l_SpellInfo->Effects[0].BasePoints / 100.f) * (1 - (l_Target->GetHealthPct() / 100.f) / (l_Threshold / 100.f)));

                if (l_NewAmount == l_CurAmount)
                    return;

                l_Effect->ChangeAmount(l_NewAmount);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::SeedOfCorruption:
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_deaths_embrace_AuraScript::HandleCalcAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
                        break;
                    case eSpells::DrainSoul:
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_deaths_embrace_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_LEECH);
                        break;
                    default:
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_deaths_embrace_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_deaths_embrace_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Deadwind Harvester - 216708
class spell_warl_deadwind_harvester : public SpellScriptLoader
{
    public:
        spell_warl_deadwind_harvester() : SpellScriptLoader("spell_warl_deadwind_harvester") { }

        class spell_warl_deadwind_harvester_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_deadwind_harvester_AuraScript);

            enum eSpells
            {
                SoulLeech       = 108366,
                DemonSkin       = 219272
            };

            void HandleRecalculateAuras(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_SoulLeechShield = 0;

                if (l_Caster->HasSpell(eSpells::DemonSkin))      ///< With November 6th 2017 cycle update, Reaping Souls was reseting Soul Leech Shield if you had Demonic Skin talent
                {
                    AuraEffect* l_AuraEff = l_Caster->GetAuraEffect(eSpells::SoulLeech, EFFECT_0);
                    if (l_AuraEff)
                        l_SoulLeechShield = l_AuraEff->GetAmount();
                }

                if (l_SoulLeechShield != 0)
                    l_Caster->CastCustomSpell(l_Caster, eSpells::SoulLeech, &l_SoulLeechShield, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_deadwind_harvester_AuraScript::HandleRecalculateAuras, EFFECT_2, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_deadwind_harvester_AuraScript::HandleRecalculateAuras, EFFECT_2, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_deadwind_harvester_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Soul Effigy - 205178
class spell_warl_soul_effigy : public SpellScriptLoader
{
    public:
        spell_warl_soul_effigy() : SpellScriptLoader("spell_warl_soul_effigy") { }

        class spell_warl_soul_effigy_SpellScript: public SpellScript
        {
            PrepareSpellScript(spell_warl_soul_effigy_SpellScript);

            enum eSpells
            {
                SoulEffigySum   = 205247,
                SoulEffigyAura  = 205178
            };

            enum eNpc
            {
                SoulEffigy = 103679
            };

            uint64 m_SumGUID = 0;

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                Creature* l_Creature = l_Caster->FindNearestCreature(eNpc::SoulEffigy, 50);
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Creature || !l_Player)
                    return;

                l_Creature->ClearVisibleOnlyForSomePlayers(); ///< Set the creature onyly visible for the caster
                l_Creature->AddPlayerInPersonnalVisibilityList(l_Player->GetGUID());

                l_Creature->CastSpell(l_Creature, eSpells::SoulEffigySum, true);
                l_Creature->SetMaxHealth(l_Target->GetMaxHealth(l_Caster));
                l_Player->SetSelection(l_Creature->GetGUID());
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_soul_effigy_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_soul_effigy_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
class PlayerScript_Soul_Flame : public PlayerScript
{
public:
    PlayerScript_Soul_Flame() :PlayerScript("PlayerScript_Soul_Flame") { }

    enum eSpells
    {
        SoulFlame = 199471,
        SoulFlameDamage = 199581
    };


    void OnKill(Player* p_Killer, Unit* p_Killed, bool p_BeforeAuraRemoved) override
    {
        if (!p_Killer || !p_Killed || !p_BeforeAuraRemoved)
            return;

        std::list<uint64> l_ThreatListPlayerGuids;

        for (HostileReference* l_Ref : p_Killed->getThreatManager().GetThreatList())
        {
            if (l_Ref->getThreat() < 1)
                continue;

            if (Unit* l_Unit = l_Ref->getTarget())
            {
                if (!l_Unit->isAlive())
                    continue;

                if (Player* l_Player = l_Unit->ToPlayer())
                    l_ThreatListPlayerGuids.push_back(l_Player->GetGUID());
            }
        }

        for (uint64 l_PlayerGuid : l_ThreatListPlayerGuids)
        {
            Player* l_Player = sObjectAccessor->GetPlayer(*p_Killer, l_PlayerGuid);
            if (!l_Player)
                continue;

            if (l_Player->HasAura(eSpells::SoulFlame))
                l_Player->CastSpell(p_Killed, eSpells::SoulFlameDamage, true);
        }
    }
};


/// Last Update 7.1.5 Build 23420
/// Called by Soul Flame - 199581
class spell_warl_soul_flame_damages : public SpellScriptLoader
{
    public:
        spell_warl_soul_flame_damages() : SpellScriptLoader("spell_warl_soul_flame_damages") { }

        class spell_warl_soul_flame_damages_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soul_flame_damages_SpellScript);

            enum eNpc
            {
                SoulEffigy = 103679
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Creature* l_Target = GetHitCreature();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->GetEntry() == eNpc::SoulEffigy)
                    SetHitDamage(0);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_soul_flame_damages_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_soul_flame_damages_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dimension Ripper - 219415
class spell_warl_dimension_ripper : public SpellScriptLoader
{
    public:
        spell_warl_dimension_ripper() : SpellScriptLoader("spell_warl_dimension_ripper") { }

        class spell_warl_dimension_ripper_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_dimension_ripper_AuraScript);

            enum eSpells
            {
                Incinerate      = 29722,
                DimensionalRift = 196586
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Incinerate)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                AuraEffect* l_AuraEffect = GetEffect(EFFECT_0);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DimensionalRift);
                if (!l_Caster || !l_AuraEffect || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_dimension_ripper_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warl_dimension_ripper_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_dimension_ripper_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26124
/// Called by Thal'kiels Discord 211720
class spell_warl_thalkiels_discord : public SpellScriptLoader
{
    public:
        spell_warl_thalkiels_discord() : SpellScriptLoader("spell_warl_thalkiels_discord") { }

        class spell_warl_thalkiels_discord_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_thalkiels_discord_AuraScript);

            enum eSpells
            {
                ThalkielsDiscordTeleport    = 211729,
                ShadowBolt                  = 686,
                Demonwrath                  = 193439,
                Demonbolt                   = 157695
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::Demonwrath && l_SpellInfo->Id != eSpells::ShadowBolt && l_SpellInfo->Id != eSpells::Demonbolt)
                    return false;

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

               l_Caster->CastSpell(l_Target, eSpells::ThalkielsDiscordTeleport, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_thalkiels_discord_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_warl_thalkiels_discord_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_thalkiels_discord_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Soul Harvest - 196098
class spell_warl_soul_harvest : public SpellScriptLoader
{
    public:
        spell_warl_soul_harvest() : SpellScriptLoader("spell_warl_soul_harvest") { }

        enum eSpells
        {
            TheMasterHarvester  = 248113
        };

        class spell_warl_soul_harvest_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soul_harvest_SpellScript);

            enum eNpc
            {
                SoulEffigy = 103679
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpells::TheMasterHarvester) = 0;

                if (Aura* l_Aura = l_Caster->GetAura(m_scriptSpellId))
                    l_Caster->m_SpellHelper.GetUint32(eSpells::TheMasterHarvester) = l_Aura->GetDuration();
            }

            void HandleAfterCast()
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Aura* l_Aura = l_Caster->GetAura(l_SpellInfo->Id);
                if (!l_Aura)
                    return;

                if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::MasterHarvester))
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TheMasterHarvester);
                    if (l_SpellInfo)
                    {
                        l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS);
                        l_Aura->SetMaxDuration(l_SpellInfo->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS);
                    }
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::MasterHarvester) = false;
                    return;
                }

                int32 l_NbTargets = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::SoulHarvest);

                int32 l_newDuration = l_SpellInfo->GetMaxDuration() + l_Caster->m_SpellHelper.GetUint32(eSpells::TheMasterHarvester);

                for (uint8 l_Idx = 0; l_Idx < l_NbTargets; ++l_Idx)
                    l_newDuration += l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS;

                l_Aura->SetMaxDuration(l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
                l_Aura->SetDuration(std::min(l_newDuration, l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS));
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_warl_soul_harvest_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_warl_soul_harvest_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_soul_harvest_SpellScript();
        }

        class spell_warl_soul_harvest_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_soul_harvest_AuraScript);

            bool CanRefreshProcDummy(bool p_Decrease)
            {
                return false;
            }

            void Register() override
            {
                CanRefreshProc += AuraCanRefreshProcFn(spell_warl_soul_harvest_AuraScript::CanRefreshProcDummy);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_soul_harvest_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Agony - 980, Doom - 603, Immolate - 157736
class spell_warl_soul_harvest_targets : public SpellScriptLoader
{
    public:
        spell_warl_soul_harvest_targets() : SpellScriptLoader("spell_warl_soul_harvest_targets") { }

        class spell_warl_soul_harvest_targets_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_soul_harvest_targets_AuraScript);

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32& l_NbTargets = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::SoulHarvest);
                l_NbTargets++;
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint32 &l_NbTargets = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::SoulHarvest);

                if (l_NbTargets > 0)
                    l_NbTargets--;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_soul_harvest_targets_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_soul_harvest_targets_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_soul_harvest_targets_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Soul Link - 108415
class spell_warl_soul_link : public SpellScriptLoader
{
    public:
        spell_warl_soul_link() : SpellScriptLoader("spell_warl_soul_link") { }

        class spell_warl_soul_link_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_soul_link_AuraScript);

            enum eSpells
            {
                SoulLinkEffect = 108446
            };

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SoulLinkEffect, true);
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SoulLinkEffect);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_soul_link_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_soul_link_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_soul_link_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Chaos Bolt - 116858
/// Called For SoulSnatcher - 196236
class spell_warl_soulsnatcher : public SpellScriptLoader
{
    public:
        spell_warl_soulsnatcher() : SpellScriptLoader("spell_warl_soulsnatcher") { }

        class spell_warl_soulsnatcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soulsnatcher_SpellScript);

            enum eSpells
            {
                Soulsnatcher        = 196234,
                SoulsnatcherPassive = 196236
            };

            enum eArtifactPowerId
            {
                SoulsnatcherId = 808
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const*  l_SoulsnatcherInfo = sSpellMgr->GetSpellInfo(eSpells::SoulsnatcherPassive);
                if (!l_Caster || !l_SoulsnatcherInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!roll_chance_i(l_SoulsnatcherInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eArtifactPowerId::SoulsnatcherId)))
                    return;

                l_Caster->DelayedCastSpell(l_Caster, eSpells::Soulsnatcher, true, 1);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_warl_soulsnatcher_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_soulsnatcher_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Devourer of Life - 196301
class spell_warl_devourer_of_life : public SpellScriptLoader
{
    public:
        spell_warl_devourer_of_life() : SpellScriptLoader("spell_warl_devourer_of_life") { }

        class spell_warl_devourer_of_life_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_devourer_of_life_AuraScript);

            enum eSpells
            {
                DevourerOfLife = 196301,
                DevourerOfLifeBuff = 215165

            };

            enum eArtifactPowerId
            {
                DevourerOfLifeId = 811
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const*  l_DevourerOfLife = GetSpellInfo();
                if (!l_Player || !l_DevourerOfLife)
                    return false;

                float l_Chance = 0.f;
                auto l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowerId::DevourerOfLifeId);
                if (ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowerId::DevourerOfLifeId, l_Rank - 1))
                {
                    l_Chance = l_RankEntry->AuraPointsOverride;
                }

                if (!roll_chance_f(l_Chance))
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DevourerOfLifeBuff, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_devourer_of_life_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_warl_devourer_of_life_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_devourer_of_life_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called for Sweet Soul - 190220
/// Called By Healthstone - 6262
class spell_warl_sweet_soul : public SpellScriptLoader
{
    public:
        spell_warl_sweet_soul() : SpellScriptLoader("spell_warl_sweet_soul") { }

        class spell_warl_sweet_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_sweet_soul_SpellScript);

            enum eSpells
            {
                SweetSoul           = 199220,
                SweetSoulHeal       = 199221,
                DeadwindHarvester   = 216708
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SweetSoul);
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_HealAmout = GetHitHeal();
                int32 l_HealFromSweetSoul = l_Caster->CountPctFromMaxHealth(l_SpellInfo->Effects[EFFECT_0].BasePoints);

                if (l_Caster->HasAura(eSpells::DeadwindHarvester))
                    l_HealFromSweetSoul *= 2;

                int32 l_StrongestReducedHealingAura = 0; // reduce heal
                for (AuraEffect* l_Effect : l_Caster->GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_PCT))
                {
                    int32 l_Value = l_Effect->GetAmount();
                    if (l_Value < 0) // only negative auras
                        l_StrongestReducedHealingAura = std::min(l_StrongestReducedHealingAura, l_Value);
                }
                l_HealAmout = CalculatePct(l_HealAmout, 100 + l_StrongestReducedHealingAura);
                l_HealFromSweetSoul = CalculatePct(l_HealFromSweetSoul, 100 + l_StrongestReducedHealingAura);

                if (l_Caster->HasAura(eSpells::SweetSoul))
                    SetHitHeal(l_HealAmout + l_HealFromSweetSoul);
                else
                    SetHitHeal(l_HealAmout);

                Spell* l_Spell = GetSpell();
                if (!l_Spell)
                    return;

                Item* l_Item  = l_Spell->m_CastItem;
                if (!l_Item)
                    return;

                uint64 l_WarlockGuid = l_Item->GetGuidValue(ITEM_FIELD_CREATOR);
                if (!l_WarlockGuid)
                    return;

                Unit* l_Warlock = sObjectAccessor->GetUnit(*l_Caster, l_WarlockGuid);
                if (!l_Warlock || !l_Warlock->HasAura(eSpells::SweetSoul))
                    return;

                if (l_Warlock == l_Caster)
                    return;

                SpellInfo const* l_SweetSoulInfo = sSpellMgr->GetSpellInfo(eSpells::SweetSoulHeal);
                if (!l_SweetSoulInfo)
                    return;

                ///< Deadwind Harvester double effect of artifact
                l_Caster->HealBySpell(l_Warlock, l_SweetSoulInfo, l_Warlock->HasAura(eSpells::DeadwindHarvester) ? 2 * l_HealFromSweetSoul : l_HealFromSweetSoul, false);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_sweet_soul_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_sweet_soul_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Compounding Horror 199282
class spell_warl_compounding_horror_proc : public SpellScriptLoader
{
public:
    spell_warl_compounding_horror_proc() : SpellScriptLoader("spell_warl_compounding_horror_proc") { }

    class spell_warl_compounding_horror_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_compounding_horror_proc_AuraScript);

        enum eSpells
        {
            Corruption  = 146739,
            Agony       = 980
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::Agony &&  l_SpellInfo->Id != eSpells::Corruption))
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warl_compounding_horror_proc_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warl_compounding_horror_proc_AuraScript();
    }
};


/// Last Update 7.1.5 Build 23420
/// Called By Compounding Horror 199281
class spell_warl_compounding_horror : public SpellScriptLoader
{
    public:
        spell_warl_compounding_horror() : SpellScriptLoader("spell_warl_compounding_horror") { }

        class spell_warl_compounding_horror_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_compounding_horror_AuraScript);

            enum eSpells
            {
                CompoundingHorrorDamages = 231489,
                UnstableAffliction       = 30108
            };


            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Target || !l_SpellInfo || l_SpellInfo->Id != eSpells::UnstableAffliction)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::CompoundingHorrorDamages, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warl_compounding_horror_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_compounding_horror_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Compounding Horror 231489
class spell_warl_compounding_horror_damages : public SpellScriptLoader
{
    public:
        spell_warl_compounding_horror_damages() : SpellScriptLoader("spell_warl_compounding_horror_damages") { }

        class spell_warl_compounding_horror_damages_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_compounding_horror_damages_SpellScript);

            enum eSpells
            {
                CompoundingHorror           = 199281,
                CompoundingHorrorDamages    = 231489
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CompoundingHorrorDamages);
                if (!l_Caster || !l_SpellInfo)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::CompoundingHorror);
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Aura || !l_Player)
                    return;

                int32 l_Damage = GetHitDamage() * l_Aura->GetStackAmount();
                SetHitDamage(l_Damage);

                l_Aura->Remove();
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_compounding_horror_damages_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_compounding_horror_damages_SpellScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Called by Summon Darkglare 205180 - SummonDoomguard 18540 - SummonInfernal 1122
/// Call Fel Lord 212459 - Call Observer 201996
class spell_warl_ui_timer_summon : public SpellScriptLoader
{
    public:
        spell_warl_ui_timer_summon() : SpellScriptLoader("spell_warl_ui_timer_summon") { }

        class spell_warl_ui_timer_summon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_ui_timer_summon_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->m_Controlled.empty())
                    return;

                uint64 l_MinionGUID = l_Caster->GetLastMinionSpawn();
                Unit* l_Creature = sObjectAccessor->GetUnit(*l_Caster, l_MinionGUID);
                if (l_Creature && l_Caster && l_Caster->IsPlayer())
                    l_Caster->ToPlayer()->SendTempSummonUITimer(l_Creature);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_ui_timer_summon_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_ui_timer_summon_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Soulshatter - 212356
class spell_warl_soulshatter_pvp : public SpellScriptLoader
{
    public:
        spell_warl_soulshatter_pvp() : SpellScriptLoader("spell_warl_soulshatter_pvp") { }

        class spell_warl_soulshatter_pvp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soulshatter_pvp_SpellScript);

            enum eSpells
            {
                SoulshatterShard    = 212921,
                SoulshatterHaste    = 236471
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Targets.empty())
                    return;

                p_Targets.resize(5);

                p_Targets.remove_if([l_Caster](WorldObject* l_Target) -> bool
                {
                    if (!l_Target || !l_Target->ToUnit() || !l_Target->ToUnit()->GetDoTsByCaster(l_Caster->GetGUID()))
                        return true;

                    return false;
                });
            }

            void HandleOnHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, l_Caster->GetGUID());
                l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT, l_Caster->GetGUID());
                l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH, l_Caster->GetGUID());
                l_Target->RemoveAurasByType(SPELL_AURA_PERIODIC_HEALTH_FUNNEL, l_Caster->GetGUID());

                l_Caster->CastSpell(l_Caster, eSpells::SoulshatterShard, true);
                l_Caster->CastSpell(l_Caster, eSpells::SoulshatterHaste, true);

                SetHitDamage(CalculatePct(l_Target->GetMaxHealth(), GetSpellInfo()->Effects[EFFECT_1].BasePoints));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_soulshatter_pvp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_warl_soulshatter_pvp_SpellScript::HandleOnHitTarget, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_soulshatter_pvp_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Curse of Shadows - 234877
class spell_warl_curse_of_shadows : public SpellScriptLoader
{
    public:
        spell_warl_curse_of_shadows() : SpellScriptLoader("spell_warl_curse_of_shadows") { }

        class spell_warl_curse_of_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_curse_of_shadows_AuraScript);

            enum eSpells
            {
                CurseOfShadows = 236615
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                AuraEffect* l_Effect = GetEffect(EFFECT_0);
                if (!l_Caster || !l_DamageInfo || !l_Effect)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Target || !l_SpellInfo)
                    return;

                int32 l_Amount = CalculatePct(l_DamageInfo->GetAmount(), l_Effect->GetAmount());
                l_Caster->CastCustomSpell(l_Target, eSpells::CurseOfShadows, &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_warl_curse_of_shadows_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_curse_of_shadows_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called for Drain Soul - 198590
class PlayerScript_Drain_Soul : public PlayerScript
{
    public:
        PlayerScript_Drain_Soul() :PlayerScript("PlayerScript_Drain_Soul") { }

        enum eSpells
        {
            DrainSoul = 198590
        };

        void OnSpellInterrupt(Player* p_Player, Spell* p_Spell, Spell* p_InterruptingSpell, uint32 p_InterruptingSpellId)
        {
            if (!p_Player || p_Player->GetActiveSpecializationID() != SPEC_WARLOCK_AFFLICTION)
                return;

            if (!p_Spell || p_Spell->GetSpellInfo()->Id != eSpells::DrainSoul || p_InterruptingSpellId != eSpells::DrainSoul)
                return;

            SpellInfo const* l_SpellInfo = p_Spell->GetSpellInfo();
            std::list<Spell::TargetInfo> l_Targets = p_Spell->GetTargetInfos();
            if (!l_SpellInfo || l_Targets.empty())
                return;

            int32 l_LeftDuration = p_Spell->GetTimer();
            int32 l_MaxDuration = l_SpellInfo->GetMaxDuration();

            l_MaxDuration = int32(l_MaxDuration * std::max<float>(p_Player->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_CASTING_SPEED), 0.5f));

            /// Max duration of reapplied aura should be 130% of base max duration
            int32 l_MaxAllowedDuration = CalculatePct(l_MaxDuration, 130);

            uint32& l_Helper = p_Player->m_SpellHelper.GetUint32(eSpellHelpers::DrainSoul);
            p_Player->m_SpellHelper.GetUint64(eSpellHelpers::DrainSoul) = l_Targets.front().targetGUID;

            if (l_LeftDuration + l_MaxDuration < l_MaxAllowedDuration)
                l_Helper = l_LeftDuration + l_MaxDuration;
            else
                l_Helper = l_MaxAllowedDuration;
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Felbolt - 115746
class spell_warl_pet_felbolt : public SpellScriptLoader
{
public:
    spell_warl_pet_felbolt() : SpellScriptLoader("spell_warl_pet_felbolt") { }

    class spell_warl_pet_felbolt_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_pet_felbolt_SpellScript);

        enum eSpells
        {
            Immolate = 157736
        };

        void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            int32 l_Damage = GetHitDamage();

            if (l_Target->HasAura(eSpells::Immolate, l_Owner->GetGUID()))
                AddPct(l_Damage, GetSpellInfo()->Effects[EFFECT_1].BasePoints);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_warl_pet_felbolt_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warl_pet_felbolt_SpellScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Felbolt - 115778
class spell_warl_pet_tongue_lash : public SpellScriptLoader
{
public:
    spell_warl_pet_tongue_lash() : SpellScriptLoader("spell_warl_pet_tongue_lash") { }

    class spell_warl_pet_tongue_lash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_pet_tongue_lash_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            int32 l_Damage = GetHitDamage();

            std::list<AuraEffect*> l_TargetDots = l_Target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE);
            for (AuraEffect* l_AuraEffect : l_TargetDots)
            {
                if (l_AuraEffect->GetCaster() != l_Owner)
                    continue;

                SpellInfo const* l_DotInfo = l_AuraEffect->GetSpellInfo();
                if (!l_DotInfo)
                    continue;

                if (l_DotInfo->SpellFamilyName != SPELLFAMILY_WARLOCK)
                    continue;

                AddPct(l_Damage, 50);
            }

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_warl_pet_tongue_lash_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warl_pet_tongue_lash_SpellScript();
    }
};

/// Last update 7.3.2 Build 25549
/// Called by Legendary: Soul of the Netherlord - 247603
class spell_warl_soul_of_the_netherlord : public SpellScriptLoader
{
    public:
        spell_warl_soul_of_the_netherlord() : SpellScriptLoader("spell_warl_soul_of_the_netherlord") { }

        class spell_warl_soul_of_the_netherlord_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_soul_of_the_netherlord_AuraScript);

            enum eSpells
            {
                Eradication         = 196412,
                DeathsEmbrace       = 234876,
                GrimoireOfSynergy   = 171975
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
                case SPEC_WARLOCK_AFFLICTION:
                    l_Player->CastSpell(l_Player, eSpells::DeathsEmbrace, true);
                    break;
                case SPEC_WARLOCK_DESTRUCTION:
                    l_Player->CastSpell(l_Player, eSpells::Eradication, true);
                    break;
                default:
                    l_Player->CastSpell(l_Player, eSpells::GrimoireOfSynergy, true);
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
                case SPEC_WARLOCK_AFFLICTION:
                    l_Player->RemoveAura(eSpells::DeathsEmbrace);
                    break;
                case SPEC_WARLOCK_DESTRUCTION:
                    l_Player->RemoveAura(eSpells::Eradication);
                    break;
                default:
                    l_Player->RemoveAura(eSpells::GrimoireOfSynergy);
                    break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_soul_of_the_netherlord_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_soul_of_the_netherlord_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_soul_of_the_netherlord_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Conflagrate - 17962
class spell_warl_conflagrate : public SpellScriptLoader
{
    public:
        spell_warl_conflagrate() : SpellScriptLoader("spell_warl_conflagrate") { }

        class spell_warl_conflagrate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_conflagrate_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                l_Caster->ModifyPower(POWER_SOUL_SHARDS, 5);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_warl_conflagrate_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_conflagrate_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Warlock T19 Destruction 2P Bonus 212018
class spell_warl_t19_destriction_2p_bonus : public SpellScriptLoader
{
public:
    spell_warl_t19_destriction_2p_bonus() : SpellScriptLoader("spell_warl_t19_destriction_2p_bonus") { }

    class spell_warl_t19_destriction_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_t19_destriction_2p_bonus_AuraScript);

        enum eSpells
        {
            ChaosBolt = 116858
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            /*Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ChaosBolt)
                return false;*/

            return false; ///< Proc happens on target hit, and it should happen on CAST. May be a generic issue with the proc flag tbh, but in the meantime i'll Hardcode it on the cast of Chaos Bolt
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warl_t19_destriction_2p_bonus_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warl_t19_destriction_2p_bonus_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by The Master Harvester - 248113
class PlayerScript_The_Master_Harvester : public PlayerScript
{
    public:
        PlayerScript_The_Master_Harvester() : PlayerScript("PlayerScript_The_Master_Harvester") { }

        enum eSpells
        {
            TheMasterHarvester  = 248113,
            SoulHarvest         = 196098
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_SOUL_SHARDS || p_OldValue <= p_NewValue || p_Regen || !p_After)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TheMasterHarvester);
            if (!p_Player->HasAura(eSpells::TheMasterHarvester) || !l_SpellInfo)
                return;

            uint32 l_ProcChance = 0;

            switch(p_Player->GetActiveSpecializationID())
            {
                case SPEC_WARLOCK_DEMONOLOGY:
                    l_ProcChance = l_SpellInfo->Effects[EFFECT_1].BasePoints;
                    break;
                case SPEC_WARLOCK_AFFLICTION:
                    l_ProcChance = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                    break;
                case SPEC_WARLOCK_DESTRUCTION:
                    l_ProcChance = l_SpellInfo->Effects[EFFECT_2].BasePoints;
                    break;
            }

            for (uint8 l_Itr = 0; l_Itr < (p_OldValue - p_NewValue)/10; ++l_Itr)
                if (roll_chance_i(l_ProcChance))
                {
                    p_Player->ToUnit()->m_SpellHelper.GetBool(eSpellHelpers::MasterHarvester) = true;
                    if (Aura* l_Aura = p_Player->GetAura(eSpells::SoulHarvest))
                    {
                        if (SpellInfo const* l_SpellInfo_SoulHarvest = sSpellMgr->GetSpellInfo(eSpells::SoulHarvest))
                            l_Aura->SetDuration(std::min(l_Aura->GetDuration() + l_SpellInfo->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS,
                                l_SpellInfo_SoulHarvest->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS));
                    }
                    else
                    {
                        p_Player->ToUnit()->m_SpellHelper.GetBool(eSpellHelpers::MasterHarvester) = true;
                        p_Player->CastSpell(p_Player, eSpells::SoulHarvest, true);
                    }
                }
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Rend Soul - 238144
class spell_warl_rend_soul : public SpellScriptLoader
{
    public:
        spell_warl_rend_soul() : SpellScriptLoader("spell_warl_rend_soul") { }

        class spell_warl_rend_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_rend_soul_AuraScript);

            enum eSpells
            {
                DrainSoul = 198590
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::DrainSoul)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_rend_soul_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_rend_soul_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Left Hand Of Darkness - 238073
class spell_warl_left_hand_of_darkness : public SpellScriptLoader
{
    public:
        spell_warl_left_hand_of_darkness() : SpellScriptLoader("spell_warl_left_hand_of_darkness") { }

        class spell_warl_left_hand_of_darkness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_left_hand_of_darkness_AuraScript);

            enum eSpells
            {
                LeftHandOfDarkness      = 238073,
                LeftHandOfDarknessBuff  = 238998
            };

            void HandleApply(AuraEffect const* /*p_AuraEFfect*/, AuraEffectHandleModes /*p_Modes*/)
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

                l_Pet->CastSpell(l_Pet, eSpells::LeftHandOfDarknessBuff, true);
            }

            void HandleUnapply(AuraEffect const* /*p_AuraEFfect*/, AuraEffectHandleModes /*p_Modes*/)
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

                if (l_Pet->HasAura(eSpells::LeftHandOfDarknessBuff))
                    l_Pet->RemoveAura(eSpells::LeftHandOfDarknessBuff);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_warl_left_hand_of_darkness_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_left_hand_of_darkness_AuraScript::HandleUnapply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_left_hand_of_darkness_AuraScript();
        }
};

/// Left Hand Of Darkness - 238073
class PlayerScript_Left_Hand_Of_Darkness : public PlayerScript
{
    public:
        PlayerScript_Left_Hand_Of_Darkness() :PlayerScript("PlayerScript_Left_Hand_Of_Darkness") { }

        enum eSpells
        {
            LeftHandOfDarkness      = 238073,
            LeftHandOfDarknessBuff  = 238998
        };

        void OnSummonPet(Player* p_Player)
        {
            if (!p_Player || p_Player->getClass() != CLASS_WARLOCK)
                return;

            if (!p_Player->HasAura(eSpells::LeftHandOfDarkness))
                return;

            Pet* l_Pet = p_Player->GetPet();
            if (!l_Pet)
                return;

            l_Pet->CastSpell(l_Pet, eSpells::LeftHandOfDarknessBuff, true);
        }
};

/// Left Hand Of Darkness (buff) - 238998
class spell_warl_left_hand_of_darkness_amount: public SpellScriptLoader
{
    public:
        spell_warl_left_hand_of_darkness_amount() : SpellScriptLoader("spell_warl_left_hand_of_darkness_amount") { }

        class spell_warl_left_hand_of_darkness_amount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_left_hand_of_darkness_amount_AuraScript);

            enum eSpells
            {
                LeftHandOfDarkness      = 238073,
                GrimoireOfSupremacy     = 152107
            };

            enum eArtifactPowers
            {
                LeftHandOfDarknessArtifact = 1605
            };

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_PlayerOwner = l_Owner->ToPlayer();
                if (!l_PlayerOwner)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LeftHandOfDarkness);
                if (!l_SpellInfo)
                    return;

                float l_BasePoints = l_PlayerOwner->HasAura(eSpells::GrimoireOfSupremacy) ? ((float)l_SpellInfo->Effects[EFFECT_0].BasePoints / 2.0f) : (float)l_SpellInfo->Effects[EFFECT_0].BasePoints;

                uint8 l_Rank = l_PlayerOwner->GetRankOfArtifactPowerId(eArtifactPowers::LeftHandOfDarknessArtifact);
                p_Amount = l_BasePoints * l_Rank;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_left_hand_of_darkness_amount_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_left_hand_of_darkness_amount_AuraScript();
        }
};

/// Jaws of Shadow (debuff) - 242922
class spell_warl_jaws_of_shadow_amount: public SpellScriptLoader
{
    public:
        spell_warl_jaws_of_shadow_amount() : SpellScriptLoader("spell_warl_jaws_of_shadow_amount") { }

        class spell_warl_jaws_of_shadow_amount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_jaws_of_shadow_amount_AuraScript);

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                p_Amount = 10;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_jaws_of_shadow_amount_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_jaws_of_shadow_amount_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called Feretory of Souls - 205702
class spell_warl_feretory_of_souls_proc : public SpellScriptLoader
{
    public:
        spell_warl_feretory_of_souls_proc() : SpellScriptLoader("spell_warl_feretory_of_souls_proc") { }

        class spell_warl_feretory_of_souls_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_feretory_of_souls_proc_AuraScript);

            enum eSpells
            {
                RainOfFireDamage    = 42223,
                RainOfFireOriginal  = 5740
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                SpellSchoolMask l_SchoolMask = l_DamageInfo->GetSchoolMask();
                if (!l_SchoolMask)
                    return false;

                if (l_SchoolMask != SPELL_SCHOOL_MASK_FIRE)
                    return false;

                if (l_SpellInfo->Id == eSpells::RainOfFireDamage || l_SpellInfo->Id == eSpells::RainOfFireOriginal)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_feretory_of_souls_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_feretory_of_souls_proc_AuraScript();
        }
};

/// Last Update 7.3.5
/// For Feretory of Souls
/// Called By Rain of Fire - 5740
class spell_warl_feretory_of_souls_by_rof: public SpellScriptLoader
{
    public:
        spell_warl_feretory_of_souls_by_rof() : SpellScriptLoader("spell_warl_feretory_of_souls_by_rof") { }

        class spell_warl_feretory_of_souls_by_rof_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_feretory_of_souls_by_rof_SpellScript);

            enum eSpells
            {
                FierySoulsEnergize      = 205704,
                FetorityOfSouls         = 205702
            };

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::FetorityOfSouls))
                        if (roll_chance_i(6))
                            l_Caster->CastSpell(l_Caster, eSpells::FierySoulsEnergize, true);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_warl_feretory_of_souls_by_rof_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_feretory_of_souls_by_rof_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26124
/// Called by 196099 Demonic Power
class spell_warl_demonic_power_proc : public SpellScriptLoader
{
    public:
        spell_warl_demonic_power_proc() : SpellScriptLoader("spell_warl_demonic_power_proc") { }

        class spell_warl_demonic_power_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_power_proc_AuraScript);

            enum eSpells
            {
                ChaosBarrageMissile     = 187394,
                ChaosBolt               = 215279,
                ShadowBolt              = 196657,
                SearingBolt             = 243050,
                OverrideCommandDemon    = 119904
            };

            uint32 m_UpdateTimer = IN_MILLISECONDS;

            void OnUpdate(uint32 p_Diff)
            {
                if (!m_UpdateTimer)
                    return;

                if (m_UpdateTimer <= p_Diff)
                {
                    m_UpdateTimer = IN_MILLISECONDS;

                    Unit* l_Owner = GetUnitOwner();
                    if (!l_Owner)
                        return;

                    Player* l_Player = l_Owner->ToPlayer();
                    if (!l_Player)
                        return;

                    if (l_Player->GetPet())
                        Remove();
                }
                else
                    m_UpdateTimer -= p_Diff;
            }

            bool HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id == eSpells::ChaosBarrageMissile || l_SpellInfo->Id == eSpells::ChaosBolt
                    || l_SpellInfo->Id == eSpells::ShadowBolt || l_SpellInfo->Id == eSpells::SearingBolt)
                    return false;

                for (uint8 i = 0; i < l_SpellInfo->EffectCount; ++i)
                {
                    if (l_SpellInfo->Effects[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE)
                        return true;

                    if (l_SpellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AURA)
                    {
                        if (l_SpellInfo->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE ||
                            l_SpellInfo->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_LEECH)
                            return true;
                    }
                }

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_demonic_power_proc_AuraScript::HandleOnProc);
                OnAuraUpdate += AuraUpdateFn(spell_warl_demonic_power_proc_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_demonic_power_proc_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Channel Demonfire - 196448
class spell_warl_channel_demonfire_damage : public SpellScriptLoader
{
    public:
        spell_warl_channel_demonfire_damage() : SpellScriptLoader("spell_warl_channel_demonfire_damage") { }

        class spell_warl_channel_demonfire_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_channel_demonfire_damage_SpellScript);

            enum eSpells
            {
                ChaoticEnergies          = 77220,
                OdrShawlOfTheYmirjar     = 212172,
                OdrShawlOfTheYmirjarAura = 212173
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Targets.empty())
                    return;

                if (l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ChannelDemonfireCurrentTargetGUID) == 0)
                    return;

                uint64 l_MainTargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ChannelDemonfireCurrentTargetGUID);

                p_Targets.remove_if([l_Caster, l_MainTargetGUID](WorldObject* l_Target) -> bool
                {
                    if (!l_Target || l_Target->GetGUID() == l_MainTargetGUID)
                        return true;

                    return false;
                });

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ChannelDemonfireCurrentTargetGUID) = 0;
            }

            void HandleDamage(SpellEffIndex p_EffectIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                int32 l_Damage = (int32)GetHitDamage();
                if (l_Player->HasAura(eSpells::ChaoticEnergies))
                {
                    float l_Mastery = l_Player->GetFloatValue(PLAYER_FIELD_MASTERY);
                    l_Mastery = l_Mastery + frand(0, l_Mastery);
                    l_Damage += CalculatePct(l_Damage, l_Mastery);
                }

                if (l_Caster->HasAura(eSpells::OdrShawlOfTheYmirjar) && l_Target->GetGUID() == l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HavocTarget))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::OdrShawlOfTheYmirjarAura))
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_channel_demonfire_damage_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_warl_channel_demonfire_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnEffectHitTarget += SpellEffectFn(spell_warl_channel_demonfire_damage_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_channel_demonfire_damage_SpellScript();
        }
};

/// Last Update 7.2.5 Build 24218
/// Called by Nighfall - 248813
class spell_warl_nightfall : public SpellScriptLoader
{
    public:
        spell_warl_nightfall() : SpellScriptLoader("spell_warl_nightfall")
        {}

        enum eSpells
        {
            SpellCorruptionDot = 146739
        };

        class spell_warl_nightfall_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_nightfall_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_Proc)
            {
                DamageInfo* l_DmgInfo = p_Proc.GetDamageInfo();

                if (l_DmgInfo == nullptr)
                    return false;

                SpellInfo const* l_Spell = l_DmgInfo->GetSpellInfo();

                if (l_Spell == nullptr)
                    return false;

                return l_Spell->Id == eSpells::SpellCorruptionDot;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_nightfall_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_nightfall_AuraScript();
        }
};

/// Last Update 7.3.5
/// Called By Curse of Fragility - 199954
class spell_warl_curse_of_fragility : public SpellScriptLoader
{
public:
    spell_warl_curse_of_fragility() : SpellScriptLoader("spell_warl_curse_of_fragility") { }

    class spell_warl_curse_of_fragility_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_curse_of_fragility_SpellScript);

        void HandleBeforeCast()
        {
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Target)
                return;

            if (Aura* l_Aura = l_Target->GetAura(m_scriptSpellId))
                l_Aura->Remove();
        }

        void Register() override
        {
            BeforeCast += SpellCastFn(spell_warl_curse_of_fragility_SpellScript::HandleBeforeCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warl_curse_of_fragility_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called By Cremation 212327
class spell_warl_cremation_damage : public SpellScriptLoader
{
    public:
        spell_warl_cremation_damage() : SpellScriptLoader("spell_warl_cremation_damage") { }

        class spell_warl_cremation_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_cremation_damage_SpellScript);

                void HandleDamage(SpellEffIndex p_EffIndex)
                {
                    Unit* l_Caster = GetCaster();
                    Unit* l_Target = GetHitUnit();

                    if (!l_Caster || !l_Target)
                        return;

                    Player* l_Player = l_Caster->ToPlayer();
                    if (!l_Player)
                        return;

                    uint32 l_Pct = GetSpellInfo()->Effects[p_EffIndex].BasePoints;

                    if (Battleground* l_Bg = l_Player->GetBattleground())
                        if (l_Bg->IsRated()) /// If battleground rated or arena not skirmish
                            --l_Pct;

                    uint32 l_Damage = CalculatePct(l_Target->GetMaxHealth(), l_Pct);

                    SetHitDamage(l_Damage);
                }

                void Register() override
                {
                    OnEffectHitTarget += SpellEffectFn(spell_warl_cremation_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_cremation_damage_SpellScript();
        }
};

/// Last Update : 7.3.5 Build 26365
/// Called by  Item - Warlock T20 Demonology 2P Bonus - 242293
class spell_warl_t20_demo_2p_bonus_proc : public SpellScriptLoader
{
    public:
        spell_warl_t20_demo_2p_bonus_proc() : SpellScriptLoader("spell_warl_t20_demo_2p_bonus_proc") { }

        class spell_warl_t20_demo_2p_bonus_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_t20_demo_2p_bonus_proc_AuraScript);

            enum eSpells
            {
                Demonbolt           = 157695,
                Shadowbolt          = 686,
                Demonwrath          = 193439,
                CallDreadstalkers   = 104316
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

                if (l_ProcSpellInfo->Id == eSpells::Demonbolt || l_ProcSpellInfo->Id == eSpells::Shadowbolt || l_ProcSpellInfo->Id == eSpells::Demonwrath)
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

                if (l_Player->HasSpellCooldown(eSpells::CallDreadstalkers))
                    l_Player->RemoveSpellCooldown(eSpells::CallDreadstalkers, true);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_warl_t20_demo_2p_bonus_proc_AuraScript::HandleOnCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_warl_t20_demo_2p_bonus_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_t20_demo_2p_bonus_proc_AuraScript();
        }
};

/// Last Update : 7.3.5 Build 26365
/// Called by Demonic Calling - 205145
class spell_warl_demonic_calling : public SpellScriptLoader
{
public:
    spell_warl_demonic_calling() : SpellScriptLoader("spell_warl_demonic_calling") { }

    class spell_warl_demonic_calling_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_demonic_calling_AuraScript);

        enum eSpells
        {
            ShadowBolt = 686,
            Demonbolt = 157695,
            DemonicCalling = 205146
        };

        void OnProcSB(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            PreventDefaultAction();
            Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo || !l_Caster)
                return;

            SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_ProcSpellInfo || (l_ProcSpellInfo->Id != eSpells::ShadowBolt && l_ProcSpellInfo->Id != eSpells::Demonbolt))
                return;

            if (roll_chance_i(p_AurEff->GetAmount()))
                l_Caster->CastSpell(l_Caster, eSpells::DemonicCalling, true);
        }

        void OnProcD(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            PreventDefaultAction();
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_warl_demonic_calling_AuraScript::OnProcSB, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            OnEffectProc += AuraEffectProcFn(spell_warl_demonic_calling_AuraScript::OnProcD, EFFECT_1, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warl_demonic_calling_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Soul Conduit - 215941
class PlayerScript_T20_Affliction_4P_bonus : public PlayerScript
{
    public:
        PlayerScript_T20_Affliction_4P_bonus() :PlayerScript("PlayerScript_T20_Affliction_4P_bonus") { }

        enum eSpells
        {
            T20Affliction4P = 242291
        };

        void OnModifyPower(Player * p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After)
        {
            if (!p_Player || p_Power != POWER_SOUL_SHARDS || p_OldValue <= p_NewValue || p_Regen || !p_After)
                return;

            if (!p_Player->HasAura(eSpells::T20Affliction4P))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T20Affliction4P);
            if (!l_SpellInfo)
                return;

            if (roll_chance_i(l_SpellInfo->ProcChance))
                p_Player->CastSpell(p_Player, l_SpellInfo->Effects[EFFECT_0].TriggerSpell, true);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Thal'kiel's Ascendance - 242832
class spell_warl_pet_thalkiels_ascendence : public SpellScriptLoader
{
public:
    spell_warl_pet_thalkiels_ascendence() : SpellScriptLoader("spell_warl_pet_thalkiels_ascendence") { }

    class spell_warl_pet_thalkiels_ascendence_SpellScript : public SpellScript
    {
        enum eSpells
        {
            DemonicEmpowerment  = 193396,
            LeftHandOfDarkness  = 238073,
            GrimoireOfSupremacy = 152107,
            GrimoireOfService   = 216187
        };

        enum eArtifactPowers
        {
            LeftHandOfDarknessArtifact = 1605
        };

        PrepareSpellScript(spell_warl_pet_thalkiels_ascendence_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
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

            int32 l_SpellPower = l_Owner->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
            float l_Modifier = GetSpellInfo()->Effects[EFFECT_0].BonusMultiplier;
            int32 l_Damage = l_SpellPower * l_Modifier;
            l_Damage = l_Owner->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, SpellEffIndex::EFFECT_0, SPELL_DIRECT_DAMAGE);
            l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

            if (l_Caster->GetEntry() == WarlockPet::FelGuard)
                AddPct(l_Damage, 10);

            if (AuraEffect* l_DemonicEffect = l_Caster->GetAuraEffect(eSpells::DemonicEmpowerment, EFFECT_3))
                l_Damage = AddPct(l_Damage, l_DemonicEffect->GetAmount());

            if (l_Caster->isPet() && l_Player->HasAura(eSpells::LeftHandOfDarkness))
            {
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LeftHandOfDarkness))
                {
                    uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::LeftHandOfDarknessArtifact);
                    float l_Amount = l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank;

                    if (l_Owner->HasAura(eSpells::GrimoireOfSupremacy))
                        l_Amount /= 2;

                    if (l_Amount > 0)
                        l_Damage = AddPct(l_Damage, l_Amount);
                }
            }

            if (SpellInfo const* l_GrimoireOfServiceInfo = sSpellMgr->GetSpellInfo(eSpells::GrimoireOfService))
                if (l_Caster->HasAura(eSpells::GrimoireOfService))
                    AddPct(l_Damage, l_GrimoireOfServiceInfo->Effects[EFFECT_0].BasePoints);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_warl_pet_thalkiels_ascendence_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warl_pet_thalkiels_ascendence_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Chaos Barrage - 187394 , Searing Bolt - 243050
class spell_warl_portals_damage_against_invise : public SpellScriptLoader
{
    public:
        spell_warl_portals_damage_against_invise() : SpellScriptLoader("spell_warl_portals_damage_against_invise") { }

        class spell_warl_portals_damage_against_invise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_portals_damage_against_invise_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Caster)
                    return;

                if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
                    SetHitDamage(0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_portals_damage_against_invise_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_portals_damage_against_invise_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Infernal Awakening 22703 , Immolation - 20153
class spell_warl_pet_infernal_damage : public SpellScriptLoader
{
public:
    spell_warl_pet_infernal_damage() : SpellScriptLoader("spell_warl_pet_infernal_damage") { }

    class spell_warl_pet_infernal_damage_SpellScript : public SpellScript
    {
        enum eSpells
        {
            DemonicEmpowerment      = 193396,
            LeftHandOfDarkness      = 238073,
            GrimoireOfSupremacy     = 152107,
            GrimoireOfSynergy       = 171982,
            AfflictionWarlock       = 137043,
            DestructionWarlock      = 137046,
            EmpoweredLifeTap        = 235156,
            TheExpendables          = 211218,
        };

        enum eArtifactPowers
        {
            LeftHandOfDarknessArtifact = 1605
        };

        PrepareSpellScript(spell_warl_pet_infernal_damage_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                l_Owner = l_Caster;

            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            Player* l_Player = l_Owner->ToPlayer();
            if (!l_Player)
                return;

            int32 l_SpellPower = l_Owner->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
            float l_Modifier = GetSpellInfo()->Effects[EFFECT_0].BonusMultiplier;
            int32 l_Damage = l_SpellPower * l_Modifier;
            l_Damage = l_Owner->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, SpellEffIndex::EFFECT_0, SPELL_DIRECT_DAMAGE);

            /// Grimoire of Synergy mustn't be applied
            if (l_Owner->HasAura(eSpells::GrimoireOfSynergy))
                l_Damage /= 1.25;

            /// Empowered life tap musn't be applied
            if (l_Owner->HasAura(eSpells::EmpoweredLifeTap))
                l_Damage /= 1.10;

            ///< Warlock's pets the expendables - 211218
            if (AuraEffect* l_TheExpendablesEffect = l_Caster->GetAuraEffect(eSpells::TheExpendables, EFFECT_0))
                AddPct(l_Damage, l_TheExpendablesEffect->GetAmount());

            l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

            if (AuraEffect* l_DemonicEffect = l_Caster->GetAuraEffect(eSpells::DemonicEmpowerment, EFFECT_3))
                l_Damage = AddPct(l_Damage, l_DemonicEffect->GetAmount());

            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AfflictionWarlock))
                if (l_Owner->HasAura(eSpells::AfflictionWarlock))
                    l_Damage = AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DestructionWarlock))
                if (l_Owner->HasAura(eSpells::DestructionWarlock))
                    l_Damage = AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

            if (l_Caster->isPet() && l_Player->HasAura(eSpells::LeftHandOfDarkness))
            {
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LeftHandOfDarkness))
                {
                    uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::LeftHandOfDarknessArtifact);
                    float l_Amount = l_SpellInfo->Effects[EFFECT_0].BasePoints * l_Rank;

                    if (l_Owner->HasAura(eSpells::GrimoireOfSupremacy))
                        l_Amount /= 2;

                    if (l_Amount > 0)
                        l_Damage = AddPct(l_Damage, l_Amount);
                }
            }

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_warl_pet_infernal_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_warl_pet_infernal_damage_SpellScript();
    }
};

/// Last Update 7.3.5 build 263635
/// Called by Immolate - 157736 , Drain Life - 234153
class spell_warl_amount_odr_shawl_of_the_Ymirjar: public SpellScriptLoader
{
    public:
        spell_warl_amount_odr_shawl_of_the_Ymirjar() : SpellScriptLoader("spell_warl_amount_odr_shawl_of_the_Ymirjar") { }

        class spell_warl_amount_odr_shawl_of_the_Ymirjar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_amount_odr_shawl_of_the_Ymirjar_AuraScript);

            enum eSpells
            {
                OdrShawlOfTheYmirjar     = 212172,
                OdrShawlOfTheYmirjarAura = 212173,
                DrainLife                = 234153
            };

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::OdrShawlOfTheYmirjar))
                    return;

                if (l_Target->GetGUID() != l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::HavocTarget))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::OdrShawlOfTheYmirjarAura);
                if (!l_SpellInfo)
                    return;

                AddPct(p_Amount, l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::DrainLife)
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_amount_odr_shawl_of_the_Ymirjar_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_LEECH);
                else
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_amount_odr_shawl_of_the_Ymirjar_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_amount_odr_shawl_of_the_Ymirjar_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Demonic Gateway - 113896
class spell_warl_demonic_gateway_with_cc : public SpellScriptLoader
{
    public:
        spell_warl_demonic_gateway_with_cc() : SpellScriptLoader("spell_warl_demonic_gateway_with_cc") { }

        class spell_warl_demonic_gateway_with_cc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_gateway_with_cc_AuraScript);

            std::vector<Mechanics> m_Mechanic =
            {
                MECHANIC_FEAR,
                MECHANIC_STUN,
                MECHANIC_DISORIENTED,
                MECHANIC_DISTRACT,
                MECHANIC_ROOT,
                MECHANIC_SLEEP,
                MECHANIC_FREEZE,
                MECHANIC_POLYMORPH,
                MECHANIC_BANISH,
                MECHANIC_SHACKLE,
                MECHANIC_HORROR,
                MECHANIC_DAZE,
                MECHANIC_SAPPED
            };

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (auto l_Mechanic : m_Mechanic)
                    l_Caster->ApplySpellImmune(GetSpellInfo()->Id, SpellImmunity::IMMUNITY_MECHANIC, l_Mechanic, false);
            }

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (auto l_Mechanic : m_Mechanic)
                    l_Caster->ApplySpellImmune(GetSpellInfo()->Id, SpellImmunity::IMMUNITY_MECHANIC, l_Mechanic, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_demonic_gateway_with_cc_AuraScript::HandleAfterRemove, EFFECT_4, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_warl_demonic_gateway_with_cc_AuraScript::HandleAfterApply, EFFECT_4, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_demonic_gateway_with_cc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Item - Warlock T21 Affliction 2P Bonus - 251847
class spell_warl_t21_afflication_2p_bonus : public SpellScriptLoader
{
public:
    spell_warl_t21_afflication_2p_bonus() : SpellScriptLoader("spell_warl_t21_afflication_2p_bonus") { }

    class spell_warl_t21_afflication_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_t21_afflication_2p_bonus_AuraScript);

        enum eSpells
        {
            Agony               = 980,
            UnstableAffliction1 = 233490,
            UnstableAffliction2 = 233496,
            UnstableAffliction3 = 233497,
            UnstableAffliction4 = 233498,
            UnstableAffliction5 = 233499
        };

        std::set<uint32> m_UnstableAfflictions = { eSpells::UnstableAffliction1, eSpells::UnstableAffliction2, eSpells::UnstableAffliction3, eSpells::UnstableAffliction4, eSpells::UnstableAffliction5 };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Agony)
                return false;

            return true;
        }

        void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_EventInfo.GetProcTarget();
            if (!l_Caster || !l_Target)
                return;

            for (uint32 l_UA : m_UnstableAfflictions)
            {
                if (Aura* l_Aura = l_Target->GetAura(l_UA, l_Caster->GetGUID()))
                    l_Aura->SetDuration(l_Aura->GetDuration() + p_AurEff->GetAmount());
            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warl_t21_afflication_2p_bonus_AuraScript::HandleOnCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_warl_t21_afflication_2p_bonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warl_t21_afflication_2p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Unstable Affliction 30108 - Seed of Corruption 27243
/// Called for Item - Warlock T21 Affliction 4P Bonus - 251849
class spell_warl_t21_afflication_4p_bonus : public SpellScriptLoader
{
    public:
        spell_warl_t21_afflication_4p_bonus() : SpellScriptLoader("spell_warl_t21_afflication_4p_bonus") { }

        class spell_warl_t21_afflication_4p_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_t21_afflication_4p_bonus_SpellScript);

            enum eSpells
            {
                TormentedAgony          = 256807,
                WarlockT21Affliction4P  = 251849
            };

            void HandleProcOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::WarlockT21Affliction4P))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::TormentedAgony, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_warl_t21_afflication_4p_bonus_SpellScript::HandleProcOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_t21_afflication_4p_bonus_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Tormented Agony - 256807
class spell_warl_tormented_agony : public SpellScriptLoader
{
    public:
        spell_warl_tormented_agony() : SpellScriptLoader("spell_warl_tormented_agony") { }

        class spell_warl_tormented_agony_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_tormented_agony_SpellScript);

            enum eSpells
            {
                Agony           = 980,
                TormentedAgony  = 252938
            };

            void FilterTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* l_Target) -> bool
                {
                    if (!l_Target || !l_Target->ToUnit() || !l_Target->ToUnit()->HasAura(eSpells::Agony, l_Caster->GetGUID()))
                        return true;

                    return false;
                });
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                    if (Unit* l_Target = GetHitUnit())
                        l_Caster->CastSpell(l_Target, eSpells::TormentedAgony, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_tormented_agony_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_tormented_agony_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_tormented_agony_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Item - Warlock T21 Destruction 2P Bonus - 251854
class spell_warl_t21_destruction_2p_bonus : public SpellScriptLoader
{
public:
    spell_warl_t21_destruction_2p_bonus() : SpellScriptLoader("spell_warl_t21_destruction_2p_bonus") { }

    class spell_warl_t21_destruction_2p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_t21_destruction_2p_bonus_AuraScript);

        enum eSpells
        {
            ChaosBolt = 116858
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ChaosBolt)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warl_t21_destruction_2p_bonus_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warl_t21_destruction_2p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Item - Warlock T21 Destruction 4P Bonus - 251855
class spell_warl_t21_destruction_4p_bonus : public SpellScriptLoader
{
public:
    spell_warl_t21_destruction_4p_bonus() : SpellScriptLoader("spell_warl_t21_destruction_4p_bonus") { }

    class spell_warl_t21_destruction_4p_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_t21_destruction_4p_bonus_AuraScript);

        enum eSpells
        {
            ChaosBolt = 116858,
            FlamesOfArgus = 253097
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ChaosBolt)
                return false;

            return true;
        }

        void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_EventInfo.GetProcTarget();
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_Target || !l_DamageInfo)
                return;

            int32 l_Damage = CalculatePct(l_DamageInfo->GetAmount(), p_AurEff->GetAmount()) / 2;
            l_Caster->CastCustomSpell(l_Target, eSpells::FlamesOfArgus, &l_Damage, NULL, NULL, true);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warl_t21_destruction_4p_bonus_AuraScript::HandleOnCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_warl_t21_destruction_4p_bonus_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warl_t21_destruction_4p_bonus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// 216695 - Tormented Souls
class spell_warl_tormented_souls : public AuraScript
{
    PrepareAuraScript(spell_warl_tormented_souls);

    enum eSpells
    {
        TormentedSouls = 216695,
        SoulOfTheDamned = 216463,
        SoulOfTheDamnedKill = 216721
    };

    bool m_NeedRemove = false;
    uint32 m_RemoveTimer = 0;

    void OnUpdate(uint32 p_Diff)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster)
            return;

        if (!l_Caster->isInCombat() && !m_NeedRemove)
        {
            m_NeedRemove = true;
            m_RemoveTimer = 1.5 * MINUTE * IN_MILLISECONDS;
        }
        else if (l_Caster->isInCombat() && m_NeedRemove)
            m_NeedRemove = false;

        if (!m_NeedRemove)
            return;

        if (m_RemoveTimer <= p_Diff)
        {
            m_RemoveTimer = IN_MILLISECONDS;

            std::list<AreaTrigger*> l_AreaTriggerList;
            l_Caster->GetAreaTriggerList(l_AreaTriggerList, eSpells::SoulOfTheDamned);
            l_Caster->GetAreaTriggerList(l_AreaTriggerList, eSpells::SoulOfTheDamnedKill);
            if (!l_AreaTriggerList.empty())
            {
                JadeCore::Containers::RandomResizeList(l_AreaTriggerList, 1);
                l_AreaTriggerList.front()->Remove();
            }

            GetAura()->DropStack();
        }
        else
            m_RemoveTimer -= p_Diff;
    }

    void Register() override
    {
        OnAuraUpdate += AuraUpdateFn(spell_warl_tormented_souls::OnUpdate);
    }
};

/// Last Update 7.3.5
/// Tormented Souls - 216695
class PlayerScript_Tormented_souls : public PlayerScript
{
    public:
        PlayerScript_Tormented_souls() :PlayerScript("PlayerScript_Tormented_souls") { }

        enum eSpells
        {
            TormentedSouls      = 216695,
            SoulOfTheDamned     = 216463,
            SoulOfTheDamnedKill = 216721
        };

        void DropTormentedSouls(Player* p_Player)
        {
            std::list<AreaTrigger*> l_AreaTriggerList;
            p_Player->GetAreaTriggerList(l_AreaTriggerList, eSpells::SoulOfTheDamned);
            p_Player->GetAreaTriggerList(l_AreaTriggerList, eSpells::SoulOfTheDamnedKill);
            for (auto l_AreaTrigger : l_AreaTriggerList)
                l_AreaTrigger->Remove();

            p_Player->RemoveAurasDueToSpell(eSpells::TormentedSouls);
        }

        void OnLogin(Player* p_Player) override
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TormentedSouls);
            if (!p_Player || p_Player->GetActiveSpecializationID() != SPEC_WARLOCK_AFFLICTION || !l_SpellInfo)
                return;

            DropTormentedSouls(p_Player);
        }

        void OnLogout(Player* p_Player) override
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TormentedSouls);
            if (!p_Player || p_Player->GetActiveSpecializationID() != SPEC_WARLOCK_AFFLICTION || !l_SpellInfo)
                return;

            DropTormentedSouls(p_Player);
        }

        void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 /*p_NewAreaID*/) override
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TormentedSouls);
            if (!p_Player || p_NewZoneID == p_OldZoneID || p_Player->GetActiveSpecializationID() != SPEC_WARLOCK_AFFLICTION || !l_SpellInfo)
                return;

            DropTormentedSouls(p_Player);
        }
};

/// Last Update 7.3.5
/// 111400 - Burning Rush
class spell_warl_burning_rush : public SpellScript
{
    PrepareSpellScript(spell_warl_burning_rush);

    SpellCastResult CheckHealth()
    {
        Unit* l_Caster = GetCaster();
        SpellInfo const* l_SpellInfo = GetSpellInfo();
        if (!l_Caster || !l_SpellInfo)
            return SPELL_FAILED_DONT_REPORT;

        int32 l_Damage = int32(l_Caster->CountPctFromMaxHealth(l_SpellInfo->Effects[EFFECT_1].BasePoints));
        if (l_Caster->GetHealth() < l_Damage)
            return SPELL_FAILED_DONT_REPORT;

        return SPELL_CAST_OK;
    }

    void Register()
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_burning_rush::CheckHealth);
    }
};

/// Last Update 7.3.5
/// Seduction - 6358
class spell_warl_seduction : public SpellScript
{
    PrepareSpellScript(spell_warl_seduction);

    std::vector<AuraType> m_DiseaseAuraTypes =
    {
        SPELL_AURA_PERIODIC_DAMAGE,
        SPELL_AURA_PERIODIC_SCHOOL_DAMAGE,
        SPELL_AURA_PERIODIC_DAMAGE_PERCENT,
        SPELL_AURA_PERIODIC_LEECH,
        SPELL_AURA_PERIODIC_HEALTH_FUNNEL
    };

    void HandleAfterHit()
    {
        Unit* l_Caster = GetCaster();
        Unit* l_Target = GetHitUnit();
        if (!l_Caster || !l_Target)
            return;

        l_Target->RemoveAurasWithMechanic((1LL << MECHANIC_BLEED) | (1LL << MECHANIC_INFECTED));

        for (auto l_AuraType : m_DiseaseAuraTypes)
            if (l_Target->HasAuraType(l_AuraType))
                l_Target->RemoveAurasByType(l_AuraType);

    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warl_seduction::HandleAfterHit);
    }
};

class spell_ritual_of_doom : public SpellScriptLoader
{
public:
    spell_ritual_of_doom() : SpellScriptLoader("spell_ritual_of_doom") {}

    class spell_ritual_of_doom_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ritual_of_doom_SpellScript);

        Position l_SummonPosition = { 3438.63f, 1536.32f, 436.85f, 5.51f };
        uint32 l_Demon = RAND(112387, 112381, 112388, 112379, 112377, 112385, 112396, 112399, 112378, 112389, 112395, 112398, 112384, 112376, 112394, 112391, 112375, 112374, 112380, 112397);

        void HandleDummy(SpellEffIndex Index)
        {
            Unit* l_Caster = GetCaster();

            if (!l_Caster)
                return;

            l_Caster->SummonCreature(l_Demon, l_SummonPosition, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 900000);
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(spell_ritual_of_doom_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_ritual_of_doom_SpellScript();
    }
};

#ifndef __clang_analyzer__
void AddSC_warlock_spell_scripts()
{
    new spell_warl_soul_link();
    new spell_warl_soul_harvest();
    new spell_warl_soul_harvest_targets();
    new spell_warl_deaths_embrace();
    new spell_warl_sacrolashs_dark_strike();
    new spell_warl_stretens_insanity();
    new spell_warl_call_fellhunter();
    new spell_warl_singe_magic();
    new spell_warl_harvester_of_souls();
    new spell_warl_implosion();
    new spell_warl_drain_fel_energy();
    new spell_warl_demonic_servitude();
    new spell_warl_cataclysm();
    new spell_warl_siphon_life();
    new spell_warl_siphon_life_heal();
    new spell_warl_haunt_dispel();
    new spell_warl_dark_regeneration();
    new spell_warl_sacrificial_pact();
    new spell_warl_soul_swap();
    new spell_warl_demonic_gateway();
    new spell_warl_fear();
    new spell_warl_banish();
    new spell_warl_soulshatter();
    new spell_warl_demonic_circle_summon();
    new spell_warl_demonic_circle_teleport();
    new spell_warl_WodPvPDemonology4PBonus();
    new spell_warl_fel_firebolt();
    new spell_warl_doom_bolt();
    new spell_warl_grimoire_of_synergy();
    new PlayerScript_WoDPvPDemonology2PBonus();
    new spell_warl_create_healthstone();
    new spell_warl_command_demon_spells();
    new spell_warl_cripple_doomguard();
    new spell_warl_t21_afflication_2p_bonus();
    new spell_warl_t21_afflication_4p_bonus();
    new spell_warl_tormented_agony();
    new spell_warl_t21_destruction_2p_bonus();
    new spell_warl_t21_destruction_4p_bonus();
    new PlayerScript_Tormented_souls();
    RegisterSpellScript(spell_warl_burning_rush);
    RegisterSpellScript(spell_warl_seduction);

    /// General
    new spell_warl_soul_leech();
    new spell_warl_soul_leech_pet();
    new PlayerScript_Soul_Leech();
    new spell_warl_demon_skin();
    new spell_warl_health_funnel();
    new spell_warl_life_tap();
    new spell_warl_grimoire_of_sacrifice();
    new PlayerScript_Grimoire_Of_Sacrifice();
    new spell_warl_grimoire_of_synergy_pvp();
    new spell_warl_reverse_entropy();
    new spell_warl_backdraft_pvp();
    new spell_warl_soul_of_the_netherlord();
    new PlayerScript_The_Master_Harvester();
    new spell_warl_enslave_demon();
    new spell_warl_pet_infernal_damage();
    new spell_warl_demonic_gateway_with_cc();
    RegisterAuraScript(spell_warl_tormented_souls);
    new spell_ritual_of_doom();

    /// Destruction
    new spell_warl_immolate_energize();
    new spell_warl_immolate();
    new spell_warl_immolate_marker();
    new spell_warl_channel_demonfire();
    new spell_warl_unaura_roaring_blaze();
    new spell_warl_roaring_blaze();
    new spell_warl_incinerate();
    new spell_warl_rain_of_fire();
    new spell_warl_havoc();
    new spell_warl_havoc_proc();
    new spell_warl_eradication();
    new spell_warl_dimensionnal_rift();
    new spell_warl_shadow_barrage();
    new spell_warl_chaos_barrage();
    new PlayerScript_Soul_Conduit();
    new spell_warl_shadowburn();
    new spell_warl_backdraft();
    new spell_warl_backdraft_consumption();
    new spell_warl_chaos_bolt();
    new spell_warl_lord_of_flames();
    new spell_warl_meteor_strike();
    new spell_warl_dimension_ripper();
    new spell_warl_soulsnatcher();
    new spell_warl_devourer_of_life();
    new spell_warl_conflagrate();
    new spell_warl_t19_destriction_2p_bonus();
    new spell_warl_feretory_of_souls_proc();
    new spell_warl_feretory_of_souls_by_rof();
    new spell_warl_demonic_power_proc();
    new spell_warl_channel_demonfire_damage();
    new spell_warl_portals_damage_against_invise();
    new spell_warl_amount_odr_shawl_of_the_Ymirjar();

    /// Affliction
    new spell_warl_agony();
    new spell_warl_seed_of_corruption();
    new spell_warl_seed_of_corruption_detonate();
    new spell_warl_unstable_affliction();
    new spell_warl_unstable_affliction_dispell();
    new spell_warl_phantom_singularity();
    new spell_warl_phantom_singularity_damage();
    new spell_warl_absolute_corruption();
    new spell_warl_reap_souls();
    new spell_warl_drain_soul();
    new spell_warl_shadow_bite();
    new PlayerScript_Wrath_Of_Consumption_Reap_Soul();
    new spell_warl_deadwind_harvester();
    new spell_warl_soul_effigy();
    new spell_warl_soul_flame_damages();
    new spell_warl_sweet_soul();
    new spell_warl_compounding_horror();
    new spell_warl_compounding_horror_damages();
    new spell_warl_compounding_horror_proc();
    new PlayerScript_Drain_Soul();
    new PlayerScript_Soul_Flame();
    new spell_warl_rend_soul();
    new spell_warl_corruption_damage();
    new spell_warl_nightfall();
    new PlayerScript_T20_Affliction_4P_bonus();

    /// Demonology
    new spell_warl_call_dreadstalkers();
    new spell_warl_demonwrath();
    new spell_warl_shadow_bolt();
    new spell_warl_doom();
    new spell_warl_hand_of_guldan();
    new spell_warl_hand_of_guldan_damage();
    new spell_warl_darkglare_eye_laser();
    new spell_warl_grimoire_of_service_learn();
    new spell_warl_grimoire_of_service();
    new spell_warl_demonbolt();
    new spell_warl_demonic_empowerment();
    new spell_warl_demonic_thal_kiel_consumption();
    new spell_warl_summon_felguard();
    new spell_warl_eye_of_the_observer();
    new spell_warl_sindorei_spite();
    new PlayerScript_Wakeners_Loyalty();
    new PlayerScript_The_Expendables();
    new spell_warl_stolen_power();
    new spell_warl_thalkiels_discord();
    new spell_warl_ui_timer_summon();
    new spell_warl_left_hand_of_darkness();
    new PlayerScript_Left_Hand_Of_Darkness();
    new spell_warl_left_hand_of_darkness_amount();
    new spell_warl_jaws_of_shadow_amount();
    new spell_warl_t20_demo_2p_bonus_proc();
    new spell_warl_demonic_calling();
    new spell_warl_pet_thalkiels_ascendence();

    /// Honor Talent
    new spell_warl_essence_drain();
    new spell_warl_cremation();
    new spell_warl_fel_fissure();
    new PlayerScript_Pleasure_Through_Pain();
    new spell_warl_pleasure_through_pain_removal();
    new spell_warl_rot_and_decay();
    new spell_warl_bane_of_havoc();
    new spell_warl_entrenched_in_flame();
    new spell_warl_soulshatter_pvp();
    new spell_warl_curse_of_shadows();
    new spell_warl_curse_of_fragility();
    new spell_warl_cremation_damage();

    /// Pet Abiltiies
    new spell_warl_pet_felbolt();
    new spell_warl_pet_tongue_lash();
    new spell_warl_dreadbite();
}
#endif
