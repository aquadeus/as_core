////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScenarioMgr.h"

enum eCreatures
{
    /// Imp Mother's Den
    CreatureAgatha                  = 115638,
    CreatureFumingImp               = 115640,
    CreatureSmolderingImp           = 115641,
    CreatureImpServant1             = 115643,
    CreatureImpServant2             = 115719,
    CreatureUmbralImp               = 115642,
    CreatureFelfireBoulder          = 121109,
    /// Feltotem's Fall
    CreatureJormogTheBehemoth       = 117484,
    CreatureTugarBloodtotem         = 117230,
    CreatureFinalBossDefeated       = 117368,
    CreatureJormogTrigger           = 119900,
    CreatureGasCloudStalker         = 119879,
    CreatureNavaroggOutro           = 117863,
    /// The Archmage's Reckoning
    CreatureArchmageXylem           = 115244,
    CreatureArchmageXylemDummy      = 116295,
    CreatureOldInvisibleMan         = 64693,
    CreatureRazorIce                = 116806,
    CreatureCorruptingShadows       = 116839,
    /// Thwarting the Twins
    CreatureRaestMagespear          = 116409,
    CreatureKaramMagespear          = 116410,
    CreatureOrbiter0                = 119580,
    CreatureOrbiter1                = 119581,
    CreatureOrbiter2                = 119582,
    CreatureOrbiter3                = 119583,
    CreatureOrbiter4                = 119584,
    /// The God-Queen's Fury
    CreatureValarjarChampion        = 97087,
    CreatureValarjarThundercaller   = 95842,
    CreatureValarjarRunecarver      = 96664,
    CreatureValarjarMystic          = 95834,
    CreatureStormforgedSentinel     = 96574,
    CreatureValarjarMarksman        = 96640,
    CreatureSigryn                  = 116484,
    CreatureJarlVelbrand            = 116499,
    CreatureRuneseerFaljar          = 116496,
    CreatureEyir                    = 119814,
    /// End of the Risen Threat
    CreatureJarodShadowsong         = 118447,
    CreatureGrannyMarl              = 118448,
    CreatureCallieCarrington        = 118451,
    CreatureCorruptedRisenSoldier   = 118489,
    CreatureCorruptedRisenSoldier2  = 119474,
    CreatureCorruptedRisenMage      = 118491,
    CreatureCorruptedRisenArbalest  = 118492,
    CreatureFlickeringEye           = 119475,
    CreatureDreadCorruptor          = 118487,
    CreatureFelBatPup               = 119469,
    CreatureDamagedSoul             = 119470,
    CreatureLordErdrisThorn         = 118488,
    CreatureDamagedSoldier          = 119476,
    CreatureDamagedMage             = 119477,
    CreatureDamagedArcher           = 119480,
    CreatureFinalBossDefeatedHealer = 118529,
    /// The Highlord's Return
    CreatureProphetVelen            = 117042,
    CreatureKorvasBloodthorn        = 116880,
    CreatureShadowSweeper           = 118263,
    CreaturePortal                  = 118452,
    CreatureEmpoweredDoomherald     = 117228,
    CreatureDarkbladeChampion       = 117231,
    CreatureHighlordKruul           = 117198,
    CreatureInquisitorVariss        = 117933,
    CreatureTormentingEye           = 118032,
    CreatureGenericBunny            = 54638
};

enum eGameObjects
{
    GameObjectPortalToAzshara   = 267033,
    GameObjectPortalToDalaran   = 267183,
    GameObjectBarrier           = 269149,
    GameObjectNetherBreach      = 266696,
    GameObjectFirstDoor         = 247402,
    GameObjectDoorToThirdStage  = 245104,
    GameObjectDoorToThirdStage2 = 245105,
    GameObjectDoorToFirstStairs = 268369,
    GameObjectDoorToLastRoom    = 267688,
    GameObjectStatue            = 267010
};

enum eDungeons
{
    DungeonTheImpMothersDen         = 1417,
    DungeonFeltotemsFall            = 1445,
    DungeonTheArchmagesReckoning    = 1406,
    DungeonThwartingTheTwins        = 1452,
    DungeonTheGodQueensFury         = 1446,
    DungeonEndOfTheRisenThreat      = 1471,
    DungeonTheHighlordsReturn       = 1440
};

enum eData
{
    /// Getters
    DataPlayerClass,
    DataPlayerGuid,
    DataTwartingTheTwins,
    DataRisenThreatMoves,
    DataRisenThreatWaves,
    DataRisenThreatCapture,
    DataRisenThreatEyes,
    DataRisenThreatTimer,

    /// Values
    DataResLocID        = 5983,
    DataXylemDummyCount = 37,
    DataMaxPortals      = 4,
    DataMaxEyes         = 6,
    DataInvisDisplay    = 11686,
    DataMaxDarkWings    = 4,
    DataMaxDarkWingsPos = 11,
    /// The Imp Mother's Den
    DataAgathaAsset     = 55521,
    /// Feltotem's Fall
    DataTugarAsset      = 58089,
    DataJormogAsset     = 58088,
    /// The Archmage's Reckoning
    DataIntroAsset      = 55486,
    DataGainingPower1   = 55527,
    DataGainingPower2   = 55884,
    DataGainingPower3   = 55885,
    DataTheShadowWithin = 55528,
    /// Thwarting the Twins
    DataRaestAsset      = 56197,
    DataTwinsAsset      = 56198,
    /// The God-Queen's Fury
    DataDefendingHalls  = 56051,
    DataShieldmaiden    = 56052,
    /// End of the Risen Threat
    DataWavesCount      = 5,
    DataSpiritsToHeal   = 8,
    DataAlliesAlive     = 56912,
    DataStage1Asset     = 56705,
    DataStage2Asset     = 56527,
    DataStage3Timer     = 56915,
    DataStage3Asset     = 56562,
    DataStage4Asset1    = 56560,
    DataStage4Asset2    = 56977,
    DataStage5Asset     = 56911,
    DataStage6Asset     = 56980,
    DataStage7Asset     = 56563,
    /// The Highlord's Return
    DataStatueAnimID    = 10455,
    DataFirstStep       = 55971,
    DataSecondStep1     = 56480,
    DataSecondStep2     = 56553,
    DataThirdStep1      = 56552,
    DataThirdStep2      = 56796,
    DataQuestID         = 45416
};

enum eCommonSpells
{
    SpellGasCloudAura   = 238445,
    SpellGasCloudDoT    = 238443,
    SpellFeignDeath     = 159474,
    SpellFireVisual     = 181569,
    SpellFelChannel     = 233436,
    SpellFelCorruption  = 239026,
    SpellFelTouched     = 237582,
    SpellRestartNPCs    = 236132,
    SpellRestartPlayer  = 236134,
    SpellDustyPockets   = 238986
};
