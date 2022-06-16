////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "arcatraz.h"
enum Say
{
    // Dalliah the Doomsayer
    SAY_AGGRO                       = 1,
    SAY_SLAY                        = 2,
    SAY_WHIRLWIND                   = 3,
    SAY_HEAL                        = 4,
    SAY_DEATH                       = 5,
    SAY_SOCCOTHRATES_DEATH          = 7,

    // Wrath-Scryer Soccothrates
    SAY_AGGRO_DALLIAH_FIRST         = 0,
    SAY_DALLIAH_25_PERCENT          = 5
};

enum Spells
{
    SPELL_GIFT_OF_THE_DOOMSAYER     = 36173,
    SPELL_WHIRLWIND                 = 36142,
    SPELL_HEAL                      = 36144,
    SPELL_SHADOW_WAVE               = 39016
};

enum Events
{
    EVENT_GIFT_OF_THE_DOOMSAYER     = 1,
    EVENT_WHIRLWIND                 = 2,
    EVENT_HEAL                      = 3,
    EVENT_SHADOW_WAVE               = 4,
    EVENT_ME_FIRST                  = 5,
    EVENT_SOCCOTHRATES_DEATH        = 6
};

class boss_dalliah_the_doomsayer : public CreatureScript
{
    public:
        boss_dalliah_the_doomsayer() : CreatureScript("boss_dalliah_the_doomsayer") { }

        struct boss_dalliah_the_doomsayerAI : public BossAI
        {
            boss_dalliah_the_doomsayerAI(Creature* creature) : BossAI(creature, TYPE_DALLIAH)
            {
                m_SoccothratesTaunt = false;
                m_SoccothratesDeath = false;
            }

            void Reset() override
            {
                _Reset();
                m_SoccothratesTaunt = false;
                m_SoccothratesDeath = false;

                if (instance)
                    instance->SetData(TYPE_DALLIAH, NOT_STARTED);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);

                if (Creature* l_Soccothrates = Creature::GetCreature(*me, instance->GetData64(TYPE_SOCCOTHRATES)))
                    if (l_Soccothrates->isAlive() && !l_Soccothrates->isInCombat() && l_Soccothrates->IsAIEnabled)
                        l_Soccothrates->AI()->SetData(1, 1);

                if (instance)
                    instance->SetData(TYPE_DALLIAH, DONE);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                events.ScheduleEvent(EVENT_GIFT_OF_THE_DOOMSAYER, urand(1000, 4000));
                events.ScheduleEvent(EVENT_WHIRLWIND, urand(7000, 9000));
                events.ScheduleEvent(EVENT_ME_FIRST, 6000);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SHADOW_WAVE, urand(11000, 16000));

                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(TYPE_DALLIAH, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*p_Victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void SetData(uint32 /*p_Type*/, uint32 p_Data) override
            {
                switch (p_Data)
                {
                    case 1:
                        events.ScheduleEvent(EVENT_SOCCOTHRATES_DEATH, 6000);
                        m_SoccothratesDeath = true;
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                {
                    if (m_SoccothratesDeath)
                    {
                        events.Update(p_Diff);

                        switch (events.ExecuteEvent())
                        {
                            case EVENT_SOCCOTHRATES_DEATH:
                                Talk(SAY_SOCCOTHRATES_DEATH);
                                break;
                            default:
                                break;
                        }
                    }

                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_GIFT_OF_THE_DOOMSAYER:
                        DoCastVictim(SPELL_GIFT_OF_THE_DOOMSAYER, true);
                        events.ScheduleEvent(EVENT_GIFT_OF_THE_DOOMSAYER, urand(16000, 21000));
                        break;
                    case EVENT_WHIRLWIND:
                        DoCast(me, SPELL_WHIRLWIND);
                        Talk(SAY_WHIRLWIND);
                        events.ScheduleEvent(EVENT_WHIRLWIND, urand(19000, 21000));
                        events.ScheduleEvent(EVENT_HEAL, 6000);
                        break;
                    case EVENT_HEAL:
                        DoCast(me, SPELL_HEAL);
                        Talk(SAY_HEAL);
                        break;
                    case EVENT_SHADOW_WAVE:
                        DoCastVictim(SPELL_SHADOW_WAVE, true);
                        events.ScheduleEvent(EVENT_SHADOW_WAVE, urand(11000, 16000));
                        break;
                    case EVENT_ME_FIRST:
                        if (Creature* l_Soccothrates = Creature::GetCreature(*me, instance->GetData64(TYPE_SOCCOTHRATES)))
                            if (l_Soccothrates->isAlive() && !l_Soccothrates->isInCombat() && l_Soccothrates->IsAIEnabled)
                                l_Soccothrates->AI()->Talk(SAY_AGGRO_DALLIAH_FIRST);
                        break;
                    default:
                        break;
                }

                if (HealthBelowPct(25) && !m_SoccothratesTaunt)
                {
                    if (Creature* l_Soccothrates = Creature::GetCreature(*me, instance->GetData64(TYPE_SOCCOTHRATES)))
                    {
                        if (l_Soccothrates->IsAIEnabled)
                            l_Soccothrates->AI()->Talk(SAY_DALLIAH_25_PERCENT);
                    }
                    m_SoccothratesTaunt = true;
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool m_SoccothratesTaunt;
            bool m_SoccothratesDeath;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_dalliah_the_doomsayerAI(p_Creature);
        }
};

void AddSC_boss_dalliah_the_doomsayer()
{
    new boss_dalliah_the_doomsayer();
}