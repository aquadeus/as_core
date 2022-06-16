////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBCENUMS_H
#define DBCENUMS_H

enum LevelLimit
{
    MAX_LEVEL        = 110, ///< client supported max level for player/pets/etc. Avoid overflow or client stability affected.
    GT_MAX_LEVEL     = 123, ///< All gametable store data for 123 levels, some by 123 per class/race
    STRONG_MAX_LEVEL = 255  ///< Server side limitation. Base at used code requirements.
};

#define MIN_SPECIALIZATION_LEVEL    10
#define MAX_TALENT_TIERS 7
#define MAX_TALENT_COLUMNS 3

#define MAX_ITEM_PROTO_SOCKETS 3
#define MAX_ITEM_PROTO_SPELLS  5
#define MAX_ITEM_PROTO_STATS  10

enum CharSectionType
{
    SECTION_TYPE_SKIN_LOW_RES             = 0,
    SECTION_TYPE_FACE_LOW_RES             = 1,
    SECTION_TYPE_FACIAL_HAIR_LOW_RES      = 2,
    SECTION_TYPE_HAIR_LOW_RES             = 3,
    SECTION_TYPE_UNDERWEAR_LOW_RES        = 4,
    SECTION_TYPE_SKIN                     = 5,
    SECTION_TYPE_FACE                     = 6,
    SECTION_TYPE_FACIAL_HAIR              = 7,
    SECTION_TYPE_HAIR                     = 8,
    SECTION_TYPE_UNDERWEAR                = 9,
    SECTION_TYPE_CUSTOM_DISPLAY_1_LOW_RES = 10,
    SECTION_TYPE_CUSTOM_DISPLAY_1         = 11,
    SECTION_TYPE_CUSTOM_DISPLAY_2_LOW_RES = 12,
    SECTION_TYPE_CUSTOM_DISPLAY_2         = 13,
    SECTION_TYPE_CUSTOM_DISPLAY_3_LOW_RES = 14,
    SECTION_TYPE_CUSTOM_DISPLAY_3         = 15
};

enum BattlePetEffectCategory
{
    BATTLEPET_EFFECT_CATEGORY_DEAL = 9,
    BATTLEPET_EFFECT_CATEGORY_PERIODIC = 11,
    BATTLEPET_EFFECT_CATEGORY_BUFF = 12,
    BATTLEPET_EFFECT_CATEGORY_AURA = 23,
};


/// @see EmoteType in EmotesEntry
namespace EmoteTypes
{
    enum
    {
        OneStep,
        EmoteLoop,
        StateLoop       ///< Also related to m_EventSoundID, client play the sound only when EmoteType == StateLoop
    };
}

enum SkillRaceClassInfoFlags
{
    SKILL_FLAG_NO_SKILLUP_MESSAGE       = 0x002,
    SKILL_FLAG_ALWAYS_MAX_VALUE         = 0x010,
    SKILL_FLAG_UNLEARNABLE              = 0x020,        ///< Skill can be unlearned
    SKILL_FLAG_INCLUDE_IN_SORT          = 0x080,        ///< Spells belonging to a skill with this flag will additionally compare skill ids when sorting spellbook in client
    SKILL_FLAG_NOT_TRAINABLE            = 0x100,
    SKILL_FLAG_MONO_VALUE               = 0x400         ///< Skill always has value 1 - clientside display flag, real value can be different
};

enum SkillLineAbilityFlags
{
    SKILL_LINE_ABILITY_LEARNED_ON_SKILL_VALUE = 1, // Spell state will update depending on skill value
    SKILL_LINE_ABILITY_LEARNED_ON_SKILL_LEARN = 2  // Spell will be learned/removed together with entire skill
};

enum ChrSpecializationFlag
{
    CHR_SPECIALIZATION_FLAG_CASTER                  = 0x01,
    CHR_SPECIALIZATION_FLAG_RANGED                  = 0x02,
    CHR_SPECIALIZATION_FLAG_MELEE                   = 0x04,
    CHR_SPECIALIZATION_FLAG_UNKNOWN                 = 0x08,
    CHR_SPECIALIZATION_FLAG_DUAL_WIELD_TWO_HANDED   = 0x10,     // used for CUnitDisplay::SetSheatheInvertedForDualWield
    CHR_SPECIALIZATION_FLAG_PET_OVERRIDE_SPEC       = 0x20,
    CHR_SPECIALIZATION_FLAG_RECOMMENDED             = 0x40,
};

enum Curves
{
    CURVE_ID_ARTIFACT_RELIC_ITEM_LEVEL_BONUS = 1718
};

enum BattlePetEffectFlags
{
    BATTLEPET_EFFECT_FLAG_POSITIVE = 1 << 0,
    BATTLEPET_EFFECT_FLAG_NEGATIVE = 1 << 1,
    /*
    BATTLEPET_EFFECT_CATEGORY_HEAL = 37,
    BATTLEPET_EFFECT_CATEGORY_DAMAGE = 38,
    BATTLEPET_EFFECT_CATEGORY_PERIODIC_POSITIVE_BUFF = 47,
    BATTLEPET_EFFECT_CATEGORY_POSITIVE_BUFF = 49,
    BATTLEPET_EFFECT_CATEGORY_CONTROL_BUFF = 50,
    BATTLEPET_EFFECT_CATEGORY_PERIODIC_NEGATIVE_BUFF = 51,
    BATTLEPET_EFFECT_CATEGORY_NEGATIVE_BUFF = 94,
    */
};

enum BattlePetSpeciesFlags
{
    BATTLEPET_SPECIES_FLAG_CONDITIONAL = 0x002,
    BATTLEPET_SPECIES_FLAG_CAGEABLE    = 0x010,
    BATTLEPET_SPECIES_FLAG_UNTAMEABLE  = 0x020,
    BATTLEPET_SPECIES_FLAG_UNIQUE      = 0x040,
    BATTLEPET_SPECIES_FLAG_COMPANION   = 0x080,
    BATTLEPET_SPECIES_FLAG_ELITE       = 0x400,
};


namespace CharShipmentFlags
{
    enum
    {
        Quest = 0x1,
        Internal = 0x8
    };
}

enum CurrencyFlags
{
    CURRENCY_FLAG_TRADEABLE             = 0x01,
    CURRENCY_FLAG_HIGH_PRECISION        = 0x08,
    CURRENCY_FLAG_ARCHAEOLOGY_FRAGMENT  = 0x20,
    CURRENCY_FLAG_HAS_SEASON_COUNT      = 0x80                  // guessed
};

enum AreaTeams
{
    AREATEAM_NONE  = 0,
    AREATEAM_ALLY  = 2,
    AREATEAM_HORDE = 4,
    AREATEAM_ANY   = 6
};

enum AreaFlags
{
    AREA_FLAG_SNOW                        = 0x00000001, // snow (only Dun Morogh, Naxxramas, Razorfen Downs and Winterspring)
    AREA_FLAG_UNK1                        = 0x00000002, // Razorfen Downs, Naxxramas and Acherus: The Ebon Hold (3.3.5a)
    AREA_FLAG_UNK2                        = 0x00000004, // Only used for areas on map 571 (development before)
    AREA_FLAG_SLAVE_CAPITAL               = 0x00000008, // city and city subsones
    AREA_FLAG_UNK3                        = 0x00000010, // can't find common meaning
    AREA_FLAG_SLAVE_CAPITAL2              = 0x00000020, // slave capital city flag?
    AREA_FLAG_ALLOW_DUELS                 = 0x00000040, // allow to duel here
    AREA_FLAG_ARENA                       = 0x00000080, // arena, both instanced and world arenas
    AREA_FLAG_CAPITAL                     = 0x00000100, // main capital city flag
    AREA_FLAG_CITY                        = 0x00000200, // only for one zone named "City" (where it located?)
    AREA_FLAG_OUTLAND                     = 0x00000400, // expansion zones? (only Eye of the Storm not have this flag, but have 0x00004000 flag)
    AREA_FLAG_SANCTUARY                   = 0x00000800, // sanctuary area (PvP disabled)
    AREA_FLAG_NEED_FLY                    = 0x00001000, // Respawn alive at the graveyard without corpse
    AREA_FLAG_UNUSED1                     = 0x00002000, // Unused in 3.3.5a
    AREA_FLAG_OUTLAND2                    = 0x00004000, // expansion zones? (only Circle of Blood Arena not have this flag, but have 0x00000400 flag)
    AREA_FLAG_OUTDOOR_PVP                 = 0x00008000, // pvp objective area? (Death's Door also has this flag although it's no pvp object area)
    AREA_FLAG_ARENA_INSTANCE              = 0x00010000, // used by instanced arenas only
    AREA_FLAG_UNUSED2                     = 0x00020000, // Unused in 3.3.5a
    AREA_FLAG_CONTESTED_AREA              = 0x00040000, // On PvP servers these areas are considered contested, even though the zone it is contained in is a Horde/Alliance territory.
    AREA_FLAG_UNK6                        = 0x00080000, // Valgarde and Acherus: The Ebon Hold
    AREA_FLAG_LOWLEVEL                    = 0x00100000, // used for some starting areas with area_level <= 15
    AREA_FLAG_TOWN                        = 0x00200000, // small towns with Inn
    AREA_FLAG_REST_ZONE_HORDE             = 0x00400000, // Warsong Hold, Acherus: The Ebon Hold, New Agamand Inn, Vengeance Landing Inn, Sunreaver Pavilion (Something to do with team?)
    AREA_FLAG_REST_ZONE_ALLIANCE          = 0x00800000, // Valgarde, Acherus: The Ebon Hold, Westguard Inn, Silver Covenant Pavilion (Something to do with team?)
    AREA_FLAG_WINTERGRASP                 = 0x01000000, // Wintergrasp and it's subzones
    AREA_FLAG_INSIDE                      = 0x02000000, // used for determinating spell related inside/outside questions in Map::IsOutdoors
    AREA_FLAG_OUTSIDE                     = 0x04000000, // used for determinating spell related inside/outside questions in Map::IsOutdoors
    AREA_FLAG_CAN_HEARTH_AND_RESURRECT    = 0x08000000, // Can Hearth And Resurrect From Area
    AREA_FLAG_NO_FLY_ZONE                 = 0x20000000, // Marks zones where you cannot fly
    AREA_FLAG_UNK9                        = 0x40000000
};

enum AreaFlags2
{
    AREA_FLAG2_UNK1             = 0x00000001,                //
    AREA_FLAG2_UNK2             = 0x00000002,                // PattymackLand only
    AREA_FLAG2_UNK3             = 0x00000004,                // donjons / raids
    AREA_FLAG2_UNK4             = 0x00000008,                // OrgrimmarRaid and DraenorAuchindoun
    AREA_FLAG2_UNK5             = 0x00000010,                //
    AREA_FLAG2_UNK6             = 0x00000020,                //
    AREA_FLAG2_UNK7             = 0x00000040,                // Garrison
    AREA_FLAG2_UNK8             = 0x00000080,                //
    AREA_FLAG2_UNK9             = 0x00000100,                //
    AREA_FLAG2_UNK10            = 0x00000200,                //
    AREA_FLAG2_UNK11            = 0x00000400,                //
    AREA_FLAG2_UNK12            = 0x00000800,                //
    AREA_FLAG2_UNK13            = 0x00001000,                //
    AREA_FLAG2_UNK14            = 0x00002000,                //
    AREA_FLAG2_UNK15            = 0x00004000,                //
    AREA_FLAG2_UNK16            = 0x00008000,                //
    AREA_FLAG2_UNK17            = 0x00010000,                //
    AREA_FLAG2_UNK18            = 0x00020000,                //
    AREA_FLAG2_UNK19            = 0x00040000,                //
    AREA_FLAG2_UNK20            = 0x00080000,                //
    AREA_FLAG2_UNK21            = 0x00100000,                //
    AREA_FLAG2_UNK22            = 0x00200000,                //
    AREA_FLAG2_UNK23            = 0x00400000,                //
    AREA_FLAG2_UNK24            = 0x00800000,                //
    AREA_FLAG2_UNK25            = 0x01000000,                //
    AREA_FLAG2_UNK26            = 0x02000000,                //
    AREA_FLAG2_UNK27            = 0x04000000,                //
    AREA_FLAG2_UNK28            = 0x08000000,                //
    AREA_FLAG2_UNK29            = 0x20000000,                //
    AREA_FLAG2_UNK30            = 0x40000000
};

enum Difficulty
{
    DifficultyNone              = 0,  ///< difficulty_entry_0
    DifficultyNormal            = 1,  ///< difficulty_entry_1
    DifficultyHeroic            = 2,  ///< difficulty_entry_2
    Difficulty10N               = 3,  ///< difficulty_entry_3
    Difficulty25N               = 4,  ///< difficulty_entry_4
    Difficulty10HC              = 5,  ///< difficulty_entry_5
    Difficulty25HC              = 6,  ///< difficulty_entry_6
    DifficultyRaidTool          = 7,  ///< difficulty_entry_7
    DifficultyMythicKeystone    = 8,  ///< difficulty_entry_8
    Difficulty40                = 9,  ///< difficulty_entry_9

    DifficultyHCScenario        = 11, ///< difficulty_entry_11
    DifficultyNScenario         = 12, ///< difficulty_entry_12

    DifficultyRaidNormal        = 14, ///< difficulty_entry_14
    DifficultyRaidHeroic        = 15, ///< difficulty_entry_15
    DifficultyRaidMythic        = 16, ///< difficulty_entry_16
    DifficultyRaidLFR           = 17, ///< difficulty_entry_17
    DifficultyEventRaid         = 18, ///< difficulty_entry_18
    DifficultyEventDungeon      = 19, ///< difficulty_entry_19
    DifficultyEventScenario     = 20, ///< difficulty_entry_20

    DifficultyMythic            = 23,
    DifficultyTimewalker        = 24,
    DifficultyWorldPvPScenario  = 25,
    DifficultyPvEvPScenario     = 29,
    DifficultyEvent             = 30,
    DifficultyWorldPvPScenario2 = 32,
    DifficultyTimewalkingRaid   = 33,
    DifficultyPVP               = 34,

    MaxDifficulties
};

enum DifficultyFlags
{
    DIFFICULTY_FLAG_HEROIC          = 0x01,
    DIFFICULTY_FLAG_DEFAULT         = 0x02,
    DIFFICULTY_FLAG_CAN_SELECT      = 0x04, ///< Player can select this difficulty in dropdown menu
    DIFFICULTY_FLAG_CHALLENGE_MODE  = 0x08,

    DIFFICULTY_FLAG_LEGACY          = 0x20,
    DIFFICULTY_FLAG_DISPLAY_HEROIC  = 0x40, ///< Controls icon displayed on minimap when inside the instance
    DIFFICULTY_FLAG_DISPLAY_MYTHIC  = 0x80  ///< Controls icon displayed on minimap when inside the instance
};

enum MapDifficultyFlags : uint8
{
    MAP_DIFFICULTY_FLAG_CANNOT_EXTEND   = 0x10
};

enum SpawnMask
{
    SpawnMaskContinent          = (1 << Difficulty::DifficultyNone),
    SpawnMaskDungeonNormal      = (1 << Difficulty::DifficultyNormal),
    SpawnMaskDungeonHeroic      = (1 << Difficulty::DifficultyHeroic),
    SpawnMaskRaid10Normal       = (1 << Difficulty::Difficulty10N),
    SpawnMaskRaid25Normal       = (1 << Difficulty::Difficulty25N),
    SpawnMaskRaid10Heroic       = (1 << Difficulty::Difficulty10HC),
    SpawnMaskRaid25Heroic       = (1 << Difficulty::Difficulty25HC),
    SpawnMaskRaidTool           = (1 << Difficulty::DifficultyRaidTool),
    SpawnMaskChallengeMode      = (1 << Difficulty::DifficultyMythicKeystone),
    SpawnMaskRaid40Normal       = (1 << Difficulty::Difficulty40),

    SpawnMaskScenarioHeroic     = (1 << Difficulty::DifficultyHCScenario),
    SpawnMaskScenarioNormal     = (1 << Difficulty::DifficultyNScenario),

    SpawnMaskRaidNormal         = (1 << Difficulty::DifficultyRaidNormal),
    SpawnMaskRaidHeroic         = (1 << Difficulty::DifficultyRaidHeroic),
    SpawnMaskRaidMythic         = (1 << Difficulty::DifficultyRaidMythic),
    SpawnMaskRaidLFR            = (1 << Difficulty::DifficultyRaidLFR),
    SpawnMaskEventRaid          = (1 << Difficulty::DifficultyEventRaid),
    SpawnMaskEventDungeon       = (1 << Difficulty::DifficultyEventDungeon),
    SpawnMaskEventScenario      = (1 << Difficulty::DifficultyEventScenario),

    SpawnMaskDungeonMythic      = (1 << Difficulty::DifficultyMythic),
    SpawnMaskDungeonTimewalking = (1 << Difficulty::DifficultyTimewalker),

    SpawnMaskDungeonAll         = (SpawnMask::SpawnMaskDungeonNormal | SpawnMask::SpawnMaskDungeonHeroic | SpawnMask::SpawnMaskChallengeMode),
    SpawnMaskLegacyNormalAll    = (SpawnMask::SpawnMaskRaid10Normal | SpawnMask::SpawnMaskRaid25Normal | SpawnMask::SpawnMaskRaid40Normal),
    SpawnMaskLegacyHeroicAll    = (SpawnMask::SpawnMaskRaid10Heroic | SpawnMask::SpawnMaskRaid25Heroic),
    SpawnMaskLegacyRaidAll      = (SpawnMask::SpawnMaskLegacyNormalAll | SpawnMask::SpawnMaskLegacyHeroicAll | SpawnMask::SpawnMaskRaidTool),
    SpawnMaskActualRaidAll      = (SpawnMask::SpawnMaskRaidNormal | SpawnMask::SpawnMaskRaidHeroic | SpawnMask::SpawnMaskRaidMythic | SpawnMask::SpawnMaskRaidLFR),
    SpawnMaskRaidAll            = (SpawnMask::SpawnMaskLegacyRaidAll | SpawnMask::SpawnMaskActualRaidAll)
};

enum MapFlags
{
    MAP_FLAG_UNK_1                  = 0x00000001,
    MAP_FLAG_DEV                    = 0x00000002,   ///< Client will reject loading of this map
    MAP_FLAG_UNK_3                  = 0x00000004,
    MAP_FLAG_UNK_4                  = 0x00000008,
    MAP_FLAG_UNK_5                  = 0x00000010,
    MAP_FLAG_UNK_6                  = 0x00000020,
    MAP_FLAG_UNK_7                  = 0x00000040,
    MAP_FLAG_UNK_8                  = 0x00000080,
    MAP_FLAG_CAN_TOGGLE_DIFFICULTY  = 0x00000100,   ///< Allow players to change difficulty
    MAP_FLAG_UNK_10                 = 0x00000200,
    MAP_FLAG_UNK_11                 = 0x00000400,
    MAP_FLAG_UNK_12                 = 0x00000800,
    MAP_FLAG_UNK_13                 = 0x00001000,
    MAP_FLAG_UNK_14                 = 0x00002000,
    MAP_FLAG_UNK_15                 = 0x00004000,
    MAP_FLAG_FLEX_LOCKING           = 0x00008000,   ///< All difficulties share completed encounters lock, not bound to a single instance id
                                                    ///< heroic difficulty flag overrides it and uses instance id bind
    MAP_FLAG_LIMIT_FAR_CLIP         = 0x00010000,   ///< Limit farclip to 727.0
    MAP_FLAG_UNK_18                 = 0x00020000,
    MAP_FLAG_UNK_19                 = 0x00040000,
    MAP_FLAG_UNK_20                 = 0x00080000,
    MAP_FLAG_UNK_21                 = 0x00100000,
    MAP_FLAG_UNK_22                 = 0x00200000,
    MAP_FLAG_UNK_23                 = 0x00400000,
    MAP_FLAG_UNK_24                 = 0x00800000,
    MAP_FLAG_UNK_25                 = 0x01000000,
    MAP_FLAG_UNK_26                 = 0x02000000,
    MAP_FLAG_GARRISON               = 0x04000000,
    MAP_FLAG_UNK_28                 = 0x08000000,
    MAP_FLAG_UNK_29                 = 0x10000000,
    MAP_FLAG_UNK_30                 = 0x20000000
};

enum FactionTemplateFlags
{
    FACTION_TEMPLATE_ENEMY_SPAR             = 0x00000020,   // guessed, sparring with enemies?
    FACTION_TEMPLATE_FLAG_PVP               = 0x00000800,   // flagged for PvP
    FACTION_TEMPLATE_FLAG_CONTESTED_GUARD   = 0x00001000,   // faction will attack players that were involved in PvP combats
    FACTION_TEMPLATE_FLAG_HOSTILE_BY_DEFAULT= 0x00002000
};

enum FactionMasks
{
    FACTION_MASK_PLAYER   = 1,                              // any player
    FACTION_MASK_ALLIANCE = 2,                              // player or creature from alliance team
    FACTION_MASK_HORDE    = 4,                              // player or creature from horde team
    FACTION_MASK_MONSTER  = 8                               // aggressive creature from monster team
    // if none flags set then non-aggressive creature
};

enum InstanceTypes                                          // m_InstanceTypes (Map.db2)
{
    MAP_COMMON          = 0,                                // none
    MAP_INSTANCE        = 1,                                // party
    MAP_RAID            = 2,                                // raid
    MAP_BATTLEGROUND    = 3,                                // pvp
    MAP_ARENA           = 4,                                // arena
    MAP_SCENARIO        = 5                                 // Scenario
};

enum ScenarioFlags
{
    SCENARIO_FLAG_CHALLENGE             = 0x1,
    SCENARIO_FLAG_SUPRESS_STAGE_TEXT    = 0x2,
};

enum ScenarioStepFlags
{
    SCENARIO_STEP_FLAG_BONUS_OBJECTIVE = 0x1,
};


enum ItemEnchantmentType
{
    ITEM_ENCHANTMENT_TYPE_NONE                              = 0,
    ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL                      = 1,
    ITEM_ENCHANTMENT_TYPE_DAMAGE                            = 2,
    ITEM_ENCHANTMENT_TYPE_EQUIP_SPELL                       = 3,
    ITEM_ENCHANTMENT_TYPE_RESISTANCE                        = 4,
    ITEM_ENCHANTMENT_TYPE_STAT                              = 5,
    ITEM_ENCHANTMENT_TYPE_TOTEM                             = 6,
    ITEM_ENCHANTMENT_TYPE_USE_SPELL                         = 7,
    ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET                  = 8,
    ITEM_ENCHANTMENT_TYPE_ARTIFACT_POWER_BONUS_RANK_BY_TYPE = 9,
    ITEM_ENCHANTMENT_TYPE_ARTIFACT_POWER_BONUS_RANK_BY_ID   = 10,
    ITEM_ENCHANTMENT_TYPE_BONUS_LIST_ID                     = 11,
    ITEM_ENCHANTMENT_TYPE_BONUS_LIST_CURVE                  = 12,
    ITEM_ENCHANTMENT_TYPE_ARTIFACT_POWER_BONUS_RANK_PICKER  = 13
};

enum ItemLimitCategoryMode
{
    ITEM_LIMIT_CATEGORY_MODE_HAVE       = 0,                      // limit applied to amount items in inventory/bank
    ITEM_LIMIT_CATEGORY_MODE_EQUIP      = 1                       // limit applied to amount equipped items (including used gems)
};

enum ItemSetFlags
{
    ITEM_SET_FLAG_LEGACY_INACTIVE = 0x01,
};

#define ITEM_LEVEL_ROLL_LIMIT 850

/// From client
enum class ItemContext : uint8
{
    None                 = 0,
    DungeonNormal        = 1,
    DungeonHeroic        = 2,
    RaidNormal           = 3,
    RaidLfr              = 4,
    RaidHeroic           = 5,
    RaidMythic           = 6,
    PvpUnranked          = 7,
    PvPRanked            = 8, ///< old name 855
    ScenarioNormal       = 9,
    ScenarioHeroic       = 10,
    QuestReward          = 11,
    Store                = 12,
    TradeSkill           = 13,
    Vendor               = 14,
    BlackMarket          = 15,
    ChallengeMode        = 16,
    DungeonLevelUp1      = 17,
    DungeonLevelUp2      = 18,
    DungeonLevelUp3      = 19,
    DungeonLevelUp4      = 20,
    ForceNone            = 21,
    TimeWalker           = 22,
    DungeonMythic        = 23,
    PvPHonorReward       = 24,
    WorldQuest1          = 25, ///< old name 805
    WorldQuest2          = 26, ///< old name 810
    WorldQuest3          = 27, ///< old name 815
    WorldQuest4          = 28, ///< old name 820
    WorldQuest5          = 29, ///< old name 825
    WorldQuest6          = 30, ///< old name 830
    MissionReward1       = 33,
    MissionReward2       = 34,
    MissionReward3       = 35,
    ChallengeModeJackpot = 36, ///< old name 835
    WorldQuest7          = 37, ///< old name 840
    WorldQuest8          = 38,
    PvPRanked2           = 39,
    PvPRanked3           = 40, ///< old name 865
    PvPRanked4           = 42, ///< old name 845
    WorldQuest9          = 43,
    WorldQuest10         = 44, ///< old name 870
    PvPRanked5           = 45, ///< old name 875
    PvPRanked6           = 46, ///< old name 880
    PvPUnranked3         = 47,
    PvPUnranked4         = 48,
    PvPUnranked5         = 49,
    PvPUnranked6         = 50,
    PvPUnranked7         = 51,
    PvPRanked8           = 52,
    WorldQuest11         = 53,
    WorldQuest12         = 54,
    WorldQuest13         = 55,
    PvPRankedJackpot     = 56,
    TournamentRealm      = 57,

    Max_ItemContext,

    /// Please drop me
    Ilvl850         = 100,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl860         = 101,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl885         = 102,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl890         = 103,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl895         = 104,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl900         = 105,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl905         = 106,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl910         = 107,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl915         = 108,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl920         = 109,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl925         = 110,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl930         = 111,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl935         = 112,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl940         = 113,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl945         = 114,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl950         = 115,  ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
    Ilvl955         = 116   ///< CUSTOM, CORE INTERNAL ONLY, NOT VALID CLIENT SIDE / NOT EXIST IN DB2
};

enum ArtifactPowerFlag : uint8
{
    ARTIFACT_POWER_FLAG_GOLD                        = 0x01,
    ARTIFACT_POWER_FLAG_FIRST                       = 0x02,
    ARTIFACT_POWER_FLAG_FINAL                       = 0x04,
    ARTIFACT_POWER_FLAG_SCALES_WITH_NUM_POWERS      = 0x08,
    ARTIFACT_POWER_FLAG_DONT_COUNT_FIRST_BONUS_RANK = 0x10,
};

enum MountFlags
{
    MOUNT_FLAG_SELF_MOUNT               = 0x2,                    // Player becomes the mount himself
    MOUNT_FLAG_CAN_PITCH                = 0x4,                    // client checks MOVEMENTFLAG2_FULL_SPEED_PITCHING
    MOUNT_FLAG_CAN_SWIM                 = 0x8                     // client checks MOVEMENTFLAG_SWIMMING
};

enum SpellProcsPerMinuteModType
{
    SPELL_PPM_MOD_HASTE         = 1,
    SPELL_PPM_MOD_CRIT          = 2,
    SPELL_PPM_MOD_CLASS         = 3,
    SPELL_PPM_MOD_SPEC          = 4,
    SPELL_PPM_MOD_RACE          = 5,
    SPELL_PPM_MOD_ITEM_LEVEL    = 6,
    SPELL_PPM_MOD_BATTLEGROUND  = 7
};

enum SpellShapeshiftFormFlags
{
    SHAPESHIFT_FORM_IS_NOT_A_SHAPESHIFT         = 0x0001,
    SHAPESHIFT_FORM_CANNOT_CANCEL               = 0x0002,   // player cannot cancel the aura giving this shapeshift
    SHAPESHIFT_FORM_CAN_INTERACT                = 0x0008,   // if the form does not have SHAPESHIFT_FORM_IS_NOT_A_SHAPESHIFT then this flag must be present to allow NPC interaction
    SHAPESHIFT_FORM_CAN_EQUIP_ITEMS             = 0x0040,   // if the form does not have SHAPESHIFT_FORM_IS_NOT_A_SHAPESHIFT then this flag allows equipping items without ItemFlags::USE_WHEN_SHAPESHIFTED
    SHAPESHIFT_FORM_CAN_USE_ITEMS               = 0x0080,   // if the form does not have SHAPESHIFT_FORM_IS_NOT_A_SHAPESHIFT then this flag allows using items without ItemFlags::USE_WHEN_SHAPESHIFTED
    SHAPESHIFT_FORM_CAN_AUTO_UNSHIFT            = 0x0100,   // clientside
    SHAPESHIFT_FORM_PREVENT_LFG_TELEPORT        = 0x0200,
    SHAPESHIFT_FORM_PREVENT_USING_OWN_SKILLS    = 0x0400,   // prevents using spells that don't have any shapeshift requirement
    SHAPESHIFT_FORM_PREVENT_EMOTE_SOUNDS        = 0x1000
};

enum TotemCategoryType
{
    TOTEM_CATEGORY_TYPE_KNIFE           = 1,
    TOTEM_CATEGORY_TYPE_TOTEM           = 2,
    TOTEM_CATEGORY_TYPE_ROD             = 3,
    TOTEM_CATEGORY_TYPE_PICK            = 21,
    TOTEM_CATEGORY_TYPE_STONE           = 22,
    TOTEM_CATEGORY_TYPE_HAMMER          = 23,
    TOTEM_CATEGORY_TYPE_SPANNER         = 24
};

// SummonProperties.db2
enum SummonPropGroup
{
    SUMMON_PROP_GROUP_UNKNOWN1       = 0,                   // 1160 spells in 3.0.3
    SUMMON_PROP_GROUP_UNKNOWN2       = 1,                   // 861 spells in 3.0.3
    SUMMON_PROP_GROUP_PETS           = 2,                   // 52 spells in 3.0.3, pets mostly
    SUMMON_PROP_GROUP_CONTROLLABLE   = 3,                   // 13 spells in 3.0.3, mostly controllable
    SUMMON_PROP_GROUP_UNKNOWN3       = 4,                   // 86 spells in 3.0.3, taxi/mounts
    SUMMON_PROP_GROUP_UNKNOWN4       = 5                    // 86 spells in 3.0.3, taxi/mounts
};

// SummonProperties.db2
enum SummonPropType
{
    SUMMON_PROP_TYPE_UNKNOWN         = 0,                   // different summons, 1330 spells in 3.0.3
    SUMMON_PROP_TYPE_SUMMON          = 1,                   // generic summons, 49 spells in 3.0.3
    SUMMON_PROP_TYPE_GUARDIAN        = 2,                   // summon guardian, 393 spells in 3.0.3
    SUMMON_PROP_TYPE_ARMY            = 3,                   // summon army, 5 spells in 3.0.3
    SUMMON_PROP_TYPE_TOTEM           = 4,                   // summon totem, 169 spells in 3.0.3
    SUMMON_PROP_TYPE_CRITTER         = 5,                   // critter/minipet, 195 spells in 3.0.3
    SUMMON_PROP_TYPE_DK              = 6,                   // summon DRW/Ghoul, 2 spells in 3.0.3
    SUMMON_PROP_TYPE_BOMB            = 7,                   // summon bot/bomb, 4 spells in 3.0.3
    SUMMON_PROP_TYPE_PHASING         = 8,                   // something todo with DK prequest line, 2 spells in 3.0.3
    SUMMON_PROP_TYPE_SIEGE_VEH       = 9,                   // summon different vehicles, 14 spells in 3.0.3
    SUMMON_PROP_TYPE_DRAKE_VEH       = 10,                  // summon drake (vehicle), 3 spells
    SUMMON_PROP_TYPE_LIGHTWELL       = 11,                  // summon lightwell, 6 spells in 3.0.3
    SUMMON_PROP_TYPE_JEEVES          = 12,                  // summon Jeeves, 1 spell in 3.3.5a
    SUMMON_PROP_TYPE_LASHTAIL        = 13,                  // Lashtail Hatchling, 1 spell in 4.2.2
    SUMMON_PROP_TYPE_UNKNOWN_1       = 14,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_2       = 15,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_3       = 16,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_4       = 17,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_5       = 18,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_6       = 19,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_7       = 20,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_8       = 21,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_9       = 24,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_10      = 25,                  // unk
    SUMMON_PROP_TYPE_UNKNOWN_11      = 26                   // unk
};

// SummonProperties.db2
enum SummonPropFlags
{
    SUMMON_PROP_FLAG_NONE            = 0x00000000,          // 1342 spells in 3.0.3
    SUMMON_PROP_FLAG_UNK1            = 0x00000001,          // 75 spells in 3.0.3, something unfriendly
    SUMMON_PROP_FLAG_UNK2            = 0x00000002,          // 616 spells in 3.0.3, something friendly
    SUMMON_PROP_FLAG_UNK3            = 0x00000004,          // 22 spells in 3.0.3, no idea...
    SUMMON_PROP_FLAG_UNK4            = 0x00000008,          // 49 spells in 3.0.3, some mounts
    SUMMON_PROP_FLAG_PERSONAL_SPAWN  = 0x00000010,          // Personal Spawn (creature visible only by summoner) NYI
    SUMMON_PROP_FLAG_UNK6            = 0x00000020,          // 0 spells in 3.3.5, unused
    SUMMON_PROP_FLAG_UNK7            = 0x00000040,          // 12 spells in 3.0.3, no idea
    SUMMON_PROP_FLAG_UNK8            = 0x00000080,          // 4 spells in 3.0.3, no idea
    SUMMON_PROP_FLAG_UNK9            = 0x00000100,          // 51 spells in 3.0.3, no idea, many quest related
    SUMMON_PROP_FLAG_UNK10           = 0x00000200,          // 51 spells in 3.0.3, something defensive
    SUMMON_PROP_FLAG_UNK11           = 0x00000400,          // 3 spells, requires something near?
    SUMMON_PROP_FLAG_UNK12           = 0x00000800,          // 30 spells in 3.0.3, no idea
    SUMMON_PROP_FLAG_UNK13           = 0x00001000,          // Lightwell, Jeeves, Gnomish Alarm-o-bot, Build vehicles(wintergrasp)
    SUMMON_PROP_FLAG_UNK14           = 0x00002000,          // Guides, player follows
    SUMMON_PROP_FLAG_UNK15           = 0x00004000,          // Force of Nature, Shadowfiend, Feral Spirit, Summon Water Elemental
    SUMMON_PROP_FLAG_UNK16           = 0x00008000,          // Light/Dark Bullet, Soul/Fiery Consumption, Twisted Visage, Twilight Whelp. Phase related?
    SUMMON_PROP_FLAG_UNK17           = 0x00010000,
    SUMMON_PROP_FLAG_UNK18           = 0x00020000,
    SUMMON_PROP_FLAG_UNK19           = 0x00040000,
    SUMMON_PROP_FLAG_UNK20           = 0x00080000,
    SUMMON_PROP_FLAG_UNK21           = 0x00100000           // Totems
};

enum VehicleSeatFlags
{
    VEHICLE_SEAT_FLAG_HAS_LOWER_ANIM_FOR_ENTER                         = 0x00000001,
    VEHICLE_SEAT_FLAG_HAS_LOWER_ANIM_FOR_RIDE                          = 0x00000002,
    VEHICLE_SEAT_FLAG_UNK3                                             = 0x00000004,
    VEHICLE_SEAT_FLAG_SHOULD_USE_VEH_SEAT_EXIT_ANIM_ON_VOLUNTARY_EXIT  = 0x00000008,
    VEHICLE_SEAT_FLAG_UNK5                                             = 0x00000010,
    VEHICLE_SEAT_FLAG_UNK6                                             = 0x00000020,
    VEHICLE_SEAT_FLAG_UNK7                                             = 0x00000040,
    VEHICLE_SEAT_FLAG_UNK8                                             = 0x00000080,
    VEHICLE_SEAT_FLAG_UNK9                                             = 0x00000100,
    VEHICLE_SEAT_FLAG_HIDE_PASSENGER                                   = 0x00000200, // Passenger is hidden
    VEHICLE_SEAT_FLAG_ALLOW_TURNING                                    = 0x00000400, // needed for CGCamera__SyncFreeLookFacing
    VEHICLE_SEAT_FLAG_CAN_CONTROL                                      = 0x00000800, // Lua_UnitInVehicleControlSeat
    VEHICLE_SEAT_FLAG_CAN_CAST_MOUNT_SPELL                             = 0x00001000, // Can cast spells with SPELL_AURA_MOUNTED from seat (possibly 4.x only, 0 seats on 3.3.5a)
    VEHICLE_SEAT_FLAG_UNCONTROLLED                                     = 0x00002000, // can override !& VEHICLE_SEAT_FLAG_CAN_ENTER_OR_EXIT
    VEHICLE_SEAT_FLAG_CAN_ATTACK                                       = 0x00004000, // Can attack, cast spells and use items from vehicle
    VEHICLE_SEAT_FLAG_SHOULD_USE_VEH_SEAT_EXIT_ANIM_ON_FORCED_EXIT     = 0x00008000,
    VEHICLE_SEAT_FLAG_UNK17                                            = 0x00010000,
    VEHICLE_SEAT_FLAG_UNK18                                            = 0x00020000, // Needs research and support (28 vehicles): Allow entering vehicles while keeping specific permanent(?) auras that impose visuals (states like beeing under freeze/stun mechanic, emote state animations).
    VEHICLE_SEAT_FLAG_HAS_VEH_EXIT_ANIM_VOLUNTARY_EXIT                 = 0x00040000,
    VEHICLE_SEAT_FLAG_HAS_VEH_EXIT_ANIM_FORCED_EXIT                    = 0x00080000,
    VEHICLE_SEAT_FLAG_PASSENGER_NOT_SELECTABLE                         = 0x00100000,
    VEHICLE_SEAT_FLAG_UNK22                                            = 0x00200000,
    VEHICLE_SEAT_FLAG_REC_HAS_VEHICLE_ENTER_ANIM                       = 0x00400000,
    VEHICLE_SEAT_FLAG_IS_USING_VEHICLE_CONTROLS                        = 0x00800000, // Lua_IsUsingVehicleControls
    VEHICLE_SEAT_FLAG_ENABLE_VEHICLE_ZOOM                              = 0x01000000,
    VEHICLE_SEAT_FLAG_CAN_ENTER_OR_EXIT                                = 0x02000000, // Lua_CanExitVehicle - can enter and exit at free will
    VEHICLE_SEAT_FLAG_CAN_SWITCH                                       = 0x04000000, // Lua_CanSwitchVehicleSeats
    VEHICLE_SEAT_FLAG_HAS_START_WARITING_FOR_VEH_TRANSITION_ANIM_ENTER = 0x08000000,
    VEHICLE_SEAT_FLAG_HAS_START_WARITING_FOR_VEH_TRANSITION_ANIM_EXIT  = 0x10000000,
    VEHICLE_SEAT_FLAG_CAN_CAST                                         = 0x20000000, // Lua_UnitHasVehicleUI
    VEHICLE_SEAT_FLAG_UNK2                                             = 0x40000000, // checked in conjunction with 0x800 in CastSpell2
    VEHICLE_SEAT_FLAG_ALLOWS_INTERACTION                               = 0x80000000
};

enum VehicleSeatFlagsB
{
    VEHICLE_SEAT_FLAG_B_NONE                     = 0x00000000,
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED            = 0x00000002,
    VEHICLE_SEAT_FLAG_B_TARGETS_IN_RAIDUI        = 0x00000008,           // Lua_UnitTargetsVehicleInRaidUI
    VEHICLE_SEAT_FLAG_B_EJECTABLE                = 0x00000020,           // ejectable
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED_2          = 0x00000040,
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED_3          = 0x00000100,
    VEHICLE_SEAT_FLAG_B_USABLE_FORCED_4          = 0x02000000,
    VEHICLE_SEAT_FLAG_B_CAN_SWITCH               = 0x04000000,
    VEHICLE_SEAT_FLAG_B_VEHICLE_PLAYERFRAME_UI   = 0x80000000            // Lua_UnitHasVehiclePlayerFrameUI - actually checked for flagsb &~ 0x80000000
};

enum TaxiNodeFlags
{
    TAXI_NODE_FLAG_ALLIANCE           = 0x01,
    TAXI_NODE_FLAG_HORDE              = 0x02,
    TAXI_NODE_FLAG_UNK                = 0x04,
    TAXI_NODE_FLAG_UNK2               = 0x08,
    TAXI_NODE_FLAG_USE_FAVORITE_MOUNT = 0x10,
    TAXI_NODE_FLAG_ARGUS              = 0x20
};

enum TaxiPathNodeFlags
{
    TAXI_PATH_NODE_FLAG_TELEPORT    = 0x1,
    TAXI_PATH_NODE_FLAG_STOP        = 0x2
};

// CurrencyTypes.db2
enum CurrencyTypes
{
    /// @todo this currency's are deprecated we need to rework strongboxes and garrison gladiator sanctum to removed them properly
    CURRENCY_TYPE_HONOR_POINTS =0,
    CURRENCY_TYPE_CONQUEST_POINTS =1,

    CURRENCY_TYPE_BADGE_OF_JUSTICE                 = 42,    ///<
    CURRENCY_TYPE_DALARAN_JEWEL                    = 61,    ///< Jewelcrafting token WoTLK
    CURRENCY_TYPE_EPICUREAN                        = 81,    ///< Cook token WoTLK
    CURRENCY_TYPE_EMBLEM_OF_HEROISM                = 101,   ///<
    CURRENCY_TYPE_EMBLEM_OF_VALOR                  = 102,   ///<
    CURRENCY_TYPE_ARENA_POINT                      = 103,   ///<
    CURRENCY_TYPE_HONOR_POINT_DEPRECATED           = 104,   ///<
    CURRENCY_TYPE_AV_MARK_OF_HONOR                 = 121,   ///<
    CURRENCY_TYPE_AB_MARK_OF_HONOR                 = 122,   ///<
    CURRENCY_TYPE_EOS_MARK_OF_HONOR                = 123,   ///<
    CURRENCY_TYPE_SOA_MARK_OF_HONOR                = 124,   ///<
    CURRENCY_TYPE_WG_MARK_OF_HONOR                 = 125,   ///<
    CURRENCY_TYPE_WINTER_GRASP_MARK_OF_HONOR       = 126,   ///<
    CURRENCY_TYPE_STONE_KEPPER_SHARD               = 161,   ///<
    CURRENCY_TYPE_HONOR_POINT_DEPRECATED_2         = 181,   ///<
    CURRENCY_TYPE_VENTURE_COIN                     = 201,   ///<
    CURRENCY_TYPE_EMBLEM_OF_CONQUEST               = 221,   ///<
    CURRENCY_TYPE_CHAMPION_SEAL                    = 241,   ///< Argent tournament token
    CURRENCY_TYPE_EMBLEM_OF_TRIUMPH                = 301,   ///<
    CURRENCY_TYPE_IOC_MARK_OF_HONOR                = 321,   ///<
    CURRENCY_TYPE_EMBLEM_OF_FROST                  = 341,   ///<
    CURRENCY_TYPE_ILLUSTROUS_JEWEL                 = 361,   ///< Jewelcrafting token Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_DWARF                = 384,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_TROLL                = 385,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_TOL_BARAD                        = 391,   ///< Battleground Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_FOSSIL               = 393,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_NIGHT_ELF            = 394,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_JUSTICE_POINTS                   = 395,   ///< PvE
    CURRENCY_TYPE_VALOR_POINTS                     = 396,   ///< PvE
    CURRENCY_TYPE_ARCHAEOLOGY_ORC                  = 397,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_DRAENEI              = 398,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_VRYKUL               = 399,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_NERUBIAN             = 400,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_ARCHAEOLOGY_TOLVIR               = 401,   ///< ARCHAEOLOGY Cataclysm
    CURRENCY_TYPE_IRONPAW                          = 402,   ///< Cook token MoP
    CURRENCY_TYPE_WORLD_TREE                       = 416,   ///< 4.2 token Molten front
    CURRENCY_TYPE_CONQUEST_META_ARENA_BG           = 483,   ///< PvP
    CURRENCY_TYPE_CONQUEST_META_RBG                = 484,   ///< Deprecated since WoD (merge of arena/rbg weekcap)
    CURRENCY_TYPE_DARKMOON_TICKET                  = 515,   ///< Darkmoon fair
    CURRENCY_TYPE_MOTE_OF_DARKNESS                 = 614,   ///< 4.3.4 token Dragon soul
    CURRENCY_TYPE_CORRUPTED_ESSENCE                = 615,   ///< 4.3.4 Deathwing token
    CURRENCY_TYPE_ARCHAEOLOGY_PANDAREN             = 676,   ///< ARCHAEOLOGY MoP
    CURRENCY_TYPE_ARCHAEOLOGY_MOGU                 = 677,   ///< ARCHAEOLOGY MoP
    CURRENCY_TYPE_CONQUEST_RANDOM_BG_META          = 692,   ///<
    CURRENCY_TYPE_ELDER_CHARM_GOOD_FORTUNE         = 697,   ///< LFR roll chance MoP
    CURRENCY_TYPE_ZEN_JEWEL                        = 698,   ///< Jewelcrafting token MoP NYI
    CURRENCY_TYPE_LESSER_CHARM_GOOD_FORTUNE        = 738,   ///< LFR roll chance MoP
    CURRENCY_TYPE_MOGU_RUNE_FATE                   = 752,   ///< roll chance token for T15 boss
    CURRENCY_TYPE_ARCHAEOLOGY_MANTID               = 754,   ///< ARCHAEOLOGY MoP
    CURRENCY_TYPE_WARFORGED_SEAL                   = 776,   ///< roll chance token for T16 boss
    CURRENCY_TYPE_TIMELESS_COIN                    = 777,   ///< timeless isle token
    CURRENCY_TYPE_BLOODY_COIN                      = 789,   ///< timeless isle token
    CURRENCY_TYPE_CONQUEST_META_ASHRAN             = 692,   ///< Deprecated CURRENCY_TYPE_CONQUEST_META_RANDOM_BG, we use it for Ashran
    CURRENCY_TYPE_BLACK_IRON_FRAGEMENT             = 810,   ///<
    CURRENCY_TYPE_DRAENOR_CLANS_ARCHAEOLOGY        = 821,   ///< ARCHAEOLOGY WoD
    CURRENCY_TYPE_APEXIS_CRYSTAL                   = 823,   ///<
    CURRENCY_TYPE_GARRISON_RESSOURCES              = 824,   ///< Garrison currency
    CURRENCY_TYPE_OGRE_ARCHAEOLOGY_FRAGEMENT       = 828,   ///< ARCHAEOLOGY WoD
    CURRENCY_TYPE_ARAKKOA_ARCHAEOLOGY              = 829,   ///< ARCHAEOLOGY WoD
    CURRENCY_TYPE_UNUSED                           = 830,   ///<
    CURRENCY_TYPE_UNUSED_2                         = 897,   ///<
    CURRENCY_TYPE_SECRET_OF_DRAENOR_ALCHEMY        = 910,   ///< Professions
    CURRENCY_TYPE_ARTIFACT_FRAGEMENT               = 944,   ///<
    CURRENCY_TYPE_DINGY_IRON_COINS                 = 980,   ///<
    CURRENCY_TYPE_SEAL_OF_TEMPERED_FATE            = 994,   ///<
    CURRENCY_TYPE_SECRET_OF_DRAENOR_TAILORING      = 999,   ///< Professions
    CURRENCY_TYPE_SECRET_OF_DRAENOR_JEWELCRAFTING  = 1008,  ///< Professions
    CURRENCY_TYPE_SECRET_OF_DRAENOR_LEATHERWORKING = 1017,  ///< Professions
    CURRENCY_TYPE_SECRET_OF_DRAENOR_BLACKSMITHING  = 1020,  ///< Professions
    CURRENCY_TYPE_OIL                              = 1101,  ///<
    CURRENCY_TYPE_SEAL_OF_INEVITABLE_FATE          = 1129,  ///<
    CURRENCY_TYPE_SLIGHTLESS_EYE                   = 1149,  ///<
    CURRENCY_TYPE_SHADOWY_COINS                    = 1154,  ///<
    CURRENCY_TYPE_ANCIEN_MANA                      = 1155,  ///<
    CURRENCY_TYPE_TIMEWARPED_BADGE                 = 1166,  ///< Timewalking currency
    CURRENCY_TYPE_ARTIFACT_KNOWLEDGE               = 1171,  ///<
    CURRENCY_TYPE_HIGHBORNE_ARCHAEOLOGY            = 1172,  ///<
    CURRENCY_TYPE_HIGHMOUNTAIN_ARCHAEOLOGY         = 1173,  ///<
    CURRENCY_TYPE_DEMONIC_ARCHAEOLOGY              = 1174,  ///<
    CURRENCY_TYPE_VALOR                            = 1191,  ///<
    CURRENCY_TYPE_ORDER_RESSOURCES                 = 1220,  ///<
    CURRENCY_TYPE_NETHERSHARD                      = 1226,  ///<
    CURRENCY_TYPE_TIMEWORN_ARTIFACT                = 1268,  ///<
    CURRENCY_TYPE_SEAL_OF_BROKEN_FATE              = 1273,  ///<
    CURRENCY_TYPE_CURIOUS_COIN                     = 1275,  ///<
    CURRENCY_TYPE_BRAWLER_GOLD                     = 1299,  ///<
    CURRENCY_TYPE_LINGERING_SOUL_FRAGMENT          = 1314,  ///< Illiand questline
    CURRENCY_TYPE_HORDE_QIRAJ_COMMENDATION         = 1324,  ///< World event Qiraj opening
    CURRENCY_TYPE_ALLIANCE_QIRAJ_COMMENDATION      = 1325,  ///< World event Qiraj opening
    CURRENCY_TYPE_LEGIONFALL_WAR_SUPLIES           = 1342,  ///< Used for Broken shore war effort
    CURRENCY_TYPE_LEGIONFALL_BUILDING_MAGE_TOWER   = 1347,  ///< Personal Tracker (hidden)
    CURRENCY_TYPE_LEGIONFALL_BUILDING_COMMAND_TOWER = 1349, ///< Personal Tracker (hidden)
    CURRENCY_TYPE_LEGIONFALL_BUILDING_NETHER_TOWER  = 1350, ///< Personal Tracker (hidden)
    CURRENCY_TYPE_FELESSENCE                        = 1355, ///< Unused ?
    CURRENCY_TYPE_ECHOES_OF_BATTLE                  = 1356, ///< Used to purchase powerful PvP armor. At the end of the season, this currency converts to gold.
    CURRENCY_TYPE_ECHOES_OF_DOMINATION              = 1357, ///< Used to purchase elite PvP armor. At the end of the season, this currency converts to gold.
    CURRENCY_TYPE_TRIAL_OF_STYLE                    = 1379, ///< Token used in Tmog event
    CURRENCY_TYPE_COIN_OF_AIR                       = 1416, ///< 7.2.0 rogue token
    CURRENCY_TYPE_WRITHING_ESSENCE                  = 1501, ///< Used to upgrade Legion Legendary items to item level 970.
    CURRENCY_TYPE_ARGUS_WAYSTONE                    = 1506, ///< Unused, was never implemented in live realms
    CURRENCY_TYPE_VEILED_ARGUNITE                   = 1508, ///< Argus currency, looted on rare/chest/invasion and quest, extended cost for 910 item token
    CURRENCY_TYPE_WAKENING_ESSENCE                  = 1533, ///< Used to upgrade Legion Legendary items to item level 1000.

};

enum WorldMapTransformsFlags
{
    WORLD_MAP_TRANSFORMS_FLAG_DUNGEON   = 0x04
};

/// Used for CRITERIA_CONDITION_LEGACY_RAID_TYPE
enum class CriteriaLegacyRaidType : uint8
{
    Normal10    = 0,
    Normal25    = 1,
    Heroic10    = 2,
    Heroic25    = 3,
    None        = 255     ///< Custom ID, internal to server
};

/// Flags of PrevQuestLogic (PlayerConditionEntry)
namespace PrevQuestLogicFlags
{
    enum
    {
        TrackingQuest = 0x10000
    };
}

enum class PackageItemRewardType : uint8
{
    SpecializationReward = 0,
    ClassReward = 1,
    DefaultHiddenReward = 2,
    NoRequire = 3
};

#endif
