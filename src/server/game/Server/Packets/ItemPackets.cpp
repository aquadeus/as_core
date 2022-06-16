////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ItemPackets.h"

void WorldPackets::WPItem::SocketGems::Read()
{
    _worldPacket.readPackGUID(ItemGuid);
    for (uint64 &gem : GemItem)
        _worldPacket.readPackGUID(gem);
}

WorldPacket const* WorldPackets::WPItem::SocketGemsResult::Write()
{
    _worldPacket.appendPackGUID(Item);

    return &_worldPacket;
}
