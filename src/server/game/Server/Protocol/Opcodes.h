////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _OPCODES_H
#define _OPCODES_H

#include "Common.h"

enum ConnectionType
{
    CONNECTION_TYPE_REALM       = 0,
    CONNECTION_TYPE_INSTANCE    = 1,
    MAX_CONNECTION_TYPES,

    CONNECTION_TYPE_DEFAULT     = -1
};

enum OpcodeTransferDirection : uint8
{
    WOW_SERVER_TO_CLIENT = 0,
    WOW_CLIENT_TO_SERVER = 1,

    TRANSFER_DIRECTION_MAX = 2
};

/// List of Opcodes
enum Opcodes
{
    MAX_OPCODE                                      = 0xFFFF,
    NUM_OPCODE_HANDLERS                             = MAX_OPCODE + 1,
    UNKNOWN_OPCODE                                  = (0xFFFF+1),
    NULL_OPCODE                                     = 0,

    SMSG_COMPRESSED_PACKET                          = 0x3052, ///< 7.3.5 25996
    SMSG_MULTIPLE_PACKETS                           = 0x3051, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Protocol
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamProtocol
        /// Opcodes
        SMSG_AUTH_CHALLENGE                         = 0x3048, ///< 7.3.5 25996
        SMSG_CRYPT_ENABLE                           = 0x3049, ///< 7.3.5 25996
        SMSG_RESUME_COMMS                           = 0x304B, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Dispatch
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamDispatch
        /// Base opcodes
        SMSG_AUTH_RESPONSE                          = 0x256C, ///< 7.3.5 25996
        SMSG_CACHE_VERSION                          = 0x2742, ///< 7.3.5 25996
        SMSG_FEATURE_SYSTEM_STATUS                  = 0x25D1, ///< 7.3.5 25996
        SMSG_FEATURE_SYSTEM_STATUS_GLUE_SCREEN      = 0x25D2, ///< 7.3.5 25996
        SMSG_SET_TIME_ZONE_INFORMATION              = 0x269F, ///< 7.3.5 25996
        SMSG_LOGIN_VERIFY_WORLD                     = 0x25AC, ///< 7.3.5 25996
        SMSG_NEW_WORLD                              = 0x25AB, ///< 7.3.5 25996
        SMSG_LOGIN_SET_TIME_SPEED                   = 0x274C, ///< 7.3.5 25996
        SMSG_AVAILABLE_HOTFIXES                     = 0x25A1, ///< 7.3.5 25996
        SMSG_HOTFIX_RESPONSE                        = 0x25A3, ///< 7.3.5 25996
        SMSG_WORLD_SERVER_INFO                      = 0x25C2, ///< 7.3.5 25996
        SMSG_INITIAL_SETUP                          = 0x257F, ///< 7.3.5 25996
        SMSG_PONG                                   = 0x304E, ///< 7.3.5 25996
        SMSG_UI_TIME                                = 0x2753, ///< 7.3.5 25996
        SMSG_CONNECT_TO                             = 0x304D, ///< 7.3.5 25996

        /// Query
        SMSG_DB_REPLY                               = 0x25A0, ///< 7.3.5 25996
        SMSG_QUERY_CREATURE_RESPONSE                = 0x26FA, ///< 7.3.5 25996
        SMSG_NPC_TEXT_UPDATE                        = 0x26FE, ///< 7.3.5 25996
        SMSG_NAME_QUERY_RESPONSE                    = 0x26FF, ///< 7.3.5 25996
        SMSG_REALM_QUERY_RESPONSE                   = 0x26E5, ///< 7.3.5 25996
        SMSG_GAMEOBJECT_QUERY_RESPONSE              = 0x26FB, ///< 7.3.5 25996
        SMSG_QUERY_TIME_RESPONSE                    = 0x271A, ///< 7.3.5 25996
        SMSG_QUERY_QUEST_REWARD_RESPONSE            = 0x2846, ///< 7.3.5 25996

        /// Misc
        SMSG_ACTIVE_GLYPHS                          = 0x2C53, ///< 7.3.5 25996
        SMSG_DISPLAY_GAME_ERROR                     = 0x25B5, ///< 7.3.5 25996
        SMSG_UPDATE_ACTION_BUTTONS                  = 0x25F5, ///< 7.3.5 25996
        SMSG_SET_PROFICIENCY                        = 0x2776, ///< 7.3.5 25996
        SMSG_INIT_WORLD_STATES                      = 0x278B, ///< 7.3.5 25996
        SMSG_UPDATE_WORLD_STATE                     = 0x278C, ///< 7.3.5 25996
        SMSG_EMOTE                                  = 0x280D, ///< 7.3.5 25996
        SMSG_EXPLORATION_EXPERIENCE                 = 0x27A2, ///< 7.3.5 25996
        SMSG_LOG_XP_GAIN                            = 0x271B, ///< 7.3.5 25996
        SMSG_LEVELUP_INFO                           = 0x271F, ///< 7.3.5 25996
        SMSG_ITEM_PUSH_RESULT                       = 0x2637, ///< 7.3.5 25996
        SMSG_SETUP_CURRENCY                         = 0x2572, ///< 7.3.5 25996
        SMSG_UPDATE_CURRENCY                        = 0x2573, ///< 7.3.5 25996
        SMSG_UPDATE_CURRENCY_WEEK_LIMIT             = 0x25B7, ///< 7.3.5 25996
        SMSG_WEEKLY_RESET_CURRENCY                  = 0x2574, ///< 7.3.5 25996
        SMSG_EQUIPMENT_SET_LIST                     = 0x274D, ///< 7.3.5 25996
        SMSG_BINDER_CONFIRM                         = 0x2739, ///< 7.3.5 25996
        SMSG_BIND_POINT_UPDATE                      = 0x257C, ///< 7.3.5 25996
        SMSG_UPDATE_TALENT_DATA                     = 0x25EB, ///< 7.3.5 25996
        SMSG_TRANSFER_ABORTED                       = 0x2740, ///< 7.3.5 25996
        SMSG_TRANSFER_PENDING                       = 0x25E5, ///< 7.3.5 25996
        SMSG_INVENTORY_CHANGE_FAILURE               = 0x2763, ///< 7.3.5 25996
        SMSG_SET_DUNGEON_DIFFICULTY                 = 0x26CD, ///< 7.3.5 25996
        SMSG_SET_RAID_DIFFICULTY                    = 0x27EF, ///< 7.3.5 25996
        SMSG_LOAD_CUF_PROFILES                      = 0x25CE, ///< 7.3.5 25996
        SMSG_STANDSTATE_UPDATE                      = 0x275B, ///< 7.3.5 25996
        SMSG_START_TIMER                            = 0x25BB, ///< 7.3.5 25996
        SMSG_START_ELAPSED_TIMER                    = 0x261A, ///< 7.3.5 25996
        SMSG_START_ELAPSED_TIMERS                   = 0x261C, ///< 7.3.5 25996
        SMSG_STOP_ELAPSED_TIMER                     = 0x261B, ///< 7.3.5 25996
        SMSG_DEATH_RELEASE_LOC                      = 0x2705, ///< 7.3.5 25996
        SMSG_CORPSE_RECLAIM_DELAY                   = 0x278E, ///< 7.3.5 25996
        SMSG_CORPSE_LOCATION                        = 0x266B, ///< 7.3.5 25996
        SMSG_CORPSE_TRANSPORT_QUERY                 = 0x2751, ///< 7.3.5 25996
        SMSG_DURABILITY_DAMAGE_DEATH                = 0x278A, ///< 7.3.5 25996
        SMSG_PLAY_MUSIC                             = 0x27AB, ///< 7.3.5 25996
        SMSG_PLAY_OBJECT_SOUND                      = 0x27AC, ///< 7.3.5 25996
        SMSG_PLAY_SOUND                             = 0x27AA, ///< 7.3.5 25996
        SMSG_DISPLAY_TOAST                          = 0x2638, ///< 7.3.5 25996
        SMSG_ITEM_ENCHANT_TIME_UPDATE               = 0x2797, ///< 7.3.5 25996
        SMSG_ENCHANTMENT_LOG                        = 0x2752, ///< 7.3.5 25996
        SMSG_PARTY_KILL_LOG                         = 0x279D, ///< 7.3.5 25996
        SMSG_PROCRESIST                             = 0x279E, ///< 7.3.5 25996
        SMSG_CLEAR_TARGET                           = 0x26DB, ///< 7.3.5 25996
        SMSG_WEATHER                                = 0x26CF, ///< 7.3.5 25996
        SMSG_PHASE_SHIFT_CHANGE                     = 0x2577, ///< 7.3.5 25996
        SMSG_TRIGGER_CINEMATIC                      = 0x280E, ///< 7.3.5 25996
        SMSG_TEMPSUMMON_UI_TIMER                    = 0x26F2, ///< 7.3.5 25996
        SMSG_RESPEC_WIPE_CONFIRM                    = 0x2626, ///< 7.3.5 25996
        SMSG_USE_EQUIPMENT_SET_RESULT               = 0x2792, ///< 7.3.5 25996
        SMSG_TRADE_STATUS                           = 0x2581, ///< 7.3.5 25996
        SMSG_TRADE_UPDATED                          = 0x2580, ///< 7.3.5 25996
        SMSG_SHOW_NEUTRAL_PLAYER_FACTION_SELECT_UI  = 0x25F0, ///< 7.3.5 25996
        SMSG_NEUTRAL_PLAYER_FACTION_SELECT_RESULT   = 0x25F1, ///< 7.3.5 25996
        SMSG_INSPECT_TALENT                         = 0x264D, ///< 7.3.5 25996
        SMSG_INSPECT_HONOR_STATS                    = 0x25B2, ///< 7.3.5 25996
        SMSG_INSPECT_PVP                            = 0x2761, ///< 7.3.5 25996
        SMSG_TIME_SYNC_REQUEST                      = 0x2DA0, ///< 7.3.5 25996
        SMSG_SUSPEND_TOKEN                          = 0x25BD, ///< 7.3.5 25996
        SMSG_RESUME_TOKEN                           = 0x25BE, ///< 7.3.5 25996
        SMSG_BARBER_SHOP_RESULT                     = 0x26E8, ///< 7.3.5 25996
        SMSG_ENABLE_BARBER_SHOP                     = 0x26E7, ///< 7.3.5 25996
        SMSG_PAGE_TEXT_QUERY_RESPONSE               = 0x2700, ///< 7.3.5 25996
        SMSG_PAGE_TEXT                              = 0x2759, ///< 7.3.5 25996
        SMSG_READ_ITEM_FAILED                       = 0x27EB, ///< 7.3.5 25996
        SMSG_READ_ITEM_OK                           = 0x27E1, ///< 7.3.5 25996
        SMSG_TEXT_EMOTE                             = 0x26A3, ///< 7.3.5 25996
        SMSG_TITLE_EARNED                           = 0x270A, ///< 7.3.5 25996
        SMSG_TITLE_LOST                             = 0x270B, ///< 7.3.5 25996
        SMSG_WHOIS                                  = 0x26CE, ///< 7.3.5 25996
        SMSG_TRIGGER_MOVIE                          = 0x26F4, ///< 7.3.5 25996
        SMSG_UPDATE_LAST_INSTANCE                   = 0x26B1, ///< 7.3.5 25996
        SMSG_UPDATE_INSTANCE_OWNERSHIP              = 0x26D0, ///< 7.3.5 25996
        SMSG_SUMMON_REQUEST                         = 0x2760, ///< 7.3.5 25996
        SMSG_START_MIRROR_TIMER                     = 0x274E, ///< 7.3.5 25996
        SMSG_PAUSE_MIRROR_TIMER                     = 0x274F, ///< 7.3.5 25996
        SMSG_STOP_MIRROR_TIMER                      = 0x2750, ///< 7.3.5 25996
        SMSG_SOCKET_GEMS                            = 0x2768, ///< 7.3.5 25996
        SMSG_RESURRECT_REQUEST                      = 0x257E, ///< 7.3.5 25996
        SMSG_RESPOND_INSPECT_ACHIEVEMENTS           = 0x2571, ///< 7.3.5 25996
        SMSG_RESET_FAILED_NOTIFY                    = 0x26E1, ///< 7.3.5 25996
        SMSG_SETUP_RESEARCH_HISTORY                 = 0x2584, ///< 7.3.5 25996
        SMSG_RESEARCH_COMPLETE                      = 0x2585, ///< 7.3.5 25996
        SMSG_REQUEST_CEMETERY_LIST_RESPONSE         = 0x259D, ///< 7.3.5 25996
        SMSG_PLAYED_TIME                            = 0x2708, ///< 7.3.5 25996
        SMSG_BREAK_TARGET                           = 0x266C, ///< 7.3.5 25996
        SMSG_CANCEL_AUTO_REPEAT                     = 0x2712, ///< 7.3.5 25996
        SMSG_CANCEL_COMBAT                          = 0x2731, ///< 7.3.5 25996
        SMSG_COMPLAIN_RESULT                        = 0x26D3, ///< 7.3.5 25996
        SMSG_XP_GAIN_ABORTED                        = 0x25E0, ///< 7.3.5 25996
        SMSG_SUPERCEDED_SPELLS                      = 0x2C4C, ///< 7.3.5 25996
        SMSG_PVP_CREDIT                             = 0x2716, ///< 7.3.5 25996
        SMSG_PRE_RESURRECT                          = 0x27A9, ///< 7.3.5 25996
        SMSG_PLAY_ONE_SHOT_ANIM_KIT                 = 0x2772, ///< 7.3.5 25996
        SMSG_SET_AI_ANIM_KIT                        = 0x2771, ///< 7.3.5 25996
        SMSG_SET_MELEE_ANIM_KIT                     = 0x2774, ///< 7.3.5 25996
        SMSG_SET_ANIM_TIER                          = 0x2775, ///< 7.3.5 25996
        SMSG_SET_PLAY_HOVER_ANIM                    = 0x25CC, ///< 7.3.5 25996
        SMSG_SET_MOVEMENT_ANIM_KIT                  = 0x2773, ///< 7.3.5 25996
        SMSG_PLAYER_BOUND                           = 0x257D, ///< 7.3.5 25996
        SMSG_OVERRIDE_LIGHT                         = 0x26E6, ///< 7.3.5 25996
        SMSG_PRINT_NOTIFICATION                     = 0x25E1, ///< 7.3.5 25996
        SMSG_SPECIAL_MOUNT_ANIM                     = 0x26C8, ///< 7.3.5 25996
        SMSG_ITEM_TIME_UPDATE                       = 0x2796, ///< 7.3.5 25996
        SMSG_QUERY_ITEM_TEXT_RESPONSE               = 0x280A, ///< 7.3.5 25996
        SMSG_ITEM_PURCHASE_REFUND_RESULT            = 0x25AF, ///< 7.3.5 25996
        SMSG_GAMEOBJECT_DESPAWN                     = 0x25D8, ///< 7.3.5 25996
        SMSG_GAMEOBJECT_CUSTOM_ANIM                 = 0x25D7, ///< 7.3.5 25996
        SMSG_GAME_OBJECT_UI_ACTION                  = 0x275A, ///< 7.3.5 25996
        SMSG_GAME_OBJECT_ACTIVATE_ANIM_KIT          = 0x25D6, ///< 7.3.5 25996
        SMSG_FISH_NOT_HOOKED                        = 0x26F8, ///< 7.3.5 25996
        SMSG_FISH_ESCAPED                           = 0x26F9, ///< 7.3.5 25996
        SMSG_DESTRUCTIBLE_BUILDING_DAMAGE           = 0x2732, ///< 7.3.5 25996
        SMSG_CROSSED_INEBRIATION_THRESHOLD          = 0x26EC, ///< 7.3.5 25996
        SMSG_CLIENT_CONTROL_UPDATE                  = 0x2664, ///< 7.3.5 25996
        SMSG_AREA_TRIGGER_NO_CORPSE                 = 0x2755, ///< 7.3.5 25996
        SMSG_SORT_BAGS_RESULT                       = 0x2824, ///< 7.3.5 25996
        SMSG_VIGNETTE_UPDATE                        = 0x27B0, ///< 7.3.5 25996
        SMSG_ACCOUNT_MOUNT_UPDATE                   = 0x25C3, ///< 7.3.5 25996
        SMSG_PLAY_SCENE                             = 0x2653, ///< 7.3.5 25996
        SMSG_CANCEL_SCENE                           = 0x2654, ///< 7.3.5 25996
        SMSG_TALENTS_INVOLUNTARILY_RESET            = 0x2756, ///< 7.3.5 25996
        SMSG_SET_ITEM_PURCHASE_DATA                 = 0x25B0, ///< 7.3.5 25996
        SMSG_SHOW_TRADE_SKILL_RESPONSE              = 0x27B2, ///< 7.3.5 25996
        SMSG_FORCED_DEATH_UPDATE                    = 0x2706, ///< 7.3.5 25996
        SMSG_TRANSMOG_COLLECTION_UPDATE             = 0x25C6, ///< 7.3.5 25996
        SMSG_GET_DISPLAYED_TROPHY_LIST_RESPONSE     = 0x2928, ///< 7.3.5 25996
        SMSG_CHANGE_ITEM                            = 0x2720, ///< 7.3.5 25996
        SMSG_UPDATE_CELESTIAL_BODY                  = 0x285E, ///< 7.3.5 26365

        /// Control Alert
        SMSG_LOSS_OF_CONTROL_AURA_UPDATE            = 0x2695, ///< 7.3.5 25996
        SMSG_ADD_LOSS_OF_CONTROL                    = 0x2696, ///< 7.3.5 25996
        SMSG_CLEAR_LOSS_OF_CONTROL                  = 0x2697, ///< 7.3.5 25996
        SMSG_REMOVE_LOSS_OF_CONTROL                 = 0x2698, ///< 7.3.5 25996

        /// Reputations
        SMSG_INITIALIZE_FACTIONS                    = 0x2765, ///< 7.3.5 25996
        SMSG_FACTION_BONUS_INFO                     = 0x2766, ///< 7.3.5 25996
        SMSG_SET_FACTION_VISIBLE                    = 0x276B, ///< 7.3.5 25996
        SMSG_SET_FACTION_NOT_VISIBLE                = 0x276C, ///< 7.3.5 25996
        SMSG_SET_FACTION_STANDING                   = 0x276D, ///< 7.3.5 25996
        SMSG_SET_FACTION_ATWAR                      = 0x273C, ///< 7.3.5 25996
        SMSG_SET_FORCED_REACTIONS                   = 0x275C, ///< 7.3.5 25996

        /// Interaction
        SMSG_LOGOUT_RESPONSE                        = 0x26AC, ///< 7.3.5 25996
        SMSG_LOGOUT_CANCEL_ACK                      = 0x26AE, ///< 7.3.5 25996
        SMSG_LOGOUT_COMPLETE                        = 0x26AD, ///< 7.3.5 25996
        SMSG_GOSSIP_POI                             = 0x27D8, ///< 7.3.5 25996
        SMSG_ARCHAEOLOGY_SURVERY_CAST               = 0x2586, ///< 7.3.5 25996

        /// World Object management
        SMSG_UPDATE_OBJECT                          = 0x280F, ///< 7.3.5 25996
        SMSG_POWER_UPDATE                           = 0x26FD, ///< 7.3.5 25996
        SMSG_ADJUST_SPLINE_DURATION                 = 0x25E8, ///< 7.3.5 25996

        /// Character list
        SMSG_ENUM_CHARACTERS_RESULT                 = 0x2582, ///< 7.3.5 25996
        SMSG_CREATE_CHAR                            = 0x273E, ///< 7.3.5 25996
        SMSG_CHAR_DELETE                            = 0x273F, ///< 7.3.5 25996
        SMSG_SET_PLAYER_DECLINED_NAMES_RESULT       = 0x2707, ///< 7.3.5 25996
        SMSG_CHAR_FACTION_CHANGE_RESULT             = 0x27EE, ///< 7.3.5 25996
        SMSG_CHAR_RENAME                            = 0x27A5, ///< 7.3.5 25996
        SMSG_CHAR_CUSTOMIZE_FAILED                  = 0x2718, ///< 7.3.5 25996
        SMSG_CHAR_CUSTOMIZE                         = 0x2719, ///< 7.3.5 25996
        SMSG_GENERATE_RANDOM_CHARACTER_NAME_RESULT  = 0x2583, ///< 7.3.5 25996
        SMSG_GET_ACCOUNT_CHARACTER_LIST_RESULT      = 0x27A3, ///< 7.3.5 25996

        /// Account data
        SMSG_ACCOUNT_DATA_TIMES                     = 0x2749, ///< 7.3.5 25996
        SMSG_UPDATE_ACCOUNT_DATA                    = 0x2748, ///< 7.3.5 25996
        SMSG_TUTORIAL_FLAGS                         = 0x2800, ///< 7.3.5 25996

        /// Combat
        SMSG_ATTACKER_STATE_UPDATE                  = 0x27CF, ///< 7.3.5 25996
        SMSG_ATTACK_START                           = 0x266D, ///< 7.3.5 25996
        SMSG_ATTACK_STOP                            = 0x266E, ///< 7.3.5 25996
        SMSG_AI_REACTION                            = 0x26DF, ///< 7.3.5 25996
        SMSG_ATTACK_SWING_ERROR                     = 0x2733, ///< 7.3.5 25996
        SMSG_ATTACK_SWING_LANDED_LOG                = 0x2734, ///< 7.3.5 25996

        /// Duel
        SMSG_DUEL_REQUESTED                         = 0x2670, ///< 7.3.5 25996
        SMSG_DUEL_COUNTDOWN                         = 0x2673, ///< 7.3.5 25996
        SMSG_DUEL_COMPLETE                          = 0x2674, ///< 7.3.5 25996
        SMSG_DUEL_INBOUNDS                          = 0x2672, ///< 7.3.5 25996
        SMSG_DUEL_OUT_OF_BOUNDS                     = 0x2671, ///< 7.3.5 25996
        SMSG_DUEL_WINNER                            = 0x2675, ///< 7.3.5 25996
        SMSG_CAN_DUEL_RESULT                        = 0x2676, ///< 7.3.5 25996

        /// Vendor
        SMSG_LIST_INVENTORY                         = 0x25CA, ///< 7.3.5 25996
        SMSG_BUY_FAILED                             = 0x26F1, ///< 7.3.5 25996
        SMSG_BUY_ITEM                               = 0x26F0, ///< 7.3.5 25996
        SMSG_SELL_ITEM                              = 0x26EF, ///< 7.3.5 25996

        /// Achievement
        SMSG_ALL_ACHIEVEMENT_DATA                   = 0x256F, ///< 7.3.5 25996
        SMSG_ALL_ACCOUNT_CRITERIA                   = 0x2570, ///< 7.3.5 25996
        SMSG_ACHIEVEMENT_DELETED                    = 0x271E, ///< 7.3.5 25996
        SMSG_ACHIEVEMENT_EARNED                     = 0x2660, ///< 7.3.5 25996
        SMSG_ACCOUNT_CRITERIA_UPDATE                = 0x2652, ///< 7.3.5 25996
        SMSG_CRITERIA_DELETED                       = 0x271D, ///< 7.3.5 25996
        SMSG_CRITERIA_UPDATE                        = 0x2717, ///< 7.3.5 25996

        /// Friends
        SMSG_CONTACT_LIST                           = 0x27CA, ///< 7.3.5 25996
        SMSG_FRIEND_STATUS                          = 0x27CB, ///< 7.3.5 25996

        /// Taxi
        SMSG_TAXI_NODE_STATUS                       = 0x26A5, ///< 7.3.5 25996
        SMSG_NEW_TAXI_PATH                          = 0x26A7, ///< 7.3.5 25996
        SMSG_SHOW_TAXI_NODES                        = 0x26F6, ///< 7.3.5 25996
        SMSG_ACTIVATE_TAXI_REPLY                    = 0x26A6, ///< 7.3.5 25996

        /// Loot
        SMSG_LOOT_LIST                              = 0x2783, ///< 7.3.5 25996
        SMSG_LOOT_RESPONSE                          = 0x2628, ///< 7.3.5 25996
        SMSG_LOOT_MONEY_NOTIFY                      = 0x2630, ///< 7.3.5 25996
        SMSG_LOOT_RELEASE                           = 0x262F, ///< 7.3.5 25996
        SMSG_LOOT_RELEASE_ALL                       = 0x262E, ///< 7.3.5 25996
        SMSG_LOOT_REMOVED                           = 0x2629, ///< 7.3.5 25996
        SMSG_COIN_REMOVED                           = 0x262B, ///< 7.3.5 25996
        SMSG_LOOT_ALL_PASSED                        = 0x2635, ///< 7.3.5 25996
        SMSG_MASTER_LOOT_CANDIDATE_LIST             = 0x2633, ///< 7.3.5 25996
        SMSG_LOOT_ROLL                              = 0x2632, ///< 7.3.5 25996
        SMSG_LOOT_ROLLS_COMPLETE                    = 0x2634, ///< 7.3.5 25996
        SMSG_LOOT_ROLL_WON                          = 0x2636, ///< 7.3.5 25996
        SMSG_LOOT_START_ROLL                        = 0x2631, ///< 7.3.5 25996
        SMSG_AE_LOOT_TARGETS                        = 0x262C, ///< 7.3.5 25996
        SMSG_AE_LOOT_TARGETS_ACK                    = 0x262D, ///< 7.3.5 25996

        /// Bank
        SMSG_SHOW_BANK                              = 0x26A8, ///< 7.3.5 25996

        /// Rune
        SMSG_RESYNC_RUNES                           = 0x273D, ///< 7.3.5 25996
        SMSG_RUNE_REGEN_DEBUG                       = 0x25C8, ///< 7.3.5 25996
        SMSG_ADD_RUNE_POWER                         = 0x26E2, ///< 7.3.5 25996

        /// Group
        SMSG_PARTY_UPDATE                           = 0x260B, ///< 7.3.5 25996
        SMSG_PARTY_MEMBER_STATE_FULL                = 0x279B, ///< 7.3.5 25996
        SMSG_PARTY_MEMBER_STATE_PARTIAL             = 0x279A, ///< 7.3.5 25996
        SMSG_PARTY_INVITE                           = 0x25CF, ///< 7.3.5 25996
        SMSG_GROUP_DECLINE                          = 0x27D3, ///< 7.3.5 25996
        SMSG_PARTY_COMMAND_RESULT                   = 0x27D7, ///< 7.3.5 25996
        SMSG_GROUP_UNINVITE                         = 0x27D4, ///< 7.3.5 25996
        SMSG_GROUP_NEW_LEADER                       = 0x2649, ///< 7.3.5 25996
        SMSG_MINIMAP_PING                           = 0x26F7, ///< 7.3.5 25996
        SMSG_SET_LOOT_METHOD_FAILED                 = 0x2816, ///< 7.3.5 25996
        SMSG_RANDOM_ROLL                            = 0x264C, ///< 7.3.5 25996
        SMSG_SEND_RAID_TARGET_UPDATE_SINGLE         = 0x264B, ///< 7.3.5 25996
        SMSG_SEND_RAID_TARGET_UPDATE_ALL            = 0x264A, ///< 7.3.5 25996
        SMSG_READY_CHECK_COMPLETED                  = 0x260E, ///< 7.3.5 25996
        SMSG_READY_CHECK_RESPONSE                   = 0x260D, ///< 7.3.5 25996
        SMSG_READY_CHECK_STARTED                    = 0x260C, ///< 7.3.5 25996
        SMSG_ROLL_POLL_INFORM                       = 0x258D, ///< 7.3.5 25996
        SMSG_ROLE_CHANGED_INFORM                    = 0x258C, ///< 7.3.5 25996
        SMSG_GROUP_DESTROYED                        = 0x27D5, ///< 7.3.5 25996

        /// Battleground
        SMSG_BATTLEFIELD_STATUS_QUEUED                      = 0x2591, ///< 7.3.5 25996
        SMSG_BATTLEFIELD_STATUS_NONE                        = 0x2592, ///< 7.3.5 25996
        SMSG_BATTLEFIELD_STATUS_NEED_CONFIRMATION           = 0x258F, ///< 7.3.5 25996
        SMSG_BATTLEFIELD_STATUS_ACTIVE                      = 0x2590, ///< 7.3.5 25996
        SMSG_BATTLEFIELD_STATUS_FAILED                      = 0x2593, ///< 7.3.5 25996
        SMSG_BATTLEFIELD_STATUS_WAIT_FOR_GROUPS             = 0x25A5, ///< 7.3.5 25996
        SMSG_BATTLEFIELD_PORT_DENIED                        = 0x259A, ///< 7.3.5 25996
        SMSG_RATED_BATTLEFIELD_INFO                         = 0x25A6, ///< 7.3.5 25996
        SMSG_BATTLEGROUND_PLAYER_JOINED                     = 0x2598, ///< 7.3.5 25996
        SMSG_BATTLEGROUND_PLAYER_LEFT                       = 0x2599, ///< 7.3.5 25996
        SMSG_BATTLEGROUND_PLAYER_POSITIONS                  = 0x2595, ///< 7.3.5 25996
        SMSG_REQUEST_PVP_REWARDS_RESPONSE                   = 0x25D4, ///< 7.3.5 25996
        SMSG_PVP_OPTIONS_ENABLED                            = 0x25B6, ///< 7.3.5 25996
        SMSG_PVP_LOG_DATA                                   = 0x25B3, ///< 7.3.5 25996
        SMSG_ARENA_PREP_OPPONENT_SPECIALIZATIONS            = 0x2665, ///< 7.3.5 25996
        SMSG_ARENA_CROWD_CONTROL_SPELLS                     = 0x264E, ///< 7.3.5 26365
        SMSG_DESTROY_ARENA_UNIT                             = 0x2784, ///< 7.3.5 25996
        SMSG_BATTLEGROUND_POINTS                            = 0x279F, ///< 7.3.5 25996
        SMSG_BATTLEGROUND_INIT                              = 0x27A0, ///< 7.3.5 25996
        SMSG_MAP_OBJECTIVES_INIT                            = 0x27A1, ///< 7.3.5 25996
        SMSG_CONQUEST_FORMULA_CONSTANTS                     = 0x27C7, ///< 7.3.5 25996
        SMSG_PVP_SEASON                                     = 0x25D3, ///< 7.3.5 25996
        SMSG_WARGAME_REQUEST_SUCCESSFULLY_SENT_TO_OPPONENT  = 0x25B4, ///< 7.3.5 25996
        SMSG_SPIRIT_HEALER_CONFIRM                          = 0x2754, ///< 7.3.5 25996
        SMSG_AREA_SPIRIT_HEALER_TIME                        = 0x2782, ///< 7.3.5 25996
        SMSG_BATTLEFIELD_LIST                               = 0x2594, ///< 7.3.5 25996
        SMSG_CHECK_WARGAME_ENTRY                            = 0x259E, ///< 7.3.5 25996

        /// Pet
        SMSG_PET_NAME_QUERY_RESPONSE                        = 0x2702, ///< 7.3.5 25996
        SMSG_PET_NAME_INVALID                               = 0x26EE, ///< 7.3.5 25996
        SMSG_PET_DISMISS_SOUND                              = 0x26CA, ///< 7.3.5 25996
        SMSG_PET_GUIDS                                      = 0x2741, ///< 7.3.5 25996
        SMSG_PET_MODE                                       = 0x2589, ///< 7.3.5 25996
        SMSG_PET_SLOT_UPDATED                               = 0x2588, ///< 7.3.5 25996
        SMSG_PET_STABLE_LIST                                = 0x25A9, ///< 7.3.5 25996
        SMSG_PET_ACTION_FEEDBACK                            = 0x278D, ///< 7.3.5 25996
        SMSG_PET_ACTION_SOUND                               = 0x26C9, ///< 7.3.5 25996
        SMSG_PET_ADDED                                      = 0x25A8, ///< 7.3.5 25996
        SMSG_SET_PET_SPECIALIZATION                         = 0x2641, ///< 7.3.5 25996
        SMSG_STABLE_RESULT                                  = 0x25AA, ///< 7.3.5 25996

        /// Battle pet opcodes
        SMSG_BATTLE_PET_UPDATES                             = 0x25FF, ///< 7.3.5 25996
        SMSG_BATTLE_PET_TRAP_LEVEL                          = 0x2600, ///< 7.3.5 25996
        SMSG_BATTLE_PET_JOURNAL_LOCK_ACQUIRED               = 0x2603, ///< 7.3.5 25996
        SMSG_BATTLE_PET_JOURNAL_LOCK_DENIED                 = 0x2604, ///< 7.3.5 25996
        SMSG_BATTLE_PET_JOURNAL                             = 0x2605, ///< 7.3.5 25996
        SMSG_BATTLE_PET_DELETED                             = 0x2606, ///< 7.3.5 25996
        SMSG_BATTLE_PET_REVOKED                             = 0x2607, ///< 7.3.5 25996
        SMSG_BATTLE_PET_RESTORED                            = 0x2608, ///< 7.3.5 25996
        SMSG_BATTLE_PETS_HEALED                             = 0x2609, ///< 7.3.5 25996
        SMSG_BATTLE_PET_LICENSE_CHANGED                     = 0x260A, ///< 7.3.5 25996
        SMSG_BATTLE_PET_ERROR                               = 0x2655, ///< 7.3.5 25996
        SMSG_BATTLE_PET_CAGE_DATE_ERROR                     = 0x26A0, ///< 7.3.5 25996
        SMSG_QUERY_BATTLE_PET_NAME_RESPONSE                 = 0x2703, ///< 7.3.5 25996

        /// Pet Battle opcodes
        SMSG_PET_BATTLE_SLOT_UPDATES                        = 0x2602, ///< 7.3.5 25996
        SMSG_PET_BATTLE_REQUEST_FAILED                      = 0x260F, ///< 7.3.5 25996
        SMSG_PET_BATTLE_PVPCHALLENGE                        = 0x2610, ///< 7.3.5 25996
        SMSG_PET_BATTLE_FINALIZE_LOCATION                   = 0x2611, ///< 7.3.5 25996
        SMSG_PET_BATTLE_INITIAL_UPDATE                      = 0x2612, ///< 7.3.5 25996
        SMSG_PET_BATTLE_FIRST_ROUND                         = 0x2613, ///< 7.3.5 25996
        SMSG_PET_BATTLE_ROUND_RESULT                        = 0x2614, ///< 7.3.5 25996
        SMSG_PET_BATTLE_REPLACEMENTS_MADE                   = 0x2615, ///< 7.3.5 25996
        SMSG_PET_BATTLE_FINAL_ROUND                         = 0x2616, ///< 7.3.5 25996
        SMSG_PET_BATTLE_FINISHED                            = 0x2617, ///< 7.3.5 25996
        SMSG_PET_BATTLE_CHAT_RESTRICTED                     = 0x2618, ///< 7.3.5 25996
        SMSG_PET_BATTLE_MAX_GAME_LENGTH_WARNING             = 0x2619, ///< 7.3.5 25996
        SMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH                 = 0x2656, ///< 7.3.5 25996
        SMSG_PET_BATTLE_QUEUE_STATUS                        = 0x2657, ///< 7.3.5 25996
        SMSG_PET_BATTLE_DEBUG_QUEUE_DUMP_RESPONSE           = 0x269C, ///< 7.3.5 25996

        /// Instances
        SMSG_INSTANCE_ENCOUNTER_ENGAGE_UNIT                     = 0x27F2, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_DISENGAGE_UNIT                  = 0x27F3, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_CHANGE_PRIORITY                 = 0x27F4, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_TIMER_START                     = 0x27F5, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_START                 = 0x27F6, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_COMPLETE              = 0x27F7, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_START                           = 0x27F8, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_UPDATE                = 0x27FB, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_END                             = 0x27FC, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_IN_COMBAT_RESURRECTION          = 0x27FD, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_PHASE_SHIFT_CHANGED             = 0x27FF, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_GAIN_COMBAT_RESURRECTION_CHARGE = 0x27FE, ///< 7.3.5 25996
        SMSG_INSTANCE_ENCOUNTER_SET_ALLOWING_RELEASE            = 0x27FA, ///< 7.3.5 26365
        SMSG_INSTANCE_RESET                                     = 0x26AF, ///< 7.3.5 25996
        SMSG_INSTANCE_RESET_FAILED                              = 0x26B0, ///< 7.3.5 25996
        SMSG_RAID_INSTANCE_INFO                                 = 0x2650, ///< 7.3.5 25996
        SMSG_RAID_GROUP_ONLY                                    = 0x27F1, ///< 7.3.5 25996
        SMSG_RAID_MARKERS_CHANGED                               = 0x25B9, ///< 7.3.5 25996
        SMSG_INSTANCE_SAVE_CREATED                              = 0x27BD, ///< 7.3.5 25996
        SMSG_INSTANCE_GROUP_SIZE_CHANGED                        = 0x2736, ///< 7.3.5 25996
        SMSG_ENCOUNTER_END                                      = 0x27BF, ///< 7.3.5 25996
        SMSG_ENCOUNTER_START                                    = 0x27BE, ///< 7.3.5 25996
        SMSG_BOSS_KILL_CREDIT                                   = 0x27C0, ///< 7.3.5 25996
        SMSG_PENDING_RAID_LOCK                                  = 0x2730, ///< 7.3.5 25996

        /// Scenarios
        SMSG_SCENARIO_BOOT                                      = 0x27EC, ///< 7.3.5 25996
        SMSG_SCENARIO_COMPLETED                                 = 0x282C, ///< 7.3.5 25996
        SMSG_SCENARIO_POIS                                      = 0x264F, ///< 7.3.5 25996
        SMSG_SCENARIO_PROGRESS_UPDATE                           = 0x2648, ///< 7.3.5 25996
        SMSG_SCENARIO_STATE                                     = 0x2647, ///< 7.3.5 25996
        SMSG_SCENARIO_SPELL_UPDATE                              = 0x2839, ///< 7.3.5 25996

        /// Auction House
        SMSG_AUCTION_HELLO_RESPONSE                             = 0x2723, ///< 7.3.5 25996
        SMSG_AUCTION_LIST_BIDDER_ITEMS_RESULT                   = 0x272C, ///< 7.3.5 25996
        SMSG_AUCTION_OUTBID_NOTIFICATION                        = 0x2727, ///< 7.3.5 25996
        SMSG_AUCTION_WON_NOTIFICATION                           = 0x2726, ///< 7.3.5 25996
        SMSG_AUCTION_COMMAND_RESULT                             = 0x2725, ///< 7.3.5 25996
        SMSG_AUCTION_CLOSED_NOTIFICATION                        = 0x2728, ///< 7.3.5 25996
        SMSG_AUCTION_LIST_PENDING_SALES_RESULT                  = 0x272D, ///< 7.3.5 25996
        SMSG_AUCTION_LIST_ITEMS_RESULT                          = 0x272A, ///< 7.3.5 25996
        SMSG_AUCTION_LIST_OWNER_ITEMS_RESULT                    = 0x272B, ///< 7.3.5 25996
        SMSG_AUCTION_OWNER_BID_NOTIFICATION                     = 0x2729, ///< 7.3.5 25996
        SMSG_AUCTION_REPLICATE_RESPONSE                         = 0x2724, ///< 7.3.5 25996

        /// Mail
        SMSG_SEND_MAIL_RESULT                                   = 0x2658, ///< 7.3.5 25996
        SMSG_MAIL_LIST_RESULT                                   = 0x2798, ///< 7.3.5 25996
        SMSG_RECEIVED_MAIL                                      = 0x2659, ///< 7.3.5 25996
        SMSG_MAIL_QUERY_NEXT_TIME_RESULT                        = 0x2799, ///< 7.3.5 25996

        /// Trainers
        SMSG_TRAINER_LIST                                       = 0x2714, ///< 7.3.5 25996
        SMSG_TRAINER_SERVICE                                    = 0x2715, ///< 7.3.5 25996

        /// Void Storage
        SMSG_VOID_ITEM_SWAP_RESPONSE                            = 0x25DF, ///< 7.3.5 25996
        SMSG_VOID_STORAGE_CONTENTS                              = 0x25DC, ///< 7.3.5 25996
        SMSG_VOID_STORAGE_FAILED                                = 0x25DB, ///< 7.3.5 25996
        SMSG_VOID_STORAGE_TRANSFER_CHANGES                      = 0x25DD, ///< 7.3.5 25996
        SMSG_VOID_TRANSFER_RESULT                               = 0x25DE, ///< 7.3.5 25996

        /// Petition
        SMSG_PETITION_ALREADY_SIGNED                            = 0x25B8, ///< 7.3.5 25996
        SMSG_PETITION_SIGN_RESULTS                              = 0x278F, ///< 7.3.5 25996
        SMSG_QUERY_PETITION_RESPONSE                            = 0x2704, ///< 7.3.5 25996
        SMSG_PETITION_SHOW_LIST                                 = 0x26E9, ///< 7.3.5 25996
        SMSG_PETITION_SHOW_SIGNATURES                           = 0x26EA, ///< 7.3.5 25996
        SMSG_TURN_IN_PETITION_RESULTS                           = 0x2791, ///< 7.3.5 25996
        SMSG_PETITION_DECLINED                                  = 0x26E0, ///< 7.3.5 25996

        /// Threat
        SMSG_THREAT_CLEAR                                       = 0x270F, ///< 7.3.5 25996
        SMSG_THREAT_REMOVE                                      = 0x270E, ///< 7.3.5 25996
        SMSG_THREAT_UPDATE                                      = 0x270D, ///< 7.3.5 25996
        SMSG_HIGHEST_THREAT_UPDATE                              = 0x270C, ///< 7.3.5 25996

        /// Tickets
        SMSG_GM_TICKET_SYSTEM_STATUS                            = 0x26CB, ///< 7.3.5 25996
        SMSG_GM_TICKET_CASE_STATUS                              = 0x26CC, ///< 7.3.5 25996

        /// Calendar
        SMSG_CALENDAR_CLEAR_PENDING_ACTION                      = 0x26C6, ///< 7.3.5 25996
        SMSG_CALENDAR_COMMAND_RESULT                            = 0x26C7, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INITIAL_INVITES                     = 0x26B6, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE                              = 0x26B7, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_ALERT                        = 0x26B8, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_NOTES                        = 0x26C0, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT                  = 0x26C1, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_MODERATOR_STATUS             = 0x26BB, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_REMOVED                      = 0x26BC, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT                = 0x26BD, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_STATUS                       = 0x26B9, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT                 = 0x26BA, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_REMOVED_ALERT                       = 0x26BE, ///< 7.3.5 25996
        SMSG_CALENDAR_EVENT_UPDATED_ALERT                       = 0x26BF, ///< 7.3.5 25996
        SMSG_CALENDAR_RAID_LOCKOUT_ADDED                        = 0x26C2, ///< 7.3.5 25996
        SMSG_CALENDAR_RAID_LOCKOUT_REMOVED                      = 0x26C3, ///< 7.3.5 25996
        SMSG_CALENDAR_RAID_LOCKOUT_UPDATED                      = 0x26C4, ///< 7.3.5 25996
        SMSG_CALENDAR_SEND_CALENDAR                             = 0x26B4, ///< 7.3.5 25996
        SMSG_CALENDAR_SEND_EVENT                                = 0x26B5, ///< 7.3.5 25996
        SMSG_CALENDAR_SEND_NUM_PENDING                          = 0x26C5, ///< 7.3.5 25996

        /// Warden
        SMSG_WARDEN_DATA                                        = 0x2576, ///< 7.3.5 25996

        /// Challenges
        SMSG_CHALLENGE_MODE_REQUEST_LEADERS_RESULT              = 0x2624, ///< 7.3.5 25996
        SMSG_CHALLENGE_MODE_MAP_STATS_UPDATE                    = 0x2623, ///< 7.3.5 25996
        SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD                   = 0x2625, ///< 7.3.5 25996
        SMSG_CHALLENGE_MODE_ALL_MAP_STATS                       = 0x2622, ///< 7.3.5 25996
        SMSG_CHALLENGE_MODE_START                               = 0x261D, ///< 7.3.5 25996
        SMSG_CHALLENGE_MODE_COMPLETE                            = 0x2620, ///< 7.3.5 25996
        SMSG_CHALLENGE_MODE_REWARDS                             = 0x2621, ///< 7.3.5 25996 packet without any data
        SMSG_CHALLENGE_MODE_RESET                               = 0x261F, ///< 7.3.5 25996
        SMSG_CHALLENGE_MODE_UPDATE_DEATH_COUNT                  = 0x261E, ///< 7.3.5 25996

        /// Battlepay
        SMSG_BATTLE_PAY_ACK_FAILED                                   = 0x27C6, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_CONFIRM_PURCHASE                             = 0x27C5, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_DELIVERY_ENDED                               = 0x27B9, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_DELIVERY_STARTED                             = 0x27B8, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_DISTRIBUTION_UPDATE                          = 0x27B7, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_GET_DISTRIBUTION_LIST_RESPONSE               = 0x27B5, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE                    = 0x27B3, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_GET_PURCHASE_LIST_RESPONSE                   = 0x27B4, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_PURCHASE_UPDATE                              = 0x27C4, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_START_DISTRIBUTION_ASSIGN_TO_TARGET_RESPONSE = 0x27C2, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_START_PURCHASE_RESPONSE                      = 0x27C1, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_VAS_BOOST_CONSUMED                           = 0x27B6, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_VAS_CHARACTER_LIST                           = 0x2830, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_VAS_PURCHASE_COMPLETE                        = 0x2833, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_VAS_PURCHASE_LIST                            = 0x2834, ///< 7.3.5 25996
        SMSG_BATTLE_PAY_VAS_PURCHASE_STARTED                         = 0x2832, ///< 7.3.5 25996

        /// Black Market
        SMSG_BLACK_MARKET_OPEN_RESULT                           = 0x2642, ///< 7.3.5 25996
        SMSG_BLACK_MARKET_OUTBID                                = 0x2645, ///< 7.3.5 25996
        SMSG_BLACK_MARKET_REQUEST_ITEMS_RESULT                  = 0x2643, ///< 7.3.5 25996
        SMSG_BLACK_MARKET_BID_ON_ITEM_RESULT                    = 0x2644, ///< 7.3.5 25996
        SMSG_BLACK_MARKET_WON                                   = 0x2646, ///< 7.3.5 25996

        /// Twitter
        SMSG_TWITTER_STATUS                                     = 0x2FFD, ///< 7.3.5 25996
        SMSG_OAUTH_SAVED_DATA                                   = 0x0000, ///< unused

        /// Toys
        SMSG_ACCOUNT_TOYS_UPDATE                                = 0x25C4, ///< 7.3.5 25996

        /// AreaTriggers
        SMSG_AREA_TRIGGER_RE_PATH                               = 0x263C, ///< 7.3.5 25996
        SMSG_AREA_TRIGGER_RE_SHAPE                              = 0x263D, ///< 7.3.5 25996
        SMSG_AREA_TRIGGER_PROBABLY_RE_PATH_UNK                  = 0x263F, ///< 7.3.5 25996

        /// Artifacts
        SMSG_ARTIFACT_KNOWLEDGE                                 = 0x27EA, ///< 7.3.5 25996
        SMSG_ARTIFACT_FORGE_OPENED                              = 0x27E2, ///< 7.3.5 25996
        SMSG_ARTIFACT_TRAITS_REFUNDED                           = 0x27E6, ///< 7.3.5 25996 not used
        SMSG_ARTIFACT_RESPEC_CONFIRM                            = 0x27E5, ///< 7.3.5 25996
        SMSG_ARTIFACT_XP_GAIN                                   = 0x282D, ///< 7.3.5 25996

        /// Adventure Map
        SMSG_ADVENTURE_MAP_OPEN                                 = 0x2835, ///< 7.3.5 25996
        SMSG_GARRISON_SCOUTING_MAP_RESULT                       = 0x283B, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Guild
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamGuild
        /// Petition
        SMSG_PETITION_RENAME                        = 0x29F7, ///< 7.3.5 25996

        /// Guild Finder
        SMSG_LF_GUILD_BROWSE                        = 0x29CE, ///< 7.3.5 25996
        SMSG_LF_GUILD_APPLICATIONS                  = 0x29D1, ///< 7.3.5 25996
        SMSG_LF_GUILD_APPLICATIONS_LIST_CHANGED     = 0x29D6, ///< 7.3.5 25996
        SMSG_LF_GUILD_COMMAND_RESULT                = 0x29D0, ///< 7.3.5 25996
        SMSG_LF_GUILD_RECRUITS                      = 0x29CF, ///< 7.3.5 25996
        SMSG_LF_GUILD_POST                          = 0x29CD, ///< 7.3.5 25996
        SMSG_LF_GUILD_APPLICANT_LIST_CHANGED        = 0x29D5, ///< 7.3.5 25996

        /// Bank
        SMSG_GUILD_RANKS                            = 0x29C8, ///< 7.3.5 25996
        SMSG_GUILD_BANK_LOG_QUERY_RESULT            = 0x29DF, ///< 7.3.5 25996
        SMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY    = 0x29E0, ///< 7.3.5 25996
        SMSG_GUILD_BANK_QUERY_TEXT_RESULT           = 0x29E3, ///< 7.3.5 25996
        SMSG_GUILD_BANK_QUERY_RESULTS               = 0x29DE, ///< 7.3.5 25996
        SMSG_GUILD_PERMISSIONS_QUERY_RESULTS        = 0x29E1, ///< 7.3.5 25996

        /// Achievement
        SMSG_GUILD_ACHIEVEMENT_DELETED              = 0x29C5, ///< 7.3.5 25996
        SMSG_GUILD_ACHIEVEMENT_MEMBERS              = 0x29C7, ///< 7.3.5 25996
        SMSG_GUILD_ACHIEVEMENT_EARNED               = 0x29C4, ///< 7.3.5 25996
        SMSG_GUILD_CRITERIA_UPDATE                  = 0x29C3, ///< 7.3.5 25996
        SMSG_GUILD_CRITERIA_DELETED                 = 0x29C6, ///< 7.3.5 25996
        SMSG_ALL_GUILD_ACHIEVEMENTS                 = 0x29B8, ///< 7.3.5 25996

        /// Info
        SMSG_GUILD_ROSTER                           = 0x29BB, ///< 7.3.5 25996
        SMSG_GUILD_ROSTER_UPDATE                    = 0x29BC, ///< 7.3.5 25996
        SMSG_QUERY_GUILD_INFO_RESPONSE              = 0x29E5, ///< 7.3.5 25996
        SMSG_GUILD_MEMBER_UPDATE_NOTE               = 0x29C9, ///< 7.3.5 25996
        SMSG_GUILD_REWARDS_LIST                     = 0x29C0, ///< 7.3.5 25996
        SMSG_GUILD_SEND_RANK_CHANGE                 = 0x29B9, ///< 7.3.5 25996
        SMSG_GUILD_CHALLENGE_UPDATED                = 0x29D2, ///< 7.3.5 25996
        SMSG_GUILD_MEMBER_RECIPES                   = 0x29BD, ///< 7.3.5 25996
        SMSG_GUILD_INVITE_EXPIRED                   = 0x29E7, ///< 7.3.5 25996
        SMSG_GUILD_COMMAND_RESULT                   = 0x29BA, ///< 7.3.5 25996
        SMSG_GUILD_CHALLENGE_COMPLETED              = 0x29D3, ///< 7.3.5 25996
        SMSG_GUILD_REPUTATION_REACTION_CHANGED      = 0x29CC, ///< 7.3.5 25996
        SMSG_GUILD_KNOWN_RECIPES                    = 0x29BE, ///< 7.3.5 25996
        SMSG_GUILD_MEMBER_DAILY_RESET               = 0x29E4, ///< 7.3.5 25996
        SMSG_GUILD_FLAGGED_FOR_RENAME               = 0x29DC, ///< 7.3.5 25996
        SMSG_GUILD_NAME_CHANGED                     = 0x29DB, ///< 7.3.5 25996
        SMSG_GUILD_RESET                            = 0x29D8, ///< 7.3.5 25996
        SMSG_GUILD_CHANGE_NAME_RESULT               = 0x29DD, ///< 7.3.5 25996
        SMSG_GUILD_INVITE                           = 0x29CA, ///< 7.3.5 25996
        SMSG_GUILD_INVITE_DECLINED                  = 0x29E6, ///< 7.3.5 25996
        SMSG_GUILD_PARTY_STATE                      = 0x29CB, ///< 7.3.5 25996
        SMSG_GUILD_MEMBERS_WITH_RECIPE              = 0x29BF, ///< 7.3.5 25996
        SMSG_GUILD_MOVE_STARTING                    = 0x29D9, ///< 7.3.5 25996
        SMSG_GUILD_MOVED                            = 0x29DA, ///< 7.3.5 25996
        SMSG_GUILD_NEWS                             = 0x29C1, ///< 7.3.5 25996
        SMSG_GUILD_NEWS_DELETED                     = 0x29C2, ///< 7.3.5 25996
        SMSG_PLAYER_TABAR_VENDOR_SHOW               = 0x279C, ///< 7.3.5 25996
        SMSG_PLAYER_SAVE_GUILD_EMBLEM               = 0x29F6, ///< 7.3.5 25996
        SMSG_GUILD_BROADCAST_LOOTED_ITEM            = 0x29D4, ///< 7.3.5 25996

        /// Event system
        SMSG_GUILD_EVENT_NEW_LEADER                 = 0x29EA, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_PRESENCE_CHANGE            = 0x29ED, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_PLAYER_JOINED              = 0x29E8, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_PLAYER_LEFT                = 0x29E9, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_RANKS_UPDATED              = 0x29EE, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_RANK_CHANGED               = 0x29EF, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_LOG_QUERY_RESULTS          = 0x29E2, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_MOTD                       = 0x29EC, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_DISBANDED                  = 0x29EB, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_TAB_ADDED                  = 0x29F0, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_TAB_MODIFIED               = 0x29F2, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_TAB_DELETED                = 0x29F1, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_TAB_TEXT_CHANGED           = 0x29F3, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_BANK_CONTENTS_CHANGED      = 0x29F5, ///< 7.3.5 25996
        SMSG_GUILD_EVENT_BANK_MONEY_CHANGED         = 0x29F4, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Chat
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamChat
        /// Misc
        SMSG_SERVER_FIRST_ACHIEVEMENT               = 0x2BBC, ///< 7.3.5 25996
        SMSG_RAID_INSTANCE_MESSAGE                  = 0x2BB4, ///< 7.3.5 25996
        SMSG_MOTD                                   = 0x2BAF, ///< 7.3.5 25996
        SMSG_EXPECTED_SPAM_RECORDS                  = 0x2BB1, ///< 7.3.5 25996
        SMSG_DEFENSE_MESSAGE                        = 0x2BB6, ///< 7.3.5 25996
        SMSG_ZONE_UNDER_ATTACK                      = 0x2BB5, ///< 7.3.5 25996
        SMSG_WHO                                    = 0x2BAE, ///< 7.3.5 25996

        /// Chat
        SMSG_CHAT_SERVER_MESSAGE                    = 0x2BC4, ///< 7.3.5 25996
        SMSG_CHAT_RESTRICTED                        = 0x2BB3, ///< 7.3.5 25996
        SMSG_CHAT_RECONNECT                         = 0x2BBF, ///< 7.3.5 25996
        SMSG_CHAT_PLAYER_NOTFOUND                   = 0x2BB7, ///< 7.3.5 25996
        SMSG_CHAT_PLAYER_AMBIGUOUS                  = 0x2BB0, ///< 7.3.5 25996
        SMSG_CHAT_NOT_IN_PARTY                      = 0x2BB2, ///< 7.3.5 25996
        SMSG_CHAT_IS_DOWN                           = 0x2BBE, ///< 7.3.5 25996
        SMSG_CHAT_IGNORED_ACCOUNT_MUTED             = 0x2BAC, ///< 7.3.5 25996
        SMSG_CHAT_DOWN                              = 0x2BBD, ///< 7.3.5 25996
        SMSG_CHAT_AUTO_RESPONDED                    = 0x2BB8, ///< 7.3.5 25996
        SMSG_CHAT                                   = 0x2BAD, ///< 7.3.5 25996
        CMSG_SET_ADVANCED_COMBAT_LOGGING            = 0x32A5, ///< 7.3.5 25996
        CMSG_REPORT_CLIENT_VARIABLES                = 0x36FF, ///< 7.3.5 25996
        CMSG_REPORT_ENABLED_ADDONS                  = 0x36FE, ///< 7.3.5 25996
        CMSG_REPORT_KEYBINDING_EXECUTION_COUNTS     = 0x3700, ///< 7.3.5 25996

        /// Channel
        SMSG_CHANNEL_NOTIFY_LEFT                    = 0x2BC2, ///< 7.3.5 25996
        SMSG_CHANNEL_NOTIFY_JOINED                  = 0x2BC1, ///< 7.3.5 25996
        SMSG_CHANNEL_NOTIFY                         = 0x2BC0, ///< 7.3.5 25996
        SMSG_CHANNEL_LIST                           = 0x2BC3, ///< 7.3.5 25996
        SMSG_USERLIST_ADD                           = 0x2BB9, ///< 7.3.5 25996
        SMSG_USERLIST_REMOVE                        = 0x2BBA, ///< 7.3.5 25996
        SMSG_USERLIST_UPDATE                        = 0x2BBB, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Move
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamMove
        /// Move speeds
        SMSG_MOVE_SET_RUN_SPEED                     = 0x2DBE, ///< 7.3.5 25996
        SMSG_MOVE_SET_RUN_BACK_SPEED                = 0x2DBF, ///< 7.3.5 25996
        SMSG_MOVE_SET_SWIM_SPEED                    = 0x2DC0, ///< 7.3.5 25996
        SMSG_MOVE_SET_SWIM_BACK_SPEED               = 0x2DC1, ///< 7.3.5 25996
        SMSG_MOVE_SET_FLIGHT_SPEED                  = 0x2DC2, ///< 7.3.5 25996
        SMSG_MOVE_SET_FLIGHT_BACK_SPEED             = 0x2DC3, ///< 7.3.5 25996
        SMSG_MOVE_SET_WALK_SPEED                    = 0x2DC4, ///< 7.3.5 25996
        SMSG_MOVE_SET_TURN_RATE                     = 0x2DC5, ///< 7.3.5 25996
        SMSG_MOVE_SET_PITCH_RATE                    = 0x2DC6, ///< 7.3.5 25996
        SMSG_MOVE_SET_MOVEMENT_FORCE_SPEED          = 0x2DB4, ///< 7.3.5 25996
        SMSG_MOVE_KNOCK_BACK                        = 0x2DD1, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_RUN_SPEED                  = 0x2DA4, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_RUN_BACK_SPEED             = 0x2DA5, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_SWIM_SPEED                 = 0x2DA7, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_SWIM_BACK_SPEED            = 0x2DA8, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_FLIGHT_SPEED               = 0x2DA9, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_FLIGHT_BACK_SPEED          = 0x2DAA, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_WALK_SPEED                 = 0x2DA6, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_TURN_RATE                  = 0x2DAB, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_PITCH_RATE                 = 0x2DAC, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_KNOCK_BACK                 = 0x2DB0, ///< 7.3.5 25996

        /// Player
        SMSG_MOVE_UPDATE                                    = 0x2DAE, ///< 7.3.5 25996
        SMSG_MOVE_TELEPORT                                  = 0x2DD2, ///< 7.3.5 25996
        SMSG_MOVE_UPDATE_TELEPORT                           = 0x2DAF, ///< 7.3.5 25996
        SMSG_MOVE_ROOT                                      = 0x2DC7, ///< 7.3.5 25996
        SMSG_MOVE_UNROOT                                    = 0x2DC8, ///< 7.3.5 25996
        SMSG_MOVE_WATER_WALK                                = 0x2DC9, ///< 7.3.5 25996
        SMSG_MOVE_LAND_WALK                                 = 0x2DCC, ///< 7.3.5 25996
        SMSG_MOVE_FEATHER_FALL                              = 0x2DCD, ///< 7.3.5 25996
        SMSG_MOVE_NORMAL_FALL                               = 0x2DCE, ///< 7.3.5 25996
        SMSG_MOVE_SET_ACTIVE_MOVER                          = 0x2DA3, ///< 7.3.5 25996
        SMSG_MOVE_SET_CAN_FLY                               = 0x2DD3, ///< 7.3.5 25996
        SMSG_MOVE_UNSET_CAN_FLY                             = 0x2DD4, ///< 7.3.5 25996
        SMSG_MOVE_SET_HOVER                                 = 0x2DCF, ///< 7.3.5 25996
        SMSG_MOVE_UNSET_HOVER                               = 0x2DD0, ///< 7.3.5 25996
        SMSG_MOVE_SET_CAN_TURN_WHILE_FALLING                = 0x2DD5, ///< 7.3.5 25996
        SMSG_MOVE_UNSET_CAN_TURN_WHILE_FALLING              = 0x2DD6, ///< 7.3.5 25996
        SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY   = 0x2DD9, ///< 7.3.5 25996
        SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY = 0x2DDA, ///< 7.3.5 25996
        SMSG_MOVE_SET_VEHICLE_REC_ID                        = 0x2DE0, ///< 7.3.5 25996
        SMSG_MOVE_SET_COLLISION_HEIGHT                      = 0x2DDF, ///< 7.3.5 25996
        SMSG_MOVE_APPLY_MOVEMENT_FORCE                      = 0x2DE1, ///< 7.3.5 25996
        SMSG_MOVE_REMOVE_MOVEMENT_FORCE                     = 0x2DE2, ///< 7.3.5 25996
        SMSG_MOVE_SKIP_TIME                                 = 0x2DE4, ///< 7.3.5 25996
        SMSG_MOVE_ENABLE_GRAVITY                            = 0x2DDC, ///< 7.3.5 25996
        SMSG_MOVE_DISABLE_GRAVITY                           = 0x2DDB, ///< 7.3.5 25996
        SMSG_MOVE_ENABLE_COLLISION                          = 0x2DDE, ///< 7.3.5 25996
        SMSG_MOVE_DISABLE_COLLISION                         = 0x2DDD, ///< 7.3.5 25996
        SMSG_MOVE_ENABLE_DOUBLE_JUMP                        = 0x2DCA, ///< 7.3.5 25996
        SMSG_MOVE_DISABLE_DOUBLE_JUMP                       = 0x2DCB, ///< 7.3.5 25996

        /// Creature
        SMSG_MONSTER_MOVE                           = 0x2DA2, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_RUN_SPEED              = 0x2DB5, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED         = 0x2DB6, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_SWIM_SPEED             = 0x2DB7, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED        = 0x2DB8, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED           = 0x2DB9, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED      = 0x2DBA, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_WALK_SPEED             = 0x2DBB, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_TURN_RATE              = 0x2DBC, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_PITCH_RATE             = 0x2DBD, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_ROOT                       = 0x2DE5, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_UNROOT                     = 0x2DE6, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_GRAVITY_DISABLE            = 0x2DE7, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_GRAVITY_ENABLE             = 0x2DE8, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_COLLISION_DISABLE          = 0x2DE9, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_COLLISION_ENABLE           = 0x2DEA, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_FEATHER_FALL           = 0x2DEB, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_NORMAL_FALL            = 0x2DEC, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_HOVER                  = 0x2DED, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_UNSET_HOVER                = 0x2DEE, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_WATER_WALK             = 0x2DEF, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_LAND_WALK              = 0x2DF0, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_START_SWIM                 = 0x2DF1, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_STOP_SWIM                  = 0x2DF2, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_RUN_MODE               = 0x2DF3, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_WALK_MODE              = 0x2DF4, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_SET_FLYING                 = 0x2DF5, ///< 7.3.5 25996
        SMSG_SPLINE_MOVE_UNSET_FLYING               = 0x2DF6, ///< 7.3.5 25996
        SMSG_FLIGHT_SPLINE_SYNC                     = 0x2DF7, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Spell
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamSpell
        /// Aura
        SMSG_AURA_POINTS_DEPLETED                   = 0x2C23, ///< 7.3.5 25996
        SMSG_AURA_UPDATE                            = 0x2C22, ///< 7.3.5 25996

        /// Misc
        SMSG_SPELL_REGUID                           = 0x2C38, ///< 7.3.5 25996
        SMSG_CHEAT_IGNORE_DIMISHING_RETURNS         = 0x2C12, ///< 7.3.5 25996
        SMSG_DISPEL_FAILED                          = 0x2C30, ///< 7.3.5 25996
        SMSG_MIRROR_IMAGE_COMPONENTED_DATA          = 0x2C14, ///< 7.3.5 25996
        SMSG_MIRROR_IMAGE_CREATURE_DATA             = 0x2C13, ///< 7.3.5 25996
        SMSG_PET_CLEAR_SPELLS                       = 0x2C24, ///< 7.3.5 25996
        SMSG_REFRESH_SPELL_HISTORY                  = 0x2C2C, ///< 7.3.5 25996
        SMSG_RESUME_CAST_BAR                        = 0x2C3E, ///< 7.3.5 25996
        SMSG_CAST_FAILED                            = 0x2C56, ///< 7.3.5 25996
        SMSG_SPELL_FAILURE                          = 0x2C52, ///< 7.3.5 25996
        SMSG_SPELL_FAILED_OTHER                     = 0x2C54, ///< 7.3.5 25996
        SMSG_PET_CAST_FAILED                        = 0x2C57, ///< 7.3.5 25996
        SMSG_ITEM_COOLDOWN                          = 0x280B, ///< 7.3.5 25996
        SMSG_MODIFY_COOLDOWN                        = 0x27A6, ///< 7.3.5 25996
        SMSG_COOLDOWN_CHEAT                         = 0x277B, ///< 7.3.5 25996
        SMSG_PET_TAME_FAILURE                       = 0x26DD, ///< 7.3.5 25996
        SMSG_FEIGN_DEATH_RESISTED                   = 0x2787, ///< 7.3.5 25996
        SMSG_NOTIFY_DEST_LOC_SPELL_CAST             = 0x2C43, ///< 7.3.5 25996
        SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA   = 0x271C, ///< 7.3.5 25996
        SMSG_SET_VEHICLE_REC_ID                     = 0x272F, ///< 7.3.5 25996
        SMSG_COOLDOWN_EVENT                         = 0x26E3, ///< 7.3.5 25996
        SMSG_DISMOUNT                               = 0x26DA, ///< 7.3.5 25996
        SMSG_DISMOUNTRESULT                         = 0x257B, ///< 7.3.5 25996
        SMSG_MOUNT_RESULT                           = 0x257A, ///< 7.3.5 25996
        SMSG_MODIFY_COOLDOWN_RECOVERY_SPEED         = 0x27A7, ///< 7.3.5 25996
        SMSG_MODIFY_CHARGE_RECOVERY_SPEED           = 0x27A8, ///< 7.3.5 25996
        SMSG_CONTRIBUTION_SEND_STATE                = 0x285C, ///< 7.3.5 25996
        SMSG_ITEM_EXPIRE_PURCHASE_REFUND            = 0x25B1, ///< 7.3.5 26365

        /// Spell Book / Bar
        SMSG_UPDATE_WEEKLY_SPELL_USAGE              = 0x2C19, ///< 7.3.5 25996
        SMSG_WEEKLY_SPELL_USAGE                     = 0x2C18, ///< 7.3.5 25996
        SMSG_SEND_KNOWN_SPELLS                      = 0x2C2A, ///< 7.3.5 25996
        SMSG_SEND_SPELL_CHARGES                     = 0x2C2D, ///< 7.3.5 25996
        SMSG_SEND_SPELL_HISTORY                     = 0x2C2B, ///< 7.3.5 25996
        SMSG_SEND_UNLEARN_SPELLS                    = 0x2C2E, ///< 7.3.5 25996
        SMSG_CLEAR_ALL_SPELL_CHARGES                = 0x2C27, ///< 7.3.5 25996
        SMSG_CLEAR_COOLDOWN                         = 0x26E4, ///< 7.3.5 25996
        SMSG_CLEAR_COOLDOWNS                        = 0x2C26, ///< 7.3.5 25996
        SMSG_CATEGORY_COOLDOWN                      = 0x2C16, ///< 7.3.5 25996
        SMSG_CLEAR_SPELL_CHARGES                    = 0x2C28, ///< 7.3.5 25996
        SMSG_SET_FLAT_SPELL_MODIFIER                = 0x2C36, ///< 7.3.5 25996
        SMSG_SET_PCT_SPELL_MODIFIER                 = 0x2C37, ///< 7.3.5 25996
        SMSG_SET_SPELL_CHARGES                      = 0x2C29, ///< 7.3.5 25996
        SMSG_LEARNED_SPELL                          = 0x2C4D, ///< 7.3.5 25996
        SMSG_UNLEARNED_SPELLS                       = 0x2C4E, ///< 7.3.5 25996
        SMSG_LEARN_TALENTS_FAILED                   = 0x25E9, ///< 7.3.5 25996

        /// Casting
        SMSG_SPELL_CHANNEL_START                    = 0x2C34, ///< 7.3.5 25996
        SMSG_SPELL_COOLDOWN                         = 0x2C15, ///< 7.3.5 25996
        SMSG_SPELL_CHANNEL_UPDATE                   = 0x2C35, ///< 7.3.5 25996
        SMSG_SPELL_DAMAGE_SHIELD                    = 0x2C31, ///< 7.3.5 25996
        SMSG_SPELL_DELAYED                          = 0x2C3F, ///< 7.3.5 25996
        SMSG_SPELL_GO                               = 0x2C39, ///< 7.3.5 25996
        SMSG_SPELL_OR_DAMAGE_IMMUNE                 = 0x2C2F, ///< 7.3.5 25996
        SMSG_SPELL_START                            = 0x2C3A, ///< 7.3.5 25996
        SMSG_SCRIPT_CAST                            = 0x2C55, ///< 7.3.5 25996

        /// Logging
        SMSG_SPELL_DISPELL_LOG                      = 0x2C1A, ///< 7.3.5 25996
        SMSG_SPELL_ENERGIZE_LOG                     = 0x2C1C, ///< 7.3.5 25996
        SMSG_SPELL_EXECUTE_LOG                      = 0x2C40, ///< 7.3.5 25996
        SMSG_SPELL_NON_MELEE_DAMAGE_LOG             = 0x2C32, ///< 7.3.5 25996
        SMSG_SPELL_HEAL_LOG                         = 0x2C1D, ///< 7.3.5 25996
        SMSG_SPELL_INSTAKILL_LOG                    = 0x2C33, ///< 7.3.5 25996
        SMSG_SPELL_INTERRUPT_LOG                    = 0x2C20, ///< 7.3.5 25996
        SMSG_SPELL_MISS_LOG                         = 0x2C41, ///< 7.3.5 25996
        SMSG_ENVIRONMENTAL_DAMAGE_LOG               = 0x2C21, ///< 7.3.5 25996
        SMSG_SPELL_PERIODIC_AURA_LOG                = 0x2C1B, ///< 7.3.5 25996
        SMSG_SPELL_ABSORB_LOG                       = 0x2C1F, ///< 7.3.5 25996
        SMSG_SPELL_PROC_SCRIPT_LOG                  = 0x0000,                       ///<
        SMSG_SPELL_PROCS_PER_MINUTE_LOG             = 0x0000,                       ///<
        SMSG_SPELL_CHANCE_PROC_LOG                  = 0x0000,                       ///<
        SMSG_RESIST_LOG                             = 0x0000,                       ///<
        SMSG_AURA_CAST_LOG                          = 0x0000,                       ///<

        /// Pet
        SMSG_PET_SPELLS_MESSAGE                     = 0x2C25, ///< 7.3.5 25996
        SMSG_PET_UNLEARNED_SPELLS                   = 0x2C50, ///< 7.3.5 25996
        SMSG_PET_LEARNED_SPELLS                     = 0x2C4F, ///< 7.3.5 25996

        /// Visuals
        SMSG_PLAY_ORPHAN_SPELL_VISUAL               = 0x2C47, ///< 7.3.5 25996
        SMSG_CANCEL_ORPHAN_SPELL_VISUAL             = 0x2C46, ///< 7.3.5 25996
        SMSG_PLAY_SPELL_VISUAL                      = 0x2C45, ///< 7.3.5 25996
        SMSG_CANCEL_SPELL_VISUAL                    = 0x2C44, ///< 7.3.5 25996
        SMSG_PLAY_SPELL_VISUAL_KIT                  = 0x2C49, ///< 7.3.5 25996
        SMSG_CANCEL_SPELL_VISUAL_KIT                = 0x2C48, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Quest
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamQuest
        /// Quest log
        SMSG_QUERY_QUEST_INFO_RESPONSE              = 0x2A95, ///< 7.3.5 25996
        SMSG_QUEST_UPDATE_FAILED_TIMER              = 0x2A8A, ///< 7.3.5 25996
        SMSG_QUEST_UPDATE_FAILED                    = 0x2A89, ///< 7.3.5 25996
        SMSG_QUEST_UPDATE_COMPLETE                  = 0x2A88, ///< 7.3.5 25996
        SMSG_QUEST_UPDATE_COMPLETE_BY_SPELL         = 0x2A87, ///< 7.3.5 25996
        SMSG_QUEST_UPDATE_ADD_PVP_CREDIT            = 0x2A8D, ///< 7.3.5 25996
        SMSG_QUEST_UPDATE_ADD_CREDIT_SIMPLE         = 0x2A8C, ///< 7.3.5 25996
        SMSG_QUEST_UPDATE_ADD_CREDIT                = 0x2A8B, ///< 7.3.5 25996
        SMSG_QUEST_SPAWN_TRACKING_UPDATE            = 0x2A9E, ///< 7.3.5 25996
        SMSG_QUEST_PUSH_RESULT                      = 0x2A8F, ///< 7.3.5 25996
        SMSG_QUEST_POIQUERY_RESPONSE                = 0x2A9C, ///< 7.3.5 25996
        SMSG_QUEST_LOG_FULL                         = 0x2A86, ///< 7.3.5 25996
        SMSG_IS_QUEST_COMPLETE_RESPONSE             = 0x2A83, ///< 7.3.5 25996
        SMSG_QUEST_FORCE_REMOVED                    = 0x2A9B, ///< 7.3.5 25996
        SMSG_QUEST_CONFIRM_ACCEPT                   = 0x2A8E, ///< 7.3.5 25996
        SMSG_QUEST_COMPLETION_NPCRESPONSE           = 0x2A81, ///< 7.3.5 25996
        SMSG_DAILY_QUESTS_RESET                     = 0x2A80, ///< 7.3.5 25996
        SMSG_WORLD_QUEST_UPDATE                     = 0x2847, ///< 7.3.5 25996
        SMSG_SET_ALL_TASK_PROGRESS                  = 0x27D1, ///< 7.3.5 25996
        SMSG_UPDATE_TASK_PROGRESS                   = 0x27D0, ///< 7.3.5 25996
        SMSG_SET_TASK_COMPLETE                      = 0x27D2, ///< 7.3.5 25996

        /// Quest giver
        SMSG_QUEST_GIVER_STATUS_MULTIPLE            = 0x2A90, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_STATUS                     = 0x2A9A, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_REQUEST_ITEMS              = 0x2A92, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE         = 0x2A99, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_QUEST_FAILED               = 0x2A85, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_QUEST_DETAILS              = 0x2A91, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_QUEST_COMPLETE             = 0x2A82, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE       = 0x2A93, ///< 7.3.5 25996
        SMSG_QUEST_GIVER_INVALID_QUEST              = 0x2A84, ///< 7.3.5 25996

        /// Gossip
        SMSG_GOSSIP_MESSAGE                         = 0x2A97, ///< 7.3.5 25996
        SMSG_GOSSIP_COMPLETE                        = 0x2A96, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Lfg
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamLFG
        /// LFG
        SMSG_LFG_BOOT_PLAYER                              = 0x2A35, ///< 7.3.5 25996
        SMSG_LFG_DISABLED                                 = 0x2A33, ///< 7.3.5 25996
        SMSG_LFG_JOIN_RESULT                              = 0x2A1C, ///< 7.3.5 25996
        SMSG_LFG_OFFER_CONTINUE                           = 0x2A34, ///< 7.3.5 25996
        SMSG_LFG_PARTY_INFO                               = 0x2A36, ///< 7.3.5 25996
        SMSG_LFG_PLAYER_INFO                              = 0x2A37, ///< 7.3.5 25996
        SMSG_LFG_PLAYER_REWARD                            = 0x2A38, ///< 7.3.5 25996
        SMSG_LFG_PROPOSAL_UPDATE                          = 0x2A2D, ///< 7.3.5 25996
        SMSG_LFG_QUEUE_STATUS                             = 0x2A20, ///< 7.3.5 25996
        SMSG_LFG_ROLE_CHECK_UPDATE                        = 0x2A21, ///< 7.3.5 25996
        SMSG_LFG_ROLE_CHOSEN                              = 0x2A39, ///< 7.3.5 25996
        SMSG_LFG_SLOT_INVALID                             = 0x2A30, ///< 7.3.5 25996
        SMSG_LFG_TELEPORT_DENIED                          = 0x2A32, ///< 7.3.5 25996
        SMSG_LFG_UPDATE_STATUS                            = 0x2A24, ///< 7.3.5 25996
        SMSG_UPDATE_DUNGEON_ENCOUNTER_FOR_LOOT            = 0x0000, ///< opcode not found

        /// LFG List
        SMSG_LFG_LIST_JOIN_RESULT                         = 0x2A1D, ///< 7.3.5 25996
        SMSG_LFG_LIST_UPDATE_BLACKLIST                    = 0x2A2A, ///< 7.3.5 25996
        SMSG_LFG_LIST_UPDATE_STATUS                       = 0x2A26, ///< 7.3.5 25996
        SMSG_LFG_LIST_SEARCH_RESULT                       = 0x2A1E, ///< 7.3.5 25996
        SMSG_LFG_LIST_SEARCH_STATUS                       = 0x2A1F, ///< 7.3.5 25996
        SMSG_LFG_LIST_APPLY_FOR_GROUP_RESULT              = 0x2A29, ///< 7.3.5 25996
        SMSG_LFG_LIST_APPLICANT_LIST_UPDATE               = 0x2A2B, ///< 7.3.5 25996
        SMSG_LFG_LIST_APPLICANT_GROUP_INVITE              = 0x2A28, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// WoW Token
    //////////////////////////////////////////////////////////////////////////
        SMSG_WOW_TOKEN_AUCTION_SOLD                       = 0x281C, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_BUY_REQUEST_CONFIRMATION           = 0x281E, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_BUY_RESULT_CONFIRMATION            = 0x281F, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_CAN_REDEEM_FOR_BALANCE_RESULT      = 0x2856, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_CAN_VETERAN_BUY_RESULT             = 0x281D, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_DISTRIBUTION_GLUE_UPDATE           = 0x2817, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_DISTRIBUTION_UPDATE                = 0x2818, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_MARKET_PRICE_RESPONSE              = 0x2819, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_REDEEM_GAME_TIME_UPDATED           = 0x2820, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_REDEEM_REQUEST_CONFIRMATION        = 0x2821, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_REDEEM_RESULT                      = 0x2822, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_SELL_REQUEST_CONFIRMATION          = 0x281A, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_SELL_RESULT_CONFIRMATION           = 0x281B, ///< 7.3.5 25996
        SMSG_WOW_TOKEN_UPDATE_AUCTIONABLE_LIST_RESPONSE   = 0x2823, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Garrison
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamGarrison
        SMSG_OPEN_SHIPMENT_NPC_FROM_GOSSIP                              = 0x27DA, ///< 7.3.5 25996
        SMSG_GET_SHIPMENT_INFO_RESPONSE                                 = 0x27DB, ///< 7.3.5 25996
        SMSG_OPEN_SHIPMENT_NPC_RESULT                                   = 0x27DC, ///< 7.3.5 25996
        SMSG_CREATE_SHIPMENT_RESPONSE                                   = 0x27DD, ///< 7.3.5 25996
        SMSG_COMPLETE_SHIPMENT_RESPONSE                                 = 0x27DE, ///< 7.3.5 25996
        SMSG_GET_SHIPMENTS_OF_TYPE_RESPONSE                             = 0x27DF, ///< 7.3.5 25996
        SMSG_GARRISON_LANDING_PAGE_SHIPMENT_INFO                        = 0x27E0, ///< 7.3.5 25996
        SMSG_GET_GARRISON_INFO_RESULT                                   = 0x28F0, ///< 7.3.5 25996
        SMSG_GARRISON_PLOT_PLACED                                       = 0x28F1, ///< 7.3.5 25996
        SMSG_GARRISON_PLOT_REMOVED                                      = 0x28F2, ///< 7.3.5 25996
        SMSG_GARRISON_PLACE_BUILDING_RESULT                             = 0x28F3, ///< 7.3.5 25996
        SMSG_GARRISON_BUILDING_REMOVED                                  = 0x28F4, ///< 7.3.5 25996
        SMSG_GARRISON_LEARN_SPECIALIZATION_RESULT                       = 0x28F5, ///< 7.3.5 25996
        SMSG_GARRISON_BUILDING_SET_ACTIVE_SPECIALIZATION_RESULT         = 0x28F6, ///< 7.3.5 25996
        SMSG_GARRISON_LEARN_BLUEPRINT_RESULT                            = 0x28F7, ///< 7.3.5 25996
        SMSG_GARRISON_UNLEARN_BLUEPRINT_RESULT                          = 0x28F8, ///< 7.3.5 25996
        SMSG_GARRISON_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA_RESULT  = 0x28F9, ///< 7.3.5 25996
        SMSG_GARRISON_REMOTE_INFO                                       = 0x28FA, ///< 7.3.5 25996
        SMSG_GARRISON_BUILDING_ACTIVATED                                = 0x28FB, ///< 7.3.5 25996
        SMSG_GARRISON_CREATE_RESULT                                     = 0x28FC, ///< 7.3.5 25996
        SMSG_GARRISON_UPGRADE_RESULT                                    = 0x28FD, ///< 7.3.5 25996
        SMSG_GARRISON_ADD_FOLLOWER_RESULT                               = 0x2902, ///< 7.3.5 25996
        SMSG_GARRISON_REMOVE_FOLLOWER_RESULT                            = 0x2903, ///< 7.3.5 25996
        SMSG_GARRISON_FOLLOWER_CHANGED_DURABILITY                       = 0x2904, ///< 7.3.5 25996
        SMSG_GARRISON_LIST_FOLLOWERS_CHEAT_RESULT                       = 0x2905, ///< 7.3.5 25996
        SMSG_GARRISON_ADD_MISSION_RESULT                                = 0x2906, ///< 7.3.5 25996
        SMSG_GARRISON_START_MISSION_RESULT                              = 0x2907, ///< 7.3.5 25996
        SMSG_GARRISON_UPDATE_MISSION                                    = 0x2908, ///< 7.3.5 25996
        SMSG_GARRISON_COMPLETE_MISSION_RESULT                           = 0x2909, ///< 7.3.5 25996
        SMSG_GARRISON_RECALL_PORTAL_LAST_USED_TIME                      = 0x290A, ///< 7.3.5 25996
        SMSG_GARRISON_RECALL_PORTAL_USED                                = 0x290B, ///< 7.3.5 25996
        SMSG_GARRISON_MISSION_BONUS_ROLL_RESULT                         = 0x290C, ///< 7.3.5 25996
        SMSG_ADVENTURE_LIST_UPDATE                                      = 0x290E, ///< 7.3.5 25996
        SMSG_GARRISON_MISSION_AREA_BONUS_ADDED                          = 0x2910, ///< 7.3.5 25996
        SMSG_GARRISON_MISSION_UPDATE_CAN_START                          = 0x2911, ///< 7.3.5 25996
        SMSG_GARRISON_FOLLOWER_CHANGED_XP                               = 0x2912, ///< 7.3.5 25996
        SMSG_GARRISON_FOLLOWER_CHANGED_ITEM_LEVEL                       = 0x2913, ///< 7.3.5 25996
        SMSG_GARRISON_FOLLOWER_CHANGED_ABILITIES                        = 0x2914, ///< 7.3.5 25996
        SMSG_GARRISON_FOLLOWER_CHANGED_STATUS                           = 0x2915, ///< 7.3.5 25996
        SMSG_GARRISON_CLEAR_ALL_FOLLOWERS_EXHAUSTION                    = 0x2916, ///< 7.3.5 25996
        SMSG_GARRISON_NUM_FOLLOWER_ACTIVATIONS_REMAINING                = 0x2917, ///< 7.3.5 25996
        SMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING_RESULT                = 0x2918, ///< 7.3.5 25996
        SMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING_RESULT              = 0x2919, ///< 7.3.5 25996
        SMSG_GARRISON_OPEN_RECRUITMENT_NPC                              = 0x291C, ///< 7.3.5 25996
        SMSG_GARRISON_RECRUITMENT_FOLLOWERS_GENERATED                   = 0x291E, ///< 7.3.5 25996
        SMSG_GARRISON_RECRUIT_FOLLOWER_RESULT                           = 0x291F, ///< 7.3.5 25996
        SMSG_GARRISON_DELETE_RESULT                                     = 0x2920, ///< 7.3.5 25996
        SMSG_GARRISON_OPEN_ARCHITECT                                    = 0x2921, ///< 7.3.5 25996
        SMSG_GARRISON_OPEN_TRADESKILL_NPC                               = 0x2922, ///< 7.3.5 25996
        SMSG_GARRISON_OPEN_MISSION_NPC                                  = 0x2923, ///< 7.3.5 25996
        SMSG_GARRISON_IS_UPGRADEABLE_RESULT                             = 0x2929, ///< 7.3.5 25996
        SMSG_GARRISON_LIST_MISSIONS_CHEAT_RESULT                        = 0x292A, ///< 7.3.5 25996
        SMSG_GARRISON_BUILDING_LANDMARKS                                = 0x292C, ///< 7.3.5 25996
        SMSG_GARRISON_OPEN_TALENT_NPC                                   = 0x291D, ///< 7.3.5 25996
        SMSG_GARRISON_UPDATE_TALENT                                     = 0x28FE, ///< 7.3.5 25996
        SMSG_GARRISON_UNK_MISSION_OPCODE                                = 0x0000, ///<  opcode not found

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        SMSG_CHARACTER_CLASS_TRIAL_CREATE = 0x2804, ///< 7.3.5 25996
        SMSG_UNDELETE_CHARACTER_RESPONSE  = 0x2811, ///< 7.3.5 25996
        SMSG_BATTLENET_RESPONSE           = 0x2842, ///< 7.3.5 25996

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        SMSG_AREA_POI_UPDATE              = 0x2848, ///< 7.3.5 25996

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        CMSG_QUERY_GARRISON_CREATURE_NAME                               = 0x3269, ///< 7.3.5 25996
        CMSG_UPGRADE_GARRISON                                           = 0x32AD, ///< 7.3.5 25996
        CMSG_GET_GARRISON_INFO                                          = 0x32B2, ///< 7.3.5 25996
        CMSG_GARRISON_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA         = 0x32B7, ///< 7.3.5 25996
        CMSG_GARRISON_PURCHASE_BUILDING                                 = 0x32B8, ///< 7.3.5 25996
        CMSG_GARRISON_SET_BUILDING_ACTIVE                               = 0x32B9, ///< 7.3.5 25996
        CMSG_GARRISON_CANCEL_CONSTRUCTION                               = 0x32BC, ///< 7.3.5 25996
        CMSG_GARRISON_SWAP_BUILDINGS                                    = 0x32BD, ///< 7.3.5 25996
        CMSG_GARRISON_SET_FOLLOWER_INACTIVE                             = 0x32C5, ///< 7.3.5 25996
        CMSG_GARRISON_SET_FOLLOWER_FAVORITE                             = 0x32C9, ///< 7.3.5 25996
        CMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING                       = 0x32CB, ///< 7.3.5 25996
        CMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING                     = 0x32CC, ///< 7.3.5 25996
        CMSG_GARRISON_RENAME_FOLLOWER                                   = 0x32CD, ///< 7.3.5 25996
        CMSG_GARRISON_GENERATE_RECRUITS                                 = 0x32CE, ///< 7.3.5 25996
        CMSG_GARRISON_SET_RECRUITMENT_PREFERENCES                       = 0x32CF, ///< 7.3.5 25996
        CMSG_GARRISON_RECRUIT_FOLLOWER                                  = 0x32D0, ///< 7.3.5 25996
        CMSG_GARRISON_RESEARCH_TALENT                                   = 0x32D1, ///< 7.3.5 25996
        CMSG_GARRISON_REQUEST_CLASS_SPEC_CATEGORY_INFO                  = 0x32D5, ///< 7.3.5 25996
        CMSG_OPEN_MISSION_NPC                                           = 0x32D7, ///< 7.3.5 25996
        CMSG_SET_USING_PARTY_GARRISON                                   = 0x32D9, ///< 7.3.5 25996
        CMSG_GARRISON_GET_BUILDING_LANDMARKS                            = 0x32DC, ///< 7.3.5 25996
        CMSG_OPEN_SHIPMENT_NPC                                          = 0x32DD, ///< 7.3.5 25996
        CMSG_GARRISON_REQUEST_SHIPMENT_INFO                             = 0x32DE, ///< 7.3.5 25996
        CMSG_GARRISON_REQUEST_LANDING_PAGE_SHIPMENT_INFO                = 0x32DF, ///< 7.3.5 25996
        CMSG_CREATE_SHIPMENT                                            = 0x32E0, ///< 7.3.5 25996
        CMSG_OPEN_TRADESKILL_NPC                                        = 0x32E8, ///< 7.3.5 25996
        CMSG_GARRISON_REMOVE_FOLLOWER                                   = 0x32F8, ///< 7.3.5 25996
        CMSG_GARRISON_START_MISSION                                     = 0x3300, ///< 7.3.5 25996
        CMSG_GARRISON_COMPLETE_MISSION                                  = 0x3301, ///< 7.3.5 25996
        CMSG_GARRISON_MISSION_BONUS_ROLL                                = 0x3303, ///< 7.3.5 25996
        CMSG_GARRISON_CHECK_UPGRADEABLE                                 = 0x330E, ///< 7.3.5 25996
        CMSG_REVERT_MONUMENT_APPEARANCE                                 = 0x32F5, ///< 7.3.5 25996
        CMSG_CHANGE_MONUMENT_APPEARANCE                                 = 0x32F4, ///< 7.3.5 25996
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// User Router
    //////////////////////////////////////////////////////////////////////////
    CMSG_CRYPT_ENABLED                          = 0x3767, ///< 7.3.5 25996
    CMSG_LOG_STREAMING_ERROR                    = 0x376D, ///< 7.3.5 25996
    CMSG_QUEUED_MESSAGES_END                    = 0x376C, ///< 7.3.5 25996
    CMSG_LOG_DISCONNECT                         = 0x3769, ///< 7.3.5 25996
    CMSG_PING                                   = 0x3768, ///< 7.3.5 25996
    CMSG_AUTH_CONTINUED_SESSION                 = 0x3766, ///< 7.3.5 25996
    CMSG_SUSPEND_TOKEN_RESPONSE                 = 0x376A, ///< 7.3.5 25996
    CMSG_AUTH_SESSION                           = 0x3765, ///< 7.3.5 25996
    CMSG_ENABLE_NAGLE                           = 0x376B, ///< 7.3.5 25996
    CMSG_SUSPEND_COMMS_ACK                      = 0x3764, ///< 7.3.5 25996
    CMSG_KEEP_ALIVE                             = 0x367F, ///< 7.3.5 25996
    CMSG_OBJECT_UPDATE_FAILED                   = 0x317F, ///< 7.3.5 25996
    CMSG_OBJECT_UPDATE_RESCUED                  = 0x3180, ///< 7.3.5 25996
    CMSG_UNDELETE_CHARACTER                     = 0x36DE, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Character
    //////////////////////////////////////////////////////////////////////////
    CMSG_ENUM_CHARACTERS                        = 0x35E8, ///< 7.3.5 25996
    CMSG_CREATE_CHARACTER                       = 0x3643, ///< 7.3.5 25996
    CMSG_CHAR_DELETE                            = 0x369B, ///< 7.3.5 25996
    CMSG_GENERATE_RANDOM_CHARACTER_NAME         = 0x35E7, ///< 7.3.5 25996
    CMSG_REORDER_CHARACTERS                     = 0x35E9, ///< 7.3.5 25996
    CMSG_PLAYER_LOGIN                           = 0x35EA, ///< 7.3.5 25996
    CMSG_VIOLENCE_LEVEL                         = 0x3183, ///< 7.3.5 25996
    CMSG_LOADING_SCREEN_NOTIFY                  = 0x35F8, ///< 7.3.5 25996
    CMSG_REQUEST_ACCOUNT_DATA                   = 0x3695, ///< 7.3.5 25996
    CMSG_UPDATE_ACCOUNT_DATA                    = 0x3696, ///< 7.3.5 25996
    CMSG_SET_DIFFICULTY_ID                      = 0x3218, ///< 7.3.5 25996
    CMSG_SET_DUNGEON_DIFFICULTY                 = 0x3682, ///< 7.3.5 25996
    CMSG_SET_RAID_DIFFICULTY                    = 0x36DB, ///< 7.3.5 25996
    CMSG_AUTO_DECLINE_GUILD_INVITES             = 0x3522, ///< 7.3.5 25996
    CMSG_SET_ACTIVE_MOVER                       = 0x3A37, ///< 7.3.5 25996
    CMSG_LEARN_TALENTS                          = 0x3556, ///< 7.3.5 25996
    CMSG_LEARN_PVP_TALENTS                      = 0x3557, ///< 7.3.5 25996
    CMSG_AUTOEQUIP_ITEM                         = 0x399A, ///< 7.3.5 25996
    CMSG_SWAP_INV_ITEM                          = 0x399D, ///< 7.3.5 25996
    CMSG_SWAP_ITEM                              = 0x399C, ///< 7.3.5 25996
    CMSG_AUTOSTORE_BAG_ITEM                     = 0x399B, ///< 7.3.5 25996
    CMSG_REQUEST_PET_INFO                       = 0x3493, ///< 7.3.5 25996
    CMSG_STAND_STATE_CHANGE                     = 0x3188, ///< 7.3.5 25996
    CMSG_BINDER_ACTIVATE                        = 0x34B5, ///< 7.3.5 25996
    CMSG_REQUEST_FORCED_REACTIONS               = 0x31FE, ///< 7.3.5 25996
    CMSG_DESTROY_ITEM                           = 0x3285, ///< 7.3.5 25996
    CMSG_OPEN_ITEM                              = 0x3310, ///< 7.3.5 25996
    CMSG_SET_TITLE                              = 0x3271, ///< 7.3.5 25996
    CMSG_REQUEST_PLAYED_TIME                    = 0x326D, ///< 7.3.5 25996
    CMSG_SAVE_EQUIPMENT_SET                     = 0x350F, ///< 7.3.5 25996
    CMSG_USE_EQUIPMENT_SET                      = 0x3995, ///< 7.3.5 25996
    CMSG_DELETE_EQUIPMENT_SET                   = 0x3510, ///< 7.3.5 25996
    CMSG_WHO                                    = 0x3681, ///< 7.3.5 25996
    CMSG_SOCKET_GEMS                            = 0x34EF, ///< 7.3.5 25996
    CMSG_RESURRECT_RESPONSE                     = 0x3684, ///< 7.3.5 25996
    CMSG_QUERY_INSPECT_ACHIEVEMENTS             = 0x3506, ///< 7.3.5 25996
    CMSG_SPLIT_ITEM                             = 0x399E, ///< 7.3.5 25996
    CMSG_SET_PLAYER_DECLINED_NAMES              = 0x368A, ///< 7.3.5 25996
    CMSG_MOUNT_SET_FAVORITE                     = 0x3631, ///< 7.3.5 25996
    CMSG_CHAR_RENAME                            = 0x36BE, ///< 7.3.5 25996
    CMSG_CHAR_CUSTOMIZE                         = 0x368E, ///< 7.3.5 25996
    CMSG_CHAR_RACE_OR_FACTION_CHANGE            = 0x3694, ///< 7.3.5 25996
    CMSG_SET_ACHIEVEMENTS_HIDDEN                = 0x321C, ///< 7.3.5 25996
    CMSG_MOUNT_CLEAR_FANFARE                    = 0x312D, ///< 7.3.5 25996
    CMSG_REQUEST_CROWD_CONTROL_SPELL            = 0x352E, ///< 7.3.5 26365

    //////////////////////////////////////////////////////////////////////////
    /// Bank
    //////////////////////////////////////////////////////////////////////////
    CMSG_BANKER_ACTIVATE                        = 0x34B6, ///< 7.3.5 25996
    CMSG_AUTOBANK_ITEM                          = 0x3996, ///< 7.3.5 25996
    CMSG_AUTOSTORE_BANK_ITEM                    = 0x3997, ///< 7.3.5 25996
    CMSG_BUY_BANK_SLOT                          = 0x34B7, ///< 7.3.5 25996
    CMSG_BUY_REAGENT_BANK                       = 0x34B8, ///< 7.3.5 25996
    CMSG_SET_SORT_BAGS_RIGHT_TO_LEFT            = 0x3316, ///< 7.3.5 25996
    CMSG_SET_INSERT_ITEMS_LEFT_TO_RIGHT         = 0x3317, ///< 7.3.5 25996
    CMSG_SORT_BAGS                              = 0x3318, ///< 7.3.5 25996
    CMSG_SORT_BANK_BAGS                         = 0x3319, ///< 7.3.5 25996
    CMSG_SORT_REAGENT_BANK_BAGS                 = 0x331A, ///< 7.3.5 25996
    CMSG_DEPOSIT_ALL_REAGENTS                   = 0x331B, ///< 7.3.5 25996
    CMSG_AUTOBANK_REAGENT                       = 0x3998, ///< 7.3.5 25996
    CMSG_AUTOSTORE_BANK_REAGENT                 = 0x3999, ///< 7.3.5 25996
    CMSG_CHANGE_BAG_SLOT_FLAG                   = 0x3312, ///< 7.3.5 25996
    CMSG_SET_BANK_BAG_SLOT_FLAG                 = 0x3313, ///< 7.3.5 25996
    CMSG_SET_BACKPACK_AUTOSORT_DISABLED         = 0x3314, ///< 7.3.5 25996
    CMSG_SET_BANK_AUTOSORT_DISABLED             = 0x3315, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Auction House
    //////////////////////////////////////////////////////////////////////////
    CMSG_AUCTION_HELLO_REQUEST                  = 0x34CF, ///< 7.3.5 25996
    CMSG_AUCTION_LIST_BIDDER_ITEMS              = 0x34D5, ///< 7.3.5 25996
    CMSG_AUCTION_LIST_ITEMS                     = 0x34D2, ///< 7.3.5 25996
    CMSG_AUCTION_LIST_OWNER_ITEMS               = 0x34D4, ///< 7.3.5 25996
    CMSG_AUCTION_LIST_PENDING_SALES             = 0x34D7, ///< 7.3.5 25996
    CMSG_AUCTION_PLACE_BID                      = 0x34D6, ///< 7.3.5 25996
    CMSG_AUCTION_REMOVE_ITEM                    = 0x34D1, ///< 7.3.5 25996
    CMSG_AUCTION_SELL_ITEM                      = 0x34D0, ///< 7.3.5 25996
    CMSG_AUCTION_REPLICATE_ITEMS                = 0x34D3, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Loot
    //////////////////////////////////////////////////////////////////////////
    CMSG_LOOT_UNIT                              = 0x3203, ///< 7.3.5 25996
    CMSG_LOOT_MONEY                             = 0x3204, ///< 7.3.5 25996
    CMSG_LOOT_ITEM                              = 0x3205, ///< 7.3.5 25996
    CMSG_LOOT_RELEASE                           = 0x3209, ///< 7.3.5 25996
    CMSG_LOOT_ROLL                              = 0x320A, ///< 7.3.5 25996
    CMSG_MASTER_LOOT_ITEM                       = 0x3206, ///< 7.3.5 25996
    CMSG_DO_MASTER_LOOT_ROLL                    = 0x3207, ///< 7.3.5 25996
    CMSG_SET_LOOT_SPECIALIZATION                = 0x3543, ///< 7.3.5 25996
    CMSG_SET_LOOT_METHOD                        = 0x3648, ///< 7.3.5 25996
    CMSG_OPT_OUT_OF_LOOT                        = 0x34FA, ///< 7.3.5 25996
    CMSG_CANCEL_MASTER_LOOT_ROLL                = 0x3208, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Combat
    //////////////////////////////////////////////////////////////////////////
    CMSG_ATTACK_SWING                           = 0x324C, ///< 7.3.5 25996
    CMSG_ATTACK_STOP                            = 0x324D, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Duel
    //////////////////////////////////////////////////////////////////////////
    CMSG_CAN_DUEL                               = 0x3662, ///< 7.3.5 25996
    CMSG_DUEL_RESPONSE                          = 0x34E6, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Spell
    //////////////////////////////////////////////////////////////////////////
    CMSG_CAST_SPELL                             = 0x328E, ///< 7.3.5 25996
    CMSG_CANCEL_CAST                            = 0x3291, ///< 7.3.5 25996
    CMSG_USE_ITEM                               = 0x328A, ///< 7.3.5 25996
    CMSG_CANCEL_AURA                            = 0x31AC, ///< 7.3.5 25996
    CMSG_CANCEL_AUTO_REPEAT_SPELL               = 0x34EB, ///< 7.3.5 25996
    CMSG_CANCEL_CHANNELLING                     = 0x325C, ///< 7.3.5 25996
    CMSG_CANCEL_GROWTH_AURA                     = 0x3261, ///< 7.3.5 25996
    CMSG_CANCEL_MOUNT_AURA                      = 0x3272, ///< 7.3.5 25996
    CMSG_CANCEL_QUEUED_SPELL                    = 0x317E, ///< 7.3.5 25996
    CMSG_CANCEL_MOD_SPEED_NO_CONTROL_AURAS      = 0x31AB, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Cache
    //////////////////////////////////////////////////////////////////////////
    CMSG_DB_QUERY_BULK                          = 0x35E4, ///< 7.3.5 25996
    CMSG_HOTFIX_REQUEST                         = 0x35E5, ///< 7.3.5 25996
    CMSG_QUERY_CREATURE                         = 0x3262, ///< 7.3.5 25996
    CMSG_QUERY_NPC_TEXT                         = 0x3264, ///< 7.3.5 25996
    CMSG_QUERY_PLAYER_NAME                      = 0x368B, ///< 7.3.5 25996
    CMSG_QUERY_QUEST_INFO                       = 0x3265, ///< 7.3.5 25996
    CMSG_QUEST_POI_QUERY                        = 0x36B0, ///< 7.3.5 25996
    CMSG_QUERY_REALM_NAME                       = 0x368C, ///< 7.3.5 25996
    CMSG_QUERY_GAME_OBJECT                      = 0x3263, ///< 7.3.5 25996
    CMSG_QUERY_PETITION                         = 0x326A, ///< 7.3.5 25996
    CMSG_QUERY_GUILD_INFO                       = 0x368D, ///< 7.3.5 25996
    CMSG_QUERY_PAGE_TEXT                        = 0x3266, ///< 7.3.5 25996
    CMSG_ITEM_TEXT_QUERY                        = 0x330F, ///< 7.3.5 25996
    CMSG_QUERY_QUEST_REWARDS                    = 0x3336, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Interaction
    //////////////////////////////////////////////////////////////////////////
    CMSG_LOGOUT_REQUEST                         = 0x34DB, ///< 7.3.5 25996
    CMSG_LOGOUT_CANCEL                          = 0x34DC, ///< 7.3.5 25996
    CMSG_LOGOUT_INSTANT                         = 0x34DD, ///< 7.3.5 25996
    CMSG_SET_ACTION_BUTTON                      = 0x3635, ///< 7.3.5 25996
    CMSG_SET_ACTION_BAR_TOGGLES                 = 0x3536, ///< 7.3.5 25996
    CMSG_SET_SELECTION                          = 0x352C, ///< 7.3.5 25996
    CMSG_SET_SHEATHED                           = 0x348B, ///< 7.3.5 25996
    CMSG_TALK_TO_GOSSIP                         = 0x3495, ///< 7.3.5 25996
    CMSG_GOSSIP_SELECT_OPTION                   = 0x3497, ///< 7.3.5 25996
    CMSG_TOGGLE_DIFFICULTY                      = 0x3657, ///< 7.3.5 25996
    CMSG_TOGGLE_PVP                             = 0x329C, ///< 7.3.5 25996
    CMSG_SET_PVP                                = 0x329D, ///< 7.3.5 25996
    CMSG_TUTORIAL_FLAG                          = 0x36DC, ///< 7.3.5 25996
    CMSG_SET_WATCHED_FACTION                    = 0x34E5, ///< 7.3.5 25996
    CMSG_SET_FACTION_INACTIVE                   = 0x34E4, ///< 7.3.5 25996
    CMSG_AREA_TRIGGER                           = 0x31CD, ///< 7.3.5 25996
    CMSG_GAMEOBJECT_USE                         = 0x34F2, ///< 7.3.5 25996
    CMSG_GAMEOBJECT_REPORT_USE                  = 0x34F3, ///< 7.3.5 25996
    CMSG_SAVE_CUF_PROFILES                      = 0x318A, ///< 7.3.5 25996
    CMSG_SPELL_CLICK                            = 0x3498, ///< 7.3.5 25996
    CMSG_REPOP_REQUEST                          = 0x352A, ///< 7.3.5 25996
    CMSG_RECLAIM_CORPSE                         = 0x34DF, ///< 7.3.5 25996
    CMSG_QUERY_CORPSE_LOCATION_FROM_CLIENT      = 0x3660, ///< 7.3.5 25996
    CMSG_QUERY_CORPSE_TRANSPORT                 = 0x3661, ///< 7.3.5 25996
    CMSG_RETURN_TO_GRAVEYARD                    = 0x352B, ///< 7.3.5 25996
    CMSG_CLOSE_INTERACTION                      = 0x3496, ///< 7.3.5 25996
    CMSG_ITEM_REFUND_INFO                       = 0x3533, ///< 7.3.5 25996
    CMSG_FAR_SIGHT                              = 0x34EC, ///< 7.3.5 25996
    CMSG_MOUNT_SPECIAL_ANIM                     = 0x3273, ///< 7.3.5 25996
    CMSG_OPENING_CINEMATIC                      = 0x3547, ///< 7.3.5 25996
    CMSG_NEXT_CINEMATIC_CAMERA                  = 0x3548, ///< 7.3.5 25996
    CMSG_COMPLETE_CINEMATIC                     = 0x3549, ///< 7.3.5 25996
    CMSG_REQUEST_CEMETERY_LIST                  = 0x3174, ///< 7.3.5 25996
    CMSG_TOTEM_DESTROYED                        = 0x34FE, ///< 7.3.5 25996
    CMSG_CONFIRM_RESPEC_WIPE                    = 0x3202, ///< 7.3.5 25996
    CMSG_CANCEL_TRADE                           = 0x315C, ///< 7.3.5 25996
    CMSG_SET_TRADE_CURRENCY                     = 0x3160, ///< 7.3.5 25996
    CMSG_SET_TRADE_GOLD                         = 0x315F, ///< 7.3.5 25996
    CMSG_SET_TRADE_ITEM                         = 0x315D, ///< 7.3.5 25996
    CMSG_CLEAR_TRADE_ITEM                       = 0x315E, ///< 7.3.5 25996
    CMSG_ACCEPT_TRADE                           = 0x315A, ///< 7.3.5 25996
    CMSG_BUSY_TRADE                             = 0x3158, ///< 7.3.5 25996
    CMSG_BEGIN_TRADE                            = 0x3157, ///< 7.3.5 25996
    CMSG_IGNORE_TRADE                           = 0x3159, ///< 7.3.5 25996
    CMSG_INITIATE_TRADE                         = 0x3156, ///< 7.3.5 25996
    CMSG_UNACCEPT_TRADE                         = 0x315B, ///< 7.3.5 25996
    CMSG_NEUTRAL_PLAYER_SELECT_FACTION          = 0x31CA, ///< 7.3.5 25996
    CMSG_INSPECT                                = 0x352D, ///< 7.3.5 25996
    CMSG_REQUEST_HONOR_STATS                    = 0x3179, ///< 7.3.5 25996
    CMSG_REQUEST_INSPECT_PVP                    = 0x36A1, ///< 7.3.5 25996
    CMSG_TIME_ADJUSTMENT_RESPONSE               = 0x3A3B, ///< 7.3.5 25996
    CMSG_TIME_SYNC_RESPONSE                     = 0x3A38, ///< 7.3.5 25996
    CMSG_TIME_SYNC_RESPONSE_DROPPED             = 0x3A3A, ///< 7.3.5 25996
    CMSG_TIME_SYNC_RESPONSE_FAILED              = 0x3A39, ///< 7.3.5 25996
    CMSG_DISCARDED_TIME_SYNC_ACKS               = 0x3A3C, ///< 7.3.5 25996
    CMSG_UNLEARN_SKILL                          = 0x34E9, ///< 7.3.5 25996
    CMSG_UNLEARN_SPECIALIZATION                 = 0x31A0, ///< 7.3.5 25996
    CMSG_EMOTE                                  = 0x3545, ///< 7.3.5 25996
    CMSG_SEND_TEXT_EMOTE                        = 0x348A, ///< 7.3.5 25996
    CMSG_ALTER_APPEARANCE                       = 0x34F9, ///< 7.3.5 25996
    CMSG_SELF_RES                               = 0x3535, ///< 7.3.5 25996
    CMSG_READ_ITEM                              = 0x3311, ///< 7.3.5 25996
    CMSG_COMPLETE_MOVIE                         = 0x34E1, ///< 7.3.5 25996
    CMSG_SCENE_TRIGGER_EVENT                    = 0x3217, ///< 7.3.5 25996
    CMSG_GET_MIRRORIMAGE_DATA                   = 0x3289, ///< 7.3.5 25996
    CMSG_SHOW_TRADE_SKILL                       = 0x36BF, ///< 7.3.5 25996
    CMSG_TRADE_SKILL_SET_FAVORITE               = 0x3335, ///< 7.3.5 25996
    CMSG_SCENE_PLAYBACK_CANCELED                = 0x3216, ///< 7.3.5 25996
    CMSG_SCENE_PLAYBACK_COMPLETE                = 0x3215, ///< 7.3.5 25996
    CMSG_REQUEST_RESEARCH_HISTORY               = 0x3167, ///< 7.3.5 25996
    CMSG_SUMMON_RESPONSE                        = 0x366A, ///< 7.3.5 25996
    CMSG_SET_FACTION_AT_WAR                     = 0x34E2, ///< 7.3.5 25996
    CMSG_SET_FACTION_NOT_AT_WAR                 = 0x34E3, ///< 7.3.5 25996
    CMSG_USE_CRITTER_ITEM                       = 0x3235, ///< 7.3.5 25996
    CMSG_SET_CURRENCY_FLAGS                     = 0x3169, ///< 7.3.5 25996
    CMSG_UPDATE_CLIENT_SETTINGS                 = 0x3664, ///< 7.3.5 25996
    CMSG_TABARD_VENDOR_ACTIVATE                 = 0x329A, ///< 7.3.5 25996
    CMSG_SAVE_CLIENT_VARIABLES                  = 0x36FF, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Vehicles
    //////////////////////////////////////////////////////////////////////////
    CMSG_MOVE_SET_VEHICLE_REC_ID_ACK            = 0x3A11, ///< 7.3.5 25996
    CMSG_RIDE_VEHICLE_INTERACT                  = 0x322F, ///< 7.3.5 25996
    CMSG_REQUEST_VEHICLE_EXIT                   = 0x322B, ///< 7.3.5 25996
    CMSG_REQUEST_VEHICLE_NEXT_SEAT              = 0x322D, ///< 7.3.5 25996
    CMSG_REQUEST_VEHICLE_PREV_SEAT              = 0x322C, ///< 7.3.5 25996
    CMSG_REQUEST_VEHICLE_SWITCH_SEAT            = 0x322E, ///< 7.3.5 25996
    CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE     = 0x3A31, ///< 7.3.5 25996
    CMSG_EJECT_PASSENGER                        = 0x3230, ///< 7.3.5 25996
    CMSG_MOVE_DISMISS_VEHICLE                   = 0x3A30, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Vendors
    //////////////////////////////////////////////////////////////////////////
    CMSG_LIST_INVENTORY                         = 0x34A4, ///< 7.3.5 25996
    CMSG_REPAIR_ITEM                            = 0x34F0, ///< 7.3.5 25996
    CMSG_BUYBACK_ITEM                           = 0x34A7, ///< 7.3.5 25996
    CMSG_BUY_ITEM                               = 0x34A6, ///< 7.3.5 25996
    CMSG_SELL_ITEM                              = 0x34A5, ///< 7.3.5 25996
    CMSG_ITEM_REFUND                            = 0x3534, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Taxi
    //////////////////////////////////////////////////////////////////////////
    CMSG_SET_TAXI_BENCHMARK_MODE                = 0x34F7, ///< 7.3.5 25996
    CMSG_ENABLE_TAXI_NODE                       = 0x34AC, ///< 7.3.5 25996
    CMSG_TAXI_QUERY_AVAILABLE_NODES             = 0x34AD, ///< 7.3.5 25996
    CMSG_ACTIVATE_TAXI                          = 0x34AE, ///< 7.3.5 25996
    CMSG_TAXI_NODE_STATUS_QUERY                 = 0x34AB, ///< 7.3.5 25996
    CMSG_TAXI_REQUEST_EARLY_LANDING             = 0x34AF, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Quest
    //////////////////////////////////////////////////////////////////////////
    CMSG_QUERY_QUEST_COMPLETION_NPCS            = 0x3173, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_HELLO                      = 0x3499, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_QUERY_QUEST                = 0x349A, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_COMPLETE_QUEST             = 0x349C, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_CHOOSE_REWARD              = 0x349D, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_STATUS_QUERY               = 0x349F, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_STATUS_MULTIPLE_QUERY      = 0x34A0, ///< 7.3.5 25996
    CMSG_QUEST_CONFIRM_ACCEPT                   = 0x34A1, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_ACCEPT_QUEST               = 0x349B, ///< 7.3.5 25996
    CMSG_PUSH_QUEST_TO_PARTY                    = 0x34A2, ///< 7.3.5 25996
    CMSG_QUEST_PUSH_RESULT                      = 0x34A3, ///< 7.3.5 25996
    CMSG_QUEST_LOG_REMOVE_QUEST                 = 0x3532, ///< 7.3.5 25996
    CMSG_QUEST_GIVER_REQUEST_REWARD             = 0x349E, ///< 7.3.5 25996
    CMSG_REQUEST_WORLD_QUEST_UPDATE             = 0x3337, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Account data
    //////////////////////////////////////////////////////////////////////////
    CMSG_GET_UNDELETE_CHARACTER_COOLDOWN_STATUS = 0x36DF, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Chat
    //////////////////////////////////////////////////////////////////////////
    /// Addon chat
    CMSG_CHAT_ADDON_MESSAGE_INSTANCE_CHAT       = 0x37F3, ///< 7.3.5 25996
    CMSG_CHAT_ADDON_MESSAGE_WHISPER             = 0x37D2, ///< 7.3.5 25996
    CMSG_CHAT_ADDON_MESSAGE_GUILD               = 0x37D4, ///< 7.3.5 25996
    CMSG_CHAT_ADDON_MESSAGE_OFFICER             = 0x37D6, ///< 7.3.5 25996
    CMSG_CHAT_ADDON_MESSAGE_RAID                = 0x37F1, ///< 7.3.5 25996
    CMSG_CHAT_ADDON_MESSAGE_PARTY               = 0x37EF, ///< 7.3.5 25996
    CMSG_CHAT_ADDON_MESSAGE_CHANNEL             = 0x37D0, ///< 7.3.5 25996
    CMSG_CHAT_REGISTER_ADDON_PREFIXES           = 0x37CD, ///< 7.3.5 25996
    CMSG_CHAT_UNREGISTER_ALL_ADDON_PREFIXES     = 0x37CE, ///< 7.3.5 25996
    CMSG_SAVE_ENABLED_ADDONS                    = 0x36FE, ///< 7.3.5 25996
    CMSG_ADDON_LIST                             = 0x35D8, ///< 7.3.5 25996

    /// Chat
    CMSG_CHAT_MESSAGE_RAID_WARNING              = 0x37F4, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_PARTY                     = 0x37EE, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_YELL                      = 0x37ED, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_SAY                       = 0x37EB, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_OFFICER                   = 0x37D5, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_EMOTE                     = 0x37EC, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_AFK                       = 0x37D7, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_DND                       = 0x37D8, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_GUILD                     = 0x37D3, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_RAID                      = 0x37F0, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_WHISPER                   = 0x37D1, ///< 7.3.5 25996
    CMSG_CHAT_MESSAGE_CHANNEL                   = 0x37CF, ///< 7.3.5 25996

    /// Channel
    CMSG_JOIN_CHANNEL                           = 0x37C8, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_ANNOUNCEMENTS             = 0x37E7, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_BAN                       = 0x37E5, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_DISPLAY_LIST              = 0x37DA, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_INVITE                    = 0x37E3, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_KICK                      = 0x37E4, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_LIST                      = 0x37D9, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_MODERATE                  = 0x37DE, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_MODERATOR                 = 0x37DF, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_MUTE                      = 0x37E1, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_OWNER                     = 0x37DD, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_PASSWORD                  = 0x37DB, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_SET_OWNER                 = 0x37DC, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_SILENCE_ALL               = 0x37E8, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_UNBAN                     = 0x37E6, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_UNMODERATOR               = 0x37E0, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_UNMUTE                    = 0x37E2, ///< 7.3.5 25996
    CMSG_CHAT_CHANNEL_UNSILENCE_ALL             = 0x37E9, ///< 7.3.5 25996
    CMSG_CHAT_DECLINE_CHANNEL_INVITE            = 0x37EA, ///< 7.3.5 25996
    CMSG_CHAT_LEAVE_CHANNEL                     = 0x37C9, ///< 7.3.5 25996
    CMSG_SILENCE_PARTY_TALKER                   = 0x3652, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Movement
    //////////////////////////////////////////////////////////////////////////
    /// Start
    CMSG_MOVE_START_FORWARD                     = 0x39E4, ///< 7.3.5 25996
    CMSG_MOVE_START_BACKWARD                    = 0x39E5, ///< 7.3.5 25996
    CMSG_MOVE_START_STRAFE_LEFT                 = 0x39E7, ///< 7.3.5 25996
    CMSG_MOVE_START_STRAFE_RIGHT                = 0x39E8, ///< 7.3.5 25996
    CMSG_MOVE_START_TURN_LEFT                   = 0x39EC, ///< 7.3.5 25996
    CMSG_MOVE_START_TURN_RIGHT                  = 0x39ED, ///< 7.3.5 25996
    CMSG_MOVE_START_SWIM                        = 0x39FA, ///< 7.3.5 25996
    CMSG_MOVE_START_ASCEND                      = 0x3A26, ///< 7.3.5 25996
    CMSG_MOVE_START_DESCEND                     = 0x3A2D, ///< 7.3.5 25996
    CMSG_MOVE_START_PITCH_UP                    = 0x39EF, ///< 7.3.5 25996
    CMSG_MOVE_START_PITCH_DOWN                  = 0x39F0, ///< 7.3.5 25996

    /// Misc
    CMSG_MOVE_JUMP                              = 0x39EA, ///< 7.3.5 25996
    CMSG_MOVE_DOUBLE_JUMP                       = 0x39EB, ///< 7.3.5 25996
    CMSG_MOVE_FALL_LAND                         = 0x39F9, ///< 7.3.5 25996
    CMSG_WORLD_PORT_RESPONSE                    = 0x35F9, ///< 7.3.5 25996
    CMSG_MOVE_TIME_SKIPPED                      = 0x3A18, ///< 7.3.5 25996
    CMSG_MOVE_SPLINE_DONE                       = 0x3A15, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_UNROOT_ACK                  = 0x3A0C, ///< 7.3.5 25996
    CMSG_MOVE_SET_RELATIVE_POSITION             = 0x0000, ///< opcode not found
    CMSG_MOVE_FALL_RESET                        = 0x3A16, ///< 7.3.5 25996
    CMSG_DISPLAY_PLAYER_CHOICE_RESPONSE         = 0x3293, ///< 7.3.5 25996
    CMSG_CONTRIBUTION_CONTRIBUTE                = 0x3558, ///< 7.3.5 25996
    CMSG_CONTRIBUTION_GET_STATE                 = 0x3559, ///< 7.3.5 25996

    /// Update
    CMSG_MOVE_SET_RUN_MODE                      = 0x39F2, ///< 7.3.5 25996
    CMSG_MOVE_SET_WALK_MODE                     = 0x39F3, ///< 7.3.5 25996
    CMSG_MOVE_TELEPORT_ACK                      = 0x39F8, ///< 7.3.5 25996
    CMSG_MOVE_HEARTBEAT                         = 0x3A0D, ///< 7.3.5 25996
    CMSG_MOVE_SET_FACING                        = 0x3A06, ///< 7.3.5 25996
    CMSG_MOVE_SET_PITCH                         = 0x3A07, ///< 7.3.5 25996
    CMSG_MOVE_CHANGE_TRANSPORT                  = 0x3A2C, ///< 7.3.5 25996
    CMSG_MOVE_SET_FLY                           = 0x3A25, ///< 7.3.5 25996
    CMSG_MOVE_REMOVE_MOVEMENT_FORCES            = 0x3A14, ///< 7.3.5 25996

    /// Stop
    CMSG_MOVE_STOP                              = 0x39E6, ///< 7.3.5 25996
    CMSG_MOVE_STOP_STRAFE                       = 0x39E9, ///< 7.3.5 25996
    CMSG_MOVE_STOP_SWIM                         = 0x39FB, ///< 7.3.5 25996
    CMSG_MOVE_STOP_TURN                         = 0x39EE, ///< 7.3.5 25996
    CMSG_MOVE_STOP_ASCEND                       = 0x3A27, ///< 7.3.5 25996
    CMSG_MOVE_STOP_PITCH                        = 0x39F1, ///< 7.3.5 25996

    /// ACK
    CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK            = 0x3A08, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_RUN_BACK_SPEED_CHANGE_ACK       = 0x3A09, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_WALK_SPEED_CHANGE_ACK           = 0x3A1E, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_SWIM_SPEED_CHANGE_ACK           = 0x3A0A, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_SWIM_BACK_SPEED_CHANGE_ACK      = 0x3A1F, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_FLIGHT_SPEED_CHANGE_ACK         = 0x3A2A, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK    = 0x3A2B, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_TURN_RATE_CHANGE_ACK            = 0x3A20, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_PITCH_RATE_CHANGE_ACK           = 0x3A2F, ///< 7.3.5 25996
    CMSG_MOVE_KNOCK_BACK_ACK                        = 0x3A0F, ///< 7.3.5 25996
    CMSG_MOVE_SET_CAN_FLY_ACK                       = 0x3A24, ///< 7.3.5 25996
    CMSG_MOVE_FEATHER_FALL_ACK                      = 0x3A19, ///< 7.3.5 25996
    CMSG_MOVE_WATER_WALK_ACK                        = 0x3A1A, ///< 7.3.5 25996
    CMSG_MOVE_HOVER_ACK                             = 0x3A10, ///< 7.3.5 25996
    CMSG_MOVE_APPLY_MOVEMENT_FORCE_ACK              = 0x3A12, ///< 7.3.5 25996
    CMSG_MOVE_REMOVE_MOVEMENT_FORCE_ACK             = 0x3A13, ///< 7.3.5 25996
    CMSG_MOVE_SET_IGNORE_MOVEMENT_FORCES_ACK        = 0x3A23, ///< 7.3.5 25996
    CMSG_MOVE_SET_COLLISION_HEIGHT_ACK              = 0x3A36, ///< 7.3.5 25996
    CMSG_MOVE_SET_CAN_TURN_WHILE_FALLING_ACK        = 0x3A22, ///< 7.3.5 25996
    CMSG_MOVE_GRAVITY_ENABLE_ACK                    = 0x3A33, ///< 7.3.5 25996
    CMSG_MOVE_GRAVITY_DISABLE_ACK                   = 0x3A32, ///< 7.3.5 25996
    CMSG_MOVE_FORCE_ROOT_ACK                        = 0x3A0B, ///< 7.3.5 25996
    CMSG_MOVE_ENABLE_SWIM_TO_FLY_TRANS_ACK          = 0x3A21, ///< 7.3.5 25996
    CMSG_MOVE_ENABLE_DOUBLE_JUMP_ACK                = 0x3A1B, ///< 7.3.5 25996

    /// Cheat
    CMSG_MOVE_TOGGLE_COLLISION_CHEAT                = 0x3A05, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Group
    //////////////////////////////////////////////////////////////////////////
    CMSG_PARTY_INVITE                           = 0x3602, ///< 7.3.5 25996
    CMSG_PARTY_INVITE_RESPONSE                  = 0x3603, ///< 7.3.5 25996
    CMSG_LEAVE_GROUP                            = 0x3649, ///< 7.3.5 25996
    CMSG_PARTY_UNINVITE                         = 0x3647, ///< 7.3.5 25996
    CMSG_SET_PARTY_LEADER                       = 0x364A, ///< 7.3.5 25996
    CMSG_SET_ROLE                               = 0x35D9, ///< 7.3.5 25996
    CMSG_MINIMAP_PING                           = 0x364B, ///< 7.3.5 25996
    CMSG_RANDOM_ROLL                            = 0x3654, ///< 7.3.5 25996
    CMSG_UPDATE_RAID_TARGET                     = 0x3650, ///< 7.3.5 25996
    CMSG_CONVERT_RAID                           = 0x364E, ///< 7.3.5 25996
    CMSG_DO_READY_CHECK                         = 0x3633, ///< 7.3.5 25996
    CMSG_READY_CHECK_RESPONSE                   = 0x3634, ///< 7.3.5 25996
    CMSG_INITIATE_ROLE_POLL                     = 0x35DA, ///< 7.3.5 25996
    CMSG_REQUEST_RAID_INFO                      = 0x36C7, ///< 7.3.5 25996
    CMSG_SET_EVERYONE_IS_ASSISTANT              = 0x3617, ///< 7.3.5 25996
    CMSG_CLEAR_RAID_MARKER                      = 0x31A1, ///< 7.3.5 25996
    CMSG_SET_ASSISTANT_LEADER                   = 0x364F, ///< 7.3.5 25996
    CMSG_SET_PARTY_ASSIGNMENT                   = 0x3651, ///< 7.3.5 25996
    CMSG_CHANGE_SUB_GROUP                       = 0x364C, ///< 7.3.5 25996
    CMSG_SWAP_SUB_GROUPS                        = 0x364D, ///< 7.3.5 25996
    CMSG_INSTANCE_LOCK_RESPONSE                 = 0x3511, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Friend
    //////////////////////////////////////////////////////////////////////////
    CMSG_SEND_CONTACT_LIST                      = 0x36CE, ///< 7.3.5 25996
    CMSG_ADD_FRIEND                             = 0x36CF, ///< 7.3.5 25996
    CMSG_ADD_IGNORE                             = 0x36D3, ///< 7.3.5 25996
    CMSG_SET_CONTACT_NOTES                      = 0x36D1, ///< 7.3.5 25996
    CMSG_DEL_FRIEND                             = 0x36D0, ///< 7.3.5 25996
    CMSG_DEL_IGNORE                             = 0x36D4, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Petition
    //////////////////////////////////////////////////////////////////////////
    CMSG_OFFER_PETITION                         = 0x36AF, ///< 7.3.5 25996
    CMSG_PETITION_BUY                           = 0x34CD, ///< 7.3.5 25996
    CMSG_PETITION_DECLINE                       = 0x3538, ///< 7.3.5 25996
    CMSG_PETITION_RENAME_GUILD                  = 0x36C6, ///< 7.3.5 25996
    CMSG_PETITION_SHOW_LIST                     = 0x34CC, ///< 7.3.5 25996
    CMSG_PETITION_SHOW_SIGNATURES               = 0x34CE, ///< 7.3.5 25996
    CMSG_SIGN_PETITION                          = 0x3537, ///< 7.3.5 25996
    CMSG_TURN_IN_PETITION                       = 0x3539, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Pet
    //////////////////////////////////////////////////////////////////////////
    CMSG_QUERY_PET_NAME                         = 0x3267, ///< 7.3.5 25996
    CMSG_PET_RENAME                             = 0x3685, ///< 7.3.5 25996
    CMSG_PET_ABANDON                            = 0x3490, ///< 7.3.5 25996
    CMSG_PET_ACTION                             = 0x348E, ///< 7.3.5 25996
    CMSG_PET_CANCEL_AURA                        = 0x3491, ///< 7.3.5 25996
    CMSG_PET_CAST_SPELL                         = 0x328D, ///< 7.3.5 25996
    CMSG_PET_SET_ACTION                         = 0x348D, ///< 7.3.5 25996
    CMSG_PET_STOP_ATTACK                        = 0x348F, ///< 7.3.5 25996
    CMSG_LIST_STABLE_PETS                       = 0x3494, ///< 7.3.5 25996
    CMSG_SET_PET_SLOT                           = 0x3168, ///< 7.3.5 25996
    CMSG_PET_SPELL_AUTOCAST                     = 0x3492, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Battlegrounds
    //////////////////////////////////////////////////////////////////////////
    CMSG_BATTLEMASTER_HELLO                     = 0x32A1, ///< 7.3.5 25996
    CMSG_BATTLEMASTER_JOIN                      = 0x3524, ///< 7.3.5 25996
    CMSG_BATTLEMASTER_JOIN_ARENA                = 0x3525, ///< 7.3.5 25996
    CMSG_BATTLEMASTER_JOIN_ARENA_SKIRMISH       = 0x3526, ///< 7.3.5 25996
    CMSG_JOIN_RATED_BATTLEGROUND                = 0x3176, ///< 7.3.5 25996
    CMSG_BATTLEFIELD_PORT                       = 0x3529, ///< 7.3.5 25996
    CMSG_REQUEST_BATTLEFIELD_STATUS             = 0x35DC, ///< 7.3.5 25996
    CMSG_PVP_LOG_DATA                           = 0x317A, ///< 7.3.5 25996
    CMSG_REQUEST_PVP_REWARDS                    = 0x3190, ///< 7.3.5 25996
    CMSG_REQUEST_PVP_BRAWL_INFO                 = 0x3191, ///< 7.3.5 25996
    CMSG_REQUEST_PVP_OPTIONS_ENABLED            = 0x35EE, ///< 7.3.5 25996
    CMSG_QUERY_COUNTDOWN_TIMER                  = 0x31A4, ///< 7.3.5 25996
    CMSG_REQUEST_CONQUEST_FORMULA_CONSTANTS     = 0x32A4, ///< 7.3.5 25996
    CMSG_BATTLEFIELD_LEAVE                      = 0x3171, ///< 7.3.5 25996
    CMSG_SPIRIT_HEALER_ACTIVATE                 = 0x34B2, ///< 7.3.5 25996
    CMSG_AREA_SPIRIT_HEALER_QUERY               = 0x34B3, ///< 7.3.5 25996
    CMSG_AREA_SPIRIT_HEALER_QUEUE               = 0x34B4, ///< 7.3.5 25996
    CMSG_BATTLEFIELD_LIST                       = 0x317D, ///< 7.3.5 25996
    CMSG_REQUEST_RATED_BATTLEFIELD_INFO         = 0x35E3, ///< 7.3.5 25996
    CMSG_START_SPECTATOR_WAR_GAME               = 0x35DF, ///< 7.3.5 25996
    CMSG_START_WAR_GAME                         = 0x35DE, ///< 7.3.5 25996
    CMSG_ACCEPT_WARGAME_INVITE                  = 0x35E0, ///< 7.3.5 25996
    CMSG_REPORT_PVP_PLAYER_AFK                  = 0x34F8, ///< 7.3.5 25996
    CMSG_PVP_PRESTIGE_RANK_UP                   = 0x3332, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Guild
    //////////////////////////////////////////////////////////////////////////
    CMSG_GUILD_INVITE_BY_NAME                      = 0x3606, ///< 7.3.5 25996
    CMSG_ACCEPT_GUILD_INVITE                       = 0x35FC, ///< 7.3.5 25996
    CMSG_GUILD_DECLINE_INVITATION                  = 0x3060, ///< 7.3.5 25996
    CMSG_GUILD_OFFICER_REMOVE_MEMBER               = 0x3063, ///< 7.3.5 25996
    CMSG_GUILD_SET_GUILD_MASTER                    = 0x36C5, ///< 7.3.5 25996
    CMSG_GUILD_LEAVE                               = 0x3062, ///< 7.3.5 25996
    CMSG_GUILD_DELETE                              = 0x3068, ///< 7.3.5 25996
    CMSG_GUILD_UPDATE_MOTD_TEXT                    = 0x3074, ///< 7.3.5 25996
    CMSG_GUILD_UPDATE_INFO_TEXT                    = 0x3075, ///< 7.3.5 25996
    CMSG_GUILD_GET_RANKS                           = 0x306D, ///< 7.3.5 25996
    CMSG_GUILD_ADD_RANK                            = 0x3064, ///< 7.3.5 25996
    CMSG_GUILD_DELETE_RANK                         = 0x3065, ///< 7.3.5 25996
    CMSG_GUILD_SET_RANK_PERMISSIONS                = 0x3067, ///< 7.3.5 25996
    CMSG_GUILD_SHIFT_RANK                          = 0x3066, ///< 7.3.5 25996
    CMSG_GUILD_ASSIGN_MEMBER_RANK                  = 0x305F, ///< 7.3.5 25996
    CMSG_GUILD_GET_ROSTER                          = 0x3073, ///< 7.3.5 25996
    CMSG_GUILD_BANK_ACTIVATE                       = 0x34B9, ///< 7.3.5 25996
    CMSG_GUILD_BANK_BUY_TAB                        = 0x34C8, ///< 7.3.5 25996
    CMSG_GUILD_BANK_DEPOSIT_MONEY                  = 0x34CA, ///< 7.3.5 25996
    CMSG_GUILD_BANK_LOG_QUERY                      = 0x3082, ///< 7.3.5 25996
    CMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY_QUERY = 0x3083, ///< 7.3.5 25996
    CMSG_GUILD_BANK_QUERY_TAB                      = 0x34C7, ///< 7.3.5 25996
    CMSG_GUILD_BANK_TEXT_QUERY                     = 0x3087, ///< 7.3.5 25996
    CMSG_GUILD_BANK_SWAP_ITEMS_LEGACY              = 0x34BA, ///< 7.3.5 25996
    CMSG_GUILD_BANK_UPDATE_TAB                     = 0x34C9, ///< 7.3.5 25996
    CMSG_GUILD_BANK_WITHDRAW_MONEY                 = 0x34CB, ///< 7.3.5 25996
    CMSG_GUILD_BANK_SET_TAB_TEXT                   = 0x3086, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK         = 0x34BB, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER         = 0x34BC, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK           = 0x34BF, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK_COUNT   = 0x34C1, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_COUNT   = 0x34C3, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK_COUNT     = 0x34C6, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_AUTO    = 0x34C4, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MERGE_ITEMS_PLAYER_BANK        = 0x34C0, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MERGE_ITEMS_BANK_PLAYER        = 0x34C2, ///< 7.3.5 25996
    CMSG_GUILD_BANK_MERGE_ITEMS_BANK_BANK          = 0x34C5, ///< 7.3.5 25996
    CMSG_GUILD_BANK_SWAP_ITEMS_BANK_PLAYER         = 0x34BD, ///< 7.3.5 25996
    CMSG_GUILD_BANK_SWAP_ITEMS_BANK_BANK           = 0x34BE, ///< 7.3.5 25996
    CMSG_GUILD_GET_ACHIEVEMENT_MEMBERS             = 0x3071, ///< 7.3.5 25996
    CMSG_GUILD_SET_FOCUSED_ACHIEVEMENT             = 0x3070, ///< 7.3.5 25996
    CMSG_GUILD_SET_ACHIEVEMENT_TRACKING            = 0x306F, ///< 7.3.5 25996
    CMSG_GUILD_SET_MEMBER_NOTE                     = 0x3072, ///< 7.3.5 25996
    CMSG_GUILD_CHALLENGE_UPDATE_REQUEST            = 0x307B, ///< 7.3.5 25996
    CMSG_REQUEST_GUILD_PARTY_STATE                 = 0x31A3, ///< 7.3.5 25996
    CMSG_REQUEST_GUILD_REWARDS_LIST                = 0x31A2, ///< 7.3.5 25996
    CMSG_GUILD_REPLACE_GUILD_MASTER                = 0x3088, ///< 7.3.5 25996
    CMSG_GUILD_CHANGE_NAME_REQUEST                 = 0x307E, ///< 7.3.5 25996
    CMSG_GUILD_PERMISSIONS_QUERY                   = 0x3084, ///< 7.3.5 25996
    CMSG_GUILD_EVENT_LOG_QUERY                     = 0x3085, ///< 7.3.5 25996
    CMSG_GUILD_NEWS_UPDATE_STICKY                  = 0x306E, ///< 7.3.5 25996
    CMSG_GUILD_QUERY_NEWS                          = 0x306C, ///< 7.3.5 25996
    CMSG_QUERY_GUILD_MEMBERS_FOR_RECIPE            = 0x306B, ///< 7.3.5 25996
    CMSG_QUERY_GUILD_MEMBER_RECIPES                = 0x3069, ///< 7.3.5 25996
    CMSG_QUERY_GUILD_RECIPES                       = 0x306A, ///< 7.3.5 25996
    CMSG_SAVE_GUILD_EMBLEM                         = 0x3299, ///< 7.3.5 25996

    /// Guild finding
    CMSG_LF_GUILD_ADD_RECRUIT                      = 0x361B, ///< 7.3.5 25996
    CMSG_LF_GUILD_BROWSE                           = 0x361D, ///< 7.3.5 25996
    CMSG_LF_GUILD_DECLINE_RECRUIT                  = 0x3078, ///< 7.3.5 25996
    CMSG_LF_GUILD_GET_APPLICATIONS                 = 0x3079, ///< 7.3.5 25996
    CMSG_LF_GUILD_GET_RECRUITS                     = 0x3077, ///< 7.3.5 25996
    CMSG_LF_GUILD_GET_GUILD_POST                   = 0x3076, ///< 7.3.5 25996
    CMSG_LF_GUILD_REMOVE_RECRUIT                   = 0x307A, ///< 7.3.5 25996
    CMSG_LF_GUILD_SET_GUILD_POST                   = 0x361C, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Battle pet opcodes
    //////////////////////////////////////////////////////////////////////////
    CMSG_BATTLE_PET_CLEAR_FANFARE                  = 0x312C, ///< 7.3.5 25996
    CMSG_QUERY_BATTLE_PET_NAME                     = 0x3268, ///< 7.3.5 25996
    CMSG_BATTLE_PET_UPDATE_NOTIFY                  = 0x31D6, ///< 7.3.5 25996
    CMSG_BATTLE_PET_REQUEST_JOURNAL_LOCK           = 0x3622, ///< 7.3.5 25996
    CMSG_BATTLE_PET_REQUEST_JOURNAL                = 0x3623, ///< 7.3.5 25996
    CMSG_BATTLE_PET_DELETE_PET                     = 0x3624, ///< 7.3.5 25996
    CMSG_BATTLE_PET_DELETE_PET_CHEAT               = 0x3625, ///< 7.3.5 25996
    CMSG_BATTLE_PET_MODIFY_NAME                    = 0x3627, ///< 7.3.5 25996
    CMSG_BATTLE_PET_SUMMON                         = 0x3628, ///< 7.3.5 25996
    CMSG_BATTLE_PET_SET_BATTLE_SLOT                = 0x362B, ///< 7.3.5 25996
    CMSG_BATTLE_PET_SET_FLAGS                      = 0x362F, ///< 7.3.5 25996
    CMSG_CAGE_BATTLE_PET                           = 0x31E8, ///< 7.3.5 25996

    CMSG_JOIN_PET_BATTLE_QUEUE                     = 0x31D4, ///< 7.3.5 25996
    CMSG_LEAVE_PET_BATTLE_QUEUE                    = 0x31D5, ///< 7.3.5 25996
    CMSG_PET_BATTLE_REQUEST_WILD                   = 0x31D0, ///< 7.3.5 25996
    CMSG_PET_BATTLE_WILD_LOCATION_FAIL             = 0x31D1, ///< 7.3.5 25996
    CMSG_PET_BATTLE_REQUEST_PVP                    = 0x31D2, ///< 7.3.5 25996
    CMSG_PET_BATTLE_REQUEST_UPDATE                 = 0x31D3, ///< 7.3.5 25996
    CMSG_PET_BATTLE_QUIT_NOTIFY                    = 0x31D8, ///< 7.3.5 25996
    CMSG_PET_BATTLE_FINAL_NOTIFY                   = 0x31D9, ///< 7.3.5 25996
    CMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH_RESULT     = 0x321A, ///< 7.3.5 25996
    CMSG_PET_BATTLE_INPUT                          = 0x3640, ///< 7.3.5 25996
    CMSG_PET_BATTLE_REPLACE_FRONT_PET              = 0x3641, ///< 7.3.5 25996
    CMSG_PET_BATTLE_SCRIPT_ERROR_NOTIFY            = 0x31DA, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Battle pay
    //////////////////////////////////////////////////////////////////////////
    CMSG_BATTLE_PAY_ACK_FAILED_RESPONSE            = 0x36CA, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_GET_PURCHASE_LIST              = 0x36BB, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_GET_PRODUCT_LIST_QUERY         = 0x36BA, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_START_PURCHASE                 = 0x36F2, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_CONFIRM_PURCHASE_RESPONSE      = 0x36C9, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_DISTRIBUTION_ASSIGN_TO_TARGET  = 0x36C0, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_QUERY_CLASS_TRIAL_BOOST_RESULT = 0x36C3, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_START_VAS_PURCHASE             = 0x36F3, ///< 7.3.5 25996
    CMSG_BATTLE_PAY_TRIAL_BOOST_CHARACTER          = 0x36C2, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// WoW Token
    //////////////////////////////////////////////////////////////////////////
    CMSG_CAN_REDEEM_WOW_TOKEN_FOR_BALANCE          = 0x3706, ///< 7.3.5 25996
    CMSG_REQUEST_WOW_TOKEN_MARKET_PRICE            = 0x36E6, ///< 7.3.5 25996
    CMSG_UPDATE_VAS_PURCHASE_STATES                = 0x36F4, ///< 7.3.5 25996
    CMSG_UPDATE_WOW_TOKEN_AUCTIONABLE_LIST         = 0x36EF, ///< 7.3.5 25996
    CMSG_UPDATE_WOW_TOKEN_COUNT                    = 0x36E5, ///< 7.3.5 25996
    CMSG_SELL_WOW_TOKEN_CONFIRM                    = 0x36E8, ///< 7.3.5 25996
    CMSG_SELL_WOW_TOKEN_START                      = 0x36E7, ///< 7.3.5 25996
    CMSG_REDEEM_WOW_TOKEN_CONFIRM                  = 0x36EE, ///< 7.3.5 25996
    CMSG_REDEEM_WOW_TOKEN_START                    = 0x36ED, ///< 7.3.5 25996
    CMSG_BUY_WOW_TOKEN_CONFIRM                     = 0x36EB, ///< 7.3.5 25996
    CMSG_BUY_WOW_TOKEN_START                       = 0x36EA, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// LFG
    //////////////////////////////////////////////////////////////////////////
    CMSG_DFGET_SYSTEM_INFO                         = 0x3612, ///< 7.3.5 25996
    CMSG_LFG_GET_STATUS                            = 0x3613, ///< 7.3.5 25996
    CMSG_LFG_JOIN                                  = 0x3608, ///< 7.3.5 25996
    CMSG_LFG_LEAVE                                 = 0x3611, ///< 7.3.5 25996
    CMSG_LFG_PROPOSAL_RESULT                       = 0x3607, ///< 7.3.5 25996
    CMSG_LFG_SET_BOOT_VOTE                         = 0x3615, ///< 7.3.5 25996
    CMSG_DFSET_ROLES                               = 0x3614, ///< 7.3.5 25996
    CMSG_LFG_TELEPORT                              = 0x3616, ///< 7.3.5 25996
    CMSG_RESET_INSTANCES                           = 0x3668, ///< 7.3.5 25996
    CMSG_SET_LFG_BONUS_FACTION_ID                  = 0x3294, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// LFG LIST
    //////////////////////////////////////////////////////////////////////////
    CMSG_LFG_LIST_GET_STATUS                        = 0x360A, ///< 7.3.5 25996
    CMSG_REQUEST_LFG_LIST_BLACKLIST                 = 0x3295, ///< 7.3.5 25996
    CMSG_LFG_LIST_JOIN                              = 0x3345, ///< 7.3.5 25996
    CMSG_LFG_LIST_UPDATE_REQUEST                    = 0x3346, ///< 7.3.5 25996
    CMSG_LFG_LIST_LEAVE                             = 0x3609, ///< 7.3.5 25996
    CMSG_LFG_LIST_SEARCH                            = 0x360B, ///< 7.3.5 25996
    CMSG_LFG_LIST_APPLY_TO_GROUP                    = 0x360C, ///< 7.3.5 25996
    CMSG_LFG_LIST_INVITE_APPLICANT                  = 0x360F, ///< 7.3.5 25996
    CMSG_LFG_LIST_DECLINE_APPLICANT                 = 0x360E, ///< 7.3.5 25996
    CMSG_LFG_LIST_CANCEL_APPLICATION                = 0x360D, ///< 7.3.5 25996
    CMSG_LFG_LIST_INVITE_RESPONSE                   = 0x3610, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Trainers
    //////////////////////////////////////////////////////////////////////////
    CMSG_TRAINER_LIST                              = 0x34B0, ///< 7.3.5 25996
    CMSG_TRAINER_BUY_SPELL                         = 0x34B1, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Void storage
    //////////////////////////////////////////////////////////////////////////
    CMSG_QUERY_VOID_STORAGE                        = 0x319D, ///< 7.3.5 25996
    CMSG_VOID_STORAGE_TRANSFER                     = 0x319E, ///< 7.3.5 25996
    CMSG_UNLOCK_VOID_STORAGE                       = 0x319C, ///< 7.3.5 25996
    CMSG_VOID_SWAP_ITEM                            = 0x319F, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Transmogrification
    //////////////////////////////////////////////////////////////////////////
    CMSG_TRANSMOGRIFY_ITEMS                        = 0x3192, ///< 7.3.5 25996
    CMSG_COLLECTION_ITEM_SET_FAVORITE              = 0x3632, ///< 7.3.5 25996 NYI

    //////////////////////////////////////////////////////////////////////////
    /// Mail
    //////////////////////////////////////////////////////////////////////////
    CMSG_MAIL_GET_LIST                             = 0x353A, ///< 7.3.5 25996
    CMSG_MAIL_CREATE_TEXT_ITEM                     = 0x353F, ///< 7.3.5 25996
    CMSG_MAIL_DELETE                               = 0x321B, ///< 7.3.5 25996
    CMSG_MAIL_MARK_AS_READ                         = 0x353E, ///< 7.3.5 25996
    CMSG_MAIL_RETURN_TO_SENDER                     = 0x3655, ///< 7.3.5 25996
    CMSG_MAIL_TAKE_ITEM                            = 0x353C, ///< 7.3.5 25996
    CMSG_MAIL_TAKE_MONEY                           = 0x353B, ///< 7.3.5 25996
    CMSG_SEND_MAIL                                 = 0x35FA, ///< 7.3.5 25996
    CMSG_QUERY_NEXT_MAIL_TIME                      = 0x353D, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Time
    //////////////////////////////////////////////////////////////////////////
    CMSG_UI_TIME_REQUEST                            = 0x369A, ///< 7.3.5 25996
    CMSG_QUERY_TIME                                 = 0x34DA, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// GameMasters
    //////////////////////////////////////////////////////////////////////////
    CMSG_SUPPORT_TICKET_SUBMIT_COMPLAINT            = 0x3644, ///< 7.3.5 25996
    CMSG_SUPPORT_TICKET_SUBMIT_BUG                  = 0x3645, ///< 7.3.5 25996
    CMSG_SUPPORT_TICKET_SUBMIT_SUGGESTION           = 0x3646, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// ToyBox
    //////////////////////////////////////////////////////////////////////////
    CMSG_ADD_TOY                                    = 0x328B, ///< 7.3.5 25996
    CMSG_USE_TOY                                    = 0x328C, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Calendar
    //////////////////////////////////////////////////////////////////////////
    CMSG_CALENDAR_ADD_EVENT                           = 0x367D, ///< 7.3.5 25996
    CMSG_CALENDAR_COMPLAIN                            = 0x3679, ///< 7.3.5 25996
    CMSG_CALENDAR_COPY_EVENT                          = 0x3678, ///< 7.3.5 25996
    CMSG_CALENDAR_EVENT_INVITE                        = 0x3672, ///< 7.3.5 25996
    CMSG_CALENDAR_EVENT_MODERATOR_STATUS              = 0x3676, ///< 7.3.5 25996
    CMSG_CALENDAR_EVENT_RSVP                          = 0x3674, ///< 7.3.5 25996
    CMSG_CALENDAR_EVENT_SIGN_UP                       = 0x367B, ///< 7.3.5 25996
    CMSG_CALENDAR_EVENT_STATUS                        = 0x3675, ///< 7.3.5 25996
    CMSG_CALENDAR_GET                                 = 0x366F, ///< 7.3.5 25996
    CMSG_CALENDAR_GET_EVENT                           = 0x3670, ///< 7.3.5 25996
    CMSG_CALENDAR_GET_NUM_PENDING                     = 0x367A, ///< 7.3.5 25996
    CMSG_CALENDAR_GUILD_FILTER                        = 0x3671, ///< 7.3.5 25996
    CMSG_CALENDAR_REMOVE_EVENT                        = 0x3677, ///< 7.3.5 25996
    CMSG_CALENDAR_REMOVE_INVITE                       = 0x3673, ///< 7.3.5 25996
    CMSG_CALENDAR_UPDATE_EVENT                        = 0x367E, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Challenges
    //////////////////////////////////////////////////////////////////////////
    CMSG_CHALLENGE_MODE_REQUEST_LEADERS             = 0x308F, ///< 7.3.5 25996
    CMSG_GET_CHALLENGE_MODE_REWARDS                 = 0x3683, ///< 7.3.5 25996
    CMSG_CHALLENGE_MODE_REQUEST_MAP_STATS           = 0x308E, ///< 7.3.5 25996
    CMSG_RESET_CHALLENGE_MODE                       = 0x31FB, ///< 7.3.5 25996
    CMSG_START_CHALLENGE_MODE                       = 0x354E, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// BlackMarket
    //////////////////////////////////////////////////////////////////////////
    CMSG_BLACK_MARKET_OPEN                         = 0x352F, ///< 7.3.5 25996
    CMSG_BLACK_MARKET_REQUEST_ITEMS                = 0x3530, ///< 7.3.5 25996
    CMSG_BLACK_MARKET_PLACE_BID                    = 0x3531, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Twitter
    //////////////////////////////////////////////////////////////////////////
    CMSG_REQUEST_TWITTER_STATUS                     = 0x312A, ///< 7.3.5 25996
    CMSG_TWITTER_CONNECT                            = 0x3127, ///< 7.3.5 25996
    CMSG_TWITTER_DISCONNECT                         = 0x312B, ///< 7.3.5 25996
    CMSG_TWITTER_POST                               = 0x331C, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Artifact
    //////////////////////////////////////////////////////////////////////////
    CMSG_ARTIFACT_ADD_POWER                         = 0x31A5, ///< 7.3.5 25996
    CMSG_ARTIFACT_CONFIRM_RESPEC                    = 0x31A6, ///< 7.3.5 25996
    CMSG_ARTIFACT_SET_APPEARANCE                    = 0x31A7, ///< 7.3.5 25996
    CMSG_ARTIFACT_ADD_RELIC_TALENT                  = 0x31A8, ///< 7.3.5 25996
    CMSG_ARTIFACT_ATTUNE_PREVIEW_RELIC              = 0x31A9, ///< 7.3.5 25996
    CMSG_ARTIFACT_ATTUNE_SOCKETED_RELIC             = 0x31AA, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Adventure Map
    //////////////////////////////////////////////////////////////////////////
    CMSG_GARRISON_REQUEST_SCOUTING_MAP              = 0x323B, ///< 7.3.5 25996
    CMSG_ADVENTURE_JOURNAL_START_QUEST              = 0x332E, ///< 7.3.5 25996
    CMSG_ADVENTURE_JOURNAL_OPEN_QUEST               = 0x31F9, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Battle.net
    //////////////////////////////////////////////////////////////////////////
    CMSG_ADD_BATTLENET_FRIEND                       = 0x365A, ///< 7.3.5 25996
    CMSG_BATTLENET_CHALLENGE_RESPONSE               = 0x36D2, ///< 7.3.5 25996
    CMSG_BATTLENET_REQUEST                          = 0x36F6, ///< 7.3.5 25996
    CMSG_BATTLENET_REQUEST_REALM_LIST_TICKET        = 0x36F7, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// Area POI
    //////////////////////////////////////////////////////////////////////////
    CMSG_REQUEST_AREA_POI_UPDATE                    = 0x3338, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    // CMSG
    CMSG_ACCEPT_LEVEL_GRANT                             = 0x34FD, ///< 7.3.5 25996
    CMSG_CANCEL_TEMP_ENCHANTMENT                        = 0x34F6, ///< 7.3.5 25996
    CMSG_COMMENTATOR_ENABLE                             = 0x35F0, ///< 7.3.5 25996
    CMSG_COMMENTATOR_ENTER_INSTANCE                     = 0x35F4, ///< 7.3.5 25996
    CMSG_COMMENTATOR_EXIT_INSTANCE                      = 0x35F5, ///< 7.3.5 25996
    CMSG_COMMENTATOR_GET_MAP_INFO                       = 0x35F1, ///< 7.3.5 25996
    CMSG_COMMENTATOR_GET_PLAYER_INFO                    = 0x35F2, ///< 7.3.5 25996
    CMSG_COMMENTATOR_START_WARGAME                      = 0x35EF, ///< 7.3.5 25996
    CMSG_COMPLAINT                                      = 0x366C, ///< 7.3.5 25996
    CMSG_CONNECT_TO_FAILED                              = 0x35D4, ///< 7.3.5 25996
    CMSG_DISMISS_CRITTER                                = 0x34FF, ///< 7.3.5 25996
    CMSG_GRANT_LEVEL                                    = 0x34FB, ///< 7.3.5 25996
    CMSG_HEARTH_AND_RESURRECT                           = 0x350C, ///< 7.3.5 25996
    CMSG_REQUEST_CATEGORY_COOLDOWNS                     = 0x317C, ///< 7.3.5 25996
    CMSG_REQUEST_PARTY_JOIN_UPDATES                     = 0x35F7, ///< 7.3.5 25996
    CMSG_REQUEST_PARTY_MEMBER_STATS                     = 0x3653, ///< 7.3.5 25996
    CMSG_SEND_SOR_REQUEST_VIA_ADDRESS                   = 0x3620, ///< 7.3.5 25996
    CMSG_SET_PREFERRED_CEMETERY                         = 0x3175, ///< 7.3.5 25996
    CMSG_SET_SAVED_INSTANCE_EXTEND                      = 0x3688, ///< 7.3.5 25996
    CMSG_BUG_REPORT                                     = 0x3686, ///< 7.3.5 25996
    CMSG_UPDATE_MISSILE_TRAJECTORY                      = 0x3A3E, ///< 7.3.5 25996
    CMSG_UPDATE_SPELL_VISUAL                            = 0x328F, ///< 7.3.5 25996
    CMSG_MISSILE_TRAJECTORY_COLLISION                   = 0x3189, ///< 7.3.5 25996
    CMSG_UPGRADE_ITEM                                   = 0x321D, ///< 7.3.5 25996
    CMSG_USED_FOLLOW                                    = 0x3185, ///< 7.3.5 25996
    CMSG_WARDEN_DATA                                    = 0x35EC, ///< 7.3.5 25996
    CMSG_WHOIS                                          = 0x3680, ///< 7.3.5 25996
    CMSG_WORLD_TELEPORT                                 = 0x0000, ///< opcode not found
    CMSG_WRAP_ITEM                                      = 0x3994, ///< 7.3.5 25996
    CMSG_REQUEST_CONSUMPTION_CONVERSION_INFO            = 0x36F8, ///< 7.3.5 25996
    CMSG_REPLACE_TROPHY                                 = 0x32F3, ///< 7.3.5 25996
    CMSG_RECRUIT_A_FRIEND                               = 0x36CC, ///< 7.3.5 25996
    CMSG_RAID_OR_BATTLEGROUND_ENGINE_SURVEY             = 0x36E4, ///< 7.3.5 25996
    CMSG_QUICK_JOIN_REQUEST_INVITE                      = 0x3704, ///< 7.3.5 25996
    CMSG_QUICK_JOIN_RESPOND_TO_INVITE                   = 0x3703, ///< 7.3.5 25996
    CMSG_QUICK_JOIN_AUTO_ACCEPT_REQUESTS                = 0x3705, ///< 7.3.5 25996
    CMSG_QUICK_JOIN_SIGNAL_TOAST_DISPLAYED              = 0x3702, ///< 7.3.5 25996
    CMSG_QUERY_SCENARIO_POI                             = 0x3656, ///< 7.3.5 25996
    CMSG_PROTOCOL_MISMATCH                              = 0x376E, ///< 7.3.5 25996
    CMSG_LOW_LEVEL_RAID1                                = 0x369F, ///< 7.3.5 25996
    CMSG_LOW_LEVEL_RAID2                                = 0x3518, ///< 7.3.5 25996
    CMSG_LOAD_SELECTED_TROPHY                           = 0x32F2, ///< 7.3.5 25996
    CMSG_LIVE_REGION_ACCOUNT_RESTORE                    = 0x36B9, ///< 7.3.5 25996
    CMSG_LIVE_REGION_CHARACTER_COPY                     = 0x36B8, ///< 7.3.5 25996
    CMSG_LIVE_REGION_GET_ACCOUNT_CHARACTER_LIST         = 0x36B7, ///< 7.3.5 25996
    CMSG_KEYBOUND_OVERRIDE                              = 0x3219, ///< 7.3.5 25996

    //////////////////////////////////////////////////////////////////////////
    /// GM
    //////////////////////////////////////////////////////////////////////////
    CMSG_GM_TICKET_GET_CASE_STATUS                      = 0x3691, ///< 7.3.5 25996
    CMSG_GM_TICKET_GET_SYSTEM_STATUS                    = 0x3690, ///< 7.3.5 25996

    /// SMSG
    SMSG_ADD_BATTLENET_FRIEND_RESPONSE                  = 0x265A, ///< 7.3.5 25996
    SMSG_BONUS_ROLL_EMPTY                               = 0x2662, ///< 7.3.5 25996
    SMSG_CONSOLE_WRITE                                  = 0x2651, ///< 7.3.5 25996
    SMSG_DISPLAY_PROMOTION                              = 0x2668, ///< 7.3.5 25996
    SMSG_DISPLAY_PLAYER_CHOICE                          = 0x26A1, ///< 7.3.5 25996
    SMSG_FORCE_OBJECT_RELINK                            = 0x2667, ///< 7.3.5 25996
    SMSG_REFRESH_COMPONENT                              = 0x2678, ///< 7.3.5 25996
    SMSG_SCENE_OBJECT_EVENT                             = 0x25F7, ///< 7.3.5 25996
    SMSG_SCENE_OBJECT_PET_BATTLE_FINISHED               = 0x25FD, ///< 7.3.5 25996
    SMSG_SCENE_OBJECT_PET_BATTLE_FIRST_ROUND            = 0x25F9, ///< 7.3.5 25996
    SMSG_SCENE_OBJECT_PET_BATTLE_FINAL_ROUND            = 0x25FC, ///< 7.3.5 25996
    SMSG_SCENE_OBJECT_PET_BATTLE_ROUND_RESULT           = 0x25FA, ///< 7.3.5 25996
    SMSG_SCENE_OBJECT_PET_BATTLE_REPLACEMENTS_MADE      = 0x25FB, ///< 7.3.5 25996
    SMSG_UPDATE_EXPANSION_LEVEL                         = 0x2663, ///< 7.3.5 25996
    SMSG_AREA_TRIGGER_DENIED                            = 0x269D, ///< 7.3.5 25996
    SMSG_ARENA_ERROR                                    = 0x2711, ///< 7.3.5 25996
    SMSG_BATTLEGROUND_INFO_THROTTLED                    = 0x259B, ///< 7.3.5 25996
    SMSG_CHARACTER_LOGIN_FAILED                         = 0x2744, ///< 7.3.5 25996
    SMSG_CLEAR_BOSS_EMOTES                              = 0x25CD, ///< 7.3.5 25996
    SMSG_COMBAT_EVENT_FAILED                            = 0x266F, ///< 7.3.5 25996
    SMSG_COMMENTATOR_MAP_INFO                           = 0x2746, ///< 7.3.5 25996
    SMSG_COMMENTATOR_PLAYER_INFO                        = 0x2747, ///< 7.3.5 25996
    SMSG_COMMENTATOR_STATE_CHANGED                      = 0x2745, ///< 7.3.5 25996
    SMSG_CUSTOM_LOAD_SCREEN                             = 0x25E3, ///< 7.3.5 25996
    SMSG_DAMAGE_CALC_LOG                                = 0x280C, ///< 7.3.5 25996
    SMSG_DIFFERENT_INSTANCE_FROM_PARTY                  = 0x258A, ///< 7.3.5 25996
    SMSG_DISENCHANT_CREDIT                              = 0x25BC, ///< 7.3.5 25996
    SMSG_DONT_AUTO_PUSH_SPELLS_TO_ACTION_BAR            = 0x25F6, ///< 7.3.5 25996
    SMSG_FAILED_PLAYER_CONDITION                        = 0x25E2, ///< 7.3.5 25996
    SMSG_GAMEOBJECT_RESET_STATE                         = 0x275D, ///< 7.3.5 25996
    SMSG_GAME_SPEED_SET                                 = 0x26AA, ///< 7.3.5 25996
    SMSG_GAME_TIME_SET                                  = 0x274B, ///< 7.3.5 25996
    SMSG_GAME_TIME_UPDATE                               = 0x274A, ///< 7.3.5 25996
    SMSG_GM_PLAYER_INFO                                 = 0x277A, ///< 7.3.5 25996
    SMSG_GOD_MODE                                       = 0x2738, ///< 7.3.5 25996
    SMSG_GROUP_ACTION_THROTTLED                         = 0x259C, ///< 7.3.5 25996
    SMSG_HEALTH_UPDATE                                  = 0x26FC, ///< 7.3.5 25996
    SMSG_INVALIDATE_PLAYER                              = 0x26D2, ///< 7.3.5 25996
    SMSG_INVALID_PROMOTION_CODE                         = 0x2795, ///< 7.3.5 25996
    SMSG_REMOVE_ITEM_PASSIVE                            = 0x25C0, ///< 7.3.5 25996
    SMSG_SEND_ITEM_PASSIVES                             = 0x25C1, ///< 7.3.5 25996
    SMSG_MAP_OBJ_EVENTS                                 = 0x25D9, ///< 7.3.5 25996
    SMSG_MESSAGE_BOX                                    = 0x2575, ///< 7.3.5 25996
    SMSG_MISSILE_CANCEL                                 = 0x25DA, ///< 7.3.5 25996
    SMSG_NOTIFY_MONEY                                   = 0x25AE, ///< 7.3.5 25996
    SMSG_MOVE_SET_COMPOUND_STATE                        = 0x2DE3, ///< 7.3.5 25996
    SMSG_MOVE_UPDATE_COLLISION_HEIGHT                   = 0x2DAD, ///< 7.3.5 25996
    SMSG_ABORT_NEW_WORLD                                = 0x25AD, ///< 7.3.5 25996
    SMSG_OPEN_CONTAINER                                 = 0x2764, ///< 7.3.5 25996
    SMSG_OPEN_LFG_DUNGEON_FINDER                        = 0x2A31, ///< 7.3.5 25996
    SMSG_OPEN_ALLIED_RACE_DETAILS_GIVER                 = 0x2837, ///< 7.3.5 25996
    SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT                = 0x2735, ///< 7.3.5 25996
    SMSG_PLAYER_SKINNED                                 = 0x2788, ///< 7.3.5 25996
    SMSG_PLAY_TIME_WARNING                              = 0x273A, ///< 7.3.5 25996
    SMSG_PROPOSE_LEVEL_GRANT                            = 0x2710, ///< 7.3.5 25996
    SMSG_SUMMON_RAID_MEMBER_VALIDATE_FAILED             = 0x258E, ///< 7.3.5 25996
    SMSG_REFER_A_FRIEND_EXPIRED                         = 0x2762, ///< 7.3.5 25996
    SMSG_REFER_A_FRIEND_FAILURE                         = 0x26EB, ///< 7.3.5 25996
    SMSG_REPORT_PVP_PLAYER_AFK_RESULT                   = 0x26D9, ///< 7.3.5 25996
    SMSG_SERVER_TIME                                    = 0x26AB, ///< 7.3.5 25996
    SMSG_SET_DF_FAST_LAUNCH_RESULT                      = 0x2A2E, ///< 7.3.5 25996
    SMSG_SOR_START_EXPERIENCE_INCOMPLETE                = 0x25F2, ///< 7.3.5 25996
    SMSG_STREAMING_MOVIES                               = 0x25BA, ///< 7.3.5 25996
    SMSG_SUMMON_CANCEL                                  = 0x26D8, ///< 7.3.5 25996
    SMSG_TIME_ADJUSTMENT                                = 0x2DA1, ///< 7.3.5 25996
    SMSG_WAIT_QUEUE_FINISH                              = 0x256E, ///< 7.3.5 25996
    SMSG_WAIT_QUEUE_UPDATE                              = 0x256D, ///< 7.3.5 25996
    SMSG_FORCE_ANIM                                     = 0x2794, ///< 7.3.5 25996
    SMSG_KICK_REASON                                    = 0x282F, ///< 7.3.5 25996
    SMSG_RESET_RANGED_COMBAT_TIMER                      = 0x2713, ///< 7.3.5 25996
    SMSG_SHOW_MAILBOX                                   = 0x27ED, ///< 7.3.5 25996
    SMSG_ADD_ITEM_PASSIVE                               = 0x25BF, ///< 7.3.5 25996
    SMSG_BAN_REASON                                     = 0x26B2, ///< 7.3.5 25996
    SMSG_BATTLENET_CHALLENGE_ABORT                      = 0x27CE, ///< 7.3.5 25996
    SMSG_BATTLENET_CHALLENGE_START                      = 0x27CD, ///< 7.3.5 25996
    SMSG_BATTLENET_NOTIFICATION                         = 0x2843, ///< 7.3.5 25996
    SMSG_BATTLENET_REALM_LIST_TICKET                    = 0x2845, ///< 7.3.5 25996
    SMSG_BATTLENET_SET_SESSION_STATE                    = 0x2844, ///< 7.3.5 25996
    SMSG_CAMERA_EFFECT                                  = 0x2767, ///< 7.3.5 25996
    SMSG_CHARACTER_UPGRADE_COMPLETE                     = 0x2803, ///< 7.3.5 25996
    SMSG_CHARACTER_UPGRADE_QUEUED                       = 0x2802, ///< 7.3.5 25996
    SMSG_CHARACTER_UPGRADE_SPELL_TIER_SET               = 0x25F4, ///< 7.3.5 25996
    SMSG_CHARACTER_UPGRADE_STARTED                      = 0x2801, ///< 7.3.5 25996
    SMSG_GARRISON_RENAME_FOLLOWER_RESPONSE              = 0x291A, ///< 7.3.5 25996

    SMSG_BATTLEFIELD_MGR_ENTERING                       = 0x0000, ///<
    SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE         = 0x0000, ///<
    SMSG_BATTLEFIELD_MGR_EJECT_PENDING                  = 0x0000, ///<
    SMSG_BATTLEFIELD_MGR_EJECTED                        = 0x0000, ///<
    SMSG_BATTLEFIELD_MGR_QUEUE_INVITE                   = 0x0000, ///<
    SMSG_BATTLEFIELD_MGR_STATE_CHANGED                  = 0x0000, ///<
    SMSG_BFMGR_ENTRY_INVITE                             = 0x0000, ///<
    SMSG_BATTLEFIELD_MGR_EXIT_REQUEST                   = 0x0000, ///<
};

/// Player state
enum SessionStatus
{
    STATUS_AUTHED = 0,                                          // Player authenticated (_player == NULL, m_playerRecentlyLogout = false or will be reset before handler call, m_GUID have garbage)
    STATUS_LOGGEDIN,                                            // Player in game (_player != NULL, m_GUID == _player->GetGUID(), inWorld())
    STATUS_TRANSFER,                                            // Player transferring to another map (_player != NULL, m_GUID == _player->GetGUID(), !inWorld())
    STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT,                        // _player != NULL or _player == NULL && m_playerRecentlyLogout && m_playerLogout, m_GUID store last _player guid)
    STATUS_NEVER,                                               // Opcode not accepted from client (deprecated or server side only)
    STATUS_UNHANDLED                                            // Opcode not handled yet
};

enum PacketProcessing
{
    PROCESS_INPLACE = 0,                                        // process packet whenever we receive it - mostly for non-handled or non-implemented packets
    PROCESS_THREADUNSAFE,                                       // packet is not thread-safe - process it in World::UpdateSessions()
    PROCESS_THREADSAFE                                          // packet is thread-safe - process it in Map::Update()
};

enum IRPacketProcessing
 {
     PROCESS_LOCAL           = 0,                            // Never send to interrealm
     PROCESS_DISTANT_IF_NEED = 1,                            // Send to interrealm if needed (player is on bg)
 };

class WorldPacket;
class WorldSession;

typedef void(WorldSession::*g_OpcodeHandlerType)(WorldPacket& recvPacket);

struct OpcodeHandler
{
    OpcodeHandler() {}
    OpcodeHandler(char const* _name, SessionStatus _status, PacketProcessing _processing, g_OpcodeHandlerType _handler, IRPacketProcessing _forwardToIR)
        : name(_name), status(_status), packetProcessing(_processing), handler(_handler), forwardToIR(_forwardToIR) {}

    char const* name;
    SessionStatus status;
    PacketProcessing packetProcessing;
    g_OpcodeHandlerType handler;
    IRPacketProcessing forwardToIR;
};

extern OpcodeHandler* g_OpcodeTable[TRANSFER_DIRECTION_MAX][NUM_OPCODE_HANDLERS];
void InitOpcodes();

ConnectionType GetOpcodeConnectionType(uint32 p_OpcodeID);


// Lookup opcode name for human understandable logging
inline std::string GetOpcodeNameForLogging(uint32 id, int p_Direction)
{
    std::ostringstream ss;
    ss << '[';

    if (id < UNKNOWN_OPCODE)
    {
        OpcodeHandler* handler = g_OpcodeTable[p_Direction][uint32(id) & 0x7FFF];

        if (!handler)
            ss << "UNKNOWN OPCODE";
        else
            ss << handler->name;
    }
    else
        ss << "INVALID OPCODE";

    ss << " 0x" << std::hex << std::uppercase << id << std::nouppercase << " (" << std::dec << id << ")]";
    return ss.str();
}


inline bool CanBeSentDuringInterRealm(uint16 id)
{
    switch (id)
    {
        case SMSG_CHAT:
        case SMSG_CONTACT_LIST:
        case SMSG_GUILD_BANK_QUERY_RESULTS:
        case SMSG_GUILD_COMMAND_RESULT:
        case SMSG_GUILD_INVITE:
        case SMSG_QUERY_GUILD_INFO_RESPONSE:
        case SMSG_GUILD_ROSTER:
            return true;
    }

    return false;
}

#endif
/// @}
