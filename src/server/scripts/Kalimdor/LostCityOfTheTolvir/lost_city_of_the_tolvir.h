////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEF_LOST_CITY_OF_THE_TOLVIR_H
# define DEF_LOST_CITY_OF_THE_TOLVIR_H

#include"KalimdorPCH.h"
#include "AchievementMgr.h"

enum Data64
{
    DATA_GENERAL_HUSAM,
    DATA_LOCKMAW,
    DATA_AUGH,
    DATA_HIGH_PROPHET_BARIM,
    DATA_SIAMAT,
    MAX_ENCOUNTER,
    DATA_HARBINGER,
    DATA_BLAZE
};

enum Creatures
{
    // Dungeon Bosses
    BOSS_GENERAL_HUSAM           = 44577,
    BOSS_HIGH_PROPHET_BARIM      = 43612,
    BOSS_LOCKMAW                 = 43614,
    BOSS_AUGH                    = 49045,
    BOSS_SIAMAT                  = 44819,
    // Various NPCs
    NPC_WIND_TUNNEL              = 48092,
    NPC_WIND_TUNNEL_VISUAL       = 42895
};

enum GameObjects
{
    SIAMAT_PLATFORM              = 205365
};

enum eBoss
{
    DATA_BOSS_HUSAM,
    DATA_BOSS_BARIM,
    DATA_BOSS_LOCKMAW_AUGH,
    DATA_BOSS_SIAMAT,

    DATA_MAX_ENCOUNTER
};

#endif
