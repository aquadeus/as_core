////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "Conversation.hpp"
#include "InstanceScript.h"
#include "UpdateFieldFlags.h"
#include "Group.h"

Conversation::Conversation() :
    WorldObject(false), m_CasterUnit(nullptr), m_SpellID(0), m_Duration(0), m_CasterGUID(0)
{
    m_objectType        |= TypeMask::TYPEMASK_CONVERSATION;
    m_objectTypeId      = TypeID::TYPEID_CONVERSATION;
    m_updateFlag        = OBJECT_UPDATE_FLAGS::UPDATEFLAG_HAS_POSITION;
    m_valuesCount       = EConversationFields::CONVERSATION_END;
    _dynamicValuesCount = EConversationDynamicFields::CONVERSATION_DYNAMIC_END;

    _fieldNotifyFlags = UpdatefieldFlags::UF_FLAG_PUBLIC | UpdatefieldFlags::UF_FLAG_VIEWER_DEPENDENT | UpdatefieldFlags::UF_FLAG_0x100;
}

Conversation::~Conversation()
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void Conversation::AddToWorld()
{
    /// Register the Conversation for guid lookup
    if (!IsInWorld())
    {
        sObjectAccessor->AddObject(this);
        WorldObject::AddToWorld();
        BindToCaster();
    }
}

void Conversation::RemoveFromWorld()
{
    /// Remove the Conversation from the accessors and from all lists of objects in world
    if (IsInWorld())
    {
        /// dynobj could get removed in Aura::RemoveAura
        if (!IsInWorld())
            return;

        UnbindFromCaster();
        WorldObject::RemoveFromWorld();
        sObjectAccessor->RemoveObject(this);
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool Conversation::CanNeverSee2(WorldObject const* p_Seer) const
{
    if (p_Seer->GetTypeId() != TYPEID_PLAYER)
        return true;

    auto l_Data = m_Playing.find(p_Seer->GetGUID());
    if (l_Data == m_Playing.end())
    {
        const_cast<Conversation*>(this)->m_Playing[p_Seer->GetGUID()] = getMSTime();
        return CanNeverSee2(p_Seer);
    }

    const uint32 l_Duration = getMSTime() - l_Data->second;

    if (l_Duration > GetUInt32Value(CONVERSATION_FIELD_FIELD_LAST_LINE_DURATION))
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool Conversation::LoadFromDB(uint32 p_Guid, Map* p_Map)
{
    return LoadConversationFromDB(p_Guid, p_Map, false);
}

bool Conversation::LoadConversationFromDB(uint32 p_Guid, Map* p_Map, bool p_AddToMap)
{
    ConversationSpawnData const* l_Data = sObjectMgr->GetConversationSpawnData(p_Guid);
    if (!l_Data)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (GUID: %u) not found in table `creature`, can't load. ", p_Guid);
        return false;
    }

    if (p_Map->GetInstanceId() == 0)
    {
        /*if (map->GetConversation(ObjectGuid::Create<HighGuid::Conversation>(data->mapid, data->id, guid)))
        return false;*/
    }
    else
        p_Guid = sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION);

    std::vector<ConversationActor>      const* l_ConversationActors      = sObjectMgr->GetConversationActor(l_Data->ConversationEntry);
    std::vector<ConversationCreature>   const* l_ConversationCreatures   = sObjectMgr->GetConversationCreature(l_Data->ConversationEntry);
    std::vector<ConversationData>       const* l_ConversationDatas       = sObjectMgr->GetConversationData(l_Data->ConversationEntry);

    bool l_HasActorData     = l_ConversationActors      && !l_ConversationActors->empty();
    bool l_HasCreatureData  = l_ConversationCreatures   && !l_ConversationCreatures->empty();
    bool l_HasData          = l_ConversationDatas       && !l_ConversationDatas->empty();

    if (!l_HasData || (!l_HasActorData && !l_HasCreatureData))
        return false;

    SetMap(p_Map);
    Relocate(l_Data->PositionX, l_Data->PositionY, l_Data->PositionZ, l_Data->Orientation);
    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_GENERAL, "Conversation (conversation %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)", l_Data->ConversationEntry, GetPositionX(), GetPositionY());
        return false;
    }

    uint32 l_Duration = 30000;

    WorldObject::_Create(p_Guid, HighGuid::HIGHGUID_CONVERSATION, l_Data->PhaseMask);
    SetPhaseMask(l_Data->PhaseMask, false);

    SetEntry(l_Data->ConversationEntry);
    SetObjectScale(1.0f);
    SetUInt32Value(CONVERSATION_FIELD_FIELD_LAST_LINE_DURATION, l_Duration);

    if (l_HasActorData)
    {
        for (ConversationActor const& l_Current : *l_ConversationActors)
        {
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.ActorID);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.CreatureID);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.DisplayID);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk1);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk2);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk3);

            if (l_Current.Duration)
                l_Duration = l_Current.Duration;
        }
    }

    if (l_HasCreatureData)
    {
        for (ConversationCreature const& l_Current : *l_ConversationCreatures)
        {
            Creature* l_Creature = nullptr;

            if (l_Current.CreatureID)
            {
                l_Creature = FindNearestCreature(l_Current.CreatureID, GetVisibilityRange());

                if (!l_Creature)
                {
                    sLog->outError(LOG_FILTER_GENERAL, "Conversation (conversation %u) not created. Can't find creature %u", l_Data->ConversationEntry, l_Current.CreatureID);
                    return false;

                }
            }
            else
                return false;

            AddDynamicGuidValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Creature->GetGUID());
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk1);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk2);

            if (l_Current.Duration)
                l_Duration = l_Current.Duration;
        }
    }

    for (ConversationData const& l_Current : *l_ConversationDatas)
    {
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.ID);
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.TextID);
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.Unk1);
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.Unk2);
    }

    SetUInt32Value(CONVERSATION_FIELD_FIELD_LAST_LINE_DURATION, l_Duration);
    //SetDuration(duration);
    setActive(true);

    if (p_AddToMap && !GetMap()->AddToMap(this))
        return false;

    return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool Conversation::CreateConversation(uint32 p_Guid, uint32 p_TriggerEntry, Unit* p_Caster, SpellInfo const* p_SpellInfo, Position const& p_Position)
{
    std::vector<ConversationActor>      const* l_ConversationActors     = sObjectMgr->GetConversationActor(p_TriggerEntry);
    std::vector<ConversationCreature>   const* l_ConversationCreatures  = sObjectMgr->GetConversationCreature(p_TriggerEntry);
    std::vector<ConversationData>       const* l_ConversationDatas      = sObjectMgr->GetConversationData(p_TriggerEntry);

    bool l_HasActorData     = l_ConversationActors      && !l_ConversationActors->empty();
    bool l_HasCreatureData  = l_ConversationCreatures   && !l_ConversationCreatures->empty();
    bool l_HasData          = l_ConversationDatas       && !l_ConversationDatas->empty();

    if (!l_HasData || (!l_HasActorData && !l_HasCreatureData))
        return false;

    SetMap(p_Caster->GetMap());
    Relocate(p_Position);
    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_GENERAL, "Conversation (spell %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)", p_SpellInfo ? p_SpellInfo->Id : 0, GetPositionX(), GetPositionY());
        return false;
    }

    uint32 l_Duration = 0;

    p_Guid = sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION);

    WorldObject::_Create(p_Guid, HighGuid::HIGHGUID_CONVERSATION, p_Caster->GetPhaseMask());
    SetPhaseMask(p_Caster->GetPhaseMask(), false);

    SetEntry(p_TriggerEntry);
    SetObjectScale(1.0f);
    m_CasterGUID = p_Caster->GetGUID();

    if (Player* player = p_Caster->ToPlayer())
    {
        if (GroupPtr l_Group = player->GetGroup())
        {
            l_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr)
            {
                if (Player* member = l_Itr->player)
                    AddPlayerInPersonnalVisibilityList(member->GetGUID());
            });
        }
        else
            AddPlayerInPersonnalVisibilityList(p_Caster->GetGUID());
    }

    if (l_HasActorData)
    {
        for (ConversationActor const& l_Current : *l_ConversationActors)
        {
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.ActorID);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.CreatureID);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.DisplayID);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk1);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk2);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk3);

            if (l_Current.Duration)
                l_Duration = l_Current.Duration;
        }
    }

    if (l_HasCreatureData)
    {
        for (ConversationCreature const& l_Current : *l_ConversationCreatures)
        {
            Creature* l_Creature = nullptr;

            if (l_Current.CreatureID)
            {
                l_Creature = p_Caster->FindNearestCreature(l_Current.CreatureID, GetVisibilityRange());

                if (!l_Creature)
                {
                    sLog->outError(LOG_FILTER_GENERAL, "Conversation (conversation %u) not created. Can't find creature %u", p_TriggerEntry, l_Current.CreatureID);
                    return false;

                }
            }
            else
                return false;

            AddDynamicGuidValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Creature->GetGUID());
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk1);
            AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_ACTORS, l_Current.Unk2);

            if (l_Current.Duration)
                l_Duration = l_Current.Duration;
        }
    }

    for (ConversationData const& l_Current : *l_ConversationDatas)
    {
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.ID);
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.TextID);
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.Unk1);
        AddDynamicValue(EConversationDynamicFields::CONVERSATION_DYNAMIC_FIELD_LINES, l_Current.Unk2);
    }

    if (!l_Duration)
        l_Duration = 30000;

    /// possible it should be add.
    l_Duration += 5000;

    SetUInt32Value(CONVERSATION_FIELD_FIELD_LAST_LINE_DURATION, l_Duration);
    SetDuration(l_Duration);
    setActive(true);

    if (!GetMap()->AddToMap(this))
        return false;

    return true;
}

void Conversation::Update(uint32 p_Time)
{
    bool l_IsExpired = false;

    if (GetDuration())
    {
        auto l_Data = m_Playing.find(m_CasterGUID);
        if (l_Data == m_Playing.end())
            return;

        uint32 const l_Duration = getMSTime() - l_Data->second;
        if (l_Duration > GetUInt32Value(CONVERSATION_FIELD_FIELD_LAST_LINE_DURATION))
            l_IsExpired = true;

        if (l_IsExpired)
            Remove();
    }
}

void Conversation::Remove()
{
    if (IsInWorld())
    {
        RemoveFromWorld();
        AddObjectToRemoveList();
    }
}

void Conversation::SetDuration(int32 p_NewDuration)
{
    m_Duration = p_NewDuration;
}

int32 Conversation::GetDuration() const
{
    return m_Duration;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Unit* Conversation::GetCaster() const
{
    return m_CasterUnit;
}

void Conversation::BindToCaster()
{
    if (!GetCasterGUID())
        return;

    ASSERT(!m_CasterUnit);

    m_CasterUnit = ObjectAccessor::GetUnit(*this, GetCasterGUID());

    if (!m_CasterUnit)
        return;

    ASSERT(m_CasterUnit->GetMap() == GetMap());
}

void Conversation::UnbindFromCaster()
{
    if (!GetCasterGUID())
        return;

    if (!m_CasterUnit)
        return;

    m_CasterUnit = NULL;
}

uint32 Conversation::GetSpellId() const
{
    return m_SpellID;
}

uint64 Conversation::GetCasterGUID() const
{
    return m_CasterGUID;
}
