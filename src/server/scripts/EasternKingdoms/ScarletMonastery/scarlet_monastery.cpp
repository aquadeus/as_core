#include "scarlet_monastery.h"

enum Spells
{
    // Scarlet Fanatic
    SPELL_FANATIC_STRIKE        = 110956,
    SPELL_HANDS_OF_PURITY       = 110954,
    SPELL_HANDS_OF_PURITY_DMG   = 110953,

    // Scarlet Judicator
    SPELL_PASS_JUDGMENT         = 111107,

    // Scarlet Purifier
    SPELL_FLAMESTRIKE           = 110963,
    SPELL_PURIFYING_FLAMES      = 110968,

    // Scarlet Zealot
    SPELL_HEAL                  = 111008,
    SPELL_SMITE                 = 111010,
    SPELL_RESURRECTION          = 111670,

    // Scarlet Centurion
    SPELL_CLEAVE                = 115519,
    SPELL_RETALIATION           = 115511,

    // Scarlet Flamethrower
    SPELL_FLAMETHROWER          = 115506,
    SPELL_FLAMETHROWER_DMG      = 115507,

    // Fuel Tank
    SPELL_FUEL_BARREL           = 114875,
    SPELL_BARREL_EXPLOSION      = 114952,

    // Pile of Corpses
    SPELL_PILE_OF_CORPSES       = 114951
};

enum Adds
{
    NPC_SCARLET_FANATIC         = 58555,
    NPC_SCARLET_JUDICATOR       = 58605,
    NPC_SCARLET_PURIFIER        = 58569,
    NPC_SCARLET_ZEALOT          = 58590,
    NPC_SCARLET_CENTURION       = 59746,
    NPC_SCARLET_FLAMETHROWER    = 59705,
    NPC_FUEL_TANK               = 59706,
    NPC_PILE_OF_CORPSES         = 59722
};

enum Events
{
    // Scarlet Fanatic
    EVENT_FANATIC_STRIKE    = 1,

    // Scarlet Judicator
    EVENT_PASS_JUDGMENT,

    // Scarlet Purifier
    EVENT_FLAMESTRIKE,
    EVENT_PURIFYING_FLAMES,

    // Scarlet Zealot
    EVENT_HEAL,
    EVENT_SMITE,
    EVENT_RESURRECTION,

    // Scarlet Centurion
    EVENT_CLEAVE,
    EVENT_RETALIATION,

    // Scarlet Flamethrower
    EVENT_FLAMETHROWER,
};

class npc_scarlet_monastery_scarlet_fanatic : public CreatureScript
{
    public:
        npc_scarlet_monastery_scarlet_fanatic() : CreatureScript("npc_scarlet_monastery_scarlet_fanatic") {}

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_monastery_scarlet_fanaticAI(creature);
        }

        struct npc_scarlet_monastery_scarlet_fanaticAI : public ScriptedAI
        {
            npc_scarlet_monastery_scarlet_fanaticAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                events.Reset();                
            }

            void EnterCombat(Unit* who) override
            {
                events.ScheduleEvent(EVENT_FANATIC_STRIKE, urand(3000, 7000));
                DoCast(me, Spells::SPELL_HANDS_OF_PURITY, true);
            }

            void JustDied(Unit* who) override
            {
                events.Reset();
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
                        case EVENT_FANATIC_STRIKE:
                            DoCastVictim(Spells::SPELL_FANATIC_STRIKE);
                            events.ScheduleEvent(EVENT_FANATIC_STRIKE, urand(5000, 7000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };  
};

class npc_scarlet_monastery_scarlet_judicator : public CreatureScript
{
    public:
        npc_scarlet_monastery_scarlet_judicator() : CreatureScript("npc_scarlet_monastery_scarlet_judicator") {}

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_monastery_scarlet_judicatorAI(creature);
        }

        struct npc_scarlet_monastery_scarlet_judicatorAI : public ScriptedAI
        {
            npc_scarlet_monastery_scarlet_judicatorAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                events.Reset();                
            }

            void EnterCombat(Unit* who) override
            {
                events.ScheduleEvent(EVENT_PASS_JUDGMENT, urand(3000, 7000));
            }

            void JustDied(Unit* who) override
            {
                events.Reset();
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
                        case EVENT_PASS_JUDGMENT:
                            DoCastVictim(Spells::SPELL_PASS_JUDGMENT);
                            events.ScheduleEvent(EVENT_PASS_JUDGMENT, urand(7000, 10000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };  
};

class npc_scarlet_monastery_scarlet_purifier : public CreatureScript
{
    public:
        npc_scarlet_monastery_scarlet_purifier() : CreatureScript("npc_scarlet_monastery_scarlet_purifier") {}

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_monastery_scarlet_purifierAI(creature);
        }

        struct npc_scarlet_monastery_scarlet_purifierAI : public ScriptedAI
        {
            npc_scarlet_monastery_scarlet_purifierAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                events.Reset();     

                SetEquipmentSlots(false, 50181, -1, -1);
            }

            void EnterCombat(Unit* who) override
            {
                events.ScheduleEvent(EVENT_FLAMESTRIKE, urand(5000, 12000));
                events.ScheduleEvent(EVENT_PURIFYING_FLAMES, 100);
            }

            void JustDied(Unit* who) override
            {
                events.Reset();
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
                        case EVENT_PURIFYING_FLAMES:
                            DoCastVictim(Spells::SPELL_PURIFYING_FLAMES);
                            events.ScheduleEvent(EVENT_PURIFYING_FLAMES, 3000);
                            break;
                        case EVENT_FLAMESTRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, Spells::SPELL_FLAMESTRIKE);
                            events.ScheduleEvent(EVENT_FLAMESTRIKE, urand(12000, 20000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };  
};

class npc_scarlet_monastery_scarlet_zealot : public CreatureScript
{
    public:
        npc_scarlet_monastery_scarlet_zealot() : CreatureScript("npc_scarlet_monastery_scarlet_zealot") {}

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_monastery_scarlet_zealotAI(creature);
        }

        struct npc_scarlet_monastery_scarlet_zealotAI : public ScriptedAI
        {
            npc_scarlet_monastery_scarlet_zealotAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                events.Reset();     

            }

            void EnterCombat(Unit* who) override
            {
                events.ScheduleEvent(EVENT_SMITE, 100);
                events.ScheduleEvent(EVENT_HEAL, urand(5000, 10000));
                //events.ScheduleEvent(EVENT_RESURRECTION)
            }

            void JustDied(Unit* who) override
            {
                events.Reset();
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
                        case EVENT_SMITE:
                            DoCastVictim(Spells::SPELL_SMITE);
                            events.ScheduleEvent(EVENT_SMITE, 1500);
                            break;
                        case EVENT_HEAL:
                            if (Unit* target = me->SelectNearbyAlly(me, 30.0f, true))
                                DoCast(target, Spells::SPELL_HEAL);
                            events.ScheduleEvent(EVENT_HEAL, urand(10000, 12000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };  
};

class npc_scarlet_monastery_scarlet_centurion : public CreatureScript
{
    public:
        npc_scarlet_monastery_scarlet_centurion() : CreatureScript("npc_scarlet_monastery_scarlet_centurion") {}

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_scarlet_monastery_scarlet_centurionAI(p_Creature);
        }

        struct npc_scarlet_monastery_scarlet_centurionAI : public ScriptedAI
        {
            npc_scarlet_monastery_scarlet_centurionAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
            }

            void Reset() override
            {
                events.Reset();     
            }

            void EnterCombat(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 7000));
                events.ScheduleEvent(EVENT_RETALIATION, urand(6000, 7000));
            }

            void JustDied(Unit* who) override
            {
                events.Reset();
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
                        case EVENT_CLEAVE:
                            DoCastVictim(Spells::SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(6000, 8000));
                            break;
                        case EVENT_RETALIATION:
                            DoCast(me, Spells::SPELL_RETALIATION);
                            events.ScheduleEvent(EVENT_RETALIATION, urand(15000, 20000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };  
};

class npc_scarlet_monastery_scarlet_flamethrower : public CreatureScript
{
    public:
        npc_scarlet_monastery_scarlet_flamethrower() : CreatureScript("npc_scarlet_monastery_scarlet_flamethrower") {}

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_scarlet_monastery_scarlet_flamethrowerAI(p_Creature);
        }

        struct npc_scarlet_monastery_scarlet_flamethrowerAI : public ScriptedAI
        {
            npc_scarlet_monastery_scarlet_flamethrowerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
            }

            void Reset() override
            {
                events.Reset();     
            }

            void EnterCombat(Unit* p_Who) override
            {
                events.ScheduleEvent(Events::EVENT_FLAMETHROWER, urand(3000, 8000));
            }

            void JustDied(Unit* p_Who) override
            {
                events.Reset();
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (uint32 l_EventId = events.ExecuteEvent())
                {
                    switch (l_EventId)
                    {
                        case Events::EVENT_FLAMETHROWER:
                            DoCastVictim(Spells::SPELL_FLAMETHROWER);
                            events.ScheduleEvent(Events::EVENT_FLAMETHROWER, urand(8000, 12000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };  
};

class npc_scarlet_monastery_fuel_tank : public CreatureScript
{
    public:
        npc_scarlet_monastery_fuel_tank() : CreatureScript("npc_scarlet_monastery_fuel_tank") {}

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_scarlet_monastery_fuel_tankAI(p_Creature);
        }

        struct npc_scarlet_monastery_fuel_tankAI : public Scripted_NoMovementAI
        {
            npc_scarlet_monastery_fuel_tankAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->AddAura(Spells::SPELL_FUEL_BARREL, me);

                m_isExplode = false;
            }

            void DamageTaken(Unit* p_Who, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_isExplode)
                {
                    m_isExplode = true;
                    DoCast(me, Spells::SPELL_BARREL_EXPLOSION, true);
                }
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                
            }

        private:
            bool m_isExplode = false;
        };  
};

class npc_scarlet_monastery_pile_of_corpses : public CreatureScript
{
    public:
        npc_scarlet_monastery_pile_of_corpses() : CreatureScript("npc_scarlet_monastery_pile_of_corpses") {}

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_scarlet_monastery_pile_of_corpsesAI(p_Creature);
        }

        struct npc_scarlet_monastery_pile_of_corpsesAI : public Scripted_NoMovementAI
        {
            npc_scarlet_monastery_pile_of_corpsesAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->AddAura(Spells::SPELL_PILE_OF_CORPSES, me);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAura(Spells::SPELL_PILE_OF_CORPSES);
            }
        };
};

#ifndef __clang_analyzer__
void AddSC_scarlet_monastery()
{
    new npc_scarlet_monastery_scarlet_fanatic();
    new npc_scarlet_monastery_scarlet_judicator();
    new npc_scarlet_monastery_scarlet_purifier();
    new npc_scarlet_monastery_scarlet_zealot();
    new npc_scarlet_monastery_scarlet_centurion();
    new npc_scarlet_monastery_scarlet_flamethrower();
    new npc_scarlet_monastery_fuel_tank();
    new npc_scarlet_monastery_pile_of_corpses();
}
#endif
