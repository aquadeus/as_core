////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Unit.h"
#include "Player.h"
#include "Pet.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "SharedDefines.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "World.h"
#include "GameTables.h"
#include "HelperDefines.h"
#include "CreatureAI.h"

inline bool _ModifyUInt32(bool apply, uint32& baseValue, int32& amount)
{
    // If amount is negative, change sign and value of apply.
    if (amount < 0)
    {
        apply = !apply;
        amount = -amount;
    }
    if (apply)
        baseValue += amount;
    else
    {
        // Make sure we do not get uint32 overflow.
        if (amount > int32(baseValue))
            amount = baseValue;
        baseValue -= amount;
    }
    return apply;
}

/*#######################################
########                         ########
########   PLAYERS STAT SYSTEM   ########
########                         ########
#######################################*/

bool Player::UpdateStats(Stats stat)
{
    if (uint8(stat) >= MAX_STATS)
        return false;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = GetTotalStatValue(stat);

    SetStat(stat, int32(value));

    if (stat == STAT_STAMINA || stat == STAT_INTELLECT || stat == STAT_STRENGTH)
    {
        Pet* pet = GetPet();
        if (pet)
            pet->UpdateStats(stat);
    }

    switch (stat)
    {
        case STAT_AGILITY:
            UpdateDodgePercentage();
            UpdateAllCritPercentages();
            break;
        case STAT_STAMINA:
            UpdateMaxHealth();
            break;
        case STAT_INTELLECT:
            UpdateMaxPower(POWER_MANA);
            UpdateArmor();                                  //SPELL_AURA_MOD_RESISTANCE_OF_INTELLECT_PERCENT, only armor currently
            if (HasAuraType(SPELL_AURA_OVERRIDE_AP_BY_SPELL_POWER_PCT))
                UpdateAttackPowerAndDamage(true);
            break;
        case STAT_STRENGTH:
            if (getLevel() == MAX_LEVEL && (getClass() == CLASS_DEATH_KNIGHT || getClass() == CLASS_WARRIOR || getClass() == CLASS_PALADIN))
                UpdateParryPercentage();
            break;
        default:
            break;
    }

    if (stat == STAT_STRENGTH)
        UpdateAttackPowerAndDamage(false);
    else if (stat == STAT_AGILITY)
    {
        UpdateAttackPowerAndDamage(false);
        UpdateAttackPowerAndDamage(true);
    }

    UpdateSpellDamageAndHealingBonus();
    UpdateManaRegen();

    return true;
}

void Player::ApplySpellPowerBonus(int32 amount, bool apply)
{
    apply = _ModifyUInt32(apply, m_baseSpellPower, amount);

    if (HasAuraType(SPELL_AURA_OVERRIDE_AP_BY_SPELL_POWER_PCT))
    {
        UpdateAttackPowerAndDamage(false);
        UpdateAttackPowerAndDamage(true);
    }

    // For speed just update for client
    ApplyModUInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, amount, apply);
    for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, amount, apply);

    if (Pet* l_Pet = GetPet())
        l_Pet->UpdateAttackPowerAndDamage();
}

void Player::UpdateSpellDamageAndHealingBonus()
{
    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Get healing bonus for all schools
    SetStatInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_ALL));
    // Get damage bonus for all schools
    for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        SetStatInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS+i, SpellBaseDamageBonusDone(SpellSchoolMask(1 << i)));
}

bool Player::UpdateAllStats()
{
    for (int8 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        float value = GetTotalStatValue(Stats(i));
        SetStat(Stats(i), int32(value));
    }

    UpdateArmor();
    // calls UpdateAttackPowerAndDamage() in UpdateArmor for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
    UpdateAttackPowerAndDamage(true);
    UpdateMaxHealth();

    for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    UpdateAllRatings();
    UpdateAllCritPercentages();
    UpdateBlockPercentage();
    UpdateParryPercentage();
    UpdateDodgePercentage();
    UpdateSpellDamageAndHealingBonus();
    UpdateManaRegen();
    UpdateItemLevel();
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    return true;
}

void Player::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        float value  = GetTotalAuraModValue(UnitMods(UNIT_MOD_RESISTANCE_START + school));
        SetResistance(SpellSchools(school), int32(value));

        Pet* pet = GetPet();
        if (pet)
            pet->UpdateResistances(school);
    }
    else
        UpdateArmor();
}

void Player::UpdateArmor()
{
    float l_Armor = 0.0f;
    UnitMods l_UnitMod = UNIT_MOD_ARMOR;

    l_Armor  = GetModifierValue(l_UnitMod, BASE_VALUE);         // base armor (from items)
    l_Armor *= GetModifierValue(l_UnitMod, BASE_PCT);           // armor percent from items
    l_Armor += GetModifierValue(l_UnitMod, TOTAL_VALUE);
    l_Armor *= GetModifierValue(l_UnitMod, TOTAL_PCT);

    uint32 l_BonusArmor = GetModifierValue(UNIT_MOD_BONUS_ARMOR, BASE_VALUE);   ///< base armor (from items)
    l_BonusArmor *= GetModifierValue(UNIT_MOD_BONUS_ARMOR, BASE_PCT);           ///< armor percent from items
    l_BonusArmor += GetModifierValue(UNIT_MOD_BONUS_ARMOR, TOTAL_VALUE);
    l_BonusArmor *= GetModifierValue(UNIT_MOD_BONUS_ARMOR, TOTAL_PCT);
    SetUInt32Value(UNIT_FIELD_MOD_BONUS_ARMOR, l_BonusArmor);

    l_Armor += l_BonusArmor;

    //add dynamic flat mods
    AuraEffectList const& mResbyIntellect = GetAuraEffectsByType(SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT);
    for (AuraEffectList::const_iterator i = mResbyIntellect.begin(); i != mResbyIntellect.end(); ++i)
    {
        if ((*i)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
            l_Armor += CalculatePct(GetStat(Stats((*i)->GetMiscValueB())), (*i)->GetAmount());
    }

    SetArmor(int32(l_Armor));

    if (Pet* l_Pet = GetPet())
        l_Pet->UpdateArmor();

    UpdateAttackPowerAndDamage();                           // armor dependent auras update for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
}

float Player::GetHealthBonusFromStamina()
{
    HpPerStaTableEntry const* l_HpBase = g_HpPerStaTable.LookupEntry(getLevel());
    float l_Ratio = l_HpBase ? l_HpBase->m_HP : 10.0f;

    return GetStat(STAT_STAMINA) * l_Ratio;
}

void Player::UpdateMaxHealth()
{
    UnitMods unitMod = UNIT_MOD_HEALTH;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreateHealth();
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + GetHealthBonusFromStamina();
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxHealth((uint32)value);
}

void Player::UpdateMaxPower(Powers p_Power)
{
    if (GetPowerIndex(p_Power, getClass()) == MAX_POWERS)
        return;

    UnitMods l_UnitMod = UnitMods(UNIT_MOD_POWER_START + p_Power);

    float l_Value       = GetModifierValue(l_UnitMod, BASE_VALUE);
    float l_CreatePower = GetCreatePowers(p_Power);

    float l_Mod = 1.0f;

    if (p_Power == Powers::POWER_MANA)
    {
        AddPct(l_Mod, GetTotalAuraModifier(AuraType::SPELL_AURA_MODIFY_MANA_POOL_PCT));
    }

    AuraEffectList const& mModMaxPowerPct = GetAuraEffectsByType(AuraType::SPELL_AURA_MOD_MAX_POWER_PCT);
    for (AuraEffectList::const_iterator i = mModMaxPowerPct.begin(); i != mModMaxPowerPct.end(); ++i)
    {
        if (p_Power == (*i)->GetMiscValue())
            AddPct(l_Mod, (*i)->GetAmount());
    }

    l_CreatePower *= l_Mod;
    l_Value       += l_CreatePower;

    l_Value *= GetModifierValue(l_UnitMod, BASE_PCT);
    l_Value += GetModifierValue(l_UnitMod, TOTAL_VALUE);
    l_Value *= GetModifierValue(l_UnitMod, TOTAL_PCT);

    AuraEffectList const& mModMaxPower = GetAuraEffectsByType(SPELL_AURA_MOD_MAX_POWER);
    for (AuraEffectList::const_iterator i = mModMaxPower.begin(); i != mModMaxPower.end(); ++i)
    {
        if (p_Power == (*i)->GetMiscValue())
            l_Value += float((*i)->GetAmount());
    }

    int32 l_Cur_maxpower = GetMaxPower(p_Power);

    l_Value = floor(l_Value + 0.5f);
    if (l_Cur_maxpower != l_Value)
        SetMaxPower(p_Power, uint32(l_Value));

    AddDelayedEvent([this, p_Power]()->void
    {
        if (p_Power == POWER_MANA && GetActiveSpecializationID() == SPEC_MAGE_ARCANE)
            UpdateMasteryPercentage();
    }, 10);
}

void Player::UpdateItemLevel()
{
    SetFloatValue(PLAYER_FIELD_AVG_ITEM_LEVEL + PlayerAvgItemLevelOffsets::TotalAvgItemLevel, (float)GetAverageItemLevelTotal());
    SetFloatValue(PLAYER_FIELD_AVG_ITEM_LEVEL + PlayerAvgItemLevelOffsets::EquippedAvgItemLevel, (float)GetAverageItemLevelEquipped());
    SetFloatValue(PLAYER_FIELD_AVG_ITEM_LEVEL + PlayerAvgItemLevelOffsets::NonPvPAvgItemLevel, (float)GetAverageItemLevelTotalWithOrWithoutPvPBonus(false));
    SetFloatValue(PLAYER_FIELD_AVG_ITEM_LEVEL + PlayerAvgItemLevelOffsets::PvPAvgItemLevel, (float)GetAverageItemLevelTotalWithOrWithoutPvPBonus(true));
}

void Player::UpdateAttackPowerAndDamage(bool ranged)
{
    float val2 = 0.0f;

    ChrClassesEntry const* entry = sChrClassesStore.LookupEntry(getClass());
    UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

    uint16 index = UNIT_FIELD_ATTACK_POWER;
    uint16 index_mod_pos = UNIT_FIELD_ATTACK_POWER_MOD_POS;
    uint16 index_mod_neg = UNIT_FIELD_ATTACK_POWER_MOD_NEG;
    uint16 index_mult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

    if (ranged)
    {
        index = UNIT_FIELD_RANGED_ATTACK_POWER;
        index_mod_pos = UNIT_FIELD_RANGED_ATTACK_POWER_MOD_POS; ///< index_mod_pos is never read 01/18/16
        index_mod_neg = UNIT_FIELD_RANGED_ATTACK_POWER_MOD_NEG; ///< index_mod_neg is never read 01/18/16
        index_mult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;
        val2 = GetStat(STAT_AGILITY) *  entry->RangedAttackPowerPerAgility;
    }
    else
    {
        float strengthValue = GetStat(STAT_STRENGTH) * entry->AttackPowerPerStrength;
        float agilityValue = GetStat(STAT_AGILITY) * entry->AttackPowerPerAgility;

        val2 = strengthValue + agilityValue;
    }

    SetModifierValue(unitMod, BASE_VALUE, val2);

    float base_attPower = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMod = GetModifierValue(unitMod, TOTAL_VALUE);

    float attPowerMultiplier = 1.0f;
    if (ranged)
        attPowerMultiplier = GetTotalAuraMultiplier(SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT) - 1.0f;
    else
        attPowerMultiplier = GetTotalAuraMultiplier(SPELL_AURA_MOD_ATTACK_POWER_PCT) - 1.0f;

    //add dynamic flat mods
    if (!ranged && HasAuraType(SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR))
    {
        AuraEffectList const& mAPbyArmor = GetAuraEffectsByType(SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR);
        for (AuraEffectList::const_iterator iter = mAPbyArmor.begin(); iter != mAPbyArmor.end(); ++iter)
        {
            // always: ((*i)->GetModifier()->m_miscvalue == 1 == SPELL_SCHOOL_MASK_NORMAL)
            int32 temp = int32(GetArmor() / (*iter)->GetAmount());
            if (temp > 0)
                attPowerMod += temp;
            else
                attPowerMod -= temp;
        }
    }

    attPowerMod += CalculatePct(GetUInt32Value(UNIT_FIELD_MOD_BONUS_ARMOR), GetTotalAuraModifier(SPELL_AURA_MOD_AP_FROM_BONUS_ARMOR_PCT));

    if (HasAuraType(SPELL_AURA_OVERRIDE_AP_BY_SPELL_POWER_PCT))
    {
        int32 ApBySpellPct = 0;
        int32 spellPower = GetBaseSpellPowerBonus(); // SpellPower from Weapon
        spellPower += std::max(0, int32(GetStat(STAT_INTELLECT)) - 10); // SpellPower from intellect

        AuraEffectList const& mAPFromSpellPowerPct = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_AP_BY_SPELL_POWER_PCT);
        for (AuraEffectList::const_iterator i = mAPFromSpellPowerPct.begin(); i != mAPFromSpellPowerPct.end(); ++i)
            ApBySpellPct += CalculatePct(spellPower, (*i)->GetAmount());

        if (ApBySpellPct > 0)
        {
            SetInt32Value(index, uint32(ApBySpellPct));     //UNIT_FIELD_(RANGED)_ATTACK_POWER field
            SetFloatValue(index_mult, attPowerMultiplier);  //UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field
        }
        else
        {
            SetInt32Value(index, uint32(base_attPower + attPowerMod));  //UNIT_FIELD_(RANGED)_ATTACK_POWER field
            SetFloatValue(index_mult, attPowerMultiplier);              //UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field
        }
    }
    else
    {
        SetInt32Value(index, uint32(base_attPower + attPowerMod));  //UNIT_FIELD_(RANGED)_ATTACK_POWER field
        SetFloatValue(index_mult, attPowerMultiplier);              //UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field
    }

    Pet* pet = GetPet();                                //update pet's AP
    //automatically update weapon damage after attack power modification
    if (ranged)
    {
        UpdateDamagePhysical(WeaponAttackType::RangedAttack);
        if (pet && pet->isHunterPet()) // At ranged attack change for hunter pet
            pet->UpdateAttackPowerAndDamage();
    }
    else
    {
        UpdateDamagePhysical(WeaponAttackType::BaseAttack);
        if (Item* offhand = GetWeaponForAttack(WeaponAttackType::OffAttack, true))
        {
            if (CanDualWield() || offhand->GetTemplate()->Flags3 & int32(ItemFlagsEX2::ALWAYS_ALLOW_DUAL_WIELD))
                UpdateDamagePhysical(WeaponAttackType::OffAttack);
        }
        if (getClass() == CLASS_SHAMAN || getClass() == CLASS_PALADIN)                      // mental quickness
            UpdateSpellDamageAndHealingBonus();

        if (pet && pet->IsPetGhoulOrAbomination()) // At ranged attack change for hunter pet
            pet->UpdateAttackPowerAndDamage();
    }
}

void Player::CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& min_damage, float& max_damage, bool l_NoLongerDualWields)
{
    Item* mainItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    Item* l_OffHandItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    Item* l_UsedWeapon = attType == WeaponAttackType::OffAttack ? l_OffHandItem : mainItem;

    UnitMods unitMod;

    switch (attType)
    {
        case WeaponAttackType::BaseAttack:
        default:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case WeaponAttackType::OffAttack:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case WeaponAttackType::RangedAttack:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
    }

    float att_speed = (l_UsedWeapon ? l_UsedWeapon->GetTemplate()->Delay : BASE_ATTACK_TIME) / 1000.0f;
    float attackPower = GetTotalAttackPowerValue(attType);

    /// If player doesn't have weapons we should calculate damage with this values: min damage = 1 and max damage = 2
    float weapon_mindamage = GetBaseWeaponDamageRange(attType, MINDAMAGE) == 0 ? 1.0f : GetBaseWeaponDamageRange(attType, MINDAMAGE);
    float weapon_maxdamage = GetBaseWeaponDamageRange(attType, MAXDAMAGE) == 0 ? 2.0f : GetBaseWeaponDamageRange(attType, MAXDAMAGE);

    bool dualWield = mainItem && l_OffHandItem && !l_NoLongerDualWields;
    float dualWieldModifier = 1.0f; ///< I don't know about reducing for 19%. We have checked it on PTR, and damage is the same with dual wield - like without it.
    if (dualWield && HasAuraType(SPELL_AURA_INCREASE_DUAL_WIELD_DAMAGE))
        dualWieldModifier += (float)GetTotalAuraModifier(SPELL_AURA_INCREASE_DUAL_WIELD_DAMAGE) / 100.0f;

    if (IsInFeralForm() && HasAura((uint32)PvPStatsSpells::SPEC_STAT_TEMPLATE_ALL_SPECS))
    {
        if (GetShapeshiftForm() == FORM_CAT_FORM)
            att_speed = 1.0f;

        if (GetShapeshiftForm() == FORM_BEAR_FORM)
            att_speed = 2.5f;
    }

    float weapon_normalized_min = weapon_mindamage + attackPower / 3.5f * att_speed * dualWieldModifier;
    float weapon_normalized_max = weapon_maxdamage + attackPower / 3.5f * att_speed * dualWieldModifier;

    if (IsInFeralForm() && !HasAura((uint32)PvPStatsSpells::SPEC_STAT_TEMPLATE_ALL_SPECS))
    {
        float weaponSpeed = BASE_ATTACK_TIME / 1000.0f;
        if (mainItem && mainItem->GetTemplate()->Class == ITEM_CLASS_WEAPON)
            weaponSpeed = float(mainItem->GetTemplate()->Delay) / 1000.0f;

        if (GetShapeshiftForm() == FORM_CAT_FORM)
        {
            /// Special cases for Cat form
            if (weapon_mindamage == 1)
                weapon_mindamage = 2;
            if (weapon_maxdamage == 2)
                weapon_maxdamage = 3;

            weapon_normalized_min = ((weapon_mindamage / weaponSpeed) + (attackPower / 3.5f));
            weapon_normalized_max = ((weapon_maxdamage / weaponSpeed) + (attackPower / 3.5f));
        }
        else if (GetShapeshiftForm() == FORM_BEAR_FORM)
        {
            weapon_normalized_min = ((weapon_mindamage / weaponSpeed) + (attackPower / 3.5f)) * 2.5f;
            weapon_normalized_max = ((weapon_maxdamage / weaponSpeed) + (attackPower / 3.5f)) * 2.5f;
        }
    }

    float base_value = GetModifierValue(unitMod, BASE_VALUE);
    float base_pct = GetModifierValue(unitMod, BASE_PCT);
    float total_value = GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct = addTotalPct ? GetModifierValue(unitMod, TOTAL_PCT) : 1.0f;

    /// Normalized Weapon Damage
    if (normalized && getClass() != CLASS_MONK && getClass() != CLASS_DRUID) ///< Monks and Druids have their own damage calculation
    {
        CalculateNormalizedWeaponDamage(attType, min_damage, max_damage, attackPower, weapon_mindamage, weapon_maxdamage, l_UsedWeapon);
        min_damage = (min_damage * base_pct + total_value) * total_pct;
        max_damage = (max_damage * base_pct + total_value) * total_pct;
    }
    /// Damage based on auto-attack
    else
    {
        min_damage = ((base_value + weapon_normalized_min) * base_pct + total_value) * total_pct;
        max_damage = ((base_value + weapon_normalized_max) * base_pct + total_value) * total_pct;

        int32 autoAttacksPctBonus = GetTotalAuraModifier(SPELL_AURA_MOD_AUTOATTACK_DAMAGE);
        AddPct(min_damage, autoAttacksPctBonus);
        AddPct(max_damage, autoAttacksPctBonus);
    }
}

void Player::CalculateNormalizedWeaponDamage(WeaponAttackType /*attType*/, float& min_damage, float& max_damage, float attackPower, float weapon_mindamage, float weapon_maxdamage, Item* l_UsedWeapon)
{
    float l_NormalizedSpeedCoef = 1.0f;

    /// Speed coefficients from http://wowwiki.wikia.com/Normalization - tested on official server, information is correct
    if (l_UsedWeapon && l_UsedWeapon->GetTemplate())
    {
        if (l_UsedWeapon->GetTemplate()->IsRangedWeapon())
            l_NormalizedSpeedCoef = 2.8f;
        else if (l_UsedWeapon->GetTemplate()->IsOneHanded())
        {
            if (l_UsedWeapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                l_NormalizedSpeedCoef = 1.7f;
            else
                l_NormalizedSpeedCoef = 2.4f;
        }
        else if (l_UsedWeapon->GetTemplate()->IsTwoHandedWeapon())
            l_NormalizedSpeedCoef = 3.3f;
    }

    min_damage = weapon_mindamage + (attackPower / 3.5f * l_NormalizedSpeedCoef);
    max_damage = weapon_maxdamage + (attackPower / 3.5f * l_NormalizedSpeedCoef);
}

void Player::UpdateDamagePhysical(WeaponAttackType attType, bool l_NoLongerDualWields)
{
    float l_MinDamage;
    float l_MaxDamage;

    CalculateMinMaxDamage(attType, false, true, l_MinDamage, l_MaxDamage, l_NoLongerDualWields);

    float l_MinDamageBonus = MeleeDamageBonusDone(nullptr, l_MinDamage, attType);
    float l_MaxDamageBonus = MeleeDamageBonusDone(nullptr, l_MaxDamage, attType);

    switch (attType)
    {
        case WeaponAttackType::BaseAttack:
        default:
            m_WeaponDamage[WeaponAttackType::BaseAttack][MINDAMAGE] = l_MinDamage;
            m_WeaponDamage[WeaponAttackType::BaseAttack][MAXDAMAGE] = l_MaxDamage;

            SetStatFloatValue(UNIT_FIELD_MIN_DAMAGE, l_MinDamageBonus);
            SetStatFloatValue(UNIT_FIELD_MAX_DAMAGE, l_MaxDamageBonus);
            break;
        case WeaponAttackType::OffAttack:
            m_WeaponDamage[WeaponAttackType::OffAttack][MINDAMAGE] = l_MinDamage;
            m_WeaponDamage[WeaponAttackType::OffAttack][MAXDAMAGE] = l_MaxDamage;

            SetStatFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE, l_MinDamageBonus);
            SetStatFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE, l_MaxDamageBonus);
            break;
        case WeaponAttackType::RangedAttack:
            m_WeaponDamage[WeaponAttackType::RangedAttack][MINDAMAGE] = l_MinDamage;
            m_WeaponDamage[WeaponAttackType::RangedAttack][MAXDAMAGE] = l_MaxDamage;

            SetStatFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE, l_MinDamageBonus);
            SetStatFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE, l_MaxDamageBonus);
            break;
    }
}

void Player::UpdateCritPercentage(WeaponAttackType attType)
{
    BaseModGroup modGroup;
    uint16 index;
    CombatRating cr;

    switch (attType)
    {
        case WeaponAttackType::OffAttack:
            modGroup = OFFHAND_CRIT_PERCENTAGE;
            index = PLAYER_FIELD_OFFHAND_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
        case WeaponAttackType::RangedAttack:
            modGroup = RANGED_CRIT_PERCENTAGE;
            index = PLAYER_FIELD_RANGED_CRIT_PERCENTAGE;
            cr = CR_CRIT_RANGED;
            break;
        case WeaponAttackType::BaseAttack:
        default:
            modGroup = CRIT_PERCENTAGE;
            index = PLAYER_FIELD_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
    }

    const float l_Crit = 5.0f;
    /// -1.0f because it's already calculated in GetTotalPercentageModValue
    float value = l_Crit - 1.0f + GetTotalPercentageModValue(modGroup) + GetRatingBonusValue(cr);
    // Modify crit from weapon skill and maximized defense skill of same level victim difference
    if (sWorld->getBoolConfig(CONFIG_STATS_LIMITS_ENABLE))
        value = value > sWorld->getFloatConfig(CONFIG_STATS_LIMITS_CRIT) ? sWorld->getFloatConfig(CONFIG_STATS_LIMITS_CRIT) : value;

    value = value < 0.0f ? 0.0f : value;
    SetStatFloatValue(index, value);

    if (HasAuraType(AuraType::SPELL_AURA_CONVERT_CRIT_RATING_PCT_TO_PARRY_RATING))
        UpdateParryPercentage();
}

void Player::UpdateAllCritPercentages()
{
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateSpellCritChance(i);
}

static float m_diminishing_k[MAX_CLASSES] =
{
    0.9560f,  /// Warrior
    0.8860f,  /// Paladin
    0.9880f,  /// Hunter
    0.9880f,  /// Rogue
    0.9560f,  /// Priest
    0.9560f,  /// DK
    0.9880f,  /// Shaman
    0.9830f,  /// Mage
    0.9830f,  /// Warlock
    0.9830f,  /// Monk
    0.9720f,  /// Druid
    0.9830f   /// DemonHunter
};

void Player::UpdateParryPercentage()
{
    const float l_Parry_cap[MAX_CLASSES] =
    {
        65.631440f,     /// Warrior
        65.631440f,     /// Paladin
        0.0f,           /// Hunter
        145.560408f,    /// Rogue
        0.0f,           /// Priest
        65.631440f,     /// DK
        145.560408f,    /// Shaman
        0.0f,           /// Mage
        0.0f,           /// Warlock
        90.6425f,     /// Monk
        0.0f,           /// Druid
        65.631440f,    /// DemonHunter
    };

    const bool l_MainStr[MAX_CLASSES] =
    {
        true,  /// Warrior
        true,  /// Paladin
        false, /// Hunter
        false, /// Rogue
        false, /// Priest
        true,  /// DK
        false, /// Shaman
        false, /// Mage
        false, /// Warlock
        false, /// Monk
        false, /// Druid
        false, /// DemonHunter
    };

    float l_Total = 0.0f;
    uint32 l_PClass = getClass() - 1;

    if (CanParry() && l_Parry_cap[l_PClass] > 0.0f)
    {
        float l_BaseParry = 3.0f;

        l_BaseParry += GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT) + GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT_2);
        float l_RatingParry = GetRatingBonusValue(CR_PARRY);
        float l_Scaling = 1331.2f; /// 7.1.5 - Build 23420: changed value from 951 to match retail
        float l_Base_strength = GetCreateStat(STAT_STRENGTH) * m_auraModifiersGroup[UNIT_MOD_STAT_START + STAT_STRENGTH][BASE_PCT];
        float l_Total_strength = GetModifierValue(UNIT_MOD_STAT_STRENGTH, BASE_VALUE);

        float l_ParryFromStrength = l_MainStr[l_PClass] ? (l_Total_strength / l_Scaling) : 0.0f;

        float l_ParryFromCrit = 0.f;
        if (int32 l_CritToParryMod = GetTotalAuraModifier(SPELL_AURA_CONVERT_CRIT_RATING_PCT_TO_PARRY_RATING))
        {
            float l_CritRat = float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_CRIT_MELEE)) * GetRatingMultiplier(CR_PARRY);
            l_ParryFromCrit += l_CritRat * ((float)l_CritToParryMod / 100.f);
        }

        l_Total = pow((1 / l_Parry_cap[l_PClass]) + (m_diminishing_k[l_PClass] / (l_ParryFromStrength + l_RatingParry)), -1);
        l_Total += l_BaseParry;
        l_Total += l_ParryFromCrit;
        l_Total = l_Total < 0.0f ? 0.0f : l_Total;
    }

    SetStatFloatValue(PLAYER_FIELD_PARRY_PERCENTAGE, l_Total);
}

void Player::UpdateDodgePercentage()
{
    const float l_Dodge_cap[MAX_CLASSES] =
    {
        90.6425f,       /// Warrior
        66.5674f,       /// Paladin
        145.560408f,    /// Hunter
        145.560408f,    /// Rogue
        66.5674f,       /// Priest
        90.6425f,       /// DK
        145.560408f,    /// Shaman
        150.375940f,    /// Mage
        150.375940f,    /// Warlock
        145.560408f,    /// Monk
        116.890707f,    /// Druid
        145.560408f,    /// DemonHunter
    };

    const bool l_MainAgi[MAX_CLASSES] =
    {
        false, /// Warrior
        false, /// Paladin
        true,  /// Hunter
        true,  /// Rogue
        false, /// Priest
        false, /// DK
        true,  /// Shaman
        false, /// Mage
        false, /// Warlock
        true,  /// Monk  @todo: find me !
        true,  /// Druid
        true,  /// DemonHunter
    };

    uint32 l_PClass = getClass() - 1;

    /// Dodge from rating
    float l_BaseDodge = 3.0f;
    float l_Total = 0.0f;

    l_BaseDodge += GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT);
    float l_Scaling = 1331.2f; /// 7.1.5 - Build 23420: changed value from 951 to match retail
    float l_Base_agility = GetCreateStat(STAT_AGILITY) * m_auraModifiersGroup[UNIT_MOD_STAT_START + STAT_AGILITY][BASE_PCT];
    float l_RatingDodge = GetRatingBonusValue(CR_DODGE);
    float l_Total_agility = GetTotalStatValue(STAT_AGILITY);

    float l_DodgeFromAgility = 0.0f;
    if (GetPrimaryStat() == Stats::STAT_AGILITY) ///< Dodgerating doesn't scale with gear for specs who don't use agility as main stat. It stays at 3%, always.
        l_DodgeFromAgility = ((l_Total_agility - l_Base_agility) / l_Scaling);

    l_Total = pow((1 / l_Dodge_cap[l_PClass]) + (m_diminishing_k[l_PClass] / (l_DodgeFromAgility + l_RatingDodge)), -1);
    l_Total += l_BaseDodge;
    l_Total = l_Total < 0.0f ? 0.0f : l_Total;

    if (sWorld->getBoolConfig(CONFIG_STATS_LIMITS_ENABLE))
        l_Total = l_Total > sWorld->getFloatConfig(CONFIG_STATS_LIMITS_DODGE) ? sWorld->getFloatConfig(CONFIG_STATS_LIMITS_DODGE) : l_Total;

    if (GetActiveSpecializationID() == SPEC_DRUID_GUARDIAN && HasAura(231065)) ///< Guardian druid passive which turns any bonus crit into dodge
        l_Total += ((GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE) - 10.0) * 400 / 515); ///< guardian druids have 10% base crit and crit rating is 400, when dodge one is 515. THIS IS PIG but i had no time to make it properly

    SetStatFloatValue(PLAYER_FIELD_DODGE_PERCENTAGE, l_Total);
}

void Player::UpdateBlockPercentage()
{
    /// No block
    float l_Value = 0.0f;
    if (CanBlock())
    {
        /// Base value
        l_Value = 3.0f;
        if (GetActiveSpecializationID() == SPEC_PALADIN_PROTECTION)
            l_Value -= 1.38f; ///< Hacky ?

        /// Increase from SPELL_AURA_MOD_BLOCK_PERCENT aura
        l_Value += GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);

        /// Increase from rating
        l_Value += GetRatingBonusValue(CR_BLOCK);
        l_Value = l_Value < 0.0f ? 0.0f : l_Value;
    }
    SetStatFloatValue(PLAYER_FIELD_BLOCK_PERCENTAGE, l_Value);
}

void Player::UpdateSpellCritChance(uint32 p_School)
{
    // For normal school set zero crit chance
    if (p_School == SPELL_SCHOOL_NORMAL)
    {
        SetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE, 0.0f);
        return;
    }

    float l_Crit = 5.0f;
    l_Crit += GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
    l_Crit += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
    l_Crit += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL, 1 << p_School);
    l_Crit += GetRatingBonusValue(CR_CRIT_SPELL);

    SetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE, l_Crit);

    if (Pet* l_Pet = GetPet())
        l_Pet->m_baseSpellCritChance = l_Crit;
}

void Player::UpdateMeleeHitChances()
{
    m_modMeleeHitChance = (float)GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
    SetFloatValue(PLAYER_FIELD_UI_HIT_MODIFIER, m_modMeleeHitChance);

    m_modMeleeHitChance += GetRatingBonusValue(CR_HIT_MELEE);
}

void Player::UpdateRangedHitChances()
{
    m_modRangedHitChance = (float)GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
    m_modRangedHitChance += GetRatingBonusValue(CR_HIT_RANGED);
}

void Player::UpdateSpellHitChances()
{
    m_modSpellHitChance = (float)GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE);
    SetFloatValue(PLAYER_FIELD_UI_SPELL_HIT_MODIFIER, m_modSpellHitChance);

    m_modSpellHitChance += GetRatingBonusValue(CR_HIT_SPELL);
}

void Player::UpdateMasteryPercentage()
{
    /// No mastery
    float l_Value = 0.0f;
    if (CanMastery() && getLevel() >= 80)
    {
        /// Mastery from SPELL_AURA_MASTERY aura
        l_Value += GetTotalAuraModifier(SPELL_AURA_MASTERY);
        float l_Modifier = 0;

        /// Mastery from rating
        float l_RatingValue = GetRatingBonusValue(CombatRating::CR_MASTERY);

        l_Value += l_RatingValue;

        l_Value = l_Value < 0.0f ? 0.0f : l_Value;
    }
    SetFloatValue(PLAYER_FIELD_MASTERY, l_Value);

    bool l_MasteryCache = false;

    /// Update some mastery spells
    AuraApplicationMap& l_AppliedAuras = GetAppliedAuras();
    for (auto l_Iter : l_AppliedAuras)
    {
        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Iter.first);
        if (l_SpellInfo != nullptr && l_SpellInfo->HasAttribute(SpellAttr8::SPELL_ATTR8_MASTERY_SPECIALIZATION))
        {
            Aura* l_Aura = l_Iter.second->GetBase();
            for (uint8 l_I = 0; l_I < l_Aura->GetEffectCount(); ++l_I)
            {
                if (AuraEffect* l_AurEff = l_Aura->GetEffect(l_I))
                {
                    l_AurEff->SetCanBeRecalculated(true);
                    float l_floatAmount = (l_Value * l_SpellInfo->Effects[l_I].BonusMultiplier);
                    if (l_AurEff->GetAuraType() == SPELL_AURA_MOD_BLOCK_PERCENT) ///< Paladin prot & Warrior prot block from mastery has diminishing return https://www.mmo-champion.com/threads/2192678-Parry-block-Diminishing-returns-formula
                        HandleBlockMastery(l_AurEff, l_Value);
                    else
                        l_AurEff->ChangeAmount((int32) l_floatAmount, true, true, false, l_floatAmount);

                    if (!l_MasteryCache)
                    {
                        m_MasteryCache = l_Value * l_SpellInfo->Effects[l_I].BonusMultiplier;
                        l_MasteryCache = true;
                    }
                }
            }
        }
    }
}

void Player::UpdateLeechPercentage()
{
    float value = GetTotalAuraModifier(SPELL_AURA_MOD_LEECH_PCT);
    value += GetRatingBonusValue(CR_LIFESTEAL);
    SetFloatValue(PLAYER_FIELD_LIFESTEAL, value);
}

void Player::UpdateVersatilityPercentage()
{
    float valueBonus = GetTotalAuraModifier(SPELL_AURA_MOD_VERSATILITY_PCT);

    float valueDone = GetRatingBonusValue(CR_VERSATILITY_DAMAGE_DONE);

    SetFloatValue(PLAYER_FIELD_VERSATILITY, valueDone);
    SetFloatValue(PLAYER_FIELD_VERSATILITY_BONUS, valueBonus);

    /// Update damage after applying versatility rating
    if (getClass() == CLASS_HUNTER)
        UpdateDamagePhysical(WeaponAttackType::RangedAttack);
    else
        UpdateDamagePhysical(WeaponAttackType::BaseAttack);
}

void Player::UpdateAvoidancePercentage()
{
    float value = 0.0f;//GetTotalAuraModifier(SPELL_AURA);
    value += GetRatingBonusValue(CR_AVOIDANCE);
    SetFloatValue(PLAYER_FIELD_AVOIDANCE, value);
}

void Player::UpdateSpeedPercentage()
{
    float value = 0.0f;
    value += GetRatingBonusValue(CR_SPEED);
    SetFloatValue(PLAYER_FIELD_SPEED, value);
}

void Player::HandleBlockMastery(AuraEffect* p_AurEff, float p_Amount)
{
    p_Amount /= (0.018f * p_Amount + (1.0f / 0.94f));
    p_AurEff->ChangeAmount((int32)p_Amount, true, true, false, p_Amount);
    UpdateBlockPercentage();
}

void Player::ApplyManaRegenBonus(int32 amount, bool apply)
{
    _ModifyUInt32(apply, m_baseManaRegen, amount);
    UpdateManaRegen();
}

void Player::ApplyHealthRegenBonus(int32 amount, bool apply)
{
    _ModifyUInt32(apply, m_baseHealthRegen, amount);
}

void Player::UpdateManaRegen()
{
    if (GetPowerIndexByClass(getClass(), POWER_MANA) == Powers::MAX_POWERS && !IsInFeralForm())
        return;

    /// See http://us.battle.net/wow/en/forum/topic/6794873160 .

    float l_PercentOfMana = 2.0f;

    float l_CombatRegenFromSpirit = 0;
    float l_CombatRegenFromAurPct = 0;
    float l_BaseRegenFromAurPct = 0;
    float l_RegenFromModPowerRegen = /*GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, POWER_MANA) / 5.0f*/ 0.0f;
    {
        std::map<SpellGroup, int32> l_SameEffectSpellGroup;

        AuraEffectList const& l_List = GetAuraEffectsByType(SPELL_AURA_MOD_POWER_REGEN);
        for (auto const& l_Eff : l_List)
        {
            if (l_Eff->GetMiscValue() == POWER_MANA)
            {
                if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(l_Eff->GetSpellInfo(), l_Eff->GetAmount(), l_SameEffectSpellGroup))
                {
                    auto l_Base = l_Eff->GetBase();
                    l_RegenFromModPowerRegen += l_Eff->GetAmount();
                }
            }
        }

        for (auto const& l_Kvp : l_SameEffectSpellGroup)
        {
            l_RegenFromModPowerRegen += l_Kvp.second / 5.0f;
        }
    }

    switch (getClass())
    {
        case Classes::CLASS_WARLOCK:
        case Classes::CLASS_MAGE:
        {
            l_PercentOfMana = 5.0f;
        }
        default:
        {
            switch (GetActiveSpecializationID())
            {
                case SpecIndex::SPEC_MONK_MISTWEAVER:
                case SpecIndex::SPEC_PRIEST_DISCIPLINE:
                case SpecIndex::SPEC_PRIEST_HOLY:
                {
                    l_PercentOfMana = 4.0f;
                    break;
                }
                case SpecIndex::SPEC_PRIEST_SHADOW:
                {
                    l_PercentOfMana = 16.0f;
                    break;
                }
                case SpecIndex::SPEC_PALADIN_PROTECTION:
                case SpecIndex::SPEC_PALADIN_RETRIBUTION:
                case SpecIndex::SPEC_SHAMAN_ENHANCEMENT:
                case SpecIndex::SPEC_SHAMAN_ELEMENTAL:
                case SpecIndex::SPEC_DRUID_BALANCE:
                {
                    l_PercentOfMana = 5.0f;
                    break;
                }
                case SpecIndex::SPEC_PALADIN_HOLY:
                case SpecIndex::SPEC_DRUID_RESTORATION:
                case SpecIndex::SPEC_SHAMAN_RESTORATION:
                {
                    l_PercentOfMana = 1.0f;
                    break;
                }
                case SpecIndex::SPEC_MONK_BREWMASTER:
                case SpecIndex::SPEC_MONK_WINDWALKER:
                {
                    l_PercentOfMana = 1.5625f;
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }

    /// 2% of base mana each 5 seconds.
    float l_Combat_regen = float(CalculatePct(GetMaxPower(POWER_MANA), l_PercentOfMana));
    float l_Base_regen = float(CalculatePct((float)GetMaxPower(POWER_MANA), (float)l_PercentOfMana));

    if (HasAuraType(SPELL_AURA_MOD_MANA_REGEN_BY_HASTE))
    {
        float l_HastePct = 1.0f + GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_HASTE_MELEE) * GetRatingMultiplier(CR_HASTE_MELEE) / 100.0f;

        l_Combat_regen *= l_HastePct;
        l_Base_regen *= l_HastePct;
    }

    /// Try to get aura with spirit addition to combat mana regen.
    /// Meditation: Allows 50% of your mana regeneration from Spirit to continue while in combat.
    int32 l_PercentAllowCombatRegenBySpirit = 0;
    Unit::AuraEffectList const& ModPowerRegenPCTAuras = GetAuraEffectsByType(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);
    for (AuraEffectList::const_iterator i = ModPowerRegenPCTAuras.begin(); i != ModPowerRegenPCTAuras.end(); ++i)
        l_PercentAllowCombatRegenBySpirit += (*i)->GetAmount();

    /// Increase mana regen.
    int32 l_IncreaseManaRegen = l_Combat_regen;
    int32 l_BonusManaRegen = 0;

    /// Increase mana from SPELL_AURA_MOD_POWER_REGEN_PERCENT
    Unit::AuraEffectList const& ModRegenPct = GetAuraEffectsByType(SPELL_AURA_MOD_POWER_REGEN_PERCENT);
    for (AuraEffectList::const_iterator i = ModRegenPct.begin(); i != ModRegenPct.end(); ++i)
        if (Powers((*i)->GetMiscValue()) == POWER_MANA)
            l_IncreaseManaRegen += l_Combat_regen * ((*i)->GetAmount() / 100.0f);

    /// Increase mana from SPELL_AURA_MODIFY_MANA_REGEN_FROM_MANA_PCT
    Unit::AuraEffectList const& ModRegenPctUnk = GetAuraEffectsByType(SPELL_AURA_MODIFY_MANA_REGEN_FROM_MANA_PCT);
    for (AuraEffectList::const_iterator i = ModRegenPctUnk.begin(); i != ModRegenPctUnk.end(); ++i)
    {
        if (!(*i)->GetSpellInfo()->HasAura(SPELL_AURA_MODIFY_MANA_POOL_PCT))
            l_IncreaseManaRegen += l_IncreaseManaRegen * ((*i)->GetAmount() / 100.0f);
    }

    /// If IncreaseManaRegen is bigger then combat_regen we have increased mana regen by auras, so we should add it
    if ((HasAuraType(SPELL_AURA_MOD_POWER_REGEN_PERCENT) || HasAuraType(SPELL_AURA_MOD_MANA_REGEN_FROM_STAT) || HasAuraType(SPELL_AURA_MODIFY_MANA_REGEN_FROM_MANA_PCT)) && l_IncreaseManaRegen > l_Combat_regen)
    {
        l_IncreaseManaRegen -= l_Combat_regen;
        l_BaseRegenFromAurPct = l_IncreaseManaRegen;
        l_CombatRegenFromAurPct = l_IncreaseManaRegen;
    }

    /// Calculate for 1 second, the client multiplies the field values by 5.
    l_Base_regen = ((l_Base_regen + l_BaseRegenFromAurPct + l_RegenFromModPowerRegen + l_BonusManaRegen) / 5.0f);
    l_Combat_regen = ((l_Combat_regen + l_CombatRegenFromAurPct + l_RegenFromModPowerRegen + l_BonusManaRegen) / 5.0f);

    l_Base_regen /= GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);
    l_Combat_regen /= GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);

    /// Out of Combat
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, HasAura(211336) ? 0 : l_Base_regen);
    /// In Combat
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, HasAura(211336) ? 0 : l_Combat_regen);
}

void Player::UpdateEnergyRegen()
{
    if (getPowerType() != Powers::POWER_ENERGY)
        return;

    uint32 l_PowerIndex = GetPowerIndex(Powers::POWER_ENERGY, getClass());

    SetFloatValue(EUnitFields::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + l_PowerIndex, GetRegenForPower(Powers::POWER_ENERGY));
    SetFloatValue(EUnitFields::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + l_PowerIndex, GetRegenForPower(Powers::POWER_ENERGY));
}

void Player::UpdateFocusRegen()
{
    if (getPowerType() != Powers::POWER_FOCUS)
        return;

    SetFloatValue(EUnitFields::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, GetRegenForPower(Powers::POWER_FOCUS));
    SetFloatValue(EUnitFields::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, GetRegenForPower(Powers::POWER_FOCUS));
}

void Player::UpdateAllRunesRegen()
{
    if (getClass() != CLASS_DEATH_KNIGHT)
        return;

    uint32 runeIndex = GetPowerIndex(POWER_RUNES, CLASS_DEATH_KNIGHT);
    if (runeIndex == MAX_POWERS)
        return;

    uint32 cooldown = GetRuneBaseCooldown();
    cooldown /= GetTotalAuraMultiplierByMiscValue(SPELL_AURA_MOD_POWER_REGEN_PERCENT, POWER_RUNES);
    cooldown *= GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);
    float l_Mod = (1.f / ((float)cooldown / IN_MILLISECONDS)) - 0.1f;
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + runeIndex, l_Mod);
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + runeIndex, l_Mod);
}

float Player::GetRegenForPower(Powers p_Power)
{
    if (p_Power != Powers::POWER_ENERGY && p_Power != Powers::POWER_FOCUS)
        return 0.0f;

    PowerTypeEntry const* l_PowerType = sPowerTypeStore.LookupEntry(p_Power);
    if (!l_PowerType)
        return 0.0f;

    float l_BaseRegen   = l_PowerType->RegenerationPeace;
    float l_Total       = l_BaseRegen / GetFloatValue(UNIT_FIELD_MOD_HASTE);

    Unit::AuraEffectList const& l_ModPowerRegenPCT = GetAuraEffectsByType(AuraType::SPELL_AURA_MOD_POWER_REGEN_PERCENT);
    for (Unit::AuraEffectList::const_iterator l_Iter = l_ModPowerRegenPCT.begin(); l_Iter != l_ModPowerRegenPCT.end(); ++l_Iter)
        if (Powers((*l_Iter)->GetMiscValue()) == p_Power)
            AddPct(l_Total, (*l_Iter)->GetAmount());

    /// Need more info for other powers
    if (p_Power == Powers::POWER_FOCUS || p_Power == Powers::POWER_ENERGY)
    {
        Unit::AuraEffectList const& l_ModPowerRegen = GetAuraEffectsByType(AuraType::SPELL_AURA_MOD_POWER_REGEN);
        for (Unit::AuraEffectList::const_iterator l_Iter = l_ModPowerRegen.begin(); l_Iter != l_ModPowerRegen.end(); ++l_Iter)
            if (Powers((*l_Iter)->GetMiscValue()) == p_Power)
                l_Total += float((*l_Iter)->GetAmount() / 5);
    }

    l_Total = l_Total / GetFloatValue(UNIT_FIELD_MOD_TIME_RATE) - l_BaseRegen;

    return l_Total;
}

void Player::_ApplyAllStatBonuses()
{
    SetCanModifyStats(false);

    _ApplyAllAuraStatMods();
    _ApplyAllItemMods();

    SetCanModifyStats(true);

    UpdateAllStats();
}

void Player::_RemoveAllStatBonuses()
{
    SetCanModifyStats(false);

    _RemoveAllItemMods();
    _RemoveAllAuraStatMods();

    SetCanModifyStats(true);

    UpdateAllStats();
}

/*#######################################
########                         ########
########    MOBS STAT SYSTEM     ########
########                         ########
#######################################*/

bool Creature::UpdateStats(Stats /*stat*/)
{
    return true;
}

bool Creature::UpdateAllStats()
{
    UpdateMaxHealth();
    UpdateAttackPowerAndDamage();
    UpdateAttackPowerAndDamage(true);

    for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    for (int8 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    return true;
}

void Creature::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        float value  = GetTotalAuraModValue(UnitMods(UNIT_MOD_RESISTANCE_START + school));
        SetResistance(SpellSchools(school), int32(value));
    }
    else
        UpdateArmor();
}

void Creature::UpdateArmor()
{
    float value = GetTotalAuraModValue(UNIT_MOD_ARMOR);
    SetArmor(int32(value));
}

void Creature::UpdateMaxHealth()
{
    double value        = double(GetTotalAuraModValue(UNIT_MOD_HEALTH));
    float mod           = 1.0f;
    float percHealth    = GetHealthPct();

    /// World creatures
    if (GetMap()->GetInstanceId() == 0)
    {
        uint32 count = GetSizeSaveThreat();
        if (IsPersonal() && count)
        {
            mod += _GetHealthModPersonal(count) * count;
            value *= mod;
        }

        SetMaxHealth(uint64(value));

        if (IsPersonal() && count)
        {
            uint64 l_Health = std::max(uint64(1), CalculatePct(GetMaxHealth(), percHealth));
            SetHealth(l_Health);
        }

        return;
    }
    /// Dungeons or Raid (except for Legion ones, using creature_scaling_pct table)
    else if (GetMap()->IsNeedRecalc() && GetMap()->Expansion() != Expansion::EXPANSION_LEGION)
    {
        if (uint32 count = GetMap()->GetPlayerCount())
        {
            mod += _GetHealthModPersonal(count) * count;
            value *= mod;

            SetMaxHealth(uint64(value));

            uint64 l_Health = std::max(uint64(1), CalculatePct(GetMaxHealth(), percHealth));
            SetHealth(l_Health);
        }

        return;
    }

    SetMaxHealth(uint64(value));
}

void Creature::UpdateMaxPower(Powers power)
{
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + power);

    float value  = GetTotalAuraModValue(unitMod);
    SetMaxPower(power, uint32(value));
}

void Creature::UpdateAttackPowerAndDamage(bool ranged)
{
    UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

    uint16 index = UNIT_FIELD_ATTACK_POWER;
    uint16 index_mult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

    if (ranged)
    {
        index = UNIT_FIELD_RANGED_ATTACK_POWER;
        index_mult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;
    }

    float base_attPower  = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    SetInt32Value(index, (uint32)base_attPower);            //UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetFloatValue(index_mult, attPowerMultiplier);          //UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field

    //automatically update weapon damage after attack power modification
    if (ranged)
        UpdateDamagePhysical(WeaponAttackType::RangedAttack);
    else
    {
        UpdateDamagePhysical(WeaponAttackType::BaseAttack);
        UpdateDamagePhysical(WeaponAttackType::OffAttack);
    }
}

void Creature::UpdateDamagePhysical(WeaponAttackType p_AttType, bool /*l_NoLongerDualWields*/)
{
    float l_Variance = 1.0f;
    UnitMods l_UnitMod;
    switch (p_AttType)
    {
        case WeaponAttackType::BaseAttack:
        default:
            l_Variance = GetCreatureTemplate()->baseVariance;
            l_UnitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case WeaponAttackType::OffAttack:
            l_Variance = GetCreatureTemplate()->baseVariance;
            l_UnitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case WeaponAttackType::RangedAttack:
            l_Variance = GetCreatureTemplate()->rangeVariance;
            l_UnitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
    }

    float l_WeaponMinDamage = GetBaseWeaponDamageRange(p_AttType, MINDAMAGE);
    float l_WeaponMaxDamage = GetBaseWeaponDamageRange(p_AttType, MAXDAMAGE);

    /* difference in AP between current attack power and base value from DB */
    // creature's damage in battleground is calculated in Creature::SelectLevel
    // so don't change it with ap and dmg multipliers

    float l_AttackPower      = GetTotalAttackPowerValue(p_AttType);
    float l_AttackSpeedMulti = GetAPMultiplier(p_AttType, false);
    float l_BaseValue        = GetModifierValue(l_UnitMod, BASE_VALUE) + (l_AttackPower / 14.0f) * l_Variance;
    float l_BasePct          = GetModifierValue(l_UnitMod, BASE_PCT) * l_AttackSpeedMulti;
    float l_TotalValue       = GetModifierValue(l_UnitMod, TOTAL_VALUE);
    float l_TotalPct         = GetModifierValue(l_UnitMod, TOTAL_PCT);
    float l_DmgMultiplier    = GetCreatureTemplate()->dmg_multiplier;

    m_ModDamagePercentTotalPct = GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, SPELL_SCHOOL_MASK_NORMAL);

    /// Since WoD, there is no need to add specific template for each difficulty (except for LFR and Mythic modes)
    /// Then for heroic mode we must increase the dmg_multiplier by 20% (guessed from creature_groupsizestats changes)
    if (GetCreatureTemplate()->expansion == Expansion::EXPANSION_WARLORDS_OF_DRAENOR && GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidHeroic && isWorldBoss())
        l_DmgMultiplier *= 1.2f;

    if (!CanUseAttackType(p_AttType))
    {
        l_WeaponMinDamage = 0;
        l_WeaponMaxDamage = 0;
    }

    if (IsAIEnabled)
        AI()->GetDamageMultiplier(l_DmgMultiplier);

    float mindamage = ((l_BaseValue + l_WeaponMinDamage) * l_DmgMultiplier * l_BasePct + l_TotalValue) * l_TotalPct;
    float maxdamage = ((l_BaseValue + l_WeaponMaxDamage) * l_DmgMultiplier * l_BasePct + l_TotalValue) * l_TotalPct;

    switch (p_AttType)
    {
        case WeaponAttackType::BaseAttack:
        default:
            m_WeaponDamage[WeaponAttackType::BaseAttack][MINDAMAGE] = mindamage;
            m_WeaponDamage[WeaponAttackType::BaseAttack][MAXDAMAGE] = maxdamage;

            SetStatFloatValue(UNIT_FIELD_MIN_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_DAMAGE, maxdamage);
            break;
        case WeaponAttackType::OffAttack:
            m_WeaponDamage[WeaponAttackType::OffAttack][MINDAMAGE] = mindamage;
            m_WeaponDamage[WeaponAttackType::OffAttack][MAXDAMAGE] = maxdamage;

            SetStatFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE, maxdamage);
            break;
        case WeaponAttackType::RangedAttack:
            m_WeaponDamage[WeaponAttackType::RangedAttack][MINDAMAGE] = mindamage;
            m_WeaponDamage[WeaponAttackType::RangedAttack][MAXDAMAGE] = maxdamage;

            SetStatFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE, maxdamage);
            break;
    }
}

/*#######################################
########                         ########
########    PETS STAT SYSTEM     ########
########                         ########
#######################################*/

/// @TODO: Rewrite it to works with pet_stats table
bool Guardian::UpdateStats(Stats p_Stat)
{
    if (uint8(p_Stat) >= MAX_STATS)
        return false;

    float l_Value  = GetTotalStatValue(p_Stat);
    ApplyStatBuffMod(p_Stat, m_statFromOwner[p_Stat], false);
    float l_OwnersBonus = 0.0f;

    Unit* l_Owner = GetSummoner();

    if (!l_Owner)
        return false;

    float l_Mod = 0.75f;

    switch (p_Stat)
    {
        case STAT_STAMINA:
        {
            l_Mod = 0.3f; ///< l_mod is never read 01/18/16

            if (IsPetGhoulOrAbomination() || IsPetGargoyle())
                l_Mod = 0.45f;
            else if (l_Owner->getClass() == CLASS_WARLOCK && isPet())
                l_Mod = 0.75f;
            else if (l_Owner->getClass() == CLASS_MAGE && isPet())
                l_Mod = 0.75f;
            else
            {
                l_Mod = 0.45f;

                if (isPet())
                {
                    switch (ToPet()->GetSpecialization())
                    {
                        case SPEC_PET_FEROCITY: l_Mod = 0.67f; break;
                        case SPEC_PET_TENACITY: l_Mod = 0.78f; break;
                        case SPEC_PET_CUNNING:  l_Mod = 0.725f; break;
                    }
                }
            }

            l_OwnersBonus = float(l_Owner->GetStat(p_Stat)) * l_Mod;
            l_OwnersBonus *= GetModifierValue(UNIT_MOD_STAT_STAMINA, TOTAL_PCT);

            float l_Modifier = 1.0f;
            AuraEffectList const& mModHealthFromOwner = l_Owner->GetAuraEffectsByType(SPELL_AURA_INCREASE_HEALTH_FROM_OWNER);
            for (AuraEffectList::const_iterator l_Iterator = mModHealthFromOwner.begin(); l_Iterator != mModHealthFromOwner.end(); ++l_Iterator)
                l_Modifier += float((*l_Iterator)->GetAmount() / 100.0f);

            l_OwnersBonus *= l_Modifier;
            l_Value += l_OwnersBonus;
            break;
        }
        case STAT_STRENGTH:
        {
            l_Mod = 0.7f;

            /// Artifact Apocalypse - 'The Shambler' Trait - Pet doesnt have a 0.7 multiplier
            if (GetEntry() == DeathKnightPet::ShamblingHorror)
                l_Mod = 0.5175f;

            l_OwnersBonus = l_Owner->GetStat(p_Stat) * l_Mod;

            /// Brewmaster's Niuzao benefit from 100% of owner's agility as strength, for damages
            if (GetEntry() == eMonkPets::Niuzao)
                l_OwnersBonus = l_Owner->GetStat(Stats::STAT_AGILITY);

            l_Value += l_OwnersBonus;
            break;
        }
        case STAT_INTELLECT:
        {
            if (l_Owner->getClass() == CLASS_WARLOCK || l_Owner->getClass() == CLASS_MAGE)
            {
                l_Mod = 0.3f;
                l_OwnersBonus = l_Owner->GetStat(p_Stat) * l_Mod;
            }
            else if (l_Owner->getClass() == CLASS_DEATH_KNIGHT && GetEntry() == 31216)
            {
                l_Mod = 0.3f;

                Unit* l_SimulacrumTarget = sObjectAccessor->GetUnit(*l_Owner, l_Owner->m_SpellHelper.GetUint64(eSpellHelpers::SimulacrumTarget));
                if (l_SimulacrumTarget)
                    l_OwnersBonus = l_SimulacrumTarget->GetStat(p_Stat) * l_Mod;
                else
                    l_OwnersBonus = l_Owner->GetStat(p_Stat) * l_Mod;
            }

            /// Ebon Gargoyle
            if (GetEntry() == DeathKnightPet::Gargoyle)
                l_OwnersBonus = 0.46625f * l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

            l_Value += l_OwnersBonus;
            break;
        }
        case Stats::STAT_AGILITY:
        {
            /// Brewmaster's Niuzao benefit from 100% of owner's agility for armor
            if (GetEntry() == eMonkPets::Niuzao)
                l_OwnersBonus = l_Owner->GetStat(Stats::STAT_AGILITY);

            l_Value += l_OwnersBonus;
            break;
        }
        default:
            break;
    }

    SetStat(p_Stat, int32(l_Value));
    m_statFromOwner[p_Stat] = l_OwnersBonus;
    ApplyStatBuffMod(p_Stat, m_statFromOwner[p_Stat], true);

    switch (p_Stat)
    {
        case STAT_STRENGTH:
            UpdateAttackPowerAndDamage();
            break;
        case STAT_AGILITY:
            UpdateArmor();
            break;
        case STAT_STAMINA:
            UpdateMaxHealth();
            break;
        case STAT_INTELLECT:
            UpdateMaxPower(POWER_MANA);
            if (l_Owner->getClass() == CLASS_MAGE || l_Owner->getClass() == CLASS_WARLOCK)
                UpdateAttackPowerAndDamage();
            break;
        default:
            break;
    }

    return true;
}

// Wod updated
bool Guardian::UpdateAllStats()
{
    for (uint8 l_I = STAT_STRENGTH; l_I < MAX_STATS; ++l_I)
        UpdateStats(Stats(l_I));

    for (uint8 l_I = POWER_MANA; l_I < MAX_POWERS; ++l_I)
        UpdateMaxPower(Powers(l_I));

    for (uint8 l_I = SPELL_SCHOOL_NORMAL; l_I < MAX_SPELL_SCHOOL; ++l_I)
        UpdateResistances(l_I);

    return true;
}

// WoD updated
void Guardian::UpdateResistances(uint32 p_School)
{
    if (p_School == SPELL_SCHOOL_NORMAL)
    {
        UpdateArmor();
        return;
    }

    float l_Value = GetTotalAuraModValue(UnitMods(UNIT_MOD_RESISTANCE_START + p_School));

    SetResistance(SpellSchools(p_School), int32(l_Value));
}

// WoD updated
void Guardian::UpdateArmor()
{
    Unit* l_Owner = GetSummoner();
    if (l_Owner == nullptr)
        return;

    UnitMods l_InitMod = UNIT_MOD_ARMOR;
    PetStatInfo const* l_PetStat = GetPetStat();

    /// - Since 6.x, armor of pet are always percentage of owner armor (default 1)
    float l_Value = l_Owner->GetArmor() * (l_PetStat != nullptr ? l_PetStat->m_ArmorCoef : 1.0f);

    /// - Apply mods
    {
        l_Value *= GetModifierValue(l_InitMod, BASE_PCT);
        l_Value *= GetModifierValue(l_InitMod, TOTAL_PCT);

        float l_Amount = 0;
        AuraEffectList const& l_ModPetStats = l_Owner->GetAuraEffectsByType(SPELL_AURA_MOD_PET_STATS);
        for (AuraEffectList::const_iterator l_Iterator = l_ModPetStats.begin(); l_Iterator != l_ModPetStats.end(); ++l_Iterator)
        {
            if ((*l_Iterator)->GetMiscValue() == INCREASE_ARMOR_PERCENT && (*l_Iterator)->GetMiscValueB() && (int32)GetEntry() == (*l_Iterator)->GetMiscValueB())
                l_Amount += float((*l_Iterator)->GetAmount());
        }

        AddPct(l_Value, l_Amount);
    }

    SetArmor(int32(std::round(l_Value)));
}

// WoD updated
void Guardian::UpdateMaxHealth()
{
    Unit* l_Owner = GetSummoner();
    if (l_Owner == nullptr)
        return;

    UnitMods l_UnitMod = UNIT_MOD_HEALTH;
    PetStatInfo const* l_PetStat = GetPetStat();

    float l_Value = (l_PetStat != nullptr) ? l_Owner->GetMaxHealth() * GetPetStat()->m_HealthCoef : GetModifierValue(l_UnitMod, BASE_VALUE) + GetCreateHealth();

    l_Value *= GetModifierValue(l_UnitMod, BASE_PCT);
    l_Value += GetModifierValue(l_UnitMod, TOTAL_VALUE);
    l_Value *= GetModifierValue(l_UnitMod, TOTAL_PCT);

    float l_Amount = 0;
    AuraEffectList const& l_ModPetStats = l_Owner->GetAuraEffectsByType(SPELL_AURA_MOD_PET_STATS);
    for (AuraEffectList::const_iterator l_Iterator = l_ModPetStats.begin(); l_Iterator != l_ModPetStats.end(); ++l_Iterator)
    {
        if ((*l_Iterator)->GetMiscValueB() && (int32)GetEntry() != (*l_Iterator)->GetMiscValueB())
            continue;
        if ((*l_Iterator)->GetMiscValue() == INCREASE_HEALTH_PERCENT)
            l_Amount += float((*l_Iterator)->GetAmount());
    }

    AddPct(l_Value, l_Amount);
    SetMaxHealth((uint32)l_Value);
}

// WoD updated
void Guardian::UpdateMaxPower(Powers p_Power)
{
    UnitMods l_UnitMod = UnitMods(UNIT_MOD_POWER_START + p_Power);
    PetStatInfo const* l_PetStat = GetPetStat();

    Unit* l_Owner = GetSummoner();
    if (l_Owner == nullptr)
        return;

    float l_AddValue = (l_PetStat != nullptr && l_PetStat->m_Power == p_Power) ? 0.0f : GetStat(STAT_INTELLECT) - GetCreateStat(STAT_INTELLECT);
    float l_Multiplicator = 15.0f;

    float l_Value = 0.0f;
    if (l_PetStat != nullptr)
    {
        if (l_PetStat->m_CreatePower == -1)
            l_Value = GetCreatePowers(l_PetStat->m_Power);
        else
        {
            // Negative number, it's fix value
            if (l_PetStat->m_CreatePower < 0.0f)
                l_Value = l_PetStat->m_CreatePower * -1;
            // Positive number, it's percentage of owner power
            else
                l_Value = float(l_Owner->GetMaxPower(l_Owner->getPowerType()) * l_PetStat->m_CreatePower);
        }
    }

    l_Value += GetModifierValue(l_UnitMod, BASE_VALUE);
    l_Value *= GetModifierValue(l_UnitMod, BASE_PCT);
    l_Value += GetModifierValue(l_UnitMod, TOTAL_VALUE) + l_AddValue * l_Multiplicator;
    l_Value *= GetModifierValue(l_UnitMod, TOTAL_PCT);

    SetMaxPower(p_Power, uint32(l_Value));
}

// WoD updated
void Guardian::UpdateAttackPowerAndDamage(bool p_Ranged)
{
    if (p_Ranged)
        return;

    Unit* l_Owner = GetSummoner();
    if (l_Owner == nullptr)
        return;

    UnitMods l_UnitMod = UNIT_MOD_ATTACK_POWER;
    float l_BaseValue  = std::max(0.0f, 2 * GetStat(STAT_STRENGTH) - 20.0f);

    float l_BaseAttackPower       = l_BaseValue;
    float l_SpellPower            = l_BaseValue;
    float l_AttackPowerMultiplier = 1.0f;

    PetStatInfo const* l_PetStat = GetPetStat();
    if (l_PetStat != nullptr)
    {
        switch (l_PetStat->m_PowerStat)
        {
            case PetStatInfo::PowerStatBase::AttackPower:
                if (isHunterPet())
                    l_BaseValue = l_Owner->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack);
                else
                    l_BaseValue = l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
                l_BaseAttackPower = l_BaseValue * l_PetStat->m_APSPCoef;
                l_SpellPower      = l_BaseValue * l_PetStat->m_SecondaryStatCoef;
                break;
            case PetStatInfo::PowerStatBase::SpellPower:
                l_BaseValue       = l_Owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL);
                l_SpellPower      = l_BaseValue * l_PetStat->m_APSPCoef;
                l_BaseAttackPower = l_BaseValue * l_PetStat->m_SecondaryStatCoef;
                break;
            default:
                break;
        }
    }

    SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, l_BaseAttackPower);

    if (l_Owner->IsPlayer() && isPet())
        l_Owner->SetUInt32Value(PLAYER_FIELD_PET_SPELL_POWER, l_SpellPower);
    else if (l_Owner->IsPlayer())
        SetUInt32Value(UNIT_FIELD_STATS + STAT_INTELLECT, l_SpellPower);

    l_BaseAttackPower      *= GetModifierValue(l_UnitMod, BASE_PCT);
    l_AttackPowerMultiplier = GetModifierValue(l_UnitMod, TOTAL_PCT) - 1.0f;

    /// - UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetInt32Value(UNIT_FIELD_ATTACK_POWER,        (int32)std::round(l_BaseAttackPower));
    SetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER, (int32)std::round(l_BaseAttackPower));

    /// - UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field
    SetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER,        l_AttackPowerMultiplier);
    SetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, l_AttackPowerMultiplier);

    /// - Automatically update weapon damage after attack power modification
    UpdateDamagePhysical(WeaponAttackType::BaseAttack);

    switch (GetEntry())
    {
        case WarlockPet::Shivarra:
        case WarlockPet::WrathGuard:
        case eMonkPets::Fire:
        case eMonkPets::Earth:
            UpdateDamagePhysical(WeaponAttackType::OffAttack);
            break;
        case DeathKnightPet::Skulker:
            UpdateDamagePhysical(WeaponAttackType::RangedAttack);
            break;
    }
}

/// WoD updated
void Guardian::UpdateDamagePhysical(WeaponAttackType p_AttType, bool /*l_NoLongerDualWields*/)
{
    UnitMods l_UnitMod;
    switch (p_AttType)
    {
        case WeaponAttackType::BaseAttack:
        default:
            l_UnitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case WeaponAttackType::OffAttack:
            l_UnitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case WeaponAttackType::RangedAttack:
            l_UnitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
    }

    float l_AttackSpeed = float(GetAttackTime(WeaponAttackType::BaseAttack)) / 1000.0f;

    float l_BaseValue  = GetModifierValue(l_UnitMod, BASE_VALUE);

    l_BaseValue += GetTotalAttackPowerValue(p_AttType);

    PetStatInfo const* l_PetStat = GetPetStat();

    /// Mindbender for priest doesn't have attack speed consideration for its damages.
    if (GetEntry() != 62982)
    {
        l_BaseValue /= 3.5f;

        if (l_PetStat)
            l_BaseValue *= l_PetStat->m_AttackSpeed;
        else
            l_BaseValue *= l_AttackSpeed;
    }

    if (l_PetStat)
        l_BaseValue *= l_PetStat->m_DamageCoef;

    float l_BasePct    = GetModifierValue(l_UnitMod, BASE_PCT);
    float l_TotalValue = GetModifierValue(l_UnitMod, TOTAL_VALUE);
    float l_TotalPct   = GetModifierValue(l_UnitMod, TOTAL_PCT);

    float l_WeaponMinDamage = GetBaseWeaponDamageRange(WeaponAttackType::BaseAttack, MINDAMAGE);
    float l_WeaponMaxDamage = GetBaseWeaponDamageRange(WeaponAttackType::BaseAttack, MAXDAMAGE);

    float l_MinDamage = ((l_BaseValue + l_WeaponMinDamage) * l_BasePct + l_TotalValue) * l_TotalPct;
    float l_MaxDamage = ((l_BaseValue + l_WeaponMaxDamage) * l_BasePct + l_TotalValue) * l_TotalPct;

    switch (p_AttType)
    {
        case WeaponAttackType::BaseAttack:
        default:
            m_WeaponDamage[WeaponAttackType::BaseAttack][MINDAMAGE] = l_MinDamage;
            m_WeaponDamage[WeaponAttackType::BaseAttack][MAXDAMAGE] = l_MaxDamage;

            SetStatFloatValue(UNIT_FIELD_MIN_DAMAGE, l_MinDamage);
            SetStatFloatValue(UNIT_FIELD_MAX_DAMAGE, l_MaxDamage);
            break;
        case WeaponAttackType::OffAttack:
            m_WeaponDamage[WeaponAttackType::OffAttack][MINDAMAGE] = l_MinDamage;
            m_WeaponDamage[WeaponAttackType::OffAttack][MAXDAMAGE] = l_MaxDamage;

            SetStatFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE, l_MinDamage);
            SetStatFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE, l_MaxDamage);
            break;
        case WeaponAttackType::RangedAttack:
            m_WeaponDamage[WeaponAttackType::RangedAttack][MINDAMAGE] = l_MinDamage;
            m_WeaponDamage[WeaponAttackType::RangedAttack][MAXDAMAGE] = l_MaxDamage;

            SetStatFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE, l_MinDamage);
            SetStatFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE, l_MaxDamage);
            break;
    }
}
