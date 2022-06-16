////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEF_BRD_H
#define DEF_BRD_H

#include "EasternKingdomsPCH.h"

enum eTypes
{
    TYPE_RING_OF_LAW        = 1,
    TYPE_VAULT              = 2,
    TYPE_BAR                = 3,
    TYPE_TOMB_OF_SEVEN      = 4,
    TYPE_LYCEUM             = 5,
    TYPE_IRON_HALL          = 6,

    DATA_EMPEROR            = 10,
    DATA_PHALANX            = 11,

    DATA_ARENA1             = 12,
    DATA_ARENA2             = 13,
    DATA_ARENA3             = 14,
    DATA_ARENA4             = 15,

    DATA_GO_BAR_KEG         = 16,
    DATA_GO_BAR_KEG_TRAP    = 17,
    DATA_GO_BAR_DOOR        = 18,
    DATA_GO_CHALICE         = 19,

    DATA_GHOSTKILL          = 20,
    DATA_EVENSTARTER        = 21,

    DATA_GOLEM_DOOR_N       = 22,
    DATA_GOLEM_DOOR_S       = 23,

    DATA_THRONE_DOOR        = 24,

    DATA_SF_BRAZIER_N       = 25,
    DATA_SF_BRAZIER_S       = 26,
    DATA_MOIRA_TYPE         = 27
};

enum eBossData
{
    DATA_GERSTAHN          = 0,
    DATA_ANUBSHIAH         = 1,
    DATA_GOROSH            = 2,
    DATA_GRIZZLE           = 3,
    DATA_ANGERFORGE        = 4,
    DATA_FLAMELASH         = 5,
    DATA_TOMB_OF_SEVEN     = 6,
    DATA_MAGMUS            = 7,
    DATA_MOIRA             = 8,
    DATA_DAGRAN_THAURISSAN = 9,

    DATA_MAX_ENCOUNTER     = 10
};

#endif
