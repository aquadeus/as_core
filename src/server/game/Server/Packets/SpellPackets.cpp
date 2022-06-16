////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "SpellPackets.h"

WorldPacket const* WorldPackets::Spells::AreaTriggerRePath::Write()
{
    _worldPacket.appendPackGUID(TriggerGUID);
    _worldPacket.WriteBit(Spline.is_initialized());
    _worldPacket.WriteBit(CircleData.is_initialized());
    _worldPacket.FlushBits();

    if (Spline)
        _worldPacket << *Spline;

    if (CircleData)
        _worldPacket << *CircleData;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::AreaTriggerReShape::Write()
{
    _worldPacket.appendPackGUID(TriggerGUID);
    _worldPacket << (uint32)ShapeID;
    _worldPacket.WriteBit(UnkBit);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, AreaTriggerSpline const& triggerSplineData)
{
    data << triggerSplineData.TimeToTarget;
    data << triggerSplineData.ElapsedTimeForMovement;

    data.FlushBits();

    data.WriteBits(triggerSplineData.VerticesPoints.size(), 16);
    for (auto& x : const_cast<AreaTriggerSpline&>(triggerSplineData).VerticesPoints)
        data << x;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, AreaTriggerCircle const& triggerCircleData)
{
    data.FlushBits();

    data.WriteBit(triggerCircleData.TargetGuid.is_initialized());
    data.WriteBit(triggerCircleData.CenterPoint.is_initialized());
    data.WriteBit(triggerCircleData.Clockwise);
    data.WriteBit(triggerCircleData.byte45);

    data << triggerCircleData.TimeToTarget;
    data << triggerCircleData.dword2C;
    data << triggerCircleData.dword30;
    data << triggerCircleData.Radius;
    data << triggerCircleData.BlendFormRadius;
    data << triggerCircleData.InitialAngle;
    data << triggerCircleData.ZOffset;

    if (triggerCircleData.TargetGuid)
        data.appendPackGUID(*triggerCircleData.TargetGuid);

    if (triggerCircleData.CenterPoint)
        data << *triggerCircleData.CenterPoint;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, AreaTriggerPolygon const& polygon)
{
    data << static_cast<uint32>(polygon.Vertices.size());
    data << static_cast<uint32>(polygon.VerticesTarget.size());
    data << polygon.Height;
    data << polygon.HeightTarget;

    for (auto const& v : polygon.Vertices)
        data << v.second;

    for (auto const& v : polygon.VerticesTarget)
        data << v.second;

    return data;
}

WorldPacket const* WorldPackets::Spells::CustomLoadScreen::Write()
{
    _worldPacket << uint32(TeleportSpellID);
    _worldPacket << uint32(LoadingScreenID);

    return &_worldPacket;
}
