////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_GENERIC spells used by items.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_item_".
 */

#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SkillDiscovery.h"
#include "HelperDefines.h"
#include "WowTime.hpp"
#include "GameEventMgr.h"
#include "GridNotifiers.h"

/// Last Update 7.1.5 Build 23420
/// Masquerade - 202477
class spell_item_masquerade : public SpellScriptLoader
{
    public:
        spell_item_masquerade() : SpellScriptLoader("spell_item_masquerade") { }

        class spell_item_masquerade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_masquerade_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    uint32 l_ModelId;
                    switch (l_Player->getGender())
                    {
                        case GENDER_MALE:
                            l_ModelId = 66592;
                            break;
                        case GENDER_FEMALE:
                            l_ModelId = 66276;
                            break;
                        default:
                            l_ModelId = 66592;
                            break;
                    }
                    l_Player->SetDisplayId(l_ModelId);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_item_masquerade_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_masquerade_SpellScript();
        }

        class spell_item_masquerade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_masquerade_AuraScript);

            enum eSpells
            {
                MasqueradeOverride  = 211135
            };

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->AddAura(eSpells::MasqueradeOverride, l_Caster);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_masquerade_AuraScript::HandleAfterEffectRemove, EFFECT_3, SPELL_AURA_FORCE_REACTION, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_masquerade_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Leyflame Burner - 189624
class spell_item_leyflame_burner : public SpellScriptLoader
{
    public:
        spell_item_leyflame_burner() : SpellScriptLoader("spell_item_leyflame_burner") { }

        class spell_item_leyflame_burner_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_leyflame_burner_SpellScript);

            enum eCredits
            {
                NarthalasEnergyKillCredit   = 95670,
                DragonPoolKillCredit        = 95668,
                ZarkhenarEnergyKillCredit   = 95669
            };

            enum eAreas
            {
                NarthalasAcademy            = 7366,
                LeyRuinsOfZarkhenar         = 7361,
                SenegosDragonPool           = 7340
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    uint32 l_AreaId = l_Player->GetAreaId();
                    switch (l_AreaId)
                    {
                        case eAreas::NarthalasAcademy:
                        {
                            l_Player->KilledMonsterCredit(eCredits::NarthalasEnergyKillCredit);
                            break;
                        }
                        case eAreas::SenegosDragonPool:
                        {
                            l_Player->KilledMonsterCredit(eCredits::DragonPoolKillCredit);
                            break;
                        }
                        case eAreas::LeyRuinsOfZarkhenar:
                        {
                            l_Player->KilledMonsterCredit(eCredits::ZarkhenarEnergyKillCredit);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_leyflame_burner_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_leyflame_burner_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Drink - 192001
class spell_item_ley_enriched_water : public SpellScriptLoader
{
    public:
        spell_item_ley_enriched_water() : SpellScriptLoader("spell_item_ley_enriched_water") { }

        class spell_item_ley_enriched_water_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_ley_enriched_water_AuraScript);

            void HandlePeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ModifyPower(Powers::POWER_MANA, p_AurEff->GetAmount() / 5);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_ley_enriched_water_AuraScript::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_ley_enriched_water_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Arrogance - 234113
class spell_item_arrogance : public SpellScriptLoader
{
public:
    spell_item_arrogance() : SpellScriptLoader("spell_item_arrogance") { }

    class spell_item_arrogance_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_arrogance_AuraScript);

        enum eSpells
        {
            ArroganceDebuff = 234115,
            ArroganceMarker = 234317
        };
        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            Unit* l_Target = l_DamageInfo->GetTarget();
            if (!l_Target || l_Target->HasAura(eSpells::ArroganceMarker))
                return false;

            return true;
        }
        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return;

            Unit* l_Target = l_DamageInfo->GetTarget();

            Unit* l_Caster = GetCaster();
            if (!l_Target || !l_Caster)
                return;

            l_Caster->CastSpell(l_Target, eSpells::ArroganceDebuff, true);
            l_Caster->CastSpell(l_Target, eSpells::ArroganceMarker, true);

        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_arrogance_AuraScript::HandleOnCheckProc);
            OnProc += AuraProcFn(spell_item_arrogance_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_arrogance_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Allies of Nature - 222512
class spell_item_allies_of_nature : public SpellScriptLoader
{
    public:
        spell_item_allies_of_nature() : SpellScriptLoader("spell_item_allies_of_nature") { }

        class spell_item_allies_of_nature_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_allies_of_nature_AuraScript);

            enum eSpells
            {
                CleansedAncientsBlessing    = 222517,
                CleansedWispsBlessing       = 222518,
                CleansedSistersBlessing     = 222519,
                CleansedDrakesBreath        = 222520
            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::CleansedAncientsBlessing,
                eSpells::CleansedWispsBlessing,
                eSpells::CleansedSistersBlessing
            };

            void HandleProc(ProcEventInfo& /*p_EvenInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                AuraEffect const* l_AuraEffect0 = l_Aura->GetEffect(EFFECT_0);
                AuraEffect const* l_AuraEffect1 = l_Aura->GetEffect(EFFECT_1);
                if (!l_Item || !l_AuraEffect0 || !l_AuraEffect1)
                    return;

                int32 l_Bp0 = l_AuraEffect0->GetAmount();
                int32 l_Bp1 = l_AuraEffect1->GetAmount();
                l_Caster->CastCustomSpell(l_Caster, eSpells::CleansedDrakesBreath, &l_Bp0, nullptr, nullptr, true, l_Item);
                l_Caster->CastCustomSpell(l_Caster, m_Spells[urand(0, m_Spells.size() - 1)], &l_Bp1, nullptr, nullptr, true, l_Item);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_allies_of_nature_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_allies_of_nature_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Nerubian Chitin - 214492
class spell_item_nerubian_chitin : public SpellScriptLoader
{
    public:
        spell_item_nerubian_chitin() : SpellScriptLoader("spell_item_nerubian_chitin") { }

        class spell_item_nerubian_chitin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nerubian_chitin_AuraScript);

            enum eSpells
            {
                NerubianChitin = 214493
            };

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::NerubianChitin, true, l_Item);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_nerubian_chitin_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nerubian_chitin_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Nerubian Chitin - 214493
class spell_item_nerubian_chitin_periodic : public SpellScriptLoader
{
    public:
        spell_item_nerubian_chitin_periodic() : SpellScriptLoader("spell_item_nerubian_chitin_periodic") { }

        class spell_item_nerubian_chitin_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nerubian_chitin_periodic_AuraScript);

            enum eSpells
            {
                NerubianChitin = 214494
            };

            void HandleOnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::NerubianChitin);
                if (!l_Aura)
                    return;

                uint8 l_Tick = p_AuraEffect->GetTickNumber();

                if (l_Aura->GetStackAmount() < p_AuraEffect->GetTotalTicks())
                    l_Aura->ModStackAmount(1);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_nerubian_chitin_periodic_AuraScript::HandleOnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nerubian_chitin_periodic_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Nerubian Chitin - 214494
class spell_item_nerubian_chitin_stack : public SpellScriptLoader
{
    public:
        spell_item_nerubian_chitin_stack() : SpellScriptLoader("spell_item_nerubian_chitin_stack") { }

        class spell_item_nerubian_chitin_stack_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nerubian_chitin_stack_AuraScript);

            bool HandleDontRefresh(bool p_Decrease)
            {
                return false;
            }

            void Register() override
            {
                CanRefreshProc += AuraCanRefreshProcFn(spell_item_nerubian_chitin_stack_AuraScript::HandleDontRefresh);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nerubian_chitin_stack_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Devilsaur's Stampede - 224059
class spell_item_devilsaurs_stampede : public SpellScriptLoader
{
    public:
        spell_item_devilsaurs_stampede() : SpellScriptLoader("spell_item_devilsaurs_stampede") { }

        class spell_item_devilsaurs_stampede_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_devilsaurs_stampede_AuraScript);

            enum eSpells
            {
                DevilsaursStampedePeriodic = 224060
            };

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DevilsaursStampedePeriodic, true, l_Item);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_devilsaurs_stampede_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_devilsaurs_stampede_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Devilsaur's Stampede - 224060
class spell_item_devilsaurs_stampede_periodic : public SpellScriptLoader
{
    public:
        spell_item_devilsaurs_stampede_periodic() : SpellScriptLoader("spell_item_devilsaurs_stampede_periodic") { }

        class spell_item_devilsaurs_stampede_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_devilsaurs_stampede_periodic_AuraScript);

            enum eSpells
            {
                DevilsaursStampedeDamage = 224061
            };

            void HandlePeriodic(AuraEffect const* /*p_AurEff*/)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DevilsaursStampedeDamage, true, l_Item);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_devilsaurs_stampede_periodic_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_devilsaurs_stampede_periodic_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Ice Bomb - 214584
class spell_item_ice_bomb : public SpellScriptLoader
{
    public:
        spell_item_ice_bomb() : SpellScriptLoader("spell_item_ice_bomb") { }

        class spell_item_ice_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_ice_bomb_SpellScript);

            enum eSpells
            {
                IceBomb     = 214584,
                FrigidArmor = 214589
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Item* l_Item = GetCastItem();
                if (!l_Caster || !l_Item)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::FrigidArmor, true, l_Item);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_item_ice_bomb_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_ice_bomb_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Avalanche Elixir - 188021
class spell_item_avalanche_elixir : public SpellScriptLoader
{
    public:
        spell_item_avalanche_elixir() : SpellScriptLoader("spell_item_avalanche_elixir") { }

        class spell_item_avalanche_elixir_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_avalanche_elixir_AuraScript);

            enum eSpells
            {
                AvalancheElixir         = 188021,
                AvalancheElixirVisual   = 188414
            };

            void HandleOnEffectPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
                    return;

                float l_FloorZ = l_Caster->GetMap()->GetHeight(l_Caster->GetPhaseMask(), l_Caster->GetPositionX(), l_Caster->GetPositionY(), l_Caster->GetPositionZ());
                if (l_Caster->GetPositionZ() - l_FloorZ > 5.0f)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::AvalancheElixirVisual, true);
                    l_Caster->RemoveAura(eSpells::AvalancheElixir);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_avalanche_elixir_AuraScript::HandleOnEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_avalanche_elixir_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Avalanche Elixir - 188414
class spell_item_avalanche_elixir_visual : public SpellScriptLoader
{
    public:
        spell_item_avalanche_elixir_visual() : SpellScriptLoader("spell_item_avalanche_elixir_visual") { }

        class spell_item_avalanche_elixir_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_avalanche_elixir_visual_AuraScript);

            enum eSpells
            {
                AvalancheElixirDamages = 188416
            };

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::AvalancheElixirDamages, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_avalanche_elixir_visual_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_avalanche_elixir_visual_AuraScript();
        }
};

// Generic script for handling item dummy effects which trigger another spell.
class spell_item_trigger_spell: public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;

    public:
        spell_item_trigger_spell(const char* name, uint32 triggeredSpellId) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_item_trigger_spell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_trigger_spell_SpellScript);
        private:
            uint32 _triggeredSpellId;

        public:
            spell_item_trigger_spell_SpellScript(uint32 triggeredSpellId) : SpellScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Item* item = GetCastItem())
                    caster->CastSpell(caster, _triggeredSpellId, true, item);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_trigger_spell_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_trigger_spell_SpellScript(_triggeredSpellId);
        }
};

// http://www.wowhead.com/item=6522 Deviate Fish
// 8063 Deviate Fish
enum DeviateFishSpells
{
    SPELL_SLEEPY            = 8064,
    SPELL_INVIGORATE        = 8065,
    SPELL_SHRINK            = 8066,
    SPELL_PARTY_TIME        = 8067,
    SPELL_HEALTHY_SPIRIT    = 8068
};

class spell_item_deviate_fish: public SpellScriptLoader
{
    public:
        spell_item_deviate_fish() : SpellScriptLoader("spell_item_deviate_fish") { }

        class spell_item_deviate_fish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_deviate_fish_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                for (uint32 spellId = SPELL_SLEEPY; spellId <= SPELL_HEALTHY_SPIRIT; ++spellId)
                    if (!sSpellMgr->GetSpellInfo(spellId))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = urand(SPELL_SLEEPY, SPELL_HEALTHY_SPIRIT);
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_deviate_fish_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_deviate_fish_SpellScript();
        }
};

// http://www.wowhead.com/item=47499 Flask of the North
// 67019 Flask of the North
enum FlaskOfTheNorthSpells
{
    SPELL_FLASK_OF_THE_NORTH_SP = 67016,
    SPELL_FLASK_OF_THE_NORTH_AP = 67017,
    SPELL_FLASK_OF_THE_NORTH_STR = 67018
};

class spell_item_flask_of_the_north: public SpellScriptLoader
{
    public:
        spell_item_flask_of_the_north() : SpellScriptLoader("spell_item_flask_of_the_north") { }

        class spell_item_flask_of_the_north_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_flask_of_the_north_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FLASK_OF_THE_NORTH_SP) || !sSpellMgr->GetSpellInfo(SPELL_FLASK_OF_THE_NORTH_AP) || !sSpellMgr->GetSpellInfo(SPELL_FLASK_OF_THE_NORTH_STR))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                std::vector<uint32> possibleSpells;
                switch (caster->getClass())
                {
                    case CLASS_WARLOCK:
                    case CLASS_MAGE:
                    case CLASS_PRIEST:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_SP);
                        break;
                    case CLASS_DEATH_KNIGHT:
                    case CLASS_WARRIOR:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_STR);
                        break;
                    case CLASS_ROGUE:
                    case CLASS_HUNTER:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_AP);
                        break;
                    case CLASS_DRUID:
                    case CLASS_PALADIN:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_SP);
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_STR);
                        break;
                    case CLASS_SHAMAN:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_SP);
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_AP);
                        break;
                }

                caster->CastSpell(caster, possibleSpells[irand(0, (possibleSpells.size() - 1))], true, NULL);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_flask_of_the_north_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_flask_of_the_north_SpellScript();
        }
};

// http://www.wowhead.com/item=10645 Gnomish Death Ray
// 13280 Gnomish Death Ray
enum GnomishDeathRay
{
    SPELL_GNOMISH_DEATH_RAY_SELF = 13493,
    SPELL_GNOMISH_DEATH_RAY_TARGET = 13279
};

class spell_item_gnomish_death_ray: public SpellScriptLoader
{
    public:
        spell_item_gnomish_death_ray() : SpellScriptLoader("spell_item_gnomish_death_ray") { }

        class spell_item_gnomish_death_ray_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_gnomish_death_ray_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GNOMISH_DEATH_RAY_SELF) || !sSpellMgr->GetSpellInfo(SPELL_GNOMISH_DEATH_RAY_TARGET))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    if (urand(0, 99) < 15)
                        caster->CastSpell(caster, SPELL_GNOMISH_DEATH_RAY_SELF, true, NULL);    // failure
                    else
                        caster->CastSpell(target, SPELL_GNOMISH_DEATH_RAY_TARGET, true, NULL);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_gnomish_death_ray_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_gnomish_death_ray_SpellScript();
        }
};

// http://www.wowhead.com/item=27388 Mr. Pinchy
// 33060 Make a Wish
enum MakeAWish
{
    SPELL_MR_PINCHYS_BLESSING       = 33053,
    SPELL_SUMMON_MIGHTY_MR_PINCHY   = 33057,
    SPELL_SUMMON_FURIOUS_MR_PINCHY  = 33059,
    SPELL_TINY_MAGICAL_CRAWDAD      = 33062,
    SPELL_MR_PINCHYS_GIFT           = 33064
};

class spell_item_make_a_wish: public SpellScriptLoader
{
    public:
        spell_item_make_a_wish() : SpellScriptLoader("spell_item_make_a_wish") { }

        class spell_item_make_a_wish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_make_a_wish_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MR_PINCHYS_BLESSING) || !sSpellMgr->GetSpellInfo(SPELL_SUMMON_MIGHTY_MR_PINCHY) || !sSpellMgr->GetSpellInfo(SPELL_SUMMON_FURIOUS_MR_PINCHY) || !sSpellMgr->GetSpellInfo(SPELL_TINY_MAGICAL_CRAWDAD) || !sSpellMgr->GetSpellInfo(SPELL_MR_PINCHYS_GIFT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = SPELL_MR_PINCHYS_GIFT;
                switch (urand(1, 5))
                {
                    case 1: spellId = SPELL_MR_PINCHYS_BLESSING; break;
                    case 2: spellId = SPELL_SUMMON_MIGHTY_MR_PINCHY; break;
                    case 3: spellId = SPELL_SUMMON_FURIOUS_MR_PINCHY; break;
                    case 4: spellId = SPELL_TINY_MAGICAL_CRAWDAD; break;
                }
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_make_a_wish_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_make_a_wish_SpellScript();
        }
};

// http://www.wowhead.com/item=32686 Mingo's Fortune Giblets
// 40802 Mingo's Fortune Generator
class spell_item_mingos_fortune_generator: public SpellScriptLoader
{
    public:
        spell_item_mingos_fortune_generator() : SpellScriptLoader("spell_item_mingos_fortune_generator") { }

        class spell_item_mingos_fortune_generator_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_mingos_fortune_generator_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                // Selecting one from Bloodstained Fortune item
                uint32 newitemid;
                switch (urand(1, 20))
                {
                    case 1:  newitemid = 32688; break;
                    case 2:  newitemid = 32689; break;
                    case 3:  newitemid = 32690; break;
                    case 4:  newitemid = 32691; break;
                    case 5:  newitemid = 32692; break;
                    case 6:  newitemid = 32693; break;
                    case 7:  newitemid = 32700; break;
                    case 8:  newitemid = 32701; break;
                    case 9:  newitemid = 32702; break;
                    case 10: newitemid = 32703; break;
                    case 11: newitemid = 32704; break;
                    case 12: newitemid = 32705; break;
                    case 13: newitemid = 32706; break;
                    case 14: newitemid = 32707; break;
                    case 15: newitemid = 32708; break;
                    case 16: newitemid = 32709; break;
                    case 17: newitemid = 32710; break;
                    case 18: newitemid = 32711; break;
                    case 19: newitemid = 32712; break;
                    case 20: newitemid = 32713; break;
                    default:
                        return;
                }

                CreateItem(effIndex, newitemid);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_mingos_fortune_generator_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_mingos_fortune_generator_SpellScript();
        }
};

// http://www.wowhead.com/item=10720 Gnomish Net-o-Matic Projector
// 13120 Net-o-Matic
enum NetOMaticSpells
{
    SPELL_NET_O_MATIC_TRIGGERED1 = 16566,
    SPELL_NET_O_MATIC_TRIGGERED2 = 13119,
    SPELL_NET_O_MATIC_TRIGGERED3 = 13099
};

class spell_item_net_o_matic: public SpellScriptLoader
{
    public:
        spell_item_net_o_matic() : SpellScriptLoader("spell_item_net_o_matic") { }

        class spell_item_net_o_matic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_net_o_matic_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NET_O_MATIC_TRIGGERED1) || !sSpellMgr->GetSpellInfo(SPELL_NET_O_MATIC_TRIGGERED2) || !sSpellMgr->GetSpellInfo(SPELL_NET_O_MATIC_TRIGGERED3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    uint32 spellId = SPELL_NET_O_MATIC_TRIGGERED3;
                    uint32 roll = urand(0, 99);
                    if (roll < 2)                            // 2% for 30 sec self root (off-like chance unknown)
                        spellId = SPELL_NET_O_MATIC_TRIGGERED1;
                    else if (roll < 4)                       // 2% for 20 sec root, charge to target (off-like chance unknown)
                        spellId = SPELL_NET_O_MATIC_TRIGGERED2;

                    GetCaster()->CastSpell(target, spellId, true, NULL);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_net_o_matic_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_net_o_matic_SpellScript();
        }
};

// http://www.wowhead.com/item=8529 Noggenfogger Elixir
// 16589 Noggenfogger Elixir
enum NoggenfoggerElixirSpells
{
    SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED1 = 16595,
    SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED2 = 16593,
    SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED3 = 16591
};

class spell_item_noggenfogger_elixir: public SpellScriptLoader
{
    public:
        spell_item_noggenfogger_elixir() : SpellScriptLoader("spell_item_noggenfogger_elixir") { }

        class spell_item_noggenfogger_elixir_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_noggenfogger_elixir_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED1) || !sSpellMgr->GetSpellInfo(SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED2) || !sSpellMgr->GetSpellInfo(SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED3;
                switch (urand(1, 3))
                {
                    case 1: spellId = SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED1; break;
                    case 2: spellId = SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED2; break;
                }

                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_noggenfogger_elixir_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_noggenfogger_elixir_SpellScript();
        }
};

// http://www.wowhead.com/item=6657 Savory Deviate Delight
// 8213 Savory Deviate Delight
enum SavoryDeviateDelight
{
    SPELL_FLIP_OUT_MALE     = 8219,
    SPELL_FLIP_OUT_FEMALE   = 8220,
    SPELL_YAAARRRR_MALE     = 8221,
    SPELL_YAAARRRR_FEMALE   = 8222
};

class spell_item_savory_deviate_delight: public SpellScriptLoader
{
    public:
        spell_item_savory_deviate_delight() : SpellScriptLoader("spell_item_savory_deviate_delight") { }

        class spell_item_savory_deviate_delight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_savory_deviate_delight_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                for (uint32 spellId = SPELL_FLIP_OUT_MALE; spellId <= SPELL_YAAARRRR_FEMALE; ++spellId)
                    if (!sSpellMgr->GetSpellInfo(spellId))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = 0;
                switch (urand(1, 2))
                {
                    // Flip Out - ninja
                    case 1: spellId = (caster->getGender() == GENDER_MALE ? SPELL_FLIP_OUT_MALE : SPELL_FLIP_OUT_FEMALE); break;
                    // Yaaarrrr - pirate
                    case 2: spellId = (caster->getGender() == GENDER_MALE ? SPELL_YAAARRRR_MALE : SPELL_YAAARRRR_FEMALE); break;
                }
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_savory_deviate_delight_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_savory_deviate_delight_SpellScript();
        }
};

// http://www.wowhead.com/item=7734 Six Demon Bag
// 14537 Six Demon Bag
enum SixDemonBagSpells
{
    SPELL_FROSTBOLT                 = 11538,
    SPELL_POLYMORPH                 = 14621,
    SPELL_SUMMON_FELHOUND_MINION    = 14642,
    SPELL_FIREBALL                  = 15662,
    SPELL_CHAIN_LIGHTNING           = 21179,
    SPELL_ENVELOPING_WINDS          = 25189
};

class spell_item_six_demon_bag: public SpellScriptLoader
{
    public:
        spell_item_six_demon_bag() : SpellScriptLoader("spell_item_six_demon_bag") { }

        class spell_item_six_demon_bag_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_six_demon_bag_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FROSTBOLT) || !sSpellMgr->GetSpellInfo(SPELL_POLYMORPH) || !sSpellMgr->GetSpellInfo(SPELL_SUMMON_FELHOUND_MINION) || !sSpellMgr->GetSpellInfo(SPELL_FIREBALL) || !sSpellMgr->GetSpellInfo(SPELL_CHAIN_LIGHTNING) || !sSpellMgr->GetSpellInfo(SPELL_ENVELOPING_WINDS))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    uint32 spellId = 0;
                    uint32 rand = urand(0, 99);
                    if (rand < 25)                      // Fireball (25% chance)
                        spellId = SPELL_FIREBALL;
                    else if (rand < 50)                 // Frostball (25% chance)
                        spellId = SPELL_FROSTBOLT;
                    else if (rand < 70)                 // Chain Lighting (20% chance)
                        spellId = SPELL_CHAIN_LIGHTNING;
                    else if (rand < 80)                 // Polymorph (10% chance)
                    {
                        spellId = SPELL_POLYMORPH;
                        if (urand(0, 100) <= 30)        // 30% chance to self-cast
                            target = caster;
                    }
                    else if (rand < 95)                 // Enveloping Winds (15% chance)
                        spellId = SPELL_ENVELOPING_WINDS;
                    else                                // Summon Felhund minion (5% chance)
                    {
                        spellId = SPELL_SUMMON_FELHOUND_MINION;
                        target = caster;
                    }

                    caster->CastSpell(target, spellId, true, GetCastItem());
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_six_demon_bag_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_six_demon_bag_SpellScript();
        }
};

// http://www.wowhead.com/item=44012 Underbelly Elixir
// 59640 Underbelly Elixir
enum UnderbellyElixirSpells
{
    SPELL_UNDERBELLY_ELIXIR_TRIGGERED1 = 59645,
    SPELL_UNDERBELLY_ELIXIR_TRIGGERED2 = 59831,
    SPELL_UNDERBELLY_ELIXIR_TRIGGERED3 = 59843
};

class spell_item_underbelly_elixir: public SpellScriptLoader
{
    public:
        spell_item_underbelly_elixir() : SpellScriptLoader("spell_item_underbelly_elixir") { }

        class spell_item_underbelly_elixir_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_underbelly_elixir_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }
            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_UNDERBELLY_ELIXIR_TRIGGERED1) || !sSpellMgr->GetSpellInfo(SPELL_UNDERBELLY_ELIXIR_TRIGGERED2) || !sSpellMgr->GetSpellInfo(SPELL_UNDERBELLY_ELIXIR_TRIGGERED3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = SPELL_UNDERBELLY_ELIXIR_TRIGGERED3;
                switch (urand(1, 3))
                {
                    case 1: spellId = SPELL_UNDERBELLY_ELIXIR_TRIGGERED1; break;
                    case 2: spellId = SPELL_UNDERBELLY_ELIXIR_TRIGGERED2; break;
                }
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_underbelly_elixir_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_underbelly_elixir_SpellScript();
        }
};

enum eShadowmourneVisuals
{
    SPELL_SHADOWMOURNE_VISUAL_LOW       = 72521,
    SPELL_SHADOWMOURNE_VISUAL_HIGH      = 72523,
    SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF  = 73422
};

class spell_item_shadowmourne: public SpellScriptLoader
{
public:
    spell_item_shadowmourne() : SpellScriptLoader("spell_item_shadowmourne") { }

    class spell_item_shadowmourne_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_shadowmourne_AuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_VISUAL_LOW) || !sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_VISUAL_HIGH) || !sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF))
                return false;
            return true;
        }

        void OnStackChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            switch (GetStackAmount())
            {
                case 1:
                    target->CastSpell(target, SPELL_SHADOWMOURNE_VISUAL_LOW, true);
                    break;
                case 6:
                    target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_LOW);
                    target->CastSpell(target, SPELL_SHADOWMOURNE_VISUAL_HIGH, true);
                    break;
                case 10:
                    target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_HIGH);
                    target->CastSpell(target, SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF, true);
                    break;
                default:
                    break;
            }
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_LOW);
            target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_HIGH);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_item_shadowmourne_AuraScript::OnStackChange, EFFECT_0, SPELL_AURA_MOD_STAT, AuraEffectHandleModes(AURA_EFFECT_HANDLE_REAL | AURA_EFFECT_HANDLE_REAPPLY));
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_shadowmourne_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STAT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_item_shadowmourne_AuraScript();
    }
};

enum AirRifleSpells
{
    SPELL_AIR_RIFLE_HOLD_VISUAL = 65582,
    SPELL_AIR_RIFLE_SHOOT       = 67532,
    SPELL_AIR_RIFLE_SHOOT_SELF  = 65577
};

class spell_item_red_rider_air_rifle: public SpellScriptLoader
{
    public:
        spell_item_red_rider_air_rifle() : SpellScriptLoader("spell_item_red_rider_air_rifle") { }

        class spell_item_red_rider_air_rifle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_red_rider_air_rifle_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AIR_RIFLE_HOLD_VISUAL) || !sSpellMgr->GetSpellInfo(SPELL_AIR_RIFLE_SHOOT) || !sSpellMgr->GetSpellInfo(SPELL_AIR_RIFLE_SHOOT_SELF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    caster->CastSpell(caster, SPELL_AIR_RIFLE_HOLD_VISUAL, true);
                    // needed because this spell shares GCD with its triggered spells (which must not be cast with triggered flag)
                    if (Player* player = caster->ToPlayer())
                        player->GetGlobalCooldownMgr().CancelGlobalCooldown(GetSpellInfo());
                    if (urand(0, 4))
                        caster->CastSpell(target, SPELL_AIR_RIFLE_SHOOT, false);
                    else
                        caster->CastSpell(caster, SPELL_AIR_RIFLE_SHOOT_SELF, false);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_red_rider_air_rifle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_red_rider_air_rifle_SpellScript();
        }
};

enum GenericData
{
    SPELL_ARCANITE_DRAGONLING           = 19804,
    SPELL_BATTLE_CHICKEN                = 13166,
    SPELL_MECHANICAL_DRAGONLING         = 4073,
    SPELL_MITHRIL_MECHANICAL_DRAGONLING = 12749
};

enum CreateHeartCandy
{
    ITEM_HEART_CANDY_1 = 21818,
    ITEM_HEART_CANDY_2 = 21817,
    ITEM_HEART_CANDY_3 = 21821,
    ITEM_HEART_CANDY_4 = 21819,
    ITEM_HEART_CANDY_5 = 21816,
    ITEM_HEART_CANDY_6 = 21823,
    ITEM_HEART_CANDY_7 = 21822,
    ITEM_HEART_CANDY_8 = 21820
};

class spell_item_create_heart_candy: public SpellScriptLoader
{
    public:
        spell_item_create_heart_candy() : SpellScriptLoader("spell_item_create_heart_candy") { }

        class spell_item_create_heart_candy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_create_heart_candy_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* target = GetHitPlayer())
                {
                    static const uint32 items[] = {ITEM_HEART_CANDY_1, ITEM_HEART_CANDY_2, ITEM_HEART_CANDY_3, ITEM_HEART_CANDY_4, ITEM_HEART_CANDY_5, ITEM_HEART_CANDY_6, ITEM_HEART_CANDY_7, ITEM_HEART_CANDY_8};
                    target->AddItem(items[urand(0, 7)], 1);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_create_heart_candy_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_create_heart_candy_SpellScript();
        }
};

// Item 45912 spell 64323
class spell_item_book_of_glyph_mastery: public SpellScriptLoader
{
    public:
        spell_item_book_of_glyph_mastery() : SpellScriptLoader("spell_item_book_of_glyph_mastery") {}

        class spell_item_book_of_glyph_mastery_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_book_of_glyph_mastery_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            SpellCastResult CheckRequirement()
            {
                if (HasDiscoveredAllSpells(GetSpellInfo()->Id, GetCaster()->ToPlayer()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_LEARNED_EVERYTHING);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_book_of_glyph_mastery_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_book_of_glyph_mastery_SpellScript();
        }
};

enum GiftOfTheHarvester
{
    NPC_GHOUL   = 28845,
    MAX_GHOULS  = 5
};

class spell_item_gift_of_the_harvester: public SpellScriptLoader
{
    public:
        spell_item_gift_of_the_harvester() : SpellScriptLoader("spell_item_gift_of_the_harvester") {}

        class spell_item_gift_of_the_harvester_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_gift_of_the_harvester_SpellScript);

            SpellCastResult CheckRequirement()
            {
                std::list<Creature*> ghouls;
                GetCaster()->GetAllMinionsByEntry(ghouls, NPC_GHOUL);
                if (ghouls.size() >= MAX_GHOULS)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_TOO_MANY_GHOULS);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_gift_of_the_harvester_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_gift_of_the_harvester_SpellScript();
        }
};

enum Sinkholes
{
    NPC_SOUTH_SINKHOLE      = 25664,
    NPC_NORTHEAST_SINKHOLE  = 25665,
    NPC_NORTHWEST_SINKHOLE  = 25666
};

class spell_item_map_of_the_geyser_fields: public SpellScriptLoader
{
    public:
        spell_item_map_of_the_geyser_fields() : SpellScriptLoader("spell_item_map_of_the_geyser_fields") {}

        class spell_item_map_of_the_geyser_fields_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_map_of_the_geyser_fields_SpellScript);

            SpellCastResult CheckSinkholes()
            {
                Unit* caster = GetCaster();
                if (caster->FindNearestCreature(NPC_SOUTH_SINKHOLE, 30.0f, true) ||
                    caster->FindNearestCreature(NPC_NORTHEAST_SINKHOLE, 30.0f, true) ||
                    caster->FindNearestCreature(NPC_NORTHWEST_SINKHOLE, 30.0f, true))
                    return SPELL_CAST_OK;

                SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_CLOSE_TO_SINKHOLE);
                return SPELL_FAILED_CUSTOM_ERROR;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_map_of_the_geyser_fields_SpellScript::CheckSinkholes);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_map_of_the_geyser_fields_SpellScript();
        }
};

enum VanquishedClutchesSpells
{
    SPELL_CRUSHER       = 64982,
    SPELL_CONSTRICTOR   = 64983,
    SPELL_CORRUPTOR     = 64984
};

class spell_item_vanquished_clutches: public SpellScriptLoader
{
    public:
        spell_item_vanquished_clutches() : SpellScriptLoader("spell_item_vanquished_clutches") { }

        class spell_item_vanquished_clutches_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_vanquished_clutches_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CRUSHER) || !sSpellMgr->GetSpellInfo(SPELL_CONSTRICTOR) || !sSpellMgr->GetSpellInfo(SPELL_CORRUPTOR))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId = RAND(SPELL_CRUSHER, SPELL_CONSTRICTOR, SPELL_CORRUPTOR);
                Unit* caster = GetCaster();
                caster->CastSpell(caster, spellId, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_vanquished_clutches_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_vanquished_clutches_SpellScript();
        }
};

enum MagicEater
{
    SPELL_WILD_MAGIC                             = 58891,
    SPELL_WELL_FED_1                             = 57288,
    SPELL_WELL_FED_2                             = 57139,
    SPELL_WELL_FED_3                             = 57111,
    SPELL_WELL_FED_4                             = 57286,
    SPELL_WELL_FED_5                             = 57291
};

class spell_magic_eater_food: public SpellScriptLoader
{
    public:
        spell_magic_eater_food() : SpellScriptLoader("spell_magic_eater_food") {}

        class spell_magic_eater_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_magic_eater_food_AuraScript);

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                Unit* target = GetTarget();
                switch (urand(0, 5))
                {
                    case 0:
                        target->CastSpell(target, SPELL_WILD_MAGIC, true);
                        break;
                    case 1:
                        target->CastSpell(target, SPELL_WELL_FED_1, true);
                        break;
                    case 2:
                        target->CastSpell(target, SPELL_WELL_FED_2, true);
                        break;
                    case 3:
                        target->CastSpell(target, SPELL_WELL_FED_3, true);
                        break;
                    case 4:
                        target->CastSpell(target, SPELL_WELL_FED_4, true);
                        break;
                    case 5:
                        target->CastSpell(target, SPELL_WELL_FED_5, true);
                        break;
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_magic_eater_food_AuraScript::HandleTriggerSpell, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_magic_eater_food_AuraScript();
        }
};

class spell_item_shimmering_vessel: public SpellScriptLoader
{
    public:
        spell_item_shimmering_vessel() : SpellScriptLoader("spell_item_shimmering_vessel") { }

        class spell_item_shimmering_vessel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_shimmering_vessel_SpellScript);

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Creature* target = GetHitCreature())
                    target->setDeathState(JUST_RESPAWNED);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_shimmering_vessel_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_shimmering_vessel_SpellScript();
        }
};

enum PurifyHelboarMeat
{
    SPELL_SUMMON_PURIFIED_HELBOAR_MEAT      = 29277,
    SPELL_SUMMON_TOXIC_HELBOAR_MEAT         = 29278,
    ITEM_PURIFIED_HELBOAR_MEAT              = 23248
};

class spell_item_purify_helboar_meat: public SpellScriptLoader
{
    public:
        spell_item_purify_helboar_meat() : SpellScriptLoader("spell_item_purify_helboar_meat") { }

        class spell_item_purify_helboar_meat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_purify_helboar_meat_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_PURIFIED_HELBOAR_MEAT) ||  !sSpellMgr->GetSpellInfo(SPELL_SUMMON_TOXIC_HELBOAR_MEAT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (roll_chance_i(50))
                    caster->CastSpell(caster, SPELL_SUMMON_TOXIC_HELBOAR_MEAT, true);
                else
                    if (caster->IsPlayer())
                        caster->ToPlayer()->AddItem(ITEM_PURIFIED_HELBOAR_MEAT, 1);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_purify_helboar_meat_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_purify_helboar_meat_SpellScript();
        }
};

enum CrystalPrison
{
    OBJECT_IMPRISONED_DOOMGUARD     = 179644
};

class spell_item_crystal_prison_dummy_dnd: public SpellScriptLoader
{
    public:
        spell_item_crystal_prison_dummy_dnd() : SpellScriptLoader("spell_item_crystal_prison_dummy_dnd") { }

        class spell_item_crystal_prison_dummy_dnd_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_crystal_prison_dummy_dnd_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sObjectMgr->GetGameObjectTemplate(OBJECT_IMPRISONED_DOOMGUARD))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Creature* target = GetHitCreature())
                    if (target->isDead() && !target->isPet())
                    {
                        GetCaster()->SummonGameObject(OBJECT_IMPRISONED_DOOMGUARD, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), 0, 0, 0, 0, uint32(target->GetRespawnTime()-time(NULL)));
                        target->DespawnOrUnsummon();
                    }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_crystal_prison_dummy_dnd_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_crystal_prison_dummy_dnd_SpellScript();
        }
};

enum ReindeerTransformation
{
    SPELL_FLYING_REINDEER_310                   = 44827,
    SPELL_FLYING_REINDEER_280                   = 44825,
    SPELL_FLYING_REINDEER_60                    = 44824,
    SPELL_REINDEER_100                          = 25859,
    SPELL_REINDEER_60                           = 25858
};

class spell_item_reindeer_transformation: public SpellScriptLoader
{
    public:
        spell_item_reindeer_transformation() : SpellScriptLoader("spell_item_reindeer_transformation") { }

        class spell_item_reindeer_transformation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_reindeer_transformation_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FLYING_REINDEER_310) || !sSpellMgr->GetSpellInfo(SPELL_FLYING_REINDEER_280)
                    || !sSpellMgr->GetSpellInfo(SPELL_FLYING_REINDEER_60) || !sSpellMgr->GetSpellInfo(SPELL_REINDEER_100)
                    || !sSpellMgr->GetSpellInfo(SPELL_REINDEER_60))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (caster->HasAuraType(SPELL_AURA_MOUNTED))
                {
                    float flyspeed = caster->GetSpeedRate(MOVE_FLIGHT);
                    float speed = caster->GetSpeedRate(MOVE_RUN);

                    caster->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    //5 different spells used depending on mounted speed and if mount can fly or not

                    if (flyspeed >= 4.1f)
                        // Flying Reindeer
                        caster->CastSpell(caster, SPELL_FLYING_REINDEER_310, true); //310% flying Reindeer
                    else if (flyspeed >= 3.8f)
                        // Flying Reindeer
                        caster->CastSpell(caster, SPELL_FLYING_REINDEER_280, true); //280% flying Reindeer
                    else if (flyspeed >= 1.6f)
                        // Flying Reindeer
                        caster->CastSpell(caster, SPELL_FLYING_REINDEER_60, true); //60% flying Reindeer
                    else if (speed >= 2.0f)
                        // Reindeer
                        caster->CastSpell(caster, SPELL_REINDEER_100, true); //100% ground Reindeer
                    else
                        // Reindeer
                        caster->CastSpell(caster, SPELL_REINDEER_60, true); //60% ground Reindeer
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_reindeer_transformation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_reindeer_transformation_SpellScript();
    }
};

enum NighInvulnerability
{
    SPELL_NIGH_INVULNERABILITY                  = 30456,
    SPELL_COMPLETE_VULNERABILITY                = 30457
};

class spell_item_nigh_invulnerability: public SpellScriptLoader
{
    public:
        spell_item_nigh_invulnerability() : SpellScriptLoader("spell_item_nigh_invulnerability") { }

        class spell_item_nigh_invulnerability_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_nigh_invulnerability_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NIGH_INVULNERABILITY) || !sSpellMgr->GetSpellInfo(SPELL_COMPLETE_VULNERABILITY))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (Item* castItem = GetCastItem())
                {
                    if (roll_chance_i(86))                  // Nigh-Invulnerability   - success
                        caster->CastSpell(caster, SPELL_NIGH_INVULNERABILITY, true, castItem);
                    else                                    // Complete Vulnerability - backfire in 14% casts
                        caster->CastSpell(caster, SPELL_COMPLETE_VULNERABILITY, true, castItem);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_nigh_invulnerability_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_nigh_invulnerability_SpellScript();
        }
};

enum Poultryzer
{
    SPELL_POULTRYIZER_SUCCESS    = 30501,
    SPELL_POULTRYIZER_BACKFIRE   = 30504
};

class spell_item_poultryizer: public SpellScriptLoader
{
    public:
        spell_item_poultryizer() : SpellScriptLoader("spell_item_poultryizer") { }

        class spell_item_poultryizer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_poultryizer_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_POULTRYIZER_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_POULTRYIZER_BACKFIRE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (GetCastItem() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), roll_chance_i(80) ? SPELL_POULTRYIZER_SUCCESS : SPELL_POULTRYIZER_BACKFIRE , true, GetCastItem());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_poultryizer_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_poultryizer_SpellScript();
        }
};

enum SocretharsStone
{
    SPELL_SOCRETHAR_TO_SEAT     = 35743,
    SPELL_SOCRETHAR_FROM_SEAT   = 35744
};

class spell_item_socrethars_stone: public SpellScriptLoader
{
    public:
        spell_item_socrethars_stone() : SpellScriptLoader("spell_item_socrethars_stone") { }

        class spell_item_socrethars_stone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_socrethars_stone_SpellScript);

            bool Load()
            {
                return (GetCaster()->GetAreaId() == 3900 || GetCaster()->GetAreaId() == 3742);
            }
            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SOCRETHAR_TO_SEAT) || !sSpellMgr->GetSpellInfo(SPELL_SOCRETHAR_FROM_SEAT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                switch (caster->GetAreaId())
                {
                    case 3900:
                        caster->CastSpell(caster, SPELL_SOCRETHAR_TO_SEAT, true);
                        break;
                    case 3742:
                        caster->CastSpell(caster, SPELL_SOCRETHAR_FROM_SEAT, true);
                        break;
                    default:
                        return;
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_socrethars_stone_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_socrethars_stone_SpellScript();
        }
};

enum DemonBroiledSurprise
{
    QUEST_SUPER_HOT_STEW                    = 11379,
    SPELL_CREATE_DEMON_BROILED_SURPRISE     = 43753,
    NPC_ABYSSAL_FLAMEBRINGER                = 19973
};

class spell_item_demon_broiled_surprise: public SpellScriptLoader
{
    public:
        spell_item_demon_broiled_surprise() : SpellScriptLoader("spell_item_demon_broiled_surprise") { }

        class spell_item_demon_broiled_surprise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_demon_broiled_surprise_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_DEMON_BROILED_SURPRISE) || !sObjectMgr->GetCreatureTemplate(NPC_ABYSSAL_FLAMEBRINGER) || !sObjectMgr->GetQuestTemplate(QUEST_SUPER_HOT_STEW))
                    return false;
                return true;
            }

            bool Load()
            {
               return GetCaster()->IsPlayer();
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* player = GetCaster();
                player->CastSpell(player, SPELL_CREATE_DEMON_BROILED_SURPRISE, false);
            }

            SpellCastResult CheckRequirement()
            {
                Player* player = GetCaster()->ToPlayer();
                if (player->GetQuestStatus(QUEST_SUPER_HOT_STEW) != QUEST_STATUS_INCOMPLETE)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (Creature* creature = player->FindNearestCreature(NPC_ABYSSAL_FLAMEBRINGER, 10, false))
                    if (creature->isDead())
                        return SPELL_CAST_OK;
                return SPELL_FAILED_NOT_HERE;
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_demon_broiled_surprise_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_item_demon_broiled_surprise_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_demon_broiled_surprise_SpellScript();
        }
};

enum CompleteRaptorCapture
{
    SPELL_RAPTOR_CAPTURE_CREDIT     = 42337
};

class spell_item_complete_raptor_capture: public SpellScriptLoader
{
    public:
        spell_item_complete_raptor_capture() : SpellScriptLoader("spell_item_complete_raptor_capture") { }

        class spell_item_complete_raptor_capture_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_complete_raptor_capture_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RAPTOR_CAPTURE_CREDIT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (GetHitCreature())
                {
                    GetHitCreature()->DespawnOrUnsummon();

                    //cast spell Raptor Capture Credit
                    caster->CastSpell(caster, SPELL_RAPTOR_CAPTURE_CREDIT, true, NULL);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_complete_raptor_capture_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_complete_raptor_capture_SpellScript();
        }
};

enum ImpaleLeviroth
{
    NPC_LEVIROTH                = 26452,
    SPELL_LEVIROTH_SELF_IMPALE  = 49882
};

class spell_item_impale_leviroth: public SpellScriptLoader
{
    public:
        spell_item_impale_leviroth() : SpellScriptLoader("spell_item_impale_leviroth") { }

        class spell_item_impale_leviroth_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_impale_leviroth_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sObjectMgr->GetCreatureTemplate(NPC_LEVIROTH))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Unit* target = GetHitCreature())
                    if (target->GetEntry() == NPC_LEVIROTH && !target->HealthBelowPct(95))
                    {
                        target->CastSpell(target, SPELL_LEVIROTH_SELF_IMPALE, true);
                        target->SetHealth(target->CountPctFromMaxHealth(10));
                    }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_impale_leviroth_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_impale_leviroth_SpellScript();
        }
};

enum BrewfestMountTransformation
{
    SPELL_MOUNT_RAM_100                         = 43900,
    SPELL_MOUNT_RAM_60                          = 43899,
    SPELL_MOUNT_KODO_100                        = 49379,
    SPELL_MOUNT_KODO_60                         = 49378,
    SPELL_BREWFEST_MOUNT_TRANSFORM              = 49357,
    SPELL_BREWFEST_MOUNT_TRANSFORM_REVERSE      = 52845
};

class spell_item_brewfest_mount_transformation: public SpellScriptLoader
{
    public:
        spell_item_brewfest_mount_transformation() : SpellScriptLoader("spell_item_brewfest_mount_transformation") { }

        class spell_item_brewfest_mount_transformation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_brewfest_mount_transformation_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MOUNT_RAM_100) || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_RAM_60) || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_KODO_100) || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_KODO_60))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (caster->HasAuraType(SPELL_AURA_MOUNTED))
                {
                    caster->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    uint32 spell_id;

                    switch (GetSpellInfo()->Id)
                    {
                        case SPELL_BREWFEST_MOUNT_TRANSFORM:
                            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                                spell_id = caster->GetTeam() == ALLIANCE ? SPELL_MOUNT_RAM_100 : SPELL_MOUNT_KODO_100;
                            else
                                spell_id = caster->GetTeam() == ALLIANCE ? SPELL_MOUNT_RAM_60 : SPELL_MOUNT_KODO_60;
                            break;
                        case SPELL_BREWFEST_MOUNT_TRANSFORM_REVERSE:
                            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                                spell_id = caster->GetTeam() == HORDE ? SPELL_MOUNT_RAM_100 : SPELL_MOUNT_KODO_100;
                            else
                                spell_id = caster->GetTeam() == HORDE ? SPELL_MOUNT_RAM_60 : SPELL_MOUNT_KODO_60;
                            break;
                        default:
                            return;
                    }
                    caster->CastSpell(caster, spell_id, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_brewfest_mount_transformation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_brewfest_mount_transformation_SpellScript();
        }
};

enum NitroBoots
{
    SPELL_NITRO_BOOTS_SUCCESS       = 54861,
    SPELL_NITRO_BOOTS_BACKFIRE      = 46014,
    SPELL_NITRO_BOOTS_FUEL_LEAK     = 94794
};

class spell_item_nitro_boots: public SpellScriptLoader
{
    public:
        spell_item_nitro_boots() : SpellScriptLoader("spell_item_nitro_boots") { }

        class spell_item_nitro_boots_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_nitro_boots_SpellScript);

            bool Load()
            {
                if (!GetCastItem())
                    return false;
                return true;
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NITRO_BOOTS_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_NITRO_BOOTS_BACKFIRE))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    uint32 l_SpellId = NitroBoots::SPELL_NITRO_BOOTS_SUCCESS;
                    uint8 l_Roll = urand(0, 99);

                    if (l_Roll < 5)
                        l_SpellId = NitroBoots::SPELL_NITRO_BOOTS_BACKFIRE;
                    else if (l_Roll < 10)
                        l_SpellId = NitroBoots::SPELL_NITRO_BOOTS_FUEL_LEAK;

                    l_Caster->CastSpell(l_Caster, l_SpellId, true, GetCastItem());
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_nitro_boots_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_nitro_boots_SpellScript();
        }
};

enum TeachLanguage
{
    SPELL_LEARN_GNOMISH_BINARY      = 50242,
    SPELL_LEARN_GOBLIN_BINARY       = 50246
};

class spell_item_teach_language: public SpellScriptLoader
{
    public:
        spell_item_teach_language() : SpellScriptLoader("spell_item_teach_language") { }

        class spell_item_teach_language_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_teach_language_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LEARN_GNOMISH_BINARY) || !sSpellMgr->GetSpellInfo(SPELL_LEARN_GOBLIN_BINARY))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();

                if (roll_chance_i(34))
                    caster->CastSpell(caster,caster->GetTeam() == ALLIANCE ? SPELL_LEARN_GNOMISH_BINARY : SPELL_LEARN_GOBLIN_BINARY, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_teach_language_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_teach_language_SpellScript();
        }
};

enum RocketBoots
{
    SPELL_ROCKET_BOOTS_PROC      = 30452
};

class spell_item_rocket_boots: public SpellScriptLoader
{
    public:
        spell_item_rocket_boots() : SpellScriptLoader("spell_item_rocket_boots") { }

        class spell_item_rocket_boots_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_rocket_boots_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ROCKET_BOOTS_PROC))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (!GetCaster() || !GetCaster()->ToPlayer())
                    return;

                Player* caster = GetCaster()->ToPlayer();
                if (Battleground* bg = caster->GetBattleground())
                    bg->EventPlayerDroppedFlag(caster);

                caster->RemoveSpellCooldown(SPELL_ROCKET_BOOTS_PROC);
                caster->CastSpell(caster, SPELL_ROCKET_BOOTS_PROC, true, NULL);
            }

            SpellCastResult CheckCast()
            {
                if (GetCaster()->IsInWater())
                    return SPELL_FAILED_ONLY_ABOVEWATER;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_rocket_boots_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_item_rocket_boots_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_rocket_boots_SpellScript();
        }
};

enum PygmyOil
{
    SPELL_PYGMY_OIL_PYGMY_AURA      = 53806,
    SPELL_PYGMY_OIL_SMALLER_AURA    = 53805
};

class spell_item_pygmy_oil: public SpellScriptLoader
{
    public:
        spell_item_pygmy_oil() : SpellScriptLoader("spell_item_pygmy_oil") { }

        class spell_item_pygmy_oil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_pygmy_oil_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PYGMY_OIL_PYGMY_AURA) || !sSpellMgr->GetSpellInfo(SPELL_PYGMY_OIL_SMALLER_AURA))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (Aura* aura = caster->GetAura(SPELL_PYGMY_OIL_PYGMY_AURA))
                    aura->RefreshDuration();
                else
                {
                    aura = caster->GetAura(SPELL_PYGMY_OIL_SMALLER_AURA);
                    if (!aura || aura->GetStackAmount() < 5 || !roll_chance_i(50))
                         caster->CastSpell(caster, SPELL_PYGMY_OIL_SMALLER_AURA, true);
                    else
                    {
                        aura->Remove();
                        caster->CastSpell(caster, SPELL_PYGMY_OIL_PYGMY_AURA, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_pygmy_oil_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_pygmy_oil_SpellScript();
        }
};

class spell_item_unusual_compass: public SpellScriptLoader
{
    public:
        spell_item_unusual_compass() : SpellScriptLoader("spell_item_unusual_compass") { }

        class spell_item_unusual_compass_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_unusual_compass_SpellScript);

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                caster->SetFacingTo(frand(0.0f, 62832.0f) / 10000.0f);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_unusual_compass_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_unusual_compass_SpellScript();
        }
};

enum ChickenCover
{
    SPELL_CHICKEN_NET               = 51959,
    SPELL_CAPTURE_CHICKEN_ESCAPE    = 51037,
    QUEST_CHICKEN_PARTY             = 12702,
    QUEST_FLOWN_THE_COOP            = 12532
};

class spell_item_chicken_cover: public SpellScriptLoader
{
    public:
        spell_item_chicken_cover() : SpellScriptLoader("spell_item_chicken_cover") { }

        class spell_item_chicken_cover_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_chicken_cover_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CHICKEN_NET) || !sSpellMgr->GetSpellInfo(SPELL_CAPTURE_CHICKEN_ESCAPE) || !sObjectMgr->GetQuestTemplate(QUEST_CHICKEN_PARTY) || !sObjectMgr->GetQuestTemplate(QUEST_FLOWN_THE_COOP))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Unit* target = GetHitUnit())
                {
                    if (!target->HasAura(SPELL_CHICKEN_NET) && (caster->GetQuestStatus(QUEST_CHICKEN_PARTY) == QUEST_STATUS_INCOMPLETE || caster->GetQuestStatus(QUEST_FLOWN_THE_COOP) == QUEST_STATUS_INCOMPLETE))
                    {
                        caster->CastSpell(caster, SPELL_CAPTURE_CHICKEN_ESCAPE, true);
                        target->Kill(target);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_chicken_cover_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_chicken_cover_SpellScript();
        }
};

enum Refocus
{
    SPELL_AIMED_SHOT    = 19434,
    SPELL_MULTISHOT     = 2643
};

class spell_item_refocus: public SpellScriptLoader
{
    public:
        spell_item_refocus() : SpellScriptLoader("spell_item_refocus") { }

        class spell_item_refocus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_refocus_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();

                if (!caster || caster->getClass() != CLASS_HUNTER)
                    return;

                if (caster->HasSpellCooldown(SPELL_AIMED_SHOT))
                    caster->RemoveSpellCooldown(SPELL_AIMED_SHOT, true);

                if (caster->HasSpellCooldown(SPELL_MULTISHOT))
                    caster->RemoveSpellCooldown(SPELL_MULTISHOT, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_refocus_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_refocus_SpellScript();
        }
};

class spell_item_muisek_vessel: public SpellScriptLoader
{
    public:
        spell_item_muisek_vessel() : SpellScriptLoader("spell_item_muisek_vessel") { }

        class spell_item_muisek_vessel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_muisek_vessel_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Creature* target = GetHitCreature())
                    if (target->isDead())
                        target->DespawnOrUnsummon();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_muisek_vessel_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_muisek_vessel_SpellScript();
        }
};

enum GreatmothersSoulcather
{
    SPELL_FORCE_CAST_SUMMON_GNOME_SOUL = 46486
};

class spell_item_greatmothers_soulcatcher: public SpellScriptLoader
{
public:
    spell_item_greatmothers_soulcatcher() : SpellScriptLoader("spell_item_greatmothers_soulcatcher") { }

    class spell_item_greatmothers_soulcatcher_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_greatmothers_soulcatcher_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (GetHitUnit())
                GetCaster()->CastSpell(GetCaster(),SPELL_FORCE_CAST_SUMMON_GNOME_SOUL);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_item_greatmothers_soulcatcher_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_greatmothers_soulcatcher_SpellScript();
    }
};

// Enohar Explosive Arrows - 78838
class spell_item_enohar_explosive_arrows: public SpellScriptLoader
{
    public:
        spell_item_enohar_explosive_arrows() : SpellScriptLoader("spell_item_enohar_explosive_arrows") { }

        class spell_item_enohar_explosive_arrows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_enohar_explosive_arrows_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (caster && target)
                    caster->DealDamage(target, target->GetHealth());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_enohar_explosive_arrows_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_enohar_explosive_arrows_SpellScript();
        }
};

enum HolyThurible
{
    NPC_WITHDRAWN_SOUL = 45166
};

class spell_item_holy_thurible: public SpellScriptLoader
{
    public:
        spell_item_holy_thurible() : SpellScriptLoader("spell_item_holy_thurible") { }

        class spell_item_holy_thurible_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_holy_thurible_SpellScript);

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();
                // GetHitCreature don't work
                Creature* target  = caster->FindNearestCreature(NPC_WITHDRAWN_SOUL, 2.0f, true);
                if (target && caster)
                {

                    if (roll_chance_i(50))
                    {
                        caster->KilledMonsterCredit(NPC_WITHDRAWN_SOUL, target->GetGUID());
                        target->DespawnOrUnsummon(0);
                    }
                    else
                        target->setFaction(FACTION_MONSTER);
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_holy_thurible_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_holy_thurible_SpellScript();
        }
};

enum bandage
{
    NPC_WOUNDED_DEFENDER = 38805
};

class spell_item_bandage_q24944: public SpellScriptLoader
{
    public:
        spell_item_bandage_q24944() : SpellScriptLoader("spell_item_bandage_q24944") { }

        class spell_item_bandage_q24944_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_bandage_q24944_SpellScript);

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();
                Creature* target  = GetHitCreature();
                if (target && caster && target->GetEntry() == NPC_WOUNDED_DEFENDER)
                {
                    target->SetFullHealth();
                    target->MonsterSay("Thank you my friend", LANG_UNIVERSAL, 0);
                    target->ForcedDespawn();
                    caster->KilledMonsterCredit(NPC_WOUNDED_DEFENDER, 0);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_bandage_q24944_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_bandage_q24944_SpellScript();
        }
};

class spell_item_gen_alchemy_mop: public SpellScriptLoader
{
    public:
        spell_item_gen_alchemy_mop() : SpellScriptLoader("spell_item_gen_alchemy_mop") { }

        class spell_item_gen_alchemy_mop_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_gen_alchemy_mop_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            void HandleOnHit()
            {
                uint8 chance = urand(1,5); // not official, todo: find the rate
                Player* caster = GetCaster()->ToPlayer();
                if (caster && GetCaster()->IsPlayer() && !HasDiscoveredAllSpells(114751, GetCaster()->ToPlayer()) && chance == 1)
                {
                    if (uint32 discoveredSpellId = GetExplicitDiscoverySpell(114751, caster->ToPlayer()))
                        caster->learnSpell(discoveredSpellId, false);
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_gen_alchemy_mop_SpellScript::HandleOnHit);
            }

        };
        SpellScript* GetSpellScript() const
        {
            return new spell_item_gen_alchemy_mop_SpellScript();
        }
};

class spell_alchemist_rejuvenation: public SpellScriptLoader
{
    public:
        spell_alchemist_rejuvenation() : SpellScriptLoader("spell_alchemist_rejuvenation") { }

        class spell_alchemist_rejuvenation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_alchemist_rejuvenation_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            void HandleOnHit()
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->getPowerType() == POWER_MANA)
                    {
                        switch (caster->getLevel())
                        {
                             case 85:
                                caster->EnergizeBySpell(caster, 105704, urand(8484, 9376), POWER_MANA);
                                break;
                            case 86:
                                caster->EnergizeBySpell(caster, 105704, urand(13651, 15087), POWER_MANA);
                                break;
                            case 87:
                                caster->EnergizeBySpell(caster, 105704, urand(16451, 18181), POWER_MANA);
                                break;
                            case 88:
                                caster->EnergizeBySpell(caster, 105704, urand(19818, 21902), POWER_MANA);
                                break;
                            case 89:
                                caster->EnergizeBySpell(caster, 105704, urand(23884, 26398), POWER_MANA);
                                break;
                            case 90:
                                caster->EnergizeBySpell(caster, 105704, urand(28500, 31500), POWER_MANA);
                                break;
                            default:
                                break;
                        }
                    }
                    switch (caster->getLevel())
                    {
                        case 85:
                            caster->HealBySpell(caster, sSpellMgr->GetSpellInfo(105704), urand(33935, 37505), false);
                            break;
                        case 86:
                            caster->HealBySpell(caster, sSpellMgr->GetSpellInfo(105704), urand(54601, 60347), false);
                            break;
                        case 87:
                            caster->HealBySpell(caster, sSpellMgr->GetSpellInfo(105704), urand(65801, 72727), false);
                            break;
                        case 88:
                            caster->HealBySpell(caster, sSpellMgr->GetSpellInfo(105704), urand(79268, 87610), false);
                            break;
                        case 89:
                            caster->HealBySpell(caster, sSpellMgr->GetSpellInfo(105704), urand(95534, 105590), false);
                            break;
                        case 90:
                            caster->HealBySpell(caster, sSpellMgr->GetSpellInfo(105704), urand(114001, 126001), false);
                            break;
                        default:
                            break;
                    }
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_alchemist_rejuvenation_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_alchemist_rejuvenation_SpellScript();
        }
};

#define SPELL_AMBER_PRISON 127266

class spell_item_amber_prison: public SpellScriptLoader
{
    public:
        spell_item_amber_prison() : SpellScriptLoader("spell_item_amber_prison") { }

        class spell_item_amber_prison_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_amber_prison_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AMBER_PRISON))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                if (Unit* target = GetExplTargetUnit())
                {
                    if (target->IsPlayer())
                        return SPELL_FAILED_TARGET_IS_PLAYER;
                    if (target->GetTypeId() == TYPEID_UNIT && target->getLevel() > 94)
                        return SPELL_FAILED_HIGHLEVEL;

                }
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_amber_prison_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_amber_prison_SpellScript();
        }
};

class spell_item_first_aid: public SpellScriptLoader
{
    public:
        spell_item_first_aid() : SpellScriptLoader("spell_item_first_aid") { }

        class spell_item_first_aid_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_first_aid_SpellScript);

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();

                if (Player* player = caster->ToPlayer())
                {
                    if (player->GetQuestStatus(30148) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (Creature* creature = target->ToCreature())
                        {
                            if (creature->GetEntry() == 58416)
                            {
                                player->KilledMonsterCredit(58417);
                                creature->DespawnOrUnsummon();
                            }
                        }
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_item_first_aid_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_first_aid_SpellScript();
        }
};

class spell_item_imputting_the_final_code: public SpellScriptLoader
{
    public:
        spell_item_imputting_the_final_code() : SpellScriptLoader("spell_item_imputting_the_final_code") { }

        class spell_item_imputting_the_final_code_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_imputting_the_final_code_SpellScript);

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();

                if (Player* player = caster->ToPlayer())
                {
                    if (player->GetQuestStatus(10447) == QUEST_STATUS_INCOMPLETE)
                    {
                        player->KilledMonsterCredit(21039);
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_item_imputting_the_final_code_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_imputting_the_final_code_SpellScript();
        }
};

class spell_item_pot_of_fire: public SpellScriptLoader
{
    public:
        spell_item_pot_of_fire() : SpellScriptLoader("spell_item_pot_of_fire") { }

        class spell_item_pot_of_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_pot_of_fire_SpellScript);

            void CorrectTarget(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> tempTargets = targets;
                for (auto itr : tempTargets)
                {
                    if (itr->GetEntry() != 61510)
                        targets.remove(itr);
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_pot_of_fire_SpellScript::CorrectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_pot_of_fire_SpellScript();
        }

        class spell_item_pot_of_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_pot_of_fire_AuraScript);

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                Unit* caster = GetCaster();

                if (!target || !caster)
                    return;

                if (Creature* creature = target->ToCreature())
                {
                    if (Player* player = caster->ToPlayer())
                    {
                        player->KilledMonsterCredit(61510);
                        creature->DespawnOrUnsummon();
                    }
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_pot_of_fire_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_pot_of_fire_AuraScript();
        }
};

class spell_item_dit_da_jow: public SpellScriptLoader
{
    public:
        spell_item_dit_da_jow() : SpellScriptLoader("spell_item_dit_da_jow") { }

        class spell_item_dit_da_jow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_dit_da_jow_SpellScript);

            void HandleBeforeCast()
            {
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();

                if (Player* player = caster->ToPlayer())
                {
                    if (player->GetQuestStatus(30460) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (Creature* creature = target->ToCreature())
                        {
                            if (creature->GetEntry() == 59143)
                            {
                                player->KilledMonsterCredit(59143);
                                creature->DespawnOrUnsummon();
                            }
                        }
                    }
                }
            }

            void Register()
            {
                BeforeCast += SpellCastFn(spell_item_dit_da_jow_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_dit_da_jow_SpellScript();
        }
};

class spell_item_zuluhed_chains: public SpellScriptLoader
{
    public:
        spell_item_zuluhed_chains() : SpellScriptLoader("spell_item_zuluhed_chains") { }

        class spell_item_zuluhed_chains_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_zuluhed_chains_SpellScript);

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();

                if (Player* player = caster->ToPlayer())
                {
                    if (player->GetQuestStatus(10866) == QUEST_STATUS_INCOMPLETE)
                    {
                        player->KilledMonsterCredit(22112);
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_item_zuluhed_chains_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_zuluhed_chains_SpellScript();
        }
};

class spell_item_yak_s_milk: public SpellScriptLoader
{
    public:
        spell_item_yak_s_milk() : SpellScriptLoader("spell_item_yak_s_milk") { }

        class spell_item_yak_s_milk_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_yak_s_milk_SpellScript);

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();

                if (Player* player = caster->ToPlayer())
                {
                    if (player->GetQuestStatus(30953) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (Creature* creature = target->ToCreature())
                        {
                            if (creature->GetEntry() == 61570)
                            {
                                player->KilledMonsterCredit(61569);
                                creature->DespawnOrUnsummon();
                            }
                        }
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_item_yak_s_milk_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_yak_s_milk_SpellScript();
        }
};

class spell_item_throw_mantra: public SpellScriptLoader
{
    public:
        spell_item_throw_mantra() : SpellScriptLoader("spell_item_throw_mantra") { }

        class spell_item_throw_mantra_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_throw_mantra_SpellScript);

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();

                if (Player* player = caster->ToPlayer())
                {
                    if (player->GetQuestStatus(30066) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (Creature* creature = target->ToCreature())
                        {
                            if (creature->GetEntry() == 58186 || creature->GetEntry() == 57400 || creature->GetEntry() == 57326)
                            {
                                player->KilledMonsterCredit(57705);
                                creature->DespawnOrUnsummon();
                            }
                        }
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_item_throw_mantra_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_throw_mantra_SpellScript();
        }
};

/// Ancient Knowledge - 127250
class spell_item_ancient_knowledge : public SpellScriptLoader
{
    public:
        spell_item_ancient_knowledge() : SpellScriptLoader("spell_item_ancient_knowledge") { }

        class spell_item_ancient_knowledge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_ancient_knowledge_SpellScript);

            SpellCastResult CheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->getLevel() > 84)
                        return SpellCastResult::SPELL_FAILED_HIGHLEVEL;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_ancient_knowledge_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_ancient_knowledge_SpellScript();
        }

        class spell_item_ancient_knowledge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_ancient_knowledge_AuraScript);

            void OnUpdate(uint32 /*p_Diff*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (l_Target->getLevel() >= 85)
                        Remove();
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_item_ancient_knowledge_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_ancient_knowledge_AuraScript();
        }
};

/// Sky Golem - 134359
/// Mechanized Lumber Extractor - 223814
class spell_item_sky_golem : public SpellScriptLoader
{
    public:
        spell_item_sky_golem() : SpellScriptLoader("spell_item_sky_golem") { }

        class spell_item_sky_golem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_sky_golem_AuraScript);

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Target->IsPlayer() && !l_Target->GetMap()->IsBattlegroundOrArena())
                        l_Target->ToPlayer()->ApplyModFlag(EPlayerFields::PLAYER_FIELD_LOCAL_FLAGS, PlayerLocalFlags::PLAYER_LOCAL_FLAG_CAN_USE_OBJECTS_MOUNTED, true);
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Target->IsPlayer())
                        l_Target->ToPlayer()->ApplyModFlag(EPlayerFields::PLAYER_FIELD_LOCAL_FLAGS, PlayerLocalFlags::PLAYER_LOCAL_FLAG_CAN_USE_OBJECTS_MOUNTED, false);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_item_sky_golem_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_sky_golem_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_sky_golem_AuraScript();
        }
};

enum eEngineeringScopesSpells
{
    SpellOglethorpesMissileSplitter = 156055,
    SpellMegawattFilament           = 156060,
    HemetsHeartseeker               = 173288
};

/// Called by Oglethorpe's Missile Splitter 156052, Megawatt Filament 156059, Hemet's Heartseeker 173286
class spell_item_engineering_scopes: public SpellScriptLoader
{
    public:
        spell_item_engineering_scopes(const char* p_Name, uint32 p_TriggeredSpellId) : SpellScriptLoader(p_Name), m_TriggeredSpellId(p_TriggeredSpellId) { }

        class spell_item_engineering_scopes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_engineering_scopes_AuraScript);

            public:
                spell_item_engineering_scopes_AuraScript(uint32 p_TriggeredSpellId) : AuraScript(), m_TriggeredSpellId(p_TriggeredSpellId) { }

                bool Validate(SpellInfo const* /*p_SpellEntry*/)
                {
                    if (!sSpellMgr->GetSpellInfo(m_TriggeredSpellId))
                        return false;
                    return true;
                }

                void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
                {
                    PreventDefaultAction();

                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    l_Caster->CastSpell(l_Caster, m_TriggeredSpellId, true);
                }

                void Register()
                {
                    OnEffectProc += AuraEffectProcFn(spell_item_engineering_scopes_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                }

            private:
                uint32 m_TriggeredSpellId;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_engineering_scopes_AuraScript(m_TriggeredSpellId);
        }

    private:
        uint32 m_TriggeredSpellId;
};

/// Summon Chauffeur (Horde) - 179244
/// Summon Chauffeur (Alliance) - 179245
class spell_item_summon_chauffeur : public SpellScriptLoader
{
    public:
        spell_item_summon_chauffeur() : SpellScriptLoader("spell_item_summon_chauffeur") { }

        class spell_item_summon_chauffeur_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_summon_chauffeur_AuraScript);

            enum eCreatures
            {
                HordeChauffeur      = 89713,
                AllianceChauffeur   = 89715
            };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    uint32 l_Entry = 0;
                    if (l_Target->GetTypeId() == TypeID::TYPEID_PLAYER)
                    {
                        if (l_Target->ToPlayer()->GetTeamId() == TeamId::TEAM_ALLIANCE)
                            l_Entry = eCreatures::AllianceChauffeur;
                        else
                            l_Entry = eCreatures::HordeChauffeur;
                    }

                    uint64 l_Guid = 0;
                    if (Creature* l_Chauffeur = l_Target->SummonCreature(l_Entry, *l_Target))
                    {
                        l_Guid = l_Chauffeur->GetGUID();
                        l_Target->SetPersonnalChauffeur(l_Guid);
                    }

                    l_Target->AddDelayedEvent([l_Target, l_Guid]() -> void
                    {
                        if (Creature* l_Chauffeur = sObjectAccessor->FindCreature(l_Guid))
                            l_Chauffeur->EnterVehicle(l_Target);
                    }, 10);
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (Creature* l_Chauffeur = sObjectAccessor->FindCreature(l_Target->GetPersonnalChauffeur()))
                    {
                        uint64 l_ChauffeurGUID = l_Chauffeur->GetGUID();
                        l_Chauffeur->GetMap()->AddTask([l_ChauffeurGUID]()->void
                        {
                            if (Creature* l_Chauffeur = sObjectAccessor->FindCreature(l_ChauffeurGUID))
                                l_Chauffeur->DespawnOrUnsummon();
                        });
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_summon_chauffeur_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_summon_chauffeur_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_summon_chauffeur_AuraScript();
        }
};

/// Forgemaster's Vigor - 177096
class spell_item_forgemasters_vigor : public SpellScriptLoader
{
    public:
        spell_item_forgemasters_vigor() : SpellScriptLoader("spell_item_forgemasters_vigor") { }

        class spell_item_forgemasters_vigor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_forgemasters_vigor_AuraScript);

            enum eSpell
            {
                HammerBlows = 177099
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->HasAura(eSpell::HammerBlows))
                    {
                        l_Target->CastSpell(l_Target, eSpell::HammerBlows, true);

                        if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpell::HammerBlows, EFFECT_0))
                            l_AuraEffect->ChangeAmount(p_AurEff->GetAmount());
                    }
                    else
                    {
                        if (Aura* l_Aura = l_Target->GetAura(eSpell::HammerBlows))
                            l_Aura->ModStackAmount(1);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpell::HammerBlows);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_forgemasters_vigor_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_forgemasters_vigor_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_forgemasters_vigor_AuraScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Called by Warlock Legendary 'Feretory of Souls' item 132456 / Spell Id = 205702
class spell_item_feretory_of_souls : public SpellScriptLoader
{
public:
    spell_item_feretory_of_souls() : SpellScriptLoader("spell_item_feretory_of_souls") { }

    class spell_item_feretory_of_souls_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_feretory_of_souls_AuraScript);

        enum eSpells
        {
            FierySoul           = 205704,
            RainOfFireProc      = 42223,
            Immolate            = 157736,
            ChannelDemonfireDmg = 196448,
        };

        bool HandleOnCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
            if (!l_SpellInfo || (l_SpellInfo->SchoolMask & SpellSchoolMask::SPELL_SCHOOL_MASK_FIRE) == 0)
                return false;

            switch (l_SpellInfo->Id)
            {
                case eSpells::Immolate:
                case eSpells::RainOfFireProc:
                case eSpells::ChannelDemonfireDmg:
                    return false;

                default:
                    return true;
            }
        }

        void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_FierySoulInfo = sSpellMgr->GetSpellInfo(eSpells::FierySoul);
            if (!l_Caster || !l_FierySoulInfo)
                return;

            uint8 l_Energize = l_FierySoulInfo->Effects[EFFECT_0].BasePoints;
            ///l_Caster->EnergizeBySpell(l_Caster, eSpells::FierySoul, l_Energize, Powers::POWER_SOUL_SHARDS);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_feretory_of_souls_AuraScript::HandleOnCheckProc);
            OnProc += AuraProcFn(spell_item_feretory_of_souls_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_feretory_of_souls_AuraScript();
    }
};

/// Hammer Blows - 177099
class spell_item_hammer_blows : public SpellScriptLoader
{
    public:
        spell_item_hammer_blows() : SpellScriptLoader("spell_item_hammer_blows") { }

        class spell_item_hammer_blows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_hammer_blows_AuraScript);

            enum eSpell
            {
                ForgemastersVigor = 177096
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpell::ForgemastersVigor, EFFECT_0))
                        p_Amount = l_AurEff->GetAmount();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_hammer_blows_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_hammer_blows_AuraScript();
        }
};

/// Detonation - 177067
class spell_item_detonation : public SpellScriptLoader
{
    public:
        spell_item_detonation() : SpellScriptLoader("spell_item_detonation") { }

        class spell_item_detonation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_detonation_AuraScript);

            enum eSpell
            {
                Detonating = 177070
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->HasAura(eSpell::Detonating))
                    {
                        l_Target->CastSpell(l_Target, eSpell::Detonating, true);

                        if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpell::Detonating, EFFECT_0))
                            l_AuraEffect->ChangeAmount(p_AurEff->GetAmount());
                    }
                    else
                    {
                        if (Aura* l_Aura = l_Target->GetAura(eSpell::Detonating))
                            l_Aura->ModStackAmount(1);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpell::Detonating);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_detonation_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_detonation_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_detonation_AuraScript();
        }
};

/// Detonating - 177070
class spell_item_detonating : public SpellScriptLoader
{
    public:
        spell_item_detonating() : SpellScriptLoader("spell_item_detonating") { }

        class spell_item_detonating_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_detonating_AuraScript);

            enum eSpell
            {
                Detonation = 177067
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpell::Detonation, EFFECT_0))
                        p_Amount = l_AurEff->GetAmount();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_detonating_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_detonating_AuraScript();
        }
};

/// Battering - 177102
class spell_item_battering : public SpellScriptLoader
{
    public:
        spell_item_battering() : SpellScriptLoader("spell_item_battering") { }

        class spell_item_battering_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_battering_AuraScript);

            enum eSpell
            {
                Cracks = 177103
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->HasAura(eSpell::Cracks))
                    {
                        l_Target->CastSpell(l_Target, eSpell::Cracks, true);

                        if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpell::Cracks, EFFECT_0))
                            l_AuraEffect->ChangeAmount(p_AurEff->GetAmount());
                    }
                    else
                    {
                        if (Aura* l_Aura = l_Target->GetAura(eSpell::Cracks))
                            l_Aura->ModStackAmount(1);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpell::Cracks);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_battering_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_battering_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_battering_AuraScript();
        }
};

/// Cracks! - 177103
class spell_item_cracks : public SpellScriptLoader
{
    public:
        spell_item_cracks() : SpellScriptLoader("spell_item_cracks") { }

        class spell_item_cracks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_cracks_AuraScript);

            enum eSpell
            {
                Battering = 177102
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpell::Battering, EFFECT_0))
                        p_Amount = l_AurEff->GetAmount();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_cracks_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_cracks_AuraScript();
        }
};

/// Sanitizing - 177086
class spell_item_sanatizing : public SpellScriptLoader
{
    public:
        spell_item_sanatizing() : SpellScriptLoader("spell_item_sanatizing") { }

        class spell_item_sanatizing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_sanatizing_AuraScript);

            enum eSpell
            {
                CleansingSteam = 177087
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->HasAura(eSpell::CleansingSteam))
                    {
                        l_Target->CastSpell(l_Target, eSpell::CleansingSteam, true);

                        if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpell::CleansingSteam, EFFECT_0))
                            l_AuraEffect->ChangeAmount(p_AurEff->GetAmount());
                    }
                    else
                    {
                        if (Aura* l_Aura = l_Target->GetAura(eSpell::CleansingSteam))
                            l_Aura->ModStackAmount(1);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpell::CleansingSteam);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_sanatizing_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_sanatizing_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_sanatizing_AuraScript();
        }
};

/// Cleansing Steam - 177087
class spell_item_cleansing_steam : public SpellScriptLoader
{
    public:
        spell_item_cleansing_steam() : SpellScriptLoader("spell_item_cleansing_steam") { }

        class spell_item_cleansing_steam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_cleansing_steam_AuraScript);

            enum eSpell
            {
                Sanitizing = 177086
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpell::Sanitizing, EFFECT_0))
                        p_Amount = l_AurEff->GetAmount();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_cleansing_steam_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_cleansing_steam_AuraScript();
        }
};

/// Molten Metal - 177081
class spell_item_molten_metal : public SpellScriptLoader
{
    public:
        spell_item_molten_metal() : SpellScriptLoader("spell_item_molten_metal") { }

        class spell_item_molten_metal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_molten_metal_AuraScript);

            enum eSpell
            {
                PouringSlag = 177083
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->HasAura(eSpell::PouringSlag))
                    {
                        l_Target->CastSpell(l_Target, eSpell::PouringSlag, true);

                        if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpell::PouringSlag, EFFECT_0))
                            l_AuraEffect->ChangeAmount(p_AurEff->GetAmount());
                    }
                    else
                    {
                        if (Aura* l_Aura = l_Target->GetAura(eSpell::PouringSlag))
                            l_Aura->ModStackAmount(1);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->RemoveAura(eSpell::PouringSlag);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_molten_metal_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_molten_metal_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_molten_metal_AuraScript();
        }
};

/// Pouring Slag - 177083
class spell_item_pouring_slag : public SpellScriptLoader
{
    public:
        spell_item_pouring_slag() : SpellScriptLoader("spell_item_pouring_slag") { }

        class spell_item_pouring_slag_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_pouring_slag_AuraScript);

            enum eSpell
            {
                MoltenMetal = 177081
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpell::MoltenMetal, EFFECT_0))
                        p_Amount = l_AurEff->GetAmount();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_pouring_slag_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_pouring_slag_AuraScript();
        }
};

/// Legs of Iron (T17) - 178210
/// Legs of Iron (Normal - T17 - Quest) - 178218
/// Legs of Iron (Heroic - T17 - Quest) - 178221
/// Legs of Iron (Mythic - T17 - Quest) - 178228
class spell_item_legs_of_iron : public SpellScriptLoader
{
    public:
        spell_item_legs_of_iron() : SpellScriptLoader("spell_item_legs_of_iron") { }

        class spell_item_legs_of_iron_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_legs_of_iron_SpellScript);

            enum eItemIDs
            {
                LegplatesOfGudingLight      = 115569,
                ShadowCouncilsLeggings      = 115587,
                SoulPriestsLeggings         = 115564,
                ArcanoshatterLeggings       = 115554,
                LivingWoodLegguards         = 115543,
                OgreskullBoneplateGreaves   = 115535,
                PoisonersLegguards          = 115573,
                BlackhandsLegguards         = 115580,
                LegwrapsOfTheSomberGaze     = 115557,
                RylakstalkersLegguards      = 115546,
                WindspeakersLegwraps        = 115575
            };

            enum eBonusIDs
            {
                /// For Token
                TokenHeroic = 570,
                TokenMythic = 569,
                /// For T17
                Heroic      = 566,
                Mythic      = 567
            };

            enum eSpellIDs
            {
                NormalQuestToken    = 178218,
                HeroicQuestToken    = 178221,
                MythicQuestToken    = 178228
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Item* l_Item = GetCastItem())
                    {
                        uint32 l_ItemID = 0;
                        std::vector<uint32> l_Bonuses;

                        switch (l_Player->getClass())
                        {
                            case Classes::CLASS_PALADIN:
                                l_ItemID = eItemIDs::LegplatesOfGudingLight;
                                break;
                            case Classes::CLASS_WARLOCK:
                                l_ItemID = eItemIDs::ShadowCouncilsLeggings;
                                break;
                            case Classes::CLASS_PRIEST:
                                l_ItemID = eItemIDs::SoulPriestsLeggings;
                                break;
                            case Classes::CLASS_MAGE:
                                l_ItemID = eItemIDs::ArcanoshatterLeggings;
                                break;
                            case Classes::CLASS_DRUID:
                                l_ItemID = eItemIDs::LivingWoodLegguards;
                                break;
                            case Classes::CLASS_DEATH_KNIGHT:
                                l_ItemID = eItemIDs::OgreskullBoneplateGreaves;
                                break;
                            case Classes::CLASS_ROGUE:
                                l_ItemID = eItemIDs::PoisonersLegguards;
                                break;
                            case Classes::CLASS_WARRIOR:
                                l_ItemID = eItemIDs::BlackhandsLegguards;
                                break;
                            case Classes::CLASS_MONK:
                                l_ItemID = eItemIDs::LegwrapsOfTheSomberGaze;
                                break;
                            case Classes::CLASS_HUNTER:
                                l_ItemID = eItemIDs::RylakstalkersLegguards;
                                break;
                            case Classes::CLASS_SHAMAN:
                                l_ItemID = eItemIDs::WindspeakersLegwraps;
                                break;
                            default:
                                break;
                        }

                        if (l_Item->HasItemBonus(eBonusIDs::TokenMythic))
                            l_Bonuses.push_back(eBonusIDs::Mythic);
                        else if (l_Item->HasItemBonus(eBonusIDs::TokenHeroic))
                            l_Bonuses.push_back(eBonusIDs::Heroic);

                        switch (GetSpellInfo()->Id)
                        {
                            case eSpellIDs::HeroicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Heroic);
                                break;
                            case eSpellIDs::MythicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Mythic);
                                break;
                            case eSpellIDs::NormalQuestToken:
                            default:
                                break;
                        }

                        if (l_ItemID)
                        {
                            /// Adding items
                            uint32 l_NoSpaceForCount = 0;
                            uint32 l_Count = 1;

                            /// check space and find places
                            ItemPosCountVec l_Dest;
                            InventoryResult l_MSG = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Dest, l_ItemID, l_Count, &l_NoSpaceForCount);

                            /// Convert to possible store amount
                            if (l_MSG != InventoryResult::EQUIP_ERR_OK)
                                l_Count -= l_NoSpaceForCount;

                            /// Can't add any
                            if (l_Count == 0 || l_Dest.empty())
                                return;

                            /// Context must be normal, difficulty bonus is already added
                            Item::GenerateItemBonus(l_ItemID, ItemContext::RaidNormal, l_Bonuses, false, l_Player);

                            if (Item* l_NewItem = l_Player->StoreNewItem(l_Dest, l_ItemID, true, 0, l_Bonuses))
                                l_Player->SendNewItem(l_NewItem, l_Count, false, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_legs_of_iron_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_legs_of_iron_SpellScript();
        }
};

/// Chest of Iron (T17) - 178209
/// Chest of Iron (Normal - T17 - Quest) - 178217
/// Chest of Iron (Heroic - T17 - Quest) - 178225
/// Chest of Iron (Mythic - T17 - Quest) - 178227
class spell_item_chest_of_iron : public SpellScriptLoader
{
    public:
        spell_item_chest_of_iron() : SpellScriptLoader("spell_item_chest_of_iron") { }

        class spell_item_chest_of_iron_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_chest_of_iron_SpellScript);

            enum eItemIDs
            {
                BattleplateOfGudingLight        = 115566,
                ShadowCouncilsRobes             = 115588,
                SoulPriestsRaiment              = 115560,
                ArcanoshatterRobes              = 115550,
                LivingWoodRaiment               = 115540,
                OgreskullBoneplateBreastplate   = 115537,
                PoisonersTunic                  = 115570,
                BlackhandsChestguard            = 115582,
                VestOfTheSomberGaze             = 115558,
                RylakstalkersTunic              = 115548,
                WindspeakersTunic               = 115577
            };

            enum eBonusIDs
            {
                /// For Token
                TokenHeroic = 570,
                TokenMythic = 569,
                /// For T17
                Heroic      = 566,
                Mythic      = 567
            };

            enum eSpellIDs
            {
                NormalQuestToken    = 178217,
                HeroicQuestToken    = 178225,
                MythicQuestToken    = 178227
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Item* l_Item = GetCastItem())
                    {
                        uint32 l_ItemID = 0;
                        std::vector<uint32> l_Bonuses;

                        switch (l_Player->getClass())
                        {
                            case Classes::CLASS_PALADIN:
                                l_ItemID = eItemIDs::BattleplateOfGudingLight;
                                break;
                            case Classes::CLASS_WARLOCK:
                                l_ItemID = eItemIDs::ShadowCouncilsRobes;
                                break;
                            case Classes::CLASS_PRIEST:
                                l_ItemID = eItemIDs::SoulPriestsRaiment;
                                break;
                            case Classes::CLASS_MAGE:
                                l_ItemID = eItemIDs::ArcanoshatterRobes;
                                break;
                            case Classes::CLASS_DRUID:
                                l_ItemID = eItemIDs::LivingWoodRaiment;
                                break;
                            case Classes::CLASS_DEATH_KNIGHT:
                                l_ItemID = eItemIDs::OgreskullBoneplateBreastplate;
                                break;
                            case Classes::CLASS_ROGUE:
                                l_ItemID = eItemIDs::PoisonersTunic;
                                break;
                            case Classes::CLASS_WARRIOR:
                                l_ItemID = eItemIDs::BlackhandsChestguard;
                                break;
                            case Classes::CLASS_MONK:
                                l_ItemID = eItemIDs::VestOfTheSomberGaze;
                                break;
                            case Classes::CLASS_HUNTER:
                                l_ItemID = eItemIDs::RylakstalkersTunic;
                                break;
                            case Classes::CLASS_SHAMAN:
                                l_ItemID = eItemIDs::WindspeakersTunic;
                                break;
                            default:
                                break;
                        }

                        if (l_Item->HasItemBonus(eBonusIDs::TokenMythic))
                            l_Bonuses.push_back(eBonusIDs::Mythic);
                        else if (l_Item->HasItemBonus(eBonusIDs::TokenHeroic))
                            l_Bonuses.push_back(eBonusIDs::Heroic);

                        switch (GetSpellInfo()->Id)
                        {
                            case eSpellIDs::HeroicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Heroic);
                                break;
                            case eSpellIDs::MythicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Mythic);
                                break;
                            case eSpellIDs::NormalQuestToken:
                            default:
                                break;
                        }

                        if (l_ItemID)
                        {
                            /// Adding items
                            uint32 l_NoSpaceForCount = 0;
                            uint32 l_Count = 1;

                            /// check space and find places
                            ItemPosCountVec l_Dest;
                            InventoryResult l_MSG = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Dest, l_ItemID, l_Count, &l_NoSpaceForCount);

                            /// Convert to possible store amount
                            if (l_MSG != InventoryResult::EQUIP_ERR_OK)
                                l_Count -= l_NoSpaceForCount;

                            /// Can't add any
                            if (l_Count == 0 || l_Dest.empty())
                                return;

                            /// Context must be normal, difficulty bonus is already added
                            Item::GenerateItemBonus(l_ItemID, ItemContext::RaidNormal, l_Bonuses, false, l_Player);

                            if (Item* l_NewItem = l_Player->StoreNewItem(l_Dest, l_ItemID, true, 0, l_Bonuses))
                                l_Player->SendNewItem(l_NewItem, l_Count, false, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_chest_of_iron_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_chest_of_iron_SpellScript();
        }
};

/// Helm of Iron (T17) - 178212
/// Helm of Iron (Normal - T17 - Quest) - 178216
/// Helm of Iron (Heroic - T17 - Quest) - 178224
/// Helm of Iron (Mythic - T17 - Quest) - 178226
class spell_item_helm_of_iron : public SpellScriptLoader
{
    public:
        spell_item_helm_of_iron() : SpellScriptLoader("spell_item_helm_of_iron") { }

        class spell_item_helm_of_iron_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_helm_of_iron_SpellScript);

            enum eItemIDs
            {
                HelmetOfGudingLight         = 115568,
                ShadowCouncilsHood          = 115586,
                SoulPriestsHood             = 115563,
                ArcanoshatterHood           = 115553,
                LivingWoodHeadpiece         = 115542,
                OgreskullBoneplateGreathelm = 115539,
                PoisonersHelmet             = 115572,
                BlackhandsFaceguard         = 115584,
                HelmOfTheSomberGaze         = 115556,
                RylakstalkersHeadguard      = 115545,
                WindspeakersFaceguard       = 115579
            };

            enum eBonusIDs
            {
                /// For Token
                TokenHeroic = 570,
                TokenMythic = 569,
                /// For T17
                Heroic      = 566,
                Mythic      = 567
            };

            enum eSpellIDs
            {
                NormalQuestToken    = 178216,
                HeroicQuestToken    = 178224,
                MythicQuestToken    = 178226
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Item* l_Item = GetCastItem())
                    {
                        uint32 l_ItemID = 0;
                        std::vector<uint32> l_Bonuses;

                        switch (l_Player->getClass())
                        {
                            case Classes::CLASS_PALADIN:
                                l_ItemID = eItemIDs::HelmetOfGudingLight;
                                break;
                            case Classes::CLASS_WARLOCK:
                                l_ItemID = eItemIDs::ShadowCouncilsHood;
                                break;
                            case Classes::CLASS_PRIEST:
                                l_ItemID = eItemIDs::SoulPriestsHood;
                                break;
                            case Classes::CLASS_MAGE:
                                l_ItemID = eItemIDs::ArcanoshatterHood;
                                break;
                            case Classes::CLASS_DRUID:
                                l_ItemID = eItemIDs::LivingWoodHeadpiece;
                                break;
                            case Classes::CLASS_DEATH_KNIGHT:
                                l_ItemID = eItemIDs::OgreskullBoneplateGreathelm;
                                break;
                            case Classes::CLASS_ROGUE:
                                l_ItemID = eItemIDs::PoisonersHelmet;
                                break;
                            case Classes::CLASS_WARRIOR:
                                l_ItemID = eItemIDs::BlackhandsFaceguard;
                                break;
                            case Classes::CLASS_MONK:
                                l_ItemID = eItemIDs::HelmOfTheSomberGaze;
                                break;
                            case Classes::CLASS_HUNTER:
                                l_ItemID = eItemIDs::RylakstalkersHeadguard;
                                break;
                            case Classes::CLASS_SHAMAN:
                                l_ItemID = eItemIDs::WindspeakersFaceguard;
                                break;
                            default:
                                break;
                        }

                        if (l_Item->HasItemBonus(eBonusIDs::TokenMythic))
                            l_Bonuses.push_back(eBonusIDs::Mythic);
                        else if (l_Item->HasItemBonus(eBonusIDs::TokenHeroic))
                            l_Bonuses.push_back(eBonusIDs::Heroic);

                        switch (GetSpellInfo()->Id)
                        {
                            case eSpellIDs::HeroicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Heroic);
                                break;
                            case eSpellIDs::MythicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Mythic);
                                break;
                            case eSpellIDs::NormalQuestToken:
                            default:
                                break;
                        }

                        if (l_ItemID)
                        {
                            /// Adding items
                            uint32 l_NoSpaceForCount = 0;
                            uint32 l_Count = 1;

                            /// check space and find places
                            ItemPosCountVec l_Dest;
                            InventoryResult l_MSG = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Dest, l_ItemID, l_Count, &l_NoSpaceForCount);

                            /// Convert to possible store amount
                            if (l_MSG != InventoryResult::EQUIP_ERR_OK)
                                l_Count -= l_NoSpaceForCount;

                            /// Can't add any
                            if (l_Count == 0 || l_Dest.empty())
                                return;

                            /// Context must be normal, difficulty bonus is already added
                            Item::GenerateItemBonus(l_ItemID, ItemContext::RaidNormal, l_Bonuses, false, l_Player);

                            if (Item* l_NewItem = l_Player->StoreNewItem(l_Dest, l_ItemID, true, 0, l_Bonuses))
                                l_Player->SendNewItem(l_NewItem, l_Count, false, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_helm_of_iron_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_helm_of_iron_SpellScript();
        }
};

/// Shoulders of Iron (T17) - 178213
/// Shoulders of Iron (Normal - T17 - Quest) - 178220
/// Shoulders of Iron (Heroic - T17 - Quest) - 178223
/// Shoulders of Iron (Mythic - T17 - Quest) - 178230
class spell_item_shoulders_of_iron : public SpellScriptLoader
{
    public:
        spell_item_shoulders_of_iron() : SpellScriptLoader("spell_item_shoulders_of_iron") { }

        class spell_item_shoulders_of_iron_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_shoulders_of_iron_SpellScript);

            enum eItemIDs
            {
                PauldronsOfGudingLight      = 115565,
                ShadowCouncilsMantle        = 115589,
                SoulPriestsShoulderguards   = 115561,
                ArcanoshatterMantle         = 115551,
                LivingWoodSpaulders         = 115544,
                OgreskullBoneplatePauldrons = 115536,
                PoisonersSpaulders          = 115574,
                BlackhandsShoulderguards    = 115581,
                MantleOfTheSomberGaze       = 115559,
                RylakstalkersSpaulders      = 115547,
                WindspeakersMantle          = 115576
            };

            enum eBonusIDs
            {
                /// For Token
                TokenHeroic = 570,
                TokenMythic = 569,
                /// For T17
                Heroic      = 566,
                Mythic      = 567
            };

            enum eSpellIDs
            {
                NormalQuestToken    = 178220,
                HeroicQuestToken    = 178223,
                MythicQuestToken    = 178230
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Item* l_Item = GetCastItem())
                    {
                        uint32 l_ItemID = 0;
                        std::vector<uint32> l_Bonuses;

                        switch (l_Player->getClass())
                        {
                            case Classes::CLASS_PALADIN:
                                l_ItemID = eItemIDs::PauldronsOfGudingLight;
                                break;
                            case Classes::CLASS_WARLOCK:
                                l_ItemID = eItemIDs::ShadowCouncilsMantle;
                                break;
                            case Classes::CLASS_PRIEST:
                                l_ItemID = eItemIDs::SoulPriestsShoulderguards;
                                break;
                            case Classes::CLASS_MAGE:
                                l_ItemID = eItemIDs::ArcanoshatterMantle;
                                break;
                            case Classes::CLASS_DRUID:
                                l_ItemID = eItemIDs::LivingWoodSpaulders;
                                break;
                            case Classes::CLASS_DEATH_KNIGHT:
                                l_ItemID = eItemIDs::OgreskullBoneplatePauldrons;
                                break;
                            case Classes::CLASS_ROGUE:
                                l_ItemID = eItemIDs::PoisonersSpaulders;
                                break;
                            case Classes::CLASS_WARRIOR:
                                l_ItemID = eItemIDs::BlackhandsShoulderguards;
                                break;
                            case Classes::CLASS_MONK:
                                l_ItemID = eItemIDs::MantleOfTheSomberGaze;
                                break;
                            case Classes::CLASS_HUNTER:
                                l_ItemID = eItemIDs::RylakstalkersSpaulders;
                                break;
                            case Classes::CLASS_SHAMAN:
                                l_ItemID = eItemIDs::WindspeakersMantle;
                                break;
                            default:
                                break;
                        }

                        if (l_Item->HasItemBonus(eBonusIDs::TokenMythic))
                            l_Bonuses.push_back(eBonusIDs::Mythic);
                        else if (l_Item->HasItemBonus(eBonusIDs::TokenHeroic))
                            l_Bonuses.push_back(eBonusIDs::Heroic);

                        switch (GetSpellInfo()->Id)
                        {
                            case eSpellIDs::HeroicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Heroic);
                                break;
                            case eSpellIDs::MythicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Mythic);
                                break;
                            case eSpellIDs::NormalQuestToken:
                            default:
                                break;
                        }

                        if (l_ItemID)
                        {
                            /// Adding items
                            uint32 l_NoSpaceForCount = 0;
                            uint32 l_Count = 1;

                            /// check space and find places
                            ItemPosCountVec l_Dest;
                            InventoryResult l_MSG = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Dest, l_ItemID, l_Count, &l_NoSpaceForCount);

                            /// Convert to possible store amount
                            if (l_MSG != InventoryResult::EQUIP_ERR_OK)
                                l_Count -= l_NoSpaceForCount;

                            /// Can't add any
                            if (l_Count == 0 || l_Dest.empty())
                                return;

                            /// Context must be normal, difficulty bonus is already added
                            Item::GenerateItemBonus(l_ItemID, ItemContext::RaidNormal, l_Bonuses, false, l_Player);

                            if (Item* l_NewItem = l_Player->StoreNewItem(l_Dest, l_ItemID, true, 0, l_Bonuses))
                                l_Player->SendNewItem(l_NewItem, l_Count, false, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_shoulders_of_iron_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_shoulders_of_iron_SpellScript();
        }
};

/// Gauntlets of Iron (T17) - 178211
/// Gauntlets of Iron (Normal - T17 - Quest) - 178219
/// Gauntlets of Iron (Heroic - T17 - Quest) - 178222
/// Gauntlets of Iron (Mythic - T17 - Quest) - 178229
class spell_item_gauntlets_of_iron : public SpellScriptLoader
{
    public:
        spell_item_gauntlets_of_iron() : SpellScriptLoader("spell_item_gauntlets_of_iron") { }

        class spell_item_gauntlets_of_iron_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_gauntlets_of_iron_SpellScript);

            enum eItemIDs
            {
                GauntletsOfGudingLight      = 115567,
                ShadowCouncilsGloves        = 115585,
                SoulPriestsGloves           = 115562,
                ArcanoshatterGloves         = 115552,
                LivingWoodGrips             = 115541,
                OgreskullBoneplateGauntlets = 115538,
                PoisonersGloves             = 115571,
                BlackhandsHandguards        = 115583,
                HandwrapsOfTheSomberGaze    = 115555,
                RylakstalkersGloves         = 115549,
                WindspeakersHandwraps       = 115578
            };

            enum eBonusIDs
            {
                /// For Token
                TokenHeroic = 570,
                TokenMythic = 569,
                /// For T17
                Heroic      = 566,
                Mythic      = 567
            };

            enum eSpellIDs
            {
                NormalQuestToken    = 178219,
                HeroicQuestToken    = 178222,
                MythicQuestToken    = 178229
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if (Item* l_Item = GetCastItem())
                    {
                        uint32 l_ItemID = 0;
                        std::vector<uint32> l_Bonuses;

                        switch (l_Player->getClass())
                        {
                            case Classes::CLASS_PALADIN:
                                l_ItemID = eItemIDs::GauntletsOfGudingLight;
                                break;
                            case Classes::CLASS_WARLOCK:
                                l_ItemID = eItemIDs::ShadowCouncilsGloves;
                                break;
                            case Classes::CLASS_PRIEST:
                                l_ItemID = eItemIDs::SoulPriestsGloves;
                                break;
                            case Classes::CLASS_MAGE:
                                l_ItemID = eItemIDs::ArcanoshatterGloves;
                                break;
                            case Classes::CLASS_DRUID:
                                l_ItemID = eItemIDs::LivingWoodGrips;
                                break;
                            case Classes::CLASS_DEATH_KNIGHT:
                                l_ItemID = eItemIDs::OgreskullBoneplateGauntlets;
                                break;
                            case Classes::CLASS_ROGUE:
                                l_ItemID = eItemIDs::PoisonersGloves;
                                break;
                            case Classes::CLASS_WARRIOR:
                                l_ItemID = eItemIDs::BlackhandsHandguards;
                                break;
                            case Classes::CLASS_MONK:
                                l_ItemID = eItemIDs::HandwrapsOfTheSomberGaze;
                                break;
                            case Classes::CLASS_HUNTER:
                                l_ItemID = eItemIDs::RylakstalkersGloves;
                                break;
                            case Classes::CLASS_SHAMAN:
                                l_ItemID = eItemIDs::WindspeakersHandwraps;
                                break;
                            default:
                                break;
                        }

                        if (l_Item->HasItemBonus(eBonusIDs::TokenMythic))
                            l_Bonuses.push_back(eBonusIDs::Mythic);
                        else if (l_Item->HasItemBonus(eBonusIDs::TokenHeroic))
                            l_Bonuses.push_back(eBonusIDs::Heroic);

                        switch (GetSpellInfo()->Id)
                        {
                            case eSpellIDs::HeroicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Heroic);
                                break;
                            case eSpellIDs::MythicQuestToken:
                                l_Bonuses.push_back(eBonusIDs::Mythic);
                                break;
                            case eSpellIDs::NormalQuestToken:
                            default:
                                break;
                        }

                        if (l_ItemID)
                        {
                            /// Adding items
                            uint32 l_NoSpaceForCount = 0;
                            uint32 l_Count = 1;

                            /// check space and find places
                            ItemPosCountVec l_Dest;
                            InventoryResult l_MSG = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Dest, l_ItemID, l_Count, &l_NoSpaceForCount);

                            /// Convert to possible store amount
                            if (l_MSG != InventoryResult::EQUIP_ERR_OK)
                                l_Count -= l_NoSpaceForCount;

                            /// Can't add any
                            if (l_Count == 0 || l_Dest.empty())
                                return;

                            /// Context must be normal, difficulty bonus is already added
                            Item::GenerateItemBonus(l_ItemID, ItemContext::RaidNormal, l_Bonuses, false, l_Player);

                            if (Item* l_NewItem = l_Player->StoreNewItem(l_Dest, l_ItemID, true, 0, l_Bonuses))
                                l_Player->SendNewItem(l_NewItem, l_Count, false, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_gauntlets_of_iron_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_gauntlets_of_iron_SpellScript();
        }
};

/// Create Ring - 168103
/// Create Glove - 168115
/// Create Weapon - 168700
/// Create Trinket - 168701
/// Create Belt - 168705
/// Create Shoulders - 168706
/// Create Legs - 168707
/// Create Helm - 168708
/// Create Glove - 168709
/// Create Boot - 168710
/// Create Bracer - 168712
/// Create Weapon - 168713
/// Create Trinket - 168714
/// Create Neck - 168716
/// Create Shoulders - 168719
/// Create Boot - 168723
/// Create Chest - 168724
/// Create Bracer - 168725
/// Create Weapon - 168726
/// Create Trinket - 168727
/// Create Neck - 168729
/// Create Shoulders - 168732
/// Create Boot - 168736
/// Create Chest - 168737
/// Create Bracer - 168738
/// Create Weapon - 168739
/// Create Trinket - 168740
/// Create Neck - 168742
/// Create Shoulders - 168745
/// Create Boot - 168749
/// Create Chest - 168750
/// Create Bracer - 168751
/// Create Trinket - 168753
/// Create Trinket - 181732
class spell_create_reward_item : public SpellScriptLoader
{
    public:
        spell_create_reward_item() : SpellScriptLoader("spell_create_reward_item") { }

        class spell_create_reward_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_reward_item_SpellScript);

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            uint32 m_ItemID;

            bool Load() override
            {
                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                LootStore& l_LootStore = LootTemplates_Spell;
                LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(GetSpellInfo()->Id);
                if (l_LootTemplate == nullptr)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::list<ItemTemplate const*> l_LootTable;
                std::vector<uint32> l_Items;
                l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, false);

                if (l_LootTable.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                for (ItemTemplate const* l_Template : l_LootTable)
                {
                    if (l_Template->HasSpec((SpecIndex)l_SpecID, l_Player->getLevel()))
                        l_Items.push_back(l_Template->ItemId);
                }

                if (l_Items.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::random_shuffle(l_Items.begin(), l_Items.end());

                m_ItemID = l_Items[0];

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                std::vector<uint32> l_BonusList;

                /// Some of created items have already secondary stats, but only with item level <= 519
                if (sObjectMgr->GetItemTemplate(m_ItemID)->ItemLevel > 519)
                {
                    Item::GenerateItemBonus(m_ItemID, ItemContext::RaidNormal, l_BonusList, false, l_Player);

                    std::vector<uint32> l_RandBonus = { eBonuses::Warforged, eBonuses::Avoidance, eBonuses::Indestructible, eBonuses::Leech, eBonuses::Speed, eBonuses::PrismaticSocket };
                    for (uint32 l_Bonus : l_RandBonus)
                    {
                        if (roll_chance_i(10))
                            l_BonusList.push_back(l_Bonus);
                    }
                }

                if (Item* l_Item = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_BonusList))
                {
                    l_Player->SendNewItem(l_Item, 1, false, true);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_reward_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_reward_item_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_reward_item_SpellScript();
        }
};

/// Ai-Li's Skymirror - 86589, Called by: 129803
class spell_item_skymirror_image : public SpellScriptLoader
{
public:
    spell_item_skymirror_image() : SpellScriptLoader("spell_item_skymirror_image") { }

    class spell_item_skymirror_image_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_skymirror_image_SpellScript);

        enum eSpells
        {
            SkymirrorImage = 127315,
        };

        void HandleDummy()
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (l_Player == nullptr)
                return;

            if (Unit* l_Target = l_Player->GetSelectedPlayer())
                l_Target->CastSpell(l_Player, eSpells::SkymirrorImage, true);
            else
                l_Player->CastSpell(l_Player, eSpells::SkymirrorImage);
        }

        SpellCastResult HandleCheckCast()
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return SpellCastResult::SPELL_FAILED_ERROR;

            Player* l_Player = l_Caster->ToPlayer();
            if (l_Player == nullptr)
                return SpellCastResult::SPELL_FAILED_ERROR;

            if (Unit* l_Target = l_Player->GetSelectedPlayer())
            {
                if (l_Player->IsPvP() && !l_Target->IsPvP())
                    return SpellCastResult::SPELL_FAILED_PVP_TARGET_WHILE_UNFLAGGED;
            }

            return SpellCastResult::SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_item_skymirror_image_SpellScript::HandleCheckCast);
            OnHit += SpellHitFn(spell_item_skymirror_image_SpellScript::HandleDummy);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_skymirror_image_SpellScript();
    }
};

/// Memory of Mr. Smite - 127207. Item - Mr. Smite's Brass Compass - 86568
class spell_item_memory_of_mr_smite : public SpellScriptLoader
{
    public:
        spell_item_memory_of_mr_smite() : SpellScriptLoader("spell_item_memory_of_mr_smite") { }

        class spell_item_memory_of_mr_smite_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_memory_of_mr_smite_AuraScript);

            enum eSpells
            {
                MrSmiteSword = 127196,
                MrSmiteHummer = 127206,
                MrSmiteBlade = 127205
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                uint32 l_MrSmiteId;
                l_MrSmiteId = (urand(0, 2));
                switch (l_MrSmiteId)
                {
                    case 0:
                        l_MrSmiteId = eSpells::MrSmiteSword;
                        break;
                    case 1:
                        l_MrSmiteId = eSpells::MrSmiteHummer;
                     break;
                    case 2:
                        l_MrSmiteId = eSpells::MrSmiteBlade;
                        break;
                    default:
                        return;
                }

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, l_MrSmiteId, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->HasAura(eSpells::MrSmiteSword))
                    l_Caster->RemoveAura(eSpells::MrSmiteSword);
                else if (l_Caster->HasAura(eSpells::MrSmiteHummer))
                    l_Caster->RemoveAura(eSpells::MrSmiteHummer);
                else if (l_Caster->HasAura(eSpells::MrSmiteBlade))
                    l_Caster->RemoveAura(eSpells::MrSmiteBlade);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_memory_of_mr_smite_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_memory_of_mr_smite_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_memory_of_mr_smite_AuraScript();
        }
};

/// Celestial Defender's Medallion - 103685, Called by: 149228
class spell_item_celestial_defender : public SpellScriptLoader
{
    public:
        spell_item_celestial_defender() : SpellScriptLoader("spell_item_celestial_defender") { }

        class spell_item_celestial_defender_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_celestial_defender_SpellScript);

            enum eSpells
            {
                CelestialDefenderMale = 148369,
                CelestialDefenderFemale = 149229
            };

            void HandleDummy(SpellEffIndex /* index */)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                if (l_Caster->getGender() == GENDER_MALE)
                    l_Caster->CastSpell(l_Caster, eSpells::CelestialDefenderMale, true);
                else if (l_Caster->getGender() == GENDER_FEMALE)
                    l_Caster->CastSpell(l_Caster, eSpells::CelestialDefenderFemale, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_celestial_defender_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_celestial_defender_SpellScript();
        }
};

/// Curious Bronze Timepiece - 104113, Called by: 147988
class spell_item_curious_bronze_timepiece_alliance : public SpellScriptLoader
{
    public:
        spell_item_curious_bronze_timepiece_alliance() : SpellScriptLoader("spell_item_curious_bronze_timepiece_alliance") { }

        class spell_item_curious_bronze_timepiece_alliance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_curious_bronze_timepiece_alliance_SpellScript);

            void HandleDummy()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                l_Player->TeleportTo(870, -906.980957f, -4643.734375f, 5.223629f, 2.934408f);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_curious_bronze_timepiece_alliance_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_curious_bronze_timepiece_alliance_SpellScript();
        }
};

/// Curious Bronze Timepiece - 104110, Called by: 147985
class spell_item_curious_bronze_timepiece_horde : public SpellScriptLoader
{
    public:
        spell_item_curious_bronze_timepiece_horde() : SpellScriptLoader("spell_item_curious_bronze_timepiece_horde") { }

        class spell_item_curious_bronze_timepiece_horde_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_curious_bronze_timepiece_horde_SpellScript);

            void HandleDummy()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                l_Player->TeleportTo(870, -420.822906f, -4604.593262f, 3.901290f, 5.138229f);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_curious_bronze_timepiece_horde_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_curious_bronze_timepiece_horde_SpellScript();
        }
};

/// Bladespire Relic - 118662, Called by: 175604
class spell_item_ascend_to_bladespire : public SpellScriptLoader
{
public:
    spell_item_ascend_to_bladespire() : SpellScriptLoader("spell_item_ascend_to_bladespire") { }

    class spell_item_ascend_to_bladespire_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_ascend_to_bladespire_SpellScript);

        void HandleDummy()
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (l_Player == nullptr)
                return;

            l_Player->TeleportTo(1116, 6802.729004f, 5861.151855f, 258.688385f, 5.067857f);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_item_ascend_to_bladespire_SpellScript::HandleDummy);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_ascend_to_bladespire_SpellScript();
    }
};

/// Faded Wizard Hat - 53057, Called by: 74589
class spell_item_faded_wizard_hat : public SpellScriptLoader
{
public:
    spell_item_faded_wizard_hat() : SpellScriptLoader("spell_item_faded_wizard_hat") { }

    class spell_item_faded_wizard_hat_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_faded_wizard_hat_AuraScript);

        enum eSpells
        {
            GoblinMaleOne = 30089,
            GoblinMaleTwo = 30096,
            GoblinFemaleOne = 30084,
            GoblinFemaleTwo = 29907,
            BloodElfMaleOne = 19840,
            BloodElfMaleTwo = 30085,
            BloodElfMaleThree = 30086,
            BloodElfFemale = 29909,
            MaleUndead = 30094,
            FemaleUndead = 30093,
            MaleOrc = 29908,
            FemaleTroll = 30088,
            MaleHuman = 7614,
            FemaleHuman = 30092,
            MaleGnome = 30095,
            FemaleNightElf = 11670,
        };

        void OnApply(AuraEffect const*, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            AuraEffect* l_AuraEffect = GetEffect(0);
            if (l_AuraEffect == nullptr)
                return;

            uint32 l_BaseDisplayId = l_AuraEffect->GetAmount();
            if (l_BaseDisplayId != 0)
                l_Caster->SetDisplayId(l_BaseDisplayId);
            else
            {
                uint32 l_DisplayId;
                l_DisplayId = (urand(0, 15));
                switch (l_DisplayId)
                {
                    case 0:  l_DisplayId = eSpells::GoblinMaleOne;
                        break;
                    case 1:  l_DisplayId = eSpells::GoblinMaleTwo;
                        break;
                    case 2:  l_DisplayId = eSpells::GoblinFemaleOne;
                        break;
                    case 3:  l_DisplayId = eSpells::GoblinFemaleTwo;
                        break;
                    case 4:  l_DisplayId = eSpells::BloodElfMaleOne;
                        break;
                    case 5:  l_DisplayId = eSpells::BloodElfMaleTwo;
                        break;
                    case 6:  l_DisplayId = eSpells::BloodElfMaleThree;
                        break;
                    case 7:  l_DisplayId = eSpells::BloodElfFemale;
                        break;
                    case 8:  l_DisplayId = eSpells::MaleUndead;
                        break;
                    case 9:  l_DisplayId = eSpells::FemaleUndead;
                        break;
                    case 10:  l_DisplayId = eSpells::MaleOrc;
                        break;
                    case 11:  l_DisplayId = eSpells::FemaleTroll;
                        break;
                    case 12:  l_DisplayId = eSpells::MaleHuman;
                        break;
                    case 13:  l_DisplayId = eSpells::FemaleHuman;
                        break;
                    case 14:  l_DisplayId = eSpells::MaleGnome;
                        break;
                    case 15:  l_DisplayId = eSpells::FemaleNightElf;
                        break;
                    default:
                        return;
                }

                if (!l_DisplayId)
                    return;

                l_AuraEffect->SetAmount(l_DisplayId);
                l_Caster->SetDisplayId(l_AuraEffect->GetAmount());
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_item_faded_wizard_hat_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_item_faded_wizard_hat_AuraScript();
    }
};

/// Reflecting Prism - 163219
class spell_item_reflecting_prism : public SpellScriptLoader
{
    public:
        spell_item_reflecting_prism() : SpellScriptLoader("spell_item_reflecting_prism") { }

        class spell_item_reflecting_prism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_reflecting_prism_SpellScript);

            enum eSpells
            {
                PrismaticReflection = 163267
            };

            void HandleDummy()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                if (Unit* l_Target = l_Player->GetSelectedPlayer())
                    l_Target->CastSpell(l_Player, eSpells::PrismaticReflection, true, nullptr, nullptr, l_Player->GetGUID());
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_reflecting_prism_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_reflecting_prism_SpellScript();
        }
};

/// Relic of Karabor - 118663, Called by: 175608
class spell_item_ascend_to_karabor : public SpellScriptLoader
{
public:
    spell_item_ascend_to_karabor() : SpellScriptLoader("spell_item_ascend_to_karabor") { }

    class spell_item_ascend_to_karabor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_ascend_to_karabor_SpellScript);

        void HandleDummy()
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (l_Player == nullptr)
                return;

            l_Player->TeleportTo(1191, 578.738037f, -2476.123779f, 96.248749f, 4.711049f);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_item_ascend_to_karabor_SpellScript::HandleDummy);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_ascend_to_karabor_SpellScript();
    }
};

/// Sargerei Disguise - 119134, Called by: 176567
class spell_item_sargerei_disguise : public SpellScriptLoader
{
public:
    spell_item_sargerei_disguise() : SpellScriptLoader("spell_item_sargerei_disguise") { }

    class spell_item_sargerei_disguise_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_sargerei_disguise_SpellScript);

        enum eSpells
        {
            SargereiDisguise = 176568
        };

        void AfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::SargereiDisguise, true);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_item_sargerei_disguise_SpellScript::AfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_sargerei_disguise_SpellScript();
    }
};

/// Swapblaster - 111820, Called by: 161399
class spell_item_swapblaster : public SpellScriptLoader
{
    public:
        spell_item_swapblaster() : SpellScriptLoader("spell_item_swapblaster") { }

        class spell_item_swapblaster_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_swapblaster_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = l_Caster->ToPlayer();
                Player* l_Target = l_Player->GetSelectedPlayer();
                if (!l_Caster || !l_Player || !l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Player->isInCombat() || l_Target->isInCombat())
                    return SPELL_FAILED_TARGET_IN_COMBAT;

                if (!l_Target->IsInSameGroupWith(l_Player) || !l_Target->IsInSameRaidWith(l_Player))
                    return SPELL_FAILED_TARGET_NOT_IN_PARTY;

                return SPELL_CAST_OK;
            }

            void HandleDummy()
            {
                Unit* l_Caster = GetCaster();
                Player* l_Player = l_Caster->ToPlayer();
                Player* l_Target = l_Player->GetSelectedPlayer();
                if (!l_Caster || !l_Player || !l_Target)
                    return;

                l_Player->TeleportTo(l_Target->GetMapId(), l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ(), l_Target->GetOrientation());
                l_Target->TeleportTo(l_Player->GetMapId(), l_Player->GetPositionX(), l_Player->GetPositionY(), l_Player->GetPositionZ(), l_Player->GetOrientation());
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_swapblaster_SpellScript::CheckCast);
                OnHit += SpellHitFn(spell_item_swapblaster_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_swapblaster_SpellScript();
        }
};

/// Steel Claws - 122157, Called by: 83134
class spell_item_bronze_claws : public SpellScriptLoader
{
    public:
        spell_item_bronze_claws() : SpellScriptLoader("spell_item_bronze_claws") { }

        class spell_item_bronze_claws_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_bronze_claws_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->GetAreaId() != 6307) ///< Should be castable only in Dao Shanze Isle
                    return SPELL_FAILED_INCORRECT_AREA;

                if (l_Target->IsPlayer())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_bronze_claws_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_bronze_claws_SpellScript();
        }
};

/// Summon Tonga's Guide - 65751, Called by: 46782
class spell_item_tonga_totem : public SpellScriptLoader
{
    public:
        spell_item_tonga_totem() : SpellScriptLoader("spell_item_tonga_totem") { }

        class spell_item_tonga_totem_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_tonga_totem_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->GetAreaId() != 17) ///< Should be castable only in Northern Barrens
                    return SPELL_FAILED_INCORRECT_AREA;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_tonga_totem_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_tonga_totem_SpellScript();
        }
};

/// Call Withered Ent - 64306, Called by: 45911
class spell_item_petrified_root : public SpellScriptLoader
{
    public:
        spell_item_petrified_root() : SpellScriptLoader("spell_item_petrified_root") { }

        class spell_item_petrified_root_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_petrified_root_SpellScript);

            enum eNpc
            {
                DarkscaleScout = 33206
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Creature->GetEntry() != eNpc::DarkscaleScout && !l_Creature->isDead())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_petrified_root_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_petrified_root_SpellScript();
        }
};

/// Transmorphic Tincture - 112090, Called by: 162313
class spell_item_transmorphic_tincture : public SpellScriptLoader
{
    public:
    spell_item_transmorphic_tincture() : SpellScriptLoader("spell_item_transmorphic_tincture") { }

    class  spell_item_transmorphic_tincture_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_transmorphic_tincture_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* l_Player = GetTarget()->ToPlayer();
            if (l_Player == nullptr)
                return;

            AuraEffect* l_AuraEffect = GetEffect(0);
            if (l_AuraEffect == nullptr)
                return;

            Gender l_NewGender = (Gender)l_Player->getGender();

            /// This need to understand if player already have this buff (for example after logout), to prevent change of the gender one more time
            uint32 l_NativeGender = l_AuraEffect->GetAmount();
            if (l_NativeGender == 0)
                l_AuraEffect->SetAmount(l_NewGender + 1);

            uint32 l_OtherGender = l_AuraEffect->GetAmount() - 1;
            if (l_OtherGender == GENDER_MALE)            ///< MALE
                l_NewGender = GENDER_FEMALE;
            else if (l_OtherGender == GENDER_FEMALE)     ///< FEMALE
                l_NewGender = GENDER_MALE;
            else
                return;

            /// Set new gender
            l_Player->SetGender(l_NewGender);
            l_Player->SetByteValue(PLAYER_FIELD_ARENA_FACTION, 0, l_NewGender);

            PlayerInfo const* l_Info = sObjectMgr->GetPlayerInfo(l_Player->getRace(), l_Player->getClass());
            if (!l_Info)
                return;
            uint16 l_NewDisplayId = 0;

            if (l_NewGender == GENDER_MALE)
                l_NewDisplayId = l_Info->displayId_m;
            else if (l_NewGender == GENDER_FEMALE)
                l_NewDisplayId = l_Info->displayId_f;

            /// Check if this is first time, our player already has this aura
            /// If aura has applied less then 3 seconds ago - just casted
            bool l_FirstTime = false;
            if (Aura* l_Transmorphed = l_Player->GetAura(GetSpellInfo()->Id))
                if (l_Transmorphed->GetDuration() > (l_Transmorphed->GetMaxDuration() - 500))
                    l_FirstTime = true;

            /// Check if at the moment player has the same model as his native
            bool l_CurrentModelEqualNative = l_Player->GetNativeDisplayId() == l_Player->GetDisplayId();

            /// If not in original form (for example cat,bear,metamorphosis), we don't need to change displayId now
            if ((!l_FirstTime && !l_CurrentModelEqualNative) || l_CurrentModelEqualNative)
                l_Player->InitDisplayIds();
            else
                l_Player->SetNativeDisplayId(l_NewDisplayId);
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* l_Player = GetTarget()->ToPlayer();
            if (l_Player == nullptr)
                return;

            AuraEffect* l_AuraEffect = GetEffect(0);
            if (l_AuraEffect == nullptr)
                return;

            uint32 l_NativeGender = l_AuraEffect->GetAmount() - 1;

            l_Player->SetGender(l_NativeGender);
            l_Player->SetByteValue(PLAYER_FIELD_ARENA_FACTION, 0, l_NativeGender);


            PlayerInfo const* l_Info = sObjectMgr->GetPlayerInfo(l_Player->getRace(), l_Player->getClass());
            if (!l_Info)
                return;
            uint16 l_NativeDisplayId = 0;

            if (l_NativeGender == GENDER_MALE)
                l_NativeDisplayId = l_Info->displayId_m;
            else if (l_NativeGender == GENDER_FEMALE)
                l_NativeDisplayId = l_Info->displayId_f;

            /// If not in original form (for example cat,bear,metamorphosis), we don't need to change displayId now
            if (l_Player->GetNativeDisplayId() == l_Player->GetDisplayId())
                l_Player->InitDisplayIds();
            else
                l_Player->SetNativeDisplayId(l_NativeDisplayId);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_item_transmorphic_tincture_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AuraEffectHandleModes(AURA_EFFECT_HANDLE_REAL | AURA_EFFECT_HANDLE_SEND_FOR_CLIENT));
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_transmorphic_tincture_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_item_transmorphic_tincture_AuraScript();
    }
};

/// Hypnosis Goggles - 113631, Called by: 167839
class spell_item_hypnotize_critter : public SpellScriptLoader
{
    public:
        spell_item_hypnotize_critter() : SpellScriptLoader("spell_item_hypnotize_critter") { }

        class spell_item_hypnotize_critter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_hypnotize_critter_SpellScript);

            void HandleDummy()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                Unit* l_Critter = GetHitUnit();

                if (l_Player == nullptr || l_Critter == nullptr)
                    return;

                if (l_Critter->GetMotionMaster())
                    l_Critter->GetMotionMaster()->MoveFollow(l_Player, 0.0f, PET_FOLLOW_ANGLE, MOTION_SLOT_IDLE);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_hypnotize_critter_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_hypnotize_critter_SpellScript();
        }
};

class spell_item_ghostly_backfire : public SpellScriptLoader
{
    public:
        spell_item_ghostly_backfire() : SpellScriptLoader("spell_item_ghostly_backfire") {}

        class spell_item_ghostly_backfire_SpellScript : public SpellScript
        {
            enum Creatures
            {
                NPC_DOZER = 35558
            };

            PrepareSpellScript(spell_item_ghostly_backfire_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (GetCaster()->FindNearestCreature(NPC_DOZER, 20.0f) != nullptr)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_USE_THAT_ITEM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_ghostly_backfire_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_ghostly_backfire_SpellScript();
        }
};

/// Blessing of A'dal 175442
class spell_item_blessing_of_adal : public SpellScriptLoader
{
    public:
        spell_item_blessing_of_adal() : SpellScriptLoader("spell_item_blessing_of_adal") { }

        class spell_item_blessing_of_adal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_blessing_of_adal_AuraScript);

            void OnUpdate(uint32 /*p_Diff*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (l_Target->GetMapId() != 1116)
                        Remove();
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_item_blessing_of_adal_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_blessing_of_adal_AuraScript();
        }
};

/// Enduring Elixir of Wisdom - 146939 (item 103557)
class spell_item_enduring_elixir_of_wisdom : public SpellScriptLoader
{
    public:
        spell_item_enduring_elixir_of_wisdom() : SpellScriptLoader("spell_item_enduring_elixir_of_wisdom") { }

        class spell_item_enduring_elixir_of_wisdom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_enduring_elixir_of_wisdom_SpellScript);

            enum eSpells
            {
                EnduringElixirOfWisdomBonus = 146929
            };

            void AfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::EnduringElixirOfWisdomBonus, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_enduring_elixir_of_wisdom_SpellScript::AfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_enduring_elixir_of_wisdom_SpellScript();
        }
};

/// Spike-Toed Booterang - 129295
/// Called by Spike-Toed Booterang - 193832
class spell_item_spike_toed_booterang : public SpellScriptLoader
{
    public:
        spell_item_spike_toed_booterang() : SpellScriptLoader("spell_item_spike_toed_booterang") { }

        class spell_item_spike_toed_booterang_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_spike_toed_booterang_SpellScript);

            SpellCastResult CheckRequirement()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                if (Unit* l_Target = l_Player->GetSelectedUnit())
                {
                    if (l_Target->getLevel() > 100)
                        return SPELL_FAILED_BAD_TARGETS;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy()
            {
                Unit* l_Target = GetHitUnit();
                if (l_Target == nullptr)
                    return;

                if (l_Target->IsMounted())
                    l_Target->RemoveAurasByType(SPELL_AURA_MOUNTED);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_item_spike_toed_booterang_SpellScript::CheckRequirement);
                AfterHit += SpellHitFn(spell_item_spike_toed_booterang_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_spike_toed_booterang_SpellScript();
        }
};

/// Magic Pet Mirror - 127696
/// Called by Mystic Image - 187356
class spell_item_mystic_image : public SpellScriptLoader
{
    public:
        spell_item_mystic_image() : SpellScriptLoader("spell_item_mystic_image") { }

        class spell_item_mystic_image_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_mystic_image_SpellScript);

            enum eSpells
            {
                MysticImage = 187358
            };

            void HandleDummy()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                Unit* l_Target = GetExplTargetUnit();   ///< Our minipet
                if (l_Target == nullptr)
                    return;

                l_Target->CastSpell(l_Player, eSpells::MysticImage, true);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_item_mystic_image_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_mystic_image_SpellScript();
        }
};

/// Sunreaver Beacon(For Horde) - 95568, Called by: 140300
class spell_item_sunreaver_beacon : public SpellScriptLoader
{
    public:
        spell_item_sunreaver_beacon() : SpellScriptLoader("spell_item_sunreaver_beacon") { }

        class spell_item_sunreaver_beacon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_sunreaver_beacon_SpellScript);

            void HandleDummy()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                l_Player->TeleportTo(1064, 7250.192871f, 6277.653320f, 19.352745f, 5.504922f);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_item_sunreaver_beacon_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_sunreaver_beacon_SpellScript();
        }
};

/// Super Simian Sphere - 37254
/// Called by Going Ape - 48333
class spell_item_super_simian_sphere : public SpellScriptLoader
{
    public:
        spell_item_super_simian_sphere() : SpellScriptLoader("spell_item_super_simian_sphere") { }

        class  spell_item_super_simian_sphere_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_super_simian_sphere_AuraScript);

            enum eSpells
            {
                GoingApe = 48332
            };

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Player* l_Player = GetTarget()->ToPlayer();
                if (l_Player == nullptr)
                    return;

                if (l_Player->HasAura(eSpells::GoingApe))
                    l_Player->RemoveAura(eSpells::GoingApe);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_super_simian_sphere_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_super_simian_sphere_AuraScript();
        }
};

/// The Perfect Blossom - 127766
/// Called by The Perfect Blossom - 187676
class spell_item_the_perfect_blossom : public SpellScriptLoader
{
    public:
        spell_item_the_perfect_blossom() : SpellScriptLoader("spell_item_the_perfect_blossom") { }

        class spell_item_the_perfect_blossom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_the_perfect_blossom_SpellScript);

            enum eSpells
            {
                FelPetal = 127768,
                minEntry = 1,
                maxEntry = 5
            };

            void HandleCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                uint32 l_Count = urand(eSpells::minEntry, eSpells::maxEntry);
                if (!l_Count)
                    return;

                l_Player->AddItem(eSpells::FelPetal, l_Count);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_item_the_perfect_blossom_SpellScript::HandleCast);

            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_the_perfect_blossom_SpellScript();
        }
};

/// Plans_Balanced_Trillium_Ingot_and_Its_Use - 100865
class spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses : public SpellScriptLoader
{
    public:
        spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses() : SpellScriptLoader("spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses") { }

        class spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses_SpellScript);

            enum eSpells
            {
                BalancedTrilliumIngot = 143255,
                AcceleratedBalancedTrilliumIngot = 146921,
                AvengersTrilliumLegplates = 142959,
                AvengersTrilliumWaistplate = 142968,
                BlessedTrilliumBelt = 142963,
                BlessedTrilliumGreaves = 142954,
                ProtectorsTrilliumLegguards = 142958,
                ProtectorsTrilliumWaistguard = 142967,
                MaxTrilliumPlans = 6,
            };

            std::vector<uint32> m_TrilliumPlans =
            {
                eSpells::AvengersTrilliumLegplates,
                eSpells::AvengersTrilliumWaistplate,
                eSpells::BlessedTrilliumBelt,
                eSpells::BlessedTrilliumGreaves,
                eSpells::ProtectorsTrilliumLegguards,
                eSpells::ProtectorsTrilliumWaistguard
            };

            void HandleAfterCast()
            {
                Player* p_Player = GetCaster()->ToPlayer();
                if (!p_Player)
                    return;

                if (!p_Player->HasSpell(eSpells::BalancedTrilliumIngot))
                    p_Player->learnSpell(eSpells::BalancedTrilliumIngot, false);

                if (!p_Player->HasSpell(eSpells::AcceleratedBalancedTrilliumIngot))
                    p_Player->learnSpell(eSpells::AcceleratedBalancedTrilliumIngot, false);

                uint32 l_Itr = 0;
                for (auto l_Itr = m_TrilliumPlans.begin(); l_Itr != m_TrilliumPlans.end();)
                {
                    if (p_Player->HasSpell(*l_Itr))
                        l_Itr = m_TrilliumPlans.erase(l_Itr);
                    else
                        ++l_Itr;
                }

                if (m_TrilliumPlans.size() != 0)
                {
                    l_Itr = rand() % m_TrilliumPlans.size();
                    p_Player->learnSpell(m_TrilliumPlans[l_Itr], false);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses_SpellScript();
        }
};

/// Last Update 7.1.5 - Build 23420
/// Called by Artificial Stamina - 211309
class spell_item_artificial_stamina : public SpellScriptLoader
{
    public:
        spell_item_artificial_stamina() : SpellScriptLoader("spell_item_artifical_stamina") { }

        class spell_item_artificial_stamina_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_artificial_stamina_AuraScript);

            bool Validate(SpellInfo const* spellInfo) override
            {
                return spellInfo->Effects[EFFECT_1].IsEffect();
            }

            bool Load() override
            {
                return GetOwner()->IsPlayer();
            }

            static const uint32 BASE_TRAIT_INCREASE = 6;

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                WorldObject* l_Owner = GetOwner();
                if (!l_Owner)
                    return;

                MS::Artifact::Manager* l_Artifact = l_Owner->ToPlayer()->GetCurrentlyEquippedArtifact();
                if (!l_Artifact)
                    return;

                float l_FullEffectMultiplier    = float(GetSpellInfo()->Effects[EFFECT_1].BasePoints) * 0.01f;
                float l_ReducedEffectMultiplier = l_FullEffectMultiplier * 0.2f;    ///< After 52nd point, Artificial Stamina is 5 times less effective.
                uint32 l_TraitForFullScaling    = std::min(52u, (uint32)l_Artifact->GetLevel());
                uint32 l_TraitForReducedScaling = l_Artifact->GetLevel() - l_TraitForFullScaling;

                p_Amount = (l_FullEffectMultiplier * (l_TraitForFullScaling + BASE_TRAIT_INCREASE) + (l_ReducedEffectMultiplier * l_TraitForReducedScaling)) * 100.0f;  ///< *100 for float value, /100 in GetFloatAmount
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_artificial_stamina_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_artificial_stamina_AuraScript();
        }
};

// 226829 219655 213428
class spell_item_artifical_damage : public SpellScriptLoader
{
public:
    spell_item_artifical_damage() : SpellScriptLoader("spell_item_artifical_damage") { }

    class spell_item_artifical_damage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_artifical_damage_AuraScript);

        bool Validate(SpellInfo const* spellInfo) override
        {
            return spellInfo->Effects[EFFECT_1].IsEffect();
        }

        bool Load() override
        {
            return GetOwner()->GetTypeId() == TYPEID_PLAYER;
        }

        static const uint32 BASE_TRAIT_INCREASE = 6;

        void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& p_CanBeRecalculated)
        {
            WorldObject* l_Owner = GetOwner();
            if (!l_Owner || !l_Owner->IsPlayer())
                return;

            Player* l_Player = l_Owner->ToPlayer();

            MS::Artifact::Manager* l_Artifact = l_Player->GetCurrentlyEquippedArtifact();
            if (!l_Artifact)
                return;

            float l_Multiplier = 0.013f;

            /// Buffed by 30% in 7.2, but spelldata still indicates the old value of 0.5 for tanks/discos and 1 for healers
            /// As of 2018-02-23 there is no damage increase past the 52nd trait
            p_Amount = ((GetSpellInfo()->Effects[EFFECT_1].BasePoints * l_Multiplier) * (std::min((uint32)l_Artifact->GetLevel(), 52u) + BASE_TRAIT_INCREASE) * 100.f); ///< *100 for float value, /100 in GetFloatAmount
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_artifical_damage_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_artifical_damage_AuraScript();
    }
};

// Potion of the Old War - 188028
class spell_item_potion_of_the_old_war : public SpellScriptLoader
{
    public:
        spell_item_potion_of_the_old_war() : SpellScriptLoader("spell_item_potion_of_the_old_war") { }

        class spell_item_potion_of_the_old_war_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_potion_of_the_old_war_AuraScript);

            enum eSpells
            {
                SummonGhost = 229050
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;
                Position l_PosLeft = l_Caster->GetPosition();
                Position l_PosRight = l_Caster->GetPosition();

                l_PosLeft.m_positionX = l_PosLeft.m_positionX + 3.0f * std::cos(M_PI / 2.0f);
                l_PosLeft.m_positionY = l_PosLeft.m_positionY + 3.0f * std::sin(M_PI / 2.0f);
                l_PosRight.m_positionX = l_PosRight.m_positionX + 3.0f * std::cos(-M_PI / 2.0f);
                l_PosRight.m_positionY = l_PosRight.m_positionY + 3.0f * std::sin(-M_PI / 2.0f);
                l_Caster->CastSpell(l_PosLeft, eSpells::SummonGhost, true);
                l_Caster->CastSpell(l_PosRight, eSpells::SummonGhost, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_potion_of_the_old_war_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_potion_of_the_old_war_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Poisoned Dreams - 222706
/// Called for Bough of Corruption (item) - 139336
class spell_item_bough_of_corruption : public SpellScriptLoader
{
    public:
        spell_item_bough_of_corruption() : SpellScriptLoader("spell_item_bough_of_corruption") { }

        class spell_item_bough_of_corruption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_bough_of_corruption_AuraScript);

            enum eSpells
            {
                PoisonedDreamsItemAura  = 222705,
                PoisonedDreamsDamage    = 222711,
                PoisonedDreamsSpread    = 222715,

                MindFlay                = 15407,
                MindSear                = 237388,
                DrainSoul               = 198590,
                DrainLife               = 234153
            };

            Guid128 m_LastProcGUID;

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_SpellInfo->Id == eSpells::PoisonedDreamsDamage)
                    return false;

                if (l_SpellInfo->Id == eSpells::MindFlay || l_SpellInfo->Id == eSpells::MindSear || l_SpellInfo->Id == eSpells::DrainSoul || l_SpellInfo->Id == eSpells::DrainLife)
                    return true;

                Spell const* l_Spell = l_DamageInfo->GetSpell();
                if (!l_Spell)
                    return false;

                Guid128 l_CastGUID = l_Spell->GetCastGuid();
                if (l_CastGUID == m_LastProcGUID)
                    return false;

                m_LastProcGUID = l_CastGUID;

                switch (l_DamageInfo->GetDamageType())
                {
                    case DamageEffectType::DIRECT_DAMAGE:
                    case DamageEffectType::SPELL_DIRECT_DAMAGE:
                    case DamageEffectType::DOT:
                        break;
                    default:
                        return false;
                }

                if (GetCaster() != p_EventInfo.GetActor())
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Aura* l_Aura = l_Player->GetAura(eSpells::PoisonedDreamsItemAura);
                if (!l_Aura)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                float l_Multiplier = 0.0f;
                int32 l_TrinketItemLevel = l_Item->GetItemLevel(l_Player);
                RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
                if (l_RandomPropertiesPoints)
                    l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

                SpellInfo const* l_PoisonedDreamsInfo = sSpellMgr->GetSpellInfo(eSpells::PoisonedDreamsItemAura);
                if (!l_PoisonedDreamsInfo)
                    return;

                float l_ScalingMultiplier = l_PoisonedDreamsInfo->Effects[EFFECT_0].ScalingMultiplier;
                float l_Amount = l_ScalingMultiplier * l_Multiplier;
                float l_DeltaScalingMultiplier = l_PoisonedDreamsInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
                if (l_DeltaScalingMultiplier)
                {
                    float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                    l_Amount += frand(-l_Delta, l_Delta);
                }

                int32 l_Damage = (int32)l_Amount * GetAura()->GetStackAmount();

                l_Caster->CastCustomSpell(l_Target, eSpells::PoisonedDreamsDamage, &l_Damage, nullptr, nullptr, true, l_Item);
            }

            void HandleOnEffectPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PoisonedDreamsSpread, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_bough_of_corruption_AuraScript::HandleOnCheckProc);
                OnProc += AuraProcFn(spell_item_bough_of_corruption_AuraScript::HandleOnProc);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_bough_of_corruption_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_bough_of_corruption_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Poisoned Dreams (spread) - 222715
/// Called for Bough of Corruption (item) - 139336
class spell_item_bough_of_corruption_spread : public SpellScriptLoader
{
    public:
        spell_item_bough_of_corruption_spread() : SpellScriptLoader("spell_item_bough_of_corruption_spread") { }

        class spell_item_bough_of_corruption_spread_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_bough_of_corruption_spread_SpellScript);

            enum eSpells
            {
                PoisonedDreams  = 222706
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetExplTargetUnit();
                if (!l_Caster || !l_Owner)
                    return;

                p_Targets.remove_if([=](WorldObject* p_Target) -> bool
                {
                    if (p_Target->GetGUID() == l_Owner->GetGUID())
                        return true;

                    Unit* l_UnitTarget = p_Target->ToUnit();
                    if (!l_UnitTarget || l_UnitTarget->HasAura(eSpells::PoisonedDreams))
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                {
                    Aura* l_Aura = l_Owner->GetAura(eSpells::PoisonedDreams);
                    if (!l_Aura)
                        return;

                    uint32 l_duration = l_Aura->GetDuration();
                    l_Aura->ModStackAmount(1);
                    l_Aura->SetDuration(l_duration);
                    return;
                }

                p_Targets.sort(JadeCore::ObjectDistanceOrderPred(l_Owner));
                if (p_Targets.empty())
                    return;

                Unit* l_Target = p_Targets.front()->ToUnit();
                if (!l_Target)
                    return;

                l_Caster->AddAura(eSpells::PoisonedDreams, l_Target);

                Aura* l_AuraOwner = l_Owner->GetAura(eSpells::PoisonedDreams);
                Aura* l_AuraTarget = l_Target->GetAura(eSpells::PoisonedDreams);
                if (!l_AuraOwner || !l_AuraTarget)
                    return;

                l_AuraTarget->SetDuration(l_AuraOwner->GetDuration());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_bough_of_corruption_spread_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_bough_of_corruption_spread_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Fel Meteor - 214054
/// Called for Eye of Skovald (item) - 133641
class spell_item_eye_of_skovald : public SpellScriptLoader
{
    public:
        spell_item_eye_of_skovald() : SpellScriptLoader("spell_item_eye_of_skovald") { }

        class spell_item_eye_of_skovald_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_eye_of_skovald_AuraScript);

            enum eSpells
            {
                MindFlay = 15407
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActionTarget();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_Target || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo || l_Target->IsFriendlyTo(l_Caster) || l_SpellInfo->Id == eSpells::MindFlay)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_eye_of_skovald_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_eye_of_skovald_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Maddening Whispers - 222046
/// Called for Wriggling Sinew (item) - 139326
class spell_item_maddening_whispers : public SpellScriptLoader
{
    public:
        spell_item_maddening_whispers() : SpellScriptLoader("spell_item_maddening_whispers") { }

        class spell_item_maddening_whispers_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_maddening_whispers_SpellScript);

            enum eSpells
            {
                MaddeningAura = 222046
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::MaddeningAura);
                if (!l_Aura)
                    return;

                l_Aura->SetStackAmount(l_SpellInfo->StackAmount);
                l_Aura->SetCharges(l_SpellInfo->StackAmount);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_maddening_whispers_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_maddening_whispers_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Maddening Whispers - 222050
class spell_item_maddening_whispers_damage : public SpellScriptLoader
{
    public:
        spell_item_maddening_whispers_damage() : SpellScriptLoader("spell_item_maddening_whispers_damage") { }

        class spell_item_maddening_whispers_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_maddening_whispers_damage_AuraScript);

            enum eSpells
            {
                MaddeningWhispers = 222046,
                MaddeningHit    = 222052
            };

            uint32 m_Time = 0;

            void HandleApply(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes /* p_Mode */)
            {
                m_Time = GetDuration();
            }

            void HandleReApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /* p_Mode */)
            {
                SetDuration(m_Time);
            }

            void HandleAfterRemove(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                float l_Multiplier = 0.0f;
                int32 l_TrinketItemLevel = l_Item->GetItemLevel(l_Player);
                RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
                if (l_RandomPropertiesPoints)
                    l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

                SpellInfo const* l_MaddeningWhispersInfo = sSpellMgr->GetSpellInfo(eSpells::MaddeningWhispers);
                if (!l_MaddeningWhispersInfo)
                    return;

                float l_ScalingMultiplier = l_MaddeningWhispersInfo->Effects[EFFECT_0].ScalingMultiplier;
                float l_Amount = l_ScalingMultiplier * l_Multiplier;
                float l_DeltaScalingMultiplier = l_MaddeningWhispersInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
                if (l_DeltaScalingMultiplier)
                {
                    float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                    l_Amount += frand(-l_Delta, l_Delta);
                }

                int32 l_Damage = (int32)l_Amount * GetAura()->GetStackAmount();

                l_Player->CastCustomSpell(l_Target, eSpells::MaddeningHit, &l_Damage, nullptr, nullptr, true, l_Item);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_item_maddening_whispers_damage_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
                AfterEffectApply += AuraEffectApplyFn(spell_item_maddening_whispers_damage_AuraScript::HandleReApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_maddening_whispers_damage_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_maddening_whispers_damage_AuraScript();
        }

        class spell_item_maddening_whispers_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_maddening_whispers_damage_SpellScript);

            enum eSpells
            {
                MaddeningHit    = 222052,
                MaddeningAura   = 222046,
                MaddeningDebuff = 222050
            };

            uint32 m_Time = 0;

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::MaddeningAura);
                Unit* l_Target = GetExplTargetUnit();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Aura || !l_Target || !l_Player)
                    return;

                m_Time = l_Aura->GetDuration();
                l_Aura->DropStack();

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                if (l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::WhisperMaddening].find(l_Target->GetGUID()) == l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::WhisperMaddening].end())
                    l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::WhisperMaddening].insert(l_Target->GetGUID());
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::MaddeningAura);
                if (!l_Aura || l_Aura->GetStackAmount() <= 0)
                {
                    for (uint64 l_Guid : l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::WhisperMaddening])
                    {
                        if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Guid))
                            l_Target->RemoveAura(eSpells::MaddeningDebuff);
                    }

                    l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::WhisperMaddening].clear();
                    return;
                }
                l_Aura->SetDuration(m_Time);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_item_maddening_whispers_damage_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_item_maddening_whispers_damage_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_maddening_whispers_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Infested Ground - 221804
class spell_item_infested_ground : public SpellScriptLoader
{
    public:
        spell_item_infested_ground() : SpellScriptLoader("spell_item_infested_ground") { }

        class spell_item_infested_ground_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_infested_ground_SpellScript);

            enum eSpells
            {
                InfestedGroundAT = 221803
            };

            float m_Amount = 0.0;

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::InfestedGroundAT);
                if (!l_Caster || !l_SpellInfo || !l_Target)
                    return;

                int32 l_Damage = m_Amount;

                if (l_Caster->IsPlayer())
                {
                    Player* l_Player = l_Caster->ToPlayer();

                    Spell* l_Spell = GetSpell();
                    if (!l_Player || !l_Spell)
                        return;

                    float l_Versatility = (1.0f + l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f);
                    l_Damage *= l_Versatility;

                    l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);
                }

                SetHitDamage(l_Damage);
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                Item* l_Item = GetCastItem();
                if (!l_Caster || !l_Item || !l_Caster->IsPlayer())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                float l_Multiplier = 0.0f;
                int32 l_TrinketItemLevel = l_Item->GetItemLevel(l_Player);
                RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
                if (l_RandomPropertiesPoints)
                    l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

                SpellInfo const* l_InfestedGroundInfo = sSpellMgr->GetSpellInfo(eSpells::InfestedGroundAT);
                if (!l_InfestedGroundInfo)
                    return;

                float l_ScalingMultiplier = l_InfestedGroundInfo->Effects[EFFECT_1].ScalingMultiplier;
                float l_Amount = l_ScalingMultiplier * l_Multiplier;
                float l_DeltaScalingMultiplier = l_InfestedGroundInfo->Effects[EFFECT_1].DeltaScalingMultiplier;
                if (l_DeltaScalingMultiplier)
                {
                    float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                    l_Amount += frand(-l_Delta, l_Delta);
                }

                m_Amount = l_Amount;
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_item_infested_ground_SpellScript::HandleOnCast);
                BeforeHit += SpellHitFn(spell_item_infested_ground_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_infested_ground_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Leeching Pestilence - 221805
class spell_item_leeching_pestilence : public SpellScriptLoader
{
    public:
        spell_item_leeching_pestilence() : SpellScriptLoader("spell_item_leeching_pestilence") { }

        class spell_item_leeching_pestilence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_leeching_pestilence_AuraScript);

            enum eSpells
            {
                InfestedGroundAT = 221803
            };

            void HandleEffectCalcAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::InfestedGroundAT, EFFECT_2);
                if (!l_AuraEffect)
                    return;

                p_Amount = l_AuraEffect->GetAmount();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_leeching_pestilence_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_leeching_pestilence_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Solemnity - 224346
class spell_item_solemnity : public SpellScriptLoader
{
    public:
        spell_item_solemnity() : SpellScriptLoader("spell_item_solemnity") { }

        enum eSpells
        {
            SolemnityProc = 224347
        };

        class spell_item_solemnity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_solemnity_AuraScript);

            void HandleOnEffectPeriodic(AuraEffect const*)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Aura* l_Aura = l_Owner->GetAura(eSpells::SolemnityProc))
                {
                    int32 l_Duration = l_Aura->GetDuration();
                    l_Aura->DropStack();
                    l_Aura->SetDuration(l_Duration);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_solemnity_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_solemnity_AuraScript();
        }

        class spell_item_solemnity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_solemnity_SpellScript);

            void HandleLaunch(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SolemnityProc, true, GetCastItem());
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::SolemnityProc))
                {
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Aura->SetStackAmount(20);
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_item_solemnity_SpellScript::HandleLaunch, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_solemnity_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Shadowy Reflection - 222478 For Phantasmal Echo - 138225
class spell_item_shadowy_reflection : public SpellScriptLoader
{
    public:
        spell_item_shadowy_reflection() : SpellScriptLoader("spell_item_shadowy_reflection") { }

        class spell_item_shadowy_reflection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_shadowy_reflection_AuraScript);

            enum eSpells
            {
                ShadowyReflection = 222479
            };

            bool CheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->GetHealthPct() >= 50)
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_shadowy_reflection_AuraScript::CheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_shadowy_reflection_AuraScript();
        }
};

/// Called by Shadowy Reflection - 222479
class spell_item_shadowy_reflection_absorb : public SpellScriptLoader
{
public:
    spell_item_shadowy_reflection_absorb() : SpellScriptLoader("spell_item_shadowy_reflection_absorb") { }

    class  spell_item_shadowy_reflection_absorb_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_shadowy_reflection_absorb_AuraScript);

        enum eSpells
        {
            ShadowyReflection = 222478
        };

        enum eNPCs
        {
            ShadowyReflectionNPC = 111748
        };

        void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
        {
            Unit* l_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_SpellInfo || !l_Caster)
                return;

            p_AbsorbAmount = CalculatePct(p_DamageInfo.GetAmount(), l_SpellInfo->Effects[EFFECT_1].BasePoints);
            p_AbsorbAmount = std::min<uint32>(p_AbsorbAmount, p_AuraEffect->GetAmount());

            for (Unit* l_Summon : l_Caster->m_Controlled)
            {
                if (l_Summon->GetEntry() != eNPCs::ShadowyReflectionNPC)
                    continue;

                UnitAI* l_AI = l_Summon->GetAI();
                if (!l_AI)
                    continue;

                l_AI->SetData(0, p_AbsorbAmount);
                break;
            }

            return;
        }

        void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::ShadowyReflection, EFFECT_2);
            if (!l_AuraEffect)
                return;

            const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(l_AuraEffect->GetAmount());
        }

        void Register() override
        {

            AfterEffectApply += AuraEffectApplyFn(spell_item_shadowy_reflection_absorb_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_shadowy_reflection_absorb_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        }
    };
    AuraScript* GetAuraScript() const override
    {

        return new spell_item_shadowy_reflection_absorb_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Six-Feather Fan - 227868
class spell_item_six_feather_fan : public SpellScriptLoader
{
    public:
        spell_item_six_feather_fan() : SpellScriptLoader("spell_item_six_feather_fan") { }

        class spell_item_six_feather_fan_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_six_feather_fan_AuraScript);

            enum eSpells
            {
                SixFeatherFanAura = 227869
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Target || !l_Player)
                    return;

                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SixFeatherFanAura, true, l_Item, nullptr, l_Player->GetGUID());
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_six_feather_fan_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_six_feather_fan_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Six-Feather Fan - 227869
class spell_item_six_feather_fan_damage : public SpellScriptLoader
{
    public:
        spell_item_six_feather_fan_damage() : SpellScriptLoader("spell_item_six_feather_fan_damage") { }

        class spell_item_six_feather_fan_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_six_feather_fan_damage_AuraScript);

            enum eSpells
            {
                SixFeatherFanDamages = 227870
            };

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->GetTargetGUID());
                if (!l_Player || !l_Target)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(GetAura()->GetCastItemGUID());
                if (!l_Item)
                    return;

                for (uint8 i = 0; i < 6; i++) /// Following the Tooltip
                    l_Player->CastSpell(l_Target, eSpells::SixFeatherFanDamages, true, l_Item);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_six_feather_fan_damage_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_six_feather_fan_damage_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Screams of the Dead - 214798  Item: Memento of Angerboda - 133644
class spell_item_screams_of_the_dead : public SpellScriptLoader
{
    public:
        spell_item_screams_of_the_dead() : SpellScriptLoader("spell_item_screams_of_the_dead") { }

        class spell_item_screams_of_the_dead_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_screams_of_the_dead_AuraScript);

            enum eSpells
            {
                DirgeofAngerboda    = 214807,
                WailofSvala         = 214803,
                HowlofIngvar        = 214802
            };

            std::vector<eSpells> m_Spells = {
                eSpells::DirgeofAngerboda,
                eSpells::WailofSvala,
                eSpells::HowlofIngvar
            };

            void HandleProc(ProcEventInfo& /* p_EvenInfo */)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                AuraEffect const* l_AuraEffect = l_Aura->GetEffect(EFFECT_1);
                if (!l_Item || !l_AuraEffect)
                    return;

                int32 l_Bp = l_AuraEffect->GetAmount();
                l_Caster->CastCustomSpell(l_Caster, m_Spells[urand(0, m_Spells.size() - 1)], &l_Bp, nullptr, nullptr, true, l_Item);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_screams_of_the_dead_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_screams_of_the_dead_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Imp Generator - 196777
class spell_item_imp_generator : public SpellScriptLoader
{
    public:
        spell_item_imp_generator() : SpellScriptLoader("spell_item_imp_generator") { }

        class spell_item_imp_generator_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_imp_generator_AuraScript);

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ImpGenerator) = l_Target->GetGUID();
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_imp_generator_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_imp_generator_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Nightwell Energy - 214577
class spell_item_nightwell_energy : public SpellScriptLoader
{
    public:
        spell_item_nightwell_energy() : SpellScriptLoader("spell_item_nightwell_energy") { }

        class spell_item_nightwell_energy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nightwell_energy_AuraScript);

            enum eSpells
            {
                NightwellEnergyBuff = 214572
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Amount *= l_Caster->GetAuraCount(eSpells::NightwellEnergyBuff);

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->RemoveAura(eSpells::NightwellEnergyBuff);
                }, 0);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_nightwell_energy_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nightwell_energy_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Elune's Light Draft - 215648   Item: Moonlit Prism - 137541
class spell_item_elunes_light: public SpellScriptLoader
{
    public:
        spell_item_elunes_light() : SpellScriptLoader("spell_item_elunes_light") { }

        class spell_item_elunes_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_elunes_light_AuraScript);

            void HandleProc(ProcEventInfo& /* p_EvenInfo */)
            {
                if (GetStackAmount() < 20) ///< Maximum stack is only 20
                    SetStackAmount(GetStackAmount() + 1);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_elunes_light_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_elunes_light_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Cleansing Flame - 201408
class spell_item_cleansing_flame : public SpellScriptLoader
{
    public:
        spell_item_cleansing_flame() : SpellScriptLoader("spell_item_cleansing_flame") { }

        class spell_item_cleansing_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_cleansing_flame_SpellScript);

            void HandleTargetSelect(WorldObject*& p_Target)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster)
                    p_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->GetTargetGUID());
            }

            void Register() override
            {
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_item_cleansing_flame_SpellScript::HandleTargetSelect, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_cleansing_flame_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Taint of the Sea - 215670
class spell_item_taint_of_the_sea_target : public SpellScriptLoader
{
    public:
        spell_item_taint_of_the_sea_target() : SpellScriptLoader("spell_item_taint_of_the_sea_target") { }

        class spell_item_taint_of_the_sea_target_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_taint_of_the_sea_target_AuraScript);

            void HandleApply(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();
                if (!l_Owner || !l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::TaintOfTheSea].insert(l_Owner->GetGUID());
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();
                if (!l_Owner || !l_Caster)
                    return;

                l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::TaintOfTheSea].erase(l_Owner->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_taint_of_the_sea_target_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_taint_of_the_sea_target_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_taint_of_the_sea_target_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Taint of the Sea - 215672
class spell_item_taint_of_the_sea_caster : public SpellScriptLoader
{
    public:
        spell_item_taint_of_the_sea_caster() : SpellScriptLoader("spell_item_taint_of_the_sea_caster") { }

        class spell_item_taint_of_the_sea_caster_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_taint_of_the_sea_caster_AuraScript);

            enum eSpells
            {
                TaintOfTheSeaDamage = 215695
            };

            void HandleProc(AuraEffect const*, ProcEventInfo& p_ProcInfo)
            {
                DamageInfo* l_DamageInfo = p_ProcInfo.GetDamageInfo();
                Unit* l_Owner = GetUnitOwner();
                Unit* l_ActionTarget = p_ProcInfo.GetActionTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_DamageInfo || !l_Owner || !l_SpellInfo)
                    return;

                std::set<uint64> l_TargetSet = l_Owner->m_SpellHelper.GetUint64Set()[eSpellHelpers::TaintOfTheSea];
                int32 l_Damage = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_1].BasePoints);

                if (l_Damage > l_SpellInfo->Effects[EFFECT_0].BasePoints)
                    l_Damage = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                for (uint64 l_TargetGUID : l_TargetSet)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*l_Owner, l_TargetGUID);

                    if (l_Target && l_Target != l_ActionTarget)
                        l_Owner->CastCustomSpell(l_Target, eSpells::TaintOfTheSeaDamage, &l_Damage, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_taint_of_the_sea_caster_AuraScript::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_taint_of_the_sea_caster_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dark Blast - 215444
class spell_item_dark_blast : public SpellScriptLoader
{
    public:
        spell_item_dark_blast() : SpellScriptLoader("spell_item_dark_blast") { }

        class spell_item_dark_blast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_dark_blast_AuraScript);

            enum eSpells
            {
                DarkBlastDamage      = 215407,
                DarkBlastVisual      = 214399,
                DisciplinePriestAura = 137032
            };

            void HandleProc(AuraEffect const*, ProcEventInfo& p_ProcInfo)
            {
                Unit* l_Owner = GetUnitOwner();
                Unit* l_ActionTarget = p_ProcInfo.GetActionTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_ActionTarget || !l_Owner || !l_SpellInfo)
                    return;

                int32 l_Damage = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                if (l_Owner->HasAura(eSpells::DisciplinePriestAura))
                {
                    if (SpellInfo const* l_DisciplineInfo = sSpellMgr->GetSpellInfo(eSpells::DisciplinePriestAura))
                    {
                        int32 l_Pct = 100.0f + l_DisciplineInfo->Effects[EFFECT_8].BasePoints;
                        l_Damage = CalculatePct(l_Damage, l_Pct);
                    }
                }

                l_Owner->CastCustomSpell(l_ActionTarget, eSpells::DarkBlastDamage, &l_Damage, nullptr, nullptr, true);
                l_Owner->CastSpell(l_ActionTarget, eSpells::DarkBlastVisual, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_dark_blast_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_dark_blast_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Burning Intensity - 215815
class spell_item_burning_intensity : public SpellScriptLoader
{
    public:
        spell_item_burning_intensity() : SpellScriptLoader("spell_item_burning_intensity") { }

        enum eSpells
        {
            BurningIntensityProc = 215816
        };

        class spell_item_burning_intensity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_burning_intensity_AuraScript);

            void HandleOnEffectPeriodic(AuraEffect const*)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Aura* l_Aura = l_Owner->GetAura(eSpells::BurningIntensityProc))
                {
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                    {
                        int32 l_Duration = l_Aura->GetDuration();
                        l_Aura->ModStackAmount(1);
                        l_Aura->SetDuration(l_Duration);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_burning_intensity_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_burning_intensity_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Imp Portal - 196760
class spell_item_imp_generator_portal : public SpellScriptLoader
{
    public:
        spell_item_imp_generator_portal() : SpellScriptLoader("spell_item_imp_generator_portal") { }

        class spell_item_imp_generator_portal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_imp_generator_portal_SpellScript);

            enum eEntry
            {
                ImpPortal = 99937
            };

            enum eSpells
            {
                ImpGeneratorAura        = 196777,
                ImpGeneratorPortalAura  = 196763,
                AngryImps               = 196765
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                uint64 l_TargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ImpGenerator);

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon && l_Summon->GetEntry() == eEntry::ImpPortal)
                    {
                        l_Summon->AddUnitState(UnitState::UNIT_STATE_ROOT);

                        l_Summon->CastSpell(l_Summon, eSpells::ImpGeneratorPortalAura, true);
                        l_Summon->AddDelayedEvent([l_Summon, l_TargetGUID]() -> void
                        {
                            if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Summon, l_TargetGUID))
                                l_Summon->CastSpell(l_Target, eSpells::AngryImps, false);
                        }, 1000);
                    }
                }

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ImpGenerator) = 0;
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_item_imp_generator_portal_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_imp_generator_portal_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Volatile Magic - 215859
class spell_item_volatile_magic : public SpellScriptLoader
{
    public:
        spell_item_volatile_magic() : SpellScriptLoader("spell_item_volatile_magic") { }

        enum eSpells
        {
            WitheringConsumption = 215884,
            DisciplinePriestAura = 137032
        };

        enum eDatas
        {
            MaxStacks = 6
        };

        class spell_item_volatile_magic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_volatile_magic_AuraScript);

            void HandleApply(AuraEffect const *p_Effect, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();

                if (l_Owner == nullptr || l_Caster == nullptr || !l_Caster->IsPlayer())
                    return;

                if (Aura* l_Aura = p_Effect->GetBase())
                {
                    if (l_Aura->GetStackAmount() >= eDatas::MaxStacks)
                    {
                        int32 l_Bp = p_Effect->GetAmount();
                        l_Bp /= eDatas::MaxStacks;

                        Item* l_Trinket = l_Caster->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

                        if (l_Caster->HasAura(eSpells::DisciplinePriestAura))
                        {
                            if (SpellInfo const* l_DisciplineInfo = sSpellMgr->GetSpellInfo(eSpells::DisciplinePriestAura))
                            {
                                int32 l_Pct = 100.0f + l_DisciplineInfo->Effects[EFFECT_4].BasePoints;
                                l_Bp = CalculatePct(l_Bp, l_Pct);
                            }
                        }

                        l_Caster->CastCustomSpell(eSpells::WitheringConsumption, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Bp, l_Owner, true, l_Trinket);
                        l_Aura->Remove();
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_volatile_magic_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_volatile_magic_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Incensed - 228142
class spell_item_incensed : public SpellScriptLoader
{
    public:
        spell_item_incensed() : SpellScriptLoader("spell_item_incensed") { }

        class spell_item_incensed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_incensed_AuraScript);

            void HandleCalcAmount(AuraEffect const* p_Effect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                p_Amount = p_Effect->GetBaseAmount();
                p_CanBeRecalculated = false;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_incensed_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_MOD_CRIT_DAMAGE_BONUS);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_incensed_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dominion Deck - 191563
class spell_item_darkmoon_deck : public SpellScriptLoader
{
    public:
        spell_item_darkmoon_deck() : SpellScriptLoader("spell_item_darkmoon_deck") { }

        class spell_item_darkmoon_deck_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_darkmoon_deck_AuraScript);

            enum eDominion
            {
                AceDominion     = 191545,
                TwoDominion     = 191548,
                ThreeDominion   = 191549,
                FourDominion    = 191550,
                FiveDominion    = 191551,
                SixDominion     = 191552,
                SevenDominion   = 191553,
                EightDominion   = 191554,
            };

            enum ePromise
            {
                AcePromise      = 191615,
                TwoPromise      = 191616,
                ThreePromise    = 191617,
                FourPromise     = 191618,
                FivePromise     = 191619,
                SixPromise      = 191620,
                SevenPromise    = 191621,
                EightPromise    = 191622
            };

            enum eImmortality
            {
                AceImmortality      = 191624,
                TwoImmortality      = 191625,
                ThreeImmortality    = 191626,
                FourImmortality     = 191627,
                FiveImmortality     = 191628,
                SixImmortality      = 191629,
                SevenImmortality    = 191630,
                EightImmortality    = 191631
            };

            enum eHellfire
            {
                AceHellfire     = 191603,
                TwoHellfire     = 191604,
                ThreeHellfire   = 191605,
                FourHellfire    = 191606,
                FiveHellfire    = 191607,
                SixHellfire     = 191608,
                SevenHellfire   = 191609,
                EightHellfire   = 191610
            };

            enum eRange
            {
                Dominion,
                Promise,
                Immortality,
                Hellfire
            };

            std::vector<std::vector<uint32>> m_Spells
            {
                {
                    eDominion::AceDominion,
                    eDominion::TwoDominion,
                    eDominion::ThreeDominion,
                    eDominion::FourDominion,
                    eDominion::FiveDominion,
                    eDominion::SixDominion,
                    eDominion::SevenDominion,
                    eDominion::EightDominion
                },
                {
                    ePromise::AcePromise,
                    ePromise::TwoPromise,
                    ePromise::ThreePromise,
                    ePromise::FourPromise,
                    ePromise::FivePromise,
                    ePromise::SixPromise,
                    ePromise::SevenPromise,
                    ePromise::EightPromise
                },
                {
                    eImmortality::AceImmortality,
                    eImmortality::TwoImmortality,
                    eImmortality::ThreeImmortality,
                    eImmortality::FourImmortality,
                    eImmortality::FiveImmortality,
                    eImmortality::SixImmortality,
                    eImmortality::SevenImmortality,
                    eImmortality::EightImmortality
                },
                {
                    eHellfire::AceHellfire,
                    eHellfire::TwoHellfire,
                    eHellfire::ThreeHellfire,
                    eHellfire::FourHellfire,
                    eHellfire::FiveHellfire,
                    eHellfire::SixHellfire,
                    eHellfire::SevenHellfire,
                    eHellfire::EightHellfire
                },
            };

            int8 l_DeckType = -1;
            uint32 l_LastCard = 0;

            void HandleEffectApply(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                switch (GetId())
                {
                    case 191563:
                        l_DeckType = eRange::Dominion;
                        break;
                    case 191623:
                        l_DeckType = eRange::Promise;
                        break;
                    case 191632:
                        l_DeckType = eRange::Immortality;
                        break;
                    case 191611:
                        l_DeckType = eRange::Hellfire;
                        break;
                    default:
                        return;
                }
            }

            void HandleOnEffectPeriodic(AuraEffect const* /* p_AuraEffect */)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura || !l_Caster->isInCombat())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                if (l_DeckType == -1)
                    return;

                if (l_LastCard)
                    l_Caster->RemoveAura(l_LastCard);

                l_LastCard = m_Spells[l_DeckType][urand(0, m_Spells[l_DeckType].size() - 1)];
                l_Caster->CastSpell(l_Caster, l_LastCard, true, l_Item);
            }

            void HandleAfterEffectRemove(AuraEffect const* /* p_AuraEffect */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_LastCard)
                    l_Caster->RemoveAura(l_LastCard);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_item_darkmoon_deck_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_darkmoon_deck_AuraScript::HandleOnEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_darkmoon_deck_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_darkmoon_deck_AuraScript();
        }
};

/// Called by Opening Hand - 227388
/// Called for Eyasu's Mulligan (item) - 141584
class spell_item_opening_hand : public SpellScriptLoader
{
    public:
        spell_item_opening_hand() : SpellScriptLoader("spell_item_opening_hand") { }

        class spell_item_opening_hand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_opening_hand_AuraScript);

            enum eSpells
            {
                CritRoll        = 227389,
                HasteRoll       = 227395,
                MasteryRoll     = 227396,
                VersatilityRoll = 227397
            };

            std::array<uint32, 4> const l_SpellsRoll =
            {
                {eSpells::CritRoll, eSpells::HasteRoll, eSpells::MasteryRoll, eSpells::VersatilityRoll}
            };

            void HandleAfterEffectApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint32 l_SpellId : l_SpellsRoll)
                    l_Caster->RemoveAura(l_SpellId);

                int32 l_Rand = urand(0, l_SpellsRoll.size() - 1);
                int32 l_Amount = p_AurEff->GetAmount();
                l_Caster->CastCustomSpell(l_Caster, l_SpellsRoll[l_Rand], &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_opening_hand_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_opening_hand_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Phased Webbing - 215197
class spell_item_phased_webbing : public SpellScriptLoader
{
    public:
        spell_item_phased_webbing() : SpellScriptLoader("spell_item_phased_webbing") { }

        enum eSpells
        {
            PhaseWebbingBuff = 215198
        };

        class spell_item_phased_webbing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_phased_webbing_AuraScript);

            void HandleOnEffectPeriodic(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::PhaseWebbingBuff))
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() + 1);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_phased_webbing_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_phased_webbing_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Nightmare Egg Shell - 215024
class spell_item_nightmare_egg_shell : public SpellScriptLoader
{
    public:
        spell_item_nightmare_egg_shell() : SpellScriptLoader("spell_item_nightmare_egg_shell") { }

        enum eSpells
        {
            DownDraft = 214342
        };

        class spell_item_nightmare_egg_shell_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nightmare_egg_shell_AuraScript);

            void HandleOnEffectPeriodic(AuraEffect const*)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::DownDraft))
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() + 1);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_nightmare_egg_shell_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nightmare_egg_shell_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mechanical Bomb Squirrel - 216085
class spell_item_mechanical_bomb_squirrel : public SpellScriptLoader
{
    public:
        spell_item_mechanical_bomb_squirrel() : SpellScriptLoader("spell_item_mechanical_bomb_squirrel") { }

        enum eSpells
        {
            DownDraft = 214342
        };

        class spell_item_mechanical_bomb_squirrel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_mechanical_bomb_squirrel_AuraScript);

            enum eSpells
            {
                MechanicalBombSquirrel  = 216092,
                BombSquirrel            = 201302,
                SquirrelExplosion       = 201303
            };

            enum eNpc
            {
                Squirrel               = 108804
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Aura* l_Aura = GetAura();
                Player* l_Player = l_Caster->ToPlayer();
                Unit *l_Target = l_DamageInfo->GetTarget();
                if (!l_Aura || !l_Player || !l_Target)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::MechanicalBombSquirrel, true, l_Item);

                for (auto l_Squirrel : l_Caster->m_Controlled)
                {
                    if (l_Squirrel->GetEntry() == eNpc::Squirrel)
                    {
                        l_Squirrel->CastSpell(l_Target, eSpells::BombSquirrel, false);

                        uint64 l_TargetGuid = l_Target->GetGUID();
                        uint64 l_ItemGuid = l_Item->GetGUID();
                        uint64 l_PlayerGuid = l_Player->GetGUID();

                        l_Squirrel->AddDelayedEvent([l_Squirrel, l_TargetGuid, l_ItemGuid, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = Player::GetPlayer(*l_Squirrel, l_PlayerGuid))
                            {
                                if (Item* l_Item = l_Player->GetItemByGuid(l_ItemGuid))
                                {
                                    if (Unit* l_Target = Unit::GetUnit(*l_Squirrel, l_TargetGuid))
                                        l_Squirrel->CastSpell(l_Target, eSpells::SquirrelExplosion, true, l_Item);
                                }
                            }

                            static_cast<Creature*>(l_Squirrel)->DisappearAndDie();
                        }, 2 * IN_MILLISECONDS);
                    }
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_mechanical_bomb_squirrel_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {

            return new spell_item_mechanical_bomb_squirrel_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Volatile Ichor - 222187
class spell_item_volatile_ichor : public SpellScriptLoader
{
    public:
        spell_item_volatile_ichor() : SpellScriptLoader("spell_item_volatile_ichor") { }

        class spell_item_volatile_ichor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_volatile_ichor_AuraScript);

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo || !l_Caster)
                    return;

                Unit* l_Victim = l_DamageInfo->GetTarget();
                if (!l_Victim)
                    return;

                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::VolatileIchor) = l_Victim->GetGUID();
            }


            void Register() override
            {
                OnProc += AuraProcFn(spell_item_volatile_ichor_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_volatile_ichor_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Feed on the Weak - 214224
class spell_item_writhing_heart_of_darkness : public SpellScriptLoader
{
    public:
        spell_item_writhing_heart_of_darkness() : SpellScriptLoader("spell_item_writhing_heart_of_darkness") { }

        class spell_item_writhing_heart_of_darkness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_writhing_heart_of_darkness_AuraScript);

            enum eSpells
            {
                FeedOnTheWeak = 214229
            };

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcEventInfo)
            {
                PreventDefaultAction();

                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura || !l_DamageInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Player || !l_Target)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                if (p_ProcEventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
                    l_Caster->CastSpell(l_Target, eSpells::FeedOnTheWeak, true, l_Item);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_writhing_heart_of_darkness_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_writhing_heart_of_darkness_AuraScript();
        }
};

/// Called by Lethal On Board - 227389, The Coin - 227395, Top Decking - 227396, Full Hand - 227397
/// Called for Eyasu's Mulligan (item) - 141584
class spell_item_opening_hand_buff : public SpellScriptLoader
{
    public:
        spell_item_opening_hand_buff() : SpellScriptLoader("spell_item_opening_hand_buff") { }

        class spell_item_opening_hand_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_opening_hand_buff_AuraScript);

            enum eItems
            {
                EyasusMulligan = 141584
            };

            enum eSpells
            {
                OpeningHand     = 227388,

                CritRoll        = 227389,
                HasteRoll       = 227395,
                MasteryRoll     = 227396,
                VersatilityRoll = 227397,

                CritBuff        = 227390,
                HasteBuff       = 227392,
                MasteryBuff     = 227393,
                VersatilityBuff = 227394
            };

            std::map<uint32, uint32> m_BuffForRoll =
            {
                { CritRoll, CritBuff },
                { HasteRoll, HasteBuff },
                { MasteryRoll, MasteryBuff },
                { VersatilityRoll, VersatilityBuff }
            };

            void HandleAfterEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = p_AurEff->GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (m_BuffForRoll.find(l_SpellInfo->Id) == m_BuffForRoll.end())
                    return;

                int32 l_Amount = p_AurEff->GetAmount();

                l_Caster->CastCustomSpell(l_Caster, m_BuffForRoll[l_SpellInfo->Id], &l_Amount, nullptr, nullptr, true);
                l_Player->AddSpellCooldown(eSpells::OpeningHand, eItems::EyasusMulligan, 120000, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_opening_hand_buff_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_opening_hand_buff_AuraScript();
        }
};

/// 7.3.5 - Build 26365
/// Called for Eyasu's Mulligan item - 141584
class PlayerScript_eyasus_mulligan : public PlayerScript
{
    public:
        PlayerScript_eyasus_mulligan() :PlayerScript("PlayerScript_eyasus_mulligan") {}

        enum eItems
        {
            EyasusMulligan = 141584
        };

        enum eSpells
        {
            OpeningHand     = 227388,

            CritRoll        = 227389,
            HasteRoll       = 227395,
            MasteryRoll     = 227396,
            VersatilityRoll = 227397,

            CritBuff        = 227390,
            HasteBuff       = 227392,
            MasteryBuff     = 227393,
            VersatilityBuff = 227394
        };

        void OnAfterUnequipItem(Player* p_Player, Item const* p_Item) override
        {
            if (!p_Player || !p_Item || p_Item->GetEntry() != eItems::EyasusMulligan)
                return;

            constexpr std::array<int, 9> m_SpellsToRemove =
            {{
                eSpells::OpeningHand,
                eSpells::CritRoll, eSpells::HasteRoll, eSpells::MasteryRoll, eSpells::VersatilityRoll,
                eSpells::CritBuff, eSpells::HasteBuff, eSpells::MasteryBuff, eSpells::VersatilityBuff
            }};

            for (auto l_SpellId : m_SpellsToRemove)
                p_Player->RemoveAura(l_SpellId);
        }
};

/// last update 7.1.5
/// Deadly Grace - 188091
class spell_item_deadly_grace : public SpellScriptLoader
{
    public:
        spell_item_deadly_grace() : SpellScriptLoader("spell_item_deadly_grace") { }

        class spell_item_deadly_grace_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_deadly_grace_SpellScript);

            void SelectTarget(WorldObject*& p_Target)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->GetTargetGUID());
            }

            void Register() override
            {
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_item_deadly_grace_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_deadly_grace_SpellScript();
        }
};

/// last update 7.1.5
/// Darkening Soul - 188091
class spell_item_darkening_soul : public SpellScriptLoader
{
    public:
        spell_item_darkening_soul() : SpellScriptLoader("spell_item_darkening_soul") { }

        class spell_item_darkening_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_darkening_soul_AuraScript);

            enum eSpells
            {
                DarkeningSoul = 222209
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Aura* l_Aura = GetAura();
                Unit* l_Target = p_EventInfo.GetActor();
                Player* l_Player = l_Caster->ToPlayer();

                if (!l_Target || !l_Player || !l_Aura)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Player->CastSpell(l_Target, eSpells::DarkeningSoul, true, l_Item);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_darkening_soul_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_darkening_soul_AuraScript();
        }
};

/// last update 7.1.5 - Build 23420
/// Darkening Soul - 222209
class spell_item_darkening_soul_damage_reduction : public SpellScriptLoader
{
    public:
        spell_item_darkening_soul_damage_reduction() : SpellScriptLoader("spell_item_darkening_soul_damage_reduction") { }

        class spell_item_darkening_soul_damage_reduction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_darkening_soul_damage_reduction_AuraScript);

            bool HandleDontRefresh(bool p_Decrease)
            {
                return false;
            }

            void Register() override
            {
                CanRefreshProc += AuraCanRefreshProcFn(spell_item_darkening_soul_damage_reduction_AuraScript::HandleDontRefresh);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_darkening_soul_damage_reduction_AuraScript();
        }
};

/// Spirit Realm - 188023
class spell_item_spirit_realm : public SpellScriptLoader
{
    public:
        spell_item_spirit_realm() : SpellScriptLoader("spell_item_spirit_realm") { }

        class spell_item_spirit_realm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_spirit_realm_AuraScript);

            enum eItem
            {
                Skaggldrynk = 127840
            };

            enum eSpell
            {
                SpiritRealm = 188023
            };

            void HandleAfterEffectApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetCaster()->IsPlayer())
                    return;

                uint64 l_Guid = p_AurEff->GetBase()->GetCastItemGUID();
                if (!l_Guid)
                    return;

                GetCaster()->ToPlayer()->SendItemExpirePurchaseRefund(l_Guid);
            }

            void HandleAfterEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetCaster()->IsPlayer())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();

                l_Player->SendCooldownEvent(eSpell::SpiritRealm);
                l_Player->AddSpellCooldown(eSpell::SpiritRealm, eItem::Skaggldrynk, 10 * TimeConstants::IN_MILLISECONDS, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_item_spirit_realm_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_spirit_realm_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_spirit_realm_AuraScript();
        }
};

/// Leytorrent Potion - 188030
class spell_item_leytorrent_potion : public SpellScriptLoader
{
    public:
        spell_item_leytorrent_potion() : SpellScriptLoader("spell_item_leytorrent_potion") { }

        class spell_item_leytorrent_potion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_leytorrent_potion_AuraScript);

            enum eItem
            {
                ItemLeytorrentPotion = 127846
            };

            enum eSpell
            {
                SpellLeytorrentPotion = 188030
            };

            void HandleAfterEffectApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetCaster()->IsPlayer())
                    return;

                uint64 l_Guid = p_AurEff->GetBase()->GetCastItemGUID();
                if (!l_Guid)
                    return;

                GetCaster()->ToPlayer()->SendItemExpirePurchaseRefund(l_Guid);
            }

            void HandleAfterEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetCaster()->IsPlayer())
                    return;

                Player * l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->isInCombat())
                {
                    l_Player->SendCooldownEvent(eSpell::SpellLeytorrentPotion);
                    l_Player->AddSpellCooldown(eSpell::SpellLeytorrentPotion, eItem::ItemLeytorrentPotion, 60 * TimeConstants::IN_MILLISECONDS, true);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_item_leytorrent_potion_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_leytorrent_potion_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_leytorrent_potion_AuraScript();
        }
};

class PlayerScript_leytorrent_potion : public PlayerScript
{
    public:
        PlayerScript_leytorrent_potion() : PlayerScript("PlayerScript_leytorrent_potion") { }

        enum eItem
        {
            ItemLeytorrentPotion = 127846
        };

        enum eSpell
        {
            SpellLeytorrentPotion = 188030
        };

        void OnLeaveCombat(Player* p_Player)
        {
            /// Leytorrent Potion is usable one time in combat, then cooldown start when leaving combat
            if (p_Player->GetItemByEntry(eItem::ItemLeytorrentPotion))
            {
                p_Player->SendCooldownEvent(eSpell::SpellLeytorrentPotion);
                p_Player->AddSpellCooldown(eSpell::SpellLeytorrentPotion, eItem::ItemLeytorrentPotion, 60 * TimeConstants::IN_MILLISECONDS, true);
            }
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Nightmarish Ichor - 222015
class spell_item_nightmarish_ichor : public SpellScriptLoader
{
    public:
        spell_item_nightmarish_ichor() : SpellScriptLoader("spell_item_nightmarish_ichor") { }

        class spell_item_nightmarish_ichor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nightmarish_ichor_AuraScript);

            enum eSpells
            {
                NightmarishIchorBuff    = 222027,
                NightmarishIchorIchor   = 222023
            };

            bool HandleCheckProc(ProcEventInfo& /* p_ProcEventInfo */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->IsActiveSpecTankSpec())
                    return false;

                Aura* l_Aura = l_Caster->GetAura(eSpells::NightmarishIchorBuff);
                if (l_Aura && l_Aura->GetDuration() > l_Aura->GetMaxDuration() / 2)
                    return false;

                else if (roll_chance_i(10) && !l_Caster->FindNearestAreaTrigger(eSpells::NightmarishIchorIchor, 20.0f)) ///< Proc Chance not specify in spellwork
                    return true;

                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_nightmarish_ichor_AuraScript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nightmarish_ichor_AuraScript();
        }
};

/// Last Update: 7.1.5 build 23420
/// Called by Raging Storm - 215296
class spell_item_raging_storm : public SpellScriptLoader
{
    public:
        spell_item_raging_storm() : SpellScriptLoader("spell_monk_raging_storm") { }

        class spell_item_raging_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_raging_storm_SpellScript);

            enum eSpells
            {
                RagingStorm     = 215293,
                RagingStormBuff = 215294
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::RagingStorm, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::RagingStormBuff);
                if (!l_Aura)
                {
                    l_AuraEffect->SetAmount(0);
                    return;
                }

                l_AuraEffect->SetAmount(l_Aura->GetStackAmount());
                l_Caster->RemoveAura(eSpells::RagingStormBuff);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_raging_storm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_raging_storm_SpellScript();
        }
};

/// Last Update: 7.1.5 build 23420
/// Called by Raging Storm - 215314
class spell_item_raging_storm_damage : public SpellScriptLoader
{
    public:
        spell_item_raging_storm_damage() : SpellScriptLoader("spell_monk_raging_storm_damage") { }

        class spell_item_raging_storm_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_raging_storm_damage_SpellScript);

            enum eSpells
            {
                RagingStorm     = 215293,
                RagingStormBuff = 215296
            };

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                AuraEffect* l_AuraEffect0 = l_Caster->GetAuraEffect(eSpells::RagingStorm, EFFECT_0);
                AuraEffect* l_AuraEffect1 = l_Caster->GetAuraEffect(eSpells::RagingStormBuff, EFFECT_1);
                if (!l_AuraEffect0 || !l_AuraEffect1)
                    return;

                SetHitHeal(l_AuraEffect0->GetAmount() * l_AuraEffect1->GetAmount());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_raging_storm_damage_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_raging_storm_damage_SpellScript();
        }
};

/// Last Update: 7.1.5 build 23420
/// Called by Raging Storm - 215630
class spell_item_stormsinger : public SpellScriptLoader
{
    public:
        spell_item_stormsinger() : SpellScriptLoader("spell_item_stormsinger") { }

        class spell_item_stormsinger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_stormsinger_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->getClass() == CLASS_DEMON_HUNTER)
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_stormsinger_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_stormsinger_SpellScript();
        }
};

/// Last Update: 7.1.5 build 23420
/// Called by Raging Storm 215632
class spell_item_stormsinger_bonus : public SpellScriptLoader
{
    public:
        spell_item_stormsinger_bonus() : SpellScriptLoader("spell_item_stormsinger_bonus") { }

        class spell_item_stormsinger_bonus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_stormsinger_bonus_AuraScript);

            bool m_Time = false;

            void HandlePeriodicEffect(AuraEffect const* p_AuraEffect)
            {
                PreventDefaultAction();
                if (!m_Time && GetStackAmount() >= 10)
                    m_Time = true;

                Aura* l_Aura = GetAura();
                if (!l_Aura || !m_Time)
                    return;

                l_Aura->DropStack();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_stormsinger_bonus_AuraScript::HandlePeriodicEffect, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_stormsinger_bonus_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Expel Light - 214200
/// Called for Mote of Sanctification (item) - 133646
class spell_item_mote_of_sanctification : public SpellScriptLoader
{
    public:
        spell_item_mote_of_sanctification() : SpellScriptLoader("spell_item_mote_of_sanctification") { }

        class spell_item_mote_of_sanctification_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_mote_of_sanctification_SpellScript);

            enum eSpells
            {
                ExpelLight = 214198
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ExpelLight);
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::HealthPctOrderPred());
                if (p_Targets.empty())
                    return;

                p_Targets.resize(l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_mote_of_sanctification_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_mote_of_sanctification_SpellScript();
        }
};

/// Update to Legion 7.1.5 build: 23420
/// Called by tier 18 set:
/// Rogue T18 Subtlety  - 186278 - 186279 Rogue T18 Assassination 186182 - 186277 Rogue T18 Combat - 186288 - 186285
/// Paladin T18 Protection - 185675 - 185677 Paladin T18 Retribution - 185581 - 185648 Paladin T18 Holy - 185545 - 185571
/// Mage T18 Fire - 186167 - 186168  Mage T18 Arcane - 186166 - 186165  Mage T18 Frost 185971 - 185969
/// Monk T18 - 185126 - 185258 - 185398 - 185399 - 185542 - 185543
/// Hunter T18 - 188190 - 188191 - 188198 - 188199 - 188200 - 188201
/// Warrior T18 - 185796 - 185797 - 185798 - 185799 - 185800 - 185804
/// Druid T18 - 185294 - 185397 - 185805 - 185811 - 185813 - 185814 - 187875 - 187877
/// Shaman T18 - 185871 - 185872 - 185873 - 185874 - 185880 - 185881
/// Warlock T18 - 185882 - 185883 - 185884 - 185964 - 185965 - 185967
/// Priest T18 - 186298 - 186378 - 186477 - 186492 - 186980 - 186981
/// Death Knight T18 - 187865 - 187866 - 187868 - 187870 - 187872 - 187873
class spell_item_tier_set : public SpellScriptLoader
{
    public:
        spell_item_tier_set() : SpellScriptLoader("spell_item_tier_set") { }

        class spell_item_tier_set_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_tier_set_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->getLevel() > 105)
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_tier_set_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_tier_set_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Infernal Alchemist Stone - 188026
class spell_item_infernal_alchemist_stone : public SpellScriptLoader
{
public:
    spell_item_infernal_alchemist_stone() : SpellScriptLoader("spell_item_infernal_alchemist_stone")
    {}

    enum eSpells
    {
        AgilityModifier = 60233,
        StrengthModifier = 60229,
        IntellectModifier = 60234
    };

    class spell_item_infernal_alchemist_stone_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_infernal_alchemist_stone_AuraScript);

        void HandleOnProc(ProcEventInfo& p_Proc)
        {
            if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                return;

            if (AuraEffect* l_AurEff = GetCaster()->GetAuraEffect(GetId(), EFFECT_0))
            {
                int32 l_Amount = l_AurEff->GetAmount();
                Item* l_Item = GetCaster()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

                switch (GetCaster()->ToPlayer()->GetPrimaryStat()) {
                    case Stats::STAT_STRENGTH:
                    {
                        GetCaster()->CastCustomSpell(eSpells::StrengthModifier, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true, l_Item);
                        break;
                    }
                    case Stats::STAT_AGILITY:
                    {
                        GetCaster()->CastCustomSpell(eSpells::AgilityModifier, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true, l_Item);
                        break;
                    }
                    case Stats::STAT_INTELLECT:
                    {
                        GetCaster()->CastCustomSpell(eSpells::IntellectModifier, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true, l_Item);
                        break;
                    }
                    default: break;
                    }
            }
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_item_infernal_alchemist_stone_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_infernal_alchemist_stone_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Spirit Flask - 188116
class spell_item_spirit_flask : public SpellScriptLoader
{
    public:
        spell_item_spirit_flask() : SpellScriptLoader("spell_item_spirit_flask") { }

        class spell_item_spirit_flask_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_spirit_flask_SpellScript);

            enum eSpells
            {
                FlaskOfTheTenThousandScars  = 188035,
                FlaskOfTheCountlessArmies   = 188034,
                FlaskOfTheSeventhDemon      = 188033,
                FlaskOfTheWhisperedPact     = 188031
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                {
                    l_Player->CastSpell(l_Player, eSpells::FlaskOfTheTenThousandScars, true);
                    return;
                }

                switch (l_Player->GetPrimaryStat())
                {
                    case Stats::STAT_STRENGTH:
                    {
                        l_Player->CastSpell(l_Player, eSpells::FlaskOfTheCountlessArmies, true);
                        break;
                    }
                    case Stats::STAT_AGILITY:
                    {
                        l_Player->CastSpell(l_Player, eSpells::FlaskOfTheSeventhDemon, true);
                        break;
                    }
                    case Stats::STAT_INTELLECT:
                    {
                        l_Player->CastSpell(l_Player, eSpells::FlaskOfTheWhisperedPact, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_spirit_flask_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_spirit_flask_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Flesh to Stone - 127249
class spell_item_flesh_to_stone : public SpellScriptLoader
{
    public:
        spell_item_flesh_to_stone() : SpellScriptLoader("spell_item_flesh_to_stone") { }

        class spell_item_flesh_to_stone_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_item_flesh_to_stone_Aurascript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveGameObject(m_scriptSpellId, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_flesh_to_stone_Aurascript::OnRemove, EFFECT_2, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_flesh_to_stone_Aurascript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Spirit Fragment - 221873
class spell_item_spirit_fragment : public SpellScriptLoader
{
    public:
        spell_item_spirit_fragment() : SpellScriptLoader("spell_item_spirit_fragment") { }

        class spell_item_spirit_fragment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_spirit_fragment_AuraScript);

            enum eSpells
            {
                SpellLifeSteal = 146347
            };

            bool CheckOnProc(ProcEventInfo& p_EventInfo)
            {
                if (p_EventInfo.GetDamageInfo())
                {
                    SpellInfo const* l_SpellInfo = p_EventInfo.GetDamageInfo()->GetSpellInfo();

                        if (!l_SpellInfo || l_SpellInfo->Id == eSpells::SpellLifeSteal)
                            return false;

                        return true;
                }
                    return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_spirit_fragment_AuraScript::CheckOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_spirit_fragment_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Warlord's Fortitude - 214624
/// Item Parjesh's Medallion - 137362
class spell_item_warlords_fortitude : public SpellScriptLoader
{
    public:
        spell_item_warlords_fortitude() : SpellScriptLoader("spell_item_warlords_fortitude") { }

        class spell_item_warlords_fortitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_warlords_fortitude_AuraScript);

            enum eSpells
            {
                WarlordsFortitude = 214622
            };

            void CalculateAmountHealth(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpells::WarlordsFortitude, SpellEffIndex::EFFECT_1))
                        p_Amount = l_AurEff->GetAmount();
                }
            }

            void CalculateAmountMastery(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Unit* l_Target = GetUnitOwner())
                {
                    if (AuraEffect* l_AurEff = l_Target->GetAuraEffect(eSpells::WarlordsFortitude, SpellEffIndex::EFFECT_3))
                        p_Amount = l_AurEff->GetAmount();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_warlords_fortitude_AuraScript::CalculateAmountHealth, SpellEffIndex::EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_2);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_warlords_fortitude_AuraScript::CalculateAmountMastery, SpellEffIndex::EFFECT_1, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_warlords_fortitude_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Dinner Bell - 230101
/// Item Majordomo's Dinner Bell - 142168
class spell_item_dinner_bell : public SpellScriptLoader
{
    public:
        spell_item_dinner_bell() : SpellScriptLoader("spell_item_dinner_bell") { }

        class spell_item_dinner_bell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_dinner_bell_SpellScript);

            enum eSpells
            {
                QuiteSatisfiedCrit      = 230102,
                QuiteSatisfiedHaste     = 230103,
                QuiteSatisfiedMastery   = 230104,
                QuiteSatisfiedVersality = 230105
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                const size_t l_Count = 4;
                std::array<uint32, l_Count> l_Spells =
                {{
                    QuiteSatisfiedCrit,
                    QuiteSatisfiedHaste,
                    QuiteSatisfiedMastery,
                    QuiteSatisfiedVersality
                }};

                l_Caster->CastSpell(l_Caster, l_Spells[urand(0, l_Count - 1)], true, GetSpell()->m_CastItem);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_item_dinner_bell_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_dinner_bell_SpellScript();
        }
};

/// 7.3.5 - Build 26365
/// Called for Item Majordomo's Dinner Bell - 142168
class PlayerScript_dinner_bell : public PlayerScript
{
    public:
        PlayerScript_dinner_bell() :PlayerScript("PlayerScript_dinner_bell") {}

        enum eItems
        {
            DinnerBell = 142168
        };

        enum eSpells
        {
            QuiteSatisfiedCrit      = 230102,
            QuiteSatisfiedHaste     = 230103,
            QuiteSatisfiedMastery   = 230104,
            QuiteSatisfiedVersality = 230105
        };

        void OnAfterUnequipItem(Player* p_Player, Item const* p_Item) override
        {
            if (!p_Player || !p_Item || p_Item->GetEntry() != eItems::DinnerBell)
                return;

            constexpr std::array<int, 4> m_SpellsToRemove = { { QuiteSatisfiedCrit, QuiteSatisfiedHaste, QuiteSatisfiedMastery, QuiteSatisfiedVersality } };

            for (auto l_SpellId : m_SpellsToRemove)
                p_Player->RemoveAura(l_SpellId);
        }
};

/// Last Update 7.1.5 Build 23420
/// Masquerade - 215751
/// Item: Essence Swapper - 138393
class spell_item_particle_arranger : public SpellScriptLoader
{
    public:

        spell_item_particle_arranger() : SpellScriptLoader("spell_item_particle_arranger") { }

        class spell_item_particle_arranger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_particle_arranger_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->GetActiveSpecializationID() != SpecIndex::SPEC_HUNTER_BEASTMASTERY)
                    return SpellCastResult::SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target)
                    return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                if (l_Player->GetPetGUID() != l_Target->GetGUID() ||
                    l_Target->HasAuraType(AuraType::SPELL_AURA_TRANSFORM))
                    return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_particle_arranger_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_particle_arranger_SpellScript();
        }

        class spell_item_particle_arranger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_particle_arranger_AuraScript);


            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    l_Player->SetCharacterWorldState(CharacterWorldStates::EssenceSwapperState, l_Target->GetDisplayId());
                    l_Player->SetCharacterWorldState(CharacterWorldStates::EssenceSwapperEntry, l_Target->GetEntry());
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_particle_arranger_AuraScript::HandleAfterEffectRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_particle_arranger_AuraScript();
        }
};

class spell_item_upgrade_class_hall_armor : public SpellScriptLoader
{
    public:
        spell_item_upgrade_class_hall_armor() : SpellScriptLoader("spell_item_upgrade_class_hall_armor") { }

        class spell_item_upgrade_class_hall_armor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_upgrade_class_hall_armor_SpellScript);

            enum eItemsIDs
            {
                /// Death Knight
                DKChest         = 139673,
                DKHands         = 139675,
                DKLegs          = 139677,
                DKWaist         = 139679,
                DKFeet          = 139674,
                DKHead          = 139676,
                DKShoulder      = 139678,
                DKWrist         = 139680,

                /// Demon Hunter
                DHChest         = 139715,
                DHHands         = 139717,
                DHLegs          = 139719,
                DHWaist         = 139721,
                DHFeet          = 139716,
                DHHead          = 139718,
                DHShoulder      = 139720,
                DHWrist         = 139722,

                /// Druid
                DruidChest      = 139723,
                DruidHands      = 139725,
                DruidLegs       = 139727,
                DruidWaist      = 139729,
                DruidFeet       = 139724,
                DruidHead       = 139726,
                DruidShoulder   = 139728,
                DruidWrist      = 139730,

                /// Hunter
                HunterChest     = 139707,
                HunterHands     = 139709,
                HunterLegs      = 139711,
                HunterWaist     = 139713,
                HunterFeet      = 139708,
                HunterHead      = 139710,
                HunterShoulder  = 139712,
                HunterWrist     = 139714,

                /// Mage
                MageChest       = 139751,
                MageHands       = 139748,
                MageLegs        = 139750,
                MageWaist       = 139753,
                MageFeet        = 139747,
                MageHead        = 139749,
                MageShoulder    = 139752,
                MageWrist       = 139754,

                /// Monk
                MonkChest       = 139731,
                MonkHands       = 139733,
                MonkLegs        = 139735,
                MonkWaist       = 139737,
                MonkFeet        = 139732,
                MonkHead        = 139734,
                MonkShoulder    = 139736,
                MonkWrist       = 139738,

                /// Paladin
                PalaChest       = 139690,
                PalaHands       = 139692,
                PalaLegs        = 139694,
                PalaWaist       = 139696,
                PalaFeet        = 139691,
                PalaHead        = 139693,
                PalaShoulder    = 139695,
                PalaWrist       = 139697,

                /// Priest
                PriestChest     = 139759,
                PriestHands     = 139756,
                PriestLegs      = 139758,
                PriestWaist     = 139761,
                PriestFeet      = 139755,
                PriestHead      = 139757,
                PriestShoulder  = 139760,
                PriestWrist     = 139762,

                /// Rogue
                RogueChest      = 139739,
                RogueHands      = 139741,
                RogueLegs       = 139743,
                RogueWaist      = 139745,
                RogueFeet       = 139740,
                RogueHead       = 139742,
                RogueShoulder   = 139744,
                RogueWrist      = 139746,

                /// Shaman
                ShamanChest     = 139698,
                ShamanHands     = 139700,
                ShamanLegs      = 139702,
                ShamanWaist     = 139704,
                ShamanFeet      = 139699,
                ShamanHead      = 139701,
                ShamanShoulder  = 139703,
                ShamanWrist     = 139705,

                /// Warlock
                WarlockChest    = 139767,
                WarlockHands    = 139764,
                WarlockLegs     = 139766,
                WarlockWaist    = 139769,
                WarlockFeet     = 139763,
                WarlockHead     = 139765,
                WarlockShoulder = 139768,
                WarlockWrist    = 139770,

                /// Warrior
                WarriorChest    = 139681,
                WarriorHands    = 139683,
                WarriorLegs     = 139685,
                WarriorWaist    = 139687,
                WarriorFeet     = 139682,
                WarriorHead     = 139684,
                WarriorShoulder = 139686,
                WarriorWrist    = 139688
            };

            enum eSpellIDs
            {
                UpgradeTo820    = 225158,
                UpgradeTo830    = 225160,
                UpgradeTo840    = 225161
            };

            enum eBonuses
            {
                iLvl820         = 1482,
                iLvl830         = 1492,
                iLvl840         = 1502
            };

            std::vector<uint32> l_Items =
            {
                eItemsIDs::DKChest, eItemsIDs::DKFeet, eItemsIDs::DKHands, eItemsIDs::DKHead, eItemsIDs::DKLegs, eItemsIDs::DKShoulder, eItemsIDs::DKWaist, eItemsIDs::DKWrist,
                eItemsIDs::DHChest, eItemsIDs::DHFeet, eItemsIDs::DHHands, eItemsIDs::DHHead, eItemsIDs::DHLegs, eItemsIDs::DHShoulder, eItemsIDs::DHWaist, eItemsIDs::DHWrist,
                eItemsIDs::DruidChest, eItemsIDs::DruidFeet, eItemsIDs::DruidHands, eItemsIDs::DruidHead, eItemsIDs::DruidLegs, eItemsIDs::DruidShoulder, eItemsIDs::DruidWaist, eItemsIDs::DruidWrist,
                eItemsIDs::HunterChest, eItemsIDs::HunterFeet, eItemsIDs::HunterHands, eItemsIDs::HunterHead, eItemsIDs::HunterLegs, eItemsIDs::HunterShoulder, eItemsIDs::HunterWaist, eItemsIDs::HunterWrist,
                eItemsIDs::MageChest, eItemsIDs::MageFeet, eItemsIDs::MageHands, eItemsIDs::MageHead, eItemsIDs::MageLegs, eItemsIDs::MageShoulder, eItemsIDs::MageWaist, eItemsIDs::MageWrist,
                eItemsIDs::MonkChest, eItemsIDs::MonkFeet, eItemsIDs::MonkHands, eItemsIDs::MonkHead, eItemsIDs::MonkLegs, eItemsIDs::MonkShoulder, eItemsIDs::MonkWaist, eItemsIDs::MonkWrist,
                eItemsIDs::PalaChest, eItemsIDs::PalaFeet, eItemsIDs::PalaHands, eItemsIDs::PalaHead, eItemsIDs::PalaLegs, eItemsIDs::PalaShoulder, eItemsIDs::PalaWaist, eItemsIDs::PalaWrist,
                eItemsIDs::PriestChest, eItemsIDs::PriestFeet, eItemsIDs::PriestHands, eItemsIDs::PriestHead, eItemsIDs::PriestLegs, eItemsIDs::PriestShoulder, eItemsIDs::PriestWaist, eItemsIDs::PriestWrist,
                eItemsIDs::RogueChest, eItemsIDs::RogueFeet, eItemsIDs::RogueHands, eItemsIDs::RogueHead, eItemsIDs::RogueLegs, eItemsIDs::RogueShoulder, eItemsIDs::RogueWaist, eItemsIDs::RogueWrist,
                eItemsIDs::ShamanChest, eItemsIDs::ShamanFeet, eItemsIDs::ShamanHands, eItemsIDs::ShamanHead, eItemsIDs::ShamanLegs, eItemsIDs::ShamanShoulder, eItemsIDs::ShamanWaist, eItemsIDs::ShamanWrist,
                eItemsIDs::WarlockChest, eItemsIDs::WarlockFeet, eItemsIDs::WarlockHands, eItemsIDs::WarlockHead, eItemsIDs::WarlockLegs, eItemsIDs::WarlockShoulder, eItemsIDs::WarlockWaist, eItemsIDs::WarlockWrist,
                eItemsIDs::WarriorChest, eItemsIDs::WarriorFeet, eItemsIDs::WarriorHands, eItemsIDs::WarriorHead, eItemsIDs::WarriorLegs, eItemsIDs::WarriorShoulder, eItemsIDs::WarriorWaist, eItemsIDs::WarriorWrist
            };

            SpellCastResult HandleCheckCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return SpellCastResult::SPELL_FAILED_ITEM_NOT_FOUND;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return SpellCastResult::SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                std::vector<uint32>::iterator l_Iter = std::find(l_Items.begin(), l_Items.end(), l_Item->GetEntry());
                if (l_Iter == l_Items.end())
                    return SpellCastResult::SPELL_FAILED_ITEM_NOT_FOUND;

                if (l_Item->GetItemLevel(l_Player) != l_SpellInfo->Effects[EFFECT_2].BasePoints)
                    return SpellCastResult::SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                uint32 l_OldBonus;
                uint32 l_NewBonus;

                switch (l_SpellInfo->Effects[EFFECT_2].BasePoints)
                {
                    case 810:
                    {
                        l_OldBonus = 0;
                        l_NewBonus = eBonuses::iLvl820;
                        break;
                    }
                    case 820:
                    {
                        if (l_Item->HasItemBonus(eBonuses::iLvl820))
                            l_OldBonus = eBonuses::iLvl820;
                        else
                            l_OldBonus = 0;

                        if (l_OldBonus != 0)
                            l_NewBonus = eBonuses::iLvl830;
                        else
                            l_NewBonus = eBonuses::iLvl820; ///< Has the background that the bonus 1482 means ilvl + 10. Looks weird but works :P

                        break;
                    }
                    case 830:
                    {
                        if (l_Item->HasItemBonus(eBonuses::iLvl830))
                            l_OldBonus = eBonuses::iLvl830;
                        else if (l_Item->HasItemBonus(eBonuses::iLvl820))
                            l_OldBonus = eBonuses::iLvl820;
                        else
                            l_OldBonus = 0;

                        if (l_OldBonus == eBonuses::iLvl820)
                            l_NewBonus = eBonuses::iLvl830;
                        else
                            l_NewBonus = eBonuses::iLvl840;
                        break;
                    }
                }

                if (l_OldBonus == 0)
                {
                    l_Item->AddItemBonus(l_NewBonus);
                    return;
                }

                l_Item->ModifyItemBonus(l_OldBonus, l_NewBonus);

                // HACK: ModifyItemBonus doesn't change _bonusData
                // RemoveBonus works for ItemLevel bonuses only
                BonusData* l_BonusData = const_cast<BonusData*>(l_Item->GetBonus());

                auto l_OldBonuses = GetItemBonusesByID(l_OldBonus);
                if (l_OldBonuses)
                {
                    for (auto l_Bonus : *l_OldBonuses)
                    {
                        if (l_Bonus == nullptr)
                            continue;

                        l_BonusData->RemoveBonus(l_Bonus->Type, l_Bonus->Value);
                    }
                }

                auto l_NewBonuses = GetItemBonusesByID(l_NewBonus);
                if (l_NewBonuses)
                {

                    for (auto l_Bonus : *l_NewBonuses)
                    {
                        if (l_Bonus == nullptr)
                            continue;

                        l_BonusData->AddBonus(l_Bonus->Type, l_Bonus->Value);
                    }
                }

                l_Item->SetState(ITEM_CHANGED, l_Player);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_upgrade_class_hall_armor_SpellScript::HandleCheckCast);
                AfterCast += SpellCastFn(spell_item_upgrade_class_hall_armor_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_upgrade_class_hall_armor_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Stabilized Energy - 228450
class spell_item_stabilized_energy : public SpellScriptLoader
{
    public:
        spell_item_stabilized_energy() : SpellScriptLoader("spell_item_stabilized_energy") { }

        class spell_item_stabilized_energy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_stabilized_energy_AuraScript);

            enum eSpells
            {
                StabilizedEnergyDruid   = 228451, ///< Mana, Rage, Energy
                StabilizedEnergyWarrior = 228452, ///< Rage
                StabilizedEnergyMage    = 228453, ///< Mana
                StabilizedEnergyHunter  = 228454, ///< Focus
                StabilizedEnergyRogue   = 228455, ///< Energy
                StabilizedEnergyMonk    = 228456, ///< Mana, Energy
                StabilizedEnergyShaman  = 228457, ///< Mana, Maelstorm
                StabilizedEnergyDH      = 228458, ///< Fury, Pain
                StabilizedEnergyDK      = 228459, ///< Runic Power
                StabilizedEnergyPriest  = 228460  ///< Mana, Insanity
            };

            std::array<uint32 const, Classes::MAX_CLASSES> l_AurasForClasses =
            {
                {
                    0,
                    eSpells::StabilizedEnergyWarrior,   ///< CLASS_WARRIOR
                    eSpells::StabilizedEnergyMage,      ///< CLASS_PALADIN
                    eSpells::StabilizedEnergyHunter,    ///< CLASS_HUNTER
                    eSpells::StabilizedEnergyRogue,     ///< CLASS_ROGUE
                    eSpells::StabilizedEnergyPriest,    ///< CLASS_PRIEST
                    eSpells::StabilizedEnergyDK,        ///< CLASS_DEATH_KNIGHT
                    eSpells::StabilizedEnergyShaman,    ///< CLASS_SHAMAN
                    eSpells::StabilizedEnergyMage,      ///< CLASS_MAGE
                    eSpells::StabilizedEnergyMage,      ///< CLASS_WARLOCK
                    eSpells::StabilizedEnergyMonk,      ///< CLASS_MONK
                    eSpells::StabilizedEnergyDruid,     ///< CLASS_DRUID
                    eSpells::StabilizedEnergyDH         ///< CLASS_DEMON_HUNTER
                }
            };

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->getClass() >= Classes::MAX_CLASSES)
                    return;

                if (uint32 l_Spell = l_AurasForClasses[l_Caster->getClass()])
                    l_Caster->AddAura(l_Spell, l_Caster);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->getClass() >= Classes::MAX_CLASSES)
                    return;

                if (uint32 l_Spell = l_AurasForClasses[l_Caster->getClass()])
                    l_Caster->RemoveAura(l_Spell);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_stabilized_energy_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_stabilized_energy_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_stabilized_energy_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fragile Echoes - 215266
class spell_item_fragile_echoes : public SpellScriptLoader
{
    public:
        spell_item_fragile_echoes() : SpellScriptLoader("spell_item_fragile_echoes") { }

        class spell_item_fragile_echoes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_fragile_echoes_AuraScript);

            enum eSpells
            {
                FragileEchoesDruid = 225298
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Item* l_Item = l_Caster->ToPlayer()->GetItemByEntry(136714);
                l_Caster->CastSpell(l_Caster, eSpells::FragileEchoesDruid, true, l_Item);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->RemoveAura(eSpells::FragileEchoesDruid);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_fragile_echoes_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_fragile_echoes_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_fragile_echoes_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fragile Echoes - 225298
class spell_item_fragile_echoes_proc : public SpellScriptLoader
{
    public:
        spell_item_fragile_echoes_proc() : SpellScriptLoader("spell_item_fragile_echoes_proc") { }

        class spell_item_fragile_echoes_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_fragile_echoes_proc_AuraScript);

            enum eSpells
            {
                HealingTouch    = 5185,
                Regrowth        = 8936,
                Effuse          = 116694,
                EnveloppingMist = 124682,
                Heal            = 2060,
                FlashHeal       = 2061,
                PWShield        = 17,
                ShadowMend      = 186263,
                HolyLight       = 82326,
                FlashLight      = 19750,
                HealingSurge    = 8004,
                HealingWave     = 77472,
                FragileEchoes   = 215267
            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::HealingTouch,
                eSpells::Regrowth,
                eSpells::Effuse,
                eSpells::EnveloppingMist,
                eSpells::Heal,
                eSpells::FlashHeal,
                eSpells::PWShield,
                eSpells::ShadowMend,
                eSpells::HolyLight,
                eSpells::FlashLight,
                eSpells::HealingSurge,
                eSpells::HealingWave
            };

            bool HandleProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_SpellInfo || !l_Target)
                    return false;

                if (std::find(m_Spells.begin(), m_Spells.end(), l_SpellInfo->Id) == m_Spells.end())
                    return false;

                if (Aura* l_Aura = l_Target->GetAura(eSpells::FragileEchoes))
                    if (l_Aura->GetCasterGUID() != l_Caster->GetGUID())
                        return false;

                return true;
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_item_fragile_echoes_proc_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_fragile_echoes_proc_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fragile Echo - 215267
class spell_item_fragile_echoes_restore : public SpellScriptLoader
{
    public:
        spell_item_fragile_echoes_restore() : SpellScriptLoader("spell_item_fragile_echoes_restore") { }

        class spell_item_fragile_echoes_restore_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_fragile_echoes_restore_AuraScript);

            enum eSpells
            {
                FragileEchoEnergize = 215270
            };

            void OnRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                int32 l_Amount = p_AurEff->GetAmount();
                l_Caster->CastCustomSpell(eSpells::FragileEchoEnergize, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Amount, l_Caster, true);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_fragile_echoes_restore_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_fragile_echoes_restore_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Called By Sands of Time - 225124
/// Item Royal Dagger Haft - 140791
class spell_item_sands_of_time : public SpellScriptLoader
{
    public:
        spell_item_sands_of_time() : SpellScriptLoader("spell_item_sands_of_time")
        {}

        enum eSpells
        {
            SpellSandsOfTimeDebuff  = 229333,
            SpellSandsOfTimeHeal    = 225720,
            SpellSandsOfTimeDot     = 229457,
        };

        class spell_item_sands_of_time_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_sands_of_time_AuraScript);

            void HandleOnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Victim = p_DmgInfo.GetTarget();

                if (l_Victim == nullptr)
                    return;

                if (p_DmgInfo.GetAmount() >= l_Victim->GetHealth() && !l_Victim->HasAura(eSpells::SpellSandsOfTimeDebuff))
                {
                    p_AbsorbAmount = GetAura()->GetEffect(EFFECT_0)->GetAmount();

                    if (l_Victim->ToPlayer() == nullptr)
                        return;

                    Item* l_Trinket = l_Victim->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

                    l_Victim->CastSpell(l_Victim, eSpells::SpellSandsOfTimeDebuff, true);
                    l_Victim->CastSpell(l_Victim, eSpells::SpellSandsOfTimeHeal, true, l_Trinket);
                    l_Victim->CastCustomSpell(eSpells::SpellSandsOfTimeDot, SPELLVALUE_BASE_POINT0, p_DmgInfo.GetAmount() / 5, l_Victim, true);
                }
                else
                    p_AbsorbAmount = 0;
            }

            void HandleCalculateAmount(AuraEffect const* /**/, int32& p_Amount, bool& /**/)
            {
                p_Amount = -1;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_sands_of_time_AuraScript::HandleCalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_sands_of_time_AuraScript::HandleOnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_sands_of_time_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Fury of the Burning Sky - 140801
/// Called By Solar Collapse - 229746
class spell_item_solar_collapse : public SpellScriptLoader
{
    public:
        spell_item_solar_collapse() : SpellScriptLoader("spell_item_solar_collapse")
        {}

        class spell_item_solar_collapse_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_solar_collapse_AuraScript);

            enum eSpells
            {
                SpellSolarCollapseDmg = 229737
            };

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (!GetUnitOwner() || !GetCaster() || !GetAura())
                    return;

                Player* l_Caster = GetCaster()->ToPlayer();

                if (l_Caster == nullptr)
                    return;

                Item* l_Item = l_Caster->GetItemByGuid(GetAura()->GetCastItemGUID());

                if (l_Item == nullptr)
                    return;

                l_Caster->CastSpell(GetUnitOwner(), eSpells::SpellSolarCollapseDmg, true, l_Item);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_solar_collapse_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_solar_collapse_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Icon of Rot - 1470798
/// Called By Carrion Swarm - 225131
class spell_item_carrion_swarm : public SpellScriptLoader
{
    public:
        spell_item_carrion_swarm() : SpellScriptLoader("spell_item_carrion_swarm")
        {}

        enum eSpells
        {
            SpellCarrionSwarmDmg    =   225731
        };

        class spell_item_carrion_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_carrion_swarm_AuraScript);

            void HandleOnProc(ProcEventInfo& p_Proc)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                if (AuraEffect* l_AurEff = GetCaster()->GetAuraEffect(GetId(), EFFECT_0))
                {
                    int32 l_Dmg = l_AurEff->GetAmount();
                    Item* l_Item = GetCaster()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

                    GetCaster()->CastCustomSpell(eSpells::SpellCarrionSwarmDmg, SPELLVALUE_BASE_POINT0, l_Dmg, GetCaster(), true, l_Item);
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_carrion_swarm_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_carrion_swarm_AuraScript();
        }
};

/// Last update 7.3.5 Build 26365
/// Item 151955 'Acrid Catalyst Injector' From Antorus
/// Called By Cycle of the Legion - 253259
class spell_item_cycle_of_the_legion : public SpellScriptLoader
{
    public:
        spell_item_cycle_of_the_legion() : SpellScriptLoader("spell_item_cycle_of_the_legion") { }

        class spell_item_cycle_of_the_legion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_cycle_of_the_legion_AuraScript);

            enum eSpells
            {
                FervorOfTheLegion       = 253261,
                BrutalityOfTheLegion    = 255742,
                MaliceOfTheLegion       = 255744
            };

            void HandleOnProc(ProcEventInfo& p_Proc)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(m_scriptSpellId);
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Aura || !l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                SpellInfo const* l_CycleLegion = sSpellMgr->GetSpellInfo(m_scriptSpellId);
                if (!l_Item || !l_CycleLegion)
                    return;

                /// Temporary Workaround Kappa (buff values were way out of line, I checked given value and tooltip value, and this gives the correct values (really small difference)
                int32 l_AuraAmount = l_CycleLegion->Effects[EFFECT_1].CalcValue(l_Caster, NULL, NULL, l_Item);
                uint32 l_IlvlSupp = l_Item->GetItemLevel(l_Player) - 930;
                if (l_IlvlSupp > 65)
                    l_IlvlSupp = 65;

                float l_Factor = (2.0f + float(l_IlvlSupp / 20) * 0.3f);
                l_AuraAmount /= l_Factor;

                uint32 l_TriggerId = 0;
                uint32 l_Rand = urand(1, 3);
                switch (l_Rand)
                {
                    case 1:
                        l_TriggerId = eSpells::FervorOfTheLegion;
                        break;
                    case 2:
                        l_TriggerId = eSpells::BrutalityOfTheLegion;
                        break;
                    case 3:
                        l_TriggerId = eSpells::MaliceOfTheLegion;
                        break;
                    default:
                        break;
                }

                if (!l_TriggerId) /// ???
                    return;

                l_Caster->CastCustomSpell(l_Caster, l_TriggerId, &l_AuraAmount, NULL, NULL, true, l_Item);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_cycle_of_the_legion_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_cycle_of_the_legion_AuraScript();
        }
};

/// Last update 7.3.5 Build 26365
/// Called By Fervor Of The Legion - 253261 / Brutality Of The Legion - 255742 / Malice Of The Legion - 255744
class spell_item_buffs_of_the_legion : public SpellScriptLoader
{
    public:
        spell_item_buffs_of_the_legion() : SpellScriptLoader("spell_item_buffs_of_the_legion") { }

        class spell_item_buffs_of_the_legion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_buffs_of_the_legion_AuraScript);

            enum eSpells
            {
                CycleOfTheLegion        = 253259, ///< Base Aura
                FervorOfTheLegion       = 253261,
                BrutalityOfTheLegion    = 255742,
                MaliceOfTheLegion       = 255744,
                CycleOfTheLegionBuff    = 253260 ///< When u have one of the buffs that reaches 5 stacks, it should cast this on the caster
            };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_Aura = l_Caster->GetAura(m_scriptSpellId);
                if (!l_Aura || l_Aura->GetStackAmount() < 5)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                Aura* l_CycleAura = l_Caster->GetAura(eSpells::CycleOfTheLegion);
                SpellInfo const* l_CycleLegion = sSpellMgr->GetSpellInfo(eSpells::CycleOfTheLegion);
                if (!l_Player || !l_CycleAura || !l_CycleLegion)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_CycleAura->GetCastItemGUID());
                if (!l_Item)
                    return;

                /// Temporary Workaround Kappa (buff values were way out of line, I checked given value and tooltip value, and this gives the correct values (really small difference)
                int32 l_AuraAmount = l_CycleLegion->Effects[EFFECT_2].CalcValue(l_Caster, NULL, NULL, l_Item);
                uint32 l_IlvlSupp = l_Item->GetItemLevel(l_Player) - 930;
                if (l_IlvlSupp > 65)
                    l_IlvlSupp = 65;

                float l_Factor = (2.0f + float(l_IlvlSupp / 20) * 0.3f);
                l_AuraAmount /= l_Factor;

                l_Caster->RemoveAura(eSpells::FervorOfTheLegion);
                l_Caster->RemoveAura(eSpells::BrutalityOfTheLegion);
                l_Caster->RemoveAura(eSpells::MaliceOfTheLegion);

                l_Caster->CastCustomSpell(l_Caster, eSpells::CycleOfTheLegionBuff, &l_AuraAmount, &l_AuraAmount, &l_AuraAmount, true, l_Item);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_buffs_of_the_legion_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_RATING, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_buffs_of_the_legion_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Erratic Metronome - 140792
/// Called By Accelerando - 225125
class spell_item_accelerando : public SpellScriptLoader
{
public:
    spell_item_accelerando() : SpellScriptLoader("spell_item_accelerando")
    {}

    enum eSpells
    {
        SpellAccelerandoBuff    = 225719,
    };

    class spell_item_accelerando_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_accelerando_AuraScript);

        bool Load() override
        {
            m_TimerProc = 0;
            m_ProcsPPM  = 0;
            return true;
        }

        void HandleOnAuraUpdate(const uint32 p_Diff)
        {
            m_TimerProc += p_Diff;

            if (m_TimerProc >= 60 * IN_MILLISECONDS)
            {
                m_TimerProc = 0;
                m_ProcsPPM  = 0;
            }
        }

        void HandleOnProc(ProcEventInfo& p_Proc)
        {
            if (m_ProcsPPM >= 11)
                return;

            if (!GetCaster())
                return;

            if (GetCaster()->ToPlayer() == nullptr)
                return;

            Unit* l_Caster = GetCaster();
            Item* l_Item = GetCaster()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

            m_ProcsPPM++;

            if (Aura* l_Aura = l_Caster->GetAura(eSpells::SpellAccelerandoBuff))
            {
                int32 l_Duration = l_Aura->GetDuration();

                if (l_Aura->GetStackAmount() < 5)
                {
                    l_Aura->ModStackAmount(1);
                    l_Aura->SetDuration(l_Duration);
                    l_Aura->SetMaxDuration(l_Duration);
                }
            }
            else
                l_Caster->CastSpell(l_Caster, eSpells::SpellAccelerandoBuff, true, l_Item);
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_item_accelerando_AuraScript::HandleOnAuraUpdate);
            OnProc += AuraProcFn(spell_item_accelerando_AuraScript::HandleOnProc);
        }

        uint32 m_TimerProc;
        uint8 m_ProcsPPM;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_accelerando_AuraScript();
    }
};

/// Last update 7.1.5 Build 23420
/// Item Star Gate - 140804
/// Called By Star Gate - 225137
class spell_item_star_gate : public SpellScriptLoader
{
public:
    spell_item_star_gate() : SpellScriptLoader("spell_item_star_gate")
    {}

    enum eSpells
    {
        SpellNetherMeteor   = 225764,
    };

    class spell_item_star_gate_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_star_gate_AuraScript);

        bool Load() override
        {
            m_TimerProc = 0;
            m_ProcsPPM  = 0;
            return true;
        }

        void HandleOnAuraUpdate(const uint32 p_Diff)
        {
            m_TimerProc += p_Diff;

            if (m_TimerProc >= 60 * IN_MILLISECONDS)
            {
                m_TimerProc = 0;
                m_ProcsPPM  = 0;
            }
        }

        void HandleOnProc(ProcEventInfo& p_Proc)
        {
            if (!GetUnitOwner() || !p_Proc.GetActionTarget())
                return;

            if (!GetUnitOwner()->ToPlayer())
                return;

            if (m_ProcsPPM > 2)
                return;

            m_ProcsPPM++;

            Item* l_Trinket = GetUnitOwner()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

            GetUnitOwner()->CastSpell(p_Proc.GetActionTarget(), eSpells::SpellNetherMeteor, true, l_Trinket);
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_item_star_gate_AuraScript::HandleOnAuraUpdate);
            OnProc += AuraProcFn(spell_item_star_gate_AuraScript::HandleOnProc);
        }

        uint32 m_TimerProc;
        uint8 m_ProcsPPM;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_star_gate_AuraScript();
    }
};

/// Last update 7.1.5 Build 23420
/// Item Claw of Crystalline Scorpid - 1470790
/// Called By Crystalline Shocwave - 225123
class spell_item_crystalline_shockwave : public SpellScriptLoader
{
public:
    spell_item_crystalline_shockwave() : SpellScriptLoader("spell_item_crystalline_shockwave")
    {}

    class spell_item_crystalline_shockwave_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_crystalline_shockwave_AuraScript);

        bool Load() override
        {
            m_TimerProc = 0;
            m_ProcsPPM = 0;
            return true;
        }

        void HandleOnAuraUpdate(const uint32 p_Diff)
        {
            m_TimerProc += p_Diff;

            if (m_TimerProc >= 60 * IN_MILLISECONDS)
            {
                m_TimerProc = 0;
                m_ProcsPPM = 0;
            }
        }

        bool HandleCheckProc(ProcEventInfo& p_Proc)
        {
            if (m_ProcsPPM < 2)
                return true;
            else
                return false;
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_item_crystalline_shockwave_AuraScript::HandleOnAuraUpdate);
            DoCheckProc += AuraCheckProcFn(spell_item_crystalline_shockwave_AuraScript::HandleCheckProc);
        }

        uint32 m_TimerProc;
        uint8 m_ProcsPPM;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_crystalline_shockwave_AuraScript();
    }
};

/// Last update 7.1.5 Build 23420
/// Item Etraeus Constellation Map - 140803
/// Called By Constellations - 225136
class spell_item_constellations : public SpellScriptLoader
{
    public:
        spell_item_constellations() : SpellScriptLoader("spell_item_constellations")
        {}

        enum eSpells
        {
            SpellSignOfTheHyppo  = 225749,
            SpellSignOfTheHare  = 225752,
            SpellSignOfDragon   = 225753,
        };

        class spell_item_constellations_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_constellations_AuraScript);

            void HandleOnProc(ProcEventInfo& p_Proc)
            {
                if (!GetUnitOwner())
                    return;

                uint32 l_Index = urand(0, 2);
                uint32 l_Amount = 0;

                if (AuraEffect* l_AurEff = GetUnitOwner()->GetAuraEffect(GetId(), EFFECT_1))
                    l_Amount += l_AurEff->GetAmount();

                switch (l_Index)
                {
                    case 0:
                    {
                        GetUnitOwner()->CastCustomSpell(eSpells::SpellSignOfTheHyppo, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true);
                        break;
                    }

                    case 1:
                    {
                        GetUnitOwner()->CastCustomSpell(eSpells::SpellSignOfTheHare, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true);
                        break;
                    }

                    case 2:
                    {
                        GetUnitOwner()->CastCustomSpell(eSpells::SpellSignOfDragon, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true);
                        break;
                    }

                    default: break;
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_constellations_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_constellations_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Entwined Elemental Foci - 140796
/// Called By Triumvirate - 225129
class spell_item_triumvirate : public SpellScriptLoader
{
    public:
        spell_item_triumvirate() : SpellScriptLoader("spell_item_triumvirate")
        {}

        enum eSpells
        {
            SpellFieryEnchant   = 225726,
            SpellFrostEnchant   = 225729,
            SpellArcaneEnchant  = 225730,
        };

        class spell_item_triumvirate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_triumvirate_AuraScript);

            void HandleOnProc(ProcEventInfo& /**/)
            {
                Unit* l_Owner = GetUnitOwner();
                Aura* l_Aura = GetAura();
                if (!l_Owner || !l_Aura)
                    return;

                uint64 l_ItemGUID = l_Aura->GetCastItemGUID();
                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_ItemGUID);
                int32 l_Amount = 0;

                if (AuraEffect* l_AurEff = l_Owner->GetAuraEffect(GetId(), EFFECT_1))
                    l_Amount += l_AurEff->GetAmount();

                uint32 l_Index = urand(0, 2);

                switch (l_Index)
                {
                    case 0:
                    {
                        l_Owner->CastCustomSpell(eSpells::SpellFieryEnchant, SPELLVALUE_BASE_POINT0, l_Amount, l_Owner, true, l_Item);
                        break;
                    }
                    case 1:
                    {
                        l_Owner->CastCustomSpell(eSpells::SpellArcaneEnchant, SPELLVALUE_BASE_POINT0, l_Amount, l_Owner, true, l_Item);
                        break;
                    }
                    case 2:
                    {
                        l_Owner->CastCustomSpell(eSpells::SpellFrostEnchant, SPELLVALUE_BASE_POINT0, l_Amount, l_Owner, true, l_Item);
                        break;
                    }
                    default: break;
                }
            }

            void Register()
            {
                OnProc += AuraProcFn(spell_item_triumvirate_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_triumvirate_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Whispers in the Dark - 140809
/// Called By Nefarious Pact - 225774
class spell_item_nefarious_pact : public SpellScriptLoader
{
    public:
        spell_item_nefarious_pact() : SpellScriptLoader("spell_item_nefarious_pact")
        {}

        enum eSpells
        {
            SpellDevilsDue  = 225776,
        };

        class spell_item_nefarious_pact_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nefarious_pact_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (l_Owner->isDead() || l_Owner->ToPlayer() == nullptr)
                    return;

                Item* l_Trinket = l_Owner->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

                l_Owner->CastSpell(l_Owner, eSpells::SpellDevilsDue, true, l_Trinket);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_nefarious_pact_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nefarious_pact_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Ephemeral Paradox - 140805
/// Called By Temporal Shift -  225138
class spell_item_temporal_shift : public SpellScriptLoader
{
    public:
        spell_item_temporal_shift() : SpellScriptLoader("spell_item_temporal_shift")
        {}

        enum eSpells
        {
            SpellTemporalShiftPriestDiscipline  = 225769,
            SpellTemporalShiftPriestHoly        = 225770,
            SpellTemporalShiftDruid             = 225766,
            SpellTemporalShiftMonk              = 225767,
            SpellTemporalShiftPaladin           = 225768,
            SpellTemporalShiftShaman            = 225771,
            SpellTemporalShiftEnergize          = 225772,
        };

        class spell_item_temporal_shift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_temporal_shift_AuraScript);

            void HandleOnProc(ProcEventInfo& /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (GetUnitOwner()->ToPlayer() == nullptr)
                    return;

                Player* l_Owner = GetUnitOwner()->ToPlayer();
                Item* l_Trinket = l_Owner->GetItemByGuid(GetAura()->GetCastItemGUID());

                switch (l_Owner->GetActiveSpecializationID())
                {
                    case SpecIndex::SPEC_PRIEST_DISCIPLINE:
                    {
                        l_Owner->CastSpell(l_Owner, eSpells::SpellTemporalShiftPriestDiscipline, true, l_Trinket);
                        break;
                    }

                    case SpecIndex::SPEC_PRIEST_HOLY:
                    {
                        l_Owner->CastSpell(l_Owner, eSpells::SpellTemporalShiftPriestHoly, true, l_Trinket);
                        break;
                    }

                    case SpecIndex::SPEC_DRUID_RESTORATION:
                    {
                        l_Owner->CastSpell(l_Owner, eSpells::SpellTemporalShiftDruid, true, l_Trinket);
                        break;
                    }

                    case SpecIndex::SPEC_SHAMAN_RESTORATION:
                    {
                        l_Owner->CastSpell(l_Owner, eSpells::SpellTemporalShiftShaman, true, l_Trinket);
                        break;
                    }

                    case SpecIndex::SPEC_PALADIN_HOLY:
                    {
                        l_Owner->CastSpell(l_Owner, eSpells::SpellTemporalShiftPaladin, true, l_Trinket);
                        break;
                    }

                    case SpecIndex::SPEC_MONK_MISTWEAVER:
                    {
                        l_Owner->CastSpell(l_Owner, eSpells::SpellTemporalShiftMonk, true, l_Trinket);
                        break;
                    }

                    default: break;
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_temporal_shift_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_temporal_shift_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Ephemeral Paradox - 140805
/// Called By Temporal Shift -  225766, 225767, 225768, 225769, 225770, 225771
class spell_item_temporal_shift_buff : public SpellScriptLoader
{
    public:
        spell_item_temporal_shift_buff() : SpellScriptLoader("spell_item_temporal_shift_buff")
        {}

        enum eSpells
        {
            SpellTemporalShiftEnergize  = 225772,
            SpellTemporalShiftItemAura  = 225138
        };

        class spell_item_temporal_shift_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_temporal_shift_buff_AuraScript);

            void HandleOnProc(ProcEventInfo& /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();
                int32 l_Amount = 0;

                if (AuraEffect* l_AurEff = l_Owner->GetAuraEffect(SpellTemporalShiftItemAura, EFFECT_1))
                    l_Amount += l_AurEff->GetAmount();

                l_Owner->CastCustomSpell(eSpells::SpellTemporalShiftEnergize, SPELLVALUE_BASE_POINT0, l_Amount, l_Owner, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_temporal_shift_buff_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_temporal_shift_buff_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Animated Exoskeleton - 140789
/// Called By Living Carapace - 225033
class spell_item_living_carapace : public SpellScriptLoader
{
    public:
        spell_item_living_carapace() : SpellScriptLoader("spell_item_living_carapace")
        {}

        class spell_item_living_carapace_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_living_carapace_AuraScript);

            void HandleOnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                float l_Pct = GetSpellInfo()->Effects[EFFECT_1].BasePoints / 100.f;

                p_AbsorbAmount = p_DmgInfo.GetAmount() * l_Pct;
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_living_carapace_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_living_carapace_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Infernal Contract - 140807
/// Called By Infernal Contract - 225140
class spell_item_infernal_contract : public SpellScriptLoader
{
    public:
        spell_item_infernal_contract() : SpellScriptLoader("spell_item_infernal_contract")
        {}

        enum eSpells
        {
            SpellBacklash   = 225778
        };

        class spell_item_infernal_contract_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_infernal_contract_AuraScript);

            void HandleOnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (GetUnitOwner() == nullptr || GetSpellInfo() == nullptr)
                    return;

                float l_Pct = GetSpellInfo()->Effects[EFFECT_1].BasePoints / 100.f;

                p_AbsorbAmount = p_DmgInfo.GetAmount() * l_Pct;
            }

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (l_Owner->isDead() || l_Owner->ToPlayer() == nullptr)
                    return;

                Item* l_Trinket = l_Owner->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());

                l_Owner->CastSpell(l_Owner, eSpells::SpellBacklash, true, l_Trinket);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_infernal_contract_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_infernal_contract_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_infernal_contract_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Infernal Contract - 140807
/// Called By Backlash - 225778
class spell_item_backlash : public SpellScriptLoader
{
    public:
        spell_item_backlash() : SpellScriptLoader("spell_item_backlash")
        {}

        enum eSpells
        {
            SpellBacklashDmg    = 229528
        };

        class spell_item_backlash_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_backlash_AuraScript);

            bool Load() override
            {
                l_CurrDmg = 0;
                return true;
            }

            void HandleOnProc(ProcEventInfo& p_Proc)
            {
                if (p_Proc.GetDamageInfo() == nullptr || GetUnitOwner() == nullptr || GetSpellInfo() == nullptr)
                    return;


                Unit* l_Owner = GetUnitOwner();
                float l_Pct = GetSpellInfo()->Effects[EFFECT_0].BasePoints / 100.f;

                uint32 l_Dmg = p_Proc.GetDamageInfo()->GetAmount() * l_Pct;

                if (AuraEffect* l_AurEff = GetAura()->GetEffect(EFFECT_1))
                {
                    if (l_AurEff->GetAmount() < l_CurrDmg)
                    {
                        Remove();
                        return;
                    }
                }

                l_CurrDmg += l_Dmg;
                l_Owner->CastCustomSpell(eSpells::SpellBacklashDmg, SPELLVALUE_BASE_POINT0, l_Dmg, l_Owner, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_backlash_AuraScript::HandleOnProc);
            }

            uint32 l_CurrDmg;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_backlash_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Called by Plague Swarm - 221811 For Swarming Plaguehive - 139321
class spell_item_plague_swarm : public SpellScriptLoader
{
    public:
        spell_item_plague_swarm() : SpellScriptLoader("spell_item_plague_swarm") { }

        class spell_item_plague_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_plague_swarm_AuraScript);

            bool CheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetProcTarget();
                if (!l_Caster || !l_Target)
                    return false;

                if (!l_Caster->IsValidAttackTarget(l_Target))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_plague_swarm_AuraScript::CheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_plague_swarm_AuraScript();
        }
};

/// Last update 7.1.5 Build 23420
/// Called by Recursive Strike - 225736 For Nighthold Trinket Nightblooming Frond - 140802
class spell_item_recursive_strike : public SpellScriptLoader
{
public:
    spell_item_recursive_strike() : SpellScriptLoader("spell_item_recursive_strike") { }

    class spell_item_recursive_strike_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_recursive_strike_AuraScript);

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (Aura* l_Aura = GetAura())
            {
                uint32 l_Stacks = l_Aura->GetStackAmount();
                if (l_Stacks < GetSpellInfo()->StackAmount)
                    l_Aura->SetStackAmount(l_Stacks + 1);
            }
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_item_recursive_strike_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_recursive_strike_AuraScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Recursive Strike Damage - 225739 for Nighthold Trinket Nightblooming Frond - 140802
class spell_item_recursive_strike_damage : public SpellScriptLoader
{
public:
    spell_item_recursive_strike_damage() : SpellScriptLoader("spell_item_recursive_strike_damage") { }

    class spell_item_recursive_strike_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_recursive_strike_damage_SpellScript);

        enum eSpells
        {
            RecursiveStrike = 225736
        };

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Aura* l_Aura = l_Caster->GetAura(eSpells::RecursiveStrike);
            if (!l_Aura)
                return;

            uint8 l_Stack = l_Aura->GetStackAmount() - 1;
            SetHitDamage(GetHitDamage() * l_Stack);
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_item_recursive_strike_damage_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_recursive_strike_damage_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Colossal Smash - 225132 for Nighthold Trinket Might of Krosus - 140799
class spell_item_colossal_smash : public SpellScriptLoader
{
public:
    spell_item_colossal_smash() : SpellScriptLoader("spell_item_colossal_smash") { }

    class spell_item_colossal_smash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_colossal_smash_SpellScript);

        enum eSpells
        {
            ColossalSmashDamage = 232044
        };

        void HandleOnCast()
        {
            Unit* l_Caster = GetCaster();
            Item* l_Item = GetCastItem();
            if (!l_Caster || !l_Item)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::ColossalSmashDamage, true, l_Item);
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_item_colossal_smash_SpellScript::HandleOnCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_colossal_smash_SpellScript();
    }
};

/// Last Update 7.1.5 Build 23420
/// Called by Colossal Smash Damaging Spell - 232044 for Nighthold Trinket Might of Krosus - 140799
class spell_item_colossal_smash_damage : public SpellScriptLoader
{
public:
    spell_item_colossal_smash_damage() : SpellScriptLoader("spell_item_colossal_smash_damage") { }

    class spell_item_colossal_smash_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_colossal_smash_damage_SpellScript);

        enum eSpells
        {
            ColossalSmash = 225132
        };

        uint8 m_NbTargets = 1;

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            m_NbTargets = p_Targets.size();
            if (m_NbTargets > 2)
                return;

            Unit* l_Caster = GetCaster();
            SpellInfo const* l_ColossalInfo = sSpellMgr->GetSpellInfo(eSpells::ColossalSmash);
            if (!l_Caster || !l_Caster->IsPlayer() || !l_ColossalInfo)
                return;

            l_Caster->ToPlayer()->ReduceSpellCooldown(eSpells::ColossalSmash, l_ColossalInfo->Effects[EFFECT_2].BasePoints * TimeConstants::IN_MILLISECONDS);
        }

        void HandleOnHit()
        {
            SetHitDamage(GetHitDamage() / m_NbTargets);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_colossal_smash_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnHit += SpellHitFn(spell_item_colossal_smash_damage_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_colossal_smash_damage_SpellScript();
    }
};

/// Last update 7.3.5 Build 25996
/// Called by Prescience 225139 for Nighthold Trinket 'Convergence of Fate' 140806
class spell_item_prescience : public SpellScriptLoader
{
    public:
        spell_item_prescience() : SpellScriptLoader("spell_item_prescience") { }

        class spell_item_prescience_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_prescience_AuraScript);

            enum eSpells
            {
                AvengingWrath           = 31884,
                Crusade                 = 231895,
                BattleCry               = 1719,
                Berserk                 = 106951,
                KingOfTheJungle         = 102543,
                EmpowerRuneWeapon       = 47568,
                HungeringRuneWeapon     = 207127,
                SummonGargoyle          = 49206,
                DarkArbiter             = 207349,
                AspectOfTheWild         = 193530,
                TrueShot                = 193526,
                AspectOfTheEagle        = 186289,
                Vandetta                = 79140,
                AdrenalineRush          = 13750,
                ShadowBlades            = 121471,
                FeralSpirit             = 51533,
                StormEarthAndFire       = 137639,
                Serenity                = 152173,
                Metamorphosis           = 191427
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                uint32 l_CooldownReduce = l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS;

                switch (l_Player->GetActiveSpecializationID()) /// All Roll chances here are taken from http://www.wowhead.com/item=140806/convergence-of-fates&bonus=3514:1472#comments. Our core handles PPM/Spec, but some specs got a talent that modifies the reduced Spell, with a different PPM, so need to hardcode it. PIGPIGPIGPIG
                {
                    case SpecIndex::SPEC_PALADIN_RETRIBUTION:
                        if (!l_Player->HasSpell(eSpells::Crusade))
                        {
                            if (SpellInfo const* l_AvengingWrath = sSpellMgr->GetSpellInfo(eSpells::AvengingWrath))
                                l_Player->ReduceChargeCooldown(l_AvengingWrath->ChargeCategoryEntry, l_CooldownReduce);
                        }
                        else /// Reducing the CD of Avenging Wrath reduces Crusade's one, but the proc chance is too high, so I divide it by 3.
                        {
                            if (SpellInfo const* l_AvengingWrath = sSpellMgr->GetSpellInfo(eSpells::AvengingWrath))
                            {
                                if (roll_chance_i(33))
                                    l_Player->ReduceChargeCooldown(l_AvengingWrath->ChargeCategoryEntry, l_CooldownReduce);
                            }
                        }
                        break;

                    case SpecIndex::SPEC_WARRIOR_ARMS:
                    case SpecIndex::SPEC_WARRIOR_FURY:
                        l_Player->ReduceSpellCooldown(eSpells::BattleCry, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_DRUID_FERAL:
                        if (roll_chance_i(50))
                            l_Player->ReduceSpellCooldown(eSpells::Berserk, l_CooldownReduce);

                        l_Player->ReduceSpellCooldown(eSpells::KingOfTheJungle, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_DK_FROST:
                        if (!l_Player->HasSpell(eSpells::HungeringRuneWeapon))
                        {
                            if (SpellInfo const* l_EmpowerRuneWeapon = sSpellMgr->GetSpellInfo(eSpells::EmpowerRuneWeapon))
                                l_Player->ReduceChargeCooldown(l_EmpowerRuneWeapon->ChargeCategoryEntry, l_CooldownReduce);
                        }
                        else /// Reducing the CD of EmpowerRune reduces HungeringRune's one, but the proc chance is too high, so I divide it by 3/2.
                        {
                            if (SpellInfo const* l_EmpowerRuneWeapon = sSpellMgr->GetSpellInfo(eSpells::EmpowerRuneWeapon))
                            {
                                if (roll_chance_i(66))
                                    l_Player->ReduceChargeCooldown(l_EmpowerRuneWeapon->ChargeCategoryEntry, l_CooldownReduce);
                            }
                        }

                        break;

                    case SpecIndex::SPEC_DK_UNHOLY:
                        if (roll_chance_i(50))
                        {
                            l_Player->ReduceSpellCooldown(eSpells::SummonGargoyle, l_CooldownReduce);
                            l_Player->ReduceSpellCooldown(eSpells::DarkArbiter, l_CooldownReduce);
                        }
                        break;

                    case SpecIndex::SPEC_HUNTER_BEASTMASTERY:
                        l_Player->ReduceSpellCooldown(eSpells::AspectOfTheWild, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_HUNTER_MARKSMANSHIP:
                        l_Player->ReduceSpellCooldown(eSpells::TrueShot, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_HUNTER_SURVIVAL:
                        l_Player->ReduceSpellCooldown(eSpells::AspectOfTheEagle, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_ROGUE_ASSASSINATION:
                        if (roll_chance_i(66))
                            l_Player->ReduceSpellCooldown(eSpells::Vandetta, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_ROGUE_OUTLAW:
                        l_Player->ReduceSpellCooldown(eSpells::AdrenalineRush, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_ROGUE_SUBTLETY:
                        l_Player->ReduceSpellCooldown(eSpells::ShadowBlades, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_SHAMAN_ENHANCEMENT:
                        if (roll_chance_i(66))
                            l_Player->ReduceSpellCooldown(eSpells::FeralSpirit, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_MONK_WINDWALKER:
                        l_Player->ReduceSpellCooldown(eSpells::Serenity, l_CooldownReduce);
                        if (SpellInfo const* l_StormEarthAndFire = sSpellMgr->GetSpellInfo(eSpells::StormEarthAndFire))
                            l_Player->ReduceChargeCooldown(l_StormEarthAndFire->ChargeCategoryEntry, l_CooldownReduce);
                        break;

                    case SpecIndex::SPEC_DEMON_HUNTER_HAVOC:
                        l_Player->ReduceSpellCooldown(eSpells::Metamorphosis, l_CooldownReduce);
                        break;

                    default:
                        break;
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_prescience_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_prescience_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Orb of Destruction 229700 for Nighthold Trinket 'Pharamere's Forbidden Grimoire' 140800
class spell_item_orb_of_destruction : public SpellScriptLoader
{
public:
    spell_item_orb_of_destruction() : SpellScriptLoader("spell_item_orb_of_destruction") { }

    class spell_item_orb_of_destruction_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_orb_of_destruction_SpellScript);

        void HandleOnHit()
        {
            Unit* l_OriginalCaster = GetCaster();
            if (!l_OriginalCaster)
                return;

            Unit* l_Target = GetHitUnit();
            WorldLocation const* l_CenterOfExplosion = GetExplTargetDest();
            SpellInfo const* l_OrbInfo = sSpellMgr->GetSpellInfo(m_scriptSpellId);
            if (!l_Target || !l_CenterOfExplosion || !l_OrbInfo)
                return;

            float l_MaxRadius = l_OrbInfo->Effects[EFFECT_0].CalcRadius();
            float l_DistancetoCenter = l_Target->GetExactDist2d(l_CenterOfExplosion);

            /// Damage decreases when u get far from the center of explosion
            if (l_MaxRadius != 0.0f)
            {
                float l_Ratio  = std::abs((l_MaxRadius - l_DistancetoCenter) / l_MaxRadius);
                int32 l_Damage = GetHitDamage();

                l_Damage = CalculatePct(GetHitDamage(), 100 * l_Ratio);

                if (l_Damage < 0 || l_Damage > GetHitDamage())
                    l_Damage = GetHitDamage();

                SetHitDamage(l_Damage);
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_item_orb_of_destruction_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_orb_of_destruction_SpellScript();
    }
};

/// Last update 7.1.5 Build 23420
/// Called by Grand Commendation of the Anglers - 135704
/// Called by Grand Commendation of the Klaxxi - 135719
/// Called by Grand Commendation of the Golden Lotus - 135717
/// Called by Grand Commendation of the Shado-Pan - 135715
/// Called by Grand Commendation of the August Celestials - 135716
/// Called by Grand Commendation of the Tillers - 135714
/// Called by Grand Commendation of the Order of the Cloud Serpent - 135713
/// Called by Grand Commendation of Operation: Shieldwall - 135710
/// Called by Grand Commendation of the Dominance Offensive - 135711
/// Called by Grand Commendation of the Kirin Tor Offensive - 140226
/// Called by Grand Commendation of the Sunreaver Onslaught - 140228
/// Called by Grand Commendation of the Lorewalkers - 135712
class spell_item_grand_commedation : public SpellScriptLoader
{
    public:
        spell_item_grand_commedation() : SpellScriptLoader("spell_item_grand_commedation") { }

        enum eSpells
        {
            GrandCommedationOfTheAnglers            = 135704,
            GrandCommedationOfTheKlaxxi             = 135719,
            GrandCommedationOfTheGoldenLotus        = 135717,
            GrandCommedationOfTheShadoPan           = 135715,
            GrandCommedationOfTheAugustCelestials   = 135716,
            GrandCommedationOfTheTillers            = 135714,
            GrandCommedationOfCloudSerpent          = 135713,
            GrandCommedationOfTheShieldWall         = 135710,
            GrandCommedationOfTheDominanceOffensive = 135711,
            GrandCommedationOfTheKirinTor           = 140226,
            GrandCommedationOfTheSunreaverOnslaught = 140228,
            GrandCommedationOfTheLorewalkers        = 135712
        };

        enum eFactions
        {
            TheAnglers              = 1302,
            TheKlaxxi               = 1337,
            TheGoldenLotus          = 1269,
            TheShadoPan             = 1270,
            TheAugustCelestials     = 1341,
            TheTillers              = 1272,
            CloudSerpent            = 1271,
            TheShieldWall           = 1376,
            TheDominanceOffensive   = 1375,
            TheKirinTor             = 1387,
            TheSunreaverOnslaught   = 1388,
            TheLorewalkers          = 1345
        };

        class spell_item_grand_commedation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_grand_commedation_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::unordered_map<uint32, uint32> l_SpellsToFactions =
                {
                    { eSpells::GrandCommedationOfTheAnglers, eFactions::TheAnglers },
                    { eSpells::GrandCommedationOfTheKlaxxi, eFactions::TheKlaxxi },
                    { eSpells::GrandCommedationOfTheGoldenLotus, eFactions::TheGoldenLotus },
                    { eSpells::GrandCommedationOfTheShadoPan, eFactions::TheShadoPan },
                    { eSpells::GrandCommedationOfTheAugustCelestials, eFactions::TheAugustCelestials },
                    { eSpells::GrandCommedationOfTheTillers, eFactions::TheTillers },
                    { eSpells::GrandCommedationOfCloudSerpent, eFactions::CloudSerpent },
                    { eSpells::GrandCommedationOfTheShieldWall, eFactions::TheShieldWall },
                    { eSpells::GrandCommedationOfTheDominanceOffensive, eFactions::TheDominanceOffensive },
                    { eSpells::GrandCommedationOfTheKirinTor, eFactions::TheKirinTor },
                    { eSpells::GrandCommedationOfTheSunreaverOnslaught, eFactions::TheSunreaverOnslaught },
                    { eSpells::GrandCommedationOfTheLorewalkers, eFactions::TheLorewalkers }
                };

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    auto l_Itr = l_SpellsToFactions.find(GetSpellInfo()->Id);
                    if (l_Itr != l_SpellsToFactions.end())
                        l_Player->GetReputationMgr().AddReputationMod(l_Itr->second, GetSpellInfo()->Id, 100);
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_grand_commedation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_grand_commedation_SpellScript();
        }
};

/// Cloak of the Foreseen - 231958
/// Gloves of the Foreseen - 231954
/// Chest of the Foreseen - 231953
/// Shoulders of the Foreseen - 231957
/// Helm of the Foreseen - 231955
/// Leggings of the Foreseen - 231956
class spell_create_nighthold_set_item : public SpellScriptLoader
{
    public:
        spell_create_nighthold_set_item() : SpellScriptLoader("spell_create_nighthold_set_item") { }

        class spell_create_nighthold_set_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_nighthold_set_item_SpellScript);

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            enum eTokenBonuses
            {
                BonusHeroic = 570,
                BonusMythic = 569
            };

            uint32 m_ItemID;

            bool Load() override
            {
                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                LootStore& l_LootStore = LootTemplates_Spell;
                LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(GetSpellInfo()->Id);
                if (l_LootTemplate == nullptr)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::list<ItemTemplate const*> l_LootTable;
                std::vector<uint32> l_Items;
                l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, false);

                if (l_LootTable.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                for (ItemTemplate const* l_Template : l_LootTable)
                {
                    if (l_Template->HasSpec((SpecIndex)l_SpecID, l_Player->getLevel()))
                        l_Items.push_back(l_Template->ItemId);
                }

                if (l_Items.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::random_shuffle(l_Items.begin(), l_Items.end());

                m_ItemID = l_Items[0];

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return;

                Item* l_Item = GetCastItem();
                if (!l_Item || !m_ItemID)
                    return;

                ItemContext l_Context = ItemContext::RaidNormal;

                if (l_Item->HasItemBonus(eTokenBonuses::BonusMythic))
                    l_Context = ItemContext::RaidMythic;
                else if (l_Item->HasItemBonus(eTokenBonuses::BonusHeroic))
                    l_Context = ItemContext::RaidHeroic;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                std::vector<uint32> l_BonusList;
                Item::GenerateItemBonus(m_ItemID, l_Context, l_BonusList, false, l_Player);

                if (Item* l_Item = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_BonusList))
                {
                    l_Player->SendNewItem(l_Item, 1, false, true, false, 0, l_Context);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_BonusList);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_nighthold_set_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_nighthold_set_item_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_nighthold_set_item_SpellScript();
        }
};

/// Cloak of the Foregone - 240721
/// Gloves of the Foregone - 240717
/// Chest of the Foregone - 240716
/// Shoulders of the Foregone - 240720
/// Helm of the Foregone - 240718
/// Leggings of the Foregone - 240719
class spell_create_tos_set_item : public SpellScriptLoader
{
    public:
        spell_create_tos_set_item() : SpellScriptLoader("spell_create_tos_set_item") { }

        class spell_create_tos_set_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_tos_set_item_SpellScript);

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            enum eTokenBonuses
            {
                BonusLFR1   = 3564, ///< Whole raid, except Kil'Jaeden
                BonusLFR2   = 1477, ///< Kil'Jaeden only

                BonusHeroic = 570,
                BonusMythic = 569
            };

            uint32 m_ItemID;

            bool Load() override
            {
                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                LootStore& l_LootStore = LootTemplates_Spell;
                LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(GetSpellInfo()->Id);
                if (l_LootTemplate == nullptr)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::list<ItemTemplate const*> l_LootTable;
                std::vector<uint32> l_Items;
                l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, false);

                if (l_LootTable.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                for (ItemTemplate const* l_Template : l_LootTable)
                {
                    if (l_Template->HasSpec((SpecIndex)l_SpecID, l_Player->getLevel()))
                        l_Items.push_back(l_Template->ItemId);
                }

                if (l_Items.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::random_shuffle(l_Items.begin(), l_Items.end());

                m_ItemID = l_Items[0];

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return;

                Item* l_Item = GetCastItem();
                if (!l_Item || !m_ItemID)
                    return;

                ItemContext l_Context = ItemContext::RaidNormal;

                if (l_Item->HasItemBonus(eTokenBonuses::BonusMythic))
                    l_Context = ItemContext::RaidMythic;
                else if (l_Item->HasItemBonus(eTokenBonuses::BonusHeroic))
                    l_Context = ItemContext::RaidHeroic;
                else if (l_Item->HasItemBonus(eTokenBonuses::BonusLFR1) || l_Item->HasItemBonus(eTokenBonuses::BonusLFR2))
                    l_Context = ItemContext::RaidLfr;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                std::vector<uint32> l_BonusList;
                Item::GenerateItemBonus(m_ItemID, l_Context, l_BonusList, false, l_Player);

                if (Item* l_Item = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_BonusList))
                {
                    l_Player->SendNewItem(l_Item, 1, false, true, false, 0, l_Context);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_BonusList);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_tos_set_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_tos_set_item_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_tos_set_item_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Shadow Wave - 215047
/// Item Terrorbound Nexus - 137406
class spell_item_shadow_wave : public SpellScriptLoader
{
public:
    spell_item_shadow_wave() : SpellScriptLoader("spell_item_shadow_wave") { }

    class spell_item_shadow_wave_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_shadow_wave_SpellScript);

        void HandleOnHit()
        {
            Unit* l_TargetHit = GetHitUnit();
            Unit* l_Caster = GetCaster();
            if (!l_TargetHit ||!l_Caster)
                return;

            uint32 l_TimeOfHit = getMSTime();

            if (((l_TimeOfHit - (l_TargetHit->m_SpellHelper.GetUint32(eSpellHelpers::ShadowWaveHitTimer))) < 500))
                SetHitDamage(0);
            else
                l_TargetHit->m_SpellHelper.GetUint32(eSpellHelpers::ShadowWaveHitTimer) = l_TimeOfHit;
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_item_shadow_wave_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_shadow_wave_SpellScript();
    }
};

/// Last updated - 23420
/// Titan Essence - 232255
class spell_item_titan_essence_940_upgrade : public SpellScriptLoader
{
    public:
        spell_item_titan_essence_940_upgrade() : SpellScriptLoader("spell_item_titan_essence_940_upgrade") { }

        enum e940Bonuses
        {
            BonusIlvlDisplay    = 1811,
            BonusIlvlUpgrade    = 3455,

            BonusIlvlBase       = 910
        };

        enum eItem
        {
            DistilledTitanEssence = 143613
        };

        class spell_item_titan_essence_940_upgrade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_titan_essence_940_upgrade_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->IsPlayer())
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                    if (Item* l_Item = GetExplTargetItem())
                    {
                        if (l_Item->GetTemplate()->ItemId == eItem::DistilledTitanEssence)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->GetTemplate()->Quality != ItemQualities::ITEM_QUALITY_LEGENDARY)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (!sObjectMgr->IsLegionLegendaryItem(l_Item->GetEntry()))
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->HasItemBonus(e940Bonuses::BonusIlvlDisplay) && l_Item->HasItemBonus(e940Bonuses::BonusIlvlUpgrade))
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->GetItemLevel(l_Caster->ToPlayer()) != e940Bonuses::BonusIlvlBase)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return;

                l_Item->RemoveAllItemBonuses();
                l_Item->UpgradeItemWithBonuses({ e940Bonuses::BonusIlvlDisplay, e940Bonuses::BonusIlvlUpgrade });
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_titan_essence_940_upgrade_SpellScript::HandleCheckCast);
                AfterCast += SpellCastFn(spell_item_titan_essence_940_upgrade_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_titan_essence_940_upgrade_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Titan Essence - 246747
/// Called for Stabilized Titan Essence - 151423
class spell_item_titan_essence_970_upgrade : public SpellScriptLoader
{
    public:
        spell_item_titan_essence_970_upgrade() : SpellScriptLoader("spell_item_titan_essence_970_upgrade") { }

        enum e970Bonuses
        {
            BonusIlvlDisplay    = 3570,
            BonusIlvlUpgrade    = 3459,

            BonusIlvlBaseMin    = 910,
            BonusIlvlBaseMax    = 940
        };

        enum eItem
        {
            StabilizedTitanEssence = 151423
        };

        class spell_item_titan_essence_970_upgrade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_titan_essence_970_upgrade_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->IsPlayer())
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                    if (Item* l_Item = GetExplTargetItem())
                    {
                        if (l_Item->GetTemplate()->ItemId == eItem::StabilizedTitanEssence)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->GetTemplate()->Quality != ItemQualities::ITEM_QUALITY_LEGENDARY)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (!sObjectMgr->IsLegionLegendaryItem(l_Item->GetEntry()))
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->HasItemBonus(e970Bonuses::BonusIlvlDisplay) && l_Item->HasItemBonus(e970Bonuses::BonusIlvlUpgrade))
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->GetItemLevel(l_Caster->ToPlayer()) < e970Bonuses::BonusIlvlBaseMin ||
                            l_Item->GetItemLevel(l_Caster->ToPlayer()) > e970Bonuses::BonusIlvlBaseMax)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return;

                l_Item->UpgradeItemWithBonuses({ e970Bonuses::BonusIlvlDisplay, e970Bonuses::BonusIlvlUpgrade }, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_titan_essence_970_upgrade_SpellScript::HandleCheckCast);
                AfterCast += SpellCastFn(spell_item_titan_essence_970_upgrade_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_titan_essence_970_upgrade_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Awoken Essence - 257327
/// Called for Awoken Titan Essence - 154879
class spell_item_titan_essence_1000_upgrade : public SpellScriptLoader
{
    public:
        spell_item_titan_essence_1000_upgrade() : SpellScriptLoader("spell_item_titan_essence_1000_upgrade") { }

        enum e1000Bonuses
        {
            BonusIlvlDisplay    = 1811,
            BonusIlvlUpgrade    = 3630,

            BonusIlvlBaseMin    = 910,
            BonusIlvlBaseMax    = 970
        };

        enum eItem
        {
            AwokenTitanEssence = 154879
        };

        class spell_item_titan_essence_1000_upgrade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_titan_essence_1000_upgrade_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->IsPlayer())
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                    if (Item* l_Item = GetExplTargetItem())
                    {
                        if (l_Item->GetTemplate()->ItemId == eItem::AwokenTitanEssence)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->GetTemplate()->Quality != ItemQualities::ITEM_QUALITY_LEGENDARY)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (!sObjectMgr->IsLegionLegendaryItem(l_Item->GetEntry()))
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->HasItemBonus(e1000Bonuses::BonusIlvlDisplay) && l_Item->HasItemBonus(e1000Bonuses::BonusIlvlUpgrade))
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                        if (l_Item->GetItemLevel(l_Caster->ToPlayer()) < e1000Bonuses::BonusIlvlBaseMin ||
                            l_Item->GetItemLevel(l_Caster->ToPlayer()) > e1000Bonuses::BonusIlvlBaseMax)
                            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return;

                if (l_Item->IsEquipped())
                    l_Player->_ApplyItemMods(l_Item, l_Item->GetSlot(), false);

                l_Item->UpgradeItemWithBonuses({ e1000Bonuses::BonusIlvlDisplay, e1000Bonuses::BonusIlvlUpgrade }, true);

                if (l_Item->IsEquipped())
                    l_Player->_ApplyItemMods(l_Item, l_Item->GetSlot(), true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_titan_essence_1000_upgrade_SpellScript::HandleCheckCast);
                AfterCast += SpellCastFn(spell_item_titan_essence_1000_upgrade_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_titan_essence_1000_upgrade_SpellScript();
        }
};

/// Helm of the Cruel Gladiator - 239581
/// Leggings of the Cruel Gladiator - 239584
/// Pauldrons of the Cruel Gladiator - 239586
/// Gloves of the Cruel Gladiator - 239587
/// Cinch of the Cruel Gladiator - 239589
/// Threads of the Cruel Gladiator - 239590
/// Bracers of the Cruel Gladiator - 239591
/// Cloak of the Cruel Gladiator - 239592
/// Chest of the Cruel Gladiator - 239583
/// Helm of the Cruel Gladiator (Elite) - 239596
/// Leggings of the Cruel Gladiator (Elite) - 239598
/// Pauldrons of the Cruel Gladiator (Elite) - 239599
/// Gloves of the Cruel Gladiator (Elite) - 239600
/// Cinch of the Cruel Gladiator (Elite) - 239601
/// Threads of the Cruel Gladiator (Elite) - 239602
/// Bracers of the Cruel Gladiator (Elite) - 239603
/// Cloak of the Cruel Gladiator (Elite) - 239604
/// Chest of the Cruel Gladiator (Elite) - 239597
class spell_create_season_3_item : public SpellScriptLoader
{
    public:
        spell_create_season_3_item() : SpellScriptLoader("spell_create_season_3_item") { }

        class spell_create_season_3_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_season_3_item_SpellScript);

            enum eSpells
            {
                SpellHelm           = 239581,
                SpellChest          = 239583,
                SpellLeggings       = 239584,
                SpellPauldrons      = 239586,
                SpellGloves         = 239587,
                SpellCinch          = 239589,
                SpellThreads        = 239590,
                SpellBracers        = 239591,
                SpellCloak          = 239592
            };

            enum eEliteSpells
            {
                EliteSpellHelm      = 239596,
                EliteSpellChest     = 239597,
                EliteSpellLeggings  = 239598,
                EliteSpellPauldrons = 239599,
                EliteSpellGloves    = 239600,
                EliteSpellCinch     = 239601,
                EliteSpellThreads   = 239602,
                EliteSpellBracers   = 239603,
                EliteSpellCloak     = 239604,

                MaxEliteSpells      = 9
            };

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            enum eTokenBonuses
            {
                BonusHeroic = 570,
                BonusMythic = 569
            };

            std::array<uint32, eEliteSpells::MaxEliteSpells> m_EliteSpells
            {
                {
                    eEliteSpells::EliteSpellHelm,
                    eEliteSpells::EliteSpellChest,
                    eEliteSpells::EliteSpellLeggings,
                    eEliteSpells::EliteSpellPauldrons,
                    eEliteSpells::EliteSpellGloves,
                    eEliteSpells::EliteSpellCinch,
                    eEliteSpells::EliteSpellThreads,
                    eEliteSpells::EliteSpellBracers,
                    eEliteSpells::EliteSpellCloak
                }
            };

            std::map<uint32, InventoryType> m_SpellInventoryType =
            {
                { eEliteSpells::EliteSpellHelm,         InventoryType::INVTYPE_HEAD },
                { eEliteSpells::EliteSpellChest,        InventoryType::INVTYPE_CHEST },
                { eEliteSpells::EliteSpellLeggings,     InventoryType::INVTYPE_LEGS },
                { eEliteSpells::EliteSpellPauldrons,    InventoryType::INVTYPE_SHOULDERS },
                { eEliteSpells::EliteSpellGloves,       InventoryType::INVTYPE_HANDS },
                { eEliteSpells::EliteSpellCinch,        InventoryType::INVTYPE_WAIST },
                { eEliteSpells::EliteSpellThreads,      InventoryType::INVTYPE_FEET },
                { eEliteSpells::EliteSpellBracers,      InventoryType::INVTYPE_WRISTS },
                { eEliteSpells::EliteSpellCloak,        InventoryType::INVTYPE_CLOAK },

                { eSpells::SpellHelm,                   InventoryType::INVTYPE_HEAD },
                { eSpells::SpellChest,                  InventoryType::INVTYPE_CHEST },
                { eSpells::SpellLeggings,               InventoryType::INVTYPE_LEGS },
                { eSpells::SpellPauldrons,              InventoryType::INVTYPE_SHOULDERS },
                { eSpells::SpellGloves,                 InventoryType::INVTYPE_HANDS },
                { eSpells::SpellCinch,                  InventoryType::INVTYPE_WAIST },
                { eSpells::SpellThreads,                InventoryType::INVTYPE_FEET },
                { eSpells::SpellBracers,                InventoryType::INVTYPE_WRISTS },
                { eSpells::SpellCloak,                  InventoryType::INVTYPE_CLOAK }
            };

            bool m_Elite;
            uint32 m_ItemID;

            bool Load() override
            {
                if (std::find(m_EliteSpells.begin(), m_EliteSpells.end(), GetSpellInfo()->Id) != m_EliteSpells.end())
                    m_Elite = true;
                else
                    m_Elite = false;

                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_LootSpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                InventoryType l_InventoryType = InventoryType::INVTYPE_NON_EQUIP;

                auto const& l_Iter = m_SpellInventoryType.find(GetSpellInfo()->Id);
                if (l_Iter != m_SpellInventoryType.end())
                    l_InventoryType = l_Iter->second;

                ItemTemplate const* l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, l_InventoryType, PvpSeasons::LegionSeason3);

                if (!l_GearReward && l_InventoryType == InventoryType::INVTYPE_CHEST)
                    l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, InventoryType::INVTYPE_ROBE, PvpSeasons::LegionSeason3);

                if (l_GearReward != nullptr)
                {
                    m_ItemID = l_GearReward->ItemId;

                    ItemPosCountVec l_Dest;
                    InventoryResult l_Msg = l_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, InventorySlot::NULL_SLOT, l_Dest, m_ItemID, 1);

                    if (l_Msg != InventoryResult::EQUIP_ERR_OK)
                    {
                        l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    }
                }
                else
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player || !m_ItemID)
                    return;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                uint32 l_BestPrevWeekRating         = 0;
                BattlegroundBracketType l_Bracket   = BattlegroundBracketType::Arena2v2;

                for (uint8 l_I = 0; l_I < uint8(BattlegroundBracketType::Max); ++l_I)
                {
                    if (l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I)) > l_BestPrevWeekRating)
                    {
                        l_Bracket = BattlegroundBracketType(l_I);
                        l_BestPrevWeekRating = l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I));
                    }
                }

                std::vector<uint32> l_ItemBonuses;
                std::vector<uint32> l_Dummy;
                Item::GenerateItemBonus(m_ItemID, static_cast<ItemContext>(l_Player->GetWorldQuestManager()->GenerateItemContext(MS::WorldQuest::WorldQuestType::DailyPvP, sObjectMgr->GetItemTemplate(m_ItemID), l_Dummy, l_Bracket)), l_ItemBonuses, true, l_Player);

                /// PvP tokens give straight 900 iLvl on Greymane
                if (sWorld->getBoolConfig(WorldBoolConfigs::CONFIG_TOURNAMENT_ENABLE))
                {
                    l_ItemBonuses = { 1532, 3568 };

                    std::list<uint32> l_List = { 1676, 1683, 1690, 1697, 1704, 1711, 1718, 1719, 1721 };

                    JadeCore::RandomResizeList(l_List, 1);

                    l_ItemBonuses.push_back(l_List.front());
                }

                if (Item* l_AddedItem = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_ItemBonuses))
                {
                    l_Player->SendNewItem(l_AddedItem, 1, true, false);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_season_3_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_season_3_item_SpellScript::HandleCreateItem, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_season_3_item_SpellScript();
        }
};

/// Helm of the Ferocious Gladiator - 239605
/// Leggings of the Ferocious Gladiator - 239606
/// Pauldrons of the Ferocious Gladiator - 239607
/// Gloves of the Ferocious Gladiator - 239608
/// Cinch of the Ferocious Gladiator - 239609
/// Threads of the Ferocious Gladiator - 239610
/// Bracers of the Ferocious Gladiator - 239611
/// Cloak of the Ferocious Gladiator - 239612
/// Chest of the Ferocious Gladiator - 239613
/// Helm of the Ferocious Gladiator (Elite) - 239614
/// Leggings of the Ferocious Gladiator (Elite) - 239615
/// Pauldrons of the Ferocious Gladiator (Elite) - 239616
/// Gloves of the Ferocious Gladiator (Elite) - 239617
/// Cinch of the Ferocious Gladiator (Elite) - 239618
/// Threads of the Ferocious Gladiator (Elite) - 239619
/// Bracers of the Ferocious Gladiator (Elite) - 239620
/// Cloak of the Ferocious Gladiator (Elite) - 239621
/// Chest of the Ferocious Gladiator (Elite) - 239622
class spell_create_season_4_item : public SpellScriptLoader
{
    public:
        spell_create_season_4_item() : SpellScriptLoader("spell_create_season_4_item") { }

        class spell_create_season_4_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_season_4_item_SpellScript);

            enum eSpells
            {
                SpellHelm           = 239605,
                SpellChest          = 239606,
                SpellLeggings       = 239607,
                SpellPauldrons      = 239608,
                SpellGloves         = 239609,
                SpellCinch          = 239610,
                SpellThreads        = 239611,
                SpellBracers        = 239612,
                SpellCloak          = 239613
            };

            enum eEliteSpells
            {
                EliteSpellHelm      = 239614,
                EliteSpellChest     = 239615,
                EliteSpellLeggings  = 239616,
                EliteSpellPauldrons = 239617,
                EliteSpellGloves    = 239618,
                EliteSpellCinch     = 239619,
                EliteSpellThreads   = 239620,
                EliteSpellBracers   = 239621,
                EliteSpellCloak     = 239622,

                MaxEliteSpells      = 9
            };

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            enum eTokenBonuses
            {
                BonusHeroic = 570,
                BonusMythic = 569
            };

            std::array<uint32, eEliteSpells::MaxEliteSpells> m_EliteSpells
            {
                {
                    eEliteSpells::EliteSpellHelm,
                    eEliteSpells::EliteSpellChest,
                    eEliteSpells::EliteSpellLeggings,
                    eEliteSpells::EliteSpellPauldrons,
                    eEliteSpells::EliteSpellGloves,
                    eEliteSpells::EliteSpellCinch,
                    eEliteSpells::EliteSpellThreads,
                    eEliteSpells::EliteSpellBracers,
                    eEliteSpells::EliteSpellCloak
                }
            };

            std::map<uint32, InventoryType> m_SpellInventoryType =
            {
                { eEliteSpells::EliteSpellHelm,         InventoryType::INVTYPE_HEAD },
                { eEliteSpells::EliteSpellChest,        InventoryType::INVTYPE_CHEST },
                { eEliteSpells::EliteSpellLeggings,     InventoryType::INVTYPE_LEGS },
                { eEliteSpells::EliteSpellPauldrons,    InventoryType::INVTYPE_SHOULDERS },
                { eEliteSpells::EliteSpellGloves,       InventoryType::INVTYPE_HANDS },
                { eEliteSpells::EliteSpellCinch,        InventoryType::INVTYPE_WAIST },
                { eEliteSpells::EliteSpellThreads,      InventoryType::INVTYPE_FEET },
                { eEliteSpells::EliteSpellBracers,      InventoryType::INVTYPE_WRISTS },
                { eEliteSpells::EliteSpellCloak,        InventoryType::INVTYPE_CLOAK },

                { eSpells::SpellHelm,                   InventoryType::INVTYPE_HEAD },
                { eSpells::SpellChest,                  InventoryType::INVTYPE_CHEST },
                { eSpells::SpellLeggings,               InventoryType::INVTYPE_LEGS },
                { eSpells::SpellPauldrons,              InventoryType::INVTYPE_SHOULDERS },
                { eSpells::SpellGloves,                 InventoryType::INVTYPE_HANDS },
                { eSpells::SpellCinch,                  InventoryType::INVTYPE_WAIST },
                { eSpells::SpellThreads,                InventoryType::INVTYPE_FEET },
                { eSpells::SpellBracers,                InventoryType::INVTYPE_WRISTS },
                { eSpells::SpellCloak,                  InventoryType::INVTYPE_CLOAK }
            };

            bool m_Elite;
            uint32 m_ItemID;

            bool Load() override
            {
                if (std::find(m_EliteSpells.begin(), m_EliteSpells.end(), GetSpellInfo()->Id) != m_EliteSpells.end())
                    m_Elite = true;
                else
                    m_Elite = false;

                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_LootSpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                InventoryType l_InventoryType = InventoryType::INVTYPE_NON_EQUIP;

                auto const& l_Iter = m_SpellInventoryType.find(GetSpellInfo()->Id);
                if (l_Iter != m_SpellInventoryType.end())
                    l_InventoryType = l_Iter->second;

                ItemTemplate const* l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, l_InventoryType, PvpSeasons::LegionSeason4);

                if (!l_GearReward && l_InventoryType == InventoryType::INVTYPE_CHEST)
                    l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, InventoryType::INVTYPE_ROBE, PvpSeasons::LegionSeason4);

                if (l_GearReward != nullptr)
                {
                    m_ItemID = l_GearReward->ItemId;

                    ItemPosCountVec l_Dest;
                    InventoryResult l_Msg = l_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, InventorySlot::NULL_SLOT, l_Dest, m_ItemID, 1);

                    if (l_Msg != InventoryResult::EQUIP_ERR_OK)
                    {
                        l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    }
                }
                else
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player || !m_ItemID)
                    return;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                uint32 l_BestPrevWeekRating         = 0;
                BattlegroundBracketType l_Bracket   = BattlegroundBracketType::Arena2v2;

                for (uint8 l_I = 0; l_I < uint8(BattlegroundBracketType::Max); ++l_I)
                {
                    if (l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I)) > l_BestPrevWeekRating)
                    {
                        l_Bracket = BattlegroundBracketType(l_I);
                        l_BestPrevWeekRating = l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I));
                    }
                }

                std::vector<uint32> l_ItemBonuses;
                std::vector<uint32> l_Dummy;
                Item::GenerateItemBonus(m_ItemID, m_Elite ? ItemContext::Ilvl915 : ItemContext::Ilvl895, l_ItemBonuses, true, l_Player);

                if (Item* l_AddedItem = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_ItemBonuses))
                {
                    l_Player->SendNewItem(l_AddedItem, 1, true, false);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_season_4_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_season_4_item_SpellScript::HandleCreateItem, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_season_4_item_SpellScript();
        }
};

class spell_create_season_5_item : public SpellScriptLoader
{
    public:
        spell_create_season_5_item() : SpellScriptLoader("spell_create_season_5_item") { }

        class spell_create_season_5_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_season_5_item_SpellScript);

            enum eSpells
            {
                SpellHelm           = 243727,
                SpellChest          = 243728,
                SpellLeggings       = 243729,
                SpellPauldrons      = 243730,
                SpellGloves         = 243731,
                SpellCinch          = 243732,
                SpellThreads        = 243733,
                SpellBracers        = 243734,
                SpellCloak          = 243735
            };

            enum eEliteSpells
            {
                EliteSpellHelm      = 243736,
                EliteSpellChest     = 243738,
                EliteSpellLeggings  = 243740,
                EliteSpellPauldrons = 243741,
                EliteSpellGloves    = 243742,
                EliteSpellCinch     = 243743,
                EliteSpellThreads   = 243744,
                EliteSpellBracers   = 243745,
                EliteSpellCloak     = 243746,

                MaxEliteSpells      = 9
            };

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            enum eTokenBonuses
            {
                BonusHeroic = 570,
                BonusMythic = 569
            };

            std::array<uint32, eEliteSpells::MaxEliteSpells> m_EliteSpells
            {
                {
                    eEliteSpells::EliteSpellHelm,
                    eEliteSpells::EliteSpellChest,
                    eEliteSpells::EliteSpellLeggings,
                    eEliteSpells::EliteSpellPauldrons,
                    eEliteSpells::EliteSpellGloves,
                    eEliteSpells::EliteSpellCinch,
                    eEliteSpells::EliteSpellThreads,
                    eEliteSpells::EliteSpellBracers,
                    eEliteSpells::EliteSpellCloak
                }
            };

            std::map<uint32, InventoryType> m_SpellInventoryType =
            {
                { eEliteSpells::EliteSpellHelm,         InventoryType::INVTYPE_HEAD },
                { eEliteSpells::EliteSpellChest,        InventoryType::INVTYPE_CHEST },
                { eEliteSpells::EliteSpellLeggings,     InventoryType::INVTYPE_LEGS },
                { eEliteSpells::EliteSpellPauldrons,    InventoryType::INVTYPE_SHOULDERS },
                { eEliteSpells::EliteSpellGloves,       InventoryType::INVTYPE_HANDS },
                { eEliteSpells::EliteSpellCinch,        InventoryType::INVTYPE_WAIST },
                { eEliteSpells::EliteSpellThreads,      InventoryType::INVTYPE_FEET },
                { eEliteSpells::EliteSpellBracers,      InventoryType::INVTYPE_WRISTS },
                { eEliteSpells::EliteSpellCloak,        InventoryType::INVTYPE_CLOAK },

                { eSpells::SpellHelm,                   InventoryType::INVTYPE_HEAD },
                { eSpells::SpellChest,                  InventoryType::INVTYPE_CHEST },
                { eSpells::SpellLeggings,               InventoryType::INVTYPE_LEGS },
                { eSpells::SpellPauldrons,              InventoryType::INVTYPE_SHOULDERS },
                { eSpells::SpellGloves,                 InventoryType::INVTYPE_HANDS },
                { eSpells::SpellCinch,                  InventoryType::INVTYPE_WAIST },
                { eSpells::SpellThreads,                InventoryType::INVTYPE_FEET },
                { eSpells::SpellBracers,                InventoryType::INVTYPE_WRISTS },
                { eSpells::SpellCloak,                  InventoryType::INVTYPE_CLOAK }
            };

            bool m_Elite;
            uint32 m_ItemID;

            bool Load() override
            {
                if (std::find(m_EliteSpells.begin(), m_EliteSpells.end(), GetSpellInfo()->Id) != m_EliteSpells.end())
                    m_Elite = true;
                else
                    m_Elite = false;

                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_LootSpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                InventoryType l_InventoryType = InventoryType::INVTYPE_NON_EQUIP;

                auto const& l_Iter = m_SpellInventoryType.find(GetSpellInfo()->Id);
                if (l_Iter != m_SpellInventoryType.end())
                    l_InventoryType = l_Iter->second;

                ItemTemplate const* l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, l_InventoryType, PvpSeasons::LegionSeason5);

                if (!l_GearReward && l_InventoryType == InventoryType::INVTYPE_CHEST)
                    l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, InventoryType::INVTYPE_ROBE, PvpSeasons::LegionSeason5);

                if (l_GearReward != nullptr)
                {
                    m_ItemID = l_GearReward->ItemId;

                    ItemPosCountVec l_Dest;
                    InventoryResult l_Msg = l_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, InventorySlot::NULL_SLOT, l_Dest, m_ItemID, 1);

                    if (l_Msg != InventoryResult::EQUIP_ERR_OK)
                    {
                        l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    }
                }
                else
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player || !m_ItemID)
                    return;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                uint32 l_BestPrevWeekRating         = 0;
                BattlegroundBracketType l_Bracket   = BattlegroundBracketType::Arena2v2;

                for (uint8 l_I = 0; l_I < uint8(BattlegroundBracketType::Max); ++l_I)
                {
                    if (l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I)) > l_BestPrevWeekRating)
                    {
                        l_Bracket = BattlegroundBracketType(l_I);
                        l_BestPrevWeekRating = l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I));
                    }
                }

                std::vector<uint32> l_ItemBonuses;
                std::vector<uint32> l_Dummy;
                Item::GenerateItemBonus(m_ItemID, m_Elite ? ItemContext::Ilvl930 : ItemContext::Ilvl905, l_ItemBonuses, true, l_Player);

                /// PvP tokens give straight 950 iLvl on Greymane and are selled by customs vendors
                if (sWorld->getBoolConfig(WorldBoolConfigs::CONFIG_TOURNAMENT_ENABLE))
                {
                    l_ItemBonuses = { 1582, 3568 };

                    std::list<uint32> l_List = { 1676, 1683, 1690, 1697, 1704, 1711, 1718, 1719, 1721 };

                    JadeCore::RandomResizeList(l_List, 1);

                    l_ItemBonuses.push_back(l_List.front());
                }

                if (Item* l_AddedItem = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_ItemBonuses))
                {
                    l_Player->SendNewItem(l_AddedItem, 1, true, false);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_season_5_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_season_5_item_SpellScript::HandleCreateItem, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_season_5_item_SpellScript();
        }
};

class spell_create_season_6_item : public SpellScriptLoader
{
    public:
        spell_create_season_6_item() : SpellScriptLoader("spell_create_season_6_item") { }

        class spell_create_season_6_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_season_6_item_SpellScript);

            enum eSpells
            {
                SpellHelm           = 244356,
                SpellChest          = 244352,
                SpellLeggings       = 244357,
                SpellPauldrons      = 244358,
                SpellGloves         = 244355,
                SpellCinch          = 244359,
                SpellThreads        = 244354,
                SpellBracers        = 244360,
                SpellCloak          = 244353
            };

            enum eEliteSpells
            {
                EliteSpellHelm      = 244370,
                EliteSpellChest     = 244363,
                EliteSpellLeggings  = 244372,
                EliteSpellPauldrons = 244374,
                EliteSpellGloves    = 244369,
                EliteSpellCinch     = 244375,
                EliteSpellThreads   = 244367,
                EliteSpellBracers   = 244376,
                EliteSpellCloak     = 244365,

                MaxEliteSpells      = 9
            };

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            enum eTokenBonuses
            {
                BonusHeroic = 570,
                BonusMythic = 569
            };

            std::array<uint32, eEliteSpells::MaxEliteSpells> m_EliteSpells
            {
                {
                    eEliteSpells::EliteSpellHelm,
                    eEliteSpells::EliteSpellChest,
                    eEliteSpells::EliteSpellLeggings,
                    eEliteSpells::EliteSpellPauldrons,
                    eEliteSpells::EliteSpellGloves,
                    eEliteSpells::EliteSpellCinch,
                    eEliteSpells::EliteSpellThreads,
                    eEliteSpells::EliteSpellBracers,
                    eEliteSpells::EliteSpellCloak
                }
            };

            std::map<uint32, InventoryType> m_SpellInventoryType =
            {
                { eEliteSpells::EliteSpellHelm,         InventoryType::INVTYPE_HEAD },
                { eEliteSpells::EliteSpellChest,        InventoryType::INVTYPE_CHEST },
                { eEliteSpells::EliteSpellLeggings,     InventoryType::INVTYPE_LEGS },
                { eEliteSpells::EliteSpellPauldrons,    InventoryType::INVTYPE_SHOULDERS },
                { eEliteSpells::EliteSpellGloves,       InventoryType::INVTYPE_HANDS },
                { eEliteSpells::EliteSpellCinch,        InventoryType::INVTYPE_WAIST },
                { eEliteSpells::EliteSpellThreads,      InventoryType::INVTYPE_FEET },
                { eEliteSpells::EliteSpellBracers,      InventoryType::INVTYPE_WRISTS },
                { eEliteSpells::EliteSpellCloak,        InventoryType::INVTYPE_CLOAK },

                { eSpells::SpellHelm,                   InventoryType::INVTYPE_HEAD },
                { eSpells::SpellChest,                  InventoryType::INVTYPE_CHEST },
                { eSpells::SpellLeggings,               InventoryType::INVTYPE_LEGS },
                { eSpells::SpellPauldrons,              InventoryType::INVTYPE_SHOULDERS },
                { eSpells::SpellGloves,                 InventoryType::INVTYPE_HANDS },
                { eSpells::SpellCinch,                  InventoryType::INVTYPE_WAIST },
                { eSpells::SpellThreads,                InventoryType::INVTYPE_FEET },
                { eSpells::SpellBracers,                InventoryType::INVTYPE_WRISTS },
                { eSpells::SpellCloak,                  InventoryType::INVTYPE_CLOAK }
            };

            bool m_Elite;
            uint32 m_ItemID;

            bool Load() override
            {
                if (std::find(m_EliteSpells.begin(), m_EliteSpells.end(), GetSpellInfo()->Id) != m_EliteSpells.end())
                    m_Elite = true;
                else
                    m_Elite = false;

                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_LootSpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                InventoryType l_InventoryType = InventoryType::INVTYPE_NON_EQUIP;

                auto const& l_Iter = m_SpellInventoryType.find(GetSpellInfo()->Id);
                if (l_Iter != m_SpellInventoryType.end())
                    l_InventoryType = l_Iter->second;

                ItemTemplate const* l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, l_InventoryType, PvpSeasons::LegionSeason6);

                if (!l_GearReward && l_InventoryType == InventoryType::INVTYPE_CHEST)
                    l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, InventoryType::INVTYPE_ROBE, PvpSeasons::LegionSeason6);

                if (l_GearReward != nullptr)
                {
                    m_ItemID = l_GearReward->ItemId;

                    ItemPosCountVec l_Dest;
                    InventoryResult l_Msg = l_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, InventorySlot::NULL_SLOT, l_Dest, m_ItemID, 1);

                    if (l_Msg != InventoryResult::EQUIP_ERR_OK)
                    {
                        l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                    }
                }
                else
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player || !m_ItemID)
                    return;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                uint32 l_BestPrevWeekRating         = 0;
                BattlegroundBracketType l_Bracket   = BattlegroundBracketType::Arena2v2;

                for (uint8 l_I = 0; l_I < uint8(BattlegroundBracketType::Max); ++l_I)
                {
                    if (l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I)) > l_BestPrevWeekRating)
                    {
                        l_Bracket = BattlegroundBracketType(l_I);
                        l_BestPrevWeekRating = l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I));
                    }
                }

                std::vector<uint32> l_ItemBonuses;
                std::vector<uint32> l_Dummy;
                Item::GenerateItemBonus(m_ItemID, m_Elite ? ItemContext::Ilvl940 : ItemContext::Ilvl915, l_ItemBonuses, true, l_Player);

                if (Item* l_AddedItem = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_ItemBonuses))
                {
                    l_Player->SendNewItem(l_AddedItem, 1, true, false);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_season_6_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_season_6_item_SpellScript::HandleCreateItem, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_season_6_item_SpellScript();
        }
};

class spell_create_season_7_item : public SpellScriptLoader
{
public:
    spell_create_season_7_item() : SpellScriptLoader("spell_create_season_7_item") { }

    class spell_create_season_7_item_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_create_season_7_item_SpellScript);

        enum eSpells
        {
            SpellHelm       = 244424,
            SpellChest      = 244377,
            SpellLeggings   = 244425,
            SpellPauldrons  = 244426,
            SpellGloves     = 244423,
            SpellCinch      = 244427,
            SpellThreads    = 244379,
            SpellBracers    = 244428,
            SpellCloak      = 244378
        };

        enum eEliteSpells
        {
            EliteSpellHelm      = 244434,
            EliteSpellChest     = 244429,
            EliteSpellLeggings  = 244435,
            EliteSpellPauldrons = 244436,
            EliteSpellGloves    = 244432,
            EliteSpellCinch     = 244437,
            EliteSpellThreads   = 244431,
            EliteSpellBracers   = 244438,
            EliteSpellCloak     = 244430,

            MaxEliteSpells = 9
        };

        enum eBonuses
        {
            Warforged       = 560,
            Avoidance       = 40,
            Indestructible  = 43,
            Leech           = 41,
            Speed           = 42,
            PrismaticSocket = 563
        };

        enum eTokenBonuses
        {
            BonusHeroic = 570,
            BonusMythic = 569
        };

        std::array<uint32, eEliteSpells::MaxEliteSpells> m_EliteSpells
        {
            {
                eEliteSpells::EliteSpellHelm,
                eEliteSpells::EliteSpellChest,
                eEliteSpells::EliteSpellLeggings,
                eEliteSpells::EliteSpellPauldrons,
                eEliteSpells::EliteSpellGloves,
                eEliteSpells::EliteSpellCinch,
                eEliteSpells::EliteSpellThreads,
                eEliteSpells::EliteSpellBracers,
                eEliteSpells::EliteSpellCloak
            }
        };

        std::map<uint32, InventoryType> m_SpellInventoryType =
        {
            { eEliteSpells::EliteSpellHelm,         InventoryType::INVTYPE_HEAD },
            { eEliteSpells::EliteSpellChest,        InventoryType::INVTYPE_CHEST },
            { eEliteSpells::EliteSpellLeggings,     InventoryType::INVTYPE_LEGS },
            { eEliteSpells::EliteSpellPauldrons,    InventoryType::INVTYPE_SHOULDERS },
            { eEliteSpells::EliteSpellGloves,       InventoryType::INVTYPE_HANDS },
            { eEliteSpells::EliteSpellCinch,        InventoryType::INVTYPE_WAIST },
            { eEliteSpells::EliteSpellThreads,      InventoryType::INVTYPE_FEET },
            { eEliteSpells::EliteSpellBracers,      InventoryType::INVTYPE_WRISTS },
            { eEliteSpells::EliteSpellCloak,        InventoryType::INVTYPE_CLOAK },

            { eSpells::SpellHelm,                   InventoryType::INVTYPE_HEAD },
            { eSpells::SpellChest,                  InventoryType::INVTYPE_CHEST },
            { eSpells::SpellLeggings,               InventoryType::INVTYPE_LEGS },
            { eSpells::SpellPauldrons,              InventoryType::INVTYPE_SHOULDERS },
            { eSpells::SpellGloves,                 InventoryType::INVTYPE_HANDS },
            { eSpells::SpellCinch,                  InventoryType::INVTYPE_WAIST },
            { eSpells::SpellThreads,                InventoryType::INVTYPE_FEET },
            { eSpells::SpellBracers,                InventoryType::INVTYPE_WRISTS },
            { eSpells::SpellCloak,                  InventoryType::INVTYPE_CLOAK }
        };

        bool m_Elite;
        uint32 m_ItemID;

        bool Load() override
        {
            if (std::find(m_EliteSpells.begin(), m_EliteSpells.end(), GetSpellInfo()->Id) != m_EliteSpells.end())
                m_Elite = true;
            else
                m_Elite = false;

            m_ItemID = 0;
            return true;
        }

        SpellCastResult CheckCast()
        {
            Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
            if (!l_Player)
                return SpellCastResult::SPELL_FAILED_ERROR;

            uint32 l_LootSpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

            InventoryType l_InventoryType = InventoryType::INVTYPE_NON_EQUIP;

            auto const& l_Iter = m_SpellInventoryType.find(GetSpellInfo()->Id);
            if (l_Iter != m_SpellInventoryType.end())
                l_InventoryType = l_Iter->second;

            ItemTemplate const* l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, l_InventoryType, PvpSeasons::CurrentSeason);

            if (!l_GearReward && l_InventoryType == InventoryType::INVTYPE_CHEST)
                l_GearReward = sObjectMgr->GetItemRewardForPvp(l_LootSpecID, m_Elite ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, l_Player, InventoryType::INVTYPE_ROBE, PvpSeasons::CurrentSeason);

            if (l_GearReward != nullptr)
            {
                m_ItemID = l_GearReward->ItemId;

                ItemPosCountVec l_Dest;
                InventoryResult l_Msg = l_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, InventorySlot::NULL_SLOT, l_Dest, m_ItemID, 1);

                if (l_Msg != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }
            }
            else
                return SpellCastResult::SPELL_FAILED_DONT_REPORT;

            return SpellCastResult::SPELL_CAST_OK;
        }

        void HandleCreateItem(SpellEffIndex p_EffIndex)
        {
            PreventHitDefaultEffect(p_EffIndex);

            Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
            if (!l_Player || !m_ItemID)
                return;

            ItemPosCountVec l_Destination;
            InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

            if (l_Message != InventoryResult::EQUIP_ERR_OK)
                return;

            uint32 l_BestPrevWeekRating = 0;
            BattlegroundBracketType l_Bracket = BattlegroundBracketType::Arena2v2;

            for (uint8 l_I = 0; l_I < uint8(BattlegroundBracketType::Max); ++l_I)
            {
                if (l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I)) > l_BestPrevWeekRating)
                {
                    l_Bracket = BattlegroundBracketType(l_I);
                    l_BestPrevWeekRating = l_Player->GetBestRatingOfPreviousWeek(static_cast<uint8>(l_I));
                }
            }

            std::vector<uint32> l_ItemBonuses;
            std::vector<uint32> l_Dummy;
            Item::GenerateItemBonus(m_ItemID, m_Elite ? ItemContext::Ilvl950 : ItemContext::Ilvl925, l_ItemBonuses, true, l_Player);

            if (Item* l_AddedItem = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_ItemBonuses))
            {
                l_Player->SendNewItem(l_AddedItem, 1, true, false);
                l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
            }
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_create_season_7_item_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_create_season_7_item_SpellScript::HandleCreateItem, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_CREATE_LOOT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_create_season_7_item_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
class spell_item_trinkets_discipline_priest_nerf : public SpellScriptLoader
{
    public:
        spell_item_trinkets_discipline_priest_nerf() : SpellScriptLoader("spell_item_trinkets_discipline_priest_nerf") { }

        enum eSpells
        {
            DisciplinePriestAura  = 137032,
            AtonementAura         = 81749,
            ArtificialDamage      = 219655,
            BarrierOfTheDevoted   = 197815,
            PowerWordBarrier      = 81782,
            VimAndVigor           = 195488,
            VelensFutureSight     = 235966,
            AtonementHeal         = 81751,
            AtonementHealCrit     = 94472,

            OakheartsGnarledRoot  = 214350,
            DevilsaurShockLeash   = 224078,
            SquirrelExplosion     = 201303,
            PlagueSwarm           = 221812,
            InsidiousCorruption   = 243941,
            RefreshingAgony       = 253284,
            VoidTendril           = 250834,
            ThunderRitual         = 230224,
            UndendingHunger       = 242536,
            FlameWreath           = 230261, /// Aran's Relaxing Ruby
            /// Caged Horror (in another script)
            Nightfall             = 213786, /// Corrupted Starlight
            FelMeteor             = 214052, /// Eye of Skovald
            SolarCollapse         = 229737, /// Fury of the Burning Sky
            CarrionSwarm          = 225731, /// Icon of Rot (dot)
            KilJaedensBurningWish = 235999, /// Kil'jaeden's Burning Wish
            /// Pharamere's Forbidden Grimoire (in another script)
            NetherMeteor          = 225764, /// Star Gate
            TormentingCyclone     = 221865, /// Twisting Wind
            VolatileIchor         = 222197, /// Unstable Horrorslime
            /// Spectral Thurible (doesn't work)
            UmbralGlaiveStorm     = 242556, /// Umbral Moonglaives
            ShatteringUmbralGlaives     = 242557, /// Umbral Moonglaives
            PrototypePersonnelDecimator = 255629, /// Prototype Personnel Decimator
            /// Terminus Signalling Beacon (doesn't work)
            SpectralBolt          = 242571, /// Tarnished sentinel medallion
            SpectralBlast         = 246442, /// Tarnished sentinel medallion
            RancidMaw             = 215405,
            VolatileEnergy        = 230242,
            MaddeningWhispers     = 222052,
            PoisonedDreams        = 222711,
            OrbOfDestruction      = 229700,
            ShadowBlades          = 257702,
            Fireball              = 257241,
            Frostbolt             = 257242,
            ArcaneMissile         = 257243,
            DivineSmite           = 257532,
            Wrath                 = 257533,
            ShadowBolt            = 257534
        };

        class spell_item_trinkets_discipline_priest_nerf_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_trinkets_discipline_priest_nerf_SpellScript);

            void HandleOnHit()
            {
                Unit* l_OriginalCaster = GetCaster();
                if (!l_OriginalCaster)
                    return;

                Unit* l_Caster = l_OriginalCaster->GetCharmerOrOwnerOrSelf();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::DisciplinePriestAura))
                {
                    if (SpellInfo const* l_DisciplineInfo = sSpellMgr->GetSpellInfo(eSpells::DisciplinePriestAura))
                    {
                        int32 l_ReduceAmount = l_DisciplineInfo->Effects[EFFECT_4].BasePoints;
                        switch(m_scriptSpellId)
                        {
                            case eSpells::FlameWreath:
                            case eSpells::Nightfall:
                            case eSpells::FelMeteor:
                            case eSpells::SolarCollapse:
                            case eSpells::KilJaedensBurningWish:
                            case eSpells::NetherMeteor:
                            case eSpells::TormentingCyclone:
                            case eSpells::VolatileIchor:
                            case eSpells::PrototypePersonnelDecimator:
                            case eSpells::OrbOfDestruction:
                                l_ReduceAmount = l_DisciplineInfo->Effects[EFFECT_8].BasePoints;
                                break;
                        }

                        int32 l_Pct = 100.0f + l_ReduceAmount;
                        SetHitDamage(CalculatePct(GetHitDamage(), l_Pct));
                    }

                    switch (m_scriptSpellId)
                    {
                        case eSpells::SpectralBolt:
                        case eSpells::SpectralBlast:
                        case eSpells::UmbralGlaiveStorm:
                        case eSpells::ShatteringUmbralGlaives:
                        case eSpells::FlameWreath:
                        case eSpells::RancidMaw:
                        case eSpells::FelMeteor:
                        case eSpells::VolatileEnergy:
                        case eSpells::Nightfall:
                        case eSpells::SquirrelExplosion:
                        case eSpells::DevilsaurShockLeash:
                        case eSpells::VolatileIchor:
                        case eSpells::TormentingCyclone:
                        case eSpells::MaddeningWhispers:
                        case eSpells::PoisonedDreams:
                        case eSpells::SolarCollapse:
                        case eSpells::NetherMeteor:
                        case eSpells::OrbOfDestruction:
                        case eSpells::VoidTendril:
                        case eSpells::ShadowBlades:
                        case eSpells::PrototypePersonnelDecimator:
                        case eSpells::Fireball:
                        case eSpells::Frostbolt:
                        case eSpells::ArcaneMissile:
                        case eSpells::DivineSmite:
                        case eSpells::Wrath:
                        case eSpells::ShadowBolt:
                        {
                            if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::AtonementAura, EFFECT_0))
                            {
                                float l_Pct = l_AuraEffect->GetAmount();
                                int32 l_Damage = GetHitDamage();

                                if (Player* l_Player = l_Caster->ToPlayer())
                                {
                                    MS::Artifact::Manager* l_Artifact = l_Player->GetCurrentlyEquippedArtifact();
                                    if (l_Artifact)
                                    {
                                        if (l_Artifact->GetLevel() > 0 && l_Player->HasAura(eSpells::ArtificialDamage))
                                        {
                                            float l_PctDecreasing = (0.65f * (std::min((uint32)l_Artifact->GetLevel(), 52u) + 6)) / 100.0f + 1.0f;
                                            l_Damage /= l_PctDecreasing;
                                        }
                                    }
                                }

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

                                if (l_Caster->HasAura(eSpells::VimAndVigor))
                                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VimAndVigor))
                                        l_Damage = AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                                if (AuraEffect const* l_Effect = l_Caster->GetAuraEffect(eSpells::VelensFutureSight, SpellEffIndex::EFFECT_1))
                                    l_Damage = AddPct(l_Damage, l_Effect->GetAmount());

                                if (AuraEffect const* l_Effect = l_Caster->GetAuraEffect(eSpells::VelensFutureSight, SpellEffIndex::EFFECT_1))
                                    l_Damage = AddPct(l_Damage, l_Effect->GetAmount());

                                std::set<uint64> l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                                for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                                {
                                    Unit* l_AtonementTarget = ObjectAccessor::GetUnit(*l_Caster,  *l_Itr);

                                    if (!l_AtonementTarget)
                                        continue;

                                    int32 l_DamageForTarget = l_Damage;
                                    if (GetSpell()->IsCritForTarget(l_Target))
                                        l_DamageForTarget *= 2;

                                    l_Caster->CastCustomSpell(l_AtonementTarget, GetSpell()->IsCritForTarget(l_Target) ? eSpells::AtonementHealCrit : eSpells::AtonementHeal, &l_DamageForTarget, NULL, NULL, true);
                                }
                            }
                            break;
                        }
                    }
                }
            }

            void Register() override
            {
                switch(m_scriptSpellId)
                {
                    case eSpells::DevilsaurShockLeash:
                    case eSpells::SquirrelExplosion:
                    case eSpells::VoidTendril:
                    case eSpells::ThunderRitual:
                    case eSpells::FlameWreath:
                    case eSpells::Nightfall:
                    case eSpells::FelMeteor:
                    case eSpells::SolarCollapse:
                    case eSpells::KilJaedensBurningWish:
                    case eSpells::NetherMeteor:
                    case eSpells::TormentingCyclone:
                    case eSpells::VolatileIchor:
                    case eSpells::PrototypePersonnelDecimator:
                    case eSpells::SpectralBolt:
                    case eSpells::SpectralBlast:
                    case eSpells::ShatteringUmbralGlaives:
                    case eSpells::UmbralGlaiveStorm:
                    case eSpells::RancidMaw:
                    case eSpells::VolatileEnergy:
                    case eSpells::MaddeningWhispers:
                    case eSpells::PoisonedDreams:
                    case eSpells::OrbOfDestruction:
                    case eSpells::ShadowBlades:
                    case eSpells::Fireball:
                    case eSpells::Frostbolt:
                    case eSpells::ArcaneMissile:
                    case eSpells::DivineSmite:
                    case eSpells::Wrath:
                    case eSpells::ShadowBolt:
                    {
                        OnHit += SpellHitFn(spell_item_trinkets_discipline_priest_nerf_SpellScript::HandleOnHit);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_trinkets_discipline_priest_nerf_SpellScript();
        }

        class spell_item_trinkets_discipline_priest_nerf_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_trinkets_discipline_priest_nerf_AuraScript);

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_OriginalCaster = GetCaster();
                if (!l_OriginalCaster)
                    return;

                Unit* l_Caster = l_OriginalCaster->GetCharmerOrOwnerOrSelf();
                if (!l_Caster)
                    return;

                if (l_Caster->HasAura(eSpells::DisciplinePriestAura))
                {
                    if (SpellInfo const* l_DisciplineInfo = sSpellMgr->GetSpellInfo(eSpells::DisciplinePriestAura))
                    {
                        int32 l_Pct = 100.0f + l_DisciplineInfo->Effects[EFFECT_4].BasePoints;
                        if (m_scriptSpellId == eSpells::CarrionSwarm)
                            l_Pct = 100.0f + l_DisciplineInfo->Effects[EFFECT_8].BasePoints;

                        p_Amount = CalculatePct(p_Amount, l_Pct);
                    }
                }
            }

            void Register() override
            {
                switch(m_scriptSpellId)
                {
                    case eSpells::OakheartsGnarledRoot:
                    case eSpells::PlagueSwarm:
                    case eSpells::InsidiousCorruption:
                    case eSpells::RefreshingAgony:
                    case eSpells::CarrionSwarm:
                    {
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_trinkets_discipline_priest_nerf_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                        break;
                    }
                    case eSpells::UndendingHunger:
                    {
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_trinkets_discipline_priest_nerf_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_LEECH);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_trinkets_discipline_priest_nerf_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Fel-crazed Rage - 225777
class spell_item_fel_crazed_rage_damage : public SpellScriptLoader
{
    public:
        spell_item_fel_crazed_rage_damage() : SpellScriptLoader("spell_item_fel_crazed_rage_damage") { }

        class spell_item_fel_crazed_rage_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_fel_crazed_rage_damage_SpellScript);

            enum eSpells
            {
                MaddeningWhisper     = 222050,
                DisciplinePriestAura = 137032
            };

            void HandleOnHit(SpellEffIndex /* effIndex */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->IsActiveSpecTankSpec())
                    SetHitDamage(GetHitDamage() * 0.5f);
                if (l_Player->GetActiveSpecializationID() == SPEC_DK_UNHOLY)
                    SetHitDamage(GetHitDamage() * 0.7f);
                if (l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_ARMS)
                    SetHitDamage(GetHitDamage() * 0.8f);
                if (l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_FURY)
                    SetHitDamage(GetHitDamage() * 0.85f);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_fel_crazed_rage_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_fel_crazed_rage_damage_SpellScript();
        }
};

/// Last update 7.1.5 - Build 23420
/// Called for Cataclysm Quest item 'Dryad Spear' - 44995 / Spell 'Dryad Spear' - 62599
class spell_item_dryad_spear : public SpellScriptLoader
{
public:
    spell_item_dryad_spear() : SpellScriptLoader("spell_item_dryad_spear") {}

    class spell_item_dryad_spear_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_dryad_spear_SpellScript);

        enum eEntry
        {
            LorenthThundercall  = 32868,
            SheyaStormweaver    = 32869
        };

        SpellCastResult CheckTarget()
        {
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Target)
                return SPELL_FAILED_DONT_REPORT;

            if (l_Target->GetEntry() == eEntry::LorenthThundercall || l_Target->GetEntry() == eEntry::SheyaStormweaver)
                return SPELL_CAST_OK;

            return SPELL_FAILED_BAD_TARGETS;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_item_dryad_spear_SpellScript::CheckTarget);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_dryad_spear_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called for Aluriel's Mirror - 140795
/// Called by Nightwell Tranquility - 229670
class spell_item_nightwell_tranquility : public SpellScriptLoader
{
    public:
        spell_item_nightwell_tranquility() : SpellScriptLoader("spell_item_nightwell_tranquility") { }

        class spell_item_nightwell_tranquility_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_nightwell_tranquility_AuraScript);

            enum eSpells
            {
                NightwellTranquilityHeal = 225724
            };

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return false;

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target || !l_Target->HasAura(eSpells::NightwellTranquilityHeal, l_Caster->GetGUID()))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_nightwell_tranquility_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_nightwell_tranquility_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called for Inherited Mark of Tyranny - 93900
/// Called by Tremendous Fortitude - 137211
class spell_item_tremendous_fortitude : public SpellScriptLoader
{
    public:
        spell_item_tremendous_fortitude() : SpellScriptLoader("spell_item_tremendous_fortitude") { }

        class spell_item_tremendous_fortitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_tremendous_fortitude_AuraScript);

            enum eSpells
            {
                NightwellTranquilityHeal = 225724
            };

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                uint32 l_MaxHealth = l_Target->GetMaxHealth();
                uint32 l_HealthBuff = CalculatePct(l_MaxHealth, p_AurEff->GetAmount());
                if (l_HealthBuff > 180000) ///< Not handled generically, but this buff has a "cap" in max health bonus
                    l_HealthBuff = 180000;

                AuraEffect* l_AuraEff = l_Target->GetAuraEffect(m_scriptSpellId, EFFECT_1);
                if (!l_AuraEff)
                    return;

                l_AuraEff->ChangeAmount(l_HealthBuff);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_tremendous_fortitude_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_MOD_INCREASE_HEALTH_2, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_tremendous_fortitude_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Infernal Skin - 242209
/// Called for Feverish Carapace (item) - 147022
class spell_item_infernal_skin : public SpellScriptLoader
{
    public:
        spell_item_infernal_skin() : SpellScriptLoader("spell_item_infernal_skin") { }

    class spell_item_infernal_skin_AuraScript : public AuraScript
    {
        enum eSpells
        {
            FoveredTouch = 242650
        };

        PrepareAuraScript(spell_item_infernal_skin_AuraScript);

        void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Attacker = p_DamageInfo.GetActor();
            if (!l_Attacker)
                return;

            if (p_DamageInfo.GetSpellInfo() && p_DamageInfo.GetSpellInfo()->Id == eSpells::FoveredTouch)
                return;

            if (p_DamageInfo.GetSpell() && p_DamageInfo.GetSpell()->IsTriggered())
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetAura()->GetCastItemGUID());
            if (!l_Trinket)
                return;

            float l_Multiplier = 0.0f;
            int32 l_TrinketItemLevel = l_Trinket->GetItemLevel(l_Player);
            RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
            if (l_RandomPropertiesPoints)
                l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

            SpellInfo const* l_InfernalSkinInfo = sSpellMgr->GetSpellInfo(FoveredTouch);
            if (!l_InfernalSkinInfo)
                return;

            float l_ScalingMultiplier = l_InfernalSkinInfo->Effects[EFFECT_0].ScalingMultiplier;
            float l_Amount = l_ScalingMultiplier * l_Multiplier;
            float l_DeltaScalingMultiplier = l_InfernalSkinInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
            if (l_DeltaScalingMultiplier)
            {
                float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                l_Amount += frand(-l_Delta, l_Delta);
            }

            uint32 l_Damage = (uint32)l_Amount;
            p_AbsorbAmount = l_Damage;
            int32 l_Bp0 = l_Damage;
            l_Caster->CastCustomSpell(l_Attacker, eSpells::FoveredTouch, &l_Bp0, nullptr, nullptr, true, l_Trinket);

            return;
        }

        void Register() override
        {
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_infernal_skin_AuraScript::HandleOnAbsorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
        }
    };

    AuraScript* GetAuraScript() const override
    {

        return new spell_item_infernal_skin_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Guilty Conscience - 242325
class spell_item_guilty_conscience : public SpellScriptLoader
{
public:
    spell_item_guilty_conscience() : SpellScriptLoader("spell_item_guilty_conscience") { }

    class spell_item_guilty_conscience_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_guilty_conscience_AuraScript);

        enum eSpells
        {
            GuiltyConscience = 242327
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
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

            if (l_SpellInfo->IsHealingSpell())
                return true;

            return false;
        }

        void HandleProcPerMinutes(float & p_ProcsPerMinute, ProcEventInfo & p_Proc)
        {
            if (GetUnitOwner() == nullptr)
                return;

            Unit* l_Owner = GetUnitOwner();

            if (!l_Owner->IsPlayer())
                return;

            float l_ManaPct = l_Owner->GetPowerPct(Powers::POWER_MANA);
            float totalHaste = l_Owner->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_HASTE_RANGED) * l_Owner->ToPlayer()->GetRatingMultiplier(CR_HASTE_RANGED) / 100.0f;
            p_ProcsPerMinute *= (1 + totalHaste);
            p_ProcsPerMinute += l_ManaPct * 0.075;
        }

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_EventInfo.GetProcTarget();
            Aura* l_Aura = GetAura();
            if (!l_Caster || !l_Caster->IsPlayer() || !l_Target || !l_Aura)
                return;

            Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
            if (!l_Item)
                return;

            l_Caster->CastSpell(l_Target, eSpells::GuiltyConscience, true, l_Item);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_guilty_conscience_AuraScript::HandleOnCheckProc);
            DoCalcPPM += AuraProcPerMinuteFn(spell_item_guilty_conscience_AuraScript::HandleProcPerMinutes);
            OnProc += AuraProcFn(spell_item_guilty_conscience_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_guilty_conscience_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Strength of Will - 242640
class spell_item_strength_of_will : public SpellScriptLoader
{
public:
    spell_item_strength_of_will() : SpellScriptLoader("spell_item_strength_of_will") { }

    class spell_item_strength_of_will_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_strength_of_will_AuraScript);

        enum eSpells
        {
            StrengthOfWill = 242642
        };

        void OnTick(AuraEffect const* p_AurEff)
        {
            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            if (!l_Aura || !l_Caster || !l_Caster->IsPlayer())
                return;

            Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
            if (!l_Item)
                return;

            if (l_Caster->GetHealthPct() > p_AurEff->GetAmount())
                l_Caster->CastSpell(l_Caster, eSpells::StrengthOfWill, true, l_Item);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_strength_of_will_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_strength_of_will_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Strength of Will Proc - 242642
class spell_item_strength_of_will_proc : public SpellScriptLoader
{
    public:
        spell_item_strength_of_will_proc() : SpellScriptLoader("spell_item_strength_of_will_proc") { }

        class spell_item_strength_of_will_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_strength_of_will_proc_AuraScript);

            enum eSpells
            {
                StrengthOfWill = 242640
            };

            void OnUpdate(uint32 /*p_Diff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasAura(eSpells::StrengthOfWill))
                    l_Caster->RemoveAura(m_scriptSpellId);
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_item_strength_of_will_proc_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_strength_of_will_proc_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Rising Tides - 242458
/// Called for Charm of the Rising Tide (item) - 147002
class spell_item_rising_tides : public SpellScriptLoader
{
    public:
        spell_item_rising_tides() : SpellScriptLoader("spell_item_rising_tides") { }

        class spell_item_rising_tides_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_rising_tides_AuraScript);

            void HandlePeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_MainAura = GetAura();
                if (!l_MainAura)
                    return;

                int32 l_Duration = l_MainAura->GetDuration();

                if (!l_Caster->IsMoving())
                {
                    l_MainAura->ModStackAmount(1);
                    l_MainAura->SetDuration(l_Duration);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_rising_tides_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_rising_tides_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Ocean's Embrace - 242459
/// Called for Sea Star of the Depthmother (item) - 147004
class spell_item_oceans_embrace : public SpellScriptLoader
{
    public:
        spell_item_oceans_embrace() : SpellScriptLoader("spell_item_oceans_embrace") { }

        class spell_item_oceans_embrace_AuraScript : public AuraScript
        {
            enum eSpells
            {
                OceansEmbraceDruid    = 242460,
                OceansEmbraceMonk     = 242461,
                OceansEmbracePaladin  = 242462,
                OceansEmbracePreist   = 242463,
                OceansEmbracePreist2  = 242464,
                OceansEmbraceShaman   = 242465
            };

            std::vector<eSpells> m_Spells =
            {
                eSpells::OceansEmbraceDruid,
                eSpells::OceansEmbraceMonk,
                eSpells::OceansEmbracePaladin,
                eSpells::OceansEmbracePreist,
                eSpells::OceansEmbracePreist2,
                eSpells::OceansEmbraceShaman
            };

            PrepareAuraScript(spell_item_oceans_embrace_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (Player* l_Player = l_Target->ToPlayer())
                    {
                        uint32 l_SpellID = 0;
                        switch (l_Player->GetActiveSpecializationID())
                        {
                            case SPEC_DRUID_RESTORATION:
                                l_SpellID = eSpells::OceansEmbraceDruid;
                                break;
                            case SPEC_MONK_MISTWEAVER:
                                l_SpellID = eSpells::OceansEmbraceMonk;
                                break;
                            case SPEC_PALADIN_HOLY:
                                l_SpellID = eSpells::OceansEmbracePaladin;
                                break;
                            case SPEC_PRIEST_DISCIPLINE:
                                l_SpellID = eSpells::OceansEmbracePreist;
                                break;
                            case SPEC_PRIEST_HOLY:
                                l_SpellID = eSpells::OceansEmbracePreist2;
                                break;
                            case SPEC_SHAMAN_RESTORATION:
                                l_SpellID = eSpells::OceansEmbraceShaman;
                                break;
                        }

                        if (l_SpellID != 0)
                            l_Player->CastSpell(l_Player, l_SpellID, true);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    for (auto l_SpellID : m_Spells)
                        if (l_Target->HasAura(l_SpellID))
                            l_Target->RemoveAura(l_SpellID);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_oceans_embrace_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_oceans_embrace_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_oceans_embrace_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Wild Growth (48438), Essence Font (191837), Prayer of Healing (596), Power Word: Radiance (194509), Light of Dawn (85222), Chain Heal (1064)
/// Called for Ocean's Embrace - (242460, 242461, 242462, 242463, 242464, 242465)
/// Called for item Sea Star of the Depthmother - 147004
class spell_item_oceans_embrace_procs : public SpellScriptLoader
{
    public:
        spell_item_oceans_embrace_procs() : SpellScriptLoader("spell_item_oceans_embrace_procs") { }

        class spell_item_oceans_embrace_procs_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_oceans_embrace_procs_SpellScript);

            enum eSpells
            {
                OceansEmbraceMainAura   = 242459,
                OceansEmbraceBuff       = 242467
            };


            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Aura* l_MainAura = l_Caster->GetAura(eSpells::OceansEmbraceMainAura);
                if (!l_MainAura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                int32 l_CustomProcChance = 0;

                switch (l_Player->GetActiveSpecializationID()) ///< Should use the deck card system, which isnt implemented yet https://gyazo.com/ebd31c9249fa1a87b1c4f55f63bc2390 from http://www.wowhead.com/item=147004/sea-star-of-the-depthmother&bonus=3561:1482#comments
                {
                    case SPEC_PRIEST_HOLY:
                    case SPEC_PALADIN_HOLY:
                    case SPEC_SHAMAN_RESTORATION:
                        l_CustomProcChance = 40;
                        break;
                    case SPEC_MONK_MISTWEAVER:
                    case SPEC_DRUID_RESTORATION:
                        l_CustomProcChance = 50;
                        break;
                    case SPEC_PRIEST_DISCIPLINE:
                        l_CustomProcChance = 60;
                        break;
                    default:
                        break;
                }

                Item* l_Item = l_Player->GetItemByGuid(l_MainAura->GetCastItemGUID());
                if (!l_Item)
                    return;

                if (roll_chance_i(l_CustomProcChance) && !l_Caster->HasAura(eSpells::OceansEmbraceBuff)) ///< Proc has a 8sec Internal CD with a 8sec buff duration, since it's hardcoded, cba making a proper fix https://gyazo.com/b67688751faed0078a36d5783c40712e
                    l_Caster->CastSpell(l_Caster, OceansEmbraceBuff, true, l_Item);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_oceans_embrace_procs_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_oceans_embrace_procs_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Ocean's Embrace - 242474 AND Emerald Blossom 257442
/// Called for item Sea Star of the Depthmother - 147004 AND Eonar's Compassion - 154175
class spell_item_oceans_embrace_targeting : public SpellScriptLoader
{
    public:
        spell_item_oceans_embrace_targeting() : SpellScriptLoader("spell_item_oceans_embrace_targeting") { }

        class spell_item_oceans_embrace_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_oceans_embrace_targeting_SpellScript);

            enum eSpells
            {
                OceansEmbraceBuff = 242467
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!p_Targets.empty())
                {
                    p_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                    p_Targets.resize(1);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_oceans_embrace_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_oceans_embrace_targeting_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Ceaseless Toxin - 242497
/// Called for item Vial of Ceaseless Toxins - 147011
class spell_item_ceaseless_toxin : public SpellScriptLoader
{
    public:
        spell_item_ceaseless_toxin() : SpellScriptLoader("spell_item_ceaseless_toxin") { }

        class spell_item_ceaseless_toxin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_ceaseless_toxin_AuraScript);

            void HandleProc(AuraEffect const*, ProcEventInfo& p_ProcInfo)
            {
                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                Unit* l_Caster = l_Aura->GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasSpellCooldown(GetSpellInfo()->Id))
                    l_Player->ReduceSpellCooldown(GetSpellInfo()->Id, GetSpellInfo()->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_ceaseless_toxin_AuraScript::HandleProc, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_ceaseless_toxin_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Terror From Below - 242525
/// Called for item Terror From Below - 147016
class spell_item_terror_from_below_damage : public SpellScriptLoader
{
    public:
        spell_item_terror_from_below_damage() : SpellScriptLoader("spell_item_terror_from_below_damage") { }

        class spell_item_terror_from_below_damage_SpellScript : public SpellScript
        {
            enum eSpells
            {
                DisciplinePriestAura  = 137032,
                AtonementAura         = 81749,
                ArtificialDamage      = 219655,
                BarrierOfTheDevoted   = 197815,
                PowerWordBarrier      = 81782,
                VimAndVigor           = 195488,
                AtonementHeal         = 81751,
                AtonementHealCrit     = 94472,
                VelensFutureSight     = 235966
            };

            PrepareSpellScript(spell_item_terror_from_below_damage_SpellScript);

            uint32 m_TargetsCount = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                WorldLocation const* l_TargetDest = GetExplTargetDest();
                if (!l_TargetDest)
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (l_Caster->GetExactDist2d(l_TargetDest) <= GetSpellInfo()->Effects[EFFECT_0].CalcRadius())
                    p_Targets.push_back(l_Caster);

                m_TargetsCount = p_Targets.size();
            }

            void HandleOnHit(SpellEffIndex /* effIndex */)
            {
                Unit* l_OriginalCaster = GetCaster();
                if (!l_OriginalCaster)
                    return;

                Unit* l_Caster = l_OriginalCaster->GetCharmerOrOwnerOrSelf();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (l_Caster->HasAura(eSpells::DisciplinePriestAura))
                {
                    if (SpellInfo const* l_DisciplineInfo = sSpellMgr->GetSpellInfo(eSpells::DisciplinePriestAura))
                    {
                        int32 l_ReduceAmount = l_DisciplineInfo->Effects[EFFECT_4].BasePoints;
                        int32 l_Pct = 100.0f + l_ReduceAmount;
                        SetHitDamage(CalculatePct(GetHitDamage(), l_Pct));
                    }
                }

                SetHitDamage(GetHitDamage() / m_TargetsCount);

                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::AtonementAura, EFFECT_0))
                {
                    float l_Pct = l_AuraEffect->GetAmount();
                    int32 l_Damage = GetHitDamage();

                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        MS::Artifact::Manager* l_Artifact = l_Player->GetCurrentlyEquippedArtifact();
                        if (l_Artifact)
                        {
                            if (l_Artifact->GetLevel() > 0 && l_Player->HasAura(eSpells::ArtificialDamage))
                            {
                                float l_PctDecreasing = (0.65f * (std::min((uint32)l_Artifact->GetLevel(), 52u) + 6)) / 100.0f + 1.0f;
                                l_Damage /= l_PctDecreasing;
                            }
                        }
                    }

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

                    if (l_Caster->HasAura(eSpells::VimAndVigor))
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VimAndVigor))
                            l_Damage = AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

                    if (AuraEffect const* l_Effect = l_Caster->GetAuraEffect(eSpells::VelensFutureSight, SpellEffIndex::EFFECT_1))
                        l_Damage = AddPct(l_Damage, l_Effect->GetAmount());

                    std::set<uint64> l_TargetGUIDs = l_Caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                    for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                    {
                        Unit* l_AtonementTarget = ObjectAccessor::GetUnit(*l_Caster,  *l_Itr);

                        if (!l_AtonementTarget)
                            continue;

                        if (GetSpell()->IsCritForTarget(l_Target))
                            l_Damage *= 2;

                        l_Caster->CastCustomSpell(l_AtonementTarget, GetSpell()->IsCritForTarget(l_Target) ? eSpells::AtonementHealCrit : eSpells::AtonementHeal, &l_Damage, NULL, NULL, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_terror_from_below_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_terror_from_below_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_terror_from_below_damage_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Terror From Below - 242524
/// Called for item Terror From Below - 147016
class spell_item_terror_from_below_proc : public SpellScriptLoader
{
    public:
        spell_item_terror_from_below_proc() : SpellScriptLoader("spell_item_terror_from_below_proc") { }

        class spell_item_terror_from_below_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_terror_from_below_proc_AuraScript);

            enum eSpells
            {
                TerrorFromBelowTrigger = 246560
            };

            void HandleProc(AuraEffect const*, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                Unit* l_Target = p_EventInfo.GetActionTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::TerrorFromBelowTrigger, true, l_Item);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_terror_from_below_proc_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_terror_from_below_proc_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Chalice of Moonlight - 242541
class spell_item_chalice_of_moonlight : public SpellScriptLoader
{
public:
    spell_item_chalice_of_moonlight() : SpellScriptLoader("spell_item_chalice_of_moonlight") { }

    class spell_item_chalice_of_moonlight_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_chalice_of_moonlight_AuraScript);

        enum eSpells
        {
            LunarInfusion = 242543,
            SolarInfusion = 242544,
            EventNight = 25 // Id on game_event
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return false;

            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo || l_DamageInfo->GetDamageType() != DamageEffectType::HEAL)
                return false;

            return true;
        }

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            if (!l_Caster || !l_Caster->IsPlayer() || !l_Aura)
                return;

            Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
            if (!l_Item)
                return;

            int32 l_Amount = 0;
            if (AuraEffect const* l_Effect = GetAura()->GetEffect(SpellEffIndex::EFFECT_1))
                l_Amount = l_Effect->GetAmount();

            if (l_Amount <= 0)
                return;

            if (sGameEventMgr->IsActiveEvent(eSpells::EventNight))
                l_Caster->CastCustomSpell(eSpells::LunarInfusion, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Amount, l_Caster, true, l_Item);
            else
                l_Caster->CastCustomSpell(eSpells::SolarInfusion, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Amount, l_Caster, true, l_Item);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_chalice_of_moonlight_AuraScript::HandleOnCheckProc);
            OnProc += AuraProcFn(spell_item_chalice_of_moonlight_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_chalice_of_moonlight_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Spectral Owl - 242570
class spell_item_spectral_owl : public SpellScriptLoader
{
    public:

        spell_item_spectral_owl() : SpellScriptLoader("spell_item_spectral_owl") { }

        enum eSpells
        {
            SpectralOwl     = 242570,
            SpectralBolt    = 242571,
            SpectralBlast   = 246442
        };

        enum eData
        {
            TargetGuid = 1
        };

        class spell_item_spectral_owl_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_spectral_owl_SpellScript);

            void HandleHitTarget(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (l_Caster == nullptr || l_Target == nullptr)
                    return;

                uint64 l_TargetGuid = l_Target->GetGUID();

                l_Caster->AddDelayedEvent([l_Caster, l_TargetGuid]() -> void
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::SpectralOwl))
                        l_Aura->SetScriptGuid(eData::TargetGuid, l_TargetGuid);
                }, 100);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_spectral_owl_SpellScript::HandleHitTarget, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_spectral_owl_SpellScript();
        }

        class spell_item_spectral_owl_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_spectral_owl_AuraScript);

            void SetGuid(uint32 p_Type, uint64 p_Guid) override
            {
                if (p_Type == eData::TargetGuid)
                    m_MainTargetGuid = p_Guid;
            }

            void HandleEffectProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetProcTarget();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Caster->IsPlayer() || !l_Target || !l_Aura || l_Target->GetGUID() != m_MainTargetGuid)
                    return;

                Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpectralBlast, true, l_Item);
            }

            void HandlePeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Caster->IsPlayer() || !l_Aura)
                    return;

                Unit* l_Target = l_Caster->GetUnit(*l_Caster, m_MainTargetGuid);
                if (!l_Target || !l_Caster->IsValidAttackTarget(l_Target))
                    return;

                Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpectralBolt, true, l_Item);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_spectral_owl_AuraScript::HandleEffectProc, SpellEffIndex::EFFECT_2, AuraType::SPELL_AURA_DUMMY);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_spectral_owl_AuraScript::HandlePeriodic, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }

        private:

            uint64 m_MainTargetGuid = 0;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_spectral_owl_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Wailing Souls - 242609
class spell_item_wailing_souls : public SpellScriptLoader
{
public:
    spell_item_wailing_souls() : SpellScriptLoader("spell_item_wailing_souls") {}

    class spell_item_wailing_souls_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_wailing_souls_AuraScript);

        void HandleOnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
        {
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            Unit* l_Target = GetTarget();
            Unit* l_Caster = GetCaster();

            if (!l_Target || !l_Caster || !l_SpellInfo)
                return;

            if (!m_First)
            {
                m_First = true;
                m_MaxAbsorb = p_AurEff->GetAmount();
                m_Absorb = p_AurEff->GetAmount();
            }

            int32 l_AbsorbPct = l_SpellInfo->Effects[EFFECT_1].BasePoints;
            int32 l_AbsorbCapacity = p_AurEff->GetAmount();
            int32 l_ToAbsorb = CalculatePct(p_DmgInfo.GetAmount(), l_AbsorbPct);

            if (l_ToAbsorb > l_AbsorbCapacity)
                l_ToAbsorb = l_AbsorbCapacity;

            p_AbsorbAmount = l_ToAbsorb;
            p_AurEff->ChangeAmount(l_AbsorbCapacity - l_ToAbsorb);
            m_Absorb = p_AurEff->GetAmount();
        }

        void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->IsPlayer())
                return;

            float l_Pct = float(100.0f - (100.0f * ((float)m_Absorb / (float)m_MaxAbsorb)));

            if (l_Pct > p_AurEff->GetAmount())
                l_Pct = p_AurEff->GetAmount();

            l_Caster->ToPlayer()->ReduceSpellCooldown(m_scriptSpellId, uint32((uint32)l_Pct * 1000));
        }

        void Register() override
        {
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_wailing_souls_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_wailing_souls_AuraScript::HandleAfterRemove, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }

    private:
        int32 m_Absorb = 0;
        int32 m_MaxAbsorb = 0;
        bool m_First = false;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_wailing_souls_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called By Grace of the Creators - 242617
class spell_item_grace_of_the_creators : public SpellScriptLoader
{
public:
    spell_item_grace_of_the_creators() : SpellScriptLoader("spell_item_grace_of_the_creators") {}

    class spell_item_grace_of_the_creators_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_grace_of_the_creators_AuraScript);

        enum eSpells
        {
            BulwarkOfGrace = 242618
        };

        void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
        {
            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            if (!l_Aura || !l_Caster || !l_Caster->IsPlayer() || !IsExpired())
                return;

            Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
            if (!l_Item)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::BulwarkOfGrace, true, l_Item);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_grace_of_the_creators_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_grace_of_the_creators_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called By Cleansing Matrix - 242619
class spell_item_cleansing_matrix : public SpellScriptLoader
{
public:
    spell_item_cleansing_matrix() : SpellScriptLoader("spell_item_cleansing_matrix") {}

    class spell_item_cleansing_matrix_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_cleansing_matrix_AuraScript);

        enum eSpells
        {
            InfusionOfLight = 242621
        };

        void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
        {
            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();
            if (!l_Aura || !l_Caster || !l_Caster->IsPlayer() || !l_Target || !IsExpired())
                return;

            Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
            if (!l_Item)
                return;

            if (!l_Caster->IsPlayer())
                return;

            uint64 l_TargetGUID = l_Target->GetGUID();
            uint64 l_ItemGUID = l_Item->GetGUID();
            l_Caster->AddDelayedEvent([l_Caster, l_TargetGUID, l_ItemGUID]() -> void
            {
                if (Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGUID))
                {
                    if (Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_ItemGUID))
                        l_Caster->CastSpell(l_Target, eSpells::InfusionOfLight, true, l_Item);
                }
            }, 5);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_cleansing_matrix_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_cleansing_matrix_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called By Guiding Hand - 242622
class spell_item_guiding_hand : public SpellScriptLoader
{
public:
    spell_item_guiding_hand() : SpellScriptLoader("spell_item_guiding_hand") {}

    class spell_item_guiding_hand_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_guiding_hand_AuraScript);

        enum eSpells
        {
            FruitfulMachinations = 242623
        };

        void HandleProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            PreventDefaultAction();

            Unit* l_Caster = GetCaster();
            Aura* l_Aura = GetAura();
            if (!l_Caster || !l_Aura)
                return;

            if (l_Caster->GetHealthPct() > p_AurEff->GetAmount())
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
            if (!l_Item)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::FruitfulMachinations, true, l_Item);
            Remove();
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_item_guiding_hand_AuraScript::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_guiding_hand_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called By Insidious Corruption - 243941
class spell_item_insidious_corruption : public SpellScriptLoader
{
public:
    spell_item_insidious_corruption() : SpellScriptLoader("spell_item_insidious_corruption") {}

    class spell_item_insidious_corruption_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_insidious_corruption_AuraScript);

        enum eSpells
        {
            ExtractedSanity = 243942
        };

        void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
        {
            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            if (!l_Aura || !l_Caster || !l_Caster->IsPlayer())
                return;

            Item* l_Item = l_Caster->ToPlayer()->GetItemByGuid(l_Aura->GetCastItemGUID());
            if (!l_Item)
                return;

            int32 l_Duration = l_Aura->GetDuration();

            l_Caster->CastSpell(l_Caster, eSpells::ExtractedSanity, true, l_Item);
            if (l_Duration)
            {
                if (Aura* l_Aura = l_Caster->GetAura(eSpells::ExtractedSanity, l_Caster->GetGUID(), l_Item->GetGUID()))
                {
                    l_Aura->SetDuration(l_Aura->GetDuration() + l_Duration);
                }
            }
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_insidious_corruption_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_insidious_corruption_AuraScript();
    }
};

/// Called by Acceleration - 214128
/// For Journey Through Time set (spell id 228447)
class spell_item_chrono_shard_acceleration : public SpellScriptLoader
{
    public:
        spell_item_chrono_shard_acceleration() : SpellScriptLoader("spell_item_chrono_shard_acceleration") {}

        enum eSpells
        {
            JourneyThroughTimeSet = 228447,
        };

        class spell_item_chrono_shard_acceleration_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_chrono_shard_acceleration_AuraScript);

            void CalculateAmounts(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                SpellEffIndex l_setEffect = p_AurEff->GetEffIndex() == EFFECT_0 ? EFFECT_1 : EFFECT_0;
                if (AuraEffect* l_SetAura = l_Caster->GetAuraEffect(eSpells::JourneyThroughTimeSet, l_setEffect))
                    if (SpellInfo const* l_SpellInfo = l_SetAura->GetSpellInfo())
                    {
                        int32 l_Bp = l_SpellInfo->Effects[l_setEffect].BasePoints;
                        p_Amount = l_setEffect == EFFECT_1 ? p_Amount + l_Bp : l_Bp;
                    }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_chrono_shard_acceleration_AuraScript::CalculateAmounts, EFFECT_0, SPELL_AURA_MOD_RATING);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_chrono_shard_acceleration_AuraScript::CalculateAmounts, EFFECT_1, SPELL_AURA_MOD_SPEED_ALWAYS);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_chrono_shard_acceleration_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Cunning of the Deceiver - 242629
class spell_item_cunning_of_the_deceiver : public SpellScriptLoader
{
public:
    spell_item_cunning_of_the_deceiver() : SpellScriptLoader("spell_item_cunning_of_the_deceiver") { }

    class spell_item_cunning_of_the_deceiver_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_cunning_of_the_deceiver_SpellScript);

        enum eSpells
        {
            IceboundFortitude       = 48792,
            Metamorphosis           = 187827,
            SurvivalInstincts       = 61336,
            FortifyingBrew          = 115203,
            FortifyingBrewAura      = 120954,
            GuardianOfAncientKings  = 86659,
            ShieldWall              = 871,
            CunningOfTheDeceiver    = 242630
        };

        std::vector<uint32> m_SpellList = { eSpells::IceboundFortitude, eSpells::Metamorphosis, eSpells::SurvivalInstincts,
            eSpells::FortifyingBrew, eSpells::GuardianOfAncientKings, eSpells::ShieldWall };

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = GetCaster()->ToPlayer();
            if (!l_Player)
                return;

            for (uint32 l_SpellId : m_SpellList)
            {
                if (!l_Player->HasSpell(l_SpellId))
                    continue;

                if (Aura* l_Aura = l_Player->GetAura(eSpells::CunningOfTheDeceiver))
                {
                    uint32 l_ReduceCooldown = 10 * l_Aura->GetStackAmount() * IN_MILLISECONDS;
                    float l_Coeff = 1.0f;

                    if (l_SpellId == eSpells::IceboundFortitude || l_SpellId == eSpells::SurvivalInstincts)
                        l_Coeff = 0.5f;
                    if (l_SpellId == eSpells::Metamorphosis || l_SpellId == eSpells::ShieldWall)
                        l_Coeff = 0.75f;

                    int32 l_ModDuration = l_Coeff * l_Aura->GetStackAmount() * IN_MILLISECONDS;

                    if (Aura* l_Aura2 = l_Player->GetAura(l_SpellId == eSpells::FortifyingBrew ? eSpells::FortifyingBrewAura : l_SpellId))
                        l_Aura2->SetDuration(l_Aura2->GetDuration() + l_ModDuration);
                    else if (l_SpellId == eSpells::SurvivalInstincts)
                    {
                        if (SpellInfo const* l_SpellInfoSurvivalInstincts = sSpellMgr->GetSpellInfo(eSpells::SurvivalInstincts))
                            if (SpellCategoryEntry const* l_CategoryEntry = l_SpellInfoSurvivalInstincts->ChargeCategoryEntry)
                                l_Player->ReduceChargeCooldown(l_CategoryEntry, l_ReduceCooldown);
                    }
                    else if (l_Player->HasSpellCooldown(l_SpellId))
                        l_Player->ReduceSpellCooldown(l_SpellId, l_ReduceCooldown);

                    l_Aura->Remove();
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_item_cunning_of_the_deceiver_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_cunning_of_the_deceiver_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Dreadstone - 238498
class spell_item_dreadstone_of_endless_shadows : public SpellScriptLoader
{
    public:
        spell_item_dreadstone_of_endless_shadows() : SpellScriptLoader("spell_item_dreadstone_of_endless_shadows") {}

        enum eSpells
        {
            SpellSignOfTheHyppo  = 225749,
            SpellSignOfTheHare   = 225752,
            SpellSignOfDragon    = 225753,
        };

        class spell_item_dreadstone_of_endless_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_dreadstone_of_endless_shadows_AuraScript);

            void HandleOnProc(ProcEventInfo& p_Proc)
            {
                if (!GetUnitOwner())
                    return;

                uint32 l_Index = urand(0, 2);
                uint32 l_Amount = 0;

                if (AuraEffect* l_AurEff = GetUnitOwner()->GetAuraEffect(GetId(), EFFECT_1))
                    l_Amount += l_AurEff->GetAmount()-1;

                switch (l_Index)
                {
                    case 0:
                    {
                        GetUnitOwner()->CastCustomSpell(eSpells::SpellSignOfTheHyppo, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true);
                        break;
                    }

                    case 1:
                    {
                        GetUnitOwner()->CastCustomSpell(eSpells::SpellSignOfTheHare, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true);
                        break;
                    }

                    case 2:
                    {
                        GetUnitOwner()->CastCustomSpell(eSpells::SpellSignOfDragon, SPELLVALUE_BASE_POINT0, l_Amount, GetUnitOwner(), true);
                        break;
                    }

                    default: break;
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_dreadstone_of_endless_shadows_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_dreadstone_of_endless_shadows_AuraScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Umbral Umbral Glaive Storm - 242556, 242557, Infernal Cinders - 242218, Ceaseless Toxin - 242497, Dread Torrent - 246464
class spell_item_tos_melee_dps_trinkets : public SpellScriptLoader
{
    public:
        spell_item_tos_melee_dps_trinkets() : SpellScriptLoader("spell_item_tos_melee_dps_trinkets") { }

        enum eSpells
        {
            UmbralGlaive1   = 242556,
            UmbralGlaive2   = 242557,
            CeaselessToxin  = 242497,
            FuryofNatureAura = 248083,
            FuryofNatureHeal = 248522,
            InfernalCinders  = 242218,
            DisciplinePriestAura = 137032
        };

        class spell_item_tos_melee_dps_trinkets_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_tos_melee_dps_trinkets_SpellScript);

            void HandleOnHit(SpellEffIndex /* effIndex */)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Caster->HasAura(eSpells::DisciplinePriestAura))
                {
                    if (m_scriptSpellId == eSpells::UmbralGlaive1 || m_scriptSpellId == eSpells::UmbralGlaive2)
                        return;

                    if (SpellInfo const* l_DisciplineInfo = sSpellMgr->GetSpellInfo(eSpells::DisciplinePriestAura))
                    {
                        int32 l_Pct = 100.0f + l_DisciplineInfo->Effects[EFFECT_8].BasePoints;
                        SetHitDamage(CalculatePct(GetHitDamage(), l_Pct));
                        return;
                    }
                }

                if (!l_Player->IsMeleeDamageDealer()) ///< should do 50% less dmg
                    SetHitDamage(GetHitDamage() / 2);

                if (l_Player->GetActiveSpecializationID() == SPEC_DK_UNHOLY) ///< should do 30% less dmg
                    SetHitDamage(CalculatePct(GetHitDamage(), 70));

                if (l_Player->GetActiveSpecializationID() == SPEC_SHAMAN_ENHANCEMENT && m_scriptSpellId == eSpells::InfernalCinders) ///< should do 30% less dmg
                    SetHitDamage(CalculatePct(GetHitDamage(), 70));

                if (l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_FURY || l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_ARMS) ///< should do 40% less dmg
                    SetHitDamage(CalculatePct(GetHitDamage(), 60));

                if (l_Caster->HasAura(eSpells::FuryofNatureAura) && (m_scriptSpellId == eSpells::UmbralGlaive1 || m_scriptSpellId == eSpells::UmbralGlaive2))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FuryofNatureAura))
                    {
                        int32 l_HealAmount = CalculatePct(GetHitDamage(), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                        l_Caster->CastCustomSpell(l_Caster, eSpells::FuryofNatureHeal, &l_HealAmount, nullptr, nullptr, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_tos_melee_dps_trinkets_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_tos_melee_dps_trinkets_SpellScript();
        }

        class spell_item_tos_melee_dps_trinkets_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_tos_melee_dps_trinkets_AuraScript);

            enum eSpell
            {
                ForgemastersVigor = 177096
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Player->IsMeleeDamageDealer()) ///< should do 50% less dmg
                    p_Amount /= 2;

                if (l_Player->GetActiveSpecializationID() == SPEC_DK_UNHOLY) ///< should do 30% less dmg
                    p_Amount = CalculatePct(p_Amount, 70);

                if (l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_FURY || l_Player->GetActiveSpecializationID() == SPEC_WARRIOR_ARMS) ///< should do 40% less dmg
                    p_Amount = CalculatePct(p_Amount, 60);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::CeaselessToxin)
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_tos_melee_dps_trinkets_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_tos_melee_dps_trinkets_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Void Tendril - 250848
class spell_item_void_tendril : public SpellScriptLoader
{
public:
    spell_item_void_tendril() : SpellScriptLoader("spell_item_void_tendril") { }

    class spell_item_void_tendril_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_void_tendril_SpellScript);

        enum eSpells
        {
            VoidTendril = 250846,
            VoidTendrilDmg = 250834
        };

        void HandleHitTarget(SpellEffIndex p_EffIndex)
        {
            PreventHitDefaultEffect(p_EffIndex);

            Unit* l_Caster = GetCaster();
            WorldLocation const* l_Position = GetExplTargetDest();
            if (!l_Caster || !l_Position)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetSpell()->m_castItemGUID);
            if (!l_Trinket)
                return;

            float l_Multiplier = 0.0f;
            int32 l_TrinketItemLevel = l_Trinket->GetItemLevel(l_Player);
            RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
            if (l_RandomPropertiesPoints)
                l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

            SpellInfo const* l_VoidTendrilInfo = sSpellMgr->GetSpellInfo(eSpells::VoidTendrilDmg);
            if (!l_VoidTendrilInfo)
                return;

            float l_ScalingMultiplier = l_VoidTendrilInfo->Effects[EFFECT_0].ScalingMultiplier;
            float l_Amount = l_ScalingMultiplier * l_Multiplier;
            float l_DeltaScalingMultiplier = l_VoidTendrilInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
            if (l_DeltaScalingMultiplier)
            {
                float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                l_Amount += frand(-l_Delta, l_Delta);
            }

            int32 l_Damage = (int32)l_Amount;
            l_Caster->CastCustomSpell(l_Position->GetPositionX(), l_Position->GetPositionY(), l_Position->GetPositionZ(), eSpells::VoidTendrilDmg, &l_Damage, nullptr, nullptr, true, l_Trinket);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_item_void_tendril_SpellScript::HandleHitTarget, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_void_tendril_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Shadow Blades - 253263
class spell_item_shadow_blades : public SpellScriptLoader
{
public:
    spell_item_shadow_blades() : SpellScriptLoader("spell_item_shadow_blades") { }

    class spell_item_shadow_blades_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_shadow_blades_AuraScript);

        void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcEventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_ProcEventInfo.GetProcTarget();
            if (!l_Caster || !l_Target)
                return;

            l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ShadowBlades) = l_Target->GetGUID();
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_item_shadow_blades_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_shadow_blades_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Shadow Blades - 255857
class spell_item_shadow_blades_trigger : public SpellScriptLoader
{
public:
    spell_item_shadow_blades_trigger() : SpellScriptLoader("spell_item_shadow_blades_trigger") { }

    class spell_item_shadow_blades_trigger_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_shadow_blades_trigger_SpellScript);

        enum eSpells
        {
            ShadowBlades = 253263,
            ShadowBladesPeriodic = 255818
        };

        void HandleHitTarget(SpellEffIndex p_EffIndex)
        {
            PreventHitDefaultEffect(p_EffIndex);

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowBlades);
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_SpellInfo)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetSpell()->m_castItemGUID);
            if (!l_Trinket)
                return;

            for (int32 i = 0; i < l_SpellInfo->Effects[EFFECT_2].BasePoints; i++)
                l_Caster->CastSpell(l_Caster, eSpells::ShadowBladesPeriodic, true, l_Trinket);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_item_shadow_blades_trigger_SpellScript::HandleHitTarget, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_TRIGGER_SPELL);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_shadow_blades_trigger_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Shadow Blades - 255818
class spell_item_shadow_blades_periodic : public SpellScriptLoader
{
public:
    spell_item_shadow_blades_periodic() : SpellScriptLoader("spell_item_shadow_blades_periodic") { }

    class spell_item_shadow_blades_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_shadow_blades_periodic_AuraScript);

        enum eSpells
        {
            ShadowBlades = 253263,
            ShadowBladesPeriodic = 255818,
            ShadowBladesBlock = 255856
        };

        std::vector<uint32> m_SpellList = { 255846, 255880, 255881, 255882, 255883, 255884 };

        void OnTick(AuraEffect const* p_AurEff)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ShadowBlades));
            if (!l_Target)
                return;

            if (l_Caster->HasAura(eSpells::ShadowBladesBlock))
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetAura()->GetCastItemGUID());
            if (!l_Trinket)
                return;

            l_Caster->CastSpell(l_Target, m_SpellList[GetAura()->GetStackAmount() - 1], true, l_Trinket);
            GetAura()->DropStack();
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_shadow_blades_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_shadow_blades_periodic_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Shadow Blades - 257702
class spell_item_shadow_blades_damage : public SpellScriptLoader
{
public:
    spell_item_shadow_blades_damage() : SpellScriptLoader("spell_item_shadow_blades_damage") { }

    class spell_item_shadow_blades_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_shadow_blades_damage_SpellScript);

        enum eSpells
        {
            ShadowBladesBuff = 255870
        };

        void HandleDamage(SpellEffIndex)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            int32 l_Damage = GetHitDamage();
            if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::ShadowBladesBuff, EFFECT_0, l_Caster->GetGUID()))
                AddPct(l_Damage, l_AuraEffect->GetAmount() / 10);
            SetHitDamage(l_Damage);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_item_shadow_blades_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_shadow_blades_damage_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Prototype Personnel Decimator - 255629
class spell_item_prototype_personnel_decimator : public SpellScriptLoader
{
public:
    spell_item_prototype_personnel_decimator() : SpellScriptLoader("spell_item_prototype_personnel_decimator") { }

    class spell_item_prototype_personnel_decimator_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_prototype_personnel_decimator_SpellScript);

        void HandleDamage(SpellEffIndex)
        {
            WorldLocation const* l_Position = GetExplTargetDest();
            Unit* l_Target = GetHitUnit();
            if (!l_Position || !l_Target)
                return;

            int32 l_Damage = GetHitDamage();
            float l_Distance = l_Target->GetDistance(l_Position->GetPositionX(), l_Position->GetPositionY(), l_Position->GetPositionZ());

            if (l_Distance > 20.f)
                l_Distance = 20.f;
            if (l_Distance > 1.0f)
                SetHitDamage(int32(l_Damage * ((20.f - l_Distance) / 20.f)));
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_item_prototype_personnel_decimator_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_prototype_personnel_decimator_SpellScript();
    }
};

/// Last Update: 7.3.5 build 26365
/// Called by Netherlord's Accursed Wrathsteed - 238454
class spell_item_netherlords_accursed_wrathsteed : public SpellScriptLoader
{
    public:
        spell_item_netherlords_accursed_wrathsteed() : SpellScriptLoader("spell_item_netherlords_accursed_wrathsteed") { }

        class spell_item_netherlords_accursed_wrathsteed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_netherlords_accursed_wrathsteed_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || l_Player->getClass() != CLASS_WARLOCK)
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_netherlords_accursed_wrathsteed_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_netherlords_accursed_wrathsteed_SpellScript();
        }
};

/// Cloak of the Antoran - 251104
/// Chest of the Antoran - 251105
/// Shoulders of the Antoran - 251107
/// Helm of the Antoran - 251108
/// Leggings of the Antoran - 251109
/// Gloves of the Antoran - 251110
class spell_create_antorus_set_item : public SpellScriptLoader
{
    public:
        spell_create_antorus_set_item() : SpellScriptLoader("spell_create_antorus_set_item") { }

        class spell_create_antorus_set_item_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_create_antorus_set_item_SpellScript);

            enum eBonuses
            {
                Warforged       = 560,
                Avoidance       = 40,
                Indestructible  = 43,
                Leech           = 41,
                Speed           = 42,
                PrismaticSocket = 563
            };

            enum eTokenBonuses
            {
                BonusLFR    = 3613,
                BonusHeroic = 570,
                BonusMythic = 569
            };

            uint32 m_ItemID;

            bool Load() override
            {
                m_ItemID = 0;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                LootStore& l_LootStore = LootTemplates_Spell;
                LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(GetSpellInfo()->Id);
                if (l_LootTemplate == nullptr)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::list<ItemTemplate const*> l_LootTable;
                std::vector<uint32> l_Items;
                l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, false);

                if (l_LootTable.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                for (ItemTemplate const* l_Template : l_LootTable)
                {
                    if (l_Template->HasSpec((SpecIndex)l_SpecID, l_Player->getLevel()))
                        l_Items.push_back(l_Template->ItemId);
                }

                if (l_Items.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::random_shuffle(l_Items.begin(), l_Items.end());

                m_ItemID = l_Items[0];

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return;

                Item* l_Item = GetCastItem();
                if (!l_Item || !m_ItemID)
                    return;

                ItemContext l_Context = ItemContext::RaidNormal;

                if (l_Item->HasItemBonus(eTokenBonuses::BonusMythic))
                    l_Context = ItemContext::RaidMythic;
                else if (l_Item->HasItemBonus(eTokenBonuses::BonusHeroic))
                    l_Context = ItemContext::RaidHeroic;
                else if (l_Item->HasItemBonus(eTokenBonuses::BonusLFR))
                    l_Context = ItemContext::RaidLfr;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                    return;

                std::vector<uint32> l_BonusList;
                Item::GenerateItemBonus(m_ItemID, l_Context, l_BonusList, false, l_Player);

                if (Item* l_Item = l_Player->StoreNewItem(l_Destination, m_ItemID, true, Item::GenerateItemRandomPropertyId(m_ItemID), l_BonusList))
                {
                    l_Player->SendNewItem(l_Item, 1, false, true, false, 0, l_Context);
                    l_Player->SendDisplayToast(m_ItemID, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_BonusList);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_create_antorus_set_item_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_create_antorus_set_item_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_create_antorus_set_item_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Summon Dread Reflection - 246461
/// Called for item Specter of Betrayal - 151190
class spell_item_summon_dread_reflection : public SpellScriptLoader
{
    public:
        spell_item_summon_dread_reflection() : SpellScriptLoader("spell_item_summon_dread_reflection") { }

        class spell_item_summon_dread_reflection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_summon_dread_reflection_SpellScript);

            enum eSpells
            {
                CommandDreadReflections = 246463
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Item* l_Item = GetCastItem();
                if (l_Item == nullptr)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::CommandDreadReflections, true,  l_Item);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_summon_dread_reflection_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_summon_dread_reflection_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Command Dread Reflections - 246463
/// Called for item Specter of Betrayal - 151190
class spell_item_command_dread_reflections : public SpellScriptLoader
{
    public:
        spell_item_command_dread_reflections() : SpellScriptLoader("spell_item_command_dread_reflections") { }

        class spell_item_command_dread_reflections_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_command_dread_reflections_AuraScript);

            enum eSpells
            {
                SummonDreadReflection   = 246461,
                DreadReflections        = 246466,
                DreadTorrent            = 246464
            };

            void HandlePeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                AuraEffect const* l_DamageEffect = l_Caster->GetAuraEffect(eSpells::DreadReflections, SpellEffIndex::EFFECT_0);
                if (l_DamageEffect == nullptr)
                    return;

                int32 l_Damage = l_DamageEffect->GetAmount();

                std::list<AreaTrigger*> l_AreaTriggers;
                l_Caster->GetAreaTriggerList(l_AreaTriggers, eSpells::SummonDreadReflection);

                for (AreaTrigger* l_AreaTrigger : l_AreaTriggers)
                {
                    l_Caster->CastCustomSpell(l_AreaTrigger->GetPositionX(), l_AreaTrigger->GetPositionY(), l_AreaTrigger->GetPositionZ(), eSpells::DreadTorrent, &l_Damage, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_command_dread_reflections_AuraScript::HandlePeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_command_dread_reflections_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Khadgars Wand 215910 - Suramar quest item - by Hearts
class spell_khadgars_wand_215910 : public SpellScriptLoader
{
    public:
        spell_khadgars_wand_215910() : SpellScriptLoader("spell_khadgars_wand_215910") { }

        class spell_khadgars_wand_215910_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_khadgars_wand_215910_SpellScript);

            enum eSpells
            {
                SummonWandSpark1 = 215948,
                SummonWandSpark2 = 215953,
                SummonWandSpark3 = 215954
            };

            enum eQuests
            {
                TrailOfEchoes = 39987
            };

            enum eObjectiveIds
            {
                FaintArchaneEcho = 280333,
                BlinkingArchaneEcho = 280296,
                PulsingArchaneEcho = 280297
            };

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasQuest(eQuests::TrailOfEchoes) && l_Player->GetQuestObjectiveCounter(eObjectiveIds::FaintArchaneEcho) == 0)
                    l_Player->CastSpell(l_Player, eSpells::SummonWandSpark1, true);

                if (l_Player->HasQuest(eQuests::TrailOfEchoes) && l_Player->GetQuestObjectiveCounter(eObjectiveIds::FaintArchaneEcho) == 1 && l_Player->GetQuestObjectiveCounter(eObjectiveIds::BlinkingArchaneEcho) == 0)
                    l_Player->CastSpell(l_Player, eSpells::SummonWandSpark2, true);

                if (l_Player->HasQuest(eQuests::TrailOfEchoes) && l_Player->GetQuestObjectiveCounter(eObjectiveIds::FaintArchaneEcho) == 1 && l_Player->GetQuestObjectiveCounter(eObjectiveIds::BlinkingArchaneEcho) == 1 && l_Player->GetQuestObjectiveCounter(eObjectiveIds::PulsingArchaneEcho) == 0)
                    l_Player->CastSpell(l_Player, eSpells::SummonWandSpark3, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_khadgars_wand_215910_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_khadgars_wand_215910_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Demonic Vigor - 242611
/// Called for item Engine of Eradication - 147015
class spell_item_demonic_vigor : public SpellScriptLoader
{
    public:
        spell_item_demonic_vigor() : SpellScriptLoader("spell_item_demonic_vigor") { }

        class spell_item_demonic_vigor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_demonic_vigor_AuraScript);

            enum Spells
            {
                FragmentOfVigor1 = 242614,
                FragmentOfVigor2 = 243627,
                FragmentOfVigor3 = 243628,
                FragmentOfVigor4 = 243629,
                FragmentOfVigor5 = 244584,
                FragmentOfVigor6 = 244585,
                FragmentOfVigor7 = 244586,
                FragmentOfVigor8 = 244587
            };

            void HandleProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& /*p_EventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<uint32> l_Spells =
                {
                    FragmentOfVigor1, FragmentOfVigor2, FragmentOfVigor3, FragmentOfVigor4,
                    FragmentOfVigor5, FragmentOfVigor6, FragmentOfVigor7, FragmentOfVigor8
                };

                constexpr uint8 l_NumberOfOrbs = 4;

                JadeCore::Containers::RandomResizeList(l_Spells, l_NumberOfOrbs);

                for (uint32 l_Spell : l_Spells)
                    l_Caster->CastSpell(l_Caster, l_Spell, true);
            }

            void Register()
            {
                AfterEffectProc += AuraEffectProcFn(spell_item_demonic_vigor_AuraScript::HandleProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_demonic_vigor_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Fragment of Vigor - 243630
/// Called for item Engine of Eradication - 147015
class spell_item_fragment_of_vigor : public SpellScriptLoader
{
    public:
        spell_item_fragment_of_vigor() : SpellScriptLoader("spell_item_fragment_of_vigor") { }

        class spell_item_fragment_of_vigor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_fragment_of_vigor_SpellScript);

            enum eSpells
            {
                DemonicVigor = 242612
            };

            void HandleHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || m_IsUsed)
                    return;

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::DemonicVigor))
                {
                    constexpr uint32 l_Addition = 3 * TimeConstants::IN_MILLISECONDS;
                    int32 l_MaxDurationDiff = l_Aura->GetMaxDuration() - l_Aura->GetDuration();
                    if (l_MaxDurationDiff < l_Addition)
                        l_Aura->SetMaxDuration(l_Aura->GetMaxDuration() + (l_Addition - l_MaxDurationDiff));

                    l_Aura->SetDuration(l_Aura->GetDuration() + l_Addition);

                    m_IsUsed = true;
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_item_fragment_of_vigor_SpellScript::HandleHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }

        private:
            bool m_IsUsed = false;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_fragment_of_vigor_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Stance of the Mountain - 214423
class spell_item_stance_of_the_mountain : public SpellScriptLoader
{
    public:
        spell_item_stance_of_the_mountain() : SpellScriptLoader("spell_item_stance_of_the_mountain") {}

        class spell_item_stance_of_the_mountain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_stance_of_the_mountain_AuraScript);

            void HandleOnEffectAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                int32 l_Absorb = p_AurEff->GetAmount();
                int32 l_Damage = p_DmgInfo.GetAmount();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Absorb || !l_Damage || !l_SpellInfo)
                    return;

                p_AbsorbAmount = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_1].BasePoints);
                p_AbsorbAmount = std::min<uint32>(p_AbsorbAmount, l_Absorb);
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_stance_of_the_mountain_AuraScript::HandleOnEffectAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_stance_of_the_mountain_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Soul Sap - 215938
class spell_item_soul_sap : public SpellScriptLoader
{
public:
    spell_item_soul_sap() : SpellScriptLoader("spell_item_soul_sap") { }

    class spell_item_soul_sap_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_soul_sap_AuraScript);

        enum eSpells
        {
            Spell_SoulSapEnemyAura = 215936,
            Spell_SoulSapHeal = 215940
        };

        bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_ProcEventInfo.GetActionTarget();
            if (!l_Caster || !l_Target)
                return false;

            if (l_Target->HasAura(eSpells::Spell_SoulSapEnemyAura, l_Caster->GetGUID()))
            {
                return true;
            }

            return false;
        }

        void HandleEffectProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            DamageInfo const* l_DamageInfo = p_EventInfo.GetDamageInfo();
            int32 l_AvailableHeal = p_AurEff->GetAmount();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_DamageInfo || !l_DamageInfo->GetAmount() ||
                !l_AvailableHeal || !l_SpellInfo)
                return;

            int32 l_ShouldBeHealed = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);

            if (l_AvailableHeal - l_ShouldBeHealed >= 0)
            {
                l_Caster->CastCustomSpell(l_Caster, eSpells::Spell_SoulSapHeal, &l_ShouldBeHealed, nullptr, nullptr, true);
                const_cast<AuraEffect*>(p_AurEff)->SetAmount(l_AvailableHeal - l_ShouldBeHealed);
            }
            else
            {
                l_Caster->CastCustomSpell(l_Caster, eSpells::Spell_SoulSapHeal, &l_AvailableHeal, nullptr, nullptr, true);
                const_cast<AuraEffect*>(p_AurEff)->SetAmount(0);
            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_soul_sap_AuraScript::HandleCheckProc);
            OnEffectProc += AuraEffectProcFn(spell_item_soul_sap_AuraScript::HandleEffectProc, EFFECT_1, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_soul_sap_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Spawn of Serpentrix - 215745
class spell_item_spawn_of_serpentrix : public SpellScriptLoader
{
public:
    spell_item_spawn_of_serpentrix() : SpellScriptLoader("spell_item_spawn_of_serpentrix") { }

    class spell_item_spawn_of_serpentrix_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_spawn_of_serpentrix_AuraScript);

        enum eSpells
        {
            Spell_SpawnOfSerpentrixSummon = 215750
        };

        void HandleEffectProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::Spell_SpawnOfSerpentrixSummon, true);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_item_spawn_of_serpentrix_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_spawn_of_serpentrix_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Legion's Gaze - 230150
class spell_item_legions_gaze : public SpellScriptLoader
{
public:
    spell_item_legions_gaze() : SpellScriptLoader("spell_item_legions_gaze") { }

    class spell_item_legions_gaze_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_legions_gaze_AuraScript);

        void HandleEffectProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_EventInfo.GetActionTarget();
            if (!l_Caster || !l_Target)
                return;

            if (l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::LegionGazeLastDamagedGUID) == 0)
            {
                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::LegionGazeLastDamagedGUID) = l_Target->GetGUID();
            }
            else if (l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::LegionGazeLastDamagedGUID) != 0 &&
                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::LegionGazeLastDamagedGUID) != l_Target->GetGUID())
            {
                l_Caster->RemoveAura(p_AurEff->GetTriggerSpell());
                l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::LegionGazeLastDamagedGUID) = l_Target->GetGUID();
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_item_legions_gaze_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_legions_gaze_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Flame Gale - 230215
class spell_item_flame_gale : public SpellScriptLoader
{
public:
    spell_item_flame_gale() : SpellScriptLoader("spell_item_flame_gale") { }

    class spell_item_flame_gale_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_flame_gale_SpellScript);

        enum eSpells
        {
            Spell_FlameGaleAreatrigger = 230213
        };

        void ChangeDamage(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlameGaleBonusDamage))
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Spell_FlameGaleAreatrigger);
                AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Spell_FlameGaleAreatrigger, EFFECT_1);
                if (!l_SpellInfo || !l_AuraEffect)
                    return;

                int32 l_Damage = GetHitDamage();
                AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_2].BasePoints);

                SetHitDamage(l_Damage);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_item_flame_gale_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_flame_gale_SpellScript();
    }
};

/// Last update 7.3.5 Build 26365
/// Item - Khaz'goroth's Courage - 154176
/// Called By Mark of Khaz'goroth - 256825
class spell_item_mark_of_khazgoroth : public SpellScriptLoader
{
    public:
        spell_item_mark_of_khazgoroth() : SpellScriptLoader("spell_item_mark_of_khazgoroth") {}

        class spell_item_mark_of_khazgoroth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_mark_of_khazgoroth_AuraScript);

            void HandleCalcProcPPM(float & p_ProcsPerMinute, ProcEventInfo & p_Proc)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player->GetRoleForGroup() == Roles::ROLE_TANK) ///< https://www.wowhead.com/item=154176/khazgoroths-courage&bonus=1522:3983#comments
                    p_ProcsPerMinute /= 2;
            }

            void Register() override
            {
                DoCalcPPM += AuraProcPerMinuteFn(spell_item_mark_of_khazgoroth_AuraScript::HandleCalcProcPPM);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_mark_of_khazgoroth_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Worldforger's Flame 257244 - From trinket Khaz'goroth's Courage item ID 154176
class spell_item_worldforgers_flame : public SpellScriptLoader
{
    public:
        spell_item_worldforgers_flame() : SpellScriptLoader("spell_item_worldforgers_flame") { }

        class spell_item_worldforgers_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_worldforgers_flame_SpellScript);

            void ChangeDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                int32 l_Damage = GetHitDamage() * 3.0f; ///< Don't know if it's the correct value, but it seems that for every item lvl, the damage is far too low. It's disgusting, but it's quick.
                Unit* l_Caster = GetCaster();
                if (!l_Damage || !l_Caster || !l_Caster->IsPlayer())
                    return;

                int32 l_Factor = 0;
                Player* l_Player = l_Caster->ToPlayer();
                switch (l_Player->GetActiveSpecializationID())
                {
                    case SpecIndex::SPEC_DK_FROST: ///< DK Frost + tanks have -50% damage. Everything is here: https://www.wowhead.com/item=154176/khazgoroths-courage&bonus=1522:3983#comments (last comment at time of writing)
                    case SpecIndex::SPEC_PALADIN_PROTECTION:
                    case SpecIndex::SPEC_WARRIOR_PROTECTION:
                    case SpecIndex::SPEC_DRUID_GUARDIAN:
                    case SpecIndex::SPEC_DK_BLOOD:
                    case SpecIndex::SPEC_MONK_BREWMASTER:
                    case SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE:
                        l_Factor = -50;
                        break;
                    case SpecIndex::SPEC_WARRIOR_FURY:
                        l_Factor = -70;
                        break;
                    case SpecIndex::SPEC_WARRIOR_ARMS:
                        l_Factor = -34;
                        break;
                    case SpecIndex::SPEC_PALADIN_RETRIBUTION:
                        l_Factor = 5;
                        break;
                    default:
                        break;
                }

                AddPct(l_Damage, l_Factor);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_worldforgers_flame_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_worldforgers_flame_SpellScript();
        }
};

/// Last update 7.3.5 Build 26365
/// Item - Garothi Feedback Conduit - 151956
/// Called By Feedback Loop - 253268
class spell_item_feedback_loop : public SpellScriptLoader
{
public:
    spell_item_feedback_loop() : SpellScriptLoader("spell_item_feedback_loop") {}

    enum eSpells
    {
        FeedbackLoopBuff   = 253269,
    };

    class spell_item_feedback_loop_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_feedback_loop_AuraScript);

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_EventInfo.GetActionTarget();
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_Target || !l_DamageInfo)
                return false;

            return true;
        }

        void HandleCalcProcPPM(float & p_ProcsPerMinute, ProcEventInfo & p_Proc)
        {
            if (GetUnitOwner() == nullptr ||
                p_Proc.GetActionTarget() == nullptr)
                return;

            Unit* l_Target = p_Proc.GetActionTarget();

            if (!GetUnitOwner()->IsPlayer())
                return;

            ///< From trinket description: This is more likely to occur when you heal allies who are at low health.
            p_ProcsPerMinute += (100.0f - l_Target->GetHealthPct()) / 10.0f;
        }

        void HandleOnProc(ProcEventInfo& p_Proc)
        {
            if (!GetUnitOwner() || !p_Proc.GetActionTarget())
                return;

            if (!GetUnitOwner()->ToPlayer())
                return;

            Item* l_Trinket = GetUnitOwner()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
            if (!l_Trinket)
                return;

            GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::FeedbackLoopBuff, true, l_Trinket);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_feedback_loop_AuraScript::HandleOnCheckProc);
            DoCalcPPM += AuraProcPerMinuteFn(spell_item_feedback_loop_AuraScript::HandleCalcProcPPM);
            OnProc += AuraProcFn(spell_item_feedback_loop_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_feedback_loop_AuraScript();
    }
};

/// Last update 7.3.5 Build 26365
/// Item - Prototype Personnel Decimator - 151962
/// Called By Prototype Personnel Decimator - 253242
class spell_item_prototype_personnel_decimator_proc : public SpellScriptLoader
{
public:
    spell_item_prototype_personnel_decimator_proc() : SpellScriptLoader("spell_item_prototype_personnel_decimator_proc") {}

    class spell_item_prototype_personnel_decimator_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_prototype_personnel_decimator_proc_AuraScript);

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = p_EventInfo.GetActionTarget();
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_Caster || !l_Target || !l_DamageInfo)
                return false;

            if (l_Caster->GetDistance(l_Target) < GetSpellInfo()->Effects[EFFECT_0].BasePoints)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_prototype_personnel_decimator_proc_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_prototype_personnel_decimator_proc_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called By Swap Hounds - 255770, 255771
class spell_item_swap_hounds : public SpellScriptLoader
{
    public:
        spell_item_swap_hounds() : SpellScriptLoader("spell_item_swap_hounds") { }

        class spell_item_swap_hounds_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_swap_hounds_SpellScript);

            enum eSpells
            {
                SwapHoundsEyeOfShatug   = 255770,
                SwapHoundsEyeOfFharg    = 255771
            };

            enum eItemEntries
            {
                EyeOfShatug             = 152645,
                EyeOfFharg              = 153544
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    Item* l_Trinket = l_Player->GetItemByGuid(GetSpell()->m_castItemGUID);
                    if (!l_Trinket)
                        return;

                    uint32 l_ItemEntry = eItemEntries::EyeOfShatug;

                    if (GetSpellInfo()->Id == eSpells::SwapHoundsEyeOfShatug)
                        l_ItemEntry = eItemEntries::EyeOfFharg;

                    l_Trinket->ChangeItemTo(l_ItemEntry);
                    l_Player->ApplyEquipCooldown(l_Trinket);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_item_swap_hounds_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_swap_hounds_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Called by Flames of F'harg - 253305
class spell_item_shadown_singed_fang_proc : public SpellScriptLoader
{
public:
    spell_item_shadown_singed_fang_proc() : SpellScriptLoader("spell_item_shadown_singed_fang_proc") { }

    class spell_item_shadown_singed_fang_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_shadown_singed_fang_proc_AuraScript);

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            if (!l_DamageInfo->GetSpellInfo())
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_shadown_singed_fang_proc_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_shadown_singed_fang_proc_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Felshield - 253277
class spell_item_felshield : public SpellScriptLoader
{
public:
    spell_item_felshield() : SpellScriptLoader("spell_item_felshield") { }

    class  spell_item_felshield_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_felshield_AuraScript);

        enum eSpells
        {
            Felshield = 253278,
            DisciplinePriest = 137032
        };

        void HandleAfterApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
        {
            m_StartAmount = p_AuraEffect->GetAmount();
        }

        void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
        {
            AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
            if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE &&
                l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                return;

            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetTarget();
            if (!l_Caster || !l_Target || !l_Aura)
                return;

            float l_Pct = 0;
            if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_1))
                l_Pct = (float)l_AuraEffect->GetAmount();

            if (l_Caster->HasAura(eSpells::DisciplinePriest))
                l_Pct *= 0.45f;

            int32 l_Damage = m_StartAmount - p_AuraEffect->GetAmount();
            l_Damage = CalculatePct(l_Damage, l_Pct);

            l_Target->CastCustomSpell(l_Target, eSpells::Felshield, &l_Damage, NULL, NULL, true, 0, nullptr, l_Caster->GetGUID());
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_item_felshield_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_felshield_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        }

    private:
        uint32 m_StartAmount = 0;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_felshield_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Unstable Portals - 251925
class spell_item_unstable_portals : public SpellScriptLoader
{
    public:
        spell_item_unstable_portals() : SpellScriptLoader("spell_item_unstable_portals") { }

        class spell_item_unstable_portals_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_unstable_portals_AuraScript);

            enum eSpells
            {
                WindsOfKareth       = 251938,
                LightOfAbsolarn     = 252545,
                FlamesOfRuvaraad    = 256415
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetUnitOwner();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                Item* l_Trinket = l_Caster->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
                if (!l_Trinket)
                    return;

                uint32 l_TriggerId = 0;
                uint32 l_Rand = urand(1, 3);
                switch (l_Rand)
                {
                    case 1:
                        l_TriggerId = eSpells::WindsOfKareth;
                        break;
                    case 2:
                        l_TriggerId = eSpells::LightOfAbsolarn;
                        break;
                    case 3:
                        l_TriggerId = eSpells::FlamesOfRuvaraad;
                        break;
                    default:
                        break;
                }

                l_Caster->CastSpell(l_Caster, l_TriggerId, true, l_Trinket);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_unstable_portals_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_unstable_portals_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Tarratus Keystone - 253282
class spell_item_tarratus_keystone : public SpellScriptLoader
{
public:
    spell_item_tarratus_keystone() : SpellScriptLoader("spell_item_tarratus_keystone") { }

    class spell_item_tarratus_keystone_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_tarratus_keystone_SpellScript);

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            m_CountTargets = p_Targets.size();
        }

        void ChangeHeal(SpellEffIndex)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetSpell()->m_castItemGUID);
            if (!l_Trinket)
                return;

            float l_Multiplier = 0.0f;
            int32 l_TrinketItemLevel = l_Trinket->GetItemLevel(l_Player);
            RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
            if (l_RandomPropertiesPoints)
                l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

            float l_ScalingMultiplier = GetSpellInfo()->Effects[EFFECT_0].ScalingMultiplier;
            float l_Amount = l_ScalingMultiplier * l_Multiplier;
            float l_DeltaScalingMultiplier = GetSpellInfo()->Effects[EFFECT_0].DeltaScalingMultiplier;
            if (l_DeltaScalingMultiplier)
            {
                float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                l_Amount += frand(-l_Delta, l_Delta);
            }

            l_Amount /= m_CountTargets;
            SetHitHeal(l_Amount);
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_tarratus_keystone_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            OnEffectHitTarget += SpellEffectFn(spell_item_tarratus_keystone_SpellScript::ChangeHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }

    private:
        uint32 m_CountTargets = 0;
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_tarratus_keystone_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Eye of the Hounds - 255768, 255769
class spell_item_eye_of_the_hounds : public SpellScriptLoader
{
    public:
        spell_item_eye_of_the_hounds() : SpellScriptLoader("spell_item_eye_of_the_hounds") { }

        class spell_item_eye_of_the_hounds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_eye_of_the_hounds_AuraScript);

            enum eSpells
            {
                EyeOfTheHoundsReqSpell  = 251963,

                EyeOfTheHoundsFharg     = 255768,
                EyeOfTheHoundsShatug    = 255769,
                EyeOfFhargAura          = 256719,
                EyeOfShatugAura         = 256718,
                EyeOfFhargArmorAura     = 251968,
                EyeOfShatugVersaAura    = 251967
            };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    Item* l_Trinket = l_Caster->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
                    if (!l_Trinket)
                        return;

                    if (l_Caster->ToPlayer() && l_Caster->ToPlayer()->IsActiveSpecTankSpec())
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::EyeOfTheHoundsReqSpell, true, l_Trinket);

                        if (GetSpellInfo()->Id == eSpells::EyeOfTheHoundsFharg)
                        {
                            l_Caster->CastSpell(l_Caster, eSpells::EyeOfFhargAura, true, l_Trinket);
                            l_Caster->CastSpell(l_Caster, eSpells::EyeOfFhargArmorAura, true, l_Trinket);
                        }

                        if (GetSpellInfo()->Id == eSpells::EyeOfTheHoundsShatug)
                        {
                            l_Caster->CastSpell(l_Caster, eSpells::EyeOfShatugAura, true, l_Trinket);
                            l_Caster->CastSpell(l_Caster, eSpells::EyeOfShatugVersaAura, true, l_Trinket);
                        }
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->RemoveAura(eSpells::EyeOfTheHoundsReqSpell);
                    l_Caster->RemoveAura(eSpells::EyeOfFhargAura);
                    l_Caster->RemoveAura(eSpells::EyeOfShatugAura);
                    l_Caster->RemoveAura(eSpells::EyeOfFhargArmorAura);
                    l_Caster->RemoveAura(eSpells::EyeOfShatugVersaAura);
                }
            }

            uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;

            void OnUpdate(uint32 p_Diff)
            {
                if (m_UpdateTimer > p_Diff)
                {
                    m_UpdateTimer -= p_Diff;
                    return;
                }
                m_UpdateTimer = 1 * IN_MILLISECONDS;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                    return;

                if (Unit* l_Target = GetUnitOwner())
                {
                    if (!l_Target->ToPlayer())
                        return;

                    Item* l_Trinket = l_Target->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
                    if (!l_Trinket)
                        return;

                    float l_Radius = (float)l_SpellInfo->Effects[EFFECT_0].BasePoints;

                    std::list<Unit*> l_FriendlyList;
                    JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(l_Target, l_Target, l_Radius);
                    JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(l_Target, l_FriendlyList, l_Check);
                    l_Target->VisitNearbyObject(l_Radius, l_Searcher);

                    uint32 l_AuraId = GetSpellInfo()->Id != eSpells::EyeOfTheHoundsFharg ? eSpells::EyeOfFhargAura : eSpells::EyeOfShatugAura;
                    int32 l_AuraAmount = 0;
                    for (auto l_Unit : l_FriendlyList)
                    {
                        if (Aura* l_Aura = l_Unit->GetAura(l_AuraId))
                            if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                                if (l_Unit->GetGUID() != l_Target->GetGUID())
                                    l_AuraAmount = l_Effect->GetAmount();
                    }

                    uint32 l_NeedAuraId = GetSpellInfo()->Id == eSpells::EyeOfTheHoundsFharg ? eSpells::EyeOfFhargArmorAura : eSpells::EyeOfShatugVersaAura;

                    l_Target->RemoveAura(l_NeedAuraId);
                    if (l_AuraAmount > 0)
                        l_Target->CastCustomSpell(l_Target, l_NeedAuraId, &l_AuraAmount, 0, 0, true, l_Trinket);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_eye_of_the_hounds_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_eye_of_the_hounds_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnAuraUpdate += AuraUpdateFn(spell_item_eye_of_the_hounds_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_eye_of_the_hounds_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Reverberating Vitality - 253258
class spell_item_reverberating_vitality : public SpellScriptLoader
{
public:
    spell_item_reverberating_vitality() : SpellScriptLoader("spell_item_reverberating_vitality") { }

    class spell_item_reverberating_vitality_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_reverberating_vitality_SpellScript);

        void HandleOnCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();
            if (!l_Caster || !l_Target)
                return;

            l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ReverberatingVitality) = l_Target->GetGUID();
        }

        void Register()
        {
            OnCast += SpellCastFn(spell_item_reverberating_vitality_SpellScript::HandleOnCast);
        }
    };

    class spell_item_reverberating_vitality_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_reverberating_vitality_AuraScript);

        void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::ReverberatingVitality));
            if (!l_Target)
            {
                p_Amount = 0;
                return;
            }

            p_Amount *= float(((l_Target->GetHealthPct() / 2) + 50) / 100.0f);
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_reverberating_vitality_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_STAT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_reverberating_vitality_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_reverberating_vitality_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Echo of Gorshalach - 253326
class spell_item_echo_of_gorshalach : public SpellScriptLoader
{
public:
    spell_item_echo_of_gorshalach() : SpellScriptLoader("spell_item_echo_of_gorshalach") { }

    class spell_item_echo_of_gorshalach_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_echo_of_gorshalach_AuraScript);

        enum eSpells
        {
            EchoOfGorshalach = 253327
        };

        void HandleOnProc(ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetAura()->GetCastItemGUID());
            if (!l_Trinket)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::EchoOfGorshalach, true, l_Trinket);
        }

        void Register() override
        {
            OnProc += AuraProcFn(spell_item_echo_of_gorshalach_AuraScript::HandleOnProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_echo_of_gorshalach_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Echo of Gorshalach - 253327
class spell_item_echo_of_gorshalach_check : public SpellScriptLoader
{
public:
    spell_item_echo_of_gorshalach_check() : SpellScriptLoader("spell_item_echo_of_gorshalach_check") { }

    class spell_item_echo_of_gorshalach_check_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_echo_of_gorshalach_check_AuraScript);

        enum eSpells
        {
            GorshalachsLegacy = 253329,
            GorshalachsLegacy2 = 255673
        };

        void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Aura* l_Aura = GetAura();
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Aura)
                return;

            if (l_Aura->GetStackAmount() < 15)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            uint64 l_ItemGuid = l_Aura->GetCastItemGUID();

            Item* l_Trinket = l_Player->GetItemByGuid(l_ItemGuid);
            if (!l_Trinket)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::GorshalachsLegacy, true, l_Trinket);

            uint64 l_PlayerGuid = l_Player->GetGUID();
            l_Player->m_Functions.AddFunction([l_PlayerGuid, l_ItemGuid]() -> void
            {
                Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
                if (!l_Player)
                    return;

                Item* l_Trinket = l_Player->GetItemByGuid(l_ItemGuid);
                if (!l_Trinket)
                    return;

                l_Player->CastSpell(l_Player, eSpells::GorshalachsLegacy2, true, l_Trinket);

            }, l_Player->m_Functions.CalculateTime(500));

            Remove();
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_item_echo_of_gorshalach_check_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_echo_of_gorshalach_check_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Gorshalach's Legacy damaging spells 253329 AND 255673 - From trinket Gorshalach's Legacy item ID 152093
class spell_item_gorshalachs_legacy : public SpellScriptLoader
{
    public:
        spell_item_gorshalachs_legacy() : SpellScriptLoader("spell_item_gorshalachs_legacy") { }

        class spell_item_gorshalachs_legacy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_gorshalachs_legacy_SpellScript);

            void ChangeDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                SetHitDamage(GetHitDamage() * 0.5f); ///< Don't know if it's the correct value, but it seems that for every item lvl, the damage is far too low. It's disgusting, but it's quick.
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_gorshalachs_legacy_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_gorshalachs_legacy_SpellScript();
        }
};


/// Last Update 7.3.5 Build 26365
/// Called by Bulwark of Flame - 251946
class spell_item_bulwark_of_flame : public SpellScriptLoader
{
public:
    spell_item_bulwark_of_flame() : SpellScriptLoader("spell_item_bulwark_of_flame") { }

    class spell_item_bulwark_of_flame_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_bulwark_of_flame_AuraScript);

        enum eSpells
        {
            WaveOfFlame = 251947
        };

        void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
            if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetAura()->GetCastItemGUID());
            if (!l_Trinket)
                return;

            l_Caster->CastSpell(l_Caster, eSpells::WaveOfFlame, true, l_Trinket);
            l_Player->m_SpellHelper.GetUint64List()[eSpellHelpers::SmolderingTitanguard].clear();
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_bulwark_of_flame_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_bulwark_of_flame_AuraScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called by Highfather's Timekeeping - 253285
class spell_item_highfathers_timekeeping : public SpellScriptLoader
{
public:
    spell_item_highfathers_timekeeping() : SpellScriptLoader("spell_item_highfathers_timekeeping") { }

    class spell_item_highfathers_timekeeping_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_highfathers_timekeeping_AuraScript);

        enum eSpells
        {
            ArroganceDebuff = 234115,
            ArroganceMarker = 234317
        };

        bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
        {
            DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
            if (!l_DamageInfo)
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_item_highfathers_timekeeping_AuraScript::HandleOnCheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_highfathers_timekeeping_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Highfather's Timekeeping - 253287
class spell_item_highfathers_timekeeping_buff : public SpellScriptLoader
{
    public:
        spell_item_highfathers_timekeeping_buff() : SpellScriptLoader("spell_item_highfathers_timekeeping_buff") { }

        class spell_item_highfathers_timekeeping_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_highfathers_timekeeping_buff_AuraScript);

            enum eSpells
            {
                HighfathersTimekeepingHeal  = 253288
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                if (Aura* l_Aura = GetAura())
                    p_Amount /= l_Aura->GetStackAmount();
            }

            bool HandleOnCheckProc(ProcEventInfo& p_EventInfo)
            {
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return false;

                if (l_Target->GetHealthPct() > GetSpellInfo()->Effects[EFFECT_0].BasePoints)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = p_EventInfo.GetActionTarget();
                if (!l_Target)
                    return;

                Item* l_Trinket = l_Caster->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
                if (!l_Trinket)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::HighfathersTimekeepingHeal, true, l_Trinket);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_highfathers_timekeeping_buff_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
                OnEffectProc += AuraEffectProcFn(spell_item_highfathers_timekeeping_buff_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                DoCheckProc += AuraCheckProcFn(spell_item_highfathers_timekeeping_buff_AuraScript::HandleOnCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_highfathers_timekeeping_buff_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Highfather's Timekeeping - 253288
class spell_item_highfathers_timekeeping_heal : public SpellScriptLoader
{
    public:
        spell_item_highfathers_timekeeping_heal() : SpellScriptLoader("spell_item_highfathers_timekeeping_heal") { }

        class spell_item_highfathers_timekeeping_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_highfathers_timekeeping_heal_SpellScript);

            enum eSpells
            {
                HighfathersTimekeepingBuff  = 253287
            };

            void HandleHeal(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                uint32 l_Heal = GetHitHeal();
                if (Aura* l_Aura = l_Target->GetAura(eSpells::HighfathersTimekeepingBuff))
                {
                    SetHitHeal(l_Heal * l_Aura->GetStackAmount());
                    l_Aura->Remove();
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_highfathers_timekeeping_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_highfathers_timekeeping_heal_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Legion Bombardment - 256135
class spell_item_legion_bombardment : public SpellScriptLoader
{
public:
    spell_item_legion_bombardment() : SpellScriptLoader("spell_item_legion_bombardment") { }

    class spell_item_legion_bombardment_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_legion_bombardment_SpellScript);

        enum eSpells
        {
            DisciplinePriest            = 137032,
            LegionBombardmentDamage     = 255712,
            LegionBombardmentGetDamage  = 256325
        };

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Caster = GetOriginalCaster();
            WorldLocation const* l_CenterOfExplosion = GetExplTargetDest();
            if (!l_Caster || !l_CenterOfExplosion)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(GetSpell()->m_castItemGUID);
            if (!l_Trinket)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LegionBombardmentGetDamage);
            if (!l_SpellInfo)
                return;

            float l_Multiplier = 0.0f;
            int32 l_TrinketItemLevel = l_Trinket->GetItemLevel(l_Player);
            RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
            if (l_RandomPropertiesPoints)
                l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

            float l_ScalingMultiplier = l_SpellInfo->Effects[EFFECT_0].ScalingMultiplier;
            float l_Amount = l_ScalingMultiplier * l_Multiplier;
            float l_DeltaScalingMultiplier = l_SpellInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
            if (l_DeltaScalingMultiplier)
            {
                float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                l_Amount += frand(-l_Delta, l_Delta);
            }

            /// Damages reduced by 50% for melee damages dealers
            if (l_Player->IsMeleeDamageDealer(true))
            {
                l_Amount *= 0.7f;

                /// Enhancement Shamans also have another multiplicative modifier of 30%
                if (l_Player->GetActiveSpecializationID() == SpecIndex::SPEC_SHAMAN_ENHANCEMENT)
                    l_Amount *= 0.7f;
            }

            /// -65% damage for disc priest
            if (l_Player->HasAura(eSpells::DisciplinePriest))
                l_Amount *= 0.35f;

            int32 l_Damage = l_Amount;

            l_Caster->CastCustomSpell(l_CenterOfExplosion->GetPosition(), eSpells::LegionBombardmentDamage, &l_Damage, NULL, NULL, true, l_Trinket);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_item_legion_bombardment_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_legion_bombardment_SpellScript();
    }
};

/// Last Update 7.3.5 build 25996
/// Item Apocalypse Drive - 151975
/// Called By Hammer Forged - 251952 / 251949
class spell_item_apocalypse_drive : public SpellScriptLoader
{
    public:
        spell_item_apocalypse_drive() : SpellScriptLoader("spell_item_apocalypse_drive") {}

        class spell_item_apocalypse_drive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_apocalypse_drive_AuraScript);

            enum eSpells
            {
                HammerForgedPassive = 251949,
                HammerForgedActive  = 251952
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsPlayer())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                l_Player->ReduceSpellCooldown(eSpells::HammerForgedActive, 1000);
            }

            void HandleOnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                if (p_DmgInfo.GetSpellInfo() != nullptr) ///< Only auto attacks
                {
                    p_AbsorbAmount = 0;
                    return;
                }
                l_Aura->SetStackAmount(l_Aura->GetStackAmount() - 1);
            }

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Aura* l_Aura = GetAura();
                if (!l_Aura)
                    return;

                l_Aura->SetStackAmount(10);
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::HammerForgedPassive)
                    OnProc += AuraProcFn(spell_item_apocalypse_drive_AuraScript::HandleOnProc);
                else
                {
                    OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_apocalypse_drive_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                    AfterEffectApply += AuraEffectApplyFn(spell_item_apocalypse_drive_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_apocalypse_drive_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Item Forgefiend's Fabricator - 151963
/// Called By Fire Mines -  253310
class spell_item_fire_mines : public SpellScriptLoader
{
    public:
        spell_item_fire_mines() : SpellScriptLoader("spell_item_fire_mines") { }

        class spell_item_fire_mines_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_fire_mines_AuraScript);

            enum eSpells
            {
                ThrowMine = 256025
            };

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                Unit* l_Owner = GetUnitOwner();
                Aura* l_Aura = GetAura();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Owner || !l_Aura || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Owner->CastSpell(l_Target, eSpells::ThrowMine, true, l_Item);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_fire_mines_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_fire_mines_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Item Forgefiend's Fabricator - 151963
/// Called By Fire Mines -  253322
class spell_item_fire_mines_detonate : public SpellScriptLoader
{
    public:
        spell_item_fire_mines_detonate() : SpellScriptLoader("spell_item_fire_mines_detonate") { }

        class spell_item_fire_mines_detonate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_fire_mines_detonate_SpellScript);

            enum eSpells
            {
                FireMines = 253320
            };

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<AreaTrigger*> l_Mines;
                l_Caster->GetAreaTriggerList(l_Mines, eSpells::FireMines);

                for (AreaTrigger* l_Areatrigger : l_Mines)
                    l_Areatrigger->Remove(0);
            }

            void Register()
            {
                BeforeCast += SpellCastFn(spell_item_fire_mines_detonate_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_fire_mines_detonate_SpellScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Item Carafe of Searing Light - 151963
/// Called By Refreshing Agony - 253284
class spell_item_refreshing_agony_periodic : public SpellScriptLoader
{
    public:
        spell_item_refreshing_agony_periodic() : SpellScriptLoader("spell_item_refreshing_agony_periodic") { }

        class spell_item_refreshing_agony_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_refreshing_agony_periodic_AuraScript);

            enum eSpells
            {
                RefreshingAgonyMana = 255981
            };

            void HandleOnTick(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Item)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::RefreshingAgonyMana, true, l_Item);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_refreshing_agony_periodic_AuraScript::HandleOnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_refreshing_agony_periodic_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Item Seeping Scourgewing - 151964
/// Called By Shadow Strike - 253323
class spell_item_shadow_strike : public SpellScriptLoader
{
    public:
        spell_item_shadow_strike() : SpellScriptLoader("spell_item_shadow_strike") { }

        class spell_item_shadow_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_shadow_strike_AuraScript);

            enum eSpells
            {
                IsolatedStrike = 255609,
                ShadowStrike = 253323
            };

            void HandleOnProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowStrike);
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_Aura || !l_SpellInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                Item* l_Item = l_Player->GetItemByGuid(l_Aura->GetCastItemGUID());
                if (!l_Player || !l_Target)
                    return;

                int32 l_Range = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                std::list<Unit*> l_NearTargets;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(const_cast<Unit*>(l_Caster), l_Caster, l_Range);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(const_cast<Unit*>(l_Caster), l_NearTargets, u_check);
                l_Caster->VisitNearbyObject(l_Range, searcher);

                if (l_NearTargets.size() <= 1)
                    l_Caster->CastSpell(l_Target, eSpells::IsolatedStrike, true, l_Item);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_shadow_strike_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_shadow_strike_AuraScript();
        }
};

/// Antorus Vantus Rune - Item 151610
/// Antorus Vantus Rune - Spell 247617
class spell_item_antorus_vantus_rune : public SpellScriptLoader
{
    public:
        spell_item_antorus_vantus_rune() : SpellScriptLoader("spell_item_antorus_vantus_rune") { }

        class spell_item_antorus_vantus_rune_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_antorus_vantus_rune_SpellScript);

            enum eBoss
            {
                GarothiTheWorldbreaker = 122450,
                FelhoundShatug         = 122135,
                FelhoundFharg          = 122477,
                HighCommandSvirax      = 122367,
                HighCommandIshkar      = 122369,
                HighCommandErodus      = 122333,
                PortalKeeperHasabel    = 122104,
                EssenceOfEonar         = 122500,
                ImonarTheSoulhunter    = 124158,
                KinGaroth              = 122578,
                Varimathras            = 122366,
                ShivarraAsara          = 122467,
                ShivarraDiima          = 122469,
                ShivarraNoura          = 122468,
                ShivarraThuraya        = 125436,
                Aggramar               = 121975,
                ArgusTheUnmaker        = 124828
            };

            enum eSpells
            {
                VantusRuneGarothi      = 250153,
                VantusRuneFelhounds    = 250156,
                VantusRuneHighCommand  = 250167,
                VantusRuneHasabel      = 250160,
                VantusRuneEonar        = 250150,
                VantusRuneImonar       = 250158,
                VantusRuneKingaroth    = 250148,
                VantusRuneVarimathras  = 250165,
                VantusRuneShivarra     = 250163,
                VantusRuneAggramar     = 250144,
                VantusRuneArgus        = 250146

            };

            SpellCastResult CheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SPELL_FAILED_DONT_REPORT;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                switch (l_Target->GetEntry())
                {
                    case eBoss::GarothiTheWorldbreaker:
                    case eBoss::FelhoundFharg:
                    case eBoss::FelhoundShatug:
                    case eBoss::HighCommandErodus:
                    case eBoss::HighCommandIshkar:
                    case eBoss::HighCommandSvirax:
                    case eBoss::PortalKeeperHasabel:
                    case eBoss::EssenceOfEonar:
                    case eBoss::ImonarTheSoulhunter:
                    case eBoss::KinGaroth:
                    case eBoss::Varimathras:
                    case eBoss::ShivarraAsara:
                    case eBoss::ShivarraDiima:
                    case eBoss::ShivarraNoura:
                    case eBoss::ShivarraThuraya:
                    case eBoss::Aggramar:
                    case eBoss::ArgusTheUnmaker:
                        return SPELL_CAST_OK;
                        break;
                    default:
                        break;
                }

                return SPELL_FAILED_NO_VALID_TARGETS;
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                switch (l_Target->GetEntry())
                {
                    case eBoss::GarothiTheWorldbreaker:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneGarothi, true);
                        break;
                    case eBoss::FelhoundFharg:
                    case eBoss::FelhoundShatug:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneFelhounds, true);
                        break;
                    case eBoss::HighCommandErodus:
                    case eBoss::HighCommandIshkar:
                    case eBoss::HighCommandSvirax:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneHighCommand, true);
                        break;
                    case eBoss::PortalKeeperHasabel:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneHasabel, true);
                        break;
                    case eBoss::EssenceOfEonar:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneEonar, true);
                        break;
                    case eBoss::ImonarTheSoulhunter:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneImonar, true);
                        break;
                    case eBoss::KinGaroth:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneKingaroth, true);
                        break;
                    case eBoss::Varimathras:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneVarimathras, true);
                        break;
                    case eBoss::ShivarraAsara:
                    case eBoss::ShivarraDiima:
                    case eBoss::ShivarraNoura:
                    case eBoss::ShivarraThuraya:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneShivarra, true);
                        break;
                    case eBoss::Aggramar:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneAggramar, true);
                        break;
                    case eBoss::ArgusTheUnmaker:
                        l_Player->CastSpell(l_Player, eSpells::VantusRuneArgus, true);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_antorus_vantus_rune_SpellScript::CheckCast);
                AfterCast += SpellCastFn(spell_item_antorus_vantus_rune_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_item_antorus_vantus_rune_SpellScript();
        }
};

////////////////////////////////////////////////////////////////////////////////////////
/// Argus' Pantheon's trinkets
////////////////////////////////////////////////////////////////////////////////////////
/// Called by Eonar's Verdant Embrace - 257470, 257471, 257472, 257473, 257474, 257475
class spell_item_eonars_verdant_embrace : public SpellScriptLoader
{
    public:
        spell_item_eonars_verdant_embrace() : SpellScriptLoader("spell_item_eonars_verdant_embrace") { }

        class spell_item_eonars_verdant_embrace_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_eonars_verdant_embrace_AuraScript);

            enum eSpells
            {
                SpellRejuvenation   = 774,
                SpellVivify         = 116670,
                SpellFlashHeal      = 2061,
                SpellPlea           = 200829,
                SpellFlashOfLight   = 19750,
                SpellHealingWave    = 77472,

                SpellVerdantEmbrace = 257444
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                DamageInfo const* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_DamageInfo)
                    return false;

                SpellInfo const* l_SpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_SpellInfo)
                    return false;

                switch (l_SpellInfo->Id)
                {
                    case eSpells::SpellRejuvenation:
                    case eSpells::SpellVivify:
                    case eSpells::SpellFlashHeal:
                    case eSpells::SpellPlea:
                    case eSpells::SpellFlashOfLight:
                    case eSpells::SpellHealingWave:
                        return true;
                    default:
                        return false;
                }
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !p_EventInfo.GetActionTarget())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Trinket = l_Player->GetItemByGuid(GetAura()->GetCastItemGUID());
                if (!l_Trinket)
                    return;

                p_AurEff->GetBase()->DropStack();

                l_Caster->CastSpell(p_EventInfo.GetActionTarget(), eSpells::SpellVerdantEmbrace, true, l_Trinket);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_item_eonars_verdant_embrace_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_item_eonars_verdant_embrace_AuraScript::OnProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_eonars_verdant_embrace_AuraScript();
        }
};

/// Called by Norgannon's Command - 256836
class spell_item_norgannons_command : public SpellScriptLoader
{
    public:
        spell_item_norgannons_command() : SpellScriptLoader("spell_item_norgannons_command") { }

        class spell_item_norgannons_command_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_norgannons_command_AuraScript);

            enum eSpells
            {
                SpellNorgannonsFireball         = 257241,
                SpellNorgannonsFrostbolt        = 257242,
                SpellNorgannonsArcaneMissile    = 257243,
                SpellNorgannonsDivineSmite      = 257532,
                SpellNorgannonsWrath            = 257533,
                SpellNorgannonsShadowBolt       = 257534
            };

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !p_EventInfo.GetActionTarget() || p_EventInfo.GetActionTarget() == p_EventInfo.GetActor())
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_Trinket = l_Player->GetItemByGuid(GetAura()->GetCastItemGUID());
                if (!l_Trinket)
                    return;

                p_AurEff->GetBase()->DropStack();

                std::array<uint32, 6> l_Spells =
                {
                    {
                        eSpells::SpellNorgannonsFireball,
                        eSpells::SpellNorgannonsFrostbolt,
                        eSpells::SpellNorgannonsArcaneMissile,
                        eSpells::SpellNorgannonsDivineSmite,
                        eSpells::SpellNorgannonsWrath,
                        eSpells::SpellNorgannonsShadowBolt
                    }
                };

                l_Caster->CastSpell(p_EventInfo.GetActionTarget(), l_Spells[urand(0, l_Spells.size() - 1)], true, l_Trinket);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_item_norgannons_command_AuraScript::OnProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_norgannons_command_AuraScript();
        }
};

/// Called by Khaz'goroth's Shaping - 256835
class spell_item_khazgoroths_shaping : public SpellScriptLoader
{
    public:
        spell_item_khazgoroths_shaping() : SpellScriptLoader("spell_item_khazgoroths_shaping") { }

        class spell_item_khazgoroths_shaping_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_khazgoroths_shaping_AuraScript);

            void CalculateCriticalChance(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_CritValue = l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_CRIT_MELEE);

                /// Set amount to 0 if crit isn't the highest stat
                if ((l_CritValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_HASTE_MELEE)) ||
                    (l_CritValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_MASTERY)) ||
                    (l_CritValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_VERSATILITY_DAMAGE_DONE)))
                    p_Amount = 0;
            }

            void CalculateHaste(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_HasteValue = l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_HASTE_MELEE);

                /// Set amount to 0 if haste isn't the highest stat
                if ((l_HasteValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_CRIT_MELEE)) ||
                    (l_HasteValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_MASTERY)) ||
                    (l_HasteValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_VERSATILITY_DAMAGE_DONE)))
                    p_Amount = 0;
            }

            void CalculateMastery(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_MasteryValue = l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_MASTERY);

                /// Set amount to 0 if mastery isn't the highest stat
                if ((l_MasteryValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_CRIT_MELEE)) ||
                    (l_MasteryValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_HASTE_MELEE)) ||
                    (l_MasteryValue < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_VERSATILITY_DAMAGE_DONE)))
                    p_Amount = 0;
            }

            void CalculateVersatility(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_Versatility = l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_VERSATILITY_DAMAGE_DONE);

                /// Set amount to 0 if versatility isn't the highest stat
                if ((l_Versatility < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_CRIT_MELEE)) ||
                    (l_Versatility < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_HASTE_MELEE)) ||
                    (l_Versatility < l_Player->GetUInt32Value(EPlayerFields::PLAYER_FIELD_COMBAT_RATINGS + CombatRating::CR_MASTERY)))
                    p_Amount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_khazgoroths_shaping_AuraScript::CalculateCriticalChance, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_RATING);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_khazgoroths_shaping_AuraScript::CalculateHaste, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_MOD_RATING);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_khazgoroths_shaping_AuraScript::CalculateMastery, SpellEffIndex::EFFECT_2, AuraType::SPELL_AURA_MOD_RATING);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_khazgoroths_shaping_AuraScript::CalculateVersatility, SpellEffIndex::EFFECT_3, AuraType::SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_khazgoroths_shaping_AuraScript();
        }
};

/// Called by Aman'Thul's Grandeur - 256832
class spell_item_amanthuls_grandeur : public SpellScriptLoader
{
    public:
        spell_item_amanthuls_grandeur() : SpellScriptLoader("spell_item_amanthuls_grandeur") { }

        class spell_item_amanthuls_grandeur_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_amanthuls_grandeur_AuraScript);

            void CalculateStrength(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_Strength = l_Player->GetStat(Stats::STAT_STRENGTH);

                /// Set amount to 0 if strength isn't the highest stat
                if ((l_Strength < l_Player->GetStat(Stats::STAT_AGILITY)) ||
                    (l_Strength < l_Player->GetStat(Stats::STAT_INTELLECT)))
                    p_Amount = 0;
            }

            void CalculateAgility(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_Agility = l_Player->GetStat(Stats::STAT_AGILITY);

                /// Set amount to 0 if agility isn't the highest stat
                if ((l_Agility < l_Player->GetStat(Stats::STAT_STRENGTH)) ||
                    (l_Agility < l_Player->GetStat(Stats::STAT_INTELLECT)))
                    p_Amount = 0;
            }

            void CalculateIntellect(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_Intellect = l_Player->GetStat(Stats::STAT_INTELLECT);

                /// Set amount to 0 if intellect isn't the highest stat
                if ((l_Intellect < l_Player->GetStat(Stats::STAT_STRENGTH)) ||
                    (l_Intellect < l_Player->GetStat(Stats::STAT_AGILITY)))
                    p_Amount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_amanthuls_grandeur_AuraScript::CalculateStrength , SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_STAT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_amanthuls_grandeur_AuraScript::CalculateAgility, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_MOD_STAT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_amanthuls_grandeur_AuraScript::CalculateIntellect, SpellEffIndex::EFFECT_2, AuraType::SPELL_AURA_MOD_STAT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_amanthuls_grandeur_AuraScript();
        }
};

/// Called by Umbral Glaive Storm - 242553
class at_item_umbral_glaive_storm : public AreaTriggerEntityScript
{
    public:
        at_item_umbral_glaive_storm() : AreaTriggerEntityScript("at_item_umbral_glaive_storm") { }

        enum eSpell
        {
            ShatteringUmbralGlaives = 242557
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Item* l_CastItem = nullptr;
            if (p_AreaTrigger->GetCastItemGuid() && l_Caster->IsPlayer())
                l_CastItem = l_Caster->ToPlayer()->GetItemByGuid(p_AreaTrigger->GetCastItemGuid());

            l_Caster->CastSpell(p_AreaTrigger->GetPosition(), eSpell::ShatteringUmbralGlaives, true, l_CastItem);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_item_umbral_glaive_storm();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Umbral Glaive Storm - 242553
class spell_item_umbral_glaive_storm : public SpellScriptLoader
{
    public:
        spell_item_umbral_glaive_storm() : SpellScriptLoader("spell_item_umbral_glaive_storm") { }

        class spell_item_umbral_glaive_storm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_umbral_glaive_storm_AuraScript);

            enum eSpell
            {
                UmbralGlaiveStormTick = 242556
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (AreaTrigger* l_AT = l_Caster->GetAreaTrigger(GetSpellInfo()->Id))
                {
                    Item* l_CastItem = nullptr;
                    if (l_AT->GetCastItemGuid() && l_Caster->IsPlayer())
                        l_CastItem = l_Caster->ToPlayer()->GetItemByGuid(l_AT->GetCastItemGuid());

                    l_Caster->CastSpell(l_AT->GetPosition(), eSpell::UmbralGlaiveStormTick, true, l_CastItem);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_umbral_glaive_storm_AuraScript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_umbral_glaive_storm_AuraScript();
        }
};

// 203808 203843 203844 203845
class spell_item_pvp_pennants_toy : public SpellScript
{
    PrepareSpellScript(spell_item_pvp_pennants_toy);

    void HandleScript(SpellEffIndex effIndex)
    {
        if (auto player = GetCaster()->ToPlayer())
        {
            auto IsAlliance = player->GetTeam() == ALLIANCE;

            if (GetSpellInfo()->Id == 203808)
                player->CastSpell(player, IsAlliance ? 203812 : 203814, true);
            else if (GetSpellInfo()->Id == 203843)
                player->CastSpell(player, IsAlliance ? 203846 : 203849, true);
            else if (GetSpellInfo()->Id == 203844)
                player->CastSpell(player, IsAlliance ? 203847 : 203850, true);
            else if (GetSpellInfo()->Id == 203845)
                player->CastSpell(player, IsAlliance ? 203848 : 203852, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_pvp_pennants_toy::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 201600
class spell_item_lavish_suramar_feast_use : public SpellScript
{
    PrepareSpellScript(spell_item_lavish_suramar_feast_use);

    void HandleScript(SpellEffIndex effIndex)
    {
        if (auto caster = GetCaster())
        {
            caster->CastSpell(caster, 192002, true);
            caster->CastSpell(caster, 201644, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_lavish_suramar_feast_use::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 201644
class spell_item_lavish_suramar_feast : public AuraScript
{
    PrepareAuraScript(spell_item_lavish_suramar_feast);

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        if (auto caster = GetUnitOwner())
        {
            if (auto plr = caster->ToPlayer())
            {
                uint32 spellId = 0;

                switch (plr->GetPrimaryStat())
                {
                    case STAT_INTELLECT:
                        spellId = 201640;
                        break;
                    case STAT_STRENGTH:
                        spellId = 201638;
                        break;
                    case STAT_AGILITY:
                        spellId = 201639;
                        break;
                    default:
                        break;
                }

                if (plr->IsActiveSpecTankSpec())
                    spellId = 201641;

                caster->CastSpell(caster, spellId, true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_lavish_suramar_feast::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 251461 251459
class spell_item_call_of_the_void_stalker_trinket : public SpellScript
{
    PrepareSpellScript(spell_item_call_of_the_void_stalker_trinket);

    void HandleAfterCast()
    {
        auto caster = GetCaster();
        auto target = GetExplTargetUnit();

        if (!caster || !target)
            return;

        caster->CastSpell(target, 251035, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_item_call_of_the_void_stalker_trinket::HandleAfterCast);
    }
};


// 251035 (AT 16113) - Call of the Void Stalker
struct areatrigger_item_coth_void_stalker : public AreaTriggerAI
{
    areatrigger_item_coth_void_stalker(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) {}

    void OnUnitEnter(Unit* unit) override
    {
        if (auto caster = at->GetCaster())
            if (auto aura = caster->GetAura(250908))
                if (auto player = caster->ToPlayer())
                    if (auto item = player->GetItemByGuid(aura->GetCastItemGUID()))
                        if (item->IsEquipped())
                            caster->CastSpell(unit, 251034, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_CASTED_BY_AREATRIGGER), item);
    }
};

#ifndef __clang_analyzer__
void AddSC_item_spell_scripts()
{
    RegisterSpellScript(spell_item_call_of_the_void_stalker_trinket);
    RegisterSpellScript(spell_item_pvp_pennants_toy);
    RegisterSpellScript(spell_item_lavish_suramar_feast_use);
    RegisterAuraScript(spell_item_lavish_suramar_feast);
    RegisterAreaTriggerAI(areatrigger_item_coth_void_stalker);
    new spell_item_artifical_damage();
    new spell_item_masquerade();
    new spell_item_stormsinger();
    new spell_item_ley_enriched_water();
    new spell_item_spirit_realm();
    new spell_item_leytorrent_potion();
    new PlayerScript_leytorrent_potion();
    new spell_item_deadly_grace();
    new spell_item_allies_of_nature();
    new spell_item_nerubian_chitin();
    new spell_item_nerubian_chitin_periodic();
    new spell_item_nerubian_chitin_stack();
    new spell_item_incensed();
    new spell_item_volatile_magic();
    new spell_item_devilsaurs_stampede();
    new spell_item_devilsaurs_stampede_periodic();
    new spell_item_ice_bomb();
    new spell_item_burning_intensity();
    new spell_item_dark_blast();
    new spell_item_taint_of_the_sea_caster();
    new spell_item_taint_of_the_sea_target();
    new spell_item_cleansing_flame();
    new spell_item_solemnity();
    new spell_item_infested_ground();
    new spell_item_leeching_pestilence();
    new spell_item_avalanche_elixir();
    new spell_item_avalanche_elixir_visual();
    // 175442 Blessing of A'dal
    new spell_item_blessing_of_adal();
    // 67689 Ghostly Backfire
    new spell_item_ghostly_backfire();
    // 23074 Arcanite Dragonling
    new spell_item_trigger_spell("spell_item_arcanite_dragonling", SPELL_ARCANITE_DRAGONLING);
    // 23133 Gnomish Battle Chicken
    new spell_item_trigger_spell("spell_item_gnomish_battle_chicken", SPELL_BATTLE_CHICKEN);
    // 23076 Mechanical Dragonling
    new spell_item_trigger_spell("spell_item_mechanical_dragonling", SPELL_MECHANICAL_DRAGONLING);
    // 23075 Mithril Mechanical Dragonling
    new spell_item_trigger_spell("spell_item_mithril_mechanical_dragonling", SPELL_MITHRIL_MECHANICAL_DRAGONLING);
    new spell_item_deviate_fish();
    new spell_item_super_simian_sphere();
    new spell_item_the_perfect_blossom();
    new spell_item_mystic_image();
    new spell_item_sunreaver_beacon();
    new spell_item_spike_toed_booterang();
    new spell_item_flask_of_the_north();
    new spell_item_gnomish_death_ray();
    new spell_item_make_a_wish();
    new spell_item_mingos_fortune_generator();
    new spell_item_net_o_matic();
    new spell_item_noggenfogger_elixir();
    new spell_item_savory_deviate_delight();
    new spell_item_six_demon_bag();
    new spell_item_underbelly_elixir();
    new spell_item_shadowmourne();
    new spell_item_red_rider_air_rifle();
    new spell_item_create_heart_candy();
    new spell_item_book_of_glyph_mastery();
    new spell_item_gift_of_the_harvester();
    new spell_item_map_of_the_geyser_fields();
    new spell_item_vanquished_clutches();
    new spell_magic_eater_food();
    new spell_item_refocus();
    new spell_item_shimmering_vessel();
    new spell_item_purify_helboar_meat();
    new spell_item_crystal_prison_dummy_dnd();
    new spell_item_reindeer_transformation();
    new spell_item_nigh_invulnerability();
    new spell_item_poultryizer();
    new spell_item_socrethars_stone();
    new spell_item_demon_broiled_surprise();
    new spell_item_complete_raptor_capture();
    new spell_item_impale_leviroth();
    new spell_item_brewfest_mount_transformation();
    new spell_item_nitro_boots();
    new spell_item_teach_language();
    new spell_item_rocket_boots();
    new spell_item_pygmy_oil();
    new spell_item_unusual_compass();
    new spell_item_chicken_cover();
    new spell_item_muisek_vessel();
    new spell_item_greatmothers_soulcatcher();
    new spell_item_enohar_explosive_arrows();
    new spell_item_holy_thurible();
    new spell_item_bandage_q24944();
    new spell_item_gen_alchemy_mop();
    new spell_alchemist_rejuvenation();
    new spell_item_amber_prison();
    new spell_item_first_aid();
    new spell_item_imputting_the_final_code();
    new spell_item_pot_of_fire();
    new spell_item_dit_da_jow();
    new spell_item_zuluhed_chains();
    new spell_item_yak_s_milk();
    new spell_item_throw_mantra();
    new spell_item_ancient_knowledge();
    new spell_item_sky_golem();
    new spell_item_engineering_scopes("spell_item_oglethorpe_s_missile_splitter", eEngineeringScopesSpells::SpellOglethorpesMissileSplitter);
    new spell_item_engineering_scopes("spell_item_megawatt_filament", eEngineeringScopesSpells::SpellMegawattFilament);
    new spell_item_engineering_scopes("spell_item_hemet_s_heartseeker", eEngineeringScopesSpells::HemetsHeartseeker);
    new spell_item_summon_chauffeur();
    new spell_item_forgemasters_vigor();
    new spell_item_hammer_blows();
    new spell_item_detonation();
    new spell_item_detonating();
    new spell_item_battering();
    new spell_item_cracks();
    new spell_item_sanatizing();
    new spell_item_cleansing_steam();
    new spell_item_molten_metal();
    new spell_item_pouring_slag();
    new spell_item_legs_of_iron();
    new spell_item_chest_of_iron();
    new spell_item_helm_of_iron();
    new spell_item_shoulders_of_iron();
    new spell_item_gauntlets_of_iron();
    new spell_create_reward_item();
    new spell_item_skymirror_image();
    new spell_item_memory_of_mr_smite();
    new spell_item_celestial_defender();
    new spell_item_curious_bronze_timepiece_alliance();
    new spell_item_curious_bronze_timepiece_horde();
    new spell_item_ascend_to_bladespire();
    new spell_item_faded_wizard_hat();
    new spell_item_reflecting_prism();
    new spell_item_ascend_to_karabor();
    new spell_item_sargerei_disguise();
    new spell_item_swapblaster();
    new spell_item_bronze_claws();
    new spell_item_tonga_totem();
    new spell_item_petrified_root();
    new spell_item_hypnotize_critter();
    new spell_item_enduring_elixir_of_wisdom();
    new spell_item_Plans_Balanced_Trillium_Ingot_and_Its_Uses();
    new spell_item_artificial_stamina();
    new spell_item_potion_of_the_old_war();
    new spell_item_bough_of_corruption();
    new spell_item_bough_of_corruption_spread();
    new spell_item_eye_of_skovald();
    new spell_item_maddening_whispers();
    new spell_item_maddening_whispers_damage();
    new spell_item_shadowy_reflection();
    new spell_item_shadowy_reflection_absorb();
    new spell_item_six_feather_fan();
    new spell_item_six_feather_fan_damage();
    new spell_item_screams_of_the_dead();
    new spell_item_nightwell_energy();
    new spell_item_elunes_light();
    new spell_item_imp_generator();
    new spell_item_imp_generator_portal();
    new spell_item_darkmoon_deck();
    new spell_item_phased_webbing();
    new spell_item_nightmare_egg_shell();
    new spell_item_volatile_ichor();
    new spell_item_mechanical_bomb_squirrel();
    new spell_item_writhing_heart_of_darkness();
    new spell_item_opening_hand();
    new spell_item_opening_hand_buff();
    new PlayerScript_eyasus_mulligan();
    new spell_item_darkening_soul();
    new spell_item_darkening_soul_damage_reduction();
    new spell_item_nightmarish_ichor();
    new spell_item_raging_storm();
    new spell_item_raging_storm_damage();
    new spell_item_stormsinger_bonus();
    new spell_item_mote_of_sanctification();
    new spell_item_leyflame_burner();
    new spell_item_spirit_flask();
    new spell_item_flesh_to_stone();
    new spell_item_spirit_fragment();
    new spell_item_warlords_fortitude();
    new spell_item_dinner_bell();
    new PlayerScript_dinner_bell();
    new spell_item_stabilized_energy();
    new spell_item_fragile_echoes();
    new spell_item_fragile_echoes_proc();
    new spell_item_particle_arranger();
    new spell_item_arrogance();
    new spell_item_fragile_echoes_restore();
    new spell_item_sands_of_time();
    new spell_item_solar_collapse();
    new spell_item_carrion_swarm();
    new spell_item_accelerando();
    new spell_item_star_gate();
    new spell_item_crystalline_shockwave();
    new spell_item_constellations();
    new spell_item_triumvirate();
    new spell_item_nefarious_pact();
    new spell_item_temporal_shift();
    new spell_item_temporal_shift_buff();
    new spell_item_living_carapace();
    new spell_item_infernal_contract();
    new spell_item_backlash();
    new spell_item_plague_swarm();
    new spell_item_grand_commedation();
    new spell_item_shadow_wave();
    new spell_item_prescience();
    new spell_item_orb_of_destruction();
    new spell_item_recursive_strike();
    new spell_item_recursive_strike_damage();
    new spell_item_colossal_smash();
    new spell_item_colossal_smash_damage();
    new spell_item_feretory_of_souls();
    new spell_item_dryad_spear();
    new spell_item_nightwell_tranquility();
    new spell_item_infernal_skin();
    new spell_item_guilty_conscience();
    new spell_item_strength_of_will();
    new spell_item_rising_tides();
    new spell_item_oceans_embrace();
    new spell_item_oceans_embrace_procs();
    new spell_item_oceans_embrace_targeting();
    new spell_item_ceaseless_toxin();
    new spell_item_terror_from_below_damage();
    new spell_item_terror_from_below_proc();
    new spell_item_chalice_of_moonlight();
    new spell_item_spectral_owl();
    new spell_item_wailing_souls();
    new spell_item_grace_of_the_creators();
    new spell_item_cleansing_matrix();
    new spell_item_guiding_hand();
    new spell_item_insidious_corruption();
    new spell_item_tremendous_fortitude();
    new spell_item_infernal_skin();
    new spell_item_chrono_shard_acceleration();
    new spell_item_cunning_of_the_deceiver();
    new spell_item_strength_of_will_proc();
    new spell_item_dreadstone_of_endless_shadows();
    new spell_item_infernal_alchemist_stone();
    new spell_item_tos_melee_dps_trinkets();
    new spell_item_void_tendril();
    new spell_item_shadow_blades();
    new spell_item_shadow_blades_trigger();
    new spell_item_shadow_blades_periodic();
    new spell_item_shadow_blades_damage();
    new spell_item_prototype_personnel_decimator();
    new spell_item_netherlords_accursed_wrathsteed();
    new spell_item_summon_dread_reflection();
    new spell_item_command_dread_reflections();
    new spell_item_demonic_vigor();
    new spell_item_fragment_of_vigor();
    new spell_item_apocalypse_drive();
    new spell_item_fire_mines();
    new spell_item_fire_mines_detonate();
    new spell_item_refreshing_agony_periodic();
    new spell_item_shadow_strike();
    new spell_khadgars_wand_215910();

    /// Tier Bonus T19
    new spell_item_tier_set();

    /// Class Hall armor upgrade
    new spell_item_upgrade_class_hall_armor();

    new spell_create_nighthold_set_item();
    new spell_create_tos_set_item();

    /// Legendary upgrade
    new spell_item_titan_essence_940_upgrade();
    new spell_item_titan_essence_970_upgrade();
    new spell_item_titan_essence_1000_upgrade();

    new spell_create_season_3_item();
    new spell_create_season_4_item();
    new spell_create_season_5_item();
    new spell_create_season_6_item();
    new spell_create_season_7_item();

    new spell_item_trinkets_discipline_priest_nerf();
    new spell_item_fel_crazed_rage_damage();
    new spell_create_antorus_set_item();
    new spell_item_stance_of_the_mountain();
    new spell_item_soul_sap();
    new spell_item_spawn_of_serpentrix();
    new spell_item_legions_gaze();
    new spell_item_flame_gale();
    new spell_item_feedback_loop();
    new spell_item_prototype_personnel_decimator_proc();
    new spell_item_swap_hounds();
    new spell_item_shadown_singed_fang_proc();
    new spell_item_felshield();
    new spell_item_unstable_portals();
    new spell_item_tarratus_keystone();
    new spell_item_eye_of_the_hounds();
    new spell_item_reverberating_vitality();
    new spell_item_echo_of_gorshalach();
    new spell_item_echo_of_gorshalach_check();
    new spell_item_bulwark_of_flame();
    new spell_item_highfathers_timekeeping();
    new spell_item_highfathers_timekeeping_buff();
    new spell_item_highfathers_timekeeping_heal();
    new spell_item_legion_bombardment();
    new spell_item_cycle_of_the_legion();
    new spell_item_buffs_of_the_legion();
    new spell_item_antorus_vantus_rune();
    new at_item_umbral_glaive_storm();
    new spell_item_umbral_glaive_storm();

    ////////////////////////////////////////////////////////////////////////////////////////
    /// Argus' Pantheon's trinkets
    ////////////////////////////////////////////////////////////////////////////////////////
    new spell_item_eonars_verdant_embrace();
    new spell_item_norgannons_command();
    new spell_item_khazgoroths_shaping();
    new spell_item_amanthuls_grandeur();
    new spell_item_mark_of_khazgoroth();
    new spell_item_worldforgers_flame();
    new spell_item_gorshalachs_legacy();
    ////////////////////////////////////////////////////////////////////////////////////////
}
#endif
