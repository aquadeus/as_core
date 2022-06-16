////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

enum eCreatures
{
    Npc_GroveMarksman = 118243,
    Npc_DreamWeaver = 118445,
    Npc_Crushfist = 117838,
    Npc_FelCoreTargetStalker = 118463,
    Npc_FelCore = 118461,
    Npc_FelCannon = 118630,
    Npc_Hippogryph = 118687,
    Npc_DemonShipBomber = 118247
};

enum eSpells
{
    SummonHippogryphTaxi = 235579,
    IntoTheFray = 235105
};

enum eGameobjects
{
    Gob_InvisibleCollision = 268519,
};

enum eData
{
    Data_Defenders_Move,
    Data_Summon_Crushfist,
    Data_Destroy_Ship,
    Data_New_Hippogryphs
};

enum eScenarioSteps
{
    Step_SpeakWithCenarius,
    Step_SecureAFoothold,
    Step_OpenTheWay,
    Step_MovingOnUp,
    Step_LordOfTheTower,
    Step_ClearTheSkies,
    Step_UpAndAway,
    Step_DefeatTheLegionCommander,
    Step_Escape
};

enum eScenarioAssets
{
    Asset_MovingOnUp = 56399,
    Asset_UpAndAway = 56400,
    Asset_Escape = 56408
};

enum eSafeLocs
{
    Loc_Cenarius = 5885,
    Loc_Climb = 5887,
    Loc_Tower = 5888
};

enum ePoints
{
    Point_Defenders_Path,
    Point_Defenders_Last,
    Point_Hippogryph_Tower,
    Point_Hippogryph_Ship,
    Point_Hippogryph_Despawn,
    Point_Hippogryph_Escape
};

enum eConversations
{
    Conv_DreadVizerDefeated = 4372
};

namespace Positions
{
    const Position CrushfistSummonPos = { 3235.012f, 7830.527f, 36.26306f, 2.008533f };
    const Position CrushfistJumpPos = { 3221.04f, 7860.38f, 0.3935574f };
    const Position GroveMarksmanPositionFirst = { 3223.46f, 7847.74f, 0.4463351f };
    const Position GroveMarksmanPositionSecond = { 3227.66f, 7857.55f, 0.3744071f };
    const Position DreamWeaverPositionFirst = { 3234.69f, 7855.42f, 1.317537f };
    const Position DreamWeaverPositionSecond = { 3230.87f, 7849.44f, 1.392551f };
    const Position DreamWeaverPositionThird = { 3228.39f, 7851.47f, 0.829319f };
    const Position DreamWeaverPositionFourth = { 3227.04f,  7843.87f, 1.194309f };
    const Position HippogryphSpawnPosFirst = { 3364.833f, 7613.057f, 206.6074f, 1.313745f };
    const Position HippogryphSpawnPosSecond = { 3376.54f, 7609.119f, 203.9937f, 1.575494f };
    const Position HippogryphSpawnPosThird = { 3388.696f, 7606.863f, 202.766f, 1.38371f };
    const Position ScenarioEndTeleportPos = { 2285.82f, 6597.9f, 137.54f, 4.113392f };
};

namespace Waypoints
{
    G3D::Vector3 const Path_GroveMarksmanFirst[7] =
    {
        { 3100.836f, 7790.67f, 3.719452f },
        { 3122.05f, 7831.807f, 4.038078f },
        { 3133.514f, 7846.063f, 5.248989f },
        { 3149.696f, 7857.458f, 2.092006f },
        { 3174.858f, 7869.736f, 0.3872141f },
        { 3186.35f, 7875.961f, 0.3486308f },
        { 3197.759f, 7881.754f, 0.3136601f }
    };

    G3D::Vector3 const Path_GroveMarksmanSecond[7] =
    {
        { 3093.112f, 7791.174f, 2.529135f },
        { 3104.148f, 7801.865f, 3.018913f },
        { 3114.976f, 7822.897f, 4.32299f },
        { 3135.425f, 7846.149f, 5.004848f },
        { 3161.425f, 7860.924f, 0.2538452f },
        { 3171.52f, 7863.475f, 0.3833102f },
        { 3183.898f, 7866.101f, 0.430318f }
    };

    G3D::Vector3 const Path_DreamWeaverFirst[8] =
    {
        { 3094.915f, 7794.898f, 2.545126f },
        { 3102.273f, 7804.531f, 2.793938f },
        { 3106.8f, 7814.446f, 3.544914f },
        { 3121.747f, 7834.432f, 3.541218f },
        { 3136.231f, 7848.186f, 4.489711f },
        { 3159.088f, 7860.185f, 0.2538452f },
        { 3175.15f, 7865.51f, 0.4239739f },
        { 3191.078f, 7873.598f, 0.3751439f }
    };

    G3D::Vector3 const Path_DreamWeaverSecond[8] =
    {
        { 3101.753f, 7794.244f, 3.457733f },
        { 3109.77f, 7810.363f, 4.708733f },
        { 3116.742f, 7823.43f, 4.531975f },
        { 3126.363f, 7838.648f, 5.021687f },
        { 3141.231f, 7851.041f, 4.019252f },
        { 3163.016f, 7857.964f, 0.2538452f },
        { 3180.326f, 7861.975f, 0.4611686f },
        { 3197.253f, 7864.626f, 0.4604514f }
    };

    G3D::Vector3 const Path_DreamWeaverThird[9] =
    {
        { 3105.676f, 7792.787f, 4.221893f },
        { 3112.6f, 7803.0f, 5.232658f },
        { 3118.218f, 7816.542f, 6.005119f },
        { 3126.234f, 7828.618f, 5.047722f },
        { 3134.748f, 7839.238f, 6.248134f },
        { 3154.009f, 7849.511f, 2.100673f },
        { 3165.325f, 7851.492f, 0.2410874f },
        { 3184.086f, 7849.918f, 0.5698649f },
        { 3190.254f, 7849.438f, 0.5216834f }
    };

    G3D::Vector3 const Path_DreamWeaverFourth[8] =
    {
        { 3102.843f, 7770.355f, 3.637421f },
        { 3102.272f, 7782.382f, 3.80014f },
        { 3108.562f, 7798.627f, 4.21347f },
        { 3113.69f, 7811.648f, 5.539055f },
        { 3135.699f, 7842.849f, 5.718349f },
        { 3147.527f, 7847.917f, 3.050258f },
        { 3177.037f, 7855.998f, 0.50832f },
        { 3185.286f, 7857.51f, 0.5060769f }
    };

    G3D::Vector3 const Path_HippogryphFirst[9] =
    {
        { 3362.693f, 7604.916f, 201.2094f },
        { 3364.833f, 7613.057f, 206.6074f },
        { 3366.736f, 7620.296f, 211.407f },
        { 3361.202f, 7635.251f, 211.407f },
        { 3360.286f, 7669.736f, 190.8355f },
        { 3389.103f, 7753.826f, 153.6406f },
        { 3404.615f, 7775.798f, 146.371f },
        { 3430.669f, 7780.027f, 137.7038f },
        { 3449.226f, 7773.755f, 122.6014f }
    };

    G3D::Vector3 const Path_HippogryphSecond[9] =
    {
        { 3376.589f, 7598.776f, 196.5804f },
        { 3376.54f, 7609.119f, 203.9937f },
        { 3376.491f, 7619.462f, 211.407f },
        { 3377.761f, 7633.351f, 211.407f },
        { 3403.133f, 7718.964f, 190.8355f },
        { 3398.148f, 7746.917f, 153.6406f },
        { 3413.621f, 7756.961f, 146.371f },
        { 3426.732f, 7757.316f, 137.7038f },
        { 3443.419f, 7755.861f, 122.1646f }
    };

    G3D::Vector3 const Path_HippogryphThird[9] =
    {
        { 3386.521f, 7595.372f, 194.1251f },
        { 3388.696f, 7606.863f, 202.766f },
        { 3390.872f, 7618.354f, 211.407f },
        { 3390.952f, 7628.728f, 211.407f },
        { 3389.677f, 7689.155f, 190.8355f },
        { 3392.233f, 7744.318f, 153.6406f },
        { 3406.851f, 7764.515f, 146.371f },
        { 3431.606f, 7769.718f, 137.7038f },
        { 3442.005f, 7766.733f, 122.797f }
    };

    G3D::Vector3 const Path_HippogryphShip[9] =
    {
        { 3357.536f, 7764.849f, 140.9344f },
        { 3305.741f, 7756.821f, 166.0843f },
        { 3235.492f, 7738.116f, 206.4967f },
        { 3156.643f, 7690.199f, 259.6553f },
        { 3055.86f, 7676.297f, 294.4569f },
        { 2975.287f, 7680.299f, 317.4323f },
        { 2916.625f, 7695.105f, 317.4323f },
        { 2903.954f, 7718.213f, 309.0742f },
        { 2908.245f, 7730.87f, 299.1319f }
    };

    G3D::Vector3 const Path_HippogryphDespawn[3] =
    {
        { 2893.006f, 7803.339f, 309.4541f },
        { 2868.03f, 7848.546f, 309.4541f },
        { 2788.879f, 7924.161f, 309.4541f }
    };

    G3D::Vector3 const Path_HippogryphEscape[9] =
    {
        { 3204.545f, 7770.829f, 207.2216f },
        { 3282.479f, 7768.247f, 181.7589f },
        { 3348.537f, 7771.381f, 161.7913f },
        { 3390.595f, 7798.984f, 150.7244f },
        { 3456.568f, 7808.816f, 147.2162f },
        { 3483.464f, 7805.485f, 145.2832f },
        { 3499.607f, 7790.108f, 145.2832f },
        { 3503.802f, 7777.674f, 140.255f },
        { 3496.849f, 7749.519f, 122.7537f }
    };
};