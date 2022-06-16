////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Boss_Princess_Theradras
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "maraudon.h"

enum Spells
{
    SPELL_DUSTFIELD             = 21909,
    SPELL_BOULDER               = 21832,
    SPELL_THRASH                = 3391,
    SPELL_REPULSIVEGAZE         = 21869
};

class boss_princess_theradras : public CreatureScript
{
public:
    boss_princess_theradras() : CreatureScript("boss_princess_theradras") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ptheradrasAI (creature);
    }

    struct boss_ptheradrasAI : public ScriptedAI
    {
        boss_ptheradrasAI(Creature* creature) : ScriptedAI(creature)
        {
            m_Instance = creature->GetInstanceScript();
        }

        uint32 DustfieldTimer;
        uint32 BoulderTimer;
        uint32 ThrashTimer;
        uint32 RepulsiveGazeTimer;

        InstanceScript* m_Instance;

        void Reset()
        {
            DustfieldTimer = 8000;
            BoulderTimer = 2000;
            ThrashTimer = 5000;
            RepulsiveGazeTimer = 23000;

            if (m_Instance)
                m_Instance->SetBossState(DATA_THERADRAS, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_THERADRAS, IN_PROGRESS);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->SummonCreature(12238, 28.067f, 61.875f, -123.405f, 4.67f, TEMPSUMMON_TIMED_DESPAWN, 600000);

            if (m_Instance)
                m_Instance->SetBossState(DATA_THERADRAS, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            //DustfieldTimer
            if (DustfieldTimer <= diff)
            {
                DoCast(me, SPELL_DUSTFIELD);
                DustfieldTimer = 14000;
            }
            else DustfieldTimer -= diff;

            //BoulderTimer
            if (BoulderTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_BOULDER);
                BoulderTimer = 10000;
            }
            else BoulderTimer -= diff;

            //RepulsiveGazeTimer
            if (RepulsiveGazeTimer <= diff)
            {
                DoCastVictim(SPELL_REPULSIVEGAZE);
                RepulsiveGazeTimer = 20000;
            }
            else RepulsiveGazeTimer -= diff;

            //ThrashTimer
            if (ThrashTimer <= diff)
            {
                DoCast(me, SPELL_THRASH);
                ThrashTimer = 18000;
            }
            else ThrashTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_boss_ptheradras()
{
    new boss_princess_theradras();
}
#endif
