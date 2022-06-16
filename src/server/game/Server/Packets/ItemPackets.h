////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ItemPackets_h__
#define ItemPackets_h__

#include "Packet.h"
#include "Item.h"
#include "Group.h"

namespace WorldPackets
{
    namespace WPItem
    {
        class SocketGems final : public ClientPacket
        {
            public:
                SocketGems(WorldPacket& packet) : ClientPacket(CMSG_SOCKET_GEMS, packet) { }

                void Read() override;
            
                std::array<uint64, MAX_GEM_SOCKETS> GemItem;
                uint64 ItemGuid;
        };

        class SocketGemsResult final : public ServerPacket
        {
            public:
                SocketGemsResult(ObjectGuid item) : ServerPacket(SMSG_SOCKET_GEMS, 16), Item(item) { }

                WorldPacket const* Write() override;
            
                uint64 Item;
        };
    }
}
#endif // ItemPackets_h__
