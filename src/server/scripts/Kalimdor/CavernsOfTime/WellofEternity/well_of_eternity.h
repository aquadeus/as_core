////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEF_WELLOFETERNITY_H
#define DEF_WELLOFETERNITY_H

#include "KalimdorPCH.h"

#define WoEScriptName "instance_well_of_eternity"

enum Data
{
    DATA_PEROTHARN          = 0,
    DATA_AZSHARA            = 1,
    DATA_MANNOROTH          = 2,
    DATA_EVENT_NOZDORMU     = 3,
    DATA_EVENT_DEMON        = 4,
    DATA_EVENT_ILLIDAN_1    = 5,
    DATA_ROYAL_CACHE        = 6,
    DATA_EVENT_ILLIDAN_2    = 7,
    DATA_VAROTHEN           = 8,
    DATA_MINOR_CACHE        = 9
};


enum CreatureIds
{
    NPC_PEROTHARN   = 55085,
    NPC_ILLIDAN_1   = 55500,
    NPC_AZSHARA     = 54853,
    NPC_TYRANDE     = 55524,
    NPC_ILLIDAN_2   = 55532,
    NPC_MALFURION   = 55570,
    NPC_VAROTHEN    = 55419,
    NPC_ABYSSAL     = 55510,
    NPC_DOOMGUARD   = 55519,
    NPC_MANNOROTH   = 54969
};

enum GameObjectIds
{
    GO_COURTYARD_DOOR_1         = 210084,
    GO_LARGE_FIREWALL_DOOR      = 210234,
    GO_SMALL_FIREWALL_DOOR      = 210130,
    GO_INVISIBLE_FIREWALL_DOOR  = 210097,

    GO_ROYAL_CACHE              = 210025,
    GO_MINOR_CACHE              = 209541
};

enum QuestIds
{
    QUEST_DOCUMENTING_THE_TIMEWAYS      = 30104,
    QUEST_IN_UNENDING_NUMBERS           = 30099,
    QUEST_THE_VAINGLORIOUS              = 30100,
    QUEST_THE_PATH_TO_THE_DRAGON_SOUL   = 30101
};

enum QuestSpells
{
    SPELL_ARCHIVED_DEMON_1      = 109265,
    SPELL_ARCHIVED_DEMON_2      = 109266,
    SPELL_ARCHIVED_HANDMAIDEN_1 = 109270,
    SPELL_ARCHIVED_HANDMAIDEN_2 = 109271,
    SPELL_ARCHIVED_VAROTHEN_1   = 109273,
    SPELL_ARCHIVED_VAROTHEN_2   = 109274
};

#endif
