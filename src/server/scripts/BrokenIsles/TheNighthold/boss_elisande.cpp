////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eScriptData
{
    DataSpanningSingularity = 1,
    EncounterPhase          = 2,
    CanBounceTwice          = 3,
    DataArcaneticRing       = 4,
    DataEpochericOrb        = 5,
    DataDelphuricBeam       = 6,
    Trash                   = 7,
    InfiniteWhelpling       = 8,
    ValidateAchievement     = 9,
    DataSpanningSingularityWithoutMissile = 10
};

Position const g_RoomCenter = { 282.3180f, 3135.5100f, 226.84f, static_cast<float>(M_PI / 12.0f) };

/// Last Udpate 7.1.5 Build 23420
/// Elisande - 106643
class boss_elisande : public CreatureScript
{
    public:
        boss_elisande() : CreatureScript("boss_elisande") { }

        struct boss_elisandeAI : BossAI
        {
            boss_elisandeAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataElisande)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                m_TrashSetInit = false;

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> l_Trashes;
                    me->GetCreatureListWithEntryInGrid(l_Trashes, eCreatures::NpcMidnightSiphoner, 40.0f);
                    me->GetCreatureListWithEntryInGrid(l_Trashes, eCreatures::NpcTwilightStardancer, 40.0f);
                    me->GetCreatureListWithEntryInGrid(l_Trashes, eCreatures::NpcResoluteCourtesan, 40.0f);
                    me->GetCreatureListWithEntryInGrid(l_Trashes, eCreatures::NpcAstralFarseer, 40.0f);
                    for (auto l_Itr = l_Trashes.begin(); l_Itr != l_Trashes.end(); ++l_Itr)
                    {
                        if (!(*l_Itr)->isAlive() || (*l_Itr)->GetPositionZ() < 220.0f || (*l_Itr)->GetPositionZ() > 230.0f)
                            continue;

                        m_TrashSet.insert((*l_Itr)->GetGUID());
                    }

                    m_TrashSetInit = true;

                    if (!m_TrashSet.empty())
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                    GameObject* l_GameObject = me->FindNearestGameObject(eGameObjects::GoElisandeNightwell, 30.0f);
                    if (l_GameObject)
                        l_GameObject->SetGoState(GOState::GO_STATE_READY);
                });

                me->SetDisableGravity(false);
                me->SetCanFly(false, true);
                me->ClearUnitState(UnitState::UNIT_STATE_ROOT);

                Reset();
            }

            enum eAchievements
            {
                Infinitesimal = 10699
            };

            enum eSpells
            {
                LeavingTheNightwell     = 208863,
                LeavingTheNightwellEnd  = 208865,
                TimeStop                = 208944,
                EtchedInTime            = 208861,
                Disappear               = 209026,

                SummonTimeElementalSlow = 209005,
                SummonTimeElementalFast = 209007,

                ArcaneticRing           = 208808,

                SpanningSingularity     = 209168,
                SpanningSingularityEcho = 209170,
                SpanningSingularityAreatrigger = 209500,

                CascadentStar           = 229738,

                Ablation                = 209615,

                EpochericOrb            = 213739,
                EpochericOrbPortal      = 210275,

                DelphuricBeamPeriodic5s = 213716,
                DelphuricBeamPeriodic4s = 214278,
                DelphuricBeamPeriodic3s = 225654,
                DelphuricBeam           = 209242,

                AblatingExplosion       = 209973,

                ConflexingBurst         = 209597,
                PermeliativeTorment     = 225653,

                AblativePulse           = 209971,
                Ablated                 = 211887,

                Terminate               = 229886,

                SpellFastBubbleAT       = 209882,
                SpellSlowBubbleAT       = 209881
            };

            enum eEvents
            {
                EventLeavingTheNightwell = 1,
                EventSummonTimeElementalSlow,
                EventSummonTimeElementalFast,
                EventArcaneticRing,
                EventSpanningSingularity,
                EventCascadentStar,
                EventAblation,
                EventEpochericOrb,
                EventDelphuricBeam,
                EventAblatingExplosion,
                EventConflexingBurst,
                EventPermeliativeTorment,
                EventAblativePulse,
                EventBerserk,
                EventTerminate,
                EventTeleport
            };

            enum eTimers
            {
                TimerBerserk        = 198 * IN_MILLISECONDS,
                TimerBerserkPhase3  = 193 * IN_MILLISECONDS,
                TimerTerminate      = 300 * IN_MILLISECONDS
            };

            enum eTalks
            {
                TalkAggro,
                TalkSpanningSingularity,
                TalkSpanningSingularityEcho,
                TalkArcaneticRing,
                TalkArcaneticRingEcho,
                TalkPhase2,
                TalkEpochericOrb,
                TalkEpochericOrbEcho,
                TalkDelphuricBeam,
                TalkDelphuricBeamEcho,
                TalkPhase3,
                TalkTaunt,
                TalkTauntEcho,
                TalkConflexiveBurst,
                TalkConflexiveBurstEcho,
                TalkPhase3Half,
                TalkKilledPlayer,
                TalkWipe,
                TalkDeath,
                TalkAmbianceIntro
            };

            std::map<Difficulty, std::map<uint32, std::vector<uint32>>> m_TimerData =
            {
                {
                    Difficulty::DifficultyRaidLFR,
                    {
                        { eEvents::EventSummonTimeElementalSlow, { 5000, 62000, 40000, 55000 } },
                        { eEvents::EventSummonTimeElementalFast, { 65000 } },
                        { eEvents::EventArcaneticRing, { 21000, 30000, 37000, 35000, 35000 } },
                        { eEvents::EventSpanningSingularity, { 17000, 57000, 30000 } },
                        { eEvents::EventEpochericOrb, { 40000, 37000 } },
                        { eEvents::EventAblatingExplosion, {12100, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000 } },
                        { eEvents::EventDelphuricBeam, { 35000, 77000, 25000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidNormal,
                    {
                        { eEvents::EventSummonTimeElementalSlow, { 5000, 49000, 41000, 60000 } },
                        { eEvents::EventSummonTimeElementalFast, { 8000, 71000, 101000 } },
                        { eEvents::EventArcaneticRing, { 34000, 31000, 76000, 50000, 40000, 15000, 30000 } },
                        { eEvents::EventSpanningSingularity, { 23000, 36000, 46000, 65000 } },
                        { eEvents::EventEpochericOrb, { 18000, 56000, 31000, 85000 } },
                        { eEvents::EventDelphuricBeam, { 59000, 26000, 40000, 110000} },
                        { eEvents::EventAblatingExplosion, { 12100, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000, 21000 } },
                        { eEvents::EventPermeliativeTorment, { 23000, 41000, 106000 } },
                        { eEvents::EventConflexingBurst, { 48000, 67000, 50000, 45000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidHeroic,
                    {
                        { eEvents::EventSummonTimeElementalSlow, { 5000, 49000, 52000, 60000 } },
                        { eEvents::EventSummonTimeElementalFast, { 8000, 88000, 95000, 20000 } },
                        { eEvents::EventArcaneticRing, { 35000, 40000, 10000, 63000, 10000, 45000, 30000 } },
                        { eEvents::EventSpanningSingularity, { 25000, 36000, 57000, 65000 } },
                        { eEvents::EventEpochericOrb, { 18000, 76000, 37000, 70000, 15000, 15000, 30000, 15000 } },
                        { eEvents::EventDelphuricBeam, { 59000, 57000, 60000, 70000 } },
                        { eEvents::EventAblatingExplosion, { 12000, 20700, 20600, 22000, 20700, 25500, 20600, 20600, 20600, 20600, 25500, 20600, 20600, 20600, 20600, 25500, 20600, 20600, 20600 } },
                        { eEvents::EventPermeliativeTorment, { 23000, 61000, 37000, 60000 } },
                        { eEvents::EventConflexingBurst, { 48000, 52000, 56000, 65000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 } }
                    }
                },
                {
                    Difficulty::DifficultyRaidMythic,
                    {
                        { eEvents::EventSummonTimeElementalSlow, { 5000, 49000, 52000, 60000 } },
                        { eEvents::EventSummonTimeElementalFast, { 8000, 88000, 95000, 20000 } },
                        { eEvents::EventArcaneticRing, { 35000, 40000, 10000, 63000, 10000 } },
                        { eEvents::EventSpanningSingularity, { 25000, 36000, 57000, 65000 } },
                        { eEvents::EventEpochericOrb, { 18000, 76000, 37000, 70000, 15000, 15000, 30000, 15000 } },
                        { eEvents::EventDelphuricBeam, { 59000, 57000, 60000, 70000 } },
                        { eEvents::EventAblatingExplosion, { 12000, 20700, 20600, 22000, 20700, 25500, 20600, 20600, 20600, 20600, 25500, 20600, 20600, 20600, 20600, 25500, 20600, 20600, 20600 } },
                        { eEvents::EventPermeliativeTorment, { 23000, 61000, 37000, 60000 } },
                        { eEvents::EventConflexingBurst, { 48000, 52000, 56000, 65000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 } },
                    }
                }
            };

            std::map<Difficulty, std::map<uint32, std::queue<uint32>>> m_Timers;

            EventMap m_MainEvents;
            uint32 m_CurrentPhase;

            std::set<uint64> m_TrashSet;
            bool m_TrashSetInit;
            bool m_CombatStart;

            bool m_IsAchievementCompleted;

            std::map<uint32, std::vector<std::tuple<uint32, Position, Position>>> m_SpellSaves;
            std::map<uint32, std::queue<std::tuple<uint32, Position, Position>>> m_SpellQueues;

            uint32 m_DurationForPhase;
            bool m_BouncingTwice;

            bool m_TalkedP3;

            std::vector<std::pair<uint32, Position>> m_BlueBubblesPositions;
            std::queue<Position> m_NextBlueBubble;

            void InitTimers()
            {
                for (auto& l_Itr : m_TimerData)
                {
                    for (auto& l_SecondItr : l_Itr.second)
                    {
                        while (!m_Timers[l_Itr.first][l_SecondItr.first].empty())
                            m_Timers[l_Itr.first][l_SecondItr.first].pop();

                        for (uint32& l_Timer : l_SecondItr.second)
                            m_Timers[l_Itr.first][l_SecondItr.first].push(l_Timer);
                    }
                }
            }

            void AutoSchedule(uint32 p_EventID)
            {
                switch (p_EventID)
                {
                    case eEvents::EventCascadentStar:
                        events.ScheduleEvent(eEvents::EventCascadentStar, 15000);
                        return;
                    case eEvents::EventAblation:
                        events.ScheduleEvent(eEvents::EventAblation, 6000);
                        return;
                    case eEvents::EventAblativePulse:
                        events.ScheduleEvent(eEvents::EventAblativePulse, 10000);
                        return;
                    default:
                        break;
                }

                if (!me->GetMap())
                    return;

                auto l_Itr = m_Timers.find(me->GetMap()->GetDifficultyID());
                if (l_Itr == m_Timers.end())
                    return;

                auto l_SecondItr = l_Itr->second.find(p_EventID);
                if ((l_SecondItr == l_Itr->second.end()) || l_SecondItr->second.empty())
                    return;

                if (l_SecondItr->second.empty())
                    return;

                events.ScheduleEvent(p_EventID, l_SecondItr->second.front());
                l_SecondItr->second.pop();
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (!p_Victim->IsPlayer())
                    return;

                if (instance && instance->IsWipe())
                    Talk(eTalks::TalkWipe);
                else
                    Talk(eTalks::TalkKilledPlayer);
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return !me->isInCombat() && m_TrashSet.empty();
            }

            void Reset() override
            {
                _Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);
                DoCastAOE(eSpells::EtchedInTime, true);

                if (m_TrashSet.empty())
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetDisableGravity(true);
                    me->SetCanFly(true, true);
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                    me->NearTeleportTo(*me);
                });

                m_CombatStart = false;
                m_CurrentPhase = 0;

                for (auto& l_Pair : m_SpellSaves)
                    l_Pair.second.clear();

                m_DurationForPhase = 0;
                m_BouncingTwice = false;
                m_TalkedP3 = false;
                m_IsAchievementCompleted = false;

                for (auto& l_Pair : m_SpellQueues)
                {
                    while (!l_Pair.second.empty())
                        l_Pair.second.pop();
                }

                for (auto& l_Pair : m_SpellSaves)
                    l_Pair.second.clear();

                m_BlueBubblesPositions.clear();
                while (!m_NextBlueBubble.empty())
                    m_NextBlueBubble.pop();

                std::list<Player*> l_Players;
                me->GetPlayerListInGrid(l_Players, 130.0f);

                for (Player* l_Player : l_Players)
                {
                    l_Player->RemoveAurasDueToSpell(eSpells::TimeStop);
                    l_Player->RemoveAurasDueToSpell(eSpells::Ablated);
                }

                AddTimedDelayedOperation(4 * IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> p_Elementals;
                    me->GetCreatureListWithEntryInGrid(p_Elementals, eCreatures::RecursiveElemental, 100.0f);
                    me->GetCreatureListWithEntryInGrid(p_Elementals, eCreatures::ExpedientElemental, 100.0f);

                    for (Creature* l_Creature : p_Elementals)
                        l_Creature->DespawnOrUnsummon();
                });

                m_MainEvents.Reset();
                m_MainEvents.ScheduleEvent(eEvents::EventTeleport, 2 * IN_MILLISECONDS);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();
            }

            void DefaultEvents()
            {
                events.Reset();
                InitTimers();

                bool l_LFR = me->GetMap() && me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidLFR;

                switch (m_CurrentPhase)
                {
                    case 1:
                    {
                        AutoSchedule(eEvents::EventArcaneticRing);
                        AutoSchedule(eEvents::EventSpanningSingularity);
                        AutoSchedule(eEvents::EventAblation);
                        if (IsMythic())
                        {
                            m_MainEvents.CancelEvent(eEvents::EventBerserk);
                            m_MainEvents.ScheduleEvent(eEvents::EventBerserk, eTimers::TimerBerserk);
                        }
                        else
                        {
                            m_MainEvents.CancelEvent(eEvents::EventTerminate);
                            m_MainEvents.ScheduleEvent(eEvents::EventTerminate, eTimers::TimerTerminate);
                        }

                        break;
                    }
                    case 2:
                    {
                        AutoSchedule(eEvents::EventEpochericOrb);
                        AutoSchedule(eEvents::EventAblatingExplosion);
                        if (!l_LFR)
                            AutoSchedule(eEvents::EventDelphuricBeam);

                        if (IsMythic())
                        {
                            m_MainEvents.CancelEvent(eEvents::EventBerserk);
                            m_MainEvents.ScheduleEvent(eEvents::EventBerserk, eTimers::TimerBerserk);
                        }
                        else
                        {
                            m_MainEvents.CancelEvent(eEvents::EventTerminate);
                            m_MainEvents.ScheduleEvent(eEvents::EventTerminate, eTimers::TimerTerminate);
                        }

                        break;
                    }
                    case 3:
                    {
                        AutoSchedule(eEvents::EventConflexingBurst);
                        AutoSchedule(eEvents::EventPermeliativeTorment);
                        AutoSchedule(eEvents::EventAblativePulse);
                        if (l_LFR)
                            AutoSchedule(eEvents::EventDelphuricBeam);

                        if (IsMythic())
                        {
                            m_MainEvents.CancelEvent(eEvents::EventBerserk);
                            m_MainEvents.ScheduleEvent(eEvents::EventBerserk, eTimers::TimerBerserkPhase3);
                        }
                        else
                        {
                            m_MainEvents.CancelEvent(eEvents::EventTerminate);
                            m_MainEvents.ScheduleEvent(eEvents::EventTerminate, eTimers::TimerTerminate);
                        }

                        break;
                    }
                    default:
                        break;
                }

                while (!m_NextBlueBubble.empty())
                    m_NextBlueBubble.pop();

                for (auto const& l_Pair : m_BlueBubblesPositions)
                    m_NextBlueBubble.push(l_Pair.second);

                AutoSchedule(eEvents::EventSummonTimeElementalSlow);
                AutoSchedule(eEvents::EventSummonTimeElementalFast);

                AutoSchedule(eEvents::EventCascadentStar);
            }

            void ChangePhase()
            {
                m_CurrentPhase++;
                m_DurationForPhase = 0;

                for (auto& l_Pair : m_SpellQueues)
                {
                    while (!l_Pair.second.empty())
                        l_Pair.second.pop();
                }

                for (auto& l_Pair : m_SpellSaves)
                {
                    for (std::tuple<uint32, Position, Position> l_Tuple : l_Pair.second)
                        m_SpellQueues[l_Pair.first].push(l_Tuple);
                }

                DefaultEvents();

                if (m_CurrentPhase > 1)
                    me->ModifyPower(Powers::POWER_ENERGY, -50 * me->GetPowerCoeff(POWER_ENERGY));

                if  (m_CurrentPhase == 1)
                    Talk(eTalks::TalkAggro);

                me->DespawnAreaTriggersInArea({ eSpells::SpellFastBubbleAT, eSpells::SpellSlowBubbleAT }, 200.0f, false);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::TimeStop:
                    {
                        summons.DespawnAll();
                        DoCastAOE(eSpells::EtchedInTime, true);
                        DoCastAOE(eSpells::Disappear, true);
                        m_MainEvents.ScheduleEvent(eEvents::EventLeavingTheNightwell, 7 * IN_MILLISECONDS);

                        me->SetDisableGravity(true);
                        me->SetCanFly(true, true);
                        me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                        me->SetHealth(me->GetMaxHealth());
                        me->NearTeleportTo(me->GetHomePosition());
                        GameObject* l_GameObject = me->FindNearestGameObject(eGameObjects::GoElisandeNightwell, 30.0f);
                        if (!l_GameObject)
                            break;

                        l_GameObject->SetGoState(GOState::GO_STATE_READY);
                        break;
                    }
                    case eSpells::LeavingTheNightwellEnd:
                    {
                        if (me->GetMap()->IsHeroicOrMythic())
                        {
                            AddTimedDelayedOperation(250, [this]() -> void
                            {
                                me->CastSpell(me, eSpells::SpanningSingularityAreatrigger, true);
                                me->GetAI()->SetGUID(me->GetGUID(), eScriptData::DataSpanningSingularityWithoutMissile);
                            });
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
                    case eScriptData::ValidateAchievement:
                        m_IsAchievementCompleted = true;
                        break;
                    default:
                        break;
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID) override
            {
                switch (p_ID)
                {
                    case eScriptData::DataSpanningSingularityWithoutMissile:
                    {
                        m_SpellSaves[p_ID].push_back(std::tuple<uint32, Position, Position>(m_DurationForPhase, me->GetPosition(), me->GetPosition()));
                        break;
                    }
                    case eScriptData::DataSpanningSingularity:
                    case eScriptData::DataEpochericOrb:
                    {
                        Unit* l_Unit = sObjectAccessor->GetUnit(*me, p_Guid);
                        if (!l_Unit || m_SpellSaves[p_ID].empty())
                            break;

                        std::tuple<uint32, Position, Position> l_Tuple = m_SpellSaves[p_ID].back();
                        m_SpellSaves[p_ID].pop_back();
                        m_SpellSaves[p_ID].push_back(std::tuple<uint32, Position, Position>(std::get<0>(l_Tuple), l_Unit->GetPosition(), std::get<2>(l_Tuple)));

                        break;
                    }
                    case eScriptData::DataDelphuricBeam:
                    {
                        if (m_CurrentPhase != 2)
                            break;

                        m_SpellSaves[p_ID].push_back(std::tuple<uint32, Position, Position>(m_DurationForPhase, {}, me->GetPosition()));
                        break;
                    }
                    case eScriptData::Trash:
                    {
                        m_TrashSet.erase(p_Guid);
                        break;
                    }
                    case eScriptData::InfiniteWhelpling:
                    {
                        Creature* l_Whelpling = Creature::GetCreature(*me, p_Guid);
                        if (!l_Whelpling)
                            break;

                        Position l_TargetPos = *l_Whelpling;
                        l_Whelpling->DespawnOrUnsummon();

                        me->SummonCreature(eCreatures::NpcInfiniteDrakeling, l_TargetPos);
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eScriptData::EncounterPhase:
                        return m_CurrentPhase;
                    case eScriptData::CanBounceTwice:
                    {
                        if (m_CurrentPhase > 1)
                            return 0;

                        m_BouncingTwice = !m_BouncingTwice;

                        if (m_BouncingTwice) ///< Check inverted because boolean is inverted before check
                            return 1;

                        return 0;
                    }
                    default:
                        break;
                }

                return 0;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_IsAchievementCompleted && instance && !IsLFR())
                    instance->DoCompleteAchievement(eAchievements::Infinitesimal);

                std::list<Player*> l_Players;
                me->GetPlayerListInGrid(l_Players, 130.0f);

                for (Player* l_Player : l_Players)
                    l_Player->RemoveAurasDueToSpell(eSpells::Ablated);

                Talk(eTalks::TalkDeath);
                _JustDied();
            }

            void OnTaunt(Unit* p_Taunter) override
            {
                uint32 p_Damage = 0;
                DamageTaken(nullptr, p_Damage, nullptr);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_DamagingSpell*/) override
            {
                if (!m_CombatStart)
                {
                    Player* l_Player = me->SelectNearestPlayerNotGM(100.0f);
                    if (!l_Player || !me->IsValidAttackTarget(l_Player))
                    {
                        EnterEvadeMode();
                        return;
                    }

                    m_MainEvents.ScheduleEvent(eEvents::EventLeavingTheNightwell, 0);

                    m_CombatStart = true;
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                }

                if (m_CurrentPhase == 3 && !m_TalkedP3 && me->HealthBelowPctDamaged(50, p_Damage))
                {
                    Talk(eTalks::TalkPhase3Half);
                    m_TalkedP3 = true;
                }

                if (!me->GetPower(Powers::POWER_ENERGY) || !me->HealthBelowPctDamaged(10, p_Damage))
                    return;

                events.Reset();
                m_MainEvents.Reset();
                switch (m_CurrentPhase)
                {
                    case 1:
                        Talk(eTalks::TalkPhase2);
                        break;
                    case 2:
                        Talk(eTalks::TalkPhase3);
                        break;
                    default:
                        break;
                }

                DoCastAOE(eSpells::TimeStop, false);
            }

            Position GetBlueBubbleDest()
            {
                if (m_NextBlueBubble.empty())
                {
                    Position l_Dest;
                    std::set<uint32> l_AllowedAngles = { 0, 4, 8 };
                    std::vector<uint32> l_SecondaryAngle = { 2, 6, 10 };
                    for (auto l_Pair : m_BlueBubblesPositions)
                        l_AllowedAngles.erase(l_Pair.first);

                    uint32 l_Angle;
                    if (l_AllowedAngles.empty())
                        l_Angle = l_SecondaryAngle[urand(0, 2)];
                    else
                    {
                        auto l_Itr = l_AllowedAngles.begin();
                        std::advance(l_Itr, urand(0, l_AllowedAngles.size() - 1));
                        l_Angle = *l_Itr;
                    }

                    g_RoomCenter.SimplePosXYRelocationByAngle(l_Dest, 33.0f, static_cast<float>(l_Angle) * M_PI / 6.0f );
                    m_BlueBubblesPositions.push_back(std::make_pair(l_Angle, l_Dest));
                    return l_Dest;
                }

                Position l_Dest = m_NextBlueBubble.front();
                m_NextBlueBubble.pop();
                return l_Dest;
            }

            void UpdateSpellQueues(uint32 const p_Diff)
            {
                m_DurationForPhase += p_Diff;

                for (auto& l_Pair : m_SpellQueues)
                {
                    while (!l_Pair.second.empty() && std::get<0>(l_Pair.second.front()) < m_DurationForPhase)
                    {
                        Creature* l_Echo = me->SummonCreature(eCreatures::ElisandeEcho, std::get<2>(l_Pair.second.front()));
                        if (!l_Echo || !l_Echo->IsAIEnabled)
                            continue;

                        switch (l_Pair.first)
                        {
                            case eScriptData::DataArcaneticRing:
                            {
                                l_Echo->CastSpell(l_Echo, eSpells::ArcaneticRing, false);
                                sCreatureTextMgr->SendChat(l_Echo, eTalks::TalkArcaneticRingEcho, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
                                break;
                            }
                            case eScriptData::DataSpanningSingularity:
                            {
                                if (me->GetMap() && me->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                                {
                                    m_SpellSaves[eScriptData::DataSpanningSingularity].push_back(std::tuple<uint32, Position, Position>(m_DurationForPhase, {}, l_Echo->GetPosition()));
                                    l_Echo->CastSpell(l_Echo, eSpells::SpanningSingularity, false);
                                }

                                l_Echo->CastSpell(std::get<1>(l_Pair.second.front()), eSpells::SpanningSingularityEcho, true);
                                sCreatureTextMgr->SendChat(l_Echo, eTalks::TalkSpanningSingularityEcho, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
                                break;
                            }
                            case eScriptData::DataSpanningSingularityWithoutMissile:
                            {
                                l_Echo->CastSpell(std::get<1>(l_Pair.second.front()), eSpells::SpanningSingularityAreatrigger, true);
                                break;
                            }
                            case eScriptData::DataEpochericOrb:
                            {
                                l_Echo->CastSpell(l_Echo, eSpells::EpochericOrb, false);

                                sCreatureTextMgr->SendChat(l_Echo, eTalks::TalkEpochericOrbEcho, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
                                std::tuple<uint32, Position, Position> l_Tuple = l_Pair.second.front();
                                l_Echo->CastSpell(l_Echo, eSpells::EpochericOrb, false);
                                l_Echo->AddDelayedEvent([l_Echo, l_Tuple]() -> void
                                {
                                    l_Echo->CastSpell(std::get<1>(l_Tuple), eSpells::EpochericOrbPortal, false);
                                    Position l_SummonDest = std::get<1>(l_Tuple);
                                    l_SummonDest.m_positionZ = 244.55f;
                                    Creature* l_Creature = l_Echo->SummonCreature(eCreatures::NpcEpochericOrb, l_SummonDest);
                                }, 2 * IN_MILLISECONDS);
                                break;
                            }
                            case eScriptData::DataDelphuricBeam:
                            {
                                l_Echo->CastSpell(l_Echo, eSpells::DelphuricBeam, false);
                                sCreatureTextMgr->SendChat(l_Echo, eTalks::TalkDelphuricBeamEcho, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
                                break;
                            }
                            default:
                                break;
                        }

                        l_Pair.second.pop();
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_MainEvents.Update(p_Diff);
                UpdateOperations(p_Diff);
                UpdateSpellQueues(p_Diff);

                if (me->GetExactDist2d(&g_RoomCenter) > 57.5f)
                    EnterEvadeMode();

                switch (m_MainEvents.ExecuteEvent())
                {
                    case eEvents::EventLeavingTheNightwell:
                    {
                        ChangePhase();

                        DoCastAOE(eSpells::LeavingTheNightwell);
                        AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetDisableGravity(false);
                            me->SetCanFly(false, true);
                            me->ClearUnitState(UnitState::UNIT_STATE_ROOT);

                            GameObject* l_GameObject = me->FindNearestGameObject(eGameObjects::GoElisandeNightwell, 30.0f);
                            if (!l_GameObject)
                                return;

                            l_GameObject->SetGoState(GOState::GO_STATE_ACTIVE);
                        });

                        break;
                    }
                    case eEvents::EventBerserk:
                    {
                        DoCastAOE(eSharedSpells::SpellBerserk, true);
                        break;
                    }
                    case eEvents::EventTerminate:
                    {
                        DoCastAOE(eSpells::Terminate);
                        break;
                    }
                    case eEvents::EventTeleport:
                    {
                        if (!me->isInCombat() && me->isAlive())
                        {
                            me->SetDisableGravity(true);
                            me->SetCanFly(true, true);
                            me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                            me->NearTeleportTo(*me);
                        }

                        m_MainEvents.ScheduleEvent(eEvents::EventTeleport, 2 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                if (m_TrashSetInit && m_TrashSet.empty())
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                    m_TrashSetInit = false;
                    Talk(eTalks::TalkAmbianceIntro);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSummonTimeElementalSlow:
                    {
                        me->CastSpell(GetBlueBubbleDest(), eSpells::SummonTimeElementalSlow, false);

                        AutoSchedule(eEvents::EventSummonTimeElementalSlow);
                        break;
                    }
                    case eEvents::EventSummonTimeElementalFast:
                    {
                        Position l_Dest;
                        g_RoomCenter.SimplePosXYRelocationByAngle(l_Dest, 33.0f, frand(0.0f, 2.0f * M_PI));
                        me->CastSpell(l_Dest, eSpells::SummonTimeElementalFast, false);

                        AutoSchedule(eEvents::EventSummonTimeElementalFast);
                        break;
                    }
                    case eEvents::EventArcaneticRing:
                    {
                        Position l_Dest;
                        float l_Angle = g_RoomCenter.GetAngle(me);
                        g_RoomCenter.SimplePosXYRelocationByAngle(l_Dest, frand(20.0f, 25.0f), frand(-M_PI /4.0f, M_PI / 4.0f) + l_Angle + M_PI);
                        me->NearTeleportTo(l_Dest);
                        me->AddUnitState(UNIT_STATE_ROOT);
                        SetCombatMovement(false);

                        AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->ClearUnitState(UNIT_STATE_ROOT);
                            SetCombatMovement(true);
                        });

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            DoCastAOE(eSpells::ArcaneticRing, false);
                            AutoSchedule(eEvents::EventArcaneticRing);
                            Talk(eTalks::TalkArcaneticRing);

                            if (!me->GetMap())
                                return;

                            switch (me->GetMap()->GetDifficultyID())
                            {
                                case Difficulty::DifficultyRaidHeroic:
                                case Difficulty::DifficultyRaidMythic:
                                {
                                    if (m_CurrentPhase == 1)
                                        m_SpellSaves[eScriptData::DataArcaneticRing].push_back(std::tuple<uint32, Position, Position>(m_DurationForPhase, me->GetPosition(), me->GetPosition()));

                                    break;
                                }
                                default:
                                    break;
                            }
                        });
                        break;
                    }
                    case eEvents::EventSpanningSingularity:
                    {
                        for (uint32 l_I = 0; l_I < std::min<uint32>(2, me->getThreatManager().GetThreatList().size()); ++l_I)
                        {
                            m_SpellSaves[eScriptData::DataSpanningSingularity].push_back(std::tuple<uint32, Position, Position>(m_DurationForPhase, {}, me->GetPosition()));
                            DoCastAOE(eSpells::SpanningSingularity, false);
                        }

                        AutoSchedule(eEvents::EventSpanningSingularity);
                        Talk(eTalks::TalkSpanningSingularity);
                        break;
                    }
                    case eEvents::EventCascadentStar:
                    {
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM);
                        if (!l_Target)
                            break;

                        me->CastSpell(l_Target, eSpells::CascadentStar, false);

                        AutoSchedule(eEvents::EventCascadentStar);
                        break;
                    }
                    case eEvents::EventAblation:
                    {
                        DoCastVictim(eSpells::Ablation, false);
                        AutoSchedule(eEvents::EventAblation);
                        break;
                    }
                    case eEvents::EventEpochericOrb:
                    {
                        m_SpellSaves[eScriptData::DataEpochericOrb].push_back(std::tuple<uint32, Position, Position>(m_DurationForPhase, {}, me->GetPosition()));
                        DoCastAOE(eSpells::EpochericOrb, false);
                        AutoSchedule(eEvents::EventEpochericOrb);
                        Talk(eTalks::TalkEpochericOrb);
                        break;
                    }
                    case eEvents::EventDelphuricBeam:
                    {
                        DoCastAOE(eSpells::DelphuricBeamPeriodic4s, false);
                        AutoSchedule(eEvents::EventDelphuricBeam);
                        Talk(eTalks::TalkDelphuricBeam);
                        break;
                    }
                    case eEvents::EventAblatingExplosion:
                    {
                        DoCastVictim(eSpells::AblatingExplosion, false);
                        AutoSchedule(eEvents::EventAblatingExplosion);
                        break;
                    }
                    case eEvents::EventConflexingBurst:
                    {
                        DoCastAOE(eSpells::ConflexingBurst, false);
                        AutoSchedule(eEvents::EventConflexingBurst);
                        Talk(eTalks::TalkConflexiveBurst);
                        break;
                    }
                    case eEvents::EventPermeliativeTorment:
                    {
                        DoCastAOE(eSpells::PermeliativeTorment, false);
                        AutoSchedule(eEvents::EventPermeliativeTorment);
                        break;
                    }
                    case eEvents::EventAblativePulse:
                    {
                        DoCastVictim(eSpells::AblativePulse, false);
                        AutoSchedule(eEvents::EventAblativePulse);
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
            return new boss_elisandeAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Elisande Echo - 106682
class npc_elisande_echo : public CreatureScript
{
    public:
        npc_elisande_echo() : CreatureScript("npc_elisande_echo") { }

        struct npc_elisande_echoAI : public ScriptedAI
        {
            npc_elisande_echoAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetVisible(false);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_elisande_echoAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Leaving The Nightwell - 208863
class spell_elisande_leaving_the_nightwell : public SpellScriptLoader
{
    public:
        spell_elisande_leaving_the_nightwell() : SpellScriptLoader("spell_elisande_leaving_the_nightwell") { }

        class spell_elisande_leaving_the_nightwell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_leaving_the_nightwell_SpellScript);

            enum eSpells
            {
                LeavingTheNightwell = 208866
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::LeavingTheNightwell, true);

                WorldLocation l_Dest = *l_Target;
                l_Caster->AddDelayedEvent([l_Caster, l_Dest]() -> void
                {
                    l_Caster->NearTeleportTo(l_Dest);
                }, 200);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (p_Targets.empty() || !l_Caster)
                    return;

                std::list<Unit*> l_FinalTargets;
                for (WorldObject* l_Target : p_Targets)
                {
                    Unit* l_UnitTarget = l_Target->ToUnit();
                    if (!l_UnitTarget)
                        continue;

                    l_FinalTargets.push_back(l_UnitTarget);
                }

                if (l_FinalTargets.empty())
                    return;

                l_FinalTargets.sort(JadeCore::DistanceOrderPred(l_Caster));
                l_FinalTargets.resize(1);

                p_Targets.clear();
                p_Targets.push_back(l_FinalTargets.front());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elisande_leaving_the_nightwell_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_elisande_leaving_the_nightwell_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_leaving_the_nightwell_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Time Stop - 208944
class spell_elisande_time_stop : public SpellScriptLoader
{
    public:
        spell_elisande_time_stop() : SpellScriptLoader("spell_elisande_time_stop") { }

        class spell_elisande_time_stop_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_time_stop_AuraScript);

            enum eSpells
            {
                TimeLayerChanges = 209030
            };

            void HandleScreenEffect(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::TimeLayerChanges, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_elisande_time_stop_AuraScript::HandleScreenEffect, EFFECT_1, SPELL_AURA_MOD_TIME_RATE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_time_stop_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Leaving The Nightwell - 208863
class spell_elisande_summon_time_elemental : public SpellScriptLoader
{
    public:
        spell_elisande_summon_time_elemental() : SpellScriptLoader("spell_elisande_summon_time_elemental") { }

        class spell_elisande_summon_time_elemental_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_summon_time_elemental_SpellScript);

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Target = GetExplTargetDest();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                l_Caster->SummonCreature(l_SpellInfo->Effects[EFFECT_0].MiscValue, *l_Target);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_elisande_summon_time_elemental_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_summon_time_elemental_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Recursive Elemental - 105299
/// Expedient Elemental - 105301
class npc_elisande_time_elemental : public CreatureScript
{
    public:
        npc_elisande_time_elemental() : CreatureScript("npc_elisande_time_elemental") { }

        struct npc_elisande_time_elementalAI : public LegionCombatAI
        {
            npc_elisande_time_elementalAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

            enum eEvents
            {
                EventDeath = 2
            };

            void Reset() override
            {
                LegionCombatAI::Reset();

                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventDeath:
                    {
                        Creature* l_Elisande = me->FindNearestCreature(eCreatures::Elisande, 150.0f);
                        if (!l_Elisande)
                            return false;

                        l_Elisande->CastSpell(p_Target, GetEventData(p_EventID).SpellID, TriggerCastFlags(TRIGGERED_IGNORE_POWER_AND_REAGENT_COST | TRIGGERED_IGNORE_LOS));

                        return true;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }

            bool CanChase() override
            {
                return me->GetEntry() != eCreatures::RecursiveElemental;
            }

            bool CanAutoAttack() override
            {
                return me->GetEntry() != eCreatures::RecursiveElemental;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_elisande_time_elementalAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Arcanetic Ring - 208808
class spell_elisande_arcanetic_ring : public SpellScriptLoader
{
    public:
        spell_elisande_arcanetic_ring() : SpellScriptLoader("spell_elisande_arcanetic_ring") { }

        class spell_elisande_arcanetic_ring_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_arcanetic_ring_SpellScript);

            enum eSpells
            {
                ArcaneticRing = 208666
            };

            std::array<Position, 25> m_Spawns =
            {{
                { 332.7638f, 3107.938f, 226.8453f },
                { 324.2182f, 3096.176f, 226.8453f },
                { 338.1158f, 3150.304f, 226.8453f },
                { 285.5798f, 3077.994f, 226.8453f },
                { 299.8610f, 3080.719f, 226.8453f },
                { 339.9380f, 3135.880f, 226.8453f },
                { 338.1158f, 3121.456f, 226.8453f },
                { 313.0159f, 3086.909f, 226.8453f },
                { 332.7638f, 3163.822f, 226.8453f },
                { 271.0699f, 3078.907f, 226.8453f },
                { 324.2182f, 3175.583f, 226.8453f },
                { 257.2428f, 3083.400f, 226.8453f },
                { 313.0159f, 3184.851f, 226.8453f },
                { 244.9674f, 3091.190f, 226.8453f },
                { 299.8610f, 3191.041f, 226.8453f },
                { 235.0150f, 3101.788f, 226.8453f },
                { 285.5798f, 3193.765f, 226.8453f },
                { 228.0110f, 3114.529f, 226.8453f },
                { 271.0699f, 3192.853f, 226.8453f },
                { 224.3953f, 3128.611f, 226.8453f },
                { 257.2428f, 3188.360f, 226.8453f },
                { 224.3953f, 3143.149f, 226.8453f },
                { 244.9674f, 3180.570f, 226.8453f },
                { 228.0110f, 3157.231f, 226.8453f },
                { 235.0150f, 3169.971f, 226.8453f }
            }};

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (Position const& l_Pos : m_Spawns)
                {
                    Creature* l_ArcaneticRing = l_Caster->SummonCreature(eCreatures::NpcArcaneticRing, l_Pos);
                    if (!l_ArcaneticRing)
                        continue;

                    l_ArcaneticRing->CastSpell(l_ArcaneticRing, eSpells::ArcaneticRing, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_elisande_arcanetic_ring_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_arcanetic_ring_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Arcanetic Ring - 105315
class npc_elisande_arcanetic_ring : public CreatureScript
{
    public:
        npc_elisande_arcanetic_ring() : CreatureScript("npc_elisande_arcanetic_ring") { }

        struct npc_elisande_arcanetic_ringAI : public ScriptedAI
        {
            npc_elisande_arcanetic_ringAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                ApplyAllImmunities(true);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->GetMotionMaster()->Clear();

                if (me->GetMap() && me->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    me->GetMotionMaster()->MovePoint(10, p_Summoner->AsVector3());
                else
                {
                    Position l_Dest;
                    me->SimplePosXYRelocationByAngle(l_Dest, 110.0f, me->GetAngle(p_Summoner), true);
                    me->GetMotionMaster()->MovePoint(10, l_Dest.AsVector3());
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                return;
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                        me->DespawnOrUnsummon();
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_elisande_arcanetic_ringAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Slow Time - 209165 - Fast Time - 209166
class spell_elisande_alter_time : public SpellScriptLoader
{
    public:
        spell_elisande_alter_time() : SpellScriptLoader("spell_elisande_alter_time") { }

        class spell_elisande_alter_time_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_alter_time_AuraScript);

            enum eSpells
            {
                SlowTime = 209165
            };

            void HandleOnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || l_Target->GetEntry() != eCreatures::NpcArcaneticRing)
                    return;

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(m_scriptSpellId == eSpells::SlowTime ? -65 : 50);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_elisande_alter_time_AuraScript::HandleOnApply, EFFECT_1, SPELL_AURA_MOD_TIME_RATE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_alter_time_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Recursion - 209621
class spell_elisande_recursion : public SpellScriptLoader
{
    public:
        spell_elisande_recursion() : SpellScriptLoader("spell_elisande_recursion") { }

        class spell_elisande_recursion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_recursion_SpellScript);

            enum eSpells
            {
                DelphuricBeam1 = 213716,
                DelphuricBeam2 = 214278,
                DelphuricBeam3 = 225654
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster || !(l_Target->IsPlayer() || (l_Target->GetEntry() == eCreatures::Elisande)))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (l_Target->IsPlayer())
                {
                    for (auto l_Aura : l_Target->GetAppliedAuras())
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Aura.first);
                        if (!l_SpellInfo || l_SpellInfo->IsPositive())
                            continue;

                        Aura* l_Base = l_Aura.second->GetBase();
                        if (!l_Base)
                            continue;

                        l_Base->RefreshDuration();
                    }
                }

                if (l_Target->GetEntry() == eCreatures::Elisande)
                {
                    Aura* l_Aura = l_Target->GetAura(eSpells::DelphuricBeam1);
                    if (!l_Aura)
                        l_Aura = l_Target->GetAura(eSpells::DelphuricBeam2);
                    if (!l_Aura)
                        l_Aura = l_Target->GetAura(eSpells::DelphuricBeam3);
                    if (!l_Aura)
                        return;

                    l_Aura->RefreshDuration();
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_elisande_recursion_SpellScript::HandleCheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_elisande_recursion_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_recursion_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Expedite - 209617
class spell_elisande_expedite : public SpellScriptLoader
{
    public:
        spell_elisande_expedite() : SpellScriptLoader("spell_elisande_expedite") { }

        class spell_elisande_expedite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_expedite_SpellScript);

            enum eSpells
            {
                Expedite = 209618
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();
                if (!l_Target || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Expedite, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_elisande_expedite_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_expedite_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Spanning Singularity - 209168
class spell_elisande_spanning_singularity : public SpellScriptLoader
{
    public:
        spell_elisande_spanning_singularity() : SpellScriptLoader("spell_elisande_spanning_singularity") { }

        class spell_elisande_spanning_singularity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_spanning_singularity_SpellScript);

            enum eSpells
            {
            /// SpanningSingularityDamages              = 209174, triggered by missiles

            /// SpanningSingularityUnkRemoveAura        = 209187,
            /// SpanningSingularityTimeEchoAura         = 225712,

            /// SpanningSingularityMissileSpeed6 = 209186,
            /// SpanningSingularityMissileSpeed5 = 209169,
                SpanningSingularityMissileSpeed4 = 209170,
            /// SpanningSingularityMissileSpeed3 = 209171,
            /// SpanningSingularityMissileSpeed2 = 209172,

            /// SpanningSingularityEchoMissileSpeed6 = 233009,
            /// SpanningSingularityEchoMissileSpeed5 = 233010,
            /// SpanningSingularityEchoMissileSpeed4 = 233011,
            /// SpanningSingularityEchoMissileSpeed3 = 233012,
            /// SpanningSingularityEchoMissileSpeed2 = 233013,

            /// SpanningSingularityReducedCastTimeMissileSpeed6 = 233064,
            /// SpanningSingularityReducedCastTimeMissileSpeed5 = 233065,
            /// SpanningSingularityReducedCastTimeMissileSpeed4 = 233066,
            /// SpanningSingularityReducedCastTimeMissileSpeed3 = 233067,
            /// SpanningSingularityReducedCastTimeMissileSpeed2 = 233068,
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, p_Targets.size());

                std::list<WorldObject*> l_Targets;
                for (WorldObject* l_Target : p_Targets)
                {
                    if (!l_Target->ToPlayer() || !l_Target->ToPlayer()->IsRangedDamageDealer(true))
                        continue;

                    l_Targets.push_back(l_Target);
                }

                if (l_Targets.size() < 3)
                {
                    for (WorldObject* l_Target : p_Targets)
                    {
                        if (std::find(l_Targets.begin(), l_Targets.end(), l_Target) != l_Targets.end())
                            continue;

                        l_Targets.push_back(l_Target);
                    }
                }

                JadeCore::RandomResizeList(l_Targets, 3);
                p_Targets = l_Targets;
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpanningSingularityMissileSpeed4, true);

                if (l_Caster && l_Caster->GetEntry() == eCreatures::ElisandeEcho)
                    l_Caster = l_Caster->FindNearestCreature(eCreatures::Elisande, 150.0f);

                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                l_Caster->GetAI()->SetGUID(l_Target->GetGUID(), eScriptData::DataSpanningSingularity);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elisande_spanning_singularity_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_elisande_spanning_singularity_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_spanning_singularity_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Spanning Singularity (Damages) - 209174
class spell_elisande_spanning_singularity_damage : public SpellScriptLoader
{
    public:
        spell_elisande_spanning_singularity_damage() : SpellScriptLoader("spell_elisande_spanning_singularity_damage") { }

        class spell_elisande_spanning_singularity_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_spanning_singularity_damage_SpellScript);

            enum eSpells
            {
                SpanningSingularityAreatrigger          = 209500,
            /// SpanningSingularityAreatriggerDamage    = 209433, triggered by areatrigger
            /// SpanningSingularityAreatriggerAura      = 232974, triggered by areatrigger
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::SpanningSingularityAreatrigger, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_elisande_spanning_singularity_damage_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_spanning_singularity_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Spanning Singularity - 6616
class at_elisande_spanning_singularity : public AreaTriggerEntityScript
{
    public:
        at_elisande_spanning_singularity() : AreaTriggerEntityScript("at_elisande_spanning_singularity") { }

        enum eSpells
        {
            SpanningSingularity = 209433,
            Expedite            = 209618
        };

        std::map<uint64, int32> m_PlayerTickTimers;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            m_PlayerTickTimers[p_Target->GetGUID()] = 1 * IN_MILLISECONDS;
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            m_PlayerTickTimers.erase(p_Target->GetGUID());
            return false;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            for (std::pair<const uint64, int32>& l_Target : m_PlayerTickTimers)
            {
                Unit* l_UnitTarget = sObjectAccessor->GetUnit(*p_AreaTrigger, l_Target.first);
                if (!l_UnitTarget)
                    continue;

                float l_Speed = 1.0f;
                if (AuraEffect* l_AuraEffect = l_UnitTarget->GetAuraEffect(eSpells::Expedite, EFFECT_0))
                    AddPct(l_Speed, l_AuraEffect->GetAmount());

                float l_ModTimeRate = l_UnitTarget->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);

                l_Target.second -= (static_cast<float>(p_Time) / l_ModTimeRate) * l_Speed;

                if (l_Target.second < 0)
                {
                    l_Target.second += 1 * IN_MILLISECONDS;
                    p_AreaTrigger->CastSpellByOwnerFromATLocation(l_UnitTarget, eSpells::SpanningSingularity);
                }
            }

            if (!p_AreaTrigger->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4) || (p_AreaTrigger->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4) >= 0.20f))
                return;

            p_AreaTrigger->SetDuration(0);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_elisande_spanning_singularity();
        }
};

/// Last Update 7.1.5 Build 23420
/// Cascadent Star - 229738 - Cascadent Start - 229741
class spell_elisande_cascadent_star : public SpellScriptLoader
{
    public:
        spell_elisande_cascadent_star() : SpellScriptLoader("spell_elisande_cascadent_star") { }

        class spell_elisande_cascadent_star_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_cascadent_star_SpellScript);

            enum eSpells
            {
                CascadentStar           = 229740,
                CascadentStarBounced    = 229741
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->IsAIEnabled)
                    return;

                if (l_Caster->GetAI()->GetData(eScriptData::EncounterPhase) > 2)
                    return;

                if (m_scriptSpellId == eSpells::CascadentStarBounced && !l_Caster->GetAI()->GetData(eScriptData::CanBounceTwice))
                    return;

                std::list<Player*> l_Targets;
                l_Target->GetPlayerListInGrid(l_Targets, 100.0f);

                if (l_Targets.empty())
                    return;

                JadeCore::RandomResizeList(l_Targets, 1);

                l_Target->CastSpell(l_Targets.front(), eSpells::CascadentStar, true, nullptr, nullptr, l_Caster->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_elisande_cascadent_star_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_cascadent_star_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Cascadent Star - 229740
class spell_elisande_cascadent_star_bounced : public SpellScriptLoader
{
    public:
        spell_elisande_cascadent_star_bounced() : SpellScriptLoader("spell_elisande_cascadent_star_bounced") { }

        class spell_elisande_cascadent_star_bounced_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_cascadent_star_bounced_SpellScript);

            enum eSpells
            {
                CascadentStar = 229741
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetOriginalCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::CascadentStar, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_elisande_cascadent_star_bounced_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_cascadent_star_bounced_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Epocheric Orb - 213739
class spell_elisande_epocheric_orb : public SpellScriptLoader
{
    public:
        spell_elisande_epocheric_orb() : SpellScriptLoader("spell_elisande_epocheric_orb") { }

        class spell_elisande_epocheric_orb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_epocheric_orb_SpellScript);

            enum eSpells
            {
                EpochericOrbPortal      = 210275,

            /// EpochericOrb2           = 210022,
            /// EpochericOrb3           = 225655,

            /// EpochericOrbUnk110      = 210023, ///< triggered by EpochericOrbPortal
            /// EpochericOrbUnk108      = 210577, ///< triggered by EpochericOrbPortal2
            /// EpochericOrbUnk106      = 210578, ///< triggered by EpochericOrbPortal3
            /// EpochericOrbUnk105      = 210581, ///< triggered by EpochericOrbPortal4
            /// EpochericOrbUnk104      = 210582, ///< triggered by EpochericOrbPortal5
            /// EpochericOrbPortal2     = 210588,
            /// EpochericOrbPortal3     = 210589,
            /// EpochericOrbPortal4     = 210591,
            /// EpochericOrbPortal5     = 210592,

            /// EpochericOrbUnkRemoveAura = 211618,
            /// EpochericOrbTimeEchoAura = 225508,

            /// EpochericOrbUnk         = 228944, ///< Triggered by EpochericOrbUnkMissile
            /// EpochericOrbUnkMissile  = 228949,
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled || l_Caster->GetEntry() == eCreatures::ElisandeEcho)
                    return;

                Position l_Dest;

                uint32 l_OrbCount = 1 + (l_Caster->GetMap()->GetPlayersCountExceptGMs() / 10);
                for (uint32 l_I = 0; l_I < l_OrbCount; ++l_I)
                {
                    g_RoomCenter.SimplePosXYRelocationByAngle(l_Dest, frand(15.0f, 35.0f), frand(0.0f, 2.0f * M_PI));

                    l_Caster->CastSpell(l_Dest, eSpells::EpochericOrbPortal, true);

                    l_Dest.m_positionZ = 244.55f;
                    Creature* l_Creature = l_Caster->SummonCreature(eCreatures::NpcEpochericOrb, l_Dest);
                    if (!l_Creature)
                        continue;;

                    l_Caster->GetAI()->SetGUID(l_Creature->GetGUID(), eScriptData::DataEpochericOrb);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_elisande_epocheric_orb_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_epocheric_orb_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Epocheric Orb - 107754
class npc_elisande_epocheric_orb : public CreatureScript
{
    public:
        npc_elisande_epocheric_orb() : CreatureScript("npc_elisande_epocheric_orb") { }

        struct npc_elisande_epocheric_orbAI : public ScriptedAI
        {
            npc_elisande_epocheric_orbAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                me->CastSpell(me, eSpells::EpochericOrbAura, true);
            }

            enum eSpells
            {
                EpochericOrbAura    = 213955,
                EpochericOrbFalloff = 210024
            };

            void Reset() override
            {
                Position l_Target = *me;

                l_Target.m_positionZ = me->GetMap()->GetHeight(me->m_positionX, me->m_positionY, me->m_positionZ);

                me->GetMotionMaster()->MovePoint(10, l_Target);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        me->CastSpell(me, eSpells::EpochericOrbFalloff, true);
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_elisande_epocheric_orbAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Epocheric Orb (Areatrigger) - 213955
class spell_elisande_epocheric_orb_aura : public SpellScriptLoader
{
    public:
        spell_elisande_epocheric_orb_aura() : SpellScriptLoader("spell_elisande_epocheric_orb_aura") { }

        class spell_elisande_epocheric_orb_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_epocheric_orb_aura_SpellScript);

            enum eSpells
            {
                EpochericOrbAreatrigger = 213956
            };

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Position l_Target = *l_Caster;

                l_Target.m_positionZ = l_Caster->GetMap()->GetHeight(l_Caster->m_positionX, l_Caster->m_positionY, l_Caster->m_positionZ);
                l_Caster->CastSpell(l_Target, eSpells::EpochericOrbAreatrigger, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_elisande_epocheric_orb_aura_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_epocheric_orb_aura_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Epocheric Orb (Damage) - 213955
class spell_elisande_epocheric_orb_damage : public SpellScriptLoader
{
    public:
        spell_elisande_epocheric_orb_damage() : SpellScriptLoader("spell_elisande_epocheric_orb_damage") { }

        class spell_elisande_epocheric_orb_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_epocheric_orb_damage_SpellScript);

            enum eSpells
            {
                EpochericOrbFalloff = 210546
            };

            void CheckTarget(std::list<WorldObject*>& p_Targets)
            {
                if (!p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::EpochericOrbFalloff);
                if (!l_Caster || !l_SpellInfo)
                    return;

                SpellRadiusEntry const* l_Radius = l_SpellInfo->Effects[EFFECT_0].RadiusEntry;
                if (!l_Radius)
                    return;

                std::list<Player*> l_Targets;
                l_Caster->GetPlayerListInGrid(l_Targets, l_Radius->RadiusMax);

                for (Player* l_Player : l_Targets)
                    p_Targets.push_back(l_Player);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elisande_epocheric_orb_damage_SpellScript::CheckTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_epocheric_orb_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Delphuric Beam - 213716 - Delphuric Beam - 214278 - Delphuric Beam - 225654
class spell_elisande_delphuric_beam_periodic : public SpellScriptLoader
{
    public:
        spell_elisande_delphuric_beam_periodic() : SpellScriptLoader("spell_elisande_delphuric_beam_periodic") { }

        class spell_elisande_delphuric_beam_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_delphuric_beam_periodic_AuraScript);

            enum eSpells
            {
                DelphuricBeam = 209242
            };

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::DelphuricBeam, true);
                if (l_Caster->GetMap() && l_Caster->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidLFR)
                    p_AuraEffect->GetBase()->SetDuration(0);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_elisande_delphuric_beam_periodic_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_delphuric_beam_periodic_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Delphuric Beam - 209242
class spell_elisande_delphuric_beam : public SpellScriptLoader
{
    public:
        spell_elisande_delphuric_beam() : SpellScriptLoader("spell_elisande_delphuric_beam") { }

        class spell_elisande_delphuric_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_delphuric_beam_SpellScript);

            enum eSpells
            {
                DelphuricBeamArrow      = 209244
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->IsAIEnabled)
                    return;

                l_Caster->GetAI()->SetGUID(0, eScriptData::DataDelphuricBeam);

                if (p_Targets.empty())
                    return;

                uint32 l_TargetCount = 1 + (l_Caster->GetMap()->GetPlayersCountExceptGMs() / 10);
                JadeCore::RandomResizeList(p_Targets, std::min<uint32>(p_Targets.size(), l_TargetCount));
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DelphuricBeamArrow, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elisande_delphuric_beam_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_elisande_delphuric_beam_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_delphuric_beam_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Delphuric Beam - 209244
class spell_elisande_delphuric_beam_arrow : public SpellScriptLoader
{
    public:
        spell_elisande_delphuric_beam_arrow() : SpellScriptLoader("spell_elisande_delphuric_beam_arrow") { }

        class spell_elisande_delphuric_beam_arrow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_delphuric_beam_arrow_AuraScript);

            enum eSpells
            {
                DelphuricBeamDamages = 209248
            };

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                if ((l_Caster->GetEntry() == eCreatures::Elisande) && l_Caster->GetMap() && l_Caster->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic)
                {
                    Position l_Dest;
                    l_Caster->SimplePosXYRelocationByAngle(l_Dest, 150.0f, l_Caster->GetAngle(l_Target), true);
                    l_Caster->CastSpell(l_Dest, eSpells::DelphuricBeamDamages, true);
                }
                else
                    l_Caster->CastSpell(l_Target, eSpells::DelphuricBeamDamages, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_elisande_delphuric_beam_arrow_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_delphuric_beam_arrow_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ablating Explosion - 209973
class spell_elisande_ablating_explosion : public SpellScriptLoader
{
    public:
        spell_elisande_ablating_explosion() : SpellScriptLoader("spell_elisande_ablating_explosion") { }

        class spell_elisande_ablating_explosion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_ablating_explosion_AuraScript);

            enum eSpells
            {
                AblatingExplosion = 211799
            };

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::AblatingExplosion, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_elisande_ablating_explosion_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_ablating_explosion_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Ablating Explosion (Damage) - 211799
class spell_elisande_ablating_explosion_damage : public SpellScriptLoader
{
    public:
        spell_elisande_ablating_explosion_damage() : SpellScriptLoader("spell_elisande_ablating_explosion_damage") { }

        class spell_elisande_ablating_explosion_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_ablating_explosion_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.push_back(l_Caster);
            }

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                WorldLocation const* l_MainTarget = GetExplTargetDest();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_MainTarget || !l_Target || !l_SpellInfo)
                    return;

                float l_Radius = 100.0f;

                float l_Pct = std::max(0.25f * l_Radius, l_Radius - l_MainTarget->GetExactDist(l_Target)) / l_Radius;
                SetHitDamage(static_cast<float>(GetHitDamage()) * l_Pct);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elisande_ablating_explosion_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_elisande_ablating_explosion_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_ablating_explosion_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Conflexive Burst - 209597
class spell_elisande_conflexive_burst : public SpellScriptLoader
{
    public:
        spell_elisande_conflexive_burst() : SpellScriptLoader("spell_elisande_conflexive_burst") { }

        class spell_elisande_conflexive_burst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_conflexive_burst_SpellScript);

            enum eSpells
            {
                ConflexiveBurst = 209598
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, std::min<uint32>(p_Targets.size(), 3));
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ConflexiveBurst, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elisande_conflexive_burst_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_elisande_conflexive_burst_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_conflexive_burst_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Conflexive Burst (Aura) - 209598
class spell_elisande_conflexive_burst_aura : public SpellScriptLoader
{
    public:
        spell_elisande_conflexive_burst_aura() : SpellScriptLoader("spell_elisande_conflexive_burst_aura") { }

        class spell_elisande_conflexive_burst_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_conflexive_burst_aura_AuraScript);

            enum eSpells
            {
                ConflexiveBurst = 209599
            };

            void HandleOnRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ConflexiveBurst, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_elisande_conflexive_burst_aura_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_conflexive_burst_aura_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Conflexive Burst - 209599
class spell_elisande_conflexive_burst_damage : public SpellScriptLoader
{
    public:
        spell_elisande_conflexive_burst_damage() : SpellScriptLoader("spell_elisande_conflexive_burst_damage") { }

        class spell_elisande_conflexive_burst_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_conflexive_burst_damage_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo || !l_Caster->GetMap() || l_Caster->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidMythic)
                    return;

                SpellRadiusEntry const* l_RadiusEntry = l_SpellInfo->Effects[EFFECT_0].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                float l_Pct = std::max(0.25f * l_RadiusEntry->RadiusMax, l_RadiusEntry->RadiusMax - l_Caster->GetExactDist(l_Target)) / l_RadiusEntry->RadiusMax;
                SetHitDamage(static_cast<float>(GetHitDamage()) * l_Pct);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_elisande_conflexive_burst_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_conflexive_burst_damage_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Permeliative Torment - 225653
class spell_elisande_permeliative_torment : public SpellScriptLoader
{
    public:
        spell_elisande_permeliative_torment() : SpellScriptLoader("spell_elisande_permeliative_torment") { }

        class spell_elisande_permeliative_torment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elisande_permeliative_torment_SpellScript);

            enum eSpells
            {
                PermeliativeTorment = 211261
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, std::min<uint32>(p_Targets.size(), 4));
            }

            void HandleOnHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::PermeliativeTorment, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_elisande_permeliative_torment_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_elisande_permeliative_torment_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elisande_permeliative_torment_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Permeliative Torment - 211261
class spell_elisande_permeliative_torment_aura : public SpellScriptLoader
{
    public:
        spell_elisande_permeliative_torment_aura() : SpellScriptLoader("spell_elisande_permeliative_torment_aura") { }

        class spell_elisande_permeliative_torment_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_permeliative_torment_aura_AuraScript);

            uint32 m_Tick = 0;
            uint32 m_BaseAmount = 0;

            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                m_Tick++;
                if (!m_BaseAmount)
                    m_BaseAmount = p_AuraEffect->GetAmount();

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->GetMap())
                    return;

                uint32 m_DifficultyMultiplier = 0;
                switch (l_Caster->GetMap()->GetDifficultyID())
                {
                    case Difficulty::DifficultyRaidLFR:
                        m_DifficultyMultiplier = 0;
                        break;
                    case Difficulty::DifficultyRaidNormal:
                        m_DifficultyMultiplier = 1;
                        break;
                    case Difficulty::DifficultyRaidHeroic:
                        m_DifficultyMultiplier = 2;
                        break;
                    case Difficulty::DifficultyRaidMythic:
                        m_DifficultyMultiplier = 4;
                        break;
                    default:
                        break;
                }

                uint32 l_Amount = CalculatePct(m_BaseAmount, 100 + (m_Tick * m_DifficultyMultiplier));

                const_cast<AuraEffect*>(p_AuraEffect)->SetAmount(l_Amount);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_elisande_permeliative_torment_aura_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_permeliative_torment_aura_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Etched in Time - 208861
class spell_elisande_etched_in_time : public SpellScriptLoader
{
    public:
        spell_elisande_etched_in_time() : SpellScriptLoader("spell_elisande_etched_in_time") { }

        class spell_elisande_etched_in_time_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_elisande_etched_in_time_AuraScript);

            void HandleAfterEffectApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || l_Target->isInCombat())
                    return;

                l_Target->ApplySpellImmune(p_AuraEffect->GetId(), IMMUNITY_DAMAGE, p_AuraEffect->GetMiscValue(), false);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_elisande_etched_in_time_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_DAMAGE_IMMUNITY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_elisande_etched_in_time_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Infinite Whelpling - 68820
class npc_elisande_infinite_whelpling : public CreatureScript
{
    public:
        npc_elisande_infinite_whelpling() : CreatureScript("npc_elisande_infinite_whelpling") { }

        struct npc_elisande_infinite_whelplingAI : public ScriptedAI
        {
            npc_elisande_infinite_whelplingAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_IsInBothAreatriggers = 0;
            }

            enum eSpells
            {
                SlowTime = 209165,
                FastTime = 209166
            };

            enum eAreatriggers
            {
                AtSlowTime  = 11124,
                AtFastTime  = 11128
            };

            enum eMaps
            {
                TheNighthold = 1530
            };

            uint32 m_IsInBothAreatriggers;

            void UpdateAI(uint32 const p_Diff) override
            {
                if ((me->GetMapId() != eMaps::TheNighthold) || (me->GetExactDist(&g_RoomCenter) > 100.0f))
                {
                    m_IsInBothAreatriggers = 0;
                    return;
                }

                std::list<AreaTrigger*> l_Areatriggers;
                me->GetAreatriggerListInRange(l_Areatriggers, 10.0f);
                std::list<AreaTrigger*>::iterator l_FastTimeBubble = std::find_if(l_Areatriggers.begin(), l_Areatriggers.end(), [](AreaTrigger const* p_Areatrigger) -> bool
                {
                    if (p_Areatrigger->GetEntry() == eAreatriggers::AtFastTime)
                        return true;

                    return false;
                });

                if (l_FastTimeBubble == l_Areatriggers.end())
                {
                    m_IsInBothAreatriggers = 0;
                    return;
                }

                std::list<AreaTrigger*>::iterator l_SlowTimeBubble = std::find_if(l_Areatriggers.begin(), l_Areatriggers.end(), [](AreaTrigger const* p_Areatrigger) -> bool
                {
                    if (p_Areatrigger->GetEntry() == eAreatriggers::AtSlowTime)
                        return true;

                    return false;
                });

                if (l_SlowTimeBubble == l_Areatriggers.end())
                {
                    m_IsInBothAreatriggers = 0;
                    return;
                }

                m_IsInBothAreatriggers += p_Diff;

                if (m_IsInBothAreatriggers >= 1 * IN_MILLISECONDS)
                {
                    Creature* l_Elisande = me->FindNearestCreature(eCreatures::Elisande, 150.0f);
                    if (!l_Elisande || !l_Elisande->IsAIEnabled)
                        return;

                    l_Elisande->GetAI()->SetGUID(me->GetGUID(), eScriptData::InfiniteWhelpling);

                    m_IsInBothAreatriggers = 0;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_elisande_infinite_whelplingAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Infinite Drakeling - 108802
class npc_elisande_infinite_drakeling : public CreatureScript
{
    public:
        npc_elisande_infinite_drakeling() : CreatureScript("npc_elisande_infinite_drakeling") { }

        struct npc_elisande_infinite_drakelingAI : public LegionCombatAI
        {
            npc_elisande_infinite_drakelingAI(Creature* p_Creature) : LegionCombatAI(p_Creature) {}

            void JustDied(Unit* /*p_Killer*/) override
            {
                Creature* l_Elisande = me->FindNearestCreature(eCreatures::Elisande, 150.0f);
                if (!l_Elisande || !l_Elisande->IsAIEnabled)
                    return;

                l_Elisande->GetAI()->DoAction(eScriptData::ValidateAchievement);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_elisande_infinite_drakelingAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_elisande()
{
    /// Boss
    new boss_elisande();
    new npc_elisande_echo();

    /// Spells
    new spell_elisande_leaving_the_nightwell();
    new spell_elisande_time_stop();
    new spell_elisande_summon_time_elemental();
    new spell_elisande_arcanetic_ring();
    new spell_elisande_alter_time();
    new spell_elisande_recursion();
    new spell_elisande_expedite();
    new spell_elisande_spanning_singularity();
    new spell_elisande_spanning_singularity_damage();
    new spell_elisande_cascadent_star();
    new spell_elisande_cascadent_star_bounced();
    new spell_elisande_epocheric_orb();
    new spell_elisande_epocheric_orb_aura();
    new spell_elisande_epocheric_orb_damage();
    new spell_elisande_delphuric_beam_periodic();
    new spell_elisande_delphuric_beam();
    new spell_elisande_delphuric_beam_arrow();
    new spell_elisande_ablating_explosion();
    new spell_elisande_ablating_explosion_damage();
    new spell_elisande_conflexive_burst();
    new spell_elisande_conflexive_burst_aura();
    new spell_elisande_conflexive_burst_damage();
    new spell_elisande_permeliative_torment();
    new spell_elisande_permeliative_torment_aura();
    new spell_elisande_etched_in_time();

    /// NPC
    new npc_elisande_time_elemental();
    new npc_elisande_arcanetic_ring();
    new npc_elisande_epocheric_orb();

    /// Achievement
    new npc_elisande_infinite_whelpling();
    new npc_elisande_infinite_drakeling();

    /// Areatrigger
    new at_elisande_spanning_singularity();
}
#endif
