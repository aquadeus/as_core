
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "old_hillsbrad.h"
#include "SmartScriptMgr.h"

enum LieutenantDrake
{
    SAY_ENTER               = 0,
    SAY_AGGRO               = 1,
    SAY_SLAY                = 2,
    SAY_MORTAL              = 3,
    SAY_SHOUT               = 4,
    SAY_DEATH               = 5,

    SPELL_WHIRLWIND         = 31909,
    SPELL_EXPLODING_SHOT    = 33792,
    SPELL_HAMSTRING         = 9080,
    SPELL_MORTAL_STRIKE     = 31911,
    SPELL_FRIGHTENING_SHOUT = 33789,

    EVENT_WHIRLWIND         = 1,
    EVENT_FRIGHTENING_SHOUT = 2,
    EVENT_MORTAL_STRIKE     = 3,
    EVENT_HAMSTRING         = 4,
    EVENT_EXPLODING_SHOT    = 5
};

struct boss_lieutenant_drake : public ScriptedAI
{
    explicit boss_lieutenant_drake(Creature* creature) : ScriptedAI(creature)
    {
        InitPath();
    }

    void InitPath()
    {
        pathPoints.clear();
        WPPath* path = sSmartWaypointMgr->GetPath(me->GetEntry());
        if (!path || path->empty())
            return;

        pathPoints.push_back(G3D::Vector3(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()));

        uint32 wpCounter = 1;
        WPPath::const_iterator itr;
        while ((itr = path->find(wpCounter++)) != path->end())
        {
            WayPoint* wp = itr->second;
            pathPoints.push_back(G3D::Vector3(wp->x, wp->y, wp->z));
        }
    }

    void InitializeAI() override
    {
        ScriptedAI::InitializeAI();
        JustReachedHome();
    }

    void JustReachedHome() override
    {
        me->SetWalk(true);
        me->GetMotionMaster()->LaunchMoveCyclicPath(pathPoints);
    }

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        Talk(SAY_AGGRO);

        events.RescheduleEvent(EVENT_WHIRLWIND, 4000);
        events.RescheduleEvent(EVENT_FRIGHTENING_SHOUT, 14000);
        events.RescheduleEvent(EVENT_MORTAL_STRIKE, 9000);
        events.RescheduleEvent(EVENT_HAMSTRING, 18000);
        events.RescheduleEvent(EVENT_EXPLODING_SHOT, 1000);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            Talk(SAY_SLAY);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_DEATH);
        if (InstanceScript* instance = me->GetInstanceScript())
            instance->SetData(DATA_ESCORT_PROGRESS, ENCOUNTER_PROGRESS_DRAKE_KILLED);
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
        case EVENT_WHIRLWIND:
            me->CastSpell(me, SPELL_WHIRLWIND, false);
            events.RescheduleEvent(EVENT_WHIRLWIND, 25000);
            break;
        case EVENT_EXPLODING_SHOT:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 40.0f))
                me->CastSpell(target, SPELL_EXPLODING_SHOT, false);
            events.RescheduleEvent(EVENT_EXPLODING_SHOT, 25000);
            break;
        case EVENT_MORTAL_STRIKE:
            if (roll_chance_i(40))
                Talk(SAY_MORTAL);
            DoCastVictim(SPELL_MORTAL_STRIKE, false);
            events.RescheduleEvent(EVENT_MORTAL_STRIKE, 10000);
            break;
        case EVENT_FRIGHTENING_SHOUT:
            if (roll_chance_i(40))
                Talk(SAY_SHOUT);
            me->CastSpell(me, SPELL_FRIGHTENING_SHOUT, false);
            events.RescheduleEvent(EVENT_FRIGHTENING_SHOUT, 25000);
            break;
        case EVENT_HAMSTRING:
            DoCastVictim(SPELL_HAMSTRING, false);
            events.RescheduleEvent(EVENT_HAMSTRING, 25000);
            break;
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap events;
    Movement::PointsArray pathPoints;
};

void AddSC_boss_lieutenant_drake()
{
    RegisterCreatureAI(boss_lieutenant_drake);
}
