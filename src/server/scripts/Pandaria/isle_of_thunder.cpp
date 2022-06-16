#include "isle_of_thunder.h"
#include "Group.h"

// Fleshcrafter Hoku 69435
class npc_isle_of_thunder_fleshcrafter_hoku : public CreatureScript
{
    public:

        enum LocalScriptedTexts
        {
            SAY_AGGRO   = 0,
            SAY_BLOOD   = 1,
            SAY_DEATH   = 2,
        };

        enum LocalEvents
        {
            EVENT_BLOOD_BOLT    = 1,
            EVENT_RIVERS_OF_BLOOD,
            EVENT_EXSANGUINATION,
        };

        enum LocalSpells
        {
            SPELL_BLOOD_BOLT            = 136985,
            SPELL_RIVERS_OF_BLOOD       = 140526,
            SPELL_RIVERS_OF_BLOOD_DMG   = 140550,
            SPELL_EXSANGUINATION        = 137696,
            SPELL_EXSANGUINATION_SUMMON = 136981,

            SPELL_ACHIEVEMENT           = 139306,
        };

        enum LocalAdds
        {
            NPC_POSSESSED_BLOOD         = 69436,
        };

        enum LocalTimers
        {
            TIMER_BLOOD_BOLT_FIRST      = 1000,
            TIMER_BLOOD_BOLT            = 3000,
            TIMER_RIVERS_OF_BLOOD_FIRST = 10000,
            TIMER_RIVERS_OF_BLOOD       = 15000,
            TIMER_EXSANGUINATION_FIRST  = 20000,
            TIMER_EXSANGUINATION        = 60000,
        };

    public:
        npc_isle_of_thunder_fleshcrafter_hoku() : CreatureScript("npc_isle_of_thunder_fleshcrafter_hoku") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_isle_of_thunder_fleshcrafter_hokuAI(creature);
        }

        struct npc_isle_of_thunder_fleshcrafter_hokuAI : public ScriptedAI
        {
            npc_isle_of_thunder_fleshcrafter_hokuAI(Creature* creature) :
                ScriptedAI(creature), m_IsBloodKilled(false)
            {

            }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();

                m_IsBloodKilled = false;
            }

            void EnterCombat(Unit* attacker) override
            {
                Talk(SAY_AGGRO);

                events.ScheduleEvent(EVENT_BLOOD_BOLT, TIMER_BLOOD_BOLT_FIRST);
                events.ScheduleEvent(EVENT_RIVERS_OF_BLOOD, TIMER_RIVERS_OF_BLOOD_FIRST);
                events.ScheduleEvent(EVENT_EXSANGUINATION, TIMER_EXSANGUINATION_FIRST);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);

                if (summon->GetEntry() == NPC_POSSESSED_BLOOD)
                {
                    m_IsBloodKilled = true;
                }
            }

            void JustDied(Unit* killer) override
            {
                Talk(SAY_DEATH);

                events.Reset();
                summons.DespawnAll();

                if (!m_IsBloodKilled)
                {
                    if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                    {
                        DoAchievementForKillers(player);
                    }
                }
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
                    ProcessEvents(eventId);
                }

                DoMeleeAttackIfReady();
            }

        private:

            void ProcessEvents(const uint32 eventId)
            {
                switch (eventId)
                {
                    case EVENT_BLOOD_BOLT:
                        DoBloodBolt();
                        events.ScheduleEvent(EVENT_BLOOD_BOLT, TIMER_BLOOD_BOLT);
                        break;
                    case EVENT_RIVERS_OF_BLOOD:
                        DoRiversOfBlood();
                        events.ScheduleEvent(EVENT_RIVERS_OF_BLOOD, TIMER_RIVERS_OF_BLOOD);
                        break;
                    case EVENT_EXSANGUINATION:
                        DoExsanguination();
                        events.ScheduleEvent(EVENT_EXSANGUINATION, TIMER_EXSANGUINATION);
                        break;
                }
            }

            void DoBloodBolt()
            {
                DoCastVictim(SPELL_BLOOD_BOLT);
            }

            void DoRiversOfBlood()
            {
                DoCastVictim(SPELL_RIVERS_OF_BLOOD);
            }

            void DoExsanguination()
            {
                Talk(SAY_BLOOD);

                DoCastAOE(SPELL_EXSANGUINATION);
            }

            void DoAchievementForKillers(Player* searcher)
            {
                GroupPtr group = searcher->GetGroup();
                if (group)
                {
                    group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr) -> void
                    {
                        if (Player* member = l_Itr->player)
                        {
                            if (member->IsWithinDistInMap(me, 30.0f))
                                member->UpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT);
                        }
                    });
                }
                else
                {
                    searcher->UpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT);
                }
            }

        private:

            bool m_IsBloodKilled;

        };
};

//
class spell_isle_of_thunder_exsanguination : public SpellScriptLoader
{
    public:
        spell_isle_of_thunder_exsanguination() : SpellScriptLoader("spell_isle_of_thunder_exsanguination") { }

        class spell_isle_of_thunder_exsanguination_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_isle_of_thunder_exsanguination_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), npc_isle_of_thunder_fleshcrafter_hoku::SPELL_EXSANGUINATION_SUMMON, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_isle_of_thunder_exsanguination_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_isle_of_thunder_exsanguination_SpellScript();
        }
};


void AddSC_isle_of_thunder()
{
    new npc_isle_of_thunder_fleshcrafter_hoku();    // 69435

    new spell_isle_of_thunder_exsanguination();     // 137696
}
