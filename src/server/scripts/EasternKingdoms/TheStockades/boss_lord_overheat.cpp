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
    Spell_Fireball          = 12466,
    Spell_Overheat          = 86633,
    Spell_Rain_Of_Fire      = 86636
};

enum eEvents
{
    Event_Fireball          = 0,
    Event_Overheat,
    Event_Rain_Of_Fire 
};

enum eTalks
{
    Talk_Aggro              = 0,     ///< ALL MUST BURN!
    Talk_Death                       ///< FIRE...EXTINGUISHED
};

class boss_lord_overheat : public CreatureScript
{
    public:
        boss_lord_overheat() : CreatureScript("boss_lord_overheat") { }

        struct boss_lord_overheatAI : public BossAI
        {
            boss_lord_overheatAI(Creature* p_Creature) : BossAI(p_Creature, eTheStockadesData::Data_Lord_Overheat)
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
                Talk(eTalks::Talk_Aggro);

                events.ScheduleEvent(eEvents::Event_Fireball, 3000);
                events.ScheduleEvent(eEvents::Event_Overheat, urand(4000, 5000));
                events.ScheduleEvent(eEvents::Event_Rain_Of_Fire, urand(6000, 8000));
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                Talk(eTalks::Talk_Death);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 10.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::Event_Fireball:
                    {
                        me->CastSpell(me->getVictim(), eSpells::Spell_Fireball);
                        events.ScheduleEvent(eEvents::Event_Fireball, urand(3000, 5000));
                        break;
                    }
                    case eEvents::Event_Overheat:
                    {
                        me->CastSpell(me->getVictim(), eSpells::Spell_Overheat);
                        events.ScheduleEvent(eEvents::Event_Overheat, urand(10000, 12000));
                        break;
                    }
                    case eEvents::Event_Rain_Of_Fire:
                    {
                        me->CastSpell(me->getVictim(), eSpells::Spell_Rain_Of_Fire);
                        events.ScheduleEvent(eEvents::Event_Rain_Of_Fire, urand(10000, 12000));
                        break;
                    }
                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_lord_overheatAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_lord_overheat()
{
    new boss_lord_overheat();
}
#endif