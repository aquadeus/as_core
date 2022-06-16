////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Instance_Blackwing_Lair
SD%Complete: 0
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

#include "blackwing_lair.h"

class instance_blackwing_lair : public InstanceMapScript
{
    public:
        instance_blackwing_lair() : InstanceMapScript("instance_blackwing_lair", 469) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_blackwing_lair_InstanceScript(map);
        }

        struct instance_blackwing_lair_InstanceScript : public InstanceScript
        {
            instance_blackwing_lair_InstanceScript(Map* map): InstanceScript(map) { }

            void Initialize()
            {
                SetBossNumber(DATA_MAX_ENCOUNTER);
            }

            bool SetBossState(uint32 id, EncounterState state)
            {
                if (!InstanceScript::SetBossState(id, state))
                    return false;

                return true;
            }
        };
};

void AddSC_instance_throne_of_thunder()
{
    new instance_blackwing_lair();
}
