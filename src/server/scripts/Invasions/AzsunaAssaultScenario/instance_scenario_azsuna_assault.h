////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

enum eCreatures
{
    Npc_DemonShipBomber = 118247,
    Npc_FelFireEjector = 119648,
    Npc_AzureWarDrake = 119456,
    Npc_FelweaverAxtris = 119453,
    Npc_FelImp = 119633,
    Npc_AzureWarDrakeShip = 119459,
    Npc_Xeritas = 118975
};

enum eSpells
{
    FelFireEjection = 237671,
    FarondisAssistBlueDrakesConversation = 237750,
    XeritasOrdersFelweaverConversation = 237756
};

enum eData
{
    Data_First_Ejector,
    Data_Second_Ejector,
    Data_Third_Ejector,
    Data_Fourth_Ejector,
    Data_LeftSide_FirstPath,
    Data_LeftSide_SecondPath,
    Data_RightSide_FirstPath,
    Data_RightSide_SecondPath,
    Data_Command_Ship_Guid
};

enum eScenarioSteps
{
    Step_AssistTheBlueDragons,
    Step_MeetUpWithTheBlueDrakes,
    Step_ReachTheLegionShip,
    Step_GetOnUpThere,
    Step_TakeDownTheLeader,
    Step_Escape
};

enum eScenarioAssets
{
    Asset_AssistTheBlueDragons_Demons = 56887,
    Asset_MeetUpWithTheBlueDrakes = 56891,
    Asset_ReachTheLegionShip = 56892,
    Asset_Escape = 56885
};

enum eSafeLocs
{
    Loc_Farondis = 5908,
    Loc_Ship = 5918
};

enum ePoints
{
    Point_AzureWarDrake_Land,
    Point_AzureWarDrake_Ship,
    Point_AzureWarDrake_Despawn,
    Point_Imp_Path,
    Point_AzureWarDrake_Escape
};

namespace Positions
{
    const Position AzureWarDrakeLandFirst = { 964.6303f, 6278.35f, 209.5601f, 6.260363f };
    const Position AzureWarDrakeLandSecond = { 975.6136f, 6316.91f, 200.5657f, 0.2882064f };
    const Position AzureWarDrakeLandThird = { 978.9957f, 6299.73f, 180.0664f, 0.002312131f };
    const Position ImpSpawnPos = { 1466.367310f, 6076.260254f, 479.049713f };
    const Position AzureWarDrakeShipFirst = { 1278.827f, 6258.828f, 497.8597f, 5.490951f };
    const Position AzureWarDrakeShipSecond = { 1291.563f, 6296.076f, 498.5954f, 6.20336f };
    const Position AzureWarDrakeShipThird = { 1285.107f, 6278.604f, 499.6979f, 5.857538f };
    const Position ScenarioEndTeleportPos = { 6.7f, 6734.53f, 55.59f, 3.114191f };


    Position const ThreePieceEjectorsFirst[6] =
    {
        { 1443.538f, 6173.605f, 470.3494f, 3.141593f },
        { 1443.903f, 6173.622f, 467.8186f, 3.141593f },
        { 1444.208f, 6173.629f, 465.0164f, 3.141593f },
        { 1473.177f, 6173.618f, 467.8186f },
        { 1473.542f, 6173.62f, 470.3494f },
        { 1472.87f, 6173.618f, 465.0164f }
    };

    Position const ThreePieceEjectorsSecond[6] =
    {
        { 1445.198f, 6161.491f, 472.6474f, 3.141593f },
        { 1446.385f, 6161.502f, 467.5307f, 3.141593f },
        { 1445.901f, 6161.491f, 470.136f, 3.141593f },
        { 1473.566f, 6161.491f, 472.7534f },
        { 1472.63f, 6161.491f, 467.4185f },
        { 1472.938f, 6161.491f, 469.9369f }
    };

    Position const TwoPieceEjectors[4] =
    {
        { 1447.892f, 6149.336f, 469.321f, 3.141593f },
        { 1447.153f, 6149.324f, 471.9263f, 3.141593f },
        { 1473.875f, 6149.324f, 471.7272f },
        { 1473.231f, 6149.324f, 469.2088f }
    };

    Position const OnePieceEjectors[2] =
    {
        { 1449.408f, 6133.583f, 471.7764f, 3.141593f },
        { 1473.948f, 6136.239f, 472.1038f }
    };
}

namespace Waypoints
{
    G3D::Vector3 const Path_AzureWarDrakeToLandFirst[3] =
    {
        { 999.0417f, 6284.665f, 162.0956f },
        { 1041.425f, 6288.229f, 146.8118f },
        { 1067.561f, 6299.118f, 118.735405f }
    };

    G3D::Vector3 const Path_AzureWarDrakeToLandSecond[3] =
    {
        { 1002.646f, 6325.616f, 162.0956f },
        { 1033.229f, 6334.992f, 146.8118f },
        { 1065.337f, 6314.511f, 118.982262f }
    };

    G3D::Vector3 const Path_AzureWarDrakeToLandThird[3] =
    {
        { 992.3785f, 6299.991f, 162.0956f },
        { 1021.007f, 6302.673f, 143.0178f },
        { 1063.276f, 6306.948f, 118.78762f }
    };

    G3D::Vector3 const Path_AzureWarDrakeToShip[8] =
    {
        { 1089.054f, 6304.057f, 155.4448f },
        { 1120.014f, 6291.233f, 182.2696f },
        { 1169.0f, 6272.082f, 244.9067f },
        { 1226.868f, 6256.015f, 310.4449f },
        { 1281.651f, 6234.292f, 380.3205f },
        { 1322.512f, 6220.962f, 440.0138f },
        { 1369.453f, 6197.499f, 476.7541f },    
        { 1425.108f, 6191.265f, 461.0651f }
    };

    G3D::Vector3 const Path_AzureWarDrakeDespawn[3] =
    {
        { 1411.845f, 6192.03f, 488.6546f },
        { 1254.819f, 6062.268f, 488.6546f },
        { 1039.255f, 5982.916f, 488.6546f }
    };

    G3D::Vector3 const Path_ImpLeftFirst[19] =
    {
        { 1452.531f, 6084.408f, 477.8622f },
        { 1451.028f, 6085.251f, 478.5738f },
        { 1449.526f, 6086.094f, 479.2855f },
        { 1446.047f, 6086.813f, 479.8918f },
        { 1444.925f, 6103.7f, 476.5282f },
        { 1444.87f, 6104.056f, 476.4597f },
        { 1444.816f, 6104.413f, 476.3911f },
        { 1444.552f, 6106.128f, 476.0184f },
        { 1444.183f, 6108.526f, 475.5165f },
        { 1443.865f, 6110.597f, 475.1889f },
        { 1443.662f, 6112.033f, 474.9503f },
        { 1442.854f, 6117.749f, 473.7097f },
        { 1442.349f, 6121.328f, 472.9177f },
        { 1441.654f, 6126.244f, 471.8498f },
        { 1440.652f, 6133.334f, 470.4068f },
        { 1440.585f, 6133.811f, 470.2331f },
        { 1439.965f, 6144.132f, 467.939f },
        { 1438.601f, 6165.333f, 463.4767f },
        { 1437.165f, 6177.096f, 460.5928f }
    };

    G3D::Vector3 const Path_ImpLeftSecond[16] =
    {
        { 1452.531f, 6084.408f, 477.8622f },
        { 1451.028f, 6085.251f, 478.5738f },
        { 1449.526f, 6086.094f, 479.2855f },
        { 1446.047f, 6086.813f, 479.8918f },
        { 1445.367f, 6087.233f, 480.5512f },
        { 1441.644f, 6092.912f, 478.0866f },
        { 1441.301f, 6095.418f, 477.4283f },
        { 1440.776f, 6099.25f, 476.5298f },
        { 1440.54f, 6100.973f, 476.4137f },
        { 1439.967f, 6105.149f, 476.3306f },
        { 1439.176f, 6110.926f, 475.1108f },
        { 1438.85f, 6113.305f, 474.5695f },
        { 1435.401f, 6136.266f, 469.3616f },
        { 1433.873f, 6146.18f, 467.1481f },
        { 1431.873f, 6166.427f, 462.6897f },
        { 1430.243f, 6179.041f, 458.6706f }
    };

    G3D::Vector3 const Path_ImpRightFirst[17] =
    {
        { 1473.748f, 6086.111f, 478.2126f },
        { 1476.093f, 6089.248f, 478.3135f },
        { 1478.438f, 6092.385f, 478.4144f },
        { 1480.405f, 6092.934f, 478.1887f },
        { 1482.698f, 6094.945f, 478.7163f },
        { 1482.589f, 6097.106f, 478.3383f },
        { 1482.522f, 6098.457f, 478.1213f },
        { 1482.408f, 6100.72f, 477.3648f },
        { 1482.325f, 6102.364f, 477.2574f },
        { 1482.187f, 6105.123f, 476.6794f },
        { 1482.0f, 6108.836f, 476.071f },
        { 1481.56f, 6117.604f, 474.3496f },
        { 1481.358f, 6121.631f, 473.5773f },
        { 1481.34f, 6121.984f, 473.5389f },
        { 1479.943f, 6141.531f, 469.3011f },
        { 1477.903f, 6173.522f, 462.5398f },
        { 1477.7f, 6185.033f, 460.4742f }
    };

    G3D::Vector3 const Path_ImpRightSecond[13] =
    {
        { 1488.647f, 6101.346f, 477.9006f },
        { 1488.176f, 6111.779f, 475.7017f },
        { 1487.704f, 6122.211f, 473.5027f },
        { 1487.639f, 6123.656f, 473.1888f },
        { 1487.565f, 6125.3f, 472.6963f },
        { 1487.442f, 6127.662f, 472.1907f },
        { 1487.336f, 6129.687f, 471.97f },
        { 1487.146f, 6133.334f, 470.8954f },
        { 1486.949f, 6137.132f, 470.0256f },
        { 1486.774f, 6140.482f, 469.3665f },
        { 1486.568f, 6144.446f, 468.4471f },
        { 1484.839f, 6178.798f, 460.3726f },
        { 1483.859f, 6193.895f, 456.978f }
    };

    G3D::Vector3 const Path_AzureWarDrakeEscapeFirst[5] =
    {
        { 1254.132f, 6294.712f, 498.3879f },
        { 1269.292f, 6274.369f, 498.1053f },
        { 1284.451f, 6254.026f, 497.8227f },
        { 1342.106f, 6220.199f, 497.8227f },
        { 1448.457f, 6187.608f, 479.5695f }
    };

    G3D::Vector3 const Path_AzureWarDrakeEscapeSecond[5] =
    {
        { 1244.361f, 6296.397f, 504.1082f },
        { 1274.017f, 6294.514f, 500.9655f },
        { 1303.674f, 6292.631f, 497.8227f },
        { 1368.818f, 6252.327f, 497.8227f },
        { 1452.434f, 6207.552f, 478.7211f }
    };

    G3D::Vector3 const Path_AzureWarDrakeEscapeThird[5] =
    {
        { 1218.066f, 6310.312f, 504.4163f },
        { 1266.392f, 6287.213f, 501.1195f },
        { 1314.719f, 6264.114f, 497.8227f },
        { 1361.227f, 6234.522f, 497.8227f },
        { 1445.686f, 6198.742f, 478.6139f }
    };
};