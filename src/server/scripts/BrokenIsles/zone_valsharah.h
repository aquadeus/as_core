////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

# include "BrokenIslesPCH.h"

namespace ValsharahCreatures
{
    enum eCreatures
    {
        Npc_Ysera_Mount      = 100906,
        Npc_Tyrande_custom   = 100051,
        Npc_Tyrande_Starsong = 993559,
        Npc_Malfurion_Scene  = 100601,
        Mpc_Ysera_Grove      = 100602
    };
}

namespace ValsharahSpells
{
    enum eSpells
    {
        Spell_Xavius_Root    = 218397,
        Spell_Saber_Mount    = 8394,
        Spell_Mount_Creature = 59119
    };
}

namespace ValsharahQuests
{
    enum eQuests
    {
        Quest_Demons_Trail  = 38753,
        Quest_Emerald_Queen = 38377
    };
}

namespace ValsharahEvents
{
    enum eEvents
    {
        /// Ysera Event
        Event_Mount       = 4,
        Event_Flight      = 5,
        Event_Fly_Talk_1  = 6,
        Event_Fly_Talk_2  = 7,

        /// Tyrande Event
        Event_Tyrande_Wp  = 8,
        Event_Tyrande_Wp2 = 9,
        Event_Tyrande_Starsong_Ride = 10,

        /// Ysera Grove Events
        Event_Ysera_Talk  = 11,
        Event_Ysera_Talk2 = 12,
        Event_Ysera_Talk3 = 13,
        Event_Ysera_Die   = 14,

        /// Flight Hippogryph Events
        Event_Hippo_Mount = 15,
        Event_Hippo_Fly   = 16
    };
}

namespace ValsharahMoves
{
    enum eMoves
    {
        Move_Ysera        = 1,
        Move_Tyrande      = 2,
        Move_Tyrande2     = 3,
        Move_Tyrande_Starsong = 4,
        Move_Hippogryph   = 5
    };
}

namespace ValsharahWaypoints
{
    std::vector<G3D::Vector3> g_Path_Ysera =
    {
        { 2616.71f, 6731.68f, 149.107f },
        { 2530.58f, 6713.51f, 186.943f },
        { 2481.07f, 6696.39f, 185.147f },
        { 2451.65f, 6651.94f, 203.650f },
        { 2483.03f, 6546.38f, 195.587f },
        { 2617.63f, 6452.26f, 250.188f },
        { 2772.31f, 6392.37f, 241.662f },
        { 2773.49f, 6343.44f, 231.952f },
        { 2725.26f, 6311.49f, 230.084f },
        { 2708.77f, 6272.78f, 223.320f }
    };

    std::vector<G3D::Vector3> g_Path_Tyrande =
    {
        { 2931.74f, 5902.60f, 306.379f },
        { 2933.23f, 5880.59f, 305.708f }
    };

    std::vector<G3D::Vector3> g_Path_Tyrande_2 =
    {
        { 2906.08f, 5886.73f, 297.115f },
        { 2901.33f, 5887.97f, 297.115f }
    };

    std::vector<G3D::Vector3> g_Path_Tyrande_Starsong =
    {
        { 2904.19f, 5872.30f, 297.130f },
        { 2906.16f, 5867.16f, 299.003f },
        { 2912.39f, 5854.47f, 299.891f },
        { 2914.99f, 5850.62f, 302.624f },
        { 2957.10f, 5818.66f, 306.393f },
        { 3001.77f, 5813.14f, 309.357f },
        { 3035.44f, 5816.68f, 308.246f },
        { 3104.45f, 5817.68f, 299.761f },
        { 3128.43f, 5791.75f, 308.799f },
        { 3145.67f, 5774.15f, 308.262f }
    };

    std::vector<G3D::Vector3> g_Path_Valsharah_Intro =
    {
        { -843.811f, 4262.04f, 759.37f },
        { -840.685f, 4230.44f, 762.83f },
        { -831.496f, 4211.34f, 747.96f },
        { -182.152f, 4171.88f, 621.09f },
        { 467.315f, 4754.77f, 426.057f },
        { 1394.67f, 5938.79f, 284.364f },
        { 2044.85f, 6222.65f, 251.669f },
        { 2119.94f, 6390.72f, 166.695f },
        { 2110.01f, 6533.96f, 149.441f },
        { 2227.59f, 6575.64f, 141.633f }
    };
}

namespace ValsharahScenes
{
    enum eScenes
    {
        Scene_Arrival       = 1560,
        Scene_Xavius_Escape = 1676,
        Scene_Temple_Attack = 1490,
        Scene_Corruption    = 473,
        Scene_Summon_Ysera  = 1518
    };
}

namespace ValsharahGossips
{
    enum eGossips
    {
        Gossip_Malfurion_Grove = 911090
    };
}
