////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef EVENTOBJECTOBJECT_HPP
# define EVENTOBJECTOBJECT_HPP

# include "Object.h"

struct EventObjectData
{
    explicit EventObjectData() : m_DBData(true) { }
    uint32 Guid = 0;
    uint32 ID;                                              ///< Entry in eventobject_template
    uint16 MapID;
    uint16 ZoneID = 0;
    uint16 AreaID = 0;
    uint32 PhaseMask = 1;
    float PosX;
    float PosY;
    float PosZ;
    float Orientation;
    uint32 SpawnMask = 1;
    bool m_DBData;
    uint32 PhaseID;
};

struct EventObjectTemplate
{
    uint32 Entry;
    std::string Name;
    float Radius = 0.0f;
    uint32 SpellID = 0;
    uint32 WorldSafeLocID = 0;
    uint32 ScriptID = 0;
};

typedef std::unordered_map<uint32, EventObjectTemplate> EventObjectTemplateContainer;

class EventObject : public WorldObject, public GridObject<EventObject>
{
    public:
        EventObject();
        ~EventObject();

        bool Create(uint32 p_Guid, Map* p_Map, uint32 p_PhaseMask, uint32 p_Entry, float p_X, float p_Y, float p_Z, float p_Angle, float p_Radius, uint32 p_SpellID, uint32 p_WorldSafe);

        void AddToWorld() override;
        void RemoveFromWorld() override;

        bool LoadFromDB(uint32 p_Guid, Map* p_Map) { return LoadEventObjectFromDB(p_Guid, p_Map); }
        bool LoadEventObjectFromDB(uint32 p_Guid, Map* p_Map);
        void SaveToDB(uint32 p_MapID, uint32 p_SpawnMask, uint32 p_PhaseMask);

        void Update(uint32 p_Time) override;
        void Remove();

        void MoveInLineOfSight(Unit* p_Who);

        uint64 GetDBTableGUIDLow() const { return m_DBTableGuid; }

        uint32 GetScriptId() const;

        EventObjectTemplate const* m_EventTemplate = nullptr;
        uint64 m_DBTableGuid;
};

#endif ///< EVENTOBJECTOBJECT_HPP
