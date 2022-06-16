////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eGuldanData
{
    /// Counts
    MaxQuarters     = 4,
    MaxBonds        = 2,
    MaxFelScythes   = 3,
    MaxLightsHeart  = 4,
    MaxFlames       = 3,
    MaxStormVisual1 = 20,
    MaxStormVisual2 = 80,
    MaxNightorbs    = 4,
    MaxVisionsDummy = 8,
    MaxVisionsVisu  = 15,

    /// Script data
    BondsOfFelGuid  = 0,
    PlayMovieSpeed  = 2693,
    EmpoweredBonds  = 11051
};

std::array<Position, eGuldanData::MaxQuarters> g_AmanthulTriggers =
{
    {
        { 245.375f, 3098.60f, 465.626f, 4.01533f },
        { 319.194f, 3098.54f, 465.626f, 5.51095f },
        { 319.219f, 3172.23f, 465.626f, 0.81190f },
        { 245.479f, 3172.29f, 465.626f, 2.41422f }
    }
};

std::array<uint32, eGuldanData::MaxQuarters> g_EnergyConduits =
{
    {
        eGameObjects::GoEnergyConduitSW,
        eGameObjects::GoEnergyConduitNW,
        eGameObjects::GoEnergyConduitSE,
        eGameObjects::GoEnergyConduitNE
    }
};

std::array<uint32, eGuldanData::MaxQuarters> g_FocusingStatues =
{
    {
        eGameObjects::GoFocusingStatueNE,
        eGameObjects::GoFocusingStatueNW,
        eGameObjects::GoFocusingStatueSE,
        eGameObjects::GoFocusingStatueSW
    }
};

Position const g_CosmeticOrbPos = { 282.1979f, 3135.806f, 465.8505f, 0.0f };

Position const g_MovieMovePos = { 313.7256f, 3139.089f, 475.99f, 0.0f };

Position const g_WellOfSoulsMove = { 277.2511f, 3121.487f, 465.8915f, 1.21737f };

Position const g_StormOfTheDestroyerSrc = { 237.3836f, 3147.522f, 465.85f, 0.0f };

Position const g_TheDemonWithinPos = { 282.4097f, 3135.275f, 466.0295f, 6.18194f };

/// Gul'Dan <Lord of the Shadow Council> - 104154
class boss_guldan : public CreatureScript
{
    public:
        boss_guldan() : CreatureScript("boss_guldan") { }

        enum eSpells
        {
            /// Misc
            SpellTheEyeOfAmanThulCosmeticChannel    = 227743,
            SpellTheEyeOfAmanThulFightCosmeticChan  = 227745,
            SpellTheEyeOfAmanThulCosmeticAT         = 227433,
            SpellTheEyeOfAmanThulCosmeticAT2        = 227694,
            SpellTheEyeOfAmanThulCosmeticAT3        = 227695,
            SpellTheEyeOfAmanThulCosmeticAT4        = 227696,
            SpellTheEyeOfAmanThulImmunityP1         = 206516,
            SpellTheEyeOfAmanThulImmunityP2ToP3     = 227427,
            SpellTheEyeOfAmanThulTeleportP2         = 227489,
            SpellTheEyeOfAmanThulTransP3AT1         = 227751,
            SpellTheEyeOfAmanThulTransP3AT2         = 227752,
            SpellTheEyeOfAmanThulTransP3AT3         = 227753,
            SpellTheEyeOfAmanThulTransP3AT4         = 227754,
            SpellTheEyeOfAmanThulTransP3ATVisual    = 232848,
            SpellTheEyeOfAmanThulTransitionToP3     = 227426,
            SpellTheEyeOfAmanThulP2                 = 206508,
            SpellGuldanMythicAura                   = 227639,
            SpellTheEyeOfAmanThulMythicAura         = 227320,
            SpellFelLordFadeOut                     = 209600,
            SpellJailerFadeOut                      = 209593,
            SpellSoulVortexAT                       = 206885,
            SpellREUSEAT                            = 151120,
            SpellPlayMovieAT                        = 228642,
            SpellRaidGuldanOutro                    = 234147,
            SpellParasiticWoundAura                 = 212294,
            SpellGuldanMythicDeathFadeOut           = 206500,
            SpellSoulseverSoulAT                    = 211109,
            SpellTornSoulDoT                        = 206896,
            SpellBulwarkOfAzzinothAura              = 221299,
            SpellVisionsScreenEffect                = 227032,
            /// Player extra action button
            SpellTimeDilationOverrider              = 210345,
            SpellScatteringFieldOverrider           = 210332,
            SpellScatteringFieldAbsorb              = 206985,
            SpellResonantBarrierOverrider           = 210336,
            /// Phase 1
            /// Liquid Hellfire
            SpellLiquidHellfireDummy                = 206204,
            SpellLiquidHellfireCast                 = 206219,
            SpellLiquidHellfireSummon               = 206561,
            SpellEmpoweredLiquidHellfire            = 206220,
            SpellEmpoweredLiquidHellfireAT          = 206581,
            /// Fel Efflux
            SpellFelEffluxDummy                     = 206513,
            SpellFelEffluxTrigger                   = 206525,
            SpellFelEffluxAura                      = 206514,
            /// Hand of Gul'dan
            SpellHandOfGuldanDummy                  = 212256,
            SpellHandOfGuldanCast                   = 212258,
            SpellHandOfGuldanDamage                 = 212262,
            SpellHandOfGuldanSummonKurazmal         = 215736,
            SpellHandOfGuldanSummonVethriz          = 215738,
            SpellHandOfGuldanSummonDzorykx          = 215739,
            SpellHandOfGuldanSummonAzagrim          = 209126,
            SpellHandOfGuldanSummonBeltheris        = 212721,
            SpellHandOfGuldanSummonDalvengyr        = 212722,
            /// Phase 2
            /// Bonds of Fel
            SpellBondsOfFelCast                     = 206222,
            SpellBondsOfFelMissile                  = 206330,
            SpellBondsOfFelAura                     = 209011,
            SpellBondsOfFelAT                       = 206354,
            SpellEmpoweredBondsOfFelCast            = 206221,
            SpellEmpoweredBondsOfFelMissile         = 209029,
            SpellEmpoweredBondsOfFelKnock           = 206366,
            SpellEmpoweredBondsOfFelAura            = 206384,
            SpellEmpoweredBondsOfFelAT              = 209086,
            /// Eye of Gul'dan
            SpellEyeOfGuldan                        = 206226,
            SpellEyeOfGuldanTrigger                 = 209277,
            SpellEyeOfGuldanCast                    = 209270,
            SpellEmpoweredEyeOfGuldanCast           = 211152,
            SpellEmpoweredEyeOfGuldanAura           = 221728,
            /// Fel Scythe
            SpellFelScytheEnergizer                 = 227554,
            SpellFelScythePowerIncrease             = 227557,
            SpellFelScytheSearcher                  = 227551,
            SpellFelScytheSearcher2                 = 228163,
            SpellFelScytheDamage                    = 227550,
            SpellFuryOfTheFel                       = 227556,
            /// Phase 3
            SpellGuldanWindAT                       = 211101,
            SpellWellOfSouls                        = 206939,
            SpellWellOfSoulsDoT                     = 208536,
            /// Flames of Sargeras
            SpellFlamesOfSargerasCast               = 221783,
            SpellFlamesOfSargerasTwoTargets         = 212686,
            SpellFlamesOfSargerasThreeTargets       = 221784,
            SpellFlamesOfSargerasFourTargets        = 221785,
            SpellFlamesOfSargerasAura               = 221606,
            SpellFlamesOfSargerasPeriodic           = 221603,
            SpellFlamesOfSargerasAT                 = 221605,
            /// Soul Siphon
            SpellSoulSiphonPeriodic                 = 217766,
            SpellSoulSiphonSearcher                 = 206446,
            SpellSoulSiphonSummon                   = 206393,
            SpellSoulSiphonAura                     = 221891,
            SpellSoulCorrosionAura                  = 208802,
            /// Black Harvest
            SpellBlackHarvestSearcher               = 208806,
            SpellBlackHarvestCast                   = 206744,
            /// Storm of the Destroyer
            SpellStormOfTheDestroyerAura            = 161121,
            SpellStormOfTheDestroyerFirst           = 167819,
            SpellStormOfTheDestroyerSecond          = 167935,
            SpellStormOfTheDestroyerThird           = 177380,
            SpellStormOfTheDestroyerFourth          = 152987,
            SpellGuldanStormCosmetic                = 227682,
            SpellREUSECosmetic                      = 179300,
            SpellPhaseBurst                         = 32939,
            SpellDemonicCircleSearcher              = 227732,
            SpellDemonicCircleTriggered             = 227726
        };

        enum eEvents
        {
            /// Phase 1
            EventLiquidHellfire = 1,
            EventHandOfGuldan,
            EventFelEfflux,
            /// Phase 2
            EventBondsOfFel,
            EventEyeOfGuldan,
            /// Phase 3
            EventViolentWinds,
            EventFlamesOfSargeras,
            EventBlackHarvest,
            EventStormOfTheDestroyer,
            EventSoulSiphon,

            EventBerserk
        };

        enum ePhases
        {
            Phase1TheCouncilOfElders,
            Phase2TheRitualOfAmanThul,
            Phase3TheMastersPower,
            Phase4TheDemonWithin
        };

        enum eTalks
        {
            TalkIntro1,
            TalkIntro2Alliance1,
            TalkIntro2Alliance2,
            TalkIntro2Horde1,
            TalkIntro2Horde2,
            TalkIntro3,
            TalkIntro4,
            TalkAggro,
            TalkWipe,
            TalkLiquidHellfire,
            TalkLiquidHellfireWarn,
            TalkFelEffluxAndFlamesOfSargeras,
            TalkHandOfGuldan,
            TalkLockedWithinTheEye,
            TalkP1End,
            TalkP2Start,
            TalkBondsOfFel,
            TalkEyeOfGuldan,
            TalkEmpoweredBondsOfFel,
            TalkEmpoweredLiquidHellfire,
            TalkEmpoweredEyeOfGuldan,
            TalkKillPlayer,
            TalkEmpoweredBondsOfFelWarn,
            TalkEmpoweredLiquidHellfireWarn,
            TalkEmpoweredEyeOfGuldanWarn,
            TalkAfflictedByEmpoweredBondsOfFel,
            TalkAfflictedByBondsOfFel,
            TalkP2End,
            TalkP3Start,
            TalkBlackHarvest,
            TalkStormOfTheDestroyer,
            TalkHeroicDreadlord,
            TalkMythicAggro,
            TalkMythicDeath
        };

        enum eActions
        {
            /// Khadgar
            ActionTriggerKhadgar,
            ActionResetKhadgar,
            ActionTalk20Percents,
            ActionTalk10Percents,
            ActionEndFight,
            /// Gul'dan
            ActionCosmeticTrigger = 0,
            ActionIntroduction,
            ActionFelScythe
        };

        enum eVisuals
        {
            VisualPlatformActivation    = 11839,
            VisualPlatformDeactivation  = 11924,
            VisualPlatformCircleStopped = 12327,
            VisualPlatformCircleFinish  = 12329,

            VisualEyeActivation1        = 12316,
            VisualEyeActivation2        = 10596,

            VisualEyeRuneActivation1    = 12326,
            VisualEyeRuneActivation2    = 12325,

            VisualFocusingStatueChannel = 10415,

            VisualConduitActivated      = 12319,
            VisualConduitActivated2     = 12321,
            VisualConduitActivated3     = 12320,

            VisualStatueChanneling      = 10415,
            VisualStatueRotating        = 10418,
            VisualStatueRotated         = 10419,

            VisualWorldOpenedSouth      = 10686,

            VisualToIllidanPrison1      = 61560,
            VisualToIllidanPrison2      = 61561,

            VisualRoomIntact            = 35766,
            VisualRoomDestroyed         = 35767,

            VisualStormOfTheDestroyer1  = 61442,
            VisualStormOfTheDestroyer2  = 55649,
            VisualStormOfTheDestroyer3  = 55677,
            VisualStormOfTheDestroyer4  = 55683,
            VisualStormOfTheDestroyer5  = 61453
        };

        struct boss_guldanAI : BossAI
        {
            boss_guldanAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataGuldan) { }

            EventMap m_EnrageEvent;

            bool m_IntroDone = false;
            bool m_IntroHorde = false;
            uint32 m_IntroCheckTimer = 0;

            uint8 m_Phase = ePhases::Phase1TheCouncilOfElders;

            uint8 m_EnergizeState = 0;

            uint8 m_LiquidHellfireCount = 1;
            uint8 m_HandOfGuldanCount = 1;
            uint8 m_HandOfGuldanBossIDx = 0;
            uint8 m_HandOfGuldanBossKilled = 0;

            uint64 m_CosmeticTrigger = 0;
            uint64 m_LiquidHellfireGuid = 0;

            uint64 m_LosBlockerGuid = 0;

            uint8 m_EyeOfGuldanCount = 1;

            bool m_FelEfflux = false;

            bool m_EmpoweredBondsOfFel = false;
            bool m_EmpoweredLiquidHellfire = false;
            bool m_EmpoweredEyeOfGuldan = false;

            std::map<uint32, std::array<uint64, eGuldanData::MaxBonds>> m_BondsOfFelGuids;

            uint32 m_FelScytheTargetCount = 1;

            uint8 m_FlamesOfSargerasIDx = 0;
            uint8 m_FlamesOfSargerasCount = 1;
            uint8 m_SoulSiphonCount = 1;
            uint8 m_BlackHarvestCount = 0;
            uint8 m_StormCount = 0;

            bool m_20PercentsWarn = false;
            bool m_10PercentsWarn = false;

            bool m_LockedInEye = false;

            uint64 m_LastRegisteredForceID = 0;
            std::map<uint32, std::queue<uint64>> m_ForcedMovements;

            bool m_FirstViolentWinds = true;

            uint32 m_CheckPlayersTimer = 0;

            bool m_FeignDeath = false;

            void Reset() override
            {
                /// Prevent double call
                if (me->IsInEvadeMode())
                    return;

                _Reset();

                m_EnrageEvent.Reset();

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_FEIGN_DEATH);

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                m_Phase = ePhases::Phase1TheCouncilOfElders;

                m_EnergizeState = 0;

                m_HandOfGuldanCount = 1;

                m_CosmeticTrigger = 0;

                m_FelEfflux = false;

                m_EmpoweredBondsOfFel = false;
                m_EmpoweredLiquidHellfire = false;
                m_EmpoweredEyeOfGuldan = false;

                m_FelScytheTargetCount = 1;

                m_20PercentsWarn = false;
                m_10PercentsWarn = false;

                m_LockedInEye = false;

                m_FirstViolentWinds = true;

                m_CheckPlayersTimer = 0;

                m_FeignDeath = false;

                me->RemoveAura(eSharedSpells::SpellBerserkGuldan);

                me->SetPower(me->getPowerType(), 0);

                if (instance)
                    instance->DoUpdateWorldState(eGuldanData::EmpoweredBonds, 0);

                summons.DespawnAll();

                if (!IsMythic())
                {
                    DoCast(me, eSpells::SpellTheEyeOfAmanThulCosmeticChannel, true);

                    if (Creature* l_Trigger = me->SummonCreature(eCreatures::NpcEyeOfAmanThulTrigger, g_AmanthulTriggers[2]))
                        m_CosmeticTrigger = l_Trigger->GetGUID();

                    SetCombatMovement(false);

                    if (instance)
                    {
                        for (uint32 const& l_Entry : g_EnergyConduits)
                        {
                            if (GameObject* l_Conduit = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                                l_Conduit->SendGameObjectActivateAnimKit(eVisuals::VisualConduitActivated);
                        }

                        for (uint32 const& l_Entry : g_FocusingStatues)
                        {
                            if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                            {
                                switch (l_Entry)
                                {
                                    /// Facing center - Energizing
                                    case eGameObjects::GoFocusingStatueNW:
                                    {
                                        l_Statue->SendGameObjectActivateAnimKit(0);
                                        break;
                                    }
                                    /// Facing outside, doing nothing
                                    case eGameObjects::GoFocusingStatueNE:
                                    case eGameObjects::GoFocusingStatueSE:
                                    case eGameObjects::GoFocusingStatueSW:
                                    {
                                        if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                                            l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueRotated);

                                        break;
                                    }
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    m_Phase = ePhases::Phase2TheRitualOfAmanThul;

                    SetFlyMode(true);

                    SetCombatMovement(true);

                    /// On Mythic difficulty, Gul'dan will already be inside the Eye of Aman'thul and you can engage him immediately.
                    m_IntroDone = true;

                    DoCast(me, eSpells::SpellGuldanMythicAura, true);
                    DoCast(me, eSpells::SpellTheEyeOfAmanThulMythicAura, true);
                    DoCast(me, eSpells::SpellFelScytheEnergizer, true);
                    DoCast(me, eSpells::SpellParasiticWoundAura, true);

                    if (instance)
                    {
                        for (uint32 const& l_Entry : g_EnergyConduits)
                        {
                            if (GameObject* l_Conduit = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                                l_Conduit->SendGameObjectActivateAnimKit(eVisuals::VisualConduitActivated);
                        }

                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                            l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformActivation);

                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                            l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeActivation1);

                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorRune)))
                            l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeRuneActivation1);
                    }
                }

                if (!m_IntroDone)
                {
                    m_IntroCheckTimer = 500;

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                    me->SetReactState(ReactStates::REACT_PASSIVE);
                }

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return IsMythic() && !me->isInCombat();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                m_CheckPlayersTimer = 1 * TimeConstants::IN_MILLISECONDS;

                if (IsMythic())
                {
                    me->ExitVehicle();
                    me->NearTeleportTo(me->GetHomePosition());

                    SetFlyMode(false);

                    DoCast(me, eSpells::SpellTheEyeOfAmanThulP2, true);

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanRoom)))
                        {
                            l_Gob->SetDisplayId(eVisuals::VisualRoomDestroyed);
                            l_Gob->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DESTROYED);
                        }

                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                        {
                            l_Gob->SendGameObjectActivateAnimKit(0);
                            l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformDeactivation);
                        }

                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                            l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeActivation2);
                    });

                    Talk(eTalks::TalkMythicAggro);
                }
                else
                    Talk(eTalks::TalkAggro);

                _EnterCombat();

                if (!IsMythic())
                    m_EnergizeState = 1;

                me->RemoveAura(eSpells::SpellTheEyeOfAmanThulCosmeticChannel);
                me->RemoveAura(eSpells::SpellTheEyeOfAmanThulMythicAura);

                DoCast(me, eSpells::SpellTheEyeOfAmanThulFightCosmeticChan, true);

                if (!IsMythic())
                    summons.DespawnAll();

                std::list<Player*> l_PlayerList;

                me->GetPlayerListInGrid(l_PlayerList, 75.0f);

                for (Player* l_Player : l_PlayerList)
                {
                    switch (l_Player->GetRoleForGroup())
                    {
                        case Roles::ROLE_DAMAGE:
                        {
                            l_Player->CastSpell(l_Player, eSpells::SpellTimeDilationOverrider, true);
                            break;
                        }
                        case Roles::ROLE_HEALER:
                        {
                            l_Player->CastSpell(l_Player, eSpells::SpellScatteringFieldOverrider, true);
                            break;
                        }
                        case Roles::ROLE_TANK:
                        {
                            l_Player->CastSpell(l_Player, eSpells::SpellResonantBarrierOverrider, true);
                            break;
                        }
                        default:
                            break;
                    }
                }

                m_EnrageEvent.Reset();

                DefaultEvents();

                if (instance && !IsMythic())
                {
                    if (Creature* l_Trigger = instance->instance->GetCreature(m_CosmeticTrigger))
                        l_Trigger->DespawnOrUnsummon();

                    if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueNW)))
                        l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueChanneling);

                    if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueNE)))
                        l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueRotating);

                    /// Energizing
                    AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Creature* l_Trigger = me->SummonCreature(eCreatures::NpcEyeOfAmanThulTrigger, g_AmanthulTriggers[1]))
                            m_CosmeticTrigger = l_Trigger->GetGUID();

                        if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueNE)))
                            l_Statue->SendGameObjectActivateAnimKit(0);
                    });
                }
            }

            void DefaultEvents()
            {
                switch (m_Phase)
                {
                    case ePhases::Phase1TheCouncilOfElders:
                    {
                        m_LiquidHellfireCount = 1;
                        m_HandOfGuldanCount = 1;
                        m_HandOfGuldanBossIDx = 0;
                        m_HandOfGuldanBossKilled = 0;

                        events.ScheduleEvent(eEvents::EventLiquidHellfire, 2 * TimeConstants::IN_MILLISECONDS);

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventHandOfGuldan, 16 * TimeConstants::IN_MILLISECONDS + 600);
                        else
                            events.ScheduleEvent(eEvents::EventHandOfGuldan, 7 * TimeConstants::IN_MILLISECONDS);

                        events.ScheduleEvent(eEvents::EventFelEfflux, 11 * TimeConstants::IN_MILLISECONDS);

                        if (!IsMythic())
                            m_EnrageEvent.ScheduleEvent(eEvents::EventBerserk, 720 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case ePhases::Phase2TheRitualOfAmanThul:
                    {
                        m_LiquidHellfireCount = 1;
                        m_EyeOfGuldanCount = 1;

                        m_EmpoweredBondsOfFel = false;
                        m_EmpoweredLiquidHellfire = false;
                        m_EmpoweredEyeOfGuldan = false;

                        m_FelEfflux = false;

                        if (IsLFR() || IsNormal())
                        {
                            events.ScheduleEvent(eEvents::EventLiquidHellfire, 45 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventBondsOfFel, 7 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventEyeOfGuldan, 32 * TimeConstants::IN_MILLISECONDS + 400);
                        }
                        else if (IsHeroic())
                        {
                            events.ScheduleEvent(eEvents::EventLiquidHellfire, 23 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventHandOfGuldan, 13 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventBondsOfFel, 9 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventEyeOfGuldan, 29 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventLiquidHellfire, 36 * TimeConstants::IN_MILLISECONDS + 600);
                            events.ScheduleEvent(eEvents::EventHandOfGuldan, 16 * TimeConstants::IN_MILLISECONDS + 600);
                            events.ScheduleEvent(eEvents::EventBondsOfFel, 6 * TimeConstants::IN_MILLISECONDS + 600);
                            events.ScheduleEvent(eEvents::EventEyeOfGuldan, 26 * TimeConstants::IN_MILLISECONDS + 600);
                        }

                        break;
                    }
                    case ePhases::Phase3TheMastersPower:
                    {
                        m_EyeOfGuldanCount = 1;
                        m_FlamesOfSargerasIDx = 0;
                        m_FlamesOfSargerasCount = 1;
                        m_SoulSiphonCount = 1;
                        m_BlackHarvestCount = 0;
                        m_StormCount = 0;

                        if (IsMythic())
                        {
                            events.ScheduleEvent(eEvents::EventFlamesOfSargeras, 24 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventEyeOfGuldan, 35 * TimeConstants::IN_MILLISECONDS + 100);
                            events.ScheduleEvent(eEvents::EventBlackHarvest, 55 * TimeConstants::IN_MILLISECONDS + 700);
                            events.ScheduleEvent(eEvents::EventStormOfTheDestroyer, 72 * TimeConstants::IN_MILLISECONDS + 600);
                        }
                        else if (IsHeroic())
                        {
                            events.ScheduleEvent(eEvents::EventFlamesOfSargeras, 27 * TimeConstants::IN_MILLISECONDS + 500);
                            events.ScheduleEvent(eEvents::EventEyeOfGuldan, 39 * TimeConstants::IN_MILLISECONDS + 100);
                            events.ScheduleEvent(eEvents::EventBlackHarvest, 64 * TimeConstants::IN_MILLISECONDS + 100);
                            events.ScheduleEvent(eEvents::EventStormOfTheDestroyer, 84 * TimeConstants::IN_MILLISECONDS + 100);
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventFlamesOfSargeras, 29 * TimeConstants::IN_MILLISECONDS + 300);
                            events.ScheduleEvent(eEvents::EventEyeOfGuldan, 42 * TimeConstants::IN_MILLISECONDS + 700);
                            events.ScheduleEvent(eEvents::EventBlackHarvest, 71 * TimeConstants::IN_MILLISECONDS + 200);
                            events.ScheduleEvent(eEvents::EventStormOfTheDestroyer, 94 * TimeConstants::IN_MILLISECONDS);
                        }

                        events.ScheduleEvent(eEvents::EventViolentWinds, 11 * TimeConstants::IN_MILLISECONDS + 500);

                        if (!IsLFR())
                            events.ScheduleEvent(eEvents::EventSoulSiphon, 19 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case ePhases::Phase4TheDemonWithin:
                    {
                        if (instance)
                            instance->instance->SummonCreature(eCreatures::NpcTheDemonWithin, g_TheDemonWithinPos);

                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (IsMythic())
                {
                    me->ExitVehicle();
                    me->NearTeleportTo(me->GetHomePosition());
                }

                BossAI::EnterEvadeMode();

                if (m_Phase != ePhases::Phase4TheDemonWithin)
                    Talk(eTalks::TalkWipe);

                me->InterruptNonMeleeSpells(true);

                me->RemoveAura(eSpells::SpellFelScytheEnergizer);
                me->RemoveAura(eSpells::SpellFelScythePowerIncrease);

                me->DespawnCreaturesInArea(eCreatures::NpcTheDemonWithin);

                ClearEncounterSpawns();

                summons.DespawnAll();

                ClearDelayedOperations();

                m_LastRegisteredForceID = 0;
                m_ForcedMovements.clear();

                if (instance)
                {
                    ClearEncounterAuras();

                    instance->DoRemoveForcedMovementsOnPlayers();

                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanRoom)))
                    {
                        l_Gob->SetDisplayId(eVisuals::VisualRoomIntact);
                        l_Gob->RemoveFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DESTROYED);
                    }

                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                        l_Gob->SendGameObjectActivateAnimKit(0);

                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                        l_Gob->SendGameObjectActivateAnimKit(0);

                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorRune)))
                        l_Gob->SendGameObjectActivateAnimKit(0);

                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulWorldS)))
                        l_Gob->SendGameObjectActivateAnimKit(0);

                    if (GameObject* l_Gob = instance->instance->GetGameObject(m_LosBlockerGuid))
                    {
                        m_LosBlockerGuid = 0;

                        l_Gob->Delete();
                    }

                    for (uint32 const& l_Entry : g_EnergyConduits)
                    {
                        if (GameObject* l_Conduit = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                            l_Conduit->SendGameObjectActivateAnimKit(0);
                    }

                    for (uint32 const& l_Entry : g_FocusingStatues)
                    {
                        switch (l_Entry)
                        {
                            case eGameObjects::GoFocusingStatueNE:
                            case eGameObjects::GoFocusingStatueSE:
                            case eGameObjects::GoFocusingStatueSW:
                            {
                                if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                                    l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueRotated);

                                break;
                            }
                            default:
                                break;
                        }
                    }

                    if (Creature* l_Khadgar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcArchmageKhadgar)))
                    {
                        if (l_Khadgar->IsAIEnabled)
                            l_Khadgar->AI()->DoAction(eActions::ActionResetKhadgar);
                    }
                }
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 5.8415f;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->ClearLootContainers();

                _JustDied();

                ClearEncounterSpawns();

                if (instance)
                {
                    DoCast(me, eSpells::SpellRaidGuldanOutro, true);

                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulWorldS)))
                        l_Gob->SendGameObjectActivateAnimKit(0);

                    if (GameObject* l_Gob = instance->instance->GetGameObject(m_LosBlockerGuid))
                    {
                        m_LosBlockerGuid = 0;

                        l_Gob->Delete();
                    }

                    ClearEncounterAuras();

                    for (uint32 const& l_Entry : g_EnergyConduits)
                    {
                        if (GameObject* l_Conduit = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                            l_Conduit->SendGameObjectActivateAnimKit(0);
                    }

                    for (uint32 const& l_Entry : g_FocusingStatues)
                    {
                        if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                            l_Statue->SendGameObjectActivateAnimKit(0);
                    }

                    if (Creature* l_Khadgar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcArchmageKhadgar)))
                    {
                        if (l_Khadgar->IsAIEnabled)
                            l_Khadgar->AI()->DoAction(eActions::ActionEndFight);
                    }
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (!p_Killed->IsPlayer())
                    return;

                Talk(eTalks::TalkKillPlayer);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                switch (m_Phase)
                {
                    case ePhases::Phase1TheCouncilOfElders:
                    {
                        if (!m_LockedInEye && me->HealthBelowPctDamaged(90.0f, p_Damage))
                        {
                            m_LockedInEye = true;

                            DoCast(me, eSpells::SpellTheEyeOfAmanThulImmunityP1, true);

                            Talk(eTalks::TalkLockedWithinTheEye);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformActivation);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeActivation1);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorRune)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeRuneActivation1);
                        }

                        break;
                    }
                    case ePhases::Phase2TheRitualOfAmanThul:
                    {
                        /// Allow Gul'dan to finish his current cast before changing phase
                        if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                            return;

                        /// Throughout the phase, Gul'dan will empower his abilities at certain health breakpoints.
                        /// At 85% he will empower his Bonds of Fel.
                        if (!m_EmpoweredBondsOfFel && me->HealthBelowPctDamaged(85.0f, p_Damage))
                        {
                            m_EmpoweredBondsOfFel = true;

                            Talk(eTalks::TalkEmpoweredBondsOfFel);
                            Talk(eTalks::TalkEmpoweredBondsOfFelWarn);

                            if (instance)
                                instance->DoUpdateWorldState(eGuldanData::EmpoweredBonds, 1);
                        }
                        /// At 70% he will empower his Liquid Hellfire
                        else if (!m_EmpoweredLiquidHellfire && me->HealthBelowPctDamaged(70.0f, p_Damage))
                        {
                            m_EmpoweredLiquidHellfire = true;

                            Talk(eTalks::TalkEmpoweredLiquidHellfire);
                            Talk(eTalks::TalkEmpoweredLiquidHellfireWarn);
                        }
                        /// At 55%, he will empower his Eye of Gul'dan.
                        else if (!m_EmpoweredEyeOfGuldan && me->HealthBelowPctDamaged(55.0f, p_Damage))
                        {
                            m_EmpoweredEyeOfGuldan = true;

                            Talk(eTalks::TalkEmpoweredEyeOfGuldan);
                            Talk(eTalks::TalkEmpoweredEyeOfGuldanWarn);
                        }
                        /// Once Gul'dan reaches 40% HP (50% on mythic), Phase Three will begin.
                        else if (me->HealthBelowPctDamaged(IsMythic() ? 50.0f : 40.0f, p_Damage) && instance)
                        {
                            me->InterruptNonMeleeSpells(true);

                            events.Reset();

                            Talk(eTalks::TalkP2End);

                            m_Phase = ePhases::Phase3TheMastersPower;

                            DoCast(me, eSpells::SpellTheEyeOfAmanThulTeleportP2, true);

                            DefaultEvents();

                            me->StopAttack();
                            me->ClearAllUnitState();
                            me->SetGuidValue(EUnitFields::UNIT_FIELD_TARGET, 0);
                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                            {
                                l_Gob->SendGameObjectActivateAnimKit(0);
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformActivation);
                            }

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeActivation1);

                            for (uint32 const& l_Entry : g_EnergyConduits)
                            {
                                if (GameObject* l_Conduit = GameObject::GetGameObject(*me, instance->GetData64(l_Entry)))
                                    l_Conduit->SendGameObjectActivateAnimKit(eVisuals::VisualConduitActivated3);
                            }
                        }

                        break;
                    }
                    case ePhases::Phase3TheMastersPower:
                    {
                        if (!instance)
                            break;

                        if (!m_20PercentsWarn && me->HealthBelowPctDamaged(20.0f, p_Damage))
                        {
                            if (Creature* l_Khadgar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcArchmageKhadgar)))
                            {
                                if (l_Khadgar->IsAIEnabled)
                                    l_Khadgar->AI()->DoAction(eActions::ActionTalk20Percents);
                            }

                            m_20PercentsWarn = true;

                            if (IsMythic())
                            {
                                if (Creature* l_Khadgar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcArchmageKhadgar)))
                                {
                                    if (l_Khadgar->IsAIEnabled)
                                        l_Khadgar->AI()->DoAction(eActions::ActionTriggerKhadgar);
                                }
                            }
                        }
                        else if (!m_10PercentsWarn && me->HealthBelowPctDamaged(10.0f, p_Damage))
                        {
                            if (Creature* l_Khadgar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcArchmageKhadgar)))
                            {
                                if (l_Khadgar->IsAIEnabled)
                                    l_Khadgar->AI()->DoAction(eActions::ActionTalk10Percents);
                            }

                            m_10PercentsWarn = true;

                            /// Handle new mythic phase: The Demon Within - Gul'dan continues his spells
                            if (IsMythic())
                            {
                                /// When Gul'dan reaches 10% health, Archmage Khadgar attempts to return Illidan's soul to his dormant body. A new Mythic Only boss called The Demon Within will then spawn.
                                /// It is at this point that you have 40 seconds to kill Gul'dan before The Demon Within becomes active and attackable.
                                m_Phase = ePhases::Phase4TheDemonWithin;

                                AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    DefaultEvents();
                                });
                            }
                        }

                        break;
                    }
                    case ePhases::Phase4TheDemonWithin:
                    {
                        if (m_FeignDeath)
                        {
                            p_Damage = 0;
                            return;
                        }

                        /// Prevent real death during Mythic P3, make him invisible until The Demon Within dies
                        if (p_Damage >= me->GetHealth())
                        {
                            p_Damage = 0;

                            m_FeignDeath = true;

                            me->SetHealth(1);

                            me->StopAttack();
                            me->GetMotionMaster()->Clear();

                            me->RemoveAura(eSpells::SpellTheEyeOfAmanThulFightCosmeticChan);

                            me->InterruptNonMeleeSpells(true);

                            SetCombatMovement(false);

                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_FEIGN_DEATH);

                            if (instance)
                                instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                            AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                DoCast(me, eSpells::SpellGuldanMythicDeathFadeOut, true);

                                AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    ClearEncounterSpawns();
                                });
                            });

                            Talk(eTalks::TalkMythicDeath);
                            return;
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
                    case eActions::ActionCosmeticTrigger:
                    {
                        if (m_EnergizeState >= eGuldanData::MaxQuarters)
                            break;

                        std::array<uint32, eGuldanData::MaxQuarters> l_SpellIDs =
                        {
                            {
                                eSpells::SpellTheEyeOfAmanThulCosmeticAT,
                                eSpells::SpellTheEyeOfAmanThulCosmeticAT2,
                                eSpells::SpellTheEyeOfAmanThulCosmeticAT3,
                                eSpells::SpellTheEyeOfAmanThulCosmeticAT4
                            }
                        };

                        DoCast(g_CosmeticOrbPos, l_SpellIDs[m_EnergizeState], true);
                        break;
                    }
                    case eActions::ActionIntroduction:
                    {
                        Talk(eTalks::TalkIntro1);

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(m_IntroHorde ? eTalks::TalkIntro2Horde1 : eTalks::TalkIntro2Alliance1);

                            AddTimedDelayedOperation(23 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(m_IntroHorde ? eTalks::TalkIntro2Horde2 : eTalks::TalkIntro2Alliance2);

                                AddTimedDelayedOperation(16 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    Talk(eTalks::TalkIntro3);

                                    AddTimedDelayedOperation(18 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        Talk(eTalks::TalkIntro4);

                                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                        {
                                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                        });
                                    });
                                });
                            });
                        });

                        break;
                    }
                    case eActions::ActionFelScythe:
                    {
                        if (Unit* l_Victim = me->getVictim())
                        {
                            DoCast(l_Victim, eSpells::SpellFelScytheSearcher2, true);

                            float l_Angle = frand(0.0f, 2.0f * M_PI);

                            for (uint8 l_I = 0; l_I < eGuldanData::MaxFelScythes; ++l_I)
                            {
                                Position l_Pos = l_Victim->GetPosition();

                                l_Pos.SimplePosXYRelocationByAngle(l_Pos, 5.0f, l_Angle, true);

                                l_Pos.SetOrientation(l_Pos.GetAngle(l_Victim));

                                l_Angle = Position::NormalizeOrientation(l_Angle + (2.0f * M_PI / 3.0f));

                                if (Creature* l_Fog = me->SummonCreature(eCreatures::NpcFogCloud, l_Pos))
                                {
                                    if (l_Fog->IsAIEnabled)
                                        l_Fog->AI()->SetGUID(l_Victim->GetGUID());
                                }
                            }
                        }

                        me->SetPower(Powers::POWER_ENERGY, 0);
                        me->RemoveAura(eSpells::SpellFelScythePowerIncrease);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID == eSpells::SpellWellOfSouls)
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->SetFacingTo(g_WellOfSoulsMove.m_orientation);

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellWellOfSouls);
                        });
                    });
                }
            }

            void JustSummonedGO(GameObject* p_GameObject) override
            {
                if (p_GameObject->GetEntry() == eGameObjects::GoEyeOfAmanthulCollisionBox)
                {
                    if (IsMythic() && !me->isInCombat())
                        p_GameObject->Delete();

                    m_LosBlockerGuid = p_GameObject->GetGUID();
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);

                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcLiquidHellfire:
                    {
                        m_LiquidHellfireGuid = p_Summon->GetGUID();
                        break;
                    }
                    case eCreatures::NpcFelLordKurazmal:
                    case eCreatures::NpcInquisitorVethriz:
                    case eCreatures::NpcDzorykxTheTrapper:
                    {
                        if (IsMythic())
                            break;

                        /// Last mini-boss summoned, Gul'dan locks himself within the Eye of Aman'thul
                        if (m_HandOfGuldanBossIDx > 2 && instance && !m_LockedInEye)
                        {
                            m_LockedInEye = true;

                            DoCast(me, eSpells::SpellTheEyeOfAmanThulImmunityP1, true);

                            Talk(eTalks::TalkLockedWithinTheEye);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformActivation);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeActivation1);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorRune)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeRuneActivation1);
                        }

                        break;
                    }
                    case eCreatures::NpcAzagrim:
                    case eCreatures::NpcBeltheris:
                    case eCreatures::NpcDalvengyr:
                    {
                        Talk(eTalks::TalkHeroicDreadlord, p_Summon->GetGUID());
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
                    case eCreatures::NpcFelLordKurazmal:
                    case eCreatures::NpcInquisitorVethriz:
                    case eCreatures::NpcDzorykxTheTrapper:
                    {
                        if (!instance)
                            break;

                        m_HandOfGuldanBossKilled++;
                        m_EnergizeState++;

                        switch (m_HandOfGuldanBossKilled)
                        {
                            case 1:
                            {
                                if (Creature* l_Trigger = instance->instance->GetCreature(m_CosmeticTrigger))
                                    l_Trigger->DespawnOrUnsummon();

                                if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueNE)))
                                    l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueChanneling);

                                if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueSE)))
                                    l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueRotating);

                                AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    if (Creature* l_Trigger = me->SummonCreature(eCreatures::NpcEyeOfAmanThulTrigger, g_AmanthulTriggers[0]))
                                        m_CosmeticTrigger = l_Trigger->GetGUID();

                                    if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueSE)))
                                        l_Statue->SendGameObjectActivateAnimKit(0);
                                });

                                break;
                            }
                            case 2:
                            {
                                if (Creature* l_Trigger = instance->instance->GetCreature(m_CosmeticTrigger))
                                    l_Trigger->DespawnOrUnsummon();

                                if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueSE)))
                                    l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueChanneling);

                                if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueSW)))
                                    l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueRotating);

                                AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    if (Creature* l_Trigger = me->SummonCreature(eCreatures::NpcEyeOfAmanThulTrigger, g_AmanthulTriggers[3]))
                                        m_CosmeticTrigger = l_Trigger->GetGUID();

                                    if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueSW)))
                                        l_Statue->SendGameObjectActivateAnimKit(0);
                                });

                                break;
                            }
                            case 3:
                            {
                                if (Creature* l_Trigger = instance->instance->GetCreature(m_CosmeticTrigger))
                                    l_Trigger->DespawnOrUnsummon();

                                if (GameObject* l_Statue = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoFocusingStatueSW)))
                                    l_Statue->SendGameObjectActivateAnimKit(eVisuals::VisualStatueChanneling);

                                break;
                            }
                            default:
                                break;
                        }

                        switch (p_Summon->GetEntry())
                        {
                            case eCreatures::NpcFelLordKurazmal:
                            {
                                uint64 l_Guid = p_Summon->GetGUID();
                                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                                {
                                    if (Creature* l_Lord = me->GetMap()->GetCreature(l_Guid))
                                        l_Lord->CastSpell(l_Lord, eSpells::SpellFelLordFadeOut, true);
                                });

                                break;
                            }
                            case eCreatures::NpcDzorykxTheTrapper:
                            {
                                uint64 l_Guid = p_Summon->GetGUID();
                                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                                {
                                    if (Creature* l_Trapper = me->GetMap()->GetCreature(l_Guid))
                                        l_Trapper->CastSpell(l_Trapper, eSpells::SpellJailerFadeOut, true);
                                });

                                if (instance)
                                    instance->DoRemoveForcedMovementsOnPlayers();

                                break;
                            }
                            case eCreatures::NpcInquisitorVethriz:
                            {
                                me->DespawnCreaturesInArea(eCreatures::NpcGazeOfVethriz, 150.0f);
                                break;
                            }
                            default:
                                break;
                        }

                        p_Summon->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);

                        if (m_HandOfGuldanBossKilled < 3)
                            break;

                        me->InterruptNonMeleeSpells(true);

                        events.Reset();

                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 378);

                        Talk(eTalks::TalkP1End);

                        AddTimedDelayedOperation(12 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_POINT);
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                            AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                DoCast(me, eSpells::SpellTheEyeOfAmanThulP2, true);

                                if (GameObject* l_Gob = instance->instance->GetGameObject(m_LosBlockerGuid))
                                {
                                    m_LosBlockerGuid = 0;

                                    l_Gob->Delete();
                                }

                                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanRoom)))
                                    {
                                        l_Gob->SetDisplayId(eVisuals::VisualRoomDestroyed);
                                        l_Gob->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_DESTROYED);
                                    }

                                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                                    {
                                        l_Gob->SendGameObjectActivateAnimKit(0);
                                        l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformDeactivation);
                                    }

                                    if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                                        l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeActivation2);

                                    AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                    {
                                        me->RemoveAura(eSpells::SpellTheEyeOfAmanThulImmunityP1);

                                        SetCombatMovement(true);

                                        Talk(eTalks::TalkP2Start);

                                        DoCast(me, eSpells::SpellFelScytheEnergizer, true);

                                        m_Phase = ePhases::Phase2TheRitualOfAmanThul;

                                        DefaultEvents();
                                    });
                                });
                            });
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
                    case eSpells::SpellPlayMovieAT:
                    {
                        p_AreaTrigger->MoveAreaTrigger(g_MovieMovePos, eGuldanData::PlayMovieSpeed, false, false);
                        break;
                    }
                    case eSpells::SpellGuldanWindAT:
                    {
                        uint64 l_Guid = p_AreaTrigger->GetGUID();

                        auto l_AddForce = [this, l_Guid](bool p_Add = true, bool p_Increment = false) -> void
                        {
                            AreaTrigger* l_AT = sObjectAccessor->GetAreaTrigger(*me, l_Guid);
                            if (!l_AT)
                                return;

                            std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                            for (HostileReference* l_Ref : l_ThreatList)
                            {
                                if (Unit* l_Target = l_Ref->getTarget())
                                {
                                    Player* l_Player = l_Target->ToPlayer();
                                    if (!l_Player)
                                        continue;

                                    if (p_Add)
                                    {
                                        if (!p_Increment)
                                            m_LastRegisteredForceID = l_AT->GetGUID();
                                        else
                                            ++m_LastRegisteredForceID;

                                        std::queue<uint64>& l_Queue = m_ForcedMovements[l_Player->GetGUIDLow()];

                                        l_Queue.push(m_LastRegisteredForceID);

                                        l_Player->SendApplyMovementForce(m_LastRegisteredForceID, p_Add, l_AT->GetPosition(), 1.5f, 0, G3D::Vector3(-1.5f, 0.0f, 0.0f));
                                    }
                                    else
                                    {
                                        if (m_ForcedMovements.find(l_Player->GetGUIDLow()) == m_ForcedMovements.end())
                                            return;

                                        std::queue<uint64>& l_Queue = m_ForcedMovements[l_Player->GetGUIDLow()];
                                        if (l_Queue.empty())
                                        {
                                            m_ForcedMovements.erase(l_Player->GetGUIDLow());
                                            return;
                                        }

                                        l_Player->SendApplyMovementForce(l_Queue.front(), p_Add, Position());

                                        l_Queue.pop();
                                    }
                                }
                            }
                        };

                        uint32 l_Time = 0;

                        for (uint8 l_I = 0; l_I < ((IsMythic() && m_FirstViolentWinds) ? 20 : 10); ++l_I)
                        {
                            if (l_Time)
                            {
                                AddTimedDelayedOperation(l_Time, [l_AddForce]() -> void
                                {
                                    l_AddForce(true, true);
                                });
                            }
                            else
                                l_AddForce();

                            l_Time += ((IsMythic() && m_FirstViolentWinds) ? 250 : 500);
                        }

                        for (uint8 l_I = 0; l_I < ((IsMythic() && m_FirstViolentWinds) ? 20 : 10); ++l_I)
                        {
                            if (l_Time)
                            {
                                AddTimedDelayedOperation(l_Time, [l_AddForce]() -> void
                                {
                                    l_AddForce(false);
                                });
                            }
                            else
                                l_AddForce(false);

                            l_Time += ((IsMythic() && m_FirstViolentWinds) ? 250 : 500);
                        }

                        m_FirstViolentWinds = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool /*p_Removed*/) override
            {
                switch (p_AreaTrigger->GetSpellId())
                {
                    case eSpells::SpellTheEyeOfAmanThulTransP3AT1:
                    case eSpells::SpellTheEyeOfAmanThulTransP3AT2:
                    case eSpells::SpellTheEyeOfAmanThulTransP3AT3:
                    case eSpells::SpellTheEyeOfAmanThulTransP3AT4:
                    {
                        DoCast(*p_AreaTrigger, eSpells::SpellTheEyeOfAmanThulTransP3ATVisual, true);
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
                    case eSpells::SpellFelEffluxDummy:
                    {
                        DoCast(me, eSpells::SpellFelEffluxTrigger);
                        break;
                    }
                    case eSpells::SpellFelScytheSearcher:
                    case eSpells::SpellFelScytheSearcher2:
                    {
                        DoCast(me, eSpells::SpellFuryOfTheFel, true);
                        break;
                    }
                    case eSpells::SpellWellOfSouls:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            if (Player* l_Tank = SelectMainTank())
                                AttackStart(l_Tank);
                        });

                        break;
                    }
                    case eSpells::SpellFlamesOfSargerasCast:
                    {
                        std::array<uint32, eGuldanData::MaxFlames> l_SpellIDs =
                        {
                            {
                                eSpells::SpellFlamesOfSargerasTwoTargets,
                                eSpells::SpellFlamesOfSargerasThreeTargets,
                                eSpells::SpellFlamesOfSargerasFourTargets
                            }
                        };

                        DoCast(me, l_SpellIDs[m_FlamesOfSargerasIDx++], true);

                        if (m_FlamesOfSargerasIDx >= eGuldanData::MaxFlames)
                            m_FlamesOfSargerasIDx = 0;

                        break;
                    }
                    case eSpells::SpellBlackHarvestSearcher:
                    {
                        DoCast(me, eSpells::SpellBlackHarvestCast);
                        break;
                    }
                    case eSpells::SpellStormOfTheDestroyerAura:
                    {
                        std::vector<Position> l_CastPos =
                        {
                            { 264.5214f, 3118.947f, 465.9633f, 5.47200200f },
                            { 257.4600f, 3163.428f, 465.9067f, 0.67001880f },
                            { 263.2668f, 3113.822f, 465.9067f, 5.36734700f },
                            { 271.1328f, 3121.239f, 465.9702f, 5.62151500f },
                            { 248.1637f, 3136.006f, 465.9067f, 5.46480200f },
                            { 260.5090f, 3173.828f, 465.9067f, 0.84966050f },
                            { 261.0703f, 3141.301f, 465.8879f, 6.02635200f },
                            { 257.4715f, 3152.037f, 465.9082f, 0.22110570f },
                            { 264.4269f, 3148.346f, 465.8870f, 0.03047727f },
                            { 266.8120f, 3172.698f, 465.9067f, 0.70766850f },
                            { 247.0634f, 3125.375f, 465.9067f, 5.12444000f },
                            { 238.7993f, 3131.403f, 465.9067f, 4.79999400f },
                            { 288.3010f, 3146.088f, 465.9682f, 6.25503300f },
                            { 250.4131f, 3158.093f, 465.9067f, 0.68161460f },
                            { 256.7990f, 3134.552f, 465.9072f, 5.69426000f },
                            { 254.4830f, 3170.906f, 465.9067f, 0.93941770f },
                            { 254.2305f, 3179.690f, 465.9067f, 1.08836000f },
                            { 283.6826f, 3152.853f, 465.9478f, 0.11462880f },
                            { 274.6256f, 3142.059f, 465.9661f, 6.13754500f },
                            { 253.7917f, 3140.226f, 465.9070f, 5.86476300f },
                            { 293.3592f, 3173.483f, 465.9067f, 0.43426610f },
                            { 267.2352f, 3183.649f, 465.9067f, 0.88022480f },
                            { 292.5655f, 3140.931f, 465.9589f, 6.16430600f },
                            { 255.9079f, 3146.440f, 465.9091f, 6.22486700f },
                            { 267.3172f, 3141.711f, 465.9735f, 6.09144800f },
                            { 243.3129f, 3123.604f, 465.9067f, 4.95539000f },
                            { 272.9113f, 3135.773f, 466.1064f, 5.96380700f },
                            { 244.2872f, 3154.588f, 465.9067f, 0.79704980f },
                            { 277.4825f, 3176.247f, 465.9067f, 0.62161090f },
                            { 238.6379f, 3118.355f, 465.9067f, 4.75536900f },
                            { 294.9893f, 3156.450f, 465.9127f, 0.15375660f },
                            { 278.6710f, 3160.566f, 465.9279f, 0.30601760f },
                            { 265.5948f, 3134.639f, 465.9067f, 5.85480400f },
                            { 246.1113f, 3125.635f, 465.9067f, 5.09182100f },
                            { 232.8174f, 3128.977f, 465.9067f, 4.47097300f },
                            { 271.1546f, 3178.560f, 465.9067f, 0.74325430f },
                            { 274.3937f, 3111.838f, 465.9285f, 5.51603100f },
                            { 282.7531f, 3180.948f, 465.9067f, 0.63495990f },
                            { 254.1496f, 3123.500f, 465.9058f, 5.32172600f },
                            { 244.6316f, 3161.968f, 465.9067f, 1.10576300f },
                            { 301.9010f, 3157.528f, 465.9068f, 0.15386930f },
                            { 232.2973f, 3122.660f, 465.9067f, 4.51058900f },
                            { 286.8834f, 3159.151f, 465.9562f, 0.23075310f },
                            { 249.8055f, 3149.362f, 465.9064f, 0.14706320f },
                            { 281.4138f, 3145.522f, 466.1044f, 6.23778800f },
                            { 273.1165f, 3171.532f, 465.9067f, 0.59164710f },
                            { 248.7209f, 3167.755f, 465.9067f, 1.06004300f },
                            { 259.0696f, 3128.014f, 465.9189f, 5.55062200f },
                            { 280.7290f, 3139.156f, 466.0292f, 6.09251200f },
                            { 264.2896f, 3125.405f, 465.8942f, 5.59515900f },
                            { 271.5668f, 3128.958f, 465.8693f, 5.78568800f },
                            { 295.8223f, 3181.097f, 465.9067f, 0.52148910f },
                            { 275.6221f, 3148.417f, 465.9611f, 0.02339554f },
                            { 275.7719f, 3117.527f, 465.9296f, 5.61991100f }
                        };

                        for (Position const& l_Pos : l_CastPos)
                            DoCast(l_Pos, eSpells::SpellREUSECosmetic, true);

                        Position l_Src  = me->GetPosition();

                        float l_Dist    = 10.0f;
                        float l_Step    = l_Dist / eGuldanData::MaxStormVisual1;

                        uint32 l_Time   = 0;

                        l_Dist = 0.0f;

                        for (uint8 l_I = 0; l_I < (eGuldanData::MaxStormVisual1 / 2); ++l_I)
                        {
                            Position l_Pos = me->GetPosition();

                            for (uint8 l_J = 0; l_J < 2; ++l_J)
                            {
                                l_Src.SimplePosXYRelocationByAngle(l_Pos, l_Dist, frand(0.0f, 2.0f * M_PI), true);

                                auto l_SendVisual = [this, l_Pos]() -> void
                                {
                                    me->SendPlaySpellVisual(eVisuals::VisualStormOfTheDestroyer5, nullptr, 0.75f, l_Pos, true);
                                };

                                if (!l_Time)
                                    l_SendVisual();
                                else
                                {
                                    AddTimedDelayedOperation(l_Time, [this, l_SendVisual]() -> void
                                    {
                                        l_SendVisual();
                                    });
                                }

                                l_Dist += l_Step;
                            }

                            l_Time += 200;
                        }

                        DoCast(me, eSpells::SpellStormOfTheDestroyerFirst);
                        break;
                    }
                    case eSpells::SpellStormOfTheDestroyerFirst:
                    {
                        std::vector<Position> l_CastPos =
                        {
                            { 238.2465f, 3148.027f, 465.85f, 0.5297823f },
                            { 237.6365f, 3148.490f, 465.85f, 1.3151000f },
                            { 236.8783f, 3148.385f, 465.85f, 2.1004480f },
                            { 236.4161f, 3147.775f, 465.85f, 2.8858850f },
                            { 236.5206f, 3147.017f, 465.85f, 3.6713750f },
                            { 237.1306f, 3146.554f, 465.85f, 4.4566930f },
                            { 237.8888f, 3146.659f, 465.85f, 5.2420410f },
                            { 238.3510f, 3147.269f, 465.85f, 6.0274780f }
                        };

                        me->DespawnAreaTriggersInArea(eSpells::SpellFlamesOfSargerasAT, 10.0f);

                        for (Position const& l_Pos : l_CastPos)
                        {
                            Position l_MePos    = me->GetPosition();

                            float l_RelativeX   = g_StormOfTheDestroyerSrc.m_positionX - l_Pos.m_positionX;
                            float l_RelativeY   = g_StormOfTheDestroyerSrc.m_positionY - l_Pos.m_positionY;
                            float l_RelativeZ   = g_StormOfTheDestroyerSrc.m_positionZ - l_Pos.m_positionZ;

                            Position l_DestPos  = Position(l_MePos.m_positionX + l_RelativeX, l_MePos.m_positionY + l_RelativeY, l_MePos.m_positionZ + l_RelativeZ);

                            me->PlayOrphanSpellVisual(l_MePos.AsVector3(), G3D::Vector3(0.0f, 0.0f, l_Pos.m_orientation), l_DestPos.AsVector3(), eVisuals::VisualStormOfTheDestroyer2);
                        }

                        Position l_Src  = me->GetPosition();

                        float l_Dist    = 20.0f;
                        float l_Step    = l_Dist / eGuldanData::MaxStormVisual1;

                        uint32 l_Time   = 0;

                        l_Dist = 0.0f;

                        for (uint8 l_I = 0; l_I < (eGuldanData::MaxStormVisual1 / 2); ++l_I)
                        {
                            Position l_Pos = me->GetPosition();

                            for (uint8 l_J = 0; l_J < 2; ++l_J)
                            {
                                l_Src.SimplePosXYRelocationByAngle(l_Pos, l_Dist, frand(0.0f, 2.0f * M_PI), true);

                                auto l_SendVisual = [this, l_Pos]() -> void
                                {
                                    me->SendPlaySpellVisual(eVisuals::VisualStormOfTheDestroyer5, nullptr, 0.75f, l_Pos, true);
                                };

                                if (!l_Time)
                                    l_SendVisual();
                                else
                                {
                                    AddTimedDelayedOperation(l_Time, [this, l_SendVisual]() -> void
                                    {
                                        l_SendVisual();
                                    });
                                }

                                l_Dist += l_Step;
                            }

                            l_Time += 200;
                        }

                        DoCast(me, eSpells::SpellStormOfTheDestroyerSecond);
                        break;
                    }
                    case eSpells::SpellStormOfTheDestroyerSecond:
                    {
                        if (!instance)
                            break;

                        if (Creature* l_Khadgar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcArchmageKhadgar)))
                            l_Khadgar->CastSpell(l_Khadgar, eSpells::SpellGuldanStormCosmetic, true);

                        std::vector<Position> l_CastPos =
                        {
                            { 238.3342f, 3147.832f, 465.85f, 0.3155115f },
                            { 238.0155f, 3148.297f, 465.85f, 0.8866441f },
                            { 237.4962f, 3148.516f, 465.85f, 1.4579030f },
                            { 236.9411f, 3148.419f, 465.85f, 2.0291350f },
                            { 236.3840f, 3147.492f, 465.85f, 3.1713820f },
                            { 236.5588f, 3146.957f, 465.85f, 3.7425890f },
                            { 236.9955f, 3146.600f, 465.85f, 4.3138890f },
                            { 237.5554f, 3146.537f, 465.85f, 4.8850940f },
                            { 238.0607f, 3146.786f, 465.85f, 5.4562790f },
                            { 238.3510f, 3147.269f, 465.85f, 6.0274780f }
                        };

                        me->DespawnAreaTriggersInArea(eSpells::SpellFlamesOfSargerasAT, 20.0f);

                        for (Position const& l_Pos : l_CastPos)
                        {
                            Position l_MePos    = me->GetPosition();

                            float l_RelativeX   = g_StormOfTheDestroyerSrc.m_positionX - l_Pos.m_positionX;
                            float l_RelativeY   = g_StormOfTheDestroyerSrc.m_positionY - l_Pos.m_positionY;
                            float l_RelativeZ   = g_StormOfTheDestroyerSrc.m_positionZ - l_Pos.m_positionZ;

                            Position l_DestPos  = Position(l_MePos.m_positionX + l_RelativeX, l_MePos.m_positionY + l_RelativeY, l_MePos.m_positionZ + l_RelativeZ);

                            me->PlayOrphanSpellVisual(l_MePos.AsVector3(), G3D::Vector3(0.0f, 0.0f, l_Pos.m_orientation), l_DestPos.AsVector3(), eVisuals::VisualStormOfTheDestroyer3);
                        }

                        Position l_Src  = me->GetPosition();

                        float l_Dist    = 30.0f;
                        float l_Step    = l_Dist / eGuldanData::MaxStormVisual1;

                        uint32 l_Time   = 0;

                        l_Dist = 0.0f;

                        for (uint8 l_I = 0; l_I < (eGuldanData::MaxStormVisual1 / 2); ++l_I)
                        {
                            Position l_Pos = me->GetPosition();

                            for (uint8 l_J = 0; l_J < 2; ++l_J)
                            {
                                l_Src.SimplePosXYRelocationByAngle(l_Pos, l_Dist, frand(0.0f, 2.0f * M_PI), true);

                                auto l_SendVisual = [this, l_Pos]() -> void
                                {
                                    me->SendPlaySpellVisual(eVisuals::VisualStormOfTheDestroyer5, nullptr, 0.75f, l_Pos, true);
                                };

                                if (!l_Time)
                                    l_SendVisual();
                                else
                                {
                                    AddTimedDelayedOperation(l_Time, [this, l_SendVisual]() -> void
                                    {
                                        l_SendVisual();
                                    });
                                }

                                l_Dist += l_Step;
                            }

                            l_Time += 200;
                        }

                        DoCast(me, eSpells::SpellStormOfTheDestroyerThird);
                        break;
                    }
                    case eSpells::SpellStormOfTheDestroyerThird:
                    {
                        std::vector<Position> l_CastPos =
                        {
                            { 238.3792f, 3147.615f, 465.85f, 0.09339655f },
                            { 238.2873f, 3147.950f, 465.85f, 0.44250630f },
                            { 238.0863f, 3148.233f, 465.85f, 0.79150500f },
                            { 237.8006f, 3148.431f, 465.85f, 1.14059000f },
                            { 237.4646f, 3148.519f, 465.85f, 1.48963200f },
                            { 237.1189f, 3148.486f, 465.85f, 1.83869200f },
                            { 236.6808f, 3146.811f, 465.85f, 3.93309800f },
                            { 236.9665f, 3146.613f, 465.85f, 4.28218300f },
                            { 237.3025f, 3146.525f, 465.85f, 4.63122500f },
                            { 237.6483f, 3146.558f, 465.85f, 4.98028500f },
                            { 237.9621f, 3146.706f, 465.85f, 5.32933800f },
                            { 238.2062f, 3146.953f, 465.85f, 5.67839000f },
                            { 238.3510f, 3147.269f, 465.85f, 6.02747800f }
                        };

                        me->DespawnAreaTriggersInArea(eSpells::SpellFlamesOfSargerasAT, 30.0f);

                        for (Position const& l_Pos : l_CastPos)
                        {
                            Position l_MePos    = me->GetPosition();

                            float l_RelativeX   = g_StormOfTheDestroyerSrc.m_positionX - l_Pos.m_positionX;
                            float l_RelativeY   = g_StormOfTheDestroyerSrc.m_positionY - l_Pos.m_positionY;
                            float l_RelativeZ   = g_StormOfTheDestroyerSrc.m_positionZ - l_Pos.m_positionZ;

                            Position l_DestPos  = Position(l_MePos.m_positionX + l_RelativeX, l_MePos.m_positionY + l_RelativeY, l_MePos.m_positionZ + l_RelativeZ);

                            me->PlayOrphanSpellVisual(l_MePos.AsVector3(), G3D::Vector3(0.0f, 0.0f, l_Pos.m_orientation), l_DestPos.AsVector3(), eVisuals::VisualStormOfTheDestroyer4);
                        }

                        Position l_Src  = me->GetPosition();

                        float l_Dist    = 60.0f;
                        float l_Step    = l_Dist / eGuldanData::MaxStormVisual2;

                        uint32 l_Time   = 0;

                        l_Dist = 0.0f;

                        for (uint8 l_I = 0; l_I < (eGuldanData::MaxStormVisual2 / 2); ++l_I)
                        {
                            Position l_Pos = me->GetPosition();

                            for (uint8 l_J = 0; l_J < 2; ++l_J)
                            {
                                l_Src.SimplePosXYRelocationByAngle(l_Pos, l_Dist, frand(0.0f, 2.0f * M_PI), true);

                                auto l_SendVisual = [this, l_Pos]() -> void
                                {
                                    me->SendPlaySpellVisual(eVisuals::VisualStormOfTheDestroyer5, nullptr, 1.333333f, l_Pos, true);
                                };

                                if (!l_Time)
                                    l_SendVisual();
                                else
                                {
                                    AddTimedDelayedOperation(l_Time, [this, l_SendVisual]() -> void
                                    {
                                        l_SendVisual();
                                    });
                                }

                                l_Dist += l_Step;
                            }

                            l_Time += 100;
                        }

                        DoCast(me, eSpells::SpellStormOfTheDestroyerFourth);
                        break;
                    }
                    case eSpells::SpellStormOfTheDestroyerFourth:
                    {
                        std::vector<Position> l_CastPos =
                        {
                            { 238.3742f, 3147.658f, 465.85f, 0.1369036f },
                            { 238.2465f, 3148.027f, 465.85f, 0.5297823f },
                            { 237.9875f, 3148.319f, 465.85f, 0.9224371f },
                            { 237.6365f, 3148.490f, 465.85f, 1.3151000f },
                            { 237.2470f, 3148.513f, 465.85f, 1.7077780f },
                            { 237.1306f, 3146.554f, 465.85f, 4.4566930f },
                            { 237.5201f, 3146.531f, 465.85f, 4.8493710f },
                            { 237.8888f, 3146.659f, 465.85f, 5.2420410f },
                            { 238.1806f, 3146.918f, 465.85f, 5.6347170f },
                            { 238.3510f, 3147.269f, 465.85f, 6.0274780f }
                        };

                        me->DespawnAreaTriggersInArea(eSpells::SpellFlamesOfSargerasAT, 60.0f);

                        for (Position const& l_Pos : l_CastPos)
                        {
                            Position l_MePos    = me->GetPosition();

                            float l_RelativeX   = g_StormOfTheDestroyerSrc.m_positionX - l_Pos.m_positionX;
                            float l_RelativeY   = g_StormOfTheDestroyerSrc.m_positionY - l_Pos.m_positionY;
                            float l_RelativeZ   = g_StormOfTheDestroyerSrc.m_positionZ - l_Pos.m_positionZ;

                            Position l_DestPos  = Position(l_MePos.m_positionX + l_RelativeX, l_MePos.m_positionY + l_RelativeY, l_MePos.m_positionZ + l_RelativeZ);

                            me->PlayOrphanSpellVisual(l_MePos.AsVector3(), G3D::Vector3(0.0f, 0.0f, l_Pos.m_orientation), l_DestPos.AsVector3(), eVisuals::VisualStormOfTheDestroyer1);
                        }

                        DoCast(me, eSpells::SpellDemonicCircleSearcher, true);
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
                    case eSpells::SpellBlackHarvestCast:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (Unit* l_Target = me->getVictim())
                        {
                            AttackStart(l_Target);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Target);
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
                    case eSpells::SpellLiquidHellfireCast:
                    case eSpells::SpellEmpoweredLiquidHellfire:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        if (Creature* l_Hellfire = Creature::GetCreature(*me, m_LiquidHellfireGuid))
                        {
                            if (l_Hellfire->IsAIEnabled)
                                l_Hellfire->AI()->SetGUID(p_Target->GetGUID());
                        }

                        break;
                    }
                    case eSpells::SpellBondsOfFelCast:
                    {
                        sCreatureTextMgr->SendChat(me, eTalks::TalkAfflictedByBondsOfFel, p_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                        DoCast(p_Target, eSpells::SpellBondsOfFelMissile, true);
                        break;
                    }
                    case eSpells::SpellEmpoweredBondsOfFelCast:
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpellEmpoweredBondsOfFelKnock, me->GetMap()->GetDifficultyID());
                        if (!l_SpellInfo)
                            break;

                        sCreatureTextMgr->SendChat(me, eTalks::TalkAfflictedByEmpoweredBondsOfFel, p_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                        float l_Ratio   = 0.1f;
                        float l_SpeedXY = float(l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].MiscValue) * l_Ratio;
                        float l_SpeedZ  = float(l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints) * l_Ratio;

                        if (l_SpeedXY < 0.1f && l_SpeedZ < 0.1f)
                            break;

                        DoCast(p_Target, eSpells::SpellEmpoweredBondsOfFelKnock, true);

                        uint64 l_Guid = p_Target->GetGUID();
                        AddTimedDelayedOperation(10, [this, l_Guid, l_SpeedXY, l_SpeedZ]() -> void
                        {
                            if (Unit* l_Target = Unit::GetUnit(*me, l_Guid))
                            {
                                l_Target->KnockbackFrom(me->m_positionX, me->m_positionY, l_SpeedXY, l_SpeedZ);

                                if (l_Target->IsPlayer())
                                    l_Target->ToPlayer()->SetKnockBackTime(getMSTime());
                            }

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                            {
                                if (Unit* l_Target = Unit::GetUnit(*me, l_Guid))
                                    DoCast(l_Target, eSpells::SpellEmpoweredBondsOfFelMissile, true);
                            });
                        });

                        break;
                    }
                    case eSpells::SpellBondsOfFelAura:
                    case eSpells::SpellEmpoweredBondsOfFelAura:
                    {
                        for (uint8 l_I = 0; l_I < eGuldanData::MaxBonds; ++l_I)
                        {
                            auto& l_ArrayIter = m_BondsOfFelGuids[p_Target->GetGUIDLow()];

                            Position l_Pos = p_Target->GetPosition();

                            l_Pos.SimplePosXYRelocationByAngle(l_Pos, 3.0f, frand(0.0f, 2.0f * M_PI), true);

                            if (Creature* l_Bond = me->SummonCreature(eCreatures::NpcBondsOfFel, l_Pos))
                            {
                                if (l_Bond->IsAIEnabled)
                                    l_Bond->AI()->SetGUID(p_Target->GetGUID());

                                l_ArrayIter[l_I] = l_Bond->GetGUID();
                            }
                            else
                                l_ArrayIter[l_I] = 0;
                        }

                        if (p_SpellInfo->Id == eSpells::SpellEmpoweredBondsOfFelAura)
                            p_Target->CastSpell(p_Target, eSpells::SpellEmpoweredBondsOfFelAT, true);

                        break;
                    }
                    case eSpells::SpellFelScytheSearcher:
                    case eSpells::SpellFelScytheSearcher2:
                    {
                        if (!m_FelScytheTargetCount)
                            break;

                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpellFelScytheDamage, me->GetMap()->GetDifficultyID());
                        if (!l_SpellInfo)
                            break;

                        int32 l_Damage = l_SpellInfo->Effects[SpellEffIndex::EFFECT_0].CalcValue(me);

                        l_Damage /= m_FelScytheTargetCount;

                        me->CastCustomSpell(p_Target, eSpells::SpellFelScytheDamage, &l_Damage, nullptr, nullptr, true);
                        break;
                    }
                    case eSpells::SpellFlamesOfSargerasTwoTargets:
                    case eSpells::SpellFlamesOfSargerasThreeTargets:
                    case eSpells::SpellFlamesOfSargerasFourTargets:
                    {
                        DoCast(p_Target, eSpells::SpellFlamesOfSargerasAura, true);
                        break;
                    }
                    case eSpells::SpellSoulSiphonSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellSoulSiphonAura, true);
                        DoCast(p_Target, eSpells::SpellSoulSiphonSummon, true);
                        break;
                    }
                    case eSpells::SpellDemonicCircleSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellDemonicCircleTriggered, true);
                        break;
                    }
                    case eSpells::SpellWellOfSoulsDoT:
                    {
                        me->getThreatManager().modifyThreatPercent(p_Target, -99);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                if (p_Mode != SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellHandOfGuldanDamage:
                    {
                        if (m_Phase == ePhases::Phase1TheCouncilOfElders || IsMythic())
                        {
                            if (m_HandOfGuldanBossIDx > 2)
                                break;

                            std::vector<uint32> l_IDs = { eSpells::SpellHandOfGuldanSummonKurazmal, eSpells::SpellHandOfGuldanSummonVethriz, eSpells::SpellHandOfGuldanSummonDzorykx };

                            DoCast(p_Dest->_position, l_IDs[m_HandOfGuldanBossIDx++], true);
                        }
                        else if (m_Phase == ePhases::Phase2TheRitualOfAmanThul)
                        {
                            /// In Phase 2, Gul'dan will now cast Hand of Gul'dan which will summon a Dreadlord of the Twisting Nether.
                            std::vector<uint32> l_IDs = { eSpells::SpellHandOfGuldanSummonAzagrim, eSpells::SpellHandOfGuldanSummonBeltheris, eSpells::SpellHandOfGuldanSummonDalvengyr };

                            DoCast(p_Dest->_position, l_IDs[urand(0, 2)], true);
                        }

                        break;
                    }
                    case eSpells::SpellEyeOfGuldanTrigger:
                    {
                        if (m_EmpoweredEyeOfGuldan)
                        {
                            DoCast(p_Dest->_position, eSpells::SpellEmpoweredEyeOfGuldanCast);

                            Position l_Pos = p_Dest->_position;
                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Pos]() -> void
                            {
                                me->SummonCreature(eCreatures::NpcEmpoweredEyeOfGuldan, l_Pos);
                            });
                        }
                        else
                        {
                            DoCast(p_Dest->_position, eSpells::SpellEyeOfGuldanCast);

                            me->SummonCreature(eCreatures::NpcEyeOfGuldan, p_Dest);

                            Position l_Pos = p_Dest->_position;
                            AddTimedDelayedOperation(500, [this, l_Pos]() -> void
                            {
                                me->SummonCreature(eCreatures::NpcEyeOfGuldan, l_Pos);
                            });

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this, l_Pos]() -> void
                            {
                                me->SummonCreature(eCreatures::NpcEyeOfGuldan, l_Pos);
                            });
                        }

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
                    case eSpells::SpellFelEffluxAura:
                    {
                        m_FelEfflux = false;
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        break;
                    }
                    case eSpells::SpellTheEyeOfAmanThulImmunityP2ToP3:
                    {
                        DoCast(me, eSpells::SpellTheEyeOfAmanThulTransitionToP3, true);

                        if (!instance)
                            break;

                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                        {
                            l_Gob->SendGameObjectActivateAnimKit(0);
                            l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformCircleStopped);
                        }

                        if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulFloorSphere)))
                            l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualEyeActivation2);

                        AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoEyeOfAmanthulWorldS)))
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualWorldOpenedSouth);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(instance->GetData64(eGameObjects::GoGulDanCenterPlatform)))
                            {
                                l_Gob->SendGameObjectActivateAnimKit(0);
                                l_Gob->SendGameObjectActivateAnimKit(eVisuals::VisualPlatformCircleFinish);
                            }

                            /// On mythic mode, this will happens at the transition to The Demon Within phase
                            if (!IsMythic())
                            {
                                if (Creature* l_Khadgar = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcArchmageKhadgar)))
                                {
                                    if (l_Khadgar->IsAIEnabled)
                                        l_Khadgar->AI()->DoAction(eActions::ActionTriggerKhadgar);
                                }
                            }

                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeDilationOverrider);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellScatteringFieldOverrider);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellResonantBarrierOverrider);

                            me->DespawnAreaTriggersInArea(eSpells::SpellEmpoweredLiquidHellfireAT, 150.0f);

                            if (GameObject* l_Gob = instance->instance->GetGameObject(m_LosBlockerGuid))
                            {
                                m_LosBlockerGuid = 0;

                                l_Gob->Delete();
                            }

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::TalkP3Start);

                                if (!IsLFR())
                                {
                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MovePoint(eSpells::SpellWellOfSouls, g_WellOfSoulsMove);
                                }
                                else
                                {
                                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                    if (Player* l_Tank = SelectMainTank())
                                        AttackStart(l_Tank);
                                }
                            });
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellLiquidHellfireCast:
                    case eSpells::SpellEmpoweredLiquidHellfire:
                    {
                        p_Targets.clear();

                        if (Player* l_Target = SelectRangedTarget(false))
                            p_Targets.push_back(l_Target);
                        else if (Player* l_Target = SelectRangedTarget(true))
                            p_Targets.push_back(l_Target);
                        else if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            p_Targets.push_back(l_Target);

                        break;
                    }
                    case eSpells::SpellBondsOfFelCast:
                    case eSpells::SpellEmpoweredBondsOfFelCast:
                    {
                        p_Targets.clear();

                        /// This ability will always target the primary tank.
                        if (Player* l_Tank = SelectMainTank())
                            p_Targets.push_back(l_Tank);
                        else if (Unit* l_Tank = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 0.0f, true))
                            p_Targets.push_back(l_Tank);

                        for (uint8 l_I = 0; l_I < (IsMythic() ? 3 : 2); ++l_I)
                        {
                            if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                                p_Targets.push_back(l_Player);
                        }

                        break;
                    }
                    case eSpells::SpellFelScytheSearcher:
                    case eSpells::SpellFelScytheSearcher2:
                    {
                        m_FelScytheTargetCount = uint32(p_Targets.size());
                        break;
                    }
                    case eSpells::SpellDemonicCircleSearcher:
                    {
                        p_Targets.clear();

                        if (Unit* l_Tank = me->getVictim())
                            p_Targets.push_back(l_Tank);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eGuldanData::BondsOfFelGuid:
                    {
                        if (m_BondsOfFelGuids.find(p_Value) == m_BondsOfFelGuids.end())
                            break;

                        auto const& l_ArrayIter = m_BondsOfFelGuids[p_Value];

                        for (uint8 l_I = 0; l_I < eGuldanData::MaxBonds; ++l_I)
                        {
                            if (Creature* l_Bond = Creature::GetCreature(*me, l_ArrayIter[l_I]))
                                l_Bond->DespawnOrUnsummon();
                        }

                        m_BondsOfFelGuids.erase(p_Value);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_CheckPlayersTimer)
                {
                    if (m_CheckPlayersTimer <= p_Diff)
                    {
                        m_CheckPlayersTimer = 1 * TimeConstants::IN_MILLISECONDS;

                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                        std::list<Player*> l_PlayerList;
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = l_Ref->getTarget())
                            {
                                Player* l_Player = l_Target->ToPlayer();
                                if (!l_Player || l_Player->GetMap() != me->GetMap())
                                    continue;

                                if (l_Player->IsFalling() && l_Player->m_positionZ <= 450.0f)
                                    l_PlayerList.push_back(l_Player);
                            }
                        }

                        for (Player* l_Player : l_PlayerList)
                            me->Kill(l_Player);
                    }
                    else
                        m_CheckPlayersTimer -= p_Diff;
                }

                if (!m_IntroDone && m_IntroCheckTimer)
                {
                    if (m_IntroCheckTimer <= p_Diff)
                    {
                        if (Player* l_Player = me->FindNearestPlayer(50.0f))
                        {
                            if (l_Player->GetTeamId() == TeamId::TEAM_HORDE)
                                m_IntroHorde = true;

                            m_IntroDone = true;
                            m_IntroCheckTimer = 0;

                            DoAction(eActions::ActionIntroduction);
                        }
                        else
                            m_IntroCheckTimer = 500;
                    }
                    else
                        m_IntroCheckTimer -= p_Diff;
                }

                if (!UpdateVictim() || m_FeignDeath)
                    return;

                m_EnrageEvent.Update(p_Diff);

                if (m_EnrageEvent.ExecuteEvent() == eEvents::EventBerserk)
                    DoCast(me, eSharedSpells::SpellBerserkGuldan, true);

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || m_FelEfflux)
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventLiquidHellfire:
                    {
                        ++m_LiquidHellfireCount;

                        Talk(eTalks::TalkLiquidHellfire);

                        Talk(eTalks::TalkLiquidHellfireWarn);

                        DoCast(me, eSpells::SpellLiquidHellfireSummon, true);
                        DoCast(me, eSpells::SpellLiquidHellfireDummy, true);

                        if (m_Phase == ePhases::Phase1TheCouncilOfElders)
                        {
                            if (m_LiquidHellfireCount == 2)
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 15 * TimeConstants::IN_MILLISECONDS);
                            else if (!IsHeroicOrMythic() && m_LiquidHellfireCount > 3)
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 32 * TimeConstants::IN_MILLISECONDS + 500);
                            else
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 25 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (IsMythic())
                        {
                            if (m_LiquidHellfireCount == 5 || m_LiquidHellfireCount == 7)
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 66 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 33 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (!IsHeroicOrMythic())
                        {
                            if (m_LiquidHellfireCount == 5)
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 82 * TimeConstants::IN_MILLISECONDS + 500);
                            else
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 41 * TimeConstants::IN_MILLISECONDS + 200);
                        }
                        else
                        {
                            if (m_LiquidHellfireCount == 5)
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 73 * TimeConstants::IN_MILLISECONDS + 200);
                            else
                                events.ScheduleEvent(eEvents::EventLiquidHellfire, 36 * TimeConstants::IN_MILLISECONDS + 600);
                        }

                        break;
                    }
                    case eEvents::EventHandOfGuldan:
                    {
                        ++m_HandOfGuldanCount;

                        Talk(eTalks::TalkHandOfGuldan);

                        DoCast(me, eSpells::SpellHandOfGuldanDummy, true);

                        if (Player* l_Target = (IsMythic() ? SelectPlayerTarget(eTargetTypeMask::TypeMaskHealer) : SelectRangedTarget()))
                            DoCast(l_Target->GetPosition(), eSpells::SpellHandOfGuldanCast);
                        else if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Target->GetPosition(), eSpells::SpellHandOfGuldanCast);

                        if (m_Phase == ePhases::Phase1TheCouncilOfElders && m_HandOfGuldanCount < 4)
                        {
                            if (m_HandOfGuldanCount == 2)
                                events.ScheduleEvent(eEvents::EventHandOfGuldan, 14 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventHandOfGuldan, 10 * TimeConstants::IN_MILLISECONDS);
                        }
                        else if (m_Phase == ePhases::Phase2TheRitualOfAmanThul)
                        {
                            if (IsHeroic())
                            {
                                if (m_HandOfGuldanCount == 2)
                                    events.ScheduleEvent(eEvents::EventHandOfGuldan, 48 * TimeConstants::IN_MILLISECONDS + 900);
                                else
                                    events.ScheduleEvent(eEvents::EventHandOfGuldan, 138 * TimeConstants::IN_MILLISECONDS + 900);
                            }
                            else if (IsMythic())
                            {
                                if (m_HandOfGuldanCount == 2)
                                    events.ScheduleEvent(eEvents::EventHandOfGuldan, 181 * TimeConstants::IN_MILLISECONDS + 600);
                            }
                        }

                        break;
                    }
                    case eEvents::EventFelEfflux:
                    {
                        Talk(eTalks::TalkFelEffluxAndFlamesOfSargeras);

                        me->SetFacingTo(frand(0.0f, 2.0f * M_PI));

                        m_FelEfflux = true;

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                            DoCast(me, eSpells::SpellFelEffluxDummy, true);
                        });

                        events.ScheduleEvent(eEvents::EventFelEfflux, IsHeroicOrMythic() ? 12 * TimeConstants::IN_MILLISECONDS : (15 * TimeConstants::IN_MILLISECONDS + 600));
                        break;
                    }
                    case eEvents::EventBondsOfFel:
                    {
                        Talk(eTalks::TalkBondsOfFel);

                        if (m_EmpoweredBondsOfFel)
                            DoCast(me, eSpells::SpellEmpoweredBondsOfFelCast);
                        else
                            DoCast(me, eSpells::SpellBondsOfFelCast);

                        if (IsMythic())
                            events.ScheduleEvent(eEvents::EventBondsOfFel, 40 * TimeConstants::IN_MILLISECONDS);
                        else if (IsHeroic())
                            events.ScheduleEvent(eEvents::EventBondsOfFel, 44 * TimeConstants::IN_MILLISECONDS + 500);
                        else
                            events.ScheduleEvent(eEvents::EventBondsOfFel, 50 * TimeConstants::IN_MILLISECONDS);

                        break;
                    }
                    case eEvents::EventEyeOfGuldan:
                    {
                        Talk(eTalks::TalkEyeOfGuldan);

                        DoCast(me, eSpells::SpellEyeOfGuldan);

                        ++m_EyeOfGuldanCount;

                        if (m_Phase == ePhases::Phase2TheRitualOfAmanThul)
                        {
                            if (IsLFR())
                                events.ScheduleEvent(eEvents::EventEyeOfGuldan, 64 * TimeConstants::IN_MILLISECONDS);
                            else if (IsNormal())
                                events.ScheduleEvent(eEvents::EventEyeOfGuldan, 60 * TimeConstants::IN_MILLISECONDS);
                            else if (IsMythic())
                            {
                                if (m_EyeOfGuldanCount == 7)
                                    events.ScheduleEvent(eEvents::EventEyeOfGuldan, 80 * TimeConstants::IN_MILLISECONDS);
                                else
                                    events.ScheduleEvent(eEvents::EventEyeOfGuldan, 48 * TimeConstants::IN_MILLISECONDS);
                            }
                            else
                                events.ScheduleEvent(eEvents::EventEyeOfGuldan, 53 * TimeConstants::IN_MILLISECONDS + 300);
                        }
                        else
                        {
                            std::map<uint32, std::vector<uint32>> l_Timers =
                            {
                                { Difficulty::DifficultyRaidNormal, { 42700, 71400, 71400, 28600, 114300 } },
                                { Difficulty::DifficultyRaidHeroic, { 39100, 62500, 62500, 25000, 100000 } },
                                { Difficulty::DifficultyRaidMythic, { 35100, 52600, 53300, 20400, 84200, 52600 } }
                            };

                            auto const& l_DiffIter = IsLFR() ? l_Timers.find(Difficulty::DifficultyRaidNormal) : l_Timers.find(me->GetMap()->GetDifficultyID());
                            if (l_DiffIter == l_Timers.end())
                                break;

                            if ((m_EyeOfGuldanCount - 1) >= l_DiffIter->second.size())
                                break;

                            events.ScheduleEvent(eEvents::EventEyeOfGuldan, l_DiffIter->second[m_EyeOfGuldanCount - 1]);
                        }

                        break;
                    }
                    case eEvents::EventViolentWinds:
                    {
                        DoCast(me, eSpells::SpellGuldanWindAT, true);
                        break;
                    }
                    case eEvents::EventFlamesOfSargeras:
                    {
                        ++m_FlamesOfSargerasCount;

                        Talk(eTalks::TalkFelEffluxAndFlamesOfSargeras);

                        DoCast(me, eSpells::SpellFlamesOfSargerasCast);

                        if (IsNormal() || IsLFR())
                        {
                            if (m_FlamesOfSargerasCount == 9)
                                events.ScheduleEvent(eEvents::EventFlamesOfSargeras, 41 * TimeConstants::IN_MILLISECONDS);
                            else if ((m_FlamesOfSargerasCount % 2) == 0)
                                events.ScheduleEvent(eEvents::EventFlamesOfSargeras, 19 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventFlamesOfSargeras, 39 * TimeConstants::IN_MILLISECONDS + 600);
                        }
                        else
                        {
                            std::map<uint32, std::vector<uint32>> l_Timers =
                            {
                                { Difficulty::DifficultyRaidHeroic, { 27600, 7800, 8800, 34700, 7800, 8800, 34700, 7800, 8700, 34800, 7700, 8800, 36000, 7700, 8800 } },
                                { Difficulty::DifficultyRaidMythic, { 25700, 6400, 7400, 29400, 6400, 7400, 29400, 6400, 7400, 29400, 6400, 7400, 29500, 7400, 7400, 28400, 6400, 7400, 28400, 6400, 7400 } }
                            };

                            auto const& l_DiffIter = l_Timers.find(me->GetMap()->GetDifficultyID());
                            if (l_DiffIter == l_Timers.end())
                                break;

                            if ((m_FlamesOfSargerasCount - 1) >= l_DiffIter->second.size())
                                break;

                            events.ScheduleEvent(eEvents::EventFlamesOfSargeras, l_DiffIter->second[m_FlamesOfSargerasCount - 1]);
                        }

                        break;
                    }
                    case eEvents::EventBlackHarvest:
                    {
                        Talk(eTalks::TalkBlackHarvest);

                        me->GetMotionMaster()->Clear();
                        me->StopAttack();
                        me->SetGuidValue(EUnitFields::UNIT_FIELD_TARGET, 0);
                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        DoCast(me, eSpells::SpellBlackHarvestSearcher, true);

                        ++m_BlackHarvestCount;

                        std::map<uint32, std::vector<uint32>> l_Timers =
                        {
                            { Difficulty::DifficultyRaidNormal, { 71200, 82800, 100000 } },
                            { Difficulty::DifficultyRaidHeroic, { 64100, 72500, 87600 } },
                            { Difficulty::DifficultyRaidMythic, { 55700, 61000, 75300, 86800 } }
                        };

                        auto const& l_DiffIter = IsLFR() ? l_Timers.find(Difficulty::DifficultyRaidNormal) : l_Timers.find(me->GetMap()->GetDifficultyID());
                        if (l_DiffIter == l_Timers.end())
                            break;

                        if (m_BlackHarvestCount >= l_DiffIter->second.size())
                            break;

                        events.ScheduleEvent(eEvents::EventBlackHarvest, l_DiffIter->second[m_BlackHarvestCount]);

                        /// Violent Winds timers - Handled by Black Harvest, as on BigWigs data
                        if (IsMythic())
                        {
                            std::vector<uint32> l_WindTimers = { 11500, 43400, 66000, 75400 };

                            if (m_BlackHarvestCount >= l_WindTimers.size())
                                break;

                            events.ScheduleEvent(eEvents::EventViolentWinds, l_WindTimers[m_BlackHarvestCount]);
                        }

                        break;
                    }
                    case eEvents::EventStormOfTheDestroyer:
                    {
                        Talk(eTalks::TalkStormOfTheDestroyer);

                        DoCast(me, eSpells::SpellStormOfTheDestroyerAura, true);

                        ++m_StormCount;

                        if (IsLFR() || IsNormal())
                        {
                            if (instance)
                                instance->DoCastSpellOnPlayers(eSpells::SpellPhaseBurst);

                            if ((m_StormCount + 1) == 2)
                                events.ScheduleEvent(eEvents::EventStormOfTheDestroyer, 78 * TimeConstants::IN_MILLISECONDS + 500);
                            else
                                events.ScheduleEvent(eEvents::EventStormOfTheDestroyer, 70 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                        {
                            std::map<uint32, std::vector<uint32>> l_Timers =
                            {
                                { Difficulty::DifficultyRaidHeroic, { 84100, 68800, 61200, 76500        } },
                                { Difficulty::DifficultyRaidMythic, { 72600, 57900, 51600, 64700, 57400 } }
                            };

                            auto const& l_DiffIter = l_Timers.find(me->GetMap()->GetDifficultyID());
                            if (l_DiffIter == l_Timers.end())
                                break;

                            if (m_StormCount >= l_DiffIter->second.size())
                                break;

                            events.ScheduleEvent(eEvents::EventStormOfTheDestroyer, l_DiffIter->second[m_StormCount]);
                        }

                        break;
                    }
                    case eEvents::EventSoulSiphon:
                    {
                        ++m_SoulSiphonCount;

                        DoCast(me, eSpells::SpellSoulSiphonPeriodic, true);

                        std::vector<uint32> l_Timers = { 19000, 11500, 50000, 11000, 11000, 60000, 11500 };

                        if ((m_SoulSiphonCount - 1) >= l_Timers.size())
                            break;

                        events.ScheduleEvent(eEvents::EventSoulSiphon, l_Timers[m_SoulSiphonCount - 1]);
                        break;
                    }
                    default:
                        break;
                }

                if (m_Phase != ePhases::Phase1TheCouncilOfElders)
                    DoMeleeAttackIfReady();
            }

            void ClearEncounterSpawns()
            {
                me->RemoveAllAreasTrigger();

                std::vector<uint32> l_EntriesToDespawn =
                {
                    eCreatures::NpcGazeOfVethriz,
                    eCreatures::NpcNightwellEntity,
                    eCreatures::NpcFelObelisk,
                    eCreatures::NpcDarkSoul,
                    eCreatures::NpcWellOfSouls1,
                    eCreatures::NpcWellOfSouls2
                };

                me->DespawnCreaturesInArea(l_EntriesToDespawn, 150.0f);

                l_EntriesToDespawn =
                {
                    eSpells::SpellSoulVortexAT,
                    eSpells::SpellBondsOfFelAT,
                    eSpells::SpellEmpoweredLiquidHellfireAT,
                    eSpells::SpellEmpoweredBondsOfFelAT,
                    eSpells::SpellREUSEAT,
                    eSpells::SpellWellOfSouls,
                    eSpells::SpellFlamesOfSargerasAT,
                    eSpells::SpellSoulseverSoulAT
                };

                me->DespawnAreaTriggersInArea(l_EntriesToDespawn, 150.0f, false);
            }

            void ClearEncounterAuras()
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTimeDilationOverrider);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellScatteringFieldOverrider);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellScatteringFieldAbsorb);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellResonantBarrierOverrider);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellBondsOfFelAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmpoweredBondsOfFelAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFlamesOfSargerasAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFlamesOfSargerasPeriodic);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSoulSiphonAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmpoweredEyeOfGuldanAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSoulCorrosionAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellTornSoulDoT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellBulwarkOfAzzinothAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVisionsScreenEffect);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_guldanAI(p_Creature);
        }
};

/// Gul'dan - 114276
class npc_guldan_mythic_vehicle : public CreatureScript
{
    public:
        npc_guldan_mythic_vehicle() : CreatureScript("npc_guldan_mythic_vehicle") { }

        struct npc_guldan_mythic_vehicleAI : public Scripted_NoMovementAI
        {
            npc_guldan_mythic_vehicleAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                SetFlyMode(true);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        if (Creature* l_Guldan = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcGulDan)))
                            l_Guldan->EnterVehicle(me);
                    }

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Guldan = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcGulDan)))
                            {
                                if (Creature* l_Trigger = l_Guldan->SummonCreature(eCreatures::NpcEyeOfAmanThulMythic, l_Guldan->GetPosition()))
                                    l_Trigger->EnterVehicle(l_Guldan);
                            }
                        }
                    });
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_mythic_vehicleAI(p_Creature);
        }
};

/// Liquid Hellfire - 104396
class npc_guldan_liquid_hellfire : public CreatureScript
{
    public:
        npc_guldan_liquid_hellfire() : CreatureScript("npc_guldan_liquid_hellfire") { }

        enum eSpells
        {
            SpellLiquidHellfireVisual           = 206611,
            SpellEncounterBoundsAuraBroken      = 227371,
            SpellEncounterBoundsAuraIntact      = 206261,

            SpellLiquidHellfireMissile          = 206554,
            SpellEmpoweredLiquidHellfireMissile = 206586
        };

        struct npc_guldan_liquid_hellfireAI : public ScriptedAI
        {
            npc_guldan_liquid_hellfireAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_Target = 0;

            Position m_FinalPos;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                DoCast(me, eSpells::SpellLiquidHellfireVisual, true);
                DoCast(me, eSpells::SpellEncounterBoundsAuraBroken, true);
                DoCast(me, eSpells::SpellEncounterBoundsAuraIntact, true);

                Movement::MoveSplineInit l_Init(me);

                Optional<Movement::MonsterSplineFilter> l_Filter;

                l_Init.SetFly();
                l_Init.SetSmooth();
                l_Init.SetUncompressed();

                l_Filter.emplace();

                l_Filter->FilterFlags   = 2;
                l_Filter->BaseSpeed     = 6.0f;
                l_Filter->AddedToStart  = 1;

                l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(0, 600));
                l_Filter->FilterKeys.push_back(Movement::MonsterSplineFilterKey(2, 200));

                l_Init.SetMonsterSplineFilters(*l_Filter.get_ptr());

                std::vector<float> l_ZOffsets   = { 1.0f, 4.0f, 8.0f, 19.0f };
                std::vector<float> l_Dists      = { 3.0f, 3.0f, 3.0f, 5.0f };

                uint8 l_IDx         = 0;
                float l_CurrDist    = 0.0f;

                for (float const& l_Dist : l_Dists)
                {
                    l_CurrDist += l_Dist;

                    Position l_Pos = me->GetPosition();

                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, l_CurrDist, me->m_orientation, true);

                    l_Pos.m_positionZ += l_ZOffsets[l_IDx++] * 2.0f;

                    if (l_IDx >= uint8(l_ZOffsets.size()))
                        m_FinalPos = l_Pos;

                    l_Init.Path().push_back(l_Pos.AsVector3());

                    /// Push the first pos twice
                    if (l_IDx == 1)
                        l_Init.Path().push_back(l_Pos.AsVector3());
                }

                l_Init.Launch();
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                m_Target = p_Guid;

                if (!IsMythic())
                {
                    me->RemoveAura(eSpells::SpellLiquidHellfireVisual);

                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, 100);
                    me->SetFloatValue(EObjectFields::OBJECT_FIELD_SCALE, 1.0f);
                }

                me->DespawnOrUnsummon(IsMythic() ? 13 * TimeConstants::IN_MILLISECONDS : 3 * TimeConstants::IN_MILLISECONDS);

                me->Relocate(m_FinalPos);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (!me->GetAnyOwner())
                        return;

                    Creature* l_Guldan = me->GetAnyOwner()->ToCreature();
                    Unit*     l_Target = Unit::GetUnit(*me, m_Target);

                    if (!l_Guldan || !l_Target || !l_Guldan->IsAIEnabled)
                        return;

                    boss_guldan::boss_guldanAI* l_AI = CAST_AI(boss_guldan::boss_guldanAI, l_Guldan->GetAI());
                    if (!l_AI)
                        return;

                    if (l_AI->m_EmpoweredLiquidHellfire)
                        me->CastSpell(l_Target, eSpells::SpellEmpoweredLiquidHellfireMissile, true, nullptr, nullptr, l_Guldan->GetGUID());
                    else
                        me->CastSpell(l_Target, eSpells::SpellLiquidHellfireMissile, true, nullptr, nullptr, l_Guldan->GetGUID());
                });

                if (!IsMythic())
                    return;

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!me->GetAnyOwner())
                        return;

                    Creature* l_Guldan = me->GetAnyOwner()->ToCreature();

                    if (!l_Guldan || !l_Guldan->IsAIEnabled)
                        return;

                    boss_guldan::boss_guldanAI* l_AI = CAST_AI(boss_guldan::boss_guldanAI, l_Guldan->GetAI());
                    if (!l_AI || !l_AI->m_EmpoweredLiquidHellfire)
                        return;

                    Player* l_Target = l_AI->SelectRangedTarget();
                    if (!l_Target)
                        return;

                    me->CastSpell(l_Target, eSpells::SpellEmpoweredLiquidHellfireMissile, true, nullptr, nullptr, l_Guldan->GetGUID());
                });

                AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!me->GetAnyOwner())
                        return;

                    Creature* l_Guldan = me->GetAnyOwner()->ToCreature();

                    if (!l_Guldan || !l_Guldan->IsAIEnabled)
                        return;

                    boss_guldan::boss_guldanAI* l_AI = CAST_AI(boss_guldan::boss_guldanAI, l_Guldan->GetAI());
                    if (!l_AI || !l_AI->m_EmpoweredLiquidHellfire)
                        return;

                    Player* l_Target = l_AI->SelectRangedTarget();
                    if (!l_Target)
                        return;

                    me->CastSpell(l_Target, eSpells::SpellEmpoweredLiquidHellfireMissile, true, nullptr, nullptr, l_Guldan->GetGUID());
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_liquid_hellfireAI(p_Creature);
        }
};

/// Fel Lord Kuraz'mal - 104537
class npc_guldan_fel_lord_kurazmal : public CreatureScript
{
    public:
        npc_guldan_fel_lord_kurazmal() : CreatureScript("npc_guldan_fel_lord_kurazmal") { }

        enum eSpells
        {
            SpellFelLord            = 212292,

            SpellFelLordDeath       = 209601,

            SpellShatterEssenceAura = 212519,
            SpellShatterEssence     = 206675,

            SpellFelObeliskDummy    = 210273,
            SpellFelObeliskSummon   = 229946
        };

        enum eEvents
        {
            EventShatterEssence = 1,
            EventFelObelisk
        };

        enum eTalk
        {
            TalkShatterEssence
        };

        struct npc_guldan_fel_lord_kurazmalAI : public ScriptedAI
        {
            npc_guldan_fel_lord_kurazmalAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_CanAttack = false;

            uint8 m_ObeliskCounter = 1;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 3.55f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetPower(me->getPowerType(), 0);

                DoCast(me, eSpells::SpellFelLord, true);

                me->SetInCombatWithZone();
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellFelLord:
                    {
                        m_CanAttack = true;
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventShatterEssence, 20 * TimeConstants::IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(eEvents::EventFelObelisk, 13 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellFelLordDeath, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                me->DespawnCreaturesInArea(eCreatures::NpcFelObelisk, 150.0f);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellFelObeliskDummy)
                {
                    std::list<Unit*> l_Targets;

                    SelectTargetList(l_Targets, 2, SelectAggroTarget::SELECT_TARGET_RANDOM, 0.0f, true, {});

                    for (Unit* l_Unit : l_Targets)
                        DoCast(l_Unit, eSpells::SpellFelObeliskSummon, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() || !m_CanAttack)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventShatterEssence:
                    {
                        Talk(eTalk::TalkShatterEssence);

                        DoCast(me, eSpells::SpellShatterEssenceAura, true);

                        DoCastVictim(eSpells::SpellShatterEssence);

                        events.ScheduleEvent(eEvents::EventShatterEssence, IsMythic() ? 21 * TimeConstants::IN_MILLISECONDS : 52 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFelObelisk:
                    {
                        ++m_ObeliskCounter;

                        DoCast(me, eSpells::SpellFelObeliskDummy, true);

                        if (IsMythic())
                        {
                            if ((m_ObeliskCounter % 2) == 0)
                                events.ScheduleEvent(eEvents::EventFelObelisk, 5 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventFelObelisk, 16 * TimeConstants::IN_MILLISECONDS);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventFelObelisk, 23 * TimeConstants::IN_MILLISECONDS);

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
            return new npc_guldan_fel_lord_kurazmalAI(p_Creature);
        }
};

/// Fel Obelisk - 104569
class npc_guldan_fel_obelisk : public CreatureScript
{
    public:
        npc_guldan_fel_obelisk() : CreatureScript("npc_guldan_fel_obelisk") { }

        enum eSpells
        {
            SpellFelObeliskActivation   = 229952,
            SpellFelObeliskSearcher     = 229947,
            SpellFelObeliskVisual       = 229953,
            SpellFelObeliskVehicle      = 229951
        };

        struct npc_guldan_fel_obeliskAI : public Scripted_NoMovementAI
        {
            npc_guldan_fel_obeliskAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellFelObeliskActivation, true);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellFelObeliskActivation:
                    {
                        DoCast(me, eSpells::SpellFelObeliskSearcher, true);
                        DoCast(me, eSpells::SpellFelObeliskVisual, true);
                        break;
                    }
                    case eSpells::SpellFelObeliskVisual:
                    {
                        me->DespawnOrUnsummon();
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
                    case eSpells::SpellFelObeliskSearcher:
                    {
                        p_Target->CastSpell(me, eSpells::SpellFelObeliskVehicle, true);
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
            return new npc_guldan_fel_obeliskAI(p_Creature);
        }
};

/// Inquisitor Vethriz - 104536
class npc_guldan_inquisitor_vethriz : public CreatureScript
{
    public:
        npc_guldan_inquisitor_vethriz() : CreatureScript("npc_guldan_inquisitor_vethriz") { }

        enum eSpells
        {
            SpellInquisitorFadeIn   = 212293,
            SpellInquisitorFadeOut  = 209637,

            SpellDarkBlast          = 207006,
            SpellDrain              = 212568,

            SpellGazeOfVethrizAllow = 208533,
            SpellGazeOfVethrizCast  = 206840,
            SpellGazeOfVethrizSum   = 207627,

            SpellShadowblik         = 207938
        };

        enum eEvents
        {
            EventDarkBlast = 1,
            EventDrain,
            EventGazeOfVethriz,
            EventShadowblink
        };

        struct npc_guldan_inquisitor_vethrizAI : public Scripted_NoMovementAI
        {
            npc_guldan_inquisitor_vethrizAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            uint32 m_StartTime = 0;
            uint32 m_EnergizeTimer = 0;

            int32 m_PowerCount = 0;

            bool m_CanAttack = false;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->setPowerType(Powers::POWER_ENERGY);

                me->SetPower(me->getPowerType(), 0);

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_OVERRIDE_DISPLAY_POWER_ID, 241);

                DoCast(me, eSpells::SpellInquisitorFadeIn, true);

                m_PowerCount = 0;

                me->SetInCombatWithZone();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventDarkBlast, 2 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventGazeOfVethriz, 15 * TimeConstants::IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(eEvents::EventDrain, 10 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                m_StartTime     = uint32(time(nullptr));
                m_EnergizeTimer = 1 * TimeConstants::IN_MILLISECONDS;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellInquisitorFadeOut, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellInquisitorFadeIn:
                    case eSpells::SpellShadowblik:
                    {
                        m_CanAttack = true;
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
                    case eSpells::SpellGazeOfVethrizCast:
                    {
                        m_StartTime     = uint32(time(nullptr));
                        m_EnergizeTimer = 1 * TimeConstants::IN_MILLISECONDS;
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
                    case eSpells::SpellGazeOfVethrizCast:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        DoCast(p_Target, eSpells::SpellGazeOfVethrizSum, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() || !m_CanAttack)
                    return;

                if (m_EnergizeTimer && !events.HasEvent(eEvents::EventShadowblink))
                {
                    if (m_EnergizeTimer <= p_Diff)
                    {
                        m_PowerCount = me->GetPower(Powers::POWER_ENERGY);

                        if (m_PowerCount < 100)
                        {
                            uint32 l_CurrTime   = uint32(time(nullptr));

                            /// 27s to reach full power
                            int32 l_CurPower    = me->GetPower(Powers::POWER_ENERGY);
                            int32 l_NewPower    = std::min<uint32>((l_CurrTime - m_StartTime) * 100 / 27, 100);

                            if (l_NewPower <= l_CurPower)
                                return;

                            me->SetPower(Powers::POWER_ENERGY, l_NewPower);
                        }
                        else
                        {
                            DoCast(me, eSpells::SpellGazeOfVethrizAllow, true);

                            events.ScheduleEvent(eEvents::EventShadowblink, 1);
                        }

                        m_EnergizeTimer = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_EnergizeTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDarkBlast:
                    {
                        DoCastVictim(eSpells::SpellDarkBlast);

                        events.ScheduleEvent(eEvents::EventDarkBlast, IsHeroicOrMythic() ? 15 * TimeConstants::IN_MILLISECONDS : 2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventDrain:
                    {
                        DoCast(me, eSpells::SpellDrain);

                        events.ScheduleEvent(eEvents::EventDrain, 2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventGazeOfVethriz:
                    {
                        DoCast(me, eSpells::SpellGazeOfVethrizCast);

                        events.ScheduleEvent(eEvents::EventGazeOfVethriz, 16 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventShadowblink:
                    {
                        m_EnergizeTimer = 0;

                        if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                        {
                            m_CanAttack = false;

                            DoCast(l_Player, eSpells::SpellShadowblik);
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
            return new npc_guldan_inquisitor_vethrizAI(p_Creature);
        }
};

/// Gaze of Vethriz - 106986
class npc_guldan_gaze_of_vethriz : public CreatureScript
{
    public:
        npc_guldan_gaze_of_vethriz() : CreatureScript("npc_guldan_gaze_of_vethriz") { }

        enum eSpells
        {
            SpellGazeOfVethrizAT    = 217771
        };

        struct npc_guldan_gaze_of_vethrizAI : public ScriptedAI
        {
            npc_guldan_gaze_of_vethrizAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                DoCast(me, eSpells::SpellGazeOfVethrizAT);

                me->SetInCombatWithZone();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellGazeOfVethrizAT)
                {
                    AddTimedDelayedOperation(1, [this]() -> void
                    {
                        MoveRotate();
                    });
                }
            }

            void LastOperationCalled() override
            {
                AddTimedDelayedOperation(1, [this]() -> void
                {
                    MoveRotate();
                });
            }

            void MoveRotate()
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveRotate(20 * TimeConstants::IN_MILLISECONDS, RotateDirection::ROTATE_DIRECTION_LEFT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_gaze_of_vethrizAI(p_Creature);
        }
};

/// D'zorykx the Trapper - 104534
class npc_guldan_dzorykx_the_trapper : public CreatureScript
{
    public:
        npc_guldan_dzorykx_the_trapper() : CreatureScript("npc_guldan_dzorykx_the_trapper") { }

        enum eSpells
        {
            SpellJailer                     = 212275,
            SpellJailerDeath                = 208831,

            SpellAnguishedSpiritsCast       = 208545,
            SpellAnguishedSpiritsAoE1       = 213897,
            SpellAnguishedSpiritsAoE2       = 213898,
            SpellAnguishedSpiritsMissile    = 216961,
            SpellAnguishedSpiritsDamage     = 208546,

            SpellSoulVortexAllow            = 208581,
            SpellSoulVortexCast             = 206883,
            SpellSoulVortexAT               = 206885,
            SpellSoulVortexDzorykxStacks    = 206765
        };

        enum eEvents
        {
            EventAnguishedSpirits = 1,
            EventSoulVortex
        };

        struct npc_guldan_dzorykx_the_trapperAI : public ScriptedAI
        {
            npc_guldan_dzorykx_the_trapperAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_CanAttack = false;

            bool m_CanModifyPush = false;
            float m_PushModifier = 1.0f;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetPower(me->getPowerType(), 0);

                DoCast(me, eSpells::SpellJailer, true);

                me->SetInCombatWithZone();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventSoulVortex, 10 * TimeConstants::IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(eEvents::EventAnguishedSpirits, 20 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(me, eSpells::SpellJailerDeath, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellJailer:
                    {
                        m_CanAttack = true;
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
                    case eSpells::SpellAnguishedSpiritsCast:
                    {
                        DoCast(me, eSpells::SpellAnguishedSpiritsAoE1, true);
                        DoCast(me, eSpells::SpellAnguishedSpiritsAoE2, true);
                        break;
                    }
                    case eSpells::SpellSoulVortexCast:
                    {
                        /// First cast doesn't increase push force of other casts
                        if (!m_CanModifyPush)
                        {
                            m_CanModifyPush = true;
                            break;
                        }

                        DoCast(me, eSpells::SpellSoulVortexDzorykxStacks, true);

                        m_PushModifier = 1.0f;

                        if (AuraEffect const* l_AurEff = me->GetAuraEffect(eSpells::SpellSoulVortexDzorykxStacks, SpellEffIndex::EFFECT_0))
                            m_PushModifier += float(l_AurEff->GetAmount()) / 100.0f;

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
                    case eSpells::SpellAnguishedSpiritsAoE1:
                    case eSpells::SpellAnguishedSpiritsAoE2:
                    {
                        DoCast(p_Target, eSpells::SpellAnguishedSpiritsMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
            {
                if (p_SpellInfo->Id == eSpells::SpellAnguishedSpiritsMissile)
                    DoCast(p_Dest->_position, eSpells::SpellAnguishedSpiritsDamage, true);
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellSoulVortexAT)
                {
                    if (Unit* l_Guldan = me->GetAnyOwner())
                    {
                        std::list<HostileReference*> l_ThreatList = l_Guldan->getThreatManager().getThreatList();
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = l_Ref->getTarget())
                            {
                                if (!l_Target->IsPlayer())
                                    continue;

                                float l_Magnitude = 2.5f * m_PushModifier;

                                l_Target->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), true, p_AreaTrigger->GetPosition(), l_Magnitude, 1);
                            }
                        }
                    }
                }
            }

            void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool /*p_Removed*/) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellSoulVortexAT)
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Instance->DoRemoveForcedMovementsOnPlayers();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() || !m_CanAttack)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventAnguishedSpirits:
                    {
                        DoCast(me, eSpells::SpellAnguishedSpiritsCast);

                        events.ScheduleEvent(eEvents::EventAnguishedSpirits, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSoulVortex:
                    {
                        DoCast(me, eSpells::SpellSoulVortexAllow, true);
                        DoCastVictim(eSpells::SpellSoulVortexCast);

                        events.ScheduleEvent(eEvents::EventSoulVortex, 21 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_guldan_dzorykx_the_trapperAI(p_Creature);
        }
};

/// Azagrim - 105295
/// Beltheris - 107232
/// Dalvengyr - 107233
class npc_guldan_heroic_dreadlord : public CreatureScript
{
    public:
        npc_guldan_heroic_dreadlord() : CreatureScript("npc_guldan_heroic_dreadlord") { }

        enum eSpells
        {
            SpellDreadlordVisual        = 209127,
            SpellFocusedAssault         = 212625,
            SpellFervor                 = 208626,

            SpellCarrionWaveSearcher    = 208658,
            SpellCarrionWaveCast        = 208672,
            SpellCarrionWaveDamage      = 208671
        };

        enum eEvents
        {
            EventCarrionWave = 1
        };

        struct npc_guldan_heroic_dreadlordAI : public ScriptedAI
        {
            npc_guldan_heroic_dreadlordAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_CanAttack = false;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

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

                DoCast(me, eSpells::SpellDreadlordVisual, true);
                DoCast(me, eSpells::SpellFocusedAssault, true);

                me->SetInCombatWithZone();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventCarrionWave, 5 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE);
            }

            void OnInterruptCast(Unit* /*p_Caster*/, SpellInfo const* /*p_SpellInfo*/, SpellInfo const* /*p_CurrSpellInfo*/, uint32 /*p_SchoolMask*/) override
            {
                DoCast(me, eSpells::SpellFervor, true);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_Mode) override
            {
                if (p_Mode == AuraRemoveMode::AURA_REMOVE_BY_EXPIRE && p_SpellID == eSpells::SpellDreadlordVisual)
                    m_CanAttack = true;
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellCarrionWaveSearcher:
                    {
                        me->SetFacingTo(me->GetAngle(p_Target));

                        DoCast(p_Target, eSpells::SpellCarrionWaveCast);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode) override
            {
                if (p_Mode != SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellCarrionWaveCast:
                    {
                        DoCast(p_Dest->_position, eSpells::SpellCarrionWaveDamage, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim() || !m_CanAttack)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventCarrionWave:
                    {
                        DoCast(me, eSpells::SpellCarrionWaveSearcher, true);

                        events.ScheduleEvent(eEvents::EventCarrionWave, 7 * TimeConstants::IN_MILLISECONDS + 500);
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
            return new npc_guldan_heroic_dreadlordAI(p_Creature);
        }
};

/// Nightwell Entity - 104214
class npc_guldan_nightwell_entity : public CreatureScript
{
    public:
        npc_guldan_nightwell_entity() : CreatureScript("npc_guldan_nightwell_entity") { }

        enum eSpells
        {
            SpellMirrorImageAura        = 179402,

            SpellNightwellEntityScale   = 206225,
            SpellNightwellEntityVisual  = 206255
        };

        struct npc_guldan_nightwell_entityAI : public Scripted_NoMovementAI
        {
            npc_guldan_nightwell_entityAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                p_Summoner->CastSpell(me, eSpells::SpellNightwellEntityScale, true);
                p_Summoner->CastSpell(me, eSpells::SpellMirrorImageAura, true);

                AddTimedDelayedOperation(800, [this]() -> void
                {
                    DoCast(me, eSpells::SpellNightwellEntityVisual, true);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_nightwell_entityAI(p_Creature);
        }
};

/// Eye of Gul'dan <Gul'dan's Minion> - 105630
class npc_guldan_eye_of_guldan : public CreatureScript
{
    public:
        npc_guldan_eye_of_guldan() : CreatureScript("npc_guldan_eye_of_guldan") { }

        enum eEvent
        {
            EventEyeOfGuldan = 1
        };

        enum eSpells
        {
            SpellEyeOfGuldanSpawn       = 211133,
            SpellEyeOfGuldanAura        = 207718,

            SpellValidTarget            = 198294,

            SpellEyeOfGuldanPeriodic    = 209454,
            SpellEyeOfGuldanCast        = 209270,
            SpellEyeOfGuldanEnergize    = 209324,

            SpellDuplicateCast          = 209291,
            SpellDuplicatePowerBurn     = 209296
        };

        enum eAction
        {
            ActionDuplicate
        };

        enum eTalk
        {
            TalkFocused
        };

        struct npc_guldan_eye_of_guldanAI : public ScriptedAI
        {
            npc_guldan_eye_of_guldanAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Duplicating = false;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 8.92353f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                ApplyAllImmunities(true);

                DoCast(me, eSpells::SpellEyeOfGuldanSpawn, true);
                DoCast(me, eSpells::SpellEyeOfGuldanAura, true);

                me->SetPower(Powers::POWER_ENERGY, 0);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellEyeOfGuldanCast)
                {
                    Position l_Pos = me->GetPosition();

                    g_TheDemonWithinPos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 15.0f), frand(0.0f, 2.0f * M_PI), true);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJump(l_Pos, 30.0f, 1.0f, eSpells::SpellEyeOfGuldanPeriodic);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == eSpells::SpellEyeOfGuldanPeriodic)
                {
                    DoCast(me, eSpells::SpellEyeOfGuldanEnergize, true);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();

                        SetCombatMovement(false);

                        events.ScheduleEvent(eEvent::EventEyeOfGuldan, 1);
                    });
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionDuplicate && !m_Duplicating)
                {
                    me->InterruptNonMeleeSpells(true);

                    DoCast(me, eSpells::SpellDuplicateCast);

                    m_Duplicating = true;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellDuplicateCast)
                {
                    m_Duplicating = false;

                    DoCast(me, eSpells::SpellDuplicatePowerBurn, true);

                    if (Unit* l_Owner = me->GetAnyOwner())
                    {
                        if (Creature* l_Duplicate = l_Owner->SummonCreature(me->GetEntry(), me->GetPosition()))
                        {
                            l_Duplicate->AddDelayedEvent([this, l_Duplicate]() -> void
                            {
                                /// Prevent unkillable Eyes if cast finishes right during death (makes Eye with 0 health)
                                if (!me->isAlive())
                                {
                                    l_Duplicate->Kill(l_Duplicate);
                                    return;
                                }

                                l_Duplicate->SetHealth(me->GetHealth());

                                Position l_Pos = l_Duplicate->GetPosition();

                                l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(2.0f, 15.0f), frand(0.0f, 2.0f * M_PI), true);

                                l_Duplicate->GetMotionMaster()->Clear();
                                l_Duplicate->GetMotionMaster()->MoveJump(l_Pos, 30.0f, 1.0f, eSpells::SpellEyeOfGuldanPeriodic);
                            }, 1);
                        }
                    }
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
                    case eEvent::EventEyeOfGuldan:
                    {
                        Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAllRanged, { -int32(eSpells::SpellEyeOfGuldanPeriodic), -int32(eSpells::SpellValidTarget) });
                        if (!l_Target)
                            l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll, { -int32(eSpells::SpellEyeOfGuldanPeriodic), -int32(eSpells::SpellValidTarget) });

                        if (l_Target != nullptr)
                        {
                            DoCast(l_Target, eSpells::SpellValidTarget, true);

                            sCreatureTextMgr->SendChat(me, eTalk::TalkFocused, l_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);

                            DoCast(l_Target, eSpells::SpellEyeOfGuldanPeriodic);
                        }

                        events.ScheduleEvent(eEvent::EventEyeOfGuldan, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_eye_of_guldanAI(p_Creature);
        }
};

/// Bonds of Fel - 104252
class npc_guldan_bonds_of_fel : public CreatureScript
{
    public:
        npc_guldan_bonds_of_fel() : CreatureScript("npc_guldan_bonds_of_fel") { }

        enum eSpells
        {
            SpellBondsOfFelVisual   = 206373,
            SpellBondsOfFelChannel  = 206383
        };

        struct npc_guldan_bonds_of_felAI : public Scripted_NoMovementAI
        {
            npc_guldan_bonds_of_felAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellBondsOfFelVisual, true);
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                if (Player* l_Player = Player::GetPlayer(*me, p_Guid))
                    DoCast(l_Player, eSpells::SpellBondsOfFelChannel, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_bonds_of_felAI(p_Creature);
        }
};

/// Fog Cloud - 116233
class npc_guldan_fog_cloud : public CreatureScript
{
    public:
        npc_guldan_fog_cloud() : CreatureScript("npc_guldan_fog_cloud") { }

        enum eSpells
        {
            SpellPirateCharge       = 198336,
            SpellMythicCallAdds     = 229478,
            SpellFelScytheVisual    = 227571
        };

        struct npc_guldan_fog_cloudAI : public Scripted_NoMovementAI
        {
            npc_guldan_fog_cloudAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellPirateCharge, true);
                DoCast(me, eSpells::SpellMythicCallAdds, true);
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
            {
                if (Player* l_Player = Player::GetPlayer(*me, p_Guid))
                    DoCast(l_Player, eSpells::SpellFelScytheVisual, true);

                me->DespawnOrUnsummon(1);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_fog_cloudAI(p_Creature);
        }
};

/// Empowered Eye of Gul'dan <Gul'dan's Minion> - 106545
class npc_guldan_empowered_eye_of_guldan : public CreatureScript
{
    public:
        npc_guldan_empowered_eye_of_guldan() : CreatureScript("npc_guldan_empowered_eye_of_guldan") { }

        enum eEvent
        {
            EventEyeOfGuldan = 1
        };

        enum eSpells
        {
            SpellEmpoweredEyeOfGuldanSpawn      = 211107,
            SpellEyeOfGuldanAura                = 207718,

            SpellEmpoweredEyeOfGuldanPeriodic   = 209489,
            SpellEmpoweredEyeOfGuldanCast       = 211152,
            SpellEmpoweredEyeOfGuldanVisual     = 221731,
            SpellEmpoweredEyeOfGuldanDamage     = 211132,
            SpellEmpoweredEyeOfGuldanAura       = 221728,

            SpellEyeOfGuldanEnergize            = 209324,
            SpellEyeOfGuldanDamageIncrease      = 209463,

            SpellDuplicateCast                  = 209291,
            SpellDuplicatePowerBurn             = 209296
        };

        enum eAction
        {
            ActionDuplicate
        };

        enum eTalk
        {
            TalkFocused
        };

        struct npc_guldan_empowered_eye_of_guldanAI : public ScriptedAI
        {
            npc_guldan_empowered_eye_of_guldanAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Duplicating = false;

            uint32 m_LastTickTime = 0;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 3.149514f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                ApplyAllImmunities(true);

                DoCast(me, eSpells::SpellEmpoweredEyeOfGuldanSpawn, true);
                DoCast(me, eSpells::SpellEyeOfGuldanAura, true);

                me->SetPower(Powers::POWER_ENERGY, 0);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 3);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->InterruptNonMeleeSpells(true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmpoweredEyeOfGuldanVisual, me->GetGUID());
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEmpoweredEyeOfGuldanAura, me->GetGUID());
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellEmpoweredEyeOfGuldanCast)
                {
                    Position l_Pos = me->GetPosition();

                    g_TheDemonWithinPos.SimplePosXYRelocationByAngle(l_Pos, frand(5.0f, 15.0f), frand(0.0f, 2.0f * M_PI), true);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJump(l_Pos, 30.0f, 1.0f, eSpells::SpellEmpoweredEyeOfGuldanPeriodic);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == eSpells::SpellEmpoweredEyeOfGuldanPeriodic)
                {
                    DoCast(me, eSpells::SpellEyeOfGuldanEnergize, true);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        events.ScheduleEvent(eEvent::EventEyeOfGuldan, 1);
                    });
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionDuplicate && !m_Duplicating)
                {
                    me->InterruptNonMeleeSpells(true);

                    DoCast(me, eSpells::SpellDuplicateCast);

                    m_Duplicating = true;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDuplicateCast:
                    {
                        m_Duplicating = false;

                        DoCast(me, eSpells::SpellDuplicatePowerBurn, true);

                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (Creature* l_Duplicate = l_Owner->SummonCreature(me->GetEntry(), me->GetPosition()))
                            {
                                l_Duplicate->AddDelayedEvent([this, l_Duplicate]() -> void
                                {
                                    /// Prevent unkillable Eyes if cast finishes right during death (makes Eye with 0 health)
                                    if (!me->isAlive())
                                    {
                                        l_Duplicate->Kill(l_Duplicate);
                                        return;
                                    }

                                    l_Duplicate->SetHealth(me->GetHealth());

                                    Position l_Pos = l_Duplicate->GetPosition();

                                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, frand(2.0f, 15.0f), frand(0.0f, 2.0f * M_PI), true);

                                    l_Duplicate->GetMotionMaster()->Clear();
                                    l_Duplicate->GetMotionMaster()->MoveJump(l_Pos, 30.0f, 1.0f, eSpells::SpellEmpoweredEyeOfGuldanPeriodic);
                                }, 1);
                            }
                        }

                        break;
                    }
                    case eSpells::SpellEmpoweredEyeOfGuldanDamage:
                    {
                        /// Safety timer to prevent multiple damage increase at each tick
                        if (m_LastTickTime && uint32(time(nullptr)) < (m_LastTickTime + 100))
                            break;

                        m_LastTickTime = uint32(time(nullptr));

                        DoCast(me, eSpells::SpellEyeOfGuldanDamageIncrease, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellEmpoweredEyeOfGuldanVisual)
                {
                    DoCast(p_Target, p_SpellInfo->Effects[SpellEffIndex::EFFECT_0].TriggerSpell, true);

                    sCreatureTextMgr->SendChat(me, eTalk::TalkFocused, p_Target->GetGUID(), ChatMsg::CHAT_MSG_ADDON, Language::LANG_ADDON, TextRange::TEXT_RANGE_NORMAL);
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
                    case eEvent::EventEyeOfGuldan:
                    {
                        DoCast(me, eSpells::SpellEmpoweredEyeOfGuldanPeriodic);

                        events.ScheduleEvent(eEvent::EventEyeOfGuldan, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_empowered_eye_of_guldanAI(p_Creature);
        }
};

/// Archmage Khadgar - 106522
class npc_guldan_archmage_khadgar : public CreatureScript
{
    public:
        npc_guldan_archmage_khadgar() : CreatureScript("npc_guldan_archmage_khadgar") { }

        enum eActions
        {
            ActionStartMoveFight,
            ActionResetFight,
            ActionTalk20Percents,
            ActionTalk10Percents,
            ActionEndFight
        };

        enum eSpells
        {
            SpellArcaneFlight           = 227671,
            SpellIllidansSoul           = 227674,
            SpellIllidansSoul2          = 231226,

            SpellIllidansSoulVisual1    = 231256,
            SpellIllidansSoulVisual2    = 231262,
            SpellIllidansSoulVisual3    = 231263,
            SpellIllidansSoulVisual4    = 231264,

            SpellGuldanStormCosmetic    = 227682,

            SpellWillOfTheDemonWithin   = 227069,

            /// Fight spells
            SpellArcaneBlast            = 227661,
            SpellArcaneBarrage          = 227662,
            SpellArcaneMissiles         = 227663,
            SpellTimeWarp               = 227665
        };

        enum eEvents
        {
            EventArcaneBlast = 1,
            EventArcaneBarrage,
            EventArcaneMissiles,
            EventTimeWarp
        };

        enum eMove
        {
            MovementFight = 1
        };

        enum eTalks
        {
            TalkP3,
            TalkStormOfTheDestroyer,
            Talk20Percents,
            Talk10Percents
        };

        struct npc_guldan_archmage_khadgarAI : public ScriptedAI
        {
            npc_guldan_archmage_khadgarAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::map<uint32, std::set<uint64>> m_LightsHeartGuids;

            std::array<Position const, eGuldanData::MaxLightsHeart> m_LightsHeartPos =
            {
                {
                    { 268.4727f, 3110.804f, 473.7354f, 1.099469f },
                    { 265.7303f, 3109.346f, 475.8518f, 1.099469f },
                    { 266.1622f, 3106.271f, 477.2982f, 1.099469f },
                    { 268.9046f, 3107.728f, 475.8518f, 1.099469f }
                }
            };

            std::array<uint32, eGuldanData::MaxLightsHeart> m_LightsHeartVisuals =
            {
                {
                    eSpells::SpellIllidansSoulVisual1,
                    eSpells::SpellIllidansSoulVisual2,
                    eSpells::SpellIllidansSoulVisual3,
                    eSpells::SpellIllidansSoulVisual4
                }
            };

            bool m_IsFighting = false;

            void Reset() override { }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionStartMoveFight:
                    {
                        DoCast(me, eSpells::SpellArcaneFlight, true);

                        SetFlyMode(true);

                        std::vector<G3D::Vector3> l_Path =
                        {
                            { 281.7708f, 3173.227f, 374.0981f },
                            { 214.9375f, 3215.140f, 464.8235f },
                            { 198.5556f, 3154.693f, 500.8486f },
                            { 264.2986f, 3102.614f, 465.9386f }
                        };

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothFlyPath(eMove::MovementFight, l_Path.data(), l_Path.size());
                        break;
                    }
                    case eActions::ActionResetFight:
                    {
                        me->RemoveAura(eSpells::SpellWillOfTheDemonWithin);

                        me->InterruptNonMeleeSpells(true);

                        events.Reset();

                        m_IsFighting = false;

                        me->NearTeleportTo(me->GetHomePosition());
                        break;
                    }
                    case eActions::ActionTalk20Percents:
                    {
                        Talk(eTalks::Talk20Percents);
                        break;
                    }
                    case eActions::ActionTalk10Percents:
                    {
                        Talk(eTalks::Talk10Percents);
                        break;
                    }
                    case eActions::ActionEndFight:
                    {
                        me->RemoveAura(eSpells::SpellWillOfTheDemonWithin);

                        me->InterruptNonMeleeSpells(true);

                        events.Reset();

                        m_IsFighting = false;

                        me->NearTeleportTo({ 264.5208f, 3102.599f, 465.9332f, 0.8599688f });

                        me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                auto& l_SetIter = m_LightsHeartGuids[p_Summon->GetEntry()];

                l_SetIter.insert(p_Summon->GetGUID());
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellIllidansSoul && !m_LightsHeartGuids.empty())
                {
                    for (auto const& l_Iter : m_LightsHeartGuids)
                    {
                        for (uint64 const& l_Guid : l_Iter.second)
                        {
                            if (Creature* l_Lights = Creature::GetCreature(*me, l_Guid))
                                l_Lights->DespawnOrUnsummon();
                        }
                    }

                    m_LightsHeartGuids.clear();

                    SetCombatMovement(false);

                    m_IsFighting = true;

                    events.ScheduleEvent(eEvents::EventArcaneBlast, 100);
                    events.ScheduleEvent(eEvents::EventArcaneBarrage, 29 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventArcaneMissiles, 47 * TimeConstants::IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventTimeWarp, 120 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellGuldanStormCosmetic)
                    Talk(eTalks::TalkStormOfTheDestroyer);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellWillOfTheDemonWithin)
                    events.Reset();
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == eMove::MovementFight && p_Type == MovementGeneratorType::EFFECT_MOTION_TYPE)
                {
                    me->RemoveAura(eSpells::SpellArcaneFlight);

                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            if (Creature* l_Prison = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcIllidanStormragePrison)))
                            {
                                me->SetFacingTo(me->GetAngle(l_Prison));

                                DoCast(me, eSpells::SpellIllidansSoul);

                                AddTimedDelayedOperation(10, [this]() -> void
                                {
                                    Talk(eTalks::TalkP3);

                                    DoCast(me, eSpells::SpellIllidansSoul2, true);

                                    AddTimedDelayedOperation(10, [this]() -> void
                                    {
                                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                                        {
                                            auto const& l_Iter = m_LightsHeartGuids.find(eCreatures::NpcLightsHeart);
                                            if (l_Iter != m_LightsHeartGuids.end())
                                            {
                                                if (Creature* l_Heart = Creature::GetCreature(*me, (*l_Iter->second.begin())))
                                                {
                                                    if (Creature* l_Prison = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcIllidanStormragePrison)))
                                                    {
                                                        uint8 l_IDx = 0;
                                                        for (Position const& l_Pos : m_LightsHeartPos)
                                                        {
                                                            if (Creature* l_Heart2 = me->SummonCreature(eCreatures::NpcLightsHeart2, l_Pos))
                                                            {
                                                                l_Heart->CastSpell(l_Heart2, m_LightsHeartVisuals[l_IDx], true);
                                                                l_Heart2->CastSpell(l_Prison, m_LightsHeartVisuals[l_IDx], true);

                                                                ++l_IDx;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    });
                                });
                            }
                        }
                    });
                }
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity = 35.0f;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_IsFighting)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventArcaneBlast:
                    {
                        DoCast(me, eSpells::SpellArcaneBlast);

                        events.ScheduleEvent(eEvents::EventArcaneBlast, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventArcaneBarrage:
                    {
                        DoCast(me, eSpells::SpellArcaneBarrage);

                        events.ScheduleEvent(eEvents::EventArcaneBarrage, 17 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventArcaneMissiles:
                    {
                        DoCast(me, eSpells::SpellArcaneMissiles);

                        events.ScheduleEvent(eEvents::EventArcaneMissiles, 27 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTimeWarp:
                    {
                        DoCast(me, eSpells::SpellTimeWarp);

                        events.ScheduleEvent(eEvents::EventTimeWarp, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_archmage_khadgarAI(p_Creature);
        }
};

/// Light's Heart - 116153
/// Light's Heart - 116156
class npc_guldan_lights_heart : public CreatureScript
{
    public:
        npc_guldan_lights_heart() : CreatureScript("npc_guldan_lights_heart") { }

        enum eSpells
        {
            SpellLightsHeartVisual  = 228367,
            SpellLightsHeartVisual2 = 231242
        };

        struct npc_guldan_lights_heartAI : public Scripted_NoMovementAI
        {
            npc_guldan_lights_heartAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (me->GetEntry() == eCreatures::NpcLightsHeart)
                    DoCast(me, eSpells::SpellLightsHeartVisual, true);
                else
                    DoCast(me, eSpells::SpellLightsHeartVisual2, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_lights_heartAI(p_Creature);
        }
};

/// Well of Souls - 104526
/// Well of Souls - 104594
/// Well of Souls - 105371
class npc_guldan_well_of_souls : public CreatureScript
{
    public:
        npc_guldan_well_of_souls() : CreatureScript("npc_guldan_well_of_souls") { }

        enum eGuidData
        {
            DataDarkSoul,
            DataSoulRemoved
        };

        enum eSpells
        {
            SpellSoulCorrosionMissile   = 212140,
            SpellBlackHarvestCast       = 206744,
            SpellHarvestedSoul          = 209087
        };

        struct npc_guldan_well_of_soulsAI : public Scripted_NoMovementAI
        {
            npc_guldan_well_of_soulsAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            std::map<uint32, uint64> m_WellOfSoulGuids;
            std::map<uint32, uint8> m_AvailableSeats;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (me->GetEntry() != eCreatures::NpcWellOfSoulsMain)
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveRotate(5 * TimeConstants::IN_MILLISECONDS, RotateDirection::ROTATE_DIRECTION_RIGHT);
                    });

                    return;
                }

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Unit* l_Guldan = me->GetAnyOwner())
                    {
                        if (Creature* l_Soul = l_Guldan->SummonCreature(eCreatures::NpcWellOfSouls1, me->GetPosition()))
                        {
                            m_WellOfSoulGuids[l_Soul->GetEntry()] = l_Soul->GetGUID();
                            m_AvailableSeats[l_Soul->GetEntry()] = MAX_VEHICLE_SEATS;

                            l_Soul->EnterVehicle(me);
                        }

                        if (Creature* l_Soul = l_Guldan->SummonCreature(eCreatures::NpcWellOfSouls2, me->GetPosition()))
                        {
                            m_WellOfSoulGuids[l_Soul->GetEntry()] = l_Soul->GetGUID();
                            m_AvailableSeats[l_Soul->GetEntry()] = MAX_VEHICLE_SEATS;

                            l_Soul->EnterVehicle(me);
                        }
                    }
                });
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                switch (p_ID)
                {
                    case eGuidData::DataDarkSoul:
                    {
                        if (m_WellOfSoulGuids.empty() || m_WellOfSoulGuids.size() != 2)
                            return;

                        bool l_Okay = false;
                        if (Creature* l_Soul = Creature::GetCreature(*me, p_Guid))
                        {
                            for (auto& l_Iter : m_AvailableSeats)
                            {
                                if (l_Iter.second > 0)
                                {
                                    uint64 const& l_Guid = m_WellOfSoulGuids[l_Iter.first];

                                    if (Creature* l_Well = Creature::GetCreature(*me, l_Guid))
                                        l_Soul->EnterVehicle(l_Well);

                                    l_Okay = true;

                                    --l_Iter.second;
                                    break;
                                }
                            }

                            if (!l_Okay)
                            {
                                if (Unit* l_Guldan = me->GetAnyOwner())
                                    l_Guldan->CastSpell(l_Soul, eSpells::SpellHarvestedSoul, true);
                            }
                        }

                        break;
                    }
                    case eGuidData::DataSoulRemoved:
                    {
                        uint32 l_Entry = urand(0, 1) ? eCreatures::NpcWellOfSouls1 : eCreatures::NpcWellOfSouls2;

                        if (!RemoveSoul(l_Entry, p_Guid))
                        {
                            if (l_Entry == eCreatures::NpcWellOfSouls1)
                                l_Entry = eCreatures::NpcWellOfSouls2;
                            else
                                l_Entry = eCreatures::NpcWellOfSouls1;

                            RemoveSoul(l_Entry, p_Guid);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != MovementGeneratorType::ROTATE_MOTION_TYPE || me->GetEntry() == eCreatures::NpcWellOfSoulsMain)
                    return;

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveRotate(5 * TimeConstants::IN_MILLISECONDS, RotateDirection::ROTATE_DIRECTION_RIGHT);
                });
            }

            bool RemoveSoul(uint32 p_Entry, uint64 p_TargetGUID)
            {
                Player* l_Player = Player::GetPlayer(*me, p_TargetGUID);
                if (!l_Player)
                    return false;

                if (m_WellOfSoulGuids.find(p_Entry) != m_WellOfSoulGuids.end())
                {
                    if (Creature* l_Passenger = Creature::GetCreature(*me, m_WellOfSoulGuids[p_Entry]))
                    {
                        if (Vehicle* l_Vehicle = l_Passenger->GetVehicleKit())
                        {
                            for (int8 l_I = (MAX_VEHICLE_SEATS - 1); l_I >= 0; --l_I)
                            {
                                if (Unit* l_Soul = l_Vehicle->GetPassenger(l_I))
                                {
                                    if (l_Soul->ToCreature())
                                    {
                                        l_Soul->CastSpell(l_Player, eSpells::SpellSoulCorrosionMissile, true);
                                        l_Soul->ExitVehicle();
                                        l_Soul->RemoveAllAuras();

                                        if (m_AvailableSeats.find(p_Entry) != m_AvailableSeats.end())
                                        {
                                            auto& l_Iter = m_AvailableSeats[p_Entry];

                                            --l_Iter;
                                        }

                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }

                return false;
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellBlackHarvestCast)
                {
                    Unit* l_Guldan = me->GetAnyOwner();
                    if (!l_Guldan)
                        return;

                    for (auto const& l_Iter : m_WellOfSoulGuids)
                    {
                        if (Creature* l_Well = Creature::GetCreature(*me, l_Iter.second))
                        {
                            if (Vehicle* l_Vehicle = l_Well->GetVehicleKit())
                            {
                                for (int8 l_I = 0; l_I < MAX_VEHICLE_SEATS; ++l_I)
                                {
                                    if (Unit* l_Soul = l_Vehicle->GetPassenger(l_I))
                                    {
                                        l_Guldan->CastSpell(l_Soul->GetPosition(), eSpells::SpellHarvestedSoul, true);

                                        if (l_Soul->ToCreature())
                                            l_Soul->ToCreature()->DespawnOrUnsummon();
                                    }
                                }
                            }
                        }
                    }

                    for (auto& l_Iter : m_AvailableSeats)
                        l_Iter.second = MAX_VEHICLE_SEATS;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_well_of_soulsAI(p_Creature);
        }
};

/// Dark Soul - 110688
class npc_guldan_dark_soul : public CreatureScript
{
    public:
        npc_guldan_dark_soul() : CreatureScript("npc_guldan_dark_soul") { }

        enum eSpells
        {
            SpellDarkSoulVisual         = 206227,

            SpellSoulCorrosionMissile   = 212140,
            SpellSoulCorrosionAura      = 208802
        };

        enum eGuidData
        {
            DataDarkSoul
        };

        struct npc_guldan_dark_soulAI : public Scripted_NoMovementAI
        {
            npc_guldan_dark_soulAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellDarkSoulVisual, true);

                if (Creature* l_Well = me->FindNearestCreature(eCreatures::NpcWellOfSoulsMain, 100.0f))
                {
                    if (l_Well->IsAIEnabled)
                        l_Well->AI()->SetGUID(me->GetGUID(), eGuidData::DataDarkSoul);
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellSoulCorrosionMissile:
                    {
                        if (Unit* l_Guldan = me->GetAnyOwner())
                            l_Guldan->CastSpell(p_Target, eSpells::SpellSoulCorrosionAura, true);

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
            return new npc_guldan_dark_soulAI(p_Creature);
        }
};

/// The Demon Within - 111022
class npc_guldan_the_demon_within : public CreatureScript
{
    public:
        npc_guldan_the_demon_within() : CreatureScript("npc_guldan_the_demon_within") { }

        enum eSpells
        {
            /// Misc
            SpellGuldanSpawnVisual      = 210969,
            SpellWillOfTheDemonWithin   = 211439,
            SpellSoulseverNext          = 229219,
            SpellFlameCrashNext         = 229216,
            SpellDemonbane              = 221486,
            SpellWounded                = 227009,
            /// Parasitic Wound
            SpellParasiticWoundDummy    = 227035,
            SpellParasiticWoundDoT      = 206847,
            /// Soulsever
            SpellSoulseverSearcher      = 227293,
            SpellSoulseverCast          = 220957,
            SpellSoulseverMissile       = 208830,
            SpellSoulseverAreaTrigger   = 208229,
            SpellShearedSoulATMissile   = 206822,
            SpellShearedSoulAbsorb      = 206458,
            /// Flame Crash
            SpellFlameCrashSearcher     = 227071,
            SpellFlameCrashTriggered    = 227094,
            SpellFlameCrashAuraBack     = 227107,
            SpellFlameCrashBack         = 227106,
            SpellFlameCrashTeleportBack = 227114,
            /// Nightorb
            SpellSummonNightorbSummon   = 227283,
            /// Manifest Azzinoth
            SpellManifestAzzinothSpawn  = 221149, ///< Both are exactly the same, addons handle both
            SpellManifestAzzinothSpawn2 = 227277, ///< Both are exactly the same, addons handle both
            /// Visions of the Dark Titan
            SpellVisionsPeriodicAura    = 227008,
            SpellVisionsScreen          = 227032,
            SpellVisionsTransform       = 226982,
            SpellVisionsImmunity        = 226975,
            SpellVisionsDummyAura       = 227007,
            SpellVisionsDummyAura2      = 227004
        };

        enum eEvents
        {
            EventParasiticWound = 1,
            EventSoulsever,
            EventManifestAzzinoth,
            EventFlameCrash,
            EventSummonNightorb,
            EventVisionsOfTheDarkTitan
        };

        enum eTalks
        {
            TalkIntro1,
            TalkIntro2,
            TalkIntro3,
            TalkParasiticWound,
            TalkManifestAzzinoth,
            TalkVisionsOfTheDarkTitan,
            TalkSlayPlayer,
            TalkWipe,
            TalkDeath
        };

        enum eAction
        {
            ActionFullEnergy
        };

        struct npc_guldan_the_demon_withinAI : public ScriptedAI
        {
            npc_guldan_the_demon_withinAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint32 m_NextPowerEvent = eSpells::SpellSoulseverNext;

            std::map<uint64, uint64> m_ShearedSoulGuids;

            uint8 m_NightorbCounter = 0;
            uint8 m_VisionsCounter = 1;

            std::array<Position const, eGuldanData::MaxNightorbs> m_NightorbPos =
            {
                {
                    { 312.8096f, 3118.838f, 465.9068f, 6.113874f },
                    { 275.5196f, 3101.449f, 465.9190f, 4.292622f },
                    { 247.4588f, 3131.541f, 465.9067f, 3.180216f },
                    { 295.3154f, 3164.029f, 465.9189f, 1.241087f }
                }
            };

            std::array<Position const, eGuldanData::MaxVisionsDummy> m_VisionsDummyPos =
            {
                {
                    { 208.2875f, 3166.421f, 470.9338f, 5.890486f },
                    { 251.5832f, 3209.717f, 470.9338f, 5.105088f },
                    { 312.8126f, 3209.717f, 470.9338f, 4.319691f },
                    { 356.1083f, 3166.421f, 470.9338f, 3.534293f },
                    { 356.1083f, 3105.192f, 470.9338f, 2.748893f },
                    { 312.8126f, 3061.896f, 470.9338f, 1.963495f },
                    { 251.5832f, 3061.896f, 470.9338f, 1.178097f },
                    { 208.2875f, 3105.192f, 470.9338f, 0.392699f }
                }
            };

            Position const m_VisionsPos         = { 172.1979f, 3135.806f, 480.9338f, 0.0f };
            Position const m_VisionsFinalPos    = { 282.1979f, 3135.806f, 477.9338f, 0.0f };
            Position const m_VisionsEndPos      = { 282.1979f, 3135.806f, 465.8505f, 0.0f };

            bool m_CastingVisions = false;

            void Reset() override
            {
                me->UpdateStatsForLevel();

                SetFlyMode(false);

                m_NextPowerEvent = eSpells::SpellSoulseverNext;

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetPower(Powers::POWER_ENERGY, 0);

                me->LoadEquipment(1, true);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISARM_OFFHAND);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                DoCast(me, eSpells::SpellGuldanSpawnVisual, true);

                Talk(eTalks::TalkIntro1);

                AddTimedDelayedOperation(12 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::TalkIntro2);
                });
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 1.0f;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eEvents::EventParasiticWound, 8 * TimeConstants::IN_MILLISECONDS + 600);
                events.ScheduleEvent(eEvents::EventManifestAzzinoth, 26 * TimeConstants::IN_MILLISECONDS + 600);
                events.ScheduleEvent(eEvents::EventSummonNightorb, 39 * TimeConstants::IN_MILLISECONDS + 600);
                events.ScheduleEvent(eEvents::EventVisionsOfTheDarkTitan, 96 * TimeConstants::IN_MILLISECONDS + 200);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me);
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(eTalks::TalkSlayPlayer);
            }

            void JustDespawned() override
            {
                Talk(eTalks::TalkWipe);

                summons.DespawnAll();

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticWoundDoT);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellShearedSoulAbsorb);
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(eTalks::TalkDeath);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellParasiticWoundDoT);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellShearedSoulAbsorb);

                    Player* l_Killer = p_Killer->IsPlayer() ? p_Killer->ToPlayer() : nullptr;
                    if (!l_Killer && p_Killer->GetOwner())
                        l_Killer = p_Killer->GetOwner() ? p_Killer->GetOwner()->ToPlayer() : nullptr;

                    if (!l_Killer)
                        return;

                    if (Creature* l_Guldan = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcGulDan)))
                        l_Killer->Kill(l_Guldan);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                summons.Summon(p_Summon);
            }

            void SummonedCreatureDespawn(Creature* p_Summon) override
            {
                summons.Despawn(p_Summon);
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                switch (p_SpellID)
                {
                    case eSpells::SpellGuldanSpawnVisual:
                    {
                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            Talk(eTalks::TalkIntro3);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                            me->SetInCombatWithZone();

                            AddTimedDelayedOperation(9 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                DoCast(me, eSpells::SpellSoulseverNext, true);
                            });

                            DoCast(me, eSpells::SpellWillOfTheDemonWithin);
                        });

                        break;
                    }
                    case eSpells::SpellVisionsPeriodicAura:
                    {
                        EndVisionsOfTheDarkTitan();
                        break;
                    }
                    case eSpells::SpellWounded:
                    {
                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(me, m_NextPowerEvent, true);
                        });

                        break;
                    }
                    case eSpells::SpellFlameCrashAuraBack:
                    {
                        if (Unit* l_Target = me->getVictim())
                            DoCast(l_Target, eSpells::SpellFlameCrashBack, true);

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
                    case eSpells::SpellWillOfTheDemonWithin:
                    {
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISARM_OFFHAND);
                        break;
                    }
                    case eSpells::SpellParasiticWoundDummy:
                    {
                        std::list<Player*> l_Targets;

                        SelectRangedTargetList(l_Targets);

                        if (l_Targets.size() > 2)
                            JadeCore::Containers::RandomResizeList(l_Targets, 2);

                        for (Player* l_Target : l_Targets)
                            DoCast(l_Target, eSpells::SpellParasiticWoundDoT, true);

                        break;
                    }
                    case eSpells::SpellFlameCrashTeleportBack:
                    {
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (Unit* l_Target = me->getVictim())
                            {
                                AttackStart(l_Target);

                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_Target);
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
                switch (p_Action)
                {
                    /// The Demon Within alternates casting Soulsever and Flame Crash every time he reaches full energy, initially doing a Soulsever.
                    case eAction::ActionFullEnergy:
                    {
                        if (!events.HasEvent(eEvents::EventSoulsever) && !events.HasEvent(eEvents::EventFlameCrash))
                        {
                            me->RemoveAura(m_NextPowerEvent);

                            if (m_NextPowerEvent == eSpells::SpellFlameCrashNext)
                            {
                                m_NextPowerEvent = eSpells::SpellSoulseverNext;

                                events.ScheduleEvent(eEvents::EventFlameCrash, 1);
                            }
                            else
                            {
                                m_NextPowerEvent = eSpells::SpellFlameCrashNext;

                                events.ScheduleEvent(eEvents::EventSoulsever, 1);
                            }

                            DoCast(me, m_NextPowerEvent, true);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellSoulseverSearcher:
                    {
                        p_Targets.clear();

                        if (Unit* l_Target = me->getVictim())
                            p_Targets.push_back(l_Target);

                        break;
                    }
                    case eSpells::SpellFlameCrashSearcher:
                    {
                        if (p_Targets.empty())
                            break;

                        /// Furthest target first
                        p_Targets.sort(JadeCore::ObjectDistanceOrderPred(me, false));

                        WorldObject* l_Obj = p_Targets.front();

                        p_Targets.clear();
                        p_Targets.push_back(l_Obj);
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
                    case eSpells::SpellSoulseverSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellSoulseverCast);
                        break;
                    }
                    case eSpells::SpellSoulseverCast:
                    {
                        DoCast(p_Target, eSpells::SpellSoulseverMissile, true);
                        DoCast(p_Target, eSpells::SpellShearedSoulAbsorb, true);
                        break;
                    }
                    case eSpells::SpellSoulseverMissile:
                    {
                        Position l_Pos = me->GetPosition();

                        float l_Angle = me->GetAngle(p_Target) - M_PI / 2.0f;
                        float l_OStep = M_PI / 9.0f;

                        for (uint8 l_I = 0; l_I < 9; ++l_I)
                        {
                            l_Pos.m_orientation = l_Angle;

                            DoCast(l_Pos, eSpells::SpellSoulseverAreaTrigger, true);

                            l_Angle += l_OStep;
                        }

                        break;
                    }
                    case eSpells::SpellShearedSoulAbsorb:
                    {
                        Position l_Pos = me->GetPosition();

                        float l_Angle = me->GetAngle(p_Target);

                        p_Target->SimplePosXYRelocationByAngle(l_Pos, 8.5f, l_Angle, true);

                        DoCast(l_Pos, eSpells::SpellShearedSoulATMissile, true);

                        if (Creature* l_Soul = me->SummonCreature(eCreatures::NpcSeveredSoul, l_Pos))
                        {
                            if (l_Soul->IsAIEnabled)
                                l_Soul->AI()->SetGUID(p_Target->GetGUID());
                        }

                        break;
                    }
                    case eSpells::SpellFlameCrashSearcher:
                    {
                        DoCast(p_Target, eSpells::SpellFlameCrashTriggered, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellDemonbane && m_CastingVisions)
                    EndVisionsOfTheDarkTitan(true);
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
                    case eEvents::EventParasiticWound:
                    {
                        Talk(eTalks::TalkParasiticWound);

                        DoCast(me, eSpells::SpellParasiticWoundDummy, true);

                        events.ScheduleEvent(eEvents::EventParasiticWound, 36 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSoulsever:
                    {
                        DoCast(me, eSpells::SpellSoulseverSearcher, true);
                        DoCastVictim(eSpells::SpellSoulseverCast);
                        break;
                    }
                    case eEvents::EventManifestAzzinoth:
                    {
                        Talk(eTalks::TalkManifestAzzinoth);

                        Position l_Pos = m_VisionsEndPos;

                        m_VisionsEndPos.SimplePosXYRelocationByAngle(l_Pos, frand(15.0f, 50.0f), frand(0.0f, 2.0f * M_PI), true);

                        DoCast(l_Pos, urand(0, 1) ? eSpells::SpellManifestAzzinothSpawn : eSpells::SpellManifestAzzinothSpawn2, true);

                        events.ScheduleEvent(eEvents::EventManifestAzzinoth, 41 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFlameCrash:
                    {
                        DoCast(me, eSpells::SpellFlameCrashSearcher, true);
                        break;
                    }
                    case eEvents::EventSummonNightorb:
                    {
                        Position const& l_Pos = m_NightorbPos[m_NightorbCounter++];

                        DoCast(l_Pos, eSpells::SpellSummonNightorbSummon, true);

                        if (m_NightorbCounter < 4)
                        {
                            if (m_NightorbCounter == 2)
                                events.ScheduleEvent(eEvents::EventSummonNightorb, 60 * TimeConstants::IN_MILLISECONDS);
                            else if (m_NightorbCounter == 3)
                                events.ScheduleEvent(eEvents::EventSummonNightorb, 40 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventSummonNightorb, 45 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eEvents::EventVisionsOfTheDarkTitan:
                    {
                        Talk(eTalks::TalkVisionsOfTheDarkTitan);

                        me->RemoveAura(eSpells::SpellSoulseverNext);
                        me->RemoveAura(eSpells::SpellFlameCrashNext);

                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->SetTarget(0);
                        me->StopAttack();

                        me->SetPower(Powers::POWER_ENERGY, 0);

                        SetFlyMode(true);

                        Position l_Pos = m_VisionsPos;

                        l_Pos.m_positionZ -= 205.0f;

                        me->NearTeleportTo(l_Pos);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        me->LoadEquipment(0, true);

                        DoCast(me, eSpells::SpellVisionsTransform, true);
                        DoCast(me, eSpells::SpellVisionsImmunity, true);

                        AddTimedDelayedOperation(1, [this]() -> void
                        {
                            if (InstanceScript* l_Instance = me->GetInstanceScript())
                                l_Instance->DoCastSpellOnPlayers(eSpells::SpellVisionsScreen);

                            AddTimedDelayedOperation(1, [this]() -> void
                            {
                                m_CastingVisions = true;

                                DoCast(me, eSpells::SpellVisionsPeriodicAura);
                            });
                        });

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(me, eSpells::SpellVisionsDummyAura, true);

                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                DoCast(me, eSpells::SpellVisionsDummyAura2, true);
                            });
                        });

                        uint32 l_Time = 5 * TimeConstants::IN_MILLISECONDS;

                        for (Position const l_Pos : m_VisionsDummyPos)
                        {
                            AddTimedDelayedOperation(l_Time, [this, l_Pos]() -> void
                            {
                                me->SummonCreature(eCreatures::NpcVisionsDummy, l_Pos);
                            });

                            l_Time += 600;
                        }

                        AddTimedDelayedOperation(l_Time, [this]() -> void
                        {
                            me->SummonCreature(eCreatures::NpcVisionsFinal, m_VisionsFinalPos);
                        });

                        ++m_VisionsCounter;

                        if (m_VisionsCounter < 4)
                        {
                            if (m_VisionsCounter == 3)
                                events.ScheduleEvent(eEvents::EventVisionsOfTheDarkTitan, 150 * TimeConstants::IN_MILLISECONDS);
                            else
                                events.ScheduleEvent(eEvents::EventVisionsOfTheDarkTitan, 90 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void EndVisionsOfTheDarkTitan(bool p_Stunned = false)
            {
                m_CastingVisions = false;

                SetFlyMode(false);

                me->RemoveAura(eSpells::SpellVisionsTransform);
                me->RemoveAura(eSpells::SpellVisionsImmunity);
                me->RemoveAura(eSpells::SpellVisionsDummyAura);
                me->RemoveAura(eSpells::SpellVisionsDummyAura2);

                me->InterruptNonMeleeSpells(true);

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                me->DespawnCreaturesInArea({ eCreatures::NpcVisionsDummy, eCreatures::NpcVisionsFinal });

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVisionsScreen);

                if (p_Stunned)
                    DoCast(me, eSpells::SpellWounded, true);
                else
                    DoCast(me, m_NextPowerEvent, true);

                ClearDelayedOperations();

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->LoadEquipment(1, true);

                me->NearTeleportTo(m_VisionsEndPos);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_the_demon_withinAI(p_Creature);
        }
};

/// Parasitic Shadow Demon - 111047
class npc_guldan_parasitic_shadow_demon : public CreatureScript
{
    public:
        npc_guldan_parasitic_shadow_demon() : CreatureScript("npc_guldan_parasitic_shadow_demon") { }

        enum eSpells
        {
            SpellSlickHusk          = 227292,
            SpellShadowShroud       = 206779,
            SpellShadowyGaze        = 206983,
            SpellParasiticWoundDoT  = 206847
        };

        struct npc_guldan_parasitic_shadow_demonAI : public ScriptedAI
        {
            npc_guldan_parasitic_shadow_demonAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_FixateTarget = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellSlickHusk, true);
                DoCast(me, eSpells::SpellShadowShroud, true);

                me->SetInCombatWithZone();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_FixateTarget = p_Guid;

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (Player* l_Player = sObjectAccessor->FindPlayer(m_FixateTarget))
                    {
                        me->DeleteThreatList();

                        me->AddThreat(l_Player, std::numeric_limits<float>::max());
                        me->TauntApply(l_Player);

                        DoCast(l_Player, eSpells::SpellShadowyGaze, true);
                    }
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_FixateTarget)
                {
                    if (Player* l_Player = sObjectAccessor->FindPlayer(m_FixateTarget))
                    {
                        if (l_Player->isDead())
                        {
                            m_FixateTarget = 0;

                            if (Player* l_Target = SelectRangedTarget(true, -(eSpells::SpellParasiticWoundDoT)))
                                SetGUID(l_Target->GetGUID(), 0);
                        }
                        else if (me->IsWithinMeleeRange(l_Player))
                        {
                            if (Unit* l_Demon = me->GetAnyOwner())
                            {
                                m_FixateTarget = 0;

                                l_Demon->CastSpell(l_Player, eSpells::SpellParasiticWoundDoT, true);

                                if (Player* l_Target = SelectRangedTarget(true, -(eSpells::SpellParasiticWoundDoT)))
                                    SetGUID(l_Target->GetGUID(), 0);
                            }
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_parasitic_shadow_demonAI(p_Creature);
        }
};

/// Severed Soul - 107229
class npc_guldan_severed_soul : public CreatureScript
{
    public:
        npc_guldan_severed_soul() : CreatureScript("npc_guldan_severed_soul") { }

        enum eSpells
        {
            SpellCloneCaster            = 60352,
            SpellSeveredSoulScale       = 229534,
            SpellShearedSoulAbsorb      = 206458,
            SpellShearedSoulAfterAbsorb = 206506,
            SpellSeveredAoE             = 227040
        };

        enum eVisual
        {
            VisualSeveredSoul = 61112
        };

        struct npc_guldan_severed_soulAI : public Scripted_NoMovementAI
        {
            npc_guldan_severed_soulAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            uint64 m_PlayerGuid = 0;

            bool m_Soaked = false;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                m_PlayerGuid = p_Guid;

                AddTimedDelayedOperation(1, [this]() -> void
                {
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_MIRROR_IMAGE);

                    if (Player* l_Player = Player::GetPlayer(*me, m_PlayerGuid))
                    {
                        me->SetDisplayId(l_Player->GetDisplayId());

                        me->SetFloatValue(EUnitFields::UNIT_FIELD_BOUNDING_RADIUS, l_Player->GetFloatValue(EUnitFields::UNIT_FIELD_BOUNDING_RADIUS));
                        me->SetFloatValue(EUnitFields::UNIT_FIELD_COMBAT_REACH, l_Player->GetFloatValue(EUnitFields::UNIT_FIELD_COMBAT_REACH));
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_SEX, l_Player->GetUInt32Value(EUnitFields::UNIT_FIELD_SEX));

                        l_Player->CastSpell(me, eSpells::SpellCloneCaster, true);
                        l_Player->CastSpell(me, eSpells::SpellSeveredSoulScale, true);
                    }
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_Soaked)
                    return;

                if (Player* l_Player = me->SelectNearestPlayerNotGM(1.0f))
                {
                    /// Can't be soaked by affected player
                    if (l_Player->GetGUID() == m_PlayerGuid)
                        return;

                    Player* l_Target = Player::GetPlayer(*me, m_PlayerGuid);
                    if (!l_Target)
                        return;

                    m_Soaked = true;

                    /// Soul going back to tank
                    l_Target->SendPlaySpellVisual(eVisual::VisualSeveredSoul, nullptr, 0.5f, me->GetPosition(), true);

                    l_Target->RemoveAura(eSpells::SpellShearedSoulAbsorb);

                    if (Unit* l_Demon = me->GetAnyOwner())
                    {
                        l_Demon->CastSpell(l_Target, eSpells::SpellShearedSoulAfterAbsorb, true);
                        l_Demon->CastSpell(l_Player, eSpells::SpellSeveredAoE, true);
                    }

                    me->RemoveAllAreasTrigger();
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_severed_soulAI(p_Creature);
        }
};

/// Nightorb - 111054
class npc_guldan_nightorb : public CreatureScript
{
    public:
        npc_guldan_nightorb() : CreatureScript("npc_guldan_nightorb") { }

        enum eSpells
        {
            SpellNightorbVisual     = 208862,
            SpellNightshieldAbsorb  = 221292,
            SpellInstability        = 227250,
            SpellDistortionAura     = 208801,
            SpellTimeStopField      = 211832,
            SpellCapriciousBarrage  = 206912
        };

        enum eEvent
        {
            EventCapriciousBarrage = 1
        };

        struct npc_guldan_nightorbAI : public Scripted_NoMovementAI
        {
            npc_guldan_nightorbAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                ApplyAllImmunities(true);

                DoCast(me, eSpells::SpellNightorbVisual, true);
                DoCast(me, eSpells::SpellNightshieldAbsorb, true);
                DoCast(me, eSpells::SpellInstability, true);
                DoCast(me, eSpells::SpellDistortionAura, false);

                events.ScheduleEvent(eEvent::EventCapriciousBarrage, 9 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Unit* l_Demon = me->GetAnyOwner())
                    l_Demon->CastSpell(me, eSpells::SpellTimeStopField, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eEvent::EventCapriciousBarrage)
                {
                    DoCastVictim(eSpells::SpellCapriciousBarrage);

                    events.ScheduleEvent(eEvent::EventCapriciousBarrage, 6 * TimeConstants::IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_nightorbAI(p_Creature);
        }
};

/// Soul Fragment of Azzinoth - 111070
class npc_guldan_soul_fragment_of_azzinoth : public CreatureScript
{
    public:
        npc_guldan_soul_fragment_of_azzinoth() : CreatureScript("npc_guldan_soul_fragment_of_azzinoth") { }

        enum eSpells
        {
            /// Misc
            SpellManifestAzzinothVisual = 221148,
            SpellFervor                 = 208626,
            /// Chaos Seed
            SpellChaosSeedPeriodic      = 221336,
            SpellChaosSeedFilter        = 221337,
            SpellChaosSeedMissile       = 221317,
            SpellChaosSeedVisual        = 221382,
            /// Bulwark of Azzinoth
            SpellBulwarkOfAzzinothCast  = 221408,
            SpellBulwarkOfAzzinothAura  = 221414,
            SpellBulwarkOfAzzinothSteal = 221299,
            SpellMentalConditioning     = 221302,
            /// Demonic Essence
            SpellDemonicEssenceSummon   = 221434
        };

        enum eEvents
        {
            EventFervor = 1,
            EventChaosSeed,
            EventBulwarkOfAzzinoth
        };

        struct npc_guldan_soul_fragment_of_azzinothAI : public ScriptedAI
        {
            npc_guldan_soul_fragment_of_azzinothAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellManifestAzzinothVisual, true);

                me->SetInCombatWithZone();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoCast(me, eSpells::SpellFervor, true);

                events.ScheduleEvent(eEvents::EventFervor, 10 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventChaosSeed, 3 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBulwarkOfAzzinoth, 15 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::SpellChaosSeedFilter:
                    {
                        p_Targets.clear();

                        if (Player* l_Ranged = SelectRangedTarget())
                            p_Targets.push_back(l_Ranged);
                        else if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            p_Targets.push_back(l_Target);

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
                    case eSpells::SpellChaosSeedFilter:
                    {
                        DoCast(p_Target, eSpells::SpellChaosSeedVisual, true);
                        DoCast(p_Target, eSpells::SpellChaosSeedMissile, true);
                        break;
                    }
                    case eSpells::SpellBulwarkOfAzzinothCast:
                    {
                        DoCast(me, eSpells::SpellMentalConditioning, true);
                        DoCast(me, eSpells::SpellBulwarkOfAzzinothAura, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Unit* l_Demon = me->GetAnyOwner())
                    l_Demon->CastSpell(me, eSpells::SpellDemonicEssenceSummon, true);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            bool OnSpellSteal(Unit* p_Caster, uint32 p_SpellID) override
            {
                if (p_SpellID == eSpells::SpellBulwarkOfAzzinothAura)
                {
                    uint64 l_Guid = p_Caster->GetGUID();
                    AddTimedDelayedOperation(1, [this, l_Guid]() -> void
                    {
                        me->RemoveAura(eSpells::SpellBulwarkOfAzzinothAura);

                        if (Unit* l_Caster = Unit::GetUnit(*me, l_Guid))
                            l_Caster->CastSpell(l_Caster, eSpells::SpellBulwarkOfAzzinothSteal, true);
                    });

                    return true;
                }

                return false;
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
                    case eEvents::EventFervor:
                    {
                        DoCast(me, eSpells::SpellFervor, true);

                        events.ScheduleEvent(eEvents::EventFervor, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventChaosSeed:
                    {
                        DoCast(me, eSpells::SpellChaosSeedPeriodic);

                        events.ScheduleEvent(eEvents::EventChaosSeed, 10 * TimeConstants::IN_MILLISECONDS + 900);
                        break;
                    }
                    case eEvents::EventBulwarkOfAzzinoth:
                    {
                        DoCast(me, eSpells::SpellBulwarkOfAzzinothCast);
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
            return new npc_guldan_soul_fragment_of_azzinothAI(p_Creature);
        }
};

/// Demonic Essence - 111251
class npc_guldan_demonic_essence_vehicle : public CreatureScript
{
    public:
        npc_guldan_demonic_essence_vehicle() : CreatureScript("npc_guldan_demonic_essence_vehicle") { }

        struct npc_guldan_demonic_essence_vehicleAI : public Scripted_NoMovementAI
        {
            npc_guldan_demonic_essence_vehicleAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                Position l_Pos = me->GetPosition();

                l_Pos.m_positionZ += 4.0f;

                if (Creature* l_Essence = p_Summoner->SummonCreature(eCreatures::NpcDemonicEssence, l_Pos))
                {
                    uint64 l_Guid = l_Essence->GetGUID();
                    AddTimedDelayedOperation(1, [this, l_Guid]() -> void
                    {
                        if (Creature* l_Essence = Creature::GetCreature(*me, l_Guid))
                            l_Essence->EnterVehicle(me);
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
            return new npc_guldan_demonic_essence_vehicleAI(p_Creature);
        }
};

/// Demonic Essence - 111222
class npc_guldan_demonic_essence : public CreatureScript
{
    public:
        npc_guldan_demonic_essence() : CreatureScript("npc_guldan_demonic_essence") { }

        enum eSpells
        {
            SpellDemonicEssenceVisual   = 221431,
            SpellDemonicEssenceAT       = 221436,
            SpellDemonicEssenceAT2      = 229293,
            SpellDemonicEssenceDoT      = 221437,
            SpellPurifiedEssenceCast    = 221486
        };

        enum eVisuals
        {
            VisalShadow = 59311,
            VisualLight = 62221
        };

        struct npc_guldan_demonic_essenceAI : public Scripted_NoMovementAI
        {
            npc_guldan_demonic_essenceAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            uint64 m_LightOrbGuid = 0;
            uint64 m_ShadowOrbGuid = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->DisableHealthRegen();

                me->SetHealth(1);

                ApplyAllImmunities(true);

                Position l_Pos = me->GetPosition();

                l_Pos.m_positionZ = 467.0f;

                DoCast(l_Pos, eSpells::SpellDemonicEssenceAT, true);
            }

            void OnVehicleEntered(Unit* /*p_Vehicle*/) override
            {
                AddTimedDelayedOperation(1, [this]() -> void
                {
                    DoCast(me, eSpells::SpellDemonicEssenceVisual, true);
                });
            }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                switch (p_AreaTrigger->GetSpellId())
                {
                    case eSpells::SpellDemonicEssenceAT2:
                    {
                        m_LightOrbGuid = p_AreaTrigger->GetGUID();
                        break;
                    }
                    case eSpells::SpellDemonicEssenceVisual:
                    {
                        m_ShadowOrbGuid = p_AreaTrigger->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellPurifiedEssenceCast)
                {
                    AreaTrigger* l_Shadow = sObjectAccessor->GetAreaTrigger(*me, m_ShadowOrbGuid);
                    if (!l_Shadow)
                        return;

                    l_Shadow->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, l_Shadow->GetLiveTime());
                    l_Shadow->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 2.0f);
                    l_Shadow->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 0.01f);

                    l_Shadow->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 250);

                    for (uint8 l_I = 0; l_I < 7; ++l_I)
                        l_Shadow->ForceValuesUpdateAtIndex(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

                    AreaTrigger* l_Light = sObjectAccessor->GetAreaTrigger(*me, m_LightOrbGuid);
                    if (!l_Light)
                        return;

                    l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, l_Light->GetLiveTime());
                    l_Light->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, l_Light->GetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2));
                    l_Light->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 0.01f);

                    l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 250);

                    for (uint8 l_I = 0; l_I < 7; ++l_I)
                        l_Light->ForceValuesUpdateAtIndex(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

                    AddTimedDelayedOperation(250, [this]() -> void
                    {
                        if (Vehicle* l_Vehicle = me->GetVehicle())
                        {
                            if (l_Vehicle->GetBase() && l_Vehicle->GetBase()->ToCreature())
                                l_Vehicle->GetBase()->ToCreature()->DespawnOrUnsummon();
                        }

                        me->RemoveAllAreasTrigger();
                        me->DespawnOrUnsummon();
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellDemonicEssenceDoT)
                {
                    p_Target->SendPlaySpellVisual(eVisuals::VisalShadow, nullptr, 7.0f, me->GetPosition());
                    p_Target->SendPlaySpellVisual(eVisuals::VisualLight, nullptr, 7.0f, me->GetPosition());
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::SpellPurifiedEssenceCast && p_EffIndex == SpellEffIndex::EFFECT_0 && !p_Targets.empty())
                {
                    /// SPELL_EFFECT_INSTAKILL -> Only for players
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object || !p_Object->IsPlayer())
                            return true;

                        return false;
                    });
                }
            }

            void DoAction(int32 const p_Action) override
            {
                /// Full health reached
                if (!p_Action)
                {
                    DoCast(me, eSpells::SpellPurifiedEssenceCast);

                    me->RemoveAreaTrigger(eSpells::SpellDemonicEssenceAT);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_demonic_essenceAI(p_Creature);
        }
};

/// Visions of the Dark Titan - 113969
class npc_guldan_visions_of_the_dark_titan_dummy : public CreatureScript
{
    public:
        npc_guldan_visions_of_the_dark_titan_dummy() : CreatureScript("npc_guldan_visions_of_the_dark_titan_dummy") { }

        enum eSpells
        {
            SpellVisionsVisual  = 227014,
            SpellVisionsAura    = 227016
        };

        struct npc_guldan_visions_of_the_dark_titan_dummyAI : public Scripted_NoMovementAI
        {
            npc_guldan_visions_of_the_dark_titan_dummyAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellVisionsVisual, true);

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellVisionsAura, true);
                });
            }

            void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode) override
            {
                if (p_SpellID == eSpells::SpellVisionsAura)
                    me->DespawnOrUnsummon(1);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_visions_of_the_dark_titan_dummyAI(p_Creature);
        }
};

/// Visions of the Dark Titan - 113972
class npc_guldan_visions_of_the_dark_titan_final : public CreatureScript
{
    public:
        npc_guldan_visions_of_the_dark_titan_final() : CreatureScript("npc_guldan_visions_of_the_dark_titan_final") { }

        enum eSpells
        {
            SpellVisionsVisual  = 227024
        };

        enum eVisual
        {
            VisualVisions = 61190
        };

        struct npc_guldan_visions_of_the_dark_titan_finalAI : public Scripted_NoMovementAI
        {
            npc_guldan_visions_of_the_dark_titan_finalAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            std::array<Position const, eGuldanData::MaxVisionsVisu> m_VisionsVisualsPos =
            {
                {
                    { 275.5176f, 3183.339f, 465.9067f, 0.0f },
                    { 333.8384f, 3151.594f, 465.9068f, 0.0f },
                    { 263.5628f, 3184.353f, 465.9067f, 0.0f },
                    { 230.8510f, 3155.517f, 465.9067f, 0.0f },
                    { 288.2716f, 3162.115f, 465.9418f, 0.0f },
                    { 248.9639f, 3102.572f, 465.9067f, 0.0f },
                    { 254.2360f, 3105.821f, 465.9067f, 0.0f },
                    { 243.2268f, 3158.306f, 465.9067f, 0.0f },
                    { 257.5341f, 3100.583f, 465.9067f, 0.0f },
                    { 236.0573f, 3149.037f, 465.9067f, 0.0f },
                    { 287.7546f, 3167.320f, 465.9067f, 0.0f },
                    { 253.0674f, 3125.204f, 465.9067f, 0.0f },
                    { 259.8410f, 3181.645f, 465.9067f, 0.0f },
                    { 316.9856f, 3161.081f, 465.9068f, 0.0f },
                    { 230.5563f, 3123.884f, 465.9067f, 0.0f }
                }
            };

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::SpellVisionsVisual, true);

                uint32 l_Time = 300;
                uint8  l_IDx  = 0;

                for (Position const l_Pos : m_VisionsVisualsPos)
                {
                    if (l_IDx >= 3)
                    {
                        l_Time += 300;
                        l_IDx   = 0;
                    }

                    AddTimedDelayedOperation(l_Time, [this, l_Pos]() -> void
                    {
                        me->SendPlaySpellVisual(eVisual::VisualVisions, nullptr, 0.5f, l_Pos, true);
                    });

                    ++l_IDx;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guldan_visions_of_the_dark_titan_finalAI(p_Creature);
        }
};

/// The Eye of Aman'thul - 227743
/// The Eye of Aman'thul - 227745
class spell_guldan_eye_of_amanthul_cosmetic : public SpellScriptLoader
{
    public:
        spell_guldan_eye_of_amanthul_cosmetic() : SpellScriptLoader("spell_guldan_eye_of_amanthul_cosmetic") { }

        enum eAction
        {
            ActionCosmeticTrigger
        };

        class spell_guldan_eye_of_amanthul_cosmetic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_eye_of_amanthul_cosmetic_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(eAction::ActionCosmeticTrigger);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_eye_of_amanthul_cosmetic_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_eye_of_amanthul_cosmetic_AuraScript();
        }
};

/// Liquid Hellfire - 206204
class spell_guldan_liquid_hellfire_dummy : public SpellScriptLoader
{
    public:
        spell_guldan_liquid_hellfire_dummy() : SpellScriptLoader("spell_guldan_liquid_hellfire_dummy") { }

        enum eSpells
        {
            SpellLiquidHellfireCast             = 206219,
            SpellEmpoweredLiquidHellfireCast    = 206220
        };

        class spell_guldan_liquid_hellfire_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_liquid_hellfire_dummy_SpellScript);

            void HandleDummy(SpellEffIndex p_EffIndex)
            {
                PreventHitEffect(p_EffIndex);

                /// Handle Empowered Liquid Hellfire
                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    boss_guldan::boss_guldanAI* l_AI = CAST_AI(boss_guldan::boss_guldanAI, l_Caster->GetAI());
                    if (l_AI == nullptr)
                        return;

                    if (l_AI->m_EmpoweredLiquidHellfire)
                        l_Caster->CastSpell(l_Caster, eSpells::SpellEmpoweredLiquidHellfireCast);
                    else
                        l_Caster->CastSpell(l_Caster, eSpells::SpellLiquidHellfireCast);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_guldan_liquid_hellfire_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_liquid_hellfire_dummy_SpellScript();
        }
};

/// Liquid Hellfire - 206561
class spell_guldan_liquid_hellfire_summon : public SpellScriptLoader
{
    public:
        spell_guldan_liquid_hellfire_summon() : SpellScriptLoader("spell_guldan_liquid_hellfire_summon") { }

        class spell_guldan_liquid_hellfire_summon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_liquid_hellfire_summon_SpellScript);

            void ModDestHeight(SpellEffIndex /*p_EffIndex*/)
            {
                WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest());
                if (l_Dest == nullptr)
                    return;

                Position const l_Offset = { 0.0f, 0.0f, 1.1f, 0.0f };

                l_Dest->RelocateOffset(l_Offset);
                GetHitDest()->RelocateOffset(l_Offset);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_guldan_liquid_hellfire_summon_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_liquid_hellfire_summon_SpellScript();
        }
};

/// Liquid Hellfire - 206555
/// Empowered Liquid Hellfire - 206581
class spell_guldan_liquid_hellfire_damage : public SpellScriptLoader
{
    public:
        spell_guldan_liquid_hellfire_damage() : SpellScriptLoader("spell_guldan_liquid_hellfire_damage") { }

        class spell_guldan_liquid_hellfire_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_liquid_hellfire_damage_SpellScript);

            void DealDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster() || !GetHitUnit() || !GetExplTargetDest())
                    return;

                float l_Distance    = GetExplTargetDest()->GetExactDist2d(GetHitUnit());
                float l_Radius      = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].CalcRadius(GetCaster()) / 2.0f;

                if (!l_Radius)
                    return;

                float l_Pct = std::max(0.25f * l_Radius, l_Radius - l_Distance) / l_Radius * 100.0f;

                int32 l_Damage = CalculatePct(GetHitDamage(), l_Pct);
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_guldan_liquid_hellfire_damage_SpellScript::DealDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_liquid_hellfire_damage_SpellScript();
        }
};

/// Soul Vortex - 206883
class spell_guldan_soul_vortex : public SpellScriptLoader
{
    public:
        spell_guldan_soul_vortex() : SpellScriptLoader("spell_guldan_soul_vortex") { }

        enum eSpell
        {
            SpellSoulVortexMissile = 206884
        };

        class spell_guldan_soul_vortex_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_soul_vortex_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                        l_Caster->CastSpell(l_Target, eSpell::SpellSoulVortexMissile, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_guldan_soul_vortex_SpellScript::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_soul_vortex_SpellScript();
        }
};

/// Resonant Barrier - 210296
class spell_guldan_resonant_barrier : public SpellScriptLoader
{
    public:
        spell_guldan_resonant_barrier() : SpellScriptLoader("spell_guldan_resonant_barrier") { }

        enum eSpell
        {
            SpellResonantBarrierDummy = 206497
        };

        class spell_guldan_resonant_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_resonant_barrier_AuraScript);

            uint64 m_TriggerGuid = 0;

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    l_Player->SendSpellCooldown(GetSpellInfo()->Id, 0, 180 * TimeConstants::IN_MILLISECONDS);

                    Position l_Pos = l_Player->GetPosition();

                    l_Pos.m_positionZ += 2.15f;

                    if (Creature* l_Trigger = l_Player->SummonCreature(eCreatures::NpcNightwellEntity, l_Pos))
                        m_TriggerGuid = l_Trigger->GetGUID();
                }
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !m_TriggerGuid)
                    return;

                if (Creature* l_Trigger = l_Caster->GetMap()->GetCreature(m_TriggerGuid))
                    l_Caster->CastSpell(l_Trigger, eSpell::SpellResonantBarrierDummy, true);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !m_TriggerGuid)
                    return;

                if (Creature* l_Trigger = l_Caster->GetMap()->GetCreature(m_TriggerGuid))
                    l_Trigger->DespawnOrUnsummon();
            }

            void OnAbsorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                /// Absorb all incoming damages
                p_AbsorbAmount = p_DmgInfo.GetAmount();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_guldan_resonant_barrier_AuraScript::CalculateAmount, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_guldan_resonant_barrier_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_resonant_barrier_AuraScript::OnTick, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_guldan_resonant_barrier_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_guldan_resonant_barrier_AuraScript::OnAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_resonant_barrier_AuraScript();
        }
};

/// Time Dilation - 210339
class spell_guldan_time_dilation : public SpellScriptLoader
{
    public:
        spell_guldan_time_dilation() : SpellScriptLoader("spell_guldan_time_dilation") { }

        class spell_guldan_time_dilation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_time_dilation_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->SendSpellCooldown(GetSpellInfo()->Id, 0, 55 * TimeConstants::IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_guldan_time_dilation_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_time_dilation_SpellScript();
        }

        class spell_guldan_time_dilation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_time_dilation_AuraScript);

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->GetMap()->IsMythic())
                    p_Duration = 30 * TimeConstants::IN_MILLISECONDS;
                else
                    p_Duration = 10 * TimeConstants::IN_MILLISECONDS;
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_guldan_time_dilation_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_time_dilation_AuraScript();
        }
};

/// Scattering Field - 217830
class spell_guldan_scattering_field : public SpellScriptLoader
{
    public:
        spell_guldan_scattering_field() : SpellScriptLoader("spell_guldan_scattering_field") { }

        enum eSpell
        {
            SpellScatteringFieldAbsorb = 206985
        };

        class spell_guldan_scattering_field_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_scattering_field_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->SendSpellCooldown(GetSpellInfo()->Id, 0, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_guldan_scattering_field_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_scattering_field_SpellScript();
        }

        class spell_guldan_scattering_field_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_scattering_field_Aurascript);

            uint64 m_TriggerGuid = 0;

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->IsPlayer())
                        return;

                    if (Creature* l_Trigger = l_Caster->SummonCreature(WORLD_TRIGGER, l_Caster->GetPosition()))
                    {
                        m_TriggerGuid = l_Trigger->GetGUID();

                        l_Trigger->CastSpell(l_Trigger, GetSpellInfo()->Id, true);

                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::SpellScatteringFieldAbsorb, l_Caster->GetMap()->GetDifficultyID()))
                        {
                            uint32& l_Helper = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::GuldanScatteringField);

                            l_Helper = l_SpellInfo->Effects[EFFECT_1].CalcValue(l_Caster);
                        }
                    }
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!l_Caster->IsPlayer())
                        return;

                    if (Creature* l_Trigger = Creature::GetCreature(*l_Caster, m_TriggerGuid))
                    {
                        l_Trigger->RemoveAllAreasTrigger();
                        l_Trigger->DespawnOrUnsummon();

                        uint32& l_Helper = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::GuldanScatteringField);

                        l_Helper = 0;
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_guldan_scattering_field_Aurascript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_AREATRIGGER, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_guldan_scattering_field_Aurascript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_AREATRIGGER, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_scattering_field_Aurascript();
        }
};

/// Bonds of Fel - 209011
class spell_guldan_bonds_of_fel : public SpellScriptLoader
{
    public:
        spell_guldan_bonds_of_fel() : SpellScriptLoader("spell_guldan_bonds_of_fel") { }

        enum eSpells
        {
            SpellBondsOfFelAT       = 206354,
            SpellBondsOfFelDamage   = 206339,
            SpellBondsOfFelAoE      = 206340,
            SpellBondsOfFelChannel  = 206383
        };

        class spell_guldan_bonds_of_fel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_bonds_of_fel_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster())
                    return;

                Unit* l_Target = GetTarget();
                Creature* l_Caster = GetCaster()->ToCreature();

                if (!l_Target || !l_Caster || !l_Caster->IsAIEnabled)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpellBondsOfFelDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_bonds_of_fel_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_bonds_of_fel_AuraScript();
        }
};

/// Eye of Gul'dan - 209454
/// Empowered Eye of Gul'dan - 221728
class spell_guldan_eye_of_guldan : public SpellScriptLoader
{
    public:
        spell_guldan_eye_of_guldan() : SpellScriptLoader("spell_guldan_eye_of_guldan") { }

        enum eSpell
        {
            SpellEmpoweredEyeOfGuldanAura = 221731
        };

        class spell_guldan_eye_of_guldan_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_eye_of_guldan_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster || l_Caster->isDead())
                {
                    /// In case of Eye dead
                    if (l_Target && l_Caster)
                        l_Target->RemoveAura(eSpell::SpellEmpoweredEyeOfGuldanAura, l_Caster->GetGUID());

                    p_AurEff->GetBase()->Remove();
                    return;
                }

                l_Caster->CastSpell(l_Target, p_AurEff->GetAmount(), true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_eye_of_guldan_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_eye_of_guldan_AuraScript();
        }
};

/// Eye of Gul'dan - 209324
class spell_guldan_eye_of_guldan_energizer : public SpellScriptLoader
{
    public:
        spell_guldan_eye_of_guldan_energizer() : SpellScriptLoader("spell_guldan_eye_of_guldan_energizer") { }

        enum eAction
        {
            ActionDuplicate
        };

        class spell_guldan_eye_of_guldan_energizer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_eye_of_guldan_energizer_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Creature* l_Target = GetTarget()->ToCreature();
                if (!l_Target || !l_Target->IsAIEnabled)
                    return;

                l_Target->EnergizeBySpell(l_Target, GetSpellInfo()->Id, 10, Powers::POWER_ENERGY);

                if (l_Target->GetPower(Powers::POWER_ENERGY) >= 100)
                    l_Target->AI()->DoAction(eAction::ActionDuplicate);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_eye_of_guldan_energizer_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_eye_of_guldan_energizer_AuraScript();
        }
};

/// Fel Scythe - 227554
class spell_guldan_fel_scythe : public SpellScriptLoader
{
    public:
        spell_guldan_fel_scythe() : SpellScriptLoader("spell_guldan_fel_scythe") { }

        class spell_guldan_fel_scythe_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_fel_scythe_Aurascript);

            enum eAction
            {
                ActionFelScythe = 2
            };

            enum eSpells
            {
                SpellFelScythePowerIncrease = 227557,
                SpellFelScytheSearcher      = 227551
            };

            uint8 m_TickCount   = 0;
            bool m_OddTick      = true;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetTarget())
                    return;

                Creature* l_Target = GetTarget()->ToCreature();
                if (!l_Target || !l_Target->IsAIEnabled)
                    return;

                int32 l_Amount = m_OddTick ? 3 : 2;

                for (uint8 l_I = 0; l_I < uint8(l_Amount); ++l_I)
                    l_Target->CastSpell(l_Target, eSpells::SpellFelScythePowerIncrease, true);

                l_Target->EnergizeBySpell(l_Target, GetSpellInfo()->Id, l_Amount, Powers::POWER_ENERGY);

                m_OddTick = !m_OddTick;

                /// Repeat cycle: +3 +2 +3 +2 +3 +2 +3
                if (++m_TickCount >= 7)
                {
                    m_TickCount = 0;
                    m_OddTick   = true;
                }

                if (l_Target->GetPower(Powers::POWER_ENERGY) >= 100)
                    l_Target->AI()->DoAction(eAction::ActionFelScythe);
            }

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_ProcInfos)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcInfos.GetActionTarget();

                if (!l_Caster || !l_Target)
                    return;

                if (!l_Target->FindNearestPlayer(2.0f))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpellFelScytheSearcher, true);

                float l_Angle = frand(0.0f, 2.0f * M_PI);

                for (uint8 l_I = 0; l_I < eGuldanData::MaxFelScythes; ++l_I)
                {
                    Position l_Pos = l_Target->GetPosition();

                    l_Pos.SimplePosXYRelocationByAngle(l_Pos, 5.0f, l_Angle, true);

                    l_Pos.SetOrientation(l_Pos.GetAngle(l_Target));

                    l_Angle = Position::NormalizeOrientation(l_Angle + (2.0f * M_PI / 3.0f));

                    if (Creature* l_Fog = l_Caster->SummonCreature(eCreatures::NpcFogCloud, l_Pos))
                    {
                        if (l_Fog->IsAIEnabled)
                            l_Fog->AI()->SetGUID(l_Target->GetGUID());
                    }
                }

                l_Caster->SetPower(Powers::POWER_ENERGY, 0);
                l_Caster->RemoveAura(eSpells::SpellFelScythePowerIncrease);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_fel_scythe_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
                OnEffectProc += AuraEffectProcFn(spell_guldan_fel_scythe_Aurascript::OnProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_fel_scythe_Aurascript();
        }
};

/// Empowered Bonds of Fel - 206366
class spell_guldan_empowered_bonds_of_fel : public SpellScriptLoader
{
    public:
        spell_guldan_empowered_bonds_of_fel() : SpellScriptLoader("spell_guldan_empowered_bonds_of_fel") { }

        enum eSpell
        {
            SpellEmpoweredBondsOfFelAT = 209086
        };

        class spell_guldan_empowered_bonds_of_fel_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_empowered_bonds_of_fel_Aurascript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Target->GetAreaTrigger(eSpell::SpellEmpoweredBondsOfFelAT))
                        p_AurEff->GetBase()->Remove();
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_empowered_bonds_of_fel_Aurascript::OnTick, SpellEffIndex::EFFECT_2, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_empowered_bonds_of_fel_Aurascript();
        }
};

/// Empowered Bonds of Fel - 206384
class spell_guldan_empowered_bonds_of_fel_aura : public SpellScriptLoader
{
    public:
        spell_guldan_empowered_bonds_of_fel_aura() : SpellScriptLoader("spell_guldan_empowered_bonds_of_fel_aura") { }

        enum eSpells
        {
            SpellEmpoweredBondsOfFelAT      = 209086,
            SpellEmpoweredBondsOfFelDamage  = 206367,
            SpellEmpoweredBondsOfFelAoE     = 206370,
            SpellEmpoweredBondsOfFelChannel = 206383
        };

        class spell_guldan_empowered_bonds_of_fel_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_empowered_bonds_of_fel_aura_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster())
                    return;

                Unit* l_Target = GetTarget();
                Creature* l_Caster = GetCaster()->ToCreature();

                if (!l_Target || !l_Caster || !l_Caster->IsAIEnabled)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpellEmpoweredBondsOfFelDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_empowered_bonds_of_fel_aura_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_empowered_bonds_of_fel_aura_AuraScript();
        }
};

/// Illidan's Soul - 231226
class spell_guldan_illidans_soul : public SpellScriptLoader
{
    public:
        spell_guldan_illidans_soul() : SpellScriptLoader("spell_guldan_illidans_soul") { }

        class spell_guldan_illidans_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_illidans_soul_AuraScript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].TriggerSpell, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_illidans_soul_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_illidans_soul_AuraScript();
        }
};

/// Flames of Sargeras - 221606
class spell_guldan_flames_of_sargeras : public SpellScriptLoader
{
    public:
        spell_guldan_flames_of_sargeras() : SpellScriptLoader("spell_guldan_flames_of_sargeras") { }

        enum eSpell
        {
            SpellFlamesOfSargerasPeriodic = 221603
        };

        class spell_guldan_flames_of_sargeras_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_flames_of_sargeras_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpell::SpellFlamesOfSargerasPeriodic, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_guldan_flames_of_sargeras_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_flames_of_sargeras_AuraScript();
        }
};

/// Flames of Sargeras - 221603
class spell_guldan_flames_of_sargeras_periodic : public SpellScriptLoader
{
    public:
        spell_guldan_flames_of_sargeras_periodic() : SpellScriptLoader("spell_guldan_flames_of_sargeras_periodic") { }

        enum eSpell
        {
            SpellFlamesOfSargerasDmg = 228190
        };

        class spell_guldan_flames_of_sargeras_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_flames_of_sargeras_periodic_AuraScript);

            uint8 m_TickCount = 0;

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (m_TickCount >= 4)
                {
                    p_AurEff->GetBase()->Remove();
                    return;
                }

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                ++m_TickCount;

                l_Target->CastSpell(l_Target, eSpell::SpellFlamesOfSargerasDmg, true);
                l_Target->CastSpell(l_Target, p_AurEff->GetAmount(), true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_flames_of_sargeras_periodic_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_flames_of_sargeras_periodic_AuraScript();
        }
};

/// Flames of Sargeras - 206503
/// Flames of Sargeras - 228190
class spell_guldan_flames_of_sargeras_aoe : public SpellScriptLoader
{
    public:
        spell_guldan_flames_of_sargeras_aoe() : SpellScriptLoader("spell_guldan_flames_of_sargeras_aoe") { }

        enum eSpell
        {
            SpellFlamesOfSargerasMissile = 206499
        };

        class spell_guldan_flames_of_sargeras_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_flames_of_sargeras_aoe_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = nullptr;
                Unit* l_Target = GetHitUnit();

                if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                {
                    if (Creature* l_Guldan = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::NpcGulDan)))
                        l_Caster = l_Guldan;
                }

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpell::SpellFlamesOfSargerasMissile, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_guldan_flames_of_sargeras_aoe_SpellScript::HandleDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_flames_of_sargeras_aoe_SpellScript();
        }
};

/// Well of Souls - 208536
class spell_guldan_well_of_souls : public SpellScriptLoader
{
    public:
        spell_guldan_well_of_souls() : SpellScriptLoader("spell_guldan_well_of_souls") { }

        enum eGuidData
        {
            DataSoulRemoved = 1
        };

        class spell_guldan_well_of_souls_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_well_of_souls_AuraScript);

            uint32 m_AppliedTime = 0;

            void OnUpdate(uint32 p_Diff, AuraEffect* /*p_AurEff*/)
            {
                m_AppliedTime += p_Diff;

                if (m_AppliedTime >= 3 * TimeConstants::IN_MILLISECONDS)
                {
                    Unit* l_Target = GetUnitOwner();
                    if (!l_Target)
                        return;

                    if (Creature* l_Well = l_Target->FindNearestCreature(eCreatures::NpcWellOfSoulsMain, 100.0f))
                    {
                        if (l_Well->IsAIEnabled)
                            l_Well->AI()->SetGUID(l_Target->GetGUID(), eGuidData::DataSoulRemoved);
                    }

                    m_AppliedTime = 0;
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_guldan_well_of_souls_AuraScript::OnUpdate, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_well_of_souls_AuraScript();
        }
};

/// Soul Corrosion - 208802
class spell_guldan_soul_corrosion : public SpellScriptLoader
{
    public:
        spell_guldan_soul_corrosion() : SpellScriptLoader("spell_guldan_soul_corrosion") { }

        class spell_guldan_soul_corrosion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_soul_corrosion_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target || !p_AurEff->GetBase()->GetStackAmount())
                    return;

                SpellInfo const* l_ProcSpell = sSpellMgr->GetSpellInfo((p_AurEff->GetAmount() / p_AurEff->GetBase()->GetStackAmount()), l_Caster->GetMap()->GetDifficultyID());
                if (!l_ProcSpell)
                    return;

                int32 l_Damage = l_ProcSpell->Effects[SpellEffIndex::EFFECT_0].BasePoints * p_AurEff->GetBase()->GetStackAmount();

                l_Caster->CastCustomSpell(l_Target, (p_AurEff->GetAmount() / p_AurEff->GetBase()->GetStackAmount()), &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_soul_corrosion_AuraScript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_soul_corrosion_AuraScript();
        }
};

/// Focused Assault - 212625
class spell_guldan_focused_assault : public SpellScriptLoader
{
    public:
        spell_guldan_focused_assault() : SpellScriptLoader("spell_guldan_focused_assault") { }

        class spell_guldan_focused_assault_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_focused_assault_Aurascript);

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_guldan_focused_assault_Aurascript::HandleCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_focused_assault_Aurascript();
        }
};

/// Scattering Field - 206985
class spell_guldan_scattering_field_absorb : public SpellScriptLoader
{
    public:
        spell_guldan_scattering_field_absorb() : SpellScriptLoader("spell_guldan_scattering_field_absorb") { }

        enum eSpell
        {
            SpellScatteringFieldAura = 217830
        };

        class spell_guldan_scattering_field_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_scattering_field_absorb_AuraScript);

            void CalculateAbsorb(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                /// Set to infinite amount, dynamically calculated below
                p_Amount = -1;
            }

            void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Spell const* l_Spell = p_DamageInfo.GetSpell();
                if (!l_Spell)
                    return;

                /// Despawn shield if no absorb remaining
                uint32& l_Amount = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::GuldanScatteringField);
                if (l_Amount == 0)
                {
                    p_AbsorbAmount = 0;
                    l_Caster->RemoveAura(eSpell::SpellScatteringFieldAura);
                    return;
                }

                uint32 l_TargetCount = 0;

                for (auto const& l_Iter : l_Spell->GetTargetInfos())
                {
                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, l_Iter.targetGUID))
                    {
                        if (!l_Unit->HasAura(GetSpellInfo()->Id, l_Caster->GetGUID()))
                            continue;

                        ++l_TargetCount;
                    }
                }

                if (!l_TargetCount)
                    return;

                uint32 l_Damage = CalculatePct(p_DamageInfo.GetAmount(), 50);

                uint32& l_Count = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::GuldanScatteringFieldTargets);

                /// AoE target spell case
                if (l_TargetCount > 1)
                {
                    /// Set target count for caster, will be used to split the absorb
                    if (!l_Count)
                        l_Count = l_TargetCount;

                    uint32 l_Absorb = l_Amount / l_Count;

                    if (l_Damage < l_Absorb)
                        l_Absorb = l_Damage;

                    p_AbsorbAmount = l_Absorb;
                    l_Amount -= p_AbsorbAmount;

                    --l_Count;
                }
                /// Single target spell case
                else
                {
                    /// Reset target count of caster
                    l_Count = 0;

                    /// Damage < remaining absorb, just remove the damage from value
                    if (l_Amount < l_Damage)
                    {
                        p_AbsorbAmount = l_Damage;
                        l_Amount -= l_Damage;
                    }
                    /// Damage >= remaining absorb, absorb what we can, and remove shield
                    else
                    {
                        p_AbsorbAmount = l_Amount;
                        l_Amount = 0;
                        l_Caster->RemoveAura(eSpell::SpellScatteringFieldAura);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_guldan_scattering_field_absorb_AuraScript::CalculateAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_guldan_scattering_field_absorb_AuraScript::HandleOnAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_scattering_field_absorb_AuraScript();
        }
};

/// Parasitic Wound - 206847
class spell_guldan_parasitic_wound : public SpellScriptLoader
{
    public:
        spell_guldan_parasitic_wound() : SpellScriptLoader("spell_guldan_parasitic_wound") { }

        enum eSpell
        {
            SpellParasiticExpulsion = 226955
        };

        class spell_guldan_parasitic_wound_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_parasitic_wound_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                l_Caster->CastSpell(l_Target, eSpell::SpellParasiticExpulsion, true);

                npc_guldan_the_demon_within::npc_guldan_the_demon_withinAI* l_AI = CAST_AI(npc_guldan_the_demon_within::npc_guldan_the_demon_withinAI, l_Caster->GetAI());
                if (!l_AI)
                    return;

                Player* l_Fixate = l_AI->SelectRangedTarget();
                if (!l_Fixate)
                    return;

                for (uint8 l_I = 0; l_I < 2; ++l_I)
                {
                    if (Creature* l_Demon = l_Caster->SummonCreature(eCreatures::NpcParasiticShadowDemon, l_Target->GetPosition()))
                    {
                        if (l_Demon->IsAIEnabled)
                            l_Demon->AI()->SetGUID(l_Fixate->GetGUID());
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_guldan_parasitic_wound_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_parasitic_wound_AuraScript();
        }
};

/// Shadow Shroud - 206779
class spell_guldan_shadow_shroud : public SpellScriptLoader
{
    public:
        spell_guldan_shadow_shroud() : SpellScriptLoader("spell_guldan_shadow_shroud") { }

        class spell_guldan_shadow_shroud_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_shadow_shroud_AuraScript);

            void CalculateAbsorb(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                /// Set to infinite amount
                p_Amount = -1;
            }

            void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Attacker = p_DamageInfo.GetActor();
                if (!l_Attacker)
                    return;

                float l_Distance = l_Attacker->GetDistance(l_Caster);

                /// Doesn't absorb damages if attacker is at 0 yards away
                if (l_Distance < 1.0f)
                    p_AbsorbAmount = 0;
                /// Absorbs full damages past 15 yards
                else if (l_Distance >= 15.0f)
                    p_AbsorbAmount = p_DamageInfo.GetAmount();
                /// Starts absorbing damage at 1 yard, with a full absorb past 15 yards
                else
                {
                    float l_ReductionPct = 100.0f / 15.0f * l_Distance;

                    p_AbsorbAmount = CalculatePct(float(p_DamageInfo.GetAmount()), l_ReductionPct);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_guldan_shadow_shroud_AuraScript::CalculateAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_guldan_shadow_shroud_AuraScript::HandleOnAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_shadow_shroud_AuraScript();
        }
};

/// FLAME CRASH NEXT - 229216
/// SOULSEVER NEXT - 229219
class spell_guldan_the_demon_within_energizers : public SpellScriptLoader
{
    public:
        spell_guldan_the_demon_within_energizers() : SpellScriptLoader("spell_guldan_the_demon_within_energizers") { }

        enum eAction
        {
            ActionFullEnergy
        };

        class spell_guldan_the_demon_within_energizers_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_the_demon_within_energizers_Aurascript);

            uint32 m_PowerGain = 10;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetTarget())
                    return;

                Creature* l_Target = GetTarget()->ToCreature();
                if (!l_Target || !l_Target->IsAIEnabled)
                    return;

                int32 l_Power = l_Target->GetPower(Powers::POWER_ENERGY);

                if ((l_Power + m_PowerGain) >= l_Target->GetMaxPower(Powers::POWER_ENERGY))
                    l_Target->AI()->DoAction(eAction::ActionFullEnergy);
                else
                    l_Target->EnergizeBySpell(l_Target, GetSpellInfo()->Id, m_PowerGain, Powers::POWER_ENERGY);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_the_demon_within_energizers_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_the_demon_within_energizers_Aurascript();
        }
};

/// Sheared Soul - 206458
class spell_guldan_sheared_soul : public SpellScriptLoader
{
    public:
        spell_guldan_sheared_soul() : SpellScriptLoader("spell_guldan_sheared_soul") { }

        class spell_guldan_sheared_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_sheared_soul_AuraScript);

            void CalculateAbsorb(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                /// Set to infinite amount
                p_Amount = -1;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_guldan_sheared_soul_AuraScript::CalculateAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_HEAL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_sheared_soul_AuraScript();
        }
};

/// Flame Crash - 227096
class spell_guldan_flame_crash_damage : public SpellScriptLoader
{
    public:
        spell_guldan_flame_crash_damage() : SpellScriptLoader("spell_guldan_flame_crash_damage") { }

        class spell_guldan_flame_crash_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_guldan_flame_crash_damage_SpellScript);

            void DealDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (!l_Caster || !l_Target || !GetExplTargetDest())
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();

                int32 l_MinDamage = l_SpellInfo->Effects[p_EffIndex].CalcValue(l_Caster);
                int32 l_MaxDamage = l_MinDamage * l_SpellInfo->Effects[SpellEffIndex::EFFECT_1].BasePoints;

                Position l_Pos = *GetExplTargetDest();

                float l_Distance  = std::min(50.0f, l_Target->GetDistance(l_Pos));

                /// Set to max damage if <= 1 yards to the boss
                if (l_Distance <= 1.0f)
                {
                    SetHitDamage(l_MaxDamage);
                    return;
                }

                float l_Pct = l_Distance / 50.0f;

                int32 l_NewDamage = std::max(l_MinDamage, l_MaxDamage - int32(l_Pct * float(l_MaxDamage - l_MinDamage)));

                SetHitDamage(l_NewDamage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_guldan_flame_crash_damage_SpellScript::DealDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_guldan_flame_crash_damage_SpellScript();
        }
};

/// Nightshield - 221292
class spell_guldan_nightshield_absorb : public SpellScriptLoader
{
    public:
        spell_guldan_nightshield_absorb() : SpellScriptLoader("spell_guldan_nightshield_absorb") { }

        enum eSpell
        {
            SpellBulwarkOfAzzinothAura = 221299
        };

        class spell_guldan_nightshield_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_nightshield_absorb_AuraScript);

            void CalculateAbsorb(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                /// Set to infinite amount, dynamically calculated below
                p_Amount = -1;
            }

            void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Attacker    = p_DamageInfo.GetActor();
                Unit* l_Target      = GetUnitOwner();

                /// Bulwark of Azzinoth bypasses Nightshield protection
                if (!l_Attacker || !l_Target || l_Target->GetEntry() == l_Attacker->GetEntry() || l_Attacker->HasAura(eSpell::SpellBulwarkOfAzzinothAura))
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                /// Otherwise, absorbs 50% of all damage taken
                p_AbsorbAmount = CalculatePct(p_DamageInfo.GetAmount(), GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_guldan_nightshield_absorb_AuraScript::CalculateAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_guldan_nightshield_absorb_AuraScript::HandleOnAbsorb, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_nightshield_absorb_AuraScript();
        }
};

/// Instability - 227250
class spell_guldan_instability : public SpellScriptLoader
{
    public:
        spell_guldan_instability() : SpellScriptLoader("spell_guldan_instability") { }

        enum eSpell
        {
            SpellInstabilityDamage = 227251
        };

        class spell_guldan_instability_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_instability_Aurascript);

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                int32 l_Damage = l_Target->CountPctFromMaxHealth(1.5f);

                l_Target->CastCustomSpell(l_Target, eSpell::SpellInstabilityDamage, &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_instability_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_instability_Aurascript();
        }
};

/// Capricious Barrage - 206912
class spell_guldan_capricious_barrage : public SpellScriptLoader
{
    public:
        spell_guldan_capricious_barrage() : SpellScriptLoader("spell_guldan_capricious_barrage") { }

        class spell_guldan_capricious_barrage_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_capricious_barrage_Aurascript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, p_AurEff->GetAmount(), true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_capricious_barrage_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_capricious_barrage_Aurascript();
        }
};

/// Demonic Essence - 221431
class spell_guldan_demonic_essence_periodic : public SpellScriptLoader
{
    public:
        spell_guldan_demonic_essence_periodic() : SpellScriptLoader("spell_guldan_demonic_essence_periodic") { }

        class spell_guldan_demonic_essence_periodic_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_demonic_essence_periodic_Aurascript);

            uint64 m_LastHealth = 0;
            bool m_DownScaled = false;

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || m_DownScaled)
                    return;

                npc_guldan_demonic_essence::npc_guldan_demonic_essenceAI* l_AI = CAST_AI(npc_guldan_demonic_essence::npc_guldan_demonic_essenceAI, l_Caster->GetAI());
                if (!l_AI)
                    return;

                AreaTrigger* l_Light = sObjectAccessor->GetAreaTrigger(*l_Caster, l_AI->m_LightOrbGuid);
                if (!l_Light)
                    return;

                float l_LastHealthPct = float(m_LastHealth) / float(l_Caster->GetMaxHealth()) * 100.0f;

                l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, l_Light->GetLiveTime());
                l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
                l_Light->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f + (l_LastHealthPct / 4.0f));
                l_Light->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
                l_Light->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 1.0f + (l_Caster->GetHealthPct() / 4.0f));
                l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);

                l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 750);

                m_LastHealth = l_Caster->GetHealth();

                for (uint8 l_I = 0; l_I < 7; ++l_I)
                    l_Light->ForceValuesUpdateAtIndex(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

                if (l_Caster->GetHealthPct() >= 100.0f)
                {
                    l_AI->DoAction(0);

                    AreaTrigger* l_Shadow = sObjectAccessor->GetAreaTrigger(*l_Caster, l_AI->m_ShadowOrbGuid);
                    if (!l_Shadow)
                        return;

                    l_Shadow->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, l_Shadow->GetLiveTime());
                    l_Shadow->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 1, 0);
                    l_Shadow->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
                    l_Shadow->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);
                    l_Shadow->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 2.0f);
                    l_Shadow->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                    l_Shadow->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);

                    l_Shadow->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 4 * TimeConstants::IN_MILLISECONDS);

                    l_Light->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 55.0f);

                    l_Light->SetUInt32Value(EAreaTriggerFields::AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 4 * TimeConstants::IN_MILLISECONDS);

                    for (uint8 l_I = 0; l_I < 7; ++l_I)
                        l_Shadow->ForceValuesUpdateAtIndex(EAreaTriggerFields::AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + l_I);

                    m_DownScaled = true;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_guldan_demonic_essence_periodic_Aurascript::OnTick, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_demonic_essence_periodic_Aurascript();
        }
};

/// Demonbane - 226842
class spell_guldan_demonbane : public SpellScriptLoader
{
    public:
        spell_guldan_demonbane() : SpellScriptLoader("spell_guldan_demonbane") { }

        class spell_guldan_demonbane_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_guldan_demonbane_Aurascript);

            bool CanRefresh(bool p_Decrease)
            {
                return false;
            }

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    Aura* l_Aura = p_AurEff->GetBase();

                    /// Stacking Demonbane adds its duration to the existing one
                    /// Eg. Refreshing Demonbane would make 2 stacks for twice the base duration
                    l_Aura->SetMaxDuration(l_Aura->GetSpellInfo()->GetMaxDuration() + l_Aura->GetDuration());
                    l_Aura->SetDuration(l_Aura->GetMaxDuration());

                    l_Aura->SetNeedClientUpdateForTargets();
                }
            }

            void Register() override
            {
                CanRefreshProc += AuraCanRefreshProcFn(spell_guldan_demonbane_Aurascript::CanRefresh);
                AfterEffectApply += AuraEffectApplyFn(spell_guldan_demonbane_Aurascript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AuraEffectHandleModes::AURA_EFFECT_HANDLE_CHANGE_AMOUNT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_guldan_demonbane_Aurascript();
        }
};

/// The Eye of Aman'thul - 227433
class areatrigger_guldan_eye_of_amanthul_cosmetic : public AreaTriggerEntityScript
{
    public:
        areatrigger_guldan_eye_of_amanthul_cosmetic() : AreaTriggerEntityScript("areatrigger_guldan_eye_of_amanthul_cosmetic") { }

        enum eMoves : uint16
        {
            MoveID1     = 18283,
            MoveID2     = 18282,
            MoveID3     = 18285,
            MoveID4     = 18284,
            MoveID5     = 18287,
            MoveID6     = 18286,
            MoveID7     = 18289,
            MoveID8     = 18288,
            MoveID9     = 18303,
            MoveID10    = 18302,
            MoveID11    = 18301,
            MoveID12    = 18300,
            MoveID13    = 18299,
            MoveID14    = 18298,
            MoveID15    = 18297,
            MoveID16    = 18296
        };

        std::map<uint16, Position> m_OrbIDsPerPos =
        {
            { eMoves::MoveID1,  { 294.7464f, 3130.505f, 465.543f, 0.0f } },
            { eMoves::MoveID2,  { 268.5625f, 3140.576f, 465.543f, 0.0f } },
            { eMoves::MoveID3,  { 286.9752f, 3152.050f, 465.543f, 0.0f } },
            { eMoves::MoveID4,  { 277.3938f, 3120.020f, 465.543f, 0.0f } },
            { eMoves::MoveID5,  { 297.1389f, 3130.077f, 465.543f, 0.0f } },
            { eMoves::MoveID6,  { 269.6129f, 3140.588f, 465.543f, 0.0f } },
            { eMoves::MoveID7,  { 277.0538f, 3121.109f, 465.543f, 0.0f } },
            { eMoves::MoveID8,  { 287.6215f, 3149.594f, 465.543f, 0.0f } },
            { eMoves::MoveID9,  { 248.1640f, 3167.056f, 465.543f, 0.0f } },
            { eMoves::MoveID10, { 250.3998f, 3172.628f, 465.543f, 0.0f } },
            { eMoves::MoveID11, { 250.5212f, 3101.018f, 465.543f, 0.0f } },
            { eMoves::MoveID12, { 245.1657f, 3103.643f, 465.543f, 0.0f } },
            { eMoves::MoveID13, { 316.1441f, 3103.671f, 465.543f, 0.0f } },
            { eMoves::MoveID14, { 314.2692f, 3098.345f, 465.543f, 0.0f } },
            { eMoves::MoveID15, { 314.0694f, 3169.365f, 465.543f, 0.0f } },
            { eMoves::MoveID16, { 319.5553f, 3167.067f, 465.543f, 0.0f } }
        };

        void OnSetCreatePosition(AreaTrigger* p_AreaTrigger, Unit* /*p_Caster*/, Position& /*p_SourcePosition*/, Position& /*p_DestinationPosition*/, std::vector<G3D::Vector3>& /*p_PathToLinearDestination*/) override
        {
            uint16 l_MoveID = eMoves::MoveID1;

            for (auto const& l_Iter : m_OrbIDsPerPos)
            {
                if (p_AreaTrigger->IsNearPosition(&l_Iter.second, 0.1f))
                {
                    l_MoveID = l_Iter.first;
                    break;
                }
            }

            if (!sObjectMgr->GetAreaTriggerPathSize(l_MoveID))
                return;

            for (uint32 l_Itr = 0; l_Itr < sObjectMgr->GetAreaTriggerPathSize(l_MoveID) - 1; ++l_Itr)
                p_AreaTrigger->GetSplineData().push_back(sObjectMgr->GetAreaTriggerMoveSplines(l_MoveID, l_Itr));

            p_AreaTrigger->FillVerticesPoint(p_AreaTrigger);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_guldan_eye_of_amanthul_cosmetic();
        }
};

/// Bonds of Fel - 206354
class areatrigger_guldan_bonds_of_fel : public AreaTriggerEntityScript
{
    public:
        areatrigger_guldan_bonds_of_fel() : AreaTriggerEntityScript("areatrigger_guldan_bonds_of_fel") { }

        enum eSpells
        {
            SpellBondsOfFelAura             = 209011,
            SpellBondsOfFelChannel          = 206383,
            SpellBondsOfFelAoE              = 206340,

            SpellEmpoweredBondsOfFelAura    = 206384,
            SpellEmpoweredBondsOfFelAT      = 209086,
            SpellEmpoweredBondsOfFelAoE     = 206370,
            SpellEmpoweredBondsOfFelChannel = 206383
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                InstanceScript* l_Instance = l_Caster->GetInstanceScript();

                if (l_Instance && l_Caster->GetDistance(*p_AreaTrigger) > p_AreaTrigger->GetRadius())
                {
                    switch (p_AreaTrigger->GetSpellId())
                    {
                        case eSpells::SpellEmpoweredBondsOfFelAT:
                        {
                            l_Caster->RemoveAura(eSpells::SpellEmpoweredBondsOfFelAura);

                            if (Creature* l_Guldan = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::NpcGulDan)))
                            {
                                l_Guldan->CastSpell(l_Caster, eSpells::SpellEmpoweredBondsOfFelAoE, true);

                                if (l_Guldan->IsAIEnabled)
                                    l_Guldan->AI()->SetData(eGuldanData::BondsOfFelGuid, l_Caster->GetGUIDLow());
                            }

                            p_AreaTrigger->Remove();

                            l_Caster->RemoveAura(eSpells::SpellEmpoweredBondsOfFelChannel);
                            break;
                        }
                        default:
                        {
                            l_Caster->RemoveAura(eSpells::SpellBondsOfFelAura);

                            if (Creature* l_Guldan = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::NpcGulDan)))
                            {
                                l_Guldan->CastSpell(l_Caster, eSpells::SpellBondsOfFelAoE, true);

                                if (l_Guldan->IsAIEnabled)
                                    l_Guldan->AI()->SetData(eGuldanData::BondsOfFelGuid, l_Caster->GetGUIDLow());
                            }

                            p_AreaTrigger->Remove();

                            l_Caster->RemoveAura(eSpells::SpellBondsOfFelChannel);
                            break;
                        }
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_guldan_bonds_of_fel();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_guldan()
{
    /// Boss
    new boss_guldan();

    /// Creatures
    new npc_guldan_mythic_vehicle();
    new npc_guldan_liquid_hellfire();
    new npc_guldan_fel_lord_kurazmal();
    new npc_guldan_fel_obelisk();
    new npc_guldan_inquisitor_vethriz();
    new npc_guldan_gaze_of_vethriz();
    new npc_guldan_dzorykx_the_trapper();
    new npc_guldan_heroic_dreadlord();
    new npc_guldan_nightwell_entity();
    new npc_guldan_eye_of_guldan();
    new npc_guldan_bonds_of_fel();
    new npc_guldan_fog_cloud();
    new npc_guldan_empowered_eye_of_guldan();
    new npc_guldan_archmage_khadgar();
    new npc_guldan_lights_heart();
    new npc_guldan_well_of_souls();
    new npc_guldan_dark_soul();
    new npc_guldan_the_demon_within();
    new npc_guldan_parasitic_shadow_demon();
    new npc_guldan_severed_soul();
    new npc_guldan_nightorb();
    new npc_guldan_soul_fragment_of_azzinoth();
    new npc_guldan_demonic_essence_vehicle();
    new npc_guldan_demonic_essence();
    new npc_guldan_visions_of_the_dark_titan_dummy();
    new npc_guldan_visions_of_the_dark_titan_final();

    /// Spells
    new spell_guldan_eye_of_amanthul_cosmetic();
    new spell_guldan_liquid_hellfire_dummy();
    new spell_guldan_liquid_hellfire_summon();
    new spell_guldan_liquid_hellfire_damage();
    new spell_guldan_soul_vortex();
    new spell_guldan_resonant_barrier();
    new spell_guldan_time_dilation();
    new spell_guldan_scattering_field();
    new spell_guldan_bonds_of_fel();
    new spell_guldan_eye_of_guldan();
    new spell_guldan_eye_of_guldan_energizer();
    new spell_guldan_fel_scythe();
    new spell_guldan_empowered_bonds_of_fel();
    new spell_guldan_empowered_bonds_of_fel_aura();
    new spell_guldan_illidans_soul();
    new spell_guldan_flames_of_sargeras();
    new spell_guldan_flames_of_sargeras_periodic();
    new spell_guldan_flames_of_sargeras_aoe();
    new spell_guldan_well_of_souls();
    new spell_guldan_soul_corrosion();
    new spell_guldan_focused_assault();
    new spell_guldan_scattering_field_absorb();
    new spell_guldan_parasitic_wound();
    new spell_guldan_shadow_shroud();
    new spell_guldan_the_demon_within_energizers();
    new spell_guldan_sheared_soul();
    new spell_guldan_flame_crash_damage();
    new spell_guldan_nightshield_absorb();
    new spell_guldan_instability();
    new spell_guldan_capricious_barrage();
    new spell_guldan_demonic_essence_periodic();
    new spell_guldan_demonbane();

    /// AreaTrigger
    new areatrigger_guldan_eye_of_amanthul_cosmetic();
    new areatrigger_guldan_bonds_of_fel();
}
#endif
