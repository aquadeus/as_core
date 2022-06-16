////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Object.h"
#include "Timer.h"

class Unit;

struct ConversationSpawnData
{
    explicit ConversationSpawnData() : dbData(true) {}
    uint64 GUID = 0;
    uint32 ConversationEntry;                                              // entry in creature_template
    uint16 MapID;
    uint16 ZoneID = 0;
    uint16 AreaID = 0;
    uint32 PhaseMask = 1;
    float PositionX;
    float PositionY;
    float PositionZ;
    float Orientation;
    uint32 SpawnMask = 1;
    bool dbData;
    std::set<uint32> PhaseID;
};

class Conversation : public WorldObject, public GridObject<Conversation>
{
    public:
        Conversation();
        ~Conversation();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        bool CanNeverSee2(WorldObject const* p_Seer) const override;

        bool LoadFromDB(uint32 p_Guid, Map* p_Map);
        bool LoadConversationFromDB(uint32 p_Guid, Map* p_Map, bool p_AddToMap = true);

        bool CreateConversation(uint32 p_Guid, uint32 p_TriggerEntry, Unit* p_Caster, SpellInfo const* p_SpellInfo, Position const& p_Position);
        void Update(uint32 p_Time) override;
        void Remove();
        void SetDuration(int32 p_NewDuration);
        int32 GetDuration() const;

        Unit* GetCaster() const;
        void BindToCaster();
        void UnbindFromCaster();
        uint32 GetSpellId() const;
        uint64 GetCasterGUID() const;

    private:
        Unit* m_CasterUnit;
        uint32 m_SpellID;
        uint32 m_Duration;
        uint64 m_CasterGUID;

        std::map<uint64, uint32> m_Playing;
};
