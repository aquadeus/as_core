////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "halls_of_valor.hpp"

enum Says
{
    SAY_UNNERVING_HOWL  = 0,
    SAY_END             = 1,
    SAY_BLOOD           = 1
};

enum Spells
{
    SPELL_UNNERVING_HOWL        = 196543,
    SPELL_RAVENOUS_LEAP         = 197558,
    SPELL_RAVENOUS_LEAP_MARK    = 197549,
    SPELL_RAVENOUS_LEAP_AURA    = 197556,
    SPELL_RAVENOUS_LEAP_T_1     = 197560,
    SPELL_RAVENOUS_LEAP_JUMP    = 196495,
    SPELL_RAVENOUS_LEAP_DOT     = 196497,
    SPELL_RAVENOUS_LEAP_AOE     = 196496,
    SPELL_CLAW_FRENZY           = 196512,
    SPELL_SCENT_OF_BLOOD        = 198800,
    SPELL_SCENT_OF_BLOOD_AURA   = 196838,
    SPELL_SCENT_OF_BLOOD_BUFF   = 196828,

    /// Heroic
    SPELL_SPAWN_WOLVES          = 207708,
    SPELL_STEALTH_WOLFS         = 207707,
    SPELL_STRENGTH_PACK         = 199186,
    SPELL_STRENGTH_PACK_MOD_DMG = 199184,

    /// Cosmetic
    SPELL_STEALTH               = 196567,
    SPELL_LICKING_WOUNDS        = 200561,
    SPELL_BLOOD_SPLAT           = 200553,

    /// Conversation
    SPELL_ODYN_HUNTING_GROUND   = 202153
};

enum eEvents
{
    EVENT_UNNERVING_HOWL        = 1,
    EVENT_RAVENOUS_LEAP         = 2,
    EVENT_CLAW_FRENZY           = 3,
    EVENT_SCENT_OF_BLOOD        = 4,
    EVENT_SPAWN_WOLVES          = 5,

    /// Other
    EVENT_LOW_HP_MOVE_LAIR      = 5
};

Position const g_OneTrashPos[12] =
{
    { 3006.39f, 2980.55f, 618.48f, 2.26f }, ///< Npc 95674
    { 3075.17f, 3008.41f, 609.65f, 5.10f }, ///< Go 246272
    { 3088.34f, 2989.26f, 614.69f, 5.63f },
    { 3016.72f, 3009.90f, 609.72f, 0.35f },
    { 3036.83f, 3021.59f, 607.72f, 0.31f },
    { 3066.19f, 3016.52f, 609.92f, 5.52f },
    { 2999.78f, 2996.95f, 615.46f, 1.04f },
    { 3048.63f, 3021.88f, 607.67f, 0.19f },
    { 3078.62f, 2995.89f, 612.31f, 5.37f },
    { 3029.34f, 3013.41f, 608.78f, 0.58f },
    { 3058.14f, 3023.87f, 607.94f, 6.06f },
    { 3009.28f, 3003.14f, 612.59f, 0.64f }
};

Position const g_SecondTrashPos[13] =
{
    { 3154.69f, 3166.24f, 593.60f, 3.29f }, ///< Npc 95674
    { 3130.42f, 3104.78f, 597.30f, 4.24f }, ///< Go 246272
    { 3088.16f, 2989.61f, 614.72f, 5.46f },
    { 3078.62f, 2996.61f, 612.20f, 5.23f },
    { 3076.25f, 3009.70f, 609.88f, 4.60f },
    { 3075.56f, 3029.13f, 606.97f, 4.30f },
    { 3093.55f, 3041.78f, 605.68f, 3.84f },
    { 3098.51f, 3055.33f, 603.27f, 4.06f },
    { 3109.60f, 3063.75f, 604.57f, 4.21f },
    { 3113.69f, 3076.36f, 602.11f, 4.15f },
    { 3126.08f, 3089.76f, 600.51f, 4.22f },
    { 3140.12f, 3117.60f, 597.83f, 4.38f },
    { 3142.36f, 3131.53f, 596.98f, 4.36f }
};

/// Fenryr - 99868
class boss_fenryr : public CreatureScript
{
    public:
        boss_fenryr() : CreatureScript("boss_fenryr") { }

        struct boss_fenryrAI : public BossAI
        {
            boss_fenryrAI(Creature* p_Creature) : BossAI(p_Creature, DATA_FENRYR)
            {
                if (instance && instance->GetData(DATA_FENRYR_EVENT) != DONE)
                {
                    me->SetVisible(false);
                    me->SetReactState(REACT_PASSIVE);
                }
            }

            bool m_Jumping;

            void Reset() override
            {
                m_Jumping = false;

                _Reset();

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAVENOUS_LEAP_DOT);

                me->DisableHealthRegen();
                me->SetHealth(me->CountPctFromMaxHealth(60));
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();

                me->SetReactState(REACT_AGGRESSIVE);

                events.ScheduleEvent(EVENT_UNNERVING_HOWL, 3000);
                events.ScheduleEvent(EVENT_RAVENOUS_LEAP, 9000);
                events.ScheduleEvent(EVENT_CLAW_FRENZY, 20000);
                events.ScheduleEvent(EVENT_SCENT_OF_BLOOD, 23000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_SPAWN_WOLVES, 4000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SKINNABLE);

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAVENOUS_LEAP_DOT);

                if (IsMythic())
                    me->SummonCreature(NPC_FENRYR_TAMEABLE, g_FenryrTameablePos);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE && p_ID == SPELL_RAVENOUS_LEAP_JUMP)
                {
                    m_Jumping = false;

                    me->GetMotionMaster()->Clear(false);

                    DoCast(me, SPELL_RAVENOUS_LEAP_T_1, true);
                    DoCast(me, SPELL_RAVENOUS_LEAP_AOE, true);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        if (!m_Jumping && me->getVictim())
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_RAVENOUS_LEAP_T_1:
                    {
                        m_Jumping = true;

                        p_Target->RemoveAurasDueToSpell(SPELL_RAVENOUS_LEAP_AURA);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(*p_Target, 40.0f, 10.0f, SPELL_RAVENOUS_LEAP_JUMP);
                        break;
                    }
                    case SPELL_RAVENOUS_LEAP_AOE:
                    {
                        DoCast(p_Target, SPELL_RAVENOUS_LEAP_DOT, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustReachedHome() override
            {
                me->SetByteFlag(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_STAND_STATE, UNIT_STAND_FLAGS_CREEP | UNIT_STAND_FLAGS_UNK1);
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                me->CastSpell(me, SPELL_LICKING_WOUNDS, false);

                me->SetByteFlag(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_STAND_STATE, UNIT_STAND_FLAGS_CREEP | UNIT_STAND_FLAGS_UNK1);
            }

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_DamageType, SpellInfo const* p_SpellInfo) override
            {
                if (!p_Victim->HasAura(SPELL_SCENT_OF_BLOOD_AURA) || p_SpellInfo || p_Damage == 0)
                    return;

                me->CastSpell(me, SPELL_SCENT_OF_BLOOD_BUFF, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_SCENT_OF_BLOOD_AURA))
                {
                    if (me->getVictim() && me->getVictim()->HasAura(SPELL_SCENT_OF_BLOOD_AURA))
                    {
                        DoMeleeAttackIfReady();
                        return;
                    }

                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_UNNERVING_HOWL:
                    {
                        Talk(SAY_UNNERVING_HOWL);
                        DoCast(SPELL_UNNERVING_HOWL);
                        events.ScheduleEvent(EVENT_UNNERVING_HOWL, 38000);

                        if ((events.GetEventTime(EVENT_RAVENOUS_LEAP) - events.GetTimer()) < 3 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_RAVENOUS_LEAP, 3 * TimeConstants::IN_MILLISECONDS);

                        if ((events.GetEventTime(EVENT_CLAW_FRENZY) - events.GetTimer()) < 3 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_CLAW_FRENZY, 3 * TimeConstants::IN_MILLISECONDS);

                        if ((events.GetEventTime(EVENT_SCENT_OF_BLOOD) - events.GetTimer()) < 3 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_SCENT_OF_BLOOD, 3 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case EVENT_RAVENOUS_LEAP:
                    {
                        CustomSpellValues l_Values;
                        l_Values.AddSpellMod(SpellValueMod::SPELLVALUE_MAX_TARGETS, 3);

                        me->CastCustomSpell(SPELL_RAVENOUS_LEAP_MARK, l_Values, me, true);
                        DoCast(SPELL_RAVENOUS_LEAP);
                        events.ScheduleEvent(EVENT_RAVENOUS_LEAP, 40000);

                        if ((events.GetEventTime(EVENT_CLAW_FRENZY) - events.GetTimer()) < 10 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_CLAW_FRENZY, urand(10, 20) * TimeConstants::IN_MILLISECONDS);

                        if ((events.GetEventTime(EVENT_SCENT_OF_BLOOD) - events.GetTimer()) < 10 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_SCENT_OF_BLOOD, 10 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case EVENT_CLAW_FRENZY:
                    {
                        DoCast(SPELL_CLAW_FRENZY);
                        events.ScheduleEvent(EVENT_CLAW_FRENZY, urand(10, 20) * IN_MILLISECONDS);

                        if ((events.GetEventTime(EVENT_RAVENOUS_LEAP) - events.GetTimer()) < 2 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_RAVENOUS_LEAP, 2 * TimeConstants::IN_MILLISECONDS);

                        if ((events.GetEventTime(EVENT_SCENT_OF_BLOOD) - events.GetTimer()) < 2 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_SCENT_OF_BLOOD, 2 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case EVENT_SCENT_OF_BLOOD:
                    {
                        DoCast(me, SPELL_SCENT_OF_BLOOD, true);
                        events.ScheduleEvent(EVENT_SCENT_OF_BLOOD, 38000);

                        if ((events.GetEventTime(EVENT_RAVENOUS_LEAP) - events.GetTimer()) < 15 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_RAVENOUS_LEAP, 15 * TimeConstants::IN_MILLISECONDS);

                        if ((events.GetEventTime(EVENT_CLAW_FRENZY) - events.GetTimer()) < 15 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_CLAW_FRENZY, urand(15, 20) * TimeConstants::IN_MILLISECONDS);

                        if ((events.GetEventTime(EVENT_UNNERVING_HOWL) - events.GetTimer()) < 15 * TimeConstants::IN_MILLISECONDS) ///< Avoid simultaneous casts of spells that ask you to do different stuff in order to survive
                            events.RescheduleEvent(EVENT_UNNERVING_HOWL, 20 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case EVENT_SPAWN_WOLVES:
                    {
                        Position l_Pos;
                        for (uint8 l_I = 0; l_I < 3; l_I++)
                        {
                            me->GetNearPosition(l_Pos, 40.0f, l_I);
                            me->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), SPELL_SPAWN_WOLVES, true);
                        }

                        events.ScheduleEvent(EVENT_SPAWN_WOLVES, 36000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_fenryrAI(p_Creature);
        }
};

/// Fenryr - 95674
class npc_fenryr : public CreatureScript
{
    public:
        npc_fenryr() : CreatureScript("npc_fenryr") { }

        struct npc_fenryrAI : public ScriptedAI
        {
            npc_fenryrAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                m_LooseFight = false;
            }

            InstanceScript* m_Instance;
            EventMap m_Events;
            bool m_LooseFight;

            bool m_Jumping;

            void Reset() override
            {
                m_Jumping = false;

                m_Events.Reset();

                if (m_Instance)
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAVENOUS_LEAP_DOT);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoZoneInCombat();

                m_Events.ScheduleEvent(EVENT_UNNERVING_HOWL, 7000);
                m_Events.ScheduleEvent(EVENT_RAVENOUS_LEAP, 12000);
                m_Events.ScheduleEvent(EVENT_CLAW_FRENZY, 20000);
            }

            void SummonTraces()
            {
                if (me->GetDistance(g_OneTrashPos[0]) < me->GetDistance(g_SecondTrashPos[0]))
                {
                    for (uint8 l_I = 1; l_I < 12; l_I++)
                        me->SummonGameObject(GO_TRACES_OF_FENRIR, g_OneTrashPos[l_I], 0, 0, 0, 0, 1000);
                }
                else
                {
                    for (uint8 l_I = 1; l_I < 13; l_I++)
                        me->SummonGameObject(GO_TRACES_OF_FENRIR, g_SecondTrashPos[l_I], 0, 0, 0, 0, 1000);
                }

                std::list<Creature*> l_TrashList;
                GetCreatureListWithEntryInGrid(l_TrashList, me, NPC_GILDEFUR_STAG, 30.0f);
                GetCreatureListWithEntryInGrid(l_TrashList, me, 96611, 30.0f);
                GetCreatureListWithEntryInGrid(l_TrashList, me, 103801, 30.0f);
                GetCreatureListWithEntryInGrid(l_TrashList, me, 96640, 30.0f);
                GetCreatureListWithEntryInGrid(l_TrashList, me, 96934, 30.0f);
                GetCreatureListWithEntryInGrid(l_TrashList, me, 99804, 30.0f);
                GetCreatureListWithEntryInGrid(l_TrashList, me, 96677, 30.0f);

                for (Creature* l_Iter : l_TrashList)
                {
                    l_Iter->CastSpell(l_Iter, 29266, true); ///< Fake Death
                    l_Iter->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(60, p_Damage) && !m_LooseFight)
                {
                    me->GetMotionMaster()->Clear(false);
                    me->InterruptNonMeleeSpells(true);
                    m_LooseFight = true;
                    m_Events.Reset();
                    DoStopAttack();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    m_Events.ScheduleEvent(EVENT_LOW_HP_MOVE_LAIR, 500);

                    if (m_Instance)
                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAVENOUS_LEAP_DOT);
                }

                if (m_LooseFight)
                    p_Damage = 0;
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                SummonTraces();
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    if (p_ID == SPELL_RAVENOUS_LEAP_JUMP)
                    {
                        m_Jumping = false;

                        me->GetMotionMaster()->Clear(false);

                        DoCast(me, SPELL_RAVENOUS_LEAP_T_1, true);
                        DoCast(me, SPELL_RAVENOUS_LEAP_AOE, true);

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            if (!m_Jumping && me->getVictim())
                                me->GetMotionMaster()->MoveChase(me->getVictim());
                        });
                    }
                }

                if (p_Type == POINT_MOTION_TYPE)
                {
                    if (p_ID == 1)
                        me->DespawnOrUnsummon();
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_RAVENOUS_LEAP_T_1:
                    {
                        m_Jumping = true;

                        p_Target->RemoveAurasDueToSpell(SPELL_RAVENOUS_LEAP_AURA);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(*p_Target, 40.0f, 10.0f, SPELL_RAVENOUS_LEAP_JUMP);
                        break;
                    }
                    case SPELL_RAVENOUS_LEAP_AOE:
                    {
                        DoCast(p_Target, SPELL_RAVENOUS_LEAP_DOT, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!m_LooseFight)
                {
                    if (me->GetDistance(me->GetHomePosition()) >= 50.0f)
                    {
                        EnterEvadeMode();
                        return;
                    }
                }

                switch (m_Events.ExecuteEvent())
                {
                    case EVENT_UNNERVING_HOWL:
                    {
                        Talk(SAY_UNNERVING_HOWL);
                        DoCast(SPELL_UNNERVING_HOWL);
                        m_Events.ScheduleEvent(EVENT_UNNERVING_HOWL, 32000);
                        break;
                    }
                    case EVENT_RAVENOUS_LEAP:
                    {
                        CustomSpellValues l_Values;
                        l_Values.AddSpellMod(SpellValueMod::SPELLVALUE_MAX_TARGETS, 2);

                        me->CastCustomSpell(SPELL_RAVENOUS_LEAP_MARK, l_Values, me, true);
                        DoCast(SPELL_RAVENOUS_LEAP);
                        m_Events.ScheduleEvent(EVENT_RAVENOUS_LEAP, 34000);

                        if ((m_Events.GetEventTime(EVENT_CLAW_FRENZY) - m_Events.GetTimer()) < 3 * TimeConstants::IN_MILLISECONDS) ///< Avoid Casts of Leap into claw, when the first spell wants you to spread, and the second splits damage
                        {
                            m_Events.CancelEvent(EVENT_CLAW_FRENZY);
                            m_Events.ScheduleEvent(EVENT_CLAW_FRENZY, 3 * TimeConstants::IN_MILLISECONDS);
                        }
                        break;
                    }
                    case EVENT_CLAW_FRENZY:
                    {
                        DoCast(SPELL_CLAW_FRENZY);
                        m_Events.ScheduleEvent(EVENT_CLAW_FRENZY, 10000);

                        if ((m_Events.GetEventTime(EVENT_RAVENOUS_LEAP) - m_Events.GetTimer()) < 3 * TimeConstants::IN_MILLISECONDS) ///< Avoid Casts of Leap into claw, when the first spell wants you to spread, and the second splits damage
                        {
                            m_Events.CancelEvent(EVENT_RAVENOUS_LEAP);
                            m_Events.ScheduleEvent(EVENT_RAVENOUS_LEAP, 3 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case EVENT_LOW_HP_MOVE_LAIR:
                    {
                        DoCast(me, SPELL_LICKING_WOUNDS, true);
                        DoCast(me, SPELL_BLOOD_SPLAT, true);
                        DoCast(me, SPELL_STEALTH, true);

                        Talk(SAY_END);

                        if (m_Instance && m_Instance->GetData(DATA_FENRYR_EVENT) != DONE)
                            m_Instance->SetData(DATA_FENRYR_EVENT, DONE);

                        if (me->GetDistance(g_OneTrashPos[0]) < me->GetDistance(g_SecondTrashPos[0]))
                            me->GetMotionMaster()->MovePoint(1, 2991.90f, 3071.94f, 605.72f);
                        else
                            me->GetMotionMaster()->MovePoint(1, 3100.44f, 3216.60f, 593.64f);

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fenryrAI(p_Creature);
        }
};

/// Ebonclaw Packmate <Fenryr's Pack> - 99922
class npc_fenryr_ebonclaw_worg : public CreatureScript
{
    public:
        npc_fenryr_ebonclaw_worg() : CreatureScript("npc_fenryr_ebonclaw_worg") { }

        struct npc_fenryr_ebonclaw_worgAI : public ScriptedAI
        {
            npc_fenryr_ebonclaw_worgAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint16 m_CheckStealthTimer;

            void Reset() override
            {
                m_CheckStealthTimer = 1000;

                DoCast(me, SPELL_STRENGTH_PACK, true);
                DoCast(me, SPELL_STEALTH_WOLFS, true);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetSpeed(MOVE_RUN, 0.4f);
                DoZoneInCombat(me, 100.0f);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_CheckStealthTimer)
                {
                    if (m_CheckStealthTimer <= p_Diff)
                    {
                        if (me->HasAura(SPELL_STEALTH_WOLFS))
                            m_CheckStealthTimer = 1000;
                        else
                        {
                            me->SetSpeed(MOVE_RUN, 1.0f);
                            m_CheckStealthTimer = 0;
                        }
                    }
                    else
                        m_CheckStealthTimer -= p_Diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fenryr_ebonclaw_worgAI(p_Creature);
        }
};

/// Halls of Valor - 101712
class npc_trigger_hov_conversation : public CreatureScript
{
    public:
        npc_trigger_hov_conversation() : CreatureScript("npc_trigger_hov_conversation") { }

        struct npc_trigger_hov_conversationAI : public ScriptedAI
        {
            npc_trigger_hov_conversationAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_IntroDone = false;
            }

            bool m_IntroDone;

            void Reset() override
            {
                DoCast(me, 200611, true);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 45.0f))
                {
                    m_IntroDone = true;

                    if (Player* l_Target = me->FindNearestPlayer(30.0f))
                        l_Target->CastSpell(l_Target, SPELL_ODYN_HUNTING_GROUND, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_trigger_hov_conversationAI (p_Creature);
        }
};

/// Scent of Blood - 198800
class spell_fenryr_scent_of_blood_filter : public SpellScriptLoader
{
    public:
        spell_fenryr_scent_of_blood_filter() : SpellScriptLoader("spell_fenryr_scent_of_blood_filter") { }

        class spell_fenryr_scent_of_blood_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fenryr_scent_of_blood_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!GetCaster() || p_Targets.empty())
                    return;

                if (p_Targets.size() > 1)
                {
                    if (GetCaster()->getVictim())
                        p_Targets.remove(GetCaster()->getVictim());

                    JadeCore::Containers::RandomResizeList(p_Targets, 1);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fenryr_scent_of_blood_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fenryr_scent_of_blood_filter_SpellScript();
        }
};

/// Scent of Blood (fixate) - 196838
class spell_fenryr_scent_of_blood_fixate : public SpellScriptLoader
{
    public:
        spell_fenryr_scent_of_blood_fixate() : SpellScriptLoader("spell_fenryr_scent_of_blood_fixate") { }

        class spell_fenryr_scent_of_blood_fixate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fenryr_scent_of_blood_fixate_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Fenryr = GetCaster()->ToCreature())
                {
                    l_Fenryr->SetReactState(REACT_PASSIVE);
                    l_Fenryr->AttackStop();
                    l_Fenryr->ClearUnitState(UNIT_STATE_CASTING);

                    if (GetTarget() && l_Fenryr->IsAIEnabled)
                    {
                        l_Fenryr->AI()->AttackStart(GetTarget());

                        if (Player* l_Player = GetTarget()->ToPlayer())
                            l_Fenryr->AI()->Talk(SAY_BLOOD, l_Player->GetGUID());
                    }
                }
            }

            void AfterRemove(AuraEffect const * /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Fenryr = GetCaster()->ToCreature())
                {
                    l_Fenryr->RemoveAurasDueToSpell(SPELL_SCENT_OF_BLOOD_BUFF);
                    l_Fenryr->SetReactState(REACT_AGGRESSIVE);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_fenryr_scent_of_blood_fixate_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_FIXATE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_fenryr_scent_of_blood_fixate_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_FIXATE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fenryr_scent_of_blood_fixate_AuraScript();
        }
};

/// Strength of the Pack (searcher) - 200922
class spell_fenryr_strength_of_the_pack : public SpellScriptLoader
{
    public:
        spell_fenryr_strength_of_the_pack() : SpellScriptLoader("spell_fenryr_strength_of_the_pack") { }

        class spell_fenryr_strength_of_the_pack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fenryr_strength_of_the_pack_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                {
                    if (Unit* l_Caster = GetCaster())
                        l_Caster->RemoveAura(SPELL_STRENGTH_PACK_MOD_DMG);

                    return;
                }

                if (Unit* l_Caster = GetCaster())
                {
                    if (Aura* l_Aura = l_Caster->GetAura(SPELL_STRENGTH_PACK_MOD_DMG))
                    {
                        l_Aura->SetStackAmount(p_Targets.size());
                        l_Aura->RefreshDuration();
                    }
                    else
                    {
                        for (uint32 l_I = 0; l_I < uint32(p_Targets.size()); ++l_I)
                            l_Caster->CastSpell(l_Caster, SPELL_STRENGTH_PACK_MOD_DMG, true);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fenryr_strength_of_the_pack_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fenryr_strength_of_the_pack_SpellScript();
        }
};

/// Teleport to Fenryr's Lair
class eventobject_fenryr_teleporter_in : public EventObjectScript
{
    public:
        eventobject_fenryr_teleporter_in() : EventObjectScript("eventobject_fenryr_teleporter_in") { }

        bool OnTrigger(Player* p_Player, EventObject* /*p_EventObject*/) override
        {
            p_Player->NearTeleportTo({ 3189.92f, 2899.97f, 639.454f, 2.6707f });
            return true;
        }
};

/// Teleport from Fenryr's Lair
class eventobject_fenryr_teleporter_out : public EventObjectScript
{
    public:
        eventobject_fenryr_teleporter_out() : EventObjectScript("eventobject_fenryr_teleporter_out") { }

        bool OnTrigger(Player* p_Player, EventObject* /*p_EventObject*/) override
        {
            p_Player->NearTeleportTo({ 3245.144f, 638.563f, 634.50f, 5.502f });
            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_fenryr()
{
    /// Boss
    new boss_fenryr();

    /// Creatures
    new npc_fenryr();
    new npc_fenryr_ebonclaw_worg();
    new npc_trigger_hov_conversation();

    /// Spells
    new spell_fenryr_scent_of_blood_filter();
    new spell_fenryr_scent_of_blood_fixate();
    new spell_fenryr_strength_of_the_pack();

    /// EventObjects
    new eventobject_fenryr_teleporter_in();
    new eventobject_fenryr_teleporter_out();
}
#endif
