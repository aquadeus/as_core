////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef TOMB_OF_SARGERAS_H
# define TOMB_OF_SARGERAS_H

# include "AreaTrigger.h"
# include "ScriptedCreature.h"
# include "ScriptMgr.h"
# include "SpellAuras.h"
# include "SpellScript.h"
# include "InstanceScript.h"
# include "CreatureTextMgr.h"

#define TOSScriptName "instance_tomb_of_sargeras"

Position const PositionTeleKadgar[2] =
{
    { 5986.409f, -795.8195f, 2978.126f, 6.269365f }, ///< Pre Event Gorroth
    { 6205.62f, -793.69f, 2974.40f, 6.18f } ///< After Defeat Gorroth
};

Position const PositionTeleMaiev[2] =
{
    { 5990.001f, -801.5347f, 2977.88f, 0.3382317f }, ///< Pre Event Gorroth
    { 6196.97f, -799.62f, 2974.5f, 0.28f } ///< After Defeat Gorroth
};

Position const PositionTeleIllidari[2] =
{
    { 5993.796f, -789.9375f, 2976.677f, 5.799626f }, ///< Pre Event Gorroth
    { 6196.59f, -788.4439f, 2974.5022f, 6.11f } ///< After Defeat Gorroth
};

Position const PositionTeleProphetVelen[2] =
{
    { 5978.724f, -801.1215f, 2978.127f, 0.1859942f }, ///< Pre Event Gorroth
    { 6205.297f, -802.4968f, 2974.3984f, 0.62f } ///< After Defeat Gorroth
};

static Position const g_MaidenCenterPos         = { 6347.99f,   -795.1302f, 2888.0f, 0.0f };
static Position const g_SistersDeadZoneCenter   = { 6091.390f,  -995.314f,  2902.322f     };
static Position const g_DesolateCenterRoom      = { 6438.200f,  -1089.589f, 2883.929f   };
static Position const g_KilJaedenCenterPos      = { 4500.056f, -1510.1171f, 5385.645f, 4.689f };

enum eMiscSpells
{
    SpellSpiritOfRedemptionAura = 27827
};

class CheckDeadTarget
{
    public:
        bool operator()(WorldObject* target) const
        {
            if (Unit* unit = target->ToUnit())
                return (!unit->isAlive() || unit->HasAura(eMiscSpells::SpellSpiritOfRedemptionAura) || (unit->IsPlayer() && unit->ToPlayer()->isGameMaster()));
            return false;
        }
};

enum eDungeonIDs
{
    DungeonIDNone               = 0,
    DungeonIDTheGatesofHell     = 1494,
    DungeonIDWailingHalls       = 1495,
    DungeonIDChamberOfTheAvatar = 1496,
    DungeonIDDeceiversFall      = 1497
};

enum eLocations
{
    LocationRaidEntrance        = 5889,
    LocationChamberOfMoon       = 6015,
    LocationWailingHalls        = 6014,
    LocationChamberOfTheAvatar  = 6016,
    LocationKJSpaceship         = 6017,
    LocationAvatarChamber       = 6095,
    LocationAfterKJDeath        = 6117
};

enum eData
{
    DataGoroth, ///< The Gates of Hell
    DataHarjatan,
    DataMistressSasszine,
    DataDemonicInquisition, ///< Wailing Halls
    DataSistersOfTheMoon,
    DataDesolateHost,
    DataMaidenOfVigilance, ///< Chamber of the Avatar
    DataFallenAvatar,
    DataKiljaeden, ///< Deceiver's Fall
    MaxEncounters,
    DataPreEventGoroth, ///< Custom Encounters
    DataMageKadgar,
    DataIllidari,
    DataNoBossAtrigan,
    DataBreach,
    DataProphetVelen,
    DataMaievShadowsong,
    DataPreEventHarjatan,
    DataNoBossMistressSasszine,
    DataPreEventDemonicInquisition,
    DataDungeonID
};

enum eCreatures
{
    /// Intro (Pre Event)Goroth NPCS
    NpcMageKadgar                = 119726,
    NpcIllidari                  = 119729,
    NpcEventAtrigan              = 120996,
    NpcBreach                    = 121605, ///< For Event with Infernal summon
    NpcProphetVelen              = 119728,
    NpcMaievShadowsong           = 119730,
    NpcLegionfallSoldierHorde    = 119768,
    NpcLegionfallSoldierAlliance = 121023,
    NpcFelguardInvader           = 119742,
    NpcFelguardInvader2          = 121024,
    NpcImageOfAegwynn            = 119723,
    NpcDurganStonestorm          = 119777,
    NpcWindRushTotem             = 121633,

    /// Intro Event(Pre Event) Harjatan
    NpcEventMistressSasszine     = 121184, /// no boss
    /// Goroth
    NpcGoroth                    = 115844,
    /// Harjatan
    NpcHarjatan                  = 116407,
    NpcIncubatedEgg              = 120545,
    NpcMythicEggController       = 120557,
    /// Quest npc Mistress Sassz'ine
    /// Mistress Sassz'ine
    NpcMistressSasszine          = 115767,

    /// Trashmob
    NpcAegwynnTrap               = 120784,

    /// Sisters of The Moon
    BossHuntressKasparian       = 118523,
    BossPriestessLunaspyre      = 118518,
    BossCaptainYathaeMoonStrike = 118374,
    NpcMoonStalker              = 118182,
    NpcGlaiveTarget             = 119054,
    NpcMoontalon                = 119205,
    NpcTwilightSoul             = 121498,

    /// Demonic Inquisition
    NpcAtrigan                   = 116689,
    NpcBelac                     = 116691,

    /// Desolate Host
    BossDesolateHost            = 119072,
    BossEngineOfSouls           = 118460,
    BossSoulQueenDejahna        = 118462,
    NpcDesolateHostDummy        = 120368,
    NpcReanimatedTemplar        = 118715,
    NpcGhastlyBonewarden        = 118728,
    NpcFallenPriestess          = 118729,
    NpcSoulResidue              = 118730,
    NpcSoulFont                 = 118701,
    NpcSoulFont2                = 118699,

    /// Maiden of Vigilance
    BossMaidenOfVigilance       = 118289,
    NpcEonicDefender            = 120153,
    NpcFragmentOfCreation       = 118640,
    NpcFragmentOfObliteration   = 118643,
    NpcTitanicAnomaly           = 120552,

    /// Fallen Avatar
    BossFallenAvatar            = 116939,
    NpcMaidenOfValor            = 117264,
    NpcContainementPylon        = 117279,
    NpcPylonBeamStalker         = 119846,
    NpcShadowyBlade             = 119158,
    NpcTouchOfSargeras          = 120838,
    NpcBlackWinds               = 121294,
    NpcFallenAvatarInfernalImpact = 120961,
    NpcFallenAvatarKilJaeden    = 120867,
    NpcFallenAvatarKhadgar      = 121171,
    NpcFallenAvatarIllidan      = 121169,
    NpcFallenAvatarVelen        = 121170,
    NpcDefensiveCountermeasure  = 120449,

    /// Kil'jaeden
    NpcIxallonTheSoulbreaker    = 118930,
    BossKilJaeden               = 117269,
    BossProphetVelen            = 118052,
    BossIllidanStormrage        = 124590,
    NpcArmageddonStalker        = 120839,
    NpcEruptingReflection       = 119206,
    NpcWailingReflection        = 119107,
    NpcShadowsoul               = 121193,
    NpcIllidanStormrage         = 121227,
    NpcDemonicObelisk           = 120270,
    NpcNetherRift               = 120390,

    /// Pillars of Creation Images
    NpcHammerKhazgorothImage    = 121494,
    NpcTearsOfEluneImage        = 121495,
    NpcTidestoneGolganethImage  = 121496,
    NpcAegisOfAggramarImage     = 121630,

    /// Pillars of Creation Boss Room
    NpcHammerOfKhazgoroth       = 121111,
    NpcTearsOfElune             = 121110,
    NpcTidestoneofGolganneth    = 121105,
};

enum eGameObjects
{
    GoCrashingComet             = 269839,
    GoCrashingComet2            = 269838,
    GoDoorRoomGorothS           = 269974,
    GoDoorRoomGorothN           = 269975,
    GoDoorRoomGorothE           = 269973,
    GoDoorRoomGorothW           = 272802,
    GoDoorRoomHarjatanS         = 269192,
    GoDoorRoomHarjatanW         = 269120,
    GoDoorRoomSasszineS         = 269976,
    GoDoorBeforeInquisition     = 268580,
    GoDoorRoomInquisition       = 268579,

    /// Mistress Sasszine
    GoGolganethTidePillarPlace  = 269782,

    /// Demonic Inquisition
    GoKhazgorothPillarPlace     = 269780,

    /// Sisters of the Moon
    GoDoorPreSistersOfTheMoonControlUnit  = 269840,
    GoDoorPreSistersOfTheMoonControlUnit2 = 269841,
    GoDoorPreSistersOfTheMoon   = 268752,
    GoDoorPreSistersOfTheMoon2  = 270940,
    GoDoorPreSistersOfTheMoon3  = 268514,
    GoDoorPreSistersOfTheMoon4  = 268750,
    GoDoorPreSistersOfTheMoon5  = 268365,
    GoDoorSistersOfTheMoon      = 269045,
    GoDoorPassageDesolateHost   = 268751,

    /// Desolate Host
    GoDoorDesolateHost          = 268749,
    GoTearsOfElunePillarPlace   = 269779,

    /// Maiden of Vigilance
    GoDoorBeforeMaidenRoom      = 269164,
    GoFelstormCollision         = 269988,
    GoElevatorToFallenAvatar    = 268574,

    /// Fallen Avatar
    GoDoorPreFallenAvatar       = 269262,
    GoLeftChain                 = 269795,
    GoRightChain                = 269794,
    GoFelStorm                  = 269783,
    GoFelStormCollision1        = 269987,
    GoFelStormCollision2        = 269988,
    GoUpperPlatform             = 267934,
    GoLowerPlatform1            = 267709,
    GoLowerPlatform2            = 267712,
    GoLowerPlatform3            = 267711,
    GoLowerPlatform4            = 267714,
    GoLowerPlatform5            = 267703,
    GoLowerPlatform6            = 267701,
    GoLowerPlatform7            = 267704,
    GoLowerPlatform8            = 267702,
    GoLowerPlatform9            = 267713,
    GoLowerPlatform10           = 267706,
    GoLowerPlatform11           = 267707,
    GoLowerPlatform12           = 267708,
    GoLowerPlatform13           = 267700,
    GoLowerPlatform14           = 267710,
    GoLowerPlatform15           = 267705,
    GoLowerPlatform16           = 267724,
    GoLowerPlatform17           = 267716,
    GoLowerPlatform18           = 267721,
    GoLowerPlatform19           = 267718,
    GoLowerPlatform20           = 267719,
    GoLowerPlatform21           = 267722,
    GoLowerPlatform22           = 267723,
    GoLowerPlatform23           = 267715,
    GoLowerPlatform24           = 267717,
    GoLowerPlatform25           = 267720,

    /// Kil'jaeden
    GoBridgeNorth               = 271127,
    GoBridgeSouth               = 271126,
    GoKJSkybox                  = 269255,
    GoKJChest                   = 271099
};

enum ePhasesMasks
{
    PhaseCorporealRealm = 8309,
    PhaseSpiritRealm    = 8308
};

enum eSharedSpells
{
    SpellAegwynnTraps       = 240825,
    SpellSpiritRealm        = 235621,
    SpellSpiritRealmVisual  = 235732,
    SpellCorporealRealm     = 235734,
    SpellStartKJIntroConv   = 243326,
    SpellBerserk            = 26662,
    SpellBerserkOther       = 64238,
    SpellSafeFall30s        = 231462,
    SpellCurseOfGuldan      = 241742,
    SpellWindRushTotem      = 243002,
    SpellArgusFilter        = 258803
};

enum eEncounters
{
    EncounterKilJaeden      = 2051
};

enum eAchievements
{
    SkyWalker                   = 11675,
    DarkSouls                   = 11770,
    AheadOfTheCurveKilJaeden    = 11874,
    CuttingEdgeKilJaeden        = 11875,
    WaxOnWax                    = 11773,
    GreatSoulGreatPurpose       = 11674
};

struct SearcherCmp
{
    bool operator() (WorldObject*& p_Lhs, WorldObject*& p_Rhs) const
    {
        if (p_Lhs == nullptr || p_Rhs == nullptr)
            return false;

        Player* l_LhsPlayer = p_Lhs->ToPlayer();
        Player* l_RhsPlayer = p_Rhs->ToPlayer();

        if (l_LhsPlayer == nullptr || l_RhsPlayer == nullptr)
            return false;

        uint32 l_LhsRole = l_LhsPlayer->GetRoleForGroup();
        uint32 l_RhsRole = l_RhsPlayer->GetRoleForGroup();

        if (l_LhsRole == ROLE_DAMAGE && l_RhsRole == ROLE_DAMAGE)
            return l_LhsPlayer->IsRangedDamageDealer(false) && !l_RhsPlayer->IsRangedDamageDealer(false);

        return l_LhsRole > l_RhsRole;
    }
};

#endif // TOMB_OF_SARGERAS_H