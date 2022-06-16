////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef BLACK_ROOK_HOLD_DUNGEON_HPP
# define BLACK_ROOK_HOLD_DUNGEON_HPP

# include "BrokenIslesPCH.h"
# include "Challenge.h"

enum eData
{
    Amalgam     = 0,
    Illysanna   = 1,
    Smashspite  = 2,
    Kurtalos    = 3,
    MAX_ENCOUNTER,

    KurtalosState,
    AmalgamOutro,
    IllysannaIntro
};

enum eCreatures
{
    /// Amalgam of Souls
    SoulEchoesStalker               = 99090,
    RestlessSoul                    = 99664,
    FrustratedSoul                  = 99663,
    LordEtheldrinRavencrest         = 99857,
    StaellisRivermoor               = 99426,
    LadyVelandrasRavencrest         = 99858,
    SecretDoorStalker               = 103662,
    GhostlyProtector                = 98368,
    GhostlyRetainer                 = 98366,
    GhostlyCouncilor                = 98370,
    AmalgamofSouls                  = 98542,

    /// Ambush Event
    RookSpiderling                  = 98677,
    RookSpider                      = 98681,
    RisenArcher                     = 98275,
    RisenArcanist                   = 98280,
    RisenScout                      = 98691,
    SoulTornChampion                = 98243,
    RisenCompagnion                 = 101839,

    /// Illysanna
    EyeBeamStalker                  = 100436,
    SoulTornVanguard                = 100485,
    CommanderShemdahSohn            = 98706,

    /// Smashspite
    Boulder                         = 111706,
    FelBat                          = 100759,
    FelBatPup                       = 102781,
    FelspiteDominator               = 102788,
    SmashspiteEntry                 = 98949,
    RisenSwordsman                  = 102094,
    RisenLancer                     = 102095,

    /// Kurtalos
    NPC_KURTALOS                    = 98965,
    NPC_LATOSIUS                    = 98970,
    NPC_DANTALIONAX                 = 99611,
    NPC_SOUL_KURTALOS               = 101054,
    NPC_IMAGE_OF_LATOSIUS           = 101028,
    NPC_KURTALOS_BLADE_TRIGGER      = 100861, ///< Cast AT 198782
    NPC_CLOUD_OF_HYPNOSIS           = 100994,
    NPC_STINGING_SWARM              = 101008
};

enum eGameObjects
{
    EntranceDoorLeft            = 252327,
    EntranceDoorRight           = 252328,
    AmalgamWall_1               = 247403,
    AmalgamWall_2               = 247404,
    AmalgamWall_3               = 247405,
    AmalgamWall_4               = 247406,
    AmalgamExitDoor             = 247407,
    AmbushEventDoorLeft         = 245079,
    AmbushEventDoorRight        = 245080,
    IllysannaDoor1              = 247401,
    IllysannaDoor2              = 247399,
    IllysannaDoor3              = 247400,
    IllysannaDoor4              = 245079,
    SmashspiteDoor              = 247402,
    SmashspiteToKurtalosDoor1   = 245104,
    SmashspiteToKurtalosDoor2   = 245105,
    KurtalosDoor                = 247498,
    UnkDoor                     = 253921
};

#endif ///< BLACK_ROOK_HOLD_DUNGEON_HPP
