////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ragefire_chasm.h"

#define MAX_ENCOUNTER 4

class instance_ragefire_chasm : public InstanceMapScript
{
public:
    instance_ragefire_chasm() : InstanceMapScript("instance_ragefire_chasm", 389) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_ragefire_chasm_InstanceMapScript(map);
    }

    struct instance_ragefire_chasm_InstanceMapScript : public InstanceScript
    {
        instance_ragefire_chasm_InstanceMapScript(Map* map) : InstanceScript(map) {}

        void Initialize() override
        {
            SetBossNumber(MAX_ENCOUNTER);

            teamInInstance = 0;
        }

        void BeforePlayerEnter(Player* player) override
        {
            if (!teamInInstance)
                teamInInstance = player->GetTeam();
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_COMMANDER_BAGRAN:
                case NPC_INVOKER_XORENTH:
                case NPC_KORKRON_SCOUT:
                case NPC_KORKRON_ELITE:
                    if (teamInInstance == ALLIANCE)
                    {
                        // hide creature
                        creature->SetPhaseMask(2, true);
                    }
                    break;
            }

        }

        bool SetBossState(uint32 id, EncounterState state) override
        {
            return InstanceScript::SetBossState(id, state);
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            
        }

        uint64 GetData64(uint32 data) override
        {
            return 0;
        }

    private:
        uint32 teamInInstance;

    };
};

void AddSC_instance_ragefire_chasm()
{
    new instance_ragefire_chasm();
}
