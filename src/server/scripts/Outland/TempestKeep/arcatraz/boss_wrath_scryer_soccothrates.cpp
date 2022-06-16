////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: boss_wrath_scryer_soccothrates
SD%Complete: 95%
SDComment: charge left to script
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

#include "arcatraz.h"

enum Say
{
    // Wrath-Scryer Soccothrates
    SAY_AGGRO                       = 1,
    SAY_SLAY                        = 2,
    SAY_KNOCK_AWAY                  = 3,
    SAY_DEATH                       = 4,
    SAY_DALLIAH_DEATH               = 6,
    SAY_SOCCOTHRATES_CONVO_1        = 7,
    SAY_SOCCOTHRATES_CONVO_2        = 8,
    SAY_SOCCOTHRATES_CONVO_3        = 9,
    SAY_SOCCOTHRATES_CONVO_4        = 10,

    // l_Dalliah the Doomsayer
    SAY_AGGRO_SOCCOTHRATES_FIRST    = 0,
    SAY_SOCCOTHRATES_25_PERCENT     = 6,
    SAY_DALLIAH_CONVO_1             = 8,
    SAY_DALLIAH_CONVO_2             = 9,
    SAY_DALLIAH_CONVO_3             = 10
};

enum Spells
{
    SPELL_FEL_IMMOLATION            = 36051,
    SPELL_FELFIRE_SHOCK             = 35759,
    SPELL_KNOCK_AWAY                = 36512,
    SPELL_FELFIRE_LINE_UP           = 35770,
    SPELL_CHARGE_TARGETING          = 36038,
    SPELL_CHARGE                    = 35754
};

enum Events
{
    EVENT_FELFIRE_SHOCK             = 1,
    EVENT_KNOCK_AWAY                = 2,

    EVENT_PREFIGHT_1                = 3,
    EVENT_PREFIGHT_2                = 4,
    EVENT_PREFIGHT_3                = 5,
    EVENT_PREFIGHT_4                = 6,
    EVENT_PREFIGHT_5                = 7,
    EVENT_PREFIGHT_6                = 8,
    EVENT_PREFIGHT_7                = 9,
    EVENT_PREFIGHT_8                = 10,
    EVENT_PREFIGHT_9                = 11,
    EVENT_ME_FIRST                  = 12,
    EVENT_DALLIAH_DEATH             = 13
};

class boss_wrath_scryer_soccothrates : public CreatureScript
{
    public:
        boss_wrath_scryer_soccothrates() : CreatureScript("boss_wrath_scryer_soccothrates") { }

        struct boss_wrath_scryer_soccothratesAI : public BossAI
        {
            boss_wrath_scryer_soccothratesAI(Creature* creature) : BossAI(creature, TYPE_SOCCOTHRATES)
            {
                m_PreFight = false;
                m_DalliahTaunt = false;
                m_DalliahDeath = false;
            }

            void Reset() override
            {
                _Reset();
                m_PreFight = false;
                m_DalliahTaunt = false;
                m_DalliahDeath = false;
                DoCast(me, SPELL_FEL_IMMOLATION);

                if (instance)
                    instance->SetData(TYPE_DALLIAH, NOT_STARTED);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);

                if (Creature* l_Dalliah =Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                    if (l_Dalliah->isAlive() && !l_Dalliah->isInCombat() && l_Dalliah->IsAIEnabled)
                        l_Dalliah->AI()->SetData(1, 1);

                if (instance)
                    instance->SetData(TYPE_SOCCOTHRATES, DONE);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
                events.ScheduleEvent(EVENT_FELFIRE_SHOCK, urand(12000, 14000));
                events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(11000, 12000));
                events.ScheduleEvent(EVENT_ME_FIRST, 6000);
                Talk(SAY_AGGRO);
                m_PreFight = false;

                if (instance)
                    instance->SetData(TYPE_DALLIAH, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*p_Victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who && instance->GetData(DATA_CONVERSATION) == NOT_STARTED && p_Who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(p_Who, 70.0f))
                {
                    Talk(SAY_SOCCOTHRATES_CONVO_1);
                    instance->SetData(DATA_CONVERSATION, DONE);

                    m_PreFight = true;
                    events.ScheduleEvent(EVENT_PREFIGHT_1, 2000);
                }
            }

            void SetData(uint32 /*p_Type*/, uint32 p_Data) override
            {
                switch (p_Data)
                {
                    case 1:
                        events.ScheduleEvent(EVENT_DALLIAH_DEATH, 6000);
                        m_DalliahDeath = true;
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
                    if (m_PreFight)
                    {
                        events.Update(p_Diff);

                        switch (events.ExecuteEvent())
                        {
                            case EVENT_PREFIGHT_1:
                                if (Creature* l_Dalliah = Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                                {
                                    if (l_Dalliah->IsAIEnabled)
                                        l_Dalliah->AI()->Talk(SAY_DALLIAH_CONVO_1);
                                }
                                events.ScheduleEvent(EVENT_PREFIGHT_2, 3000);
                                break;
                            case EVENT_PREFIGHT_2:
                                Talk(SAY_SOCCOTHRATES_CONVO_2);
                                events.ScheduleEvent(EVENT_PREFIGHT_3, 3000);
                                break;
                            case EVENT_PREFIGHT_3:
                                if (Creature* l_Dalliah = Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                                {
                                    if (l_Dalliah->IsAIEnabled)
                                        l_Dalliah->AI()->Talk(SAY_DALLIAH_CONVO_2);
                                }
                                events.ScheduleEvent(EVENT_PREFIGHT_4, 6000);
                                break;
                            case EVENT_PREFIGHT_4:
                                Talk(SAY_SOCCOTHRATES_CONVO_3);
                                events.ScheduleEvent(EVENT_PREFIGHT_5, 2000);
                                break;
                            case EVENT_PREFIGHT_5:
                                if (Creature* l_Dalliah = Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                                {
                                    if (l_Dalliah->IsAIEnabled)
                                        l_Dalliah->AI()->Talk(SAY_DALLIAH_CONVO_3);
                                }
                                events.ScheduleEvent(EVENT_PREFIGHT_6, 3000);
                                break;
                            case EVENT_PREFIGHT_6:
                                Talk(SAY_SOCCOTHRATES_CONVO_4);
                                events.ScheduleEvent(EVENT_PREFIGHT_7, 2000);
                                break;
                            case EVENT_PREFIGHT_7:
                                if (Creature* l_Dalliah = Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                                    l_Dalliah->GetMotionMaster()->MovePoint(0, 118.6048f, 96.84852f, 22.44115f);
                                events.ScheduleEvent(EVENT_PREFIGHT_8, 4000);
                                break;
                            case EVENT_PREFIGHT_8:
                                me->GetMotionMaster()->MovePoint(0, 122.1035f, 192.7203f, 22.44115f);
                                events.ScheduleEvent(EVENT_PREFIGHT_9, 4000);
                                break;
                            case EVENT_PREFIGHT_9:
                                if (Creature* l_Dalliah = Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                                {
                                    l_Dalliah->SetFacingToObject(me);
                                    me->SetFacingToObject(l_Dalliah);
                                    l_Dalliah->SetHomePosition(l_Dalliah->GetPositionX(), l_Dalliah->GetPositionY(), l_Dalliah->GetPositionZ(), 1.51737f);
                                    me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 4.725722f);
                                    m_PreFight = false;
                                }
                                break;
                            default:
                                break;
                        }
                    }

                    if (m_DalliahDeath)
                    {
                        events.Update(p_Diff);

                        switch (events.ExecuteEvent())
                        {
                            case EVENT_DALLIAH_DEATH:
                                Talk(SAY_DALLIAH_DEATH);
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
                    case EVENT_FELFIRE_SHOCK:
                        DoCastVictim(SPELL_FELFIRE_SHOCK, true);
                        events.ScheduleEvent(EVENT_FELFIRE_SHOCK, urand(12000, 14000));
                        break;
                    case EVENT_KNOCK_AWAY:
                        DoCast(me, SPELL_KNOCK_AWAY);
                        Talk(SAY_KNOCK_AWAY);
                        events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(11000, 12000));
                        break;
                    case EVENT_ME_FIRST:
                        if (Creature* l_Dalliah = Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                            if (l_Dalliah->isAlive() && !l_Dalliah->isInCombat() && l_Dalliah->IsAIEnabled)
                                l_Dalliah->AI()->Talk(SAY_AGGRO_SOCCOTHRATES_FIRST);
                        break;
                    default:
                        break;
                }

                if (HealthBelowPct(25) && !m_DalliahTaunt)
                {
                    if (Creature* l_Dalliah = Creature::GetCreature(*me, instance->GetData64(TYPE_DALLIAH)))
                    {
                        if (l_Dalliah->IsAIEnabled)
                            l_Dalliah->AI()->Talk(SAY_SOCCOTHRATES_25_PERCENT);
                    }
                    m_DalliahTaunt = true;
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool m_PreFight;
            bool m_DalliahTaunt;
            bool m_DalliahDeath;
        };

        CreatureAI* GetAI(Creature* P_Creature) const override
        {
            return new boss_wrath_scryer_soccothratesAI(P_Creature);
        }
};

void AddSC_boss_wrath_scryer_soccothrates()
{
    new boss_wrath_scryer_soccothrates();
}