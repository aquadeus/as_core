//////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "maraudon.h"

class instance_maraudon : public InstanceMapScript
{
public:
    instance_maraudon() : InstanceMapScript("instance_maraudon", 349) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_maraudon_InstanceMapScript(map);
    }

    struct instance_maraudon_InstanceMapScript : public InstanceScript
    {
        instance_maraudon_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }

        void Initialize()
        {
            SetBossNumber(DATA_MAX_ENCOUNTER);
        }

        bool SetBossState(uint32 id, EncounterState state, uint32 p_EncounterID = 0) override
        {
            return InstanceScript::SetBossState(id, state, p_EncounterID);
        }
    };
};

void AddSC_instance_maraudon()
{
    new instance_maraudon();
}
