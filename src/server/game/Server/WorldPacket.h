////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WORLDPACKET_H
#define WORLDPACKET_H

#include "Common.h"
#include "Opcodes.h"
#include "ByteBuffer.h"

struct z_stream_s;

extern std::mutex gPacketProfilerMutex;
extern std::map<uint32, uint32> gPacketProfilerData;

struct PacketSendingInfo
{
    WorldObject*  Sender;
    ConnectionType ConnIdx;

    WorldPacket* Packet;
    bool         Self;
    bool         Broadcast;

    PacketSendingInfo(WorldObject* p_Sender, WorldPacket const& p_Packet, bool p_Self, bool p_Broadcast);
};

class WorldPacket : public ByteBuffer
{
    public:
                                                            // just container for later use
        WorldPacket() : ByteBuffer(0), m_opcode((uint16)UNKNOWN_OPCODE)
        {
            GeneratePacketID();
            m_UnsafeReferenceCount = 0;
            m_SafeReferenceCount = 1;
        }

        WorldPacket(uint16 opcode, size_t res = 200) : ByteBuffer(res), m_opcode(opcode)
        {
            GeneratePacketID();

            m_UnsafeReferenceCount = 0;
            m_SafeReferenceCount = 1;
        }
                                                            // copy constructor
        WorldPacket(WorldPacket const& packet) : ByteBuffer(packet), m_opcode(packet.m_opcode), m_PacketId(packet.m_PacketId)
        {
            m_UnsafeReferenceCount = 0;
            m_SafeReferenceCount = 1;
        }

        void Initialize(uint16 opcode, size_t newres = 200)
        {
            GeneratePacketID();
            clear();
            _storage.reserve(newres);
            m_opcode = opcode;
            m_BaseSize = newres;
        }

        static void InitPacketID();
        static void ResetPacketID();
        static int32 GetNextPacketID();
        static int32 GetCurrentPacketID();

        static void AddMapPacket(PacketSendingInfo p_PacketInfo);
        static std::vector<PacketSendingInfo>& GetMapPacketBuffer();

        void GeneratePacketID()
        {
            if (GetCurrentPacketID() != -1)
                m_PacketId = GetNextPacketID();
        }

        int32 GetPacketId() const { return m_PacketId; }
        uint16 GetOpcode() const { return m_opcode; }
        void SetOpcode(uint16 opcode) { m_opcode = opcode; }

        uint32 m_UnsafeReferenceCount;          ///< Single Thread only
        std::atomic_uint m_SafeReferenceCount;  ///< Multi thread

    protected:
        uint16 m_opcode;
        int32  m_PacketId;
};
#endif
