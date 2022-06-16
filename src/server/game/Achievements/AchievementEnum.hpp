////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

enum AchievementFaction
{
    ACHIEVEMENT_FACTION_HORDE           = 0,
    ACHIEVEMENT_FACTION_ALLIANCE        = 1,
    ACHIEVEMENT_FACTION_ANY             = -1
};

enum AchievementFlags
{
    ACHIEVEMENT_FLAG_COUNTER               = 0x00000001,    ///< Just count statistic (never stop and complete)
    ACHIEVEMENT_FLAG_HIDDEN                = 0x00000002,    ///< Not sent to client - internal use only
    ACHIEVEMENT_FLAG_PLAY_NO_VISUAL        = 0x00000004,    ///< Client does not play achievement earned visual
    ACHIEVEMENT_FLAG_SUMM                  = 0x00000008,    ///< Use summ criteria value from all requirements (and calculate max value)
    ACHIEVEMENT_FLAG_MAX_USED              = 0x00000010,    ///< Show max criteria (and calculate max value ??)
    ACHIEVEMENT_FLAG_REQ_COUNT             = 0x00000020,    ///< Use not zero req count (and calculate max value)
    ACHIEVEMENT_FLAG_AVERAGE               = 0x00000040,    ///< Show as average value (value / time_in_days) depend from other flag (by def use last criteria value)
    ACHIEVEMENT_FLAG_BAR                   = 0x00000080,    ///< Show as progress bar (value / max vale) depend from other flag (by def use last criteria value)
    ACHIEVEMENT_FLAG_REALM_FIRST_REACH     = 0x00000100,
    ACHIEVEMENT_FLAG_REALM_FIRST_KILL      = 0x00000200,
    ACHIEVEMENT_FLAG_UNK3                  = 0x00000400,    ///< ACHIEVEMENT_FLAG_HIDE_NAME_IN_TIE
    ACHIEVEMENT_FLAG_HIDE_INCOMPLETE       = 0x00000800,    ///< hide from UI if not completed
    ACHIEVEMENT_FLAG_SHOW_IN_GUILD_NEWS    = 0x00001000,    ///< Shows in guild news
    ACHIEVEMENT_FLAG_SHOW_IN_GUILD_HEADER  = 0x00002000,    ///< Shows in guild news header
    ACHIEVEMENT_FLAG_GUILD                 = 0x00004000,
    ACHIEVEMENT_FLAG_SHOW_GUILD_MEMBERS    = 0x00008000,
    ACHIEVEMENT_FLAG_SHOW_CRITERIA_MEMBERS = 0x00010000,
    ACHIEVEMENT_FLAG_ACCOUNT               = 0x00020000,
    ACHIEVEMENT_FLAG_UNK5                  = 0x00040000,
    ACHIEVEMENT_FLAG_HIDE_ZERO_COUNTER     = 0x00080000,    ///< statistic is hidden from UI if no criteria value exists
    ACHIEVEMENT_FLAG_TRACKING_FLAG         = 0x00100000,    ///< hidden tracking flag, sent to client in all cases except completion announcements
};

enum CriteriaCondition
{
    CRITERIA_CONDITION_NONE               = 0,
    CRITERIA_CONDITION_NO_DEATH           = 1,    ///< reset progress on death
    CRITERIA_CONDITION_UNK2               = 2,    ///< only used in "Complete a daily quest every day for five consecutive days"
    CRITERIA_CONDITION_BG_MAP             = 3,    ///< requires you to be on specific map, reset at change
    CRITERIA_CONDITION_NO_LOSE            = 4,    ///< only used in "Win 10 arenas without losing"
    CRITERIA_CONDITION_NOT_LOSE_AURA      = 5,    ///< used by spell 95529
    CRITERIA_CONDITION_UNK8               = 8,
    CRITERIA_CONDITION_NO_SPELL_HIT       = 9,    ///< requires the player not to be hit by specific spell
    CRITERIA_CONDITION_NOT_IN_GROUP       = 10,   ///< requires the player not to be in group
    CRITERIA_CONDITION_NO_LOSE_PET_BATTLE = 11,  ///< only used in "Win 10 consecutive pet battles."
    CRITERIA_CONDITION_SERVER_SIDE_CHECK  = 13    ///< Needs server side script
};

enum CriteriaAdditionalCondition
{
    CRITERIA_ADDITIONAL_CONDITION_SOURCE_DRUNK_VALUE            = 1,
    CRITERIA_ADDITIONAL_CONDITION_SERVER_SIDE_CHECK             = 2,
    CRITERIA_ADDITIONAL_CONDITION_ITEM_LEVEL                    = 3,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_ENTRY         = 4,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_PLAYER         = 5,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_DEAD           = 6,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_ENEMY          = 7,
    CRITERIA_ADDITIONAL_CONDITION_SOURCE_HAS_AURA               = 8,
    CRITERIA_ADDITIONAL_CONDITION_UNK_9                         = 9,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_HAS_AURA               = 10,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_HAS_AURA_TYPE          = 11,
    CRITERIA_ADDITIONAL_CONDITION_UNK_12                        = 12,
    CRITERIA_ADDITIONAL_CONDITION_UNK_13                        = 13,
    CRITERIA_ADDITIONAL_CONDITION_ITEM_QUALITY_MIN              = 14,
    CRITERIA_ADDITIONAL_CONDITION_ITEM_QUALITY_EQUALS           = 15,
    CRITERIA_ADDITIONAL_CONDITION_UNK16                         = 16,
    CRITERIA_ADDITIONAL_CONDITION_SOURCE_AREA_OR_ZONE           = 17,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_AREA_OR_ZONE           = 18,
    CRITERIA_ADDITIONAL_CONDITION_UNK_19                        = 19,
    CRITERIA_ADDITIONAL_CONDITION_MAP_DIFFICULTY_OLD            = 20,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_YIELDS_XP     = 21,
    CRITERIA_ADDITIONAL_CONDITION_UNK_22                        = 22,
    CRITERIA_ADDITIONAL_CONDITION_UNK_23                        = 23,
    CRITERIA_ADDITIONAL_CONDITION_ARENA_TYPE                    = 24,
    CRITERIA_ADDITIONAL_CONDITION_SOURCE_RACE                   = 25,
    CRITERIA_ADDITIONAL_CONDITION_SOURCE_CLASS                  = 26,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_RACE                   = 27,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_CLASS                  = 28,
    CRITERIA_ADDITIONAL_CONDITION_MAX_GROUP_MEMBERS             = 29,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_TYPE          = 30,
    CRITERIA_ADDITIONAL_CONDITION_UNK_31                        = 31,
    CRITERIA_ADDITIONAL_CONDITION_SOURCE_MAP                    = 32,
    CRITERIA_ADDITIONAL_CONDITION_ITEM_CLASS                    = 33,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_ITEM_SUBCLASS                 = 34,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_COMPLETE_QUEST_NOT_IN_GROUP   = 35,
    CRITERIA_ADDITIONAL_CONDITION_UNK_36                        = 36,
    CRITERIA_ADDITIONAL_CONDITION_MIN_PERSONAL_RATING           = 37,
    CRITERIA_ADDITIONAL_CONDITION_TITLE_BIT_INDEX               = 38,
    CRITERIA_ADDITIONAL_CONDITION_SOURCE_LEVEL                  = 39,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_LEVEL                  = 40,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_ZONE                   = 41,
    CRITERIA_ADDITIONAL_CONDITION_UNK_42                        = 42,
    CRITERIA_ADDITIONAL_CONDITION_UNK_43                        = 43,
    CRITERIA_ADDITIONAL_CONDITION_UNK_44                        = 44,
    CRITERIA_ADDITIONAL_CONDITION_UNK_45                        = 45,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_HEALTH_PERCENT_BELOW   = 46,
    CRITERIA_ADDITIONAL_CONDITION_UNK_47                        = 47,
    CRITERIA_ADDITIONAL_CONDITION_UNK_48                        = 48,
    CRITERIA_ADDITIONAL_CONDITION_UNK_49                        = 49,
    CRITERIA_ADDITIONAL_CONDITION_UNK_50                        = 50,
    CRITERIA_ADDITIONAL_CONDITION_UNK_51                        = 51,
    CRITERIA_ADDITIONAL_CONDITION_UNK_52                        = 52,
    CRITERIA_ADDITIONAL_CONDITION_UNK_53                        = 53,
    CRITERIA_ADDITIONAL_CONDITION_UNK_54                        = 54,
    CRITERIA_ADDITIONAL_CONDITION_UNK55                         = 55,
    CRITERIA_ADDITIONAL_CONDITION_MIN_ACHIEVEMENT_POINTS        = 56,
    CRITERIA_ADDITIONAL_CONDITION_UNK_57                        = 57,
    CRITERIA_ADDITIONAL_CONDITION_REQUIRES_LFG_GROUP            = 58,
    CRITERIA_ADDITIONAL_CONDITION_UNK_59                        = 59,
    CRITERIA_ADDITIONAL_CONDITION_UNK60                         = 60,
    CRITERIA_ADDITIONAL_CONDITION_REQUIRES_GUILD_GROUP          = 61,
    CRITERIA_ADDITIONAL_CONDITION_GUILD_REPUTATION              = 62,
    CRITERIA_ADDITIONAL_CONDITION_RATED_BATTLEGROUND            = 63,
    CRITERIA_ADDITIONAL_CONDITION_RATED_BATTLEGROUND_RATING     = 64,
    CRITERIA_ADDITIONAL_CONDITION_PROJECT_RARITY                = 65,
    CRITERIA_ADDITIONAL_CONDITION_PROJECT_RACE                  = 66,
    CRITERIA_ADDITIONAL_CONDITION_WORLD_STATE                   = 67,
    CRITERIA_ADDITIONAL_CONDITION_MAP_DIFFICULTY                = 68,
    CRITERIA_ADDITIONAL_CONDITION_PLAYER_LEVEL                  = 69,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_MIN_LEVEL              = 70,
    CRITERIA_ADDITIONAL_CONDITION_TARGET_MAX_LEVEL              = 71,
    CRITERIA_ADDITIONAL_CONDITION_UNK_72                        = 72,
    CRITERIA_ADDITIONAL_CONDITION_UNK73                         = 73,   ///< References another modifier tree id
    CRITERIA_ADDITIONAL_CONDITION_ACTIVE_SCENARIO               = 74,
    CRITERIA_ADDITIONAL_CONDITION_UNK_75                        = 75,
    CRITERIA_ADDITIONAL_CONDITION_ACHIEV_POINTS                 = 76,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK_77                        = 77,
    CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_FAMILY              = 78,
    CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_HP_LOW_THAT         = 79,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK80                         = 80,   ///< Something to do with world bosses
    CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_MASTER_PET_TAMER    = 81,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_ENTRY_ID           = 82,   ///< Some sort of data id?
    CRITERIA_ADDITIONAL_CONDITION_CHALLENGE_MODE_MEDAL          = 83,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_ID                 = 84,   ///< NYI Quest id
    CRITERIA_ADDITIONAL_CONDITION_UNK_85                        = 85,
    CRITERIA_ADDITIONAL_CONDITION_HAS_ACHIEVEMENT               = 86,
    CRITERIA_ADDITIONAL_CONDITION_UNK87                         = 87,   ///< Achievement id
    CRITERIA_ADDITIONAL_CONDITION_UNK_88                        = 88,
    CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_QUALITY             = 89,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_WIN_IN_PVP          = 90,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_SPECIES            = 91,
    CRITERIA_ADDITIONAL_CONDITION_EXPANSION_LESS                = 92,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK_93                        = 93,
    CRITERIA_ADDITIONAL_CONDITION_UNK_94                        = 94,
    CRITERIA_ADDITIONAL_CONDITION_REPUTATION                    = 95,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_ITEM_CLASS_AND_SUBCLASS       = 96,   ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_HAS_QUEST_REWARDED            = 110,
    CRITERIA_ADDITIONAL_CONDITION_CURRENCY                      = 121,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_DEATH_COUNTER                 = 122,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_ARENA_SEASON                  = 125,
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_ENTRY       = 144,
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_QUALITY     = 145,
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_LEVEL       = 146,
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_RARE_MISSION         = 147,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_BUILDING_LEVEL       = 149,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK_151                       = 151,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK_152                       = 152,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK_153                       = 153,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK_154                       = 154,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_UNK_155                       = 155,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_ENTRY_2     = 157,
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_MISSION_TYPE         = 167,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_PLAYER_ITEM_LEVEL             = 169,  ///< NYI
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_ILVL        = 184,
    CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_TYPE        = 187,
    CRITERIA_ADDITIONAL_CONDITION_HONOR_LEVEL                   = 193,
    CRITERIA_ADDITIONAL_CONDITION_PRESTIGE_LEVEL                = 194,
    CRITERIA_ADDITIONAL_CONDITION_HAS_GARRISON_TALENT           = 202,
    CRITERIA_ADDITIONAL_CONDITION_WORLD_QUEST_TYPE              = 206,
    CRITERIA_ADDITIONAL_CONDITION_UNK_216                       = 216  ///< NYI
};

enum CriteriaFlags
{
    CRITERIA_FLAG_SHOW_PROGRESS_BAR = 0x00000001,   // Show progress as bar
    CRITERIA_FLAG_HIDDEN            = 0x00000002,   // Not show criteria in client
    CRITERIA_FLAG_FAIL_ACHIEVEMENT  = 0x00000004,   // BG related??
    CRITERIA_FLAG_RESET_ON_START    = 0x00000008,   //
    CRITERIA_FLAG_IS_DATE           = 0x00000010,   // not used
    CRITERIA_FLAG_MONEY_COUNTER     = 0x00000020,   // Displays counter as money
    CRITERIA_FLAG_QUEST             = 0x00001000    // For quest
};

enum CriteriaTimedTypes
{
    CRITERIA_TIMED_TYPE_EVENT           = 1,    // Timer is started by internal event with id in timerStartEvent
    CRITERIA_TIMED_TYPE_QUEST           = 2,    // Timer is started by accepting quest with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_SPELL_CASTER    = 5,    // Timer is started by casting a spell with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_SPELL_TARGET    = 6,    // Timer is started by being target of spell with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_CREATURE        = 7,    // Timer is started by killing creature with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_SPELL_TARGET2   = 8,    // Timer is started by being target of spell with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_ITEM            = 9,    // Timer is started by using item with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_CREATURE2       = 10,   // Timer is started by killing creature with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_BATTLEPET       = 11,   // Timer is started by win in battlepet to first lose
    CRITERIA_TIMED_TYPE_ITEM2           = 12,   // Timer is started by using item with entry in timerStartEvent
    CRITERIA_TIMED_TYPE_EVENT2          = 13,   // Timer is started by internal event with id in timerStartEvent
    CRITERIA_TIMED_TYPE_SCENARIO_STAGE  = 14,   // Timer is started by changing stages in a scenario

    CRITERIA_TIMED_TYPE_MAX
};

enum CriteriaTypes
{
    CRITERIA_TYPE_KILL_CREATURE                         = 0,
    CRITERIA_TYPE_WIN_BG                                = 1,
    CRITERIA_TYPE_UNK_2                                 = 2,
    CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS         = 3, // struct { uint32 itemCount; }
    CRITERIA_TYPE_SURVEY_GAMEOBJECT                     = 4,
    CRITERIA_TYPE_REACH_LEVEL                           = 5,
    CRITERIA_TYPE_CLEAR_DIGSITE                         = 6,
    CRITERIA_TYPE_REACH_SKILL_LEVEL                     = 7,
    CRITERIA_TYPE_COMPLETE_ACHIEVEMENT                  = 8,
    CRITERIA_TYPE_COMPLETE_QUEST_COUNT                  = 9,
    CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY            = 10, // you have to complete a daily quest x times in a row
    CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE               = 11,
    CRITERIA_TYPE_CURRENCY                              = 12,
    CRITERIA_TYPE_DAMAGE_DONE                           = 13,
    CRITERIA_TYPE_COMPLETE_DAILY_QUEST                  = 14,
    CRITERIA_TYPE_COMPLETE_BATTLEGROUND                 = 15,
    CRITERIA_TYPE_DEATH_AT_MAP                          = 16,
    CRITERIA_TYPE_DEATH                                 = 17,
    CRITERIA_TYPE_DEATH_IN_DUNGEON                      = 18,
    CRITERIA_TYPE_COMPLETE_RAID                         = 19,
    CRITERIA_TYPE_KILLED_BY_CREATURE                    = 20,
    CRITERIA_TYPE_MANUAL_COMPLETE_CRITERIA              = 21,
    CRITERIA_TYPE_COMPLETE_CHALLENGE                    = 22,
    CRITERIA_TYPE_KILLED_BY_PLAYER                      = 23,
    CRITERIA_TYPE_FALL_WITHOUT_DYING                    = 24,
    CRITERIA_TYPE_UNK_25                                = 25,
    CRITERIA_TYPE_DEATHS_FROM                           = 26,
    CRITERIA_TYPE_COMPLETE_QUEST                        = 27,
    CRITERIA_TYPE_BE_SPELL_TARGET                       = 28,
    CRITERIA_TYPE_CAST_SPELL                            = 29,
    CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE                  = 30,
    CRITERIA_TYPE_HONORABLE_KILL_AT_AREA                = 31,
    CRITERIA_TYPE_WIN_ARENA                             = 32,
    CRITERIA_TYPE_PLAY_ARENA                            = 33,
    CRITERIA_TYPE_LEARN_SPELL                           = 34,
    CRITERIA_TYPE_HONORABLE_KILL                        = 35,
    CRITERIA_TYPE_OWN_ITEM                              = 36,
    CRITERIA_TYPE_WIN_RATED_ARENA                       = 37,
    CRITERIA_TYPE_HIGHEST_TEAM_RATING                   = 38,
    CRITERIA_TYPE_HIGHEST_PERSONAL_RATING               = 39,
    CRITERIA_TYPE_LEARN_SKILL_LEVEL                     = 40,
    CRITERIA_TYPE_USE_ITEM                              = 41,
    CRITERIA_TYPE_LOOT_ITEM                             = 42,
    CRITERIA_TYPE_EXPLORE_AREA                          = 43,
    CRITERIA_TYPE_OWN_RANK                              = 44,
    CRITERIA_TYPE_BUY_BANK_SLOT                         = 45,
    CRITERIA_TYPE_GAIN_REPUTATION                       = 46,
    CRITERIA_TYPE_GAIN_EXALTED_REPUTATION               = 47,
    CRITERIA_TYPE_VISIT_BARBER_SHOP                     = 48,
    CRITERIA_TYPE_EQUIP_EPIC_ITEM                       = 49,
    CRITERIA_TYPE_ROLL_NEED_ON_LOOT                     = 50, /// @todo itemlevel is mentioned in text but not present in db2
    CRITERIA_TYPE_ROLL_GREED_ON_LOOT                    = 51,
    CRITERIA_TYPE_HK_CLASS                              = 52,
    CRITERIA_TYPE_HK_RACE                               = 53,
    CRITERIA_TYPE_DO_EMOTE                              = 54,
    CRITERIA_TYPE_HEALING_DONE                          = 55,
    CRITERIA_TYPE_GET_KILLING_BLOWS                     = 56, /// @todo in some cases map not present, and in some cases need do without die
    CRITERIA_TYPE_EQUIP_ITEM                            = 57,
    CRITERIA_TYPE_UNK_58                                = 58,
    CRITERIA_TYPE_MONEY_FROM_VENDORS                    = 59,
    CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS                = 60,
    CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS               = 61,
    CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD               = 62,
    CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING             = 63,
    CRITERIA_TYPE_DEFEAT_CREATURE_GROUP                 = 64,
    CRITERIA_TYPE_GOLD_SPENT_AT_BARBER                  = 65,
    CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL                   = 66,
    CRITERIA_TYPE_LOOT_MONEY                            = 67,
    CRITERIA_TYPE_USE_GAMEOBJECT                        = 68,
    CRITERIA_TYPE_BE_SPELL_TARGET2                      = 69,
    CRITERIA_TYPE_SPECIAL_PVP_KILL                      = 70,
    CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE               = 71,
    CRITERIA_TYPE_FISH_IN_GAMEOBJECT                    = 72,
    CRITERIA_TYPE_SEND_EVENT                            = 73,
    CRITERIA_TYPE_ON_LOGIN                              = 74,
    CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS                = 75,
    CRITERIA_TYPE_WIN_DUEL                              = 76,
    CRITERIA_TYPE_LOSE_DUEL                             = 77,
    CRITERIA_TYPE_KILL_CREATURE_TYPE                    = 78,
    CRITERIA_TYPE_COOK_RECIPES_GUILD                    = 79,
    CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS               = 80,
    CRITERIA_TYPE_EARN_PET_BATTLE_ACHIEVEMENT_POINTS    = 81,
    CRITERIA_TYPE_CREATE_AUCTION                        = 82,
    CRITERIA_TYPE_HIGHEST_AUCTION_BID                   = 83,
    CRITERIA_TYPE_WON_AUCTIONS                          = 84,
    CRITERIA_TYPE_HIGHEST_AUCTION_SOLD                  = 85,
    CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED              = 86,
    CRITERIA_TYPE_GAIN_REVERED_REPUTATION               = 87,
    CRITERIA_TYPE_GAIN_HONORED_REPUTATION               = 88,
    CRITERIA_TYPE_KNOWN_FACTIONS                        = 89,
    CRITERIA_TYPE_LOOT_EPIC_ITEM                        = 90,
    CRITERIA_TYPE_RECEIVE_EPIC_ITEM                     = 91,
    CRITERIA_TYPE_SEND_EVENT_SCENARIO                   = 92,
    CRITERIA_TYPE_ROLL_NEED                             = 93,
    CRITERIA_TYPE_ROLL_GREED                            = 94,
    CRITERIA_TYPE_RELEASE_SPIRIT                        = 95,
    CRITERIA_TYPE_OWN_PET                               = 96,
    CRITERIA_TYPE_GARRISON_COMPLETE_DUNGEON_ENCOUNTER   = 97,
    CRITERIA_TYPE_UNK_98                                = 98,
    CRITERIA_TYPE_UNK_99                                = 99,
    CRITERIA_TYPE_UNK_100                               = 100,
    CRITERIA_TYPE_HIGHEST_HIT_DEALT                     = 101,
    CRITERIA_TYPE_HIGHEST_HIT_RECEIVED                  = 102,
    CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED                 = 103,
    CRITERIA_TYPE_HIGHEST_HEAL_CAST                     = 104,
    CRITERIA_TYPE_TOTAL_HEALING_RECEIVED                = 105,
    CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED              = 106,
    CRITERIA_TYPE_QUEST_ABANDONED                       = 107,
    CRITERIA_TYPE_FLIGHT_PATHS_TAKEN                    = 108,
    CRITERIA_TYPE_LOOT_TYPE                             = 109,
    CRITERIA_TYPE_CAST_SPELL2                           = 110, /// @todo target entry is missing
    CRITERIA_TYPE_UNK_111                               = 111,
    CRITERIA_TYPE_LEARN_SKILL_LINE                      = 112,
    CRITERIA_TYPE_EARN_HONORABLE_KILL                   = 113,
    CRITERIA_TYPE_ACCEPTED_SUMMONINGS                   = 114,
    CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS               = 115,
    CRITERIA_TYPE_UNK_116                               = 116,
    CRITERIA_TYPE_UNK_117                               = 117,
    CRITERIA_TYPE_COMPLETE_LFG_DUNGEON                  = 118,
    CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS         = 119,
    CRITERIA_TYPE_LFG_VOTE_KICKS_INITIATED_BY_PLAYER    = 120,
    CRITERIA_TYPE_LFG_VOTE_KICKS_NOT_INIT_BY_PLAYER     = 121,
    CRITERIA_TYPE_BE_KICKED_FROM_LFG                    = 122,
    CRITERIA_TYPE_LFG_LEAVES                            = 123,
    CRITERIA_TYPE_SPENT_GOLD_GUILD_REPAIRS              = 124,
    CRITERIA_TYPE_REACH_GUILD_LEVEL                     = 125,
    CRITERIA_TYPE_CRAFT_ITEMS_GUILD                     = 126,
    CRITERIA_TYPE_CATCH_FROM_POOL                       = 127,
    CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS                  = 128,
    CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS         = 129,
    CRITERIA_TYPE_WIN_RATED_BATTLEGROUND                = 130,
    CRITERIA_TYPE_UNK_131                               = 131,
    CRITERIA_TYPE_REACH_RATED_BATTLEGROUND_RATING       = 132,
    CRITERIA_TYPE_BUY_GUILD_TABARD                      = 133,
    CRITERIA_TYPE_COMPLETE_QUESTS_GUILD                 = 134,
    CRITERIA_TYPE_HONORABLE_KILLS_GUILD                 = 135,
    CRITERIA_TYPE_KILL_CREATURE_TYPE_GUILD              = 136,
    CRITERIA_TYPE_COUNT_OF_LFG_QUEUE_BOOSTS_BY_TANK     = 137,
    CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE         = 138, //struct { Flag flag; uint32 count; } 1: Guild Dungeon, 2:Guild Challenge, 3:Guild battlefield
    CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE              = 139, //struct { uint32 count; } Guild Challenge
    CRITERIA_TYPE_UNK_140                               = 140, ///< 1 criteria (16883)
    CRITERIA_TYPE_UNK_141                               = 141, ///< 1 criteria (16884)
    CRITERIA_TYPE_UNK_142                               = 142, ///< 1 criteria (16881)
    CRITERIA_TYPE_UNK_143                               = 143, ///< 1 criteria (16882)
    CRITERIA_TYPE_UNK_144                               = 144, ///< 1 criteria (17386)
    CRITERIA_TYPE_LFR_DUNGEONS_COMPLETED                = 145,
    CRITERIA_TYPE_LFR_LEAVES                            = 146,
    CRITERIA_TYPE_LFR_VOTE_KICKS_INITIATED_BY_PLAYER    = 147,
    CRITERIA_TYPE_LFR_VOTE_KICKS_NOT_INIT_BY_PLAYER     = 148,
    CRITERIA_TYPE_BE_KICKED_FROM_LFR                    = 149,
    CRITERIA_TYPE_COUNT_OF_LFR_QUEUE_BOOSTS_BY_TANK     = 150,
    CRITERIA_TYPE_COMPLETE_SCENARIO_COUNT               = 151,
    CRITERIA_TYPE_COMPLETE_SCENARIO                     = 152,
    CRITERIA_TYPE_REACH_SCENARIO_BOSS                   = 153,
    CRITERIA_TYPE_UNK_154                               = 154,
    CRITERIA_TYPE_OWN_BATTLE_PET                        = 155,
    CRITERIA_TYPE_OWN_BATTLE_PET_COUNT                  = 156,
    CRITERIA_TYPE_CAPTURE_BATTLE_PET                    = 157,
    CRITERIA_TYPE_WIN_PET_BATTLE                        = 158,
    CRITERIA_TYPE_UNK_159                               = 159, ///< 2 criterias(22312, 22314)
    CRITERIA_TYPE_LEVEL_BATTLE_PET                      = 160,
    CRITERIA_TYPE_CAPTURE_BATTLE_PET_CREDIT             = 161, // triggers a quest credit
    CRITERIA_TYPE_LEVEL_BATTLE_PET_CREDIT               = 162, // triggers a quest credit
    CRITERIA_TYPE_ENTER_AREA                            = 163, // triggers a quest credit
    CRITERIA_TYPE_LEAVE_AREA                            = 164, // triggers a quest credit
    CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER            = 165,
    CRITERIA_TYPE_UNK_166                               = 166,
    CRITERIA_TYPE_PLACE_GARRISON_BUILDING               = 167,
    CRITERIA_TYPE_UPGRADE_GARRISON_BUILDING             = 168,
    CRITERIA_TYPE_CONSTRUCT_GARRISON_BUILDING           = 169,
    CRITERIA_TYPE_UPGRADE_GARRISON                      = 170,
    CRITERIA_TYPE_START_GARRISON_MISSION                = 171,
    CRITERIA_TYPE_START_ORDER_HALL_MISSION              = 172,
    CRITERIA_TYPE_COMPLETE_GARRISON_MISSION_COUNT       = 173,
    CRITERIA_TYPE_COMPLETE_GARRISON_MISSION             = 174,
    CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_2           = 175,
    CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER             = 176,
    CRITERIA_TYPE_UNK_177                               = 177,
    CRITERIA_TYPE_LEARN_GARRISON_BLUEPRINT_COUNT        = 178,
    CRITERIA_TYPE_UNK_179                               = 179,
    CRITERIA_TYPE_UNK_180                               = 180,
    CRITERIA_TYPE_UNK_181                               = 181,
    CRITERIA_TYPE_COMPLETE_GARRISON_SHIPMENT            = 182,
    CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_ITEM_LEVEL    = 183,
    CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_LEVEL         = 184,
    CRITERIA_TYPE_OWN_TOY                               = 185,
    CRITERIA_TYPE_OWN_TOY_COUNT                         = 186,
    CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_WITH_QUALITY = 187,
    CRITERIA_TYPE_UNK_188                               = 188,
    CRITERIA_TYPE_OWN_HEIRLOOMS                         = 189,
    CRITERIA_TYPE_ARTIFACT_POWER_EARNED                 = 190,
    CRITERIA_TYPE_ARTIFACT_TRAITS_UNLOCKED              = 191,
    CRITERIA_TYPE_UNK_192                               = 192,
    CRITERIA_TYPE_UNK_193                               = 193,
    CRITERIA_TYPE_HONOR_LEVEL_REACHED                   = 194,
    CRITERIA_TYPE_PRESTIGE_REACHED                      = 195,
    CRITERIA_TYPE_REACH_LEVEL_2                         = 196,
    CRITERIA_TYPE_LEARN_ALL_CLASS_HALL_TALENT           = 197, ///< Research all options in your Class Order Hall Advancement.
    CRITERIA_TYPE_ORDER_HALL_TALENT_LEARNED             = 198,
    CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT           = 199,
    CRITERIA_TYPE_ORDER_HALL_RECRUIT_TROOP              = 200,
    CRITERIA_TYPE_UNK_201                               = 201,
    CRITERIA_TYPE_UNK_202                               = 202,
    CRITERIA_TYPE_COMPLETE_WORLD_QUEST                  = 203,
    CRITERIA_TYPE_ESPORT_AWARD                          = 204,  ///< Special criteria type called World of Warcraft Esports Award
    CRITERIA_TYPE_TRANSMOG_SET_UNLOCKED                 = 205,
    CRITERIA_TYPE_GAIN_PARAGON_REPUTATION               = 206,
    CRITERIA_TYPE_EARN_HONOR_XP                         = 207,
    CRITERIA_TYPE_UNK_208                               = 208,
    CRITERIA_TYPE_UNK_209                               = 209,
    CRITERIA_TYPE_UNK_210                               = 210,
    CRITERIA_TYPE_RELIC_TALENT_UNLOCKED                 = 211,
    CRITERIA_TYPE_UNK_212                               = 212, ///< achievement ID 12457
};

#define CRITERIA_TYPE_TOTAL 213

enum CriteriaTreeFlags : uint16
{
    CRITERIA_TREE_FLAG_PROGRESS_BAR         = 0x0001,
    CRITERIA_TREE_FLAG_PROGRESS_IS_DATE     = 0x0004,
    CRITERIA_TREE_FLAG_SHOW_CURRENCY_ICON   = 0x0008,
    CRITERIA_TREE_FLAG_UNK                  = 0x0080, ///< Related to battle pet master to defeat and lore master
    CRITERIA_TREE_FLAG_GRAND_COMMENDATION   = 0x0100, ///< Related to Grand Commendation it's bind to account ex:"Use the item to get 100% bonus Klaxxi rep on Klaxxi kill"
    CRITERIA_TREE_FLAG_ALLIANCE_ONLY        = 0x0200,
    CRITERIA_TREE_FLAG_HORDE_ONLY           = 0x0400,
    CRITERIA_TREE_FLAG_SHOW_REQUIRED_COUNT  = 0x0800,
    CRITERIA_TREE_FLAG_IS_QUEST_CRITERIA    = 0x1000
};

enum CriteriaTreeOperator : uint8
{
    CRITERIA_TREE_OPERATOR_SINGLE                   = 0,
    CRITERIA_TREE_OPERATOR_SINGLE_NOT_COMPLETED     = 1,
    CRITERIA_TREE_OPERATOR_ALL                      = 4,
    CRITERIA_TREE_OPERAROR_SUM_CHILDREN             = 5,
    CRITERIA_TREE_OPERATOR_MAX_CHILD                = 6,
    CRITERIA_TREE_OPERATOR_COUNT_DIRECT_CHILDREN    = 7,
    CRITERIA_TREE_OPERATOR_ANY                      = 8,
    CRITERIA_TREE_OPERATOR_SUM_CHILDREN_WEIGHT      = 9
};

enum ModifierTreeOperator : uint8
{
    OperatorEqual       = 2,
    OperatorDifferent   = 3,
    OperatorAnd         = 4,
    OperatorSupOrEqual  = 8
};
