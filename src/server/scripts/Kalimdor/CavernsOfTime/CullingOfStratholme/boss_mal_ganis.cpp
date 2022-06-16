
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "culling_of_stratholme.h"
#include "Player.h"

enum Spells
{
    SPELL_CARRION_SWARM_N                       = 52720,
    SPELL_CARRION_SWARM_H                       = 58852,
    SPELL_MIND_BLAST_N                          = 52722,
    SPELL_MIND_BLAST_H                          = 58850,
    SPELL_SLEEP_N                               = 52721,
    SPELL_SLEEP_H                               = 58849,
    SPELL_VAMPIRIC_TOUCH                        = 52723,
};

enum Events
{
    EVENT_SPELL_CARRION_SWARM                   = 1,
    EVENT_SPELL_MIND_BLAST                      = 2,
    EVENT_SPELL_SLEEP                           = 3,
    EVENT_SPELL_VAMPIRIC_TOUCH                  = 4,
};

enum Yells
{
    SAY_AGGRO                                   = 2,
    SAY_KILL                                    = 3,
    SAY_SLAY                                    = 4,
    SAY_SLEEP                                   = 5,
    SAY_30HEALTH                                = 6,
    SAY_15HEALTH                                = 7,
    SAY_ESCAPE_SPEECH_1                         = 8,
    SAY_ESCAPE_SPEECH_2                         = 9,
    SAY_OUTRO                                   = 10
};

struct boss_mal_ganis : public BossAI
{
    boss_mal_ganis(Creature* c) : BossAI(c, BOSS_MALGANIS)
    {
        finished = false;
    }

    bool finished = false;

    void Reset() override
    {
        _Reset();
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
        events.Reset();
        if (finished)
        {
            Talk(SAY_OUTRO);
            me->DespawnOrUnsummon(20000);
        }
    }

    void EnterCombat(Unit* /*who*/) override
    {
        _EnterCombat();
        Talk(SAY_AGGRO);
        events.RescheduleEvent(EVENT_SPELL_CARRION_SWARM, 6000);
        events.RescheduleEvent(EVENT_SPELL_MIND_BLAST, 11000);
        events.RescheduleEvent(EVENT_SPELL_SLEEP, 20000);
        events.RescheduleEvent(EVENT_SPELL_VAMPIRIC_TOUCH, 15000);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
    }

    void KilledUnit(Unit*  /*victim*/) override
    {
        if (!urand(0, 1))
            return;

        Talk(SAY_SLAY);
    }

    void DamageTaken(Unit* who, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
    {
        if (!finished && damage >= me->GetHealth())
        {
            damage = 0;
            finished = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
            if (auto instance = me->GetInstanceScript())
            {
                if (auto cr = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ARTHAS)))
                    cr->AI()->DoAction(ACTION_KILLED_MALGANIS);

                // give credit to players
                instance->DoUpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, 58630);

                instance->DoUpdateCriteria(CRITERIA_TYPE_SEND_EVENT_SCENARIO, 18716);
                if (auto chest = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_MALGANIS_CHEST)))
                {
                    chest->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                    chest->SetVisible(true);
                }

                instance->instance->ApplyOnEveryPlayer([&](Player* player)
                {
                    player->KilledMonsterCredit(31006, 0);
                });
            }

            EnterEvadeMode();
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
        case EVENT_SPELL_CARRION_SWARM:
            me->CastSpell(me->getVictim(), DUNGEON_MODE(SPELL_CARRION_SWARM_N, SPELL_CARRION_SWARM_H), false);
            events.RepeatEvent(7000);
            break;
        case EVENT_SPELL_MIND_BLAST:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                me->CastSpell(target, DUNGEON_MODE(SPELL_MIND_BLAST_N, SPELL_MIND_BLAST_H), false);
            events.RepeatEvent(6000);
            break;
        case EVENT_SPELL_SLEEP:
            Talk(SAY_SLEEP);
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                me->CastSpell(target, DUNGEON_MODE(SPELL_SLEEP_N, SPELL_SLEEP_H), false);
            events.RepeatEvent(17000);
            break;
        case EVENT_SPELL_VAMPIRIC_TOUCH:
            me->CastSpell(me, SPELL_VAMPIRIC_TOUCH, true);
            events.RepeatEvent(30000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_mal_ganis()
{
    RegisterCreatureAI(boss_mal_ganis);
}
