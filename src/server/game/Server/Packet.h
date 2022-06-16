////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef PacketBaseWorld_h__
#define PacketBaseWorld_h__

#include "WorldPacket.h"

namespace WorldPackets
{
    class Packet
    {
        public:
            Packet(WorldPacket& worldPacket) : _worldPacket(worldPacket) { }

            virtual ~Packet() = default;

            Packet(Packet const& right) = delete;
            Packet& operator=(Packet const& right) = delete;

            virtual WorldPacket const* Write() = 0;
            virtual void Read() = 0;

            WorldPacket const* GetRawPacket() const { return &_worldPacket; }
            size_t GetSize() const { return _worldPacket.size(); }

        protected:
            WorldPacket& _worldPacket;
    };

    class ServerPacket : public Packet
    {
        public:
            ServerPacket(Opcodes opcode, size_t initialSize = 200) : m_ServerPacket(WorldPacket(static_cast<uint16>(opcode), initialSize)), Packet(m_ServerPacket) { }

            void Read() override final { ASSERT(!"Read not implemented for server packets."); }

            void Clear() { _worldPacket.clear(); }

            Opcodes GetOpcode() const { return Opcodes(_worldPacket.GetOpcode()); }

        protected:
            WorldPacket m_ServerPacket;
    };
    
    class ClientPacket : public Packet
    {
        public:
            ClientPacket(WorldPacket& packet) : Packet(packet) { }
            ClientPacket(Opcodes expectedOpcode, WorldPacket& packet) : Packet(packet) { ASSERT(GetOpcode() == expectedOpcode); }

            WorldPacket const* Write() override final
            {
                ASSERT(!"Write not allowed for client packets.");
                // Shut up some compilers
                return nullptr;
            }

            Opcodes GetOpcode() const { return Opcodes(_worldPacket.GetOpcode()); }
    };
}

#endif // PacketBaseWorld_h__
