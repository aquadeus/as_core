////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef ItemPacketsCommon_h__
#define ItemPacketsCommon_h__

#include "Packet.h"
#include "Define.h"
#include "PacketUtilities.h"
#include "SharedDefines.h"

class Item;
struct ItemDynamicFieldGems;

namespace WorldPackets
{
    namespace WPItem
    {
        struct ItemBonusInstanceData
        {
            uint8 Context = 0;
            std::vector<int32> BonusListIDs;

            bool operator==(ItemBonusInstanceData const& r) const;
            bool operator!=(ItemBonusInstanceData const& r) const { return !(*this == r); }
        };

        struct ItemInstance
        {
            void Initialize(Item const* item);
            void Initialize(ItemDynamicFieldGems const* gem);

            uint32 ItemID = 0;
            uint32 RandomPropertiesSeed = 0;
            uint32 RandomPropertiesID = 0;
            Optional<ItemBonusInstanceData> ItemBonus;
            Optional<CompactArray<int32>> Modifications;

            bool operator==(ItemInstance const& r) const;
            bool operator!=(ItemInstance const& r) const { return !(*this == r); }
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::WPItem::ItemBonusInstanceData const& itemBonusInstanceData);
ByteBuffer& operator >> (ByteBuffer& data, WorldPackets::WPItem::ItemBonusInstanceData& itemBonusInstanceData);

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::WPItem::ItemInstance const& itemInstance);
ByteBuffer& operator >> (ByteBuffer& data, WorldPackets::WPItem::ItemInstance& itemInstance);


#endif // ItemPacketsCommon_h__
