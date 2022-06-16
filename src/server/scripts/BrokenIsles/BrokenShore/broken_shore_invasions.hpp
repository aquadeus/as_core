////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace Invasions
{
    std::multimap<uint32, uint32> g_InvasionQuestByAreaID;
    std::multimap<uint32, uint32> g_InvasionQuestByZoneID;
    std::map<uint32, uint32> g_InvasionMetaQuestByZoneID;
    std::set<uint32> g_InvasionCurrentWQuests;
    std::map<uint64, std::set<uint32>> g_WorldObjectOriginalPhases;
    uint32 g_ActiveInvasionZone;
    std::set<uint64> g_HiddenObjectsForInvasion;
    std::mutex g_HiddenObjectsForInvasionLock;
    std::map<uint32, std::set<std::pair<uint32, uint32>>> g_WorldStatesByQuestID;
    std::unordered_set<uint64> g_InvasionPhasingDisabledFor;

    std::map<uint32, std::set<uint32>> g_InvasionQuestLinesByZoneID;
    std::set<uint32> g_InvasionLastQuests;
}
