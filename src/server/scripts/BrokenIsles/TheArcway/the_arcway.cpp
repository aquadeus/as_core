////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_arcway.hpp"

enum eSpells
{
    FlyBallVisual   = 225673,
    BunnyArrival    = 223264,

    /// Acid Bile
    ConsumeJump     = 202175,
    ConsumeAT       = 202223,
    Corrision       = 202156,
    Corrosive       = 202158,

    /// Wyrmtongue Scavanger
    ThrowVial       = 211994,
    ThrowSkull      = 211975,
    ThrowCrate      = 211959,
    ThrowBlanket    = 211995,
    ThrowBook       = 211958,
    ThrowBoot       = 212008,
    ThrowCake       = 211999,
    ThrowCandle     = 211956,

    /// Wrathguard Felblade
    FelStrike       = 211744,
    SearingWound    = 211756,
    Felstep         = 211754,

    /// Mana Wyrm
    NetherSpike     = 211476,
    CelerityZone    = 211037,
    CelerityZoneBuff = 211064,

    /// Eye of the Beast
    OpticBlast      = 211777,

    /// Withered Manawraith
    CosmeticChannel = 210069,
    SiphonEssence   = 210684,
    CollapsingRift  = 210688,

    /// Nightborne Spellsword
    ArcaneBarrier   = 210868,
    NetherWound     = 211000,

    /// Eredar Chaosbringer
    ChaosBolt       = 211571,
    BrandOfLegion   = 211632,
    DemonicAscens   = 226285,
    DemonForm       = 226383,
    HoundForm       = 226362,
    FlameBreath     = 226420,
    GroundSlam      = 226423,
    FelCinders      = 226495
};

enum eEvents
{
    /// Acid Bile - 98728
    EventConsume            = 1,

    /// Wyrmtongue Scavanger
    EventThrowSmth          = 2,

    /// Wrathguard Felblade

    EventFelStrike          = 3,
    EventSearingWound       = 4,

    /// Withered Fiend - 98733
    EventArcanicBane        = 5,
    EventUnstableFlux       = 6,

    /// Astral Spark - 106546
    EventDestructiveWake    = 7,

    /// Mana Wyrm - 102351
    EventNetherSpike        = 8,
    EventCelerityZone       = 9,

    /// Eye of the Beast - 106757
    EventOpticBlast         = 10,
    EventDespawn            = 11,

    /// Withered Manawraith - 105952
    EventCosmetic           = 12,
    EventSiphonEssence      = 13,
    EventCollapsingRift     = 14,

    /// Nightborne Spellsword - 105921
    EventArcanceBarrier     = 15,
    EventNetherWound        = 16,

    /// Eredar Chaosbringer - 105617
    EventChaosBolt          = 17,
    EventBrandOfTheLegion   = 18,
    EventDemonicAscension   = 19,
    EventFlameBreath        = 20,
    EventGroundSlam         = 21,
    EventFelCinders         = 22
};

std::vector<G3D::Vector3> l_FlyPath1 =
{
    { 1145.887f, 4182.940f, 98.30918f },
    { 1146.064f, 4182.837f, 82.40360f },
    { 1146.135f, 4182.775f, 45.28102f },
    { 1146.144f, 4182.792f, 24.29016f }
};

std::vector<G3D::Vector3> l_FlyPath2 =
{
    { 1075.508f, 4223.336f, 95.910f },
    { 1075.508f, 4223.336f, 82.40360f },
    { 1075.508f, 4223.336f, 45.28102f },
    { 1075.508f, 4223.336f, 24.29016f }
};

std::vector<G3D::Vector3> l_FlyPath3 =
{
    { 1028.79f, 3882.757f, 84.759f },
    { 1028.79f, 3882.757f, 70.759f },
    { 1028.79f, 3882.757f, 39.759f },
    { 1028.79f, 3882.757f, 11.759f }
};

std::vector<G3D::Vector3> l_FlyPath4 =
{
    { 1146.144f, 4182.792f, 24.29016f },
    { 1146.135f, 4182.775f, 45.28102f },
    { 1146.064f, 4182.837f, 82.40360f },
    { 1145.887f, 4182.940f, 98.30918f }
};

std::vector<G3D::Vector3> l_FlyPath5 =
{
    { 1075.508f, 4223.336f, 24.29016f },
    { 1075.508f, 4223.336f, 45.28102f },
    { 1075.508f, 4223.336f, 82.40360f },
    { 1075.508f, 4223.336f, 95.91000f }
};

std::vector<G3D::Vector3> l_FlyPath6 =
{
    { 1028.79f, 3882.757f, 11.759f },
    { 1028.79f, 3882.757f, 39.759f },
    { 1028.79f, 3882.757f, 70.759f },
    { 1028.79f, 3882.757f, 84.759f }
};

std::set<uint32> m_GuidsMovingUp;
std::set<uint32> m_GuidsMovingDown;

class npc_arcway_teleport_bunny : public CreatureScript
{
    public:
        npc_arcway_teleport_bunny() : CreatureScript("npc_arcway_teleport_bunny") { }

        struct npc_arcway_teleport_bunnyAI : public ScriptedAI
        {
            npc_arcway_teleport_bunnyAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetDisplayId(eDisplayIDs::INVISIBLE_MODEL);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who == nullptr || !p_Who->IsPlayer())
                    return;

                if (Player* l_Player = p_Who->ToPlayer())
                {
                    uint64 l_GUID = l_Player->GetGUID();
                    uint32 l_AreaID = me->GetAreaId(true);

                    if (l_AreaID != 8353 && l_AreaID != 8355) ///< Concourse of Destiny (way to CoS)
                    {
                        Position l_PosUpRight   = { 1146.549f, 4182.269f, 95.909f, 2.565f };
                        Position l_PosDownRight = { 1146.630f, 4182.250f, 23.344f, 2.628f };
                        Position l_PosUpLeft    = { 1075.508f, 4223.336f, 95.910f, 5.754f };
                        Position l_PosDownLeft  = { 1075.508f, 4223.336f, 24.076f, 5.754f };

                        if (me->GetDistance(l_Player) < 3.0f)
                        {
                            if (me->IsNearPosition(&l_PosUpRight, 5.0f) || me->IsNearPosition(&l_PosUpLeft, 5.0f))
                            {
                                if (m_GuidsMovingUp.find(l_GUID) != m_GuidsMovingUp.end())
                                {
                                    me->CastSpell(me, eSpells::BunnyArrival, false);
                                    l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.0f);
                                    l_Player->SetDisplayId(p_Who->GetNativeDisplayId());
                                    l_Player->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                    l_Player->RemoveAura(eSpells::FlyBallVisual);
                                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this, l_GUID]() -> void { m_GuidsMovingUp.erase(l_GUID); });
                                }
                                else
                                {
                                    if (me->IsNearPosition(&l_PosUpRight, 5.0f))
                                    {
                                        me->CastSpell(me, eSpells::BunnyArrival, false);
                                        l_Player->SetDisplayId(eDisplayIDs::TELEPORT_MODEL);
                                        l_Player->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                        l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.0f);
                                        l_Player->GetMotionMaster()->MoveSmoothFlyPath(0, l_FlyPath1.data(), l_FlyPath1.size());
                                        m_GuidsMovingDown.insert(l_GUID);
                                    }

                                    if (me->IsNearPosition(&l_PosUpLeft, 5.0f))
                                    {
                                        me->CastSpell(me, eSpells::BunnyArrival, false);
                                        l_Player->SetDisplayId(eDisplayIDs::TELEPORT_MODEL);
                                        l_Player->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                        l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.0f);
                                        l_Player->GetMotionMaster()->MoveSmoothFlyPath(0, l_FlyPath2.data(), l_FlyPath2.size());
                                        m_GuidsMovingDown.insert(l_GUID);
                                    }
                                }
                            }

                            if (me->IsNearPosition(&l_PosDownRight, 3.0f) || me->IsNearPosition(&l_PosDownLeft, 3.0f))
                            {
                                if (m_GuidsMovingDown.find(l_GUID) != m_GuidsMovingDown.end())
                                {
                                    me->CastSpell(me, eSpells::BunnyArrival, false);
                                    l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.0f);
                                    l_Player->SetDisplayId(p_Who->GetNativeDisplayId());
                                    l_Player->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                    l_Player->RemoveAura(eSpells::FlyBallVisual);
                                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this, l_GUID]() -> void { m_GuidsMovingDown.erase(l_GUID); });
                                }
                                else
                                {
                                    if (me->IsNearPosition(&l_PosDownRight, 3.0f) && l_Player->GetDisplayId() != eDisplayIDs::TELEPORT_MODEL)
                                    {
                                        me->CastSpell(me, eSpells::BunnyArrival, false);
                                        l_Player->SetDisplayId(eDisplayIDs::TELEPORT_MODEL);
                                        l_Player->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                        l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.0f);
                                        l_Player->GetMotionMaster()->MoveSmoothFlyPath(0, l_FlyPath4.data(), l_FlyPath4.size());
                                        m_GuidsMovingUp.insert(l_GUID);
                                    }

                                    if (me->IsNearPosition(&l_PosDownLeft, 3.0f) && l_Player->GetDisplayId() != eDisplayIDs::TELEPORT_MODEL)
                                    {
                                        me->CastSpell(me, eSpells::BunnyArrival, false);
                                        l_Player->SetDisplayId(eDisplayIDs::TELEPORT_MODEL);
                                        l_Player->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                        l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.0f);
                                        l_Player->GetMotionMaster()->MoveSmoothFlyPath(0, l_FlyPath5.data(), l_FlyPath5.size());
                                        m_GuidsMovingUp.insert(l_GUID);
                                    }
                                }
                            }
                        }
                    }
                    else if (me->GetAreaId(true) == 8353) ///< Going to Court of Stars
                    {
                        Position l_CoSTP = { 1028.79f, 3882.757f, 84.759f, 0.0f };

                        if (l_Player->IsNearPosition(&l_CoSTP, 1.0f))
                        {
                            if (m_GuidsMovingUp.find(l_GUID) != m_GuidsMovingUp.end())
                            {
                                me->CastSpell(me, eSpells::BunnyArrival, false);
                                l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.0f);
                                l_Player->SetDisplayId(p_Who->GetNativeDisplayId());
                                l_Player->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                l_Player->RemoveAura(eSpells::FlyBallVisual);
                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this, l_GUID]() -> void { m_GuidsMovingUp.erase(l_GUID); });
                            }
                            else
                            {
                                me->CastSpell(me, eSpells::BunnyArrival, false);
                                l_Player->SetDisplayId(eDisplayIDs::TELEPORT_MODEL);
                                l_Player->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.0f);
                                l_Player->GetMotionMaster()->MoveSmoothFlyPath(0, l_FlyPath3.data(), l_FlyPath3.size());
                                m_GuidsMovingUp.insert(l_GUID);
                            }
                        }
                    }
                    else if (me->GetAreaId(true) == 8355) ///< The Aetherium, Court of Stars entrance place
                    {
                        Position l_CoSTP = { 1028.79f, 3882.757f, 11.759f, 0.0f };

                        if (l_Player->IsNearPosition(me, 3.0f))
                        {
                            if (m_GuidsMovingDown.find(l_GUID) != m_GuidsMovingDown.end())
                            {
                                me->CastSpell(me, eSpells::BunnyArrival, false);
                                l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.0f);
                                l_Player->SetDisplayId(p_Who->GetNativeDisplayId());
                                l_Player->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                l_Player->RemoveAura(eSpells::FlyBallVisual);
                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this, l_GUID]() -> void { m_GuidsMovingDown.erase(l_GUID); });
                            }
                            else
                            {
                                me->CastSpell(me, eSpells::BunnyArrival, false);
                                l_Player->SetDisplayId(eDisplayIDs::TELEPORT_MODEL);
                                l_Player->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);
                                l_Player->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.0f);
                                l_Player->GetMotionMaster()->MoveSmoothFlyPath(0, l_FlyPath3.data(), l_FlyPath3.size());
                                m_GuidsMovingDown.insert(l_GUID);
                            }
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arcway_teleport_bunnyAI(p_Creature);
        }
};

/// Withered Manawraith - 105952
class npc_arcway_withered_manawraith : public CreatureScript
{
    public:
        npc_arcway_withered_manawraith() : CreatureScript("npc_arcway_withered_manawraith") { }

        struct npc_arcway_withered_manawraithAI : public ScriptedAI
        {
            npc_arcway_withered_manawraithAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_FightStarted = false;
            }

            bool m_OutOfCombat;
            bool m_FightStarted;

            enum eAnimKits
            {
                AnimKitFight    = 2758
            };

            void Reset() override
            {
                events.Reset();
                Position l_Pos = { 3163.143f, 4859.929f, 617.766f, 1.616f };
                Position l_Pos2 = { 3182.776f, 4900.021f, 618.173f, 5.870f };
                if (me->IsNearPosition(&l_Pos, 15.0f) || me->IsNearPosition(&l_Pos2, 15.0f))
                    me->SetAIAnimKitId(eAnimKits::AnimKitFight);

                events.ScheduleEvent(eEvents::EventCosmetic, 6000);
                m_OutOfCombat = true;
            }

            void OnTaunt(Unit* p_Taunter) override
            {
                if (!p_Taunter->IsPlayer())
                    return;

                EnterCombat(p_Taunter);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (m_FightStarted)
                    return;

                m_FightStarted = true;
                m_OutOfCombat = false;
                me->SetAIAnimKitId(0);
                events.ScheduleEvent(eEvents::EventSiphonEssence, 15000);
                events.ScheduleEvent(eEvents::EventCollapsingRift, 20000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_OutOfCombat)
                {
                    events.Update(p_Diff);
                    switch (events.ExecuteEvent())
                    {
                        case eEvents::EventCosmetic:
                            DoCast(eSpells::CosmeticChannel);
                            events.ScheduleEvent(eEvents::EventCosmetic, 6000);
                            break;
                        default:
                            break;
                    }
                    return;
                }

                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSiphonEssence:
                        DoCast(eSpells::SiphonEssence);
                        events.ScheduleEvent(eEvents::EventSiphonEssence, 15000);
                        break;
                    case eEvents::EventCollapsingRift:
                        DoCast(eSpells::CollapsingRift);
                        events.ScheduleEvent(eEvents::EventCollapsingRift, 20000);
                        break;
                    default:
                        break;
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arcway_withered_manawraithAI(p_Creature);
        }
};

/// Nightborne Spellsword - 105921
class npc_arcway_nightborne_spellsword : public CreatureScript
{
    public:
        npc_arcway_nightborne_spellsword() : CreatureScript("npc_arcway_nightborne_spellsword") { }

        struct npc_arcway_nightborne_spellswordAI : public ScriptedAI
        {
            npc_arcway_nightborne_spellswordAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            Position l_Pos;

            enum eAnimKits
            {
                AnimKitFight    = 2758
            };

            void Reset() override
            {
                events.Reset();
                Position l_Pos = { 3182.776f, 4900.021f, 618.173f, 5.870f };
                if (me->IsNearPosition(&l_Pos, 15.0f))
                    me->SetAIAnimKitId(eAnimKits::AnimKitFight);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventArcanceBarrier, 18000);
                events.ScheduleEvent(eEvents::EventNetherWound, 8000);
                events.ScheduleEvent(eEvents::EventCelerityZone, urand(10000, 30000));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArcanceBarrier:
                        DoCast(eSpells::ArcaneBarrier);
                        events.ScheduleEvent(eEvents::EventArcanceBarrier, 18000);
                        break;
                    case eEvents::EventNetherWound:
                        DoCast(eSpells::NetherWound);
                        events.ScheduleEvent(eEvents::EventNetherWound, 8000);
                        break;
                    case eEvents::EventCelerityZone:
                        me->GetRandomNearPosition(l_Pos, 30.0f);
                        DoCast(l_Pos, eSpells::CelerityZone, false);
                        break;
                    default:
                        break;
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arcway_nightborne_spellswordAI(p_Creature);
        }
};

/// Withered Fiend - 98733
class npc_arcway_withered_fiend : public CreatureScript
{
    public:
        npc_arcway_withered_fiend() : CreatureScript("npc_arcway_withered_fiend") { }

        struct npc_arcway_withered_fiendAI : public ScriptedAI
        {
            npc_arcway_withered_fiendAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eAminKits
            {
                AnimKitFight    = 2758
            };

            enum eSpells
            {
                ArcanicBane     = 210645,
                UnstableFlux    = 210662,
                WitheringVoid   = 226246
            };

            void Reset() override
            {
                events.Reset();
                Position l_Pos = { 3433.512f, 4807.061f, 590.930f, 3.390f };
                Position l_Pos2 = { 3163.143f, 4859.929f, 617.766f, 1.616f };
                Position l_Pos3 = { 3180.143f, 4897.929f, 617.766f, 1.616f };
                if (me->IsNearPosition(&l_Pos, 15.0f) || me->IsNearPosition(&l_Pos2, 15.0f) || me->IsNearPosition(&l_Pos3, 15.0f))
                    me->SetAIAnimKitId(eAminKits::AnimKitFight);
            }

            void EnterCombat(Unit* p_Who) override
            {
                me->SetAIAnimKitId(0);
                events.ScheduleEvent(eEvents::EventArcanicBane, urand(15, 18) * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventUnstableFlux, urand(30, 31) * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* p_Killer) override
            {
                DoCast(eSpells::WitheringVoid);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArcanicBane:
                    {
                        DoCast(eSpells::ArcanicBane);
                        events.ScheduleEvent(eEvents::EventArcanicBane, urand(12, 15) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventUnstableFlux:
                    {
                        DoCast(eSpells::UnstableFlux);
                        events.ScheduleEvent(eEvents::EventUnstableFlux, urand(25, 30) * TimeConstants::IN_MILLISECONDS);
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
            return new npc_arcway_withered_fiendAI(p_Creature);
        }
};

/// Acidic Bile - 98728
class npc_the_arcway_acidic_bile : public CreatureScript
{
    public:
        npc_the_arcway_acidic_bile() : CreatureScript("npc_the_arcway_acidic_bile") { }

        struct npc_the_arcway_acidic_bileAI : public ScriptedAI
        {
            npc_the_arcway_acidic_bileAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            enum eAminKits
            {
                AnimKitFight = 2758
            };

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_DamageType, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo || p_Victim == nullptr)
                    return;

                if (roll_chance_i(75) && !p_Victim->HasAura(eSpells::Corrision))
                    p_Victim->AddAura(eSpells::Corrision, p_Victim);
            }

            bool CheckTrash()
            {
                Creature* l_WitheredManaFiend = GetClosestCreatureWithEntry(me, 98733, 20.0f, true);

                if (l_WitheredManaFiend)
                    return false;

                return true;
            }

            void Reset() override
            {
                Position l_Pos = { 3434.770f, 4806.058f, 590.928f, 6.087f };

                m_Events.Reset();
                DoCast(me, eSpells::Corrosive, true);

                if (me->IsNearPosition(&l_Pos, 20.0f) && CheckTrash())
                    me->PlayOneShotAnimKitId(2758);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_Events.ScheduleEvent(eEvents::EventConsume, 15000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() || me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventConsume:
                    {
                        uint64 l_GUID = me->GetGUID();
                        Player* l_Target = SelectRangedTarget(true);
                        if (l_Target == nullptr)
                        {
                            m_Events.ScheduleEvent(eEvents::EventConsume, 10000);
                            break;
                        }

                        DoCast(l_Target, eSpells::ConsumeJump, false);
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [l_GUID]() -> void
                        {
                            Creature* l_Creature = sObjectAccessor->FindCreature(l_GUID);
                            if (l_Creature == nullptr || !l_Creature->IsAIEnabled)
                                return;

                            l_Creature->StopAttack();
                            l_Creature->SetReactState(REACT_PASSIVE);
                            l_Creature->AI()->DoCast(eSpells::ConsumeAT);

                            l_Creature->AI()->AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [l_GUID]() -> void
                            {
                                Creature* l_Creature = sObjectAccessor->FindCreature(l_GUID);
                                if (l_Creature == nullptr || !l_Creature->IsAIEnabled)
                                    return;

                                l_Creature->SetReactState(REACT_AGGRESSIVE);
                            });
                        });

                        m_Events.ScheduleEvent(eEvents::EventConsume, 10000);
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
            return new npc_the_arcway_acidic_bileAI(p_Creature);
        }
};

/// Wyrmtongue Scavanger - 101923
class npc_the_arcway_wyrmtongue_scavanger : public CreatureScript
{
    public:
        npc_the_arcway_wyrmtongue_scavanger() : CreatureScript("npc_the_arcway_wyrmtongue_scavanger") { }

        struct npc_the_arcway_wyrmtongue_scavangerAI : public ScriptedAI
        {
            npc_the_arcway_wyrmtongue_scavangerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_Events.ScheduleEvent(eEvents::EventThrowSmth, 1200);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() || me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventThrowSmth:
                    {
                        /// Create array with possible spells and select a random one from that array
                        std::array<uint32, 9> l_Spells = { { eSpells::ThrowBlanket, eSpells::ThrowBook, eSpells::ThrowBoot, eSpells::ThrowCake, eSpells::ThrowCake, eSpells::ThrowCandle, eSpells::ThrowCrate, eSpells::ThrowSkull, eSpells::ThrowVial } };
                        uint32 l_SpellID = l_Spells[urand(0, l_Spells.size() - 1)];

                        /// Select a random Target and cast the spell
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f, true);
                        if (l_Target == nullptr)
                            return;

                        DoCast(l_Target, l_SpellID, false);

                        m_Events.ScheduleEvent(eEvents::EventThrowSmth, 1250);
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
            return new npc_the_arcway_wyrmtongue_scavangerAI(p_Creature);
        }
};

/// Wrathguard Felblade - 98770
class npc_the_arcway_wrathguard_felblade : public CreatureScript
{
    public:
        npc_the_arcway_wrathguard_felblade() : CreatureScript("npc_the_arcway_wrathguard_felblade") { }

        struct npc_the_arcway_wrathguard_felbladeAI : public ScriptedAI
        {
            npc_the_arcway_wrathguard_felbladeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_Events.ScheduleEvent(eEvents::EventFelStrike, 15 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventSearingWound, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() || me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventFelStrike:
                    {
                        DoCastVictim(eSpells::FelStrike, false);
                        m_Events.ScheduleEvent(eEvents::EventFelStrike, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSearingWound:
                    {
                        /// Selects first a ranged Target, if not possible a random one
                        Player* l_Target = SelectRangedTarget(true);
                        if (l_Target == nullptr)
                        {
                            Unit* l_NewTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f, true);
                            if (l_NewTarget == nullptr)
                                return;

                            Position l_Destination = l_NewTarget->GetPosition();
                            DoCast(l_Destination, eSpells::Felstep, false);
                            DoCast(l_NewTarget, eSpells::SearingWound, false);
                            m_Events.ScheduleEvent(eEvents::EventSearingWound, urand(6, 10) * TimeConstants::IN_MILLISECONDS);
                            break;
                        }

                        Position l_Destination = l_Target->GetPosition();
                        DoCast(l_Destination, eSpells::Felstep, false);
                        DoCast(l_Target, eSpells::SearingWound, false);
                        m_Events.ScheduleEvent(eEvents::EventSearingWound, urand(6, 10) * TimeConstants::IN_MILLISECONDS);
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
            return new npc_the_arcway_wrathguard_felbladeAI(p_Creature);
        }
};

class npc_the_arcway_astral_spark : public CreatureScript
{
    public:
        npc_the_arcway_astral_spark() : CreatureScript("npc_the_arcway_astral_spark") { }

        struct npc_the_arcway_astral_sparkAI : public ScriptedAI
        {
            npc_the_arcway_astral_sparkAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            EventMap m_Events;

            enum eSpells
            {
                DestructiveWake = 211191
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                events.ScheduleEvent(eEvents::EventDestructiveWake, 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventDestructiveWake:
                        DoCast(eSpells::DestructiveWake);
                        break;
                    default:
                        break;
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_the_arcway_astral_sparkAI(p_Creature);
        }
};

class npc_arcway_mana_wyrm : public CreatureScript
{
    public:
        npc_arcway_mana_wyrm() : CreatureScript("npc_arcway_mana_wyrm") { }

        struct npc_arcway_mana_wyrmAI : public ScriptedAI
        {
            npc_arcway_mana_wyrmAI(Creature* p_Creature) : ScriptedAI(p_Creature) 
            {
                me->SetReactState(REACT_AGGRESSIVE);
            }

            EventMap m_Events;

            void EnterCombat(Unit* p_Attacker) override
            {
                if (!me->HasUnitState(UnitState::UNIT_STATE_ROOT))
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                m_Events.ScheduleEvent(eEvents::EventNetherSpike, urand(1, 2) * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventCelerityZone, urand(5, 8) * TimeConstants::IN_MILLISECONDS);
            }

            void Reset() override
            {
                m_Events.Reset();
                me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void JustReachedHome() override
            {
                if (!me->HasUnitState(UnitState::UNIT_STATE_ROOT))
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!me->HasUnitState(UnitState::UNIT_STATE_ROOT))
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (Player* l_Player = me->getVictim()->ToPlayer())
                {
                    if (!l_Player->IsWithinLOS(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()) && me->HasUnitState(UnitState::UNIT_STATE_ROOT))
                        me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                }

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventNetherSpike:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            if (me->HasUnitState(UnitState::UNIT_STATE_ROOT))
                                DoCast(l_Target, eSpells::NetherSpike, false);
                        }

                        m_Events.ScheduleEvent(eEvents::EventNetherSpike, urand(2, 4) * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventCelerityZone:
                    {
                        DoCast(eSpells::CelerityZone);
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
            return new npc_arcway_mana_wyrmAI(p_Creature);
        }
};

class npc_arcway_eye_of_the_beast : public CreatureScript
{
    public:
        npc_arcway_eye_of_the_beast() : CreatureScript("npc_arcway_eye_of_the_beast") { }

        struct npc_arcway_eye_of_the_beastAI : public ScriptedAI
        {
            npc_arcway_eye_of_the_beastAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            }

            EventMap m_Events;
            uint32 l_DespawnTimer;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_Events.ScheduleEvent(eEvents::EventOpticBlast, 3 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventDespawn, 20 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventOpticBlast:
                    {
                        float l_Orientation = (float)(urand(0, 6));
                        uint64 l_GUID = me->GetGUID();

                        me->SetFacingTo(l_Orientation);
                        me->SetOrientation(l_Orientation);
                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [l_GUID]() -> void
                        {
                            Creature* l_Creature = sObjectAccessor->FindCreature(l_GUID);
                            if (!l_Creature || !l_Creature->IsAIEnabled)
                                return;

                            l_Creature->AI()->DoCast(eSpells::OpticBlast);
                        });
                        m_Events.ScheduleEvent(eEvents::EventOpticBlast, 5.5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventDespawn:
                    {
                        me->ForcedDespawn();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arcway_eye_of_the_beastAI(p_Creature);
        }
};

class npc_arcway_eredar_chaosbringer : public CreatureScript
{
    public:
        npc_arcway_eredar_chaosbringer() : CreatureScript("npc_arcway_eredar_chaosbringer") { }

        struct npc_arcway_eredar_chaosbringerAI : public ScriptedAI
        {
            npc_arcway_eredar_chaosbringerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_IsTransformed;
            bool m_AscensionQueued;
            bool m_OnCooldown;
            uint8 l_Amount;

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!p_Attacker)
                    return;

                if (me->HasAura(eSpells::DemonicAscens) && me->GetHealthPct() <= 1)
                {
                    p_Damage = 0;

                    if (roll_chance_i(50))
                    {
                        events.ScheduleEvent(eEvents::EventFelCinders, 10 * TimeConstants::IN_MILLISECONDS);
                        me->AddAura(eSpells::DemonForm, me);
                    }
                    else
                    {
                        events.ScheduleEvent(eEvents::EventFlameBreath, 3 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventGroundSlam, 10 * TimeConstants::IN_MILLISECONDS);
                        me->AddAura(eSpells::HoundForm, me);
                    }

                    m_IsTransformed = true;
                    me->SetHealth(me->GetMaxHealth());
                    me->RemoveAura(eSpells::DemonicAscens);
                }

                if (m_IsTransformed && roll_chance_i(8) && !m_AscensionQueued && !m_OnCooldown && l_Amount <= 3)
                {
                    m_OnCooldown = true;
                    m_AscensionQueued = true;
                    events.ScheduleEvent(eEvents::EventDemonicAscension, urand(10, 60) * TimeConstants::IN_MILLISECONDS);
                    AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void { m_OnCooldown = false; });
                }
            }

            void Reset() override
            {
                DeleteOperations();
                m_IsTransformed = false;
                m_OnCooldown = false;
                m_AscensionQueued = false;
                l_Amount = 0;
                me->RemoveAura(eSpells::DemonForm);
                me->RemoveAura(eSpells::HoundForm);
            }

            void EnterCombat(Unit* p_Unit) override
            {
                events.ScheduleEvent(eEvents::EventChaosBolt, 14 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBrandOfTheLegion, 25 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDemonicAscension, urand(10, 60) * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventChaosBolt:
                    {
                        DoCast(eSpells::ChaosBolt);
                        events.ScheduleEvent(eEvents::EventChaosBolt, 14 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBrandOfTheLegion:
                    {
                        DoCast(eSpells::BrandOfLegion);
                        events.ScheduleEvent(eEvents::EventBrandOfTheLegion, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventDemonicAscension:
                    {
                        DoCast(eSpells::DemonicAscens);

                        if (m_AscensionQueued)
                            AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void { m_AscensionQueued = false; });

                        l_Amount++;
                        break;
                    }
                    case eEvents::EventGroundSlam:
                    {
                        DoCast(eSpells::GroundSlam);
                        events.ScheduleEvent(eEvents::EventGroundSlam, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFlameBreath:
                    {
                        DoCast(eSpells::FlameBreath);
                        events.ScheduleEvent(eEvents::EventFlameBreath, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFelCinders:
                    {
                        DoCast(eSpells::FelCinders);
                        events.ScheduleEvent(eEvents::EventFelCinders, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_arcway_eredar_chaosbringerAI(p_Creature);
        }
};

class at_arcway_remove_celerity_zone : public AreaTriggerEntityScript
{
    public:
        at_arcway_remove_celerity_zone() : AreaTriggerEntityScript("at_arcway_remove_celerity_zone") { }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Map* l_Map = p_AreaTrigger->GetMap())
            {
                Map::PlayerList const &l_PlayerList = l_Map->GetPlayers();
                if (l_PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                {
                    if (Player* l_Player = l_Itr->getSource())
                    {
                        if (!l_Player->HasAura(eSpells::CelerityZoneBuff))
                            continue;

                        l_Player->RemoveAura(eSpells::CelerityZoneBuff);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_arcway_remove_celerity_zone();
        }
};

#ifndef __clang_analyzer__
void AddSC_the_arcway()
{
    /// Spells

    /// Creatures
    new npc_arcway_mana_wyrm();
    new npc_arcway_teleport_bunny();
    new npc_arcway_withered_fiend();
    new npc_arcway_eye_of_the_beast();
    new npc_arcway_withered_manawraith();
    new npc_arcway_nightborne_spellsword();
    new npc_the_arcway_acidic_bile();
    new npc_the_arcway_wyrmtongue_scavanger();
    new npc_the_arcway_wrathguard_felblade();
    new npc_the_arcway_astral_spark();
    new npc_arcway_eredar_chaosbringer();

    /// Playerscripts

    /// Areatriggers
    new at_arcway_remove_celerity_zone();
}
#endif
