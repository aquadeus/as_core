////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "InstanceScript.h"
#include "ScenarioMgr.h"
#include "instance_scenario_highmountain_assault.h"

class instance_scenario_highmountain_assault : public InstanceMapScript
{
public:
    instance_scenario_highmountain_assault() : InstanceMapScript("instance_scenario_highmountain_assault", 1706) { }

    struct instance_scenario_highmountain_assault_InstanceMapScript : public InstanceScript
    {
        instance_scenario_highmountain_assault_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
        }

        std::vector<ObjectGuid> m_HighmountainProtectorGuids;
        std::vector<ObjectGuid> m_WarEagleGuids;
        std::vector<ObjectGuid> m_StonefistGuids;
        ObjectGuid m_MaylaHighmountainGuid;
        ObjectGuid m_StonedarkGeomancerGuid;
        ObjectGuid m_LasanGuid;
        Position m_GameObjectCenter = { 4414.18f, 4528.268f, 960.8342f };
        float m_GameObjectRadius = 100.0f;

        void OnPlayerEnter(Player* p_Player) override
        {
            p_Player->SetPhaseMask(1, true);

            if (getScenarionStep() == eScenarioSteps::Step_SecuringThunderTotem)
            {
                p_Player->SetDisableGravity(true);
                p_Player->CastSpell(p_Player, eSpells::SummonIntroWarEagle, true);
            }
        }

        void OnCreatureCreateForScript(Creature* p_Creature) override
        {
            switch (p_Creature->GetEntry())
            {
            case eCreatures::Npc_MaylaHighmountain:
                m_MaylaHighmountainGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_StonedarkGeomancer:
                m_StonedarkGeomancerGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_WarEagle:
                m_WarEagleGuids.push_back(p_Creature->GetGUID());
                break;
            case eCreatures::Npc_Lasan:
                m_LasanGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_StonefistStalker:
            {
                m_StonefistGuids.push_back(p_Creature->GetGUID());
                break;
            }
            default:
                break;
            }
        }

        void onScenarionNextStep(uint32 p_NewStep) override
        {
            switch (p_NewStep)
            {
            case eScenarioSteps::Step_BaseDefense:
            {
                for (auto itr : Positions::HighmountainProtectorSpawnPos)
                {
                    if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_HighmountainProtector, itr))
                        m_HighmountainProtectorGuids.push_back(l_Creature->GetGUID());
                }

                DoCastSpellOnPlayers(eSpells::MaylaAssistantRequestConversation);
                break;
            }
            case eScenarioSteps::Step_Incoming:
            {
                uint8 i = 0;

                for (auto itr : m_HighmountainProtectorGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        l_Creature->SetWalk(false);
                        l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                        l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_HighmountainProtector_Defense, Positions::HighmountainProtectorDefensePos[i], false);
                        l_Creature->SetHomePosition(Positions::HighmountainProtectorDefensePos[i]);
                        l_Creature->Relocate(Positions::HighmountainProtectorDefensePos[i]);
                    }
                    i++;
                }

                if (Creature* l_Creature = instance->GetCreature(m_StonedarkGeomancerGuid))
                    l_Creature->CastSpell(l_Creature, eSpells::NatureChanneling, true);
                break;
            }
            case eScenarioSteps::Step_LastDefense:
            {
                if (Creature* l_Gargath = instance->SummonCreature(eCreatures::Npc_FelChampionGargath, Positions::FelChampionGargathSpawnPos))
                {
                    if (Creature* l_Creature = instance->GetCreature(m_MaylaHighmountainGuid))
                        l_Creature->AI()->DoAction(eActions::Action_Kneel);

                    if (Creature* l_Creature = instance->GetCreature(m_StonedarkGeomancerGuid))
                    {
                        l_Creature->CastSpell(l_Creature, eSpells::Disengage, true);
                        l_Creature->DespawnOrUnsummon(2000);
                    }

                    for (auto itr : m_HighmountainProtectorGuids)
                    {
                        if (Creature* l_Creature = instance->GetCreature(itr))
                            l_Creature->Attack(l_Gargath, true);
                    }
                }
                break;
            }
            case eScenarioSteps::Step_Regroup:
            {
                if (Creature* l_Creature = instance->GetCreature(m_MaylaHighmountainGuid))
                    l_Creature->AI()->DoAction(eActions::Action_Regroup);
                break;
            }
            case eScenarioSteps::Step_ToTheShip:
            {
                for (auto itr : m_WarEagleGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                        l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                break;
            }
            case eScenarioSteps::Step_DestroyedFromWithin:
            {
                for (auto itr : Positions::DynamitesSpawnPos)
                    instance->SummonCreature(eCreatures::Npc_Dynamite, itr);
                break;
            }
            case eScenarioSteps::Step_TheFinalFight:
            {
                for (auto itr : Positions::PortalsToFelCommanderSpawnPos)
                    instance->SummonCreature(eCreatures::Npc_PortalToFelCommander, itr);

                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Lasan, Positions::LasanCapturedSpawnPos))
                    l_Creature->SetAIAnimKitId(5081);

                DoCastSpellOnPlayers(eSpells::CapturedLasanConversation);
                break;
            }
            case eScenarioSteps::Step_TimeToGo:
            {
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_WarEagleEscape, Positions::WarEagleSpawnPosFirst))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_WarEagle_Escape, Waypoints::Path_WarEagle_Escape_First, 7);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_WarEagleEscape, Positions::WarEagleSpawnPosSecond))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_WarEagle_Escape, Waypoints::Path_WarEagle_Escape_Second, 7);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_WarEagleEscape, Positions::WarEagleSpawnPosThird))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_WarEagle_Escape, Waypoints::Path_WarEagle_Escape_Third, 7);

                if (Creature* l_Creature = instance->GetCreature(m_LasanGuid))
                {
                    l_Creature->SetAIAnimKitId(0, true);
                    l_Creature->CastSpell(l_Creature, eSpells::LasanRescuedConversation, true);
                }

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
            if (!l_Scenario || !l_Player || getScenarionStep() != eScenarioSteps::Step_SecuringThunderTotem)
                return;

            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_SecuringThunderForces, 0, 0, l_Player, l_Player);
        }

        void SetData(uint32 p_Type, uint32 /*p_Data*/) override
        {
            switch (p_Type)
            {
            case eDatas::Data_CastFirstFist:
            {
                if (Creature* l_Creature = instance->GetCreature(m_StonedarkGeomancerGuid))
                {
                    l_Creature->CastSpell(l_Creature, eSpells::CosmeticNatureCast, false);
                    l_Creature->CastSpell(l_Creature, eSpells::NatureChanneling, false);
                }

                for (auto itr : m_StonefistGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        if (int32(l_Creature->GetPositionX()) == 4132)
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::CosmeticDrogbarFistBlocker, true);
                        }
                    }
                }

                break;
            }
            case eDatas::Data_CastSecondFist:
            {
                if (Creature* l_Creature = instance->GetCreature(m_StonedarkGeomancerGuid))
                {
                    l_Creature->CastSpell(l_Creature, eSpells::CosmeticNatureCast, false);
                }

                for (auto itr : m_StonefistGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        if (int32(l_Creature->GetPositionX()) == 4100)
                        {
                            l_Creature->CastSpell(l_Creature, eSpells::CosmeticDrogbarFistBlocker, true);
                        }
                    }
                }

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

            switch (l_Scenario->GetCurrentStep())
            {
            case eScenarioSteps::Step_SecuringThunderTotem:
            case eScenarioSteps::Step_BaseDefense:
            case eScenarioSteps::Step_Incoming:
            case eScenarioSteps::Step_LastDefense:
            case eScenarioSteps::Step_Regroup:
                l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_ThunderTotem);
                break;
            default:
                l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Ship);
                break;
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
        return new instance_scenario_highmountain_assault_InstanceMapScript(map);
    }
};

void AddSC_instance_scenario_highmountain_assault()
{
    new instance_scenario_highmountain_assault();
}