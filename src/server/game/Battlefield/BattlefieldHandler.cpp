////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "OutdoorPvPMgr.h"
#include "Opcodes.h"

//This send to player windows for invite player to join the war
//Param1:(m_Guid) the BattleId of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(time) Time in second that the player have for accept
void WorldSession::SendBfInvitePlayerToWar(uint64 guid, uint32 zoneId, uint32 pTime)
{
    ///< Send packet
    WorldPacket data(SMSG_BFMGR_ENTRY_INVITE);
    data << uint64(guid);               ///< QueueID
    data << uint32(zoneId);             ///< Zone Id
    data << uint32(pTime);              ///< Invite lasts until

    ///< Sending the packet to player
    SendPacket(&data);
}

//This send invitation to player to join the queue
//Param1:(guid) the guid of Bf
void WorldSession::SendBfInvitePlayerToQueue(uint64 p_Guid)
{
    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE);
    data << uint64(p_Guid);         ///< QueueID
    data << uint8(1);               ///< BattleState

    /// contain Timeout, MinLevel, MaxLevel, InstanceID and MapID  dont know the order
    data << int32(0);
    data << int32(0);
    data << int32(0);
    data << uint32(0);
    data << uint32(0);

    data.WriteBit(0); ///< Index
    data.FlushBits();

    /// Sending packet to player
    SendPacket(&data);
}

//This send packet for inform player that he join queue
//Param1:(guid) the guid of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(CanQueue) if able to queue
//Param4:(Full) on log in is full
void WorldSession::SendBfQueueInviteResponse(uint64 p_Guid, uint32 p_ZoneID, bool p_CanQueue, bool p_Full)
{
    WorldPacket l_Data(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE);
    l_Data << uint64(p_Guid);               ///< QueueID
    l_Data << uint32(p_ZoneID);             ///< AreaID
    l_Data << uint8((p_CanQueue ? 1 : 0));  ///< Result (0 : you cannot queue wg, 1 : you are queued)
    l_Data << uint8(1);                     ///< BattleState
    l_Data.appendPackGUID(0);               ///< FailedPlayerGUID

    l_Data.WriteBit((p_Full ? 0 : 1));      ///< LoggingIn(0 : wg full, 1 : queue for upcoming (we may need to swap it))
    l_Data.FlushBits();

    SendPacket(&l_Data);
}

//This is call when player accept to join war
//Param1:(guid) the guid of Bf
void WorldSession::SendBfEntered(uint64 p_Guid)
{
    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTERING);
    data.WriteBit(m_Player->isAFK() ? 1 : 0);   ///< Cleared AFK
    data.WriteBit(true);                        ///< On Offense
    data.WriteBit(true);                        ///< Relocated
    data.FlushBits();
    data << uint64(p_Guid);

    SendPacket(&data);
}

void WorldSession::SendBfLeaveMessage(uint64 p_Guid, BFLeaveReason p_Reason)
{
    WorldPacket l_Data(SMSG_BATTLEFIELD_MGR_EJECTED);
    l_Data << uint64(p_Guid);   ///< Queue ID
    l_Data << uint8(p_Reason);  ///< Reason
    l_Data << uint8(2);         ///< BattleStatus

    l_Data.WriteBit(false);     ///< Relocated
    l_Data.FlushBits();

    SendPacket(&l_Data);
}

void WorldSession::HandleBfExitRequest(WorldPacket& /*recv_data*/)
{
    sLog->outError(LOG_FILTER_GENERAL, "HandleBfExitRequest");
    Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(m_Player->GetZoneId());
    if (bf)
         bf->KickPlayerFromBattlefield(m_Player->GetGUID());
}

void WorldSession::HandleReportPvPAFK(WorldPacket& recvData)
{
    uint64 l_PlayerGUID = 0;
    recvData.readPackGUID(l_PlayerGUID);

    Player* l_ReportedPlayer = ObjectAccessor::FindPlayer(l_PlayerGUID);
    if (!l_ReportedPlayer)
    {
        sLog->outDebug(LOG_FILTER_BATTLEGROUND, "WorldSession::HandleReportPvPAFK: player not found");
        return;
    }

    sLog->outDebug(LOG_FILTER_BATTLEGROUND, "WorldSession::HandleReportPvPAFK: %s reported %s", m_Player->GetName(), l_ReportedPlayer->GetName());

    l_ReportedPlayer->ReportedAfkBy(m_Player);
}

void WorldSession::HandleRequestPvpOptions(WorldPacket& /*recvData*/)
{
   /// The values should be hooked on active pvp season, so the ui get blocked
   bool l_ActiveRatedArenas          = true;
   bool l_ActiveArenaSkirmish        = true;
   bool l_ActivePugBattlegrounds     = true;
   bool l_ActiveWargameBattlegrounds = true;
   bool l_ActiveWargameArenas        = true;
   bool l_ActiveRatedBattlegrounds   = true;

   WorldPacket l_Data(SMSG_PVP_OPTIONS_ENABLED, 1);
   l_Data.WriteBit(l_ActiveRatedArenas);            ///< RatedArenas
   l_Data.WriteBit(l_ActiveArenaSkirmish);          ///< ArenaSkirmish
   l_Data.WriteBit(l_ActivePugBattlegrounds);       ///< PugBattlegrounds
   l_Data.WriteBit(l_ActiveWargameBattlegrounds);   ///< WargameBattlegrounds
   l_Data.WriteBit(l_ActiveWargameArenas);          ///< WargameArenas
   l_Data.WriteBit(l_ActiveRatedBattlegrounds);     ///< RatedBattlegrounds
   l_Data.FlushBits();

   SendPacket(&l_Data);
}

void WorldSession::HandleRequestPvpReward(WorldPacket& /*recvData*/)
{
    m_Player->SendPvpRewards();
}

void WorldSession::HandleRequestRatedBgStats(WorldPacket& /*recvData*/)
{
    WorldPacket l_Data(SMSG_RATED_BATTLEFIELD_INFO, (uint8)BattlegroundBracketType::Max * 11 * 4);

    for (uint8 l_Bracket = 0; l_Bracket < (uint8)BattlegroundBracketType::Max; l_Bracket++)
    {
        l_Data << uint32(m_Player->GetArenaPersonalRating(l_Bracket));
        l_Data << uint32(0);                                            ///< Ranking
        l_Data << uint32(m_Player->GetSeasonGames(l_Bracket));
        l_Data << uint32(m_Player->GetSeasonWins(l_Bracket));
        l_Data << uint32(m_Player->GetPrevWeekGames(l_Bracket));
        l_Data << uint32(m_Player->GetPrevWeekWins(l_Bracket));
        l_Data << uint32(m_Player->GetWeekGames(l_Bracket));
        l_Data << uint32(m_Player->GetWeekWins(l_Bracket));
        l_Data << uint32(m_Player->GetBestRatingOfWeek(l_Bracket));
        l_Data << uint32(m_Player->GetBestRatingOfPreviousWeek(l_Bracket));
        l_Data << uint32(m_Player->GetBestRatingOfSeason(l_Bracket));
    }

    SendPacket(&l_Data);
}

void WorldSession::HandleRequestConquestFormulaConstants(WorldPacket& /*p_RecvData*/)
{
}
