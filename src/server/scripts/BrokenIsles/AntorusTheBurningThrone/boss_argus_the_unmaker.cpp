////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"

Position const g_CenterPos = { 2867.12f, -4567.54f, 292.942f, 3.25418f };

struct SargerasGaze
{
    SargerasGaze(uint8 p_Fear, uint8 p_Melees, uint8 p_Ranges)
    {
        FearCount   = p_Fear;
        MeleeRages  = p_Melees;
        RangedRages = p_Ranges;
    }

    SargerasGaze()
    {
        FearCount   = 0;
        MeleeRages  = 0;
        RangedRages = 0;
    }

    uint8 FearCount;
    uint8 MeleeRages;
    uint8 RangedRages;
};

std::array<SargerasGaze const, eData::DataArgusMaxSargerasGazes + 1> g_SargerasGazePattern =
{
    {
        { 0, 0, 0 },
        { 1, 1, 0 },
        { 2, 1, 0 },
        { 2, 1, 1 },
        { 3, 1, 1 },
        { 3, 2, 1 },
        { 4, 2, 1 },
        { 4, 2, 2 },
        { 5, 2, 2 }
    }
};

/// Argus the Unmaker - 124828
class boss_argus_the_unmaker : public CreatureScript
{
    public:
        boss_argus_the_unmaker() : CreatureScript("boss_argus_the_unmaker") { }

        struct boss_argus_the_unmakerAI : BossAI
        {
            boss_argus_the_unmakerAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataArgusTheUnmaker) { }

            enum eSpells
            {
                /// Misc
                SpellTitanicEssence             = 258040,
                SpellArgusP1EnergyController    = 258041,
                SpellArgusP2EnergyController    = 258042,
                SpellArgusP4EnergyController    = 258044,
                SpellGiftOfTheSea               = 258647,
                SpellGiftOfTheSky               = 258646,
                SpellGiftOfTheSeaAT             = 257306,
                SpellGiftOfTheSkyAT             = 257313,
                SpellStrengthOfTheSea           = 253901,
                SpellStrengthOfTheSky           = 253903,
                SpellAvatarOfAggramar           = 255199,
                SpellCancelAvatarOfAggramar     = 259033,
                SpellBreakTargeting             = 140562,
                SpellArcaneDissolveOut          = 255976,
                SpellArcaneDissolveOutSecond    = 256543,
                SpellTemporalBlast              = 257645,
                SpellCosmicRayAura              = 252729,
                SpellCosmicBeaconCast           = 252616,
                SpellReapSoulBlue               = 256542,
                SpellGraspOfTheUnmaker          = 258373,
                SpellReapSoulInstakill          = 258399,
                SpellGiftOfTheLifebinder        = 257619,
                SpellEndOfAllThings             = 256544,
                SpellEmberOfRagePeriodic        = 257300,
                SpellEmberOfRageDoT             = 257299,
                SpellArgusMythicTransform       = 258104,
                SpellShatteredBonds             = 258000,
                SpellEdgeOfAnnihilationDoT      = 258834,
                SpellArgusSummoningMovie        = 257605,
                SpellVolatileSoul               = 252280,
                SpellTitanforgedBuff            = 257215,
                /// Sweeping Scythe
                SpellSweepingScythe             = 248499,
                /// Cone of Death
                SpellConeOfDeathSearcher        = 256457,
                SpellConeOfDeathCast            = 248165,
                SpellConeOfDeathAT              = 248166,
                /// Soulblight Orb
                SpellSoulblightOrbCast          = 248317,
                SpellSoulblightDoT              = 248396,
                SpellDeathFogAT                 = 248290,
                /// Tortured Rage
                SpellTorturedRageCast           = 257296,
                /// Soulburst
                SpellSoulburstAura              = 250669,
                /// Soulbomb
                SpellSoulbombAura               = 251570,
                /// Edge of Obliteration
                SpellEdgeOfObliterationCast     = 255826,
                SpellEdgeOfObliterationDoT      = 251815,
                SpellEdgeOfObliterationWarning  = 252136,
                /// Deadly Scythe
                SpellDeadlyScytheCast           = 258039,
                /// Sargeras' Gaze
                SpellSargerasGazeSearcher       = 258068,
                SpellSargerasRageAT             = 257869,
                SpellSargerasFearAT             = 257931,
                /// Soulrending Scythe
                SpellSoulrendingScythe          = 258838,
                SpellRentSoulMissile            = 258939,
                SpellRentSoulAT                 = 258836,
                /// Sentence of Sargeras
                SpellSentenceOfSargeraSearcher  = 258088
            };

            enum eEvents
            {
                /// Stage One: Storm and Sky
                EventSkyAndSea = 1,
                EventTorturedRage,
                EventConeOfDeath,
                EventSoulblightOrb,
                EventSweepingScythe,
                EventGolgannethsWrath,
                /// Stage Two: The Protector Redeemed
                EventEdgeOfObliteration,
                EventAggramarsAid,
                EventSargerasGaze,
                EventSoulburst,
                EventSoulbomb,
                /// Stage Three: The Arcane Masters
                EventCosmicBeacon,
                EventCosmicRay,
                EventStellarArmory,
                /// Stage Four: The Gift of Life, The Forge of Loss
                EventDeadlyScythe,
                EventInitializationSequence,
                /// Mythic abilities
                EventSoulrendingScythe,
                EventSentenceOfSargeras,
                EventEdgeOfAnnihilation
            };

            enum eStages
            {
                StageOneStormAndSky,
                StageTwoTheProtectorRedeemed,
                StageThreeTheArcaneMasters,
                StageFourTheGiftOfLife
            };

            enum eTalks
            {
                TalkAggro,
                TalkWipe,
                TalkDeath,
                TalkSlay,
                TalkStage3,
                TalkStage4,
                TalkModules,
                TalkSargerasGaze
            };

            enum eActions
            {
                ActionConeOfDeath,
                ActionSkyAndSea,
                ActionStage2,
                ActionGolgannethsWrath,
                ActionSoulburst,
                ActionSoulbomb,
                ActionAggramarsAid,
                ActionStage3,
                ActionDiscsOfNorgannon,
                ActionCosmicBeacon,
                ActionBladesOfTheEternal,
                ActionSwordOfTheCosmos,
                ActionGiftOfTheLifebinder,
                ActionCosmicRay
            };

            enum eVisuals
            {
                VisualKneel         = 3647,
                VisualHover         = 4942,
                VisualClassicScythe = 152429,
                VisualMythicScythe  = 155685
            };

            enum eConversation
            {
                ConversationStage4 = 6425
            };

            bool m_EnergyRegen = false;

            uint8 m_StageID = 0;

            uint8 m_SwitchStagePctIDx = 0;

            std::vector<uint32> m_SwitchHealthPcts;

            uint32 m_SweepingSytheCount = 0;
            uint32 m_SoulblightOrbCount = 0;
            uint32 m_TorturedRageCount = 0;
            uint32 m_ConstellarKilled = 0;
            uint32 m_InitializationCount = 0;
            uint32 m_BerserkerTimer = 0;
            uint32 m_MotesCollected = 0;
            uint32 m_LastSlayTalkTime = 0;
            uint32 m_SargerasGazeCount = 0;
            uint32 m_AnnihilationCount = 0;
            uint32 m_SentenceOfSargerasCount = 0;

            bool m_Transition = false;
            bool m_CheckWipe = false;
            bool m_Achievement = false;
            bool m_AnnihilationSide = false;

            uint64 m_LastAggramarTankGuid = 0;

            std::array<Position const, 7> m_ConstellarPos =
            {
                {
                    { 2905.767f, -4570.622f, 292.0283f, 3.065151f },    ///< Physical
                    { 2890.833f, -4538.693f, 292.0280f, 4.020361f },    ///< Holy
                    { 2856.431f, -4533.665f, 292.0267f, 5.009749f },    ///< Nature
                    { 2828.067f, -4553.245f, 292.0292f, 5.927704f },    ///< Frost
                    { 2888.437f, -4600.903f, 292.0283f, 2.146618f },    ///< Fire
                    { 2852.883f, -4606.910f, 292.0286f, 1.229251f },    ///< Shadow
                    { 2828.718f, -4584.802f, 292.0288f, 0.422982f }     ///< Arcane
                }
            };

            std::array<Position const, eData::DataArgusHungeringSouls> m_HungeringSouls =
            {
                {
                    { 2879.850f, -4572.710f, 291.9490f, 5.524070f },
                    { 2861.390f, -4590.630f, 291.9492f, 5.524075f },
                    { 2839.623f, -4584.421f, 291.9497f, 5.524071f },
                    { 2853.907f, -4551.636f, 291.9494f, 5.524073f },
                    { 2864.882f, -4543.668f, 291.9492f, 5.524075f },
                    { 2844.157f, -4537.746f, 291.9575f, 5.524071f },
                    { 2891.152f, -4533.885f, 291.9480f, 5.524073f },
                    { 2831.202f, -4591.524f, 292.0190f, 5.524070f },
                    { 2872.847f, -4597.142f, 291.9738f, 5.524075f },
                    { 2836.351f, -4559.610f, 291.9678f, 5.524071f },
                    { 2884.374f, -4599.639f, 291.9494f, 5.524071f },
                    { 2882.578f, -4548.632f, 291.9496f, 5.524070f },
                    { 2897.824f, -4601.151f, 292.2978f, 5.524076f },
                    { 2849.517f, -4574.662f, 291.9635f, 5.524073f },
                    { 2872.653f, -4600.941f, 291.9754f, 5.524079f },
                    { 2907.010f, -4576.425f, 291.9488f, 5.524073f },
                    { 2861.772f, -4592.230f, 291.9556f, 5.524075f },
                    { 2887.678f, -4568.551f, 291.9624f, 5.524079f },
                    { 2839.291f, -4568.653f, 291.9490f, 5.524074f },
                    { 2894.667f, -4546.270f, 291.9645f, 5.524072f },
                    { 2839.131f, -4560.833f, 291.9595f, 5.524077f },
                    { 2850.289f, -4541.830f, 291.9662f, 5.524074f },
                    { 2876.066f, -4585.488f, 291.9515f, 5.524073f },
                    { 2909.867f, -4556.029f, 292.1080f, 5.524072f },
                    { 2857.624f, -4575.816f, 291.9581f, 5.524073f }
                }
            };

            std::vector<uint64> m_RentSoulTargets;

            std::array<std::array<Position const, 8>, 2> m_EdgeOfAnnihilationPos =
            {
                {
                    {
                        {
                            { 2842.720f, -4531.281f, 292.3707f, 4.712389f },
                            { 2858.720f, -4525.521f, 292.2545f, 4.712389f },
                            { 2874.720f, -4525.521f, 292.2894f, 4.712389f },
                            { 2890.720f, -4531.281f, 292.2932f, 4.712389f },
                            { 2830.151f, -4591.850f, 292.3728f, 0.000000f },
                            { 2824.391f, -4575.850f, 292.3740f, 0.000000f },
                            { 2824.391f, -4559.850f, 292.2992f, 0.000000f },
                            { 2830.151f, -4543.850f, 292.3408f, 0.000000f }
                        }
                    },
                    {
                        {
                            { 2882.720f, -4608.105f, 292.3059f, 1.570796f },
                            { 2850.720f, -4608.105f, 292.3036f, 1.570796f },
                            { 2898.720f, -4598.627f, 292.3084f, 1.570796f },
                            { 2866.720f, -4610.850f, 292.1141f, 1.570796f },
                            { 2909.720f, -4567.850f, 292.0894f, 3.141593f },
                            { 2906.975f, -4583.850f, 292.1432f, 3.141593f },
                            { 2897.497f, -4535.850f, 292.3694f, 3.141593f },
                            { 2906.975f, -4551.850f, 292.1404f, 3.141593f }
                        }
                    }
                }
            };

            std::set<uint64> m_PlayerIntroTriggered;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanFly() override
            {
                return m_CheckWipe;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void Reset() override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                _Reset();

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_DISARMED);

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS, eVisuals::VisualClassicScythe);

                DoCast(me, eSpells::SpellTitanicEssence, true);

                me->SetPower(Powers::POWER_ENERGY, 0);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                m_EnergyRegen = false;

                m_StageID = eStages::StageOneStormAndSky;

                m_SwitchStagePctIDx = 0;

                m_SwitchHealthPcts = { 70, 40 };

                m_SweepingSytheCount = 0;
                m_SoulblightOrbCount = 0;
                m_TorturedRageCount = 0;
                m_ConstellarKilled = 0;
                m_InitializationCount = 0;
                m_BerserkerTimer = 0;
                m_MotesCollected = 0;
                m_LastSlayTalkTime = 0;
                m_SargerasGazeCount = 0;
                m_AnnihilationCount = 0;
                m_SentenceOfSargerasCount = 0;

                m_Transition = false;
                m_CheckWipe = false;
                m_Achievement = false;
                m_AnnihilationSide = false;

                m_LastAggramarTankGuid = 0;

                m_RentSoulTargets.clear();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                Talk(eTalks::TalkAggro);

                m_BerserkerTimer = (IsMythic() ? 660 : 720) * TimeConstants::IN_MILLISECONDS;

                DefaultEvents();
            }

            void DefaultEvents()
            {
                switch (m_StageID)
                {
                    case eStages::StageOneStormAndSky:
                    {
                        DoCast(me, eSpells::SpellArgusP1EnergyController, true);

                        events.ScheduleEvent(eEvents::EventSkyAndSea, 11 * TimeConstants::IN_MILLISECONDS);

                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventTorturedRage, 12 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventSoulblightOrb, 35 * TimeConstants::IN_MILLISECONDS + 100);
                            events.ScheduleEvent(eEvents::EventSweepingScythe, 5 * TimeConstants::IN_MILLISECONDS + 900);
                            events.ScheduleEvent(eEvents::EventSargerasGaze, 8 * TimeConstants::IN_MILLISECONDS + 200);
                        }
                        else if (IsHeroic())
                        {
                            events.ScheduleEvent(eEvents::EventTorturedRage, 12 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventSoulblightOrb, 35 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventSweepingScythe, 5 * TimeConstants::IN_MILLISECONDS + 800);
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventTorturedRage, 13 * TimeConstants::IN_MILLISECONDS + 500);

                            if (!IsLFR())
                                events.ScheduleEvent(eEvents::EventSoulblightOrb, 36 * TimeConstants::IN_MILLISECONDS);

                            events.ScheduleEvent(eEvents::EventSweepingScythe, 7 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eStages::StageTwoTheProtectorRedeemed:
                    {
                        me->RemoveAura(eSpells::SpellArgusP1EnergyController);

                        DoCast(me, eSpells::SpellArgusP2EnergyController, true);

                        AddTimedDelayedOperation(13 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                        {
                            me->SetPower(Powers::POWER_ENERGY, 70);

                            m_EnergyRegen = true;
                        });

                        events.ScheduleEvent(eEvents::EventSweepingScythe, 17 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventAggramarsAid, 20 * TimeConstants::IN_MILLISECONDS + 800);

                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventEdgeOfObliteration, 25 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventSargerasGaze, 26 * TimeConstants::IN_MILLISECONDS + 300);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventEdgeOfObliteration, 24 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eStages::StageFourTheGiftOfLife:
                    {
                        m_SweepingSytheCount = 0;
                        m_TorturedRageCount = 0;
                        m_InitializationCount = 0;

                        if (IsMythic())
                        {
                            m_SargerasGazeCount = 0;
                            m_AnnihilationCount = 0;
                            m_SentenceOfSargerasCount = 0;

                            events.ScheduleEvent(eEvents::EventTorturedRage, 40 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventInitializationSequence, 30 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventSargerasGaze, 23 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventSoulrendingScythe, 3 * TimeConstants::IN_MILLISECONDS + 800);
                            events.ScheduleEvent(eEvents::EventSentenceOfSargeras, 53 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventEdgeOfAnnihilation, 5 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            m_SoulblightOrbCount = 0;

                            if (IsHeroic())
                                events.ScheduleEvent(eEvents::EventDeadlyScythe, 6 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventSweepingScythe, 5 * TimeConstants::IN_MILLISECONDS + 100);

                            events.ScheduleEvent(eEvents::EventTorturedRage, 11 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventInitializationSequence, 18 * TimeConstants::IN_MILLISECONDS + 500);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_SET_ALLOWING_RELEASE, nullptr, false);

                    if (Creature* l_Khazgoroth = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcKhazgoroth)))
                    {
                        l_Khazgoroth->InterruptNonMeleeSpells(true);
                        l_Khazgoroth->SetPower(Powers::POWER_ENERGY, 0);
                    }

                    bool l_AchievDisabled = sObjectMgr->IsDisabledMap(instance->instance->GetId(), instance->instance->GetDifficultyID());

                    if (!l_AchievDisabled)
                        l_AchievDisabled = sObjectMgr->IsDisabledEncounter(eData::DataArgusEncounterID, instance->instance->GetDifficultyID());

                    if (!l_AchievDisabled && !IsLFR() && m_Achievement && instance)
                        instance->DoCompleteAchievement(eAchievements::StartdustCrusaders);
                }

                _JustDied();

                Talk(eTalks::TalkDeath);

                RemoveEncounterAuras();

                me->ClearLootContainers();
            }

            void EnterEvadeMode() override
            {
                me->SetAnimTier(0);
                me->SendPlayHoverAnim(false);

                SetFlyMode(false);

                me->SetAIAnimKitId(0);

                me->InterruptNonMeleeSpells(true);

                Talk(eTalks::TalkWipe);

                if (instance)
                {
                    instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_SET_ALLOWING_RELEASE, nullptr, false);

                    if (Creature* l_Khazgoroth = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcKhazgoroth)))
                    {
                        l_Khazgoroth->InterruptNonMeleeSpells(true);
                        l_Khazgoroth->SetPower(Powers::POWER_ENERGY, 0);
                    }
                }

                BossAI::EnterEvadeMode();

                RemoveEncounterAuras();
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer() && time(nullptr) >= (m_LastSlayTalkTime + 10))
                {
                    m_LastSlayTalkTime = time(nullptr);

                    Talk(eTalks::TalkSlay);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionConeOfDeath:
                    {
                        if (!m_EnergyRegen || events.HasEvent(eEvents::EventConeOfDeath))
                            break;

                        events.ScheduleEvent(eEvents::EventConeOfDeath, 1);
                        break;
                    }
                    case eActions::ActionSoulburst:
                    {
                        if (IsMythic() && m_StageID == eStages::StageFourTheGiftOfLife)
                            break;

                        if (!m_EnergyRegen || events.HasEvent(eEvents::EventSoulburst))
                            break;

                        events.ScheduleEvent(eEvents::EventSoulburst, 1);
                        break;
                    }
                    case eActions::ActionSoulbomb:
                    {
                        if (IsMythic() && m_StageID == eStages::StageFourTheGiftOfLife)
                            break;

                        if (!m_EnergyRegen || events.HasEvent(eEvents::EventSoulbomb))
                            break;

                        events.ScheduleEvent(eEvents::EventSoulbomb, 1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_SwitchStagePctIDx >= m_SwitchHealthPcts.size())
                    return;

                if (me->HealthBelowPctDamaged(m_SwitchHealthPcts[m_SwitchStagePctIDx], p_Damage))
                {
                    events.Reset();

                    m_EnergyRegen = false;

                    m_SweepingSytheCount = 0;
                    m_SoulblightOrbCount = 0;
                    m_TorturedRageCount = 0;

                    ++m_SwitchStagePctIDx;
                    ++m_StageID;

                    if (!instance)
                        return;

                    switch (m_StageID - 1)
                    {
                        case eStages::StageOneStormAndSky:
                        {
                            Creature* l_Amanthul = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAmanThul));
                            if (!l_Amanthul || !l_Amanthul->IsAIEnabled)
                                return;

                            l_Amanthul->AI()->DoAction(eActions::ActionStage2);

                            events.ScheduleEvent(eEvents::EventGolgannethsWrath, 6 * TimeConstants::IN_MILLISECONDS);
                            break;
                        }
                        case eStages::StageTwoTheProtectorRedeemed:
                        {
                            Talk(eTalks::TalkStage3);

                            m_Transition = true;

                            summons.DespawnEntry(eCreatures::NpcEdgeOfObliteration);

                            me->DespawnAreaTriggersInArea(eSpells::SpellEdgeOfObliterationWarning);

                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            me->StopAttack();

                            me->InterruptNonMeleeSpells(true);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eStages::StageThreeTheArcaneMasters, me->GetHomePosition());
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eStages::StageThreeTheArcaneMasters:
                    {
                        me->SetAIAnimKitId(eVisuals::VisualKneel);

                        DoCast(me, eSpells::SpellBreakTargeting, true);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        AddTimedDelayedOperation(11 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (!instance)
                                return;

                            Creature* l_Amanthul = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAmanThul));
                            if (!l_Amanthul || !l_Amanthul->IsAIEnabled)
                                return;

                            l_Amanthul->AI()->DoAction(eActions::ActionStage3);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellConeOfDeathSearcher:
                    {
                        p_Targets.clear();

                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllRanged))
                            p_Targets.push_back(l_Target);

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
                    case eCreatures::NpcConstellarDesignate:
                    {
                        p_Summon->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);

                        if ((++m_ConstellarKilled) >= uint32(m_ConstellarPos.size()))
                            ReapSoul();

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
                    case eSpells::SpellConeOfDeathSearcher:
                    {
                        Position l_Pos = p_Target->GetPosition();

                        me->SetFacingTo(me->GetAngle(&l_Pos));
                        me->NearTeleportTo(*me);

                        DoCast(l_Pos, eSpells::SpellConeOfDeathCast);

                        AddTimedDelayedOperation(1, [this, l_Pos]() -> void
                        {
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        });

                        break;
                    }
                    case eSpells::SpellSoulrendingScythe:
                    {
                        m_RentSoulTargets.push_back(p_Target->GetGUID());

                        if (Aura* l_Aura = p_Target->GetAura(eSpells::SpellSoulrendingScythe, me->GetGUID()))
                        {
                            for (uint8 l_I = 0; l_I < l_Aura->GetStackAmount(); ++l_I)
                            {
                                Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllRanged, { }, 30.0f, m_RentSoulTargets);
                                if (l_Player == nullptr)
                                    l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll, { }, 0.0f, m_RentSoulTargets);
                                if (l_Player != nullptr)
                                {
                                    m_RentSoulTargets.push_back(l_Player->GetGUID());

                                    p_Target->CastSpell(l_Player, eSpells::SpellRentSoulMissile, true);
                                }
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellTemporalBlast:
                    {
                        if (IsMythic())
                        {
                            DoCast(me, eSpells::SpellArgusMythicTransform, true);

                            me->SetHealth(me->CountPctFromMaxHealth(60));

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->SetAIAnimKitId(0);
                            });

                            AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED);

                                me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS, eVisuals::VisualMythicScythe);
                            });

                            AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                DoCast(me, eSpells::SpellArcaneDissolveOut, true);
                                DoCast(me, eSpells::SpellCancelAvatarOfAggramar, true);

                                ReapSoul();
                            });
                        }
                        else
                        {
                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                DoCast(me, eSpells::SpellArcaneDissolveOut, true);
                                DoCast(me, eSpells::SpellCancelAvatarOfAggramar, true);
                            });

                            AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->SetAnimTier(3);
                                me->SendPlayHoverAnim(true);

                                SetFlyMode(true);

                                m_CheckWipe = true;

                                uint8 l_VulnerabilityID = 0;

                                for (Position const& l_Pos : m_ConstellarPos)
                                {
                                    if (Creature* l_Constellar = me->SummonCreature(eCreatures::NpcConstellarDesignate, l_Pos))
                                    {
                                        if (l_Constellar->IsAIEnabled)
                                            l_Constellar->AI()->SetData(0, l_VulnerabilityID++);
                                    }
                                }

                                if (!instance)
                                    return;

                                Creature* l_Norgannon = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcNorgannon));
                                if (!l_Norgannon || !l_Norgannon->IsAIEnabled)
                                    return;

                                l_Norgannon->AI()->DoAction(eActions::ActionDiscsOfNorgannon);

                                if (IsNormal() || IsLFR())
                                {
                                    events.ScheduleEvent(eEvents::EventCosmicBeacon, 43 * TimeConstants::IN_MILLISECONDS + 400);
                                    events.ScheduleEvent(eEvents::EventCosmicRay, 24 * TimeConstants::IN_MILLISECONDS + 400);
                                    events.ScheduleEvent(eEvents::EventStellarArmory, 13 * TimeConstants::IN_MILLISECONDS + 900);
                                }
                                else
                                {
                                    events.ScheduleEvent(eEvents::EventCosmicBeacon, 31 * TimeConstants::IN_MILLISECONDS + 700);
                                    events.ScheduleEvent(eEvents::EventCosmicRay, 16 * TimeConstants::IN_MILLISECONDS + 800);
                                    events.ScheduleEvent(eEvents::EventStellarArmory, 9 * TimeConstants::IN_MILLISECONDS + 700);
                                }
                            });
                        }

                        break;
                    }
                    case eSpells::SpellGiftOfTheLifebinder:
                    {
                        me->SummonCreature(eCreatures::NpcGiftOfTheLifebinder, { 2893.734f, -4545.361f, 292.0279f, 3.778285f });

                        for (Position const& l_Pos : m_HungeringSouls)
                            me->SummonCreature(eCreatures::NpcHungeringSoul, l_Pos);

                        me->SetAnimTier(0);
                        me->SendPlayHoverAnim(false);

                        SetFlyMode(false);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        me->RemoveAura(eSpells::SpellArcaneDissolveOutSecond);
                        me->RemoveAura(eSpells::SpellArgusP1EnergyController);
                        me->RemoveAura(eSpells::SpellArgusP2EnergyController);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetPower(Powers::POWER_ENERGY, 85);

                            DoCast(me, eSpells::SpellEndOfAllThings);
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
                    case eSpells::SpellConeOfDeathCast:
                    {
                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        });

                        break;
                    }
                    case eSpells::SpellArcaneDissolveOutSecond:
                    {
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED);
                        me->SetAIAnimKitId(0);

                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (!instance)
                                return;

                            Creature* l_Eonar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcEonar));
                            if (!l_Eonar || !l_Eonar->IsAIEnabled)
                                return;

                            l_Eonar->AI()->DoAction(eActions::ActionGiftOfTheLifebinder);
                        });

                        break;
                    }
                    case eSpells::SpellEndOfAllThings:
                    {
                        m_CheckWipe = true;
                        break;
                    }
                    case eSpells::SpellVolatileSoul:
                    {
                        me->SetPower(Powers::POWER_ENERGY, 0);

                        TriggerSoulburst(true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnInterruptCast(Unit* /*p_Caster*/, SpellInfo const* /*p_SpellInfo*/, SpellInfo const* p_CurrSpellInfo, uint32 /*p_SchoolMask*/) override
            {
                switch (p_CurrSpellInfo->Id)
                {
                    case eSpells::SpellEndOfAllThings:
                    {
                        m_CheckWipe = true;
                        m_Transition = false;

                        ++m_StageID;

                        DefaultEvents();

                        DoCast(me, eSpells::SpellArgusP4EnergyController, true);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            m_EnergyRegen = true;

                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            if (!IsMythic())
                                DoCast(me, eSpells::SpellEmberOfRagePeriodic, true);

                            if (Unit* l_Victim = me->getVictim())
                            {
                                AttackStart(l_Victim);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Victim);
                            }
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                switch (p_AreaTrigger->GetSpellId())
                {
                    case eSpells::SpellConeOfDeathAT:
                    {
                        p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, 538.5165f);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Transition && !UpdateVictim())
                    return;

                if (m_BerserkerTimer)
                {
                    if (m_BerserkerTimer <= p_Diff)
                    {
                        m_BerserkerTimer = 0;

                        DoCast(me, eSharedSpells::SpellBerserk, true);
                    }
                    else
                        m_BerserkerTimer -= p_Diff;
                }

                /// Check wipe conditions manually during transition phases
                /// Argus will kill the whole raid group at stage 4 start, don't make him reset!
                if (m_CheckWipe && instance->IsWipe())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSkyAndSea:
                    {
                        m_EnergyRegen = true;

                        if (!instance)
                            break;

                        Creature* l_Amanthul = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAmanThul));
                        if (!l_Amanthul || !l_Amanthul->IsAIEnabled)
                            break;

                        l_Amanthul->AI()->DoAction(eActions::ActionSkyAndSea);

                        if (IsNormal() || IsLFR())
                            events.ScheduleEvent(eEvents::EventSkyAndSea, 25 * TimeConstants::IN_MILLISECONDS + 300);
                        else
                            events.ScheduleEvent(eEvents::EventSkyAndSea, 22 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventTorturedRage:
                    {
                        ++m_TorturedRageCount;

                        DoCast(me, eSpells::SpellTorturedRageCast);

                        std::map<uint32, std::vector<uint32>> l_Timers =
                        {
                            {
                                Difficulty::DifficultyRaidHeroic,
                                { 12000, 13500, 13500, 15900, 13500, 13500, 15900, 20900, 13500, 13300, 13300 }
                            },
                            {
                                Difficulty::DifficultyRaidMythic,
                                { 40000, 40000, 50000, 30000, 35000, 10000, 8000, 35000, 10000, 8000, 35000 }
                            }
                        };

                        auto const& l_Iter = l_Timers.find(me->GetMap()->GetDifficultyID());

                        if (IsMythic())
                        {
                            if (m_StageID == eStages::StageFourTheGiftOfLife && l_Iter != l_Timers.end() && m_TorturedRageCount < l_Iter->second.size())
                                events.ScheduleEvent(eEvents::EventTorturedRage, l_Iter->second[m_TorturedRageCount]);
                            else
                                events.ScheduleEvent(eEvents::EventTorturedRage, 13 * TimeConstants::IN_MILLISECONDS + 500);
                        }
                        else if (m_StageID == eStages::StageFourTheGiftOfLife)
                            events.ScheduleEvent(eEvents::EventTorturedRage, 13 * TimeConstants::IN_MILLISECONDS + 500);
                        else if (IsNormal() || IsLFR())
                            events.ScheduleEvent(eEvents::EventTorturedRage, 15 * TimeConstants::IN_MILLISECONDS + 800);
                        else
                        {
                            if (l_Iter != l_Timers.end() && m_TorturedRageCount < l_Iter->second.size())
                                events.ScheduleEvent(eEvents::EventTorturedRage, l_Iter->second[m_TorturedRageCount]);
                        }

                        break;
                    }
                    case eEvents::EventConeOfDeath:
                    {
                        me->SetPower(Powers::POWER_ENERGY, 0);

                        DoCast(me, eSpells::SpellConeOfDeathSearcher, true);
                        break;
                    }
                    case eEvents::EventSoulblightOrb:
                    {
                        ++m_SoulblightOrbCount;

                        DoCast(me, eSpells::SpellSoulblightOrbCast);

                        std::map<uint32, std::vector<uint32>> l_Timers =
                        {
                            {
                                Difficulty::DifficultyRaidLFR,
                                { 36000, 33000, 28000, 26500, 26600 }
                            },
                            {
                                Difficulty::DifficultyRaidNormal,
                                { 36000, 33000, 28000, 26500, 26600 }
                            },
                            {
                                Difficulty::DifficultyRaidHeroic,
                                { 35500, 25500, 26800, 23200, 31000 }
                            },
                            {
                                Difficulty::DifficultyRaidMythic,
                                { 35100, 24400, 25600, 26400, 24000, 24400 }
                            }
                        };

                        auto const& l_Iter = l_Timers.find(me->GetMap()->GetDifficultyID());

                        if (l_Iter != l_Timers.end() && m_SoulblightOrbCount < l_Iter->second.size())
                            events.ScheduleEvent(eEvents::EventSoulblightOrb, l_Iter->second[m_SoulblightOrbCount]);

                        break;
                    }
                    case eEvents::EventSweepingScythe:
                    {
                        ++m_SweepingSytheCount;

                        DoCastVictim(eSpells::SpellSweepingScythe);

                        if (m_StageID == eStages::StageOneStormAndSky && !IsMythic())
                        {
                            std::map<uint32, std::vector<uint32>> l_Timers =
                            {
                                {
                                    Difficulty::DifficultyRaidLFR,
                                    { 7000, 9500, 7300, 8400, 9000, 6800, 7300, 9500, 7500, 7300, 13100, 7500, 8400, 7300, 11500, 6500, 10900 }
                                },
                                {
                                    Difficulty::DifficultyRaidNormal,
                                    { 7000, 9500, 7300, 8400, 9000, 6800, 7300, 9500, 7500, 7300, 13100, 7500, 8400, 7300, 11500, 6500, 10900 }
                                },
                                {
                                    Difficulty::DifficultyRaidHeroic,
                                    { 5800, 11700, 6600, 10300, 10000, 5600, 10300, 5900, 11500, 10100, 5600, 10300, 5600, 15200 }
                                }
                            };

                            auto const& l_Iter = l_Timers.find(me->GetMap()->GetDifficultyID());

                            if (l_Iter != l_Timers.end() && m_SweepingSytheCount < l_Iter->second.size())
                                events.ScheduleEvent(eEvents::EventSweepingScythe, l_Iter->second[m_SweepingSytheCount]);
                            else
                                events.ScheduleEvent(eEvents::EventSweepingScythe, 5 * TimeConstants::IN_MILLISECONDS + 500);
                        }
                        else if (m_StageID == eStages::StageFourTheGiftOfLife)
                        {
                            if (m_SweepingSytheCount == 2)
                                events.ScheduleEvent(eEvents::EventSweepingScythe, 8 * TimeConstants::IN_MILLISECONDS + 300);
                            else if (!(m_SweepingSytheCount % 2))
                                events.ScheduleEvent(eEvents::EventSweepingScythe, 7 * TimeConstants::IN_MILLISECONDS + 100);
                            else
                                events.ScheduleEvent(eEvents::EventSweepingScythe, 6 * TimeConstants::IN_MILLISECONDS + 100);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventSweepingScythe, 6 * TimeConstants::IN_MILLISECONDS + 100);

                        break;
                    }
                    case eEvents::EventGolgannethsWrath:
                    {
                        if (!instance)
                            break;

                        Creature* l_Amanthul = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAmanThul));
                        if (!l_Amanthul || !l_Amanthul->IsAIEnabled)
                            break;

                        l_Amanthul->AI()->DoAction(eActions::ActionGolgannethsWrath);

                        DefaultEvents();
                        break;
                    }
                    case eEvents::EventEdgeOfObliteration:
                    {
                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 45.0f, frand(0.0f, 2.0f * M_PI), true);

                        DoCast(l_Pos, eSpells::SpellEdgeOfObliterationCast);

                        events.ScheduleEvent(eEvents::EventEdgeOfObliteration, 34 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventAggramarsAid:
                    {
                        if (!instance)
                            break;

                        Creature* l_Amanthul = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAmanThul));
                        if (!l_Amanthul || !l_Amanthul->IsAIEnabled)
                            break;

                        l_Amanthul->AI()->DoAction(eActions::ActionAggramarsAid);

                        events.ScheduleEvent(eEvents::EventAggramarsAid, 60 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSargerasGaze:
                    {
                        ++m_SargerasGazeCount;

                        DoCast(me, eSpells::SpellSargerasGazeSearcher, true);

                        if (instance)
                        {
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSargerasFearAT);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSargerasRageAT);
                        }

                        Talk(eTalks::TalkSargerasGaze);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            SargerasGaze const* l_Data = nullptr;

                            if (m_SargerasGazeCount < g_SargerasGazePattern.size())
                                l_Data = &g_SargerasGazePattern[m_SargerasGazeCount];
                            else
                                l_Data = &g_SargerasGazePattern[uint8(g_SargerasGazePattern.size() - 1)];

                            if (!l_Data)
                                return;

                            for (uint8 l_FearIDx = 0; l_FearIDx < l_Data->FearCount; ++l_FearIDx)
                            {
                                if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank, { -int32(eSpells::SpellSargerasFearAT), -int32(eSpells::SpellSargerasRageAT) }))
                                    l_Target->CastSpell(l_Target, eSpells::SpellSargerasFearAT, true);
                            }

                            for (uint8 l_MeleeIDx = 0; l_MeleeIDx < l_Data->MeleeRages; ++l_MeleeIDx)
                            {
                                if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskMelee, { -int32(eSpells::SpellSargerasFearAT), -int32(eSpells::SpellSargerasRageAT) }))
                                    l_Target->CastSpell(l_Target, eSpells::SpellSargerasRageAT, true);
                            }

                            if (l_Data->RangedRages)
                            {
                                for (uint8 l_RangedIDx = 0; l_RangedIDx < l_Data->RangedRages; ++l_RangedIDx)
                                {
                                    if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskRanged, { -int32(eSpells::SpellSargerasFearAT), -int32(eSpells::SpellSargerasRageAT) }))
                                        l_Target->CastSpell(l_Target, eSpells::SpellSargerasRageAT, true);
                                }
                            }
                        });

                        if (m_StageID == eStages::StageFourTheGiftOfLife)
                        {
                            std::vector<uint32> l_Timers =
                            {
                                { 23000, 75000, 70000, 53000, 53000 }
                            };

                            if (m_SargerasGazeCount < l_Timers.size())
                                events.ScheduleEvent(eEvents::EventSargerasGaze, l_Timers[m_SargerasGazeCount]);
                            else
                                events.ScheduleEvent(eEvents::EventSargerasGaze, l_Timers[uint8(l_Timers.size() - 1)]);
                        }
                        else if (m_StageID == eStages::StageTwoTheProtectorRedeemed)
                            events.ScheduleEvent(eEvents::EventSargerasGaze, 60 * TimeConstants::IN_MILLISECONDS + 500);
                        else
                            events.ScheduleEvent(eEvents::EventSargerasGaze, 35 * TimeConstants::IN_MILLISECONDS + 100);

                        break;
                    }
                    case eEvents::EventSoulburst:
                    {
                        /// Each time Argus reaches 50% energy, he will place out two Soulburst debuffs
                        TriggerSoulburst();
                        break;
                    }
                    case eEvents::EventSoulbomb:
                    {
                        /// Each time Argus reaches 100% energy, he will place out Soulburst debuffs onto two different players and Soulbomb onto one player
                        DoCast(me, eSpells::SpellVolatileSoul);
                        break;
                    }
                    case eEvents::EventCosmicBeacon:
                    {
                        std::list<Creature*> l_Concellars;

                        me->GetCreatureListWithEntryInGrid(l_Concellars, eCreatures::NpcConstellarDesignate, 150.0f);

                        if (!l_Concellars.empty())
                        {
                            l_Concellars.remove_if([this](Creature* p_Creature) -> bool
                            {
                                if (!p_Creature || !p_Creature->IsAIEnabled)
                                    return true;

                                /// Has weapon
                                if (p_Creature->AI()->GetData(0))
                                    return true;

                                return false;
                            });
                        }

                        if (l_Concellars.size() > 2)
                            JadeCore::Containers::RandomResizeList(l_Concellars, 2);

                        for (Creature* l_Creature : l_Concellars)
                        {
                            if (l_Creature->IsAIEnabled)
                                l_Creature->AI()->DoAction(eActions::ActionCosmicBeacon);
                        }

                        if (IsNormal() || IsLFR())
                            events.ScheduleEvent(eEvents::EventCosmicBeacon, 30 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventCosmicBeacon, 20 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventCosmicRay:
                    {
                        std::list<Creature*> l_Concellars;

                        me->GetCreatureListWithEntryInGrid(l_Concellars, eCreatures::NpcConstellarDesignate, 150.0f);

                        if (!l_Concellars.empty())
                        {
                            l_Concellars.remove_if([this](Creature* p_Creature) -> bool
                            {
                                if (!p_Creature || !p_Creature->IsAIEnabled)
                                    return true;

                                /// Has weapon
                                if (p_Creature->AI()->GetData(0))
                                    return true;

                                return false;
                            });
                        }

                        if (l_Concellars.size() > 2)
                            JadeCore::Containers::RandomResizeList(l_Concellars, 2);

                        for (Creature* l_Creature : l_Concellars)
                        {
                            if (l_Creature->IsAIEnabled)
                                l_Creature->AI()->DoAction(eActions::ActionCosmicRay);
                        }

                        if (IsNormal() || IsLFR())
                            events.ScheduleEvent(eEvents::EventCosmicRay, 30 * TimeConstants::IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventCosmicRay, 20 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventStellarArmory:
                    {
                        std::list<Creature*> l_Concellars;

                        me->GetCreatureListWithEntryInGrid(l_Concellars, eCreatures::NpcConstellarDesignate, 150.0f);

                        if (!l_Concellars.empty())
                        {
                            l_Concellars.remove_if([this](Creature* p_Creature) -> bool
                            {
                                if (!p_Creature || !p_Creature->IsAIEnabled)
                                    return true;

                                /// Has weapon
                                if (p_Creature->AI()->GetData(0))
                                    return true;

                                return false;
                            });
                        }

                        if (l_Concellars.size() > 2)
                            JadeCore::Containers::RandomResizeList(l_Concellars, 2);

                        bool l_Blades = true;
                        for (Creature* l_Creature : l_Concellars)
                        {
                            if (l_Creature->IsAIEnabled)
                                l_Creature->AI()->DoAction(l_Blades ? eActions::ActionBladesOfTheEternal : eActions::ActionSwordOfTheCosmos);

                            l_Blades = !l_Blades;
                        }

                        if (IsNormal() || IsLFR())
                            break;
                        else
                            events.ScheduleEvent(eEvents::EventStellarArmory, 40 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventDeadlyScythe:
                    {
                        DoCastVictim(eSpells::SpellDeadlyScytheCast);

                        events.ScheduleEvent(eEvents::EventDeadlyScythe, 6 * TimeConstants::IN_MILLISECONDS + 600);
                        break;
                    }
                    case eEvents::EventInitializationSequence:
                    {
                        if (IsMythic())
                        {
                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank, { -int32(eSpells::SpellSargerasRageAT) }))
                                {
                                    Position l_Pos = l_Target->GetPosition();

                                    l_Pos.m_positionZ += 1.5f;

                                    me->SummonCreature(eCreatures::NpcApocalypsisModule, l_Pos);
                                }
                            });
                        }
                        else
                        {
                            float l_Angle = frand(0.0f, 2.0f * M_PI);

                            for (uint8 l_I = 0; l_I < (3 + m_InitializationCount); ++l_I)
                            {
                                Position l_Pos;

                                g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 40.0f, l_Angle, true);

                                l_Pos.m_positionZ += 1.5f;

                                me->SummonCreature(eCreatures::NpcReoriginationModule, l_Pos);

                                l_Angle = Position::NormalizeOrientation(l_Angle + (2.0f * M_PI / (3.0f + m_InitializationCount)));
                            }
                        }

                        ++m_InitializationCount;

                        Talk(eTalks::TalkModules);

                        if (IsMythic())
                        {
                            std::vector<uint32> l_Timers =
                            {
                                { 30000, 47500, 46000, 45500, 52500, 52500 }
                            };

                            if (m_InitializationCount < l_Timers.size())
                                events.ScheduleEvent(eEvents::EventInitializationSequence, l_Timers[m_InitializationCount]);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventInitializationSequence, 50 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventSoulrendingScythe:
                    {
                        m_RentSoulTargets.clear();

                        DoCastVictim(eSpells::SpellSoulrendingScythe);

                        events.ScheduleEvent(eEvents::EventSoulrendingScythe, 8 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventSentenceOfSargeras:
                    {
                        ++m_SentenceOfSargerasCount;

                        DoCast(me, eSpells::SpellSentenceOfSargeraSearcher, true);

                        if (m_StageID == eStages::StageFourTheGiftOfLife)
                        {
                            std::vector<uint32> l_Timers =
                            {
                                { 53000, 57000, 60000, 53000, 53000 }
                            };

                            if (m_SentenceOfSargerasCount < l_Timers.size())
                                events.ScheduleEvent(eEvents::EventSentenceOfSargeras, l_Timers[m_SentenceOfSargerasCount]);
                        }

                        break;
                    }
                    case eEvents::EventEdgeOfAnnihilation:
                    {
                        ++m_AnnihilationCount;

                        for (Position const& l_Pos : m_EdgeOfAnnihilationPos[m_AnnihilationSide])
                        {
                            Position l_RealPos  = l_Pos;
                            float l_Angle       = g_CenterPos.GetAngle(&l_Pos);

                            g_CenterPos.SimplePosXYRelocationByAngle(l_RealPos, 45.0f, l_Angle, true);

                            l_RealPos.m_orientation = l_RealPos.GetAngle(&g_CenterPos);

                            me->SummonCreature(eCreatures::NpcEdgeOfAnnihilation, l_RealPos);
                        }

                        m_AnnihilationSide = !m_AnnihilationSide;

                        if (m_StageID == eStages::StageFourTheGiftOfLife)
                        {
                            std::vector<uint32> l_Timers =
                            {
                                { 5000, 5000, 90000, 5000, 45000, 5000 }
                            };

                            if (m_AnnihilationCount < l_Timers.size())
                                events.ScheduleEvent(eEvents::EventEdgeOfAnnihilation, l_Timers[m_AnnihilationCount]);
                        }

                        break;
                    }
                    default:
                        break;
                }

                if (!m_Transition)
                    DoMeleeAttackIfReady();
            }

            void TriggerSoulburst(bool p_Soulbomb = false)
            {
                std::vector<uint64> l_Targets;

                for (uint8 l_I = 0; l_I < 2; ++l_I)
                {
                    if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllDPS, { }, 0.0f, l_Targets))
                        l_Targets.push_back(l_Player->GetGUID());
                }

                for (uint64 const& l_Guid : l_Targets)
                {
                    if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                        DoCast(l_Player, eSpells::SpellSoulburstAura, true);
                }

                if (p_Soulbomb)
                {
                    Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskHealer, { }, 0.0f, l_Targets);
                    if (l_Player == nullptr)
                        l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank, { }, 0.0f, l_Targets);
                    if (l_Player != nullptr)
                        DoCast(l_Player, eSpells::SpellSoulbombAura, true);
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                return m_EnergyRegen;
            }

            uint64 GetData64(uint32 /*p_ID*/) override
            {
                return m_LastAggramarTankGuid;
            }

            void SetData(uint32 p_ID, uint32 /*p_Value*/) override
            {
                if (!IsLFR() && (++m_MotesCollected) >= eData::DataArgusMotesToCollect)
                    m_Achievement = true;
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                m_LastAggramarTankGuid = p_Guid;
            }

            void RemoveEncounterAuras()
            {
                me->DespawnAreaTriggersInArea
                ({
                    eSpells::SpellDeathFogAT,
                    eSpells::SpellGiftOfTheSeaAT,
                    eSpells::SpellGiftOfTheSkyAT,
                    eSpells::SpellRentSoulAT
                }, 100.0f, false);

                me->DespawnCreaturesInArea(eCreatures::NpcChainsOfSargeras, 100.0f);

                if (!instance)
                    return;

                if (Creature* l_Khazgoroth = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcKhazgoroth)))
                    instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, l_Khazgoroth);

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSweepingScythe);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSoulblightDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellGiftOfTheSea);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellGiftOfTheSky);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellStrengthOfTheSea);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellStrengthOfTheSky);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSoulburstAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSoulbombAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEdgeOfObliterationDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAvatarOfAggramar);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCosmicRayAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellCosmicBeaconCast);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellGraspOfTheUnmaker);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDeadlyScytheCast);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmberOfRageDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSargerasRageAT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSargerasFearAT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSoulrendingScythe);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellShatteredBonds);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEdgeOfAnnihilationDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTitanforgedBuff);
            }

            void ReapSoul()
            {
                me->SetAIAnimKitId(eVisuals::VisualHover);

                me->RemoveAura(eSpells::SpellArcaneDissolveOut);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED);

                uint32 l_Time = 3 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    DoCast(me, eSpells::SpellReapSoulBlue);
                });

                l_Time += 4 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    std::list<Player*> l_Players;

                    me->GetPlayerListInGrid(l_Players, 200.0f, false, false);

                    for (Player* l_Iter : l_Players)
                        l_Iter->ResurrectPlayer(100.0f);
                });

                l_Time += 300;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    Talk(eTalks::TalkStage4);

                    DoCast(me, eSpells::SpellGraspOfTheUnmaker, true);
                });

                l_Time += 5 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    Position l_MovePos = { 2846.756f, -4567.445f, 293.0727f, 6.26289f };

                    me->GetMotionMaster()->MovePoint(eSpells::SpellReapSoulBlue, l_MovePos);
                });

                l_Time += 5 * TimeConstants::IN_MILLISECONDS + 900;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    m_CheckWipe = false;

                    DoCast(me, eSpells::SpellReapSoulInstakill, true);

                    if (!instance)
                        return;

                    if (Creature* l_Khazgoroth = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcKhazgoroth)))
                        instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, l_Khazgoroth, 2);
                });

                l_Time += 1 * TimeConstants::IN_MILLISECONDS;
                AddTimedDelayedOperation(l_Time, [this]() -> void
                {
                    DoCast(me, eSpells::SpellArcaneDissolveOutSecond);

                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversation::ConversationStage4, me, nullptr, *me))
                        delete l_Conversation;
                });
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (m_PlayerIntroTriggered.find(p_Who->GetGUID()) != m_PlayerIntroTriggered.end())
                    return;

                m_PlayerIntroTriggered.insert(p_Who->GetGUID());

                p_Who->CastSpell(p_Who, eSpells::SpellArgusSummoningMovie, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_argus_the_unmakerAI(p_Creature);
        }
};

/// Aman'thul - 125885
class npc_argus_amanthul : public CreatureScript
{
    public:
        npc_argus_amanthul() : CreatureScript("npc_argus_amanthul") { }

        enum eSpells
        {
            SpellTemporalBlast = 257645
        };

        enum eActions
        {
            ActionSkyAndSea = 1,
            ActionStage2,
            ActionGolgannethsWrath,
            ActionAggramarsAid = 6,
            ActionStage3
        };

        enum eTalks
        {
            TalkSkyAndSea,
            TalkStage2,
            TalkAggramarsAid,
            TalkStage3
        };

        struct npc_argus_amanthulAI : public Scripted_NoMovementAI
        {
            npc_argus_amanthulAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance = nullptr;
            bool m_SkySeaTalk;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IsPassived() override
            {
                return true;
            }

            void Reset() override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                m_SkySeaTalk = false;

                me->SetPower(me->getPowerType(), 0);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionSkyAndSea:
                    {
                        if (!m_Instance)
                            break;

                        Creature* l_Golganneth = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcGolganneth));
                        if (!l_Golganneth || !l_Golganneth->IsAIEnabled)
                            break;

                        l_Golganneth->AI()->DoAction(eActions::ActionSkyAndSea);

                        if (!m_SkySeaTalk)
                        {
                            m_SkySeaTalk = true;
                            Talk(eTalks::TalkSkyAndSea);
                        }
                        break;
                    }
                    case eActions::ActionStage2:
                    {
                        Talk(eTalks::TalkStage2);
                        break;
                    }
                    case eActions::ActionGolgannethsWrath:
                    {
                        if (!m_Instance)
                            break;

                        Creature* l_Golganneth = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcGolganneth));
                        if (!l_Golganneth || !l_Golganneth->IsAIEnabled)
                            break;

                        l_Golganneth->AI()->DoAction(eActions::ActionGolgannethsWrath);
                        break;
                    }
                    case eActions::ActionAggramarsAid:
                    {
                        if (!m_Instance)
                            break;

                        Creature* l_Aggramar = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::NpcAggramar));
                        if (!l_Aggramar || !l_Aggramar->IsAIEnabled)
                            break;

                        l_Aggramar->AI()->DoAction(eActions::ActionAggramarsAid);

                        Talk(eTalks::TalkAggramarsAid);
                        break;
                    }
                    case eActions::ActionStage3:
                    {
                        Talk(eTalks::TalkStage3);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (!m_Instance)
                                return;

                            if (Creature* l_Argus = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::BossArgusTheUnmaker)))
                                DoCast(l_Argus, eSpells::SpellTemporalBlast);
                        });

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
            return new npc_argus_amanthulAI(p_Creature);
        }
};

/// Golganneth - 126268
class npc_argus_golganneth : public CreatureScript
{
    public:
        npc_argus_golganneth() : CreatureScript("npc_argus_golganneth") { }

        enum eSpells
        {
            /// Misc
            SpellConeOfDeathAT          = 248166,
            SpellDeathFogAT             = 248290,
            SpellSoulblightDebuff       = 248396,
            SpellSoulblightOrbAT        = 248317,
            /// Sky and Sea
            SpellSkyAndSea              = 255594,
            SpellGiftOfTheSkyAura       = 258646,
            SpellGiftOfTheSeaAura       = 258647,
            /// Golganneth's Wrath
            SpellGolgannethsWrathDummy  = 256674,
            SpellGolgannethsWrathCast   = 255648,
            SpellGolgannethsWrathAura   = 255683,
            SpellGolgannethsWrathDamage = 255646
        };

        enum eActions
        {
            ActionSkyAndSea = 1,
            ActionGolgannethsWrath = 3
        };

        enum eTalks
        {
            TalkSkyAndSea
        };

        enum eCreatures
        {
            SoulblightNpc = 248317
        };

        struct npc_argus_golgannethAI : public Scripted_NoMovementAI
        {
            npc_argus_golgannethAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool IsPassived() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetPower(me->getPowerType(), 0);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionSkyAndSea:
                    {
                        Talk(eTalks::TalkSkyAndSea);

                        DoCast(me, eSpells::SpellSkyAndSea);
                        break;
                    }
                    case eActions::ActionGolgannethsWrath:
                    {
                        Position l_Pos = { 2866.72f, -4567.85f, 291.949f, 2.792527f };

                        DoCast(l_Pos, eSpells::SpellGolgannethsWrathDummy, true);
                        DoCast(l_Pos, eSpells::SpellGolgannethsWrathCast);
                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellSkyAndSea:
                    {
                        std::list<WorldObject*> l_Targets = p_Targets;

                        l_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                                return true;

                            if (p_Object->ToPlayer()->HasAura(eSpells::SpellSoulblightDebuff))
                                return true;

                            return false;
                        });

                        if (l_Targets.size() > 2)
                            JadeCore::Containers::RandomResizeList(l_Targets, 2);

                        bool l_First = true;

                        for (WorldObject* l_Target : l_Targets)
                        {
                            if (Unit* l_Unit = l_Target->ToUnit())
                            {
                                if (l_First)
                                    l_Unit->CastSpell(l_Unit, eSpells::SpellGiftOfTheSkyAura, true);
                                else
                                    l_Unit->CastSpell(l_Unit, eSpells::SpellGiftOfTheSeaAura, true);

                                l_First = !l_First;
                            }
                        }

                        break;
                    }
                    case eSpells::SpellGolgannethsWrathDamage:
                    {
                        if (DynamicObject* l_Object = me->GetDynObject(eSpells::SpellGolgannethsWrathCast))
                        {
                            p_Targets.remove_if([this, l_Object](WorldObject* p_Object) -> bool
                            {
                                return !p_Object->IsPlayer() || p_Object->GetDistance(l_Object) < 18.0f;
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellGolgannethsWrathCast)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        DoCast(me, eSpells::SpellGolgannethsWrathAura, true);
                    });

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->DespawnAreaTriggersInArea({ eSpells::SpellConeOfDeathAT, eSpells::SpellDeathFogAT }, 130.0f, false);
                        me->DespawnCreaturesInArea(eCreatures::SoulblightNpc, 130.0f);

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSoulblightDebuff);
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
            return new npc_argus_golgannethAI(p_Creature);
        }
};

/// Aggramar - 125893
class npc_argus_aggramar : public CreatureScript
{
    public:
        npc_argus_aggramar() : CreatureScript("npc_argus_aggramar") { }

        enum eSpells
        {
            SpellAvatarOfAggramar = 255199
        };

        enum eActions
        {
            ActionAggramarsAid = 6
        };

        struct npc_argus_aggramarAI : public Scripted_NoMovementAI
        {
            npc_argus_aggramarAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance = nullptr;

            bool IsPassived() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetPower(me->getPowerType(), 0);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionAggramarsAid:
                    {
                        if (!m_Instance)
                            break;

                        if (Creature* l_Argus = Creature::GetCreature(*me, m_Instance->GetData64(eCreatures::BossArgusTheUnmaker)))
                        {
                            if (l_Argus->IsAIEnabled)
                            {
                                if (Player* l_Target = l_Argus->AI()->SelectPlayerTarget(eTargetTypeMask::TypeMaskTank, { }, 0.0f, { l_Argus->AI()->GetData64(0) }))
                                {
                                    l_Target->CastSpell(l_Target, eSpells::SpellAvatarOfAggramar, true);

                                    l_Argus->AI()->SetGUID(l_Target->GetGUID());
                                }
                            }
                        }

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
            return new npc_argus_aggramarAI(p_Creature);
        }
};

/// Norgannon - 126266
class npc_argus_norgannon : public CreatureScript
{
    public:
        npc_argus_norgannon() : CreatureScript("npc_argus_norgannon") { }

        enum eSpell
        {
            SpellTheDiscsOfNorgannon    = 252516
        };

        enum eAction
        {
            ActionDiscsOfNorgannon = 8
        };

        struct npc_argus_norgannonAI : public Scripted_NoMovementAI
        {
            npc_argus_norgannonAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance = nullptr;

            bool IsPassived() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetPower(me->getPowerType(), 0);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eAction::ActionDiscsOfNorgannon:
                    {
                        DoCast(me, eSpell::SpellTheDiscsOfNorgannon);
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
            return new npc_argus_norgannonAI(p_Creature);
        }
};

/// Eonar - 126267
class npc_argus_eonar : public CreatureScript
{
    public:
        npc_argus_eonar() : CreatureScript("npc_argus_eonar") { }

        enum eSpell
        {
            SpellGiftOfTheLifebinder = 257619
        };

        enum eAction
        {
            ActionGiftOfTheLifebinder = 12
        };

        struct npc_argus_eonarAI : public Scripted_NoMovementAI
        {
            npc_argus_eonarAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance = nullptr;

            bool IsPassived() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetPower(me->getPowerType(), 0);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eAction::ActionGiftOfTheLifebinder:
                    {
                        DoCast(me, eSpell::SpellGiftOfTheLifebinder);
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
            return new npc_argus_eonarAI(p_Creature);
        }
};

/// Khaz'goroth - 125886
class npc_argus_khaz_goroth : public CreatureScript
{
    public:
        npc_argus_khaz_goroth() : CreatureScript("npc_argus_khaz_goroth") { }

        enum eSpells
        {
            SpellMoteOfTitanicPowerEnergy   = 253580,

            SpellTitanforgingChannel        = 257214,
            SpellTitanforgedBuff            = 257215
        };

        struct npc_argus_khaz_gorothAI : public Scripted_NoMovementAI
        {
            npc_argus_khaz_gorothAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance = nullptr;

            bool IsPassived() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->SetPower(me->getPowerType(), 0);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellMoteOfTitanicPowerEnergy)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        if (me->GetPower(Powers::POWER_ENERGY) >= 20 && !me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                        {
                            DoCast(me, eSpells::SpellTitanforgingChannel);
                            DoCast(me, eSpells::SpellTitanforgedBuff, true);
                        }
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
            return new npc_argus_khaz_gorothAI(p_Creature);
        }
};

/// Soulblight Orb - 125008
class npc_argus_soulblight_orb : public CreatureScript
{
    public:
        npc_argus_soulblight_orb() : CreatureScript("npc_argus_soulblight_orb") { }

        enum eSpells
        {
            SpellBlightOrbAura      = 248376,
            SpellSoulblightDebuff   = 248396,

            SpellGiftOfTheSkyBuff   = 258646,
            SpellGiftOfTheSeaBuff   = 258647
        };

        struct npc_argus_soulblight_orbAI : public Scripted_NoMovementAI
        {
            npc_argus_soulblight_orbAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellBlightOrbAura, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellSoulblightDebuff:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            Player* l_Player = p_Object ? p_Object->ToPlayer() : nullptr;
                            if (!l_Player || l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                                return true;

                            if (l_Player->HasAura(eSpells::SpellGiftOfTheSkyBuff) || l_Player->HasAura(eSpells::SpellGiftOfTheSeaBuff))
                                return true;

                            return false;
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target && p_SpellInfo->Id == eSpells::SpellSoulblightDebuff)
                    sCreatureTextMgr->SendChat(me, 0, p_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                me->DespawnOrUnsummon(10 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_soulblight_orbAI(p_Creature);
        }
};

/// Edge of Obliteration - 126828
class npc_argus_edge_of_obliteration : public CreatureScript
{
    public:
        npc_argus_edge_of_obliteration() : CreatureScript("npc_argus_edge_of_obliteration") { }

        enum eSpells
        {
            SpellEdgeOfObliterationVisual   = 251779,
            SpellEdgeOfObliterationWarning  = 252136,
            SpellEdgeOfObliterationSearcher = 252049,
            SpellEdgeOfObliterationJump     = 252152,
            SpellEdgeOfObliterationDamage   = 251815
        };

        enum eRestrict
        {
            TargetRestrict = 39529
        };

        struct npc_argus_edge_of_obliterationAI : public ScriptedAI
        {
            npc_argus_edge_of_obliterationAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            Position m_TargetedPos;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IsPassived() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                DoCast(me, eSpells::SpellEdgeOfObliterationVisual, true);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellEdgeOfObliterationSearcher, true);
                });
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                AddTimedDelayedOperation(p_AreaTrigger->GetDuration(), [this]() -> void
                {
                    /// Relocate center of circle to 0 to avoid useless calculation
                    float l_MeRelativeX     = me->m_positionX - g_CenterPos.m_positionX;
                    float l_MeRelativeY     = me->m_positionY - g_CenterPos.m_positionY;;
                    float l_TargetRelativeX = m_TargetedPos.m_positionX - g_CenterPos.m_positionX;
                    float l_TargetRelativeY = m_TargetedPos.m_positionY - g_CenterPos.m_positionY;

                    float l_Radius = 43.0f;

                    /// straight line equation (ax +b) between Scythe and player
                    float l_A = (l_MeRelativeY - l_TargetRelativeY) / (l_MeRelativeX - l_TargetRelativeX);
                    float l_B = -(l_A * l_MeRelativeX) + l_MeRelativeY;

                    /// Circle equation ((x-c)^2 + (y-d)^2 = radius^2) (center 0, y replaced by straight line equation)
                    float l_C = 1.0f + (l_A * l_A);
                    float l_D = (2.0f * l_A * l_B);
                    float l_R = (l_B * l_B) - (l_Radius * l_Radius);

                    /// Roots of the circle equation
                    float l_RootA = (-l_D + std::sqrt((l_D * l_D) - 4.0f * l_C * l_R)) / (2.0f * l_C);
                    float l_RootB = (-l_D - std::sqrt((l_D * l_D) - 4.0f * l_C * l_R)) / (2.0f * l_C);

                    /// Abscissa of Roots
                    float l_RootAY = l_A * l_RootA + l_B;
                    float l_RootBY = l_A * l_RootB + l_B;

                    /// Relocate at correct coordinates
                    Position l_IntersectA = { g_CenterPos.m_positionX + l_RootA, g_CenterPos.m_positionY + l_RootAY, g_CenterPos.m_positionZ };
                    Position l_IntersectB = { g_CenterPos.m_positionX + l_RootB, g_CenterPos.m_positionY + l_RootBY, g_CenterPos.m_positionZ };

                    Position l_Pos;

                    if (l_IntersectA.IsInBack(me))
                        l_Pos = l_IntersectA;
                    else
                        l_Pos = l_IntersectB;

                    l_Pos.m_positionZ = 292.0f;

                    DoCast(l_Pos, eSpells::SpellEdgeOfObliterationDamage, true);

                    std::vector<G3D::Vector3> l_Path;

                    float l_Step = me->GetExactDist(&l_Pos) / 16;
                    float l_Dist = l_Step;

                    for (uint8 l_I = 0; l_I < 15; ++l_I)
                    {
                        Position l_Point = me->GetPosition();

                        l_Point.SimplePosXYRelocationByAngle(l_Point, l_Dist, me->m_orientation, true);

                        l_Path.push_back(l_Point.AsVector3());

                        l_Dist += l_Step;
                    }

                    l_Path.push_back(l_Pos.AsVector3());

                    Optional<Movement::SplineSpellEffectExtraData> l_SpellEffectExtraData;
                    l_SpellEffectExtraData.emplace();
                    l_SpellEffectExtraData->SpellVisualId = 70108;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJumpWithPath(453.3f, 4.39216f, l_Path.data(), l_Path.size(), EventId::EVENT_CHARGE, 0, l_SpellEffectExtraData.get_ptr());

                    if (AreaTrigger* l_AT = me->GetAreaTrigger(eSpells::SpellEdgeOfObliterationWarning))
                    {
                        l_AT->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_SPELL_ID, 0);
                        l_AT->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                        l_AT->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                        l_AT->SetDuration(200);
                    }
                });

                p_AreaTrigger->SetDuration(p_AreaTrigger->GetDuration() + 500);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID != EVENT_CHARGE)
                    return;

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellEdgeOfObliterationSearcher, true);
                });
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellEdgeOfObliterationSearcher:
                    {
                        me->SetFacingTo(me->GetAngle(p_Target));

                        m_TargetedPos = p_Target->GetPosition();

                        DoCast(me, eSpells::SpellEdgeOfObliterationWarning, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellEdgeOfObliterationDamage:
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
            return new npc_argus_edge_of_obliterationAI(p_Creature);
        }
};

/// Constellar Designate - 127192
class npc_argus_constellar_designate : public CreatureScript
{
    public:
        npc_argus_constellar_designate() : CreatureScript("npc_argus_constellar_designate") { }

        enum eSpells
        {
            /// Misc
            SpellZeroEnergyZeroRegen    = 72242,
            SpellTheDiscsOfNorgannon    = 252516,
            SpellBladesOfTheEternal     = 255478,
            SpellSwordOfTheCosmos       = 255496,
            SpellInevitability          = 253021,
            SpellImpedingInevitability  = 253026,
            /// Starblast
            SpellStarblast              = 253061,
            /// Cosmic Ray
            SpellCosmicRayAura          = 252729,
            /// Cosmic Beacon
            SpellCosmicBeaconCast       = 252616,
            /// Cosmic Power
            SpellCosmicPower            = 255935,
            /// Vulnerabilities
            SpellPhysicalVulnerability  = 255418,
            SpellHolyVulnerability      = 255419,
            SpellNatureVulnerability    = 255422,
            SpellFrostVulnerability     = 255425,
            SpellFireVulnerability      = 255429,
            SpellShadowVulnerability    = 255430,
            SpellArcaneVulnerability    = 255433
        };

        enum eVulnerabilities
        {
            VulnerabilityPhysical,
            VulnerabilityHoly,
            VulnerabilityNature,
            VulnerabilityFrost,
            VulnerabilityFire,
            VulnerabilityShadow,
            VulnerabilityArcane
        };

        enum eEvents
        {
            EventStarblast = 1,
            EventCosmicRay,
            EventCosmicBeacon,
            EventCosmicPower
        };

        enum eActions
        {
            ActionCosmicBeacon = 9,
            ActionBladesOfTheEternal,
            ActionSwordOfTheCosmos,
            ActionCosmicRay = 13
        };

        enum eItem
        {
            ItemID = 113093
        };

        struct npc_argus_constellar_designateAI : public ScriptedAI
        {
            npc_argus_constellar_designateAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            int32 m_VulnerabilityIDx = -1;

            bool m_Passived = true;
            bool m_HasWeapon = false;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IsPassived() override
            {
                return m_Passived;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                SetCombatMovement(false);

                me->DisableHealthRegen();
                me->DisableEvadeMode();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellZeroEnergyZeroRegen, true);

                if (IsHeroic())
                    DoCast(me, eSpells::SpellImpedingInevitability, true);

                AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    m_Passived = false;

                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    if (Player* l_Player = me->FindNearestPlayer(100.0f))
                        AttackStart(l_Player);
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventStarblast, 1);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCosmicBeacon:
                    {
                        events.ScheduleEvent(eEvents::EventCosmicBeacon, 1);
                        break;
                    }
                    case eActions::ActionBladesOfTheEternal:
                    {
                        WeaponizeMe(eSpells::SpellBladesOfTheEternal);
                        break;
                    }
                    case eActions::ActionSwordOfTheCosmos:
                    {
                        events.ScheduleEvent(eEvents::EventCosmicPower, 8 * TimeConstants::IN_MILLISECONDS);

                        WeaponizeMe(eSpells::SpellSwordOfTheCosmos);
                        break;
                    }
                    case eActions::ActionCosmicRay:
                    {
                        events.ScheduleEvent(eEvents::EventCosmicRay, 1);
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
                    case eSpells::SpellTheDiscsOfNorgannon:
                    {
                        std::map<uint32, uint32> l_VulnerabilitySpells =
                        {
                            { eVulnerabilities::VulnerabilityPhysical,  eSpells::SpellPhysicalVulnerability },
                            { eVulnerabilities::VulnerabilityHoly,      eSpells::SpellHolyVulnerability     },
                            { eVulnerabilities::VulnerabilityNature,    eSpells::SpellNatureVulnerability   },
                            { eVulnerabilities::VulnerabilityFrost,     eSpells::SpellFrostVulnerability    },
                            { eVulnerabilities::VulnerabilityFire,      eSpells::SpellFireVulnerability     },
                            { eVulnerabilities::VulnerabilityShadow,    eSpells::SpellShadowVulnerability   },
                            { eVulnerabilities::VulnerabilityArcane,    eSpells::SpellArcaneVulnerability   }
                        };

                        auto const& l_Iter = l_VulnerabilitySpells.find(m_VulnerabilityIDx);
                        if (l_Iter == l_VulnerabilitySpells.end())
                            break;

                        DoCast(me, l_Iter->second, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                m_VulnerabilityIDx = p_Value;
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellCosmicRayAura:
                    case eSpells::SpellCosmicBeaconCast:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (!p_Object || !p_Object->IsPlayer())
                                return true;

                            if (p_Object->ToUnit()->HasAura(eSpells::SpellCosmicBeaconCast) ||
                                p_Object->ToUnit()->HasAura(eSpells::SpellCosmicRayAura))
                                return true;

                            return false;
                        });

                        if (p_Targets.size() > 1)
                            JadeCore::Containers::RandomResizeList(p_Targets, 1);

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
                    case eSpells::SpellBladesOfTheEternal:
                    case eSpells::SpellSwordOfTheCosmos:
                    {
                        m_HasWeapon = false;

                        SetCombatMovement(false);

                        me->SetVirtualItem(0, 0);

                        me->StopMoving();
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 /*p_ID*/) override
            {
                return m_HasWeapon;
            }

            void OnSpellInterrupted(SpellInfo const* /*p_SpellInfo*/) override
            {
                if (IsHeroic())
                    DoCast(me, eSpells::SpellInevitability, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Passived || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventStarblast:
                    {
                        if (!m_HasWeapon)
                        {
                            if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                                DoCast(l_Target, eSpells::SpellStarblast);
                        }

                        events.ScheduleEvent(eEvents::EventStarblast, 2 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    case eEvents::EventCosmicRay:
                    {
                        if (!m_HasWeapon)
                            DoCast(me, eSpells::SpellCosmicRayAura, true);

                        break;
                    }
                    case eEvents::EventCosmicBeacon:
                    {
                        if (!m_HasWeapon)
                            DoCast(me, eSpells::SpellCosmicBeaconCast);

                        break;
                    }
                    case eEvents::EventCosmicPower:
                    {
                        DoCast(me, eSpells::SpellCosmicPower);

                        events.ScheduleEvent(eEvents::EventCosmicPower, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void WeaponizeMe(uint32 p_SpellID)
            {
                m_HasWeapon = true;

                SetCombatMovement(true);

                me->SetVirtualItem(0, eItem::ItemID);

                DoCast(me, p_SpellID, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_constellar_designateAI(p_Creature);
        }
};

/// Gift of the Lifebinder - 129386
class npc_argus_gift_of_the_lifebinder : public CreatureScript
{
    public:
        npc_argus_gift_of_the_lifebinder() : CreatureScript("npc_argus_gift_of_the_lifebinder") { }

        enum eSpells
        {
            SpellGiftOfTheLifebinderAT  = 256338,
            SpellLifeGivingRoots        = 256398,
            SpellZeroEnergyZeroRegen    = 72242
        };

        enum eVisual
        {
            VisualAnimKit = 2133
        };

        struct npc_argus_gift_of_the_lifebinderAI : public Scripted_NoMovementAI
        {
            npc_argus_gift_of_the_lifebinderAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance = nullptr;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);
                me->m_serverSideVisibilityDetect.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetAIAnimKitId(eVisual::VisualAnimKit);

                me->DisableHealthRegen();
                me->DisableEvadeMode();

                DoCast(me, eSpells::SpellGiftOfTheLifebinderAT, true);
                DoCast(me, eSpells::SpellLifeGivingRoots, true);
                DoCast(me, eSpells::SpellZeroEnergyZeroRegen, true);

                me->SetPower(Powers::POWER_ENERGY, me->GetMaxPower(Powers::POWER_ENERGY));

                if (m_Instance)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_SET_ALLOWING_RELEASE, nullptr, true);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_SET_ALLOWING_RELEASE, nullptr, false);
                }
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_gift_of_the_lifebinderAI(p_Creature);
        }
};

/// Reorigination Module - 127809
class npc_argus_reorigination_module : public CreatureScript
{
    public:
        npc_argus_reorigination_module() : CreatureScript("npc_argus_reorigination_module") { }

        enum eSpells
        {
            SpellReoriginationModuleAT      = 256389,
            SpellInitializationSequence     = 256388,
            SpellInitializationSequenceLFR  = 258029,
            SpellReoriginationPulse         = 256396,
            SpellReoriginationPulseSingle   = 256393
        };

        enum eVisual
        {
            VisualAnimKit = 2133
        };

        struct npc_argus_reorigination_moduleAI : public Scripted_NoMovementAI
        {
            npc_argus_reorigination_moduleAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool m_Pulsating = false;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                ApplyAllImmunities(true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetAIAnimKitId(eVisual::VisualAnimKit);

                me->DisableHealthRegen();
                me->DisableEvadeMode();

                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellReoriginationModuleAT, true);

                    if (IsLFR())
                        DoCast(me, eSpells::SpellInitializationSequenceLFR);
                    else
                        DoCast(me, eSpells::SpellInitializationSequence);
                });
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Pulsating || p_Damage >= me->GetHealth())
                {
                    if (!m_Pulsating)
                    {
                        me->InterruptNonMeleeSpells(true);

                        DoCast(me, eSpells::SpellReoriginationPulse);

                        if (AreaTrigger* l_AT = me->GetAreaTrigger(eSpells::SpellReoriginationModuleAT))
                            l_AT->DisableScaling();
                    }

                    m_Pulsating = true;

                    p_Damage = 0;

                    me->SetHealth(1);
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool /*p_Removed*/) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellReoriginationModuleAT)
                {
                    float l_Radius = p_AreaTrigger->GetRadius();

                    std::list<Player*> l_PlayerList;

                    p_AreaTrigger->GetPlayerListInGrid(l_PlayerList, l_Radius);

                    for (Player* l_Player : l_PlayerList)
                        DoCast(l_Player, eSpells::SpellReoriginationPulseSingle, true);

                    for (uint8 l_I = 0; l_I < 5; ++l_I)
                    {
                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(15.0f, 40.0f), frand(0.0f, 2.0f * M_PI), true);

                        if (Unit* l_Argus = me->GetAnyOwner())
                            l_Argus->SummonCreature(eCreatures::NpcMoteOfTitanicPower, l_Pos);
                    }

                    me->DespawnOrUnsummon(1);
                }
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                uint32 l_Damage = me->GetMaxHealth();

                if (p_SpellID == eSpells::SpellInitializationSequence || p_SpellID == eSpells::SpellInitializationSequenceLFR)
                    DamageTaken(nullptr, l_Damage, nullptr);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_reorigination_moduleAI(p_Creature);
        }
};

/// Hungering Soul - 129635
class npc_argus_hungering_soul : public CreatureScript
{
    public:
        npc_argus_hungering_soul() : CreatureScript("npc_argus_hungering_soul") { }

        enum eSpells
        {
            SpellHungeringSoulAT    = 256894,
            SpellGhostAura          = 258826,
            SpellSoulDetonationAoE  = 256899
        };

        struct npc_argus_hungering_soulAI : public ScriptedAI
        {
            npc_argus_hungering_soulAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint32 m_CheckPlayerTimer = 0;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IsPassived() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_GHOST);
                me->m_serverSideVisibilityDetect.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                DoCast(me, eSpells::SpellHungeringSoulAT, true);
                DoCast(me, eSpells::SpellGhostAura, true);

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveRandom(15.0f);

                    m_CheckPlayerTimer = 100;
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_CheckPlayerTimer)
                    return;

                if (m_CheckPlayerTimer <= p_Diff)
                {
                    if (me->FindNearestDeadPlayer(0.0f))
                        DoCast(me, eSpells::SpellSoulDetonationAoE, true);

                    m_CheckPlayerTimer = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_CheckPlayerTimer -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_hungering_soulAI(p_Creature);
        }
};

/// Mote of Titanic Power - 129722
class npc_argus_mote_of_titanic_power : public CreatureScript
{
    public:
        npc_argus_mote_of_titanic_power() : CreatureScript("npc_argus_mote_of_titanic_power") { }

        enum eSpells
        {
            SpellGhostAura                  = 9036,
            SpellMoteOfTitanicPowerAT       = 253569,
            SpellMoteOfTitanicPowerEnergy   = 253580
        };

        struct npc_argus_mote_of_titanic_powerAI : public ScriptedAI
        {
            npc_argus_mote_of_titanic_powerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint32 m_CheckPlayerTimer = 0;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IsPassived() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_GHOST);
                me->m_serverSideVisibilityDetect.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_GHOST);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellGhostAura, true);
                DoCast(me, eSpells::SpellMoteOfTitanicPowerAT, true);

                m_CheckPlayerTimer = 100;
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target->GetEntry() == eCreatures::NpcKhazgoroth && p_SpellInfo->Id == eSpells::SpellMoteOfTitanicPowerEnergy)
                    me->DespawnOrUnsummon(1);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_CheckPlayerTimer)
                    return;

                if (m_CheckPlayerTimer <= p_Diff)
                {
                    if (me->FindNearestDeadPlayer(1.0f))
                    {
                        me->RemoveAura(eSpells::SpellMoteOfTitanicPowerAT);

                        DoCast(me, eSpells::SpellMoteOfTitanicPowerEnergy, true);

                        if (me->GetAnyOwner())
                        {
                            Creature* l_Argus = me->GetAnyOwner()->ToCreature();

                            if (l_Argus && l_Argus->IsAIEnabled)
                                l_Argus->AI()->SetData(0, 0);
                        }

                        m_CheckPlayerTimer = 0;
                        return;
                    }

                    m_CheckPlayerTimer = 200;
                }
                else
                    m_CheckPlayerTimer -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_mote_of_titanic_powerAI(p_Creature);
        }
};

/// Apocalypsis Module - 130179
class npc_argus_apocalypsis_module : public CreatureScript
{
    public:
        npc_argus_apocalypsis_module() : CreatureScript("npc_argus_apocalypsis_module") { }

        enum eSpells
        {
            SpellApocalypsisModuleAT        = 258007,
            SpellInitializationSequence     = 258029,
            SpellApocalypsisZone            = 258030,
            SpellApocalypsisZoneAT          = 258034,
            SpellApocalypsisPulse           = 258038,
            SpellApocalypsisPulseDamage     = 258037
        };

        enum eVisual
        {
            VisualAnimKit = 2133
        };

        struct npc_argus_apocalypsis_moduleAI : public Scripted_NoMovementAI
        {
            npc_argus_apocalypsis_moduleAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool m_Pulsating = false;

            float m_Radius = 0.0f;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                ApplyAllImmunities(true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetAIAnimKitId(eVisual::VisualAnimKit);

                me->DisableHealthRegen();
                me->DisableEvadeMode();

                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                    DoCast(me, eSpells::SpellApocalypsisModuleAT, true);
                    DoCast(me, eSpells::SpellInitializationSequence);
                });
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Pulsating || p_Damage >= me->GetHealth())
                {
                    if (!m_Pulsating)
                    {
                        me->InterruptNonMeleeSpells(true);

                        DoCast(me, eSpells::SpellApocalypsisZone);

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                            l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                        if (AreaTrigger* l_AT = me->GetAreaTrigger(eSpells::SpellApocalypsisModuleAT))
                            l_AT->DisableScaling();
                    }

                    m_Pulsating = true;

                    p_Damage = 0;

                    me->SetHealth(1);
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool /*p_Removed*/) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellApocalypsisModuleAT)
                {
                    m_Radius = p_AreaTrigger->GetRadius();

                    for (uint8 l_I = 0; l_I < 15; ++l_I)
                    {
                        Position l_Pos = g_CenterPos;

                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, frand(15.0f, 40.0f), frand(0.0f, 2.0f * M_PI), true);

                        if (Unit* l_Argus = me->GetAnyOwner())
                            l_Argus->SummonCreature(eCreatures::NpcMoteOfTitanicPower, l_Pos);
                    }

                    DoCast(me, eSpells::SpellApocalypsisZoneAT, true);
                    DoCast(me, eSpells::SpellApocalypsisPulse, true);

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellApocalypsisZoneAT && !G3D::fuzzyEq(m_Radius, 0.0f))
                {
                    p_AreaTrigger->SetRadius(m_Radius);

                    p_AreaTrigger->SetCircleAreaTrigger(m_Radius);
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellApocalypsisPulseDamage:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                        {
                            if (p_Object->GetExactDist2d(me) > m_Radius)
                                return true;

                            return false;
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

                uint32 l_Damage = me->GetMaxHealth();

                if (p_SpellID == eSpells::SpellInitializationSequence)
                    DamageTaken(nullptr, l_Damage, nullptr);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_apocalypsis_moduleAI(p_Creature);
        }
};

/// Chains of Sargeras - 130202
class npc_argus_chains_of_sargeras : public CreatureScript
{
    public:
        npc_argus_chains_of_sargeras() : CreatureScript("npc_argus_chains_of_sargeras") { }

        enum eSpell
        {
            SpellSentenceOfSargerasDoT = 257966
        };

        struct npc_argus_chains_of_sargerasAI : public Scripted_NoMovementAI
        {
            npc_argus_chains_of_sargerasAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IsPassived() override
            {
                return true;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(p_Summoner, eSpell::SpellSentenceOfSargerasDoT, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_argus_chains_of_sargerasAI(p_Creature);
        }
};

/// Edge of Annihilation - 130842
class npc_argus_edge_of_annihilation : public CreatureScript
{
    public:
        npc_argus_edge_of_annihilation() : CreatureScript("npc_argus_edge_of_annihilation") { }

        enum eSpells
        {
            SpellEdgeOfAnnihilationVisual   = 258832,
            SpellEdgeOfAnnihilationDamage   = 258834,
            SpellEdgeOfAnnihilationClean    = 258835
        };

        enum eRestrict
        {
            TargetRestrict = 39540
        };

        struct npc_argus_edge_of_annihilationAI : public ScriptedAI
        {
            npc_argus_edge_of_annihilationAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            Position m_TargetedPos;

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool IsPassived() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                DoCast(me, eSpells::SpellEdgeOfAnnihilationVisual, true);

                me->SimplePosXYRelocationByAngle(m_TargetedPos, 43.0f, me->m_orientation, true);

                AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    /// Relocate center of circle to 0 to avoid useless calculation
                    float l_MeRelativeX = me->m_positionX - g_CenterPos.m_positionX;
                    float l_MeRelativeY = me->m_positionY - g_CenterPos.m_positionY;;
                    float l_TargetRelativeX = m_TargetedPos.m_positionX - g_CenterPos.m_positionX;
                    float l_TargetRelativeY = m_TargetedPos.m_positionY - g_CenterPos.m_positionY;

                    float l_Radius = 43.0f;

                    /// straight line equation (ax +b) between Scythe and player
                    float l_A = (l_MeRelativeY - l_TargetRelativeY) / (l_MeRelativeX - l_TargetRelativeX);
                    float l_B = -(l_A * l_MeRelativeX) + l_MeRelativeY;

                    /// Circle equation ((x-c)^2 + (y-d)^2 = radius^2) (center 0, y replaced by straight line equation)
                    float l_C = 1.0f + (l_A * l_A);
                    float l_D = (2.0f * l_A * l_B);
                    float l_R = (l_B * l_B) - (l_Radius * l_Radius);

                    /// Roots of the circle equation
                    float l_RootA = (-l_D + std::sqrt((l_D * l_D) - 4.0f * l_C * l_R)) / (2.0f * l_C);
                    float l_RootB = (-l_D - std::sqrt((l_D * l_D) - 4.0f * l_C * l_R)) / (2.0f * l_C);

                    /// Abscissa of Roots
                    float l_RootAY = l_A * l_RootA + l_B;
                    float l_RootBY = l_A * l_RootB + l_B;

                    /// Relocate at correct coordinates
                    Position l_IntersectA = { g_CenterPos.m_positionX + l_RootA, g_CenterPos.m_positionY + l_RootAY, g_CenterPos.m_positionZ };
                    Position l_IntersectB = { g_CenterPos.m_positionX + l_RootB, g_CenterPos.m_positionY + l_RootBY, g_CenterPos.m_positionZ };

                    Position l_Pos;

                    if (l_IntersectA.IsInBack(me))
                        l_Pos = l_IntersectA;
                    else
                        l_Pos = l_IntersectB;

                    l_Pos.m_positionZ = 292.3707f;

                    if (Unit* l_Owner = me->GetAnyOwner())
                        me->CastSpell(l_Pos, eSpells::SpellEdgeOfAnnihilationDamage, true, nullptr, nullptr, l_Owner->GetGUID());

                    std::vector<G3D::Vector3> l_Path;

                    float l_Step = me->GetExactDist(&l_Pos) / 16;
                    float l_Dist = l_Step;

                    for (uint8 l_I = 0; l_I < 15; ++l_I)
                    {
                        Position l_Point = me->GetPosition();

                        l_Point.SimplePosXYRelocationByAngle(l_Point, l_Dist, me->m_orientation, true);

                        l_Path.push_back(l_Point.AsVector3());

                        l_Dist += l_Step;
                    }

                    l_Path.push_back(l_Pos.AsVector3());

                    Optional<Movement::SplineSpellEffectExtraData> l_SpellEffectExtraData;
                    l_SpellEffectExtraData.emplace();
                    l_SpellEffectExtraData->SpellVisualId = 72775;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJumpWithPath(453.3f, 4.39216f, l_Path.data(), l_Path.size(), EventId::EVENT_CHARGE, 0, l_SpellEffectExtraData.get_ptr());
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID != EVENT_CHARGE)
                    return;

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    DoCast(me, eSpells::SpellEdgeOfAnnihilationClean, true);

                    me->DespawnOrUnsummon(1);
                });
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellEdgeOfAnnihilationDamage:
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
            return new npc_argus_edge_of_annihilationAI(p_Creature);
        }
};

/// Argus P1 Energy Controller - 258041
class spell_argus_argus_p1_energy_controller : public SpellScriptLoader
{
    public:
        spell_argus_argus_p1_energy_controller() : SpellScriptLoader("spell_argus_argus_p1_energy_controller") { }

        enum eAction
        {
            ActionConeOfDeath
        };

        class spell_argus_argus_p1_energy_controller_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_argus_p1_energy_controller_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetTarget())
                    return;

                if (Creature* l_Argus = GetTarget()->ToCreature())
                {
                    if (l_Argus->IsAIEnabled && l_Argus->AI()->GetData(0))
                    {
                        l_Argus->ModifyPower(Powers::POWER_ENERGY, 5);

                        if (l_Argus->GetPower(Powers::POWER_ENERGY) >= l_Argus->GetMaxPower(Powers::POWER_ENERGY))
                            l_Argus->AI()->DoAction(eAction::ActionConeOfDeath);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_argus_p1_energy_controller_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_argus_p1_energy_controller_AuraScript();
        }
};

/// Argus P2 Energy Controller - 258042
class spell_argus_argus_p2_energy_controller : public SpellScriptLoader
{
    public:
        spell_argus_argus_p2_energy_controller() : SpellScriptLoader("spell_argus_argus_p2_energy_controller") { }

        enum eActions
        {
            ActionSoulburst = 4,
            ActionSoulbomb = 5
        };

        class spell_argus_argus_p2_energy_controller_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_argus_p2_energy_controller_AuraScript);

            bool m_Even = true;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetTarget())
                    return;

                if (Creature* l_Argus = GetTarget()->ToCreature())
                {
                    if (l_Argus->IsAIEnabled && l_Argus->AI()->GetData(0))
                    {
                        uint32 l_Energy = l_Argus->GetPower(Powers::POWER_ENERGY);

                        l_Argus->ModifyPower(Powers::POWER_ENERGY, m_Even ? 2 : 3);

                        m_Even = !m_Even;

                        if (l_Energy < 50 && l_Argus->GetPower(Powers::POWER_ENERGY) >= 50)
                            l_Argus->AI()->DoAction(eActions::ActionSoulburst);
                        else if (l_Energy < 100 && l_Argus->GetPower(Powers::POWER_ENERGY) >= l_Argus->GetMaxPower(Powers::POWER_ENERGY))
                            l_Argus->AI()->DoAction(eActions::ActionSoulbomb);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_argus_p2_energy_controller_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_argus_p2_energy_controller_AuraScript();
        }
};

/// Argus P4 Energy Controller - 258044
class spell_argus_argus_p4_energy_controller : public SpellScriptLoader
{
    public:
        spell_argus_argus_p4_energy_controller() : SpellScriptLoader("spell_argus_argus_p4_energy_controller") { }

        enum eActions
        {
            ActionSoulburst = 4,
            ActionSoulbomb = 5
        };

        class spell_argus_argus_p4_energy_controller_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_argus_p4_energy_controller_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetTarget())
                    return;

                if (Creature* l_Argus = GetTarget()->ToCreature())
                {
                    if (l_Argus->IsAIEnabled && l_Argus->AI()->GetData(0))
                    {
                        uint32 l_Energy = l_Argus->GetPower(Powers::POWER_ENERGY);

                        l_Argus->ModifyPower(Powers::POWER_ENERGY, 2);

                        if (l_Energy < 50 && l_Argus->GetPower(Powers::POWER_ENERGY) > 50)
                            l_Argus->AI()->DoAction(eActions::ActionSoulburst);
                        else if (l_Argus->GetPower(Powers::POWER_ENERGY) >= l_Argus->GetMaxPower(Powers::POWER_ENERGY))
                            l_Argus->AI()->DoAction(eActions::ActionSoulbomb);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_argus_p4_energy_controller_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_argus_p4_energy_controller_AuraScript();
        }
};

/// Cone of Death - 248166
class spell_argus_cone_of_death_at : public SpellScriptLoader
{
    public:
        spell_argus_cone_of_death_at() : SpellScriptLoader("spell_argus_cone_of_death_at") { }

        class spell_argus_cone_of_death_at_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_cone_of_death_at_SpellScript);

            void ModDestHeight(SpellEffIndex /*p_EffIndex*/)
            {
                WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest());
                if (l_Dest == nullptr)
                    return;

                float l_ExpectedZ = 292.0061f;

                Position const l_Offset = { 0.0f, 0.0f, l_ExpectedZ - l_Dest->GetPositionZ(), 0.0f };

                l_Dest->RelocateOffset(l_Offset);
                GetHitDest()->RelocateOffset(l_Offset);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_argus_cone_of_death_at_SpellScript::ModDestHeight, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_CREATE_AREATRIGGER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_cone_of_death_at_SpellScript();
        }
};

/// Soulblight Orb - 248317
class spell_argus_soulblight_orb : public SpellScriptLoader
{
    public:
        spell_argus_soulblight_orb() : SpellScriptLoader("spell_argus_soulblight_orb") { }

        class spell_argus_soulblight_orb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_soulblight_orb_SpellScript);

            void ModDestHeight(SpellEffIndex /*p_EffIndex*/)
            {
                WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest());
                if (l_Dest == nullptr)
                    return;

                Position const l_Offset = { 0.0f, 0.0f, 8.2f, 0.0f };

                l_Dest->RelocateOffset(l_Offset);
                GetHitDest()->RelocateOffset(l_Offset);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_argus_soulblight_orb_SpellScript::ModDestHeight, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_soulblight_orb_SpellScript();
        }
};

/// Soulblight - 248396
class spell_argus_soulblight : public SpellScriptLoader
{
    public:
        spell_argus_soulblight() : SpellScriptLoader("spell_argus_soulblight") { }

        enum eSpell
        {
            SpellDeathFogAT = 248290
        };

        class spell_argus_soulblight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_soulblight_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                GetTarget()->CastSpell(GetTarget(), eSpell::SpellDeathFogAT, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_soulblight_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_soulblight_AuraScript();
        }
};

/// Gift of the Sky - 258646
class spell_argus_gift_of_the_sky : public SpellScriptLoader
{
    public:
        spell_argus_gift_of_the_sky() : SpellScriptLoader("spell_argus_gift_of_the_sky") { }

        enum eSpell
        {
            SpellGiftOfTheSkyMissile = 253884
        };

        class spell_argus_gift_of_the_sky_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_gift_of_the_sky_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellGiftOfTheSkyMissile, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_gift_of_the_sky_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_gift_of_the_sky_AuraScript();
        }
};

/// Gift of the Sea - 258647
class spell_argus_gift_of_the_sea : public SpellScriptLoader
{
    public:
        spell_argus_gift_of_the_sea() : SpellScriptLoader("spell_argus_gift_of_the_sea") { }

        enum eSpell
        {
            SpellGiftOfTheSeaMissile = 253889
        };

        class spell_argus_gift_of_the_sea_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_gift_of_the_sea_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::SpellGiftOfTheSeaMissile, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_gift_of_the_sea_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_gift_of_the_sea_AuraScript();
        }
};

/// Soulburst - 250669
class spell_argus_soulburst : public SpellScriptLoader
{
    public:
        spell_argus_soulburst() : SpellScriptLoader("spell_argus_soulburst") { }

        enum eSpell
        {
            SpellSoulburstDetonation = 251572
        };

        class spell_argus_soulburst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_soulburst_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_DEFAULT && l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                GetTarget()->CastSpell(GetTarget(), eSpell::SpellSoulburstDetonation, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_soulburst_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_soulburst_AuraScript();
        }
};

/// Soulburst Detonation - 251572
class spell_argus_soulburst_detonation : public SpellScriptLoader
{
    public:
        spell_argus_soulburst_detonation() : SpellScriptLoader("spell_argus_soulburst_detonation") { }

        class spell_argus_soulburst_detonation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_soulburst_detonation_SpellScript);

            void DealDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || !GetExplTargetDest() || l_Caster->GetMap()->IsLFR())
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();

                int32 l_MaxDamage = l_SpellInfo->Effects[p_EffIndex].CalcValue(l_Caster);

                Position l_Pos = *GetExplTargetDest();

                bool l_Normal = l_Caster->GetMap()->IsNormal();

                float l_Distance = std::min(l_Normal ? 35.0f : 45.0f, l_Target->GetDistance(l_Pos));

                /// Set to max damage if <= 1 yards to the boss
                if (l_Distance <= 1.0f)
                {
                    SetHitDamage(l_MaxDamage);
                    return;
                }

                float l_DamagePct = float((l_Normal ? -2.058824f : -1.5909091f) * l_Distance + (l_Normal ? 102.058824f : 101.5909091f));
                int32 l_NewDamage = CalculatePct(l_MaxDamage, l_DamagePct);

                SetHitDamage(l_NewDamage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_soulburst_detonation_SpellScript::DealDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_soulburst_detonation_SpellScript();
        }
};

/// Soulbomb - 251570
class spell_argus_soulbomb : public SpellScriptLoader
{
    public:
        spell_argus_soulbomb() : SpellScriptLoader("spell_argus_soulbomb") { }

        enum eSpell
        {
            SpellSoulbombDetonation = 251571
        };

        class spell_argus_soulbomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_soulbomb_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_DEFAULT && l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                GetTarget()->CastSpell(GetTarget(), eSpell::SpellSoulbombDetonation, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_soulbomb_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_soulbomb_AuraScript();
        }
};

/// Soulbomb Detonation - 251571
class spell_argus_soulbomb_detonation : public SpellScriptLoader
{
    public:
        spell_argus_soulbomb_detonation() : SpellScriptLoader("spell_argus_soulbomb_detonation") { }

        class spell_argus_soulbomb_detonation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_soulbomb_detonation_SpellScript);

            void DealDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || !GetExplTargetDest() || l_Caster->GetMap()->IsLFR())
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();

                int32 l_MaxDamage = l_SpellInfo->Effects[p_EffIndex].CalcValue(l_Caster);

                Position l_Pos = *GetExplTargetDest();

                bool l_Normal = l_Caster->GetMap()->IsNormal();

                float l_Distance = std::min(l_Normal ? 35.0f : 45.0f, l_Target->GetDistance(l_Pos));

                /// Set to max damage if <= 1 yards to the boss
                if (l_Distance <= 1.0f)
                {
                    SetHitDamage(l_MaxDamage);
                    return;
                }

                float l_DamagePct = float((l_Normal ? -2.5f : -2.14293f) * l_Distance + (l_Normal ? 102.5f : 101.93182f));
                int32 l_NewDamage = CalculatePct(l_MaxDamage, l_DamagePct);

                SetHitDamage(l_NewDamage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_argus_soulbomb_detonation_SpellScript::DealDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_soulbomb_detonation_SpellScript();
        }
};

/// Aggramar's Boon - 255200
class spell_argus_aggramars_boon : public SpellScriptLoader
{
    public:
        spell_argus_aggramars_boon() : SpellScriptLoader("spell_argus_aggramars_boon") { }

        class spell_argus_aggramars_boon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_aggramars_boon_AuraScript);

            enum eSpell
            {
                SpellAggramarsSacrifice = 255201
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                /// Absorb all incoming damages...
                p_AbsorbAmount = p_DmgInfo.GetAmount();
                SpellInfo const* l_SpellInfo = p_DmgInfo.GetSpellInfo();
                if (l_SpellInfo && l_SpellInfo->Id == eSpell::SpellAggramarsSacrifice)
                    return;

                /// And redirect damages on Avatar of Aggramar target
                if (Unit* l_Target = GetTarget())
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        if (l_Caster == l_Target)
                            return;

                        int32 l_Damage = p_AbsorbAmount;

                        l_Target->CastCustomSpell(l_Caster, eSpell::SpellAggramarsSacrifice, &l_Damage, nullptr, nullptr, true);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_argus_aggramars_boon_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_argus_aggramars_boon_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_aggramars_boon_AuraScript();
        }
};

/// Cosmic Ray - 252729
class spell_argus_cosmic_ray : public SpellScriptLoader
{
    public:
        spell_argus_cosmic_ray() : SpellScriptLoader("spell_argus_cosmic_ray") { }

        enum eSpell
        {
            SpellCosmicRayDmg = 252707
        };

        class spell_argus_cosmic_ray_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_cosmic_ray_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE || !GetCaster())
                    return;

                GetCaster()->CastSpell(GetTarget(), eSpell::SpellCosmicRayDmg, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_cosmic_ray_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_cosmic_ray_AuraScript();
        }
};

/// Cosmic Ray - 252707
class spell_argus_cosmic_ray_damage : public SpellScriptLoader
{
    public:
        spell_argus_cosmic_ray_damage() : SpellScriptLoader("spell_argus_cosmic_ray_damage") { }

        class spell_argus_cosmic_ray_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argus_cosmic_ray_damage_SpellScript);

            enum eSpell
            {
                TargetRestrict = 38387
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                if (l_Restriction == nullptr)
                    return;

                p_Targets.remove_if([this, l_Caster, l_Restriction](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (!p_Object->IsInAxe(l_Caster, l_Restriction->Width / 2.0f, 300.0f))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_argus_cosmic_ray_damage_SpellScript::FilterTargets, SpellEffIndex::EFFECT_1, Targets::TARGET_UNIT_ENEMY_BETWEEN_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_argus_cosmic_ray_damage_SpellScript();
        }
};

/// Cosmic Beacon - 252616
class spell_argus_cosmic_beacon : public SpellScriptLoader
{
    public:
        spell_argus_cosmic_beacon() : SpellScriptLoader("spell_argus_cosmic_beacon") { }

        enum eSpell
        {
            SpellCosmicSmashMissile = 252630
        };

        class spell_argus_cosmic_beacon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_cosmic_beacon_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE || !GetCaster())
                    return;

                GetCaster()->CastSpell(GetTarget(), eSpell::SpellCosmicSmashMissile, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_cosmic_beacon_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_cosmic_beacon_AuraScript();
        }
};

/// Grasp of the Unmaker - 258373
class spell_argus_grasp_of_the_unmaker : public SpellScriptLoader
{
    public:
        spell_argus_grasp_of_the_unmaker() : SpellScriptLoader("spell_argus_grasp_of_the_unmaker") { }

        enum eSpell
        {
            SpellGraspOfTheUnmakerJump = 258375
        };

        class spell_argus_grasp_of_the_unmaker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_grasp_of_the_unmaker_AuraScript);

            Position const m_JumpPos = { 2865.725f, -4567.749f, 297.0026f, 2.79527f };

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->SetDisableGravity(true);
                    l_Target->SendPlayHoverAnim(true);

                    l_Target->CastSpell(m_JumpPos, eSpell::SpellGraspOfTheUnmakerJump, true);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->SetDisableGravity(false);
                    l_Target->SendPlayHoverAnim(false);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_argus_grasp_of_the_unmaker_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_STRANGULATE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_grasp_of_the_unmaker_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_STRANGULATE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_grasp_of_the_unmaker_AuraScript();
        }
};

/// Ember of Rage - 257300
class spell_argus_ember_of_rage : public SpellScriptLoader
{
    public:
        spell_argus_ember_of_rage() : SpellScriptLoader("spell_argus_ember_of_rage") { }

        enum eSpell
        {
            SpellEmberOfRageMissile = 257298
        };

        class spell_argus_ember_of_rage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_ember_of_rage_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                GetTarget()->CastSpell(GetTarget(), eSpell::SpellEmberOfRageMissile, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_ember_of_rage_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_ember_of_rage_AuraScript();
        }
};

/// Initialization Sequence - 256388
/// Initialization Sequence - 258029
class spell_argus_initialization_sequence : public SpellScriptLoader
{
    public:
        spell_argus_initialization_sequence() : SpellScriptLoader("spell_argus_initialization_sequence") { }

        enum eSpell
        {
            SpellInitializationSequenceDamage = 256397
        };

        class spell_argus_initialization_sequence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_initialization_sequence_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Module = GetTarget();
                if (!l_Module)
                    return;

                int32 l_BP = l_Module->GetMap()->IsLFR() ? 4 : 5;

                l_Module->CastCustomSpell(l_Module, eSpell::SpellInitializationSequenceDamage, &l_BP, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_initialization_sequence_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_initialization_sequence_AuraScript();
        }
};

/// Titanforging - 257214
class spell_argus_titanforging : public SpellScriptLoader
{
    public:
        spell_argus_titanforging() : SpellScriptLoader("spell_argus_titanforging") { }

        enum eSpell
        {
            SpellTitanforged = 257215
        };

        class spell_argus_titanforging_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_titanforging_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->CastSpell(l_Caster, eSpell::SpellTitanforged, true);
                    l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, -1, Powers::POWER_ENERGY);

                    if (l_Caster->GetPower(Powers::POWER_ENERGY) <= 0)
                        l_Caster->RemoveAura(GetSpellInfo()->Id);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_titanforging_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_titanforging_AuraScript();
        }
};

/// Sentence of Sargeras - 257966
class spell_argus_sentence_of_sargeras : public SpellScriptLoader
{
    public:
        spell_argus_sentence_of_sargeras() : SpellScriptLoader("spell_argus_sentence_of_sargeras") { }

        enum eSpell
        {
            SpellShatteredBonds = 258000
        };

        class spell_argus_sentence_of_sargeras_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_argus_sentence_of_sargeras_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        constexpr float l_Distance = 35.0f;

                        if (l_Target->GetExactDist(l_Caster) >= l_Distance)
                            p_AurEff->GetBase()->Remove(AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL);
                    }
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE && l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->ToCreature())
                        l_Caster->ToCreature()->DespawnOrUnsummon();

                    if (Unit* l_Target = GetTarget())
                        l_Target->CastSpell(l_Target, eSpell::SpellShatteredBonds, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_argus_sentence_of_sargeras_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_argus_sentence_of_sargeras_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_argus_sentence_of_sargeras_AuraScript();
        }
};

/// Gift of the Sea - 257306
/// Gift of the Sky - 257313
class areatrigger_argus_gifts_of_golganneth : public AreaTriggerEntityScript
{
    public:
        areatrigger_argus_gifts_of_golganneth() : AreaTriggerEntityScript("areatrigger_argus_gifts_of_golganneth") { }

        std::set<uint64> m_AffectedTargets;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (m_AffectedTargets.find(p_Target->GetGUID()) != m_AffectedTargets.end())
                return true;

            m_AffectedTargets.insert(p_Target->GetGUID());
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_argus_gifts_of_golganneth();
        }
};

/// Edge of Obliteration Warning Vis - 252136
class areatrigger_argus_edge_of_obliteration : public AreaTriggerEntityScript
{
    public:
        areatrigger_argus_edge_of_obliteration() : AreaTriggerEntityScript("areatrigger_argus_edge_of_obliteration") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            /// Don't calculate movement
            /*p_AreaTrigger->SetDuration(6 * TimeConstants::IN_MILLISECONDS);

            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET, 0);
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_DURATION, 6 * TimeConstants::IN_MILLISECONDS);
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 6 * TimeConstants::IN_MILLISECONDS);
            p_AreaTrigger->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 129604);

            p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, 100.1012f);*/
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_argus_edge_of_obliteration();
        }
};

/// Gift of the Lifebinder - 256338
class areatrigger_argus_gift_of_the_lifebinder : public AreaTriggerEntityScript
{
    public:
        areatrigger_argus_gift_of_the_lifebinder() : AreaTriggerEntityScript("areatrigger_argus_gift_of_the_lifebinder") { }

        enum eSpell
        {
            SpellWitheringRoots = 256399
        };

        enum eVisual
        {
            VisualWitheredDisplay = 81337
        };

        float m_EnergyCost = 0.0f;
        float m_EnergyMod = 0.0f;

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetCanSearchDead(true);

            p_AreaTrigger->m_serverSideVisibility.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_ALIVE | GhostVisibilityType::GHOST_VISIBILITY_GHOST);
            p_AreaTrigger->m_serverSideVisibilityDetect.SetValue(ServerSideVisibilityType::SERVERSIDE_VISIBILITY_GHOST, GhostVisibilityType::GHOST_VISIBILITY_GHOST);

            switch (p_AreaTrigger->GetMap()->GetDifficultyID())
            {
                case Difficulty::DifficultyRaidNormal:
                case Difficulty::DifficultyRaidLFR:
                {
                    m_EnergyCost = 2.25f;
                    break;
                }
                case Difficulty::DifficultyRaidHeroic:
                {
                    m_EnergyCost = 3.25f;
                    break;
                }
                case Difficulty::DifficultyRaidMythic:
                {
                    m_EnergyCost = 4.25f;
                    break;
                }
                default:
                    break;
            }
        }

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            if (p_Target->isAlive() || !p_Target->IsPlayer())
                return true;

            m_EnergyMod += m_EnergyCost;

            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                p_Target->ToPlayer()->ResurrectPlayer(100.0f);

                if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                {
                    if (Creature* l_Argus = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::BossArgusTheUnmaker)))
                        l_Argus->AddThreat(p_Target, 10.0f);
                }

                if (Corpse* l_Corpse = p_Target->ToPlayer()->GetCorpse())
                    p_Target->GetMap()->RemoveCorpse(l_Corpse);

                int32 l_Value = int32(m_EnergyMod);

                m_EnergyMod -= l_Value;

                if (l_Value <= l_Caster->GetPower(Powers::POWER_ENERGY))
                    l_Caster->ModifyPower(Powers::POWER_ENERGY, -l_Value);
                else
                {
                    if (l_Caster->ToCreature() && l_Caster->GetEntry() == eCreatures::NpcGiftOfTheLifebinder)
                    {
                        l_Caster->ToCreature()->SetEntry(eCreatures::NpcWitheredGiftOfTheLifebinder);
                        l_Caster->SetDisplayId(eVisual::VisualWitheredDisplay);
                    }

                    l_Caster->SetPower(Powers::POWER_ENERGY, 0);
                    l_Caster->CastSpell(l_Caster, eSpell::SpellWitheringRoots, true);
                }
            }

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_argus_gift_of_the_lifebinder();
        }
};

/// Sargeras' Rage - 257869
class areatrigger_argus_sargeras_rage : public AreaTriggerEntityScript
{
    public:
        areatrigger_argus_sargeras_rage() : AreaTriggerEntityScript("areatrigger_argus_sargeras_rage") { }

        enum eSpell
        {
            SpellUnleashedRage = 257911
        };

        std::map<uint64, uint32> m_Targets;

        const uint32 m_MaxTimerValue = 1 * TimeConstants::IN_MILLISECONDS;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (!l_Caster || p_Target == l_Caster)
                return true;

            m_Targets.insert(std::pair<uint64, uint32>(p_Target->GetGUID(), m_MaxTimerValue));
            return true;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (!l_Caster || p_Target == l_Caster)
                return true;

            m_Targets.erase(p_Target->GetGUID());
            return true;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Diff) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (l_Caster == nullptr || m_Targets.empty())
                return;

            for (auto& l_Pair : m_Targets)
            {
                if (l_Pair.second <= p_Diff)
                {
                    l_Pair.second = m_MaxTimerValue;

                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, l_Pair.first))
                        l_Caster->CastSpell(l_Unit, eSpell::SpellUnleashedRage, true);
                }
                else
                    l_Pair.second -= p_Diff;
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_argus_sargeras_rage();
        }
};

/// Sargeras' Fear - 257931
class areatrigger_argus_sargeras_fear : public AreaTriggerEntityScript
{
    public:
        areatrigger_argus_sargeras_fear() : AreaTriggerEntityScript("areatrigger_argus_sargeras_fear") { }

        enum eSpells
        {
            SpellCrushingFear   = 257930,
            SargerasFearVisual  = 258739
        };

        uint32 m_UpdateTime = 1 * TimeConstants::IN_MILLISECONDS;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Diff) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (m_UpdateTime && l_Caster)
            {
                if (m_UpdateTime <= p_Diff)
                {
                    m_UpdateTime = 1 * TimeConstants::IN_MILLISECONDS;

                    if (l_Caster->FindNearestPlayer(p_AreaTrigger->GetRadius()))
                    {
                        l_Caster->RemoveAura(eSpells::SargerasFearVisual);
                        return;
                    }

                    l_Caster->CastSpell(l_Caster, eSpells::SargerasFearVisual, true);
                    l_Caster->CastSpell(l_Caster, eSpells::SpellCrushingFear, true);
                }
                else
                    m_UpdateTime -= p_Diff;
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_argus_sargeras_fear();
        }
};

/// Rent Soul - 258836
class areatrigger_argus_rent_soul : public AreaTriggerEntityScript
{
    public:
        areatrigger_argus_rent_soul() : AreaTriggerEntityScript("areatrigger_argus_rent_soul") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                if (!l_Instance)
                    return;

                Creature* l_Argus = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::BossArgusTheUnmaker));
                if (!l_Argus || !l_Argus->IsAIEnabled)
                    return;

                if (Player* l_Player = l_Argus->AI()->SelectPlayerTarget(UnitAI::eTargetTypeMask::TypeMaskAll))
                {
                    p_AreaTrigger->SetMoveType(AreaTriggerMoveType::AT_MOVE_TYPE_TO_TARGET);
                    p_AreaTrigger->SetTargetGuid(l_Player->GetGUID());
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_argus_rent_soul();
        }
};

/// Spoils of the Pantheon - 277355
class go_argus_spoils_of_the_pantheon : public GameObjectScript
{
    public:
        go_argus_spoils_of_the_pantheon() : GameObjectScript("go_argus_spoils_of_the_pantheon") { }

        struct go_argus_spoils_of_the_pantheonAI : public GameObjectAI
        {
            go_argus_spoils_of_the_pantheonAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            void HandleCustomLoot(Player* p_Player) override
            {
                /// Mount Shackled Ur'zul only drops in Mythic mode
                if (!go->GetMap()->IsMythic())
                    return;

                GroupPtr l_Group = p_Player->GetGroup();

                LootStoreItem l_Item = LootStoreItem(ePantheonItems::ItemShackledUrzul, LootItemType::LOOT_ITEM_TYPE_ITEM, 100.0f, LootModes::LOOT_MODE_DEFAULT, LootModes::LOOT_MODE_DEFAULT, 1, 1, std::vector<uint32>(), 0, 0, 0);

                Loot* l_Loot = nullptr;

                /// Group is on Master Loot, add one mount for the group, leader will assign it himself
                if (l_Group && l_Group->GetLootMethod() == LootMethod::MASTER_LOOT)
                {
                    if (go->m_LootContainers.find(l_Group->GetGUID()) == go->m_LootContainers.end())
                        return;

                    l_Loot = &go->m_LootContainers[l_Group->GetGUID()];

                    l_Loot->AddItem(l_Item, p_Player);
                }
                /// Group is on personal loot, add one mount for one random player who didn't already drop it
                else
                {
                    std::list<Player*> l_PlayerList;

                    Map::PlayerList const& l_Players = go->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                    {
                        if (Player* l_Player = l_Itr->getSource())
                        {
                            if (l_Player->HasItemCount(ePantheonItems::ItemShackledUrzul) || l_Player->HasSpell(eSharedSpells::SpellShackledUrzul))
                                continue;

                            l_PlayerList.push_back(l_Player);
                        }
                    }

                    JadeCore::RandomResizeList(l_PlayerList, 1);

                    for (Player* l_Player : l_PlayerList)
                    {
                        if (go->m_LootContainers.find(l_Player->GetGUID()) == go->m_LootContainers.end())
                            return;

                        l_Loot = &go->m_LootContainers[l_Player->GetGUID()];

                        l_Loot->AddItem(l_Item, l_Player);
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_argus_spoils_of_the_pantheonAI(p_GameObject);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_argus_the_unmaker()
{
    /// Boss
    new boss_argus_the_unmaker();

    /// Creatures
    new npc_argus_amanthul();
    new npc_argus_golganneth();
    new npc_argus_aggramar();
    new npc_argus_norgannon();
    new npc_argus_eonar();
    new npc_argus_khaz_goroth();
    new npc_argus_soulblight_orb();
    new npc_argus_edge_of_obliteration();
    new npc_argus_constellar_designate();
    new npc_argus_gift_of_the_lifebinder();
    new npc_argus_reorigination_module();
    new npc_argus_hungering_soul();
    new npc_argus_mote_of_titanic_power();
    new npc_argus_apocalypsis_module();
    new npc_argus_chains_of_sargeras();
    new npc_argus_edge_of_annihilation();

    /// Spells
    new spell_argus_argus_p1_energy_controller();
    new spell_argus_argus_p2_energy_controller();
    new spell_argus_argus_p4_energy_controller();
    new spell_argus_cone_of_death_at();
    new spell_argus_soulblight_orb();
    new spell_argus_soulblight();
    new spell_argus_gift_of_the_sky();
    new spell_argus_gift_of_the_sea();
    new spell_argus_soulburst();
    new spell_argus_soulburst_detonation();
    new spell_argus_soulbomb();
    new spell_argus_soulbomb_detonation();
    new spell_argus_aggramars_boon();
    new spell_argus_cosmic_ray();
    new spell_argus_cosmic_ray_damage();
    new spell_argus_cosmic_beacon();
    new spell_argus_grasp_of_the_unmaker();
    new spell_argus_ember_of_rage();
    new spell_argus_initialization_sequence();
    new spell_argus_titanforging();
    new spell_argus_sentence_of_sargeras();

    /// AreaTriggers
    new areatrigger_argus_gifts_of_golganneth();
    new areatrigger_argus_edge_of_obliteration();
    new areatrigger_argus_gift_of_the_lifebinder();
    new areatrigger_argus_sargeras_rage();
    new areatrigger_argus_sargeras_fear();
    new areatrigger_argus_rent_soul();

    /// GameObject
    new go_argus_spoils_of_the_pantheon();
}
#endif
