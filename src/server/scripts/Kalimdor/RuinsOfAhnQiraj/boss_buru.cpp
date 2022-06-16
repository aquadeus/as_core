////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Boss_Buru
SD%Complete: 0
SDComment: Place Holder
SDCategory: Ruins of Ahn'Qiraj
EndScriptData*/

#include "ruins_of_ahnqiraj.h"

enum Yells
{
    EMOTE_TARGET               = -1509002
};

class boss_buru : public CreatureScript
{
    public:
        boss_buru() : CreatureScript("boss_buru") { }

        struct boss_buruAI : public ScriptedAI
        {
            boss_buruAI(Creature* creature) : ScriptedAI(creature)
            {
                m_Instance = creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                if (m_Instance)
                    m_Instance->SetBossState(BOSS_BURU, NOT_STARTED);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                if (m_Instance)
                    m_Instance->SetBossState(BOSS_BURU, IN_PROGRESS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance)
                    m_Instance->SetBossState(BOSS_BURU, DONE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_buruAI(creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_buru()
{
    new boss_buru();
}
#endif
