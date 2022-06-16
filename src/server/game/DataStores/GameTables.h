////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "GameTableReader.h"
#include "Common.h"

const int8 g_ClassOrderByClassID[MAX_CLASSES]
{
    -1,
    8,  ///< CLASS_WARRIOR
    4,  ///< CLASS_PALADIN
    2,  ///< CLASS_HUNTER
    0,  ///< CLASS_ROGUE
    5,  ///< CLASS_PRIEST
    9,  ///< CLASS_DEATH_KNIGHT
    6,  ///< CLASS_SHAMAN
    3,  ///< CLASS_MAGE
    7,  ///< CLASS_WARLOCK
    10, ///< CLASS_MONK
    1,  ///< CLASS_DRUID
    11, ///< CLASS_DEMON_HUNTER
};

enum eSpellScaling
{
    ItemScaling         = 31,
    ConsumableScaling   = 30,
    Gem1Scaling         = 29,
    Gem2Scaling         = 28,
    Gem3Scaling         = 27,
    HealthScaling       = 26,
    ItemScaling2        = 25, ///< between 7.1.5 and 7.3.5, there was a split between some of the ScalingClass 255: some became 31, and some became 25
    EnchantScaling      = -1,
    Gem1Scaling_2       = -3,
    Gem2Scaling_2       = -4,
    Gem3Scaling_2       = -5
};

struct ClassTable
{
    float m_ClassValue[MAX_CLASSES - 1];

    /// They are not in the same order as classIds, but all of them have the same order
    float GetScalingForClassID(uint8 p_ClassId) const { return m_ClassValue[g_ClassOrderByClassID[p_ClassId]]; }
};

struct BaseManaTableEntry
{
    uint32 m_Level;
    ClassTable m_ClassScaling;
};

struct SpellScalingTableEntry
{
    uint32 m_Level;

    union
    {
        float m_RawValues[18];

        struct
        {
            ClassTable m_ClassScaling;
            float m_ItemLevel;                      ///< ItemLevel at the specified Level
            float m_Consumable;                     ///< Consumable Scaling Value
            float m_Gem[MAX_ITEM_PROTO_SOCKETS];    ///< Gem Scaling
            float m_Health;                         ///< Health
        } Named;
    };

    float GetSpellScalingValue(uint32 p_Index) const
    {
        if (p_Index < MAX_CLASSES)
            return Named.m_ClassScaling.GetScalingForClassID(p_Index);

        return m_RawValues[p_Index];
    }
};

inline float GetSpellScalingColumnForClass(SpellScalingTableEntry const* p_Row, int32 p_Class)
{
    switch (p_Class)
    {
        case CLASS_WARRIOR:
        case CLASS_PALADIN:
        case CLASS_HUNTER:
        case CLASS_ROGUE:
        case CLASS_PRIEST:
        case CLASS_DEATH_KNIGHT:
        case CLASS_SHAMAN:
        case CLASS_MAGE:
        case CLASS_WARLOCK:
        case CLASS_MONK:
        case CLASS_DRUID:
        case CLASS_DEMON_HUNTER:
            return p_Row->GetSpellScalingValue(p_Class);

        case eSpellScaling::ItemScaling:
        case eSpellScaling::ItemScaling2:
        case eSpellScaling::EnchantScaling:
            return p_Row->Named.m_ItemLevel;

        case eSpellScaling::ConsumableScaling:
            return p_Row->Named.m_Consumable;

        case eSpellScaling::Gem1Scaling:
        case eSpellScaling::Gem1Scaling_2:
            return p_Row->Named.m_Gem[0];

        case eSpellScaling::Gem2Scaling:
        case eSpellScaling::Gem2Scaling_2:
            return p_Row->Named.m_Gem[1];

        case eSpellScaling::Gem3Scaling:
        case eSpellScaling::Gem3Scaling_2:
            return p_Row->Named.m_Gem[2];

        case eSpellScaling::HealthScaling:
            return p_Row->Named.m_Health;

        default:
            return (p_Row->m_RawValues[p_Class - 1]);
            break;
    }

    return 0.0f;
}

struct XPTableEntry
{
    uint32 m_Level;
    float m_Required;
    float m_PerKill;
    float m_Junk;
    float m_Stats;
    float m_Dividor;
};

#define MAX_HONOR_PRESTIGE_LEVEL 33

struct HonorLevelTableEntry
{
    uint32 m_Level;
    float m_RequiredHonor[MAX_HONOR_PRESTIGE_LEVEL];
};

struct HpPerStaTableEntry
{
    uint32 m_Level;
    float m_HP;
};

struct ArmorMitigationByLvlTableEntry
{
    uint32 m_Level;
    float m_Multiplier;
};

struct ChallengeModeDamageEntry
{
    float ChallengeLevel;
    float Scalar;
};

struct ChallengeModeHealthEntry
{
    float ChallengeLevel;
    float Scalar;
};

struct CombatRatingsTableEntry
{
    uint32 m_Level;
    float Amplify;
    float DefenseSkill;
    float Dodge;
    float Parry;
    float Block;
    float HitMelee;
    float HitRanged;
    float HitSpell;
    float CritMelee;
    float CritRanged;
    float CritSpell;
    float MultiStrike;
    float Readiness;
    float Speed;
    float ResilienceCritTaken;
    float ResiliencePlayerDamage;
    float Lifesteal;
    float HasteMelee;
    float HasteRanged;
    float HasteSpell;
    float Avoidance;
    float Sturdiness;
    float Unused7;
    float Expertise;
    float ArmorPenetration;
    float Mastery;
    float PvPPower;
    float Cleave;
    float VersatilityDamageDone;
    float VersatilityHealingDone;
    float VersatilityDamageTaken;
    float Unused12;

    inline float GetValue(uint32 p_Rating) const
    {
        switch (p_Rating)
        {
            case CR_AMPLIFY:
                return this->Amplify;
            case CR_DEFENSE_SKILL:
                return this->DefenseSkill;
            case CR_DODGE:
                return this->Dodge;
            case CR_PARRY:
                return this->Parry;
            case CR_BLOCK:
                return this->Block;
            case CR_HIT_MELEE:
                return this->HitMelee;
            case CR_HIT_RANGED:
                return this->HitRanged;
            case CR_HIT_SPELL:
                return this->HitSpell;
            case CR_CRIT_MELEE:
                return this->CritMelee;
            case CR_CRIT_RANGED:
                return this->CritRanged;
            case CR_CRIT_SPELL:
                return this->CritSpell;
            case CR_MULTISTRIKE:
                return this->MultiStrike;
            case CR_READINESS:
                return this->Readiness;
            case CR_SPEED:
                return this->Speed;
            case CR_RESILIENCE_CRIT_TAKEN:
                return this->ResilienceCritTaken;
            case CR_RESILIENCE_PLAYER_DAMAGE_TAKEN:
                return this->ResiliencePlayerDamage;
            case CR_LIFESTEAL:
                return this->Lifesteal;
            case CR_HASTE_MELEE:
                return this->HasteMelee;
            case CR_HASTE_RANGED:
                return this->HasteRanged;
            case CR_HASTE_SPELL:
                return this->HasteSpell;
            case CR_AVOIDANCE:
                return this->Avoidance;
            case CR_STURDINESS:
                return this->Sturdiness;
            case CR_UNUSED_7:
                return this->Unused7;
            case CR_EXPERTISE:
                return this->Expertise;
            case CR_ARMOR_PENETRATION:
                return this->ArmorPenetration;
            case CR_MASTERY:
                return this->Mastery;
            case CR_PVP_POWER:
                return this->PvPPower;
            case CR_CLEAVE:
                return this->Cleave;
            case CR_VERSATILITY_DAMAGE_DONE:
                return this->VersatilityDamageDone;
            case CR_VERSATUKUTY_HEALING_DONE:
                return this->VersatilityHealingDone;
            case CR_VERSATILITY_DAMAGE_TAKEN:
                return this->VersatilityDamageTaken;
            case CR_UNUSED_12:
                return this->Unused12;
            default:
                break;
        }

        return 1.0f;
    }
};

struct CombatRatingsMultByILvl
{
    uint32 m_Level;
    float ArmorMultiplier   = 0.0f;
    float WeaponMultiplier  = 0.0f;
    float TrinketMultiplier = 0.0f;
    float JewelryMultiplier = 0.0f;
};

struct ArtifactLevelXPTableEntry
{
    uint32 m_Level;
    float XP = 0.0f;
    float XP2 = 0.0f;
};

struct ArtifactKnowledgeMultiplierEntry
{
    uint32 m_Level;
    float Multiplier = 0.0f;
};

struct ItemSocketCostPerLevelEntry
{
    uint32 m_Level;
    float cost;
};

struct BarberShopCostBaseEntry
{
    uint32 m_Level;
    float   cost;
};

struct BattlePetTypeDamageModEntry
{
    uint32 m_Level;

    union
    {
        float m_RawValues[10];

        struct
        {
            float Humanoid;
            float Dragonkin;
            float Flying;
            float Undead;
            float Critter;
            float Magic;
            float Elemental;
            float Beast;
            float Aquatic;
            float Mechanical;
        } Named;
    };
};

struct BattlePetXPEntry
{
    uint32 m_Level;
    float   wins;
    float   xp;
};

struct NpcDamageByClassEntry
{
    uint32 m_Level;
    ClassTable m_ClassScaling;
};

struct NpcTotalHpEntry
{
    uint32 m_Level;
    ClassTable m_ClassScaling;
};

struct NpcManaCostScalerEntry
{
    uint32 m_Level;
    float Scaler;
};

extern GameTable<BaseManaTableEntry>                g_BaseManaTable;
extern GameTable<SpellScalingTableEntry>            g_SpellScalingTable;
extern GameTable<XPTableEntry>                      g_XPTable;
extern GameTable<HonorLevelTableEntry>              g_HonorLevelTable;
extern GameTable<HpPerStaTableEntry>                g_HpPerStaTable;
extern GameTable<ArmorMitigationByLvlTableEntry>    g_ArmorMitigationByLvlTable;
extern GameTable<CombatRatingsTableEntry>           g_CombatRatingsTable;
extern GameTable<CombatRatingsMultByILvl>           g_CombatRatingMultByIlvl;
extern GameTable<ArtifactLevelXPTableEntry>         g_ArtifactLevelXP;
extern GameTable<ArtifactKnowledgeMultiplierEntry>  g_ArtifactKnowledgeMultiplier;
extern GameTable<ItemSocketCostPerLevelEntry>       g_ItemSocketCostPerLevel;
extern GameTable<BarberShopCostBaseEntry>           g_BarberShopCostBaseEntry;
extern GameTable<BattlePetTypeDamageModEntry>       g_BattlePetTypeDamageModEntry;
extern GameTable<BattlePetXPEntry>                  g_BattlePetXPStore;
extern GameTable<NpcDamageByClassEntry>             g_NpcDamageByClassGameTable[MAX_EXPANSION];
extern GameTable<NpcTotalHpEntry>                   g_NpcTotalHpGameTable[MAX_EXPANSION];
extern GameTable<NpcManaCostScalerEntry>            g_NpcManaCostScalerGameTable;
extern GameTable<ChallengeModeDamageEntry>          g_ChallengeModeDamageTable;
extern GameTable<ChallengeModeHealthEntry>          g_ChallengeModeHealthTable;

void LoadGameTables(const std::string& p_DataPath);
