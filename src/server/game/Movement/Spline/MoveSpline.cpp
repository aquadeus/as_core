////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "MoveSpline.h"
#include "Log.h"
#include "Creature.h"
#include "Common.h"

#include <sstream>

namespace Movement{

Location MoveSpline::ComputePosition() const
{
    ASSERT(Initialized());

    float u = 1.0f;
    int32 seg_time = spline.length(point_Idx, point_Idx+1);
    if (seg_time > 0)
        u = (time_passed - spline.length(point_Idx)) / (float)seg_time;
    Location c;
    c.orientation = initialOrientation;
    spline.evaluate_percent(point_Idx, u, c);

    if (splineflags.animation)
        ;// MoveSplineFlag::Animation disables falling or parabolic movement
    else if (splineflags.parabolic)
        computeParabolicElevation(c.z);
    else if (splineflags.falling)
        computeFallElevation(c.z);

    if (splineflags.done && facing.type != MONSTER_MOVE_NORMAL)
    {
        if (facing.type == MONSTER_MOVE_FACING_ANGLE)
            c.orientation = facing.angle;
        else if (facing.type == MONSTER_MOVE_FACING_SPOT)
            c.orientation = std::atan2(facing.f.y - c.y, facing.f.x - c.x);
        //nothing to do for MoveSplineFlag::Final_Target flag
    }
    else
    {
        if (!splineflags.hasFlag(MoveSplineFlag::OrientationFixed | MoveSplineFlag::Falling | MoveSplineFlag::Unknown0))
        {
            Vector3 hermite;
            spline.evaluate_derivative(point_Idx, u, hermite);
            c.orientation = std::atan2(hermite.y, hermite.x);
        }

        if (splineflags.backward)
            c.orientation = -c.orientation;
    }
    return c;
}

void MoveSpline::computeParabolicElevation(float& el) const
{
    if (time_passed > effect_start_time)
    {
        float t_passedf = MSToSec(time_passed - effect_start_time);
        float t_durationf = MSToSec(Duration() - effect_start_time); //client use not modified duration here

        // -a*x*x + bx + c:
        //(dur * v3->z_acceleration * dt)/2 - (v3->z_acceleration * dt * dt)/2 + Z;
        el += (t_durationf - t_passedf) * 0.5f * vertical_acceleration * t_passedf;
    }
}

void MoveSpline::computeFallElevation(float& el) const
{
    float z_now = spline.getPoint(spline.first()).z - Movement::computeFallElevation(MSToSec(time_passed), false);
    float final_z = FinalDestination().z;
    el = std::max(z_now, final_z);
}

inline uint32 computeDuration(float length, float velocity)
{
    return SecToMS(length / velocity);
}

struct FallInitializer
{
    FallInitializer(float _start_elevation) : start_elevation(_start_elevation) { }
    float start_elevation;
    inline int32 operator()(Spline<int32>& s, int32 i)
    {
        return Movement::computeFallTime(start_elevation - s.getPoint(i+1).z, false) * 1000.0f;
    }
};

enum{
    minimal_duration = 1
};

struct CommonInitializer
{
    CommonInitializer(float _velocity) : velocityInv(1000.0f/_velocity), time(minimal_duration) { }
    float velocityInv;
    int32 time;
    inline int32 operator()(Spline<int32>& s, int32 i)
    {
        time += (s.SegLength(i) * velocityInv);
        return time;
    }
};

void MoveSpline::init_spline(const MoveSplineInitArgs& args)
{
    const SplineBase::EvaluationMode modes[2] = {SplineBase::ModeLinear, SplineBase::ModeCatmullrom};
    if (args.flags.cyclic)
    {
        uint32 cyclic_point = 0;
        // MoveSplineFlag::Enter_Cycle support dropped
        //if (splineflags & SPLINEFLAG_ENTER_CYCLE)
        //cyclic_point = 1;   // shouldn't be modified, came from client
        spline.init_cyclic_spline(&args.path[0], args.path.size(), modes[args.flags.isSmooth()], cyclic_point);
    }
    else
    {
        spline.init_spline(&args.path[0], args.path.size(), modes[args.flags.isSmooth()]);
    }

    // init spline timestamps
    if (splineflags.falling)
    {
        FallInitializer init(spline.getPoint(spline.first()).z);
        spline.initLengths(init);
    }
    else
    {
        CommonInitializer init(args.velocity);
        spline.initLengths(init);
    }

    /// @todo what to do in such cases? problem is in input data (all points are at same coords)
    if (spline.length() < minimal_duration)
    {
        sLog->outError(LOG_FILTER_GENERAL, "MoveSpline::init_spline: zero length spline, wrong input data?");
        spline.set_length(spline.last(), spline.isCyclic() ? 1000 : 1);
    }
    point_Idx = spline.first();
}

void MoveSpline::Initialize(MoveSplineInitArgs const& args)
{
    splineflags = args.flags;
    facing = args.facing;
    m_Id = args.splineId;
    point_Idx_offset = args.path_Idx_offset;
    initialOrientation = args.initialOrientation;
    m_MonsterSplineFilter = args.m_MonsterSplineFilter;
    m_SpellEffectExtraData = args.m_SpellEffectExtraData;
    time_passed = 0;
    vertical_acceleration = 0.0f;
    effect_start_time = 0;
    splineIsFacingOnly = args.path.size() == 2 && args.facing.type != MONSTER_MOVE_NORMAL && ((args.path[1] - args.path[0]).length() < 0.1f);

    // Check if its a stop spline
    if (args.flags.done)
    {
        spline.clear();
        return;
    }

    init_spline(args);

    // init parabolic / animation
    // spline initialized, duration known and i able to compute parabolic acceleration
    if (args.flags & (MoveSplineFlag::Parabolic | MoveSplineFlag::Animation | MoveSplineFlag::FadeObject))
    {
        effect_start_time = Duration() * args.time_perc;
        if (args.flags.parabolic && effect_start_time < Duration())
        {
            float f_duration = MSToSec(Duration() - effect_start_time);
            vertical_acceleration = args.parabolic_amplitude * 8.0f / (f_duration * f_duration);
        }
    }
}

MoveSpline::MoveSpline() : m_Id(0), time_passed(0),
    vertical_acceleration(0.0f), initialOrientation(0.0f), effect_start_time(0), point_Idx(0), point_Idx_offset(0),
    onTransport(false), splineIsFacingOnly(false)
{
    splineflags.done = true;
}

/// ============================================================================================

bool MoveSplineInitArgs::Validate(Unit* unit) const
{
#define CHECK(exp) \
    if (!(exp))\
    {\
        return false;\
    }
    CHECK(path.size() > 1);
    CHECK(velocity > 0.1f);
    CHECK(time_perc >= 0.0f && time_perc <= 1.0f);
    //CHECK(_checkPathLengths());
    return true;
#undef CHECK
}

// check path lengths - why are we even starting such short movement?
bool MoveSplineInitArgs::_checkPathLengths() const
{
    if (path.size() > 2 || facing.type == MONSTER_MOVE_NORMAL)
        for (uint32 i = 0; i < path.size() - 1; ++i)
            if ((path[i + 1] - path[i]).length() < 0.1f)
                return false;
    return true;
}

/// ============================================================================================

MoveSpline::UpdateResult MoveSpline::_updateState(int32& ms_time_diff)
{
    if (Finalized())
    {
        ms_time_diff = 0;
        return Result_Arrived;
    }

    UpdateResult result = Result_None;

    int32 minimal_diff = std::min(ms_time_diff, segment_time_elapsed());
    ASSERT(minimal_diff >= 0);
    time_passed += minimal_diff;
    ms_time_diff -= minimal_diff;

    if (time_passed >= next_timestamp())
    {
        ++point_Idx;
        if (point_Idx < spline.last())
        {
            result = Result_NextSegment;
        }
        else
        {
            if (spline.isCyclic())
            {
                point_Idx = spline.first();
                time_passed = time_passed % Duration();
                result = Movement::MoveSpline::UpdateResult(Result_NextCycle | Result_JustArrived);
            }
            else
            {
                _Finalize();
                ms_time_diff = 0;
                result = Movement::MoveSpline::UpdateResult(Result_Arrived | Result_JustArrived);
            }
        }
    }

    return result;
}

std::string MoveSpline::ToString() const
{
    std::stringstream str;
    str << "MoveSpline" << std::endl;
    str << "spline Id: " << GetId() << std::endl;
    str << "flags: " << splineflags.ToString() << std::endl;
    if (facing.type == MONSTER_MOVE_FACING_ANGLE)
        str << "facing  angle: " << facing.angle;
    else if (facing.type == MONSTER_MOVE_FACING_TARGET)
        str << "facing target: " << facing.target;
    else if (facing.type == MONSTER_MOVE_FACING_SPOT)
        str << "facing  point: " << facing.f.x << " " << facing.f.y << " " << facing.f.z;
    str << std::endl;
    str << "time passed: " << time_passed << std::endl;
    str << "total  time: " << Duration() << std::endl;
    str << "spline point Id: " << point_Idx << std::endl;
    str << "path  point  Id: " << currentPathIdx() << std::endl;
    str << spline.ToString();
    return str.str();
}

void MoveSpline::_Finalize()
{
    splineflags.done = true;
    point_Idx = spline.last() - 1;
    time_passed = Duration();
}

int32 MoveSpline::currentPathIdx() const
{
    int32 point = point_Idx_offset + point_Idx - spline.first() + (int)Finalized();
    if (isCyclic())
        point = point % (spline.last()-spline.first());
    return point;
}
}
