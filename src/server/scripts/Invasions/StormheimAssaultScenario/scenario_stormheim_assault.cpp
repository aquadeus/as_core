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
#include "instance_scenario_stormheim_assault.h"

/// Odyn - 119092
class npc_odyn_119092 : public CreatureScript
{
public:
    npc_odyn_119092() : CreatureScript("npc_odyn_119092") { }

    struct npc_odyn_119092AI : public ScriptedAI
    {
        npc_odyn_119092AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        bool m_Moved = false;

        void MoveInLineOfSight(Unit* /*p_Who*/) override
        {
            if (!m_Moved)
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_Odyn_Path, Waypoints::Path_Odyn, 9, true);
                m_Moved = true;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_odyn_119092AI(p_Creature);
    }
};

/// 118789, 119200, 119201
class npc_captured_dragons : public CreatureScript
{
public:
    npc_captured_dragons() : CreatureScript("npc_captured_dragons") { }

    struct npc_captured_dragonsAI : public ScriptedAI
    {
        npc_captured_dragonsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            FreeingDrake = 235735
        };

        EventMap m_Events;

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            Player* l_Player = p_Caster->ToPlayer();
            if (!l_Scenario || !l_Player)
                return;

            if (p_Spell->Id == eSpells::FreeingDrake)
            {
                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Freedom, 0, 0, l_Player, l_Player);
                me->PlayOneShotAnimKitId(1439);
                me->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                m_Events.ScheduleEvent(c_events::EVENT_1, 6000);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_Dragons_Freedom)
                me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case c_events::EVENT_1:
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Freedom, Waypoints::Path_Dragons_Freedom, 4);
                break;
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_captured_dragonsAI(p_Creature);
    }
};

/// 119196, 119197, 119198
class npc_cycle_dragons : public CreatureScript
{
public:
    npc_cycle_dragons() : CreatureScript("npc_cycle_dragons") { }

    struct npc_cycle_dragonsAI : public VehicleAI
    {
        npc_cycle_dragonsAI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        ObjectGuid m_OwnerGuid;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            if (!l_Scenario)
                return;

            m_OwnerGuid = p_Summoner->GetGUID();

            if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_SiegeBreaker)
            {
                switch (me->GetEntry())
                {
                case eCreatures::Npc_Aleifir_Cycle:
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Cycle, Waypoints::Path_Aleifir_Cycle, 10);
                    break;
                case eCreatures::Npc_Hrafsir_Cycle:
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Cycle, Waypoints::Path_Erilar_Cycle, 10);
                    break;
                case eCreatures::Npc_Erilar_Cycle:
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Cycle, Waypoints::Path_Erilar_Cycle, 10);
                    break;
                default:
                    break;
                }
            }
            else
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Ship, Waypoints::Path_To_Ship, 5);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            InstanceScript* l_Instance = me->GetInstanceScript();
            Player* l_Summoner = ObjectAccessor::GetPlayer(*me, m_OwnerGuid);
            if (!l_Scenario || !l_Instance || !l_Summoner)
                return;

            switch (p_PointId)
            {
            case ePoints::Point_Dragons_Cycle:
            {
                if (l_Scenario->GetCurrentStep() != eScenarioSteps::Step_SiegeBreaker)
                    break;

                switch (me->GetEntry())
                {
                case eCreatures::Npc_Aleifir_Cycle:
                    me->GetMotionMaster()->MoveSmoothFlyPath(10, Positions::AleifirHackPos);
                    me->GetMotionMaster()->Clear(true);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Cycle, Waypoints::Path_Aleifir_Cycle, 10);
                    break;
                case eCreatures::Npc_Hrafsir_Cycle:
                    me->GetMotionMaster()->MoveSmoothFlyPath(10, Positions::HrafsirHackPos);
                    me->GetMotionMaster()->Clear(true);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Cycle, Waypoints::Path_Erilar_Cycle, 10);
                    break;
                case eCreatures::Npc_Erilar_Cycle:
                    me->GetMotionMaster()->MoveSmoothFlyPath(10, Positions::ErilarHackPos);
                    me->GetMotionMaster()->Clear(true);
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Cycle, Waypoints::Path_Erilar_Cycle, 10);
                    break;
                default:
                    break;
                }
                break;
            case ePoints::Point_Dragons_Ship:
            {
                if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_Insertion)
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Insertion, 0, 0, l_Summoner, l_Summoner);
                l_Summoner->ExitVehicle();
                break;
            }
            default:
                break;
            }
            }
        }

        void DoAction(const int32 /*p_Action*/) override
        {
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Dragons_Ship, Waypoints::Path_To_Ship, 5);
        }

        void UpdateAI(uint32 const /*p_Diff*/) override
        {
            Player* l_Summoner = ObjectAccessor::GetPlayer(*me, m_OwnerGuid);
            if (!l_Summoner || !l_Summoner->IsInWorld())
            {
                me->DespawnOrUnsummon();
                return;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_cycle_dragonsAI(p_Creature);
    }
};

/// 118858, 119224, 119225
class npc_get_out_dragons : public CreatureScript
{
public:
    npc_get_out_dragons() : CreatureScript("npc_get_out_dragons") { }

    struct npc_get_out_dragonsAI : public VehicleAI
    {
        npc_get_out_dragonsAI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        void InitializeAI() override
        {
            switch (me->GetEntry())
            {
            case eCreatures::Npc_Aleifir_GetOut:
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Get_Out, Waypoints::Path_Aleifir_Get_Out, 2);
                break;
            case eCreatures::Npc_Hrafsir_GetOut:
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Get_Out, Waypoints::Path_Hrafsir_Get_Out, 3);
                break;
            case eCreatures::Npc_Erilar_GetOut:
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Get_Out, Waypoints::Path_Erilar_Get_Out, 3);
                break;
            default:
                break;
            }
        }

        void OnSpellClick(Unit* p_Clicker) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            Player* l_Player = p_Clicker->ToPlayer();
            if (!l_Scenario || !l_Player)
                return;

            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_GetOutOfThere, 0, 0, l_Player, l_Player);
            l_Player->SetDisableGravity(true);
            l_Player->SetRooted(true);
            l_Player->PlayScene(1836, l_Player);
            l_Player->SetPhaseMask(2, true);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_Get_Out)
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_get_out_dragonsAI(p_Creature);
    }
};

/// Shock Lightning - 235745
class spell_gen_shock_lightning : public SpellScriptLoader
{
public:
    spell_gen_shock_lightning() : SpellScriptLoader("spell_gen_shock_lightning") { }

    class spell_gen_shock_lightning_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_shock_lightning_SpellScript);

        void HandleHitTarget(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Target = GetHitUnit();
            Unit* l_Caster = GetCaster();
            if (!l_Target || !l_Caster)
                return;

            Map* l_Map = l_Caster->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            if (!l_Scenario)
                return;

            if (l_Target->IsFriendlyTo(l_Caster) || l_Scenario->GetCurrentStep() != eScenarioSteps::Step_SiegeBreaker)
                SetHitDamage(0);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_shock_lightning_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_shock_lightning_SpellScript();
    }
};

/// Cosmetic Fel Cannon Channel - 236169
class spell_gen_cosmetic_fel_cannon_channel : public SpellScriptLoader
{
public:
    spell_gen_cosmetic_fel_cannon_channel() : SpellScriptLoader("spell_gen_cosmetic_fel_cannon_channel") { }

    class spell_gen_cosmetic_fel_cannon_channel_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_cosmetic_fel_cannon_channel_SpellScript);

        enum eSpells
        {
            Spell_CosmeticFelCannonChannelMissle = 236168
        };

        int32 m_Delay = 0;

        void HandleHitTarget(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            l_Caster->DelayedCastSpell(l_Target, eSpells::Spell_CosmeticFelCannonChannelMissle, true, m_Delay);
            m_Delay += 200;
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_cosmetic_fel_cannon_channel_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_cosmetic_fel_cannon_channel_SpellScript();
    }
};

class playerscript_scene_leave_stormheim_scenario : public PlayerScript
{
public:
    playerscript_scene_leave_stormheim_scenario() : PlayerScript("playerscript_scene_leave_stormheim_scenario") {}

    void OnSceneCancel(Player* p_Player, uint32 /*p_SceneInstanceId*/) override
    {
        if (p_Player->GetMapId() == 1707)
        {
            p_Player->SetPhaseMask(1, true);
            p_Player->SetDisableGravity(false);
            p_Player->SetRooted(false);
            p_Player->TeleportTo(1220, Positions::LeaveScenario);
        }
    }

    void OnSceneComplete(Player* p_Player, uint32 /*p_SceneInstanceID*/) override
    {
        if (p_Player->GetMapId() == 1707)
        {
            p_Player->SetPhaseMask(1, true);
            p_Player->SetDisableGravity(false);
            p_Player->SetRooted(false);
            p_Player->TeleportTo(1220, Positions::LeaveScenario);
        }
    }
};

#ifndef __clang_analyzer__
void AddSC_scenario_stormheim_assault()
{
    new npc_odyn_119092();
    new npc_captured_dragons();
    new npc_cycle_dragons();
    new npc_get_out_dragons();
    new spell_gen_shock_lightning();
    new spell_gen_cosmetic_fel_cannon_channel();
    new playerscript_scene_leave_stormheim_scenario();
}
#endif