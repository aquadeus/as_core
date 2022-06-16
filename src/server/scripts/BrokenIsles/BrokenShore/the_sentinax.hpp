////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////  //////

#include "Common.h"
#include "POIMgr.hpp"

namespace Sentinax
{
    enum eActions
    {
        SetInCooldown   = 1,
        SummonedPortals,
        ExtendPortals,
        EmpowerPortals,
        SummonedBoss,
        DespawnedPortal,
        PortalCount
    };

    enum eDatas
    {
        PortalLevel = 1,
        SpawnIndex
    };

    enum eBroadcastTextID
    {
        PlayerSummonedPortals       = 129707,
        PlayerExtendedPortals       = 129708,
        PlayerEmpoweredPortals      = 129709,
        SentinaxStartsRecharging    = 130654,
        SentinaxFinishedRecharging  = 130655,
        PlayerSummonedLieutenant    = 129710
    };

    enum eCreatures
    {
        SentinaxLaser           = 121083,
        GroundLaser             = 121098,

        /// Domination
        HungeringStalker        = 120575,
        InfectiousStalker       = 120576,
        DarkfiendDevourer       = 120577,
        BattleswornNetherMage   = 120578,
        FelswornChaosMage       = 120579,
        DarkRitualist           = 120580,

        DrainingEye             = 120581,
        DebilitatingEye         = 120582,

        Thanatolion             = 120583,

        /// Firestorm
        BlazingImp              = 120627,
        VirulentImp             = 120628,
        DarkfireFiend           = 120629,

        HellfireInfernal        = 120631,
        VileMother              = 120640,

        Skulguloth              = 120641,

        /// Carnage
        FelguardInvader         = 120658,
        LegionVanguard          = 120659,
        FelbladeSentry          = 120660,
        Wrathguard              = 120661,

        Doombringer             = 120662,
        DoomguardFirecaller     = 120663,

        ForceCommanderXillious  = 120665,

        /// Warbeasts
        LegionWebspewer         = 120666,
        LegionVenomretcher      = 120667,
        Spellstalker            = 120668,

        DarkFelshrieker         = 120669,
        RotwingRipper           = 120674,

        AnThyna                 = 120675,

        /// Engineering
        PutridAlchemist         = 120676,
        CursedPillager          = 120677,
        GanArgEngineer          = 120678,

        MoArgBonecrusher        = 120679,
        MoArgBrute              = 120680,

        FelObliterator          = 120681,

        /// Torment
        LegionPainmaiden        = 120682,
        SentinaxInterrogator    = 120683,
        SentinaxObserver        = 120684,

        SentinaxWarden          = 120685,

        Illisthyndria           = 120686
    };

    enum eSpawnCounts
    {
        DominationLevel1    = 4,
        DominationLevel2    = 1,
        FirestormLevel1     = 10,
        FirestormLevel2     = 1,
        CarnageLevel1       = 4,
        CarnageLevel2       = 2,
        WarbeastsLevel1     = 6,
        WarbeastsLevel2     = 5,
        EngineeringLevel1   = 3,
        EngineeringLevel2   = 2,
        TormentLevel1       = 4,
        TormentLevel2       = 2
    };

    enum eGameObjects
    {
        GobSentinax                     = 268706,
        Portal                          = 268705,
        PortalAdditionalVisual          = 65807,
        GreaterPortalAdditionalVisual   = 66049
    /// GreaterPortal                   = 269125
    };

    enum eTimers
    {
        PortalStart     = 3  * IN_MILLISECONDS,
        MaxTimer        = 45 * IN_MILLISECONDS,
        AdditionalTimer = 25 * IN_MILLISECONDS,
        UpdateCooldown  = 3 * IN_MILLISECONDS,
        DespawnTimer    = 5 * IN_MILLISECONDS,
        WaveTimer       = 15 * IN_MILLISECONDS
    };

    enum eSentinaxSpells
    {
        TheShadowOfTheSentinax      = 240640,
        SentinaxCooldown            = 242872,
        Laser                       = 241399,
        SentinaxDestructionZone     = 240852,

        DominationPortal            = 240123,
        FirestormPortal             = 240299,
        CarnagePortal               = 240302,
        WarbeastsPortal             = 240305,
        EngineeringPortal           = 240308,
        TormentPortal               = 240311,

        GreaterDominationPortal     = 240297,
        GreaterFirestormPortal      = 240300,
        GreaterCarnagePortal        = 240303,
        GreaterWarbeastsPortal      = 240306,
        GreaterEngineeringPortal    = 240309,
        GreaterTormentPortal        = 240312,

        SummonThanatolion           = 240298,
        SummonSkulguloth            = 240301,
        SummonGeneralXillious       = 240304,
        SummonAnThyna               = 240307,
        SummonFelObliterator        = 240310,
        SummonIllisthyndria         = 240313,

        SpawnEffect                 = 241418,
        InfernalSpawnEffect         = 205266,
        DemonSpawn                  = 233240,
        BossSpawnEffect             = 241419
    };

    enum eSentinaxLocations
    {
        TheCreepingGrotto,
        ShadowFracture,
        SoulRuin,
        DarkStockades,
        CoastOfAnguish,
        GarrisonOfTheFel,
        FelfirePass,
        FelrageStrand,
        CrescentRuins,

        MaxLocations
    };

    enum eBeaconCategory
    {
        Domination,
        Firestorm,
        Carnage,
        Warbeasts,
        Engineering,
        Torment,

        MaxBeacon
    };

    std::map<uint32, std::pair<eBeaconCategory, uint32>> g_BeaconsData =
    {
        { eSentinaxSpells::DominationPortal,            { eBeaconCategory::Domination,    1 } },
        { eSentinaxSpells::GreaterDominationPortal,     { eBeaconCategory::Domination,    2 } },
        { eSentinaxSpells::SummonThanatolion,           { eBeaconCategory::Domination,    eCreatures::Thanatolion } },
        { eSentinaxSpells::FirestormPortal,             { eBeaconCategory::Firestorm,     1 } },
        { eSentinaxSpells::GreaterFirestormPortal,      { eBeaconCategory::Firestorm,     2 } },
        { eSentinaxSpells::SummonSkulguloth,            { eBeaconCategory::Firestorm,     eCreatures::Skulguloth } },
        { eSentinaxSpells::CarnagePortal,               { eBeaconCategory::Carnage,       1 } },
        { eSentinaxSpells::GreaterCarnagePortal,        { eBeaconCategory::Carnage,       2 } },
        { eSentinaxSpells::SummonGeneralXillious,       { eBeaconCategory::Carnage,       eCreatures::ForceCommanderXillious } },
        { eSentinaxSpells::WarbeastsPortal,             { eBeaconCategory::Warbeasts,     1 } },
        { eSentinaxSpells::GreaterWarbeastsPortal,      { eBeaconCategory::Warbeasts,     2 } },
        { eSentinaxSpells::SummonAnThyna,               { eBeaconCategory::Warbeasts,     eCreatures::AnThyna } },
        { eSentinaxSpells::EngineeringPortal,           { eBeaconCategory::Engineering,   1 } },
        { eSentinaxSpells::GreaterEngineeringPortal,    { eBeaconCategory::Engineering,   2 } },
        { eSentinaxSpells::SummonFelObliterator,        { eBeaconCategory::Engineering,   eCreatures::FelObliterator } },
        { eSentinaxSpells::TormentPortal,               { eBeaconCategory::Torment,       1 } },
        { eSentinaxSpells::GreaterTormentPortal,        { eBeaconCategory::Torment,       2 } },
        { eSentinaxSpells::SummonIllisthyndria,         { eBeaconCategory::Torment,       eCreatures::Illisthyndria } }
    };

    struct SentinaxData
    {
        SentinaxData(eSentinaxLocations p_LocationName, Position p_SentinaxPosition,
            std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon> p_PortalPositions) :
            LocationName(p_LocationName), SentinaxPosition(p_SentinaxPosition),
            PortalPositions(p_PortalPositions) {}

        eSentinaxLocations LocationName;
        Position SentinaxPosition;
        std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon> PortalPositions;
    };

    SentinaxData* g_CurrentSentinaxData;

    static std::map<uint32, SentinaxData*> g_SentinaxDatas =
    {
        {
            5261,
            new SentinaxData(
                eSentinaxLocations::TheCreepingGrotto,
                Position(-645.50f, 3478.27f, 77.74f, 0.33f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -834.05f, 3399.40f, 58.70f, 0.94f }),
                        Position({ -846.56f, 3606.06f, 1.65f, 4.05f }),
                        Position({ -732.73f, 3471.76f, 20.91f, 0.90f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -717.39f, 3327.37f, 41.87f, 1.16f }),
                        Position({ -778.61f, 3489.78f, 23.49f, 5.42f }),
                        Position({ -617.07f, 3529.49f, 3.00f, 3.27f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -882.91f, 3457.15f, 57.07f, 5.75f }),
                        Position({ -654.53f, 3366.01f, 20.90f, 2.27f }),
                        Position({ -702.18f, 3560.68f, 3.14f, 4.88f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -887.82f, 3646.25f, 17.18f, 5.12f }),
                        Position({ -825.29f, 3446.35f, 51.85f, 5.91f }),
                        Position({ -763.89f, 3484.16f, 23.39f, 2.17f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -689.99f, 3369.64f, 33.45f, 0.58f }),
                        Position({ -611.93f, 3436.62f, 12.03f, 3.35f }),
                        Position({ -845.19f, 3534.73f, 3.07f, 5.20f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -700.51f, 3403.71f, 36.48f, 1.67f }),
                        Position({ -853.03f, 3419.88f, 61.45f, 1.49f }),
                        Position({ -755.58f, 3611.93f, 5.41f, 4.69f })
                    }})
                }})
            )
        },
        {
            5252,
            new SentinaxData(
                eSentinaxLocations::ShadowFracture,
                Position(-1050.76f, 3246.54f, 24.44f, 0.26f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -1010.29f, 3106.90f, 8.74f, 0.73f }),
                        Position({ -1045.29f, 3298.99f, 21.46f, 4.93f }),
                        Position({ -1012.78f, 3184.07f, 18.19f, 2.08f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1065.43f, 3285.14f, 18.05f, 5.82f }),
                        Position({ -953.73f, 3232.95f, 31.86f, 2.94f }),
                        Position({ -974.24f, 3099.26f, 11.27f, 1.30f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1047.882f, 3186.38f, 12.40f, 0.94f }),
                        Position({ -989.40f, 3340.21f, 32.62f, 3.82f }),
                        Position({ -911.48f, 3110.94f, 21.32f, 2.94f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1082.22f, 3246.39f, 12.31f, 0.19f }),
                        Position({ -908.73f, 3163.39f, 20.41f, 2.77f }),
                        Position({ -1010.93f, 3202.31f, 18.48f, 3.13f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -966.63f, 3185.34f, 20.81f, 3.15f }),
                        Position({ -1086.88f, 3218.52f, 11.88f, 0.08f }),
                        Position({ -1000.72f, 3288.09f, 29.68f, 3.37f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -984.34f, 3228.99f, 28.85f, 1.62f }),
                        Position({ -922.63f, 3184.95f, 22.19f, 3.52f }),
                        Position({ -1059.46f, 3203.35f, 11.07f, 1.05f })
                    }})
                }})
            )
        },
        {
            5254,
            new SentinaxData(
                eSentinaxLocations::SoulRuin,
                Position(-1138.56f, 2969.70f, 55.94f, 6.08f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -1190.80f, 2870.90f, -10.78f, 1.23f }),
                        Position({ -1088.53f, 2860.41f, -13.56f, 0.75f }),
                        Position({ -1064.15f, 3098.48f, -10.72f, 4.28f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1015.82f, 3016.90f, -11.91f, 1.84f }),
                        Position({ -972.17f, 2912.78f, -12.87f, 3.91f }),
                        Position({ -1166.81f, 2848.50f, -10.73f, 1.19f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1229.06f, 2881.79f, -10.05f, 0.85f }),
                        Position({ -1162.25f, 3090.45f, -10.78f, 2.94f }),
                        Position({ -1112.77f, 2970.59f, -9.32f, 3.72f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1164.61f, 3045.45f, 6.73f, 5.91f }),
                        Position({ -1200.51f, 2945.45f, -9.72f, 5.61f}),
                        Position({ -1073.83f, 2824.18f, -11.04f, 1.17f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1078.37f, 3007.28f, -8.78f, 2.99f }),
                        Position({ -948.58f, 2867.96f, -13.20f, 3.15f }),
                        Position({ -1083.38f, 2947.89f, -6.00f, 2.89f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -994.92f, 2860.21f, -12.25, 2.48f }),
                        Position({ -1159.18f, 2986.70f, -10.31f, 5.21f }),
                        Position({ -1112.72f, 3094.10f, -10.91f, 4.26f })
                    }})
                }})
            )
        },
        {
            5255,
            new SentinaxData(
                eSentinaxLocations::DarkStockades,
                Position(-1248.332f, 3211.681f, 55.94429f, 5.96524f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -1357.47f, 3220.57f, 16.50f, 5.54f }),
                        Position({ -1254.65f, 3305.19f, 19.69f, 4.66f }),
                        Position({ -1283.92f, 3149.82f, 5.22f, 1.55f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1209.41f, 3158.41f, 5.57f, 1.33f }),
                        Position({ -1229.51f, 3280.27f, 19.70f, 3.20f }),
                        Position({ -1336.84f, 3126.10f, 3.32f, 1.32f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1129.81f, 3179.67f, 11.10f, 3.25f }),
                        Position({ -1290.82f, 3292.41f, 20.37f, 5.55f }),
                        Position({ -1359.58f, 3179.22f, 11.34f, 0.32f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1391.00f, 3161.23f, 21.72f, 6.04f }),
                        Position({ -1261.87f, 3186.19f, 8.22f, 3.70f }),
                        Position({ -1188.65f, 3303.45f, 23.65f, 3.42f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1223.40f, 3243.33f, 12.05f, 4.00f }),
                        Position({ -1267.34f, 3122.20f, 1.51f, 1.34f }),
                        Position({ -1145.34f, 3212.41f, 11.46f, 3.15f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1301.13f, 3256.20f, 13.67f, 5.01f }),
                        Position({ -1152.06f, 3253.16f, 7.67f, 3.94f }),
                        Position({ -1307.75f, 3155.50f, 6.27f, 1.95f })
                    }})
                }})
            )
        },
        {
            5256,
            new SentinaxData(
                eSentinaxLocations::CoastOfAnguish,
                Position(-1658.72f, 2869.33f, 55.94f, 0.32f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -1675.01f, 2931.77f, 6.81f, 4.34f }),
                        Position({ -1650.13f, 2591.25f, 0.00f, 1.42f }),
                        Position({ -1810.44f, 2942.16f, 0.15f, 5.54f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1618.09f, 2911.12f, 18.05f, 3.83f }),
                        Position({ -1619.02f, 2548.83f, -0.22f, 1.32f }),
                        Position({ -1729.46f, 2772.43f, 1.08f, 1.33f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1764.08f, 2928.67f, 1.65f, 5.55f }),
                        Position({ -1667.87f, 2836.09f, 3.13f, 3.45f }),
                        Position({ -1664.95f, 2648.62f, -1.75f, 0.32f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1690.75f, 2721.62f, 19.15f, 1.83f }),
                        Position({ -1717.31f, 2935.99f, 7.71f, 4.55f }),
                        Position({ -1782.36f, 2795.97f, 12.84f, 0.65f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1586.10f, 2636.75f, -0.20f, 4.05f }),
                        Position({ -1792.72f, 2831.78f, 2.65f, 0.30f }),
                        Position({ -1678.83f, 2894.18f, 3.56f, 4.04f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1740.47f, 2865.68f, 1.42f, 0.70f }),
                        Position({ -1569.97f, 2568.20f, -0.21f, 2.81f }),
                        Position({ -1646.27f, 2766.22f, 16.00f, 2.11f })
                    }}),
                }})
            )
        },
        {
            5257,
            new SentinaxData(
                eSentinaxLocations::GarrisonOfTheFel,
                Position(-1164.34f, 2706.02f, 86.70f, 1.37f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -1293.72f, 2709.00f, 29.10f, 5.54f }),
                        Position({ -1134.54f, 2571.66f, 32.02f, 1.69f }),
                        Position({ -1155.24f, 2691.52f, 26.16f, 3.21f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1324.76f, 2576.80f, 44.45f, 1.32f }),
                        Position({ -1113.48f, 2752.78f, 20.62f, 3.78f }),
                        Position({ -1339.26f, 2672.18f, 34.59f, 0.47f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1326.14f, 2756.68f, 26.37f, 6.12f }),
                        Position({ -1208.72f, 2650.96f, 29.23f, 0.32f }),
                        Position({ -1053.75f, 2684.82f, 18.62f, 3.41f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1221.99f, 2523.00f, 50.47f, 1.88f }),
                        Position({ -1296.30f, 2647.16f, 32.58f, 0.26f }),
                        Position({ -1096.92f, 2608.96f, 30.71f, 1.62f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1226.37f, 2598.94f, 34.70f, 0.30f }),
                        Position({ -1204.61f, 2711.77f, 27.96f, 4.68f }),
                        Position({ -1074.81f, 2570.47f, 42.23f, 0.88f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1247.64f, 2750.02f, 26.92f, 4.37f }),
                        Position({ -1348.16f, 2642.36f, 37.95f, 0.05f }),
                        Position({ -1088.89f, 2716.58f, 19.98f, 2.81f })
                    }})
                }})
            )
        },
        {
            5258,
            new SentinaxData(
                eSentinaxLocations::FelfirePass,
                Position(-1199.82f, 1741.83f, 113.25f, 1.78f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -1113.73f, 1784.12f, 47.18f, 2.82f }),
                        Position({ -1385.96f, 1722.64f, 0.23f, 5.54f }),
                        Position({ -1292.00f, 1656.24f, 8.91f, 2.68f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1327.70f, 1645.18f, 9.04f, 1.61f }),
                        Position({ -1224.42f, 1776.33f, 9.22f, 3.53f }),
                        Position({ -1354.17f, 1739.30f, 8.01f, 5.28f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1071.84f, 1860.27f, 50.56f, 3.90f }),
                        Position({ -1223.18f, 1747.55f, 13.61f, 3.19f }),
                        Position({ -1359.43f, 1694.28f, 7.31f, 6.13f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1247.40f, 1695.72f, 17.60f, 3.20f }),
                        Position({ -1375.42f, 1624.34f, 4.26f, 1.96f }),
                        Position({ -1310.60f, 1797.98f, 12.26f, 5.94f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1265.77f, 1800.27f, 13.79f, 4.04f }),
                        Position({ -1300.36f, 1699.12f, 7.27f, 2.69f }),
                        Position({ -1431.38f, 1657.44f, 0.57f, 0.10f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -1187.77f, 1814.84f, 28.80f, 4.99f }),
                        Position({ -1121.98f, 1855.59f, 56.05f, 5.32f }),
                        Position({ -1245.33f, 1719.91f, 8.27f, 2.53f })
                    }}),
                }})
            )
        },
        {
            5259,
            new SentinaxData(
                eSentinaxLocations::FelrageStrand,
                Position(-701.15f, 1823.17f, 113.25f, 6.17f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -769.27f, 2089.54f, 0.77f, 5.54f }),
                        Position({ -705.68f, 1860.75f, 0.88f, 2.66f }),
                        Position({ -713.86f, 2002.93f, 0.76f, 3.49f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -783.92f, 1694.67f, 7.47f, 1.56f }),
                        Position({ -787.92f, 1939.00f, 2.40f, 5.96f }),
                        Position({ -658.99f, 1920.52f, 0.74f, 3.36f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -720.30f, 1808.33f, 1.55f, 4.35f }),
                        Position({ -689.00f, 1682.53f, 2.27f, 1.99f }),
                        Position({ -777.75f, 1984.51f, 2.03f, 6.13f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -640.99f, 1974.18f, 11.48f, 3.00f }),
                        Position({ -768.18f, 1780.26f, 3.08f, 0.25f }),
                        Position({ -639.39f, 1728.43f, 3.10f, 3.16f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -660.19f, 1793.65f, 3.23f, 3.97f }),
                        Position({ -702.71f, 2078.79f, 2.67f, 4.30f }),
                        Position({ -809.46f, 1883.10f, 2.24f, 0.10f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -736.44f, 1918.74f, 0.77f, 3.48f }),
                        Position({ -733.84f, 1689.57f, 4.09f, 1.74f }),
                        Position({ -681.39f, 1847.01f, 3.75f, 2.16f })
                    }})
                }})
            )
        },
        {
            5260,
            new SentinaxData(
                eSentinaxLocations::CrescentRuins,
                Position(-373.19f, 2872.62f, 78.26f, 1.73f),
                std::array<std::array<Position, 3>, eBeaconCategory::MaxBeacon>(
                {{
                    std::array<Position, 3>({{
                        Position({ -512.15f, 2861.13f, 2.91f, 6.16f }),
                        Position({ -407.43f, 2778.42f, 19.91f, 2.08f }),
                        Position({ -463.07f, 3010.71f, 65.77f, 4.93f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -475.15f, 2892.94f, 4.17f, 5.02f }),
                        Position({ -314.27f, 2759.78f, 24.42f, 3.12f }),
                        Position({ -296.34f, 2886.46f, 21.29f, 3.70f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -373.74f, 2766.09f, 24.44f, 1.20f }),
                        Position({ -324.18f, 2848.90f, 19.62f, 2.43f }),
                        Position({ -372.80f, 3009.40f, 45.62f, 4.22f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -277.70f, 2984.66f, 0.88f, 4.08f }),
                        Position({ -413.13f, 2857.59f, 24.95f, 0.42f }),
                        Position({ -304.76f, 2805.46f, 20.89f, 3.03f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -381.87f, 2893.41f, 21.54f, 5.93f }),
                        Position({ -264.85f, 2918.74f, 0.61f, 3.04f }),
                        Position({ -347.66f, 2798.02f, 24.08f, 2.57f })
                    }}),
                    std::array<Position, 3>({{
                        Position({ -388.19f, 2796.82f, 19.94f, 1.67f }),
                        Position({ -319.11f, 2955.18f, 20.12f, 4.69f }),
                        Position({ -491.36f, 2818.28f, 2.80f, 0.99f })
                    }})
                }})
            )
        }
    };

    struct MonsterPool
    {
        MonsterPool(std::set<uint32> p_Entries, uint32 p_Count)
            : Entries(p_Entries), Count(p_Count) { }

        std::set<uint32> Entries;
        uint32 Count;
    };

    std::map<uint64, MonsterPool*> g_MonsterPools =
    {
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Domination, 1),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::HungeringStalker,
                        eCreatures::InfectiousStalker,
                        eCreatures::DarkfiendDevourer,
                        eCreatures::BattleswornNetherMage,
                        eCreatures::FelswornChaosMage,
                        eCreatures::DarkRitualist
                    }
                ), eSpawnCounts::DominationLevel1
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Domination, 2),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::DrainingEye,
                        eCreatures::DebilitatingEye
                    }
                ), eSpawnCounts::DominationLevel2
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Firestorm, 1),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::BlazingImp,
                        eCreatures::VirulentImp,
                        eCreatures::DarkfireFiend
                    }
                ), eSpawnCounts::FirestormLevel1
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Firestorm, 2),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::HellfireInfernal,
                        eCreatures::VileMother
                    }
                ), eSpawnCounts::FirestormLevel2
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Carnage, 1),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::FelguardInvader,
                        eCreatures::LegionVanguard,
                        eCreatures::FelbladeSentry,
                        eCreatures::Wrathguard
                    }
                ), eSpawnCounts::CarnageLevel1
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Carnage, 2),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::Doombringer,
                        eCreatures::DoomguardFirecaller
                    }
                ), eSpawnCounts::CarnageLevel2
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Warbeasts, 1),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::LegionWebspewer,
                        eCreatures::LegionVenomretcher,
                        eCreatures::Spellstalker
                    }
                ), eSpawnCounts::WarbeastsLevel1
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Warbeasts, 2),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::DarkFelshrieker,
                        eCreatures::RotwingRipper
                    }
                ), eSpawnCounts::WarbeastsLevel2
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Engineering, 1),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::PutridAlchemist,
                        eCreatures::CursedPillager,
                        eCreatures::GanArgEngineer
                    }
                ), eSpawnCounts::EngineeringLevel1
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Engineering, 2),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::MoArgBonecrusher,
                        eCreatures::MoArgBrute
                    }
                ), eSpawnCounts::EngineeringLevel2
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Torment, 1),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::LegionPainmaiden,
                        eCreatures::SentinaxInterrogator,
                        eCreatures::SentinaxObserver
                    }
                ), eSpawnCounts::TormentLevel1
            )
        ),
        std::map<uint64, MonsterPool*>::value_type
        (
            MAKE_PAIR64(eBeaconCategory::Torment, 2),
            new MonsterPool(std::set<uint32>(
                    {
                        eCreatures::SentinaxWarden
                    }
                ), eSpawnCounts::TormentLevel2
            )
        )
    };
}
