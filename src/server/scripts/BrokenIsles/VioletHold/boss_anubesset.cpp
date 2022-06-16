////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

enum Spells
{
    SPELL_MANDIBLE_STRIKE       = 202217,
    SPELL_IMPALE                = 202341,
    SPELL_IMPALE_TRIG           = 202350,
    SPELL_CALL_OF_THE_SWARM     = 201863,
    SPELL_CALL_OF_THE_SWARM_SUM = 201868,

    SPELL_POISON_SPIT           = 202300,
    SPELL_BURROW                = 202304,

    SPELL_FIXATED               = 202480,
    SPELL_BLISTERING_OOZE       = 202487,

    SPELL_BONUS_ROLL            = 226656
};

enum eEvents
{
    EVENT_MANDIBLE_STRIKE       = 1,
    EVENT_IMPALE                = 2,
    EVENT_CALL_OF_THE_SWARM     = 3,
};

/// Anub'esset - 102246
class boss_anubesset : public CreatureScript
{
public:
    boss_anubesset() : CreatureScript("boss_anubesset") {}

    struct boss_anubessetAI : public BossAI
    {
        boss_anubessetAI(Creature* creature) : BossAI(creature, DATA_ANUBESSET)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            removeloot = false;
        }

        bool removeloot;

        void Reset() override
        {
            _Reset();

            if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                me->SetReactState(REACT_DEFENSIVE);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();

            events.ScheduleEvent(EVENT_MANDIBLE_STRIKE, 8000);    //01:49, 02:20, 02:43
            events.ScheduleEvent(EVENT_IMPALE, 18000);            //01:59, 02:22
            events.ScheduleEvent(EVENT_CALL_OF_THE_SWARM, 30000); //02:11, 03:13
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();

            if (removeloot)
            {
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                me->ClearLootContainers();
            }

            CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_REMOVE_LOOT)
                removeloot = true;
        }

        void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
        {
            if (p_Dest != nullptr && p_SpellInfo->Id == SPELL_CALL_OF_THE_SWARM_SUM && IsHeroicOrMythic())
            {
                if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 80.0f, true))
                {
                    Position l_Pos;

                    l_Target->GetNearPosition(l_Pos, 10.0f, frand(0.0f, M_PI * 2.0f));

                    if (Creature* l_Summon = me->SummonCreature(NPC_BLISTERING_BEETLE, l_Pos))
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 80.0f, true, -int32(SPELL_FIXATED) ))
                        {
                            if (l_Summon->IsAIEnabled)
                                l_Summon->AI()->SetGUID(l_Target->GetGUID());
                        }
                    }
                }
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_IMPALE)
            {
                Position pos;
                float dist = 1.0f;
                float angle;
                for(uint16 i = 0; i < 120; i++)
                {
                    angle = frand(-0.2f, 0.2f);
                    me->GetNearPosition(pos, dist, angle);
                    me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_IMPALE_TRIG, true);
                    dist += 2;
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_MANDIBLE_STRIKE:
                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_MANDIBLE_STRIKE);
                        events.ScheduleEvent(EVENT_MANDIBLE_STRIKE, 23000);
                        break;
                    case EVENT_IMPALE:
                        if (Unit* l_Target = SelectRangedTarget(true))
                            DoCast(l_Target, SPELL_IMPALE);
                        else if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                            DoCast(pTarget, SPELL_IMPALE);
                        events.ScheduleEvent(EVENT_IMPALE, 23000);
                        break;
                    case EVENT_CALL_OF_THE_SWARM:
                        DoCast(SPELL_CALL_OF_THE_SWARM);
                        events.ScheduleEvent(EVENT_CALL_OF_THE_SWARM, 60000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_anubessetAI (creature);
    }
};

//102271
class npc_anubesset_spitting_scarab : public CreatureScript
{
public:
    npc_anubesset_spitting_scarab() : CreatureScript("npc_anubesset_spitting_scarab") {}

    struct npc_anubesset_spitting_scarabAI : public ScriptedAI
    {
        npc_anubesset_spitting_scarabAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void Reset() override
        {
            events.Reset();
            events.ScheduleEvent(EVENT_1, 2000);
            events.ScheduleEvent(EVENT_2, 8000);
        }

        void IsSummonedBy(Unit* summoner) override
        {
            DoZoneInCombat(me, 120.0f);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                            DoCast(pTarget, SPELL_POISON_SPIT);
                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    case EVENT_2:
                    {
                        Position pos;
                        me->GetRandomNearPosition(pos, 40.0f);
                        me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_BURROW, true);
                        events.ScheduleEvent(EVENT_2, 16000);
                        break;
                    }
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_anubesset_spitting_scarabAI(creature);
    }
};

/// Blistering Beetle - 102540
class npc_anubesset_blistering_beetle : public CreatureScript
{
    public:
        npc_anubesset_blistering_beetle() : CreatureScript("npc_anubesset_blistering_beetle") { }

        struct npc_anubesset_blistering_beetleAI : public ScriptedAI
        {
            npc_anubesset_blistering_beetleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_FixateTarget;

            bool m_Exploded;

            void Reset() override
            {
                m_FixateTarget = 0;

                m_Exploded = false;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoZoneInCombat(me, 120.0f);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                if (Unit* l_Target = Unit::GetUnit(*me, p_Guid))
                {
                    m_FixateTarget = l_Target->GetGUID();

                    DoResetThreat();
                    me->AddThreat(l_Target, 1000000.0f);

                    AttackStart(l_Target);

                    me->ClearUnitState(UnitState::UNIT_STATE_CASTING);
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                if (m_Exploded)
                    return;

                DoCast(me, SPELL_BLISTERING_OOZE);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (Unit* l_Target = Unit::GetUnit(*me, m_FixateTarget))
                {
                    if (me->IsWithinMeleeRange(l_Target))
                    {
                        m_FixateTarget = 0;

                        me->GetMotionMaster()->Clear();

                        DoCast(me, SPELL_BLISTERING_OOZE);

                        m_Exploded = true;

                        me->Kill(me);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_anubesset_blistering_beetleAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_anubesset()
{
    new boss_anubesset();
    new npc_anubesset_spitting_scarab();
    new npc_anubesset_blistering_beetle();
}
#endif
