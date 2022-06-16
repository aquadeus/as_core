////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"
#include "Object.h"
#include "GameObject.h"

#include "Common.h"
#include "WaypointManager.h"
#include "ScriptedEscortAI.h"


/// Fierce Winds - 106432
class npc_fierce_winds : public CreatureScript
{
public:
    npc_fierce_winds() : CreatureScript("npc_fierce_winds") { }

    enum eSpells
    {   
        Eject = 50630,
        FierceWinds = 210975
    };

    enum ePoints
    {
        Point_endWinds = 1
    };

    enum eEvents
    {
        Event_castWinds = 1
    };

    struct npc_fierce_windsAI : public ScriptedAI
    {
        npc_fierce_windsAI(Creature *creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* p_Summoner) override
        {
            if (!p_Summoner)
                return;

            float x, y;
            GetPositionWithDistInOrientation(p_Summoner, 75, p_Summoner->GetOrientation(), x, y);
            Position const l_end_of_winds = {x, y, me->GetPositionZ()};
            
            me->GetMotionMaster()->MovePoint(ePoints::Point_endWinds, l_end_of_winds, false);
            events.ScheduleEvent(eEvents::Event_castWinds, 1000);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            events.Update(p_Diff);

            switch (events.ExecuteEvent())
            {
            case eEvents::Event_castWinds:
                me->CastSpell(me, eSpells::FierceWinds, true);
                events.ScheduleEvent(eEvents::Event_castWinds, 100);
                break;
            default:
                break;
            }
        }

        void MovementInform(uint32 p_Type, uint32 p_ID) override
        {
            switch (p_ID)
            {
            case ePoints::Point_endWinds:
            {
                me->CastSpell(me, eSpells::Eject, true);
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fierce_windsAI(creature);
    }
};

/// Quicksteel Arrow - 106230
class npc_quicksteel_arrow : public CreatureScript
{
public:
    npc_quicksteel_arrow() : CreatureScript("npc_quicksteel_arrow") { }

    enum eSpells
    {
        Spell_Stun = 210399
    };

    struct npc_quicksteel_arrowAI : public ScriptedAI
    {
        npc_quicksteel_arrowAI(Creature *creature) : ScriptedAI(creature) { }
        
        void OnSpellClick(Unit* p_Clicker) override
        {
            me->DespawnOrUnsummon();
        }

        void IsSummonedBy(Unit* /*p_Summon*/) override
        {
            Unit* l_target = me->SelectNearbyTarget();
            if (!l_target)
                return;

            me->CastSpell(l_target, eSpells::Spell_Stun, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_quicksteel_arrowAI(creature);
    }
};

/// Inna the Cryptstalker - 106245
class npc_inna_cryptstalker : public CreatureScript
{
public:
    npc_inna_cryptstalker() : CreatureScript("npc_inna_cryptstalker") { }

    enum eSpells
    {
        Spell_PinDown = 210396,
        Spell_Shoot = 210661,
        Spell_PinDownPlayer = 210700
    };

    enum eTalks
    {
        Talk_Aggro = 1,
        Talk_Pollute 
    };

    enum eEvents
    {
        Event_PinDown = 1,
        Event_Shoot = 3
    };

    enum eGobs
    {
        Gob_EntranceDoor = 249364
    };

    enum eNPC
    {
        Npc_QuicksteelArrow = 106230
    };

    struct npc_inna_cryptstalkerAI : public ScriptedAI
    {
        npc_inna_cryptstalkerAI(Creature *creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* /*p_Summon*/) override
        {
            Talk(eTalks::Talk_Aggro);
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            Talk(eTalks::Talk_Pollute);
            events.ScheduleEvent(eEvents::Event_Shoot, 1000);
            events.ScheduleEvent(eEvents::Event_PinDown, 13000);
        }

        void Reset() override
        {
            events.Reset();

            std::list<Creature*> l_Arrows;
            GetCreatureListWithEntryInGrid(l_Arrows, me, eNPC::Npc_QuicksteelArrow, 30.0f);
            for (Creature* l_Creature : l_Arrows)
                l_Creature->DespawnOrUnsummon();

            std::list<Player*> l_PlayerList;
            me->GetPlayerListInGrid(l_PlayerList, 200.0f);
            for (Player* l_Player : l_PlayerList)
                l_Player->RemoveAurasDueToSpell(eSpells::Spell_PinDownPlayer);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent()) {

                switch (eventId)
                {
                case eEvents::Event_PinDown:
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(target, eSpells::Spell_PinDown);

                    events.ScheduleEvent(eEvents::Event_PinDown, 13000);
                    break;
                }
                case eEvents::Event_Shoot:
                {
                    DoCastVictim(eSpells::Spell_Shoot, false);
                    events.ScheduleEvent(eEvents::Event_Shoot, 2500);
                    break;
                }
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/) override
        {
            std::list<Creature*> l_Arrows;
            GetCreatureListWithEntryInGrid(l_Arrows, me, eNPC::Npc_QuicksteelArrow, 30.0f);
            for (Creature* l_Creature : l_Arrows)
                l_Creature->DespawnOrUnsummon();

            std::list<GameObject*> l_ObjectList;
            me->GetGameObjectListWithEntryInGrid(l_ObjectList, eGobs::Gob_EntranceDoor, 200.0f);
            for (GameObject* l_Obj : l_ObjectList)
                l_Obj->SetGoState(GO_STATE_ACTIVE);
            
            std::list<Player*> l_PlayerList;
            me->GetPlayerListInGrid(l_PlayerList, 200.0f);
            for (Player* l_Player : l_PlayerList)
                l_Player->RemoveAurasDueToSpell(eSpells::Spell_PinDownPlayer);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_inna_cryptstalkerAI(creature);
    }
};

///< 7.3.5
///< Called by Blocking -  210223
class spell_gen_blocking : public SpellScriptLoader
{
public:
    spell_gen_blocking() : SpellScriptLoader("spell_gen_blocking") { }

    class spell_gen_blocking_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_blocking_SpellScript);

        enum eNpcs
        {
            Bunny = 106094
        };

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || p_Targets.empty())
                return;

            p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
            {
                Unit* l_Target = p_Target->ToUnit();
                if (!l_Target)
                    return true;

                if (l_Target != l_Caster && l_Target->GetEntry() != eNpcs::Bunny)
                    return true;

                return false;

            });
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_blocking_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_blocking_SpellScript();
    }
};

///< Pin Down - 210704
class spell_pin_down : public SpellScriptLoader
{
public:
    spell_pin_down() : SpellScriptLoader("spell_pin_down") { }

    class spell_pin_down_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pin_down_SpellScript);

        enum eNpcs
        {
            Npc_Inna = 106245,
            Npc_Shae = 106246
        };

        enum eSpells
        {
            Spell_PinDownPlayer = 210700
        };

        SpellCastResult HandleCheckCast()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetExplTargetUnit();

            if (!l_Caster || !l_Target)
                return SpellCastResult::SPELL_FAILED_DONT_REPORT;
            
            if (!l_Target->ToCreature())
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
            
            uint32 l_Entry = l_Target->ToCreature()->GetEntry();
            if (l_Entry != eNpcs::Npc_Inna && l_Entry != eNpcs::Npc_Shae)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
            
            return SpellCastResult::SPELL_CAST_OK;
        }

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster)
                l_Caster->RemoveAurasDueToSpell(eSpells::Spell_PinDownPlayer);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_pin_down_SpellScript::HandleAfterCast);
            OnCheckCast += SpellCheckCastFn(spell_pin_down_SpellScript::HandleCheckCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pin_down_SpellScript();
    }
};

/// Paladin's artifact AT 6840
/// Summoned by 210975 - npc 106432
class at_fierce_winds : public AreaTriggerEntityScript
{
public:
    at_fierce_winds() : AreaTriggerEntityScript("at_fierce_winds") { }

    enum eSpells
    {
        Ride = 232721
    };

    bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
    {
        Unit *l_Caster = p_Areatrigger->GetCaster();

        if (!l_Caster || l_Caster == p_Target || !p_Target->IsPlayer())
            return false;

        p_Target->CastSpell(l_Caster, eSpells::Ride, true);

        return false;
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new at_fierce_winds();
    }
};

#ifndef __clang_analyzer__
void AddSC_paladin_artifact_questline()
{
    new npc_fierce_winds();
    new npc_quicksteel_arrow();
    new npc_inna_cryptstalker();

    new spell_gen_blocking();
    new spell_pin_down;

    new at_fierce_winds();
}
#endif
