////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Opcodes.h"
#include "WorldSession.h"

OpcodeHandler* g_OpcodeTable[TRANSFER_DIRECTION_MAX][NUM_OPCODE_HANDLERS] = { };

template<bool isInValidRange, bool isNonZero>
inline void ValidateAndSetOpcode(uint16 /*p_Opcode*/, char const* /*p_Name*/, SessionStatus /*p_Status*/, PacketProcessing /*p_Processing*/, g_OpcodeHandlerType /*p_Handler*/, IRPacketProcessing /*forwardToIR*/)
{
    /// if for some reason we are here, that means NUM_OPCODE_HANDLERS == 0 (or your compiler is broken)
}

template<>
void ValidateAndSetOpcode<true, true>(uint16 p_Opcode, char const* p_Name, SessionStatus p_Status, PacketProcessing p_Processing, g_OpcodeHandlerType p_Handler, IRPacketProcessing forwardToIR)
{
    OpcodeTransferDirection l_OpcodeDirection = TRANSFER_DIRECTION_MAX;

    switch (p_Name[0])
    {
        case 'S':
        case 's':
            l_OpcodeDirection = WOW_SERVER_TO_CLIENT; // SMSG
            break;

        case 'C':
        case 'c':
            l_OpcodeDirection = WOW_CLIENT_TO_SERVER; // CMSG
            break;
    }

    if (l_OpcodeDirection == TRANSFER_DIRECTION_MAX)
    {
        for (uint8 l_CurrentDirection = 0; l_CurrentDirection < TRANSFER_DIRECTION_MAX; ++l_CurrentDirection)
        {
            if (g_OpcodeTable[l_CurrentDirection][p_Opcode] != NULL) // register MSG opcode as client and server
            {
                sLog->outError(LOG_FILTER_NETWORKIO, "Tried to override handler of %s with %s (opcode %u)", g_OpcodeTable[l_CurrentDirection][p_Opcode]->name, p_Name, p_Opcode);
                return;
            }

            g_OpcodeTable[l_CurrentDirection][p_Opcode] = new OpcodeHandler(p_Name, p_Status, p_Processing, p_Handler, forwardToIR);
        }

        return;
    }

    if (g_OpcodeTable[l_OpcodeDirection][p_Opcode] != NULL)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Tried to override handler of %s with %s (opcode %u)", g_OpcodeTable[l_OpcodeDirection][p_Opcode]->name, p_Name, p_Opcode);
        return;
    }

    g_OpcodeTable[l_OpcodeDirection][p_Opcode] = new OpcodeHandler(p_Name, p_Status, p_Processing, p_Handler, forwardToIR);
}

template<>
void ValidateAndSetOpcode<false, true>(uint16 p_Opcode, char const* /*p_Name*/, SessionStatus /*p_Status*/, PacketProcessing /*p_Processing*/, g_OpcodeHandlerType /*p_Handler*/, IRPacketProcessing /*forwardToIR*/)
{
    sLog->outError(LOG_FILTER_NETWORKIO, "Tried to set handler for an invalid opcode %d", p_Opcode);
}

template<>
void ValidateAndSetOpcode<true, false>(uint16 /*p_Opcode*/, char const* p_Name, SessionStatus /*p_Status*/, PacketProcessing /*p_Processing*/, g_OpcodeHandlerType /*p_Handler*/, IRPacketProcessing /*forwardToIR*/)
{
    sLog->outError(LOG_FILTER_NETWORKIO, "Opcode %s got value 0", p_Name);
}

#define DEFINE_OPCODE_HANDLER(p_Opcode, p_Status, p_Processing, p_Handler, forwardToIR)                                      \
    ValidateAndSetOpcode<(p_Opcode < NUM_OPCODE_HANDLERS), (p_Opcode != 0)>(p_Opcode, #p_Opcode, p_Status, p_Processing, p_Handler, forwardToIR);

/// Correspondence between opcodes and their names
void InitOpcodes()
{
    memset(g_OpcodeTable, 0, sizeof(g_OpcodeTable));

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Protocol
    //////////////////////////////////////////////////////////////////////////
    /// Opcodes
    DEFINE_OPCODE_HANDLER(SMSG_AUTH_CHALLENGE                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CRYPT_ENABLE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESUME_COMMS                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Dispatch
    //////////////////////////////////////////////////////////////////////////
    /// Base opcodes
    DEFINE_OPCODE_HANDLER(SMSG_AUTH_RESPONSE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CACHE_VERSION                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FEATURE_SYSTEM_STATUS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FEATURE_SYSTEM_STATUS_GLUE_SCREEN                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL); ///< NYI
    DEFINE_OPCODE_HANDLER(SMSG_SET_TIME_ZONE_INFORMATION                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOGIN_VERIFY_WORLD                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_NEW_WORLD                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOGIN_SET_TIME_SPEED                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AVAILABLE_HOTFIXES                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_HOTFIX_RESPONSE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WORLD_SERVER_INFO                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INITIAL_SETUP                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PONG                                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UI_TIME                                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CONNECT_TO                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Query
    DEFINE_OPCODE_HANDLER(SMSG_DB_REPLY                                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_CREATURE_RESPONSE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_NPC_TEXT_UPDATE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_NAME_QUERY_RESPONSE                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REALM_QUERY_RESPONSE                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_QUERY_RESPONSE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_TIME_RESPONSE                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_QUEST_REWARD_RESPONSE                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Misc
    DEFINE_OPCODE_HANDLER(SMSG_ACTIVE_GLYPHS                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_GAME_ERROR                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_PLAYER_CHOICE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_ACTION_BUTTONS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PROFICIENCY                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INIT_WORLD_STATES                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_WORLD_STATE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_EMOTE                                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_EXPLORATION_EXPERIENCE                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOG_XP_GAIN                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LEVELUP_INFO                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_PUSH_RESULT                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SETUP_CURRENCY                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_CURRENCY                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_CURRENCY_WEEK_LIMIT                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_RESET_CURRENCY                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_EQUIPMENT_SET_LIST                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BINDER_CONFIRM                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BIND_POINT_UPDATE                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_TALENT_DATA                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRANSFER_ABORTED                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRANSFER_PENDING                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INVENTORY_CHANGE_FAILURE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_DUNGEON_DIFFICULTY                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_RAID_DIFFICULTY                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOAD_CUF_PROFILES                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_STANDSTATE_UPDATE                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_START_TIMER                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_START_ELAPSED_TIMER                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_START_ELAPSED_TIMERS                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_STOP_ELAPSED_TIMER                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DEATH_RELEASE_LOC                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CORPSE_RECLAIM_DELAY                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CORPSE_LOCATION                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CORPSE_TRANSPORT_QUERY                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DURABILITY_DAMAGE_DEATH                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_MUSIC                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_OBJECT_SOUND                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_SOUND                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_TOAST                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_ENCHANT_TIME_UPDATE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ENCHANTMENT_LOG                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_KILL_LOG                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PROCRESIST                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_TARGET                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WEATHER                                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PHASE_SHIFT_CHANGE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRIGGER_CINEMATIC                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TEMPSUMMON_UI_TIMER                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESPEC_WIPE_CONFIRM                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_USE_EQUIPMENT_SET_RESULT                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRADE_STATUS                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRADE_UPDATED                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SHOW_NEUTRAL_PLAYER_FACTION_SELECT_UI                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT_TALENT                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT_HONOR_STATS                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT_PVP                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TIME_SYNC_REQUEST                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESUME_TOKEN                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SUSPEND_TOKEN                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PAGE_TEXT_QUERY_RESPONSE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PAGE_TEXT                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_READ_ITEM_FAILED                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_READ_ITEM_OK                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TEXT_EMOTE                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TITLE_EARNED                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TITLE_LOST                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WHOIS                                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRIGGER_MOVIE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_LAST_INSTANCE                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_INSTANCE_OWNERSHIP                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SUMMON_REQUEST                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_START_MIRROR_TIMER                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PAUSE_MIRROR_TIMER                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_STOP_MIRROR_TIMER                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SOCKET_GEMS                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESURRECT_REQUEST                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESPOND_INSPECT_ACHIEVEMENTS                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESET_FAILED_NOTIFY                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SETUP_RESEARCH_HISTORY                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESEARCH_COMPLETE                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARCHAEOLOGY_SURVERY_CAST                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REQUEST_CEMETERY_LIST_RESPONSE                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYED_TIME                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BREAK_TARGET                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_AUTO_REPEAT                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_COMBAT                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COMPLAIN_RESULT                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_XP_GAIN_ABORTED                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SUPERCEDED_SPELLS                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PVP_CREDIT                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PRE_RESURRECT                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_ONE_SHOT_ANIM_KIT                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_AI_ANIM_KIT                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_MELEE_ANIM_KIT                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_ANIM_TIER                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PLAY_HOVER_ANIM                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_MOVEMENT_ANIM_KIT                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_BOUND                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_OVERRIDE_LIGHT                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PRINT_NOTIFICATION                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPECIAL_MOUNT_ANIM                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_TIME_UPDATE                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_ITEM_TEXT_RESPONSE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_PURCHASE_REFUND_RESULT                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_DESPAWN                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_CUSTOM_ANIM                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAME_OBJECT_UI_ACTION                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAME_OBJECT_ACTIVATE_ANIM_KIT                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FISH_NOT_HOOKED                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FISH_ESCAPED                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CROSSED_INEBRIATION_THRESHOLD                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLIENT_CONTROL_UPDATE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_NO_CORPSE                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SORT_BAGS_RESULT                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_VIGNETTE_UPDATE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_MOUNT_UPDATE                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_SCENE                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_SCENE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TALENTS_INVOLUNTARILY_RESET                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_ITEM_PURCHASE_DATA                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SHOW_TRADE_SKILL_RESPONSE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FORCED_DEATH_UPDATE                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRANSMOG_COLLECTION_UPDATE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHANGE_ITEM                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CONTRIBUTION_SEND_STATE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_EXPIRE_PURCHASE_REFUND                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_CELESTIAL_BODY                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Control Alert
    DEFINE_OPCODE_HANDLER(SMSG_LOSS_OF_CONTROL_AURA_UPDATE                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ADD_LOSS_OF_CONTROL                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_LOSS_OF_CONTROL                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REMOVE_LOSS_OF_CONTROL                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Reputations
    DEFINE_OPCODE_HANDLER(SMSG_INITIALIZE_FACTIONS                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FACTION_BONUS_INFO                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_VISIBLE                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_NOT_VISIBLE                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_STANDING                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_ATWAR                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FORCED_REACTIONS                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Interaction
    DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_RESPONSE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_CANCEL_ACK                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_COMPLETE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_POI                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BARBER_SHOP_RESULT                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ENABLE_BARBER_SHOP                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// World Object management
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_OBJECT                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_POWER_UPDATE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ADJUST_SPLINE_DURATION                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Character list
    DEFINE_OPCODE_HANDLER(SMSG_ENUM_CHARACTERS_RESULT                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CREATE_CHAR                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_DELETE                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_FACTION_CHANGE_RESULT                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_RENAME                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_CUSTOMIZE_FAILED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_CUSTOMIZE                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GENERATE_RANDOM_CHARACTER_NAME_RESULT                    , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GET_ACCOUNT_CHARACTER_LIST_RESULT                        , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHARACTER_LOGIN_FAILED                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Account data
    DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_DATA_TIMES                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_ACCOUNT_DATA                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TUTORIAL_FLAGS                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Combat
    DEFINE_OPCODE_HANDLER(SMSG_ATTACKER_STATE_UPDATE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ATTACK_START                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ATTACK_STOP                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AI_REACTION                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ATTACK_SWING_ERROR                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Duel
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_REQUESTED                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_COMPLETE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_COUNTDOWN                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_INBOUNDS                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_OUT_OF_BOUNDS                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_WINNER                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CAN_DUEL_RESULT                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Vendor
    DEFINE_OPCODE_HANDLER(SMSG_LIST_INVENTORY                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BUY_FAILED                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BUY_ITEM                                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SELL_ITEM                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Achievement
    DEFINE_OPCODE_HANDLER(SMSG_ALL_ACHIEVEMENT_DATA                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ALL_ACCOUNT_CRITERIA                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ACHIEVEMENT_DELETED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ACHIEVEMENT_EARNED                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_CRITERIA_UPDATE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CRITERIA_DELETED                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CRITERIA_UPDATE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Friends
    DEFINE_OPCODE_HANDLER(SMSG_CONTACT_LIST                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FRIEND_STATUS                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Taxi
    DEFINE_OPCODE_HANDLER(SMSG_TAXI_NODE_STATUS                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_NEW_TAXI_PATH                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SHOW_TAXI_NODES                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ACTIVATE_TAXI_REPLY                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Loot
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_LIST                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_RESPONSE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_MONEY_NOTIFY                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_RELEASE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_RELEASE_ALL                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_REMOVED                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COIN_REMOVED                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ALL_PASSED                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MASTER_LOOT_CANDIDATE_LIST                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLL                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLLS_COMPLETE                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLL_WON                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_START_ROLL                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AE_LOOT_TARGETS                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AE_LOOT_TARGETS_ACK                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Bank
    DEFINE_OPCODE_HANDLER(SMSG_SHOW_BANK                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Rune
    DEFINE_OPCODE_HANDLER(SMSG_RESYNC_RUNES                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RUNE_REGEN_DEBUG                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ADD_RUNE_POWER                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Group
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_INVITE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_UPDATE                                             , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_MEMBER_STATE_FULL                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_MEMBER_STATE_PARTIAL                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_DECLINE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_COMMAND_RESULT                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_UNINVITE                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_NEW_LEADER                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ROLE_CHANGED_INFORM                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MINIMAP_PING                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_LOOT_METHOD_FAILED                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RANDOM_ROLL                                              , STATUS_NEVER,                        PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_RAID_TARGET_UPDATE_SINGLE                           , STATUS_NEVER,                        PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_RAID_TARGET_UPDATE_ALL                              , STATUS_NEVER,                        PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_READY_CHECK_RESPONSE                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_READY_CHECK_STARTED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_READY_CHECK_COMPLETED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ROLL_POLL_INFORM                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_DESTROYED                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Battleground
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_QUEUED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_NONE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_NEED_CONFIRMATION                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_ACTIVE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_FAILED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_WAIT_FOR_GROUPS                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_PORT_DENIED                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RATED_BATTLEFIELD_INFO                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_JOINED                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_LEFT                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_POSITIONS                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_ENTERING                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EJECT_PENDING                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EJECTED                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_STATE_CHANGED                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BFMGR_ENTRY_INVITE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EXIT_REQUEST                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REQUEST_PVP_REWARDS_RESPONSE                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PVP_OPTIONS_ENABLED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PVP_LOG_DATA                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARENA_PREP_OPPONENT_SPECIALIZATIONS                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARENA_CROWD_CONTROL_SPELLS                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DESTROY_ARENA_UNIT                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_POINTS                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_INIT                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MAP_OBJECTIVES_INIT                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CONQUEST_FORMULA_CONSTANTS                               , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PVP_SEASON                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WARGAME_REQUEST_SUCCESSFULLY_SENT_TO_OPPONENT            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPIRIT_HEALER_CONFIRM                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AREA_SPIRIT_HEALER_TIME                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_LIST                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHECK_WARGAME_ENTRY                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Pet
    DEFINE_OPCODE_HANDLER(SMSG_PET_NAME_QUERY_RESPONSE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_NAME_INVALID                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_ACTION_FEEDBACK                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_ACTION_SOUND                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_ADDED                                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_DISMISS_SOUND                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_GUIDS                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_MODE                                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_SLOT_UPDATED                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_STABLE_LIST                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PET_SPECIALIZATION                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_STABLE_RESULT                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Battle pet opcodes
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_UPDATES                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_TRAP_LEVEL                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL_LOCK_ACQUIRED                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL_LOCK_DENIED                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_DELETED                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_REVOKED                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_RESTORED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PETS_HEALED                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_LICENSE_CHANGED                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_ERROR                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_CAGE_DATE_ERROR                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_BATTLE_PET_NAME_RESPONSE                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Pet Battle opcodes
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_SLOT_UPDATES                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_REQUEST_FAILED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_PVPCHALLENGE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_FINALIZE_LOCATION                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_INITIAL_UPDATE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_FIRST_ROUND                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_ROUND_RESULT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_REPLACEMENTS_MADE                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_FINAL_ROUND                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_FINISHED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_CHAT_RESTRICTED                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_MAX_GAME_LENGTH_WARNING                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_QUEUE_STATUS                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_DEBUG_QUEUE_DUMP_RESPONSE                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Instances
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_ENGAGE_UNIT                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_DISENGAGE_UNIT                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_CHANGE_PRIORITY                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_TIMER_START                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_START                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_COMPLETE                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_START                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_UPDATE                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_END                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_IN_COMBAT_RESURRECTION                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_PHASE_SHIFT_CHANGED                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_GAIN_COMBAT_RESURRECTION_CHARGE       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_SET_ALLOWING_RELEASE                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_RESET                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_RESET_FAILED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_INSTANCE_INFO                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_GROUP_ONLY                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_MARKERS_CHANGED                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_SAVE_CREATED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_GROUP_SIZE_CHANGED                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ENCOUNTER_END                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ENCOUNTER_START                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BOSS_KILL_CREDIT                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PENDING_RAID_LOCK                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Scenarios
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_BOOT                                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_POIS                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_COMPLETED                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_PROGRESS_UPDATE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_STATE                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_SPELL_UPDATE                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    /// Auction House
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_HELLO_RESPONSE                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_BIDDER_ITEMS_RESULT                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_OUTBID_NOTIFICATION                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_WON_NOTIFICATION                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_COMMAND_RESULT                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_CLOSED_NOTIFICATION                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_PENDING_SALES_RESULT                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_ITEMS_RESULT                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_OWNER_ITEMS_RESULT                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_OWNER_BID_NOTIFICATION                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_REPLICATE_RESPONSE                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Mail
    DEFINE_OPCODE_HANDLER(SMSG_SEND_MAIL_RESULT                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MAIL_LIST_RESULT                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RECEIVED_MAIL                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MAIL_QUERY_NEXT_TIME_RESULT                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Trainers
    DEFINE_OPCODE_HANDLER(SMSG_TRAINER_LIST                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TRAINER_SERVICE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Void Storage
    DEFINE_OPCODE_HANDLER(SMSG_VOID_ITEM_SWAP_RESPONSE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_CONTENTS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_FAILED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_TRANSFER_CHANGES                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_VOID_TRANSFER_RESULT                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Petition
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_ALREADY_SIGNED                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_SIGN_RESULTS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_PETITION_RESPONSE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_SHOW_LIST                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_SHOW_SIGNATURES                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TURN_IN_PETITION_RESULTS                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_DECLINED                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Threat
    DEFINE_OPCODE_HANDLER(SMSG_THREAT_CLEAR                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_THREAT_REMOVE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_THREAT_UPDATE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_HIGHEST_THREAT_UPDATE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Tickets
    DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_SYSTEM_STATUS                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_CASE_STATUS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Toybox
    DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_TOYS_UPDATE                                      , STATUS_NEVER,                        PROCESS_THREADSAFE  , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// AreaTriggers
    DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_RE_PATH                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_RE_SHAPE                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Artifacts
    DEFINE_OPCODE_HANDLER(SMSG_ARTIFACT_KNOWLEDGE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARTIFACT_FORGE_OPENED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARTIFACT_RESPEC_CONFIRM                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARTIFACT_XP_GAIN                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARTIFACT_TRAITS_REFUNDED                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Adventure Map
    DEFINE_OPCODE_HANDLER(SMSG_ADVENTURE_MAP_OPEN                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_SCOUTING_MAP_RESULT                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Calendar
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_CLEAR_PENDING_ACTION                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_COMMAND_RESULT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INITIAL_INVITES                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_ALERT                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_NOTES                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_STATUS                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_MODERATOR_STATUS                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_REMOVED_ALERT                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_UPDATED_ALERT                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_ADDED                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_CALENDAR                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_EVENT                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_NUM_PENDING                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Warden
    DEFINE_OPCODE_HANDLER(SMSG_WARDEN_DATA                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Challenges
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_REQUEST_LEADERS_RESULT                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_MAP_STATS_UPDATE                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_ALL_MAP_STATS                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_START                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_COMPLETE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_REWARDS                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_Deprecated                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_RESET                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_UPDATE_DEATH_COUNT                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                                     , PROCESS_LOCAL);

    /// BlackMarket
    DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_OPEN_RESULT                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_OUTBID                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_REQUEST_ITEMS_RESULT                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_BID_ON_ITEM_RESULT                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_WON                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    // Battlepay
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_PURCHASE_LIST_RESPONSE                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_DISTRIBUTION_LIST_RESPONSE                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_START_PURCHASE_RESPONSE                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_PURCHASE_UPDATE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_CONFIRM_PURCHASE                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// WoW Token
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_BUY_REQUEST_CONFIRMATION                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_BUY_RESULT_CONFIRMATION                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_CAN_REDEEM_FOR_BALANCE_RESULT                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_CAN_VETERAN_BUY_RESULT                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_DISTRIBUTION_GLUE_UPDATE                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_DISTRIBUTION_UPDATE                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_MARKET_PRICE_RESPONSE                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_REDEEM_GAME_TIME_UPDATED                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_REDEEM_REQUEST_CONFIRMATION                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_REDEEM_RESULT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_SELL_REQUEST_CONFIRMATION                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_SELL_RESULT_CONFIRMATION                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WOW_TOKEN_UPDATE_AUCTIONABLE_LIST_RESPONSE               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Area POI
    DEFINE_OPCODE_HANDLER(SMSG_AREA_POI_UPDATE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Guild
    //////////////////////////////////////////////////////////////////////////
    /// Petition
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_RENAME                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Guild Finder
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_BROWSE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_APPLICATIONS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_APPLICATIONS_LIST_CHANGED                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_COMMAND_RESULT                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_RECRUITS                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_POST                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_APPLICANT_LIST_CHANGED                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Bank
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_RANKS                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_LOG_QUERY_RESULT                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_QUERY_TEXT_RESULT                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_QUERY_RESULTS                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_PERMISSIONS_QUERY_RESULTS                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Achievement
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_DELETED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_MEMBERS                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_EARNED                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_UPDATE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_DELETED                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ALL_GUILD_ACHIEVEMENTS                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Info
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ROSTER                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ROSTER_UPDATE                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_GUILD_INFO_RESPONSE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_UPDATE_NOTE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_REWARDS_LIST                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_SEND_RANK_CHANGE                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_UPDATED                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_RECIPES                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE_EXPIRED                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_COMMAND_RESULT                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_COMPLETED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_REPUTATION_REACTION_CHANGED                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_KNOWN_RECIPES                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_DAILY_RESET                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_FLAGGED_FOR_RENAME                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_NAME_CHANGED                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_RESET                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHANGE_NAME_RESULT                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE_DECLINED                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_PARTY_STATE                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBERS_WITH_RECIPE                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MOVE_STARTING                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MOVED                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_NEWS                                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_NEWS_DELETED                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_TABAR_VENDOR_SHOW                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_SAVE_GUILD_EMBLEM                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BROADCAST_LOOTED_ITEM                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Event system
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_NEW_LEADER                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_PRESENCE_CHANGE                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_PLAYER_JOINED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_PLAYER_LEFT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_RANKS_UPDATED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_RANK_CHANGED                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_LOG_QUERY_RESULTS                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_MOTD                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_DISBANDED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_ADDED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_MODIFIED                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_DELETED                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_TEXT_CHANGED                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_CONTENTS_CHANGED                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_MONEY_CHANGED                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Chat
    //////////////////////////////////////////////////////////////////////////
    /// Misc
    DEFINE_OPCODE_HANDLER(SMSG_SERVER_FIRST_ACHIEVEMENT                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_INSTANCE_MESSAGE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOTD                                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_EXPECTED_SPAM_RECORDS                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DEFENSE_MESSAGE                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ZONE_UNDER_ATTACK                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WHO                                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GET_DISPLAYED_TROPHY_LIST_RESPONSE                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Chat
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_SERVER_MESSAGE                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_RESTRICTED                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_RECONNECT                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_PLAYER_NOTFOUND                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_PLAYER_AMBIGUOUS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_NOT_IN_PARTY                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_IS_DOWN                                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_IGNORED_ACCOUNT_MUTED                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_DOWN                                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_AUTO_RESPONDED                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT                                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Channel
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY_LEFT                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY_JOINED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_LIST                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_USERLIST_ADD                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_USERLIST_REMOVE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_USERLIST_UPDATE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Twitter
    DEFINE_OPCODE_HANDLER(SMSG_TWITTER_STATUS                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_OAUTH_SAVED_DATA                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL); ///< disabled in 7.3.2 we dont have the opcodeID

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Move
    //////////////////////////////////////////////////////////////////////////
    /// Move speeds
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_SPEED                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_BACK_SPEED                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_SPEED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_BACK_SPEED                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_SPEED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_BACK_SPEED                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_WALK_SPEED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_TURN_RATE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_PITCH_RATE                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_MOVEMENT_FORCE_SPEED                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_KNOCK_BACK                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_SPEED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_BACK_SPEED                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_SPEED                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_BACK_SPEED                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_BACK_SPEED                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_SPEED                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_WALK_SPEED                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TURN_RATE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_PITCH_RATE                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_KNOCK_BACK                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Player
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_TELEPORT                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TELEPORT                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_ROOT                                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNROOT                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_WATER_WALK                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_LAND_WALK                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_FEATHER_FALL                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_NORMAL_FALL                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_FLY                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_FLY                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_ACTIVE_MOVER                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_HOVER                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_HOVER                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_TURN_WHILE_FALLING                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_TURN_WHILE_FALLING                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_VEHICLE_REC_ID                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_COLLISION_HEIGHT                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_APPLY_MOVEMENT_FORCE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_REMOVE_MOVEMENT_FORCE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SKIP_TIME                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_ENABLE_GRAVITY                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_DISABLE_GRAVITY                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_ENABLE_COLLISION                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_DISABLE_COLLISION                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_ENABLE_DOUBLE_JUMP                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_DISABLE_DOUBLE_JUMP                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Creature
    DEFINE_OPCODE_HANDLER(SMSG_MONSTER_MOVE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_SPEED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_SPEED                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_SPEED                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_TURN_RATE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_PITCH_RATE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_ROOT                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNROOT                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_DISABLE                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_DISABLE                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_ENABLE                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_ENABLE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FEATHER_FALL                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_NORMAL_FALL                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_HOVER                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_HOVER                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WATER_WALK                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_LAND_WALK                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_START_SWIM                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_STOP_SWIM                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_MODE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_MODE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLYING                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_FLYING                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FLIGHT_SPLINE_SYNC                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Spell
    //////////////////////////////////////////////////////////////////////////
    /// Aura
    DEFINE_OPCODE_HANDLER(SMSG_AURA_POINTS_DEPLETED                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AURA_UPDATE                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Misc
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_REGUID                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CHEAT_IGNORE_DIMISHING_RETURNS                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISPEL_FAILED                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MIRROR_IMAGE_COMPONENTED_DATA                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MIRROR_IMAGE_CREATURE_DATA                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_CLEAR_SPELLS                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REFRESH_SPELL_HISTORY                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESUME_CAST_BAR                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CAST_FAILED                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_FAILURE                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_FAILED_OTHER                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_CAST_FAILED                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_COOLDOWN                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MODIFY_COOLDOWN                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COOLDOWN_CHEAT                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_TAME_FAILURE                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FEIGN_DEATH_RESISTED                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_NOTIFY_DEST_LOC_SPELL_CAST                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_VEHICLE_REC_ID                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COOLDOWN_EVENT                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISMOUNT                                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISMOUNTRESULT                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOUNT_RESULT                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Spell Book / Bar
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_WEEKLY_SPELL_USAGE                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_SPELL_USAGE                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_KNOWN_SPELLS                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_SPELL_CHARGES                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_SPELL_HISTORY                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_UNLEARN_SPELLS                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_ALL_SPELL_CHARGES                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWN                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWNS                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CATEGORY_COOLDOWN                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_SPELL_CHARGES                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FLAT_SPELL_MODIFIER                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PCT_SPELL_MODIFIER                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_SPELL_CHARGES                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LEARNED_SPELL                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UNLEARNED_SPELLS                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LEARN_TALENTS_FAILED                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Casting
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_CHANNEL_START                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_CHANNEL_UPDATE                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_COOLDOWN                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_DAMAGE_SHIELD                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_DELAYED                                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_GO                                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_OR_DAMAGE_IMMUNE                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_START                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCRIPT_CAST                                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MODIFY_COOLDOWN_RECOVERY_SPEED                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MODIFY_CHARGE_RECOVERY_SPEED                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Logging
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_DISPELL_LOG                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_ENERGIZE_LOG                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_EXECUTE_LOG                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_NON_MELEE_DAMAGE_LOG                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_HEAL_LOG                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_INSTAKILL_LOG                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_INTERRUPT_LOG                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_MISS_LOG                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ENVIRONMENTAL_DAMAGE_LOG                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_PERIODIC_AURA_LOG                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_ABSORB_LOG                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_PROC_SCRIPT_LOG                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_PROCS_PER_MINUTE_LOG                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_CHANCE_PROC_LOG                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESIST_LOG                                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AURA_CAST_LOG                                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Pet
    DEFINE_OPCODE_HANDLER(SMSG_PET_SPELLS_MESSAGE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_UNLEARNED_SPELLS                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PET_LEARNED_SPELLS                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Visuals
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_ORPHAN_SPELL_VISUAL                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_ORPHAN_SPELL_VISUAL                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_SPELL_VISUAL                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL_KIT                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_SPELL_VISUAL_KIT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Quest
    //////////////////////////////////////////////////////////////////////////
    /// Quest log
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_QUEST_INFO_RESPONSE                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_FAILED_TIMER                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_FAILED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_COMPLETE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_COMPLETE_BY_SPELL                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_ADD_PVP_CREDIT                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_ADD_CREDIT_SIMPLE                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_ADD_CREDIT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_PUSH_RESULT                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_POIQUERY_RESPONSE                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_LOG_FULL                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_IS_QUEST_COMPLETE_RESPONSE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_FORCE_REMOVED                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_CONFIRM_ACCEPT                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_COMPLETION_NPCRESPONSE                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DAILY_QUESTS_RESET                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WORLD_QUEST_UPDATE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_ALL_TASK_PROGRESS                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_TASK_PROGRESS                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_TASK_COMPLETE                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// Quest giver
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_STATUS_MULTIPLE                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_STATUS                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_REQUEST_ITEMS                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_FAILED                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_DETAILS                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_COMPLETE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_INVALID_QUEST                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_SPAWN_TRACKING_UPDATE                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    /// Gossip
    DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_MESSAGE                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_COMPLETE                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Lfg
    //////////////////////////////////////////////////////////////////////////
    /// LFG
    DEFINE_OPCODE_HANDLER(SMSG_LFG_BOOT_PLAYER                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_DISABLED                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_JOIN_RESULT                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_OFFER_CONTINUE                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PARTY_INFO                                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PLAYER_INFO                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PLAYER_REWARD                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PROPOSAL_UPDATE                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_QUEUE_STATUS                                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_ROLE_CHECK_UPDATE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_ROLE_CHOSEN                                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_SLOT_INVALID                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_TELEPORT_DENIED                                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_STATUS                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_DUNGEON_ENCOUNTER_FOR_LOOT                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    /// LFG List
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_JOIN_RESULT                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_UPDATE_BLACKLIST                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_UPDATE_STATUS                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_SEARCH_RESULT                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_SEARCH_STATUS                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_APPLY_FOR_GROUP_RESULT                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_APPLICANT_LIST_UPDATE                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_APPLICANT_GROUP_INVITE                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Garrison
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_SHIPMENT_NPC_FROM_GOSSIP                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GET_SHIPMENT_INFO_RESPONSE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_SHIPMENT_NPC_RESULT                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CREATE_SHIPMENT_RESPONSE                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COMPLETE_SHIPMENT_RESPONSE                               , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GET_SHIPMENTS_OF_TYPE_RESPONSE                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LANDING_PAGE_SHIPMENT_INFO                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ADVENTURE_LIST_UPDATE                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    DEFINE_OPCODE_HANDLER(SMSG_GET_GARRISON_INFO_RESULT                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_PLOT_PLACED                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_PLOT_REMOVED                                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_PLACE_BUILDING_RESULT                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BUILDING_REMOVED                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LEARN_SPECIALIZATION_RESULT                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BUILDING_SET_ACTIVE_SPECIALIZATION_RESULT       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LEARN_BLUEPRINT_RESULT                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UNLEARN_BLUEPRINT_RESULT                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA_RESULT, STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REMOTE_INFO                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BUILDING_ACTIVATED                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_CREATE_RESULT                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UPGRADE_RESULT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_ADD_FOLLOWER_RESULT                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REMOVE_FOLLOWER_RESULT                          , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_FOLLOWER_CHANGED_DURABILITY                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LIST_FOLLOWERS_CHEAT_RESULT                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_ADD_MISSION_RESULT                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_START_MISSION_RESULT                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_COMPLETE_MISSION_RESULT                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UPDATE_MISSION                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_RECALL_PORTAL_LAST_USED_TIME                    , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_RECALL_PORTAL_USED                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_MISSION_BONUS_ROLL_RESULT                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_MISSION_AREA_BONUS_ADDED                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_MISSION_UPDATE_CAN_START                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_FOLLOWER_CHANGED_XP                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_FOLLOWER_CHANGED_ITEM_LEVEL                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_FOLLOWER_CHANGED_ABILITIES                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_FOLLOWER_CHANGED_STATUS                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_CLEAR_ALL_FOLLOWERS_EXHAUSTION                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_NUM_FOLLOWER_ACTIVATIONS_REMAINING              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING_RESULT              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING_RESULT            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_OPEN_RECRUITMENT_NPC                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_RECRUITMENT_FOLLOWERS_GENERATED                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_RECRUIT_FOLLOWER_RESULT                         , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_DELETE_RESULT                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_OPEN_ARCHITECT                                  , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_OPEN_TRADESKILL_NPC                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_OPEN_MISSION_NPC                                , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_IS_UPGRADEABLE_RESULT                           , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LIST_MISSIONS_CHEAT_RESULT                      , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BUILDING_LANDMARKS                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_OPEN_TALENT_NPC                                 , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UPDATE_TALENT                                   , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UNK_MISSION_OPCODE                              , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_ServerSide                                               , PROCESS_LOCAL);

#ifndef CROSS
    //////////////////////////////////////////////////////////////////////////
    /// CMSG
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GARRISON_CREATURE_NAME                             , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPGRADE_GARRISON                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleUpgradeGarrisonOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GET_GARRISON_INFO                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGetGarrisonInfoOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRequestGarrisonRequestBlueprintAndSpecializationDataOpcode, PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_PURCHASE_BUILDING                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonPurchaseBuildingOpcode                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_BUILDING_ACTIVE                             , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_CANCEL_CONSTRUCTION                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonCancelConstructionOpcode                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SWAP_BUILDINGS                                  , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_FOLLOWER_INACTIVE                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonChangeFollowerActivationStateOpcode               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_FOLLOWER_FAVORITE                           , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonAssignFollowerToBuilding                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonRemoveFollowerFromBuilding                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_RENAME_FOLLOWER                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonFollowerRename                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_GENERATE_RECRUITS                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonGenerateRecruitsOpcode                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_RECRUITMENT_PREFERENCES                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonSetRecruitmentPreferencesOpcode                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_RECRUIT_FOLLOWER                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonRecruitFollower                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_RESEARCH_TALENT                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonResearchTalent                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_CLASS_SPEC_CATEGORY_INFO                , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_OPEN_MISSION_NPC                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonMissionNPCHelloOpcode                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_USING_PARTY_GARRISON                                 , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_GET_BUILDING_LANDMARKS                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonGetBuildingLandmarkOpcode                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_OPEN_SHIPMENT_NPC                                        , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_SHIPMENT_INFO                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonGetShipmentInfoOpcode                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_LANDING_PAGE_SHIPMENT_INFO              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRequestLandingPageShipmentInfoOpcode                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CREATE_SHIPMENT                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonCreateShipmentOpcode                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_OPEN_TRADESKILL_NPC                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonRequestSetMissionNPC                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REMOVE_FOLLOWER                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonRemoveFollowerOpcode                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_START_MISSION                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonStartMissionOpcode                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_COMPLETE_MISSION                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonCompleteMissionOpcode                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_MISSION_BONUS_ROLL                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGarrisonMissionBonusRollOpcode                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_CHECK_UPGRADEABLE                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRequestGarrisonUpgradeableOpcode                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_DISPLAY_PLAYER_CHOICE_RESPONSE                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleDisplayPlayerChoiceResponse                               , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// User Router
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_CRYPT_ENABLED                                            , STATUS_NEVER,                        PROCESS_THREADUNSAFE, &WorldSession::Handle_EarlyProccess                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LOG_STREAMING_ERROR                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUEUED_MESSAGES_END                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LOG_DISCONNECT                                           , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::Handle_EarlyProccess                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PING                                                     , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_EarlyProccess                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUTH_CONTINUED_SESSION                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SUSPEND_TOKEN_RESPONSE                                   , STATUS_TRANSFER,                     PROCESS_INPLACE     , &WorldSession::HandleSuspendTokenResponse                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AUTH_SESSION                                             , STATUS_NEVER,                        PROCESS_THREADUNSAFE, &WorldSession::Handle_EarlyProccess                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ENABLE_NAGLE                                             , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_EarlyProccess                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SUSPEND_COMMS_ACK                                        , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_KEEP_ALIVE                                               , STATUS_NEVER,                        PROCESS_THREADUNSAFE, &WorldSession::Handle_EarlyProccess                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_OBJECT_UPDATE_FAILED                                     , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleObjectUpdateFailedOpcode                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_OBJECT_UPDATE_RESCUED                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Character
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_ENUM_CHARACTERS                                          , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandleCharEnumOpcode                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CREATE_CHARACTER                                         , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandleCharCreateOpcode                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GENERATE_RANDOM_CHARACTER_NAME                           , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandleRandomizeCharNameOpcode                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_DELETE                                              , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandleCharDeleteOpcode                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REORDER_CHARACTERS                                       , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleReorderCharacters                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PLAYER_LOGIN                                             , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandlePlayerLoginOpcode                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_VIOLENCE_LEVEL                                           , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleViolenceLevel                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_RACE_OR_FACTION_CHANGE                              , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandleCharRaceOrFactionChange                                   , PROCESS_LOCAL);
#endif

    DEFINE_OPCODE_HANDLER(CMSG_LOADING_SCREEN_NOTIFY                                    , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleLoadScreenOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_ACCOUNT_DATA                                     , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleRequestAccountData                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_ACCOUNT_DATA                                      , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleUpdateAccountData                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_DUNGEON_DIFFICULTY                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetDungeonDifficultyOpcode                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_RAID_DIFFICULTY                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetRaidDifficultyOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTIVE_MOVER                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetActiveMoverOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LEARN_TALENTS                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleLearnTalents                                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LEARN_PVP_TALENTS                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleLearnPvPTalents                                           , PROCESS_DISTANT_IF_NEED);

    DEFINE_OPCODE_HANDLER(CMSG_AUTOEQUIP_ITEM                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAutoEquipItemOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SWAP_INV_ITEM                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSwapInvItemOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SWAP_ITEM                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSwapItem                                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_BAG_ITEM                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAutoStoreBagItemOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PET_INFO                                         , STATUS_UNHANDLED,                    PROCESS_THREADSAFE  , &WorldSession::HandleRequestPetInfoOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SAVE_CUF_PROFILES                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSaveCUFProfiles                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_STAND_STATE_CHANGE                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleStandStateChangeOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BINDER_ACTIVATE                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleBinderActivateOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_FORCED_REACTIONS                                 , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleForcedReactionsOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_DESTROY_ITEM                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleDestroyItemOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_OPEN_ITEM                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleOpenItemOpcode                                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_TITLE                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetTitleOpcode                                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PLAYED_TIME                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePlayedTime                                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SAVE_EQUIPMENT_SET                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleEquipmentSetSave                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_USE_EQUIPMENT_SET                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleEquipmentSetUse                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_DELETE_EQUIPMENT_SET                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleEquipmentSetDelete                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_WHO                                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleWhoOpcode                                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SOCKET_GEMS                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSocketOpcode                                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_RESURRECT_RESPONSE                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleResurrectResponseOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_INSPECT_ACHIEVEMENTS                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQueryInspectAchievements                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SPLIT_ITEM                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSplitItemOpcode                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_PLAYER_DECLINED_NAMES                                , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleSetPlayerDeclinedNames                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SORT_BAGS                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSortBags                                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOUNT_SET_FAVORITE                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMountSetFavoriteOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_GET_MIRRORIMAGE_DATA                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMirrorImageDataRequest                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SHOW_TRADE_SKILL                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleShowTradeSkillOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RESEARCH_HISTORY                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleArcheologyRequestHistory                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_SORT_BAGS_RIGHT_TO_LEFT                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_INSERT_ITEMS_LEFT_TO_RIGHT                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_BANK_AUTOSORT_DISABLED                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_RENAME                                              , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandleCharRenameOpcode                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_CUSTOMIZE                                           , STATUS_AUTHED,                       PROCESS_THREADUNSAFE, &WorldSession::HandleCharCustomize                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUTO_DECLINE_GUILD_INVITES                               , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleAutoDeclineGuildInvites                                   , PROCESS_DISTANT_IF_NEED);

#endif

    //////////////////////////////////////////////////////////////////////////
    /// Bank
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_BANKER_ACTIVATE                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleBankerActivateOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AUTOBANK_ITEM                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleAutoBankItemOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_BANK_ITEM                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleAutoStoreBankItemOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AUTOBANK_REAGENT                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleAutoBankReagentOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_BANK_REAGENT                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleAutoStoreBankReagentOpcode                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BUY_BANK_SLOT                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleBuyBankSlotOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BUY_REAGENT_BANK                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleBuyReagentBankOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SORT_BANK_BAGS                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleSortReagentBankBagsOpcode                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SORT_REAGENT_BANK_BAGS                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleSortReagentBankBagsOpcode                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHANGE_BAG_SLOT_FLAG                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleChangeBagSlotFlagOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_BANK_BAG_SLOT_FLAG                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleChangeBankBagSlotFlagOpcode                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_BACKPACK_AUTOSORT_DISABLED                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleSetBackpackAutoSortDisable                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_DEPOSIT_ALL_REAGENTS                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleDepositAllReagentsOpcode                                  , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Auction House
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_BIDDER_ITEMS                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAuctionListBidderItems                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_ITEMS                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAuctionListItems                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_OWNER_ITEMS                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAuctionListOwnerItems                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_PENDING_SALES                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleAuctionListPendingSales                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_PLACE_BID                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAuctionPlaceBid                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_REMOVE_ITEM                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAuctionRemoveItem                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_SELL_ITEM                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAuctionSellItem                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_REPLICATE_ITEMS                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleReplicateItems                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_HELLO_REQUEST                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAuctionHelloOpcode                                        , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// Loot
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_UNIT                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleLootOpcode                                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_MONEY                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleLootMoneyOpcode                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_ITEM                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleLootItemOpcode                                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_RELEASE                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleLootReleaseOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_ROLL                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleLootRoll                                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MASTER_LOOT_ITEM                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMasterLootItemOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_DO_MASTER_LOOT_ROLL                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleDoMasterLootRollOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_LOOT_SPECIALIZATION                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetLootSpecialization                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_LOOT_METHOD                                          , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleSetLootMethodOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_OPT_OUT_OF_LOOT                                          , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleOptOutOfLootOpcode                                        , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Combat
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_ATTACK_STOP                                              , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleAttackStopOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ATTACK_SWING                                             , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleAttackSwingOpcode                                         , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Duel
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_CAN_DUEL                                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSendDuelRequest                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_DUEL_RESPONSE                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleDuelResponseOpcode                                        , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Spell
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_CAST_SPELL                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCastSpellOpcode                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_CAST                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCancelCastOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_USE_ITEM                                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleUseItemOpcode                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_AURA                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCancelAuraOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_AUTO_REPEAT_SPELL                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCancelAutoRepeatSpellOpcode                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_CHANNELLING                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCancelChanneling                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_GROWTH_AURA                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCancelGrowthAuraOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_MOUNT_AURA                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCancelMountAuraOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_QUEUED_SPELL                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Cache
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_DB_QUERY_BULK                                            , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleDBQueryBulk                                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_HOTFIX_REQUEST                                           , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleHotfixRequest                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_CREATURE                                           , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleCreatureQueryOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_NPC_TEXT                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleNpcTextQueryOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_PLAYER_NAME                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleNameQueryOpcode                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_QUEST_INFO                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestQueryOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_POI_QUERY                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestPOIQuery                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_SCENARIO_POI                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQueryScenarioPOI                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_REALM_NAME                                         , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleRealmQueryNameOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GAME_OBJECT                                        , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleGameObjectQueryOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_INFO                                         , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleQueryGuildInfoOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_QUEST_REWARDS                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQueryQuestRewards                                         , PROCESS_DISTANT_IF_NEED);

#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_PAGE_TEXT                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePageTextQueryOpcode                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_TEXT_QUERY                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleItemTextQuery                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_PETITION                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetitionQueryOpcode                                       , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// Interaction
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LOGOUT_REQUEST                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,  &WorldSession::HandleLogoutRequestOpcode                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LOGOUT_CANCEL                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleLogoutCancelOpcode                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTION_BUTTON                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetActionButtonOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTION_BAR_TOGGLES                                   , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleSetActionBarToggles                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_SELECTION                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleSetSelectionOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_SHEATHED                                             , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleSetSheathedOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TALK_TO_GOSSIP                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGossipTalkOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_GOSSIP_SELECT_OPTION                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGossipSelectOptionOpcode                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_PVP                                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetPvP                                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TUTORIAL_FLAG                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTutorial                                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_INACTIVE                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetFactionInactiveOpcode                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_WATCHED_FACTION                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetWatchedFactionOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AREA_TRIGGER                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAreaTriggerOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJECT_REPORT_USE                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGameobjectReportUse                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJECT_USE                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGameObjectUseOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SPELL_CLICK                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSpellClick                                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REPOP_REQUEST                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRepopRequestOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_RECLAIM_CORPSE                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleReclaimCorpseOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_CORPSE_LOCATION_FROM_CLIENT                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCorpseLocationFromClientQueryOpcode                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_CORPSE_TRANSPORT                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCorpseTransportQueryOpcode                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_RETURN_TO_GRAVEYARD                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleReturnToGraveyard                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CLOSE_INTERACTION                                        , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleCloseInteraction                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_REFUND_INFO                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleItemRefundInfoRequest                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_FAR_SIGHT                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleFarSightOpcode                                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOUNT_SPECIAL_ANIM                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMountSpecialAnimOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_OPENING_CINEMATIC                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_NEXT_CINEMATIC_CAMERA                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleNextCinematicCamera                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_CINEMATIC                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCompleteCinematic                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CEMETERY_LIST                                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleCemeteryListOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TOTEM_DESTROYED                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTotemDestroyed                                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CONFIRM_RESPEC_WIPE                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTalentWipeConfirmOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_TRADE                                             , STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT, PROCESS_THREADSAFE  , &WorldSession::HandleCancelTradeOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_CURRENCY                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_GOLD                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetTradeGoldOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_ITEM                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetTradeItemOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CLEAR_TRADE_ITEM                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleClearTradeItemOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_TRADE                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAcceptTradeOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BUSY_TRADE                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleBusyTradeOpcode                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BEGIN_TRADE                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleBeginTradeOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_IGNORE_TRADE                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleIgnoreTradeOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_INITIATE_TRADE                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleInitiateTradeOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_UNACCEPT_TRADE                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleUnacceptTradeOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_NEUTRAL_PLAYER_SELECT_FACTION                            , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleSetFactionOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_INSPECT                                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleInspectOpcode                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_HONOR_STATS                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleInspectHonorStatsOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_INSPECT_PVP                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleInspectPVPStatsOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TIME_SYNC_RESPONSE                                       , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleTimeSyncResp                                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_DISCARDED_TIME_SYNC_ACKS                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_SKILL                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleUnlearnSkillOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_EMOTE                                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleEmoteOpcode                                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ALTER_APPEARANCE                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAlterAppearance                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SELF_RES                                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSelfResOpcode                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SEND_TEXT_EMOTE                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTextEmoteOpcode                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_READ_ITEM                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleReadItem                                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_MOVIE                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleCompleteMovieOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SCENE_TRIGGER_EVENT                                      , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleSceneTriggerEventOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SCENE_PLAYBACK_CANCELED                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleSceneCancelOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SCENE_PLAYBACK_COMPLETE                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleSceneCancelOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SUMMON_RESPONSE                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSummonResponseOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_AT_WAR                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSetFactionAtWar                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_NOT_AT_WAR                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleUnSetFactionAtWar                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_USE_CRITTER_ITEM                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleUseCritterItemOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_CURRENCY_FLAGS                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleChangeCurrencyFlags                                       , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Vehicles
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_VEHICLE_REC_ID_ACK                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_RIDE_VEHICLE_INTERACT                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleEnterPlayerVehicle                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_EXIT                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRequestVehicleExit                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_NEXT_SEAT                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleChangeSeatsOnControlledVehicle                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_PREV_SEAT                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleChangeSeatsOnControlledVehicle                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_SWITCH_SEAT                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleChangeSeatsOnControlledVehicle                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleChangeSeatsOnControlledVehicle                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_EJECT_PASSENGER                                          , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleEjectPassenger                                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_DISMISS_VEHICLE                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleDismissControlledVehicle                                  , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Vendors
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LIST_INVENTORY                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleListInventoryOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REPAIR_ITEM                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRepairItemOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BUYBACK_ITEM                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleBuybackItem                                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BUY_ITEM                                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleBuyItemOpcode                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SELL_ITEM                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleSellItemOpcode                                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_REFUND                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleItemRefund                                                , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Taxi
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_SET_TAXI_BENCHMARK_MODE                                  , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleSetTaxiBenchmarkOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ENABLE_TAXI_NODE                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTaxiQueryAvailableNodes                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TAXI_QUERY_AVAILABLE_NODES                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTaxiQueryAvailableNodes                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ACTIVATE_TAXI                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleActivateTaxiOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TAXI_NODE_STATUS_QUERY                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTaxiNodeStatusQueryOpcode                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TAXI_REQUEST_EARLY_LANDING                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTaxiRequestEarlyLandingOpcode                             , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Quest
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_HELLO                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestgiverHelloOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_STATUS_QUERY                                 , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleQuestgiverStatusQueryOpcode                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_STATUS_MULTIPLE_QUERY                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestgiverStatusMultipleQuery                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_QUERY_QUEST                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestgiverQueryQuestOpcode                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_COMPLETE_QUEST                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestgiverCompleteQuest                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_CHOOSE_REWARD                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestgiverChooseRewardOpcode                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_ACCEPT_QUEST                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestgiverAcceptQuestOpcode                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_REQUEST_REWARD                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestgiverRequestRewardOpcode                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_CONFIRM_ACCEPT                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestConfirmAccept                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_LOG_REMOVE_QUEST                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestLogRemoveQuest                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_PUSH_RESULT                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQuestPushResult                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PUSH_QUEST_TO_PARTY                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePushQuestToParty                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_QUEST_COMPLETION_NPCS                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQueryQuestCompletionNpcs                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_WORLD_QUEST_UPDATE                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRequestWorldQUestUpdate                                   , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Account data
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_GET_UNDELETE_CHARACTER_COOLDOWN_STATUS                   , STATUS_AUTHED,                       PROCESS_INPLACE, &WorldSession::HandleUndeleteCharacter                                         , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Chat
    //////////////////////////////////////////////////////////////////////////
    /// Addon chat
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_INSTANCE_CHAT                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonMessagechatOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_WHISPER                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonMessagechatOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_GUILD                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonMessagechatOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_OFFICER                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonMessagechatOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_RAID                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonMessagechatOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_PARTY                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonMessagechatOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_CHANNEL                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonMessagechatOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_REGISTER_ADDON_PREFIXES                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddonRegisteredPrefixesOpcode                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_UNREGISTER_ALL_ADDON_PREFIXES                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleUnregisterAddonPrefixesOpcode                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_ADDON_LIST                                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    /// Chat
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_RAID_WARNING                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_PARTY                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_YELL                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_SAY                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_OFFICER                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_EMOTE                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_GUILD                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_RAID                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_AFK                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_CHANNEL                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_DND                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_WHISPER                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMessagechatOpcode                                         , PROCESS_DISTANT_IF_NEED);

    /// Channel
    DEFINE_OPCODE_HANDLER(CMSG_JOIN_CHANNEL                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleJoinChannel                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_ANNOUNCEMENTS                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelAnnouncements                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_BAN                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelBan                                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_DISPLAY_LIST                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelDisplayListQuery                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_INVITE                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelInvite                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_KICK                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelKick                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_LIST                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelList                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_MODERATOR                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelModerator                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_MUTE                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelMute                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_OWNER                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelOwner                                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_PASSWORD                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelPassword                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_SET_OWNER                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelSetOwner                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_SILENCE_ALL                                 , STATUS_UNHANDLED,                    PROCESS_THREADSAFE, &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_UNBAN                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelUnban                                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_UNMODERATOR                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelUnmoderator                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_UNMUTE                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelUnmute                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_CHANNEL_UNSILENCE_ALL                               , STATUS_UNHANDLED,                    PROCESS_THREADSAFE, &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_DECLINE_CHANNEL_INVITE                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleChannelDeclineInvite                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_LEAVE_CHANNEL                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLeaveChannel                                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SILENCE_PARTY_TALKER                                     , STATUS_UNHANDLED,                    PROCESS_THREADSAFE, &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Movement
    //////////////////////////////////////////////////////////////////////////
    /// Start
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_FORWARD                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_TURN_LEFT                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_TURN_RIGHT                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_BACKWARD                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_STRAFE_LEFT                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_STRAFE_RIGHT                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_ASCEND                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_DESCEND                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_PITCH_UP                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_PITCH_DOWN                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_SWIM                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);

    /// Misc
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_DOUBLE_JUMP                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_JUMP                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FALL_LAND                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_WORLD_PORT_RESPONSE                                      , STATUS_TRANSFER,                     PROCESS_THREADUNSAFE, &WorldSession::HandleMoveWorldportAckOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TIME_SKIPPED                                        , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleMoveTimeSkippedOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SPLINE_DONE                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMoveSplineDoneOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_FLY                                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RELATIVE_POSITION                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FALL_RESET                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CONTRIBUTION_GET_STATE                                   , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleContributionGetState                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CONTRIBUTION_CONTRIBUTE                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleContributionContribute                                    , PROCESS_DISTANT_IF_NEED);

    /// Update
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_HEARTBEAT                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_FACING                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_PITCH                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RUN_MODE                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_WALK_MODE                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TELEPORT_ACK                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMoveTeleportAck                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_CHANGE_TRANSPORT                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);

    /// Stop
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_TURN                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_STRAFE                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_SWIM                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_ASCEND                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_PITCH                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);;
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_REMOVE_MOVEMENT_FORCES                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);

    /// Ack
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_RUN_BACK_SPEED_CHANGE_ACK                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_WALK_SPEED_CHANGE_ACK                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_SWIM_SPEED_CHANGE_ACK                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_SWIM_BACK_SPEED_CHANGE_ACK                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_FLIGHT_SPEED_CHANGE_ACK                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_TURN_RATE_CHANGE_ACK                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_PITCH_RATE_CHANGE_ACK                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleForceSpeedChangeAck                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_KNOCK_BACK_ACK                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMoveKnockBackAck                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_CAN_FLY_ACK                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FEATHER_FALL_ACK                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_WATER_WALK_ACK                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_HOVER_ACK                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_APPLY_MOVEMENT_FORCE_ACK                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_REMOVE_MOVEMENT_FORCE_ACK                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleMovementOpcodes                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_ENABLE_SWIM_TO_FLY_TRANS_ACK                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_IGNORE_MOVEMENT_FORCES_ACK                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_COLLISION_HEIGHT_ACK                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_CAN_TURN_WHILE_FALLING_ACK                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_GRAVITY_ENABLE_ACK                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_GRAVITY_DISABLE_ACK                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_ROOT_ACK                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_UNROOT_ACK                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_ENABLE_DOUBLE_JUMP_ACK                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TOGGLE_COLLISION_CHEAT                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Group
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_INVITE                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGroupInviteOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_INVITE_RESPONSE                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGroupInviteResponseOpcode                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LEAVE_GROUP                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLeaveGroupOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_UNINVITE                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePartyUninviteOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_PARTY_LEADER                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleSetPartyLeaderOpcode                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_ROLE                                                 , STATUS_LOGGEDIN,                     PROCESS_INPLACE   , &WorldSession::HandleSetRoleOpcode                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_MINIMAP_PING                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMinimapPingOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_RANDOM_ROLL                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRandomRollOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_RAID_TARGET                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleUpdateRaidTargetOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CONVERT_RAID                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGroupRaidConvertOpcode                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_DO_READY_CHECK                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRaidLeaderReadyCheck                                      , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_READY_CHECK_RESPONSE                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRaidConfirmReadyCheck                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_INITIATE_ROLE_POLL                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRolePollBegin                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RAID_INFO                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestRaidInfoOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_EVERYONE_IS_ASSISTANT                                , STATUS_LOGGEDIN,                     PROCESS_INPLACE   , &WorldSession::HandleGroupEveryoneIsAssistantOpcode                            , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CLEAR_RAID_MARKER                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleClearRaidMarkerOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_ASSISTANT_LEADER                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGroupAssistantLeaderOpcode                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_PARTY_ASSIGNMENT                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePartyAssignmentOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_CHANGE_SUB_GROUP                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGroupChangeSubGroupOpcode                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SWAP_SUB_GROUPS                                          , STATUS_UNHANDLED,                    PROCESS_THREADSAFE, &WorldSession::HandleGroupSwapSubGroupOpcode                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_INSTANCE_LOCK_RESPONSE                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleInstanceLockResponseOpcode                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PARTY_MEMBER_STATS                               , STATUS_UNHANDLED,                    PROCESS_INPLACE   , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PARTY_JOIN_UPDATES                               , STATUS_UNHANDLED,                    PROCESS_INPLACE   , &WorldSession::Handle_NULL                                                     , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Friend
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_SEND_CONTACT_LIST                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleContactListOpcode                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ADD_FRIEND                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddFriendOpcode                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ADD_IGNORE                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddIgnoreOpcode                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_CONTACT_NOTES                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleSetContactNotesOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_DEL_FRIEND                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleDelFriendOpcode                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_DEL_IGNORE                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleDelIgnoreOpcode                                           , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Pet
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_PET_NAME                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetNameQuery                                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PET_RENAME                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetRename                                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PET_ABANDON                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetAbandon                                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PET_ACTION                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetAction                                                 , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PET_CANCEL_AURA                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetCancelAuraOpcode                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PET_CAST_SPELL                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetCastSpellOpcode                                        , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PET_SET_ACTION                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetSetAction                                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PET_STOP_ATTACK                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandlePetStopAttack                                             , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_LIST_STABLE_PETS                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleListStabledPetsOpcode                                     , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_SET_PET_SLOT                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleStableSetPetSlot                                          , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Battle grounds
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBattlemasterJoinOpcode                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN_ARENA                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBattlemasterJoinArena                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN_ARENA_SKIRMISH                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBattlemasterJoinArenaSkirmish                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_JOIN_RATED_BATTLEGROUND                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBattlemasterJoinRated                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_PORT                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBattleFieldPortOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_BATTLEFIELD_STATUS                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBattlefieldStatusOpcode                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PVP_LOG_DATA                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePVPLogDataOpcode                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PVP_REWARDS                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestPvpReward                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PVP_BRAWL_INFO                                   , STATUS_UNHANDLED,                    PROCESS_THREADSAFE, &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_RAID_OR_BATTLEGROUND_ENGINE_SURVEY                       , STATUS_UNHANDLED,                    PROCESS_THREADSAFE, &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PVP_OPTIONS_ENABLED                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestPvpOptions                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_COUNTDOWN_TIMER                                    , STATUS_UNHANDLED,                    PROCESS_THREADSAFE, &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CONQUEST_FORMULA_CONSTANTS                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestConquestFormulaConstants                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_LEAVE                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLeaveBattlefieldOpcode                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SPIRIT_HEALER_ACTIVATE                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleSpiritHealerActivateOpcode                                , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUERY                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAreaSpiritHealerQueryOpcode                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUEUE                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAreaSpiritHealerQueueOpcode                               , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_LIST                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBattlefieldListOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RATED_BATTLEFIELD_INFO                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestRatedBgStats                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_START_WAR_GAME                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleStartWarGame                                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_WARGAME_INVITE                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAcceptWarGameInvite                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REPORT_PVP_PLAYER_AFK                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleReportPvPAFK                                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_PVP_PRESTIGE_RANK_UP                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePrestige                                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CROWD_CONTROL_SPELL                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestCrowdControlSpell                                  , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Guild
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_INVITE_BY_NAME                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildInviteByNameOpcode                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_GUILD_INVITE                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAcceptGuildInviteOpcode                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DECLINE_INVITATION                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildDeclineInvitationsOpcode                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_OFFICER_REMOVE_MEMBER                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildOfficierRemoveMemberOpcode                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_GUILD_MASTER                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildSetGuildMasterOpcode                                 , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_LEAVE                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildLeaveOpcode                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DELETE                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildDeleteOpcode                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_UPDATE_MOTD_TEXT                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildUpdateMOTDTextOpcode                                 , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_UPDATE_INFO_TEXT                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildUpdateInfoTextOpcode                                 , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_GET_RANKS                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildGetRanksOpcode                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ADD_RANK                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildAddRankOpcode                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DELETE_RANK                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildDeleteRankOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_RANK_PERMISSIONS                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildSetRankPermissionsOpcode                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SHIFT_RANK                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleShiftRanks                                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ASSIGN_MEMBER_RANK                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildAssignRankOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_GET_ROSTER                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildRosterOpcode                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_ACTIVATE                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankActivate                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_BUY_TAB                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankBuyTab                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_DEPOSIT_MONEY                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankDepositMoney                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_LOG_QUERY                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankLogQuery                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY_QUERY                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankRemainingWithdrawMoneyQueryOpcode                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_QUERY_TAB                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankQueryTab                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_TEXT_QUERY                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleQueryGuildBankTextQuery                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_SWAP_ITEMS_LEGACY                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankSwapItemsLegacy                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_UPDATE_TAB                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankUpdateTab                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_WITHDRAW_MONEY                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGuildBankWithdrawMoney                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_SET_TAB_TEXT                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleSetGuildBankTabText                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK                        , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMoveItemsPlayerBank                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER                        , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMoveItemsBankPlayer                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK                          , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMoveItemsBankBank                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK_COUNT                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMoveItemsPlayerBankCount                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_COUNT                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMoveItemsBankPlayerCount                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK_COUNT                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMoveItemsBankBankCount                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_AUTO                   , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMoveItemsBankPlayerAuto                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MERGE_ITEMS_PLAYER_BANK                       , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMergeItemsPlayerBank                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MERGE_ITEMS_BANK_PLAYER                       , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMergeItemsBankPlayer                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MERGE_ITEMS_BANK_BANK                         , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankMergeItemsBankBank                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_SWAP_ITEMS_BANK_PLAYER                        , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankSwapItemsBankPlayer                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_SWAP_ITEMS_BANK_BANK                          , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildBankSwapItemsBankBank                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_GET_ACHIEVEMENT_MEMBERS                            , STATUS_UNHANDLED,                    PROCESS_INPLACE,      &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_FOCUSED_ACHIEVEMENT                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleGuildAchievementProgressQuery                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_ACHIEVEMENT_TRACKING                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleGuildSetAchievementTracking                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_MEMBER_NOTE                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleGuildSetMemberNoteOpcode                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_CHALLENGE_UPDATE_REQUEST                           , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildChallengeUpdateRequest                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_GUILD_PARTY_STATE                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleRequestGuildPartyState                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_GUILD_REWARDS_LIST                               , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleRequestGuildRewardsListOpcode                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_REPLACE_GUILD_MASTER                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleGuildMasterReplaceOpcode                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_CHANGE_NAME_REQUEST                                , STATUS_UNHANDLED,                    PROCESS_INPLACE,      &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_PERMISSIONS_QUERY                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleGuildPermissionsQueryOpcode                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_EVENT_LOG_QUERY                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandleGuildEventLogQueryOpcode                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_NEWS_UPDATE_STICKY                                 , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildNewsUpdateStickyOpcode                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_QUERY_NEWS                                         , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildQueryNewsOpcode                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_MEMBERS_FOR_RECIPE                           , STATUS_UNHANDLED,                    PROCESS_INPLACE,      &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_MEMBER_RECIPES                               , STATUS_UNHANDLED,                    PROCESS_INPLACE,      &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_RECIPES                                      , STATUS_LOGGEDIN,                     PROCESS_INPLACE,      &WorldSession::HandleGuildRequestGuildRecipes                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SAVE_GUILD_EMBLEM                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,   &WorldSession::HandlePlayerSaveGuildEmblemOpcode                               , PROCESS_LOCAL);

    /// Guild finder
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_ADD_RECRUIT                                     , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderAddRecruit                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_BROWSE                                          , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderBrowse                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_DECLINE_RECRUIT                                 , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderDeclineRecruit                                 , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_GET_APPLICATIONS                                , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderGetApplications                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_GET_RECRUITS                                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderGetRecruits                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_GET_GUILD_POST                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderPostRequest                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_REMOVE_RECRUIT                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderRemoveRecruit                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_SET_GUILD_POST                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE, &WorldSession::HandleGuildFinderSetGuildPost                                   , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Petition
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_OFFER_PETITION                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleOfferPetitionOpcode                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_BUY                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePetitionBuyOpcode                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_DECLINE                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePetitionDeclineOpcode                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_RENAME_GUILD                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePetitionRenameOpcode                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SHOW_LIST                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePetitionShowListOpcode                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SHOW_SIGNATURES                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePetitionShowSignOpcode                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SIGN_PETITION                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandlePetitionSignOpcode                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TURN_IN_PETITION                                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleTurnInPetitionOpcode                                      , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// Battle pet opcodes
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_BATTLE_PET_NAME                                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetQueryName                                        , PROCESS_LOCAL);

    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_UPDATE_NOTIFY                                 , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetUpdateNotify                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_REQUEST_JOURNAL_LOCK                          , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetRequestJournalLock                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_REQUEST_JOURNAL                               , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetRequestJournal                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_DELETE_PET                                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetDeletePet                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_DELETE_PET_CHEAT                              , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetDeletePetCheat                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_MODIFY_NAME                                   , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetModifyName                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_SUMMON                                        , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetSummon                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_SET_BATTLE_SLOT                               , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetSetBattleSlot                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_SET_FLAGS                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleBattlePetSetFlags                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CAGE_BATTLE_PET                                          , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleBattlePetCage                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_JOIN_PET_BATTLE_QUEUE                                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleJoinQueue                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LEAVE_PET_BATTLE_QUEUE                                   , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleLeaveQueue                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_REQUEST_WILD                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleRequestWild                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_WILD_LOCATION_FAIL                            , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::HandlePetBattleWildLocationFail                                 , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_REQUEST_PVP                                   , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleRequestPvP                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_REQUEST_UPDATE                                , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleRequestUpdate                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_QUIT_NOTIFY                                   , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleQuitNotify                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_FINAL_NOTIFY                                  , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleFinalNotify                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH_RESULT                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleQueueProposeMatchResult                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_INPUT                                         , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleInput                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_REPLACE_FRONT_PET                             , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandlePetBattleReplaceFrontPet                                  , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Battle pay
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_GET_PURCHASE_LIST                             , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleBattlepayGetPurchaseList                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_GET_PRODUCT_LIST_QUERY                        , STATUS_AUTHED,                       PROCESS_THREADSAFE  , &WorldSession::HandleBattlepayGetProductListQuery                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_START_PURCHASE                                , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleBattlePayStartPurchase                                    , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_CONFIRM_PURCHASE_RESPONSE                     , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleBattlePayConfirmPurchase                                  , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_ACK_FAILED_RESPONSE                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_DISTRIBUTION_ASSIGN_TO_TARGET                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_QUERY_CLASS_TRIAL_BOOST_RESULT                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_START_VAS_PURCHASE                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_TRIAL_BOOST_CHARACTER                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// WoW Token
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_CAN_REDEEM_WOW_TOKEN_FOR_BALANCE                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_VAS_PURCHASE_STATES                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_WOW_TOKEN_AUCTIONABLE_LIST                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_WOW_TOKEN_COUNT                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SELL_WOW_TOKEN_CONFIRM                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SELL_WOW_TOKEN_START                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REDEEM_WOW_TOKEN_CONFIRM                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REDEEM_WOW_TOKEN_START                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BUY_WOW_TOKEN_CONFIRM                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BUY_WOW_TOKEN_START                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_WOW_TOKEN_MARKET_PRICE                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// LFG
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_DFGET_SYSTEM_INFO                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgLockInfoRequestOpcode                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_GET_STATUS                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgGetStatus                                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_JOIN                                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgJoinOpcode                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LEAVE                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgLeaveOpcode                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_PROPOSAL_RESULT                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgProposalResultOpcode                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_SET_BOOT_VOTE                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgSetBootVoteOpcode                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_DFSET_ROLES                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleDfSetRolesOpcode                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_TELEPORT                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgTeleportOpcode                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_RESET_INSTANCES                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleResetInstancesOpcode                                      , PROCESS_LOCAL);

#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_SET_SAVED_INSTANCE_EXTEND                                , STATUS_LOGGEDIN,                     PROCESS_INPLACE,    &WorldSession::HandleSetSavedInstanceExtend                                    , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// LFG LIST
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_GET_STATUS                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE,    &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_LFG_LIST_BLACKLIST                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestLfgListBlacklist                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_JOIN                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListJoin                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_UPDATE_REQUEST                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListUpdateRequest                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_LEAVE                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListLeave                                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_SEARCH                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListSearch                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_APPLY_TO_GROUP                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListApplyForGroup                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_INVITE_APPLICANT                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListInviteApplicant                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_DECLINE_APPLICANT                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListRemoveApplicant                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_CANCEL_APPLICATION                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListCancelApplication                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_INVITE_RESPONSE                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleLfgListInvitationAnswer                                   , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Trainers
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_TRAINER_LIST                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleTrainerListOpcode                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TRAINER_BUY_SPELL                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleTrainerBuySpellOpcode                                     , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Void storage
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_VOID_STORAGE                                       , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleVoidStorageQuery                                          , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_VOID_STORAGE_TRANSFER                                    , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleVoidStorageTransfer                                       , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_UNLOCK_VOID_STORAGE                                      , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleVoidStorageUnlock                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_VOID_SWAP_ITEM                                           , STATUS_LOGGEDIN,                     PROCESS_INPLACE     , &WorldSession::HandleVoidSwapItem                                              , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Transmogrification
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_TRANSMOGRIFY_ITEMS                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleTransmogrifyItems                                         , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_COLLECTION_ITEM_SET_FAVORITE                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Mail
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_GET_LIST                                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleGetMailList                                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_CREATE_TEXT_ITEM                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMailCreateTextItem                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_DELETE                                              , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMailDelete                                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_MARK_AS_READ                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMailMarkAsRead                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_RETURN_TO_SENDER                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMailReturnToSender                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_TAKE_ITEM                                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMailTakeItem                                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_TAKE_MONEY                                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleMailTakeMoney                                             , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SEND_MAIL                                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleSendMail                                                  , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_NEXT_MAIL_TIME                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleQueryNextMailTime                                         , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// Time
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_UI_TIME_REQUEST                                          , STATUS_AUTHED,                       PROCESS_INPLACE     , &WorldSession::HandleWorldStateUITimerUpdate                                   , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_TIME                                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleQueryTimeOpcode                                           , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_TIME_ADJUSTMENT_RESPONSE                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TIME_SYNC_RESPONSE_DROPPED                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TIME_SYNC_RESPONSE_FAILED                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// GameMasters
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_GM_TICKET_GET_CASE_STATUS                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GM_TICKET_GET_SYSTEM_STATUS                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
#endif
    DEFINE_OPCODE_HANDLER(CMSG_SUPPORT_TICKET_SUBMIT_BUG                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleReportBugOpcode                                           , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SUPPORT_TICKET_SUBMIT_SUGGESTION                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleReportSuggestionOpcode                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SUPPORT_TICKET_SUBMIT_COMPLAINT                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// ToyBox
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_ADD_TOY                                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleAddToyOpcode                                              , PROCESS_DISTANT_IF_NEED);
    DEFINE_OPCODE_HANDLER(CMSG_USE_TOY                                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleUseToyOpcode                                              , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    /// Calendar
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_ADD_EVENT                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarAddEvent                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_COMPLAIN                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarComplain                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_COPY_EVENT                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarCopyEvent                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_INVITE                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarEventInvite                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_MODERATOR_STATUS                          , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarEventModeratorStatus                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_REMOVE_INVITE                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarEventRemoveInvite                                 , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_RSVP                                      , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarEventRsvp                                         , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_SIGN_UP                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarEventSignup                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_STATUS                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarEventStatus                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET                                             , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarGetCalendar                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_EVENT                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarGetEvent                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_NUM_PENDING                                 , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarGetNumPending                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GUILD_FILTER                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarGuildFilter                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_REMOVE_EVENT                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarRemoveEvent                                       , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_UPDATE_EVENT                                    , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleCalendarUpdateEvent                                       , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// Challenges
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_LEADERS                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRequestLeaders                                            , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GET_CHALLENGE_MODE_REWARDS                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleGetChallengeModeRewards                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_MAP_STATS                         , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleChallengeModeRequestMapStats                              , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_RESET_CHALLENGE_MODE                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleResetChallengeMode                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_START_CHALLENGE_MODE                                     , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleStartChallengeMode                                        , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Black market
    //////////////////////////////////////////////////////////////////////////
#ifndef CROSS
    DEFINE_OPCODE_HANDLER(CMSG_BLACK_MARKET_OPEN                                        , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBlackMarketHello                                          , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BLACK_MARKET_REQUEST_ITEMS                               , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBlackMarketRequestItems                                   , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BLACK_MARKET_PLACE_BID                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleBlackMarketBid                                            , PROCESS_LOCAL);
#endif

    //////////////////////////////////////////////////////////////////////////
    /// Twitter
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_TWITTER_STATUS                                   , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleRequestTwitterStatus                                      , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TWITTER_CONNECT                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TWITTER_DISCONNECT                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TWITTER_POST                                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Artifact
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_ARTIFACT_ADD_POWER                                       , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleArtifactLearnTrait                                        , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ARTIFACT_CONFIRM_RESPEC                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleArtifactConfirmRespec                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ARTIFACT_SET_APPEARANCE                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleArtifactSetAppearance                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ARTIFACT_ADD_RELIC_TALENT                                , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleArtifactAddRelicTalent                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ARTIFACT_ATTUNE_PREVIEW_RELIC                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleArtifactAttunePreviewRelic                                , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ARTIFACT_ATTUNE_SOCKETED_RELIC                           , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleArtifactAttuneSocketedRelic                               , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Battle                                                                                                                                          .net
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_ADD_BATTLENET_FRIEND                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLENET_CHALLENGE_RESPONSE                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLENET_REQUEST                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLENET_REQUEST_REALM_LIST_TICKET                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Adventure Map
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_SCOUTING_MAP                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAdventureMapPOIEnabledQuery                               , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ADVENTURE_JOURNAL_START_QUEST                            , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE  , &WorldSession::HandleAdventureMapStartQuest                                    , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ADVENTURE_JOURNAL_OPEN_QUEST                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    //////////////////////////////////////////////////////////////////////////
    /// Area POI
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_AREA_POI_UPDATE                                  , STATUS_LOGGEDIN,                     PROCESS_THREADSAFE, &WorldSession::HandleRequestAreaPOIUpdate                                      , PROCESS_DISTANT_IF_NEED);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    /// NYI
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    DEFINE_OPCODE_HANDLER(CMSG_REPORT_CLIENT_VARIABLES                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REPORT_ENABLED_ADDONS                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REPORT_KEYBINDING_EXECUTION_COUNTS                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_ADVANCED_COMBAT_LOGGING                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REVERT_MONUMENT_APPEARANCE                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CHANGE_MONUMENT_APPEARANCE                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACHIEVEMENTS_HIDDEN                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_DIFFICULTY_ID                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MOUNT_CLEAR_FANFARE                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_MOD_SPEED_NO_CONTROL_AURAS                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LOGOUT_INSTANT                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_SPECIALIZATION                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TRADE_SKILL_SET_FAVORITE                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_CLIENT_SETTINGS                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_TABARD_VENDOR_ACTIVATE                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_SPELL_AUTOCAST                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_START_SPECTATOR_WAR_GAME                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_CLEAR_FANFARE                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PET_BATTLE_SCRIPT_ERROR_NOTIFY                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_LFG_BONUS_FACTION_ID                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SEND_SOR_REQUEST_VIA_ADDRESS                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LOW_LEVEL_RAID1                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LOW_LEVEL_RAID2                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_USED_FOLLOW                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CONNECT_TO_FAILED                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_LEVEL_GRANT                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_TEMP_ENCHANTMENT                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_COMPLAINT                                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_DISMISS_CRITTER                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_GRANT_LEVEL                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_HEARTH_AND_RESURRECT                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CATEGORY_COOLDOWNS                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_MISSILE_TRAJECTORY                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPGRADE_ITEM                                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_WARDEN_DATA                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_WHOIS                                                    , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_WORLD_TELEPORT                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_WRAP_ITEM                                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_HELLO                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUICK_JOIN_REQUEST_INVITE                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUICK_JOIN_RESPOND_TO_INVITE                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUICK_JOIN_AUTO_ACCEPT_REQUESTS                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_QUICK_JOIN_SIGNAL_TOAST_DISPLAYED                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_ENABLE                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_ENTER_INSTANCE                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_EXIT_INSTANCE                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_GET_MAP_INFO                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_GET_PLAYER_INFO                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_START_WARGAME                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_SET_PREFERRED_CEMETERY                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_BUG_REPORT                                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_SPELL_VISUAL                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_MISSILE_TRAJECTORY_COLLISION                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CONSUMPTION_CONVERSION_INFO                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_REPLACE_TROPHY                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_RECRUIT_A_FRIEND                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_PROTOCOL_MISMATCH                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LOAD_SELECTED_TROPHY                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LIVE_REGION_ACCOUNT_RESTORE                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LIVE_REGION_CHARACTER_COPY                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_LIVE_REGION_GET_ACCOUNT_CHARACTER_LIST                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(CMSG_KEYBOUND_OVERRIDE                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);

    /// SMSG
    DEFINE_OPCODE_HANDLER(SMSG_ADD_BATTLENET_FRIEND_RESPONSE                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BONUS_ROLL_EMPTY                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CONSOLE_WRITE                                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_PROMOTION                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FORCE_OBJECT_RELINK                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REFRESH_COMPONENT                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_EVENT                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_FINISHED                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_FIRST_ROUND                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_FINAL_ROUND                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_ROUND_RESULT                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_REPLACEMENTS_MADE                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_EXPANSION_LEVEL                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_DENIED                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ARENA_ERROR                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_INFO_THROTTLED                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_BOSS_EMOTES                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COMBAT_EVENT_FAILED                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_MAP_INFO                                     , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_PLAYER_INFO                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_STATE_CHANGED                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_CUSTOM_LOAD_SCREEN                                       , STATUS_NEVER,                        PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DAMAGE_CALC_LOG                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DIFFERENT_INSTANCE_FROM_PARTY                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DISENCHANT_CREDIT                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_DONT_AUTO_PUSH_SPELLS_TO_ACTION_BAR                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FAILED_PLAYER_CONDITION                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_RESET_STATE                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAME_SPEED_SET                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAME_TIME_SET                                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GAME_TIME_UPDATE                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GM_PLAYER_INFO                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GOD_MODE                                                 , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_ACTION_THROTTLED                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_HEALTH_UPDATE                                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INVALIDATE_PLAYER                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_INVALID_PROMOTION_CODE                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REMOVE_ITEM_PASSIVE                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_ITEM_PASSIVES                                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MAP_OBJ_EVENTS                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MESSAGE_BOX                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MISSILE_CANCEL                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_NOTIFY_MONEY                                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_COMPOUND_STATE                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_COLLISION_HEIGHT                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_ABORT_NEW_WORLD                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_CONTAINER                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_LFG_DUNGEON_FINDER                                  , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_ALLIED_RACE_DETAILS_GIVER                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_SKINNED                                           , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_TIME_WARNING                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_PROPOSE_LEVEL_GRANT                                      , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SUMMON_RAID_MEMBER_VALIDATE_FAILED                       , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_EXPIRED                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_FAILURE                                   , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_REPORT_PVP_PLAYER_AFK_RESULT                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SERVER_TIME                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SET_DF_FAST_LAUNCH_RESULT                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SOR_START_EXPERIENCE_INCOMPLETE                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_STREAMING_MOVIES                                         , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SUMMON_CANCEL                                            , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_TIME_ADJUSTMENT                                          , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WAIT_QUEUE_FINISH                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_WAIT_QUEUE_UPDATE                                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_FORCE_ANIM                                               , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_KICK_REASON                                              , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_RESET_RANGED_COMBAT_TIMER                                , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_SHOW_MAILBOX                                             , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
    DEFINE_OPCODE_HANDLER(SMSG_GARRISON_RENAME_FOLLOWER_RESPONSE                        , STATUS_UNHANDLED,                    PROCESS_INPLACE     , &WorldSession::Handle_NULL                                                     , PROCESS_LOCAL);
#undef DEFINE_OPCODE_HANDLER
};

ConnectionType GetOpcodeConnectionType(uint32 p_OpcodeID)
{
    switch (p_OpcodeID)
    {
        case SMSG_ACCOUNT_MOUNT_UPDATE:
        case SMSG_ACCOUNT_TOYS_UPDATE:
        case SMSG_ACHIEVEMENT_DELETED:
        case SMSG_ACHIEVEMENT_EARNED:
        case SMSG_ADVENTURE_LIST_UPDATE:
        case SMSG_AI_REACTION:
        case SMSG_ALL_ACCOUNT_CRITERIA:
        case SMSG_ALL_ACHIEVEMENT_DATA:
        case SMSG_ARTIFACT_FORGE_OPENED:
        case SMSG_ARTIFACT_RESPEC_CONFIRM:
        case SMSG_ARTIFACT_XP_GAIN:
        case SMSG_ATTACK_START:
        case SMSG_ATTACK_STOP:
        case SMSG_ATTACKER_STATE_UPDATE:
        case SMSG_AURA_UPDATE:
        case SMSG_AREA_TRIGGER_RE_PATH:
        case SMSG_AREA_TRIGGER_RE_SHAPE:
        case SMSG_BATTLEGROUND_PLAYER_JOINED:
        case SMSG_BATTLEGROUND_PLAYER_LEFT:
        case SMSG_BATTLEGROUND_PLAYER_POSITIONS:
        case SMSG_BIND_POINT_UPDATE:
        case SMSG_CAST_FAILED:
        case SMSG_CATEGORY_COOLDOWN:
        case SMSG_CLEAR_ALL_SPELL_CHARGES:
        case SMSG_CLEAR_COOLDOWN:
        case SMSG_CLEAR_COOLDOWNS:
        case SMSG_CLEAR_SPELL_CHARGES:
        case SMSG_CONQUEST_FORMULA_CONSTANTS:
        case SMSG_COOLDOWN_EVENT:
        case SMSG_CORPSE_RECLAIM_DELAY:
        case SMSG_CRITERIA_DELETED:
        case SMSG_CRITERIA_UPDATE:
        case SMSG_DESTRUCTIBLE_BUILDING_DAMAGE:
        case SMSG_DISPEL_FAILED:
        case SMSG_DUEL_COMPLETE:
        case SMSG_DUEL_COUNTDOWN:
        case SMSG_DUEL_INBOUNDS:
        case SMSG_DUEL_OUT_OF_BOUNDS:
        case SMSG_DUEL_REQUESTED:
        case SMSG_DUEL_WINNER:
        case SMSG_EMOTE:
        case SMSG_EQUIPMENT_SET_LIST:
        case SMSG_FACTION_BONUS_INFO:
        case SMSG_GAME_OBJECT_ACTIVATE_ANIM_KIT:
        case SMSG_GAMEOBJECT_CUSTOM_ANIM:
        case SMSG_GARRISON_ADD_FOLLOWER_RESULT:
        case SMSG_GARRISON_ADD_MISSION_RESULT:
        case SMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING_RESULT:
        case SMSG_GARRISON_BUILDING_ACTIVATED:
        case SMSG_GARRISON_BUILDING_LANDMARKS:
        case SMSG_GARRISON_BUILDING_REMOVED:
        case SMSG_GARRISON_BUILDING_SET_ACTIVE_SPECIALIZATION_RESULT:
        case SMSG_GARRISON_COMPLETE_MISSION_RESULT:
        case SMSG_GARRISON_UPDATE_MISSION:
        case SMSG_GARRISON_CREATE_RESULT:
        case SMSG_GARRISON_DELETE_RESULT:
        case SMSG_GARRISON_FOLLOWER_CHANGED_DURABILITY:
        case SMSG_GARRISON_FOLLOWER_CHANGED_ITEM_LEVEL:
        case SMSG_GARRISON_FOLLOWER_CHANGED_XP:
        case SMSG_GARRISON_IS_UPGRADEABLE_RESULT:
        case SMSG_GARRISON_LANDING_PAGE_SHIPMENT_INFO:
        case SMSG_GARRISON_LEARN_BLUEPRINT_RESULT:
        case SMSG_GARRISON_LEARN_SPECIALIZATION_RESULT:
        case SMSG_GARRISON_LIST_FOLLOWERS_CHEAT_RESULT:
        case SMSG_GARRISON_LIST_MISSIONS_CHEAT_RESULT:
        case SMSG_GARRISON_MISSION_BONUS_ROLL_RESULT:
        case SMSG_GARRISON_NUM_FOLLOWER_ACTIVATIONS_REMAINING:
        case SMSG_GARRISON_OPEN_ARCHITECT:
        case SMSG_GARRISON_OPEN_MISSION_NPC:
        case SMSG_GARRISON_OPEN_TRADESKILL_NPC:
        case SMSG_GARRISON_PLACE_BUILDING_RESULT:
        case SMSG_GARRISON_PLOT_PLACED:
        case SMSG_GARRISON_PLOT_REMOVED:
        case SMSG_GARRISON_RECALL_PORTAL_LAST_USED_TIME:
        case SMSG_GARRISON_RECALL_PORTAL_USED:
        case SMSG_GARRISON_RECRUIT_FOLLOWER_RESULT:
        case SMSG_GARRISON_RECRUITMENT_FOLLOWERS_GENERATED:
        case SMSG_GARRISON_REMOTE_INFO:
        case SMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING_RESULT:
        case SMSG_GARRISON_REMOVE_FOLLOWER_RESULT:
        case SMSG_GARRISON_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA_RESULT:
        case SMSG_GARRISON_START_MISSION_RESULT:
        case SMSG_GARRISON_UNLEARN_BLUEPRINT_RESULT:
        case SMSG_GARRISON_UPGRADE_RESULT:
        case SMSG_GET_DISPLAYED_TROPHY_LIST_RESPONSE:
        case SMSG_GET_GARRISON_INFO_RESULT:
        case SMSG_GET_SHIPMENT_INFO_RESPONSE:
        case SMSG_GET_SHIPMENTS_OF_TYPE_RESPONSE:
        case SMSG_GUILD_PARTY_STATE:
        case SMSG_HIGHEST_THREAT_UPDATE:
        case SMSG_INITIAL_SETUP:
        case SMSG_INITIALIZE_FACTIONS:
        case SMSG_INSPECT_HONOR_STATS:
        case SMSG_INSPECT_PVP:
        case SMSG_INSPECT_TALENT:
        case SMSG_INSTANCE_ENCOUNTER_CHANGE_PRIORITY:
        case SMSG_INSTANCE_ENCOUNTER_DISENGAGE_UNIT:
        case SMSG_INSTANCE_ENCOUNTER_END:
        case SMSG_INSTANCE_ENCOUNTER_ENGAGE_UNIT:
        case SMSG_INSTANCE_ENCOUNTER_GAIN_COMBAT_RESURRECTION_CHARGE:
        case SMSG_INSTANCE_ENCOUNTER_IN_COMBAT_RESURRECTION:
        case SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_COMPLETE:
        case SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_START:
        case SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_UPDATE:
        case SMSG_INSTANCE_ENCOUNTER_PHASE_SHIFT_CHANGED:
        case SMSG_INSTANCE_ENCOUNTER_START:
        case SMSG_INSTANCE_ENCOUNTER_TIMER_START:
        case SMSG_INSTANCE_ENCOUNTER_SET_ALLOWING_RELEASE:
        case SMSG_ITEM_ENCHANT_TIME_UPDATE:
        case SMSG_ITEM_PURCHASE_REFUND_RESULT:
        case SMSG_LEARNED_SPELL:
        case SMSG_LFG_PARTY_INFO:
        case SMSG_LOAD_CUF_PROFILES:
        case SMSG_LOGIN_SET_TIME_SPEED:
        case SMSG_LOGIN_VERIFY_WORLD:
        case SMSG_LOGOUT_CANCEL_ACK:
        case SMSG_LOGOUT_RESPONSE:
        case SMSG_LOOT_RELEASE:
        case SMSG_LOOT_RESPONSE:
        case SMSG_MODIFY_COOLDOWN:
        case SMSG_MOUNT_RESULT:
        case SMSG_MONSTER_MOVE:
        case SMSG_SPLINE_MOVE_SET_RUN_SPEED:
        case SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED:
        case SMSG_SPLINE_MOVE_SET_SWIM_SPEED:
        case SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED:
        case SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED:
        case SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED:
        case SMSG_SPLINE_MOVE_SET_WALK_SPEED:
        case SMSG_SPLINE_MOVE_SET_TURN_RATE:
        case SMSG_SPLINE_MOVE_SET_PITCH_RATE:
        case SMSG_SPLINE_MOVE_ROOT:
        case SMSG_SPLINE_MOVE_UNROOT:
        case SMSG_SPLINE_MOVE_GRAVITY_DISABLE:
        case SMSG_SPLINE_MOVE_GRAVITY_ENABLE:
        case SMSG_SPLINE_MOVE_COLLISION_DISABLE:
        case SMSG_SPLINE_MOVE_COLLISION_ENABLE:
        case SMSG_SPLINE_MOVE_SET_FEATHER_FALL:
        case SMSG_SPLINE_MOVE_SET_NORMAL_FALL:
        case SMSG_SPLINE_MOVE_SET_HOVER:
        case SMSG_SPLINE_MOVE_UNSET_HOVER:
        case SMSG_SPLINE_MOVE_SET_WATER_WALK:
        case SMSG_SPLINE_MOVE_SET_LAND_WALK:
        case SMSG_SPLINE_MOVE_START_SWIM:
        case SMSG_SPLINE_MOVE_STOP_SWIM:
        case SMSG_SPLINE_MOVE_SET_RUN_MODE:
        case SMSG_SPLINE_MOVE_SET_WALK_MODE:
        case SMSG_SPLINE_MOVE_SET_FLYING:
        case SMSG_SPLINE_MOVE_UNSET_FLYING:
        case SMSG_FLIGHT_SPLINE_SYNC:
        case SMSG_MOVE_UPDATE:
        case SMSG_MOVE_TELEPORT:
        case SMSG_MOVE_UPDATE_TELEPORT:
        case SMSG_MOVE_ROOT:
        case SMSG_MOVE_UNROOT:
        case SMSG_MOVE_WATER_WALK:
        case SMSG_MOVE_LAND_WALK:
        case SMSG_MOVE_FEATHER_FALL:
        case SMSG_MOVE_NORMAL_FALL:
        case SMSG_MOVE_SET_ACTIVE_MOVER:
        case SMSG_MOVE_SET_CAN_FLY:
        case SMSG_MOVE_UNSET_CAN_FLY:
        case SMSG_MOVE_SET_HOVER:
        case SMSG_MOVE_UNSET_HOVER:
        case SMSG_MOVE_SET_CAN_TURN_WHILE_FALLING:
        case SMSG_MOVE_UNSET_CAN_TURN_WHILE_FALLING:
        case SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY:
        case SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY:
        case SMSG_MOVE_SET_VEHICLE_REC_ID:
        case SMSG_MOVE_SET_COLLISION_HEIGHT:
        case SMSG_MOVE_APPLY_MOVEMENT_FORCE:
        case SMSG_MOVE_REMOVE_MOVEMENT_FORCE:
        case SMSG_MOVE_SKIP_TIME:
        case SMSG_MOVE_ENABLE_GRAVITY:
        case SMSG_MOVE_DISABLE_GRAVITY:
        case SMSG_MOVE_ENABLE_COLLISION:
        case SMSG_MOVE_DISABLE_COLLISION:
        case SMSG_MOVE_ENABLE_DOUBLE_JUMP:
        case SMSG_MOVE_DISABLE_DOUBLE_JUMP:
        case SMSG_MOVE_SET_RUN_SPEED:
        case SMSG_MOVE_SET_RUN_BACK_SPEED:
        case SMSG_MOVE_SET_SWIM_SPEED:
        case SMSG_MOVE_SET_SWIM_BACK_SPEED:
        case SMSG_MOVE_SET_FLIGHT_SPEED:
        case SMSG_MOVE_SET_FLIGHT_BACK_SPEED:
        case SMSG_MOVE_SET_WALK_SPEED:
        case SMSG_MOVE_SET_TURN_RATE:
        case SMSG_MOVE_SET_PITCH_RATE:
        case SMSG_MOVE_SET_MOVEMENT_FORCE_SPEED:
        case SMSG_MOVE_KNOCK_BACK:
        case SMSG_MOVE_UPDATE_RUN_SPEED:
        case SMSG_MOVE_UPDATE_RUN_BACK_SPEED:
        case SMSG_MOVE_UPDATE_SWIM_SPEED:
        case SMSG_MOVE_UPDATE_SWIM_BACK_SPEED:
        case SMSG_MOVE_UPDATE_FLIGHT_SPEED:
        case SMSG_MOVE_UPDATE_FLIGHT_BACK_SPEED:
        case SMSG_MOVE_UPDATE_WALK_SPEED:
        case SMSG_MOVE_UPDATE_TURN_RATE:
        case SMSG_MOVE_UPDATE_PITCH_RATE:
        case SMSG_MOVE_UPDATE_KNOCK_BACK:
        case SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA:
        case SMSG_PET_LEARNED_SPELLS:
        case SMSG_PET_SPELLS_MESSAGE:
        case SMSG_PET_STABLE_LIST:
        case SMSG_PET_UNLEARNED_SPELLS:
        case SMSG_PHASE_SHIFT_CHANGE:
        case SMSG_PLAYED_TIME:
        case SMSG_PVP_LOG_DATA:
        case SMSG_QUERY_CREATURE_RESPONSE:
        case SMSG_NPC_TEXT_UPDATE:
        case SMSG_PET_NAME_QUERY_RESPONSE:
        case SMSG_QUERY_QUEST_INFO_RESPONSE:
        case SMSG_QUERY_TIME_RESPONSE:
        case SMSG_QUEST_COMPLETION_NPCRESPONSE:
        case SMSG_QUEST_GIVER_STATUS:
        case SMSG_QUEST_GIVER_STATUS_MULTIPLE:
        case SMSG_QUEST_UPDATE_ADD_CREDIT:
        case SMSG_QUEST_UPDATE_ADD_CREDIT_SIMPLE:
        case SMSG_QUEST_UPDATE_ADD_PVP_CREDIT:
        case SMSG_REPORT_PVP_PLAYER_AFK_RESULT:
        case SMSG_REQUEST_CEMETERY_LIST_RESPONSE:
        case SMSG_RESPEC_WIPE_CONFIRM:
        case SMSG_RESPOND_INSPECT_ACHIEVEMENTS:
        case SMSG_RESUME_TOKEN:
        case SMSG_SEND_KNOWN_SPELLS:
        case SMSG_SEND_SPELL_CHARGES:
        case SMSG_SEND_SPELL_HISTORY:
        case SMSG_SEND_UNLEARN_SPELLS:
        case SMSG_SET_AI_ANIM_KIT:
        case SMSG_SET_ALL_TASK_PROGRESS:
        case SMSG_UPDATE_CURRENCY:
        case SMSG_SET_FACTION_ATWAR:
        case SMSG_SET_FACTION_NOT_VISIBLE:
        case SMSG_SET_FACTION_STANDING:
        case SMSG_SET_FACTION_VISIBLE:
        case SMSG_SET_FLAT_SPELL_MODIFIER:
        case SMSG_SET_FORCED_REACTIONS:
        case SMSG_SET_ITEM_PURCHASE_DATA:
        case SMSG_SET_MELEE_ANIM_KIT:
        case SMSG_SET_MOVEMENT_ANIM_KIT:
        case SMSG_SET_PCT_SPELL_MODIFIER:
        case SMSG_SET_PLAY_HOVER_ANIM:
        case SMSG_SET_PROFICIENCY:
        case SMSG_SET_SPELL_CHARGES:
        case SMSG_SET_VEHICLE_REC_ID:
        case SMSG_SETUP_CURRENCY:
        case SMSG_SETUP_RESEARCH_HISTORY:
        case SMSG_SHOW_BANK:
        case SMSG_SOCKET_GEMS:
        case SMSG_SPELL_CHANNEL_START:
        case SMSG_SPELL_CHANNEL_UPDATE:
        case SMSG_SPELL_COOLDOWN:
        case SMSG_SPELL_DAMAGE_SHIELD:
        case SMSG_SPELL_DELAYED:
        case SMSG_SPELL_DISPELL_LOG:
        case SMSG_SPELL_ENERGIZE_LOG:
        case SMSG_SPELL_EXECUTE_LOG:
        case SMSG_SPELL_FAILED_OTHER:
        case SMSG_SPELL_FAILURE:
        case SMSG_SPELL_GO:
        case SMSG_SPELL_HEAL_LOG:
        case SMSG_SPELL_INSTAKILL_LOG:
        case SMSG_SPELL_INTERRUPT_LOG:
        case SMSG_SPELL_MISS_LOG:
        case SMSG_SPELL_NON_MELEE_DAMAGE_LOG:
        case SMSG_SPELL_OR_DAMAGE_IMMUNE:
        case SMSG_SPELL_PERIODIC_AURA_LOG:
        case SMSG_SPELL_ABSORB_LOG:
        case SMSG_SPELL_REGUID:
        case SMSG_SPELL_START:
        case SMSG_START_ELAPSED_TIMER:
        case SMSG_START_ELAPSED_TIMERS:
        case SMSG_SUMMON_REQUEST:
        case SMSG_SUPERCEDED_SPELLS:
        case SMSG_SUSPEND_TOKEN:
        case SMSG_TEXT_EMOTE:
        case SMSG_THREAT_REMOVE:
        case SMSG_THREAT_UPDATE:
        case SMSG_TIME_SYNC_REQUEST:
        case SMSG_TRADE_STATUS:
        case SMSG_TRADE_UPDATED:
        case SMSG_TRAINER_LIST:
        case SMSG_UNLEARNED_SPELLS:
        case SMSG_UPDATE_ACTION_BUTTONS:
        case SMSG_UPDATE_TALENT_DATA:
        case SMSG_UPDATE_WORLD_STATE:
        case SMSG_LIST_INVENTORY:
        case SMSG_VIGNETTE_UPDATE:
        case SMSG_VOID_STORAGE_CONTENTS:
        case SMSG_VOID_STORAGE_FAILED:
        case SMSG_VOID_STORAGE_TRANSFER_CHANGES:
        case SMSG_VOID_TRANSFER_RESULT:
        case SMSG_WEATHER:
        case SMSG_WEEKLY_SPELL_USAGE:
        case SMSG_WORLD_SERVER_INFO:
        case SMSG_WORLD_QUEST_UPDATE:
        case SMSG_WOW_TOKEN_BUY_REQUEST_CONFIRMATION:
        case SMSG_WOW_TOKEN_MARKET_PRICE_RESPONSE:
        case SMSG_WOW_TOKEN_REDEEM_REQUEST_CONFIRMATION:
        case SMSG_WOW_TOKEN_SELL_REQUEST_CONFIRMATION:
        case SMSG_WOW_TOKEN_UPDATE_AUCTIONABLE_LIST_RESPONSE:
        case SMSG_UPDATE_OBJECT:
        case SMSG_RESUME_COMMS:
        case SMSG_LOSS_OF_CONTROL_AURA_UPDATE:
            return ConnectionType::CONNECTION_TYPE_INSTANCE;
        default:
            return ConnectionType::CONNECTION_TYPE_REALM;
    }
}
