////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "WorldSession.h"
#include "World.h"
#include <boost/dynamic_bitset.hpp>

struct ItemModifiedAppearanceEntry;

class CollectionMgr/* Mgr == Manager <-> HardCPP */
{
    public:
        /// Constructor
        /// @p_Owner : Collection mgr owner player
        explicit CollectionMgr(WorldSession* p_Owner);

        /// Get this collection owner player
        WorldSession* GetOwner() const;

        /// When an item is add to the player
        /// @p_Item : Added item instance
        void OnItemAdded(Item* p_Item);

    /// Wardrobe
    public:
        /// Appearance favorite state
        enum class FavoriteAppearanceState
        {
            New,
            Removed,
            Unchanged
        };

        /// Init known appearance in player update fields
        void LoadItemAppearances();
        /// Load from database
        /// @p_KnownAppearances     : Known appearance result set
        /// @p_FavoriteAppearances  : Favorite ones
        void LoadAccountItemAppearances(PreparedQueryResult p_KnownAppearances, PreparedQueryResult p_FavoriteAppearances);
        /// Save player appearances into the database
        /// @p_Transaction : Save transaction
        void SaveAccountItemAppearances(SQLTransaction& p_Transaction);

        /// Add an item appearance
        /// @p_Item : Source item
        void AddItemAppearance(Item* p_Item);
        /// Add an item appearance
        /// @p_ItemID           : Item entry
        /// @p_AppearanceModID  : Appearance modifier ID
        void AddItemAppearance(uint32 p_ItemID, uint32 p_AppearanceModID = 0);
        /// Has unlocked a specific appearance
        /// @p_ItemModifiedAppearanceID : Appearance ID
        /// Returns pair<HasAppearance, IsTemporary>
        std::pair<bool, bool> HasItemAppearance(uint32 p_ItemModifiedAppearanceID) const;

        /// Remove an temporary appearance
        /// @p_Item : Appearance source
        void RemoveTemporaryAppearance(Item* p_Item);
        /// Check if the player unlocked a specific appearance
        /// @p_ItemModifiedAppearanceID : Appearance ID
        std::unordered_set<uint64> GetItemsProvidingTemporaryAppearance(uint32 p_ItemModifiedAppearanceID) const;

        /// Flag an appearance as favorite
        /// @p_ItemModifiedAppearanceID : Appearance ID
        /// @p_Apply                    : Set or unset favorite flag
        void SetAppearanceIsFavorite(uint32 p_ItemModifiedAppearanceID, bool p_Apply);
        /// Send favorite appearance list to the client
        void SendFavoriteAppearances() const;

        /// How many items by slot the player has
        /// @l_EquipmentSlot : Equipment slot
        /// Returns count of items
        uint32 GetCountOfAppearancesBySlot(uint32 l_EquipmentSlot) const;

    /// Wardrobe
    private:
        /// Can we add a new appearance for the player
        /// @p_ItemModifiedAppearance : Appearance to add
        bool CanAddAppearance(ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance) const;
        /// Add item appearance and update player fields
        /// @p_ItemModifiedAppearance : Appearance to add
        void AddItemAppearance(ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance);
        /// Add item temp appearance and update player fields
        /// @p_ItemGUID               : Source item GUID
        /// @p_ItemModifiedAppearance : Appearance to add
        void AddTemporaryAppearance(uint64 p_ItemGUID, ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance);

        /// Send transmogrification collection update
        /// @p_FullUpdate       : Is this update a full one
        /// @p_IsSetFavorite    : Set or unset favorite flag
        /// @p_Appearances      : List of appearances
        void SendTransmogCollectionUpdate(bool p_FullUpdate, bool p_IsSetFavorite, std::unordered_set<uint32> const& p_Appearances) const;

        /// Recalculate appearances by equipment slots
        void CalculateAppearancesBySlot();

        /// Process some functions if appearances has been added
        /// @p_ItemModifiedAppearance : Appearance added
        void AfterAppearanceAdded(ItemModifiedAppearanceEntry const* p_ItemModifiedAppearance);

    private:
        /// Collection owner player
        WorldSession* m_Owner;

        /// Known appearance bitset
        boost::dynamic_bitset<uint32> m_Appearances;
        /// Lock of appearance bitset to avoid race issues
        mutable std::mutex m_AppearancesLock;
        /// Temporary appearance
        std::unordered_map<uint32, std::unordered_set<uint64>> m_TemporaryAppearances;
        /// Favorite ones
        std::unordered_map<uint32, FavoriteAppearanceState> m_FavoriteAppearances;
        /// Static item appearances known by every player
        std::set<uint32> m_HiddenAppearanceItems =
        {
            134110, ///< Hidden Helm
            134111, ///< Hidden Cloak
            134112, ///< Hidden Shoulder
            142503, ///< Hidden Shirt
            142504, ///< Hidden Tabard
            143539  ///< Hidden Belt
        };
        /// Count of appearances by equipment slot to prevent ingame calculations
        uint32 m_AppearancesBySlot[EquipmentSlots::EQUIPMENT_SLOT_END];


};