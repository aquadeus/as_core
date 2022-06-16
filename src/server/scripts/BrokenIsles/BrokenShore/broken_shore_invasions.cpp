////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "broken_shore_invasions.hpp"
#include "BrokenIslesPCH.h"
#include "ScriptMgr.h"
#include "MapManager.h"
#include "POIMgr.hpp"
#include "WorldQuestMgr.h"

namespace Invasions
{
    class POI_Invasions : public POIScript
    {
        public:
            POI_Invasions() : POIScript("POI_Invasions")
            {
                AddInvasionQuests();
                SetInvasionsAreaIDs();
            }

            enum eSpells
            {
                InvasionSkyBox = 240622
            };

            void OnCreateBaseMap(Map* p_Map, uint32 p_ZoneID) override
            {
                if (!g_ActiveInvasionZone || (p_ZoneID && p_ZoneID != g_ActiveInvasionZone))
                    return;

                std::set<uint32> l_AreasToPhase;
                for (auto const& l_Quest : g_InvasionQuestByAreaID)
                {
                    if (g_InvasionCurrentWQuests.find(l_Quest.second) != g_InvasionCurrentWQuests.end())
                        l_AreasToPhase.insert(l_Quest.first);
                }

                auto& l_WorldObjects = p_Map->GetObjectsOnMap();
                for (WorldObject* l_WorldObject : l_WorldObjects)
                {
                    if (Player* l_Player = l_WorldObject->ToPlayer())
                        continue;

                    if (!CanBePhasedOut(l_WorldObject))
                        continue;

                    if (l_AreasToPhase.find(l_WorldObject->GetAreaId()) == l_AreasToPhase.end())
                        continue;

                    bool l_IsInvasionWorldObject = false;
                    std::set<uint32> const& l_WorldObjectPhases = l_WorldObject->GetPhases();
                    MultimapBounds_uint32 l_Bounds = g_InvasionQuestByAreaID.equal_range(l_WorldObject->GetAreaId());
                    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
                    {
                        if (l_WorldObjectPhases.find(l_Itr->second) != l_WorldObjectPhases.end())
                        {
                            l_IsInvasionWorldObject = true;
                            break;
                        }
                    }

                    if (l_IsInvasionWorldObject)
                        continue;

                    g_HiddenObjectsForInvasionLock.lock();
                    g_HiddenObjectsForInvasion.insert(l_WorldObject->GetGUID());
                    g_HiddenObjectsForInvasionLock.unlock();
                    l_WorldObject->SetInExcludePhase(eExcludePhases::PhaseBrokenShoreInvasions, true, true);
                }
            }

            bool CanBePhasedOut(WorldObject const* p_WorldObject)
            {
                /// Hackfix
                /// Never phase out prince Farondis which is in the Area of a WQ invasion
                switch (p_WorldObject->GetEntry())
                {
                    case 119002: ///< Prince Farondis
                    case 119944: ///< Lasan Skyhorn
                    case 252247: ///< EoA Entrace Portal
                    case 117265: ///< Injured Protector
                    case 107995: ///< Stellagosa
                    case 96465: ///< Vethir
                        return false;
                    default:
                        break;
                }

                return true;
            }

            void OnWorldObjectCreate(WorldObject const* p_WorldObject) override
            {
                if (!g_ActiveInvasionZone || g_ActiveInvasionZone != p_WorldObject->GetZoneId())
                    return;

                if (p_WorldObject->ToUnit() && p_WorldObject->ToUnit()->isSummon())
                    return;

                if (!CanBePhasedOut(p_WorldObject))
                    return;

                std::set<uint32> const& l_WorldObjectPhases = p_WorldObject->GetPhases();
                MultimapBounds_uint32 l_Bounds = g_InvasionQuestByAreaID.equal_range(p_WorldObject->GetAreaId());
                bool l_ActiveQuestInArea = false;

                MS::WorldQuest::ActiveWorldQuestsList const& l_ActiveWorldQuests = sWorldQuestGenerator->GetActiveWorldQuests();

                for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
                {
                    bool l_Found = false;
                    l_ActiveWorldQuests.safe_foreach(true, [&l_Found, &l_Itr](MS::WorldQuest::ActiveWorldQuestsList::const_iterator& p_Itr) -> void
                    {
                        l_Found = l_Found || (p_Itr->second->m_ID == l_Itr->second);
                    });

                    l_ActiveQuestInArea = l_Found;

                    if (l_WorldObjectPhases.find(l_Itr->second) != l_WorldObjectPhases.end())
                        return;
                }

                if (!l_ActiveQuestInArea)
                    return;

                g_HiddenObjectsForInvasionLock.lock();
                g_HiddenObjectsForInvasion.insert(p_WorldObject->GetGUID());
                g_HiddenObjectsForInvasionLock.unlock();
                const_cast<WorldObject*>(p_WorldObject)->SetInExcludePhase(eExcludePhases::PhaseBrokenShoreInvasions, true, true);
            }

            void OnUpdateWorldQuests(Player* p_Player, uint32 /*p_OldAreaID*/, uint32 p_NewAreaID) override
            {
                std::map<uint32, std::pair<MS::WorldQuest::Template const*, bool>> l_WQToUpdate;
                for (auto const& l_Current : g_InvasionQuestByAreaID)
                {
                    if (l_WQToUpdate.find(l_Current.second) == l_WQToUpdate.end())
                        l_WQToUpdate[l_Current.second] = { nullptr, false };

                    if (!l_WQToUpdate[l_Current.second].first)
                        l_WQToUpdate[l_Current.second].first = MS::WorldQuest::Template::GetTemplate(l_Current.second);

                    if (!l_WQToUpdate[l_Current.second].first)
                        continue;

                    if (l_WQToUpdate[l_Current.second].first->GetAreaID() == p_NewAreaID || l_Current.first == p_NewAreaID)
                        l_WQToUpdate[l_Current.second].second = true;
                }

                for (auto const& l_WorldQuest : l_WQToUpdate)
                    p_Player->UpdateWorldQuest(l_WorldQuest.second.first, l_WorldQuest.second.second);
            }

            void OnPlayerLogin(Player* p_Player) override
            {
                for (auto const& l_Quest : g_InvasionQuestByZoneID)
                {
                    if (l_Quest.first != g_ActiveInvasionZone)
                        EnableQuestForPlayer(l_Quest.second, p_Player, false, false);
                }

                for (auto const& l_Quest : g_InvasionMetaQuestByZoneID)
                {
                    if (l_Quest.first == g_ActiveInvasionZone)
                        continue;

                    p_Player->SetInPhase(l_Quest.second * 2, false, false);
                    p_Player->SetInPhase(l_Quest.second, true, false);

                    /// Code taken from cs_quest.cpp
                    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                    {
                        uint32 logQuest = p_Player->GetQuestSlotQuestId(slot);
                        if (logQuest == l_Quest.second)
                        {
                            p_Player->SetQuestSlot(slot, 0);
                            p_Player->TakeQuestSourceItem(logQuest, false);
                        }
                    }

                    p_Player->RemoveActiveQuest(l_Quest.second);
                    p_Player->RemoveRewardedQuest(l_Quest.second);
                    p_Player->SetInPhase(l_Quest.second, true, false); ///< Use Quest ID as Phase ID

                    Quest const* l_QuestTemplate = sObjectMgr->GetQuestTemplate(l_Quest.second);
                    if (!l_QuestTemplate)
                        continue;

                    for (uint32 l_QuestID : g_InvasionQuestLinesByZoneID[l_QuestTemplate->GetZoneOrSort()])
                    {
                        for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                        {
                            uint32 logQuest = p_Player->GetQuestSlotQuestId(slot);
                            if (logQuest == l_QuestID)
                            {
                                p_Player->SetQuestSlot(slot, 0);
                                p_Player->TakeQuestSourceItem(logQuest, false);
                            }
                        }

                        p_Player->RemoveActiveQuest(l_QuestID);
                        p_Player->RemoveRewardedQuest(l_QuestID);
                    }
                }

                if (g_ActiveInvasionZone && CanSeeInvasions(p_Player))
                {
                    for (uint32 const& l_QuestID : p_Player->getRewardedQuests())
                    {
                        if (g_InvasionLastQuests.find(l_QuestID) == g_InvasionLastQuests.end())
                            continue;

                        DisableInvasionPhasingForPlayer(p_Player);
                        return;
                    }

                    std::map<uint32, uint32>::const_iterator l_Itr = g_InvasionMetaQuestByZoneID.find(g_ActiveInvasionZone);
                    if (p_Player->GetWorldQuestManager())
                    {
                        if (g_ActiveInvasionZone == p_Player->GetZoneId() && p_Player->getLevel() >= 110)
                        {
                            p_Player->CastSpell(p_Player, eSpells::InvasionSkyBox, true);
                            if (l_Itr != g_InvasionMetaQuestByZoneID.end())
                            {
                                if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_Itr->second))
                                {
                                    if (!p_Player->HasQuest(l_Quest->GetQuestId()) && !p_Player->IsQuestRewarded(l_Quest->GetQuestId()))
                                    {
                                        p_Player->AddQuest(l_Quest, p_Player);
                                        p_Player->SetInPhase(l_Itr->second, true, true); ///< Use Quest ID as Phase ID
                                    }
                                }
                            }
                        }

                        for (uint32 l_QuestID : g_InvasionCurrentWQuests)
                            EnableQuestForPlayer(l_QuestID, p_Player, true, true);
                    }
                    else
                        p_Player->SetInPhase(l_Itr->second * 2, true, true);
                }
            }

            void OnPlayerUpdateZone(Player* p_Player, uint32 p_NewZone, uint32 p_OldZone) override
            {
                if (!p_Player->GetWorldQuestManager())
                    return;

                if (p_NewZone == g_ActiveInvasionZone && CanSeeInvasions(p_Player) && p_Player->getLevel() >= 110)
                {
                    p_Player->CastSpell(p_Player, eSpells::InvasionSkyBox, true);
                    std::map<uint32, uint32>::const_iterator l_Itr = g_InvasionMetaQuestByZoneID.find(p_NewZone);
                    if (l_Itr != g_InvasionMetaQuestByZoneID.end())
                    {
                        if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_Itr->second))
                        {
                            if (!p_Player->HasQuest(l_Quest->GetQuestId()) && !p_Player->IsQuestRewarded(l_Quest->GetQuestId()))
                            {
                                p_Player->AddQuest(l_Quest, p_Player);
                                p_Player->SetInPhase(l_Itr->second, true, true); ///< Use Quest ID as Phase ID
                            }
                        }
                    }

                    return;
                }

                p_Player->RemoveAura(eSpells::InvasionSkyBox, true);
            }

            void OnPOIAppears(AreaPOI const& p_AreaPOI) override
            {
                if (!IsBrokenIslesInvasion(p_AreaPOI))
                    return;

                AreaPOIEntry const* p_AreaPoiEntry = sPOIMgr->GetAreaPOIEntry(p_AreaPOI.GetAreaPoiID());
                if (!p_AreaPoiEntry)
                    return;

                uint32 p_ZoneID = sMapMgr->GetZoneId(p_AreaPoiEntry->MapID, p_AreaPoiEntry->PositionX, p_AreaPoiEntry->PositionY, p_AreaPoiEntry->PositionZ);

                g_ActiveInvasionZone = p_ZoneID;

                auto l_Bounds = g_InvasionQuestByZoneID.equal_range(p_ZoneID);
                for (std::map<uint32, uint32>::const_iterator l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
                {
                    MS::WorldQuest::Template const* l_Template = MS::WorldQuest::Template::GetTemplate(l_Itr->second);
                    if (!l_Template)
                        continue;

                    sWorldQuestGenerator->AddQuest(l_Template, p_AreaPOI.StartTime, p_AreaPOI.Duration, true);
                    g_InvasionCurrentWQuests.insert(l_Itr->second);
                }

                std::set<uint32> l_AreasToPhase;
                for (auto const& l_Quest : g_InvasionQuestByAreaID)
                {
                    if (g_InvasionCurrentWQuests.find(l_Quest.second) != g_InvasionCurrentWQuests.end())
                        l_AreasToPhase.insert(l_Quest.first);
                }

                std::list<Map*> l_MapsToPhase = sMapMgr->GetMapsPerZoneId(p_AreaPoiEntry->MapID, p_ZoneID);
                for (Map* l_Map : l_MapsToPhase)
                {
                    l_Map->AddTask([l_Map, this, l_AreasToPhase]()->void
                    {
                        std::unordered_set<WorldObject*> const l_WorldObjects = l_Map->GetObjectsOnMap();
                        for (WorldObject* l_WorldObject : l_WorldObjects)
                        {
                            if (Player* l_Player = l_WorldObject->ToPlayer())
                                continue;

                            if (!CanBePhasedOut(l_WorldObject))
                                continue;

                            if (l_AreasToPhase.find(l_WorldObject->GetAreaId()) == l_AreasToPhase.end())
                                continue;

                            bool l_IsInvasionWorldObject = false;
                            std::set<uint32> const& l_WorldObjectPhases = l_WorldObject->GetPhases();
                            MultimapBounds_uint32 l_Bounds = g_InvasionQuestByAreaID.equal_range(l_WorldObject->GetAreaId());
                            for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
                            {
                                if (l_WorldObjectPhases.find(l_Itr->second) != l_WorldObjectPhases.end())
                                {
                                    l_IsInvasionWorldObject = true;
                                    break;
                                }
                            }

                            if (l_IsInvasionWorldObject)
                                continue;

                            g_HiddenObjectsForInvasionLock.lock();
                            g_HiddenObjectsForInvasion.insert(l_WorldObject->GetGUID());
                            g_HiddenObjectsForInvasionLock.unlock();
                            l_WorldObject->SetInExcludePhase(eExcludePhases::PhaseBrokenShoreInvasions, true, true);
                        }
                    });
                }

                SessionMap const& l_Sessions = sWorld->GetAllSessions();
                sWorld->GetSessionsLock().acquire_read();
                for (SessionMap::const_iterator l_Session = l_Sessions.begin(); l_Session != l_Sessions.end(); ++l_Session)
                {
                    if (Player* l_Player = l_Session->second->GetPlayer())
                    {
                        l_Player->AddDelayedEvent([l_Player]() -> void
                        {
                            for (auto const& l_Quest : g_InvasionMetaQuestByZoneID)
                            {
                                l_Player->SetInPhase(l_Quest.second * 2, false, false);
                                l_Player->SetInPhase(l_Quest.second, true, false);

                                /// Code taken from cs_quest.cpp
                                for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                                {
                                    uint32 logQuest = l_Player->GetQuestSlotQuestId(slot);
                                    if (logQuest == l_Quest.second)
                                    {
                                        l_Player->SetQuestSlot(slot, 0);
                                        l_Player->TakeQuestSourceItem(logQuest, false);
                                    }
                                }

                                l_Player->RemoveActiveQuest(l_Quest.second);
                                l_Player->RemoveRewardedQuest(l_Quest.second);
                                l_Player->SetInPhase(l_Quest.second, true, false); ///< Use Quest ID as Phase ID

                                Quest const* l_QuestTemplate = sObjectMgr->GetQuestTemplate(l_Quest.second);
                                if (!l_QuestTemplate)
                                    continue;

                                for (uint32 l_QuestID : g_InvasionQuestLinesByZoneID[l_QuestTemplate->GetZoneOrSort()])
                                {
                                    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                                    {
                                        uint32 logQuest = l_Player->GetQuestSlotQuestId(slot);
                                        if (logQuest == l_QuestID)
                                        {
                                            l_Player->SetQuestSlot(slot, 0);
                                            l_Player->TakeQuestSourceItem(logQuest, false);
                                        }
                                    }

                                    l_Player->RemoveActiveQuest(l_QuestID);
                                    l_Player->RemoveRewardedQuest(l_QuestID);
                                }
                            }

                            for (uint32 l_QuestID : g_InvasionCurrentWQuests)
                                EnableQuestForPlayer(l_QuestID, l_Player, true, true);
                        });
                    }
                }
                sWorld->GetSessionsLock().release();
            }

            void OnPOIDisappears(AreaPOI const& p_AreaPOI) override
            {
                if (!IsBrokenIslesInvasion(p_AreaPOI))
                    return;

                std::map<uint32, uint32>::const_iterator l_Itr = g_InvasionMetaQuestByZoneID.find(g_ActiveInvasionZone);
                if (l_Itr != g_InvasionMetaQuestByZoneID.end())
                {
                    std::pair<uint32, uint32> l_Pair = *l_Itr;

                    SessionMap const& l_Sessions = sWorld->GetAllSessions();
                    sWorld->GetSessionsLock().acquire_read();
                    for (SessionMap::const_iterator l_Session = l_Sessions.begin(); l_Session != l_Sessions.end(); ++l_Session)
                    {
                        if (Player* l_Player = l_Session->second->GetPlayer())
                        {
                            auto l_WQs = g_InvasionCurrentWQuests;
                            l_Player->AddDelayedEvent([l_Player, l_Pair, l_WQs]() -> void
                            {
                                /// Code taken from cs_quest.cpp
                                for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                                {
                                    uint32 logQuest = l_Player->GetQuestSlotQuestId(slot);
                                    if (logQuest == l_Pair.second)
                                    {
                                        l_Player->SetQuestSlot(slot, 0);
                                        l_Player->TakeQuestSourceItem(logQuest, false);
                                    }
                                }

                                l_Player->RemoveActiveQuest(l_Pair.second);
                                l_Player->RemoveRewardedQuest(l_Pair.second);
                                l_Player->SetInPhase(l_Pair.second, true, false); ///< Use Quest ID as Phase ID

                                for (uint32 l_QuestID : l_WQs)
                                    EnableQuestForPlayer(l_QuestID, l_Player, true, false);

                                Quest const* l_QuestTemplate = sObjectMgr->GetQuestTemplate(l_Pair.second);
                                if (!l_QuestTemplate)
                                    return;

                                for (uint32 l_QuestID : g_InvasionQuestLinesByZoneID[l_QuestTemplate->GetZoneOrSort()])
                                {
                                    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                                    {
                                        uint32 logQuest = l_Player->GetQuestSlotQuestId(slot);
                                        if (logQuest == l_QuestID)
                                        {
                                            l_Player->SetQuestSlot(slot, 0);
                                            l_Player->TakeQuestSourceItem(logQuest, false);
                                        }
                                    }

                                    l_Player->RemoveActiveQuest(l_QuestID);
                                    l_Player->RemoveRewardedQuest(l_QuestID);
                                }
                            });
                        }
                    }
                    sWorld->GetSessionsLock().release();

                    if (Quest const* l_QuestTemplate = sObjectMgr->GetQuestTemplate(l_Itr->second))
                    {
                        std::ostringstream l_Query;
                        l_Query << "DELETE FROM character_queststatus_rewarded WHERE quest IN ";
                        uint32 l_I = 0;
                        for (uint32 l_QuestID : g_InvasionQuestLinesByZoneID[l_QuestTemplate->GetZoneOrSort()])
                        {
                            if (l_I == 0)
                                l_Query << "(";
                            else
                                l_Query << ", ";

                            l_Query << l_QuestID;
                            ++l_I;
                        }
                        if (!l_I)
                            return;

                        l_Query << ");";

                        CharacterDatabase.Execute(l_Query.str().c_str());
                    }
                }

                g_InvasionCurrentWQuests.clear();

                g_HiddenObjectsForInvasionLock.lock();
                for (uint64 l_GUID : g_HiddenObjectsForInvasion)
                {
                    WorldObject* l_WorldObject = nullptr;
                    if (IS_CREATURE_GUID(l_GUID))
                        l_WorldObject = sObjectAccessor->FindCreature(l_GUID);
                    else
                        l_WorldObject = sObjectAccessor->FindGameObject(l_GUID);

                    if (!l_WorldObject)
                        continue;

                    l_WorldObject->GetMap()->AddTask([l_WorldObject]()->void
                    {
                        l_WorldObject->SetInExcludePhase(eExcludePhases::PhaseBrokenShoreInvasions, false, false);
                    });
                }

                g_HiddenObjectsForInvasion.clear();

                g_HiddenObjectsForInvasionLock.unlock();

                for (uint64 l_GUID : g_InvasionPhasingDisabledFor)
                {
                    Player* l_Player = sObjectAccessor->FindPlayer(l_GUID);
                    if (!l_Player)
                        continue;

                    l_Player->AddDelayedEvent([l_Player]()->void { l_Player->SetInExcludePhase(eExcludePhases::PhaseBrokenShoreInvasions, false, true); });
                }

                g_InvasionPhasingDisabledFor.clear();

                g_ActiveInvasionZone = 0;
            }

            void OnQuestReward(Player* p_Player, Quest const* p_Quest) override
            {
                if (g_InvasionLastQuests.find(p_Quest->GetQuestId()) == g_InvasionLastQuests.end())
                    return;

                DisableInvasionPhasingForPlayer(p_Player);
            }

        private:
            void AddInvasionQuests()
            {
                for (auto l_Itr : sObjectMgr->GetQuestTemplates())
                {
                    Quest const* l_Quest = l_Itr.second;

                    if (l_Quest->GetType() == QUEST_INFO_LEGION_INVASION_WQ || l_Quest->GetType() == QUEST_INFO_LEGION_INVASION_ELITE_WQ)
                        g_InvasionQuestByZoneID.insert(std::map<uint32, uint32>::value_type(l_Quest->GetZoneOrSort(), l_Quest->GetQuestId()));

                    if (l_Quest->GetType() == QUEST_INFO_LEGION_INVASION_WQ_WRAPPER)
                    {
                        g_InvasionMetaQuestByZoneID[l_Quest->GetZoneOrSort()] = l_Quest->GetQuestId();

                        uint32 l_LastQuestInChain = l_Quest->GetQuestId();
                        while (l_Quest && l_Quest->GetNextQuestInChain())
                        {
                            l_LastQuestInChain = l_Quest->GetNextQuestInChain();
                            g_InvasionQuestLinesByZoneID[l_Quest->GetZoneOrSort()].insert(l_LastQuestInChain);
                            l_Quest = sObjectMgr->GetQuestTemplate(l_LastQuestInChain);
                        }

                        g_InvasionLastQuests.insert(l_LastQuestInChain);
                    }
                }
            }

            void AddQuestForArea(uint32 p_QuestID, uint32 p_AreaID)
            {
                MultimapBounds_uint32 l_Bounds = g_InvasionQuestByAreaID.equal_range(p_AreaID);
                for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
                {
                    if (l_Itr->second == p_QuestID)
                        return;
                }

                g_InvasionQuestByAreaID.insert(std::map<uint32, uint32>::value_type(p_AreaID, p_QuestID));
            }

            void SetInvasionsAreaIDs()
            {
                for (auto l_Itr : g_InvasionQuestByZoneID)
                {
                    Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_Itr.second);
                    if (!l_Quest)
                        continue;

                    if (l_Quest->GetAreaGroupId())
                    {
                        for (uint32 l_AreaID : GetAreasForGroup(l_Quest->GetAreaGroupId()))
                            AddQuestForArea(l_Itr.second, l_AreaID);

                        continue;
                    }

                    for (QuestObjective const& l_Objective : l_Quest->QuestObjectives)
                    {
                        switch (l_Objective.Type)
                        {
                            case QuestObjectiveType::QUEST_OBJECTIVE_TYPE_NPC:
                            {
                                MultimapBounds_uint32 l_Bounds = sObjectMgr->GetAreasForCreature(l_Objective.ObjectID);
                                for (auto l_NpcItr = l_Bounds.first; l_NpcItr != l_Bounds.second; ++l_NpcItr)
                                    AddQuestForArea(l_Itr.second, l_NpcItr->second);

                                break;
                            }
                            case QuestObjectiveType::QUEST_OBJECTIVE_TYPE_GO:
                            {
                                MultimapBounds_uint32 l_Bounds = sObjectMgr->GetAreasForGameObject(l_Objective.ObjectID);
                                for (auto l_GoItr = l_Bounds.first; l_GoItr != l_Bounds.second; ++l_GoItr)
                                    AddQuestForArea(l_Itr.second, l_GoItr->second);

                                break;
                            }
                            default:
                                break;
                        }
                    }
                }

                sObjectMgr->ClearInvasionsQuestsAreaContainers();
            }

            static void EnableQuestForPlayer(uint32 p_QuestID, Player* p_Player, bool p_Update, bool p_Apply)
            {
                p_Player->SetInPhase(p_QuestID, p_Update, p_Apply);

                if (g_WorldStatesByQuestID[p_QuestID].empty())
                {
                    QuestV2CliTaskEntry const* l_QuestV2CliTaskEntry = sQuestV2CliTaskStore.LookupEntry(p_QuestID);
                    if (!l_QuestV2CliTaskEntry)
                        return;

                    WorldStateExpressionEntry const* l_WorldStateExpressionEntry = sWorldStateExpressionStore.LookupEntry(l_QuestV2CliTaskEntry->WorldStateExpressionID);
                    if (!l_WorldStateExpressionEntry)
                        return;

                    std::vector<std::pair<uint32, uint32>> l_WorldStates;
                    l_WorldStateExpressionEntry->GetRequiredWorldStatesIfExists(&l_WorldStates);

                    for (std::pair<uint32, uint32> l_WorldState : l_WorldStates)
                        g_WorldStatesByQuestID[p_QuestID].insert(l_WorldState);
                }

                for (std::pair<uint32, uint32> l_WorldState : g_WorldStatesByQuestID[p_QuestID])
                    p_Player->SendUpdateWorldState(l_WorldState.first, p_Apply ? l_WorldState.second : 0);
            }

            bool IsBrokenIslesInvasion(AreaPOI const& p_AreaPOI) const
            {
                switch (p_AreaPOI.GetAreaPoiID())
                {
                    case 5175:
                    case 5177:
                    case 5178:
                    case 5210:
                        return true;
                    default:
                        break;
                }

                return false;
            }

            bool CanSeeInvasions(WorldObject const* p_WorldObject) const
            {
                Player const* l_Player = p_WorldObject->ToPlayer();
                if (!l_Player)
                    return false;

                return l_Player->HasInExcludePhaseList(eExcludePhases::PhaseBrokenShoreInvasions);
            }

            void DisableInvasionPhasingForPlayer(Player* p_Player)
            {
                if (g_InvasionPhasingDisabledFor.find(p_Player->GetGUID()) != g_InvasionPhasingDisabledFor.end())
                    return;

                p_Player->SetInExcludePhase(eExcludePhases::PhaseBrokenShoreInvasions, false, false);
                g_InvasionPhasingDisabledFor.insert(p_Player->GetGUID());

                std::map<uint32, uint32>::const_iterator l_Itr = g_InvasionMetaQuestByZoneID.find(g_ActiveInvasionZone);
                if (l_Itr != g_InvasionMetaQuestByZoneID.end())
                {
                    /// Code taken from cs_quest.cpp
                    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                    {
                        uint32 logQuest = p_Player->GetQuestSlotQuestId(slot);
                        if (logQuest == l_Itr->second)
                        {
                            p_Player->SetQuestSlot(slot, 0);
                            p_Player->TakeQuestSourceItem(logQuest, false);
                        }
                    }

                    p_Player->RemoveActiveQuest(l_Itr->second);
                    p_Player->RemoveRewardedQuest(l_Itr->second);
                    p_Player->SetInPhase(l_Itr->second, true, false); ///< Use Quest ID as Phase ID

                    for (uint32 l_QuestID : g_InvasionCurrentWQuests)
                        EnableQuestForPlayer(l_QuestID, p_Player, true, false);
                }
            }
    };
}

#ifndef __clang_analyzer__
void AddSC_broken_shore_invasions()
{
    new Invasions::POI_Invasions();
}
#endif
