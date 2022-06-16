////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
//Basic headers
#include "WaypointMovementGenerator.h"
//Extended headers
#include "ObjectMgr.h"
#include "Transport.h"
//Flightmaster grid preloading
#include "MapManager.h"
//Creature-specific headers
#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureGroups.h"
//Player-specific
#include "Player.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

void WaypointMovementGenerator<Creature>::LoadPath(Creature* creature)
{
    if (!path_id && !(path_id = creature->GetWaypointPath()))
        return;

    /// First check path from waypoint_data
    i_path = sWaypointMgr->GetPath(path_id);

    /// Then check path from waypoint_script_data if waypoint_data is empty for our creature
    if (!i_path)
        i_path = sWaypointMgr->GetPathScript(path_id);

    if (!i_path)
    {
        // No path id found for entry
        sLog->outError(LOG_FILTER_SQL, "WaypointMovementGenerator::LoadPath: creature %s (%lu DB GUID: %u) doesn't have waypoint path id: %u", creature->GetName(), creature->GetGUID(), creature->GetDBTableGUIDLow(), path_id);
        return;
    }

    StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::DoInitialize(Creature* creature)
{
    LoadPath(creature);
    creature->AddUnitState(UNIT_STATE_ROAMING|UNIT_STATE_ROAMING_MOVE);
}

void WaypointMovementGenerator<Creature>::DoFinalize(Creature* creature)
{
    creature->ClearUnitState(UNIT_STATE_ROAMING|UNIT_STATE_ROAMING_MOVE);
    creature->SetWalk(false);
}

void WaypointMovementGenerator<Creature>::DoReset(Creature* creature)
{
    creature->AddUnitState(UNIT_STATE_ROAMING|UNIT_STATE_ROAMING_MOVE);
    StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::OnArrived(Creature* creature)
{
    if (!i_path || i_path->empty())
        return;
    if (m_isArrivalDone)
        return;

    m_isArrivalDone = true;

    if (i_path->at(i_currentNode)->event_id && urand(0, 99) < i_path->at(i_currentNode)->event_chance)
    {
        //sLog->outDebug(LOG_FILTER_MAPSCRIPTS, "Creature movement start script %u at point %u for %llu.", i_path->at(i_currentNode)->event_id, i_currentNode, creature->GetGUID());
        creature->ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        creature->GetMap()->ScriptsStart(sWaypointScripts, i_path->at(i_currentNode)->event_id, creature, NULL);
    }

    // Inform script
    MovementInform(creature);
    creature->UpdatePrevWaypointID(i_currentNode - 1);
    creature->UpdateWaypointID(i_currentNode);

    if (i_path->at(i_currentNode)->delay)
    {
        creature->ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        Stop(i_path->at(i_currentNode)->delay);
    }
}

bool WaypointMovementGenerator<Creature>::StartMove(Creature* creature)
{
    if (!i_path || i_path->empty() || !creature->CanFreeMove())
        return false;

    if (Stopped())
        return true;

    bool transportPath = creature->GetTransport() != NULL;

    if (m_isArrivalDone)
    {
        if ((i_currentNode == i_path->size() - 1) && !repeating) // If that's our last waypoint
        {
            float x = i_path->at(i_currentNode)->x + m_RandomX;
            float y = i_path->at(i_currentNode)->y + m_RandomY;
            float z = i_path->at(i_currentNode)->z;
            float o = creature->GetOrientation();

            if (!transportPath)
                creature->SetHomePosition(x, y, z, o);
            else
            {
                if (Transport* trans = creature->GetTransport())
                {
                    o -= trans->GetOrientation();
                    creature->SetTransportHomePosition(x, y, z, o);
                    trans->CalculatePassengerPosition(x, y, z, o);
                    creature->SetHomePosition(x, y, z, o);
                }
                else
                    transportPath = false;
                // else if (vehicle) - this should never happen, vehicle offsets are const
            }

            if (creature->IsAIEnabled)
                creature->AI()->LastWPReached();

            creature->GetMotionMaster()->Initialize();
            return false;
        }

        i_currentNode = (i_currentNode+1) % i_path->size();
    }

    WaypointData const* node = i_path->at(i_currentNode);

    m_isArrivalDone = false;

    creature->AddUnitState(UNIT_STATE_ROAMING_MOVE);

    Movement::Location formationDest(node->x, node->y, node->z, 0.0f);
    Movement::MoveSplineInit init(creature);

    //! If creature is on transport, we assume waypoints set in DB are already transport offsets
    if (transportPath)
    {
        init.DisableTransportPathTransformations();
        if (TransportBase* trans = creature->GetDirectTransport())
            trans->CalculatePassengerPosition(formationDest.x, formationDest.y, formationDest.z, formationDest.orientation);
    }

    //! Do not use formationDest here, MoveTo requires transport offsets due to DisableTransportPathTransformations() call
    //! but formationDest contains global coordinates
    init.MoveTo(node->x + m_RandomX, node->y + m_RandomY, node->z);

    //! Accepts angles such as 0.00001 and -0.00001, 0 must be ignored, default value in waypoint table
    if (node->orientation && node->delay)
        init.SetFacing(node->orientation);

    if (node->speed != 0.0f)
        init.SetVelocity(std::fabs(node->speed));

    switch (node->move_type)
    {
        case WAYPOINT_MOVE_TYPE_LAND:
            init.SetAnimation(Movement::ToGround);
            break;
        case WAYPOINT_MOVE_TYPE_TAKEOFF:
            init.SetAnimation(Movement::ToFly);
            break;
        case WAYPOINT_MOVE_TYPE_RUN:
            init.SetWalk(false);
            break;
        case WAYPOINT_MOVE_TYPE_WALK:
            init.SetWalk(true);
            break;
    }

    init.Launch();

    /// Call for creature group update
    if (!creature->IsPlayer())
    {
        if (creature->GetFormation() && creature->GetFormation()->getLeader() == creature)
        {
            creature->SetWalk(node->move_type != WAYPOINT_MOVE_TYPE_RUN);
            creature->GetFormation()->LeaderMoveTo(formationDest.x, formationDest.y, formationDest.z);
        }
    }

    return true;
}

bool WaypointMovementGenerator<Creature>::DoUpdate(Creature* creature, uint32 diff)
{
    // Waypoint movement can be switched on/off
    // This is quite handy for escort quests and other stuff
    if (creature->HasUnitState(UNIT_STATE_NOT_MOVE))
    {
        creature->ClearUnitState(UNIT_STATE_ROAMING_MOVE);
        return true;
    }
    // prevent a crash at empty waypoint path.
    if (!i_path || i_path->empty())
        return false;

    if (Stopped())
    {
        if (CanMove(diff))
            return StartMove(creature);
    }
    else
    {
        // Set home position at place on waypoint movement.
        if (!creature->GetTransGUID())
            creature->SetHomePosition(*creature);

        if (creature->IsStopped())
        {
            if (i_path && i_currentNode < i_path->size() && !i_path->at(i_currentNode)->delay)
                Stop(1 * TimeConstants::IN_MILLISECONDS);
            else
                Stop(STOP_TIME_FOR_PLAYER);
        }
        else if (creature->movespline->Finalized())
        {
            OnArrived(creature);
            return StartMove(creature);
        }
        else
        {
            /// Speed changed during path execution, calculate remaining path and launch it once more
            if (i_recalculateSpeed)
            {
                i_recalculateSpeed = false;

                if (!Stopped())
                    return StartMove(creature);
            }
        }
    }

    return true;
 }

void WaypointMovementGenerator<Creature>::MovementInform(Creature* creature)
{
    if (creature->AI())
        creature->AddMovementInform(WAYPOINT_MOTION_TYPE, i_currentNode);
}

bool WaypointMovementGenerator<Creature>::GetResetPos(Creature*, float& x, float& y, float& z)
{
    // prevent a crash at empty waypoint path.
    if (!i_path || i_path->empty())
        return false;

    const WaypointData* node = i_path->at(i_currentNode);
    x = node->x; y = node->y; z = node->z;
    return true;
}

//----------------------------------------------------//

uint32 FlightPathMovementGenerator::GetPathAtMapEnd() const
{
    if (i_currentNode >= i_path.size())
        return i_path.size();

    uint32 curMapId = i_path[i_currentNode]->MapID;
    for (uint32 i = i_currentNode; i < i_path.size(); ++i)
        if (i_path[i]->MapID != curMapId)
            return i;

    return i_path.size();
}

#define SKIP_SPLINE_POINT_DISTANCE_SQ (40.0f * 40.0f)

bool IsNodeIncludedInShortenedPath(TaxiPathNodeEntry const* p1, TaxiPathNodeEntry const* p2)
{
    return p1->MapID != p2->MapID || std::pow(p1->x - p2->x, 2) + std::pow(p1->y - p2->y, 2) > SKIP_SPLINE_POINT_DISTANCE_SQ;
}

void FlightPathMovementGenerator::LoadPath(Player* player, uint32 startNode /*= 0*/)
{
    i_path.clear();
    i_currentNode = startNode;
    _pointsForPathSwitch.clear();
    std::deque<uint32> const& taxi = player->m_taxi.GetPath();
    for (uint32 src = 0, dst = 1; dst < taxi.size(); src = dst++)
    {
        uint32 path, cost;
        sObjectMgr->GetTaxiPath(taxi[src], taxi[dst], path, cost);
        if (path > sTaxiPathNodesByPath.size())
            return;

        TaxiPathNodeList const& nodes = sTaxiPathNodesByPath[path];
        if (!nodes.empty())
        {
            TaxiPathNodeEntry const* start = nodes[0];
            TaxiPathNodeEntry const* end = nodes[nodes.size() - 1];
            bool passedPreviousSegmentProximityCheck = false;
            for (uint32 i = 0; i < nodes.size(); ++i)
            {
                if (passedPreviousSegmentProximityCheck || !src || i_path.empty() || IsNodeIncludedInShortenedPath(i_path.back(), nodes[i]))
                {
                    if ((!src || (IsNodeIncludedInShortenedPath(start, nodes[i]) && i >= 2)) &&
                        (dst == taxi.size() - 1 || (IsNodeIncludedInShortenedPath(end, nodes[i]) && i < nodes.size() - 1)))
                    {
                        passedPreviousSegmentProximityCheck = true;
                        i_path.push_back(nodes[i]);
                    }
                }
                else
                {
                    i_path.pop_back();
                    --_pointsForPathSwitch.back().PathIndex;
                }
            }
        }

        _pointsForPathSwitch.push_back({ uint32(i_path.size() - 1), int32(cost) });
    }
}

void FlightPathMovementGenerator::DoInitialize(Player* player)
{
    Reset(player);
    InitEndGridInfo();
}

void FlightPathMovementGenerator::DoFinalize(Player* player)
{
    // remove flag to prevent send object build movement packets for flight state and crash (movement generator already not at top of stack)
    player->ClearUnitState(UNIT_STATE_IN_FLIGHT);

    player->Dismount();
    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);

    if (player->m_taxi.IsPathEmpty())
    {
        player->getHostileRefManager().setOnlineOfflineState(true);
        // update z position to ground and orientation for landing point
        // this prevent cheating with landing  point at lags
        // when client side flight end early in comparison server side
        player->StopMoving();
    }

    player->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_TAXI_BENCHMARK);

    if (ShapeshiftForm l_ShapeShiftForm = player->GetShapeshiftForm())
    {
        SpellShapeshiftFormEntry const* l_ShapeShiftEntry = sSpellShapeshiftFormStore.LookupEntry(l_ShapeShiftForm);
        if (!l_ShapeShiftEntry)
            return;

        player->SetDisplayId(player->GetModelForForm(l_ShapeShiftForm));
    }
}

#define PLAYER_FLIGHT_SPEED 32.0f

void FlightPathMovementGenerator::DoReset(Player* player)
{
    player->getHostileRefManager().setOnlineOfflineState(false);
    player->AddUnitState(UNIT_STATE_IN_FLIGHT);
    player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);

    Movement::MoveSplineInit init(player);
    uint32 end = GetPathAtMapEnd();

    for (uint32 i = GetCurrentNode(); i < end; ++i)
    {
        G3D::Vector3 vertice(i_path[i]->x, i_path[i]->y, i_path[i]->z);
        init.Path().push_back(vertice);
    }

    init.SetFirstPointId(GetCurrentNode());
    init.SetFly();
    init.SetSmooth();
    init.SetUncompressed();
    //init.SetWalk(true);
    init.SetVelocity(PLAYER_FLIGHT_SPEED * player->GetTotalAuraMultiplier(AuraType::SPELL_AURA_MOD_TAXI_FLIGHT_SPEED));
    init.Launch();
}

bool FlightPathMovementGenerator::DoUpdate(Player* player, uint32 /*diff*/)
{
    uint32 pointId = (uint32)player->movespline->currentPathIdx();
    if (pointId > i_currentNode)
    {
        bool departureEvent = true;
        do
        {
            if (i_currentNode == i_path.size())
                return false;

            DoEventIfAny(player, i_path[i_currentNode], departureEvent);
            while (!_pointsForPathSwitch.empty() && _pointsForPathSwitch.front().PathIndex <= i_currentNode)
            {
                _pointsForPathSwitch.pop_front();
                player->m_taxi.NextTaxiDestination();
                if (!_pointsForPathSwitch.empty())
                {
                    player->UpdateCriteria(CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING, _pointsForPathSwitch.front().Cost);
                    player->ModifyMoney(-_pointsForPathSwitch.front().Cost, "FlightPathMovementGenerator::DoUpdate");
                }
            }

            if (pointId == i_currentNode)
                break;

            if (i_currentNode == _preloadTargetNode)
                PreloadEndGrid();
            i_currentNode += (uint32)departureEvent;
            departureEvent = !departureEvent;

            if (i_currentNode >= i_path.size() - 1)
            {
                player->CleanupAfterTaxiFlight();
                player->SetFallInformation(0, player->GetPositionZ());
                if (player->pvpInfo.inHostileArea)
                    player->CastSpell(player, 2479, true);

                return false;
            }

        } while (true);
    }

    return i_currentNode < (i_path.size() - 1);
}

void FlightPathMovementGenerator::SetCurrentNodeAfterTeleport()
{
    if (i_path.empty() || i_currentNode >= i_path.size())
        return;

    uint32 map0 = i_path[i_currentNode]->MapID;
    for (size_t i = i_currentNode + 1; i < i_path.size(); ++i)
    {
        if (i_path[i]->MapID != map0)
        {
            i_currentNode = i;
            return;
        }
    }
}

void FlightPathMovementGenerator::DoEventIfAny(Player* player, TaxiPathNodeEntry const* node, bool departure)
{
    if (node)
    {
        if (uint32 eventid = departure ? node->DepartureEventID : node->ArrivalEventID)
        {
            sLog->outDebug(LOG_FILTER_MAPSCRIPTS, "Taxi %s event %u of node %u of path %u for player %s", departure ? "departure" : "arrival", eventid, node->NodeIndex, node->PathID, player->GetName());
            player->GetMap()->ScriptsStart(sEventScripts, eventid, player, player);
        }
    }
}

bool FlightPathMovementGenerator::GetResetPos(Player*, float& x, float& y, float& z)
{
    TaxiPathNodeEntry const* node = i_path[i_currentNode];
    x = node->x;
    y = node->y;
    z = node->z;
    return true;
}

void FlightPathMovementGenerator::InitEndGridInfo()
{
    /*! Storage to preload flightmaster grid at end of flight. For multi-stop flights, this will
    be reinitialized for each flightmaster at the end of each spline (or stop) in the flight. */
    uint32 nodeCount = i_path.size();        //! Number of nodes in path.
    _endMapId = i_path[nodeCount - 1]->MapID; //! MapId of last node
    _preloadTargetNode = nodeCount - 3;
    _endGridX = i_path[nodeCount - 1]->x;
    _endGridY = i_path[nodeCount - 1]->y;
    _endGrixZ = i_path[nodeCount - 1]->z;
}

void FlightPathMovementGenerator::PreloadEndGrid()
{
    uint32 l_ZoneId = sMapMgr->GetZoneId(_endMapId, _endGridX, _endGridY, _endGrixZ);

    // used to preload the final grid where the flightmaster is
    if (Map* l_EndMap = sMapMgr->FindBaseNonInstanceMap(_endMapId, l_ZoneId))
    {
        float l_EndGridX = _endGridX;
        float l_EndGridY = _endGridY;
        l_EndMap->AddTask([l_EndMap, l_EndGridX, l_EndGridY]()->void
        {
            l_EndMap->LoadGrid(l_EndGridX, l_EndGridY);
        });
    }
}