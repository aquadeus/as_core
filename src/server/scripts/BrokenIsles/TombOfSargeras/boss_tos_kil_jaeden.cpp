////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "tomb_of_sargeras.h"

Position const g_CenterPos = { 4500.056f, -1510.1171f, 5385.645f, 4.689f };

/// Kil'jaeden <The Deceiver> - 117269
class boss_tos_kil_jaeden : public CreatureScript
{
    public:
        boss_tos_kil_jaeden() : CreatureScript("boss_tos_kil_jaeden") { }

        enum eSpells
        {
            /// Misc
            SpellKilJaedenFarClipAura       = 248197,
            SpellNetherGale                 = 244834,
            SpellBreakPlayerTargetting      = 140562,
            SpellConfrontingTheDeceiver     = 247194,
            /// Felclaws
            SpellFelclaws                   = 239932,
            /// Armageddon
            SpellArmageddonCast             = 240910,
            SpellArmageddonBlast            = 240908,
            SpellArmageddonRainAoE          = 234310,
            SpellArmageddonHailAoE          = 240916,
            /// Rupturing Singularity
            SpellRupturingSingularity       = 235059,
            /// Bursting Dreadflame
            SpellBurstingDreadflameSearcher = 238429,
            SpellBurstingDreadflameMissile  = 238430,
            /// Focused Dreadflame
            SpellFocusedDreadflameAura      = 238505,
            SpellFocusedDreadflameCast      = 238502,
            SpellFocusedDreadburstAoE       = 238503,
            /// Shadow Reflection: Erupting
            SpellShadowReflectionErupting   = 236710,
            SpellEruptingOrbAT              = 243542,
            /// Deceiver's Veil
            SpellDeceiversVeilCast          = 241983,
            SpellDeceiversVeil8YardsAura    = 236555,
            SpellDeceiversVeil40YardsAura   = 242696,
            SpellIllidanSightlessGaze       = 241721,
            /// Darkness of a Thousand Souls
            SpellDarknessThousandSoulsCast  = 238999,
            SpellDarknessMissiles           = 239112,
            SpellDarknessMissilesRift       = 239129,
            SpellDarknessDoT                = 239216,
            /// Tear Rift
            SpellTearRiftCast               = 243982,
            SpellTearRiftDummy              = 239214,
            SpellGravitySqueeze             = 239155,
            /// Shadow Reflection: Wailing
            SpellShadowReflectionWailing    = 236378,
            /// Flaming Orb
            SpellFlamingOrbMissile          = 239253,
            /// Shadow Reflection: Hopeless
            SpellShadowReflectionHopeless   = 237590
        };

        enum eEvents
        {
            EventArmageddon = 1,
            EventRupturingSingularity,
            EventBurstingDreadflame,
            EventFocusedDreadflame,
            EventFelclaws,
            EventIntermissionMove,
            EventIntermissionEnd,
            EventShadowReflectionErupting,
            EventDarknessOfAThousandSouls,
            EventTearRift,
            EventFlamingOrb,
            EventDemonicObelisk,
            EventShadowReflectionWailing,
            EventShadowReflectionHopeless,

            EventMax
        };

        enum eTalks
        {
            TalkAggro,
            TalkArmageddon,
            TalkArmageddonWarn,
            TalkArmageddonBlast,
            TalkRupturingSingularity,
            TalkRupturingSingularityWarn,
            TalkIntermissionEternalFlame,
            TalkBurstingDreadflame,
            TalkBurstingDreadflameWarn,
            TalkFocusedDreadflame,
            TalkFocusedDreadflameWarn,
            TalkSlayPlayer,
            TalkWipe,
            TalkShadowReflection,
            TalkIntermissionDeceiversVeil,
            TalkDeceiversVeil,
            TalkDeceiversVeilEnd,
            TalkDarknessOfAThousandSouls,
            TalkTearRift,
            TalkMythicFelclaws
        };

        enum eActions
        {
            ActionScheduleIntro,
            ActionArmageddonRain,
            ActionArmageddonHail,
            ActionDeceiversVeilEnd,
            ActionShadowSoulCast
        };

        enum eMoves
        {
            MoveIntro = 1,
            MoveFlyUp,
            MoveFly,
            MoveToCenter,
            MoveFlyDown,
            MoveDarknessCenter
        };

        enum ePhases
        {
            Phase1TheBetrayer,
            PhaseInterEternalFlame,
            Phase2ReflectedSouls,
            PhaseInterDeceiversVeil,
            Phase3DarknessOfAThousandSouls
        };

        enum eKJData
        {
            MaxArmageddonRain   = 6,
            MaxSingularityPos   = 5,
            MaxShadowsouls      = 5,
            MaxDemonicObeliskNm = 2,
            MaxDemonicObeliskHm = 3,

            AnimFightVisual     = 13530,
            InvisibleVisual     = 11686
        };

        struct boss_tos_kil_jaedenAI : public BossAI
        {
            boss_tos_kil_jaedenAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataKiljaeden) { }

            std::vector<uint32> m_WailingMythicTimers = { 49400, 60000, 169100, 60000 };

            ///      DifficultyID ->      PhaseID ->       Event -> CastCount -> Timer
            std::map<Difficulty, std::map<uint32, std::map<uint32, std::vector<uint32>>>> m_TimerData =
            {
                {
                    Difficulty::DifficultyRaidLFR,
                    {
                        {
                            ePhases::Phase1TheBetrayer,
                            {
                                { eEvents::EventArmageddon,             { 10000, 22000, 42000, 22000, 30000 } }
                            }
                        },
                        {
                            ePhases::PhaseInterEternalFlame,
                            {
                                { eEvents::EventArmageddon,             { 6100, 29400 } },
                                { eEvents::EventBurstingDreadflame,     { 7700, 17000, 13400, 17000 } }
                            }
                        },
                        {
                            ePhases::Phase2ReflectedSouls,
                            {
                                { eEvents::EventArmageddon,             { 56000, 27700, 56700, 26700, 12200, 18900, 18900 } },
                                { eEvents::EventBurstingDreadflame,     { 58200, 53300, 61100, 56700 } }
                            }
                        },
                        {
                            ePhases::Phase3DarknessOfAThousandSouls,
                            {
                                { eEvents::EventBurstingDreadflame,     { 42000, 25000, 70000, 25000, 70000, 25000, 70000, 25000, 70000 } }
                            }
                        }
                    }
                },
                {
                    Difficulty::DifficultyRaidNormal,
                    {
                        {
                            ePhases::Phase1TheBetrayer,
                            {
                                { eEvents::EventArmageddon,             { 10000, 64000 } },
                                { eEvents::EventRupturingSingularity,   { 58000, 56000 } }
                            }
                        },
                        {
                            ePhases::PhaseInterEternalFlame,
                            {
                                { eEvents::EventArmageddon,             { 6100, 29400 } },
                                { eEvents::EventBurstingDreadflame,     { 7700, 46000 } },
                                { eEvents::EventFocusedDreadflame,      { 24600, 13400 } }
                            }
                        },
                        {
                            ePhases::Phase2ReflectedSouls,
                            {
                                { eEvents::EventArmageddon,             { 50400, 45000, 31000, 35000, 31000, 78000 } },
                                { eEvents::EventRupturingSingularity,   { 73500, 81000 } },
                                { eEvents::EventBurstingDreadflame,     { 52400, 48000, 55000, 50000 } },
                                { eEvents::EventFocusedDreadflame,      { 76400, 99000 } }
                            }
                        },
                        {
                            ePhases::Phase3DarknessOfAThousandSouls,
                            {
                                { eEvents::EventBurstingDreadflame,     { 42000, 25000, 70000, 25000, 70000, 25000, 70000 } },
                                { eEvents::EventFocusedDreadflame,      { 80000, 95000, 95000, 95000, 95000, 95000, 95000 } }
                            }
                        }
                    }
                },
                {
                    Difficulty::DifficultyRaidHeroic,
                    {
                        {
                            ePhases::Phase1TheBetrayer,
                            {
                                { eEvents::EventArmageddon,             { 10000, 64000 } },
                                { eEvents::EventRupturingSingularity,   { 58000, 56000 } }
                            }
                        },
                        {
                            ePhases::PhaseInterEternalFlame,
                            {
                                { eEvents::EventArmageddon,             { 6100, 29400 } },
                                { eEvents::EventBurstingDreadflame,     { 7700, 46000 } },
                                { eEvents::EventFocusedDreadflame,      { 24600, 13400 } }
                            }
                        },
                        {
                            ePhases::Phase2ReflectedSouls,
                            {
                                { eEvents::EventArmageddon,             { 50400, 76000, 35000, 31000 } },
                                { eEvents::EventRupturingSingularity,   { 73500, 26000, 55000, 44000 } },
                                { eEvents::EventBurstingDreadflame,     { 52400, 48000, 55000, 50000 } },
                                { eEvents::EventFocusedDreadflame,      { 30400, 46000, 53000, 46000 } }
                            }
                        },
                        {
                            ePhases::Phase3DarknessOfAThousandSouls,
                            {
                                { eEvents::EventBurstingDreadflame,     { 42000, 25000, 70000, 25000, 70000, 25000, 70000 } },
                                { eEvents::EventFocusedDreadflame,      { 80000, 95000, 95000, 95000, 95000, 95000, 95000 } }
                            }
                        }
                    }
                },
                {
                    Difficulty::DifficultyRaidMythic,
                    {
                        {
                            ePhases::Phase1TheBetrayer,
                            {
                                { eEvents::EventArmageddon,             { 11000, 54000, 38000 } },
                                { eEvents::EventRupturingSingularity,   { 55000, 25000, 25000, 28000 } }
                            }
                        },
                        {
                            ePhases::PhaseInterEternalFlame,
                            {
                                { eEvents::EventArmageddon,             { 6500, 58900 } },
                                { eEvents::EventBurstingDreadflame,     { 10100, 79000 } },
                                { eEvents::EventFocusedDreadflame,      { 28700, 38900 } }
                            }
                        },
                        {
                            ePhases::Phase2ReflectedSouls,
                            {
                                { eEvents::EventArmageddon,             { 18400, 32000, 45000, 33000, 36000, 36000, 47000, 32000, 45000 } },
                                { eEvents::EventRupturingSingularity,   { 21500, 50000, 67000, 78000, 84000 } },
                                { eEvents::EventBurstingDreadflame,     { 52400, 50000, 45000, 48000, 86000, 50000 } },
                                { eEvents::EventFocusedDreadflame,      { 30400, 44000, 47000, 138000, 44000 } }
                            }
                        },
                        {
                            ePhases::Phase3DarknessOfAThousandSouls,
                            {
                                { eEvents::EventBurstingDreadflame,     { 30000, 52000, 43000, 52000, 43000, 52000, 43000 } },
                                { eEvents::EventFocusedDreadflame,      { 80000, 36000, 59000, 36000, 59000, 36000, 59000 } }
                            }
                        }
                    }
                }
            };

            bool m_IntroDone = false;
            bool m_DarknessMoves = false;
            bool m_Achievement = true;

            ePhases m_Phase;

            uint32 m_ArmageddonCount = 0;
            uint32 m_SingularityCount = 0;
            uint32 m_BurstingCount = 0;
            uint32 m_FocusedCount = 0;
            uint32 m_DarknessCount = 0;
            uint32 m_FlamingOrbCount = 0;
            uint32 m_ObeliskCount = 0;
            uint32 m_WailingCount = 0;

            std::array<Position const, eKJData::MaxSingularityPos> m_SingularityPos =
            {
                {
                    { 4500.056f, -1510.1171f, 5385.645f, 4.689f },  ///< Center
                    { 4535.894f, -1474.9326f, 5385.645f, 3.925f },  ///< N/W
                    { 4463.952f, -1474.8186f, 5385.645f, 5.513f },  ///< S/W
                    { 4464.281f, -1546.1646f, 5385.645f, 0.781f },  ///< S/E
                    { 4536.258f, -1545.6653f, 5385.645f, 2.367f }   ///< N/E
                }
            };

            uint32 m_BerserkTimer = 0;

            uint32 m_ShadowsoulKilled = 0;

            uint64 m_RiftGuid = 0;

            uint64 m_IllidanStormrage = 0;

            bool m_BiggestSingularity = false;

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return me->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE) || m_Phase == ePhases::PhaseInterEternalFlame;
            }

            void Reset() override
            {
                DoCast(me, eSpells::SpellKilJaedenFarClipAura, true);

                SetFlyMode(!m_IntroDone);

                me->SetPlayerHoverAnim(!m_IntroDone);
                me->SendPlayHoverAnim(!m_IntroDone);

                me->SetReactState(m_IntroDone ? ReactStates::REACT_AGGRESSIVE : ReactStates::REACT_PASSIVE);

                if (m_IntroDone)
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                else
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->NearTeleportTo(me->GetHomePosition());
                });

                m_DarknessMoves = false;
                m_Achievement = true;

                m_ArmageddonCount = 0;
                m_SingularityCount = 0;
                m_BurstingCount = 0;
                m_FocusedCount = 0;
                m_DarknessCount = 0;
                m_FlamingOrbCount = 0;
                m_ObeliskCount = 0;
                m_WailingCount = 0;

                m_BerserkTimer = 0;

                m_ShadowsoulKilled = 0;

                m_RiftGuid = 0;

                m_BiggestSingularity = false;

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!instance)
                        return;

                    if (Creature* l_Ixallon = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcIxallonTheSoulbreaker)))
                    {
                        if (l_Ixallon->isDead())
                            DoAction(0);
                    }
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                Talk(eTalks::TalkAggro);

                m_BerserkTimer = (IsMythic() ? 840 : 600) * TimeConstants::IN_MILLISECONDS;

                DefaultEvents(ePhases::Phase1TheBetrayer);

                if (instance)
                {
                    if (Creature* l_Velen = Creature::GetCreature(*me, instance->GetData64(eCreatures::BossProphetVelen)))
                    {
                        l_Velen->SetFacingTo(1.710423f);
                        l_Velen->SetAIAnimKitId(eKJData::AnimFightVisual);
                    }

                    if (Creature* l_Illidan = instance->instance->SummonCreature(eCreatures::BossIllidanStormrage, { 4455.815f, -1425.495f, 5386.957f, 1.53589f }, nullptr, 0, nullptr, 0, 0, 0, nullptr, true))
                    {
                        m_IllidanStormrage = l_Illidan->GetGUID();

                        l_Illidan->SetBig(true);
                        l_Illidan->SetAIAnimKitId(eKJData::AnimFightVisual);
                    }

                    /// Close bridges
                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (GameObject* l_Bridge = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoBridgeSouth)))
                            l_Bridge->SetGoState(GOState::GO_STATE_ACTIVE);

                        if (GameObject* l_Bridge = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoBridgeNorth)))
                            l_Bridge->SetGoState(GOState::GO_STATE_ACTIVE);
                    });
                }
            }

            void EnterEvadeMode() override
            {
                me->NearTeleportTo(me->GetHomePosition());

                BossAI::EnterEvadeMode();

                me->RemoveAura(eSharedSpells::SpellBerserk);

                Talk(eTalks::TalkWipe);

                me->InterruptNonMeleeSpells(true);

                me->DespawnAreaTriggersInArea(eSpells::SpellEruptingOrbAT);
                me->DespawnCreaturesInArea({ eCreatures::NpcEruptingReflection, eCreatures::NpcWailingReflection });

                if (instance)
                {
                    if (Creature* l_Velen = Creature::GetCreature(*me, instance->GetData64(eCreatures::BossProphetVelen)))
                    {
                        l_Velen->SetFacingTo(4.310963f);
                        l_Velen->SetAIAnimKitId(0);
                    }

                    if (m_IllidanStormrage)
                    {
                        if (Creature* l_Illidan = instance->instance->GetCreature(m_IllidanStormrage))
                        {
                            l_Illidan->DespawnOrUnsummon();

                            m_IllidanStormrage = 0;
                        }
                    }

                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil8YardsAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil40YardsAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellIllidanSightlessGaze);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDarknessDoT);

                    if (GameObject* l_Skybox = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoKJSkybox)))
                        l_Skybox->SetGoState(GOState::GO_STATE_ACTIVE);

                    /// Open bridges
                    {
                        if (GameObject* l_Bridge = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoBridgeSouth)))
                            l_Bridge->SetGoState(GOState::GO_STATE_READY);

                        if (GameObject* l_Bridge = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoBridgeNorth)))
                            l_Bridge->SetGoState(GOState::GO_STATE_READY);
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->ClearLootContainers();

                _JustDied();

                me->DespawnCreaturesInArea(eCreatures::NpcEruptingReflection);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil8YardsAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil40YardsAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellIllidanSightlessGaze);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDarknessDoT);

                    instance->DoCastSpellOnPlayers(eSpells::SpellConfrontingTheDeceiver);

                    instance->DoNearTeleportPlayers({ -245.101f, 5942.25f, 165.91f, 5.521349f });

                    if (m_Achievement && !IsLFR())
                        instance->DoCompleteAchievement(eAchievements::DarkSouls);

                    /// Open bridges
                    {
                        if (GameObject* l_Bridge = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoBridgeSouth)))
                            l_Bridge->SetGoState(GOState::GO_STATE_READY);

                        if (GameObject* l_Bridge = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoBridgeNorth)))
                            l_Bridge->SetGoState(GOState::GO_STATE_READY);
                    }
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (!p_Killed->IsPlayer())
                    return;

                Talk(eTalks::TalkSlayPlayer);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionScheduleIntro:
                    {
                        if (m_IntroDone || !instance)
                            break;

                        m_IntroDone = true;

                        if (Creature* l_Velen = instance->instance->SummonCreature(eCreatures::BossProphetVelen, { 4540.9f, -1432.49f, 5386.42f, 4.310963f }, nullptr, 0, nullptr, 0, 0, 0, nullptr, true))
                        {
                            l_Velen->SetBig(true);
                            l_Velen->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);

                            me->CastSpell(l_Velen, eSharedSpells::SpellStartKJIntroConv, true);
                        }

                        me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(eMoves::MoveIntro, { 4500.068f, -1542.896f, 5385.832f, me->m_orientation }, false);
                        break;
                    }
                    case eActions::ActionShadowSoulCast:
                    {
                        m_Achievement = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE && p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoves::MoveIntro:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            SetFlyMode(false);

                            me->SetPlayerHoverAnim(false);
                            me->SendPlayHoverAnim(false);

                            me->SetHomePosition({ 4500.068f, -1542.896f, 5385.832f, me->m_orientation });

                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            me->NearTeleportTo({ 4500.068f, -1542.896f, 5385.832f, me->m_orientation });

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        });

                        break;
                    }
                    case eMoves::MoveFlyUp:
                    {
                        events.ScheduleEvent(eEvents::EventIntermissionMove, 1);
                        break;
                    }
                    case eMoves::MoveFly:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->SetFacingTo(me->GetAngle(&g_CenterPos));
                        });

                        break;
                    }
                    case eMoves::MoveToCenter:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            Position l_Pos = me->GetPosition();

                            l_Pos.m_positionZ -= 10.0f;

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveTakeoff(eMoves::MoveFlyDown, l_Pos);
                        });

                        break;
                    }
                    case eMoves::MoveFlyDown:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            SetFlyMode(false);

                            me->SetPlayerHoverAnim(false);
                            me->SendPlayHoverAnim(false);

                            DefaultEvents(ePhases::Phase2ReflectedSouls);
                        });

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            if (Player* l_Player = SelectMainTank())
                            {
                                AttackStart(l_Player);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Player);
                            }
                        });

                        break;
                    }
                    case eMoves::MoveDarknessCenter:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellDarknessThousandSoulsCast);
                        });

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
                    case eSpells::SpellArmageddonCast:
                    {
                        if (!instance)
                            break;

                        Position l_Pos  = g_CenterPos;

                        float l_Angle   = frand(0.0f, 2.0f * M_PI);
                        float l_Step    = 2.0f * M_PI / float(eKJData::MaxArmageddonRain);

                        uint8 l_RainCount = 0;
                        uint8 l_HailCount = 0;

                        switch (GetDifficulty())
                        {
                            case Difficulty::DifficultyRaidLFR:
                            {
                                l_RainCount = 1 + (instance->instance->GetPlayersCountExceptGMs() / 5);
                                break;
                            }
                            case Difficulty::DifficultyRaidNormal:
                            {
                                l_RainCount = 2 + (instance->instance->GetPlayersCountExceptGMs() / 5);
                                l_HailCount = 1;
                                break;
                            }
                            case Difficulty::DifficultyRaidHeroic:
                            {
                                l_RainCount = 3 + (instance->instance->GetPlayersCountExceptGMs() / 5);
                                l_HailCount = 1;
                                break;
                            }
                            case Difficulty::DifficultyRaidMythic:
                            {
                                l_RainCount = 6;
                                l_HailCount = 2;
                                break;
                            }
                            default:
                                break;
                        }

                        /// Armageddon Rain
                        if (l_RainCount)
                        {
                            for (uint8 l_I = 0; l_I < l_RainCount; ++l_I)
                            {
                                g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 45.0f), l_Angle, true);

                                if (Creature* l_Stalker = me->SummonCreature(eCreatures::NpcArmageddonStalker, l_Pos))
                                {
                                    if (l_Stalker->IsAIEnabled)
                                        l_Stalker->AI()->DoAction(eActions::ActionArmageddonRain);
                                }

                                l_Angle += l_Step;
                            }
                        }

                        /// Armageddon Hail
                        if (l_HailCount)
                        {
                            for (uint8 l_I = 0; l_I < l_HailCount; ++l_I)
                            {
                                g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 45.0f), frand(0.0f, 2.0f * M_PI), true);

                                if (Creature* l_Stalker = me->SummonCreature(eCreatures::NpcArmageddonStalker, l_Pos))
                                {
                                    if (l_Stalker->IsAIEnabled)
                                        l_Stalker->AI()->DoAction(eActions::ActionArmageddonHail);
                                }
                            }
                        }

                        break;
                    }
                    case eSpells::SpellArmageddonBlast:
                    {
                        Talk(eTalks::TalkArmageddonBlast);
                        break;
                    }
                    case eSpells::SpellDeceiversVeilCast:
                    {
                        Talk(eTalks::TalkDeceiversVeil);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellBreakPlayerTargetting, true);
                        });

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            me->SetDisplayId(eKJData::InvisibleVisual);

                            me->NearTeleportTo(g_CenterPos);
                        });

                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 31.5f, frand(0.0f, 2.0f * M_PI), true);

                        l_Pos.SetOrientation(l_Pos.GetAngle(&g_CenterPos));

                        me->SummonCreature(eCreatures::NpcIllidanStormrage, l_Pos);
                        break;
                    }
                    case eSpells::SpellDarknessThousandSoulsCast:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            m_DarknessMoves = false;

                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            if (Player* l_Tank = SelectMainTank())
                            {
                                AttackStart(l_Tank);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Tank);
                            }
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
                    case eSpells::SpellBurstingDreadflameSearcher:
                    {
                        sCreatureTextMgr->SendChat(me, eTalks::TalkBurstingDreadflameWarn, p_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                        DoCast(p_Target, eSpells::SpellBurstingDreadflameMissile, true);
                        break;
                    }
                    case eSpells::SpellFocusedDreadflameAura:
                    {
                        Talk(eTalks::TalkFocusedDreadflame);
                        Talk(eTalks::TalkFocusedDreadflameWarn, p_Target->GetGUID());

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        DoCast(p_Target, eSpells::SpellFocusedDreadflameCast);
                        break;
                    }
                    case eSpells::SpellFocusedDreadflameCast:
                    {
                        events.DelayEvent(eEvents::EventIntermissionMove, 100);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        });

                        if (IsHeroicOrMythic())
                            DoCast(p_Target, eSpells::SpellFocusedDreadburstAoE, true);

                        break;
                    }
                    case eSpells::SpellDarknessThousandSoulsCast:
                    {
                        if (p_Target->HasAura(eSpells::SpellGravitySqueeze))
                        {
                            if (Creature* l_Rift = Creature::GetCreature(*me, m_RiftGuid))
                                DoCast(l_Rift, eSpells::SpellDarknessMissilesRift, true);
                        }
                        else
                            DoCast(p_Target, eSpells::SpellDarknessMissiles, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcShadowsoul:
                    {
                        ++m_ShadowsoulKilled;

                        p_Summon->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);

                        /// End intermission phase
                        if (m_ShadowsoulKilled >= eKJData::MaxShadowsouls && instance)
                        {
                            me->RemoveAura(eSpells::SpellDeceiversVeilCast);

                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil8YardsAura);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil40YardsAura);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellIllidanSightlessGaze);

                            Talk(eTalks::TalkDeceiversVeilEnd);

                            me->RestoreDisplayId();
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                            DefaultEvents(ePhases::Phase3DarknessOfAThousandSouls);

                            if (Creature* l_Illidan = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcIllidanStormrage)))
                            {
                                if (l_Illidan->IsAIEnabled)
                                    l_Illidan->AI()->DoAction(0);
                            }

                            uint32 l_Time = 10 * TimeConstants::IN_MILLISECONDS + 800;
                            AddTimedDelayedOperation(l_Time, [this]() -> void
                            {
                                if (GameObject* l_Skybox = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoKJSkybox)))
                                    l_Skybox->SetGoState(GOState::GO_STATE_READY);
                            });

                            l_Time += 3 * TimeConstants::IN_MILLISECONDS;
                            AddTimedDelayedOperation(l_Time, [this]() -> void
                            {
                                if (Creature* l_Velen = Creature::GetCreature(*me, instance->GetData64(eCreatures::BossProphetVelen)))
                                {
                                    if (l_Velen->IsAIEnabled)
                                        l_Velen->AI()->Talk(0);
                                }
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Phase)
                {
                    case ePhases::Phase1TheBetrayer:
                    {
                        if (me->HealthBelowPctDamaged(80, p_Damage))
                        {
                            Talk(eTalks::TalkIntermissionEternalFlame);

                            DefaultEvents(ePhases::PhaseInterEternalFlame);

                            me->StopAttack();

                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            SetFlyMode(true);

                            me->SetPlayerHoverAnim(true);
                            me->SendPlayHoverAnim(true);

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                Position l_Pos = me->GetPosition();

                                l_Pos.m_positionZ += 10.0f;

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveTakeoff(eMoves::MoveFlyUp, l_Pos);
                            });
                        }

                        break;
                    }
                    case ePhases::Phase2ReflectedSouls:
                    {
                        if (me->HealthBelowPctDamaged(40, p_Damage))
                        {
                            DefaultEvents(ePhases::PhaseInterDeceiversVeil);

                            me->StopAttack();

                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            Talk(eTalks::TalkIntermissionDeceiversVeil);

                            me->InterruptNonMeleeSpells(true);

                            me->StopMoving();
                            me->GetMotionMaster()->Clear();

                            DoCast(me, eSpells::SpellDeceiversVeilCast);

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Position l_Pos = g_CenterPos;

                                for (uint8 l_I = 0; l_I < eKJData::MaxShadowsouls; ++l_I)
                                {
                                    g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(7.5f, 30.0f), frand(0.0f, 2.0f * M_PI), true);

                                    me->SummonCreature(eCreatures::NpcShadowsoul, l_Pos);
                                }
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellShadowReflectionErupting:
                    {
                        if (p_Targets.empty())
                            break;

                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer())
                                return true;

                            if (p_Object->ToPlayer()->GetRoleForGroup() != Roles::ROLE_DAMAGE)
                                return true;

                            return false;
                        });

                        break;
                    }
                    case eSpells::SpellShadowReflectionHopeless:
                    {
                        if (p_Targets.empty())
                            break;

                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer())
                                return true;

                            if (p_Object->ToPlayer()->GetRoleForGroup() != Roles::ROLE_HEALER)
                                return true;

                            return false;
                        });

                        break;
                    }
                    case eSpells::SpellArmageddonRainAoE:
                    case eSpells::SpellArmageddonHailAoE:
                    {
                        if (p_EffIndex != SpellEffIndex::EFFECT_0)
                            break;

                        if (!p_Targets.empty())
                            break;

                        if (SpellDestination const* l_Loc = p_Spell->m_targets.GetDst())
                            me->CastSpell(l_Loc->_position, eSpells::SpellArmageddonBlast, true);

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

                if (m_Phase != ePhases::PhaseInterDeceiversVeil && m_Phase != ePhases::PhaseInterEternalFlame)
                {
                    Position l_Pos = me->GetPosition();
                    if (g_CenterPos.GetExactDist(&l_Pos) >= 75.0f)
                    {
                        EnterEvadeMode();
                        return;
                    }
                }

                if (m_BerserkTimer)
                {
                    if (m_BerserkTimer <= p_Diff)
                    {
                        m_BerserkTimer = 0;

                        DoCast(me, eSharedSpells::SpellBerserk, true);
                    }
                    else
                        m_BerserkTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_DarknessMoves)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArmageddon:
                    {
                        Talk(eTalks::TalkArmageddon);
                        Talk(eTalks::TalkArmageddonWarn);

                        ++m_ArmageddonCount;

                        DoCast(me, eSpells::SpellArmageddonCast);

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventArmageddon))
                            events.ScheduleEvent(eEvents::EventArmageddon, l_Timer);

                        break;
                    }
                    case eEvents::EventRupturingSingularity:
                    {
                        Talk(eTalks::TalkRupturingSingularity);
                        Talk(eTalks::TalkRupturingSingularityWarn);

                        ++m_SingularityCount;

                        if (m_Phase == ePhases::PhaseInterEternalFlame)
                        {
                            if (IsMythic())
                            {
                                std::vector<uint8> l_IDx = { 1, 2, 3, 4 };

                                std::random_device l_RandomDevice;
                                std::mt19937 l_RandomGenerator(l_RandomDevice());
                                std::shuffle(l_IDx.begin(), l_IDx.end(), l_RandomGenerator);

                                if (!m_BiggestSingularity)
                                {
                                    uint32 l_Time = 5 * TimeConstants::IN_MILLISECONDS;

                                    for (uint8 l_I = 0; l_I < 2; ++l_I)
                                    {
                                        if (uint32 l_Delay = (l_Time * l_I))
                                        {
                                            AddTimedDelayedOperation(l_Delay, [this, l_IDx, l_I]() -> void
                                            {
                                                DoCast(m_SingularityPos[l_IDx[l_I]], eSpells::SpellRupturingSingularity, true);
                                            });
                                        }
                                        else
                                            DoCast(m_SingularityPos[l_IDx[l_I]], eSpells::SpellRupturingSingularity, true);
                                    }
                                }
                                else
                                {
                                    uint32 l_Time = 5 * TimeConstants::IN_MILLISECONDS;

                                    for (uint8 l_I = 0; l_I < 4; ++l_I)
                                    {
                                        if (uint32 l_Delay = (l_Time * l_I))
                                        {
                                            AddTimedDelayedOperation(l_Delay, [this, l_IDx, l_I]() -> void
                                            {
                                                DoCast(m_SingularityPos[l_IDx[l_I]], eSpells::SpellRupturingSingularity, true);
                                            });
                                        }
                                        else
                                            DoCast(m_SingularityPos[l_IDx[l_I]], eSpells::SpellRupturingSingularity, true);
                                    }
                                }

                                m_BiggestSingularity = !m_BiggestSingularity;
                            }
                            else
                                DoCast(m_SingularityPos[urand(1, eKJData::MaxSingularityPos - 1)], eSpells::SpellRupturingSingularity);
                        }
                        else
                            DoCast(m_SingularityPos[urand(0, eKJData::MaxSingularityPos - 1)], eSpells::SpellRupturingSingularity);

                        if (m_Phase == ePhases::PhaseInterDeceiversVeil || m_Phase == ePhases::PhaseInterEternalFlame)
                        {
                            if (m_Phase == ePhases::PhaseInterDeceiversVeil && IsMythic())
                            {
                                if (!(m_SingularityCount % 2))
                                    events.ScheduleEvent(eEvents::EventRupturingSingularity, 20 * TimeConstants::IN_MILLISECONDS);
                                else
                                    events.ScheduleEvent(eEvents::EventRupturingSingularity, 10 * TimeConstants::IN_MILLISECONDS);
                            }
                            else if (m_Phase == ePhases::PhaseInterEternalFlame && IsMythic() && m_SingularityCount < 3)
                            {
                                if (m_SingularityCount < 2)
                                    events.ScheduleEvent(eEvents::EventRupturingSingularity, 17 * TimeConstants::IN_MILLISECONDS);
                                else
                                    events.ScheduleEvent(eEvents::EventRupturingSingularity, 41 * TimeConstants::IN_MILLISECONDS);
                            }
                            else if ((IsLFR() || IsNormal()) || m_SingularityCount > 1 || IsMythic())
                                break;
                            else
                                events.ScheduleEvent(eEvents::EventRupturingSingularity, 30 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            if (uint32 l_Timer = GetEventTimer(eEvents::EventRupturingSingularity))
                                events.ScheduleEvent(eEvents::EventRupturingSingularity, l_Timer);
                        }

                        break;
                    }
                    case eEvents::EventBurstingDreadflame:
                    {
                        Talk(eTalks::TalkBurstingDreadflame);

                        DoCast(me, eSpells::SpellBurstingDreadflameSearcher, true);

                        ++m_BurstingCount;

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventBurstingDreadflame))
                            events.ScheduleEvent(eEvents::EventBurstingDreadflame, l_Timer);

                        break;
                    }
                    case eEvents::EventFocusedDreadflame:
                    {
                        if (me->IsMoving())
                        {
                            events.ScheduleEvent(eEvents::EventFocusedDreadflame, 1);
                            break;
                        }

                        DoCast(me, eSpells::SpellFocusedDreadflameAura, true);

                        ++m_FocusedCount;

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventFocusedDreadflame))
                            events.ScheduleEvent(eEvents::EventFocusedDreadflame, l_Timer);

                        break;
                    }
                    case eEvents::EventFelclaws:
                    {
                        Talk(eTalks::TalkMythicFelclaws);

                        DoCast(me, eSpells::SpellFelclaws);

                        events.ScheduleEvent(eEvents::EventFelclaws, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventIntermissionMove:
                    {
                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 68.0f, frand(0.0f, 2.0f * M_PI), true);

                        l_Pos.m_positionZ = me->m_positionZ;

                        AddTimedDelayedOperation(1, [this, l_Pos]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveFly, l_Pos);
                        });

                        events.ScheduleEvent(eEvents::EventIntermissionMove, 27 * TimeConstants::IN_MILLISECONDS + 300);
                        break;
                    }
                    case eEvents::EventIntermissionEnd:
                    {
                        events.CancelEvent(eEvents::EventFocusedDreadflame);

                        Position l_Pos = g_CenterPos;

                        l_Pos.m_positionZ = me->m_positionZ;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(eMoves::MoveToCenter, l_Pos);
                        break;
                    }
                    case eEvents::EventShadowReflectionErupting:
                    {
                        Talk(eTalks::TalkShadowReflection);

                        DoCast(me, eSpells::SpellShadowReflectionErupting);

                        if (m_Phase == ePhases::Phase2ReflectedSouls && !IsMythic())
                        {
                            if (IsLFR())
                                events.ScheduleEvent(eEvents::EventShadowReflectionErupting, 124 * TimeConstants::IN_MILLISECONDS + 400);
                            else
                                events.ScheduleEvent(eEvents::EventShadowReflectionErupting, 112 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (m_Phase == ePhases::Phase1TheBetrayer && IsMythic())
                            events.ScheduleEvent(eEvents::EventShadowReflectionErupting, 109 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventDarknessOfAThousandSouls:
                    {
                        ++m_DarknessCount;

                        if (!m_DarknessMoves)
                        {
                            m_DarknessMoves = true;

                            me->StopAttack();

                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveDarknessCenter, g_CenterPos);

                            Talk(eTalks::TalkDarknessOfAThousandSouls);
                        }
                        else
                            DoCast(me, eSpells::SpellDarknessThousandSoulsCast);

                        if (m_DarknessCount == 1)
                        {
                            events.ScheduleEvent(eEvents::EventDarknessOfAThousandSouls, 90 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventDemonicObelisk, 25 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventDarknessOfAThousandSouls, 95 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventDemonicObelisk, 28 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eEvents::EventTearRift:
                    {
                        Talk(eTalks::TalkTearRift);

                        me->DespawnCreaturesInArea(eCreatures::NpcNetherRift);

                        bool l_XSide = urand(0, 1);

                        float l_X = 0.0f;
                        float l_Y = 0.0f;
                        float l_Z = 5385.909f;

                        if (l_XSide)
                        {
                            l_X = urand(0, 1) ? 4450.902f : 4549.154f;
                            l_Y = frand(-1559.2151f, -1461.0191f);
                        }
                        else
                        {
                            l_Y = urand(0, 1) ? -1559.2151f : -1461.0191f;
                            l_X = frand(4450.902f, 4549.154f);
                        }

                        Position l_Pos = { l_X, l_Y, l_Z, 0.0f };

                        l_Pos.SetOrientation(l_Pos.GetAngle(&g_CenterPos));

                        if (Creature* l_Rift = me->SummonCreature(eCreatures::NpcNetherRift, l_Pos))
                            m_RiftGuid = l_Rift->GetGUID();

                        DoCast(l_Pos, eSpells::SpellTearRiftCast);
                        DoCast(g_CenterPos, eSpells::SpellTearRiftDummy, true);

                        events.ScheduleEvent(eEvents::EventTearRift, (m_ObeliskCount == 0 ? 90 : 95) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFlamingOrb:
                    {
                        m_FlamingOrbCount = (m_FlamingOrbCount % (IsMythic() ? 2 : 1)) + 1;

                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(10.0f, 30.0f), frand(0.0f, 2.0f * M_PI), true);

                        DoCast(l_Pos, eSpells::SpellFlamingOrbMissile, true);

                        if (IsMythic())
                        {
                            if (m_FlamingOrbCount == 1)
                                events.ScheduleEvent(eEvents::EventFlamingOrb, 15 * TimeConstants::IN_MILLISECONDS);
                            else if (m_FlamingOrbCount == 2)
                                events.ScheduleEvent(eEvents::EventFlamingOrb, 16 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventFlamingOrb, 64 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (m_FlamingOrbCount == 1)
                            events.ScheduleEvent(eEvents::EventFlamingOrb, 30 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventFlamingOrb, 64 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventDemonicObelisk:
                    {
                        ++m_ObeliskCount;

                        Position l_Pos = g_CenterPos;
                        float l_Angle = frand(0.0f, 2.0f * M_PI);

                        uint8 l_StartCount = 0;

                        if (IsMythic())
                            l_StartCount = m_ObeliskCount + 2;
                        else if (IsHeroic())
                            l_StartCount = m_DarknessCount + 2;
                        else
                            l_StartCount = m_DarknessCount + 1;

                        for (uint8 l_I = 0; l_I < l_StartCount; ++l_I)
                        {
                            g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 40.0f), l_Angle, true);

                            l_Angle = Position::NormalizeOrientation(l_Angle + frand(M_PI / 2.0f, 2.0f * M_PI / 3.0f));

                            me->SummonCreature(eCreatures::NpcDemonicObelisk, l_Pos);
                        }

                        if ((m_ObeliskCount % 2) == 1)
                            events.ScheduleEvent(eEvents::EventDemonicObelisk, 36 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventShadowReflectionWailing:
                    {
                        Talk(eTalks::TalkShadowReflection);

                        ++m_WailingCount;

                        if (Player* l_Tank = SelectMainTank())
                            DoCast(l_Tank, eSpells::SpellShadowReflectionWailing);

                        if (IsMythic() && m_Phase == ePhases::Phase2ReflectedSouls && m_WailingCount < m_WailingMythicTimers.size())
                            events.ScheduleEvent(eEvents::EventShadowReflectionWailing, m_WailingMythicTimers[m_WailingCount]);
                        else
                            events.ScheduleEvent(eEvents::EventShadowReflectionWailing, 114 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventShadowReflectionHopeless:
                    {
                        Talk(eTalks::TalkShadowReflection);

                        DoCast(me, eSpells::SpellShadowReflectionHopeless);

                        events.ScheduleEvent(eEvents::EventShadowReflectionHopeless, 196 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void DefaultEvents(ePhases p_Phase)
            {
                events.Reset();

                me->RemoveAura(eSpells::SpellNetherGale);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil8YardsAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeceiversVeil40YardsAura);
                }

                m_Phase = p_Phase;

                m_ArmageddonCount = 0;
                m_SingularityCount = 0;
                m_BurstingCount = 0;
                m_FocusedCount = 0;
                m_DarknessCount = 0;
                m_FlamingOrbCount = 0;
                m_ObeliskCount = 0;
                m_WailingCount = 0;

                auto l_DiffMap = m_TimerData.find(GetDifficulty());
                if (l_DiffMap == m_TimerData.end())
                    return;

                auto l_PhaseIter = l_DiffMap->second.find(m_Phase);
                if (l_PhaseIter == l_DiffMap->second.end())
                    return;

                for (uint8 l_I = 0; l_I < eEvents::EventMax; ++l_I)
                {
                    auto l_EventIter = l_PhaseIter->second.find(l_I);
                    if (l_EventIter == l_PhaseIter->second.end())
                        continue;

                    uint8 l_Counter = 0;
                    switch (l_I)
                    {
                        case eEvents::EventArmageddon:
                            l_Counter = m_ArmageddonCount;
                            break;
                        case eEvents::EventRupturingSingularity:
                            l_Counter = m_SingularityCount;
                            break;
                        case eEvents::EventBurstingDreadflame:
                            l_Counter = m_BurstingCount;
                            break;
                        case eEvents::EventFocusedDreadflame:
                            l_Counter = m_FocusedCount;
                            break;
                        default:
                            break;
                    }

                    if (l_Counter >= l_EventIter->second.size())
                        continue;

                    events.ScheduleEvent(l_I, l_EventIter->second[l_Counter]);
                }

                switch (m_Phase)
                {
                    case ePhases::Phase1TheBetrayer:
                    {
                        events.ScheduleEvent(eEvents::EventFelclaws, 25 * TimeConstants::IN_MILLISECONDS);

                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventShadowReflectionErupting, 18 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventShadowReflectionWailing, 55 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (IsHeroic())
                            events.ScheduleEvent(eEvents::EventShadowReflectionErupting, 20 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case ePhases::PhaseInterEternalFlame:
                    {
                        DoCast(me, eSpells::SpellNetherGale, true);

                        events.ScheduleEvent(eEvents::EventIntermissionEnd, 60 * TimeConstants::IN_MILLISECONDS);

                        if (IsHeroic())
                            events.ScheduleEvent(eEvents::EventRupturingSingularity, 13 * TimeConstants::IN_MILLISECONDS + 300);
                        else if (IsMythic())
                            events.ScheduleEvent(eEvents::EventRupturingSingularity, 14 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case ePhases::Phase2ReflectedSouls:
                    {
                        events.ScheduleEvent(eEvents::EventFelclaws, 10 * TimeConstants::IN_MILLISECONDS + 400);

                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventShadowReflectionErupting, 164 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventShadowReflectionWailing, 49 * TimeConstants::IN_MILLISECONDS + 400);
                            events.ScheduleEvent(eEvents::EventShadowReflectionHopeless, 27 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventShadowReflectionErupting, 13 * TimeConstants::IN_MILLISECONDS + 900);

                        if (IsHeroic())
                            events.ScheduleEvent(eEvents::EventShadowReflectionWailing, 48 * TimeConstants::IN_MILLISECONDS + 400);

                        break;
                    }
                    case ePhases::PhaseInterDeceiversVeil:
                    {
                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventRupturingSingularity, 19 * TimeConstants::IN_MILLISECONDS + 100);

                        break;
                    }
                    case ePhases::Phase3DarknessOfAThousandSouls:
                    {
                        events.ScheduleEvent(eEvents::EventFelclaws, 11 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventDarknessOfAThousandSouls, 2 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventTearRift, 15 * TimeConstants::IN_MILLISECONDS);

                        if (IsHeroicOrMythic())
                            events.ScheduleEvent(eEvents::EventFlamingOrb, (IsMythic() ? 40 : 30) * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetEventTimer(uint8 p_EventID) const
            {
                auto l_DiffMap = m_TimerData.find(GetDifficulty());
                if (l_DiffMap == m_TimerData.end())
                    return 0;

                auto l_PhaseIter = l_DiffMap->second.find(m_Phase);
                if (l_PhaseIter == l_DiffMap->second.end())
                    return 0;

                auto l_EventIter = l_PhaseIter->second.find(p_EventID);
                if (l_EventIter == l_PhaseIter->second.end())
                    return 0;

                uint8 l_Counter = 0;
                switch (p_EventID)
                {
                    case eEvents::EventArmageddon:
                        l_Counter = m_ArmageddonCount;
                        break;
                    case eEvents::EventRupturingSingularity:
                        l_Counter = m_SingularityCount;
                        break;
                    case eEvents::EventBurstingDreadflame:
                        l_Counter = m_BurstingCount;
                        break;
                    case eEvents::EventFocusedDreadflame:
                        l_Counter = m_FocusedCount;
                        break;
                    default:
                        break;
                }

                if (l_Counter >= l_EventIter->second.size())
                    return 0;

                return l_EventIter->second[l_Counter];
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_tos_kil_jaedenAI(p_Creature);
        }
};

/// Armageddon Stalker - 120839
class npc_kj_armageddon_stalker : public CreatureScript
{
    public:
        npc_kj_armageddon_stalker() : CreatureScript("npc_kj_armageddon_stalker") { }

        enum eSpells
        {
            SpellArmageddonRainAT       = 234295,
            SpellArmageddonRainVisual   = 240832,
            SpellArmageddonRainMissile  = 234309,
            SpellArmageddonRainAoE      = 234310,

            SpellArmageddonHailAT       = 240911,
            SpellArmageddonHailVisual   = 240915,
            SpellArmageddonHailMissile  = 240912,
            SpellArmageddonHailAoE      = 240916,

            SpellArmageddonBlast        = 240908
        };

        enum eActions
        {
            ActionArmageddonRain = 1,
            ActionArmageddonHail
        };

        struct npc_kj_armageddon_stalkerAI : public Scripted_NoMovementAI
        {
            npc_kj_armageddon_stalkerAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionArmageddonRain:
                    {
                        DoCast(me, eSpells::SpellArmageddonRainAT, true);
                        DoCast(me, eSpells::SpellArmageddonRainVisual, true);
                        break;
                    }
                    case eActions::ActionArmageddonHail:
                    {
                        DoCast(me, eSpells::SpellArmageddonHailAT, true);
                        DoCast(me, eSpells::SpellArmageddonHailVisual, true);
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
                    case eSpells::SpellArmageddonRainMissile:
                    {
                        me->RemoveAllAuras();
                        me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);

                        InstanceScript* l_Instance = me->GetInstanceScript();
                        if (!l_Instance)
                            break;

                        Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden));
                        if (!l_Boss)
                            break;

                        l_Boss->CastSpell(p_Target, eSpells::SpellArmageddonRainAoE, true);
                        break;
                    }
                    case eSpells::SpellArmageddonHailMissile:
                    {
                        me->RemoveAllAuras();
                        me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);

                        InstanceScript* l_Instance = me->GetInstanceScript();
                        if (!l_Instance)
                            break;

                        Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden));
                        if (!l_Boss)
                            break;

                        l_Boss->CastSpell(p_Target, eSpells::SpellArmageddonHailAoE, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_armageddon_stalkerAI(p_Creature);
        }
};

/// Erupting Reflection - 119206
class npc_kj_erupting_reflection : public CreatureScript
{
    public:
        npc_kj_erupting_reflection() : CreatureScript("npc_kj_erupting_reflection") { }

        enum eSpells
        {
            SpellEruptingReflectionClone    = 236711,
            SpellEruptingDreadflame         = 235120,
            SpellEruption                   = 235974,
            SpellLingeringEruption          = 243536,
            SpellEruptingEssence            = 249629
        };

        enum eEvent
        {
            EventEruptingDreadflame = 1
        };

        struct npc_kj_erupting_reflectionAI : public Scripted_NoMovementAI
        {
            npc_kj_erupting_reflectionAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            uint64 m_OwnerGuid = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                m_OwnerGuid = p_Summoner->GetGUID();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetInCombatWithZone();

                me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_BATTLEROAR);

                p_Summoner->CastSpell(me, eSpells::SpellEruptingReflectionClone, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvent::EventEruptingDreadflame, 500);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellEruptingDreadflame)
                    DoCast(me, eSpells::SpellEruption, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellEruption:
                    case eSpells::SpellEruptingEssence:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (p_Object->GetEntry() != me->GetEntry())
                                return true;

                            return false;
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);

                if (IsMythic())
                {
                    DoCast(me, eSpells::SpellEruptingEssence, true);

                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden));
                        if (!l_Boss)
                            return;

                        if (Player* l_Player = Player::GetPlayer(*me, m_OwnerGuid))
                            l_Boss->CastSpell(l_Player, eSpells::SpellLingeringEruption, true);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvent::EventEruptingDreadflame)
                {
                    DoCast(me, eSpells::SpellEruptingDreadflame);

                    events.ScheduleEvent(eEvent::EventEruptingDreadflame, 6 * TimeConstants::IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_erupting_reflectionAI(p_Creature);
        }
};

/// Shadowsoul - 121193
class npc_kj_shadowsoul : public CreatureScript
{
    public:
        npc_kj_shadowsoul() : CreatureScript("npc_kj_shadowsoul") { }

        enum eSpells
        {
            SpellShadowsoulWarningVisual    = 241905,
            SpellDestabilizedShadowsoul     = 241702,
            SpellDestabilizedShadowsoulTP   = 242074
        };

        enum eEvent
        {
            EventDestabilizedShadowsoul = 1
        };

        enum eAction
        {
            ActionShadowSoulCast = 4
        };

        struct npc_kj_shadowsoulAI : public Scripted_NoMovementAI
        {
            npc_kj_shadowsoulAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellShadowsoulWarningVisual, true);

                me->SetInCombatWithZone();
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSpells::SpellShadowsoulWarningVisual)
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    events.ScheduleEvent(eEvent::EventDestabilizedShadowsoul, 1);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellDestabilizedShadowsoul)
                {
                    if (me->GetAnyOwner())
                    {
                        if (Creature* l_Boss = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Boss->IsAIEnabled)
                                l_Boss->AI()->DoAction(eAction::ActionShadowSoulCast);
                        }
                    }

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 35.0f), frand(0.0f, 2.0f * M_PI), true);

                        me->NearTeleportTo(l_Pos);
                    });
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

                if (events.ExecuteEvent() == eEvent::EventDestabilizedShadowsoul)
                {
                    DoCast(me, eSpells::SpellDestabilizedShadowsoul);

                    events.ScheduleEvent(eEvent::EventDestabilizedShadowsoul, 8 * TimeConstants::IN_MILLISECONDS + 200);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_shadowsoulAI(p_Creature);
        }
};

/// Illidan Stormrage - 121227
class npc_kj_illidan_stormrage : public CreatureScript
{
    public:
        npc_kj_illidan_stormrage() : CreatureScript("npc_kj_illidan_stormrage") { }

        enum eSpell
        {
            SpellCursedSight    = 241768
        };

        enum eVisuals
        {
            VisualSpawn = 12447,
            VisualConv  = 5170
        };

        struct npc_kj_illidan_stormrageAI : public Scripted_NoMovementAI
        {
            npc_kj_illidan_stormrageAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->PlayOneShotAnimKitId(eVisuals::VisualSpawn);
                });

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                {
                    me->SetInCombatWithZone();
                });

                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpell::SpellCursedSight, true);

                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eVisuals::VisualConv, me, nullptr, *me))
                        delete l_Conversation;
                });
            }

            void DoAction(int32 const /*p_Diff*/) override
            {
                me->InterruptNonMeleeSpells(true);
                me->RemoveAllAreasTrigger();

                me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_illidan_stormrageAI(p_Creature);
        }
};

/// Demonic Obelisk - 120270
class npc_kj_demonic_obelisk : public CreatureScript
{
    public:
        npc_kj_demonic_obelisk() : CreatureScript("npc_kj_demonic_obelisk") { }

        enum eSpells
        {
            SpellDemonicObeliskVisual = 239785,
            SpellDemonicObeliskDamage = 239852
        };

        enum eVisuals
        {
            ObeliskVisualStart  = 4396,
            ObeliskVisualEnd    = 8216
        };

        struct npc_kj_demonic_obeliskAI : public Scripted_NoMovementAI
        {
            npc_kj_demonic_obeliskAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetInCombatWithZone();

                DoCast(me, eSpells::SpellDemonicObeliskVisual, true);

                AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->PlayOneShotAnimKitId(eVisuals::ObeliskVisualStart);

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Position l_Pos  = me->GetPosition();
                        float l_Angle   = l_Pos.m_orientation;
                        float l_Step    = M_PI / 2.0f;

                        for (uint8 l_I = 0; l_I < 4; ++l_I)
                        {
                            me->SimplePosXYRelocationByAngle(l_Pos, 100.0f, l_Angle, true);

                            DoCast(l_Pos, eSpells::SpellDemonicObeliskDamage, true);

                            l_Angle = Position::NormalizeOrientation(l_Angle + l_Step);
                        }

                        me->PlayOneShotAnimKitId(eVisuals::ObeliskVisualEnd);
                        me->DespawnOrUnsummon(1);
                    });
                });
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::SpellDemonicObeliskDamage && !p_Targets.empty())
                {
                    WorldLocation const* l_Dest = p_Spell->m_targets.GetDstPos();
                    if (!l_Dest)
                        return;

                    p_Targets.remove_if([this, l_Dest](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object)
                            return true;

                        if (!p_Object->IsInAxe(me, l_Dest, 9.6f))
                            return true;

                        return false;
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_demonic_obeliskAI(p_Creature);
        }
};

/// Nether Rift - 120390
class npc_kj_nether_rift : public CreatureScript
{
    public:
        npc_kj_nether_rift() : CreatureScript("npc_kj_nether_rift") { }

        enum eSpells
        {
            SpellTearRiftVisual     = 240020,
            SpellTearRiftATs        = 239130,
            SpellFlamingDetonation  = 239267
        };

        struct npc_kj_nether_riftAI : public Scripted_NoMovementAI
        {
            npc_kj_nether_riftAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetInCombatWithZone();

                DoCast(me, eSpells::SpellTearRiftVisual, true);

                uint64 l_Guid = p_Summoner->GetGUID();
                AddTimedDelayedOperation(35 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                {
                    if (Unit* l_Summoner = Unit::GetUnit(*me, l_Guid))
                    {
                        l_Summoner->CastSpell(me, eSpells::SpellTearRiftATs, true);

                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = l_Ref->getTarget())
                            {
                                if (!l_Target->IsPlayer())
                                    continue;

                                l_Target->ToPlayer()->SendApplyMovementForce(me->GetGUID(), true, me->GetPosition(), 2.0f, 1);
                            }
                        }

                        std::list<AreaTrigger*> l_ATs;

                        me->GetAreaTriggerListWithSpellIDInRange(l_ATs, eSpells::SpellFlamingDetonation, 200.0f);

                        for (AreaTrigger* l_AT : l_ATs)
                        {
                            Position l_Pos = me->GetPosition();

                            me->SimplePosXYRelocationByAngle(l_Pos, 10.0f, me->GetAngle(l_AT), true);

                            l_AT->MoveAreaTrigger(l_Pos, 3, true);
                        }
                    }
                });
            }

            void JustDespawned() override
            {
                me->RemoveAllAreasTrigger();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->DoRemoveForcedMovementsOnPlayers(me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_nether_riftAI(p_Creature);
        }
};

/// Wailing Reflection - 119107
class npc_kj_wailing_reflection : public CreatureScript
{
    public:
        npc_kj_wailing_reflection() : CreatureScript("npc_kj_wailing_reflection") { }

        enum eSpells
        {
            SpellWailingReflectionClone = 236475,
            SpellSorrowfulWailChannel   = 241564,
            SpellSorrowfulWailAoE       = 236381,
            SpellSorrowfulWailVisual    = 241597,
            SpellCrescendoAura          = 241606,
            SpellLingeringWail          = 243624
        };

        enum eEvent
        {
            EventSorrowfulWail = 1
        };

        struct npc_kj_wailing_reflectionAI : public ScriptedAI
        {
            npc_kj_wailing_reflectionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_OwnerGuid = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                m_OwnerGuid = p_Summoner->GetGUID();

                me->SetInCombatWithZone();

                me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_BATTLEROAR);

                p_Summoner->CastSpell(me, eSpells::SpellWailingReflectionClone, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvent::EventSorrowfulWail, 15 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    if (IsMythic())
                    {
                        Creature* l_Boss = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden));
                        if (!l_Boss)
                            return;

                        if (Player* l_Player = Player::GetPlayer(*me, m_OwnerGuid))
                            l_Boss->CastSpell(l_Player, eSpells::SpellLingeringWail, true);
                    }
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (p_SpellID == eSpells::SpellSorrowfulWailChannel)
                {
                    DoCast(me, eSpells::SpellSorrowfulWailAoE, true);
                    DoCast(me, eSpells::SpellSorrowfulWailVisual, true);
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::SpellSorrowfulWailAoE && !p_Targets.empty())
                {
                    float l_BaseRadius  = 10.0f;
                    float l_RadiusBonus = 0.0f;

                    if (AuraEffect const* l_AurEff = me->GetAuraEffect(eSpells::SpellCrescendoAura, SpellEffIndex::EFFECT_1))
                        l_RadiusBonus = l_AurEff->GetAmount();

                    p_Targets.remove_if([this, l_BaseRadius, l_RadiusBonus](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object)
                            return true;

                        if (p_Object->GetDistance(me) > (l_BaseRadius + l_RadiusBonus))
                            return true;

                        return false;
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvent::EventSorrowfulWail)
                {
                    DoCast(me, eSpells::SpellSorrowfulWailChannel);

                    events.ScheduleEvent(eEvent::EventSorrowfulWail, 15 * TimeConstants::IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_wailing_reflectionAI(p_Creature);
        }
};

/// Flaming Orb - 120082
class npc_kj_flaming_orb : public CreatureScript
{
    public:
        npc_kj_flaming_orb() : CreatureScript("npc_kj_flaming_orb") { }

        enum eSpells
        {
            SpellFlamingOrbAT       = 239256,
            SpellFlamingOrbSearcher = 244856,
            SpellFixate             = 239280
        };

        struct npc_kj_flaming_orbAI : public ScriptedAI
        {
            npc_kj_flaming_orbAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            float m_Radius = 0.0f;

            uint64 m_TargetGuid = 0;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                SetCombatMovement(false);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellFlamingOrbAT, true);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellFlamingOrbSearcher, true);
                });
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellFlamingOrbSearcher:
                    {
                        p_Targets.sort(JadeCore::DistanceCompareOrderPred(me));

                        WorldObject* l_Target = p_Targets.front();

                        p_Targets.clear();
                        p_Targets.push_back(l_Target);
                        break;
                    }
                    case eSpells::SpellFixate:
                    {
                        p_Targets.clear();

                        if (Unit* l_Unit = Unit::GetUnit(*me, m_TargetGuid))
                            p_Targets.push_back(l_Unit);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellFlamingOrbSearcher)
                {
                    m_TargetGuid = p_Target->GetGUID();

                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    DoCast(me, eSpells::SpellFixate, true);

                    me->GetMotionMaster()->Clear();
                }
            }

            void SetFData(uint32 /*p_ID*/, float p_Value) override
            {
                m_Radius = p_Value;
            }

            float GetFData(uint32 /*p_ID*/) const override
            {
                me->RemoveAura(eSpells::SpellFlamingOrbAT);
                me->InterruptNonMeleeSpells(true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->StopMoving();

                return m_Radius;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_TargetGuid)
                    return;

                if (Unit* l_Unit = Unit::GetUnit(*me, m_TargetGuid))
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, l_Unit->GetPosition());
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_flaming_orbAI(p_Creature);
        }
};

/// Hopeless Reflection - 119663
class npc_kj_hopeless_reflection : public CreatureScript
{
    public:
        npc_kj_hopeless_reflection() : CreatureScript("npc_kj_hopeless_reflection") { }

        enum eSpells
        {
            SpellHopelessReflectionClone    = 237724,
            SpellHopelessnessCast           = 237725,
            SpellHopelessnessAoE            = 237728,
            SpellHopelessReflectionFadeOut  = 243778,
            SpellLingeringHopeAura          = 243621
        };

        struct npc_kj_hopeless_reflectionAI : public Scripted_NoMovementAI
        {
            npc_kj_hopeless_reflectionAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool m_Healed = false;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->SetHealth(1);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->DisableHealthRegen();

                me->SetInCombatWithZone();

                p_Summoner->CastSpell(me, eSpells::SpellHopelessReflectionClone, true);

                DoCast(me, eSpells::SpellHopelessnessCast);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellHopelessnessCast:
                    {
                        m_Healed = true;

                        DoCast(me, eSpells::SpellHopelessnessAoE, true);
                        break;
                    }
                    case eSpells::SpellHopelessnessAoE:
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_KJ = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::BossKilJaeden)))
                                l_KJ->CastSpell(l_KJ, eSpells::SpellLingeringHopeAura, true);
                        }

                        DoCast(me, eSpells::SpellHopelessReflectionFadeOut, true);

                        me->DespawnOrUnsummon(6 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellHopelessnessAoE)
                {
                    float l_Radius = std::max(5.0f, CalculatePct(50.0f, (100.0f - me->GetHealthPct())));

                    uint64 l_Guid = p_AreaTrigger->GetGUID();
                    AddTimedDelayedOperation(10, [this, l_Guid, l_Radius]() -> void
                    {
                        if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*me, l_Guid))
                        {
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, 17);
                            l_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
                            l_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
                            l_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Radius);
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
                            l_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 100);
                        }

                        AddTimedDelayedOperation(100, [this, l_Guid, l_Radius]() -> void
                        {
                            if (AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*me, l_Guid))
                                l_AreaTrigger->SetRadius(l_Radius);
                        });
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->GetHealthPct() >= 100.0f && !m_Healed)
                {
                    me->InterruptNonMeleeSpells(true);

                    m_Healed = true;

                    DoCast(me, eSpells::SpellHopelessnessAoE, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kj_hopeless_reflectionAI(p_Creature);
        }
};

/// Shadow Reflection: Erupting - 236710
class spell_kj_shadow_reflection_erupting : public SpellScriptLoader
{
    public:
        spell_kj_shadow_reflection_erupting() : SpellScriptLoader("spell_kj_shadow_reflection_erupting") { }

        enum eSpell
        {
            ShadowReflectionEruptingSummon = 242032
        };

        class spell_kj_shadow_reflection_erupting_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_shadow_reflection_erupting_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::ShadowReflectionEruptingSummon, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kj_shadow_reflection_erupting_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_SCALE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_shadow_reflection_erupting_AuraScript();
        }
};

/// Illidan's Sightless Gaze - 241721
class spell_kj_illidans_sightless_gaze : public SpellScriptLoader
{
    public:
        spell_kj_illidans_sightless_gaze() : SpellScriptLoader("spell_kj_illidans_sightless_gaze") { }

        enum eSpells
        {
            SpellDeceiversVeil8y    = 236555,
            SpellDeceiversVeil40y   = 242696
        };

        class spell_kj_illidans_sightless_gaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_illidans_sightless_gaze_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveAura(eSpells::SpellDeceiversVeil40y);
                l_Target->CastSpell(l_Target, eSpells::SpellDeceiversVeil8y, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kj_illidans_sightless_gaze_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCREEN_EFFECT, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_illidans_sightless_gaze_AuraScript();
        }
};

/// Deceiver's Veil - 236555
/// Deceiver's Veil - 242696
class spell_kj_deceivers_veil_periodic : public SpellScriptLoader
{
    public:
        spell_kj_deceivers_veil_periodic() : SpellScriptLoader("spell_kj_deceivers_veil_periodic") { }

        enum eSpell
        {
            SpellChokingShadow = 242822
        };

        class spell_kj_deceivers_veil_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_deceivers_veil_periodic_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    {
                        if (Creature* l_KJ = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::BossKilJaeden)))
                            l_KJ->CastSpell(l_Target, eSpell::SpellChokingShadow, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kj_deceivers_veil_periodic_AuraScript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_deceivers_veil_periodic_AuraScript();
        }
};

/// Gravity Squeeze - 239154
class spell_kj_gravity_squeeze : public SpellScriptLoader
{
    public:
        spell_kj_gravity_squeeze() : SpellScriptLoader("spell_kj_gravity_squeeze") { }

        enum eSpell
        {
            SpellGravitySqueezeDoT = 239155
        };

        class spell_kj_gravity_squeeze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_gravity_squeeze_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    const_cast<AuraEffect*>(p_AurEff)->SetAmount(p_AurEff->GetAmount() + 1);

                    if (AuraEffect* l_DoT = l_Target->GetAuraEffect(eSpell::SpellGravitySqueezeDoT, SpellEffIndex::EFFECT_0))
                    {
                        int32 l_Amount = l_DoT->GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].BasePoints;

                        AddPct(l_Amount, p_AurEff->GetAmount() * 10);

                        l_DoT->ChangeAmount(l_Amount);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kj_gravity_squeeze_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_gravity_squeeze_AuraScript();
        }
};

/// Shadow Reflection: Wailing - 236378
class spell_kj_shadow_reflection_wailing : public SpellScriptLoader
{
    public:
        spell_kj_shadow_reflection_wailing() : SpellScriptLoader("spell_kj_shadow_reflection_wailing") { }

        enum eSpell
        {
            ShadowReflectionWailingSummon = 243408
        };

        class spell_kj_shadow_reflection_wailing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_shadow_reflection_wailing_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::ShadowReflectionWailingSummon, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kj_shadow_reflection_wailing_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_SCALE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_shadow_reflection_wailing_AuraScript();
        }
};

/// Lingering Wail - 243624
class spell_kj_lingering_wail : public SpellScriptLoader
{
    public:
        spell_kj_lingering_wail() : SpellScriptLoader("spell_kj_lingering_wail") { }

        enum eSpell
        {
            WailingGraspMain = 243625
        };

        class spell_kj_lingering_wail_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_lingering_wail_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::WailingGraspMain, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kj_lingering_wail_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_lingering_wail_AuraScript();
        }
};

/// Wailing Grasp - 243625
class spell_kj_wailing_grasp : public SpellScriptLoader
{
    public:
        spell_kj_wailing_grasp() : SpellScriptLoader("spell_kj_wailing_grasp") { }

        enum eSpell
        {
            WailingGraspJump = 243626
        };

        class spell_kj_wailing_grasp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kj_wailing_grasp_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                        l_Target->CastSpell(l_Caster, eSpell::WailingGraspJump, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_kj_wailing_grasp_SpellScript::HandleDummy, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kj_wailing_grasp_SpellScript();
        }
};

/// Lingering Eruption - 243536
class spell_kj_lingering_eruption : public SpellScriptLoader
{
    public:
        spell_kj_lingering_eruption() : SpellScriptLoader("spell_kj_lingering_eruption") { }

        enum eSpells
        {
            SpellLatentEruptionMissile  = 245014,
            SpellEruptingOrb            = 243542
        };

        class spell_kj_lingering_eruption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_lingering_eruption_AuraScript);

            float m_StartAngle = 0.0f;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->CastSpell(l_Target, eSpells::SpellLatentEruptionMissile, true);

                    Position l_Pos  = l_Target->GetPosition();
                    float l_Angle   = Position::NormalizeOrientation(l_Target->m_orientation + (M_PI / 6.0f) + m_StartAngle);

                    for (uint8 l_I = 0; l_I < 6; ++l_I)
                    {
                        l_Target->SimplePosXYRelocationByAngle(l_Pos, 6.0f, l_Angle, true);

                        l_Pos.SetOrientation(Position::NormalizeOrientation(l_Pos.GetAngle(l_Target) + M_PI));

                        l_Target->CastSpell(l_Pos, eSpells::SpellEruptingOrb, true);

                        l_Angle = Position::NormalizeOrientation(l_Angle + (M_PI / 3.0f));
                    }

                    m_StartAngle += (M_PI / 12.0f);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kj_lingering_eruption_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_lingering_eruption_AuraScript();
        }
};

/// Shadow Reflection: Hopeless - 237590
class spell_kj_shadow_reflection_hopeless : public SpellScriptLoader
{
    public:
        spell_kj_shadow_reflection_hopeless() : SpellScriptLoader("spell_kj_shadow_reflection_hopeless") { }

        enum eSpell
        {
            ShadowReflectionHopelessSummon = 243404
        };

        class spell_kj_shadow_reflection_hopeless_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kj_shadow_reflection_hopeless_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::ShadowReflectionHopelessSummon, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kj_shadow_reflection_hopeless_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_SCALE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kj_shadow_reflection_hopeless_AuraScript();
        }
};

/// Flaming Orb - 239256
class areatrigger_kj_flaming_orb : public AreaTriggerEntityScript
{
    public:
        areatrigger_kj_flaming_orb() : AreaTriggerEntityScript("areatrigger_kj_flaming_orb") { }

        enum eSpell
        {
            SpellFlamingDetonation = 239267
        };

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 8 * TimeConstants::IN_MILLISECONDS);

            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 0.25f);
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);

            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (!l_Caster->ToCreature() || !l_Caster->ToCreature()->IsAIEnabled)
                    return;

                l_Caster->ToCreature()->AI()->SetFData(0, p_AreaTrigger->GetRadius());
            }
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            float l_MinRadius   = p_AreaTrigger->GetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4);
            float l_DecPerMs    = (p_AreaTrigger->GetAreaTriggerInfo().Radius - l_MinRadius) / float(p_AreaTrigger->GetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE));
            float l_CurrRadius  = p_AreaTrigger->GetRadius();

            l_CurrRadius -= l_DecPerMs * p_Time;

            p_AreaTrigger->SetRadius(std::max(l_CurrRadius, l_MinRadius));

            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (!l_Caster->ToCreature() || !l_Caster->ToCreature()->IsAIEnabled)
                    return;

                l_Caster->ToCreature()->AI()->SetFData(0, p_AreaTrigger->GetRadius());
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_kj_flaming_orb();
        }
};

/// Flaming Detonation - 239267
class areatrigger_kj_flaming_detonation : public AreaTriggerEntityScript
{
    public:
        areatrigger_kj_flaming_detonation() : AreaTriggerEntityScript("areatrigger_kj_flaming_detonation") { }

        enum eSpell
        {
            SpellTearRiftATs = 239130
        };

        uint32 m_MoveTime = 0;
        uint32 m_ScaleTime = 0;

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (!l_Caster->ToCreature() || !l_Caster->ToCreature()->IsAIEnabled)
                    return;

                float l_Radius = l_Caster->ToCreature()->AI()->GetFData() * 4.0f;

                p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, uint32(l_Radius * 100.0f));

                p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, p_AreaTrigger->GetAreaTriggerInfo().ElapsedTime);
                p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
                p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
                p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Radius);
                p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);

                l_Caster->ToCreature()->DespawnOrUnsummon(p_AreaTrigger->GetDuration());

                if (AreaTrigger* l_Rift = p_AreaTrigger->FindNearestAreaTrigger(eSpell::SpellTearRiftATs, 200.0f))
                {
                    Position l_Pos = l_Rift->GetPosition();

                    l_Rift->SimplePosXYRelocationByAngle(l_Pos, 10.0f, l_Rift->GetAngle(p_AreaTrigger), true);

                    p_AreaTrigger->MoveAreaTrigger(l_Pos, 3, true);
                }
            }
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            /// Begin to shrink
            if (m_MoveTime)
            {
                if (m_MoveTime <= p_Time)
                {
                    m_MoveTime = 0;

                    p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 500);

                    p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, p_AreaTrigger->GetAreaTriggerInfo().ElapsedTime);
                    p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, p_AreaTrigger->GetRadius());
                    p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
                    p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, p_AreaTrigger->GetRadius() - 1.0f);
                    p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                    p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);

                    m_ScaleTime = 500;
                    return;
                }
                else
                    m_MoveTime -= p_Time;
            }
            /// Shrink
            else if (m_ScaleTime)
            {
                if (m_ScaleTime <= p_Time)
                {
                    p_AreaTrigger->SetRadius(p_AreaTrigger->GetRadius() - 1.0f);

                    if (p_AreaTrigger->GetRadius() > 0.0f)
                    {
                        p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, p_AreaTrigger->GetAreaTriggerInfo().ElapsedTime);
                        p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, p_AreaTrigger->GetRadius());
                        p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
                        p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, std::max(p_AreaTrigger->GetRadius() - 1.0f, 0.0f));
                        p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                        p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
                    }

                    m_ScaleTime = 500;
                    return;
                }
                else
                    m_ScaleTime -= p_Time;
            }

            float l_MaxRadius = p_AreaTrigger->GetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4);
            float l_ScaleTime = p_AreaTrigger->GetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE);
            float l_RadPerMSec = (l_MaxRadius - 1.0f) / float(l_ScaleTime);
            float l_CurrRadius = p_AreaTrigger->GetRadius();

            l_CurrRadius += l_RadPerMSec * p_Time;

            p_AreaTrigger->SetRadius(std::min(l_CurrRadius, l_MaxRadius));
        }

        void OnMoveAreaTrigger(AreaTrigger* /*p_AreaTrigger*/, uint32 p_ArrivalTime) override
        {
            m_MoveTime = p_ArrivalTime;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_kj_flaming_detonation();
        }
};

/// Armageddon Rain - 234295
/// Armageddon Hail - 240911
class areatrigger_kj_armageddon_rain_hail : public AreaTriggerEntityScript
{
    public:
        areatrigger_kj_armageddon_rain_hail() : AreaTriggerEntityScript("areatrigger_kj_armageddon_rain_hail") { }

        std::set<uint64> m_Targets;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return false;

            m_Targets.insert(p_Target->GetGUID());
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return false;

            m_Targets.erase(p_Target->GetGUID());

            /// If a player is still soaking the Blossom, don't reapply the visual
            if (!m_Targets.empty())
                return true;

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_kj_armageddon_rain_hail();
        }
};

#ifndef __clang_analyzer__
void AddSC_tos_boss_kil_jaeden()
{
    /// Boss
    new boss_tos_kil_jaeden();

    /// Creatures
    new npc_kj_armageddon_stalker();
    new npc_kj_erupting_reflection();
    new npc_kj_shadowsoul();
    new npc_kj_illidan_stormrage();
    new npc_kj_demonic_obelisk();
    new npc_kj_nether_rift();
    new npc_kj_wailing_reflection();
    new npc_kj_flaming_orb();
    new npc_kj_hopeless_reflection();

    /// Spells
    new spell_kj_shadow_reflection_erupting();
    new spell_kj_illidans_sightless_gaze();
    new spell_kj_deceivers_veil_periodic();
    new spell_kj_gravity_squeeze();
    new spell_kj_shadow_reflection_wailing();
    new spell_kj_lingering_wail();
    new spell_kj_wailing_grasp();
    new spell_kj_lingering_eruption();
    new spell_kj_shadow_reflection_hopeless();

    /// AreaTriggers
    new areatrigger_kj_flaming_orb();
    new areatrigger_kj_flaming_detonation();
    new areatrigger_kj_armageddon_rain_hail();
}
#endif
