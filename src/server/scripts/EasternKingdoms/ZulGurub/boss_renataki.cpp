////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "zulgurub.h"

enum Yells
{
};

enum Spells
{
};

enum Events
{
};

class boss_renataki : public CreatureScript
{
    public:
        boss_renataki() : CreatureScript("boss_renataki") { }

        struct boss_renatakiAI : public BossAI
        {
            boss_renatakiAI(Creature* creature) : BossAI(creature, DATA_BOSS_MADNESS)
            {
            }

            void Reset()
            {
                _Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                /*
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                */

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_renatakiAI(creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_renataki()
{
    new boss_renataki();
}
#endif
