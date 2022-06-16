////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SpellPackets_h__
#define SpellPackets_h__

#include "Packet.h"
#include "PacketUtilities.h"
#include "AreaTrigger.h"


namespace WorldPackets
{
    namespace Spells
    {
        class AreaTriggerRePath final : public ServerPacket
        {
            public:
                AreaTriggerRePath() : ServerPacket(SMSG_AREA_TRIGGER_RE_PATH, 12 + 2) { }

                WorldPacket const* Write() override;

                Optional<AreaTriggerSpline> Spline;
                Optional<AreaTriggerCircle> CircleData;
                uint64 TriggerGUID = 0;
        };

        class AreaTriggerReShape final : public ServerPacket
        {
            public:
                AreaTriggerReShape() : ServerPacket(SMSG_AREA_TRIGGER_RE_SHAPE, 16 + 4 + 1) { }

                WorldPacket const* Write() override;

                uint64  TriggerGUID = 0;
                uint32  ShapeID     = 0;
                bool    UnkBit      = false;
        };

        class CustomLoadScreen final : public ServerPacket
        {
            public:
                CustomLoadScreen(uint32 teleportSpellId, uint32 loadingScreenId) : ServerPacket(Opcodes::SMSG_CUSTOM_LOAD_SCREEN), TeleportSpellID(teleportSpellId), LoadingScreenID(loadingScreenId) { }

                WorldPacket const* Write() override;

                uint32 TeleportSpellID;
                uint32 LoadingScreenID;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, AreaTriggerSpline const& triggerSplineData);
ByteBuffer& operator<<(ByteBuffer& data, AreaTriggerCircle const& triggerCircleData);
ByteBuffer& operator<<(ByteBuffer& data, AreaTriggerPolygon const& polygon);
#endif // SpellPackets_h__
