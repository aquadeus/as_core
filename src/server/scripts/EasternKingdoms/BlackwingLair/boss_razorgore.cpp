////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Boss_Razorgore
SD%Complete: 50
SDComment: Needs additional review. Phase 1 NYI (Grethok the Controller)
SDCategory: Blackwing Lair
EndScriptData */

#include "blackwing_lair.h"

//Razorgore Phase 2 Script

enum Say
{
    SAY_EGGS_BROKEN1        = -1469022,
    SAY_EGGS_BROKEN2        = -1469023,
    SAY_EGGS_BROKEN3        = -1469024,
    SAY_DEATH               = -1469025
};

enum Spells
{
    SPELL_CLEAVE            = 22540,
    SPELL_WARSTOMP          = 24375,
    SPELL_FIREBALLVOLLEY    = 22425,
    SPELL_CONFLAGRATION     = 23023
};

class boss_razorgore : public CreatureScript
{
public:
    boss_razorgore() : CreatureScript("boss_razorgore") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_razorgoreAI (creature);
    }

    struct boss_razorgoreAI : public ScriptedAI
    {
        boss_razorgoreAI(Creature* creature) : ScriptedAI(creature)
        {
            m_Instance = creature->GetInstanceScript();
        }

        uint32 Cleave_Timer;
        uint32 WarStomp_Timer;
        uint32 FireballVolley_Timer;
        uint32 Conflagration_Timer;

        InstanceScript* m_Instance;

        void Reset() override
        {
            Cleave_Timer = 15000;                               //These times are probably wrong
            WarStomp_Timer = 35000;
            FireballVolley_Timer = 7000;
            Conflagration_Timer = 12000;

            if (m_Instance)
                m_Instance->SetBossState(DATA_RAZORGORE, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            DoZoneInCombat();

            if (m_Instance)
                m_Instance->SetBossState(DATA_RAZORGORE, IN_PROGRESS);
        }

        void EnterEvadeMode() override
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_RAZORGORE, FAIL);

            ScriptedAI::EnterEvadeMode();
        }

        void JustDied(Unit* /*killer*/) override
        {
            DoScriptText(SAY_DEATH, me);

            if (m_Instance)
                m_Instance->SetBossState(DATA_RAZORGORE, DONE);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //Cleave_Timer
            if (Cleave_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_CLEAVE);
                Cleave_Timer = urand(7000, 10000);
            } else Cleave_Timer -= diff;

            //WarStomp_Timer
            if (WarStomp_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_WARSTOMP);
                WarStomp_Timer = urand(15000, 25000);
            } else WarStomp_Timer -= diff;

            //FireballVolley_Timer
            if (FireballVolley_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_FIREBALLVOLLEY);
                FireballVolley_Timer = urand(12000, 15000);
            } else FireballVolley_Timer -= diff;

            //Conflagration_Timer
            if (Conflagration_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_CONFLAGRATION);
                //We will remove this threat reduction and add an aura check.

                //if (DoGetThreat(me->getVictim()))
                //DoModifyThreatPercent(me->getVictim(), -50);

                Conflagration_Timer = 12000;
            } else Conflagration_Timer -= diff;

            // Aura Check. If the gamer is affected by confliguration we attack a random gamer.
            if (me->getVictim() && me->getVictim()->HasAura(SPELL_CONFLAGRATION))
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true))
                    me->TauntApply(target);

            DoMeleeAttackIfReady();
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_boss_razorgore()
{
    new boss_razorgore();
}
#endif
