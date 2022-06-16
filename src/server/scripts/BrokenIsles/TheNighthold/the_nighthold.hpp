////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef THE_NIGHTHOLD_HPP
# define THE_NIGHTHOLD_HPP

# include "ScriptMgr.h"
# include "ScriptedCreature.h"
# include "GridNotifiersImpl.h"
# include "SpellScript.h"
# include "InstanceScript.h"
# include "Group.h"
# include "ScriptedGossip.h"
# include "Packets/SpellPackets.h"
# include "Vehicle.h"
# include "MoveSplineInit.h"
# include "CreatureTextMgr.h"
# include "LegionCombatAI.hpp"
# include "HelperDefines.h"

enum eData
{
    DataSkorpyron,
    DataChronomaticAnomaly,
    DataTrilliax,
    DataSpellbladeAluriel,
    DataStarAugurEtraeus,
    DataHighBotanistTelarn,
    DataKrosus,
    DataTichondrius,
    DataElisande,
    DataGuldan,

    MaxEncounters,

    DataSkorpyronPath,
    DataAnomalySpeed,
    DataAnomalyOverwhelming,
    DataTrilliaxIntro,
    DataTrilliaxImprintDoor,
    DataFirstTrashesKilled,
    DataSkorpyronIntro,
    DataAnomalyIntro,
    DataSkorpyronAchiev,
    DataAnomalyAchiev,
    DataSecondTrashesKilled,
    DataPreAlurielTrash,
    DataAstrologerJarin,
    DataTrilliaxAchiev,
    DataHasQuickAccess,

    DataKrosusEncounterID = 1842,
    DataGuldanEncounterID = 1866
};

enum eCreatures
{
    /// General
    NpcThalyssra                = 110791,
    NpcKirinTorGuardian         = 113608,
    NpcKirinTorGuardian2        = 113605,
    NpcKhadgar                  = 110792,
    NpcDeadCrystallineScorpid   = 113367,
    NpcFallenDefender           = 113383,
    NpcPulsauronFighter         = 111071,
    NpcPulsauronShielder        = 111072,
    NpcSLGGenericStalker        = 92879,
    NpcFulminant                = 111081,
    NpcChaotoid                 = 111075,
    NpcSLGGenericMoPLargeAoI    = 68553,

    /// Trashes
    NpcTrashVolatileScorpid     = 108359,
    NpcTrashAcidmawScorpid      = 108360,
    NpcDuskwatchSentinel        = 112675,
    NpcDuskwatchBattleMagus     = 112671,
    NpcShamblingHungerer        = 113321,
    NpcWitheredSkulker          = 113128,
    NpcChronowraith             = 113307,
    NpcPromenadeGuard           = 112709,

    /// Skorpyron
    NpcSkorpyron                = 102263,
    NpcCrystallineScorpid       = 103217,
    NpcVolatileScorpid          = 103224,
    NpcAcidmawScorpid           = 103225,
    NpcCrystallineShard         = 103209,
    NpcVolatileShard            = 108132,
    NpcAcidicShard              = 108131,
    NpcArcaneTether             = 103682,
    NpcCrystalSpine1            = 101339,
    NpcCrystalSpine2            = 101340,
    NpcCrystalSpine3            = 101342,
    NpcCrystalTail              = 101335,
    NpcCrystalMandible1         = 101337,
    NpcCrystalMandible2         = 101338,

    /// Chromatic Anomaly
    NpcChromaticAnomaly         = 104415,
    NpcFragmentedTime           = 114671,
    NpcSuramarTrigger           = 108786,
    NpcTheNightwell             = 104738,
    NpcWaningTimeParticle       = 104676,
    NpcTemporalRift             = 106878,

    /// Trilliax
    NpcTrilliax                 = 104288,
    NpcTrilliaxCopyCleaner      = 108303, ///< Mythic
    NpcTrilliaxCopyManiac       = 108144, ///< Mythic
    NpcSludgerax                = 112255,
    NpcPutridSludge             = 112251,
    NpcSucculentFeast           = 104561,
    NpcToxicSlice               = 104547,
    NpcScrubber                 = 104596,
    NpcSlimePool                = 112293,

    /// Spellblade Aluriel
    BossSpellbladeAluriel       = 104881,
    NpcSpellbladeAlurielImage   = 107980,
    NpcMarkOfFrost              = 107694,
    NpcIcyEnchantment           = 107237,
    NpcSearingBrand             = 107584,
    NpcFieryEnchantment         = 107285,
    NpcArcaneOrb                = 107510,
    NpcArcaneEnchantment        = 107287,
    NpcIceShards                = 107592,
    NpcFrostVisual1             = 108545,
    NpcFrostVisual2             = 108546,
    NpcFrostVisual3             = 108547,
    NpcFrostVisual4             = 108548,
    NpcFelSoul                  = 115905,

    /// Star Augur Etraeus
    BossStarAugurEtraeus        = 103758,
    NpcCoronalEjection          = 103790,
    NpcIceCrystal               = 109003,
    NpcThingThatNotShouldBe     = 104880,
    NpcStarShootingStalker      = 112767,
    NpcVoidling                 = 104688,
    NpcArcWell                  = 112664,
    NpcEyeOfTheVoid             = 109088,
    NpcRemmantOfTheVoid         = 109151,

    /// High Botanist Tel'arn
    BossHighBotanistTelarn      = 104528,
    NpcSolaristTelarn           = 109038,
    NpcArcanistTelarn           = 109040,
    NpcNaturalistTelarn         = 109041,
    NpcSolarCollapseStalker     = 109583,
    NpcPlasmaSphere             = 109804,
    NpcChaosSphere              = 111993,
    NpcChaoticSphere            = 112092,
    NpcParasiticLasher          = 109075,
    NpcToxicSpore               = 110125,
    NpcControlledChaosStalker   = 109792,

    /// Krosus
    Krosus                      = 101002,
    BurningEmber                = 104262,
    NpcFelweaverPharamere       = 111227,
    NpcSummonerXiv              = 111226,
    NpcChaosMageBeleron         = 111225,
    NpcWorldTrigger             = 83816,

    /// Tichondrius
    Tichondrius                 = 103685,
    TaintedBlood                = 108934,
    FelSpire                    = 108625,
    FelSpire2                   = 108677,
    NpcCarrionNightmare         = 108739,
    NpcPhantasmaticBloodfang    = 104326,
    FelswornSpellguard          = 108591,
    SightlessWatcher            = 108593,

    /// Elisande
    Elisande                    = 106643,
    ElisandeEcho                = 106682,
    RecursiveElemental          = 105299,
    ExpedientElemental          = 105301,
    NpcArcaneticRing            = 105315,
    NpcEpochericOrb             = 107754,
    NpcArcaneticRingTrigger     = 106330,
    TheNightwell                = 115683,
    NpcMidnightSiphoner         = 111151,
    NpcTwilightStardancer       = 111164,
    NpcResoluteCourtesan        = 111166,
    NpcAstralFarseer            = 111170,
    NpcInfiniteDrakeling        = 108802,

    /// Gul'Dan
    NpcGulDan                   = 104154,
    NpcEyeOfAmanThulTrigger     = 114440,
    NpcEyeOfAmanThulMythic      = 114438,
    NpcLiquidHellfire           = 104396,
    NpcFelLordKurazmal          = 104537,
    NpcFelObelisk               = 104569,
    NpcInquisitorVethriz        = 104536,
    NpcGazeOfVethriz            = 106986,
    NpcDzorykxTheTrapper        = 104534,
    NpcBondsOfFel               = 104252,
    NpcEyeOfGuldan              = 105630,
    NpcFogCloud                 = 116233,
    NpcEmpoweredEyeOfGuldan     = 106545,
    NpcNightwellEntity          = 104214,
    NpcWellOfSoulsMain          = 104526,
    NpcWellOfSouls1             = 104594,
    NpcWellOfSouls2             = 105371,
    NpcDarkSoul                 = 110688,
    NpcAzagrim                  = 105295,
    NpcBeltheris                = 107232,
    NpcDalvengyr                = 107233,
    NpcTheDemonWithin           = 111022,
    NpcParasiticShadowDemon     = 111047,
    NpcSeveredSoul              = 107229,
    NpcVisionsDummy             = 113969,
    NpcVisionsFinal             = 113972,
    NpcDemonicEssence           = 111222,

    /// Cosmetic NPCs
    NpcIllidanStormragePrison   = 114437,
    NpcVictoire                 = 116256,
    NpcTyrandeWhisperwind       = 114838,
    NpcSilgryn                  = 115499,
    NpcArluelle                 = 116372,
    NpcKal                      = 115772,
    NpcArcanistValtrois         = 115840,
    NpcLadyLiadrin              = 114841,
    NpcGrandMagisterRommath     = 114883,
    NpcIllidanStormrage         = 116146,
    NpcArchmageKhadgar          = 106522,
    NpcLightsHeart              = 116153,
    NpcLightsHeart2             = 116156
};

enum eGameObjects
{
    GoSkorpyronDoor1            = 252105,
    GoSkorpyronDoor2            = 252103,
    GoSkorpyronTeleporterDoor   = 254240,
    GoSkorpyronLOSBlocker       = 247065,
    GoAnomalyDoor1              = 252349,
    GoAnomalyDoor2              = 252348,
    GoTrilliaxDoor1             = 250243,
    GoTrilliaxDoor2             = 251397,
    GoTrilliaxDoor3             = 250241,
    GoTrilliaxDoor4             = 250242,

    GoAnomalyPre                = 251519,

    GoArcingDepthsExit          = 254241,

    /// High Botanist Tel'arn
    GoBotanistDoorRight         = 251832,
    GoBotanistDoorLeft          = 251833,
    GoMysteriousFruit           = 252108,

    GoAstromancerDome           = 251401,
    GoAstromancerDoor           = 248932,

    GoKrosusChest               = 248513,
    GoKrosusBridge1             = 247970,
    GoKrosusBridge2             = 247973,
    GoKrosusBridge3             = 247971,
    GoKrosusBridge4             = 247972,
    GoKrosusTeleporterDoor      = 258847,

    FelSpireGob                 = 251259,

    GoElisandeDoorNW            = 251612,
    GoElisandeDoorNE            = 252316,
    GoElisandeDoorSW            = 252315,
    GoElisandeNightwell         = 251330,

    /// Gul'Dan
    GoGiftOfTheNightborne       = 266483,
    GoGulDanRoom                = 253462,
    GoGulDanCenterPlatform      = 252318,
    GoEyeOfAmanthulCollisionBox = 250082,
    GoEyeOfAmanthulFloorSphere  = 266174,
    GoEyeOfAmanthulFloorRune    = 260985,

    GoFocusingStatueNE          = 251988,
    GoFocusingStatueNW          = 252319,
    GoFocusingStatueSE          = 252320,
    GoFocusingStatueSW          = 252321,

    GoEnergyConduitSW           = 266180,
    GoEnergyConduitNW           = 266181,
    GoEnergyConduitSE           = 266182,
    GoEnergyConduitNE           = 266183,

    GoEyeOfAmanthulWorldS       = 266395,

    GoAstromancerTowerDoor      = 254243,

    GoNightholdSpireDoorFront   = 251981,
    GoNightholdSpireDoorBack    = 251982
};

enum eActions ///< Actions move for Talysra
{
    ActionThalyssraRaidIntro,
    ActionMoveFirstBoss,
    ActionMoveAfterFirstBoss,
    ActionFirstMoveToAnomaly,
    ActionSecondMoveToAnomaly,
    ActionAlurielOutro,
    TichondriusStartIllusionaryNight
};

enum eSharedSpells
{
    SpellTalysraIntroConversation   = 231471,
    SpellTalysraOutroConversation   = 231486,
    SpellOpenDoor                   = 230980,
    SpellSuramarPortal05            = 225665,
    SpellSuramarPortal05_2          = 225667,
    SpellSuramarPortal06            = 225673,
    SpellSuramarPortal06_2          = 225675,
    SpellFakeDeath                  = 29266,
    SpellArcaneExposure             = 220114,
    SpellBerserk                    = 47008,
    SpellBerserkTrilliax            = 26662,
    SpellBerserkGuldan              = 64238,
    SpellDoorLock                   = 232575
};

enum eLocations
{
    LocEntrance                 = 5338,
    LocScorpionRoom             = 5807,
    LocAnomalysRoom             = 5808,
    LocCatacombStairs           = 5739,
    LocCatacombStarisUp         = 5535,
    LocAlurielPreStairs         = 5809,
    LocCaptainQuartersEntrance  = 5536,
    LocTichondriusRoom          = 5656,
    LocGuldanRoom               = 5537,
    LocLFRWing2Entrance         = 5535
};

enum eAchievementsData
{
    /// Skorpyron
    CageRematchWorldStateID = 11880,
    CageRematchAchievID     = 10678,
    /// Chronomatic Anomaly
    GrandOpening            = 10697,
    GrandOpeningCount       = 12,
    /// Trilliax
    GlutenFree              = 10742,
    GlutenFreeMaxCount      = 20,
    /// Spellblade Aluriel
    AChangeInScenery        = 10817,
    /// High Botanist Tel'arn
    FruitOfAllEvil          = 10754,
    BotanistMythic          = 10846,
    /// Gul'dan
    IveGotMyEyesOnYou       = 10696
};

enum eNHQuests
{
    QuickAccessNormal = 45381,
    QuickAccessHeroic = 45382,
    QuickAccessMythic = 45383
};

enum eSharedPoints
{
    PointAlurielDead    = 10
};

enum eSharedTalks
{
    TalkAlurielOutroStart   = 8,
    TalkAlurielOutroEnd     = 9,
    TalkKhadgarAlurielEnd   = 10,
    TalkAlurielPromenadeGuardsDead = 12,
};

enum eDungeonIDs
{
    DungeonArcingAqueducts  = 1290,
    DungeonRoyalAthaneum    = 1291,
    DungeonNightspire       = 1292,
    DungeonBetrayersRise    = 1293
};

static constexpr uint32 g_MaxTrashDiedDepthsSecondWing = 13;

const std::array<Position, 2> g_CitizenBuildings =
{
    {
        { 577.020f, 3358.314f, 110.853f },
        { 505.981f, 3429.017f, 110.854f }
    }
};

static const Position g_AlurielStairsNpcPos =
{
    511.849f, 3364.590f, 115.213997f
};

static const Position g_KhadgarAlurielStairTeleportPos =
{
    465.510f, 3324.060f, 142.293f
};

static const Position g_EntrancePos = { -139.45f, 3528.51f, -253.88f, 0.0f };
static const Position g_ThalysrraSecondPos = { 408.010010f, 3345.179932f, 59.972599f };

struct RangedFilter
{
    bool operator() (HostileReference* p_It) const
    {
        if (p_It == nullptr || p_It->getTarget() == nullptr)
            return false;

        return RangedFilter::operator()(p_It->getTarget()->ToPlayer());
    }

    bool operator() (Player* p_It) const
    {
        if (p_It == nullptr)
            return false;

        uint32 l_Spec = p_It->GetActiveSpecializationID();

        switch (p_It->getClass())
        {
            case CLASS_WARLOCK:
            case CLASS_MAGE:
            case CLASS_PRIEST:
            case CLASS_HUNTER:
                return true;

            case CLASS_SHAMAN:
                return l_Spec == SpecIndex::SPEC_SHAMAN_ELEMENTAL || l_Spec == SpecIndex::SPEC_SHAMAN_RESTORATION;

            case CLASS_DRUID:
                return l_Spec == SpecIndex::SPEC_DRUID_RESTORATION || l_Spec == SpecIndex::SPEC_DRUID_BALANCE;

            default: return false;
        }
    }
};

struct TankFilter
{
    bool operator() (Player*& p_It) const
    {
        if (p_It == nullptr)
            return false;

        switch (p_It->GetActiveSpecializationID())
        {
            case SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE:
            case SpecIndex::SPEC_DK_BLOOD:
            case SpecIndex::SPEC_DRUID_GUARDIAN:
            case SpecIndex::SPEC_PALADIN_PROTECTION:
            case SpecIndex::SPEC_MONK_BREWMASTER:
            case SpecIndex::SPEC_WARRIOR_PROTECTION:
                return true;

            default: return false;
        }
    }

    bool operator() (Player*&& p_It) const
    {
        if (p_It == nullptr)
            return false;

        switch (p_It->GetActiveSpecializationID())
        {
        case SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE:
        case SpecIndex::SPEC_DK_BLOOD:
        case SpecIndex::SPEC_DRUID_GUARDIAN:
        case SpecIndex::SPEC_PALADIN_PROTECTION:
        case SpecIndex::SPEC_MONK_BREWMASTER:
        case SpecIndex::SPEC_WARRIOR_PROTECTION:
            return true;

        default: return false;
        }
    }
};

struct FormDead
{
    FormDead(bool const p_FirstDead, bool const p_Dead, uint32 const p_Entry) : m_FirstDead(p_FirstDead), m_Dead(p_Dead), m_Entry(p_Entry)
    {}

    bool m_FirstDead;
    bool m_Dead;
    uint32 m_Entry;
};

static std::array<FormDead, 3> g_FormsDead =
{
    {
        { false, false, eCreatures::NpcNaturalistTelarn },
        { false, false, eCreatures::NpcArcanistTelarn },
        { false, false, eCreatures::NpcSolaristTelarn }
    }
};


#endif ///< THE_NIGHTHOLD_HPP
