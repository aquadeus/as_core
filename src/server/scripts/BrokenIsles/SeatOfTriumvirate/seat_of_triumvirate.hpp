////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

# ifndef SEAT_OF_THRIUNVIRATE_HPP
# define SEAT_OF_THRIUNVIRATE_HPP

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
# include "HelperDefines.h"
# include "Challenge.h"
# include "ScenarioMgr.h"

constexpr uint8 g_EncountersMax = 4;

enum eData
{
    DataZuraal,
    DataSaprish,
    DataNezhar,
    DataLura,

    DataBossIntro,
    DataFirstPortalSaprish,
    DataPortalBehindSaprish,
    DataMax,
};

enum eCreatures
{
    /// Zuraal
    BossZuraalTheAscended   = 122313,
    NpcDarkAberration       = 122482,
    NpcCoalescedVoid        = 122716,
    NpcUrjad                = 126283,

    /// Saprish
    BossSaprish             = 122316,
    NpcShadewing            = 125340,
    NpcDarkfang             = 122319,
    NpcSaprishDoorStalker   = 127008,
    NpcVoidPortalStalker    = 123767,
    NpcVoidTrap             = 124427,
    NpcRiftWardenSaprish    = 122571,
    NpcWaveRift             = 124266,
    NpcSaprishStaff         = 125131,
    NpcSealedVoidCache      = 126312,
    NpcFragementedVoidling  = 125981,

    /// Viceroy Nezhar
    BossViceroyNezhar       = 122056,
    NpcVoidPortalNezhar     = 125102,
    NpcShadowguardVoidTender= 125615,
    NpcCenterPoint          = 122832,
    NpcTentacleStalker      = 122826,

    /// L'ura
    BossLura                = 122314,
    NpcLuraDoorStalker      = 125104,
    NpcVoidPortalDummy      = 124734,
    NpcRemnantOfAnguish     = 123054,
    NpcRiftWarden           = 125860,
    NpcVoidPortalTrash      = 125102,
    NpcGreatRiftWarden      = 124745,
    NpcWaningVoid           = 123050,

    /// Allies
    NpcLocusWalkerZuraal    = 123744,
    NpcLocusWalkerSaprish   = 125840,
    NpcLocusWalkerNezhar    = 125099,
    NpcLocusWalkerLura      = 125872,
    NpcAlleriaZuraal        = 123743,
    NpcAlleriaSaprish       = 125836,
    NpcAlleriaNezhar        = 123187,
    NpcAlleriaLura          = 125871,
    NpcController           = 124276,

    /// Trashmob
    NpcShadowguardSubjugator= 124171,
    NpcVoidFlayer           = 124947,
    NpcFamishedBroken       = 122322,
    NpcVoidDischarge        = 122478,
    NpcSappedVoidLord       = 122398,
    NpcVoidBladeZedaat      = 127911,
    NpcLashingVoidling      = 125857,

    /// Entries for Mythic+
};

enum eWorldQuests
{
    VoidBladeZedaat = 48957,
};

enum eGameObjects
{
    GoCollisionWall         = 272950,
    /// Saprish
    GoSaprishEntranceDoor   = 273789,

    /// L'ura
    GoLuraEntranceDoor  = 272062,
    GoLuraCombatDoor    = 273661,
};

enum eSharedSpells
{
    /// Zuraal
    SpellVoidContainment    = 246922,
    
    /// Saprish
    SpellAmplifyVoidTear    = 245175,
    SpellAmplifyVoidTearAOE = 250185,
    SpellPortalUpperNether  = 246953,
    SpellPortalLowerNether  = 246954,
    SpellPortalLowerVoid    = 246955,
    SpellPortalUpperVoid    = 246956,
    SpellSaprishWaveOneConv = 246876,
    SpellSaprishWaveTwoConv = 246878,
    SpellSaprishWaveEndConv = 246879,
    SpellSaprishAppears     = 246880,
    SpellVoidPortalVisual   = 246258,
    SpellVoidPortalAT       = 248144,
    SpellSummonLocusWalker  = 249955,
    SpellSummonSaprishStaff = 248480,
    SpellWaveRift           = 246859,
    SpellGrowingPressure    = 250820,
    SpellCrushingDarkness   = 250821,
    
    /// L'ura
    SpellOpenLuraDoor       = 248461,
    SpellVoidSever          = 247878,
    SpellCheckUnboundRifts  = 247870,
    SpellDarkTorrent        = 250318,
    SpellAlleriaMovie       = 250035,
    SpellAlleriaVoidForm    = 250027,
};

enum eSharedTalks
{
    /// Zuraal Outro
    TalkZuraalOutro,
    TalkAlleriaZuraalOutro2,
    TalkDesactivateWall,

    /// Lura Intro
    TalkLuraIntro,
    TalkLuraIntro2,
};

enum ePortalsQuotes
{
    TalkLowerNetherGate,
    TalkLowerVoidBreach,
    TalkRuinedOverlook,
    TalkTempleFoundation,
    TalkUpperVoidBreach,
    TalkCrumblingRuins,
};

enum eSharedActions
{
    ActionCreatureEvent = 10,

    /// Saprish
    ActionSaprishIntro,
    ActionSaprishPortalOpen,
    ActionSaprishPortalClosed,
    
    /// L'ura
    ActionLuraIntro,
    ActionLuraOutro,
    ActionVoidPortal,
    ActionCallOfTheVoid,
};

enum eSharedPoints
{
    PointWallToSaprish  = 10,
    PointLuraDoor,
};

enum eSafeLocations
{
    LocEntrance = 6048,
    LocZuraal   = 6110,
    LocSaprishEv= 6114,
    LocSaprish  = 6306,
    LocNezhar   = 6115,
    LocExitIns  = 6116,
};

enum eAchievements
{
    HeroicSoT       = 12007,
    WelcomeTheVoid  = 12004,
    LetItAllOut     = 12005,
    DarkerSide      = 12009
};

enum eScenarioEvents
{
    LuraDeath       = 61068,
    ZuraalDeath     = 61071,
    SaprishDeath    = 61070,
    NezharDeath     = 61069,
    CompleteDungeon = 58793
};

static const Position g_AlleriaSaprishWallPos       = { 5693.950f, 10762.100f, 23.487f };
static const Position g_LocusWalkerLuuraStartPos    = { 6007.620f, 10252.799f, 15.071f };
static const Position g_AlleriaLuuraStartPos        = { 6008.140f, 10249.900f, 14.874f };
static const Position g_StartLuraTrashPos           = { 6013.057f, 10254.894f, 15.418f };
static const Position g_EndLuraTrashPos             = { 6071.843f, 10314.583f, 23.152f };

static const std::array<G3D::Vector3, 52> g_FirstWalkerPath =
{
    {
        { 5572.195f, 10783.44f, 19.79083f },
        { 5574.195f, 10783.44f, 19.79083f },
        { 5576.695f, 10782.69f, 19.79083f },
        { 5578.445f, 10783.44f, 20.04083f },
        { 5582.195f, 10783.69f, 20.29083f },
        { 5590.083f, 10781.87f, 20.19869f },

        { 5590.423f, 10781.77f, 20.64149f },
        { 5592.673f, 10781.52f, 21.14149f },
        { 5593.673f, 10781.27f, 21.64149f },
        { 5594.173f, 10781.27f, 21.89149f },
        { 5595.423f, 10781.02f, 22.14149f },
        { 5596.673f, 10780.77f, 22.39149f },
        { 5598.423f, 10780.52f, 23.14149f },
        { 5599.673f, 10780.27f, 23.64149f },
        { 5600.923f, 10780.27f, 23.89149f },
        { 5606.968f, 10778.49f, 23.60491f },

        { 5607.300f, 10778.53f, 23.72713f },
        { 5613.800f, 10776.78f, 23.47713f },
        { 5615.050f, 10776.53f, 23.47713f },
        { 5616.550f, 10776.03f, 23.47713f },
        { 5619.050f, 10775.53f, 23.47713f },
        { 5621.642f, 10774.45f, 23.11854f },

        { 5623.467f, 10774.10f, 23.10406f },
        { 5625.717f, 10773.60f, 23.10406f },
        { 5628.217f, 10773.35f, 23.35406f },
        { 5630.467f, 10772.60f, 23.10406f },
        { 5635.340f, 10771.35f, 23.00403f },

        { 5635.712f, 10771.46f, 23.26931f },
        { 5636.712f, 10770.96f, 23.26931f },
        { 5639.462f, 10770.46f, 23.26931f },
        { 5640.712f, 10769.96f, 23.26931f },
        { 5643.212f, 10769.46f, 21.76931f },
        { 5644.462f, 10768.96f, 21.26931f },
        { 5645.462f, 10768.71f, 21.01931f },
        { 5646.712f, 10768.46f, 20.51931f },
        { 5647.962f, 10768.21f, 20.51931f },
        { 5649.426f, 10767.46f, 20.02236f },
        { 5658.617f, 10765.31f, 20.00993f },

        { 5658.893f, 10765.21f, 20.34252f },
        { 5660.393f, 10764.96f, 20.84252f },
        { 5664.393f, 10763.96f, 21.84252f },
        { 5666.393f, 10763.46f, 23.09252f },
        { 5667.393f, 10763.21f, 23.84252f },
        { 5668.068f, 10762.84f, 23.66781f },

        { 5668.348f, 10762.75f, 23.77181f },
        { 5669.848f, 10762.25f, 23.77181f },
        { 5672.598f, 10761.00f, 23.77181f },
        { 5673.848f, 10760.25f, 23.77181f },
        { 5679.098f, 10757.50f, 23.52181f },
        { 5682.098f, 10755.75f, 23.52181f },
        { 5685.173f, 10753.71f, 23.40405f },
        { 5688.768f, 10749.90f, 23.40405f },
    }
};

static const std::array<G3D::Vector3, 47> g_FirstAlleriaPath = 
{
    {
        { 5571.906f,  10786.07f, 19.29397f },
        { 5574.748f,  10786.69f, 19.96227f },
        { 5576.748f,  10786.94f, 19.96227f },
        { 5579.248f,  10786.44f, 20.71227f },
        { 5583.498f,  10785.44f, 20.71227f },
        { 5591.248f,  10783.94f, 20.71227f },
        { 5593.498f,  10783.19f, 21.21227f },
        { 5593.998f,  10782.94f, 21.46227f },
        { 5594.748f,  10782.69f, 21.71227f },
        { 5599.091f,  10780.81f, 23.13058f },
        { 5599.861f, 10779.59f,  23.88957f },
        { 5601.611f, 10779.09f,  23.88957f },
        { 5607.111f, 10777.59f,  23.88957f },
        { 5613.611f, 10775.84f,  23.63957f },
        { 5614.844f, 10775.14f,  23.20417f },

        { 5614.283f, 10777.22f, 23.6332f },
        { 5615.283f, 10776.97f, 23.3832f },
        { 5616.783f, 10776.72f, 23.3832f },
        { 5620.783f, 10776.22f, 23.3832f },
        { 5622.533f, 10776.22f, 23.3832f },
        { 5623.533f, 10775.97f, 23.3832f },
        { 5626.533f, 10775.47f, 23.3832f },
        { 5628.033f, 10775.22f, 23.3832f },
        { 5629.283f, 10775.22f, 23.3832f },
        { 5630.716f, 10774.63f, 23.0040f },

        { 5628.418f, 10772.91f, 23.24298f },
        { 5630.668f, 10772.41f, 23.24298f },
        { 5635.340f, 10771.35f, 23.00403f },

        { 5636.704f, 10771.14f, 23.33329f },
        { 5640.704f, 10770.14f, 23.33329f },
        { 5643.204f, 10769.39f, 21.83329f },
        { 5644.069f, 10768.92f, 21.16256f },

        { 5645.643f, 10768.500f,  20.83989f },
        { 5646.893f, 10768.250f,  20.58989f },
        { 5648.393f, 10768.001f,  20.33989f },
        { 5653.217f, 10766.58f,   20.01723f },

        { 5659.624f, 10765.100f,  20.486f },
        { 5660.624f, 10764.850f,  20.736f },
        { 5664.874f, 10763.850f,  21.986f },
        { 5666.624f, 10763.350f,  22.986f },
        { 5667.374f, 10763.100f,  23.736f },
        { 5668.374f, 10763.100f,  23.986f },
        { 5669.624f, 10762.600f,  23.986f },
        { 5672.624f, 10761.850f,  23.736f },
        { 5678.874f, 10760.350f,  23.736f },
        { 5681.031f, 10759.630f,  23.455f },
        { 5693.950f, 10762.100f,  23.487f }
    }
};

static const std::array<Position, 2> g_SaprishPortalsPositions =
{
    {
        { 5761.180f, 10694.900f, 5.3478f }, ///< First Portal
        { 5919.460f, 10705.800f, 13.747f }, ///< Portal In Back of Saprish
    }
};

static const std::array<G3D::Vector3, 3> g_DoorPath =
{
    {
        { 6117.167f, 10356.10f, 19.82022f },
        { 6112.108f, 10349.75f, 19.82022f },
        { 6108.153f, 10343.51f, 19.69522f },
    }
};

static const std::array<std::pair<Position, float>, 4> g_VoidBlackHole = 
{
    { 
        std::make_pair(Position(6027.816f, 10653.337f, 10.834f), 30.f), 
        std::make_pair(Position(5940.193f, 10505.493f, 14.599f), 41.f), 
        std::make_pair(Position(5805.504f, 10523.822f, -6.4431f), 12.5f), 
        std::make_pair(Position(5647.166f, 10679.535f, 5.5617f), 15.f) 
    }
};

static const std::array<std::pair<Position, uint8>, 6> g_VoidTearsData = 
{
    {
        std::make_pair(Position(5982.64f,  10637.7f, 12.6401f), ePortalsQuotes::TalkUpperVoidBreach),
        std::make_pair(Position(5647.66f,  10678.4f, 5.02419f), ePortalsQuotes::TalkLowerVoidBreach),
        std::make_pair(Position(6023.38f,  10580.3f, 19.6582f), ePortalsQuotes::TalkCrumblingRuins),
        std::make_pair(Position(5870.92f,  10463.7f, 8.77059f), ePortalsQuotes::TalkTempleFoundation),
        std::make_pair(Position(5570.3f,  10590.6f, 5.79536f),  ePortalsQuotes::TalkRuinedOverlook),
        std::make_pair(Position(5761.18f,  10694.9f, 5.34789f), ePortalsQuotes::TalkLowerNetherGate),
    }
};

#endif