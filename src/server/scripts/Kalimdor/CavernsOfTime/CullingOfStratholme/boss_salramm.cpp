
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "culling_of_stratholme.h"
#include "SpellScript.h"

enum Spells
{
    SPELL_SHADOW_BOLT_N                         = 57725,
    SPELL_SHADOW_BOLT_H                         = 58827,
    SPELL_STEAL_FLESH_CHANNEL                   = 52708,
    SPELL_STEAL_FLESH_TARGET                    = 52711,
    SPELL_STEAL_FLESH_CASTER                    = 52712,
    SPELL_SUMMON_GHOULS                         = 52451,
    SPELL_EXPLODE_GHOUL_N                       = 52480,
    SPELL_EXPLODE_GHOUL_H                       = 58825,
    SPELL_CURSE_OF_TWISTED_FAITH                = 58845,
};

enum Events
{
    EVENT_SPELL_SHADOW_BOLT                     = 1,
    EVENT_SPELL_STEAL_FLESH                     = 2,
    EVENT_SPELL_SUMMON_GHOULS                   = 3,
    EVENT_EXPLODE_GHOUL                         = 4,
    EVENT_SPELL_CURSE                           = 5,
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SPAWN                                   = 1,
    SAY_SLAY                                    = 2,
    SAY_DEATH                                   = 3,
    SAY_EXPLODE_GHOUL                           = 4,
    SAY_STEAL_FLESH                             = 5,
    SAY_SUMMON_GHOULS                           = 6
};

struct boss_salramm : public BossAI
{
    boss_salramm(Creature* c) : BossAI(c, BOSS_SALRAMM)
    {
        Talk(SAY_SPAWN);
    }

    void Reset() override
    {
        _Reset();
    }

    void JustSummoned(Creature* cr) override { summons.Summon(cr); }

    void EnterCombat(Unit* /*who*/) override
    {
        _EnterCombat();
        Talk(SAY_AGGRO);
        events.RescheduleEvent(EVENT_SPELL_SHADOW_BOLT, 7000);
        events.RescheduleEvent(EVENT_SPELL_STEAL_FLESH, 11000);
        events.RescheduleEvent(EVENT_SPELL_SUMMON_GHOULS, 16000);
        events.RescheduleEvent(EVENT_EXPLODE_GHOUL, 22000);
        if (IsHeroic())
            events.RescheduleEvent(EVENT_SPELL_CURSE, 25000);
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

    void ExplodeGhoul()
    {
        for (SummonList::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* cr = ObjectAccessor::GetCreature(*me, (*itr)))
                if (cr->isAlive())
                {
                    me->CastSpell(cr, DUNGEON_MODE(SPELL_EXPLODE_GHOUL_N, SPELL_EXPLODE_GHOUL_H), false);
                    return;
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
        case EVENT_SPELL_SHADOW_BOLT:
            me->CastSpell(me->getVictim(), DUNGEON_MODE(SPELL_SHADOW_BOLT_N, SPELL_SHADOW_BOLT_H), false);
            events.RepeatEvent(10000);
            break;
        case EVENT_SPELL_STEAL_FLESH:
            if (!urand(0, 2))
                Talk(SAY_STEAL_FLESH);
            me->CastSpell(me->getVictim(), SPELL_STEAL_FLESH_CHANNEL, false);
            events.RepeatEvent(12000);
            break;
        case EVENT_SPELL_SUMMON_GHOULS:
            if (!urand(0, 2))
                Talk(SAY_SUMMON_GHOULS);
            me->CastSpell(me, SPELL_SUMMON_GHOULS, false);
            events.RepeatEvent(10000);
            break;
        case EVENT_EXPLODE_GHOUL:
            if (!urand(0, 2))
                Talk(SAY_EXPLODE_GHOUL);
            ExplodeGhoul();
            events.RepeatEvent(15000);
            break;
        case EVENT_SPELL_CURSE:
            me->CastSpell(me->getVictim(), SPELL_CURSE_OF_TWISTED_FAITH, false);
            events.RepeatEvent(30000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class spell_boss_salramm_steal_flesh : public AuraScript
{
    PrepareAuraScript(spell_boss_salramm_steal_flesh);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetUnitOwner();
        if (caster)
        {
            caster->CastSpell(caster, SPELL_STEAL_FLESH_CASTER, true);
            caster->CastSpell(target, SPELL_STEAL_FLESH_TARGET, true);
        }
    }

    void Register()
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_boss_salramm_steal_flesh::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_salramm()
{
    RegisterCreatureAI(boss_salramm);
    RegisterAuraScript(spell_boss_salramm_steal_flesh);
}
