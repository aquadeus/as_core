
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "old_hillsbrad.h"

enum EpochHunter
{
    SAY_AGGRO                   = 3,
    SAY_SLAY                    = 4,
    SAY_BREATH                  = 5,
    SAY_DEATH                   = 6,

    SPELL_SAND_BREATH           = 31914,
    SPELL_IMPENDING_DEATH       = 31916,
    SPELL_MAGIC_DISRUPTION_AURA = 33834,
    SPELL_WING_BUFFET           = 31475,

    EVENT_SPELL_SAND_BREATH     = 1,
    EVENT_SPELL_IMPENDING_DEATH = 2,
    EVENT_SPELL_DISRUPTION      = 3,
    EVENT_SPELL_WING_BUFFET     = 4
};

struct boss_epoch_hunter : public ScriptedAI
{
    explicit boss_epoch_hunter(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        Talk(SAY_AGGRO);

        events.RescheduleEvent(EVENT_SPELL_SAND_BREATH, 8000);
        events.RescheduleEvent(EVENT_SPELL_IMPENDING_DEATH, 2000);
        events.RescheduleEvent(EVENT_SPELL_DISRUPTION, 20000);
        events.RescheduleEvent(EVENT_SPELL_WING_BUFFET, 14000);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            Talk(SAY_SLAY);
    }

    void JustDied(Unit* killer) override
    {
        if (killer == me)
            return;
        Talk(SAY_DEATH);
        me->GetInstanceScript()->SetData(DATA_ESCORT_PROGRESS, ENCOUNTER_PROGRESS_EPOCH_KILLED);
        if (Creature* taretha = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetData64(DATA_TARETHA_GUID)))
            taretha->AI()->DoAction(me->GetEntry());
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_SAND_BREATH:
            if (roll_chance_i(50))
                Talk(SAY_BREATH);
            DoCastVictim(SPELL_SAND_BREATH, false);
            events.RescheduleEvent(EVENT_SPELL_SAND_BREATH, 20000);
            break;
        case EVENT_SPELL_IMPENDING_DEATH:
            DoCastVictim(SPELL_IMPENDING_DEATH, false);
            events.RescheduleEvent(EVENT_SPELL_IMPENDING_DEATH, 30000);
            break;
        case EVENT_SPELL_WING_BUFFET:
            me->CastSpell(me, SPELL_WING_BUFFET, false);
            events.RescheduleEvent(EVENT_SPELL_WING_BUFFET, 30000);
            break;
        case EVENT_SPELL_DISRUPTION:
            me->CastSpell(me, SPELL_MAGIC_DISRUPTION_AURA, false);
            events.RescheduleEvent(EVENT_SPELL_DISRUPTION, 30000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_epoch_hunter()
{
    RegisterCreatureAI(boss_epoch_hunter);
}
