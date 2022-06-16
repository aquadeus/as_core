////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

enum Says
{
    SAY_AGGRO           = 0,
    SAY_DEATH           = 7,
};

enum Spells
{
    SPELL_VENOM_SPRAY       = 202414,
    SPELL_WEB_GRAB          = 202462,
    SPELL_FEL_DETONATION    = 202473,
    SPELL_TOXIC_BLOOD_TICK  = 210504,

    SPELL_BONUS_LOOT        = 226662
};

enum eEvents
{
    EVENT_VENOM_SPRAY = 1,
    EVENT_WEB_GRAB,
    EVENT_FEL_DETONATION,
    EVENT_PRE_FIGHT,
    EVENT_TOXIC_BLOOD
};

/// Sael'orn - 102387
class boss_saelorn : public CreatureScript
{
    public:
        boss_saelorn() : CreatureScript("boss_saelorn") { }

        struct boss_saelornAI : public BossAI
        {
            boss_saelornAI(Creature* creature) : BossAI(creature, DATA_SAELORN)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                me->SetVisible(false);
                removeloot = false;
            }

            bool readyFight;
            bool removeloot;
            uint32 timer;
            uint32 text;

            void Reset() override
            {
                _Reset();

                if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                    me->SetReactState(REACT_DEFENSIVE);
                else
                    readyFight = false;

                timer = 13000;
                text = 1;
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();

                events.ScheduleEvent(EVENT_VENOM_SPRAY, 3000);
                events.ScheduleEvent(EVENT_WEB_GRAB, 14000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_TOXIC_BLOOD, 11000);
            }

            void JustDied(Unit* killer) override
            {
                Talk(SAY_DEATH);
                _JustDied();

                if (removeloot)
                    me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                CastSpellToPlayers(me, SPELL_BONUS_LOOT, true);

                /// We have to handle the dungeon reward by script for Sael'orn because the end boss can be Sael'orn OR Betrug
                /// Our generic system assume the last boss of a dungeon is the dungeonencounter entry of the dungeon with the biggest orderIndex
                Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                if (l_PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                {
                    if (Player* l_Player = l_Itr->getSource())
                    {
                        uint32 l_DungeonID = l_Player->GetGroup() ? sLFGMgr->GetDungeon(l_Player->GetGroup()->GetGUID()) : 0;
                        if (l_Player->IsAtGroupRewardDistance(me))
                            sLFGMgr->RewardDungeonDoneFor(l_DungeonID, l_Player);
                    }
                }
            }

            void DoAction(int32 const action) override
            {
                if (instance->GetData(DATA_MAIN_EVENT_PHASE) != IN_PROGRESS)
                    return;

                if (action == 1)
                {
                    me->SetVisible(true);
                    me->GetMotionMaster()->MoveJump(saboFightPos.GetPositionX(), saboFightPos.GetPositionY(), saboFightPos.GetPositionZ(), 20.0f, 20.0f, 10);
                    me->SetHomePosition(saboFightPos);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                }
                else if (action == ACTION_REMOVE_LOOT)
                    removeloot = true;
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type != EFFECT_MOTION_TYPE)
                    return;

                if (id == 10)
                    events.ScheduleEvent(EVENT_PRE_FIGHT, 1000);
            }

            void UpdateAI(uint32 const diff) override
            {
                if (!UpdateVictim() && !readyFight)
                    return;

                if (timer <= diff)
                {
                    if(text < 7)
                    {
                        Talk(text);
                        text++;
                        timer = 13000;
                    }
                } else timer -= diff;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_PRE_FIGHT:
                    {
                        readyFight = true;
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    }
                    case EVENT_VENOM_SPRAY:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            DoCast(target, SPELL_VENOM_SPRAY);
                        events.ScheduleEvent(EVENT_VENOM_SPRAY, 27000);
                        break;
                    }
                    case EVENT_WEB_GRAB:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 50.0f, true))
                            DoCast(target, SPELL_WEB_GRAB);
                        events.ScheduleEvent(EVENT_WEB_GRAB, 27000);
                        events.ScheduleEvent(EVENT_FEL_DETONATION, 500);
                        break;
                    }
                    case EVENT_FEL_DETONATION:
                    {
                        DoCast(SPELL_FEL_DETONATION);
                        break;
                    }
                    case EVENT_TOXIC_BLOOD:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            me->CastSpell(l_Target, SPELL_TOXIC_BLOOD_TICK, true);

                        events.ScheduleEvent(EVENT_TOXIC_BLOOD, 30000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_saelornAI (creature);
        }
};

/// Toxic Blood (Periodic) - 210504
class spell_saelorn_toxic_blood : public SpellScriptLoader
{
    public:
        spell_saelorn_toxic_blood() : SpellScriptLoader("spell_saelorn_toxic_blood") { }

        class spell_saelorn_toxic_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_saelorn_toxic_blood_AuraScript);

            void OnPeriodic(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(l_Target, p_AurEff->GetAmount(), true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_saelorn_toxic_blood_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_saelorn_toxic_blood_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_saelorn()
{
    /// Boss
    new boss_saelorn();

    /// Spell
    new spell_saelorn_toxic_blood();
}
#endif
