////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "black_rook_hold_dungeon.hpp"

enum Spells
{
    /// Kurtalos
    /// Phase 1
    SPELL_UNERRING_SHEAR            = 198635,
    SPELL_WHIRLING_BLADE            = 198641,
    SPELL_WHIRLING_BLADE_AT         = 198782,
    SPELL_SUICIDE                   = 117624,

    /// Phase 2
    SPELL_KURTALOS_GHOST_VISUAL     = 199243,
    SPELL_LEGACY_RAVENCREST         = 199368,

    /// Latosius
    /// Phase 1
    SPELL_TELEPORT_1                = 198835,
    SPELL_TELEPORT_2                = 199058,
    SPELL_SHADOW_BOLT               = 198833,
    SPELL_DARK_BLAST                = 198820,
    SPELL_DARK_OBLITERATION         = 199567,
    SPELL_DARK_OBLITERATION_VISUAL  = 241672,

    /// Phase 2
    SPELL_SAP_SOUL                  = 198961,
    SPELL_TRANSFORM                 = 199064,
    SPELL_SHADOW_BOLT_VOLLEY        = 202019,
    SPELL_CLOUD_OF_HYPNOSIS         = 199143,
    SPELL_DREADLORDS_GUILE          = 199193,
    SPELL_BREAK_PLR_TARGETTING      = 140562,

    /// Heroic
    SPELL_STINGING_SWARM            = 201733,

    /// Achievement
    SPELL_LOST_SOUL                 = 213407
};

enum eEvents
{
    /// Kurtalos
    /// Phase 1
    EVENT_UNERRING_SHEAR        = 1,
    EVENT_WHIRLING_BLADE        = 2,

    /// Latosius
    /// Phase 1
    EVENT_LATOSIUS_TP           = 1,
    EVENT_SHADOW_BOLT           = 2,
    EVENT_DARK_BLAST            = 3,
    /// Phase 2
    EVENT_LATOSIUS_TP_2         = 4,
    EVENT_SAP_SOUL              = 5,
    EVENT_TRANSFORM             = 6,
    EVENT_SHADOW_BOLT_VOLLEY    = 7,
    EVENT_CLOUD_OF_HYPNOSIS     = 8,
    EVENT_DREADLORDS_GUILE      = 9,
    EVENT_SUM_IMAGE             = 10,
    EVENT_IMAGE_END             = 11,
    EVENT_STINGING_SWARM        = 12,  ///< Heroic
    EVENT_CHECK_DISTANCE        = 13
};

enum ePhase
{
    PHASE_1     = 0,
    PHASE_2     = 1
};

Position const g_TeleportPos[12] =
{
    { 3169.09f, 7432.38f, 272.10f, 5.42f },
    { 3165.53f, 7421.75f, 272.10f, 5.85f },
    { 3158.29f, 7414.00f, 272.10f, 0.0f  },
    { 3164.16f, 7395.63f, 272.10f, 0.62f },
    { 3173.76f, 7387.44f, 272.10f, 1.08f },
    { 3184.47f, 7383.66f, 272.10f, 1.49f },
    { 3200.61f, 7390.41f, 272.10f, 2.14f },
    { 3205.70f, 7400.89f, 272.10f, 2.61f },
    { 3210.50f, 7409.22f, 272.10f, 3.02f },
    { 3204.84f, 7427.28f, 272.10f, 3.84f },
    { 3195.91f, 7430.39f, 272.10f, 4.24f },
    { 3186.87f, 7436.26f, 272.10f, 4.70f }
};

Position const g_RoomCenter = { 3185.39f, 7410.0f, 270.4f };

/// Latosius <Advisor to Lord Ravencrest> - 98970
class boss_latosius : public CreatureScript
{
    public:
        boss_latosius() : CreatureScript("boss_latosius") { }

        struct boss_latosiusAI : public BossAI
        {
            boss_latosiusAI(Creature* p_Creature) : BossAI(p_Creature, eData::Kurtalos) { }

            uint8 m_ImageSumCount;
            uint8 m_ImageIndex;
            uint32 m_TPState;

            enum eTalks
            {
                DarkBlast,
                CloudOfHypnosis,
                DarkObliteration
            };

            void Reset() override
            {
                if (instance && instance->GetData(eData::KurtalosState) != PHASE_1)
                    return;

                _Reset();
                me->DespawnCreaturesInArea(NPC_SOUL_KURTALOS);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                SetEquipmentSlots(false, 65483);
                me->SummonCreature(NPC_KURTALOS, 3191.72f, 7423.69f, 270.462f, 0.57f);
                RemoveAuras();
                m_TPState = 0;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                if (instance && instance->GetData(eData::KurtalosState) != PHASE_1)
                    return;

                _EnterCombat();

                if (Creature* l_Kurtalos = me->FindNearestCreature(NPC_KURTALOS, 30.0f))
                {
                    if (l_Kurtalos->IsAIEnabled)
                        l_Kurtalos->AI()->DoZoneInCombat(l_Kurtalos, 100.0f);
                }

                events.ScheduleEvent(EVENT_LATOSIUS_TP, 0);
                events.ScheduleEvent(EVENT_DARK_BLAST, 9000);
            }

            uint32 GetData(uint32 p_ID /* = 0 */) override
            {
                return m_TPState;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void EnterEvadeMode() override
            {
                RemoveAuras();
                if (instance && instance->GetData(eData::KurtalosState) != PHASE_1)
                    instance->SetData(eData::KurtalosState, PHASE_1);

                me->RemoveAurasDueToSpell(SPELL_TRANSFORM);
                me->NearTeleportTo(3196.04f, 7425.02f, 270.373f, 3.47f);
                me->UpdateEntry(NPC_LATOSIUS);
                me->SetVisible(true);
                me->DespawnCreaturesInArea(NPC_SOUL_KURTALOS);

                BossAI::EnterEvadeMode();
                me->SummonCreature(NPC_KURTALOS, 3191.72f, 7423.69f, 270.462f, 0.57f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                RemoveAuras();

                if (instance)
                {
                    instance->DoCastSpellOnPlayers(SPELL_LOST_SOUL);
                    instance->DoKilledMonsterKredit(39349, NPC_DANTALIONAX);
                }

                Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                if (!l_Players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                            l_Player->KilledMonsterCredit(NPC_DANTALIONAX);
                    }
                }
            }

            void RemoveAuras()
            {
                if (!instance)
                    return;

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNERRING_SHEAR);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LEGACY_RAVENCREST);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STINGING_SWARM);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == ACTION_1 && instance && instance->GetData(eData::KurtalosState) != PHASE_2)
                {
                    instance->SetData(eData::KurtalosState, PHASE_2);
                    events.Reset();
                    events.ScheduleEvent(EVENT_LATOSIUS_TP_2, 1000);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_DREADLORDS_GUILE:
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetVisible(false);
                        me->SetReactState(REACT_PASSIVE);
                        DoCast(me, SPELL_BREAK_PLR_TARGETTING, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_SAP_SOUL:
                    {
                        AddTimedDelayedOperation(7000, [this]() -> void
                        {
                            me->UpdateEntry(NPC_DANTALIONAX);
                            SetEquipmentSlots(false, 0, 0, 0);
                            DoCast(me, SPELL_TRANSFORM, true);
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_CHECK_DISTANCE, 500);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, 18000);
                            events.ScheduleEvent(EVENT_CLOUD_OF_HYPNOSIS, 13000);
                            events.ScheduleEvent(EVENT_DREADLORDS_GUILE, 33 * IN_MILLISECONDS);
                            if (IsHeroicOrMythic())
                                events.ScheduleEvent(EVENT_STINGING_SWARM, 22000);
                            DoZoneInCombat(me, 100.0f);
                        });

                        Creature* l_Kurtalos = p_Target->ToCreature();
                        if (!l_Kurtalos || !l_Kurtalos->IsAIEnabled)
                            return;

                        l_Kurtalos->AI()->DoAction(0);

                        break;
                    }
                    case SPELL_STINGING_SWARM:
                    {
                        if (Creature* l_Summon = p_Target->SummonCreature(NPC_STINGING_SWARM, p_Target->GetPositionX(), p_Target->GetPositionY(), p_Target->GetPositionZ()))
                            l_Summon->EnterVehicle(p_Target);

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_DARK_BLAST:
                    {
                        events.ScheduleEvent(EVENT_SHADOW_BOLT, 1000);
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

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventID = events.ExecuteEvent())
                {
                    switch (l_EventID)
                    {
                        case EVENT_LATOSIUS_TP:
                        {
                            DoCast(me, SPELL_TELEPORT_1, true);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, 500);
                            me->SendPlayHoverAnim(true);
                            break;
                        }
                        case EVENT_SHADOW_BOLT:
                        {
                            DoCast(SPELL_SHADOW_BOLT);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, 3000);
                            break;
                        }
                        case EVENT_DARK_BLAST:
                        {
                            Talk(eTalks::DarkBlast);
                            Position l_Pos = me->GetPosition();
                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, 60.0f, 0.0f);
                            Unit* l_Target = me->SummonCreature(68553, l_Pos, TEMPSUMMON_TIMED_DESPAWN, 5000);
                            if (l_Target)
                                DoCast(l_Target, SPELL_DARK_BLAST);
                            events.CancelEvent(EVENT_SHADOW_BOLT);
                            events.ScheduleEvent(EVENT_LATOSIUS_TP, 13000);
                            events.ScheduleEvent(EVENT_DARK_BLAST, 18000);
                            break;
                        }
                        case EVENT_LATOSIUS_TP_2:
                        {
                            m_TPState = 1;
                            DoCast(me, SPELL_TELEPORT_1, true);
                            events.ScheduleEvent(EVENT_SAP_SOUL, 1000);
                            me->SendPlayHoverAnim(false);
                            break;
                        }
                        case EVENT_SAP_SOUL:
                        {
                            DoCast(me, SPELL_SAP_SOUL, true);
                            break;
                        }
                        case EVENT_SHADOW_BOLT_VOLLEY:
                        {
                            DoCast(SPELL_SHADOW_BOLT_VOLLEY);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, 14000);
                            break;
                        }
                        case EVENT_CLOUD_OF_HYPNOSIS:
                        {
                            if (!me->HasAura(SPELL_DREADLORDS_GUILE))
                            {
                                Talk(eTalks::CloudOfHypnosis);
                                if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                    DoCast(l_Target, SPELL_CLOUD_OF_HYPNOSIS);
                            }
                            events.ScheduleEvent(EVENT_CLOUD_OF_HYPNOSIS, 32000);
                            break;
                        }
                        case EVENT_DREADLORDS_GUILE:
                        {
                            DoStopAttack();
                            m_ImageSumCount = 0;
                            m_ImageIndex = urand(0, 11);
                            DoCast(SPELL_DREADLORDS_GUILE);
                            events.ScheduleEvent(EVENT_DREADLORDS_GUILE, 62000);
                            events.ScheduleEvent(EVENT_SUM_IMAGE, 4000);
                            break;
                        }
                        case EVENT_SUM_IMAGE:
                        {
                            if (m_ImageSumCount == 0)
                                Talk(eTalks::DarkObliteration);

                            if (m_ImageSumCount > 11)
                            {
                                events.RescheduleEvent(EVENT_IMAGE_END, 3000);
                                break;
                            }

                            if (m_ImageIndex > 11)
                                m_ImageIndex = 0;

                            me->SummonCreature(NPC_IMAGE_OF_LATOSIUS, g_TeleportPos[m_ImageIndex++]);
                            m_ImageSumCount++;
                            events.RescheduleEvent(EVENT_SUM_IMAGE, 1.25 * TimeConstants::IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_IMAGE_END:
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                            me->SetVisible(true);
                            me->RemoveAurasDueToSpell(SPELL_DREADLORDS_GUILE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.RescheduleEvent(EVENT_DREADLORDS_GUILE, 65 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_STINGING_SWARM:
                        {
                            if (!me->HasAura(SPELL_DREADLORDS_GUILE))
                            {
                                if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 90.0f, true, -SPELL_STINGING_SWARM))
                                    DoCast(l_Target, SPELL_STINGING_SWARM);
                            }
                            events.ScheduleEvent(EVENT_STINGING_SWARM, 17000);
                            break;
                        }
                        case EVENT_CHECK_DISTANCE:
                        {
                            if (me->GetDistance(g_RoomCenter) > 19.0f)
                            {
                                EnterEvadeMode();
                                break;
                            }
                            else
                               events.ScheduleEvent(EVENT_CHECK_DISTANCE, 500);

                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_latosiusAI(p_Creature);
        }
};

/// Kur'talos Ravencrest <Lord of Black Rook Hold> - 98965
class npc_kurtalos_ravencrest : public CreatureScript
{
    public:
        npc_kurtalos_ravencrest() : CreatureScript("npc_kurtalos_ravencrest") { }

        struct npc_kurtalos_ravencrestAI : public ScriptedAI
        {
            npc_kurtalos_ravencrestAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
            }

            enum eTalks
            {
                Aggro,
                UnerringShear,
                PlayerKill
            };

            InstanceScript* m_Instance;
            bool m_SecondPhase;

            void Reset() override
            {
                m_SecondPhase = false;
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim->IsPlayer())
                    Talk(eTalks::PlayerKill);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);

                if (Creature* l_Latosius = me->FindNearestCreature(NPC_LATOSIUS, 30.0f))
                {
                    if (l_Latosius->IsAIEnabled)
                        l_Latosius->AI()->DoZoneInCombat(l_Latosius, 100.0f);
                }

                events.ScheduleEvent(EVENT_UNERRING_SHEAR, 6000);
                events.ScheduleEvent(EVENT_WHIRLING_BLADE, 10000);
            }

            void EnterEvadeMode() override
            {
                if (m_Instance && m_Instance->GetData(eData::KurtalosState) != PHASE_1)
                    m_Instance->SetData(eData::KurtalosState, PHASE_1);

                if (Creature* l_Latosius = me->FindNearestCreature(NPC_LATOSIUS, 30.0f))
                {
                    if (l_Latosius->IsAIEnabled)
                        l_Latosius->AI()->EnterEvadeMode();
                }

                ScriptedAI::EnterEvadeMode();
            }

            void DoAction(int32 const /*p_Action*/) override
            {
                me->SummonCreature(NPC_SOUL_KURTALOS, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_WHIRLING_BLADE)
                {
                    if (Creature* l_Blade = me->SummonCreature(NPC_KURTALOS_BLADE_TRIGGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                        l_Blade->GetMotionMaster()->MovePoint(1, p_Target->GetPositionX(), p_Target->GetPositionY(), p_Target->GetPositionZ());
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SAP_SOUL)
                    me->m_Events.AddEvent(new DelayCastEvent(*me, me->GetGUID(), SPELL_SUICIDE, true), me->m_Events.CalculateTime(5000));
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth() || me->HealthBelowPct(20))
                {
                    if (!m_SecondPhase)
                    {
                        m_SecondPhase = true;
                        DoStopAttack();
                        events.Reset();

                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (Creature* l_Summoner = l_Owner->ToCreature())
                            {
                                if (l_Summoner->IsAIEnabled)
                                    l_Summoner->AI()->DoAction(ACTION_1);
                            }
                        }
                    }

                    if (p_Attacker != me)
                        p_Damage = 0;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->GetDistance(g_RoomCenter) > 19.0f)
                    EnterEvadeMode();

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_UNERRING_SHEAR:
                    {
                        Talk(eTalks::UnerringShear);
                        DoCastVictim(SPELL_UNERRING_SHEAR);
                        events.ScheduleEvent(EVENT_UNERRING_SHEAR, 13000);
                        break;
                    }
                    case EVENT_WHIRLING_BLADE:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            DoCast(l_Target, SPELL_WHIRLING_BLADE);
                        events.ScheduleEvent(EVENT_WHIRLING_BLADE, 22000);
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
            return new npc_kurtalos_ravencrestAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Image of Latosius <Advisor to Lord Ravencrest> - 101028
class npc_latosius_double : public CreatureScript
{
    public:
        npc_latosius_double() : CreatureScript("npc_latosius_double") { }

        struct npc_latosius_doubleAI : public ScriptedAI
        {
            npc_latosius_doubleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                SetEquipmentSlots(false, 65483);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                events.ScheduleEvent(EVENT_DARK_BLAST, 9000);
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->DespawnOrUnsummon(10000);
                Position l_Pos = me->GetPosition();
                l_Pos.SimplePosXYRelocationByAngle(l_Pos, 60.0f, 0.0f);

                DoCast(me, Spells::SPELL_DARK_OBLITERATION_VISUAL, true);

                Unit* l_Target = me->SummonCreature(68553, l_Pos, TEMPSUMMON_TIMED_DESPAWN, 6500);
                if (l_Target)
                    me->DelayedCastSpell(l_Target, Spells::SPELL_DARK_OBLITERATION, false, 100);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                return;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_latosius_doubleAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Soul of Ravencrest - 101054
class npc_kurtalos_trigger : public CreatureScript
{
    public:
        npc_kurtalos_trigger() : CreatureScript("npc_kurtalos_trigger") { }

        struct npc_kurtalos_triggerAI : public ScriptedAI
        {
            npc_kurtalos_triggerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->setFaction(14);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            enum eSpells
            {
                ConversationTransition = 199239
            };

            InstanceScript* m_Instance;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (m_Instance && m_Instance->GetBossState(eData::Kurtalos) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                uint32 l_DisplayID = me->GetDisplayId();
                me->SetDisplayId(11686);
                AddTimedDelayedOperation(8 * IN_MILLISECONDS, [this, l_DisplayID]() -> void
                {
                    me->SetDisplayId(l_DisplayID);
                    DoCast(me, SPELL_KURTALOS_GHOST_VISUAL, true);
                });

                me->setFaction(35);
                DoCast(eSpells::ConversationTransition, true);
                events.ScheduleEvent(EVENT_2, 15000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_2:
                    {
                        me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);
                        events.ScheduleEvent(EVENT_3, 17000);
                        break;
                    }
                    case EVENT_3:
                    {
                        me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, 0);
                        if (m_Instance && m_Instance->GetBossState(eData::Kurtalos) == IN_PROGRESS)
                            me->CastSpell(me, SPELL_LEGACY_RAVENCREST, true);

                        events.ScheduleEvent(EVENT_4, 13000);
                    }
                    case EVENT_4:
                    {
                        me->DespawnOrUnsummon(2000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kurtalos_triggerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Whirling blade areatrigger dummy - 100861
class npc_whirling_blade_dummy : public CreatureScript
{
    public:
        npc_whirling_blade_dummy() : CreatureScript("npc_whirling_blade_dummy") { }

        struct npc_whirling_blade_dummyAI : public ScriptedAI
        {
            npc_whirling_blade_dummyAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->setFaction(14);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            enum eSpells
            {
                WhirlingBladeAreatrigger = 198782
            };

            InstanceScript* m_Instance;
            Position m_TargetPosition;
            Position m_BossPosition;
            uint64 m_Kurtalos;
            bool m_IsMovingTowardsPlayer;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (m_Instance && m_Instance->GetBossState(eData::Kurtalos) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                UnitAI* l_SummonerAI = p_Summoner->GetAI();
                if (!l_SummonerAI)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                Unit* l_Target = l_SummonerAI->SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true);
                if (!l_Target)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                m_Kurtalos = p_Summoner->GetGUID();

                DoCast(me, eSpells::WhirlingBladeAreatrigger, true);
                m_TargetPosition = *l_Target;
                m_BossPosition = *me;
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, m_TargetPosition);
                m_IsMovingTowardsPlayer = true;
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (m_IsMovingTowardsPlayer)
                {
                    m_IsMovingTowardsPlayer = false;
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, m_BossPosition);
                }
                else
                {
                    m_IsMovingTowardsPlayer = true;
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, m_TargetPosition);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->HasAura(eSpells::WhirlingBladeAreatrigger))
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                Creature* l_Creature = Creature::GetCreature(*me, m_Kurtalos);
                if (!l_Creature || !l_Creature->isInCombat())
                {
                    me->DespawnOrUnsummon();
                    return;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_whirling_blade_dummyAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Stinging Swarm - 101008
class npc_kurtalos_stinging_swarm : public CreatureScript
{
    public:
        npc_kurtalos_stinging_swarm() : CreatureScript("npc_kurtalos_stinging_swarm") { }

        struct npc_kurtalos_stinging_swarmAI : public ScriptedAI
        {
            npc_kurtalos_stinging_swarmAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
            }

            InstanceScript* m_Instance;
            uint16 m_CheckTimer;

            enum eSpells
            {
                InsectsVisual = 199160
            };

            void Reset() override
            {
                m_CheckTimer = 2000;
                DoCast(eSpells::InsectsVisual, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->ToTempSummon())
                {
                    if (Unit* l_Summoner = me->ToTempSummon()->GetSummoner())
                        l_Summoner->RemoveAurasDueToSpell(SPELL_STINGING_SWARM);

                    me->DespawnOrUnsummon(0);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_CheckTimer)
                {
                    if (m_CheckTimer <= p_Diff)
                    {
                        if (m_Instance && m_Instance->GetBossState(eData::Kurtalos) != IN_PROGRESS)
                        {
                            me->DespawnOrUnsummon();
                            return;
                        }

                        m_CheckTimer = 2000;
                    }
                    else
                        m_CheckTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kurtalos_stinging_swarmAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Cloud of Hypnosis - 100994
class npc_kurtalos_cloud_of_hypnosis : public CreatureScript
{
    public:
        npc_kurtalos_cloud_of_hypnosis() : CreatureScript("npc_kurtalos_cloud_of_hypnosis") { }

        struct npc_kurtalos_cloud_of_hypnosisAI : public ScriptedAI
        {
            npc_kurtalos_cloud_of_hypnosisAI(Creature* p_Creature) : ScriptedAI(p_Creature) 
            {
                me->setFaction(16);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            enum eSpells
            {
                CloudOfHypnosisAreatrigger = 199092
            };

            void Reset() override
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveRandom(2.0f);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(eSpells::CloudOfHypnosisAreatrigger, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kurtalos_cloud_of_hypnosisAI(p_Creature);
        }
};

/// Teleport - 198835
class spell_latosius_random_teleport : public SpellScriptLoader
{
    public:
        spell_latosius_random_teleport() : SpellScriptLoader("spell_latosius_random_teleport") { }

        class spell_latosius_random_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_latosius_random_teleport_SpellScript);

            uint8 m_Random;

            void HandleScriptEffect(SpellEffIndex p_EffIndex)
            {
                PreventHitDefaultEffect(p_EffIndex);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* l_Creature = l_Caster->ToCreature();
                if (!l_Caster || !l_Creature->IsAIEnabled)
                    return;

                if (l_Creature->AI()->GetData(0) == 0)
                {
                    m_Random = urand(0, 11);
                    l_Caster->NearTeleportTo(g_TeleportPos[m_Random].GetPositionX(), g_TeleportPos[m_Random].GetPositionY(), g_TeleportPos[m_Random].GetPositionZ(), g_TeleportPos[m_Random].GetOrientation());
                }
                else
                    l_Caster->NearTeleportTo(g_RoomCenter);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_latosius_random_teleport_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_latosius_random_teleport_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Risen Lancer - 102095
class npc_risen_lancer : public CreatureScript
{
    public:
        npc_risen_lancer() : CreatureScript("npc_risen_lancer") { }

        struct npc_risen_lancerAI : public ScriptedAI
        {
            npc_risen_lancerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                RavensDive = 214001
            };

            int32 m_RavensDiveTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_RavensDiveTimer = 8 * IN_MILLISECONDS;
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                m_RavensDiveTimer = 10 * IN_MILLISECONDS;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!me->isInCombat())
                    return;

                m_RavensDiveTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_RavensDiveTimer < 0)
                {
                    Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f, true);
                    if (!l_Victim)
                        return;

                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    me->CastSpell(l_Victim, eSpells::RavensDive, false);
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_risen_lancerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Risen Swordsman - 102094
class npc_risen_swordsman : public CreatureScript
{
    public:
        npc_risen_swordsman() : CreatureScript("npc_risen_swordsman") { }

        struct npc_risen_swordsmanAI : public ScriptedAI
        {
            npc_risen_swordsmanAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                CoupDeGrace = 214003
            };

            int32 m_CoupDeGraceTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_CoupDeGraceTimer = 15 * IN_MILLISECONDS;
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                m_CoupDeGraceTimer = 20 * IN_MILLISECONDS;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!me->isInCombat())
                    return;

                m_CoupDeGraceTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_CoupDeGraceTimer < 0)
                {
                    Unit* l_Victim = me->getVictim();
                    if (!l_Victim)
                        return;

                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    me->CastSpell(l_Victim, eSpells::CoupDeGrace, false);
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_risen_swordsmanAI(p_Creature);
        }
};

///< Last update 7.1.5 Build 23420
///< Unerring Shear - 198635
class spell_kurtalos_unerring_shear : public SpellScriptLoader
{
    public:
        spell_kurtalos_unerring_shear() : SpellScriptLoader("spell_kurtalos_unerring_shear") { }

        class spell_kurtalos_unerring_shear_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kurtalos_unerring_shear_AuraScript);

            void HandleAfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Modes*/)
            {
                Aura* l_Aura = p_AurEff->GetBase();
                if (!l_Aura)
                    return;

                if (l_Aura->GetStackAmount() > 1)
                    l_Aura->SetStackAmount(1);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_kurtalos_unerring_shear_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kurtalos_unerring_shear_AuraScript();
        }
};

///< Last update 7.1.5 Build 23420
///< Stinging Swarm - 201733
class spell_kurtalos_stinging_swarm : public SpellScriptLoader
{
    public:
        spell_kurtalos_stinging_swarm() : SpellScriptLoader("spell_kurtalos_stinging_swarm") { }

        class spell_kurtalos_stinging_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kurtalos_stinging_swarm_AuraScript);

            enum eSpells
            {
                Itchy = 199168
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!roll_chance_i(20))
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::Itchy, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kurtalos_stinging_swarm_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kurtalos_stinging_swarm_AuraScript();
        }
};

///< Last update 7.1.5 Build 23420
///< Sap soul - 198961
class spell_kurtalos_sap_soul : public SpellScriptLoader
{
    public:
        spell_kurtalos_sap_soul() : SpellScriptLoader("spell_kurtalos_sap_soul") { }

        class spell_kurtalos_sap_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kurtalos_sap_soul_AuraScript);

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || !l_Target->IsAIEnabled)
                    return;

                Creature* l_Creature = l_Target->ToCreature();
                if (!l_Creature)
                    return;

                l_Creature->SetReactState(REACT_PASSIVE);
                l_Creature->GetMotionMaster()->Clear();
                l_Creature->AttackStop();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_kurtalos_sap_soul_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kurtalos_sap_soul_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_lord_kurtalos_ravencrest()
{
    /// Boss
    new boss_latosius();

    /// Creature
    new npc_kurtalos_ravencrest();
    new npc_whirling_blade_dummy();
    new npc_latosius_double();
    new npc_kurtalos_trigger();
    new npc_kurtalos_stinging_swarm();
    new npc_kurtalos_cloud_of_hypnosis();

    /// Spell
    new spell_latosius_random_teleport();
    new spell_kurtalos_unerring_shear();
    new spell_kurtalos_stinging_swarm();
    new spell_kurtalos_sap_soul();

    /// Trashes
    new npc_risen_lancer();
    new npc_risen_swordsman();
}
#endif
