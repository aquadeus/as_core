////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2017 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiersImpl.h"
#include "GridNotifiers.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "HelperDefines.h"

////////////////////////////////////////////////////////////////////////////////
/// Common
////////////////////////////////////////////////////////////////////////////////

/// Sephuz's Secret - 208051 - 236763 - 234867
/// Updated as of 7.1.5 23420
class spell_leg_sephuzs_secret : public SpellScriptLoader
{
public:
    spell_leg_sephuzs_secret() : SpellScriptLoader("spell_leg_sephuzs_secret") { }

    class spell_leg_sephuzs_secret_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_sephuzs_secret_AuraScript);

        enum eSpells
        {
            SephuzsSecretProc   = 208052,
            SephuzsSecretMarker = 226262
        };

        void SetData(uint32/* p_Type*/, uint32/* p_Data*/) override
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            if (l_Caster->HasAura(eSpells::SephuzsSecretMarker))
                return;

            l_Caster->CastSpell(l_Caster, eSpells::SephuzsSecretProc, true);
            l_Caster->CastSpell(l_Caster, eSpells::SephuzsSecretMarker, true);
        }

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            return false;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_leg_sephuzs_secret_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_sephuzs_secret_AuraScript();
    }
};

/// Aggramar's Stride - 207438
/// Last Update - 7.3.5 26365
class spell_leg_aggramars_stride : public SpellScriptLoader
{
    public:
        spell_leg_aggramars_stride() : SpellScriptLoader("spell_leg_aggramars_stride") { }

        class spell_leg_aggramars_stride_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_aggramars_stride_AuraScript);

            enum eSpells
            {
                AggramarsStrideEffect = 226974
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                int32 l_Amount = GetBuffAmount(l_Player);

                l_Player->CastCustomSpell(l_Player, eSpells::AggramarsStrideEffect, &l_Amount, nullptr, nullptr, true);
            }

            void HandleUpdate(uint32 p_Diff)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                int32 l_Amount = GetBuffAmount(l_Player);

                if (AuraEffect* l_Aura = l_Player->GetAuraEffect(eSpells::AggramarsStrideEffect, EFFECT_0))
                {
                    if (l_Aura->GetAmount() != l_Amount)
                        l_Aura->ChangeAmount(l_Amount);
                }
                else
                    l_Player->CastCustomSpell(l_Player, eSpells::AggramarsStrideEffect, &l_Amount, nullptr, nullptr, true);
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                l_Player->RemoveAura(eSpells::AggramarsStrideEffect);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_aggramars_stride_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnAuraUpdate += AuraUpdateFn(spell_leg_aggramars_stride_AuraScript::HandleUpdate);
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_aggramars_stride_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            int32 GetBuffAmount(Player* p_Player)
            {
                float l_HastePct = p_Player->GetHastePct();
                float l_CritPct = p_Player->GetFloatValue(PLAYER_FIELD_CRIT_PERCENTAGE);
                float l_MasterPct = p_Player->GetFloatValue(PLAYER_FIELD_MASTERY);
                float l_VersaPct = p_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY);
                float l_Pct = GetSpellInfo()->Effects[EFFECT_0].BasePoints;

                int32 l_Amount = CalculatePct(std::max({ l_HastePct, l_CritPct, l_MasterPct, l_VersaPct }), l_Pct);
                return l_Amount;
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_aggramars_stride_AuraScript();
        }
};

/// Xavaric's Magnum Opus - 207428
/// Updated as of 7.0.3 22522
class spell_leg_xavarics_magnum_opus : public SpellScriptLoader
{
    public:
        spell_leg_xavarics_magnum_opus() : SpellScriptLoader("spell_leg_xavarics_magnum_opus") { }

        class spell_leg_xavarics_magnum_opus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_xavarics_magnum_opus_AuraScript);

            enum eSpells
            {
                XavaricsMagnumOpusAuraProc  = 207446,
                XavaricsMagnumOpusMarker    = 224476
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::XavaricsMagnumOpusAuraProc, true);
            }

            void HandleUpdate(uint32 p_Diff)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::XavaricsMagnumOpusMarker) && !l_Caster->HasAura(eSpells::XavaricsMagnumOpusAuraProc))
                    l_Caster->CastSpell(l_Caster, eSpells::XavaricsMagnumOpusAuraProc, true);
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAura(eSpells::XavaricsMagnumOpusAuraProc);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_xavarics_magnum_opus_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnAuraUpdate += AuraUpdateFn(spell_leg_xavarics_magnum_opus_AuraScript::HandleUpdate);
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_xavarics_magnum_opus_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_xavarics_magnum_opus_AuraScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Called by Xavaric's Magnum Opus - 207472
class spell_leg_xavarics_magnum_opus_shield : public SpellScriptLoader
{
    public:
        spell_leg_xavarics_magnum_opus_shield() : SpellScriptLoader("spell_leg_xavarics_magnum_opus_shield") { }

        class spell_leg_xavarics_magnum_opus_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_xavarics_magnum_opus_shield_AuraScript);

            enum eSpells
            {
                XavaricsMagnumOpusShield = 207472
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::XavaricsMagnumOpusShield);
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetRoleBySpecializationId(l_Player->GetActiveSpecializationID()) == Roles::ROLE_TANK)
                    p_Amount = l_Caster->CountPctFromMaxHealth(15); ///< 15% for tank specialization
                else
                    p_Amount = l_Caster->CountPctFromMaxHealth(l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_leg_xavarics_magnum_opus_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_leg_xavarics_magnum_opus_shield_AuraScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Called by Archimonde's Hatred Reborn - 235169
class spell_leg_archimondes_hatred_reborn : public SpellScriptLoader
{
    public:
        spell_leg_archimondes_hatred_reborn() : SpellScriptLoader("spell_leg_archimondes_hatred_reborn") { }

        class spell_leg_archimondes_hatred_reborn_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_archimondes_hatred_reborn_AuraScript);

            uint32 m_Absorb = 0;

            enum eSpells
            {
                ArchimondesHatredReborn         = 235169,
                ArchimondesHatredRebornDamages  = 235188
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ArchimondesHatredReborn);
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                p_Amount = l_Caster->CountPctFromMaxHealth(l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& /*p_AbsorbAmount*/)
            {
                m_Absorb += p_DmgInfo.GetAmount();
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ArchimondesHatredReborn);
                SpellInfo const* l_SpellInfoDamages = sSpellMgr->GetSpellInfo(eSpells::ArchimondesHatredRebornDamages);
                if (!l_Caster || !l_SpellInfo || !l_SpellInfoDamages || !m_Absorb)
                    return;

                int32 l_Damages;
                std::list<Unit*> l_TargetList;
                SpellRadiusEntry const* l_RadiusEntry = l_SpellInfoDamages->Effects[EFFECT_0].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_RadiusEntry->RadiusMax);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                l_Caster->VisitNearbyObject(l_RadiusEntry->RadiusMax, l_Searcher);

                l_TargetList.remove_if([=](Unit* l_TargetInList) -> bool
                {
                    return (!l_TargetInList || l_TargetInList == l_Caster);
                });

                if (l_TargetList.empty())
                    return;

                l_Damages = CalculatePct(m_Absorb, l_SpellInfo->Effects[EFFECT_1].BasePoints) / l_TargetList.size();
                l_Caster->CastCustomSpell(l_Caster, eSpells::ArchimondesHatredRebornDamages, &l_Damages, nullptr, nullptr, true);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_leg_archimondes_hatred_reborn_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_leg_archimondes_hatred_reborn_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_archimondes_hatred_reborn_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_leg_archimondes_hatred_reborn_AuraScript();
        }
};

/// Last Update: 7.1.5
/// Called by Norgannon's Foresight (item Aura) - 236373
class spell_leg_norgannons_foresight : public SpellScriptLoader
{
    public:
        spell_leg_norgannons_foresight() : SpellScriptLoader("spell_leg_norgannons_foresight") { }

        class spell_leg_norgannons_foresight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_norgannons_foresight_AuraScript);

            enum eSpells
            {
                NorgannonsForesightMoving           = 236374,
                NorgannonsForesightNotMoving        = 236375,
                NorgannonsForesightRecentlyMoved    = 234797,
                NorgannonsForesightMage             = 236380,
                NorgannonsForesightPriest           = 236430,
                NorgannonsForesightWarlock          = 236431
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->IsMoving())
                    l_Caster->CastSpell(l_Caster, eSpells::NorgannonsForesightMoving, true);
                else
                    l_Caster->CastSpell(l_Caster, eSpells::NorgannonsForesightNotMoving, true);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::NorgannonsForesightMoving);
                l_Caster->RemoveAura(eSpells::NorgannonsForesightNotMoving);
                l_Caster->RemoveAura(eSpells::NorgannonsForesightRecentlyMoved);

                switch (l_Caster->getClass())
                {
                    case CLASS_MAGE:
                        l_Caster->RemoveAura(eSpells::NorgannonsForesightMage);
                        break;
                    case CLASS_PRIEST:
                        l_Caster->RemoveAura(eSpells::NorgannonsForesightPriest);
                        break;
                    case CLASS_WARLOCK:
                        l_Caster->RemoveAura(eSpells::NorgannonsForesightWarlock);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_norgannons_foresight_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_leg_norgannons_foresight_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
            bool m_LastMovingStatus = false;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_norgannons_foresight_AuraScript();
        }
};

/// Last Update: 7.1.5
/// Called by Norgannon's Foresight (moving) - 236374
class spell_leg_norgannons_foresight_moving : public SpellScriptLoader
{
    public:
        spell_leg_norgannons_foresight_moving() : SpellScriptLoader("spell_leg_norgannons_foresight_moving") { }

        class spell_leg_norgannons_foresight_moving_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_norgannons_foresight_moving_AuraScript);

            enum eSpells
            {
                NorgannonsForesightAura             = 236373,
                NorgannonsForesightMoving           = 236374,
                NorgannonsForesightNotMoving        = 236375,
                NorgannonsForesightRecentlyMoved    = 234797,
                NorgannonsForesightMage             = 236380,
                NorgannonsForesightPriest           = 236430,
                NorgannonsForesightWarlock          = 236431
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::NorgannonsForesightAura))
                    return;

                l_Caster->RemoveAura(eSpells::NorgannonsForesightRecentlyMoved);
                l_Caster->RemoveAura(eSpells::NorgannonsForesightNotMoving);

                uint32 l_SpellId = GetSpellIdForClass(l_Caster);
                Aura* l_Aura = l_Caster->GetAura(l_SpellId);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_SpellId);
                if (l_Aura && l_Aura->GetDuration() == -1 && l_SpellInfo)
                {
                    int32 l_Duration = l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints;
                    l_Aura->SetMaxDuration(l_Duration);
                    l_Aura->SetDuration(l_Duration);
                }
            }

            void HandleOnEffectPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->IsMoving())
                    l_Caster->RemoveAura(eSpells::NorgannonsForesightMoving);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::NorgannonsForesightAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::NorgannonsForesightNotMoving, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_norgannons_foresight_moving_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_leg_norgannons_foresight_moving_AuraScript::HandleOnEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectApplyFn(spell_leg_norgannons_foresight_moving_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            uint32 GetSpellIdForClass(Unit* p_Caster)
            {
                switch (p_Caster->getClass())
                {
                    case CLASS_MAGE:
                        return eSpells::NorgannonsForesightMage;
                    case CLASS_PRIEST:
                        return eSpells::NorgannonsForesightPriest;
                    case CLASS_WARLOCK:
                        return eSpells::NorgannonsForesightWarlock;
                    default:
                        break;
                }

                return 0;
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_norgannons_foresight_moving_AuraScript();
        }
};

/// Last Update: 7.1.5
/// Called by Norgannon's Foresight (not moving) - 236375
class spell_leg_norgannons_foresight_not_moving : public SpellScriptLoader
{
    public:
        spell_leg_norgannons_foresight_not_moving() : SpellScriptLoader("spell_leg_norgannons_foresight_not_moving") { }

        class spell_leg_norgannons_foresight_not_moving_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_norgannons_foresight_not_moving_AuraScript);

            enum eSpells
            {
                NorgannonsForesightAura             = 236373,
                NorgannonsForesightRecentlyMoved    = 234797,
                NorgannonsForesightMoving           = 236374
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::NorgannonsForesightAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::NorgannonsForesightRecentlyMoved, true);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::NorgannonsForesightAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::NorgannonsForesightMoving, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_norgannons_foresight_not_moving_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_leg_norgannons_foresight_not_moving_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_norgannons_foresight_not_moving_AuraScript();
        }
};

/// Last Update: 7.1.5
/// Called by Norgannon's Foresight (recently moved) - 234797
class spell_leg_norgannons_foresight_recently_moved : public SpellScriptLoader
{
    public:
        spell_leg_norgannons_foresight_recently_moved() : SpellScriptLoader("spell_leg_norgannons_foresight_recently_moved") { }

        class spell_leg_norgannons_foresight_recently_moved_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_norgannons_foresight_recently_moved_AuraScript);

            enum eSpells
            {
                NorgannonsForesightAura             = 236373,
                NorgannonsForesightRecentlyMoved    = 234797,
                NorgannonsForesightMage             = 236380,
                NorgannonsForesightPriest           = 236430,
                NorgannonsForesightWarlock          = 236431
            };

            void HandleOnEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::NorgannonsForesightAura))
                    return;

                Aura* l_Aura = p_AurEff->GetBase();
                if (!l_Aura || p_AurEff->GetBase()->GetDuration() != 0)
                    return;

                if (uint32 l_SpellId = GetSpellIdForClass(l_Caster))
                {
                    l_Caster->CastSpell(l_Caster, l_SpellId, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_norgannons_foresight_recently_moved_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:

            uint32 GetSpellIdForClass(Unit* p_Caster)
            {
                switch (p_Caster->getClass())
                {
                    case CLASS_MAGE:
                        return eSpells::NorgannonsForesightMage;
                    case CLASS_PRIEST:
                        return eSpells::NorgannonsForesightPriest;
                    case CLASS_WARLOCK:
                        return eSpells::NorgannonsForesightWarlock;
                    default:
                        break;
                }

                return 0;
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_norgannons_foresight_recently_moved_AuraScript();
        }
};

////////////////////////////////////////////////////////////////////////////////
/// Shaman
////////////////////////////////////////////////////////////////////////////////

/// Eye of the Twisting Nether - 207994
/// Updated as of 7.0.3 22522
class spell_leg_eye_of_the_twisting_nether : public SpellScriptLoader
{
    public:
        spell_leg_eye_of_the_twisting_nether() : SpellScriptLoader("spell_leg_eye_of_the_twisting_nether") { }

        class spell_leg_eye_of_the_twisting_nether_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_eye_of_the_twisting_nether_AuraScript);

            enum eSpells
            {
                FireOfTheTwistingNether     = 207995,
                ChillOfTheTwistingNether    = 207998,
                ShockOfTheTwistingNether    = 207999
            };

            void HandleOnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (p_ProcEventInfo.GetDamageInfo() == nullptr)
                    return;

                SpellInfo const* l_ProcSpell = p_ProcEventInfo.GetDamageInfo()->GetSpellInfo();
                if (l_ProcSpell == nullptr)
                    return;

                std::map<SpellSchoolMask, uint32> l_SpellIDs =
                {
                    { SpellSchoolMask::SPELL_SCHOOL_MASK_FIRE, eSpells::FireOfTheTwistingNether },
                    { SpellSchoolMask::SPELL_SCHOOL_MASK_FROST, eSpells::ChillOfTheTwistingNether },
                    { SpellSchoolMask::SPELL_SCHOOL_MASK_NATURE, eSpells::ShockOfTheTwistingNether }
                };

                for (auto l_SpellPair : l_SpellIDs)
                {
                    if (l_SpellPair.first & l_ProcSpell->GetSchoolMask())
                        l_Caster->CastSpell(l_Caster, l_SpellPair.second, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_leg_eye_of_the_twisting_nether_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_eye_of_the_twisting_nether_AuraScript();
        }
};

/// Velen's Future Sight - 235966
/// Updated as of 7.3.5 build 26124
class spell_leg_velens_future_sight : public SpellScriptLoader
{
    public:
        spell_leg_velens_future_sight() : SpellScriptLoader("spell_leg_velens_future_sight") { }

        class spell_leg_velens_future_sight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_velens_future_sight_AuraScript);

            enum eSpells
            {
                VelensFutureSightHeal = 235967
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::VelensFutureSight) = 0;
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_HealAmount = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::VelensFutureSight);

                l_HealAmount = CalculatePct(l_HealAmount, l_SpellInfo->Effects[EFFECT_3].BasePoints);

                if (l_HealAmount > 0)
                {
                    l_Caster->CastCustomSpell(l_Caster, eSpells::VelensFutureSightHeal, &l_HealAmount, nullptr, nullptr, true);
                    l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::VelensFutureSight) = 0;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_velens_future_sight_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_leg_velens_future_sight_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_velens_future_sight_AuraScript();
        }
};

/// Velen's Future Sight - 235967
/// Updated as of 7.3.5 build 26124
class spell_leg_velens_future_sight_heal : public SpellScriptLoader
{
    public:
        spell_leg_velens_future_sight_heal() : SpellScriptLoader("spell_leg_velens_future_sight_heal") { }

        class spell_leg_velens_future_sight_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_velens_future_sight_heal_SpellScript);

            enum eSpells
            {
                VelensFutureSight = 235966
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VelensFutureSight);
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPred());
                if (p_Targets.empty())
                    return;

                if (p_Targets.size() > l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    p_Targets.resize(l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_leg_velens_future_sight_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_velens_future_sight_heal_SpellScript();
        }
};

/// Called by Riptide - 61295
/// Intact Nazjatar Molting - 208207
/// Updated as of 7.1.5 23420
class spell_leg_intact_nazjatar_molting : public SpellScriptLoader
{
    public:
        spell_leg_intact_nazjatar_molting() : SpellScriptLoader("spell_leg_intact_nazjatar_molting") { }

        class spell_leg_intact_nazjatar_molting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_intact_nazjatar_molting_SpellScript);

            enum eSpell
            {
                IntactNazjatarMoltingAura = 208207
            };

            bool Load() override
            {
                m_IsActivated = false;

                return true;
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpell::IntactNazjatarMoltingAura))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::IntactNazjatarMoltingAura);
                if (l_SpellInfo == nullptr)
                    return;

                if (l_Target->GetHealthPct() < l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    m_IsActivated = true;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_IsActivated && l_Caster->IsPlayer() && GetSpellInfo()->ChargeCategoryEntry != nullptr)
                    l_Caster->ToPlayer()->RestoreCharge(GetSpellInfo()->ChargeCategoryEntry);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_leg_intact_nazjatar_molting_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_leg_intact_nazjatar_molting_SpellScript::HandleAfterCast);
            }

        private:

            bool m_IsActivated;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_intact_nazjatar_molting_SpellScript;
        }
};

/// Roots of Shaladrassil - 233665
/// Updated as of 7.1.5 23420
class spell_leg_roots_of_shaladrassil : public SpellScriptLoader
{
    public:
        spell_leg_roots_of_shaladrassil() : SpellScriptLoader("spell_leg_roots_of_shaladrassil") { }

        enum eSpell
        {
            RootsOfShaladrassilDummy = 208980
        };

        class spell_leg_roots_of_shaladrassil_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_roots_of_shaladrassil_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Target->IsMoving())
                        return;

                    l_Target->CastSpell(l_Target, eSpell::RootsOfShaladrassilDummy, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_leg_roots_of_shaladrassil_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_roots_of_shaladrassil_AuraScript();
        }
};

/// Roots of Shaladrassil (Dummy) - 208980
/// Updated as of 7.1.5 23420
class spell_leg_roots_of_shaladrassil_dummy : public SpellScriptLoader
{
    public:
        spell_leg_roots_of_shaladrassil_dummy() : SpellScriptLoader("spell_leg_roots_of_shaladrassil_dummy") { }

        class spell_leg_roots_of_shaladrassil_dummy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_roots_of_shaladrassil_dummy_AuraScript);

            enum eSpell
            {
                ShaladrassilsNourishment = 208981
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, eSpell::ShaladrassilsNourishment, true);
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAura(eSpell::ShaladrassilsNourishment);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_roots_of_shaladrassil_dummy_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_roots_of_shaladrassil_dummy_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_roots_of_shaladrassil_dummy_AuraScript();
        }
};

/// Called by Chain Lightning - 188443 and Chain Lightning Overload - 45297
/// Al'Akir's Acrimony - 208699
/// Updated as of 7.1.5 23420
class spell_leg_alakirs_acrimony : public SpellScriptLoader
{
    public:
        spell_leg_alakirs_acrimony() : SpellScriptLoader("spell_leg_alakirs_acrimony") { }

        class spell_leg_alakirs_acrimony_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_alakirs_acrimony_SpellScript);

            uint32 m_TargetCount = 0;

            enum eSpell
            {
                AlakirsAcrimony = 208699
            };

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpell::AlakirsAcrimony))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::AlakirsAcrimony);
                if (l_SpellInfo == nullptr)
                    return;

                if (m_TargetCount >= 1)
                {
                    int32 l_Damage = GetHitDamage();

                    for (uint8 l_I = 0; l_I < m_TargetCount; ++l_I)
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                    SetHitDamage(l_Damage);
                }

                ++m_TargetCount;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_leg_alakirs_acrimony_SpellScript::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_alakirs_acrimony_SpellScript;
        }
};

/// Called by Lava Lash - 60103
/// Akainu's Absolute Justice - 213359
/// Updated as of 7.1.5 23420
class spell_leg_akainus_absolute_justice : public SpellScriptLoader
{
    public:
        spell_leg_akainus_absolute_justice() : SpellScriptLoader("spell_leg_akainus_absolute_justice") { }

        class spell_leg_akainus_absolute_justice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_akainus_absolute_justice_SpellScript);

            enum eSpells
            {
                AkainusAbsoluteJustice  = 213359,

                Flametongue             = 194084,
                Frostbrand              = 196834
            };

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::AkainusAbsoluteJustice))
                    return;

                if (!l_Caster->HasAura(eSpells::Flametongue) || !l_Caster->HasAura(eSpells::Frostbrand))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::AkainusAbsoluteJustice);
                if (l_SpellInfo == nullptr)
                    return;

                int32 l_Damage = GetHitDamage();

                AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_leg_akainus_absolute_justice_SpellScript::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_akainus_absolute_justice_SpellScript;
        }
};

/// Last Update: 7.3.5 Build 26365
/// Uncertain Reminder - 234814
/// Called by 178207, 90355, 160452, 80353, 2825, 32182
/// Called by Sense of Urgency - 208416
class spell_leg_uncertain_reminder : public SpellScriptLoader
{
    public:
        spell_leg_uncertain_reminder() : SpellScriptLoader("spell_leg_uncertain_reminder") { }

        enum eDataType
        {
            AuraGuidHigh    = 1,
            AuraGuidLow     = 2,
            DurationUsed    = 3
        };

        class spell_leg_uncertain_reminder_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_uncertain_reminder_AuraScript);

            enum eSpells
            {
                UncertainReminder   = 234814,
                SenseOfUrgencyBuff  = 208416
            };

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Owner = GetUnitOwner();
                AuraEffect* l_ReminderEffect = l_Owner->GetAuraEffect(eSpells::UncertainReminder, SpellEffIndex::EFFECT_0);
                if (l_Owner == nullptr || l_ReminderEffect == nullptr)
                    return;

                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                int32 l_Duration = l_Aura->GetDuration();
                int32 l_NewDuration = l_Duration + CalculatePct(l_Duration, l_ReminderEffect->GetAmount());
                l_Aura->SetMaxDuration(l_NewDuration);
                l_Aura->SetDuration(l_NewDuration);

                /// Mark the aura used
                /// Cannot use Guid128To64, split the guid
                l_ReminderEffect->GetBase()->SetScriptGuid(eDataType::AuraGuidHigh, l_Aura->GetCastGUID().GetHi());
                l_ReminderEffect->GetBase()->SetScriptGuid(eDataType::AuraGuidLow, l_Aura->GetCastGUID().GetLow());
                l_ReminderEffect->GetBase()->SetScriptData(eDataType::DurationUsed, l_NewDuration - l_Duration);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::SenseOfUrgencyBuff)
                    AfterEffectApply += AuraEffectApplyFn(spell_leg_uncertain_reminder_AuraScript::HandleApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_HEALING_DONE_PERCENT, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                else
                    AfterEffectApply += AuraEffectApplyFn(spell_leg_uncertain_reminder_AuraScript::HandleApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MELEE_SLOW, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_uncertain_reminder_AuraScript();
        }
};

/// Last Update: 7.3.5 Build 26365
/// Called by Uncertain Reminder - 234814
class spell_leg_uncertain_reminder_aura : public SpellScriptLoader
{
    public:
        spell_leg_uncertain_reminder_aura() : SpellScriptLoader("spell_leg_uncertain_reminder_aura") { }

        class spell_leg_uncertain_reminder_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_uncertain_reminder_aura_AuraScript);

            enum eSpells
            {
                DrumsOfFury     = 178207,
                AncientHysteria = 90355,
                Netherwinds     = 160452,
                TimeWarp        = 80353,
                Bloodlust       = 2825,
                Heroism         = 32182,
                SenseOfUrgency  = 208416
            };

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                /// It's all need to remove the bonus duration if the item is removed
                constexpr uint32 l_SpellIds[] = { eSpells::DrumsOfFury, eSpells::AncientHysteria, eSpells::Netherwinds, eSpells::TimeWarp, eSpells::Bloodlust, eSpells::Heroism, eSpells::SenseOfUrgency };

                Guid128 l_LastAuraGuid = Guid128(m_LastAuraGuidLow, m_LastAuraGuidHigh);

                for (auto l_SpellId : l_SpellIds)
                {
                    Aura* l_Aura = l_Owner->GetAura(l_SpellId);
                    if (l_Aura == nullptr || l_Aura->GetCastGUID() != l_LastAuraGuid)
                        continue;

                    int32 l_Duration = l_Aura->GetDuration();
                    if (l_Duration > m_LastDurationUsed)
                    {
                        l_Aura->SetDuration(l_Duration - m_LastDurationUsed);
                        l_Aura->SetMaxDuration(l_Aura->GetMaxDuration() - m_LastDurationUsed);
                    }
                    else
                    {
                        l_Aura->Remove(AuraRemoveMode::AURA_REMOVE_BY_EXPIRE);
                    }
                }
            }

            void SetGuid(uint32 p_Type, uint64 p_Guid) override
            {
                if (p_Type == spell_leg_uncertain_reminder::eDataType::AuraGuidHigh)
                {
                    m_LastAuraGuidHigh = p_Guid;
                }
                else if (p_Type == spell_leg_uncertain_reminder::eDataType::AuraGuidLow)
                {
                    m_LastAuraGuidLow = p_Guid;
                }
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                if (p_Type == spell_leg_uncertain_reminder::eDataType::DurationUsed)
                {
                    m_LastDurationUsed = p_Data;
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_uncertain_reminder_aura_AuraScript::HandleRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }

        private:

            uint64 m_LastAuraGuidHigh;
            uint64 m_LastAuraGuidLow;
            int32 m_LastDurationUsed = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_uncertain_reminder_aura_AuraScript();
        }
};

////////////////////////////////////////////////////////////////////////////////
/// Death Knight
////////////////////////////////////////////////////////////////////////////////

/// Last Update 7.1.5
/// Koltira's Newfound Will - 208782
class spell_leg_newfound_will : public SpellScriptLoader
{
    public:
        spell_leg_newfound_will() : SpellScriptLoader("spell_leg_newfound_will") { }

        class spell_leg_newfound_will_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_newfound_will_AuraScript);

            enum eSpells
            {
                Obliterate          = 49020
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

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_leg_newfound_will_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_newfound_will_AuraScript();
        }
};

/// Last Update 7.1.5
/// Draugr, Girdle of the Everlasting King - 208161
class spell_leg_girdle_of_the_everlasting_king : public SpellScriptLoader
{
    public:
        spell_leg_girdle_of_the_everlasting_king() : SpellScriptLoader("spell_leg_girdle_of_the_everlasting_king") { }

        class spell_leg_girdle_of_the_everlasting_king_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_girdle_of_the_everlasting_king_AuraScript);

            enum eSpells
            {
                FesteringStrike          = 85948
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::FesteringStrike)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_leg_girdle_of_the_everlasting_king_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_girdle_of_the_everlasting_king_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Toravon's Whiteout Bindings - 205658
/// Called by Pillar of Frost - 51271
class spell_leg_toravons_whiteout_bindings : public SpellScriptLoader
{
    public:
        spell_leg_toravons_whiteout_bindings() : SpellScriptLoader("spell_leg_toravons_whiteout_bindings") { }

        class spell_leg_toravons_whiteout_bindings_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_toravons_whiteout_bindings_AuraScript);

            enum eSpells
            {
                ToravonsWhiteoutBindings        = 205658,
                ToravonsWhiteoutBindingsBuff    = 205659
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::ToravonsWhiteoutBindings))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::ToravonsWhiteoutBindingsBuff, true);
                }
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Owner = GetUnitOwner();
                if (l_Owner == nullptr)
                    return;

                l_Owner->RemoveAura(eSpells::ToravonsWhiteoutBindingsBuff);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_toravons_whiteout_bindings_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_toravons_whiteout_bindings_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_toravons_whiteout_bindings_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Ovyd's Winter Wrap - 217647
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
                if (!l_Caster || p_Targets.empty())
                    return;

                if (GetExplTargetUnit())
                    p_Targets.remove(GetExplTargetUnit());

                if (p_Targets.size() > 2)
                    JadeCore::RandomResizeList(p_Targets, 2);
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::OvydsWinterWrapProc, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_ovyds_winter_wrap_SpellScript::HandleSelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_monk_ovyds_winter_wrap_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_ovyds_winter_wrap_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Called By Eye Beam - 198030
class spell_leg_raddons_cascading_eyes : public SpellScriptLoader
{
    public:
        spell_leg_raddons_cascading_eyes() : SpellScriptLoader("spell_leg_raddons_cascading_eyes") { }

        enum eSpells
        {
            EyeBeam                 = 198013,
            RaddonsCascadingEyes    = 215149
        };

        class spell_leg_raddons_cascading_eyes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_raddons_cascading_eyes_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::RaddonsCascadingEyes);

                if (!l_Caster->HasAura(eSpells::RaddonsCascadingEyes))
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (l_Player->HasSpellCooldown(eSpells::EyeBeam))
                        l_Player->ReduceSpellCooldown(eSpells::EyeBeam, l_SpellInfo->Effects[0].BasePoints);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_leg_raddons_cascading_eyes_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_raddons_cascading_eyes_SpellScript();
        }
};

////////////////////////////////////////////////////////////////////////////////
/// Warlock
////////////////////////////////////////////////////////////////////////////////

/// Last Update 7.1.5 Build 23420
/// Called by Call Dreadstalkers - 193331, 193332, 196273, 196274, Summon Darkglare - 205180, Hand of Gul'dan - 104317
/// Called for Wilfred's Sigil of Superior Summoning - 214345
class spell_leg_wilfreds_sigil_of_superior_summoning : public SpellScriptLoader
{
    public:
        spell_leg_wilfreds_sigil_of_superior_summoning() : SpellScriptLoader("spell_leg_wilfreds_sigil_of_superior_summoning") { }

        class spell_leg_wilfreds_sigil_of_superior_summoning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_wilfreds_sigil_of_superior_summoning_SpellScript);

            enum eSpells
            {
                SummonDoomguard                     = 18540,
                SummonInfernal                      = 1122,
                WilfredsSigilOfSuperiorSummoning    = 214345
            };

            std::vector<uint32> Spells =
            {
                eSpells::SummonDoomguard,
                eSpells::SummonInfernal
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WilfredsSigilOfSuperiorSummoning);
                if (!l_Caster || !l_Caster->HasAura(eSpells::WilfredsSigilOfSuperiorSummoning) || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                for (uint32 l_Spell : Spells)
                    l_Player->ReduceSpellCooldown(l_Spell, l_SpellInfo->Effects[EFFECT_0].BasePoints * -1);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_leg_wilfreds_sigil_of_superior_summoning_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_wilfreds_sigil_of_superior_summoning_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Rain of Fire 5740 and Channel Demonfire 196447
/// Called for Feretory of Souls - Item 132456 / Spell 205702
class spell_leg_warlock_feretory_of_souls : public SpellScriptLoader
{
public:
    spell_leg_warlock_feretory_of_souls() : SpellScriptLoader("spell_leg_warlock_feretory_of_souls") { }

    class spell_leg_warlock_feretory_of_souls_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_leg_warlock_feretory_of_souls_SpellScript);

        enum eSpells
        {
            FeretoryAura    = 205702,
            FeretoryProc    = 205704
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_FeretoryOfSoulsInfo = sSpellMgr->GetSpellInfo(eSpells::FeretoryAura);
            if (!l_Caster || !l_FeretoryOfSoulsInfo || !l_Caster->HasAura(eSpells::FeretoryAura))
                return;

            uint32 l_ProcChance = l_FeretoryOfSoulsInfo->ProcChance;
            if (roll_chance_i(l_ProcChance))
                l_Caster->CastSpell(l_Caster, eSpells::FeretoryProc, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_leg_warlock_feretory_of_souls_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_leg_warlock_feretory_of_souls_SpellScript();
    }
};

////////////////////////////////////////////////////////////////////////////////
/// Paladin
////////////////////////////////////////////////////////////////////////////////

/// Last Update: 7.1.5 Build 23420
/// Called by Chain of Thrayn - 206338
class spell_pal_chain_of_thrayn : public SpellScriptLoader
{
    public:
        spell_pal_chain_of_thrayn() : SpellScriptLoader("spell_pal_chain_of_thrayn") { }

        class spell_pal_chain_of_thrayn_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_chain_of_thrayn_AuraScript);

            enum eSpells
            {
                AvengingWrathHoly   = 31842,
                AvengingWrath       = 31884,
                AvengingCrusader    = 216331,
                Crusade             = 231895
            };

            std::array<uint32, 4> const m_Spells =
            { {
                    eSpells::AvengingWrathHoly,
                    eSpells::AvengingWrath,
                    eSpells::AvengingCrusader,
                    eSpells::Crusade
            } };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                for (uint32 l_SpellId : m_Spells)
                    if (l_SpellId == l_SpellInfo->Id)
                        return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_chain_of_thrayn_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_chain_of_thrayn_AuraScript();
        }
};

/// Last Update: 7.1.5 Build 23420
/// Called for Chain of Thrayn - 206338
/// Called by Avenging Wrath - 31842, Avenging Wrath - 31884, Avenging Crusader - 216331, Crusade - 231895
class spell_pal_chain_of_thrayn_remove : public SpellScriptLoader
{
    public:
        spell_pal_chain_of_thrayn_remove() : SpellScriptLoader("spell_pal_chain_of_thrayn_remove") { }

        class spell_pal_chain_of_thrayn_remove_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_chain_of_thrayn_remove_AuraScript);

            enum eSpells
            {
                ChainOfThrayn       = 236328,
                AvengingWrathHealer = 31842,
                AvengingWrath       = 31884,
                Crusade             = 231895
            };

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::ChainOfThrayn);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::AvengingWrath:
                    case eSpells::Crusade:
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_chain_of_thrayn_remove_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                        break;
                    case eSpells::AvengingWrathHealer:
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_chain_of_thrayn_remove_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_MOD_HEALING_DONE_PERCENT, AURA_EFFECT_HANDLE_REAL);
                        break;
                    default:
                        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_chain_of_thrayn_remove_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_chain_of_thrayn_remove_AuraScript();
        }
};

/// 7.3.5 - Build 26365
/// Called for Scarlet Inquisitor's Expurgation - 248289
/// Called for Scarlet Inquisitor's Expurgation item - 151813
class PlayerScript_scarlet_inquisitors_expurgation_remove : public PlayerScript
{
    public:
        PlayerScript_scarlet_inquisitors_expurgation_remove() :PlayerScript("PlayerScript_scarlet_inquisitors_expurgation_remove") {}

        enum eItems
        {
            ItemScarletInquisitorsExpurgation = 151813
        };

        enum eSpells
        {
            ScarletInquisitorsExpurgationBuff = 248289
        };

        void OnAfterUnequipItem(Player* p_Player, Item const* p_Item) override
        {
            if (!p_Player || !p_Item || p_Item->GetEntry() != eItems::ItemScarletInquisitorsExpurgation)
                return;

            p_Player->RemoveAurasDueToSpell(eSpells::ScarletInquisitorsExpurgationBuff);
        }
};

////////////////////////////////////////////////////////////////////////////////
/// Hunter
////////////////////////////////////////////////////////////////////////////////

/// Last Update 7.1.5 Build 23420
/// Called by Carve - 187708 and Butchery - 212436
/// Called for Butcher's Bone Apron - 236447
class spell_hun_butchers_bone_apron_remove : public SpellScriptLoader
{
    public:
        spell_hun_butchers_bone_apron_remove() : SpellScriptLoader("spell_hun_butchers_bone_apron_remove") { }

        class spell_hun_butchers_bone_apron_remove_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_butchers_bone_apron_remove_SpellScript);

            enum eSpells
            {
                ButchersBoneApron = 236446
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster && l_Caster->HasAura(eSpells::ButchersBoneApron))
                    l_Caster->RemoveAura(eSpells::ButchersBoneApron);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_hun_butchers_bone_apron_remove_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_butchers_bone_apron_remove_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Celerity of the Windrunners - 248087
class spell_leg_celerity_of_the_windrunners : public SpellScriptLoader
{
    public:
        spell_leg_celerity_of_the_windrunners() : SpellScriptLoader("spell_leg_celerity_of_the_windrunners") { }

        class spell_leg_celerity_of_the_windrunners_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_celerity_of_the_windrunners_AuraScript);

            enum eSpells
            {
                Windburst    = 204147
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Windburst)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_leg_celerity_of_the_windrunners_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_celerity_of_the_windrunners_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Cinidaria  207692
class spell_gen_cinidaria : public SpellScriptLoader
{
    public:
        spell_gen_cinidaria() : SpellScriptLoader("spell_gen_cinidaria") { }

        class  spell_gen_cinidaria_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_cinidaria_AuraScript);

            enum eSpells
            {
                SymbioteStrike = 207694,
                CinidariaTheSymbiote = 207692
            };

            void HandleProc(ProcEventInfo &p_ProcInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcInfo.GetProcTarget();

                if (!l_Caster || !l_Target)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CinidariaTheSymbiote);
                if (!l_SpellInfo)
                    return;

                int32 l_TargetMinHealthPct = l_SpellInfo->Effects[1].BasePoints;

                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return;

                switch (l_DamageInfo->GetDamageType())
                {
                    case DamageEffectType::DIRECT_DAMAGE:
                    case DamageEffectType::SPELL_DIRECT_DAMAGE:
                    {
                        if (l_Target->HealthAbovePct(l_TargetMinHealthPct))
                        {
                            int32 l_Damage = p_ProcInfo.GetDamageInfo()->GetAmount();
                            l_Caster->CastCustomSpell(l_Target, eSpells::SymbioteStrike, &l_Damage, nullptr, nullptr, true);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_cinidaria_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_cinidaria_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Belo'vir's Final Stand - 207283
class spell_gen_belovirs_final_stand : public SpellScriptLoader
{
    public:
        spell_gen_belovirs_final_stand() : SpellScriptLoader("spell_gen_belovirs_final_stand") { }

        class  spell_gen_belovirs_final_stand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_belovirs_final_stand_AuraScript);

            enum eSpells
            {
                BelovirsFinalStandAura = 207277
            };

            void HandleCalculateAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BelovirsFinalStandAura);

                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_MaximumHealth = l_Caster->GetMaxHealth();
                p_Amount = CalculatePct(l_MaximumHealth, l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_belovirs_final_stand_AuraScript::HandleCalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_belovirs_final_stand_AuraScript();
        }
};

void ProcSmolderingHeart(Unit* l_Caster, int32 p_PowerSpent)
{
	enum eSpells
	{
		SmolderingHeart = 248029,
		AscendanceElementalist = 114050,
		AscendanceEnhancement = 114051,
	};

    if (l_Caster->HasAura(eSpells::SmolderingHeart))
    {
        SpellInfo const* l_SmolderingHeartSpellInfo = sSpellMgr->GetSpellInfo(eSpells::SmolderingHeart);
        if (l_SmolderingHeartSpellInfo)
        {
            if (l_Caster->ToPlayer())
            {
                float l_Mod = l_Caster->ToPlayer()->GetActiveSpecializationID() == SPEC_SHAMAN_ENHANCEMENT ? 0.12f : 0.1f;
                float l_ProcChance = p_PowerSpent*l_Mod;
                uint32 l_DurationTime = l_SmolderingHeartSpellInfo->Effects[0].BasePoints;
                if (roll_chance_f(l_ProcChance))
                {
                    uint32 l_SpellId = l_Caster->ToPlayer()->GetActiveSpecializationID() == SPEC_SHAMAN_ENHANCEMENT ? eSpells::AscendanceEnhancement : eSpells::AscendanceElementalist;
                    if (Aura* l_AscendanceAura = l_Caster->GetAura(l_SpellId))
                    {
                        l_AscendanceAura->SetMaxDuration(l_AscendanceAura->GetMaxDuration() + l_DurationTime);
                        l_AscendanceAura->SetDuration(l_AscendanceAura->GetDuration() + l_DurationTime);
                        return;
                    }
                    if (Aura* l_AscendanceAura = l_Caster->AddAura(l_SpellId, l_Caster))
                    {
                        l_AscendanceAura->SetMaxDuration(l_DurationTime);
                        l_AscendanceAura->SetDuration(l_DurationTime);
                    }
                }
            }
        }
    }
}

/// Last Update 7.3.2 Build 25549
class spell_leg_smoldering_heart : public SpellScriptLoader
{
    public:
        spell_leg_smoldering_heart() : SpellScriptLoader("spell_leg_smoldering_heart") { }

        enum eSpells
        {
            EarthShock                = 8042,
            FlameShock                = 188389,
            FrostShock                = 196840,
            LightningBoltEnhancement  = 187837
        };

        class spell_leg_smoldering_heart_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_smoldering_heart_SpellScript);

            void HandlePowerCost(Powers p_PowerType, int32& p_PowerCost)
            {
                if (p_PowerType != POWER_MAELSTROM)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 m_PowerSpent = p_PowerCost;

                if (GetSpellInfo())
                {
                    if (GetSpellInfo()->Id == eSpells::EarthShock)
                        m_PowerSpent = l_Caster->GetPower(POWER_MAELSTROM);
                    else if (GetSpellInfo()->Id == eSpells::FrostShock || GetSpellInfo()->Id == eSpells::FlameShock)
                        m_PowerSpent = std::min(l_Caster->GetPower(POWER_MAELSTROM), 20);
                    else if (GetSpellInfo()->Id == eSpells::LightningBoltEnhancement)
                        m_PowerSpent = std::min(l_Caster->GetPower(POWER_MAELSTROM), 40);
                }

                ProcSmolderingHeart(l_Caster, m_PowerSpent);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_leg_smoldering_heart_SpellScript::HandlePowerCost);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_smoldering_heart_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
class spell_leg_smoldering_heart_periodic : public SpellScriptLoader
{
    public:
        spell_leg_smoldering_heart_periodic() : SpellScriptLoader("spell_leg_smoldering_heart_periodic") { }

        class spell_leg_smoldering_heart_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_smoldering_heart_periodic_AuraScript);

            enum eSpells
            {
                FuryOfAir       = 197211
            };

            void HandleOnPeriodic(AuraEffect const*  /* aurEff */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetSpellInfo() && GetSpellInfo()->Id == eSpells::FuryOfAir)
                    ProcSmolderingHeart(l_Caster, 3);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_leg_smoldering_heart_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_smoldering_heart_periodic_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// The Sentinel's Eternal Refuge - 241331, 241846
class spell_leg_the_sentinels_eternal_refuge : public SpellScriptLoader
{
public:
    spell_leg_the_sentinels_eternal_refuge() : SpellScriptLoader("spell_leg_the_sentinels_eternal_refuge") { }

    class spell_leg_the_sentinels_eternal_refuge_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_the_sentinels_eternal_refuge_AuraScript);

        enum eSpells
        {
            TheSentinelsEternalRefuge = 241846,
            WispForm = 242112,
            WispFormPeriodic = 242111
        };

        void HandleOnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& /*p_ProcEventInfo*/)
        {
            if (Unit* l_Caster = GetCaster())
                l_Caster->CastSpell(l_Caster, eSpells::TheSentinelsEternalRefuge, true);
        }

        void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_SpellInfo)
                return;

            if (!l_Caster->HasAura(eSpells::WispForm) && p_AurEff->GetBase()->GetStackAmount() == l_SpellInfo->Effects[EFFECT_1].BasePoints)
            {
                l_Caster->CastSpell(l_Caster, eSpells::WispForm, true);
                l_Caster->CastSpell(l_Caster, eSpells::WispFormPeriodic, true);
            }
        }

        void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::WispForm);
            l_Caster->RemoveAura(eSpells::WispFormPeriodic);
        }

        void Register() override
        {
            if (m_scriptSpellId == eSpells::TheSentinelsEternalRefuge)
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_the_sentinels_eternal_refuge_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_the_sentinels_eternal_refuge_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
            else
                OnEffectProc += AuraEffectProcFn(spell_leg_the_sentinels_eternal_refuge_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_the_sentinels_eternal_refuge_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// The Sentinel's Eternal Refuge WispForm - 242111
class spell_leg_the_sentinels_eternal_refuge_wisp_form : public SpellScriptLoader
{
public:
    spell_leg_the_sentinels_eternal_refuge_wisp_form() : SpellScriptLoader("spell_leg_the_sentinels_eternal_refuge_wisp_form") { }

    class spell_leg_the_sentinels_eternal_refuge_wisp_form_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_the_sentinels_eternal_refuge_wisp_form_AuraScript);

        enum eSpells
        {
            TheSentinelsEternalRefuge = 241846,
            WispForm = 241849,
            WispFormFly = 241980
        };

        void OnTick(AuraEffect const* p_AuraEffect)
        {
            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_Aura || !l_SpellInfo)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if (!l_Player->HasAura(eSpells::TheSentinelsEternalRefuge))
            {
                Remove();
                return;
            }

            if (l_Player->isInCombat())
            {
                l_Player->RemoveAura(eSpells::WispForm);
                l_Player->RemoveAura(eSpells::WispFormFly);
                return;
            }

            if (l_Player->IsKnowHowFlyIn(l_Player->GetMapId(), l_Player->GetZoneId(), eSpells::WispFormFly))
            {
                l_Player->CastSpell(l_Player, eSpells::WispFormFly, true);
                l_Player->RemoveAura(eSpells::WispForm);
            }
            else
            {
                l_Player->CastSpell(l_Player, eSpells::WispForm, true);
                l_Player->RemoveAura(eSpells::WispFormFly);
            }
        }

        void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::WispForm);
            l_Caster->RemoveAura(eSpells::WispFormFly);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_leg_the_sentinels_eternal_refuge_wisp_form_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            AfterEffectRemove += AuraEffectRemoveFn(spell_leg_the_sentinels_eternal_refuge_wisp_form_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_the_sentinels_eternal_refuge_wisp_form_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Fireball (133) and Pyroblast (11366)
class spell_leg_contained_infrenal_core : public SpellScriptLoader
{
    public:
        spell_leg_contained_infrenal_core() : SpellScriptLoader("spell_leg_contained_infrenal_core") { }

        class spell_leg_contained_infrenal_core_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_contained_infrenal_core_SpellScript);

            enum eSpells
            {
                CounterBuff    = 248146,
                Meteor         = 177345,
                CICTalent      = 248099
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->HasAura(CICTalent))
                    return;

                if (Aura* l_CounterAura = l_Caster->AddAura(eSpells::CounterBuff, l_Caster))
                {
                    if (l_CounterAura->GetStackAmount() == 30)
                    {
                        l_Caster->RemoveAura(eSpells::CounterBuff);

                        l_Caster->CastSpell(l_Target->GetPosition(), eSpells::Meteor, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_leg_contained_infrenal_core_SpellScript::HandleOnHit);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_contained_infrenal_core_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Frostbolt (116) and Flurry (44614)
/// Called for Shattered Fragments of Sindragosa 248100
class spell_leg_shattered_fragments_of_sindragosa : public SpellScriptLoader
{
    public:
        spell_leg_shattered_fragments_of_sindragosa() : SpellScriptLoader("spell_leg_shattered_fragments_of_sindragosa") { }

        class spell_leg_shattered_fragments_of_sindragosa_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_shattered_fragments_of_sindragosa_SpellScript);

            enum eSpells
            {
                ShatteredFragmentsOfSindragosa     = 248100,
                ShatteredFragmentsOfSindragosaAura = 248176,
                CometStorm                         = 153595
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShatteredFragmentsOfSindragosaAura);
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_Caster->HasAura(eSpells::ShatteredFragmentsOfSindragosa))
                    return;

                if (Aura* l_Aura = l_Caster->AddAura(eSpells::ShatteredFragmentsOfSindragosaAura, l_Caster))
                {
                    if (l_Aura->GetStackAmount() == l_SpellInfo->StackAmount)
                    {
                        l_Caster->CastSpell(l_Target->GetPosition(), eSpells::CometStorm, true);
                        l_Caster->RemoveAurasDueToSpell(eSpells::ShatteredFragmentsOfSindragosaAura);
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_leg_shattered_fragments_of_sindragosa_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_shattered_fragments_of_sindragosa_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// The Mantle of Command - 235721
class spell_leg_the_mantle_of_command : public SpellScriptLoader
{
public:
    spell_leg_the_mantle_of_command() : SpellScriptLoader("spell_leg_the_mantle_of_command") { }

    class spell_leg_the_mantle_of_command_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_the_mantle_of_command_AuraScript);

        enum eSpells
        {
            DireFrenzy = 217200
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::DireFrenzy)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_leg_the_mantle_of_command_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_the_mantle_of_command_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Cord of Infinity - 209311
class spell_leg_cord_of_infinity : public SpellScriptLoader
{
public:
    spell_leg_cord_of_infinity() : SpellScriptLoader("spell_leg_cord_of_infinity") { }

    class spell_leg_cord_of_infinity_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_cord_of_infinity_AuraScript);

        enum eSpells
        {
            ArcaneMissiles = 7268
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::ArcaneMissiles)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_leg_cord_of_infinity_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_cord_of_infinity_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Rhonin's Assaulting Armwraps - 208080
class spell_leg_rhonins_assaulting_armwraps: public SpellScriptLoader
{
public:
    spell_leg_rhonins_assaulting_armwraps() : SpellScriptLoader("spell_leg_rhonins_assaulting_armwraps") { }

    class spell_leg_rhonins_assaulting_armwraps_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_rhonins_assaulting_armwraps_AuraScript);

        enum eSpells
        {
            ArcaneMissiles = 7268,
            MissiliesCast  = 5143
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return false;

            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo)
                return false;

            if (l_SpellInfo->Id != eSpells::ArcaneMissiles)
                return false;

            if (Aura* l_MissilesCastAura = l_Caster->GetAura(eSpells::MissiliesCast))
                if (l_MissilesCastAura->GetDuration() >= 500)
                    return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_leg_rhonins_assaulting_armwraps_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_rhonins_assaulting_armwraps_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Vigilance Perch - 241332, 242066
class spell_leg_vigilance_perch : public SpellScriptLoader
{
public:
    spell_leg_vigilance_perch() : SpellScriptLoader("spell_leg_vigilance_perch") { }

    class spell_leg_vigilance_perch_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_vigilance_perch_AuraScript);

        enum eSpells
        {
            VigilancePerch = 242066,
            SpiritOwl = 242110,
            SpiritOwlPeriodic = 242105
        };

        void HandleOnProc(AuraEffect const* /*p_AuraEffect*/, ProcEventInfo& /*p_ProcEventInfo*/)
        {
            if (Unit* l_Caster = GetCaster())
                l_Caster->CastSpell(l_Caster, eSpells::VigilancePerch, true);
        }

        void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_SpellInfo)
                return;

            if (!l_Caster->HasAura(eSpells::SpiritOwl) && p_AurEff->GetBase()->GetStackAmount() == l_SpellInfo->Effects[EFFECT_1].BasePoints)
            {
                l_Caster->CastSpell(l_Caster, eSpells::SpiritOwl, true);
                l_Caster->CastSpell(l_Caster, eSpells::SpiritOwlPeriodic, true);
            }
        }

        void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::SpiritOwl);
            l_Caster->RemoveAura(eSpells::SpiritOwlPeriodic);
        }

        void Register() override
        {
            if (m_scriptSpellId == eSpells::VigilancePerch)
            {
                AfterEffectApply += AuraEffectApplyFn(spell_leg_vigilance_perch_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_vigilance_perch_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
            else
                OnEffectProc += AuraEffectProcFn(spell_leg_vigilance_perch_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_vigilance_perch_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Vigilance Perch Spirit Owl - 242105
class spell_leg_vigilance_perch_spirit_owl : public SpellScriptLoader
{
public:
    spell_leg_vigilance_perch_spirit_owl() : SpellScriptLoader("spell_leg_vigilance_perch_spirit_owl") { }

    class spell_leg_vigilance_perch_spirit_owl_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_vigilance_perch_spirit_owl_AuraScript);

        enum eSpells
        {
            VigilancePerch = 242066,
            SpiritOwl = 242070,
            SpiritOwlFly = 242072
        };

        void OnTick(AuraEffect const* p_AuraEffect)
        {
            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_Aura || !l_SpellInfo)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if (!l_Player->HasAura(eSpells::VigilancePerch))
            {
                Remove();
                return;
            }

            if (l_Player->isInCombat())
            {
                l_Player->RemoveAura(eSpells::SpiritOwl);
                l_Player->RemoveAura(eSpells::SpiritOwlFly);
                return;
            }

            if (l_Player->IsKnowHowFlyIn(l_Player->GetMapId(), l_Player->GetZoneId(), eSpells::SpiritOwlFly))
            {
                l_Player->CastSpell(l_Player, eSpells::SpiritOwlFly, true);
                l_Player->RemoveAura(eSpells::SpiritOwl);
            }
            else
            {
                l_Player->CastSpell(l_Player, eSpells::SpiritOwl, true);
                l_Player->RemoveAura(eSpells::SpiritOwlFly);
            }
        }

        void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::SpiritOwl);
            l_Caster->RemoveAura(eSpells::SpiritOwlFly);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_leg_vigilance_perch_spirit_owl_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            AfterEffectRemove += AuraEffectRemoveFn(spell_leg_vigilance_perch_spirit_owl_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_vigilance_perch_spirit_owl_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Parsel's Tongue - 248084
class spell_leg_parsels_tongue : public SpellScriptLoader
{
public:
    spell_leg_parsels_tongue() : SpellScriptLoader("spell_leg_parsels_tongue") { }

    class spell_leg_parsels_tongue_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_parsels_tongue_AuraScript);

        enum eSpells
        {
            CobraShot = 193455
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::CobraShot)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_leg_parsels_tongue_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_parsels_tongue_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Rethu's Incessant Courage - 241330, 242601
class spell_leg_rethus_incessant_courage : public SpellScriptLoader
{
public:
    spell_leg_rethus_incessant_courage() : SpellScriptLoader("spell_leg_rethus_incessant_courage") { }

    enum eSpells
    {
        Heal = 242597,
        Speed = 242599
    };

    class spell_leg_rethus_incessant_courage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_leg_rethus_incessant_courage_SpellScript);

        void FilterTargets(std::list<WorldObject*> &p_Targets)
        {
            m_EnemyCount = p_Targets.size();
        }

        void HandleAfterCast()
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (m_EnemyCount != 0)
                {
                    if (!l_Caster->HasAura(eSpells::Heal))
                        l_Caster->CastSpell(l_Caster, eSpells::Heal, true);
                    l_Caster->RemoveAura(eSpells::Speed);
                }
                else
                {
                    if (!l_Caster->HasAura(eSpells::Speed))
                        l_Caster->CastSpell(l_Caster, eSpells::Speed, true);
                    l_Caster->RemoveAura(eSpells::Heal);
                }
            }
        }

        void Register() override
        {
            if (m_scriptSpellId == 242601)
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_leg_rethus_incessant_courage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_leg_rethus_incessant_courage_SpellScript::HandleAfterCast);
            }
        }

    private:
        int32 m_EnemyCount = 0;
    };

    class spell_leg_rethus_incessant_courage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_rethus_incessant_courage_AuraScript);

        void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            if (Unit* l_Caster = GetCaster())
            {
                l_Caster->RemoveAura(eSpells::Heal);
                l_Caster->RemoveAura(eSpells::Speed);
            }
        }

        void Register() override
        {
            if (m_scriptSpellId == 241330)
                AfterEffectRemove += AuraEffectRemoveFn(spell_leg_rethus_incessant_courage_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_leg_rethus_incessant_courage_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_rethus_incessant_courage_AuraScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Chilled Heart - 235592
class spell_leg_cold_heart : public SpellScriptLoader
{
    public:
        spell_leg_cold_heart() : SpellScriptLoader("spell_leg_cold_heart") { }

        class spell_leg_cold_heart_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leg_cold_heart_AuraScript);

            enum eSpells
            {
                ColdHeartBuff       = 235599
            };

            void HandleOnEffectPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::ColdHeartBuff, true);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::ColdHeartBuff);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_leg_cold_heart_AuraScript::HandleOnEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectApplyFn(spell_leg_cold_heart_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leg_cold_heart_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Chains of Ice - 45524
class spell_leg_cold_heart_damage : public SpellScriptLoader
{
    public:
        spell_leg_cold_heart_damage() : SpellScriptLoader("spell_leg_cold_heart_damage") { }

        class spell_leg_cold_heart_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_cold_heart_damage_SpellScript);

            enum eSpells
            {
                ChainsOfIce         = 45524,
                ColdHeartDamage     = 248397,
                ColdHeartIceBlock   = 248406,
                ChilledHeart        = 235592,
                ColdHeartBuff       = 235599
            };

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::ChilledHeart) || !l_Caster->HasAura(eSpells::ColdHeartBuff))
                    return;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ColdHeartDamage))
                {
                    if (Aura* l_BuffAura = l_Caster->GetAura(eSpells::ColdHeartBuff))
                    {
                        l_Caster->CastSpell(l_Target, eSpells::ColdHeartDamage, true);

                        if (l_BuffAura->GetStackAmount() >= 20)
                            l_Caster->CastSpell(l_Target, eSpells::ColdHeartIceBlock, true);

                        l_Caster->RemoveAura(eSpells::ColdHeartBuff);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_leg_cold_heart_damage_SpellScript::HandleEffectHitTarget, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_cold_heart_damage_SpellScript;
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Cold Heart (damage) - 248397
class spell_leg_cold_heart_damage_amount : public SpellScriptLoader
{
    public:
        spell_leg_cold_heart_damage_amount() : SpellScriptLoader("spell_leg_cold_heart_damage_amount") { }

        class spell_leg_cold_heart_damage_amount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_cold_heart_damage_amount_SpellScript);

            enum eSpells
            {
                ChilledHeart        = 235592,
                ColdHeartBuff       = 235599
            };

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::ChilledHeart) || !l_Caster->HasAura(eSpells::ColdHeartBuff))
                    return;

                if (Aura* l_BuffAura = l_Caster->GetAura(eSpells::ColdHeartBuff))
                    SetHitDamage(GetHitDamage() * l_BuffAura->GetStackAmount());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_leg_cold_heart_damage_amount_SpellScript::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_cold_heart_damage_amount_SpellScript;
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Kil'jaeden's Burning Wish - 235991
class spell_leg_kiljaedens_burning_wish_break_stealth : public SpellScriptLoader
{
    public:
        spell_leg_kiljaedens_burning_wish_break_stealth() : SpellScriptLoader("spell_leg_kiljaedens_burning_wish_break_stealth") { }

        class spell_leg_kiljaedens_burning_wish_break_stealth_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_kiljaedens_burning_wish_break_stealth_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAurasByType(AuraType::SPELL_AURA_MOD_STEALTH);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_leg_kiljaedens_burning_wish_break_stealth_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_kiljaedens_burning_wish_break_stealth_SpellScript;
        }
};

/// Update to Legion 7.3.5 build: 25996
/// Called By Zeks Exterminatus 236545
class spell_leg_zeks_exterminatus : public SpellScriptLoader
{
public:
    spell_leg_zeks_exterminatus() : SpellScriptLoader("spell_leg_zeks_exterminatus") { }

    class spell_leg_zeks_exterminatus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_zeks_exterminatus_AuraScript);

        enum eSpells
        {
            Pain = 589
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEvent)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo* l_DamageInfo = p_ProcEvent.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            SpellInfo const * l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Pain)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_leg_zeks_exterminatus_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_zeks_exterminatus_AuraScript();
    }
};

/// Last Update 7.3.5 build 26124
/// Called by Rampage hits (218617, 184707, 184709, 201364, 201363)
class spell_leg_valarjar_berserkers : public SpellScriptLoader
{
    public:
        spell_leg_valarjar_berserkers() : SpellScriptLoader("spell_leg_valarjar_berserkers") { }

        class spell_leg_valarjar_berserkers_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_valarjar_berserkers_SpellScript);

            enum eSpells
            {
                ValarjarBerserkers  = 248120
            };

            void HandleAfterHit()
            {
                Spell* l_Spell = GetSpell();
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();

                if (!l_Spell || !l_Target || !l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::ValarjarBerserkers))
                {
                    bool crit = l_Spell->IsCritForTarget(l_Target);
                    if (crit)
                    {
                        if (l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::RampageMainTarget) == 0 || l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::RampageMainTarget) == l_Target->GetGUID())
                            l_Caster->ModifyPower(POWER_RAGE,  60);
                    }
                }

                /// Used for T21 set
                l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::RampageTarget][l_Target->GetGUID()] += GetHitDamage();
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_leg_valarjar_berserkers_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_valarjar_berserkers_SpellScript();
        }
};

/// Called By Wild Growth - 48438
/// For Chameleon Song - 248034
/// Updated as of 7.3.5 build 26124
class spell_leg_chameleon_song : public SpellScriptLoader
{
    public:
        spell_leg_chameleon_song() : SpellScriptLoader("spell_leg_chameleon_song") { }

        class spell_leg_chameleon_song_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_leg_chameleon_song_SpellScript);

            enum eSpells
            {
                IncranationTreeOfLife   = 33891,
                ChameleonSong           = 248034
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ChameleonSong);
                if (!l_Caster || !l_Caster->HasAura(eSpells::ChameleonSong) || !l_SpellInfo)
                    return;

                if (roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::IncranationTreeOfLife))
                    {
                        l_Aura->SetMaxDuration(l_Aura->GetDuration() + l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                        l_Aura->SetDuration(l_Aura->GetDuration() + l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                    }
                    else if (Aura* l_Aura = l_Caster->AddAura(eSpells::IncranationTreeOfLife, l_Caster))
                    {
                        l_Aura->SetMaxDuration(l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                        l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_leg_chameleon_song_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_leg_chameleon_song_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By The Night's Dichotomy 241334
class spell_leg_celumbra_the_nights_dichotomy_aura : public SpellScriptLoader
{
public:
    spell_leg_celumbra_the_nights_dichotomy_aura() : SpellScriptLoader("spell_leg_celumbra_the_nights_dichotomy_aura") { }

    class spell_leg_celumbra_the_nights_dichotomy_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_leg_celumbra_the_nights_dichotomy_aura_AuraScript);

        enum eSpells
        {
            ShadowOfCelumbra    = 241836, // speed and decrease damage
            StarlightOfCelumbra = 241835, // health regen
            CelumbraTrigger     = 242600
        };

        void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (l_Caster->HasAura(eSpells::ShadowOfCelumbra))
                l_Caster->RemoveAura(eSpells::ShadowOfCelumbra);

            if (l_Caster->HasAura(eSpells::StarlightOfCelumbra))
                l_Caster->RemoveAura(eSpells::StarlightOfCelumbra);
        }

        void OnUpdate(uint32 /*p_Diff*/)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CelumbraTrigger);
            if (!l_Caster || !l_SpellInfo)
                return;

            std::list<Unit*> l_TargetList;
            SpellRadiusEntry const* l_RadiusEntry = l_SpellInfo->Effects[EFFECT_0].RadiusEntry;
            if (!l_RadiusEntry)
                return;

            JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_RadiusEntry->RadiusMax);
            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
            l_Caster->VisitNearbyObject(l_RadiusEntry->RadiusMax, l_Searcher);

            if (l_TargetList.empty())
            {
                if (l_Caster->HasAura(eSpells::ShadowOfCelumbra))
                    l_Caster->RemoveAura(eSpells::ShadowOfCelumbra);

                l_Caster->AddAura(eSpells::StarlightOfCelumbra, l_Caster);
            }
            else
            {
                if (l_Caster->HasAura(eSpells::StarlightOfCelumbra))
                    l_Caster->RemoveAura(eSpells::StarlightOfCelumbra);

                l_Caster->AddAura(eSpells::ShadowOfCelumbra, l_Caster);
            }
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_leg_celumbra_the_nights_dichotomy_aura_AuraScript::OnUpdate);
            AfterEffectRemove += AuraEffectRemoveFn(spell_leg_celumbra_the_nights_dichotomy_aura_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_leg_celumbra_the_nights_dichotomy_aura_AuraScript();
    }
};

#ifndef __clang_analyzer__
void AddSC_spell_item_legendary()
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Common
    ////////////////////////////////////////////////////////////////////////////////

    new spell_leg_sephuzs_secret();
    new spell_leg_aggramars_stride();
    new spell_leg_xavarics_magnum_opus();
    new spell_leg_xavarics_magnum_opus_shield();
    new spell_leg_archimondes_hatred_reborn();
    new spell_leg_norgannons_foresight();
    new spell_leg_norgannons_foresight_moving();
    new spell_leg_norgannons_foresight_not_moving();
    new spell_leg_norgannons_foresight_recently_moved();
    new spell_gen_cinidaria();
    new spell_leg_celumbra_the_nights_dichotomy_aura();
    new spell_leg_vigilance_perch();
    new spell_leg_vigilance_perch_spirit_owl();
    new spell_leg_rethus_incessant_courage();
    new spell_leg_kiljaedens_burning_wish_break_stealth();

    ////////////////////////////////////////////////////////////////////////////////
    /// Priest
    ////////////////////////////////////////////////////////////////////////////////

    new spell_leg_zeks_exterminatus();

    ////////////////////////////////////////////////////////////////////////////////
    /// Shaman
    ////////////////////////////////////////////////////////////////////////////////

    new spell_leg_eye_of_the_twisting_nether();
    new spell_leg_velens_future_sight();
    new spell_leg_velens_future_sight_heal();
    new spell_leg_intact_nazjatar_molting();
    new spell_leg_roots_of_shaladrassil();
    new spell_leg_roots_of_shaladrassil_dummy();
    new spell_leg_alakirs_acrimony();
    new spell_leg_akainus_absolute_justice();
    new spell_leg_uncertain_reminder();
    new spell_leg_uncertain_reminder_aura();
    new spell_leg_smoldering_heart();
    new spell_leg_smoldering_heart_periodic();
    new spell_leg_the_sentinels_eternal_refuge();
    new spell_leg_the_sentinels_eternal_refuge_wisp_form();

    ////////////////////////////////////////////////////////////////////////////////
    /// Death Knight
    ////////////////////////////////////////////////////////////////////////////////

    new spell_leg_newfound_will();
    new spell_leg_girdle_of_the_everlasting_king();
    new spell_leg_cold_heart();
    new spell_leg_cold_heart_damage();
    new spell_leg_cold_heart_damage_amount();

    ////////////////////////////////////////////////////////////////////////////////
    /// Hunter
    ////////////////////////////////////////////////////////////////////////////////

    new spell_hun_butchers_bone_apron_remove();
    new spell_leg_celerity_of_the_windrunners();
    new spell_leg_the_mantle_of_command();
    new spell_leg_parsels_tongue();

    ////////////////////////////////////////////////////////////////////////////////
    /// Monk
    ////////////////////////////////////////////////////////////////////////////////

    new spell_monk_ovyds_winter_wrap();

    ////////////////////////////////////////////////////////////////////////////////
    /// Demon Hunter
    ////////////////////////////////////////////////////////////////////////////////

    new spell_leg_raddons_cascading_eyes();

    ////////////////////////////////////////////////////////////////////////////////
    /// Death Knight
    ////////////////////////////////////////////////////////////////////////////////

    new spell_leg_newfound_will();
    new spell_leg_girdle_of_the_everlasting_king();
    new spell_leg_toravons_whiteout_bindings();

    ////////////////////////////////////////////////////////////////////////////////
    /// Warlock
    ////////////////////////////////////////////////////////////////////////////////
    new spell_leg_wilfreds_sigil_of_superior_summoning();
    new spell_leg_warlock_feretory_of_souls();

    ////////////////////////////////////////////////////////////////////////////////
    /// Paladin
    ////////////////////////////////////////////////////////////////////////////////
    new spell_pal_chain_of_thrayn();
    new spell_pal_chain_of_thrayn_remove();
    new PlayerScript_scarlet_inquisitors_expurgation_remove();

    ////////////////////////////////////////////////////////////////////////////////
    /// Mage
    ////////////////////////////////////////////////////////////////////////////////
    new spell_gen_belovirs_final_stand();
    new spell_leg_contained_infrenal_core();
    new spell_leg_shattered_fragments_of_sindragosa();
    new spell_leg_cord_of_infinity();
    new spell_leg_rhonins_assaulting_armwraps();

    ////////////////////////////////////////////////////////////////////////////////
    /// Warrior
    ////////////////////////////////////////////////////////////////////////////////
    new spell_leg_valarjar_berserkers();

    ////////////////////////////////////////////////////////////////////////////////
    /// Druid
    ////////////////////////////////////////////////////////////////////////////////
    new spell_leg_chameleon_song();
}
#endif
