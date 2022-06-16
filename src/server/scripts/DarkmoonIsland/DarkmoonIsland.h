////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

const Position g_ATFireRingChallengePos[56] =
{
    {-4177.096f, 6412.299f, 53.74761f, 0.9677705f},
    {-4153.389f, 6421.354f, 49.73269f, 0.7785892f},
    {-4200.064f, 6404.912f, 49.83862f, 1.364654f},
    {-4123.092f, 6420.391f, 79.5131f, 0.5568334f},
    {-4000.738f, 6319.748f, 61.96915f, 6.056061f},
    {-4298.453f, 6435.71f, 99.74864f, 2.497026f},
    {-3976.156f, 6239.41f, 59.68959f, 5.777801f},
    {-4061.708f, 6240.754f, 55.67439f, 5.568335f},
    {-4417.354f, 6368.175f, 34.5676f, 3.138926f},
    {-4411.45f, 6469.171f, 37.86602f, 2.678885f},
    {-4416.028f, 6263.413f, 30.60862f, 3.605707f},
    {-4464.976f, 6074.34f, 56.51786f, 3.992603f},
    {-4384.887f, 6059.405f, 30.4877f, 4.191037f},
    {-4387.158f, 6218.971f, 85.1059f, 3.833803f},
    {-4278.502f, 6305.27f, 55.3745f, 3.864719f},
    {-4219.653f, 6288.33f, 56.897f, 4.564775f},
    {-4127.677f, 6190.193f, 87.08089f, 5.13686f},
    {-4204.17f, 6195.492f, 80.76868f, 4.73384f},
    {-4282.986f, 6089.893f, 74.05066f, 4.448216f},
    {-4327.917f, 6013.772f, 118.3816f, 4.385294f},
    {-4276.29f, 6004.999f, 98.62133f, 4.52588f},
    {-4152.132f, 6029.343f, 112.4916f, 4.875675f},
    {-3994.031f, 6130.03f, 143.9004f, 5.445213f},
    {-3888.948f, 6244.342f, 71.50654f, 5.914334f},
    {-3871.005f, 6321.215f, 78.78794f, 6.146299f},
    {-3931.849f, 6370.7f, 73.34687f, 0.01116927f},
    {-4015.245f, 6472.497f, 85.551f, 0.4986074f},
    {-3860.811f, 6494.958f, 97.87495f, 0.3516689f},
    {-4163.147f, 6539.726f, 78.76099f, 1.316605f},
    {-4265.304f, 6513.369f, 93.58659f, 1.946203f},
    {-4231.226f, 6586.085f, 93.67531f, 1.677328f},
    {-4162.233f, 6763.257f, 25.24334f, 1.455971f},
    {-4088.217f, 6675.956f, 53.79656f, 1.200647f},
    {-4319.019f, 6655.865f, 24.96414f, 1.93885f},
    {-3833.854f, 6670.972f, 60.10976f, 0.6814674f},
    {-3754.766f, 6475.822f, 64.21665f, 0.2344222f},
    {-3781.519f, 6438.263f, 61.80006f, 0.1642108f},
    {-3794.465f, 6427.009f, 69.57449f, 0.1426917f},
    {-3861.29f, 6380.367f, 55.15146f, 0.03677358f},
    {-3812.991f, 6369.193f, 118.9729f, 0.003991582f},
    {-4142.12f, 5947.434f, 119.1986f, 4.867595f},
    {-4196.168f, 5890.585f, 127.7626f, 4.7369f},
    {-4237.936f, 5942.774f, 117.577f, 4.641721f},
    {-4311.99f, 5950.87f, 104.305f, 4.467546f},
    {-4068.458f, 5933.453f, 115.6255f, 5.023079f},
    {-3969.863f, 5989.153f, 113.2228f, 5.273759f},
    {-3887.325f, 6097.111f, 116.1136f, 5.582238f},
    {-3817.255f, 6127.589f, 126.3984f, 5.732175f},
    {-3760.503f, 6175.529f, 123.6823f, 5.87761f},
    {-4149.393f, 6684.266f, 31.54233f, 1.388198f},
    {-3951.658f, 6719.8f, 37.38012f, 0.9418668f},
    {-4147.766f, 6777.004f, 32.39946f, 1.425039f},
    {-3792.443f, 6895.78f, 26.00098f, 0.9043181f},
    {-3740.898f, 6846.799f, 56.73854f, 0.7983093f},
    {-3777.356f, 6723.497f, 24.15144f, 0.6907824f},
    {-3880.603f, 6715.769f, 22.8216f, 0.8163199f}
};

enum eMusic
{
    PowerOfTheHorde = 11803,
};

enum Actions
{
    SpawnDeathmatchChest = 1,
    Warn30MinutesLeft,
    LaunchAnotherTarget,
    LaunchQuickshotAchievement,
    SummonNewWolf,
    DreadhowlMoonfangSpawn
};

enum Events
{
    /// Concert
    Warn15MinutesLeft = 1,
    Warn5MinutesLeft,
    ConcertChieftainsBegin,
    SummonSamuro,
    SummonMayKil,
    SummonSig,
    SummonBergrisst,
    SummonThunderChief,
    SamCustom2,
    BergrCustom1,
    SigCustom2,
    MayKilCustom1,
    SamuroCustom3,
    MaykylCustom3,
    MayKylCustom2,
    ChiefCustom3,
    EndOfConcert,

    /// Mini game shot
    LaunchTargetIndicator,
    AbortQuickshotAchievement,

    /// Moonfang Wolves
    CripplingBiteEvent,
    HorrificHowlEvent,
    LeapForTheKillEvent,
    CallOfThePackEvent,
    MoonfangsTearsEvent
};

enum eSpellDarkmoonFaire
{
    /// Stay out area gaming
    StayOutToss                 = 109972,
    StayOutTonk                 = 109976,
    StayOutGnoll                = 109977,
    StayOutCarousel             = 134810,
    PowerBarGnoll               = 101612,
    PowerBarTonk                = 100752,

    /// Darkmoon Arena
    EnterDeathmatch             = 108919,
    ExitDeathmatch              = 108923,

    /// Darkmoon Guide
    DarkmoonAdventurerAura      = 185381,
    DarkmoonTrophyAura          = 100775,

    /// Race
    Wanderluster                = 179256,
    Powermonger                 = 179251,
    Rocketeer                   = 179252,
    RacingStrider               = 179750,
    RaceCancelled               = 149066,
    HazardImmunity              = 148975,

    /// Darkmoon Rabbit
    HugeSharpTeeth              = 114078,

    /// Moonfang Wolves
    CripplingBite               = 144519,
    DreadfulHowl                = 144564,
    LeapForTheKill              = 144546,

    /// Firebird
    SummonRingFirebird          = 170815,

    /// Ring toss
    RingTossCredit              = 101807,
    VisualOneRing               = 101736,
    VisualTwoRing               = 101738,

    /// Cannon game
    Bullseye                    = 62173,
    GreatShot                   = 62175,
    PoorShot                    = 62179,
    CannonBlastUIBar            = 102121,
    CannonCredit                = 100962,
    Cannonblast                 = 62244,

    /// Shot mini game
    TargetIndicator             = 43313,
    ShotOverride                = 101872,
    ShotCredit                  = 43300,

    /// Toys
    FlingRings                  = 179884,
    FireBreathingPurple         = 102911,
    FireBreathingGreen          = 102910,

    ///L70ETC
    EarthQuakeVisual            = 42499,
    LightningCloud              = 42500,
    RainOfFire                  = 42501,
    CallLightning               = 42510,
    SummonEffect                = 42505
};

enum eCreature
{
    /// Mini game questgiver
    Mola                        = 54601,
    Finlay                      = 54605,
    JessRog                     = 54485,
    KaeTi                       = 68402,

    /// Whack a gnoll
    GnollBarrel                 = 54546,
    BasicGnoll                  = 54444,
    BossGnoll                   = 54549,
    BabyGnoll                   = 54466,

    /// Shot mini game
    BunnyTarget1                = 24171,
    BunnyControllerShot         = 3500023,

    /// Toss
    Dubenko                     = 54490,

    /// Arena
    Korgol                      = 55402,

    /// Race
    BigRaceBunny                = 3500042,
    ShortRaceBunny              = 3500040,
    BunnyCheckpoint             = 75718,
    PowerUpLoot                 = 74872,

    /// Rabbit
    DarkmoonRabbit              = 58336,

    /// Moonfang
    MoonfangSnarler             = 56160,
    MoonfangDreadhowl           = 71982,
    MoonfangMother              = 71992,
    BrendonPaulson              = 56050,

    /// L70ETC
    ControllerConcert           = 23830,
    MichaelSchweitzer           = 23988,
    MaiKyl                      = 23624,
    SigNicious                  = 23626,
    Samuro                      = 23625,
    Bergrisst                   = 23619,
    ChiefThunderSkins           = 23623,
    MaiKylCtrlr                 = 23852,
    SigNiciousCtrlr             = 23854,
    SamuroCtrlr                 = 23853,
    BergrisstCtrlr              = 23845,
    ChiefThunderSkinsCtrlr      = 23855,
    AllianceCitizen             = 55347,
    HordeCitizen                = 55348
};

enum eTalk
{
    MonitorWarn30               = 0,
    MonitorWarn15               = 1,
    MonitorWarn5                = 2,
    MonitorBegin                = 3
};

enum eGameObject
{
    DarkmoonChest               = 209620,
};

enum eTrinityString
{
    OutsideGamingArea           = 200000,
    OutsideCarouselArea         = 200001
};

enum eAchievementCriteria
{
    DalaranCrit                 = 27718,
    OrgrimmarCrit               = 27719,
    ShattrathCrit               = 27720,
    SilvermoonCrit              = 27721,
    ThunderbluffCrit            = 27722,
    UndercityCrit               = 27723,
    DarnassusCrit               = 27724,
    ExodarCrit                  = 27725,
    IronforgeCrit               = 27726,
    StormwindCrit               = 27727

};

enum eAchievement
{
    DarkmoonDungeoneer          = 6027,
    DarkmoonDefender            = 6028,
    DarkmoonDespoiler           = 6029,
    QuickShot                   = 6022,
    BlastenheimmerBullseye      = 6021
};

enum eItems
{
    SoothsayersRunes            = 71716,
    GrislyTrophy                = 71096,
    DarkmoonFairGuide           = 71634,
    FallenAdventurerJournal     = 71953,
    MonstrousEgg                = 71636,
    OrnateWeapon                = 71638,
    ATreatiseOnStrategy         = 71715,
    ImbuedCrystal               = 71635,
    CapturedInsigna             = 71952,
    BannerOfTheFallen           = 71951
};

enum eQuests
{
    ShortRaceQuestOne           = 37819,
    ShortRaceQuestTwo           = 33756,
    ShortRaceQuestThree         = 37910,
    LongRaceQuestOne            = 37868,
    LongRaceQuestTwo            = 37911,
    TestYourStrengh             = 29433,
    ACapturedBanner             = 29456,
    TheCapturedJournal          = 29458,
    AnExoticEgg                 = 29444,
    AWondrousWeapon             = 29446,
    ACuriousCrystal             = 29443,
    TheMasterStrategist         = 29451,
    TheEnemyInsigna             = 29457,
    ToolsOfDivination           = 29464
};

enum eArea
{
    DarkmoonDeathmatch          = 5877,
    DarkmoonFaire               = 5870
};

enum eZoneId
{
    DalaranLegion               = 7202,
    DalaranTLK                  = 4395,
    Darnassus                   = 1657,
    Exodar                      = 3557,
    Ironforge                   = 1537,
    Shattrath                   = 3703,
    Stormwind                   = 1519,
    Silvermoon                  = 3487,
    Orgrimmar                   = 1637,
    ThunderBluff                = 1638,
    Undercity                   = 1497
};


uint32 RewardPVPItemID[3] =
{
    71953,
    71951,
    71952
};

uint32 RewardPVEItemID[5] =
{
    71715,
    71638,
    71637,
    71636,
    71635
};

uint32 RaceQuestId[5]
{
    37819,
    33756,
    37910,
    37868,
    37911
};

uint32 DeckOfNemelexToySpell[10]
{
    170963,
    170964,
    170965,
    170968,
    170970,
    170966,
    170967,
    170969,
    170971,
    170972
};


uint32 FireworksDummyId[2][7]
{
    {
        128313,
        128314,
        128315,
        128316,
        128317,
        128318,
        128319
    },

    {
        129679,
        129680,
        129681,
        129682,
        129683,
        129684,
        129685
    }
};

uint32 AllianceModels[15] =
{
    2225,
    17479,
    2220,
    1437,
    22699,
    18231,
    25521,
    11680,
    4888,
    1434,
    3112,
    1482,
    18137,
    3068,
    17241
};

uint32 HordeModels[12] =
{
    1879,
    11756,
    5346,
    18212,
    16758,
    4085,
    3606,
    2667,
    2025,
    4540,
    16814,
    27260
};
