////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef MAW_OF_SOULS_HPP
# define MAW_OF_SOULS_HPP

# include "BrokenIslesPCH.h"
# include "Challenge.h"

static void CastSpellToPlayers(Unit* p_Caster, uint32 p_SpellID, bool p_Triggered)
{
    if (p_Caster == nullptr || !p_Caster->GetMap()->IsMythic())
        return;

    Map::PlayerList const& l_Players = p_Caster->GetMap()->GetPlayers();
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

enum eData
{
    DATA_YMIRON         = 0,
    DATA_HARBARON       = 1,
    DATA_HELYA          = 2,
    MAX_ENCOUNTER,

    DATA_YMIRON_RP_EVENT,
    DATA_YMIRON_GORN,
    DATA_SHIP,
    DATA_SKJAL_WALL
};

enum eCreatures
{
    /// Harbaron summons
    NPC_SOUL_FRAGMENT           = 98761,
    NPC_COSMIC_SCYTHE           = 98989,
    NPC_COSMIC_SCYTHE_2         = 100839,

    NPC_HELARJAR_CHAMPION       = 97097,
    NPC_THE_GRIMEWALKER         = 97185,
    NPC_SEACURSED_ROWER         = 102326,

    /// Helya
    NPC_HELYA                   = 96759,

    /// Helya summons
    NPC_DESTRUCTOR_TENTACLE     = 99803,  ///< Veh - 99801
    NPC_DESTRUCTOR_TENTACLE_2   = 99801,
    NPC_GRASPING_TENTACLE_1     = 99800,  ///< Veh right 1 98363,
    NPC_GRASPING_TENTACLE_2     = 100361, ///< Veh left 1  100362,
    NPC_GRASPING_TENTACLE_3     = 100359, ///< Veh left 2  100360,
    NPC_GRASPING_TENTACLE_4     = 100353, ///< Veh right 2 100354,
    NPC_SWIRLING_POOL_VEH       = 97099,  ///< Veh - создает АТ и и суммонит тентаклю.
    NPC_PIERCING_TENTACLE       = 100188,
    NPC_SWIRLING_POOL_TRIG      = 102306,
    NPC_SKELETAL_WARRIOR        = 98973,
    NPC_SKELETAL_SORCERER       = 99447,
    NPC_SEACURSED_SWIFTBLADE    = 98919,

    NPC_YMIRON                  = 96756,
    NPC_RISEN_SOLDIER           = 98246,

    /// Ymiron trash
    NPC_BIRD                    = 97163,
    NPC_SHIELDMAIDEN            = 102104,
    NPC_RUNECARVER              = 102375,
    NPC_SEACURSED_SLAVER        = 97043,
    NPC_WEAPON_VEHICLE          = 97208,

    /// Helya Trash
    NPC_SKJAL                   = 99307,
};

enum eGameObjects
{
    GO_YMIRON_GORN                  = 247041,
    GO_HARBARON_DOOR                = 246947,
    GO_SHIP                         = 246919,
    GO_HELYA_CHEST                  = 246036,
    GO_KVALDIR_CAGE                 = 245064,
    GO_KVALDIR_CAGE_2               = 245062,

    GO_TRAP_DOOR_001                = 246840,
    GO_TRAP_DOOR_002                = 246842,
    GO_TRAP_DOOR_003                = 246843,
    GO_TRAP_DOOR_004                = 246844,
    GO_TRAP_DOOR_005                = 246839,
    GO_TRAP_DOOR_006                = 246838,
    GO_TRAP_DOOR_007                = 246834,
    GO_TRAP_DOOR_009                = 246835,
    GO_TRAP_DOOR_010                = 246837,
    GO_TRAP_DOOR_011                = 246841,

    GO_PORTAL_TO_STORMHEIM          = 267034,
    GO_SKJAL_WALL_OF_SOULS          = 246435
};

enum eStateShip
{
    STATE_DESTROY       = 0,
    STATE_REBUILDING    = 1
};

enum eLocations
{
    LocationAfterYmiron     = 5101,
    LocationAfterHarbaron   = 5508
};

enum eAchievements
{
    InstantKarma            = 10413,
    PoorUnfortunateSouls    = 10412
};

enum eQuests
{
    MawOfSoulsAHopeInHelheim    = 39350,
    MawOfSoulsSpiritingAway     = 40560,
    MawOfSoulsPiercingTheMists  = 43596
};

Position const g_PlayerTeleportPos = { 2931.86f, 870.70f, 518.0f, 4.73f };

/// Used to teleport players if CheckRequiredBosses is not passed.
Position const g_StartPos = { 7213.03f, 7309.40f, 22.42f, 5.76f };

#endif ///< MAW_OF_SOULS_HPP
