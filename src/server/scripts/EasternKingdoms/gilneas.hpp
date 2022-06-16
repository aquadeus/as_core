////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2019 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef GILNEAS_HPP
#define GILNEAS_HPP

enum eQuests
{
    Lockdown                    = 14078,
    EvacuatetheMerchantSquare   = 14098,
    WhileYoureAtIt              = 24930,
    BytheSkinofHisTeeth         = 14154,
    TheRebelLordsArsenal        = 14159,
    FromtheShadows              = 14204,
    SaveKrennanAranas           = 14293,
    TimetoRegroup               = 14294,
    Sacrifices                  = 14212,
    ByBloodandAsh               = 14218,
    LastStand                   = 14222,
    LastChanceatHumanity        = 14375,
    AmongHumansAgain            = 14313,
    InNeedofIngredients         = 14320,
    Invasion                    = 14321,
    LeaderofthePack             = 14386,
    TheHungryEttin              = 14416,
    GrandmasCat                 = 14401,
    ToGreymaneManor             = 14465,
    Exodus                      = 24438,
    IntroductionsAreinOrder     = 24472,
    LosingYourTail              = 24616,
    AtOurDoorstep               = 24627,
    NeitherHumanNorBeast        = 24593,
    BetrayalatTempestsReach     = 24592,
    TheBattleforGilneasCity     = 24904,
    TheHuntForSylvanas          = 24902,
    SlowingtheInevitable        = 24920,
    PatriarchsBlessing          = 24679,
    TheyHaveAlliesButSoDoWe     = 24681,
    Endgame                     = 26706
};

enum eCreatures
{
    NpcPanickedCitizen      = 34851,
    NpcPrinceLiamGreymane   = 34913,
    NpcRampagingWorgen      = 34884,
    NpcRampagingWorgen2     = 35660,
    NpcGilneasCityGuard     = 34916,
    NpcFrightenedCitizen1   = 34981,
    NpcFrightenedCitizen2   = 35836,
    NpcGilneasEvacuation    = 35830,
    NpcMyriamSpellwaker     = 35872,
    NpcSergeantCleese       = 35839,
    NpcGilneanRoyalGuard    = 35232,
    NpcBloodfangWorgen      = 35118,
    NpcWorgenAlpha1         = 35170,
    NpcWorgenAlpha2         = 35167,
    NpcWorgenRunt1          = 35188,
    NpcWorgenRunt2          = 35456,
    NpcSeanDempsey          = 35081,
    NpcLornaCrowley         = 35378,
    NpcGilneanMastiff       = 38844,
    NpcJosiahAvery          = 35370,
    NpcJosiahEventTrigger   = 50415,
    NpcBloodfangLurker      = 35463,
    NpcKingGreymanesHorse   = 35905,
    NpcKrennanAranas        = 35907,
    NpcKrennanAranasTree    = 35753,
    NpcBloodfangRipper      = 35505,
    NpcCommandeeredCannon   = 35914,
    NpcBloodfangRipper2     = 35916,
    NpcGilneasCityGuard2    = 50474,
    NpcLordDariusCrowley    = 35552,
    NpcPrinceLiamGreymane2  = 35551,
    NpcCrowleysHorse        = 44427,
    NpcCrowleysHorse2       = 35231,
    NpcLordDariusCrowley2   = 35230,
    NpcNorthgateRebel       = 36057,
    NpcBloodfangStalker     = 35229,
    NpcBloodfangStalkerKC   = 35582,
    NpcRebelCannon          = 35317,
    NpcTobiasMistmantle     = 35618,
    NpcCrowleysHorse3       = 44429,
    NpcNorthgateRebel2      = 41015,
    NpcFrenziedStalker      = 35627,
    NpcAfflictedGilnean     = 50471,
    NpcKrennanAranas2       = 36331,
    NpcLordGodfrey          = 36330,
    NpcKingGennGreymane     = 36332,
    NpcKingGennGreymane2    = 35112,
    NpcGwenArmstead         = 34571,
    NpcForsakenAssassin     = 36207,
    NpcDuskhavenWatchman    = 36211,
    NpcForsakenInvader      = 34511,
    NpcForsakenFootsoldier  = 36236,
    NpcHorridAbomination    = 36231,
    NpcHorridAbominationKC  = 36233,
    NpcGenericTriggerCanon  = 36286,
    NpcForsakenMachinist    = 36292,
    NpcDarkRangerThyala     = 36312,
    NpcAttackMastiff        = 36405,
    NpcMelindaHammond       = 36291,
    NpcDrowningWatchman     = 36440,
    NpcPrinceLiamGreymane3  = 36451,
    NpcDrowningWatchmanKC   = 36450,
    NpcLornaCrowley2        = 36457,
    NpcMountainHorse        = 36540,
    NpcMountainHorseKC      = 36560,
    NpcForsakenCastaway     = 36488,
    NpcLuciustheCruel       = 36461,
    NpcGrandmaWahl          = 36458,
    NpcSwiftMountainHorse   = 36741,
    NpcStagecoachHorse      = 43338,
    NpcStagecoachCarriage   = 44928,
    NpcStagecoachCarriage2  = 43337,
    NpcMarieAllen           = 38853,
    NpcGwenArmstead2        = 44460,
    NpcKrennanAranas3       = 36138,
    NpcDuskhavenWatchman2   = 43907,
    NpcDuskhavenWatchman3   = 37946,
    NpcLornaCrowley3        = 51409,
    NpcStagecoachHarness    = 43336,
    NpcSwampCrocolisk       = 37078,
    NpcCrashSurvivor        = 37067,
    NpcKorothHillbreaker    = 36294,
    NpcForsakenSoldier      = 37805,
    NpcCaptainAsther        = 37806,
    NpcKorothHillbreaker2   = 37808,
    NpcTaldorenTracker      = 38027,
    NpcTobiasMistmantle2    = 38029,
    NpcTalranoftheWild      = 36814,
    NpcVassandraStormclaw   = 37873,
    NpcLyrosSwiftwind       = 37870,
    NpcGenericTrigger       = 35374,
    NpcLornaCrowley4        = 38768,
    NpcKingGennGreymane3    = 38767,
    NpcLordGodfrey2         = 38766,
    NpcTobiasMistmantle3    = 38051,
    NpcLordGodfrey3         = 37875,
    NpcStoutMountainHorse   = 38765,
    NpcEnslavedVillager     = 37694,
    NpcBattleforGilneasKC   = 38854,
    NpcTobiasMistmantle4    = 38507,
    NpcForsakenGeneral      = 38617,
    NpcSylvanasWindrunner   = 38530,
    NpcHighExecutorCrenshaw = 38537,
    NpcGeneralWarhowl       = 38533,
    NpcPlagueCloudBunny     = 38344,
    NpcCapturedRidingBat    = 38615,
    NpcCapturedRidingBat2   = 38540,
    NpcForsakenCatapult     = 38287,
    NpcForsakenInvader2     = 38363,
    NpcForsakenPlaguesmith  = 38364,
    NpcKingGennGreymane4    = 50893,
    NpcGlaiveThrower        = 38150,
    NpcWolfmawOutrider      = 37938,
    NpcToweringAncient      = 37914,
    NpcOrcRaider            = 37916,
    NpcFinaleCredit         = 43729,
    NpcChance               = 36459,
    NpcGilneanSurvivor      = 38781
};

enum eGameobjects
{
    GobStolenBanner         = 201891,
};

enum eSpells
{
    SpellEnrage                 = 8599,
    SpellShadowstalkerStealth   = 5916,
    SpellShoot                  = 67593,
    SpellFrostbolt              = 11538,
    SpellGilneasPrison          = 66914,
    SpellCombatAttack1H         = 42880,
    SpellSummonGilneanMastiff   = 67807,
    SpellAttackLurker           = 67805,
    SpellCannonFire             = 68235,
    SpellThrowTorch             = 67063,
    SpellLastStandComplete      = 72799,
    SpellTwoForms               = 68996,
    SpellCurseoftheWorgen       = 68630,
    SpellPingGilneanCrow        = 93275,
    SpellKegPlaced              = 68555,
    SpellPointPoison            = 42266,
    SpellBloodyDeath            = 80705,
    SpellAbominationExplosion   = 68560,
    SpellFieryBoulder           = 68591,
    SpellLaunchFall             = 66251,
    SpellLeap                   = 68687,
    SpellDemoralizingRoar       = 15971,
    SpellForcecastCataclysm     = 69027,
    SpellRescueDrowningWatchman = 68735,
    SpellRoundUpHorse           = 68903,
    SpellRopeChannel            = 68940,
    SpellSummonSwampCrocolisk   = 69854,
    SpellPlaceBanner            = 70511,
    SpellFreezingTrapEffect     = 70794,
    SpellDarkScoutTracker       = 95845,
    SpellAimedShot              = 70796,
    SpellBelysrasTalisman       = 70797,
    SpellWarStomp               = 71019,
    SpellSummonTaldorenTracker  = 71011,
    SpellTaldorenWell           = 71200,
    SpellWorgenCombatTransform  = 81768,
    SpellPlagueBarrelLaunch     = 71804,
    SpellShootPlague            = 72205,
    SpellSlimeSpigo             = 71808,
    SpellFlyBack                = 72849,
    SpellFuneralEventCamera     = 94244,
    SpellFuneralExit            = 94260,
    SpellFlamingBoulder         = 70717,
    SpellCuttoBlack             = 122343
};

enum eActions
{
    ActionFrightenedCitizen1    = 100,
    ActionFrightenedCitizen2    = 101,
    ActionSeanDempsey           = 102,
    ActionLornaCrowley          = 103,
    ActionKingGreymanesHorse    = 104,
    ActionCommandeeredCannon    = 105,
    ActionCrowleysHorse         = 106,
    ActionKrennanAranas         = 107,
    ActionLordGodfrey           = 108,
    ActionKingGennGreymane      = 109,
    ActionCynthia               = 110,
    ActionJames                 = 111,
    ActionAshley                = 112,
    ActionAttackMastiff         = 113,
    ActionLuciustheCruel        = 114,
    ActionGrandmaWah            = 115,
    ActionGrandmaWah2           = 116,
    ActionSwiftMountainHorse    = 117,
    ActionStagecoachHarness     = 118,
    ActionKoroththeHillbreaker  = 119,
    ActionTaldorenTracker       = 120,
    ActionTaldorenTracker2      = 121,
    ActionTobiasMistmantle      = 122,
    ActionStoutMountainHorse    = 123,
    ActionTobiasMistmantle2     = 124,
    ActionSylvanasWindrunner    = 125,
    ActionHighExecutorCrenshaw  = 126,
    ActionGeneralWarhowl        = 127,
    ActionCapturedRidingBat     = 128,
    ActionGilneanSurvivor       = 129
};

enum eMovies
{
    GilneasCathedral    = 21
};

enum eItems
{
    KorothsBanner   = 49742
};

enum eMaps
{
    MapGilneas = 654
};

enum eBroadcasts
{
    PlayerCynthia   = 36329,
    PlayerJames     = 36328,
    PlayerAshley    = 36331,
    PlayerCatapult  = 36408,
};

namespace Positions
{
    Position const g_NorthWestRoof1 =   { -1618.86f, 1505.68f, 70.24f, 3.91f };
    Position const g_NorthWestRoof2 =   { -1611.40f, 1498.49f, 69.82f, 3.79f };
    Position const g_SouthWestRoof1 =   { -1732.81f, 1526.34f, 55.39f, 0.01f };
    Position const g_SouthWestRoof2 =   { -1737.49f, 1526.11f, 55.51f, 0.01f };
    Position const g_NorthRoof1 =       { -1562.59f, 1409.35f, 71.66f, 3.16f };
    Position const g_NorthRoof1Move =   { -1593.38f, 1408.02f, 72.64f, 0.00f };
    Position const g_JosiahAvery1 =     { -1813.71f, 1428.18f, 12.54f, 1.91f };
    Position const g_JosiahPlayerJump = { -1791.84f, 1427.24f, 12.45f, 3.20f };
    Position const g_JosiahAveryJump =  { -1796.63f, 1427.73f, 12.46f, 0.00f };
    Position const g_LornaCrowley1 =    { -1789.75f, 1427.41f, 13.00f, 3.11f };
    Position const g_GilneanMastiff1 =  { -1788.74f, 1428.53f, 12.99f, 3.35f };
    Position const g_GilneanMastiff2 =  { -1788.55f, 1426.82f, 13.00f, 3.35f };
    Position const g_GreymanesHorse1 =  { -1801.44f, 1407.35f, 19.85f, 5.01f };
    Position const g_GreymanesHorse2 =  { -1676.16f, 1346.19f, 15.13f, 0.00f };
    Position const g_KrennanAranas1 =   { -1773.43f, 1430.11f, 19.78f, 1.65f };
    Position const g_BloodfangRight =   { -1796.11f, 1389.15f, 19.90f, 1.91f };
    Position const g_BloodfangLeft =    { -1762.16f, 1398.26f, 19.91f, 1.93f };
    Position const g_CrowleysHorse1 =   { -1714.88f, 1673.11f, 20.49f, 0.27f };
    Position const g_CrowleysHorse2 =   { -1566.71f, 1708.04f, 20.48f, 6.12f };
    Position const g_KrennanAranas2 =   { -1815.90f, 2283.85f, 42.48f, 2.47f };
    Position const g_LordGodfrey2 =     { -1843.18f, 2289.84f, 42.46f, 0.24f };
    Position const g_GennGreymane2 =    { -1846.70f, 2288.75f, 42.48f, 0.14f };
    Position const g_KrennanAranas3 =   { -1819.53f, 2291.25f, 42.32f, 0.00f };
    Position const g_LordGodfrey3 =     { -1821.92f, 2295.05f, 42.17f, 0.00f };
    Position const g_GennGreymane3 =    { -1821.09f, 2292.59f, 42.23f, 0.00f };
    Position const g_ForsakenAssassin = { -1930.07f, 2387.62f, 30.82f, 1.11f };
    Position const g_BoatRight =        { -2119.61f, 2674.33f, 13.88f, 0.00f };
    Position const g_BoatLeft =         { -2218.65f, 2575.91f, 14.35f, 0.00f };
    Position const g_LuciustheCruel =   { -2111.53f, 2329.95f, 7.390f, 0.22f };
    Position const g_LuciustheCruel2 =  { -2106.37f, 2331.10f, 7.238f, 0.00f };
    Position const g_GrandmaWah =       { -2098.37f, 2352.07f, 7.160f, 0.00f };
    Position const g_MountainHorse =    { -1899.18f, 2253.57f, 42.32f, 0.78f };
    Position const g_Koroth =           { -2272.33f, 1966.39f, 99.23f, 0.00f };
    Position const g_StolenBanner =     { -2201.47f, 1808.66f, 12.31f, 6.21f };
    Position const g_CaptainAsther =    { -2176.93f, 1807.72f, 12.72f, 3.10f };
    Position const g_Koroth2 =          { -2199.40f, 1865.87f, 17.56f, 4.62f };
    Position const g_TaldorenTracker1 = { -2134.14f, 1564.05f, -51.3f, 0.10f };
    Position const g_TaldorenTracker2 = { -2043.08f, 1528.51f, -3.81f, 5.77f };
    Position const g_TobiasMistmantle = { -2146.12f, 1583.35f, -48.8f, 1.23f };
    Position const g_LornaCrowley2 =    { -2107.43f, 1283.57f, -83.4f, 0.00f };
    Position const g_GennGreymane1 =    { -2117.18f, 1291.66f, -81.6f, 0.00f };
    Position const g_LordGodfrey1 =     { -2119.80f, 1290.00f, -80.1f, 0.00f };
    Position const g_TobiasMistmantl2 = { -2064.77f, 1292.22f, -84.2f, 0.00f };
    Position const g_BattleGilneas =    { -1659.40f, 1598.45f, 23.13f, 3.94f };
    Position const g_Tobias1 =          { -1661.33f, 1592.44f, 23.21f, 3.70f };
    Position const g_Tobias2 =          { -1589.10f, 1606.64f, 21.59f, 0.00f };
    Position const g_Tobias3 =          { -1578.49f, 1628.04f, 20.61f, 0.00f };
    Position const g_Tobias4 =          { -1541.38f, 1615.75f, 20.48f, 0.00f };
    Position const g_Tobias5 =          { -1617.57f, 1531.64f, 26.23f, 0.00f };
    Position const g_Sylvanas1 =        { -1566.79f, 1555.30f, 29.36f, 4.32f };
    Position const g_Crenshaw1 =        { -1566.79f, 1555.30f, 29.36f, 4.32f };
    Position const g_Warhowl1 =         { -1566.79f, 1555.30f, 29.36f, 4.32f };
    Position const g_CapturedBatEnd =   { -1304.23f, 1650.36f, 64.76f, 0.00f };
}

namespace Waypoints
{
    std::vector<G3D::Vector3> const g_NorthWestRoof1Move =
    {
        { -1630.62f, 1480.55f, 70.40f },
        { -1630.62f, 1480.55f, 70.40f },
        { -1636.01f, 1475.81f, 64.51f }
    };

    std::vector<G3D::Vector3> const g_NorthWestRoof2Move =
    {
        { -1637.26f, 1488.86f, 69.95f },
        { -1637.26f, 1488.86f, 69.95f },
        { -1642.45f, 1482.23f, 64.30f }
    };

    std::vector<G3D::Vector3> const g_SouthWestRoof1Move =
    {
        { -1718.31f, 1526.62f, 55.91f },
        { -1718.31f, 1526.62f, 55.91f },
        { -1717.86f, 1490.77f, 56.61f }
    };

    std::vector<G3D::Vector3> const g_SouthWestRoof2Move =
    {
        { -1718.31f, 1526.62f, 55.91f },
        { -1718.31f, 1526.62f, 55.91f },
        { -1717.86f, 1487.00f, 57.07f }
    };

    std::vector<G3D::Vector3> const g_GreymanesHorseStart =
    {
        { -1797.514f, 1397.043f, 20.07586f },
        { -1788.764f, 1378.293f, 20.07586f },
        { -1780.764f, 1369.043f, 20.07586f },
        { -1767.264f, 1358.043f, 19.82586f },
        { -1746.514f, 1358.543f, 20.07586f },
        { -1726.264f, 1354.293f, 19.82586f },
        { -1709.064f, 1348.535f, 19.78232f }
    };

    std::vector<G3D::Vector3> const g_GreymanesHorseEnd =
    {
        { -1664.807f, 1345.011f, 15.48518f },
        { -1662.807f, 1354.511f, 15.48518f },
        { -1667.307f, 1362.511f, 15.48518f },
        { -1674.307f, 1363.761f, 15.48518f },
        { -1686.057f, 1355.011f, 15.48518f },
        { -1691.057f, 1347.261f, 15.48518f },
        { -1705.807f, 1350.011f, 19.98518f },
        { -1731.307f, 1360.011f, 19.98518f },
        { -1744.807f, 1370.511f, 20.23518f },
        { -1758.807f, 1389.511f, 19.98518f },
        { -1768.307f, 1410.011f, 19.98518f },
        { -1771.557f, 1423.011f, 19.98518f },
        { -1770.955f, 1430.332f, 19.83506f }
    };

    std::vector<G3D::Vector3> const g_CrowleysHorseStart =
    {
        { -1735.470f, 1658.464f, 20.48043f },
        { -1732.723f, 1659.912f, 20.48043f }
    };

    std::vector<G3D::Vector3> const g_CrowleysHorseJump1 =
    {
        { -1706.941f, 1668.399f, 21.05144f },
        { -1685.810f, 1655.574f, 20.88366f },
        { -1677.310f, 1647.824f, 21.13366f },
        { -1672.810f, 1643.824f, 20.88366f },
        { -1668.156f, 1639.432f, 20.61058f },
        { -1666.711f, 1628.966f, 20.60086f },
        { -1670.711f, 1617.466f, 20.85086f },
        { -1693.648f, 1592.639f, 20.61058f },
        { -1707.855f, 1590.913f, 20.86050f },
        { -1719.034f, 1595.819f, 20.85894f },
        { -1720.337f, 1610.404f, 20.85839f },
        { -1711.656f, 1629.972f, 20.85899f },
        { -1698.168f, 1656.621f, 20.81776f },
        { -1696.424f, 1682.012f, 20.52616f },
        { -1684.424f, 1700.230f, 20.55438f },
        { -1658.309f, 1710.313f, 20.58983f },
        { -1609.069f, 1711.410f, 22.83985f },
        { -1593.819f, 1710.660f, 20.83985f },
        { -1592.095f, 1710.710f, 20.58986f }
    };

    std::vector<G3D::Vector3> const g_CrowleysHorseJump2 =
    {
        { -1564.396f, 1704.136f, 20.79760f },
        { -1554.230f, 1704.906f, 20.48522f },
        { -1549.896f, 1691.886f, 20.79760f },
        { -1545.027f, 1684.188f, 20.85992f },
        { -1542.729f, 1656.159f, 20.85992f },
        { -1541.296f, 1637.844f, 21.11023f },
        { -1520.510f, 1618.233f, 20.86054f },
        { -1498.438f, 1621.994f, 20.86054f },
        { -1468.074f, 1630.752f, 20.86054f },
        { -1441.665f, 1627.687f, 20.85019f },
        { -1429.677f, 1616.673f, 20.84468f },
        { -1418.013f, 1586.581f, 20.71917f },
        { -1428.013f, 1552.331f, 20.96917f },
        { -1447.507f, 1532.921f, 20.84473f },
        { -1463.178f, 1535.285f, 20.85602f },
        { -1483.016f, 1544.442f, 20.86054f },
        { -1505.644f, 1567.322f, 21.07463f }
    };

    std::vector<G3D::Vector3> const g_CrowleysHorseEnd =
    {
        { -1517.301f, 1574.261f, 26.27148f },
        { -1531.420f, 1581.928f, 26.87003f },
        { -1540.594f, 1574.429f, 29.20671f }
    };

    std::vector<G3D::Vector3> const g_CynthiaDespawn =
    {
        { -1970.527f, 2512.032f, 1.827797f },
        { -1949.968f, 2520.129f, 1.725489f },
        { -1926.536f, 2519.312f, 2.246772f }
    };

    std::vector<G3D::Vector3> const g_JamesDespawn =
    {
        { -1933.188f, 2541.863f, 1.76396f },
        { -1928.938f, 2542.863f, 1.76396f },
        { -1920.938f, 2542.863f, 1.76396f },
        { -1917.188f, 2547.113f, 1.76396f },
        { -1912.781f, 2558.720f, 1.51100f }
    };

    std::vector<G3D::Vector3> const g_AshleyDespawn =
    {
        { -1929.448f, 2558.799f, 13.0254f },
        { -1928.698f, 2559.299f, 13.0254f },
        { -1928.948f, 2558.049f, 13.0254f },
        { -1928.698f, 2554.299f, 13.0254f },
        { -1924.948f, 2553.049f, 13.0254f },
        { -1923.283f, 2552.308f, 12.7358f },
        { -1922.697f, 2553.542f, 12.9071f },
        { -1920.697f, 2556.792f, 8.90718f },
        { -1920.023f, 2558.055f, 7.07646f }
    };

    std::vector<G3D::Vector3> const g_SwiftMountainHorse =
    {
        { -1866.630f, 2291.625f, 42.51527f },
        { -1832.420f, 2321.030f, 38.17225f },
        { -1789.512f, 2357.157f, 37.47993f },
        { -1784.463f, 2370.525f, 40.24399f },
        { -1779.198f, 2422.809f, 55.33919f },
        { -1757.976f, 2462.610f, 68.40892f },
        { -1716.726f, 2466.110f, 81.15892f },
        { -1705.539f, 2469.926f, 85.21272f },
        { -1693.323f, 2493.177f, 95.14577f },
        { -1669.870f, 2520.097f, 97.97956f }
    };

    std::vector<G3D::Vector3> const g_StagecoachHarness =
    {
        { -1675.117f, 2518.685f, 97.89457f },
        { -1681.110f, 2508.630f, 97.85060f },
        { -1696.677f, 2486.415f, 92.34124f },
        { -1704.450f, 2473.580f, 87.09741f },
        { -1698.950f, 2429.330f, 75.84741f },
        { -1724.450f, 2392.580f, 61.59741f },
        { -1745.100f, 2339.655f, 68.03456f },
        { -1764.450f, 2290.830f, 78.59741f },
        { -1784.033f, 2262.941f, 84.13220f },
        { -1794.450f, 2255.080f, 87.09741f },
        { -1812.200f, 2245.830f, 89.59741f },
        { -1828.450f, 2232.330f, 89.84741f },
        { -1867.200f, 2172.830f, 89.84741f },
        { -1872.570f, 2117.760f, 89.31726f },
        { -1877.780f, 2055.420f, 89.31690f },
        { -1886.721f, 2018.716f, 89.31711f },
        { -1881.360f, 1990.490f, 89.43994f },
    };

    std::vector<G3D::Vector3> const g_StagecoachHarness2 =
    {
        { -1878.692f, 1971.615f, 89.46884f },
        { -1875.942f, 1953.865f, 89.46884f },
        { -1881.688f, 1913.211f, 89.43948f },
        { -1908.414f, 1902.409f, 89.28595f },
        { -1988.700f, 1901.355f, 89.45432f },
        { -2018.125f, 1907.281f, 87.27414f },
        { -2042.809f, 1914.195f, 81.44321f },
        { -2067.909f, 1902.200f, 70.45465f },
        { -2087.244f, 1886.532f, 57.75508f },
        { -2102.434f, 1870.239f, 46.72014f },
        { -2109.848f, 1849.656f, 37.73809f },
        { -2135.848f, 1821.906f, 23.48809f },
        { -2180.848f, 1807.156f, 12.98809f },
        { -2213.640f, 1808.830f, 11.83050f }
    };

    std::vector<G3D::Vector3> const g_StagecoachHarnessDespawn =
    {
        { -2276.135f, 1788.580f, 11.57640f },
        { -2300.130f, 1780.330f, 11.32229f },
        { -2319.799f, 1761.530f, 11.18315f }
    };

    std::vector<G3D::Vector3> const g_KorothHillbreaker =
    {
        { -2199.060f, 1863.932f, 17.51848f },
        { -2196.720f, 1811.491f, 12.46838f },
        { -2179.756f, 1808.339f, 12.72839f }
    };

    std::vector<G3D::Vector3> const g_LornaCrowley1 =
    {
        { -2111.985f, 1285.762f, -83.08337f },
        { -2089.681f, 1275.028f, -84.46104f }
    };

    std::vector<G3D::Vector3> const g_LornaCrowley2 =
    {
        { -2089.681f, 1275.028f, -84.46104f },
        { -2087.436f, 1275.573f, -84.62315f },
        { -2071.692f, 1277.617f, -85.28526f }
    };

    std::vector<G3D::Vector3> const g_KingGennGreymane1 =
    {
        { -2117.964f, 1292.304f, -81.22814f },
        { -2095.382f, 1273.814f, -83.49229f },
        { -2077.418f, 1272.584f, -85.28526f }
    };

    std::vector<G3D::Vector3> const g_LordGodfrey1 =
    {
        { -2119.808f, 1290.003f, -80.25018f },
        { -2113.088f, 1289.877f, -82.93607f },
        { -2100.000f, 1283.333f, -83.81107f },
        { -2095.834f, 1282.813f, -83.91026f },
        { -2093.750f, 1281.250f, -83.78526f },
        { -2089.666f, 1281.250f, -83.53526f },
        { -2087.500f, 1282.291f, -83.41026f },
        { -2083.334f, 1281.250f, -84.28526f },
        { -2080.084f, 1278.914f, -85.78526f }
    };

    std::vector<G3D::Vector3> const g_TobiasMistmantle1 =
    {
        { -2064.117f, 1294.119f, -84.08433f },
        { -2068.735f, 1280.735f, -85.38673f }
    };

    std::vector<G3D::Vector3> const g_TobiasMistmantle2 =
    {
        { -2068.735f, 1280.735f, -85.38673f },
        { -2073.486f, 1301.439f, -83.20079f },
        { -2075.736f, 1312.939f, -83.45079f },
        { -2079.486f, 1328.439f, -82.70079f },
        { -2079.736f, 1330.439f, -82.45079f },
        { -2080.236f, 1331.142f, -82.51484f }
    };

    std::vector<G3D::Vector3> const g_StoutMountainHorse1 =
    {
        { -2049.250f, 959.6590f, 70.11994f },
        { -2049.280f, 959.6890f, 70.18994f },
        { -2051.218f, 946.5648f, 69.94863f },
        { -2011.828f, 932.1172f, 62.58153f },
        { -1998.221f, 914.1250f, 57.80591f },
        { -1986.538f, 878.9733f, 61.25958f },
        { -1970.960f, 863.9561f, 64.86116f },
        { -1933.518f, 858.5723f, 69.30852f },
        { -1907.408f, 861.2493f, 76.85585f },
        { -1882.361f, 878.5790f, 82.25259f },
        { -1878.588f, 901.1068f, 80.92768f },
        { -1876.088f, 922.6068f, 79.17768f },
        { -1877.815f, 950.7874f, 78.05544f },
        { -1885.392f, 980.9464f, 76.20494f },
        { -1879.916f, 1016.281f, 68.72333f },
        { -1870.748f, 1046.192f, 61.28024f }
    };

    std::vector<G3D::Vector3> const g_StoutMountainHorse2 =
    {
        { -1870.748f, 1046.192f, 61.28024f },
        { -1857.934f, 1060.521f, 57.64919f },
        { -1851.385f, 1089.852f, 49.99001f },
        { -1846.083f, 1138.298f, 38.04576f },
        { -1858.474f, 1219.528f, 23.13380f },
        { -1840.959f, 1211.818f, 20.72514f },
        { -1807.560f, 1183.729f, 20.67065f },
        { -1711.668f, 1145.854f, 26.52302f },
        { -1688.125f, 1134.279f, 26.29358f },
        { -1649.340f, 1113.352f, 28.81067f },
        { -1614.438f, 1113.786f, 29.81935f },
        { -1576.159f, 1120.797f, 30.52411f }
    };

    std::vector<G3D::Vector3> const g_StoutMountainHorse3 =
    {
        { -1576.159f, 1120.797f, 30.52411f },
        { -1549.029f, 1119.651f, 30.71073f },
        { -1532.758f, 1112.951f, 30.22555f },
        { -1509.648f, 1094.925f, 29.70753f },
        { -1494.842f, 1085.970f, 29.71576f },
        { -1478.377f, 1082.035f, 29.71868f },
        { -1443.679f, 1082.329f, 29.29541f },
        { -1434.929f, 1084.329f, 29.29541f },
        { -1408.578f, 1089.540f, 29.25032f },
        { -1365.325f, 1114.263f, 31.41663f },
        { -1332.648f, 1122.527f, 35.65106f },
        { -1321.146f, 1123.042f, 36.84188f },
        { -1317.936f, 1133.676f, 37.19041f },
        { -1364.035f, 1186.955f, 35.25325f }
    };

    std::vector<G3D::Vector3> const g_Tobias1 =
    {
        { -1655.753f, 1603.838f, 23.35051f },
        { -1657.503f, 1607.088f, 23.35051f },
        { -1661.253f, 1610.088f, 21.60051f },
        { -1663.503f, 1615.338f, 20.85051f },
        { -1659.003f, 1620.838f, 20.85051f },
        { -1650.503f, 1620.588f, 20.85051f },
        { -1641.503f, 1617.838f, 20.85051f },
        { -1629.172f, 1613.231f, 20.48811f }
    };

    std::vector<G3D::Vector3> const g_Tobias2 =
    {
        { -1568.589f, 1623.524f, 20.67323f },
        { -1562.839f, 1623.024f, 20.92323f },
        { -1557.688f, 1622.509f, 20.73626f }
    };

    std::vector<G3D::Vector3> const g_Tobias3 =
    {
        { -1523.842f, 1608.757f, 20.61054f },
        { -1508.776f, 1596.195f, 20.48653f },
        { -1497.075f, 1579.869f, 20.86054f },
        { -1490.689f, 1576.042f, 20.61054f }
    };

    std::vector<G3D::Vector3> const g_Tobias4 =
    {
        { -1500.058f, 1577.252f, 21.07350f },
        { -1503.558f, 1576.752f, 21.07350f },
        { -1509.808f, 1578.752f, 23.57350f },
        { -1511.808f, 1579.252f, 24.32350f },
        { -1517.308f, 1580.752f, 26.32350f },
        { -1521.558f, 1582.002f, 26.82350f },
        { -1525.405f, 1583.029f, 27.06539f },
        { -1529.405f, 1580.529f, 27.06539f },
        { -1531.155f, 1579.279f, 27.06539f },
        { -1535.905f, 1576.029f, 28.56539f },
        { -1538.405f, 1575.029f, 29.31539f },
        { -1541.155f, 1573.029f, 29.31539f },
        { -1545.585f, 1570.170f, 29.45258f },
        { -1548.085f, 1569.170f, 29.45258f },
        { -1550.085f, 1568.670f, 29.45258f },
        { -1552.835f, 1567.420f, 29.45258f },
        { -1560.862f, 1566.384f, 29.45713f },
        { -1564.859f, 1562.234f, 29.20217f },
        { -1566.230f, 1552.779f, 29.45208f },
        { -1569.230f, 1548.529f, 29.45208f },
        { -1571.424f, 1546.017f, 29.46566f },
        { -1573.424f, 1544.017f, 29.46566f },
        { -1578.674f, 1538.767f, 29.46566f },
        { -1586.613f, 1531.092f, 29.48014f },
        { -1591.113f, 1528.842f, 29.48014f },
        { -1593.613f, 1529.842f, 29.48014f },
        { -1600.026f, 1531.869f, 29.48048f },
        { -1603.026f, 1532.869f, 29.48048f },
        { -1609.750f, 1535.884f, 29.22817f }
    };

    std::vector<G3D::Vector3> const g_Sylvanas1 =
    {
        { -1566.286f, 1556.543f, 29.51530f },
        { -1567.169f, 1550.709f, 29.20722f },
        { -1603.868f, 1522.163f, 29.24303f }
    };

    std::vector<G3D::Vector3> const g_Sylvanas2 =
    {
        { -1603.868f, 1522.163f, 29.24303f },
        { -1568.255f, 1549.149f, 29.45688f },
        { -1567.505f, 1551.649f, 29.45688f },
        { -1565.797f, 1555.969f, 29.21154f }
    };

    std::vector<G3D::Vector3> const g_Crenshaw1 =
    {
        { -1567.511f, 1553.954f, 29.28596f },
        { -1567.261f, 1550.704f, 29.28596f },
        { -1578.051f, 1538.749f, 29.46135f },
        { -1591.288f, 1521.837f, 29.23878f },
        { -1596.321f, 1521.710f, 29.24288f }
    };

    std::vector<G3D::Vector3> const g_Crenshaw2 =
    {
        { -1596.321f, 1521.710f, 29.24288f },
        { -1580.594f, 1536.144f, 29.22161f },
        { -1572.097f, 1544.972f, 29.20324f }
    };

    std::vector<G3D::Vector3> const g_Warhowl1 =
    {
        { -1565.554f, 1556.788f, 29.51535f },
        { -1560.393f, 1551.679f, 29.23084f },
        { -1561.595f, 1550.911f, 29.46516f },
        { -1565.095f, 1549.661f, 29.46516f },
        { -1577.810f, 1537.636f, 29.21927f },
        { -1590.626f, 1525.167f, 29.48687f },
        { -1594.793f, 1526.280f, 29.23890f }
    };

    std::vector<G3D::Vector3> const g_Warhowl2 =
    {
        { -1594.793f, 1526.280f, 29.23890f },
        { -1566.405f, 1550.673f, 29.44441f },
        { -1564.655f, 1552.423f, 29.44441f },
        { -1566.941f, 1559.931f, 29.18744f }
    };

    std::vector<G3D::Vector3> const g_CapturedRidingBatStart =
    {
        { -1665.710f, 1639.311f, 27.45119f },
        { -1665.715f, 1639.310f, 27.45118f },
        { -1663.250f, 1621.879f, 25.96168f },
        { -1637.625f, 1617.241f, 33.15612f },
        { -1595.771f, 1606.172f, 44.07281f },
        { -1545.931f, 1598.307f, 44.07281f },
        { -1448.483f, 1587.660f, 44.07281f },
        { -1395.726f, 1615.543f, 44.07281f },
        { -1319.311f, 1680.958f, 44.07281f },
        { -1241.325f, 1747.767f, 53.76252f }
    };

    std::vector<G3D::Vector3> const g_CapturedRidingBatLoop1 =
    {
        { -1241.325f, 1747.767f, 53.76252f },
        { -1186.814f, 1708.936f, 53.76252f },
        { -1107.872f, 1646.177f, 53.76252f },
        { -1056.335f, 1608.783f, 53.76252f },
        { -984.1858f, 1623.411f, 64.90141f },
        { -960.5712f, 1668.052f, 69.84588f },
        { -919.2639f, 1708.543f, 90.70698f },
        { -859.6215f, 1700.193f, 90.70698f },
        { -864.6215f, 1626.049f, 90.70698f }
    };

    std::vector<G3D::Vector3> const g_CapturedRidingBatLoop2 =
    {
        { -887.0070f, 1510.622f, 90.70698f },
        { -950.3246f, 1506.309f, 82.34586f },
        { -1025.245f, 1428.929f, 74.51246f },
        { -1077.819f, 1401.005f, 58.45690f },
        { -1143.061f, 1407.379f, 51.04022f },
        { -1197.042f, 1471.483f, 47.34577f },
    };

    std::vector<G3D::Vector3> const g_CapturedRidingBatLoop3 =
    {
        { -1269.991f, 1510.726f, 47.34577f },
        { -1352.618f, 1566.734f, 47.34577f },
        { -1379.911f, 1681.130f, 47.34577f },
        { -1304.021f, 1763.345f, 54.04024f },
        { -1277.788f, 1762.722f, 55.95692f }
    };

    std::vector<G3D::Vector3> const g_CapturedRidingBatEnd =
    {
        { -1304.236f, 1650.365f, 64.76084f },
        { -1410.293f, 1638.580f, 37.38527f },
        { -1491.441f, 1632.970f, 32.07970f },
        { -1578.608f, 1633.108f, 32.07970f },
        { -1621.252f, 1625.786f, 29.66303f },
        { -1637.115f, 1616.464f, 28.71857f },
        { -1657.575f, 1619.925f, 25.35749f },
        { -1665.670f, 1630.521f, 25.35749f },
        { -1667.727f, 1662.474f, 22.60748f }
    };

    std::vector<G3D::Vector3> const g_GilneanSurvivor =
    {
        { -1290.950f, 2101.679f, 5.802049f },
        { -1283.700f, 2099.179f, 5.302049f },
        { -1283.311f, 2094.073f, 6.906514f },
        { -1283.561f, 2090.323f, 6.906514f },
        { -1283.811f, 2083.573f, 10.90651f }
    };
}

static std::array<Position const, 9> g_AttackMastiffPos =
{{
    { -1994.557f, 2672.134f, -2.303949f, 0.576510f },
    { -1972.606f, 2639.383f, 1.2116730f, 1.217762f },
    { -1973.627f, 2654.836f, -0.699540f, 1.098488f },
    { -1944.483f, 2656.656f, 1.0514410f, 1.691914f },
    { -1997.009f, 2650.811f, -1.030188f, 0.818140f },
    { -1983.201f, 2662.242f, -1.666527f, 0.862712f },
    { -1949.314f, 2642.024f, 1.2990830f, 1.580580f },
    { -1956.602f, 2649.942f, 1.3742570f, 1.441321f },
    { -2006.259f, 2663.115f, -2.004312f, 0.594008f }
}};

static std::array<Position const, 12> g_ForsakenSoldierPos =
{{
    { -2166.85f, 1807.18f, 13.9673f, 3.15534f },
    { -2172.53f, 1804.56f, 12.9612f, 3.17824f },
    { -2169.79f, 1809.31f, 13.6041f, 3.16646f },
    { -2172.61f, 1806.79f, 13.0548f, 3.17824f },
    { -2172.78f, 1811.48f, 13.2734f, 3.17824f },
    { -2169.74f, 1807.21f, 13.4491f, 3.15534f },
    { -2172.71f, 1809.38f, 13.1777f, 3.17824f },
    { -2166.88f, 1809.28f, 14.1292f, 3.15534f },
    { -2166.91f, 1811.52f, 14.3392f, 3.15534f },
    { -2166.82f, 1804.80f, 13.8117f, 3.15534f },
    { -2169.85f, 1811.55f, 13.7588f, 3.16646f },
    { -2169.71f, 1804.76f, 13.3213f, 3.15534f }
}};

static std::array<Position const, 6> g_GilneanSurvivorPos =
{{
    { -1332.800f, 2145.755f, 5.814756f, 5.420936f },
    { -1324.750f, 2096.590f, 5.714315f, 6.093734f },
    { -1354.276f, 2119.517f, 6.232161f, 5.998109f },
    { -1366.750f, 2122.918f, 9.094904f, 6.002673f },
    { -1350.252f, 2130.688f, 5.816634f, 5.728516f },
    { -1301.439f, 2133.847f, 5.549032f, 4.942795f }
}};

#endif
