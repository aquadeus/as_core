////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eSays
{
    SayAggro,
    SayTimeBomb,
    SayFragmentedTimeParticle,
    SaySlowedTime,
    SaySlowedTimeWarn,
    SayFastenTime,
    SayFastenTimeWarn,
    SayNormalTime,
    SayNormalTimeWarn,
    SaySlayPlayer,
    SayWipe,
    SayDeath,
    SaySummonSlowAdd,
    SayPowerOverwhelming
};

enum eSpells
{
    SpellPassageOfTime          = 205653,

    SpellSpeedSlow              = 207011,
    SpellSpeedNormal            = 207012,
    SpellSpeedFast              = 207013,
    SpellBurstOfTime            = 219984,
    SpellBurstOfTimeNormal      = 206613,
    SpellBurstOfTimeSlow        = 214050,
    SpellBurstOfTimeFast        = 214049,
    SpellPowerOverwhelming      = 211927,
    SpellPowerOverwhelmingCosm  = 228770,
    SpellPowerOverwhelmingMod   = 219823,
    SpellChronometricParticles  = 206607,
    SpellChronometricOverload   = 207491,
    SpellTimeReleaseFilter      = 206610,
    SpellTimeReleaseAbsorb      = 206609,
    SpellTimeReleaseDmg         = 206608,
    SpellTimeReleaseGreen       = 219964,
    SpellTimeReleaseYellow      = 219965,
    SpellTimeReleaseRed         = 219966,
    SpellTimeBombFilter         = 206618,
    SpellTimeBombDmg            = 206615,
    SpellTimeBomb               = 206617,
    SpellSumSlowAddFilter       = 206700,
    SpellSumSlowAdd             = 206698,
    SpellTemporalOrb            = 219815,
    SpellTemporalOrbAT          = 212874,
    SpellTemporalOrbAT2         = 227217,
    SpellTemporalSmashMod       = 222283,
    SpellTemporalSmash          = 212115,
    SpellTemporalSmash2         = 212109,
    SpellTemporalCharge         = 226845,
    SpellTemporalCharge2        = 212099,
    SpellFadeOut                = 199615,

    SpellWarpNightwellBig       = 207228,
    SpellWarpNightwellSmall     = 228335,
    SpellChronomate             = 219808,
    SpellTemporalRift           = 212072,
    SpellSumTemporalRift        = 212076,
    SpellTemporalRiftVis        = 212090,

    SpellFullPowerPeriodic      = 207976
};

enum eEvents
{
    EventBurstOfTime = 1,
    EventChronometricParticles,
    EventTimeRelease,
    EventTimeBomb,
    EventSumSlowAdd,
    EventTemporalOrb,
    EventPowerOverwhelming,

    /// Mythic
    EventChangeSpeed,

    EventMax
};

enum eAnomalyData
{
    MaxAnomalies    = 12,

    ParticleVisual  = 54240
};

enum ePhases
{
    PhaseNormal1,
    PhaseSlow1,
    PhaseFast1,
    PhaseNormal2,
    PhaseFast2,
    PhaseSlow2,
    PhaseNormal3,
    PhaseSlow3,
    PhaseFast3,
    PhaseNormal4,
    PhaseSlow4,
    PhaseFast4,
    PhaseNormal5,
    PhaseSlow5,
    PhaseFast5
};

enum eMythicPhases
{
    MythicPhaseNormal1,
    MythicPhaseFast1,
    MythicPhaseSlow1,
    MythicPhaseFast2,
    MythicPhaseSlow2,
    MythicPhaseFast3,
    MythicPhaseSlow3,
    MythicPhaseFast4,
    MythicPhaseSlow4,
    MythicPhaseFast5,
    MythicPhaseSlow5
};

std::array<Position const, eAnomalyData::MaxAnomalies> g_AnomalySumPos =
{
    {
        { 232.46f, 3156.74f, -236.95f, 5.98f }, ///< 108786
        { 248.43f, 3175.77f, -236.95f, 5.46f }, ///< 108786
        { 271.78f, 3184.27f, -236.95f, 4.89f }, ///< 108786
        { 296.25f, 3179.95f, -236.95f, 4.29f }, ///< 108786
        { 315.29f, 3163.98f, -236.95f, 3.68f }, ///< 108786
        { 323.78f, 3140.63f, -236.93f, 3.09f }, ///< 108786
        { 319.47f, 3116.16f, -236.95f, 2.55f }, ///< 108786
        { 303.50f, 3097.13f, -236.95f, 2.05f }, ///< 108786
        { 277.72f, 3087.88f, -236.93f, 1.58f }, ///< 108786
        { 254.49f, 3093.32f, -236.93f, 1.12f }, ///< 108786
        { 236.42f, 3108.52f, -236.93f, 0.66f }, ///< 108786
        { 228.15f, 3132.27f, -236.95f, 0.19f }  ///< 108786
    }
};

/// First point must be the last point too
std::array<G3D::Vector3, eAnomalyData::MaxAnomalies + 1> g_WaitingPath =
{
    {
        { 232.46f, 3156.74f, -236.95f },
        { 248.43f, 3175.77f, -236.95f },
        { 271.78f, 3184.27f, -236.95f },
        { 296.25f, 3179.95f, -236.95f },
        { 315.29f, 3163.98f, -236.95f },
        { 323.78f, 3140.63f, -236.93f },
        { 319.47f, 3116.16f, -236.95f },
        { 303.50f, 3097.13f, -236.95f },
        { 277.72f, 3087.88f, -236.93f },
        { 254.49f, 3093.32f, -236.93f },
        { 236.42f, 3108.52f, -236.93f },
        { 228.15f, 3132.27f, -236.95f },
        { 232.46f, 3156.74f, -236.95f }
    }
};

Position const g_CenterPos = { 276.35f, 3136.08f, -236.95f, 0.0f };

///      DifficultyID ->      PhaseID ->       Event -> CastCount -> Timer
std::map<Difficulty, std::map<uint32, std::map<uint32, std::vector<uint32>>>> g_TimerData =
{
    {
        Difficulty::DifficultyRaidLFR,
        {
            {
                ePhases::PhaseNormal1,
                {
                    { eEvents::EventTimeRelease,        { 5000, 15000 } },
                    { eEvents::EventTimeBomb,           { 36500 } },
                    { eEvents::EventTemporalOrb,        { 48000 } },
                    { eEvents::EventSumSlowAdd,         { 28000 } },
                    { eEvents::EventPowerOverwhelming,  { 75000 } }
                }
            },
            {
                ePhases::PhaseNormal2,
                {
                    { eEvents::EventTimeRelease,        { 5000, 16000 } },
                    { eEvents::EventTimeBomb,           { 19500 } },
                    { eEvents::EventTemporalOrb,        { 13000 } },
                    { eEvents::EventSumSlowAdd,         { 30000 } },
                    { eEvents::EventPowerOverwhelming,  { 75000 } }
                }
            },
            {
                ePhases::PhaseSlow1,
                {
                    { eEvents::EventTimeRelease,        { 5000, 23000 } },
                    { eEvents::EventTimeBomb,           { 22200 } },
                    { eEvents::EventTemporalOrb,        { 30000 } },
                    { eEvents::EventSumSlowAdd,         { 38000 } },
                    { eEvents::EventPowerOverwhelming,  { 90000 } }
                }
            },
            {
                ePhases::PhaseSlow2,
                {
                    { eEvents::EventTimeRelease,    { 28000 } },
                    { eEvents::EventTemporalOrb,    { 8000 } },
                    { eEvents::EventSumSlowAdd,     { 23000 } }
                }
            },
            {
                ePhases::PhaseFast1,
                {
                    { eEvents::EventTimeRelease,        { 10000, 15000, 20000, 15000 } },
                    { eEvents::EventTemporalOrb,        { 15000, 25000 } },
                    { eEvents::EventSumSlowAdd,         { 32000 } },
                    { eEvents::EventPowerOverwhelming,  { 850000 } }
                }
            }
        }
    },
    {
        Difficulty::DifficultyRaidNormal,
        {
            {
                ePhases::PhaseNormal1,
                {
                    { eEvents::EventTimeRelease,        { 5000, 15000 } },
                    { eEvents::EventTimeBomb,           { 36500 } },
                    { eEvents::EventTemporalOrb,        { 48000 } },
                    { eEvents::EventSumSlowAdd,         { 28000 } },
                    { eEvents::EventPowerOverwhelming,  { 60000 } }
                }
            },
            {
                ePhases::PhaseNormal2,
                {
                    { eEvents::EventTimeRelease,        { 5000, 16000 } },
                    { eEvents::EventTimeBomb,           { 19500 } },
                    { eEvents::EventTemporalOrb,        { 13000 } },
                    { eEvents::EventSumSlowAdd,         { 49000 } },
                    { eEvents::EventPowerOverwhelming,  { 60000 } }
                }
            },
            {
                ePhases::PhaseNormal3,
                {
                    { eEvents::EventSumSlowAdd,         { 8000 } },
                    { eEvents::EventPowerOverwhelming,  { 19000 } }
                }
            },
            {
                ePhases::PhaseSlow1,
                {
                    { eEvents::EventTimeRelease,        { 5000, 23000 } },
                    { eEvents::EventTimeBomb,           { 22200 } },
                    { eEvents::EventTemporalOrb,        { 30000 } },
                    { eEvents::EventSumSlowAdd,         { 38000 } },
                    { eEvents::EventPowerOverwhelming,  { 60000 } }
                }
            },
            {
                ePhases::PhaseSlow2,
                {
                    { eEvents::EventTimeRelease,        { 28000 } },
                    { eEvents::EventTemporalOrb,        { 8000 } },
                    { eEvents::EventSumSlowAdd,         { 23000 } },
                    { eEvents::EventPowerOverwhelming,  { 38000 } }
                }
            },
            {
                ePhases::PhaseFast1,
                {
                    { eEvents::EventTimeRelease,        { 10000, 15000, 20000, 15000 } },
                    { eEvents::EventTemporalOrb,        { 15000, 25000 } },
                    { eEvents::EventSumSlowAdd,         { 32000 } },
                    { eEvents::EventPowerOverwhelming,  { 60000 } }
                }
            }
        }
    },
    {
        Difficulty::DifficultyRaidHeroic,
        {
            {
                ePhases::PhaseNormal1,
                {
                    { eEvents::EventTimeRelease,        { 5000, 13000, 25000 } },
                    { eEvents::EventTimeBomb,           { 29500, 5000 } },
                    { eEvents::EventTemporalOrb,        { 38000 } },
                    { eEvents::EventSumSlowAdd,         { 25000 } },
                    { eEvents::EventPowerOverwhelming,  { 55000 } }
                }
            },
            {
                ePhases::PhaseNormal2,
                {
                    { eEvents::EventTimeRelease,        { 30000, 20000, 7000 } },
                    { eEvents::EventTimeBomb,           { 6500, 10000, 10000 } },
                    { eEvents::EventTemporalOrb,        { 10000, 25000, 30000 } },
                    { eEvents::EventSumSlowAdd,         { 16000 } },
                    { eEvents::EventPowerOverwhelming,  { 40000 } }
                }
            },
            {
                ePhases::PhaseSlow1,
                {
                    { eEvents::EventTimeRelease,        { 10000, 20000 } },
                    { eEvents::EventTimeBomb,           { 17000, 10000, 10000, 5000 } },
                    { eEvents::EventTemporalOrb,        { 20000, 18000, 7000 } },
                    { eEvents::EventSumSlowAdd,         { 43000 } },
                    { eEvents::EventPowerOverwhelming,  { 60000 } }
                }
            },
            {
                ePhases::PhaseFast1,
                {
                    { eEvents::EventTimeRelease,        { 5000, 7000, 13000, 5000, 5000, 8000 } },
                    { eEvents::EventTimeBomb,           { 18000 } },
                    { eEvents::EventSumSlowAdd,         { 38000 } },
                    { eEvents::EventPowerOverwhelming,  { 50000 } }
                }
            },
            {
                ePhases::PhaseFast2,
                {
                    { eEvents::EventTimeRelease,    { 40000 } },
                    { eEvents::EventTimeBomb,       { 31000 } },
                    { eEvents::EventTemporalOrb,    { 10000, 15000, 20000 } }
                }
            }
        }
    }
};

///      DifficultyID ->      PhaseID ->       Event -> CastCount -> Timer
std::map<Difficulty, std::map<uint32, std::map<uint32, std::vector<uint32>>>> g_MythicTimerData =
{
    {
        Difficulty::DifficultyRaidMythic,
        {
            {
                eMythicPhases::MythicPhaseNormal1,
                {
                    { eEvents::EventTimeRelease,    { 10000 } },
                    { eEvents::EventTimeBomb,       { 6500 } },
                    { eEvents::EventChangeSpeed,    { 12000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseFast1,
                {
                    { eEvents::EventTimeRelease,        { 5000 } },
                    { eEvents::EventTemporalOrb,        { 12000 } },
                    { eEvents::EventSumSlowAdd,         { 7000 } },
                    { eEvents::EventPowerOverwhelming,  { 22000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseFast2,
                {
                    { eEvents::EventTimeRelease,        { 5000, 5000, 5000, 5000 } },
                    { eEvents::EventTemporalOrb,        { 25000 } },
                    { eEvents::EventSumSlowAdd,         { 23000 } },
                    { eEvents::EventPowerOverwhelming,  { 30000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseFast3,
                {
                    { eEvents::EventTimeRelease,        { 5000, 5000, 5000, 5000 } },
                    { eEvents::EventTemporalOrb,        { 23000 } },
                    { eEvents::EventSumSlowAdd,         { 25000 } },
                    { eEvents::EventPowerOverwhelming,  { 30000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseFast4,
                {
                    { eEvents::EventTimeRelease,    { 5000 } },
                    { eEvents::EventTimeBomb,       { 6000 } },
                    { eEvents::EventChangeSpeed,    { 8000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseSlow1,
                {
                    { eEvents::EventTimeRelease,        { 13000, 10000 } },
                    { eEvents::EventTimeBomb,           { 18000 } },
                    { eEvents::EventTemporalOrb,        { 5000 } },
                    { eEvents::EventPowerOverwhelming,  { 25000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseSlow2,
                {
                    { eEvents::EventTimeRelease,        { 7000 } },
                    { eEvents::EventTimeBomb,           { 4000 } },
                    { eEvents::EventTemporalOrb,        { 14000 } },
                    { eEvents::EventPowerOverwhelming,  { 28000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseSlow3,
                {
                    { eEvents::EventTimeRelease,        { 20000 } },
                    { eEvents::EventTimeBomb,           { 9000 } },
                    { eEvents::EventTemporalOrb,        { 5000 } },
                    { eEvents::EventPowerOverwhelming,  { 9000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseSlow4,
                {
                    { eEvents::EventTimeRelease,        { 5000 } },
                    { eEvents::EventTimeBomb,           { 22000 } },
                    { eEvents::EventTemporalOrb,        { 15000, 10000 } },
                    { eEvents::EventPowerOverwhelming,  { 30000 } }
                }
            },
            {
                eMythicPhases::MythicPhaseSlow5,
                {
                    { eEvents::EventTimeRelease,        { 2000, 22000 } },
                    { eEvents::EventTimeBomb,           { 5000 } },
                    { eEvents::EventPowerOverwhelming,  { 8000 } }
                }
            }
        }
    }
};

/// Chronomatic Anomaly - 104415
class boss_chronomatic_anomaly : public CreatureScript
{
    public:
        boss_chronomatic_anomaly() : CreatureScript("boss_chronomatic_anomaly") { }

        struct boss_chronomatic_anomalyAI : BossAI
        {
            boss_chronomatic_anomalyAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataChronomaticAnomaly)
            {
                m_PulledOnce = false;
            }

            std::list<uint64> m_TriggerList;

            bool m_PulledOnce;
            bool m_PhaseOverwhelming;

            uint8 m_TimeBombCounter;
            uint8 m_TimeReleaseCounter;
            uint8 m_TemporalOrbCounter;

            uint8 m_SpeedCounter;
            int8 m_AnomalyPhaseID;
            uint32 m_AnomalySpeed;
            uint32 m_BurstTimer;
            uint32 m_BurstSpell;

            uint32 m_SlowCount = 0;
            uint32 m_FastCount = 0;

            uint32 m_LastTalkTimer = 0;

            uint32 m_MoveID = 1;

            bool m_FullPower = false;
            uint32 m_FullPowerTimer = 0;

            EventMap m_Events;

            bool m_FirstPowerFinished = false;

            void Reset() override
            {
                _Reset();

                me->SetPower(Powers::POWER_RAGE, 0);

                m_TimeBombCounter = 0;
                m_TimeReleaseCounter = 0;
                m_TemporalOrbCounter = 0;
                m_AnomalySpeed = 0;
                m_SpeedCounter = 0;
                m_LastTalkTimer = 0;
                m_SlowCount = 0;
                m_FastCount = 0;
                m_PhaseOverwhelming = false;

                m_FullPower = false;
                m_FullPowerTimer = 0;

                m_FirstPowerFinished = false;

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                me->RemoveAurasDueToSpell(eSpells::SpellPassageOfTime);
                me->RemoveAurasDueToSpell(eSpells::SpellFullPowerPeriodic);

                m_TriggerList.clear();

                for (Position const& l_Pos : g_AnomalySumPos)
                {
                    if (Creature* l_Trigger = me->SummonCreature(eCreatures::NpcSuramarTrigger, l_Pos))
                        m_TriggerList.push_back(l_Trigger->GetGUID());
                }

                me->SummonCreature(eCreatures::NpcTheNightwell, g_CenterPos);

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (m_PulledOnce)
                    {
                        DoCast(me, eSpells::SpellPowerOverwhelmingCosm);
                        return;
                    }

                    m_MoveID = 1;

                    me->GetMotionMaster()->Clear();
                    me->StopMoving();
                    me->GetMotionMaster()->MoveSmoothPath(m_MoveID, g_WaitingPath.data(), g_WaitingPath.size(), true);
                });

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 3.388f;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                me->InterruptNonMeleeSpells(true);

                m_PulledOnce = true;

                ClearDelayedOperations();

                me->GetMotionMaster()->Clear();
                me->StopMoving();

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Unit* l_Victim = me->getVictim())
                        me->GetMotionMaster()->MoveChase(l_Victim);
                });

                Talk(eSays::SayAggro);

                _EnterCombat();

                m_AnomalyPhaseID = -1;

                SetAnomalySpeed(false, true);

                DoCast(me, eSpells::SpellPassageOfTime, true);

                if (instance)
                    instance->DoCastSpellOnPlayers(eSpells::SpellPassageOfTime);

                if (IsMythic())
                    m_FullPowerTimer = 360 * TimeConstants::IN_MILLISECONDS;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eSays::SayDeath);

                _JustDied();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellPassageOfTime);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseAbsorb);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseGreen);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseYellow);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseRed);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellChronometricParticles);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeBomb);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTemporalCharge);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTemporalCharge2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSpeedNormal);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSpeedSlow);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSpeedFast);
                }
            }

            void EnterEvadeMode() override
            {
                me->GetMotionMaster()->Clear();
                me->StopMoving();

                me->InterruptNonMeleeSpells(true);

                BossAI::EnterEvadeMode();

                Talk(eSays::SayWipe);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellPassageOfTime);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseAbsorb);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseGreen);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseYellow);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeReleaseRed);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellChronometricParticles);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeBomb);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTemporalCharge);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTemporalCharge2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSpeedNormal);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSpeedSlow);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSpeedFast);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                /// Cosmetic moves or Full Power orbiting
                if (p_ID == m_MoveID && (!me->isInCombat() || m_FullPower))
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();
                        me->GetMotionMaster()->MoveSmoothPath(++m_MoveID, g_WaitingPath.data(), g_WaitingPath.size(), true);
                    });
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                if (!m_FullPower)
                    return;

                p_Velocity = baseMoveSpeed[UnitMoveType::MOVE_RUN] * 3.0f;
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(eSays::SaySlayPlayer);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellPowerOverwhelming)
                    m_PhaseOverwhelming = true;
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                /// Temporal Rift
                if (p_SpellInfo->Id == eSpells::SpellTemporalSmash2 && m_PhaseOverwhelming)
                {
                    m_PhaseOverwhelming = false;
                    m_FirstPowerFinished = true;

                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    p_Caster->CastSpell(p_Caster, eSpells::SpellTemporalSmash, true);
                    p_Caster->CastSpell(me, eSpells::SpellTemporalSmashMod, true);

                    me->InterruptNonMeleeSpells(false);
                    me->RemoveAurasDueToSpell(eSpells::SpellPowerOverwhelmingMod);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        if (Unit* l_Target = me->getVictim())
                            AttackStart(l_Target);
                    });

                    if (!IsMythic())
                    {
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (me->isAlive() && me->isInCombat())
                                SetAnomalySpeed();
                        });
                    }
                    else
                    {
                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (me->isAlive() && me->isInCombat())
                                SetAnomalySpeed(true);
                        });
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellBurstOfTime:
                    {
                        DoCast(p_Target, m_BurstSpell, true);
                        break;
                    }
                    case eSpells::SpellTimeBombFilter:
                    {
                        sCreatureTextMgr->SendChat(me, eSays::SayTimeBomb, p_Target->GetGUID(), CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL);
                        DoCast(p_Target, eSpells::SpellTimeBomb, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnCalculateCastingTime(SpellInfo const* p_SpellInfo, int32& p_CastingTime) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellPowerOverwhelming:
                    {
                        switch (m_AnomalySpeed)
                        {
                            case eSpells::SpellSpeedNormal:
                            {
                                p_CastingTime = 4 * TimeConstants::IN_MILLISECONDS;
                                break;
                            }
                            case eSpells::SpellSpeedFast:
                            {
                                p_CastingTime = 3 * TimeConstants::IN_MILLISECONDS;
                                break;
                            }
                            default:
                            {
                                if (IsMythic())
                                    p_CastingTime = 6 * TimeConstants::IN_MILLISECONDS;
                                else
                                    p_CastingTime = 7 * TimeConstants::IN_MILLISECONDS;

                                break;
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case eData::DataAnomalySpeed:
                        return m_AnomalySpeed;
                    case eData::DataAnomalyOverwhelming:
                        return m_PhaseOverwhelming;
                }

                return 0;
            }

            void SetAnomalySpeed(bool p_MythicFast = false, bool p_WarnOnly = false)
            {
                events.Reset();
                m_Events.Reset();

                m_TimeBombCounter = 0;
                m_TimeReleaseCounter = 0;
                m_TemporalOrbCounter = 0;

                if (!p_MythicFast)
                {
                    ++m_AnomalyPhaseID;

                    if (m_AnomalySpeed == eSpells::SpellSpeedNormal)
                        m_AnomalySpeed = eSpells::SpellSpeedSlow;
                    else if (m_AnomalySpeed == eSpells::SpellSpeedSlow)
                        m_AnomalySpeed = eSpells::SpellSpeedFast;
                    else
                        m_AnomalySpeed = eSpells::SpellSpeedNormal;
                }
                else
                {
                    ++m_AnomalyPhaseID;

                    /// As always, the fight begins with Speed: Normal, then switches automatically to Speed: Fast
                    if (m_AnomalySpeed == eSpells::SpellSpeedNormal)
                        m_AnomalySpeed = eSpells::SpellSpeedFast;
                    /// 5 seconds after the first Temporal Smash, the fight enters Speed: Slow
                    else if (m_AnomalySpeed == eSpells::SpellSpeedFast)
                        m_AnomalySpeed = eSpells::SpellSpeedSlow;
                    /// 5 seconds after the second Temporal Smash, the fight enters Speed: Fast
                    else
                        m_AnomalySpeed = eSpells::SpellSpeedFast;
                }

                switch (m_AnomalySpeed)
                {
                    case eSpells::SpellSpeedSlow:
                    {
                        ++m_SlowCount;

                        m_BurstSpell = eSpells::SpellBurstOfTimeSlow;

                        Talk(eSays::SaySlowedTimeWarn);

                        if (!p_WarnOnly)
                            Talk(eSays::SaySlowedTime);

                        break;
                    }
                    case eSpells::SpellSpeedNormal:
                    {
                        m_BurstSpell = eSpells::SpellBurstOfTimeNormal;

                        Talk(eSays::SayNormalTimeWarn);

                        if (!p_WarnOnly)
                            Talk(eSays::SayNormalTime);

                        break;
                    }
                    case eSpells::SpellSpeedFast:
                    {
                        ++m_FastCount;

                        m_BurstSpell = eSpells::SpellBurstOfTimeFast;

                        Talk(eSays::SayFastenTimeWarn);

                        if (!p_WarnOnly)
                            Talk(eSays::SayFastenTime);

                        break;
                    }
                    default:
                        break;
                }

                events.ScheduleEvent(eEvents::EventChronometricParticles, 14 * TimeConstants::IN_MILLISECONDS);

                if (IsMythic())
                {
                    m_BurstTimer = 3 * TimeConstants::IN_MILLISECONDS;
                    m_Events.ScheduleEvent(eEvents::EventBurstOfTime, 2 * TimeConstants::IN_MILLISECONDS);
                }
                else if (IsHeroic())
                {
                    m_BurstTimer = 4 * TimeConstants::IN_MILLISECONDS;
                    m_Events.ScheduleEvent(eEvents::EventBurstOfTime, 8 * TimeConstants::IN_MILLISECONDS);
                }
                else
                {
                    m_BurstTimer = 5 * TimeConstants::IN_MILLISECONDS;
                    m_Events.ScheduleEvent(eEvents::EventBurstOfTime, 14 * TimeConstants::IN_MILLISECONDS);
                }

                auto l_DiffMap = IsMythic() ? g_MythicTimerData.find(GetDifficulty()) : g_TimerData.find(GetDifficulty());
                if (l_DiffMap == (IsMythic() ? g_MythicTimerData.end() : g_TimerData.end()))
                    return;

                auto l_PhaseIter = l_DiffMap->second.find(m_AnomalyPhaseID);
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
                        case eEvents::EventTimeRelease:
                            l_Counter = m_TimeReleaseCounter;
                            break;
                        case eEvents::EventTimeBomb:
                            l_Counter = m_TimeBombCounter;
                            break;
                        case eEvents::EventTemporalOrb:
                            l_Counter = m_TemporalOrbCounter;
                            break;
                        default:
                            break;
                    }

                    if (l_Counter >= l_EventIter->second.size())
                        continue;

                    events.ScheduleEvent(l_I, l_EventIter->second[l_Counter]);
                }

                /// Alter existing Chronometric Particles on players
                std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                for (HostileReference* l_Ref : l_ThreatList)
                {
                    if (Unit* l_Target = l_Ref->getTarget())
                    {
                        if (!l_Target->IsPlayer())
                            continue;

                        Aura* l_Aura = l_Target->GetAura(eSpells::SpellChronometricParticles, me->GetGUID());
                        if (!l_Aura || !l_Aura->GetMaxDuration())
                            continue;

                        uint32 l_Duration = 40 * TimeConstants::IN_MILLISECONDS;
                        uint32 l_Amplitude = 6 * TimeConstants::IN_MILLISECONDS;

                        switch (m_AnomalySpeed)
                        {
                            case eSpells::SpellSpeedNormal:
                            {
                                l_Duration = 30 * TimeConstants::IN_MILLISECONDS;
                                l_Amplitude = 2 * TimeConstants::IN_MILLISECONDS;
                                break;
                            }
                            case eSpells::SpellSpeedFast:
                            {
                                l_Duration = 24 * TimeConstants::IN_MILLISECONDS;
                                l_Amplitude = 1 * TimeConstants::IN_MILLISECONDS;
                                break;
                            }
                            default:
                                break;
                        }

                        float l_RemainingPct = float(l_Aura->GetDuration()) / float(l_Aura->GetMaxDuration()) * 100.0f;

                        l_Aura->SetMaxDuration(l_Duration);

                        l_Duration = CalculatePct(float(l_Duration), l_RemainingPct);

                        l_Aura->SetDuration(l_Duration);

                        if (AuraEffect* l_AurEff = l_Aura->GetEffect(SpellEffIndex::EFFECT_0))
                        {
                            l_RemainingPct = float(l_AurEff->GetPeriodicTimer()) / float(l_AurEff->GetAmplitude()) * 100.0f;

                            l_AurEff->SetAmplitude(l_Amplitude);

                            l_Amplitude = CalculatePct(l_Amplitude, l_RemainingPct);

                            l_AurEff->SetPeriodicTimer(l_Amplitude);
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetExactDist(&g_CenterPos) > 120.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                if (m_FullPowerTimer)
                {
                    if (m_FullPowerTimer <= p_Diff)
                    {
                        m_FullPowerTimer = 0;

                        m_FullPower = true;

                        me->InterruptNonMeleeSpells(true);

                        DoCast(me, eSpells::SpellFullPowerPeriodic, true);

                        me->AttackStop();
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_MoveID = 1;

                            me->SetWalk(false);
                            me->GetMotionMaster()->Clear();
                            me->StopMoving();
                            me->GetMotionMaster()->MoveSmoothPath(m_MoveID, g_WaitingPath.data(), g_WaitingPath.size(), true);
                        });
                    }
                    else
                        m_FullPowerTimer -= p_Diff;
                }

                events.Update(p_Diff);
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventBurstOfTime:
                    {
                        /// Anomaly continues Burst of Time during Power Overwhelming, after a first phase
                        if (IsMythic() && (!m_PhaseOverwhelming || m_FirstPowerFinished))
                            DoCast(me, eSpells::SpellBurstOfTime, true);
                        else if (!m_PhaseOverwhelming)
                            DoCast(me, eSpells::SpellBurstOfTime, true);

                        m_Events.ScheduleEvent(eEvents::EventBurstOfTime, m_BurstTimer);
                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventChronometricParticles:
                    {
                        if (uint32(time(nullptr)) >= m_LastTalkTimer + 10)
                        {
                            Talk(eSays::SayFragmentedTimeParticle);
                            m_LastTalkTimer = uint32(time(nullptr));
                        }

                        DoCastVictim(eSpells::SpellChronometricParticles);
                        events.ScheduleEvent(eEvents::EventChronometricParticles, 6 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTimeRelease:
                    {
                        DoCast(me, eSpells::SpellTimeReleaseFilter, true);

                        m_TimeReleaseCounter++;

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventTimeRelease))
                            events.ScheduleEvent(eEvents::EventTimeRelease, l_Timer);

                        break;
                    }
                    case eEvents::EventTimeBomb:
                    {
                        DoCast(me, eSpells::SpellTimeBombFilter, true);

                        m_TimeBombCounter++;

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventTimeBomb))
                            events.ScheduleEvent(eEvents::EventTimeBomb, l_Timer);


                        break;
                    }
                    case eEvents::EventSumSlowAdd:
                    {
                        Talk(eSays::SaySummonSlowAdd);

                        DoCast(me, eSpells::SpellSumSlowAddFilter, true);

                        uint8 l_Counter = 0;
                        uint8 l_RandCounter = urand(6, 7);

                        std::list<Creature*> l_Triggers;
                        for (uint64 l_Guid : m_TriggerList)
                        {
                            if (Creature* l_Trigger = Creature::GetCreature(*me, l_Guid))
                                l_Triggers.push_back(l_Trigger);
                        }

                        l_Triggers.sort(JadeCore::UnitSortDistance(true, me));

                        Creature* l_FirstTrigger;

                        for (auto const& l_Creature : l_Triggers)
                        {
                            l_FirstTrigger = l_Creature;
                            break;
                        }

                        l_Triggers.sort(JadeCore::UnitSortDistance(true, l_FirstTrigger));

                        for (auto const& l_Creature : l_Triggers)
                        {
                            l_Counter++;

                            if (IsMythic())
                            {
                                if (l_Counter == 6 || l_Counter == 7)
                                {
                                    me->PlayOrphanSpellVisual(me->GetPosition().AsVector3(), G3D::Vector3(), l_Creature->GetPosition().AsVector3(), eAnomalyData::ParticleVisual, 3.0f);

                                    uint64 l_Guid = l_Creature->GetGUID();
                                    AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                                    {
                                        if (Creature* l_Sum = ObjectAccessor::GetCreature(*me, l_Guid))
                                            DoCast(l_Sum, eSpells::SpellSumSlowAdd, true);
                                    });
                                }
                            }
                            else
                            {
                                if (l_Counter == l_RandCounter)
                                {
                                    me->PlayOrphanSpellVisual(me->GetPosition().AsVector3(), G3D::Vector3(), l_Creature->GetPosition().AsVector3(), eAnomalyData::ParticleVisual, 3.0f);

                                    uint64 l_Guid = l_Creature->GetGUID();
                                    AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                                    {
                                        if (Creature* l_Sum = ObjectAccessor::GetCreature(*me, l_Guid))
                                            DoCast(l_Sum, eSpells::SpellSumSlowAdd, true);
                                    });
                                }
                            }
                        }

                        break;
                    }
                    case eEvents::EventTemporalOrb:
                    {
                        DoCast(eSpells::SpellTemporalOrb);

                        m_TemporalOrbCounter++;

                        if (uint32 l_Timer = GetEventTimer(eEvents::EventTemporalOrb))
                            events.ScheduleEvent(eEvents::EventTemporalOrb, l_Timer);

                        break;
                    }
                    case eEvents::EventPowerOverwhelming:
                    {
                        Talk(eSays::SayPowerOverwhelming);

                        events.Reset();

                        me->StopAttack();
                        me->SetGuidValue(EUnitFields::UNIT_FIELD_TARGET, 0);
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        DoCast(eSpells::SpellPowerOverwhelming);
                        break;
                    }
                    case eEvents::EventChangeSpeed:
                    {
                        SetAnomalySpeed(true);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            uint32 GetEventTimer(uint8 p_EventID) const
            {
                auto l_DiffMap = IsMythic() ? g_MythicTimerData.find(GetDifficulty()) : g_TimerData.find(GetDifficulty());
                if (l_DiffMap == (IsMythic() ? g_MythicTimerData.end() : g_TimerData.end()))
                    return 0;

                auto l_PhaseIter = l_DiffMap->second.find(m_AnomalyPhaseID);
                if (l_PhaseIter == l_DiffMap->second.end())
                    return 0;

                auto l_EventIter = l_PhaseIter->second.find(p_EventID);
                if (l_EventIter == l_PhaseIter->second.end())
                    return 0;

                uint8 l_Counter = 0;
                switch (p_EventID)
                {
                    case eEvents::EventTimeRelease:
                        l_Counter = m_TimeReleaseCounter;
                        break;
                    case eEvents::EventTimeBomb:
                        l_Counter = m_TimeBombCounter;
                        break;
                    case eEvents::EventTemporalOrb:
                        l_Counter = m_TemporalOrbCounter;
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
            return new boss_chronomatic_anomalyAI(p_Creature);
        }
};

/// Waning Time Particle - 104676
class npc_anomaly_waning_time_particle : public CreatureScript
{
    public:
        npc_anomaly_waning_time_particle() : CreatureScript("npc_anomaly_waning_time_particle") { }

        struct npc_anomaly_waning_time_particleAI : public ScriptedAI
        {
            npc_anomaly_waning_time_particleAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_STATE, AuraType::SPELL_AURA_MOD_CONFUSE, true);
            }

            InstanceScript* m_Instance;
            bool m_Despawn;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 1.6f;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE, p_Summoner->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE));

                DoCast(me, eSpells::SpellPassageOfTime, true);

                m_Despawn = false;

                if (m_Instance)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(c_events::EVENT_1, 2 * TimeConstants::IN_MILLISECONDS);
            }

            void NonInterruptCast(Unit* p_Caster, uint32 p_SpellID, uint32 /*p_SchoolMask*/) override
            {
                if (p_SpellID == eSpells::SpellWarpNightwellBig)
                {
                    /// If LFR or Normal difficulties, adds can be interrupted all the time
                    /// Otherwise, adds become uninterruptible below 30% health
                    if (!IsHeroicOrMythic() || me->GetHealthPct() > 30)
                    {
                        me->InterruptNonMeleeSpells(false);
                        events.ScheduleEvent(c_events::EVENT_2, 1 * TimeConstants::IN_MILLISECONDS);
                        AttackStart(p_Caster);
                    }
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;
                    me->SetHealth(me->CountPctFromMaxHealth(5));

                    if (!m_Despawn)
                    {
                        m_Despawn = true;

                        events.Reset();

                        me->InterruptNonMeleeSpells(false);
                        me->AttackStop();

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                        DoCast(me, eSpells::SpellFadeOut, true);

                        if (m_Instance)
                            m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            me->CastSpell(me, eSpells::SpellSumTemporalRift, true, nullptr, nullptr, l_Owner->GetGUID());

                            Position l_Pos;
                            float l_Angle = M_PI / 2.0f;
                            for (uint8 i = 0; i < 4; i++)
                            {
                                me->GetNearPosition(l_Pos, 6.0f, l_Angle);
                                l_Owner->SummonCreature(eCreatures::NpcFragmentedTime, l_Pos);
                                l_Angle += M_PI / 2.0f;
                            }
                        }

                        events.ScheduleEvent(c_events::EVENT_3, 3 * TimeConstants::IN_MILLISECONDS);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case c_events::EVENT_1:
                    {
                        me->AttackStop();
                        if (Creature* l_Nightwell = me->FindNearestCreature(eCreatures::NpcTheNightwell, 100.0f))
                            me->SetFacingTo(me->GetAngle(l_Nightwell));
                        DoCast(eSpells::SpellWarpNightwellBig);
                        events.ScheduleEvent(c_events::EVENT_1, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_events::EVENT_2:
                    {
                        DoCastVictim(eSpells::SpellChronomate);
                        break;
                    }
                    case c_events::EVENT_3:
                    {
                        me->DespawnOrUnsummon(100);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_anomaly_waning_time_particleAI(p_Creature);
        }
};

/// Fragmented Time Particle - 114671
class npc_anomaly_fragmented_time : public CreatureScript
{
    public:
        npc_anomaly_fragmented_time() : CreatureScript("npc_anomaly_fragmented_time") { }

        struct npc_anomaly_fragmented_timeAI : public ScriptedAI
        {
            npc_anomaly_fragmented_timeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 1.6f;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE, p_Summoner->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE));

                events.ScheduleEvent(c_events::EVENT_1, 1 * TimeConstants::IN_MILLISECONDS);
            }

            void NonInterruptCast(Unit* p_Caster, uint32 p_SpellID, uint32 /*p_SchoolMask*/) override
            {
                if (p_SpellID == eSpells::SpellWarpNightwellSmall)
                {
                    /// If LFR or Normal difficulties, adds can be interrupted all the time
                    /// Otherwise, adds become uninterruptible below 30% health
                    if (!IsHeroicOrMythic() || me->GetHealthPct() > 30)
                    {
                        me->InterruptNonMeleeSpells(false);
                        events.ScheduleEvent(c_events::EVENT_1, 1 * TimeConstants::IN_MILLISECONDS);
                        AttackStart(p_Caster);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case c_events::EVENT_1:
                    {
                        DoCast(eSpells::SpellWarpNightwellSmall);
                        events.ScheduleEvent(c_events::EVENT_1, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_anomaly_fragmented_timeAI(p_Creature);
        }
};

/// Temporal Rift - 106878
class npc_anomaly_temporal_rift : public CreatureScript
{
    public:
        npc_anomaly_temporal_rift() : CreatureScript("npc_anomaly_temporal_rift") { }

        struct npc_anomaly_temporal_riftAI : public ScriptedAI
        {
            npc_anomaly_temporal_riftAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                m_Click = false;
            }

            bool m_Click;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellTemporalRiftVis, true);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (!m_Click && !p_Clicker->HasAura(eSpells::SpellTemporalCharge))
                {
                    m_Click = true;
                    p_Clicker->CastSpell(p_Clicker, eSpells::SpellTemporalCharge2, true);
                    me->DespawnOrUnsummon(100);
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_anomaly_temporal_riftAI(p_Creature);
        }
};

/// Passage of Time - 205653
class spell_anomaly_passage_of_time : public SpellScriptLoader
{
    public:
        spell_anomaly_passage_of_time() : SpellScriptLoader("spell_anomaly_passage_of_time") { }

        class spell_anomaly_passage_of_time_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_passage_of_time_AuraScript);

            uint32 m_TempData = 0;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Anomaly = l_Instance->instance->GetCreature(l_Instance->GetData64(eCreatures::NpcChromaticAnomaly)))
                    {
                        if (l_Anomaly->IsAIEnabled && l_Anomaly->isInCombat() && m_TempData != l_Anomaly->AI()->GetData(eData::DataAnomalySpeed))
                        {
                            m_TempData = l_Anomaly->GetAI()->GetData(eData::DataAnomalySpeed);
                            GetCaster()->CastSpell(GetCaster(), m_TempData, true);

                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_TempData))
                            {
                                if (AuraEffect* p_AurEffb = GetAura()->GetEffect(EFFECT_2))
                                    p_AurEffb->ChangeAmount(l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_anomaly_passage_of_time_AuraScript::OnTick, SpellEffIndex::EFFECT_3, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_passage_of_time_AuraScript();
        }
};

/// Burst of Time - 219984
class spell_anomaly_burst_of_time : public SpellScriptLoader
{
    public:
        spell_anomaly_burst_of_time() : SpellScriptLoader("spell_anomaly_burst_of_time") { }

        class spell_anomaly_burst_of_time_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anomaly_burst_of_time_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!GetCaster())
                    return;

                uint8 l_PlayerCount = GetCaster()->GetMap()->GetPlayerCount();
                uint8 l_Count;

                if (l_PlayerCount > 25)
                    l_Count = 5;
                else if (l_PlayerCount > 15)
                    l_Count = 4;
                else
                    l_Count = 3;

                if (p_Targets.size() > l_Count)
                    JadeCore::RandomResizeList(p_Targets, l_Count);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anomaly_burst_of_time_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_anomaly_burst_of_time_SpellScript();
        }
};

/// Chronometric Particles - 206607
class spell_anomaly_chronometric_particles : public SpellScriptLoader
{
    public:
        spell_anomaly_chronometric_particles() : SpellScriptLoader("spell_anomaly_chronometric_particles") { }

        class spell_anomaly_chronometric_particles_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_chronometric_particles_AuraScript);

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget() || !GetTarget()->isAlive())
                    return;

                if (Aura* l_Aura = p_AurEff->GetBase())
                {
                    if (l_Aura->GetStackAmount() > GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints)
                        GetCaster()->CastSpell(GetTarget(), eSpells::SpellChronometricOverload, true);
                }
            }

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (!GetCaster() || !GetCaster()->GetAI())
                    return;

                uint32 l_Data = GetCaster()->GetAI()->GetData(eData::DataAnomalySpeed);

                if (l_Data == eSpells::SpellSpeedNormal)
                    p_Duration = 30 * TimeConstants::IN_MILLISECONDS;
                else if (l_Data == eSpells::SpellSpeedFast)
                    p_Duration = 24 * TimeConstants::IN_MILLISECONDS;
                else
                    p_Duration = 40 * TimeConstants::IN_MILLISECONDS;
            }

            void CalcPeriodic(AuraEffect const* /*p_AurEff*/, bool& /*p_IsPeriodic*/, int32& p_Amplitude)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->GetAI())
                    return;

                uint32 l_Data = GetCaster()->GetAI()->GetData(eData::DataAnomalySpeed);

                if (l_Data == eSpells::SpellSpeedNormal)
                    p_Amplitude = 2 * TimeConstants::IN_MILLISECONDS;
                else if (l_Data == eSpells::SpellSpeedFast)
                    p_Amplitude = 1 * TimeConstants::IN_MILLISECONDS;
                else
                    p_Amplitude = 6 * TimeConstants::IN_MILLISECONDS;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_anomaly_chronometric_particles_AuraScript::AfterApply, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAPPLY);
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_anomaly_chronometric_particles_AuraScript::CalculateMaxDuration);
                DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_anomaly_chronometric_particles_AuraScript::CalcPeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_chronometric_particles_AuraScript();
        }
};

/// Time Release - 206610
class spell_anomaly_time_release_filter : public SpellScriptLoader
{
    public:
        spell_anomaly_time_release_filter() : SpellScriptLoader("spell_anomaly_time_release_filter") { }

        class spell_anomaly_time_release_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anomaly_time_release_filter_SpellScript);

            uint8 m_Count;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (GetCaster())
                {
                    if (GetCaster()->HasAura(eSpells::SpellSpeedNormal))
                        m_Count = 4;
                    else if (GetCaster()->HasAura(eSpells::SpellSpeedFast))
                        m_Count = 2;
                    else
                        m_Count = 0;

                    if (m_Count != 0 && p_Targets.size() > m_Count)
                        JadeCore::RandomResizeList(p_Targets, m_Count);
                }
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                float l_Mod = 1.0f;
                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Anomaly = l_Instance->instance->GetCreature(l_Instance->GetData64(eCreatures::NpcChromaticAnomaly)))
                    {
                        if (l_Anomaly->IsAIEnabled)
                        {
                            if (l_Anomaly->GetAI()->GetData(eData::DataAnomalySpeed) == eSpells::SpellSpeedSlow)
                                l_Mod = 0.7f;
                            else if (l_Anomaly->GetAI()->GetData(eData::DataAnomalySpeed) == eSpells::SpellSpeedFast)
                                l_Mod = 1.3f;
                        }
                    }
                }

                int32 l_BP = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].CalcValue(l_Caster) * l_Mod;
                l_Caster->CastCustomSpell(l_Target, eSpells::SpellTimeReleaseAbsorb, &l_BP, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anomaly_time_release_filter_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_anomaly_time_release_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_anomaly_time_release_filter_SpellScript();
        }
};

/// Time Release - 206609
class spell_anomaly_time_release : public SpellScriptLoader
{
    public:
        spell_anomaly_time_release() : SpellScriptLoader("spell_anomaly_time_release") { }

        class spell_anomaly_time_release_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_time_release_AuraScript);

            uint32 m_Absorb = 0;

            void Absorb(AuraEffect* p_AurEff, DamageInfo& /*p_DmgInfo*/, uint32& p_AbsorbAmount)
            {
                if (GetTarget())
                {
                    int32 l_NewAbsorb = p_AurEff->GetAmount() - p_AbsorbAmount;

                    if (l_NewAbsorb > m_Absorb * 0.66)
                    {
                        if (GetTarget()->HasAura(eSpells::SpellTimeReleaseRed))
                        {
                            if (AuraEffect* l_AurEff = GetTarget()->GetAuraEffect(eSpells::SpellTimeReleaseRed, SpellEffIndex::EFFECT_0))
                                l_AurEff->SetAmount(l_NewAbsorb);
                        }
                        else
                            GetTarget()->CastCustomSpell(eSpells::SpellTimeReleaseRed, SpellValueMod::SPELLVALUE_BASE_POINT0, l_NewAbsorb);
                    }
                    else if (l_NewAbsorb > m_Absorb * 0.33)
                    {
                        if (GetTarget()->HasAura(eSpells::SpellTimeReleaseYellow))
                        {
                            if (AuraEffect* l_AurEff = GetTarget()->GetAuraEffect(eSpells::SpellTimeReleaseYellow, SpellEffIndex::EFFECT_0))
                                l_AurEff->SetAmount(l_NewAbsorb);
                        }
                        else
                            GetTarget()->CastCustomSpell(eSpells::SpellTimeReleaseYellow, SpellValueMod::SPELLVALUE_BASE_POINT0, l_NewAbsorb);
                    }
                    else
                    {
                        if (GetTarget()->HasAura(eSpells::SpellTimeReleaseGreen))
                        {
                            if (AuraEffect* l_AurEff = GetTarget()->GetAuraEffect(eSpells::SpellTimeReleaseGreen, SpellEffIndex::EFFECT_0))
                                l_AurEff->SetAmount(l_NewAbsorb);
                        }
                        else
                            GetTarget()->CastCustomSpell(eSpells::SpellTimeReleaseGreen, SpellValueMod::SPELLVALUE_BASE_POINT0, l_NewAbsorb);
                    }
                }
            }

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget() && !m_Absorb)
                {
                    m_Absorb = p_AurEff->GetAmount();
                    GetTarget()->CastCustomSpell(eSpells::SpellTimeReleaseRed, SpellValueMod::SPELLVALUE_BASE_POINT0, m_Absorb);
                }
            }

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTarget())
                {
                    GetTarget()->RemoveAurasDueToSpell(eSpells::SpellTimeReleaseGreen);
                    GetTarget()->RemoveAurasDueToSpell(eSpells::SpellTimeReleaseRed);
                    GetTarget()->RemoveAurasDueToSpell(eSpells::SpellTimeReleaseYellow);

                    if (GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    {
                        int32 l_BP = p_AurEff->GetAmount();

                        if (l_BP > 0)
                        {
                            if (Unit* l_Caster = GetCaster())
                                l_Caster->CastCustomSpell(GetTarget(), eSpells::SpellTimeReleaseDmg, &l_BP, 0, 0, true);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_anomaly_time_release_AuraScript::Absorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_HEAL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_anomaly_time_release_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_HEAL_ABSORB, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_anomaly_time_release_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_HEAL_ABSORB, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_time_release_AuraScript();
        }
};

/// Time Release - 219964, 219965, 219966, 206609
class spell_anomaly_time_release_duration : public SpellScriptLoader
{
    public:
        spell_anomaly_time_release_duration() : SpellScriptLoader("spell_anomaly_time_release_duration") { }

        class spell_anomaly_time_release_duration_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_time_release_duration_AuraScript);

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (!GetCaster())
                    return;

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* l_Anomaly = l_Instance->instance->GetCreature(l_Instance->GetData64(eCreatures::NpcChromaticAnomaly)))
                    {
                        if (l_Anomaly->IsAIEnabled)
                        {
                            if (l_Anomaly->GetAI()->GetData(eData::DataAnomalySpeed) == eSpells::SpellSpeedSlow)
                                p_Duration = 22 * TimeConstants::IN_MILLISECONDS;
                            else if (l_Anomaly->GetAI()->GetData(eData::DataAnomalySpeed) == eSpells::SpellSpeedNormal)
                                p_Duration = 18 * TimeConstants::IN_MILLISECONDS;
                            else
                                p_Duration = 15 * TimeConstants::IN_MILLISECONDS;
                        }
                    }
                }
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_anomaly_time_release_duration_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_time_release_duration_AuraScript();
        }
};

/// Time Bomb - 206618
class spell_anomaly_time_bomb_filter : public SpellScriptLoader
{
    public:
        spell_anomaly_time_bomb_filter() : SpellScriptLoader("spell_anomaly_time_bomb_filter") { }

        class spell_anomaly_time_bomb_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anomaly_time_bomb_filter_SpellScript);

            uint8 m_Count;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (GetCaster() && !p_Targets.empty())
                {
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object || !p_Object->IsPlayer())
                            return true;

                        if (p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                            return true;

                        return false;
                    });

                    if (GetCaster()->GetMap()->IsMythic())
                        m_Count = 2;
                    else
                        m_Count = 1;

                    if (p_Targets.size() > m_Count)
                        JadeCore::RandomResizeList(p_Targets, m_Count);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anomaly_time_bomb_filter_SpellScript::FilterTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_anomaly_time_bomb_filter_SpellScript();
        }
};

/// Time Bomb - 206617
class spell_anomaly_time_bomb : public SpellScriptLoader
{
    public:
        spell_anomaly_time_bomb() : SpellScriptLoader("spell_anomaly_time_bomb") { }

        enum eBombSpells
        {
            SpellActivated      = 216047,
            SpellTimeBombVisual = 212845
        };

        class spell_anomaly_time_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_time_bomb_AuraScript);

            bool m_VisualApplied = false;

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (!GetCaster() || !GetCaster()->GetAI())
                    return;

                uint32 l_Data = GetCaster()->GetAI()->GetData(eData::DataAnomalySpeed);

                if (l_Data == eSpells::SpellSpeedNormal)
                    p_Duration = 20 * TimeConstants::IN_MILLISECONDS;
                else if (l_Data == eSpells::SpellSpeedFast)
                    p_Duration = 6 * TimeConstants::IN_MILLISECONDS;
                else
                    p_Duration = 100 * TimeConstants::IN_MILLISECONDS;
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        l_Target->RemoveAura(eBombSpells::SpellTimeBombVisual);

                        l_Target->CastSpell(l_Target, eSpells::SpellTimeBombDmg, true);

                        if (!l_Target->GetMap()->IsLFR())
                        {
                            if (Creature* l_Trigger = l_Target->FindNearestCreature(eCreatures::NpcSuramarTrigger, 3.0f))
                            {
                                if (!l_Trigger->HasAura(eBombSpells::SpellActivated))
                                {
                                    l_Trigger->CastSpell(l_Trigger, eBombSpells::SpellActivated, true);

                                    if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                                        l_Instance->SetData(eData::DataAnomalyAchiev, 1);
                                }
                            }
                        }
                    }
                }
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                /// Remove 1, because of SPELL_ATTR5_START_PERIODIC_AT_APPLY
                int32 l_TickNumber = p_AurEff->GetTickNumber() - 1;

                if (!m_VisualApplied && (p_AurEff->GetTotalTicks() - l_TickNumber) <= 6)
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        if (Unit* l_Target = GetTarget())
                        {
                            m_VisualApplied = true;

                            l_Caster->CastSpell(l_Target, eBombSpells::SpellTimeBombVisual, true);
                        }
                    }
                }
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_anomaly_time_bomb_AuraScript::CalculateMaxDuration);
                AfterEffectRemove += AuraEffectRemoveFn(spell_anomaly_time_bomb_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_anomaly_time_bomb_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_time_bomb_AuraScript();
        }
};

/// Time Bomb - 206615
class spell_anomaly_time_bomb_dmg : public SpellScriptLoader
{
    public:
        spell_anomaly_time_bomb_dmg() : SpellScriptLoader("spell_anomaly_time_bomb_dmg") { }

        class spell_anomaly_time_bomb_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anomaly_time_bomb_dmg_SpellScript);

            void DealDamage()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                float l_Distance    = GetCaster()->GetExactDist2d(GetHitUnit());
                float l_Radius      = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].CalcRadius(GetCaster());

                uint32 l_Percent;

                if (l_Distance < 15.0f)
                    l_Percent = 100 - (l_Distance / 10) * 30;
                else
                    l_Percent = 50 - (l_Distance / l_Radius) * 30;

                int32 l_Damage = CalculatePct(GetHitDamage(), l_Percent);

                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_anomaly_time_bomb_dmg_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_anomaly_time_bomb_dmg_SpellScript();
        }
};

/// Temporal Orbs - 219815
class spell_anomaly_temporal_orb : public SpellScriptLoader
{
    public:
        spell_anomaly_temporal_orb() : SpellScriptLoader("spell_anomaly_temporal_orb") { }

        class spell_anomaly_temporal_orb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_temporal_orb_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target || !l_Caster->isAlive() || !l_Caster->isInCombat())
                    return;

                Position l_Pos      = l_Target->GetPosition();
                bool l_FirstTick    = p_AurEff->GetTickNumber() == 1;
                float l_Step        = 2.0f * M_PI / 36.0f;
                uint32 l_SpellID    = l_FirstTick ? eSpells::SpellTemporalOrbAT2 : eSpells::SpellTemporalOrbAT;

                for (uint8 l_I = 0; l_I < 36; ++l_I)
                {
                    Position l_TmpPos = l_Pos;

                    if (!l_FirstTick)
                    {
                        l_TmpPos.m_orientation += l_Step / 2.0f;

                        Position::NormalizeOrientation(l_TmpPos.m_orientation);
                    }

                    l_Caster->CastSpell(l_TmpPos, l_SpellID, true);

                    l_Pos.m_orientation += l_Step;

                    Position::NormalizeOrientation(l_Pos.m_orientation);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_anomaly_temporal_orb_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_temporal_orb_AuraScript();
        }
};

/// Temporal Smash - 212109
class spell_anomaly_temporal_smash : public SpellScriptLoader
{
    public:
        spell_anomaly_temporal_smash() : SpellScriptLoader("spell_anomaly_temporal_smash") { }

        class spell_anomaly_temporal_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anomaly_temporal_smash_SpellScript);

            SpellCastResult CheckRequirement()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                if (Creature* l_Creature = l_Target->ToCreature())
                {
                    if (!l_Creature || l_Creature->GetEntry() != eCreatures::NpcChromaticAnomaly || !l_Creature->IsAIEnabled || !l_Creature->AI()->GetData(eData::DataAnomalyOverwhelming))
                        return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_anomaly_temporal_smash_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_anomaly_temporal_smash_SpellScript();
        }
};

/// Power Overwhelming - 211927
class spell_anomaly_power_overwhelming : public SpellScriptLoader
{
    public:
        spell_anomaly_power_overwhelming() : SpellScriptLoader("spell_anomaly_power_overwhelming") { }

        enum eSpell
        {
            SpellPowerOverwhelmingTriggered = 219823
        };

        class spell_anomaly_power_overwhelming_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_power_overwhelming_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSpell::SpellPowerOverwhelmingTriggered, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_anomaly_power_overwhelming_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_power_overwhelming_AuraScript();
        }
};

/// Temporal Charge - 226845
class spell_anomaly_temporal_charge : public SpellScriptLoader
{
    public:
        spell_anomaly_temporal_charge() : SpellScriptLoader("spell_anomaly_temporal_charge") { }

        class spell_anomaly_temporal_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_anomaly_temporal_charge_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_Mode = GetTargetApplication()->GetRemoveMode();
                if (l_Mode != AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                    return;

                InstanceScript* l_Instance = GetTarget()->GetInstanceScript();
                if (!l_Instance)
                    return;

                Creature* l_Anomaly = Creature::GetCreature(*GetTarget(), l_Instance->GetData64(eCreatures::NpcChromaticAnomaly));
                if (!l_Anomaly)
                    return;

                l_Anomaly->CastSpell(GetTarget(), eSpells::SpellSumTemporalRift, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_anomaly_temporal_charge_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_OVERRIDE_SPELLS, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_anomaly_temporal_charge_AuraScript();
        }
};

/// Temporal Orb - 212874
/// Temporal Orb - 227217
class areatrigger_anomaly_temporal_orb : public AreaTriggerEntityScript
{
    public:
        areatrigger_anomaly_temporal_orb() : AreaTriggerEntityScript("areatrigger_anomaly_temporal_orb") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, p_AreaTrigger->GetSpellId() == eSpells::SpellTemporalOrbAT2 ? 16 * TimeConstants::IN_MILLISECONDS : 11 * TimeConstants::IN_MILLISECONDS);

            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, 2);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 3.0f); ///< (+200% / TimeToTargetScale / 1000) per second
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1.0f);
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            float l_PctPerMSec = (200.0f / (float(p_AreaTrigger->GetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE)) / 1000.0f) / 1000.0f);
            float l_BaseRadius = p_AreaTrigger->GetAreaTriggerInfo().Radius;
            float l_CurrRadius = p_AreaTrigger->GetRadius();

            l_CurrRadius += CalculatePct(l_BaseRadius, l_PctPerMSec * p_Time);

            p_AreaTrigger->SetRadius(std::min(l_CurrRadius, l_BaseRadius * p_AreaTrigger->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4)));
        }

        void OnDestinationReached(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetDuration(0);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_anomaly_temporal_orb();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_chronomatic_anomaly()
{
    /// Boss
    new boss_chronomatic_anomaly();

    /// Creatures
    new npc_anomaly_waning_time_particle();
    new npc_anomaly_fragmented_time();
    new npc_anomaly_temporal_rift();

    /// Spells
    new spell_anomaly_passage_of_time();
    new spell_anomaly_burst_of_time();
    new spell_anomaly_chronometric_particles();
    new spell_anomaly_time_release_filter();
    new spell_anomaly_time_release();
    new spell_anomaly_time_release_duration();
    new spell_anomaly_time_bomb_filter();
    new spell_anomaly_time_bomb();
    new spell_anomaly_time_bomb_dmg();
    new spell_anomaly_temporal_orb();
    new spell_anomaly_temporal_smash();
    new spell_anomaly_power_overwhelming();
    new spell_anomaly_temporal_charge();

    /// AreaTrigger
    new areatrigger_anomaly_temporal_orb();
}
#endif
