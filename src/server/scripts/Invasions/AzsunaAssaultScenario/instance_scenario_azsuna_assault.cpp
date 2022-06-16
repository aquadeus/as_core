////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "InstanceScript.h"
#include "ScenarioMgr.h"
#include "instance_scenario_azsuna_assault.h"

class instance_scenario_azsuna_assault : public InstanceMapScript
{
public:
    instance_scenario_azsuna_assault() : InstanceMapScript("instance_scenario_azsuna_assault", 1705) { }

    struct instance_scenario_azsuna_assault_InstanceMapScript : public InstanceScript
    {
        instance_scenario_azsuna_assault_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
        }

        ObjectGuid m_DemonShipBomberGuid;
        ObjectGuid m_FelweaverAxtrisGuid;
        ObjectGuid m_XeritasGuid;
        std::vector<ObjectGuid> m_ThreePieceEjectorFirstGuids;
        std::vector<ObjectGuid> m_ThreePieceEjectorSecondGuids;
        std::vector<ObjectGuid> m_TwoPieceEjectorGuids;
        std::vector<ObjectGuid> m_OnePieceEjectorGuids;
        bool m_BomberActivated = false;

        Position m_GameObjectCenter = { 1461.410767f, 6131.452637f, 478.034576f };
        float m_GameObjectRadius = 70.0f;

        void OnPlayerEnter(Player* p_Player) override
        {
            Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());
            if (!l_Scenario)
                return;

            p_Player->SetPhaseMask(1, true);

            if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_AssistTheBlueDragons)
                p_Player->CastSpell(p_Player, eSpells::FarondisAssistBlueDrakesConversation, true);
        }

        void OnCreatureCreateForScript(Creature* p_Creature) override
        {
            switch (p_Creature->GetEntry())
            {
            case eCreatures::Npc_DemonShipBomber:
                m_DemonShipBomberGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_FelweaverAxtris:
                m_FelweaverAxtrisGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_Xeritas:
                m_XeritasGuid = p_Creature->GetGUID();
                break;
            default:
                break;
            }
        }

        void onScenarionNextStep(uint32 p_NewStep) override
        {
            switch (p_NewStep)
            {
            case eScenarioSteps::Step_MeetUpWithTheBlueDrakes:
            {
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_AzureWarDrake, Positions::AzureWarDrakeLandFirst))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Land, Waypoints::Path_AzureWarDrakeToLandFirst, 3);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_AzureWarDrake, Positions::AzureWarDrakeLandSecond))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Land, Waypoints::Path_AzureWarDrakeToLandSecond, 3);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_AzureWarDrake, Positions::AzureWarDrakeLandThird))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Land, Waypoints::Path_AzureWarDrakeToLandThird, 3);
                break;
            }
            case eScenarioSteps::Step_ReachTheLegionShip:
            {
                for (auto itr : Positions::ThreePieceEjectorsFirst)
                {
                    if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelFireEjector, itr))
                        m_ThreePieceEjectorFirstGuids.push_back(l_Creature->GetGUID());
                }

                for (auto itr : Positions::ThreePieceEjectorsSecond)
                {
                    if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelFireEjector, itr))
                        m_ThreePieceEjectorSecondGuids.push_back(l_Creature->GetGUID());
                }

                for (auto itr : Positions::TwoPieceEjectors)
                {
                    if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelFireEjector, itr))
                        m_TwoPieceEjectorGuids.push_back(l_Creature->GetGUID());
                }

                for (auto itr : Positions::OnePieceEjectors)
                {
                    if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelFireEjector, itr))
                        m_OnePieceEjectorGuids.push_back(l_Creature->GetGUID());
                }

                if (Creature* l_Creature = instance->GetCreature(m_FelweaverAxtrisGuid))
                    l_Creature->AI()->SetData(0, 0);

                if (Creature* l_Creature = instance->GetCreature(m_DemonShipBomberGuid))
                    l_Creature->AI()->SetData(1, 1);
                break;
            }
            case eScenarioSteps::Step_GetOnUpThere:
                DoCastSpellOnPlayers(eSpells::XeritasOrdersFelweaverConversation);
                break;
            case eScenarioSteps::Step_TakeDownTheLeader:
            {
                if (Creature* l_Creature = instance->GetCreature(m_XeritasGuid))
                {
                    l_Creature->RemoveAllAuras();
                    l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                break;
            }
            case eScenarioSteps::Step_Escape:
            {
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_AzureWarDrakeShip, Positions::AzureWarDrakeShipFirst))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Escape, Waypoints::Path_AzureWarDrakeEscapeFirst, 5);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_AzureWarDrakeShip, Positions::AzureWarDrakeShipSecond))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Escape, Waypoints::Path_AzureWarDrakeEscapeSecond, 5);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_AzureWarDrakeShip, Positions::AzureWarDrakeShipThird))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Escape, Waypoints::Path_AzureWarDrakeEscapeThird, 5);
                break;
            }
            default:
                break;
            }
        }

        void SetData(uint32 p_Type, uint32 /*p_Data*/) override
        {
            switch (p_Type)
            {
            case eData::Data_First_Ejector:
            {
                for (auto itr : m_ThreePieceEjectorFirstGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                        l_Creature->CastSpell(l_Creature, eSpells::FelFireEjection, false);
                }
                break;
            }
            case eData::Data_Second_Ejector:
            {
                for (auto itr : m_ThreePieceEjectorSecondGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                        l_Creature->CastSpell(l_Creature, eSpells::FelFireEjection, false);
                }
                break;
            }
            case eData::Data_Third_Ejector:
            {
                for (auto itr : m_TwoPieceEjectorGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                        l_Creature->CastSpell(l_Creature, eSpells::FelFireEjection, false);
                }
                break;
            }
            case eData::Data_Fourth_Ejector:
            {
                for (auto itr : m_OnePieceEjectorGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                        l_Creature->CastSpell(l_Creature, eSpells::FelFireEjection, false);
                }
                break;
            }
            case eData::Data_LeftSide_FirstPath:
            {
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelImp, Positions::ImpSpawnPos))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Imp_Path, Waypoints::Path_ImpLeftFirst, 19, false);
                break;
            }
            case eData::Data_LeftSide_SecondPath:
            {
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelImp, Positions::ImpSpawnPos))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Imp_Path, Waypoints::Path_ImpLeftSecond, 16, false);
                break;
            }
            case eData::Data_RightSide_FirstPath:
            {
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelImp, Positions::ImpSpawnPos))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Imp_Path, Waypoints::Path_ImpRightFirst, 17, false);
                break;
            }
            case eData::Data_RightSide_SecondPath:
            {
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_FelImp, Positions::ImpSpawnPos))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Imp_Path, Waypoints::Path_ImpRightSecond, 13, false);
                break;
            }
            default:
                break;
            }
        }

        void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
        {
            Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());
            Player* l_Player = p_Killer->ToPlayer();
            if (!l_Scenario || !l_Player || l_Scenario->GetCurrentStep() != eScenarioSteps::Step_AssistTheBlueDragons)
                return;

            if (!m_BomberActivated)
            {
                if (Creature* l_Creature = instance->GetCreature(m_DemonShipBomberGuid))
                {
                    l_Creature->AI()->SetData(0, 0);
                    m_BomberActivated = true;
                }
            }

            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_AssistTheBlueDragons_Demons, 0, 0, l_Player, l_Player);
        }

        void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
        {
            WorldSafeLocsEntry const* l_Loc = nullptr;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());
            if (!l_Scenario)
                return;

            switch (l_Scenario->GetCurrentStep())
            {
            case eScenarioSteps::Step_AssistTheBlueDragons:
                l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Farondis);
                break;
            case eScenarioSteps::Step_MeetUpWithTheBlueDrakes:
                l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Farondis);
                break;
            default:
                l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Ship);
            }

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
            return (p_Position.GetExactDist2d(&m_GameObjectCenter) < m_GameObjectRadius);
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_scenario_azsuna_assault_InstanceMapScript(map);
    }
};

void AddSC_instance_scenario_azsuna_assault()
{
    new instance_scenario_azsuna_assault();
}