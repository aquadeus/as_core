////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef RETURN_TO_KARAZHAN_HPP
# define RETURN_TO_KARAZHAN_HPP

# include "BrokenIslesPCH.h"

enum eDungeons
{
    RTKLower    = 1475,
    RTKSummit   = 1474
};

enum eData
{
    OperaHall,
    MaidenVirtue,
    DataMidnight,
    DataMoroes,

    DataNightbane,

    DataCurator,
    ManaDevourer,
    DataShadeOfMedivh,
    Vizaduum,

    MaxEncounter,

    OperaHallIntro,
    OperaHallScene,

    OperaHallWikket          = 1,
    OperaHallWestfall        = 2,
    OperaHallBeautifulBeast  = 3,

    CancelCristalTimer,
    CannonTargetGUID,
    VizaduumHPPhase,
    Phase2Door                        ///< After kill Moroes
};

enum eCreatures
{
    /// Opera Balcony
    SpectralPatron                  = 114541,

    /// Opera Hall:
    TheatreSpotlight                = 18342,

    /// Wikket
    TheMonkeyKing                   = 115022,
    CagedAssistant                  = 115005,
    HozenCage                       = 115007,
    WingedAssistant                 = 114324,
    Galindre                        = 114251, ///< Boss
    Elfyra                          = 114284, ///< Boss
    CroneBroom                      = 114486,

    /// Westfall
    Mrrgria                         = 114260, ///< Boss
    ToeKnee                         = 114261, ///< Boss
    GangRuffian                     = 114265,
    ShorelineTidespeaker            = 114266,
    WashAway                        = 114471,

    /// Beautiful Beast
    Coggleston                      = 114328,
    Luminore                        = 114329,
    Babblet                         = 114330,
    Cauldrons                       = 114522,
    Bella                           = 114545,
    Adem                            = 114583,
    Brute                           = 114551,

    /// Maiden of Virtue
    MaidenOfVirtue                  = 113971,
    AnduinMaiden                    = 115489,
    LlaneMaiden                     = 115490,
    MedivhMaiden                    = 115487,

    /// Attumen the Huntsman
    AttumenTheHuntsman              = 114262,
    Midnight                        = 114264,
    IntangiblePresence              = 114315,

    /// Banquet Hall Ghostbuster Event
    PhantomGuest                    = 114625,
    EgonaSpangly                    = 114552,
    PetaVenker                      = 114554,

    /// Moroes
    Moroes                          = 114312, ///< Boss
    BaronessDorothea                = 114316, ///< Baroness Dorothea Millstipe
    LadyCatriona                    = 114317, ///< Lady Catriona Von'Indi
    BaronRafe                       = 114318, ///< Baron Rafe Dreuger
    LadyKeira                       = 114319, ///< Lady Keira Berrybuck
    LordRobin                       = 114320, ///< Lord Robin Daris
    LordCrispin                     = 114321, ///< Lord Crispin Ference

    WhirlingEdge                    = 114327, ///< Lord Robin Daris - Summons

    /// After death Moroes
    SceneActor                      = 116804,
    Medivh                          = 115426,
    NielasAran                      = 115427,

    /// The Curator
    Curator                         = 114247,
    VolatileEnergy                  = 114249,
    CuratorVision                   = 114462,
    CuratorMedivh                   = 115491,
    CuratorCristal                  = 115113,

    /// Nightbane
    Nightbane                       = 114895,
    MoroesCristal                   = 115103,
    MaidenCristal                   = 115013,
    OperaCristal                    = 115105,
    ServantsQuartersCristal         = 115101,
    ImageOfArcanagos                = 115213,

    /// Library
    InfusedPyromancer               = 115488,

    /// Shade of Medivh
    ShadeOfMedivh                   = 114350,
    GuardianImage                   = 114675,

    /// Mana Devourer
    CreatureManaDevourer            = 114252,
    LittleManaDevourer              = 115831,
    RodentOfUsualSize               = 116802,

    /// Third library
    ArchmageKhadgarLibrary          = 115501,
    LotharLibrary                   = 115489,
    MedivhLibrary                   = 000000,

    /// Gamesman's Room
    King                            = 115388,
    Queen                           = 115395,
    Rook                            = 115407,
    Knight                          = 115406,
    BlackBishop                     = 115402,
    WhiteBishop                     = 115401,
    ArchmageKhadgarChess            = 116259,
    MedivhChess                     = 116124,

    /// Vizaduum the Watcher
    VizaduumTheWatcher              = 114790,
    FelCannon                       = 115274,
    CommandShip                     = 114913,
    SoulHarvester                   = 115694,
    LegionConsole                   = 115743,
    FelguardSentry                  = 115730,
    ShadowSpitter                   = 115734,
    DemonicPortal                   = 115493,

    /// After death scene
    ArchmageKhagdarVizaduum         = 115497,
    MedivhVizaduum                  = 114463,

    /// Others
    ArchmageKhadgarEntrance         = 115496,
    BarnesTheStageManager           = 114339,
    RLCooperTheCareTaker            = 116573,
    ShriekingTerror                 = 114627,
    ForlornSpirit                   = 114626,
    KorenTheBlacksmith              = 114815,
    CalliardTheNightman             = 114799,
};

enum eGameObjects
{
    /// Opera GO
    OperaSceneVelum                 = 259684,
    OperaSceneLeftDoor              = 259694,
    OperaSceneRightDoor             = 259702,
    OperaDecorWikket1               = 260311, ///< Wikket
    OperaSceneWikket2               = 260312,
    OperaSceneWikket3               = 260313,
    OperaDecorWestfall1             = 260304, ///< Westfall
    OperaDecorWestfall2             = 260307,
    OperaDecorWestfall3             = 260306,
    OperaDecorWestfall4             = 260305,
    OperaDecorWestfall5             = 260308,
    OperaDecorBeautifulBeast1       = 260298, ///< Beautiful Beast
    OperaDecorBeautifulBeast2       = 260299,
    OperaDecorBeautifulBeast3       = 260300,
    OperaDecorBeautifulBeast4       = 260301,
    OperaDecorBeautifulBeast5       = 260303,
    OperDoorToMoroes                = 259730,

    /// Moroes GO
    MoroesDoor1                     = 269977,
    MoroesDoor2                     = 269978,
    GhostTrap                       = 259928,

    DoorInstancePH2_1               = 259689,
    DoorInstancePH2_2               = 266803,
    DoorInstancePH2_3               = 266804,

    /// The Curator
    CuratorDoor                     = 266508,

    /// Shade of Medivh
    MedivhDoor1                     = 266846,
    MedivhDoor2                     = 266510,

    /// Gamesman's Room
    GamesmansRoomWall               = 266656,
    ShatteredBoard                  = 266295,

    /// Vizaduum the watcher
    VizaduumDoor1                   = 266593,
    PortalDalaran                   = 266857,

    ChallengersCache                = 252680
};

enum eOperaActions
{
    WikketIntroEnd = 1,
    ZoneInCombat,
    EncounterDespawn
};

Position const g_OperaSpawnPos[40] =
{
    /// Wikket
    { -10874.0f, -1757.86f, 90.560f, 3.23f }, ///< 0  - 115005
    { -10872.5f, -1751.04f, 90.560f, 3.90f }, ///< 1  - 115005
    { -10896.3f, -1745.27f, 90.550f, 4.55f }, ///< 2  - 115005
    { -10905.9f, -1743.38f, 90.550f, 5.06f }, ///< 3  - 115005
    { -10880.5f, -1747.40f, 90.550f, 3.90f }, ///< 4  - 115005
    { -10872.7f, -1751.13f, 90.560f, 3.90f }, ///< 5  - 115007
    { -10896.4f, -1745.43f, 90.550f, 4.55f }, ///< 6  - 115007
    { -10874.1f, -1757.86f, 90.560f, 3.23f }, ///< 7  - 115007
    { -10905.8f, -1743.53f, 90.550f, 5.06f }, ///< 8  - 115007
    { -10880.6f, -1747.50f, 90.550f, 3.90f }, ///< 9  - 115007
    { -10888.8f, -1756.79f, 104.94f, 4.59f }, ///< 10 - 114251 - intro pos
    { -10891.5f, -1761.89f, 90.470f, 4.56f }, ///< 11 - 114251 - final pos
    { -10876.3f, -1805.15f, 116.51f, 1.67f }, ///< 12 - 114284 - intro pos
    { -10896.4f, -1758.50f, 90.480f, 4.58f }, ///< 13 - 114284 - final pos

    /// Westfall
    { -10879.00f, -1756.53f, 90.55f, 2.9f }, ///< 14 - 114260
    { -10905.00f, -1755.24f, 90.55f, 0.0f }, ///< 15 - 114261
    { -10908.70f, -1760.70f, 90.56f, 0.4f }, ///< 16 - 114265
    { -10909.40f, -1756.20f, 90.56f, 6.2f }, ///< 17 - 114265
    { -10907.40f, -1752.71f, 90.55f, 5.9f }, ///< 18 - 114265
    { -10874.10f, -1756.75f, 90.56f, 2.7f }, ///< 19 - 114266
    { -10872.50f, -1759.25f, 90.56f, 2.7f }, ///< 20 - 114266
    { -10874.50f, -1753.44f, 90.56f, 3.6f }, ///< 21 - 114266

    { -10913.78f, -1741.09f, 90.55f, 6.2f }, ///< 22 - 114471
    { -10913.08f, -1749.38f, 90.56f, 6.2f }, ///< 23 - 114471
    { -10913.13f, -1756.79f, 90.56f, 6.2f }, ///< 24 - 114471
    { -10916.40f, -1763.52f, 90.56f, 6.2f }, ///< 25 - 114471
    { -10916.08f, -1771.52f, 90.56f, 6.2f }, ///< 26 - 114471
    { -10913.43f, -1779.65f, 90.56f, 6.2f }, ///< 27 - 114471
    { -10867.05f, -1751.13f, 90.56f, 3.0f }, ///< 28 - 114471
    { -10870.98f, -1758.55f, 90.56f, 3.0f }, ///< 29 - 114471
    { -10870.82f, -1766.00f, 90.56f, 3.0f }, ///< 30 - 114471
    { -10870.89f, -1773.27f, 90.56f, 3.0f }, ///< 31 - 114471
    { -10871.60f, -1780.78f, 90.56f, 3.0f }, ///< 32 - 114471
    { -10878.17f, -1787.46f, 90.56f, 3.0f }, ///< 33 - 114471

    /// Beautiful Beast
    { -10883.4f, -1752.55f, 90.55f, 3.74f }, ///< 34 - 114328
    { -10888.2f, -1751.59f, 90.55f, 3.62f }, ///< 35 - 114329
    { -10892.2f, -1749.48f, 90.55f, 3.74f }, ///< 36 - 114330
    { -10884.7f, -1757.01f, 90.56f, 3.74f }, ///< 37 - 114522
    { -10903.3f, -1752.55f, 90.55f, 3.60f }, ///< 38 - 114545
    { -10906.3f, -1754.22f, 90.55f, 0.55f }  ///< 39 - 114583
};

static uint64 ExtractNumberFromBytes(uint64 p_Bytes, uint8 p_Position /*From the right*/, uint8 p_Size)
{
    if ((p_Position + p_Size) > 63)
        return 0;

    return ((p_Bytes >> static_cast<uint64>(p_Position)) % (1ll << static_cast<uint64>(p_Size)));
};

#endif ///< RETURN_TO_KARAZHAN_HPP
