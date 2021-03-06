////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Boss_Firemaw
SD%Complete: 100
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

#include "blackwing_lair.h"

#define SPELL_SHADOWFLAME       22539
#define SPELL_WINGBUFFET        23339
#define SPELL_FLAMEBUFFET       23341

class boss_firemaw : public CreatureScript
{
public:
    boss_firemaw() : CreatureScript("boss_firemaw") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_firemawAI (creature);
    }

    struct boss_firemawAI : public ScriptedAI
    {
        boss_firemawAI(Creature* creature) : ScriptedAI(creature)
        {
            m_Instance = creature->GetInstanceScript();
        }

        uint32 ShadowFlame_Timer;
        uint32 WingBuffet_Timer;
        uint32 FlameBuffet_Timer;
        InstanceScript* m_Instance;

        void Reset() override
        {
            ShadowFlame_Timer = 30000;                          //These times are probably wrong
            WingBuffet_Timer = 24000;
            FlameBuffet_Timer = 5000;

            if (m_Instance)
                m_Instance->SetBossState(DATA_FIREMAW, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            DoZoneInCombat();

            if (m_Instance)
                m_Instance->SetBossState(DATA_FIREMAW, IN_PROGRESS);
        }

        void EnterEvadeMode() override
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_FIREMAW, FAIL);

            ScriptedAI::EnterEvadeMode();
        }

        void JustDied(Unit* p_Killer) override
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_FIREMAW, DONE);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //ShadowFlame_Timer
            if (ShadowFlame_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_SHADOWFLAME);
                ShadowFlame_Timer = urand(15000, 18000);
            } else ShadowFlame_Timer -= diff;

            //WingBuffet_Timer
            if (WingBuffet_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_WINGBUFFET);
                if (DoGetThreat(me->getVictim()))
                    DoModifyThreatPercent(me->getVictim(), -75);

                WingBuffet_Timer = 25000;
            } else WingBuffet_Timer -= diff;

            //FlameBuffet_Timer
            if (FlameBuffet_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_FLAMEBUFFET);
                FlameBuffet_Timer = 5000;
            } else FlameBuffet_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_boss_firemaw()
{
    new boss_firemaw();
}
#endif
