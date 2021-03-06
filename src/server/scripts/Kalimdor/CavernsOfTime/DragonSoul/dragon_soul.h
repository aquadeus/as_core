////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef DEF_DRAGONSOUL_H
#define DEF_DRAGONSOUL_H

#include "KalimdorPCH.h"

#define DSScriptName "instance_dragon_soul"

enum Datas
{
    DATA_MORCHOK                    = 0,
    DATA_YORSAHJ                    = 1,
    DATA_ZONOZZ                     = 2,
    DATA_HAGARA                     = 3,
    DATA_ULTRAXION                  = 4,
    DATA_BLACKHORN                  = 5,
    DATA_SPINE                      = 6,
    DATA_MADNESS                    = 7,
    DATA_DEATHWING                  = 8,
    DATA_KOHCROM                    = 10,
    DATA_HAGARA_EVENT               = 11,
    DATA_LESSER_CACHE_10N           = 12,
    DATA_LESSER_CACHE_25N           = 13,
    DATA_LESSER_CACHE_10H           = 14,
    DATA_LESSER_CACHE_25H           = 15,
    DATA_SWAYZE                     = 16,
    DATA_REEVS                      = 17,
    DATA_ALLIANCE_SHIP              = 18,
    DATA_BACK_PLATE_1               = 19,
    DATA_BACK_PLATE_2               = 20,
    DATA_BACK_PLATE_3               = 21,
    DATA_GREATER_CACHE_10N          = 22,
    DATA_GREATER_CACHE_25N          = 23,
    DATA_GREATER_CACHE_10H          = 24,
    DATA_GREATER_CACHE_25H          = 25,
    DATA_ALEXSTRASZA_DRAGON         = 26,
    DATA_NOZDORMU_DRAGON            = 27,
    DATA_YSERA_DRAGON               = 28,
    DATA_KALECGOS_DRAGON            = 29,
    DATA_ELEMENTIUM_FRAGMENT_10N    = 30,
    DATA_ELEMENTIUM_FRAGMENT_25N    = 31,
    DATA_ELEMENTIUM_FRAGMENT_10H    = 32,
    DATA_ELEMENTIUM_FRAGMENT_25H    = 33,
    DATA_ALL_HEROIC                 = 34,
};

enum CreatureIds
{
    NPC_MORCHOK                     = 55265,
    NPC_KOHCROM                     = 57773,
    NPC_YORSAHJ                     = 55312,
    NPC_ZONOZZ                      = 55308,
    NPC_HAGARA                      = 55689,
    NPC_ULTRAXION                   = 55294,
    NPC_BLACKHORN                   = 56427,
    NPC_GORIONA                     = 56781,
    NPC_SKY_CAPTAIN_SWAYZE          = 55870,
    NPC_KAANU_REEVS                 = 55891,
    NPC_SPINE_OF_DEATHWING          = 53879, // 109983 105003 109035 95278 105036
    NPC_DEATHWING                   = 56173, // at the eye

    NPC_TRAVEL_START                = 57684, // teleport at entrance
    NPC_NETHESTRASZ                 = 57287, // teleport upstairs
    NPC_EIENDORMI                   = 57288, // teleport to Yor'sahj
    NPC_VALEERA                     = 57289, // teleport to Zon'ozz
    NPC_TRAVEL_TO_WYRMREST_TEMPLE   = 57328, //
    NPC_TRAVEL_TO_WYRMREST_BASE     = 57882, //
    NPC_TRAVEL_TO_WYRMREST_SUMMIT   = 57379, //
    NPC_TRAVEL_TO_EYE_OF_ETERNITY   = 57377, // teleport to Hagara
    NPC_TRAVEL_TO_MAELSTORM         = 57443, //
    NPC_TRAVEL_TO_DECK              = 57378, //
    NPC_DASNURIMI                   = 58153, // trader
    NPC_YSERA_THE_AWAKENED          = 56665,
    NPC_ALEXTRASZA_THE_LIFE_BINDER  = 56630,
    NPC_KALECGOS                    = 56664,
    NPC_THRALL_1                    = 56667, // near summit
    NPC_NOZDORMU_THE_TIMELESS_ONE   = 56666,
    NPC_THE_DRAGON_SOUL             = 56668, // near summit

    NPC_THRALL_2                    = 56103, // after spine
    NPC_THE_DRAGON_SOUL_2           = 56694, // after spine

    NPC_ALEXSTRASZA_DRAGON          = 56099, // 1
    NPC_NOZDORMU_DRAGON             = 56102, // 2
    NPC_YSERA_DRAGON                = 56100, // 3
    NPC_KALECGOS_DRAGON             = 56101, // 4

    NPC_AGGRA                       = 58211, // after madness
    NPC_THRALL_3                    = 58232, // after madness
    NPC_KALECGOS_2                  = 58210, // after madness
    NPC_NOZDORMU_2                  = 58208, // after madness
    NPC_ALEXSTRASZA_2               = 58207, // after madness
    NPC_YSERA_2                     = 58209, // after madness
};

enum GameObjects
{
    GO_INNER_WALL                       = 209596,
    GO_THE_FOCUSING_IRIS                = 210132,
    GO_LESSER_CACHE_OF_THE_ASPECTS_LFR  = 210221,
    GO_LESSER_CACHE_OF_THE_ASPECTS_10N  = 210160,
    GO_LESSER_CACHE_OF_THE_ASPECTS_25N  = 210161,
    GO_LESSER_CACHE_OF_THE_ASPECTS_10H  = 210162,
    GO_LESSER_CACHE_OF_THE_ASPECTS_25H  = 210163,
    GO_ALLIANCE_SHIP                    = 210210,
    GO_DEATHWING_BACK_PLATE_1           = 209623,
    GO_DEATHWING_BACK_PLATE_2           = 209631,
    GO_DEATHWING_BACK_PLATE_3           = 209632,
    GO_GREATER_CACHE_OF_THE_ASPECTS_LFR = 210222,
    GO_GREATER_CACHE_OF_THE_ASPECTS_10N = 209894,
    GO_GREATER_CACHE_OF_THE_ASPECTS_25N = 209895,
    GO_GREATER_CACHE_OF_THE_ASPECTS_10H = 209896,
    GO_GREATER_CACHE_OF_THE_ASPECTS_25H = 209897,
    GO_ELEMENTIUM_FRAGMENT_LFR          = 210079,
    GO_ELEMENTIUM_FRAGMENT_10N          = 210217,
    GO_ELEMENTIUM_FRAGMENT_25N          = 210218,
    GO_ELEMENTIUM_FRAGMENT_10H          = 210219,
    GO_ELEMENTIUM_FRAGMENT_25H          = 210220,
};

enum SharedSpells
{
    SPELL_TELEPORT_VISUAL_ACTIVE                = 108203,
    SPELL_TELEPORT_VISUAL_DISABLED              = 108227,

    SPELL_CHARGING_UP_LIFE                      = 108490,
    SPELL_CHARGING_UP_MAGIC                     = 108491,
    SPELL_CHARGING_UP_EARTH                     = 108492,
    SPELL_CHARGING_UP_TIME                      = 108493,
    SPELL_CHARGING_UP_DREAMS                    = 108494,
    SPELL_WARD_OF_TIME                          = 108160,
    SPELL_WARD_OF_EARTH                         = 108161,
    SPELL_WARD_OF_MAGIC                         = 108162,
    SPELL_WARD_OF_LIFE                          = 108163,
    SPELL_WARD_OF_DREAMS                        = 108164,

    SPELL_TELEPORT_SINGLE_TO_DEATHWINGS_BACK    = 106054,
    SPELL_DRAGON_SOUL_PARATROOPER_KIT_1         = 104953, // Swayze has it while jumping to spine of deathwing
    SPELL_DRAGON_SOUL_PARATROOPER_KIT_2         = 105008, // Reevs has it while jumping to spine of deathwing

    SPELL_PARACHUTE                             = 110660, // used by players

    SPELL_PLAY_MOVIE_DEATHWING_2                = 106085, // movie before jumping at spine of deathwing
};

const Position teleportPos[6] =
{
    {-1779.503662f, -2393.439941f, 45.61f, 3.20f},   // Wyrmrest Temple/Base
    {-1854.233154f, -3068.658691f, -178.34f, 0.46f}, // Yor'sahj The Unsleeping
    {-1743.647827f, -1835.132568f, -220.51f, 4.53f}, // Warlord Zon'ozz
    {-1781.188477f, -2375.122559f, 341.35f, 4.43f},  // Wyrmrest Summit
    {13629.356445f, 13612.099609f, 123.49f, 3.14f},  // Hagara
    {-13854.668945f, -13666.967773f, 275.f, 1.60f},  // Spine
};

const Position ultraxionPos[2] =
{
    {-1564.f, -2369.f, 250.083f, 3.28122f}, // spawn
    {-1699.469971f, -2388.030029f, 355.192993f, 3.21552f} // move to
};

const Position skyfirePos = {13444.9f, -12133.3f, 151.21f, 0.0f};
const Position spinedeathwingPos = {-13852.5f, -13665.38f, 297.3786f, 1.53589f};

const Position madnessdeathwingPos = {-12081.390625f, 12160.050781f, 30.60f, 6.03f};

enum eEncounterIDs
{
    Morchok     = 1292,
    Zonozz      = 1294,
    Yorsahj     = 1295,
    Hagara      = 1296,
    Ultraxion   = 1297,
    Blackhorn   = 1298,
    SpineOfDW   = 1291,
    MadnessOfDW = 1299
};

#endif
