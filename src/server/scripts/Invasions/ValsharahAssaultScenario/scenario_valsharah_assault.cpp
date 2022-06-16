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
#include "instance_scenario_valsharah_assault.h"

/// 234811 - Tormenting Eyes
class spell_gen_tormenting_eyes : public SpellScriptLoader
{
public:
    spell_gen_tormenting_eyes() : SpellScriptLoader("spell_gen_tormenting_eyes") { }

    class spell_gen_tormenting_eyes_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_tormenting_eyes_SpellScript);

        enum eSpells
        {
            TormentingEyesSummon = 234810
        };

        void HandleAfterCast()
        {
            if (Unit* l_Caster = GetCaster())
                for (uint8 i = 0; i < 3; i++)
                    l_Caster->CastSpell(l_Caster, eSpells::TormentingEyesSummon, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_gen_tormenting_eyes_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_tormenting_eyes_SpellScript();
    }
};

/// 235083 - Fel Bombardment
class spell_gen_fel_bombardment : public SpellScriptLoader
{
public:
    spell_gen_fel_bombardment() : SpellScriptLoader("spell_gen_fel_bombardment") { }

    class spell_gen_fel_bombardment_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_fel_bombardment_SpellScript);

        enum eSpells
        {
            FelBombardmentMissiles = 235084
        };

        void HandleHitTarget(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target || !l_Caster->ToCreature() || !l_Target->ToPlayer())
                return;

            l_Caster->CastSpell(l_Target, eSpells::FelBombardmentMissiles, true);
            l_Caster->ToCreature()->AI()->Talk(0, l_Target->GetGUID());
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_fel_bombardment_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_fel_bombardment_SpellScript();
    }
};

/// 235048 - Dark Wave
class spell_gen_dark_wave : public SpellScriptLoader
{
public:
    spell_gen_dark_wave() : SpellScriptLoader("spell_gen_dark_wave") { }

    class spell_gen_dark_wave_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_dark_wave_SpellScript);

        enum eSpells
        {
            Spell_DarkWaveTriggerFirst = 235047,
            Spell_DarkWaveTriggerSecond = 235049,
            Spell_DarkWaveTriggerThird = 235050
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Position l_FirstTriggerSpawnPos = l_Caster->GetPosition();
            Position l_SecondTriggerSpawnPos = l_Caster->GetPosition();
            Position l_ThirdTriggerSpawnPos = l_Caster->GetPosition();
            l_FirstTriggerSpawnPos.m_orientation += 1.0f;
            l_ThirdTriggerSpawnPos.m_orientation -= 1.0f;

            l_Caster->CastSpell(l_FirstTriggerSpawnPos, eSpells::Spell_DarkWaveTriggerFirst, true);
            l_Caster->CastSpell(l_SecondTriggerSpawnPos, eSpells::Spell_DarkWaveTriggerFirst, true);
            l_Caster->CastSpell(l_ThirdTriggerSpawnPos, eSpells::Spell_DarkWaveTriggerFirst, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_gen_dark_wave_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_dark_wave_SpellScript();
    }
};

/// 234660 - Dread Beam
class spell_gen_dread_beam : public SpellScriptLoader
{
public:
    spell_gen_dread_beam() : SpellScriptLoader("spell_gen_dread_beam") { }

    class spell_gen_dread_beam_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_dread_beam_SpellScript);

        void HandleBeforeCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->ToCreature())
                return;

            l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);
            l_Caster->AttackStop();
            l_Caster->SetFacingTo(l_Caster->GetOrientation());
        }

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->ToCreature())
                return;

            l_Caster->ToCreature()->AI()->SetData(1, 1);
        }

        void Register() override
        {
            OnPrepare += SpellOnPrepareFn(spell_gen_dread_beam_SpellScript::HandleBeforeCast);
            AfterCast += SpellCastFn(spell_gen_dread_beam_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_dread_beam_SpellScript();
    }
};

/// 118687 - Hippogryph
class npc_hippogryph_118687 : public CreatureScript
{
public:
    npc_hippogryph_118687() : CreatureScript("npc_hippogryph_118687") { }

    struct npc_hippogryph_118687AI : public VehicleAI
    {
        npc_hippogryph_118687AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        ObjectGuid m_Clicker;
        EventMap m_Events;

        void OnSpellClick(Unit* p_Clicker) override
        {
            if (!p_Clicker->IsMounted())
            {
                m_Events.ScheduleEvent(c_events::EVENT_1, 2000);
                m_Clicker = p_Clicker->GetGUID();
                me->SetSpeed(UnitMoveType::MOVE_RUN, 5.0f, false);
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
            case ePoints::Point_Hippogryph_Tower:
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                break;
            case ePoints::Point_Hippogryph_Ship:
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_Clicker))
                {
                    if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_UpAndAway)
                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_UpAndAway, 0, 0, l_Player, l_Player);
                    l_Player->ExitVehicle();
                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Despawn, Waypoints::Path_HippogryphDespawn, 3);
                }
                break;
            }
            case ePoints::Point_Hippogryph_Despawn:
                me->DespawnOrUnsummon();
                break;
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
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Ship, Waypoints::Path_HippogryphShip, 9);
                break;
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_hippogryph_118687AI(p_Creature);
    }
};

/// 118697 - Hippogryph
class npc_hippogryph_118697 : public CreatureScript
{
public:
    npc_hippogryph_118697() : CreatureScript("npc_hippogryph_118697") { }

    struct npc_hippogryph_118697AI : public VehicleAI
    {
        npc_hippogryph_118697AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

        ObjectGuid m_SummonerGuid;
        EventMap m_Events;

        enum eSpells
        {
            RideVehicle = 52391,
            FadeToBlackToTransition = 235649
        };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            Map* l_Map = me->GetMap();
            if (!l_Map)
                return;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            Player* l_Player = p_Summoner->ToPlayer();
            if (!l_Scenario || !l_Player)
                return;

            if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_Escape)
                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_Escape, 0, 0, l_Player, l_Player);

            me->SetPhaseMask(2, true);
            m_SummonerGuid = p_Summoner->GetGUID();
            l_Player->SetDisableGravity(true);
            l_Player->SetPhaseMask(2, true);
            l_Player->PlayScene(1790, l_Player);
            l_Player->CastSpell(me, eSpells::RideVehicle, true);
            l_Player->DelayedCastSpell(p_Summoner, eSpells::FadeToBlackToTransition, true, 3000);
            m_Events.ScheduleEvent(c_events::EVENT_1, 500);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_Hippogryph_Escape)
                me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player || !l_Player->IsInWorld())
            {
                me->DespawnOrUnsummon();
                return;
            }

            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case c_events::EVENT_1:
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_Hippogryph_Escape, Waypoints::Path_HippogryphEscape, 9);
                m_Events.ScheduleEvent(c_events::EVENT_2, 6000);
                break;
            case c_events::EVENT_2:
                l_Player->SetDisableGravity(false);
                l_Player->SetPhaseMask(1, true);
                l_Player->TeleportTo(1220, Positions::ScenarioEndTeleportPos);
                break;
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_hippogryph_118697AI(p_Creature);
    }
};

class playerscript_falling_check : public PlayerScript
{
public:
    playerscript_falling_check() : PlayerScript("playerscript_falling_check") {}

    void OnUpdateMovement(Player* p_Player, uint16 /*p_OpcodeID*/) override
    {
        if (!p_Player)
            return;

        InstanceScript* l_Instance = p_Player->GetInstanceScript();
        Map* l_Map = p_Player->GetMap();
        if (!l_Instance || !l_Map || l_Map->GetId() != 1704)
            return;

        Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
        if (!l_Scenario)
            return;

        if (l_Scenario->GetCurrentStep() == eScenarioSteps::Step_Escape || l_Scenario->IsCompleted())
        {
            if (p_Player->GetPositionZ() < 245 && p_Player->GetPositionZ() > 220 &&
                !p_Player->HasAura(eSpells::SummonHippogryphTaxi) && p_Player->IsFalling())
                p_Player->CastSpell(p_Player, eSpells::SummonHippogryphTaxi, true);
        }
    }
};

class at_valsharah_assault_steps : public AreaTriggerEntityScript
{
public:
    at_valsharah_assault_steps() : AreaTriggerEntityScript("at_valsharah_assault_steps") { }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
    {
        InstanceScript* l_Instance = p_AreaTrigger->GetInstanceScript();
        Map* l_Map = p_AreaTrigger->GetMap();
        if (!l_Instance || !l_Map)
            return;

        Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
        if (!l_Scenario)
            return;

        switch (l_Scenario->GetCurrentStep())
        {
        case eScenarioSteps::Step_SecureAFoothold:
        {
            if (Player* l_Player = p_AreaTrigger->FindNearestPlayer(20.0f))
            {
                l_Instance->SetData(eData::Data_Summon_Crushfist, 0);
                p_AreaTrigger->SetDuration(0);
            }
            break;
        }
        case eScenarioSteps::Step_MovingOnUp:
        {
            if (Player* l_Player = p_AreaTrigger->FindNearestPlayer(5.0f))
            {
                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eScenarioAssets::Asset_MovingOnUp, 0, 0, l_Player, l_Player);
                p_AreaTrigger->SetDuration(0);
            }
            break;
        }
        default:
            break;
        }
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new at_valsharah_assault_steps();
    }
};

#ifndef __clang_analyzer__
void AddSC_scenario_valsharah_assault()
{
    new spell_gen_tormenting_eyes();
    new spell_gen_fel_bombardment();
    new spell_gen_dark_wave();
    new spell_gen_dread_beam();
    new npc_hippogryph_118687();
    new npc_hippogryph_118697();
    new playerscript_falling_check();
    new at_valsharah_assault_steps();
}
#endif
