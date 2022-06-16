////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eSpells
{
    /// Intro
    SpellDeadCosmetic                   = 223945,
    SpellBurstingSlime                  = 223710,

    /// Sludgerax
    SpellDeathCosmetic2                 = 223721,
    SpellBurstingSlimeAura              = 223710,
    SpellBurstingSlimeSearcher          = 223712,
    SpellBurstingSlimeMissile           = 223714,
    SpellOozingRushFixate               = 223655,
    SpellOozingRushAreaTrigger          = 223671,
    SpellOozingRushEnter                = 223674,
    SpellOozingRushExit                 = 223668,
    SpellOozingRushDamage               = 234585,
    SpellSludgeEruptionAoE              = 223955,
    SpellSludgeEruptionAura             = 223958,

    SpellArcaneSeepage                  = 206482,
    SpellArcaneSeepageFilter            = 208875,
    SpellArcaneSeepageMissile           = 206484,
    SpellArcaneSeepageAT                = 206487,
    SpellArcaneSlash                    = 206641,
    SpellFormCleanerDummy               = 206570,
    SpellFormCleaner                    = 206560,
    SpellFormManiac                     = 206557,
    SpellFormCaretaker                  = 206559,
    SpellAnnihilationJump               = 207620,
    SpellAnnihilationAT                 = 207630,
    SpellToxicSlice                     = 206788,
    SpellToxicSliceTrigger              = 206789,
    SpellToxicSliceAT                   = 206796,
    SpellToxicSliceDoT                  = 206798,
    SpellSterilize                      = 208506,
    SpellSterilizeFilter2               = 214582,
    SpellSterilizeAura                  = 208499,
    SpellSterilizeAura2                 = 218247,
    SpellSterilizeEnergize              = 214583,
    SpellCleansingRageFilter            = 206820,
    SpellCleansingRageMissile           = 206824,
    SpellManaRupture                    = 206645,
    SpellManaRuptureFilter              = 206711,
    SpellManaRuptureMissile             = 206748,
    SpellArcingBondsFilter              = 208924,
    SpellArcingBondsDmg                 = 208918,
    SpellArcingBondsVisual              = 209023,
    SpellArcingBondsPeriodic            = 208910,
    SpellArcingBondsDummy               = 208915,
    SpellSucculentFeast                 = 207502,
    SpellSucculentFeastSum              = 207503,
    SpellSucculentFeastAT               = 206835,
    SpellSucculentFeastAbsorb           = 206838,
    SpellTidyUp                         = 207513,
    SpellTidyUpSum                      = 207531,
    SpellRideVehicle                    = 207433,

    /// Heroic
    SpellToxicSliceHeroic               = 214573,

    /// Mythic
    SpellDeactivatedConstruct           = 214881,
    SpellEnergized                      = 214670,
    SpellDualPersonalitiesCleaner       = 214882,
    SpellDualPersonalitiesManiac        = 215066,
    SpellToxicSliceCopy                 = 215062,
    SpellCleansingRageCopy              = 215072,
    SpellAnnihilationATCopy             = 214672,

    /// Scrubber
    SpellEnergyAura                     = 224470,
    SpellScrubbingMax                   = 211897,
    SpellScrubbingMin                   = 211907,
    SpellEnergyState1                   = 215048,
    SpellEnergyState2                   = 224472,
    SpellCleansingDestruction           = 207327,
    SpellCleansingDestructionPlayer     = 207328
};

enum eEvents
{
    EventArcaneSlash = 1,
    EventAnnihilation,
    EventToxicSlice,
    EventSterilize,
    EventCleansingRage,
    EventManaRupture,
    EventArcingBonds,
    EventSucculentFeast,
    EventTidyUp,
    EventBerserker,

    /// Sludgerax
    EventOozingRush
};

enum eTrilliaxData
{
    DataRotationType,
    DataTargetedSeepage
};

Position const g_ScrubberPos[5] =
{
    { 413.16f, 3285.52f, -225.67f, 3.64f }, ///< 104596
    { 459.70f, 3360.98f, -225.67f, 1.52f },
    { 479.96f, 3355.19f, -225.69f, 1.11f },
    { 453.32f, 3259.95f, -225.69f, 4.62f },
    { 502.97f, 3331.67f, -225.69f, 0.44f }
};

Position const g_CenterPos = { 457.5139f, 3309.968f, -225.6947f, 0.0f };

Position const g_SludgeraxSumPos[8] =
{
    { 427.32f, 3336.66f, -225.69f, 2.21f }, ///< 112251
    { 429.49f, 3340.25f, -225.69f, 0.73f },
    { 459.06f, 3344.21f, -225.59f, 1.59f },
    { 466.09f, 3280.36f, -225.64f, 2.11f },
    { 475.85f, 3291.92f, -225.66f, 0.70f },
    { 477.00f, 3278.06f, -225.63f, 3.26f },
    { 494.54f, 3319.75f, -225.69f, 2.81f },
    { 495.03f, 3313.42f, -225.69f, 1.97f }
};

/// Trilliax - 104288
class boss_trilliax : public CreatureScript
{
    public:
        boss_trilliax() : CreatureScript("boss_trilliax") { }

        enum eTalks
        {
            TalkIntro1,
            TalkIntro2,
            TalkIntro3,
            TalkIntro4,
            TalkIntro5,
            TalkAggro,
            TalkAnnihilation,
            TalkToxicSlice,
            TalkTheManiac,
            TalkTheCaretaker,
            TalkTheCleaner,
            TalkSucculentFeast,
            TalkTidyUp,
            TalkCleansingRage,
            TalkWipe,
            TalkTidyUpWarn
        };

        enum eVisual
        {
            AnnihilationVisual = 625
        };

        struct boss_trilliaxAI : BossAI
        {
            boss_trilliaxAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataTrilliax) { }

            uint32 m_AnnihilationTimer;
            uint32 m_SumImprintTimer;
            uint32 m_ImprintEntry;

            uint32 m_RotateDirection = RotateDirection::ROTATE_DIRECTION_LEFT;

            std::set<uint64> m_TargetedSeepages;

            void Intro()
            {
                DoCast(me, eSpells::SpellDeadCosmetic, true);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SummonCreature(eCreatures::NpcSludgerax, 454.26f, 3307.14f, -225.61f, 3.92f);
            }

            void CleanUp()
            {
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
                me->DespawnAreaTriggersInArea(eSpells::SpellArcaneSeepageAT, 150.f);
                me->DespawnAreaTriggersInArea(eSpells::SpellToxicSliceAT, 150.f);
                me->DespawnCreaturesInArea(eCreatures::NpcScrubber, 150.f);
            }

            void Reset() override
            {
                CleanUp();

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (instance && instance->GetData(eData::DataTrilliaxIntro) != EncounterState::DONE)
                        Intro();
                });

                _Reset();

                m_AnnihilationTimer = 0;
                m_SumImprintTimer = 0;

                m_RotateDirection = RotateDirection::ROTATE_DIRECTION_LEFT;

                m_TargetedSeepages.clear();

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSterilize);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSterilizeAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSterilizeAura2);

                    if (instance->GetData(eData::DataTrilliaxIntro) == EncounterState::DONE)
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    if (IsMythic())
                    {
                        instance->SetData(eData::DataTrilliaxImprintDoor, 0); ///< Close Cleaner door
                        instance->SetData(eData::DataTrilliaxImprintDoor, 2); ///< Close Maniac door
                    }
                }

                me->SummonCreature(eCreatures::NpcTrilliaxCopyCleaner, 475.52f, 3237.49f, -226.73f, 1.83f);
                me->SummonCreature(eCreatures::NpcTrilliaxCopyManiac, 383.61f, 3330.26f, -226.64f, 6.01f);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();

                Talk(eTalks::TalkAggro);

                DoCast(me, eSpells::SpellArcaneSeepage, true);
                DoCast(me, eSpells::SpellFormCleanerDummy, true);

                events.ScheduleEvent(eEvents::EventArcaneSlash, 7 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventToxicSlice, 11 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBerserker, 540 * TimeConstants::IN_MILLISECONDS);

                if (IsMythic())
                {
                    m_ImprintEntry = eCreatures::NpcTrilliaxCopyCleaner;
                    m_SumImprintTimer = 150 * TimeConstants::IN_MILLISECONDS; ///< 2m30s
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                CleanUp();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSterilizeAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellArcaneSlash);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellToxicSliceDoT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellToxicSliceHeroic);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSucculentFeastAbsorb);
                }
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                me->InterruptNonMeleeSpells(true);

                Talk(eTalks::TalkWipe);

                me->RemoveAura(eSharedSpells::SpellBerserkTrilliax);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSterilizeAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellArcaneSlash);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellToxicSliceDoT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellToxicSliceHeroic);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSucculentFeastAbsorb);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case c_actions::ACTION_1:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->RemoveAurasDueToSpell(eSpells::SpellDeadCosmetic);

                        Talk(eTalks::TalkIntro5);
                        break;
                    }
                    case c_actions::ACTION_2:
                    {
                        Talk(eTalks::TalkIntro1);

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro2);

                            AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::TalkIntro3);

                                AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    Talk(eTalks::TalkIntro4);
                                });
                            });
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellFormCleanerDummy:
                    case eSpells::SpellFormCleaner:
                    {
                        events.Reset();

                        me->InterruptNonMeleeSpells(false);

                        Talk(eTalks::TalkTheManiac);

                        DoCast(me, eSpells::SpellFormManiac, true);

                        events.ScheduleEvent(eEvents::EventArcaneSlash, 6 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventManaRupture, 2 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventArcingBonds, 5 * TimeConstants::IN_MILLISECONDS);

                        m_AnnihilationTimer = 20 * TimeConstants::IN_MILLISECONDS + 500;

                        if (IsMythic())
                        {
                            EntryCheckPredicate l_Pred(eCreatures::NpcTrilliaxCopyManiac);
                            summons.DoAction(c_actions::ACTION_2, l_Pred);
                        }

                        break;
                    }
                    case eSpells::SpellFormManiac:
                    {
                        events.Reset();

                        me->InterruptNonMeleeSpells(false);
                        me->RemoveAurasDueToSpell(eSpells::SpellManaRupture);

                        Talk(eTalks::TalkTheCaretaker);

                        DoCast(me, eSpells::SpellFormCaretaker, true);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_STUNNED);

                        events.ScheduleEvent(eEvents::EventSucculentFeast, 5 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventTidyUp, 12 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eSpells::SpellFormCaretaker:
                    {
                        events.Reset();

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        Talk(eTalks::TalkTheCleaner);

                        DoCast(me, eSpells::SpellFormCleaner, true);

                        events.ScheduleEvent(eEvents::EventArcaneSlash, 19 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventToxicSlice, 13 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventSterilize, 1 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventCleansingRage, 10 * TimeConstants::IN_MILLISECONDS + 500);

                        if (IsMythic())
                        {
                            EntryCheckPredicate l_Pred(eCreatures::NpcTrilliaxCopyCleaner);
                            summons.DoAction(c_actions::ACTION_2, l_Pred);
                        }

                        break;
                    }
                    case eSpells::SpellAnnihilationAT:
                    {
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == MovementGeneratorType::EFFECT_MOTION_TYPE)
                {
                    if (p_ID == eSpells::SpellAnnihilationJump)
                    {
                        me->GetMotionMaster()->Clear();

                        me->PlayOneShotAnimKitId(eVisual::AnnihilationVisual);

                        events.ScheduleEvent(eEvents::EventAnnihilation, 2 * TimeConstants::IN_MILLISECONDS);
                    }
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellToxicSlice:
                    {
                        Position l_Pos;

                        for (uint8 l_I = 0; l_I < (IsMythic() ? 8 : 5); ++l_I)
                        {
                            g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(20.0f, 43.0f), frand(0.0f, 2.0f * M_PI));
                            me->CastSpell(l_Pos, eSpells::SpellToxicSliceTrigger, true);
                        }

                        break;
                    }
                    case eSpells::SpellSucculentFeast:
                    {
                        Position l_Pos;

                        for (uint8 l_I = 0; l_I < (IsMythic() ? 4 : 2); ++l_I)
                        {
                            me->GetNearPosition(l_Pos, 25.0f, frand(0.0f, 2.0f * M_PI));
                            me->CastSpell(l_Pos, eSpells::SpellSucculentFeastSum, true);
                        }

                        break;
                    }
                    case eSpells::SpellTidyUp:
                    {
                        for (uint8 l_I = 0; l_I < 5; ++l_I)
                        {
                            AddTimedDelayedOperation(250 * l_I, [this, l_I]() -> void
                            {
                                if (me->isAlive())
                                    me->CastSpell(g_ScrubberPos[l_I], eSpells::SpellTidyUpSum, true);
                            });
                        }

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
                    case eSpells::SpellArcaneSeepageFilter:
                    {
                        DoCast(p_Target, eSpells::SpellArcaneSeepageMissile, true);
                        break;
                    }
                    case eSpells::SpellCleansingRageFilter:
                    {
                        DoCast(p_Target, eSpells::SpellCleansingRageMissile, true);
                        break;
                    }
                    case eSpells::SpellManaRuptureFilter:
                    {
                        DoCast(p_Target, eSpells::SpellManaRuptureMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool p_OnDespawn) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellArcaneSeepageAT)
                    m_TargetedSeepages.erase(p_AreaTrigger->GetGUID());
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                if (p_ID == eTrilliaxData::DataTargetedSeepage)
                    m_TargetedSeepages.insert(p_Guid);
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                if (p_ID == eTrilliaxData::DataRotationType)
                    m_RotateDirection = p_Value;
            }

            uint32 GetData(uint32 p_ID) override
            {
                if (p_ID == eTrilliaxData::DataRotationType)
                    return m_RotateDirection;

                return 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 100.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (m_AnnihilationTimer)
                {
                    if (m_AnnihilationTimer <= p_Diff)
                    {
                        m_AnnihilationTimer = 0;

                        events.Reset();

                        me->InterruptNonMeleeSpells(false);
                        me->AttackStop();
                        me->SetTarget(0);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        Position l_Pos = g_CenterPos;

                        l_Pos.m_orientation = me->GetAngle(&g_CenterPos);

                        DoCast(l_Pos, eSpells::SpellAnnihilationJump, true);
                    }
                    else
                        m_AnnihilationTimer -= p_Diff;
                }

                if (m_SumImprintTimer)
                {
                    if (m_SumImprintTimer <= p_Diff)
                    {
                        m_SumImprintTimer = 135 * TimeConstants::IN_MILLISECONDS; ///< 2m15s

                        EntryCheckPredicate l_Pred(m_ImprintEntry);
                        summons.DoAction(c_actions::ACTION_1, l_Pred); ///< Activate Golem

                        if (m_ImprintEntry == eCreatures::NpcTrilliaxCopyCleaner)
                            m_ImprintEntry = eCreatures::NpcTrilliaxCopyManiac;
                        else
                            m_SumImprintTimer = 0;
                    }
                    else
                        m_SumImprintTimer -= p_Diff;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArcaneSlash:
                    {
                        DoCastVictim(eSpells::SpellArcaneSlash);

                        if (me->HasAura(eSpells::SpellFormManiac))
                            events.ScheduleEvent(eEvents::EventArcaneSlash, 7 * TimeConstants::IN_MILLISECONDS + 300);
                        else
                            events.ScheduleEvent(eEvents::EventArcaneSlash, 11 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventAnnihilation:
                    {
                        Talk(eTalks::TalkAnnihilation);

                        DoCast(me, eSpells::SpellAnnihilationAT);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        m_RotateDirection = urand(0, 1) ? RotateDirection::ROTATE_DIRECTION_LEFT : RotateDirection::ROTATE_DIRECTION_RIGHT;
                        me->GetMotionMaster()->MoveRotate(20 * TimeConstants::IN_MILLISECONDS, RotateDirection(m_RotateDirection));
                        break;
                    }
                    case eEvents::EventToxicSlice:
                    {
                        Talk(eTalks::TalkToxicSlice);

                        DoCast(eSpells::SpellToxicSlice);
                        events.ScheduleEvent(eEvents::EventToxicSlice, 26 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSterilize:
                    {
                        DoCast(eSpells::SpellSterilize);
                        break;
                    }
                    case eEvents::EventCleansingRage:
                    {
                        Talk(eTalks::TalkCleansingRage);
                        DoCast(me, eSpells::SpellCleansingRageFilter);
                        break;
                    }
                    case eEvents::EventManaRupture:
                    {
                        DoCast(me, eSpells::SpellManaRupture, true);
                        break;
                    }
                    case eEvents::EventArcingBonds:
                    {
                        DoCast(eSpells::SpellArcingBondsFilter);
                        break;
                    }
                    case eEvents::EventSucculentFeast:
                    {
                        Talk(eTalks::TalkSucculentFeast);
                        DoCast(eSpells::SpellSucculentFeast);
                        break;
                    }
                    case eEvents::EventTidyUp:
                    {
                        Talk(eTalks::TalkTidyUp);
                        Talk(eTalks::TalkTidyUpWarn, me->GetGUID());
                        DoCast(eSpells::SpellTidyUp);
                        break;
                    }
                    case eEvents::EventBerserker:
                    {
                        DoCast(me, eSharedSpells::SpellBerserkTrilliax, true);
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
            return new boss_trilliaxAI(p_Creature);
        }
};

/// Sludgerax - 112255
class npc_trilliax_sludgerax : public CreatureScript
{
    public:
        npc_trilliax_sludgerax() : CreatureScript("npc_trilliax_sludgerax") { }

        struct npc_trilliax_sludgeraxAI : public ScriptedAI
        {
            npc_trilliax_sludgeraxAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            uint32 m_CheckPlayerTimer = 0;

            InstanceScript* m_Instance;
            uint8 m_PutriDies;

            bool m_IntroDone = false;

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoCast(me, eSpells::SpellBurstingSlimeAura, true);

                events.ScheduleEvent(eEvents::EventOozingRush, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTargetedHome();

                events.Reset();

                _EnterEvadeMode();

                if (me->GetVehicleKit())
                    me->GetVehicleKit()->Reset(true);

                me->RemoveAllAreasTrigger();
            }

            void JustReachedHome() override
            {
                Reset();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_IntroDone = false;

                m_CheckPlayerTimer = 1 * TimeConstants::IN_MILLISECONDS;

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);

                for (uint8 l_I = 0; l_I < 8; l_I++)
                    me->SummonCreature(eCreatures::NpcPutridSludge, g_SludgeraxSumPos[l_I]);

                m_PutriDies = 0;

                DoCast(me, eSpells::SpellBurstingSlime, true);
                DoCast(me, eSpells::SpellDeathCosmetic2, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBurstingSlimeSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellBurstingSlimeMissile, true);
                        break;
                    }
                    case eSpells::SpellOozingRushFixate:
                    {
                        DoCast(me, eSpells::SpellOozingRushAreaTrigger, true);
                        break;
                    }
                    case eSpells::SpellSludgeEruptionAoE:
                    {
                        DoCast(p_Target, eSpells::SpellSludgeEruptionAura, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Apply || !p_Passenger->IsPlayer())
                    return;

                uint64 l_Guid = p_Passenger->GetGUID();
                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                {
                    if (Unit* l_Passenger = Unit::GetUnit(*me, l_Guid))
                        DoCast(l_Passenger, eSpells::SpellOozingRushExit, true);
                });
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == c_actions::ACTION_1)
                {
                    m_PutriDies++;

                    if (m_PutriDies == 8)
                    {
                        me->RemoveAurasDueToSpell(eSpells::SpellDeathCosmetic2);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                        });
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance && m_Instance->GetData(eData::DataTrilliaxIntro) != DONE)
                {
                    m_Instance->SetData(eData::DataTrilliaxIntro, EncounterState::DONE);

                    if (Unit* l_Owner = me->GetAnyOwner())
                    {
                        if (l_Owner->GetAI())
                            l_Owner->GetAI()->DoAction(c_actions::ACTION_1);
                    }
                }

                DoCast(me, eSpells::SpellSludgeEruptionAoE, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_CheckPlayerTimer && !m_IntroDone && m_Instance)
                {
                    if (m_CheckPlayerTimer <= p_Diff)
                    {
                        if (me->FindNearestPlayer(45.0f))
                        {
                            m_CheckPlayerTimer = 0;

                            m_IntroDone = true;

                            if (Creature* l_Trilliax = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcTrilliax)))
                            {
                                if (l_Trilliax->IsAIEnabled)
                                    l_Trilliax->AI()->DoAction(c_actions::ACTION_2);
                            }
                        }
                        else
                            m_CheckPlayerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_CheckPlayerTimer -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventOozingRush:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, eSpells::SpellOozingRushFixate, true);

                        events.ScheduleEvent(eEvents::EventOozingRush, 33 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_trilliax_sludgeraxAI(p_Creature);
        }
};

/// Toxic Slice - 104547
/// Succulent Feast - 104561
class npc_trilliax_toxic_slice_succulent_feast : public CreatureScript
{
    public:
        npc_trilliax_toxic_slice_succulent_feast() : CreatureScript("npc_trilliax_toxic_slice_succulent_feast") { }

        struct npc_trilliax_toxic_slice_succulent_feastAI : public ScriptedAI
        {
            npc_trilliax_toxic_slice_succulent_feastAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            uint32 m_DespawnTimer = 0;

            bool m_TargetedByScrubber = false;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (me->GetEntry() == eCreatures::NpcToxicSlice)
                    DoCast(me, eSpells::SpellToxicSliceAT, true);
                else
                    DoCast(me, eSpells::SpellSucculentFeastAT, true);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellCleansingRageMissile)
                {
                    m_DespawnTimer = 500;

                    if (me->GetEntry() == eCreatures::NpcToxicSlice)
                        me->RemoveAurasDueToSpell(eSpells::SpellToxicSliceAT);
                    else
                        me->RemoveAurasDueToSpell(eSpells::SpellSucculentFeastAT);
                }
            }

            void OnAreaTriggerCast(Unit* /*p_Caster*/, Unit* p_Target, uint32 p_SpellID) override
            {
                if (p_SpellID == eSpells::SpellToxicSliceDoT || p_SpellID == eSpells::SpellSucculentFeastAbsorb)
                {
                    if (!IsLFR() && p_SpellID == eSpells::SpellToxicSliceDoT)
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                            l_Instance->SetData(eData::DataTrilliaxAchiev, 1);
                    }

                    if (p_Target && IsHeroicOrMythic())
                        p_Target->CastSpell(p_Target, eSpells::SpellToxicSliceHeroic, true);

                    me->DespawnOrUnsummon(100);
                }
            }

            void SetData(uint32 /*p_Type*/, uint32 /*p_Value*/) override
            {
                m_TargetedByScrubber = true;
            }

            uint32 GetData(uint32 /*p_Type*/) override
            {
                return uint32(m_TargetedByScrubber);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_DespawnTimer)
                {
                    if (m_DespawnTimer <= p_Diff)
                    {
                        m_DespawnTimer = 0;
                        me->DespawnOrUnsummon();
                    }
                    else
                        m_DespawnTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_trilliax_toxic_slice_succulent_feastAI(p_Creature);
        }
};

/// Scrubber - 104596
class npc_trilliax_scrubber : public CreatureScript
{
    public:
        npc_trilliax_scrubber() : CreatureScript("npc_trilliax_scrubber") { }

        struct npc_trilliax_scrubberAI : public ScriptedAI
        {
            npc_trilliax_scrubberAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            uint64 m_AreaTriggerGuid = 0;
            uint64 m_SliceGuid = 0;
            bool m_OnClick;

            uint32 m_ForceDespawnTimer = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_OnClick = false;

                me->setPowerType(Powers::POWER_MANA);
                me->SetMaxPower(Powers::POWER_MANA, 100);
                me->SetPower(Powers::POWER_MANA, 0);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);

                DoCast(me, eSpells::SpellEnergyAura, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                events.ScheduleEvent(c_events::EVENT_1, 2 * TimeConstants::IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == MovementGeneratorType::POINT_MOTION_TYPE)
                {
                    if (p_ID == 10)
                    {
                        /// Reschedule movement if stopped before reaching dest
                        if (m_AreaTriggerGuid)
                        {
                            if (AreaTrigger* l_AT = sObjectAccessor->GetAreaTrigger(*me, m_AreaTriggerGuid))
                            {
                                Position l_Pos = l_AT->GetPosition();
                                if (!me->IsNearPosition(&l_Pos, 0.5f))
                                {
                                    AddTimedDelayedOperation(10, [this]() -> void
                                    {
                                        if (AreaTrigger* l_AT = sObjectAccessor->GetAreaTrigger(*me, m_AreaTriggerGuid))
                                            me->GetMotionMaster()->MovePoint(10, l_AT->GetPosition());
                                    });

                                    return;
                                }
                            }
                        }
                        else if (m_SliceGuid)
                        {
                            if (Creature* l_Slice = Creature::GetCreature(*me, m_SliceGuid))
                            {
                                Position l_Pos = l_Slice->GetPosition();
                                if (!me->IsNearPosition(&l_Pos, 0.5f))
                                {
                                    AddTimedDelayedOperation(10, [this]() -> void
                                    {
                                        if (Creature* l_Slice = Creature::GetCreature(*me, m_SliceGuid))
                                            me->GetMotionMaster()->MovePoint(10, l_Slice->GetPosition());
                                    });

                                    return;
                                }
                            }
                        }

                        events.ScheduleEvent(c_events::EVENT_2, 100);
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == c_actions::ACTION_1)
                {
                    events.Reset();

                    if (!me->HasAura(eSpells::SpellEnergyState2))
                        DoCast(me, eSpells::SpellEnergyState2, true);

                    ApplyAllImmunities(true);

                    DoCast(eSpells::SpellCleansingDestruction);

                    me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSpells::SpellCleansingDestruction)
                {
                    m_OnClick = true;

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(500);
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellRideVehicle:
                    {
                        if (m_OnClick)
                            break;

                        m_OnClick = true;
                        me->InterruptNonMeleeSpells(false);
                        DoCast(p_Caster, eSpells::SpellCleansingDestructionPlayer, true);
                        me->DespawnOrUnsummon(500);
                        break;
                    }
                    case eSpells::SpellSterilizeFilter2:
                    {
                        p_Caster->CastSpell(me, eSpells::SpellSterilizeEnergize, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (events.Empty() && !me->IsMoving())
                {
                    m_ForceDespawnTimer += p_Diff;

                    /// Despawn Scrubber after 10 seconds doing nothing
                    if (m_ForceDespawnTimer >= 10 * TimeConstants::IN_MILLISECONDS)
                    {
                        m_ForceDespawnTimer = 0;
                        me->DespawnOrUnsummon();
                        return;
                    }
                }

                switch (events.ExecuteEvent())
                {
                    case c_events::EVENT_1:
                    {
                        m_AreaTriggerGuid = 0;
                        m_SliceGuid = 0;

                        std::list<Creature*> l_SliceList;

                        me->GetCreatureListWithEntryInGrid(l_SliceList, eCreatures::NpcToxicSlice, 100.0f);

                        if (!l_SliceList.empty())
                            l_SliceList.sort(JadeCore::UnitSortDistance(true, me));

                        float l_Distance = 100000.0f;

                        for (Creature* l_Slice : l_SliceList)
                        {
                            /// Slice already targeted by a Scrubber
                            if (!l_Slice->IsAIEnabled || l_Slice->AI()->GetData())
                                continue;

                            /// Register distance + Slice guid
                            l_Distance = l_Slice->GetDistance(me);
                            m_SliceGuid = l_Slice->GetGUID();
                            break;
                        }

                        Creature* l_Trilliax = nullptr;

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                            l_Trilliax = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcTrilliax));

                        boss_trilliax::boss_trilliaxAI* l_AI = CAST_AI(boss_trilliax::boss_trilliaxAI, l_Trilliax != nullptr ? l_Trilliax->GetAI() : nullptr);

                        std::list<AreaTrigger*> l_AreaTriggers;

                        if (l_Trilliax != nullptr && l_AI != nullptr)
                        {
                            l_Trilliax->GetAreaTriggerList(l_AreaTriggers, eSpells::SpellArcaneSeepageAT);

                            if (!l_AreaTriggers.empty())
                                l_AreaTriggers.sort(JadeCore::UnitSortDistance(true, me));

                            for (AreaTrigger* l_Seepage : l_AreaTriggers)
                            {
                                /// Seepage already targeted by a Scrubber
                                if (l_AI->m_TargetedSeepages.find(l_Seepage->GetGUID()) != l_AI->m_TargetedSeepages.end())
                                    continue;

                                /// If Seepage is nearest to the Scrubber than the nearest Slice, choose & register it
                                if (me->GetDistance(l_Seepage) < l_Distance)
                                {
                                    m_SliceGuid = 0;
                                    m_AreaTriggerGuid = l_Seepage->GetGUID();

                                    l_Trilliax->AI()->SetGUID(m_AreaTriggerGuid, eTrilliaxData::DataTargetedSeepage);
                                    break;
                                }
                                /// If not, choose the previously registered Slice
                                else
                                {
                                    m_AreaTriggerGuid = 0;

                                    if (Creature* l_Slice = Creature::GetCreature(*me, m_SliceGuid))
                                    {
                                        if (l_Slice->IsAIEnabled)
                                            l_Slice->AI()->SetData(0, 0);
                                    }
                                }
                            }

                            /// If no Slice nor Seepage have been selected, despawn Scrubber
                            if (!m_SliceGuid && !m_AreaTriggerGuid)
                            {
                                me->DespawnOrUnsummon(2 * TimeConstants::IN_MILLISECONDS);
                                break;
                            }

                            if (m_SliceGuid)
                            {
                                if (Creature* l_Slice = Creature::GetCreature(*me, m_SliceGuid))
                                    me->GetMotionMaster()->MovePoint(10, l_Slice->GetPosition());
                            }
                            else if (m_AreaTriggerGuid)
                            {
                                if (AreaTrigger* l_Seepage = sObjectAccessor->GetAreaTrigger(*me, m_AreaTriggerGuid))
                                    me->GetMotionMaster()->MovePoint(10, l_Seepage->GetPosition());
                            }
                        }

                        break;
                    }
                    case c_events::EVENT_2:
                    {
                        if (m_SliceGuid)
                        {
                            if (Creature* l_Slice = ObjectAccessor::GetCreature(*me, m_SliceGuid))
                            {
                                DoCast(eSpells::SpellScrubbingMax);
                                l_Slice->DespawnOrUnsummon();
                                break;
                            }
                        }
                        else if (m_AreaTriggerGuid)
                        {
                            if (AreaTrigger* l_AreaTrigger = ObjectAccessor::GetAreaTrigger(*me, m_AreaTriggerGuid))
                            {
                                l_AreaTrigger->Remove();
                                DoCast(eSpells::SpellScrubbingMin);
                            }
                        }

                        events.ScheduleEvent(c_events::EVENT_1, 2000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_trilliax_scrubberAI(p_Creature);
        }
};

/// Trilliax's Imprint - 108144, 108303
class npc_trilliax_imprint : public CreatureScript
{
    public:
        npc_trilliax_imprint() : CreatureScript("npc_trilliax_imprint") { }

        enum eVisual
        {
            AnnihilationVisual = 625
        };

        struct npc_trilliax_imprintAI : public ScriptedAI
        {
            npc_trilliax_imprintAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            InstanceScript* m_Instance;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                ApplyAllImmunities(true);

                me->SetMaxPower(Powers::POWER_MANA, 100);
                me->SetPower(Powers::POWER_MANA, 0);

                DoCast(me, eSpells::SpellDeactivatedConstruct, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_Type)
                {
                    case MovementGeneratorType::POINT_MOTION_TYPE:
                    {
                        if (p_ID == 1)
                        {
                            if (m_Instance && me->GetEntry() == eCreatures::NpcTrilliaxCopyCleaner)
                            {
                                m_Instance->SetData(eData::DataTrilliaxImprintDoor, 0); ///< Close Cleaner Door
                                events.ScheduleEvent(c_events::EVENT_3, 20 * TimeConstants::IN_MILLISECONDS);
                            }
                            else if (m_Instance)
                            {
                                m_Instance->SetData(eData::DataTrilliaxImprintDoor, 2); ///< Close Maniac Door
                                DoCast(me, eSpells::SpellManaRupture, true);
                            }

                            events.ScheduleEvent(c_events::EVENT_6, 1 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case MovementGeneratorType::EFFECT_MOTION_TYPE:
                    {
                        if (p_ID == eSpells::SpellAnnihilationJump)
                        {
                            me->GetMotionMaster()->Clear();

                            me->PlayOneShotAnimKitId(eVisual::AnnihilationVisual);

                            events.ScheduleEvent(c_events::EVENT_7, 2 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case c_actions::ACTION_1:
                    {
                        me->RemoveAurasDueToSpell(eSpells::SpellDeactivatedConstruct);

                        if (m_Instance)
                        {
                            if (me->GetEntry() == eCreatures::NpcTrilliaxCopyCleaner)
                                m_Instance->SetData(eData::DataTrilliaxImprintDoor, 1); ///< Open Cleaner Door
                            else
                                m_Instance->SetData(eData::DataTrilliaxImprintDoor, 3); ///< Open Maniac Door

                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                        }

                        events.ScheduleEvent(c_events::EVENT_1, 2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_actions::ACTION_2:
                    {
                        if (me->isInCombat() && me->isAlive())
                        {
                            if (Unit* l_Owner = me->GetAnyOwner())
                            {
                                if (me->GetEntry() == eCreatures::NpcTrilliaxCopyCleaner)
                                    l_Owner->CastSpell(me, eSpells::SpellDualPersonalitiesCleaner, true);
                                else
                                    l_Owner->CastSpell(me, eSpells::SpellDualPersonalitiesManiac, true);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellToxicSliceCopy:
                    {
                        Position l_Pos;

                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(20.0f, 43.0f), frand(0.0f, 2.0f * M_PI));

                            if (Unit* l_Owner = me->GetAnyOwner())
                                me->CastSpell(l_Pos, eSpells::SpellToxicSliceTrigger, true, nullptr, nullptr, l_Owner->GetGUID());
                        }

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
                    case eSpells::SpellCleansingRageCopy:
                    {
                        DoCast(p_Target, eSpells::SpellCleansingRageMissile, true);
                        break;
                    }
                    case eSpells::SpellManaRuptureFilter:
                    {
                        DoCast(p_Target, eSpells::SpellManaRuptureMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellAnnihilationATCopy:
                    {
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();

                        events.ScheduleEvent(c_events::EVENT_5, 500);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() && me->isInCombat())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case c_events::EVENT_1:
                    {
                        if (me->GetEntry() == eCreatures::NpcTrilliaxCopyCleaner)
                            me->GetMotionMaster()->MovePoint(1, 466.93f, 3271.80f, -225.67f);
                        else
                            me->GetMotionMaster()->MovePoint(1, 420.84f, 3319.18f, -225.69f);

                        break;
                    }
                    case c_events::EVENT_2:
                    {
                        DoCast(me, eSpells::SpellEnergized, true);
                        events.ScheduleEvent(c_events::EVENT_2, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_events::EVENT_3:
                    {
                        DoCast(eSpells::SpellToxicSliceCopy);
                        events.ScheduleEvent(c_events::EVENT_3, 16 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_events::EVENT_4:
                    {
                        if (me->GetPower(Powers::POWER_MANA) >= 100)
                        {
                            if (me->GetEntry() == eCreatures::NpcTrilliaxCopyCleaner)
                                DoCast(eSpells::SpellCleansingRageCopy);
                            else
                            {
                                me->InterruptNonMeleeSpells(false);
                                me->AttackStop();
                                me->SetTarget(0);

                                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                                me->SetReactState(ReactStates::REACT_PASSIVE);

                                Position l_Pos = g_CenterPos;

                                l_Pos.m_orientation = me->GetAngle(&g_CenterPos);

                                DoCast(l_Pos, eSpells::SpellAnnihilationJump, true);
                            }
                        }

                        events.ScheduleEvent(c_events::EVENT_4, 2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_events::EVENT_5:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Target);
                        }

                        break;
                    }
                    case c_events::EVENT_6:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 100.0f);
                        events.ScheduleEvent(c_events::EVENT_2, 100);
                        events.ScheduleEvent(c_events::EVENT_4, 2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_events::EVENT_7:
                    {
                        DoCast(me, eSpells::SpellAnnihilationATCopy);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        RotateDirection l_RotateDirection = urand(0, 1) ? RotateDirection::ROTATE_DIRECTION_LEFT : RotateDirection::ROTATE_DIRECTION_RIGHT;
                        me->GetMotionMaster()->MoveRotate(20 * TimeConstants::IN_MILLISECONDS, l_RotateDirection);
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
            return new npc_trilliax_imprintAI(p_Creature);
        }
};

/// Annihilation - 207630
class spell_trilliax_annihilation : public SpellScriptLoader
{
    public:
        spell_trilliax_annihilation() : SpellScriptLoader("spell_trilliax_annihilation") { }

        class spell_trilliax_annihilation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_trilliax_annihilation_AuraScript);

            uint32 m_RandTimer = 0;

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMap()->IsHeroicOrMythic() && l_Caster->GetEntry() == eCreatures::NpcTrilliax)
                        m_RandTimer = urand(3, 9) * TimeConstants::IN_MILLISECONDS;
                }
            }

            void OnUpdate(uint32 p_Diff, AuraEffect* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (m_RandTimer && l_Caster->IsAIEnabled)
                    {
                        if (m_RandTimer <= p_Diff)
                        {
                            RotateDirection l_CurrDir = RotateDirection(l_Caster->AI()->GetData());

                            if (l_CurrDir == RotateDirection::ROTATE_DIRECTION_LEFT)
                                l_CurrDir = RotateDirection::ROTATE_DIRECTION_RIGHT;
                            else
                                l_CurrDir = RotateDirection::ROTATE_DIRECTION_LEFT;

                            l_Caster->AI()->SetData(eTrilliaxData::DataRotationType, l_CurrDir);

                            l_Caster->GetMotionMaster()->Clear();
                            l_Caster->GetMotionMaster()->MoveRotate(20 * TimeConstants::IN_MILLISECONDS, l_CurrDir);

                            m_RandTimer = urand(3, 9) * TimeConstants::IN_MILLISECONDS;
                        }
                        else
                            m_RandTimer -= p_Diff;
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_trilliax_annihilation_AuraScript::OnApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_AREATRIGGER, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectUpdate += AuraEffectUpdateFn(spell_trilliax_annihilation_AuraScript::OnUpdate, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_AREATRIGGER);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_trilliax_annihilation_AuraScript();
        }
};

/// Arcing Bonds - 208924
class spell_trilliax_arcing_bonds_filter : public SpellScriptLoader
{
    public:
        spell_trilliax_arcing_bonds_filter() : SpellScriptLoader("spell_trilliax_arcing_bonds_filter") { }

        class spell_trilliax_arcing_bonds_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_trilliax_arcing_bonds_filter_SpellScript);

            uint8 m_Count = 0;
            uint8 m_TargetMaxCount = 0;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!GetCaster())
                    return;

                m_TargetMaxCount = GetCaster()->GetMap()->GetPlayerCount();

                if (m_TargetMaxCount < 15)
                    m_TargetMaxCount = 2;
                else if (m_TargetMaxCount < 25)
                    m_TargetMaxCount = 4;
                else
                    m_TargetMaxCount = 6;

                std::list<Player*> l_PlayerList;
                for (auto const& l_Object : p_Targets)
                {
                    if (Player* l_Player = l_Object->ToPlayer())
                    {
                        if (l_Player->GetRoleForGroup() != Roles::ROLE_TANK)
                            l_PlayerList.push_back(l_Player);
                    }
                }

                while (l_PlayerList.size() > 1)
                {
                    Player* l_PlayerA = JadeCore::Containers::SelectRandomContainerElement(l_PlayerList);

                    l_PlayerList.remove(l_PlayerA);

                    Player* l_PlayerB = JadeCore::Containers::SelectRandomContainerElement(l_PlayerList);

                    l_PlayerList.remove(l_PlayerB);

                    if (l_PlayerA && l_PlayerB)
                    {
                        l_PlayerA->CastSpell(l_PlayerB, eSpells::SpellArcingBondsPeriodic, true);
                        l_PlayerB->CastSpell(l_PlayerA, eSpells::SpellArcingBondsDummy, true);
                    }

                    m_Count += 2;

                    if (m_Count >= m_TargetMaxCount)
                        break;
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_trilliax_arcing_bonds_filter_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_trilliax_arcing_bonds_filter_SpellScript();
        }
};

/// Arcing Bonds - 208910
class spell_trilliax_arcing_bonds : public SpellScriptLoader
{
    public:
        spell_trilliax_arcing_bonds() : SpellScriptLoader("spell_trilliax_arcing_bonds") { }

        class spell_trilliax_arcing_bonds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_trilliax_arcing_bonds_AuraScript);

            void OnPeriodic(AuraEffect const* p_AurEff)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (!GetCaster()->HasAura(eSpells::SpellArcingBondsDummy))
                {
                    p_AurEff->GetBase()->Remove();
                    return;
                }

                if (GetCaster()->GetDistance(GetTarget()) >= 5.0f)
                {
                    GetCaster()->CastSpell(GetTarget(), eSpells::SpellArcingBondsDmg, true);
                    GetTarget()->CastSpell(GetCaster(), eSpells::SpellArcingBondsDmg, true);
                }
                else
                {
                    GetCaster()->CastSpell(GetTarget(), eSpells::SpellArcingBondsVisual, true);
                    GetTarget()->CastSpell(GetCaster(), eSpells::SpellArcingBondsVisual, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_trilliax_arcing_bonds_AuraScript::OnPeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_trilliax_arcing_bonds_AuraScript();
        }
};

/// Energy Aura - 224470
class spell_trilliax_energy_aura : public SpellScriptLoader
{
    public:
        spell_trilliax_energy_aura() : SpellScriptLoader("spell_trilliax_energy_aura") { }

        class spell_trilliax_energy_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_trilliax_energy_aura_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (l_Caster->GetPower(Powers::POWER_MANA) >= 100)
                    {
                        if (l_Caster->IsAIEnabled)
                            l_Caster->AI()->DoAction(c_actions::ACTION_1);

                        p_AurEff->GetBase()->Remove();
                    }
                    else if (l_Caster->GetPower(Powers::POWER_MANA) > 70)
                    {
                        if (!l_Caster->HasAura(eSpells::SpellEnergyState2))
                            l_Caster->CastSpell(l_Caster, eSpells::SpellEnergyState2, true);
                    }
                    else if (l_Caster->GetPower(Powers::POWER_MANA) > 20)
                    {
                        if (!l_Caster->HasAura(eSpells::SpellEnergyState1))
                            l_Caster->CastSpell(l_Caster, eSpells::SpellEnergyState1, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_trilliax_energy_aura_AuraScript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_trilliax_energy_aura_AuraScript();
        }
};

/// Sterilize - 208500
class spell_trilliax_sterilize : public SpellScriptLoader
{
    public:
        spell_trilliax_sterilize() : SpellScriptLoader("spell_trilliax_sterilize") { }

        class spell_trilliax_sterilize_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_trilliax_sterilize_SpellScript);

            void HandleDummy(SpellEffIndex p_EffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                        l_Caster->CastSpell(l_Target, GetSpellInfo()->Effects[p_EffIndex].BasePoints, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_trilliax_sterilize_SpellScript::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_trilliax_sterilize_SpellScript();
        }
};

/// Oozing Rush - 223671
class areatrigger_trilliax_oozing_rush : public AreaTriggerEntityScript
{
    public:
        areatrigger_trilliax_oozing_rush() : AreaTriggerEntityScript("areatrigger_trilliax_oozing_rush") { }

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return true;

            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                Vehicle* l_Vehicle = l_Caster->GetVehicleKit();
                if (!l_Vehicle)
                    return true;

                for (int8 l_I = 0; l_I < MAX_VEHICLE_SEATS; ++l_I)
                {
                    if (Unit* l_Passenger = l_Vehicle->GetPassenger(l_I))
                    {
                        if (!l_Passenger->GetVehicleKit())
                            continue;

                        int8 l_SeatID = l_Passenger->GetVehicleKit()->GetNextEmptySeat(0, true);
                        if (l_SeatID != -1)
                        {
                            p_Target->CastSpell(l_Passenger, eSpells::SpellOozingRushEnter, true);
                            l_Caster->CastSpell(p_Target, eSpells::SpellOozingRushDamage, true);
                            return true;
                        }
                    }
                }
            }

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_trilliax_oozing_rush();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_trilliax()
{
    /// Boss
    new boss_trilliax();

    /// Creatures
    new npc_trilliax_sludgerax();
    new npc_trilliax_toxic_slice_succulent_feast();
    new npc_trilliax_scrubber();
    new npc_trilliax_imprint();

    /// Spells
    new spell_trilliax_annihilation();
    new spell_trilliax_arcing_bonds_filter();
    new spell_trilliax_arcing_bonds();
    new spell_trilliax_energy_aura();
    new spell_trilliax_sterilize();

    /// AreaTrigger
    new areatrigger_trilliax_oozing_rush();
}
#endif
