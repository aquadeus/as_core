////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "vault_of_the_wardens.hpp"

enum Says
{
    SAY_AGGRO,
    SAY_DARKSTRIKES,
    SAY_DARKSTRIKES_WARN,
    SAY_FURIOUS_BLAST,
    SAY_FURIOUS_BLAST_WARN,
    SAY_VENGEANCE,
    SAY_HAVOC,
    SAY_SLAY,
    SAY_DEATH,
    SAY_LOW_HEALTH,
    SAY_HATRED
};

enum Spells
{
    SPELL_DARKSTRIKES               = 191941,
    SPELL_DARKSTRIKES_DMG_MH        = 191948,
    SPELL_DARKSTRIKES_DMG_OH        = 191949,
    SPELL_DARK_ENERGIES             = 191999,
    SPELL_SWOOP                     = 191765,
    SPELL_SWOOP_JUMP                = 191766,
    SPELL_SWOOP_DAMAGE              = 191767,
    SPELL_FURIOUS_BLAST             = 191823,
    SPELL_FURIOUS_FLAMES_AT         = 202919,

    /// Second Phase 50% HP
    SPELL_METAMORPHOSIS_VENGEANCE   = 202740,
    SPELL_METAMORPHOSIS_HAVOC       = 192504,
    SPELL_FEL_MORTAR                = 202913,
    SPELL_FEL_MORTAR_MISSILE        = 202920,

    /// Hatred
    SPELL_SUMMON_LASER_BEAM         = 202780,
    SPELL_HATRED_CHANNELED          = 190830,
    SPELL_HATRED_VISUAL             = 190833,
    SPELL_HATRED_AOE_DAMAGE         = 190836,
    SPELL_HATRED_DAMAGE             = 202862,

    /// Throw Glaive
    SPELL_THROW_GLAIVE_TRIGGERED    = 192014,
    SPELL_THROW_GLAIVE_VISUAL       = 204251,
    SPELL_THROW_GLAIVE_MISSILE      = 192027,
    SPELL_THROW_GLAIVE_AT_01        = 192028,
    SPELL_THROW_GLAIVE_AT_02        = 192092,
    SPELL_FEL_CHAIN_PERIODIC        = 202825,
    SPELL_FEL_CHAIN_DOT             = 214625,

    /// Other
    SPELL_JUMP_DOWN                 = 199132,
    SPELL_DUAL_WIELD                = 42459,
    SPELL_SUMMON_FEL_FURY           = 212558,
    SPELL_SCORCH                    = 212541,
    SPELL_FEL_DETONATION            = 212551,
    SPELL_FEL_EMISSION              = 212547,

    /// Glayvianna
    SPELL_DOUBLE_STRIKE             = 193607,
    SPELL_METAMORPHOSIS             = 193502,
    SPELL_IMMOLATION_AURA           = 193609,
    SPELL_FEL_SEED                  = 193615,
    SPELL_FEL_SEED_MISSILE          = 193613,
    SPELL_TRASH_THROW_GLAIVE        = 193559,
    SPELL_TRASH_THROW_GLAIVE_2      = 193578,
    SPELL_INFUSION                  = 204879,
    SPELL_DEMON_HUNTER_WINGS        = 200888,
    SPELL_GLAYVIANNA_DUAL_WIELD     = 42459
};

enum eEvents
{
    EVENT_DARKSTRIKES       = 1,
    EVENT_SWOOP             = 2,
    EVENT_METAMORPHOSIS     = 3,
    EVENT_FEL_MORTAR        = 4,
    EVENT_HATRED            = 5,
    EVENT_FEL_FURY          = 6
};

/// Tirathon Saltheril - 95885
class boss_tirathon_saltheril : public CreatureScript
{
    public:
        boss_tirathon_saltheril() : CreatureScript("boss_tirathon_saltheril") { }

        struct boss_tirathon_saltherilAI : public BossAI
        {
            boss_tirathon_saltherilAI(Creature* p_Creature) : BossAI(p_Creature, DATA_SALTHERIL)
            {
                m_IntroDone = false;
                m_Timer = 0;
                m_Phase = 0;
                m_IntroScene = false;
            }

            bool m_IntroScene;
            bool m_IntroDone;
            bool m_PhaseSecond;
            bool m_PhaseThird;
            bool m_LowHpText;
            uint32 m_Timer, m_Phase;
            uint8 m_FelFuryCount;

            std::set<uint64> m_FelFuryAffectedATs;

            void Reset() override
            {
                SetCanSeeEvenInPassiveMode(true);

                _Reset();
                m_PhaseSecond = false;
                m_PhaseThird = false;
                m_LowHpText = false;
                m_FelFuryCount = 0;
                DoCast(me, SPELL_DUAL_WIELD, true);
                me->RemoveAreaTrigger(SPELL_FURIOUS_BLAST);

                m_FelFuryAffectedATs.clear();

                if (m_IntroDone)
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                    });
                }
                else
                {
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();

                events.ScheduleEvent(EVENT_DARKSTRIKES, 16000);
                events.ScheduleEvent(EVENT_SWOOP, 22000);
                events.ScheduleEvent(EVENT_FEL_MORTAR, 10000);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_FEL_FURY, 30000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();
                me->RemoveAreaTrigger(SPELL_FURIOUS_BLAST);

                if (Creature* l_Drelanim = me->FindNearestCreature(NPC_DRELANIM_WHISPERWIND, 100.0f))
                {
                    if (l_Drelanim->IsAIEnabled)
                        l_Drelanim->AI()->DoAction(2);
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(SAY_SLAY);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 118.0f))
                {
                    m_IntroDone = true;
                    m_Timer = 5000;
                    m_Phase = 0;
                    m_IntroScene = true;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    switch (p_ID)
                    {
                        case SPELL_SWOOP_JUMP:
                        {
                            DoCast(SPELL_FURIOUS_BLAST);

                            Talk(SAY_FURIOUS_BLAST);
                            Talk(SAY_FURIOUS_BLAST_WARN);
                            break;
                        }
                        case SPELL_JUMP_DOWN:
                        {
                            AddTimedDelayedOperation(10, [this]() -> void
                            {
                                me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 333);
                            });

                            AddTimedDelayedOperation(2000, [this]() -> void
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                            });

                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0:
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 559, me, nullptr, *me))
                            delete l_Conversation;

                        uint32 l_Time = 6 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 866, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        l_Time += 9 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 867, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        l_Time += 14 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 868, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        l_Time += 13 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 869, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        l_Time += 29 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 870, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        l_Time += 4 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 871, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        l_Time += 14 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 872, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        l_Time += 14 * TimeConstants::IN_MILLISECONDS;
                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            Conversation* l_Conversation = new Conversation;
                            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 873, me, nullptr, *me))
                                delete l_Conversation;
                        });

                        break;
                    }
                    case ACTION_1:
                    {
                        me->InterruptNonMeleeSpells(false);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(66, p_Damage) && !m_PhaseSecond)
                {
                    m_PhaseSecond = true;
                    events.ScheduleEvent(EVENT_METAMORPHOSIS, 0);
                }
                else if (me->HealthBelowPctDamaged(40, p_Damage) && !m_PhaseThird)
                {
                    m_PhaseThird = true;
                    events.ScheduleEvent(EVENT_METAMORPHOSIS, 0);
                }
                else if (me->HealthBelowPctDamaged(10, p_Damage) && !m_LowHpText)
                {
                    m_LowHpText = true;
                    Talk(SAY_LOW_HEALTH);
                }
            }

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_DamageType, SpellInfo const* p_SpellInfo) override
            {
                if (!p_Victim || !p_SpellInfo)
                    return;

                if (p_SpellInfo->Id == SPELL_DARKSTRIKES_DMG_MH || p_SpellInfo->Id == SPELL_DARKSTRIKES_DMG_OH)
                {
                    if (!p_Victim->IsActiveSaveAbility())
                    {
                        int32 l_BP = p_Damage;

                        if (AuraEffect const* l_Absorb = me->GetAuraEffect(SPELL_DARK_ENERGIES, EFFECT_0))
                            l_BP += l_Absorb->GetAmount();

                        me->CastCustomSpell(me, SPELL_DARK_ENERGIES, &l_BP, nullptr, nullptr, true);
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_FEL_MORTAR:
                    {
                        Position l_Pos;

                        p_Target->GetRandomNearPosition(l_Pos, 5.0f);
                        me->CastSpell(l_Pos, SPELL_FEL_MORTAR_MISSILE, true);
                        break;
                    }
                    case SPELL_SWOOP:
                    {
                        DoCast(p_Target, SPELL_SWOOP_JUMP);
                        break;
                    }
                    case SPELL_SWOOP_DAMAGE:
                    {
                        Position l_Pos = p_Target->GetPosition();
                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 15.0f, me->GetAngle(p_Target));

                        p_Target->KnockbackFrom(l_Pos, 10.0, 10.0f);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_THROW_GLAIVE_TRIGGERED:
                    {
                        DoCast({ 4301.258f, -458.2631f, 260.0187f, 5.847115f }, SPELL_THROW_GLAIVE_VISUAL, true);
                        break;
                    }
                    case SPELL_THROW_GLAIVE_MISSILE:
                    {
                        me->SummonCreature(NPC_GLAIVE_01, { 4301.81f, -458.2235f, 261.0006f, 0.06539254f });

                        AddTimedDelayedOperation(1100, [this]() -> void
                        {
                            me->SummonCreature(NPC_GLAIVE_02, { 4301.007f, -448.6829f, 261.4007f, 6.178479f });
                        });

                        break;
                    }
                    case SPELL_METAMORPHOSIS_VENGEANCE:
                    {
                        me->RemoveAura(SPELL_METAMORPHOSIS_HAVOC);

                        events.CancelEvent(EVENT_SWOOP);
                        events.CancelEvent(EVENT_HATRED);

                        events.RescheduleEvent(EVENT_FEL_MORTAR, 10000);
                        break;
                    }
                    case SPELL_METAMORPHOSIS_HAVOC:
                    {
                        me->RemoveAura(SPELL_METAMORPHOSIS_VENGEANCE);

                        events.CancelEvent(EVENT_FEL_MORTAR);

                        events.RescheduleEvent(EVENT_HATRED, 24000);
                        events.RescheduleEvent(EVENT_SWOOP, 22000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_IntroScene)
                {
                    switch (m_Phase)
                    {
                        case 0:
                        {
                            if (m_Timer < p_Diff)
                            {
                                DoCast(SPELL_JUMP_DOWN);
                                m_Phase++;
                                m_Timer = 2000;
                            }
                            else
                                m_Timer -= p_Diff;

                            break;
                        }
                        case 1:
                        {
                            if (m_Timer < p_Diff)
                            {
                                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                                m_Phase++;
                                m_IntroScene = false;

                                if (Creature* l_Drelanim = me->FindNearestCreature(NPC_DRELANIM_WHISPERWIND, 100.0f, true))
                                {
                                    if (l_Drelanim->IsAIEnabled)
                                        l_Drelanim->AI()->DoAction(1);
                                }
                            }
                            else
                                m_Timer -= p_Diff;

                            break;
                        }
                        default:
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_DARKSTRIKES:
                    {
                        DoCast(SPELL_DARKSTRIKES);

                        Talk(SAY_DARKSTRIKES);

                        if (Unit* l_Target = me->getVictim())
                            Talk(SAY_DARKSTRIKES_WARN, l_Target->GetGUID());

                        events.ScheduleEvent(EVENT_DARKSTRIKES, 60000);
                        break;
                    }
                    case EVENT_SWOOP:
                    {
                        DoCast(SPELL_SWOOP);
                        events.ScheduleEvent(EVENT_SWOOP, 30000);
                        break;
                    }
                    case EVENT_METAMORPHOSIS:
                    {
                        /// First transform, make it random
                        if (!m_PhaseThird && m_PhaseSecond)
                        {
                            /// Vengeance Tirathon retains the Fel Mortar ability from phase 1
                            if (urand(0, 1))
                                TriggerVengeanceForm();
                            /// Havoc Tirathon retains the Swoop and Furious Blast mechanics from phase 1
                            else
                                TriggerHavocForm();
                        }
                        /// Second transform, choose the other one
                        else
                        {
                            if (me->HasAura(SPELL_METAMORPHOSIS_HAVOC))
                                TriggerVengeanceForm();
                            else
                                TriggerHavocForm();
                        }

                        break;
                    }
                    case EVENT_FEL_MORTAR:
                    {
                        DoCast(SPELL_FEL_MORTAR);
                        events.ScheduleEvent(EVENT_FEL_MORTAR, 16000);
                        break;
                    }
                    case EVENT_HATRED:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            DoCast(l_Target, SPELL_SUMMON_LASER_BEAM, true);

                        events.ScheduleEvent(EVENT_HATRED, 30000);
                        break;
                    }
                    case EVENT_FEL_FURY:
                    {
                        events.ScheduleEvent(EVENT_FEL_FURY, 30000 - (m_FelFuryCount * 5000));

                        std::list<AreaTrigger*> l_ATList;

                        me->GetAreaTriggerList(l_ATList, SPELL_FURIOUS_FLAMES_AT);
                        me->GetAreaTriggerList(l_ATList, SPELL_FURIOUS_BLAST);

                        for (AreaTrigger* l_AT : l_ATList)
                        {
                            if (m_FelFuryAffectedATs.find(l_AT->GetGUID()) != m_FelFuryAffectedATs.end())
                                continue;

                            m_FelFuryAffectedATs.insert(l_AT->GetGUID());
                            DoCast(l_AT->GetPosition(), SPELL_SUMMON_FEL_FURY, true);
                        }

                        ++m_FelFuryCount;
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void TriggerVengeanceForm()
            {
                DoCast(SPELL_METAMORPHOSIS_VENGEANCE);

                Talk(SAY_VENGEANCE);

                events.CancelEvent(EVENT_SWOOP);
            }

            void TriggerHavocForm()
            {
                DoCast(SPELL_METAMORPHOSIS_HAVOC);

                Talk(SAY_HAVOC);

                events.CancelEvent(EVENT_FEL_MORTAR);
                events.ScheduleEvent(EVENT_HATRED, 24000);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_tirathon_saltherilAI(p_Creature);
        }
};

/// Glayvianna Soulrender - 98177
class npc_glayvianna_soulrender : public CreatureScript
{
    public:
        npc_glayvianna_soulrender() : CreatureScript("npc_glayvianna_soulrender") { }

        struct npc_glayvianna_soulrenderAI : public ScriptedAI
        {
            npc_glayvianna_soulrenderAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();

                me->SetDisableGravity(true);
                DoCast(SPELL_GLAYVIANNA_DUAL_WIELD);
                DoCast(SPELL_DEMON_HUNTER_WINGS);
                m_IntroDone = false;
            }

            InstanceScript* m_Instance;

            bool m_IntroDone;
            bool m_Metamorphosis;

            void Reset() override
            {
                me->RemoveAllAreasTrigger();

                events.Reset();

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    DoCast(me, SPELL_INFUSION);
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_DEMON_HUNTER_WINGS);
                me->RemoveAurasDueToSpell(SPELL_INFUSION);
                me->SetDisableGravity(false);
                events.ScheduleEvent(EVENT_1, 12000);
                events.ScheduleEvent(EVENT_5, 14000);
                m_Metamorphosis = false;
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(50) && !m_Metamorphosis)
                {
                    m_Metamorphosis = true;
                    events.ScheduleEvent(EVENT_2, 0);
                    events.ScheduleEvent(EVENT_4, 13 * IN_MILLISECONDS);
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == Spells::SPELL_METAMORPHOSIS)
                    me->CastSpell(me, Spells::SPELL_IMMOLATION_AURA, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (!p_Target)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case SPELL_SWOOP:
                    {
                        if (Unit* l_Target = me->getVictim())
                        {
                            DoCast(SPELL_TRASH_THROW_GLAIVE);
                            DoCast(SPELL_TRASH_THROW_GLAIVE_2);

                            Position l_Pos = p_Target->GetPosition();

                            l_Pos.m_orientation = me->GetAngle(l_Target);

                            DoCast(l_Pos, SPELL_SWOOP_JUMP);
                        }

                        break;
                    }
                    case SPELL_SWOOP_DAMAGE:
                    {
                        Position l_Pos = p_Target->GetPosition();
                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 15.0f, me->GetAngle(p_Target));

                        p_Target->KnockbackFrom(l_Pos, 10.0, 10.0f);
                        break;
                    }
                    case SPELL_FEL_SEED:
                    {
                        Position l_Pos = p_Target->GetPosition();

                        for (uint8 l_I = 0; l_I < 3; ++l_I)
                        {
                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(0.0f, 10.0f), frand(0.0f, 2.0f * M_PI));

                            DoCast(l_Pos, SPELL_FEL_SEED_MISSILE, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE && p_ID == SPELL_SWOOP_JUMP)
                    DoCast(me, SPELL_SWOOP_DAMAGE, true);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 75.0f))
                {
                    m_IntroDone = true;

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (m_Instance == nullptr)
                            return;

                        if (Creature* l_Boss = Creature::GetCreature(*me, m_Instance->GetData64(NPC_TIRATHON_SALTHERIL)))
                        {
                            if (l_Boss->IsAIEnabled)
                                l_Boss->AI()->DoAction(0);
                        }
                    });
                }

                if (!me->isInCombat() && me->IsWithinDistInMap(p_Who, 35.0f))
                    AttackStart(p_Who);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCastVictim(SPELL_DOUBLE_STRIKE);
                        events.ScheduleEvent(EVENT_1, 12000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_METAMORPHOSIS);
                        break;
                    }
                    case EVENT_4:
                    {
                        DoCast(SPELL_FEL_SEED);
                        events.ScheduleEvent(EVENT_4, 12000);
                        break;
                    }
                    case EVENT_5:
                    {
                        DoCast(SPELL_SWOOP);
                        events.ScheduleEvent(EVENT_5, 14000);
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
            return new npc_glayvianna_soulrenderAI(p_Creature);
        }
};

/// Drelanim Whisperwind <Warden> - 99013
class npc_drelanim : public CreatureScript
{
    public:
        npc_drelanim() : CreatureScript("npc_drelanim") { }

        struct npc_drelanimAI : public ScriptedAI
        {
            npc_drelanimAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
               m_OutroScene = false;
            }

            bool m_OutroScene;
            uint32 m_Timer;

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 1:
                    {
                        me->GetMotionMaster()->MoveJump({ 4244.69f, -428.03f, 259.35f, 5.8742f }, 20.0f, 25.0f);
                        DoCast(194836);
                        break;
                    }
                    case 2:
                    {
                        Talk(0);
                        m_Timer = 5000;
                        m_OutroScene = true;
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_OutroScene)
                {
                    if (m_Timer < p_Diff)
                    {
                        Talk(1);
                        m_OutroScene = false;
                    }
                    else
                        m_Timer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
          return new npc_drelanimAI(p_Creature);
        }
};

/// Hatred - 96241
class npc_tirathon_beam_target : public CreatureScript
{
    public:
        npc_tirathon_beam_target() : CreatureScript("npc_tirathon_beam_target") { }

        struct npc_tirathon_beam_targetAI : public ScriptedAI
        {
            npc_tirathon_beam_targetAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.5f);
                me->SetSpeed(MOVE_WALK, 0.5f);
            }

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

                p_Summoner->CastSpell(me, SPELL_HATRED_VISUAL, true);
                p_Summoner->CastSpell(me, SPELL_HATRED_CHANNELED, true);

                events.ScheduleEvent(EVENT_1, 2000);
                events.ScheduleEvent(EVENT_2, 9000);
                events.ScheduleEvent(EVENT_3, 1000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        Position l_Pos = me->GetPosition();
                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 50.0f, frand(0, 2.0f * M_PI));

                        me->GetMotionMaster()->MovePoint(1, l_Pos, false);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            l_Owner->RemoveAreaTrigger(SPELL_HATRED_CHANNELED);

                            if (l_Owner->GetAI() != nullptr)
                                l_Owner->GetAI()->DoAction(ACTION_1);
                        }

                        me->RemoveAllAreasTrigger();
                        me->RemoveAllAuras();
                        me->DespawnOrUnsummon(100);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(me, SPELL_HATRED_AOE_DAMAGE, true);
                        events.ScheduleEvent(EVENT_3, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tirathon_beam_targetAI(p_Creature);
        }
};

/// Glaive - 102643
/// Glaive - 102644
class npc_tirathon_glaive : public CreatureScript
{
    public:
        npc_tirathon_glaive() : CreatureScript("npc_tirathon_glaive") { }

        struct npc_tirathon_glaiveAI : public ScriptedAI
        {
            npc_tirathon_glaiveAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (me->GetEntry() == NPC_GLAIVE_01)
                    DoCast(me, SPELL_THROW_GLAIVE_AT_01, true);
                else if (me->GetEntry() == NPC_GLAIVE_02)
                {
                    me->SetCollision(false);

                    DoCast(me, SPELL_THROW_GLAIVE_AT_02, true);
                    DoCast(me, SPELL_FEL_CHAIN_PERIODIC, true);
                }

                InitCircularMovement();
            }

            void InitCircularMovement()
            {
                /// Creating the circle path from the center
                Movement::MoveSplineInit l_Init(me);

                std::vector<G3D::Vector3> l_Path;

                switch (me->GetEntry())
                {
                    case NPC_GLAIVE_01:
                    {
                        l_Path =
                        {
                            { 4303.981f, -458.0517f, 261.0007f },
                            { 4306.598f, -457.1471f, 261.0143f },
                            { 4308.592f, -455.1032f, 261.0296f },
                            { 4309.668f, -452.1427f, 261.0217f },
                            { 4309.405f, -449.2095f, 261.0245f },
                            { 4307.854f, -446.7206f, 261.2524f },
                            { 4305.643f, -444.8013f, 261.1779f },
                            { 4303.037f, -443.9864f, 261.1189f },
                            { 4300.196f, -444.5732f, 261.0007f },
                            { 4297.742f, -445.7920f, 261.0037f },
                            { 4295.985f, -447.8856f, 261.0007f },
                            { 4295.138f, -450.4822f, 261.0007f },
                            { 4295.349f, -453.2053f, 261.0007f },
                            { 4296.586f, -455.6402f, 261.0007f },
                            { 4298.662f, -457.4163f, 261.0244f },
                            { 4301.258f, -458.2631f, 261.0007f }
                        };

                        break;
                    }
                    case NPC_GLAIVE_02:
                    {
                        l_Path =
                        {
                            { 4324.608f, -451.1632f, 259.43f },
                            { 4329.343f, -460.6962f, 259.43f },
                            { 4326.505f, -467.9254f, 259.43f },
                            { 4316.947f, -477.4826f, 259.43f },
                            { 4306.917f, -480.5608f, 259.43f },
                            { 4254.378f, -480.8941f, 259.43f },
                            { 4240.406f, -474.4219f, 259.43f },
                            { 4208.853f, -462.8733f, 259.43f },
                            { 4208.226f, -439.5764f, 259.43f },
                            { 4241.705f, -427.4549f, 259.43f },
                            { 4252.059f, -422.1667f, 259.43f },
                            { 4306.912f, -421.7188f, 259.43f },
                            { 4316.774f, -425.0903f, 259.43f },
                            { 4326.901f, -433.9549f, 259.43f },
                            { 4329.472f, -442.8524f, 259.43f }
                        };

                        break;
                    }
                    default:
                        break;
                }

                for (G3D::Vector3 l_Point : l_Path)
                    l_Init.Path().push_back(l_Point);

                if (l_Init.Path().empty())
                    return;

                l_Init.SetWalk(true);
                l_Init.SetCyclic();
                l_Init.Launch();

                SetFlyMode(true);
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity *= me->GetCreatureTemplate()->speed_walk;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tirathon_glaiveAI(p_Creature);
        }
};

/// Fel Fury - 107101
class npc_tirathon_fel_fury : public CreatureScript
{
    public:
        npc_tirathon_fel_fury() : CreatureScript("npc_tirathon_fel_fury") { }

        struct npc_tirathon_fel_furyAI : public ScriptedAI
        {
            npc_tirathon_fel_furyAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint8 m_ScorchCount;

            void Reset() override
            {
                m_ScorchCount = 0;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(EVENT_1, 3000);
                events.ScheduleEvent(EVENT_2, 12000);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                me->SetInCombatWithZone();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SCORCH)
                {
                    ++m_ScorchCount;

                    if (m_ScorchCount >= 10)
                    {
                        m_ScorchCount = 0;

                        events.Reset();

                        DoCast(me, SPELL_FEL_DETONATION);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(l_Target, SPELL_SCORCH);

                        events.ScheduleEvent(EVENT_1, 3500);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(me, SPELL_FEL_EMISSION);
                        events.ScheduleEvent(EVENT_2, 15000);
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
            return new npc_tirathon_fel_furyAI(p_Creature);
        }
};

/// Hatred - 190833
class spell_tirathon_beam_dmg : public SpellScriptLoader
{
    public:
        spell_tirathon_beam_dmg() : SpellScriptLoader("spell_tirathon_beam_dmg") { }

        class spell_tirathon_beam_dmg_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tirathon_beam_dmg_AuraScript);

            uint16 m_CastTimer = 500;

            void OnUpdate(uint32 p_Diff, AuraEffect* /*aurEff*/)
            {
                if (m_CastTimer <= p_Diff)
                {
                    m_CastTimer = 500;

                    if (GetCaster() && GetTarget())
                        GetCaster()->CastSpell(GetTarget(), SPELL_HATRED_DAMAGE, true);
                }
                else
                    m_CastTimer -= p_Diff;
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_tirathon_beam_dmg_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tirathon_beam_dmg_AuraScript();
        }
};

/// Swoop - 191765
class spell_tirathon_swoop_filter : public SpellScriptLoader
{
    public:
        spell_tirathon_swoop_filter() : SpellScriptLoader("spell_tirathon_swoop_filter") { }

        class spell_tirathon_swoop_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tirathon_swoop_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::ObjectDistanceOrderPred(GetCaster(), false));

                if (GetCaster()->GetEntry() == NPC_TIRATHON_SALTHERIL)
                    JadeCore::RandomResizeList(p_Targets, 1);

                WorldObject* l_Target = p_Targets.front();

                p_Targets.clear();
                p_Targets.push_back(l_Target);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tirathon_swoop_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tirathon_swoop_filter_SpellScript();
        }
};

/// Throw Glaive - 193559, 193578
class spell_tirathon_throw_glaive_aura : public SpellScriptLoader
{
    public:
        spell_tirathon_throw_glaive_aura() : SpellScriptLoader("spell_tirathon_throw_glaive_aura") { }

        enum eEquipIDs
        {
            FirstItem   = 128359,
            SecondItem  = 128371
        };

        class spell_tirathon_throw_glaive_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tirathon_throw_glaive_aura_AuraScript);

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, 0);
                l_Caster->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2, 0);
            }

            void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, eEquipIDs::FirstItem);
                l_Caster->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2, eEquipIDs::SecondItem);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tirathon_throw_glaive_aura_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tirathon_throw_glaive_aura_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tirathon_throw_glaive_aura_AuraScript();
        }
};

/// Fel Chain - 202825
class spell_tirathon_fel_chain : public SpellScriptLoader
{
    public:
        spell_tirathon_fel_chain() : SpellScriptLoader("spell_tirathon_fel_chain") { }

        class spell_tirathon_fel_chain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tirathon_fel_chain_AuraScript);

            void OnPeriodic(AuraEffect const* p_AurEff)
            {
                PreventDefaultAction();

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        Creature* l_Boss = nullptr;
                        if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                            l_Boss = Creature::GetCreature(*l_Caster, l_Instance->GetData64(NPC_TIRATHON_SALTHERIL));

                        if (l_Boss == nullptr)
                            return;

                        std::list<Player*> l_PlayerList;
                        l_Caster->GetPlayerListInGrid(l_PlayerList, 100.0f);

                        if (l_PlayerList.empty())
                            return;

                        l_PlayerList.remove_if([this, l_Caster, l_Target, l_Boss](Player* p_Player) -> bool
                        {
                            /// Not attackable
                            if (!l_Boss->IsValidAttackTarget(p_Player))
                            {
                                p_Player->RemoveAura(SPELL_FEL_CHAIN_DOT);
                                return true;
                            }

                            /// Not in the beam line
                            if (!p_Player->IsInBetween(l_Caster, l_Target, 3.0f))
                            {
                                p_Player->RemoveAura(SPELL_FEL_CHAIN_DOT);
                                return true;
                            }

                            /// Not between the two Lens
                            if (l_Caster->GetDistance(p_Player) > l_Caster->GetDistance(l_Target))
                            {
                                p_Player->RemoveAura(SPELL_FEL_CHAIN_DOT);
                                return true;
                            }

                            return false;
                        });

                        for (Player* l_Player : l_PlayerList)
                            l_Caster->CastSpell(l_Player, SPELL_FEL_CHAIN_DOT, true, nullptr, nullptr, l_Boss->GetGUID());
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tirathon_fel_chain_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tirathon_fel_chain_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_tirathon_saltheril()
{
    /// Boss
    new boss_tirathon_saltheril();

    /// Creatures
    new npc_glayvianna_soulrender();
    new npc_drelanim();
    new npc_tirathon_beam_target();
    new npc_tirathon_glaive();
    new npc_tirathon_fel_fury();

    /// Spells
    new spell_tirathon_beam_dmg();
    new spell_tirathon_swoop_filter();
    new spell_tirathon_throw_glaive_aura();
    new spell_tirathon_fel_chain();
}
#endif
