////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "the_stockades.h"

class instance_the_stockades : public InstanceMapScript
{
    public:
        instance_the_stockades() : InstanceMapScript("instance_the_stockades", 34) { }

        struct instance_the_stockades_InstanceMapScript : public InstanceScript
        {
            instance_the_stockades_InstanceMapScript(InstanceMap* p_Map) : InstanceScript(p_Map) { Initialize(); }

            uint64 m_HoggerGuid;
            uint64 m_LordOverheatedGuid;
            uint64 m_RandolphMolochGuid;

            void Initialize() override
            {
                SetBossNumber(3);

                /// Bosses
                m_HoggerGuid                = 0;
                m_LordOverheatedGuid        = 0;
                m_RandolphMolochGuid        = 0;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eTheStockadesCreatures::Creature_Hogger:
                        m_HoggerGuid = p_Creature->GetGUID();
                        break;
                    case eTheStockadesCreatures::Creature_Lord_Overheated:
                        m_LordOverheatedGuid = p_Creature->GetGUID();
                        break;
                    case eTheStockadesCreatures::Creature_Randolph_Moloch:
                        m_RandolphMolochGuid = p_Creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    /// Bosses
                    case eTheStockadesData::Data_Hogger:
                        return m_HoggerGuid;
                        break;
                    case eTheStockadesData::Data_Lord_Overheat:
                        return m_LordOverheatedGuid;
                        break;
                    case eTheStockadesData::Data_Randolph_Moloch:
                        return m_RandolphMolochGuid;
                        break;
                    default:
                        break;
                }
                return 0;
            }

            void Update(uint32 p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap * p_Map) const override
        {
            return new instance_the_stockades_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_the_stockades()
{
    new instance_the_stockades();
}
#endif