////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace BrokenShore
{
    enum eBrokenShoreContributions
    {
        MageTower       = 1,
        CommandCenter   = 3,
        NetherDisruptor = 4
    };

    enum eGameObjects
    {
        GobMageTower        = 268700,
        GobCommandCenter    = 269803,
        GobNetherDisruptor  = 268702
    };

    std::set<uint32> g_BrokenShoreBuildingContributions =
    {
        eBrokenShoreContributions::MageTower,
        eBrokenShoreContributions::CommandCenter,
        eBrokenShoreContributions::NetherDisruptor
    };

    std::set<uint32> g_BrokenShoreBuildingGameObjects =
    {
        eGameObjects::GobMageTower,
        eGameObjects::GobCommandCenter,
        eGameObjects::GobNetherDisruptor
    };

    std::set<uint32> g_CurrentDailyQuest;

    std::map<uint64, uint32> g_DailyPortals;
};
