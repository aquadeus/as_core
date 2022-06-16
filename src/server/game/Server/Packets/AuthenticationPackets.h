////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef AuthenticationPackets_h__
#define AuthenticationPackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace Auth
    {
        class AuthChallenge final : public ServerPacket
        {
        public:
            AuthChallenge() : ServerPacket(SMSG_AUTH_CHALLENGE, 4 + 32 + 1) { }

            WorldPacket const* Write() override;

            std::array<uint8, 16> Challenge;
            uint32 DosChallenge[8];
            uint8 DosZeroBits = 0;
        };

        enum class ConnectToSerial : uint32
        {
            None = 0,
            Realm = 14,
            WorldAttempt1 = 17,
            WorldAttempt2 = 35,
            WorldAttempt3 = 53,
            WorldAttempt4 = 71,
            WorldAttempt5 = 89
        };

        class ConnectTo final : public ServerPacket
        {
            static std::string const Haiku;
            static uint8 const PiDigits[130];

            enum AddressType : uint8
            {
                IPv4 = 1,
                IPv6 = 2
            };

            struct ConnectPayload
            {
                ACE_INET_Addr Address;
                uint32 Adler32 = 0;
                uint8 XorMagic = 0x2A;
                uint8 PanamaKey[32];
            };

        public:
            static bool InitializeEncryption();

            ConnectTo();

            WorldPacket const* Write() override;

            uint64 Key = 0;
            ConnectToSerial Serial = ConnectToSerial::None;
            ConnectPayload Payload;
            uint8 Con = 0;
        };

        class AuthContinuedSession final : public ClientPacket
        {
        public:
            static uint32 const DigestLength = 24;

            AuthContinuedSession(WorldPacket& packet) : ClientPacket(CMSG_AUTH_CONTINUED_SESSION, packet)
            {
                LocalChallenge.fill(0);
                Digest.fill(0);
            }

            void Read() override;

            uint64 DosResponse = 0;
            uint64 Key = 0;
            std::array<uint8, 16> LocalChallenge;
            std::array<uint8, DigestLength> Digest;
        };

        class ConnectToFailed final : public ClientPacket
        {
        public:
            ConnectToFailed(WorldPacket& packet) : ClientPacket(CMSG_CONNECT_TO_FAILED, packet) { }

            void Read() override;

            ConnectToSerial Serial = ConnectToSerial::None;
            uint8 Con = 0;
        };

        class ResumeComms final : public ServerPacket
        {
        public:
            ResumeComms() : ServerPacket(SMSG_RESUME_COMMS, 0) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };
    }
}

#endif ///< AuthenticationPacketsWorld_h__