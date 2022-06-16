////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ItemPacketsCommon.h"
#include "Item.h"
#include "Player.h"

bool WorldPackets::WPItem::ItemBonusInstanceData::operator==(ItemBonusInstanceData const& r) const
{
    if (Context != r.Context)
        return false;

    if (BonusListIDs.size() != r.BonusListIDs.size())
        return false;

    return std::is_permutation(BonusListIDs.begin(), BonusListIDs.end(), r.BonusListIDs.begin());
}

void WorldPackets::WPItem::ItemInstance::Initialize(Item const* item)
{
    ItemID               = item->GetEntry();
    RandomPropertiesSeed = item->GetItemSuffixFactor();
    RandomPropertiesID   = item->GetItemRandomPropertyId();
    std::vector<uint32> const& bonusListIds = item->GetDynamicValues(EItemDynamicFields::ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS);
    if (!bonusListIds.empty())
    {
        ItemBonus->BonusListIDs.insert(ItemBonus->BonusListIDs.end(), bonusListIds.begin(), bonusListIds.end());
        ItemBonus->Context = item->GetUInt32Value(ITEM_FIELD_CONTEXT);
    }

    if (uint32 mask = item->GetUInt32Value(ITEM_FIELD_MODIFIERS_MASK))
    {
        for (size_t i = 0; mask != 0; mask >>= 1, ++i)
        {
            if ((mask & 1) != 0)
                Modifications->Insert(i, item->GetModifier(ItemModifiers(i)));
        }
    }
}

void WorldPackets::WPItem::ItemInstance::Initialize(ItemDynamicFieldGems const* gem)
{
    ItemID = gem->ItemId;

    ItemBonusInstanceData bonus;
    bonus.Context = gem->Context;
    for (uint16 bonusListId : gem->BonusListIDs)
        if (bonusListId)
            bonus.BonusListIDs.push_back(bonusListId);

    if (bonus.Context || !bonus.BonusListIDs.empty())
        ItemBonus = bonus;
}

bool WorldPackets::WPItem::ItemInstance::operator==(ItemInstance const& r) const
{
    if (ItemID != r.ItemID || RandomPropertiesID != r.RandomPropertiesID || RandomPropertiesSeed != r.RandomPropertiesSeed)
        return false;

    if (ItemBonus.is_initialized() != r.ItemBonus.is_initialized() || Modifications.is_initialized() != r.Modifications.is_initialized())
        return false;

    if (Modifications.is_initialized() && *Modifications != *r.Modifications)
        return false;

    if (ItemBonus.is_initialized() && *ItemBonus != *r.ItemBonus)
        return false;

    return true;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::WPItem::ItemBonusInstanceData const& itemBonusInstanceData)
{
    data << uint8(itemBonusInstanceData.Context);
    data << uint32(itemBonusInstanceData.BonusListIDs.size());
    for (uint32 bonusID : itemBonusInstanceData.BonusListIDs)
        data << uint32(bonusID);

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::WPItem::ItemBonusInstanceData& itemBonusInstanceData)
{
    uint32 bonusListIdSize;

    data >> itemBonusInstanceData.Context;
    data >> bonusListIdSize;

    for (uint32 i = 0u; i < bonusListIdSize; ++i)
    {
        uint32 bonusId;
        data >> bonusId;
        itemBonusInstanceData.BonusListIDs.push_back(bonusId);
    }

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::WPItem::ItemInstance const& itemInstance)
{
    data << int32(itemInstance.ItemID);
    data << int32(itemInstance.RandomPropertiesSeed);
    data << int32(itemInstance.RandomPropertiesID);

    data.WriteBit(itemInstance.ItemBonus.is_initialized());
    data.WriteBit(itemInstance.Modifications.is_initialized());
    data.FlushBits();

    if (itemInstance.ItemBonus)
        data << *itemInstance.ItemBonus;

    if (itemInstance.Modifications)
        data << *itemInstance.Modifications;

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::WPItem::ItemInstance& itemInstance)
{
    data >> itemInstance.ItemID;
    data >> itemInstance.RandomPropertiesSeed;
    data >> itemInstance.RandomPropertiesID;

    bool hasItemBonus = data.ReadBit();
    bool hasModifications = data.ReadBit();
    data.ResetBitReading();

    if (hasItemBonus)
        data >> *itemInstance.ItemBonus;

    if (hasModifications)
        data >> *itemInstance.Modifications;

    return data;
}

