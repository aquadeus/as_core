////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ZONE_KEZAN
#define ZONE_KEZAN

enum eQuests
{
    RollingWithMyHomies = 14071,
    FourthAndGoal = 24503,
    LifeOfTheParty = 14113,
    RobbingHoods = 14121,
    LiberateTheKajamite = 14124,
    q447 = 14125,
    LifeSavings = 14126,
    NecessaryRoughness = 24502,
};

enum eCreatures
{
    Izzy = 34959,
    Gobber = 34958,
    Ace = 34957,
    SteamwheedleShark = 37114,
    FourthAndGoalTarget = 37203,
    BilgewaterBuccaneer = 37213,
    Gasbot = 37598
};

enum eGameobjects
{
    KajamiteChunk = 195492
};

enum eSpells
{
    SummonIzzy = 66600,
    SummonGobber = 66599,
    SummonAce = 66597,
    ResummonIzzy = 66646,
    ResummonGobber = 66645,
    ResummonAce = 66644,
    SummonSteamwheedleShark000 = 69971,
    SummonSteamwheedleShark001 = 69976,
    SummonSteamwheedleShark002 = 69977,
    SummonSteamwheedleShark003 = 69978,
    SummonSteamwheedleShark004 = 69979,
    SummonSteamwheedleShark005 = 69980,
    SummonSteamwheedleShark006 = 69981,
    SummonSteamwheedleShark007 = 69982,
    DespawnSummonedSteamwheedleSharks = 69987,
    ThrowFootbomb = 69993,
    FourthAndGoalTargetAndInvi = 70086,
    Bubbly = 66909,
    Bucket = 66910,
    Dance = 66911,
    Fireworks = 66912,
    HorsDoeuvres = 66913,
    HappyPartyGoer = 66916,
    NeedFireworks = 75048,
    NeedHorsDoeuvres = 75050,
    NeedBucket = 75044,
    NeedBubbly = 75042,
    NeedDance = 75046,
    GasbotSteam = 70256,
    GasbotExplosion = 70259,
    SeeQuestInv = 71313,
    QuestInv = 66404,
    SeeQuestInv15 = 81019
};

enum eObjectives
{
    PickIzzy = 265516,
    PickAce = 265517,
    PickGobber = 265518,
};

namespace Positions
{
    Position const g_SteamwheedleShark000 = { -8288.62f, 1479.97f, 43.8908f, 0.0f };
    Position const g_SteamwheedleShark001 = { -8273.75f, 1484.46f, 42.9395f, 0.0f };
    Position const g_SteamwheedleShark002 = { -8288.08f, 1487.72f, 43.8463f, 0.0f };
    Position const g_SteamwheedleShark003 = { -8281.04f, 1477.49f, 43.3046f, 0.0f };
    Position const g_SteamwheedleShark004 = { -8281.33f, 1490.41f, 43.4756f, 0.0f };
    Position const g_SteamwheedleShark005 = { -8295.1f, 1484.91f, 44.3231f, 0.0f };
    Position const g_SteamwheedleShark006 = { -8294.66f, 1474.68f, 44.2946f, 0.0f };
    Position const g_SteamwheedleShark007 = { -8294.61f, 1493.67f, 44.6239f, 0.0f };
    Position const g_DeathwingSpawnPos    = { -8178.59f, 1482.14f, 83.99894f, 0.0f };
    Position const g_FourthAndGoalTarget  = { -8385.20f, 1482.47f, 126.7303f, 0.01333477f };
    Position const g_FourthAndGoalBuccaneer  = { -8250.97f, 1484.26f, 41.45206f, 3.154927f };
    Position const g_RopeLadderJumpPos  = { -7861.19f, 1831.99f, 8.14545f };
};

namespace Waypoints
{
    G3D::Vector3 const Path_Deathwing[14] =
    {
        { -8307.88f, 1483.67f, 137.101f },
        { -8357.73f, 1482.93f, 150.912f },
        { -8544.15f, 1481.06f, 276.202f },
        { -8562.12f, 1483.93f, 283.795f },
        { -8644.45f, 1499.07f, 312.923f },
        { -8707.98f, 1559.36f, 312.923f },
        { -8689.69f, 1657.08f, 312.923f },
        { -8565.21f, 1675.97f, 285.023f },
        { -8311.69f, 1501.69f, 93.4831f },
        { -8250.97f, 1484.26f, 41.4215f },
        { -8262.02f, 1468.50f, 93.7929f },
        { -8140.40f, 1405.29f, 89.0123f },
        { -7904.96f, 1380.62f, 89.0123f },
        { -7736.61f, 1402.56f, 89.0123f }
    };

    G3D::Vector3 const Path_Gasbot[5] =
    {
        { -8424.281f, 1332.188f, 102.533f },
        { -8424.217f, 1341.339f, 102.5234f },
        { -8424.152f, 1348.864f, 105.0063f },
        { -8424.152f, 1354.114f, 105.0063f },
        { -8424.041f, 1363.018f, 104.9537f }
    };

};

#endif // ZONE_KEZAN
