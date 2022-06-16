////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEF_WAILING_CAVERNS_H
#define DEF_WAILING_CAVERNS_H

#include "KalimdorPCH.h"

enum eTypes
{
    TYPE_LORD_COBRAHN               = 1,
    TYPE_LORD_PYTHAS                = 2,
    TYPE_LADY_ANACONDRA             = 3,
    TYPE_LORD_SERPENTIS             = 4,
    TYPE_NARALEX_EVENT              = 5,
    TYPE_NARALEX_PART1              = 6,
    TYPE_NARALEX_PART2              = 7,
    TYPE_NARALEX_PART3              = 8,
    TYPE_MUTANUS_THE_DEVOURER       = 9,
    TYPE_NARALEX_YELLED             = 10,

    DATA_NARALEX                    = 3679
};

enum eWailingCavernsData
{
    Data_Lord_Cobrahn               = 1,
    Data_Lord_Pythas,
    Data_Lady_Anacondra,
    Data_Lord_Serpentis,
    Data_Kresh,
    Data_Skum,
    Data_Verdan,
    Data_Mutanus
};

enum eWailingCavernsScenario
{
    /// Scenario ID
    WailingCavernsScenarioID        = 515,

    /// Bosses
    WailingCavernsLordCobrahn       = 24770,
    WailingCavernsLadyAnacorndra    = 24764,
    WailingCavernsKresh             = 24772,
    WailingCavernLordPythas         = 24769,
    WailingCavernsSkum              = 24773,
    WailingCavernsLordSerpentis     = 24771,
    WailingCavernsVerdan            = 24787,
    WailingCavernsMutanus           = 34405
};

#endif
