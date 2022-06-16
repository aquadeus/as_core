////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ITEM_H
#define ITEM_H

#include "Common.h"
#include "Object.h"
#include "LootMgr.h"
#include "ItemPrototype.h"
#include "DatabaseEnv.h"
#include "ArtifactMgr.h"
#include "DB2Stores.h"
#include <unordered_set>

class SpellInfo;
class Bag;
class Unit;

struct VoidStorageItem;

struct ItemSetEffect
{
    uint32 ItemSetID;
    uint32 EquippedItemCount;
    std::unordered_set<ItemSetSpellEntry const*> SetBonuses;
};

#define MAX_ITEM_BONUS 6

enum ItemBonusType
{
    ITEM_BONUS_ITEM_LEVEL                  = 1,
    ITEM_BONUS_STAT                        = 2,
    ITEM_BONUS_QUALITY                     = 3,
    ITEM_BONUS_DESCRIPTION                 = 4,
    ITEM_BONUS_SUFFIX                      = 5,
    ITEM_BONUS_SOCKET                      = 6,
    ITEM_BONUS_APPEARANCE                  = 7,
    ITEM_BONUS_REQUIRED_LEVEL              = 8,
    ITEM_BONUS_DISPLAY_TOAST_METHOD        = 9,
    ITEM_BONUS_REPAIR_COST_MULTIPLIER      = 10,
    ITEM_BONUS_SCALING_STAT_DISTRIBUTION   = 11,
    ITEM_BONUS_DISENCHANT_LOOT_ID          = 12,
    ITEM_BONUS_SCALING_STAT_DISTRIBUTION_2 = 13,
    ITEM_BONUS_ITEM_LEVEL_OVERRIDE         = 14,
    ITEM_BONUS_RANDOM_ENCHANTMENT          = 15,    ///< Responsible for showing "<Random additional stats>" or "+%d Rank Random Minor Trait" in the tooltip before item is obtained
    ITEM_BONUS_BONDING                     = 16,
    ITEM_BONUS_RELIC_TYPE                  = 17,
    ITEM_BONUS_OVERRIDE_REQUIRED_LEVEL     = 18
};

enum InventoryResult
{
    EQUIP_ERR_OK                                           = 0,
    EQUIP_ERR_CANT_EQUIP_LEVEL_I                           = 1,  // You must reach level %d to use that item.
    EQUIP_ERR_CANT_EQUIP_SKILL                             = 2,  // You aren't skilled enough to use that item.
    EQUIP_ERR_WRONG_SLOT                                   = 3,  // That item does not go in that slot.
    EQUIP_ERR_BAG_FULL                                     = 4,  // That bag is full.
    EQUIP_ERR_BAG_IN_BAG                                   = 5,  // Can't put non-empty bags in other bags.
    EQUIP_ERR_TRADE_EQUIPPED_BAG                           = 6,  // You can't trade equipped bags.
    EQUIP_ERR_AMMO_ONLY                                    = 7,  // Only ammo can go there.
    EQUIP_ERR_PROFICIENCY_NEEDED                           = 8,  // You do not have the required proficiency for that item.
    EQUIP_ERR_NO_SLOT_AVAILABLE                            = 9,  // No equipment slot is available for that item.
    EQUIP_ERR_CANT_EQUIP_EVER                              = 10, // You can never use that item.
    EQUIP_ERR_CANT_EQUIP_EVER_2                            = 11, // You can never use that item.
    EQUIP_ERR_NO_SLOT_AVAILABLE_2                          = 12, // No equipment slot is available for that item.
    EQUIP_ERR_2HANDED_EQUIPPED                             = 13, // Cannot equip that with a two-handed weapon.
    EQUIP_ERR_2HSKILLNOTFOUND                              = 14, // You cannot dual-wield
    EQUIP_ERR_WRONG_BAG_TYPE                               = 15, // That item doesn't go in that container.
    EQUIP_ERR_WRONG_BAG_TYPE_2                             = 16, // That item doesn't go in that container.
    EQUIP_ERR_ITEM_MAX_COUNT                               = 17, // You can't carry any more of those items.
    EQUIP_ERR_NO_SLOT_AVAILABLE_3                          = 18, // No equipment slot is available for that item.
    EQUIP_ERR_CANT_STACK                                   = 19, // This item cannot stack.
    EQUIP_ERR_NOT_EQUIPPABLE                               = 20, // This item cannot be equipped.
    EQUIP_ERR_CANT_SWAP                                    = 21, // These items can't be swapped.
    EQUIP_ERR_SLOT_EMPTY                                   = 22, // That slot is empty.
    EQUIP_ERR_ITEM_NOT_FOUND                               = 23, // The item was not found.
    EQUIP_ERR_DROP_BOUND_ITEM                              = 24, // You can't drop a soulbound item.
    EQUIP_ERR_OUT_OF_RANGE                                 = 25, // Out of range.
    EQUIP_ERR_TOO_FEW_TO_SPLIT                             = 26, // Tried to split more than number in stack.
    EQUIP_ERR_SPLIT_FAILED                                 = 27, // Couldn't split those items.
    EQUIP_ERR_SPELL_FAILED_REAGENTS_GENERIC                = 28, // Missing reagent
    EQUIP_ERR_NOT_ENOUGH_MONEY                             = 29, // You don't have enough money.
    EQUIP_ERR_NOT_A_BAG                                    = 30, // Not a bag.
    EQUIP_ERR_DESTROY_NONEMPTY_BAG                         = 31, // You can only do that with empty bags.
    EQUIP_ERR_NOT_OWNER                                    = 32, // You don't own that item.
    EQUIP_ERR_ONLY_ONE_QUIVER                              = 33, // You can only equip one quiver.
    EQUIP_ERR_NO_BANK_SLOT                                 = 34, // You must purchase that bag slot first
    EQUIP_ERR_NO_BANK_HERE                                 = 35, // You are too far away from a bank.
    EQUIP_ERR_ITEM_LOCKED                                  = 36, // Item is locked.
    EQUIP_ERR_GENERIC_STUNNED                              = 37, // You are stunned
    EQUIP_ERR_PLAYER_DEAD                                  = 38, // You can't do that when you're dead.
    EQUIP_ERR_CLIENT_LOCKED_OUT                            = 39, // You can't do that right now.
    EQUIP_ERR_INTERNAL_BAG_ERROR                           = 40, // Internal Bag Error
    EQUIP_ERR_ONLY_ONE_BOLT                                = 41, // You can only equip one quiver.
    EQUIP_ERR_ONLY_ONE_AMMO                                = 42, // You can only equip one ammo pouch.
    EQUIP_ERR_CANT_WRAP_STACKABLE                          = 43, // Stackable items can't be wrapped.
    EQUIP_ERR_CANT_WRAP_EQUIPPED                           = 44, // Equipped items can't be wrapped.
    EQUIP_ERR_CANT_WRAP_WRAPPED                            = 45, // Wrapped items can't be wrapped.
    EQUIP_ERR_CANT_WRAP_BOUND                              = 46, // Bound items can't be wrapped.
    EQUIP_ERR_CANT_WRAP_UNIQUE                             = 47, // Unique items can't be wrapped.
    EQUIP_ERR_CANT_WRAP_BAGS                               = 48, // Bags can't be wrapped.
    EQUIP_ERR_LOOT_GONE                                    = 49, // Already looted
    EQUIP_ERR_INV_FULL                                     = 50, // Inventory is full.
    EQUIP_ERR_BANK_FULL                                    = 51, // Your bank is full
    EQUIP_ERR_VENDOR_SOLD_OUT                              = 52, // That item is currently sold out.
    EQUIP_ERR_BAG_FULL_2                                   = 53, // That bag is full.
    EQUIP_ERR_ITEM_NOT_FOUND_2                             = 54, // The item was not found.
    EQUIP_ERR_CANT_STACK_2                                 = 55, // This item cannot stack.
    EQUIP_ERR_BAG_FULL_3                                   = 56, // That bag is full.
    EQUIP_ERR_VENDOR_SOLD_OUT_2                            = 57, // That item is currently sold out.
    EQUIP_ERR_OBJECT_IS_BUSY                               = 58, // That object is busy.
    EQUIP_ERR_CANT_BE_DISENCHANTED                         = 59,
    EQUIP_ERR_NOT_IN_COMBAT                                = 60, // You can't do that while in combat
    EQUIP_ERR_NOT_WHILE_DISARMED                           = 61, // You can't do that while disarmed
    EQUIP_ERR_BAG_FULL_4                                   = 62, // That bag is full.
    EQUIP_ERR_CANT_EQUIP_RANK                              = 63, // You don't have the required rank for that item
    EQUIP_ERR_CANT_EQUIP_REPUTATION                        = 64, // You don't have the required reputation for that item
    EQUIP_ERR_TOO_MANY_SPECIAL_BAGS                        = 65, // You cannot equip another bag of that type
    EQUIP_ERR_LOOT_CANT_LOOT_THAT_NOW                      = 66, // You can't loot that item now.
    EQUIP_ERR_ITEM_UNIQUE_EQUIPPABLE                       = 67, // You cannot equip more than one of those.
    EQUIP_ERR_VENDOR_MISSING_TURNINS                       = 68, // You do not have the required items for that purchase
    EQUIP_ERR_NOT_ENOUGH_HONOR_POINTS                      = 69, // You don't have enough honor points
    EQUIP_ERR_NOT_ENOUGH_ARENA_POINTS                      = 70, // You don't have enough arena points
    EQUIP_ERR_ITEM_MAX_COUNT_SOCKETED                      = 71, // You have the maximum number of those gems in your inventory or socketed into items.
    EQUIP_ERR_MAIL_BOUND_ITEM                              = 72, // You can't mail soulbound items.
    EQUIP_ERR_INTERNAL_BAG_ERROR_2                         = 73, // Internal Bag Error
    EQUIP_ERR_BAG_FULL_5                                   = 74, // That bag is full.
    EQUIP_ERR_ITEM_MAX_COUNT_EQUIPPED_SOCKETED             = 75, // You have the maximum number of those gems socketed into equipped items.
    EQUIP_ERR_ITEM_UNIQUE_EQUIPPABLE_SOCKETED              = 76, // You cannot socket more than one of those gems into a single item.
    EQUIP_ERR_TOO_MUCH_GOLD                                = 77, // At gold limit
    EQUIP_ERR_NOT_DURING_ARENA_MATCH                       = 78, // You can't do that while in an arena match
    EQUIP_ERR_TRADE_BOUND_ITEM                             = 79, // You can't trade a soulbound item.
    EQUIP_ERR_CANT_EQUIP_RATING                            = 80, // You don't have the personal, team, or battleground rating required to buy that item
    EQUIP_ERR_EVENT_AUTOEQUIP_BIND_CONFIRM                 = 81,
    EQUIP_ERR_NOT_SAME_ACCOUNT                             = 82, // Account-bound items can only be given to your own characters.
    EQUIP_ERR_NO_OUTPUT                                    = 83,
    EQUIP_ERR_ITEM_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS    = 84, // You can only carry %d %s
    EQUIP_ERR_ITEM_MAX_LIMIT_CATEGORY_SOCKETED_EXCEEDED_IS = 85, // You can only equip %d |4item:items in the %s category
    EQUIP_ERR_SCALING_STAT_ITEM_LEVEL_EXCEEDED             = 86, // Your level is too high to use that item
    EQUIP_ERR_PURCHASE_LEVEL_TOO_LOW                       = 87, // You must reach level %d to purchase that item.
    EQUIP_ERR_CANT_EQUIP_NEED_TALENT                       = 88, // You do not have the required talent to equip that.
    EQUIP_ERR_ITEM_MAX_LIMIT_CATEGORY_EQUIPPED_EXCEEDED_IS = 89, // You can only equip %d |4item:items in the %s category
    EQUIP_ERR_SHAPESHIFT_FORM_CANNOT_EQUIP                 = 90, // Cannot equip item in this form
    EQUIP_ERR_ITEM_INVENTORY_FULL_SATCHEL                  = 91, // Your inventory is full. Your satchel has been delivered to your mailbox.
    EQUIP_ERR_SCALING_STAT_ITEM_LEVEL_TOO_LOW              = 92, // Your level is too low to use that item
    EQUIP_ERR_CANT_BUY_QUANTITY                            = 93, // You can't buy the specified quantity of that item.
    EQUIP_ERR_ITEM_IS_BATTLE_PAY_LOCKED                    = 94, // Your purchased item is still waiting to be unlocked
    EQUIP_ERR_REAGENT_BANK_FULL                            = 95, // Your reagent bank is full
    EQUIP_ERR_REAGENT_BANK_LOCKED                          = 96,
    EQUIP_ERR_WRONG_BAG_TYPE_3                             = 97,
    EQUIP_ERR_CANT_USE_ITEM                                = 98, // You can't use that item.
    EQUIP_ERR_CANT_BE_OBLITERATED                          = 99, // You can't obliterate that item
    EQUIP_ERR_GUILD_BANK_CONJURED_ITEM                     = 100,// You cannot store conjured items in the guild bank
};

enum BuyResult
{
    BUY_ERR_CANT_FIND_ITEM                      = 0,
    BUY_ERR_ITEM_ALREADY_SOLD                   = 1,
    BUY_ERR_NOT_ENOUGHT_MONEY                   = 2,
    BUY_ERR_SELLER_DONT_LIKE_YOU                = 4,
    BUY_ERR_DISTANCE_TOO_FAR                    = 5,
    BUY_ERR_ITEM_SOLD_OUT                       = 7,
    BUY_ERR_CANT_CARRY_MORE                     = 8,
    BUY_ERR_RANK_REQUIRE                        = 11,
    BUY_ERR_REPUTATION_REQUIRE                  = 12
};

enum SellResult
{
    SELL_ERR_CANT_FIND_ITEM                      = 1,
    SELL_ERR_CANT_SELL_ITEM                      = 2,       // merchant doesn't like that item
    SELL_ERR_CANT_FIND_VENDOR                    = 3,       // merchant doesn't like you
    SELL_ERR_YOU_DONT_OWN_THAT_ITEM              = 4,       // you don't own that item
    SELL_ERR_UNK                                 = 5,       // nothing appears...
    SELL_ERR_ONLY_EMPTY_BAG                      = 6        // can only do with empty bags
};

enum ItemModifiers
{
    ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS         = 0,
    ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1            = 1,
    ITEM_MODIFIER_UPGRADE_ID                            = 2,
    ITEM_MODIFIER_BATTLE_PET_SPECIES_ID                 = 3,
    ITEM_MODIFIER_BATTLE_PET_BREED_DATA                 = 4, // (breedId) | (breedQuality << 24)
    ITEM_MODIFIER_BATTLE_PET_LEVEL                      = 5,
    ITEM_MODIFIER_BATTLE_PET_DISPLAY_ID                 = 6,
    ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS            = 7,
    ITEM_MODIFIER_ARTIFACT_APPEARANCE_ID                = 8,
    ITEM_MODIFIER_SCALING_STAT_DISTRIBUTION_FIXED_LEVEL = 9,
    ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1               = 10,
    ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2            = 11,
    ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2               = 12,
    ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3            = 13,
    ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3               = 14,
    ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4            = 15,
    ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4               = 16,
    ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID       = 17,
    ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL              = 18,
    ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1         = 19,
    ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2         = 20,
    ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3         = 21,
    ITEM_MODIFIER_CHALLENGE_KEYSTONE_IS_CHARGED         = 22,
    ITEM_MODIFIER_ARTIFACT_KNOWLEDGE_LEVEL              = 23,
    ITEM_MODIFIER_ARTIFACT_TIER                         = 24,

    MAX_ITEM_MODIFIERS
};

// -1 from client enchantment slot number
enum EnchantmentSlot
{
    PERM_ENCHANTMENT_SLOT           = 0,
    TEMP_ENCHANTMENT_SLOT           = 1,
    SOCK_ENCHANTMENT_SLOT           = 2,
    SOCK_ENCHANTMENT_SLOT_2         = 3,
    SOCK_ENCHANTMENT_SLOT_3         = 4,
    BONUS_ENCHANTMENT_SLOT          = 5,
    PRISMATIC_ENCHANTMENT_SLOT      = 6,                    // added at apply special permanent enchantment
    ENGINEERING_ENCHANTMENT_SLOT    = 7,
    MAX_INSPECTED_ENCHANTMENT_SLOT  = 8,

    PROP_ENCHANTMENT_SLOT_0         = 8,                    // used with RandomSuffix
    PROP_ENCHANTMENT_SLOT_1         = 9,                    // used with RandomSuffix
    PROP_ENCHANTMENT_SLOT_2         = 10,                   // used with RandomSuffix and RandomProperty
    PROP_ENCHANTMENT_SLOT_3         = 11,                   // used with RandomProperty
    PROP_ENCHANTMENT_SLOT_4         = 12,                   // used with RandomProperty
    MAX_ENCHANTMENT_SLOT
};

#define MAX_VISIBLE_ITEM_OFFSET       2                     // 2 fields per visible item (entry+enchantment)

#define MAX_GEM_SOCKETS               MAX_ITEM_PROTO_SOCKETS// (BONUS_ENCHANTMENT_SLOT-SOCK_ENCHANTMENT_SLOT) and item proto size, equal value expected

enum EnchantmentOffset
{
    ENCHANTMENT_ID_OFFSET       = 0,
    ENCHANTMENT_DURATION_OFFSET = 1,
    ENCHANTMENT_CHARGES_OFFSET  = 2                         // now here not only charges, but something new in wotlk
};

#define MAX_ENCHANTMENT_OFFSET    3

enum EnchantmentSlotMask
{
    ENCHANTMENT_CAN_SOULBOUND           = 0x01,
    ENCHANTMENT_UNK1                    = 0x02,
    ENCHANTMENT_UNK2                    = 0x04,
    ENCHANTMENT_UNK3                    = 0x08,
    ENCHANTMENT_COLLECTABLE             = 0x100,
    ENCHANTMENT_HIDE_IF_NOT_COLLECTED   = 0x200,
};

enum ItemUpdateState
{
    ITEM_UNCHANGED             = 0,
    ITEM_CHANGED               = 1,
    ITEM_NEW                   = 2,
    ITEM_REMOVED               = 3
};

namespace ItemBonus
{
    enum Stats : uint32
    {
        Avoidance       = 40,
        Leech           = 41,
        Speed           = 42,
        Indestructible  = 43,
        PrismaticSocket = 523
    };

    namespace Chances
    {
        enum
        {
            Stats           = 10,
            Warforged       = 10,
            PrismaticSocket = 10,
            IlevelRoll      = 35
        };
    }

    namespace HeroicOrRaid
    {
        enum
        {
            Warforged       = 499
        };
    }

    static int g_ItemBonusIlvlNegativeLimit = -100;
    static int g_ItemBonusIlvlPositiveLimit = 200;
    static int g_MaxIlvlRoll = 985;

    namespace Bonuses
    {
        enum
        {
            WorldQuestRare              = 1825, ///< rare 110
            WorldQuestEpic              = 3528, ///< epic 110
            StartNormalDungeon          = 1826, ///< ilvl 805+
            StartHeroicDungeon          = 1726, ///< ilvl 825+
            StartMythicDungeon          = 1727, ///< ilvl 840+
            StartMythicPlusDungeon      = 3410, ///< ilvl 840+
            StartLfrRaid                = 3379, ///< iLvl 835+
            StartNormalRaid             = 1807, ///< iLvl 850+
            StartLowerKarazhan          = 3452, ///< iLvl 855+
            StartUpperKarazhan          = 3453, ///< iLvl 860+
            StartNightbane              = 3454, ///< iLvl 875+
            StartToVOdynAndGuarmLFR     = 3470, ///< iLvl 845+
            StartToVOdynAndGuarmNormal  = 3467, ///< iLvl 860+
            StartToVOdynAndGuarmHeroic  = 3468, ///< iLvl 875+
            StartToVOdynAndGuarmMythic  = 3469, ///< iLvl 890+
            StartToVHelyaLFR            = 3505, ///< iLvl 850+
            StartToVHelyaNormal         = 3506, ///< iLvl 865+
            StartToVHelyaHeroic         = 3507, ///< iLvl 880+
            StartToVHelyaMythic         = 3508, ///< iLvl 895+
            StartNightholdLFR1stWing    = 3446, ///< iLvl 855+
            StartNightholdNormal1stWing = 3443, ///< iLvl 870+
            StartNightholdHeroic1stWing = 3444, ///< iLvl 885+
            StartNightholdMythic1stWing = 3445, ///< iLvl 900+
            StartNightholdLFR2ndWing    = 3520, ///< iLvl 860+
            StartNightholdNormal2ndWing = 3514, ///< iLvl 875+
            StartNightholdHeroic2ndWing = 3516, ///< iLvl 890+
            StartNightholdMythic2ndWing = 3518, ///< iLvl 905+
            StartNightholdLFR4thWing    = 3521, ///< iLvl 865+
            StartNightholdNormal4thWing = 3515, ///< iLvl 880+
            StartNightholdHeroic4thWing = 3517, ///< iLvl 895+
            StartNightholdMythic4thWing = 3519, ///< iLvl 910+
            StartTombOfSargerasNormal   = 3561, ///< iLvl 900/910+
            StartTombOfSargerasHeroic   = 3562, ///< iLvl 915/925+
            StartTombOfSargerasMythic   = 3563, ///< iLvl 930/940+
            StartTombOfSargerasLFR      = 3564, ///< iLvl 885/895+
            StartKilJaedenNormal        = 1492, ///< iLvl 910+
            StartKilJaedenHeroic        = 1507, ///< iLvl 925+
            StartKilJaedenMythic        = 1522, ///< iLvl 940+
            StartKilJaedenLFR           = 1477, ///< iLvl 895+
            StartAntorusNormal          = 3610, ///< iLvl 930+
            StartAntorusHeroic          = 3611, ///< iLvl 945+
            StartAntorusMythic          = 3612, ///< iLvl 960+
            StartAntorusLFR             = 3613, ///< iLvl 915+
            StartHeroicRaid             = 1805, ///< iLvl 865+
            StartMythicRaid             = 1806, ///< iLvl 880+
            StartLegionWorldBoss        = 3466, ///< iLvl 860+
            Start720LegionWorldBoss     = 3572, ///< iLvl 900+
            Start730LegionWorldBoss     = 3614, ///< Ilvl 930+
            Warforge                    = 3339, ///< Warforged (+10 roll)
            EpicWarforge                = 3441, ///< Warforged (+5 roll - epic)
            TitanForge                  = 3337, ///< Titanforged (+15 roll)
            /// Legendary940                = 3530, ///< Item level 940
            /// Legendary910                = 3458, ///< Item level 910
            /// Legendary1000               = 3571, ///< Item level 1000

            /// Broken Shore
            /// RelinquishedBrokenShore     = 3573, ///< Relinquished for broken shore zone ilvl 880+
            /// Dauntless                   = 3574, ///< Dauntless for broken shore zone ilvl 850+

            /// ITEM_BONUS_ITEM_LEVEL base (+0) from -100 (bonus id 1472 - 100 = 1372) to +200 (bonus id 1472 + 200 = 1672)
            /// see g_ItemBonusIlvlNegativeLimit / g_ItemBonusIlvlPositiveLimit
            IlvlModificationBase        = 1472,
            IlvlModificationBaseOver200 = 3130
        };
    }
}

namespace ItemCustomFlags
{
    enum
    {
        FromStore       = 0x01,
        LoyaltyLootBox  = 0x02
    };
}

#define MAX_ITEM_SPELLS 5
#define MIN_ITEM_LEVEL 1
#define MAX_ITEM_LEVEL 1300

bool ItemCanGoIntoBag(ItemTemplate const* proto, ItemTemplate const* pBagProto);

extern ItemModifiers const g_AppearanceModifierSlotBySpec[MAX_SPECIALIZATIONS];
extern ItemModifiers const g_IllusionModifierSlotBySpec[MAX_SPECIALIZATIONS];

struct BonusData
{
    uint32 Quality;
    int32 ItemLevelBonus;
    int32 RequiredLevel;
    int32 ItemStatType[MAX_ITEM_PROTO_STATS];
    int32 ItemStatValue[MAX_ITEM_PROTO_STATS];
    int32 ItemStatAllocation[MAX_ITEM_PROTO_STATS];
    float ItemStatSocketCostMultiplier[MAX_ITEM_PROTO_STATS];
    uint32 SocketColor[MAX_ITEM_PROTO_SOCKETS];
    uint32 AppearanceModID;
    float RepairCostMultiplier;
    uint32 ScalingStatDistribution;
    uint32 DisplayToastMethod[2];
    uint32 ItemLevelOverride;
    uint32 SandboxScalingId;
    uint32 GemItemLevelBonus[MAX_ITEM_PROTO_SOCKETS];
    int32 RelicType;
    bool HasItemLevelBonus;

    void Initialize(ItemTemplate const* proto);
    void AddBonus(uint32 type, int32 const (&values)[3]);
    void RemoveBonus(uint32 type, int32 const (&values)[3]);

private:
    struct
    {
        int32 AppearanceModPriority;
        int32 ScalingStatDistributionPriority;
        int32 ItemLevelOverridePriority;
        bool HasQualityBonus;
    } _state;
};

#pragma pack(push, 1)
struct ItemDynamicFieldGems
{
    uint32 ItemId;
    uint16 BonusListIDs[16];
    uint8 Context;
    uint8 Padding[3];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ItemDynamicFieldRelicTalentData
{
    uint32 Enable;
    uint32 RelicSlot;
    uint16 PathMask;
    uint16 Talents[6];
    uint8  Padding[2];
};

#define MAX_RELIC_TALENT 6

#pragma pack(pop)
struct RelicTalentData
{
    RelicTalentData()
    {
        NeedSave = false;

        memset(PathMask, 0, sizeof(PathMask));
        memset(RelicTalent, 0, sizeof(RelicTalent));
    }

    uint32 PathMask[MAX_GEM_SOCKETS];
    uint32 RelicTalent[MAX_GEM_SOCKETS][MAX_RELIC_TALENT];
    bool NeedSave;

    bool IsInitialized(uint8 p_RelicTalentSlot)
    {
        return RelicTalent[p_RelicTalentSlot][0] != 0;
    }

    ItemDynamicFieldRelicTalentData GetDynamicFieldStruct(uint8 p_SocketSlot);
    void GenerateTalents(uint8 p_Slot, uint32 p_Except);
    void EquipNewRelic(Item* p_Relic, uint8 p_Slot);
};

class Item : public Object
{
    public:
        static Item* CreateItem(uint32 item, uint32 count, Player const* player = NULL);
        Item* CloneItem(uint32 count, Player const* player = NULL) const;

        Item();
        ~Item();

        virtual bool Create(uint32 guidlow, uint32 itemid, Player const* owner);

        ItemTemplate const* GetTemplate() const;

        uint64 GetOwnerGUID()    const { return GetGuidValue(ITEM_FIELD_OWNER); }
        void SetOwnerGUID(uint64 guid) { SetGuidValue(ITEM_FIELD_OWNER, guid); }
        Player* GetOwner()const;
        uint64 GetRealOwnerGUID() const;

        void SetBinding(bool val) { ApplyModFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_SOULBOUND, val); }
        bool IsSoulBound() const { return HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_SOULBOUND); }
        bool IsBoundAccountWide() const { return (GetTemplate()->Flags & int32(ItemFlags::IS_BOUND_TO_ACCOUNT)) != 0; }
        bool IsBindedNotWith(Player const* player) const;
        bool IsBoundByEnchant() const;
        virtual void SaveToLogs(Player const* p_Player, uint32 p_ItemEntry, time_t p_Timestamp, uint32 p_ItemGUID);
        virtual void UpdateLogs(uint32 p_ItemGUID, std::vector<uint32> p_Bonuses);
        virtual void SaveToDB(SQLTransaction& trans);
        virtual bool LoadFromDB(uint32 guid, uint64 owner_guid, Field* fields, uint32 entry);
        static void DeleteFromDB(SQLTransaction& trans, uint32 itemGuid, uint32 realmId);
        virtual void DeleteFromDB(SQLTransaction& trans);
        static void DeleteFromInventoryDB(SQLTransaction& trans, uint32 itemGuid, uint32 realmId);
        void DeleteFromInventoryDB(SQLTransaction& trans);
        void SaveRefundDataToDB();
        void DeleteRefundDataFromDB(SQLTransaction* trans);

        Bag* ToBag() { if (IsBag()) return reinterpret_cast<Bag*>(this); else return NULL; }
        const Bag* ToBag() const { if (IsBag()) return reinterpret_cast<const Bag*>(this); else return NULL; }
        bool IsEquipable() const { return GetTemplate()->InventoryType != INVTYPE_NON_EQUIP; }

        bool IsSuitableForItemLevelCalulcation(bool p_IncludeOffHand) const
        {  return (GetTemplate()->Class == ITEM_CLASS_WEAPON || GetTemplate()->Class == ITEM_CLASS_ARMOR) &&
                  GetTemplate()->InventoryType != INVTYPE_NON_EQUIP &&
                  GetTemplate()->InventoryType != INVTYPE_TABARD &&
                  GetTemplate()->InventoryType != INVTYPE_RANGED &&
                  GetTemplate()->InventoryType != INVTYPE_ROBE &&
                  (!p_IncludeOffHand ||  GetTemplate()->InventoryType != INVTYPE_TABARD);
        }

        bool IsLocked() const { return !HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_UNLOCKED); }
        bool IsBag() const { return GetTemplate()->InventoryType == INVTYPE_BAG; }
        bool IsCurrencyToken() const { return GetTemplate()->IsCurrencyToken(); }
        bool IsNotEmptyBag() const;
        bool CantBeUse() const { return (IsBroken() || IsDisable()); }
        bool IsBroken() const { return GetUInt32Value(ITEM_FIELD_MAX_DURABILITY) > 0 && GetUInt32Value(ITEM_FIELD_DURABILITY) == 0; }
        bool IsDisable() const { return HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ItemFieldFlags::ITEM_FIELD_FLAG_DISABLE); }
        bool CanBeTraded(bool mail = false, bool trade = false) const;
        void SetInTrade(bool b = true) { mb_in_trade = b; }
        bool IsInTrade() const { return mb_in_trade; }

        bool HasEnchantRequiredSkill(const Player* player) const;
        uint32 GetEnchantRequiredLevel() const;

        bool IsFitToSpellRequirements(SpellInfo const* spellInfo) const;
        bool IsLimitedToAnotherMapOrZone(uint32 cur_mapId, uint32 cur_zoneId) const;
        bool GemsFitSockets() const;

        uint32 GetCount() const { return GetUInt32Value(ITEM_FIELD_STACK_COUNT); }
        void SetCount(uint32 value) { SetUInt32Value(ITEM_FIELD_STACK_COUNT, value); }
        uint32 GetMaxStackCount() const { return GetTemplate()->GetMaxStackSize(); }
        uint8 GetGemCountWithID(uint32 GemID) const;
        uint8 GetGemCountWithLimitCategory(uint32 limitCategory) const;
        InventoryResult CanBeMergedPartlyWith(ItemTemplate const* proto) const;
        DynamicFieldStructuredView<ItemDynamicFieldGems> GetGems() const;
        ItemDynamicFieldGems const* GetGem(uint16 slot) const;
        void SetGem(uint16 slot, ItemDynamicFieldGems const* gem, uint32 gemScalingLevel);

        uint8 GetSlot() const {return m_slot;}
        Bag* GetContainer() { return m_container; }
        uint8 GetBagSlot() const;
        void SetSlot(uint8 slot) { m_slot = slot; }
        uint16 GetPos() const { return uint16(GetBagSlot()) << 8 | GetSlot(); }
        void SetContainer(Bag* container) { m_container = container; }

        bool IsInBag() const { return m_container != NULL; }
        bool IsEquipped() const;
        bool IsArtifactOrArtifactChild(Player* p_Player) const;

        uint32 GetSkill() const;

        // RandomPropertyId (signed but stored as unsigned)
        int32 GetItemRandomPropertyId() const { return GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID); }
        uint32 GetItemSuffixFactor() const { return GetUInt32Value(ITEM_FIELD_PROPERTY_SEED); }
        void SetItemRandomProperties(int32 randomPropId, Player* p_Player = nullptr);
        void UpdateItemSuffixFactor();
        static int32 GenerateItemRandomPropertyId(uint32 item_id);

        /**
        * Generate item bonus from item id & current difficulty
        * @param p_ItemId : Item id of the item which we wanna generate bonus
        * @param p_MapDifficulty: Information about the current difficulty we are to determine the right bonus to apply
        * @param p_ItemBonus: Vector of bonus to fill
        */
        static void GenerateItemBonus(uint32 p_ItemId, ItemContext p_Context, std::vector<uint32>& p_ItemBonus, bool p_OnlyDifficulty = false, Player* p_Player = nullptr, Loot* p_Loot = nullptr, bool p_RollIlvl = true);

        static void BuildDynamicItemDatas(WorldPacket& p_Datas, Item const* p_Item, ItemContext p_Context = ItemContext::None);
        static void BuildDynamicItemDatas(ByteBuffer& p_Datas, Item const* p_Item);
        static void BuildDynamicItemDatas(WorldPacket& p_Datas, VoidStorageItem const p_Item);
        static void BuildDynamicItemDatas(ByteBuffer& p_Datas, LootItem const p_Item);
        static void BuildDynamicItemDatas(ByteBuffer& p_Datas, uint32 p_Entry, std::vector<uint32> p_ItemBonuses = std::vector<uint32>());
        static void BuildDynamicItemDatas(WorldPacket& p_Datas, uint32 p_Entry, std::vector<uint32> p_ItemBonuses = std::vector<uint32>());

        void SetEnchantment(EnchantmentSlot slot, uint32 id, uint32 duration, uint32 charges);
        void SetEnchantmentDuration(EnchantmentSlot slot, uint32 duration, Player* owner);
        void SetEnchantmentCharges(EnchantmentSlot slot, uint32 charges);
        void ClearEnchantment(EnchantmentSlot slot);
        uint32 GetEnchantmentId(EnchantmentSlot slot)       const { return GetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_ID_OFFSET);}
        uint32 GetEnchantmentDuration(EnchantmentSlot slot) const { return GetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET);}
        uint32 GetEnchantmentCharges(EnchantmentSlot slot)  const { return GetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET);}

        std::string const& GetText() const { return m_text; }
        inline void SetText(std::string const& text)
        {
            m_text = text;

            // Fixes some MySQL shit ...
            if (m_text.size() < 2)
                m_text = "";

            for (uint64 i = 0; i < m_text.size(); i++)
            {
                if (uint32(m_text.c_str()[i]) == 0xA0)
                {
                    m_text = "";
                    return;
                }
            }
        }

        void SetCustomFlags(uint32 p_Flags) { m_CustomFlags = p_Flags; }
        void ApplyCustomFlags(uint32 p_Flags) { m_CustomFlags |= p_Flags; }
        uint32 GetCustomFlags() const { return m_CustomFlags; }
        bool HasCustomFlags(uint32 p_Flags) const { return m_CustomFlags & p_Flags; }

        void SendTimeUpdate(Player* owner);
        void UpdateDuration(Player* owner, uint32 diff);

        // spell charges (signed but stored as unsigned)
        int32 GetSpellCharges(uint8 index/*0..5*/ = 0) const { return GetInt32Value(ITEM_FIELD_SPELL_CHARGES + index); }
        void  SetSpellCharges(uint8 index/*0..5*/, int32 value) { SetInt32Value(ITEM_FIELD_SPELL_CHARGES + index, value); }

        Loot loot;
        bool m_lootGenerated;

        // Update States
        ItemUpdateState GetState() const { return uState; }
        void SetState(ItemUpdateState state, Player* forplayer = NULL);
        void AddToUpdateQueueOf(Player* player);
        void RemoveFromUpdateQueueOf(Player* player);
        bool IsInUpdateQueue() const { return uQueuePos != -1; }
        uint16 GetQueuePos() const { return uQueuePos; }
        void FSetState(ItemUpdateState state)               // forced
        {
            uState = state;
        }

        bool hasQuest(uint32 quest_id) const override { return GetTemplate()->StartQuest == quest_id; }
        bool hasInvolvedQuest(uint32 /*quest_id*/) const override { return false; }
        bool HasStats() const;
        bool HasSpells() const;
        bool IsPotion() const;
        bool IsHealthstone() const { return GetEntry() == 5512; }
        bool IsVellum() const { return GetTemplate()->IsVellum(); }
        bool IsConjuredConsumable() const { return GetTemplate()->IsConjuredConsumable(); }
        bool IsRangedWeapon() const { return GetTemplate()->IsRangedWeapon(); }

        // Item Refund system
        void SetNotRefundable(Player* owner, bool changestate = true, SQLTransaction* trans = NULL, bool addToCollection = true);
        void SetRefundRecipient(uint32 pGuidLow) { m_refundRecipient = pGuidLow; }
        void SetPaidMoney(uint64 money) { m_paidMoney = money; }
        void SetPaidExtendedCost(uint32 iece) { m_paidExtendedCost = iece; }
        uint32 GetRefundRecipient() { return m_refundRecipient; }
        uint64 GetPaidMoney() { return m_paidMoney; }
        uint32 GetPaidExtendedCost() { return m_paidExtendedCost; }

        void UpdatePlayedTime(Player* owner);
        uint32 GetPlayedTime();
        bool IsRefundExpired();

        // Soulbound trade system
        void SetSoulboundTradeable(AllowedLooterSet& allowedLooters);
        void ClearSoulboundTradeable(Player* currentOwner);
        bool CheckSoulboundTradeExpire();

        void AddToObjectUpdate() override;
        void RemoveFromObjectUpdate() override;
        void BuildUpdate(UpdateDataMapType&) override;
        void BuildDynamicValuesUpdate(uint8 p_UpdateType, ByteBuffer* p_Data, Player* p_Target) const override;

        uint32 GetScriptId() const { return GetTemplate()->ScriptId; }

        static bool SubclassesCompatible(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified);
        static bool CanTransmogrifyItemWithItem(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified, bool p_CustomTransmo = false);
        uint32 GetBuyPrice(Player const* owner, bool& standardPrice) const;
        static uint32 GetBuyPrice(ItemTemplate const* proto, uint32 quality, uint32 itemLevel, bool& standardPrice);
        uint32 GetSellPrice(Player const* owner) const;
        static uint32 GetSellPrice(ItemTemplate const* proto, uint32 quality, uint32 itemLevel);

        uint16 GetVisibleAppearanceModID(Player const* p_Owner) const;

        uint32 GetVisibleEntry(Player const* p_Owner) const;
        uint32 GetVisibleEnchantmentId(Player const* p_Owner) const;
        uint16 GetVisibleItemVisual(Player const* p_Owner) const;

        uint32 GetModifier(ItemModifiers p_Modifier) const { return m_Modifiers[p_Modifier]; }
        void SetModifier(ItemModifiers p_Modifier, uint32 p_Value);

        uint32 GetBagFlags() const { return m_BagFlags; }
        void SetBagFlags(uint32 p_Flag) { m_BagFlags = p_Flag; }
        void ApplyModBagFlag(uint32 p_Flag, bool p_Apply) { m_BagFlags = p_Apply ? (m_BagFlags | p_Flag) : (m_BagFlags & ~p_Flag); }

        static uint32 GetSellPrice(ItemTemplate const* proto, bool& success);

        bool IsPvPItem() const;
        bool IsStuffItem() const;
        bool CanUpgrade() const;
        bool IsLegendaryCloak() const;

        void SetScaleIlvl(uint32 ilvl) { m_scaleLvl = ilvl; }
        uint32 GetScaleIlvl() const { return m_scaleLvl; }

        void UpgradeItemWithBonuses(std::vector<uint32> const& p_ItemBonuses, bool p_RemoveOldBonuses = false);
        bool AddItemBonus(uint32 p_ItemBonusId, bool p_EnableLogging = true);
        void AddItemBonuses(std::vector<uint32> const& p_ItemBonuses);
        bool HasItemBonus(uint32 p_ItemBonusId) const;
        bool RemoveItemBonus(uint32 p_ItemBonusId);
        void ModifyItemBonus(uint32 p_OldBonus, uint32 p_NewBonus);
        void RemoveAllItemBonuses();
        std::vector<uint32> const& GetAllItemBonuses() const;

        BonusData const* GetBonus() const { return &_bonusData; }
        uint32 GetQuality() const { return _bonusData.Quality; }
        uint32 GetItemLevel(Player const* p_Owner, bool p_IgnorePvPModifiers = false, bool p_ForcePvPItemLevel = false) const;
        static uint32 GetItemLevel(ItemTemplate const* itemTemplate, BonusData const& bonusData, uint32 level, uint32 fixedLevel, uint32 upgradeId, Player const* p_Owner, bool p_IgnorePvPModifiers = false, bool p_ForcePvPItemLevel = false);
        int32 GetRequiredLevel() const { return _bonusData.RequiredLevel; }
        int32 GetItemStatType(uint32 index) const { ASSERT(index < MAX_ITEM_PROTO_STATS); return _bonusData.ItemStatType[index]; }
        int32 GetItemStatValue(uint32 p_Index, Player* p_Owner, uint32 p_Ilevel) const;
        SocketColor GetSocketColor(uint32 index) const { ASSERT(index < MAX_ITEM_PROTO_SOCKETS); return SocketColor(_bonusData.SocketColor[index]); }
        float GetRepairCostMultiplier() const { return _bonusData.RepairCostMultiplier; }
        uint32 GetScalingStatDistribution() const { return _bonusData.ScalingStatDistribution; }
        uint32 GetArmor(Player const* p_Player, uint32 p_ItemLevel) const { return GetTemplate()->GetArmor(p_ItemLevel); }

        ItemDisenchantLootEntry const* GetDisenchantLoot(Player const* owner) const;
        static ItemDisenchantLootEntry const* GetDisenchantLoot(ItemTemplate const* itemTemplate, uint32 quality, uint32 itemLevel);

        static bool SubclassesCompatibleForRandomWeapon(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified);
        static bool CanTransmogrifyIntoRandomWeapon(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified);
        void RandomWeaponTransmogrificationFromPrimaryBag(Player* p_Player, Item* p_Transmogrified, bool p_Apply);

        MS::Artifact::Manager* GetArtifactManager() { return m_ArtifactMgr; }
        void SetArtifactMgr(MS::Artifact::Manager* l_Manager) { m_ArtifactMgr = l_Manager; }

        uint64 GetChildItem() const { return m_ChildItem; }
        void SetChildItem(uint64 p_ChildItem) { m_ChildItem = p_ChildItem; }

        void CopyArtifactDataFromParent(Item* parent);
        uint32 GetAppearanceModId() const { return GetUInt32Value(ITEM_FIELD_ITEM_APPEARANCE_MOD_ID); }
        void SetAppearanceModId(uint32 appearanceModId) { SetUInt32Value(ITEM_FIELD_ITEM_APPEARANCE_MOD_ID, appearanceModId); }

        uint32 GetArtifactKnowledgeOnAcquire() const;
        void SetArtifactKnowledgeOnAcquire(uint32 p_ArtifactKnowledge);

        RelicTalentData* GetRelicTalentData() { return &m_RelicTalentData; }

        void CheckRelicTalentData(uint8 p_GemSlot);

        void AddServerSideBonus(uint32 p_ItemBonusListID);
        void RemoveServerSideBonus(uint32 p_ItemBonusListID);

        void ChangeItemTo(uint32 p_NewItemEntry);

    protected:
        BonusData _bonusData;
        RelicTalentData m_RelicTalentData;

    private:
        std::string m_text;
        uint8 m_slot;
        Bag* m_container;
        ItemUpdateState uState;
        int16 uQueuePos;
        bool mb_in_trade;                                   // true if item is currently in trade-window
        time_t m_lastPlayedTimeUpdate;
        uint32 m_refundRecipient;
        uint64 m_paidMoney;
        uint32 m_paidExtendedCost;
        uint32 m_CustomFlags;
        AllowedLooterSet allowedGUIDs;
        uint64 m_ChildItem;
        uint32 m_scaleLvl;
        uint32 m_ScaleArtifactKnowledge;
        std::array<uint32, MAX_ITEM_PROTO_SOCKETS> m_gemScalingLevels;
        std::set<uint32> ServerSideItemBonusListIDs;

        MS::Artifact::Manager* m_ArtifactMgr;
        uint32 m_Modifiers[ItemModifiers::MAX_ITEM_MODIFIERS];

        uint32 m_BagFlags;
};
#endif
