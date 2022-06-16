////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Define.h"
#include "Common.h"

namespace MS { namespace Garrison
{
    struct GarrisonPlotInstanceInfoLocation
    {
        uint32 SiteLevelID;
        uint32 PlotInstanceID;
        float X, Y, Z;
        float O;
    };

    struct GarrisonCacheInfoLocation
    {
        uint32 SiteLevelID;
        float X, Y, Z;
        float O;
    };

    struct GarrisonMissionReward
    {
        std::vector<std::pair<uint32, uint32>> RewardCurrencies;
        std::vector<std::pair<uint32, uint32>> RewardItems;
        std::vector<std::pair<uint64, uint32>> RewardFollowerXPBonus;
        uint32 RewardGold;
        uint32 RewardFollowerXP;

        std::vector<uint64> MissionFollowers;

        bool BonusChest;

        uint32 MissionID;
        bool Rewarded;
    };

    struct WeeklyTavernData
    {
        uint32 FollowerID;
        std::vector<uint32> Abilities;
    };

}   ///< namespace Garrison
}   ///< namespace MS


