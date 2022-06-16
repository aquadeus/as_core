////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/** \file
    \ingroup u2w
*/

#ifndef CROSS
# include "WorldSocket.h"
# include "GarrisonMgr.hpp"
# include "InterRealmOpcodes.h"
# include "Channel.h"
# include "ChannelMgr.h"
# include "Guild.h"
#endif

#include <zlib.h>
#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "Vehicle.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "Group.h"
#include "World.h"
#include "ObjectAccessor.h"
#include "BattlegroundMgr.hpp"
#include "OutdoorPvPMgr.h"
#include "MapManager.h"
#include "SocialMgr.h"
#include "zlib.h"
#include "ScriptMgr.h"
#include "Transport.h"
#include "WardenWin.h"
#include "WardenMac.h"
#include "AccountMgr.h"
#include "PetBattle.h"
#include "CollectionMgr.hpp"
#include "Chat.h"
#include "BattlepayMgr.h"
#include "InstanceScript.h"
#include "AuthenticationPackets.h"
#include "RecruitAFriendMgr.hpp"

bool MapSessionFilter::Process(WorldPacket* packet)
{
    uint16 opcode = DropHighBytes(packet->GetOpcode());
    OpcodeHandler const* opHandle = g_OpcodeTable[WOW_CLIENT_TO_SERVER][opcode];

    if (!opHandle)
        return true;

    //let's check if our opcode can be really processed in Map::Update()
    if (opHandle->packetProcessing == PROCESS_INPLACE)
        return true;

    //we do not process thread-unsafe packets
    if (opHandle->packetProcessing == PROCESS_THREADUNSAFE)
        return false;

    Player* player = m_pSession->GetPlayer();
    if (!player)
        return false;

    //in Map::Update() we do not process packets where player is not in world!
    return player->IsInWorld();
}

//we should process ALL packets when player is not in world/logged in
//OR packet handler is not thread-safe!
bool WorldSessionFilter::Process(WorldPacket* packet)
{
    uint16 opcode = DropHighBytes(packet->GetOpcode());
    OpcodeHandler const* opHandle = g_OpcodeTable[WOW_CLIENT_TO_SERVER][opcode];

    if (!opHandle)
        return true;

    //thread-unsafe packets should be processed in World::UpdateSessions()
    if (opHandle->packetProcessing == PROCESS_THREADUNSAFE)
        return true;

    //no player attached? -> our client! ^^
    Player* player = m_pSession->GetPlayer();
    if (!player)
        return true;

    //lets process all packets for non-in-the-world player
    return (player->IsInWorld() == false);
}

/// WorldSession constructor
#ifndef CROSS
WorldSession::WorldSession(uint32 id, WorldSocket * sock, AccountTypes sec, bool p_IsAnimator, bool ispremium, uint8 premiumType, uint8 expansion, time_t mute_time, LocaleConstant locale, uint32 recruiter, bool isARecruiter, uint32 p_ServiceFlags, uint32 p_CustomFlags, uint32 p_LastMacAddr)
#else /* CROSS */
WorldSession::WorldSession(uint32 id, InterRealmClient* irc, AccountTypes sec, bool ispremium, uint8 expansion, time_t mute_time, LocaleConstant locale, uint32 recruiter, bool isARecruiter, std::string p_ServerName)
#endif /* CROSS */
{
    ///////////////////////////////////////////////////////////////////////////////
    /// Members initialization
    ///////////////////////////////////////////////////////////////////////////////
    m_Player                = nullptr;
    m_muteTime              = mute_time;
    _security               = sec;
    _accountId              = id;
    m_expansion             = expansion;
    _ispremium              = ispremium;

    m_PremiumType           = 0;

    m_sessionDbLocaleIndex  = locale;
    recruiterId             = recruiter;
    isRecruiter             = isARecruiter;

    m_ServiceFlags          = 0;
    m_CustomFlags           = 0;

    m_sessionDbcLocale      = sWorld->GetAvailableDbcLocale(locale);

    m_timeOutTime                       = 0;
    timeCharEnumOpcode                  = 0;
    m_TimeLastChannelInviteCommand      = 0;
    m_TimeLastChannelPassCommand        = 0;
    m_TimeLastChannelMuteCommand        = 0;
    m_TimeLastChannelBanCommand         = 0;
    m_TimeLastChannelUnbanCommand       = 0;
    m_TimeLastChannelAnnounceCommand    = 0;
    m_TimeLastGroupInviteCommand        = 0;
    m_TimeLastChannelModerCommand       = 0;
    m_TimeLastChannelOwnerCommand       = 0;
    m_TimeLastChannelUnmoderCommand     = 0;
    timeLastArenaTeamCommand            = 0;
    m_TimeLastChannelSetownerCommand    = 0;
    m_TimeLastChannelUnmuteCommand      = 0;
    m_TimeLastChannelKickCommand        = 0;
    m_TimeLastAddFriend                 = 0;
    m_TimeLastAddIgnoreFriend           = 0;
    m_TimeLastGarrisonInfo              = 0;
    timeLastServerCommand               = 0;
    timeLastChangeSubGroupCommand       = 0;
    m_TimeLastSellItemOpcode            = 0;
    m_uiAntispamMailSentCount           = 0;
    m_PlayerLoginCounter                = 0;
    m_clientTimeDelay                   = 0;
    m_TimeLastUseItem                   = 0;
    m_TimeLastTicketOnlineList          = 0;
    m_AccountJoinDate                   = 0;
    timeLastWhoCommand                  = 0;
    _logoutTime                         = 0;
    m_latency                           = 0;
    m_TimeLastMoveBankGuildItems        = 0;
    m_TimeLastLogout                    = 0;

    m_ActivityDays = 0;
    m_LastBan = 0;
    m_LastClaim = 0;


    m_EmailValidated = false;
    m_AlreadyPurchasePoints = false;

    m_IsStressTestSession      = false;
    m_playerRecentlyLogout     = false;
    m_playerSave               = false;
    m_TutorialsChanged         = false;
    m_playerLoading            = 0;
    m_playerLogout             = false;
    m_inQueue                  = false;
    m_IsPetBattleJournalLocked = false;
    m_HasRecentSpellPacket     = false;
    m_Trusted                  = false;
    ///////////////////////////////////////////////////////////////////////////////

    m_LoyaltyEvents.resize((uint32)LoyaltyEvent::Max, 0);

    _warden = NULL;
    _filterAddonMessages = false;
    m_LoginTime = time(nullptr);

    forceExit = false;
    forceExitTime = 0;
    expireTime = 60000; ///< 1 min after socket loss, session is deleted

#ifndef CROSS
    m_LastMacAddr = p_LastMacAddr;
    m_BackFromCross     = false;
    m_InterRealmZoneId  = 0;
    m_ServiceFlags      = p_ServiceFlags;
    m_CustomFlags       = p_CustomFlags;
    m_PremiumType       = premiumType;
    m_IsAnimator        = p_IsAnimator;

    m_Socket[CONNECTION_TYPE_REALM]    = sock;
    m_Socket[CONNECTION_TYPE_INSTANCE] = nullptr;

    _instanceConnectKey.Raw = UI64LIT(0);

    if (sock)
    {
        m_Address = sock->GetRemoteAddress();
        sock->AddReference();
        ResetTimeOutTime();
        LoginDatabase.PExecute("UPDATE account SET online = 1 WHERE id = %u;", GetAccountId());     // One-time query
    }
    m_ChangelogReceived = false;
#else
    m_isinIRBG = false;
    m_ir_socket = irc;
    m_ir_closing = false;
    m_ir_server_name = p_ServerName;
    m_ir_number = irc->GetRealmId();

    m_RemoveType = 0;
    m_RemoveProgress = 0;

    m_GUIDLow = 0;
    m_GUID = 0;
    m_RealGUID = 0;

    m_ChangelogReceived = true;
#endif

    InitializeQueryCallbackParameters();

    _collectionMgr = std::unique_ptr<CollectionMgr>(new CollectionMgr(this));

    m_IsInWorldUpdateSession = false;
    m_IsInDestructor = false;
    m_IsSendingPacketFromIRSocket = false;
    m_CanResumCommms = false;
    m_IsDummySession = false;
    m_IsUpdating = false;

    m_QueryCallbackMgr = std::make_shared<QueryCallbackManager>();

    sLog->outInfo(LOG_FILTER_POINTERS, "Pointer 0x%" PRIxPTR " -> WorldSession : AccountId: %u", std::uintptr_t(this), id);
}

/// WorldSession destructor
WorldSession::~WorldSession()
{
    m_IsInDestructor = true;

    ///- unload player if not unloaded
    if (m_Player && !m_IsDummySession)
        LogoutPlayer (true);

#ifndef CROSS
    /// - If have unclosed socket, close it
    for (uint8 i = 0; i < MAX_CONNECTION_TYPES; ++i)
    {
        if (m_Socket[i])
        {
            while (m_IsSendingPacketFromIRSocket)
                ACE_Based::Thread::Sleep(1);

            m_Socket[i]->CloseSocket();
            m_Socket[i]->RemoveReference();
            m_Socket[i] = nullptr;
        }
    }

    if (!m_IsDummySession)
        LoginDatabase.PExecute("UPDATE account SET online = 0 WHERE id = %u;", GetAccountId());     // One-time query
#endif

    if (_warden)
        delete _warden;

    ///- empty incoming packet queue
    {
        WorldPacket* packet = nullptr;
        while (m_RecvQueue.next(packet))
            delete packet;

        while (m_ConcurrentRecvQueue.try_dequeue(packet))
            delete packet;
    }
}

/// Get the player name
std::string WorldSession::GetPlayerName(bool simple /* = true */) const
 {
    std::string name = "[Player: ";
    uint32 guidLow = 0;

    if (Player* player = GetPlayer())
    {
        name.append(player->GetName());
        guidLow = player->GetGUIDLow();
    }
    else
        name.append("<none>");

    if (!simple)
    {
        std::ostringstream ss;
        ss << " (Guid: " << guidLow << ", Account: " << GetAccountId() << ")";
        name.append(ss.str());
    }

    name.append("]");
    return name;
}

/// Get player guid if available. Use for logging purposes only
uint32 WorldSession::GetGuidLow() const
{
    return GetPlayer() ? GetPlayer()->GetGUIDLow() : 0;
}


void WorldSession::SendPackets(std::vector<WorldPacket*>& p_Packets, ConnectionType p_ConnIdx)
{
# ifndef CROSS
    if (!m_Socket[p_ConnIdx])
        return;

    m_Socket[p_ConnIdx]->SendPackets(p_Packets);
# else
    if (!m_ir_socket || !m_Player || m_ir_closing)
        return;

    for (WorldPacket* l_Packet : p_Packets)
    {
        m_ir_socket->SendTunneledPacket(m_Player->GetRealGUID(), l_Packet);

        if (l_Packet->m_SafeReferenceCount.fetch_sub(1) == 1)
            delete l_Packet;
    }
# endif
}

/// Send a packet to the client
void WorldSession::SendPacket(WorldPacket const* packet, bool forced /*= false*/, bool ir_packet /*=false*/)
{
    if (!forced)
    {
        OpcodeHandler* handler = g_OpcodeTable[WOW_SERVER_TO_CLIENT][packet->GetOpcode()];
        if (!handler || handler->status == STATUS_UNHANDLED)
            return;
    }

#ifdef CROSS

    if (!m_ir_socket || !m_Player || m_ir_closing)
        return;

    if (!m_isinIRBG && packet->GetOpcode() != SMSG_BATTLEFIELD_LIST &&
        packet->GetOpcode() != SMSG_BATTLEFIELD_STATUS_NONE &&
        packet->GetOpcode() != SMSG_BATTLEFIELD_STATUS_FAILED &&
        packet->GetOpcode() != SMSG_BATTLEFIELD_STATUS_QUEUED &&
        packet->GetOpcode() != SMSG_BATTLEFIELD_STATUS_ACTIVE &&
        packet->GetOpcode() != SMSG_NEW_WORLD &&
        packet->GetOpcode() != SMSG_TRANSFER_PENDING &&
        packet->GetOpcode() != SMSG_SUSPEND_TOKEN &&
        packet->GetOpcode() != SMSG_BATTLEFIELD_STATUS_NEED_CONFIRMATION)
        return;

    if (sWorld->getBoolConfig(CONFIG_ENABLE_BROADCAST_MESSAGE_BATCH) && WorldPacket::GetCurrentPacketID() != -1 && m_Player)
    {
        WorldPacket::AddMapPacket(PacketSendingInfo(m_Player, *packet, true, false));
        return;
    }

    m_ir_socket->SendTunneledPacket(m_Player->GetRealGUID(), packet);

#else

    if (!ir_packet && GetInterRealmBG() && !CanBeSentDuringInterRealm(packet->GetOpcode()))
        return;

    if (packet->GetOpcode() == NULL_OPCODE && !forced)
    {
        sLog->outError(LOG_FILTER_OPCODES, "Prevented sending of NULL_OPCODE to %s", GetPlayerName(false).c_str());
        return;
    }
    else if (packet->GetOpcode() == uint16(UNKNOWN_OPCODE) && !forced)
    {
        sLog->outError(LOG_FILTER_OPCODES, "Prevented sending of UNKNOWN_OPCODE to %s", GetPlayerName(false).c_str());
        return;
    }

    if (sWorld->getBoolConfig(CONFIG_ENABLE_BROADCAST_MESSAGE_BATCH) && WorldPacket::GetCurrentPacketID() != -1 && m_Player)
    {
        WorldPacket::AddMapPacket(PacketSendingInfo(m_Player, *packet, true, false));
        return;
    }

    ConnectionType l_ConnType = GetOpcodeConnectionType(packet->GetOpcode());

    if (!m_Socket[l_ConnType])
        return;

    m_Socket[l_ConnType]->SendPacket(*packet);
#endif
}

/// Add an incoming packet to the queue
void WorldSession::QueuePacket(WorldPacket* new_packet)
{
    m_ConcurrentRecvQueue.enqueue(new_packet);
}

/// Logging helper for unexpected opcodes
void WorldSession::LogUnexpectedOpcode(WorldPacket* packet, const char* status, const char *reason)
{
    sLog->outError(LOG_FILTER_OPCODES, "Received unexpected opcode %s Status: %s Reason: %s from %s",
        GetOpcodeNameForLogging(packet->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), status, reason, GetPlayerName(false).c_str());
}

/// Logging helper for unexpected opcodes
void WorldSession::LogUnprocessedTail(WorldPacket* packet)
{
    sLog->outError(LOG_FILTER_OPCODES, "Unprocessed tail data (read stop at %u from %u) Opcode %s from %s",
        uint32(packet->rpos()), uint32(packet->wpos()), GetOpcodeNameForLogging(packet->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), GetPlayerName(false).c_str());
    packet->print_storage();
}

struct OpcodeInfo
{
    OpcodeInfo(uint32 nb, uint32 time) : nbPkt(nb), totalTime(time) {}
    uint32 nbPkt;
    uint32 totalTime;
};

std::map<uint16, uint32> k_maxOpcodeByUpdate =
{
   { CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK       , 1},
   { CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER       , 1},
   { CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK         , 1},
   { CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK_COUNT , 1},
   { CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_COUNT , 1},
   { CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK_COUNT   , 1},
   { CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_AUTO  , 1},
   { CMSG_GUILD_BANK_MERGE_ITEMS_PLAYER_BANK      , 1},
   { CMSG_GUILD_BANK_MERGE_ITEMS_BANK_PLAYER      , 1},
   { CMSG_GUILD_BANK_MERGE_ITEMS_BANK_BANK        , 1},
   { CMSG_GUILD_BANK_SWAP_ITEMS_BANK_PLAYER       , 1},
   { CMSG_GUILD_BANK_SWAP_ITEMS_BANK_BANK         , 1}
};

#ifdef CROSS
/// Update the WorldSession (triggered by World update)
bool WorldSession::Update(uint32 diff, PacketFilter& updater)
{
    m_QueryCallbackMgr->Update();

    if (IsIRClosing())
        return false;

    if (!GetInterRealmClient())
        return false;

    bool deletePacket = true;

    uint32 processedPackets = 0;

    uint32 opcode = 0;

    ///- Retrieve packets from the receive queue and call the appropriate handlers
    /// not process packets if socket already closed
    WorldPacket* packet = nullptr;

    if (updater.ProcessLogout())
        m_IsInWorldUpdateSession = true;

    /// Get the packet from the lock-free queue and add them to the processing packet queue
    while (m_ConcurrentRecvQueue.try_dequeue(packet))
        m_RecvQueue.add(packet);

    while (!m_RecvQueue.empty() && m_RecvQueue.next(packet, updater))
    {
        opcode = packet->GetOpcode();

        const OpcodeHandler* opHandle = g_OpcodeTable[WOW_CLIENT_TO_SERVER][packet->GetOpcode()];
        if (opHandle)
        {
            try
            {
                switch (opHandle->status)
                {
                case STATUS_LOGGEDIN:
                    if (m_Player && m_Player->IsInWorld())
                        (this->*opHandle->handler)(*packet);
                    break;
                case STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT:
                case STATUS_TRANSFER:
                case STATUS_AUTHED:
                    (this->*opHandle->handler)(*packet);
                    break;
                case STATUS_NEVER:
                case STATUS_UNHANDLED:
                    break;
                }
            }
            catch (ByteBufferException &)
            {
                //
            }
        }

        if (packet != NULL)
            delete packet;

#define MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE 200
        processedPackets++;

        if (processedPackets > MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE)
            break;
    }

    ProcessQueryCallbacks();

    if (m_Player && (!m_IsInWorldUpdateSession || (m_IsInWorldUpdateSession && !m_Player->IsInWorld())))
        m_Player->UpdateTasks();

    m_IsInWorldUpdateSession = false;

    return true;
}
#else
/// Update the WorldSession (triggered by World update)
bool WorldSession::Update(uint32 diff, PacketFilter& updater)
{
    m_IsUpdating = true;

    m_QueryCallbackMgr->Update();

    /// Antispam Timer update
    if (sWorld->getBoolConfig(CONFIG_ANTISPAM_ENABLED))
        UpdateAntispamTimer(diff);

    /// Update Timeout timer.
    UpdateTimeOutTime(diff);

    ///- Before we process anything:
    /// If necessary, kick the player from the character select screen
    if (IsConnectionIdle() && m_Socket[CONNECTION_TYPE_REALM])
        m_Socket[CONNECTION_TYPE_REALM]->CloseSocket();

    if (updater.ProcessLogout())
        m_IsInWorldUpdateSession = true;

    m_HasRecentSpellPacket = false;

    m_LastSpellCasted.clear();

    ///- Retrieve packets from the receive queue and call the appropriate handlers
    /// not process packets if socket already closed
    WorldPacket* packet = nullptr;

    /// When the session have a player in world, only process the incoming packet queue in the map threads to reduce the load in World::UpdateSessions
    if (!m_IsInWorldUpdateSession || !m_Player || !m_Player->IsInWorld())
    {
        /// Get the packet from the lock-free queue and add them to the processing packet queue
        while (m_ConcurrentRecvQueue.try_dequeue(packet))
            m_RecvQueue.add(packet);
    }

    //! Delete packet after processing by default
    bool deletePacket = true;
    //! To prevent infinite loop
    WorldPacket* firstDelayedPacket = nullptr;
    //! If _recvQueue.peek() == firstDelayedPacket it means that in this Update call, we've processed all
    //! *properly timed* packets, and we're now at the part of the queue where we find
    //! delayed packets that were re-enqueued due to improper timing. To prevent an infinite
    //! loop caused by re-enqueueing the same packets over and over again, we stop updating this session
    //! and continue updating others. The re-enqueued packets will be handled in the next Update call for this session.
    uint32 processedPackets = 0;

    std::map<uint16, uint32> l_LimitedOpcodeHandled;

#define MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE 50
    while (m_Socket[CONNECTION_TYPE_REALM] && !m_Socket[CONNECTION_TYPE_REALM]->IsClosed() &&
            !m_RecvQueue.empty() && m_RecvQueue.peek(true) != firstDelayedPacket &&
            m_RecvQueue.next(packet, updater))
    {
        const OpcodeHandler* opHandle = g_OpcodeTable[WOW_CLIENT_TO_SERVER][packet->GetOpcode()];

        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::time_point();

        if (sMapMgr->m_LogMapPerformance)
            start = std::chrono::steady_clock::now();

        /// Prevent fast logout/login to mess around with the async saving
        if (packet->GetOpcode() == CMSG_PLAYER_LOGIN && time(nullptr) < (m_TimeLastLogout + 10))
        {
            if (!firstDelayedPacket)
                firstDelayedPacket = packet;

            QueuePacket(packet);
            continue;
        }

        /// Discard any packets when you are in a transfer state (avoid duplications based on async saving/loading)
        if (m_Player && m_Player->GetInterRealmPlayerState() == InterRealmPlayerState::InTransfer)
        {
            delete packet;
            processedPackets++;

            if (processedPackets > MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE)
                break;

            continue;
        }

        try
        {
            switch (opHandle->status)
            {
                case STATUS_LOGGEDIN:
                    if (!m_Player)
                    {
                        // skip STATUS_LOGGEDIN opcode unexpected errors if player logout sometime ago - this can be network lag delayed packets
                        //! If player didn't log out a while ago, it means packets are being sent while the server does not recognize
                        //! the client to be in world yet. We will re-add the packets to the bottom of the queue and process them later.
                        if (!m_playerRecentlyLogout)
                        {
                            //! Prevent infinite loop
                            if (!firstDelayedPacket)
                                firstDelayedPacket = packet;
                            //! Because checking a bool is faster than reallocating memory
                            deletePacket = false;
                            QueuePacket(packet);
                            //! Log
                                sLog->outDebug(LOG_FILTER_NETWORKIO, "Re-enqueueing packet with opcode %s with with status STATUS_LOGGEDIN. "
                                    "Player is currently not in world yet.", GetOpcodeNameForLogging(packet->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str());
                        }
                    }
                    else if (m_Player->IsInWorld())
                    {
                        switch (packet->GetOpcode())
                        {
                            case CMSG_CAST_SPELL:
                            case CMSG_CANCEL_CAST:
                            case CMSG_CANCEL_AURA:
                            case CMSG_CANCEL_CHANNELLING:
                            //case CMSG_SET_SELECTION:
                            case CMSG_ATTACK_SWING:
                            case CMSG_ATTACK_STOP:
                            case CMSG_PET_ACTION:
                                m_HasRecentSpellPacket = true;
                                break;
                            default:
                                break;
                        }

                        sScriptMgr->OnPacketReceive(WorldPacket(*packet), this);
                        (this->*opHandle->handler)(*packet);
                        if (sLog->ShouldLog(LOG_FILTER_NETWORKIO, LOG_LEVEL_TRACE) && packet->rpos() < packet->wpos())
                            LogUnprocessedTail(packet);
                    }
                    else if (m_InterRealmZoneId)
                        (this->*opHandle->handler)(*packet);
                    // lag can cause STATUS_LOGGEDIN opcodes to arrive after the player started a transfer
                    break;
                case STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT:
                    if (!m_Player && !m_playerRecentlyLogout && !m_playerLogout) // There's a short delay between _player = null and m_playerRecentlyLogout = true during logout
                        LogUnexpectedOpcode(packet, "STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT",
                            "the player has not logged in yet and not recently logout");
                    else
                    {
                        // not expected _player or must checked in packet hanlder
                        sScriptMgr->OnPacketReceive(WorldPacket(*packet), this);
                        (this->*opHandle->handler)(*packet);
                        if (sLog->ShouldLog(LOG_FILTER_NETWORKIO, LOG_LEVEL_TRACE) && packet->rpos() < packet->wpos())
                            LogUnprocessedTail(packet);
                    }
                    break;
                case STATUS_TRANSFER:
                    if (!m_Player)
                        LogUnexpectedOpcode(packet, "STATUS_TRANSFER", "the player has not logged in yet");
                    else if (m_Player->IsInWorld())
                        LogUnexpectedOpcode(packet, "STATUS_TRANSFER", "the player is still in world");
                    else
                    {
                        sScriptMgr->OnPacketReceive(WorldPacket(*packet), this);
                        (this->*opHandle->handler)(*packet);
                        if (sLog->ShouldLog(LOG_FILTER_NETWORKIO, LOG_LEVEL_TRACE) && packet->rpos() < packet->wpos())
                            LogUnprocessedTail(packet);
                    }
                    break;
                case STATUS_AUTHED:
                    // prevent cheating with skip queue wait
                    if (m_inQueue)
                    {
                        LogUnexpectedOpcode(packet, "STATUS_AUTHED", "the player not pass queue yet");
                        break;
                    }

                    // some auth opcodes can be recieved before STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT opcodes
                    // however when we recieve CMSG_CHAR_ENUM we are surely no longer during the logout process.
                    if (packet->GetOpcode() == CMSG_ENUM_CHARACTERS)
                        m_playerRecentlyLogout = false;

                    sScriptMgr->OnPacketReceive(WorldPacket(*packet), this);
                    (this->*opHandle->handler)(*packet);
                    if (sLog->ShouldLog(LOG_FILTER_NETWORKIO, LOG_LEVEL_TRACE) && packet->rpos() < packet->wpos())
                        LogUnprocessedTail(packet);
                    break;
                case STATUS_NEVER:
                        sLog->outError(LOG_FILTER_OPCODES, "Received not allowed opcode %s from %s", GetOpcodeNameForLogging(packet->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str()
                            , GetPlayerName(false).c_str());
                    break;
                case STATUS_UNHANDLED:
                        sLog->outError(LOG_FILTER_OPCODES, "Received not handled opcode %s from %s", GetOpcodeNameForLogging(packet->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str()
                            , GetPlayerName(false).c_str());
                    break;
            }
        }
        catch(ByteBufferException &)
        {
            if (deletePacket)
            {
                sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::Update ByteBufferException occured while parsing a packet (opcode: %u) from client %s, accountid=%i. Skipped packet.",
                    packet->GetOpcode(), GetRemoteAddress().c_str(), GetAccountId());
                packet->hexlike();
            }
        }

        if (k_maxOpcodeByUpdate.find(packet->GetOpcode()) != k_maxOpcodeByUpdate.end())
        {
            if (l_LimitedOpcodeHandled.find(packet->GetOpcode()) == l_LimitedOpcodeHandled.end())
                l_LimitedOpcodeHandled[packet->GetOpcode()] = 0;

            l_LimitedOpcodeHandled[packet->GetOpcode()]++;

            if (l_LimitedOpcodeHandled[packet->GetOpcode()] >= k_maxOpcodeByUpdate[packet->GetOpcode()])
                break;
        }

        if (deletePacket)
            delete packet;

        processedPackets++;

        //process only a max amout of packets in 1 Update() call.
        //Any leftover will be processed in next update
        if (processedPackets > MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE)
            break;
    }

    if (m_Socket[CONNECTION_TYPE_REALM] && !m_Socket[CONNECTION_TYPE_REALM]->IsClosed() && _warden)
        _warden->Update();

    ProcessQueryCallbacks();

    time_t currTime = time(NULL);

    Player* l_Player = m_Player;
    bool l_CanLogout = l_Player && !m_playerLoading && !l_Player->IsBeingTeleportedFar() && !l_Player->m_IsInMapSwitch && (m_TimeLastMoveBankGuildItems + 5) < time(nullptr)
        && ((!m_IsInWorldUpdateSession && l_Player->IsInWorld()) || (m_IsInWorldUpdateSession && !l_Player->IsInWorld())) && !l_Player->m_TaskInProgress;  ///< Logout players in map thread if the player is in world

    if ((forceExit || forceExitTime) && !m_Player)
    {
        forceExit = false;
        forceExitTime = 0;
    }

    if (l_Player && !m_playerLoading && forceExitTime && GetMSTimeDiffToNow(forceExitTime) > MINUTE * IN_MILLISECONDS)
    {
        l_CanLogout = true;
        sLog->outAshran("Force to logout a kicked player account %u guid %u playerloading: %u IsBeingTeleportedFar %u m_IsInMapSwitch %u m_TaskInProgress %u",
            GetAccountId(), l_Player->GetGUIDLow(), m_playerLoading, l_Player->IsBeingTeleportedFar(), l_Player->m_IsInMapSwitch, (bool)l_Player->m_TaskInProgress);
    }

    ///- If necessary, log the player out
    if (l_CanLogout && (ShouldLogOut(currTime) || forceExit || !expireTime))
        LogoutPlayer(true);

    l_Player = m_Player;

    if (m_IsInWorldUpdateSession && !m_IsStressTestSession)
    {
        if (m_Socket[CONNECTION_TYPE_REALM] && l_Player && _warden)
            _warden->Update();

        ///- Cleanup socket pointer if need
        if ((m_Socket[CONNECTION_TYPE_REALM] && m_Socket[CONNECTION_TYPE_REALM]->IsClosed()) || (m_Socket[CONNECTION_TYPE_INSTANCE] && m_Socket[CONNECTION_TYPE_INSTANCE]->IsClosed()))
        {
            expireTime -= expireTime > diff ? diff : expireTime;
            if (!l_Player)
            {
                if (m_Socket[CONNECTION_TYPE_REALM])
                {
                    m_Socket[CONNECTION_TYPE_REALM]->CloseSocket();
                    m_Socket[CONNECTION_TYPE_REALM]->RemoveReference();
                    m_Socket[CONNECTION_TYPE_REALM] = nullptr;
                }

                if (m_Socket[CONNECTION_TYPE_INSTANCE])
                {
                    m_Socket[CONNECTION_TYPE_INSTANCE]->CloseSocket();
                    m_Socket[CONNECTION_TYPE_INSTANCE]->RemoveReference();
                    m_Socket[CONNECTION_TYPE_INSTANCE] = nullptr;
                }
            }
        }

        if (!m_Socket[CONNECTION_TYPE_REALM])
        {
            m_IsUpdating = false;
            m_IsInWorldUpdateSession = false;
            return false;                                       //Will remove this session from the world session map
        }
    }

    int l_Count = 0;
    while (!m_HotfixQueue.empty() && l_Count++ < 10)
    {
        std::pair<uint32_t, uint32_t> l_Pair = m_HotfixQueue.front();

        uint32_t l_Hash = l_Pair.first;
        uint32_t l_Entry = l_Pair.second;

        DB2StorageBase* l_DB2Store = sDB2PerHash[l_Hash];

        if (l_DB2Store)
        {
            ByteBuffer l_ResponseData(2 * 1024);
            if (l_DB2Store->WriteRecord(l_Entry, l_ResponseData, GetSessionDbLocaleIndex(), sObjectMgr->HotfixTableIndexInfos[l_Hash]))
            {
                WorldPacket l_Data(SMSG_DB_REPLY, 4 + 4 + 4 + 4 + l_ResponseData.size());
                l_Data << uint32(l_Hash);
                l_Data << uint32(l_Entry);
                l_Data << uint32(sObjectMgr->GetHotfixDate(l_Entry, l_Hash));
                l_Data.WriteBit(true);
                l_Data.FlushBits();
                l_Data << uint32(l_ResponseData.size());
                l_Data.append(l_ResponseData);

                SendPacket(&l_Data);
            }
            else
            {
                WorldPacket l_Data(SMSG_DB_REPLY, 4 + 4 + 4 + 4);
                l_Data << uint32(l_Hash);
                l_Data << uint32(-int32(l_Entry));
                l_Data << uint32(time(NULL));
                l_Data.WriteBit(false);
                l_Data.FlushBits();
                l_Data << uint32(0);

                SendPacket(&l_Data);
            }
        }

        m_HotfixQueue.pop();
    }

    if (l_Player && (!m_IsInWorldUpdateSession || (m_IsInWorldUpdateSession && !l_Player->IsInWorld())))
        l_Player->UpdateTasks();

    m_IsInWorldUpdateSession = false;
    m_IsUpdating = false;

    return true;
}
#endif

/// %Log the player out
void WorldSession::LogoutPlayer(bool p_Save, bool p_AfterInterRealm)
{
    std::ostringstream l_Log;

    uint32 l_StartTime = getMSTime();

    m_TimeLastLogout = time(nullptr);

    sPetBattleSystem->LeaveQueue(m_Player);

    /// Reset the login counter
    m_PlayerLoginCounter = 0;

    // fix exploit with Aura Bind Sight
    m_Player->StopCastingBindSight();
    m_Player->StopCastingCharm();
    m_Player->RemoveAurasByType(SPELL_AURA_BIND_SIGHT);

    l_Log << getMSTime() - l_StartTime << " ";

    forceExit = false;
    forceExitTime = 0;
    m_playerLogout = true;
    m_playerSave = p_Save;

    /// Fix problems with SPELL_AURA_MOD_TIME_RATE
    /// after m_playerLogout = true
    m_Player->RemoveAurasByType(AuraType::SPELL_AURA_MOD_TIME_RATE);

    if (m_Player)
    {
#ifndef CROSS
        if (m_Player->GetMap() && m_Player->IsInDraenorGarrison() && m_Player->GetDraenorGarrison())
            m_Player->GetDraenorGarrison()->OnPlayerLeave();
#endif

        l_Log << getMSTime() - l_StartTime << " ";

        if (uint64 lguid = m_Player->GetLootGUID())
            DoLootRelease(lguid);

        l_Log << getMSTime() - l_StartTime << " ";

        ///- If the player just died before logging out, make him appear as a ghost
        //FIXME: logout must be delayed in case lost connection with client in time of combat
        if (m_Player->GetDeathTimer())
        {
            m_Player->getHostileRefManager().deleteReferences();

            /// Make player appears as a ghost only if there is no encounter in progress (since WoD, for battle resurrection)
            if (InstanceScript* l_Instance = m_Player->GetInstanceScript())
            {
                if (!(l_Instance->IsEncounterInProgress() && l_Instance->instance->Expansion() >= Expansion::EXPANSION_WARLORDS_OF_DRAENOR))
                {
                    m_Player->BuildPlayerRepop();
                    m_Player->RepopAtGraveyard();
                }
            }
            else
            {
                m_Player->BuildPlayerRepop();
                m_Player->RepopAtGraveyard();
            }

            l_Log << getMSTime() - l_StartTime << " ";
        }
        else if (!m_Player->getAttackers().empty())
        {
            // build set of player who attack _player or who have pet attacking of _player
            std::set<Player*> aset;
            for (Unit::AttackerSet::const_iterator itr = m_Player->getAttackers().begin(); itr != m_Player->getAttackers().end(); ++itr)
            {
                Unit* owner = (*itr)->GetOwner();           // including player controlled case
                if (owner && owner->IsPlayer())
                    aset.insert(owner->ToPlayer());
                else if ((*itr)->IsPlayer())
                    aset.insert((Player*)(*itr));
            }


            // CombatStop() method is removing all attackers from the AttackerSet
            // That is why it must be AFTER building current set of attackers
            m_Player->CombatStop();
            m_Player->getHostileRefManager().setOnlineOfflineState(false);
            m_Player->RemoveAllAurasOnDeath();
            m_Player->SetPvPDeath(!aset.empty());
            m_Player->KillPlayer();
            m_Player->BuildPlayerRepop();
            m_Player->RepopAtGraveyard();

            // give honor to all attackers from set like group case
            for (std::set<Player*>::const_iterator itr = aset.begin(); itr != aset.end(); ++itr)
                (*itr)->RewardHonor(m_Player, aset.size());

            // give bg rewards and update counters like kill by first from attackers
            // this can't be called for all attackers.
            if (!aset.empty())
                if (Battleground* bg = m_Player->GetBattleground())
                    bg->HandleKillPlayer(m_Player, *aset.begin());
        }
        else if (m_Player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        {
            // this will kill character by SPELL_AURA_SPIRIT_OF_REDEMPTION
            m_Player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
            m_Player->KillPlayer();
            m_Player->BuildPlayerRepop();
            m_Player->RepopAtGraveyard();
        }
#ifndef CROSS
        else if (m_Player->HasPendingBind())
        {
            m_Player->RepopAtGraveyard();
            m_Player->SetPendingBind(0, 0);
        }
#endif
        else if (m_Player->GetVehicleBase() && m_Player->isInCombat())
        {
            m_Player->KillPlayer();
            m_Player->BuildPlayerRepop();
            m_Player->RepopAtGraveyard();
        }

        l_Log << getMSTime() - l_StartTime << " ";

        //drop a flag if player is carrying it
        if (Battleground* bg = m_Player->GetBattleground())
            bg->EventPlayerLoggedOut(m_Player);

        l_Log << getMSTime() - l_StartTime << " ";

#ifndef CROSS
        ///- Teleport to home if the player is in an invalid instance
        if (!m_Player->m_InstanceValid && !m_Player->isGameMaster())
        {
            m_Player->SetLocationMapId(m_Player->m_homebindMapId);
            m_Player->Relocate(m_Player->m_homebindX, m_Player->m_homebindY, m_Player->m_homebindZ, m_Player->GetOrientation());
            m_Player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CHANGE_MAP | AURA_INTERRUPT_FLAG_MOVE | AURA_INTERRUPT_FLAG_TURNING);
        }
#endif /* not CROSS */

        l_Log << getMSTime() - l_StartTime << " ";

        sOutdoorPvPMgr->HandlePlayerLeaveZone(m_Player, m_Player->GetZoneId());

        l_Log << getMSTime() - l_StartTime << " ";

        for (int i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
        {
            if (MS::Battlegrounds::BattlegroundType::Type bgQueueTypeId = m_Player->GetBattlegroundQueueTypeId(i))
            {
                m_Player->RemoveBattlegroundQueueId(bgQueueTypeId);
                sBattlegroundMgr->RemovePlayer(m_Player->GetGUID(), true, bgQueueTypeId);
            }
        }

        l_Log << getMSTime() - l_StartTime << " ";

        /// If, when the player logout, the battleground pointer of the player is still good, we apply deserter buff.
        if (p_Save && m_Player->GetBattleground() != nullptr && m_Player->GetBattleground()->GetStatus() != STATUS_WAIT_LEAVE)
        {
            /// We add the Deserter buff, otherwise it can be used bug.
            m_Player->AddAura(MS::Battlegrounds::Spells::DeserterBuff, m_Player);
        }

        l_Log << getMSTime() - l_StartTime << " ";

        // Repop at GraveYard or other player far teleport will prevent saving player because of not present map
        // Teleport player immediately for correct player save
        if (m_Player->IsBeingTeleportedFar())
        {
            WorldLocation const loc = m_Player->GetTeleportDest();
            m_Player->SetLocationMapId(loc.GetMapId());
            m_Player->Relocate(loc);
            m_Player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CHANGE_MAP | AURA_INTERRUPT_FLAG_MOVE | AURA_INTERRUPT_FLAG_TURNING);
            m_Player->SetSemaphoreTeleportFar(false);
        }

        l_Log << getMSTime() - l_StartTime << " ";

#ifndef CROSS
        ///- If the player is in a guild, update the guild roster and broadcast a logout message to other guild members
        if (Guild* guild = sGuildMgr->GetGuildById(m_Player->GetGuildId()))
            guild->HandleMemberLogout(this);
#endif /* not CROSS */

        l_Log << getMSTime() - l_StartTime << " ";

        m_Player->UnsummonCurrentBattlePetIfAny(true);

        l_Log << getMSTime() - l_StartTime << " ";

        ///- Remove pet
        if (m_Player->getClass() != CLASS_WARLOCK)
        {
            if (Pet* _pet = m_Player->GetPet())
                m_Player->RemovePet(_pet, PET_SLOT_ACTUAL_PET_SLOT, true, _pet->m_Stampeded);
            else
                m_Player->RemovePet(NULL, PET_SLOT_ACTUAL_PET_SLOT, true, true);
        }
        else
        {
            if (Pet* _pet = m_Player->GetPet())
                _pet->SavePetToDB(PET_SLOT_ACTUAL_PET_SLOT, _pet->m_Stampeded);
        }

        l_Log << getMSTime() - l_StartTime << " ";

        //! Call script hook before deletion
        sScriptMgr->OnPlayerLogout(m_Player);
        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventPlayerLogout, m_Player->GetRealGUID(), m_Player->GetName());

        l_Log << getMSTime() - l_StartTime << " ";

        ///- empty buyback items and save the player in the database
        // some save parts only correctly work in case player present in map/player_lists (pets, etc)
        if (p_Save)
        {
            uint32 eslot;
            for (int j = BUYBACK_SLOT_START; j < BUYBACK_SLOT_END; ++j)
            {
                eslot = j - BUYBACK_SLOT_START;
                m_Player->SetGuidValue(PLAYER_FIELD_INV_SLOTS + (BUYBACK_SLOT_START * 4) + (eslot * 4), 0);
                m_Player->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE + eslot, 0);
                m_Player->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP + eslot, 0);
            }
        }

        l_Log << getMSTime() - l_StartTime << " ";


#ifndef CROSS
        if (p_Save)
        {
            uint32 l_AccountID = GetAccountId();
            m_Player->SaveToDB(false, true, std::make_shared<MS::Utilities::Callback>([l_AccountID](bool p_Success) -> void
            {
                CharacterDatabase.PExecute("UPDATE characters SET online = 0 WHERE account = '%u'", l_AccountID);
            }));
        }

        l_Log << getMSTime() - l_StartTime << " ";

        InterRealmSession* tunnel = sWorld->GetInterRealmSession();
        if (tunnel && tunnel->IsTunnelOpened())
        {
            WorldPacket tunPacket(IR_CMSG_PLAYER_LOGOUT, 8);
            tunPacket << uint64(m_Player->GetGUID());
            tunnel->SendPacket(&tunPacket);
            m_InterRealmZoneId = 0;
        }
#else /* CROSS */
        m_Player->SaveToDB();
#endif
        ///- Leave all channels before player delete...
        if (!p_AfterInterRealm)
            m_Player->CleanupChannels();

        l_Log << getMSTime() - l_StartTime << " ";

        ///- If the player is in a group (or invited), remove him. If the group if then only 1 person, disband the group.
        m_Player->UninviteFromGroup();

        l_Log << getMSTime() - l_StartTime << " ";

#ifndef CROSS
        if (!p_AfterInterRealm)
#endif
        {
            //! Send update to group and reset stored max enchanting level
            if (m_Player->GetGroup())
            {
                m_Player->GetGroup()->SendUpdate();
                m_Player->GetGroup()->ResetMaxEnchantingLevel();
            }

            l_Log << getMSTime() - l_StartTime << " ";

#ifdef CROSS
            m_Player->SetNeedRemove(true);
#endif

            //! Broadcast a logout message to the player's friends
            sSocialMgr->SendFriendStatus(m_Player, FRIEND_OFFLINE, m_Player->GetGUIDLow(), true);
            sSocialMgr->RemovePlayerSocial(m_Player->GetGUIDLow());

            l_Log << getMSTime() - l_StartTime << " ";

            //! Call script hook before deletion
            sScriptMgr->OnPlayerLogout(m_Player);
        }

        /// Remove link & reference from the player groups
        if (GroupPtr l_Group = m_Player->GetGroup())
            l_Group->UnlinkPlayer(m_Player);

        if (GroupPtr l_Group = m_Player->GetOriginalGroup())
            l_Group->UnlinkPlayer(m_Player);

        l_Log << getMSTime() - l_StartTime << " ";

        //! Remove the player from the world
        // the player may not be in the world when logging out
        // e.g if he got disconnected during a transfer to another map
        // calls to GetMap in this case may cause crashes
        m_Player->CleanupsBeforeDelete();
        sLog->outInfo(LOG_FILTER_CHARACTER, "Account: %d (IP: %s) Logout Character:[%s] (GUID: %u) Level: %d", GetAccountId(), GetRemoteAddress().c_str(), m_Player->GetName(), m_Player->GetGUIDLow(), m_Player->getLevel());
        if (Map* _map = m_Player->GetMap())
            _map->RemovePlayerFromMap(m_Player, true);

        l_Log << getMSTime() - l_StartTime << " ";

        SetPlayer(NULL); //! Pointer already deleted during RemovePlayerFromMap


#ifndef CROSS
        if (!p_AfterInterRealm)
#endif
        {
            //! Send the 'logout complete' packet to the client
            //! Client will respond by sending 3x CMSG_CANCEL_TRADE, which we currently dont handle
            WorldPacket data(SMSG_LOGOUT_COMPLETE, 16);
            data.appendPackGUID(0);
            SendPacket(&data);
            sLog->outDebug(LOG_FILTER_NETWORKIO, "SESSION: Sent SMSG_LOGOUT_COMPLETE Message");
            //! Since each account can only have one online character at any given time, ensure all characters for active account are marked as offline
            CharacterDatabase.PExecute("UPDATE characters SET online = 0 WHERE account = '%u'", GetAccountId());
        }

        l_Log << getMSTime() - l_StartTime << " ";
    }

#ifndef CROSS
    if (!p_AfterInterRealm && m_Socket[CONNECTION_TYPE_INSTANCE])
    {
        m_Socket[CONNECTION_TYPE_INSTANCE]->CloseSocket();
        m_Socket[CONNECTION_TYPE_INSTANCE]->RemoveReference();
        m_Socket[CONNECTION_TYPE_INSTANCE] = nullptr;
    }
#endif

    m_playerLogout = false;
    m_playerSave = false;
    m_playerRecentlyLogout = true;
    LogoutRequest(0);

    sLog->outAshran(l_Log.str().c_str());
}

/// Kick a player out of the World
void WorldSession::KickPlayer()
{
#ifndef CROSS
    for (uint8 i = 0; i < MAX_CONNECTION_TYPES; ++i)
    {
        if (m_Socket[i])
        {
            m_Socket[i]->CloseSocket();
            SetforceExit(true);
        }
    }
#endif
}

void WorldSession::SendNotification(const char *format, ...)
{
    if (format)
    {
        va_list ap;
        char szStr[1024];
        szStr[0] = '\0';
        va_start(ap, format);
        vsnprintf(szStr, 1024, format, ap);
        va_end(ap);

        size_t len = strlen(szStr);
        WorldPacket l_Data(SMSG_PRINT_NOTIFICATION, 2 + len);
        l_Data.WriteBits(len, 12);
        l_Data.FlushBits();
        l_Data.append(szStr, len);
        SendPacket(&l_Data);
    }
}

void WorldSession::SendNotification(uint32 string_id, ...)
{
    char const* format = GetTrinityString(string_id);
    if (format)
    {
        va_list ap;
        char szStr[1024];
        szStr[0] = '\0';
        va_start(ap, string_id);
        vsnprintf(szStr, 1024, format, ap);
        va_end(ap);

        size_t len = strlen(szStr);
        WorldPacket l_Data(SMSG_PRINT_NOTIFICATION, 2 + len);
        l_Data.WriteBits(len, 12);
        l_Data.FlushBits();
        l_Data.append(szStr, len);
        SendPacket(&l_Data);
    }
}

const char *WorldSession::GetTrinityString(int32 entry) const
{
    return sObjectMgr->GetTrinityString(entry, GetSessionDbLocaleIndex());
}

void WorldSession::Handle_NULL(WorldPacket& recvPacket)
{
    sLog->outError(LOG_FILTER_OPCODES, "Received unhandled opcode %s from %s", GetOpcodeNameForLogging(recvPacket.GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), GetPlayerName(false).c_str());
}

void WorldSession::Handle_EarlyProccess(WorldPacket& recvPacket)
{
    sLog->outError(LOG_FILTER_OPCODES, "Received opcode %s that must be processed in WorldSocket::OnRead from %s", GetOpcodeNameForLogging(recvPacket.GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), GetPlayerName(false).c_str());
}

void WorldSession::Handle_ServerSide(WorldPacket& recvPacket)
{
    sLog->outError(LOG_FILTER_OPCODES, "Received server-side opcode %s from %s", GetOpcodeNameForLogging(recvPacket.GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), GetPlayerName(false).c_str());
}

void WorldSession::Handle_Deprecated(WorldPacket& recvPacket)
{
    sLog->outError(LOG_FILTER_OPCODES, "Received deprecated opcode %s from %s", GetOpcodeNameForLogging(recvPacket.GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), GetPlayerName(false).c_str());
}

void WorldSession::SendAuthWaitQue(uint32 position)
{
    WorldPacket l_Data;
    if (position == 0)
        WorldSession::WriteAuthResponse(l_Data, BattlenetRpcErrorCode::ERROR_OK, false);
    else
        WorldSession::WriteAuthResponse(l_Data, BattlenetRpcErrorCode::ERROR_OK, true, position);

    SendPacket(&l_Data);
}

void WorldSession::LoadGlobalAccountData()
{
    PreparedStatement* stmt = SessionRealmDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_DATA);
    stmt->setUInt32(0, GetAccountId());
    LoadAccountData(SessionRealmDatabase.Query(stmt), GLOBAL_CACHE_MASK);
}

void WorldSession::LoadAccountData(PreparedQueryResult result, uint32 mask)
{
    for (uint32 i = 0; i < NUM_ACCOUNT_DATA_TYPES; ++i)
        if (mask & (1 << i))
            m_accountData[i] = AccountData();

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        uint32 type = fields[0].GetUInt8();
        if (type >= NUM_ACCOUNT_DATA_TYPES)
        {
            sLog->outError(LOG_FILTER_GENERAL, "Table `%s` have invalid account data type (%u), ignore.",
                mask == GLOBAL_CACHE_MASK ? "account_data" : "character_account_data", type);
            continue;
        }

        if ((mask & (1 << type)) == 0)
        {
            sLog->outError(LOG_FILTER_GENERAL, "Table `%s` have non appropriate for table  account data type (%u), ignore.",
                mask == GLOBAL_CACHE_MASK ? "account_data" : "character_account_data", type);
            continue;
        }

        m_accountData[type].Time = time_t(fields[1].GetUInt32());
        m_accountData[type].Data = fields[2].GetString();
    }
    while (result->NextRow());
}

void WorldSession::SetAccountData(AccountDataType type, time_t tm, std::string data)
{
    uint32 id = 0;
    CharacterDatabaseStatements index;
    if ((1 << type) & GLOBAL_CACHE_MASK)
    {
        id = GetAccountId();
        index = CHAR_REP_ACCOUNT_DATA;
    }
    else
    {
        // _player can be NULL and packet received after logout but m_GUID still store correct guid
        if (!m_GUIDLow)
            return;

        id = m_GUIDLow;
        index = CHAR_REP_PLAYER_ACCOUNT_DATA;
    }

    PreparedStatement* stmt = SessionRealmDatabase.GetPreparedStatement(index);
    stmt->setUInt32(0, id);
    stmt->setUInt8 (1, type);
    stmt->setUInt32(2, uint32(tm));
    stmt->setString(3, data);
    SessionRealmDatabase.Execute(stmt);

    m_accountData[type].Time = tm;
    m_accountData[type].Data = data;
}

void WorldSession::SendAccountDataTimes(uint64 p_Guid)
{
    WorldPacket l_Data(SMSG_ACCOUNT_DATA_TIMES, 16 + 2 + 4 + (NUM_ACCOUNT_DATA_TYPES * 4));
    l_Data.appendPackGUID(p_Guid);
    l_Data << uint32(time(NULL));                                           ///< Server time

    for (uint32 l_I = 0; l_I < NUM_ACCOUNT_DATA_TYPES; ++l_I)
        l_Data << uint32(GetAccountData(AccountDataType(l_I))->Time);       ///< also unix time

    SendPacket(&l_Data);
}

void WorldSession::LoadTutorialsData()
{
    memset(m_Tutorials, 0, sizeof(uint32) * MAX_ACCOUNT_TUTORIAL_VALUES);

    PreparedStatement* stmt = SessionRealmDatabase.GetPreparedStatement(CHAR_SEL_TUTORIALS);
    stmt->setUInt32(0, GetAccountId());
    if (PreparedQueryResult result = SessionRealmDatabase.Query(stmt))
        for (uint8 i = 0; i < MAX_ACCOUNT_TUTORIAL_VALUES; ++i)
            m_Tutorials[i] = (*result)[i].GetUInt32();

    m_TutorialsChanged = false;
}

void WorldSession::LoadTutorialsData(PreparedQueryResult p_Result)
{
    memset(m_Tutorials, 0, sizeof(uint32) * MAX_ACCOUNT_TUTORIAL_VALUES);

    if (p_Result)
    {
        for (uint8 i = 0; i < MAX_ACCOUNT_TUTORIAL_VALUES; ++i)
            m_Tutorials[i] = (*p_Result)[i].GetUInt32();
    }

    m_TutorialsChanged = false;
}

void WorldSession::SendTutorialsData()
{
    WorldPacket data(SMSG_TUTORIAL_FLAGS, 4 * MAX_ACCOUNT_TUTORIAL_VALUES);
    for (uint8 i = 0; i < MAX_ACCOUNT_TUTORIAL_VALUES; ++i)
        data << m_Tutorials[i];
    SendPacket(&data);
}

void WorldSession::SaveTutorialsData(SQLTransaction &trans)
{
    if (!m_TutorialsChanged)
        return;

    // Modify data in DB
    PreparedStatement* l_Statement = SessionRealmDatabase.GetPreparedStatement(CHAR_INS_TUTORIALS);
    for (uint8 l_I = 0; l_I < MAX_ACCOUNT_TUTORIAL_VALUES; ++l_I)
        l_Statement->setUInt32(l_I, m_Tutorials[l_I]);
    l_Statement->setUInt32(MAX_ACCOUNT_TUTORIAL_VALUES, GetAccountId());
    trans->Append(l_Statement);

    m_TutorialsChanged = false;
}

void WorldSession::SendFeatureSystemStatus()
{
    bool l_EuropaTicketSystemEnabled            = true;
    bool l_PlayTimeAlert                        = false;
    bool l_ScrollOfResurrectionEnabled          = false;
    bool l_VoiceChatSystemEnabled               = false;
    bool l_ItemRestorationButtonEnbaled         = false;
    bool l_RecruitAFriendSystem                 = false;
    bool l_HasTravelPass                        = false;
#ifndef CROSS
    bool l_InGameBrowser                        = sBattlepayMgr->IsAvailable(this);;
#else /* CROSS */
    bool l_InGameBrowser                        = false;
#endif /* CROSS */
    bool l_StoreEnabled                         = true;
    bool l_StoreIsDisabledByParentalControls    = false;
#ifndef CROSS
    bool l_StoreIsAvailable                     = sBattlepayMgr->IsAvailable(this);
#else /* CROSS */
    bool l_StoreIsAvailable                     = false;
#endif /* CROSS */
    bool l_IsRestrictedAccount                  = false;
    bool l_IsTutorialEnabled                    = false;
    bool l_ShowNPETutorial                      = true;
    bool l_TwitterEnabled                       = true;
    bool l_CommerceSystemEnabled                = true;
    bool l_WillKickFromWorld                    = true;
    bool l_KioskModeEnabled                     = false;
    bool l_RaceClassExpansionLevels             = false;

    bool l_CompetitiveModeEnabled               = false;
    bool l_TokenBalanceEnabled                  = false;
    bool l_LiveRegionCharacterListEnabled       = false;
    bool l_LiveRegionCharacterCopyEnabled       = false;
    bool l_LiveRegionAccountCopyEnabled         = false;

    uint32 l_PlayTimeAlertDisplayAlertTime      = 0;
    uint32 l_PlayTimeAlertDisplayAlertDelay     = 0;
    uint32 l_PlayTimeAlertDisplayAlertPeriod    = 0;

    uint32 l_ScrollOfResurrectionRequestsRemaining = 1;
    uint32 l_ScrollOfResurrectionMaxRequestsPerDay = 1;

    uint32 l_ConfigRealmRecordID    = 640;
    uint32 l_ConfigRealmID          = g_RealmID;

    uint32 l_ComplainSystemStatus = 2;                              ///< 0 - Disabled | 1 - Calendar & Mail | 2 - Calendar & Mail & Ignoring system

    uint32 l_TwitterPostThrottleLimit       = 60;
    uint32 l_TwitterPostThrottleCooldown    = 20;
    uint32 l_TokenPollTimeSeconds           = 300;
    uint32 l_TokenRedeemIndex               = 0;
    int64  l_TokenBalanceAmount             = 0;
    uint32 l_BpayStoreProductDeliveryDelay  = 0;

    /// Added in 7.3.5 ,value in comment are from my sniff
    uint32 Unknown1 = 0; ///< 3
    uint32 Unknown2 = 0; ///< 2
    uint32 Unknown3 = 0; ///< 5
    uint32 Unknown4 = 0; ///< 7

    WorldPacket l_Data(SMSG_FEATURE_SYSTEM_STATUS, 100);

    l_Data << uint8(l_ComplainSystemStatus);                        ///< Complaints System Status
    l_Data << uint32(l_ScrollOfResurrectionMaxRequestsPerDay);      ///< Max SOR Per day
    l_Data << uint32(l_ScrollOfResurrectionRequestsRemaining);      ///< SOR remaining

    l_Data << uint32(l_ConfigRealmID);                              ///< Config Realm ID
    l_Data << uint32(l_ConfigRealmRecordID);                        ///< Config Realm Record ID (used for url db2 reading)

    l_Data << uint32(l_TwitterPostThrottleLimit);                   ///< Number of twitter posts the client can send before they start being throttled
    l_Data << uint32(l_TwitterPostThrottleCooldown);                ///< Time in seconds the client has to wait before posting again after hitting post limit

    l_Data << uint32(l_TokenPollTimeSeconds);                       ///< TokenPollTimeSeconds
    l_Data << uint32(l_TokenRedeemIndex);                           ///< TokenRedeemIndex
    l_Data << int64(l_TokenBalanceAmount);                          ///< TokenBalanceAmount
    l_Data << uint32(l_BpayStoreProductDeliveryDelay);              ///< BpayStoreProductDeliveryDelay

    l_Data << uint32(Unknown1);              ///< TODO : 7.3.5
    l_Data << uint32(Unknown2);              ///< TODO : 7.3.5
    l_Data << uint32(Unknown3);              ///< TODO : 7.3.5
    l_Data << uint32(Unknown4);              ///< TODO : 7.3.5

    l_Data.FlushBits();

    l_Data.WriteBit(l_VoiceChatSystemEnabled);                      ///< voice Chat System Status
    l_Data.WriteBit(l_EuropaTicketSystemEnabled);                   ///< Europa Ticket System Enabled
    l_Data.WriteBit(l_ScrollOfResurrectionEnabled);                 ///< Scroll Of Resurrection Enabled
    l_Data.WriteBit(l_StoreEnabled);                                ///< Store system status
    l_Data.WriteBit(l_StoreIsAvailable);                            ///< Can purchase in store
    l_Data.WriteBit(l_StoreIsDisabledByParentalControls);           ///< Is store disabled by parental controls
    l_Data.WriteBit(l_ItemRestorationButtonEnbaled);                ///< Item Restoration Button Enabled
    l_Data.WriteBit(l_InGameBrowser);                               ///< Web ticket system enabled
    l_Data.WriteBit(l_PlayTimeAlert);                               ///< Session Alert Enabled
    l_Data.WriteBit(l_RecruitAFriendSystem);                        ///< Recruit A Friend System Status
    l_Data.WriteBit(l_HasTravelPass);                               ///< Has travel pass (can group with cross-realm Battle.net friends.)
    l_Data.WriteBit(l_IsRestrictedAccount);                         ///< Is restricted account
    l_Data.WriteBit(l_IsTutorialEnabled);                           ///< Is tutorial system enabled
    l_Data.WriteBit(l_ShowNPETutorial);                             ///< Show NPE tutorial
    l_Data.WriteBit(l_TwitterEnabled);                              ///< Enable ingame twitter interface
    l_Data.WriteBit(l_CommerceSystemEnabled);                       ///< Commerce System Enabled (WoWToken)
    l_Data.WriteBit(1);                                             ///< Unk 6.1.2 19796
    l_Data.WriteBit(l_WillKickFromWorld);                           ///< WillKickFromWorld
    l_Data.WriteBit(l_KioskModeEnabled);                            ///< Disables half of your interface and options
    l_Data.WriteBit(l_CompetitiveModeEnabled);                      ///< CompetitiveModeEnabled
    l_Data.WriteBit(l_RaceClassExpansionLevels);                    ///< RaceClassExpansionLevels
    l_Data.WriteBit(l_TokenBalanceEnabled);                         ///< TokenBalanceEnabled
    l_Data.WriteBit(l_LiveRegionCharacterListEnabled);              ///< LiveRegionCharacterListEnabled
    l_Data.WriteBit(l_LiveRegionCharacterCopyEnabled);              ///< LiveRegionCharacterCopyEnabled
    l_Data.WriteBit(l_LiveRegionAccountCopyEnabled);                ///< LiveRegionAccountCopyEnabled

    l_Data.FlushBits();

    {
        l_Data.WriteBit(0);    ///< ToastsDisabled
        l_Data << float(0.0f); ///< ToastDuration
        l_Data << float(0.0f); ///< DelayDuration
        l_Data << float(0.0f); ///< QueueMultiplier
        l_Data << float(0.0f); ///< PlayerMultiplier
        l_Data << float(0.0f); ///< PlayerFriendValue
        l_Data << float(0.0f); ///< PlayerGuildValue
        l_Data << float(0.0f); ///< ThrottleInitialThreshold
        l_Data << float(0.0f); ///< ThrottleDecayTime
        l_Data << float(0.0f); ///< ThrottlePrioritySpike
        l_Data << float(0.0f); ///< ThrottleMinThreshold
        l_Data << float(0.0f); ///< ThrottlePvPPriorityNormal
        l_Data << float(0.0f); ///< ThrottlePvPPriorityLow
        l_Data << float(0.0f); ///< ThrottlePvPHonorThreshold
        l_Data << float(0.0f); ///< ThrottleLfgListPriorityDefault
        l_Data << float(0.0f); ///< ThrottleLfgListPriorityAbove
        l_Data << float(0.0f); ///< ThrottleLfgListPriorityBelow
        l_Data << float(0.0f); ///< ThrottleLfgListIlvlScalingAbove
        l_Data << float(0.0f); ///< ThrottleLfgListIlvlScalingBelow
        l_Data << float(0.0f); ///< ThrottleRfPriorityAbove
        l_Data << float(0.0f); ///< ThrottleRfIlvlScalingAbove
        l_Data << float(0.0f); ///< ThrottleDfMaxItemLevel
        l_Data << float(0.0f); ///< ThrottleDfBestPriority
    }

    if (l_PlayTimeAlert)
    {
        l_Data << uint32(l_PlayTimeAlertDisplayAlertDelay);         ///< Alert delay
        l_Data << uint32(l_PlayTimeAlertDisplayAlertPeriod);        ///< Alert period
        l_Data << uint32(l_PlayTimeAlertDisplayAlertTime);          ///< Alert display time
    }

    if (l_RaceClassExpansionLevels)
    {
        l_Data << uint32(0);
    }

    l_Data.FlushBits();

    if (l_EuropaTicketSystemEnabled)
    {
        l_Data.WriteBit(true);                                      ///< TicketsEnabled
        l_Data.WriteBit(true);                                      ///< BugsEnabled
        l_Data.WriteBit(true);                                      ///< ComplaintsEnabled
        l_Data.WriteBit(true);                                      ///< SuggestionsEnabled
        l_Data.FlushBits();

        l_Data << uint32(10);                                       ///< Max Tries
        l_Data << uint32(60000);                                    ///< Per Milliseconds
        l_Data << uint32(1);                                        ///< Try Count
        l_Data << uint32(0);                                        ///< Last Reset Time Before Now
    }

    SendPacket(&l_Data);
}

void WorldSession::SendTimeZoneInformations()
{
    const static std::string l_TimeZoneName = "Europe/Paris";

    WorldPacket l_Data(SMSG_SET_TIME_ZONE_INFORMATION, 26);
    l_Data.WriteBits(l_TimeZoneName.size(), 7);
    l_Data.WriteBits(l_TimeZoneName.size(), 7);
    l_Data.FlushBits();

    l_Data.WriteString(l_TimeZoneName);
    l_Data.WriteString(l_TimeZoneName);

    SendPacket(&l_Data);
}

bool WorldSession::IsAddonRegistered(const std::string& prefix) const
{
    if (!_filterAddonMessages) // if we have hit the softcap (64) nothing should be filtered
        return true;

    if (_registeredAddonPrefixes.empty())
        return false;

    std::vector<std::string>::const_iterator itr = std::find(_registeredAddonPrefixes.begin(), _registeredAddonPrefixes.end(), prefix);
    return itr != _registeredAddonPrefixes.end();
}

void WorldSession::HandleUnregisterAddonPrefixesOpcode(WorldPacket& /*recvPacket*/) // empty packet
{
    _registeredAddonPrefixes.clear();
}

void WorldSession::HandleAddonRegisteredPrefixesOpcode(WorldPacket& p_Packet)
{
    uint32 l_Count = 0;

    p_Packet >> l_Count;

    if (l_Count > REGISTERED_ADDON_PREFIX_SOFTCAP)
    {
        /// if we have hit the softcap (64) nothing should be filtered
        _filterAddonMessages = false;
        p_Packet.rfinish();
        return;
    }

    for (uint32 l_I = 0; l_I < l_Count; ++l_I)
    {
        uint8 l_Size = p_Packet.ReadBits(5);

        p_Packet.ResetBitReading();

        _registeredAddonPrefixes.push_back(p_Packet.ReadString(l_Size));
    }

    if (_registeredAddonPrefixes.size() > REGISTERED_ADDON_PREFIX_SOFTCAP) // shouldn't happen
    {
        _filterAddonMessages = false;
        return;
    }

    _filterAddonMessages = true;
}

void WorldSession::SetPlayer(Player* player)
{
    m_Player = player;

    // set m_GUID that can be used while player loggined and later until m_playerRecentlyLogout not reset
    if (m_Player)
    {
        m_GUIDLow = m_Player->GetGUIDLow();
#ifdef CROSS
        m_GUID = m_Player->GetGUID();
        m_RealGUID = m_Player->GetRealGUID();
#endif
    }
}

void WorldSession::InitializeQueryCallbackParameters()
{
    // Callback parameters that have pointers in them should be properly
    // initialized to NULL here.
    _charCreateCallback.SetParam(NULL);
}

void WorldSession::ProcessQueryCallbacks()
{
    /// Only process thread-safe callbacks here
    PreparedQueryResult l_Result;

# ifndef CROSS
    //! HandleCharEnumOpcode
    if (m_CharEnumCallback.ready())
    {
        m_CharEnumCallback.get(l_Result);
        HandleCharEnum(l_Result);
        m_CharEnumCallback.cancel();
    }

    if (_charCreateCallback.IsReady())
    {
        _charCreateCallback.GetResult(l_Result);
        HandleCharCreateCallback(l_Result, _charCreateCallback.GetParam());
        // Don't call FreeResult() here, the callback handler will do that depending on the events in the callback chain
    }

    //- HandleCharRenameOpcode
    if (_charRenameCallback.IsReady())
    {
        std::string param = _charRenameCallback.GetParam();
        _charRenameCallback.GetResult(l_Result);
        HandleChangePlayerNameOpcodeCallBack(l_Result, param);
        _charRenameCallback.FreeResult();
    }
# endif

    /// Process callbacks that need to be executed sync in World::UpdateSessions
    if (m_IsInWorldUpdateSession)
    {
        //! HandlePlayerLoginOpcode
        if (m_CharacterLoginCallback.ready() && m_CharacterLoginDBCallback.ready())
        {
            SQLQueryHolder* l_Param;
            SQLQueryHolder* l_Param2;
            m_CharacterLoginCallback.get(l_Param);
            m_CharacterLoginDBCallback.get(l_Param2);
#ifndef CROSS
            HandlePlayerLogin((LoginQueryHolder*)l_Param, (LoginDBQueryHolder*)l_Param2);

#else /* CROSS */
            LoadCharacterDone((LoginQueryHolder*)l_Param, (LoginDBQueryHolder*)l_Param2);
#endif
            m_CharacterLoginCallback.cancel();
            m_CharacterLoginDBCallback.cancel();
        }
    }

    /// Process safe callback that can be handled in map only in Map::Update to reduce World::UpdateSessions load if the player is online
    bool l_CanProcessMapSafeCallback = !m_IsInWorldUpdateSession || !m_Player;

    PreparedQueryResult result;

    uint32 l_Time = getMSTime();
    std::vector<uint32> l_Measures;

#ifndef CROSS

    l_Measures.push_back(getMSTime() - l_Time);

    //! HandleAddFriendOpcode
    if (_addFriendCallback.IsReady() && l_CanProcessMapSafeCallback)
    {
        std::string param = _addFriendCallback.GetParam();
        _addFriendCallback.GetResult(result);
        HandleAddFriendOpcodeCallBack(result, param);
        _addFriendCallback.FreeResult();
    }

    l_Measures.push_back(getMSTime() - l_Time);

    //- HandleCharAddIgnoreOpcode
    if (m_AddIgnoreCallback.ready() && l_CanProcessMapSafeCallback)
    {
        m_AddIgnoreCallback.get(result);
        HandleAddIgnoreOpcodeCallBack(result);
        m_AddIgnoreCallback.cancel();
    }

    l_Measures.push_back(getMSTime() - l_Time);

#endif

    l_Measures.push_back(getMSTime() - l_Time);

    //- SendStabledPet
    if (_sendStabledPetCallback.IsReady() && l_CanProcessMapSafeCallback)
    {
        uint64 param = _sendStabledPetCallback.GetParam();
        _sendStabledPetCallback.GetResult(result);
        SendStablePetCallback(result, param);
        _sendStabledPetCallback.FreeResult();
    }

    l_Measures.push_back(getMSTime() - l_Time);

    //- HandleStableSwapPet
    if (_setPetSlotCallback.IsReady() && l_CanProcessMapSafeCallback)
    {
        uint32 param = _setPetSlotCallback.GetParam();
        _setPetSlotCallback.GetResult(result);
        HandleStableSetPetSlotCallback(result, param);
        _setPetSlotCallback.FreeResult();
    }

    l_Measures.push_back(getMSTime() - l_Time);

    if ((getMSTime() - l_Time) > 10)
    {
        std::ostringstream l_Log;
        l_Log << "WorldSession::ProcessQueryCallbacks [" << GetAccountId() << "] took " << getMSTime() - l_Time << " ms ( ";
        for (auto l_Itr : l_Measures)
            l_Log << l_Itr << " ";
        l_Log << ")";

        sLog->outAshran(l_Log.str().c_str());
    }
}

void WorldSession::InitWarden(BigNumber* k, std::string os)
{
    if (os == "Win")
    {
        _warden = new WardenWin();
        _warden->Init(this, k);
    }
    else if (os == "OSX")
    {
        // Disabled as it is causing the client to crash
        // _warden = new WardenMac();
        // _warden->Init(this, k);
    }
}

void WorldSession::SetServiceFlags(uint32 p_Flags)
{
    m_ServiceFlags |= p_Flags;

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LoginDatabaseStatements::LOGIN_SET_ACCOUNT_SERVICE);
    l_Statement->setUInt32(0, p_Flags);
    l_Statement->setUInt32(1, GetAccountId());
    LoginDatabase.AsyncQuery(l_Statement);
}

void WorldSession::UnsetServiceFlags(uint32 p_Flags)
{
    m_ServiceFlags &= ~p_Flags;

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LoginDatabaseStatements::LOGIN_REMOVE_ACCOUNT_SERVICE);
    l_Statement->setUInt32(0, p_Flags);
    l_Statement->setUInt32(1, GetAccountId());
    LoginDatabase.AsyncQuery(l_Statement);
}

void WorldSession::SetCustomFlags(uint32 p_Flags)
{
    m_CustomFlags |= p_Flags;

    LoginDatabase.AsyncPQuery("UPDATE account SET custom_flags = custom_flags | %u WHERE id = %u", p_Flags, GetAccountId());
}

void WorldSession::UnsetCustomFlags(uint32 p_Flags)
{
    m_CustomFlags &= ~p_Flags;

    LoginDatabase.AsyncPQuery("UPDATE account SET custom_flags = custom_flags &~ %u WHERE id = %u", p_Flags, GetAccountId());
}

#ifndef CROSS
void WorldSession::LoadPremades()
{
    PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PREMADES);
    l_Statement->setUInt32(0, GetAccountId());

    PreparedQueryResult l_Result = CharacterDatabase.Query(l_Statement);
    if (!l_Result)
        return;

    uint32 l_CharactersCount = AccountMgr::GetCharactersCount(GetAccountId());
    if (l_CharactersCount >= sWorld->getIntConfig(CONFIG_CHARACTERS_PER_REALM))
        return;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_Transaction = l_Fields[0].GetUInt32();
        uint32 l_TemplateID = l_Fields[1].GetUInt32();
        uint32 l_Faction = l_Fields[2].GetUInt8();
        uint8  l_Type = l_Fields[3].GetUInt8();

        CharacterTemplate const* l_Template = sObjectMgr->GetCharacterTemplate(l_TemplateID);
        if (!l_Template)
            continue;

        WorldPacket l_Data; ///< Dummy
        CharacterCreateInfo* l_CreateInfo = new CharacterCreateInfo("#Premade", l_Faction == TEAM_HORDE ? l_Template->m_HordeDefaultRace : l_Template->m_AllianceDefaultRace, l_Template->m_PlayerClass, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, l_Data);

        Player l_NewCharacter(this);
        l_NewCharacter.GetMotionMaster()->Initialize();

        if (!l_NewCharacter.Create(sObjectMgr->GenerateLowGuid(HIGHGUID_PLAYER), l_CreateInfo))
        {
            l_NewCharacter.CleanupsBeforeDelete();
            delete l_CreateInfo;
            continue;
        }

        l_NewCharacter.SetAtLoginFlag(AT_LOGIN_FIRST);
        l_NewCharacter.SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);

        l_NewCharacter.SetLevel(l_Template->m_Level);
        l_NewCharacter.SetMoney(l_Template->m_Money);

        for (auto& l_ReputationInfo : l_Template->m_TemplateFactions)
        {
            if (FactionEntry const* l_Faction = sFactionStore.LookupEntry(l_ReputationInfo.m_FactionID))
                l_NewCharacter.GetReputationMgr().SetReputation(l_Faction, l_ReputationInfo.m_Reputaion);
        }

        for (auto l_Spell : l_Template->m_SpellIDs)
            l_NewCharacter.learnSpell(l_Spell, false);

        std::list<CharacterTemplate::TemplateItem const*> l_RemainingTemplates;
        for (auto& l_Item : l_Template->m_TemplateItems)
        {
            if (ItemTemplate const* l_Proto = sObjectMgr->GetItemTemplate(l_Item.m_ItemID))
            {
                // Give bags first to the players, then the equipment
                if (l_Proto->Class == ITEM_CLASS_CONTAINER)
                {
                    if (!l_Item.m_Faction || (l_Item.m_Faction == 1 && l_NewCharacter.GetTeam() == ALLIANCE) || (l_Item.m_Faction == 2 && l_NewCharacter.GetTeam() == HORDE))
                        l_NewCharacter.StoreNewItemInBestSlots(l_Item.m_ItemID, l_Item.m_Count);

                    continue;
                }
            }
            l_RemainingTemplates.push_back(&l_Item);
        }

        for (auto l_Item : l_RemainingTemplates)
        {
            if (ItemTemplate const* l_Proto = sObjectMgr->GetItemTemplate(l_Item->m_ItemID))
            {
                if ((l_Proto->AllowableRace & l_NewCharacter.getRaceMask()) == 0)
                    continue;

                if (!l_Item->m_Faction || (l_Item->m_Faction == 1 && l_NewCharacter.GetTeam() == ALLIANCE) || (l_Item->m_Faction == 2 && l_NewCharacter.GetTeam() == HORDE))
                {
                    ItemContext l_Context = ItemContext::None;

                    /// Pve premade have LFR items
                    if (l_Type == 1)
                        l_Context = ItemContext::RaidLfr;

                    if (l_Item->m_Type == 0 || l_Type == l_Item->m_Type)
                        l_NewCharacter.StoreNewItemInBestSlots(l_Item->m_ItemID, l_Item->m_Count, l_Context);
                }
            }
        }

        uint32 l_AccountID = GetAccountId();

        l_NewCharacter.SaveToDB(true, false,std::make_shared<MS::Utilities::Callback>([l_AccountID](bool p_Success) -> void
        {
            WorldSession* l_Session = sWorld->FindSession(l_AccountID);
            if (l_Session == nullptr)
                return;

            WorldPacket l_Data(SMSG_CREATE_CHAR, 1);
            l_Data << uint8(p_Success ? CHAR_CREATE_SUCCESS : CHAR_CREATE_ERROR);
            l_Session->SendPacket(&l_Data);
        }));

        if (l_CreateInfo->Class == CLASS_HUNTER)
        {
            uint32 pet_id = sObjectMgr->GeneratePetNumber();

            switch (l_CreateInfo->Race)
            {
                case RACE_HUMAN:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 299, %u, 903, 13481, 1, 1, 0, 0, 'Wolf', 0, 0, 192, 0, 1295727347, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_DWARF:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42713, %u, 822, 13481, 1, 1, 0, 0, 'Bear', 0, 0, 212, 0, 1295727650, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_ORC:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42719, %u, 744, 13481, 1, 1, 0, 0, 'Boar', 0, 0, 212, 0, 1295727175, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_NIGHTELF:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42718, %u,  17090, 13481, 1, 1, 0, 0, 'Cat', 0, 0, 192, 0, 1295727501, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_UNDEAD_PLAYER:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 51107, %u,  368, 13481, 1, 1, 0, 0, 'Spider', 0, 0, 202, 0, 1295727821, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_TAUREN:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42720, %u,  29057, 13481, 1, 1, 0, 0, 'Tallstrider', 0, 0, 192, 0, 1295727912, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_TROLL:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42721, %u,  23518, 13481, 1, 1, 0, 0, 'Raptor', 0, 0, 192, 0, 1295727987, '7 2 7 1 7 0 129 2649 129 50498 129 16827 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_GOBLIN:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42715, %u, 27692, 13481, 1, 1, 0, 0, 'Crab', 0, 0, 212, 0, 1295720595, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_BLOODELF:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42710, %u, 23515, 13481, 1, 1, 0, 0, 'Dragonhawk', 0, 0, 202, 0, 1295728068, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_DRAENEI:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42712, %u, 29056, 13481, 1, 1, 0, 0, 'Moth', 0, 0, 192, 0, 1295728128, '7 2 7 1 7 0 129 2649 129 49966 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_WORGEN:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 42722, %u, 30221, 13481, 1, 1, 0, 0, 'Dog', 0, 0, 192, 0, 1295728219, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_PANDAREN_NEUTRAL:
                    CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 57239, %u, 42656, 13481, 1, 1, 0, 0, 'Turtle', 0, 0, 192, 0, 1295728219, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                /// Todo : handle new subraces ?
                case RACE_HIGHMOUNTAIN_TAUREN:
                    ///CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 57239, %u, 42656, 13481, 1, 1, 0, 0, 'Turtle', 0, 0, 192, 0, 1295728219, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_NIGHTBORNE:
                    ///CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 57239, %u, 42656, 13481, 1, 1, 0, 0, 'Turtle', 0, 0, 192, 0, 1295728219, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_VOID_ELF:
                    ///CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 57239, %u, 42656, 13481, 1, 1, 0, 0, 'Turtle', 0, 0, 192, 0, 1295728219, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                case RACE_LIGHTFORGED_DRAENEI:
                    ///CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `abdata`, `specialization`) VALUES (%u, 57239, %u, 42656, 13481, 1, 1, 0, 0, 'Turtle', 0, 0, 192, 0, 1295728219, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ', 0)", pet_id, l_NewCharacter.GetGUIDLow());
                    break;
                default:
                    break;
            }

            CharacterDatabase.PExecute("UPDATE characters SET currentPetSlot = '0', petSlotUsed = '1' WHERE guid = %u", l_NewCharacter.GetGUIDLow());
            l_NewCharacter.SetTemporaryUnsummonedPetNumber(pet_id);
        }

        l_NewCharacter.CleanupsBeforeDelete();
        delete l_CreateInfo;

        l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PREMADE_SUCESS);
        l_Statement->setUInt32(0, l_Transaction);
        CharacterDatabase.Execute(l_Statement);
    }
    while (l_Result->NextRow());
}
#endif

void WorldSession::LoadCollections()
{
    PreparedStatement* l_StatementA = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WARDROBES);
    l_StatementA->setUInt32(0, GetAccountId());

    PreparedStatement* l_StatementB = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WARDROBE_FAVORITES);
    l_StatementB->setUInt32(0, GetAccountId());

    _collectionMgr->LoadAccountItemAppearances(LoginDatabase.Query(l_StatementA), LoginDatabase.Query(l_StatementB));
}

#ifdef CROSS
void WorldSession::LoadCharacter(std::function<void()> p_Callback)
{
    InterRealmClient* l_Client = GetInterRealmClient();
    if (!l_Client)
        return;

    InterRealmDatabasePool* l_RealmDatabase = l_Client->GetDatabase();

    LoginQueryHolder*   l_Holer              = new LoginQueryHolder(GetAccountId(), m_RealGUID, GetInterRealmNumber(), p_Callback);
    LoginDBQueryHolder* l_LoginDBQueryHolder = new LoginDBQueryHolder(GetAccountId());

    if (!l_Holer->Initialize() || !l_LoginDBQueryHolder->Initialize())
    {
        delete l_Holer;
        delete l_LoginDBQueryHolder;

        m_playerLoading = 0;

        sLog->outInfo(LOG_FILTER_WORLDSERVER, "Cannot initialize query holder.");
        return;
    }

    printf("IR_CMSG_BATTLEFIELD_PORT: LoadCharacter initialized ...\r\n");

    m_CharacterLoginCallback = l_RealmDatabase->DelayQueryHolder((SQLQueryHolder*)l_Holer);
    m_CharacterLoginDBCallback = LoginDatabase.DelayQueryHolder((SQLQueryHolder*)l_LoginDBQueryHolder);
}

void WorldSession::LoadCharacterDone(LoginQueryHolder* p_CharHolder, LoginDBQueryHolder* p_AuthHolder)
{
    printf("WorldSession::LoadCharacterDone: begin ...\r\n");

    if (!p_CharHolder || !p_AuthHolder)
    {
        sLog->outInfo(LOG_FILTER_WORLDSERVER, "There is no query holder in WorldSession::LoadCharacterDone.");
        return;
    }

    if (!GetPlayer())
    {
        delete p_CharHolder;
        delete p_AuthHolder;

        sLog->outInfo(LOG_FILTER_WORLDSERVER, "There is no player in WorldSession::LoadCharacterDone.");

        return;
    }

    SetPlayerLoading(true);

    uint64 l_PlayerGuid = p_CharHolder->GetGuid();

    if (!GetPlayer()->LoadFromDB(GUID_LOPART(l_PlayerGuid), p_CharHolder, p_AuthHolder))
    {
        delete p_CharHolder;
        delete p_AuthHolder;

        SetPlayerLoading(false);

        sLog->outInfo(LOG_FILTER_INTERREALM, "Cannot load player in WorldSession::LoadCharacterDone.");

        return;
    }

    printf("WorldSession::LoadCharacterDone: done\r\n");
}
#endif

void WorldSession::LoadLoyaltyData()
{
#ifndef CROSS
    if (!sWorld->getBoolConfig(CONFIG_WEB_DATABASE_ENABLE))
        return;

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACTIVITY);
    l_Statement->setUInt32(0, GetAccountId());
    LoginDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this](PreparedQueryResult p_Result)->void
    {
        if (!p_Result)
            return;

        do
        {
            Field* l_Fields = p_Result->Fetch();
            uint64 l_Days = l_Fields[1].GetUInt64();

            /// Loyalty points are based on week of activity, a week of activity is at least 3 day of activity in a single week or 7 days of activity
            m_ActivityDays += l_Days > 2 ? 7 : l_Days;
        } while (p_Result->NextRow());
    });

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_LAST_BANNED_DATE);
    l_Statement->setUInt32(0, GetAccountId());
    LoginDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this](PreparedQueryResult p_Result)->void
    {
        if (p_Result)
            m_LastBan = p_Result->Fetch()[0].GetUInt32();
    });

    l_Statement = WebDatabase.GetPreparedStatement(WEB_SEL_ACC_VALIDATE);
    l_Statement->setUInt32(0, GetAccountId());
    WebDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this](PreparedQueryResult p_Result) -> void
    {
        if (p_Result)
            m_EmailValidated = p_Result->Fetch()[0].GetBool();
    });

    l_Statement = WebDatabase.GetPreparedStatement(WEB_SEL_POINTS_PURCHASE);
    l_Statement->setUInt32(0, GetAccountId());
    WebDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this](PreparedQueryResult p_Result) -> void
    {
        if (p_Result)
            m_AlreadyPurchasePoints = true;
    });

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACC_LOYALTY);
    l_Statement->setUInt32(0, GetAccountId());

    LoginDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this](PreparedQueryResult p_Result)
    {
        PreparedStatement* l_Statement = nullptr;
        if (p_Result)
        {
            Field* l_Fields = p_Result->Fetch();

            m_LastClaim      = l_Fields[0].GetUInt32();
            m_LastEventReset = l_Fields[1].GetUInt32();

            time_t l_NowTime = time(nullptr);

            auto l_Time = localtime(&l_NowTime);
            struct tm l_Now = *l_Time;
            auto l_LastReset = localtime(&m_LastEventReset);

            /// If now is a different day than the last event reset day, then clear event history
            if (l_LastReset->tm_year != l_Now.tm_year || l_LastReset->tm_mday != l_Now.tm_mday || l_LastReset->tm_mon != l_Now.tm_mon)
            {
                m_LastEventReset = l_NowTime;

                l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACC_LOYALTY_EVENT);
                l_Statement->setUInt32(0, GetAccountId());
                LoginDatabase.Execute(l_Statement);

                l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACC_LOYALTY);
                l_Statement->setUInt32(0, GetAccountId());
                l_Statement->setUInt32(1, m_LastClaim);
                l_Statement->setUInt32(2, m_LastEventReset);
                LoginDatabase.Execute(l_Statement);
            }
            /// Load event history of the day
            else
            {

                l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACC_LOYALTY_EVENT);
                l_Statement->setUInt32(0, GetAccountId());

                LoginDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this](PreparedQueryResult p_Result)
                {
                    if (!p_Result)
                        return;

                    do
                    {
                        Field* l_Fields = p_Result->Fetch();
                        uint32 l_Event = l_Fields[0].GetUInt32();
                        uint32 l_Count = l_Fields[1].GetUInt32();

                        /// Corrupted data ?
                        if (l_Event >= (uint32)LoyaltyEvent::Max)
                            continue;

                        m_LoyaltyEvents[l_Event] = l_Count;
                    } while (p_Result->NextRow());
                });
            }
        }
        else
        {
            /// Insert default row
            m_LastEventReset = time(nullptr);

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACC_LOYALTY);
            l_Statement->setUInt32(0, GetAccountId());
            l_Statement->setUInt32(1, 0);
            l_Statement->setUInt32(2, m_LastEventReset);
            LoginDatabase.Execute(l_Statement);
        }
    });
#endif
}

void WorldSession::AddLoyaltyPoints(uint32 p_Count, std::string p_Reason)
{
    PreparedStatement* l_Statement = WebDatabase.GetPreparedStatement(WEB_INS_LOYALTY_POINTS);
    l_Statement->setUInt32(0, GetAccountId());
    l_Statement->setUInt32(1, p_Count);
    l_Statement->setString(2, p_Reason);
    WebDatabase.Execute(l_Statement);

    ChatHandler(this).PSendSysMessage(TrinityStrings::LoyaltyPointEarn, p_Count);
}

void WorldSession::AddFirestormPoints(uint32 p_Count, std::string p_Reason)
{
    PreparedStatement* l_Statement = WebDatabase.GetPreparedStatement(WEB_INS_FIRESTORM_POINTS);
    l_Statement->setUInt32(0, GetAccountId());
    l_Statement->setUInt32(1, p_Count);
    l_Statement->setString(2, p_Reason);
    WebDatabase.Execute(l_Statement);

    ChatHandler(this).PSendSysMessage(TrinityStrings::FirestormPointEarn, p_Count);
}

void WorldSession::FetchFirestormPoints(std::function<void(PreparedQueryResult)> p_Callback)
{
    if (!m_Player)
        return;

    PreparedStatement* l_Statement = WebDatabase.GetPreparedStatement(WEB_SEL_ACCOUNT_POINTS);
    l_Statement->setUInt32(0, GetAccountId());
    l_Statement->setUInt32(1, GetAccountId());
    WebDatabase.AsyncQuery(l_Statement, m_Player->GetQueryCallbackMgr(), p_Callback);
}

void WorldSession::SaveCosmeticPurchase(CosmeticEntry p_CosmeticEntry)
{
    std::map<LocaleConstant, std::string> l_LocaleData;
    l_LocaleData[LocaleConstant::LOCALE_enUS] = p_CosmeticEntry.Locale_enUS;
    l_LocaleData[LocaleConstant::LOCALE_frFR] = p_CosmeticEntry.Locale_frFR;
    l_LocaleData[LocaleConstant::LOCALE_esES] = p_CosmeticEntry.Locale_esES;
    l_LocaleData[LocaleConstant::LOCALE_ruRU] = p_CosmeticEntry.Locale_ruRU;
    l_LocaleData[LocaleConstant::LOCALE_itIT] = p_CosmeticEntry.Locale_itIT;
    l_LocaleData[LocaleConstant::LOCALE_ptPT] = p_CosmeticEntry.Locale_ptPT;

    Battlepay::Product l_Product;
    l_Product.WebsiteType = static_cast<uint8>(Battlepay::WebsiteType::Cosmetic);
    l_Product.SetSpecialCustomData(p_CosmeticEntry.Type, p_CosmeticEntry.ID);
    l_Product.SetSpecialLocaleData(l_LocaleData);

    PreparedStatement* l_Statement = WebDatabase.GetPreparedStatement(WEB_INS_PURCHASE);
    l_Statement->setUInt32(0, GetAccountId());
    l_Statement->setUInt32(1, Expansion::EXPANSION_LEGION);
    l_Statement->setUInt32(2, sLog->GetRealmID());
    l_Statement->setUInt32(3, GetPlayer() ? GetPlayer()->GetGUIDLow() : 0);
    l_Statement->setString(4, l_Product.Serialize());
    l_Statement->setUInt32(5, 1);
    l_Statement->setUInt32(6, p_CosmeticEntry.Cost);
    l_Statement->setString(7, GetRemoteAddress());

    WebDatabase.Execute(l_Statement);
}

void WorldSession::SaveCosmeticLootboxPurchase(uint32 p_Amount, uint32 p_Cost)
{
    Battlepay::Product l_Product;
    l_Product.WebsiteType = static_cast<uint8>(Battlepay::WebsiteType::Cosmetic);

    PreparedStatement* l_Statement = WebDatabase.GetPreparedStatement(WEB_INS_PURCHASE);
    l_Statement->setUInt32(0, GetAccountId());
    l_Statement->setUInt32(1, Expansion::EXPANSION_LEGION);
    l_Statement->setUInt32(2, sLog->GetRealmID());
    l_Statement->setUInt32(3, GetPlayer() ? GetPlayer()->GetGUIDLow() : 0);

    std::stringstream l_ShopEntryData;
    BuildLootBoxShopEntryData(l_ShopEntryData, p_Amount);
    l_Statement->setString(4, l_ShopEntryData.str());

    l_Statement->setUInt32(5, 1);
    l_Statement->setUInt32(6, p_Cost);
    l_Statement->setString(7, GetRemoteAddress());

    WebDatabase.Execute(l_Statement);
}

void WorldSession::BuildLootBoxShopEntryData(std::stringstream& l_ShopEntryData, uint32 p_Amount)
{
    std::map<uint32, std::pair<uint32, uint32>> l_Datas =
    {
        { 1, {228177, 30   } },
        { 6, {228178, 150  } },
        { 13,{228179, 300  } },
        { 28,{228180, 600  } },
        { 60,{228181, 1200 } }
    };

    if (l_Datas.find(p_Amount) == l_Datas.end())
        return;

    l_ShopEntryData << "{\n  \"ID\": \" " << l_Datas[p_Amount].first << "\",\n";
    l_ShopEntryData << "  \"Parent\": \"228152\",\n";
    l_ShopEntryData << "  \"Expansion\": \"6\",\n";
    l_ShopEntryData << "  \"RealmIDMask\": \"" << (sLog->GetRealmID() << 1) << "\",\n";
    l_ShopEntryData << "  \"Type\": \"36\",\n";
    l_ShopEntryData << "  \"TypeEntryOrData\": \"310000\",\n";
    l_ShopEntryData << "  \"Quantity\": \"" << p_Amount << "\",\n";
    l_ShopEntryData << "  \"Flags\": \"9\",\n";
    l_ShopEntryData << "  \"CssData\": \"\",\n";
    l_ShopEntryData << "  \"CustomData\": \"\",\n";
    l_ShopEntryData << "  \"Price\": \"" << l_Datas[p_Amount].second << "\",\n";
    l_ShopEntryData << "  \"PriceAlt\": \" - 1\",\n";
    l_ShopEntryData << "  \"PromotionPrice\": \" - 1\",\n";
    l_ShopEntryData << "  \"PromotionPriceAlt\": \" - 1\",\n";
    l_ShopEntryData << "  \"TimePromotionBegin\": -62135604000,\n";
    l_ShopEntryData << "  \"TimePromotionEnd\": -62135604000,\n";
    l_ShopEntryData << "  \"HasTimeLimit\": \"0\",\n";
    l_ShopEntryData << "  \"ItemBonus\": \"0\",\n";
    l_ShopEntryData << "  \"RelOrder\": \"1\",\n";
    l_ShopEntryData << "  \"SellCount\": \"0\",\n";
    l_ShopEntryData << "  \"Name\": {\n";
    std::set<uint32> l_LocalesID = { 0, 1, 3, 4, 5, 7 };
    uint32 l_Itr = 0;
    for (uint32 l_LocaleID : l_LocalesID)
    {
        l_ShopEntryData << "    \"" << l_LocaleID << "\": {\n";
        l_ShopEntryData << "      \"Value\": \"Loot box(Cosmetic)\",\n";
        l_ShopEntryData << "      \"Locale\": "<< l_LocaleID <<"\n";
        if (++l_Itr < l_LocalesID.size())
            l_ShopEntryData << "    },\n";
        else
            l_ShopEntryData << "    }\n";
    }
    l_ShopEntryData << "  }\n";
    l_ShopEntryData << "}";
}

void WorldSession::CompleteLoyaltyEvent(LoyaltyEvent p_Event)
{
    m_LoyaltyEvents[(uint32)p_Event]++;

    if (m_LoyaltyEvents[(uint32)p_Event] == g_LoyaltyEventObjectives[(uint32)p_Event])
    {
        uint32 l_RewardPoints = g_LoyaltyEventReward[(uint32)p_Event];

        /// loyalty event can give you max 7 points per day
        int32 l_AlreadyEarnPoints = 0;
        for (uint8 l_I = 0; l_I < (uint8)LoyaltyEvent::Max; l_I++)
        {
            if (m_LoyaltyEvents[l_I] == g_LoyaltyEventObjectives[l_I])
                l_AlreadyEarnPoints += g_LoyaltyEventReward[l_I];
        }

        if ((l_AlreadyEarnPoints + l_RewardPoints) > 7)
            l_RewardPoints = std::max(7 - l_AlreadyEarnPoints, 0);

        if (!l_RewardPoints)
            return;

        AddLoyaltyPoints(l_RewardPoints, "Loyalty Event " + std::to_string((uint32)p_Event));
    }

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACC_LOYALTY_EVENT);
    l_Statement->setUInt32(0, GetAccountId());
    l_Statement->setUInt32(1, (uint32)p_Event);
    l_Statement->setUInt32(2, m_LoyaltyEvents[(uint32)p_Event]);
    LoginDatabase.Execute(l_Statement);
}

/// Send a game error
/// @p_Error : Game error
/// @p_Data1 : Additional data 1
/// @p_Data2 : Additional data 2
void WorldSession::SendGameError(GameError::Type p_Error, uint32 p_Data1, uint32 p_Data2)
{
    WorldPacket l_Packet(SMSG_DISPLAY_GAME_ERROR, 13);
    l_Packet << uint32(p_Error);
    l_Packet.WriteBit(p_Data1 != 0xF0F0F0F0);
    l_Packet.WriteBit(p_Data2 != 0xF0F0F0F0);
    l_Packet.FlushBits();

    if (p_Data1 != 0xF0F0F0F0)
        l_Packet << uint32(p_Data1);

    if (p_Data2 != 0xF0F0F0F0)
        l_Packet << uint32(p_Data2);

    SendPacket(&l_Packet);
}

#ifndef CROSS
void WorldSession::SaveChannels()
{
    if (!m_Player)
        return;

    JoinedChannelsList const& l_Channels = m_Player->GetJoinnedChannelList();
    for (auto l_Channel : l_Channels)
    {
        m_ChannelsSave.push_back(l_Channel);
        l_Channel->Leave(m_Player->GetGUID(), false);
    }
}

void WorldSession::RestoreChannels()
{
    if (!m_Player)
    {
        m_ChannelsSave.clear();
        return;
    }

    for (Channel* l_Channel : m_ChannelsSave)
        l_Channel->Join(m_Player->GetGUID(), l_Channel->GetPassword().c_str());

    m_ChannelsSave.clear();
}
#endif

void WorldSession::SendConnectToInstance(WorldPackets::Auth::ConnectToSerial serial)
{
    m_CanResumCommms = true;

    _instanceConnectKey.Fields.AccountId = GetAccountId();
    _instanceConnectKey.Fields.ConnectionType = CONNECTION_TYPE_INSTANCE;
    _instanceConnectKey.Fields.Key = urand(0, 0x7FFFFFFF);

    WorldPackets::Auth::ConnectTo connectTo;
    connectTo.Key = _instanceConnectKey.Raw;
    connectTo.Serial = serial;
    connectTo.Payload.Address = m_Trusted ? sWorld->GetInstanceAddress() : sWorld->GetInstanceProxyAddress();
    connectTo.Con = CONNECTION_TYPE_INSTANCE;

    SendPacket(connectTo.Write());
}

void WorldSession::SetChangelogReceived()
{
    m_ChangelogReceived = true;
}

bool WorldSession::HasReceivedChangelog()
{
    return m_ChangelogReceived;
}
