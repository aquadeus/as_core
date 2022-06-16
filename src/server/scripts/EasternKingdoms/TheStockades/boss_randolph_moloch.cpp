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
    Spell_Sweep             = 86729,
    Spell_Vanish            = 55964,
    Spell_Wildly_Stabbing   = 86726
};

enum eTalks
{
    Talk_Aggro              = 0,    ///< Allow me to introduce myself. I am Randolph Moloch and I will be killing you all today.
    Talk_Vanish,                    ///< Randolph Moloch vanishes!
    Talk_Death                      ///< My epic schemes,my great plans! Gone!
};

enum eEvents
{
    Event_Sweep             = 1,
    Event_Vanish,
    Event_Wildly_Stabbing
};

class boss_randolph_moloch : public CreatureScript
{
    public:
        boss_randolph_moloch() : CreatureScript("boss_randolph_moloch") { }

        struct boss_randolph_molochAI : public BossAI
        {
            boss_randolph_molochAI(Creature* p_Creature) : BossAI(p_Creature, eTheStockadesData::Data_Randolph_Moloch)
            {
                me->SetReactState(REACT_PASSIVE);

                m_Vanished_1 = false;
                m_Vanished_2 = false;
            }

            bool m_Vanished_1;
            bool m_Vanished_2;

            void Reset() override
            {
                if (!me->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    _Reset();

                m_Vanished_1 = false;
                m_Vanished_2 = false;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();
                me->SetReactState(REACT_AGGRESSIVE);
                Talk(eTalks::Talk_Aggro);
                events.ScheduleEvent(eEvents::Event_Wildly_Stabbing, urand(3000, 5000));
                events.ScheduleEvent(eEvents::Event_Sweep, urand(8000, 9000));
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage && p_Damage > 0)
                {
                    if (me->HealthBelowPct(50) && me->HealthAbovePct(30))
                    {
                        if (!m_Vanished_1)
                        {
                            m_Vanished_1 = true;
                            events.ScheduleEvent(eEvents::Event_Vanish, 100);
                        }
                    }

                    if (me->HealthBelowPct(30))
                    {
                        if (!m_Vanished_2)
                        {
                            m_Vanished_2 = true;
                            events.ScheduleEvent(eEvents::Event_Vanish, 100);
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() && !me->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 12.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::Event_Sweep:
                    {
                        me->CastSpell(me->getVictim(), eSpells::Spell_Sweep);
                        events.ScheduleEvent(eEvents::Event_Sweep, urand(8000, 9000));
                        break;
                    }
                    case eEvents::Event_Wildly_Stabbing:
                    {
                        me->CastSpell(me->getVictim(), eSpells::Spell_Wildly_Stabbing);
                        events.ScheduleEvent(eEvents::Event_Wildly_Stabbing, urand(11000, 14000));
                        break;
                    }
                    case eEvents::Event_Vanish:
                    {
                        Talk(eTalks::Talk_Vanish);
                        me->CastSpell(me, eSpells::Spell_Vanish);
                        break;
                    }
                    DoMeleeAttackIfReady();
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Talk_Death);
                _JustDied();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_randolph_molochAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_randolph_moloch()
{
    new boss_randolph_moloch();
}
#endif