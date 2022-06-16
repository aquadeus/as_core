////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "World.h"
#include "ObjectAccessor.h"
#include "EventObject.hpp"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"

EventObject::EventObject() : WorldObject(false)
{
    m_objectType |= TYPEMASK_EVENTOBJECT;
    m_objectTypeId = TYPEID_EVENTOBJECT;

    m_updateFlag = UPDATEFLAG_NONE;

    m_valuesCount = OBJECT_END;
    _dynamicValuesCount = OBJECT_DYNAMIC_END;
}

EventObject::~EventObject()
{
}

void EventObject::AddToWorld()
{
    ///- Register the EventObject for guid lookup and for caster
    if (!IsInWorld())
    {
        sObjectAccessor->AddObject(this);
        WorldObject::AddToWorld();
    }
}

void EventObject::RemoveFromWorld()
{
    ///- Remove the EventObject from the accessor and from all lists of objects in world
    if (IsInWorld())
    {
        if (!IsInWorld())
            return;

        WorldObject::RemoveFromWorld();
        sObjectAccessor->RemoveObject(this);
    }
}

void EventObject::Update(uint32 /*p_Time*/)
{
}

void EventObject::MoveInLineOfSight(Unit* p_Who)
{
    if (!p_Who || !m_EventTemplate || !p_Who->IsPlayer()) ///< Now check only for player
        return;

    if (m_EventTemplate->Radius <= 0.0f)
        return;

    if (Player* l_Player = p_Who->ToPlayer())
    {
        if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
        {
            if (l_Instance->IsEncounterInProgress())
                return;
        }

        if (p_Who->GetDistance(this) <= m_EventTemplate->Radius)
        {
            if (m_EventTemplate->SpellID)
                p_Who->CastSpell(p_Who, m_EventTemplate->SpellID, true);

            if (m_EventTemplate->WorldSafeLocID)
            {
                if (WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(m_EventTemplate->WorldSafeLocID))
                    l_Player->TeleportTo(l_Loc->MapID, l_Loc->x, l_Loc->y, l_Loc->z, l_Loc->o, TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT);
            }

            sScriptMgr->OnEventObject(l_Player, this);
        }
    }
}

void EventObject::Remove()
{
    if (IsInWorld())
    {
        RemoveFromWorld();
        AddObjectToRemoveList();
    }
}

bool EventObject::LoadEventObjectFromDB(uint32 p_Guid, Map* p_Map)
{
    EventObjectData const* l_Data = sObjectMgr->GetEventObjectData(p_Guid);
    if (!l_Data)
    {
        sLog->outError(LOG_FILTER_SQL, "EventObject (GUID: %u) not found in table `eventobject`, can't load. ", p_Guid);
        return false;
    }

    m_DBTableGuid = p_Guid;

    if (p_Map->GetInstanceId() != 0)
        p_Guid = sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_EVENTOBJECT);

    SetMap(p_Map);
    Relocate(l_Data->PosX, l_Data->PosY, l_Data->PosZ, l_Data->Orientation);
    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_GENERAL, "EventObject (EventObject %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)", l_Data->ID, GetPositionX(), GetPositionY());
        return false;
    }

    m_EventTemplate = sObjectMgr->GetEventObjectTemplate(l_Data->ID);

    Object::_Create(p_Guid, l_Data->ID, HighGuid::HIGHGUID_EVENTOBJECT);
    SetPhaseMask(l_Data->PhaseMask, false);

    if (l_Data->PhaseID)
        SetInPhase(l_Data->PhaseID, true, true);

    SetEntry(l_Data->ID);
    SetObjectScale(1.0f);

    return true;
}

bool EventObject::Create(uint32 p_GuidLow, Map* p_Map, uint32 p_PhaseMask, uint32 p_Entry, float p_X, float p_Y, float p_Z, float p_Angle, float p_Radius, uint32 p_Spell, uint32 p_WorldSafe)
{
    ASSERT(p_Map);
    SetMap(p_Map);
    SetPhaseMask(p_PhaseMask, false);

    m_EventTemplate = sObjectMgr->GetEventObjectTemplate(p_Entry);

    if (!m_EventTemplate)
        m_EventTemplate = sObjectMgr->AddEventObjectTemplate(p_Entry, p_Radius, p_Spell, p_WorldSafe);

    Relocate(p_X, p_Y, p_Z, p_Angle);

    Object::_Create(p_GuidLow, p_Entry, HighGuid::HIGHGUID_EVENTOBJECT);

    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_UNITS, "EventObject::Create(): given coordinates for eventobject (guidlow %d, entry %d) are not valid (X: %f, Y: %f, Z: %f, O: %f)", p_GuidLow, p_Entry, p_X, p_Y, p_Z, p_Angle);
        return false;
    }

    SetEntry(p_Entry);
    return true;
}

void EventObject::SaveToDB(uint32 p_MapID, uint32 p_SpawnMask, uint32 p_PhaseMask)
{
    /// Update in loaded data
    if (!m_DBTableGuid)
        m_DBTableGuid = GetGUIDLow();

    EventObjectData& l_Data = sObjectMgr->NewOrExistEventObjectData(m_DBTableGuid);

    uint32 l_ZoneID = 0;
    uint32 l_AreaID = 0;
    sMapMgr->GetZoneAndAreaId(l_ZoneID, l_AreaID, p_MapID, GetPositionX(), GetPositionY(), GetPositionZ());

    l_Data.ID = GetEntry();
    l_Data.MapID = p_MapID;
    l_Data.ZoneID = l_ZoneID;
    l_Data.AreaID = l_AreaID;
    l_Data.PhaseMask = p_PhaseMask;
    l_Data.SpawnMask = p_SpawnMask;
    l_Data.PosX = GetPositionX();
    l_Data.PosY = GetPositionY();
    l_Data.PosZ = GetPositionZH();
    l_Data.Orientation = GetOrientation();

    /// Update in DB
    SQLTransaction l_Transaction = WorldDatabase.BeginTransaction();

    PreparedStatement* l_Statement = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENTOBJECT);
    l_Statement->setUInt64(0, m_DBTableGuid);
    l_Transaction->Append(l_Statement);

    uint8 l_Index = 0;

    l_Statement = WorldDatabase.GetPreparedStatement(WORLD_INS_EVENTOBJECT);
    l_Statement->setUInt64(l_Index++, m_DBTableGuid);
    l_Statement->setUInt32(l_Index++, GetEntry());
    l_Statement->setUInt16(l_Index++, uint16(p_MapID));
    l_Statement->setUInt32(l_Index++, l_ZoneID);
    l_Statement->setUInt32(l_Index++, l_AreaID);
    l_Statement->setUInt8(l_Index++,  p_SpawnMask);
    l_Statement->setUInt16(l_Index++, uint16(GetPhaseMask()));
    l_Statement->setFloat(l_Index++,  GetPositionX());
    l_Statement->setFloat(l_Index++,  GetPositionY());
    l_Statement->setFloat(l_Index++,  GetPositionZH());
    l_Statement->setFloat(l_Index++,  GetOrientation());
    l_Transaction->Append(l_Statement);

    WorldDatabase.CommitTransaction(l_Transaction);
}

uint32 EventObject::GetScriptId() const
{
    if (m_EventTemplate)
        return m_EventTemplate->ScriptID;
    else
        return 0;
}
