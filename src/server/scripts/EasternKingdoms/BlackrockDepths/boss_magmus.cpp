////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "blackrock_depths.h"

enum Spells
{
    SPELL_FIERYBURST                                       = 13900,
    SPELL_WARSTOMP                                         = 24375
};

class boss_magmus : public CreatureScript
{
public:
    boss_magmus() : CreatureScript("boss_magmus") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_magmusAI (creature);
    }

    struct boss_magmusAI : public ScriptedAI
    {
        boss_magmusAI(Creature* creature) : ScriptedAI(creature)
        {
            m_Instance = creature->GetInstanceScript();
        }

        uint32 FieryBurst_Timer;
        uint32 WarStomp_Timer;
        InstanceScript* m_Instance;

        void Reset() override
        {
            FieryBurst_Timer = 5000;
            WarStomp_Timer = 0;

            if (m_Instance)
                m_Instance->SetBossState(DATA_MAGMUS, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_MAGMUS, IN_PROGRESS);
        }

        void EnterEvadeMode() override
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_MAGMUS, FAIL);

            ScriptedAI::EnterEvadeMode();
        }

        void UpdateAI(const uint32 diff) override
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            //FieryBurst_Timer
            if (FieryBurst_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_FIERYBURST);
                FieryBurst_Timer = 6000;
            } else FieryBurst_Timer -= diff;

            //WarStomp_Timer
            if (HealthBelowPct(51))
            {
                if (WarStomp_Timer <= diff)
                {
                    DoCast(me->getVictim(), SPELL_WARSTOMP);
                    WarStomp_Timer = 8000;
                } else WarStomp_Timer -= diff;
            }

            DoMeleeAttackIfReady();
        }
        // When he die open door to last chamber
        void JustDied(Unit* killer) override
        {
            if (m_Instance)
            {
                m_Instance->HandleGameObject(m_Instance->GetData64(DATA_THRONE_DOOR), true);
                m_Instance->SetBossState(DATA_MAGMUS, DONE);
            }
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_boss_magmus()
{
    new boss_magmus();
}
#endif
