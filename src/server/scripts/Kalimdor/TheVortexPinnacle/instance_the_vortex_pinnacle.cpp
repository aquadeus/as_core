////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "the_vortex_pinnacle.h"

#define MAX_ENCOUNTER 3

class instance_the_vortex_pinnacle : public InstanceMapScript
{
    public:
        instance_the_vortex_pinnacle() : InstanceMapScript("instance_the_vortex_pinnacle", 657) { }
        
        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_the_vortex_pinnacle_InstanceMapScript(map);
        }

        struct instance_the_vortex_pinnacle_InstanceMapScript: public InstanceScript
        {
            instance_the_vortex_pinnacle_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }

            uint64 uiGrandVizierErtanGUID;
            uint64 uiAltairusGUID;
            uint64 uiAsaadGUID;

            void Initialize() override
            {
                SetBossNumber(MAX_ENCOUNTER);
                uiGrandVizierErtanGUID = 0;
                uiAltairusGUID = 0;
                uiAsaadGUID = 0;
            }

            void OnCreatureCreate(Creature* pCreature) override
            {
                switch(pCreature->GetEntry())
                {
                    case NPC_GRAND_VIZIER_ERTAN:
                        uiGrandVizierErtanGUID = pCreature->GetGUID();
                        break;
                    case NPC_ALTAIRUS:
                        uiAltairusGUID = pCreature->GetGUID();
                        break;
                    case NPC_ASAAD:
                        uiAsaadGUID = pCreature->GetGUID();
                        break;
                }
                
            }

            uint64 GetData64(uint32 identifier) override
            {
                switch(identifier)
                {
                    case DATA_ERTAN:
                        return uiGrandVizierErtanGUID;
                    case DATA_ALTAIRUS:
                        return uiAltairusGUID;
                    case DATA_ASAAD:
                        return uiAsaadGUID;
                }
                return 0;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;
                std::ostringstream saveStream;
                saveStream << "V P" << GetBossSaveData();
                str_data = saveStream.str();

                OUT_SAVE_INST_DATA_COMPLETE;
                return str_data;
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'V' && dataHead2 == 'P')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            bool SetBossState(uint32 id, EncounterState state) override
            {
                return InstanceScript::SetBossState(id, state);
            }
        };
};

#ifndef __clang_analyzer__
void AddSC_instance_the_vortex_pinnacle()
{
    new instance_the_vortex_pinnacle();
}
#endif
