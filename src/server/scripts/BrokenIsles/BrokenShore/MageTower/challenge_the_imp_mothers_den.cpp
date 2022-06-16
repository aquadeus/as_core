////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

/// Agatha - 115638
class boss_agatha : public CreatureScript
{
    public:
        boss_agatha() : CreatureScript("boss_agatha") { }

        enum eSpells
        {
            SpellSummonImpHorde     = 242995,
            SpellSummonImpHorde2    = 242998,
            SpellSummonImpServant1  = 230943,
            SpellSummonImpServant2  = 230944,
            SpellSummonUmbralImp    = 242999,
            SpellImpDummyAura       = 230983,
            SpellImpSummons         = 235119,
            SpellImpEnergyAura      = 236136,

            SpellAgathasVengeance   = 235140,
            SpellAgathasFuryAura    = 242878,
            SpellAgathasFuryDummy   = 242885,
            SpellAgathasFuryMissile = 242879,

            SpellDemonBarrier       = 229900,

            SpellShadowStab         = 235242,

            SpellDarkFuryChannel    = 243114,
            SpellDarkFuryAbsorb     = 243113,
            SpellDarkFuryAura       = 243111,

            SpellShadowShieldAura   = 243027,

            SpellTranslocate        = 242987,

            SpellFelLavaBlast       = 235259
        };

        enum eEvents
        {
            EventShadowStab = 1,
            EventAgathasVengeance,
            EventAgathasFury,
            EventSummonImpHorde,
            EventDarkFury,
            EventImpServants,
            EventUmbralImps,
            EventTranslocate,
            EventFelLavaBlast,
            EventFelfireBoulder
        };

        enum eTalks
        {
            TalkIntro0,
            TalkIntro1,
            TalkIntro2,
            TalkAggro,
            TalkDeath,
            TalkImpKilled,
            TalkImpSummoned,
            TalkShadowStab,
            TalkImpServant
        };

        enum eVisuals
        {
            VisualIntroAnim = 595
        };

        enum eAction
        {
            ActionFullEnergy
        };

        struct boss_agathaAI : public BossAI
        {
            boss_agathaAI(Creature* p_Creature) : BossAI(p_Creature, 0) { }

            bool m_IntroDone = false;

            uint32 m_LastSummonedKilledTime = 0;
            uint32 m_LastSummonedTime = 0;
            uint32 m_LastShadowStabTime = 0;

            bool m_SecondPhase = false;

            int32 m_SecondPhasePct = 50;

            std::map<uint32, std::array<Position const, 3>> m_ServantImpSpawn =
            {
                {
                    eSpells::SpellSummonImpServant1,
                    {
                        {
                            { -381.806f, 7574.37f, 53.27603f, 1.176613f }, ///< First phase
                            { -360.885f, 7624.88f, 53.18720f, 1.176613f }, ///< Second phase - Back of the den
                            { -360.885f, 7624.88f, 53.18720f, 4.381043f }  ///< Second phase - Start of the den
                        }
                    }
                },
                {
                    eSpells::SpellSummonImpServant2,
                    {
                        {
                            { -362.576f, 7568.33f, 53.27593f, 0.940690f }, ///< First phase
                            { -348.855f, 7621.20f, 53.25793f, 1.357260f }, ///< Second phase - Back of the den
                            { -348.855f, 7621.20f, 53.25793f, 4.361409f }  ///< Second phase - Start of the den
                        }
                    }
                }
            };

            std::map<uint32, std::array<G3D::Vector3 const, 3>> m_ServantImpMoves =
            {
                {
                    eCreatures::CreatureImpServant1,
                    {
                        {
                            { -365.2584f, 7610.229f, 53.39096f },
                            { -345.1169f, 7671.977f, 63.00195f },
                            { -373.9671f, 7593.838f, 53.15142f }
                        }
                    }
                },
                {
                    eCreatures::CreatureImpServant2,
                    {
                        {
                            { -351.2113f, 7605.969f, 53.33302f },
                            { -332.8309f, 7668.979f, 62.98038f },
                            { -359.2703f, 7588.761f, 53.19357f }
                        }
                    }
                }
            };

            uint8 m_UmbralImpIDx = 0;

            std::array<uint8, 3> m_UmbralImpsCount =
            {
                { 3, 1, 2 }
            };

            uint8 m_TranslocateIDx = 0;

            std::array<Position const, 2> m_TranslocatePos =
            {
                {
                    { -336.25f, 7682.59f, 63.31f, 4.40f },
                    { -370.21f, 7580.14f, 53.19f, 1.25f }
                }
            };

            std::array<Position const, 2> m_FelfireBoulderPos =
            {
                {
                    { -328.79f, 7682.95f, 90.0f, 4.40f },
                    { -340.86f, 7688.00f, 90.0f, 4.50f }
                }
            };

            std::array<Position const, 2> m_FelfireBoulderRelativePos =
            {
                {
                    { 7.460f, 0.36f, 26.69f, 0.0f },
                    { -4.61f, 5.41f, 26.69f, 0.0f }
                }
            };

            EventMap m_Events;

            void Reset() override
            {
                _Reset();

                m_LastSummonedKilledTime = 0;
                m_LastSummonedTime = 0;
                m_LastShadowStabTime = 0;

                m_SecondPhase = false;

                m_UmbralImpIDx = 0;

                m_TranslocateIDx = 0;

                SetCombatMovement(false);

                events.Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->setPowerType(Powers::POWER_ENERGY);
                me->SetPower(Powers::POWER_ENERGY, 0);

                if (m_IntroDone)
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    /// Health values seem to be different for each class
                    if (instance)
                    {
                        switch (instance->GetData(eData::DataPlayerClass))
                        {
                            case Classes::CLASS_WARRIOR:
                            {
                                me->SetMaxHealth(124567849);
                                me->SetFullHealth();
                                break;
                            }
                            case Classes::CLASS_SHAMAN:
                            case Classes::CLASS_MAGE:
                            {
                                me->SetMaxHealth(93430000);
                                me->SetFullHealth();
                                break;
                            }
                            case Classes::CLASS_ROGUE:
                            {
                                me->SetMaxHealth(105104136);
                                me->SetFullHealth();
                                break;
                            }
                            case Classes::CLASS_DRUID:
                            case Classes::CLASS_DEATH_KNIGHT:
                            {
                                me->SetMaxHealth(89533000);
                                me->SetFullHealth();
                                break;
                            }
                            default:
                                break;
                        }
                    }

                    me->PlayOneShotAnimKitId(eVisuals::VisualIntroAnim);

                    Talk(eTalks::TalkIntro0);

                    AddTimedDelayedOperation(21 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->PlayOneShotAnimKitId(eVisuals::VisualIntroAnim);

                        Talk(eTalks::TalkIntro1);

                        AddTimedDelayedOperation(16 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->PlayOneShotAnimKitId(eVisuals::VisualIntroAnim);

                            Talk(eTalks::TalkIntro2);

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                m_IntroDone = true;

                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                            });
                        });
                    });
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                m_LastSummonedTime = uint32(time(nullptr));

                for (uint8 l_I = 0; l_I < 3; ++l_I)
                    DoCast(me, eSpells::SpellSummonImpHorde, true);

                DoCast(me, eSpells::SpellDemonBarrier, true);

                Talk(eTalks::TalkAggro);

                events.ScheduleEvent(eEvents::EventShadowStab, 1);
                events.ScheduleEvent(eEvents::EventAgathasVengeance, 19 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventAgathasFury, 30 * TimeConstants::IN_MILLISECONDS);

                m_Events.ScheduleEvent(eEvents::EventSummonImpHorde, 20 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventImpServants, 11 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventUmbralImps, 73 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                Talk(eTalks::TalkDeath);

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

                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataAgathaAsset, 0, 0, l_Player, l_Player);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                if (p_Summon->GetEntry() == eCreatures::CreatureFelfireBoulder)
                    return;

                if (!m_LastSummonedTime || ((m_LastSummonedTime + 15) <= uint32(time(nullptr))))
                {
                    m_LastSummonedTime = uint32(time(nullptr));

                    Talk(eTalks::TalkImpSummoned);
                }

                if (Unit* l_Victim = me->getVictim())
                {
                    if (p_Summon->GetEntry() == eCreatures::CreatureFumingImp)
                    {
                        p_Summon->Attack(l_Victim, true);

                        p_Summon->GetMotionMaster()->Clear();
                        p_Summon->GetMotionMaster()->MoveChase(l_Victim);
                    }
                    else
                        p_Summon->Attack(l_Victim, false);
                }

                p_Summon->CastSpell(p_Summon, eSpells::SpellImpDummyAura, true);
                p_Summon->CastSpell(p_Summon, eSpells::SpellImpSummons, true);

                auto const& l_Iter = m_ServantImpMoves.find(p_Summon->GetEntry());
                if (l_Iter != m_ServantImpMoves.end())
                {
                    uint64 l_Guid = p_Summon->GetGUID();
                    AddTimedDelayedOperation(1, [this, l_Guid, l_Iter]() -> void
                    {
                        if (Creature* l_Creature = Creature::GetCreature(*me, l_Guid))
                        {
                            l_Creature->GetMotionMaster()->Clear();

                            if (!m_SecondPhase)
                                l_Creature->GetMotionMaster()->MovePoint(1, l_Iter->second[0]);
                            else
                                l_Creature->GetMotionMaster()->MovePoint(1, l_Iter->second[m_TranslocateIDx + 1]);
                        }
                    });
                }
            }

            void SummonedCreatureDies(Creature* p_Creature, Unit* p_Killer) override
            {
                /// Prevent spamming with Fuming Imps suicide
                if (p_Creature->GetGUID() == p_Killer->GetGUID())
                    return;

                if (!m_LastSummonedKilledTime || ((m_LastSummonedKilledTime + 15) <= uint32(time(nullptr))))
                {
                    m_LastSummonedKilledTime = uint32(time(nullptr));

                    Talk(eTalks::TalkImpKilled);
                }

                if (p_Creature->GetEntry() == eCreatures::CreatureUmbralImp)
                    me->RemoveAura(eSpells::SpellShadowShieldAura, p_Creature->GetGUID());
            }

            void EnterEvadeMode() override
            {
                /// Only evades if player is dead
                if (instance && !instance->IsWipe())
                    return;

                m_Events.Reset();

                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                me->InterruptNonMeleeSpells(true);

                me->DespawnOrUnsummon();
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellAgathasFuryDummy:
                    {
                        DoCast(p_Target, eSpells::SpellAgathasFuryMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!m_SecondPhase && me->HealthBelowPctDamaged(m_SecondPhasePct, p_Damage))
                {
                    m_SecondPhase = true;

                    me->InterruptNonMeleeSpells(false);

                    Position l_TeleportPos = m_TranslocatePos[m_TranslocateIDx++];

                    DoCast(l_TeleportPos, eSpells::SpellTranslocate);

                    events.ScheduleEvent(eEvents::EventFelLavaBlast, 38 * TimeConstants::IN_MILLISECONDS);

                    m_Events.ScheduleEvent(eEvents::EventTranslocate, 51 * TimeConstants::IN_MILLISECONDS);
                    m_Events.ScheduleEvent(eEvents::EventFelfireBoulder, urand(10 * TimeConstants::IN_MILLISECONDS, 15 * TimeConstants::IN_MILLISECONDS));

                    uint32 l_Time = 2 * TimeConstants::IN_MILLISECONDS;

                    for (Position const& l_RelativePos : m_FelfireBoulderRelativePos)
                    {
                        AddTimedDelayedOperation(l_Time, [this, l_RelativePos, l_TeleportPos]() -> void
                        {
                            Position l_Pos = l_TeleportPos;

                            l_Pos.m_positionX += l_RelativePos.m_positionX;
                            l_Pos.m_positionY += l_RelativePos.m_positionY;
                            l_Pos.m_positionZ += l_RelativePos.m_positionZ;
                            l_Pos.m_orientation += l_RelativePos.m_orientation;

                            me->SummonCreature(eCreatures::CreatureFelfireBoulder, l_Pos);
                        });

                        l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eAction::ActionFullEnergy:
                    {
                        if (!events.HasEvent(eEvents::EventDarkFury))
                            events.ScheduleEvent(eEvents::EventDarkFury, 1);

                        break;
                    }
                    default:
                        break;
                }
            }

            void NonInterruptCast(Unit* /*p_Caster*/, uint32 p_SpellID, uint32 /*p_SchoolMask*/) override
            {
                /// Dark Fury can be interrupted if absorb is gone
                if (p_SpellID == eSpells::SpellDarkFuryChannel && !me->HasAura(eSpells::SpellDarkFuryAbsorb))
                    me->InterruptNonMeleeSpells(false);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellDarkFuryChannel)
                    me->RemoveAura(eSpells::SpellDarkFuryAura);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventTranslocate:
                    {
                        me->InterruptNonMeleeSpells(false);

                        DoCast(m_TranslocatePos[m_TranslocateIDx++], eSpells::SpellTranslocate);

                        if (m_TranslocateIDx >= 2)
                            m_TranslocateIDx = 0;

                        m_Events.ScheduleEvent(eEvents::EventTranslocate, urand(45 * TimeConstants::IN_MILLISECONDS, 55 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventFelfireBoulder:
                    {
                        uint32 l_Time = 1;

                        for (Position const& l_RelativePos : m_FelfireBoulderRelativePos)
                        {
                            AddTimedDelayedOperation(l_Time, [this, l_RelativePos]() -> void
                            {
                                Position l_Pos;

                                if (m_TranslocateIDx == 0)
                                    l_Pos = m_TranslocatePos[1];
                                else
                                    l_Pos = m_TranslocatePos[0];

                                l_Pos.m_positionX   += l_RelativePos.m_positionX;
                                l_Pos.m_positionY   += l_RelativePos.m_positionY;
                                l_Pos.m_positionZ   += l_RelativePos.m_positionZ;
                                l_Pos.m_orientation += l_RelativePos.m_orientation;

                                me->SummonCreature(eCreatures::CreatureFelfireBoulder, l_Pos);
                            });

                            l_Time += 2 * TimeConstants::IN_MILLISECONDS;
                        }

                        m_Events.ScheduleEvent(eEvents::EventFelfireBoulder, urand(8 * TimeConstants::IN_MILLISECONDS, 12 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventSummonImpHorde:
                    {
                        DoCast(me, eSpells::SpellSummonImpHorde2, true);

                        m_Events.ScheduleEvent(eEvents::EventSummonImpHorde, 40 * TimeConstants::IN_MILLISECONDS + 400);
                        break;
                    }
                    case eEvents::EventImpServants:
                    {
                        std::array<uint32, 2> l_Spells = { { eSpells::SpellSummonImpServant1, eSpells::SpellSummonImpServant2 } };

                        for (uint32 const& l_Spell : l_Spells)
                        {
                            auto const& l_Iter = m_ServantImpSpawn.find(l_Spell);
                            if (l_Iter != m_ServantImpSpawn.end())
                            {
                                if (!m_SecondPhase)
                                    DoCast(l_Iter->second[0], l_Spell, true);
                                else
                                    DoCast(l_Iter->second[m_TranslocateIDx + 1], l_Spell, true);
                            }
                        }

                        Talk(eTalks::TalkImpServant);

                        m_Events.ScheduleEvent(eEvents::EventImpServants, 45 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventUmbralImps:
                    {
                        uint8 l_Count = m_UmbralImpsCount[m_UmbralImpIDx++];

                        if (m_UmbralImpIDx >= 3)
                            m_UmbralImpIDx = 0;

                        for (uint8 l_I = 0; l_I < l_Count; ++l_I)
                            DoCast(me, eSpells::SpellSummonUmbralImp, true);

                        m_Events.ScheduleEvent(eEvents::EventUmbralImps, 71 * TimeConstants::IN_MILLISECONDS + 400);
                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShadowStab:
                    {
                        if (Unit* l_Target = me->getVictim())
                        {
                            if (l_Target->IsWithinMeleeRange(me))
                            {
                                events.ScheduleEvent(eEvents::EventShadowStab, 1);
                                break;
                            }
                            else
                                DoCastVictim(eSpells::SpellShadowStab);
                        }

                        if (!m_LastShadowStabTime || ((m_LastShadowStabTime + 15) <= uint32(time(nullptr))))
                        {
                            m_LastShadowStabTime = uint32(time(nullptr));

                            Talk(eTalks::TalkShadowStab);
                        }

                        events.ScheduleEvent(eEvents::EventShadowStab, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAgathasVengeance:
                    {
                        DoCast(me, eSpells::SpellAgathasVengeance, true);

                        events.ScheduleEvent(eEvents::EventAgathasVengeance, 36 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAgathasFury:
                    {
                        DoCast(me, eSpells::SpellAgathasFuryAura, true);

                        events.ScheduleEvent(eEvents::EventAgathasFury, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventDarkFury:
                    {
                        DoCast(me, eSpells::SpellDarkFuryChannel);
                        break;
                    }
                    case eEvents::EventFelLavaBlast:
                    {
                        DoCastVictim(eSpells::SpellFelLavaBlast, true);

                        events.ScheduleEvent(eEvents::EventFelLavaBlast, urand(7 * TimeConstants::IN_MILLISECONDS, 13 * TimeConstants::IN_MILLISECONDS));
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
            return new boss_agathaAI(p_Creature);
        }
};

/// Fuming Imp - 115640
class npc_agatha_fuming_imp : public CreatureScript
{
    public:
        npc_agatha_fuming_imp() : CreatureScript("npc_agatha_fuming_imp") { }

        enum eSpells
        {
            SpellImpEnergyAura  = 236136,

            SpellPlagueZone     = 236163,
            SpellPlagueZoneAT   = 243095
        };

        struct npc_agatha_fuming_impAI : public ScriptedAI
        {
            npc_agatha_fuming_impAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellImpEnergyAura, true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance && p_SpellInfo->Id == eSpells::SpellPlagueZone)
                {
                    if (Creature* l_Agatha = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::CreatureAgatha)))
                        l_Agatha->CastSpell(me->GetPosition(), eSpells::SpellPlagueZoneAT, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (Unit* l_Victim = me->getVictim())
                {
                    if (me->IsWithinMeleeRange(l_Victim))
                    {
                        DoCast(me, eSpells::SpellPlagueZone);
                        return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_agatha_fuming_impAI(p_Creature);
        }
};

/// Imp Servant - 115643
/// Imp Servant - 115719
class npc_agatha_imp_servant : public CreatureScript
{
    public:
        npc_agatha_imp_servant() : CreatureScript("npc_agatha_imp_servant") { }

        enum eSpells
        {
            SpellFunnelEnergy = 229928
        };

        struct npc_agatha_imp_servantAI : public ScriptedAI
        {
            npc_agatha_imp_servantAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->DisableEvadeMode();

                me->SetInCombatWithZone();

                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE || p_ID != 1)
                    return;

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    me->SetFacingTo(me->GetAngle(l_Owner));

                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        DoCast(me, eSpells::SpellFunnelEnergy);
                    });
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellFunnelEnergy)
                {
                    AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->StopAttack();

                        DoCast(me, eSpells::SpellFunnelEnergy);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    if (!me->IsMoving() && me->GetDistance(l_Owner) > 10.0f)
                    {
                        me->InterruptNonMeleeSpells(true);

                        ClearDelayedOperations();

                        me->StopAttack();

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        float l_Angle = me->GetAngle(l_Owner);
                        Position l_Pos = me->GetPosition();

                        l_Pos.SimplePosXYRelocationByAngle(l_Pos, me->GetDistance(l_Owner) - 5.0f, l_Angle, true);

                        l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ, true, DEFAULT_HEIGHT_SEARCH, l_Angle > 1.0f && l_Angle < 4.0f);

                        AddTimedDelayedOperation(1, [this, l_Pos]() -> void
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(1, l_Pos);
                        });
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_agatha_imp_servantAI(p_Creature);
        }
};

/// Felfire Boulder - 121109
class npc_agatha_felfire_boulder : public CreatureScript
{
    public:
        npc_agatha_felfire_boulder() : CreatureScript("npc_agatha_felfire_boulder") { }

        enum eSpells
        {
            SpellFelfireBoulderVisual   = 241473,
            SpellFelfireBoulderAura     = 241477,

            SpellFelfireBoulderFallAoE  = 241475
        };

        struct npc_agatha_felfire_boulderAI : public ScriptedAI
        {
            npc_agatha_felfire_boulderAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool CanHaveThreatList() const override
            {
                return false;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                SetFlyMode(true);

                DoCast(me, eSpells::SpellFelfireBoulderVisual, true);
                DoCast(me, eSpells::SpellFelfireBoulderAura, true);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    SetFlyMode(false);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveFall(1);
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID != 1)
                    return;

                switch (p_Type)
                {
                    case MovementGeneratorType::EFFECT_MOTION_TYPE:
                    {
                        DoCast(me, eSpells::SpellFelfireBoulderFallAoE, true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            Position l_Pos = me->GetPosition();

                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, 100.0f, me->GetOrientation(), true);

                            l_Pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ, true, DEFAULT_HEIGHT_SEARCH, l_Pos.m_orientation > 1.0f && l_Pos.m_orientation < 4.0f);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(1, l_Pos);
                        });

                        break;
                    }
                    case MovementGeneratorType::POINT_MOTION_TYPE:
                    {
                        me->RemoveAllAreasTrigger();
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_agatha_felfire_boulderAI(p_Creature);
        }
};

/// Umbral Imp - 115642
/// Smoldering Imp - 115641
class npc_agatha_umbral_imp : public CreatureScript
{
    public:
        npc_agatha_umbral_imp() : CreatureScript("npc_agatha_umbral_imp") { }

        struct npc_agatha_umbral_impAI : public LegionCombatAI
        {
            npc_agatha_umbral_impAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            bool CanChase() override
            {
                return false;
            }

            bool CanAutoAttack() override
            {
                return false;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_agatha_umbral_impAI(p_Creature);
        }
};

/// Demon Barrier - 229900
class spell_agatha_demon_barrier_energizer : public SpellScriptLoader
{
    public:
        spell_agatha_demon_barrier_energizer() : SpellScriptLoader("spell_agatha_demon_barrier_energizer") { }

        enum eAction
        {
            ActionFullEnergy
        };

        class spell_agatha_demon_barrier_energizer_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_agatha_demon_barrier_energizer_Aurascript);

            int32 m_PowerGain = 2;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetTarget())
                    return;

                Creature* l_Target = GetTarget()->ToCreature();
                if (!l_Target || !l_Target->IsAIEnabled)
                    return;

                int32 l_Power = l_Target->GetPower(Powers::POWER_ENERGY);

                l_Target->EnergizeBySpell(l_Target, GetSpellInfo()->Id, m_PowerGain, Powers::POWER_ENERGY);

                if ((l_Power + m_PowerGain) >= l_Target->GetMaxPower(Powers::POWER_ENERGY))
                    l_Target->AI()->DoAction(eAction::ActionFullEnergy);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_agatha_demon_barrier_energizer_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_agatha_demon_barrier_energizer_Aurascript();
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_the_imp_mothers_den()
{
    new boss_agatha();

    new npc_agatha_fuming_imp();
    new npc_agatha_imp_servant();
    new npc_agatha_felfire_boulder();
    new npc_agatha_umbral_imp();

    new spell_agatha_demon_barrier_energizer();
}
#endif
