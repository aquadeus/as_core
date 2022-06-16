////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CROSS
#include "Language.h"
#include "WorldPacket.h"
#include "Common.h"
#include "ObjectMgr.h"
#include "TicketMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "Util.h"
#include "WowTime.hpp"

void WorldSession::OnGMTicketGetTicketEvent()
{
    if (m_Player == nullptr)
        return;

    SendQueryTimeResponse();

    if (GmTicket* l_Ticket = sTicketMgr->GetTicketByPlayer(GetPlayer()->GetGUID()))
    {
        if (l_Ticket->IsCompleted())
            l_Ticket->SendResponse(this);
        else
            sTicketMgr->SendTicket(this, l_Ticket);
    }
    else
        sTicketMgr->SendTicket(this, NULL);
}

void WorldSession::HandleGMTicketGetWebTicketOpcode(WorldPacket& /*p_RecvData*/)
{
    time_t l_Now = time(nullptr);
    WorldPacket l_Data(SMSG_GM_TICKET_CASE_STATUS);
    l_Data << MS::Utilities::WowTime::Encode(l_Now);
    l_Data << uint32(l_Now - GetLoginTime());
    l_Data << uint32(0);
    SendPacket(&l_Data);
}

void WorldSession::HandleGMTicketSystemStatusOpcode(WorldPacket& /*p_RecvData*/)
{
    WorldPacket l_Data(SMSG_GM_TICKET_SYSTEM_STATUS, 4);
    l_Data << uint32(sTicketMgr->GetStatus() ? GMTICKET_QUEUE_STATUS_ENABLED : GMTICKET_QUEUE_STATUS_DISABLED);
    SendPacket(&l_Data);
}

void WorldSession::SendTicketStatusUpdate(uint8 p_Response)
{
    if (!GetPlayer())
        return;

    switch (p_Response)
    {
        case GMTICKET_RESPONSE_ALREADY_EXIST:       ///< = 1
            SendGameError(GameError::ERR_TICKET_ALREADY_EXISTS);
            break;
        case GMTICKET_RESPONSE_UPDATE_ERROR:        ///< = 5
            SendGameError(GameError::ERR_TICKET_UPDATE_ERROR);
            break;
        case GMTICKET_RESPONSE_CREATE_ERROR:        ///< = 3
            SendGameError(GameError::ERR_TICKET_CREATE_ERROR);
            break;

        case GMTICKET_RESPONSE_CREATE_SUCCESS:      ///< = 2
        case GMTICKET_RESPONSE_UPDATE_SUCCESS:      ///< = 4
            OnGMTicketGetTicketEvent();
            break;

        case GMTICKET_RESPONSE_TICKET_DELETED:      ///< = 9
            GetPlayer()->SendCustomMessage("FSC_TICKET_DELETED");
            break;

        default:
            SendGameError(GameError::ERR_TICKET_DB_ERROR);
            break;

    }
}
#endif
