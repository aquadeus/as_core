////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

/// Archmage Xylem - 115244
class boss_archmage_xylem : public CreatureScript
{
    public:
        boss_archmage_xylem() : CreatureScript("boss_archmage_xylem") { }

        enum eSpells
        {
            SpellBreakFocusStart        = 231559,
            SpellBreakFocusEnd          = 231920,
            SpellBreakFocusATs          = 231563,
            SpellBreakFocusATBack       = 243820,
            SpellBreakFocusATLeft       = 243821,
            SpellBreakFocusATRight      = 243822,

            SpellPowerReturnedVisual    = 233531,
            SpellPowerReturnedScreen    = 233533,
            SpellPowerReturnedSkybox    = 233552,

            SpellFrostbolt              = 242316,

            SpellBlink                  = 242015,
            SpellBlinkTeleport          = 231612,
            SpellRazorIceStun           = 232661,
            SpellCometStormAT           = 242208,
            SpellCometStormVisual       = 242210,
            SpellCometStormDamage       = 242216,

            SpellBlinkIntermission      = 232782,
            SpellBlinkInterTeleport     = 232037,
            SpellCloakOfGlittering      = 232025,

            SpellArcaneAnnihilation     = 234728,
            SpellDarknessAoE            = 232007,

            SpellArcaneBarrage          = 232673,

            SpellShadowBarrage          = 231443,

            SpellDrawPower              = 231522,

            SpellThrowBook              = 233489,
            SpellBlinkEnd               = 233486,
            SpellReturnPower            = 233492,
            SpellBanishShadow           = 232750,

            SpellSummonPortalsDummy     = 204279
        };

        enum eEvents
        {
            EventFrostbolt = 1,
            EventRazorIce,
            EventPhaseChange,
            EventIntermission,
            EventArcaneBarrage,
            EventShadowBarrage,
            EventDrawPower
        };

        enum ePhases
        {
            PhaseFrost,
            PhaseArcaneShadow,
            PhaseMirrorImage
        };

        enum eTalks
        {
            TalkIntro0,
            TalkIntro1,
            TalkIntro2,
            TalkAggro,
            TalkAnnihilation,
            TalkCorruptingShadow0,
            TalkCorruptingShadow1,
            TalkCorruptingShadow2,
            TalkCorruptingShadow3,
            TalkOutro0,
            TalkOutro1,
            TalkOutro2
        };

        enum eAction
        {
            ActionChallengeEnd
        };

        struct boss_archmage_xylemAI : public BossAI
        {
            boss_archmage_xylemAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            uint8 m_PhaseID = ePhases::PhaseFrost;

            bool m_RazorIce = false;

            std::vector<Position> m_RazorIceRelPos =
            {
                { 0.0000f, -4.500f, -0.551f, 4.501f },
                { 1.0000f, -7.000f, -0.551f, 4.501f },
                { -4.048f, 3.2440f, -0.551f, 4.501f },
                { 5.6570f, -6.656f, -0.551f, 4.501f },
                { 3.1820f, -3.182f, -0.551f, 4.501f },
                { 8.0000f, 0.0000f, -0.551f, 4.501f },
                { 0.8540f, 3.9390f, -0.551f, 4.501f },
                { 4.6570f, 5.6570f, -0.551f, 4.501f },
                { 4.7570f, 3.6910f, -0.551f, 4.501f },
                { 1.0000f, 8.0000f, -0.551f, 4.501f },
                { 5.9290f, -3.534f, -0.551f, 4.501f },
                { -6.657f, 4.6570f, -0.551f, 4.501f },
                { -3.492f, -6.457f, -0.551f, 4.501f },
                { -8.000f, 1.0000f, -0.551f, 4.501f },
                { 3.1820f, 3.1820f, -0.551f, 4.501f },
                { -3.182f, -3.182f, -0.551f, 4.501f },
                { -3.182f, 3.1820f, -0.551f, 4.501f },
                { 3.1820f, -3.182f, -0.551f, 4.501f },
                { 0.0000f, 4.5000f, -0.551f, 4.501f },
                { 0.0000f, -4.500f, -0.551f, 4.501f },
                { 0.0000f, 4.5000f, -0.551f, 4.501f },
                { 4.5000f, 0.0000f, -0.551f, 4.501f },
                { -4.500f, 0.0000f, -0.551f, 4.501f },
                { 3.1820f, 3.1820f, -0.551f, 4.501f },
                { 4.5000f, 0.0000f, -0.551f, 4.501f },
                { -4.500f, 0.0000f, -0.551f, 4.501f },
                { -3.182f, 3.1820f, -0.551f, 4.501f },
                { -4.657f, -6.656f, -0.551f, 4.501f },
                { -3.182f, -3.182f, -0.551f, 4.501f }
            };

            std::array<Position const, eData::DataXylemDummyCount> m_XylemDummyPos =
            {
                {
                    { 821.5972f, 1335.257f, 266.2537f, 3.2411890f },
                    { 815.8906f, 1351.786f, 266.2537f, 3.9956030f },
                    { 773.6371f, 1318.818f, 267.3175f, 0.4366551f },
                    { 796.8316f, 1295.804f, 267.3175f, 1.2902850f },
                    { 791.6493f, 1361.182f, 267.3175f, 5.1751670f },
                    { 788.0504f, 1372.198f, 267.3175f, 5.1350100f },
                    { 788.7743f, 1332.769f, 266.2537f, 6.2515920f },
                    { 770.6893f, 1374.453f, 268.3141f, 5.4170150f },
                    { 808.8403f, 1312.372f, 266.2537f, 2.0857800f },
                    { 770.3542f, 1361.288f, 267.3175f, 5.6150820f },
                    { 784.3768f, 1346.635f, 266.2537f, 5.7087410f },
                    { 786.2344f, 1285.227f, 268.3141f, 1.1939770f },
                    { 801.4722f, 1352.863f, 266.2537f, 5.1167120f },
                    { 770.7031f, 1338.254f, 267.3175f, 0.0801611f },
                    { 763.9393f, 1301.193f, 268.3141f, 0.6641029f },
                    { 822.3125f, 1322.214f, 266.2537f, 2.0857800f },
                    { 756.3246f, 1339.792f, 268.3141f, 6.1584150f },
                    { 787.3924f, 1304.450f, 267.3175f, 0.9195645f },
                    { 810.2292f, 1365.866f, 267.3175f, 4.7695580f },
                    { 777.1077f, 1295.717f, 267.3175f, 2.0857800f },
                    { 812.8837f, 1297.264f, 267.3175f, 1.6760080f },
                    { 756.6371f, 1321.457f, 268.3141f, 0.2448308f },
                    { 794.8264f, 1317.632f, 266.2537f, 1.1133160f },
                    { 800.6285f, 1381.859f, 268.3141f, 4.3212680f },
                    { 848.7292f, 1333.642f, 267.3175f, 3.1735870f },
                    { 819.8108f, 1374.684f, 267.3175f, 3.9662880f },
                    { 836.4219f, 1341.700f, 267.3175f, 3.3210020f },
                    { 856.8698f, 1324.347f, 268.3141f, 3.0498120f },
                    { 832.5156f, 1294.087f, 267.9553f, 2.5248420f },
                    { 842.4809f, 1312.181f, 267.3175f, 2.8433630f },
                    { 844.0330f, 1299.602f, 268.3141f, 2.6812340f },
                    { 834.2847f, 1317.720f, 267.3175f, 2.8948040f },
                    { 834.0486f, 1375.552f, 268.3141f, 3.8246540f },
                    { 828.5781f, 1302.729f, 267.3175f, 2.6093130f },
                    { 849.5139f, 1348.457f, 267.3675f, 3.3811590f },
                    { 832.6719f, 1352.036f, 267.3175f, 3.5133370f },
                    { 815.2396f, 1282.889f, 268.3141f, 2.2008780f }
                }
            };

            bool m_Hidden = false;

            uint32 m_DarknessTimer = 0;

            bool m_FromFrost = false;

            bool m_CorruptingShadowPhase = false;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                _Reset();

                SetCombatMovement(false);

                m_PhaseID = ePhases::PhaseFrost;

                m_RazorIce = false;

                m_Hidden = false;

                m_DarknessTimer = 0;

                m_FromFrost = false;

                m_CorruptingShadowPhase = false;

                me->SetReactState(ReactStates::REACT_PASSIVE);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellBreakFocusStart, true);
                });

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Player* l_Target = me->FindNearestPlayer(50.0f))
                        Talk(eTalks::TalkIntro0, l_Target->GetGUID());

                    AddTimedDelayedOperation(13 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::TalkIntro1);

                        AddTimedDelayedOperation(12 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro2);

                            AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (!instance)
                                    return;

                                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());
                                Player* l_Player = nullptr;

                                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                                {
                                    if (Player* l_Plr = l_Iter->getSource())
                                    {
                                        l_Player = l_Plr;
                                        break;
                                    }
                                }

                                if (!l_Scenario || !l_Player)
                                    return;

                                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataIntroAsset, 0, 0, l_Player, l_Player);

                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                me->InterruptNonMeleeSpells(true);

                                DoCast(me, eSpells::SpellBreakFocusEnd, true);

                                AttackStart(l_Player, false);
                            });
                        });
                    });
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                Talk(eTalks::TalkAggro);

                DefaultEvents(ePhases::PhaseFrost, true);
            }

            void DefaultEvents(uint8 p_PhaseID, bool p_EnterCombat = false)
            {
                m_PhaseID = p_PhaseID;

                switch (m_PhaseID)
                {
                    case ePhases::PhaseFrost:
                    {
                        m_FromFrost = true;

                        events.ScheduleEvent(eEvents::EventFrostbolt, 1);
                        events.ScheduleEvent(eEvents::EventRazorIce, 10 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventPhaseChange, 90 * TimeConstants::IN_MILLISECONDS);

                        /// Shadow Barrage is present too after first Frost phase
                        if (!p_EnterCombat)
                            events.ScheduleEvent(eEvents::EventShadowBarrage, 2 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case ePhases::PhaseArcaneShadow:
                    {
                        m_FromFrost = false;

                        events.ScheduleEvent(eEvents::EventArcaneBarrage, 1);
                        events.ScheduleEvent(eEvents::EventShadowBarrage, 2 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventDrawPower, 30 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventPhaseChange, 50 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case ePhases::PhaseMirrorImage:
                    {
                        events.ScheduleEvent(eEvents::EventIntermission, 1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (!instance)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());
                Player* l_Player = nullptr;

                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Plr = l_Iter->getSource())
                    {
                        l_Player = l_Plr;
                        break;
                    }
                }

                if (!l_Scenario || !l_Player)
                    return;

                ///l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataJormogAsset, 0, 0, l_Player, l_Player);
            }

            void EnterEvadeMode() override
            {
                if (me->getFaction() == FactionTemplates::FACTION_FRIENDLY)
                    return;

                /// Only evades if player is dead
                if (instance && !instance->IsWipe())
                    return;

                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);

                me->DespawnOrUnsummon();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBlinkTeleport:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            m_RazorIce = false;

                            if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            {
                                DoCast(l_Target, eSpells::SpellCometStormAT, true);

                                Position l_Pos = l_Target->GetPosition();

                                uint32 l_Time = 1 * TimeConstants::IN_MILLISECONDS + 500;
                                for (uint8 l_I = 0; l_I < 9; ++l_I)
                                {
                                    AddTimedDelayedOperation(l_Time, [this, l_Pos]() -> void
                                    {
                                        DoCast(l_Pos, eSpells::SpellCometStormDamage, true);
                                    });

                                    l_Time += 1 * TimeConstants::IN_MILLISECONDS + 500;
                                }

                                l_Time = 0;

                                for (uint8 l_I = 0; l_I < 75; ++l_I)
                                {
                                    if (l_Time)
                                    {
                                        AddTimedDelayedOperation(l_Time, [this, l_Pos]() -> void
                                        {
                                            DoCast(l_Pos, eSpells::SpellCometStormVisual, true);
                                        });
                                    }
                                    else
                                        DoCast(l_Pos, eSpells::SpellCometStormVisual, true);

                                    l_Time += 200;
                                }

                                for (Position const& l_RelPos : m_RazorIceRelPos)
                                {
                                    Position l_SpawnPos =
                                    {
                                        l_Pos.m_positionX + l_RelPos.m_positionX,
                                        l_Pos.m_positionY + l_RelPos.m_positionY,
                                        l_Pos.m_positionZ + l_RelPos.m_positionZ,
                                        l_Pos.m_orientation + l_RelPos.m_orientation
                                    };

                                    me->SummonCreature(eCreatures::CreatureRazorIce, l_SpawnPos);
                                }
                            }
                        });

                        break;
                    }
                    case eSpells::SpellBlinkInterTeleport:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                            for (Position const& l_Pos : m_XylemDummyPos)
                                me->SummonCreature(eCreatures::CreatureArchmageXylemDummy, l_Pos);

                            AddTimedDelayedOperation(500, [this]() -> void
                            {
                                m_PhaseID = ePhases::PhaseMirrorImage;

                                DoCast(me, eSpells::SpellArcaneAnnihilation);

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    m_Hidden = true;

                                    m_DarknessTimer = 1 * TimeConstants::IN_MILLISECONDS;
                                });

                                Talk(eTalks::TalkAnnihilation, me->GetGUID());
                            });
                        });

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
                    case eSpells::SpellArcaneAnnihilation:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            m_Hidden = false;

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                            me->RemoveAura(eSpells::SpellCloakOfGlittering);

                            me->InterruptNonMeleeSpells(true);

                            summons.DespawnEntry(eCreatures::CreatureArchmageXylemDummy);

                            m_DarknessTimer = 0;

                            DefaultEvents(m_FromFrost ? ePhases::PhaseArcaneShadow : ePhases::PhaseFrost);
                        });

                        break;
                    }
                    case eSpells::SpellShadowBarrage:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        });

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
                    case eSpells::SpellBreakFocusEnd:
                    {
                        Position l_Pos = p_Target->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 12.0f, 0.0f);

                        /// Front AT
                        p_Target->CastSpell(l_Pos, eSpells::SpellBreakFocusATs, true);

                        l_Pos = p_Target->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 12.0f, -(M_PI / 2.0f));
                        l_Pos.m_orientation += -(M_PI / 2.0f);

                        p_Target->CastSpell(l_Pos, eSpells::SpellBreakFocusATRight, true);

                        l_Pos = p_Target->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 12.0f, M_PI);
                        l_Pos.m_orientation += M_PI;

                        p_Target->CastSpell(l_Pos, eSpells::SpellBreakFocusATBack, true);

                        l_Pos = p_Target->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 12.0f, M_PI / 2.0f);
                        l_Pos.m_orientation += M_PI / 2.0f;

                        p_Target->CastSpell(l_Pos, eSpells::SpellBreakFocusATLeft, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool /*p_OnDespawn*/) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellCometStormAT)
                {
                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        summons.DespawnEntry(eCreatures::CreatureRazorIce);
                    });
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_CorruptingShadowPhase && me->HealthBelowPctDamaged(10, p_Damage))
                {
                    events.Reset();

                    me->SetHealth(me->CountPctFromMaxHealth(10));

                    p_Damage = 0;

                    m_CorruptingShadowPhase = true;

                    me->RemoveAllAuras();

                    me->InterruptNonMeleeSpells(true);

                    Talk(eTalks::TalkCorruptingShadow0);

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    if (instance)
                        instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    me->SetReactState(ReactStates::REACT_PASSIVE);

                    me->StopAttack();

                    me->StopMoving();

                    uint32 l_Timer = 5 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Timer, [this]() -> void
                    {
                        me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);
                    });

                    l_Timer += 3 * TimeConstants::IN_MILLISECONDS + 600;
                    AddTimedDelayedOperation(l_Timer, [this]() -> void
                    {
                        me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);

                        if (instance)
                        {
                            if (Creature* l_Dummy = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureOldInvisibleMan)))
                                me->SetFacingTo(me->GetAngle(l_Dummy));
                        }
                    });

                    l_Timer += 1 * TimeConstants::IN_MILLISECONDS + 200;
                    AddTimedDelayedOperation(l_Timer, [this]() -> void
                    {
                        Talk(eTalks::TalkCorruptingShadow1);

                        DoCast(me, eSpells::SpellThrowBook, true);

                        me->DisableEvadeMode();
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY1H);

                        if (instance)
                            instance->DoCombatStopOnPlayers();
                    });

                    l_Timer += 5 * TimeConstants::IN_MILLISECONDS + 200;
                    AddTimedDelayedOperation(l_Timer, [this]() -> void
                    {
                        DoCast(me, eSpells::SpellBlinkEnd, true);

                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);
                    });

                    l_Timer += 1 * TimeConstants::IN_MILLISECONDS + 200;
                    AddTimedDelayedOperation(l_Timer, [this]() -> void
                    {
                        DoCast(me, eSpells::SpellReturnPower);

                        Talk(eTalks::TalkCorruptingShadow2);
                    });

                    l_Timer += 6 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Timer, [this]() -> void
                    {
                        DoCast(me, eSpells::SpellBanishShadow, true);

                        if (Player* l_Player = me->SelectNearestPlayerNotGM(150.0f))
                            l_Player->SummonCreature(eCreatures::CreatureCorruptingShadows, { 811.7639f, 1358.865f, 266.2537f, 4.444691f });
                    });

                    l_Timer += 8 * TimeConstants::IN_MILLISECONDS;
                    AddTimedDelayedOperation(l_Timer, [this]() -> void
                    {
                        Talk(eTalks::TalkCorruptingShadow3);
                    });
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon->GetEntry() == eCreatures::CreatureCorruptingShadows)
                    summons.Summon(p_Summon);
                else
                    BossAI::JustSummoned(p_Summon);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionChallengeEnd && instance)
                {
                    me->DisableEvadeMode();
                    me->SetFullHealth();
                    me->setFaction(FactionTemplates::FACTION_FRIENDLY);
                    me->CombatStop();

                    if (Creature* l_Dummy = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureOldInvisibleMan)))
                    {
                        l_Dummy->CastSpell(l_Dummy, eSpells::SpellPowerReturnedVisual, true);

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (!instance)
                                return;

                            instance->DoCombatStopOnPlayers();

                            Player* l_Player = nullptr;

                            Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                            for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                            {
                                if (Player* l_Plr = l_Iter->getSource())
                                {
                                    l_Player = l_Plr;
                                    break;
                                }
                            }

                            if (!l_Player)
                                return;

                            l_Player->CastSpell(l_Player, eSpells::SpellPowerReturnedScreen, true);
                            l_Player->CastSpell(l_Player, eSpells::SpellPowerReturnedSkybox, true);
                        });

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->InterruptNonMeleeSpells(true);
                            me->RemoveAllAuras();

                            if (instance)
                            {
                                if (Creature* l_Dummy = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureOldInvisibleMan)))
                                {
                                    l_Dummy->RemoveAllAreasTrigger();
                                    l_Dummy->RemoveAllAuras();
                                }
                            }
                        });
                    }

                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);

                        if (!instance)
                            return;

                        Player* l_Player = nullptr;

                        Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                        {
                            if (Player* l_Plr = l_Iter->getSource())
                            {
                                l_Player = l_Plr;
                                break;
                            }
                        }

                        if (!l_Player)
                            return;

                        Talk(eTalks::TalkOutro0, l_Player->GetGUID());
                    });

                    AddTimedDelayedOperation(13 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);

                        Talk(eTalks::TalkOutro1);
                    });

                    AddTimedDelayedOperation(21 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::TalkOutro2);

                        DoCast(me, eSpells::SpellSummonPortalsDummy);

                        Position l_Pos = me->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, 5.0f, me->m_orientation, true);

                        me->SummonGameObject(eGameObjects::GameObjectPortalToAzshara, l_Pos);
                    });
                }
            }

            void OnInterruptCast(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo, SpellInfo const* /*p_CurrSpellInfo*/, uint32 /*p_SchoolMask*/) override
            {
                if (!p_SpellInfo)
                    return;

                int32 l_Duration = p_SpellInfo->GetDuration();
                if (!l_Duration)
                    return;

                SetCombatMovement(true);

                if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveChase(l_Target);
                }

                AddTimedDelayedOperation(l_Duration, [this]() -> void
                {
                    me->StopMoving();

                    me->GetMotionMaster()->Clear();

                    SetCombatMovement(false);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (m_PhaseID == ePhases::PhaseMirrorImage && m_Hidden)
                {
                    if (Player* l_Target = me->FindNearestPlayer(5.0f))
                    {
                        m_Hidden = false;

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        me->RemoveAura(eSpells::SpellCloakOfGlittering);

                        me->InterruptNonMeleeSpells(true);

                        summons.DespawnEntry(eCreatures::CreatureArchmageXylemDummy);

                        m_DarknessTimer = 0;
                    }

                    if (m_DarknessTimer)
                    {
                        if (m_DarknessTimer <= p_Diff)
                        {
                            DoCast(me, eSpells::SpellDarknessAoE, true);

                            m_DarknessTimer = 1 * TimeConstants::IN_MILLISECONDS;
                        }
                        else
                            m_DarknessTimer -= p_Diff;
                    }
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_RazorIce)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFrostbolt:
                    {
                        if (Unit* l_Target = me->getVictim())
                            DoCast(l_Target, eSpells::SpellFrostbolt);

                        events.ScheduleEvent(eEvents::EventFrostbolt, 2 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventRazorIce:
                    {
                        m_RazorIce = true;

                        DoCast(me, eSpells::SpellBlink, true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            if (Unit* l_Target = me->getVictim())
                                DoCast(l_Target, eSpells::SpellRazorIceStun, true);
                        });

                        events.ScheduleEvent(eEvents::EventRazorIce, 27 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventPhaseChange:
                    {
                        events.Reset();

                        DefaultEvents(ePhases::PhaseMirrorImage);
                        break;
                    }
                    case eEvents::EventIntermission:
                    {
                        me->RemoveAllAuras();

                        DoCast(me, eSpells::SpellBlinkIntermission, true);
                        break;
                    }
                    case eEvents::EventArcaneBarrage:
                    {
                        if (Unit* l_Target = me->getVictim())
                            DoCast(l_Target, eSpells::SpellArcaneBarrage);

                        events.ScheduleEvent(eEvents::EventArcaneBarrage, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventShadowBarrage:
                    {
                        if (Unit* l_Target = me->getVictim())
                        {
                            me->SetFacingTo(me->GetAngle(l_Target));

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                                if (Unit* l_Target = me->getVictim())
                                    DoCast(l_Target, eSpells::SpellShadowBarrage);
                            });
                        }

                        events.ScheduleEvent(eEvents::EventShadowBarrage, 40 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventDrawPower:
                    {
                        if (instance)
                        {
                            if (Creature* l_Dummy = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureOldInvisibleMan)))
                            {
                                me->SetFacingTo(me->GetAngle(l_Dummy));

                                AddTimedDelayedOperation(1, [this]() -> void
                                {
                                    DoCast(me, eSpells::SpellDrawPower);
                                });
                            }
                        }

                        events.ScheduleEvent(eEvents::EventDrawPower, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_archmage_xylemAI(p_Creature);
        }
};

/// Corrupting Shadows - 116839
class boss_xylem_corrupting_shadows : public CreatureScript
{
    public:
        boss_xylem_corrupting_shadows() : CreatureScript("boss_xylem_corrupting_shadows") { }

        enum eSpells
        {
            SpellCreepingShadows    = 232664,

            SpellSeedOfDarkness     = 233164,
            SpellSeedOfDarknessAura = 233248,

            SpellStealthDetection   = 141048
        };

        enum eEvents
        {
            EventCreepingShadows = 1,
            EventSeedOfDarkness
        };

        enum eTalk
        {
            TalkCorruptingShadows
        };

        enum eActions
        {
            ActionDemonWithinKilled = 0,
            ActionChallengeEnd      = 0
        };

        struct boss_xylem_corrupting_shadowsAI : public BossAI
        {
            boss_xylem_corrupting_shadowsAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            uint8 m_DarknessWithinCounter = 0;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SILENCE, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_STUN, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SLEEP, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_KNOCKOUT, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, true);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                SetCombatMovement(false);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                Talk(eTalk::TalkCorruptingShadows);

                DoCast(me, eSpells::SpellStealthDetection, true);

                me->DisableEvadeMode();
                me->DisableHealthRegen();

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    if (!instance)
                        return;

                    Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());
                    Player* l_Player = nullptr;

                    Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                    {
                        if (Player* l_Plr = l_Iter->getSource())
                        {
                            l_Player = l_Plr;
                            break;
                        }
                    }

                    if (!l_Scenario || !l_Player)
                        return;

                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataGainingPower1, 0, 0, l_Player, l_Player);
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataGainingPower2, 0, 0, l_Player, l_Player);
                    l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataGainingPower3, 0, 0, l_Player, l_Player);
                });

                AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    SetCombatMovement(true);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    if (Player* l_Player = me->SelectNearestPlayerNotGM(150.0f))
                    {
                        AttackStart(l_Player);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(l_Player);
                    }
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventCreepingShadows, 6 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSeedOfDarkness, 20 * TimeConstants::IN_MILLISECONDS + 600);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAllAreasTrigger();

                if (!instance)
                    return;

                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->instance->GetScenarioGuid());
                Player* l_Player = nullptr;

                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Plr = l_Iter->getSource())
                    {
                        l_Player = l_Plr;
                        break;
                    }
                }

                if (!l_Scenario || !l_Player)
                    return;

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataTheShadowWithin, 0, 0, l_Player, l_Player);

                l_Player->KilledMonsterCredit(eCreatures::CreatureArchmageXylem);

                if (Creature* l_Xylem = Creature::GetCreature(*me, instance->GetData64(eCreatures::CreatureArchmageXylem)))
                {
                    if (l_Xylem->IsAIEnabled)
                        l_Xylem->AI()->DoAction(eActions::ActionChallengeEnd);
                }
            }

            void EnterEvadeMode() override
            {
                /// Only evades if player is dead
                if (instance && !instance->IsWipe())
                    return;

                me->DespawnOrUnsummon();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSeedOfDarkness:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Target, eSpells::SpellSeedOfDarknessAura, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellSeedOfDarkness)
                {
                    SetCombatMovement(true);

                    if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(l_Target);
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActions::ActionDemonWithinKilled)
                {
                    ++m_DarknessWithinCounter;

                    if (m_DarknessWithinCounter >= 3)
                        me->InterruptNonMeleeSpells(true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCreepingShadows:
                    {
                        DoCast(me, eSpells::SpellCreepingShadows);

                        events.ScheduleEvent(eEvents::EventCreepingShadows, 6 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSeedOfDarkness:
                    {
                        m_DarknessWithinCounter = 0;

                        me->StopMoving();
                        me->GetMotionMaster()->Clear();

                        SetCombatMovement(false);

                        DoCast(me, eSpells::SpellSeedOfDarkness);

                        events.ScheduleEvent(eEvents::EventSeedOfDarkness, 66 * TimeConstants::IN_MILLISECONDS);
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
            return new boss_xylem_corrupting_shadowsAI(p_Creature);
        }
};

/// Invisible Man - 116492
class npc_xylem_invisible_man : public CreatureScript
{
    public:
        npc_xylem_invisible_man() : CreatureScript("npc_xylem_invisible_man") { }

        enum eSpells
        {
            SpellBlinkDummy             = 242016,
            SpellBlinkTriggered         = 231611,

            SpellBlinkIntermissionDummy = 232649,
            SpellBlinkIntermissionTrigg = 232038
        };

        struct npc_xylem_invisible_manAI : public Scripted_NoMovementAI
        {
            npc_xylem_invisible_manAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBlinkDummy:
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Xylem = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureArchmageXylem)))
                                l_Xylem->CastSpell(me, eSpells::SpellBlinkTriggered, true);
                        }

                        break;
                    }
                    case eSpells::SpellBlinkIntermissionDummy:
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Xylem = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureArchmageXylem)))
                                l_Xylem->CastSpell(me, eSpells::SpellBlinkIntermissionTrigg, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xylem_invisible_manAI(p_Creature);
        }
};

/// Razor Ice - 116806
class npc_xylem_razor_ice : public CreatureScript
{
    public:
        npc_xylem_razor_ice() : CreatureScript("npc_xylem_razor_ice") { }

        enum eSpells
        {
            SpellRazorIceAT = 232660
        };

        struct npc_xylem_razor_iceAI : public Scripted_NoMovementAI
        {
            npc_xylem_razor_iceAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellRazorIceAT, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xylem_razor_iceAI(p_Creature);
        }
};

/// Archmage Xylem - 116295
class npc_xylem_archmage_dummy : public CreatureScript
{
    public:
        npc_xylem_archmage_dummy() : CreatureScript("npc_xylem_archmage_dummy") { }

        enum eSpells
        {
            SpellImage              = 231619,
            SpellWhirlOfIllusion    = 231622,
            SpellDummyNuke          = 242018,
            SpellPrismaticShock     = 231914,
            SpellArcaneBarrier      = 231982
        };

        enum eEvent
        {
            EventPrismaticShock = 1
        };

        struct npc_xylem_archmage_dummyAI : public Scripted_NoMovementAI
        {
            npc_xylem_archmage_dummyAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool m_Barrier = false;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellImage, true);
                DoCast(me, eSpells::SpellWhirlOfIllusion, true);

                if (Player* l_Target = me->FindNearestPlayer(100.0f))
                    DoCast(l_Target, eSpells::SpellDummyNuke);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDummyNuke:
                    {
                        events.ScheduleEvent(eEvent::EventPrismaticShock, 1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_Barrier && me->GetHealth() <= p_Damage)
                {
                    m_Barrier = true;

                    DoCast(me, eSpells::SpellArcaneBarrier, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventPrismaticShock:
                    {
                        DoCast(me, eSpells::SpellPrismaticShock);

                        events.ScheduleEvent(eEvent::EventPrismaticShock, 3 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xylem_archmage_dummyAI(p_Creature);
        }
};

/// Darkness Within - 116840
class npc_xylem_darkness_within : public CreatureScript
{
    public:
        npc_xylem_darkness_within() : CreatureScript("npc_xylem_darkness_within") { }

        enum eSpells
        {
            SpellMerge      = 232991,
            SpellDetection  = 141048
        };

        enum eAction
        {
            ActionDemonWithinKilled
        };

        struct npc_xylem_darkness_withinAI : public ScriptedAI
        {
            npc_xylem_darkness_withinAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Merged = false;

            bool m_MovementStopped = true;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 1);

                    if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureCorruptingShadows)))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, l_Boss->GetPosition());
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureCorruptingShadows)))
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->AI()->DoAction(eAction::ActionDemonWithinKilled);
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1 && !m_Merged)
                {
                    if (Unit* l_Owner = me->GetAnyOwner())
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureCorruptingShadows)))
                            {
                                if (me->GetDistance(*l_Boss) > 2.0f)
                                    m_MovementStopped = true;
                                else
                                {
                                    me->SetFacingToObject(l_Boss);

                                    Merge();
                                }
                            }
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Merged)
                    return;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureCorruptingShadows)))
                    {
                        if (me->GetDistance(*l_Boss) <= 2.0f)
                            Merge();
                        else
                        {
                            if (m_MovementStopped && !me->HasBreakableByDamageCrowdControlAura())
                            {
                                m_MovementStopped = false;

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MovePoint(1, l_Boss->GetPosition());
                            }
                        }
                    }
                }
            }

            void Merge()
            {
                me->GetMotionMaster()->Clear();

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                m_Merged = true;

                DoCast(me, eSpells::SpellMerge, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureCorruptingShadows)))
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->AI()->DoAction(eAction::ActionDemonWithinKilled);
                    }
                }

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellDetection, true);

                    me->DespawnOrUnsummon(1);
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_xylem_darkness_withinAI(p_Creature);
        }
};

/// Time Acceleration - 231980
class spell_xylem_time_acceleration : public SpellScriptLoader
{
    public:
        spell_xylem_time_acceleration() : SpellScriptLoader("spell_xylem_time_acceleration") { }

        class spell_xylem_time_acceleration_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xylem_time_acceleration_AuraScript);

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                p_AbsorbAmount = p_DmgInfo.GetAmount();
            }

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1; ///< Initialize to infinite Absorb
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_xylem_time_acceleration_AuraScript::CalculateAmount, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_xylem_time_acceleration_AuraScript::OnAbsorb, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xylem_time_acceleration_AuraScript();
        }
};

/// Draw Power - 231522
class spell_xylem_draw_power : public SpellScriptLoader
{
    public:
        spell_xylem_draw_power() : SpellScriptLoader("spell_xylem_draw_power") { }

        enum eSpell
        {
            SpellDrawPowerAura = 231610
        };

        class spell_xylem_draw_power_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_xylem_draw_power_Aurascript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSpell::SpellDrawPowerAura, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xylem_draw_power_Aurascript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xylem_draw_power_Aurascript();
        }
};

/// Seed of Darkness - 233248
class spell_xylem_seed_of_darkness : public SpellScriptLoader
{
    public:
        spell_xylem_seed_of_darkness() : SpellScriptLoader("spell_xylem_seed_of_darkness") { }

        enum eSpell
        {
            SpellPortalVisual = 233165
        };

        class spell_xylem_seed_of_darkness_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_xylem_seed_of_darkness_Aurascript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    for (uint8 l_I = 0; l_I < 3; ++l_I)
                        l_Target->CastSpell(l_Target, eSpell::SpellPortalVisual, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xylem_seed_of_darkness_Aurascript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xylem_seed_of_darkness_Aurascript();
        }
};

/// Shadow Barrage - 231443
class spell_xylem_shadow_barrage : public SpellScriptLoader
{
    public:
        spell_xylem_shadow_barrage() : SpellScriptLoader("spell_xylem_shadow_barrage") { }

        class spell_xylem_shadow_barrage_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_xylem_shadow_barrage_Aurascript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (roll_chance_f(60.0f))
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xylem_shadow_barrage_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xylem_shadow_barrage_Aurascript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xylem_shadow_barrage_Aurascript::OnTick, SpellEffIndex::EFFECT_2, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xylem_shadow_barrage_Aurascript();
        }
};

/// Break Focus - 231563
class areatrigger_xylem_break_focus : public AreaTriggerEntityScript
{
    public:
        areatrigger_xylem_break_focus() : AreaTriggerEntityScript("areatrigger_xylem_break_focus") { }

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer() || !p_AreaTrigger->GetCaster())
                return false;

            p_Target->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), true, p_AreaTrigger->GetCaster()->GetPosition(), -10.0f, 1);
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer() || !p_AreaTrigger->GetCaster())
                return false;

            p_Target->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), false, Position());
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_xylem_break_focus();
        }
};

/// Creeping Shadows - 232664
class areatrigger_xylem_creeping_shadows : public AreaTriggerEntityScript
{
    public:
        areatrigger_xylem_creeping_shadows() : AreaTriggerEntityScript("areatrigger_xylem_creeping_shadows") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 40 * TimeConstants::IN_MILLISECONDS);

            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.016043f);
            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 45.0f);
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            float l_MaxRadius  = p_AreaTrigger->GetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4);
            float l_PctPerMSec = ((l_MaxRadius - 1.0f) * 100.0f) / float(p_AreaTrigger->GetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE));
            float l_BaseRadius = p_AreaTrigger->GetAreaTriggerInfo().Radius;
            float l_CurrRadius = p_AreaTrigger->GetRadius();

            l_CurrRadius += CalculatePct(l_BaseRadius, l_PctPerMSec * p_Time);

            p_AreaTrigger->SetRadius(std::min(l_CurrRadius, p_AreaTrigger->GetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4)));
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_xylem_creeping_shadows();
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_the_archmages_reckoning()
{
    new boss_archmage_xylem();
    new boss_xylem_corrupting_shadows();

    new npc_xylem_invisible_man();
    new npc_xylem_razor_ice();
    new npc_xylem_archmage_dummy();
    new npc_xylem_darkness_within();

    new spell_xylem_time_acceleration();
    new spell_xylem_draw_power();
    new spell_xylem_seed_of_darkness();
    new spell_xylem_shadow_barrage();

    new areatrigger_xylem_break_focus();
    new areatrigger_xylem_creeping_shadows();
}
#endif
