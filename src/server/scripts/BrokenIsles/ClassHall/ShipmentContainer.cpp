////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ShipmentContainer.hpp"

namespace MS { namespace Garrison { namespace GBrokenIsles {

      /// Constructor
    go_classhall_shipment_container::go_classhall_shipment_container()
        : GameObjectScript("go_classhall_shipment_container")
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a GameObjectAI object is needed for the GameObject.
    /// @p_GameObject : GameObject instance
    GameObjectAI* go_classhall_shipment_container::GetAI(GameObject* p_GameObject) const
    {
        return new go_classhall_shipment_containerAI(p_GameObject);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_classhall_shipment_container::go_classhall_shipment_containerAI::go_classhall_shipment_containerAI(GameObject* p_GameObject)
        : GameObjectAI(p_GameObject)
    {
        auto l_Phases = go->GetPhases();
        uint32 l_PhaseID = 0;

        for (uint32 l_Current : l_Phases)
        {
            if (l_Current > 9000000 && l_Current < 10000000)
            {
                l_PhaseID = l_Current;
                break;
            }
        }

        if (l_PhaseID)
        {
            uint32 l_WorkOderID = SHIPMENT_PHASE_ID_GET_WORK_ORDER_ID(l_PhaseID);
            uint32 l_State      = SHIPMENT_PHASE_ID_GET_STATE(l_PhaseID);

            if (l_State != WorkOrderStates::AllCompleted_LimitNotReached && l_State != WorkOrderStates::AllCompleted_LimitReached)
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_ACTIVATED);

            const CharShipmentEntry*            l_Shipment = sCharShipmentStore.LookupEntry(l_WorkOderID);
            const CharShipmentContainerEntry*   l_Container = sCharShipmentContainerStore.LookupEntry(l_Shipment ? l_Shipment->ShipmentContainerID : 0);

            if (!l_Shipment || !l_Container)
                return;

            if (l_State == WorkOrderStates::InProgress)
            {
                go->SetDisplayId(l_Container->OverrideDisplayID1);
                go->SetUInt32Value(GAMEOBJECT_FIELD_SPELL_VISUAL_ID, 61015); ///< Transparency
            }

            if (l_State == WorkOrderStates::AtLeastOneCompleted || l_State == WorkOrderStates::AllCompleted_LimitNotReached)
                go->SetDisplayId(l_Container->OverrideIfAmountMet[0]);

            if (l_State == WorkOrderStates::AllCompleted_LimitReached)
                go->SetDisplayId(l_Container->OverrideIfAmountMet[1]);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a player opens a gossip dialog with the GameObject.
    /// @p_Player     : Source player instance
    bool go_classhall_shipment_container::go_classhall_shipment_containerAI::GossipHello(Player* p_Player)
    {
        if (!p_Player || !p_Player->GetBrokenIslesGarrison())
            return false;

        std::vector<GarrisonWorkOrder> l_WorkOrders = p_Player->GetGarrison()->GetWorkOrders();

        auto l_Phases = go->GetPhases();
        uint32 l_PhaseID = 0;

        for (uint32 l_Current : l_Phases)
        {
            if (l_Current > 9000000 && l_Current < 10000000)
            {
                l_PhaseID = l_Current;
                break;
            }
        }

        uint32 l_WorkOderID = SHIPMENT_PHASE_ID_GET_WORK_ORDER_ID(l_PhaseID);

        if (!l_WorkOderID)
            return false;

        static const std::map<uint32, uint32> replaceQuestShipmentId
        {
            {324, 285},
            {326, 272},
            {328, 269},
            {325, 278},
            {333, 275},
            {331, 260},
            {322, 282},
            {327, 257},
            {321, 254},
        };

        std::map<uint32, bool> l_ToastStatus;
        for (uint32 l_I = 0; l_I < l_WorkOrders.size(); ++l_I)
        {
            auto rIter = replaceQuestShipmentId.find(l_WorkOrders[l_I].ShipmentID);

            if (l_WorkOrders[l_I].ShipmentID != l_WorkOderID && rIter == replaceQuestShipmentId.end())
                continue;

            if (rIter != replaceQuestShipmentId.end() && l_WorkOrders[l_I].ShipmentID == rIter->first && l_WorkOderID != rIter->second)
                continue;

            if (l_WorkOrders[l_I].CompleteTime > time(nullptr))
                continue;

            CharShipmentEntry const* l_ShipmentEntry = sCharShipmentStore.LookupEntry(l_WorkOrders[l_I].ShipmentID);

            if (!l_ShipmentEntry)
                continue;

            uint32 l_FollowerID = p_Player->GetGarrison()->GetFollowerIDForShipmentID(l_ShipmentEntry->ID);
            if (p_Player->GetGarrison()->HasTalent(429)) ///< Enlightenment
            {
                if (roll_chance_i(25)) ///< Wowhead Comments
                {
                    switch (l_FollowerID)
                    {
                        case 622: ///< Ox Initiates
                            l_FollowerID = 627; ///< Ox Adepts
                            break;
                        case 627: ///< Ox Adepts
                            l_FollowerID = 695; ///< Ox Masters
                            break;
                        case 629: ///< Tiger Initiates
                            l_FollowerID = 630; ///< Tiger Adepts
                            break;
                        case 630: ///< Tiger Adepts
                            l_FollowerID = 631; ///< Tiger Masters
                            break;
                        default:
                            break;
                    }
                }
            }

            if (l_FollowerID)
                p_Player->GetGarrison()->AddFollower(l_FollowerID);

            /// Opening the Test Kitchen - KillCredit
            if (l_ShipmentEntry->ID == 247)
                p_Player->QuestObjectiveSatisfy(46751, 1, QUEST_OBJECTIVE_TYPE_CRITERIA_TREE);

            if (l_ShipmentEntry->ShipmentContainerID == 178)
                p_Player->KilledMonsterCredit(113323);

            uint32 l_RewardID = l_ShipmentEntry->ResultItemID;
            std::map<uint32, uint32> l_RewardItems;

            /// Default reward
            if (l_RewardID)
                l_RewardItems.insert(std::make_pair(l_RewardID, 1));

            /// Adding items
            bool l_CanGetItems = true;
            uint32 l_NoSpaceForCount = 0;

            for (auto l_RewardItem : l_RewardItems)
            {
                if (!l_RewardItem.second)
                    continue;

                /// check space and find places
                ItemPosCountVec l_Destination;
                InventoryResult l_Message = p_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, l_RewardItem.first, 1, &l_NoSpaceForCount);

                if (l_Message != EQUIP_ERR_OK)
                {
                    p_Player->SendEquipError(l_Message, nullptr, nullptr, l_RewardItem.first);
                    l_CanGetItems = false;
                    break;
                }
            }

            if (l_ShipmentEntry->OnCompleteSpellId)
                p_Player->CastSpell(p_Player, l_ShipmentEntry->OnCompleteSpellId, false);

            if (auto shipmentConteinerEntry = sCharShipmentContainerStore.LookupEntry(go->GetGOInfo()->garrisonShipment.ShipmentContainer))
                p_Player->UpdateCriteria(CRITERIA_TYPE_COMPLETE_GARRISON_SHIPMENT, l_ShipmentEntry->ID);

            if (l_CanGetItems)
            {
                for (auto l_RewardItem : l_RewardItems)
                {
                    if (!l_RewardItem.second)
                        continue;

                    /// Special Case : Seal of the Broken Fate
                    if (l_RewardItem.first == 139460)
                    {
                        p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_SEAL_OF_BROKEN_FATE, l_RewardItem.second);
                        p_Player->ModifyCharacterWorldState(CharacterWorldStates::GarrisonWeeklySealOfBrokenFateWorkorder, 1);
                        p_Player->ModifyCharacterWorldState(CharacterWorldStates::BrokenIslesWeeklyBonusRollTokenCount, 1);
                        continue;
                    }

                    /// check space and find places
                    ItemPosCountVec l_Destination;
                    InventoryResult l_Message = p_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, l_RewardItem.first, 1, &l_NoSpaceForCount);

                    if (l_Message == EQUIP_ERR_OK)
                    {
                        /// Special Case : Follower Equipment
                        if (l_RewardItem.first == 139308)
                        {
                            p_Player->AutoStoreLoot(MS::Garrison::GBrokenIsles::s_ArmamentLootTemplates[p_Player->getClass()], LootTemplates_Item);
                            continue;
                        }

                        /// Special Case : Cooking Workorder
                        if (s_CookingWorkordersLoots.find(l_RewardItem.first) != s_CookingWorkordersLoots.end())
                        {
                            for (uint32 l_I = 0; l_I < l_RewardItem.second; ++l_I)
                                p_Player->AutoStoreLoot(l_RewardItem.first, LootTemplates_Item);

                            continue;
                        }

                        if (Item* l_Item = p_Player->StoreNewItem(l_Destination, l_RewardItem.first, true, Item::GenerateItemRandomPropertyId(l_RewardItem.first)))
                            sScriptMgr->OnPlayerItemLooted(p_Player, l_Item);

                        if (l_ToastStatus[l_RewardItem.first] == false)
                        {
                            p_Player->SendDisplayToast(l_RewardItem.first, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);
                            l_ToastStatus[l_RewardItem.first] = true;
                        }
                    }
                    else
                        p_Player->SendEquipError(l_Message, nullptr, nullptr, l_RewardItem.first);
                }
            }

            p_Player->GetGarrison()->DeleteWorkOrder(l_WorkOrders[l_I].DatabaseID);
        }

        return true;
    }

}   ///< namespace GBrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS

#ifndef __clang_analyzer__
void AddSC_ShipmentContainer()
{
    /// Go
    new MS::Garrison::GBrokenIsles::go_classhall_shipment_container();
}
#endif
