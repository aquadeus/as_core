////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "CollectionMgr.hpp"
#include "ObjectMgr.h"
#include "Player.h"

/// Constructor
/// @p_Owner : Collection mgr owner player
CollectionMgr::CollectionMgr(WorldSession* p_Owner)
    : m_Owner(p_Owner), m_Appearances()
{
    memset(m_AppearancesBySlot, 0, sizeof(m_AppearancesBySlot));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Get this collection owner player
WorldSession* CollectionMgr::GetOwner() const
{
    return m_Owner;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// When an item is add to the player
/// @p_Item : Added item instance
void CollectionMgr::OnItemAdded(Item* p_Item)
{
    AddItemAppearance(p_Item);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Armor item usable class mask by item subclass
uint32 const PlayerClassByArmorSubclass[MAX_ITEM_SUBCLASS_ARMOR] =
{
    CLASSMASK_ALL_PLAYABLE,                                                                                                                 ///< ITEM_SUBCLASS_ARMOR_MISCELLANEOUS
    (1 << (CLASS_PRIEST     - 1)) | (1 << (CLASS_MAGE       - 1)) | (1 << (CLASS_WARLOCK        - 1)),                                      ///< ITEM_SUBCLASS_ARMOR_CLOTH
    (1 << (CLASS_ROGUE      - 1)) | (1 << (CLASS_MONK       - 1)) | (1 << (CLASS_DRUID          - 1)) | (1 << (CLASS_DEMON_HUNTER - 1)),    ///< ITEM_SUBCLASS_ARMOR_LEATHER
    (1 << (CLASS_HUNTER     - 1)) | (1 << (CLASS_SHAMAN     - 1)),                                                                          ///< ITEM_SUBCLASS_ARMOR_MAIL
    (1 << (CLASS_WARRIOR    - 1)) | (1 << (CLASS_PALADIN    - 1)) | (1 << (CLASS_DEATH_KNIGHT   - 1)),                                      ///< ITEM_SUBCLASS_ARMOR_PLATE
    CLASSMASK_ALL_PLAYABLE,                                                                                                                 ///< ITEM_SUBCLASS_ARMOR_BUCKLER
    (1 << (CLASS_WARRIOR    - 1)) | (1 << (CLASS_PALADIN    - 1)) | (1 << (CLASS_SHAMAN         - 1)),                                      ///< ITEM_SUBCLASS_ARMOR_SHIELD
    (1 << (CLASS_PALADIN    - 1)),                                                                                                          ///< ITEM_SUBCLASS_ARMOR_LIBRAM
    (1 << (CLASS_DRUID      - 1)),                                                                                                          ///< ITEM_SUBCLASS_ARMOR_IDOL
    (1 << (CLASS_SHAMAN     - 1)),                                                                                                          ///< ITEM_SUBCLASS_ARMOR_TOTEM
    (1 << (CLASS_DEATH_KNIGHT - 1)),                                                                                                        ///< ITEM_SUBCLASS_ARMOR_SIGIL
    (1 << (CLASS_PALADIN    - 1)) | (1 << (CLASS_DEATH_KNIGHT - 1)) | (1 << (CLASS_SHAMAN - 1)) | (1 << (CLASS_DRUID - 1)),                 ///< ITEM_SUBCLASS_ARMOR_RELIC
};

struct DynamicBitsetBlockOutputIterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
    explicit DynamicBitsetBlockOutputIterator(std::function<void(uint32)>&& action) : _action(std::forward<std::function<void(uint32)>>(action)) { }

    DynamicBitsetBlockOutputIterator& operator=(uint32 value)
    {
        _action(value);
        return *this;
    }

    DynamicBitsetBlockOutputIterator& operator*() { return *this; }
    DynamicBitsetBlockOutputIterator& operator++() { return *this; }
    DynamicBitsetBlockOutputIterator operator++(int) { return *this; }

private:
    std::function<void(uint32)> _action;
};

/// Init known appearance in player update fields
void CollectionMgr::LoadItemAppearances()
{
    m_AppearancesLock.lock();
    boost::to_block_range(m_Appearances, DynamicBitsetBlockOutputIterator([this](uint32 p_BlockValue)
    {
        m_Owner->GetPlayer()->AddDynamicValue(PLAYER_DYNAMIC_FIELD_TRANSMOG, p_BlockValue);
    }));
    m_AppearancesLock.unlock();

    for (auto l_It = m_TemporaryAppearances.begin(); l_It != m_TemporaryAppearances.end(); ++l_It)
        m_Owner->GetPlayer()->AddDynamicValue(PLAYER_DYNAMIC_FIELD_CONDITIONAL_TRANSMOG, l_It->first);

    CalculateAppearancesBySlot();

    /// Update criterias.
    /// Use delayed event because criterias cannot be sent while the player is loading.
    m_Owner->GetPlayer()->AddDelayedEvent([&] () -> void
    {
        for (uint32 l_Slot = 0; l_Slot < EQUIPMENT_SLOT_END; ++l_Slot)
            m_Owner->GetPlayer()->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT, l_Slot);
    }, 1);
}

/// Load from database
/// @p_KnownAppearances     : Known appearance result set
/// @p_FavoriteAppearances  : Favorite ones
void CollectionMgr::LoadAccountItemAppearances(PreparedQueryResult p_KnownAppearances, PreparedQueryResult p_FavoriteAppearances)
{
    m_AppearancesLock.lock();
    if (p_KnownAppearances)
    {
        std::vector<uint32> l_Blocks;
        do
        {
            Field* l_Fields = p_KnownAppearances->Fetch();
            uint16 l_BlobIndex = l_Fields[0].GetUInt16();
            if (l_BlobIndex >= l_Blocks.size())
                l_Blocks.resize(l_BlobIndex + 1);

            l_Blocks[l_BlobIndex] = l_Fields[1].GetUInt32();

        } while (p_KnownAppearances->NextRow());

        m_Appearances.init_from_block_range(l_Blocks.begin(), l_Blocks.end());
    }

    if (p_FavoriteAppearances)
    {
        do
        {
            m_FavoriteAppearances[p_FavoriteAppearances->Fetch()[0].GetUInt32()] = FavoriteAppearanceState::Unchanged;
        } while (p_FavoriteAppearances->NextRow());
    }

    for (uint32 l_Current : m_HiddenAppearanceItems)
    {
        ItemModifiedAppearanceEntry const* l_HiddenAppearance = GetItemModifiedAppearance(l_Current, 0);
        ASSERT(l_HiddenAppearance);

        if (m_Appearances.size() <= l_HiddenAppearance->ID)
            m_Appearances.resize(l_HiddenAppearance->ID + 1);

        m_Appearances.set(l_HiddenAppearance->ID);
    }
    m_AppearancesLock.unlock();
}

/// Save player appearances into the database
/// @p_Transaction : Save transaction
void CollectionMgr::SaveAccountItemAppearances(SQLTransaction& p_Transaction)
{
    if (!sWorld->CanBeSaveInLoginDatabase())
        return;

    uint16 l_BlockIndex = 0;

    m_AppearancesLock.lock();
    boost::to_block_range(m_Appearances, DynamicBitsetBlockOutputIterator([this, &l_BlockIndex, p_Transaction](uint32 p_BlockValue)
    {
        if (p_BlockValue) ///< This table is only appended/bits are set (never cleared) so don't save empty blocks
        {
            PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_WARDROBES);
            l_Statement->setUInt32(0, GetOwner()->GetAccountId());
            l_Statement->setUInt16(1, l_BlockIndex);
            l_Statement->setUInt32(2, p_BlockValue);
            p_Transaction->Append(l_Statement);
        }

        ++l_BlockIndex;
    }));
    m_AppearancesLock.unlock();

    PreparedStatement* l_Statement;
    for (auto l_It = m_FavoriteAppearances.begin(); l_It != m_FavoriteAppearances.end();)
    {
        switch (l_It->second)
        {
            case FavoriteAppearanceState::New:
                l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_WARDROBE_FAVORITE);
                l_Statement->setUInt32(0, m_Owner->GetAccountId());
                l_Statement->setUInt32(1, l_It->first);
                p_Transaction->Append(l_Statement);
                l_It->second = FavoriteAppearanceState::Unchanged;
                ++l_It;
                break;

            case FavoriteAppearanceState::Removed:
                l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_WARDROBE_FAVORITE);
                l_Statement->setUInt32(0, m_Owner->GetAccountId());
                l_Statement->setUInt32(1, l_It->first);
                p_Transaction->Append(l_Statement);
                l_It = m_FavoriteAppearances.erase(l_It);
                break;

            case FavoriteAppearanceState::Unchanged:
                ++l_It;
                break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Add an item appearance
/// @p_Item : Source item
void CollectionMgr::AddItemAppearance(Item* p_Item)
{
    if (!p_Item->IsSoulBound())
        return;

    ItemModifiedAppearanceEntry const* l_ItemModifiedAppearance = GetItemModifiedAppearance(p_Item->GetEntry(), p_Item->GetAppearanceModId());
    if (!CanAddAppearance(l_ItemModifiedAppearance))
        return;

    if (p_Item->GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS) & (ITEM_FIELD_FLAG_BOP_TRADEABLE | ITEM_FIELD_FLAG_REFUNDABLE))
    {
        AddTemporaryAppearance(p_Item->GetGUID(), l_ItemModifiedAppearance);
        return;
    }

    AddItemAppearance(l_ItemModifiedAppearance);
}

/// Add an item appearance
/// @p_ItemID           : Item entry
/// @p_AppearanceModID  : Appearance modifier ID
void CollectionMgr::AddItemAppearance(uint32 p_ItemID, uint32 p_AppearanceModID /*= 0*/)
{
    ItemModifiedAppearanceEntry const* l_ItemModifiedAppearance = GetItemModifiedAppearance(p_ItemID, p_AppearanceModID);

    if (!CanAddAppearance(l_ItemModifiedAppearance))
        return;

    AddItemAppearance(l_ItemModifiedAppearance);
}

/// Has unlocked a specific appearance
/// @p_ItemModifiedAppearanceID : Appearance ID
/// Returns pair<HasAppearance, IsTemporary>
std::pair<bool, bool> CollectionMgr::HasItemAppearance(uint32 p_ItemModifiedAppearanceID) const
{
    std::lock_guard<std::mutex> l_Lock(m_AppearancesLock);
    if (p_ItemModifiedAppearanceID < m_Appearances.size() && m_Appearances.test(p_ItemModifiedAppearanceID))
        return { true, false };

    if (m_TemporaryAppearances.find(p_ItemModifiedAppearanceID) != m_TemporaryAppearances.end())
        return { true, true };

    return { false,false };
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Remove an temporary appearance
/// @p_Item : Appearance source
void CollectionMgr::RemoveTemporaryAppearance(Item* p_Item)
{
    ItemModifiedAppearanceEntry const* l_ItemModifiedAppearance = GetItemModifiedAppearance(p_Item->GetEntry(), p_Item->GetAppearanceModId());
    if (!l_ItemModifiedAppearance)
        return;

    auto l_It = m_TemporaryAppearances.find(l_ItemModifiedAppearance->ID);
    if (l_It == m_TemporaryAppearances.end())
        return;

    l_It->second.erase(p_Item->GetGUID());
    if (l_It->second.empty())
    {
        m_Owner->GetPlayer()->RemoveDynamicValue(PLAYER_DYNAMIC_FIELD_CONDITIONAL_TRANSMOG, l_ItemModifiedAppearance->ID);
        m_TemporaryAppearances.erase(l_It);
    }
}

/// Check if the player unlocked a specific appearance
/// returns pair<HasAppearance, IsTemporary>
/// @p_ItemModifiedAppearanceId : Appearance ID
std::unordered_set<uint64> CollectionMgr::GetItemsProvidingTemporaryAppearance(uint32 p_ItemModifiedAppearanceID) const
{
    auto l_It = m_TemporaryAppearances.find(p_ItemModifiedAppearanceID);
    if (l_It != m_TemporaryAppearances.end())
        return l_It->second;

    return std::unordered_set<uint64>();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Flag an appearance as favorite
/// @p_ItemModifiedAppearanceID : Appearance ID
/// @p_Apply                    : Set or unset favorite flag
void CollectionMgr::SetAppearanceIsFavorite(uint32 p_ItemModifiedAppearanceID, bool p_Apply)
{
    auto l_It = m_FavoriteAppearances.find(p_ItemModifiedAppearanceID);
    if (p_Apply)
    {
        if (l_It == m_FavoriteAppearances.end())
            m_FavoriteAppearances[p_ItemModifiedAppearanceID] = FavoriteAppearanceState::New;
        else if (l_It->second == FavoriteAppearanceState::Removed)
            l_It->second = FavoriteAppearanceState::Unchanged;
        else
            return;
    }
    else if (l_It != m_FavoriteAppearances.end())
    {
        if (l_It->second == FavoriteAppearanceState::New)
            m_FavoriteAppearances.erase(p_ItemModifiedAppearanceID);
        else
            l_It->second = FavoriteAppearanceState::Removed;
    }
    else
        return;

    std::unordered_set<uint32> l_Appearances;
    l_Appearances.emplace(p_ItemModifiedAppearanceID);

    SendTransmogCollectionUpdate(false, p_Apply, l_Appearances);
}

/// Send favorite appearance list to the client
void CollectionMgr::SendFavoriteAppearances() const
{
    std::unordered_set<uint32> l_Appearances;

    for (auto l_It = m_FavoriteAppearances.begin(); l_It != m_FavoriteAppearances.end(); ++l_It)
    {
        if (l_It->second != FavoriteAppearanceState::Removed)
            l_Appearances.emplace(l_It->first);
    }

    SendTransmogCollectionUpdate(true, false, l_Appearances);
}

uint32 CollectionMgr::GetCountOfAppearancesBySlot(uint32 l_EquipmentSlot) const
{
    if (l_EquipmentSlot >= EquipmentSlots::EQUIPMENT_SLOT_START && l_EquipmentSlot < EquipmentSlots::EQUIPMENT_SLOT_END)
        return m_AppearancesBySlot[l_EquipmentSlot];

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Can we add a new appearance for the player
/// @p_ItemModifiedAppearance : Appearance to add
bool CollectionMgr::CanAddAppearance(ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance) const
{
    if (!p_ItemModifiedAppearance)
        return false;

    if (p_ItemModifiedAppearance->SourceType == 6 || p_ItemModifiedAppearance->SourceType == 9)
        return false;

    if (!sItemSearchNameStore.LookupEntry(p_ItemModifiedAppearance->ItemID))
        return false;

    ItemTemplate const* l_ItemTemplate = sObjectMgr->GetItemTemplate(p_ItemModifiedAppearance->ItemID);
    if (!l_ItemTemplate)
        return false;

    if (m_Owner->GetPlayer() == nullptr)
        sLog->outAshran("CollectionMgr::CanAddAppearance => m_Owner->GetPlayer() == nullptr, AccountID: %u", m_Owner->GetAccountId());

    if (m_Owner->GetPlayer()->CanUseItem(l_ItemTemplate) != EQUIP_ERR_OK)
        return false;

    if (l_ItemTemplate->Flags2 & int32(ItemFlagsEX::NO_SOURCE_FOR_ITEM_VISUAL) || l_ItemTemplate->Quality == ITEM_QUALITY_ARTIFACT)
        return false;

    switch (l_ItemTemplate->Class)
    {
        case ITEM_CLASS_WEAPON:
        {
            if (!(m_Owner->GetPlayer()->GetWeaponProficiency() & (1 << l_ItemTemplate->SubClass)))
                return false;

            if (l_ItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_EXOTIC ||
                l_ItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_EXOTIC2 ||
                l_ItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_MISCELLANEOUS ||
                l_ItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_THROWN ||
                l_ItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_SPEAR ||
                l_ItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
                return false;

            break;
        }

        case ITEM_CLASS_ARMOR:
        {
            switch (l_ItemTemplate->InventoryType)
            {
                case INVTYPE_BODY:
                case INVTYPE_SHIELD:
                case INVTYPE_CLOAK:
                case INVTYPE_TABARD:
                case INVTYPE_HOLDABLE:
                    break;

                case INVTYPE_HEAD:
                case INVTYPE_SHOULDERS:
                case INVTYPE_CHEST:
                case INVTYPE_WAIST:
                case INVTYPE_LEGS:
                case INVTYPE_FEET:
                case INVTYPE_WRISTS:
                case INVTYPE_HANDS:
                case INVTYPE_ROBE:
                    if (l_ItemTemplate->SubClass == ITEM_SUBCLASS_ARMOR_MISCELLANEOUS)
                        return false;
                    break;

                default:
                    return false;
            }

            if (l_ItemTemplate->InventoryType != INVTYPE_CLOAK)
                if (!(PlayerClassByArmorSubclass[l_ItemTemplate->SubClass] & m_Owner->GetPlayer()->getClassMask()))
                    return false;

            break;
        }

        default:
            return false;
    }

    if (l_ItemTemplate->Quality < ITEM_QUALITY_UNCOMMON)
        if (!(l_ItemTemplate->Flags2 & int32(ItemFlagsEX::IGNORE_QUALITY_FOR_ITEM_VISUAL_SOURCE) || !(l_ItemTemplate->Flags3 & int32(ItemFlagsEX2::ACTS_AS_TRANSMOG_HIDDEN_VISUAL_OPTION))))
            return false;

    std::lock_guard<std::mutex> l_Lock(m_AppearancesLock);

    if (p_ItemModifiedAppearance->ID < m_Appearances.size() && m_Appearances.test(p_ItemModifiedAppearance->ID))
        return false;

    return true;
}

/// Add item appearance and update player fields
/// @p_ItemModifiedAppearance : Appearance to add
void CollectionMgr::AddItemAppearance(ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance)
{
    m_AppearancesLock.lock();
    if (m_Appearances.size() <= p_ItemModifiedAppearance->ID)
    {
        uint32 l_NumBlocks = m_Appearances.num_blocks();
        m_Appearances.resize(p_ItemModifiedAppearance->ID + 1);
        l_NumBlocks = m_Appearances.num_blocks() - l_NumBlocks;

        while (l_NumBlocks--)
            m_Owner->GetPlayer()->AddDynamicValue(PLAYER_DYNAMIC_FIELD_TRANSMOG, 0);
    }

    m_Appearances.set(p_ItemModifiedAppearance->ID);
    m_AppearancesLock.unlock();

    uint32 l_BlockIndex = p_ItemModifiedAppearance->ID / 32;
    uint32 l_BitIndex   = p_ItemModifiedAppearance->ID % 32;

    uint32 l_CurrentMask = m_Owner->GetPlayer()->GetDynamicValue(PLAYER_DYNAMIC_FIELD_TRANSMOG, l_BlockIndex);
    m_Owner->GetPlayer()->SetDynamicValue(PLAYER_DYNAMIC_FIELD_TRANSMOG, l_BlockIndex, l_CurrentMask | (1 << l_BitIndex));

    auto l_TemporaryAppearance = m_TemporaryAppearances.find(p_ItemModifiedAppearance->ID);
    if (l_TemporaryAppearance != m_TemporaryAppearances.end())
    {
        m_Owner->GetPlayer()->RemoveDynamicValue(PLAYER_DYNAMIC_FIELD_CONDITIONAL_TRANSMOG, p_ItemModifiedAppearance->ID);
        m_TemporaryAppearances.erase(l_TemporaryAppearance);
    }

    AfterAppearanceAdded(p_ItemModifiedAppearance);
}

/// Add item temp appearance and update player fields
/// @p_ItemGUID               : Source item GUID
/// @p_ItemModifiedAppearance : Appearance to add
void CollectionMgr::AddTemporaryAppearance(uint64 p_ItemGUID, ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance)
{
    std::unordered_set<uint64>& l_ItemsWithAppearance = m_TemporaryAppearances[p_ItemModifiedAppearance->ID];

    if (l_ItemsWithAppearance.empty())
        m_Owner->GetPlayer()->AddDynamicValue(PLAYER_DYNAMIC_FIELD_CONDITIONAL_TRANSMOG, p_ItemModifiedAppearance->ID);

    l_ItemsWithAppearance.insert(p_ItemGUID);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Send transmogrification collection update
/// @p_FullUpdate       : Is this update a full one
/// @p_IsSetFavorite    : Set or unset favorite flag
/// @p_Appearances      : List of appearances
void CollectionMgr::SendTransmogCollectionUpdate(bool p_FullUpdate, bool p_IsSetFavorite, std::unordered_set<uint32> const& p_Appearances) const
{
    WorldPacket l_Data(SMSG_TRANSMOG_COLLECTION_UPDATE, 1 + 4 + (4 * p_Appearances.size()));
    l_Data.WriteBit(p_FullUpdate);
    l_Data.WriteBit(p_IsSetFavorite);
    l_Data.FlushBits();

    l_Data << uint32(p_Appearances.size());

    for (uint32 l_Current : p_Appearances)
        l_Data << uint32(l_Current);

    m_Owner->SendPacket(&l_Data);
}

/// Recalculate appearances by equipment slots
void CollectionMgr::CalculateAppearancesBySlot()
{
    memset(m_AppearancesBySlot, 0, sizeof(m_AppearancesBySlot));

    for (uint32 k = 0; k < sItemModifiedAppearanceStore.GetNumRows(); ++k)
    {
        auto l_ItemModifiedAppearance = sItemModifiedAppearanceStore.LookupEntry(k);
        if (!l_ItemModifiedAppearance)
            continue;

        bool l_HasAppearance, l_IsTemporary;
        std::tie(l_HasAppearance, l_IsTemporary) = HasItemAppearance(l_ItemModifiedAppearance->ID);
        if (!l_HasAppearance)
            continue;

        if (m_HiddenAppearanceItems.find(l_ItemModifiedAppearance->ItemID) != m_HiddenAppearanceItems.end())
            continue;

        if (auto l_ItemTemplate = sObjectMgr->GetItemTemplate(l_ItemModifiedAppearance->ItemID))
        {
            auto l_Slot = m_Owner->GetPlayer()->GetGuessedEquipSlot(l_ItemTemplate);
            if (l_Slot >= EQUIPMENT_SLOT_START && l_Slot < EQUIPMENT_SLOT_END)
            {
                ++m_AppearancesBySlot[l_Slot];
            }
        }
    }
}

/// Process some functions if appearances has been added
/// @p_ItemModifiedAppearance : Appearance added
void CollectionMgr::AfterAppearanceAdded(ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance)
{
    if (!m_Owner->PlayerLoading())
    {
        if (auto l_ItemTemplate = sObjectMgr->GetItemTemplate(p_ItemModifiedAppearance->ItemID))
        {
            auto l_Slot = m_Owner->GetPlayer()->GetGuessedEquipSlot(l_ItemTemplate);
            if (l_Slot >= EQUIPMENT_SLOT_START && l_Slot < EQUIPMENT_SLOT_END)
            {
                ++m_AppearancesBySlot[l_Slot];

                m_Owner->GetPlayer()->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT, l_Slot);
            }
        }
    }
}
