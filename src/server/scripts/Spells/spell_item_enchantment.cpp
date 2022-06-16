////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SkillDiscovery.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "InstanceScript.h"
#include "Group.h"
#include "LFGMgr.h"
#include "Player.h"
#include "GameEventMgr.h"

namespace eEnchantmentMarkProc
{
    enum
    {
        Thunderlord     = 159234,
        Warsong         = 159675,
        BleedingHollow  = 173322,
        Frostwolf       = 159676,
        Blackrock       = 159679,
        Shadowmoon      = 159678,
        Claw            = 190909,
        HeavyHide       = 228399
    };
}

namespace eEnchantmentMarkAura
{
    enum
    {
        Thunderlord     = 159243,
        Warsong         = 159682,
        BleedingHollow  = 173321,
        Frostwolf       = 159683,
        Blackrock       = 159685,
        Shadowmoon      = 159684,
        Claw            = 190888,
        HeavyHide       = 228398
    };
}

namespace eEnchantmentMarkIds
{
    enum
    {
        Thunderlord     = 5330,
        Warsong         = 5337,
        BleedingHollow  = 5384,
        Frostwolf       = 5334,
        Blackrock       = 5336,
        Shadowmoon      = 5335,
        Claw            = 5437,
        HeavyHide       = 5889
    };
}

namespace eGloryEnchantmentMarkIds
{
    enum
    {
        Thunderlord     = 5352,
        Warsong         = 5355,
        Frostwolf       = 5356,
        Blackrock       = 5354,
        Shadowmoon      = 5353,
        Claw            = 5437,
        HeavyHide       = 5889
    };
}

/// Mark of the Thunderlord - 159243
/// Mark of Warsong - 159682
/// Mark of the Frostwolf - 159683
/// Mark of Blackrock - 159685
/// Mark of Bleeding Hollow - 173321
class spell_enchantment_mark : public SpellScriptLoader
{
    public:
        spell_enchantment_mark() : SpellScriptLoader("spell_enchantment_mark") { }

        class spell_enchantment_mark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_enchantment_mark_AuraScript);

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                if (!GetOwner())
                    return;

                Player* l_Player = GetOwner()->ToPlayer();
                if (l_Player == nullptr)
                    return;

                uint32 l_SpellID = GetSpellInfo()->Id;
                if (l_SpellID == eEnchantmentMarkAura::Blackrock && l_Player->GetHealthPct() >= 60.0f)
                {
                    PreventDefaultAction();
                    return;
                }

                uint32 l_ProcAuraId     = 0;
                uint32 l_EnchantId      = 0;
                uint32 l_GloryEnchantId = 0;
                bool l_HasEnchant       = false;

                Item* l_Item = nullptr;

                switch (l_SpellID)
                {
                    case eEnchantmentMarkAura::Claw:
                    case eEnchantmentMarkAura::HeavyHide:
                        l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_NECK);
                        break;
                    default:
                        if (p_EventInfo.GetTypeMask() & ProcFlags::PROC_FLAG_DONE_OFFHAND_ATTACK)
                            l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_OFFHAND);
                        else
                            l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND);
                        break;
                }


                if (l_Item == nullptr)
                    return;

                switch (l_SpellID)
                {
                    case eEnchantmentMarkAura::Thunderlord:
                        l_ProcAuraId = eEnchantmentMarkProc::Thunderlord;
                        l_EnchantId = eEnchantmentMarkIds::Thunderlord;
                        l_GloryEnchantId = eGloryEnchantmentMarkIds::Thunderlord;
                        break;
                    case eEnchantmentMarkAura::Warsong:
                        l_ProcAuraId = eEnchantmentMarkProc::Warsong;
                        l_EnchantId = eEnchantmentMarkIds::Warsong;
                        l_GloryEnchantId = eGloryEnchantmentMarkIds::Warsong;
                        break;
                    case eEnchantmentMarkAura::BleedingHollow:
                        l_ProcAuraId = eEnchantmentMarkProc::BleedingHollow;
                        l_EnchantId = eEnchantmentMarkIds::BleedingHollow;
                        break;
                    case eEnchantmentMarkAura::Frostwolf:
                        l_ProcAuraId = eEnchantmentMarkProc::Frostwolf;
                        l_EnchantId = eEnchantmentMarkIds::Frostwolf;
                        l_GloryEnchantId = eGloryEnchantmentMarkIds::Frostwolf;
                        break;
                    case eEnchantmentMarkAura::Shadowmoon:
                        l_ProcAuraId = eEnchantmentMarkProc::Shadowmoon;
                        l_EnchantId = eEnchantmentMarkIds::Shadowmoon;
                        l_GloryEnchantId = eGloryEnchantmentMarkIds::Shadowmoon;
                        break;
                     case eEnchantmentMarkAura::Blackrock:
                        l_ProcAuraId = eEnchantmentMarkProc::Blackrock;
                        l_EnchantId = eEnchantmentMarkIds::Blackrock;
                        l_GloryEnchantId = eGloryEnchantmentMarkIds::Blackrock;
                        break;
                     case eEnchantmentMarkAura::Claw:
                        l_ProcAuraId = eEnchantmentMarkProc::Claw;
                        l_EnchantId = eEnchantmentMarkIds::Claw;
                        l_GloryEnchantId = eGloryEnchantmentMarkIds::Claw;
                        break;
                     case eEnchantmentMarkAura::HeavyHide:
                         l_ProcAuraId = eEnchantmentMarkProc::HeavyHide;
                         l_EnchantId = eEnchantmentMarkIds::HeavyHide;
                         l_GloryEnchantId = eGloryEnchantmentMarkIds::HeavyHide;
                         break;
                    default:
                        break;
                }

                if (!l_Item || l_Player->HasSpellCooldown(l_ProcAuraId))
                    return;

                /// Check if we have this enchant on that item
                for (uint32 l_Slot = EnchantmentSlot::PERM_ENCHANTMENT_SLOT; l_Slot < EnchantmentSlot::MAX_ENCHANTMENT_SLOT; ++l_Slot)
                {
                    if (int32 l_ItemEnchantId = l_Item->GetEnchantmentId(EnchantmentSlot(l_Slot)))
                    {
                        if (l_ItemEnchantId == l_EnchantId || l_ItemEnchantId == l_GloryEnchantId)
                        {
                            l_HasEnchant = true;
                            break;
                        }
                    }
                }

                if (l_HasEnchant && l_Player && l_ProcAuraId && l_Item)
                {
                    l_Player->CastSpell(l_Player, l_ProcAuraId, true, l_Item);
                    l_Player->AddSpellCooldown(l_ProcAuraId, 0, 1 * TimeConstants::IN_MILLISECONDS, true);
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eEnchantmentMarkAura::Shadowmoon:
                    case eEnchantmentMarkAura::Blackrock:
                        OnEffectProc += AuraEffectProcFn(spell_enchantment_mark_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
                        break;
                    default:
                        OnEffectProc += AuraEffectProcFn(spell_enchantment_mark_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_enchantment_mark_AuraScript();
        }
};

/// last update : 6.1.2 19865
/// Shattered Bleed - 159238
class spell_Mark_Of_The_Shattered_Bleed : public SpellScriptLoader
{
    public:
        spell_Mark_Of_The_Shattered_Bleed() : SpellScriptLoader("spell_Mark_Of_The_Shattered_Bleed") { }

        class spell_Mark_Of_The_Shattered_Bleed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_Mark_Of_The_Shattered_Bleed_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->IsValidAssistTarget(l_Target))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_Mark_Of_The_Shattered_Bleed_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_Mark_Of_The_Shattered_Bleed_SpellScript();
        }
};

/// last update 7.1.5
/// Mark of the Ancient Priestess - 228401
class spell_mark_of_the_ancient_priestess : public SpellScriptLoader
{
    public:
        spell_mark_of_the_ancient_priestess() : SpellScriptLoader("spell_mark_of_the_ancient_priestess") { }

        class spell_mark_of_the_ancient_priestess_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mark_of_the_ancient_priestess_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* l_Target) -> bool
                {
                    if (l_Target == l_Caster)
                        return true;
                    return false;
                });

                if (p_Targets.empty())
                    return;

                auto l_RandomItr = p_Targets.begin();
                std::advance(l_RandomItr, urand(0, p_Targets.size() - 1));
                WorldObject* l_Target = *l_RandomItr;
                p_Targets.clear();
                p_Targets.push_back(l_Target);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mark_of_the_ancient_priestess_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mark_of_the_ancient_priestess_SpellScript();
        }
};

/// Last Update 7.1.5
/// Mark of the Hidden Satyr - 190890
class spell_Mark_Of_The_Hidden_Satyr : public SpellScriptLoader
{
    public:
        spell_Mark_Of_The_Hidden_Satyr() : SpellScriptLoader("spell_Mark_Of_The_Hidden_Satyr") { }

        class spell_Mark_Of_The_Hidden_Satyr_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_Mark_Of_The_Hidden_Satyr_AuraScript);

            enum eSpells
            {
                MarkOfTheHiddenSatyr = 191259
            };

            void HandleOnProc(ProcEventInfo&  p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                /// Following patch 7.1.5 | No Damage on all spell Mark of the Hidden Satyr
                int32 l_Bp = l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                if (l_Bp == 0)
                    l_Bp = l_Player->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                AddPct(l_Bp, l_Player->IsMeleeDamageDealer(true) ? 150 : 100);
                float l_Versatility = l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f;

                l_Bp *= (1 + l_Versatility);
                Unit* l_Target = l_DamageInfo->GetTarget();
                if (!l_Target)
                    return;

                if (l_Caster->IsValidAttackTarget(l_Target))
                    l_Caster->CastCustomSpell(l_Target, eSpells::MarkOfTheHiddenSatyr, &l_Bp, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_Mark_Of_The_Hidden_Satyr_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_Mark_Of_The_Hidden_Satyr_AuraScript();
        }
};

class spell_enchantment_mark_of_the_distant_army : public SpellScriptLoader
{
    public:
        spell_enchantment_mark_of_the_distant_army() : SpellScriptLoader("spell_mark_of_the_distant_army") { }

        class spell_enchantment_mark_of_the_distant_army_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_enchantment_mark_of_the_distant_army_AuraScript);

            void HandleCalcAmount(AuraEffect const*, int32& p_Amount, bool&)
            {
                if (!GetCaster() || !GetCaster()->IsPlayer())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                int32 l_SpellPower = l_Player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                int32 l_AttackPower = l_Player->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                l_SpellPower = CalculatePct(l_SpellPower, 200);
                l_AttackPower = CalculatePct(l_AttackPower, 250);

                p_Amount = ((l_SpellPower > l_AttackPower) ? l_SpellPower : l_AttackPower);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_enchantment_mark_of_the_distant_army_AuraScript::HandleCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_enchantment_mark_of_the_distant_army_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called By Herb Gathering - 195114  for Faster Herbalism 190970
/// Called By Skinning - 195125 for Faster Skinning 190973
/// Called By Mining - 195122 For Faster Mining 190971
class spell_faster_gathering : public SpellScriptLoader
{
    public:
        spell_faster_gathering() : SpellScriptLoader("spell_faster_gathering") { }

        class spell_faster_gathering_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_faster_gathering_SpellScript);

            enum eSpells
            {
                FasterMining    = 190971,
                FasterSkinning  = 190973,
                FasterHerbalism = 190970
            };

            enum eGatherinSkill
            {
                Herbalism       = 195114,
                SkinningSkill   = 195125,
                Skinning        = 194174,
                SkinningBase    = 8613,
                Mining          = 195122
            };

            std::vector<std::pair<eSpells, eGatherinSkill>> m_Spells =
            {
                {
                    eSpells::FasterMining,
                    eGatherinSkill::Mining
                },
                {
                    eSpells::FasterSkinning,
                    eGatherinSkill::SkinningSkill
                },
                {
                    eSpells::FasterSkinning,
                    eGatherinSkill::Skinning
                },
                {
                    eSpells::FasterSkinning,
                    eGatherinSkill::SkinningBase
                },
                {
                    eSpells::FasterHerbalism,
                    eGatherinSkill::Herbalism
                },
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Spell* l_Spell = GetSpell();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Spell || !l_SpellInfo)
                    return;

                bool l_HasFasterAura = false;

                for (auto l_SpellId : m_Spells)
                    if (l_Caster->HasAura(l_SpellId.first) && l_SpellInfo->Id == l_SpellId.second)
                        l_HasFasterAura = true;

                if (!l_HasFasterAura)
                    return;

                l_Spell->SetCastTime(500); ///< Tauren's or people with the enchants got 0.5 gather speed
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_faster_gathering_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_faster_gathering_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_spell_item_enchantment()
{
    new spell_enchantment_mark_of_the_distant_army();
    new spell_mark_of_the_ancient_priestess();
    new spell_enchantment_mark();
    new spell_Mark_Of_The_Shattered_Bleed();
    new spell_Mark_Of_The_Hidden_Satyr();
    new spell_faster_gathering();
}
#endif
