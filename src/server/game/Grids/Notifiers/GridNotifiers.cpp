////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateData.h"
#include "Item.h"
#include "Map.h"
#include "Transport.h"
#include "ObjectAccessor.h"
#include "CellImpl.h"
#include "SpellInfo.h"

using namespace JadeCore;

thread_local WorldPacket tl_WorldPacket_SendToSelf;

void VisibleNotifier::SendToSelf()
{
    tl_WorldPacket_SendToSelf.reserve(50 * 1024);
    tl_WorldPacket_SendToSelf.clear();

#ifdef CROSS
    if (i_player.IsNeedRemove())
        return;

#endif /* CROSS */
    // at this moment i_clientGUIDs have guids that not iterate at grid level checks
    // but exist one case when this possible and object not out of range: transports
    if (Transport* transport = i_player.GetTransport())
    {
        for (std::set<WorldObject*>::const_iterator itr = transport->GetPassengers().begin(); itr != transport->GetPassengers().end(); ++itr)
        {
            if (vis_guids.find((*itr)->GetGUID()) != vis_guids.end())
            {
                vis_guids.erase((*itr)->GetGUID());

                switch ((*itr)->GetTypeId())
                {
                    case TYPEID_GAMEOBJECT:
                        i_player.UpdateVisibilityOf((*itr)->ToGameObject(), i_data, i_visibleNow);
                        break;
                    case TYPEID_PLAYER:
                        i_player.UpdateVisibilityOf((*itr)->ToPlayer(), i_data, i_visibleNow);
                        (*itr)->ToPlayer()->UpdateVisibilityOf(&i_player);
                        break;
                    case TYPEID_UNIT:
                        i_player.UpdateVisibilityOf((*itr)->ToCreature(), i_data, i_visibleNow);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (!i_player.m_Controlled.empty())
    {
        for (auto itr : i_player.m_Controlled)
        {
            if (vis_guids.find(itr->GetGUID()) != vis_guids.end())
            {
                vis_guids.erase(itr->GetGUID());
                if (itr->GetTypeId() == TYPEID_UNIT)
                    i_player.UpdateVisibilityOf((Creature*)(itr), i_data, i_visibleNow);
            }
        }
    }

    for (auto it = vis_guids.begin(); it != vis_guids.end(); ++it)
    {
        i_player.m_clientGUIDs_lock.acquire_write();
        i_player.m_clientGUIDs.erase(*it);
        i_player.m_clientGUIDs_lock.release();

        i_data.AddOutOfRangeGUID(*it);

        if (IS_PLAYER_GUID(*it))
        {
            Player* player = ObjectAccessor::GetPlayer(i_player, *it);
            if (player && player->IsInWorld())
                player->UpdateVisibilityOf(&i_player);
        }
    }

    if (!i_data.HasData())
        return;

    if (i_data.BuildPacket(&tl_WorldPacket_SendToSelf))
        i_player.GetSession()->SendPacket(&tl_WorldPacket_SendToSelf);

    for (std::set<Unit*>::const_iterator it = i_visibleNow.begin(); it != i_visibleNow.end(); ++it)
        i_player.SendInitialVisiblePackets(*it);
}

void VisibleChangesNotifier::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->getSource() == &i_object)
            continue;

        iter->getSource()->UpdateVisibilityOf(&i_object);

        if (!iter->getSource()->GetSharedVisionList().empty())
            for (SharedVisionList::const_iterator i = iter->getSource()->GetSharedVisionList().begin();
                i != iter->getSource()->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == iter->getSource())
                    (*i)->UpdateVisibilityOf(&i_object);
    }
}

void VisibleChangesNotifier::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (!iter->getSource()->GetSharedVisionList().empty())
            for (SharedVisionList::const_iterator i = iter->getSource()->GetSharedVisionList().begin();
                i != iter->getSource()->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == iter->getSource())
                    (*i)->UpdateVisibilityOf(&i_object);
}

void VisibleChangesNotifier::Visit(DynamicObjectMapType &m)
{
    for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (IS_PLAYER_GUID(iter->getSource()->GetCasterGUID()))
            if (Player* caster = (Player*)iter->getSource()->GetCaster())
                if (caster->m_seer == iter->getSource())
                    caster->UpdateVisibilityOf(&i_object);
}

void VisibleChangesNotifier::Visit(ConversationMapType &m)
{
    for (ConversationMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (IS_PLAYER_GUID(iter->getSource()->GetCasterGUID()))
            if (Player* caster = (Player*)iter->getSource()->GetCaster())
                if (caster->m_seer == iter->getSource())
                    caster->UpdateVisibilityOf(&i_object);
}

inline void CreatureUnitRelocationWorker(Creature* c, Unit* u)
{
    if (!u->isAlive() || !c->isAlive() || c == u || u->isInFlight())
        return;

    if (!c->HasUnitState(UNIT_STATE_SIGHTLESS))
        if ((c->IsAIEnabled && c->canSeeOrDetect(u, false, true)) ||
            (c->GetEntry() == 121230 && c->canSeeOrDetect(u, true, true, true)))
            if (c->HasReactState(REACT_AGGRESSIVE) || c->AI()->CanSeeEvenInPassiveMode())
                c->AI()->MoveInLineOfSight_Safe(u);
}

void AIRelocationNotifier::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Creature* c = iter->getSource();
        CreatureUnitRelocationWorker(c, &i_unit);
        if (isCreature)
            CreatureUnitRelocationWorker((Creature*)&i_unit, c);
    }
}

void AIRelocationNotifier::Visit(EventObjectMapType &m)
{
    for (EventObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        iter->getSource()->MoveInLineOfSight(&i_unit);
}

void AIRelocationNotifier::Visit(GameObjectMapType& p_Map)
{
    for (GameObjectMapType::iterator l_Iter = p_Map.begin(); l_Iter != p_Map.end(); ++l_Iter)
    {
        if (GameObject* l_Go = l_Iter->getSource())
        {
            if (l_Go->AI() != nullptr)
                l_Go->AI()->MoveInLineOfSight(&i_unit);
        }
    }
}

void AnyReceiverInRange::Visit(PlayerMapType& p_Players)
{
    for (PlayerMapType::iterator l_Iter = p_Players.begin(); l_Iter != p_Players.end(); ++l_Iter)
    {
        Player* l_Target = l_Iter->getSource();

        if (l_Target == Source)
            continue;

        if (!l_Target->InSamePhase(Source))
            continue;

        if (l_Target->GetExactDist2dSq(Source) > RangeSQ)
            continue;

        if (!l_Target->HaveAtClient(Source))
            continue;

        // Send packet to all who are sharing the player's vision
        if (!l_Target->GetSharedVisionList().empty())
        {
            SharedVisionList::const_iterator l_Itr = l_Target->GetSharedVisionList().begin();
            for (; l_Itr != l_Target->GetSharedVisionList().end(); ++l_Itr)
            {
                if ((*l_Itr)->m_seer == l_Target && (*l_Itr) != Source)
                    Receivers->push_back(*l_Itr);
            }
        }

        if (l_Target->m_seer == l_Target || l_Target->GetVehicle())
            Receivers->push_back(l_Target);
    }
}

void AnyReceiverInRange::Visit(CreatureMapType& p_Creatures)
{
    for (CreatureMapType::iterator l_Iter = p_Creatures.begin(); l_Iter != p_Creatures.end(); ++l_Iter)
    {
        Creature* l_Target = l_Iter->getSource();

        if (l_Target->GetSharedVisionList().empty())
            continue;

        if (!l_Target->InSamePhase(Source))
            continue;

        if (l_Target->GetExactDist2dSq(Source) > RangeSQ)
            continue;

        SharedVisionList::const_iterator l_Itr = l_Target->GetSharedVisionList().begin();
        for (; l_Itr != l_Target->GetSharedVisionList().end(); ++l_Itr)
        {
            if ((*l_Itr)->m_seer == l_Target && (*l_Itr) != Source)
                Receivers->push_back(*l_Itr);
        }
    }
}

void AnyReceiverInRange::Visit(DynamicObjectMapType& p_DynamicObjects)
{
    for (DynamicObjectMapType::iterator l_Iter = p_DynamicObjects.begin(); l_Iter != p_DynamicObjects.end(); ++l_Iter)
    {
        DynamicObject* l_Target = l_Iter->getSource();

        if (IS_PLAYER_GUID(l_Target->GetCasterGUID()))
        {
            // Send packet back to the caster if the caster has vision of dynamic object
            Player* l_Caster = (Player*)l_Target->GetCaster();
            if (l_Caster == Source)
                continue;

            if (l_Caster && l_Caster->m_seer == l_Target)
            {
                if (!l_Target->InSamePhase(Source))
                    continue;

                if (l_Target->GetExactDist2dSq(Source) > RangeSQ)
                    continue;

                Receivers->push_back(l_Caster);
            }
        }
    }
}

void MessageDistDeliverer::Visit(PlayerMapType& p_Players)
{
    for (PlayerMapType::iterator l_Iter = p_Players.begin(); l_Iter != p_Players.end(); ++l_Iter)
    {
        Player* l_Target = l_Iter->getSource();

        if (!l_Target->InSamePhase(m_Source))
            continue;

        if (l_Target->GetExactDist2dSq(m_Source) > m_RangeSQ)
            continue;

        // Send packet to all who are sharing the player's vision
        if (!l_Target->GetSharedVisionList().empty())
        {
            SharedVisionList::const_iterator l_Itr = l_Target->GetSharedVisionList().begin();
            for (; l_Itr != l_Target->GetSharedVisionList().end(); ++l_Itr)
            {
                if ((*l_Itr)->m_seer == l_Target)
                    SendPacket(*l_Itr);
            }
        }

        if (l_Target->m_seer == l_Target || l_Target->GetVehicle())
            SendPacket(l_Target);
    }
}

void MessageDistDeliverer::Visit(CreatureMapType& p_Creatures)
{
    for (CreatureMapType::iterator l_Iter = p_Creatures.begin(); l_Iter != p_Creatures.end(); ++l_Iter)
    {
        Creature* l_Target = l_Iter->getSource();

        if (l_Target->GetSharedVisionList().empty())
            continue;

        if (!l_Target->InSamePhase(m_Source))
            continue;

        if (l_Target->GetExactDist2dSq(m_Source) > m_RangeSQ)
            continue;

        SharedVisionList::const_iterator l_Itr = l_Target->GetSharedVisionList().begin();
        for (; l_Itr != l_Target->GetSharedVisionList().end(); ++l_Itr)
        {
            if ((*l_Itr)->m_seer == l_Target)
                SendPacket(*l_Itr);
        }
    }
}

void MessageDistDeliverer::Visit(DynamicObjectMapType& p_DynamicObjects)
{
    for (DynamicObjectMapType::iterator l_Iter = p_DynamicObjects.begin(); l_Iter != p_DynamicObjects.end(); ++l_Iter)
    {
        DynamicObject* l_Target = l_Iter->getSource();

        if (IS_PLAYER_GUID(l_Target->GetCasterGUID()))
        {
            // Send packet back to the caster if the caster has vision of dynamic object
            Player* l_Caster = (Player*)l_Target->GetCaster();
            if (l_Caster && l_Caster->m_seer == l_Target)
            {
                if (!l_Target->InSamePhase(m_Source))
                    continue;

                if (l_Target->GetExactDist2dSq(m_Source) > m_RangeSQ)
                    continue;

                SendPacket(l_Caster);
            }
        }
    }
}

template<class T>
void ObjectUpdater::Visit(GridRefManager<T> &m)
{
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->getSource()->IsInWorld())
            iter->getSource()->Update(i_timeDiff);
    }
}

bool AnyDeadUnitObjectInRangeCheck::operator()(Player* u)
{
    return !u->isAlive() && !u->HasAuraType(SPELL_AURA_GHOST) && i_searchObj->IsWithinDistInMap(u, i_range);
}

bool AnyDeadUnitObjectInRangeCheck::operator()(Corpse* u)
{
    return u->GetType() != CORPSE_BONES && i_searchObj->IsWithinDistInMap(u, i_range);
}

bool AnyDeadUnitObjectInRangeCheck::operator()(Creature* u)
{
    return !u->isAlive() && i_searchObj->IsWithinDistInMap(u, i_range);
}

bool AnyDeadUnitSpellTargetInRangeCheck::operator()(Player* u)
{
    return AnyDeadUnitObjectInRangeCheck::operator()(u) && i_check(u);
}

bool AnyDeadUnitSpellTargetInRangeCheck::operator()(Corpse* u)
{
    return AnyDeadUnitObjectInRangeCheck::operator()(u) && i_check(u);
}

bool AnyDeadUnitSpellTargetInRangeCheck::operator()(Creature* u)
{
    return AnyDeadUnitObjectInRangeCheck::operator()(u) && i_check(u);
}

template void ObjectUpdater::Visit<Creature>(CreatureMapType&);
template void ObjectUpdater::Visit<GameObject>(GameObjectMapType &);
template void ObjectUpdater::Visit<DynamicObject>(DynamicObjectMapType &);
template void ObjectUpdater::Visit<AreaTrigger>(AreaTriggerMapType &);
template void ObjectUpdater::Visit<Conversation>(ConversationMapType &);
template void ObjectUpdater::Visit<EventObject>(EventObjectMapType &);
