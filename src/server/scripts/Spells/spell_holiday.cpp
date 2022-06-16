////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Spells used in holidays/game events that do not fit any other category.
 * Scriptnames in this file should be prefixed with "spell_#holidayname_".
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "CellImpl.h"
#include "Vehicle.h"

// 45102 Romantic Picnic
enum SpellsPicnic
{
    SPELL_BASKET_CHECK              = 45119, // Holiday - Valentine - Romantic Picnic Near Basket Check
    SPELL_MEAL_PERIODIC             = 45103, // Holiday - Valentine - Romantic Picnic Meal Periodic - effect dummy
    SPELL_MEAL_EAT_VISUAL           = 45120, // Holiday - Valentine - Romantic Picnic Meal Eat Visual
    //SPELL_MEAL_PARTICLE             = 45114, // Holiday - Valentine - Romantic Picnic Meal Particle - unused
    SPELL_DRINK_VISUAL              = 45121, // Holiday - Valentine - Romantic Picnic Drink Visual
    SPELL_ROMANTIC_PICNIC_ACHIEV    = 45123, // Romantic Picnic periodic = 5000
};

class spell_love_is_in_the_air_romantic_picnic: public SpellScriptLoader
{
    public:
        spell_love_is_in_the_air_romantic_picnic() : SpellScriptLoader("spell_love_is_in_the_air_romantic_picnic") { }

        class spell_love_is_in_the_air_romantic_picnic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_love_is_in_the_air_romantic_picnic_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->SetStandState(UNIT_STAND_STATE_SIT);
                target->CastSpell(target, SPELL_MEAL_PERIODIC, false);
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                // Every 5 seconds
                Unit* target = GetTarget();
                Unit* caster = GetCaster();

                // If our player is no longer sit, remove all auras
                if (target->getStandState() != UNIT_STAND_STATE_SIT)
                {
                    target->RemoveAura(SPELL_ROMANTIC_PICNIC_ACHIEV);
                    target->RemoveAura(GetAura());
                    return;
                }

                target->CastSpell(target, SPELL_BASKET_CHECK, false); // unknown use, it targets Romantic Basket
                target->CastSpell(target, RAND(SPELL_MEAL_EAT_VISUAL, SPELL_DRINK_VISUAL), false);

                bool foundSomeone = false;
                // For nearby players, check if they have the same aura. If so, cast Romantic Picnic (45123)
                // required by achievement and "hearts" visual
                std::list<Player*> playerList;
                JadeCore::AnyPlayerInObjectRangeCheck checker(target, INTERACTION_DISTANCE*2);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> searcher(target, playerList, checker);
                target->VisitNearbyWorldObject(INTERACTION_DISTANCE*2, searcher);
                for (std::list<Player*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    if ((*itr) != target && (*itr)->HasAura(GetId())) // && (*itr)->getStandState() == UNIT_STAND_STATE_SIT)
                    {
                        if (caster)
                        {
                            caster->CastSpell(*itr, SPELL_ROMANTIC_PICNIC_ACHIEV, true);
                            caster->CastSpell(target, SPELL_ROMANTIC_PICNIC_ACHIEV, true);
                        }
                        foundSomeone = true;
                        // break;
                    }
                }

                if (!foundSomeone && target->HasAura(SPELL_ROMANTIC_PICNIC_ACHIEV))
                    target->RemoveAura(SPELL_ROMANTIC_PICNIC_ACHIEV);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_love_is_in_the_air_romantic_picnic_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_love_is_in_the_air_romantic_picnic_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_love_is_in_the_air_romantic_picnic_AuraScript();
        }
};

enum Mistletoe
{
    SPELL_CREATE_MISTLETOE  = 26206,
    SPELL_CREATE_HOLLY      = 26207,
    SPELL_CREATE_SNOWFLAKES = 45036
};

class spell_winter_veil_mistletoe: public SpellScriptLoader
{
    public:
        spell_winter_veil_mistletoe() : SpellScriptLoader("spell_winter_veil_mistletoe") { }

        class spell_winter_veil_mistletoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_winter_veil_mistletoe_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_MISTLETOE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_CREATE_HOLLY) ||
                    !sSpellMgr->GetSpellInfo(SPELL_CREATE_SNOWFLAKES))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (Player* target = GetHitPlayer())
                {
                    uint32 spellId = 0;
                    switch (urand(0, 2))
                    {
                        case 0:
                            spellId = SPELL_CREATE_MISTLETOE;
                            break;
                        case 1:
                            spellId = SPELL_CREATE_HOLLY;
                            break;
                        case 2:
                            spellId = SPELL_CREATE_SNOWFLAKES;
                            break;
                        default:
                            return;
                    }

                    caster->CastSpell(target, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_winter_veil_mistletoe_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_winter_veil_mistletoe_SpellScript();
        }
};

// 26275 - PX-238 Winter Wondervolt TRAP
enum PX238WinterWondervolt
{
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_1  = 26157,
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_2  = 26272,
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_3  = 26273,
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_4  = 26274
};

class spell_winter_veil_px_238_winter_wondervolt : public SpellScriptLoader
{
    public:
        spell_winter_veil_px_238_winter_wondervolt() : SpellScriptLoader("spell_winter_veil_px_238_winter_wondervolt") { }

        class spell_winter_veil_px_238_winter_wondervolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_winter_veil_px_238_winter_wondervolt_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_1) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_2) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_3) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_4))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                uint32 const spells[4] =
                {
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_1,
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_2,
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_3,
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_4
                };

                if (Unit* target = GetHitUnit())
                {
                    for (uint8 i = 0; i < 4; ++i)
                        if (target->HasAura(spells[i]))
                            return;

                    target->CastSpell(target, spells[urand(0, 3)], true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_winter_veil_px_238_winter_wondervolt_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_winter_veil_px_238_winter_wondervolt_SpellScript();
        }
};

enum HallowEndCandysSpells
{
    SPELL_HALLOWS_END_CANDY_ORANGE_GIANT          = 24924, // Effect 1: Apply Aura: Mod Size, Value: 30%
    SPELL_HALLOWS_END_CANDY_SKELETON              = 24925, // Effect 1: Apply Aura: Change Model (Skeleton). Effect 2: Apply Aura: Underwater Breathing
    SPELL_HALLOWS_END_CANDY_PIRATE                = 24926, // Effect 1: Apply Aura: Increase Swim Speed, Value: 50%
    SPELL_HALLOWS_END_CANDY_GHOST                 = 24927, // Effect 1: Apply Aura: Levitate / Hover. Effect 2: Apply Aura: Slow Fall, Effect 3: Apply Aura: Water Walking
    SPELL_HALLOWS_END_CANDY_FEMALE_DEFIAS_PIRATE  = 44742, // Effect 1: Apply Aura: Change Model (Defias Pirate, Female). Effect 2: Increase Swim Speed, Value: 50%
    SPELL_HALLOWS_END_CANDY_MALE_DEFIAS_PIRATE    = 44743  // Effect 1: Apply Aura: Change Model (Defias Pirate, Male).   Effect 2: Increase Swim Speed, Value: 50%
};

// 24930 - Hallow's End Candy
class spell_hallow_end_candy : public SpellScriptLoader
{
    public:
        spell_hallow_end_candy() : SpellScriptLoader("spell_hallow_end_candy") { }

        class spell_hallow_end_candy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_candy_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                for (uint32 spellId : spells)
                    if (!sSpellMgr->GetSpellInfo(spellId))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell(GetCaster(), spells[urand(0, 3)], true);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_hallow_end_candy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        private:
            static uint32 const spells[4];
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hallow_end_candy_SpellScript();
        }
};

uint32 const spell_hallow_end_candy::spell_hallow_end_candy_SpellScript::spells[4] =
{
    SPELL_HALLOWS_END_CANDY_ORANGE_GIANT,
    SPELL_HALLOWS_END_CANDY_SKELETON,
    SPELL_HALLOWS_END_CANDY_PIRATE,
    SPELL_HALLOWS_END_CANDY_GHOST
};

// 24926 - Hallow's End Candy
class spell_hallow_end_candy_pirate : public SpellScriptLoader
{
    public:
        spell_hallow_end_candy_pirate() : SpellScriptLoader("spell_hallow_end_candy_pirate") { }

        class spell_hallow_end_candy_pirate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hallow_end_candy_pirate_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWS_END_CANDY_FEMALE_DEFIAS_PIRATE)
                    || !sSpellMgr->GetSpellInfo(SPELL_HALLOWS_END_CANDY_MALE_DEFIAS_PIRATE))
                    return false;
                return true;
            }

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                uint32 spell = GetTarget()->getGender() == GENDER_FEMALE ? SPELL_HALLOWS_END_CANDY_FEMALE_DEFIAS_PIRATE : SPELL_HALLOWS_END_CANDY_MALE_DEFIAS_PIRATE;
                GetTarget()->CastSpell(GetTarget(), spell, true);
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                uint32 spell = GetTarget()->getGender() == GENDER_FEMALE ? SPELL_HALLOWS_END_CANDY_FEMALE_DEFIAS_PIRATE : SPELL_HALLOWS_END_CANDY_MALE_DEFIAS_PIRATE;
                GetTarget()->RemoveAurasDueToSpell(spell);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hallow_end_candy_pirate_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_SWIM_SPEED, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hallow_end_candy_pirate_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_SWIM_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hallow_end_candy_pirate_AuraScript();
        }
};

// 24750 Trick
enum TrickSpells
{
    SPELL_PIRATE_COSTUME_MALE           = 24708,
    SPELL_PIRATE_COSTUME_FEMALE         = 24709,
    SPELL_NINJA_COSTUME_MALE            = 24710,
    SPELL_NINJA_COSTUME_FEMALE          = 24711,
    SPELL_LEPER_GNOME_COSTUME_MALE      = 24712,
    SPELL_LEPER_GNOME_COSTUME_FEMALE    = 24713,
    SPELL_SKELETON_COSTUME              = 24723,
    SPELL_GHOST_COSTUME_MALE            = 24735,
    SPELL_GHOST_COSTUME_FEMALE          = 24736,
    SPELL_TRICK_BUFF                    = 24753,
};

class spell_hallow_end_trick : public SpellScriptLoader
{
    public:
        spell_hallow_end_trick() : SpellScriptLoader("spell_hallow_end_trick") { }

        class spell_hallow_end_trick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_trick_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE)
                    || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE)
                    || !sSpellMgr->GetSpellInfo(SPELL_SKELETON_COSTUME) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_TRICK_BUFF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    uint8 gender = target->getGender();
                    uint32 spellId = SPELL_TRICK_BUFF;
                    switch (urand(0, 5))
                    {
                        case 1:
                            spellId = gender == GENDER_FEMALE ? SPELL_LEPER_GNOME_COSTUME_FEMALE : SPELL_LEPER_GNOME_COSTUME_MALE;
                            break;
                        case 2:
                            spellId = gender == GENDER_FEMALE ? SPELL_PIRATE_COSTUME_FEMALE : SPELL_PIRATE_COSTUME_MALE;
                            break;
                        case 3:
                            spellId = gender == GENDER_FEMALE ? SPELL_GHOST_COSTUME_FEMALE : SPELL_GHOST_COSTUME_MALE;
                            break;
                        case 4:
                            spellId = gender == GENDER_FEMALE ? SPELL_NINJA_COSTUME_FEMALE : SPELL_NINJA_COSTUME_MALE;
                            break;
                        case 5:
                            spellId = SPELL_SKELETON_COSTUME;
                            break;
                        default:
                            break;
                    }

                    caster->CastSpell(target, spellId, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hallow_end_trick_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hallow_end_trick_SpellScript();
        }
};

// 24751 Trick or Treat
enum TrickOrTreatSpells
{
    SPELL_TRICK                 = 24714,
    SPELL_TREAT                 = 24715,
    SPELL_TRICKED_OR_TREATED    = 24755,
    SPELL_TRICKY_TREAT_SPEED    = 42919,
    SPELL_TRICKY_TREAT_TRIGGER  = 42965,
    SPELL_UPSET_TUMMY           = 42966
};

class spell_hallow_end_trick_or_treat : public SpellScriptLoader
{
    public:
        spell_hallow_end_trick_or_treat() : SpellScriptLoader("spell_hallow_end_trick_or_treat") { }

        class spell_hallow_end_trick_or_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_trick_or_treat_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICK) || !sSpellMgr->GetSpellInfo(SPELL_TREAT) || !sSpellMgr->GetSpellInfo(SPELL_TRICKED_OR_TREATED))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    caster->CastSpell(target, roll_chance_i(50) ? SPELL_TRICK : SPELL_TREAT, true);
                    caster->CastSpell(target, SPELL_TRICKED_OR_TREATED, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hallow_end_trick_or_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hallow_end_trick_or_treat_SpellScript();
        }
};

class spell_hallow_end_tricky_treat : public SpellScriptLoader
{
    public:
        spell_hallow_end_tricky_treat() : SpellScriptLoader("spell_hallow_end_tricky_treat") { }

        class spell_hallow_end_tricky_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_tricky_treat_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_SPEED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_TRIGGER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_UPSET_TUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->HasAura(SPELL_TRICKY_TREAT_TRIGGER) && caster->GetAuraCount(SPELL_TRICKY_TREAT_SPEED) > 3 && roll_chance_i(33))
                    caster->CastSpell(caster, SPELL_UPSET_TUMMY, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hallow_end_tricky_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hallow_end_tricky_treat_SpellScript();
        }
};

// Hallowed wands
enum HallowendData
{
    //wand spells
    SPELL_HALLOWED_WAND_PIRATE             = 24717,
    SPELL_HALLOWED_WAND_NINJA              = 24718,
    SPELL_HALLOWED_WAND_LEPER_GNOME        = 24719,
    SPELL_HALLOWED_WAND_RANDOM             = 24720,
    SPELL_HALLOWED_WAND_SKELETON           = 24724,
    SPELL_HALLOWED_WAND_WISP               = 24733,
    SPELL_HALLOWED_WAND_GHOST              = 24737,
    SPELL_HALLOWED_WAND_BAT                = 24741
};

class spell_hallow_end_wand : public SpellScriptLoader
{
public:
    spell_hallow_end_wand() : SpellScriptLoader("spell_hallow_end_wand") {}

    class spell_hallow_end_wand_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hallow_end_wand_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE) ||
                !sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE) ||
                !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE) ||
                !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE) ||
                !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE) ||
                !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE) ||
                !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE) ||
                !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE))
                return false;
            return true;
        }

        void HandleScriptEffect()
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();

            uint32 spellId = 0;
            uint8 gender = target->getGender();

            switch (GetSpellInfo()->Id)
            {
                case SPELL_HALLOWED_WAND_LEPER_GNOME:
                    spellId = gender ? SPELL_LEPER_GNOME_COSTUME_FEMALE : SPELL_LEPER_GNOME_COSTUME_MALE;
                    break;
                case SPELL_HALLOWED_WAND_PIRATE:
                    spellId = gender ? SPELL_PIRATE_COSTUME_FEMALE : SPELL_PIRATE_COSTUME_MALE;
                    break;
                case SPELL_HALLOWED_WAND_GHOST:
                    spellId = gender ? SPELL_GHOST_COSTUME_FEMALE : SPELL_GHOST_COSTUME_MALE;
                    break;
                case SPELL_HALLOWED_WAND_NINJA:
                    spellId = gender ? SPELL_NINJA_COSTUME_FEMALE : SPELL_NINJA_COSTUME_MALE;
                    break;
                case SPELL_HALLOWED_WAND_RANDOM:
                    spellId = RAND(SPELL_HALLOWED_WAND_PIRATE, SPELL_HALLOWED_WAND_NINJA, SPELL_HALLOWED_WAND_LEPER_GNOME, SPELL_HALLOWED_WAND_SKELETON, SPELL_HALLOWED_WAND_WISP, SPELL_HALLOWED_WAND_GHOST, SPELL_HALLOWED_WAND_BAT);
                    break;
                default:
                    return;
            }
            caster->CastSpell(target, spellId, true);
        }

        void Register() override
        {
            AfterHit += SpellHitFn(spell_hallow_end_wand_SpellScript::HandleScriptEffect);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_hallow_end_wand_SpellScript();
    }
};

/// Throw Torch - 95958
class spell_hallow_end_throw_torch : public SpellScriptLoader
{
    public:
        spell_hallow_end_throw_torch() : SpellScriptLoader("spell_hallow_end_throw_torch") {}

        class spell_hallow_end_throw_torch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_throw_torch_SpellScript);

            enum eNpcs
            {
                Npc_WickermanTorchPoint = 51720
            };

            enum eGobs
            {
                Gob_Wickerman = 180433
            };

            enum eSpells
            {
                Spell_Unburdened = 95987
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToPlayer() || !l_Target)
                    return;

                if (l_Caster->ToPlayer()->GetTeam() == ALLIANCE)
                {
                    if (Aura* l_Aura = l_Caster->GetAura(eSpells::Spell_Unburdened))
                    {
                        l_Aura->RefreshDuration();
                    }
                    else
                    {
                        l_Target->CastSpell(l_Caster, eSpells::Spell_Unburdened, true);
                    }
                }

                if (GameObject* l_Wickerman = l_Target->FindNearestGameObject(eGobs::Gob_Wickerman, 50.0f))
                {
                    l_Wickerman->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hallow_end_throw_torch_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hallow_end_throw_torch_SpellScript();
        }
};

/// Dousing Agent - 96022, 95964
class spell_hallow_end_dousing_agent : public SpellScriptLoader
{
    public:
        spell_hallow_end_dousing_agent() : SpellScriptLoader("spell_hallow_end_dousing_agent") {}

        class spell_hallow_end_dousing_agent_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_dousing_agent_SpellScript);

            enum eSpells
            {
                Spell_DousingAgentHorde = 96022,
                Spell_DousingAgentAlliance = 95964,
                Spell_WaterSteamImpact = 76974,
                Spell_DousingAgentQuestCredit = 99930
            };

            enum eNpcs
            {
                Npc_WickermanTorchPointHorde = 51720,
                Npc_WickermanTorchPointAlliance = 51684
            };

            enum eGobs
            {
                Gob_Wickerman = 180433
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToPlayer() || !l_Target)
                    return;

                l_Target->CastSpell(l_Caster, eSpells::Spell_DousingAgentQuestCredit, true);

                if (m_scriptSpellId == eSpells::Spell_DousingAgentHorde)
                {
                    std::list<Creature*> l_WickermanTorchPointsList;
                    l_Target->GetCreatureListWithEntryInGrid(l_WickermanTorchPointsList, eNpcs::Npc_WickermanTorchPointAlliance, 50.0f);

                    if (l_WickermanTorchPointsList.empty())
                        return;

                    for (auto l_Itr : l_WickermanTorchPointsList)
                    {
                        l_Itr->CastSpell(l_Itr, eSpells::Spell_WaterSteamImpact, true);
                    }
                }
                else if (m_scriptSpellId == eSpells::Spell_DousingAgentAlliance)
                {
                    std::list<Creature*> l_WickermanTorchPointsList;
                    l_Target->GetCreatureListWithEntryInGrid(l_WickermanTorchPointsList, eNpcs::Npc_WickermanTorchPointHorde, 50.0f);

                    if (l_WickermanTorchPointsList.empty())
                        return;

                    for (auto l_Itr : l_WickermanTorchPointsList)
                    {
                        l_Itr->CastSpell(l_Itr, eSpells::Spell_WaterSteamImpact, true);
                    }
                }

                if (GameObject* l_Wickerman = l_Target->FindNearestGameObject(eGobs::Gob_Wickerman, 50.0f))
                {
                    l_Wickerman->SetGoState(GO_STATE_READY);
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_hallow_end_dousing_agent_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hallow_end_dousing_agent_SpellScript();
        }
};

/// Clean Up Stink Bomb - 96030
class spell_hallow_end_clean_up_stink_bomb : public SpellScriptLoader
{
    public:
        spell_hallow_end_clean_up_stink_bomb() : SpellScriptLoader("spell_hallow_end_clean_up_stink_bomb") { }

        class spell_hallow_end_clean_up_stink_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_clean_up_stink_bomb_SpellScript);

            enum eGobs
            {
                Gob_StinkBombCloud = 208200
            };

            enum eKillcredits
            {
                Killcredit_CleanUpQuestCredit = 52553
            };

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                GameObject* l_Gob = GetHitGObj();
                if (!l_Caster || !l_Caster->ToPlayer() || !l_Gob || l_Gob->GetEntry() != eGobs::Gob_StinkBombCloud)
                    return;

                if (!l_Gob->GetDBTableGUIDLow())
                {
                    l_Gob->Delete();
                }
                else
                {
                    l_Gob->AddToHideList(l_Caster->GetGUID(), 360000);
                    l_Gob->DestroyForPlayer(l_Caster->ToPlayer());
                }

                l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_CleanUpQuestCredit);

            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hallow_end_clean_up_stink_bomb_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hallow_end_clean_up_stink_bomb_SpellScript();
        }
};

///Backport from trinity 3.3.5
// 45724 - Braziers Hit!
enum TorchSpells
{
    SPELL_TORCH_TOSSING_TRAINING = 45716,
    SPELL_TORCH_TOSSING_PRACTICE = 46630,
    SPELL_TORCH_TOSSING_TRAINING_SUCCESS_ALLIANCE = 45719,
    SPELL_TORCH_TOSSING_TRAINING_SUCCESS_HORDE = 46651,
    SPELL_BRAZIERS_HIT = 45724
};

class spell_midsummer_braziers_hit : public SpellScriptLoader
{
public:
    spell_midsummer_braziers_hit() : SpellScriptLoader("spell_midsummer_braziers_hit") { }

    class spell_midsummer_braziers_hit_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_midsummer_braziers_hit_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_TORCH_TOSSING_TRAINING) || !sSpellMgr->GetSpellInfo(SPELL_TORCH_TOSSING_PRACTICE))
                return false;
            return true;
        }

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Player* player = GetTarget()->ToPlayer();
            if (!player)
                return;

            if ((player->HasAura(SPELL_TORCH_TOSSING_TRAINING) && GetStackAmount() == 8) || (player->HasAura(SPELL_TORCH_TOSSING_PRACTICE) && GetStackAmount() == 20))
            {
                if (player->GetTeam() == ALLIANCE)
                    player->CastSpell(player, SPELL_TORCH_TOSSING_TRAINING_SUCCESS_ALLIANCE, true);
                else if (player->GetTeam() == HORDE)
                    player->CastSpell(player, SPELL_TORCH_TOSSING_TRAINING_SUCCESS_HORDE, true);
                Remove();
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_midsummer_braziers_hit_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AuraEffectHandleModes(AURA_EFFECT_HANDLE_REAPPLY));
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_midsummer_braziers_hit_AuraScript();
    }
};

/// Pass food from chair - 66262, 66259, 66260, 66250, 66261
class spell_pilgrims_bounty_pass_from_chair : public SpellScriptLoader
{
    public:
        spell_pilgrims_bounty_pass_from_chair() : SpellScriptLoader("spell_pilgrims_bounty_pass_from_chair") { }

        class spell_pilgrims_bounty_pass_from_chair_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pilgrims_bounty_pass_from_chair_SpellScript);

            enum eSpells
            {
                // Vehicle Throw Spells
                PassTheSweetPotatoes            = 66262,
                PassTheStuffing                 = 66259,
                PassThePie                      = 66260,
                PassTheTurkey                   = 66250,
                PassTheCranberries              = 66261,
                // Visual Throw Spells
                OnPlateSweetPotatoes            = 61824,
                OnPlateStuffing                 = 61823,
                OnPlatePie                      = 61825,
                OnPlateTurkey                   = 61822,
                OnPlateCranberries              = 61821,
                // Visual Throw-Bounce Spells
                OnPlateBounceSweetPotatoes      = 61929,
                OnPlateBounceStuffing           = 61927,
                OnPlateBouncePie                = 61926,
                OnPlateBounceTurkey             = 61928,
                OnPlateBounceCranberries        = 61925
            };

            bool IsBountifulTableChair(Unit* p_Unit)
            {
                std::vector<uint32> l_ChairEntries =
                {
                    34812,
                    34823,
                    34819,
                    34824,
                    34822
                };

                if (p_Unit->ToPlayer())
                    return false;

                for (auto l_Itr : l_ChairEntries)
                {
                    if (p_Unit->GetEntry() == l_Itr)
                        return true;
                }

                return false;
            }

            Unit* GetPlate(Unit* p_Unit)
            {
                uint8 l_SeatIndex = p_Unit->GetTransSeat();

                Vehicle* l_Table = p_Unit->GetVehicle();
                if (!l_Table)
                    return nullptr;

                Unit* l_PlateHolder = l_Table->GetPassenger(6);
                if (!l_PlateHolder)
                    return nullptr;

                Vehicle* l_Holder = l_PlateHolder->GetVehicleKit();
                if (!l_Holder)
                    return nullptr;

                Unit* l_Plate = l_Holder->GetPassenger(l_SeatIndex);
                if (!l_Plate)
                    return nullptr;
                else
                    return l_Plate;

                return nullptr;
            }

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Vehicle* l_CasterVehicle = l_Caster->GetVehicleKit();
                Vehicle* l_TargetVehicle = l_Target->GetVehicle();
                if (!l_CasterVehicle || !l_TargetVehicle)
                    return;

                Unit* l_Passenger = l_CasterVehicle->GetPassenger(0);
                Unit* l_TargetChair = l_TargetVehicle->GetBase();
                if (!l_Passenger || !l_TargetChair || l_Passenger == l_Target)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::PassTheSweetPotatoes:
                    {
                        if (IsBountifulTableChair(l_Target))
                        {
                            l_Passenger->CastSpell(GetPlate(l_Target), eSpells::OnPlateSweetPotatoes, true);
                        }
                        else if (l_Target->ToPlayer() && IsBountifulTableChair(l_TargetChair))
                        {
                            l_Passenger->CastSpell(l_Target, eSpells::OnPlateBounceSweetPotatoes, true);
                        }

                        break;
                    }
                    case eSpells::PassTheStuffing:
                    {
                        if (IsBountifulTableChair(l_Target))
                        {
                            l_Passenger->CastSpell(GetPlate(l_Target), eSpells::OnPlateStuffing, true);
                        }
                        else if (l_Target->ToPlayer() && IsBountifulTableChair(l_TargetChair))
                        {
                            l_Passenger->CastSpell(l_Target, eSpells::OnPlateBounceStuffing, true);
                        }

                        break;
                    }
                    case eSpells::PassThePie:
                    {
                        if (IsBountifulTableChair(l_Target))
                        {
                            l_Passenger->CastSpell(GetPlate(l_Target), eSpells::OnPlatePie, true);
                        }
                        else if (l_Target->ToPlayer() && IsBountifulTableChair(l_TargetChair))
                        {
                            l_Passenger->CastSpell(l_Target, eSpells::OnPlateBouncePie, true);
                        }

                        break;
                    }
                    case eSpells::PassTheTurkey:
                    {
                        if (IsBountifulTableChair(l_Target))
                        {
                            l_Passenger->CastSpell(GetPlate(l_Target), eSpells::OnPlateTurkey, true);
                        }
                        else if (l_Target->ToPlayer() && IsBountifulTableChair(l_TargetChair))
                        {
                            l_Passenger->CastSpell(l_Target, eSpells::OnPlateBounceTurkey, true);
                        }

                        break;
                    }
                    case eSpells::PassTheCranberries:
                    {
                        if (IsBountifulTableChair(l_Target))
                        {
                            l_Passenger->CastSpell(GetPlate(l_Target), eSpells::OnPlateCranberries, true);
                        }
                        else if (l_Target->ToPlayer() && IsBountifulTableChair(l_TargetChair))
                        {
                            l_Passenger->CastSpell(l_Target, eSpells::OnPlateBounceCranberries, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pilgrims_bounty_pass_from_chair_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pilgrims_bounty_pass_from_chair_SpellScript();
        }
};

/// Throw to chair - 61824, 61823, 61825, 61822, 61821
class spell_pilgrims_bounty_pass_to_chair : public SpellScriptLoader
{
    public:
        spell_pilgrims_bounty_pass_to_chair() : SpellScriptLoader("spell_pilgrims_bounty_pass_to_chair") { }

        class spell_pilgrims_bounty_pass_to_chair_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pilgrims_bounty_pass_to_chair_SpellScript);

            enum eSpells
            {
                // Visual Throw Spells
                OnPlateSweetPotatoes            = 61824,
                OnPlateStuffing                 = 61823,
                OnPlatePie                      = 61825,
                OnPlateTurkey                   = 61822,
                OnPlateCranberries              = 61821,
                // Eaten buffs for chairs
                AServingOfSweetPotatoes         = 61808,
                AServingOfStuffing              = 61806,
                AServingOfPie                   = 61805,
                AServingOfTurkey                = 61807,
                AServingOfCranberries           = 61804,
                // Eaten buffs for plates
                AServingOfSweetPotatoesPlate    = 61837,
                AServingOfStuffingPlate         = 61836,
                AServingOfPiePlate              = 61838,
                AServingOfTurkeyPlate           = 61835,
                AServingOfCranberriesPlate      = 61833,
                // Achievement Spells
                PassTheSweetPotatoesAchievement = 66376,
                PassTheStuffingAchievement      = 66375,
                PassThePieAchievement           = 66374,
                PassTheTurkeyAchievement        = 66373,
                PassTheCranberriesAchievement   = 66372
            };

            Unit* GetChair(Unit* p_Unit)
            {
                uint8 l_SeatIndex = p_Unit->GetTransSeat();

                Vehicle* l_PlateHolder = p_Unit->GetVehicle();
                if (!l_PlateHolder)
                    return nullptr;

                Vehicle* l_Table = l_PlateHolder->GetBase()->GetVehicle();
                if (!l_Table)
                    return nullptr;

                Unit* l_Chair = l_Table->GetPassenger(l_SeatIndex);
                if (!l_Chair)
                    return nullptr;
                else
                    return l_Chair;

                return nullptr;
            }

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || l_Caster == l_Target ||
                    !l_Caster->GetVehicle())
                    return;

                Unit* l_Chair = GetChair(l_Target);
                if (!l_Chair)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::OnPlateSweetPotatoes:
                    {
                        if (Aura* l_Aura = l_Chair->GetAura(eSpells::AServingOfSweetPotatoes))
                        {
                            if (l_Aura->GetStackAmount() < 5)
                            {
                                l_Aura->SetStackAmount(5);
                            }

                            l_Aura->RefreshDuration();
                        }
                        else if (Aura* l_Aura = l_Chair->AddAura(eSpells::AServingOfSweetPotatoes, l_Chair))
                        {
                            l_Aura->SetStackAmount(5);

                            if (!l_Target->HasAura(eSpells::AServingOfSweetPotatoesPlate))
                                l_Target->CastSpell(l_Target, eSpells::AServingOfSweetPotatoesPlate, true);

                            l_Caster->CastSpell(l_Chair, l_Aura->GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
                        }

                        l_Caster->CastSpell(l_Caster, eSpells::PassTheSweetPotatoesAchievement, true);
                        break;
                    }
                    case eSpells::OnPlateStuffing:
                    {
                        if (Aura* l_Aura = l_Chair->GetAura(eSpells::AServingOfStuffing))
                        {
                            if (l_Aura->GetStackAmount() < 5)
                            {
                                l_Aura->SetStackAmount(5);
                            }

                            l_Aura->RefreshDuration();
                        }
                        else if (Aura* l_Aura = l_Chair->AddAura(eSpells::AServingOfStuffing, l_Chair))
                        {
                            l_Aura->SetStackAmount(5);

                            if (!l_Target->HasAura(eSpells::AServingOfStuffingPlate))
                                l_Target->CastSpell(l_Target, eSpells::AServingOfStuffingPlate, true);

                            l_Caster->CastSpell(l_Chair, l_Aura->GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
                        }

                        l_Caster->CastSpell(l_Caster, eSpells::PassTheStuffingAchievement, true);
                        break;
                    }
                    case eSpells::OnPlatePie:
                    {
                        if (Aura* l_Aura = l_Chair->GetAura(eSpells::AServingOfPie))
                        {
                            if (l_Aura->GetStackAmount() < 5)
                            {
                                l_Aura->SetStackAmount(5);
                            }

                            l_Aura->RefreshDuration();
                        }
                        else if (Aura* l_Aura = l_Chair->AddAura(eSpells::AServingOfPie, l_Chair))
                        {
                            l_Aura->SetStackAmount(5);

                            if (!l_Target->HasAura(eSpells::AServingOfPiePlate))
                                l_Target->CastSpell(l_Target, eSpells::AServingOfPiePlate, true);

                            l_Caster->CastSpell(l_Chair, l_Aura->GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
                        }

                        l_Caster->CastSpell(l_Caster, eSpells::PassThePieAchievement, true);
                        break;
                    }
                    case eSpells::OnPlateTurkey:
                    {
                        if (Aura* l_Aura = l_Chair->GetAura(eSpells::AServingOfTurkey))
                        {
                            if (l_Aura->GetStackAmount() < 5)
                            {
                                l_Aura->SetStackAmount(5);
                            }

                            l_Aura->RefreshDuration();
                        }
                        else if (Aura* l_Aura = l_Chair->AddAura(eSpells::AServingOfTurkey, l_Chair))
                        {
                            l_Aura->SetStackAmount(5);

                            if (!l_Target->HasAura(eSpells::AServingOfTurkeyPlate))
                                l_Target->CastSpell(l_Target, eSpells::AServingOfTurkeyPlate, true);

                            l_Caster->CastSpell(l_Chair, l_Aura->GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
                        }

                        l_Caster->CastSpell(l_Caster, eSpells::PassTheTurkeyAchievement, true);
                        break;
                    }
                    case eSpells::OnPlateCranberries:
                    {
                        if (Aura* l_Aura = l_Chair->GetAura(eSpells::AServingOfCranberries))
                        {
                            if (l_Aura->GetStackAmount() < 5)
                            {
                                l_Aura->SetStackAmount(5);
                            }

                            l_Aura->RefreshDuration();
                        }
                        else if (Aura* l_Aura = l_Chair->AddAura(eSpells::AServingOfCranberries, l_Chair))
                        {
                            l_Aura->SetStackAmount(5);

                            if (!l_Target->HasAura(eSpells::AServingOfCranberriesPlate))
                                l_Target->CastSpell(l_Target, eSpells::AServingOfCranberriesPlate, true);

                            l_Caster->CastSpell(l_Chair, l_Aura->GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
                        }

                        l_Caster->CastSpell(l_Caster, eSpells::PassTheCranberriesAchievement, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pilgrims_bounty_pass_to_chair_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pilgrims_bounty_pass_to_chair_SpellScript();
        }
};

/// Eating food - 61784, 61785, 61788, 61786, 61787
class spell_pilgrims_bounty_eating_food : public SpellScriptLoader
{
    public:
        spell_pilgrims_bounty_eating_food() : SpellScriptLoader("spell_pilgrims_bounty_eating_food") { }

        class spell_pilgrims_bounty_eating_food_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pilgrims_bounty_eating_food_SpellScript);

            enum eSpells
            {
                // Vehicle eating spells
                FeastOnSweetPotatoes         = 61786,
                FeastOnStuffing              = 61788,
                FeastOnPie                   = 61787,
                FeastOnTurkey                = 61784,
                FeastOnCranberries           = 61785,
                // Eating pre-buffs
                SweetPotatoHelpings          = 61844,
                StuffingHelpings             = 61843,
                PieHelpings                  = 61845,
                TurkeyHelpings               = 61842,
                CranberryHelpings            = 61841,
                // Misc
                OnPlateEatVisual             = 61826
            };

            void HandleOnEffectHit(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Vehicle* l_Chair = l_Caster->GetVehicleKit();
                if (!l_Chair)
                    return;

                Unit* l_PlayerPassenger = l_Chair->GetPassenger(0);
                if (!l_PlayerPassenger)
                    return;

                l_Caster->RemoveAuraFromStack(GetSpellInfo()->Effects[EFFECT_0].BasePoints);
                l_PlayerPassenger->CastSpell(l_PlayerPassenger, eSpells::OnPlateEatVisual, true);

                switch (m_scriptSpellId)
                {
                    case eSpells::FeastOnSweetPotatoes:
                    {
                        l_PlayerPassenger->CastSpell(l_PlayerPassenger, eSpells::SweetPotatoHelpings, true);
                        break;
                    }
                    case eSpells::FeastOnStuffing:
                    {
                        l_PlayerPassenger->CastSpell(l_PlayerPassenger, eSpells::StuffingHelpings, true);
                        break;
                    }
                    case eSpells::FeastOnPie:
                    {
                        l_PlayerPassenger->CastSpell(l_PlayerPassenger, eSpells::PieHelpings, true);
                        break;
                    }
                    case eSpells::FeastOnTurkey:
                    {
                        l_PlayerPassenger->CastSpell(l_PlayerPassenger, eSpells::TurkeyHelpings, true);
                        break;
                    }
                    case eSpells::FeastOnCranberries:
                    {
                        l_PlayerPassenger->CastSpell(l_PlayerPassenger, eSpells::CranberryHelpings, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pilgrims_bounty_eating_food_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pilgrims_bounty_eating_food_SpellScript();
        }
};

/// Eating buffs on chairs - 61808, 61806, 61805, 61807, 61804
class spell_pilgrims_bounty_eating_buffs_on_chairs : public SpellScriptLoader
{
    public:
        spell_pilgrims_bounty_eating_buffs_on_chairs() : SpellScriptLoader("spell_pilgrims_bounty_eating_buffs_on_chairs") { }

        class spell_pilgrims_bounty_eating_buffs_on_chairs_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bounty_eating_buffs_on_chairs_AuraScript);

            enum eSpells
            {
                // Eaten buffs for chairs
                AServingOfSweetPotatoes      = 61808,
                AServingOfStuffing           = 61806,
                AServingOfPie                = 61805,
                AServingOfTurkey             = 61807,
                AServingOfCranberries        = 61804,
                // Eaten buffs for plates
                AServingOfSweetPotatoesPlate = 61837,
                AServingOfStuffingPlate      = 61836,
                AServingOfPiePlate           = 61838,
                AServingOfTurkeyPlate        = 61835,
                AServingOfCranberriesPlate   = 61833
            };

            void HandleEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                uint8 l_SeatIndex = 0;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_SeatIndex = l_Target->GetTransSeat();

                Vehicle* l_Table = l_Target->GetVehicle();
                if (!l_Table)
                    return;

                Unit* l_PlateHolderUnit = l_Table->GetPassenger(6);
                if (!l_PlateHolderUnit)
                    return;

                Vehicle* l_PlateHolderVehicle = l_PlateHolderUnit->GetVehicleKit();
                if (!l_PlateHolderVehicle)
                    return;

                Unit* l_Plate = l_PlateHolderVehicle->GetPassenger(l_SeatIndex);
                if (!l_Plate)
                    return;

                l_Target->RemoveAura(GetSpellInfo()->Effects[EFFECT_0].BasePoints);

                switch (m_scriptSpellId)
                {
                    case eSpells::AServingOfSweetPotatoes:
                    {
                        l_Plate->RemoveAura(eSpells::AServingOfSweetPotatoesPlate);
                        break;
                    }
                    case eSpells::AServingOfStuffing:
                    {
                        l_Plate->RemoveAura(eSpells::AServingOfStuffingPlate);
                        break;
                    }
                    case eSpells::AServingOfPie:
                    {
                        l_Plate->RemoveAura(eSpells::AServingOfPiePlate);
                        break;
                    }
                    case eSpells::AServingOfTurkey:
                    {
                        l_Plate->RemoveAura(eSpells::AServingOfTurkeyPlate);
                        break;
                    }
                    case eSpells::AServingOfCranberries:
                    {
                        l_Plate->RemoveAura(eSpells::AServingOfCranberriesPlate);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_pilgrims_bounty_eating_buffs_on_chairs_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pilgrims_bounty_eating_buffs_on_chairs_AuraScript();
        }
};

/// Eating buffs on players - 61808, 61806, 61805, 61807, 61804
class spell_pilgrims_bounty_eating_buffs_on_players : public SpellScriptLoader
{
    public:
        spell_pilgrims_bounty_eating_buffs_on_players() : SpellScriptLoader("spell_pilgrims_bounty_eating_buffs_on_players") { }

        class spell_pilgrims_bounty_eating_buffs_on_players_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bounty_eating_buffs_on_players_AuraScript);

            enum eSpells
            {
                // Eating pre-buffs
                SweetPotatoHelpings = 61844,
                StuffingHelpings    = 61843,
                PieHelpings         = 61845,
                TurkeyHelpings      = 61842,
                CranberryHelpings   = 61841,
                // Eating stat-buffs
                WellFedSweetPotato  = 65410,
                WellFedStuffing     = 226807,
                WellFedPie          = 175785,
                WellFedTurkey       = 175784,
                WellFedCranberry    = 226805,
                // Rep buff
                TheSpiritOfSharing  = 61849
            };

            void HandleEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::SweetPotatoHelpings:
                    {
                        if (GetStackAmount() == 5)
                        {
                            l_Caster->CastSpell(l_Caster, eSpells::WellFedSweetPotato, true);
                        }

                        break;
                    }
                    case eSpells::StuffingHelpings:
                    {
                        if (GetStackAmount() == 5)
                        {
                            l_Caster->CastSpell(l_Caster, eSpells::WellFedStuffing, true);
                        }

                        break;
                    }
                    case eSpells::PieHelpings:
                    {
                        if (GetStackAmount() == 5)
                        {
                            l_Caster->CastSpell(l_Caster, eSpells::WellFedPie, true);
                        }

                        break;
                    }
                    case eSpells::TurkeyHelpings:
                    {
                        if (GetStackAmount() == 5)
                        {
                            l_Caster->CastSpell(l_Caster, eSpells::WellFedTurkey, true);
                        }

                        break;
                    }
                    case eSpells::CranberryHelpings:
                    {
                        if (GetStackAmount() == 5)
                        {
                            l_Caster->CastSpell(l_Caster, eSpells::WellFedCranberry, true);
                        }

                        break;
                    }
                    default:
                        break;
                }

                Aura* l_Potato = l_Caster->GetAura(eSpells::SweetPotatoHelpings);
                Aura* l_Stuffing = l_Caster->GetAura(eSpells::StuffingHelpings);
                Aura* l_Pie = l_Caster->GetAura(eSpells::PieHelpings);
                Aura* l_Turkey = l_Caster->GetAura(eSpells::TurkeyHelpings);
                Aura* l_Cranberry = l_Caster->GetAura(eSpells::CranberryHelpings);
                if (!l_Potato || !l_Stuffing || !l_Pie || !l_Turkey || !l_Cranberry)
                    return;

                if (l_Potato->GetStackAmount() == 5 && l_Stuffing->GetStackAmount() == 5 &&
                    l_Pie->GetStackAmount() == 5 && l_Turkey->GetStackAmount() == 5 &&
                    l_Cranberry->GetStackAmount() == 5)
                {
                    l_Potato->Remove();
                    l_Stuffing->Remove();
                    l_Pie->Remove();
                    l_Turkey->Remove();
                    l_Cranberry->Remove();
                    l_Caster->CastSpell(l_Caster, eSpells::TheSpiritOfSharing, true);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pilgrims_bounty_eating_buffs_on_players_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pilgrims_bounty_eating_buffs_on_players_AuraScript();
        }
};

/// Turkey Tracker - 62014
class spell_pilgrims_bounty_turkey_tracker : public SpellScriptLoader
{
    public:
        spell_pilgrims_bounty_turkey_tracker() : SpellScriptLoader("spell_pilgrims_bounty_turkey_tracker") { }

        class spell_pilgrims_bounty_turkey_tracker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pilgrims_bounty_turkey_tracker_SpellScript);

            enum eSpells
            {
                KillCounterVisual    = 62015,
                KillCounterVisualMax = 62021
            };

            void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Caster->ToCreature() ||
                    !l_Target || !l_Target->ToPlayer())
                    return;

                if (Aura* l_Aura = l_Target->GetAura(GetSpellInfo()->Id))
                {
                    switch (l_Aura->GetStackAmount())
                    {
                        case 10:
                        {
                            l_Caster->ToCreature()->AI()->Talk(0, l_Target->GetGUID());
                            l_Target->CastSpell(l_Target, eSpells::KillCounterVisual, true);
                            break;
                        }
                        case 20:
                        {
                            l_Caster->ToCreature()->AI()->Talk(1, l_Target->GetGUID());
                            l_Target->CastSpell(l_Target, eSpells::KillCounterVisual, true);
                            break;
                        }
                        case 30:
                        {
                            l_Caster->ToCreature()->AI()->Talk(2, l_Target->GetGUID());
                            l_Target->CastSpell(l_Target, eSpells::KillCounterVisual, true);
                            break;
                        }
                        case 40:
                        {
                            if (!l_Target->HasAura(eSpells::KillCounterVisualMax))
                            {
                                l_Caster->ToCreature()->AI()->Talk(3, l_Target->GetGUID());
                                l_Target->CastSpell(l_Target, eSpells::KillCounterVisual, true);
                                l_Target->CastSpell(l_Target, eSpells::KillCounterVisualMax, true);
                            }

                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pilgrims_bounty_turkey_tracker_SpellScript::HandleHitTarget, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pilgrims_bounty_turkey_tracker_SpellScript();
        }
};


#ifndef __clang_analyzer__
void AddSC_holiday_spell_scripts()
{
    // Love is in the Air
    new spell_love_is_in_the_air_romantic_picnic();

    // Hallow's End
    new spell_hallow_end_candy();
    new spell_hallow_end_candy_pirate();
    new spell_hallow_end_trick();
    new spell_hallow_end_trick_or_treat();
    new spell_hallow_end_tricky_treat();
    new spell_hallow_end_wand();
    new spell_hallow_end_throw_torch();
    new spell_hallow_end_dousing_agent();
    new spell_hallow_end_clean_up_stink_bomb();

    // Winter Veil
    new spell_winter_veil_mistletoe();
    new spell_winter_veil_px_238_winter_wondervolt();

    // Midsummer Fire Festival
    /// new spell_midsummer_braziers_hit();

    // Pilgrim's Bounty
    new spell_pilgrims_bounty_pass_from_chair();
    new spell_pilgrims_bounty_pass_to_chair();
    new spell_pilgrims_bounty_eating_food();
    new spell_pilgrims_bounty_eating_buffs_on_chairs();
    new spell_pilgrims_bounty_eating_buffs_on_players();
    new spell_pilgrims_bounty_turkey_tracker();
}
#endif
