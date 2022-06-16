////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "AreaTriggerAI.h"
#include "ObjectAccessor.h"
#include "Unit.h"
#include "SpellInfo.h"
#include "Log.h"
#include "AreaTrigger.h"
#include "Chat.h"
#include "Vehicle.h"
#include "UpdateFieldFlags.h"
#include "SpellPackets.h"
#include <G3D/Vector3.h>
#include "ScriptMgr.h"
#include "VMapFactory.h"
#include "HelperDefines.h"
#include "POIMgr.hpp"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"

AreaTrigger::AreaTrigger() : WorldObject(false),
_source(AreaTriggerSource()), _caster(nullptr), _targetGuid(0), _duration(0), _activationDelay(0), m_updatePeriodic(0),
_updateDelay(0), _liveTime(0), _radius(1.0f), _range(0.0f), atInfo(), _realEntry(0), _moveSpeed(0.0f),
_moveTime(0), _nextMoveTime(0), _waitTime(0), _on_unload(false), _on_despawn(false), _on_remove(false),
_hitCount(1), _areaTriggerCylinder(false), _canMove(false), m_Script(nullptr), _length(0), _MoveElapsedDuration(0),
m_UpdateMoveTime(0), _IsMoving(false), _CircleData(), m_spellInfo(nullptr), m_CastItemGuid(0),
m_CurveChanged(false), m_MoveAreatriggerDelay(0), m_UseMoveCurveID(false), m_DBTableGuid(0), m_ScalingDelay(0), m_speedCircle(0.0f)
{
    m_objectType |= TYPEMASK_AREATRIGGER;
    m_objectTypeId = TYPEID_AREATRIGGER;

    m_updateFlag = UPDATEFLAG_HAS_POSITION | UPDATEFLAG_HAS_AREATRIGGER;

    m_valuesCount = AREATRIGGER_END;
    m_currentNode = 0;
    m_moveDistance = 0.0f;
    m_moveDistanceMax = 0.0f;
    m_moveLength = 0.0f;
    m_moveAngleLos = 0.0f;
    _dynamicValuesCount = AREATRIGGER_DYNAMIC_END;

    _spline.VerticesPoints.clear();
    _spline.TimeToTarget = 0;
    _spline.ElapsedTimeForMovement = 0;

    m_BounceRef = Position();
    m_BounceSrc = Position();
    m_BouncePos = Position();

    m_LastMovePos = Position();
    m_LastMoveCheckTimer = 0;

    m_WindTransPos = G3D::Vector3();

    m_CanAffectDead = false;
}

void AreaTrigger::BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const
{
    if (!target)
        return;

    ByteBuffer fieldBuffer;

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);

    uint32* flags = AreaTriggerUpdateFieldFlags;
    uint32 visibleFlag = UF_FLAG_PUBLIC;
    if (GetCasterGUID() == target->GetGUID())
        visibleFlag |= UF_FLAG_OWNER;

    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (_fieldNotifyFlags & flags[index] ||
            ((updateType == UPDATETYPE_VALUES ? _changesMask.GetBit(index) : m_uint32Values[index]) && (flags[index] & visibleFlag)))
        {
            updateMask.SetBit(index);
            fieldBuffer << m_uint32Values[index];                // other cases
        }
    }

    *data << uint8(updateMask.GetBlockCount());
    updateMask.AppendToPacket(data);
    data->append(fieldBuffer);
}

AreaTrigger::~AreaTrigger()
{
    if (m_Script)
        delete m_Script;
}

void AreaTrigger::AddToWorld()
{
    ///- Register the AreaTrigger for guid lookup and for caster
    if (!IsInWorld())
    {
        sObjectAccessor->AddObject(this);
        WorldObject::AddToWorld();

        /// Spawned AreaTriggers in database don't have any caster
        if (!m_DBTableGuid)
            BindToCaster();
    }
}

void AreaTrigger::RemoveFromWorld()
{
    ///- Remove the AreaTrigger from the accessor and from all lists of objects in world
    if (IsInWorld())
    {
        /// Spawned AreaTriggers in database don't have any caster
        if (!m_DBTableGuid)
            UnbindFromCaster();

        WorldObject::RemoveFromWorld();
        sObjectAccessor->RemoveObject(this);
    }
}

void AreaTrigger::FillCustomData(Unit* caster)
{
    if (GetCustomEntry())
        SetEntry(GetCustomEntry());

    switch(GetSpellId())
    {
        case 143961:    //OO: Defiled Ground
            //ToDo: should cast only 1/4 of circle
            SetSpellId(143960);
            SetDuration(-1);
            _radius = 8.0f;
            //infrontof
            break;
        case 166539:    //WOD: Q34392
        {
            m_movementInfo.t_pos.Relocate(0, 0, 0);
            m_movementInfo.t_time = 0;
            m_movementInfo.t_seat = 64;
            m_movementInfo.t_guid = caster->GetGUID();
            m_movementInfo.VehicleRecID = 0;

            caster->SetAIAnimKitId(6591);
            break;
        }
        case 216292: ///< Strike of the Mountain (Ularogg Cragshaper)
        {
            _radius = 1.0f;
            break;
        }
        default:
            break;
    }
}

void AreaTrigger::GetAttackableUnitListInPositionRange(std::list<Unit*>& p_List, Position const p_Position, float p_SearchRange, bool p_ObjectSize /*= true*/, bool p_AliveCheck /*= true*/) const
{
    CellCoord l_Coord(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell l_Cell(l_Coord);
    l_Cell.SetNoCreate();

    WorldObject const* l_WorldObject = _caster;
    if (!l_WorldObject)
        l_WorldObject = this;

    JadeCore::AnyUnitInPositionRangeCheck l_Check(p_Position, l_WorldObject, p_SearchRange, p_ObjectSize, p_AliveCheck);
    JadeCore::UnitListSearcher<decltype(l_Check)> l_Searcher(l_WorldObject, p_List, l_Check);

    TypeContainerVisitor<decltype(l_Searcher), WorldTypeMapContainer > l_WorldContainerVisitor(l_Searcher);
    TypeContainerVisitor<decltype(l_Searcher), GridTypeMapContainer >  l_GridContainerVisitor(l_Searcher);

    l_Cell.Visit(l_Coord, l_WorldContainerVisitor, *GetMap(), *this, p_SearchRange);
    l_Cell.Visit(l_Coord, l_GridContainerVisitor, *GetMap(), *this, p_SearchRange);
}

void AreaTrigger::SetCircleAreaTrigger(float p_Radius)
{
    SetRadius(p_Radius);

    SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, p_Radius);

    atInfo.Radius                   = p_Radius;
    atInfo.RadiusTarget             = p_Radius;
    atInfo.Polygon.Height           = 0.0f;
    atInfo.Polygon.HeightTarget     = 0.0f;
    atInfo.LocationZOffset          = 0.0f;
    atInfo.LocationZOffsetTarget    = 0.0f;

    atInfo.HasDynamicShape  = false;
    _areaTriggerCylinder    = false;
}

void AreaTrigger::UpdateAffectedList(uint32 p_time, AreaTriggerActionMoment actionM)
{
    if (atInfo.actions.empty() || !IsInWorld())
        return;

    if (actionM & AT_ACTION_MOMENT_ENTER)
    {
        GuidList l_AffectedPlayers = affectedPlayers;
        for (GuidList::iterator l_Itr = l_AffectedPlayers.begin(); l_Itr != l_AffectedPlayers.end(); l_Itr++)
        {
            Unit* unit = ObjectAccessor::GetUnit(*this, *l_Itr);
            if (!unit)
            {
                affectedPlayers.erase(*l_Itr);
                continue;
            }

            if (atInfo.polygon)
            {
                if (!IsInPolygon(unit))
                {
                    affectedPlayers.erase(*l_Itr);
                    AffectUnit(unit, AT_ACTION_MOMENT_LEAVE);
                    if (_ai)
                        _ai->OnUnitExit(unit);
                    if (affectedPlayers.empty())
                        AffectUnit(unit, AT_ACTION_MOMENT_LEAVE_ALL);
                    continue;
                }
            }
            else
            {
                if (!IsInHeight(unit) || !IsWithinDistInMap(unit, GetRadius()) ||
                    (isMoving() && !_spline.VerticesPoints.empty() && _HasActionsWithCharges(AT_ACTION_MOMENT_ON_THE_WAY) && !unit->IsInBetween(this, _spline.VerticesPoints[m_currentNode + 1].x, _spline.VerticesPoints[m_currentNode + 1].y)))
                {
                    bool l_IsLeaving = true;
                    for (uint64 l_Neighbour : m_Neighbours)
                    {
                        if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*this, l_Neighbour))
                        {
                            if (unit->GetExactDist(l_AreaTrigger) < l_AreaTrigger->GetRadius())
                            {
                                l_IsLeaving = false;
                                break;
                            }
                        }
                    }

                    affectedPlayers.erase(*l_Itr);

                    if (l_IsLeaving)
                    {
                        AffectUnit(unit, AT_ACTION_MOMENT_LEAVE);
                        if (_ai)
                            _ai->OnUnitExit(unit);
                    }

                    if (affectedPlayers.empty())
                        AffectUnit(unit, AT_ACTION_MOMENT_LEAVE_ALL);

                    continue;
                }
            }

            if (m_updatePeriodic && _updateDelay <= p_time)
                UpdateOnUnit(unit);
        }

        if (atInfo.maxActiveTargets && affectedPlayers.size() >= atInfo.maxActiveTargets)
            return;

        std::list<Unit*> unitList;
        GetAttackableUnitListInPositionRange(unitList, *this, GetRadius() + atInfo.LocationZOffset, IsCheckingTargetWithObjectSize(), !CanSearchDeadUnits());
        for (Unit* itr : unitList)
        {
            float l_DistToCheck = GetRadius() + (IsCheckingTargetWithObjectSize() ? itr->GetObjectSize() : 0);
            if (itr->GetExactDist2d(this) > l_DistToCheck)
                continue;

            if (atInfo.polygon && !IsInPolygon(itr))
                continue;

            if (_caster && _caster == itr)
            {
                if (affectedPlayers.find(itr->GetGUID()) == affectedPlayers.end())
                {
                    AffectOwner(actionM);
                    affectedPlayers.insert(itr->GetGUID());
                }

                continue;
            }

            if (!m_spellInfo || !(m_spellInfo->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_INVISIBLE))
            {
                if ((itr->IsPlayer() && itr->ToPlayer()->isGameMaster()) || !canSeeOrDetect(itr, true, false, true))
                    continue;
            }

            if (!IsUnitAffected(itr->GetGUID()))
            {
                if (atInfo.polygon && !IsInPolygon(itr))
                    continue;

                if (!IsInHeight(itr))
                    continue;

                /// No
                if (isMoving() && !_spline.VerticesPoints.empty() && _HasActionsWithCharges(AT_ACTION_MOMENT_ON_THE_WAY) && !itr->IsInBetween(this, _spline.VerticesPoints[m_currentNode + 1].x, _spline.VerticesPoints[m_currentNode + 1].y))
                    continue;

                if (!CheckValidateTargets(itr, actionM))
                    continue;

                affectedPlayers.insert(itr->GetGUID());
                AffectUnit(itr, actionM);
                if (_ai)
                    _ai->OnUnitEnter(itr);

                if (atInfo.maxActiveTargets && affectedPlayers.size() >= atInfo.maxActiveTargets)
                    break;
            }
        }
    }
    else
    {
        bool l_IsOwnerAffected = false;
        for (GuidList::iterator itr = affectedPlayers.begin(), next; itr != affectedPlayers.end(); itr = next)
        {
            next = itr;
            ++next;

            Unit* unit = ObjectAccessor::GetUnit(*this, *itr);
            if (!unit)
            {
                affectedPlayers.erase(itr);
                continue;
            }

            bool l_IsLeaving = true;
            for (uint64 l_Neighbour : m_Neighbours)
            {
                if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*this, l_Neighbour))
                {
                    if (unit->GetDistance(l_AreaTrigger) < l_AreaTrigger->GetRadius())
                    {
                        l_IsLeaving = false;
                        break;
                    }
                }
            }

            if (l_IsLeaving)
                AffectUnit(unit, actionM);

            if (actionM == AT_ACTION_MOMENT_REMOVE)
                affectedPlayers.erase(itr);

            if (_caster == unit)
            {
                l_IsOwnerAffected = true;
            }
        }
        if (!l_IsOwnerAffected)
        {
            AffectOwner(actionM);
        }
    }
}

void AreaTrigger::UpdateActionCharges(uint32 p_time)
{
    for (ActionInfoMap::iterator itr = _actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& info = itr->second;
        if (!info.action->chargeRecoveryTime)
            continue;
        if (info.charges >= info.action->maxCharges)
            continue;

        info.recoveryTime += p_time;
        if (info.recoveryTime >= info.action->chargeRecoveryTime)
        {
            info.recoveryTime -= info.action->chargeRecoveryTime;
            ++info.charges;
            if (info.charges == info.action->maxCharges)
                info.recoveryTime = 0;
        }
    }
}

void AreaTrigger::Update(uint32 p_time)
{
    Position const l_Pos = *this;

    _liveTime += p_time;
    _MoveElapsedDuration += p_time;
    m_UpdateMoveTime += p_time;
    m_MoveAreatriggerDelay += p_time;

    if (m_updatePeriodic && _updateDelay > 0)
        _updateDelay -= p_time;

    m_Functions.Update(p_time);

    /// Update real radius if scaled
    if (atInfo.HasDynamicShape && !HasPolygon())
    {
        if (atInfo.ScaleCurveID)
            UpdateScaleCurve(p_time);
        else if (atInfo.MorphCurveID)
            UpdateMorphCurve(p_time);
        else if (atInfo.Radius != atInfo.RadiusTarget)
        {
            uint32 l_ScaleTime = GetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE);
            float l_RadiusInc = atInfo.RadiusTarget - atInfo.Radius;
            float l_RadiusPerMs = l_RadiusInc / float(l_ScaleTime);

            SetRadius(GetRadius() + (p_time * l_RadiusPerMs));
        }
    }

    if (atInfo.HasAttached)
    {
        if (Unit* l_Target = sObjectAccessor->GetUnit(*this, GetTargetGuid()))
        {
            m_positionX = l_Target->m_positionX;
            m_positionY = l_Target->m_positionY;
            m_positionZ = l_Target->m_positionZ;
            m_orientation = l_Target->m_orientation;
        }
    }

    if (HasPolygon())
        UpdatePolygon();

    if (m_UseMoveCurveID)
        UpdateMoveCurve();
    else
    {
        bool l_MoveRecalc = false;
        if (m_LastMoveCheckTimer && atInfo.moveType == AT_MOVE_TYPE_BOUNCE_ON_COL)
        {
            if (m_LastMoveCheckTimer <= p_time)
            {
                m_LastMoveCheckTimer = 0;

                if (m_LastMovePos.IsNearPosition(this, 0.1f))
                {
                    if (sScriptMgr->OnAreaTriggerBounce(this))
                        BounceAreaTrigger(frand(0.0f, 2.0f / M_PI));

                    l_MoveRecalc = true;
                }
            }
            else
                m_LastMoveCheckTimer -= p_time;
        }

        switch (atInfo.moveType)
        {
            case AT_MOVE_TYPE_TO_TARGET:
            {
                UpdatePositionWithPathId(_MoveElapsedDuration, this);

                Unit* l_Target = Unit::GetUnit(*this, _targetGuid);
                if (!l_Target)
                    break;

                float l_Dist = atInfo.HasFollowsTerrain ? GetExactDist2d(l_Target) : GetExactDist(l_Target); /// Check dist 3D OR if areatrigger follows terrain, check dist 2D instead.

                if (l_Dist <= atInfo.Radius)
                {
                    TargetReached(l_Target);
                    if (m_spellInfo->Id == 110744 && _spline.TimeToTarget <= 0) ///< Remove Divine Star when target reached
                        Remove();
                }

                if (_spline.TimeToTarget)
                    _spline.TimeToTarget -= p_time;

                /// Update path to target every 200ms
                if ((m_UpdateMoveTime < 200 && m_spellInfo->Id != 110744)) ///< Divine Star needs more updates
                    break;

                m_UpdateMoveTime = 0;

                WorldLocation l_Loc;

                l_Loc.m_mapId       = l_Target->GetMapId();
                l_Loc.m_positionH   = l_Target->GetPositionH();
                l_Loc.m_positionX   = l_Target->GetPositionX();
                l_Loc.m_positionY   = l_Target->GetPositionY();

                if (m_spellInfo->Id == 110744) ///< Divine Star follows the target wherever it is
                    l_Loc.m_positionZ = l_Target->GetPositionZ();
                else
                    l_Loc.m_positionZ = l_Target->GetMap()->GetHeight(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZH());

                l_Loc.m_orientation = l_Target->GetOrientation();

                if ((!_dest.IsNearPosition(&l_Loc, 0.1f) && m_MoveAreatriggerDelay > 1000) || (!_dest.IsNearPosition(&l_Loc, 2.0f)))
                {
                    m_MoveAreatriggerDelay = 0;
                    _dest = l_Loc;
                    if (atInfo.speed)
                        MoveAreaTrigger(&l_Loc, atInfo.speed, true, atInfo.HasFollowsTerrain);
                    else
                        MoveAreaTrigger(&l_Loc, std::max(_spline.TimeToTarget, 1), false, atInfo.HasFollowsTerrain);
                }

                break;
            }
            case AT_MOVE_TYPE_DEFAULT:
            case AT_MOVE_TYPE_THROUGH_WALLS:
            case AT_MOVE_TYPE_DEST:
            case AT_MOVE_TYPE_RE_SHAPE:
            case AT_MOVE_TYPE_STATIC_SPLINE:
            case AT_MOVE_TYPE_PART_PATH:
            case AT_MOVE_TYPE_SPIRAL:
            case AT_MOVE_TYPE_BOOMERANG:
            case AT_MOVE_TYPE_RANDOM:
            case AT_MOVE_TYPE_SOURCE_ANGLE:
            {
                UpdatePositionWithPathId(_MoveElapsedDuration, this);
                break;
            }
            case AT_MOVE_TYPE_BOUNCE_ON_COL:
            {
                if (l_MoveRecalc)
                    break;

                UpdatePositionWithPathId(_MoveElapsedDuration, this);

                /// AreaTrigger destination reached, make it bounce if needed
                Position l_NullPos = Position();
                if ((!m_BouncePos.IsNearPosition(&l_NullPos, 0.1f) && m_BouncePos.IsNearPosition(this, 0.5f)))
                {
                    if (sScriptMgr->OnAreaTriggerBounce(this))
                        BounceAreaTrigger(0.0f);
                }

                break;
            }
            case AT_MOVE_TYPE_CIRCLE:
            {
                UpdatePositionForCircle(this);
                break;
            }
            default:
            {
                UpdateMovement(p_time);
                UpdateRotation(p_time);
                break;
            }
        }

        if (!m_LastMoveCheckTimer)
        {
            m_LastMoveCheckTimer = 200;
            m_LastMovePos = GetPosition();
        }
    }

    UpdateActionCharges(p_time);

    if (!_activationDelay)
    {
        if (m_ScalingDelay)
        {
            if (m_ScalingDelay <= p_time)
            {
                m_CurveChanged = false;

                std::list<ActionInfo> l_Actions;
                for (ActionInfoMap::iterator itr = _actionInfo.begin(); itr != _actionInfo.end(); ++itr)
                {
                    ActionInfo& info = itr->second;
                    if (!(info.action->moment & AT_ACTION_MOMENT_UPDATE))
                        continue;

                    if (info.action->actionType == AT_ACTION_TYPE_TARGETS_SCALE)
                        l_Actions.push_front(info);
                    else if (info.action->actionType == AT_ACTION_TYPE_NO_TARGETS_SCALE)
                        l_Actions.push_back(info);
                }

                /// Check if there are some valid targets first
                for (ActionInfo l_Action : l_Actions)
                {
                    /// Don't try to scale anymore if it already was
                    if (!m_CurveChanged)
                        DoAction(nullptr, l_Action);
                }

                /// Scaling periodic timer
                m_ScalingDelay = HasScalingAction() ? atInfo.Param : 0;
            }
            else
                m_ScalingDelay -= p_time;
        }

        if (!m_updatePeriodic || _updateDelay <= p_time)
        {
            for (ActionInfoMap::iterator itr = _actionInfo.begin(); itr != _actionInfo.end(); ++itr)
            {
                ActionInfo& info = itr->second;
                if (!(info.action->moment & AT_ACTION_MOMENT_UPDATE))
                    continue;

                /// Those might be already handled right before this block
                if (info.action->actionType == AT_ACTION_TYPE_TARGETS_SCALE || info.action->actionType == AT_ACTION_TYPE_NO_TARGETS_SCALE)
                    continue;

                DoAction(nullptr, info);
            }
        }
    }

    if (GetDuration() != -1)
    {
        if (GetDuration() > int32(p_time))
        {
            _duration -= p_time;
        }
        else
        {
            Remove(!_on_despawn); // expired
            return;
        }
    }

    if (_activationDelay)
    {
        if (_activationDelay > p_time)
            _activationDelay -= p_time;
        else
        {
            _activationDelay = 0;
            UpdateAffectedList(p_time, AT_ACTION_MOMENT_ON_ACTIVATE);
        }
    }
    else
        UpdateAffectedList(p_time, AT_ACTION_MOMENT_ENTER);

    if (m_updatePeriodic && _updateDelay <= int32(p_time))
        _updateDelay = m_updatePeriodic;

    sScriptMgr->OnUpdateAreaTriggerEntity(this, p_time);

    if (*this != l_Pos)
        GetMap()->AreaTriggerRelocation(this, m_positionX, m_positionY, m_positionZ, m_orientation);
}

void AreaTrigger::UpdateScaleCurve(uint32 p_Diff)
{
    uint32 l_MaxDuration = GetDuration() + _liveTime;
    if (!l_MaxDuration)
        return;

    float l_ProgressPct = float(_liveTime) / float(l_MaxDuration);
    float l_RadiusScale = GetCurveValueAt(atInfo.ScaleCurveID, l_ProgressPct);
    float l_RealRadius  = atInfo.Radius * l_RadiusScale;

    if (l_RealRadius == GetRadius())
        return;

    SetRadius(l_RealRadius);
}

void AreaTrigger::UpdateMorphCurve(uint32 p_Diff)
{
    uint32 l_MaxDuration = GetDuration() + _liveTime;
    if (!l_MaxDuration)
        return;

    float l_ProgressPct = float(_liveTime) / float(l_MaxDuration);
    float l_RadiusScale = GetCurveValueAt(atInfo.MorphCurveID, l_ProgressPct);
    float l_RealRadius  = atInfo.Radius  + ((atInfo.RadiusTarget - atInfo.Radius) * l_RadiusScale);

    if (l_RealRadius == GetRadius())
        return;

    SetRadius(l_RealRadius);
}

void AreaTrigger::UpdateMoveCurve()
{
    UpdatePositionWithPathId(_MoveElapsedDuration, this);
}

bool AreaTrigger::IsUnitAffected(uint64 guid) const
{
    return std::find(affectedPlayers.begin(), affectedPlayers.end(), guid) != affectedPlayers.end();
}

void AreaTrigger::AffectUnit(Unit* p_Unit, AreaTriggerActionMoment p_ActionM)
{
    if (p_ActionM == AreaTriggerActionMoment::AT_ACTION_MOMENT_ENTER)
    {
        Player* l_Player = p_Unit->ToPlayer();

        if (sSpellMgr->IsInATDebug() && l_Player)
            ChatHandler(l_Player).PSendSysMessage("Entering in AreaTrigger with SpellId: %u Entry: %u", GetSpellId(), GetEntry());

        if (sScriptMgr->OnSpellAreaTriggerAddTarget(this, p_Unit))
            return;
    }

    if (p_ActionM == AreaTriggerActionMoment::AT_ACTION_MOMENT_LEAVE)
    {
        Player* l_Player = p_Unit->ToPlayer();

        if (sSpellMgr->IsInATDebug() && l_Player)
            ChatHandler(l_Player).PSendSysMessage("Leaving AreaTrigger with SpellId: %u Entry: %u", GetSpellId(), GetEntry());

        if (sScriptMgr->OnSpellAreaTriggerRemoveTarget(this, p_Unit))
            return;
    }

    for (ActionInfoMap::iterator itr =_actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& info = itr->second;
        if (!(info.action->moment & p_ActionM))
            continue;

        DoAction(p_Unit, info);
        // if (unit != _caster)
            // AffectOwner(actionM);//action if action on area trigger
    }
}

void AreaTrigger::AffectOwner(AreaTriggerActionMoment actionM)
{
    /// Static areatriggers don't have any owner
    if (m_DBTableGuid > 0)
        return;

    if (actionM == AreaTriggerActionMoment::AT_ACTION_MOMENT_ENTER)
    {
        if (sScriptMgr->OnSpellAreaTriggerAddTarget(this, _caster))
            return;
    }

    if (actionM == AreaTriggerActionMoment::AT_ACTION_MOMENT_LEAVE)
    {
        if (sScriptMgr->OnSpellAreaTriggerRemoveTarget(this, _caster))
            return;
    }

    for (ActionInfoMap::iterator itr =_actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& info = itr->second;
        //if (!(info.action->targetFlags & AT_TARGET_FLAG_ALWAYS_TRIGGER))
        //    continue;
        if (!(info.action->moment & actionM))
            continue;

        DoAction(_caster, info);
    }
}

void AreaTrigger::UpdateOnUnit(Unit* unit)
{
    for (ActionInfoMap::iterator itr =_actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& info = itr->second;
        if (!(info.action->moment & AT_ACTION_MOMENT_UPDATE_TARGET))
            continue;

        DoAction(unit, info);
    }
}

bool AreaTrigger::_HasActionsWithCharges(AreaTriggerActionMoment action /*= AT_ACTION_MOMENT_ENTER*/)
{
    for (ActionInfoMap::iterator itr =_actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& info = itr->second;
        if (info.action->moment & action)
        {
            if (info.charges || !info.action->maxCharges)
                return true;
        }
    }
    return false;
}

bool AreaTrigger::HasScalingAction() const
{
    for (auto l_Itr : _actionInfo)
    {
        ActionInfo& l_Info = l_Itr.second;
        if (l_Info.action->actionType == AT_ACTION_TYPE_NO_TARGETS_SCALE ||
            l_Info.action->actionType == AT_ACTION_TYPE_TARGETS_SCALE ||
            l_Info.action->actionType == AT_ACTION_TYPE_CHANGE_SCALE)
            return true;
    }

    return false;
}

void AreaTrigger::DoAction(Unit* unit, ActionInfo& action)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(action.action->spellId, GetMap()->GetDifficultyID());
    if (action.action->actionType == AT_ACTION_TYPE_CAST_SPELL && !spellInfo)
        return;

    // do not process depleted actions
    if (!action.charges && action.action->maxCharges)
        return;

    Unit* caster = _caster;

    /// Allow static areatriggers to have a caster = affectedUnit
    if (caster == nullptr)
        caster = unit;

    if (action.action->targetFlags & AT_TARGET_FLAG_FRIENDLY)
        if (!caster || !caster->IsFriendlyTo(unit) || unit->isTotem())
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_HOSTILE)
        if (!caster || !caster->IsHostileTo(unit))
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_VALIDATTACK)
        if (!caster || !caster->_IsValidAttackTarget(unit, m_spellInfo, nullptr, true) || unit->isTotem())
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_OWNER)
        if (unit->GetGUID() != GetCasterGUID())
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_PLAYER)
        if (!unit->ToPlayer())
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_NOT_PET)
        if (unit->isPet())
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_NOT_FULL_HP)
        if (unit->IsFullHealth())
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_GROUP_OR_RAID)
        if (!unit->IsInRaidWith(caster))
            return;
    if (action.action->targetFlags & AT_TARGET_FLAG_TARGET_CHECK_LOS)
        if (!unit || !IsWithinLOSInMap(unit))
            return;

    Unit* l_SavedUnit = unit;

    //action on self
    if (action.action->targetFlags & AT_TARGET_FLAG_TARGET_IS_CASTER)
        unit = _caster;

    // should cast on self.
    if (((spellInfo && spellInfo->Effects[EFFECT_0].TargetA.GetTarget() == TARGET_UNIT_CASTER)
        || action.action->targetFlags & AT_TARGET_FLAG_CASTER_IS_TARGET) && !(action.action->targetFlags & AT_TARGET_FLAG_TARGET_IS_CASTER_2))
        caster = l_SavedUnit;

    if (action.action->targetFlags & AT_TARGET_FLAG_NOT_AURA_TARGET)
        if (GetTargetGuid() && GetTargetGuid() == unit->GetGUID())
            return;

    if (action.action->targetFlags & AT_TARGET_FLAG_CAST_AURA_TARGET)
        if (Unit* target = ObjectAccessor::GetUnit(*this, GetTargetGuid()))
            unit = target;

    if (action.action->targetFlags & AT_TARGET_FLAT_IN_FRONT)
        if (!HasInArc(static_cast<float>(M_PI), unit))
            return;

    if (action.action->targetFlags & AT_TARGET_FLAG_TARGET_IS_SUMMONER)
    {
        if (Unit* summoner = _caster->GetAnyOwner())
        {
            if (unit->GetGUID() != summoner->GetGUID())
                return;
        }
        else
            return;
    }
        

    if (action.action->targetFlags & AT_TARGET_FLAG_NOT_OWNER)
        if (unit->GetGUID() == GetCasterGUID())
            return;

    if (action.action->targetFlags & AT_TARGET_FLAG_NPC_ENTRY)
        if (action.amount != unit->GetEntry() || unit->ToPlayer())
            return;

    if (unit && action.action->aura > 0 && !unit->HasAura(action.action->aura))
        return;
    else if (unit && action.action->aura < 0 && unit->HasAura(abs(action.action->aura)))
        return;

    if (action.action->hasspell > 0 && !_caster->HasSpell(action.action->hasspell))
        return;
    else if (action.action->hasspell < 0 && _caster->HasSpell(abs(action.action->hasspell)))
        return;

    if (!CheckActionConditions(*action.action, unit))
        return;

    if (action.action->targetFlags & AT_TARGET_FLAG_NOT_FULL_ENERGY)
    {
        Powers energeType = Powers::MAX_POWERS;
        for (uint8 i = 0; i < spellInfo->EffectCount; ++i)
        {
            if (spellInfo->Effects[i].Effect == SPELL_EFFECT_ENERGIZE)
                energeType = Powers(spellInfo->Effects[i].MiscValue);
        }

        if (energeType == Powers::MAX_POWERS || unit->GetMaxPower(energeType) == 0 || unit->GetMaxPower(energeType) == unit->GetPower(energeType))
            return;
    }

    if (action.action->targetFlags & AT_TARGET_FLAG_TARGET_PASSANGER)
    {
        if (Vehicle* veh = unit->GetVehicleKit())
        {
            for (uint8 i = 0; i < MAX_VEHICLE_SEATS; i++)
                if (Unit* pass = veh->GetPassenger(i))
                    unit = pass;
        }
    }

    if (action.action->targetFlags & AT_TARGET_FLAG_TARGET_PASSANGER_VEH)
    {
        if (Vehicle* veh = unit->GetVehicleKit())
        {
            for (uint8 i = 0; i < MAX_VEHICLE_SEATS; i++)
                if (Unit* pass = veh->GetPassenger(i))
                {
                    if (Vehicle* vehPas = pass->GetVehicleKit())
                    {
                        bool empty = true;
                        for (uint8 j = 0; j < MAX_VEHICLE_SEATS; j++)
                            if (vehPas->GetPassenger(j))
                            {
                                empty = false;
                                break;
                            }
                        if (empty)
                        {
                            unit = pass;
                            break;
                        }
                    }
                }
        }
    }

    if (action.action->hitMaxCount < 0)
    {
        if (!affectedPlayersForAllTime.empty())
            for (GuidList::iterator itr = affectedPlayersForAllTime.begin(); itr != affectedPlayersForAllTime.end(); ++itr)
                if (unit->GetGUID() == (*itr))
                    return;
    }
    else if (action.action->hitMaxCount && int32(action.hitCount) >= action.action->hitMaxCount)
        return;

    switch (action.action->actionType)
    {
        case AT_ACTION_TYPE_CAST_SPELL:
        {
            if (_on_remove)
                return;

            if (caster && unit)
            {
                if (Creature* l_Creature = caster->ToCreature())
                {
                    if (l_Creature->IsAIEnabled && !l_Creature->AI()->CheckAreaTriggerCastAction(this, unit, spellInfo))
                        return;
                }

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(action.action->spellId);
                if (l_SpellInfo == nullptr)
                    break;

                caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::TriggeredByAreaTrigger][action.action->spellId] = GetGUID();

                Item* l_CastItem = nullptr;
                if (m_CastItemGuid && caster->IsPlayer())
                    l_CastItem = caster->ToPlayer()->GetItemByGuid(m_CastItemGuid);

                uint64 l_OriginalCasterGuid = _caster ? _caster->GetGUID() : 0;

                /// HACK: issues of aura stacks came up after _caster->GetGUID() started to be used as original caster guid
                /// Maybe more common way like flag or setting is needed
                if (m_spellInfo)
                {
                    switch (m_spellInfo->Id)
                    {
                        case 257306: ///< Gift of the Sea
                        case 257313: ///< Gift of the Sky
                            l_OriginalCasterGuid = caster->GetGUID();
                            break;
                    }
                }

                if (action.action->targetFlags & AT_TARGET_FLAG_CAST_AT_SRC)
                    caster->CastSpell(GetPositionX(), GetPositionY(), GetPositionZH(), action.action->spellId, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_CASTED_BY_AREATRIGGER), l_CastItem, nullptr, l_OriginalCasterGuid);
                else if (l_SpellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS || unit->IsWithinLOS(GetPositionX(), GetPositionY(), GetPositionZ()) ||
                    (caster->IsAIEnabled && (caster->ToCreature()->AI()->CanTargetOutOfLOS() || caster->ToCreature()->AI()->CanTargetOutOfLOSXYZ(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ()))))
                    caster->CastSpell(unit, action.action->spellId, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_CASTED_BY_AREATRIGGER), l_CastItem, nullptr, l_OriginalCasterGuid);

                if (!_caster)
                    break;

                if (Creature* creature = _caster->ToCreature())
                    if (creature->IsAIEnabled)
                        creature->AI()->OnAreaTriggerCast(_caster, unit, action.action->spellId);
            }
            break;
        }
        case AT_ACTION_TYPE_REMOVE_AURA:
        {
            if (unit)
                unit->RemoveAura(action.action->spellId);       //only one aura should be removed.

            break;
        }
        case AT_ACTION_TYPE_ADD_STACK:
        {
            if (unit)
            {
                if (Aura* aura = unit->GetAura(action.action->spellId))
                    aura->ModStackAmount(1);
            }

            break;
        }
        case AT_ACTION_TYPE_REMOVE_STACK:
        {
            if (unit)
            {
                if (Aura* aura = unit->GetAura(action.action->spellId))
                    aura->ModStackAmount(-1);
            }

            break;
        }
        case AT_ACTION_TYPE_CHANGE_SCALE:
        {
            float l_BaseRadius = atInfo.Radius;
            float l_CurrRadius = GetRadius();

            l_CurrRadius += CalculatePct(l_BaseRadius, action.action->scale);

            SetRadius(l_CurrRadius);
            break;
        }
        case AT_ACTION_TYPE_SHARE_DAMAGE:
        {
            if (caster)
            {
                int32 bp0 = spellInfo->Effects[EFFECT_0].BasePoints / _hitCount;
                int32 bp1 = spellInfo->Effects[EFFECT_1].BasePoints / _hitCount;
                int32 bp2 = spellInfo->Effects[EFFECT_2].BasePoints / _hitCount;

                if (action.action->targetFlags & AT_TARGET_FLAG_CAST_AT_SRC)
                {
                    SpellCastTargets targets;
                    targets.SetCaster(caster);
                    targets.SetDst(GetPositionX(), GetPositionY(), GetPositionZH(), GetOrientation());

                    CustomSpellValues values;
                    if (bp0)
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, bp0);
                    if (bp1)
                        values.AddSpellMod(SPELLVALUE_BASE_POINT1, bp1);
                    if (bp2)
                        values.AddSpellMod(SPELLVALUE_BASE_POINT2, bp2);
                    caster->CastSpell(targets, spellInfo, &values, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_CASTED_BY_AREATRIGGER));
                }
                else if (unit)
                    caster->CastCustomSpell(unit, action.action->spellId, &bp0, &bp1, &bp2, true);
            }
            break;
        }
        case AT_ACTION_TYPE_APPLY_MOVEMENT_FORCE:
        {
            if (unit)
            {
                if (Player* l_Player = unit->ToPlayer())
                {
                    if (!l_Player->HasAuraType(SPELL_AURA_DISABLE_MOVEMENT_FORCE) && (atInfo.windX != 0.0f || atInfo.windY != 0.0f || atInfo.windZ != 0.0f))
                        l_Player->SendApplyMovementForce(GetGUID(), true, Position(atInfo.windX, atInfo.windY, atInfo.windZ, 0.0f), atInfo.windSpeed, atInfo.windType, m_WindTransPos);
                    else
                        l_Player->SendApplyMovementForce(GetGUID(), true, *this, atInfo.windSpeed, atInfo.windType);
                }
            }

            break;
        }
        case AT_ACTION_TYPE_REMOVE_MOVEMENT_FORCE:
        {
            if (unit)
            {
                if (Player* player = unit->ToPlayer())
                {
                    if (player->HasMovementForce(GetGUID()))
                        player->SendApplyMovementForce(GetGUID(), false, Position());
                }
            }

            break;
        }
        case AT_ACTION_TYPE_CHANGE_DURATION_ANY_AT:
        {
            float searchRange = 0.0f;
            for (uint32 j = 0; j < spellInfo->EffectCount; ++j)
            {
                if (float radius = spellInfo->Effects[j].CalcRadius(caster))
                    searchRange = radius;
            }

            if (!searchRange)
                break;

            uint32 entry = GetEntry();
            std::list<AreaTrigger*> atlist;
            GetAreatriggerListInRange(atlist, searchRange);

            atlist.remove_if([entry](AreaTrigger const* p_AreaTrigger) -> bool
            {
                if (p_AreaTrigger->GetEntry() == entry)
                    return false;

                return true;
            });

            if (!atlist.empty())
            {
                for (std::list<AreaTrigger*>::const_iterator itr = atlist.begin(); itr != atlist.end(); ++itr)
                {
                    if (AreaTrigger* at = (*itr))
                        if (at->GetDuration() > 500)
                            at->SetDuration(100);
                }
            }
            break;
        }
        case AT_ACTION_TYPE_CHANGE_AMOUNT_FROM_HEALT:
        {
            if (_on_remove || !action.amount)
                return;

            if (caster && unit)
            {
                int32 health = unit->GetMaxHealth() - unit->GetHealth();
                if (health >= action.amount)
                {
                    health = action.amount;
                    action.amount = 0;
                }
                else
                    action.amount -= health;

                caster->CastCustomSpell(unit, action.action->spellId, &health, &health, &health, true);
            }

            if (!action.amount && action.charges > 0)
            {
                _on_despawn = true;
                SetDuration(0);
            }
            return;
        }
        case AT_ACTION_TYPE_RE_PATCH:
        {
            float dist = GetExactDist2d(_caster);
            _moveSpeed = atInfo.speed ? atInfo.speed : 7.0f;
            _spline.VerticesPoints.clear();
            G3D::Vector3 curPos;
            PositionToVector(curPos);
            _spline.VerticesPoints.push_back(curPos);
            _spline.VerticesPoints.push_back(curPos);
            _spline.VerticesPoints.push_back(curPos);
            _spline.VerticesPoints.push_back(curPos);
            m_moveDistanceMax = 3.0f;
            _waitTime = uint32(dist * 100);

            _spline.TimeToTarget = int32(m_moveDistanceMax / _moveSpeed  * 1000.0f);
            SetDuration(_spline.TimeToTarget + _waitTime);
            _canMove = true;
            break;
        }
        case AT_ACTION_TYPE_SET_AURA_CUSTOM_ADD: // 11
        {
            if (Aura* aura = caster->GetAura(action.action->spellId))
            {
                aura->ModCustomData(1);
                aura->RecalculateAmountOfEffects(true);
            }
            break;
        }
        case AT_ACTION_TYPE_SET_AURA_CUSTOM_REMOVE: // 12
        {
            if (Aura* aura = caster->GetAura(action.action->spellId))
            {
                aura->ModCustomData(-1);
                aura->RecalculateAmountOfEffects(true);
            }
            break;
        }
        case AT_ACTION_TYPE_SLOW: //13
        {
            switch (atInfo.moveType)
            {
                case AT_MOVE_TYPE_DEFAULT:
                case AT_MOVE_TYPE_THROUGH_WALLS:
                {
                    MoveAreaTrigger(&_dest, uint32((atInfo.timeToTarget - _MoveElapsedDuration) / action.action->scale), false, atInfo.HasFollowsTerrain);
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case AT_ACTION_TYPE_SET_IN_MOTION: //15
        {
            switch (atInfo.moveType)
            {
                case AT_MOVE_TYPE_DEFAULT:
                case AT_MOVE_TYPE_THROUGH_WALLS:
                {
                    _dest = *this;
                    float x = 0.0f;
                    float y = 0.0f;
                    float z = 0.0f;
                    caster->GetClosePoint(x, y, z, 0.0f, GetSpellInfo()->RangeEntry->maxRangeFriend, atInfo.AngleToCaster);
                    _dest.m_positionX = x;
                    _dest.m_positionY = y;
                    MoveAreaTrigger(&_dest, static_cast<uint32>(atInfo.timeToTarget ? atInfo.timeToTarget : GetSpellInfo()->GetDuration()), false, atInfo.HasFollowsTerrain);
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case AT_ACTION_TYPE_TARGETS_SCALE: ///< 16
        {
            /// On retail, even if the value didn't changed, the fields are sent
            for (uint8 l_I = 0; l_I < 7; ++l_I)
                ForceValuesUpdateAtIndex(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

            /// No need to update target scaling if there is no targets
            if (affectedPlayers.empty() || _liveTime <= atInfo.scalingDelay || !atInfo.Param)
                break;

            /// As far as we know, only players can trigger AT dynamic scaling
            /// So we must check if there is at least one affected player
            bool l_Break = true;
            for (uint64 l_Guid : affectedPlayers)
            {
                if (Unit* l_Unit = Unit::GetUnit(*this, l_Guid))
                {
                    if (l_Unit->IsPlayer())
                    {
                        l_Break = false;
                        break;
                    }
                }
            }

            if (l_Break)
                break;

            if (action.action->aura > 0)
            {
                bool l_Found = false;
                for (uint64 l_Guid : affectedPlayers)
                {
                    if (Unit* l_Unit = Unit::GetUnit(*this, l_Guid))
                    {
                        /// Process scaling if someone has required aura
                        if (l_Unit->HasAura(action.action->aura))
                        {
                            l_Found = true;
                            break;
                        }
                    }
                }

                if (!l_Found)
                    break;
            }

            float l_BaseScale   = atInfo.Radius;
            float l_Scale       = GetRadius();
            float l_PctPerMSec  = action.action->scale / 1000.0f;

            if (atInfo.HasAttached)
            {
                Unit* l_AttachedTo = sObjectAccessor->GetUnit(*this, m_movementInfo.t_guid);
                if (l_AttachedTo)
                    l_PctPerMSec /= l_AttachedTo->GetFloatValue(UNIT_FIELD_MOD_TIME_RATE);
            }

            float l_Pct         = (l_Scale / l_BaseScale) + (atInfo.Param * l_PctPerMSec / 100.0f);

            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, _liveTime);
            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
            SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);

            /// Current scale
            SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, l_Scale / l_BaseScale);
            /// Next scale
            SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Pct);

            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
            SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, atInfo.Param);

            SetRadius(l_BaseScale * l_Pct);

            m_CurveChanged = true;
            break;
        }
        case AT_ACTION_TYPE_NO_TARGETS_SCALE: ///< 17
        {
            /// On retail, even if the value didn't changed, the fields are sent
            for (uint8 l_I = 0; l_I < 7; ++l_I)
                ForceValuesUpdateAtIndex(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

            if (_liveTime <= atInfo.scalingDelay || !atInfo.Param)
                break;

            if (!affectedPlayers.empty())
            {
                bool l_Found = false;
                for (uint64 l_Guid : affectedPlayers)
                {
                    if (Unit* l_Unit = Unit::GetUnit(*this, l_Guid))
                    {
                        /// Process scaling if someone has required aura
                        if (l_Unit->IsPlayer() && (!action.action->aura || l_Unit->HasAura(std::abs(action.action->aura))))
                        {
                            l_Found = true;
                            break;
                        }
                    }
                }

                if (l_Found)
                    break;
            }

            float l_BaseScale   = atInfo.Radius;
            float l_Scale       = GetRadius();
            float l_PctPerMSec  = action.action->scale / 1000.0f;

            if (atInfo.HasAttached)
            {
                Unit* l_AttachedTo = sObjectAccessor->GetUnit(*this, m_movementInfo.t_guid);
                if (l_AttachedTo)
                    l_PctPerMSec /= l_AttachedTo->GetFloatValue(UNIT_FIELD_MOD_TIME_RATE);
            }

            float l_Pct         = (l_Scale / l_BaseScale) + (atInfo.Param * l_PctPerMSec / 100.0f);

            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, _liveTime);
            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
            SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);

            /// Current scale
            SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, l_Scale / l_BaseScale);
            /// Next scale
            SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Pct);

            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
            SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, atInfo.Param);

            SetRadius(l_BaseScale * l_Pct);

            m_CurveChanged = true;
            break;
        }
        case AT_ACTION_TYPE_SET_AURA_DURATION: ///< 18
        {
            if (unit)
            {
                Aura* l_Aura = unit->GetAura(action.action->spellId, caster->GetGUID());
                if (l_Aura)
                {
                    l_Aura->SetMaxDuration(action.amount);
                    l_Aura->SetDuration(action.amount);
                }
            }

            break;
        }
        case AT_ACTION_TYPE_RE_SHAPE:
        {
            ReShape(action.action->amount, false);
            break;
        }
        case AT_ACTION_TYPE_NO_ACTION:
            break;
        default:
            break;
    }

    if (unit && action.action->hitMaxCount < 0)
        affectedPlayersForAllTime.insert(unit->GetGUID());

    action.hitCount++;
    if (atInfo.hitType & (1 << action.action->actionType))
        _hitCount++;

    bool l_NoChargeLeft = false;
    if (action.charges > 0)
    {
        --action.charges;
        if (action.charges == 0 && !action.action->chargeRecoveryTime)
            l_NoChargeLeft = true;
    }

    /// Unload at next update.
    if (l_NoChargeLeft && (!_HasActionsWithCharges() || action.DespawnFromCharges))
    {
        _on_despawn = true;
        SetDuration(0);
    }
}

void AreaTrigger::ActionOnDespawn()
{
    for (ActionInfoMap::iterator itr = _actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& info = itr->second;
        if (!(info.action->moment & AT_ACTION_MOMENT_ON_DESPAWN))
            continue;

        DoAction(_caster, info);
    }
}

void AreaTrigger::TargetReached(Unit* p_Target)
{
    for (ActionInfoMap::iterator itr = _actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& info = itr->second;
        if (!(info.action->moment & AT_ACTION_MOMENT_TARGET_REACHED))
            continue;

        DoAction(p_Target, info);
    }
}

void AreaTrigger::Remove(bool duration)
{
    if (_on_unload)
        return;

    _on_unload = true;
    if (IsInWorld())
    {
        if (_caster && m_spellInfo)
            _caster->SendSpellPlayOrphanVisual(m_spellInfo, false);

        if (duration)
        {
            UpdateAffectedList(0, AT_ACTION_MOMENT_DESPAWN);//remove from world with time

            ActionOnDespawn();
        }
        else
            UpdateAffectedList(0, AT_ACTION_MOMENT_LEAVE);//remove from world in action

        UpdateAffectedList(0, AT_ACTION_MOMENT_REMOVE);//any remove from world

        // Possibly this?
        if (!IsInWorld())
            return;

        sScriptMgr->OnRemoveAreaTriggerEntity(this, 0);

        SendObjectDeSpawnAnim(GetGUID());
        RemoveFromWorld();
        AddObjectToRemoveList();

        RemoveFromNeighbours();
    }
}

void AreaTrigger::Despawn()
{
    if (_on_remove)
        return;

    _on_remove = true;
    if (IsInWorld())
    {
        ActionOnDespawn();
        UpdateAffectedList(0, AT_ACTION_MOMENT_DESPAWN);//remove from world with time
        UpdateAffectedList(0, AT_ACTION_MOMENT_REMOVE);//any remove from world

        // Possibly this?
        if (!IsInWorld())
            return;

        SendObjectDeSpawnAnim(GetGUID());
        RemoveFromWorld();
        AddObjectToRemoveList();
    }
}

float AreaTrigger::GetVisualScale(bool target /*=false*/) const
{
    if (_areaTriggerCylinder || atInfo.hasAreaTriggerBox || HasPolygon()) // Send only for sphere
        return 0.0f;

    if (target)
        return atInfo.RadiusTarget;

    return atInfo.Radius;
}

float AreaTrigger::GetRadius() const
{
    switch (GetSpellId())
    {
        case 192799:
            return 4.5f;
        case 191034: ///< Starfall
        {
            if (GetCaster() && GetCaster()->HasAura(202354)) ///< Stellar Drift
                return 19.5f;
            break;
        }
        default:
            break;
    }

    return _radius;
}

void AreaTrigger::SetRadius(float p_Radius)
{
    if (p_Radius <= 0.0f)
    {
        _on_despawn = true;
        SetDuration(0);
        return;
    }

    UpdateNeighbours();

    _radius = p_Radius;
}

Unit* AreaTrigger::GetCaster() const
{
    return ObjectAccessor::GetUnit(*this, GetCasterGUID());
}

AreaTrigger::AreaTriggerSource AreaTrigger::GetSource() const
{
    return _source;
}

AreaTrigger::AreaTriggerSource AreaTrigger::SetSource(float p_X, float p_Y, float p_Z, float p_O, Map* p_Map, uint32 p_Phasemask)
{
    _source.x = p_X;
    _source.y = p_Y;
    _source.z = p_Z;
    _source.o = p_O;
    _source.map = p_Map;
    _source.phasemask = p_Phasemask;
    return _source;
}

AreaTrigger::AreaTriggerSource AreaTrigger::SetSource(Position p_Position, Map* p_Map /*= nullptr*/, uint32 p_PhaseMask /*= 0*/)
{
    return SetSource(p_Position.m_positionX, p_Position.m_positionY, p_Position.m_positionZ, p_Position.m_orientation, p_Map, p_PhaseMask);
}

void AreaTrigger::SetCreatingEffectGuid(Guid128 p_Guid)
{
    SetGuidValue(EAreaTriggerFields::AREATRIGGER_FIELD_CREATING_EFFECT_GUID, p_Guid);
}

bool AreaTrigger::CheckActionConditions(AreaTriggerAction const& action, Unit* unit)
{
    Unit* caster = GetCaster();
    if (!caster)
        caster = unit;

    /// Allow static areatriggers to have a caster = affectedUnit
    if (!caster)
        return false;

    ConditionSourceInfo srcInfo = ConditionSourceInfo(caster, unit);

    ConditionContainer const* l_Conditions = sConditionMgr->GetConditionsForAreaTriggerAction(_realEntry, action.id);
    if (l_Conditions == nullptr)
        return true;

    return sConditionMgr->IsObjectMeetToConditions(srcInfo, *l_Conditions);
}

void AreaTrigger::BindToCaster()
{
    ASSERT(!_caster);
    _caster = ObjectAccessor::GetUnit(*this, GetCasterGUID());
    ASSERT(_caster);
    ASSERT(_caster->GetMap() == GetMap());
    _caster->_RegisterAreaTrigger(this);

    if (Creature* l_Creature = _caster->ToCreature())
    {
        if (l_Creature->IsAIEnabled)
            l_Creature->AI()->AreaTriggerCreated(this);
    }
}

void AreaTrigger::UnbindFromCaster()
{
    ASSERT(_caster);

    if (Creature* l_Creature = _caster->ToCreature())
    {
        if (l_Creature->IsAIEnabled)
            l_Creature->AI()->AreaTriggerDespawned(this, _on_despawn);
    }

    _caster->_UnregisterAreaTrigger(this);
    _caster = nullptr;
}

bool AreaTrigger::IsInHeight(WorldObject* p_ObjectTarget)
{
    float _height = atInfo.Polygon.Height;
    if (atInfo.hasAreaTriggerBox)
        _height = atInfo.LocationZOffset;

    if (!_height)
        return true;

    if ((p_ObjectTarget->GetPositionZH() - GetPositionZH()) > atInfo.Polygon.Height)
        return false;

    return true;
}

bool AreaTrigger::IsInPolygon(WorldObject* p_ObjectTarget)
{
    /// Real polygon detection
    if (HasPolygon())
    {
        std::unordered_map<uint32, G3D::Vector2> _points = m_Polygon;
        if (_points.empty())
        {
            if (!atInfo.Polygon.VerticesTarget.empty())
                _points = atInfo.Polygon.VerticesTarget;
            else
                _points = atInfo.Polygon.Vertices;
        }

        if (_points.size() < 3)
            return false;

        if (!IsInHeight(p_ObjectTarget))
            return false;

        std::vector<G3D::Vector2> l_CheckPoints;

        for (uint32 l_I = 0; l_I < _points.size(); ++l_I)
        {
            /// Reporting the distance and the angle according to the current orientation
            float l_X = _points[l_I].x * std::cos(m_orientation) - _points[l_I].y * std::sin(m_orientation) + m_positionX;
            float l_Y = _points[l_I].x * std::sin(m_orientation) + _points[l_I].y * std::cos(m_orientation) + m_positionY;

            /// Adding the point to the vector
            l_CheckPoints.push_back(G3D::Vector2(l_X, l_Y));
        }

        uint32 l_Count = uint32(l_CheckPoints.size());
        uint32 l_J = 0;
        bool l_Ok = false;

        /// Algorithm taken from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
        for (uint32 l_I = 0, l_J = l_Count - 1; l_I < l_Count; l_J = l_I++)
        {
            if (((l_CheckPoints[l_I].y > p_ObjectTarget->m_positionY) != (l_CheckPoints[l_J].y > p_ObjectTarget->m_positionY)) &&
                (p_ObjectTarget->m_positionX < (l_CheckPoints[l_J].x - l_CheckPoints[l_I].x) * (p_ObjectTarget->m_positionY - l_CheckPoints[l_I].y) / (l_CheckPoints[l_J].y - l_CheckPoints[l_I].y) + l_CheckPoints[l_I].x))
            {
                l_Ok = !l_Ok;
            }
        }

        return l_Ok;
    }
    /// Box polygon detection
    else
    {
        if (atInfo.Polygon.Vertices.size() < 3)
            return false;

        if (!IsInHeight(p_ObjectTarget))
            return false;

        std::unordered_map<uint32, G3D::Vector2> _points;
        if (!atInfo.Polygon.VerticesTarget.empty())
            _points = atInfo.Polygon.VerticesTarget;
        else
            _points = atInfo.Polygon.Vertices;

        static const int q_patt[2][2]= { {0,1}, {3,2} };
        float x_source = p_ObjectTarget->GetPositionX() - GetPositionX(); //Point X on polygon
        float y_source = p_ObjectTarget->GetPositionY() - GetPositionY(); //Point Y on polygon
        float angle = std::atan2(y_source, x_source) - GetOrientation(); angle = (angle >= 0) ? angle : 2 * M_PI + angle;

        float dist = sqrt(x_source*x_source + y_source*y_source);
        float x = dist * std::cos(angle);
        float y = dist * std::sin(angle);

        //sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::IsInPolygon x_source %f y_source %f angle %f dist %f x %f y %f", x_source, y_source, angle, dist, x, y);

        G3D::Vector2 pred_pt = _points[0];
        pred_pt.x -= x;
        pred_pt.y -= y;

        int pred_q = q_patt[pred_pt.y < 0][pred_pt.x < 0];

        int w = 0;

        for (uint16 i = 0; i < _points.size(); ++i)
        {
            G3D::Vector2 cur_pt = _points[i];

            cur_pt.x -= x;
            cur_pt.y -= y;

            int q = q_patt[cur_pt.y < 0][cur_pt.x < 0];

            switch (q - pred_q)
            {
                case -3:
                    ++w;
                    break;
                case 3:
                    --w;
                    break;
                case -2:
                    if (pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x)
                        ++w;
                    break;
                case 2:
                    if (!(pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x))
                        --w;
                    break;
            }

            pred_pt = cur_pt;
            pred_q = q;
        }
        return w != 0;
    }
}

float AreaTrigger::CalculateRadiusPolygon()
{
    //calc maxDist for search zone
    float distance = 0.0f;
    for (auto const& v : atInfo.Polygon.Vertices)
    {
        float distsq = fabs(v.second.x) > fabs(v.second.y) ? fabs(v.second.x) : fabs(v.second.y);
        if (distsq > distance)
            distance = distsq;
    }

    for (auto const& v : atInfo.Polygon.VerticesTarget)
    {
        float distsq = fabs(v.second.x) > fabs(v.second.y) ? fabs(v.second.x) : fabs(v.second.y);
        if (distsq > distance)
            distance = distsq;
    }
    //sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::CalculateRadiusPolygon distance %f", distance);

    return distance;
}

void AreaTrigger::CastAction()
{
    UpdateAffectedList(0, AT_ACTION_MOMENT_ON_CAST_ACTION);
}

bool AreaTrigger::UpdatePosition(ObjectGuid targetGuid)
{
    if (_caster->GetGUID() == targetGuid)
    {
        if (_caster->GetMap() == GetMap())
        {
            bool turn = (GetOrientation() != _caster->GetOrientation());
            bool relocated = (GetPositionX() != _caster->GetPositionX() || GetPositionY() != _caster->GetPositionY());

            if (relocated)
                GetMap()->AreaTriggerRelocation(this, _caster->GetPositionX(), _caster->GetPositionY(), _caster->GetPositionZH(), _caster->GetOrientation());
            else if (turn)
                SetOrientation(_caster->GetOrientation());
        }
        else
        {
            GetMap()->RemoveFromMap(this, false);
            Relocate(*_caster);
            SetMap(_caster->GetMap());
            GetMap()->AddToMap(this);
        }
        return true;
    }
    if (Unit* target = ObjectAccessor::GetUnit(*this, targetGuid))
    {
        if (GetMap() == target->GetMap())
        {
            bool turn = (GetOrientation() != target->GetOrientation());
            bool relocated = (GetPositionX() != target->GetPositionX() || GetPositionY() != target->GetPositionY());

            if (relocated)
                GetMap()->AreaTriggerRelocation(this, target->GetPositionX(), target->GetPositionY(), target->GetPositionZH(), target->GetOrientation());
            else if (turn)
                SetOrientation(target->GetOrientation());

            return true;
        }
    }

    return false;
}

void AreaTrigger::CalculateRadius(Spell* spell/* = nullptr*/)
{
    Unit* caster = GetCaster();

    if (atInfo.polygon)
        _radius = CalculateRadiusPolygon();
    else
    {
        bool find = false;
        if (atInfo.Radius || atInfo.RadiusTarget)
        {
            /// Slowly changes size
            if (atInfo.HasDynamicShape)
            {
                if (atInfo.Radius < atInfo.RadiusTarget)
                    _radius = atInfo.Radius;
                else
                    _radius = atInfo.RadiusTarget;
            }
            else
                _radius = atInfo.Radius;

            find = true;
        }

        if (caster && !find && m_spellInfo)
        {
            for (uint32 j = 0; j < m_spellInfo->EffectCount; ++j)
            {
                if (float r = m_spellInfo->Effects[j].CalcRadius(GetCaster()))
                    _radius = r * (spell ? spell->m_spellValue->RadiusMod : 1.0f);
            }
        }
    }

    if (caster && m_spellInfo)
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, _radius, spell);
}

bool AreaTrigger::CheckValidateTargets(Unit* unit, AreaTriggerActionMoment actionM)
{
    Unit* caster = _caster;

    if (!unit)
        return true;

    for (ActionInfoMap::iterator itr =_actionInfo.begin(); itr != _actionInfo.end(); ++itr)
    {
        ActionInfo& action = itr->second;
        if (!(action.action->moment & actionM))
            continue;

        if (action.action->targetFlags & AT_TARGET_FLAG_FRIENDLY)
        {
            if (!caster || !caster->IsFriendlyTo(unit) || unit->isTotem())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_HOSTILE)
        {
            if (!caster || !caster->IsHostileTo(unit))
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_VALIDATTACK)
        {
            if (!caster || !caster->_IsValidAttackTarget(unit, m_spellInfo, nullptr, true) || unit->isTotem())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_OWNER)
        {
            if (unit->GetGUID() != GetCasterGUID())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_PLAYER)
        {
            if (unit->GetTypeId() != TYPEID_PLAYER)
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_NOT_PET)
        {
            if (unit->isPet())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_NOT_FULL_HP)
        {
            if (unit->IsFullHealth())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_GROUP_OR_RAID)
        {
            if (!caster || !unit->IsInRaidWith(caster))
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_NOT_AURA_TARGET)
        {
            if (GetTargetGuid() && GetTargetGuid() == unit->GetGUID())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAT_IN_FRONT)
        {
            if (!HasInArc(static_cast<float>(M_PI), unit))
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_TARGET_IS_SUMMONER)
        {
            if (Unit* summoner = _caster->GetAnyOwner())
            {
                if (unit->GetGUID() != summoner->GetGUID())
                    continue;
            }
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_NOT_OWNER)
        {
            if (unit->GetGUID() == GetCasterGUID())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_NPC_ENTRY)
        {
            if (action.amount != unit->GetEntry() || unit->ToPlayer())
                continue;
        }

        if (action.action->targetFlags & AT_TARGET_FLAG_TARGET_CHECK_LOS)
        {
            if (!unit || !IsWithinLOSInMap(unit))
                continue;
        }

        if (action.action->aura > 0 && !unit->HasAura(action.action->aura))
            continue;
        else if (action.action->aura < 0 && unit->HasAura(abs(action.action->aura)))
            continue;

        if (action.action->hasspell > 0 && !_caster->HasSpell(action.action->hasspell))
            continue;
        else if (action.action->hasspell < 0 && _caster->HasSpell(abs(action.action->hasspell)))
            continue;

        return true;
    }

    return false;
}

void AreaTrigger::CastSpell(Unit* p_Target, uint32 p_SpellId)
{
    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellId);
    if (!l_SpellInfo)
        return;

    bool l_Self = false;
    for (uint8 i = 0; i < l_SpellInfo->EffectCount; ++i)
    {
        if (l_SpellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_CASTER)
        {
            l_Self = true;
            break;
        }
    }

    if (l_Self)
    {
        if (p_Target)
            p_Target->CastSpell(p_Target, l_SpellInfo, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_CASTED_BY_AREATRIGGER));
        return;
    }

    //summon world trigger
    Creature* l_Trigger = SummonTrigger(GetPositionX(), GetPositionY(), GetPositionZ(), 0, l_SpellInfo->CalcCastTime() + 100);
    if (!l_Trigger)
        return;

    if (Unit* l_Owner = GetCaster())
    {
        l_Trigger->setFaction(l_Owner->getFaction());
        // needed for GO casts for proper target validation checks
        l_Trigger->SetGuidValue(UNIT_FIELD_SUMMONED_BY, l_Owner->GetGUID());
        l_Trigger->CastSpell(p_Target ? p_Target : l_Trigger, l_SpellInfo, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_CASTED_BY_AREATRIGGER), 0, nullptr, l_Owner->GetGUID());
    }
    else
    {
        l_Trigger->setFaction(FACTION_MONSTER);
        // Set owner guid for target if no owner available - needed by trigger auras
        // - trigger gets despawned and there's no caster avalible (see AuraEffect::TriggerSpell())
        l_Trigger->CastSpell(p_Target ? p_Target : l_Trigger, l_SpellInfo, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_CASTED_BY_AREATRIGGER), 0, nullptr, p_Target ? p_Target->GetGUID() : 0);
    }
}

void AreaTrigger::CastSpellByOwnerFromATLocation(Unit* p_Target, uint32 p_SpellId)
{
    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellId);
    Unit* l_Caster = GetCaster();
    if (!l_Caster || !l_SpellInfo || !p_Target)
        return;

    if (p_Target->IsWithinLOS(GetPositionX(), GetPositionY(), GetPositionZ()))
        l_Caster->CastSpell(p_Target, p_SpellId, true);
}

bool AreaTrigger::CreateAreaTrigger(uint32 guidlow, uint32 triggerEntry, Unit* caster, Unit* p_OriginalCaster, SpellInfo const* info, Position const& p_Pos, Position const& p_PosMove, Spell* spell /*= nullptr*/, uint64 targetGuid /*= 0*/, uint32 customEntry /*= 0*/)
{
    if (caster == nullptr)
        return false;

    if (p_OriginalCaster == nullptr)
        p_OriginalCaster = caster;

    m_spellInfo = info;

    uint32 spellId = m_spellInfo ? m_spellInfo->Id : 0;

    if (m_spellInfo && m_spellInfo->HasCustomAttribute(SpellCustomAttributes::SPELL_ATTR0_CU_AREATRIGGER_BIG_OBJ))
        SetBig(true);

    // Caster not in world, might be spell triggered from aura removal
    if (!p_OriginalCaster->IsInWorld())
        return false;

    if (m_spellInfo && !(m_spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD) && !p_OriginalCaster->isAlive())
    {
        sLog->outError(LOG_FILTER_GENERAL, "AreaTrigger (spell %u) caster %s is dead", spellId, p_OriginalCaster->ToString().c_str());
        return false;
    }

    SetMap(p_OriginalCaster->GetMap());

    Relocate(p_Pos);
    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_GENERAL, "AreaTrigger (spell %u) not created. Invalid coordinates (X: %f Y: %f)", spellId, GetPositionX(), GetPositionY());
        return false;
    }

    AreaTriggerInfo const* infoAt = nullptr;
    if (triggerEntry)
        infoAt = sObjectMgr->GetAreaTriggerInfo(triggerEntry, spellId);
    if (customEntry)
        infoAt = sObjectMgr->GetAreaTriggerInfoByEntry(customEntry, spellId);

    if (infoAt != nullptr)
    {
        atInfo = *infoAt;
        _activationDelay = atInfo.activationDelay;

        for (AreaTriggerActionList::const_iterator itr = atInfo.actions.begin(); itr != atInfo.actions.end(); ++itr)
            _actionInfo[itr->id] = ActionInfo(&*itr);

        /// HACKFIX FOR IVANYR (THE ARCWAY)
        if (caster && info && info->Id == 196806)
        {
            uint32 l_I = 0;
            if (caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks].empty())
                return false;

            for (uint64 l_GUID : caster->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks])
            {
                Player* l_Player = sObjectAccessor->GetPlayer(*caster, l_GUID);
                if (!l_Player)
                    return false;

                atInfo.Polygon.Vertices[l_I++] = G3D::Vector2(l_Player->GetPositionX() - p_Pos.GetPositionX(), l_Player->GetPositionY() - p_Pos.GetPositionY());
            }
        }
    }

    if (AreaTriggerCircle const* l_CircleData = sObjectMgr->GetAreaTriggerCircle(triggerEntry))
    {
        _CircleData = *l_CircleData;
        m_speedCircle = atInfo.speed;

        if (_CircleData.Type == eAreaTriggerCircleTypes::CenterTypeCaster)
        {
            _CircleData.TargetGuid.emplace();
            _CircleData.TargetGuid = p_OriginalCaster->GetGUID();

            _CircleData.InitialAngle    = p_OriginalCaster->GetAngle(this);
            if (!_CircleData.Radius)
                _CircleData.Radius          = GetDistance(*p_OriginalCaster);
            _CircleData.BlendFormRadius = GetDistance(*p_OriginalCaster);

            if (atInfo.speed > 0.0f)
            {
                float l_Distance = _CircleData.Radius * 2.0f * M_PI;
                _CircleData.TimeToTarget = l_Distance / atInfo.speed * 1000.0f;
            }
        }
    }

    sScriptMgr->InitScriptEntity(this);

    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger (spell %u) coordinates (X: %f Y: %f) _actionInfo %lu", spellId, GetPositionX(), GetPositionY(), _actionInfo.size());

    Object::_Create(guidlow, atInfo.customEntry, HIGHGUID_AREATRIGGER);

    SetPhaseMask(p_OriginalCaster->GetPhaseMask(), false);

    if (p_OriginalCaster->ToCreature() && p_OriginalCaster->GetPhaseMask() == 0)
    {
        for (auto l_Phase : p_OriginalCaster->GetPhases())
        {
            SetInPhase(l_Phase, false, true);
        }
    }

    if (p_OriginalCaster->ToTempSummon() && p_OriginalCaster->MustBeVisibleOnlyForSomePlayers())
    {
        if (p_OriginalCaster->ToTempSummon()->GetSummonerGUID())
        {
            AddPlayerInPersonnalVisibilityList(p_OriginalCaster->ToTempSummon()->GetSummonerGUID());
        }
    }

    _realEntry = triggerEntry;
    SetEntry(atInfo.customEntry);
    uint32 duration = m_spellInfo ? m_spellInfo->GetDuration() : 0;
    if (!duration)
        duration = -1;

    _canMove = atInfo.isMoving;
    m_UseMoveCurveID = !atInfo.isMoving && atInfo.MoveCurveID > 0;

    /// Delay before first scaling
    m_ScalingDelay = HasScalingAction() ? atInfo.scalingDelay : 0;

    Player* modOwner = p_OriginalCaster ? p_OriginalCaster->GetSpellModOwner() : nullptr;

    /// Update periodic
    if (atInfo.updateDelay && m_spellInfo)
    {
        m_updatePeriodic = atInfo.updateDelay;

        if ((m_spellInfo->HasAttribute(SpellAttr5::SPELL_ATTR5_HASTE_AFFECT_DURATION) || m_spellInfo->HasAttribute(SpellAttr8::SPELL_ATTR8_HASTE_AFFECT_DURATION)) && caster)
            m_updatePeriodic = int32(m_updatePeriodic * caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED));

        if (modOwner)
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_ACTIVATION_TIME, m_updatePeriodic);

        /// Start periodic on next update
        if (!m_spellInfo->HasAttribute(SpellAttr5::SPELL_ATTR5_START_PERIODIC_AT_APPLY))
            _updateDelay += m_updatePeriodic;
    }

    // Calculate duration by haste.
    if (duration != -1 && m_spellInfo && m_spellInfo->HasAttribute(SPELL_ATTR8_HASTE_AFFECT_DURATION_RECOVERY) && p_OriginalCaster)
        duration = int32(duration * std::max<float>(p_OriginalCaster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED), 0.5f));

    if (duration != -1 && modOwner && m_spellInfo)
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

    SetDuration(duration);
    SetObjectScale(1);

    AI_Initialize();

    //if (atInfo.HasAbsoluteOrientation)
        //SetOrientation(0.0f);
    // on some spells radius set on dummy aura, not on create effect.
    // overwrite by radius from spell if exist.

    CalculateRadius(spell);

    if (atInfo.Polygon.Height && !atInfo.polygon)
        _areaTriggerCylinder = true;

    SetGuidValue(AREATRIGGER_FIELD_CASTER, p_OriginalCaster->GetGUID());

    if (caster->GetGUID() != p_OriginalCaster->GetGUID())
        SetAreatriggerSpellCasterGuid(caster->GetGUID());

    if (spell != nullptr)
        SetCreatingEffectGuid(spell->GetCastGuid());

    SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, spellId);
    SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, spellId ? spellId : atInfo.spellId);
    SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, m_spellInfo ? m_spellInfo->GetSpellXSpellVisualId(p_OriginalCaster) : atInfo.VisualID);
    SetFloatValue(AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, atInfo.Radius);
    SetUInt32Value(AREATRIGGER_FIELD_EXTRA_SCALE_CURVE + 5, 1065353217);
    SetUInt32Value(AREATRIGGER_FIELD_EXTRA_SCALE_CURVE + 6, 1);

    /// This is a visual data, it can acts as a SpellXSpellVisualID
    /// Eg. 223751 -> 63: Purple Pool, 60: Green Pool
    if (atInfo.DecalPropertiesId)
        SetUInt32Value(AREATRIGGER_FIELD_DECAL_PROPERTIES_ID, atInfo.DecalPropertiesId);

    SetTargetGuid(targetGuid);

    if (atInfo.HasAttached)
    {
        Unit* l_AttachTarget = GetCaster();

        if (Unit* l_Target = ObjectAccessor::GetUnit(*this, GetTargetGuid()))
            l_AttachTarget = l_Target;

        if (l_AttachTarget)
        {
            m_updateFlag |= UPDATEFLAG_HAS_TRANSPORT_POSITION;
            m_movementInfo.t_guid = l_AttachTarget->GetGUID();
        }
    }

    if (info)
        _range = GetSpellInfo()->GetMaxRange() < _radius ? _radius : GetSpellInfo()->GetMaxRange(); //If spline not set client crash, set default to 15m
    else
        _range = _radius;

    if (atInfo.Distance != 0.0f)
        _range = atInfo.Distance;

    if (atInfo.RollPitchYaw1Z)
        m_orientation += atInfo.RollPitchYaw1Z;

    // culculate destination point
    if (isMoving() || m_UseMoveCurveID)
    {
        if (atInfo.timeToTarget)
            SetTimeToTarget(atInfo.timeToTarget);
        else
            SetTimeToTarget(duration);

        Position pos = p_Pos;
        Position posMove = p_PosMove;;

        sScriptMgr->OnSetCreatePositionEntity(this, p_OriginalCaster, pos, posMove, _spline.VerticesPoints);

        Relocate(pos);

        _waitTime = atInfo.waitTime;

        if (atInfo.speed)
            _moveSpeed = atInfo.speed;
        else
            _moveSpeed = (_range / duration) * 1000.0f;

        _spline.TimeToTarget *= p_OriginalCaster->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);

        /// If spline verticles points aren't empty, they are calculated in scripts, we don't need the default behavior.
        if (_spline.VerticesPoints.empty())
        {
            Position* l_Source = nullptr;

            switch (atInfo.moveType)
            {
                case AT_MOVE_TYPE_RANDOM:
                {
                    if (atInfo.AngleToCaster == 0.0f)
                        atInfo.AngleToCaster = frand(0, 2.0f * M_PI);
                    else
                        atInfo.AngleToCaster *= caster->CountAreaTrigger(spellId);

                    /// No break!
                }
                case AT_MOVE_TYPE_STATIC_SPLINE:
                case AT_MOVE_TYPE_DEFAULT:
                case AT_MOVE_TYPE_THROUGH_WALLS:
                case AT_MOVE_TYPE_DEST:
                case AT_MOVE_TYPE_RE_SHAPE:
                case AT_MOVE_TYPE_SPIRAL:
                case AT_MOVE_TYPE_BOOMERANG:
                case AT_MOVE_TYPE_PART_PATH:
                case AT_MOVE_TYPE_BOUNCE_ON_COL:
                {
                    l_Source = const_cast<Position*>(&pos);
                    if (!l_Source && atInfo.moveType != AT_MOVE_TYPE_PART_PATH)
                        l_Source = p_OriginalCaster;
                    if (!l_Source)
                        return false;

                    if (atInfo.moveType == AT_MOVE_TYPE_BOUNCE_ON_COL || atInfo.moveType == AT_MOVE_TYPE_THROUGH_WALLS)
                        SetSource(l_Source->m_positionX, l_Source->m_positionY, l_Source->m_positionZ, l_Source->m_orientation + atInfo.AngleToCaster, caster->GetMap());
                    else
                        SetSource(l_Source->m_positionX, l_Source->m_positionY, l_Source->m_positionZ, caster->GetOrientation() + atInfo.AngleToCaster, caster->GetMap());

                    break;
                }
                case AT_MOVE_TYPE_SOURCE_ANGLE:
                {
                    l_Source = const_cast<Position*>(&pos);
                    if (!l_Source)
                        l_Source = p_OriginalCaster;

                    if (!l_Source)
                        return false;

                    SetSource(l_Source->m_positionX, l_Source->m_positionY, l_Source->m_positionZ, l_Source->m_orientation, caster->GetMap());
                    break;
                }
                default:
                    break;
            }

            switch (atInfo.moveType)
            {
                case AT_MOVE_TYPE_STATIC_SPLINE: ///< 10
                {
                    if (atInfo.MoveCurveID && sObjectMgr->GetAreaTriggerMoveSplines(atInfo.MoveCurveID, 0).m_move_id == atInfo.MoveCurveID)
                    {
                        for (uint32 l_Itr = 0; l_Itr < sObjectMgr->GetAreaTriggerPathSize(atInfo.MoveCurveID) - 1; ++l_Itr)
                            _splineData.push_back(sObjectMgr->GetAreaTriggerMoveSplines(atInfo.MoveCurveID, l_Itr));
                        FillVerticesPoint(l_Source);
                    }
                    break;
                }
                case AT_MOVE_TYPE_DEST: // 14
                {
                    if (spell)
                    {
                        if (Unit* l_Target = spell->GetUnitTarget())
                            const_cast<Position&>(p_PosMove) = *l_Target;

                        _range = p_Pos.GetExactDist2d(&p_PosMove);
                    }
                    /// No break intended
                }
                case AT_MOVE_TYPE_RANDOM: // 6
                case AT_MOVE_TYPE_DEFAULT: // 0
                case AT_MOVE_TYPE_THROUGH_WALLS: // 16
                case AT_MOVE_TYPE_RE_SHAPE: // 11
                case AT_MOVE_TYPE_SOURCE_ANGLE:
                {
                    if (_range != 0.0f)
                    {
                        AreaTriggerMoveSplines l_MoveSpline;
                        l_MoveSpline.m_path_x = 0;
                        l_MoveSpline.m_path_y = 0;
                        l_MoveSpline.m_path_z = 0;

                        if (UseMoveCurveID())
                        {
                            auto l_Iter = g_CurvePoints.find(atInfo.MoveCurveID);
                            if (l_Iter == g_CurvePoints.end())
                                break;

                            for (auto l_Itr : l_Iter->second)
                            {
                                l_MoveSpline.m_path_x = _range * l_Itr->Y;
                                _splineData.push_back(l_MoveSpline);
                            }
                        }
                        else
                        {
                            _splineData.push_back(l_MoveSpline);

                            for (uint32 l_Itr = 0; l_Itr < (uint32)_range / 2; ++l_Itr)
                            {
                                l_MoveSpline.m_path_x = l_Itr * 2;
                                _splineData.push_back(l_MoveSpline);
                            }

                            l_MoveSpline.m_path_x = uint32(_range);

                            _splineData.push_back(l_MoveSpline);
                        }

                        _splineData.push_back(l_MoveSpline);

                        FillVerticesPoint(l_Source);

                        /// in case of collision compensate speed
                        if (!_moveSpeed)
                            _spline.TimeToTarget = CalculatePct(_spline.TimeToTarget, (_length / _range) * 100.0f);
                    }
                    break;
                }
                case AT_MOVE_TYPE_SPIRAL: // 2
                {
                    float _nextAngle = atInfo.AngleToCaster ? atInfo.AngleToCaster : 45.0f;
                    float _modParam = atInfo.Param ? atInfo.Param : 0.3f;
                    float maxDist = duration / 1000.0f * _moveSpeed;

                    for (float a = 0;; a += _nextAngle) // next angle 45
                    {
                        float rad = a * M_PI / 180.0f;
                        float r = 1.0f * exp(rad * _modParam);
                        AreaTriggerMoveSplines l_MoveSpline;
                        if (!_splineData.size())
                        {
                            l_MoveSpline.m_path_x = 0;
                            l_MoveSpline.m_path_y = 0;
                            l_MoveSpline.m_path_z = 0;
                            _splineData.push_back(l_MoveSpline);
                        }
                        l_MoveSpline.m_path_x = _splineData.back().m_path_x - (r * std::sin(rad));
                        l_MoveSpline.m_path_y = _splineData.back().m_path_y + (r * std::cos(rad));
                        l_MoveSpline.m_path_z = _splineData.back().m_path_z;
                        _splineData.push_back(l_MoveSpline);

                        if (_splineData.size() < 2)
                            continue;

                        Position _posA = _splineData[_splineData.size() - 2].ToPosition();
                        Position _posB = _splineData[_splineData.size() - 1].ToPosition();
                        float l_Diff = _posA.GetExactDist(&_posB);
                        maxDist -= l_Diff;
                        if (maxDist <= 0.0f)
                        {
                            _splineData.push_back(l_MoveSpline);
                            break;
                        }
                    }
                    FillVerticesPoint(l_Source);
                    break;
                }
                case AT_MOVE_TYPE_BOOMERANG: // 3
                {
                    if (_range != 0.0f)
                    {
                        uint32 l_MaxItr = (uint32)_range / 2;
                        for (uint32 l_Itr = 0; l_Itr < l_MaxItr; ++l_Itr)
                        {
                            AreaTriggerMoveSplines l_MoveSpline;
                            l_MoveSpline.m_path_x = l_Itr * 2;
                            l_MoveSpline.m_path_y = 0;
                            l_MoveSpline.m_path_z = 0;
                            _splineData.push_back(l_MoveSpline);
                        }

                        FillVerticesPoint(l_Source);

                        _spline.TimeToTarget /= 2;
                    }

                    break;
                }
                case AT_MOVE_TYPE_PART_PATH: // 9
                {
                    if (_range != 0.0f)
                    {
                        for (uint32 l_Itr = 0; l_Itr < (uint32)_range / 2; ++l_Itr)
                        {
                            AreaTriggerMoveSplines l_MoveSpline;
                            l_MoveSpline.m_path_x = l_Itr * 2;
                            l_MoveSpline.m_path_y = 0;
                            l_MoveSpline.m_path_z = 0;
                            _splineData.push_back(l_MoveSpline);
                        }
                        FillVerticesPoint(l_Source);
                    }
                    break;
                }
                case AT_MOVE_TYPE_LIMIT: // 1
                {
                    G3D::Vector3 curPos, nextPos;
                    pos.PositionToVector(curPos);
                    posMove.PositionToVector(nextPos);
                    _spline.VerticesPoints.push_back(curPos);
                    _spline.VerticesPoints.push_back(curPos);
                    _spline.VerticesPoints.push_back(nextPos);
                    _spline.VerticesPoints.push_back(nextPos);
                    _moveSpeed = (curPos - nextPos).length() / (duration - _waitTime) * 1000.0f;
                    break;
                }
                case AT_MOVE_TYPE_CHAGE_ROTATION: // 4 No WP
                    break;
                case AT_MOVE_TYPE_RE_PATH: // 5 only Divine Star
                {
                    G3D::Vector3 curPos, nextPos;
                    pos.PositionToVector(curPos);
                    _spline.VerticesPoints.push_back(curPos);
                    _spline.VerticesPoints.push_back(curPos);
                    _spline.TimeToTarget = (_range / _moveSpeed  * 1000.0f);
                    SetDuration(_spline.TimeToTarget);

                    uint8 countStep = uint8(_range / 3.0f);

                    for (uint8 step = 1; step <= countStep; step++)
                    {
                        float x = curPos.x + (step * 3.0f) * std::cos(pos.GetOrientation());
                        float y = curPos.y + (step * 3.0f) * std::sin(pos.GetOrientation());
                        JadeCore::NormalizeMapCoord(x);
                        JadeCore::NormalizeMapCoord(y);
                        nextPos.x = x;
                        nextPos.y = y;
                        nextPos.z = curPos.z;
                        _spline.VerticesPoints.push_back(nextPos);
                    }
                    _spline.VerticesPoints.push_back(nextPos);
                    break;
                }
                case AT_MOVE_TYPE_RE_PATH_LOS: // 8
                {
                    if (_range != 0.0f)
                    {
                        _moveSpeed = atInfo.speed ? atInfo.speed : 7.0f;
                        G3D::Vector3 curPos, nextPos;
                        Position _dest, _destAngle;
                        pos.PositionToVector(curPos);
                        _spline.VerticesPoints.push_back(curPos);
                        _spline.VerticesPoints.push_back(curPos);

                        if (GetDistance(posMove) > 0.0f && (posMove.GetPositionX() != 0.0f || posMove.GetPositionY() != 0.0f))
                        {
                            GetFirstCollisionPosition(_dest, _range, GetAngle(&posMove));

                            _dest.PositionToVector(nextPos);
                            _spline.VerticesPoints.push_back(nextPos);
                            _spline.VerticesPoints.push_back(nextPos);
                        }
                        else
                        {
                            GetFirstCollisionPosition(_dest, _range, (atInfo.AngleToCaster + GetOrientation()));

                            _dest.PositionToVector(nextPos);
                            _spline.VerticesPoints.push_back(nextPos);
                            _spline.VerticesPoints.push_back(nextPos);
                        }

                        float angleToAt = _dest.GetAngle(this);
                        _dest.SimplePosXYRelocationByAngle(_destAngle, 2.0f, angleToAt + static_cast<float>(M_PI/2), true);
                        GetFirstCollisionPosition(_destAngle, _range+10.0f, GetAngle(&_destAngle));

                        m_moveAngleLos = _dest.GetAngle(&_destAngle) - static_cast<float>(M_PI/2);

                        _spline.TimeToTarget = ((curPos - nextPos).length() / _moveSpeed  * 1000.0f);

                        sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AT_MOVE_TYPE_RE_PATH_LOS _timeToTarget %i _dest (%f %f %f) %f",
                        _spline.TimeToTarget, _dest.GetPositionX(), _dest.GetPositionY(), _dest.GetPositionZH(), m_moveAngleLos);
                    }
                    break;
                }
                case AT_MOVE_TYPE_CIRCLE:
                {
                    switch (_CircleData.Type)
                    {
                        case eAreaTriggerCircleTypes::CenterTypeCaster:
                        {
                            SetSource(*this);
                            _moveSpeed = atInfo.speed / _CircleData.Radius;
                            break;
                        }
                        case eAreaTriggerCircleTypes::CenterTypeSource:
                        {
                            SetSource(p_Pos);
                            _moveSpeed = 1.0f;
                            break;
                        }
                        default:
                            break;
                    }
                }
                case AT_MOVE_TYPE_BOUNCE_ON_COL:
                {
                    if (atInfo.speed > 0 && GetDuration() > 0)
                    {
                        float l_Distance = atInfo.speed * GetDuration() / 1000.0f;
                        uint32 l_MaxItr = (uint32)l_Distance;
                        for (uint32 l_Itr = 0; l_Itr < l_MaxItr; ++l_Itr)
                        {
                            AreaTriggerMoveSplines l_MoveSpline;
                            l_MoveSpline.m_path_x = l_Itr;
                            l_MoveSpline.m_path_y = 0;
                            l_MoveSpline.m_path_z = 0;
                            _splineData.push_back(l_MoveSpline);
                        }

                        FillVerticesPoint(l_Source);
                    }

                    break;
                }
                default:
                    break;
            }
        }
        else
            SetSource(pos, p_OriginalCaster->GetMap(), p_OriginalCaster->GetPhaseMask());

        if (_spline.VerticesPoints.size() > 1)
        {
            _nextMoveTime = (_spline.VerticesPoints[0] - _spline.VerticesPoints[1]).length() * _moveSpeed;
            for (size_t i = 0; i < _spline.VerticesPoints.size() - 1; ++i)
                m_moveDistanceMax += (_spline.VerticesPoints[i + 1] - _spline.VerticesPoints[i]).length();
        }
        else
            _nextMoveTime = 1000;

        if (_moveSpeed && atInfo.moveType != AT_MOVE_TYPE_SOURCE_ANGLE && atInfo.moveType != AT_MOVE_TYPE_BOOMERANG)
            _spline.TimeToTarget = int32(m_moveDistanceMax / _moveSpeed * 1000.0f);

        SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET, _spline.TimeToTarget ? _spline.TimeToTarget : _duration);
    }

    if (_duration != -1)
    {
        SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_DURATION, _duration);
        SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, _duration);
    }

    FillCustomData(p_OriginalCaster);

    setActive(true);

    /// Might happen during a zone phase switch
    if (GetMap() != p_OriginalCaster->GetMap())
        return false;

    if (atInfo.polygon && p_Pos != p_PosMove)
        SetOrientation(p_Pos.GetAngle(&p_PosMove));

    if (!GetMap()->AddToMap(this))
        return false;

    sScriptMgr->OnCreateAreaTriggerEntity(this);

    if (atInfo.maxCount && info)
    {
        std::list<AreaTrigger*> oldTriggers;
        p_OriginalCaster->GetAreaTriggerList(oldTriggers, info->Id);
        oldTriggers.sort(JadeCore::GuidValueSorterPred());
        while (oldTriggers.size() > atInfo.maxCount)
        {
            AreaTrigger* at = oldTriggers.front();
            oldTriggers.remove(at);
            if (at->GetCasterGUID() == p_OriginalCaster->GetGUID())
                at->Remove(false);
        }
    }
    UpdateAffectedList(0, AT_ACTION_MOMENT_SPAWN);

    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger end (spell %u) coordinates (X: %f Y: %f)", info ? info->Id : 0, GetPositionX(), GetPositionY());

    if (atInfo.ScaleCurveID && atInfo.HasDynamicShape)
        SetRadius(std::max<float>(GetCurveValueAt(atInfo.ScaleCurveID, 0.0f), FLT_MIN));

    if (sSpellMgr->IsInATPolygonDebug() && (atInfo.polygon || atInfo.hasAreaTriggerBox))
    {
        std::unordered_map<uint32, G3D::Vector2> l_Points;
        if (!atInfo.Polygon.VerticesTarget.empty())
            l_Points = atInfo.Polygon.VerticesTarget;
        else
            l_Points = atInfo.Polygon.Vertices;

        for (auto l_Iter : l_Points)
        {
            float l_X = l_Iter.second.x * std::cos(m_orientation) - l_Iter.second.y * std::sin(m_orientation) + m_positionX;
            float l_Y = l_Iter.second.x * std::sin(m_orientation) + l_Iter.second.y * std::cos(m_orientation) + m_positionY;

            if (Creature* l_Creature = p_OriginalCaster->SummonCreature(31146, Position(l_X, l_Y, m_positionZ, m_orientation)))
            {
                l_Creature->SetObjectScale(0.5f);
                l_Creature->DespawnOrUnsummon(200);
            }
        }
    }

    if (_ai)
        _ai->OnCreate();

    UpdateNeighbours();

    sLog->outInfo(LOG_FILTER_POINTERS, "Pointer 0x%" PRIxPTR " -> AreaTrigger Entry: %u \t\t SpellID: %u %f %f %f", std::uintptr_t(this), GetEntry(), GetSpellId(), GetPositionX(), GetPositionY(), GetPositionZ());

    return true;
}

bool AreaTrigger::LoadAreaTriggerFromDB(uint32 p_Guid, Map* p_Map)
{
    AreaTriggerData const* l_Data = sObjectMgr->GetAreaTriggerData(p_Guid);
    if (!l_Data)
    {
        sLog->outError(LOG_FILTER_SQL, "AreaTrigger (GUID: %u) not found in table `areatrigger`, can't load. ", p_Guid);
        return false;
    }

    m_DBTableGuid = p_Guid;

    if (p_Map->GetInstanceId() != 0)
        p_Guid = sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_AREATRIGGER);

    Position l_Pos = Position(l_Data->PosX, l_Data->PosY, l_Data->PosZ, l_Data->Orientation);

    SetMap(p_Map);
    Relocate(l_Pos);

    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_GENERAL, "AreaTrigger (AreaTrigger %u) not created. Suggested coordinates aren't valid (X: %f Y: %f)", l_Data->SpellID, GetPositionX(), GetPositionY());
        return false;
    }

    AreaTriggerInfo const* l_InfoAT = nullptr;
    if (l_Data->Entry)
        l_InfoAT = sObjectMgr->GetAreaTriggerInfo(l_Data->Entry);
    if (l_Data->CustomEntry)
        l_InfoAT = sObjectMgr->GetAreaTriggerInfoByEntry(l_Data->CustomEntry);

    if (l_InfoAT != nullptr)
    {
        atInfo = *l_InfoAT;
        _activationDelay = atInfo.activationDelay;

        for (AreaTriggerActionList::const_iterator itr = atInfo.actions.begin(); itr != atInfo.actions.end(); ++itr)
            _actionInfo[itr->id] = ActionInfo(&*itr);
    }

    if (l_Data->Radius)
        atInfo.Radius = l_Data->Radius;
    if (l_Data->RadiusTarget)
        atInfo.RadiusTarget = l_Data->RadiusTarget;
    if (l_Data->Height)
        atInfo.Polygon.Height = l_Data->Height;
    if (l_Data->HeightTarget)
        atInfo.Polygon.HeightTarget = l_Data->HeightTarget;
    if (l_Data->DecalPropertiesId)
        atInfo.DecalPropertiesId = l_Data->DecalPropertiesId;
    if (l_Data->MorphCurveId)
        atInfo.MorphCurveID = l_Data->MorphCurveId;
    if (l_Data->PhaseID)
    {
        /// Make a copy of the set because it can be changed in SetInPhase
        std::set<uint32> l_Phases = GetPhases();
        for (uint32 l_Phase : l_Phases)
            SetInPhase(l_Phase, false, false);
        SetInPhase(l_Data->PhaseID, true, true);
    }

    sScriptMgr->InitScriptEntity(this);

    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger (spell %u) coordinates (X: %f Y: %f) _actionInfo %lu", l_Data->SpellID, GetPositionX(), GetPositionY(), _actionInfo.size());

    Object::_Create(p_Guid, atInfo.customEntry, HIGHGUID_AREATRIGGER);

    SetPhaseMask(l_Data->PhaseMask, false);

    _realEntry = l_Data->Entry ? l_Data->Entry : l_Data->CustomEntry;
    SetEntry(atInfo.customEntry);

    uint32 l_Duration = m_spellInfo ? m_spellInfo->GetDuration() : 0;
    if (!l_Duration)
        l_Duration = -1;

    _canMove = atInfo.isMoving;
    m_UseMoveCurveID = !atInfo.isMoving && atInfo.MoveCurveID > 0;

    SetDuration(l_Duration);
    SetObjectScale(1);

    CalculateRadius();

    if (atInfo.Polygon.Height && !atInfo.polygon)
        _areaTriggerCylinder = true;

    SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, l_Data->SpellID);
    SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, l_Data->SpellID);
    SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, atInfo.VisualID);
    SetFloatValue(AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, atInfo.Radius);
    SetUInt32Value(AREATRIGGER_FIELD_EXTRA_SCALE_CURVE + 5, 1065353217);
    SetUInt32Value(AREATRIGGER_FIELD_EXTRA_SCALE_CURVE + 6, 1);

    if (atInfo.DecalPropertiesId)
        SetUInt32Value(AREATRIGGER_FIELD_DECAL_PROPERTIES_ID, atInfo.DecalPropertiesId);

    _range = _radius;

    if (atInfo.Distance != 0.0f)
        _range = atInfo.Distance;

    if (_range > 200.0f)
        _range = 75.0f;

    if (l_Duration != -1)
    {
        SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_DURATION, l_Duration);
        SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, l_Duration);
    }

    setActive(true);

    /// Might happen during a zone phase switch
    if (GetMap() != p_Map)
        return false;

    sScriptMgr->OnCreateAreaTriggerEntity(this);

    if (!GetMap()->AddToMap(this))
        return false;

    UpdateAffectedList(0, AT_ACTION_MOMENT_SPAWN);

    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger end (spell %u) coordinates (X: %f Y: %f)", l_Data->SpellID, GetPositionX(), GetPositionY());

    if (atInfo.ScaleCurveID && atInfo.HasDynamicShape)
        SetRadius(GetCurveValueAt(atInfo.ScaleCurveID, 0.0f));

    if (sSpellMgr->IsInATPolygonDebug() && atInfo.polygon)
    {
        std::unordered_map<uint32, G3D::Vector2> l_Points;
        if (!atInfo.Polygon.VerticesTarget.empty())
            l_Points = atInfo.Polygon.VerticesTarget;
        else
            l_Points = atInfo.Polygon.Vertices;

        for (auto l_Iter : l_Points)
        {
            float l_X = l_Iter.second.x * std::cos(m_orientation) - l_Iter.second.y * std::sin(m_orientation) + m_positionX;
            float l_Y = l_Iter.second.x * std::sin(m_orientation) + l_Iter.second.y * std::cos(m_orientation) + m_positionY;

            if (Creature* l_Creature = SummonCreature(31146, Position(l_X, l_Y, m_positionZ, m_orientation)))
                l_Creature->SetObjectScale(0.5f);
        }
    }

    sScriptMgr->OnWorldObjectCreate(this);

    UpdateNeighbours();

    return true;
}

void AreaTrigger::FillVerticesPoint(Position* l_Source, float p_MinDist /*= 0.0f*/)
{
    if (l_Source == nullptr)
        return;

    _spline.VerticesPoints.clear();
    Unit* l_Caster = GetCaster();
    float floorZ = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
    float lastZ = 0.0f;

    Position l_Src = *l_Source;

    int32 l_BreakIter = -1;
    for (uint32 l_Itr = 0; l_Itr < _splineData.size(); ++l_Itr)
    {
        float l_Orientation = l_Src.m_orientation + atInfo.AngleToCaster;
        G3D::Vector3 nextPos;
        float l_Cos = std::cos(l_Orientation);
        float l_Sin = std::sin(l_Orientation);
        float l_X = _splineData[l_Itr].m_path_x;
        float l_Y = _splineData[l_Itr].m_path_y;

        float x = l_Src.m_positionX + l_X * l_Cos - l_Y * l_Sin;
        float y = l_Src.m_positionY + l_X * l_Sin + l_Y * l_Cos;
        float z = l_Src.m_positionZ + _splineData[l_Itr].m_path_z;
        JadeCore::NormalizeMapCoord(x);
        JadeCore::NormalizeMapCoord(y);

        if (l_Itr == 0)
        {
            lastX = x;
            lastY = y;
            lastZ = z;
        }

        bool l_Break = false;

        /// check for collision - port from GetFirstCollisionPosition
        if (atInfo.moveType == AT_MOVE_TYPE_DEFAULT || atInfo.moveType == AT_MOVE_TYPE_RE_SHAPE || atInfo.moveType == AT_MOVE_TYPE_BOUNCE_ON_COL)
        {
            if (p_MinDist <= 0.0f || _length >= p_MinDist)
            {
                float l_ZOffset = atInfo.HasFollowsTerrain ? 1.0f : 0.5f;
                bool col = VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(GetMapId(), lastX, lastY, lastZ + l_ZOffset, x, y, lastZ + l_ZOffset, x, y, z, -l_ZOffset);
                // collision occurred
                if (col)
                {
                    // move back a bit
                    x -= CONTACT_DISTANCE * l_Cos;
                    y -= CONTACT_DISTANCE * l_Sin;
                    z -= CONTACT_DISTANCE;
                    l_Break = true;
                }

                // check dynamic collision
                col = GetMap()->getObjectHitPos(GetPhaseMask(), l_Src.m_positionX, l_Src.m_positionY, l_Src.m_positionZ + 0.5f, x, y, z + 0.5f, x, y, z, -0.5f);
                // Collided with a gameobject
                if (col)
                {
                    x -= CONTACT_DISTANCE * l_Cos;
                    y -= CONTACT_DISTANCE * l_Sin;
                    z -= CONTACT_DISTANCE;
                    l_Break = true;
                }
            }
        }

        nextPos.x = x;
        nextPos.y = y;

        if (atInfo.HasFollowsTerrain)
        {
            float upFloorZ = l_Caster->GetMap()->GetHeight(l_Caster->GetPhaseMask(), x, y, floorZ ? floorZ : l_Src.GetPositionZ(), true, 10.0f, true);
            float downFloorZ = l_Caster->GetMap()->GetHeight(l_Caster->GetPhaseMask(), x, y, floorZ ? floorZ : l_Src.GetPositionZ(), true, 10.0f, false);
            if (upFloorZ < downFloorZ)
                floorZ = downFloorZ;
            else
                floorZ = std::min(upFloorZ - floorZ, floorZ - downFloorZ) == upFloorZ - floorZ ? upFloorZ : downFloorZ;

            if (!l_Caster->GetMap()->isInLineOfSight(lastX, lastY, lastZ, x, y, z, l_Caster->GetPhaseMask()) && floorZ == downFloorZ)
                floorZ = upFloorZ;

            if (l_Itr && std::max(floorZ, _spline.VerticesPoints.back().z) - std::min(floorZ, _spline.VerticesPoints.back().z) > 10.0f)
                nextPos.z = _spline.VerticesPoints.back().z;
            else
                nextPos.z = floorZ + _splineData[l_Itr].m_path_z;
        }
        else
            nextPos.z = z;

        if (atInfo.moveType == AT_MOVE_TYPE_BOUNCE_ON_COL)
            nextPos.z = l_Src.m_positionZ;

        if ((nextPos.z - l_Src.m_positionZ) != _splineData[l_Itr].m_path_z)
            _splineData[l_Itr].m_path_z = nextPos.z - l_Src.m_positionZ;

        _spline.VerticesPoints.push_back(nextPos);

        Position _posA = Position(lastX, lastY, lastZ);
        lastX = nextPos.x;
        lastY = nextPos.y;
        lastZ = nextPos.z;
        Position _posB = Position(lastX, lastY, lastZ);

        if (l_Itr == 0)
            continue;

        _length += _posA.GetExactDist(&_posB);

        /// We hit an object and need to stop
        if (l_Break)
        {
            if (atInfo.moveType != AT_MOVE_TYPE_BOUNCE_ON_COL)
            {
                l_BreakIter = l_Itr;
                if (atInfo.moveType == AT_MOVE_TYPE_DEFAULT && _range)
                    _spline.TimeToTarget = static_cast<int32>(static_cast<float>(_spline.TimeToTarget) * _length / _range);
                break;
            }

            /// Handle bounce if needed
            Position l_NullPos = Position();
            if (m_BounceRef.IsNearPosition(&l_NullPos, 0.1f))
            {
                l_BreakIter = l_Itr;
                break;
            }

            m_BouncePos = _posB;
            l_BreakIter = l_Itr;
            break;
        }
    }

    if (l_BreakIter > 1)
        _splineData.resize(l_BreakIter + 1);

    while (_spline.VerticesPoints.size() < 3)
        _spline.VerticesPoints.push_back(_spline.VerticesPoints.back());

    _dest = WorldLocation(l_Caster->GetMapId(), lastX, lastY, lastZ, 0.0f);
}

void AreaTrigger::BounceAreaTrigger(float p_ForcedAngle /*= 0.0f*/, float p_MinDist /*= 0.0f*/)
{
    if (atInfo.speed > 0 && GetDuration() > 0 && _moveSpeed > 0)
    {
        /// PosB should be collision point, PosA is the point right before
        /// Arrival angle
        float l_Angle       = Position::NormalizeOrientation(m_orientation + M_PI);
        /// Wall angle
        float l_RefAngle    = GetAngle(&m_BounceRef);
        /// Angle between wall and ray
        float l_RayAngle    = l_RefAngle > l_Angle ? l_RefAngle - l_Angle : l_Angle - l_RefAngle;
        float l_BounceAngle = 0.0f;

        if (l_RefAngle > l_Angle)
            l_BounceAngle = l_RefAngle + l_RayAngle;
        else
            l_BounceAngle = l_RefAngle - l_RayAngle;

        m_BounceSrc = *this;
        m_BounceSrc.m_orientation = Position::NormalizeOrientation(l_BounceAngle);

        if (p_ForcedAngle)
            m_BounceSrc.m_orientation = p_ForcedAngle;

        _MoveElapsedDuration = 0;
        m_moveDistanceMax = 0;
        _length = 0;

        _times.clear();
        _splineData.clear();

        AreaTriggerMoveSplines l_MoveSpline;

        l_MoveSpline.m_path_x = 0;
        l_MoveSpline.m_path_y = 0;
        l_MoveSpline.m_path_z = 0;

        _splineData.push_back(l_MoveSpline);

        float l_Distance = atInfo.speed * GetDuration() / 1000.0f;
        uint32 l_MaxItr = (uint32)l_Distance;
        for (uint32 l_Itr = 0; l_Itr < l_MaxItr; ++l_Itr)
        {
            l_MoveSpline.m_path_x = l_Itr;
            l_MoveSpline.m_path_y = 0;
            l_MoveSpline.m_path_z = 0;
            _splineData.push_back(l_MoveSpline);
        }

        FillVerticesPoint(&m_BounceSrc);

        if (_spline.VerticesPoints.size() > 1)
        {
            _nextMoveTime = (_spline.VerticesPoints[0] - _spline.VerticesPoints[1]).length() * _moveSpeed;
            for (size_t i = 0; i < _spline.VerticesPoints.size() - 1; ++i)
                m_moveDistanceMax += (_spline.VerticesPoints[i + 1] - _spline.VerticesPoints[i]).length();
        }
        else
            _nextMoveTime = 1000;

        _spline.TimeToTarget = int32(m_moveDistanceMax / _moveSpeed * 1000.0f);
        _spline.ElapsedTimeForMovement = _liveTime;

        WorldPackets::Spells::AreaTriggerRePath l_RePath;
        l_RePath.TriggerGUID = GetGUID();
        l_RePath.Spline.emplace();
        l_RePath.Spline = _spline;
        _caster->SendMessageToSet(const_cast<WorldPacket*>(l_RePath.Write()), true);
    }
}

void AreaTrigger::UpdateMovement(uint32 diff)
{
    Unit* l_Caster = GetCaster();
    if (atInfo.HasAttached)
    {
        Unit* l_AttachTarget = l_Caster;
        if (Unit* l_Target = ObjectAccessor::GetUnit(*this, GetTargetGuid()))
            l_AttachTarget = l_Target;

        if (l_AttachTarget)
        {
            m_positionX   = l_AttachTarget->m_positionX;
            m_positionY   = l_AttachTarget->m_positionY;
            m_positionZ   = l_AttachTarget->m_positionZ;
            m_orientation = l_AttachTarget->m_orientation;
        }

        return;
    }

    ObjectGuid targetGuid = GetTargetGuid();
    if (targetGuid)
    {
        if (!UpdatePosition(targetGuid))
        {
            if (GetDuration() > 100)
                SetDuration(100);
            return;
        }
    }

    if (!isMoving() || _spline.VerticesPoints.empty())
        return;

    if (m_moveDistance >= m_moveDistanceMax || m_currentNode >= _spline.VerticesPoints.size() - 1)
    {
        // sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateMovement NOT RUN m_currentNode %i size %i _duration %i _timeToTarget %i diff %i _moveTime %i m_moveDistance %f m_moveDistanceMax %f",
        // m_currentNode, _spline.VerticesPoints.size(), _duration, _spline.TimeToTarget, diff, _moveTime, m_moveDistance, m_moveDistanceMax);
        return;
    }

    if (_waitTime > 0)
    {
        if (diff >= _waitTime)
            _waitTime = 0;
        else
            _waitTime -= diff;
        return;
    }

    Position tempPos;
    tempPos.VectorToPosition(_spline.VerticesPoints[m_currentNode]);
    float angle = tempPos.GetAngle(_spline.VerticesPoints[m_currentNode + 1].x, _spline.VerticesPoints[m_currentNode + 1].y);

    //float speed = getMoveSpeed() / (((_spline.VerticesPoints.size() - 1) / 4) * m_currentNode);
    float speed = getMoveSpeed();
    if (atInfo.moveType == AT_MOVE_TYPE_SPIRAL)
    {
        int32 mod = int32(m_currentNode / 4);
        if (mod)
            speed = getMoveSpeed() / 2 * mod;
        else
            speed = getMoveSpeed() / 4;
    }
    _moveTime += diff;

    if (_moveTime >= _nextMoveTime)
    {
        float distanceMove = (speed * _moveTime) / 1000.0f;
        tempPos.SimplePosXYRelocationByAngle(*this, distanceMove, angle, true);

        m_moveDistance += distanceMove;
        m_moveLength += distanceMove;

        float lengthMove = 0.0f;
        // Calculate patch move
        for (uint16 i = m_currentNode; i < _spline.VerticesPoints.size() - 1; ++i)
        {
            lengthMove += (_spline.VerticesPoints[i + 1] - _spline.VerticesPoints[i]).length();

            sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateMovement m_currentNode %i lengthMove %f m_moveLength %f m_moveDistance %f", m_currentNode, lengthMove, m_moveLength, m_moveDistance);

            if (m_moveDistance >= lengthMove)
            {
                m_currentNode = i + 1;
                m_moveLength -= lengthMove;

                /// Reach destination
                if (m_currentNode >= _spline.VerticesPoints.size() - 1)
                    sScriptMgr->OnDestinationReached(this);

                break;
            }
        }
        //m_currentNode++;
        _moveTime = 0;

        switch (atInfo.moveType)
        {
            case AT_MOVE_TYPE_RE_PATH:
            {
                //if (m_currentNode == _spline.VerticesPoints.size() - 1)
                if (m_moveDistance >= m_moveDistanceMax || m_currentNode >= _spline.VerticesPoints.size() - 1)
                {
                    float dist = GetExactDist2d(_caster);
                    if (dist > 3.0f)
                    {
                        _spline.VerticesPoints.clear();
                        m_currentNode = 1;
                        m_moveLength = 0.0f;
                        m_moveDistance = 0.0f;
                        m_moveDistanceMax = 0.0f;
                        G3D::Vector3 curPos, nextPos;
                        PositionToVector(curPos);
                        _spline.VerticesPoints.push_back(curPos);
                        _spline.VerticesPoints.push_back(curPos);

                        _caster->PositionToVector(nextPos);
                        _spline.VerticesPoints.push_back(nextPos);
                        _spline.VerticesPoints.push_back(nextPos);

                        for (size_t i = 0; i < _spline.VerticesPoints.size() - 1; ++i)
                            m_moveDistanceMax += (_spline.VerticesPoints[i + 1] - _spline.VerticesPoints[i]).length();

                        _spline.TimeToTarget = int32(m_moveDistanceMax / _moveSpeed  * 1000.0f);
                        _spline.ElapsedTimeForMovement = _liveTime;
                        SetDuration(_spline.TimeToTarget);

                        WorldPackets::Spells::AreaTriggerRePath rePath;
                        rePath.TriggerGUID = GetGUID();
                        rePath.Spline.emplace();
                        rePath.Spline = _spline;
                        _caster->SendMessageToSet(const_cast<WorldPacket*>(rePath.Write()), true);
                    }

                    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateMovement AT_MOVE_TYPE_RE_PATH m_currentNode %i size %lu _timeToTarget %i dist %f", m_currentNode, _spline.VerticesPoints.size(), _spline.TimeToTarget, dist);
                }
                break;
            }
            case AT_MOVE_TYPE_RE_PATH_LOS: //8
            {
                if (m_moveDistance >= m_moveDistanceMax || m_currentNode >= _spline.VerticesPoints.size() - 1)
                {
                    Position startPos, _dest, _destAngle;
                    startPos.VectorToPosition(_spline.VerticesPoints[0]);

                    _spline.VerticesPoints.clear();
                    m_currentNode = 1;
                    m_moveLength = 0.0f;
                    m_moveDistance = 0.0f;
                    m_moveDistanceMax = 0.0f;
                    G3D::Vector3 curPos, nextPos;
                    PositionToVector(curPos);
                    _spline.VerticesPoints.push_back(curPos);
                    _spline.VerticesPoints.push_back(curPos);

                    GetFirstCollisionPosition(_dest, _range, m_moveAngleLos);

                    _dest.PositionToVector(nextPos);
                    _spline.VerticesPoints.push_back(nextPos);
                    _spline.VerticesPoints.push_back(nextPos);

                    for (size_t i = 0; i < _spline.VerticesPoints.size() - 1; ++i)
                        m_moveDistanceMax += (_spline.VerticesPoints[i + 1] - _spline.VerticesPoints[i]).length();

                    _spline.TimeToTarget = int32(m_moveDistanceMax / _moveSpeed  * 1000.0f);
                    _spline.ElapsedTimeForMovement = _liveTime;

                    float angleToAt = _dest.GetAngle(this);
                    _dest.SimplePosXYRelocationByAngle(_destAngle, 2.0f, angleToAt + static_cast<float>(M_PI/2), true);
                    GetFirstCollisionPosition(_destAngle, _range+10.0f, GetAngle(&_destAngle));

                    m_moveAngleLos = _dest.GetAngle(&_destAngle) - static_cast<float>(M_PI/2);
                    WorldPackets::Spells::AreaTriggerRePath rePath;
                    rePath.TriggerGUID = GetGUID();
                    rePath.Spline.emplace();
                    rePath.Spline = _spline;
                    _caster->SendMessageToSet(const_cast<WorldPacket*>(rePath.Write()), true);

                    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateMovement AT_MOVE_TYPE_RE_PATH_LOS m_currentNode %i size %lu _timeToTarget %i _dest (%f %f %f) %f",
                    m_currentNode, _spline.VerticesPoints.size(), _spline.TimeToTarget, _dest.GetPositionX(), _dest.GetPositionY(), _dest.GetPositionZ(), m_moveAngleLos);
                }
                break;
            }
        }
        if (m_moveDistance >= m_moveDistanceMax || m_currentNode >= _spline.VerticesPoints.size() - 1)
        {
            sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateMovement STOP MOVE m_currentNode %i size %lu _duration %i _duration %i diff %i _moveTime %i distanceMove %f m_moveDistance %f m_moveDistanceMax %f",
            m_currentNode, _spline.VerticesPoints.size(), _duration, _spline.TimeToTarget, diff, _moveTime, distanceMove, m_moveDistance, m_moveDistanceMax);
            return;
        }

        float speedNext = getMoveSpeed();
        switch (atInfo.moveType)
        {
            case AT_MOVE_TYPE_SPIRAL:
            {
                int32 mod = int32(m_currentNode / 4);
                if (mod)
                    speedNext = getMoveSpeed() / 2 * mod;
                else
                    speedNext = getMoveSpeed() / 4;
                break;
            }
        }

        _nextMoveTime = (_spline.VerticesPoints[m_currentNode] - _spline.VerticesPoints[m_currentNode+1]).length() / speedNext * 1000;

        // _caster->SummonCreature(44548, GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(),TEMPSUMMON_TIMED_DESPAWN, 10000); // For visual point test
        sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateMovement speedNext %f _nextMoveTime %u length %f abs %i m_currentNode %i size %lu _timeToTarget %i diff %i _moveTime %i distanceMove %f m_moveDistance %f m_moveDistanceMax %f",
            speedNext, _nextMoveTime, (_spline.VerticesPoints[m_currentNode] - _spline.VerticesPoints[m_currentNode + 1]).length(), int32(m_currentNode / 4), m_currentNode, _spline.VerticesPoints.size(), _spline.TimeToTarget, diff, _moveTime, distanceMove, m_moveDistance, m_moveDistanceMax);
    }
    else
        tempPos.SimplePosXYRelocationByAngle(*this, (speed * _moveTime) / 1000.0f, angle, true);

    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateMovement %f %f %f %f %i angle %f _nextMoveTime %i m_currentNode %i speed %f", GetPositionX(), GetPositionY(), GetPositionZ(), getMoveSpeed(), _moveTime, angle, _nextMoveTime, m_currentNode, speed);

    if (sSpellMgr->IsInATDebug())
    {
        if (Unit* l_Caster = GetCaster())
        {
            if (Creature* l_Creature = l_Caster->SummonCreature(31146, *this))
            {
                l_Creature->SetObjectScale(0.5f);
                l_Creature->DespawnOrUnsummon(200);
            }
        }
    }
}

void AreaTrigger::UpdatePolygon()
{
    if (!atInfo.MorphCurveID)
        return;

    auto l_Iter = g_CurvePoints.find(atInfo.MorphCurveID);
    if (l_Iter == g_CurvePoints.end())
        return;

    if (l_Iter->second.empty())
        return;

    CurvePointEntry const* l_LastPoint = nullptr;
    CurvePointEntry const* l_NextPoint = nullptr;

    float l_TimeProgress = float(_liveTime) / float(m_spellInfo ? m_spellInfo->GetDuration() : 0);
    for (auto l_PointItr = l_Iter->second.begin(); l_PointItr != l_Iter->second.end(); ++l_PointItr)
    {
        if (l_TimeProgress > (*l_PointItr)->X)
            continue;

        l_NextPoint = *l_PointItr;

        if (l_PointItr == l_Iter->second.begin())
            return;

        l_LastPoint = *(l_PointItr - 1);

        break;
    }

    if (!l_LastPoint || !l_NextPoint)
        return;

    float l_DistProgress = (l_NextPoint->Y - l_LastPoint->Y) * ((l_TimeProgress - l_LastPoint->X) / (l_NextPoint->X - l_LastPoint->X)) + l_LastPoint->Y;

    if (m_Polygon.empty())
        m_Polygon = atInfo.Polygon.Vertices;

    for (uint32 l_I = 0; l_I < m_Polygon.size(); ++l_I)
    {
        m_Polygon[l_I] = ((atInfo.Polygon.VerticesTarget[l_I] - atInfo.Polygon.Vertices[l_I]) * l_DistProgress) + atInfo.Polygon.Vertices[l_I];

        if (sSpellMgr->IsInATPolygonDebug())
        {
            float l_X = m_Polygon[l_I].x * std::cos(m_orientation) - m_Polygon[l_I].y * std::sin(m_orientation) + m_positionX;
            float l_Y = m_Polygon[l_I].x * std::sin(m_orientation) + m_Polygon[l_I].y * std::cos(m_orientation) + m_positionY;

            if (Creature* l_Creature = GetCaster()->SummonCreature(31146, l_X, l_Y, m_positionZ))
            {
                l_Creature->SetObjectScale(0.5f);
                l_Creature->DespawnOrUnsummon(200);
            }
        }
    }

    SetRadius(CalculateRadiusPolygon());
}

void AreaTrigger::UpdatePositionWithPathId(uint32 p_Time, Position* p_OutPos)
{
    if (!_times.size())
    {
        SpellInfo const* l_SpellInfo = GetSpellInfo();
        if (!l_SpellInfo)
            return;

        uint32 l_TimeToTarget = _spline.TimeToTarget;

        if (!l_TimeToTarget)
            return;

        if (UseMoveCurveID())
        {
            auto l_Iter = g_CurvePoints.find(atInfo.MoveCurveID);
            if (l_Iter == g_CurvePoints.end())
                return;

            for (auto l_Itr : l_Iter->second)
                _times.push_back(l_TimeToTarget * l_Itr->X);
        }
        else
        {
            float l_Speed = _length / l_TimeToTarget;
            if (!l_Speed)
                return;

            for (uint32 l_Itr = 0; l_Itr < _splineData.size(); ++l_Itr)
            {
                if (l_Itr == 0)
                {
                    _times.push_back(0);
                    continue;
                }
                else if (l_Itr == _splineData.size() - 1)
                {
                    _times.push_back(l_TimeToTarget);
                    continue;
                }

                Position _posA = (_splineData[l_Itr].ToPosition());
                Position _posB = (_splineData[l_Itr - 1].ToPosition());

                float l_LocalDistance = _posA.GetExactDist(&_posB);
                _times.push_back((l_LocalDistance / l_Speed) + _times[l_Itr - 1]);
            }
        }
    }

    if (!_splineData.size() || (_times.size() > _splineData.size()))
        return;

    uint32 l_PathId = 0;
    float l_Progress = 0.0f;

    for (uint32 l_Itr = 0; l_Itr < _times.size() - 1; ++l_Itr)
    {
        if (p_Time > _times[l_Itr] && p_Time < _times[l_Itr + 1])
        {
            l_PathId = l_Itr;
            l_Progress = ((float)p_Time - (float)_times[l_Itr]) / ((float)_times[l_Itr + 1] - (float)_times[l_Itr]);
            break;
        }

        if ((l_Itr == (_times.size() - 2)) && p_Time > _times[l_Itr + 1])
        {
            l_PathId = l_Itr;
            l_Progress = 1;
        }
    }

    if (_splineData.size() < l_PathId + 2)
        return;
    else if ((_splineData.size() == l_PathId + 2) && l_Progress == 1)
    {
        _IsMoving = false;
        if (atInfo.moveType == AT_MOVE_TYPE_BOOMERANG)
        {
            if (Unit* l_Caster = GetCaster())
            {
                atInfo.moveType = AT_MOVE_TYPE_TO_TARGET;
                _targetGuid = l_Caster->GetGUID();
                atInfo.AngleToCaster = 0;
                _length = 0;
                _times.clear();

                switch (m_spellInfo->Id)
                {
                    case 110744: ///< Divine Star follows the target wherever it is
                    case 193559: ///< Throw Glaive (Glayvianna Soulrender) follows the caster wherever it is
                    case 193578: ///< Throw Glaive (Glayvianna Soulrender) follows the caster wherever it is
                        atInfo.HasFollowsTerrain = false;
                        break;
                    default:
                        break;
                }
            }
        }
        else
            sScriptMgr->OnDestinationReached(this);
    }
    else
        _IsMoving = true;

    // We want to interpolate so that to get true position on server side.
    AreaTriggerMoveSplines l_spline0 = _splineData[l_PathId];
    AreaTriggerMoveSplines l_spline1 = _splineData[l_PathId + 1];
//
//     if (l_spline0.m_move_id == 0 || l_spline1.m_move_id == 0)
//     {
//         sLog->outError(LOG_FILTER_GENERAL, "AreaTrigger Move Splines (entry %u) not in DB.", atInfo.MoveCurveID);
//         return; // ERROR.
//     }

    AreaTriggerSource l_Source = GetSource();
    if (l_Source.map == nullptr)
    {
        Unit* l_Caster = GetCaster();
        l_Source = SetSource(l_Caster->m_positionX, l_Caster->m_positionY, l_Caster->m_positionZ, l_Caster->GetOrientation() + atInfo.AngleToCaster, l_Caster->GetMap());
    }

    if (l_Source.map == nullptr)
    {
        sLog->outError(LOG_FILTER_GENERAL, "AreaTrigger Move Splines (entry %u) no source found, or map pointer is nullptr.", atInfo.MoveCurveID);
        return; // ERROR.
    }

    Position l_NullPos = Position();
    if (atInfo.moveType == AT_MOVE_TYPE_BOUNCE_ON_COL && !m_BounceSrc.IsNearPosition(&l_NullPos, 0.1f))
    {
        l_Source.x = m_BounceSrc.m_positionX;
        l_Source.y = m_BounceSrc.m_positionY;
        l_Source.z = m_BounceSrc.m_positionZ;
        l_Source.o = m_BounceSrc.m_orientation;
    }

    float l_Cos = std::cos(l_Source.o);
    float l_Sin = std::sin(l_Source.o);
    float l_X = l_spline0.m_path_x + l_Progress * (l_spline1.m_path_x - l_spline0.m_path_x);
    float l_Y = l_spline0.m_path_y + l_Progress * (l_spline1.m_path_y - l_spline0.m_path_y);

    float lastX = p_OutPos->m_positionX;
    float lastY = p_OutPos->m_positionY;
    float lastZ = p_OutPos->m_positionZ;

    p_OutPos->m_positionX = l_Source.x + l_X * l_Cos - l_Y * l_Sin;
    p_OutPos->m_positionY = l_Source.y + l_X * l_Sin + l_Y * l_Cos;
    p_OutPos->m_positionZ = l_Source.z + l_spline0.m_path_z + l_Progress * (l_spline1.m_path_z - l_spline0.m_path_z);
    p_OutPos->m_orientation = l_Source.o;

    if (sSpellMgr->IsInATDebug())
    {
        if (Unit* l_Caster = GetCaster())
        {
            if (Creature* l_Creature = l_Caster->SummonCreature(31146, *p_OutPos))
            {
                l_Creature->SetObjectScale(0.5f);
                l_Creature->DespawnOrUnsummon(200);
            }
        }
    }
}

void AreaTrigger::UpdatePositionForCircle(Position* p_OutPos)
{
    Unit* l_Caster = GetCaster();
    if (l_Caster == nullptr)
        return;

    float l_Angle = -(_moveSpeed * _liveTime / 1000.0f);
    l_Angle += _CircleData.InitialAngle;

    Position l_Center = Position();

    switch (_CircleData.Type)
    {
        case eAreaTriggerCircleTypes::CenterTypeCaster:
        {
            if (l_Caster->IsPlayer())
                l_Angle += l_Caster->GetOrientation();

            l_Center = *l_Caster;
            break;
        }
        case eAreaTriggerCircleTypes::CenterTypeSource:
        {
            if (_CircleData.Clockwise)
            {
                l_Angle = (_moveSpeed * _liveTime / 1000.0f);
                l_Angle -= _CircleData.InitialAngle;
            }
            else
            {
                l_Angle = -(_moveSpeed * _liveTime / 1000.0f);
                l_Angle += _CircleData.InitialAngle;
            }

            l_Center = GetSource().GetPosition();
            break;
        }
        default:
            return;
    }

    l_Angle = Position::NormalizeOrientation(l_Angle);

    float l_TempX = _CircleData.Radius;

    /// Apply rotation
    float l_X = l_TempX * std::cos(l_Angle);
    float l_Y = l_TempX * std::sin(l_Angle);

    l_X += l_Center.m_positionX;
    l_Y += l_Center.m_positionY;

    JadeCore::NormalizeMapCoord(l_X);
    JadeCore::NormalizeMapCoord(l_Y);

    /// This prevents some infinite loops in VMaps calculations, it's hacky, but I didn't found a better way
    if (std::isnan(l_X) || std::isnan(l_Y))
        return;

    p_OutPos->m_positionX = l_X;
    p_OutPos->m_positionY = l_Y;
    p_OutPos->m_positionZ = GetMap()->GetHeight(GetPhaseMask(), l_X, l_Y, _CircleData.Type == eAreaTriggerCircleTypes::CenterTypeCaster ? l_Caster->m_positionZ : _source.z);

    if (sSpellMgr->IsInATDebug())
    {
        if (Creature* l_Creature = l_Caster->SummonCreature(31146, *p_OutPos))
        {
            l_Creature->SetObjectScale(0.5f);
            l_Creature->DespawnOrUnsummon(200);
        }
    }
}

void AreaTrigger::UpdateRotation(uint32 diff)
{
    if (!isMoving() || !_spline.VerticesPoints.empty())
        return;

    //sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AreaTrigger::UpdateRotation _waitTime %i _moveTime %i _nextMoveTime %i diff %i speed %f o %f", _waitTime, _moveTime, _nextMoveTime, diff, getMoveSpeed(), GetOrientation());

    if (_waitTime > 0)
    {
        if (diff >= _waitTime)
            _waitTime = 0;
        else
            _waitTime -= diff;
        return;
    }

    _moveTime += diff;

    if (_moveTime >= _nextMoveTime)
    {
        float speed = getMoveSpeed();
        float o = GetOrientation() + (speed * (_moveTime / 1000));
        _moveTime = 0;
        _nextMoveTime = 1000;
        SetOrientation(o);
        //float x = GetPositionX() + 12 * std::cos(o);
        //float y = GetPositionY() + 12 * std::sin(o);
        //_caster->SummonCreature(44548, x, y, GetPositionZ(), o,TEMPSUMMON_TIMED_DESPAWN, 20000); // For visual point test
    }
}

Position AreaTriggerMoveSplines::ToPosition()
{
    return Position(m_path_x, m_path_y, m_path_z);
}

void AreaTrigger::MoveAreaTrigger(Position const p_Position, uint32 p_TimeToTargetOrSpeed /*= 0*/, bool p_IsSpeed /*= false*/, bool p_FollowTerrain /*= true*/)
{
    Unit* l_Caster = GetCaster();
    if (!l_Caster)
        return;

    if (p_FollowTerrain)
        atInfo.HasFollowsTerrain = true;

    _times.clear();
    _splineData.clear();

    Position l_Source = *this;
    _range = l_Source.GetExactDist2d(&p_Position);
    float l_DistZ = p_Position.GetPositionZ() - l_Source.GetPositionZ();
    l_Source.SetOrientation(l_Source.GetAngle(&p_Position));
    SetSource(l_Source.m_positionX, l_Source.m_positionY, l_Source.m_positionZ, l_Source.m_orientation, l_Caster->GetMap());
    if (_range != 0.0f)
    {
        AreaTriggerMoveSplines l_MoveSpline;
        l_MoveSpline.m_path_x = 0;
        l_MoveSpline.m_path_y = 0;
        l_MoveSpline.m_path_z = 0;
        _splineData.push_back(l_MoveSpline);

        /// Can happen with really small x/y moves (mostly used for Z moves only, but with like 0.1 on x or y)
        if (!(uint32)_range)
            _splineData.push_back(l_MoveSpline);

        for (uint32 l_Itr = 0; l_Itr < (uint32)_range / 2; ++l_Itr)
        {
            l_MoveSpline.m_path_x = l_Itr * 2;
            if (!p_FollowTerrain)
                l_MoveSpline.m_path_z = l_DistZ * (float(l_Itr) / float((uint32)_range / 2));
            _splineData.push_back(l_MoveSpline);
        }

        l_MoveSpline.m_path_x = _range;
        if (!p_FollowTerrain)
            l_MoveSpline.m_path_z = l_DistZ;

        _splineData.push_back(l_MoveSpline);
        _splineData.push_back(l_MoveSpline);
        _length = 0;
        FillVerticesPoint(&l_Source);
    }

    if (p_IsSpeed && p_TimeToTargetOrSpeed)
        atInfo.timeToTarget = (uint32)((_length / (float)p_TimeToTargetOrSpeed) * (float)IN_MILLISECONDS);
    else
        atInfo.timeToTarget = p_TimeToTargetOrSpeed;

    sScriptMgr->OnMoveAreaTrigger(this, atInfo.timeToTarget);

    WorldPackets::Spells::AreaTriggerRePath l_RePath;
    l_RePath.TriggerGUID = GetGUID();
    _MoveElapsedDuration = 0;
    _spline.ElapsedTimeForMovement = _liveTime;
    _spline.TimeToTarget = atInfo.timeToTarget;
    l_RePath.Spline.emplace();
    l_RePath.Spline = _spline;
    l_Caster->SendMessageToSet(const_cast<WorldPacket*>(l_RePath.Write()), true);
}

void AreaTrigger::MoveAreaTrigger(WorldLocation const* p_Dest, uint32 p_TimeToTargetOrSpeed /*= 0*/, bool p_IsSpeed /*= false*/, bool p_FollowTerrain /*= true*/)
{
    MoveAreaTrigger(*p_Dest, p_TimeToTargetOrSpeed, p_IsSpeed, p_FollowTerrain);
}

void AreaTrigger::MoveAreaTriggerCircle(Position const p_Center, float p_Radius, uint32 p_TimeToTargetOrSpeed /*= 0*/, bool p_IsSpeed /*= false*/, bool p_Clockwise /*= true*/)
{
    Unit* l_Caster = GetCaster();
    if (!l_Caster)
        return;

    AreaTriggerCircle l_CircleData;
    WorldPackets::Spells::AreaTriggerRePath l_RePath;

    l_CircleData.CenterPoint.emplace();

    l_CircleData.CenterPoint        = G3D::Vector3(p_Center.m_positionX, p_Center.m_positionY, p_Center.m_positionZ);
    l_CircleData.TimeToTarget       = p_TimeToTargetOrSpeed;
    l_CircleData.Clockwise          = p_Clockwise;
    l_CircleData.dword30            = _liveTime;
    l_CircleData.Radius             = p_Radius;
    l_CircleData.BlendFormRadius    = p_Radius;
    l_CircleData.InitialAngle       = GetSource().o;

    l_RePath.TriggerGUID           = GetGUID();

    l_RePath.CircleData.emplace();
    l_RePath.CircleData            = l_CircleData;

    _CircleData                     = l_CircleData;

    _CircleData.Type                = eAreaTriggerCircleTypes::CenterTypeSource;

    l_Caster->SendMessageToSet(const_cast<WorldPacket*>(l_RePath.Write()), true);

    SetMoveType(AT_MOVE_TYPE_CIRCLE);

    _moveSpeed = (2.0f * M_PI * p_Radius) / float(p_TimeToTargetOrSpeed) * 1000.0f / p_Radius;
}

bool AreaTrigger::_IsWithinDist(WorldObject const* p_Obj, float p_MaxDist, bool p_is3D) const
{
    if (IsCheckingTargetWithObjectSize())
        p_MaxDist += (GetObjectSize() + p_Obj->GetObjectSize());

    if (m_transport && p_Obj->GetTransport() && p_Obj->GetTransport()->GetGUIDLow() == m_transport->GetGUIDLow())
    {
        float dtx = m_movementInfo.t_pos.m_positionX - p_Obj->m_movementInfo.t_pos.m_positionX;
        float dty = m_movementInfo.t_pos.m_positionY - p_Obj->m_movementInfo.t_pos.m_positionY;
        float disttsq = dtx * dtx + dty * dty;

        if (p_is3D)
        {
            float dtz = m_movementInfo.t_pos.m_positionZ - p_Obj->m_movementInfo.t_pos.m_positionZ;
            disttsq += dtz * dtz;
        }

        return disttsq < (p_MaxDist * p_MaxDist);
    }

    if (p_is3D)
        return GetExactDistSq(p_Obj) < (p_MaxDist * p_MaxDist);

    return GetExactDist2dSq(p_Obj) < (p_MaxDist * p_MaxDist);
}

bool AreaTrigger::IsCheckingTargetWithObjectSize() const
{
    switch (GetRealEntry())
    {
        case 9352: case 9357: case 9358:    ///< ToV: Guarm Red Breath
        case 9353: case 9359: case 9360:    ///< ToV: Guarm Green Breath
        case 9354: case 9355: case 9356:    ///< ToV: Guarm Purple Breath
        case 6505: case 6512:               ///< The Nighthold: Elisande: Time bubbles
        case 11124: case 11128:             ///< The Nighthold: Elisande: Time bubbles
        case 9183: case 9345:               ///< The Archmage's Reckoning: Xylem: Shadow Barrage/Razor Ice
        case 9551:                          ///< The Highlord's Return: Aura of Decay
        case 10989: case 11020:             ///< ABT: Imonar the Soulhunter's Pulse Grenade
        case 11006:                         ///< ABT: Imonar the Soulhunter's Blastwire
        case 10981:                         ///< ABT: Imonar the Soulhunter's Stasis Trap
        case 11004:                         ///< ABT: Imonar the Soulhunter's Shrapnel Blast
        case 11238: case 11028:             ///< ABT: Imonar the Soulhunter's Conflagration
        case 10886:                         ///< ABT: Kin'garoth - Diabolic Bomb
        case 10653:                         ///< ABT: Aggramar - Wake of Flame
        case 11023:                         ///< ABT: Argus - Death Fog
        case 12091:                         ///< ABT: Argus - Apocalypsis Zone
        case 12068:                         ///< ABT: Argus - Sargeras' Rage
        case 7132:                          ///< VoTW's Cordana 'Creeping Doom'
        case 5130:                          ///< VoTW's Glazer 'Pulse'
            return false;
        default:
            break;
    }

    switch (atInfo.spellId)
    {
        case 194232:                        ///< Maw of Souls: Harbaron's Nether Rip
        case 226489:                        ///< Sanguine Affixe
            return false;
        default:
            break;
    }

    return atInfo.isCheckingWithObjectSize;
}

void AreaTrigger::ReShape(uint32 p_NewShape, bool p_UnkBit)
{
    if (!GetCaster())
        return;

    WorldPackets::Spells::AreaTriggerReShape l_ReShape;
    l_ReShape.TriggerGUID = GetGUID();
    l_ReShape.ShapeID = p_NewShape;
    l_ReShape.UnkBit = p_UnkBit;

    GetCaster()->SendMessageToSet(const_cast<WorldPacket*>(l_ReShape.Write()), true);
}

void AreaTrigger::Relocate(Position const& p_Pos)
{
    Position::Relocate(p_Pos);
    UpdateNeighbours();
}

void AreaTrigger::UpdateNeighbours()
{
    if (atInfo.polygon || atInfo.hasAreaTriggerBox)
        return;

    if (!atInfo.isCheckingNeighbours)
        return;

    std::set<uint64> m_NeighboursCopy = m_Neighbours;
    for (uint64 l_NeighbourGUID : m_NeighboursCopy)
    {
        if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*this, l_NeighbourGUID))
        {
            if (l_AreaTrigger->GetExactDist2d(this) < (l_AreaTrigger->GetRadius() + GetRadius()) * 2.0f)
                continue;

            l_AreaTrigger->RemoveNeighbour(GetGUID());
        }

        m_Neighbours.erase(l_NeighbourGUID);
    }

    std::list<AreaTrigger*> l_AreaTriggers;
    GetOverlappingAreatriggers(l_AreaTriggers);

    for (AreaTrigger* l_Areatrigger : l_AreaTriggers)
    {
        if (m_Neighbours.find(l_Areatrigger->GetGUID()) != m_Neighbours.end())
            continue;

        m_Neighbours.insert(l_Areatrigger->GetGUID());
        l_Areatrigger->AddNeighbour(this->GetGUID());
    }
}

void AreaTrigger::AddNeighbour(uint64 p_AreaTriggerGUID)
{
    m_Neighbours.insert(p_AreaTriggerGUID);
}

void AreaTrigger::RemoveNeighbour(uint64 p_AreaTriggerGUID)
{
    m_Neighbours.erase(p_AreaTriggerGUID);
}

void AreaTrigger::RemoveFromNeighbours()
{
    for (uint64 l_NeighbourGUID : m_Neighbours)
    {
        if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*this, l_NeighbourGUID))
            l_AreaTrigger->RemoveNeighbour(GetGUID());
    }
}

void AreaTrigger::GetOverlappingAreatriggers(std::list<AreaTrigger*>& p_AreaTriggers)
{
    float l_Radius = std::max(atInfo.Radius, atInfo.RadiusTarget) * 3;

    CellCoord l_Coords(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell l_Cell(l_Coords);
    l_Cell.SetNoCreate();

    JadeCore::IdenticOverlappingAreatrigger l_Check(this);
    JadeCore::AreaTriggerListSearcher<decltype(l_Check)> l_Searcher(this, p_AreaTriggers, l_Check);
    TypeContainerVisitor<decltype(l_Searcher), GridTypeMapContainer> l_GridVisitor(l_Searcher);
    TypeContainerVisitor<decltype(l_Searcher), WorldTypeMapContainer> l_WorldVisitor(l_Searcher);

    l_Cell.Visit(l_Coords, l_GridVisitor, *GetMap(), *this, l_Radius);
    l_Cell.Visit(l_Coords, l_WorldVisitor, *GetMap(), *this, l_Radius);
}

void AreaTrigger::UpdateSpeedTriggerCircle(uint32 p_TimeToTargetOrSpeed, bool p_IsSpeed)
{
    Unit* l_Caster = GetCaster();
    if (!l_Caster)
        return;

    _liveTime = _CircleData.dword30;

    if (p_IsSpeed)
    {
        float l_Distance = _CircleData.Radius * 2.0f * M_PI;
        _CircleData.TimeToTarget = l_Distance / p_TimeToTargetOrSpeed * 1000.0f;
        m_speedCircle = float(p_TimeToTargetOrSpeed);
    }
    else
        _CircleData.TimeToTarget = p_TimeToTargetOrSpeed;

    WorldPackets::Spells::AreaTriggerRePath l_RePath;
    l_RePath.TriggerGUID = GetGUID();
    l_RePath.CircleData.emplace();
    l_RePath.CircleData  = _CircleData;

    l_Caster->SendMessageToSet(const_cast<WorldPacket*>(l_RePath.Write()), true);

    _moveSpeed = (p_IsSpeed ? p_TimeToTargetOrSpeed / _CircleData.Radius : atInfo.speed / _CircleData.Radius);
}

void AreaTrigger::AI_Initialize()
{
    AI_Destroy();
    AreaTriggerAI* ai = sScriptMgr->GetAreaTriggerAI(this);
    if (!ai)
        ai = new NullAreaTriggerAI(this);

    _ai.reset(ai);
    _ai->OnInitialize();
}

void AreaTrigger::AI_Destroy()
{
    _ai.reset();
}

AreaTriggerAI* AreaTrigger::AI()
{
    return _ai.get();
}