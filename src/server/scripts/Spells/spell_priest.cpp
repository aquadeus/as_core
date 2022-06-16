////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pri_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "Player.h"
#include "HelperDefines.h"
#include "Group.h"
#include "Battleground.h"

// Confession (Toy) - 126123
class spell_pri_confession: public SpellScriptLoader
{
    public:
        spell_pri_confession() : SpellScriptLoader("spell_pri_confession") { }

        class spell_pri_confession_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_confession_SpellScript);

            void HandleOnHit()
            {
                if (Player* l_Caster = GetCaster()->ToPlayer())
                {
                    if (!GetHitUnit())
                        return;

                    if (Player* l_Target = GetHitUnit()->ToPlayer())
                    {
                        std::string l_Name = l_Target->GetName();
                        std::string l_Text = "[" + l_Name + "]" + l_Caster->GetSession()->GetTrinityString(LANG_CONFESSION_EMOTE);
                        l_Text += l_Caster->GetSession()->GetTrinityString(urand(LANG_CONFESSION_START, LANG_CONFESSION_END));

                        std::list<Player*> l_PlayerList;
                        l_Caster->GetPlayerListInGrid(l_PlayerList, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY));

                        for (Player* l_Target : l_PlayerList)
                        {
                            if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT) && l_Target->GetTeamId() != l_Caster->GetTeamId() && !l_Target->isGameMaster())
                                continue;

                            if (WorldSession* l_Session = l_Target->GetSession())
                            {
                                WorldPacket l_Data;
                                /// No specific target needed
                                l_Caster->BuildPlayerChat(&l_Data, 0, CHAT_MSG_EMOTE, l_Text, LANG_UNIVERSAL);
                                l_Session->SendPacket(&l_Data);
                            }
                        }
                    }
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_confession_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_confession_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Spirit of Redemption - 20711, 215769
class spell_pri_spirit_of_redemption: public SpellScriptLoader
{
    public:
        spell_pri_spirit_of_redemption() : SpellScriptLoader("spell_pri_spirit_of_redemption") { }

        class spell_pri_spirit_of_redemption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_spirit_of_redemption_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Register()
            {
                if (m_scriptSpellId == 215769)
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_spirit_of_redemption_AuraScript::CalculateAmount, EFFECT_11, SPELL_AURA_SCHOOL_ABSORB);
                else
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_spirit_of_redemption_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_spirit_of_redemption_AuraScript();
        }
};

/// Last Update 6.2.3
/// Spirit of Redemption (Shapeshift) - 27827
class spell_pri_spirit_of_redemption_form: public SpellScriptLoader
{
    public:
        spell_pri_spirit_of_redemption_form() : SpellScriptLoader("spell_pri_spirit_of_redemption_form") { }

        class spell_pri_spirit_of_redemption_form_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_spirit_of_redemption_form_AuraScript);

            enum eSpells
            {
                SpiritOfRedemptionImmunity              = 62371,
                SpiritOfRedemptionForm                  = 27795,
                SpiritOfRedemptionCost                  = 27792,
                ArchbishopBenedictusRestitutionAura     = 211317,
                ArchbishopBenedictusRestitutionBuff     = 211336,
                ArchbishopBenedictusRestitutionDebuff   = 211319,

                /// Xavius specific case
                SpellCorruptionMadnessCharm             = 207409
            };

            int32 m_Mana = 0;

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Owner = GetUnitOwner();

                if (!l_Owner || !l_Target)
                    return;

                l_Target->RemoveAura(eSpells::SpiritOfRedemptionForm);
                l_Target->RemoveAura(eSpells::SpiritOfRedemptionImmunity);
                l_Target->RemoveAura(eSpells::SpiritOfRedemptionCost);

                if (l_Owner->HasAura(eSpells::ArchbishopBenedictusRestitutionBuff) && l_Owner->IsPlayer())
                {
                    Player *l_Player = l_Owner->ToPlayer();

                    if (AuraEffect *l_Effect = l_Owner->GetAuraEffect(eSpells::ArchbishopBenedictusRestitutionBuff, 0))
                    {
                        l_Player->ResurrectPlayer(l_Effect->GetAmount() / 100.f);
                        l_Owner->RemoveAurasDueToSpell(eSpells::ArchbishopBenedictusRestitutionBuff);
                        l_Owner->CastSpell(l_Owner, eSpells::ArchbishopBenedictusRestitutionDebuff, true);
                        l_Player->SetPower(POWER_MANA, m_Mana);
                        l_Player->UpdateManaRegen();
                    }
                }
            }

            void AfterApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner || !l_Owner->IsPlayer())
                    return;

                l_Owner->RemoveAura(eSpells::SpellCorruptionMadnessCharm);

                Player* l_Player = l_Owner->ToPlayer();

                if (l_Owner->HasAura(eSpells::ArchbishopBenedictusRestitutionAura))
                {
                    l_Owner->CastSpell(l_Owner, eSpells::ArchbishopBenedictusRestitutionBuff, true);
                    m_Mana = l_Player->GetPower(POWER_MANA);
                    l_Player->SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, 0);
                    l_Player->SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, 0);

                    SpellCooldowns l_Cooldowns = l_Player->GetSpellCooldowns();
                    for (auto l_Cooldown : l_Cooldowns)
                    {
                        const ItemTemplate* l_Item = sObjectMgr->GetItemTemplate(l_Cooldown.second.itemid);

                        if (l_Item && l_Item->SubClass == ITEM_SUBCLASS_POTION)
                            l_Player->RemoveSpellCooldown(l_Cooldown.first, true);
                    }
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_spirit_of_redemption_form_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_WATER_BREATHING, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectApplyFn(spell_pri_spirit_of_redemption_form_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_WATER_BREATHING, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_spirit_of_redemption_form_AuraScript();
        }
};

/// Last Update 7.1.5
/// Archbishop Benedictus Restitution - 211317
class spell_pri_archbishop_benedictus_restitution : public SpellScriptLoader
{
public:
    spell_pri_archbishop_benedictus_restitution() : SpellScriptLoader("spell_pri_archbishop_benedictus_restitution") { }

    class spell_pri_archbishop_benedictus_restitution_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_archbishop_benedictus_restitution_AuraScript);

        enum eSpells
        {
            ArchbishopBenedictusRestitutionBuff = 211336
        };

        void HandleProc(ProcEventInfo&)
        {
            Unit* l_Owner = GetUnitOwner();

            if (!l_Owner)
                return;

            if (AuraEffect *l_Effect = l_Owner->GetAuraEffect(eSpells::ArchbishopBenedictusRestitutionBuff, EFFECT_0))
            {
                int32 l_Amount = l_Effect->GetAmount() + 15;

                if (l_Amount > 100)
                    l_Amount = 100;
                l_Effect->ChangeAmount(l_Amount);
            }
        }

        void Register()
        {
            OnProc += AuraProcFn(spell_pri_archbishop_benedictus_restitution_AuraScript::HandleProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_archbishop_benedictus_restitution_AuraScript();
    }
};

// Called by Prayer of Mending - 33076
// Item : S12 4P bonus - Heal
class spell_pri_item_s12_4p_heal: public SpellScriptLoader
{
    public:
        spell_pri_item_s12_4p_heal() : SpellScriptLoader("spell_pri_item_s12_4p_heal") { }

        class spell_pri_item_s12_4p_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_item_s12_4p_heal_SpellScript);

            enum eSpells
            {
                Bonus4ItemsS12Heal  = 131566,
                HolySpark           = 131567
            };

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                    if (Unit* l_Target = GetHitUnit())
                        if (l_Player->HasAura(eSpells::Bonus4ItemsS12Heal))
                            l_Player->CastSpell(l_Target, eSpells::HolySpark, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_item_s12_4p_heal_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_item_s12_4p_heal_SpellScript();
        }
};

// Called by Dispersion - 47585
// Item : S12 2P bonus - Shadow
class spell_pri_item_s12_2p_shadow: public SpellScriptLoader
{
    public:
        spell_pri_item_s12_2p_shadow() : SpellScriptLoader("spell_pri_item_s12_2p_shadow") { }

        class spell_pri_item_s12_2p_shadow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_item_s12_2p_shadow_SpellScript);

            enum eSpells
            {
                Bonus2ItemsS12Shadow = 92711,
                DispersionSprint     = 129960
            };

            void HandleOnHit()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                    if (l_Player->HasAura(eSpells::Bonus2ItemsS12Shadow))
                        l_Player->CastSpell(l_Player, eSpells::DispersionSprint, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_item_s12_2p_shadow_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_item_s12_2p_shadow_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Shadowy Insight - 124430
class spell_pri_shadowy_insight: public SpellScriptLoader
{
    public:
        spell_pri_shadowy_insight() : SpellScriptLoader("spell_pri_shadowy_insight") { }

        class spell_pri_shadowy_insight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadowy_insight_SpellScript);

            enum eSpells
            {
                MindBlast = 8092
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MindBlast))
                    {
                        if (SpellCategoryEntry const* l_CategoryEntry = l_SpellInfo->ChargeCategoryEntry)
                        {
                            l_Player->RestoreCharge(l_CategoryEntry);
                        }
                        else
                        {
                            l_Player->RemoveSpellCooldown(eSpells::MindBlast, true);
                        }
                    }
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_shadowy_insight_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_shadowy_insight_SpellScript();
        }
};

/// Last Update 7.0.3 - 22522
/// Power Word - Solace - 129253
class spell_pri_power_word_solace: public SpellScriptLoader
{
    public:
        spell_pri_power_word_solace() : SpellScriptLoader("spell_pri_power_word_solace") { }

        class spell_pri_power_word_solace_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_power_word_solace_SpellScript);

            enum eSpells
            {
                PWSolace = 129250
            };

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {

                SpellInfo const* l_SpellInfos = sSpellMgr->GetSpellInfo(eSpells::PWSolace);
                if (!l_SpellInfos)
                    return;

                float l_Amount = l_SpellInfos->Effects[EFFECT_1].BasePoints;

                SetHitDamage(l_Amount/100.0f);
            }

            void HandleOnHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target == l_Caster)
                    SetHitDamage(0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_power_word_solace_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_ENERGIZE_PCT);
                OnHit += SpellHitFn(spell_pri_power_word_solace_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_power_word_solace_SpellScript();
        }
};

// Shadowfiend - 34433 or Mindbender - 123040
class spell_pri_shadowfiend: public SpellScriptLoader
{
    public:
        spell_pri_shadowfiend() : SpellScriptLoader("spell_pri_shadowfiend") { }

        class spell_pri_shadowfiend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadowfiend_SpellScript);

            void HandleAfterHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetExplTargetUnit())
                    {
                        if (Guardian* pet = _player->GetGuardianPet())
                        {
                            pet->InitCharmInfo();
                            pet->SetReactState(REACT_AGGRESSIVE);

                            if (pet->IsValidAttackTarget(target))
                                pet->ToCreature()->AI()->AttackStart(target);
                            else
                            {
                                Unit* victim = _player->GetSelectedUnit();
                                if (victim && pet->IsValidAttackTarget(target))
                                    pet->ToCreature()->AI()->AttackStart(target);
                            }
                        }
                    }
                }
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_pri_shadowfiend_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_shadowfiend_SpellScript();
        }
};

/// Last Update 6.2.3
/// Surge of Light - 114255
class spell_pri_surge_of_light : public SpellScriptLoader
{
    public:
        spell_pri_surge_of_light() : SpellScriptLoader("spell_pri_surge_of_light") { }

        class spell_pri_surge_of_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_surge_of_light_SpellScript);

            enum eSpells
            {
                SurgeOfLigth = 114255
            };

            int32 m_Duration = 0;

            void HandleOnPrepare()
            {
                if (Unit* l_Caster = GetCaster())
                if (Aura* l_SurgeOfLight = l_Caster->GetAura(eSpells::SurgeOfLigth))
                    m_Duration = l_SurgeOfLight->GetDuration();
            }

            void HandleOnCast()
            {
                if (Unit* l_Caster = GetCaster())
                if (Aura* l_SurgeOfLight = l_Caster->GetAura(eSpells::SurgeOfLigth))
                {
                    l_SurgeOfLight->SetDuration(m_Duration);
                    if (l_SurgeOfLight->GetStackAmount() > 1)
                        l_SurgeOfLight->SetStackAmount(1);
                }
            }

            void Register()
            {
                OnPrepare += SpellOnPrepareFn(spell_pri_surge_of_light_SpellScript::HandleOnPrepare);
                OnCast += SpellCastFn(spell_pri_surge_of_light_SpellScript::HandleOnCast);
            }
        };

        class spell_pri_surge_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_surge_of_light_AuraScript);

            enum eSpells
            {
                SurgeOfLightVisualUI = 128654
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (!l_Caster->HasAura(eSpells::SurgeOfLightVisualUI))
                    l_Caster->CastSpell(l_Caster, eSpells::SurgeOfLightVisualUI, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::SurgeOfLightVisualUI))
                    l_Caster->RemoveAura(eSpells::SurgeOfLightVisualUI);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_pri_surge_of_light_AuraScript::OnApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_surge_of_light_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_surge_of_light_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_surge_of_light_AuraScript();
        }
};

/// Surge of Light (Discipline, Holy) - 109186
class spell_pri_surge_of_light_aura : public SpellScriptLoader
{
    public:
        spell_pri_surge_of_light_aura() : SpellScriptLoader("spell_pri_surge_of_light_aura") { }

        class spell_pri_surge_of_light_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_surge_of_light_aura_AuraScript);

            enum eSpells
            {
                SurgeOfLigth = 114255
            };

            void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& procInfo)
            {
                PreventDefaultAction();

                if (!procInfo.GetDamageInfo() || !procInfo.GetDamageInfo()->GetAmount() || !procInfo.GetActor())
                    return;

                if (Player* l_Player = procInfo.GetActor()->ToPlayer())
                {
                    if (l_Player->GetActiveSpecializationID() == SPEC_PRIEST_SHADOW)
                        return;

                    if (roll_chance_i(GetSpellInfo()->Effects[EFFECT_0].BasePoints))
                    {
                        if (!CheckProcCooldown())
                            return;

                        if (Aura* l_SurgeOfLight = l_Player->GetAura(eSpells::SurgeOfLigth))
                        {
                            if (l_SurgeOfLight->GetStackAmount() == 2)
                                l_SurgeOfLight->SetDuration(l_SurgeOfLight->GetMaxDuration());
                            else
                                l_Player->CastSpell(l_Player, eSpells::SurgeOfLigth, true);
                        }
                        else
                            l_Player->CastSpell(l_Player, eSpells::SurgeOfLigth, true);
                    }
                }
            }

            void OnUpdate(uint32 p_Diff)
            {
                if (m_ProcCooldown > p_Diff)
                {
                    m_ProcCooldown -= p_Diff;
                    return;
                }
                if (!CheckProcCooldown(true))
                    m_ProcCooldown = 0;
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_surge_of_light_aura_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                OnAuraUpdate += AuraUpdateFn(spell_pri_surge_of_light_aura_AuraScript::OnUpdate);
            }

        private:

            bool CheckProcCooldown(bool fromUpdate = false)
            {
                if (m_ProcCooldown <= 0)
                {
                    if (!fromUpdate)
                        m_ProcCooldown = 0.5 * IN_MILLISECONDS;
                    return true;
                }

                return false;
            }

            uint32 m_ProcCooldown = 0;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_surge_of_light_aura_AuraScript();
        }
};

// Called by Leap of Faith - 73325 and Power Word : Shield - 17
// Body and Soul - 64129
class spell_pri_body_and_soul: public SpellScriptLoader
{
    public:
        spell_pri_body_and_soul() : SpellScriptLoader("spell_pri_body_and_soul") { }

        class spell_pri_body_and_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_body_and_soul_SpellScript);

            enum eSpells
            {
                BodyAndSoul      = 64129,
                BodyAndSoulSpeed = 65081
            };

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        if (_player->HasAura(eSpells::BodyAndSoul))
                            _player->CastSpell(target, eSpells::BodyAndSoulSpeed, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_body_and_soul_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_body_and_soul_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Binding Heal - 32546
class spell_pri_binding_heal : public SpellScriptLoader
{
    public:
        spell_pri_binding_heal() : SpellScriptLoader("spell_pri_binding_heal") { }

        class spell_pri_binding_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_binding_heal_SpellScript);

            enum eSpells
            {
                T21Bonus = 253437
            };

            uint64 m_MainTargetGUID = 0;

            void HandleOnPrepare()
            {
                Unit* l_MainTarget = GetExplTargetUnit();

                if (l_MainTarget == nullptr)
                    return;

                m_MainTargetGUID = l_MainTarget->GetGUID();
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo || p_Targets.empty())
                    return;

                Unit* l_MainTarget = sObjectAccessor->GetUnit(*l_Caster, m_MainTargetGUID);
                if (!l_MainTarget)
                    return;

                std::list<WorldObject*>::iterator l_Iter = std::find(p_Targets.begin(), p_Targets.end(), (WorldObject*)l_MainTarget);
                if (l_Iter != p_Targets.end())
                    p_Targets.erase(l_Iter);

                if (!p_Targets.empty())
                {
                    uint32 l_TargetCounts = l_SpellInfo->Effects[EFFECT_3].BasePoints - 2;
                    p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                    p_Targets.resize(l_TargetCounts);
                }
                p_Targets.push_front(l_MainTarget);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::T21Bonus);
            }

            void Register()
            {
                OnPrepare += SpellOnPrepareFn(spell_pri_binding_heal_SpellScript::HandleOnPrepare);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_binding_heal_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
                AfterCast += SpellCastFn(spell_pri_binding_heal_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_binding_heal_SpellScript();
        }
};

/// last update : 7.3.5 build 26365
/// Power Word: Shield - 17
class spell_pri_power_word_shield : public SpellScriptLoader
{
    public:
        spell_pri_power_word_shield() : SpellScriptLoader("spell_pri_power_word_shield") { }

        enum eSpells
        {
            Cyclone     = 33786
        };

        class spell_pri_power_word_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_power_word_shield_SpellScript);

            SpellCastResult CheckCast()
            {
                if (Unit* l_Target = GetExplTargetUnit())
                {
                    if (l_Target->HasAura(eSpells::Cyclone))
                    {
                        SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_YOU_CANNOT_DO_THAT_RIGHT_NOW);
                        return SPELL_FAILED_CUSTOM_ERROR;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_power_word_shield_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_power_word_shield_SpellScript();
        }

        class spell_pri_power_word_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_power_word_shield_AuraScript);

            int32 m_HealByGlyph = 0;

            enum eSpells
            {
                ShareInTheLight             = 197781,
                ShareInTheLightAbsorb       = 210027,
                StrengthOfSoulAura          = 197535,
                StrengthOfSoulHeal          = 197470,
                StrengthOfSoulBuff          = 197548,
                PainSuppression             = 33206,
                SkjoldrSanctuaryOfIvagont   = 214576,
                ShieldDiscipline            = 197045,
                T19Discipline2P             = 211556,
                PowerWordShield             = 211558,
                AegisofWrath                = 238135,
                ShieldDisciplineRegen       = 47755,
            };

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                p_Amount = ((l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL) * 5.5f) + GetSpellInfo()->Effects[EFFECT_0].BasePoints) * 1;

                if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::AegisofWrath, EFFECT_0))
                    AddPct(p_Amount, l_AurEff->GetAmount());

                if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::SkjoldrSanctuaryOfIvagont, EFFECT_1))
                    AddPct(p_Amount, l_AurEff->GetAmount());
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_AegisSpell = sSpellMgr->GetSpellInfo(eSpells::AegisofWrath);
                if (!l_Caster || !l_Caster->HasAura(eSpells::AegisofWrath) || !l_AegisSpell)
                    return;

                if (AuraEffect* l_FirstEffect = p_AurEff->GetBase()->GetEffect(EFFECT_0))
                {
                    int32 bp = l_FirstEffect->GetAmount();
                    if (bp)
                        l_FirstEffect->SetAmount(bp - CalculatePct(bp, l_AegisSpell->Effects[EFFECT_1].BasePoints));
                }
            }

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();
                if (l_Target == nullptr || l_Caster == nullptr || l_Owner == nullptr)
                    return;

                /// Share in the Light Artifact Trait 1/1
                /// Updated as of 7.0.3 - 22522
                /// Casting Power Word: Shield on a target other than yourself also shields you for 15% as much.
                if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(eSpells::ShareInTheLight, EFFECT_0))
                {
                    if (l_Target->GetGUID() != l_Caster->GetGUID())
                    {
                        int32 l_AbsorbAmount = p_AurEff->GetAmount();
                        l_AbsorbAmount = CalculatePct(l_AbsorbAmount, l_AurEff->GetAmount());

                        /// cast 15% absorb on yourself
                        l_Caster->CastCustomSpell(l_Caster, eSpells::ShareInTheLightAbsorb, &l_AbsorbAmount, nullptr, nullptr, true);
                    }
                }

                l_Target->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, true);

                if (l_Caster->HasAura(eSpells::StrengthOfSoulAura))
                {
                    l_Caster->CastSpell(l_Target, eSpells::StrengthOfSoulBuff, true);
                    l_Caster->CastSpell(l_Target, eSpells::StrengthOfSoulHeal, true);
                }

                Player* l_OwnerPlayer = l_Caster->ToPlayer();
                Player* l_TargetPlayer = l_Target->ToPlayer();

                if (l_OwnerPlayer == nullptr || l_TargetPlayer == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SkjoldrSanctuaryOfIvagont);
                if (l_SpellInfo && l_OwnerPlayer->HasAura(eSpells::SkjoldrSanctuaryOfIvagont))
                    l_OwnerPlayer->ReduceSpellCooldown(eSpells::PainSuppression, l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);

                /// Item - Priest T19 Discipline 2P Bonus
                if (l_Caster->HasAura(eSpells::T19Discipline2P))
                    l_Caster->CastSpell(l_Target, eSpells::PowerWordShield, true);
            }

            void OnRemove(AuraEffect const*  p_AurEff, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Target->RemoveAura(eSpells::StrengthOfSoulBuff);
                l_Target->RemoveAura(eSpells::PowerWordShield);
                l_Target->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, false);

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShieldDiscipline);
                if (!l_SpellInfo || !l_Caster->HasAura(eSpells::ShieldDiscipline))
                    return;

                if (p_AurEff->GetAmount() == 0)
                    l_Caster->CastSpell(l_Caster, eSpells::ShieldDisciplineRegen, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_power_word_shield_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectApplyFn(spell_pri_power_word_shield_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_power_word_shield_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_power_word_shield_AuraScript();
        }
};

// Purify - 527
class spell_pri_purify: public SpellScriptLoader
{
    public:
        spell_pri_purify() : SpellScriptLoader("spell_pri_purify") { }

        class spell_pri_purify_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_purify_SpellScript);

            enum eSpells
            {
                Purify              = 527,
                PurifiedResolve     = 196439,
                PurifiedResolveBuff = 196440,
                SpiritualCleansing  = 213654
            };

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
                                if (l_SpellInfo->Id == eSpells::Purify)
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

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = nullptr;
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target || !(l_Player = l_Caster->ToPlayer()))
                    return;


                if (l_Player->HasAura(eSpells::PurifiedResolve))
                {
                    SpellInfo const* l_PurifiedResolveInfo = sSpellMgr->GetSpellInfo(eSpells::PurifiedResolve);

                    if (l_PurifiedResolveInfo)
                    {
                        int32 l_Absorb = CalculatePct(l_Target->GetMaxHealth(), l_PurifiedResolveInfo->Effects[0].BasePoints);

                        l_Player->CastCustomSpell(l_Target, eSpells::PurifiedResolveBuff, &l_Absorb, nullptr, nullptr, true);
                    }
                }
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

                if (l_Player->HasAura(SpiritualCleansing) && l_Player->HasSpellCooldown(l_SpellInfo->Id))
                    l_Player->RemoveSpellCooldown(l_SpellInfo->Id, 8 * IN_MILLISECONDS);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_purify_SpellScript::CheckCleansing);
                OnCast += SpellCastFn(spell_pri_purify_SpellScript::HandleCast);
                AfterCast += SpellCastFn(spell_pri_purify_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_purify_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Called by Fade - 586
/// Phantasm - 108942
class spell_pri_phantasm: public SpellScriptLoader
{
    public:
        spell_pri_phantasm() : SpellScriptLoader("spell_pri_phantasm") { }

        class spell_pri_phantasm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_phantasm_SpellScript);

            enum eSpells
            {
                PhantasmAura        = 108942,
                PhantasmProc        = 114239
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->HasAura(eSpells::PhantasmAura))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::PhantasmProc, true);
                    l_Caster->RemoveMovementImpairingAuras();
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_phantasm_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_phantasm_SpellScript;
        }
};

// Mind Spike - 73510
class spell_pri_mind_spike: public SpellScriptLoader
{
    public:
        spell_pri_mind_spike() : SpellScriptLoader("spell_pri_mind_spike") { }

        class spell_pri_mind_spike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_spike_SpellScript);

            enum eSpells
            {
                SWPain             = 589,
                VampiricTouch      = 34914,
                SurgeOfDarkness    = 87160,
                SpikeAura          = 217673
            };

            bool m_InstantMindSpike;

            bool Load()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return false;

                if (l_Caster->HasAura(eSpells::SurgeOfDarkness))
                    m_InstantMindSpike = true;
                else
                    m_InstantMindSpike = false;

                return true;
            }

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpikeAura, true);
                if (m_InstantMindSpike)
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SurgeOfDarkness);
                    if (l_SpellInfo)
                        SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), l_SpellInfo->Effects[EFFECT_3].BasePoints));
                }
                else ///< Surge of Darkness - Your next Mind Spike will not consume your damage-over-time effects ...
                {
                    /// Mind Spike remove all DoT on the target's
                    if (l_Target->HasAura(eSpells::SWPain, l_Caster->GetGUID()))
                        l_Target->RemoveAura(eSpells::SWPain, l_Caster->GetGUID());
                    if (l_Target->HasAura(eSpells::VampiricTouch, l_Caster->GetGUID()))
                        l_Target->RemoveAura(eSpells::VampiricTouch, l_Caster->GetGUID());
                }
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                /// If Mind Spike is instant - drop aura charge
                if (m_InstantMindSpike)
                {
                    if (Aura* l_MindSpikeAura = l_Caster->GetAura(eSpells::SurgeOfDarkness))
                        l_MindSpikeAura->DropStack();
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_mind_spike_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnHit += SpellHitFn(spell_pri_mind_spike_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mind_spike_SpellScript;
        }
};

/// Leap of Faith - 73325
class spell_pri_leap_of_faith: public SpellScriptLoader
{
    public:
        spell_pri_leap_of_faith() : SpellScriptLoader("spell_pri_leap_of_faith") { }

        class spell_pri_leap_of_faith_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_leap_of_faith_SpellScript);

            enum eSpells
            {
                LeapOfJump      = 73325,
                LeapOfFaithJump = 97817,
                TrustInTheLightHeal = 196356,
                TrustInTheLight = 196355
            };

            enum eArtifactPowers
            {
                TrustInTheLightArtifact = 841
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Target || !l_Caster)
                    return;

                if (GetSpellInfo()->Id == eSpells::LeapOfJump)
                    l_Target->CastSpell(l_Caster, eSpells::LeapOfFaithJump, true);

                Player* l_Player = l_Caster->ToPlayer();
                Aura * l_TrustInTheLightAura = l_Caster->GetAura(TrustInTheLight);

                if (!l_Player || !l_TrustInTheLightAura)
                    return;

                int32 l_Amount = CalculatePct(l_Target->GetMaxHealth(), (l_TrustInTheLightAura->GetEffect(0)->GetAmount())); ///< % of MaxHealth that will be healed in total

                SpellInfo const* l_TrustHeal = sSpellMgr->GetSpellInfo(eSpells::TrustInTheLightHeal);
                if (!l_TrustHeal)
                    return;

                uint32 l_TrustHealAmp = l_TrustHeal->Effects[EFFECT_0].Amplitude;
                uint32 l_NbTicks = l_TrustHeal->GetMaxDuration() / l_TrustHealAmp;
                l_Amount /= l_NbTicks;

                l_Player->CastCustomSpell(l_Target, eSpells::TrustInTheLightHeal, &l_Amount, NULL, NULL, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_leap_of_faith_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_leap_of_faith_SpellScript;
        }
};

/// Psychic Horror - 64044
class spell_pri_psychic_horror: public SpellScriptLoader
{
    public:
        spell_pri_psychic_horror() : SpellScriptLoader("spell_pri_psychic_horror") { }

        class spell_pri_psychic_horror_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_psychic_horror_SpellScript);

            enum eSpells
            {
                PvP4PBonusShadow       = 171151,
                PvP4PBonusShadowEffect = 171153,
                PsychicHorror          = 64044
            };

            bool Load()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return false;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PsychicHorrorGainedPower) = false;

                return true;
            }

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        Player* l_Player = l_Caster->ToPlayer();
                        if (l_Player == nullptr)
                            return;

                        bool& l_PsychicHorrorGainedPower = l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PsychicHorrorGainedPower);
                        if (!l_PsychicHorrorGainedPower && l_Player->GetActiveSpecializationID() == SPEC_PRIEST_SHADOW)
                        {
                            /// +1s per Shadow Orb consumed
                            if (Aura* l_PsychicHorror = l_Target->GetAura(eSpells::PsychicHorror))
                            {
                                l_PsychicHorrorGainedPower = true;
                                int32 l_CurrentPowerUsed = l_Caster->GetPower(POWER_INSANITY) + 1;
                                if (l_CurrentPowerUsed > 3) ///< Maximum 3 Shadow Orb can be consumed (1 of them is base spell cost)
                                    l_CurrentPowerUsed = 3;
                                l_Caster->ModifyPower(POWER_INSANITY, -(l_CurrentPowerUsed - 1));

                                int32 l_MaxDuration = l_PsychicHorror->GetMaxDuration();
                                int32 l_NewDuration = l_MaxDuration + (GetSpellInfo()->Effects[EFFECT_0].BasePoints + l_CurrentPowerUsed) * IN_MILLISECONDS;
                                l_PsychicHorror->SetDuration(l_NewDuration);

                                if (l_NewDuration > l_MaxDuration)
                                    l_PsychicHorror->SetMaxDuration(l_NewDuration);

                                if (l_Caster->HasAura(eSpells::PvP4PBonusShadow))
                                {
                                    l_Caster->CastSpell(l_Caster, eSpells::PvP4PBonusShadowEffect, true);

                                    /// Shadow Orbs spent on Psychic Horror are refunded over 3 seconds.
                                    /// For one spent shadow orb - 3 seconds of duration, maximum 3 shadow orbs - so it's 9 seconds
                                    int32 l_SpecialDuration = (l_CurrentPowerUsed) * 3000;
                                    if (Aura* l_ShadowPvpBonusEffect = l_Caster->GetAura((eSpells::PvP4PBonusShadowEffect), l_Caster->GetGUID()))
                                    {
                                        l_ShadowPvpBonusEffect->SetMaxDuration(l_SpecialDuration);
                                        l_ShadowPvpBonusEffect->SetDuration(l_SpecialDuration);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_psychic_horror_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_psychic_horror_SpellScript();
        }
};

/// Update to legion 7.1.5 build 23420
/// Call by Guardian Spirit - 47788
class spell_pri_guardian_spirit: public SpellScriptLoader
{
    public:
        spell_pri_guardian_spirit() : SpellScriptLoader("spell_pri_guardian_spirit") { }

        class spell_pri_guardian_spirit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_guardian_spirit_AuraScript);

            enum eSpells
            {
                GuardianSpiritHeal      = 48153,
                PhyrixsEmbrace          = 234689,
                GuardiansOfTheLight     = 196437,
                GuardiansOfTheLightProc = 196463,
                GuardianAngel           = 200209,
                GuardianSpirit          = 47788
            };

            bool m_AlreadySaved = false;
            bool m_Spirit = false;

            bool Validate(SpellInfo const* /*spellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::GuardianSpiritHeal))
                    return false;

                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& p_Amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                p_Amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Owner = GetUnitOwner();

                if (!l_Target || !l_Owner)
                    return;

                if (p_DmgInfo.GetAmount() < l_Target->GetHealth() || m_AlreadySaved)
                {
                    PreventDefaultAction();
                    return;
                }

                p_AbsorbAmount = p_DmgInfo.GetAmount();

                if (!l_Owner->HasAura(eSpells::PhyrixsEmbrace))
                    Remove(AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL); ///< remove the aura now, we don't want 40% healing bonus

                l_Owner->m_SpellHelper.GetBool(eSpellHelpers::GuardianSpirit) = true;
                l_Target->m_SpellHelper.GetBool(eSpellHelpers::GuardianSpirit) = true;
                int32 l_Amount = int32(l_Target->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints));
                l_Target->CastCustomSpell(l_Target, eSpells::GuardianSpiritHeal, &l_Amount, nullptr, nullptr, true);
            }

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster != l_Target && !l_Caster->HasAura(eSpells::GuardiansOfTheLightProc) && l_Caster->HasAura(eSpells::GuardiansOfTheLight))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::GuardiansOfTheLightProc, true);
                    l_Target->AddAura(GetId(), l_Caster);
                    l_Target->m_SpellHelper.GetBool(eSpellHelpers::GuardianSpirit) = false;
                }

                if (l_Caster->HasAura(eSpells::GuardianAngel))
                    l_Caster->m_SpellHelper.GetBool(eSpellHelpers::GuardianSpirit) = false;
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster == l_Target)
                    l_Caster->RemoveAurasDueToSpell(eSpells::GuardiansOfTheLightProc);

                if (!l_Caster->HasAura(eSpells::GuardianAngel))
                    return;

                uint64 l_TargetGuid = l_Target->GetGUID();
                l_Caster->m_Functions.AddFunction([l_Caster, l_TargetGuid]() -> void
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGuid);
                    if (l_Target && l_Caster && !l_Caster->m_SpellHelper.GetBool(eSpellHelpers::GuardianSpirit) && !l_Target->m_SpellHelper.GetBool(eSpellHelpers::GuardianSpirit))
                    {
                        Player* l_Player = l_Caster->ToPlayer();
                        if (!l_Player)
                            return;

                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GuardianAngel);
                        if (!l_SpellInfo || !l_Player->HasAura(eSpells::GuardianAngel))
                            return;

                        SpellCooldowns l_Cooldowns = l_Player->GetSpellCooldowns();
                        SpellCooldown l_GuardianSpiritCooldown = l_Cooldowns[eSpells::GuardianSpirit];

                        ACE_UINT64 currTime = 0;
                        ACE_OS::gettimeofday().msec(currTime);
                        int32 l_Cooldown = l_GuardianSpiritCooldown.end - currTime;

                        int32 l_NewCooldown = -(l_SpellInfo->Effects[EFFECT_0].BasePoints * TimeConstants::IN_MILLISECONDS - l_Cooldown);
                        if (l_GuardianSpiritCooldown.end)
                            l_Player->ModifySpellCooldown(eSpells::GuardianSpirit, l_NewCooldown);
                    }
                }, 400);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit_AuraScript::Absorb, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_pri_guardian_spirit_AuraScript::HandleApply, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_guardian_spirit_AuraScript::HandleRemove, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_guardian_spirit_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Penance - 47540
class spell_pri_penance: public SpellScriptLoader
{
    public:
        spell_pri_penance() : SpellScriptLoader("spell_pri_penance") { }

        class spell_pri_penance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penance_SpellScript);

            enum eSpells
            {
                Penance                 = 47540,
                PenanceHeal             = 47757,
                PenanceDamage           = 47758,
                ThePenitent             = 200347
            };

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* spellEntry)
            {
                if (!sSpellMgr->GetSpellInfo(eSpells::Penance))
                    return false;
                // can't use other spell than this penance due to spell_ranks dependency
                if (sSpellMgr->GetFirstSpellInChain(eSpells::Penance) != sSpellMgr->GetFirstSpellInChain(spellEntry->Id))
                    return false;

                uint8 l_Rank = sSpellMgr->GetSpellRank(spellEntry->Id);
                if (!sSpellMgr->GetSpellWithRank(eSpells::PenanceDamage, l_Rank, true))
                    return false;
                if (!sSpellMgr->GetSpellWithRank(eSpells::PenanceHeal, l_Rank, true))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Unit* l_UnitTarget = GetHitUnit())
                    {
                        if (!l_UnitTarget->isAlive())
                            return;

                        uint8 l_Rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);

                        if (l_Player->IsFriendlyTo(l_UnitTarget))
                            l_Player->CastSpell(l_UnitTarget, sSpellMgr->GetSpellWithRank(eSpells::PenanceHeal, l_Rank), false, 0);
                        else
                            l_Player->CastSpell(l_UnitTarget, sSpellMgr->GetSpellWithRank(eSpells::PenanceDamage , l_Rank), false, 0);

                    }

                    l_Player->m_SpellHelper.GetBool(eSpellHelpers::PenanceT20BoltAllowed) = true;
                }
            }

            SpellCastResult CheckCast()
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                if (Unit* l_Target = GetExplTargetUnit())
                    if ((!l_Caster->IsFriendlyTo(l_Target) || !l_Caster->HasAura(eSpells::ThePenitent)) && !l_Caster->IsValidAttackTarget(l_Target))
                        return SPELL_FAILED_BAD_TARGETS;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                // add dummy effect spell handler to Penance
                OnEffectHitTarget += SpellEffectFn(spell_pri_penance_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_pri_penance_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_penance_SpellScript;
        }
};

/// Last Update 7.1.5
/// Vestments of Discipline - 197711
class spell_pri_vestments_of_discipline : public SpellScriptLoader
{
    public:
        spell_pri_vestments_of_discipline() : SpellScriptLoader("spell_pri_vestments_of_discipline") { }

        class spell_pri_vestments_of_discipline_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vestments_of_discipline_AuraScript);

            enum eSpells
            {
                VestmentsofDiscipline = 216135
            };

            enum eTraits
            {
                VestmentsofDisciplineTrait = 889
            };

            void HandleProc(ProcEventInfo&)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                int32 l_Amount = GetAura()->GetEffect(SpellEffIndex::EFFECT_0)->GetAmount();

                l_Player->CastCustomSpell(l_Player, eSpells::VestmentsofDiscipline, &l_Amount, NULL, NULL, true);
            }

            void Register()
            {
                OnProc += AuraProcFn(spell_pri_vestments_of_discipline_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_vestments_of_discipline_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Vampiric Touch - 34914
class spell_pri_vampiric_touch: public SpellScriptLoader
{
    public:
        spell_pri_vampiric_touch() : SpellScriptLoader("spell_pri_vampiric_touch") { }

        class spell_pri_vampiric_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_touch_AuraScript);

            enum eSpells
            {
                SurgeOfDarkness     = 87160,
                SurgeOfDarknessAura = 162448,
                SinAndPunishment    = 87204,
                ShadowWordPain      = 589
            };

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Dispeller = dispelInfo->GetDispeller())
                        l_Dispeller->CastSpell(l_Dispeller, eSpells::SinAndPunishment, true, 0, nullptr, l_Caster->GetGUID());
                }
            }

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (l_Caster && l_Target)
                     l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::VampiricTouch].insert(l_Target->GetGUID());
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (l_Caster && l_Target)
                    l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::VampiricTouch].erase(l_Target->GetGUID());
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_vampiric_touch_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_vampiric_touch_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_pri_vampiric_touch_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_vampiric_touch_AuraScript();
        }

        class spell_pri_vampiric_touch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_vampiric_touch_SpellScript);

            enum eSpells
            {
                ShadowWordPain  = 589,
                Misery          = 238558
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowWordPain);
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::Misery))
                {
                    int32 l_AddDuration = 0;

                    if (Aura* l_Aura = l_Target->GetAura(eSpells::ShadowWordPain, l_Caster->GetGUID()))
                    {
                        int32 l_DurationRule = CalculatePct(l_Aura->GetSpellInfo()->GetMaxDuration(), 30);
                        if (l_Aura->GetDuration() <= l_DurationRule)
                            l_AddDuration = l_Aura->GetDuration();
                    }

                    l_Caster->AddAura(eSpells::ShadowWordPain, l_Target);

                    Aura* l_Aura = l_Target->GetAura(eSpells::ShadowWordPain, l_Caster->GetGUID());
                    if (!l_Aura)
                        return;

                    l_Aura->SetMaxDuration(l_SpellInfo->GetMaxDuration() + l_AddDuration);
                    l_Aura->SetDuration(l_SpellInfo->GetMaxDuration() + l_AddDuration);
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pri_vampiric_touch_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_vampiric_touch_SpellScript();
        }
};

// Call by Flash Heal 2061 - Heal 2060 - Prayer of healing 596
// Spirit Shell - 109964
class spell_pri_spirit_shell: public SpellScriptLoader
{
    public:
        spell_pri_spirit_shell() : SpellScriptLoader("spell_pri_spirit_shell") { }

        class spell_pri_spirit_shell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_spirit_shell_SpellScript);

            enum eSpells
            {
                SpiritShellAura = 109964,
                SpiritShellProc = 114908,
                PrayerOfHealing = 596
            };

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                if (l_Caster->GetAura(eSpells::SpiritShellAura))
                {
                    int32 l_Absorb = GetHitHeal();
                    l_Caster->CastCustomSpell(GetHitUnit(), eSpells::SpiritShellProc, &l_Absorb, NULL, NULL, true);
                    SetHitHeal(0);
                }
            }

            void Register()
            {
                if (m_scriptSpellId == eSpells::PrayerOfHealing)
                    OnEffectHitTarget += SpellEffectFn(spell_pri_spirit_shell_SpellScript::HandleHeal, EFFECT_1, SPELL_EFFECT_HEAL);
                else
                    OnEffectHitTarget += SpellEffectFn(spell_pri_spirit_shell_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_spirit_shell_SpellScript();
        }
};

/// Last Update 6.1.2 19802
/// Spirit Shell - 114908
class spell_pri_spirit_shell_effect : public SpellScriptLoader
{
    public:
        spell_pri_spirit_shell_effect() : SpellScriptLoader("spell_pri_spirit_shell_effect") { }

        class spell_pri_spirit_shell_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_spirit_shell_effect_SpellScript);

            int32 m_AmountPreviousShield = 0;

            void HandleBeforeHit()
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (AuraEffect const* l_PreviousShield = l_Target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                    m_AmountPreviousShield = l_PreviousShield->GetAmount();
            }

            void HandleAfterHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Player == nullptr || l_Target == nullptr)
                    return;

                if (AuraEffect* l_Shield = l_Target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                {
                    int32 l_Bp = m_AmountPreviousShield + l_Shield->GetAmount();

                    int32 l_MaxStackAmount = CalculatePct(l_Player->GetMaxHealth(), 60); ///< Stack up to a maximum of 60% of the casting Priest's health

                    if (l_Bp > l_MaxStackAmount)
                        l_Bp = l_MaxStackAmount;

                    l_Shield->SetAmount(l_Bp);
                }
            }

            void Register()
            {
                BeforeHit += SpellHitFn(spell_pri_spirit_shell_effect_SpellScript::HandleBeforeHit);
                AfterHit += SpellHitFn(spell_pri_spirit_shell_effect_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_spirit_shell_effect_SpellScript();
        }
};

// Flash heal - 2061
class spell_pri_flash_heal: public SpellScriptLoader
{
    public:
        spell_pri_flash_heal() : SpellScriptLoader("spell_pri_flash_heal") { }

        class spell_pri_flash_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_flash_heal_SpellScript);

            enum eSpells
            {
                SurgeOfLigth = 114255,
                T21Holy2P    = 253437
            };

            void HandleBeforeCast()
            {
                if (Unit* l_Caster = GetCaster())
                    if (Aura* surgeOfLight = l_Caster->GetAura(eSpells::SurgeOfLigth))
                        surgeOfLight->ModStackAmount(-1);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;
                l_Caster->RemoveAura(eSpells::T21Holy2P);
            }

            void Register()
            {
                BeforeCast += SpellCastFn(spell_pri_flash_heal_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_pri_flash_heal_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_flash_heal_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Clarity of will - 152118
class spell_pri_clarity_of_will: public SpellScriptLoader
{
    public:
        spell_pri_clarity_of_will() : SpellScriptLoader("spell_pri_clarity_of_will") { }

        enum eSpells
        {
            Atonement = 194384
        };

        class spell_pri_clarity_of_will_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_clarity_of_will_SpellScript);

            int32 m_AmountPreviousShield = 0;

            void HandleBeforeHit()
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (AuraEffect const* l_PreviousShield = l_Target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                    m_AmountPreviousShield = l_PreviousShield->GetAmount();
            }

            void HandleAfterHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();
                Aura* l_Aura = GetHitAura();

                if (!l_Player || !l_Target || !l_Aura)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                if (AuraEffect* l_Shield = l_Target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0, l_Player->GetGUID()))
                {
                    int32 l_Amount = l_Shield->GetAmount();

                    if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::Atonement, EFFECT_1, l_Player->GetGUID()))
                        AddPct(l_Amount, l_AuraEffect->GetAmount());

                    int32 l_Bp = m_AmountPreviousShield + l_Amount;

                    int32 l_MaxStackAmount = l_Amount * 2; ///< Stack up to a maximum of 200% of Clarity of will BP

                    if (l_Bp > l_MaxStackAmount)
                        l_Bp = l_MaxStackAmount;

                    l_Shield->ChangeAmount(l_Bp);
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_pri_clarity_of_will_SpellScript::HandleBeforeHit);
                AfterHit += SpellHitFn(spell_pri_clarity_of_will_SpellScript::HandleAfterHit);
            }
        };

        class spell_pri_clarity_of_will_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_clarity_of_will_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Amount = int32(l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL) * 9.0f);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_clarity_of_will_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_clarity_of_will_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_clarity_of_will_SpellScript();
        }
};

// Angelic Feather - 121536
class spell_pri_angelic_feather: public SpellScriptLoader
{
    public:
        spell_pri_angelic_feather() : SpellScriptLoader("spell_pri_angelic_feather") {}

        class spell_pri_angelic_feather_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_angelic_feather_SpellScript);

            enum eSpells
            {
                AngelicFeather = 158624
            };

            void HandleOnCast()
            {
                if (Unit* l_Caster = GetCaster())
                    if (WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest()))
                        l_Caster->CastSpell(l_Dest->GetPositionX(), l_Dest->GetPositionY(), l_Dest->GetPositionZ(), eSpells::AngelicFeather, true);
            }

            void Register()
            {
                OnCast += SpellCastFn(spell_pri_angelic_feather_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_angelic_feather_SpellScript();
        }
};

/// Shadow Word: Pain - 589
class spell_pri_shadow_word_pain: public SpellScriptLoader
{
    public:
        spell_pri_shadow_word_pain() : SpellScriptLoader("spell_pri_shadow_word_pain") { }

        class spell_pri_shadow_word_pain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadow_word_pain_AuraScript);

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetUnitOwner())
                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::ShadowWordPainTargets].insert(l_Target->GetGUID());
                }
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetUnitOwner())
                        l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::ShadowWordPainTargets].erase(l_Target->GetGUID());
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_pri_shadow_word_pain_AuraScript::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_pri_shadow_word_pain_AuraScript::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_shadow_word_pain_AuraScript();
        }

        class spell_pri_shadow_word_pain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_word_pain_SpellScript);

            enum eSpells
            {
                ShadowWordPain      = 589,
                AnundLastBreath     = 215209,
                AnundLastBreathBuff = 215210,
                Voidform            = 194249
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (l_Caster->HasAura(eSpells::AnundLastBreath))
                    l_Caster->CastSpell(l_Caster, eSpells::AnundLastBreathBuff, true);
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Damage = GetHitDamage();
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Voidform, EFFECT_9))
                    AddPct(l_Damage, l_AuraEffect->GetAmount());
                SetHitDamage(l_Damage);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pri_shadow_word_pain_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_pri_shadow_word_pain_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_shadow_word_pain_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Mind Blast - 8092
class spell_pri_mind_blast: public SpellScriptLoader
{
    public:
        spell_pri_mind_blast() : SpellScriptLoader("spell_pri_mind_blast") { }

        enum eSpells
        {
            GlyphOfMindSpike    = 81292,
            MindBlast           = 8092,
            SpikeAura           = 217673,
            MindSpikeDetonate   = 217676,
            ShadowyInsightBuff  = 124430,
            EmptyMind           = 247226
        };

        class spell_pri_mind_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_blast_SpellScript);

            bool m_AlreadyReduceCooldown = false;

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                 l_Caster->RemoveAura(eSpells::ShadowyInsightBuff);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (l_Player && l_Player->HasAura(eSpells::ShadowyInsightBuff))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MindBlast))
                    {
                        if (SpellCategoryEntry const* l_CategoryEntry = l_SpellInfo->ChargeCategoryEntry)
                        {
                            l_Player->RestoreCharge(l_CategoryEntry);
                        }
                        else
                        {
                            l_Player->RemoveSpellCooldown(eSpells::MindBlast, true);
                        }
                    }
                }
            }

            void HandleAfterHit()
            {
                if (m_AlreadyReduceCooldown)
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Target = GetHitUnit();

                if (l_Player == nullptr || l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eSpells::SpikeAura, l_Player->GetGUID()))
                {
                    l_Target->RemoveAura(eSpells::SpikeAura, l_Player->GetGUID());
                    l_Player->CastSpell(l_Target, eSpells::MindSpikeDetonate, true);
                }
                if (l_Player->HasSpellCooldown(GetSpellInfo()->Id) && l_Player->GetHastePct() > 0.0f)
                {
                    int32 l_ReduceCooldown = CalculatePct(GetSpellInfo()->RecoveryTime, l_Player->GetHastePct());
                    l_Player->ReduceSpellCooldown(GetSpellInfo()->Id, l_ReduceCooldown);

                    m_AlreadyReduceCooldown = true;
                }

                if (Aura* l_Aura = l_Player->GetAura(eSpells::EmptyMind))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EmptyMind))
                    {
                        l_Player->ModifyPower(POWER_INSANITY, l_SpellInfo->Effects[EFFECT_1].BasePoints * l_Aura->GetStackAmount());
                        l_Aura->Remove();
                    }
                }
            }

            void Register()
            {
                OnPrepare += SpellOnPrepareFn(spell_pri_mind_blast_SpellScript::HandleOnPrepare);
                AfterCast += SpellCastFn(spell_pri_mind_blast_SpellScript::HandleAfterCast);
                AfterHit += SpellHitFn(spell_pri_mind_blast_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mind_blast_SpellScript();
        }
};

/// Legion 7.3.5 build 25996
/// Twist of Fate - 109142
class spell_pri_twist_of_fate : public SpellScriptLoader
{
    public:
        spell_pri_twist_of_fate() : SpellScriptLoader("spell_pri_twist_of_fate") { }

        class spell_pri_twist_of_fate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_twist_of_fate_AuraScript);

            enum eSpells
            {
                Penance = 47666
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return false;

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo)
                    return false;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if ((l_Player->GetActiveSpecializationID() == SPEC_PRIEST_SHADOW && l_ProcSpellInfo->IsPositive()) ||
                    (l_Player->GetActiveSpecializationID() != SPEC_PRIEST_SHADOW && !l_ProcSpellInfo->IsPositive()) || (l_ProcSpellInfo->Id == eSpells::Penance))
                    return false;

                Unit* l_Target = p_ProcEventInfo.GetProcTarget();
                if (l_Target == nullptr)
                    return false;

                /// DoCheckProc is called before the damage is done or after the heal is done so we need to make some calculations
                bool l_CheckProc = l_Player->GetActiveSpecializationID() == SPEC_PRIEST_SHADOW ? ///< It means a damage effect
                    ((l_Target->IsFullHealth() && l_Target->GetHealth() <= l_DamageInfo->GetAmount()) || ///< Oneshot
                        l_Target->HealthBelowPctDamaged(l_SpellInfo->Effects[EFFECT_0].BasePoints, l_DamageInfo->GetAmount())) :
                        (l_Target->GetHealth() - l_DamageInfo->GetAmount()) < l_Target->CountPctFromMaxHealth(l_SpellInfo->Effects[EFFECT_0].BasePoints); /// Check target's health before the heal

                if (!l_CheckProc)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_twist_of_fate_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_twist_of_fate_AuraScript();
        }
};

/// Shadowy Apparition - 148859
class spell_pri_shadowy_apparition : public SpellScriptLoader
{
    public:
        spell_pri_shadowy_apparition() : SpellScriptLoader("spell_pri_shadowy_apparition") { }

        class spell_pri_shadowy_apparition_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadowy_apparition_SpellScript);

            enum eShadowyApparitionSpell
            {
                AuspiciousSpirits = 155271
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Caster && l_Target && l_Target != l_Caster && l_Caster->HasAura(eShadowyApparitionSpell::AuspiciousSpirits))
                    l_Caster->ModifyPower(POWER_INSANITY, 1); ///< Grant you 1 Shadow Orb.

                if (l_Target == l_Caster)
                    SetHitDamage(0);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_shadowy_apparition_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_shadowy_apparition_SpellScript();
        }
};

/// last update : 7.3.2 build 25549
/// Dispersion - 47585
class spell_pri_dispersion : public SpellScriptLoader
{
    public:
        spell_pri_dispersion() : SpellScriptLoader("spell_pri_dispersion") { }

        class spell_pri_dispersion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_dispersion_AuraScript);

            enum eSpells
            {
                PvP2PShadowBonus = 171146,
                ShadowPower      = 171150,
                PureShadow       = 199131,
                PureShadowBuff   = 219772,
                Shadowform       = 232698,
                Voidform         = 194249
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveMovementImpairingAuras();
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Shadowform))
                {
                    if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0))
                    {
                        ShadowformAmount[0] = l_AuraEffect->GetAmount();
                        l_AuraEffect->SetAmount(0);
                    }
                    if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_1))
                    {
                        ShadowformAmount[1] = l_AuraEffect->GetAmount();
                        l_AuraEffect->SetAmount(0);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                /// Item - Priest WoD PvP Shadow 2P Bonus - 171146
                if (l_Caster->HasAura(eSpells::PvP2PShadowBonus))
                    l_Caster->CastSpell(l_Caster, eSpells::ShadowPower, true);

                if (l_Caster->HasAura(eSpells::PureShadow))
                    l_Caster->CastSpell(l_Caster, eSpells::PureShadowBuff, true);

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Shadowform))
                {
                    if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0))
                        l_AuraEffect->SetAmount(ShadowformAmount[0]);
                    if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_1))
                        l_AuraEffect->SetAmount(ShadowformAmount[1]);
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_pri_dispersion_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_dispersion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            int32 ShadowformAmount[2];
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_dispersion_AuraScript();
        }
};

/// Dominate Mind - 605
/// last update: 19865
class spell_pri_dominate_mind : public SpellScriptLoader
{
    public:
        spell_pri_dominate_mind() : SpellScriptLoader("spell_pri_dominate_mind") { }

        class spell_pri_dominate_mind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_dominate_mind_SpellScript);

            SpellCastResult CheckTarget()
            {
                if (Unit* l_Target = GetExplTargetUnit())
                {
                    if (l_Target->ToCreature() && l_Target->ToCreature()->isWorldBoss())
                        return SpellCastResult::SPELL_FAILED_IMMUNE;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_dominate_mind_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_dominate_mind_SpellScript();
        }
};

/// Last Update 6.2.3
/// Focused Will - 45243
class spell_pri_focused_will : public SpellScriptLoader
{
    public:
        spell_pri_focused_will() : SpellScriptLoader("spell_pri_focused_will") { }

        class spell_pri_focused_will_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_focused_will_AuraScript);

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (!p_EventInfo.GetDamageInfo())
                    return;

                /// Should proc only from damage
                if (p_EventInfo.GetDamageInfo()->GetAmount() == 0)
                    return;

                if (p_EventInfo.GetActor()->GetGUID() == l_Caster->GetGUID())
                    PreventDefaultAction();
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_focused_will_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_focused_will_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Penance (heal) - 47757, Penance (damage) - 47758
 class spell_pri_penance_aura : public SpellScriptLoader
{
    public:
        spell_pri_penance_aura() : SpellScriptLoader("spell_pri_penance_aura") { }

        class spell_pri_penance_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_penance_aura_AuraScript);

            enum eSpells
            {
                PriestWoDPvPDiscipline2PBonus   = 171124,
                BonusHeal                       = 171130,
                BonusDamage                     = 171131,
                SpeedofthePious                 = 197766,
                SpeedofthePiousAura             = 197767,
                PowerOfTheDarkSide              = 198069,
                PenanceHeal                     = 47757,
                PenanceDamage                   = 47758,
                XalanTheFearedsClench           = 214620,
                Atonement                       = 194384,
                RadiantFocus                    = 252848
            };

            bool m_PowerOfTheDarkSide = false;

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster == nullptr)
                    return;

                /// Needs to be handled at apply as well, OnTick is called only twice for three ticks
                AuraEffect* l_XalanEffect = l_Caster->GetAuraEffect(eSpells::XalanTheFearedsClench, SpellEffIndex::EFFECT_0);

                switch (m_scriptSpellId)
                {
                    case eSpells::PenanceHeal:
                    {
                        if (l_XalanEffect)
                        {
                            if (Aura* l_AtonementAura = l_Target->GetAura(eSpells::Atonement))
                                l_AtonementAura->SetDuration(l_AtonementAura->GetDuration() + l_XalanEffect->GetBaseAmount() * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eSpells::PenanceDamage:
                    {
                        if (l_XalanEffect)
                        {
                            if (Aura* l_AtonementAura = l_Caster->GetAura(eSpells::Atonement))
                                l_AtonementAura->SetDuration(l_AtonementAura->GetDuration() + l_XalanEffect->GetBaseAmount() * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    default:
                        break;
                }

                if (l_Caster->HasAura(eSpells::PriestWoDPvPDiscipline2PBonus))
                {
                    if (l_Target->IsFriendlyTo(l_Caster))
                        l_Caster->CastSpell(l_Target, eSpells::BonusHeal, true);
                    else
                        l_Caster->CastSpell(l_Target, eSpells::BonusDamage, true);
                }
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::SpeedofthePious, EFFECT_0))
                {
                    int32 l_Amount = l_AuraEffect->GetAmount();
                    l_Caster->CastCustomSpell(l_Caster, eSpells::SpeedofthePiousAura, &l_Amount, NULL, NULL, true);
                }

                if (l_Caster->HasAura(eSpells::PowerOfTheDarkSide))
                    m_PowerOfTheDarkSide = true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (l_Caster == nullptr)
                    return;

                if (l_Target->HasAura(eSpells::BonusHeal, l_Caster->GetGUID()))
                    l_Target->RemoveAura(eSpells::BonusHeal, l_Caster->GetGUID());

                if (l_Target->HasAura(eSpells::BonusDamage, l_Caster->GetGUID()))
                    l_Target->RemoveAura(eSpells::BonusDamage, l_Caster->GetGUID());

                if (l_Caster->HasAura(eSpells::PowerOfTheDarkSide) && m_PowerOfTheDarkSide == true)
                    l_Caster->RemoveAura(eSpells::PowerOfTheDarkSide);

                l_Caster->RemoveAura(eSpells::RadiantFocus);
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                AuraEffect* l_XalanEffect = l_Caster->GetAuraEffect(eSpells::XalanTheFearedsClench, SpellEffIndex::EFFECT_0);

                switch (m_scriptSpellId)
                {
                    case eSpells::PenanceHeal:
                    {
                        if (l_XalanEffect)
                        {
                            if (Aura* l_AtonementAura = l_Target->GetAura(eSpells::Atonement))
                                l_AtonementAura->SetDuration(l_AtonementAura->GetDuration() + l_XalanEffect->GetBaseAmount() * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eSpells::PenanceDamage:
                    {
                        if (l_XalanEffect)
                        {
                            if (Aura* l_AtonementAura = l_Caster->GetAura(eSpells::Atonement))
                                l_AtonementAura->SetDuration(l_AtonementAura->GetDuration() + l_XalanEffect->GetBaseAmount() * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_pri_penance_aura_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_penance_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_penance_aura_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_penance_aura_AuraScript();
        }
};

//////////////////////////////////////////////
////////////////              ////////////////
////////////////    LEGION    ////////////////
////////////////              ////////////////
//////////////////////////////////////////////

/// Last Update 7.0.3 Build 22293
/// Called by Shadow Mend - 187464
class spell_pri_shadow_mend : public SpellScriptLoader
{
    public:
        spell_pri_shadow_mend() : SpellScriptLoader("spell_pri_shadow_mend") { }

        class spell_pri_shadow_mend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadow_mend_AuraScript);

            enum eSpells
            {
                Masochism           = 193063,
                TamingTheShadows    = 197779
            };

            int32 m_DamageTaken = 0;

            void HandleOnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Caster)
                    return;

                if (p_AuraEffect->GetAmount() == 0)
                {
                    Aura* l_Aura = p_AuraEffect->GetBase();
                    l_Aura->SetDuration(0);
                    return;
                }

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::TamingTheShadows, EFFECT_0))
                {
                    if (roll_chance_i(l_AuraEffect->GetAmount()))
                    {
                        PreventDefaultAction();
                        return;
                    }
                }

                if (!l_Target->isInCombat())
                {
                    l_Target->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                    PreventDefaultAction();
                    return;
                }

                if (l_Caster == l_Target && l_Caster->HasAura(eSpells::Masochism))
                {
                    PreventDefaultAction();
                    int32 l_Heal = p_AuraEffect->GetAmount();
                    l_Caster->HealBySpell(l_Caster, GetSpellInfo(), l_Heal);
                }

                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                AuraEffect* l_AuraEffectDot = l_Aura->GetEffect(EFFECT_0);
                AuraEffect* l_AuraEffectMaxDamage = l_Aura->GetEffect(EFFECT_1);
                if (!l_AuraEffectDot || !l_AuraEffectMaxDamage)
                    return;

                int32 l_DotDamage = l_AuraEffectDot->GetBaseAmount();
                int32 l_MaxDamage = l_DotDamage * 10;

                if (l_DotDamage + m_DamageTaken > l_MaxDamage)
                    l_AuraEffectDot->SetAmount(l_MaxDamage - m_DamageTaken);

            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                Aura* l_Aura = GetAura();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_DamageInfo || !l_Target || !l_Aura || !l_Caster || !l_SpellInfo
                    || !l_DamageInfo->GetSpellInfo() || l_DamageInfo->GetSpellInfo()->IsHealingSpell())
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);

                if (!l_AuraEffect || l_SpellInfo->Id == l_DamageInfo->GetSpellInfo()->Id)
                    return;

                int32 l_MaxDamage = l_AuraEffect->GetBaseAmount() * 10;
                int32 l_Damage = l_DamageInfo->GetAmount();

                m_DamageTaken += l_Damage;

                if (m_DamageTaken >= l_MaxDamage)
                    l_Target->RemoveAurasDueToSpell(l_SpellInfo->Id, l_Caster->GetGUID());
            }

            void SetData(uint32 /*l_Data*/, uint32 /*l_Index*/) override
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ShadowMend) = m_DamageTaken;
            }

            bool HandleDoCheckProc(ProcEventInfo& eventInfo)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return false;

                return true;
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pri_shadow_mend_AuraScript::HandleOnProc);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_shadow_mend_AuraScript::HandleOnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                DoCheckProc += AuraCheckProcFn(spell_pri_shadow_mend_AuraScript::HandleDoCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_shadow_mend_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Shadow Mend - 186263
class spell_pri_shadow_mend_heal : public SpellScriptLoader
{
    public:
        spell_pri_shadow_mend_heal() : SpellScriptLoader("spell_pri_shadow_mend_heal") { }

        class spell_pri_shadow_mend_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_mend_heal_SpellScript);

            enum eSpells
            {
                ShadowMendDot = 187464,
                Masochism = 193063,
                MasochismHeal = 193065
            };

            int32 m_MaxDamage = 0;
            int32 m_DamageLeft = 0;

            void HandleBeforeCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowMendDot);
                if (!l_Target || !l_Caster || !l_SpellInfo)
                    return;

                m_MaxDamage = l_Target->GetMaxHealth() - l_Target->GetHealth();

                if (Aura* l_Aura = l_Target->GetAura(eSpells::ShadowMendDot, l_Caster->GetGUID()))
                {
                    l_Aura->SetScriptData(0, 0);
                    int32 l_DamageTaken = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ShadowMend);
                    int32 l_DamageByTick = 0;
                    int32 l_MaxDamage = 0;
                    AuraEffect* l_AuraEffect = nullptr;
                    if ((l_AuraEffect = l_Aura->GetEffect(EFFECT_1)))
                        l_MaxDamage = l_AuraEffect->GetAmount();

                    if ((l_AuraEffect = l_Aura->GetEffect(EFFECT_0)))
                        l_DamageByTick = l_AuraEffect->GetAmount();
                    else
                        return;

                    if (l_AuraEffect->GetAmplitude() <= 0)
                        return;

                    uint8 l_TickLeft = l_Aura->GetDuration() / l_AuraEffect->GetAmplitude();
                    m_DamageLeft = l_TickLeft * l_DamageByTick;

                    m_MaxDamage += l_MaxDamage - l_DamageTaken;
                }

            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                int32 l_Heal = GetHitDamage();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowMendDot);
                if (!l_Caster || !l_Target || !l_Heal || !l_SpellInfo)
                    return;

                m_MaxDamage = std::min(-l_Heal, m_MaxDamage);
                m_MaxDamage /= 2;
                m_MaxDamage += m_DamageLeft;

                if (l_SpellInfo->Effects[EFFECT_0].Amplitude != 0)
                    m_MaxDamage /= l_SpellInfo->GetMaxDuration() / l_SpellInfo->Effects[EFFECT_0].Amplitude;

                if (l_Caster->GetGUID() == l_Target->GetGUID() && l_Caster->HasAura(eSpells::Masochism))
                {
                    l_Heal = m_MaxDamage;
                    if (AuraEffect *l_Effect = l_Caster->GetAuraEffect(eSpells::MasochismHeal, 0))
                        if (l_Effect->GetTotalTicks() && l_Effect->GetAmplitude())
                            l_Heal += l_Effect->GetAmount() * (l_Effect->GetTotalTicks() - l_Effect->GetTickNumber()) / l_Effect->GetAmplitude();

                    l_Caster->CastCustomSpell(l_Caster, eSpells::MasochismHeal, &l_Heal, NULL, NULL, true);
                    return; /// break damage when active Aura Masochism
                }

                l_Caster->CastCustomSpell(l_Target, eSpells::ShadowMendDot, &m_MaxDamage, &m_MaxDamage, NULL, true);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_pri_shadow_mend_heal_SpellScript::HandleBeforeCast);
                AfterHit += SpellHitFn(spell_pri_shadow_mend_heal_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_shadow_mend_heal_SpellScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Smite - 585
class spell_pri_smite : public SpellScriptLoader
{
    public:
        spell_pri_smite() : SpellScriptLoader("spell_pri_smite") { }

        class spell_pri_smite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_smite_SpellScript);

            enum eSpells
            {
                SmiteRank2       = 231682,
                SmiteShield      = 208771,
                SmiteShieldAura  = 208772,
                HolyFire         = 14914,
                SurgeOfLightAura = 109186,
                SurgeOfLight     = 114255,
                Sanctuary        = 246393
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

                /// Surge of light
                const SpellInfo * l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SurgeOfLightAura);
                const SpellInfo * l_SanctuarySpellInfo = sSpellMgr->GetSpellInfo(eSpells::Sanctuary);

                if (!l_SanctuarySpellInfo)
                    return;

                if (l_SpellInfo != nullptr && l_Player->HasSpell(eSpells::SurgeOfLightAura) && roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                {
                    if (Aura* l_SurgeOfLight = l_Player->GetAura(eSpells::SurgeOfLight))
                    {
                        if (l_SurgeOfLight->GetStackAmount() == 2)
                            l_SurgeOfLight->SetDuration(l_SurgeOfLight->GetMaxDuration());
                        else
                            l_Player->CastSpell(l_Player, eSpells::SurgeOfLight, true);
                    }
                    else
                        l_Player->CastSpell(l_Player, eSpells::SurgeOfLight, true);
                }

                SpellInfo const* l_SpellInfoSmite = sSpellMgr->GetSpellInfo(eSpells::SmiteRank2);
                if (l_Player->GetActiveSpecializationID() == SPEC_PRIEST_DISCIPLINE && l_Player->HasAura(eSpells::SmiteRank2) && l_SpellInfoSmite)
                {
                    int32 l_SpellPower = l_Player->SpellBaseDamageBonusDone(l_SpellInfoSmite->GetSchoolMask());
                    float l_Versatility = (1 + l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f);
                    float I_Coef = (l_SpellInfoSmite->Effects[EFFECT_0].BasePoints / 100.0f);

                    int32 l_Damage = l_SpellPower * I_Coef * l_Versatility;
                    int32 I_Absorb = l_Damage;
                    int32 I_AbsorbTarget = l_Damage;
                    int32 I_CapAmount = (l_Damage + (l_Caster->HasAura(eSpells::Sanctuary) ? CalculatePct(l_Damage, l_SanctuarySpellInfo->Effects[EFFECT_0].BasePoints) : 0)) * 3.f;

                    AuraEffect* l_TargetEffect = l_Target->GetAuraEffect(eSpells::SmiteShieldAura, EFFECT_0);
                    if (l_TargetEffect)
                    {
                        if (l_Caster->HasAura(eSpells::Sanctuary))
                            I_AbsorbTarget += CalculatePct(l_TargetEffect->GetAmount(), 66.66f);
                        else
                            I_AbsorbTarget += l_TargetEffect->GetAmount();
                    }

                    if (AuraEffect *l_Effect = l_Player->GetAuraEffect(eSpells::SmiteShield, EFFECT_0))
                        I_Absorb += l_Effect->GetAmount();

                    if (l_Caster->HasAura(eSpells::Sanctuary))
                    {
                        I_Absorb += CalculatePct(I_Absorb, l_SanctuarySpellInfo->Effects[EFFECT_0].BasePoints);
                        I_AbsorbTarget += CalculatePct(I_AbsorbTarget, l_SanctuarySpellInfo->Effects[EFFECT_0].BasePoints);
                    }

                    I_Absorb = std::min(I_Absorb, I_CapAmount);
                    I_AbsorbTarget = std::min(I_AbsorbTarget, I_CapAmount);

                    l_Player->CastCustomSpell(l_Player, eSpells::SmiteShield, &I_Absorb, NULL, NULL, true);

                    if (!l_TargetEffect)
                        l_Player->CastCustomSpell(l_Target, eSpells::SmiteShieldAura, &I_AbsorbTarget, NULL, NULL, true);
                    else
                    {
                        l_TargetEffect->ChangeAmount(I_AbsorbTarget);
                        l_TargetEffect->GetBase()->RefreshDuration();
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() != SPEC_PRIEST_HOLY)
                    return;

                if (l_Player->HasSpellCooldown(eSpells::HolyFire) && roll_chance_i(20))
                    l_Player->RemoveSpellCooldown(eSpells::HolyFire, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_smite_SpellScript::HandleOnHit);
                AfterCast += SpellCastFn(spell_pri_smite_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_smite_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called By Levitate - 1706
class spell_pri_levitate: public SpellScriptLoader
{
    public:
        spell_pri_levitate() : SpellScriptLoader("spell_pri_levitate") { }

        class spell_pri_levitate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_levitate_SpellScript);

            enum eSpells
            {
                LevitateEffect = 252620
            };

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::LevitateEffect, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_levitate_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_levitate_SpellScript;
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Penance - 47750 - Penance - 47666
/// Called For Power Of The Dark Side -198068
class spell_pri_power_of_the_dark_side: public SpellScriptLoader
{
    public:
        spell_pri_power_of_the_dark_side() : SpellScriptLoader("spell_pri_power_of_the_dark_side") { }

        class spell_pri_power_of_the_dark_side_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_power_of_the_dark_side_SpellScript);

            enum eSpells
            {
                PowerOfTheDarkSideBuff  = 198069,
                PenanceDamage           = 47666,
                PenanceHeal             = 47750,
                DarkPenanceVisual       = 198070,
                PriestT20Discipline2P   = 242268,
                PenanceHealAura         = 47757,
                PenanceDamageAura       = 47758
            };

            uint32 m_Multiplier = 0;

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::PowerOfTheDarkSideBuff);
                if (!l_Aura)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                m_Multiplier = l_AuraEffect->GetAmount();
            }

            void HandleAmount(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (GetSpellInfo()->Id == eSpells::PenanceHeal)
                {
                    int32 l_HitHeal = GetHitHeal();
                    if (m_Multiplier > 0)
                        AddPct(l_HitHeal, m_Multiplier);

                    if (l_Caster->HasAura(eSpells::PriestT20Discipline2P))
                    {
                        if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PenanceT20BoltAllowed))
                        {
                            if (SpellInfo const* l_2PBonusInfo = sSpellMgr->GetSpellInfo(eSpells::PriestT20Discipline2P))
                            {
                                AddPct(l_HitHeal, l_2PBonusInfo->Effects[EFFECT_0].BasePoints);
                                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PenanceT20BoltAllowed) = false;
                            }
                        }
                    }

                    SetHitHeal(l_HitHeal);
                }
                else
                {
                    int32 l_HitDamage = GetHitDamage();
                    if (m_Multiplier > 0)
                        AddPct(l_HitDamage, m_Multiplier);

                    if (l_Caster->HasAura(eSpells::PriestT20Discipline2P))
                    {
                        if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PenanceT20BoltAllowed))
                        {
                            if (SpellInfo const* l_2PBonusInfo = sSpellMgr->GetSpellInfo(eSpells::PriestT20Discipline2P))
                            {
                                AddPct(l_HitDamage, l_2PBonusInfo->Effects[EFFECT_0].BasePoints);
                                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::PenanceT20BoltAllowed) = false;
                            }
                        }
                    }

                    SetHitDamage(l_HitDamage);
                }
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::PowerOfTheDarkSideBuff))
                    return;

                Spell const* l_ChannelSpell = l_Caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
                if (!l_ChannelSpell)
                    return;

                Unit* l_Target = l_ChannelSpell->GetUnitTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DarkPenanceVisual, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_pri_power_of_the_dark_side_SpellScript::HandleOnPrepare);
                OnCast += SpellCastFn(spell_pri_power_of_the_dark_side_SpellScript::HandleOnCast);
                switch (m_scriptSpellId)
                {
                    case eSpells::PenanceHeal:
                        OnEffectHitTarget += SpellEffectFn(spell_pri_power_of_the_dark_side_SpellScript::HandleAmount, EFFECT_0, SPELL_EFFECT_HEAL);
                        break;
                    case eSpells::PenanceDamage:
                        OnEffectHitTarget += SpellEffectFn(spell_pri_power_of_the_dark_side_SpellScript::HandleAmount, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                        break;
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_power_of_the_dark_side_SpellScript;
        }
};

/// Last Update 7.1.5 Build 22522
/// Called By Power Of The Dark Side - 198068
class spell_pri_power_of_the_dark_side_passive : public SpellScriptLoader
{
    public:
        spell_pri_power_of_the_dark_side_passive() : SpellScriptLoader("spell_pri_power_of_the_dark_side_passive") { }

        class spell_pri_power_of_the_dark_side_passive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_power_of_the_dark_side_passive_AuraScript);

            enum eSpells
            {
                SWPain              = 589,
                PurgeTheWicked      = 204197,
                PurgeTheWickedTick  = 204213

            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo)
                    return false;

                if (l_ProcSpellInfo->Id != eSpells::SWPain
                    && l_ProcSpellInfo->Id != eSpells::PurgeTheWicked && l_ProcSpellInfo->Id != eSpells::PurgeTheWickedTick)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_power_of_the_dark_side_passive_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_power_of_the_dark_side_passive_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Plea - 200829, PW: Shield - 17, Shadow Mend - 186263, PW : Radiance - 194509 - Atonement - 81749
/// Called for Atonement - 81749
class spell_pri_atonement: public SpellScriptLoader
{
    public:
        spell_pri_atonement() : SpellScriptLoader("spell_pri_atonement") { }

        class spell_pri_atonement_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_atonement_SpellScript);

            enum eSpells
            {
                AtonementPassive    = 81749,
                AtonementBuff       = 194384,
                Trinity             = 214205,
                Plea                = 200829,
                Radiance            = 194509,
                T19Discipline4P     = 211563,
                Rapture             = 47536,
                PWShield            = 17
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target ||!l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::Trinity) && l_SpellInfo->Id != eSpells::Plea)
                    return;

                if (l_Caster->HasAura(eSpells::Trinity) && l_SpellInfo->Id == eSpells::Plea && l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets].size() >= 3)
                {
                    std::set<uint64>& l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                    bool l_TargetHasBuff = false;
                    for (auto l_Itr : l_TargetGUIDs)
                    {
                        if (l_Itr == l_Target->GetGUID())
                        {
                            if (l_Caster->HasAura(eSpells::AtonementPassive) && l_SpellInfo->Id != eSpells::Radiance)
                            {
                                if (Aura* l_Aura = l_Target->GetAura(eSpells::AtonementBuff))
                                {
                                    l_Aura->SetMaxDuration(30 * IN_MILLISECONDS);
                                    l_Aura->SetDuration(30 * IN_MILLISECONDS);
                                    l_TargetHasBuff = true;
                                }
                            }
                        }
                    }

                    if (!l_TargetHasBuff)
                    {
                        if (l_Caster->HasAura(eSpells::AtonementPassive) && l_SpellInfo->Id != eSpells::Radiance)
                        {
                            Unit* l_OldTarget = nullptr;
                            uint32 l_DurationForCheck = 0;
                            for (auto l_Itr : l_TargetGUIDs)
                            {
                                if (Unit* l_CurUnit = ObjectAccessor::GetUnit(*l_Caster,  l_Itr))
                                {
                                    if (Aura* l_AtonementBuff = l_CurUnit->GetAura(eSpells::AtonementBuff, l_Caster->GetGUID()))
                                    {
                                        if (l_DurationForCheck == 0 || l_DurationForCheck > l_AtonementBuff->GetDuration())
                                        {
                                            l_DurationForCheck = l_AtonementBuff->GetDuration();

                                            if (l_DurationForCheck != 0)
                                                l_OldTarget = l_CurUnit;
                                        }
                                    }
                                }
                            }

                            if (l_OldTarget)
                            {
                                l_OldTarget->RemoveAura(eSpells::AtonementBuff);
                                l_TargetGUIDs.erase(l_OldTarget->GetGUID());

                                if (Aura* l_AtonementBuff = l_Caster->AddAura(eSpells::AtonementBuff, l_Target))
                                {
                                    l_AtonementBuff->SetMaxDuration(30 * IN_MILLISECONDS);
                                    l_AtonementBuff->SetDuration(30 * IN_MILLISECONDS);
                                }
                            }
                        }
                    }

                    return;
                }

                if (l_Caster->HasAura(eSpells::AtonementPassive) && l_SpellInfo->Id != eSpells::Radiance)
                    l_Caster->CastSpell(l_Target, eSpells::AtonementBuff, true);

                /// Item - Priest T19 Discipline 4P Bonus
                if (m_scriptSpellId == eSpells::PWShield && l_Caster->HasAura(eSpells::Rapture))
                    if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::T19Discipline4P, EFFECT_0))
                        if (Aura* l_Aura = l_Target->GetAura(eSpells::AtonementBuff))
                            l_Aura->SetDuration(l_Aura->GetDuration() + l_AuraEffect->GetAmount() * IN_MILLISECONDS);

                if (!l_Caster->HasAura(eSpells::Trinity))
                    return;

                SpellInfo const* l_TrinityInfo = sSpellMgr->GetSpellInfo(eSpells::Trinity);
                if (Aura* l_Aura = l_Target->GetAura(eSpells::AtonementBuff))
                {
                    l_Aura->SetDuration(l_Aura->GetDuration() + (l_TrinityInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS));
                    /// Can't have more than 30s on atonement
                    l_Aura->SetDuration(l_Aura->GetDuration() > 30000 ? 30000 : l_Aura->GetDuration());
                }
            }

            void Register() override
            {
                if (m_scriptSpellId != eSpells::AtonementPassive)
                    OnHit += SpellHitFn(spell_pri_atonement_SpellScript::HandleOnHit);
            }
        };

        class spell_pri_atonement_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_atonement_AuraScript);

            enum eSpells
            {
                AtonementPassive    = 81749,
                AtonementHeal       = 81751,
                AtonementHealCrit   = 94472,
                PowerWordBarrier    = 81782,
                BarrierOfTheDevoted = 197815,
                HaloDamage          = 120696,
                DivineStarDamage    = 122128,
                VimAndVigor         = 195488,
                VelensFutureSight   = 235966,
                ArtificialDamage    = 219655
            };

            void HandleProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_Aura || !l_DamageInfo || !l_DamageInfo->GetSpellInfo())
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_AuraEffect || !l_Target)
                    return;

                if (l_DamageInfo->GetSpellInfo()->Id == eSpells::HaloDamage && !l_Caster->m_SpellHelper.GetBool(eSpells::HaloDamage))
                    return;
                else if (l_DamageInfo->GetSpellInfo()->Id == eSpells::HaloDamage)
                    l_Caster->m_SpellHelper.GetBool(eSpells::HaloDamage) = false;

                if (l_DamageInfo->GetSpellInfo()->Id == eSpells::DivineStarDamage && !l_Caster->m_SpellHelper.GetBool(eSpells::DivineStarDamage))
                    return;
                else if (l_DamageInfo->GetSpellInfo()->Id == eSpells::DivineStarDamage)
                    l_Caster->m_SpellHelper.GetBool(eSpells::DivineStarDamage) = false;

                float l_Pct = l_AuraEffect->GetAmount();
                int32 l_Damage = l_DamageInfo->GetAmount();
                if (l_Damage == 0)
                    l_Damage = l_DamageInfo->GetAbsorb();

                double l_Resist = 0;
                auto const& l_ResistanceEffect = l_Target->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
                for (AuraEffect* l_Resistance : l_ResistanceEffect)
                {
                    l_Resist = -l_Resistance->GetAmount();
                    if (l_Resist != 0)
                    {
                        double l_Percentage = (1 - (l_Resist / 100));
                        l_Percentage *= 100;
                        l_Damage = l_Damage * (100 / l_Percentage);
                    }
                }

                l_Damage = CalculatePct(l_Damage, l_Pct);
                if (AuraEffect *l_BarrierOfTheDevoted = l_Caster->GetAuraEffect(eSpells::BarrierOfTheDevoted, EFFECT_0))
                {
                    if (l_Caster->HasAura(eSpells::PowerWordBarrier))
                        l_Damage = AddPct(l_Damage, l_BarrierOfTheDevoted->GetAmount());
                }

                if (AuraEffect const* l_Effect = l_Caster->GetAuraEffect(eSpells::VimAndVigor, SpellEffIndex::EFFECT_0))
                    l_Damage = AddPct(l_Damage, l_Effect->GetAmount());

                if (AuraEffect const* l_Effect = l_Caster->GetAuraEffect(eSpells::VelensFutureSight, SpellEffIndex::EFFECT_1))
                    l_Damage = AddPct(l_Damage, l_Effect->GetAmount());

                std::set<uint64> l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                {
                    Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster,  *l_Itr);

                    if (!l_Target)
                        continue;

                    l_Caster->CastCustomSpell(l_Target, p_ProcEventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT ? eSpells::AtonementHealCrit : eSpells::AtonementHeal, &l_Damage, NULL, NULL, true);
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::AtonementPassive)
                    OnProc += AuraProcFn(spell_pri_atonement_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_atonement_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_atonement_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Called by Atonement - 194384
class spell_pri_atonement_buff : public SpellScriptLoader
{
    public:
        spell_pri_atonement_buff() : SpellScriptLoader("spell_pri_atonement_buff") { }

        class spell_pri_atonement_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_atonement_buff_AuraScript);

            enum eSpells
            {
                Plea                = 212100,
                SinsOfTheMany       = 198074,
                SinsOfTheManyBuff   = 198076,
                BorrowedTimeAura    = 197762,
                BorrowedTimeBuff    = 197763,
                Contrition          = 197419
            };

            enum eArtifactPowers
            {
                BorrowedTimeArtifact = 895
            };

            void HandleApply(AuraEffect const* /*p_AuraEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                if (Aura* l_Aura = GetAura())
                    l_Aura->SetDuration(GetCorrectDuration(l_Caster));

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets].insert(l_Target->GetGUID());
                ///Update Plea cost stack
                l_Caster->CastSpell(l_Caster, eSpells::Plea, true);
                if (l_Caster->HasAura(eSpells::SinsOfTheMany))
                    l_Caster->CastSpell(l_Caster, eSpells::SinsOfTheManyBuff, true);

                if (Player *l_Player = l_Caster->ToPlayer())
                {
                    if (l_Caster->HasAura(eSpells::BorrowedTimeAura))
                    {
                        SpellInfo const* l_BorrowedTimeInfo = sSpellMgr->GetSpellInfo(eSpells::BorrowedTimeBuff);
                        if (!l_BorrowedTimeInfo)
                            return;

                        uint32 l_TraitRank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::BorrowedTimeArtifact);
                        int32 l_Bp0 = l_BorrowedTimeInfo->Effects[0].BasePoints * l_TraitRank;
                        int32 l_Bp1 = l_BorrowedTimeInfo->Effects[1].BasePoints * l_TraitRank;
                        int32 l_Bp2 = l_BorrowedTimeInfo->Effects[2].BasePoints * l_TraitRank;
                        int32 l_Bp3 = l_BorrowedTimeInfo->Effects[3].BasePoints * l_TraitRank;

                        l_Caster->CastCustomSpell(l_Caster, eSpells::BorrowedTimeBuff, &l_Bp0, &l_Bp1, &l_Bp2, &l_Bp3, nullptr, nullptr, true);
                    }
                }
            }

            void HandleRemove(AuraEffect const* /*p_AuraEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets].erase(l_Target->GetGUID());
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Plea))
                {
                    ///Update Plea cost stack
                    l_Aura->DropStack();
                }
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::SinsOfTheManyBuff))
                    l_Aura->DropStack();
            }

            void CalculateMaxDuration(int32& p_Duration)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Duration = GetCorrectDuration(l_Caster);
            }

            int32 GetCorrectDuration(Unit* p_Caster)
            {
                int32 l_Duration = GetSpellInfo()->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS;
                if (AuraEffect const* l_AurEffect = p_Caster->GetAuraEffect(eSpells::Contrition, EFFECT_0))
                    l_Duration += l_AurEffect->GetAmount() * IN_MILLISECONDS;

                return (l_Duration);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_atonement_buff_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectApplyFn(spell_pri_atonement_buff_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_pri_atonement_buff_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_atonement_buff_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Atonement
class PlayerScript_pri_atonement : public PlayerScript
{
public:
    PlayerScript_pri_atonement() :PlayerScript("PlayerScript_Atonement") { }

    enum eSpell
    {
        Atonement = 194384
    };

    void OnLogout(Player* p_Player)
    {
        std::set<uint64> l_TargetGUIDs = p_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
        for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
        {
            Unit* l_Target = ObjectAccessor::GetUnit((WorldObject const&)* p_Player, *l_Itr);
            if (l_Target)
                l_Target->RemoveAurasDueToSpell(eSpell::Atonement, p_Player->GetGUID());
        }
    }
};

/// Last Update 7.3.5 Build 26365
/// Sins of the Many
class PlayerScript_pri_sins_of_the_many : public PlayerScript
{
    public:
        PlayerScript_pri_sins_of_the_many() : PlayerScript("PlayerScript_pri_sins_of_the_many") { }

        enum eSpell
        {
            SinsOfTheManyBuff = 198076
        };

        void OnLogout(Player* p_Player)
        {
            if (!p_Player->HasAura(eSpell::SinsOfTheManyBuff))
                return;

            p_Player->RemoveAura(eSpell::SinsOfTheManyBuff);
        }

        void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 /*p_NewAreaID*/)
        {
            if (p_NewZoneID == p_OldZoneID)
                return;

            if (!p_Player->HasAura(eSpell::SinsOfTheManyBuff))
                return;

            p_Player->RemoveAura(eSpell::SinsOfTheManyBuff);
        }
};

/// Last update 7.2.3 Build 25549
/// Called by Atonemnent (Heal) - 81751
/// Called for Archangel - 197862
class spell_pri_archangel_atonement : public SpellScriptLoader
{
    public:
        spell_pri_archangel_atonement() : SpellScriptLoader("spell_pri_archangel_atonement") { }

        class spell_pri_archangel_atonement_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_archangel_atonement_SpellScript);

            enum eSpells
            {
                Archangel = 197862
            };

            void HandleHeal(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Archangel = l_Caster->GetAura(eSpells::Archangel);
                if (!l_Archangel)
                    return;

                AuraEffect const* l_AuraEffect = l_Archangel->GetEffect(EFFECT_0);
                if (!l_AuraEffect)
                    return;

                int32 l_HealAmount = GetHitHeal();

                SetHitHeal(AddPct(l_HealAmount, l_AuraEffect->GetAmount()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_archangel_atonement_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_archangel_atonement_SpellScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by PW : Radiance - 194509
class spell_pri_radiance : public SpellScriptLoader
{
    public:
        spell_pri_radiance() : SpellScriptLoader("spell_pri_radiance") { }

        class spell_pri_radiance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_radiance_SpellScript);

            enum eSpells
            {
                Atonement           = 194384,
                AtonementPassive    = 81749,
                Radiance            = 194509,
                Trinity             = 214205,
                PowerInfusion       = 10060,
                InnerHallation      = 248037
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::InnerHallation))
                {
                    SpellInfo const* l_InnerHallation = sSpellMgr->GetSpellInfo(eSpells::InnerHallation);
                    
                    if(!l_InnerHallation)
                        return;

                    if (!roll_chance_i(l_InnerHallation->Effects[EFFECT_0].BasePoints))
                        return;

                    l_Caster->CastSpell(l_Caster, eSpells::PowerInfusion, true);

                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::PowerInfusion))
                        l_Aura->SetDuration(l_InnerHallation->Effects[EFFECT_1].BasePoints * IN_MILLISECONDS);
                }
            }

            void HandleTargets(std::list<WorldObject*>& p_Target)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetExplTargetUnit())
                {
                    std::list<WorldObject*>::iterator l_Itr = std::find(p_Target.begin(), p_Target.end(), (WorldObject*)GetExplTargetUnit());
                    if (l_Itr != p_Target.end())
                        p_Target.erase(l_Itr);
                }

                uint32 l_TargetAmount = GetSpellInfo()->Effects[EFFECT_2].BasePoints;

                p_Target.sort(JadeCore::HealthPctExcludeAuraOrderPredPlayer(eSpells::Atonement, l_Caster->GetGUID()));
                p_Target.resize(l_TargetAmount);
                if (GetExplTargetUnit())
                    p_Target.push_back(GetExplTargetUnit());
                else
                    p_Target.push_back(l_Caster);
            }

            void HandleBuff(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::Trinity))
                    return;

                if (SpellInfo const* l_RadianceSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Radiance))
                {
                    if (SpellInfo const* l_AtonementSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Atonement))
                        if (Aura* l_AtonementAura = l_Target->GetAura(eSpells::Atonement))
                            if (l_AtonementAura->GetDuration() >= CalculatePct(l_AtonementSpellInfo->GetDuration(), l_RadianceSpellInfo->Effects[EFFECT_3].BasePoints))
                                return;

                    if (l_Caster->HasAura(eSpells::AtonementPassive))
                    {
                        if (Aura* l_AtonementAura = l_Caster->AddAura(eSpells::Atonement, l_Target))
                        {
                            l_AtonementAura->SetDuration(CalculatePct(l_AtonementAura->GetDuration(), l_RadianceSpellInfo->Effects[EFFECT_3].BasePoints));
                            l_AtonementAura->SetMaxDuration(CalculatePct(l_AtonementAura->GetDuration(), l_RadianceSpellInfo->Effects[EFFECT_3].BasePoints));
                        }
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_radiance_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_radiance_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_pri_radiance_SpellScript::HandleBuff, EFFECT_1, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_radiance_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Searing Light - 215768
class spell_pri_searing_light : public SpellScriptLoader
{
    public:
        spell_pri_searing_light() : SpellScriptLoader("spell_pri_searing_light") { }

        class spell_pri_searing_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_searing_light_AuraScript);

            enum eSpells
            {
                Smite   = 585,
                Penance = 47540
            };

            void OnAuraProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_SpellInfo || !l_Caster)
                    return;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::Smite)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->ReduceSpellCooldown(eSpells::Penance, -(l_SpellInfo->Effects[EFFECT_0].BasePoints));
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pri_searing_light_AuraScript::OnAuraProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_searing_light_AuraScript();
        }
};

/// Last update 7.0.3 Build 22293
/// Called by PW : Shield - 17
/// For Renewed Hope - 197469
class spell_pri_renewed_hope : public SpellScriptLoader
{
    public:
        spell_pri_renewed_hope() : SpellScriptLoader("spell_pri_renewed_hope") { }

        class spell_pri_renewed_hope_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_renewed_hope_SpellScript);

            enum eSpells
            {
                RenewedHopePassive  = 197469,
                RenewedHopeHeal     = 197470
            };

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::RenewedHopePassive))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::RenewedHopeHeal, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pri_renewed_hope_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_renewed_hope_SpellScript();
        }
};

/// Last update 7.0.3 Build 22522
/// Called by Archangel - 197862
class spell_pri_archangel : public SpellScriptLoader
{
    public:
        spell_pri_archangel() : SpellScriptLoader("spell_pri_archangel") { }

        class spell_pri_archangel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_archangel_SpellScript);

            enum eSpells
            {
                Atonement = 194384
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::set<uint64> l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, *l_Itr);
                    if (!l_Target)
                        continue;

                    Aura* l_Atonement = l_Target->GetAura(eSpells::Atonement);
                    if (!l_Atonement)
                        continue;

                    l_Atonement->RefreshTimers();
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_archangel_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_archangel_SpellScript();
        }
};

/// Last update 7.0.3 Build 22293
/// Called by Void Form 185916
class spell_pri_void_form_passive : public SpellScriptLoader
{
public:
    spell_pri_void_form_passive() : SpellScriptLoader("spell_pri_void_form_passive") { }

    class spell_pri_void_form_passive_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_void_form_passive_AuraScript);

        enum eSpells
        {
            VoidFormIconAura = 218413
        };

        void HandleOnAuraUpdate(const uint32 /*p_diff*/)
        {
            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Aura || !l_Aura->GetEffect(EFFECT_3))
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            if (l_Player->GetPower(POWER_INSANITY) >= (l_Aura->GetEffect(EFFECT_3)->GetAmount() * 100))
            {
                if (!l_Player->HasSpellCooldown(eSpells::VoidFormIconAura))
                {
                    l_Player->CastSpell(l_Player, eSpells::VoidFormIconAura, true);
                    l_Player->AddSpellCooldown(eSpells::VoidFormIconAura, 0, 5 * IN_MILLISECONDS);
                }
            }
            else if (l_Player->HasAura(eSpells::VoidFormIconAura))
            {
                l_Player->RemoveAurasDueToSpell(eSpells::VoidFormIconAura);
            }
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_pri_void_form_passive_AuraScript::HandleOnAuraUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_void_form_passive_AuraScript();
    }
};

/// Last update 7.3.2 build 25549
/// Called by Edge of Insanity 199408
class spell_pri_edge_of_insanity_passive : public SpellScriptLoader
{
public:
    spell_pri_edge_of_insanity_passive() : SpellScriptLoader("spell_pri_edge_of_insanity_passive") { }

    class spell_pri_edge_of_insanity_passive_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_edge_of_insanity_passive_AuraScript);

        enum eSpells
        {
            VoidForm = 199415,
            VoidFormPvP = 194249,
            EdgeOfInsanity = 199412,
            EdgeOfInsanityCancel = 199413,
            LegacyOfTheVoid = 193225

        };

        void HandleOnAuraUpdate(const uint32 /*p_diff*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            uint32 l_InsanityAmount = (l_Player->HasAura(eSpells::LegacyOfTheVoid)) ? 6500 : 10000;

            if ((l_Player->GetPower(POWER_INSANITY) >= l_InsanityAmount) && !l_Player->HasAura(eSpells::VoidForm) && !l_Player->HasAura(eSpells::VoidFormPvP))
                l_Player->CastSpell(l_Player, eSpells::EdgeOfInsanity, true);
            else if (l_Player->HasAura(eSpells::EdgeOfInsanity))
                l_Player->CastSpell(l_Player, eSpells::EdgeOfInsanityCancel, true);
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_pri_edge_of_insanity_passive_AuraScript::HandleOnAuraUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_edge_of_insanity_passive_AuraScript();
    }
};

/// Last update 7.3.2 Build 25549
/// Called by Void Eruption 228260
class spell_pri_void_eruption : public SpellScriptLoader
{
    public:
        spell_pri_void_eruption() : SpellScriptLoader("spell_pri_void_eruption") { }

        class spell_pri_void_eruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_eruption_SpellScript);

            enum eSpells
            {
                SWPain                  = 589,
                VampiricTouch           = 34914,
                VoidForm                = 194249,
                VoidEruptionDamageRight = 228360,
                T21Shadow4P             = 251846,
                OverwhelmingDarkness    = 252909
            };

            std::list<uint64> l_Targets;

            void HandlePowerCost(Powers p_Power, int32& p_PowerCost)
            {
                if (p_Power == Powers::POWER_INSANITY)
                    p_PowerCost = 0;
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                for (WorldObject* l_Target : p_Targets)
                {
                    if (Unit* l_UnitTarget = l_Target->ToUnit())
                    {
                        if (l_UnitTarget->HasAura(eSpells::SWPain, l_Caster->GetGUID()) || l_UnitTarget->HasAura(eSpells::VampiricTouch, l_Caster->GetGUID()))
                            l_Targets.push_back(l_UnitTarget->GetGUID());
                    }
                }

                p_Targets.clear();
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint64 l_TargetGuid : l_Targets)
                {
                    if (Unit* l_Target = Unit::GetUnit(*l_Caster, l_TargetGuid))
                    {
                        if (l_Target->HasAura(eSpells::SWPain, l_Caster->GetGUID()) || l_Target->HasAura(eSpells::VampiricTouch, l_Caster->GetGUID()))
                            l_Caster->CastSpell(l_Target, eSpells::VoidEruptionDamageRight, true);
                    }
                }

                l_Caster->CastSpell(l_Caster, eSpells::VoidForm, true);

                if (l_Caster->HasAura(eSpells::T21Shadow4P))
                    l_Caster->CastSpell(l_Caster, eSpells::OverwhelmingDarkness);
            }

            void Register() override
            {
                OnTakePowers += SpellTakePowersFn(spell_pri_void_eruption_SpellScript::HandlePowerCost);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_void_eruption_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_pri_void_eruption_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_eruption_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Void Form - 194249
class spell_pri_void_form : public SpellScriptLoader
{
    public:
        spell_pri_void_form() : SpellScriptLoader("spell_pri_void_form") { }

        class aura_impl : public AuraScript
        {
            PrepareAuraScript(aura_impl);

            enum eSpells
            {
                LingeringInsanity           = 197937,
                VoidTorrent                 = 205065,
                Dispersion                  = 47585,
                TentaclesFirst              = 210198,
                TentaclesSecond             = 210197,
                TentaclesThird              = 210196,
                MassHysteriaArtifactTrait   = 194378,
                LingeringInsanityTalent     = 199849,
                SphereOfInsanityAura        = 194179,
                SphereOfInsanitySummon      = 194182,
                SurrenderedSoul             = 212570,
                SurrenderToMadnessBuff      = 193223,
                SurrenderToMadnessKill      = 195455,
                MotherShahrazsSeduction     = 236523,
                VampiricAura                = 34914,
                ShadowWordPain              = 589,
                TheTwinsPainfulTouchSpread  = 207724,
                DreamSimulacrum             = 206005,
                SustainedSanity             = 219772,
                T19Shadow4P                 = 211654,
                Void                        = 211657,
                ShadowForm                  = 232698,
                IridisEmpowerment           = 224999,
                T20Shadow4P                 = 242273,
                OverwhelmingDarkness        = 252909
            };

            enum eNPC
            {
                SphereOfInsanityNPC = 98680
            };

            bool m_FirstTick = true;

            void HandleOnApply(AuraEffect const* p_AuraEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = p_AuraEff->GetBase();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MotherShahrazsSeduction);
                if (!l_Caster || !l_Aura || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::ShadowForm))
                    l_Caster->RemoveAura(eSpells::ShadowForm);

                if (l_Caster->HasAura(eSpells::SphereOfInsanityAura))
                    l_Caster->CastSpell(l_Caster, eSpells::SphereOfInsanitySummon, true);

                if (l_Caster->HasAura(eSpells::MotherShahrazsSeduction))
                    l_Aura->ModStackAmount(l_SpellInfo->Effects[EFFECT_0].BasePoints);

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::TheTwinsPainfulTouch) = true;

                /// Item - Priest T19 Shadow 4P Bonus
                if (l_Caster->HasAura(eSpells::T19Shadow4P))
                    l_Caster->CastSpell(l_Caster, eSpells::Void, true);
            }

            void HandleOnTick(AuraEffect const* p_AuraEff)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = p_AuraEff->GetBase();
                if (!l_Caster || !l_Aura)
                    return;

                if (l_Caster->HasAura(eSpells::Dispersion))
                    return;

                if (l_Caster->HasAura(eSpells::MassHysteriaArtifactTrait))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MassHysteriaArtifactTrait))
                    {
                        if (AuraEffect* l_AuraEfffect = l_Aura->GetEffect(EFFECT_9))
                            l_AuraEfffect->SetAmount(l_AuraEfffect->GetAmount() + l_SpellInfo->Effects[EFFECT_0].BasePoints);
                    }
                }

                l_Aura->ModStackAmount(1);
                int32 l_StackAmount = l_Aura->GetStackAmount();
                if (!l_Caster->HasAura(eSpells::VoidTorrent))
                    ++m_DecayStacks;

                float l_ModPerSecond = (6.f + (m_DecayStacks - 1) * 0.55f);

                float l_NewUpdateInterval = (1.0f * IN_MILLISECONDS) / l_ModPerSecond;
                if (!G3D::fuzzyEq(l_NewUpdateInterval, m_UpdateInterval))
                {
                    float l_IntervalDiff = m_UpdateInterval - l_NewUpdateInterval;
                    m_UpdateTimer = m_UpdateTimer > l_IntervalDiff ? m_UpdateTimer - l_IntervalDiff : 0.0f;
                    m_UpdateInterval = l_NewUpdateInterval;

                    if (l_Caster->HasAura(eSpells::T20Shadow4P))
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T20Shadow4P))
                        {
                            int32 l_Pct = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                            if (l_Caster->HasAura(eSpells::SurrenderToMadnessBuff))
                                l_Pct /= 2;

                            AddPct(m_UpdateInterval, l_Pct);
                        }
                    }
                }

                switch (l_StackAmount)
                {
                    case 3:
                        l_Caster->CastSpell(l_Caster, eSpells::TentaclesFirst, true);
                        break;
                    case 6:
                        l_Caster->CastSpell(l_Caster, eSpells::TentaclesSecond, true);
                        break;
                    case 9:
                        l_Caster->CastSpell(l_Caster, eSpells::TentaclesThird, true);
                        break;
                    default:
                        break;
                }

                if (l_Caster->GetPower(POWER_INSANITY) == 0)
                    l_Caster->RemoveAurasDueToSpell(GetSpellInfo()->Id);

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::OverwhelmingDarkness))
                {
                    l_Aura->SetStackAmount(l_StackAmount);
                    l_Aura->RefreshDuration();
                }
            }

            void HandleUpdate(uint32 p_Diff, AuraEffect* p_AurEff)
            {
                if (m_UpdateTimer > p_Diff)
                {
                    m_UpdateTimer -= p_Diff;
                    return;
                }

                m_UpdateTimer = m_UpdateInterval;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                bool l_SkipDraining = l_Caster->HasAura(eSpells::VoidTorrent) || l_Caster->HasAura(eSpells::Dispersion) || l_Caster->HasAura(eSpells::SustainedSanity);
                if (m_FirstTick)
                    l_SkipDraining = true;

                m_FirstTick = false;
                if (!l_SkipDraining)
                    l_Caster->ModifyPower(POWER_INSANITY, m_InsanityDrain);

                m_DiffCounter += (p_Diff - m_UpdateTimer);
                while (m_DiffCounter >= m_UpdateInterval)
                {
                    if (!l_SkipDraining)
                        l_Caster->ModifyPower(POWER_INSANITY, m_InsanityDrain);

                    m_DiffCounter -= m_UpdateInterval;
                }

                if (l_Caster->GetPower(POWER_INSANITY) == 0)
                    GetAura()->SetDuration(0);
            }

            void HandleOnRemove(AuraEffect const* p_AuraEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                UnSummonSphereOfInsanity(l_Caster);
                l_Caster->RemoveAurasDueToSpell(eSpells::TentaclesFirst);
                l_Caster->RemoveAurasDueToSpell(eSpells::TentaclesSecond);
                l_Caster->RemoveAurasDueToSpell(eSpells::TentaclesThird);
                l_Caster->RemoveAurasDueToSpell(eSpells::OverwhelmingDarkness);

                if (l_Caster->HasAura(eSpells::SurrenderToMadnessBuff))
                {
                    if (l_Caster->HasAura(eSpells::DreamSimulacrum))
                        l_Caster->RemoveAura(eSpells::DreamSimulacrum);
                    else
                        l_Caster->CastSpell(l_Caster, SurrenderToMadnessKill, true);
                }

                l_Caster->AddAura(eSpells::ShadowForm, l_Caster);

                if (!l_Caster->HasAura(eSpells::LingeringInsanityTalent))
                    return;

                if (!l_Caster->HasAura(eSpells::LingeringInsanity))
                {
                    uint32 l_Amout = p_AuraEff->GetAmount();
                    for (uint32 i = 0; i < l_Amout; i++)
                        l_Caster->CastSpell(l_Caster, eSpells::LingeringInsanity, true);
                }

                l_Caster->RemoveAurasDueToSpell(eSpells::TheTwinsPainfulTouchSpread);
            }

            void UnSummonSphereOfInsanity(Unit *p_Caster)
            {
                GuidList* l_Summoned = p_Caster->GetSummonList(eNPC::SphereOfInsanityNPC);

                if (!l_Summoned)
                    return;

                std::vector<uint64> l_SphereGUIDs;

                for (auto l_SphereGUID : *l_Summoned)
                    l_SphereGUIDs.push_back(l_SphereGUID);

                for (auto l_SphereGUID : l_SphereGUIDs)
                {
                    Unit* l_Sphere = Unit::GetUnit(*p_Caster, l_SphereGUID);
                    Guardian* l_Guardian = nullptr;

                    if (!l_Sphere || !l_Sphere->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
                        continue;

                    l_Guardian = dynamic_cast<Guardian *>(l_Sphere);
                    l_Guardian->DespawnOrUnsummon(500);
                }
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::IridisEmpowerment))
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::IridisEmpowerment))
                            p_Amount += l_SpellInfo->Effects[EFFECT_1].BasePoints;
            }

            void CalculateAmountLosingInsanity(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::T20Shadow4P))
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T20Shadow4P))
                            p_Amount += abs(CalculatePct(p_Amount, l_SpellInfo->Effects[EFFECT_0].BasePoints));
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(aura_impl::CalculateAmountLosingInsanity, EFFECT_1, SPELL_AURA_MOD_POWER_REGEN);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(aura_impl::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                AfterEffectApply += AuraEffectApplyFn(aura_impl::HandleOnApply, EFFECT_4, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(aura_impl::HandleOnTick, EFFECT_4, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(aura_impl::HandleOnRemove, EFFECT_2, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
                OnEffectUpdate += AuraEffectUpdateFn(aura_impl::HandleUpdate, EFFECT_4, SPELL_AURA_PERIODIC_DUMMY);
            }

        private:

            float m_UpdateTimer = 1.0f * IN_MILLISECONDS;
            float m_UpdateInterval = 1.0f * IN_MILLISECONDS / 100;
            float m_DiffCounter = 0.0f;
            int32 m_InsanityDrain = -100;
            int32 m_DecayStacks = 1;
        };

        AuraScript* GetAuraScript() const override
        {
            return new aura_impl();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Lingering Insanity - 197937
class spell_pri_lingering_insanity : public SpellScriptLoader
{
    public:
        spell_pri_lingering_insanity() : SpellScriptLoader("spell_pri_lingering_insanity") { }

        class spell_pri_lingering_insanity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_lingering_insanity_AuraScript);

            enum eSpells
            {
                LingeringInsanity = 199849
            };

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                uint32 l_Duration = (l_Aura->GetStackAmount() % 2 == 0) ? l_Aura->GetStackAmount() / 2 : l_Aura->GetStackAmount() / 2 + 1;
                l_Aura->SetDuration(l_Duration * IN_MILLISECONDS);
            }

            void HandleOnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LingeringInsanity);
                Aura* l_Aura = GetAura();
                if (!l_Aura || !l_SpellInfo)
                    return;

                for (uint8 l_I =0; l_I < l_SpellInfo->Effects[EFFECT_0].BasePoints; l_I++)
                    if (l_Aura)
                        l_Aura->DropStack();

                if (!l_Aura)
                    return;

                uint32 l_Duration = (l_Aura->GetStackAmount() % 2 == 0) ? l_Aura->GetStackAmount() / 2 : l_Aura->GetStackAmount() / 2 + 1;
                l_Aura->SetDuration(l_Duration * IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_lingering_insanity_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_lingering_insanity_AuraScript::HandleOnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_lingering_insanity_AuraScript();
        }
};

/// Last update 7.1.5 Build 23194
/// Called by Void Bolt - 205448
class spell_pri_void_bolt : public SpellScriptLoader
{
    public:
        spell_pri_void_bolt() : SpellScriptLoader("spell_pri_void_bolt") { }

        class spell_pri_void_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_bolt_SpellScript);

            enum eSpells
            {
                AnundsLastBreath        = 215210,
                SWPain                  = 589,
                VampiricTouch           = 34914
            };

            void HandleOnHit(SpellEffIndex /*p_EfffectIndex*/)
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                if (Aura *l_AnundsLastBreathAura = l_Caster->GetAura(eSpells::AnundsLastBreath))
                {
                    if (AuraEffect *l_AnundsLastBreathEffect = l_AnundsLastBreathAura->GetEffect(EFFECT_0))
                    {
                        int32 l_Damage = GetHitDamage();
                        AddPct(l_Damage, l_AnundsLastBreathAura->GetStackAmount() * l_AnundsLastBreathEffect->GetBaseAmount());
                        SetHitDamage(l_Damage);
                    }

                    l_AnundsLastBreathAura->Remove();
                }

                for (auto& itr : l_Caster->m_unitsWithMyAuras)
                {
                    for (uint32 id : itr.second)
                    {
                        if (id == eSpells::SWPain || eSpells::VampiricTouch)
                        {
                            if (auto unit = ObjectAccessor::GetUnit(*l_Caster, itr.first))
                            {
                                if (auto aura = unit->GetAura(id, l_Caster->GetGUID()))
                                {
                                    aura->SetCustomData(aura->GetDuration());
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_void_bolt_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_bolt_SpellScript();
        }
};

// 234746
class spell_pri_void_bolt_missile : public SpellScript
{
    PrepareSpellScript(spell_pri_void_bolt_missile);

    enum eSpells
    {
        SWPain          = 589,
        VampiricTouch   = 34914
    };

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (auto caster = GetCaster())
        {
            if (auto target = GetHitUnit())
            {
                if (auto aurEff = caster->GetAuraEffect(231688, EFFECT_0))
                {
                    for (uint32 i : { eSpells::SWPain, eSpells::VampiricTouch })
                        if (auto aura = target->GetAura(i, caster->GetGUID()))
                            if (auto eff = aura->GetEffect(EFFECT_1))
                            {
                                float dist = caster->GetDistance(target);
                                uint32 delay = dist / 40 * 1000 + 200;
                                uint32 newDuration = aura->GetCustomData() + aurEff->GetAmount();
                                if (delay < 2000)
                                    newDuration += delay;
                                aura->SetMaxDuration(newDuration);
                                aura->SetDuration(aura->GetCustomData() + aurEff->GetAmount() + delay);
                            }
                }
            }
        }
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_void_bolt_missile::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/// Last update 7.1.5 Build 23420
/// Called by Shadow Word : Death - 32379, Shadow Word: Death - 199853 (Reaper of Souls)
class spell_pri_shadow_word_death : public SpellScriptLoader
{
    public:
        spell_pri_shadow_word_death() : SpellScriptLoader("spell_pri_shadow_word_death") { }

        class spell_pri_shadow_word_death_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_word_death_SpellScript);

            enum eSpells
            {
                ShadowWordDeath         = 32379,
                ShadowWordDeathRoS      = 199911,
                ShadowWordDeathEnergize = 190714
            };

            void HandleAfterHit()
            {
                SpellInfo const* l_SwdSpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowWordDeath);
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster || !l_SpellInfo || !l_SwdSpellInfo)
                    return;

                uint32 alivePart = l_SwdSpellInfo->Effects[EFFECT_2].BasePoints * 100;
                uint32 deadPart = l_SwdSpellInfo->Effects[EFFECT_1].BasePoints * 100;

                if (l_SpellInfo->Id == eSpells::ShadowWordDeath)
                {
                    if (l_Target->isDead())
                        l_Caster->CastSpell(l_Caster, eSpells::ShadowWordDeathEnergize, true);
                    else
                        l_Caster->EnergizeBySpell(l_Caster, l_SpellInfo->Id, alivePart, POWER_INSANITY);
                }
                else if (l_SpellInfo->Id == eSpells::ShadowWordDeathRoS)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::ShadowWordDeathEnergize, true);
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pri_shadow_word_death_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_shadow_word_death_SpellScript();
        }
};

/// Last Update 7.0.3 - 22522
/// Called By Dispersion - 47585
/// Called For Thrive in the Shadows - 194024
class spell_pri_thrive_in_the_shadows : public SpellScriptLoader
{
    public:
        spell_pri_thrive_in_the_shadows() : SpellScriptLoader("spell_pri_thrive_in_the_shadows") { }

        class spell_pri_thrive_in_the_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_thrive_in_the_shadows_AuraScript);

            enum eSpells
            {
                ThriveInTheShadows  = 194024,
                ThriveInTheShadowsPeriodic = 194025,
                Dampening           = 110310
            };

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_ThriveInTheShadows = sSpellMgr->GetSpellInfo(eSpells::ThriveInTheShadows);
                SpellInfo const* l_Dispersion = GetSpellInfo();
                if (!l_Caster || !l_ThriveInTheShadows || !l_Dispersion)
                    return;

                if (!l_Caster->HasAura(eSpells::ThriveInTheShadows))
                    return;

                uint32 l_MaxHP = l_Caster->GetMaxHealth();
                uint32 l_Duration = l_Dispersion->GetMaxDuration();
                uint32 l_Amplitude = l_Dispersion->Effects[EFFECT_4].Amplitude;
                uint32 l_LifePct = l_ThriveInTheShadows->Effects[EFFECT_0].BasePoints;
                int32 l_DampeningValue = 0;

                if (AuraEffect* l_DampeningEffect = l_Caster->GetAuraEffect(eSpells::Dampening, EFFECT_0))
                {
                    l_DampeningValue = l_DampeningEffect->GetAmount();
                }

                if (!l_Amplitude || !l_Duration)
                    return;

                int32 l_HealAmount = CalculatePct(l_MaxHP, l_LifePct) / ((l_Duration / l_Amplitude) + 1);
                l_HealAmount = CalculatePct(l_HealAmount, 100 - l_DampeningValue);      ///< Heal amount must be affected by Dampening

                l_Caster->CastCustomSpell(l_Caster, eSpells::ThriveInTheShadowsPeriodic, &l_HealAmount, NULL, NULL, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_pri_thrive_in_the_shadows_AuraScript::HandleAfterApply, EFFECT_4, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_thrive_in_the_shadows_AuraScript();
        }
};

/// Last Update 7.0.3
/// Called by Holy Nova - 132157
class spell_pri_holy_nova : public SpellScriptLoader
{
    public:
        spell_pri_holy_nova() : SpellScriptLoader("spell_pri_holy_nova") { }

        class spell_pri_holy_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_holy_nova_SpellScript);

            enum  eSpells
            {
                HolyFire = 14914
            };

            bool m_AlreadyChecked = false;

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!m_AlreadyChecked && l_Player->HasSpellCooldown(eSpells::HolyFire) && roll_chance_i(20))
                    l_Player->RemoveSpellCooldown(eSpells::HolyFire, true);

                m_AlreadyChecked = true;
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_holy_nova_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_holy_nova_SpellScript();
        }
};

/// Last Update 7.0.3
/// Called by Holy Word: Chastise - 88625
class spell_pri_chastise : public SpellScriptLoader
{
    public:
        spell_pri_chastise() : SpellScriptLoader("spell_pri_chastise") { }

        class spell_pri_chastise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_chastise_SpellScript);

            enum  eSpells
            {
                Censure                 = 200199,
                HWChastiseIncapacitate  = 200196,
                HWChastiseStun          = 200200
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, l_Caster->HasAura(eSpells::Censure) ? eSpells::HWChastiseStun : eSpells::HWChastiseIncapacitate, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_chastise_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_chastise_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Serendipity - 63733
class spell_pri_serendipity : public SpellScriptLoader
{
    public:
        spell_pri_serendipity() : SpellScriptLoader("spell_pri_serendipity") { }

        class spell_pri_serendipity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_serendipity_AuraScript);

            enum eSpells
            {
                Heal                = 2060,
                FlashHeal           = 2061,
                PrayerOfHealing     = 596,
                Smite               = 585,
                HWChastise          = 88625,
                HWSanctify          = 34861,
                HWSerenity          = 2050,
                AlmaieshAura        = 211435,
                AlmaieshHWSerenity  = 211440,
                AlmaieshHWSanctify  = 211442,
                AlmaieshHWChastise  = 211443,
                Apotheosis          = 200183,
                LightOfTheNaaru     = 196985,
                BindingHeal         = 32546,
                PrayerOfMending     = 33076,
                Piety               = 197034,
                GlyphOfAngels       = 145722,
                GlyphOfAngelsTrigger = 145724,
                T20Holy2PBonus       = 242270
            };

            std::vector<uint32> l_BindingWordSpells =
            {
                eSpells::HWSanctify,
                eSpells::HWSerenity
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_DamageInfo || !l_Caster || !l_SpellInfo)
                    return;

                if (!CheckProcCooldown())
                    return;

                AuraEffect *l_LightOfTheNaaru = l_Caster->GetAuraEffect(eSpells::LightOfTheNaaru, EFFECT_0);
                int32 l_AddReduceCD = ((l_LightOfTheNaaru) ? -l_LightOfTheNaaru->GetAmount() : 0);

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_ProcSpellInfo || !l_Player)
                    return;

                SpellInfo const* l_SerenityInfo = sSpellMgr->GetSpellInfo(eSpells::HWSerenity);

                int l_modifierApotheosis = 1;
                if (l_Player->HasAura(eSpells::Apotheosis))
                {
                    SpellInfo const* l_ApotheosisInfos = sSpellMgr->GetSpellInfo(eSpells::Apotheosis);
                    if (l_ApotheosisInfos)
                    {
                        l_modifierApotheosis *= (1 + l_ApotheosisInfos->Effects[EFFECT_0].BasePoints / 100); /// Pct value of aura to decimal value
                    }
                }

                uint32 l_2PBonusCD = 0;
                if (l_Caster->HasAura(eSpells::T20Holy2PBonus))
                    if (SpellInfo const* l_2PBonusInfo = sSpellMgr->GetSpellInfo(eSpells::T20Holy2PBonus))
                        l_2PBonusCD = l_2PBonusInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS;

                l_AddReduceCD += l_2PBonusCD;

                switch (l_ProcSpellInfo->Id)
                {
                    case eSpells::Heal:
                    case eSpells::FlashHeal:
                    {
                        uint32 l_ReduceTime = (l_SpellInfo->Effects[EFFECT_2].BasePoints * TimeConstants::IN_MILLISECONDS + l_AddReduceCD) * l_modifierApotheosis;
                        for (uint8 l_i = 0; l_i <= 1; ++l_i)
                        {
                            if (!ReduceSpellCooldowns(l_Player, eSpells::HWSerenity, l_ReduceTime, l_i))
                            {
                                if (l_Player->HasAura(eSpells::AlmaieshAura))
                                {
                                    l_Player->CastSpell(l_Player, eSpells::AlmaieshHWSerenity, true);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    case eSpells::PrayerOfHealing:
                    case eSpells::PrayerOfMending:
                    {
                        if (l_ProcSpellInfo->Id == eSpells::PrayerOfMending && !l_Player->HasAura(eSpells::Piety))
                            break;

                        uint32 l_ReduceTime = (l_SpellInfo->Effects[EFFECT_1].BasePoints * TimeConstants::IN_MILLISECONDS + l_AddReduceCD) * l_modifierApotheosis;
                        for (uint8 l_i = 0; l_i <= 1; ++l_i)
                        {
                            if (!ReduceSpellCooldowns(l_Player, eSpells::HWSanctify, l_ReduceTime, l_i))
                            {
                                if (l_Player->HasAura(eSpells::AlmaieshAura))
                                {
                                    l_Player->CastSpell(l_Player, eSpells::AlmaieshHWSanctify, true);
                                    break;
                                }
                            }
                        }

                        break;
                    }
                    case eSpells::Smite:
                    {
                        uint32 l_ReduceTime = (l_SpellInfo->Effects[EFFECT_2].BasePoints * TimeConstants::IN_MILLISECONDS + l_AddReduceCD) * l_modifierApotheosis;
                        for (uint8 l_i = 0; l_i <= 1; ++l_i)
                        {
                            if (!ReduceSpellCooldowns(l_Player, eSpells::HWChastise, l_ReduceTime, l_i))
                            {
                                if (l_Player->HasAura(eSpells::AlmaieshAura))
                                {
                                    l_Player->CastSpell(l_Player, eSpells::AlmaieshHWChastise, true);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    case eSpells::BindingHeal:
                    {
                        uint32 l_ReduceTime = (l_SpellInfo->Effects[EFFECT_3].BasePoints * TimeConstants::IN_MILLISECONDS + l_AddReduceCD) * l_modifierApotheosis;
                        for (auto l_HWSpell : l_BindingWordSpells)
                        {
                            uint32 l_AlmaieshSpell = eSpells::AlmaieshHWSerenity;
                            if (l_HWSpell == eSpells::HWSanctify)
                                l_AlmaieshSpell = eSpells::AlmaieshHWSanctify;

                            for (uint8 l_i = 0; l_i <= 1; ++l_i)
                            {
                                if (!ReduceSpellCooldowns(l_Player, l_HWSpell, l_ReduceTime, l_i))
                                {
                                    if (l_Player->HasAura(eSpells::AlmaieshAura))
                                    {
                                        l_Player->CastSpell(l_Player, l_AlmaieshSpell, true);
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }

                if (l_ProcSpellInfo->Id == eSpells::Heal || l_ProcSpellInfo->Id == eSpells::FlashHeal ||
                    l_ProcSpellInfo->Id == eSpells::PrayerOfHealing || l_ProcSpellInfo->Id == eSpells::PrayerOfMending ||
                    l_ProcSpellInfo->Id == eSpells::Smite || l_ProcSpellInfo->Id == eSpells::BindingHeal)
                {
                    if (l_Player->HasAura(eSpells::GlyphOfAngels))
                        l_Player->CastSpell(l_Player, eSpells::GlyphOfAngelsTrigger, true);
                }
            }

            void OnUpdate(uint32 p_Diff)
            {
                if (m_ProcCooldown > p_Diff)
                {
                    m_ProcCooldown -= p_Diff;
                    return;
                }
                if (!CheckProcCooldown(true))
                    m_ProcCooldown = 0;
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pri_serendipity_AuraScript::HandleOnProc);
                OnAuraUpdate += AuraUpdateFn(spell_pri_serendipity_AuraScript::OnUpdate);
            }

        private:

            bool CheckProcCooldown(bool fromUpdate = false)
            {
                if (m_ProcCooldown <= 0)
                {
                    if (!fromUpdate)
                        m_ProcCooldown = 0.5 * IN_MILLISECONDS;
                    return true;
                }

                return false;
            }

            bool ReduceSpellCooldowns(Player* p_Player, uint32 p_SpellId, uint32 p_ReduceTime, bool p_OnlyCheck = false)
            {
                switch (p_SpellId)
                {
                    case eSpells::HWSerenity:
                    {
                        if (SpellInfo const* l_SerenityInfo = sSpellMgr->GetSpellInfo(eSpells::HWSerenity))
                        {
                            if (l_SerenityInfo->ChargeCategoryEntry)
                            {
                                auto l_Itr = p_Player->m_CategoryCharges.find(l_SerenityInfo->ChargeCategoryEntry->Id);
                                if (l_Itr == p_Player->m_CategoryCharges.end() || l_Itr->second.empty())
                                    return false;

                                if (!p_OnlyCheck)
                                    p_Player->ReduceChargeCooldown(l_SerenityInfo->ChargeCategoryEntry, p_ReduceTime);
                            }
                        }
                        break;
                    }
                    case eSpells::HWSanctify:
                        if (!p_Player->HasSpellCooldown(eSpells::HWSanctify))
                            return false;

                        if (!p_OnlyCheck)
                            p_Player->ReduceSpellCooldown(eSpells::HWSanctify, p_ReduceTime);
                        break;
                    case eSpells::HWChastise:
                        if (!p_Player->HasSpellCooldown(eSpells::HWChastise))
                            return false;

                        if (!p_OnlyCheck)
                            p_Player->ReduceSpellCooldown(eSpells::HWChastise, p_ReduceTime);
                        break;

                    default:
                        break;
                }
                return true;
            }

            private:
                uint32 m_ProcCooldown = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_serendipity_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Invoke the Naaru - 196684 and 196705
class spell_pri_invoke_the_naaru : public SpellScriptLoader
{
    public:
        spell_pri_invoke_the_naaru() : SpellScriptLoader("spell_pri_invoke_the_naaru") { }

        class spell_pri_invoke_the_naaru_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_invoke_the_naaru_AuraScript);

            enum eSpells
            {
                HWChastise          = 88625,
                HWSanctify          = 34861,
                HWSerenity          = 2050,
                Renew               = 139,
                Heal                = 2060,
                FlashHeal           = 2061,
                PrayerOfHealing     = 596,
                PrayerOfMending     = 33076,
                DivineHymn          = 64843,
                Smite               = 585,
                HolyFire            = 14914,
                HolyNova            = 132157,

                ChastiseProcAura    = 196684,
                DamageHealProcAura  = 196705,

                SummonTuure         = 196685
            };

            enum eNpcs
            {
                TuureId = 99904
            };

            /*
                @ http://www.wowhead.com/item=128825/tuure-beacon-of-the-naaru&bonus=0&spec=71#comments:id=2359192
                Your spell                                  T'uure's version
                Renew                                       Tranquil Light
                Heal & Flash Heal & Holy Word: Serenity     Healing Light
                Prayer of Healing & Holy Word: Sanctify     Dazzling Lights
                Prayer of Mending & Divine Hymn             Blessed Light
                Smite & Holy Fire & Holy Word: Chastise     Searing Light
                Holy Nova                                   Light Eruption
            */
            enum eDupeSpells
            {
                /// dupes
                TranquilLight   = 196816,
                HealingLight    = 196809,
                DazzlingLights  = 196810,
                BlessedLight    = 196813,
                SearingLight    = 196811,
                LightEruption   = 196812
            };

            std::map<uint32, uint32> l_TuureSpellIds =
            {
                { eSpells::Renew,           eDupeSpells::TranquilLight },
                { eSpells::Heal,            eDupeSpells::HealingLight },
                { eSpells::FlashHeal,       eDupeSpells::HealingLight },
                { eSpells::HWSerenity,      eDupeSpells::HealingLight },
                { eSpells::PrayerOfHealing, eDupeSpells::DazzlingLights },
                { eSpells::HWSanctify,      eDupeSpells::DazzlingLights },
                { eSpells::PrayerOfMending, eDupeSpells::BlessedLight },
                { eSpells::DivineHymn,      eDupeSpells::BlessedLight },
                { eSpells::Smite,           eDupeSpells::SearingLight },
                { eSpells::HolyFire,        eDupeSpells::SearingLight },
                { eSpells::HWChastise,      eDupeSpells::SearingLight },
                { eSpells::HolyNova,        eDupeSpells::LightEruption }
            };

            Guid128 m_LastProcingCast;

            void HandleChastiseProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                if (l_SpellInfo->Id != eSpells::HWChastise && l_SpellInfo->Id != eSpells::HWSanctify && l_SpellInfo->Id != eSpells::HWSerenity)
                    return;

                if (l_Caster->HasAura(eSpells::DamageHealProcAura))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonTuure, true);
            }

            void HandleDupeSpell(uint32 p_SpellId, Unit* p_Target)
            {
                Unit* l_Caster = GetCaster();
                uint32 l_DupeSpell = l_TuureSpellIds[p_SpellId];
                if (!l_DupeSpell)
                    return;

                std::list<Creature*> l_Tuures;
                l_Caster->GetAllMinionsByEntry(l_Tuures, eNpcs::TuureId);
                if (!l_Tuures.empty())
                    l_Tuures.front()->CastSpell(p_Target, l_DupeSpell, true);
            }

            void HandleDamageHealProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_SpellInfo || !l_Spell)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                if (m_LastProcingCast == l_Spell->GetCastGuid())
                    return;

                m_LastProcingCast = l_Spell->GetCastGuid();

                HandleDupeSpell(l_SpellInfo->Id, l_Target);
            }

            void HandlePoM(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                HandleDupeSpell(GetSpellInfo()->Id, l_Target);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::ChastiseProcAura)
                    OnProc += AuraProcFn(spell_pri_invoke_the_naaru_AuraScript::HandleChastiseProc);
                else if (m_scriptSpellId == eSpells::DamageHealProcAura)
                    OnProc += AuraProcFn(spell_pri_invoke_the_naaru_AuraScript::HandleDamageHealProc);
                else if (m_scriptSpellId == eSpells::DivineHymn)
                    AfterEffectApply += AuraEffectApplyFn(spell_pri_invoke_the_naaru_AuraScript::HandlePoM, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_invoke_the_naaru_AuraScript();
        }

        class spell_pri_invoke_the_naaru_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_invoke_the_naaru_SpellScript);

            enum eSpells
            {
                PrayerOfMending     = 33076
            };

            enum eNpcs
            {
                TuureId = 99904
            };

            /*
                @ http://www.wowhead.com/item=128825/tuure-beacon-of-the-naaru&bonus=0&spec=71#comments:id=2359192
                Your spell                                  T'uure's version
                Renew                                       Tranquil Light
                Heal & Flash Heal & Holy Word: Serenity     Healing Light
                Prayer of Healing & Holy Word: Sanctify     Dazzling Lights
                Prayer of Mending & Divine Hymn             Blessed Light
                Smite & Holy Fire & Holy Word: Chastise     Searing Light
                Holy Nova                                   Light Eruption
            */
            enum eDupeSpells
            {
                /// dupes
                BlessedLight    = 196813
            };

            void HandleHit(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                std::list<Creature*> l_Tuures;
                l_Caster->GetAllMinionsByEntry(l_Tuures, eNpcs::TuureId);
                if (!l_Tuures.empty())
                    l_Tuures.front()->CastSpell(l_Target, eDupeSpells::BlessedLight, true);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::PrayerOfMending)
                    OnEffectHitTarget += SpellEffectFn(spell_pri_invoke_the_naaru_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_invoke_the_naaru_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dazzling Lights - 196810
class spell_pri_dazzling_lights : public SpellScriptLoader
{
    public:
        spell_pri_dazzling_lights() : SpellScriptLoader("spell_pri_dazzling_lights") { }

        class spell_pri_dazzling_lights_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_dazzling_lights_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                if (p_Targets.size() > l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    JadeCore::RandomResizeList(p_Targets, l_SpellInfo->Effects[EFFECT_1].BasePoints);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_dazzling_lights_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_dazzling_lights_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Prayer of Mending - 33076
class spell_pri_prayer_of_mending : public SpellScriptLoader
{
    public:
        spell_pri_prayer_of_mending() : SpellScriptLoader("spell_pri_prayer_of_mending") { }

        class spell_pri_prayer_of_mending_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_prayer_of_mending_SpellScript);

            enum eSpells
            {
                PrayerOfMending     = 33076,
                PrayerOfMendingAura = 41635
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                bool l_hasAura = false;
                if (l_Target->HasAura(eSpells::PrayerOfMendingAura))
                    l_hasAura = true;

                l_Player->CastSpell(l_Target, eSpells::PrayerOfMendingAura, true);
                Aura* l_Aura = l_Target->GetAura(eSpells::PrayerOfMendingAura, l_Caster->GetGUID());
                if (!l_Aura)
                    return;

                uint16 l_BasePoint = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                if (!l_hasAura)
                    l_Aura->SetStackAmount(l_SpellInfo->Effects[EFFECT_0].BasePoints);
                else
                {
                    /// It's 'AfterCast' handler so we already have +1 stack
                    int32 l_Stacks = l_Aura->GetStackAmount() - 1;
                    l_Aura->SetStackAmount(std::min(l_Stacks + l_BasePoint, l_BasePoint * 2));
                }

                // Tooltip
                int32 l_Bp = 1 + CalculatePct(l_Caster->GetStat(STAT_INTELLECT), 175.0f);
                l_Aura->GetEffect(EFFECT_0)->SetAmount(l_Bp);

                l_Player->SetPrayerOfMendingTarget(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_prayer_of_mending_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_prayer_of_mending_SpellScript();
        }

};

/// Last Update 7.3.5 build 25996
/// Called By Flash Heal - 2061, Heal - 2060, Serenity - 2050
/// Called for Rammal's Ulterior Motive - 234711
class spell_pri_rammals_ulterior_motive : public SpellScriptLoader
{
public:
    spell_pri_rammals_ulterior_motive() : SpellScriptLoader("spell_pri_rammals_ulterior_motive") { }

    class spell_pri_rammals_ulterior_motive_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_rammals_ulterior_motive_SpellScript);

        enum eSpells
        {
            RammalsUlteriorMotive = 234711,
            FlashHeal = 2061,
            Heal = 2060,
            Serenity = 2050
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Caster || !l_Target)
                return;

            if (l_Target->HasAura(eSpells::RammalsUlteriorMotive, l_Caster->GetGUID()))
                l_Target->RemoveAura(eSpells::RammalsUlteriorMotive);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_pri_rammals_ulterior_motive_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_rammals_ulterior_motive_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Prayer of Mending - 41635
class spell_pri_prayer_of_mending_aura : public SpellScriptLoader
{
    public:
        spell_pri_prayer_of_mending_aura() : SpellScriptLoader("spell_pri_prayer_of_mending_aura") { }

        class spell_pri_prayer_of_mending_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_prayer_of_mending_aura_AuraScript);

            enum eSpells
            {
                PrayerOfMendingRange        = 155793,
                PrayerOfMendingVisualBounce = 225275,
                PrayerOfMendingHeal         = 33110,
                RammalsUlteriorMotiveAura   = 234710,
                RammalsUlteriorMotiveProc   = 234711,
                Benediction                 = 193157,
                Renew                       = 139,
                SayYourPrayer               = 196358,
                HolyMendingAura             = 196779,
                HolyMending                 = 196781,
                SpiritOfRedemption          = 20711
            };

            enum eArtifactPowerId
            {
                SayYourPrayers              = 842
            };

            bool CheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster)
                    return false;

                /// All damage has been absorbed
                if (l_DamageInfo->GetAmount() == 0 && l_DamageInfo->GetAbsorb() > 0)
                    return false;

                if (p_ProcEventInfo.GetTypeMask() & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS)
                    return false;

                return true;
            }

            void HandleAfterProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Caster || !GetSpellInfo())
                    return;

                uint32 l_SpellId = GetSpellInfo()->Id;
                float l_Range = 0.0f;
                if (SpellInfo const* l_RangeSpellInfo = sSpellMgr->GetSpellInfo(eSpells::PrayerOfMendingRange))
                    l_Range = l_RangeSpellInfo->Effects[EFFECT_0].RadiusEntry->RadiusMax;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                /// heal first
                int32 l_Bp = 1 + CalculatePct(l_Caster->GetStat(STAT_INTELLECT), 175.0f);
                l_Player->CastCustomSpell(l_Target, eSpells::PrayerOfMendingHeal, &l_Bp, nullptr, nullptr, true);
                if (l_Caster->HasAura(eSpells::RammalsUlteriorMotiveAura))
                    l_Caster->CastSpell(l_Target, eSpells::RammalsUlteriorMotiveProc, true);

                std::list<Unit*> l_TempList;
                l_Player->GetRaidMembers(l_TempList);
                l_TempList.remove_if([l_Target, l_Range](Unit* p_Unit) -> bool
                {
                    if (l_Target == p_Unit || !p_Unit->IsWithinDistInMap(l_Target, l_Range))
                        return true;
                    return false;
                });

                if (l_TempList.empty())
                {
                    l_Player->SetPrayerOfMendingTarget(0);
                    l_Target->RemoveAurasDueToSpell(l_SpellId, l_Caster->GetGUID());
                    if (l_Caster->HasAura(eSpells::Benediction))
                    {
                        SpellInfo const* l_BenedictionInfo = sSpellMgr->GetSpellInfo(eSpells::Benediction);
                        DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                        if (!l_DamageInfo && !l_BenedictionInfo)
                            return;

                        Unit* l_CurrentTarget = l_DamageInfo->GetTarget();
                        if (l_CurrentTarget && roll_chance_i(l_BenedictionInfo->Effects[EFFECT_0].BasePoints) && l_DamageInfo->GetAmount() > 0)
                            l_Caster->CastSpell(l_Target, eSpells::Renew, true);
                    }
                    return;
                }
                else
                {
                    l_TempList.sort(JadeCore::HealthPctExcludeAuraOrderPredPlayer(m_scriptSpellId, l_Caster->GetGUID()));

                    bool l_IgnoreAura = l_TempList.back()->HasAura(m_scriptSpellId);
                    bool l_PriorizeTank = false;
                    if (Player* l_PlayerTarget = l_Target->ToPlayer())
                    {
                        if (l_PlayerTarget->GetRoleForGroup() != Roles::ROLE_TANK)
                            l_PriorizeTank = true;
                    }

                    if (l_PriorizeTank)
                    {
                        std::list<Unit*> l_Tanks;
                        for (Unit* l_Unit : l_TempList)
                        {
                            Player* l_PlayerTarget = l_Unit->ToPlayer();
                            if (!l_PlayerTarget || l_PlayerTarget->GetRoleForGroup() != Roles::ROLE_TANK || (!l_IgnoreAura && l_PlayerTarget->HasAura(m_scriptSpellId)))
                                continue;

                            l_Tanks.push_back(l_PlayerTarget);
                        }

                        if (!l_Tanks.empty())
                            l_TempList = l_Tanks;
                    }

                    l_TempList.resize(1);
                }

                Unit* l_NewTarget = l_TempList.front();
                if (!l_NewTarget)
                {
                    l_Player->SetPrayerOfMendingTarget(0);
                    return;
                }

                Aura* l_OldAura = l_Target->GetAura(l_SpellId, l_Caster->GetGUID());
                if (!l_OldAura)
                {
                    l_Player->SetPrayerOfMendingTarget(0);
                    return;
                }

                int8 l_StackAmount = l_OldAura->GetStackAmount();
                l_Target->RemoveAurasDueToSpell(l_SpellId, l_Caster->GetGUID());
                if (l_Caster->HasAura(eSpells::Benediction))
                {
                    SpellInfo const* l_BenedictionInfo = sSpellMgr->GetSpellInfo(eSpells::Benediction);
                    DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                    if (!l_DamageInfo && !l_BenedictionInfo)
                        return;

                    Unit* l_CurrentTarget = l_DamageInfo->GetTarget();
                    if (l_CurrentTarget && roll_chance_i(l_BenedictionInfo->Effects[EFFECT_0].BasePoints) && l_DamageInfo->GetAmount() > 0)
                        l_Caster->CastSpell(l_Target, eSpells::Renew, true);
                }

                if (l_StackAmount > 1)
                {
                    l_Target->CastSpell(l_NewTarget, eSpells::PrayerOfMendingVisualBounce, true);
                    l_Player->CastSpell(l_NewTarget, l_SpellId, true);
                    if (l_NewTarget->HasAura(eSpells::Renew) && l_Player->HasAura(eSpells::HolyMendingAura))
                        l_Player->CastSpell(l_NewTarget, eSpells::HolyMending, true);

                    Aura* l_NewAura = l_NewTarget->GetAura(l_SpellId, l_Caster->GetGUID());
                    if (!l_NewAura)
                    {
                        l_Player->SetPrayerOfMendingTarget(0);
                        return;
                    }

                    if (l_Player->HasAura(eSpells::SayYourPrayer))
                    {
                        SpellInfo const* l_SayYourPrayerInfo = sSpellMgr->GetSpellInfo(eSpells::SayYourPrayer);
                        if (l_SayYourPrayerInfo  && roll_chance_i(l_SayYourPrayerInfo->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eArtifactPowerId::SayYourPrayers)))
                            l_StackAmount++;
                    }

                    l_NewAura->SetStackAmount(l_StackAmount - 1);
                    l_Player->SetPrayerOfMendingTarget(l_NewTarget->GetGUID());
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_prayer_of_mending_aura_AuraScript::CheckProc);
                AfterProc += AuraProcFn(spell_pri_prayer_of_mending_aura_AuraScript::HandleAfterProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_prayer_of_mending_aura_AuraScript();
        }
};

/// Last Update 7.0.3 - 22293
/// Smite (Absorb) - 208771
class spell_pri_smite_absorb : public SpellScriptLoader
{
    public:
        spell_pri_smite_absorb() : SpellScriptLoader("spell_pri_smite_absorb") { }

        class spell_pri_smite_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_smite_absorb_AuraScript);

            enum eSpells
            {
                SmiteShielAura = 208772
            };

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Attacker = p_DmgInfo.GetActor();
                if (l_Attacker == nullptr)
                    return;

                if (AuraEffect* l_AuraEffect = l_Attacker->GetAuraEffect(eSpells::SmiteShielAura, EFFECT_0))
                {
                    int32 l_Amount = l_AuraEffect->GetAmount();

                    if (p_DmgInfo.GetAmount() > l_Amount)
                        p_AbsorbAmount = l_Amount;
                    else
                        p_AbsorbAmount = p_DmgInfo.GetAmount();

                    if (p_AbsorbAmount > l_AuraEffect->GetAmount())
                        l_AuraEffect->GetBase()->Remove(AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL);
                    else
                        l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() + -int32(p_AbsorbAmount));
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
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_smite_absorb_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_smite_absorb_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_smite_absorb_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22293
/// Divinity - 197031
class spell_pri_divinity : public SpellScriptLoader
{
    public:
        spell_pri_divinity() : SpellScriptLoader("spell_pri_divinity") { }

        class spell_pri_divinity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_divinity_AuraScript);

            enum eSpells
            {
                HWSanctify = 34861,
                HWSerenity = 2050
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_DamageInfo || !l_Caster)
                    return;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_ProcSpellInfo)
                    return;

                if (l_ProcSpellInfo->Id != eSpells::HWSanctify && l_ProcSpellInfo->Id != eSpells::HWSerenity)
                    return;

                l_Caster->CastSpell(l_Caster, p_AurEff->GetTriggerSpell(), true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_divinity_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_divinity_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Power Word:  Shield - 17
/// Called for Void Shield - 199144
class spell_pri_void_shield : public SpellScriptLoader
{
    public:
        spell_pri_void_shield() : SpellScriptLoader("spell_pri_void_shield") { }

        class spell_pri_void_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_shield_SpellScript);

            enum eSpells
            {
                VoidShield      = 199144,
                VoidShieldBuff  = 199145
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Target || !l_Caster)
                    return;

                if (l_Caster != l_Target || !l_Caster->HasAura(eSpells::VoidShield))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::VoidShieldBuff, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_void_shield_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_shield_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Holy Concentration - 221661
class PlayerScript_pri_holy_concentration : public PlayerScript
{
    public:
        PlayerScript_pri_holy_concentration() :PlayerScript("PlayerScript_Holy_Concentration") { }

        enum eSpell
        {
            HolyConcentration       = 221661,
            HolyConcentrationBuff   = 221660
        };

        void OnSpellInterrupt(Player* p_Player, Spell* p_Spell, Spell* p_InterruptingSpell, uint32 /*p_InterruptingSpellId*/)
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_PRIEST_HOLY)
                return;

            if (!p_Player->HasAura(eSpell::HolyConcentration))
                return;

            if (!p_InterruptingSpell)
                return;

            if (p_InterruptingSpell->GetSpellInfo()->HasEffectMechanic(Mechanics::MECHANIC_INTERRUPT) || p_InterruptingSpell->GetSpellInfo()->Mechanic == Mechanics::MECHANIC_INTERRUPT)
            {
                if (!p_Player->HasAura(eSpell::HolyConcentrationBuff))
                    p_Player->CastSpell(p_Player, eSpell::HolyConcentrationBuff, true);
            }
        }
};

/// Called by Void Shield - 199145
class spell_pri_void_shield_aura : public SpellScriptLoader
{
    public:
        spell_pri_void_shield_aura() : SpellScriptLoader("spell_pri_void_shield_aura") { }

        class spell_pri_void_shield_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_void_shield_aura_AuraScript);

            enum eSpells
            {
                VoidShield      = 199144,
                VampiricTouch   = 34914
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VoidShield);
                if (!l_DamageInfo || !l_SpellInfo || !l_Caster)
                    return;

                int32 l_Damage = l_DamageInfo->GetAmount();
                int32 l_HealPct = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                if (!l_DamageInfo->GetSpellInfo() || l_DamageInfo->GetSpellInfo()->Id == eSpells::VampiricTouch)
                    return;

                l_Caster->HealBySpell(l_Caster, GetSpellInfo(), CalculatePct(l_Damage, l_HealPct));
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pri_void_shield_aura_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_void_shield_aura_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Mind Flay - 15407
/// Called for Mind Trauma - 199445
class spell_pri_mind_trauma : public SpellScriptLoader
{
    public:
        spell_pri_mind_trauma() : SpellScriptLoader("spell_pri_mind_trauma") { }
        class spell_pri_mind_trauma_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mind_trauma_AuraScript);

            uint8 m_Ticks = 0;

            enum eSpells
            {
                MindTrauma       = 199445,
                MindTraumaBuff   = 247776,
                MindTraumaDebuff = 247777
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                ++m_Ticks;

                if (m_Ticks < p_AuraEffect->GetTotalTicks())
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MindTrauma);
                SpellInfo const* l_SpellInfoCast = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_SpellInfoCast)
                    return;

                if (!l_Caster->HasAura(eSpells::MindTrauma))
                    return;

                if (!l_Caster->IsValidAttackTarget(l_Target))
                    return;

                if (Aura* l_DebuffAura = l_Target->GetAura(eSpells::MindTraumaDebuff, l_Caster->GetGUID()))
                {
                    if (l_DebuffAura->GetStackAmount() >= 4)
                    {
                        l_DebuffAura->SetDuration(l_DebuffAura->GetMaxDuration());
                        if (Aura* l_BuffAura = l_Caster->GetAura(eSpells::MindTraumaBuff, l_Caster->GetGUID()))
                            l_BuffAura->SetDuration(l_BuffAura->GetMaxDuration());

                        return;
                    }
                }

                if (Aura* l_BuffAura = l_Caster->GetAura(eSpells::MindTraumaBuff, l_Caster->GetGUID()))
                {
                    if (l_BuffAura->GetStackAmount() >= 8)
                    {
                        l_BuffAura->SetDuration(l_BuffAura->GetMaxDuration());
                        return;
                    }
                }

                l_Caster->CastSpell(l_Target, eSpells::MindTraumaDebuff, true);
                l_Caster->CastSpell(l_Caster, eSpells::MindTraumaBuff, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_mind_trauma_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mind_trauma_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called for Mind Trauma (debuff) - 247777
class spell_pri_mind_trauma_debuff : public SpellScriptLoader
{
    public:
        spell_pri_mind_trauma_debuff() : SpellScriptLoader("spell_pri_mind_trauma_debuff") { }
        class spell_pri_mind_trauma_debuff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mind_trauma_debuff_AuraScript);

            enum eSpells
            {
                MindTraumaBuff   = 247776,
                MindTraumaDebuff = 247777
            };

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                Aura* l_DebuffAura = GetAura();
                if (!l_Target || !l_Caster || !l_DebuffAura)
                    return;

                if (Aura* l_BuffAura = l_Caster->GetAura(eSpells::MindTraumaBuff, l_Caster->GetGUID()))
                {
                    if (l_BuffAura->GetStackAmount() > l_DebuffAura->GetStackAmount())
                        l_BuffAura->ModStackAmount(-(l_DebuffAura->GetStackAmount()));
                    else
                        l_BuffAura->Remove();
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_mind_trauma_debuff_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mind_trauma_debuff_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Leap Of Faith - 73325
/// Called for Delivered from Evil - 196611
class spell_pri_delivered_from_evil : public SpellScriptLoader
{
    public:
        spell_pri_delivered_from_evil() : SpellScriptLoader("spell_pri_delivered_from_evil") { }

        class spell_pri_delivered_from_evil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_delivered_from_evil_SpellScript);

            enum eSpells
            {
                DeliveredFromEvil   = 196611
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster ||!l_Target)
                    return;

                if (!l_Caster->HasAura(eSpells::DeliveredFromEvil))
                    return;

                l_Target->RemoveMovementImpairingAuras();
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_delivered_from_evil_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_delivered_from_evil_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Deliverance - 196633
class spell_pri_deliverance : public SpellScriptLoader
{
    public:
        spell_pri_deliverance() : SpellScriptLoader("spell_pri_deliverance") { }

        class spell_pri_deliverance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_deliverance_AuraScript);

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_SpellInfo)
                    return;

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(l_Target->GetMaxHealth() * l_SpellInfo->Effects[EFFECT_0].BasePoints / 100);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_deliverance_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_deliverance_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Power Word: Fortitude - 211681
class spell_pri_power_word_fortitude : public SpellScriptLoader
{
    public:
        spell_pri_power_word_fortitude() : SpellScriptLoader("spell_pri_power_word_fortitude") { }

        class spell_pri_power_word_fortitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_power_word_fortitude_AuraScript);

            enum eSpells
            {
                PowerWordFortitude = 211681
            };

            void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                std::list<uint64>& l_PowerWordFortitudeTarget = l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::PowerWordFortitudeTargets];
                if (std::find(l_PowerWordFortitudeTarget.begin(), l_PowerWordFortitudeTarget.end(), l_Target->GetGUID()) == l_PowerWordFortitudeTarget.end())
                {
                    if (l_PowerWordFortitudeTarget.size() > 2)
                    {
                        Unit* l_FirstTarget = sObjectAccessor->GetUnit(*l_Caster, l_PowerWordFortitudeTarget.front());
                        if (l_FirstTarget)
                            l_FirstTarget->RemoveAurasDueToSpell(eSpells::PowerWordFortitude);
                        else
                            l_PowerWordFortitudeTarget.pop_front();
                    }
                    l_PowerWordFortitudeTarget.push_back(l_Target->GetGUID());
                }
            }

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::PowerWordFortitudeTargets].remove(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_power_word_fortitude_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_power_word_fortitude_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_power_word_fortitude_AuraScript();
        }

        class spell_pri_power_word_fortitude_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_power_word_fortitude_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Aura* l_Aura = l_Target->GetAura(GetSpellInfo()->Id))
                {
                    Unit* l_AuraCaster = l_Aura->GetCaster();

                    if (l_AuraCaster)
                        l_AuraCaster->m_SpellHelper.GetUint64List()[eSpellHelpers::PowerWordFortitudeTargets].remove(l_Target->GetGUID());

                    l_Target->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                }

            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_pri_power_word_fortitude_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_power_word_fortitude_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Power Word: Fortitude - 211681
class PlayerScript_pri_power_word_fortitude : public PlayerScript
{
public:
    PlayerScript_pri_power_word_fortitude() : PlayerScript("PlayerScript_pri_power_word_fortitude") {}

    enum eSpells
    {
        PowerWordFortitude = 211681
    };

    void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
    {
        if (!p_Player->HasAura(eSpells::PowerWordFortitude))
            return;

        if (p_Player->CanApplyPvPSpellModifiers())
            return;

        p_Player->RemoveAurasDueToSpell(eSpells::PowerWordFortitude);
    }

    void OnSpellRemoved(Player* p_Player, uint32 p_SpellID) override
    {
        if (p_SpellID == eSpells::PowerWordFortitude && p_Player->CanApplyPvPSpellModifiers())
        {
            std::list<uint64>& l_PowerWordFortitudeTargets = p_Player->m_SpellHelper.GetUint64List()[eSpellHelpers::PowerWordFortitudeTargets];
            std::list<uint64> l_TargetsToRemove;
            for (auto l_TargetGuid : l_PowerWordFortitudeTargets)
                l_TargetsToRemove.push_back(l_TargetGuid);

            while (!l_TargetsToRemove.empty())
            {
                if (Unit* l_Target = sObjectAccessor->FindUnit(l_TargetsToRemove.front()))
                    l_Target->RemoveAurasDueToSpell(eSpells::PowerWordFortitude);

                l_TargetsToRemove.pop_front();
            }
        }
    }
};

/// Last Update 7.0.3 Build 22522
/// Called by Mind Flay - 15407
/// Called for Mind Sear - 234702
class spell_pri_mind_sear : public SpellScriptLoader
{
    public:
        spell_pri_mind_sear() : SpellScriptLoader("spell_pri_mind_sear") { }

        enum eSpells
        {
            ShadowWordPain = 589,
            MindSearPassive = 234702,
            MindSearAOE = 237388
        };

        static bool CanCastMindSearAoe(Unit* p_Caster, Unit* p_Target)
        {
            return p_Caster && p_Target && p_Caster->HasAura(eSpells::MindSearPassive) && p_Target->HasAura(eSpells::ShadowWordPain, p_Caster->GetGUID());
        }

        class spell_pri_mind_sear_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mind_sear_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!CanCastMindSearAoe(l_Caster, l_Target))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::MindSearAOE, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_mind_sear_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mind_sear_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Mind Sear - 237388
class spell_pri_mind_sear_energize : public SpellScriptLoader
{
    public:
        spell_pri_mind_sear_energize() : SpellScriptLoader("spell_pri_mind_sear_energize") { }

        class spell_pri_mind_sear_energize_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_sear_energize_SpellScript);

            enum eSpells
            {
                MindSearEnergize = 208232
            };

            void HandleOnHit(SpellEffIndex /*p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MindSearEnergize, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_mind_sear_energize_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mind_sear_energize_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Mind Spike - 73510, Void Bolt - 228266, Mind Blast - 8092, Mind Flay - 15407
class spell_pri_sphere_of_insanity_dmg : public SpellScriptLoader
{
    public:
        spell_pri_sphere_of_insanity_dmg() : SpellScriptLoader("spell_pri_sphere_of_insanity_dmg") { }

        enum eSpells
        {
            SphereOfInsanityAura = 194179,
            SphereOfInsanitySummon = 194182,
            SphereOfInsanityDmg = 194238,
            MindFlay = 15407
        };

        enum eNPC
        {
            SphereOfInsanityNPC = 98680
        };

        static void HitTargets(Unit* p_Caster, int32 p_Damage, uint32 p_SpellId)
        {
            if (!p_Caster || !p_Caster->HasAura(eSpells::SphereOfInsanityAura))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SphereOfInsanitySummon);
            GuidList* l_Summoned = p_Caster->GetSummonList(eNPC::SphereOfInsanityNPC);

            if (!l_SpellInfo || !l_Summoned)
                return;

            int32 l_Damage = CalculatePct(p_Damage, l_SpellInfo->Effects[2].BasePoints);

            for (auto l_SphereGUID : *l_Summoned)
            {
                Unit* l_Sphere = Unit::GetUnit(*p_Caster, l_SphereGUID);

                if (!l_Sphere)
                    continue;

                for (auto l_TargetGUID : p_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::ShadowWordPainTargets])
                {
                    Unit* l_Target = Unit::GetUnit(*p_Caster, l_TargetGUID);

                    if (!l_Target)
                        continue;
                    l_Sphere->CastCustomSpell(l_Target, eSpells::SphereOfInsanityDmg, &l_Damage, nullptr, nullptr, true);
                }
            }
        }

        class spell_pri_sphere_of_insanity_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_sphere_of_insanity_dmg_SpellScript);

            void HandleOnHit()
            {
               spell_pri_sphere_of_insanity_dmg::HitTargets(GetCaster(), GetHitDamage(), m_scriptSpellId);
            }

            void Register() override
            {
                if (m_scriptSpellId != eSpells::MindFlay)
                    AfterHit += SpellHitFn(spell_pri_sphere_of_insanity_dmg_SpellScript::HandleOnHit);
            }
        };

        class spell_pri_sphere_of_insanity_dmg_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_sphere_of_insanity_dmg_AuraScript);

            void HandleEffectHit(AuraEffect const* p_AuraEffect)
            {
                spell_pri_sphere_of_insanity_dmg::HitTargets(GetCaster(), p_AuraEffect->GetPeriodicHitDamage(), m_scriptSpellId);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::MindFlay)
                    AfterEffectPeriodic += AuraEffectPeriodicFn(spell_pri_sphere_of_insanity_dmg_AuraScript::HandleEffectHit, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_sphere_of_insanity_dmg_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_sphere_of_insanity_dmg_AuraScript();
        }
};

struct DamageHistoryEntry
{
    DamageHistoryEntry(uint32 p_Damage, time_t p_Timestamp)
    {
        Damage = p_Damage;
        Timestamp = p_Timestamp;
    }

    uint32           Damage;
    uint32           Timestamp;
};

typedef std::deque<DamageHistoryEntry>                              DamagesHistory;

typedef ACE_Based::LockedMap<uint64/*PlayerGUID*/, DamagesHistory>  DamagesHistoryMap;

class PlayerScript_Mental_Fortitude : public PlayerScript
{
    private:
        static DamagesHistoryMap m_HistoryDamagesPlayers;

        enum eSpells
        {
            VampiricTouch           = 34914,
            MentalFortitude         = 194018,
            MentalFortitudeAbsorb   = 194022
        };

        enum eTimers
        {
            Timelapse = 3 * IN_MILLISECONDS
        };

    public:
        PlayerScript_Mental_Fortitude() : PlayerScript("PlayerScript_Mental_Fortitude")
        {
        }

        void OnLogin(Player* p_Player) override
        {
            m_HistoryDamagesPlayers.erase(p_Player->GetGUID());
        }

        void OnDealDamage(Player* p_Player, Unit* p_Target, DamageEffectType p_DamageEffectType, uint32 p_Damage, SpellSchoolMask p_SchoolMask, CleanDamage const* p_CleanDamage) override
        {
            if (p_Player->getClass() != Classes::CLASS_PRIEST)
                return;

            if (p_DamageEffectType != DamageEffectType::DOT
                && p_DamageEffectType != DamageEffectType::SPELL_DIRECT_DAMAGE)
                return;

            if (!p_Player->HasAura(eSpells::MentalFortitudeAbsorb))
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
            if (p_Player->getClass() != CLASS_PRIEST)
                return;

            if (m_HistoryDamagesPlayers.find(p_Player->GetGUID()) == m_HistoryDamagesPlayers.end())
                return;

            DamagesHistory& l_History = m_HistoryDamagesPlayers[p_Player->GetGUID()];
            DamagesHistory l_CleanHistory;

            for (DamagesHistory::iterator l_Itr = l_History.begin(); l_Itr != l_History.end(); l_Itr++)
            {
                if ((*l_Itr).Timestamp + eTimers::Timelapse > time(NULL))
                    l_CleanHistory.push_back(*l_Itr);
            }

            l_History = l_CleanHistory;
        }

        static uint32 GetDamagesDoneInLastPeriod(uint64 p_PlayerGUID)
        {
            DamagesHistory& l_DamagesHistory = m_HistoryDamagesPlayers[p_PlayerGUID];
            uint32 l_Damages = 0;

            for (DamageHistoryEntry l_Entry : l_DamagesHistory)
                l_Damages += l_Entry.Damage;

            return l_Damages;
        }
};

DamagesHistoryMap PlayerScript_Mental_Fortitude::m_HistoryDamagesPlayers;

/// Last Update 7.1.5 Build 23420
/// Mental Fortitude - 194018
class spell_pri_mental_fortitude : public SpellScriptLoader
{
    public:
        spell_pri_mental_fortitude() : SpellScriptLoader("spell_pri_mental_fortitude") { }

        class spell_pri_mental_fortitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mental_fortitude_AuraScript);

            enum eSpells
            {
                VampiricTouch           = 34914,
                MentalFortitude         = 194018,
                MentalFortitudeAbsorb   = 194022
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::VampiricTouch)
                    return false;

                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner || !l_Owner->IsFullHealth())
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Owner = GetUnitOwner();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                AuraEffect* l_AuraEffect = GetEffect(EFFECT_0);
                if (!l_Owner || !l_DamageInfo || !l_AuraEffect)
                    return;

                int32 l_MaxAbsorb = l_Owner->CountPctFromMaxHealth((float)l_AuraEffect->GetAmount());
                int32 l_AbsorbAmount =  l_DamageInfo->GetAmount();

                if (AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::MentalFortitudeAbsorb, SpellEffIndex::EFFECT_0))
                {
                    int32 l_CurrentAbsorb = l_AuraEffect->GetAmount();
                    if (l_CurrentAbsorb >= l_MaxAbsorb &&
                        (PlayerScript_Mental_Fortitude::GetDamagesDoneInLastPeriod(l_Owner->GetGUID() < l_AbsorbAmount * 2)))
                        return;

                    l_AuraEffect->SetAmount(std::min((l_CurrentAbsorb + l_AbsorbAmount), l_MaxAbsorb));
                    l_AuraEffect->GetBase()->RefreshDuration();
                }
                else
                {
                    l_Owner->CastCustomSpell(eSpells::MentalFortitudeAbsorb, SpellValueMod::SPELLVALUE_BASE_POINT0, std::min(l_AbsorbAmount, l_MaxAbsorb), l_Owner, true);
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_mental_fortitude_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pri_mental_fortitude_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mental_fortitude_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Rebirth - 20484, Raise Ally - 61999, Eternal Guardian - 126393, Gift of Chi-Ji - 159931, Dust of Life - 159956, Soulstone - 3026
class spell_pri_surrendered_soul : public SpellScriptLoader
{
    public:
        spell_pri_surrendered_soul() : SpellScriptLoader("spell_pri_surrendered_soul") { }

        class spell_pri_surrendered_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_surrendered_soul_SpellScript);

            enum eSpells
            {
                SoulStone       = 3026,
                SurrenderedSoul = 212570
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster)
                {
                    if (l_Caster->ToPet())
                        l_Caster = l_Caster->ToPet()->GetOwner();

                    if (l_Caster && l_Caster->ToPlayer() && l_Caster->ToPlayer()->getClass() == CLASS_HUNTER)
                    {
                        return SpellCastResult::SPELL_CAST_OK;
                    }
                }

                Unit* l_Target = GetExplTargetUnit();

                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (l_SpellInfo && l_SpellInfo->Id == eSpells::SoulStone)
                    l_Target = GetCaster();

                if (!l_Target)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                if (l_Target->HasAura(eSpells::SurrenderedSoul))
                    return SpellCastResult::SPELL_FAILED_TARGET_CANNOT_BE_RESURRECTED;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_surrendered_soul_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_surrendered_soul_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Holy Word: Sanctify - 34861
/// Called by Holy Word: Serenity - 2050
/// Called by Holy Word: Chastise - 88625
/// Called for Al'maiesh, the Cord of Hope (item) - 132445
class spell_pri_almaiesh : public SpellScriptLoader
{
    public:
        spell_pri_almaiesh() : SpellScriptLoader("spell_pri_almaiesh") { }

        class spell_pri_almaiesh_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_almaiesh_SpellScript);

            enum  eSpells
            {
                HWChastise         = 88625,
                HWSanctify         = 34861,
                HWSerenity         = 2050,
                AlmaieshHWChastise = 211443,
                AlmaieshHWSanctify = 211442,
                AlmaieshHWSerenity = 211440
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::HWChastise:
                        if (l_Caster->HasAura(eSpells::AlmaieshHWChastise))
                            l_Caster->RemoveAura(eSpells::AlmaieshHWChastise);
                        break;
                    case eSpells::HWSanctify:
                        if (l_Caster->HasAura(eSpells::AlmaieshHWSanctify))
                            l_Caster->RemoveAura(eSpells::AlmaieshHWSanctify);
                        break;
                    case eSpells::HWSerenity:
                        if (l_Caster->HasAura(eSpells::AlmaieshHWSerenity))
                            l_Caster->RemoveAura(eSpells::AlmaieshHWSerenity);
                        break;
                    default:
                        break;
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_almaiesh_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_almaiesh_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Cord of Maiev, Priestess of the Moon - 214484
class spell_pri_cord_of_maiev_priestess_of_the_moon : public SpellScriptLoader
{
    public:
        spell_pri_cord_of_maiev_priestess_of_the_moon() : SpellScriptLoader("spell_pri_cord_of_maiev_priestess_of_the_moon") { }

        class spell_pri_cord_of_maiev_priestess_of_the_moon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_cord_of_maiev_priestess_of_the_moon_AuraScript);

            enum eSpells
            {
                Smite   = 585,
                Penance = 47540
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Smite)
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

                l_Player->RemoveSpellCooldown(eSpells::Penance, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_cord_of_maiev_priestess_of_the_moon_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pri_cord_of_maiev_priestess_of_the_moon_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_cord_of_maiev_priestess_of_the_moon_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Vampiric Embrace - 15290
/// Called for Zenk'aram, Iridi's Anadem (item) - 133971
class spell_pri_zenkaram_iridis_anadem : public SpellScriptLoader
{
    public:
        spell_pri_zenkaram_iridis_anadem() : SpellScriptLoader("spell_pri_zenkaram_iridis_anadem") { }

        class spell_pri_zenkaram_iridis_anadem_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_zenkaram_iridis_anadem_SpellScript);

            enum eSpells
            {
                IridisEmpowerment = 224999,
                VoidForm          = 194249
            };

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                int32 l_HealAmount = GetHitHeal();
                SpellInfo const* l_SpellInfoIridisEmpowerment = sSpellMgr->GetSpellInfo(eSpells::IridisEmpowerment);
                if (!l_Caster || !l_HealAmount || !l_SpellInfoIridisEmpowerment || !l_Caster->HasAura(eSpells::IridisEmpowerment) || !l_Caster->HasAura(eSpells::VoidForm))
                    return;

                l_HealAmount = CalculatePct(l_HealAmount, l_SpellInfoIridisEmpowerment->Effects[EFFECT_0].BasePoints);
                SetHitHeal(l_HealAmount);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_zenkaram_iridis_anadem_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_zenkaram_iridis_anadem_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Power Word: Barrier - 81782
/// Called for N'ero, Band of Promises (item) - 137276
class spell_pri_nero_band_of_promises_targets : public SpellScriptLoader
{
    public:
        spell_pri_nero_band_of_promises_targets() : SpellScriptLoader("spell_pri_nero_band_of_promises_targets") { }

        class spell_pri_nero_band_of_promises_targets_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_nero_band_of_promises_targets_AuraScript);

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::PWBarrierTargets].insert(l_Target->GetGUID());
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::PWBarrierTargets].erase(l_Target->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_nero_band_of_promises_targets_AuraScript::HandleAfterEffectApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_nero_band_of_promises_targets_AuraScript::HandleAfterEffectRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_nero_band_of_promises_targets_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Penance - 47666
/// Called for N'ero, Band of Promises (item) - 137276
class spell_pri_nero_band_of_promises_heal : public SpellScriptLoader
{
    public:
        spell_pri_nero_band_of_promises_heal() : SpellScriptLoader("spell_pri_nero_band_of_promises_heal") { }

        class spell_pri_nero_band_of_promises_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_nero_band_of_promises_heal_SpellScript);

            enum eSpells
            {
                PromisesOfNero = 225683
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                int32 l_Amount = GetHitDamage();
                SpellInfo const* l_SpellInfoPromisesOfNero = sSpellMgr->GetSpellInfo(eSpells::PromisesOfNero);
                if (!l_Caster || !l_Amount || !l_SpellInfoPromisesOfNero)
                    return;

                if (l_Caster->HasAura(eSpells::PromisesOfNero))
                {
                    std::set<uint64> l_TargetGUIDsPWBarrier = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::PWBarrierTargets];
                    std::set<uint64> l_TargetGUIDsAtonment = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                    bool l_HasAtonment;
                    for (auto l_Itr = l_TargetGUIDsPWBarrier.begin(); l_Itr != l_TargetGUIDsPWBarrier.end(); ++l_Itr)
                    {
                        Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, *l_Itr);

                        if (!l_Target)
                            continue;

                        l_HasAtonment = false;
                        for (auto l_Itr2 = l_TargetGUIDsAtonment.begin(); l_Itr2 != l_TargetGUIDsAtonment.end(); ++l_Itr2)
                        {
                            Unit* l_TargetAtonment = ObjectAccessor::GetUnit(*l_Caster, *l_Itr2);

                            if (!l_TargetAtonment)
                                continue;

                            if (l_Target == l_TargetAtonment)
                                l_HasAtonment = true;
                        }

                        if (!l_HasAtonment)
                            l_Target->HealBySpell(l_Target, l_SpellInfoPromisesOfNero, l_Amount, false);
                    }
                }
            }
            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_nero_band_of_promises_heal_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_nero_band_of_promises_heal_SpellScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Legendary : Dejahna's Inspiration - 214633
class spell_pri_dejahnas_inspiration : public SpellScriptLoader
{
public:
    spell_pri_dejahnas_inspiration() : SpellScriptLoader("spell_pri_dejahnas_inspiration") { }

    class spell_pri_dejahnas_inspiration_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_dejahnas_inspiration_AuraScript);

        enum eSpells
        {
            DejahnasInspirationHasteBonus = 214637,
            Plea = 200829
        };

        bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_ProcSpellInfo || l_ProcSpellInfo->Id != eSpells::Plea)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& /* p_EventInfo */)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::DejahnasInspirationHasteBonus);
            std::set<uint64> l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
            for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
            {
                Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, *l_Itr);

                if (!l_Target)
                    continue;

                l_Caster->CastSpell(l_Caster, eSpells::DejahnasInspirationHasteBonus);
            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_dejahnas_inspiration_AuraScript::HandleDoCheckProc);
            OnProc += AuraProcFn(spell_pri_dejahnas_inspiration_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_dejahnas_inspiration_AuraScript();
    }
};

/// Update to Legion 7.1.5 build 23420
/// Legendary : The Twins' Painful Touch - 207721
class spell_pri_the_twins_painful_touch : public SpellScriptLoader
{
    public:
        spell_pri_the_twins_painful_touch() : SpellScriptLoader("spell_pri_the_twins_painful_touch") { }

        class spell_pri_the_twins_painful_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_the_twins_painful_touch_AuraScript);

            enum eSpells
            {
                TheTwinsPainfulTouchSpread  = 207724,
                VoidEruption                = 228360
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

                if (l_ProcSpellInfo->Id == eSpells::VoidEruption)
                    return true;

                return false;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::TheTwinsPainfulTouchSpread, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_the_twins_painful_touch_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_pri_the_twins_painful_touch_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_the_twins_painful_touch_AuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Legendary : The Twins' Painful Touch - 207721
/// Called By Mind Flay 15407 and Mind Spike 73510
class spell_pri_the_twins_painful_touch_spread : public SpellScriptLoader
{
    public:
        spell_pri_the_twins_painful_touch_spread() : SpellScriptLoader("spell_pri_the_twins_painful_touch_spread") { }

        class spell_pri_the_twins_painful_touch_spread_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_the_twins_painful_touch_spread_SpellScript);

            enum eSpells
            {
                TheTwinsPainfulTouchSpread  = 207724,
                TheTwinsPainfulTouchRange   = 207721,
                ShadowWordPain              = 589,
                VampiricTouch               = 34914,
                VoidForm                    = 194249,
                VoidEruption                = 228260,
                PvPRulesEnabled             = 134735
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_HitTarget = GetExplTargetUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TheTwinsPainfulTouchRange);
                if (!l_Caster || !l_SpellInfo || !l_HitTarget || !l_Caster->HasAura(eSpells::TheTwinsPainfulTouchSpread))
                    return;

                float l_Radius = l_SpellInfo->Effects[EFFECT_2].BasePoints;
                uint8 l_MaxTarget = 0;
                std::list<Unit*> l_TwinsPainfulTouchEnemies;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(l_HitTarget, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(l_HitTarget, l_TwinsPainfulTouchEnemies, u_check);
                l_HitTarget->VisitNearbyObject(l_Radius, searcher);

                for (auto l_Target : l_TwinsPainfulTouchEnemies)
                {
                    if (l_Target != l_HitTarget && l_MaxTarget < l_SpellInfo->Effects[EFFECT_1].BasePoints && l_Target->isTargetableForAttack(false))
                    {
                        if (l_Target->IsPlayer() && (l_Target->IsFriendlyTo(l_Caster) || !l_Target->HasAura(eSpells::PvPRulesEnabled)))
                            continue;

                        if (l_HitTarget->HasAura(eSpells::ShadowWordPain, l_Caster->GetGUID()))
                            l_Caster->CastSpell(l_Target, eSpells::ShadowWordPain, true);
                        if (l_HitTarget->HasAura(eSpells::VampiricTouch, l_Caster->GetGUID()))
                            l_Caster->CastSpell(l_Target, eSpells::VampiricTouch, true);
                    }
                    l_MaxTarget++;
                }
                l_Caster->RemoveAurasDueToSpell(eSpells::TheTwinsPainfulTouchSpread);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_the_twins_painful_touch_spread_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_the_twins_painful_touch_spread_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Void Shift - 108968
class spell_pri_void_shift : public SpellScriptLoader
{
    public:
        spell_pri_void_shift() : SpellScriptLoader("spell_pri_void_shift") { }

        class spell_pri_void_shift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_shift_SpellScript);

            enum eSpells
            {
                VoidShiftEffect = 118594
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target || !l_Target->IsPlayer())
                    return SpellCastResult::SPELL_FAILED_TARGET_NOT_PLAYER;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (!l_Caster || !l_Target)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                int32 l_MinPct = l_SpellInfo->Effects[0].BasePoints;
                int32 l_CasterHealthPct = static_cast<int32>(l_Caster->GetHealthPct());
                int32 l_TargetHealthPct = static_cast<int32>(l_Target->GetHealthPct());

                if (l_CasterHealthPct < l_MinPct || l_TargetHealthPct < l_MinPct)
                {
                    if (l_CasterHealthPct < l_TargetHealthPct)
                        l_CasterHealthPct = l_MinPct;
                    else
                        l_TargetHealthPct = l_MinPct;
                }

                CastSwap(l_Caster, l_Target, static_cast<int32>(l_Target->GetHealthPct()), l_CasterHealthPct);
                CastSwap(l_Caster, l_Caster, static_cast<int32>(l_Caster->GetHealthPct()), l_TargetHealthPct);
            }

            void CastSwap(Unit* p_Caster, Unit* p_Target, int32 p_CurrentPct, int32 p_DestPct)
            {
                int32 l_Damage = 0;
                int32 l_Heal = 0;

                if (p_CurrentPct < p_DestPct)
                    l_Heal = CalculatePct(p_Target->GetMaxHealth(), p_DestPct - p_CurrentPct);
                else
                    l_Damage = CalculatePct(p_Target->GetMaxHealth(), p_CurrentPct - p_DestPct);

                p_Caster->CastCustomSpell(p_Target, eSpells::VoidShiftEffect, &l_Damage, &l_Heal, nullptr, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_void_shift_SpellScript::HandleCheckCast);
                OnCast += SpellCastFn(spell_pri_void_shift_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_shift_SpellScript();
        }
};

/// Last Update 7.1.5
/// Called by Void Shift - 118594
class spell_pri_void_shift_effect : public SpellScriptLoader
{
    public:
        spell_pri_void_shift_effect() : SpellScriptLoader("spell_pri_void_shift_effect") { }

        class spell_pri_void_shift_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_shift_effect_SpellScript);

            void HandleOnPrepare()
            {
                Spell *l_Spell = GetSpell();

                if (l_Spell)
                    l_Spell->SetCustomCritChance(0);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_pri_void_shift_effect_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_shift_effect_SpellScript();
        }
};

/// Last Update 7.1.5
/// Shadow Mania - 199572
class spell_pri_shadow_mania : public SpellScriptLoader
{
    public:
        spell_pri_shadow_mania() : SpellScriptLoader("spell_pri_shadow_mania") { }

        class spell_pri_shadow_mania_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadow_mania_AuraScript);

            enum eSpells
            {
                ShadowManiaProc = 199579
            };

            void HandlePeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster)
                {
                    if (l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::VampiricTouch].size() >= 2)
                    {
                        if (!l_Caster->HasAura(eSpells::ShadowManiaProc))
                            l_Caster->CastSpell(l_Caster, eSpells::ShadowManiaProc, true);
                    }
                    else
                        l_Caster->RemoveAurasDueToSpell(eSpells::ShadowManiaProc);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_shadow_mania_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_shadow_mania_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by Light's Wrath - 207946
class spell_pri_lights_wrath : public SpellScriptLoader
{
    public:
        spell_pri_lights_wrath() : SpellScriptLoader("spell_pri_lights_wrath") { }

        class spell_pri_lights_wrath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_lights_wrath_SpellScript);

            enum eSpells
            {
                KamXiraffAura   = 233978,
                KamXiraffBuff   = 233997
            };

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                int32 l_Damage = GetHitDamage();
                int32 l_NbAtonementTargets = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets].size();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                AddPct(l_Damage, l_NbAtonementTargets * l_SpellInfo->Effects[EFFECT_1].BasePoints);
                SetHitDamage(l_Damage);

                if (AuraEffect* l_KamXiraffEffect = l_Caster->GetAuraEffect(eSpells::KamXiraffAura, EFFECT_0))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::KamXiraffBuff, true);
                    if (Aura* l_KamXiraffBuff = l_Caster->GetAura(eSpells::KamXiraffBuff))
                    {
                        int32 l_Duration = l_KamXiraffBuff->GetDuration();

                        l_Duration += (l_KamXiraffEffect->GetBaseAmount() * l_NbAtonementTargets * TimeConstants::IN_MILLISECONDS);
                        l_KamXiraffBuff->SetDuration(l_Duration);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_lights_wrath_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_lights_wrath_SpellScript();
        }
};

/// Update to Legion 7.3.5 build 26365
/// Call by Call to the Void - 193371
class spell_pri_call_to_the_void : public SpellScriptLoader
{
    public:
        spell_pri_call_to_the_void() : SpellScriptLoader("spell_pri_call_to_the_void") { }

        class spell_pri_call_to_the_void_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_call_to_the_void_AuraScript);

            enum eSpells
            {
                MindSpike       = 73510,
                MindFlay        = 15407,
                SummonTentacle  = 193470
            };

            enum eNPCs
            {
                Tentacle = 98167
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

                if ((l_ProcSpellInfo->Id == eSpells::MindSpike || l_ProcSpellInfo->Id == eSpells::MindFlay))
                    return true;

                return false;
            }

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                if (!l_Caster->ToPlayer())
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target || !l_Target->isInCombat() || !l_Target->isAlive())
                    l_Target = l_Caster->getVictim();

                if (!l_Target || !l_Target->isInCombat() || !l_Target->isAlive())
                    if (l_Caster->ToPlayer()->GetSelectedUnit() && l_Caster->IsValidAttackTarget(l_Caster->ToPlayer()->GetSelectedUnit()))
                        l_Target = l_Caster->ToPlayer()->GetSelectedUnit();

                if (!l_Target || !l_Target->isInCombat() || !l_Target->isAlive())
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SummonTentacle, true);
                for (auto l_Summoned : l_Caster->m_Controlled)
                {
                    if (l_Summoned && l_Summoned->GetEntry() == eNPCs::Tentacle)
                        l_Summoned->SetTarget(l_Target->GetGUID());
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_call_to_the_void_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_pri_call_to_the_void_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_call_to_the_void_AuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Call by Unleash the Shadows - 194093
class spell_pri_unleash_the_shadows : public SpellScriptLoader
{
    public:
        spell_pri_unleash_the_shadows() : SpellScriptLoader("spell_pri_unleash_the_shadows") { }

        class spell_pri_unleash_the_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_unleash_the_shadows_AuraScript);

            enum eSpells
            {
                ShadowyApparition       = 147193,
                ShadowyApparitionVisual = 33584,
                VampiricTouch           = 34914
            };

            enum eArtifactPowerId
            {
                UnleashTheShadows = 767
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return false;

                SpellInfo const* l_VampirictouchInfo = l_DamageInfo->GetSpellInfo();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_VampirictouchInfo || !((p_EventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT) && l_VampirictouchInfo->Id == eSpells::VampiricTouch))
                    return false;

                if (!roll_chance_i(GetSpellInfo()->Effects[EFFECT_0].BasePoints * l_Player->GetRankOfArtifactPowerId(eArtifactPowerId::UnleashTheShadows)))
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

                l_Caster->SendPlaySpellVisual(eSpells::ShadowyApparitionVisual, l_ProcTarget, 6.0f, 0.0f, l_Caster->GetPosition());
                l_Caster->CastSpell(l_ProcTarget, eSpells::ShadowyApparition, true);

            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_unleash_the_shadows_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_pri_unleash_the_shadows_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_unleash_the_shadows_AuraScript();
        }
};

/// Last Update 7.1.5 - 23420
/// Called by Priest - 137030
class spell_pri_priest_aura : public SpellScriptLoader
{
    public:
        spell_pri_priest_aura() : SpellScriptLoader("spell_pri_priest_aura") { }

        class spell_pri_priest_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_priest_aura_AuraScript);

            void HandleCalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                p_Amount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_priest_aura_AuraScript::HandleCalculateAmount, EFFECT_2, SPELL_AURA_MODIFY_MANA_REGEN_FROM_MANA_PCT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_priest_aura_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Renew - 139
class spell_pri_renew : public SpellScriptLoader
{
public:
    spell_pri_renew() : SpellScriptLoader("spell_pri_renew") { }

    class spell_pri_renew_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_renew_AuraScript);

        enum eSpells
        {
            Perseverance = 235189
        };

        void HandleAfterEffectApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /* p_Mode */)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();
            if (!l_Caster || !l_Target)
                return;

            if (!l_Caster->HasAura(eSpells::Perseverance) || l_Caster != l_Target)
                const_cast<AuraEffect*>(p_AurEff)->SetAmount(0);
        }

        void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                return;

            p_Amount *= 1.75f;   ///< Renew's HoT should benefit from the 175% PvP modifier
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_renew_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            AfterEffectApply += AuraEffectApplyFn(spell_pri_renew_AuraScript::HandleAfterEffectApply, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_renew_AuraScript();
    }
};

/// Last Update 7.1.5 - 23420
/// 200153 Enduring Renewal
/// Called by Flash Heal 2061, Holy Word: Serenity 2050, Heal 2060, Light of T'uure 208065, Binding Heal - 32546 
class spell_pri_enduring_renewal : public SpellScriptLoader
{
    public:
        spell_pri_enduring_renewal() : SpellScriptLoader("spell_pri_enduring_renewal") { }

        class spell_pri_enduring_renewal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_enduring_renewal_SpellScript);

            enum eSpells
            {
                EnduringRenewal = 200153,
                Renew = 139
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::EnduringRenewal))
                    if (Aura* l_AurRenew = l_Target->GetAura(eSpells::Renew, l_Caster->GetGUID()))
                        l_AurRenew->RefreshDuration();
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::EnduringRenewal))
                    if (Aura* l_AurRenew = l_Target->GetAura(eSpells::Renew, l_Caster->GetGUID()))
                        l_AurRenew->RefreshDuration();
            }

            void Register() override
            {
                if (m_scriptSpellId == 32546)
                    AfterHit += SpellHitFn(spell_pri_enduring_renewal_SpellScript::HandleAfterHit);
                else
                    AfterCast += SpellCastFn(spell_pri_enduring_renewal_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_enduring_renewal_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Shadow Covenant - 204065
class spell_pri_shadow_covenant : public SpellScriptLoader
{
    public:
        spell_pri_shadow_covenant() : SpellScriptLoader("spell_pri_shadow_covenant") { }

        class spell_pri_shadow_covenant_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_covenant_SpellScript);

            enum eSpells
            {
                ShadowCovenantShieldAmount  = 219518,
                ShadowCovenantShield        = 219521
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                Unit* l_MainTarget = GetExplTargetUnit();
                if (!l_Caster || !l_SpellInfo || !l_MainTarget)
                    return;

                std::list<WorldObject*>::iterator l_Itr = std::find(p_Targets.begin(), p_Targets.end(), (WorldObject*)l_MainTarget);
                if (l_Itr != p_Targets.end())
                    p_Targets.erase(l_Itr);

                uint32 l_TargetAmount = l_SpellInfo->Effects[EFFECT_2].BasePoints - 1;

                p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                p_Targets.resize(l_TargetAmount);
                if (l_MainTarget->IsFriendlyTo(l_Caster))
                    p_Targets.push_back(l_MainTarget);
            }

            void HandleOnHit(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Caster->ToPlayer())
                    return;

                if (p_EffIndex != EFFECT_1)
                    return;

                uint32 l_AbsorbAmount = l_Caster->ToPlayer()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW) * 2.5f;
                uint64 l_UnitGuid = l_Target->GetGUID();
                l_Caster->AddDelayedEvent
                (
                    [l_AbsorbAmount, l_UnitGuid, l_Caster]() -> void
                    {
                        if (Unit* l_Target = Unit::GetUnit(*l_Caster, l_UnitGuid))
                            l_Caster->CastCustomSpell(eSpells::ShadowCovenantShield, SpellValueMod::SPELLVALUE_BASE_POINT0, l_AbsorbAmount, l_Target, true);
                    }
                    , 200
                );
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_shadow_covenant_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_pri_shadow_covenant_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_shadow_covenant_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Shadow Covenant - 219521
class spell_pri_shadow_covenant_shield : public SpellScriptLoader
{
    public:
        spell_pri_shadow_covenant_shield() : SpellScriptLoader("spell_pri_shadow_covenant_shield") { }

        class spell_pri_shadow_covenant_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadow_covenant_shield_AuraScript);

            void HandleOnEffectCalcAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->ToPlayer())
                    return;

                int32 l_Amount = l_Caster->ToPlayer()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW) * 2.5f;

                p_Amount = l_Amount;
            }

            void OnUpdate(uint32 /*p_Diff*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                {
                    if (l_Effect->GetAmount() > (l_Caster->ToPlayer()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW) * 2.5f))
                        l_Effect->SetAmount(l_Caster->ToPlayer()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW) * 2.5f);
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_pri_shadow_covenant_shield_AuraScript::OnUpdate);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_shadow_covenant_shield_AuraScript::HandleOnEffectCalcAmount, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_shadow_covenant_shield_AuraScript();
        }
};

/// Last Update 7.1.5 23420
/// Called for Purge the Wicked - 204197
/// Called by Penance - 47540
class spell_pri_purge_the_wicked : public SpellScriptLoader
{
    public:
        spell_pri_purge_the_wicked() : SpellScriptLoader("spell_pri_purge_the_wicked") { }

        class spell_pri_purge_the_wicked_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_purge_the_wicked_SpellScript);

            enum eSpells
            {
                PurgeTheWickedPeriodic  = 204213,
                PurgeTheWickedDummy     = 204215
            };

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::PurgeTheWickedPeriodic, l_Caster->GetGUID()))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PurgeTheWickedDummy, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_purge_the_wicked_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_purge_the_wicked_SpellScript;
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Purge the Wicked - 204215
class spell_pri_purge_the_wicked_dummy : public SpellScriptLoader
{
    public:
        spell_pri_purge_the_wicked_dummy() : SpellScriptLoader("spell_pri_purge_the_wicked_dummy") { }

        class spell_pri_purge_the_wicked_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_purge_the_wicked_dummy_SpellScript);

            enum eSpells
            {
                PurgeTheWickedPeriodic = 204213
            };

            void FilterTargets(std::list<WorldObject*> &l_pTargets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_pTargets.remove_if([&](WorldObject* p_Object) -> bool
                {
                    if (!p_Object)
                        return true;

                    Unit* l_UnitObject = p_Object->ToUnit();
                    if (!l_UnitObject)
                        return true;

                    if (l_UnitObject == GetExplTargetUnit())
                        return true;

                    if (l_UnitObject->HasBreakableByDamageCrowdControlAura())
                        return true;

                    return false;
                });

                if (l_pTargets.empty())
                    return;

                std::list<Unit*> l_pTargetsWithoutAura;
                std::list<Aura*> l_pAuras;
                std::for_each(l_pTargets.begin(), l_pTargets.end(), [&](WorldObject* p_Object)
                {
                    if (Unit* l_pTarget = p_Object->ToUnit())
                    {
                        if (Aura* l_pAura = l_pTarget->GetAura(eSpells::PurgeTheWickedPeriodic, l_Caster->GetGUID()))
                        {
                            l_pAuras.push_back(l_pAura);
                        }
                        else
                        {
                            l_pTargetsWithoutAura.push_back(l_pTarget);
                        }
                    }
                });

                l_pTargets.clear();

                if (!l_pTargetsWithoutAura.empty())
                {
                    l_pTargets.push_back(JadeCore::Containers::SelectRandomContainerElement(l_pTargetsWithoutAura));
                }
                else if (!l_pAuras.empty())
                {
                    l_pAuras.sort([](Aura const* l_pAura1, Aura const* l_pAura2) -> bool
                    {
                        return l_pAura1->GetDuration() < l_pAura2->GetDuration();
                    });

                    if (Unit* l_pTarget = l_pAuras.front()->GetUnitOwner())
                    {
                        l_pTargets.push_back(l_pTarget);
                    }
                }

                for (auto l_Target : l_pTargets)
                    if (l_Target->ToUnit() && l_Target->ToUnit()->HasStealthAura())
                        l_Target->ToUnit()->RemoveAurasByType(SPELL_AURA_MOD_STEALTH, 0, NULL, 131361);

                l_pTargets.remove_if([&](WorldObject* p_Object) -> bool
                {
                    if (!p_Object)
                        return true;

                    Unit* l_UnitObject = p_Object->ToUnit();
                    if (!l_UnitObject)
                        return true;

                    if (!l_UnitObject->isInCombat())
                        return true;

                    return false;
                });
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PurgeTheWickedPeriodic, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_purge_the_wicked_dummy_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_pri_purge_the_wicked_dummy_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_purge_the_wicked_dummy_SpellScript;
        }
};

/// Last Update : 7.3.2 Build 25549
/// Called by Void Torrent - 205065
class spell_pri_void_torrent : public SpellScriptLoader
{
    public:
        spell_pri_void_torrent() : SpellScriptLoader("spell_pri_void_torrent") { }

        class spell_pri_void_torrent_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_void_torrent_AuraScript);

            void CalcPeriodic(AuraEffect const* /*P_AurEff*/, bool& /*p_IsPeriodic*/, int32& p_Amplitude)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Amplitude = int32(p_Amplitude * std::max<float>(l_Caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED), 0.5f));
            }

            void Register() override
            {
                DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_pri_void_torrent_AuraScript::CalcPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        class spell_pri_void_torrent_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_torrent_SpellScript);

            enum eSpells
            {
                MindQuickening = 238101,
                MindQuickeningSpell = 240673
            };

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    if (l_Caster->HasAura(eSpells::MindQuickening))
                        l_Caster->CastSpell(l_Caster, eSpells::MindQuickeningSpell, true);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pri_void_torrent_SpellScript::HandleAfterCast);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_void_torrent_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_torrent_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mind Bomb - 205369
class spell_pri_mind_bomb : public SpellScriptLoader
{
    public:
        spell_pri_mind_bomb() : SpellScriptLoader("spell_pri_mind_bomb") { }

        class spell_pri_mind_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_bomb_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;
                ///< Before reflect
                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::MindBomb) = l_Target->GetGUID();
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_pri_mind_bomb_SpellScript::HandleBeforeCast);
            }
        };

        class spell_pri_mind_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mind_bomb_AuraScript);

            enum eSpells
            {
                MindBombStun = 226943
            };

            bool l_Stun = false;

            void HandleOnDispell(DispelInfo* /*p_DispelInfo*/)
            {
                l_Stun = true;
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Caster || l_Stun)
                    return;

                if (l_Caster != l_Target)
                    l_Caster->CastSpell(l_Target, eSpells::MindBombStun, true);
                else ///< fixed then reflect aura 205369
                {
                    if (Unit* l_Victim = sObjectAccessor->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::MindBomb)))
                        l_Victim->CastSpell(l_Caster, eSpells::MindBombStun, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_mind_bomb_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnDispel += AuraDispelFn(spell_pri_mind_bomb_AuraScript::HandleOnDispell);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mind_bomb_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mind_bomb_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Last Word - 215776
class spell_pri_last_word : public SpellScriptLoader
{
    public:
        spell_pri_last_word() : SpellScriptLoader("spell_pri_last_word") { }

        class spell_pri_last_word_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_last_word_AuraScript);

            enum eSpells
            {
                LastWordSilence = 199683
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::LastWordSilence, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_last_word_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_last_word_AuraScript();
        }
};

/// Last Update 7.1.5
/// Mania - 193173
class spell_pri_mania : public SpellScriptLoader
{
    public:
        spell_pri_mania() : SpellScriptLoader("spell_pri_mania") { }

        class spell_pri_mania_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mania_AuraScript);

            enum eSpells
            {
                ManiaSpeed = 195290
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Aura const* l_ManiaSpeedAura = l_Owner->AddAura(eSpells::ManiaSpeed, l_Owner))
                {
                    int32 l_insanityPerSpeed = p_AurEff->GetAmount();
                    int32 l_speedValue = (l_Owner->GetPower(POWER_INSANITY) / 100) / (l_insanityPerSpeed > 0 ? l_insanityPerSpeed : 3);
                    l_ManiaSpeedAura->GetEffect(EFFECT_0)->ChangeAmount(l_speedValue);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                l_Owner->RemoveAura(eSpells::ManiaSpeed);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_mania_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_mania_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_mania_AuraScript();
        }
};

/// Update 7.1.5 Build 23420
/// Mind Control - 205364
class spell_pri_mind_control : public SpellScriptLoader
{
    public:
        spell_pri_mind_control() : SpellScriptLoader("spell_pri_mind_control") { }

        class spell_pri_mind_control_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_control_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Target = GetExplTargetUnit())
                {
                    if (l_Target->IsPlayer())
                        return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_mind_control_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mind_control_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Mania - 193173
class PlayerScript_pri_mania : public PlayerScript
{
    public:
        PlayerScript_pri_mania() : PlayerScript("PlayerScript_pri_mania") {}

        enum eSpells
        {
            Mania = 193173,
            ManiaSpeed = 195290
        };

        void OnModifyPower(Player* p_Player, Powers p_Power, int32 p_OldValue, int32& p_NewValue, bool p_Regen, bool p_After) override
        {
            if (p_After)
                return;

            if (p_Player->getClass() != CLASS_PRIEST || p_Power != POWER_INSANITY)
                return;

            Aura const* l_ManiaAura = p_Player->GetAura(eSpells::Mania);
            if (!l_ManiaAura)
                return;

            if (AuraEffect* l_ManiaSpeedEffect = p_Player->GetAuraEffect(eSpells::ManiaSpeed, EFFECT_0))
            {
                int32 l_insanityPerSpeed = l_ManiaAura->GetEffect(EFFECT_0)->GetAmount();
                int32 l_speedValue = (p_NewValue / 100) / (l_insanityPerSpeed > 0 ? l_insanityPerSpeed : 3);
                if (l_ManiaSpeedEffect->GetAmount() != l_speedValue)
                    l_ManiaSpeedEffect->ChangeAmount(l_speedValue);
            }
        }
};

/// Called by Desperate Prayer - 19236
class spell_pri_desperate_prayer : public SpellScriptLoader
{
    public:
        spell_pri_desperate_prayer() : SpellScriptLoader("spell_pri_desperate_prayer") { }

        class spell_pri_desperate_prayer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_desperate_prayer_AuraScript);

            void HandleOnTick(AuraEffect const* p_AuraEffect)
            {
                AuraEffect* l_AuraEffect = GetEffect(EFFECT_0);
                if (!l_AuraEffect || !p_AuraEffect)
                    return;

                l_AuraEffect->ChangeAmount(l_AuraEffect->GetAmount() - p_AuraEffect->GetAmount());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_desperate_prayer_AuraScript::HandleOnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_desperate_prayer_AuraScript();
        }
};

/// Last Update 7.1.5 23420
/// Trail of Light - 200128
class spell_pri_trail_of_light : public SpellScriptLoader
{
    public:
        spell_pri_trail_of_light() : SpellScriptLoader("spell_pri_trail_of_light") { }

        class spell_pri_trail_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_trail_of_light_AuraScript);

            enum eSpells
            {
                TrailOfLightHeal    = 234946
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                if (!p_EventInfo.GetDamageInfo() || !p_EventInfo.GetActionTarget())
                    return;

                auto l_NewTargetGuid = p_EventInfo.GetActionTarget()->GetGUID();

                if (m_PrevTargetGuid && !m_CurrentTargetGuid && m_PrevTargetGuid == l_NewTargetGuid)
                    return;

                if (m_PrevTargetGuid == 0)
                {
                    m_PrevTargetGuid = l_NewTargetGuid;
                }
                else if (m_CurrentTargetGuid == 0)
                {
                    m_CurrentTargetGuid = l_NewTargetGuid;
                }
                else if (m_CurrentTargetGuid != l_NewTargetGuid)
                {
                    m_PrevTargetGuid = m_CurrentTargetGuid;
                    m_CurrentTargetGuid = l_NewTargetGuid;
                }

                if (m_PrevTargetGuid && m_CurrentTargetGuid && m_PrevTargetGuid != m_CurrentTargetGuid)
                {
                    Unit* l_Target = Unit::GetUnit(*l_Caster, m_PrevTargetGuid);
                    if (!l_Target || !l_Caster->IsWithinDistInMap(l_Target, 100.0f))
                        return;

                    uint32 l_HealAmount = CalculatePct(p_EventInfo.GetDamageInfo()->GetAmount(), p_AurEff->GetAmount());
                    l_Caster->CastCustomSpell(eSpells::TrailOfLightHeal, SpellValueMod::SPELLVALUE_BASE_POINT0, l_HealAmount, l_Target, true);
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_trail_of_light_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }

        private:

            uint64 m_CurrentTargetGuid = 0;
            uint64 m_PrevTargetGuid = 0;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_trail_of_light_AuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Censure 214579
class PlayerScript_pri_censure : public PlayerScript
{
    public:
        PlayerScript_pri_censure() : PlayerScript("PlayerScript_Censure") {}

        enum eSpells
        {
            Censure         = 200199,
            LightOfNaaru    = 196985,
            SymbolOfHope    = 64901,
        };

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID)
        {
            if (p_Player->GetActiveSpecializationID() != SPEC_PRIEST_HOLY)
                return;

            switch (p_SpellID)
            {
                case eSpells::Censure:
                    p_Player->CastSpell(p_Player, eSpells::Censure, true);
                    break;
                case eSpells::LightOfNaaru:
                case eSpells::SymbolOfHope:
                    p_Player->RemoveAura(eSpells::Censure);
                    break;
                default:
                    break;
            }
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Renew the Faith - 196492
class spell_pri_renew_the_faith_proc : public SpellScriptLoader
{
    public:
        spell_pri_renew_the_faith_proc() : SpellScriptLoader("spell_pri_renew_the_faith_proc") { }

        enum eSpells
        {
            DivineHymn          = 64844,
            RenewTheFaith       = 210994,
            PrayerOfMendingAura = 41635
        };

        class spell_pri_renew_the_faith_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_renew_the_faith_proc_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || l_Caster->HasAura(eSpells::RenewTheFaith) || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::DivineHymn || !l_Player)
                    return false;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Player, l_Player->GetPrayerOfMendingTarget());
                if (!l_Target || !l_Target->HasAura(eSpells::PrayerOfMendingAura))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_renew_the_faith_proc_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_renew_the_faith_proc_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Renew the Faith - 210994
class spell_pri_renew_the_faith_periodic : public SpellScriptLoader
{
    public:
        spell_pri_renew_the_faith_periodic() : SpellScriptLoader("spell_pri_renew_the_faith_periodic") { }

        class spell_pri_renew_the_faith_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_renew_the_faith_periodic_AuraScript);

            void HandlePeriodic(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmplitude(l_Caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED) * l_SpellInfo->Effects[EFFECT_0].Amplitude);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_pri_renew_the_faith_periodic_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_renew_the_faith_periodic_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Renew the Faith - 196529
class spell_pri_renew_the_faith : public SpellScriptLoader
{
    public:
        spell_pri_renew_the_faith() : SpellScriptLoader("spell_pri_renew_the_faith") { }

        class spell_pri_renew_the_faith_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_renew_the_faith_SpellScript);

            enum eSpells
            {
                PrayerOfMendingVisualBounce = 225275,
                PrayerOfMendingAura         = 41635,
                PrayerOfMendingHeal         = 33110,
                PrayerOfMendingRange        = 155793,
                DivineHymn                  = 64843,
            };

            void HandleAfterCast()
            {
                Player* l_Player = (GetCaster()) ? GetCaster()->ToPlayer() : nullptr;
                int32 l_Range = 0;
                if (!l_Player || !l_Player->HasAura(eSpells::DivineHymn))
                    return;

                int32 l_Bp = 1 + CalculatePct(l_Player->GetStat(STAT_INTELLECT), 175.0f);
                Unit* l_Target = sObjectAccessor->GetUnit(*l_Player, l_Player->GetPrayerOfMendingTarget());
                if (!l_Target)
                    return;

                if (SpellInfo const* l_RangeSpellInfo = sSpellMgr->GetSpellInfo(eSpells::PrayerOfMendingRange))
                    l_Range = (l_RangeSpellInfo->Effects[EFFECT_0].RadiusEntry) ? l_RangeSpellInfo->Effects[EFFECT_0].RadiusEntry->RadiusMax : 0;

                std::list<Unit*> l_AlliesList;
                l_Player->GetRaidMembers(l_AlliesList);
                l_AlliesList.remove_if([l_Target, l_Range](Unit* p_Unit) -> bool
                {
                    if (l_Target == p_Unit || !p_Unit->IsWithinDistInMap(l_Target, l_Range))
                        return true;

                    return false;
                });

                if (l_AlliesList.empty())
                    return;

                int32 l_StackAmount = l_Target->GetAuraCount(eSpells::PrayerOfMendingAura);

                l_Player->SetPrayerOfMendingTarget(0);
                l_Player->CastCustomSpell(l_Target, eSpells::PrayerOfMendingHeal, &l_Bp, nullptr, nullptr, true);
                l_Target->RemoveAurasDueToSpell(eSpells::PrayerOfMendingAura, l_Player->GetGUID());

                Unit* l_NewTarget = l_AlliesList.front();
                if (!l_NewTarget)
                    return;

                l_Target->CastSpell(l_NewTarget, eSpells::PrayerOfMendingVisualBounce, true);
                l_Player->CastSpell(l_NewTarget, eSpells::PrayerOfMendingAura, true);

                Aura* l_NewAura = l_NewTarget->GetAura(eSpells::PrayerOfMendingAura, l_Player->GetGUID());
                if (!l_NewAura)
                    return;

                l_NewAura->SetStackAmount(l_StackAmount);
                l_Player->SetPrayerOfMendingTarget(l_NewTarget->GetGUID());
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pri_renew_the_faith_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_renew_the_faith_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Called by Premonition - 209780
class spell_pri_premonition : public SpellScriptLoader
{
    public:
        spell_pri_premonition() : SpellScriptLoader("spell_pri_premonition") { }

        enum eSpells
        {
            Atonement = 194384,
            PremonitionDmg = 209885
        };

        class spell_pri_premonition_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_premonition_SpellScript);

            void HandleOnCast()
            {
                Player* l_Caster = (GetCaster()) ? GetCaster()->ToPlayer() : nullptr;
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Atonement, true);

                std::set<uint64> l_AtonementTargetsList = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                for (uint64 l_TargetGUID : l_AtonementTargetsList)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGUID);

                    if (!l_Target)
                        continue;

                    if (Aura* l_Atonement = l_Target->GetAura(eSpells::Atonement, l_Caster->GetGUID()))
                        l_Atonement->SetDuration(l_Atonement->GetDuration() + l_SpellInfo->Effects[0].BasePoints);
                }

                int32 l_Damage = static_cast<int32>(l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL) * 2.5f);


                l_Damage *= l_AtonementTargetsList.size();
                l_Caster->CastCustomSpell(l_Caster, eSpells::PremonitionDmg, nullptr, &l_Damage, nullptr, true, nullptr, nullptr, 0, 2 * IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_premonition_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_premonition_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Called by Power of the Naaru - 196489
class spell_pri_power_of_the_naaru : public SpellScriptLoader
{
    public:
        spell_pri_power_of_the_naaru() : SpellScriptLoader("spell_pri_power_of_the_naaru") { }


        class spell_pri_power_of_the_naaru_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_power_of_the_naaru_AuraScript);

            enum eSpells
            {
                PowerOfTheNaaru     = 196489,
                PowerOfTheNaaruProc = 196490,
                HolyWordSanctify    = 34861
            };

            enum eArtifactPowers
            {
                PowerOfTheNaaruArtifact = 838
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::HolyWordSanctify)
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

                SpellInfo const* l_NaaruInfo = sSpellMgr->GetSpellInfo(eSpells::PowerOfTheNaaru);
                if (!l_NaaruInfo)
                    return;

                int32 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::PowerOfTheNaaruArtifact);
                int32 l_Value = l_Rank * l_NaaruInfo->Effects[EFFECT_0].BasePoints;

                l_Caster->CastCustomSpell(l_Caster, eSpells::PowerOfTheNaaruProc , &l_Value, nullptr, nullptr, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_power_of_the_naaru_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pri_power_of_the_naaru_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_power_of_the_naaru_AuraScript();
        }
};

/// 7.1.5 - Build 23420
/// Called by Driven to Madness - 199259
class spell_pri_driven_to_madness : public SpellScriptLoader
{
public:
    spell_pri_driven_to_madness() : SpellScriptLoader("spell_pri_driven_to_madness") { }

    class spell_pri_driven_to_madness_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_driven_to_madness_AuraScript);

        enum eSpells
        {
            Voidform = 194249
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || l_Caster->HasAura(eSpells::Voidform))
                return false;

            return true;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_driven_to_madness_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_driven_to_madness_AuraScript();
    }
};

/// Last Update : 7.1.5 Build 23420
/// Called by Ray of Hope - 197268
class spell_pri_ray_of_hope : public SpellScriptLoader
{
    public:
        spell_pri_ray_of_hope() : SpellScriptLoader("spell_pri_ray_of_hope") { }

        class spell_pri_ray_of_hope_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_ray_of_hope_AuraScript);

            enum eSpells
            {
                RayOfHopeHeal   = 197336,
                RayOfhopeDmg    = 197341
            };

            void HandleAfterRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                AuraEffect* l_DmgEffect = GetEffect(EFFECT_2);
                AuraEffect* l_HealEffect = GetEffect(EFFECT_3);
                AuraEffect* l_HealPctEffect = GetEffect(EFFECT_4);
                Unit* l_Owner = GetUnitOwner();

                if (!l_DmgEffect || !l_HealEffect || !l_HealPctEffect || !l_Owner)
                    return;

                int32 l_Dmg = l_DmgEffect->GetAmount();
                int32 l_Heal = l_HealEffect->GetAmount();

                l_Heal = AddPct(l_Heal, l_HealPctEffect->GetAmount());
                l_Owner->CastCustomSpell(l_Owner, eSpells::RayOfhopeDmg, &l_Dmg, nullptr, nullptr, true);
                l_Owner->CastCustomSpell(l_Owner, eSpells::RayOfHopeHeal, &l_Heal, nullptr, nullptr, true);
            }

            void HandleDamageAbsorb(AuraEffect* l_AuraEffect, DamageInfo& p_DmgInfo, uint32& l_Absorb)
            {
                AuraEffect* l_DmgEffect = GetEffect(EFFECT_2);

                if (!l_DmgEffect)
                    return;

                l_AuraEffect->SetAmount(p_DmgInfo.GetAmount() + 1);
                l_Absorb = p_DmgInfo.GetAmount();
                l_DmgEffect->SetAmount(l_DmgEffect->GetAmount() + p_DmgInfo.GetAmount());
            }

            void HandleHealAbsorb(AuraEffect* l_AuraEffect, DamageInfo& p_DmgInfo, uint32& l_Absorb)
            {
                AuraEffect* l_HealEffect = GetEffect(EFFECT_3);

                if (!l_HealEffect)
                    return;

                l_AuraEffect->SetAmount(p_DmgInfo.GetAmount() + 1);
                l_Absorb = p_DmgInfo.GetAmount();
                l_HealEffect->SetAmount(l_HealEffect->GetAmount() + p_DmgInfo.GetAmount());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_ray_of_hope_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_ray_of_hope_AuraScript::HandleDamageAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_ray_of_hope_AuraScript::HandleHealAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_ray_of_hope_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Void Bolt - 205448, Mind Blast - 8092
class spell_pri_psychic_link : public SpellScriptLoader
{
    public:
        spell_pri_psychic_link() : SpellScriptLoader("spell_pri_psychic_link") { }

        enum eSpells
        {
            PsychicLinkAura = 199484,
            PsychicLinkDmg  = 199486
        };

        class spell_pri_psychic_link_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_psychic_link_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::PsychicLinkAura);

                if (!l_Caster || !l_Caster->HasAura(eSpells::PsychicLinkAura) || !l_SpellInfo)
                    return;

                int32 l_Damage = GetHitDamage();

                l_Damage = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::PsychicLink) = l_Target->GetGUID();
                l_Caster->CastCustomSpell(l_Target, eSpells::PsychicLinkDmg, &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pri_psychic_link_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_psychic_link_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by Psychic Link - 199486
class spell_pri_psychic_link_damage : public SpellScriptLoader
{
public:
    spell_pri_psychic_link_damage() : SpellScriptLoader("spell_pri_psychic_link_damage") { }

    enum eSpells
    {
        ShadowWordPain = 589
    };

    class spell_pri_psychic_link_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_psychic_link_damage_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            p_Targets.remove_if(JadeCore::UnitAuraCheck(false, eSpells::ShadowWordPain, l_Caster->GetGUID()));
            p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
            {
                if (!p_Object)
                    return true;

                Unit* l_UnitObject = p_Object->ToUnit();
                if (!l_UnitObject)
                    return true;

                if (l_UnitObject->GetGUID() == l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::PsychicLink))
                    return true;

                return false;
            });
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_psychic_link_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_psychic_link_damage_SpellScript();
    }
};

/// Last Update 7.3.5 26365
/// Called by Mass Dispel 32375
class spell_pri_mass_dispel_ally : public SpellScriptLoader
{
    public:
        spell_pri_mass_dispel_ally() : SpellScriptLoader("spell_pri_mass_dispel_ally") { }

        class spell_pri_mass_dispel_ally_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mass_dispel_ally_SpellScript);

            enum eSpells
            {
                Imprison     = 221527,
                Cyclone      = 33786
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (l_SpellInfo == nullptr)
                    return;

                /// TODO: Remove targets that don't have any dispellable auras
                if (uint32 l_Count = l_SpellInfo->Effects[SpellEffIndex::EFFECT_3].BasePoints)
                    p_Targets.resize(l_Count);
            }

            void HandleOnHit(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Target->IsFriendlyTo(l_Caster) && l_Target->HasAura(eSpells::Imprison))
                    l_Target->RemoveAura(eSpells::Imprison);

                if (l_Target->IsFriendlyTo(l_Caster) && l_Target->HasAura(eSpells::Cyclone))
                    l_Target->RemoveAura(eSpells::Cyclone);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_mass_dispel_ally_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_DEST_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_pri_mass_dispel_ally_SpellScript::HandleOnHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DISPEL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mass_dispel_ally_SpellScript();
        }
};

/// Last Update 7.3.5 26365
/// Called by Mass Dispel 32592
class spell_pri_mass_dispel_enemy : public SpellScriptLoader
{
    public:
        spell_pri_mass_dispel_enemy() : SpellScriptLoader("spell_pri_mass_dispel_enemy") { }

        class spell_pri_mass_dispel_enemy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mass_dispel_enemy_SpellScript);

            enum eSpells
            {
                Dispel = 32375
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Dispel);
                if (l_SpellInfo == nullptr)
                    return;

                /// TODO: Remove targets that don't have any dispellable auras
                if (uint32 l_Count = l_SpellInfo->Effects[SpellEffIndex::EFFECT_3].BasePoints)
                    p_Targets.resize(l_Count);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_mass_dispel_enemy_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mass_dispel_enemy_SpellScript();
        }
};

/// Last Update 7.1.5 23420
/// Called by Holy Ward  213610
class spell_pri_holy_ward : public SpellScriptLoader
{
    public:
        spell_pri_holy_ward() : SpellScriptLoader("spell_pri_holy_ward") { }

        class spell_pri_holy_ward_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_holy_ward_AuraScript);

            enum eSpells
            {
                Imprison = 221527,
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || !l_SpellInfo->_IsLossOfControl())
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /* p_EventInfo */)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                l_Aura->Remove();
            }

            void HandleEffectApply(AuraEffect const* /* p_AurEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(eSpells::Imprison, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
            }

            void HandleEffectRemove(AuraEffect const* /* p_AurEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(eSpells::Imprison, IMMUNITY_MECHANIC, MECHANIC_BANISH, false);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_holy_ward_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
                DoCheckProc += AuraCheckProcFn(spell_pri_holy_ward_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pri_holy_ward_AuraScript::HandleOnProc);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_holy_ward_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_holy_ward_AuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Called by Twist of Fate - 123254
class spell_pri_twist_of_fate_buff : public SpellScriptLoader
{
    public:
        spell_pri_twist_of_fate_buff() : SpellScriptLoader("spell_pri_twist_of_fate_buff") { }

        class spell_pri_twist_of_fate_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_twist_of_fate_buff_AuraScript);

            enum eSpells
            {
                VampiricAura = 34914
            };

            void HandleEffectApply(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Mode != AURA_EFFECT_HANDLE_REAL || l_Caster->m_SpellHelper.GetBool(eSpellHelpers::VampiricTouch) == true)
                    return;
            }

            void HandleEffectRemove(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Mode != AURA_EFFECT_HANDLE_REAL || l_Caster->m_SpellHelper.GetBool(eSpellHelpers::VampiricTouch) == false)
                    return;
;
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_twist_of_fate_buff_AuraScript::HandleEffectApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_twist_of_fate_buff_AuraScript::HandleEffectRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };
        AuraScript* GetAuraScript() const
        {
            return new spell_pri_twist_of_fate_buff_AuraScript();
        }
};

/// Update to Legion 7.1.5 build 23420
/// Called by Anund's Last Breath - 215209
class spell_pri_anunds_last_breath : public SpellScriptLoader
{
    public:
        spell_pri_anunds_last_breath() : SpellScriptLoader("spell_pri_anunds_last_breath") { }

        class spell_pri_anunds_last_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_anunds_last_breath_AuraScript);

            enum eSpells
            {
                ShadowWordPain      = 589,
                VampiricTouch       = 34914,
            };

            bool HandleProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || (l_SpellInfo->Id != eSpells::ShadowWordPain && l_SpellInfo->Id != eSpells::VampiricTouch))
                    return false;

                return true;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_anunds_last_breath_AuraScript::HandleProc);
            }
        };
        AuraScript* GetAuraScript() const
        {
            return new spell_pri_anunds_last_breath_AuraScript();
        }
};

/// Last Update 7.3.5
/// Overloaded with Ligth - 223166
class spell_pri_overloaded_with_light : public SpellScriptLoader
{
public:
    spell_pri_overloaded_with_light() : SpellScriptLoader("spell_pri_overloaded_with_light") { }

    class spell_pri_overloaded_with_light_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_overloaded_with_light_AuraScript);

        enum eSpells
        {
            LightOverload = 223126,
            LightWrathArtifact  = 207946
        };

        void HandleProc(ProcEventInfo& /*p_EventInfo*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            if (Aura* l_Aura = l_Caster->GetAura(LightOverload))
                l_Aura->SetStackAmount(0);
        }

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            if (Unit* l_Caster = GetCaster())
                if (Aura* l_Aura = l_Caster->GetAura(LightOverload))
                    if (l_Aura->GetStackAmount() < 250)
                        return false;

            return p_EventInfo.GetDamageInfo()->GetSpellInfo()->Id == LightWrathArtifact;
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_pri_overloaded_with_light_AuraScript::HandleProc);
            DoCheckProc += AuraCheckProcFn(spell_pri_overloaded_with_light_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_overloaded_with_light_AuraScript();
    }

    class spell_pri_overloaded_with_light_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_overloaded_with_light_SpellScript);

        SpellCastResult CheckBGorGreymane()
        {
            if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_pri_overloaded_with_light_SpellScript::CheckBGorGreymane);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_overloaded_with_light_SpellScript();
    }
};

/// Last Update 7.3.5
/// Light Overload stack - 223126
/// Call by Smite - 585, Plea = 200829, PurgetheWicked = 204197, ShadowWordPain = 589,
/// ShadowMend = 186263, PowerWordRadiance = 194509, Penance = 47540, Schism = 214621, PowerWordSolace = 129250,
/// DivineStar = 110744, Shadowfiend = 34433, Mindbender = 123040, Halo = 120517, LightsWrath = 207946,
class spell_pri_light_overload_stack : public SpellScriptLoader
{
public:
    spell_pri_light_overload_stack() : SpellScriptLoader("spell_pri_light_overload_stack") {}

    class spell_pri_light_overload_stack_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_light_overload_stack_SpellScript);

        enum eSpells
        {
            Smite = 585,
            Plea = 200829,
            PurgetheWicked = 204197,
            ShadowWordPain = 589,
            ShadowMend = 186263,
            ShadowMendDmg = 186439,
            PowerWordRadiance = 194509,
            Penance = 47540,
            PenanceDmg = 47666,
            Schism = 214621,
            PowerWordSolace = 129250,
            DivineStar = 110744,
            DivineStarHeal = 110745,
            DivineStarDmg = 122128,
            Shadowfiend = 34433,
            Mindbender = 123040,
            Halo = 120517,
            HaloDmg = 120696,
            HaloHeal = 120692,
            LightsWrath = 207946,

            OverloadedWithLight = 223166,
            LightOverload = 223126
        };

        void HandleOnCast()
        {
            /// Incrementing Stack amount of Light Overload with a higher step and at 250 cast Overloaded with Light
            if (Unit* l_Caster = GetCaster())
            {
                auto l_Aura = l_Caster->GetAura(LightOverload);

                if (l_Aura == nullptr)
                    return;

                auto l_Map = l_Caster->GetMap();
                auto l_Player = l_Caster->ToPlayer();

                if (!l_Player || l_Player->GetActiveSpecializationID() != SPEC_PRIEST_DISCIPLINE
                    || l_Player->InRatedBattleGround() || l_Map->IsBattleArena() || l_Map->IsRaid() || l_Map->IsChallengeMode())   ///< Shoudn't charge on this
                    return;

                if (!roll_chance_f(15.0f))
                    return;

                if (l_Aura->GetStackAmount() + 1 >= 250)
                    l_Aura->SetStackAmount(250);
                else
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() + 1);
            }
        }

        void HandleAfterHit()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            if (auto l_Aura = l_Caster->GetAura(eSpells::LightOverload))
            {
                if (l_Aura->GetStackAmount() >= 250)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::OverloadedWithLight, true);
                }
            }
        }

        void Register()
        {
            OnCast += SpellCastFn(spell_pri_light_overload_stack_SpellScript::HandleOnCast);
            AfterHit += SpellHitFn(spell_pri_light_overload_stack_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_light_overload_stack_SpellScript();
    }
};

/// Last Update 7.3.5
/// Light's Wrath - 214858
class spell_pri_lights_wrath_overloaded : public SpellScriptLoader
{
public:
    spell_pri_lights_wrath_overloaded() : SpellScriptLoader("spell_pri_lights_wrath_overloaded") { }

    class spell_pri_lights_wrath_overloaded_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_lights_wrath_overloaded_AuraScript);

        enum eSpells
        {
            LightOverload = 223126,
            OverloadedWithLight = 223166,
        };

        void HandleEffectApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
        {
            if (Unit* l_Caster = GetCaster()) 
            {
                l_Caster->CastSpell(l_Caster, eSpells::LightOverload, true);
            }
        }

        void HandleEffectRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
        {
            if (Unit* l_Caster = GetCaster())
            {
                l_Caster->RemoveAura(eSpells::LightOverload);
            }
        }

        void HandleAuraUpdate(const uint32 /*p_diff*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (auto l_Map = l_Caster->GetMap())
            {
                if ((l_Map->IsBattlegroundOrArena() || l_Map->IsChallengeMode()) && l_Caster->HasAura(eSpells::OverloadedWithLight))
                {
                    l_Caster->RemoveAura(OverloadedWithLight);
                }
            }
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_pri_lights_wrath_overloaded_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_pri_lights_wrath_overloaded_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            OnAuraUpdate += AuraUpdateFn(spell_pri_lights_wrath_overloaded_AuraScript::HandleAuraUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_lights_wrath_overloaded_AuraScript();
    }
};

/// Last update 7.3.2 Build 22522
/// Called by Evangelism - 246287
class spell_pri_evangelism : public SpellScriptLoader
{
    public:
        spell_pri_evangelism() : SpellScriptLoader("spell_pri_evangelism") { }

        class spell_pri_evangelism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_evangelism_SpellScript);

            enum eSpells
            {
                AtonementBuff = 194384
            };

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                if (GetSpellInfo())
                {
                    if (Aura* l_AtonementAura = l_Target->GetAura(eSpells::AtonementBuff))
                    {
                        l_AtonementAura->SetMaxDuration(l_AtonementAura->GetDuration() + GetSpellInfo()->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                        l_AtonementAura->SetDuration(l_AtonementAura->GetDuration() + GetSpellInfo()->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_evangelism_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_evangelism_SpellScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Overloaded with Light - 223166
class spell_pri_overloaded_with_light_mod : public SpellScriptLoader
{
    public:
        spell_pri_overloaded_with_light_mod() : SpellScriptLoader("spell_pri_overloaded_with_light_mod") { }

        class spell_pri_overloaded_with_light_mod_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_overloaded_with_light_mod_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.50f;   ///< Overloaded with Light is reduced by 50% in PvP
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_overloaded_with_light_mod_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_overloaded_with_light_mod_AuraScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Shadowform - 232698
class spell_pri_shadowform_taken : public SpellScriptLoader
{
    public:
        spell_pri_shadowform_taken() : SpellScriptLoader("spell_pri_shadowform_taken") { }

        class spell_pri_shadowform_taken_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadowform_taken_AuraScript);

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 2.f;   ///< Shadowform is reduced by 200% in PvP
            }

            void HandleEffectApply(AuraEffect const* l_AuraEffect, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ShadowForm) = true;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_shadowform_taken_AuraScript::HandleEffectApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_shadowform_taken_AuraScript::HandlePvPModifier, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_shadowform_taken_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Atonement - 81751 and Atonement (crit) - 94472
class spell_pri_atonement_healing : public SpellScriptLoader
{
    public:
        spell_pri_atonement_healing() : SpellScriptLoader("spell_pri_atonement_healing") { }

        class spell_pri_atonement_healing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_atonement_healing_SpellScript);

            enum eSpells
            {
                T192PDiscipline         = 211556,
                T192PDisciplinePWShield = 211558,
                PWShield                = 17
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                int32 l_Heal = GetHitHeal();

                /// Atonement's heal is increased by 15% in PvP
                /// Eg. https://worldofwarcraft.com/en-us/news/21365424
                if (l_Caster->CanApplyPvPSpellModifiers())
                    AddPct(l_Heal, 15);

                if (l_Caster->HasAura(eSpells::T192PDiscipline) && l_Target->HasAura(eSpells::PWShield))
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::T192PDisciplinePWShield);
                    if (l_SpellInfo)
                        AddPct(l_Heal, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                }

                SetHitHeal(l_Heal);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_atonement_healing_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_atonement_healing_SpellScript();
        }
};

/// 7.3.5
class PlayerScript_pri_overloaded_with_light : public PlayerScript
{
public:
    PlayerScript_pri_overloaded_with_light() :PlayerScript("PlayerScript_pri_overloaded_with_light") {}

    enum eSpells
    {
        OverloadedWithLight = 223166
    };

    void OnEnterBG(Player* p_Player, uint32 p_MapID) override
    {
        if (!p_Player || p_Player->getClass() != CLASS_PRIEST || p_Player->InRatedBattleGround())
            return;

        p_Player->RemoveAura(OverloadedWithLight);
    }
};

/// Update to Legion 7.1.5 build 23420
class PlayerScript_pri_priest : public PlayerScript
{
    public:
        PlayerScript_pri_priest() :PlayerScript("PlayerScript_pri_priest") {}

        enum eSpells
        {
            PriestAura = 137030
        };

        void OnModifySpec(Player* p_Player, int32 /*p_OldSpec*/, int32 /* p_NewSpec */) override
        {
            if (!p_Player)
                return;

            /// Need to recalculate amount because of 'spell_pri_priest_aura' script
            if (AuraEffect* l_AuraEffect = p_Player->GetAuraEffect(eSpells::PriestAura, SpellEffIndex::EFFECT_2))
                l_AuraEffect->RecalculateAmount();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Void Eruption 228360 - 228361
class spell_pri_void_eruption_damages : public SpellScriptLoader
{
    public:
        spell_pri_void_eruption_damages() : SpellScriptLoader("spell_pri_void_eruption_damages") { }

        class spell_pri_void_eruption_damages_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_eruption_damages_SpellScript);

            enum eSpells
            {
                HeartOfTheVoid          = 248296,
                HeartOfTheVoidHeal      = 248219
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HeartOfTheVoid);
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_Damages = GetHitDamage();
                if (!l_Caster->HasAura(eSpells::HeartOfTheVoid))
                    return;

                SetHitDamage(AddPct(l_Damages, l_SpellInfo->Effects[EFFECT_0].BasePoints));
                int32 l_Healing = CalculatePct(l_Damages, l_SpellInfo->Effects[EFFECT_1].BasePoints);
                l_Caster->CastCustomSpell(l_Caster, eSpells::HeartOfTheVoidHeal, &l_Healing, nullptr, nullptr, true);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HeartOfTheVoid);
                if (!l_Caster || !l_SpellInfo)
                    return;

                int32 l_Damages = GetHitDamage();
                if (!l_Caster->HasAura(eSpells::HeartOfTheVoid))
                    return;

                int32 l_Healing = CalculatePct(l_Damages, l_SpellInfo->Effects[EFFECT_1].BasePoints);
                l_Caster->CastCustomSpell(l_Caster, eSpells::HeartOfTheVoidHeal, &l_Healing, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_void_eruption_damages_SpellScript::HandleOnHit);
                AfterHit += SpellHitFn(spell_pri_void_eruption_damages_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_void_eruption_damages_SpellScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called for Legendary : The Alabaster Lady - 248295
/// Called by Prayer of Mending - 33076
class spell_pri_the_alabaster_lady : public SpellScriptLoader
{
    public:
        spell_pri_the_alabaster_lady() : SpellScriptLoader("spell_pri_the_alabaster_lady") { }

        class spell_pri_the_alabaster_lady_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_the_alabaster_lady_SpellScript);

            enum eSpells
            {
                TheAlabasterLady    = 248295,
                Apotheosis          = 200183
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TheAlabasterLady);
                if (!l_Caster || !l_SpellInfo || !l_Caster->HasAura(eSpells::TheAlabasterLady))
                    return;

                if (!roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Apotheosis, true);

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Apotheosis))
                    l_Aura->SetDuration(9 * IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_the_alabaster_lady_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_the_alabaster_lady_SpellScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Legendary: Soul of the High Priest - 247591
class spell_pri_soul_of_the_high_priest : public SpellScriptLoader
{
    public:
        spell_pri_soul_of_the_high_priest() : SpellScriptLoader("spell_pri_soul_of_the_high_priest") { }

        class spell_pri_soul_of_the_high_priest_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_soul_of_the_high_priest_AuraScript);

            enum eSpells
            {
                SurgeOfLight = 109186,
                TwistOfFate  = 109142
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
                case SPEC_PRIEST_HOLY:
                    l_Player->CastSpell(l_Player, eSpells::SurgeOfLight, true);
                    break;
                default:
                    l_Player->CastSpell(l_Player, eSpells::TwistOfFate, true);
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
                case SPEC_PRIEST_HOLY:
                    l_Player->RemoveAura(eSpells::SurgeOfLight);
                    break;
                default:
                    l_Player->RemoveAura(eSpells::TwistOfFate);
                    break;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_soul_of_the_high_priest_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_soul_of_the_high_priest_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_soul_of_the_high_priest_AuraScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Divin Hymn - 64844
class spell_pri_divine_hymn : public SpellScriptLoader
{
    public:
        spell_pri_divine_hymn() : SpellScriptLoader("spell_pri_divine_hymn") { }

        class spell_pri_divine_hymn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_divine_hymn_SpellScript);

            void HandleHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Unit = GetHitUnit();
                if (!l_Caster || !l_Unit)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                Player* l_Target = l_Unit->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                /// Divin Hymn heals for 100% when you are not in Raid group
                GroupPtr l_Group = l_Player->GetOriginalGroup();
                if (!l_Group)
                    l_Group = l_Player->GetGroup();

                if (l_Group && !l_Group->isRaidGroup())
                    SetHitHeal(GetHitHeal() * 2);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_divine_hymn_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_divine_hymn_SpellScript();
        }
};

/// Last update 7.3.5 Build 26365
/// Called by Prayer of Healing - 596
class spell_pri_prayer_of_healing : public SpellScriptLoader
{
    public:
        spell_pri_prayer_of_healing() : SpellScriptLoader("spell_pri_prayer_of_healing") { }

        class spell_pri_prayer_of_healing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_prayer_of_healing_SpellScript);

            enum eSpells
            {
                EverlastingHope = 253443
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    Unit* l_Target = GetExplTargetUnit();
                    if (!l_Target)
                        l_Target = l_Caster;

                    else if (!l_Target->IsFriendlyTo(l_Caster))
                        l_Target = l_Caster;

                    uint8 l_MaxTargets = GetSpellInfo()->Effects[EFFECT_2].BasePoints;

                    std::list<WorldObject*> l_PlayerTargets = p_Targets;
                    l_PlayerTargets.remove_if([=](WorldObject* l_TargetInList) -> bool
                    {
                        return (l_TargetInList->GetTypeId() != TYPEID_PLAYER);
                    });
                    if (!l_PlayerTargets.empty())
                        l_PlayerTargets.sort(JadeCore::DistanceCompareOrderPred(l_Target));

                    if (l_PlayerTargets.size() >= l_MaxTargets)
                    {
                        l_PlayerTargets.resize(l_MaxTargets);
                        p_Targets = l_PlayerTargets;
                        return;
                    }

                    std::list<WorldObject*> l_NonPlayerTargets = p_Targets;
                    l_NonPlayerTargets.remove_if([=](WorldObject* l_TargetInList) -> bool
                    {
                        return (l_TargetInList->GetTypeId() == TYPEID_PLAYER);
                    });
                    if (!l_NonPlayerTargets.empty())
                        l_NonPlayerTargets.sort(JadeCore::DistanceCompareOrderPred(l_Target));

                    if (!l_PlayerTargets.empty() && !l_NonPlayerTargets.empty())
                    {
                        p_Targets = l_PlayerTargets;
                        for (auto l_Itr : l_NonPlayerTargets)
                            p_Targets.push_back(l_Itr);
                    }
                    else if (!l_PlayerTargets.empty())
                    {
                        p_Targets = l_PlayerTargets;
                    }
                    else if (!l_NonPlayerTargets.empty())
                    {
                        p_Targets = l_NonPlayerTargets;
                    }
                    else
                    {
                        p_Targets.clear();
                        return;
                    }

                    if (!p_Targets.empty())
                    {
                        if (p_Targets.size() > l_MaxTargets)
                            p_Targets.resize(l_MaxTargets);

                        p_Targets.push_back(l_Target);
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;
                l_Caster->RemoveAura(eSpells::EverlastingHope);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_prayer_of_healing_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
                AfterCast += SpellCastFn(spell_pri_prayer_of_healing_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_prayer_of_healing_SpellScript();
        }
};

/// Last update 7.3.2 Build 25549
/// Called by Inner Renewal - 195606
class spell_pri_inner_renewal: public SpellScriptLoader
{
    public:
        spell_pri_inner_renewal() : SpellScriptLoader("spell_pri_inner_renewal") { }

        class spell_pri_inner_renewal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_inner_renewal_AuraScript);

            enum eSpells
            {
                InnerRenewal = 195559
            };

            void HandleProc(ProcEventInfo& p_EventInfo)
            {
                Aura* l_Aura = GetAura();
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_Aura || !l_DamageInfo)
                    return;

                AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0);
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_AuraEffect || !l_Target || l_Caster != l_Target || !l_SpellInfo)
                    return;

                int32 cost[MAX_POWERS_COST];
                memset(cost, 0, sizeof(uint32)* MAX_POWERS_COST);
                cost[MAX_POWERS_COST - 1] = 0;
                l_SpellInfo->CalcPowerCost(l_Caster, l_SpellInfo->GetSchoolMask(), cost);

                if (!cost[Powers::POWER_MANA])
                    return;

                int32 l_SpellCost = cost[Powers::POWER_MANA];
                l_SpellCost = CalculatePct(l_SpellCost, l_AuraEffect->GetAmount());
                l_Caster->EnergizeBySpell(l_Caster, eSpells::InnerRenewal, l_SpellCost, Powers::POWER_MANA);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pri_inner_renewal_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_inner_renewal_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called Vampiric Embrace Dummy - 15286
class spell_pri_vampiric_embrace_dummy : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_embrace_dummy() : SpellScriptLoader("spell_pri_vampiric_embrace_dummy") { }

        class spell_pri_vampiric_embrace_dummy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_embrace_dummy_AuraScript);

            enum eSpells
            {
                VampiricEmbraceHeal = 15290,
                FleetingEmbrace     = 199388
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_VampiricInfo = GetSpellInfo();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_DamageInfo || !l_VampiricInfo)
                    return;

                int32 l_HealAmount = (l_DamageInfo->GetAmount()) * (l_VampiricInfo->Effects[EFFECT_0].BasePoints) / 100;

                Player* l_CasterPlayer = l_Caster->ToPlayer();
                if (!l_CasterPlayer)
                    return;

                GroupPtr l_Group = l_CasterPlayer->GetGroup();
                if (!l_Group)
                {
                    l_Caster->CastCustomSpell(l_Caster, eSpells::VampiricEmbraceHeal, &l_HealAmount, nullptr, nullptr, true);
                    return;
                }

                float l_Dist = sSpellMgr->GetSpellInfo(eSpells::VampiricEmbraceHeal)->Effects[EFFECT_0].CalcRadius(l_CasterPlayer);

                std::list<Player*> l_PlayerList;
                l_CasterPlayer->GetSortedGroupMembers(l_PlayerList, [l_Dist, l_CasterPlayer](Player* p_Player) -> bool
                {
                    return l_CasterPlayer->IsWithinDist3d(p_Player, l_Dist);
                }, 1,
                {
                    [](Player* p_Player) -> bool
                    {
                        return !p_Player->IsFullHealth();
                    }
                });

                l_PlayerList.sort(JadeCore::HealthPctOrderPred());
                if (l_PlayerList.empty())
                    return;

                l_Caster->CastCustomSpell(l_PlayerList.front(), eSpells::VampiricEmbraceHeal, &l_HealAmount, nullptr, nullptr, true);

            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_vampiric_embrace_dummy_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_embrace_dummy_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY); ///< changed in SpellMgr.cpp
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_vampiric_embrace_dummy_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Focus in the Light - 196419
class spell_pri_focus_in_the_light : public SpellScriptLoader
{
    public:
        spell_pri_focus_in_the_light() : SpellScriptLoader("spell_pri_focus_in_the_light") { }

        class spell_pri_focus_in_the_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_focus_in_the_light_AuraScript);

            enum eSpells
            {
                FocusedWill = 45242,
                HolyFire = 14914,
                HolyWord = 88625,
                FocusInTheLightTarget = 210979,
                FocusInTheLightCaster = 210980
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::FocusedWill))
                    return false;

                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                if (l_SpellInfo->Id != eSpells::HolyFire && l_SpellInfo->Id != eSpells::HolyWord)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FocusInTheLightCaster, true);
                l_Caster->CastSpell(l_Target, eSpells::FocusInTheLightTarget, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_focus_in_the_light_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_focus_in_the_light_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_focus_in_the_light_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Fiendind Dark - 238065
class spell_pri_fiending_dark : public SpellScriptLoader
{
    public:
        spell_pri_fiending_dark() : SpellScriptLoader("spell_pri_fiending_dark") { }

        class spell_pri_fiending_dark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_fiending_dark_AuraScript);

            void HandleSpellMod(AuraEffect const* /* p_AurEff */, SpellModifier*& p_SpellMod)
            {
                p_SpellMod->value /= 3;
            }

            void Register() override
            {
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pri_fiending_dark_AuraScript::HandleSpellMod, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_fiending_dark_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Lenience's Reward - 238063
class spell_pri_lenience_reward : public SpellScriptLoader
{
    public:
        spell_pri_lenience_reward() : SpellScriptLoader("spell_pri_lenience_reward") { }

        class spell_pri_lenience_reward_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_lenience_reward_AuraScript);

            void HandleSpellMod(AuraEffect const* /* p_AurEff */, SpellModifier*& p_SpellMod)
            {
                p_SpellMod->value /= 100;
            }

            void Register() override
            {
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pri_lenience_reward_AuraScript::HandleSpellMod, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_lenience_reward_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Pain Supression - 33206
class spell_pri_pain_suppression : public SpellScriptLoader
{
    public:
        spell_pri_pain_suppression() : SpellScriptLoader("spell_pri_pain_suppression") { }

        class spell_pri_pain_suppression_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_pain_suppression_AuraScript);

            enum eSpells
            {
                TyrannyOfPain       = 238099,
                TyrannyOfPainHeal   = 242094
            };

            void HandleProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_Caster->HasAura(eSpells::TyrannyOfPain) || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TyrannyOfPain);
                if (!l_SpellInfo | !l_Target)
                    return;

                int32 l_Healing = l_DamageInfo->GetAmount();
                l_Healing = CalculatePct(l_Healing, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                l_Caster->CastCustomSpell(l_Target, eSpells::TyrannyOfPainHeal, &l_Healing, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_pri_pain_suppression_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_pain_suppression_AuraScript();
        }

        class spell_pri_pain_suppression_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_pain_suppression_SpellScript);

            enum eSpells
            {
                Cyclone     = 33786,
                Sap         = 6770
            };

            SpellCastResult CheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->HasAura(eSpells::Cyclone))
                    {
                        SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_DO_WHILE_CYCYLONED);
                        return SPELL_FAILED_CUSTOM_ERROR;
                    }

                    if (l_Caster->HasAura(eSpells::Sap))
                    {
                        SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_YOU_CANNOT_DO_THAT_RIGHT_NOW);
                        return SPELL_FAILED_CUSTOM_ERROR;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_pain_suppression_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_pain_suppression_SpellScript();
        }
};

/// Last Update 7.3.2 build 25549
/// Called by Mind Quickening - 238101
class spell_pri_mind_quickening : public SpellScriptLoader
{
    public:
        spell_pri_mind_quickening() : SpellScriptLoader("spell_pri_mind_quickening") { }

        class spell_pri_mind_quickening_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mind_quickening_AuraScript);

            bool HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_mind_quickening_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mind_quickening_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Mind Quickening - 240673
class spell_pri_mind_quickening_triggered : public SpellScriptLoader
{
    public:
        spell_pri_mind_quickening_triggered() : SpellScriptLoader("spell_pri_mind_quickening_triggered") { }

        enum eSpells
        {
            MindQuickeningAura = 238101
        };

        class spell_pri_mind_quickening_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_quickening_triggered_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MindQuickeningAura);
                if (!l_SpellInfo)
                    return;

                uint8 l_MaxTargets = l_SpellInfo->Effects[EFFECT_1].BasePoints + 1; ///< + 1 caster

                if (p_Targets.size() > l_MaxTargets)
                    p_Targets.resize(l_MaxTargets);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_mind_quickening_triggered_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ALLY_OR_RAID);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mind_quickening_triggered_SpellScript();
        }
};

/// 7.3.2 - Build 25549
/// Times and Measures - 238100
class spell_priest_times_and_measures : public SpellScriptLoader
{
    public:
        spell_priest_times_and_measures() : SpellScriptLoader("spell_priest_times_and_measures") { }

        class spell_priest_times_and_measures_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_priest_times_and_measures_AuraScript);

            enum eSpells
            {
                DesperatePrayer = 19236
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (!l_Caster || !l_DamageInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Player || (l_SpellInfo && l_SpellInfo->Id == eSpells::DesperatePrayer))
                    return;

                if (!UpdateProcCooldown(false))
                    return;

                m_Damage += l_DamageInfo->GetAmount();

                float l_Chance = float((float)m_Damage / l_Caster->GetMaxHealth() * 100.0f);
                if (roll_chance_f(l_Chance))
                {
                    if (SpellInfo const* l_DP = sSpellMgr->GetSpellInfo(eSpells::DesperatePrayer))
                        if (uint32 l_ReduceDuration = l_DP->GetRecoveryTime())
                        {
                            UpdateProcCooldown(true);
                            l_Player->ReduceSpellCooldown(l_DP->Id, l_ReduceDuration);
                            m_Damage = 0;
                        }
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_priest_times_and_measures_AuraScript::HandleOnProc);
            }

        private:
            int32 m_Damage = 0;
            uint64 m_ProcCooldown = 0;

            bool UpdateProcCooldown(bool p_Cooldown)
            {
                if (m_ProcCooldown <= time(NULL))
                {
                    if (p_Cooldown)
                        m_ProcCooldown = time(NULL) + 30;
                    return true;
                }

                return false;
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_priest_times_and_measures_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Greater Heal - 215960
class spell_pri_greater_heal : public SpellScriptLoader
{
    public:
        spell_pri_greater_heal() : SpellScriptLoader("spell_pri_greater_heal") { }

        class spell_pri_greater_heal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_greater_heal_AuraScript);

            enum eSpells
            {
                Heal = 2060
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id != eSpells::Heal)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_greater_heal_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_greater_heal_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Dark Archangel - 197871
class spell_pri_dark_archangel : public SpellScriptLoader
{
    public:
        spell_pri_dark_archangel() : SpellScriptLoader("spell_pri_dark_archangel") { }

        class spell_pri_dark_archangel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_dark_archangel_SpellScript);

            enum eSpells
            {
                DarkArchangel = 197874,
                Atonement = 194384
            };

            void HandleScriptEffect(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster != l_Target && l_Target->HasAura(eSpells::Atonement) && l_Target->IsInRaidWith(l_Caster) && l_Target->IsFriendlyTo(l_Caster) && !l_Target->isPet())
                    l_Caster->CastSpell(l_Target, eSpells::DarkArchangel, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_dark_archangel_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_dark_archangel_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Inner Focus - 196762
/// Called By Heal - 2060, Flash Heal - 2061, Holy Fire - 14914, Smite - 585, Holy Word: Serenity - 2050
class spell_pri_inner_focus : public SpellScriptLoader
{
    public:
        spell_pri_inner_focus() : SpellScriptLoader("spell_pri_inner_focus") { }

        class spell_pri_inner_focus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_inner_focus_SpellScript);

            enum eSpells
            {
                InnerFocus = 196762,
                InnerFocusTrigger = 196773,
                Serenity = 2050,
                Heal = 2060,
                FlashHeal = 2061,
                HolyFire = 14914,
                Smite = 585
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (m_scriptSpellId == eSpells::InnerFocus)
                    l_Caster->CastSpell(l_Caster, eSpells::InnerFocusTrigger, true);
                else
                    l_Caster->RemoveAura(eSpells::InnerFocus);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pri_inner_focus_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_inner_focus_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Cosmic Ripple - 238136
class spell_pri_cosmic_ripple : public SpellScriptLoader
{
    public:
        spell_pri_cosmic_ripple() : SpellScriptLoader("spell_pri_cosmic_ripple") { }

        enum eSpells
        {
            Serenity = 2050,
            Sanctify = 34861,
            CosmicRipple = 243241
        };

        class spell_pri_cosmic_ripple_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_cosmic_ripple_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (m_scriptSpellId == eSpells::Serenity)
                    l_Player->m_SpellHelper.GetBool(eSpellHelpers::CosmicRippleSerenity) = true;
                if (m_scriptSpellId == eSpells::Sanctify)
                    l_Player->m_SpellHelper.GetBool(eSpellHelpers::CosmicRippleSanctify) = true;
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::Serenity || m_scriptSpellId == eSpells::Sanctify)
                    AfterCast += SpellCastFn(spell_pri_cosmic_ripple_SpellScript::HandleAfterCast);
            }
        };

        class spell_pri_cosmic_ripple_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_cosmic_ripple_AuraScript);

            void OnUpdate(uint32 p_Diff)
            {
                if (m_UpdateTimer > p_Diff)
                {
                    m_UpdateTimer -= p_Diff;
                    return;
                }

                m_UpdateTimer = 1 * IN_MILLISECONDS;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                bool l_Proc = l_Player->m_SpellHelper.GetBool(eSpellHelpers::CosmicRippleSerenity);
                bool l_Cooldown = false;
                if (SpellInfo const* l_Serenity = sSpellMgr->GetSpellInfo(eSpells::Serenity))
                {
                    if (SpellCategoryEntry const* l_SpellCategoryEntry = l_Serenity->ChargeCategoryEntry)
                    {
                        auto l_Itr = l_Caster->ToPlayer()->m_CategoryCharges.find(l_SpellCategoryEntry->Id);
                        if (l_Itr != l_Caster->ToPlayer()->m_CategoryCharges.end())
                        {
                            if (!l_Itr->second.empty())
                            {
                                Clock::time_point l_Now = Clock::now();
                                std::chrono::milliseconds l_CooldownDuration = std::chrono::duration_cast<std::chrono::milliseconds>(l_Itr->second.front().RechargeEnd - l_Now);
                                if (l_CooldownDuration.count() > 0)
                                    l_Cooldown = true;
                            }
                        }
                    }
                }

                if (l_Proc && !l_Cooldown)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::CosmicRipple, true);
                    l_Player->m_SpellHelper.GetBool(eSpellHelpers::CosmicRippleSerenity) = false;
                }

                l_Proc = l_Player->m_SpellHelper.GetBool(eSpellHelpers::CosmicRippleSanctify);
                l_Cooldown = l_Player->HasSpellCooldown(eSpells::Sanctify);
                if (l_Proc && !l_Cooldown)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::CosmicRipple, true);
                    l_Player->m_SpellHelper.GetBool(eSpellHelpers::CosmicRippleSanctify) = false;
                }
            }

            void Register() override
            {
                if (m_scriptSpellId != Serenity && m_scriptSpellId != eSpells::Sanctify)
                    OnAuraUpdate += AuraUpdateFn(spell_pri_cosmic_ripple_AuraScript::OnUpdate);
            }

        private:
            uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_cosmic_ripple_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_cosmic_ripple_AuraScript();
        }
};

/// Cosmic Ripple (Heal) - 243241
class spell_pri_cosmic_ripple_heal : public SpellScriptLoader
{
    public:
        spell_pri_cosmic_ripple_heal() : SpellScriptLoader("spell_pri_cosmic_ripple_heal") { }

        class spell_pri_cosmic_ripple_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_cosmic_ripple_heal_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                std::list<WorldObject*> l_Targets;

                for (WorldObject* l_Object : p_Targets)
                {
                    if (l_Object->IsPlayer())
                        l_Targets.push_back(l_Object);
                }

                /// First process players
                if (!l_Targets.empty())
                    l_Targets.sort(JadeCore::HealthPctOrderPred());

                uint32 l_Max = GetSpellInfo()->MaxAffectedTargets;

                /// Too much players, just need 5
                if (uint32(l_Targets.size()) > l_Max)
                {
                    p_Targets.clear();

                    /// 5 most injured allies players
                    for (WorldObject* l_Object : l_Targets)
                    {
                        p_Targets.push_back(l_Object);

                        if (!(--l_Max))
                            break;
                    }
                }
                /// Not enough players, complete with NPCs
                else if (uint32(l_Targets.size()) < l_Max)
                {
                    /// Remove already processed players
                    for (WorldObject* l_Object : l_Targets)
                        p_Targets.remove(l_Object);

                    /// Re-order by health pct
                    if (!p_Targets.empty())
                        p_Targets.sort(JadeCore::HealthPctOrderPred());

                    for (WorldObject* l_Object : p_Targets)
                    {
                        l_Targets.push_back(l_Object);

                        if (uint32(l_Targets.size()) >= l_Max)
                            break;
                    }

                    p_Targets = l_Targets;
                }
                /// Else, exactly 5 injured players, nothing more to do
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_cosmic_ripple_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_cosmic_ripple_heal_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Mind Flay - 193473
/// Called for Lash of Insanity - 238137, Mind Sear - 234702
class spell_pri_lash_of_insanity : public SpellScriptLoader
{
    public:
        spell_pri_lash_of_insanity() : SpellScriptLoader("spell_pri_lash_of_insanity") { }

        class spell_pri_lash_of_insanity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_lash_of_insanity_AuraScript);

            enum eSpells
            {
                LashofInsanityAura = 238137,
                LashofInsanitySpell = 240843,
                MindSearAOE = 237388
            };

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;		
				
                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_Target = GetTarget();

                if (l_Target && spell_pri_mind_sear::CanCastMindSearAoe(l_Owner, GetTarget()))
                    l_Caster->CastSpell(l_Target, eSpells::MindSearAOE, true);

                if (l_Owner->HasAura(eSpells::LashofInsanityAura))
                    l_Owner->CastSpell(l_Owner, eSpells::LashofInsanitySpell, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_lash_of_insanity_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_lash_of_insanity_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Shadow Dagger Kill - 221594
class spell_pri_shadow_dagger_kill : public SpellScriptLoader
{
    public:
        spell_pri_shadow_dagger_kill() : SpellScriptLoader("spell_pri_shadow_dagger_kill")
        {}

        enum eCreatures
        {
            NpcXalathat = 111221
        };

        class spell_shadow_dagger_kill_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shadow_dagger_kill_AuraScript);

            void HandleOnProc(ProcEventInfo& p_Proc)
            {
                if (GetUnitOwner() == nullptr || p_Proc.GetActor() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                Creature* l_Whisper = l_Owner->SummonCreature(eCreatures::NpcXalathat, *l_Owner, TEMPSUMMON_TIMED_DESPAWN, 20 * IN_MILLISECONDS);

                if (l_Whisper != nullptr && l_Whisper->IsAIEnabled)
                    l_Whisper->GetAI()->SetData(10, p_Proc.GetActionTarget()->GetEntry());
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_shadow_dagger_kill_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shadow_dagger_kill_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
class spell_pri_resurection : public SpellScriptLoader
{
    public:
        spell_pri_resurection() : SpellScriptLoader("spell_pri_resurection")
        {}

        enum eCreatures
        {
            NpcXalathat = 111221
        };

        class spell_pri_resurection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_resurection_SpellScript);

            enum eSpells
            {
                ShadowForm = 232698
            };

            bool m_HasAura = false;

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->m_SpellHelper.GetBool(eSpellHelpers::ShadowForm))
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetActiveSpecializationID() == SPEC_PRIEST_SHADOW)
                    l_Caster->CastSpell(l_Caster, eSpells::ShadowForm, true);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_pri_resurection_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_resurection_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Levitate - 252620
class spell_pri_levitate_effect : public SpellScriptLoader
{
    public:
        spell_pri_levitate_effect() : SpellScriptLoader("spell_pri_levitate_effect") { }

        class spell_pri_levitate_effect_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_levitate_effect_AuraScript);

            enum eSpells
            {
                GlyphHeavens = 120581,
                GlyphHeavensTrigger = 124433
            };

            void HandleApply(AuraEffect const* /*p_AuraEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::GlyphHeavens))
                    l_Target->CastSpell(l_Target, eSpells::GlyphHeavensTrigger, true);
            }

            void HandleRemove(AuraEffect const* /*p_AuraEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpells::GlyphHeavensTrigger);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_levitate_effect_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_pri_levitate_effect_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_levitate_effect_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Mind Quickening - 240673
class spell_pri_mind_quickening_amount: public SpellScriptLoader
{
    public:
        spell_pri_mind_quickening_amount() : SpellScriptLoader("spell_pri_mind_quickening_amount") { }

        class spell_pri_mind_quickening_amount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mind_quickening_amount_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = 800;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_mind_quickening_amount_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mind_quickening_amount_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by 242269 - Priest T20 Discipline 4P Bonus
class spell_pri_tos_set : public SpellScriptLoader
{
    public:
        spell_pri_tos_set() : SpellScriptLoader("spell_pri_tos_set") { }

        class spell_pri_tos_set_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_tos_set_AuraScript);

            enum eSpells
            {
                PWShield     = 17,
            };

            bool HandleCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                    if (l_SpellInfo->Id == eSpells::PWShield)
                        return true;

                return false;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_tos_set_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_tos_set_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called by 251831 - Priest T21 Holy 2P Bonus
class spell_pri_argus_2p : public SpellScriptLoader
{
public:
    spell_pri_argus_2p() : SpellScriptLoader("spell_pri_argus_2p") { }

    class spell_pri_argus_2p_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_argus_2p_AuraScript);

        enum eSpells
        {
            PrayerOfHealing = 596
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                if (l_SpellInfo->Id == eSpells::PrayerOfHealing)
                    return true;

            return false;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_argus_2p_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_argus_2p_AuraScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called by 251842 - Priest T21 Holy 4P Bonus
class spell_pri_holy_argus_4p : public SpellScriptLoader
{
public:
    spell_pri_holy_argus_4p() : SpellScriptLoader("spell_pri_holy_argus_4p") { }

    class spell_pri_holy_argus_4p_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_holy_argus_4p_AuraScript);

        enum eSpells
        {
            FlashHeal     = 2061,
            Heal          = 2060,
            BindingOfHeal = 32546
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                if (l_SpellInfo->Id == eSpells::FlashHeal || l_SpellInfo->Id == eSpells::Heal || l_SpellInfo->Id == eSpells::BindingOfHeal)
                    return true;

            return false;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_holy_argus_4p_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_holy_argus_4p_AuraScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Called for 246519 - Penitent: Priest T20 Discipline 4P Bonus
/// Called By Penance - 47750, Penance - 47666
class spell_pri_penitent : public SpellScriptLoader
{
    public:
        spell_pri_penitent() : SpellScriptLoader("spell_pri_penitent") { }

        class spell_pri_penitent_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penitent_SpellScript);

            enum eSpells
            {
                Penitent     = 246519,
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasAura(eSpells::Penitent))
                    return;

                l_Caster->RemoveAura(eSpells::Penitent);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_penitent_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_penitent_SpellScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Called By Priest T20 Holy 4P Bonus - 242271
class spell_pri_holy_tos_bonus : public SpellScriptLoader
{
    public:
        spell_pri_holy_tos_bonus() : SpellScriptLoader("spell_pri_holy_tos_bonus") { }

        class spell_pri_holy_tos_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_holy_tos_bonus_AuraScript);

            enum eSpells
            {
                HWChastise    = 88625,
                HWSanctify    = 34861,
                HWSerenity    = 2050
            };

            Guid128 m_CastGUID;

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const*  l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || !l_Player || (l_SpellInfo->Id != eSpells::HWSanctify &&
                    l_SpellInfo->Id != eSpells::HWSerenity &&
                    l_SpellInfo->Id != eSpells::HWChastise))
                    return;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_Spell)
                    return;

                if (m_CastGUID == l_Spell->GetCastGuid())
                    return;

                m_CastGUID = l_Spell->GetCastGuid();
                SpellInfo const* l_BonusSetInfo = GetSpellInfo();
                if (!l_BonusSetInfo && roll_chance_i(40))
                    return;

                uint32 l_TotalCd = l_Player->GetSpellCooldownDelay(l_SpellInfo->Id);
                uint32 l_Reduced = CalculatePct(l_TotalCd, l_BonusSetInfo->Effects[EFFECT_0].BasePoints);
                uint32 l_SpellId = l_SpellInfo->Id;
                l_Player->m_SpellHelper.GetBool(eSpellHelpers::T20Holy4PBonusBlock) = true;
                l_Player->m_Functions.AddFunction([l_SpellId, l_Player, l_Reduced] () -> void
                {
                    if (l_SpellId == eSpells::HWSerenity)
                        l_Player->ReduceChargeCooldown(sSpellMgr->GetSpellInfo(l_SpellId)->ChargeCategoryEntry, 30 * IN_MILLISECONDS);
                    else
                        l_Player->ReduceSpellCooldown(l_SpellId, l_Reduced);
                    l_Player->m_SpellHelper.GetBool(eSpellHelpers::T20Holy4PBonusBlock) = false;
                }, l_Player->m_Functions.CalculateTime(100));
            }

            void Register()
            {
                OnProc += AuraProcFn(spell_pri_holy_tos_bonus_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_holy_tos_bonus_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Mind Control - 605
class spell_pri_mind_control_duration : public SpellScriptLoader
{
    public:
        spell_pri_mind_control_duration() : SpellScriptLoader("spell_pri_mind_control_duration") { }

        class spell_pri_mind_control_duration_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_control_duration_SpellScript);

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                if (!l_Target->ToPlayer())
                    return;

                DiminishingGroup l_DiminishingGroup = GetDiminishingReturnsGroupForSpell(GetSpellInfo(), l_Caster);
                if (l_DiminishingGroup != DIMINISHING_DISORIENT)
                    return;

                DiminishingLevels l_DiminishingLevel = l_Target->GetDiminishing(l_DiminishingGroup);
                int32 l_Duration = GetSpellInfo()->GetMaxDuration();
                int32 l_LimitDuration = GetDiminishingReturnsLimitDuration(GetSpellInfo(), l_Caster);
                l_Target->ApplyDiminishingToDuration(l_DiminishingGroup, l_Duration, l_Caster, l_DiminishingLevel, l_LimitDuration);

                if (Spell* l_ChannelSpell = GetSpell())
                    if (l_ChannelSpell->GetSpellInfo()->Id == GetSpellInfo()->Id)
                        l_ChannelSpell->SetChanneledDuration(l_Duration);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_pri_mind_control_duration_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mind_control_duration_SpellScript();
        }
};

/// Called for Disc argus 4P - 251844
class spell_pri_disc_radiant_focus : public SpellScriptLoader
{
public:
    spell_pri_disc_radiant_focus() : SpellScriptLoader("spell_pri_disc_radiant_focus") { }

    class spell_pri_disc_radiant_focus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_disc_radiant_focus_AuraScript);

        enum eSpells
        {
            PowerWordRadiance = 194509
        };

        bool HandleCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_DamageInfo)
                return false;

            if (SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo())
                if (l_SpellInfo->Id == eSpells::PowerWordRadiance)
                    return true;

            return false;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_disc_radiant_focus_AuraScript::HandleCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_disc_radiant_focus_AuraScript();
    }
};

/// Last Update 7.1.0 Build 22864
/// Shadowy Apparition - 78203
class spell_pri_shadowy_apparition_passive: public SpellScriptLoader
{
    public:
        spell_pri_shadowy_apparition_passive() : SpellScriptLoader("spell_pri_shadowy_apparition_passive")
        {}

        enum eSpells
        {
            ShadowWordPain              = 589,
            ShadowyApparitionTrigger    = 147193,
        };

        class spell_pri_shadowy_apparition_passive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadowy_apparition_passive_AuraScript);

            bool HandleCheckProc(ProcEventInfo & p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr)
                    return false;

                Unit* l_Target = p_Proc.GetActionTarget();
                SpellInfo const* p_ProcSpell = p_Proc.GetDamageInfo()->GetSpellInfo();

                if (l_Target == nullptr || p_ProcSpell == nullptr)
                    return false;

                return  (l_Target->HasAura(eSpells::ShadowWordPain) &&
                        p_Proc.GetHitMask() & PROC_EX_CRITICAL_HIT &&
                        p_ProcSpell->Id == ShadowWordPain);
            }

            void HandleOnProc(ProcEventInfo& p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr ||
                    p_Proc.GetActionTarget() == nullptr ||
                    GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();
                Unit* l_Target = p_Proc.GetActionTarget();

                l_Owner->SendPlaySpellVisual(33584, l_Target, 6.0f, 0.0f, Position());
                l_Owner->CastSpell(l_Target, eSpells::ShadowyApparitionTrigger, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_shadowy_apparition_passive_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_pri_shadowy_apparition_passive_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_shadowy_apparition_passive_AuraScript();
        }
};

// Heal - 2060
class spell_pri_heal : public SpellScriptLoader
{
    public:
        spell_pri_heal() : SpellScriptLoader("spell_pri_heal") { }

        class spell_pri_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_heal_SpellScript);

            enum eSpells
            {
                T21Holy2P = 253437
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::T21Holy2P);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pri_heal_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_heal_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_priest_spell_scripts()
{
    new spell_pri_resurection();
    new spell_pri_psychic_link();
    new spell_pri_psychic_link_damage();
    new spell_pri_ray_of_hope();
    new spell_pri_premonition();
    new spell_pri_renew_the_faith();
    new spell_pri_renew_the_faith_proc();
    new spell_pri_renew_the_faith_periodic();
    new spell_pri_archbishop_benedictus_restitution();
    new spell_pri_lights_wrath();
    new spell_pri_shadow_mania();
    new spell_pri_void_shift();
    new spell_pri_void_shift_effect();
    new spell_pri_surrendered_soul();
    new spell_pri_divinity();
    new spell_pri_smite_absorb();
    new spell_pri_penance_aura();
    new spell_pri_focused_will();
    new spell_pri_shadowy_apparition();
    new spell_pri_mind_blast();
    new spell_pri_shadow_word_pain();
    new spell_pri_angelic_feather();
    new spell_pri_spirit_shell();
    new spell_pri_clarity_of_will();
    new spell_pri_confession();
    new spell_pri_psychic_horror();
    new spell_pri_spirit_of_redemption();
    new spell_pri_spirit_of_redemption_form();
    new spell_pri_item_s12_4p_heal();
    new spell_pri_item_s12_2p_shadow();
    new spell_pri_shadowy_insight();
    new spell_pri_power_word_solace();
    new spell_pri_shadowfiend();
    new spell_pri_surge_of_light();
    new spell_pri_power_word_shield();
    new spell_pri_body_and_soul();
    new spell_pri_purify();
    new spell_pri_phantasm();
    new spell_pri_leap_of_faith();
    new spell_pri_psychic_horror();
    new spell_pri_guardian_spirit();
    new spell_pri_flash_heal();
    new spell_pri_twist_of_fate();
    new spell_pri_surge_of_light_aura();
    new spell_pri_binding_heal();
    new spell_pri_dominate_mind();
    new spell_pri_spirit_shell_effect();
    new spell_pri_mind_trauma();
    new spell_pri_mind_trauma_debuff();
    new spell_pri_lights_wrath_overloaded();
    new spell_pri_overloaded_with_light();
    new spell_pri_light_overload_stack();

    /// All Spec Spells
    new spell_pri_shadow_mend_heal();
    new spell_pri_shadow_mend();
    new spell_pri_smite();
    new spell_pri_levitate();
    new spell_pri_power_of_the_dark_side();
    new spell_pri_power_of_the_dark_side_passive();
    new spell_pri_priest_aura();
    new spell_pri_mass_dispel_ally();
    new spell_pri_mass_dispel_enemy();
    new spell_pri_soul_of_the_high_priest();
    new spell_pri_levitate_effect();
    new spell_pri_mind_control_duration();

    /// Discipline
    new spell_pri_atonement();
    new spell_pri_atonement_buff();
    new spell_pri_archangel_atonement();
    new spell_pri_radiance();
    new spell_pri_searing_light();
    new spell_pri_renewed_hope();
    new spell_pri_archangel();
    new spell_pri_cord_of_maiev_priestess_of_the_moon();
    new spell_pri_nero_band_of_promises_targets();
    new spell_pri_nero_band_of_promises_heal();
    new spell_pri_dejahnas_inspiration();
    new spell_pri_shadow_covenant();
    new spell_pri_shadow_covenant_shield();
    new spell_pri_purge_the_wicked();
    new spell_pri_purge_the_wicked_dummy();
    new spell_pri_penance();
    new spell_pri_vestments_of_discipline();
    new spell_pri_evangelism();
    new spell_pri_lenience_reward();
    new spell_pri_pain_suppression();
    new spell_pri_penitent();
    new spell_pri_tos_set();
    new spell_pri_disc_radiant_focus();

    /// Shadow
    RegisterSpellScript(spell_pri_void_bolt_missile);
    new spell_pri_void_form_passive();
    new spell_pri_void_eruption();
    new spell_pri_void_form();
    new spell_pri_lingering_insanity();
    new spell_pri_void_bolt();
    new spell_pri_shadow_word_death();
    new spell_pri_thrive_in_the_shadows();
    new spell_pri_mind_sear();
    new spell_pri_mind_sear_energize();
    new spell_pri_zenkaram_iridis_anadem();
    new spell_pri_the_twins_painful_touch();
    new spell_pri_the_twins_painful_touch_spread();
    new spell_pri_call_to_the_void();
    new spell_pri_unleash_the_shadows();
    new spell_pri_void_torrent();
    new spell_pri_mind_bomb();
    new spell_pri_last_word();
    new spell_pri_mania();
    new spell_pri_mind_control();
    new spell_pri_dispersion();
    new spell_pri_mind_spike();
    new spell_pri_vampiric_touch();
    new spell_pri_sphere_of_insanity_dmg();
    new spell_pri_mental_fortitude();
    new spell_pri_twist_of_fate_buff();
    new spell_pri_anunds_last_breath();
    new spell_pri_void_eruption_damages();
    new spell_pri_vampiric_embrace_dummy();
    new spell_pri_fiending_dark();
    new spell_pri_lash_of_insanity();
    new spell_pri_mind_quickening_amount();
    new spell_pri_shadowy_apparition_passive();

    /// Holy
    new spell_pri_holy_nova();
    new spell_pri_chastise();
    new spell_pri_serendipity();
    new spell_pri_invoke_the_naaru();
    new spell_pri_dazzling_lights();
    new spell_pri_prayer_of_mending();
    new spell_pri_prayer_of_mending_aura();
    new spell_pri_almaiesh();
    new spell_pri_enduring_renewal();
    new spell_pri_desperate_prayer();
    new spell_pri_trail_of_light();
    new spell_pri_rammals_ulterior_motive();
    new spell_pri_power_of_the_naaru();
    new spell_pri_renew();
    new spell_pri_the_alabaster_lady();
    new spell_pri_divine_hymn();
    new spell_pri_mind_quickening();
    new spell_pri_mind_quickening_triggered();
    new spell_priest_times_and_measures();
    new spell_pri_greater_heal();
    new spell_pri_prayer_of_healing();
    new spell_pri_holy_tos_bonus();
    new spell_pri_argus_2p();
    new spell_pri_holy_argus_4p();
    new spell_pri_heal();

    /// Artefact
    new spell_pri_focus_in_the_light();
    new spell_pri_cosmic_ripple();
    new spell_pri_cosmic_ripple_heal();
    new spell_pri_shadow_dagger_kill();

    /// Honor Talent
    new spell_pri_void_shield();
    new spell_pri_void_shield_aura();
    new spell_pri_delivered_from_evil();
    new spell_pri_deliverance();
    new spell_pri_power_word_fortitude();
    new spell_pri_edge_of_insanity_passive();
    new spell_pri_holy_ward();
    new spell_pri_inner_renewal();
    new spell_pri_driven_to_madness();
    new spell_pri_dark_archangel();
    new spell_pri_inner_focus();

    /// PlayerScripts
    new PlayerScript_pri_atonement();
    new PlayerScript_pri_mania();
    new PlayerScript_pri_censure();
    new PlayerScript_pri_holy_concentration();
    new PlayerScript_pri_overloaded_with_light();
    new PlayerScript_pri_priest();
    new PlayerScript_pri_power_word_fortitude();
    new PlayerScript_pri_sins_of_the_many();

    /// PvP Modifiers
    new spell_pri_overloaded_with_light_mod();
    new spell_pri_shadowform_taken();
    new spell_pri_atonement_healing();
}
#endif
