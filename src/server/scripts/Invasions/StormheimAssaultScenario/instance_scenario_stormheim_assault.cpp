////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "InstanceScript.h"
#include "ScenarioMgr.h"
#include "instance_scenario_stormheim_assault.h"

class instance_scenario_stormheim_assault : public InstanceMapScript
{
public:
    instance_scenario_stormheim_assault() : InstanceMapScript("instance_scenario_stormheim_assault", 1707) { }

    struct instance_scenario_stormheim_assault_InstanceMapScript : public InstanceScript
    {
        instance_scenario_stormheim_assault_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
        }

        ObjectGuid m_AleifirGuid;
        ObjectGuid m_ErilarGuid;
        ObjectGuid m_HrafsirGuid;
        const Position m_GameObjectCenter = { 2865.02f, 1328.488f, 255.899f };
        float m_GameObjectRadius = 80.0f;

        void OnPlayerEnter(Player* p_Player) override
        {
            if (getScenarionStep() == eScenarioSteps::Step_Freedom)
            {
                p_Player->KilledMonsterCredit(118779);
                p_Player->CastSpell(p_Player, eSpells::Spell_OdynFreeDrakesConversation, true);
            }
        }

        void OnCreatureCreateForScript(Creature* p_Creature) override
        {
            switch (p_Creature->GetEntry())
            {
            case eCreatures::Npc_Aleifir_Cycle:
                m_AleifirGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_Erilar_Cycle:
                m_ErilarGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_Hrafsir_Cycle:
                m_HrafsirGuid = p_Creature->GetGUID();
                break;
            default:
                break;
            }
        }

        void onScenarionNextStep(uint32 newStep) override
        {
            switch (newStep)
            {
            case eScenarioSteps::Step_SiegeBreaker:
            {
                instance->SummonCreature(eCreatures::Npc_Aleifir_Freed, Positions::AleifirStartPos);
                instance->SummonCreature(eCreatures::Npc_Erilar_Freed, Positions::ErilarStartPos);
                instance->SummonCreature(eCreatures::Npc_Hrafsir_Freed, Positions::HrafsirStartPos);
                break;
            }
            case eScenarioSteps::Step_Insertion:
            {
                if (Creature* l_Aleifir = instance->GetCreature(m_AleifirGuid))
                    l_Aleifir->AI()->DoAction(Action_Path_To_Ship);
                if (Creature* l_Erilar = instance->GetCreature(m_ErilarGuid))
                    l_Erilar->AI()->DoAction(Action_Path_To_Ship);
                if (Creature* l_Hrafsir = instance->GetCreature(m_HrafsirGuid))
                    l_Hrafsir->AI()->DoAction(Action_Path_To_Ship);
                break;
            }
            case eScenarioSteps::Step_CommanderTakedown:
            {
                instance->SummonCreature(eCreatures::Npc_FelCannonPortal, Positions::FelCannonPortalFirst);
                instance->SummonCreature(eCreatures::Npc_FelCannonPortal, Positions::FelCannonPortalSecond);
                DoCastSpellOnPlayers(eSpells::Spell_AlexiusSpawnConversation);
                break;
            }
            case eScenarioSteps::Step_GetOutOfThere:
            {
                instance->SummonCreature(eCreatures::Npc_Aleifir_GetOut, Positions::AleifirGetOutPos);
                instance->SummonCreature(eCreatures::Npc_Erilar_GetOut, Positions::ErilarGetOutPos);
                instance->SummonCreature(eCreatures::Npc_Hrafsir_GetOut, Positions::HrafsirGetOutPos);
                break;
            }
            default:
                break;
            }
        }

        void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
        {
            Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());
            if (!l_Scenario)
                return;

            switch (l_Scenario->GetCurrentStep())
            {
            case eScenarioSteps::Step_SiegeBreaker:
            {
                if (Unit* l_Owner = p_Killer->GetOwner())
                    if (Player* l_Player = l_Owner->ToPlayer())
                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_SiegeBreaker, 0, 0, l_Player, l_Player);
                p_Creature->Respawn(true, true, 30000);
                break;
            }
            case eScenarioSteps::Step_TakenDownFromWithin:
            {
                if (Player* l_Player = p_Killer->ToPlayer())
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_TakenDownFromWithinFirst, 0, 0, l_Player, l_Player);
                break;
            }
            default:
                break;
            }
        }

        void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
        {
            WorldSafeLocsEntry const* l_Loc = nullptr;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());
            if (!l_Scenario)
                return;

            if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_Freedom || l_Scenario->GetCurrentStep() == eScenarioSteps::Step_SiegeBreaker)
                l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Ground);
            else
                l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Ship);

            if (l_Loc == nullptr)
                return;

            p_X = l_Loc->x;
            p_Y = l_Loc->y;
            p_Z = l_Loc->z;
            p_O = l_Loc->o;
            p_MapID = l_Loc->MapID;
        }

        bool IsInNoPathArea(Position const& p_Position) const override
        {
            return (p_Position.GetExactDist2d(&m_GameObjectCenter) < m_GameObjectRadius) && (p_Position.GetPositionZ() > m_GameObjectCenter.GetPositionZ()) && (p_Position.GetPositionZ() < (m_GameObjectCenter.GetPositionZ() + 150));
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_scenario_stormheim_assault_InstanceMapScript(map);
    }
};

void AddSC_instance_scenario_stormheim_assault()
{
    new instance_scenario_stormheim_assault();
}
