////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef VAULT_OF_THE_WARDENS_HPP
# define VAULT_OF_THE_WARDENS_HPP

# include "BrokenIslesPCH.h"
# include "Challenge.h"


Position const g_AshgolmCenterPos = { 4239.14f, -451.332f, 105.947f, 0.0f };

Position const g_UnderTormentorumPos = { 4450.80f, -450.92f, 85.0f, 0.0f };

enum eData
{
    DATA_SALTHERIL      = 0,
    DATA_TORMENTORUM    = 1,
    DATA_ASHGOLM        = 2,
    DATA_GLAZER         = 3,
    DATA_CORDANA        = 4,
    MAX_ENCOUNTER,

    DATA_ASHGOLM_SYSTEM,
    DATA_GLAZER_SYSTEM
};

enum eCreatures
{
    /// Tirathon Saltheril
    NPC_TIRATHON_SALTHERIL      = 95885,
    NPC_DRELANIM_WHISPERWIND    = 99013,
    NPC_GLAIVE_01               = 102643,
    NPC_GLAIVE_02               = 102644,
    NPC_FOUL_MOTHER             = 98533,

    /// Inquisitor
    NPC_SHADOWMOON_WARLOCK      = 99704,
    NPC_FELGUARD_ANNIHILATOR    = 99644,
    NPC_SHADOWMOON_TECHNICIAN   = 99645,
    NPC_ENORMOUS_STONE_QUILEN   = 99675,
    NPC_MOGUSHAN_SECRET_KEEPER  = 99676,
    NPC_VOID_TOUCHED_JUGGERNAUT = 99655,
    NPC_DERANGED_MINDFLAYER     = 99657,
    NPC_FACELESS_VOIDCASTER     = 99651,
    NPC_LINGERING_CORRUPTION    = 99678,
    NPC_BILE_CREDIT             = 112768,

    /// Mini bosses
    NPC_GLAVIANA_SOULRENDER     = 98177,
    NPC_BLADE_DANCER_ILLIANNA   = 96657,
    NPC_BLADE_DANCER_PET_1      = 102583,
    NPC_BLADE_DANCER_PET_2      = 102584,
    NPC_DREADLORD_MENDACIUS     = 99649,

    /// Ashgolm
    NPC_ASH_GOLM                = 95886,
    NPC_EMBER                   = 99233,
    NPC_COUNTERMEASURES         = 99240,
    NPC_FREEZE_STATUE           = 99277,
    NPC_UPDRAFT                 = 101548,

    /// Glazer
    NPC_GLAZER                  = 95887,
    NPC_GLAZER_BEAM             = 102310,
    NPC_LENS                    = 98082,
    NPC_BEAM_TARGET_TRIGGER     = 98118,
    NPC_SAFETY_NET              = 103093,
    NPC_OVERLOADED_LENS         = 113552,

    /// Cordana
    NPC_SPIRIT_OF_VENGEANCE     = 100364,
    NPC_CORDANA_FELSONG         = 95888,
    NPC_DEEPENING_SHADOW        = 107533,
    NPC_GLOWING_SENTRY          = 100525,
    NPC_AVATAR_OF_SHADOW        = 104293,
    NPC_GRIMHORN_THE_ENSLAVER   = 102566,
    NPC_CAGE                    = 102572,
    NPC_DRELANIM_WHISPERWIND_2  = 103860,
    NPC_ELUNES_LIGHT_FIGHT      = 103342,
    NPC_CREEPING_DOOM           = 100336,
    NPC_BAT                     = 100285
};

enum eGameObjects
{
    GO_SALTHERIL_DOOR_1     = 246055,
    GO_SALTHERIL_DOOR_2     = 246054,
    GO_SALTHERIL_DOOR_3     = 246053,
    GO_SALTHERIL_DOOR_4     = 247081,
    GO_SALTHERIL_DOOR_5     = 247082,
    GO_TORMENTORUM_DOOR_1   = 246112,
    GO_TORMENTORUM_DOOR_2   = 246110,
    GO_TORMENTORUM_DOOR_3   = 246111,
    GO_ASHGOLM_PLATFORM     = 246418,
    GO_ASHGOLM_BRIDGE       = 246057,
    GO_GLAZER_BRIDGE        = 246727,
    GO_CORDANA_DOOR         = 246049,
    GO_MOTHER_DOOR          = 247080,

    /// Ashgolm
    GO_ASHGOLM_VOLCANO      = 247010,

    /// Glazer
    GO_OWL_STATUE_MIN       = 246058,
    GO_OWL_STATUE_MAX       = 246082,

    /// Torment cages
    GO_CAGE_1               = 246506, ///< Orks
    GO_CAGE_2               = 246507,
    GO_CAGE_3               = 246508,
    GO_CAGE_4               = 246509, ///< Mogu
    GO_CAGE_5               = 246510,
    GO_CAGE_6               = 246511,
    GO_CAGE_7               = 246512, ///< Void
    GO_CAGE_8               = 246513,
    GO_CAGE_9               = 246514,
    GO_CAGE_10              = 246515, ///< Corruptions
    GO_CAGE_11              = 246516,
    GO_CAGE_12              = 246517
};

enum eVotWSpells
{
    SpellFelDissolveIn          = 211762,
    SpellElunesLightOverride    = 204485,
    SpellElunesLightOrbVisual2  = 192656,
    SpellElunesLightSummon      = 204491,
    SpellVeiledInShadow         = 192750,
    SpellRevealed               = 192752,
    SpellHunterFlareAT          = 132950
};

enum eLocations
{
    LocEntrance             = 5105,
    LocBoss1Defeated        = 5106,
    LocBoss2Xor3Defeated    = 5107,
    LocBoss2And3Defeated    = 5108,
    LocBoss4EngagedOnce     = 5112
};

static bool IsNearElunesLight(Creature* p_Creature)
{
    if (p_Creature == nullptr)
        return false;

    Map* l_Map = p_Creature->GetMap();

    if (l_Map == nullptr)
        return false;

    Map::PlayerList const& l_Players = l_Map->GetPlayers();

    for (auto l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
    {
        Unit* l_Target = l_Itr->getSource();

        if (l_Target == nullptr)
            continue;

        if (l_Target->GetDistance2d(p_Creature) >= 7.0f)
            continue;

        if (l_Target->HasAura(eVotWSpells::SpellElunesLightOverride) && 
            l_Target->HasAura(eVotWSpells::SpellElunesLightOrbVisual2))
            return true;
    }

    if (p_Creature->FindNearestCreature(NPC_ELUNES_LIGHT_FIGHT, 7.0f))
        return true;

    /// http://www.wowhead.com/vault-of-the-wardens-dungeon-strategy-guide#comments:id=2421055
    if (p_Creature->FindNearestAreaTrigger(eVotWSpells::SpellHunterFlareAT, 17.0f))
        return true;

    return false;
}

#endif ///< VAULT_OF_THE_WARDENS_HPP
