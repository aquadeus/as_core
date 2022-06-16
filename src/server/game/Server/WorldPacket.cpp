////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include <zlib.h>
#include "WorldPacket.h"
#include "World.h"

static thread_local int32 tl_PacketID = -1;
static thread_local std::vector<PacketSendingInfo> tl_MapPacketBuffer;

PacketSendingInfo::PacketSendingInfo(WorldObject* p_Sender, WorldPacket const& p_Packet, bool p_Self, bool p_Broadcast)
{
    if (p_Sender && p_Sender->ToPlayer() && p_Sender->ToPlayer()->m_IsInDestructor)
    {
        if (p_Sender->ToPlayer()->m_IsInDestructor < time(nullptr) - 20)
        {
            sLog->outExtChat("#jarvis-log", "danger", true, "PacketSendingInfo: destructed player send a packet");
            if (sWorld->getBoolConfig(CONFIG_ENABLE_UNSAFE_SPELL_AURA_HOOK_STACK_TRACE))
            {
                ACE_Stack_Trace l_StackTrace;
                sLog->outExtChat("#jarvis-log", "danger", true, l_StackTrace.c_str());
            }
        }
    }

    Sender    = p_Sender;
    Packet    = new WorldPacket(p_Packet);
    Self      = p_Self;
    Broadcast = p_Broadcast;

    ConnIdx = GetOpcodeConnectionType(Packet->GetOpcode());
}

void WorldPacket::AddMapPacket(PacketSendingInfo p_PacketInfo)
{
    tl_MapPacketBuffer.push_back(p_PacketInfo);
}

std::vector<PacketSendingInfo>& WorldPacket::GetMapPacketBuffer()
{
    return tl_MapPacketBuffer;
}

void WorldPacket::InitPacketID()
{
    tl_PacketID = 0;
}

void WorldPacket::ResetPacketID()
{
    tl_PacketID = -1;
}

int32 WorldPacket::GetCurrentPacketID()
{
    return tl_PacketID;
}

int32 WorldPacket::GetNextPacketID()
{
    return ++tl_PacketID;
}