////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScenarioMgr.h"
#include "instance_scenario_highmountain_assault.h"

/// 119934 - War Eagle
class npc_war_eagle_119934 : public CreatureScript
{
public:
    npc_war_eagle_119934() : CreatureScript("npc_war_eagle_119934") { }

    struct npc_war_eagle_119934AI : public VehicleAI
    {
        npc_war_eagle_119934AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        ObjectGuid m_SummonerGuid;
        EventMap m_Events;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
            p_Summoner->CastSpell(me, VehicleSpells::VEHICLE_SPELL_RIDE_HARDCODED, true);
            me->SetSpeed(UnitMoveType::MOVE_RUN, 3.0f, false);
            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_WarEagle_ThunderTotem, Waypoints::Path_WarEagleIntro_ThunderTotem, 10);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player)
                return;

            if (p_PointId == ePoints::Point_WarEagle_ThunderTotem)
            {
                l_Player->ExitVehicle();
                l_Player->SetDisableGravity(false);
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_war_eagle_119934AI(p_Creature);
    }
};

/// 119850 - Mayla Highmountain
class npc_mayla_highmountain_119850 : public CreatureScript
{
public:
    npc_mayla_highmountain_119850() : CreatureScript("npc_mayla_highmountain_119850") { }

    struct npc_mayla_highmountain_119850AI : public ScriptedAI
    {
        enum eEvents
        {
            Event_StartFirstWave = 1,
            Event_StartSecondWave = 2,
            Event_StartThirdWave = 3,
            Event_Kneel_First = 4,
            Event_Kneel_Second = 5,
            Event_Lasan_Conversation = 6,
            Event_GotItConversation = 7,
            Event_Start_Path = 8,
            Event_Continue_Path = 9
        };

        enum eSpells
        {
            Kneel = 68442,
            Jump = 238661,
            LasanEscortConversation = 239066,
            MaylaStartPathConversation = 241730,
            MaylaPathEndConversation = 239091,
            LasanLeaveConversation = 239134
        };

        npc_mayla_highmountain_119850AI(Creature* p_Creature) : ScriptedAI(p_Creature) 
        {
            l_Instance = me->GetInstanceScript();
        }

        EventMap m_Events;
        uint8 m_DemonsDead = 0;
        InstanceScript* l_Instance;
        bool m_FirstWaveCompleted = false;
        bool m_CheckOOCLos = false;

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Scenario || !l_Player || me->GetExactDist2d(p_Who) > 20)
                return;

            if (l_Instance->getScenarionStep() == Step_BaseDefense && !m_CheckOOCLos)
            {
                m_CheckOOCLos = true;
                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT, eScenarioAssets::Asset_BaseDefense, 0, 0, l_Player, l_Player);
                Talk(0);
                m_Events.ScheduleEvent(eEvents::Event_StartFirstWave, 2000);
            }
        }

        void SummonedCreatureDies(Creature* p_Summon, Unit* p_Killer) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            Player* l_Player = p_Killer->ToPlayer();
            if (!l_Scenario || !l_Player || !l_Instance)
                return;

            if (p_Summon->GetEntry() == eCreatures::Npc_HellfireInfernal)
            {
                Talk(5);
                m_Events.ScheduleEvent(eEvents::Event_Continue_Path, 4000);
            }

            if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_Incoming)
            {
                ++m_DemonsDead;

                if (m_DemonsDead == 3 && !m_FirstWaveCompleted)
                {
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Incoming, 0, 0, l_Player, l_Player);
                    l_Instance->SetData(eDatas::Data_CastFirstFist, 0);
                    m_DemonsDead = 0;
                    m_FirstWaveCompleted = true;

                    if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_Incoming)
                        m_Events.ScheduleEvent(eEvents::Event_StartSecondWave, 2000);
                }

                if (m_DemonsDead == 4)
                {
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Incoming, 0, 0, l_Player, l_Player);
                    l_Instance->SetData(eDatas::Data_CastSecondFist, 0);
                    m_DemonsDead = 0;
                }
            }
        }

        void DoAction(const int32 p_Action) override
        {
            switch (p_Action)
            {
            case eActions::Action_Kneel:
            {
                DoCast(eSpells::Jump, true);
                Talk(1);
                m_Events.ScheduleEvent(eEvents::Event_Kneel_First, 1000);
                break;
            }
            case eActions::Action_Regroup:
            {
                me->RemoveAura(eSpells::Kneel);
                Talk(3);
                m_Events.ScheduleEvent(eEvents::Event_Lasan_Conversation, 7000);
                break;
            }
            default:
                break;
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            if (!l_Scenario)
                return;

            switch (p_PointId)
            {
            case Point_Mayla_Escort_First:
            {
                me->SummonCreature(eCreatures::Npc_HellfireInfernal, Positions::HellfireInfernalSpawnPos);
                Talk(4);
                break;
            }
            case Point_Mayla_Escort_Second:
            {
                if (Player* l_Player = me->FindNearestPlayer(100.0f))
                {
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Regroup, 0, 0, l_Player, l_Player);
                    DoCast(eSpells::MaylaPathEndConversation, true);
                    me->DelayedCastSpell(me, eSpells::LasanLeaveConversation, true, 21000);
                }
                break;
            }
            default:
                break;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case eEvents::Event_StartFirstWave:
            {
                if (Creature* l_Creature = me->SummonCreature(RAND(eCreatures::Npc_FelstoneWrathguard, eCreatures::Npc_FelTouchedPyromage), Positions::DemonInvadersPos[0]))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Invander_Attack, Waypoints::Path_Invader_Left, 23, false);
                if (Creature* l_Creature = me->SummonCreature(RAND(eCreatures::Npc_FelstoneWrathguard, eCreatures::Npc_FelTouchedPyromage), Positions::DemonInvadersPos[1]))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Invander_Attack, Waypoints::Path_Invader_Middle, 11, false);
                if (Creature* l_Creature = me->SummonCreature(RAND(eCreatures::Npc_FelstoneWrathguard, eCreatures::Npc_FelTouchedPyromage), Positions::DemonInvadersPos[2]))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Invander_Attack, Waypoints::Path_Invader_Right, 13, false);
                break;
            }
            case eEvents::Event_StartSecondWave:
            {
                if (Creature* l_Creature = me->SummonCreature(RAND(eCreatures::Npc_FelstoneWrathguard, eCreatures::Npc_FelTouchedPyromage), Positions::DemonInvadersPos[0]))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Invander_Attack, Waypoints::Path_Invader_Left, 23, false);
                if (Creature* l_Creature = me->SummonCreature(RAND(eCreatures::Npc_FelstoneWrathguard, eCreatures::Npc_FelTouchedPyromage), Positions::DemonInvadersPos[1]))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Invander_Attack, Waypoints::Path_Invader_Middle, 11, false);

                m_Events.ScheduleEvent(eEvents::Event_StartThirdWave, 1000);
                break;
            }
            case eEvents::Event_StartThirdWave:
            {
                if (Creature* l_Creature = me->SummonCreature(RAND(eCreatures::Npc_FelstoneWrathguard, eCreatures::Npc_FelTouchedPyromage), Positions::DemonInvadersPos[0]))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Invander_Attack, Waypoints::Path_Invader_Left, 23, false);
                if (Creature* l_Creature = me->SummonCreature(RAND(eCreatures::Npc_FelstoneWrathguard, eCreatures::Npc_FelTouchedPyromage), Positions::DemonInvadersPos[1]))
                    l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Invander_Attack, Waypoints::Path_Invader_Middle, 11, false);
                break;
            }
            case eEvents::Event_Kneel_First:
            {
                me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 670.7088f, me->GetOrientation());
                DoCast(eSpells::Kneel, true);
                me->SetRooted(true);
                m_Events.ScheduleEvent(eEvents::Event_Kneel_Second, 3000);
                break;
            }
            case eEvents::Event_Kneel_Second:
            {
                Talk(2);
                break;
            }
            case eEvents::Event_Lasan_Conversation:
            {
                DoCast(eSpells::LasanEscortConversation);
                me->SetRooted(false);
                m_Events.ScheduleEvent(eEvents::Event_Start_Path, 7000);
                break;
            }
            case eEvents::Event_Start_Path:
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Mayla_Escort_First, Waypoints::Path_Mayla_Escort_First, 11, false);
                m_Events.ScheduleEvent(eEvents::Event_GotItConversation, 2000);
                break;
            }
            case eEvents::Event_GotItConversation:
            {
                DoCast(eSpells::MaylaStartPathConversation, true);
                break;
            }
            case eEvents::Event_Continue_Path:
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Mayla_Escort_Second, Waypoints::Path_Mayla_Escort_Second, 38, false);
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_mayla_highmountain_119850AI(p_Creature);
    }
};

/// 119857 - War Eagle
class npc_war_eagle_119857 : public CreatureScript
{
public:
    npc_war_eagle_119857() : CreatureScript("npc_war_eagle_119857") { }

    struct npc_war_eagle_119857AI : public VehicleAI
    {
        npc_war_eagle_119857AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        ObjectGuid m_Clicker;
        EventMap m_Events;

        void OnSpellClick(Unit* p_Clicker) override
        {
            if (!p_Clicker->IsMounted())
            {
                m_Clicker = p_Clicker->GetGUID();
                me->SetSpeed(UnitMoveType::MOVE_RUN, 5.0f, false);
                m_Events.ScheduleEvent(c_events::EVENT_1, 1000);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_Clicker);
            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            if (!l_Player || !l_Scenario)
                return;

            switch (p_PointId)
            {
            case ePoints::Point_WarEagle_Ship:
            {
                if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_ToTheShip)
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_ToTheShip, 0, 0, l_Player, l_Player);

                l_Player->ExitVehicle();
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_WarEagle_Despawn, Waypoints::Path_WarEagle_Despawn, 4);
                break;
            }
            case ePoints::Point_WarEagle_Despawn:
            {
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case c_events::EVENT_1:
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_WarEagle_Ship, Waypoints::Path_WarEagle_Ship, 11);
                break;
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_war_eagle_119857AI(p_Creature);
    }
};

/// 120048 - War Eagle
class npc_war_eagle_120048 : public CreatureScript
{
public:
    npc_war_eagle_120048() : CreatureScript("npc_war_eagle_120048") { }

    struct npc_war_eagle_120048AI : public ScriptedAI
    {
        npc_war_eagle_120048AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        void OnSpellClick(Unit* p_Clicker) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Player* l_Player = p_Clicker->ToPlayer();
            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            if (!l_Player || !l_Scenario)
                return;

            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Leave, 0, 0, l_Player, l_Player);
            l_Player->SetDisableGravity(true);
            l_Player->SetRooted(true);
            l_Player->PlayScene(1872, l_Player);
            l_Player->SetPhaseMask(2, true);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_war_eagle_120048AI(p_Creature);
    }
};

class playerscript_scene_leave_highmountain_scenario : public PlayerScript
{
public:
    playerscript_scene_leave_highmountain_scenario() : PlayerScript("playerscript_scene_leave_highmountain_scenario") {}

    void OnSceneCancel(Player* p_Player, uint32 /*p_SceneInstanceId*/) override
    {
        if (p_Player->GetMapId() == 1706)
        {
            p_Player->SetPhaseMask(1, true);
            p_Player->SetDisableGravity(false);
            p_Player->SetRooted(false);
            p_Player->TeleportTo(1220, Positions::ScenarioEndTeleportPosition);
        }
    }

    void OnSceneComplete(Player* p_Player, uint32 /*p_SceneInstanceID*/) override
    {
        if (p_Player->GetMapId() == 1706)
        {
            p_Player->SetPhaseMask(1, true);
            p_Player->SetDisableGravity(false);
            p_Player->SetRooted(false);
            p_Player->TeleportTo(1220, Positions::ScenarioEndTeleportPosition);
        }
    }
};

#ifndef __clang_analyzer__
void AddSC_scenario_highmountain_assault()
{
    new npc_war_eagle_119934();
    new npc_mayla_highmountain_119850();
    new npc_war_eagle_119857();
    new npc_war_eagle_120048();
    new playerscript_scene_leave_highmountain_scenario();
}
#endif