////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Boss_Flamegor
SD%Complete: 100
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

#include "blackwing_lair.h"

enum Emotes
{
    EMOTE_FRENZY            = -1469031
};

enum Spells
{
    SPELL_SHADOWFLAME        = 22539,
    SPELL_WINGBUFFET         = 23339,
    SPELL_FRENZY             = 23342                      //This spell periodically triggers fire nova
};

class boss_flamegor : public CreatureScript
{
public:
    boss_flamegor() : CreatureScript("boss_flamegor") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_flamegorAI (creature);
    }

    struct boss_flamegorAI : public ScriptedAI
    {
        boss_flamegorAI(Creature* creature) : ScriptedAI(creature)
        {
            m_Instance = creature->GetInstanceScript();
        }

        uint32 ShadowFlame_Timer;
        uint32 WingBuffet_Timer;
        uint32 Frenzy_Timer;
        InstanceScript* m_Instance;

        void Reset() override
        {
            ShadowFlame_Timer = 21000;                          //These times are probably wrong
            WingBuffet_Timer = 35000;
            Frenzy_Timer = 10000;

            if (m_Instance)
                m_Instance->SetBossState(DATA_FLAMEGOR, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            DoZoneInCombat();

            if (m_Instance)
                m_Instance->SetBossState(DATA_FLAMEGOR, IN_PROGRESS);
        }

        void EnterEvadeMode() override
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_FLAMEGOR, FAIL);

            ScriptedAI::EnterEvadeMode();
        }

        void JustDied(Unit* p_Killer) override
        {
            if (m_Instance)
                m_Instance->SetBossState(DATA_FLAMEGOR, DONE);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //ShadowFlame_Timer
            if (ShadowFlame_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_SHADOWFLAME);
                ShadowFlame_Timer = urand(15000, 22000);
            } else ShadowFlame_Timer -= diff;

            //WingBuffet_Timer
            if (WingBuffet_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_WINGBUFFET);
                if (DoGetThreat(me->getVictim()))
                    DoModifyThreatPercent(me->getVictim(), -75);

                WingBuffet_Timer = 25000;
            } else WingBuffet_Timer -= diff;

            //Frenzy_Timer
            if (Frenzy_Timer <= diff)
            {
                DoScriptText(EMOTE_FRENZY, me);
                DoCast(me, SPELL_FRENZY);
                Frenzy_Timer = urand(8000, 10000);
            } else Frenzy_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_boss_flamegor()
{
    new boss_flamegor();
}
#endif
