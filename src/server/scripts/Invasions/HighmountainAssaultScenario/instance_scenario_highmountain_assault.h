////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

enum eCreatures
{
    Npc_MaylaHighmountain = 119850,
    Npc_StonedarkGeomancer = 119942,
    Npc_HighmountainProtector = 119854,
    Npc_FelstoneWrathguard = 120060,
    Npc_FelTouchedPyromage = 119860,
    Npc_FelChampionGargath = 119959,
    Npc_HellfireInfernal = 119986,
    Npc_WarEagle = 119857,
    Npc_PortalToFelCommander = 120081,
    Npc_Dynamite = 119981,
    Npc_Lasan = 119853,
    Npc_WarEagleEscape = 120048,
    Npc_StonefistStalker = 119957
};

enum eSpells
{
    SummonIntroWarEagle = 238554,
    MaylaAssistantRequestConversation = 239046,
    NatureChanneling = 71467,
    Disengage = 82707,
    CapturedLasanConversation = 239073,
    LasanRescuedConversation = 239116,
    CosmeticDrogbarFistBlocker = 238634,
    CosmeticNatureCast = 238636
};

enum eActions
{
    Action_Kneel,
    Action_Regroup
};

enum eDatas
{
    Data_CastFirstFist,
    Data_CastSecondFist
};

enum eScenarioSteps
{
    Step_SecuringThunderTotem,
    Step_BaseDefense,
    Step_Incoming,
    Step_LastDefense,
    Step_Regroup,
    Step_ToTheShip,
    Step_FreeOurPeople,
    Step_DestroyedFromWithin,
    Step_TheFinalFight,
    Step_TimeToGo
};

enum eScenarioAssets
{
    Asset_SecuringThunderForces = 57173,
    Asset_BaseDefense = 57177,
    Asset_Incoming = 57182,
    Asset_Regroup = 57189,
    Asset_ToTheShip = 57209,
    Asset_Leave = 57212
};

enum eSafeLocs
{
    Loc_ThunderTotem = 5945,
    Loc_Ship = 5946
};

enum ePoints
{
    Point_WarEagle_ThunderTotem,
    Point_WarEagle_Ship,
    Point_WarEagle_Despawn,
    Point_HighmountainProtector_Defense,
    Point_Invander_Attack,
    Point_Mayla_Escort_First,
    Point_Mayla_Escort_Second,
    Point_WarEagle_Escape
};

namespace Positions
{
    const Position FelChampionGargathSpawnPos = { 4082.547f, 4384.603f, 670.7087f, 1.222167f };
    const Position HellfireInfernalSpawnPos = { 4127.868f, 4476.01f, 660.0695f, 2.91202f };
    const Position LasanCapturedSpawnPos = { 4423.057f, 4532.654f, 900.1179f, 3.725328f };
    const Position WarEagleSpawnPosFirst = { 4491.458f, 4396.926f, 900.5198f, 3.285714f };
    const Position WarEagleSpawnPosSecond = { 4491.787f, 4405.869f, 900.5451f, 3.195891f };
    const Position WarEagleSpawnPosThird = { 4490.539f, 4416.165f, 900.6826f, 3.456574f };
    const Position ScenarioEndTeleportPosition = { 4269.96f, 4566.27f, 670.25f, 3.019943f };

    Position const DynamitesSpawnPos[5] =
    {
        { 4418.886f, 4561.333f, 941.9557f },
        { 4386.205f, 4511.163f, 960.3613f },
        { 4441.351f, 4546.917f, 960.3613f },
        { 4454.432f, 4480.877f, 932.4667f },
        { 4410.13f, 4494.503f, 941.978f }
    };

    Position const PortalsToFelCommanderSpawnPos[2] =
    {
        { 4414.168f, 4528.487f, 941.508f, 0.6591004f },
        { 4414.18f, 4528.268f, 960.8342f, 0.6591004f }
    };

    Position const HighmountainProtectorSpawnPos[3] =
    {
        { 4079.52f, 4393.143f, 670.7087f, 5.237347f },
        { 4086.459f, 4394.039f, 670.7088f, 4.14561f },
        { 4090.896f, 4388.961f, 670.7087f, 2.808861f }
    };

    Position const HighmountainProtectorDefensePos[3] =
    {
        { 4075.01f, 4401.33f, 670.8238f },
        { 4089.73f, 4401.88f, 670.8239f },
        { 4100.0f, 4390.782f, 670.7784f }
    };

    Position const DemonInvadersPos[3] =
    {
        { 4050.635f, 4466.22f, 663.1506f, 5.003976f },
        { 4116.917f, 4463.355f, 661.7588f, 3.939818f },
        { 4170.976f, 4399.163f, 661.2471f, 3.457138f }
    };
};

namespace Waypoints
{
    G3D::Vector3 const Path_WarEagleIntro_ThunderTotem[10] =
    {
        { 3922.359f, 4160.575f, 896.6343f },
        { 3933.854f, 4170.839f, 898.0194f },
        { 3976.184f, 4220.728f, 867.2411f },
        { 4118.405f, 4246.251f, 814.6242f },
        { 4149.024f, 4248.443f, 802.4409f },
        { 4178.413f, 4254.033f, 791.8299f },
        { 4199.786f, 4271.377f, 785.5535f },
        { 4204.709f, 4291.266f, 785.5535f },
        { 4166.076f, 4294.484f, 785.5535f },
        { 4114.442f, 4293.082f, 772.3085f }
    };

    G3D::Vector3 const Path_Invader_Left[23] =
    {
        { 4051.241f, 4464.926f, 663.4368f },
        { 4051.741f, 4463.176f, 662.9368f },
        { 4052.241f, 4461.426f, 663.4368f },
        { 4052.491f, 4460.176f, 663.4368f },
        { 4052.991f, 4458.176f, 663.6868f },
        { 4053.491f, 4456.426f, 664.1868f },
        { 4053.741f, 4454.676f, 664.6868f },
        { 4054.491f, 4452.176f, 665.4368f },
        { 4054.741f, 4451.176f, 665.6868f },
        { 4054.991f, 4449.676f, 666.1868f },
        { 4055.491f, 4447.676f, 665.9368f },
        { 4055.991f, 4445.926f, 665.6868f },
        { 4056.241f, 4444.426f, 665.6868f },
        { 4056.491f, 4443.426f, 665.6868f },
        { 4056.991f, 4441.676f, 665.6868f },
        { 4057.241f, 4440.426f, 665.9368f },
        { 4058.241f, 4436.926f, 665.9368f },
        { 4058.741f, 4434.926f, 666.6868f },
        { 4059.241f, 4432.426f, 666.9368f },
        { 4059.491f, 4431.426f, 667.4368f },
        { 4060.348f, 4428.133f, 667.723f },
        { 4065.221f, 4417.991f, 668.0219f },
        { 4072.335f, 4404.819f, 670.6066f }
    };

    G3D::Vector3 const Path_Invader_Middle[11] =
    {
        { 4116.015f, 4462.43f, 661.7349f },
        { 4115.445f, 4461.846f, 661.1959f },
        { 4114.246f, 4460.46f, 660.8978f },
        { 4112.557f, 4458.509f, 661.2396f },
        { 4112.019f, 4457.888f, 661.551f },
        { 4109.601f, 4455.168f, 662.1984f },
        { 4108.313f, 4453.719f, 662.3481f },
        { 4106.899f, 4452.129f, 662.3644f },
        { 4099.605f, 4433.425f, 666.8246f },
        { 4096.825f, 4421.254f, 667.8238f },
        { 4091.182f, 4405.746f, 670.497f }
    };

    G3D::Vector3 const Path_Invader_Right[13] =
    {
        { 4170.066f, 4398.865f, 661.4645f },
        { 4169.127f, 4398.559f, 661.719f },
        { 4166.143f, 4397.715f, 662.6759f },
        { 4165.063f, 4397.409f, 663.1137f },
        { 4164.027f, 4397.116f, 663.5308f },
        { 4162.971f, 4396.817f, 663.7231f },
        { 4160.685f, 4396.171f, 664.375f },
        { 4152.733f, 4395.347f, 666.1922f },
        { 4141.534f, 4397.168f, 666.8238f },
        { 4134.547f, 4398.463f, 667.4427f },
        { 4125.384f, 4399.508f, 667.8239f },
        { 4111.596f, 4394.499f, 668.66f },
        { 4104.788f, 4392.509f, 670.4778f }
    };

    G3D::Vector3 const Path_Mayla_Escort_First[11] =
    {
        { 4080.119f, 4399.093f, 670.8388f },
        { 4072.119f, 4406.093f, 670.0888f },
        { 4065.342f, 4419.599f, 667.9336f },
        { 4060.083f, 4430.799f, 667.2562f },
        { 4055.159f, 4448.432f, 665.7793f },
        { 4051.174f, 4464.837f, 663.0024f },
        { 4044.455f, 4485.361f, 662.0978f },
        { 4051.001f, 4487.891f, 662.1508f },
        { 4066.392f, 4479.938f, 661.4566f },
        { 4099.354f, 4477.323f, 660.2589f },
        { 4106.328f, 4479.612f, 660.124f }
    };

    G3D::Vector3 const Path_Mayla_Escort_Second[38] =
    {
        { 4112.035f, 4478.372f, 660.1273f },
        { 4113.785f, 4477.872f, 660.1273f },
        { 4116.535f, 4477.122f, 660.3773f },
        { 4117.785f, 4476.872f, 660.3773f },
        { 4119.535f, 4476.622f, 660.3773f },
        { 4120.535f, 4476.372f, 660.6273f },
        { 4126.285f, 4474.872f, 660.3773f },
        { 4130.741f, 4473.632f, 660.1307f },
        { 4132.588f, 4471.028f, 660.286f },
        { 4133.338f, 4469.778f, 660.286f },
        { 4134.088f, 4468.778f, 660.536f },
        { 4137.874f, 4463.593f, 660.319f },
        { 4139.667f, 4460.773f, 660.332f },
        { 4140.667f, 4458.773f, 660.332f },
        { 4141.667f, 4457.523f, 660.332f },
        { 4143.167f, 4455.273f, 660.332f },
        { 4145.417f, 4453.023f, 659.832f },
        { 4149.167f, 4449.523f, 659.832f },
        { 4150.667f, 4448.523f, 659.832f },
        { 4154.917f, 4444.273f, 659.832f },
        { 4159.167f, 4440.273f, 659.832f },
        { 4170.667f, 4429.273f, 659.832f },
        { 4179.567f, 4423.347f, 659.4438f },
        { 4181.653f, 4421.815f, 659.3502f },
        { 4186.069f, 4418.885f, 659.3695f },
        { 4188.574f, 4416.563f, 659.3141f },
        { 4189.741f, 4415.437f, 659.3885f },
        { 4192.991f, 4410.937f, 659.6385f },
        { 4193.752f, 4409.793f, 659.424f },
        { 4197.889f, 4409.471f, 659.9209f },
        { 4199.139f, 4409.971f, 660.1709f },
        { 4207.139f, 4413.471f, 660.9209f },
        { 4211.139f, 4415.221f, 661.1709f },
        { 4218.889f, 4418.221f, 661.9209f },
        { 4222.389f, 4419.221f, 662.1709f },
        { 4225.385f, 4420.795f, 663.031f },
        { 4241.813f, 4438.647f, 667.8007f },
        { 4242.848f, 4459.53f, 669.2999f }
    };

    G3D::Vector3 const Path_WarEagle_Ship[11] =
    {
        { 4265.792f, 4456.663f, 702.7202f },
        { 4286.357f, 4441.25f, 713.4445f },
        { 4359.306f, 4426.665f, 715.7656f },
        { 4390.147f, 4378.274f, 746.7672f },
        { 4371.213f, 4316.459f, 778.8858f },
        { 4291.314f, 4305.967f, 846.9174f },
        { 4296.057f, 4431.216f, 910.72f },
        { 4272.104f, 4496.248f, 953.3059f },
        { 4297.83f, 4572.617f, 968.333f },
        { 4356.038f, 4604.862f, 977.4727f },
        { 4374.779f, 4613.458f, 977.4727f }
    };

    G3D::Vector3 const Path_WarEagle_Despawn[4] =
    {
        { 4390.338f, 4618.4f, 983.2133f },
        { 4452.581f, 4645.959f, 983.2133f },
        { 4579.535f, 4575.143f, 983.2133f },
        { 4645.934f, 4543.08f, 983.2133f }
    };

    G3D::Vector3 const Path_WarEagle_Escape_First[7] =
    {
        { 4514.106f, 4400.042f, 892.2416f },
        { 4493.058f, 4397.152f, 899.8839f },
        { 4472.01f, 4394.261f, 907.5262f },
        { 4421.233f, 4389.379f, 907.5262f },
        { 4345.786f, 4410.749f, 907.5262f },
        { 4319.77f, 4488.799f, 907.5262f },
        { 4378.64f, 4525.336f, 904.0842f }
    };

    G3D::Vector3 const Path_WarEagle_Escape_Second[7] =
    {
        { 4511.001f, 4407.143f, 892.2954f },
        { 4493.102f, 4405.949f, 899.9108f },
        { 4475.202f, 4404.754f, 907.5262f },
        { 4415.834f, 4398.174f, 907.5262f },
        { 4350.777f, 4420.201f, 907.5262f },
        { 4334.642f, 4461.03f, 907.5262f },
        { 4382.129f, 4515.127f, 904.7526f }
    };

    G3D::Vector3 const Path_WarEagle_Escape_Third[7] =
    {
        { 4509.1f, 4421.287f, 892.6122f },
        { 4491.792f, 4416.539f, 900.0692f },
        { 4474.485f, 4411.792f, 907.5262f },
        { 4415.543f, 4406.229f, 907.5262f },
        { 4355.319f, 4428.016f, 907.5262f },
        { 4345.035f, 4453.374f, 907.5262f },
        { 4389.161f, 4506.124f, 905.0583f }
    };
};