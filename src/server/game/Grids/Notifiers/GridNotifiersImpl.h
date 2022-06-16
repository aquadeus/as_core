////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_GRIDNOTIFIERSIMPL_H
#define TRINITY_GRIDNOTIFIERSIMPL_H

#include "GridNotifiers.h"
#include "WorldPacket.h"
#include "Corpse.h"
#include "Player.h"
#include "UpdateData.h"
#include "CreatureAI.h"
#include "SpellAuras.h"

template<class T>
inline void JadeCore::VisibleNotifier::Visit(GridRefManager<T> &m)
{
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        vis_guids.erase(iter->getSource()->GetGUID());
        i_player.UpdateVisibilityOf(iter->getSource(), i_data, i_visibleNow);
    }
}

inline void JadeCore::VisibleNotifier::Visit(EventObjectMapType &m)
{
    for (EventObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        vis_guids.erase(iter->getSource()->GetGUID());

    if (Map* l_Map = i_player.GetMap())
    {
        for (WorldObject* l_BigObject : l_Map->GetBigObjects())
            vis_guids.erase(l_BigObject->GetGUID());
    }
}

inline void JadeCore::ObjectUpdater::Visit(CreatureMapType &m)
{
    Map* l_Map = nullptr;
    InstanceScript* l_Instance = nullptr;

    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->getSource()->IsInWorld())
        {
            if (l_Map == nullptr)
            {
                l_Map = iter->getSource()->GetMap();
                l_Instance = iter->getSource()->GetInstanceScript();
            }

            iter->getSource()->Update(i_timeDiff);
        }
    }

#ifndef WIN32
    /*if (l_Map != nullptr && m.getSize() > 1000 && (l_Map->IsRaid() || l_Map->IsDungeon()))
    {
        bool l_IsChallenge = l_Instance ? l_Instance->IsChallenge() : false;
        sLog->outExtChat("#jarvis", "danger", true, "JadeCore::ObjectUpdater::Visit, more than 500 creatures (%u) in the same cell! (Map: %u, Difficulty: %u, IsChallenge: %s)", uint32(m.getSize()), l_Map->GetId(), l_Map->GetDifficultyID(), std::to_string(l_IsChallenge).c_str());

        std::ostringstream l_Oss;
        for (MapRefManager::const_iterator l_Itr = l_Map->GetPlayers().begin(); l_Itr != l_Map->GetPlayers().end(); ++l_Itr)
        {
            if (Player* player = l_Itr->getSource())
                l_Oss << player->GetName() << " -- ";
        }

        sLog->outExtChat("#jarvis", "danger", true, "%s", l_Oss.str().c_str());

        l_Map->SetUnloadTimer(1);
    }*/
#endif
}

inline void JadeCore::ObjectUpdater::Visit(GameObjectMapType &m)
{
    for (GameObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (iter->getSource()->IsInWorld() && !iter->getSource()->IsTransport())
            iter->getSource()->Update(i_timeDiff);
}

// SEARCHERS & LIST SEARCHERS & WORKERS

// WorldObject searchers & workers

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(GameObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
        return;

    // already found
    if (i_object)
        return;

    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(PlayerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
        return;

    // already found
    if (i_object)
        return;

    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(CreatureMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
        return;

    // already found
    if (i_object)
        return;

    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(CorpseMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
        return;

    // already found
    if (i_object)
        return;

    for (CorpseMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(DynamicObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
        return;

    // already found
    if (i_object)
        return;

    for (DynamicObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(AreaTriggerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_AREATRIGGER))
        return;

    // already found
    if (i_object)
        return;

    for (AreaTriggerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(ConversationMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CONVERSATION))
        return;

    /// Already found
    if (i_object)
        return;

    for (ConversationMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (!l_Iter->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(l_Iter->getSource()))
        {
            i_object = l_Iter->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectSearcher<Check>::Visit(EventObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_EVENTOBJECT))
        return;

    /// Already found
    if (i_object)
        return;

    for (EventObjectMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (!l_Iter->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(l_Iter->getSource()))
        {
            i_object = l_Iter->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(GameObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
        return;

    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(PlayerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
        return;

    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(CreatureMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
        return;

    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(CorpseMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
        return;

    for (CorpseMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(DynamicObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
        return;

    for (DynamicObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(AreaTriggerMapType  &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_AREATRIGGER))
        return;

    for (AreaTriggerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(ConversationMapType  &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CONVERSATION))
        return;

    for (ConversationMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (!l_Iter->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(l_Iter->getSource()))
            i_object = l_Iter->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::Visit(EventObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_EVENTOBJECT))
        return;

    for (EventObjectMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (!l_Iter->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(l_Iter->getSource()))
            i_object = l_Iter->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(PlayerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
        return;

    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(CreatureMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
        return;

    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(CorpseMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
        return;

    for (CorpseMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(GameObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
        return;

    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(DynamicObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
        return;

    for (DynamicObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(AreaTriggerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_AREATRIGGER))
        return;

    for (AreaTriggerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(ConversationMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CONVERSATION))
        return;

    for (ConversationMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (i_check(l_Iter->getSource()))
            i_objects.push_back(l_Iter->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::Visit(EventObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_EVENTOBJECT))
        return;

    for (EventObjectMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (i_check(l_Iter->getSource()))
            i_objects.push_back(l_Iter->getSource());
    }
}

/// AreaTrigger searchers
template<class Check>
void JadeCore::AreaTriggerListSearcher<Check>::Visit(AreaTriggerMapType& p_AreaTriggerMap)
{
    for (AreaTriggerMapType::iterator l_Iterator = p_AreaTriggerMap.begin(); l_Iterator != p_AreaTriggerMap.end(); ++l_Iterator)
    {
        if (l_Iterator->getSource()->InSamePhase(m_Searcher))
        {
            if (m_Check(l_Iterator->getSource()))
                m_AreaTriggers.push_back(l_Iterator->getSource());
        }
    }
}

template<class Check>
void JadeCore::AreaTriggerSearcher<Check>::Visit(AreaTriggerMapType& p_AreatriggerMap)
{
    // already found
    if (i_object)
        return;

    for (AreaTriggerMapType::iterator itr=p_AreatriggerMap.begin(); itr != p_AreatriggerMap.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

/// Conversation searchers
template<class Check>
void JadeCore::ConversationSearcher<Check>::Visit(ConversationMapType& p_ConversationMap)
{
    /// Already found
    if (i_object)
        return;

    for (ConversationMapType::iterator itr = p_ConversationMap.begin(); itr != p_ConversationMap.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::ConversationListSearcher<Check>::Visit(ConversationMapType& p_ConversationMap)
{
    for (ConversationMapType::iterator l_Iterator = p_ConversationMap.begin(); l_Iterator != p_ConversationMap.end(); ++l_Iterator)
    {
        if (l_Iterator->getSource()->InSamePhase(m_Searcher))
        {
            if (m_Check(l_Iterator->getSource()))
                m_Conversations.push_back(l_Iterator->getSource());
        }
    }
}

/// Gameobject searchers
template<class Check>
void JadeCore::GameObjectSearcher<Check>::Visit(GameObjectMapType &m)
{
    // already found
    if (i_object)
        return;

    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::GameObjectLastSearcher<Check>::Visit(GameObjectMapType &m)
{
    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::GameObjectListSearcher<Check>::Visit(GameObjectMapType &m)
{
    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (itr->getSource()->InSamePhase(m_Searcher))
            if (i_check(itr->getSource()))
                i_objects.push_back(itr->getSource());
}

// Unit searchers

template<class Check>
void JadeCore::UnitSearcher<Check>::Visit(CreatureMapType &m)
{
    // already found
    if (i_object)
        return;

    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::UnitSearcher<Check>::Visit(PlayerMapType &m)
{
    // already found
    if (i_object)
        return;

    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::UnitLastSearcher<Check>::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::UnitLastSearcher<Check>::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::UnitListSearcher<Check>::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (itr->getSource()->InSamePhase(m_Searcher))
            if (i_check(itr->getSource()))
                i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::UnitListSearcher<Check>::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (itr->getSource()->InSamePhase(m_Searcher))
            if (i_check(itr->getSource()))
                i_objects.push_back(itr->getSource());
}

// Creature searchers
template<class Check>
void JadeCore::CreatureSearcher<Check>::Visit(CreatureMapType &m)
{
    // already found
    if (i_object)
        return;

    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::CreatureLastSearcher<Check>::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::CreatureListSearcher<Check>::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
        if (itr->getSource()->InSamePhase(m_Searcher))
            if (i_check(itr->getSource()))
                i_objects.push_back(itr->getSource());
}

template<class Check>
void JadeCore::PlayerListSearcher<Check>::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource())
            continue;

        if (itr->getSource()->InSamePhase(m_Searcher))
            if (i_check(itr->getSource()))
                i_objects.push_back(itr->getSource());
    }
}

template<class Check>
void JadeCore::PlayerSearcher<Check>::Visit(PlayerMapType &m)
{
    // already found
    if (i_object)
        return;

    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
        {
            i_object = itr->getSource();
            return;
        }
    }
}

template<class Check>
void JadeCore::PlayerLastSearcher<Check>::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Builder>
void JadeCore::LocalizedPacketDo<Builder>::operator()(Player* p)
{
    LocaleConstant loc_idx = p->GetSession()->GetSessionDbLocaleIndex();
    uint32 cache_idx = loc_idx+1;
    WorldPacket* data;

    // create if not cached yet
    if (i_data_cache.size() < cache_idx+1 || !i_data_cache[cache_idx])
    {
        if (i_data_cache.size() < cache_idx+1)
            i_data_cache.resize(cache_idx+1);

        data = new WorldPacket(SMSG_CHAT, 800);

        i_builder(*data, loc_idx);

        i_data_cache[cache_idx] = data;
    }
    else
        data = i_data_cache[cache_idx];

    p->SendDirectMessage(data);
}

template<class Builder>
void JadeCore::LocalizedPacketListDo<Builder>::operator()(Player* p)
{
    LocaleConstant loc_idx = p->GetSession()->GetSessionDbLocaleIndex();
    uint32 cache_idx = loc_idx+1;
    WorldPacketList* data_list;

    // create if not cached yet
    if (i_data_cache.size() < cache_idx+1 || i_data_cache[cache_idx].empty())
    {
        if (i_data_cache.size() < cache_idx+1)
            i_data_cache.resize(cache_idx+1);

        data_list = &i_data_cache[cache_idx];

        i_builder(*data_list, loc_idx);
    }
    else
        data_list = &i_data_cache[cache_idx];

    for (size_t i = 0; i < data_list->size(); ++i)
        p->SendDirectMessage((*data_list)[i]);
}

//////////
// Searchers with nearbies
//////////

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(GameObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
        return;

    for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(PlayerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
        return;

    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(CreatureMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
        return;

    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(CorpseMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
        return;

    for (CorpseMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(DynamicObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
        return;

    for (DynamicObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(AreaTriggerMapType  &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_AREATRIGGER))
        return;

    for (AreaTriggerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (!itr->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(itr->getSource()))
            i_object = itr->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(ConversationMapType  &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CONVERSATION))
        return;

    for (ConversationMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (i_visited.find(l_Iter->getSource()) != i_visited.end())
            continue;

        i_visited.insert(l_Iter->getSource());

        if (!l_Iter->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(l_Iter->getSource()))
            i_object = l_Iter->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectLastSearcher<Check>::VisitWithNearbies(EventObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_EVENTOBJECT))
        return;

    for (EventObjectMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (i_visited.find(l_Iter->getSource()) != i_visited.end())
            continue;

        i_visited.insert(l_Iter->getSource());

        if (!l_Iter->getSource()->InSamePhase(m_Searcher))
            continue;

        if (i_check(l_Iter->getSource()))
            i_object = l_Iter->getSource();
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(PlayerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
        return;

    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(CreatureMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
        return;

    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(CorpseMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
        return;

    for (CorpseMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(GameObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
        return;

    for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(DynamicObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
        return;

    for (DynamicObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(AreaTriggerMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_AREATRIGGER))
        return;

    for (AreaTriggerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_visited.find(itr->getSource()) != i_visited.end())
            continue;

        i_visited.insert(itr->getSource());

        if (i_check(itr->getSource()))
            i_objects.push_back(itr->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(ConversationMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CONVERSATION))
        return;

    for (ConversationMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (i_visited.find(l_Iter->getSource()) != i_visited.end())
            continue;

        i_visited.insert(l_Iter->getSource());

        if (i_check(l_Iter->getSource()))
            i_objects.push_back(l_Iter->getSource());
    }
}

template<class Check>
void JadeCore::WorldObjectListSearcher<Check>::VisitWithNearbies(EventObjectMapType &m)
{
    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_EVENTOBJECT))
        return;

    for (EventObjectMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
    {
        if (i_visited.find(l_Iter->getSource()) != i_visited.end())
            continue;

        i_visited.insert(l_Iter->getSource());

        if (i_check(l_Iter->getSource()))
            i_objects.push_back(l_Iter->getSource());
    }
}

#endif                                                      // TRINITY_GRIDNOTIFIERSIMPL_H
