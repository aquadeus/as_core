////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "DB2Stores.h"

class items_commandscript : public CommandScript
{
    public:

        items_commandscript() : CommandScript("items_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand s_ItemCommandTable[] =
            {
                { "getslots",       SEC_GAMEMASTER,     SEC_GAMEMASTER,  false, &HandleItemGetSlotsCommand,          "", NULL },
                { "addbonus",       SEC_GAMEMASTER,     SEC_GAMEMASTER,  false, &HandleItemAddBonusCommand,          "", NULL },
                { "removebonus",    SEC_GAMEMASTER,     SEC_GAMEMASTER,  false, &HandleItemRemoveBonusCommand,       "", NULL },
                { "clearbonuses",   SEC_GAMEMASTER,     SEC_GAMEMASTER,  false, &HandleItemClearBonusesCommand,      "", NULL },
                { "dumpspecs",      SEC_GAMEMASTER,     SEC_GAMEMASTER,  true,  &HandleItemDumpSpecs,                "", NULL },
                { "rmchilds",       SEC_GAMEMASTER,     SEC_GAMEMASTER,  true,  &HandleItemDeleteAllChilds,          "", NULL },
                { NULL,             SEC_PLAYER,         SEC_PLAYER,      false, NULL,                                "", NULL }
            };

            static ChatCommand s_CommandTable[] =
            {
                { "item",           SEC_MODERATOR,      SEC_MODERATOR,      true,  NULL,                 "", s_ItemCommandTable  },
                { NULL,             SEC_PLAYER,         SEC_PLAYER,         false, NULL,                 "", NULL                }
            };

            return s_CommandTable;
        }

        static bool HandleItemDeleteAllChilds(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player = p_Handler->getSelectedPlayer();
            if (l_Player == nullptr)
                l_Player = p_Handler->GetSession()->GetPlayer();

            if (!l_Player)
                return false;

            uint32 remcount = 0;

            /// In Equipment And Bag List
            /// In Inventory
            /// In Bank
            for (uint8 i = EQUIPMENT_SLOT_START; i < BANK_SLOT_ITEM_END; i++)
            {
                if (Item* pItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    uint32 l_ParentId = GetParentItemIDIfExists(pItem->GetEntry());
                    if (!l_ParentId)
                        continue;

                    if (ItemTemplate const* l_ParentTemplate = sObjectMgr->GetItemTemplate(l_ParentId))
                    {
                        if (l_ParentTemplate->ArtifactID)
                        {
                            remcount++;
                            l_Player->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
                        }
                    }
                }
            }

            /// In Inventory Bags
            for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
            {
                if (Bag* pBag = l_Player->GetBagByPos(i))
                {
                    for (uint32 j = 0; j < pBag->GetBagSize(); j++)
                    {
                        if (Item* pItem = pBag->GetItemByPos(j))
                        {
                            uint32 l_ParentId = GetParentItemIDIfExists(pItem->GetEntry());
                            if (!l_ParentId)
                                continue;

                            if (ItemTemplate const* l_ParentTemplate = sObjectMgr->GetItemTemplate(l_ParentId))
                            {
                                if (l_ParentTemplate->ArtifactID)
                                {
                                    remcount++;
                                    l_Player->DestroyItem(i, j, true);
                                }
                            }
                        }
                    }
                }
            }

            /// In Bank Bags
            for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
            {
                if (Bag* pBag = l_Player->GetBagByPos(i))
                {
                    for (uint32 j = 0; j < pBag->GetBagSize(); j++)
                    {
                        if (Item* pItem = pBag->GetItemByPos(j))
                        {
                            uint32 l_ParentId = GetParentItemIDIfExists(pItem->GetEntry());
                            if (!l_ParentId)
                                continue;

                            if (ItemTemplate const* l_ParentTemplate = sObjectMgr->GetItemTemplate(l_ParentId))
                            {
                                if (l_ParentTemplate->ArtifactID)
                                {
                                    remcount++;
                                    l_Player->DestroyItem(i, j, true);
                                }
                            }
                        }
                    }
                }
            }

            /// In Child Slots
            for (uint8 i = CHILD_EQUIPMENT_SLOT_START; i < CHILD_EQUIPMENT_SLOT_END; ++i)
            {
                if (Item* pItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    uint32 l_ParentId = GetParentItemIDIfExists(pItem->GetEntry());
                    if (!l_ParentId)
                        continue;

                    if (ItemTemplate const* l_ParentTemplate = sObjectMgr->GetItemTemplate(l_ParentId))
                    {
                        if (l_ParentTemplate->ArtifactID)
                        {
                            remcount++;
                            l_Player->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
                        }
                    }
                }
            }

            /// In Mailbox
            for (std::deque<Mail*>::iterator l_MailItr = l_Player->GetMailBegin(); l_MailItr != l_Player->GetMailEnd(); ++l_MailItr)
            {
                for (std::vector<MailItemInfo>::iterator l_ItemInfoItr = (*l_MailItr)->items.begin(); l_ItemInfoItr != (*l_MailItr)->items.end(); ++l_ItemInfoItr)
                {
                    uint32 l_ParentId = GetParentItemIDIfExists((*l_ItemInfoItr).item_template);
                    if (!l_ParentId)
                        continue;

                    if (ItemTemplate const* l_ParentTemplate = sObjectMgr->GetItemTemplate(l_ParentId))
                    {
                        if (l_ParentTemplate->ArtifactID)
                        {
                            remcount++;
                            (*l_MailItr)->RemoveItem((*l_ItemInfoItr).item_guid);
                            if ((*l_MailItr)->items.empty())
                                break;
                            else
                                l_ItemInfoItr = (*l_MailItr)->items.begin();
                        }
                    }
                }
            }

            std::stringstream ss;
            ss << "Removed " << remcount << " child items.";

            p_Handler->SendSysMessage(ss.str().c_str());

            return true;
        }

        static bool HandleItemGetSlotsCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            p_Handler->SendSysMessage("Head : 0");
            p_Handler->SendSysMessage("Neck : 1");
            p_Handler->SendSysMessage("Shoulders : 2");
            p_Handler->SendSysMessage("Chest : 4");
            p_Handler->SendSysMessage("Waist : 5");
            p_Handler->SendSysMessage("Legs : 6");
            p_Handler->SendSysMessage("Foot : 7");
            p_Handler->SendSysMessage("Wrist : 8");
            p_Handler->SendSysMessage("Hand : 9");
            p_Handler->SendSysMessage("Ring 1 : 10");
            p_Handler->SendSysMessage("Ring 2 : 11");
            p_Handler->SendSysMessage("Trinket 1 : 12");
            p_Handler->SendSysMessage("Trinket 2 : 13");
            p_Handler->SendSysMessage("Cloak : 14");
            p_Handler->SendSysMessage("Main hand : 15");
            p_Handler->SendSysMessage("Off hand : 16");
            return true;
        }

        static bool HandleItemAddBonusCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->getSelectedPlayer();
            if (l_Player == nullptr)
                l_Player = p_Handler->GetSession()->GetPlayer();

            char* l_SlotStr = strtok((char*)p_Args, " ");
            char* l_BonusStr = strtok(NULL, " ");

            if (!l_SlotStr)
            {
                p_Handler->SendSysMessage("Wrong argument for slot.");
                return false;
            }

            if (!l_BonusStr)
            {
                p_Handler->SendSysMessage("Wrong argument for bonus.");
                return false;
            }

            int8 l_Slot = atoi(l_SlotStr);
            uint32 l_Bonus = atoi(l_BonusStr);

            Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Slot);
            if (!l_Item)
            {
                p_Handler->SendSysMessage("Item not found.");
                return false;
            }

            if (l_Item->HasItemBonus(l_Bonus))
            {
                p_Handler->SendSysMessage("Item has already this bonus.");
                return false;
            }

            l_Player->_RemoveAllItemMods();
            l_Item->AddItemBonus(l_Bonus);
            l_Player->_ApplyAllItemMods();

            p_Handler->SendSysMessage("Item bonus successfully added.");
            return true;
        }

        static bool HandleItemRemoveBonusCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->getSelectedPlayer();
            if (l_Player == nullptr)
                l_Player = p_Handler->GetSession()->GetPlayer();

            char* l_SlotStr = strtok((char*)p_Args, " ");
            char* l_BonusStr = strtok(NULL, " ");

            if (!l_SlotStr)
            {
                p_Handler->SendSysMessage("Wrong argument for slot.");
                return false;
            }

            if (!l_BonusStr)
            {
                p_Handler->SendSysMessage("Wrong argument for bonus.");
                return false;
            }

            int8 l_Slot = atoi(l_SlotStr);
            uint32 l_Bonus = atoi(l_BonusStr);

            Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Slot);
            if (!l_Item)
            {
                p_Handler->SendSysMessage("Item not found.");
                return false;
            }

            if (!l_Item->HasItemBonus(l_Bonus))
            {
                p_Handler->SendSysMessage("Item has not this bonus.");
                return false;
            }

            l_Player->_RemoveAllItemMods();
            l_Item->RemoveItemBonus(l_Bonus);
            l_Player->_ApplyAllItemMods();

            p_Handler->SendSysMessage("Item bonus successfully removed.");
            return true;
        }

        static bool HandleItemClearBonusesCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->getSelectedPlayer();
            if (l_Player == nullptr)
                l_Player = p_Handler->GetSession()->GetPlayer();

            char* l_SlotStr = strtok((char*)p_Args, " ");

            if (!l_SlotStr)
            {
                p_Handler->SendSysMessage("Wrong argument for slot.");
                return false;
            }

            int8 l_Slot = atoi(l_SlotStr);

            Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Slot);
            if (!l_Item)
            {
                p_Handler->SendSysMessage("Item not found.");
                return false;
            }

            l_Player->_RemoveAllItemMods();
            l_Item->RemoveAllItemBonuses();
            l_Player->_ApplyAllItemMods();

            p_Handler->SendSysMessage("Item bonuses successfully removed.");
            return true;
        }

        static bool HandleItemDumpSpecs(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!p_Args)
                return false;

            uint32 l_ItemID = atoi(p_Args);
            ItemTemplate const* l_Item = sObjectMgr->GetItemTemplate(l_ItemID);

            if (!l_Item)
                return false;

            std::stringstream ss;

            for (uint32 l_ID = 0; l_ID < sChrSpecializationsStore.GetNumRows(); ++l_ID)
            {
                auto l_Entry = sChrSpecializationsStore.LookupEntry(l_ID);
                if (!l_Entry)
                    continue;

                if (l_Item->HasSpec(static_cast<SpecIndex>(l_Entry->ID), 110))
                    ss << l_Entry->SpecializationName->Get(sWorld->GetDefaultDb2Locale()) << ", ";
            }


            p_Handler->SendSysMessage(ss.str().c_str());
            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_items_commandscript()
{
    new items_commandscript();
}
#endif
