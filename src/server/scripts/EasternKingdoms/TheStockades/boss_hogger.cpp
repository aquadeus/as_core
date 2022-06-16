////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "the_stockades.h"

enum eSpells
{
    Spell_Vicious_Slice     = 86604,
    Spell_Maddening_Call    = 86620,
    Spell_Enrage            = 86736
};

enum eEvents
{
    Event_Vicious_Slice      = 1,
    Event_Maddening_Call,
    Event_Enrage
};

enum eTalk
{
    Talk_Aggro      = 0,    ///< Forest just setback!
    Talk_Death,             ///< Yiiipe!
    Talk_Enrage             ///< Hogger enrages!
};

class boss_hogger : public CreatureScript
{
    public:
        boss_hogger() : CreatureScript("boss_hogger") { }

        struct boss_hoggerAI : public BossAI
        {
            boss_hoggerAI(Creature* p_Creature) : BossAI(p_Creature, eTheStockadesData::Data_Hogger)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                _Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();
                me->SetReactState(REACT_AGGRESSIVE);
                Talk(eTalk::Talk_Aggro);
                events.ScheduleEvent(eEvents::Event_Vicious_Slice, 3000);
                events.ScheduleEvent(eEvents::Event_Maddening_Call, urand(6000, 8000));
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage && p_Damage > 0)
                {
                    if (me->HealthBelowPct(30))
                        events.ScheduleEvent(eEvents::Event_Enrage, 1000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;


                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 15.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::Event_Vicious_Slice:
                    {
                        me->CastSpell(me->getVictim(), eSpells::Spell_Vicious_Slice);
                        events.ScheduleEvent(eEvents::Event_Vicious_Slice, urand(8000, 12000));
                        break;
                    }
                    case eEvents::Event_Maddening_Call:
                    {
                        me->CastSpell(me, eSpells::Spell_Maddening_Call);
                        events.ScheduleEvent(eEvents::Event_Maddening_Call, urand(6000, 8000));
                        break;
                    }
                    case eEvents::Event_Enrage:
                    {
                        Talk(eTalk::Talk_Enrage);
                        me->CastSpell(me, eSpells::Spell_Enrage);
                        break;
                    }
                    default:
                        break;
                }
                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalk::Talk_Death);
                _JustDied();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_hoggerAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_hogger()
{
    new boss_hogger();
}
#endif