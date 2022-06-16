////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

# include "BrokenIslesPCH.h"

class instance_scenario_havoc_artifact_felsoul_hold : public InstanceMapScript
{
public:

    instance_scenario_havoc_artifact_felsoul_hold() : InstanceMapScript("instance_scenario_havoc_artifact_felsoul_hold", 1498) { }

    InstanceScript* GetInstanceScript(InstanceMap* p_Map) const
    {
        return new instance_scenario_havoc_artifact_felsoul_hold_instancemapscript(p_Map);
    }

    struct instance_scenario_havoc_artifact_felsoul_hold_instancemapscript : public InstanceScript
    {
        instance_scenario_havoc_artifact_felsoul_hold_instancemapscript(Map* p_Map) : InstanceScript(p_Map) {}

        InstanceScript* m_Instance = this;
    };
};

#ifndef __clang_analyzer__
void AddSC_instance_scenario_havoc_artifact_felsoul_hold()
{
    new instance_scenario_havoc_artifact_felsoul_hold();
}
#endif