////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef ANTORUS_THE_BURNING_THRONE_HPP
# define ANTORUS_THE_BURNING_THRONE_HPP

# include "AreaTrigger.h"
# include "ScriptedCreature.h"
# include "ScriptMgr.h"
# include "SpellAuras.h"
# include "SpellScript.h"
# include "InstanceScript.h"
# include "CreatureTextMgr.h"
# include "CosmeticEvents.hpp"
# include "Timer.h"

Position const g_WestImonarPos = { -10575.132f, 8723.624f, 1871.564f, 4.75f };
Position const g_EastImonarPos = { -10575.132f, 8536.923f, 1871.564f, 4.75f };

enum eDungeonIDs
{
    DungeonIDNone               = 0,
    DungeonIDLightsBreach       = 1610,
    DungeonIDForbiddenDescent   = 1611,
    DungeonIDHopesEnd           = 1612,
    DungeonIDSeatofthePantheon  = 1613
};

enum eData
{
    /// Bosses
    DataGarothiWorldbreaker,
    DataFelhoundsOfSargeras,
    DataAntoranHighCommand,
    DataPortalKeeperHasabel,
    DataEonarTheLifeBinder,
    DataImonarTheSoulhunter,
    DataKinGaroth,
    DataVarimathras,
    DataTheCovenOfShivarra,
    DataAggramar,
    DataArgusTheUnmaker,
    DataMaxBosses,

    /// Getters/Setters
    DataFelBatDown,
    DataAnnihilatorDeath,
    DataMoveToWorldbreaker,
    DataPositionWorldbreakerPlatform,
    DataActivateLegionShips,
    DataDestroyKingarothDoor,
    DataFirstWingClearedIntro,
    DataKinGarothIntro,
    DataTarneth,
    DataVarimathrasIntro,
    DataBrokenCliffsEnabled,
    DataClobex,
    DataImonarSkipQuest,
    DataAggramarSkipQuest,
    DataDungeonID,

    /// Misc data
    DataHasabelMaxFelcrushPortals   = 6,
    DataArgusHungeringSouls         = 25,
    DataArgusMotesToCollect         = 20,
    DataArgusMaxSargerasGazes       = 8,
    DataImonarEncounterID           = 2082,
    DataArgusEncounterID            = 2092,
    DataPantheonsBuffNeeded         = 4
};

enum eCreatures
{
    /// To First Boss
    NpcImageOfProphetVelen          = 125513,
    NpcDreadwing                    = 125487,
    NpcGarothiDecimator             = 123402,
    NpcHighExarchTuralyon           = 125512,
    NpcLightforgedCenturion         = 125478,
    NpcLightforgedCenturion2        = 129617,
    NpcLightforgedCleric            = 124778,
    NpcLightforgedCommander         = 125480,
    NpcLightforgedWarframe          = 125476,
    NpcLightforgedBarricade         = 125147,
    NpcLightforgedWarframe2         = 127256,
    NpcGarothiAnnihilator           = 123398,
    NpcDestroyedShip                = 126409,

    /// Garothi Worldbreaker
    BossGarothiWorldbreaker         = 122450,
    NpcDecimator                    = 122773,
    NpcAnnihilator                  = 122778,
    NpcAnnihilation                 = 122818,
    NpcFelSurge                     = 124167,

    /// First wing room
    NpcLightforgedBeacon            = 130137,
    NpcLightforgedWarframe3         = 124785,
    NpcLightforgedWarframeToImonar  = 127963,
    NpcLightforgedCenturion3        = 124777,
    NpcLightforgedCommander2        = 124782,
    NpcArchmageKhadgar              = 125691,
    NpcIllidanStormrage             = 125683,
    NpcGrandArtificerRomuul         = 129876,
    NpcLightforgedCenturionDead     = 128589,
    NpcBlightscaleWorm              = 124436,
    NpcLegionTalon                  = 125771,
    NpcLightforgedBarricade2        = 126584,
    NpcClubfistBeastlord            = 126767,
    NpcSlobberingFiendGround        = 126776,
    NpcLigforgedTeleportPod         = 128289,
    NpcHulkingDemolisher            = 130192,
    NpcBladeswornRavager            = 126764,
    NpcFlameweaver                  = 127223,
    NpcPortalAntoranFelguard        = 126558,
    NpcPortalAntoranChampion        = 127045,
    NpcFelPortalLeft                = 127050,
    NpcFelPortalRight               = 126527,
    NpcClobex                       = 127732,
    NpcMagniBronzebeard2            = 124913,
    NpcProphetVelen                 = 125682,

    /// Felhounds of Sargeras
    BossShatug                      = 122135,
    BossFharg                       = 122477,

    /// Antoran High Command
    BossAdmiralSvirax               = 122367,
    BossChiefEngineerIshkar         = 122369,
    BossGeneralErodus               = 122333,
    NpcAdmiralsPod                  = 122554,
    NpcEngineersPod                 = 123020,
    NpcGeneralsPod                  = 123013,
    NpcPassiveAdmiralSvirax         = 130184,
    NpcPassiveChiefEngineerIshkar   = 130185,
    NpcPassiveGeneralErodus         = 130186,
    NpcHologramStalker              = 127741,
    NpcEntropicMine                 = 122992,
    NpcFelbladeShocktrooper         = 122718,
    NpcFanaticalPyromancer          = 122890,
    NpcLegionCruiser                = 122739,
    NpcScoutShip                    = 122944,
    NpcFelshieldEmitter             = 122867,
    NpcScreamingShrike              = 128069,

    /// Second wing trashes
    NpcMagniAntoranFelguard         = 125590,
    NpcMagniBronzebeard             = 125584,
    NpcImageOfEonar                 = 128352,

    /// Portal Keeper Hasabel
    NpcDevastatorStalker            = 125603,
    BossPortalKeeperHasabel         = 122104,
    NpcBlazingImp                   = 122783,
    NpcHungeringStalker             = 123223,
    NpcFeltouchedSkitterer          = 123702,
    NpcGatewayToXoroth              = 122494,
    NpcGatewayFromXoroth            = 122533,
    NpcGatewayToRancora             = 122543,
    NpcGatewayFromRancora           = 122555,
    NpcGatewayToNathreza            = 122558,
    NpcGatewayFromNathreza          = 122559,
    NpcFelcrushPortal               = 122438,
    NpcVulcanar                     = 122211,
    NpcLadyDacidion                 = 122212,
    NpcLordEilgar                   = 122213,
    NpcEverburningFlames            = 122733,
    NpcEverburningFlamesFight       = 123003,
    NpcEternalDarkness              = 122966,
    NpcPortalXoroth                 = 122586,
    NpcPortalRancora                = 122587,
    NpcPortalNathreza               = 122588,
    NpcFlamesOfXoroth               = 122628,
    NpcAcidOfRancora                = 122647,
    NpcShadowsOfNathreza            = 122953,
    NpcFelsilkWrap                  = 122897,
    NpcChaoticRift                  = 124270,

    /// Eonar the Lifebinder
    BossEssenceOfEonar              = 122500,
    NpcJumpPad                      = 124962,
    NpcJumpDest                     = 124963,
    NpcTheParaxis                   = 124445,
    NpcTheParaxisDummy              = 125364,
    NpcFelguard                     = 123451,
    NpcFelInfusedDestructor         = 123760,
    NpcFelHound                     = 123191,
    NpcFelLord                      = 123452,
    NpcFelChargedObfuscator         = 124207,
    NpcVolantKerapteron             = 124227,
    NpcFelPoweredPurifier           = 123726,
    NpcOrbOfLife                    = 128088,
    NpcParaxisInquisitor            = 125429,
    NpcParaxisFocusingCrystal1      = 125917,
    NpcParaxisFocusingCrystal2      = 125918,
    NpcParaxisFocusingCrystal3      = 125919,
    NpcParaxisFocusingCrystal4      = 125920,
    NpcFocus                        = 125930,

    /// Imonar the Soulhunter
    BossImonarTheSoulhunter         = 124158,
    NpcImonarShip                   = 125692,
    NpcImonarsGarothi               = 128154,
    NpcBombingRun                   = 124704,
    NpcTraps                        = 124686,
    NpcArtilleryStrike              = 124889,
    NpcVindicaarCanon               = 126678,
    NpcConflagration                = 124550,

    NpcTeleportPodBrokenCliffs      = 125720,
    NpcTeleportPodTheExhaust        = 128303,
    NpcTeleportPodBurningThrone     = 128304,

    /// Kin'garoth
    NpcKingaroth                    = 122578,

    ///< Pre Varimathras
    NpcTarneth                      = 123533,
    /// Varimathras
    NpcVarimathras                  = 122366,
    NpcShadowofVarimathras          = 122590,
    NpcShadowofVarimathras2         = 122643,

    /// The Coven of Shivarra
    NpcAsaraMotherOfNight           = 122467,
    NpcNouraMotherOfFlames          = 122468,
    NpcDiimaMotherofGloom           = 122469,
    NpcThurayaMotherOfTheCosmos     = 125436,

    /// Aggramar
    BossAggramar                    = 121975,
    NpcEmberOfTaeshalach            = 122532,
    NpcFlameOfTaeshalach            = 121985,
    NpcManifestationOfTaeshalach    = 123531,

    /// Argus the Unmaker
    BossArgusTheUnmaker             = 124828,
    NpcBeaconFromPantheon           = 128304,
    NpcAmanThul                     = 125885,
    NpcGolganneth                   = 126268,
    NpcKhazgoroth                   = 125886,
    NpcEonar                        = 126267,
    NpcAggramar                     = 125893,
    NpcNorgannon                    = 126266,
    NpcConstellarDesignate          = 127192,
    NpcGiftOfTheLifebinder          = 129386,
    NpcReoriginationModule          = 127809,
    NpcWitheredGiftOfTheLifebinder  = 130239,
    NpcHungeringSoul                = 129635,
    NpcMoteOfTitanicPower           = 129722,
    NpcEdgeOfObliteration           = 126828,
    NpcApocalypsisModule            = 130179,
    NpcChainsOfSargeras             = 130202,
    NpcEdgeOfAnnihilation           = 130842
};

enum eGameObjects
{
    /// Garothi Worldbreaker
    GarothiRocks                    = 278488,
    GarothiRocksCollision           = 277365,

    /// Antoran High Command
    GoAntoranHighCommandElevator    = 278815,
    GoAntoranHighCommandDoor        = 277660,

    /// Portal Keeper Hasabel
    GoHasabelEncounterDoor          = 277521,

    /// Eonar the Lifebinder
    GoEonarEncounterDoor1           = 273688,
    GoEonarEncounterDoor2           = 273687,
    GoParaxisShip                   = 272683,
    GoBlessingOfLife                = 276503,
    GoParaxisDoor1                  = 272759,
    GoParaxisDoor2                  = 273016,
    GoParaxisDoor3                  = 273017,

    /// Imonar the Soulhunter
    GoKingarothAccessDoor           = 273911,
    GoKingarothAccessDoorColl       = 249386,

    /// Kin'garoth
    GoKingarothDoor                 = 277531,

    /// Pre Varimathras
    GoPreVarimathrasDoor            = 272485,

    /// Varimathras
    GoVarimathrasDoor1              = 277179,
    GoVarimathrasDoor2              = 277180,

    ///< Coven of Shivarra
    GoCovenShivarraDoor1            = 272868,
    GoCovenShivarraDoor2            = 277476,

    ///< Pre Agrammar
    GoPreAgrammarDoor               = 272486,

    ///< Aggramar
    GoAggramarDoor                  = 273686,
    GoAggramarBridge                = 277477,

    /// Argus the Unmaker
    GoSpoilsOfThePantheon           = 277355
};

enum eSharedSpells
{
    SpellAntorusNerf                    = 999051,
    SpellBerserk                        = 224264,
    SpellVindicaarDestroysDoor          = 251475,
    SpellDestroyDoor                    = 251391,
    SpellSurgeOfLifeSecond              = 245786,
    SpellJumpOnPad                      = 248225,
    SpellWing1ClearedIntro              = 249674,
    SpellWing1ClearedOutro              = 249441,
    SpellArgusOutroTeleport             = 255235,
    SpellArgusLeavingMovie              = 257606,
    SpellVioletSpellwing                = 253639,
    SpellShackledUrzul                  = 243651,

    /// Pantheon spells
    SpellCelestialBulwark               = 256816,
    SpellGlimpseOfEnlightenment         = 256818,
    SpellMarkOfGolganneth               = 256821,
    SpellMarkOfEonar                    = 256824,
    SpellWorldforgersFlame              = 256826,
    SpellRushOfKnowledge                = 256828,
    /// Pantheon procs
    SpellAggramarsFortitude             = 256831,
    SpellAmanThulsGrandeur              = 256832,
    SpellGolgannethsThunderousWrath     = 256833,
    SpellKhazgorothsShaping             = 256835,
    SpellNorgannonsCommand              = 256836,
    SpellEonarsVerdentEmbraceDruid      = 257470,
    SpellEonarsVerdentEmbraceMonk       = 257471,
    SpellEonarsVerdentEmbracePaladin    = 257472,
    SpellEonarsVerdentEmbraceDiscPriest = 257473,
    SpellEonarsVerdentEmbraceHolyPriest = 257474,
    SpellEonarsVerdentEmbraceShaman     = 257475
};

enum ePantheonItems
{
    ItemAmanThulsVision     = 154172,

    ItemAggramarsConviction = 154173,
    ItemGolgannethsVitality = 154174,
    ItemEonarsCompassion    = 154175,
    ItemKhazgorothsCourage  = 154176,
    ItemNorgannonsProwess   = 154177,

    ItemMaxTrinkets         = 5,

    ItemBloodOfTheUnmaker   = 152900,
    ItemPantheonsBlessing   = 155831,
    ItemShackledUrzul       = 152789,

    ItemArgusScytheBlue     = 153115,
    ItemArgusScytheRed      = 155880
};

enum eAchievements
{
    PortalCombat        = 11928,
    HardToKill          = 11949,
    StartdustCrusaders  = 12257,
    ArgusTheUnmakerNm   = 11984,
    ArgusTheUnmakerHm   = 11985,
    ArgusTheUnmakerMm   = 11986
};

enum eEncounterIDs
{
    EncounterDefenseOfEonar = 2075,
    EncounterAgrammar       = 2063
};

enum eLocations
{
    LocationRaidEntrance    = 6161,
    LocationLFRWing2        = 6165,
    LocationLFRWing3        = 6166,
    LocationWarCouncil      = 6288,
    LocationEonarLifebinder = 6290,
    LocationAntorusCore     = 6291,
    LocationAggramar        = 6292,
    LocationPantheon        = 6293,
    LocationImonarHunter    = 6295
};

enum eSkipQuests
{
    SkipQuestImonarNormal   = 49032,
    SkipQuestImonarHeroic   = 49075,
    SkipQuestImonarMythic   = 49076,

    SkipQuestAggramarNormal = 49133,
    SkipQuestAggramarHeroic = 49134,
    SkipQuestAggramarMythic = 49135
};

enum eTeleportPods
{
    FelhoundsToHighCommand = 0,
    HighCommandToFelhounds = 1
};

static std::vector<std::vector<std::vector<G3D::Vector3>>> const k_GatewayPaths =
{
    {   ///< Felhounds to High Command
        {
            {-3169.615f, 10361.83f, -109.2928f},
            {-3129.943f, 10424.14f, -101.9251f},
            {-3091.889f, 10485.77f, -107.9811f},
            {-3086.741f, 10503.14f, -112.0616f}
        },
        {
            {-3174.753f, 10364.67f, -105.2651f},
            {-3117.571f, 10396.54f, -89.31614f},
            {-3094.116f, 10464.27f, -97.37531f},
            {-3088.188f, 10484.22f, -112.5638f}
        },
        {
            {-3176.981f, 10359.40f, -98.26801f},
            {-3141.120f, 10413.99f, -85.40022f},
            {-3124.043f, 10459.21f, -99.13285f},
            {-3095.073f, 10479.23f, -112.6026f}
        },
        {
            {-3183.911f, 10368.42f, -101.0699f},
            {-3134.205f, 10405.88f, -122.3804f},
            {-3107.030f, 10451.01f, -108.3425f},
            {-3091.734f, 10482.33f, -112.9331f},
            {-3085.457f, 10495.10f, -113.3684f}
        }
    },
    {   ///< High Command to Felhounds
        {
            {-3095.323f, 10468.55f, -112.3548f},
            {-3120.825f, 10390.60f, -96.64750f},
            {-3154.017f, 10369.79f, -96.19229f},
            {-3181.552f, 10343.39f, -102.4307f}
        },
        {
            {-3104.691f, 10465.97f, -110.3998f},
            {-3138.474f, 10392.01f, -93.59305f},
            {-3182.991f, 10358.53f, -104.1474f},
            {-3200.042f, 10320.50f, -109.8413f}
        }
    }
};

#endif ///< ANTORUS_THE_BURNING_THRONE_HPP
