////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "WorldSession.h"
#include "CollectionMgr.hpp"
#include "ObjectMgr.h"
#include "Player.h"

void WorldSession::HandleTransmogrifyItems(WorldPacket& p_Packet)
{
    uint64 l_NpcGUID = 0;
    uint32 l_ItemCount = 0;

    p_Packet >> l_ItemCount;
    p_Packet.readPackGUID(l_NpcGUID);

    if (l_ItemCount < EquipmentSlots::EQUIPMENT_SLOT_START || l_ItemCount >= EquipmentSlots::EQUIPMENT_SLOT_END)
    {
        p_Packet.rfinish();
        return;
    }

    /// Validate
    if (!m_Player->GetNPCIfCanInteractWith(l_NpcGUID, NPCFlags::UNIT_NPC_FLAG_TRANSMOGRIFIER))
    {
        p_Packet.rfinish();
        return;
    }

    struct TransmogrifyItem
    {
        uint32 ItemModifiedAppearanceID;
        uint32 SlotID;
        uint32 SpellItemEnchantmentID;

        TransmogrifyItem()
        {
            ItemModifiedAppearanceID    = 0;
            SlotID                      = 0;
            SpellItemEnchantmentID      = 0;
        }

        TransmogrifyItem(WorldPacket& p_Packet)
        {
            p_Packet >> ItemModifiedAppearanceID;
            p_Packet >> SlotID;
            p_Packet >> SpellItemEnchantmentID;
        }
    };

    std::vector<TransmogrifyItem> l_TransmogrifyItems(l_ItemCount, TransmogrifyItem());
    for (uint32 l_I = 0; l_I < l_ItemCount; ++l_I)
        l_TransmogrifyItems[l_I] = TransmogrifyItem(p_Packet);

    bool l_CurrentSpecOnly = p_Packet.ReadBit();

    enum eHiddenTemplates
    {
        HiddenShoulders = 134112,
        HiddenHelm      = 134110,
        HiddenCloak     = 134111,
        HiddenShirt     = 142503,
        HiddenTabard    = 142504,
        HiddenBelt      = 143539
    };

    std::map<uint32, eHiddenTemplates> l_HiddenSlots =
    {
        { EquipmentSlots::EQUIPMENT_SLOT_HEAD,      eHiddenTemplates::HiddenHelm        },
        { EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS, eHiddenTemplates::HiddenShoulders   },
        { EquipmentSlots::EQUIPMENT_SLOT_BACK,      eHiddenTemplates::HiddenCloak       },
        { EquipmentSlots::EQUIPMENT_SLOT_BODY,      eHiddenTemplates::HiddenShirt       },
        { EquipmentSlots::EQUIPMENT_SLOT_TABARD,    eHiddenTemplates::HiddenTabard      },
        { EquipmentSlots::EQUIPMENT_SLOT_WAIST,     eHiddenTemplates::HiddenBelt        }
    };

    std::unordered_map<Item*, uint32> l_TransmogItems;
    std::unordered_map<Item*, uint32> l_IllusionItems;

    std::vector<Item*> l_ResetAppearanceItems;
    std::vector<Item*> l_ResetIllusionItems;
    std::vector<uint32> l_BindAppearances;

    int64 l_Cost = 0;

    for (TransmogrifyItem l_TransmogrifyItem : l_TransmogrifyItems)
    {
        /// Slot of the transmogrified item
        if (l_TransmogrifyItem.SlotID < EquipmentSlots::EQUIPMENT_SLOT_START || l_TransmogrifyItem.SlotID >= EquipmentSlots::EQUIPMENT_SLOT_END)
            return;

        /// Transmogrified item
        Item* l_ItemTransmogrified = m_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_TransmogrifyItem.SlotID);
        if (!l_ItemTransmogrified)
            return;

        if (l_TransmogrifyItem.ItemModifiedAppearanceID)
        {
            ItemModifiedAppearanceEntry const* l_ItemModAppearance = sItemModifiedAppearanceStore.LookupEntry(l_TransmogrifyItem.ItemModifiedAppearanceID);
            if (!l_ItemModAppearance)
                return;

            bool l_HasAppearance, l_IsTemporary;
            std::tie(l_HasAppearance, l_IsTemporary) = GetCollectionMgr()->HasItemAppearance(l_TransmogrifyItem.ItemModifiedAppearanceID);
            if (!l_HasAppearance)
                return;

            ItemTemplate const* l_ItemTemplate = sObjectMgr->GetItemTemplate(l_ItemModAppearance->ItemID);
            if (m_Player->CanUseItem(l_ItemTemplate) != InventoryResult::EQUIP_ERR_OK)
                return;

            /// Don't check transmogrification rules for hidden display
            if (l_HiddenSlots.find(l_TransmogrifyItem.SlotID) == l_HiddenSlots.end() || l_HiddenSlots[l_TransmogrifyItem.SlotID] != l_TransmogrifyItem.ItemModifiedAppearanceID)
            {
                if (l_ItemTemplate->ItemId == l_ItemTransmogrified->GetTemplate()->ItemId && l_ItemModAppearance->AppearanceModID == l_ItemTransmogrified->GetVisibleAppearanceModID(m_Player))
                    return;

                /// Validate transmogrification
                if (!Item::CanTransmogrifyItemWithItem(l_ItemTemplate, l_ItemTransmogrified->GetTemplate()))
                    return;
            }

            l_TransmogItems[l_ItemTransmogrified] = l_TransmogrifyItem.ItemModifiedAppearanceID;
            if (l_IsTemporary)
                l_BindAppearances.push_back(l_TransmogrifyItem.ItemModifiedAppearanceID);

            /// Add cost
            if (l_ItemTransmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS) != l_TransmogrifyItem.ItemModifiedAppearanceID)
                l_Cost += l_ItemTransmogrified->GetSellPrice(m_Player);
        }
        else
            l_ResetAppearanceItems.push_back(l_ItemTransmogrified);

        if (l_TransmogrifyItem.ItemModifiedAppearanceID && l_TransmogrifyItem.SpellItemEnchantmentID)
        {
            if (l_TransmogrifyItem.SlotID != EquipmentSlots::EQUIPMENT_SLOT_MAINHAND && l_TransmogrifyItem.SlotID != EquipmentSlots::EQUIPMENT_SLOT_OFFHAND)
                return;

            SpellItemEnchantmentEntry const* l_Illusion = sSpellItemEnchantmentStore.LookupEntry(l_TransmogrifyItem.SpellItemEnchantmentID);
            if (!l_Illusion)
                return;

            if (!l_Illusion->ItemVisual || !(l_Illusion->Flags & ENCHANTMENT_COLLECTABLE))
                return;

            if (l_Illusion->PlayerConditionID && !m_Player->EvalPlayerCondition(l_Illusion->PlayerConditionID).first)
                return;

            if (l_Illusion->m_ScalingClassRestricted > 0 && uint8(l_Illusion->m_ScalingClassRestricted) != m_Player->getClass())
                return;

            l_IllusionItems[l_ItemTransmogrified] = l_TransmogrifyItem.SpellItemEnchantmentID;
            if (l_ItemTransmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS) != l_TransmogrifyItem.SpellItemEnchantmentID)
                l_Cost += l_Illusion->TransmogCost;
        }
        else
            l_ResetIllusionItems.push_back(l_ItemTransmogrified);
    }

    int32 l_CostPct = 0;
    Unit::AuraEffectList const& l_ModifyPriceList = m_Player->GetAuraEffectsByType(AuraType::SPELL_AURA_REDUCE_ITEM_MODIFY_COST);
    for (Unit::AuraEffectList::const_iterator l_I = l_ModifyPriceList.begin(); l_I != l_ModifyPriceList.end(); ++l_I)
        l_CostPct += (*l_I)->GetAmount();

    AddPct(l_Cost, l_CostPct);

    /// 0 cost in case of reverting look
    if (l_Cost)
    {
        if (!m_Player->HasEnoughMoney(l_Cost))
            return;

        m_Player->ModifyMoney(-l_Cost, "HandleTransmogrifyItems");
    }

    /// Everything is fine, proceed to transmogrification
    for (auto& l_TransmogPair : l_TransmogItems)
    {
        Item* l_Transmogrified = l_TransmogPair.first;

        if (!l_CurrentSpecOnly)
        {
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS, l_TransmogPair.second);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1, 0);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2, 0);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3, 0);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4, 0);
        }
        else
        {
            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            l_Transmogrified->SetModifier(g_AppearanceModifierSlotBySpec[m_Player->GetActiveTalentGroup()], l_TransmogPair.second);
        }

        m_Player->SetVisibleItemSlot(l_Transmogrified->GetSlot(), l_Transmogrified);

        l_Transmogrified->SetNotRefundable(m_Player);
        l_Transmogrified->ClearSoulboundTradeable(m_Player);
        l_Transmogrified->SetState(ItemUpdateState::ITEM_CHANGED, m_Player);
    }

    for (auto& l_IllusionPair : l_IllusionItems)
    {
        Item* l_Transmogrified = l_IllusionPair.first;

        if (!l_CurrentSpecOnly)
        {
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS, l_IllusionPair.second);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1, 0);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2, 0);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3, 0);
            l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4, 0);
        }
        else
        {
            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            if (!l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4))
                l_Transmogrified->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4, l_Transmogrified->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            l_Transmogrified->SetModifier(g_IllusionModifierSlotBySpec[m_Player->GetActiveTalentGroup()], l_IllusionPair.second);
        }

        m_Player->SetVisibleItemSlot(l_Transmogrified->GetSlot(), l_Transmogrified);

        l_Transmogrified->SetNotRefundable(m_Player);
        l_Transmogrified->ClearSoulboundTradeable(m_Player);
        l_Transmogrified->SetState(ItemUpdateState::ITEM_CHANGED, m_Player);
    }

    for (Item* l_Item : l_ResetAppearanceItems)
    {
        if (!l_CurrentSpecOnly)
        {
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4, 0);
        }
        else
        {
            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));

            l_Item->SetModifier(g_AppearanceModifierSlotBySpec[m_Player->GetActiveTalentGroup()], 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS, 0);
        }

        m_Player->SetVisibleItemSlot(l_Item->GetSlot(), l_Item);
        l_Item->SetState(ItemUpdateState::ITEM_CHANGED, m_Player);
    }

    for (Item* l_Item : l_ResetIllusionItems)
    {
        if (!l_CurrentSpecOnly)
        {
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3, 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4, 0);
        }
        else
        {
            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            if (!l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4))
                l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4, l_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));

            l_Item->SetModifier(g_IllusionModifierSlotBySpec[m_Player->GetActiveTalentGroup()], 0);
            l_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS, 0);
        }

        m_Player->SetVisibleItemSlot(l_Item->GetSlot(), l_Item);
        l_Item->SetState(ItemUpdateState::ITEM_CHANGED, m_Player);
    }

    for (uint32 l_ItemModAppearanceID : l_BindAppearances)
    {
        GuidUnorderedSet l_ItemsProvidingAppearance = GetCollectionMgr()->GetItemsProvidingTemporaryAppearance(l_ItemModAppearanceID);
        for (uint64 const& l_ItemGuid : l_ItemsProvidingAppearance)
        {
            if (Item* l_Item = m_Player->GetItemByGuid(l_ItemGuid))
            {
                l_Item->SetNotRefundable(m_Player);
                l_Item->ClearSoulboundTradeable(m_Player);
                GetCollectionMgr()->AddItemAppearance(l_Item);
            }
        }
    }
}

