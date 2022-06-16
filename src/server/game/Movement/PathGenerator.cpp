////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "PathGenerator.h"
#include "Map.h"
#include "Creature.h"
#include "MMapFactory.h"
#include "MMapManager.h"
#include "Log.h"
#include "DisableMgr.h"
#include "DetourCommon.h"
#include "DetourNavMeshQuery.h"
#include "MoveSpline.h"

std::map<uint32, std::vector<AreaPolygon>> PathGenerator::m_ShortcutAreas;

////////////////// PathGenerator //////////////////
PathGenerator::PathGenerator(const Unit* owner) :
    _polyLength(0), _type(PATHFIND_BLANK), _useStraightPath(false),
    _forceDestination(false), _pointPathLimit(MAX_POINT_PATH_LENGTH), _straightLine(false),
    _endPosition(G3D::Vector3::zero()), _sourceUnit(owner), _navMesh(NULL),
    _navMeshQuery(NULL)
{
    memset(_pathPolyRefs, 0, sizeof(_pathPolyRefs));

    //sLog->outDebug(LOG_FILTER_MAPS, "++ PathGenerator::PathGenerator for %llu", _sourceUnit->GetGUID());

    uint32 mapId = _sourceUnit->GetMapId();
    if (DisableMgr::IsPathfindingEnabled(_sourceUnit->GetMapId()))
    {
        MMAP::MMapManager* mmap = MMAP::MMapFactory::createOrGetMMapManager();

        TRINITY_READ_GUARD(ACE_RW_Thread_Mutex, mmap->GetManagerLock());

        _navMesh = mmap->GetNavMesh(mapId);

        uint32 l_InstanceId = _sourceUnit->GetInstanceId();
        if (!_sourceUnit->GetMap()->Instanceable())
            l_InstanceId = _sourceUnit->GetMap()->GetInstanceZoneId();

        _navMeshQuery = mmap->GetNavMeshQuery(mapId, l_InstanceId);
    }

    CreateFilter();
}

PathGenerator::~PathGenerator()
{
    //sLog->outDebug(LOG_FILTER_MAPS, "++ PathGenerator::~PathGenerator() for %llu", _sourceUnit->GetGUID());
}

bool PathGenerator::CalculatePath(float destX, float destY, float destZ, bool forceDest /*= false*/, bool straightLine /*= false*/, Unit* p_TargetUnit /*= nullptr*/, bool useCombatReach /*= false*/)
{
    float x, y, z;

    if (_sourceUnit->movespline->Initialized() && !_sourceUnit->movespline->Finalized())
    {
        Movement::Location l_Computed = _sourceUnit->movespline->ComputePosition();
        x = l_Computed.x;
        y = l_Computed.y;
        z = l_Computed.z;
    }
    else
    _sourceUnit->GetPosition(x, y, z);

    if (!JadeCore::IsValidMapCoord(destX, destY, destZ) || !JadeCore::IsValidMapCoord(x, y, z))
        return false;

    /// PathGenerator owner map can change over the time
    /// We need need to update it to prevent concurrent use of _navMeshQuery
    /// dtNavMeshQuery isn't thread-safe and we have one _navMeshQuery per map/thread
    UpdateNavMeshQuery();
    if (p_TargetUnit)
    {
        /// HACK:
        /// There are many creatures having boundingradius < combatreach. It makes player to charge to the center of the creature's model.
        /// For example, Patchwerk has 2 boundingradius and 8 combatreach. The player should charge to 8 but not to 2.
        float l_Radius = p_TargetUnit->GetFloatValue(UNIT_FIELD_BOUNDING_RADIUS);
        if (useCombatReach && l_Radius < p_TargetUnit->GetFloatValue(UNIT_FIELD_COMBAT_REACH))
            l_Radius = p_TargetUnit->GetFloatValue(UNIT_FIELD_COMBAT_REACH);

        float l_Distance = (*_sourceUnit).GetExactDist2d(p_TargetUnit) - l_Radius - _sourceUnit->GetFloatValue(UNIT_FIELD_BOUNDING_RADIUS);

        if (l_Distance > 0.0f)
        {
            float l_Angle = (*_sourceUnit).GetAngle(p_TargetUnit);

            float l_X = l_Distance * std::cos(l_Angle);
            float l_Y = l_Distance * std::sin(l_Angle);

            destX = _sourceUnit->m_positionX + l_X;
            destY = _sourceUnit->m_positionY + l_Y;
        }
        // Don't need to move if distance <= 0.f
        // 'useCombatReach' is for charges now
        else if (useCombatReach)
        {
            destX = _sourceUnit->m_positionX;
            destY = _sourceUnit->m_positionY;
            return true;
        }
    }

    G3D::Vector3 dest(destX, destY, destZ);
    SetEndPosition(dest);

    G3D::Vector3 start(x, y, z);
    SetStartPosition(start);

    _forceDestination = forceDest;
    _straightLine = straightLine;

    //sLog->outDebug(LOG_FILTER_MAPS, "++ PathGenerator::CalculatePath() for %llu", _sourceUnit->GetGUID());

    // make sure navMesh works - we can run on map w/o mmap
    // check if the start and end point have a .mmtile loaded (can we pass via not loaded tile on the way?)
    if (!_sourceUnit->HasUnitState(UNIT_STATE_ONVEHICLE) || (!_navMesh || !_navMeshQuery)) /// End Position become 0 0 0 on vehicle passager, so GetHeight return the height on 0 0 0 in the map, make player undermap in some map
    {
        if (!_navMesh || !_navMeshQuery || _sourceUnit->HasUnitState(UNIT_STATE_IGNORE_PATHFINDING) || (_sourceUnit->GetTypeId() == TYPEID_UNIT && _sourceUnit->ToCreature()->GetFlagsExtra() & CREATURE_FLAG_EXTRA_IGNORE_PATHFINDING) ||
            !HaveTile(start) || !HaveTile(dest) || NeedToUseShortcut(start, dest, _sourceUnit->GetMapId()))
        {
            BuildShortcut();
            _type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
            return true;
        }
    }

    UpdateFilter();

    /// dtNavMeshQuery isn't thread-safe
    const_cast<dtNavMeshQuery*>(_navMeshQuery)->Lock();
    {
        BuildPolyPath(start, dest);
    }
    const_cast<dtNavMeshQuery*>(_navMeshQuery)->Unlock();

    return true;
}

dtPolyRef PathGenerator::GetPathPolyByPosition(dtPolyRef const* polyPath, uint32 polyPathSize, float const* point, float* distance) const
{
    if (!polyPath || !polyPathSize)
        return INVALID_POLYREF;

    dtPolyRef nearestPoly = INVALID_POLYREF;
    float minDist2d = FLT_MAX;
    float minDist3d = 0.0f;

    for (uint32 i = 0; i < polyPathSize; ++i)
    {
        float closestPoint[VERTEX_SIZE];
        if (dtStatusFailed(_navMeshQuery->closestPointOnPoly(polyPath[i], point, closestPoint, NULL)))
            continue;

        float d = dtVdist2DSqr(point, closestPoint);
        if (d < minDist2d)
        {
            minDist2d = d;
            nearestPoly = polyPath[i];
            minDist3d = dtVdistSqr(point, closestPoint);
        }

        if (minDist2d < 1.0f) // shortcut out - close enough for us
            break;
    }

    if (distance)
        *distance = dtMathSqrtf(minDist3d);

    return (minDist2d < 3.0f) ? nearestPoly : INVALID_POLYREF;
}

dtPolyRef PathGenerator::GetPolyByLocation(float const* point, float* distance) const
{
    // first we check the current path
    // if the current path doesn't contain the current poly,
    // we need to use the expensive navMesh.findNearestPoly
    dtPolyRef polyRef = GetPathPolyByPosition(_pathPolyRefs, _polyLength, point, distance);
    if (polyRef != INVALID_POLYREF)
        return polyRef;

    // we don't have it in our old path
    // try to get it by findNearestPoly()
    // first try with low search box
    float extents[VERTEX_SIZE] = {3.0f, 5.0f, 3.0f};    // bounds of poly search area
    float closestPoint[VERTEX_SIZE] = {0.0f, 0.0f, 0.0f};
    if (dtStatusSucceed(_navMeshQuery->findNearestPoly(point, extents, &_filter, &polyRef, closestPoint)) && polyRef != INVALID_POLYREF)
    {
        *distance = dtVdist(closestPoint, point);
        return polyRef;
    }

    // still nothing ..
    // try with bigger search box
    // Note that the extent should not overlap more than 128 polygons in the navmesh (see dtNavMeshQuery::findNearestPoly)

    std::array<float, 6> l_Exents = { { 50.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f } };
    for (float l_Extent : l_Exents)
    {
        extents[1] = l_Extent;

        if (dtStatusSucceed(_navMeshQuery->findNearestPoly(point, extents, &_filter, &polyRef, closestPoint)) && polyRef != INVALID_POLYREF)
        {
            *distance = dtVdist(closestPoint, point);
            return polyRef;
        }
    }

    return INVALID_POLYREF;
}

void PathGenerator::BuildPolyPath(G3D::Vector3 const& startPos, G3D::Vector3 const& endPos)
{
    // *** getting start/end poly logic ***

    float distToStartPoly, distToEndPoly;
    float startPoint[VERTEX_SIZE] = {startPos.y, startPos.z+0.5f, startPos.x};
    float endPoint[VERTEX_SIZE] = {endPos.y, endPos.z+0.5f, endPos.x};

    dtPolyRef startPoly = GetPolyByLocation(startPoint, &distToStartPoly);
    dtPolyRef endPoly = GetPolyByLocation(endPoint, &distToEndPoly);

    // we have a hole in our mesh
    // make shortcut path and mark it as NOPATH ( with flying and swimming exception )
    // its up to caller how he will use this info
    if (startPoly == INVALID_POLYREF || endPoly == INVALID_POLYREF)
    {
        //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: (startPoly == 0 || endPoly == 0)\n");
        BuildShortcut();
        bool path = _sourceUnit->GetTypeId() == TYPEID_UNIT && _sourceUnit->ToCreature()->CanFly();

        bool waterPath = _sourceUnit->GetTypeId() == TYPEID_UNIT && _sourceUnit->ToCreature()->CanSwim();
        if (waterPath)
        {
            // Check both start and end points, if they're both in water, then we can *safely* let the creature move
            for (uint32 i = 0; i < _pathPoints.size(); ++i)
            {
                ZLiquidStatus status = _sourceUnit->GetBaseMap()->getLiquidStatus(_pathPoints[i].x, _pathPoints[i].y, _pathPoints[i].z, MAP_ALL_LIQUIDS, NULL);
                // One of the points is not in the water, cancel movement.
                if (status == LIQUID_MAP_NO_WATER)
                {
                    waterPath = false;
                    break;
                }
            }
        }

        _type = (path || waterPath) ? PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH) : PATHFIND_NOPATH;
        return;
    }

    // we may need a better number here
    bool farFromPoly = (distToStartPoly > 7.0f || distToEndPoly > 7.0f);
    if (farFromPoly)
    {
        //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: farFromPoly distToStartPoly=%.3f distToEndPoly=%.3f\n", distToStartPoly, distToEndPoly);

        bool buildShotrcut = false;
        if (_sourceUnit->GetTypeId() == TYPEID_UNIT)
        {
            Creature* owner = (Creature*)_sourceUnit;

            if (owner->CanFly() || owner->CanSwim())
                buildShotrcut = true;

            /*G3D::Vector3 const& p = (distToStartPoly > 7.0f) ? startPos : endPos;
            if (_sourceUnit->GetBaseMap()->IsUnderWater(p.x, p.y, p.z))
            {
                //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: underWater case\n");
                if (owner->canSwim())
                    buildShotrcut = true;
            }
            else
            {
                //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: flying case\n");
                if (owner->CanFly())
                    buildShotrcut = true;
            }*/
        }

        if (buildShotrcut)
        {
            BuildShortcut();
            _type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
            return;
        }
        else
        {
            float closestPoint[VERTEX_SIZE];
            // we may want to use closestPointOnPolyBoundary instead
            if (dtStatusSucceed(_navMeshQuery->closestPointOnPoly(endPoly, endPoint, closestPoint, NULL)))
            {
                dtVcopy(endPoint, closestPoint);
                SetActualEndPosition(G3D::Vector3(endPoint[2], endPoint[0] + 0.5f, endPoint[1] + 0.5f));
            }

            _type = PATHFIND_INCOMPLETE;
        }
    }

    // *** poly path generating logic ***

    // start and end are on same polygon
    // just need to move in straight line
    if (startPoly == endPoly)
    {
        //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: (startPoly == endPoly)\n");

        BuildShortcut();

        _pathPolyRefs[0] = startPoly;
        _polyLength = 1;

        _type = farFromPoly ? PATHFIND_INCOMPLETE : PATHFIND_NORMAL;
        //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: path type %d\n", _type);
        return;
    }

    // look for startPoly/endPoly in current path
    /// @todo we can merge it with getPathPolyByPosition() loop
    bool startPolyFound = false;
    bool endPolyFound = false;
    uint32 pathStartIndex = 0;
    uint32 pathEndIndex = 0;

    if (_polyLength)
    {
        for (; pathStartIndex < _polyLength; ++pathStartIndex)
        {
            // here to catch few bugs
            if (_pathPolyRefs[pathStartIndex] == INVALID_POLYREF)
            {
                sLog->outError(LOG_FILTER_MAPS, "Invalid poly ref in BuildPolyPath. _polyLength: %u, pathStartIndex: %u,"
                                     " startPos: %s, endPos: %s, mapid: %u",
                                     _polyLength, pathStartIndex, startPos.toString().c_str(), endPos.toString().c_str(),
                                     _sourceUnit->GetMapId());

                break;
            }

            if (_pathPolyRefs[pathStartIndex] == startPoly)
            {
                startPolyFound = true;
                break;
            }
        }

        for (pathEndIndex = _polyLength-1; pathEndIndex > pathStartIndex; --pathEndIndex)
            if (_pathPolyRefs[pathEndIndex] == endPoly)
            {
                endPolyFound = true;
                break;
            }
    }

    if (startPolyFound && endPolyFound)
    {
        //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: (startPolyFound && endPolyFound)\n");

        // we moved along the path and the target did not move out of our old poly-path
        // our path is a simple subpath case, we have all the data we need
        // just "cut" it out

        _polyLength = pathEndIndex - pathStartIndex + 1;
        memmove(_pathPolyRefs, _pathPolyRefs + pathStartIndex, _polyLength * sizeof(dtPolyRef));
    }
    else if (startPolyFound && !endPolyFound)
    {
        //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: (startPolyFound && !endPolyFound)\n");

        // we are moving on the old path but target moved out
        // so we have atleast part of poly-path ready

        _polyLength -= pathStartIndex;

        // try to adjust the suffix of the path instead of recalculating entire length
        // at given interval the target cannot get too far from its last location
        // thus we have less poly to cover
        // sub-path of optimal path is optimal

        // take ~80% of the original length
        /// @todo play with the values here
        uint32 prefixPolyLength = uint32(_polyLength * 0.8f + 0.5f);
        memmove(_pathPolyRefs, _pathPolyRefs+pathStartIndex, prefixPolyLength * sizeof(dtPolyRef));

        dtPolyRef suffixStartPoly = _pathPolyRefs[prefixPolyLength-1];

        // we need any point on our suffix start poly to generate poly-path, so we need last poly in prefix data
        float suffixEndPoint[VERTEX_SIZE];
        if (dtStatusFailed(_navMeshQuery->closestPointOnPoly(suffixStartPoly, endPoint, suffixEndPoint, NULL)))
        {
            // we can hit offmesh connection as last poly - closestPointOnPoly() don't like that
            // try to recover by using prev polyref
            --prefixPolyLength;
            suffixStartPoly = _pathPolyRefs[prefixPolyLength-1];
            if (dtStatusFailed(_navMeshQuery->closestPointOnPoly(suffixStartPoly, endPoint, suffixEndPoint, NULL)))
            {
                // suffixStartPoly is still invalid, error state
                BuildShortcut();
                _type = PATHFIND_NOPATH;
                return;
            }
        }

        // generate suffix
        uint32 suffixPolyLength = 0;

        dtStatus dtResult;
        if (_straightLine)
        {
            float hit = 0;
            float hitNormal[3];
            memset(hitNormal, 0, sizeof(hitNormal));

            dtResult = _navMeshQuery->raycast(
                            suffixStartPoly,
                            suffixEndPoint,
                            endPoint,
                            &_filter,
                            &hit,
                            hitNormal,
                            _pathPolyRefs + prefixPolyLength - 1,
                            (int*)&suffixPolyLength,
                            MAX_PATH_LENGTH - prefixPolyLength);

            // raycast() sets hit to FLT_MAX if there is a ray between start and end
            if (hit != FLT_MAX)
            {
                // the ray hit something, return no path instead of the incomplete one
                _type = PATHFIND_NOPATH;
                return;
            }
        }
        else
        {
            dtResult = _navMeshQuery->findPath(
                            suffixStartPoly,    // start polygon
                            endPoly,            // end polygon
                            suffixEndPoint,     // start position
                            endPoint,           // end position
                            &_filter,            // polygon search filter
                            _pathPolyRefs + prefixPolyLength - 1,    // [out] path
                            (int*)&suffixPolyLength,
                            MAX_PATH_LENGTH - prefixPolyLength);   // max number of polygons in output path
        }

        if (!suffixPolyLength || dtStatusFailed(dtResult))
        {
            // this is probably an error state, but we'll leave it
            // and hopefully recover on the next Update
            // we still need to copy our preffix
            sLog->outError(LOG_FILTER_MAPS, "%lu's Path Build failed: 0 length path", _sourceUnit->GetGUID());
        }

        //sLog->outDebug(LOG_FILTER_MAPS, "++  m_polyLength=%u prefixPolyLength=%u suffixPolyLength=%u \n", _polyLength, prefixPolyLength, suffixPolyLength);

        // new path = prefix + suffix - overlap
        _polyLength = prefixPolyLength + suffixPolyLength - 1;
    }
    else
    {
        //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildPolyPath :: (!startPolyFound && !endPolyFound)\n");

        // either we have no path at all -> first run
        // or something went really wrong -> we aren't moving along the path to the target
        // just generate new path

        // free and invalidate old path data
        Clear();

        dtStatus dtResult;
        if (_straightLine)
        {
            float hit = 0;
            float hitNormal[3];
            memset(hitNormal, 0, sizeof(hitNormal));

            dtResult = _navMeshQuery->raycast(
                            startPoly,
                            startPoint,
                            endPoint,
                            &_filter,
                            &hit,
                            hitNormal,
                            _pathPolyRefs,
                            (int*)&_polyLength,
                            MAX_PATH_LENGTH);

            // raycast() sets hit to FLT_MAX if there is a ray between start and end
            if (hit != FLT_MAX)
            {
                // the ray hit something, return no path instead of the incomplete one
                _type = PATHFIND_NOPATH;
                return;
            }
        }
        else
        {
            dtResult = _navMeshQuery->findPath(
                            startPoly,          // start polygon
                            endPoly,            // end polygon
                            startPoint,         // start position
                            endPoint,           // end position
                            &_filter,           // polygon search filter
                            _pathPolyRefs,     // [out] path
                            (int*)&_polyLength,
                            MAX_PATH_LENGTH);   // max number of polygons in output path
        }

        if (!_polyLength || dtStatusFailed(dtResult))
        {
            // only happens if we passed bad data to findPath(), or navmesh is messed up
            sLog->outError(LOG_FILTER_MAPS, "%lu's Path Build failed: 0 length path", _sourceUnit->GetGUID());
            BuildShortcut();
            _type = PATHFIND_NOPATH;
            return;
        }
    }

    // by now we know what type of path we can get
    if (_pathPolyRefs[_polyLength - 1] == endPoly && !(_type & PATHFIND_INCOMPLETE))
        _type = PATHFIND_NORMAL;
    else
        _type = PATHFIND_INCOMPLETE;

    // generate the point-path out of our up-to-date poly-path
    BuildPointPath(startPoint, endPoint);
}

void PathGenerator::BuildPointPath(const float *startPoint, const float *endPoint)
{
    float pathPoints[MAX_POINT_PATH_LENGTH*VERTEX_SIZE];
    uint32 pointCount = 0;
    dtStatus dtResult = DT_FAILURE;
    if (_straightLine)
    {
        dtResult = DT_SUCCESS;
        pointCount = 1;
        memcpy(&pathPoints[VERTEX_SIZE * 0], startPoint, sizeof(float)* 3); // first point

        // path has to be split into polygons with dist SMOOTH_PATH_STEP_SIZE between them
        G3D::Vector3 startVec = G3D::Vector3(startPoint[0], startPoint[1], startPoint[2]);
        G3D::Vector3 endVec = G3D::Vector3(endPoint[0], endPoint[1], endPoint[2]);
        G3D::Vector3 diffVec = (endVec - startVec);
        G3D::Vector3 prevVec = startVec;
        float len = diffVec.length();
        diffVec *= SMOOTH_PATH_STEP_SIZE / len;
        while (len > SMOOTH_PATH_STEP_SIZE)
        {
            len -= SMOOTH_PATH_STEP_SIZE;
            prevVec += diffVec;
            pathPoints[VERTEX_SIZE * pointCount + 0] = prevVec.x;
            pathPoints[VERTEX_SIZE * pointCount + 1] = prevVec.y;
            pathPoints[VERTEX_SIZE * pointCount + 2] = prevVec.z + 0.5f;
            ++pointCount;
        }

        memcpy(&pathPoints[VERTEX_SIZE * pointCount], endPoint, sizeof(float)* 3); // last point
        ++pointCount;
    }
    else if (_useStraightPath)
    {
        dtResult = _navMeshQuery->findStraightPath(
                startPoint,         // start position
                endPoint,           // end position
                _pathPolyRefs,     // current path
                _polyLength,       // lenth of current path
                pathPoints,         // [out] path corner points
                NULL,               // [out] flags
                NULL,               // [out] shortened path
                (int*)&pointCount,
                _pointPathLimit);   // maximum number of points/polygons to use
    }
    else
    {
        dtResult = FindSmoothPath(
                startPoint,         // start position
                endPoint,           // end position
                _pathPolyRefs,     // current path
                _polyLength,       // length of current path
                pathPoints,         // [out] path corner points
                (int*)&pointCount,
                _pointPathLimit);    // maximum number of points
    }

    if (pointCount < 2 || dtStatusFailed(dtResult))
    {
        // only happens if pass bad data to findStraightPath or navmesh is broken
        // single point paths can be generated here
        /// @todo check the exact cases
        //sLog->outDebug(LOG_FILTER_MAPS, "++ PathGenerator::BuildPointPath FAILED! path sized %d returned\n", pointCount);
        BuildShortcut();
        _type = PATHFIND_NOPATH;
        return;
    }
    else if (pointCount == _pointPathLimit)
    {
        //sLog->outDebug(LOG_FILTER_MAPS, "++ PathGenerator::BuildPointPath FAILED! path sized %d returned, lower than limit set to %d\n", pointCount, _pointPathLimit);
        BuildShortcut();
        _type = PATHFIND_SHORT;
        return;
    }

    _pathPoints.resize(pointCount);
    for (uint32 i = 0; i < pointCount; ++i)
        _pathPoints[i] = G3D::Vector3(pathPoints[i*VERTEX_SIZE+2], pathPoints[i*VERTEX_SIZE], pathPoints[i*VERTEX_SIZE+1] + 0.5f);

    NormalizePath();

    // first point is always our current location - we need the next one
    SetActualEndPosition(_pathPoints[pointCount-1]);

    // force the given destination, if needed
    if (_forceDestination &&
        (!(_type & PATHFIND_NORMAL) || !InRange(GetEndPosition(), GetActualEndPosition(), 1.0f, 1.0f)))
    {
        // we may want to keep partial subpath
        if (Dist3DSqr(GetActualEndPosition(), GetEndPosition()) < 0.3f * Dist3DSqr(GetStartPosition(), GetEndPosition()))
        {
            SetActualEndPosition(GetEndPosition());
            _pathPoints[_pathPoints.size()-1] = GetEndPosition();
        }
        else
        {
            SetActualEndPosition(GetEndPosition());
            BuildShortcut();
        }

        _type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
    }

    //sLog->outDebug(LOG_FILTER_MAPS, "++ PathGenerator::BuildPointPath path type %d size %d poly-size %d\n", _type, pointCount, _polyLength);
}

void PathGenerator::NormalizePath()
{
    for (uint32 i = 0; i < _pathPoints.size(); ++i)
        _sourceUnit->UpdateAllowedPositionZ(_pathPoints[i].x, _pathPoints[i].y, _pathPoints[i].z);
}

void PathGenerator::BuildShortcut()
{
    //sLog->outDebug(LOG_FILTER_MAPS, "++ BuildShortcut :: making shortcut\n");

    Clear();

    // make two point path, our curr pos is the start, and dest is the end
    _pathPoints.resize(2);

    // set start and a default next position
    _pathPoints[0] = GetStartPosition();
    _pathPoints[1] = GetActualEndPosition();

    NormalizePath();

    _type = PATHFIND_SHORTCUT;
}

void PathGenerator::CreateFilter()
{
    uint16 includeFlags = 0;
    uint16 excludeFlags = 0;

    if (_sourceUnit->GetTypeId() == TYPEID_UNIT)
    {
        Creature* creature = (Creature*)_sourceUnit;
        if (creature->canWalk())
            includeFlags |= NAV_GROUND;          // walk

        // creatures don't take environmental damage
        if (creature->CanSwim())
            includeFlags |= (NAV_WATER | NAV_MAGMA_SLIME);           // swim
    }
    else // assume Player
    {
        // perfect support not possible, just stay 'safe'
        includeFlags |= (NAV_GROUND | NAV_WATER | NAV_MAGMA_SLIME);
    }

    _filter.setIncludeFlags(includeFlags);
    _filter.setExcludeFlags(excludeFlags);

    UpdateFilter();
}

void PathGenerator::UpdateFilter()
{
    // allow creatures to cheat and use different movement types if they are moved
    // forcefully into terrain they can't normally move in
    if (_sourceUnit->IsInWater() || _sourceUnit->IsUnderWater())
    {
        uint16 includedFlags = _filter.getIncludeFlags();
        includedFlags |= GetNavTerrain(_sourceUnit->GetPositionX(),
                                       _sourceUnit->GetPositionY(),
                                       _sourceUnit->GetPositionZ());

        _filter.setIncludeFlags(includedFlags);
    }
}

NavTerrainFlag PathGenerator::GetNavTerrain(float x, float y, float z)
{
    LiquidData data;
    ZLiquidStatus liquidStatus = _sourceUnit->GetBaseMap()->getLiquidStatus(x, y, z, MAP_ALL_LIQUIDS, &data);
    if (liquidStatus == LIQUID_MAP_NO_WATER)
        return NAV_GROUND;

    switch (data.type_flags)
    {
        case MAP_LIQUID_TYPE_WATER:
        case MAP_LIQUID_TYPE_OCEAN:
            return NAV_WATER;
        case MAP_LIQUID_TYPE_MAGMA:
        case MAP_LIQUID_TYPE_SLIME:
            return NAV_MAGMA_SLIME;
        default:
            return NAV_GROUND;
    }
}

bool PathGenerator::HaveTile(const G3D::Vector3& p) const
{
    int tx = -1, ty = -1;
    float point[VERTEX_SIZE] = {p.y, p.z, p.x};

    _navMesh->calcTileLoc(point, &tx, &ty);

    /// Workaround
    /// For some reason, often the tx and ty variables wont get a valid value
    /// Use this check to prevent getting negative tile coords and crashing on getTileAt
    if (tx < 0 || ty < 0)
        return false;

    return (_navMesh->getTileAt(tx, ty, 0) != NULL);
}

uint32 PathGenerator::FixupCorridor(dtPolyRef* path, uint32 npath, uint32 maxPath, dtPolyRef const* visited, uint32 nvisited)
{
    int32 furthestPath = -1;
    int32 furthestVisited = -1;

    // Find furthest common polygon.
    for (int32 i = npath-1; i >= 0; --i)
    {
        bool found = false;
        for (int32 j = nvisited-1; j >= 0; --j)
        {
            if (path[i] == visited[j])
            {
                furthestPath = i;
                furthestVisited = j;
                found = true;
            }
        }
        if (found)
            break;
    }

    // If no intersection found just return current path.
    if (furthestPath == -1 || furthestVisited == -1)
        return npath;

    // Concatenate paths.

    // Adjust beginning of the buffer to include the visited.
    uint32 req = nvisited - furthestVisited;
    uint32 orig = uint32(furthestPath + 1) < npath ? furthestPath + 1 : npath;
    uint32 size = npath > orig ? npath - orig : 0;
    if (req + size > maxPath)
        size = maxPath-req;

    if (size)
        memmove(path + req, path + orig, size * sizeof(dtPolyRef));

    // Store visited
    for (uint32 i = 0; i < req; ++i)
        path[i] = visited[(nvisited - 1) - i];

    return req+size;
}

bool PathGenerator::GetSteerTarget(float const* startPos, float const* endPos,
                              float minTargetDist, dtPolyRef const* path, uint32 pathSize,
                              float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef)
{
    // Find steer target.
    static const uint32 MAX_STEER_POINTS = 3;
    float steerPath[MAX_STEER_POINTS*VERTEX_SIZE];
    unsigned char steerPathFlags[MAX_STEER_POINTS];
    dtPolyRef steerPathPolys[MAX_STEER_POINTS];
    uint32 nsteerPath = 0;
    dtStatus dtResult = _navMeshQuery->findStraightPath(startPos, endPos, path, pathSize,
                                                steerPath, steerPathFlags, steerPathPolys, (int*)&nsteerPath, MAX_STEER_POINTS);
    if (!nsteerPath || dtStatusFailed(dtResult))
        return false;

    // Find vertex far enough to steer to.
    uint32 ns = 0;
    while (ns < nsteerPath)
    {
        // Stop at Off-Mesh link or when point is further than slop away.
        if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
            !InRangeYZX(&steerPath[ns*VERTEX_SIZE], startPos, minTargetDist, 1000.0f))
            break;
        ns++;
    }
    // Failed to find good point to steer to.
    if (ns >= nsteerPath)
        return false;

    dtVcopy(steerPos, &steerPath[ns*VERTEX_SIZE]);
    steerPos[1] = startPos[1];  // keep Z value
    steerPosFlag = steerPathFlags[ns];
    steerPosRef = steerPathPolys[ns];

    return true;
}

dtStatus PathGenerator::FindSmoothPath(float const* startPos, float const* endPos,
                                     dtPolyRef const* polyPath, uint32 polyPathSize,
                                     float* smoothPath, int* smoothPathSize, uint32 maxSmoothPathSize)
{
    *smoothPathSize = 0;
    uint32 nsmoothPath = 0;

    dtPolyRef polys[MAX_PATH_LENGTH];
    memcpy(polys, polyPath, sizeof(dtPolyRef)*polyPathSize);
    uint32 npolys = polyPathSize;

    float iterPos[VERTEX_SIZE], targetPos[VERTEX_SIZE];
    if (dtStatusFailed(_navMeshQuery->closestPointOnPolyBoundary(polys[0], startPos, iterPos)))
        return DT_FAILURE;

    if (dtStatusFailed(_navMeshQuery->closestPointOnPolyBoundary(polys[npolys-1], endPos, targetPos)))
        return DT_FAILURE;

    dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
    nsmoothPath++;

    // Move towards target a small advancement at a time until target reached or
    // when ran out of memory to store the path.
    while (npolys && nsmoothPath < maxSmoothPathSize)
    {
        // Find location to steer towards.
        float steerPos[VERTEX_SIZE];
        unsigned char steerPosFlag;
        dtPolyRef steerPosRef = INVALID_POLYREF;

        if (!GetSteerTarget(iterPos, targetPos, SMOOTH_PATH_SLOP, polys, npolys, steerPos, steerPosFlag, steerPosRef))
            break;

        bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) != 0;
        bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) != 0;

        // Find movement delta.
        float delta[VERTEX_SIZE];
        dtVsub(delta, steerPos, iterPos);
        float len = dtMathSqrtf(dtVdot(delta, delta));
        // If the steer target is end of path or off-mesh link, do not move past the location.
        if ((endOfPath || offMeshConnection) && len < SMOOTH_PATH_STEP_SIZE)
            len = 1.0f;
        else
            len = SMOOTH_PATH_STEP_SIZE / len;

        float moveTgt[VERTEX_SIZE];
        dtVmad(moveTgt, iterPos, delta, len);

        // Move
        float result[VERTEX_SIZE];
        const static uint32 MAX_VISIT_POLY = 16;
        dtPolyRef visited[MAX_VISIT_POLY];

        uint32 nvisited = 0;
        _navMeshQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &_filter, result, visited, (int*)&nvisited, MAX_VISIT_POLY);
        npolys = FixupCorridor(polys, npolys, MAX_PATH_LENGTH, visited, nvisited);

        _navMeshQuery->getPolyHeight(polys[0], result, &result[1]);
        result[1] += 0.5f;
        dtVcopy(iterPos, result);

        // Handle end of path and off-mesh links when close enough.
        if (endOfPath && InRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 1.0f))
        {
            // Reached end of path.
            dtVcopy(iterPos, targetPos);
            if (nsmoothPath < maxSmoothPathSize)
            {
                dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
                nsmoothPath++;
            }
            break;
        }
        else if (offMeshConnection && InRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 1.0f))
        {
            // Advance the path up to and over the off-mesh connection.
            dtPolyRef prevRef = INVALID_POLYREF;
            dtPolyRef polyRef = polys[0];
            uint32 npos = 0;
            while (npos < npolys && polyRef != steerPosRef)
            {
                prevRef = polyRef;
                polyRef = polys[npos];
                npos++;
            }

            for (uint32 i = npos; i < npolys; ++i)
                polys[i-npos] = polys[i];

            npolys -= npos;

            // Handle the connection.
            float connectionStartPos[VERTEX_SIZE], connectionEndPos[VERTEX_SIZE];
            if (dtStatusSucceed(_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, connectionStartPos, connectionEndPos)))
            {
                if (nsmoothPath < maxSmoothPathSize)
                {
                    dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], connectionStartPos);
                    nsmoothPath++;
                }
                // Move position at the other side of the off-mesh link.
                dtVcopy(iterPos, connectionEndPos);
                _navMeshQuery->getPolyHeight(polys[0], iterPos, &iterPos[1]);
                iterPos[1] += 0.5f;
            }
        }

        // Store results.
        if (nsmoothPath < maxSmoothPathSize)
        {
            dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
            nsmoothPath++;
        }
    }

    *smoothPathSize = nsmoothPath;

    // this is most likely a loop
    return nsmoothPath < MAX_POINT_PATH_LENGTH ? DT_SUCCESS : DT_FAILURE;
}

bool PathGenerator::InRangeYZX(const float* v1, const float* v2, float r, float h) const
{
    const float dx = v2[0] - v1[0];
    const float dy = v2[1] - v1[1]; // elevation
    const float dz = v2[2] - v1[2];
    return (dx * dx + dz * dz) < r * r && fabsf(dy) < h;
}

bool PathGenerator::InRange(G3D::Vector3 const& p1, G3D::Vector3 const& p2, float r, float h) const
{
    G3D::Vector3 d = p1 - p2;
    return (d.x * d.x + d.y * d.y) < r * r && fabsf(d.z) < h;
}

float PathGenerator::Dist3DSqr(G3D::Vector3 const& p1, G3D::Vector3 const& p2) const
{
    return (p1 - p2).squaredLength();
}

void PathGenerator::ReducePathLenghtByDist(float dist)
{
    if (GetPathType() == PATHFIND_BLANK)
    {
        sLog->outError(LOG_FILTER_MAPS, "PathGenerator::ReducePathLenghtByDist called before path was built");
        return;
    }

    if (_pathPoints.size() < 2) // path building failure
        return;

    uint32 i = _pathPoints.size();
    G3D::Vector3 nextVec = _pathPoints[--i];
    while (i > 0)
    {
        G3D::Vector3 currVec = _pathPoints[--i];
        G3D::Vector3 diffVec = (nextVec - currVec);
        float len = diffVec.length();
        if (len > dist)
        {
            float step = dist / len;
            // same as nextVec
            _pathPoints[i + 1] -= diffVec * step;
            _sourceUnit->UpdateAllowedPositionZ(_pathPoints[i + 1].x, _pathPoints[i + 1].y, _pathPoints[i + 1].z);
            _pathPoints.resize(i + 2);
            break;
        }
        else if (i == 0) // at second point
        {
            _pathPoints[1] = _pathPoints[0];
            _pathPoints.resize(2);
            break;
        }

        dist -= len;
        nextVec = currVec; // we're going backwards
    }
}

void PathGenerator::UpdateNavMeshQuery()
{
    if (DisableMgr::IsPathfindingEnabled(_sourceUnit->GetMapId()))
    {
        MMAP::MMapManager* mmap = MMAP::MMapFactory::createOrGetMMapManager();

        TRINITY_READ_GUARD(ACE_RW_Thread_Mutex, mmap->GetManagerLock());

        _navMesh = mmap->GetNavMesh(_sourceUnit->GetMapId());

        uint32 l_InstanceId = _sourceUnit->GetInstanceId();
        if (!_sourceUnit->GetMap()->Instanceable())
            l_InstanceId = _sourceUnit->GetMap()->GetInstanceZoneId();

        _navMeshQuery = mmap->GetNavMeshQuery(_sourceUnit->GetMapId(), l_InstanceId);
    }
}

bool PathGenerator::IsInPolygon(G3D::Vector3 const& p_Check, std::vector<G3D::Vector2> const& p_Corners)
{
    uint32 l_Count = uint32(p_Corners.size());
    uint32 l_J = 0;
    bool l_Ok = false;

    /// Algorithm taken from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
    for (uint32 l_I = 0, l_J = l_Count - 1; l_I < l_Count; l_J = l_I++)
    {
        if (((p_Corners[l_I].y > p_Check.y) != (p_Corners[l_J].y > p_Check.y)) &&
            (p_Check.x < (p_Corners[l_J].x - p_Corners[l_I].x) * (p_Check.y - p_Corners[l_I].y) / (p_Corners[l_J].y - p_Corners[l_I].y) + p_Corners[l_I].x))
            l_Ok = !l_Ok;
    }

    return l_Ok;
}

bool PathGenerator::NeedToUseShortcut(G3D::Vector3 const& p_StartPos, G3D::Vector3 const& p_EndPos, uint32 p_MapId)
{
    if (fabs(p_StartPos.z - p_EndPos.z) > (p_MapId == 1492 ? 10.0f : 1.0f))
        return false;

    auto l_Itr = m_ShortcutAreas.find(p_MapId);
    if (l_Itr == m_ShortcutAreas.end())
        return false;

    for (std::vector<G3D::Vector2> const& l_Polygon : l_Itr->second)
    {
        if (IsInPolygon(p_StartPos, l_Polygon) && IsInPolygon(p_EndPos, l_Polygon))
            return true;
    }

    return false;
}

bool PathGenerator::NeedToUseShortcut(G3D::Vector3 const& p_StartPos, G3D::Vector3 const& p_EndPos)
{
    if (fabs(p_StartPos.z - p_EndPos.z) > (_sourceUnit->GetMapId() == 1492 ? 10.0f : 1.0f))
        return false;

    auto l_Itr = m_ShortcutAreas.find(_sourceUnit->GetMapId());
    if (l_Itr == m_ShortcutAreas.end())
        return false;

    for (std::vector<G3D::Vector2> const& l_Polygon : l_Itr->second)
    {
        if (IsInPolygon(p_StartPos, l_Polygon) && IsInPolygon(p_EndPos, l_Polygon))
            return true;
    }

    return false;
}
