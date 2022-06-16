////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "InstanceScript.h"
#include "ScenarioMgr.h"
#include "instance_scenario_valsharah_assault.h"

class instance_scenario_valsharah_assault : public InstanceMapScript
{
public:
    instance_scenario_valsharah_assault() : InstanceMapScript("instance_scenario_valsharah_assault", 1704) { }

    struct instance_scenario_valsharah_assault_InstanceMapScript : public InstanceScript
    {
        instance_scenario_valsharah_assault_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
        }

        std::vector<ObjectGuid> m_GroveMarksmanGuids;
        std::vector<ObjectGuid> m_DreamWeaverGuids;
        std::vector<ObjectGuid> m_FelCannonGuids;
        std::vector<ObjectGuid> m_HippogryphGuids;
        ObjectGuid m_FelCoreTargetStalkerGuid;
        ObjectGuid m_FelCoreGuid;
        ObjectGuid m_DemonShipBomberGuid;
        ObjectGuid m_InvisibleCollisionGuid;
        Position m_GameObjectCenter = { 2955.683594f, 7753.298828f, 293.893188f };
        float m_GameObjectRadius = 70.0f;

        void OnPlayerEnter(Player* p_Player) override
        {
            p_Player->SetPhaseMask(1, true);
        }

        void OnCreatureCreateForScript(Creature* p_Creature) override
        {
            switch (p_Creature->GetEntry())
            {
            case eCreatures::Npc_GroveMarksman:
                m_GroveMarksmanGuids.push_back(p_Creature->GetGUID());
                break;
            case eCreatures::Npc_DreamWeaver:
                if (p_Creature->GetUInt32Value(UNIT_FIELD_EMOTE_STATE) == Emote::EMOTE_STATE_READYBOW)
                    m_DreamWeaverGuids.push_back(p_Creature->GetGUID());
                break;
            case eCreatures::Npc_FelCoreTargetStalker:
                m_FelCoreTargetStalkerGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_FelCore:
                m_FelCoreGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_FelCannon:
                m_FelCannonGuids.push_back(p_Creature->GetGUID());
                break;
            case eCreatures::Npc_DemonShipBomber:
                m_DemonShipBomberGuid = p_Creature->GetGUID();
                break;
            case eCreatures::Npc_Hippogryph:
                m_HippogryphGuids.push_back(p_Creature->GetGUID());
                break;
            default:
                break;
            }
        }

        void OnGameObjectCreateForScript(GameObject* p_Go) override
        {
            switch (p_Go->GetEntry())
            {
            case eGameobjects::Gob_InvisibleCollision:
                m_InvisibleCollisionGuid = p_Go->GetGUID();
                break;
            default:
                break;
            }
        }

        void onScenarionNextStep(uint32 p_NewStep) override
        {
            switch (p_NewStep)
            {
            case eScenarioSteps::Step_OpenTheWay:
                if (Creature* l_Creature = instance->GetCreature(m_DemonShipBomberGuid))
                    l_Creature->AI()->SetData(0, 0);
                break;
            case eScenarioSteps::Step_MovingOnUp:
                if (Creature* l_Creature = instance->GetCreature(m_FelCoreTargetStalkerGuid))
                    l_Creature->RemoveAllAuras();
                if (Creature* l_Creature = instance->GetCreature(m_FelCoreGuid))
                    l_Creature->RemoveAllAuras();
                if (GameObject* go = instance->GetGameObject(m_InvisibleCollisionGuid))
                    go->Delete();
                if (Creature* l_Creature = instance->GetCreature(m_DemonShipBomberGuid))
                    l_Creature->AI()->SetData(1, 1);
                break;
            case eScenarioSteps::Step_ClearTheSkies:
                for (auto itr : m_FelCannonGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                        l_Creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                break;
            case eScenarioSteps::Step_UpAndAway:
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Hippogryph, Positions::HippogryphSpawnPosFirst))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Tower, Waypoints::Path_HippogryphFirst, 9);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Hippogryph, Positions::HippogryphSpawnPosSecond))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Tower, Waypoints::Path_HippogryphSecond, 9);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Hippogryph, Positions::HippogryphSpawnPosThird))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Tower, Waypoints::Path_HippogryphThird, 9);
                break;
            case eScenarioSteps::Step_Escape:
            {
                Map::PlayerList const &PlayersList = instance->GetPlayers();
                if (PlayersList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlayersList.begin(); i != PlayersList.end(); ++i)
                {
                    if (Player* l_Player = i->getSource())
                    {
                        if (l_Player->isGameMaster())
                            continue;

                        if (Conversation* l_Conversation = new Conversation)
                        {
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_DreadVizerDefeated, l_Player, nullptr, l_Player->GetPosition()))
                                delete l_Conversation;
                        }
                    }
                }

                break;
            }
            default:
                break;
            }
        }

        void SetData(uint32 p_Type, uint32 p_Data) override
        {
            switch (p_Type)
            {
            case eData::Data_Defenders_Move:
            {
                uint8 i = 0;
                uint8 j = 0;

                for (auto itr : m_GroveMarksmanGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        l_Creature->SetReactState(ReactStates::REACT_PASSIVE);

                        switch (i)
                        {
                        case 0:
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Defenders_Path, Waypoints::Path_GroveMarksmanFirst, 7, false);
                            break;
                        case 1:
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Defenders_Path, Waypoints::Path_GroveMarksmanSecond, 7, false);
                            break;
                        default:
                            break;
                        }

                        i++;
                    }
                }

                for (auto itr : m_DreamWeaverGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        l_Creature->SetReactState(ReactStates::REACT_PASSIVE);

                        switch (j)
                        {
                        case 0:
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Defenders_Path, Waypoints::Path_DreamWeaverFirst, 8, false);
                            break;
                        case 1:
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Defenders_Path, Waypoints::Path_DreamWeaverSecond, 8, false);
                            break;
                        case 2:
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Defenders_Path, Waypoints::Path_DreamWeaverThird, 9, false);
                            break;
                        case 3:
                            l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Defenders_Path, Waypoints::Path_DreamWeaverFourth, 8, false);
                            break;
                        default:
                            break;
                        }

                        j++;
                    }
                }
                break;
            }
            case eData::Data_Summon_Crushfist:
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Crushfist, Positions::CrushfistSummonPos))
                    l_Creature->CastSpell(Positions::CrushfistJumpPos, eSpells::IntoTheFray, true);
                break;
            case eData::Data_New_Hippogryphs:
                if (!m_HippogryphGuids.empty())
                {
                    for (auto itr : m_HippogryphGuids)
                    {
                        if (Creature* l_Creature = instance->GetCreature(itr))
                            l_Creature->DespawnOrUnsummon();
                    }

                    m_HippogryphGuids.clear();
                }

                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Hippogryph, Positions::HippogryphSpawnPosFirst))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Tower, Waypoints::Path_HippogryphFirst, 9);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Hippogryph, Positions::HippogryphSpawnPosSecond))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Tower, Waypoints::Path_HippogryphSecond, 9);
                if (Creature* l_Creature = instance->SummonCreature(eCreatures::Npc_Hippogryph, Positions::HippogryphSpawnPosThird))
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Tower, Waypoints::Path_HippogryphThird, 9);
                break;
            default:
                break;
            }

        }

        void EnterCombatForScript(Creature* p_Creature, Unit* p_Enemy) override
        {
            if (p_Creature->GetEntry() == eCreatures::Npc_Crushfist && p_Enemy->ToPlayer())
            {
                for (auto itr : m_GroveMarksmanGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        l_Creature->SetReactState(ReactStates::REACT_HELPER);
                        l_Creature->Attack(p_Creature, false);
                    }
                }

                for (auto itr : m_DreamWeaverGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        l_Creature->SetReactState(ReactStates::REACT_HELPER);
                        l_Creature->Attack(p_Creature, false);
                    }
                }
            }
        }

        void CreatureDiesForScript(Creature* p_Creature, Unit* /*p_Killer*/) override
        {
            if (p_Creature->GetEntry() == eCreatures::Npc_Crushfist)
            {
                uint8 i = 0;
                uint8 j = 0;

                for (auto itr : m_GroveMarksmanGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        l_Creature->SetWalk(false);

                        switch (i)
                        {
                        case 0:
                            l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_Defenders_Last, Positions::GroveMarksmanPositionFirst, false);
                            break;
                        case 1:
                            l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_Defenders_Last, Positions::GroveMarksmanPositionSecond, false);
                            break;
                        default:
                            break;
                        }

                        i++;
                    }
                }

                for (auto itr : m_DreamWeaverGuids)
                {
                    if (Creature* l_Creature = instance->GetCreature(itr))
                    {
                        l_Creature->SetWalk(false);

                        switch (j)
                        {
                        case 0:
                            l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_Defenders_Last, Positions::DreamWeaverPositionFirst, false);
                            break;
                        case 1:
                            l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_Defenders_Last, Positions::DreamWeaverPositionSecond, false);
                            break;
                        case 2:
                            l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_Defenders_Last, Positions::DreamWeaverPositionThird, false);
                            break;
                        case 3:
                            l_Creature->GetMotionMaster()->MovePoint(ePoints::Point_Defenders_Last, Positions::DreamWeaverPositionFourth, false);
                            break;
                        default:
                            break;
                        }

                        j++;
                    }
                }

                m_GroveMarksmanGuids.clear();
                m_DreamWeaverGuids.clear();
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
                case eScenarioSteps::Step_SpeakWithCenarius:
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Cenarius);
                    break;
                case eScenarioSteps::Step_SecureAFoothold:
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Cenarius);
                    break;
                case eScenarioSteps::Step_OpenTheWay:
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Climb);
                    break;
                case eScenarioSteps::Step_MovingOnUp:
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Climb);
                    break;
                default:
                    l_Loc = sWorldSafeLocsStore.LookupEntry(eSafeLocs::Loc_Tower);
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
        return new instance_scenario_valsharah_assault_InstanceMapScript(map);
    }
};

void AddSC_instance_scenario_valsharah_assault()
{
    new instance_scenario_valsharah_assault();
}