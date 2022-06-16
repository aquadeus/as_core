////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ArtifactPackets_h__
#define ArtifactPackets_h__

#include "Packet.h"
#include "PacketUtilities.h"

namespace WorldPackets
{
    namespace Artifact
    {
        class ConfirmRespec final : public ClientPacket
        {
        public:
            ConfirmRespec(WorldPacket& packet) : ClientPacket(CMSG_ARTIFACT_CONFIRM_RESPEC, packet) { }

            void Read() override;

            uint64 ArtifactGUID = 0;
            uint64 NpcGUID = 0;
        };

        class RespecResult final : public ServerPacket
        {
        public:
            RespecResult() : ServerPacket(SMSG_ARTIFACT_RESPEC_CONFIRM, 16 + 16) { }

            WorldPacket const* Write() override;
            
            uint64 ArtifactGUID = 0;
            uint64 NpcGUID = 0;
        };
    }
}

#endif // ArtifactPackets_h__
