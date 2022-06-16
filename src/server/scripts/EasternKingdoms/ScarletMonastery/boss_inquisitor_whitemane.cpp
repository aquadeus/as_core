#include "scarlet_monastery.h"

enum Spells
{
    // Durand
    SPELL_FLASH_OF_STEEL = 115629,
    SPELL_DASHING_STRIKE = 115739,

    // Whitemane
    SPELL_DEEP_SLEEP     = 9256,
    SPELL_HOLY_SMITE     = 114848,
    SPELL_MASS_RES       = 113134,
    SPELL_SHIELD         = 127399,
    SPELL_SCARLET_RES    = 9232,
    SPELL_HEAL           = 12039,

    SPELL_ACHIEVEMENT    = 132022
};

enum Events
{
    EVENT_FLASH_OF_STEEL = 1,
    EVENT_DASHING_STRIKE,

    // Whitemane
    EVENT_SMITE,
    EVENT_MASS_RES,
    EVENT_SHIELD,
    EVENT_HEAL,
    EVENT_RES_DURAND
};

enum Texts
{
    SAY_INTRO_D   = 0, // To think you've come so far, only to perish here. I'll honor you with clean deaths. (27529)
    SAY_AGGRO_D   = 1, // My legend begins NOW! (27527)
    SAY_REZ_D     = 2, // At your side, milady! (5837)
    SAY_KILLER_D1 = 3, // My blade is peerless! (27530)
    SAY_KILLER_D2 = 4, // Perfect technique! (27531)
    SAY_DEATH_D   = 5, // But...my legend! (27528)

    SAY_AGGRO_W   = 0, // You shall pay for this treachery! (29616)
    SAY_REZ_W     = 1, // Arise, my champion! (5840)
    SAY_KILLER_W  = 2, // The light has spoken! (29618)
    SAY_DEATH_W   = 3, // Mograine... (29617)
};

enum Points
{
    POINT_ALTAR = 1
};

const Position altarPos = { 756.048f, 605.632f, 14.350f, 6.280777f };

class boss_inquisitor_whitemane : public CreatureScript
{
    public:
        boss_inquisitor_whitemane() : CreatureScript("boss_inquisitor_whitemane") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_inquisitor_whitemaneAI(creature);
        }

        struct boss_inquisitor_whitemaneAI : public BossAI
        {
            boss_inquisitor_whitemaneAI(Creature* creature) : BossAI(creature, DATA_WHITEMANE) { }

            void Reset() override
            {
                _Reset();
                m_ResScarlet = false;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_DEFENSIVE);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                _EnterCombat();
                Talk(SAY_AGGRO_W);
                events.ScheduleEvent(EVENT_SMITE, 5000);
                events.ScheduleEvent(EVENT_SHIELD, 8000);
                events.ScheduleEvent(EVENT_HEAL, 12000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH_W);

                instance->DoUpdateCriteria(CriteriaTypes::CRITERIA_TYPE_BE_SPELL_TARGET, Spells::SPELL_ACHIEVEMENT);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_ResScarlet && me->GetHealth() <= p_Damage)
                {
                    p_Damage = std::max((uint32)1, p_Damage - (uint32)me->GetHealth());
                }

                if (!m_ResScarlet && HealthBelowPct(40))
                {
                    me->InterruptNonMeleeSpells(false);
                    m_ResScarlet = true;
                    DoCastAOE(Spells::SPELL_DEEP_SLEEP);
                    events.ScheduleEvent(EVENT_MASS_RES, 12000);
                    events.ScheduleEvent(EVENT_RES_DURAND, 1000);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILLER_W);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type == MovementGeneratorType::POINT_MOTION_TYPE && p_Id == Points::POINT_ALTAR)
                {
                    me->GetMotionMaster()->MovementExpired();
                    me->GetMotionMaster()->Clear();

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    if (Unit* l_Target = me->SelectNearestTarget(10.f))
                        AttackStart(l_Target);
                }
            }

            void UpdateAI(uint32 const diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_SMITE:
                            DoCastVictim(Spells::SPELL_HOLY_SMITE);
                            events.ScheduleEvent(EVENT_SMITE, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                            break;
                        case EVENT_MASS_RES:
                            DoCastAOE(Spells::SPELL_MASS_RES);
                            break;
                        case EVENT_SHIELD:
                            DoCast(me, Spells::SPELL_SHIELD);
                            events.ScheduleEvent(EVENT_SHIELD, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            break;
                        case EVENT_HEAL:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                            {
                                if (roll_chance_i(50))
                                    DoCast(target, Spells::SPELL_HEAL);
                                else
                                    DoCast(me, Spells::SPELL_HEAL);
                                events.ScheduleEvent(EVENT_HEAL, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            }
                            break;
                        case EVENT_RES_DURAND:
                            if (Unit* durand = Unit::GetUnit(*me, instance->GetData64(DATA_DURAND)))
                            {
                                Talk(SAY_REZ_W);
                                DoCast(durand, Spells::SPELL_SCARLET_RES);
                            }
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        private:
            bool m_ResScarlet;
        }; 
};

class npc_inquisitor_whitemane_commander_durand : public CreatureScript
{
    public:
        npc_inquisitor_whitemane_commander_durand() : CreatureScript("npc_inquisitor_whitemane_commander_durand") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_inquisitor_whitemane_commander_durandAI(creature);
        }

        struct npc_inquisitor_whitemane_commander_durandAI : public ScriptedAI
        {
            npc_inquisitor_whitemane_commander_durandAI(Creature* creature) : ScriptedAI(creature) 
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
                pInstance = me->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
                m_IsDied = false;
                m_FakeDeath = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO_D);
                events.ScheduleEvent(EVENT_DASHING_STRIKE, 5000);
                events.ScheduleEvent(EVENT_FLASH_OF_STEEL, 20000);
                
                if (pInstance)
                    pInstance->SetBossState(DATA_WHITEMANE, IN_PROGRESS);
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();
                Talk(SAY_DEATH_D);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(RAND(SAY_KILLER_D1, SAY_KILLER_D2));
            }

            void DamageTaken(Unit* /*doneBy*/, uint32 &damage, SpellInfo const* p_SpellInfo) override
            {
                if (m_FakeDeath)
                {
                    damage = 0;
                }
                else if (!m_IsDied && me->GetHealth() <= damage)
                {
                    if (Unit* Whitemane = Unit::GetUnit(*me, pInstance->GetData64(DATA_WHITEMANE)))
                    {
                        Whitemane->GetMotionMaster()->MovePoint(Points::POINT_ALTAR, altarPos);

                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MoveIdle();

                        me->SetHealth(0);

                        if (me->IsNonMeleeSpellCasted(false))
                            me->InterruptNonMeleeSpells(false);

                        me->ClearComboPoints();
                        me->RemoveAllAuras();
                        me->ClearAllReactives();

                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->SetStandState(UNIT_STAND_STATE_DEAD);

                        m_IsDied = true;
                        m_FakeDeath = true;

                        damage = 0;
                    }
                }
            }

            void SpellHit(Unit* /*who*/, const SpellInfo* spell) override
            {
                if (spell->Id == Spells::SPELL_SCARLET_RES)
                {
                    Resurrected();
                }
            }

            void Resurrected()
            {
                m_FakeDeath = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                Talk(SAY_REZ_D);
            }

            void UpdateAI(uint32 const diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_FakeDeath)
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_DASHING_STRIKE:
                            DoCast(Spells::SPELL_FLASH_OF_STEEL);
                            events.ScheduleEvent(EVENT_DASHING_STRIKE, 35000);
                            break;
                        case EVENT_FLASH_OF_STEEL:
                            DoCast(Spells::SPELL_FLASH_OF_STEEL);
                            events.ScheduleEvent(EVENT_FLASH_OF_STEEL, 20000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        private:
            bool m_IsDied = false;
            bool m_FakeDeath = false;
            InstanceScript* pInstance;
        };
};

#ifndef __clang_analyzer__
void AddSC_boss_inquisitor_whitemane()
{
    new boss_inquisitor_whitemane();
    new npc_inquisitor_whitemane_commander_durand();
}
#endif