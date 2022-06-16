////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "ByteBuffer.h"
#include "WorldPacket.h"
#include "UpdateData.h"
#include "Log.h"
#include "Opcodes.h"
#include "World.h"
#include "zlib.h"

UpdateData::UpdateData(uint16 map) : m_map(map), m_blockCount(0)
{
}

void UpdateData::AddOutOfRangeGUID(std::set<uint64>& guids)
{
    m_outOfRangeGUIDs.insert(guids.begin(), guids.end());
}

void UpdateData::AddOutOfRangeGUID(uint64 guid)
{
    m_outOfRangeGUIDs.insert(guid);
}

void UpdateData::AddUpdateBlock(const ByteBuffer &block)
{
    m_data.append(block);
    ++m_blockCount;
}

bool UpdateData::BuildPacket(WorldPacket* p_Packet)
{
    ASSERT(p_Packet->empty());                                // shouldn't happen

    if (!HasData())
        return false;

    p_Packet->Initialize(SMSG_UPDATE_OBJECT, 4 + 2 + 1 + ((!m_outOfRangeGUIDs.empty()) ? (2 + 4 + (m_outOfRangeGUIDs.size() * (16 + 2))) : 0) + 4 + m_data.wpos() + 4);
    *p_Packet << uint32(m_blockCount);
    *p_Packet << uint16(m_map);

    std::deque<uint64> l_SortedGuids;
    uint32 l_AreaTriggerCount = 0;

    for (uint64 const& l_Guid : m_outOfRangeGUIDs)
    {
        if (IS_AREATRIGGER(l_Guid))
        {
            l_SortedGuids.push_front(l_Guid);
            ++l_AreaTriggerCount;
        }
        else
            l_SortedGuids.push_back(l_Guid);
    }

    if (p_Packet->WriteBit(!m_outOfRangeGUIDs.empty()))
    {
        p_Packet->FlushBits();

        /// Object limit to instantly destroy - Objects after this index on m_outOfRangeGUIDs list gets "smoothly" phased out.
        *p_Packet << uint16(l_AreaTriggerCount);
        *p_Packet << uint32(m_outOfRangeGUIDs.size());

        for (uint64 const& l_Guid : l_SortedGuids)
            p_Packet->appendPackGUID(l_Guid);
    }
    else
        p_Packet->FlushBits();

    uint32_t l_Pos = p_Packet->wpos();
    *p_Packet << uint32(0);

    p_Packet->append(m_data);

    uint32_t l_Size = p_Packet->wpos() - (l_Pos + 4);
    p_Packet->wpos(l_Pos);

    *p_Packet << uint32(l_Size);

    return true;
}

void UpdateData::Clear()
{
    m_data.clear();
    m_data.reserve(50 * 1024);
    m_outOfRangeGUIDs.clear();
    m_blockCount = 0;
    m_map = 0;
}

