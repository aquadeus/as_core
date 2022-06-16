#include "scarlet_monastery.h"

enum ScriptTexts
{
    SAY_AGGRO       = 0, 
    SAY_DEATH       = 1,
    SAY_RISE_FALLEN = 2,
    SAY_INTRO       = 3,
    SAY_KILL        = 4, 
    SAY_EVICT_SOUL  = 5,
    SAY_SUM_SPIRIT  = 6,  
};

enum Spells
{
    SPELL_SUMMON_EMPOWERING_SPIRITS = 115147,
    SPELL_EMPOWERING_SPIRIT         = 115157,
    SPELL_RAISE_FALLEN_CRUSADER     = 115139,
    SPELL_EMPOWER_ZOMBIE_TRANSFORM  = 115258,
    SPELL_EMPOWER_ZOMBIE_VEHICLE    = 115239,
    SPELL_EMPOWER_ZOMBIE_AURA       = 115249,
    SPELL_EMPOWER_ZOMBIE_SUMMON     = 115250,

    SPELL_MIND_ROT                  = 115143,
    SPELL_MIND_ROT_DMG              = 115144,

    SPELL_EVICT_SOUL                = 115297,
    SPELL_EVICT_SOUL_CLONE          = 115305,
    SPELL_EVICTED_SOUL              = 115309,
    

    SPELL_SPIRIT_GALE               = 115289,
    SPELL_SPIRIT_GALE_DMG           = 115290,
    SPELL_SPIRIT_GALE_DMG2          = 115291,
};

enum Events
{
    EVENT_RAISE_FALLEN = 1,
    EVENT_SPIRIT_GALE,
    EVENT_SUMMON_SPIRITS,
    EVENT_EVICT_SOUL,
    EVENT_EMPOWERING_SPIRIT,
};

enum Adds
{
    NPC_EMPOWERED_ZOMBIE    = 59930,
    NPC_EVICTED_SOUL        = 59974,
    NPC_FALLEN_CRUSADER     = 59884,
};

class boss_thalnos_the_soulrender : public CreatureScript
{
    public:
        boss_thalnos_the_soulrender() : CreatureScript("boss_thalnos_the_soulrender") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_thalnos_the_soulrenderAI(creature);
        }

        struct boss_thalnos_the_soulrenderAI : public BossAI
        {
            boss_thalnos_the_soulrenderAI(Creature* creature) : BossAI(creature, DATA_THALNOS) 
            { 
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
            }

            void Reset() override
            {
                _Reset();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EVICTED_SOUL);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_INTRO);
                events.ScheduleEvent(EVENT_RAISE_FALLEN, 6000);
                events.ScheduleEvent(EVENT_SPIRIT_GALE, 10000);
                events.ScheduleEvent(EVENT_EVICT_SOUL, 15500);

                instance->SetBossState(DATA_THALNOS, IN_PROGRESS);
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EVICTED_SOUL);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void UpdateAI(uint32 const diff) override
            {
                if(!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_RAISE_FALLEN:
                            Talk(SAY_RISE_FALLEN);
                            DoCast(me, SPELL_RAISE_FALLEN_CRUSADER);
                            events.ScheduleEvent(EVENT_RAISE_FALLEN, 60000);
                            events.ScheduleEvent(EVENT_SUMMON_SPIRITS, 40000);
                            break;
                        case EVENT_SPIRIT_GALE:
                            DoCastVictim(SPELL_SPIRIT_GALE);
                            events.ScheduleEvent(EVENT_SPIRIT_GALE, urand(15000, 20000));
                            break;
                        case EVENT_SUMMON_SPIRITS:
                            Talk(SAY_SUM_SPIRIT);
                            DoCast(me, SPELL_SUMMON_EMPOWERING_SPIRITS);
                            break;
                        case EVENT_EVICT_SOUL:
                            Talk(SAY_EVICT_SOUL);
                            DoCastVictim(SPELL_EVICT_SOUL);
                            events.ScheduleEvent(EVENT_EVICT_SOUL, 410000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };
};

class npc_thalnos_the_soulrender_empowering_spirit : public CreatureScript
{
    public:
        npc_thalnos_the_soulrender_empowering_spirit() : CreatureScript("npc_thalnos_the_soulrender_empowering_spirit") {}

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_thalnos_the_soulrender_empowering_spiritAI(creature);
        }

        struct npc_thalnos_the_soulrender_empowering_spiritAI : public ScriptedAI
        {
            npc_thalnos_the_soulrender_empowering_spiritAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void IsSummonedBy(Unit* owner) override
            {
                events.ScheduleEvent(EVENT_EMPOWERING_SPIRIT, 1000);
            }

            void UpdateAI(const uint32 diff) override
            {
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EMPOWERING_SPIRIT:
                            DoCast(me, SPELL_EMPOWER_ZOMBIE_TRANSFORM);
                            me->DespawnOrUnsummon(1000);
                            break;
                        default:
                            break;
                    }
                }
            }
        };  
};

class npc_thalnos_the_soulrender_fallen_crusader : public CreatureScript
{
    public:
        npc_thalnos_the_soulrender_fallen_crusader() : CreatureScript("npc_thalnos_the_soulrender_fallen_crusader") {}

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_thalnos_the_soulrender_fallen_crusaderAI(creature);
        }

        struct npc_thalnos_the_soulrender_fallen_crusaderAI : public ScriptedAI
        {
            npc_thalnos_the_soulrender_fallen_crusaderAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                me->AddAura(SPELL_MIND_ROT, me);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };  
};

class npc_thalnos_the_soulrender_evicted_soul : public CreatureScript
{
    public:
        npc_thalnos_the_soulrender_evicted_soul() : CreatureScript("npc_thalnos_the_soulrender_evicted_soul") {}

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_thalnos_the_soulrender_evicted_soulAI(creature);
        }

        struct npc_thalnos_the_soulrender_evicted_soulAI : public ScriptedAI
        {
            npc_thalnos_the_soulrender_evicted_soulAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);     
            }

            uint64 fixateGuid;

            void Reset() override
            {
                fixateGuid = 0;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (auto target = ObjectAccessor::GetUnit(*me, fixateGuid))
                    target->RemoveAurasDueToSpell(SPELL_EVICTED_SOUL);
            }

            void IsSummonedBy(Unit* who) override
            {
                events.ScheduleEvent(EVENT_EVICT_SOUL, 1000);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EVICT_SOUL:
                            me->SetReactState(REACT_AGGRESSIVE);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                fixateGuid = target->GetGUID();
                                me->AddAura(SPELL_EVICTED_SOUL, target);
                                target->CastSpell(me, SPELL_EVICT_SOUL_CLONE, true);
                                me->AddThreat(target, 100000.0f);
                                AttackStart(target);
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };  
};

class spell_area_thalos_the_soulrender_spirit_gale : public AreaTriggerEntityScript
{
    public:
        spell_area_thalos_the_soulrender_spirit_gale() : AreaTriggerEntityScript("spell_area_thalos_the_soulrender_spirit_gale") { }

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (!p_Areatrigger->GetCaster())
                return false;

            if (!p_Target->isTargetableForAttack())
                return false;

            p_Target->AddAura(SPELL_SPIRIT_GALE_DMG2, p_Target);

            return true;
        }

        bool OnRemoveTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            p_Target->RemoveAura(Spells::SPELL_SPIRIT_GALE_DMG2);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_area_thalos_the_soulrender_spirit_gale();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_thalnos_the_soulrender()
{
    new boss_thalnos_the_soulrender();

    new npc_thalnos_the_soulrender_empowering_spirit();
    new npc_thalnos_the_soulrender_fallen_crusader();
    new npc_thalnos_the_soulrender_evicted_soul();

    new spell_area_thalos_the_soulrender_spirit_gale();
}
#endif