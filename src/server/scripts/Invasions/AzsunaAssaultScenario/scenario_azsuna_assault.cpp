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
#include "instance_scenario_azsuna_assault.h"

/// 119456 - Azure War-Drake
class npc_azure_war_drake_119456 : public CreatureScript
{
public:
    npc_azure_war_drake_119456() : CreatureScript("npc_azure_war_drake_119456") { }

    struct npc_azure_war_drake_119456AI : public VehicleAI
    {
        npc_azure_war_drake_119456AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        ObjectGuid m_Clicker;

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Scenario || !l_Player || me->GetExactDist2d(p_Who) > 10.0f ||
                l_Scenario->GetCurrentStep() != eScenarioSteps::Step_MeetUpWithTheBlueDrakes)
                return;

            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_MeetUpWithTheBlueDrakes, 0, 0, l_Player, l_Player);
        }

        void OnSpellClick(Unit* p_Clicker) override
        {
            if (!p_Clicker->IsMounted())
            {
                m_Clicker = p_Clicker->GetGUID();
                me->SetSpeed(UnitMoveType::MOVE_RUN, 5.0f, false);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Ship, Waypoints::Path_AzureWarDrakeToShip, 8);
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
            case ePoints::Point_AzureWarDrake_Land:
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                break;
            case ePoints::Point_AzureWarDrake_Ship:
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_Clicker))
                {
                    if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_ReachTheLegionShip)
                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_ReachTheLegionShip, 0, 0, l_Player, l_Player);
                    l_Player->ExitVehicle();
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_AzureWarDrake_Despawn, Waypoints::Path_AzureWarDrakeDespawn, 3);
                }
                break;
            }
            case ePoints::Point_AzureWarDrake_Despawn:
                me->DespawnOrUnsummon();
                break;
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_azure_war_drake_119456AI(p_Creature);
    }
};

/// 119633 - Fel Imp
class npc_fel_imp_119633 : public CreatureScript
{
public:
    npc_fel_imp_119633() : CreatureScript("npc_fel_imp_119633") { }

    struct npc_fel_imp_119633AI : public ScriptedAI
    {
        npc_fel_imp_119633AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Volatile = 237720
        };

        void InitializeAI() override
        {
            me->SetReactState(ReactStates::REACT_PASSIVE);
            me->CastSpell(me, eSpells::Volatile, true);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_Imp_Path)
            {
                me->Kill(me);
                me->DespawnOrUnsummon(2000);
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_fel_imp_119633AI(p_Creature);
    }
};

/// 119459 - Azure War-Drake
class npc_azure_war_drake_119459 : public CreatureScript
{
public:
    npc_azure_war_drake_119459() : CreatureScript("npc_azure_war_drake_119459") { }

    struct npc_azure_war_drake_119459AI : public VehicleAI
    {
        npc_azure_war_drake_119459AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        enum eSpells
        {
            PlayScene = 239285
        };

        void OnSpellClick(Unit* p_Clicker) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            Player* l_Player = p_Clicker->ToPlayer();
            InstanceScript* l_Instance = me->GetInstanceScript();
            if (!l_Scenario || !l_Player || !l_Instance)
                return;

            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Escape, 0, 0, l_Player, l_Player);
            l_Player->SetDisableGravity(true);
            l_Player->SetRooted(true);
            l_Player->PlayScene(1845, l_Player);
            l_Player->SetPhaseMask(2, true);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_AzureWarDrake_Escape)
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_azure_war_drake_119459AI(p_Creature);
    }
};

/// 237684 - Fel-Fire Ejector Controller
class spell_gen_fel_fire_ejector_controller : public SpellScriptLoader
{
public:
    spell_gen_fel_fire_ejector_controller() : SpellScriptLoader("spell_gen_fel_fire_ejector_controller") { }

    class spell_gen_fel_fire_ejector_controller_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_fel_fire_ejector_controller_AuraScript);

        uint8 m_CurEjector = 0;

        void HandlePeriodic(AuraEffect const* /*p_AurEff*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            InstanceScript* l_Instance = l_Caster->GetInstanceScript();
            if (!l_Instance)
                return;

            switch (m_CurEjector)
            {
            case 0:
            {
                l_Instance->SetData(eData::Data_First_Ejector, 0);
                m_CurEjector++;
                break;
            }
            case 1:
            {
                l_Instance->SetData(eData::Data_Second_Ejector, 0);
                m_CurEjector++;
                break;
            }
            case 2:
            {
                l_Instance->SetData(eData::Data_Third_Ejector, 0);
                m_CurEjector++;
                break;
            }
            case 3:
            {
                l_Instance->SetData(eData::Data_Fourth_Ejector, 0);
                m_CurEjector = 0;
                break;
            }
            default:
                break;
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_fel_fire_ejector_controller_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_fel_fire_ejector_controller_AuraScript();
    }
};

/// 237671 - Fel-Fire Ejection Damage
class spell_gen_fel_fire_ejection_damage : public SpellScriptLoader
{
public:
    spell_gen_fel_fire_ejection_damage() : SpellScriptLoader("spell_gen_fel_fire_ejection_damage") { }

    class spell_gen_fel_fire_ejection_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_fel_fire_ejection_damage_SpellScript);

        void HandleHitTarget(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            if (!l_Target->IsPlayer())
            {
                SetHitDamage(0);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_fel_fire_ejection_damage_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_fel_fire_ejection_damage_SpellScript();
    }
};

/// 237653 - Fel Imps Gauntlet
class spell_gen_fel_imps_gauntlet : public SpellScriptLoader
{
public:
    spell_gen_fel_imps_gauntlet() : SpellScriptLoader("spell_gen_fel_imps_gauntlet") { }

    class spell_gen_fel_imps_gauntlet_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_fel_imps_gauntlet_AuraScript);

        uint8 m_CurImpSide = 0;

        void HandlePeriodic(AuraEffect const* /*p_AurEff*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            InstanceScript* l_Instance = l_Caster->GetInstanceScript();
            if (!l_Instance)
                return;

            switch (m_CurImpSide)
            {
            case 0:
            {
                l_Instance->SetData(eData::Data_LeftSide_FirstPath, 0);
                m_CurImpSide++;
                break;
            }
            case 1:
            {
                l_Instance->SetData(eData::Data_RightSide_FirstPath, 0);
                m_CurImpSide++;
                break;
            }
            case 2:
            {
                l_Instance->SetData(eData::Data_LeftSide_SecondPath, 0);
                m_CurImpSide++;
                break;
            }
            case 3:
            {
                l_Instance->SetData(eData::Data_RightSide_SecondPath, 0);
                m_CurImpSide = 0;
                break;
            }
            default:
                break;
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_fel_imps_gauntlet_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_fel_imps_gauntlet_AuraScript();
    }
};

/// 237720 - Volatile
class at_volatile : public AreaTriggerEntityScript
{
public:
    at_volatile() : AreaTriggerEntityScript("at_volatile") { }

    enum eSpells
    {
        FelImpLosion = 237716,
        Suicide = 8329
    };

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster || !l_Caster->ToCreature())
            return;

        if (l_Caster->FindNearestPlayer(2.0f))
        {
            l_Caster->CastSpell(l_Caster, eSpells::FelImpLosion, true);
            l_Caster->CastSpell(l_Caster, eSpells::Suicide, true);
            l_Caster->ToCreature()->DespawnOrUnsummon(2000);
            p_AreaTrigger->SetDuration(0);
        }
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new at_volatile();
    }
};

class playerscript_scene_leave_azsuna_scenario : public PlayerScript
{
public:
    playerscript_scene_leave_azsuna_scenario() : PlayerScript("playerscript_scene_leave_azsuna_scenario") {}

    void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceId) override
    {
        if (p_Player->GetMapId() == 1705)
        {
            p_Player->SetPhaseMask(1, true);
            p_Player->SetDisableGravity(false);
            p_Player->SetRooted(false);
            p_Player->TeleportTo(1220, Positions::ScenarioEndTeleportPos);
        }
    }

    void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceID) override
    {
        if (p_Player->GetMapId() == 1705)
        {
            p_Player->SetPhaseMask(1, true);
            p_Player->SetDisableGravity(false);
            p_Player->SetRooted(false);
            p_Player->TeleportTo(1220, Positions::ScenarioEndTeleportPos);
        }
    }
};


#ifndef __clang_analyzer__
void AddSC_scenario_azsuna_assault()
{
    new npc_azure_war_drake_119456();
    new npc_fel_imp_119633();
    new npc_azure_war_drake_119459();
    new spell_gen_fel_fire_ejector_controller();
    new spell_gen_fel_fire_ejection_damage();
    new spell_gen_fel_imps_gauntlet();
    new at_volatile();
    new playerscript_scene_leave_azsuna_scenario();
}
#endif