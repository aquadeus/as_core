
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_CONSTRICTING_CHAINS_N                 = 52696,
    SPELL_CONSTRICTING_CHAINS_H                 = 58823,
    SPELL_DISEASE_EXPULSION_N                   = 52666,
    SPELL_DISEASE_EXPULSION_H                   = 58824,
    SPELL_FRENZY                                = 58841,
};

enum Events
{
    EVENT_SPELL_CONSTRICTING_CHAINS             = 1,
    EVENT_SPELL_DISEASE_EXPULSION               = 2,
    EVENT_SPELL_FRENZY                          = 3,
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_SPAWN                                   = 2,
    SAY_DEATH                                   = 3
};

struct boss_meathook : public BossAI
{
    boss_meathook(Creature* c) : BossAI(c, BOSS_MEAT_HOOK)
    {
        Talk(SAY_SPAWN);
    }

    void Reset()  override { _Reset(); }

    void EnterCombat(Unit* /*who*/) override
    {
        _EnterCombat();
        Talk(SAY_AGGRO);
        events.RescheduleEvent(EVENT_SPELL_CONSTRICTING_CHAINS, 15000);
        events.RescheduleEvent(EVENT_SPELL_DISEASE_EXPULSION, 4000);
        events.RescheduleEvent(EVENT_SPELL_FRENZY, 20000);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
        Talk(SAY_DEATH);
    }

    void KilledUnit(Unit*  /*victim*/) override
    {
        if (!urand(0, 1))
            return;

        Talk(SAY_SLAY);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.GetEvent())
        {
        case EVENT_SPELL_DISEASE_EXPULSION:
            me->CastSpell(me, DUNGEON_MODE(SPELL_DISEASE_EXPULSION_N, SPELL_DISEASE_EXPULSION_H), false);
            events.RepeatEvent(6000);
            break;
        case EVENT_SPELL_FRENZY:
            me->CastSpell(me, SPELL_FRENZY, false);
            events.RepeatEvent(20000);
            break;
        case EVENT_SPELL_CONSTRICTING_CHAINS:
            if (Unit* pTarget = SelectTarget(SELECT_TARGET_BOTTOMAGGRO, 0, 50.0f, true))
                me->CastSpell(pTarget, DUNGEON_MODE(SPELL_CONSTRICTING_CHAINS_N, SPELL_CONSTRICTING_CHAINS_H), false);
            events.RepeatEvent(14000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_meathook()
{
    RegisterCreatureAI(boss_meathook);
}
