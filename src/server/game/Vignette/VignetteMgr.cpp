////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "VignetteMgr.hpp"
#include "ObjectAccessor.h"

namespace Vignette
{
    Manager::Manager(Player const* p_Player)
    {
        m_Owner = p_Player;
    }

    Manager::~Manager()
    {
        m_Owner = nullptr;

        m_VignettesLock.acquire_write();
        for (auto l_Iterator : m_Vignettes)
            delete l_Iterator.second;
        m_Vignettes.clear();
        m_VignettesLock.release();
    }

    Vignette::Entity* Manager::CreateAndAddVignette(VignetteEntry const* p_VignetteEntry, uint32 const p_MapId, Vignette::Type const p_VignetteType, G3D::Vector3 const p_Position, uint64 const p_SourceGuid)
    {
        MapEntry const* l_MapEntry = sMapStore.LookupEntry(p_MapId);
        if (!l_MapEntry)
            return nullptr;

        /// Check for duplicated vignettes - Not in raids
        if (!l_MapEntry->IsRaid())
        {
            m_VignettesLock.acquire_read();
            for (auto l_Vignette : m_Vignettes)
            {
                if (p_MapId == 1803) ///< Bypass this check as more vignettes with this ID are getting created in seething shore
                    continue;

                /// Return if same vignette has already been created
                if (l_Vignette.second->GetVignetteEntry()->Id == p_VignetteEntry->Id)
                {
                    m_VignettesLock.release();
                    return nullptr;
                }
            }
            m_VignettesLock.release();
        }


        Vignette::Entity* l_Vignette = new Vignette::Entity(p_VignetteEntry, p_MapId);
        l_Vignette->Create(p_VignetteType, p_Position, p_SourceGuid);

        m_VignettesLock.acquire_write();
        m_Vignettes.insert(std::make_pair(l_Vignette->GetGuid(), l_Vignette));
        m_VignettesLock.release();
        m_AddedCount.insert(l_Vignette->GetGuid());

        return l_Vignette;
    }

    void Manager::DestroyAndRemoveVignetteByEntry(VignetteEntry const* p_VignetteEntry)
    {
        if (p_VignetteEntry == nullptr)
            return;

        m_VignettesLock.acquire_write();
        for (auto l_Iterator = m_Vignettes.begin(); l_Iterator != m_Vignettes.end();)
        {
            if (l_Iterator->second->GetVignetteEntry()->Id == p_VignetteEntry->Id)
            {
                delete l_Iterator->second;
                m_RemovedCount.insert(l_Iterator->first);
                l_Iterator = m_Vignettes.erase(l_Iterator);
                continue;
            }

            ++l_Iterator;
        }
        m_VignettesLock.release();
    }

    void Manager::DestroyAndRemoveVignettes(std::function<bool(Vignette::Entity* const)> p_Lamba)
    {
        m_VignettesLock.acquire_write();
        for (auto l_Iterator = m_Vignettes.begin(); l_Iterator != m_Vignettes.end();)
        {
            if (p_Lamba(l_Iterator->second))
            {
                delete l_Iterator->second;
                m_RemovedCount.insert(l_Iterator->first);
                l_Iterator = m_Vignettes.erase(l_Iterator);
                continue;
            }

            ++l_Iterator;
        }
        m_VignettesLock.release();
    }

    void Manager::SendVignetteUpdateToClient()
    {
        WorldPacket l_Data(SMSG_VIGNETTE_UPDATE);
        l_Data.WriteBit(false);                                 ///< ForceUpdate

        l_Data << uint32(m_RemovedCount.size());             ///< RemovedCount
        for (auto l_VignetteGuid : m_RemovedCount)
            l_Data.appendPackGUID(l_VignetteGuid);

        m_RemovedCount.clear();

        l_Data << uint32(m_AddedCount.size()); ///< AddedCount
        for (auto l_VignetteGuid : m_AddedCount)
            l_Data.appendPackGUID(l_VignetteGuid);

        uint32 l_VignetteClientDataCount    = 0;
        size_t l_AddedVignetteCountPos = l_Data.wpos();

        l_Data << uint32(l_VignetteClientDataCount);
        for (auto l_VignetteGuid : m_AddedCount)
        {
            m_VignettesLock.acquire_read();
            auto l_FindResult = m_Vignettes.find(l_VignetteGuid);
            if (l_FindResult == m_Vignettes.end())
            {
                m_VignettesLock.release();
                continue;
            }

            m_VignettesLock.release();

            l_VignetteClientDataCount++;

            auto l_Vignette = l_FindResult->second;

            l_Data << float(l_Vignette->GetPosition().x);
            l_Data << float(l_Vignette->GetPosition().y);
            l_Data << float(l_Vignette->GetPosition().z);
            l_Data.appendPackGUID(l_Vignette->GetGuid());
            l_Data << uint32(l_Vignette->GetVignetteEntry()->Id);
            l_Data << uint32(0);                                    ///< Zone restricted (Vignette with flag 0x40)
        }

        l_Data.put<uint32>(l_AddedVignetteCountPos, l_VignetteClientDataCount);
        m_AddedCount.clear();

        l_Data << uint32(m_UpdatedVignette.size());
        for (auto l_VignetteGuid : m_UpdatedVignette)
            l_Data.appendPackGUID(l_VignetteGuid);

        uint32 l_UpdatedVignetteCount    = 0;
        size_t l_UpdatedVignetteCountPos = l_Data.wpos();

        l_Data << uint32(l_UpdatedVignetteCount);
        for (auto l_VignetteGuid : m_UpdatedVignette)
        {
            m_VignettesLock.acquire_read();
            auto l_FindResult = m_Vignettes.find(l_VignetteGuid);
            if (l_FindResult == m_Vignettes.end())
            {
                m_VignettesLock.release();
                continue;
            }

            m_VignettesLock.release();

            l_UpdatedVignetteCount++;

            auto l_Vignette = l_FindResult->second;

            l_Data << float(l_Vignette->GetPosition().x);
            l_Data << float(l_Vignette->GetPosition().y);
            l_Data << float(l_Vignette->GetPosition().z);
            l_Data.appendPackGUID(l_Vignette->GetGuid());
            l_Data << uint32(l_Vignette->GetVignetteEntry()->Id);
            l_Data << uint32(0);                                    ///< Zone restricted (Vignette with flag 0x40)
        }

        l_Data.put<uint32>(l_UpdatedVignetteCountPos, l_UpdatedVignetteCount);
        m_UpdatedVignette.clear();

        m_Owner->GetSession()->SendPacket(&l_Data);
    }

    void Manager::Update()
    {
        m_VignettesLock.acquire_read();
        for (auto l_Iterator : m_Vignettes)
        {
            auto l_Vignette = l_Iterator.second;

            /// - Update the position of the vignette if vignette is linked to a creature
            if (IS_UNIT_GUID(l_Vignette->GeSourceGuid()))
            {
                Creature* l_SourceCreature = sObjectAccessor->FindCreature(l_Vignette->GeSourceGuid());
                if (l_SourceCreature != nullptr)
                    l_Vignette->UpdatePosition(G3D::Vector3(l_SourceCreature->GetPositionX(), l_SourceCreature->GetPositionY(), l_SourceCreature->GetPositionZ()));
            }

            if (l_Vignette->NeedClientUpdate())
            {
                m_UpdatedVignette.insert(l_Vignette->GetGuid());
                l_Vignette->ResetNeedClientUpdate();
            }
        }
        m_VignettesLock.release();

        /// Send update to client if needed
        if (!m_AddedCount.empty() || !m_UpdatedVignette.empty() || !m_RemovedCount.empty())
            SendVignetteUpdateToClient();
    }

    template <class T>
    inline void Manager::OnWorldObjectAppear(T const* p_Target)
    {
        VignetteEntry const* l_VignetteEntry = GetVignetteEntryFromWorldObject(p_Target);
        if (l_VignetteEntry == nullptr)
            return;

        auto l_Type = GetDefaultVignetteTypeFromWorldObject(p_Target);
        auto l_TrackingQuest = GetTrackingQuestIdFromWorldObject(p_Target);

        if (l_TrackingQuest)
        {
            if (m_Owner->IsQuestBitFlaged(GetQuestUniqueBitFlag(l_TrackingQuest)))
                return;

            l_Type = GetTrackingVignetteTypeFromWorldObject(p_Target);
        }

        CreateAndAddVignette(l_VignetteEntry, p_Target->GetMapId(), l_Type, G3D::Vector3(p_Target->GetPositionX(), p_Target->GetPositionY(), p_Target->GetPositionZ()), p_Target->GetGUID());
    }

    template <class T>
    inline void Manager::OnWorldObjectDisappear(T const* p_Target)
    {
        auto l_VignetteEntry = GetVignetteEntryFromWorldObject(p_Target);
        if (l_VignetteEntry == nullptr)
            return;

        DestroyAndRemoveVignettes([p_Target](Vignette::Entity const* p_Vignette) -> bool
        {
            if (p_Vignette->GeSourceGuid() == p_Target->GetGUID()
                && p_Vignette->GetVignetteType() != Vignette::Type::SourceScript)
                return true;

            return false;
        });
    }

    template void Manager::OnWorldObjectDisappear(Corpse const*);
    template void Manager::OnWorldObjectDisappear(Creature const*);
    template void Manager::OnWorldObjectDisappear(GameObject const*);
    template void Manager::OnWorldObjectDisappear(DynamicObject const*);
    template void Manager::OnWorldObjectDisappear(AreaTrigger const*);
    template void Manager::OnWorldObjectDisappear(Conversation const*);
    template void Manager::OnWorldObjectDisappear(EventObject const*);
    template void Manager::OnWorldObjectDisappear(WorldObject const*);
    template void Manager::OnWorldObjectDisappear(Player const*);

    template void Manager::OnWorldObjectAppear(Corpse const*);
    template void Manager::OnWorldObjectAppear(Creature const*);
    template void Manager::OnWorldObjectAppear(GameObject const*);
    template void Manager::OnWorldObjectAppear(DynamicObject const*);
    template void Manager::OnWorldObjectAppear(AreaTrigger const*);
    template void Manager::OnWorldObjectAppear(Conversation const*);
    template void Manager::OnWorldObjectAppear(EventObject const*);
    template void Manager::OnWorldObjectAppear(WorldObject const*);
    template void Manager::OnWorldObjectAppear(Player const*);
}
