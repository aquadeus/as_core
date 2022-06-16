////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITYSERVER_MOVESPLINEINIT_ARGS_H
#define TRINITYSERVER_MOVESPLINEINIT_ARGS_H

#include "MoveSplineFlag.h"
#include <G3D/Vector3.h>

class Unit;

namespace Movement
{
    typedef std::vector<Vector3> PointsArray;

    struct FacingInfo
    {
        G3D::Vector3 f;
        uint64 target;
        float angle;

        MonsterMoveType type;
        FacingInfo() : angle(0.0f), type(MONSTER_MOVE_NORMAL) { }
    };

    struct SplineSpellEffectExtraData
    {
        uint64 TargetGuid = 0;
        uint32 SpellVisualId = 0;
        uint32 ProgressCurveId = 0;
        uint32 ParabolicCurveId = 0;
    };

    struct MonsterSplineFilterKey
    {
        MonsterSplineFilterKey(int16 p_IDx, int16 p_Speed) : Idx(p_IDx), Speed(p_Speed) { }

        int16 Idx = 0;
        int16 Speed = 0;
    };

    struct MonsterSplineFilter
    {
        std::vector<MonsterSplineFilterKey> FilterKeys;
        uint8 FilterFlags = 0;
        float BaseSpeed = 0.0f;
        int16 StartOffset = 0;
        float DistToPrevFilterKey = 0.0f;
        int16 AddedToStart = 0;
    };

    struct MovementSpline
    {
        uint32 Flags = 0;     ///< Spline flags
        uint8 Face = 0;     ///< Movement direction (see MonsterMoveType enum)
        uint8 AnimTier = 0;
        uint32 TierTransStartTime = 0;
        int32 Elapsed = 0;
        uint32 MoveTime = 0;
        float JumpGravity = 0.0f;
        uint32 SpecialTime = 0;
        std::vector<G3D::Vector3> Points;   ///< Spline path
        uint8 Mode = 0;     ///< Spline mode - actually always 0 in this packet - Catmullrom mode appears only in SMSG_UPDATE_OBJECT. In this packet it is determined by flags
        uint8 VehicleExitVoluntary = 0;
        uint64 TransportGUID = 0;
        int8 VehicleSeat = -1;
        std::vector<G3D::Vector3> PackedDeltas;
        Optional<MonsterSplineFilter> SplineFilter;
        Optional<SplineSpellEffectExtraData> SpellEffectExtraData;
        float FaceDirection = 0.0f;
        uint64 FaceGUID = 0;
        G3D::Vector3 FaceSpot;
    };

    struct MoveSplineInitArgs
    {
        MoveSplineInitArgs(size_t path_capacity = 16) : path_Idx_offset(0), velocity(0.0f),
            parabolic_amplitude(0.0f), time_perc(0.0f), splineId(0), initialOrientation(0.0f),
            walk(false), HasVelocity(false), TransformForTransport(true)
        {
            path.reserve(path_capacity);
        }

        PointsArray path;
        FacingInfo facing;
        MoveSplineFlag flags;
        int32 path_Idx_offset;
        float velocity;
        float parabolic_amplitude;
        float time_perc;
        uint32 splineId;
        float initialOrientation;
        Optional<MonsterSplineFilter> m_MonsterSplineFilter;
        Optional<SplineSpellEffectExtraData> m_SpellEffectExtraData;
        bool walk;
        bool HasVelocity;
        bool TransformForTransport;

        /** Returns true to show that the arguments were configured correctly and MoveSpline initialization will succeed. */
        bool Validate(Unit* unit) const;

    private:
        bool _checkPathLengths() const;
    };
}

#endif // TRINITYSERVER_MOVESPLINEINIT_ARGS_H
