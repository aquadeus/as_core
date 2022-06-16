////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ITEMPROTOTYPE_H
#define _ITEMPROTOTYPE_H

#include "Common.h"
#include "SharedDefines.h"
#include "DB2Stores.h"
#include <bitset>

enum class ItemModType : int32
{
    MANA                                = 0,
    HEALTH                              = 1,
    ENDURANCE                           = 2,
    AGILITY                             = 3,
    STRENGTH                            = 4,
    INTELLECT                           = 5,
    SPIRIT_UNUSED                       = 6,
    STAMINA                             = 7,
    ENERGY                              = 8,
    RAGE                                = 9,
    FOCUS                               = 10,
    WEAPON_SKILL_RATING_OBSOLETE        = 11,
    DEFENSE_SKILL_RATING                = 12,
    DODGE_RATING                        = 13,
    PARRY_RATING                        = 14,
    BLOCK_RATING                        = 15,
    HIT_MELEE_RATING                    = 16,
    HIT_RANGED_RATING                   = 17,
    HIT_SPELL_RATING                    = 18,
    CRIT_MELEE_RATING                   = 19,
    CRIT_RANGED_RATING                  = 20,
    CRIT_SPELL_RATING                   = 21,
    HIT_TAKEN_MELEE_RATING_OBSOLETE     = 22,
    HIT_TAKEN_RANGED_RATING_OBSOLETE    = 23,
    HIT_TAKEN_SPELL_RATING_OBSOLETE     = 24,
    CRIT_TAKEN_MELEE_RATING_OBSOLETE    = 25,
    CRIT_TAKEN_RANGED_RATING_OBSOLETE   = 26,
    CRIT_TAKEN_SPELL_RATING_OBSOLETE    = 27,
    HASTE_MELEE_RATING_OBSOLETE         = 28,
    HASTE_RANGED_RATING_OBSOLETE        = 29,
    HASTE_SPELL_RATING_OBSOLETE         = 30,
    HIT_RATING                          = 31,
    CRIT_RATING                         = 32,
    HIT_TAKEN_RATING_OBSOLETE           = 33,
    CRIT_TAKEN_RATING_OBSOLETE          = 34,
    RESILIENCE_RATING                   = 35,
    HASTE_RATING                        = 36,
    EXPERTISE_RATING                    = 37,
    ATTACK_POWER                        = 38,
    RANGED_ATTACK_POWER                 = 39,
    VERSATILITY                         = 40,
    SPELL_HEALING_DONE                  = 41,
    SPELL_DAMAGE_DONE                   = 42,
    MANA_REGENERATION                   = 43,
    UNUSED                              = 44,
    SPELL_POWER                         = 45,
    HEALTH_REGEN                        = 46,
    SPELL_PENETRATION                   = 47,
    BLOCK_VALUE_OBSOLETE                = 48,
    MASTERY_RATING                      = 49,
    EXTRA_ARMOR                         = 50,
    FIRE_RESISTANCE                     = 51,
    FROST_RESISTANCE                    = 52,
    HOLY_RESISTANCE                     = 53,
    SHADOW_RESISTANCE                   = 54,
    NATURE_RESISTANCE                   = 55,
    ARCANE_RESISTANCE                   = 56,
    PVP_POWER                           = 57,
    COMBAT_RATING_UNUSED_0              = 58,
    COMBAT_RATING_UNUSED_2              = 59,
    COMBAT_RATING_UNUSED_3              = 60,
    COMBAT_RATING_SPEED                 = 61,
    COMBAT_RATING_LIFESTEAL             = 62,
    COMBAT_RATING_AVOIDANCE             = 63,
    COMBAT_RATING_STURDINESS            = 64,
    COMBAT_RATING_UNUSED_7              = 65,
    COMBAT_RATING_UNUSED_27             = 66,
    COMBAT_RATING_UNUSED_9              = 67,
    COMBAT_RATING_UNUSED_10             = 68,
    COMBAT_RATING_UNUSED_11             = 69,
    COMBAT_RATING_UNUSED_12             = 70,
    AGILITY_OR_STRENGTH_OR_INTELLECT    = 71,
    AGILITY_OR_STRENGTH                 = 72,
    AGILITY_OR_INTELLECT                = 73,
    STRENGTH_OR_INTELLECT               = 74,

    MAX_ITEM_MOD
};

enum ItemSpelltriggerType
{
    ITEM_SPELLTRIGGER_ON_USE          = 0,                  // use after equip cooldown
    ITEM_SPELLTRIGGER_ON_EQUIP        = 1,
    ITEM_SPELLTRIGGER_CHANCE_ON_HIT   = 2,
    ITEM_SPELLTRIGGER_SOULSTONE       = 4,
    /*
     * ItemSpelltriggerType 5 might have changed on 2.4.3/3.0.3: Such auras
     * will be applied on item pickup and removed on item loss - maybe on the
     * other hand the item is destroyed if the aura is removed ("removed on
     * death" of spell 57348 makes me think so)
     */
    ITEM_SPELLTRIGGER_ON_OBTAIN       = 5,
    ITEM_SPELLTRIGGER_LEARN_SPELL_ID  = 6,                  // used in item_template.spell_2 with spell_id with SPELL_GENERIC_LEARN in spell_1

    MAX_ITEM_SPELLTRIGGER
};

enum ItemBondingType
{
    NO_BIND                                     = 0,
    BIND_WHEN_PICKED_UP                         = 1,
    BIND_WHEN_EQUIPED                           = 2,
    BIND_WHEN_USE                               = 3,
    BIND_QUEST_ITEM                             = 4,
    BIND_QUEST_ITEM1                            = 5         // not used in game
};

#define MAX_BIND_TYPE                             6

enum ItemFieldFlags
{
    ITEM_FIELD_FLAG_SOULBOUND     = 0x00000001, // Item is soulbound and cannot be traded <<--
    ITEM_FIELD_FLAG_TRANSLATED    = 0x00000002, // Item text will not read as garbage when player does not know the language
    ITEM_FIELD_FLAG_UNLOCKED      = 0x00000004, // Item had lock but can be opened now
    ITEM_FIELD_FLAG_WRAPPED       = 0x00000008, // Item is wrapped and contains another item
    ITEM_FIELD_FLAG_DISABLE       = 0x00000010, // Item is disable (red filter)
    ITEM_FIELD_FLAG_UNK3          = 0x00000020, // ?
    ITEM_FIELD_FLAG_UNK4          = 0x00000040, // ?
    ITEM_FIELD_FLAG_UNK5          = 0x00000080, // ?
    ITEM_FIELD_FLAG_BOP_TRADEABLE = 0x00000100, // Allows trading soulbound items
    ITEM_FIELD_FLAG_READABLE      = 0x00000200, // Opens text page when right clicked
    ITEM_FIELD_FLAG_UNK6          = 0x00000400, // ?
    ITEM_FIELD_FLAG_UNK7          = 0x00000800, // ?
    ITEM_FIELD_FLAG_REFUNDABLE    = 0x00001000, // Item can be returned to vendor for its original cost (extended cost)
    ITEM_FIELD_FLAG_UNK8          = 0x00002000, // ?
    ITEM_FIELD_FLAG_UNK9          = 0x00004000, // ?
    ITEM_FIELD_FLAG_UNK10         = 0x00008000, // ?
    ITEM_FIELD_FLAG_UNK11         = 0x00010000, // ?
    ITEM_FIELD_FLAG_UNK12         = 0x00020000, // ?
    ITEM_FIELD_FLAG_UNK13         = 0x00040000, // ?
    ITEM_FIELD_FLAG_CHILD         = 0x00080000, // Item needs to have creator specified - locks the items together
    ITEM_FIELD_FLAG_UNK15         = 0x00100000, // ?
    ITEM_FIELD_FLAG_UNK16         = 0x00200000, // ?
    ITEM_FLAG_NO_DURABILITY_LOSS  = 0x00400000, ///< Item can't suffer from durability loss
    ITEM_FIELD_FLAG_UNK18         = 0x00800000, // ?
    ITEM_FIELD_FLAG_UNK19         = 0x01000000, // ?
    ITEM_FIELD_FLAG_UNK20         = 0x02000000, // ?
    ITEM_FIELD_FLAG_UNK21         = 0x04000000, // ?
    ITEM_FIELD_FLAG_UNK22         = 0x08000000, // ?
    ITEM_FIELD_FLAG_UNK23         = 0x10000000, // ?
    ITEM_FIELD_FLAG_UNK24         = 0x20000000, // ?
    ITEM_FIELD_FLAG_UNK25         = 0x40000000, // ?
    ITEM_FIELD_FLAG_UNK26         = 0x80000000, // ?

    ITEM_FIELD_FLAG_MAIL_TEXT_MASK = ITEM_FIELD_FLAG_READABLE | ITEM_FIELD_FLAG_UNK13
};

enum class ItemFlags : uint32
{
    NO_PICKUP                                   = 0x00000001, // NYI
    CONJURED                                    = 0x00000002, // Conjured item
    HAS_LOOT                                    = 0x00000004, // Item can be right clicked to open for loot
    HEROIC_TOOLTIP                              = 0x00000008, // Makes green "Heroic" text appear on item
    DEPRECATED                                  = 0x00000010, // Cannot equip or use
    NO_USER_DESTROY                             = 0x00000020, // Item can not be destroyed, except by using spell (item can be reagent for spell)
    PLAYERCAST                                  = 0x00000040, // NYI
    NO_EQUIP_COOLDOWN                           = 0x00000080, // No default 30 seconds cooldown when equipped
    MULTILOOTQUEST                              = 0x00000100, // NYI
    WRAPPER                                     = 0x00000200, // Item can wrap other items
    USES_RESOURCES                              = 0x00000400, // NYI
    MULTI_DROP                                  = 0x00000800, // Looting this item does not remove it from available loot
    ITEMPURCHASE_RECORD                         = 0x00001000, // Item can be returned to vendor for its original cost (extended cost)
    PETITION                                    = 0x00002000, // Item is guild or arena charter
    HAS_TEXT                                    = 0x00004000, // Only readable items have this (but not all)
    NO_DISENCHANT                               = 0x00008000,
    REAL_DURATION                               = 0x00010000, // NYI
    NO_CREATOR                                  = 0x00020000, // NYI
    IS_PROSPECTABLE                             = 0x00040000, // Item can be prospected
    UNIQUE_EQUIPPABLE                           = 0x00080000, // You can only equip one of these
    IGNORE_FOR_AURAS                            = 0x00100000, // NYI
    IGNORE_DEFAULT_ARENA_RESTRICTIONS           = 0x00200000, // Item can be used during arena match
    NO_DURABILITY_LOSS                          = 0x00400000, // Some Thrown weapons have it (and only Thrown) but not all
    USE_WHEN_SHAPESHIFTED                       = 0x00800000, // Item can be used in shapeshift forms
    HAS_QUEST_GLOW                              = 0x01000000, // NYI
    HIDE_UNUSABLE_RECIPE                        = 0x02000000, // Profession recipes: can only be looted if you meet requirements and don't already know it
    NOT_USEABLE_IN_ARENA                        = 0x04000000, // Item cannot be used in arena
    IS_BOUND_TO_ACCOUNT                         = 0x08000000, // Item binds to account and can be sent only to your own characters
    NO_REAGENT_COST                             = 0x10000000, // Spell is cast with triggered flag
    IS_MILLABLE                                 = 0x20000000, // Item can be milled
    REPORT_TO_GUILD_CHAT                        = 0x40000000, // Broadcasts the item in guild
    NO_PROGRESSIVE_LOOT                         = 0x80000000  // bound item that can be traded
};

enum class ItemFlagsEX : uint32
{
    HORDE                                       = 0x00000001,
    ALLIANCE                                    = 0x00000002,
    DONT_IGNORE_BUY_PRICE                       = 0x00000004, // when item uses extended cost, gold is also required
    CLASSIFY_AS_CASTER                          = 0x00000008,
    CLASSIFY_AS_PHYSICAL                        = 0x00000010,
    EVERYONE_CAN_ROLL_NEED                      = 0x00000020,
    NO_TRADE_BIND_ON_ACQUIRE                    = 0x00000040,
    CAN_TRADE_BIND_ON_ACQUIRE                   = 0x00000080,
    CAN_ONLY_ROLL_GREED                         = 0x00000100,
    CASTER_WEAPON                               = 0x00000200,
    DELETE_ON_LOGIN                             = 0x00000400,
    INTERNAL_ITEM                               = 0x00000800,
    NO_VENDOR_VALUE                             = 0x00001000,
    SHOW_BEFORE_DISCOVERED                      = 0x00002000,
    OVERRIDE_GOLD_COST                          = 0x00004000,
    IGNORE_DEFAULT_RATED_BG_RESTRICTIONS        = 0x00008000,
    NOT_USABLE_IN_RATED_BG                      = 0x00010000,
    BNET_ACCOUNT_TRADE_OK                       = 0x00020000,
    CONFIRM_BEFORE_USE                          = 0x00040000,
    REEVALUATE_BONDING_ON_TRANSFORM             = 0x00080000,
    NO_TRANSFORM_ON_CHARGE_DEPLETION            = 0x00100000,
    NO_ALTER_ITEM_VISUAL                        = 0x00200000,
    NO_SOURCE_FOR_ITEM_VISUAL                   = 0x00400000,
    IGNORE_QUALITY_FOR_ITEM_VISUAL_SOURCE       = 0x00800000,
    NO_DURABILITY                               = 0x01000000,
    ROLE_TANK                                   = 0x02000000,
    ROLE_HEALER                                 = 0x04000000,
    ROLE_DAMAGE                                 = 0x08000000,
    CAN_DROP_IN_CHALLENGE_MODE                  = 0x10000000,
    NEVER_STACK_IN_LOOT_UI                      = 0x20000000,
    DISENCHANT_TO_LOOT_TABLE                    = 0x40000000,
    USED_IN_A_TRADESKILL                        = 0x80000000
};

enum class ItemFlagsEX2 : uint32
{
    DONT_DESTROY_ON_QUEST_ACCEPT                = 0x00000001,
    ITEM_CAN_BE_UPGRADED                        = 0x00000002,
    UPGRADE_FROM_ITEM_OVERRIDES_DROP_UPGRADE    = 0x00000004,
    ALWAYS_FFA_IN_LOOT                          = 0x00000008,
    HIDE_UPGRADE_LEVELS_IF_NOT_UPGRADED         = 0x00000010,
    UPDATE_INTERACTIONS                         = 0x00000020,
    UPDATE_DOESNT_LEAVE_PROGRESSIVE_WIN_HISTORY = 0x00000040,
    IGNORE_ITEM_HISTORY_TRACKER                 = 0x00000080, // Ignore item level adjustments from PLAYER_FIELD_ITEM_LEVEL_DELTA
    IGNORE_ITEM_LEVEL_CAP_IN_PVP                = 0x00000100,
    DISPLAY_AS_HEIRLOOM                         = 0x00000200, // Item appears as having heirloom quality ingame regardless of its real quality (does not affect stat calculation)
    SKIP_USE_CHECK_ON_PICKUP                    = 0x00000400,
    OBSOLETE                                    = 0x00000800,
    DONT_DISPLAY_IN_GUILD_NEWS                  = 0x00001000, // Item is not included in the guild news panel
    PVP_TOURNAMENT_GEAR                         = 0x00002000,
    REQUIRES_STACK_CHANGE_LOG                   = 0x00004000,
    UNUSED_FLAG                                 = 0x00008000,
    HIDE_NAME_SUFFIX                            = 0x00010000,
    PUSH_LOOT                                   = 0x00020000,
    DONT_REPORT_LOOT_LOG_TO_PARTY               = 0x00040000,
    ALWAYS_ALLOW_DUAL_WIELD                     = 0x00080000,
    OBLITERATABLE                               = 0x00100000,
    ACTS_AS_TRANSMOG_HIDDEN_VISUAL_OPTION       = 0x00200000,
    EXPIRE_ON_WEEKLY_RESET                      = 0x00400000,
    DOESNT_SHOW_UP_IN_TRANSMOG_UNTIL_COLLECTED  = 0x00800000,
    CAN_STORE_ENCHANTS                          = 0x01000000
};

enum class ItemFlagsCustom : uint32
{
    ITEM_FLAGS_CU_UNUSED                = 0x0001,
    ITEM_FLAGS_CU_IGNORE_QUEST_STATUS   = 0x0002,   ///< No quest status will be checked when this item drops
    ITEM_FLAGS_CU_FOLLOW_LOOT_RULES     = 0x0004,   ///< Item will always follow group/master/need before greed looting rules
    ITEM_FLAGS_CU_CANT_BE_SELL          = 0x0008,   ///< Item can't be sell
    ITEM_FLAGS_CU_BYPASS_VENDOR_FILTER  = 0x0010    ///< Bypass vendor filter (always shown)
};

enum CurrencyCategory
{
    CURRENCY_CATEGORY_MISC              = 1,   ///< Miscellaneous
    CURRENCY_CATEGORY_PVP               = 2,   ///< Player vs. Player
    CURRENCY_CATEGORY_UNUSED            = 3,   ///< Unused
    CURRENCY_CATEGORY_CLASSIC           = 4,   ///< Classic
    CURRENCY_CATEGORY_WOTLK             = 21,  ///< Wrath of the Lich King
    CURRENCY_CATEGORY_DUNGEON_AND_RAID  = 22,  ///< Dungeon and Raid
    CURRENCY_CATEGORY_BC                = 23,  ///< Burning Crusade
    CURRENCY_CATEGORY_TEST              = 41,  ///< Test
    CURRENCY_CATEGORY_CATACLYSM         = 81,  ///< Cataclysm
    CURRENCY_CATEGORY_ARCHAEOLOGY       = 82,  ///< Archaeology
    CURRENCY_CATEGORY_META_CONQUEST     = 89,  ///< Meta
    CURRENCY_CATEGORY_MOP               = 133, ///< Mists of Pandaria
    CURRENCY_CATEGORY_WOD               = 137, ///< Warlords of Draenor
    CURRENCY_CATEGORY_LEGION            = 141, ///< Legion
    CURRENCY_CATEGORY_HIDDEN            = 142  ///< Hidden
};

enum ItemVendorType
{
    ITEM_VENDOR_TYPE_NONE     = 0,
    ITEM_VENDOR_TYPE_ITEM     = 1,
    ITEM_VENDOR_TYPE_CURRENCY = 2,
};

enum BAG_FAMILY_MASK
{
    BAG_FAMILY_MASK_NONE                        = 0x00000000,
    BAG_FAMILY_MASK_ARROWS                      = 0x00000001,
    BAG_FAMILY_MASK_BULLETS                     = 0x00000002,
    BAG_FAMILY_MASK_SOUL_SHARDS                 = 0x00000004,
    BAG_FAMILY_MASK_LEATHERWORKING_SUPPLIES     = 0x00000008,
    BAG_FAMILY_MASK_INSCRIPTION_SUPPLIES        = 0x00000010,
    BAG_FAMILY_MASK_HERBS                       = 0x00000020,
    BAG_FAMILY_MASK_ENCHANTING_SUPPLIES         = 0x00000040,
    BAG_FAMILY_MASK_ENGINEERING_SUPPLIES        = 0x00000080,
    BAG_FAMILY_MASK_KEYS                        = 0x00000100,
    BAG_FAMILY_MASK_GEMS                        = 0x00000200,
    BAG_FAMILY_MASK_MINING_SUPPLIES             = 0x00000400,
    BAG_FAMILY_MASK_SOULBOUND_EQUIPMENT         = 0x00000800,
    BAG_FAMILY_MASK_VANITY_PETS                 = 0x00001000,
    BAG_FAMILY_MASK_CURRENCY_TOKENS             = 0x00002000,
    BAG_FAMILY_MASK_QUEST_ITEMS                 = 0x00004000,
    BAG_FAMILY_MASK_FISHING_SUPPLIES            = 0x00008000,
    BAG_FAMILY_MASK_COOKING_SUPPLIES            = 0x00010000,
    BAG_FAMILY_MASK_TOYS                        = 0x00020000,
    BAG_FAMILY_MASK_ARCHAEOLOGY                 = 0x00040000,
    BAG_FAMILY_MASK_ALCHEMY                     = 0x00080000,
    BAG_FAMILY_MASK_BLACKSMITHING               = 0x00100000,
    BAG_FAMILY_MASK_FIRSTAID                    = 0x00200000,
    BAG_FAMILY_MASK_JEWELCRAFTING               = 0x00400000,
    BAG_FAMILY_MASK_SKINING                     = 0x00800000,
    BAG_FAMILY_MASK_TAILORING                   = 0x01000000
};

enum SocketColor
{
    SOCKET_COLOR_META                           = 0x00001,
    SOCKET_COLOR_RED                            = 0x00002,
    SOCKET_COLOR_YELLOW                         = 0x00004,
    SOCKET_COLOR_BLUE                           = 0x00008,
    SOCKET_COLOR_HYDRAULIC                      = 0x00010, // not used
    SOCKET_COLOR_COGWHEEL                       = 0x00020,
    SOCKET_COLOR_PRISMATIC                      = 0x0000E,
    SOCKET_COLOR_RELIC_IRON                     = 0x00040,
    SOCKET_COLOR_RELIC_BLOOD                    = 0x00080,
    SOCKET_COLOR_RELIC_SHADOW                   = 0x00100,
    SOCKET_COLOR_RELIC_FEL                      = 0x00200,
    SOCKET_COLOR_RELIC_ARCANE                   = 0x00400,
    SOCKET_COLOR_RELIC_FROST                    = 0x00800,
    SOCKET_COLOR_RELIC_FIRE                     = 0x01000,
    SOCKET_COLOR_RELIC_WATER                    = 0x02000,
    SOCKET_COLOR_RELIC_LIFE                     = 0x04000,
    SOCKET_COLOR_RELIC_WIND                     = 0x08000,
    SOCKET_COLOR_RELIC_HOLY                     = 0x10000
};

extern uint32 const SocketColorToGemTypeMask[19];

enum InventoryType
{
    INVTYPE_NON_EQUIP                           = 0,
    INVTYPE_HEAD                                = 1,
    INVTYPE_NECK                                = 2,
    INVTYPE_SHOULDERS                           = 3,
    INVTYPE_BODY                                = 4,
    INVTYPE_CHEST                               = 5,
    INVTYPE_WAIST                               = 6,
    INVTYPE_LEGS                                = 7,
    INVTYPE_FEET                                = 8,
    INVTYPE_WRISTS                              = 9,
    INVTYPE_HANDS                               = 10,
    INVTYPE_FINGER                              = 11,
    INVTYPE_TRINKET                             = 12,
    INVTYPE_WEAPON                              = 13,
    INVTYPE_SHIELD                              = 14,
    INVTYPE_RANGED                              = 15,
    INVTYPE_CLOAK                               = 16,
    INVTYPE_2HWEAPON                            = 17,
    INVTYPE_BAG                                 = 18,
    INVTYPE_TABARD                              = 19,
    INVTYPE_ROBE                                = 20,
    INVTYPE_WEAPONMAINHAND                      = 21,
    INVTYPE_WEAPONOFFHAND                       = 22,
    INVTYPE_HOLDABLE                            = 23,
    INVTYPE_AMMO                                = 24,
    INVTYPE_THROWN                              = 25,
    INVTYPE_RANGEDRIGHT                         = 26,
    INVTYPE_QUIVER                              = 27,
    INVTYPE_RELIC                               = 28,

    MAX_INVTYPE
};

enum ItemClass
{
    ITEM_CLASS_CONSUMABLE                       = 0,
    ITEM_CLASS_CONTAINER                        = 1,
    ITEM_CLASS_WEAPON                           = 2,
    ITEM_CLASS_GEM                              = 3,
    ITEM_CLASS_ARMOR                            = 4,
    ITEM_CLASS_REAGENT                          = 5,
    ITEM_CLASS_PROJECTILE                       = 6,
    ITEM_CLASS_TRADE_GOODS                      = 7,
    ITEM_CLASS_ITEM_ENHANCEMENT                 = 8,
    ITEM_CLASS_RECIPE                           = 9,
    ITEM_CLASS_CURRENCY_TOKEN_OBSOLETE          = 10,
    ITEM_CLASS_QUIVER                           = 11,
    ITEM_CLASS_QUESTITEM                        = 12,
    ITEM_CLASS_KEY                              = 13,
    ITEM_CLASS_PERMANENT_OBSOLETE               = 14,
    ITEM_CLASS_MISCELLANEOUS                    = 15,
    ITEM_CLASS_GLYPH                            = 16,
    ITEM_CLASS_BATTLE_PET                       = 17,
    ITEM_CLASS_WOW_TOKEN                        = 18,

    MAX_ITEM_CLASS
};

enum ItemSubclassConsumable
{
    ITEM_SUBCLASS_CONSUMABLE                    = 0,
    ITEM_SUBCLASS_POTION                        = 1,
    ITEM_SUBCLASS_ELIXIR                        = 2,
    ITEM_SUBCLASS_FLASK                         = 3,
    ITEM_SUBCLASS_SCROLL                        = 4,
    ITEM_SUBCLASS_FOOD_DRINK                    = 5,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT              = 6,
    ITEM_SUBCLASS_BANDAGE                       = 7,
    ITEM_SUBCLASS_CONSUMABLE_OTHER              = 8,
    ITEM_SUBCLASS_VANTUS_RUNE                   = 9,

    MAX_ITEM_SUBCLASS_CONSUMABLE
};

enum ItemSubclassContainer
{
    ITEM_SUBCLASS_CONTAINER                     = 0,
    ITEM_SUBCLASS_SOUL_CONTAINER                = 1,
    ITEM_SUBCLASS_HERB_CONTAINER                = 2,
    ITEM_SUBCLASS_ENCHANTING_CONTAINER          = 3,
    ITEM_SUBCLASS_ENGINEERING_CONTAINER         = 4,
    ITEM_SUBCLASS_GEM_CONTAINER                 = 5,
    ITEM_SUBCLASS_MINING_CONTAINER              = 6,
    ITEM_SUBCLASS_LEATHERWORKING_CONTAINER      = 7,
    ITEM_SUBCLASS_INSCRIPTION_CONTAINER         = 8,
    ITEM_SUBCLASS_TACKLE_CONTAINER              = 9,
    ITEM_SUBCLASS_COOKING_CONTAINER             = 10,
    ITEM_SUBCLASS_OTHER_CONTAINER               = 11,
    ITEM_SUBCLASS_ENCHANTING_CONTAINER_2        = 12,
    ITEM_SUBCLASS_MATERIALS_CONTAINER           = 13,
    ITEM_SUBCLASS_ITEM_ENCHANTMENT_CONTAINER    = 14,
    ITEM_SUBCLASS_WEAPON_ENCHANTMENT_CONTAINER  = 15,

    MAX_ITEM_SUBCLASS_CONTAINER
};

enum ItemSubclassWeapon
{
    ITEM_SUBCLASS_WEAPON_AXE                    = 0,  // One-Handed Axes
    ITEM_SUBCLASS_WEAPON_AXE2                   = 1,  // Two-Handed Axes
    ITEM_SUBCLASS_WEAPON_BOW                    = 2,
    ITEM_SUBCLASS_WEAPON_GUN                    = 3,
    ITEM_SUBCLASS_WEAPON_MACE                   = 4,  // One-Handed Maces
    ITEM_SUBCLASS_WEAPON_MACE2                  = 5,  // Two-Handed Maces
    ITEM_SUBCLASS_WEAPON_POLEARM                = 6,
    ITEM_SUBCLASS_WEAPON_SWORD                  = 7,  // One-Handed Swords
    ITEM_SUBCLASS_WEAPON_SWORD2                 = 8,  // Two-Handed Swords
    ITEM_SUBCLASS_WEAPON_WARGLAIVES             = 9,  // One-Handed WarGlaive
    ITEM_SUBCLASS_WEAPON_STAFF                  = 10,
    ITEM_SUBCLASS_WEAPON_EXOTIC                 = 11, // One-Handed Exotics
    ITEM_SUBCLASS_WEAPON_EXOTIC2                = 12, // Two-Handed Exotics
    ITEM_SUBCLASS_WEAPON_FIST_WEAPON            = 13,
    ITEM_SUBCLASS_WEAPON_MISCELLANEOUS          = 14,
    ITEM_SUBCLASS_WEAPON_DAGGER                 = 15,
    ITEM_SUBCLASS_WEAPON_THROWN                 = 16,
    ITEM_SUBCLASS_WEAPON_SPEAR                  = 17,
    ITEM_SUBCLASS_WEAPON_CROSSBOW               = 18,
    ITEM_SUBCLASS_WEAPON_WAND                   = 19,
    ITEM_SUBCLASS_WEAPON_FISHING_POLE           = 20
};

#define ITEM_SUBCLASS_MASK_WEAPON_RANGED (\
    (1 << ITEM_SUBCLASS_WEAPON_BOW) | (1 << ITEM_SUBCLASS_WEAPON_GUN) |\
    (1 << ITEM_SUBCLASS_WEAPON_CROSSBOW))

#define MAX_ITEM_SUBCLASS_WEAPON                  21


enum ItemSubclassGem
{
    ITEM_SUBCLASS_GEM_INTELLECT                 = 0,
    ITEM_SUBCLASS_GEM_AGILITY                   = 1,
    ITEM_SUBCLASS_GEM_STRENGTH                  = 2,
    ITEM_SUBCLASS_GEM_STAMINA                   = 3,
    ITEM_SUBCLASS_GEM_SPIRIT                    = 4,
    ITEM_SUBCLASS_GEM_CRITICAL_STRIKE           = 5,
    ITEM_SUBCLASS_GEM_MASTERY                   = 6,
    ITEM_SUBCLASS_GEM_HASTE                     = 7,
    ITEM_SUBCLASS_GEM_VERSATILITY               = 8,
    ITEM_SUBCLASS_GEM_OTHER                     = 9,
    ITEM_SUBCLASS_GEM_MULTIPLE_STATS            = 10,
    ITEM_SUBCLASS_GEM_ARTIFACT_RELIC            = 11,

    MAX_ITEM_SUBCLASS_GEM
};

enum ItemSubclassArmor
{
    ITEM_SUBCLASS_ARMOR_MISCELLANEOUS           = 0,
    ITEM_SUBCLASS_ARMOR_CLOTH                   = 1,
    ITEM_SUBCLASS_ARMOR_LEATHER                 = 2,
    ITEM_SUBCLASS_ARMOR_MAIL                    = 3,
    ITEM_SUBCLASS_ARMOR_PLATE                   = 4,
    ITEM_SUBCLASS_ARMOR_COSMETIC                = 5, // New 5.4 (old : BUCKLER)
    ITEM_SUBCLASS_ARMOR_SHIELD                  = 6,
    ITEM_SUBCLASS_ARMOR_LIBRAM                  = 7,
    ITEM_SUBCLASS_ARMOR_IDOL                    = 8,
    ITEM_SUBCLASS_ARMOR_TOTEM                   = 9,
    ITEM_SUBCLASS_ARMOR_SIGIL                   = 10,
    ITEM_SUBCLASS_ARMOR_RELIC                   = 11,

    MAX_ITEM_SUBCLASS_ARMOR
};

enum ItemSubclassReagent
{
    ITEM_SUBCLASS_REAGENT                       = 0,
    ITEM_SUBCLASS_KEYSTONE                      = 1,

    MAX_ITEM_SUBCLASS_REAGENT
};

enum ItemSubclassProjectile
{
    ITEM_SUBCLASS_WAND                          = 0, // OBSOLETE
    ITEM_SUBCLASS_BOLT                          = 1, // OBSOLETE
    ITEM_SUBCLASS_ARROW                         = 2,
    ITEM_SUBCLASS_BULLET                        = 3,
    ITEM_SUBCLASS_THROWN                        = 4, // OBSOLETE

    MAX_ITEM_SUBCLASS_PROJECTILE
};

enum ItemSubclassTradeGoods
{
    ITEM_SUBCLASS_TRADE_GOODS                   = 0,
    ITEM_SUBCLASS_PARTS                         = 1,
    ITEM_SUBCLASS_EXPLOSIVES                    = 2,
    ITEM_SUBCLASS_DEVICES                       = 3,
    ITEM_SUBCLASS_JEWELCRAFTING                 = 4,
    ITEM_SUBCLASS_CLOTH                         = 5,
    ITEM_SUBCLASS_LEATHER                       = 6,
    ITEM_SUBCLASS_METAL_STONE                   = 7,
    ITEM_SUBCLASS_MEAT                          = 8,
    ITEM_SUBCLASS_HERB                          = 9,
    ITEM_SUBCLASS_ELEMENTAL                     = 10,
    ITEM_SUBCLASS_TRADE_GOODS_OTHER             = 11,
    ITEM_SUBCLASS_ENCHANTING                    = 12,
    ITEM_SUBCLASS_MATERIAL                      = 13,
    ITEM_SUBCLASS_ENCHANTMENT                   = 14,
    ITEM_SUBCLASS_WEAPON_ENCHANTMENT            = 15,
    ITEM_SUBCLASS_INSCRIPTION                   = 16,
    ITEM_SUBCLASS_EXPLOSIVES_DEVICES            = 17,

    MAX_ITEM_SUBCLASS_TRADE_GOODS
};

enum ItemSubclassItemEnhancement
{
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_HEAD                 = 0,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_NECK                 = 1,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_SHOULDER             = 2,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_CLOAK                = 3,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_CHEST                = 4,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_WRIST                = 5,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_HANDS                = 6,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_WAIST                = 7,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_LEGS                 = 8,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_FEET                 = 9,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_FINGER               = 10,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_WEAPON               = 11,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_TWO_HANDED_WEAPON    = 12,
    ITEM_SUBCLASS_ITEM_ENHANCEMENT_SHIELD_OFF_HAND      = 13,

    MAX_ITEM_SUBCLASS_ITEM_ENHANCEMENT
};

enum ItemSubclassRecipe
{
    ITEM_SUBCLASS_BOOK                          = 0,
    ITEM_SUBCLASS_LEATHERWORKING_PATTERN        = 1,
    ITEM_SUBCLASS_TAILORING_PATTERN             = 2,
    ITEM_SUBCLASS_ENGINEERING_SCHEMATIC         = 3,
    ITEM_SUBCLASS_BLACKSMITHING                 = 4,
    ITEM_SUBCLASS_COOKING_RECIPE                = 5,
    ITEM_SUBCLASS_ALCHEMY_RECIPE                = 6,
    ITEM_SUBCLASS_FIRST_AID_MANUAL              = 7,
    ITEM_SUBCLASS_ENCHANTING_FORMULA            = 8,
    ITEM_SUBCLASS_FISHING_MANUAL                = 9,
    ITEM_SUBCLASS_JEWELCRAFTING_RECIPE          = 10,
    ITEM_SUBCLASS_INSCRIPTION_TECHNIQUE         = 11,

    MAX_ITEM_SUBCLASS_RECIPE
};

enum ItemSubclassMoney
{
    ITEM_SUBCLASS_MONEY                         = 0,  // OBSOLETE

    MAX_ITEM_SUBCLASS_MONEY
};

enum ItemSubclassQuiver
{
    ITEM_SUBCLASS_QUIVER0                       = 0, // OBSOLETE
    ITEM_SUBCLASS_QUIVER1                       = 1, // OBSOLETE
    ITEM_SUBCLASS_QUIVER                        = 2,
    ITEM_SUBCLASS_AMMO_POUCH                    = 3,

    MAX_ITEM_SUBCLASS_QUIVER
};

enum ItemSubclassQuest
{
    ITEM_SUBCLASS_QUEST                         = 0,
    ITEM_SUBCLASS_QUEST_UNK3                    = 3, // 1 item (33604)
    ITEM_SUBCLASS_QUEST_UNK8                    = 8, // 2 items (37445, 49700)

    MAX_ITEM_SUBCLASS_QUEST
};

enum ItemSubclassKey
{
    ITEM_SUBCLASS_KEY                           = 0,
    ITEM_SUBCLASS_LOCKPICK                      = 1,

    MAX_ITEM_SUBCLASS_KEY
};

enum ItemSubclassPermanent
{
    ITEM_SUBCLASS_PERMANENT                     = 0,

    MAX_ITEM_SUBCLASS_PERMANENT
};

enum ItemSubclassMiscellaneous
{
    ITEM_SUBCLASS_MISCELLANEOUS_JUNK            = 0,
    ITEM_SUBCLASS_MISCELLANEOUS_REAGENT         = 1,
    ITEM_SUBCLASS_MISCELLANEOUS_COMPANION_PET   = 2,
    ITEM_SUBCLASS_MISCELLANEOUS_HOLIDAY         = 3,
    ITEM_SUBCLASS_MISCELLANEOUS_OTHER           = 4,
    ITEM_SUBCLASS_MISCELLANEOUS_MOUNT           = 5,

    MAX_ITEM_SUBCLASS_MISCELLANEOUS
};

enum ItemSubclassGlyph
{
    ITEM_SUBCLASS_GLYPH_WARRIOR                 = 1,
    ITEM_SUBCLASS_GLYPH_PALADIN                 = 2,
    ITEM_SUBCLASS_GLYPH_HUNTER                  = 3,
    ITEM_SUBCLASS_GLYPH_ROGUE                   = 4,
    ITEM_SUBCLASS_GLYPH_PRIEST                  = 5,
    ITEM_SUBCLASS_GLYPH_DEATH_KNIGHT            = 6,
    ITEM_SUBCLASS_GLYPH_SHAMAN                  = 7,
    ITEM_SUBCLASS_GLYPH_MAGE                    = 8,
    ITEM_SUBCLASS_GLYPH_WARLOCK                 = 9,
    ITEM_SUBCLASS_GLYPH_MONK                    = 10,
    ITEM_SUBCLASS_GLYPH_DRUID                   = 11,
    ITEM_SUBCLASS_GLYPH_DEMON_HUNTER            = 12,

    MAX_ITEM_SUBCLASS_GLYPH
};

enum ItemSubclassBattlePet
{
    ITEM_SUBCLASS_BATTLE_PET_AQUATIC            = 1,
    ITEM_SUBCLASS_BATTLE_PET_BEAST              = 2,
    ITEM_SUBCLASS_BATTLE_PET_CRITTER            = 3,
    ITEM_SUBCLASS_BATTLE_PET_DRAGONKIN          = 4,
    ITEM_SUBCLASS_BATTLE_PET_ELEMENTAL          = 5,
    ITEM_SUBCLASS_BATTLE_PET_FLYING             = 6,
    ITEM_SUBCLASS_BATTLE_PET_HUMANOID           = 7,
    ITEM_SUBCLASS_BATTLE_PET_MAGICAL            = 8,
    ITEM_SUBCLASS_BATTLE_PET_MECHANICAL         = 9,
    ITEM_SUBCLASS_BATTLE_PET_UNDEAD             = 10,

    MAX_ITEM_SUBCLASS_BATTLE_PET
};

enum ItemSubclassWowToken
{
    ITEM_SUBCLASS_WOW_TOKEN                     = 1,

    MAX_ITEM_SUBCLASS_WOW_TOKEN
};

const uint32 MaxItemSubclassValues[MAX_ITEM_CLASS] =
{
    MAX_ITEM_SUBCLASS_CONSUMABLE,
    MAX_ITEM_SUBCLASS_CONTAINER,
    MAX_ITEM_SUBCLASS_WEAPON,
    MAX_ITEM_SUBCLASS_GEM,
    MAX_ITEM_SUBCLASS_ARMOR,
    MAX_ITEM_SUBCLASS_REAGENT,
    MAX_ITEM_SUBCLASS_PROJECTILE,
    MAX_ITEM_SUBCLASS_TRADE_GOODS,
    MAX_ITEM_SUBCLASS_ITEM_ENHANCEMENT,
    MAX_ITEM_SUBCLASS_RECIPE,
    MAX_ITEM_SUBCLASS_MONEY,
    MAX_ITEM_SUBCLASS_QUIVER,
    MAX_ITEM_SUBCLASS_QUEST,
    MAX_ITEM_SUBCLASS_KEY,
    MAX_ITEM_SUBCLASS_PERMANENT,
    MAX_ITEM_SUBCLASS_MISCELLANEOUS,
    MAX_ITEM_SUBCLASS_GLYPH,
    MAX_ITEM_SUBCLASS_BATTLE_PET,
    MAX_ITEM_SUBCLASS_WOW_TOKEN
};

inline uint8 ItemSubClassToDurabilityMultiplierId(uint32 ItemClass, uint32 ItemSubClass)
{
    switch (ItemClass)
    {
        case ITEM_CLASS_WEAPON: return ItemSubClass;
        case ITEM_CLASS_ARMOR:  return ItemSubClass + 21;
    }
    return 0;
}

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct _ItemStat
{
    uint32  ItemStatType;
    int32   ItemStatValue;
    int32   ScalingValue;
    float   SocketCostRate;
};

struct _Spell
{
    int32 SpellId;                                         // id from Spell.db2
    uint32 SpellTrigger;
    int32  SpellCharges;
    int32  SpellCooldown;
    uint32 SpellCategory;                                   // id from SpellCategory.db2
    int32  SpellCategoryCooldown;
};

struct _Socket
{
    uint32 Color;
    uint32 Content;
};




namespace ItemBonus
{
    using GroupIdContainer = std::list<uint32>;
};

struct ItemTemplate
{
    uint32 ItemId;
    uint32 Class;                                           // id from ItemClass.db2
    uint32 SubClass;                                        // id from ItemSubClass.db2
    int32  SoundOverrideSubclass;                           // < 0: id from ItemSubClass.db2, used to override weapon sound from actual SubClass
    LocalizedString const* Name1;
    uint32 DisplayInfoID;                                   // id from ItemDisplayInfo.db2
    uint32 Quality;
    uint32 Flags;
    uint32 Flags2;
    uint32 Flags3;
    uint32 Flags4;
    float PriceRandomValue;
    float PriceVariance;
    uint32 BuyCount;
    int32  BuyPrice;
    uint32 SellPrice;
    uint32 InventoryType;
    uint32 AllowableClass;
    int64 AllowableRace{};
    uint32 ItemLevel;
    uint32 RequiredLevel;
    uint32 RequiredSkill;                                   // id from SkillLine.db2
    uint32 RequiredSkillRank;
    uint32 RequiredSpell;                                   // id from Spell.db2
    uint32 RequiredHonorRank;
    uint32 RequiredCityRank;
    uint32 RequiredReputationFaction;                       // id from Faction.db2
    uint32 RequiredReputationRank;
    int32  MaxCount;                                        // <= 0: no limit
    int32  Stackable;                                       // 0: not allowed, -1: put in player coin info tab and don't limit stacking (so 1 slot)
    uint32 ContainerSlots;
    _ItemStat ItemStat[MAX_ITEM_PROTO_STATS];
    uint32 ScalingStatDistribution;                         // id from ScalingStatDistribution.db2
    uint32 DamageType;                                      // id from Resistances.db2
    uint32 Delay;
    float  RangedModRange;
    _Spell Spells[MAX_ITEM_PROTO_SPELLS];
    uint32 Bonding;
    LocalizedString const* Description;
    uint32 PageText;
    uint32 LanguageID;
    uint32 PageMaterial;
    uint32 StartQuest;                                      // id from QuestCache.wdb
    uint32 LockID;
    int32  Material;                                        // id from Material.db2
    uint32 Sheath;
    int32  RandomProperty;                                  // id from ItemRandomProperties.db2
    int32  RandomSuffix;                                    // id from ItemRandomSuffix.db2
    uint32 ItemSet;                                         // id from ItemSet.db2
    uint32 MaxDurability;
    uint32 Area;                                            // id from AreaTable.db2
    uint32 Map;                                             // id from Map.db2
    uint32 BagFamily;                                       // bit mask (1 << id from ItemBagFamily.db2)
    uint32 TotemCategory;                                   // id from TotemCategory.db2
    _Socket Socket[MAX_ITEM_PROTO_SOCKETS];
    uint32 socketBonus;                                     // id from SpellItemEnchantment.db2
    uint32 GemProperties;                                   // id from GemProperties.db2
    float  ArmorDamageModifier;
    uint32 Duration;
    uint32 ItemLimitCategory;                               // id from ItemLimitCategory.db2
    uint32 HolidayId;                                       // id from Holidays.db2
    float  StatScalingFactor;
    uint32 CurrencySubstitutionId;                          // May be used instead of a currency
    uint32 CurrencySubstitutionCount;
    uint32 ItemNameDescriptionID;
    uint8 ArtifactID;
    int8  RequiredExpansion;
    bool NeedScaleOnArtifactKnowledge;

    // extra fields, not part of db2 files
    uint32 Armor;
    float  SpellPPMRate;
    uint32 ScriptId;
    uint32 FoodType;
    uint32 MinMoneyLoot;
    uint32 MaxMoneyLoot;
    uint32 FlagsCu;
    uint32 PvPScalingLevel;
    std::bitset<MAX_CLASSES * MAX_SPECIALIZATIONS> Specializations[2];  // one set for 1-40 level range and another for 41-100
    uint32 ItemSpecClassMask;

    /// Item bonus group
    ItemBonus::GroupIdContainer m_ItemBonusGroups;

    // helpers
    bool CanChangeEquipStateInCombat() const
    {
        switch (InventoryType)
        {
            case INVTYPE_RELIC:
            case INVTYPE_SHIELD:
            case INVTYPE_HOLDABLE:
                return true;
        }

        switch (Class)
        {
            case ITEM_CLASS_WEAPON:
            case ITEM_CLASS_PROJECTILE:
                return true;
        }

        return false;
    }

    bool IsStuff() const
    {
        if (Class == ITEM_CLASS_GEM)
            return true;

        switch (InventoryType)
        {
            case INVTYPE_NON_EQUIP:
            case INVTYPE_BODY:
            case INVTYPE_BAG:
            case INVTYPE_TABARD:
            case INVTYPE_AMMO:
            case INVTYPE_THROWN:
            case INVTYPE_QUIVER:
                return false;
            default:
                return true;
        }
    }

    bool IsToken() const
    {
        return InventoryType == InventoryType::INVTYPE_NON_EQUIP && Quality >= ItemQualities::ITEM_QUALITY_UNCOMMON &&
            HasSpellWithEffect(SpellEffects::SPELL_EFFECT_LOOT_BONUS, EFFECT_0);
    }

    bool IsUsableBySpecialization(uint32 specId, uint8 level) const
    {
        if (!HasSpec())
            return true;

        if (ChrSpecializationsEntry const* chrSpecialization = sChrSpecializationsStore.LookupEntry(specId))
            return Specializations[level > 40].test(CalculateItemSpecBit(chrSpecialization));

        return false;
    }

    static std::size_t CalculateItemSpecBit(ChrSpecializationsEntry const* spec)
    {
        return (spec->ClassID - 1) * MAX_SPECIALIZATIONS + spec->OrderIndex;
    }

    bool IsCurrencyToken() const { return BagFamily & BAG_FAMILY_MASK_CURRENCY_TOKENS; }

    uint32 GetMaxStackSize() const
    {
        return (Stackable == 2147483647 || Stackable <= 0) ? uint32(0x7FFFFFFF-1) : uint32(Stackable);
    }

    int GetItemLevelIncludingQuality(int l_ScalingItemLevel = 0) const
    {
        int l_ItemLevel = l_ScalingItemLevel ? l_ScalingItemLevel : (int)ItemLevel;
        switch (Quality)
        {
            case ITEM_QUALITY_POOR:
            case ITEM_QUALITY_NORMAL:
            case ITEM_QUALITY_UNCOMMON:
            case ITEM_QUALITY_ARTIFACT:
            case ITEM_QUALITY_HEIRLOOM:
                l_ItemLevel -= 13; // leaving this as a separate statement since we do not know the real behavior in this case
                break;
            case ITEM_QUALITY_EPIC:
            case ITEM_QUALITY_LEGENDARY:
            default:
                break;
        }
        return l_ItemLevel >= 0 ? l_ItemLevel : 1;
    }

    bool IsPotion() const { return Class == ITEM_CLASS_CONSUMABLE && (SubClass == ITEM_SUBCLASS_POTION || SubClass == ITEM_SUBCLASS_FOOD_DRINK); }
    bool IsVellum() const { return Class == ITEM_CLASS_TRADE_GOODS && (Flags3 & (uint32)ItemFlagsEX2::CAN_STORE_ENCHANTS); }
    bool IsConjuredConsumable() const { return Class == ITEM_CLASS_CONSUMABLE && (Flags & int32(ItemFlags::CONJURED)); }
    bool IsCraftingReagent() const { return (Flags2 & int32(ItemFlagsEX::USED_IN_A_TRADESKILL)) != 0; }
    bool IsObsolete() const { return Flags3 & (uint32)ItemFlagsEX2::OBSOLETE; };

    bool IsRangedWeapon() const
    {
        return Class == ITEM_CLASS_WEAPON && (
               SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
               SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
               SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW);
    }

    bool IsOneHanded() const
    {
        return Class == ITEM_CLASS_WEAPON && (
            SubClass == ITEM_SUBCLASS_WEAPON_SWORD ||
            SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
            SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
            SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ||
            SubClass == ITEM_SUBCLASS_WEAPON_FIST_WEAPON ||
            SubClass == ITEM_SUBCLASS_WEAPON_EXOTIC ||
            SubClass == ITEM_SUBCLASS_WEAPON_WARGLAIVES );
    }

    bool IsTwoHandedWeapon() const
    {
        return Class == ITEM_CLASS_WEAPON && !IsOneHanded();
    }

    bool IsQuestItem() const
    {
        return Class == ITEM_CLASS_QUESTITEM;
    }

    uint32 GetSkill() const
    {
        const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
        {
            SKILL_AXES,             SKILL_TWO_HANDED_AXES, SKILL_BOWS,   SKILL_GUNS,              SKILL_MACES,
            SKILL_TWO_HANDED_MACES, SKILL_POLEARMS,        SKILL_SWORDS, SKILL_TWO_HANDED_SWORDS, SKILL_WARGLAIVES,
            SKILL_STAVES,           0,                     0,            SKILL_FIST_WEAPONS,      0,
            SKILL_DAGGERS,          0,                     0,            SKILL_CROSSBOWS,         SKILL_WANDS,
            SKILL_FISHING
        };

        const static uint32 item_armor_skills[MAX_ITEM_SUBCLASS_ARMOR] =
        {
            0, SKILL_CLOTH, SKILL_LEATHER, SKILL_MAIL, SKILL_PLATE_MAIL, 0, SKILL_SHIELD, 0, 0, 0, 0
        };

        switch (Class)
        {
            case ITEM_CLASS_WEAPON:
                if (SubClass >= MAX_ITEM_SUBCLASS_WEAPON)
                    return 0;
                else
                    return item_weapon_skills[SubClass];

            case ITEM_CLASS_ARMOR:
                if (SubClass >= MAX_ITEM_SUBCLASS_ARMOR)
                    return 0;
                else
                    return item_armor_skills[SubClass];

            default:
                return 0;
        }
    }

    bool HasSpec() const { return !Specializations[1].none() || !Specializations[0].none(); }

    bool HasClassSpec(uint8 Class, uint32 level) const
    {
        if (!HasSpec())
            return true;

        for (int l_I = 0; l_I < MAX_SPECIALIZATIONS; ++l_I)
        {
            if (auto l_Entry = GetChrSpecializationByIndex(Class, l_I))
                if (HasSpec(static_cast<SpecIndex>(l_Entry->ID), level))
                    return true;
        }

        return false;
    }

    bool HasSpec(SpecIndex index, uint32 level) const
    {
        if (!HasSpec())
            return true;

        ChrSpecializationsEntry const* l_SpeEntry = sChrSpecializationsStore.LookupEntry(index);

        if (!l_SpeEntry || !l_SpeEntry->ClassID)
            return false;

        return Specializations[level > 40].test(ItemTemplate::CalculateItemSpecBit(l_SpeEntry));
    }

    bool IsWeapon() const { return Class == ITEM_CLASS_WEAPON; }
    bool IsArmor() const { return Class == ITEM_CLASS_ARMOR; }
    bool isArmorOrWeapon() const { return IsWeapon() || IsArmor(); }
    bool IsArtifactRelic() const { return Class == ITEM_CLASS_GEM && SubClass == ITEM_SUBCLASS_GEM_ARTIFACT_RELIC; };

    float GetScalingDamageValue(uint32 ilvl) const;
    int32 GetRandomPointsOffset() const;
    uint32 GetArmor(uint32 ilvl) const;
    void GetDamage(uint32 ilvl, float& minDamage, float& maxDamage) const;

    uint32 GetBaseItemLevel() const { return ItemLevel; }

    uint32 GetGemProperties() const { return GemProperties; }
    uint32 GetSocketBonus() const { return socketBonus; }
    SocketColor GetSocketColor(uint32 index) const { ASSERT(index < MAX_ITEM_PROTO_SOCKETS); return SocketColor(Socket[index].Color); }
    uint32 GetInventoryType() const { return InventoryType; }
    uint32 GetSubClass() const { return SubClass; }

    bool CanBeTransmogrified(bool p_CustomTransmo = false) const
    {
        if (Class != ITEM_CLASS_ARMOR &&
            Class != ITEM_CLASS_WEAPON)
            return false;

        if (Class == ITEM_CLASS_WEAPON && SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
            return false;

        if (Flags2 & int32(ItemFlagsEX::NO_ALTER_ITEM_VISUAL) && !p_CustomTransmo)
            return false;

        return true;
    }

    bool CanTransmogrify(bool p_CustomTransmo = false) const
    {
        if (Flags2 & int32(ItemFlagsEX::NO_SOURCE_FOR_ITEM_VISUAL) && !p_CustomTransmo)
            return false;

        if (Class != ITEM_CLASS_ARMOR &&
            Class != ITEM_CLASS_WEAPON)
            return false;

        if (Class == ITEM_CLASS_WEAPON && SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
            return false;

        if (Flags2 & int32(ItemFlagsEX::IGNORE_QUALITY_FOR_ITEM_VISUAL_SOURCE))
            return true;

        return true;
    }

    bool HasStats() const
    {
        for (uint8 l_I = 0; l_I < MAX_ITEM_PROTO_STATS; ++l_I)
        {
            if (ItemStat[l_I].ItemStatValue != 0)
                return true;
        }

        return false;
    }

    bool HasSpell(uint32 p_SpellID) const
    {
        for (uint8 l_I = 0; l_I < MAX_ITEM_PROTO_SPELLS; ++l_I)
        {
            if (Spells[l_I].SpellId == p_SpellID)
                return true;
        }

        return false;
    }

    bool HasSpellWithEffect(uint8 p_Effect, uint8 p_Index) const
    {
        for (uint8 l_I = 0; l_I < MAX_ITEM_PROTO_SPELLS; ++l_I)
        {
            if (SpellEffectEntry const* l_SpellEffect = GetSpellEffectEntry(Spells[l_I].SpellId, p_Index, 0))
            {
                if (l_SpellEffect->Effect == p_Effect)
                    return true;
            }
        }

        return false;
    }

    uint32 GetArtifactThirdSlotBonus() const
    {
        uint32 l_ThirdSlotBonus = 0;

        switch (ItemId)
        {
            case 128832: l_ThirdSlotBonus = 721; break;
            case 127829: l_ThirdSlotBonus = 719; break;
            case 128403: l_ThirdSlotBonus = 718; break;
            case 128292: l_ThirdSlotBonus = 717; break;
            case 128402: l_ThirdSlotBonus = 716; break;
            case 128858: l_ThirdSlotBonus = 722; break;
            case 128860: l_ThirdSlotBonus = 723; break;
            case 128821: l_ThirdSlotBonus = 724; break;
            case 128306: l_ThirdSlotBonus = 725; break;
            case 128861: l_ThirdSlotBonus = 726; break;
            case 128826: l_ThirdSlotBonus = 727; break;
            case 128808: l_ThirdSlotBonus = 728; break;
            case 127857: l_ThirdSlotBonus = 729; break;
            case 128820: l_ThirdSlotBonus = 730; break;
            case 128862: l_ThirdSlotBonus = 731; break;
            case 128938: l_ThirdSlotBonus = 732; break;
            case 128937: l_ThirdSlotBonus = 733; break;
            case 128940: l_ThirdSlotBonus = 734; break;
            case 128823: l_ThirdSlotBonus = 735; break;
            case 128866: l_ThirdSlotBonus = 736; break;
            case 120978: l_ThirdSlotBonus = 737; break;
            case 128868: l_ThirdSlotBonus = 738; break;
            case 128825: l_ThirdSlotBonus = 739; break;
            case 128827: l_ThirdSlotBonus = 740; break;
            case 128870: l_ThirdSlotBonus = 741; break;
            case 128872: l_ThirdSlotBonus = 742; break;
            case 128476: l_ThirdSlotBonus = 743; break;
            case 128935: l_ThirdSlotBonus = 744; break;
            case 128819: l_ThirdSlotBonus = 745; break;
            case 128911: l_ThirdSlotBonus = 746; break;
            case 128942: l_ThirdSlotBonus = 747; break;
            case 128943: l_ThirdSlotBonus = 748; break;
            case 128941: l_ThirdSlotBonus = 749; break;
            case 128910: l_ThirdSlotBonus = 750; break;
            case 128908: l_ThirdSlotBonus = 751; break;
            case 128289: l_ThirdSlotBonus = 752; break;
        }

        return l_ThirdSlotBonus;
    }
};

enum ItemSpecStat
{
    ITEM_SPEC_STAT_INTELLECT        = 0,
    ITEM_SPEC_STAT_AGILITY          = 1,
    ITEM_SPEC_STAT_STRENGTH         = 2,
    ITEM_SPEC_STAT_SPIRIT           = 3,
    ITEM_SPEC_STAT_HIT              = 4,
    ITEM_SPEC_STAT_DODGE            = 5,
    ITEM_SPEC_STAT_PARRY            = 6,
    ITEM_SPEC_STAT_ONE_HANDED_AXE   = 7,
    ITEM_SPEC_STAT_TWO_HANDED_AXE   = 8,
    ITEM_SPEC_STAT_ONE_HANDED_SWORD = 9,
    ITEM_SPEC_STAT_TWO_HANDED_SWORD = 10,
    ITEM_SPEC_STAT_ONE_HANDED_MACE  = 11,
    ITEM_SPEC_STAT_TWO_HANDED_MACE  = 12,
    ITEM_SPEC_STAT_DAGGER           = 13,
    ITEM_SPEC_STAT_FIST_WEAPON      = 14,
    ITEM_SPEC_STAT_GUN              = 15,
    ITEM_SPEC_STAT_BOW              = 16,
    ITEM_SPEC_STAT_CROSSBOW         = 17,
    ITEM_SPEC_STAT_STAFF            = 18,
    ITEM_SPEC_STAT_POLEARM          = 19,
    ITEM_SPEC_STAT_THROWN           = 20,
    ITEM_SPEC_STAT_WAND             = 21,
    ITEM_SPEC_STAT_SHIELD           = 22,
    ITEM_SPEC_STAT_RELIC            = 23,
    ITEM_SPEC_STAT_CRIT             = 24,
    ITEM_SPEC_STAT_HASTE            = 25,
    ITEM_SPEC_STAT_BONUS_ARMOR      = 26,
    ITEM_SPEC_STAT_CLOAK            = 27,
    ITEM_SPEC_STAT_WARGLAIVES       = 28,
    ITEM_SPEC_STAT_RELIC_IRON       = 29,
    ITEM_SPEC_STAT_RELIC_BLOOD      = 30,
    ITEM_SPEC_STAT_RELIC_SHADOW     = 31,
    ITEM_SPEC_STAT_RELIC_FEL        = 32,
    ITEM_SPEC_STAT_RELIC_ARCANE     = 33,
    ITEM_SPEC_STAT_RELIC_FROST      = 34,
    ITEM_SPEC_STAT_RELIC_FIRE       = 35,
    ITEM_SPEC_STAT_RELIC_WATER      = 36,
    ITEM_SPEC_STAT_RELIC_LIFE       = 37,
    ITEM_SPEC_STAT_RELIC_WIND       = 38,
    ITEM_SPEC_STAT_RELIC_HOLY       = 39,

    ITEM_SPEC_STAT_NONE             = 40
};

extern float GetCurveValue(uint32 CurveParameter, float level);

// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint32, ItemTemplate> ItemTemplateContainer;
typedef std::vector<ItemTemplate const*> ItemTemplateList;
typedef std::map<uint32, ItemTemplateList> ItemTemplateListBySpec;

struct ItemLocale
{
    StringVector Name;
    StringVector Description;
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif
#endif
