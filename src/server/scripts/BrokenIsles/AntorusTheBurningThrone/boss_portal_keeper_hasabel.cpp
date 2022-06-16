////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"

/// Portal Keeper Hasabel - 122104
class boss_portal_keeper_hasabel : public CreatureScript
{
    public:
        boss_portal_keeper_hasabel() : CreatureScript("boss_portal_keeper_hasabel") { }

        enum eSpells
        {
            /// Misc
            SpellPlatformNexusAura          = 244450,
            SpellCatastrophicImplosion      = 246075,
            SpellGatewayXorothVisual        = 257939,
            SpellGatewayRancoraVisual       = 257941,
            SpellGatewayNathrezaVisual      = 257942,
            SpellGatewayToXorothAT          = 244017,
            SpellGatewayToRancoraAT         = 244137,
            SpellGatewayToNathrezaAT        = 244148,
            SpellGatewayFromXorothAT        = 244111,
            SpellGatewayFromRancoraAT       = 244139,
            SpellGatewayFromNathrezaAT      = 244149,
            SpellEverburningFlames          = 244612,
            SpellEmpoweredPortalXoroth      = 244316,
            SpellRegularPortalXoroth        = 244319,
            SpellEmpoweredPortalRancora     = 246081,
            SpellRegularPortalRancora       = 246083,
            SpellEmpoweredPortalNathreza    = 246154,
            SpellRegularPortalNathreza      = 246155,
            SpellIntensity                  = 246164,
            SpellIntensityBuff              = 246163,
            SpellFelsilkWrapStunAura        = 244949,
            SpellDelusions                  = 245050,
            SpellCloyingShadows             = 245118,
            SpellBossPortalKeeperDefeated   = 249125,
            SpellEnergizeDNT                = 247143,
            SpellEverburningFlamesAT        = 245153,
            /// Reality Tear
            SpellRealityTearAura            = 244016,
            /// Collapsing World
            SpellCollapsingWorldSummon      = 243983,
            /// Transport Portal
            SpellTransportPortalMissile     = 244689,
            /// Felstorm Barrage
            SpellFelstormBarrageCast        = 244000
        };

        enum eEvents
        {
            EventRealityTear = 1,
            EventCollapsingWorld,
            EventTransportPortal,
            EventFelstormBarrage
        };

        enum eTalks
        {
            TalkIntro,
            TalkAggro,
            TalkSlay,
            TalkWipe,
            TalkDeath,
            TalkCollapsingWorld,
            TalkTransportPortal,
            TalkFelstormBarrage,
            TalkXoroth,
            TalkXorothWarn,
            TalkRancora,
            TalkRancoraWarn,
            TalkNathreza,
            TalkNathrezaWarn
        };

        enum ePhases
        {
            PhasePlatformNexus,
            PhasePlatformXoroth,
            PhasePlatformRancora,
            PhasePlatformNathreza
        };

        struct boss_portal_keeper_hasabelAI : public BossAI
        {
            boss_portal_keeper_hasabelAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataPortalKeeperHasabel) { }

            bool m_IntroDone = false;

            std::array<int32, ePhases::PhasePlatformNathreza> m_PortalSwitchPcts
            {
                {
                    90,
                    60,
                    30
                }
            };

            std::array<Position const, eData::DataHasabelMaxFelcrushPortals> m_FelcrushPortals =
            {
                {
                    { -3725.36f, -1307.78f, 624.486f, 5.30418f },
                    { -3753.77f, -1341.14f, 624.486f, 5.94835f },
                    { -3738.14f, -1388.33f, 624.486f, 0.88492f },
                    { -3688.55f, -1396.82f, 624.486f, 2.03804f },
                    { -3653.38f, -1359.35f, 624.486f, 2.85602f },
                    { -3666.90f, -1313.00f, 624.486f, 4.09176f }
                }
            };

            uint8 m_FelcrushIDx = 0;

            Position const m_ChaosRiftPos = { -3704.48f, -1392.71f, 624.107f, 4.71239f };

            uint8 m_PhaseID = ePhases::PhasePlatformNexus;

            uint32 m_BerserkTimer = 0;
            uint32 m_NexusPlatformTimer = 0;

            bool m_AddsAlive = false;

            bool m_Achievement = false;

            void Reset() override
            {
                _Reset();

                SetCombatMovement(true);

                me->SetPower(Powers::POWER_ENERGY, 67);

                m_FelcrushIDx = 0;

                m_PhaseID = ePhases::PhasePlatformNexus;

                m_BerserkTimer = 0;
                m_NexusPlatformTimer = 0;

                m_AddsAlive = false;

                m_Achievement = false;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                Talk(eTalks::TalkAggro);

                DoCast(me, eSpells::SpellEnergizeDNT, true);

                events.ScheduleEvent(eEvents::EventRealityTear, 7 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCollapsingWorld, 12 * TimeConstants::IN_MILLISECONDS + 700);

                if (IsMythic())
                {
                    events.ScheduleEvent(eEvents::EventTransportPortal, 38 * TimeConstants::IN_MILLISECONDS + 300);
                    events.ScheduleEvent(eEvents::EventFelstormBarrage, 26 * TimeConstants::IN_MILLISECONDS + 900);
                }
                else
                {
                    events.ScheduleEvent(eEvents::EventTransportPortal, 26 * TimeConstants::IN_MILLISECONDS + 700);
                    events.ScheduleEvent(eEvents::EventFelstormBarrage, 35 * TimeConstants::IN_MILLISECONDS + 700);
                }

                m_BerserkTimer = 720 * TimeConstants::IN_MILLISECONDS;
                m_NexusPlatformTimer = 1 * TimeConstants::IN_MILLISECONDS;

                std::list<Player*> l_PlayerList;

                me->GetPlayerListInGrid(l_PlayerList, 46.0f);

                for (Player* l_Player : l_PlayerList)
                    l_Player->CastSpell(l_Player, eSpells::SpellPlatformNexusAura, true);

                InitializeGateways();
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (!p_Killed->IsPlayer())
                    return;

                Talk(eTalks::TalkSlay);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_PhaseID < ePhases::PhasePlatformNathreza && me->HealthBelowPctDamaged(m_PortalSwitchPcts[m_PhaseID], p_Damage))
                {
                    ++m_PhaseID;

                    m_AddsAlive = true;

                    switch (m_PhaseID)
                    {
                        case ePhases::PhasePlatformXoroth:
                        {
                            Talk(eTalks::TalkXoroth);
                            Talk(eTalks::TalkXorothWarn);

                            DoCast(me, eSpells::SpellGatewayXorothVisual, true);

                            me->SummonCreature(eCreatures::NpcVulcanar, { -3795.228f, -1417.641f, 645.3241f, 0.5142926f });

                            EntryCheckPredicate l_Pred(eCreatures::NpcGatewayToXoroth);

                            summons.CastSpell(eSpells::SpellEmpoweredPortalXoroth, l_Pred);
                            summons.CastSpell(eSpells::SpellIntensity, l_Pred);

                            m_AddsAlive = true;
                            break;
                        }
                        case ePhases::PhasePlatformRancora:
                        {
                            Talk(eTalks::TalkRancora);
                            Talk(eTalks::TalkRancoraWarn);

                            DoCast(me, eSpells::SpellGatewayRancoraVisual, true);

                            me->SummonCreature(eCreatures::NpcLadyDacidion, { -3704.318f, -1251.28f, 649.8861f, 4.726699f });

                            EntryCheckPredicate l_Pred(eCreatures::NpcGatewayToRancora);

                            summons.CastSpell(eSpells::SpellEmpoweredPortalRancora, l_Pred);
                            summons.CastSpell(eSpells::SpellIntensity, l_Pred);

                            m_AddsAlive = true;
                            break;
                        }
                        case ePhases::PhasePlatformNathreza:
                        {
                            Talk(eTalks::TalkNathreza);
                            Talk(eTalks::TalkNathrezaWarn);

                            DoCast(me, eSpells::SpellGatewayNathrezaVisual, true);

                            me->SummonCreature(eCreatures::NpcLordEilgar, { -3612.814f, -1413.214f, 645.3286f, 3.461404f });

                            EntryCheckPredicate l_Pred(eCreatures::NpcGatewayToNathreza);

                            summons.CastSpell(eSpells::SpellEmpoweredPortalNathreza, l_Pred);
                            summons.CastSpell(eSpells::SpellIntensity, l_Pred);

                            m_AddsAlive = true;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                _JustDied();

                Talk(eTalks::TalkDeath);

                RemoveEncounterAuras();

                if (m_Achievement && instance && !IsLFR())
                    instance->DoCompleteAchievement(eAchievements::PortalCombat);

                if (instance)
                {
                    uint64 l_Guid = p_Killer->GetGUID();
                    instance->AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                    {
                        if (Unit* l_Unit = sObjectAccessor->FindUnit(l_Guid))
                            l_Unit->CastSpell(l_Unit, eSpells::SpellBossPortalKeeperDefeated, true);
                    });
                }
            }

            void EnterEvadeMode() override
            {
                me->InterruptNonMeleeSpells(true);

                me->ClearUnitState(UnitState::UNIT_STATE_ROOT);

                ClearDelayedOperations();

                BossAI::EnterEvadeMode();

                Talk(eTalks::TalkWipe);

                RemoveEncounterAuras();
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (m_IntroDone || !p_Who->IsPlayer() || p_Who->GetExactDist(me) > 80.0f)
                    return;

                m_IntroDone = true;

                Talk(eTalks::TalkIntro);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFelstormBarrageCast:
                    {
                        m_FelcrushIDx = urand(0, (eData::DataHasabelMaxFelcrushPortals - 1));

                        TriggerFelcrushPortal();

                        TriggerCDForOtherSpells(eEvents::EventFelstormBarrage);

                        if (IsHeroicOrMythic())
                        {
                            AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                m_FelcrushIDx += 2;

                                if (m_FelcrushIDx >= eData::DataHasabelMaxFelcrushPortals)
                                    m_FelcrushIDx -= uint8(eData::DataHasabelMaxFelcrushPortals);

                                TriggerFelcrushPortal();
                            });

                            AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                m_FelcrushIDx += 2;

                                if (m_FelcrushIDx >= eData::DataHasabelMaxFelcrushPortals)
                                    m_FelcrushIDx -= uint8(eData::DataHasabelMaxFelcrushPortals);

                                TriggerFelcrushPortal();
                            });
                        }

                        break;
                    }
                    case eSpells::SpellCollapsingWorldSummon:
                    {
                        TriggerCDForOtherSpells(eEvents::EventCollapsingWorld);
                        break;
                    }
                    case eSpells::SpellTransportPortalMissile:
                    {
                        TriggerCDForOtherSpells(eEvents::EventTransportPortal);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                p_Summon->DespawnOrUnsummon(10 * TimeConstants::IN_MILLISECONDS);

                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcVulcanar:
                    {
                        EntryCheckPredicate l_Pred(eCreatures::NpcGatewayToXoroth);

                        summons.RemoveAura(eSpells::SpellEmpoweredPortalXoroth, l_Pred);
                        summons.RemoveAura(eSpells::SpellIntensity, l_Pred);
                        summons.RemoveAura(eSpells::SpellIntensityBuff, l_Pred);

                        summons.CastSpell(eSpells::SpellRegularPortalXoroth, l_Pred);

                        m_AddsAlive = false;
                        break;
                    }
                    case eCreatures::NpcLadyDacidion:
                    {
                        EntryCheckPredicate l_Pred(eCreatures::NpcGatewayToRancora);

                        summons.RemoveAura(eSpells::SpellEmpoweredPortalRancora, l_Pred);
                        summons.RemoveAura(eSpells::SpellIntensity, l_Pred);
                        summons.RemoveAura(eSpells::SpellIntensityBuff, l_Pred);

                        summons.CastSpell(eSpells::SpellRegularPortalRancora, l_Pred);

                        m_AddsAlive = false;
                        break;
                    }
                    case eCreatures::NpcLordEilgar:
                    {
                        EntryCheckPredicate l_Pred(eCreatures::NpcGatewayToNathreza);

                        summons.RemoveAura(eSpells::SpellEmpoweredPortalNathreza, l_Pred);
                        summons.RemoveAura(eSpells::SpellIntensity, l_Pred);
                        summons.RemoveAura(eSpells::SpellIntensityBuff, l_Pred);

                        summons.CastSpell(eSpells::SpellRegularPortalNathreza, l_Pred);

                        m_AddsAlive = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcEverburningFlames:
                    {
                        uint64 l_Guid = p_Summon->GetGUID();
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                        {
                            if (!instance)
                                return;

                            if (Creature* l_Add = Creature::GetCreature(*me, l_Guid))
                            {
                                l_Add->GetMotionMaster()->Clear();
                                l_Add->GetMotionMaster()->MoveRandom(5.0f);
                            }
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (m_Achievement || !me->isAlive())
                    return;

                m_Achievement = true;

                me->SummonCreature(eCreatures::NpcChaoticRift, m_ChaosRiftPos);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                /// Reset threat on target if target's out of platform, prevent reset issues
                if (Unit* l_Target = me->getVictim())
                {
                    if (me->GetExactDist2d(l_Target) >= 45.0f)
                    {
                        DoModifyThreatPercent(l_Target, -99);

                        if (!UpdateVictim())
                            return;
                    }
                }

                Position l_Home = me->GetHomePosition();
                if (me->GetExactDist(&l_Home) >= 46.0f)
                {
                    EnterEvadeMode();
                    return;
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

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                CheckNexusPlatformPlayers(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventRealityTear:
                    {
                        DoCastVictim(eSpells::SpellRealityTearAura);

                        events.ScheduleEvent(eEvents::EventRealityTear, m_AddsAlive ? (24 * TimeConstants::IN_MILLISECONDS + 400) : (12 * TimeConstants::IN_MILLISECONDS + 200));
                        break;
                    }
                    case eEvents::EventCollapsingWorld:
                    {
                        DoCast(me->GetPosition(), eSpells::SpellCollapsingWorldSummon);

                        TriggerCDForOtherSpells(eEvents::EventCollapsingWorld, 2 * TimeConstants::IN_MILLISECONDS);

                        events.DelayEvent(eEvents::EventTransportPortal, 2 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventFelstormBarrage, 2 * TimeConstants::IN_MILLISECONDS);

                        Talk(eTalks::TalkCollapsingWorld);

                        if (IsNormal() || IsLFR())
                            events.ScheduleEvent(eEvents::EventCollapsingWorld, 39 * TimeConstants::IN_MILLISECONDS + 100);
                        else if (IsMythic())
                            events.ScheduleEvent(eEvents::EventCollapsingWorld, 29 * TimeConstants::IN_MILLISECONDS + 500);
                        else
                            events.ScheduleEvent(eEvents::EventCollapsingWorld, 34 * TimeConstants::IN_MILLISECONDS + 750);

                        break;
                    }
                    case eEvents::EventTransportPortal:
                    {
                        DoCast(me, eSpells::SpellTransportPortalMissile);

                        TriggerCDForOtherSpells(eEvents::EventTransportPortal, 1 * TimeConstants::IN_MILLISECONDS + 500);

                        events.DelayEvent(eEvents::EventCollapsingWorld, 1 * TimeConstants::IN_MILLISECONDS + 500);
                        events.DelayEvent(eEvents::EventFelstormBarrage, 1 * TimeConstants::IN_MILLISECONDS + 500);

                        Talk(eTalks::TalkTransportPortal);

                        events.ScheduleEvent(eEvents::EventTransportPortal, IsMythic() ? (38 * TimeConstants::IN_MILLISECONDS) : (43 * TimeConstants::IN_MILLISECONDS + 200));
                        break;
                    }
                    case eEvents::EventFelstormBarrage:
                    {
                        DoCast(me, eSpells::SpellFelstormBarrageCast);

                        TriggerCDForOtherSpells(eEvents::EventFelstormBarrage, 2 * TimeConstants::IN_MILLISECONDS);

                        events.DelayEvent(eEvents::EventCollapsingWorld, 2 * TimeConstants::IN_MILLISECONDS);
                        events.DelayEvent(eEvents::EventFelstormBarrage, 2 * TimeConstants::IN_MILLISECONDS);

                        Talk(eTalks::TalkFelstormBarrage);

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventFelstormBarrage, 27 * TimeConstants::IN_MILLISECONDS + 500);
                        else if (IsLFR() || IsNormal())
                            events.ScheduleEvent(eEvents::EventFelstormBarrage, 37 * TimeConstants::IN_MILLISECONDS + 100);
                        else
                            events.ScheduleEvent(eEvents::EventFelstormBarrage, 32 * TimeConstants::IN_MILLISECONDS + 750);

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void TriggerFelcrushPortal()
            {
                if (m_FelcrushIDx >= eData::DataHasabelMaxFelcrushPortals)
                    return;

                me->SummonCreature(eCreatures::NpcFelcrushPortal, m_FelcrushPortals[m_FelcrushIDx]);
            }

            void CheckNexusPlatformPlayers(uint32 const p_Diff)
            {
                if (m_NexusPlatformTimer)
                {
                    if (m_NexusPlatformTimer <= p_Diff)
                    {
                        bool l_NexusEmpty = true;

                        Position l_Pos = me->GetHomePosition();

                        /// Check if there is at least one player on Nexus platform
                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = l_Ref->getTarget())
                            {
                                if (!l_Target->IsPlayer() || l_Target->GetExactDist2d(&l_Pos) >= 200.0f)
                                    continue;

                                if (l_Target->m_positionZ < 635.0f && l_Target->GetExactDist2d(&l_Pos) <= 46.0f)
                                {
                                    l_NexusEmpty = false;
                                    break;
                                }
                            }
                        }

                        /// If no player found on Nexus platform, trigger Catastrophic Implosion
                        if (l_NexusEmpty)
                        {
                            SetCombatMovement(false);

                            me->StopMoving();
                            me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                            DoCast(me, eSpells::SpellCatastrophicImplosion);
                        }
                        else
                        {
                            SetCombatMovement(true);

                            me->ClearUnitState(UnitState::UNIT_STATE_ROOT);

                            if (Unit* l_Target = me->getVictim())
                            {
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Target);
                            }
                        }

                        m_NexusPlatformTimer = 500;
                    }
                    else
                        m_NexusPlatformTimer -= p_Diff;
                }
            }

            void RemoveEncounterAuras()
            {
                if (!instance)
                    return;

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellRealityTearAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellPlatformNexusAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelsilkWrapStunAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDelusions);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCloyingShadows);

                me->DespawnCreaturesInArea
                ({
                    eCreatures::NpcBlazingImp,
                    eCreatures::NpcHungeringStalker,
                    eCreatures::NpcFeltouchedSkitterer,
                    eCreatures::NpcEverburningFlamesFight,
                    eCreatures::NpcFelsilkWrap
                }, 200.0f);

                me->DespawnAreaTriggersInArea
                ({
                    eSpells::SpellEverburningFlames,
                    eSpells::SpellEverburningFlamesAT
                }, 200.0f);
            }

            void InitializeGateways()
            {
                std::map<uint32, uint32> l_Gateways =
                {
                    { eCreatures::NpcGatewayToXoroth,       eSpells::SpellGatewayToXorothAT },
                    { eCreatures::NpcGatewayToRancora,      eSpells::SpellGatewayToRancoraAT },
                    { eCreatures::NpcGatewayToNathreza,     eSpells::SpellGatewayToNathrezaAT },
                    { eCreatures::NpcGatewayFromXoroth,     eSpells::SpellGatewayFromXorothAT },
                    { eCreatures::NpcGatewayFromRancora,    eSpells::SpellGatewayFromRancoraAT },
                    { eCreatures::NpcGatewayFromNathreza,   eSpells::SpellGatewayFromNathrezaAT }
                };

                for (auto const& l_Iter : l_Gateways)
                {
                    EntryCheckPredicate l_Pred(l_Iter.first);

                    summons.RemoveAura(l_Iter.second, l_Pred);
                    summons.CastSpell(l_Iter.second, l_Pred);
                }
            }

            uint32 GetData(uint32 /*p_ID*/) override
            {
                return m_PhaseID;
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                if (Unit* l_Target = Unit::GetUnit(*me, p_Guid))
                {
                    me->getThreatManager().modifyThreatPercent(l_Target, -100);

                    if (Unit* l_Victim = me->getThreatManager().getHostilTarget())
                        AttackStart(l_Victim);
                }
            }

            void TriggerCDForOtherSpells(uint8 p_ExcludeEventID, uint32 p_CastTime = 0)
            {
                std::set<uint8> l_EventsToPause =
                {
                    eEvents::EventCollapsingWorld,
                    eEvents::EventTransportPortal,
                    eEvents::EventFelstormBarrage
                };

                for (uint8 l_EventID : l_EventsToPause)
                {
                    if (p_ExcludeEventID == l_EventID)
                        continue;

                    uint32 l_Cooldown = (l_EventID == eEvents::EventTransportPortal) ? (8 * TimeConstants::IN_MILLISECONDS + 500) : 9 * TimeConstants::IN_MILLISECONDS;

                    l_Cooldown += p_CastTime;

                    if ((events.GetEventTime(l_EventID) - events.GetTimer()) < l_Cooldown)
                    {
                        events.CancelEvent(l_EventID);
                        events.ScheduleEvent(l_EventID, l_Cooldown);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_portal_keeper_hasabelAI(p_Creature);
        }
};

/// Collapsing World - 122425
class npc_hasabel_collapsing_world : public CreatureScript
{
    public:
        npc_hasabel_collapsing_world() : CreatureScript("npc_hasabel_collapsing_world") { }

        struct npc_hasabel_collapsing_worldAI : public Scripted_NoMovementAI
        {
            npc_hasabel_collapsing_worldAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpell
            {
                SpellCollapsingWorldAoE = 243984
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpell::SpellCollapsingWorldAoE);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpell::SpellCollapsingWorldAoE)
                    me->DespawnOrUnsummon(100);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hasabel_collapsing_worldAI(p_Creature);
        }
};

/// Transport Portal - 122761
class npc_hasabel_transport_portal : public CreatureScript
{
    public:
        npc_hasabel_transport_portal() : CreatureScript("npc_hasabel_transport_portal") { }

        struct npc_hasabel_transport_portalAI : public Scripted_NoMovementAI
        {
            npc_hasabel_transport_portalAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpell
            {
                SpellTransportPortalAura = 244687
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpell::SpellTransportPortalAura, true);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                Scripted_NoMovementAI::JustSummoned(p_Summon);

                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcBlazingImp:
                    case eCreatures::NpcHungeringStalker:
                    case eCreatures::NpcFeltouchedSkitterer:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (Unit* l_Target = l_Owner->getVictim())
                            {
                                if (p_Summon->IsAIEnabled)
                                    p_Summon->AI()->AttackStart(l_Target);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hasabel_transport_portalAI(p_Creature);
        }
};

/// Felcrush Portal - 122438
class npc_hasabel_felcrush_portal : public CreatureScript
{
    public:
        npc_hasabel_felcrush_portal() : CreatureScript("npc_hasabel_felcrush_portal") { }

        struct npc_hasabel_felcrush_portalAI : public Scripted_NoMovementAI
        {
            npc_hasabel_felcrush_portalAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            enum eSpell
            {
                SpellFelstormBarrage = 244001
            };

            enum eRestrict
            {
                TargetRestrict = 36670
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpell::SpellFelstormBarrage);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpell::SpellFelstormBarrage && !p_Targets.empty())
                {
                    SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eRestrict::TargetRestrict);
                    if (l_Restriction == nullptr)
                        return;

                    WorldLocation l_Dest = *me;

                    l_Dest.SimplePosXYRelocationByAngle(l_Dest, 100.0f, me->m_orientation, true);

                    p_Targets.remove_if([this, l_Restriction, l_Dest](WorldObject* p_Object) -> bool
                    {
                        return !p_Object->IsInAxe(me, &l_Dest, l_Restriction->Width / 2.0f);
                    });
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpell::SpellFelstormBarrage)
                    me->DespawnOrUnsummon(100);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hasabel_felcrush_portalAI(p_Creature);
        }
};

/// Vulcanar - 122211
class npc_hasabel_vulcanar : public CreatureScript
{
    public:
        npc_hasabel_vulcanar() : CreatureScript("npc_hasabel_vulcanar") { }

        enum eSpells
        {
            SpellAegisOfFlames          = 244383,

            SpellFlamesOfXoroth         = 244607,

            SpellSupernovaMissile       = 244598,

            SpellCausticDetonationNpc   = 244864
        };

        enum eEvents
        {
            EventFlamesOfXoroth = 1,
            EventSupernova
        };

        enum eTalks
        {
            TalkAggro,
            TalkDeath
        };

        struct npc_hasabel_vulcanarAI : public ScriptedAI
        {
            npc_hasabel_vulcanarAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void EnterEvadeMode() override
            {
                me->RemoveNegativeAuras();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->ClearLootContainers();

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTargetedHome(true);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->DisableHealthRegen();

                if (IsMythic())
                {
                    for (uint8 l_I = 0; l_I < 3; ++l_I)
                        DoCast(me, eSpells::SpellAegisOfFlames, true);
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(eTalks::TalkAggro);

                events.Reset();

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                events.ScheduleEvent(eEvents::EventFlamesOfXoroth, 7 * TimeConstants::IN_MILLISECONDS + 300);
                events.ScheduleEvent(eEvents::EventSupernova, 13 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (IsMythic() && p_SpellInfo->Id == eSpells::SpellCausticDetonationNpc)
                {
                    if (Aura* l_Aura = me->GetAura(eSpells::SpellAegisOfFlames))
                        l_Aura->DropStack();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                Position l_Pos = me->GetHomePosition();
                if (me->GetExactDist2d(&l_Pos) >= 40.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFlamesOfXoroth:
                    {
                        DoCast(me, eSpells::SpellFlamesOfXoroth);

                        events.ScheduleEvent(eEvents::EventFlamesOfXoroth, 7 * TimeConstants::IN_MILLISECONDS + 300);
                        break;
                    }
                    case eEvents::EventSupernova:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 40.0f))
                            DoCast(l_Target, eSpells::SpellSupernovaMissile, true);

                        events.ScheduleEvent(eEvents::EventSupernova, 7 * TimeConstants::IN_MILLISECONDS + 300);
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
            return new npc_hasabel_vulcanarAI(p_Creature);
        }
};

/// Lady Dacidion - 122212
class npc_hasabel_lady_dacidion : public CreatureScript
{
    public:
        npc_hasabel_lady_dacidion() : CreatureScript("npc_hasabel_lady_dacidion") { }

        enum eSpells
        {
            SpellPoisonEssence      = 246316,
            SpellLeechEssence       = 244915,
            SpellFelsilkWrap        = 244926,
            SpellFelsilkWrapSummon  = 244946
        };

        enum eEvents
        {
            EventPoisonEssence = 1,
            EventFelsilkWrap
        };

        enum eTalks
        {
            TalkAggro,
            TalkDeath
        };

        struct npc_hasabel_lady_dacidionAI : public ScriptedAI
        {
            npc_hasabel_lady_dacidionAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->DisableHealthRegen();
            }

            void EnterEvadeMode() override
            {
                me->RemoveNegativeAuras();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->ClearLootContainers();

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTargetedHome(true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(eTalks::TalkAggro);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                events.ScheduleEvent(eEvents::EventPoisonEssence, 12 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventFelsilkWrap, 18 * TimeConstants::IN_MILLISECONDS + 600);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFelsilkWrap:
                    {
                        p_Target->CastSpell(p_Target, eSpells::SpellFelsilkWrapSummon, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                Position l_Pos = me->GetHomePosition();
                if (me->GetExactDist2d(&l_Pos) >= 40.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventPoisonEssence:
                    {
                        if (IsMythic())
                            DoCast(me, eSpells::SpellLeechEssence);
                        else
                            DoCast(me, eSpells::SpellPoisonEssence);

                        events.ScheduleEvent(eEvents::EventPoisonEssence, 9 * TimeConstants::IN_MILLISECONDS + 700);
                        break;
                    }
                    case eEvents::EventFelsilkWrap:
                    {
                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllDPS))
                            DoCast(l_Target, eSpells::SpellFelsilkWrap);

                        events.ScheduleEvent(eEvents::EventFelsilkWrap, 17 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_hasabel_lady_dacidionAI(p_Creature);
        }
};

/// Felsilk Wrap - 122897
class npc_hasabel_felsilk_wrap : public CreatureScript
{
    public:
        npc_hasabel_felsilk_wrap() : CreatureScript("npc_hasabel_felsilk_wrap") { }

        enum eSpells
        {
            SpellFelsilkWrapVisual  = 244948,
            SpellFelsilkWrapStun    = 244949
        };

        struct npc_hasabel_felsilk_wrapAI : public Scripted_NoMovementAI
        {
            npc_hasabel_felsilk_wrapAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            uint64 m_SummonerGuid = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellFelsilkWrapVisual, true);
                DoCast(p_Summoner, eSpells::SpellFelsilkWrapStun, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Unit* l_Summoner = Unit::GetUnit(*me, m_SummonerGuid))
                    l_Summoner->RemoveAura(eSpells::SpellFelsilkWrapStun);

                me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hasabel_felsilk_wrapAI(p_Creature);
        }
};

/// Lord Eilgar - 122213
class npc_hasabel_lord_eilgar : public CreatureScript
{
    public:
        npc_hasabel_lord_eilgar() : CreatureScript("npc_hasabel_lord_eilgar") { }

        enum eSpells
        {
            SpellCorrupt    = 245040,

            SpellDelusions  = 245050,

            SpellMindFogAT  = 245094
        };

        enum eEvents
        {
            EventCorrupt = 1,
            EventDelusions
        };

        enum eTalks
        {
            TalkAggro,
            TalkDeath
        };

        struct npc_hasabel_lord_eilgarAI : public ScriptedAI
        {
            npc_hasabel_lord_eilgarAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->DisableHealthRegen();
            }

            void EnterEvadeMode() override
            {
                me->RemoveNegativeAuras();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->ClearLootContainers();

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTargetedHome(true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(eTalks::TalkAggro);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                events.ScheduleEvent(eEvents::EventCorrupt, 15 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDelusions, 20 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                if (AreaTrigger* l_AT = me->FindNearestAreaTrigger(eSpells::SpellMindFogAT, 100.0f))
                    l_AT->SetDuration(0);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellDelusions:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object->IsPlayer())
                                return true;

                            if (p_Object->ToPlayer()->GetRoleForGroup() != Roles::ROLE_HEALER)
                                return true;

                            return false;
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                Position l_Pos = me->GetHomePosition();
                if (me->GetExactDist2d(&l_Pos) >= 40.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCorrupt:
                    {
                        DoCast(me, eSpells::SpellCorrupt);

                        events.ScheduleEvent(eEvents::EventCorrupt, 7 * TimeConstants::IN_MILLISECONDS + 600);
                        break;
                    }
                    case eEvents::EventDelusions:
                    {
                        DoCast(me, eSpells::SpellDelusions);

                        events.ScheduleEvent(eEvents::EventDelusions, 14 * TimeConstants::IN_MILLISECONDS + 500);
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
            return new npc_hasabel_lord_eilgarAI(p_Creature);
        }
};

/// Everburning Flames (for fight) - 123003
class npc_hasabel_everburning_flames : public CreatureScript
{
    public:
        npc_hasabel_everburning_flames() : CreatureScript("npc_hasabel_everburning_flames") { }

        struct npc_hasabel_everburning_flamesAI : public Scripted_NoMovementAI
        {
            npc_hasabel_everburning_flamesAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void AreaTriggerDespawned(AreaTrigger* /*p_AreaTrigger*/, bool p_Removed) override
            {
                if (!p_Removed)
                    return;

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->RemoveAllAreasTrigger();
                    me->DespawnOrUnsummon(1);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hasabel_everburning_flamesAI(p_Creature);
        }
};

/// Chaotic Rift - 124270
class npc_hasabel_chaotic_rift : public CreatureScript
{
    public:
        npc_hasabel_chaotic_rift() : CreatureScript("npc_hasabel_chaotic_rift") { }

        enum eSpells
        {
            SpellChaoticRiftAT      = 246887,
            SpellChaoticRiftAura    = 246898,

            SpellPlatformNexus      = 244450
        };

        struct npc_hasabel_chaotic_riftAI : public Scripted_NoMovementAI
        {
            npc_hasabel_chaotic_riftAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
            }

            uint32 m_CheckPlayerTimer = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(me, eSpells::SpellChaoticRiftAT, true);
                DoCast(me, eSpells::SpellChaoticRiftAura, true);

                std::list<HostileReference*> l_ThreatList = p_Summoner->getThreatManager().getThreatList();
                for (HostileReference* l_Ref : l_ThreatList)
                {
                    if (Unit* l_Target = l_Ref->getTarget())
                    {
                        if (!l_Target->IsPlayer() || !l_Target->HasAura(eSpells::SpellPlatformNexus))
                            continue;

                        l_Target->ToPlayer()->SendApplyMovementForce(me->GetGUID(), true, me->GetPosition(), 2.0f, 1);
                    }
                }

                m_CheckPlayerTimer = 1 * TimeConstants::IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_CheckPlayerTimer)
                {
                    if (m_CheckPlayerTimer <= p_Diff && me->GetAnyOwner())
                    {
                        std::list<HostileReference*> l_ThreatList = me->GetAnyOwner()->getThreatManager().getThreatList();
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = l_Ref->getTarget())
                            {
                                Player* l_Player = l_Target->ToPlayer();

                                /// Can't be affected
                                if (!l_Player)
                                    continue;

                                /// Can't be affected on other platforms than the boss platform
                                if (!l_Target->HasAura(eSpells::SpellPlatformNexus))
                                {
                                    /// Remove movement force if needed
                                    if (l_Player->HasMovementForce(me->GetGUID()))
                                        l_Player->SendApplyMovementForce(me->GetGUID(), false, Position());

                                    continue;
                                }

                                /// Already affected
                                if (l_Player->HasMovementForce(me->GetGUID()))
                                    continue;

                                l_Player->SendApplyMovementForce(me->GetGUID(), true, me->GetPosition(), 2.0f, 1);
                            }
                        }

                        m_CheckPlayerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_CheckPlayerTimer -= p_Diff;
                }
            }

            void JustDespawned() override
            {
                Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->HasMovementForce(me->GetGUID()))
                            l_Player->SendApplyMovementForce(me->GetGUID(), false, Position());
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hasabel_chaotic_riftAI(p_Creature);
        }
};

/// Reality Tear - 244016
class spell_hasabel_reality_tear : public SpellScriptLoader
{
    public:
        spell_hasabel_reality_tear() : SpellScriptLoader("spell_hasabel_reality_tear") { }

        enum eSpell
        {
            SpellBurstingDarkness = 244021
        };

        class spell_hasabel_reality_tear_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_reality_tear_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                {
                    int32 l_Stacks = p_AurEff->GetBase()->GetStackAmount();
                    if (l_Stacks > 1)
                    {
                        for (int32 l_I = 0; l_I < l_Stacks; ++l_I)
                            l_Target->CastSpell(l_Target, eSpell::SpellBurstingDarkness, true);
                    }
                    else
                        l_Target->CastSpell(l_Target, eSpell::SpellBurstingDarkness, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hasabel_reality_tear_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_reality_tear_AuraScript();
        }
};

/// Transport Portal - 244687
class spell_hasabel_transport_portal : public SpellScriptLoader
{
    public:
        spell_hasabel_transport_portal() : SpellScriptLoader("spell_hasabel_transport_portal") { }

        enum eSpells
        {
            SpellTransportPortalImpMissile      = 244692,
            SpellTransportPortalStalkerMissile  = 245485,
            SpellTransportPortalSkitterMissile  = 246192
        };

        class spell_hasabel_transport_portal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_transport_portal_AuraScript);

            uint32 m_StalkerCount = 0;
            uint32 m_SkitterCount = 0;

            bool Load() override
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMap()->IsLFR())
                        return true;

                    InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                    if (!l_Instance)
                        return true;

                    Creature* l_Boss = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::BossPortalKeeperHasabel));
                    if (!l_Boss || !l_Boss->IsAIEnabled)
                        return true;

                    switch (l_Boss->AI()->GetData(0))
                    {
                        case 0: ///< No portal opened - Only Imps
                        case 1: ///< Xoroth - Only Imps
                            break;
                        case 2: ///< Rancora - Add two Feltouched Skitterers
                            m_SkitterCount = 2;
                            break;
                        case 3: ///< Nathreza - Add one Hungering Stalkers and two Feltouched Skitterers
                            m_SkitterCount = 2;
                            m_StalkerCount = 1;
                            break;
                        default:
                            break;
                    }
                }

                return true;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Target->GetMap()->IsLFR())
                        l_Target->CastSpell(l_Target, eSpells::SpellTransportPortalImpMissile, true);
                    else
                    {
                        if (m_SkitterCount > 0)
                        {
                            --m_SkitterCount;

                            l_Target->CastSpell(l_Target, eSpells::SpellTransportPortalSkitterMissile, true);
                        }
                        else if (m_StalkerCount > 0)
                        {
                            --m_StalkerCount;

                            l_Target->CastSpell(l_Target, eSpells::SpellTransportPortalStalkerMissile, true);
                        }
                        else
                            l_Target->CastSpell(l_Target, eSpells::SpellTransportPortalImpMissile, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hasabel_transport_portal_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_transport_portal_AuraScript();
        }
};

/// Essence Transfer (Xoroth) - 252776
/// Essence Transfer (Rancora) - 252777
/// Essence Transfer (Nathreza) - 244494
class spell_hasabel_essence_transfer : public SpellScriptLoader
{
    public:
        spell_hasabel_essence_transfer() : SpellScriptLoader("spell_hasabel_essence_transfer") { }

        enum eSpells
        {
            SpellEssenceTransferXoroth      = 252776,
            SpellEssenceTransferRancora     = 252777,
            SpellEssenceTransferNathreza    = 244494,

            SpellPlatformNexus              = 244450,
            SpellPlatformRancora            = 244512,

            SpellMindFogVisionReduced       = 245099
        };

        class spell_hasabel_essence_transfer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hasabel_essence_transfer_SpellScript);

            SpellCastResult CheckEncounter()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    bool l_Reverse = false;

                    switch (GetSpellInfo()->Id)
                    {
                        case eSpells::SpellEssenceTransferXoroth:
                        {
                            if (l_Caster->FindNearestCreature(eCreatures::NpcGatewayFromXoroth, 5.0f))
                                l_Reverse = true;

                            break;
                        }
                        case eSpells::SpellEssenceTransferRancora:
                        {
                            if (l_Caster->FindNearestCreature(eCreatures::NpcGatewayFromRancora, 5.0f))
                                l_Reverse = true;

                            break;
                        }
                        case eSpells::SpellEssenceTransferNathreza:
                        {
                            if (l_Caster->FindNearestCreature(eCreatures::NpcGatewayFromNathreza, 5.0f))
                                l_Reverse = true;

                            break;
                        }
                        default:
                            break;
                    }

                    InstanceScript* l_Instance = l_Caster->GetInstanceScript();

                    /// Prevent players from taking Portals to platforms not during encounter
                    if (!l_Reverse && l_Instance && l_Instance->GetBossState(eData::DataPortalKeeperHasabel) != EncounterState::IN_PROGRESS)
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hasabel_essence_transfer_SpellScript::CheckEncounter);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hasabel_essence_transfer_SpellScript();
        }

        class spell_hasabel_essence_transfer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_essence_transfer_AuraScript);

            using PathBySpell = std::map<uint32, std::vector<G3D::Vector3>>;

            float m_Speed = 0.0f;

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->IsPlayer())
                        return;

                    PathBySpell l_PathBySpell =
                    {
                        {
                            eSpells::SpellEssenceTransferXoroth,
                            {
                                { -3722.595f, -1367.566f, 633.6159f },
                                { -3733.582f, -1392.696f, 649.2192f },
                                { -3749.212f, -1399.998f, 658.3994f },
                                { -3761.398f, -1412.021f, 649.2296f }
                            }
                        },
                        {
                            eSpells::SpellEssenceTransferRancora,
                            {
                                { -3709.7712f, -1330.8860f, 636.3751f },
                                { -3719.2468f, -1316.9465f, 650.9701f },
                                { -3721.6682f, -1295.9692f, 672.0240f },
                                { -3719.6343f, -1278.0856f, 658.5784f }
                            }
                        },
                        {
                            eSpells::SpellEssenceTransferNathreza,
                            {
                                { -3674.5776f, -1358.2299f, 637.5580f },
                                { -3662.0642f, -1365.0559f, 648.3995f },
                                { -3641.0000f, -1371.5027f, 663.5905f },
                                { -3622.4460f, -1387.4729f, 651.7955f }
                            }
                        }
                    };

                    auto const l_Iter = l_PathBySpell.find(GetSpellInfo()->Id);
                    if (l_Iter == l_PathBySpell.end())
                        return;

                    bool l_Reverse = false;

                    switch (GetSpellInfo()->Id)
                    {
                        case eSpells::SpellEssenceTransferXoroth:
                        {
                            if (l_Target->FindNearestCreature(eCreatures::NpcGatewayFromXoroth, 5.0f))
                                l_Reverse = true;

                            break;
                        }
                        case eSpells::SpellEssenceTransferRancora:
                        {
                            if (l_Target->FindNearestCreature(eCreatures::NpcGatewayFromRancora, 5.0f))
                                l_Reverse = true;

                            break;
                        }
                        case eSpells::SpellEssenceTransferNathreza:
                        {
                            if (l_Target->FindNearestCreature(eCreatures::NpcGatewayFromNathreza, 5.0f))
                                l_Reverse = true;

                            break;
                        }
                        default:
                            break;
                    }

                    InstanceScript* l_Instance = l_Target->GetInstanceScript();

                    /// Prevent players from taking Portals to platforms not during encounter
                    if (!l_Reverse && l_Instance && l_Instance->GetBossState(eData::DataPortalKeeperHasabel) != EncounterState::IN_PROGRESS)
                        return;

                    if (l_Instance)
                    {
                        if (Creature* l_Hasabel = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::BossPortalKeeperHasabel)))
                        {
                            if (l_Hasabel->IsAIEnabled)
                                l_Hasabel->AI()->SetGUID(l_Target->GetGUID());
                        }
                    }

                    std::vector<G3D::Vector3> l_Path;

                    if (l_Reverse)
                    {
                        for (std::vector<G3D::Vector3>::reverse_iterator l_Itr = l_Iter->second.rbegin(); l_Itr != l_Iter->second.rend(); ++l_Itr)
                            l_Path.push_back(*l_Itr);
                    }
                    else
                        l_Path = l_Iter->second;

                    m_Speed = l_Target->GetSpeedRate(UnitMoveType::MOVE_FLIGHT);

                    if (!l_Reverse)
                    {
                        l_Target->RemoveAura(eSpells::SpellPlatformNexus);

                        if (GetSpellInfo()->Id == eSpells::SpellEssenceTransferRancora)
                            l_Target->CastSpell(l_Target, eSpells::SpellPlatformRancora, true);
                    }
                    else
                        l_Target->RemoveAura(eSpells::SpellMindFogVisionReduced);

                    l_Target->SetSpeed(UnitMoveType::MOVE_FLIGHT, 3.0f);

                    l_Target->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);

                    l_Target->GetMotionMaster()->Clear();
                    l_Target->GetMotionMaster()->MoveSmoothFlyPath(0, l_Path.data(), l_Path.size());
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (G3D::fuzzyEq(m_Speed, 0.0f))
                        return;

                    bool l_Reverse = true;

                    switch (GetSpellInfo()->Id)
                    {
                        case eSpells::SpellEssenceTransferXoroth:
                        {
                            if (l_Target->FindNearestCreature(eCreatures::NpcGatewayFromXoroth, 20.0f))
                                l_Reverse = false;

                            break;
                        }
                        case eSpells::SpellEssenceTransferRancora:
                        {
                            if (l_Target->FindNearestCreature(eCreatures::NpcGatewayFromRancora, 20.0f))
                                l_Reverse = false;

                            break;
                        }
                        case eSpells::SpellEssenceTransferNathreza:
                        {
                            if (l_Target->FindNearestCreature(eCreatures::NpcGatewayFromNathreza, 20.0f))
                                l_Reverse = false;

                            break;
                        }
                        default:
                            break;
                    }

                    l_Target->SetSpeed(UnitMoveType::MOVE_FLIGHT, m_Speed);

                    l_Target->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_FLYING);

                    if (l_Reverse)
                    {
                        l_Target->CastSpell(l_Target, eSpells::SpellPlatformNexus, true);

                        l_Target->RemoveAura(eSpells::SpellPlatformRancora);
                    }
                    else
                    {
                        if (GetSpellInfo()->Id == eSpells::SpellEssenceTransferNathreza)
                            l_Target->CastSpell(l_Target, eSpells::SpellMindFogVisionReduced, true);
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hasabel_essence_transfer_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_FLY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hasabel_essence_transfer_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_FLY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_essence_transfer_AuraScript();
        }
};

/// Unstable Portal - 255804
class spell_hasabel_unstable_portal : public SpellScriptLoader
{
    public:
        spell_hasabel_unstable_portal() : SpellScriptLoader("spell_hasabel_unstable_portal") { }

        enum eSpell
        {
            SpellUnstablePortalCast = 255805
        };

        class spell_hasabel_unstable_portal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_unstable_portal_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->HasUnitState(UnitState::UNIT_STATE_CASTING))
                        l_Target->CastSpell(l_Target, eSpell::SpellUnstablePortalCast, false);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hasabel_unstable_portal_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_unstable_portal_AuraScript();
        }
};

/// Everburning Flames - 244613
class spell_hasabel_everburning_flames : public SpellScriptLoader
{
    public:
        spell_hasabel_everburning_flames() : SpellScriptLoader("spell_hasabel_everburning_flames") { }

        enum eSpell
        {
            SpellEverburningFlamesSummon = 244661
        };

        class spell_hasabel_everburning_flames_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_everburning_flames_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellEverburningFlamesSummon, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hasabel_everburning_flames_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_everburning_flames_AuraScript();
        }
};

/// Poison Essence - 246316
/// Leech Essence - 244915
class spell_hasabel_poison_essence : public SpellScriptLoader
{
    public:
        spell_hasabel_poison_essence() : SpellScriptLoader("spell_hasabel_poison_essence") { }

        enum eSpells
        {
            SpellLeechEssenceAura   = 244915,
            SpellPoisonEssenceAura  = 246316,

            SpellLeechEssenceVisual = 244918
        };

        class spell_hasabel_poison_essence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_poison_essence_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (l_Target && l_Caster)
                    l_Target->CastSpell(l_Caster, eSpells::SpellLeechEssenceVisual, true);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::SpellPoisonEssenceAura:
                    {
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hasabel_poison_essence_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE);
                        break;
                    }
                    case eSpells::SpellLeechEssenceAura:
                    {
                        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hasabel_poison_essence_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_LEECH);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_poison_essence_AuraScript();
        }
};

/// Felsilk Wrap - 244949
class spell_hasabel_felsilk_wrap : public SpellScriptLoader
{
    public:
        spell_hasabel_felsilk_wrap() : SpellScriptLoader("spell_hasabel_felsilk_wrap") { }

        enum eSpell
        {
            SpellFelsilkWrapDamage = 244952
        };

        class spell_hasabel_felsilk_wrap_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_felsilk_wrap_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellFelsilkWrapDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hasabel_felsilk_wrap_AuraScript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_felsilk_wrap_AuraScript();
        }
};

/// Caustic Slime - 244849
class spell_hasabel_caustic_slime : public SpellScriptLoader
{
    public:
        spell_hasabel_caustic_slime() : SpellScriptLoader("spell_hasabel_caustic_slime") { }

        enum eSpells
        {
            SpellCausticDetonation      = 244853,
            SpellCausticDetonationNpc   = 244864
        };

        class spell_hasabel_caustic_slime_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_caustic_slime_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode == AuraRemoveMode::AURA_REMOVE_BY_DEFAULT)
                    return;

                if (Unit* l_Target = GetTarget())
                {
                    l_Target->CastSpell(l_Target, eSpells::SpellCausticDetonation, true);

                    if (l_Target->GetMap()->IsMythic())
                        l_Target->CastSpell(l_Target, eSpells::SpellCausticDetonationNpc, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hasabel_caustic_slime_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_caustic_slime_AuraScript();
        }
};

/// Cloying Shadows - 245118
class spell_hasabel_cloying_shadows : public SpellScriptLoader
{
    public:
        spell_hasabel_cloying_shadows() : SpellScriptLoader("spell_hasabel_cloying_shadows") { }

        enum eSpell
        {
            SpellHungeringGloom = 245075
        };

        class spell_hasabel_cloying_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_cloying_shadows_AuraScript);

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellHungeringGloom, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hasabel_cloying_shadows_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_HEALING_PCT, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_cloying_shadows_AuraScript();
        }
};

/// Everburning Flames - 244613
/// Caustic Slime - 244849
/// Cloying Shadows - 245118
class spell_hasabel_chaotic_rift_handler : public SpellScriptLoader
{
    public:
        spell_hasabel_chaotic_rift_handler() : SpellScriptLoader("spell_hasabel_chaotic_rift_handler") { }

        enum eSpells
        {
            SpellEverburningFlames  = 244613,
            SpellCausticSlime       = 244849,
            SpellCloyingShadows     = 245118
        };

        class spell_hasabel_chaotic_rift_handler_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_chaotic_rift_handler_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    bool l_MustOpenRift = false;

                    switch (GetSpellInfo()->Id)
                    {
                        case eSpells::SpellEverburningFlames:
                        {
                            l_MustOpenRift = l_Target->HasAura(eSpells::SpellCausticSlime) && l_Target->HasAura(eSpells::SpellCloyingShadows);
                            break;
                        }
                        case eSpells::SpellCausticSlime:
                        {
                            l_MustOpenRift = l_Target->HasAura(eSpells::SpellEverburningFlames) && l_Target->HasAura(eSpells::SpellCloyingShadows);
                            break;
                        }
                        case eSpells::SpellCloyingShadows:
                        {
                            l_MustOpenRift = l_Target->HasAura(eSpells::SpellCausticSlime) && l_Target->HasAura(eSpells::SpellEverburningFlames);
                            break;
                        }
                        default:
                            break;
                    }

                    if (l_MustOpenRift)
                    {
                        InstanceScript* l_Instance = l_Target->GetInstanceScript();
                        if (!l_Instance)
                            return;

                        Creature* l_Boss = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::BossPortalKeeperHasabel));
                        if (!l_Boss || !l_Boss->IsAIEnabled)
                            return;

                        l_Boss->AI()->DoAction(0);
                    }
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::SpellCloyingShadows)
                    AfterEffectApply += AuraEffectApplyFn(spell_hasabel_chaotic_rift_handler_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_HEALING_PCT, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                else
                    AfterEffectApply += AuraEffectApplyFn(spell_hasabel_chaotic_rift_handler_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_chaotic_rift_handler_AuraScript();
        }
};

/// Mind Fog - 245099
class spell_hasabel_mind_fog_check : public SpellScriptLoader
{
    public:
        spell_hasabel_mind_fog_check() : SpellScriptLoader("spell_hasabel_mind_fog_check") { }

        class spell_hasabel_mind_fog_check_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hasabel_mind_fog_check_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMap()->IsLFR())
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hasabel_mind_fog_check_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hasabel_mind_fog_check_SpellScript();
        }
};

/// Hungering Gloom - 245075
class spell_hasabel_hungering_gloom : public SpellScriptLoader
{
    public:
        spell_hasabel_hungering_gloom() : SpellScriptLoader("spell_hasabel_hungering_gloom") { }

        class spell_hasabel_hungering_gloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_hungering_gloom_AuraScript);

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Target->IsPlayer())
                        p_Duration = 20 * TimeConstants::IN_MILLISECONDS;
                }
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_hasabel_hungering_gloom_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_hungering_gloom_AuraScript();
        }
};

/// Empowered Portal: Xoroth - 244316
class spell_hasabel_empowered_portal_xoroth : public SpellScriptLoader
{
    public:
        spell_hasabel_empowered_portal_xoroth() : SpellScriptLoader("spell_hasabel_empowered_portal_xoroth") { }

        enum eSpell
        {
            SpellEmpoweredPortalXorothVisual = 244487
        };

        class spell_hasabel_empowered_portal_xoroth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_empowered_portal_xoroth_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellEmpoweredPortalXorothVisual, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hasabel_empowered_portal_xoroth_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_empowered_portal_xoroth_AuraScript();
        }
};

/// Energize [DNT] - 247143
class spell_hasabel_energize_dnt : public SpellScriptLoader
{
    public:
        spell_hasabel_energize_dnt() : SpellScriptLoader("spell_hasabel_energize_dnt") { }

        class spell_hasabel_energize_dnt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hasabel_energize_dnt_AuraScript);

            float m_EnergyToAdd = 0.0f;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    switch (l_Target->GetMap()->GetDifficultyID())
                    {
                        case Difficulty::DifficultyRaidLFR:
                        case Difficulty::DifficultyRaidNormal:
                        {
                            m_EnergyToAdd += 2.6f;
                            break;
                        }
                        case Difficulty::DifficultyRaidHeroic:
                        {
                            m_EnergyToAdd += 3.4f;
                            break;
                        }
                        case Difficulty::DifficultyRaidMythic:
                        {
                            m_EnergyToAdd += 3.9f;
                            break;
                        }
                        default:
                            break;
                    }

                    int32 l_RealAdd = int32(m_EnergyToAdd);

                    m_EnergyToAdd -= l_RealAdd;

                    l_Target->EnergizeBySpell(l_Target, GetSpellInfo()->Id, l_RealAdd, Powers::POWER_ENERGY);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hasabel_energize_dnt_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hasabel_energize_dnt_AuraScript();
        }
};

/// Everburning Flames - 245153
class areatrigger_hasabel_everburning_flames : public AreaTriggerEntityScript
{
    public:
        areatrigger_hasabel_everburning_flames() : AreaTriggerEntityScript("areatrigger_hasabel_everburning_flames") { }

        enum eSpells
        {
            SpellEverburningLight   = 245157,
            SpellVisionReduced      = 245099
        };

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return true;

            p_Target->CastSpell(p_Target, eSpells::SpellEverburningLight, true);
            p_Target->RemoveAura(eSpells::SpellVisionReduced);
            return true;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (!p_Target->IsPlayer())
                return true;

            p_Target->RemoveAura(eSpells::SpellEverburningLight);

            if (!p_Target->GetMap()->IsLFR() && p_Target->FindNearestCreature(eCreatures::NpcShadowsOfNathreza, 49.0f))
                p_Target->CastSpell(p_Target, eSpells::SpellVisionReduced, true);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_hasabel_everburning_flames();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_portal_keeper_hasabel()
{
    /// Boss
    new boss_portal_keeper_hasabel();

    /// Creatures
    new npc_hasabel_collapsing_world();
    new npc_hasabel_transport_portal();
    new npc_hasabel_felcrush_portal();
    new npc_hasabel_vulcanar();
    new npc_hasabel_lady_dacidion();
    new npc_hasabel_felsilk_wrap();
    new npc_hasabel_lord_eilgar();
    new npc_hasabel_everburning_flames();
    new npc_hasabel_chaotic_rift();

    /// Spells
    new spell_hasabel_reality_tear();
    new spell_hasabel_transport_portal();
    new spell_hasabel_essence_transfer();
    new spell_hasabel_unstable_portal();
    new spell_hasabel_everburning_flames();
    new spell_hasabel_poison_essence();
    new spell_hasabel_felsilk_wrap();
    new spell_hasabel_caustic_slime();
    new spell_hasabel_cloying_shadows();
    new spell_hasabel_chaotic_rift_handler();
    new spell_hasabel_mind_fog_check();
    new spell_hasabel_hungering_gloom();
    new spell_hasabel_empowered_portal_xoroth();
    new spell_hasabel_energize_dnt();

    /// AreaTrigger
    new areatrigger_hasabel_everburning_flames();
}
#endif
