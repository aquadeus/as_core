////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Boss_Broggok
SD%Complete: 70
SDComment: pre-event not made
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "blood_furnace.h"

enum eEnums
{
    SAY_AGGRO               = -1542008,

    SPELL_SLIME_SPRAY       = 30913,
    SPELL_POISON_CLOUD      = 30916,
    SPELL_POISON_BOLT       = 30917,

    SPELL_POISON            = 30914
};

enum eEvents
{
    EVENT_SLIME_SPRAY       = 1,
    EVENT_POISON_BOLT,
    EVENT_POISON_CLOUD
};

// 17380
class boss_broggok : public CreatureScript
{
    public:

        boss_broggok()
            : CreatureScript("boss_broggok")
        {
        }

        struct boss_broggokAI : public BossAI
        {
            boss_broggokAI(Creature* creature) : BossAI(creature, DATA_BROGGOK)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 AcidSpray_Timer;
            uint32 PoisonSpawn_Timer;
            uint32 PoisonBolt_Timer;
            bool canAttack;


            void Reset() override
            {
                _Reset();
                AcidSpray_Timer = 10000;
                PoisonSpawn_Timer = 5000;
                PoisonBolt_Timer = 7000;
                DoAction(ACTION_RESET_BROGGOK);
                instance->SetData(TYPE_BROGGOK_EVENT, NOT_STARTED);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                DoScriptText(SAY_AGGRO, me);
            }

            void JustSummoned(Creature* summoned) override
            {
                summoned->setFaction(FACTION_MONSTER_2);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                summoned->CastSpell(summoned, SPELL_POISON, false, 0, nullptr, me->GetGUID());
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_SLIME_SPRAY:
                        DoCastVictim(SPELL_SLIME_SPRAY);
                        events.ScheduleEvent(EVENT_SLIME_SPRAY, urand(4000, 12000));
                        break;
                    case EVENT_POISON_BOLT:
                        DoCastVictim(SPELL_POISON_BOLT);
                        events.ScheduleEvent(EVENT_POISON_BOLT, urand(4000, 12000));
                        break;
                    case EVENT_POISON_CLOUD:
                        DoCast(me, SPELL_POISON_CLOUD);
                        events.ScheduleEvent(EVENT_POISON_CLOUD, 20000);
                        break;
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_PREPARE_BROGGOK:
                    {
                        me->SetInCombatWithZone();
                        break;
                    }
                    case ACTION_ACTIVATE_BROGGOK:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        events.ScheduleEvent(EVENT_SLIME_SPRAY, 10000);
                        events.ScheduleEvent(EVENT_POISON_BOLT, 7000);
                        events.ScheduleEvent(EVENT_POISON_CLOUD, 5000);
                        break;
                    }
                    case ACTION_RESET_BROGGOK:
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    instance->HandleGameObject(instance->GetData64(DATA_DOOR4), true);
                    instance->HandleGameObject(instance->GetData64(DATA_DOOR5), true);
                    instance->SetData(TYPE_BROGGOK_EVENT, DONE);
                }
            }

        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_broggokAI(creature);
        }
};

class go_broggok_lever : public GameObjectScript
{
    public:
        go_broggok_lever() : GameObjectScript("go_broggok_lever") {}

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (instance->GetData(TYPE_BROGGOK_EVENT) == NOT_STARTED)
                {
                    instance->SetData(TYPE_BROGGOK_EVENT, IN_PROGRESS);
                    if (Creature* broggok = Creature::GetCreature(*go, instance->GetData64(DATA_BROGGOK)))
                    {
                        if (broggok->IsAIEnabled)
                            broggok->AI()->DoAction(ACTION_PREPARE_BROGGOK);
                    }
                }
                go->UseDoorOrButton();
            }
            return false;
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_broggok()
{
    new boss_broggok();
    new go_broggok_lever();
}
#endif
