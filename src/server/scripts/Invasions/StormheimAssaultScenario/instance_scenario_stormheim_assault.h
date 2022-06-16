////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

enum eCreatures
{
    Npc_Odyn = 119092,
    Npc_Aleifir_Freed = 120380,
    Npc_Hrafsir_Freed = 120381,
    Npc_Erilar_Freed = 120382,
    Npc_Aleifir_Cycle = 119196,
    Npc_Hrafsir_Cycle = 119198,
    Npc_Erilar_Cycle = 119197,
    Npc_Aleifir_GetOut = 118858,
    Npc_Hrafsir_GetOut = 119225,
    Npc_Erilar_GetOut = 119224,
    Npc_FelCannonPortal = 118852
};

enum eSpells
{
    Spell_AlexiusSpawnConversation = 236451,
    Spell_OdynFreeDrakesConversation = 236425
};

enum eActions
{
    Action_Path_To_Ship,
};

enum ePoints
{
    Point_Odyn_Path,
    Point_Dragons_Freedom,
    Point_Dragons_Cycle,
    Point_Dragons_Ship,
    Point_Get_Out
};

enum eScenarioSteps
{
    Step_Freedom,
    Step_SiegeBreaker,
    Step_Insertion,
    Step_TakenDownFromWithin,
    Step_CommanderTakedown,
    Step_GetOutOfThere
};

enum eScenarioAssets
{
    Asset_Freedom = 56603,
    Asset_SiegeBreaker = 56622,
    Asset_Insertion = 56604,
    Asset_TakenDownFromWithinFirst = 56826,
    Asset_GetOutOfThere = 56607
};

enum eSafeLocs
{
    Loc_Ground = 5902,
    Loc_Ship = 5911
};

namespace Positions
{
    const Position AleifirStartPos = { 2523.247f, 970.493f, 233.3569f, 0.6170427f };
    const Position AleifirHackPos = { 2629.617f, 976.0295f, 289.0678f };
    const Position AleifirGetOutPos = { 2762.408f, 1276.84f, 280.7652f, 0.413640f };
    const Position HrafsirStartPos = { 2512.031f, 982.0434f, 233.3579f, 0.6170427f };
    const Position HrafsirHackPos = { 2573.395f, 1028.639f, 269.4672f };
    const Position HrafsirGetOutPos = { 2791.276f, 1251.598f, 274.501f, 0.8193635f };
    const Position ErilarStartPos = { 2522.635f, 980.507f, 233.3822f, 0.6170427f };
    const Position ErilarHackPos = { 2573.395f, 1028.639f, 269.4672f };
    const Position ErilarGetOutPos = { 2817.892f, 1228.099f, 276.9924f, 1.103599f };
    const Position FelCannonPortalFirst = { 2884.865f, 1347.028f, 341.462f, 3.81611f };
    const Position FelCannonPortalSecond = { 2807.112f, 1389.672f, 314.8505f, 5.545615f };
    const Position LeaveScenario = { 2407.28f, 865.77f, 253.18f, 3.914424f };
}

namespace Waypoints
{
    G3D::Vector3 const Path_Odyn[9] =
    {
        { 2470.967f, 929.381f, 242.7088f },
        { 2472.967f, 931.381f, 241.9588f },
        { 2475.217f, 933.631f, 241.9588f },
        { 2475.967f, 934.381f, 241.9588f },
        { 2478.717f, 937.131f, 241.9588f },
        { 2484.217f, 942.631f, 241.9588f },
        { 2485.967f, 944.631f, 241.9588f },
        { 2488.967f, 947.381f, 241.9588f },
        { 2497.2f, 955.696f, 241.4482f }
    };

    G3D::Vector3 const Path_Dragons_Freedom[4] =
    {
        { 2631.54f, 1089.96f, 275.084f },
        { 2596.405f, 1052.797f, 328.8401f },
        { 2523.214f, 981.309f, 397.9814f },
        { 2449.948f, 911.8768f, 443.7812f }
    };

    G3D::Vector3 const Path_Aleifir_Cycle[10] =
    {
        { 2629.617f, 976.0295f, 289.0678f },
        { 2691.258f, 984.4583f, 289.0678f },
        { 2697.773f, 1032.571f, 281.2401f },
        { 2685.452f, 1092.688f, 283.2588f },
        { 2645.461f, 1137.097f, 283.4252f },
        { 2539.284f, 1153.389f, 282.2408f },
        { 2513.365f, 1122.083f, 285.7276f },
        { 2555.01f, 1045.903f, 289.3794f },
        { 2591.434f, 1028.648f, 289.0678f },
        { 2629.617f, 976.0295f, 289.0678f }
    };

    G3D::Vector3 const Path_Aleifir_Get_Out[2] =
    {
        { 2824.649f, 1305.286f, 293.6679f },
        { 2841.979f, 1323.804f, 280.4925f }
    };

    G3D::Vector3 const Path_Erilar_Cycle[10] =
    {
        { 2573.395f, 1028.639f, 269.4672f },
        { 2536.882f, 1056.328f, 269.4672f },
        { 2463.295f, 1097.385f, 269.4672f },
        { 2468.139f, 1138.646f, 269.4672f },
        { 2534.213f, 1150.852f, 269.4672f },
        { 2623.391f, 1083.882f, 269.4672f },
        { 2688.979f, 983.3646f, 269.4672f },
        { 2648.494f, 953.7552f, 269.4672f },
        { 2588.472f, 998.4063f, 269.4672f },
        { 2573.395f, 1028.639f, 269.4672f },
    };

    G3D::Vector3 const Path_Erilar_Get_Out[3] =
    {
        { 2819.953f, 1232.177f, 277.8422f },
        { 2847.416f, 1286.618f, 293.892f },
        { 2856.321f, 1304.127f, 280.4925f }
    };

    G3D::Vector3 const Path_Hrafsir_Get_Out[3] =
    {
        { 2813.767f, 1275.724f, 275.5526f },
        { 2833.127f, 1296.976f, 290.4244f },
        { 2851.309f, 1313.405f, 280.4925f }
    };

    G3D::Vector3 const Path_To_Ship[5] =
    {
        { 2636.15f, 1093.193f, 259.3594f },
        { 2650.349f, 1178.13f, 326.3719f },
        { 2672.02f, 1254.651f, 348.0051f },
        { 2726.55f, 1382.524f, 367.59f },
        { 2799.903f, 1399.524f, 352.457f }
    };
}