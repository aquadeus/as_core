////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ZONE_SCRIPT_H_
#define ZONE_SCRIPT_H_

#include "Common.h"
#include "Creature.h"

class GameObject;

class ZoneScript
{
    public:
        ZoneScript() {}
        virtual ~ZoneScript() {}

        virtual uint32 GetCreatureEntry(uint32 /*guidlow*/, CreatureData const* data) { return data->id; }
        virtual uint32 GetGameObjectEntry(uint32 /*guidlow*/, uint32 entry) { return entry; }

        virtual void OnCreatureCreate(Creature* /*creature*/) {}
        virtual void OnCreatureRemove(Creature* /*creature*/) {}
        virtual void OnGameObjectCreate(GameObject* /*go*/) {}
        virtual void OnGameObjectRemove(GameObject* /*go*/) {}

        virtual void OnCreatureCreateForScript(Creature* /*creature*/) {}
        virtual void OnCreatureRemoveForScript(Creature* /*creature*/) {}
        virtual void OnCreatureUpdateDifficulty(Creature* /*creature*/) {}
        virtual void EnterCombatForScript(Creature* /*creature*/, Unit* /*enemy*/) {}
        virtual void CreatureDiesForScript(Creature* /*creature*/, Unit* /*killer*/) {}
        virtual void OnGameObjectCreateForScript(GameObject* /*go*/) {}
        virtual void OnGameObjectRemoveForScript(GameObject* /*go*/) {}

        virtual void OnUnitDeath(Unit* /*unit*/) {}

        //All-purpose data storage 64 bit
        virtual uint64 GetData64(uint32 /*DataId*/) { return 0; }
        virtual void SetData64(uint32 /*DataId*/, uint64 /*value*/) {}

        //All-purpose data storage 32 bit
        virtual uint32 GetData(uint32 /*DataId*/) { return 0; }
        virtual void SetData(uint32 /*DataId*/, uint32 /*value*/) {}

        virtual void ProcessEvent(WorldObject* /*obj*/, uint32 /*eventId*/) {}
        virtual std::string GetScriptName() { return ""; }
};

#endif
