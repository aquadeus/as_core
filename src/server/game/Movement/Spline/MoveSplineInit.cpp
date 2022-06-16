////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Unit.h"
#include "CreatureAI.h"
#include "Transport.h"
#include "AnticheatMgr.h"
#include <boost/optional.hpp>

struct MovementMonsterSpline
{
    uint32 ID = 0;
    G3D::Vector3 Destination;
    bool CrzTeleport = false;
    uint8 StopDistanceTolerance = 0;    // Determines how far from spline destination the mover is allowed to stop in place 0, 0, 3.0, 2.76, numeric_limits<float>::max, 1.1, float(INT_MAX); default before this field existed was distance 3.0 (index 2)
    Movement::MovementSpline Move;
};

class MonsterMove
{
public:
    MonsterMove()
    {

    }

    void InitializeSplineData(Movement::MoveSpline const& moveSpline);

    void Write(WorldPacket * data);

    MovementMonsterSpline SplineData;
    uint64 MoverGUID;
    G3D::Vector3 Pos;
};

ByteBuffer& operator<<(ByteBuffer& data, Movement::MonsterSplineFilterKey const& monsterSplineFilterKey)
{
    data << int16(monsterSplineFilterKey.Idx);
    data << uint16(monsterSplineFilterKey.Speed);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, Movement::MonsterSplineFilter const& monsterSplineFilter)
{
    data << uint32(monsterSplineFilter.FilterKeys.size());
    data << float(monsterSplineFilter.BaseSpeed);
    data << int16(monsterSplineFilter.StartOffset);
    data << float(monsterSplineFilter.DistToPrevFilterKey);
    data << int16(monsterSplineFilter.AddedToStart);
    for (Movement::MonsterSplineFilterKey const& filterKey : monsterSplineFilter.FilterKeys)
        data << filterKey;
    data.WriteBits(monsterSplineFilter.FilterFlags, 2);
    data.FlushBits();

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, Movement::MovementSpline const& movementSpline)
{
    data << uint32(movementSpline.Flags);
    data << uint8(movementSpline.AnimTier);
    data << uint32(movementSpline.TierTransStartTime);
    data << int32(movementSpline.Elapsed);
    data << uint32(movementSpline.MoveTime);
    data << float(movementSpline.JumpGravity);
    data << uint32(movementSpline.SpecialTime);
    data << uint8(movementSpline.Mode);
    data << uint8(movementSpline.VehicleExitVoluntary);
    data.appendPackGUID(movementSpline.TransportGUID);
    data << int8(movementSpline.VehicleSeat);
    data.WriteBits(movementSpline.Face, 2);
    data.WriteBits(movementSpline.Points.size(), 16);
    data.WriteBits(movementSpline.PackedDeltas.size(), 16);
    data.WriteBit(movementSpline.SplineFilter.is_initialized());
    data.WriteBit(movementSpline.SpellEffectExtraData.is_initialized());
    data.FlushBits();

    if (movementSpline.SplineFilter)
        data << *movementSpline.SplineFilter;

    switch (movementSpline.Face)
    {
    case ::Movement::MONSTER_MOVE_FACING_SPOT:
        data << movementSpline.FaceSpot.x;
        data << movementSpline.FaceSpot.y;
        data << movementSpline.FaceSpot.z;
        break;
    case ::Movement::MONSTER_MOVE_FACING_TARGET:
        data << float(movementSpline.FaceDirection);
        data.appendPackGUID(movementSpline.FaceGUID);
        break;
    case ::Movement::MONSTER_MOVE_FACING_ANGLE:
        data << float(movementSpline.FaceDirection);
        break;
    }

    for (G3D::Vector3 const& pos : movementSpline.Points)
    {
        data << pos.x;
        data << pos.y;
        data << pos.z;
    }

    for (G3D::Vector3 const& pos : movementSpline.PackedDeltas)
        data.appendPackXYZ(pos.x, pos.y, pos.z);

    if (movementSpline.SpellEffectExtraData.is_initialized())
    {
        Movement::SplineSpellEffectExtraData const& l_SpellEffectExtraData = movementSpline.SpellEffectExtraData.get();
        data.appendPackGUID(l_SpellEffectExtraData.TargetGuid);
        data << uint32(l_SpellEffectExtraData.SpellVisualId);
        data << uint32(l_SpellEffectExtraData.ProgressCurveId);
        data << uint32(l_SpellEffectExtraData.ParabolicCurveId);
    }

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, MovementMonsterSpline const& movementMonsterSpline)
{
    data << movementMonsterSpline.ID;
    data << movementMonsterSpline.Destination.x;
    data << movementMonsterSpline.Destination.y;
    data << movementMonsterSpline.Destination.z;

    data.WriteBit(movementMonsterSpline.CrzTeleport);
    data.WriteBits(movementMonsterSpline.StopDistanceTolerance, 3); ///< Unk bits. 0 if monster is moving, 1 or 2 if stopped
    data.FlushBits();

    data << movementMonsterSpline.Move;
    return data;
}

void MonsterMove::Write(WorldPacket * _worldPacket)
{
    (*_worldPacket).appendPackGUID(MoverGUID);
    (*_worldPacket) << Pos.x;
    (*_worldPacket) << Pos.y;
    (*_worldPacket) << Pos.z;
    (*_worldPacket) << SplineData;
}

void MonsterMove::InitializeSplineData(::Movement::MoveSpline const& moveSpline)
{
    SplineData.ID = moveSpline.m_Id;
    Movement::MovementSpline& movementSpline = SplineData.Move;

    ::Movement::MoveSplineFlag splineFlags = moveSpline.splineflags;
    splineFlags.enter_cycle = moveSpline.isCyclic();
    movementSpline.Flags = uint32(splineFlags & uint32(~::Movement::MoveSplineFlag::Mask_No_Monster_Move));

    movementSpline.Face = moveSpline.facing.type;
    movementSpline.FaceDirection = moveSpline.facing.angle;
    movementSpline.FaceGUID = moveSpline.facing.target;
    movementSpline.FaceSpot = moveSpline.facing.f;

    if (splineFlags.animation)
    {
        movementSpline.AnimTier = splineFlags.getAnimationId();
        movementSpline.TierTransStartTime = moveSpline.effect_start_time;
    }

    movementSpline.MoveTime = moveSpline.Duration();

    if (splineFlags.parabolic)
    {
        movementSpline.JumpGravity = moveSpline.vertical_acceleration;
        movementSpline.SpecialTime = moveSpline.effect_start_time;
    }

    if (splineFlags.unknown6)
        movementSpline.SpecialTime = moveSpline.effect_start_time;

    if (moveSpline.m_MonsterSplineFilter.is_initialized())
    {
        movementSpline.SplineFilter.emplace();

        movementSpline.SplineFilter->AddedToStart           = moveSpline.m_MonsterSplineFilter->AddedToStart;
        movementSpline.SplineFilter->BaseSpeed              = moveSpline.m_MonsterSplineFilter->BaseSpeed;
        movementSpline.SplineFilter->DistToPrevFilterKey    = moveSpline.m_MonsterSplineFilter->DistToPrevFilterKey;
        movementSpline.SplineFilter->FilterFlags            = moveSpline.m_MonsterSplineFilter->FilterFlags;
        movementSpline.SplineFilter->FilterKeys             = moveSpline.m_MonsterSplineFilter->FilterKeys;
        movementSpline.SplineFilter->StartOffset            = moveSpline.m_MonsterSplineFilter->StartOffset;
    }

    if (moveSpline.m_SpellEffectExtraData.is_initialized())
    {
        movementSpline.SpellEffectExtraData.emplace();
        movementSpline.SpellEffectExtraData->TargetGuid = moveSpline.m_SpellEffectExtraData->TargetGuid;
        movementSpline.SpellEffectExtraData->SpellVisualId = moveSpline.m_SpellEffectExtraData->SpellVisualId;
        movementSpline.SpellEffectExtraData->ProgressCurveId = moveSpline.m_SpellEffectExtraData->ProgressCurveId;
        movementSpline.SpellEffectExtraData->ParabolicCurveId = moveSpline.m_SpellEffectExtraData->ParabolicCurveId;
    }

    ::Movement::Spline<int32> const& spline = moveSpline.spline;
    std::vector<G3D::Vector3> const& array = spline.getPoints();

    if (splineFlags & ::Movement::MoveSplineFlag::UncompressedPath)
    {
        if (!splineFlags.cyclic)
        {
            uint32 count = spline.getPointCount() - 3;
            for (uint32 i = 0; i < count; ++i)
                movementSpline.Points.push_back(array[i + 2]);
        }
        else
        {
            uint32 count = spline.getPointCount() - 3;
            movementSpline.Points.push_back(array[1]);
            for (uint32 i = 0; i < count; ++i)
                movementSpline.Points.push_back(array[i + 1]);
        }
    }
    else
    {
        uint32 lastIdx = spline.getPointCount() - 3;
        G3D::Vector3 const* realPath = &spline.getPoint(1);

        movementSpline.Points.push_back(realPath[lastIdx]);

        if (lastIdx > 1)
        {
            G3D::Vector3 middle = (realPath[0] + realPath[lastIdx]) / 2.0f;

            // first and last points already appended
            for (uint32 i = 1; i < lastIdx; ++i)
                movementSpline.PackedDeltas.push_back(middle - realPath[i]);
        }
    }
}


namespace Movement
{
    UnitMoveType SelectSpeedType(uint32 moveFlags)
    {
        if (moveFlags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_DISABLE_GRAVITY))
        {
            if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.flight >= speed_obj.flight_back*/)
                return MOVE_FLIGHT_BACK;
            else
                return MOVE_FLIGHT;
        }
        else if (moveFlags & MOVEMENTFLAG_SWIMMING)
        {
            if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.swim >= speed_obj.swim_back*/)
                return MOVE_SWIM_BACK;
            else
                return MOVE_SWIM;
        }
        else if (moveFlags & MOVEMENTFLAG_WALKING)
        {
            //if (speed_obj.run > speed_obj.walk)
            return MOVE_WALK;
        }
        else if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.run >= speed_obj.run_back*/)
            return MOVE_RUN_BACK;

        // Flying creatures use MOVEMENTFLAG_CAN_FLY or MOVEMENTFLAG_DISABLE_GRAVITY
        // Run speed is their default flight speed.
        return MOVE_RUN;
    }

    int32 MoveSplineInit::Launch()
    {
        MoveSpline& l_MoveSpline = *unit->movespline;

        bool transport = unit->GetTransGUID() != 0;
        Location real_position;
        // there is a big chance that current position is unknown if current state is not finalized, need compute it
        // this also allows calculate spline position and update map position in much greater intervals
        // Don't compute for transport movement if the unit is in a motion between two transports
        if (!l_MoveSpline.Finalized() && l_MoveSpline.onTransport == transport && !(l_MoveSpline.splineflags.transportEnter && args.flags.transportExit))
            real_position = l_MoveSpline.ComputePosition();
        else
        {
            Position const* pos;
            if (!transport)
                pos = unit;
            else
                pos = &unit->m_movementInfo.t_pos;

            real_position.x = pos->GetPositionX();
            real_position.y = pos->GetPositionY();
            real_position.z = pos->GetPositionZ();
            real_position.orientation = unit->GetOrientation();
        }

        // should i do the things that user should do? - no.
        if (args.path.empty())
            return 0;

        // correct first vertex
        args.path[0] = real_position;
        args.initialOrientation = real_position.orientation;
        l_MoveSpline.onTransport = unit->GetTransGUID() != 0;

        uint32 moveFlags = unit->m_movementInfo.GetMovementFlags();
        if (!args.flags.backward)
            moveFlags = (moveFlags & ~MOVEMENTFLAG_BACKWARD) | MOVEMENTFLAG_FORWARD;
        else
            moveFlags = (moveFlags & ~MOVEMENTFLAG_FORWARD) | MOVEMENTFLAG_BACKWARD;

        if (moveFlags & MOVEMENTFLAG_ROOT)
            moveFlags &= ~MOVEMENTFLAG_MASK_MOVING;

        if (!args.HasVelocity)
        {
            // If spline is initialized with SetWalk method it only means we need to select
            // walk move speed for it but not add walk flag to unit
            uint32 moveFlagsForSpeed = moveFlags;
            if (args.walk)
                moveFlagsForSpeed |= MOVEMENTFLAG_WALKING;
            else
                moveFlagsForSpeed &= ~MOVEMENTFLAG_WALKING;

            args.velocity = unit->GetSpeed(SelectSpeedType(moveFlagsForSpeed));

            if (Creature* l_Creature = unit->ToCreature())
            {
                if (l_Creature->IsAIEnabled)
                    l_Creature->AI()->OnCalculateMoveSpeed(args.velocity);
            }
        }

        if (!args.Validate(unit))
            return 0;

        unit->m_movementInfo.SetMovementFlags(moveFlags);
        l_MoveSpline.Initialize(args);

        MonsterMove packet;
        packet.MoverGUID = unit->GetGUID();
        packet.Pos = real_position;
        packet.InitializeSplineData(l_MoveSpline);
        if (transport)
        {
            packet.SplineData.Move.TransportGUID = unit->GetTransGUID();
            packet.SplineData.Move.VehicleSeat = unit->GetTransSeat();
        }

        WorldPacket l_Data(SMSG_MONSTER_MOVE);
        packet.Write(&l_Data);

        unit->SendMessageToSet(&l_Data, true);

        if (unit->IsPlayer())
            sAnticheatMgr->HandleStartSpline(unit->GetGUIDLow());

        return l_MoveSpline.Duration();
    }

    void MoveSplineInit::LaunchRotate(float p_Orientation)
    {
        MonsterMove l_Packet;

        l_Packet.MoverGUID = unit->GetGUID();

        l_Packet.Pos.x = unit->m_positionX;
        l_Packet.Pos.y = unit->m_positionY;
        l_Packet.Pos.z = unit->m_positionZ;

        l_Packet.SplineData.StopDistanceTolerance = 2;

        l_Packet.SplineData.Move.Face = MONSTER_MOVE_FACING_ANGLE;
        l_Packet.SplineData.Move.FaceDirection = p_Orientation;

        WorldPacket l_Data(SMSG_MONSTER_MOVE);
        l_Packet.Write(&l_Data);

        unit->SendMessageToSet(&l_Data, true);
    }

    void MoveSplineInit::Stop(/*bool *//*force*/)
    {
        MoveSpline& move_spline = *unit->movespline;

        // No need to stop if we are not moving
        if (move_spline.Finalized())
            return;

        bool transport = unit->GetTransGUID() != 0;
        Location loc;
        if (move_spline.onTransport == transport)
            loc = move_spline.ComputePosition();
        else
        {
            Position const* pos;
            if (!transport)
                pos = unit;
            else
                pos = &unit->m_movementInfo.t_pos;

            loc.x = pos->GetPositionX();
            loc.y = pos->GetPositionY();
            loc.z = pos->GetPositionZ();
            loc.orientation = unit->GetOrientation();
        }

        args.flags = MoveSplineFlag::Done;
        unit->m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_FORWARD);
        move_spline.onTransport = transport;
        move_spline.Initialize(args);

        MonsterMove packet;
        packet.MoverGUID = unit->GetGUID();
        packet.Pos = loc;
        packet.SplineData.StopDistanceTolerance = 2;
        packet.SplineData.ID = move_spline.GetId();

        if (transport)
        {
            packet.SplineData.Move.TransportGUID = unit->GetTransGUID();
            packet.SplineData.Move.VehicleSeat = unit->GetTransSeat();
        }

        WorldPacket l_Data(SMSG_MONSTER_MOVE);
        packet.Write(&l_Data);

        unit->SendMessageToSet(&l_Data, true);
    }

    MoveSplineInit::MoveSplineInit(Unit* m) : unit(m)
    {
        args.splineId = splineIdGen.NewId();
        // Elevators also use MOVEMENTFLAG_ONTRANSPORT but we do not keep track of their position changes
        args.TransformForTransport = unit->GetTransGUID() != 0;
        // mix existing state into new
        args.flags.canSwim = unit->CanSwim();
        args.walk = unit->HasUnitMovementFlag(MOVEMENTFLAG_WALKING);
        args.flags.flying = unit->HasUnitMovementFlag(MovementFlags(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_DISABLE_GRAVITY));
        args.flags.smoothGroundPath = true; // enabled by default, CatmullRom mode or client config "pathSmoothing" will disable this
        args.flags.steering = unit->HasFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS + 1, NPCFlags2::UNIT_NPC_FLAG2_STEERING);
    }

    void MoveSplineInit::SetFacing(const Unit* target)
    {
        args.facing.angle = unit->GetAngle(target);
        args.facing.target = target->GetGUID();
        args.facing.type = MONSTER_MOVE_FACING_TARGET;
    }

    void MoveSplineInit::SetFacing(float angle)
    {
        if (args.TransformForTransport)
        {
            if (Unit* vehicle = unit->GetVehicleBase())
                angle -= vehicle->GetOrientation();
            else if (Transport* transport = unit->GetTransport())
                angle -= transport->GetOrientation();
        }

        args.facing.angle = G3D::wrap(angle, 0.0f, (float)G3D::twoPi());
        args.facing.type = MONSTER_MOVE_FACING_ANGLE;
    }

    void MoveSplineInit::MoveTo(const Vector3& dest, bool generatePath, bool forceDestination)
    {
        if (generatePath)
        {
            PathGenerator path(unit);
            bool result = path.CalculatePath(dest.x, dest.y, dest.z, forceDestination);
            if (result && !(path.GetPathType() & PATHFIND_NOPATH))
            {
                MovebyPath(path.GetPath());
                return;
            }
        }

        args.path_Idx_offset = 0;
        args.path.resize(2);
        TransportPathTransform transform(unit, args.TransformForTransport);
        args.path[1] = transform(dest);
    }

    void MoveSplineInit::SetFall()
    {
        args.flags.EnableFalling();
        args.flags.fallingSlow = unit->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_SLOW);
    }

    Vector3 TransportPathTransform::operator()(Vector3 input)
    {
        if (_transformForTransport)
        {
            if (TransportBase* transport = _owner->GetDirectTransport())
            {
                float l_O = 0;
                transport->CalculatePassengerOffset(input.x, input.y, input.z, l_O);
            }
        }

        return input;
    }
}
