////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _PLAYER_H
#define _PLAYER_H

#include "AchievementMgr.h"
#include "ArchaeologyMgr.hpp"
#include "Arena.h"
#include "Battleground.h"
#include "Bag.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "DBCEnums.h"
#include "GroupReference.h"
#include "ItemPrototype.h"
#include "Item.h"
#include "MapReference.h"
#include "NPCHandler.h"
#include "Pet.h"
#include "QuestDef.h"
#include "ReputationMgr.h"
#include "Unit.h"
#include "Util.h"                                           // for Tokenizer typedef
#include "WorldSession.h"
#include "PhaseMgr.h"
#include "CUFProfiles.h"
#include "CinematicPathMgr.h"
#include "VignetteMgr.hpp"
#include "BitSet.hpp"
#include "MutexedMap.hpp"
#include "PlayerTaxi.h"
#include "Common.h"
#include "KillRewarder.h"
#include "TradeData.h"
#include "WorldQuestMgr.h"
#include "QueryCallbackManager.h"

// for template
#include "SpellMgr.h"
#include <ace/Stack_Trace.h>
#include <chrono>
#include <deque>

#include "GarrisonMgrConstants.hpp"

struct Mail;
struct ItemExtendedCostEntry;
struct CustomInstanceZone;
class Channel;
class Creature;
class DynamicObject;
class Group;
using GroupPtr = std::shared_ptr<Group>;
#ifndef CROSS
class Guild;
#else /* CROSS */
class InterRealmGuild;
#endif /* CROSS */
class OutdoorPvP;
class Pet;
class PlayerMenu;
class PlayerSocial;
class SpellCastTargets;
class UpdateMask;
class PhaseMgr;
enum eTaskType;

enum eCosmetics
{
    CosmeticBegin = 0,
    CosmeticMorph = 0,
    CosmeticAura = 1,
    CosmeticMount = 2,

    MaxCosmetic
};

using CosmeticSet = std::set<uint32>;

struct CosmeticEntry
{
    eCosmetics  Type;
    uint32      ID;
    uint32      Cost;
    uint32      Expansion;
    uint8       Active;
    std::string Locale_enUS;
    std::string Locale_frFR;
    std::string Locale_esES;
    std::string Locale_ruRU;
    std::string Locale_ptPT;
    std::string Locale_itIT;
};

typedef std::deque<Mail*> PlayerMails;
typedef std::set<uint32> DailyLootsCooldowns;

typedef std::chrono::system_clock Clock;

#define PLAYER_MAX_SKILLS           128
#define DEFAULT_MAX_PRIMARY_TRADE_SKILL 2
#define PLAYER_EXPLORED_ZONES_SIZE 320
#define MAX_HONOR_LEVEL 50
#define PLAYER_CUSTOM_DISPLAY_SIZE 3

/// 7.0.3 22522
enum ToastTypes
{
    TOAST_TYPE_NEW_ITEM     = 0,
    TOAST_TYPE_NEW_CURRENCY = 1,
    TOAST_TYPE_MONEY        = 2,
    TOAST_TYPE_HONOR        = 3,
};

/// 7.0.3 22522
enum DisplayToastMethod
{
    DISPLAY_TOAST_METHOD_UNK_0,
    DISPLAY_TOAST_METHOD_LOOT,
    DISPLAY_TOAST_METHOD_PET_BATTLE_LOOT,
    DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD,
    DISPLAY_TOAST_METHOD_GARRISON_MISSION_BONUS_ROLL_LOOT_1,
    DISPLAY_TOAST_METHOD_LOOT_TOAST_UPGRADE_1,
    DISPLAY_TOAST_METHOD_LOOT_TOAST_UPGRADE_2,
    DISPLAY_TOAST_METHOD_UNK_7,
    DISPLAY_TOAST_METHOD_GARRISON_MISSION_BONUS_ROLL_LOOT_2,
    DISPLAY_TOAST_METHOD_PVP_FACTION_LOOT_TOAST,
    DISPLAY_TOAST_METHOD_GARRISON_CACHE,
    DISPLAY_TOAST_METHOD_UNK_11,
    DISPLAY_TOAST_METHOD_UNK_12_LOOT_RESULT,
    DISPLAY_TOAST_METHOD_LEGENDARY_LOOTED,
    DISPLAY_TOAST_METHOD_UNK_14,
    DISPLAY_TOAST_METHOD_UNK4,
    DISPLAY_TOAST_METHOD_WORLD_QUEST_REWARD,

    DISPLAY_TOAST_MAX
};

enum class LegendaryActivity : uint8
{
    ArenaWon,
    BattlegroundWon,
    NormalDungeon,
    HeroicDungeon,
    MythicDungeon,
    MythicPlusChest,
    MythicPlusWeeklyChest,
    LfrRaidBoss,
    NormalRaidBoss,
    HeroicRaidBoss,
    MythicRaidBoss,
    Treasure,
    RareCreature,
    Strongbox,
    Emissary,
    LootBox,
    RelinquishedToken,
    RecruitAFriend,
    Max
};

static const uint8 g_LegendaryActivityPoints[(uint8)LegendaryActivity::Max] =
{
    1,   ///< Arena won
    2,   ///< Battleground won
    2,   ///< Normal dungeon
    2,   ///< Heroic dungeon
    3,   ///< Mythic dungeon
    4,   ///< Mythic+ end chest
    17,  ///< Mythic+ weekly chest
    2,   ///< Lfr raid boss
    4,   ///< Normal raid boss
    5,   ///< Heroic raid boss
    6,   ///< Mythic raid boss
    1,   ///< Treasure
    1,   ///< RareCreature
    1,   ///< Strongbox
    4,   ///< Emissary
    2,   ///< LootBox
};

enum ToastItems
{
    TalismanOfVictory = 144297
};

// Note: SPELLMOD_* values is aura types in fact
enum SpellModType
{
    SPELLMOD_FLAT         = 107,                            // SPELL_AURA_ADD_FLAT_MODIFIER
    SPELLMOD_PCT          = 108                             // SPELL_AURA_ADD_PCT_MODIFIER
};

// 2^n values, Player::m_isunderwater is a bitmask. These are Trinity internal values, they are never send to any client
enum PlayerUnderwaterState
{
    UNDERWATER_NONE                     = 0x00,
    UNDERWATER_INWATER                  = 0x01,             // terrain type is water and player is afflicted by it
    UNDERWATER_INLAVA                   = 0x02,             // terrain type is lava and player is afflicted by it
    UNDERWATER_INSLIME                  = 0x04,             // terrain type is lava and player is afflicted by it
    UNDERWARER_INDARKWATER              = 0x08,             // terrain type is dark water and player is afflicted by it

    UNDERWATER_EXIST_TIMERS             = 0x10
};

enum BuyBankSlotResult
{
    ERR_BANKSLOT_FAILED_TOO_MANY    = 0,
    ERR_BANKSLOT_INSUFFICIENT_FUNDS = 1,
    ERR_BANKSLOT_NOTBANKER          = 2,
    ERR_BANKSLOT_OK                 = 3
};

enum PlayerSpellState : uint8
{
    PLAYERSPELL_UNCHANGED = 0,
    PLAYERSPELL_CHANGED   = 1,
    PLAYERSPELL_NEW       = 2,
    PLAYERSPELL_REMOVED   = 3,
    PLAYERSPELL_TEMPORARY = 4
};

struct PlayerSpell
{
    PlayerSpellState state : 8;
    bool active            : 1;                             // show in spellbook
    bool dependent         : 1;                             // learned as result another spell learn, skill grow, quest reward, etc
    bool disabled          : 1;                             // first rank has been learned in result talent learn but currently talent unlearned, save max learned ranks
    bool IsMountFavorite   : 1;                             // Is flagged as favorite mount spell
    bool FromShopItem      : 1;
    bool ByPassSpec        : 1;
};

enum CharacterWorldStates
{
    GarrisonStablesFirstQuest = 1,
    GarrisonStablesSecondQuest,
    GarrisonWorkshopGearworksInvention,
    GarrisonWorkshopGearworksInventionCharges,
    GarrisonTradingPostDailyRandomTrader,
    GarrisonTradingPostDailyRandomShipment,
    GarrisonArmoryWeeklyCurrencyGain,
    GarrisonTavernBoolCanRecruitFollower,
    BrokenIslesWeeklyBonusRollTokenCount,
    GlobalBadLuckCounter,
    LeavePvPQueueCounter,
    LastArenaLegendaryAttemptTimestamp,
    EmeraldNightmareCenariusAccessUnlocked,
    LastLegendaryItemEarnedTimestamp,
    GarrisonWeeklySealOfBrokenFateWorkorder,
    EssenceSwapperState,
    EssenceSwapperEntry,
    BrokenShoreMageTowerAttempts,
    ArenaDecay2v2MatchesCount,
    ArenaDecay2v2ConsecutiveCount,
    ArenaDecay3v3MatchesCount,
    ArenaDecay3v3ConsecutiveCount,

    LootBoxNoLegendaryCounter       = 3000000,
    LootBoxBetterIlvlCounter        = 3000001,
    LootBoxBetterIlvlRaid01Counter  = 3000010,
    LootBoxBetterIlvlRaid02Counter  = 3000020,
    LootBoxBetterIlvlRaid03Counter  = 3000030,
    LootBoxBetterIlvlRaid04Counter  = 3000031,
};

enum eClassHallAreas
{
    Paladin     = 7638,
    Rogue       = 8011,
    Rogue2      = 8012,
    Hunter      = 7877,
    DeathKnight = 7679,
    Druid       = 7846,
    Druid2      = 8076,
    Warrior     = 7813,
    DemonHunter = 8023,
    Priest      = 7834,
    Monk        = 7903,
    Monk2       = 7911,
    Mage        = 7879,
    Warlock     = 7875,
    Shaman      = 7745,
    Shaman2     = 7752,
    Shaman3     = 7753
};

enum eClassHallSpells
{
    ClassHallSpellDruid       = 203810,
    ClassHallSpellHunter      = 208643,
    ClassHallSpellDeathKnight = 232719,
    ClassHallSpellPaladin     = 195558
};

static std::map<uint8, std::vector<uint32>> g_ClassHalls =
{
    { CLASS_PALADIN,        { eClassHallAreas::Paladin } },
    { CLASS_ROGUE,          { eClassHallAreas::Rogue, eClassHallAreas::Rogue2 } },
    { CLASS_HUNTER,         { eClassHallAreas::Hunter } },
    { CLASS_DEATH_KNIGHT,   { eClassHallAreas::DeathKnight } },
    { CLASS_DRUID,          { eClassHallAreas::Druid, eClassHallAreas::Druid2 } },
    { CLASS_WARRIOR,        { eClassHallAreas::Warrior } },
    { CLASS_DEMON_HUNTER,   { eClassHallAreas::DemonHunter } },
    { CLASS_PRIEST,         { eClassHallAreas::Priest } },
    { CLASS_MONK,           { eClassHallAreas::Monk, eClassHallAreas::Monk2 } },
    { CLASS_MAGE,           { eClassHallAreas::Mage } },
    { CLASS_WARLOCK,        { eClassHallAreas::Warlock } },
    { CLASS_SHAMAN,         { eClassHallAreas::Shaman, eClassHallAreas::Shaman2, eClassHallAreas::Shaman3 } }
};

// Spell modifier (used for modify other spells)
struct SpellModifier
{
    SpellModifier(Aura* _ownerAura = nullptr) : op(SPELLMOD_DAMAGE), type(SPELLMOD_FLAT), charges(0), value(0), mask(), spellId(0), ownerAura(_ownerAura) {}
    SpellModOp   op   : 8;
    SpellModType type : 8;
    int16 charges     : 16;
    int32 value;
    flag128 mask;
    uint32 spellId;
    Aura const* ownerAura;
};

enum PlayerCurrencyState
{
   PLAYERCURRENCY_UNCHANGED = 0,
   PLAYERCURRENCY_CHANGED   = 1,
   PLAYERCURRENCY_NEW       = 2,
   PLAYERCURRENCY_REMOVED   = 3     //not removed just set count == 0
};

struct PlayerCurrency
{
   PlayerCurrencyState state;
   uint32 totalCount;
   uint32 weekCount;
   uint32 seasonTotal;
   uint32 weekCap;
   uint8 flags;
   bool needResetCap;
};

struct PlayerCurrencyCap
{
    uint16 currencyId;
    uint16 sourceType;
    uint32 sourceId;
    int32 value;
    bool isChanged;
};

enum SceneEventStatus
{
    SCENE_NONE          = 0,
    SCENE_LAUNCH        = 1,
    SCENE_TRIGER        = 2,
    SCENE_COMPLETE      = 3,
};

typedef std::map<uint32, PlayerSpellState> PlayerTalentMap;
typedef std::unordered_map<uint32, PlayerSpell*> PlayerSpellMap;
typedef std::list<SpellModifier*> SpellModList;
typedef std::unordered_map<uint32, PlayerCurrency> PlayerCurrenciesMap;
typedef std::list<PlayerCurrencyCap> PlayerCurrencyCaps;

typedef std::list<uint64> WhisperListContainer;

struct SpellCooldown
{
    uint64 end;
    uint32 itemid;
};

typedef std::map<uint32, SpellCooldown> SpellCooldowns;
typedef ACE_Based::LockedMap<uint32 /*instanceId*/, time_t/*releaseTime*/> InstanceTimeMap;

enum TrainerSpellState
{
    TRAINER_SPELL_GRAY           = 0,
    TRAINER_SPELL_GREEN          = 1,
    TRAINER_SPELL_RED            = 2,
    TRAINER_SPELL_GREEN_DISABLED = 10                       // custom value, not send to client: formally green but learn not allowed
};

enum ActionButtonUpdateState
{
    ACTIONBUTTON_UNCHANGED = 0,
    ACTIONBUTTON_CHANGED = 1,
    ACTIONBUTTON_NEW = 2,
    ACTIONBUTTON_DELETED = 3
};

enum ActionButtonType
{
    ACTION_BUTTON_SPELL = 0x00,
    ACTION_BUTTON_C = 0x01,                         // click?
    ACTION_BUTTON_UNK       = 0x10,
    ACTION_BUTTON_EQSET = 0x20,
    ACTION_BUTTON_DROPDOWN = 0x30,
    ACTION_BUTTON_MACRO = 0x40,
    ACTION_BUTTON_CMACRO = ACTION_BUTTON_C | ACTION_BUTTON_MACRO,
    ACTION_BUTTON_PET       = ACTION_BUTTON_UNK | ACTION_BUTTON_MACRO,
    ACTION_BUTTON_ITEM = 0x80
};

enum ReputationSource
{
    REPUTATION_SOURCE_KILL,
    REPUTATION_SOURCE_QUEST,
    REPUTATION_SOURCE_DAILY_QUEST,
    REPUTATION_SOURCE_WEEKLY_QUEST,
    REPUTATION_SOURCE_MONTHLY_QUEST,
    REPUTATION_SOURCE_REPEATABLE_QUEST,
    REPUTATION_SOURCE_SPELL
};

enum class PvPStatsSpells : uint32
{
    PVP_RULES_ENABLED = 134735,
    SET_STATS_SCRIPT_EFFECT = 198147,
    SPEC_STAT_TEMPLATE_ALL_SPECS = 198439
};

#define ACTION_BUTTON_ACTION(X) (uint64(X) & 0x00000000FFFFFFFF)
#define ACTION_BUTTON_TYPE(X)   ((uint64(X) & 0xFF00000000000000) >> 56)
#define MAX_ACTION_BUTTON_ACTION_VALUE (0x0000000000FFFFFF)

struct ActionButtonPacket
{
    uint32 id;
    uint32 type;
};

struct ActionButton
{
    ActionButton() : packedData(0), uState(ACTIONBUTTON_NEW) {}

    uint64 packedData;
    ActionButtonUpdateState uState;

    // helpers
    ActionButtonType GetType() const {
        return ActionButtonType(ACTION_BUTTON_TYPE(packedData));
    }
    uint32 GetAction() const {
        return ACTION_BUTTON_ACTION(packedData);
    }
    void SetActionAndType(uint32 action, ActionButtonType type)
    {
        uint64 newData = uint64(action) | (uint64(type) << 56);
        if (newData != packedData || uState == ACTIONBUTTON_DELETED)
        {
            packedData = newData;
            if (uState != ACTIONBUTTON_NEW)
                uState = ACTIONBUTTON_CHANGED;
        }
    }
};

#define  MAX_ACTION_BUTTONS 132                             //checked in 6.0.1

typedef std::map<uint8, ActionButton> ActionButtonList;

struct PlayerCreateInfoItem
{
    PlayerCreateInfoItem(uint32 id, uint32 amount) : item_id(id), item_amount(amount) {}

    uint32 item_id;
    uint32 item_amount;
};

typedef std::list<PlayerCreateInfoItem> PlayerCreateInfoItems;

struct PlayerLevelInfo
{
    PlayerLevelInfo() { for (uint8 i=0; i < MAX_STATS; ++i) stats[i] = 0; }

    uint16 stats[MAX_STATS];
};

typedef std::list<uint32> PlayerCreateInfoSpells;

struct PlayerCreateInfoAction
{
    PlayerCreateInfoAction() : button(0), type(0), action(0) {}
    PlayerCreateInfoAction(uint8 _button, uint32 _action, uint8 _type) : button(_button), type(_type), action(_action) {}

    uint8 button;
    uint8 type;
    uint32 action;
};

typedef std::list<PlayerCreateInfoAction> PlayerCreateInfoActions;

typedef std::list<SkillRaceClassInfoEntry const*> PlayerCreateInfoSkills;

struct PlayerInfo
{
                                                            // existence checked by displayId != 0
    PlayerInfo() : mapId(0), areaId(0), positionX(0.0f), positionY(0.0f), positionZ(0.0f), orientation(0.0f),  displayId_m(0), displayId_f(0), levelInfo(nullptr)
    {
    }

    uint32 mapId;
    uint32 areaId;
    float positionX;
    float positionY;
    float positionZ;
    float orientation;
    uint32 displayId_m;
    uint32 displayId_f;
    PlayerCreateInfoItems item;
    PlayerCreateInfoSpells customSpells;
    PlayerCreateInfoSpells castSpells;
    PlayerCreateInfoActions action;
    PlayerCreateInfoSkills skills;

    PlayerLevelInfo* levelInfo;                             //[level-1] 0..MaxPlayerLevel-1
};

struct PvPInfo
{
    PvPInfo() : inHostileArea(false), inNoPvPArea(false), inFFAPvPArea(false), forceFFA(false), endTimer(0) {}

    bool inHostileArea;
    bool inNoPvPArea;
    bool inFFAPvPArea;
    bool forceFFA;
    time_t endTimer;
};

struct DuelInfo
{
    DuelInfo() : initiator(nullptr), opponent(nullptr), startTimer(0), startTime(0), outOfBound(0), isMounted(false), started(false), m_IsInComplete(false) {}

    Player* initiator;
    Player* opponent;
    time_t startTimer;
    time_t startTime;
    time_t outOfBound;
    bool isMounted;
    bool started;
    bool m_IsInComplete;
};

struct Areas
{
    uint32 areaID;
    uint32 areaFlag;
    float x1;
    float x2;
    float y1;
    float y2;
};

#define MAX_RUNES            6
#define MAX_RECHARGING_RUNES 3

enum RuneCooldowns
{
    RUNE_BASE_COOLDOWN  = 10000,
    RUNE_MISS_COOLDOWN  = 1500,     // cooldown applied on runes when the spell misses
};

struct RuneInfo
{
    uint8 BaseRune;
    uint8 CurrentRune;
    uint32 Cooldown;
    uint32 spell_id;
    bool DeathUsed;
    bool Permanently;

    RuneInfo()
    {
        BaseRune = 0;
        CurrentRune = 0;
        Cooldown = 0;
        spell_id = 0;
        DeathUsed = false;
        Permanently = false;
    }
};

struct Runes
{
    std::deque<uint8> CooldownOrder;
    uint32 Cooldown[MAX_RUNES];
    uint8 RuneState;                                        // mask of available runes

    void SetRuneState(uint8 index, bool set = true);
};

struct EnchantDuration
{
    EnchantDuration() : item(NULL), slot(MAX_ENCHANTMENT_SLOT), leftduration(0) {};
    EnchantDuration(Item* _item, EnchantmentSlot _slot, uint32 _leftduration) : item(_item), slot(_slot),
        leftduration(_leftduration){ ASSERT(item); };

    Item* item;
    EnchantmentSlot slot;
    uint32 leftduration;
};

typedef std::list<EnchantDuration> EnchantDurationList;
typedef std::list<Item*> ItemDurationList;

enum DrunkenState
{
    DRUNKEN_SOBER   = 0,
    DRUNKEN_TIPSY   = 1,
    DRUNKEN_DRUNK   = 2,
    DRUNKEN_SMASHED = 3
};

#define MAX_DRUNKEN   4

enum PlayerFlags
{
    PLAYER_FLAGS_GROUP_LEADER           = 0x00000001,
    PLAYER_FLAGS_AFK                    = 0x00000002,
    PLAYER_FLAGS_DND                    = 0x00000004,
    PLAYER_FLAGS_GM                     = 0x00000008,
    PLAYER_FLAGS_GHOST                  = 0x00000010,
    PLAYER_FLAGS_RESTING                = 0x00000020,
    PLAYER_FLAGS_UNK6                   = 0x00000040,
    PLAYER_FLAGS_UNK7                   = 0x00000080,       // pre-3.0.3 PLAYER_FIELD_PLAYER_FLAGS_FFA_PVP flag for FFA PVP state
    PLAYER_FLAGS_CONTESTED_PVP          = 0x00000100,       // Player has been involved in a PvP combat and will be attacked by contested guards
    PLAYER_FLAGS_IN_PVP                 = 0x00000200,
    PLAYER_FLAGS_HIDE_HELM              = 0x00000400,
    PLAYER_FLAGS_HIDE_CLOAK             = 0x00000800,
    PLAYER_FLAGS_PLAYED_LONG_TIME       = 0x00001000,       // played long time
    PLAYER_FLAGS_PLAYED_TOO_LONG        = 0x00002000,       // played too long time
    PLAYER_FLAGS_IS_OUT_OF_BOUNDS       = 0x00004000,
    PLAYER_FLAGS_DEVELOPER              = 0x00008000,       // <Dev> prefix for something?
    PLAYER_FLAGS_UNK16                  = 0x00010000,       // pre-3.0.3 PLAYER_FIELD_PLAYER_FLAGS_SANCTUARY flag for player entered sanctuary
    PLAYER_FLAGS_TAXI_BENCHMARK         = 0x00020000,       // taxi benchmark mode (on/off) (2.0.1)
    PLAYER_FLAGS_PVP_TIMER              = 0x00040000,       // 3.0.2, pvp timer active (after you disable pvp manually)
    PLAYER_FLAGS_UBER                   = 0x00080000,
    PLAYER_FLAGS_UNK20                  = 0x00100000,
    PLAYER_FLAGS_UNK21                  = 0x00200000,
    PLAYER_FLAGS_COMMENTATOR2           = 0x00400000,
    PLAYER_ALLOW_ONLY_ABILITY           = 0x00800000,       // used by bladestorm and killing spree, allowed only spells with SPELL_ATTR0_REQ_AMMO, SPELL_EFFECT_ATTACK, checked only for active player
    PLAYER_FLAGS_HAS_BATTLE_PET_TRAINING= 0x01000000,       // allowed to use battle pet combat system
    PLAYER_FLAGS_NO_XP_GAIN             = 0x02000000,
    PLAYER_FLAGS_UNK26                  = 0x04000000,
    PLAYER_FLAGS_AUTO_DECLINE_GUILD     = 0x08000000,       // Automatically declines guild invites
    PLAYER_FLAGS_GUILD_LEVEL_ENABLED    = 0x10000000,       // Lua_GetGuildLevelEnabled() - enables guild leveling related UI
    PLAYER_FLAGS_VOID_UNLOCKED          = 0x20000000,       // void storage
    PLAYER_FLAGS_UNK30                  = 0x40000000,
    PLAYER_FLAGS_UNK31                  = 0x80000000,
};

enum PlayerFlagsEx
{
    PLAYER_FLAGS_EX_REAGENT_BANK_UNLOCKED = 0x00000001,
    PLAYER_FIELD_EX_UNK_MOUNT_RELATED     = 0x00000002, /// This flag prevent the player from using his mounts, displaying the message "Cannot use it while you are a mercenary"
    PLAYER_FIELD_EX_ARTIFACT_FORGE_CHEAT  = 0x00000004
};

enum PlayerLocalFlags
{
    PLAYER_LOCAL_FLAG_CONTROLLING_PET               = 0x00000001,   // Displays "You have an active summon already" when trying to tame new pet
    PLAYER_LOCAL_FLAG_TRACK_STEALTHED               = 0x00000002,
    PLAYER_LOCAL_FLAG_RELEASE_TIMER                 = 0x00000008,   // Display time till auto release spirit
    PLAYER_LOCAL_FLAG_NO_RELEASE_WINDOW             = 0x00000010,   // Display no "release spirit" window at all
    PLAYER_LOCAL_FLAG_NO_PET_BAR                    = 0x00000020,   // CGPetInfo::IsPetBarUsed
    PLAYER_LOCAL_FLAG_OVERRIDE_CAMERA_MIN_HEIGHT    = 0x00000040,
    PLAYER_LOCAL_FLAG_NEWLY_BOOSTED_CHARACTER       = 0x00000080,
    PLAYER_LOCAL_FLAG_USING_PARTY_GARRISON          = 0x00000100,
    PLAYER_LOCAL_FLAG_CAN_USE_OBJECTS_MOUNTED       = 0x00000200,
    PLAYER_LOCAL_FLAG_CAN_VISIT_PARTY_GARRISON      = 0x00000400,
    PLAYER_LOCAL_FLAG_ACCOUNT_SECURED               = 0x00001000,   // Script_IsAccountSecured used to know if the account have the authenticator
};

#define KNOWN_TITLES_SIZE   12
#define MAX_TITLE_INDEX     (KNOWN_TITLES_SIZE * 64)        // 4 uint64 fields

#define ECLIPSE_FULL_CYCLE_DURATION 40

// used in PLAYER_FIELD_BYTES values
enum PlayerFieldHairColorIdOffsets
{
    PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_SKIN_ID         = 0,
    PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_FACE_ID         = 1,
    PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_HAIR_STYLE_ID   = 2,
    PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_HAIR_COLOR_ID   = 3
};

enum PlayerCustomOptionsOffsets
{
    PLAYER_CUSTOM_OPTOTIONS_TATTOOS         = 0,
    PLAYER_CUSTOM_OPTOTIONS_HORN_STYLE      = 1,
    PLAYER_CUSTOM_OPTOTIONS_BLINDFOLDS      = 2,
    PLAYER_CUSTOM_OPTOTIONS_FACIAL_STYLE    = 3,
};

// used in PLAYER_FIELD_NUM_BACKPACK_SLOTS values
enum AuraVisionFlags
{
    AURA_VISION_NONE                 = 0x00,
    AURA_VISION_STEALTH              = 0x20,
    AURA_VISION_INVISIBILITY_GLOW    = 0x40,
};

enum PlayerFieldKillsOffsets
{
    PLAYER_FIELD_KILLS_OFFSET_TODAY_KILLS     = 0,
    PLAYER_FIELD_KILLS_OFFSET_YESTERDAY_KILLS = 1
};

enum PlayerFieldBytesOffsets
{
    PLAYER_FIELD_BYTES_OFFSET_RAF_GRANTABLE_LEVEL       = 0,
    PLAYER_FIELD_BYTES_OFFSET_ACTION_BAR_TOGGLES        = 1,
    PLAYER_FIELD_BYTES_OFFSET_LIFETIME_MAX_PVP_RANK     = 2,
    PLAYER_FIELD_BYTES_OFFSET_NUM_RESPECS               = 3,
};

enum PlayerBytes2Offsets
{
    PLAYER_BYTES_2_OFFSET_CUSTOM_DISPLAY_OPTION = 0, // 3 bytes
    PLAYER_BYTES_2_OFFSET_FACIAL_STYLE          = 3,
};

enum PlayerBytes3Offsets
{
    PLAYER_BYTES_3_OFFSET_PARTY_TYPE        = 0,
    PLAYER_BYTES_3_OFFSET_BANK_BAG_SLOTS    = 1,
    PLAYER_BYTES_3_OFFSET_GENDER            = 2,
    PLAYER_BYTES_3_OFFSET_INEBRIATION       = 3,
};

enum PlayerBytes4Offsets
{
    PLAYER_BYTES_4_OFFSET_PVP_TITLE     = 0,
    PLAYER_BYTES_4_OFFSET_ARENA_FACTION = 1
};

enum PlayerAvgItemLevelOffsets
{
    TotalAvgItemLevel       = 0,
    EquippedAvgItemLevel    = 1,
    NonPvPAvgItemLevel      = 2,
    PvPAvgItemLevel         = 3,
    MaxAvgItemLevel         = 4
};

enum PlayerFieldNumBackpackSlotsOffsets
{
   PLAYER_FIELD_NUM_BACKPACK_SLOTS_OFFSET_IGNORE_POWER_REGEN_PREDICTION_MASK  = 0,
   PLAYER_FIELD_NUM_BACKPACK_SLOTS_OFFSET_AURA_VISION                         = 1,
   PLAYER_FIELD_NUM_BACKPACK_SLOTS_OFFSET_NUM_BACKPACK_SLOTS                  = 2
};

enum PlayerFieldOverrideSpellsIdOffsets
{
    PLAYER_FIELD_OVERRIDE_SPELLS_ID_OFFSET_OVERRIDE_SPELLS_ID                  = 0     // uint16!
};

enum MirrorTimerType
{
    FATIGUE_TIMER      = 0,
    BREATH_TIMER       = 1,
    FIRE_TIMER         = 2 // feign death
};
#define MAX_TIMERS      3
#define DISABLED_MIRROR_TIMER   -1

// 2^n values
enum PlayerExtraFlags
{
    // gm abilities
    PLAYER_EXTRA_GM_ON              = 0x0001,
    PLAYER_EXTRA_ACCEPT_WHISPERS    = 0x0004,
    PLAYER_EXTRA_TAXICHEAT          = 0x0008,
    PLAYER_EXTRA_GM_INVISIBLE       = 0x0010,
    PLAYER_EXTRA_GM_CHAT            = 0x0020,               // Show GM badge in chat messages

    // other states
    PLAYER_EXTRA_PVP_DEATH          = 0x0100                // store PvP death status until corpse creating.
};

// 2^n values
enum AtLoginFlags
{
    AT_LOGIN_NONE                   = 0x0000,
    AT_LOGIN_RENAME                 = 0x0001,
    AT_LOGIN_RESET_SPELLS           = 0x0002,
    AT_LOGIN_RESET_TALENTS          = 0x0004,
    AT_LOGIN_CUSTOMIZE              = 0x0008,
    AT_LOGIN_RESET_PET_TALENTS      = 0x0010,
    AT_LOGIN_FIRST                  = 0x0020,
    AT_LOGIN_CHANGE_FACTION         = 0x0040,
    AT_LOGIN_CHANGE_RACE            = 0x0080,
    AT_LOGIN_UNLOCK                 = 0x0100,
    AT_LOGIN_LOCKED_FOR_TRANSFER    = 0x0200,
    AT_LOGIN_RESET_SPECS            = 0x0400,
    AT_LOGIN_DELETE_INVALID_SPELL   = 0x0800,     ///< Used at expension switch
    AT_LOGIN_CONVERT_FACTION        = 0x1000,
    AT_LOGIN_BORIS_ITEMS            = 0x2000,
    AT_LOGIN_ITEM_CHANGE_FACTION    = 0x4000
};

typedef std::map<uint32, QuestStatusData> QuestStatusMap;
typedef std::map<uint32, uint32> QuestObjectiveStatusMap;
typedef std::set<uint32> RewardedQuestSet;

//               quest,  keep
typedef std::map<uint32, bool> QuestStatusSaveMap;

// Size (in bytes) of client completed quests bit map
#define QUESTS_COMPLETED_BITS_SIZE 1750

enum QuestSlotOffsets
{
    QUEST_ID_OFFSET     = 0,
    QUEST_STATE_OFFSET  = 1,
    QUEST_COUNTS_OFFSET = 2,
    QUEST_TIME_OFFSET   = 15,

    MAX_QUEST_OFFSET
};

#define MAX_QUEST_COUNTS 24

enum QuestSlotStateMask
{
    QUEST_STATE_NONE     = 0x0000,
    QUEST_STATE_COMPLETE = 0x0001,
    QUEST_STATE_FAIL     = 0x0002
};

enum SkillUpdateState
{
    SKILL_UNCHANGED     = 0,
    SKILL_CHANGED       = 1,
    SKILL_NEW           = 2,
    SKILL_DELETED       = 3
};

struct SkillStatusData
{
    SkillStatusData(uint8 _pos, SkillUpdateState _uState) : pos(_pos), uState(_uState)
    {
    }
    uint8 pos;
    SkillUpdateState uState;
};

typedef ACE_Based::LockedMap<uint32, SkillStatusData> SkillStatusMap;

class Quest;
class Spell;
class Item;
class WorldSession;
class BattlePet;

#define INVENTORY_SLOT_BAG_0    255
#define INVENTORY_DEFAULT_SIZE  24

enum EquipmentSlots                                         // 19 slots
{
    EQUIPMENT_SLOT_START        = 0,
    EQUIPMENT_SLOT_HEAD         = 0,
    EQUIPMENT_SLOT_NECK         = 1,
    EQUIPMENT_SLOT_SHOULDERS    = 2,
    EQUIPMENT_SLOT_BODY         = 3,
    EQUIPMENT_SLOT_CHEST        = 4,
    EQUIPMENT_SLOT_WAIST        = 5,
    EQUIPMENT_SLOT_LEGS         = 6,
    EQUIPMENT_SLOT_FEET         = 7,
    EQUIPMENT_SLOT_WRISTS       = 8,
    EQUIPMENT_SLOT_HANDS        = 9,
    EQUIPMENT_SLOT_FINGER1      = 10,
    EQUIPMENT_SLOT_FINGER2      = 11,
    EQUIPMENT_SLOT_TRINKET1     = 12,
    EQUIPMENT_SLOT_TRINKET2     = 13,
    EQUIPMENT_SLOT_BACK         = 14,
    EQUIPMENT_SLOT_MAINHAND     = 15,
    EQUIPMENT_SLOT_OFFHAND      = 16,
    EQUIPMENT_SLOT_RANGED       = 17,
    EQUIPMENT_SLOT_TABARD       = 18,
    EQUIPMENT_SLOT_END          = 19
};

enum InventorySlots                                         // 4 slots
{
    INVENTORY_SLOT_BAG_START    = 19,
    INVENTORY_SLOT_BAG_END      = 23
};

enum InventoryPackSlots                                     // 16 slots
{
    INVENTORY_SLOT_ITEM_START   = 23,
    INVENTORY_SLOT_ITEM_END     = 47
};

enum BankItemSlots                                          // 28 slots
{
    BANK_SLOT_ITEM_START        = 47,
    BANK_SLOT_ITEM_END          = 75
};

enum BankBagSlots                                           // 7 slots
{
    BANK_SLOT_BAG_START         = 75,
    BANK_SLOT_BAG_END           = 82
};

enum BuyBackSlots                                           // 98 slots
{
    BUYBACK_SLOT_START          = 82,
    BUYBACK_SLOT_END            = 94
};

enum ReagentBankBagSlots
{
    REAGENT_BANK_SLOT_BAG_START = 94,
    REAGENT_BANK_SLOT_BAG_END = 192
};

enum ChildEquipmentSlots
{
    CHILD_EQUIPMENT_SLOT_START   = 192,
    CHILD_EQUIPMENT_SLOT_END     = 195,
};

enum EquipmentSetUpdateState
{
    EQUIPMENT_SET_UNCHANGED = 0,
    EQUIPMENT_SET_CHANGED   = 1,
    EQUIPMENT_SET_NEW       = 2,
    EQUIPMENT_SET_DELETED   = 3
};

enum PlayerSlots
{
    // first slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_START           = EQUIPMENT_SLOT_START,
    // last+1 slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_END             = CHILD_EQUIPMENT_SLOT_END,
    PLAYER_SLOTS_COUNT          = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

enum eBagSlotFlags
{
    BagSlotFlagNone         = 0x00,
    BagSlotFlagDisable      = 0x01, ///< Disable bag for auto sort
    BagSlotFlagEquipment    = 0x02, ///< Allow only equipments to be in this by with auto sort
    BagSlotFlagConsumable   = 0x04, ///< Allow only consumables to be in this by with auto sort
    BagSlotFlagReagents     = 0x08  ///< Allow only reagents to be in this by with auto sort
};

enum EquipmentSetType : int32
{
    EQUIPMENT,
    TRANSMOG
};

struct EquipmentSet
{
    EquipmentSet() : Guid(0), Type(uint32(EquipmentSetType::EQUIPMENT)), IgnoreMask(0), state(EQUIPMENT_SET_NEW), AssignedSpecIndex(-1)
    {
#ifndef CROSS
        for (uint8 i = 0; i < EQUIPMENT_SLOT_END; ++i)
            Items[i] = 0;
#else /* CROSS */
        memset(Items, 0, sizeof(Items));
        memset(OriginalItems, 0, sizeof(OriginalItems));
#endif /* CROSS */
    }

    uint64 Guid;
    std::string Name;
    std::string IconName;
    uint32 Type;
    uint32 IgnoreMask;
    int32  AssignedSpecIndex;
    uint32 Items[EQUIPMENT_SLOT_END];

#ifdef CROSS
    uint32 OriginalItems[EQUIPMENT_SLOT_END];
#endif /* CROSS */
    EquipmentSetUpdateState state;
};

#define MAX_EQUIPMENT_SET_INDEX 10                          // client limit

typedef std::map<uint32, EquipmentSet> EquipmentSets;

struct ItemPosCount
{
    ItemPosCount(uint16 _pos, uint32 _count) : pos(_pos), count(_count) {}
    bool isContainedIn(std::vector<ItemPosCount> const& vec) const;
    uint16 pos;
    uint32 count;
};
typedef std::vector<ItemPosCount> ItemPosCountVec;

/// 7.0.3 22293
enum TransferAbortReason
{
    TRANSFER_ABORT_NONE                         = 0,
    TRANSFER_ABORT_ERROR                        = 1,
    TRANSFER_ABORT_MAX_PLAYERS                  = 2,    ///< Transfer Aborted: instance is full
    TRANSFER_ABORT_NOT_FOUND                    = 3,    ///< Transfer Aborted: instance not found
    TRANSFER_ABORT_TOO_MANY_INSTANCES           = 4,    ///< You have entered too many instances recently.
    TRANSFER_ABORT_ZONE_IN_COMBAT               = 6,    ///< Unable to zone in while an encounter is in progress.
    TRANSFER_ABORT_INSUF_EXPAN_LVL              = 7,    ///< You must have <TBC, WotLK> expansion installed to access this area.
    TRANSFER_ABORT_UNIQUE_MESSAGE               = 9,    ///< Until you've escaped TLK's grasp, you cannot leave this place!
    TRANSFER_ABORT_DIFFICULTY_NOT_FOUND         = 8,    ///< client writes to console "Unable to resolve requested difficultyID %u to actual difficulty for map %d"
    TRANSFER_ABORT_TOO_MANY_REALM_INSTANCES     = 10,   ///< Additional instances cannot be launched, please try again later.
    TRANSFER_ABORT_NEED_GROUP                   = 11,   ///< Transfer Aborted: you must be in a raid group to enter this instance
    TRANSFER_ABORT_NOT_FOUND_2                  = 12,   ///< Transfer Aborted: instance not found
    TRANSFER_ABORT_NOT_FOUND_3                  = 13,   ///< Transfer Aborted: instance not found
    TRANSFER_ABORT_NOT_FOUND_4                  = 14,   ///< Transfer Aborted: instance not found
    TRANSFER_ABORT_REALM_ONLY                   = 15,   ///< All players in the party must be from the same realm to enter %s.
    TRANSFER_ABORT_MAP_NOT_ALLOWED              = 16,   ///< Map cannot be entered at this time.
    TRANSFER_ABORT_LOCKED_TO_DIFFERENT_INSTANCE = 18,   ///< You are already locked to %s
    TRANSFER_ABORT_ALREADY_COMPLETED_ENCOUNTER  = 19,   ///< You are ineligible to participate in at least one encounter in this instance because you are already locked to an instance in which it has been defeated.
    TRANSFER_ABORT_DIFFICULTY                   = 22,   ///< <Normal, Heroic, Epic> difficulty mode is not available for %s.
    TRANSFER_ABORT_XREALM_ZONE_DOWN             = 24,   ///< Transfer Aborted: cross-realm zone is down
    TRANSFER_ABORT_SOLO_PLAYER_SWITCH_DIFFICULTY = 26,  ///< This instance is already in progress. You may only switch difficulties from inside the instance.
};

/// 7.0.3 22293
enum NewWorldReason
{
    NEW_WORLD_NORMAL    = 16,   ///< Normal map change
    NEW_WORLD_SEAMLESS  = 21,   ///< Teleport to another map without a loading screen, used for outdoor scenarios
};

enum InstanceResetWarningType
{
    RAID_INSTANCE_WARNING_HOURS     = 1,                    // WARNING! %s is scheduled to reset in %d hour(s).
    RAID_INSTANCE_WARNING_MIN       = 2,                    // WARNING! %s is scheduled to reset in %d minute(s)!
    RAID_INSTANCE_WARNING_MIN_SOON  = 3,                    // WARNING! %s is scheduled to reset in %d minute(s). Please exit the zone or you will be returned to your bind location!
    RAID_INSTANCE_WELCOME           = 4,                    // Welcome to %s. This raid instance is scheduled to reset in %s.
    RAID_INSTANCE_EXPIRED           = 5
};

// PLAYER_FIELD_ARENA_TEAM_INFO_1_1 offsets
enum ArenaTeamInfoType
{
    ARENA_TEAM_ID                = 0,
    ARENA_TEAM_TYPE              = 1,                       // new in 3.2 - team type?
    ARENA_TEAM_MEMBER            = 2,                       // 0 - captain, 1 - member
    ARENA_TEAM_GAMES_WEEK        = 3,
    ARENA_TEAM_GAMES_SEASON      = 4,
    ARENA_TEAM_WINS_SEASON       = 5,
    ARENA_TEAM_PERSONAL_RATING   = 6,
    ARENA_TEAM_END               = 7
};

class InstanceSave;
using InstanceSavePtr = std::shared_ptr<InstanceSave>;

enum PlayerRestInfoOffsets
{
    REST_STATE_XP       = 0,
    REST_RESTED_XP      = 1,
    REST_STATE_HONOR    = 2,
    REST_RESTED_HONOR   = 3,

    MAX_REST_INFO
};

enum RestFlag
{
    REST_FLAG_IN_TAVERN         = 0x1,
    REST_FLAG_IN_CITY           = 0x2,
    REST_FLAG_IN_FACTION_AREA   = 0x4, // used with AREA_FLAG_REST_ZONE_*
};

enum PlayerRestState : uint8
{
    REST_STATE_RESTED                                = 0x01,
    REST_STATE_NOT_RAF_LINKED                        = 0x02,
    REST_STATE_RAF_LINKED                            = 0x06
};

enum DuelCompleteType
{
    DUEL_INTERRUPTED = 0,
    DUEL_WON         = 1,
    DUEL_FLED        = 2
};

/// Type of environmental damages
enum EnviromentalDamage
{
    DAMAGE_EXHAUSTED = 0,
    DAMAGE_DROWNING  = 1,
    DAMAGE_FALL      = 2,
    DAMAGE_LAVA      = 3,
    DAMAGE_SLIME     = 4,
    DAMAGE_FIRE      = 5,
    DAMAGE_FALL_TO_VOID = 6                                 // custom case for fall without durability loss
};

enum PlayerChatTag
{
    CHAT_TAG_NONE       = 0x00,
    CHAT_TAG_AFK        = 0x01,
    CHAT_TAG_DND        = 0x02,
    CHAT_TAG_GM         = 0x04,
    CHAT_TAG_UNK1       = 0x08,
    CHAT_TAG_DEV        = 0x10,
    CHAT_TAG_UNK        = 0x20,
    CHAT_TAG_PHONE      = 0x40,     /// Speaker Icon
    CHAT_TAG_COM        = 0x80,     /// Commentator
};

enum PlayedTimeIndex
{
    PLAYED_TIME_TOTAL = 0,
    PLAYED_TIME_LEVEL = 1
};

#define MAX_PLAYED_TIME_INDEX 2

// used at player loading query list preparing, and later result selection
enum PlayerLoginQueryIndex
{
    PLAYER_LOGIN_QUERY_LOADFROM = 0,
    PLAYER_LOGIN_QUERY_LOADGROUP,
    PLAYER_LOGIN_QUERY_LOADBOUNDINSTANCES,
    PLAYER_LOGIN_QUERY_LOADAURAS,
    PLAYER_LOGIN_QUERY_LOADAURAS_EFFECTS,
    PLAYER_LOGIN_QUERY_LOGIN_LOADSPELLS,
    PLAYER_LOGIN_QUERY_CHAR_LOADSPELLS,
    PLAYER_LOGIN_QUERY_LOADQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADDAILYQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADREPUTATION,
    PLAYER_LOGIN_QUERY_LOADINVENTORY,
    PLAYER_LOGIN_QUERY_LOAD_ITEM_WITHOUT_INVENTORY,
    PLAYER_LOGIN_QUERY_LOADACTIONS,
    PLAYER_LOGIN_QUERY_LOADMAILCOUNT,
    PLAYER_LOGIN_QUERY_LOADMAILDATE,
    PLAYER_LOGIN_QUERY_LOADSOCIALLIST,
    PLAYER_LOGIN_QUERY_LOADHOMEBIND,
    PLAYER_LOGIN_QUERY_LOADSPELLCOOLDOWNS,
    PLAYER_LOGIN_QUERY_LOADDECLINEDNAMES,
    PLAYER_LOGIN_QUERY_LOADGUILD,
    PLAYER_LOGIN_QUERY_LOAD_ACHIEVEMENTS,
    PLAYER_LOGIN_QUERY_LOAD_CRITERIA_PROGRESS,
    PLAYER_LOGIN_QUERY_LOADEQUIPMENTSETS,
    PLAYER_LOGIN_QUERY_LOADBRACKETDATA,
    PLAYER_LOGIN_QUERY_LOADBGDATA,
    PLAYER_LOGIN_QUERY_LOADTALENTS,
    PLAYER_LOGIN_QUERY_LOADACCOUNTDATA,
    PLAYER_LOGIN_QUERY_LOADSKILLS,
    PLAYER_LOGIN_QUERY_LOADWEEKLYQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADPVPINFO,
    PLAYER_LOGIN_QUERY_LOADBANNED,
    PLAYER_LOGIN_QUERY_LOADQUESTSTATUSREW,
    PLAYER_LOGIN_QUERY_LOADINSTANCELOCKTIMES,
    PLAYER_LOGIN_QUERY_LOADSEASONALQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOAD_MONTHLY_QUEST_STATUS,
    PLAYER_LOGIN_QUERY_LOADVOIDSTORAGE,
    PLAYER_LOGIN_QUERY_LOADCURRENCY,
    PLAYER_LOGIN_QUERY_LOADCURRENCYCAPS,
    PLAYER_LOGIN_QUERY_LOAD_CUF_PROFILES,
    PLAYER_LOGIN_QUERY_LOAD_ARCHAEOLOGY,
    PLAYER_LOGIN_QUERY_LOAD_ARCHAEOLOGY_PROJECTS,
    PLAYER_LOGIN_QUERY_LOAD_ARCHAEOLOGY_SITES,
    PLAYER_LOGIN_QUERY_LOAD_ACCOUNT_TOYS,
    PLAYER_LOGIN_QUERY_LOAD_QUEST_OBJECTIVE_STATUS,
    PLAYER_LOGIN_QUERY_LOAD_CHARGES_COOLDOWNS,
    PLAYER_LOGIN_QUERY_GARRISON,
    PLAYER_LOGIN_QUERY_GARRISON_FOLLOWERS,
    PLAYER_LOGIN_QUERY_GARRISON_MISSIONS,
    PLAYER_LOGIN_QUERY_GARRISON_WORKORDERS,
    PLAYER_LOGIN_QUERY_GARRISON_TALENTS,
    PLAYER_LOGIN_QUERY_GARRISON_DRAENOR,
    PLAYER_LOGIN_QUERY_GARRISON_DRAENOR_BUILDINGS,
    PLAYER_LOGIN_QUERY_GARRISON_DRAENOR_MISSIONS_TAVERNDATA,
    PLAYER_LOGIN_QUERY_GARRISON_DRAENOR_WEEKLY_TAVERNDATA,
    PLAYER_LOGIN_QUERY_GARRISON_BROKEN_ISLES,
    PLAYER_LOGIN_QUERY_DAILY_LOOT_COOLDOWNS,
    PLAYER_LOGIN_QUERY_LOADMAIL,
    PLAYER_LOGIN_QUERY_LOADMAIL_ITEMS,
    PLAYER_LOGIN_QUERY_BOUTIQUE_ITEM,
    PLAYER_LOGIN_QUERY_BOUTIQUE_GOLD,
    PLAYER_LOGIN_QUERY_BOUTIQUE_TITLE,
    PLAYER_LOGIN_QUERY_BOUTIQUE_LEVEL,
    PLAYER_LOGIN_QUERY_LOAD_PERSONAL_INSTANCES_LOCKOUT,
    PLAYER_LOGIN_QUERY_LOAD_LEGENDARIES,
    PLAYER_LOGIN_QUERY_LOADARENADATA,
    PLAYER_LOGIN_QUERY_LOADRANDOMBG,
    PLAYER_LOGIN_QUERY_BOSS_LOOTED,
    PLAYER_LOGIN_QUERY_WORLD_STATES,
    PLAYER_LOGIN_QUERY_STORE_PROFESSION,
    PLAYER_LOGIN_QUERY_LOAD_ARTIFACTS,
    PLAYER_LOGIN_QUERY_LOAD_ARTIFACT_POWERS,
    PLAYER_LOGIN_QUERY_LOAD_PVP_TALENTS,
    PLAYER_LOGIN_QUERY_LOAD_GLYPHS,
    PLAYER_LOGIN_QUERY_LOAD_WORLD_QUEST_REWARDS,
    PLAYER_LOGIN_QUERY_LOAD_NOMI_RECIPES,
    PLAYER_LOGIN_QUERY_LOADREPUTATIONMODS,
    PLAYER_LOGIN_QUERY_LOAD_PLAYERCHOICE_RESPONSE_REWARDED,
    PLAYER_LOGIN_QUERY_LOAD_PVP_SEASON_REWARD_RANKING,
    PLAYER_LOGIN_QUERY_LOAD_CHEST_TIMED_LOCKOUT,
    PLAYER_LOGIN_QUERY_LOAD_CORPSE_LOCATION,
    PLAYER_LOGIN_QUERY_LOAD_GAMEOBJECT_LOCKOUT,
    PLAYER_LOGIN_QUERY_LOAD_OBJECT_LOCKOUT,

    MAX_PLAYER_LOGIN_QUERY
};

enum PlayerLoginDBQueryIndex
{
    PLAYER_LOGINGB_SPELL                            = 0,
    PLAYER_LOGINDB_HEIRLOOM_COLLECTION              = 1,
    PLAYER_LOGINDB_TOYS                             = 2,
    PLAYER_LOGINDB_QUERY_LOAD_ACCOUNT_ACHIEVEMENTS  = 3,
    PLAYER_LOGINDB_QUERY_LOAD_CRITERIA_PROGRESS     = 4,
    PLAYER_LOGINDB_QUERY_LOAD_COSMETICS             = 5,
    PLAYER_LOGINDB_QUERY_ACCOUNT_QUEST_REWARDED     = 6,
    MAX_PLAYER_LOGINDB_QUERY
};

enum WebshopType
{
    WEBSHOP_TYPE_ITEM,
    WEBSHOP_TYPE_SKILL,
    WEBSHOP_TYPE_GOLD,
    WEBSHOP_TYPE_LEVEL,
    WEBSHOP_TYPE_PROFESSION,
    WEBSHOP_TYPE_TITLE,
    WEBSHOP_TYPE_CURRENCY
};

class PetQueryHolder : public SQLQueryHolder
{
    private:
        uint32 m_guid;
        uint32 m_RealmId;
        PreparedQueryResult m_petResult;
    public:
        PetQueryHolder(uint32 guid, uint32 realmId, PreparedQueryResult p_QueryResult) : m_guid(guid), m_RealmId(realmId), m_petResult(p_QueryResult) { }
        uint32 GetGuid() const { return m_guid; }
        bool Initialize();

        PreparedQueryResult GetPetResult() { return m_petResult; }
        static PreparedStatement* GenerateFirstLoadStatement(uint32 p_PetEntry, uint32 p_PetNumber, uint32 p_OwnerID, bool p_CurrentPet, PetSlot p_SlotID, uint32 p_RealmID);
};

enum PetLoginQueryIndex
{
    PET_LOGIN_QUERY_LOADAURA                        = 0,
    PET_LOGIN_QUERY_LOADAURAEFFECT                  = 1,
    PET_LOGIN_QUERY_LOADSPELL                       = 2,
    PET_LOGIN_QUERY_LOADSPELLCOOLDOWN               = 3,
    PET_LOGIN_QUERY_DECLINED_NAME                   = 4,
    MAX_PET_LOGIN_QUERY                             = 5
};

enum PlayerDelayedOperations
{
    DELAYED_SAVE_PLAYER         = 0x001,
    DELAYED_RESURRECT_PLAYER    = 0x002,
    DELAYED_SPELL_CAST_DESERTER = 0x004,
    DELAYED_BG_MOUNT_RESTORE    = 0x008,                     ///< Flag to restore mount state after teleport from BG
    DELAYED_BG_TAXI_RESTORE     = 0x010,                     ///< Flag to restore taxi state after teleport from BG
    DELAYED_BG_GROUP_RESTORE    = 0x020,                     ///< Flag to restore group state after teleport from BG
    DELAYED_PET_BATTLE_INITIAL  = 0x040,
    DELAYED_LEAVE_GROUP         = 0x080,
    DELAYED_REPUTATION_UPDATE   = 0x100,
    DELAYED_END
};

// Player summoning auto-decline time (in secs)
#define MAX_PLAYER_SUMMON_DELAY                   (2*MINUTE)
uint64 const MAX_MONEY_AMOUNT = 99999999999ULL;

enum BindExtensionState
{
    EXTEND_STATE_EXPIRED    = 0,
    EXTEND_STATE_NORMAL     = 1,
    EXTEND_STATE_EXTENDED   = 2,
    EXTEND_STATE_KEEP       = 255   ///< Special state: keep current save type
};

struct InstancePlayerBind
{
    InstanceSavePtr save;
    /* permanent PlayerInstanceBinds are created in Raid/Heroic instances for players
       that aren't already permanently bound when they are inside when a boss is killed
       or when they enter an instance that the group leader is permanently bound to. */
    bool perm;
    /* extend state listing:
    EXPIRED  - doesn't affect anything unless manually re-extended by player
    NORMAL   - standard state
    EXTENDED - won't be promoted to EXPIRED at next reset period, will instead be promoted to NORMAL */
    BindExtensionState extendState;

    InstancePlayerBind() : save(nullptr), perm(false), extendState(BindExtensionState::EXTEND_STATE_NORMAL) { }
};

using InstancePlayerBindPtr = std::shared_ptr< InstancePlayerBind>;

enum DungeonStatusFlag
{
    DUNGEON_STATUSFLAG_NORMAL = 0x01,
    DUNGEON_STATUSFLAG_HEROIC = 0x02,

    RAID_STATUSFLAG_10MAN_NORMAL = 0x01,
    RAID_STATUSFLAG_25MAN_NORMAL = 0x02,
    RAID_STATUSFLAG_10MAN_HEROIC = 0x04,
    RAID_STATUSFLAG_25MAN_HEROIC = 0x08
};

struct AccessRequirement
{
    uint8  levelMin;
    uint8  levelMax;
    uint32 item;
    uint32 item2;
    uint32 quest_A;
    uint32 quest_H;
    uint32 achievement;
    uint32 leader_achievement;
    uint32 itemlevelMin;
    uint32 itemlevelMax;
    std::string questFailedText;
};

struct LFRAccessRequirement
{
    uint8       LevelMin;
    uint8       LevelMax;
    uint32      Item;
    uint32      Item2;
    uint32      QuestA;
    uint32      QuestH;
    uint32      Achievement;
    uint32      LeaderAchievement;
    uint32      ItemLevelMin;
    uint32      ItemLevelMax;
    std::string QuestFailedText;
};

enum CharDeleteMethod
{
    CHAR_DELETE_REMOVE = 0,                      // Completely remove from the database
    CHAR_DELETE_UNLINK = 1                       // The character gets unlinked from the account,
                                                 // the name gets freed up and appears as deleted ingame
};

enum ReferAFriendError
{
    ERR_REFER_A_FRIEND_NONE                          = 0x00,
    ERR_REFER_A_FRIEND_NOT_REFERRED_BY               = 0x01,
    ERR_REFER_A_FRIEND_TARGET_TOO_HIGH               = 0x02,
    ERR_REFER_A_FRIEND_INSUFFICIENT_GRANTABLE_LEVELS = 0x03,
    ERR_REFER_A_FRIEND_TOO_FAR                       = 0x04,
    ERR_REFER_A_FRIEND_DIFFERENT_FACTION             = 0x05,
    ERR_REFER_A_FRIEND_NOT_NOW                       = 0x06,
    ERR_REFER_A_FRIEND_GRANT_LEVEL_MAX_I             = 0x07,
    ERR_REFER_A_FRIEND_NO_TARGET                     = 0x08,
    ERR_REFER_A_FRIEND_NOT_IN_GROUP                  = 0x09,
    ERR_REFER_A_FRIEND_SUMMON_LEVEL_MAX_I            = 0x0A,
    ERR_REFER_A_FRIEND_SUMMON_COOLDOWN               = 0x0B,
    ERR_REFER_A_FRIEND_INSUF_EXPAN_LVL               = 0x0C,
    ERR_REFER_A_FRIEND_SUMMON_OFFLINE_S              = 0x0D
};

enum PlayerCommandStates
{
    CHEAT_NONE          = 0x00,
    CHEAT_GOD           = 0x01,
    CHEAT_CASTTIME      = 0x02,
    CHEAT_COOLDOWN      = 0x04,
    CHEAT_POWER         = 0x08,
    CHEAT_WATERWALK     = 0x10,
    CHEAT_ALL_SPELLS    = 0x20,
    CHEAT_NO_DR         = 0x40
};

enum AttackSwingError
{
    ATTACKSWINGERR_CANT_ATTACK  = 0,
    ATTACKSWINGERR_BAD_FACING   = 1,
    ATTACKSWINGERR_NOT_IN_RANGE = 2,
    ATTACKSWINGERR_DEAD_TARGET  = 3,
};

struct auraEffectData
{
    auraEffectData(uint8 slot, uint8 effect, uint32 amount, uint32 baseamount)
        : _slot(slot), _effect(effect), _amount(amount), _baseamount(baseamount)
    {
    }

    uint8 _slot;
    uint8 _effect;
    uint32 _amount;
    uint32 _baseamount;
};

class Player;

/// Holder for Battleground data
struct BGData
{
    BGData() :
        bgInstanceID(0),
        bgTypeID(BATTLEGROUND_TYPE_NONE),
        bgAfkReportedCount(0),
        bgAfkReportedTimer(0),
        bgTeam(0),
        mountSpell(0),
        IsRandom(false),
#ifndef CROSS
        m_LastActiveSpec(0)
#else /* CROSS */
        m_LastActiveSpec(0),
        m_ReconnectBgTeam(0)
#endif /* CROSS */
    {
        bgQueuesJoinedTime.clear();
        ClearTaxiPath();
    }

    uint32 bgInstanceID;                    ///< This variable is set to bg->m_InstanceID,
                                            ///  when player is teleported to BG - (it is battleground's GUID)
    BattlegroundTypeId bgTypeID;
    bool IsRandom;

    std::map<MS::Battlegrounds::BattlegroundType::Type, uint32> bgQueuesJoinedTime;

    std::set<uint32>   bgAfkReporter;
    uint8              bgAfkReportedCount;
    time_t             bgAfkReportedTimer;

    uint32 bgTeam;                          ///< What side the player will be added to
#ifdef CROSS
    uint32 m_ReconnectBgTeam;
#endif /* CROSS */

    uint32 mountSpell;
    uint32 taxiPath[2];

    WorldLocation joinPos;                  ///< From where player entered BG

    uint8 m_LastActiveSpec;

    void ClearTaxiPath()     { taxiPath[0] = taxiPath[1] = 0; }
    bool HasTaxiPath() const { return taxiPath[0] && taxiPath[1]; }
};

struct VoidStorageItem
{
    VoidStorageItem()
    {
        ItemId = 0;
        ItemEntry = 0;
        CreatorGuid = 0;
        ItemRandomPropertyId = 0;
        ItemSuffixFactor = 0;
        CustomFlags = 0;
    }

    VoidStorageItem(uint64 id, uint32 entry, uint32 creator, uint32 randomPropertyId, uint32 suffixFactor, std::vector<uint32> const& bonuses, uint32 p_CustomFlags, bool _change)
    {
        ItemId = id;
        ItemEntry = entry;
        CreatorGuid = creator;
        ItemRandomPropertyId = randomPropertyId;
        ItemSuffixFactor = suffixFactor;
        Bonuses = bonuses;
        CustomFlags = p_CustomFlags;
        change = _change;
        deleted = false;
    }

    uint64 ItemId;
    uint32 ItemEntry;
    uint32 CreatorGuid;
    uint32 ItemRandomPropertyId;
    uint32 ItemSuffixFactor;
    uint32 CustomFlags;
    bool change;
    bool deleted;
    std::vector<uint32> Bonuses;
};

struct ResurrectionData
{
    uint64 GUID;
    WorldLocation Location;
    uint32 Health;
    uint32 Mana;
    uint32 Aura;
    SpellInfo const* ResSpell;
};

enum TalentLearnResult
{
    TALENT_LEARN_OK                                     = 0,
    TALENT_FAILED_UNKNOWN                               = 1,
    TALENT_FAILED_NOT_ENOUGH_TALENTS_IN_PRIMARY_TREE    = 2,
    TALENT_FAILED_NO_PRIMARY_TREE_SELECTED              = 3,
    TALENT_FAILED_CANT_DO_THAT_RIGHT_NOW                = 4,
    TALENT_FAILED_AFFECTING_COMBAT                      = 5,
    TALENT_FAILED_CANT_REMOVE_TALENT                    = 6,
    TALENT_FAILED_CANT_DO_THAT_CHALLENGE_MODE_ACTIVE    = 7,
    TALENT_FAILED_REST_AREA                             = 8
};

static uint32 const DefaultTalentRowLevels[MAX_TALENT_TIERS]    = { 15,  30,  45,  60,  75,  90, 100 };
static uint32 const DKTalentRowLevels[MAX_TALENT_TIERS]         = { 57,  58,  59,  60,  75,  90, 100 };
static uint32 const DHTalentRowLevels[MAX_TALENT_TIERS]         = { 99, 100, 102, 104, 106, 108, 110 };

struct PlayerTalentInfo
{
    PlayerTalentInfo() : ResetTalentsCost(0), ResetTalentsTime(0), PrimarySpecialization(0), ActiveGroup(0)
    {
        for (uint8 l_I = 0; l_I < MAX_SPECIALIZATIONS; ++l_I)
        {
            PvPTalents[l_I].resize(MAX_PVP_TALENT_SPELLS);
            memset(&PvPTalents[l_I][0], 0, MAX_PVP_TALENT_SPELLS * sizeof(uint32));
        }
    }

    PlayerTalentMap Talents[MAX_SPECIALIZATIONS];
    std::vector<uint32> PvPTalents[MAX_SPECIALIZATIONS];
    std::vector<uint32> Glyphs[MAX_SPECIALIZATIONS];
    uint32 ResetTalentsCost;
    time_t ResetTalentsTime;
    uint32 PrimarySpecialization;
    uint8 ActiveGroup;

private:
    PlayerTalentInfo(PlayerTalentInfo const&) = delete;
    PlayerTalentInfo& operator=(PlayerTalentInfo const&) = delete;
};

#pragma pack(push, 1)
struct PlayerDynamicFieldSpellModByLabel
{
    uint32 Mod;
    float Value;
    uint32 Label;
};
#pragma pack(pop)

struct PlayerToy
{
    PlayerToy()
    {
        memset(this, 0, sizeof (PlayerToy));
    }

    PlayerToy(uint32 p_Item, bool p_Favorite)
    {
        m_ItemID = p_Item;
        m_IsFavorite = p_Favorite;
    }

    uint32 m_ItemID;
    bool m_IsFavorite;
};

typedef std::map<uint32, PlayerToy> PlayerToys;

/// uint64 = BossEntry | ((uint64)DisplayID << 32) ... uint32 = DifficultyID
using BossLooted = std::set<std::pair<uint64, uint32>>;

using PersonalInstanceLockout = std::unordered_map<std::pair<uint32, uint32>/*MapID-Difficulty*/, uint32/*EncounterMask*/>;

struct ChargeEntry
{
    ChargeEntry() { }
    ChargeEntry(Clock::time_point p_StartTime, std::chrono::milliseconds p_RechargeTime) : RechargeStart(p_StartTime), RechargeEnd(p_StartTime + p_RechargeTime) { }
    ChargeEntry(Clock::time_point p_StartTime, Clock::time_point p_EndTime) : RechargeStart(p_StartTime), RechargeEnd(p_EndTime) { }

    Clock::time_point RechargeStart;
    Clock::time_point RechargeEnd;
};

typedef std::unordered_map<uint32 /*categoryId*/, std::deque<ChargeEntry>> ChargeStorageType;

enum BattlegroundTimerTypes
{
    PVP_TIMER,
    CHALLENGE_TIMER
};

struct WargameRequest
{
    uint64 OpposingPartyMemberGUID;
    uint64 QueueID;
    bool   TournamentRules;
    time_t CreationDate;
};

struct CharacterWorldState
{
    uint64 Value;
    bool   Changed;
};

namespace MS { namespace Garrison
{
    class Manager;

    namespace GDraenor
    {
        class GarrisonDraenor;
    }   ///< namespace GDraenor
    namespace GBrokenIsles
    {
        class GarrisonBrokenIsles;
    }   ///< namespace GBrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS

enum StoreCallback
{
    ItemDelivery,
    GoldDelivery,
    CurrencyDelivery,
    LevelDelivery,
    ProfessionDelivery,
    TitleDelivery,
    MaxDelivery
};

#ifndef CROSS
namespace InterRealmPlayerState
{
    enum Type
    {
        None,
        InTransfer,
        PlayOnCross
    };
}

typedef std::list<Channel*> JoinedChannelsList;
#endif

namespace InteractionStatus
{
    enum Type
    {
        None,
        Gossip,
        Vendor,
        Bank,
        GuildBank,
        Transmogrification,
        VoidStorage,
        ItemUpgrade,
        Trainer,
        AuctionHouse,
        MailBox
    };
}

struct ArenaCooldown
{
    uint32 SpellID;
    uint32 Unk1 = 0;
    uint32 CooldownType = 2;
    uint32 Unk2 = 0;
    uint32 StartTimeMS;
    uint32 EndPassiveTimeMS;
    uint32 EndTimeMS;
};

struct LastProcInfo
{
    LastProcInfo()
    {
        LastProcAttemptTime = getMSTime() - 10 * TimeConstants::IN_MILLISECONDS;
        LastProcSuccessTime = getMSTime() - 120 * TimeConstants::IN_MILLISECONDS;
    }

    uint32 LastProcAttemptTime;
    uint32 LastProcSuccessTime;
};

class Player : public Unit, public GridObject<Player>
{
    friend class WorldSession;
    friend void Item::AddToUpdateQueueOf(Player* player);
    friend void Item::RemoveFromUpdateQueueOf(Player* player);
    public:
        explicit Player(WorldSession* session);
        Player(Player const&) : Unit(true), m_reputationMgr(this), phaseMgr(this), m_archaeologyMgr(this), m_VignetteMgr(this) {} ///< For boost::any::placeholder purpose
        ~Player();

        uint32 m_IsInDestructor;

        void Clear();

        //AnticheatData anticheatData;

        void CleanupsBeforeDelete(bool finalCleanup = true) override;

        void AddToWorld() override;
        void RemoveFromWorld() override;

        bool TeleportTo(uint32 mapid, float x, float y, float z, float orientation, uint32 options = 0, bool p_AddToTask = true);
        bool TeleportTo(WorldLocation const &loc, uint32 options = 0)
        {
            return TeleportTo(loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), loc.GetOrientation(), options);
        }
        bool TeleportTo(uint32 p_MapID, Position const p_Pos, uint32 p_Options = 0)
        {
            return TeleportTo(p_MapID, p_Pos.m_positionX, p_Pos.m_positionY, p_Pos.m_positionZ, p_Pos.m_orientation, p_Options);
        }
        bool TeleportTo(uint32 p_MapID, Position const* p_Pos, uint32 p_Options = 0)
        {
            return TeleportTo(p_MapID, p_Pos->m_positionX, p_Pos->m_positionY, p_Pos->m_positionZ, p_Pos->m_orientation, p_Options);
        }
        bool TeleportTo(uint32 p_LocEntry, uint32 p_Options = 0)
        {
            WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(p_LocEntry);
            if (l_Loc == nullptr)
                return false;

            return TeleportTo(l_Loc->MapID, l_Loc->x, l_Loc->y, l_Loc->z, l_Loc->o, p_Options);
        }

        bool TeleportToBGEntryPoint(bool inter_realm = false);
        void SwitchToPhasedMap(uint32 p_MapID, CustomInstanceZone const* p_CustomInstanceZone = nullptr, std::function<void(Player*)> p_Callback = [](Player*)->void {});

        void SetSummonPoint(uint32 mapid, float x, float y, float z)
        {
            m_summon_expire = time(NULL) + MAX_PLAYER_SUMMON_DELAY;
            m_summon_mapid = mapid;
            m_summon_x = x;
            m_summon_y = y;
            m_summon_z = z;
        }
        void SummonIfPossible(bool agree);

        bool Create(uint32 guidlow, CharacterCreateInfo* createInfo);

        void Update(uint32 time) override;

        void AddDelayedUpdateTime(uint32 p_Time) { m_DelayedUpdateTime += p_Time; }

        static bool BuildEnumData(PreparedQueryResult p_Result, ByteBuffer * p_Data);
        static void GenerateFactionConversionQuery(SQLTransaction& p_Transaction, uint32 p_Guid, uint32 p_Team, char const* p_KnowTitlesStr = nullptr, bool p_ItemsOnly = false);

        void SetInWater(bool apply);

        void SendArenaCooldown(uint32 p_SpellID, uint32 p_Cooldown, bool p_IsPassive = false);

        bool IsInWater() const override { return m_isInWater; }
        bool IsUnderWater() const override;
        bool IsInAreaTriggerRadius(const AreaTriggerEntry* trigger) const;

        void SendInitialPacketsBeforeAddToMap();
        void SendInitialPacketsAfterAddToMap();
        void SendTransferAborted(uint32 mapid, TransferAbortReason reason, uint8 arg = 0);
        void SendRaidInstanceMessage(uint32 mapid, Difficulty difficulty, uint32 time, bool p_Welcome);
        void SendInstanceGroupSizeChanged(uint32 p_Size);

        bool CanInteractWithQuestGiver(Object* questGiver);
        Creature* GetNPCIfCanInteractWith(uint64 guid, uint32 npcflagmask);
        Creature* GetNPCIfCanInteractWithFlag2(uint64 guid, uint32 npcflagmask);
        GameObject* GetGameObjectIfCanInteractWith(uint64 guid, GameobjectTypes type) const;

        void CombatStopWithInstances(bool p_IncludeCast = false);

        bool ToggleAFK();
        bool ToggleDND();
        bool isAFK() const { return HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_AFK); }
        bool isDND() const { return HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_DND); }
        uint8 GetChatTag() const;
        std::string afkMsg;
        std::string dndMsg;

        void SendGlyphs();
        void SendGlyph(uint32 p_SpellID, uint16 p_GlyphID);

        MS::Garrison::Manager*                              GetGarrison() const;
        MS::Garrison::GDraenor::GarrisonDraenor*            GetDraenorGarrison() const;
        MS::Garrison::GBrokenIsles::GarrisonBrokenIsles*    GetBrokenIslesGarrison() const;
        void CreateGarrison(MS::Garrison::GarrisonType::Type p_Type);
        bool IsInDraenorGarrison() const;
        bool IsInBrokenIslesGarrison() const;
        bool IsInShipyard() const;
        bool IsInAnyGarrison() const;
        int32 GetGarrisonMapID() const;
        int32 GetShipyardMapID() const;
        void DeleteGarrison(MS::Garrison::GarrisonType::Type p_Type);
        uint32 GetPlotInstanceID() const;

        uint32 GetBarberShopCost(uint8 newhairstyle, uint8 newhaircolor, uint8 newfacialhair, BarberShopStyleEntry const* newSkin, BarberShopStyleEntry const* p_NewFace, std::array<BarberShopStyleEntry const*, PLAYER_CUSTOM_DISPLAY_SIZE> const& p_NewCustomDisplay);

        PlayerSocial *GetSocial() { return m_social; }

        PlayerTaxi m_taxi;
        void InitTaxiNodesForLevel() { m_taxi.InitTaxiNodesForLevel(getRace(), getClass(), getLevel()); }
        bool ActivateTaxiPathTo(std::vector<uint32> const& nodes, Creature* npc = nullptr, uint32 spellid = 0, uint32 p_PreferredMountDisplay =0);
        bool ActivateTaxiPathTo(uint32 taxi_path_id, uint32 spellid = 0);
        void CleanupAfterTaxiFlight();
        void ContinueTaxiFlight() const;
                                                            // mount_id can be used in scripting calls
        bool IsAcceptWhispers() const { return m_ExtraFlags & PLAYER_EXTRA_ACCEPT_WHISPERS; }
        void SetAcceptWhispers(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_ACCEPT_WHISPERS; else m_ExtraFlags &= ~PLAYER_EXTRA_ACCEPT_WHISPERS; }
        bool isGameMaster() const { return m_ExtraFlags & PLAYER_EXTRA_GM_ON; }
        void SetGameMaster(bool on);
        bool isGMChat() const { return m_ExtraFlags & PLAYER_EXTRA_GM_CHAT; }
        void SetGMChat(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_GM_CHAT; else m_ExtraFlags &= ~PLAYER_EXTRA_GM_CHAT; }
        bool isTaxiCheater() const { return m_ExtraFlags & PLAYER_EXTRA_TAXICHEAT; }
        void SetTaxiCheater(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_TAXICHEAT; else m_ExtraFlags &= ~PLAYER_EXTRA_TAXICHEAT; }
        bool isGMVisible() const { return !(m_ExtraFlags & PLAYER_EXTRA_GM_INVISIBLE); }
        void SetGMVisible(bool on);
        void SetPvPDeath(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_PVP_DEATH; else m_ExtraFlags &= ~PLAYER_EXTRA_PVP_DEATH; }

        void GiveXP(uint32 xp, Unit* victim, float group_rate=1.0f);
        void GiveGatheringXP();
        void GiveLevel(uint8 level, bool p_Boosted = false);

        void InitStatsForLevel(bool reapplyMods = false);

        void SetPhaseMask(uint32 p_NewPhaseMask, bool p_Update) override;
        void SetZonePhaseMask(uint32 p_NewPhaseMask, bool p_Update);
        uint32 GetZonePhaseMask(uint32 p_Zone = 0) const;

        // .cheat command related
        bool GetCommandStatus(uint32 command) const { return _activeCheats & command; }
        void SetCommandStatusOn(uint32 command) { _activeCheats |= command; }
        void SetCommandStatusOff(uint32 command) { _activeCheats &= ~command; }

        // Played Time Stuff
        time_t m_logintime;
        time_t m_Last_tick;
        uint32 m_Played_time[MAX_PLAYED_TIME_INDEX];
        uint32 GetTotalPlayedTime() { return m_Played_time[PLAYED_TIME_TOTAL]; }
        uint32 GetLevelPlayedTime() { return m_Played_time[PLAYED_TIME_LEVEL]; }

        void setDeathState(DeathState s) override;                   // overwrite Unit::setDeathState

        float GetRestBonus() const { return m_rest_bonus; }
        void ModRestBonus(float mod);
        void SetRestBonus(float rest_bonus_new);

        bool HasRestFlag(RestFlag restFlag) const { return (_restFlagMask & restFlag) != 0; }
        void SetRestFlag(RestFlag restFlag, uint32 triggerId = 0);
        void RemoveRestFlag(RestFlag restFlag);

        uint32 GetXPRestBonus(uint32 xp);
        uint32 GetInnTriggerId() const { return inn_triggerId; }

        Pet* GetPet() const;
        void SummonPet(uint32 entry, float x, float y, float z, float ang, PetType petType, uint32 despwtime, PetSlot slotID = PET_SLOT_UNK_SLOT, bool stampeded = false, std::function<void(Pet*, bool)> p_Callback = [](Pet*, bool){}, bool p_Bypass = false);
        void RemovePet(Pet* pet, PetSlot mode, bool returnreagent = false, bool stampeded = false);

        PhaseMgr& GetPhaseMgr() { return phaseMgr; }
        PhaseMgr const& GetPhaseMgr() const { return phaseMgr; }


        void Say(const std::string& text, const uint32 language);
        void Yell(const std::string& text, const uint32 language);
        void TextEmote(const std::string& text);
        void Whisper(const std::string& text, const uint32 language, uint64 receiver);
        void WhisperAddon(const std::string& text, const std::string& prefix, Player* receiver);
        void BuildPlayerChat(WorldPacket* p_Data, uint64 p_Target, uint8 p_MsgType, std::string const& p_Text, uint32 p_LangID, char const* p_AddonPrefix = nullptr, std::string const& p_Channel = "") const;

        MS::Skill::Archaeology::Manager& GetArchaeologyMgr() { return m_archaeologyMgr; }

        /*********************************************************/
        /***                    STORAGE SYSTEM                 ***/
        /*********************************************************/

        void SetVirtualItemSlot(uint8 i, Item* item);
        void SetSheath(SheathState sheathed) override;             // overwrite Unit version
        uint8 FindEquipSlot(ItemTemplate const* proto, uint32 slot, bool swap) const;
        uint8 GetGuessedEquipSlot(ItemTemplate const* proto) const;
        uint32 GetItemCount(uint32 item, bool inBankAlso = false, Item* skipItem = NULL, bool isVoidStorageAlso = false) const;
        uint32 GetItemCountWithLimitCategory(uint32 limitCategory, Item* skipItem = NULL) const;
        Item* GetItemByGuid(uint64 guid) const;
        Item* GetItemByEntry(uint32 entry) const;
        Item* GetItemByPos(uint16 pos) const;
        Item* GetItemByPos(uint8 bag, uint8 slot) const;
        Bag*  GetBagByPos(uint8 slot) const;
        Item* GetItemBySlot(uint8 p_Slot) const { return m_items[p_Slot]; }
        inline Item* GetUseableItemByPos(uint8 bag, uint8 slot) const //Does additional check for disarmed weapons
        {
            if (!CanUseAttackType(GetAttackBySlot(slot)))
                return NULL;
            return GetItemByPos(bag, slot);
        }
        Item* GetWeaponForAttack(WeaponAttackType attackType, bool useable = false) const;
        Item* GetShield(bool useable = false) const;
        Item* GetChildItemByGuid(uint64 p_Guid) const;
        static uint8 GetAttackBySlot(uint8 slot);        // MAX_ATTACK if not weapon slot
        std::vector<Item*> &GetItemUpdateQueue() { return m_itemUpdateQueue; }
        static bool IsInventoryPos(uint16 pos) { return IsInventoryPos(pos >> 8, pos & 255); }
        static bool IsInventoryPos(uint8 bag, uint8 slot);
        static bool IsEquipmentPos(uint16 pos) { return IsEquipmentPos(pos >> 8, pos & 255); }
        static bool IsEquipmentPos(uint8 bag, uint8 slot);
        static bool IsBagPos(uint16 pos);
        static bool IsBankPos(uint16 pos) { return IsBankPos(pos >> 8, pos & 255); }
        static bool IsBankPos(uint8 bag, uint8 slot);
        static bool IsReagentBankPos(uint16 pos) { return IsReagentBankPos(pos >> 8, pos & 255); }
        static bool IsReagentBankPos(uint8 bag, uint8 slot);
        static bool IsChildEquipmentPos(uint16 pos) { return IsChildEquipmentPos(pos >> 8, pos & 255); }
        static bool IsChildEquipmentPos(uint8 bag, uint8 slot);
        bool IsValidPos(uint16 pos, bool explicit_pos) { return IsValidPos(pos >> 8, pos & 255, explicit_pos); }
        bool IsValidPos(uint8 bag, uint8 slot, bool explicit_pos);
        uint8 GetInventorySlotCount() const { return GetByteValue(PLAYER_FIELD_NUM_BACKPACK_SLOTS, PLAYER_FIELD_NUM_BACKPACK_SLOTS_OFFSET_NUM_BACKPACK_SLOTS); }
        void SetInventorySlotCount(uint8 slots);
        uint8 GetBankBagSlotCount() const { return GetByteValue(PLAYER_FIELD_INEBRIATION, PLAYER_BYTES_3_OFFSET_BANK_BAG_SLOTS); }
        void SetBankBagSlotCount(uint8 count) { SetByteValue(PLAYER_FIELD_INEBRIATION, PLAYER_BYTES_3_OFFSET_BANK_BAG_SLOTS, count); }
        bool HasItemCount(uint32 p_ItemID, uint32 p_Count = 1, bool p_Bank = false, bool p_ReagentBank = false, bool p_VoidStorage = false) const;
        bool HasItemFitToSpellRequirements(SpellInfo const* spellInfo, Item const* ignoreItem = NULL) const;
        bool CanNoReagentCast(SpellInfo const* spellInfo) const;
        uint32 GetLegendaryCounts() const;
        bool HasItemOrGemWithIdEquipped(uint32 item, uint32 count, uint8 except_slot = NULL_SLOT) const;
        bool HasItemOrGemWithLimitCategoryEquipped(uint32 limitCategory, uint32 count, uint8 except_slot = NULL_SLOT) const;
        bool IsItemSupplies(ItemTemplate const* p_BagProto) const;
        InventoryResult CanTakeMoreSimilarItems(Item* pItem) const { return CanTakeMoreSimilarItems(pItem->GetEntry(), pItem->GetCount(), pItem); }
        InventoryResult CanTakeMoreSimilarItems(uint32 entry, uint32 count) const { return CanTakeMoreSimilarItems(entry, count, NULL); }
        InventoryResult CanStoreNewItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 item, uint32 count, uint32* no_space_count = NULL) const
        {
            return CanStoreItem(bag, slot, dest, item, count, NULL, false, no_space_count);
        }
        InventoryResult CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap = false) const
        {
            if (!pItem)
                return EQUIP_ERR_ITEM_NOT_FOUND;
            uint32 count = pItem->GetCount();
            return CanStoreItem(bag, slot, dest, pItem->GetEntry(), count, pItem, swap, NULL);

        }
        InventoryResult CanStoreItems(Item** pItem, int count) const;
        InventoryResult CanEquipNewItem(uint8 slot, uint16& dest, uint32 item, bool swap) const;
        InventoryResult CanEquipItem(uint8 slot, uint16& dest, Item* pItem, bool swap, bool not_loading = true) const;

        /// This method must be called before equipping parent item!
        InventoryResult CanEquipChildItem(Item* parentItem) const;

        InventoryResult CanEquipUniqueItem(Item* pItem, uint8 except_slot = NULL_SLOT, uint32 limit_count = 1) const;
        InventoryResult CanEquipUniqueItem(ItemTemplate const* itemProto, uint8 except_slot = NULL_SLOT, uint32 limit_count = 1) const;
        InventoryResult CanUnequipItems(uint32 item, uint32 count) const;
        InventoryResult CanUnequipItem(uint16 src, bool swap) const;
        InventoryResult CanBankItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap, bool not_loading = true) const;
        InventoryResult CanReagentBankItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap) const;
        InventoryResult CanUseItem(Item* pItem, bool not_loading = true) const;
        InventoryResult CanUseItem(ItemTemplate const* pItem, uint32 p_ScaledLevel = 0) const;
        bool CanLootItem(ItemTemplate const* proto) const;
        InventoryResult CanRollForItemInLFG(ItemTemplate const* item, WorldObject const* lootedObject) const;
        Item* StoreNewItem(ItemPosCountVec const& pos, uint32 item, bool update, int32 randomPropertyId = 0, std::vector<uint32> const& p_Bonuses = {});
        Item* StoreNewItem(ItemPosCountVec const& pos, uint32 item, bool update, int32 randomPropertyId, AllowedLooterSet &allowedLooters, std::vector<uint32> const& p_Bonuses);
        Item* StoreItem(ItemPosCountVec const& pos, Item* pItem, bool update);
        Item* EquipNewItem(uint16 pos, uint32 item, bool update);
        Item* EquipItem(uint16 pos, Item* pItem, bool update);
        void AutoUnequipOffhandIfNeed(bool force = false);
        void EquipChildItem(uint8 parentBag, uint8 parentSlot, Item* parentItem);
        void AutoUnequipChildItem(Item* parentItem, bool p_Log = false);
        bool StoreNewItemInBestSlots(uint32 item_id, uint32 item_count, ItemContext p_ItemContext = ItemContext::None);
        void AutoStoreLoot(uint8 bag, uint8 slot, uint32 loot_id, LootStore const& store, bool broadcast = false);
        void AutoStoreLoot(uint32 loot_id, LootStore const& store, bool broadcast = false) { AutoStoreLoot(NULL_BAG, NULL_SLOT, loot_id, store, broadcast); }
        void StoreLootItem(uint8 p_LootSlot, Loot* p_Loot, uint8 p_LinkedLootSlot = 255);
        void AddTrackingQuestIfNeeded(uint64 p_SourceGuid);

        /// Apply a function on every item found in the bags.
        /// @p_Function : A function that takes the owner of the items, the item to process, the slot bag of the item and the slot of the item.
        /// If p_Function returns false, then it will stop apply.
        void ApplyOnBagsItems(std::function<bool(Player*, Item*, uint8, uint8)>&& p_Function);

        /// Apply a function on every item found in the bank.
        /// @p_Function : A function that takes the owner of the items, the item to process, the slot bag of the item and the slot of the item.
        /// If p_Function returns false, then it will stop apply.
        void ApplyOnBankItems(std::function<bool(Player*, Item*, uint8, uint8)>&& p_Function);

        /// Apply a function on every item found in the reagent bank.
        /// @p_Function : A function that takes the owner of the items, the item to process, the slot bag of the item and the slot of the item.
        /// If p_Function returns false, then it will stop apply.
        void ApplyOnReagentBankItems(std::function<bool(Player*, Item*, uint8, uint8)>&& p_Function);

        InventoryResult CanTakeMoreSimilarItems(uint32 entry, uint32 count, Item* pItem, uint32* no_space_count = NULL) const;
        InventoryResult CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 entry, uint32 count, Item* pItem = NULL, bool swap = false, uint32* no_space_count = NULL) const;

        void AddRefundReference(uint32 it);
        void DeleteRefundReference(uint32 it);

        /// send full data about all currencies to client
        void ModifyCurrencyFlags(uint32 currencyId, uint8 flags);
        void SendCurrencies();
        void SendPvpRewards();
        /// return count of currency witch has plr
        uint32 GetCurrency(uint32 id, bool usePrecision) const;
        uint32 GetCurrencyOnWeek(uint32 id, bool usePrecision) const;
        uint32 GetCurrencyOnSeason(uint32 id, bool usePrecision) const;
        /// return presence related currency
        bool HasCurrency(uint32 id, uint32 count) const;
        /// @todo: not understand why it subtract from total count and for what it used. It should be remove and replaced by ModifyCurrency
        void SetCurrency(uint32 id, uint32 count, bool printLog = true);
        uint32 GetCurrencyWeekCap(uint32 id, bool usePrecision = false);
        void ResetCurrencyWeekCap();
        uint32 CalculateCurrencyWeekCap(uint32 id);
        uint32 CalculateCurrencyTotalCap(uint32 p_CurrencyId);
        bool AddOrUpdateCurrencyTotalCap(uint32 p_CurrencyId, uint16 p_SourceType, uint32 p_SourceId, int32 p_Value);

        bool HasUnlockedReagentBank();
        void UnlockReagentBank();
        uint32 GetFreeReagentBankSlot() const;

        /**
        * @name ModifyCurrency
        * @brief Change specific currency and send result to client

        * @param id currency entry from CurrencyTypes.db2
        * @param count integer value for adding/removing current currency
        * @param printLog used on SMSG_UPDATE_CURRENCY
        * @param ignore gain multipliers
        */

        int32 ModifyCurrency(uint32 id, int32 count, bool supressLog = true, bool ignoreMultipliers = false, bool ignoreLimit = false, MS::Battlegrounds::RewardCurrencyType::Type p_RewardCurrencyType = MS::Battlegrounds::RewardCurrencyType::Type::None);
        void ModifyCurrencyAndSendToast(uint32 id, int32 count, bool printLog = true, bool ignoreMultipliers = false, bool ignoreLimit = false);

        void HandleItemSetBonuses(bool p_Apply);
        void HandleGemBonuses(bool p_Apply);

        void ApplyEquipCooldown(Item* pItem);
        void QuickEquipItem(uint16 pos, Item* pItem);
        void VisualizeItem(uint8 slot, Item* pItem);
        void SetVisibleItemSlot(uint8 slot, Item* pItem);

        void SetBackPackAutoSortDisabled(bool p_Disabled);
        bool IsBackPackAutoSortDisabled() const { return m_BackPackAutoSortDisabled; }

        Item* BankItem(ItemPosCountVec const& dest, Item* pItem, bool update)
        {
            return StoreItem(dest, pItem, update);
        }
        void RemoveItem(uint8 bag, uint8 slot, bool update);
        void MoveItemFromInventory(uint8 bag, uint8 slot, bool update);
                                                            // in trade, auction, guild bank, mail....
        void MoveItemToInventory(ItemPosCountVec const& dest, Item* pItem, bool update, bool in_characterInventoryDB = false);
                                                            // in trade, guild bank, mail....
        void RemoveItemDependentAurasAndCasts(Item* pItem);
        void RemoveAuraDependentItem(Item* p_Item);
        void DestroyItem(uint8 bag, uint8 slot, bool update);
        void DestroyItemCount(uint32 item, uint32 count, bool update, bool unequip_check = false);
        void DestroyItemCount(Item* item, uint32& count, bool update);
        void DestroyConjuredItems(bool update);
        void DestroyZoneLimitedItem(bool update, uint32 new_zone);
        void SplitItem(uint16 src, uint16 dst, uint32 count);
        void SwapItem(uint16 src, uint16 dst);
        void AddItemToBuyBackSlot(Item* pItem);
        Item* GetItemFromBuyBackSlot(uint32 slot);
        void RemoveItemFromBuyBackSlot(uint32 slot, bool del);
        void SendEquipError(InventoryResult msg, Item* pItem, Item* pItem2 = NULL, uint32 itemid = 0);
        void SendBuyError(BuyResult msg, Creature* creature, uint32 item, uint32 param);
        void SendSellError(SellResult msg, Creature* creature, uint64 guid);
        void AddWeaponProficiency(uint32 newflag) { m_WeaponProficiency |= newflag; }
        void AddArmorProficiency(uint32 newflag) { m_ArmorProficiency |= newflag; }
        uint32 GetWeaponProficiency() const { return m_WeaponProficiency; }
        uint32 GetArmorProficiency() const { return m_ArmorProficiency; }
        bool IsUseEquipedWeapon(bool mainhand) const
        {
            // disarm applied only to mainhand weapon
            return !IsInFeralForm() && (!mainhand || !HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED));
        }
        bool IsTwoHandUsed() const
        {
            Item* mainItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

            if (mainItem && mainItem->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
                return false;

            return mainItem && ((mainItem->GetTemplate()->InventoryType == INVTYPE_2HWEAPON && !CanTitanGrip()) || mainItem->GetTemplate()->InventoryType == INVTYPE_RANGED || mainItem->GetTemplate()->InventoryType == INVTYPE_THROWN || mainItem->GetTemplate()->InventoryType == INVTYPE_RANGEDRIGHT);
        }
        void SendNewItem(Item* item, uint32 count, bool received, bool created, bool broadcast = false, uint32 p_EncounterID = 0, ItemContext p_Context = ItemContext::None, bool p_IsBonusRoll = false);
        bool BuyItemFromVendorSlot(uint64 vendorguid, uint32 vendorslot, uint32 item, uint8 count, uint8 bag, uint8 slot);
        bool BuyCurrencyFromVendorSlot(uint64 vendorGuid, uint32 vendorSlot, uint32 currency, uint32 count);
        bool _StoreOrEquipNewItem(uint32 vendorslot, uint32 item, uint8 count, uint8 bag, uint8 slot, int64 price, ItemTemplate const* pProto, Creature* pVendor, VendorItem const* crItem, bool bStore);

        float GetReputationPriceDiscount(Creature const* creature) const;

        Player* GetTrader() const { return m_trade ? m_trade->GetTrader() : nullptr; }
        TradeData* GetTradeData() const { return m_trade; }
        void TradeCancel(bool sendback);
        uint32 GetClientStateIndex() const { return m_ClientStateIndex; }
        void IncreaseClientStateIndex() { ++m_ClientStateIndex; }
        void InitializeClientStateIndex() { m_ClientStateIndex = 1; }

        void UpdateEnchantTime(uint32 time);
        void UpdateSoulboundTradeItems();
        void AddTradeableItem(Item* item);
        void RemoveTradeableItem(Item* item);
        void UpdateItemDuration(uint32 time, bool realtimeonly = false);
        void AddEnchantmentDurations(Item* item);
        void RemoveEnchantmentDurations(Item* item);
        void RemoveArenaEnchantments(EnchantmentSlot slot);
        void AddEnchantmentDuration(Item* item, EnchantmentSlot slot, uint32 duration);
        void ApplyEnchantment(Item* item, EnchantmentSlot slot, bool apply, bool apply_dur = true, bool ignore_condition = false);
        void ApplyEnchantment(Item* item, bool apply);
        void UpdateSkillEnchantments(uint16 skill_id, uint16 curr_value, uint16 new_value);
        void SendEnchantmentDurations();
        void BuildEnchantmentsInfoData(WorldPacket* data);
        void AddItemDurations(Item* item);
        void RemoveItemDurations(Item* item);
        void SendItemDurations();
        void SendDisplayToast(uint32 p_Entry, uint32 p_Count, uint32 p_Args, DisplayToastMethod p_Method, ToastTypes p_Type, bool p_BonusRoll, bool p_Mailed, std::vector<uint32> const& p_Bonus = {});
        void SendDisplayToast(Item* p_Item, uint32 p_Count, uint32 p_Args, bool p_BonusRoll = true, bool p_Mailed = false);
        void LoadCorpse(PreparedQueryResult result);
        void LoadPet(PreparedQueryResult result);

        Item* AddItem(uint32 p_ItemId, uint32 p_Count, std::list<uint32> p_Bonuses = {}, bool p_FromShop = false, uint32 p_EncounterID = 0, bool p_BonusRoll = false, bool p_ByMailIfNeeded = false, ItemContext p_Context = ItemContext::None);

        uint32 m_stableSlots;
        std::unordered_map<uint32, uint32> m_stablePetModels;

        /*********************************************************/
        /***                    GOSSIP SYSTEM                  ***/
        /*********************************************************/

        void PrepareGossipMenu(WorldObject* source, uint32 menuId = 0, bool showQuests = false);
        void SendPreparedGossip(WorldObject* source);
        void OnGossipSelect(WorldObject* source, uint32 gossipListId, uint32 menuId);

        uint32 GetGossipTextId(uint32 menuId, WorldObject* source);
        uint32 GetGossipTextId(WorldObject* source);
        static uint32 GetDefaultGossipMenuForSource(WorldObject* source);

        /*********************************************************/
        /***                    QUEST SYSTEM                   ***/
        /*********************************************************/

        int32 GetQuestLevel(Quest const* quest) const
        {
            if (!quest)
                return getLevel();
            return quest->GetQuestLevel() > 0 ? quest->GetQuestLevel() : std::min<int32>(getLevel(), quest->GetQuestMaxScalingLevel());
        }

        void PrepareQuestMenu(uint64 guid);
        void SendPreparedQuest(uint64 guid);
        bool IsActiveQuest(uint32 quest_id) const;
        Quest const* GetNextQuest(uint64 guid, Quest const* quest);
        bool CanSeeStartQuest(Quest const* quest);
        bool CanTakeQuest(Quest const* quest, bool msg);
        bool CanAddQuest(Quest const* quest, bool msg);
        bool CanCompleteQuest(uint32 quest_id);
        bool IsQuestObjectiveComplete(Quest const* p_Quest, QuestObjective const* p_Objective);
        bool CanCompleteRepeatableQuest(Quest const* quest);
        bool CanRewardQuest(Quest const* quest, bool msg);
        bool CanRewardQuest(Quest const* quest, uint32 reward, bool msg);
        void HandleAutoCompleteQuest(Quest const* quest);
        void AddQuest(Quest const* quest, Object* questGiver);
        void CompleteQuest(uint32 quest_id);
        void IncompleteQuest(uint32 quest_id);
        void RewardQuest(Quest const* quest, uint32 reward, Object* questGiver, bool announce = true);
        bool CanSelectQuestPackageItem(QuestPackageItemEntry const* p_QuestPackageItem) const;
        void RewardQuestPackage(uint32 p_QuestPackageId, uint32 p_OnlyItemId = 0);
        void FailQuest(uint32 quest_id);
        bool SatisfyQuestSkill(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLevel(Quest const* qInfo, bool msg);
        bool SatisfyQuestLog(bool msg);
        bool SatisfyQuestPreviousQuest(Quest const* qInfo, bool msg);
        bool SatisfyQuestTeam(Quest const* qInfo);
        bool SatisfyQuestClass(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestRace(Quest const* qInfo, bool msg);
        bool SatisfyQuestReputation(Quest const* qInfo, bool msg);
        bool SatisfyQuestStatus(Quest const* qInfo, bool msg);
        bool SatisfyQuestConditions(Quest const* qInfo, bool msg);
        bool SatisfyQuestTimed(Quest const* qInfo, bool msg);
        bool SatisfyQuestExclusiveGroup(Quest const* qInfo, bool msg);
        bool SatisfyQuestNextChain(Quest const* qInfo, bool msg);
        bool SatisfyQuestPrevChain(Quest const* qInfo, bool msg);
        bool SatisfyQuestDay(Quest const* qInfo);
        bool SatisfyQuestWeek(Quest const* qInfo, bool msg);
        bool SatisfyQuestMonth(Quest const* qInfo, bool msg);
        bool SatisfyQuestSeasonal(Quest const* qInfo, bool msg);
        bool GiveQuestSourceItem(Quest const* quest);
        bool TakeQuestSourceItem(uint32 questId, bool msg);
        bool GetQuestRewardStatus(uint32 quest_id) const;
        QuestStatus GetQuestStatus(uint32 quest_id) const;
        void SetQuestStatus(uint32 quest_id, QuestStatus status);
        void RemoveActiveQuest(uint32 quest_id, bool p_BonusQuest = false);
        void RemoveRewardedQuest(uint32 quest_id);
        void RemoveQuestObjectivesInternal(uint32 p_QuestID);

        void SetDailyQuestStatus(uint32 quest_id);
        void SetWeeklyQuestStatus(uint32 quest_id);
        void SetMonthlyQuestStatus(uint32 quest_id);
        void SetSeasonalQuestStatus(uint32 quest_id);
        void ResetDailyQuestStatus();
        bool CanCompleteAdventurePOI(uint32 p_ID) const;
#ifndef CROSS
        void ResetDailyGarrisonDatas();
#endif /* not CROSS */
        void ResetWeeklyQuestStatus();
        void ResetWeeklyGarrisonDatas();
        void ResetMonthlyQuestStatus();
        void ResetSeasonalQuestStatus(uint16 event_id);

        uint16 FindQuestSlot(uint32 quest_id) const;
        uint32 GetQuestSlotQuestId(uint16 slot) const { return GetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET); }
        uint32 GetQuestSlotState(uint16 slot)   const { return GetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET); }
        uint16 GetQuestSlotCounter(uint16 slot, uint8 counter) const
        {
            if (counter < MAX_QUEST_COUNTS)
               return GetUInt16Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET + counter / 2, counter % 2);

            return 0;
        }
        uint32 GetQuestSlotTime(uint16 slot)    const { return GetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET); }
        void SetQuestSlot(uint16 slot, uint32 quest_id, uint32 timer = 0)
        {
            SetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET, quest_id);
            SetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, 0);

            for (uint32 i = 0; i < MAX_QUEST_COUNTS / 2; ++i)
                SetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET + i, 0);

            SetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer);
        }
        void SetQuestSlotCounter(uint16 slot, uint8 counter, uint16 count)
        {
            if (counter >= MAX_QUEST_COUNTS)
               return;

            SetUInt16Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET + counter / 2, counter % 2, count);
        }
        void SetQuestSlotState(uint16 slot, uint32 state) { SetFlag(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, state); }
        void RemoveQuestSlotState(uint16 slot, uint32 state) { RemoveFlag(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, state); }
        void SetQuestSlotTimer(uint16 slot, uint32 timer) { SetUInt32Value(PLAYER_FIELD_QUEST_LOG + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer); }
        void SwapQuestSlot(uint16 slot1, uint16 slot2)
        {
            for (int i = 0; i < MAX_QUEST_OFFSET; ++i)
            {
                uint32 temp1 = GetUInt32Value(PLAYER_FIELD_QUEST_LOG + MAX_QUEST_OFFSET * slot1 + i);
                uint32 temp2 = GetUInt32Value(PLAYER_FIELD_QUEST_LOG + MAX_QUEST_OFFSET * slot2 + i);

                SetUInt32Value(PLAYER_FIELD_QUEST_LOG + MAX_QUEST_OFFSET * slot1 + i, temp2);
                SetUInt32Value(PLAYER_FIELD_QUEST_LOG + MAX_QUEST_OFFSET * slot2 + i, temp1);
            }
        }
        uint16 GetReqKillOrCastCurrentCount(uint32 quest_id, int32 entry);
        void AreaExploredOrEventHappens(uint32 questId, bool p_CheckCanComplete = true);
        void GroupEventHappens(uint32 questId, WorldObject const* pEventObject);
        void ItemAddedQuestCheck(uint32 entry, uint32 count);
        void ItemRemovedQuestCheck(int32 entry, uint32 count);
        void KilledMonster(CreatureTemplate const* cInfo, uint64 guid);
        void KilledMonsterCredit(uint32 entry, uint64 guid = 0);
        void KilledPlayerCredit();
        void CastedCreatureOrGO(uint32 entry, uint64 guid, uint32 spell_id);
        void TalkedToCreature(uint32 entry, uint64 guid);
        void MoneyChanged(uint64 value);
        void ReputationChanged(FactionEntry const* factionEntry);
        void ReputationChanged2(FactionEntry const* factionEntry);
        void ReputationChangedQuestCheck(FactionEntry const* factionEntry);
        bool HasQuestForItem(uint32 itemid) const;
        bool HasQuestForCreature(CreatureTemplate const* cInfo) const;
        bool HasQuestForGO(uint32 GOId) const;
        bool HasQuest(uint32 p_QuestID) const;
        void UpdateForQuestWorldObjects();
        void ForceUpdateForObjectWithEntry(uint32 p_Entry, bool p_Creature);
        bool CanShareQuest(uint32 quest_id) const;
        void QuestObjectiveSatisfy(uint32 objectId, uint32 amount, uint8 type = 0u, uint64 guid = 0u, bool p_Set = false);
        void QuestObjectiveOptionalSatisfy(uint32 objectId);

        void SendQuestComplete(Quest const* quest);
        void SendQuestReward(Quest const* quest, uint32 XP, Object* questGiver);
        void SendQuestFailed(uint32 questId, InventoryResult reason = EQUIP_ERR_OK);
        void SendQuestTimerFailed(uint32 quest_id);
        void SendCanTakeQuestResponse(uint32 msg) const;
        void SendQuestConfirmAccept(Quest const* quest, Player* pReceiver);
        void SendPushToPartyResponse(Player* player, uint32 msg);
        void SendQuestUpdateAddPlayer(Quest const* p_Quest, const QuestObjective & p_Objective, uint16 p_OldCount, uint16 p_AddCount);

        uint64 GetDivider() { return m_divider; }
        void SetDivider(uint64 guid) { m_divider = guid; }

        uint32 GetInGameTime() { return m_ingametime; }

        void SetInGameTime(uint32 time) { m_ingametime = time; }

        void AddTimedQuest(uint32 quest_id) { m_timedquests.insert(quest_id); }
        void RemoveTimedQuest(uint32 quest_id) { m_timedquests.erase(quest_id); }

        /*********************************************************/
        /***                   LOAD SYSTEM                     ***/
        /*********************************************************/

        bool LoadFromDB(uint32 guid, SQLQueryHolder* holder, SQLQueryHolder* p_LoginDBQueryHolder);
        bool isBeingLoaded() const override { return GetSession()->PlayerLoading();}

        void Initialize(uint32 guid);
        static uint32 GetUInt32ValueFromArray(Tokenizer const& data, uint16 index);
        static uint64 GetUInt64ValueFromArray(Tokenizer const& p_Datas, uint16 p_Index);
        static float  GetFloatValueFromArray(Tokenizer const& data, uint16 index);
        static uint32 GetZoneIdFromDB(uint64 guid);
        static uint32 GetLevelFromDB(uint64 guid);
        static bool   LoadPositionFromDB(uint32& mapid, float& x, float& y, float& z, float& o, bool& in_flight, uint64 guid);

        static bool IsValidGender(uint8 Gender) { return Gender <= GENDER_FEMALE; }
        static bool IsValidClass(uint8 Class) { return ((1 << (Class - 1)) & CLASSMASK_ALL_PLAYABLE) != 0; }
        static bool IsValidRace(uint8 Race) { return ((1 << (Race - 1)) & RACEMASK_ALL_PLAYABLE) != 0; }
        static bool ValidateAppearance(uint8 p_Race, uint8 p_Class, uint8 p_Gender, uint8 p_HairID, uint8 p_HairColor, uint8 p_FaceID, uint8 p_FacialHair, uint8 p_SkinColor, std::array<uint8, PLAYER_CUSTOM_DISPLAY_SIZE> const& p_CustomDisplay, bool p_Create = false);

        /*********************************************************/
        /***                   SAVE SYSTEM                     ***/
        /*********************************************************/

        void SaveToDB(bool create = false, bool logout = false, MS::Utilities::CallBackPtr p_Callback = nullptr);
        void SaveInventoryAndGoldToDB(SQLTransaction& trans);                    // fast save function for item/money cheating preventing
        void SaveGoldToDB(SQLTransaction& trans);

        static void SetUInt32ValueInArray(Tokenizer& data, uint16 index, uint32 value);
        static void Customize(uint64 guid, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair, std::array<uint8, PLAYER_CUSTOM_DISPLAY_SIZE> const& p_CustomDisplay);
        static void SavePositionInDB(uint32 mapid, float x, float y, float z, float o, uint32 zone, uint64 guid);

        void SaveSpells();

#ifndef CROSS
        static void DeleteFromDB(uint64 playerguid, uint32 accountId, bool updateRealmChars = true, bool deleteFinally = false);
#endif
        static void DeleteOldCharacters();
        static void DeleteOldCharacters(uint32 keepDays);

        bool m_mailsUpdated;

        void SetBindPoint(uint64 guid);
        void SendTalentWipeConfirm(uint64 guid, uint32 cost);
        void RegenerateAll();
        void Regenerate(Powers power);
        void ResetAllPowerCombatTimers();
        void RegenerateHealth();
        void setRegenTimerCount(uint32 time) {m_regenTimerCount = time;}
        void setWeaponChangeTimer(uint32 time) {m_weaponChangeTimer = time;}
        Powers GetDisplayPowerForCurrentState() const;

        uint64 GetMoney() const { return GetUInt64Value(PLAYER_FIELD_COINAGE); }
        void ModifyMoney(int64 d, std::string p_Source = "");
        bool HasEnoughMoney(uint64 amount) const { return GetMoney() >= amount; }
        bool HasEnoughMoney(int64 amount) const
        {
            if (amount > 0)
                return (GetMoney() >= (uint64)amount);
            return true;
        }

        void SetMoney(uint64 value);

        RewardedQuestSet const& getRewardedQuests() const { return m_RewardedQuests; }
        QuestStatusMap& getQuestStatusMap() { return m_QuestStatus; }

        size_t GetRewardedQuestCount() const { return m_RewardedQuests.size(); }
        bool IsQuestRewarded(uint32 quest_id) const
        {
            return m_RewardedQuests.find(quest_id) != m_RewardedQuests.end();
        }

        void SendWorldQuestUpdate();
        MS::WorldQuest::Manager* GetWorldQuestManager() { return m_WorldQuestMgr; }

        void SendAllTaskProgress();

        uint64 GetSelection() const { return m_curSelection; }
        Unit* GetSelectedUnit() const;
        Player* GetSelectedPlayer() const;
        void SetSelection(uint64 guid) { m_curSelection = guid; SetGuidValue(UNIT_FIELD_TARGET, guid); }

        void SendMailResult(uint32 mailId, MailResponseType mailAction, MailResponseResult mailError, uint32 equipError = 0, uint32 item_guid = 0, uint32 item_count = 0);
        void SendNewMail();
        void UpdateNextMailTimeAndUnreads();
        void AddNewMailDeliverTime(time_t deliver_time);

        void RemoveMail(uint32 id);

        void AddMail(Mail* mail) { m_mail.push_front(mail);}// for call from WorldSession::SendMailTo
        uint32 GetMailSize() { return m_mail.size();}
        Mail* GetMail(uint32 id);

        PlayerMails::iterator GetMailBegin() { return m_mail.begin();}
        PlayerMails::iterator GetMailEnd() { return m_mail.end();}

        /*********************************************************/
        /*** MAILED ITEMS SYSTEM ***/
        /*********************************************************/

        uint8 unReadMails;
        time_t m_nextMailDelivereTime;

        typedef std::map<uint32, Item*> ItemMap;

        ItemMap mMitems;                                    //template defined in objectmgr.cpp

        Item* GetMItem(uint32 id)
        {
            ItemMap::const_iterator itr = mMitems.find(id);
            return itr != mMitems.end() ? itr->second : NULL;
        }

        void AddMItem(Item* it)
        {
            ASSERT(it);
            //ASSERT deleted, because items can be added before loading
            mMitems[it->GetGUIDLow()] = it;
        }

        bool RemoveMItem(uint32 id)
        {
            return mMitems.erase(id) ? true : false;
        }

        void PetSpellInitialize();
        void CharmSpellInitialize();
        void PossessSpellInitialize();
        void VehicleSpellInitialize();
        void SendRemoveControlBar();
        void SendKnownSpells();
        bool HasSpell(uint32 spell) const override;
        bool HasOverridedSpell(uint32 p_SpellID) const;
        bool HasActiveSpell(uint32 spell) const;            // show in spellbook
        SpellInfo const* GetCastSpellInfo(SpellInfo const* spellInfo) const override;
        TrainerSpellState GetTrainerSpellState(TrainerSpell const* trainer_spell) const;
        bool IsSpellFitByClassAndRace(uint32 spell_id) const;
        bool IsNeedCastPassiveSpellAtLearn(SpellInfo const* spellInfo) const;

        void SendProficiency(ItemClass itemClass, uint32 itemSubclassMask);
        bool addSpell(uint32 spellId, bool active, bool learning, bool dependent, bool disabled, bool loading = false, bool p_IsMountFavorite = false, bool p_LearnBattlePet = true, bool p_FromShopItem = false, uint32 fromSkill = 0, bool p_ByPassSpec = false);
        void learnSpell(uint32 spell_id, bool dependent, bool p_FromItemShop = false, bool p_Temp = false, uint32 fromSkill = 0, bool p_ByPassSpec = false);
        void removeSpell(uint32 spell_id, bool disabled = false, bool learn_low_rank = true);
        void resetSpells(bool myClassOnly = false);
        void LearnCustomSpells();
        void LearnDefaultSkills();
        void LearnDefaultSkill(SkillRaceClassInfoEntry const* rcInfo);
        void LearnSpecializationSpells();
        void EquipProperArtifactIfNeeded();
        void RemoveSpecializationSpells();
        void learnQuestRewardedSpells();
        void learnQuestRewardedSpells(Quest const* quest);
        void learnSpellHighRank(uint32 spellid);
        void DeleteInvalidSpells();
        void AddTemporarySpell(uint32 spellId);
        void RemoveTemporarySpell(uint32 spellId);
        void AddOverrideSpell(uint32 overridenSpellId, uint32 newSpellId);
        void RemoveOverrideSpell(uint32 overridenSpellId, uint32 newSpellId);
        void SetReputation(uint32 factionentry, uint32 value);
        uint32 GetReputation(uint32 factionentry);
        std::string GetGuildName();

        void MountSetFavorite(uint32 p_SpellID, bool p_IsFavorite);

        //////////////////////////////////////////////////////////////////////////
        /// Talent
        //////////////////////////////////////////////////////////////////////////
        uint32 GetTalentResetCost() const;
        void SetTalentResetCost(uint32 cost);
        time_t GetTalentResetTime() const;
        void SetTalentResetTime(time_t time_);
        uint32 GetPrimarySpecialization() const;
        void SetPrimarySpecialization(uint32 spec);
        uint8 GetActiveTalentGroup() const;
        void SetActiveTalentGroup(uint8 group);
        uint32 GetDefaultSpecId() const;
        uint32 GetActiveSpecializationID() const;

        bool ResetTalents(bool noCost = false);
        uint32 GetNextResetTalentsCost() const;
        void InitTalentForLevel();
        void SendTalentsInfoData();
        TalentLearnResult LearnTalent(uint32 talentId, int32* spellOnCooldown, bool p_Forced = false);
        bool AddTalent(TalentEntry const* talent, uint8 spec, bool learning);
        bool HasTalent(uint32 talentId, uint8 group) const;
        void RemoveTalent(TalentEntry const* talent);
        uint32 CalculateTalentsTiers() const;
        void ResetTalentSpecialization();

        //////////////////////////////////////////////////////////////////////////
        /// PvP Talent
        //////////////////////////////////////////////////////////////////////////
        bool HasPvPTalent(uint32 p_ID) const;
        uint32 GetPvPTalentAtTier(uint32 p_TierIndex) const;
        void StorePvPTalent(PvpTalentEntry const* p_TalentEntry);
        void RemoveInteralPvPTalent(uint32 p_TierIndex);
        void LoadAddPvPTalentFromDB(uint32 p_Talent, uint8 p_Spec);
        void RemoveAllPvpTalents();
        bool LearnPvPTalent(uint32 p_TalentID);
        void RemovePvPTalent(uint32 p_TalentID);
        void ActivatePvPTalents();
        void DeactivatePvPTalents();

        PvPStatsSpells SelectPvpStatsSpell(int32 p_AlternateSpec = SPEC_NONE);
        bool IsInBattleground() const;
        void SetInBattleground(bool p_Apply);
        bool IsInBattlegroundProcess() const;
        void SetInBattlegroundProcess(bool p_Apply);
        bool IsInBattlegroundLeaving() const;
        void SetInBattlegroundLeaving(bool p_Apply);
        bool IsInDuel() const;
        void SetInDuel(bool p_Apply);

        // Dual Spec
        void ActivateTalentGroup(ChrSpecializationsEntry const* spec);

        PlayerTalentMap const* GetTalentMap(uint8 spec) const { return &_talentMgr->Talents[spec]; }
        PlayerTalentMap* GetTalentMap(uint8 spec) { return &_talentMgr->Talents[spec]; }
        std::vector<uint32> const* GetPvPTalentMap(uint8 spec) const { return &_talentMgr->PvPTalents[spec]; }
        std::vector<uint32>* GetPvPTalentMap(uint8 spec) { return &_talentMgr->PvPTalents[spec]; }
        ActionButtonList const& GetActionButtons() const { return m_actionButtons; }
        std::vector<uint32> const& GetGlyphs(uint8 spec) const { return _talentMgr->Glyphs[spec]; }
        std::vector<uint32>& GetGlyphs(uint8 spec) { return _talentMgr->Glyphs[spec]; }

        uint32 GetRoleForGroup(uint32 specializationId = 0) const;
#ifdef CROSS

#endif /* CROSS */
        bool IsHealer() const;
        bool IsRangedDamageDealer(bool p_AllowHeal = true) const;
        bool IsMeleeDamageDealer(bool p_AllowTank = false, bool p_AllowHeal = true) const;

        static uint32 GetRoleBySpecializationId(uint32 specializationId);

        Stats GetPrimaryStat() const;
        bool IsActiveSpecTankSpec() const;

        /// Custom functions for spells
        void HandleWarlockWodPvpBonus();
        uint32 GetRandomWeaponFromPrimaryBag(ItemTemplate const* p_Transmogrified) const;

        uint32 GetFreePrimaryProfessionPoints() const { return GetUInt32Value(PLAYER_FIELD_CHARACTER_POINTS); }
        void SetFreePrimaryProfessions(uint16 profs) { SetUInt32Value(PLAYER_FIELD_CHARACTER_POINTS, profs); }
        void InitPrimaryProfessions();

        PlayerSpellMap const& GetSpellMap() const { return m_spells; }
        PlayerSpellMap      & GetSpellMap()       { return m_spells; }

        SpellCooldowns const& GetSpellCooldownMap() const { return m_spellCooldowns; }

        void ClearAllCooldowns() { m_spellCooldowns.clear(); }
        void SetupAllCooldowns(SpellCooldowns p_Cooldowns) { m_spellCooldowns = p_Cooldowns; }

        void AddSpellMod(SpellModifier* mod, bool apply);
        bool IsAffectedBySpellmod(SpellInfo const* spellInfo, SpellModifier* mod, Spell* p_SpellModTakingSpell = nullptr, Spell* p_OriginalSpell = nullptr);
        template <class T> void ApplySpellMod(uint32 p_SpellId, SpellModOp p_Op, T &p_Basevalue, Spell* p_Spell = nullptr, bool p_RemoveStacks = true, std::ostringstream* p_DamageLogStream = nullptr);
        void RemoveSpellMods(Spell* spell);
        void RestoreSpellMods(Spell* spell, uint32 ownerAuraId = 0, Aura* aura = nullptr);
        void RestoreAllSpellMods(uint32 ownerAuraId = 0, Aura* aura = nullptr);
        void DropModCharge(SpellModifier* mod, Spell* spell);
        void SetSpellModTakingSpell(Spell* spell, bool apply);
        bool NeedToConsumeSpellMod(SpellInfo const* p_SpellInfo, SpellModifier* p_Mod) const;

        static uint64 const infinityCooldownDelay = uint64(2592000LL * 1000LL); // MONTH * IN_MILLISECONDS
        static uint64 const infinityCooldownDelayCheck = uint64(2592000LL/2 * 1000LL); // MONTH/2 * IN_MILLISECONDS

        bool HasSpellCooldown(uint32 p_SpellID) const;
        uint32 GetSpellCooldownDelay(uint32 p_SpellID) const;
        void AddSpellAndCategoryCooldowns(SpellInfo const* spellInfo, uint32 itemId, Spell* spell = nullptr, bool infinityCooldown = false);
        void AddSpellCooldown(uint32 spell_id, uint32 itemid, uint64 end_time, bool send = false);
        void SendCategoryCooldown(uint32 categoryId, int32 cooldown);
        void SendCooldownEvent(const SpellInfo* p_SpellInfo, uint32 p_ItemID = 0, Spell* p_Spell = nullptr, bool p_SetCooldown = true, uint64 p_ItemGUID = 0);
        void SendCooldownEvent(uint32 p_SpellID);
        void SendItemExpirePurchaseRefund(uint64 p_Guid);
        void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs, uint32 p_SpellID = 0) override;
        void ReduceSpellCooldown(uint32 spell_id, uint32 modifyTime);
        void RemoveSpellCooldown(uint32 spell_id, bool update = false);
        void SendClearCooldown(uint32 p_SpellID, Unit * p_Target, bool p_ClearOnHold = false);
        void ModifySpellCooldown(uint32 p_SpellID, int32 p_Delta);
        void SendModifyCooldownRecoveryRate(uint32 p_SpellID, float p_AddedMod, float p_NewMod);
        void SendModifyChargeRecoveryRate(uint32 p_ChargeCategoryId, float p_Mod, float p_NewMod);

        GlobalCooldownMgr& GetGlobalCooldownMgr() { return m_GlobalCooldownMgr; }

        void RemoveArenaSpellCooldowns(bool removeActivePetCooldowns = false);
        void RemoveAllSpellCooldown();
        void RemoveSpellCooldownsWithTime(uint32 p_MinRecoveryTime);
        void _LoadSpellCooldowns(PreparedQueryResult result);
        void _LoadChargesCooldowns(PreparedQueryResult p_Result);
        void _SaveSpellCooldowns(SQLTransaction& trans);
        void _SaveChargesCooldowns(SQLTransaction& p_Transaction);

        uint32 GetLastPotionItemId(SpellInfo const* p_SpellInfo) const;
        void SetLastPotion(SpellInfo const* p_SpellInfo, Item const* p_CastItem);

        void UpdatePotionCooldown(Spell* spell = nullptr);

        void SetResurrectRequestData(Unit* caster, uint32 health, uint32 mana, uint32 appliedAura, SpellInfo const* p_ResSpell = nullptr)
        {
            ASSERT(!IsRessurectRequested());
            _resurrectionData = new ResurrectionData();
            _resurrectionData->GUID = caster->GetGUID();
            _resurrectionData->Location.WorldRelocate(*caster);
            _resurrectionData->Health = health;
            _resurrectionData->Mana = mana;
            _resurrectionData->Aura = appliedAura;
            _resurrectionData->ResSpell = p_ResSpell;
        }

        void ClearResurrectRequestData()
        {
            m_DelayedOperations = m_DelayedOperations &~ DELAYED_RESURRECT_PLAYER;
            delete _resurrectionData;
            _resurrectionData = NULL;
        }
        bool IsRessurectRequestedBy(uint64 guid) const
        {
            if (!IsRessurectRequested())
                return false;

            return _resurrectionData->GUID == guid;
        }

        bool IsRessurectRequested() const { return _resurrectionData != NULL; }

        void ResurectUsingRequestData();

        void SendForcedDeathUpdate();
        void SendGameError(GameError::Type p_Error, uint32 p_Data1 = 0xF0F0F0F0, uint32 p_Data2 = 0xF0F0F0F0);

        uint8 getCinematic()
        {
            return m_cinematic;
        }
        void setCinematic(uint8 cine)
        {
            m_cinematic = cine;
        }

        ActionButton* addActionButton(uint8 button, uint32 action, uint8 type);
        void removeActionButton(uint8 button);
        ActionButton const* GetActionButton(uint8 button);
        void SendInitialActionButtons() const { SendActionButtons(0); }
        void SendActionButtons(uint32 state) const;
        bool IsActionButtonDataValid(uint8 button, uint32 action, uint8 type);

        int8 GetFreeActionButton();

        PvPInfo pvpInfo;
        void UpdatePvPState(bool onlyFFA = false);
        void SetPvP(bool state)
        {
            Unit::SetPvP(state);
            for (ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
                (*itr)->SetPvP(state);
        }
        void UpdatePvP(bool state, bool override=false);
        void UpdateZone(uint32 newZone, uint32 newArea);
        void UpdateArea(uint32 newArea);

        uint32 GetZoneId(bool forceRecalc = false) const override;
        uint32 GetAreaId(bool forceRecalc = false) const override;
        void GetZoneAndAreaId(uint32& zoneid, uint32& areaid, bool forceRecalc = false) const override;

        void UpdateZoneDependentAuras(uint32 zone_id);    // zones
        void UpdateAreaDependentAuras(uint32 area_id);    // subzones

        bool IsInAreaGroup(uint32 p_AreaGroupID);

        void UpdateAfkReport(time_t currTime);
        void UpdatePvPFlag(time_t currTime);
        void UpdateContestedPvP(uint32 currTime);
        void SetContestedPvPTimer(uint32 newTime) {m_contestedPvPTimer = newTime;}
        void ResetContestedPvP()
        {
            ClearUnitState(UNIT_STATE_ATTACK_PLAYER);
            RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP);
            m_contestedPvPTimer = 0;

        }

        /** todo: -maybe move UpdateDuelFlag+DuelComplete to independent DuelHandler.. **/
        DuelInfo* m_Duel;
        void UpdateDuelFlag(time_t currTime);
        void CheckDuelDistance(time_t currTime);
        void DuelComplete(DuelCompleteType type);
        void SendDuelCountdown(uint32 counter);

        bool IsGroupVisibleFor(Player const* p) const;
        bool IsInSameGroupWith(Player const* p) const;
        bool IsInSameRaidWith(Player const* p) const { return p == this || (GetGroup() != NULL && GetGroup() == p->GetGroup()); }
        void UninviteFromGroup();
        static void RemoveFromGroup(GroupPtr group, uint64 guid, RemoveMethod method = GROUP_REMOVEMETHOD_DEFAULT, uint64 kicker = 0, const char* reason = NULL);
        void RemoveFromGroup(RemoveMethod method = GROUP_REMOVEMETHOD_DEFAULT) { RemoveFromGroup(GetGroup(), GetGUID(), method); }
        void SendUpdateToOutOfRangeGroupMembers();

        void SetInGuild(uint32 guildId);
        void SetRank(uint8 rankId) { SetUInt32Value(PLAYER_FIELD_GUILD_RANK_ID, rankId); }
        uint32 GetRank() { return GetUInt32Value(PLAYER_FIELD_GUILD_RANK_ID); }
        void SetGuildLevel(uint32 level) { SetUInt32Value(PLAYER_FIELD_GUILD_LEVEL, level); }
        uint32 GetGuildLevel() { return GetUInt32Value(PLAYER_FIELD_GUILD_LEVEL); }
        void SetGuildIdInvited(uint32 GuildId) { m_GuildIdInvited = GuildId; }
        uint32 GetGuildId() const { return GetUInt32Value(OBJECT_FIELD_DATA); /* return only lower part */ }

#ifndef CROSS
        Guild* GetGuild();
#else
        uint64 GetGuildGUID() const { return GetGuidValue(OBJECT_FIELD_DATA); }
        InterRealmGuild* GetGuild();
#endif

        static uint32 GetGuildIdFromDB(uint64 guid, uint32 realmId);
        int GetGuildIdInvited() { return m_GuildIdInvited; }
        static void RemovePetitionsAndSigns(uint64 guid, uint32 type);

        /// PvP Bracket data
        uint32 GetArenaPersonalRating(uint8 p_Bracket) const { return m_ArenaPersonalRating[p_Bracket]; }
        uint32 GetBestRatingOfWeek(uint8 p_Bracket) const { return m_BestRatingOfWeek[p_Bracket]; }
        uint32 GetBestRatingOfPreviousWeek(uint8 p_Bracket) const { return m_BestRatingOfPrevWeek[p_Bracket]; }
        uint32 GetBestRatingOfSeason(uint8 p_Bracket) const { return m_BestRatingOfSeason[p_Bracket]; }
        uint32 GetDayWins(uint8 p_Bracket) const { return m_DayWins[p_Bracket]; }
        uint32 GetDayGames(uint8 p_Bracket) const { return m_DayGames[p_Bracket]; }
        uint32 GetWeekWins(uint8 p_Bracket) const { return m_WeekWins[p_Bracket]; }
        uint32 GetPrevWeekWins(uint8 p_Bracket) const { return m_PrevWeekWins[p_Bracket]; }
        uint32 GetPrevWeekGames(uint8 p_Bracket) const { return m_PrevWeekGames[p_Bracket]; }
        uint32 GetSeasonWins(uint8 p_Bracket) const { return m_SeasonWins[p_Bracket]; }
        uint32 GetWeekGames(uint8 p_Bracket) const { return m_WeekGames[p_Bracket]; }
        uint32 GetSeasonGames(uint8 p_Bracket) const { return m_SeasonGames[p_Bracket]; }
        uint32 GetArenaMatchMakerRating(uint8 p_Bracket) const { return m_ArenaMatchMakerRating[p_Bracket]; }
        bool HasWinInBracket(BattlegroundBracketType p_BracketType, bool p_Daily) const;

#ifdef CROSS
        void InitArenaPersonalRating(uint8 p_Bracket, uint32 value) { m_ArenaPersonalRating[p_Bracket] = value; }
        void InitBestRatingOfWeek(uint8 p_Bracket, uint32 value) { m_BestRatingOfWeek[p_Bracket] = value; }
        void InitBestRatingOfSeason(uint8 p_Bracket, uint32 value) { m_BestRatingOfSeason[p_Bracket] = value; }
        void InitWeekWins(uint8 p_Bracket, uint32 value) { m_WeekWins[p_Bracket] = value; }
        void InitPrevWeekWins(uint8 p_Bracket, uint32 value) { m_PrevWeekWins[p_Bracket] = value; }
        void InitSeasonWins(uint8 p_Bracket, uint32 value) { m_SeasonWins[p_Bracket] = value; }
        void InitWeekGames(uint8 p_Bracket, uint32 value) { m_WeekGames[p_Bracket] = value; }
        void InitSeasonGames(uint8 p_Bracket, uint32 value) { m_SeasonGames[p_Bracket] = value; }
        void InitArenaMatchMakerRating(uint8 p_Bracket, uint32 value) { m_ArenaMatchMakerRating[p_Bracket] = value; }
#endif

        void SetArenaPersonalRating(uint8 p_Slot, uint32 p_Value);
        void SetArenaMatchMakerRating(uint8 slot, uint32 value);

        void IncrementWeekGames(uint8 slot);
        void IncrementWeekWins(uint8 slot);
        void IncrementDayWins(uint8 slot);
        void IncrementDayGames(uint8 slot);
        void IncrementSeasonGames(uint8 slot);
        void IncrementSeasonWins(uint8 slot);

        void FinishWeek();
        void FinishDay();

        void SendStartTimer(uint32 p_Time, uint32 p_MaxTime, uint8 p_Type);

        Difficulty GetDifficultyID(MapEntry const* mapEntry) const;
        Difficulty GetDungeonDifficultyID() const { return m_dungeonDifficulty; }
        Difficulty GetRaidDifficultyID() const { return m_raidDifficulty; }
        Difficulty GetLegacyRaidDifficultyID() const { return m_LegacyRaidDifficulty; }
        void SetDungeonDifficultyID(Difficulty dungeon_difficulty) { m_dungeonDifficulty = dungeon_difficulty; }
        void SetRaidDifficultyID(Difficulty raid_difficulty) { m_raidDifficulty = raid_difficulty; }
        void SetLegacyRaidDifficultyID(Difficulty raid_difficulty) { m_LegacyRaidDifficulty = raid_difficulty; }
        static Difficulty CheckLoadedDungeonDifficultyID(Difficulty difficulty);
        static Difficulty CheckLoadedRaidDifficultyID(Difficulty difficulty);
        static Difficulty CheckLoadedLegacyRaidDifficultyID(Difficulty difficulty);

        bool UpdateSkill(uint32 skill_id, uint32 step);
        bool UpdateSkillPro(uint16 SkillId, int32 Chance, uint32 step);

        bool UpdateCraftSkill(uint32 spellid);
        bool UpdateGatherSkill(uint32 SkillId, uint32 SkillValue, uint32 RedLevel, uint32 Multiplicator = 1);
        bool UpdateFishingSkill();

        float GetHealthBonusFromStamina();

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType, bool l_NoLongerDualWields = false) override;
        void ApplySpellPowerBonus(int32 amount, bool apply);
        void UpdateSpellDamageAndHealingBonus();
        void ApplyRatingMod(CombatRating cr, int32 value, bool apply);
        void UpdateRating(CombatRating cr);
        bool HasHasteRatingRequirement(CombatRating p_CombatRating);
        void UpdateItemLevel();
        void UpdateAllRatings();
        int16 GetBaseRatingValue(CombatRating p_CombatRating) const { return m_baseRatingValue[p_CombatRating]; }

        void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& min_damage, float& max_damage, bool l_NoLongerDualWields = false);
        void CalculateNormalizedWeaponDamage(WeaponAttackType attType, float& min_damage, float& max_damage, float attackPower, float weapon_mindamage, float weapon_maxdamage, Item* l_UsedWeapon);

        inline void RecalculateRating(CombatRating cr) { ApplyRatingMod(cr, 0, true);}
        float GetRatingMultiplier(CombatRating cr) const;
        float GetRatingBonusValue(CombatRating cr) const;
        uint32 GetBaseSpellPowerBonus() const { return m_baseSpellPower; }
        int32 GetSpellPenetrationItemMod() const { return m_spellPenetrationItemMod; }

        void UpdateBlockPercentage();
        void UpdateCritPercentage(WeaponAttackType attType);
        void UpdateAllCritPercentages();
        void UpdateParryPercentage();
        void UpdateDodgePercentage();
        void UpdateMeleeHitChances();
        void UpdateRangedHitChances();
        void UpdateSpellHitChances();
        void UpdateMasteryPercentage();
        void UpdateLeechPercentage();
        void UpdateVersatilityPercentage();
        void UpdateAvoidancePercentage();
        void UpdateSpeedPercentage();
        void HandleBlockMastery(AuraEffect* p_AurEff, float p_Amount);

        float GetPvpHealingBonus() const;

        void UpdateSpellCritChance(uint32 p_School);
        void ApplyManaRegenBonus(int32 amount, bool apply);
        void ApplyHealthRegenBonus(int32 amount, bool apply);
        void UpdateManaRegen();
        void UpdateEnergyRegen();
        void UpdateFocusRegen();
        void UpdateAllRunesRegen();
        float GetRegenForPower(Powers p_Power);

        bool CanSwitch() const;
        bool IsInWorgenForm() const { return HasAuraType(SPELL_AURA_ALLOW_WORGEN_TRANSFORM); }
        void SwitchToHumanForm() { RemoveAurasByType(SPELL_AURA_ALLOW_WORGEN_TRANSFORM); }
        void SwitchToWorgenForm() { CastSpell(this, 97709, true); }
        void SwitchForm();

        uint64 GetLootGUID() const { return m_lootGuid; }
        void SetLootGUID(uint64 guid) { m_lootGuid = guid; }

        uint32 GetLootSpecId() const { return m_lootSpecId; }
        void SetLootSpecId(uint32 specId) { m_lootSpecId = specId; }

        uint32 GetBonusRollFails() const { return GetCharacterWorldStateValue(CharacterWorldStates::GlobalBadLuckCounter); }
        void IncreaseBonusRollFails() { ModifyCharacterWorldState(CharacterWorldStates::GlobalBadLuckCounter, 1); }
        void ResetBonusRollFails() { SetCharacterWorldState(CharacterWorldStates::GlobalBadLuckCounter, 0); }

        void RemovedInsignia(Player* looterPlr);

        WorldSession* GetSession() const { return m_session; }
        void SetSession(WorldSession* p_Session) { m_session = p_Session; }

        virtual void SetObjectScale(float scale) override;

        void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const override;
        void DestroyForPlayer(Player* target, bool onDeath = false) const override;
        void SendLogXPGain(uint32 GivenXP, Unit* victim, uint32 BonusXP, bool recruitAFriend = false, float group_rate=1.0f);

        // notifiers
        void SendAttackSwingError(AttackSwingError p_Error);
        void SendAttackSwingCancelAttack();
        void SendAutoRepeatCancel(Unit* target);
        void SendExplorationExperience(uint32 Area, uint32 Experience);

        void SendDungeonDifficulty(int32 p_ForcedDifficulty = -1);
        void SendRaidDifficulty(bool legacy, int32 forcedDifficulty = -1);
        void ResetInstances(uint8 method, bool isRaid, bool isLegacy);
        void SendResetInstanceSuccess(uint32 MapId);
        void SendResetInstanceFailed(ResetFailedReason reason, uint32 MapId);
        void SendResetFailedNotify();

        virtual bool UpdatePosition(float x, float y, float z, float orientation, bool teleport = false) override;
        void UpdateWorldQuests(uint32 p_OldAreaId, uint32 p_NewAreaId);
        void UpdateWorldQuest(MS::WorldQuest::Template const* p_WQuestTemplate, bool p_IsInArea);

        bool UpdatePosition(const Position &pos, bool teleport = false) { return UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), teleport); }
        void UpdateUnderwaterState(Map* m, float x, float y, float z) override;

        Corpse* GetCorpse() const;
        void SpawnCorpseBones(bool p_TriggerSave = true);
        Corpse* CreateCorpse();
        bool HasCorpse() const { return m_CorpseLocation.GetMapId() != MAPID_INVALID; }
        WorldLocation GetCorpseLocation() const { return m_CorpseLocation; }
        void KillPlayer();
        uint32 GetResurrectionSpellId();
        uint32 GetFirstDescriptorRessurectSpell() const;
        void SetResurectSpell(uint32 p_SpellID);
        void ResurrectPlayer(float restore_percent, bool applySickness = false);
        void BuildPlayerRepop();
        void RepopAtGraveyard(bool p_ForceGraveYard = false);
        void TeleportToClosestGrave(float p_X, float p_Y, float p_Z, float p_O, uint32 p_MapId, bool p_Packet = true);
        void TeleportToClosestGrave(WorldSafeLocsEntry const* p_WorldSafeLoc) { TeleportToClosestGrave(p_WorldSafeLoc->x, p_WorldSafeLoc->y, p_WorldSafeLoc->z, p_WorldSafeLoc->o, p_WorldSafeLoc->MapID);  }
        void SendCemeteryList(bool onMap);

        void DurabilityLossAll(double percent, bool inventory);
        void DurabilityLoss(Item* item, double percent);
        void DurabilityPointsLossAll(int32 points, bool inventory);
        void DurabilityPointsLoss(Item* item, int32 points);
        void DurabilityPointLossForEquipSlot(EquipmentSlots slot);
        uint32 DurabilityRepairAll(bool cost, float discountMod, bool guildBank);
        uint32 DurabilityRepair(uint16 pos, bool cost, float discountMod, bool guildBank);

        void UpdateMirrorTimers();
        void StopMirrorTimers()
        {
            StopMirrorTimer(FATIGUE_TIMER);
            StopMirrorTimer(BREATH_TIMER);
            StopMirrorTimer(FIRE_TIMER);
        }
        bool IsMirrorTimerActive(MirrorTimerType type) { return m_MirrorTimer[type] == getMaxTimer(type); }

        bool CanJoinConstantChannelInZone(ChatChannelsEntry const* channel, AreaTableEntry const* zone);

        void JoinedChannel(Channel* c);
        void LeftChannel(Channel* c);
        void CleanupChannels();
        void UpdateChatLocaleFiltering();
        void UpdateLocalChannels(uint32 newZone);
        void LeaveLFGChannel();

        void SetSkill(uint16 id, uint16 step, uint16 currVal, uint16 maxVal);
        uint16 GetMaxSkillValue(uint32 skill) const;        // max + perm. bonus + temp bonus
        uint16 GetPureMaxSkillValue(uint32 skill) const;    // max
        uint16 GetSkillValue(uint32 skill) const;           // skill value + perm. bonus + temp bonus
        uint16 GetBaseSkillValue(uint32 skill) const;       // skill value + perm. bonus
        uint16 GetPureSkillValue(uint32 skill) const;       // skill value
        int16 GetSkillPermBonusValue(uint32 skill) const;
        int16 GetSkillTempBonusValue(uint32 skill) const;
        uint16 GetSkillStep(uint16 skill) const;            // 0...6
        bool HasSkill(uint32 skill) const;
        void learnSkillRewardedSpells(uint32 id, uint32 value);

        WorldLocation& GetTeleportDest() { return m_teleport_dest; }
        bool IsBeingTeleported() const { return mSemaphoreTeleport_Near || mSemaphoreTeleport_Far; }
        bool IsBeingTeleportedNear() const { return mSemaphoreTeleport_Near; }
        bool IsBeingTeleportedFar() const { return mSemaphoreTeleport_Far; }
        void SetSemaphoreTeleportNear(bool semphsetting) { mSemaphoreTeleport_Near = semphsetting; }
        void SetSemaphoreTeleportFar(bool semphsetting) { mSemaphoreTeleport_Far = semphsetting; }
        void ProcessDelayedOperations();
        void SetTeleportFlagForAnticheat(bool on) { mTeleportFlagForAnticheat = on; }
        bool GetTeleportFlagForAnticheat() { return mTeleportFlagForAnticheat; }

        void CheckAreaExploreAndOutdoor(void);
        bool m_IsOutdoors;

        static uint32 TeamForRace(uint8 race);
        static TeamId TeamIdForRace(uint8 p_Race);
        uint32 GetTeam() const { return m_team; }
        TeamId GetTeamId() const { if (m_team == ALLIANCE) return TEAM_ALLIANCE; if (m_team == HORDE) return TEAM_HORDE; return TEAM_NEUTRAL; }
        void SwitchToOppositeTeam(bool p_Apply);
        uint32 GetBgQueueTeam() const;
        void setFactionForRace(uint8 race);
        uint32 GetRacialSkillID(bool p_Mercenary = false) const;
        void DeactivateMercenary();
        void SavePlayerBytes(uint32 p_Bytes) { m_OriginalPlayerBytes = p_Bytes; }
        void ActivateMercenary(uint8 p_MercenaryRace);
        void SetOriginalRace(uint8 p_Race) { m_OriginalRace = p_Race; }
        uint8 GetOriginalRace() const { return m_OriginalRace; }
        bool IsMercenary() const { return (HasAura(MercenarySpells::MercenaryContractHorde) || HasAura(MercenarySpells::MercenaryContractAlliance) || HasAura(MercenarySpells::MercenaryHorde1) || HasAura(MercenarySpells::MercenaryAlliance1)); }
        uint32 GetOriginalPlayerData() const { return m_OriginalPlayerBytes; }
        uint32 GetNativeTeam() const { return TeamForRace(getRace()); }
        TeamId GetNativeTeamId() const { return TeamIdForRace(getRace()); }

        void InitDisplayIds(uint8 p_MercenaryRace = 0);

        bool IsAtGroupRewardDistance(WorldObject const* pRewardSource) const;
        void RewardCurrencyAtKill(Unit* victim);
        void RewardPersonnalCurrencies(Unit* p_Victim);
        void RewardPlayerAndGroupAtKill(Unit* victim, bool isBattleGround, std::set<uint64>& p_RewardedPlayers);
        void RewardPlayerAndGroupAtEvent(uint32 creature_id, WorldObject* pRewardSource);
        bool isHonorOrXPTarget(Unit* victim);

        uint8 GetGrantableLevels() { return m_grantableLevels; }
        void SetGrantableLevels(uint8 val) { m_grantableLevels = val; }

        Expansion GetExpByLevel();

        ReputationMgr&       GetReputationMgr()       { return m_reputationMgr; }
        ReputationMgr const& GetReputationMgr() const { return m_reputationMgr; }
        ReputationRank GetReputationRank(uint32 faction_id) const;
        void RewardReputation(Unit* victim, float rate);
        void RewardReputation(Quest const* quest);
        void RewardGuildReputation(Quest const* quest);

        void UpdateSkillsForLevel();
        void UpdateSkillsToMaxSkillsForLevel();             // for .levelup
        void ModifySkillBonus(uint32 skillid, int32 val, bool talent);

        /*********************************************************/
        /***                  PVP SYSTEM                       ***/
        /*********************************************************/
        // @TODO: Properly implement correncies as of Cataclysm
        void UpdateHonorFields();
        uint32 RewardHonor(Unit* victim, uint32 groupsize, int32 honor = -1, bool pvptoken = false, MS::Battlegrounds::RewardCurrencyType::Type p_RewardCurrencyType = MS::Battlegrounds::RewardCurrencyType::Type::None);
        uint32 GetMaxPersonalArenaRatingRequirement(uint32 minarenaslot) const;

        //End of PvP System

        inline SpellCooldowns GetSpellCooldowns() const { return m_spellCooldowns; }

        void SetDrunkValue(uint8 newDrunkValue, uint32 itemId = 0);
        uint8 GetDrunkValue() const { return GetByteValue(PLAYER_FIELD_INEBRIATION, PLAYER_BYTES_3_OFFSET_INEBRIATION); }
        static DrunkenState GetDrunkenstateByValue(uint8 value);

        uint32 GetDeathTimer() const { return m_deathTimer; }
        uint32 GetCorpseReclaimDelay(bool pvp) const;
        void UpdateCorpseReclaimDelay();
        void SendCorpseReclaimDelay(bool load = false);

        uint32 GetBlockPercent() override { return GetUInt32Value(PLAYER_FIELD_SHIELD_BLOCK); }
        bool CanParry() const { return m_canParry; }
        void SetCanParry(bool value);
        bool CanBlock() const { return m_canBlock; }
        void SetCanBlock(bool value);
        bool CanMastery() const { return HasAuraType(SPELL_AURA_MASTERY); }
        bool CanTitanGrip() const { return m_canTitanGrip; }
        void SetCanTitanGrip(bool value) { m_canTitanGrip = value; }
        bool CanTameExoticPets() const { return isGameMaster() || HasAuraType(SPELL_AURA_ALLOW_TAME_PET_TYPE); }

        void SetRegularAttackTime();
        void SetBaseModValue(BaseModGroup modGroup, BaseModType modType, float value) { m_auraBaseMod[modGroup][modType] = value; }
        void HandleBaseModValue(BaseModGroup modGroup, BaseModType modType, float amount, bool apply);
        float GetBaseModValue(BaseModGroup modGroup, BaseModType modType) const;
        float GetTotalBaseModValue(BaseModGroup modGroup) const;
        float GetTotalPercentageModValue(BaseModGroup modGroup) const { return m_auraBaseMod[modGroup][FLAT_MOD] + m_auraBaseMod[modGroup][PCT_MOD]; }
        void _ApplyAllStatBonuses();
        void _RemoveAllStatBonuses();

        void ResetAllPowers();

        void _ApplyWeaponDependentAuraMods(Item* item, WeaponAttackType attackType, bool apply);
        void _ApplyWeaponDependentAuraCritMod(Item* item, WeaponAttackType attackType, AuraEffect const* aura, bool apply);
        void _ApplyWeaponDependentAuraDamageMod(Item* item, WeaponAttackType attackType, AuraEffect const* aura, bool apply);
        void UpdateWeaponDamage(ItemTemplate const* p_Proto, uint8 p_Slot, bool p_Apply, Item* p_Item);

        void _ApplyItemMods(Item* item, uint8 slot, bool apply, bool p_HandlePassives = true);
        void _RemoveAllItemMods();
        void _ApplyAllItemMods();
        void _ApplyItemBonuses(Item const* proto, uint8 slot, bool apply);
        void _ApplyWeaponDamage(uint8 slot, Item const* item, bool apply, uint32 p_Ilevel);
        bool EnchantmentFitsRequirements(uint32 enchantmentcondition, int8 slot);
        void ToggleMetaGemsActive(uint8 exceptslot, bool apply);
        void CorrectMetaGemEnchants(uint8 slot, bool apply);
        void InitDataForForm(bool reapplyMods = false);

        void ApplyItemEquipSpell(Item* item, bool apply, bool form_change = false);
        void ApplyEquipSpell(SpellInfo const* spellInfo, Item* item, bool apply, bool form_change = false);
        void UpdateEquipSpellsAtFormChange();
        void UpdateItemSetAuras(bool formChange = false);

        void CastItemCombatSpell(Unit* target, WeaponAttackType attType, uint32 procVictim, uint32 procEx);
        void CastItemUseSpell(Item* p_Item, SpellCastTargets const& p_Targets, Guid128 p_CastGUID, uint32 p_Misc, uint32 p_Misc2);
        void CastItemCombatSpell(Unit* target, WeaponAttackType attType, uint32 procVictim, uint32 procEx, Item* item, ItemTemplate const* proto);

        void SendEquipmentSetList();
        void SetEquipmentSet(uint32 index, EquipmentSet eqset);
        void DeleteEquipmentSet(uint64 setGuid);

        void SetEmoteState(uint32 anim_id);
        uint32 GetEmoteState() { return m_emote; }

        void SendInitWorldStates(uint32 zone, uint32 area);
        void SendUpdateWorldState(uint32 Field, uint32 Value);
        void SendDirectMessage(const WorldPacket* data);
        void SendBGWeekendWorldStates();
        void SetWorldState(uint32 p_ID, uint32 p_Value)
        {
            m_WorldStates.Remove(p_ID);
            m_WorldStates.Insert(p_ID, p_Value);
        }
        uint32 GetWorldState(uint32 p_ID) const
        {
            return m_WorldStates.Find(p_ID);
        }

        void SendAurasForTarget(Unit* target);
        void SendSpellHistory();

        PlayerMenu* PlayerTalkClass;
        std::vector<ItemSetEffect*> ItemSetEff;

        void SendLoot(uint64 guid, LootType loot_type, bool fetchLoot = false);
        void SendLootRelease(uint64 p_LootGuid);
        void SendNotifyLootItemRemoved(uint8 lootSlot);
        void SendNotifyLootMoneyRemoved();

        bool IsMaxLevel() const { return getLevel() == MAX_LEVEL; }
        /*********************************************************/
        /***               BATTLEGROUND SYSTEM                 ***/
        /*********************************************************/

        bool InBattleground()       const                { return m_bgData.bgInstanceID != 0; }
        bool InArena()              const;
        bool InRatedBattleGround()  const;
        uint32 GetBattlegroundId()  const                { return m_bgData.bgInstanceID; }
        BattlegroundTypeId GetBattlegroundTypeId() const { return m_bgData.bgTypeID; }
        Battleground* GetBattleground() const;

        void HandleDeserterAfterLeaveQueue(int32 p_RatingChange = 0, int32 p_MMRChange = 0, uint8 p_Slot = 0, bool p_Deserter = true);
        void ResetLeaveQueueCounter() { SetCharacterWorldState(CharacterWorldStates::LeavePvPQueueCounter, 0); }
        uint64 GetLeaveQueueCounter() const { return GetCharacterWorldStateValue(CharacterWorldStates::LeavePvPQueueCounter); }
        void SetLeaveQueueCounter(uint64 p_Value) { SetCharacterWorldState(CharacterWorldStates::LeavePvPQueueCounter, p_Value); }

        uint32 GetBattlegroundQueueJoinTime(MS::Battlegrounds::BattlegroundType::Type bgTypeId) const
        {
            auto itr = m_bgData.bgQueuesJoinedTime.find(bgTypeId);
            return itr != m_bgData.bgQueuesJoinedTime.end() ? itr->second : uint32(time(nullptr));
        }

        void ChangeBattlegroundQueueJoinTimeKey(MS::Battlegrounds::BattlegroundType::Type p_BgTypeId, MS::Battlegrounds::BattlegroundType::Type p_OldBgTypeId)
        {
            auto l_Itr = m_bgData.bgQueuesJoinedTime.find(p_OldBgTypeId);
            if (l_Itr != std::end(m_bgData.bgQueuesJoinedTime))
            {
                m_bgData.bgQueuesJoinedTime.insert(std::make_pair(p_BgTypeId, l_Itr->second));
                m_bgData.bgQueuesJoinedTime.erase(l_Itr);
            }
        }

        void AddBattlegroundQueueJoinTime(MS::Battlegrounds::BattlegroundType::Type bgTypeId, uint32 joinTime)
        {
            m_bgData.bgQueuesJoinedTime[bgTypeId] = joinTime;
        }

        void RemoveBattlegroundQueueJoinTime(MS::Battlegrounds::BattlegroundType::Type bgTypeId)
        {
            auto l_Itr = m_bgData.bgQueuesJoinedTime.find(bgTypeId);
            if (l_Itr != m_bgData.bgQueuesJoinedTime.end())
                m_bgData.bgQueuesJoinedTime.erase(l_Itr);
        }

        /// Returns true if the player is in a battleground queue.
        bool InBattlegroundQueue() const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].BgType != MS::Battlegrounds::BattlegroundType::None)
                    return true;
            return false;
        }

        /// Returns true if the player is in an arena queue.
        bool InArenaQueue() const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].BgType >= MS::Battlegrounds::BattlegroundType::Arena1v1 && m_bgBattlegroundQueueID[i].BgType <= MS::Battlegrounds::BattlegroundType::ArenaSkirmish3v3)
                    return true;
            return false;
        }

        void SetBattlegroundQueueTypeId(uint32 p_Index, MS::Battlegrounds::BattlegroundType::Type p_Type) { m_bgBattlegroundQueueID[p_Index].BgType = p_Type; }
        MS::Battlegrounds::BattlegroundType::Type GetBattlegroundQueueTypeId(uint32 index) const { return m_bgBattlegroundQueueID[index].BgType; }

        uint32 GetBattlegroundQueueIndex(MS::Battlegrounds::BattlegroundType::Type bgQueueTypeId) const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].BgType == bgQueueTypeId)
                    return i;
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }

        bool IsInvitedForBattlegroundQueueType(MS::Battlegrounds::BattlegroundType::Type bgQueueTypeId) const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].BgType == bgQueueTypeId)
                    return m_bgBattlegroundQueueID[i].invitedToInstance != 0;
            return false;
        }

        bool InBattlegroundQueueForBattlegroundQueueType(MS::Battlegrounds::BattlegroundType::Type bgQueueTypeId) const
        {
            return GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES;
        }

        void SetBattlegroundId(uint32 val, BattlegroundTypeId bgTypeId)
        {
            m_bgData.bgInstanceID = val;
            m_bgData.bgTypeID = bgTypeId;
        }

        uint32 AddBattlegroundQueueId(MS::Battlegrounds::BattlegroundType::Type val)
        {
            for (uint8 i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattlegroundQueueID[i].BgType == MS::Battlegrounds::BattlegroundType::None || m_bgBattlegroundQueueID[i].BgType == val)
                {
                    m_bgBattlegroundQueueID[i].BgType = val;
                    m_bgBattlegroundQueueID[i].invitedToInstance = 0;
                    return i;
                }
            }
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }

        bool HasFreeBattlegroundQueueId()
        {
            for (uint8 i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].BgType == MS::Battlegrounds::BattlegroundType::None)
                    return true;
            return false;
        }

        void RemoveBattlegroundQueueId(MS::Battlegrounds::BattlegroundType::Type val)
        {
            for (uint8 i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattlegroundQueueID[i].BgType == val)
                {
                    m_bgBattlegroundQueueID[i].BgType = MS::Battlegrounds::BattlegroundType::None;
                    m_bgBattlegroundQueueID[i].invitedToInstance = 0;
#ifdef CROSS
                    if (InterRealmClient* l_Client = GetSession()->GetInterRealmClient())
                        l_Client->SendInvitedForBattlegroundQueueType(GetGUID(), i, static_cast<uint32>(MS::Battlegrounds::BattlegroundType::None));
#endif
                    return;
                }
            }
        }

        void SetInviteForBattlegroundQueueType(MS::Battlegrounds::BattlegroundType::Type bgQueueTypeId, uint32 instanceId)
        {
#ifdef CROSS
            uint32 l_QueueID = 0;
#endif
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattlegroundQueueID[i].BgType == bgQueueTypeId)
                {
                    m_bgBattlegroundQueueID[i].invitedToInstance = instanceId;
#ifdef CROSS
                    l_QueueID = i;
#endif
                }
            }

#ifdef CROSS
            if (InterRealmClient* l_Client = GetSession()->GetInterRealmClient())
                l_Client->SendInvitedForBattlegroundQueueType(GetGUID(), l_QueueID, static_cast<uint32>(bgQueueTypeId));
#endif
        }

        bool IsInvitedForBattlegroundInstance(uint32 instanceId) const
        {
            for (uint8 i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].invitedToInstance == instanceId)
                    return true;

            return false;
        }

        void SetCrossInvitationForBG(uint32 p_QueueID, uint32 p_BGType)
        {
            m_CrossInvitationForBG[p_QueueID] = p_BGType;
        }

        uint32 GetCrossInvitationForBG(uint32 p_QueueID) const
        {
            std::map<uint32, uint32>::const_iterator l_Itr = m_CrossInvitationForBG.find(p_QueueID);
            if (l_Itr != m_CrossInvitationForBG.end())
                return l_Itr->second;

            return static_cast<uint32>(MS::Battlegrounds::BattlegroundType::None);
        }

        bool IsInvitedForArenaSkirmish2v2() const
        {
            for (uint32 l_I = 0; l_I < PLAYER_MAX_BATTLEGROUND_QUEUES; ++l_I)
            {
                if (((m_bgBattlegroundQueueID[l_I].BgType >= MS::Battlegrounds::BattlegroundType::FirstRandomArena)
                    && (m_bgBattlegroundQueueID[l_I].BgType <= MS::Battlegrounds::BattlegroundType::LastRandomArena)
                    && (m_bgBattlegroundQueueID[l_I].invitedToInstance != 0))
                    || ((GetCrossInvitationForBG(l_I) >= MS::Battlegrounds::BattlegroundType::FirstRandomArena)
                    && (GetCrossInvitationForBG(l_I) < MS::Battlegrounds::BattlegroundType::LastRandomArena)))
                    return true;
            }

            return false;
        }

        void SetBattlegroundEntryPoint(WorldLocation const& p_WorldLocation) { m_bgData.joinPos = p_WorldLocation; }
        WorldLocation const& GetBattlegroundEntryPoint() const { return m_bgData.joinPos; }
        void SetBattlegroundEntryPoint();

        void SetInRandomBG(bool p_Value) { m_bgData.IsRandom = p_Value; }
        bool IsInRandomBG() const { return m_bgData.IsRandom; }
        void SetBGTeam(uint32 team) { m_bgData.bgTeam = team; SetByteValue(PLAYER_FIELD_ARENA_FACTION, PLAYER_BYTES_4_OFFSET_ARENA_FACTION, uint8(team == ALLIANCE ? 1 : 0)); }

        /// Return faction team (see enum @Team)
        uint32 GetBGTeam() const { return m_bgData.bgTeam ? m_bgData.bgTeam : GetTeam(); }
#ifdef CROSS
        uint32 GetReconnectBGTeam() const { return m_bgData.m_ReconnectBgTeam; }
#endif /* CROSS */

        uint8 GetBGLastActiveSpec() const { return m_bgData.m_LastActiveSpec; }
        void SaveBGLastSpecialization() { m_bgData.m_LastActiveSpec = GetActiveTalentGroup(); }

        void LeaveBattleground(bool teleportToEntryPoint = true);
        bool CanJoinToBattleground() const;
        bool CanReportAfkDueToLimit();
        void ReportedAfkBy(Player* reporter);
        void ClearAfkReports() { m_bgData.bgAfkReporter.clear(); }

        bool GetBGAccessByLevel(BattlegroundTypeId bgTypeId) const;
        bool isTotalImmunity();
        bool CanUseBattlegroundObject();
        bool isTotalImmune();
        bool CanCaptureTowerPoint();

        bool GetRandomWinner() const { return 0; }
        void SetRandomWinner(bool p_IsWinner, bool p_DatabaseUpdate = true);

        /*********************************************************/
        /***               OUTDOOR PVP SYSTEM                  ***/
        /*********************************************************/

        OutdoorPvP* GetOutdoorPvP() const;
        // returns true if the player is in active state for outdoor pvp objective capturing, false otherwise
        bool IsOutdoorPvPActive();

        /*********************************************************/
        /***              ENVIROMENTAL SYSTEM                  ***/
        /*********************************************************/

        bool IsImmuneToEnvironmentalDamage();
        uint32 EnvironmentalDamage(EnviromentalDamage type, uint32 damage);

        /*********************************************************/
        /***               FLOOD FILTER SYSTEM                 ***/
        /*********************************************************/

        void UpdateSpeakTime();
        bool UpdatePmChatTime();
        bool CanSpeak() const;
        void ChangeSpeakTime(int utime);

        /*********************************************************/
        /***                 VARIOUS SYSTEMS                   ***/
        /*********************************************************/
        void UpdateFallInformationIfNeed(MovementInfo const& minfo, uint16 opcode);
        Unit* m_mover;
        WorldObject* m_seer;
        void SetFallInformation(uint32 time, float z)
        {
            m_lastFallTime = time;
            m_lastFallZ = z;
        }
        void HandleFall(MovementInfo const& movementInfo);

        bool IsKnowHowFlyIn(uint32 mapid, uint32 zone, uint32 spellId = 0) const;

        void SetClientControl(Unit* target, uint8 allowMove);

        void SetMover(Unit* target);

        void SendApplyMovementForce(uint64 p_Source, bool p_Apply, Position p_Direction = {}, float p_Magnitude = 0.0f, uint8 p_Type = 0, G3D::Vector3 p_TransportPos = G3D::Vector3(0.0f, 0.0f, 0.0f));
        void RemoveAllMovementForces(uint32 p_Entry = 0);
        bool HasMovementForce(uint64 p_Source = 0, bool p_IsEntry = false);

        void SetSeer(WorldObject* target) { m_seer = target; }
        void SetViewpoint(WorldObject* target, bool apply);
        WorldObject* GetViewpoint() const;
        void StopCastingCharm();
        void StopCastingBindSight();

        std::pair<uint32, uint32> GetObliterumAshesAmount(Item* p_Item);

        void SendPetTameResult(PetTameResult result);

        uint32 GetSaveTimer() const { return m_nextSave; }
        void   SetSaveTimer(uint32 timer) { m_nextSave = timer; }

        // Recall position
        uint32 m_recallMap;
        float  m_recallX;
        float  m_recallY;
        float  m_recallZ;
        float  m_recallO;
        void   SaveRecallPosition();

        void SetHomebind(WorldLocation const& loc, uint32 area_id);

        // Homebind coordinates
        uint32 m_homebindMapId;
        uint16 m_homebindAreaId;
        float m_homebindX;
        float m_homebindY;
        float m_homebindZ;

        WorldLocation GetStartPosition() const;

        WorldLocation* GetPreviousLocation();

        WorldLocation m_PreviousLocation;

        // current pet slot
        PetSlot m_currentPetSlot;
        uint64 m_petSlotUsed;

        void setPetSlotUsed(PetSlot slot, bool used)
        {
            if (used)
                m_petSlotUsed |= ((int64)1 << int64(slot));
            else
                m_petSlotUsed &= ~((int64)1 << int64(slot));
        }

        PetSlot getSlotForNewPet()
        {
            uint32 l_LastKnow = 0;
            // Call Pet Spells
            // 883 83242 83243 83244 83245
            //  1    2     3     4     5
            if (HasSpell(83245))
                l_LastKnow = 5;
            else if (HasSpell(83244))
                l_LastKnow = 4;
            else if (HasSpell(83243))
                l_LastKnow = 3;
            else if (HasSpell(83242))
                l_LastKnow = 2;
            else if (HasSpell(883))
                l_LastKnow = 1;

            for (uint32 l_I = uint32(PET_SLOT_HUNTER_FIRST); l_I < l_LastKnow; ++l_I)
            {
                if ((m_petSlotUsed & uint64(1LL << l_I)) == 0)
                    return PetSlot(l_I);
            }

            return PET_SLOT_FULL_LIST;
        }

        // currently visible objects at player client
        GuidUnorderedSet m_clientGUIDs;
        mutable ACE_RW_Thread_Mutex m_clientGUIDs_lock;

        bool HaveAtClient(WorldObject const* u) const
        {
            if (u == this)
                return true;

            bool l_Result;

            m_clientGUIDs_lock.acquire_read();
            l_Result = m_clientGUIDs.find(u->GetGUID()) != m_clientGUIDs.end();
            m_clientGUIDs_lock.release();

            return l_Result;
        }

        bool IsNeverVisible() const override;

        bool IsVisibleGloballyFor(Player* player) const;

        void SendInitialVisiblePackets(Unit* target);
        void UpdateVisibilityForPlayer(bool p_CreateGrids);
        void UpdateVisibilityOf(WorldObject* target);
        void UpdateTriggerVisibility();

        template<class T>
        void UpdateVisibilityOf(T* target, UpdateData& data, std::set<Unit*>& visibleNow);

        uint8 m_forced_speed_changes[MAX_MOVE_TYPE];

        uint32 GetAtLoginFlags() const { return m_atLoginFlags; }
        bool HasAtLoginFlag(AtLoginFlags f) const { return m_atLoginFlags & f; }
        void SetAtLoginFlag(AtLoginFlags f) { m_atLoginFlags |= f; }
        void RemoveAtLoginFlag(AtLoginFlags flags, bool persist = false);

        bool isUsingLfg();

        typedef std::set<uint32> DFQuestsDoneList;
        DFQuestsDoneList m_DFQuests;

        // Temporarily removed pet cache
        uint32 GetTemporaryUnsummonedPetNumber() const { return m_temporaryUnsummonedPetNumber; }
        void SetTemporaryUnsummonedPetNumber(uint32 petnumber) { m_temporaryUnsummonedPetNumber = petnumber; }
        void UnsummonPetTemporaryIfAny();
        void ResummonPetTemporaryUnSummonedIfAny();
        bool IsPetNeedBeTemporaryUnsummoned() const { return !IsInWorld() || !isAlive(); }

        //////////////////////////////////////////////////////////////////////////
        /// Cinematic
        //////////////////////////////////////////////////////////////////////////
        void StopCinematic();
        void SendCinematicStart(uint32 CinematicSequenceId);
        bool IsInCinematic() { return m_InCinematic && m_CinematicSequence; }

        void SendMovieStart(uint32 MovieId);

        bool CanMountAsPassenger(Player* l_DriverPlayer) const;

        std::pair<bool, bool> IsFightingWithDungeonBoss();

        /*********************************************************/
        /***                 INSTANCE SYSTEM                   ***/
        /*********************************************************/

        typedef ACE_Based::RecursiveLockedMap< uint32 /*mapId*/, InstancePlayerBindPtr> BoundInstancesMap;

        void UpdateHomebindTime(uint32 time);

        uint32 m_HomebindTimer;
        bool m_InstanceValid;
        // permanent binds and solo binds by difficulty
        BoundInstancesMap m_boundInstances[Difficulty::MaxDifficulties];
        InstancePlayerBindPtr GetBoundInstance(uint32 mapid, Difficulty difficulty, bool withExpired = false);
        BoundInstancesMap& GetBoundInstances(Difficulty difficulty) { return m_boundInstances[difficulty]; }
        InstanceSavePtr GetInstanceSave(uint32 mapid);
        void UnbindInstance(uint32 mapid, Difficulty difficulty, bool unload = false);
        void UnbindInstance(BoundInstancesMap::iterator &itr, Difficulty difficulty, bool unload = false);
        InstancePlayerBindPtr BindToInstance(InstanceSavePtr save, bool permanent, BindExtensionState extendState, bool load = false);
        void BindToInstance();
        void SetPendingBind(uint32 instanceId, uint32 bindTimer) { _pendingBindId = instanceId; _pendingBindTimer = bindTimer; }
        void SendPendingRaidLock(uint32 p_Timer, uint32 p_CompletedMask = 0, bool p_Extends = false, bool p_WarningOnly = false);
        bool HasPendingBind() const { return _pendingBindId > 0; }
        void SendRaidInfo();
        void SendSavedInstances();
        static void ConvertInstancesToGroup(Player* player, GroupPtr group, bool switchLeader);
        bool Satisfy(AccessRequirement const* ar, uint32 target_map, bool report = false);
        bool CheckInstanceLoginValid();
        bool CheckInstanceCount(uint32 instanceId) const
        {
            if (_instanceResetTimes.size() < sWorld->getIntConfig(CONFIG_MAX_INSTANCES_PER_HOUR))
                return true;
            return _instanceResetTimes.find(instanceId) != _instanceResetTimes.end();
        }

        void AddInstanceEnterTime(uint32 instanceId, time_t enterTime)
        {
            if (_instanceResetTimes.find(instanceId) == _instanceResetTimes.end())
                _instanceResetTimes.insert(InstanceTimeMap::value_type(instanceId, enterTime + HOUR));
        }

        // last used pet number (for BG's)
        uint32 GetLastPetNumber() const { return m_lastpetnumber; }
        void SetLastPetNumber(uint32 petnumber) { m_lastpetnumber = petnumber; }

        /*********************************************************/
        /***                   GROUP SYSTEM                    ***/
        /*********************************************************/

        uint32 GetGroupInvite() { return m_groupInviteGUID; }
        void SetGroupInvite(uint32 groupGUID) { m_groupInviteGUID = groupGUID; }
        GroupPtr GetGroup() { return m_Group; }
        const GroupPtr GetGroup() const { return (const GroupPtr)m_Group; }
        void SetGroup(GroupPtr group, int8 subgroup = -1);
        int8 GetSubGroup() const { return m_SubGroup; }
        void SetSubGroup(int8 p_SubGroup) { m_SubGroup = p_SubGroup; }
        uint32 GetGroupUpdateFlag() const { return m_groupUpdateMask; }
        void SetGroupUpdateFlag(uint32 flag) { m_groupUpdateMask |= flag; }
        std::bitset<MAX_AURAS> const& GetAuraUpdateMaskForRaid() const { return m_auraRaidUpdateMask; }
        void SetAuraUpdateMaskForRaid(uint8 p_Slot, bool p_Apply) { m_auraRaidUpdateMask.set(p_Slot, p_Apply); }
        Player* GetNextRandomRaidMember(float radius);
        PartyResult CanUninviteFromGroup() const;

        /// Returns List of group member, INCLUDING player that called the function.
        void GetSortedGroupMembers(std::list<Player*>& p_PlayerList, std::function<bool(Player*)> p_Check, uint32 p_Count, std::list<std::function<bool(Player*)>> p_Orders) const;

        // Battleground / Battlefield Group System
        void SetBattlegroundOrBattlefieldRaid(GroupPtr group, int8 subgroup = -1);
        void RemoveFromBattlegroundOrBattlefieldRaid();
        GroupPtr GetOriginalGroup() { return m_OriginalGroup; }
        int8 GetOriginalSubGroup() const { return m_OriginalSubGroup; }
        void SetOriginalSubGroup(int8 p_SubGroup) { m_SubGroup = p_SubGroup; }
        void SetOriginalGroup(GroupPtr group, int8 subgroup = -1);

        void SetPassOnGroupLoot(bool bPassOnGroupLoot) { m_bPassOnGroupLoot = bPassOnGroupLoot; }
        bool GetPassOnGroupLoot() const { return m_bPassOnGroupLoot; }

        MapReference &GetMapRef() { return m_mapRef; }

        // Set map to player and add reference
        void SetMap(Map* map) override;
        void ResetMap() override;
        void ResetMapForTeleport();
        void SetOldMap(Map* p_Map) { m_OldMap = p_Map; }
        Map* GetOldMap() const { return m_OldMap; }

        bool isAllowedToLoot(const Creature* creature);

        DeclinedName const* GetDeclinedNames() const { return m_declinedname; }

        uint8 GetRunesState() const { return m_runes->RuneState; }
        uint32 GetRuneCooldown(uint8 index) const { return m_runes->Cooldown[index]; }
        uint32 GetRuneBaseCooldown() const;
        std::deque<uint8> GetRuneCooldownsOrder() { return m_runes->CooldownOrder; }
        void SetRuneCooldown(uint8 index, uint32 cooldown, bool casted = false);
        void InitRunes();
        uint32 GetRuneTimer(uint8 index) const { return m_runeGraceCooldown[index]; }
        void SetRuneTimer(uint8 index, uint32 timer) { m_runeGraceCooldown[index] = timer; }
        void SetLastRuneGraceTimer(uint8 index, uint32 timer) { m_lastRuneGraceTimers[index] = timer; }
        void RestoreRune();
        void ResyncRunes(uint8 p_Count, uint8 p_OldState);

        //////////////////////////////////////////////////////////////////////////
        /// Achievement system
        //////////////////////////////////////////////////////////////////////////
        #pragma region Achivement
        void SendRespondInspectAchievements(Player* player) const;
        uint32 GetAchievementPoints() const;
        bool HasAchieved(uint32 achievementId) const;
        uint32 GetAchievementProgress(uint32 p_AchievementID) const;
        bool HasModifierTreeSatisfied(uint32 p_ModifierTreeID) const;
        void ResetAchievements();
        void ResetCriteria(CriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, bool evenIfCriteriaComplete = false);
        void UpdateCriteria(CriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, uint64 miscValue3 = 0, Unit* unit = NULL);
        void StartCriteriaTimer(CriteriaTimedTypes type, uint32 entry, uint32 timeLost = 0);
        void RemoveCriteriaTimer(CriteriaTimedTypes type, uint32 entry);
        void CompletedAchievement(AchievementEntry const* entry);
        bool ModifierTreeSatisfied(uint32 modifierTreeId) const;
        PlayerAchievementMgr const* GetAchievementMgr() const { return m_achievementMgr; }
        PlayerAchievementMgr * GetAchievementMgr() { return m_achievementMgr; }

        void ResetCriteriaTree(CriteriaTree const* p_Tree);
        #pragma endregion

        bool HasTitle(uint32 bitIndex) const;
        bool HasTitle(CharTitlesEntry const* title) { return HasTitle(title->MaskID); }
        void SetTitle(CharTitlesEntry const* title, bool lost = false);

        //bool isActiveObject() const { return true; }
        bool canSeeSpellClickOn(Creature const* creature) const;

        uint32 GetChampioningFaction() const { return m_ChampioningFaction; }
        void SetChampioningFaction(uint32 faction) { m_ChampioningFaction = faction; }
        Spell* m_spellModTakingSpell;

        uint32 GetAverageItemLevelEquipped() const;
        uint32 GetAverageItemLevelTotal() const;
        uint32 GetAverageItemLevelTotalWithOrWithoutPvPBonus(bool p_PvP) const;
        bool isDebugAreaTriggers;
        bool m_IsDebugQuestLogs;

        void ClearWhisperWhiteList() { WhisperList.clear(); }
        void AddWhisperWhiteList(uint64 guid) { WhisperList.push_back(guid); }
        bool IsInWhisperWhiteList(uint64 guid);

        /*! These methods send different packets to the client in apply and unapply case.
            These methods are only sent to the current unit.
        */
        void SendMovementSetCanTransitionBetweenSwimAndFly(bool apply);
        void SendMovementSetCollisionHeight(float height);
        void SendMovementSetCanDoubleJump(bool p_Apply);

        bool CanFly() const override { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_CAN_FLY); }

        //! Return collision height sent to client
        float GetCollisionHeight(bool p_Mounted);

        uint16 GetPrimaryProfession(uint8 index) const
        {
            //ASSERT(index < DEFAULT_MAX_PRIMARY_TRADE_SKILL);
            return uint16(GetUInt32Value(PLAYER_FIELD_PROFESSION_SKILL_LINE + index));
        }

        void SetPrimaryProfession(uint8 index, uint16 skillId)
        {
            //ASSERT(index < DEFAULT_MAX_PRIMARY_TRADE_SKILL);
            SetUInt32Value(PLAYER_FIELD_PROFESSION_SKILL_LINE + index, skillId);
        }

        // Void Storage
        bool IsVoidStorageUnlocked() const { return HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_VOID_UNLOCKED); }
        void UnlockVoidStorage() { SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_VOID_UNLOCKED); }
        void LockVoidStorage() { RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_VOID_UNLOCKED); }
        uint8 GetNextVoidStorageFreeSlot() const;
        uint8 GetNumOfVoidStorageFreeSlots() const;
        uint8 AddVoidStorageItem(const VoidStorageItem& item);
        void AddVoidStorageItemAtSlot(uint8 slot, const VoidStorageItem& item);
        void DeleteVoidStorageItem(uint8 slot);
        bool SwapVoidStorageItem(uint8 oldSlot, uint8 newSlot);
        VoidStorageItem* GetVoidStorageItem(uint8 slot) const;
        VoidStorageItem* GetVoidStorageItem(uint64 id, uint8& slot) const;
        uint32 GetVoidStorageItemsCount(uint32 entry) const;

        uint32 GetLastTargetedGO() { return _lastTargetedGO; }
        void SetLastTargetedGO(uint32 lastTargetedGO) { _lastTargetedGO = lastTargetedGO; }
        void ShowNeutralPlayerFactionSelectUI();

        float GetPersonnalXpRate()
        {
            if (sWorld->getBoolConfig(CONFIG_ENABLE_MAX_XP_BEFORE_WOD) && m_PersonnalXpRate > sWorld->getRate(RATE_MAX_XP_BEFORE_WOD)
                && GetMap() && GetMap()->GetEntry()->Expansion() < Expansion::EXPANSION_WARLORDS_OF_DRAENOR)
                return sWorld->getRate(RATE_MAX_XP_BEFORE_WOD);

            return std::min(m_PersonnalXpRate, sWorld->getRate(RATE_MAX_XP_GLOBAL));
        }

        void SetPersonnalXpRate(float PersonnalXpRate);

        void SetKnockBackTime(uint32 timer) { m_knockBackTimer = timer; }
        uint32 GetKnockBackTime() const { return m_knockBackTimer; }

        void SetLastPlayedEmote(uint32 id) { m_lastPlayedEmote = id; }
        uint32 GetLastPlayedEmote() { return m_lastPlayedEmote; }

        void SetIgnoreMovementCount(uint8 count) { m_ignoreMovementCount = count; }
        uint8 GetIgnoreMovementCount() const { return m_ignoreMovementCount; }

        void HandleStoreItemCallback(PreparedQueryResult result);
        void HandleStoreLevelCallback(PreparedQueryResult result);
        void HandleStoreGoldCallback(PreparedQueryResult result);
        void HandleStoreTitleCallback(PreparedQueryResult result);
        void HandleStoreProfessionCallback(PreparedQueryResult p_Result);

        void CheckSpellAreaOnQuestStatusChange(uint32 quest_id);

        void SendCUFProfiles();

        void SendResumeToken(uint32 token);
        void SendRefreshSpellMods();

        uint8 GetBattleGroundRoles() const { return m_bgRoles; }
        void SetBattleGroundRoles(uint8 roles) { m_bgRoles = roles; }

        /*********************************************************/
        /***                  SCENES SYSTEM                    ***/
        /*********************************************************/
        void PlayScene(uint32 sceneId, WorldObject* spectator);
        /// Play standalone scene script on client side
        /// @p_ScenePackageID : Scene package ID @ScenePackage.db2
        /// @p_PlaybackFlags  : Playback flags (@TODO make some reverse on it)
        /// @p_Location       : Scene script start location
        /// Return generated Scene instance ID
        uint32 PlayStandaloneScene(uint32 p_ScenePackageID, uint32 p_PlaybackFlags, Position p_Location);
        /// Cancel a client-side played standalone scene
        /// @p_SceneInstanceID : Scene instance ID
        void CancelStandaloneScene(uint32 p_SceneInstanceID);

        /// Start a spell scene
        /// @p_SpellID      : Spell ID
        /// @p_MiscValue    : Effect misc value
        /// @p_Apply        : Start or cancel scene
        /// @p_Location     : Scene play offset
        void SendSpellScene(uint32 p_SpellID, uint32 p_MiscValue, bool p_Apply, Position* p_Location);
        /// When the scene is complete
        /// @p_InstanceID : Scene instance ID
        void SceneCompleted(uint32 p_InstanceID);
        /// Trigger event in scene
        /// @p_InstanceID   : Scene instance ID
        /// @p_Type         : Trigger type
        void TrigerScene(uint32 p_InstanceID, std::string const& p_Type);
        /// Attempt to get isntanceID from a running scene for player
        /// @p_ScenePackageID : Scene package ID @ScenePackage.db2
        uint32 GetSceneInstanceIDByPackage(uint32 p_ScenePackageID) const;
        /// Attempt to get MiscID from a running scene for player
        /// @p_SceneInstanceID : Scene instance ID
        uint32 GetMiscIDBySceneInstanceID(uint32 p_SceneInstanceID) const;
        /// Attempt to spell ID from a running scene on player
        /// @p_InstanceID : Instance Scene ID
        uint32 GetAuraIDByScenceInstance(uint32 p_SceneInstanceID);
        ///Remove spell ID from a running scene on player
        /// @p_InstanceID : Instance Scene ID
        void RemoveAuraIDBySceneInstance(uint32 p_SceneInstanceID);
        /// Has battle pet training
        bool HasBattlePetTraining();
        /// Get battle pet trap level
        uint32 GetBattlePetTrapLevel();
        /// Compute the unlocked pet battle slot
        uint32 GetUnlockedPetBattleSlot();
        /// Summon current pet if any active
        void UnsummonCurrentBattlePetIfAny(bool p_Unvolontary);
        /// Summon new pet
        void SummonBattlePet(uint64 p_JournalID);
        /// Get current summoned battle pet
        Creature* GetSummonedBattlePet();
        /// Summon last summoned battle pet
        void SummonLastSummonedBattlePet();
        /// Summon Active Bodyguard
        void RecastCombatAllySpellIfNeeded();

        /// Get pet battles
        std::vector<std::shared_ptr<BattlePet>> GetBattlePets();
        /// Get pet battles
        std::shared_ptr<BattlePet> GetBattlePet(uint64 p_JournalID);
        /// Get pet battle combat team
        std::shared_ptr<BattlePet> * GetBattlePetCombatTeam();
        /// Get pet battle combat team size
        uint32 GetBattlePetCombatSize();

        /// Reload pet battles
        void ReloadPetBattles();
        /// PetBattleCountBattleSpeciesCallback
        void PetBattleCountBattleSpecies();
        /// Update battle pet combat team
        void UpdateBattlePetCombatTeam();

        //////////////////////////////////////////////////////////////////////////
        /// ToyBox
        void _LoadToyBox(PreparedQueryResult p_Result);
        void SendToyBox();
        void AddNewToyToBox(uint32 p_ItemID);
        void SetFavoriteToy(bool p_Apply, uint32 p_ItemID);

        PlayerToy* GetToy(uint32 p_ItemID)
        {
            if (m_PlayerToys.find(p_ItemID) != m_PlayerToys.end())
                return &m_PlayerToys[p_ItemID];

            return nullptr;
        }

        bool HasToy(uint32 p_ItemID) const
        {
            if (m_PlayerToys.find(p_ItemID) != m_PlayerToys.end())
                return true;

            return false;
        }
        //////////////////////////////////////////////////////////////////////////

        void RescaleAllItemsIfNeeded();
        bool UpdateItemLevelCutOff(uint32 p_StartsWith, uint32 p_MinLevel, uint32 p_MaxLevel, bool p_RescaleItems = true);
        void CutOffItemLevel(bool p_RescaleItems = true);

        void SetInPvPCombat(bool set);
        bool IsInPvPCombat() const { return m_pvpCombat; }
        void UpdatePvP(uint32 diff);
        void SetPvPTimer(uint32 duration) { m_PvPCombatTimer = duration; }

        void UnForcePvPArea()
        {
            pvpInfo.inNoPvPArea = true;
        }

        void ForcePvPArea()
        {
            pvpInfo.inNoPvPArea = false;
        }

        void ForceFFA()
        {
            pvpInfo.forceFFA     = true;
            pvpInfo.inFFAPvPArea = true;
        }

        void DisableForceFFA()
        {
            pvpInfo.forceFFA     = false;
            pvpInfo.inFFAPvPArea = false;
        }

        uint32 GetQuestObjectiveCounter(uint32 objectiveId) const;
        void SetQuestObjectiveCounter(uint32 p_ObjectId,  uint32 p_Amount);

        //////////////////////////////////////////////////////////////////////////
        /// SpellCharges
        ChargeStorageType m_CategoryCharges;

        ChargeStorageType& GetChargeCooldowns() { return m_CategoryCharges; }

        void SendSpellCharges();
        void SendSpellCharge(SpellCategoryEntry const* p_ChargeCategoryEntry);
        void SendSetSpellCharges(SpellCategoryEntry const* p_ChargeCategoryEntry);
        void UpdateCharges();
        void UpdateCharge(SpellCategoryEntry const* p_ChargeCategoryEntry, bool p_Packet = true);
        bool ConsumeCharge(SpellCategoryEntry const* p_ChargeCategoryEntry, uint32 p_SpellID = 0);
        void ConsumeAllCharges(SpellCategoryEntry const* p_ChargeCategoryEntry);
        void ReduceChargeCooldown(SpellCategoryEntry const* p_ChargeCategoryEntry, uint64 p_Reductiontime, bool p_Packet = true);
        void RestoreCharge(SpellCategoryEntry const* p_ChargeCategoryEntry, bool p_KeepCurrCD = false);

        ChargeStorageType::iterator ResetCharges(SpellCategoryEntry const* p_ChargeCategoryEntry)
        {
            if (!p_ChargeCategoryEntry)
                return m_CategoryCharges.begin();

            auto l_Itr = m_CategoryCharges.find(p_ChargeCategoryEntry->Id);
            if (l_Itr != m_CategoryCharges.end())
            {
                WorldPacket l_Data(Opcodes::SMSG_CLEAR_SPELL_CHARGES);
                l_Data << int32(p_ChargeCategoryEntry->Id);
                l_Data.WriteBit(false); ///< IsPet
                l_Data.FlushBits();
                SendDirectMessage(&l_Data);

                return m_CategoryCharges.erase(l_Itr);
            }

            return m_CategoryCharges.begin();
        }

        void ResetAllCharges();
        bool HasCharge(SpellCategoryEntry const* p_ChargeCategoryEntry) const;
        uint32 GetConsumedCharges(SpellCategoryEntry const* p_ChargeCategoryEntry);
        uint32 GetMaxCharges(SpellCategoryEntry const* p_ChargeCategoryEntry) const;
        int32 GetChargeRecoveryTime(SpellCategoryEntry const* p_ChargeCategoryEntry) const;
        void ModSpellCharge(uint32 p_SpellID, int32 p_Num);
        void ModSpellChargeCooldown(uint32 p_SpellID, int32 p_Delta);
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        /// Vignette
        //////////////////////////////////////////////////////////////////////////

        Vignette::Manager& GetVignetteMgr() { return m_VignetteMgr; }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////

        struct MovieDelayedTeleport
        {
            uint32 MovieID;
            uint32 MapID;
            float X, Y, Z, O;
        };

        uint32 CurrentPlayedMovie;
        std::vector<MovieDelayedTeleport> MovieDelayedTeleports;
        std::mutex MovieDelayedTeleportMutex;

        void AddMovieDelayedTeleport(uint32 p_MovieID, uint32 p_MapID, float p_X, float p_Y, float p_Z, float p_O)
        {
            MovieDelayedTeleport l_Data;
            l_Data.MovieID  = p_MovieID;
            l_Data.MapID    = p_MapID;
            l_Data.X        = p_X;
            l_Data.Y        = p_Y;
            l_Data.Z        = p_Z;
            l_Data.O        = p_O;

            MovieDelayedTeleportMutex.lock();
            MovieDelayedTeleports.push_back(l_Data);
            MovieDelayedTeleportMutex.unlock();
        }

        DailyLootsCooldowns m_DailyLootsCooldowns;
        void _LoadDailyLootsCooldowns(PreparedQueryResult&& p_Result);
        void ResetDailyLoots();
        bool CanHaveDailyLootForItem(uint32 p_Entry) const { return m_DailyLootsCooldowns.find(p_Entry) == m_DailyLootsCooldowns.end(); }
        void AddDailyLootCooldown(uint32 p_Entry);

        void _GarrisonSetIn();
        void _GarrisonSetOut();

        void _SetInShipyard();
        void _SetOutOfShipyard();

        bool AddHeirloom(HeirloomEntry const* p_HeirloomEntry, uint8 p_UpgradeLevel = 0, bool p_UseShopGroupRealmMask = false);
        bool HasHeirloom(uint32 p_ItemID) const;
        bool HasHeirloom(HeirloomEntry const* p_HeirloomEntry) const;
        uint32 GetHeirloomUpgradeLevel(HeirloomEntry const* p_HeirloomEntry) const;
        bool CanUpgradeHeirloomWith(HeirloomEntry const* p_HeirloomEntry, uint32 p_ItemId) const;

        uint64 GetBeaconOfFaithTarget() const { return m_BeaconOfFaithTargetGUID; }
        void SetBeaconOfFaithTarget(uint64 p_NewBeaconOfFaithTarget) { m_BeaconOfFaithTargetGUID = p_NewBeaconOfFaithTarget; }
        uint64 GetBeaconOfLightTarget() const { return m_BeaconOfLightTargetGUID; }
        void SetBeaconOfLightTarget(uint64 p_NewBeaconOfLightTarget) { m_BeaconOfLightTargetGUID = p_NewBeaconOfLightTarget; }
        Creature* GetHati();

        std::map<uint64, uint32> const& GetSpellFromItems() const;

        uint64 GetPrayerOfMendingTarget() const { return m_PrayerOfMendingTargetGUID; }
        void SetPrayerOfMendingTarget(uint64 p_NewPrayerOfMendingTarget) { m_PrayerOfMendingTargetGUID = p_NewPrayerOfMendingTarget; }

        uint32 GetLastCastedSpell() const { return m_LastCastedSpell; }
        uint32 GetCastingSpell() const { return m_CastingSpell; }
        void SetLastCastedSpell(uint32 p_LastCastedSpell);
        void SetCastingSpell(uint32 p_CastingSpell) { m_CastingSpell = p_CastingSpell; }

        void AddCriticalOperation(std::function<void()> const&& p_Function)
        {
            m_CriticalOperationLock.lock();
            m_CriticalOperation.push(std::function<void()>(p_Function));
            m_CriticalOperationLock.unlock();
        }

        void SetQuestBit(uint32 p_BitIndex, bool p_Completed);
        bool IsQuestBitFlaged(uint32 p_BitIndex) const;
        void ClearQuestBits(std::vector<uint32> const& p_QuestBits);

        /// Wargames
        void SetWargameRequest(WargameRequest* p_Request) { m_WargameRequest = p_Request; };
        bool HasWargameRequest() const { return m_WargameRequest != nullptr; }
        WargameRequest* GetWargameRequest() const { return m_WargameRequest; }
        void SetWargameInvite(bool p_IsInvited) { m_WargameInvited = p_IsInvited; }
        bool IsWargameInvited() const { return m_WargameInvited; }

        void ApplyWargameItemModifications();

        bool BossAlreadyLooted(WorldObject* p_Creature, bool p_CheckDiff = true, Difficulty excludingDiff = DifficultyNone) const;
        void AddBossLooted(WorldObject* p_Creature);
        void ResetBossLooted() { m_BossLooted.clear(); }
        BossLooted const GetBossLooted() const { return m_BossLooted; }

        void AddPersonalInstanceLockout(uint32 p_MapId, uint32 p_Difficulty, uint32 p_EncounterMask);
        void ResetPersonalInstanceLockout() { m_PersonInstanceLockout.clear(); }
        PersonalInstanceLockout const& GetPersonalInstanceLockout() const { return m_PersonInstanceLockout; }

        void SetCharacterWorldState(uint32 p_Index, uint64 p_Value)
        {
            CharacterWorldState l_WorldState;
            l_WorldState.Value   = p_Value;
            l_WorldState.Changed = true;

            m_CharacterWorldStates[p_Index] = l_WorldState;
        }

        void ModifyCharacterWorldState(uint32 p_Index, int64 p_Value)
        {
            CharacterWorldState l_WorldState;

            l_WorldState.Value   = GetCharacterWorldStateValue(p_Index) + p_Value;
            l_WorldState.Changed = true;

            m_CharacterWorldStates[p_Index] = l_WorldState;
        }

        uint64 GetCharacterWorldStateValue(uint32 p_Index) const
        {
            if (m_CharacterWorldStates.find(p_Index) == m_CharacterWorldStates.end())
                return 0;

            return m_CharacterWorldStates.at(p_Index).Value;
        }

        bool HasCharacterWorldState(uint32 p_Index) const
        {
            return m_CharacterWorldStates.find(p_Index) != m_CharacterWorldStates.end();
        }

        /// Send custom message with system message (addon, custom interfaces ...etc)
        void SendCustomMessage(std::string const& p_Opcode);
        void SendCustomMessage(std::string const& p_Opcode, std::ostringstream const& p_Data);
        void SendCustomMessage(std::string const& p_Opcode, std::vector<std::string> const& p_Data);

        bool SendPlayerChoice(uint64 p_Sender, int32 p_ChoiceID);

        uint32 GetBagsFreeSlots() const;
        uint32 GetGeneralFreeBagSlots() const;

        bool IsSummoned() const { return m_Summoned; }
        void FinishSummon() { m_Summoned = false; }
        void BeginSummon() { m_Summoned = true; }

        uint32 GetActiveSummonOnDeath() { return m_LastActiveSummon; }
        void SetActiveSummonOnDeath(uint32 p_Entry) { m_LastActiveSummon = p_Entry; }
        void SummonLastSummonedPet();

        MS::Artifact::Manager* GetCurrentlyEquippedArtifact();
        MS::Artifact::Manager const* GetCurrentlyEquippedArtifact() const;
        uint8 GetRankOfArtifactPowerId(uint32 p_ArtifactPowerId) const;

#ifndef CROSS
        JoinedChannelsList const& GetJoinnedChannelList() const { return m_channels; }

        uint32 GetInterRealmZoneId() const { return m_irZoneId; }
        void SetInterRealmZoneId(uint32 val) { m_irZoneId = val; }
        uint32 GetInterRealmAreaId() const { return m_irAreaId; }
        void SetInterRealmAreaId(uint32 val) { m_irAreaId = val; }
        uint32 GetInterRealmMapId() const { return m_irMapId; }
        void SetInterRealmMapId(uint32 val) { m_irMapId = val; }

        InterRealmPlayerState::Type GetInterRealmPlayerState() const { return m_InterRealmPlayerState; }
        void SetInterRealmPlayerState(InterRealmPlayerState::Type p_State)
        {
            m_InterRealmPlayerState = p_State;
            m_InterRealmPlayerStateLastChange = time(nullptr);
        }

#else /* CROSS */
        InterRealmDatabasePool* GetRealmDatabase();
#endif /* CROSS */

        /// Store callback
        bool IsStoreDeliverySaved() const { return m_StoreDeliverySave; }
        bool IsStoreDeliveryProccesed(StoreCallback p_DeliveryType) const { return m_StoreDeliveryProcessed[p_DeliveryType]; }

        void SetStoreDeliverySaved() { m_StoreDeliverySave = true; }
        void SetStoreDeliveryProccesed(StoreCallback p_DeliveryType) { m_StoreDeliveryProcessed[p_DeliveryType] = true; }

        void GiveHonor(uint32 p_Honor);
        uint32 GiveHonorLevel();
        uint32 GetHonorReward(uint8 p_Level, uint8 p_Prestige) const;
        void SetHonorLevel(uint32 p_Level);
        uint32 GetHonorLevel() const;
        uint32 GetHonorRequiredForNextLevel() const;
        uint32 GetHonor() const;
        void UpdateRequiredHonor();
        bool CanPrestige() const;
        void Prestige(bool p_CheckIfCan);
        uint32 GetPrestige() const;
        void GiveRewardPack(uint32 p_Id);

        void ScheduleDelayedOperation(uint32 operation)
        {
            if (operation < DELAYED_END)
                m_DelayedOperations |= operation;
        }

        float GetMasteryCache() const { return m_MasteryCache; }

        MS::Artifact::Manager* GetArtifactHolder(uint32 p_ArtifactID);
        MS::Artifact::Manager const* GetArtifactHolder(uint32 p_ArtifactID) const;
        MS::Artifact::Manager* GetOrGenerateNewArtifactHolder(uint32 p_ArtifactID);

        /// Interaction map type
        using InteractionStatusMap = std::map<uint64, std::queue<InteractionStatus::Type>>;
        /// Interaction callback type
        using InteractionStatusCallback = std::function<bool(Player*, uint64, InteractionStatus::Type, InteractionStatus::Type)>;
        /// Callback map type
        using InteractionStatusCallbackMap = std::map<uint64, std::vector<InteractionStatusCallback>>;

        /// Set current interaction status
        /// NOTE : This will fire all pending callback
        /// @p_GUID   : Interacted GUID
        /// @p_Status : New interaction status
        void SetInteractionStatus(uint64 p_GUID, InteractionStatus::Type p_Status);
        /// Get current interaction status
        InteractionStatusMap GetInteractionStatus();
        /// Register a "fire once" callback for interaction status change
        /// @p_GUID     : Interacted GUID
        /// @p_Callback : Callback method
        void AddInteractionStatusChangeCallback(uint64 p_GUID, const InteractionStatusCallback& p_Callback);

        /// AntiCheat
        void SetSpecialMovementTime() { m_LastSpecialMovementTime = time(nullptr); }
        time_t GetLastSpecialMovementTime() const { return m_LastSpecialMovementTime; }

        void PreventClientDestroy(uint64 p_Guid) { m_PrevenClientDestroyGUIDs.insert(p_Guid); }
        void RemovePreventClientDestroy(uint64 p_Guid) { m_PrevenClientDestroyGUIDs.erase(p_Guid); }
        bool PreventClientDestroyFor(uint64 p_Guid) const
        {
            return m_PrevenClientDestroyGUIDs.find(p_Guid) != m_PrevenClientDestroyGUIDs.end();
        }

        bool IsInMapSwitch() const { return m_IsInMapSwitch; }
        void SetInMapSwitch(bool p_In) { m_IsInMapSwitch = p_In; }

        uint64 GetLootContainerGuidForCreature(Creature* p_Creature);
        uint64 GetLootContainerGuidForGameObject(GameObject* p_Object);

        uint8 GetFace() const { return GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_FACE_ID); }
        uint8 GetSkin() const { return GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_SKIN_ID); }
        uint8 GetHairStyle() const { return GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_HAIR_STYLE_ID); }
        uint8 GetHairColor() const { return GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_HAIR_COLOR_ID); }

        uint8 GetTatooValue() const { return GetByteValue(PLAYER_FIELD_CUSTOM_DISPLAY_OPTION, PLAYER_CUSTOM_OPTOTIONS_TATTOOS); }
        uint8 GetHornValue() const { return GetByteValue(PLAYER_FIELD_CUSTOM_DISPLAY_OPTION, PLAYER_CUSTOM_OPTOTIONS_HORN_STYLE); }
        uint8 GetBlindFoldValue() const { return GetByteValue(PLAYER_FIELD_CUSTOM_DISPLAY_OPTION, PLAYER_CUSTOM_OPTOTIONS_BLINDFOLDS); }
        uint8 GetFacialStyleValue() const { return GetByteValue(PLAYER_FIELD_CUSTOM_DISPLAY_OPTION, PLAYER_CUSTOM_OPTOTIONS_FACIAL_STYLE); }

        std::unordered_map<uint8, QueryResultFuture> m_QueryResultFutures;

        void SetAppearZoneInstanceId(uint32 p_ZoneInstanceId)
        {
            m_AppearZoneInstanceId = p_ZoneInstanceId;
        }

        uint32 GetAppearZoneInstanceId() const
        {
            return m_AppearZoneInstanceId;
        }

        void SendSysMessage(const char* p_Message);

        void AddCosmetic(eCosmetics p_Cosmetic, uint32 p_Id);
        void _LoadCosmetics(PreparedQueryResult&& p_Result);
        bool HasCosmetic(eCosmetics p_Cosmetic, uint32 p_Id) const;
        std::vector<uint32> GetCosmetics(eCosmetics p_Cosmetic) const;
        void UpdateActiveCosmetic(eCosmetics p_Cosmetic, uint32 p_Id);
        void EnableCosmetic(bool apply = true);

        void _LoadNomiRecipes(PreparedQueryResult p_Result);

        void _LoadPlayerChoiceResponseRewarded(PreparedQueryResult p_Result);

        void _LoadPvpRewardSeason(PreparedQueryResult p_Result);
        void _LoadChestTimedLockout(PreparedQueryResult p_Result);

        /// GameObject interact system
        void _LoadObjectLockout(PreparedQueryResult p_Result);
        void AddObjectInLockoutList(uint32 p_DbGuid, uint32 p_QuestId, bool p_SetInvisible = false);
        void RemoveObjectsFromLockoutListByQuest(uint32 p_QuestId, bool p_IncludeDB = true);
        void RemoveObjectFromLockoutListByGuid(uint32 p_DbGuid) { m_ObjectLockout.erase(p_DbGuid); }
        bool IsObjectInLockoutList(uint32 p_DbGuid) { return m_ObjectLockout.find(p_DbGuid) != m_ObjectLockout.end(); }
        bool IsObjectInvisibleForPlayer(WorldObject const* p_Object) const;
        bool HasLockedObjects() const { return !m_ObjectLockout.empty(); }

        /// Phase timings system
        bool HasTimingForPhase(uint32 p_PhaseId);
        void SetTimingForPhase(uint32 p_PhaseId, uint32 p_Time);
        void UpdatePhaseTimings(uint32 p_Time);

        /// Lines and conversations trigger availability
        void AddCreatureInInaccessibleList(uint64 p_CreatureGuid) { m_InaccessibleCreatures.insert(p_CreatureGuid); }
        bool IsCreatureInInaccessibleList(uint64 p_CreatureGuid) { return m_InaccessibleCreatures.find(p_CreatureGuid) != m_InaccessibleCreatures.end(); }
        void RemoveCreatureFromInaccessibleList(uint64 p_CreatureGuid) { m_InaccessibleCreatures.erase(p_CreatureGuid); }

        bool CompleteLegendaryActivity(LegendaryActivity p_Activity, uint32 p_EncounterID = 0, bool p_Relinquished = false, InventoryType p_InventoryType = InventoryType::INVTYPE_NON_EQUIP);

        uint32 ComputeSellPrice() const;

        void SendTempSummonUITimer(Unit* p_TempSummon, uint32 p_CustomDuration = 0);

        void UpdateCooldowns(float p_OldModTimeRate, float p_NewModTimeRate, bool p_OnlyHasteAffectedSpells = false);
        void SetMirrorTimerFlag(uint8 p_Flag) { m_MirrorTimerFlags |= p_Flag; }
        void RemoveMirrorTimerFlag(uint8 p_Flag) { m_MirrorTimerFlags &= ~p_Flag; }

        /// Criteria update cache
        /// We update thoses criteria once per mins instead each damage/heal done (optimizations)
        uint32 m_HighestHitReceived;
        uint32 m_HighestHitDealt;
        uint32 m_TotalDamageReceived;
        uint32 m_HighestHealCast;
        uint32 m_HighestHealingReceived;
        uint32 m_TotalHealingReceived;
        uint32 m_CriteriaUpdateTimer;

        bool m_IsInLootboxProcess;
        bool m_IsInLoyaltyLootBoxProcess;
        uint32 m_CurrentLootboxId;

        std::unordered_map<uint64, uint32> m_LastDamageActionTime;

        std::unordered_map<std::string, std::set<uint64>> m_WhispersSentData;

        /// Lag tolerance
        Spell* GetQueuedSpell() const { return m_QueuedSpell; }
        void SetQueuedSpell(Spell* p_Spell) { m_QueuedSpell = p_Spell; }

        std::map<uint32, MS::Artifact::Manager*> const& GetArtifactHolder() const { return m_ArtifactMgrHolder; }

        void SetPlayerChoiceResponseRewarded(int32 p_ChoiceID, int32 p_ResponseID);

        bool CanReceiveLiveChangelog() const;
        void SetChangelogReceived();

        uint32 GetTimeSyncClient() const { return m_timeSyncClient; }

        void SendUpdateCelestialBody(uint32 p_CelestialBodyID);

        uint32 GetLastProcAttemptTime(uint32 p_SpellId) const;
        uint32 GetLastProcSuccessTime(uint32 p_SpellId) const;
        void SetLastProcAttemptTime(uint32 p_SpellId, uint32 p_LastProcAttemptTime);
        void SetLastProcSuccessTime(uint32 p_SpellId, uint32 p_LastProcSuccessTime);

        void AddMapTask(eTaskType p_TaskType, std::function<void()> p_Task);
        bool HasTaskRegistered(eTaskType p_TaskType);
        void UpdateTasks();
        void RemoveTaskInProgress() { m_TaskInProgress = false; }

        QueryCallbackManagerPtr GetQueryCallbackMgr() { return m_QueryCallbackMgr; }

        MapChallengeModeEntry const* m_challengeEntry;

        uint32 GetCommandCooldown() const { return m_PlayerCommandCooldown; }
        void ResetCommandCooldown() { m_PlayerCommandCooldown = sWorld->getIntConfig(CONFIG_PLAYER_COMMAND_COOLDOWN); }

        void UpdateAreaQuestTasks(uint32 newAreaId, uint32 oldAreaId);

        uint32 GetSpecializationRole() const;
        bool isInTankSpec() const;

    protected:
        void OnEnterPvPCombat();
        void OnLeavePvPCombat();

        /// Load pet battle async callback
        bool _LoadPetBattles(PreparedQueryResult&& p_Result);

        uint64 m_BattlePetSummon;
        uint32 m_LastSummonedBattlePet;

        std::vector<std::shared_ptr<BattlePet>>* m_BattlePets;
        std::shared_ptr<BattlePet> m_BattlePetCombatTeam[3];
        std::vector<std::pair<uint32, uint32>> m_OldPetBattleSpellToMerge;

        PreparedQueryResultFuture _petBattleJournalCallback;

        PlayerToys m_PlayerToys;

        BossLooted m_BossLooted;
        PersonalInstanceLockout m_PersonInstanceLockout;

#ifndef CROSS
        bool m_VoidStorageLoaded;
#else /* CROSS */
    public:
        /// InterRealm

        void SetForceCleanupChannels(bool val) { m_forceCleanupChannels = val; }
        bool IsNeedCleanupChannels() const { return m_forceCleanupChannels; }

        bool IsNeedRemove() const { return m_NeedRemove; }
        void SetNeedRemove(bool value) { m_NeedRemove = value; }
        void RemovePlayer();
        void SaveArenaData();
        void SaveCrossServerArenaData();

        bool PlayOnCross() const { return m_PlayOnCross; }
        void SetPlayOnCross(bool p_Play) { m_PlayOnCross = p_Play; }

        void AddItemToGuidSync(uint64 p_Guid) { m_ItemToGuidSync.push(p_Guid); }

     private:
         /// InterRealm
         bool m_forceCleanupChannels;
         bool m_NeedRemove;
         bool m_PlayOnCross;
         /// ============

         bool m_VoidStorageLoaded;
#endif /* CROSS */

    private:
        struct RunningScenePackage
        {
            RunningScenePackage() :
                m_PackageID(0), m_SpellID(0) {}

            RunningScenePackage(uint32 p_PackageID, uint32 p_SpellID) :
                m_PackageID(p_PackageID), m_SpellID(p_SpellID) {}

            RunningScenePackage(uint32 p_PackageID) :
                m_PackageID(p_PackageID), m_SpellID(0) {}

            uint32 m_PackageID; ///< PackageID of scene
            uint32 m_SpellID; ///< SpellID of scene if it's play from spell
        };
        std::map<uint32, RunningScenePackage>        m_RunningScenes;

        InteractionStatusMap            m_InteractionStatus;            ///< InteractionStatus
        InteractionStatusCallbackMap    m_InteractionStatusCallbacks;   ///< Callbacks
        std::mutex                      m_InteractionStatusMutex;       ///< Mutex

        std::string m_sentMsgCache;

        // Gamemaster whisper whitelist
        WhisperListContainer WhisperList;
        uint32 m_regenTimerCount;
        IntervalTimer m_PowerTimers[MAX_POWERS_PER_CLASS];
        IntervalTimer m_HealthTimer;
        float m_powerFraction[MAX_POWERS_PER_CLASS];
        uint32 m_contestedPvPTimer;

        std::set<uint64> m_ActiveMovementForces;

        std::unordered_map<uint32, uint32> m_sceneInstanceID;
        std::unordered_map<uint32, SceneEventStatus> m_sceneStatus;

        /*********************************************************/
        /***               BATTLEGROUND SYSTEM                 ***/
        /*********************************************************/

        /*
        this is an array of BG queues (BgTypeIDs) in which is player
        */
        struct BgBattlegroundQueueID_Rec
        {
            MS::Battlegrounds::BattlegroundType::Type BgType;
            uint32 invitedToInstance;
        };

        BgBattlegroundQueueID_Rec m_bgBattlegroundQueueID[PLAYER_MAX_BATTLEGROUND_QUEUES];
        BGData                    m_bgData;
        std::map<uint32, uint32>  m_CrossInvitationForBG;

        bool m_IsInBattleground;
        bool m_IsInBattlegroundProcess;
        bool m_IsInBattlegroundLeaving;
        bool m_IsInDuel;

        /*********************************************************/
        /***                    QUEST SYSTEM                   ***/
        /*********************************************************/

        //We allow only one timed quest active at the same time. Below can then be simple value instead of set.
        typedef std::set<uint32> QuestSet;
        typedef std::set<uint32> SeasonalQuestSet;
        typedef std::unordered_map<uint32,SeasonalQuestSet> SeasonalEventQuestMap;
        QuestSet m_timedquests;
        QuestSet m_weeklyquests;
        QuestSet m_monthlyquests;
        SeasonalEventQuestMap m_seasonalquests;

        uint64 m_divider;
        uint32 m_ingametime;

        /*********************************************************/
        /***                   LOAD SYSTEM                     ***/
        /*********************************************************/

        void _LoadActions(PreparedQueryResult result);
        void _LoadAuras(PreparedQueryResult result, PreparedQueryResult resultEffect, uint32 timediff);
        void _LoadGlyphAuras();
        void _LoadGlyphs(PreparedQueryResult result);
        void _LoadBoundInstances(PreparedQueryResult result);
        void _LoadHeirloomCollection(PreparedQueryResult p_Result);
        void _LoadArtifacts(PreparedQueryResult p_Result, PreparedQueryResult p_PowersResult);
        void _LoadInventory(PreparedQueryResult p_Result, PreparedQueryResult p_DelinkItemsResult, uint32 p_DimeDiff);
        void _LoadVoidStorage(PreparedQueryResult result);
        void _LoadMailInit(PreparedQueryResult resultUnread, PreparedQueryResult resultDelivery);
        void _LoadMail(PreparedQueryResult p_Mail);
        void _LoadMailedItems(PreparedQueryResult p_MailedItems);
        void _LoadQuestStatus(PreparedQueryResult result);
        void _LoadQuestObjectiveStatus(PreparedQueryResult result);
        void _LoadQuestStatusRewarded(PreparedQueryResult result);
        void _LoadDailyQuestStatus(PreparedQueryResult result);
        void _LoadWeeklyQuestStatus(PreparedQueryResult result);
        void _LoadMonthlyQuestStatus(PreparedQueryResult result);
        void _LoadSeasonalQuestStatus(PreparedQueryResult result);
        void _LoadAccountQuestRewarded(PreparedQueryResult p_Result);
        void _LoadPvPInfo(PreparedQueryResult p_Result);
        void _LoadGroup(PreparedQueryResult result);
        void _LoadSkills(PreparedQueryResult result);
        void _LoadSpells(PreparedQueryResult result);
        bool _LoadHomeBind(PreparedQueryResult result);
        void _LoadDeclinedNames(PreparedQueryResult result);
        void _LoadEquipmentSets(PreparedQueryResult result);
        void _LoadBracketData(PreparedQueryResult result);
        void _LoadBGData(PreparedQueryResult result);
        void _LoadTalents(PreparedQueryResult result);
        void _LoadPVPTalents(PreparedQueryResult p_Result);
        void _LoadInstanceTimeRestrictions(PreparedQueryResult result);
        void _LoadCUFProfiles(PreparedQueryResult result);
        void _LoadCurrency(PreparedQueryResult result);
        void _LoadCurrencyCaps(PreparedQueryResult result);
        void _LoadBossLooted(PreparedQueryResult p_Result);
        void _LoadWorldStates(PreparedQueryResult p_Result);
        void _LoadGarrisonDailyTavernDatas(PreparedQueryResult p_Result);
        void _LoadCharacterGarrisonWeeklyTavernDatas(PreparedQueryResult p_Result);
        void _LoadPersonalInstanceLockouts(PreparedQueryResult p_Result);
        void _LoadLegendaries(PreparedQueryResult p_Result);

        /*********************************************************/
        /***                   SAVE SYSTEM                     ***/
        /*********************************************************/

        void _SaveGlyphs(SQLTransaction& trans) const;
        void _SaveActions(SQLTransaction& trans);
        void _SaveAuras(SQLTransaction& trans);
        void _SaveInventory(SQLTransaction& trans);
        void _SaveVoidStorage(SQLTransaction& trans);
        void _SaveMail(SQLTransaction& trans);
        void _SaveQuestStatus(SQLTransaction& trans, SQLTransaction& p_AuthTrans);
        void _SaveQuestObjectiveStatus(SQLTransaction& trans);
        void _SaveDailyQuestStatus(SQLTransaction& trans);
        void _SaveWeeklyQuestStatus(SQLTransaction& trans);
        void _SaveMonthlyQuestStatus(SQLTransaction& trans);
        void _SaveSeasonalQuestStatus(SQLTransaction& trans);
        void _SaveSkills(SQLTransaction& trans);
        void _SaveSpells(SQLTransaction& charTrans, SQLTransaction& accountTrans);
        void _SaveEquipmentSets(SQLTransaction& trans);
        void _SaveArenaData(SQLTransaction& trans);
        void _SaveBGData(SQLTransaction& trans);
        void _SaveTalents(SQLTransaction& trans);
        void _SavePvPTalents(SQLTransaction& p_Trans);
        void _SaveStats(SQLTransaction& trans);
        void _SaveInstanceTimeRestrictions(SQLTransaction& trans);
        void _SaveCurrency(SQLTransaction& trans);
        void _SaveCurrencyCaps(SQLTransaction& trans);
        void _SaveCharacterWorldStates(SQLTransaction& p_Transaction);
        void _SavePvpInfo(SQLTransaction& p_Transaction);

        void _SaveCharacterGarrisonDailyTavernDatas(SQLTransaction& p_Transaction);
        void _SaveCharacterGarrisonWeeklyTavernDatas(SQLTransaction& p_Transaction);

        void _SavePersonalInstanceLockouts(SQLTransaction& p_Transaction);
        void _SaveLegendaries(SQLTransaction& p_Transaction);
        void _SaveNomiRecipes(SQLTransaction& p_Transaction);

        void _SaveObjectLockoutDatas(SQLTransaction& p_Transaction);

        /*********************************************************/
        /***              ENVIRONMENTAL SYSTEM                 ***/
        /*********************************************************/
        void HandleSobering();
        /// Send mirror timer start
        /// @p_TimerType    : Type of timer, see MirrorTimerType enum
        /// @p_MaxValue     : Max value
        /// @p_CurrValue    : Current value
        /// @p_Scale        : Time scale
        /// @p_Paused       : Is the timer paused
        void SendMirrorTimer(MirrorTimerType p_TimerType, uint32 p_MaxValue, uint32 p_CurrValue, int32 p_Scale, uint32 p_SpellID, bool p_Paused);
        /// Send mirror timer stop
        /// @p_TimerType : Type of timer, see MirrorTimerType enum
        void StopMirrorTimer(MirrorTimerType p_TimerType);
        void HandleDrowning(uint32 p_TimeDiff);

        int32 getMaxTimer(MirrorTimerType timer);

        /*********************************************************/
        /***                  HONOR SYSTEM                     ***/
        /*********************************************************/
        time_t m_lastHonorUpdateTime;

        void outDebugValues() const;
        uint64 m_lootGuid;

        uint32 m_lootSpecId;

        uint8 m_OriginalRace;
        uint32 m_OriginalPlayerBytes;
        uint32 m_team;
        uint32 m_nextSave;
        time_t m_speakTime;
        uint32 m_speakCount;
        time_t m_pmChatTime;
        uint32 m_pmChatCount;
        Difficulty m_dungeonDifficulty;
        Difficulty m_raidDifficulty;
        Difficulty m_LegacyRaidDifficulty;
        Difficulty m_PrevMapDifficulty;

        uint32 m_atLoginFlags;

        Item* m_items[PLAYER_SLOTS_COUNT];

        uint32 m_currentBuybackSlot;

        PlayerCurrenciesMap _currencyStorage;
        PlayerCurrencyCaps _currencyCapsList;

        VoidStorageItem* _voidStorageItems[VOID_STORAGE_MAX_SLOT];

        std::vector<Item*> m_itemUpdateQueue;
        bool m_itemUpdateQueueBlocked;

        uint32 m_ExtraFlags;
        uint64 m_curSelection;

        QuestStatusMap m_QuestStatus;
        QuestObjectiveStatusMap m_questObjectiveStatus;
        QuestStatusSaveMap m_QuestStatusSave;

        RewardedQuestSet m_RewardedQuests;
        RewardedQuestSet m_AccountRewardedQuests;

        QuestStatusSaveMap m_RewardedQuestsSave;
        QuestStatusSaveMap m_AccountRewardedQuestsSave;

        SkillStatusMap mSkillStatus;

        uint32 m_GuildIdInvited;

        PlayerMails m_mail;
        PlayerSpellMap m_spells;
        std::unordered_map<uint32 /*overridenSpellId*/, std::unordered_set<uint32> /*newSpellId*/> m_overrideSpells;

        struct LastPotion
        {
            uint32 m_LastPotionItemID;
            uint32 m_LastPotionSpellID;
            uint64 m_LastPotionGUID;
        };

        /// Last used health/mana potion in combat, that block next potion use
        std::map<uint32, LastPotion> m_LastPotions;

        GlobalCooldownMgr m_GlobalCooldownMgr;
        struct prohibited_struct
        {
            prohibited_struct(uint32 _time): m_time_prohibited_until(getMSTime() + _time) { }
            prohibited_struct(): m_time_prohibited_until(0) { }
            uint32 m_time_prohibited_until;
        };

        prohibited_struct prohibited[MAX_SPELL_SCHOOL];

        PlayerTalentInfo* _talentMgr;

        bool m_glyphsChanged;

        ActionButtonList m_actionButtons;

        float m_auraBaseMod[BASEMOD_END][MOD_END];
        int16 m_baseRatingValue[MAX_COMBAT_RATING];
        uint32 m_baseSpellPower;
        uint32 m_baseManaRegen;
        uint32 m_baseHealthRegen;
        int32 m_spellPenetrationItemMod;

        SpellModList m_spellMods[MAX_SPELLMOD];
        //uint32 m_pad;
//        Spell* m_spellModTakingSpell;  // Spell for which charges are dropped in spell::finish

        EnchantDurationList m_enchantDuration;
        ItemDurationList m_itemDuration;
        ItemDurationList m_itemSoulboundTradeable;

        std::map<uint64, uint32> m_SpellsFromItem;

        void ResetTimeSync();
        void SendTimeSync();

        ResurrectionData* _resurrectionData;

        WorldSession* m_session;

#ifdef CROSS
        typedef std::list<Channel*> JoinedChannelsList;
#endif /* CROSS */
        JoinedChannelsList m_channels;

        uint8 m_cinematic;

        TradeData* m_trade;
        uint32 m_ClientStateIndex;

        bool   m_DailyQuestChanged;
        bool   m_WeeklyQuestChanged;
        bool   m_MonthlyQuestChanged;
        bool   m_SeasonalQuestChanged;
        time_t m_lastDailyQuestTime;

        uint32 m_drunkTimer;
        uint32 m_weaponChangeTimer;

        uint32 m_zoneUpdateId;
        uint32 m_zoneUpdateTimer;
        uint32 m_areaUpdateId;

        uint32 m_deathTimer;
        time_t m_deathExpireTime;

        uint32 m_restTime;

        uint32 m_WeaponProficiency;
        uint32 m_ArmorProficiency;
        bool m_canParry;
        bool m_canBlock;
        bool m_canTitanGrip;
        uint8 m_swingErrorMsg;

        bool m_needSummonPetAfterStopFlying;

        ////////////////////Rest System/////////////////////
        time_t _restTime;
        uint32 inn_triggerId;
        float m_rest_bonus;
        uint32 _restFlagMask;
        ////////////////////Rest System/////////////////////

        // Social
        PlayerSocial *m_social;

        // Groups
        GroupPtr m_Group;
        int8   m_SubGroup;

        GroupPtr m_OriginalGroup;
        int8   m_OriginalSubGroup;

        uint32 m_groupInviteGUID;
        uint32 m_groupUpdateMask;
        std::bitset<MAX_AURAS> m_auraRaidUpdateMask;
        bool m_bPassOnGroupLoot;

        // last used pet number (for BG's)
        uint32 m_lastpetnumber;

        // Player summoning
        bool   m_Summoned;
        time_t m_summon_expire;
        uint32 m_summon_mapid;
        float  m_summon_x;
        float  m_summon_y;
        float  m_summon_z;

        uint32 m_LastActiveSummon;

        DeclinedName *m_declinedname;
        Runes* m_runes;
        EquipmentSets m_EquipmentSets;

        bool CanAlwaysSee(WorldObject const* obj) const override;

        bool IsAlwaysDetectableFor(WorldObject const* seer) const override;

        uint8 m_grantableLevels;

        uint32 m_tokenCounter;

        typedef std::set<uint32> DailyQuestList;
        DailyQuestList m_dailyQuestStorage;

        std::queue<std::function<void()>> m_CriticalOperation;
        std::recursive_mutex m_CriticalOperationLock;

        uint64 m_BeaconOfFaithTargetGUID;
        uint64 m_BeaconOfLightTargetGUID;

        uint64 m_PrayerOfMendingTargetGUID;

        uint32 m_LastCastedSpell;
        uint32 m_CastingSpell;

        bool m_BackPackAutoSortDisabled;

        std::set<uint64> m_PrevenClientDestroyGUIDs;

        bool m_IsInMapSwitch;

        std::set<uint16> m_PrimaryProfessionLearnt;

        uint32 m_AppearZoneInstanceId;
        uint32 m_LegendaryPoints;
        uint32 m_LegendaryCount;

        uint32 m_UpdateStep;
        std::chrono::time_point<std::chrono::steady_clock> m_CurrentStepTimer;

        uint32 m_PlayerCommandCooldown;
    private:
        std::map<uint32, MS::Artifact::Manager*> m_ArtifactMgrHolder;

        // internal common parts for CanStore/StoreItem functions
        InventoryResult CanStoreItem_InSpecificSlot(uint8 bag, uint8 slot, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool swap, Item* pSrcItem) const;
        InventoryResult CanStoreItem_InBag(uint8 bag, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool merge, bool non_specialized, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        InventoryResult CanStoreItem_InInventorySlots(uint8 slot_begin, uint8 slot_end, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool merge, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        Item* _StoreItem(uint16 pos, Item* pItem, uint32 count, bool clone, bool update);
        Item* _LoadItem(SQLTransaction& trans, uint32 zoneId, uint32 timeDiff, Field* fields);

        std::set<uint32> m_refundableItems;
        void SendRefundInfo(Item* item);
        void RefundItem(Item* item);
        void SendItemRefundResult(Item* item, ItemExtendedCostEntry const* iece, uint8 error);

        // know currencies are not removed at any point (0 displayed)
        void AddKnownCurrency(uint32 itemId);

        int32 CalculateReputationGain(ReputationSource source, uint32 creatureOrQuestLevel, int32 rep, int32 faction, bool noQuestBonus = false, bool isWorldQuest = false);
        void AdjustQuestReqItemCount(Quest const* quest);

        bool IsCanDelayTeleport() const { return m_bCanDelayTeleport; }
        void SetCanDelayTeleport(bool setting) { m_bCanDelayTeleport = setting; }
        bool IsHasDelayedTeleport() const { return m_bHasDelayedTeleport; }
        void SetDelayedTeleportFlag(bool setting) { m_bHasDelayedTeleport = setting; }

        MapReference m_mapRef;

        void UpdateCharmedAI();

        uint32 m_lastFallTime;
        float  m_lastFallZ;

        int32 m_MirrorTimer[MAX_TIMERS];
        uint8 m_MirrorTimerFlags;
        uint8 m_MirrorTimerFlagsLast;
        bool m_isInWater;

        // Rune type / Rune timer
        uint32 m_runeGraceCooldown[MAX_RUNES];
        uint32 m_lastRuneGraceTimers[MAX_RUNES];

        // Current teleport data
        WorldLocation m_teleport_dest;
        uint32 m_teleport_options;
        bool mSemaphoreTeleport_Near;
        bool mSemaphoreTeleport_Far;
        bool mTeleportFlagForAnticheat;

        uint32 m_DelayedOperations;
        bool m_bCanDelayTeleport;
        bool m_bHasDelayedTeleport;
        bool m_isMoltenCored;

        // Temporary removed pet cache
        uint32 m_temporaryUnsummonedPetNumber;
        uint32 m_oldpetspell;

        PlayerAchievementMgr* m_achievementMgr;
        ReputationMgr  m_reputationMgr;

        SpellCooldowns m_spellCooldowns;

        uint32 m_ChampioningFaction;

        uint32 m_timeSyncTimer;
        uint32 m_timeSyncClient;
        uint32 m_timeSyncServer;

        InstanceTimeMap _instanceResetTimes;
        uint32 _pendingBindId;
        uint32 _pendingBindTimer;

        uint32 _activeCheats;
        uint32 _maxPersonalArenaRate;

        PhaseMgr phaseMgr;

        uint32 _lastTargetedGO;
        float m_PersonnalXpRate;

        MS::WorldQuest::Manager* m_WorldQuestMgr;
        //////////////////////////////////////////////////////////////////////////
        /// Garrison
        //////////////////////////////////////////////////////////////////////////
        MS::Garrison::Manager* m_Garrison;
        IntervalTimer m_GarrisonUpdateTimer;

        //////////////////////////////////////////////////////////////////////////
        /// Cinematic
        //////////////////////////////////////////////////////////////////////////
        CinematicSequence * m_CinematicSequence;
        bool                m_InCinematic;
        uint32              m_CinematicClientStartTime;
        float               m_CinematicStartX, m_CinematicStartY, m_CinematicStartZ, m_CinematicStartO;
        bool                m_FlyingBeforeCinematic;

        //////////////////////////////////////////////////////////////////////////
        /// World States
        //////////////////////////////////////////////////////////////////////////
        MS::Utilities::MutextedMap<uint32, uint32> m_WorldStates;

        std::unordered_map<uint32, time_t> m_ChestTimedLockout;

        struct ObjectLockout
        {
            uint32 QuestId;
            bool Invisible;
        };
        std::unordered_map<uint32, ObjectLockout> m_ObjectLockout;
        std::unordered_map<uint32, uint32> m_PhaseTimings;

        GuidUnorderedSet m_InaccessibleCreatures;

        uint32 m_knockBackTimer;
        uint8  m_ignoreMovementCount;

        uint32 m_groupUpdateDelay;

        bool m_initializeCallback;

        uint32 m_emote;

        uint32 m_lastPlayedEmote;

        MS::Skill::Archaeology::Manager m_archaeologyMgr;

        PreparedQueryResultFuture _petPreloadCallback;
        QueryResultHolderFuture _petLoginCallback;

        /// Store callback
        bool m_StoreDeliveryProcessed[StoreCallback::MaxDelivery];
        bool m_StoreDeliverySave;

        /// AntiCheat
        time_t m_LastSpecialMovementTime;

        uint8 m_bgRoles;

        /// PvP Bracket data
        uint32 m_ArenaPersonalRating[(uint8)BattlegroundBracketType::Max];
        uint32 m_ArenaMatchMakerRating[(uint8)BattlegroundBracketType::Max];
        /// Day
        uint32 m_DayWins[(uint8)BattlegroundBracketType::Max];
        uint32 m_DayGames[(uint8)BattlegroundBracketType::Max];
        /// Week
        uint32 m_WeekWins[(uint8)BattlegroundBracketType::Max];
        uint32 m_WeekGames[(uint8)BattlegroundBracketType::Max];
        uint32 m_BestRatingOfWeek[(uint8)BattlegroundBracketType::Max];
        /// Previous Week
        uint32 m_PrevWeekWins[(uint8)BattlegroundBracketType::Max];
        uint32 m_PrevWeekGames[(uint8)BattlegroundBracketType::Max];
        uint32 m_BestRatingOfPrevWeek[(uint8)BattlegroundBracketType::Max];
        /// Season
        uint32 m_SeasonWins[(uint8)BattlegroundBracketType::Max];
        uint32 m_SeasonGames[(uint8)BattlegroundBracketType::Max];
        uint32 m_BestRatingOfSeason[(uint8)BattlegroundBracketType::Max];

        CUFProfiles m_cufProfiles;

#ifdef CROSS
        /// InterRealm
        std::queue<uint64> m_ItemToGuidSync;      ///< Every item without local realm guid
#endif /* CROSS */

#ifndef CROSS
        uint32 m_irZoneId;
        uint32 m_irAreaId;
        uint32 m_irMapId;
        InterRealmPlayerState::Type m_InterRealmPlayerState;
        time_t m_InterRealmPlayerStateLastChange;
#endif

        uint32 m_PvPCombatTimer;
        bool m_pvpCombat;

        //////////////////////////////////////////////////////////////////////////
        /// Vignette
        //////////////////////////////////////////////////////////////////////////
        Vignette::Manager m_VignetteMgr;

        /// Wargame
        WargameRequest* m_WargameRequest;
        bool m_WargameInvited;

        /// Character WorldState
        std::map<uint32/*WorldState*/, CharacterWorldState> m_CharacterWorldStates;

        /// Armory caches
        float m_MasteryCache;

        uint32 m_EndSalesTimestamp;

        //////////////////////////////////////////////////////////////////////////
        /// Cosmetics
        //////////////////////////////////////////////////////////////////////////
        std::array<CosmeticSet, eCosmetics::MaxCosmetic> m_Cosmetics;
        std::pair<eCosmetics, uint32> m_ActiveCosmetic;
        bool m_CosmeticEnabled;

        /// Skip update system
        uint8  m_SkipUpdateCounter;
        uint32 m_DelayedUpdateTime;
        /// Lag tolerance
        Spell* m_QueuedSpell;

        std::set<uint64> m_PlayerChoiceResponseRewarded;
        std::map<uint32/*ZoneID*/, uint32/*CustomPhaseMask*/> m_ZonesPhaseMask;

        bool m_NeedForcedVisibilityUpdate;
        uint32 m_CelestialBodyID;

        Map* m_OldMap;

        std::unordered_map<uint32 /*spellId*/, LastProcInfo> m_LastProcTimes;

        struct TaskInfo
        {
            TaskInfo(eTaskType p_Type, std::function<void()> p_Task)
            {
                Type = p_Type;
                Task = p_Task;
            }

            TaskInfo()
            {
            }

            eTaskType Type;
            std::function<void()> Task;
        };

        std::array<std::atomic_uint, eTaskType::MaxTaskType> m_RegisteredTaskCounters;
        ACE_Based::LockedQueue<TaskInfo, ACE_Thread_Mutex> m_RegisteredTasks;
        std::atomic_bool m_TaskInProgress;

        WorldLocation m_CorpseLocation;

        QueryCallbackManagerPtr m_QueryCallbackMgr;
};

void AddItemsSetItem(Player*player, Item* item);
void RemoveItemsSetItem(Player*player, ItemTemplate const* proto);

// "the bodies of template functions must be made available in a header file"
template <class T> void Player::ApplySpellMod(uint32 p_SpellId, SpellModOp p_Op, T& p_Basevalue, Spell* p_Spell, bool p_RemoveStacks, std::ostringstream* p_DamageStreamLog)
{
    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellId);
    if (!l_SpellInfo)
        return;

    float l_TotalMul = 1.0f;
    int32 l_TotalFlat = 0;

    bool l_ChaosBolt = false;

    /// Drop charges for triggering spells instead of triggered ones
    Spell* l_SpellModTakingSpell = p_Spell;
    if (m_spellModTakingSpell)
        l_SpellModTakingSpell = m_spellModTakingSpell;

    int32 l_PlayerWeaponMask = 0;

    for (uint8 l_I = 0; l_I < WeaponAttackType::MaxAttack; ++l_I)
    {
        Item* l_Tmpitem = GetWeaponForAttack(WeaponAttackType(l_I), true);
        if (!l_Tmpitem || l_Tmpitem->CantBeUse() || !l_Tmpitem->GetTemplate())
            continue;

        l_PlayerWeaponMask |= 1 << l_Tmpitem->GetTemplate()->SubClass;
    }

    for (SpellModList::iterator l_Iter = m_spellMods[p_Op].begin(); l_Iter != m_spellMods[p_Op].end(); ++l_Iter)
    {
        SpellModifier* l_SpellMod = *l_Iter;

        /// Charges can be set only for mods with auras
        if (!l_SpellMod->ownerAura)
            ASSERT(l_SpellMod->charges == 0);

        if (!IsAffectedBySpellmod(l_SpellInfo, l_SpellMod, l_SpellModTakingSpell, p_Spell))
            continue;

        if (l_SpellMod->ownerAura && l_SpellMod->ownerAura->GetSpellInfo())
        {
            int32 l_SpellWeaponMask = l_SpellMod->ownerAura->GetSpellInfo()->EquippedItemSubClassMask;

            if (l_SpellWeaponMask > 0 && l_PlayerWeaponMask > 0)
            {
                if (!(l_PlayerWeaponMask & l_SpellWeaponMask))
                    continue;
            }
        }

        if (l_SpellMod->type == SpellModType::SPELLMOD_FLAT)
        {
            if (p_DamageStreamLog)
                *p_DamageStreamLog <<  "SPELLMOD " << p_Op << "(SPELLMOD_FLAT, " << l_SpellMod->spellId << "," << l_SpellMod->value << ")" << std::endl;
            l_TotalFlat += l_SpellMod->value;
        }
        else if (l_SpellMod->type == SpellModType::SPELLMOD_PCT)
        {
            /// Skip percent mods for null basevalue (most important for spell mods with charges)
            /// Fix Celestial Alignment and Incarnation: Chosen of Elune and Blessing of Elune
            /// TODO: Maybe it's better to check l_TotalFlat != 0 ?
            if (p_Basevalue == T(0) && l_SpellMod->spellId != 194223 && l_SpellMod->spellId != 102560 && l_SpellMod->spellId != 202737)
                continue;

            /// Poison Knives should only be apply with a Fan of Knives
            if (p_SpellId == 2818 && l_SpellMod->spellId == 192376)
                continue;

            if (l_SpellMod->spellId == 209385)
            {
                p_Basevalue = T(40.0f);
                return;
            }

            if (l_SpellMod->spellId == 164545 && l_SpellMod->charges) ///< Solar Empowerment
            {
                AddPct(l_TotalMul, l_SpellMod->value / l_SpellMod->charges);
                continue;
            }

            /// 7.3.5 - Build 26365 - Both strikes of the windlord from Monk with Storm Earth and Fire: Damage is already reduced by 50% when calculating weapon damage, so don't add it again here :)
            if (l_SpellMod->spellId == 137639 && (p_SpellId == 222029 || p_SpellId == 205414))
                continue;

            /// Special case (skip > 10sec spell casts for instant cast setting)
            if (l_SpellMod->op == SpellModOp::SPELLMOD_CASTING_TIME && p_Basevalue >= T(10000) && l_SpellMod->value <= -100)
                continue;

            /// Fix Guardian of Elune to deal with current dodge pct
            else if (l_SpellMod->spellId == 155578 && l_SpellMod->op == SpellModOp::SPELLMOD_COST)
            {
                int32 l_DodgeChance = (int32)GetFloatValue(PLAYER_FIELD_DODGE_PERCENTAGE);
                l_SpellMod->value = l_DodgeChance * -1;
            }

            if (!NeedToConsumeSpellMod(l_SpellInfo, l_SpellMod))
                continue;

            if (p_Op == SpellModOp::SPELLMOD_CRIT_DAMAGE_BONUS && CanApplyPvPSpellModifiers())
                l_SpellMod->value *= 0.75f; ///< in pvp, crit is 150% damage instead of 200% damage, so need to lower the spellmod value accordingly.

            if (p_DamageStreamLog)
                *p_DamageStreamLog << "SPELLMOD" << p_Op << "(SPELLMOD_PCT, " << l_SpellMod->spellId << "," << l_SpellMod->value << ")" << std::endl;

            if (l_SpellMod->spellId == 197218 /*Thunder Focus Tea*/ && l_SpellMod->op == SpellModOp::SPELLMOD_DURATION)
                l_SpellMod->value = -50.0f; ///< always 50%

             AddPct(l_TotalMul, l_SpellMod->value);
        }

        if (l_SpellMod->spellId == 238072 && HasAura(216708) && l_SpellInfo->IsPeriodic()) ///< Winnowing
            p_Basevalue = float(p_Basevalue)*2;

        if (p_RemoveStacks && l_SpellModTakingSpell && !m_isMoltenCored)
            DropModCharge(l_SpellMod, l_SpellModTakingSpell);
    }

    /// TODO: Check if there should be negative cost for some spells.
    if (p_Op == SpellModOp::SPELLMOD_COST && G3D::fuzzyLt(l_TotalMul, 0.0f)
        && p_SpellId == 193315) ///< Saber Lash - 193315 and Swordplay - 211669
        l_TotalMul = 0.0f;

    /// HACK:
    /// For example, Archavon's Heavy Hand - 205144 and Dauntless - 202297, then Mortal Strike - 12294 should cost (200 * 0.8 - 20) = 100, but it's 108 if the old calculation is used
    /// TODO: check if cost/all mods should be calculated in the new way
    if (p_SpellId == 12294 && p_Op == SpellModOp::SPELLMOD_COST)
    {
        p_Basevalue = T(float(p_Basevalue * l_TotalMul + l_TotalFlat));
    }
    else
    {
        p_Basevalue = T(float(p_Basevalue + l_TotalFlat) * l_TotalMul);
    }
}
#endif
