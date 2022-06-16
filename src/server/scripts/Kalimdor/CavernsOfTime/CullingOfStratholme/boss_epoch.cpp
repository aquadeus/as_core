
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "culling_of_stratholme.h"
#include "SpellInfo.h"

enum Spells
{
    SPELL_CURSE_OF_EXERTION                     = 52772,
    SPELL_WOUNDING_STRIKE_N                     = 52771,
    SPELL_WOUNDING_STRIKE_H                     = 58830,
    SPELL_TIME_STOP                             = 58848,
    SPELL_TIME_WARP                             = 52766,
    SPELL_TIME_STEP_N                           = 52737,
    SPELL_TIME_STEP_H                           = 58829,
};

enum Events
{
    EVENT_SPELL_CURSE_OF_EXERTION               = 1,
    EVENT_SPELL_WOUNDING_STRIKE                 = 2,
    EVENT_SPELL_TIME_STOP                       = 3,
    EVENT_SPELL_TIME_WARP                       = 4,
    EVENT_TIME_WARP                             = 5,
};

enum Yells
{
    SAY_INTRO                                   = 0,
    SAY_AGGRO                                   = 1,
    SAY_TIME_WARP                               = 2,
    SAY_SLAY                                    = 3,
    SAY_DEATH                                   = 4
};

struct boss_epoch : public BossAI
{
    boss_epoch(Creature* c) : BossAI(c, BOSS_EPOCH) {}

    uint8 warps = 0;

    void Reset() override
    {
        _Reset();
        warps = 0;
    }

    void EnterCombat(Unit* /*who*/) override
    {
        _EnterCombat();
        Talk(SAY_AGGRO);

        events.RescheduleEvent(EVENT_SPELL_CURSE_OF_EXERTION, 9000);
        events.RescheduleEvent(EVENT_SPELL_WOUNDING_STRIKE, 3000);
        events.RescheduleEvent(EVENT_SPELL_TIME_WARP, 25000);

        if (IsHeroic())
            events.RescheduleEvent(EVENT_SPELL_TIME_STOP, 20000);
    }

    void SpellHitTarget(Unit* target, const SpellInfo* spellInfo) override
    {
        if (spellInfo->Id == SPELL_TIME_STEP_H || spellInfo->Id == SPELL_TIME_STEP_N)
        {
            if (target == me)
                return;

            if (warps >= 2)
            {
                warps = 0;
                return;
            }
            warps++;
            me->CastSpell(target, DUNGEON_MODE(SPELL_TIME_STEP_N, SPELL_TIME_STEP_H), true);
        }
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
        case EVENT_SPELL_CURSE_OF_EXERTION:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                me->CastSpell(target, SPELL_CURSE_OF_EXERTION, false);
            events.RepeatEvent(9000);
            break;
        case EVENT_SPELL_WOUNDING_STRIKE:
            me->CastSpell(me->getVictim(), DUNGEON_MODE(SPELL_WOUNDING_STRIKE_N, SPELL_WOUNDING_STRIKE_H), false);
            events.RepeatEvent(6000);
            break;
        case EVENT_SPELL_TIME_STOP:
            me->CastSpell(me, SPELL_TIME_STOP, false);
            events.RepeatEvent(20000);
            break;
        case EVENT_SPELL_TIME_WARP:
            Talk(SAY_TIME_WARP);
            me->CastSpell(me, SPELL_TIME_WARP, false);
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                me->CastSpell(target, DUNGEON_MODE(SPELL_TIME_STEP_N, SPELL_TIME_STEP_H), true);

            events.RepeatEvent(25000);
            break;
        }

        DoMeleeAttackIfReady();
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
};

void AddSC_boss_epoch()
{
    RegisterCreatureAI(boss_epoch);
}