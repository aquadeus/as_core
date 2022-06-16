
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_CORRUPTING_BLIGHT                     = 60588,
    SPELL_VOID_STRIKE                           = 60590,
    SPELL_CORRUPTION_OF_TIME_AURA               = 60451,
    SPELL_CORRUPTION_OF_TIME_CHANNEL            = 60422,
};

enum Events
{
    EVENT_SPELL_CORRUPTING_BLIGHT               = 1,
    EVENT_SPELL_VOID_STRIKE                     = 2,
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_DEATH                                   = 1,
    SAY_FAIL                                    = 2
};

struct boss_infinite_corruptor : public BossAI
{
    boss_infinite_corruptor(Creature* c) : BossAI(c, BOSS_CORRUPTOR) {}

    uint32 beamTimer = 0;

    void Reset() override
    {
        _Reset();
        if (InstanceScript* pInstance = me->GetInstanceScript())
            if (pInstance->GetData(DATA_GUARDIANTIME_EVENT) == 0)
                me->DespawnOrUnsummon(500);

        me->SummonCreature(NPC_TIME_RIFT, 2337.6f, 1270.0f, 132.95f, 2.79f);
        me->SummonCreature(NPC_GUARDIAN_OF_TIME, 2319.3f, 1267.7f, 132.8f, 1.0f);
        beamTimer = 1;
    }

    void JustSummoned(Creature* cr) override { summons.Summon(cr); }

    void EnterCombat(Unit* /*who*/) override
    {
        _EnterCombat();
        me->InterruptNonMeleeSpells(false);
        events.RescheduleEvent(EVENT_SPELL_VOID_STRIKE, 8000);
        events.RescheduleEvent(EVENT_SPELL_CORRUPTING_BLIGHT, 12000);
        Talk(SAY_AGGRO);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
        Talk(SAY_DEATH);

        for (SummonList::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* cr = ObjectAccessor::GetCreature(*me, (*itr)))
            {
                if (cr->GetEntry() == NPC_TIME_RIFT)
                    cr->DespawnOrUnsummon(1000);
                else
                {
                    cr->DespawnOrUnsummon(5000);
                    cr->RemoveAllAuras();

                    if (cr->IsAIEnabled)
                        cr->AI()->Talk(0);
                }
            }

        instance->SetData(DATA_SHOW_INFINITE_TIMER, 0);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);
    }

    void DoAction(int32 param) override
    {
        if (!me->isAlive())
            return;

        if (param == ACTION_RUN_OUT_OF_TIME)
        {
            Talk(SAY_FAIL);
            summons.DespawnAll();
            me->DespawnOrUnsummon(500);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (beamTimer)
        {
            beamTimer += diff;
            if (beamTimer >= 2000)
            {
                me->CastSpell(me, SPELL_CORRUPTION_OF_TIME_CHANNEL, true);
                beamTimer = 0;
            }
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.GetEvent())
        {
        case EVENT_SPELL_VOID_STRIKE:
            me->CastSpell(me->getVictim(), SPELL_VOID_STRIKE, false);
            events.RepeatEvent(8000);
            break;
        case EVENT_SPELL_CORRUPTING_BLIGHT:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                me->CastSpell(target, SPELL_CORRUPTING_BLIGHT, false);
            events.RepeatEvent(12000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_infinite_corruptor()
{
    RegisterCreatureAI(boss_infinite_corruptor);
}
