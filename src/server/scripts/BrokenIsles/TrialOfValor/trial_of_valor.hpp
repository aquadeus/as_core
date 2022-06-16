////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAL_OF_VALOR_HPP
# define TRIAL_OF_VALOR_HPP

# include "ScriptMgr.h"
# include "ScriptedCreature.h"
# include "CreatureGroups.h"
# include "GridNotifiersImpl.h"
# include "Vehicle.h"
# include "SpellScript.h"
# include "InstanceScript.h"
# include "Group.h"
# include "ScriptedGossip.h"
# include "GameObjectAI.h"
# include "CreatureTextMgr.h"
# include "MoveSplineInit.h"

static void CastSpellToPlayers(Map* p_Map, Unit* p_Caster, uint32 p_SpellID, bool p_Triggered)
{
    if (p_Map == nullptr)
        return;

    Map::PlayerList const& l_Players = p_Map->GetPlayers();
    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
        {
            if (p_Caster != nullptr)
                p_Caster->CastSpell(l_Player, p_SpellID, p_Triggered);
            else
                l_Player->CastSpell(l_Player, p_SpellID, p_Triggered);
        }
    }
}

static void DespawnCreaturesInArea(uint32 p_Entry, WorldObject* p_WorldObject)
{
    std::list<Creature*> l_Creatures;
    GetCreatureListWithEntryInGrid(l_Creatures, p_WorldObject, p_Entry, p_WorldObject->GetMap()->GetVisibilityRange());

    for (Creature* l_Iter : l_Creatures)
        l_Iter->DespawnOrUnsummon();
}

enum eTovSpells
{
    SpellKneelingVisual             = 197227,
    SpellBelfrostAura               = 192635,
    SpellReady1h                    = 175387,
    SpellReady2h                    = 175155,
    RunicBrandAuraDummyN            = 231342,
    RunicBrandAuraDummyK            = 231311,
    RunicBrandAuraDummyH            = 231344,
    RunicBrandAuraDummyA            = 231345,
    RunicBrandAuraDummyI            = 231346,
    RunicShieldDmgPctTakenI         = 227594,
    RunicShieldDmgPctTakenA         = 227595,
    RunicShieldDmgPctTakenH         = 227596,
    RunicShieldDmgPctTakenN         = 227597,
    RunicShieldDmgPctTakenK         = 227598,
    BrandedFixateK                  = 227490,
    BrandedFixateN                  = 227491,
    BrandedFixateH                  = 227498,
    BrandedFixateA                  = 227499,
    BrandedFixateI                  = 227500,
    BrandedMythicK                  = 229579,
    BrandedMythicN                  = 229580,
    BrandedMythicH                  = 229581,
    BrandedMythicA                  = 229582,
    BrandedMythicI                  = 229583,
    DrawPowerDmgIncUnerringBlastK   = 227961,
    DrawPowerDmgIncUnerringBlastN   = 227973,
    DrawPowerDmgIncUnerringBlastH   = 227974,
    DrawPowerDmgIncUnerringBlastA   = 227975,
    DrawPowerDmgIncUnerringBlastI   = 227976,
    RunicBrandAreatriggerVisK       = 227483,
    RunicBrandAreatriggerVisN       = 227484,
    RunicBrandAreatriggerVisH       = 227485,
    RunicBrandAreatriggerVisA       = 227487,
    RunicBrandAreatriggerVisI       = 227488,
    SkyJumpTeleport                 = 231166,
    SkyJumpAura                     = 231167,

    /// Helya
    SpellHelyaTentacleLeft          = 234205,
    SpellHelyaTentacleRight         = 234204
};

enum eCleansingFlames
{
    CleansingFlamesOnRight          = 227455,
    CleansingFlamesOnCenter         = 227456,
    CleansingFlamesOnLeft           = 227448
};

enum eTovCreatures
{
    /// The High Gate
    CreatureVikingARpFriendly           = 114788,
    CreatureVikingARpFriendly02         = 114784,
    CreatureVikingARpFriendly03         = 114786,

    CreatureVikingBRpFriendly           = 114787,
    CreatureVikingBRpFriendly02         = 114785,

    CreatureVikingBlueGuard             = 114789,

    /// Hall of Glory
    CreatureAudience                    = 96574,
    CreatureAudience01                  = 95842,
    CreatureAudience02                  = 95834,
    CreatureAudience03                  = 97087,

    /// The Ephemeral Way
    CreatureValkyrShieldMaiden          = 114791,

    /// Odyn encounter
    CreatureBossOdyn                    = 114263,
    CreatureHymdall                     = 114361,
    CreatureDancingBlade                = 114560,
    CreatureHyrja                       = 114360,
    CreatureValajarRuneBearer           = 114996,
    CreatureRagingTempest               = 112996,
    CreatureSpearOfLight                = 100575,
    CreatureWorldTrigger                = 83816,

    /// Helya's trashes
    CreatureBrazierStalker              = 114672,
    CreatureKvaldirSpiritTender         = 114538,
    CreatureBonespeakerSoulBinder       = 114532,
    CreatureDarkSeraph                  = 114922,
    CreatureKvaldirTideWitch            = 114813,
    CreatureDeepbrineMonstruosity       = 114932,
    CreatureKvaldirReefcaller           = 114539,
    CreatureAncientBonethrall           = 114547,
    CreatureRotsoulGiant                = 114548,
    CreatureKvaldirCoralMaiden          = 114811,
    CreatureGraspingTentacleLeft        = 117783,
    CreatureGraspingTentacleRight       = 117784,

    /// Helya's encounter
    CreatureBossHelya                   = 114537,
    CreatureBillewaterSlime             = 116195,
    CreatureNightWatchMariner           = 114809,
    CreatureNightWatchMarinerImage      = 114812,
    CreatureGrimelord                   = 114709,
    CreatureAnchor                      = 104347,
    CreatureDecayingMinion              = 114568,
    CreatureBilewaterSlime              = 114553,
    CreatureOrbOfCorruptionVehicle      = 115166,
    CreatureOrbOfCorruption             = 116214,
    CreatureOrbOfCorruptionDummy        = 114523,
    CreatureGrippingTentacle            = 114900,
    CreatureGrippingTentacle1           = 114901,
    CreatureGrippingTentacle2           = 114905,
    CreatureStrikingTentacle            = 114881,
    CreatureKvaldirLongboat             = 115941,
    CreatureOrbOfCorrosionDummy         = 114566,
    CreatureOrbOfCorrosionVehicle       = 114535,

    /// Helya's outro
    CreatureOutroOdyn                   = 116760,
    CreatureOutroHyrja                  = 116830,
    CreatureOutroEyir                   = 116818,
    CreatureOutroHymdall                = 116761,
    CreatureOutroStormforgedSentinel    = 116763,
    CreatureOutroChosenOfEyir           = 116762,

    /// The Eternal Battlefield

    /// Guarm Encounter
    CreatureBossGuarm                   = 114323, ///< Base creature boss
    CreatureGuarmHeadLeft               = 114341, ///< Left to right
    CreatureGuarmHeadMiddle             = 114343,
    CreatureGuarmHeadRight              = 114344,
    CreatureGuarmChewToy                = 116404
};

enum eTovGameObjects
{
    /// The Ephemeral Way
    GameObjectValhallaBridge001     = 246144,

    /// The High Gate
    GameObjectVahallaGate02         = 245701,

    /// Epmhereal Way
    GameObjectGatewayOfSplendor     = 244664,
    GameObjectOldInstancePortal     = 253130, ///< Green stuff.

    /// Hall of Glory
    GameObjectOdynRuneSigilN        = 245697,
    GameObjectOdynRuneSigilK        = 245698,
    GameObjectOdynRuneSigilI        = 245699,
    GameObjectOdynRuneSigilA        = 245696,
    GameObjectOdynRuneSigilH        = 245695,

    GameObjectOdynGate              = 246145,

    GameObjectOdynChest             = 260526, ///< Spoils

    /// Haustvald
    GameObjectPortalToHelheim       = 241758,
    GameObjectPortalTo2ndRealm      = 266845,

    GameObjectCampfire              = 266530,

    /// The Eternal Battlfield
    GameObjectGuarmBossDoor01       = 266532,
    GameObjectGuarmBossDoor02       = 266533,

    /// Seat of Ascension
    GameObjectValhallaGate03        = 245702,

    /// Helya
    GameObjectLightRay1             = 266889,
    GameObjectLightRay2             = 266890,
    GameObjectRitualStone           = 265596
};

enum eTovData
{
    DataBossOdyn,
    DataBossGuarm,
    DataBossHelya,
    DataMaxBosses,

    DataSaveOdynIntroduction,
    DataHelyaRitualStones
};

enum eTovAchievements
{
    YouRunedEverything  = 11337,
    BonefiedTriTip      = 11386,
    PatientZero         = 11377,
    TheChosen           = 11387
};

enum eToVLocations
{
    LocHelheimRaidEntrance          = 5775,
    LocHelheimRaidHelheimEntrance   = 5827,
    LocHelheimRaidStormheim         = 5839,
    LocHelheimRaidExitTarget        = 5779
};

std::vector<uint32> const g_Runes =
{
    eTovGameObjects::GameObjectOdynRuneSigilN,
    eTovGameObjects::GameObjectOdynRuneSigilK,
    eTovGameObjects::GameObjectOdynRuneSigilI,
    eTovGameObjects::GameObjectOdynRuneSigilA,
    eTovGameObjects::GameObjectOdynRuneSigilH
};

std::vector<uint32> const g_RunicBrandSpells =
{
    eTovSpells::RunicBrandAuraDummyN,
    eTovSpells::RunicBrandAuraDummyK,
    eTovSpells::RunicBrandAuraDummyI,
    eTovSpells::RunicBrandAuraDummyA,
    eTovSpells::RunicBrandAuraDummyH
};

std::vector<uint32> const g_SpellEntries =
{
    eTovSpells::DrawPowerDmgIncUnerringBlastK,
    eTovSpells::DrawPowerDmgIncUnerringBlastN,
    eTovSpells::DrawPowerDmgIncUnerringBlastA,
    eTovSpells::DrawPowerDmgIncUnerringBlastH,
    eTovSpells::DrawPowerDmgIncUnerringBlastI
};

std::vector<uint32> const g_ArraySpells =
{
    eCleansingFlames::CleansingFlamesOnRight,
    eCleansingFlames::CleansingFlamesOnCenter,
    eCleansingFlames::CleansingFlamesOnLeft
};

std::vector<uint32> const g_MythicBranded =
{
    eTovSpells::BrandedMythicN,
    eTovSpells::BrandedMythicK,
    eTovSpells::BrandedMythicI,
    eTovSpells::BrandedMythicA,
    eTovSpells::BrandedMythicH
};

std::vector<uint32> const g_AtsVisual =
{
    eTovSpells::RunicBrandAreatriggerVisK,
    eTovSpells::RunicBrandAreatriggerVisN,
    eTovSpells::RunicBrandAreatriggerVisH,
    eTovSpells::RunicBrandAreatriggerVisA,
    eTovSpells::RunicBrandAreatriggerVisI
};

std::vector<uint32> const g_FixationSpells =
{
    eTovSpells::BrandedFixateK,
    eTovSpells::BrandedFixateN,
    eTovSpells::BrandedFixateH,
    eTovSpells::BrandedFixateA,
    eTovSpells::BrandedFixateI
};

#define g_HaustvaldReinforcementSpawnOrientation 3.905403f

Position const g_HaustvaldFirstPortal  = { 3652.2397f, 5561.8760f, -8.7236f, 2.1258f };
Position const g_HaustvaldSecondPortal = { 341.2198f, 365.1756f, 29.5994f, 1.023189f };

const std::vector<G3D::Vector3> g_HaustvaldReinforcemanSpawnFormation =
{
  { 3657.8201f, 5561.7500f, -9.9903f, },
  { 3662.1523f, 5558.3140f, -11.9404f },
  { 3653.0801f, 5560.4702f, -9.1824f  },
  { 3651.8999f, 5555.7900f, -10.0510f },
  { 3651.8999f, 5555.7900f, -10.0510f }
};

#endif ///< HALLS_OF_VALOR_HPP
