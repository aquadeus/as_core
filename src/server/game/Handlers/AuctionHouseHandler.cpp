////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CROSS
#include "ObjectMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include "AuctionHouseMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "UpdateMask.h"
#include "Util.h"
#include "AccountMgr.h"

// Called when player click on auctioneer npc
void WorldSession::HandleAuctionHelloOpcode(WorldPacket& p_Packet)
{
    uint64 l_Guid = 0;

    p_Packet.readPackGUID(l_Guid);

    Creature* l_Unit = m_Player->GetNPCIfCanInteractWith(l_Guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Unit)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionHelloOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(l_Guid)));
        return;
    }

    // Remove fake death
    if (m_Player->HasUnitState(UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    m_Player->SetInteractionStatus(l_Guid, InteractionStatus::AuctionHouse);
    SendAuctionHello(l_Guid, l_Unit);
}

// This void causes that auction window is opened
void WorldSession::SendAuctionHello(uint64 p_Guid, Creature* p_Unit)
{
    uint32 levelReq = sWorld->getIntConfig(CONFIG_AUCTION_LEVEL_REQ);
    switch (m_Player->getRace())
    {
        case Races::RACE_VOID_ELF:
        case Races::RACE_NIGHTBORNE:
        case Races::RACE_LIGHTFORGED_DRAENEI:
        case Races::RACE_HIGHMOUNTAIN_TAUREN:
            levelReq += sWorld->getIntConfig(CONFIG_ALLIED_RACE_LEVEL_REQ_DIFF);
        default:
            break;
    }
    if (m_Player->getClass() == CLASS_DEATH_KNIGHT)
        levelReq = sWorld->getIntConfig(CONFIG_AUCTION_LEVEL_REQ) + sWorld->getIntConfig(CONFIG_DK_LEVEL_REQ_DIFF);
    else if (m_Player->getClass() == CLASS_DEMON_HUNTER)
        levelReq = sWorld->getIntConfig(CONFIG_AUCTION_LEVEL_REQ) + sWorld->getIntConfig(CONFIG_DH_LEVEL_REQ_DIFF);

    if (m_Player->getLevel() < std::min(levelReq, (uint32)LevelLimit::MAX_LEVEL))
    {
        SendNotification(GetTrinityString(LANG_AUCTION_REQ), levelReq);
        return;
    }

    AuctionHouseEntry const* l_AuctionHouseEntry = AuctionHouseMgr::GetAuctionHouseEntry(p_Unit->getFaction());
    if (!l_AuctionHouseEntry)
        return;

    WorldPacket l_Data(SMSG_AUCTION_HELLO_RESPONSE, 12);
    l_Data.appendPackGUID(p_Guid);                          ///< Guid
    l_Data.WriteBit(1);                                     ///< OpenForBusiness
    l_Data.FlushBits();

    SendPacket(&l_Data);
}

// Call this method when player bids, creates, or deletes auction
void WorldSession::SendAuctionCommandResult(AuctionEntryPtr p_Auction, uint32 p_Action, uint32 p_Error, uint32 p_BidError)
{
    uint64 l_Guid = 0;

    WorldPacket l_Data(SMSG_AUCTION_COMMAND_RESULT);
    l_Data << uint32(p_Auction ? p_Auction->Id : 0);    ///< AuctionItemID
    l_Data << uint32(p_Action);                         ///< Command
    l_Data << uint32(p_Error);                          ///< BagResult
    l_Data << uint32(p_BidError);                       ///< ErrorCode
    l_Data.appendPackGUID(l_Guid);                      ///< Guid
    l_Data << uint64(p_Auction ? (p_Auction->bid ? p_Auction->GetAuctionOutBid() : 0) : 0); ///< MinIncrement
    l_Data << uint64(p_Auction ? (p_Auction->bid ? p_Auction->GetAuctionOutBid() : 0) : 0); ///< Money
    SendPacket(&l_Data);
}

void WorldSession::SendAuctionOutBidNotification(AuctionEntryPtr p_AuctionEntry, Item const* p_Item)
{
    WorldPacket l_Data(Opcodes::SMSG_AUCTION_OUTBID_NOTIFICATION, 1024);

    l_Data << uint32(p_AuctionEntry->Id);
    l_Data.appendPackGUID(MAKE_NEW_GUID(p_AuctionEntry->bidder, 0, HighGuid::HIGHGUID_PLAYER));

    Item::BuildDynamicItemDatas(l_Data, p_Item);

    l_Data << uint64(p_AuctionEntry->bid);
    l_Data << uint64(p_AuctionEntry->GetAuctionOutBid());

    SendPacket(&l_Data);
}

void WorldSession::SendAuctionClosedNotification(AuctionEntryPtr p_AuctionEntry, float p_MailDelay, bool p_Sold, Item const* p_Item)
{
    WorldPacket l_Data(Opcodes::SMSG_AUCTION_CLOSED_NOTIFICATION, 1024);

    l_Data << uint32(p_AuctionEntry->Id);
    l_Data << uint64(p_AuctionEntry->bid);

    Item::BuildDynamicItemDatas(l_Data, p_Item);

    l_Data << float(p_MailDelay);

    l_Data.WriteBit(p_Sold);
    l_Data.FlushBits();

    SendPacket(&l_Data);
}

void WorldSession::SendAuctionWonNotification(AuctionEntryPtr p_AuctionEntry, Item const* p_Item)
{
    WorldPacket l_Data(Opcodes::SMSG_AUCTION_WON_NOTIFICATION, 1024);

    l_Data << uint32(p_AuctionEntry->Id);
    l_Data.appendPackGUID(MAKE_NEW_GUID(p_AuctionEntry->bidder, 0, HighGuid::HIGHGUID_PLAYER));

    Item::BuildDynamicItemDatas(l_Data, p_Item);

    SendPacket(&l_Data);
}

void WorldSession::SendAuctionOwnerBidNotification(AuctionEntryPtr p_AuctionEntry, Item const* p_Item)
{
    WorldPacket l_Data(Opcodes::SMSG_AUCTION_OWNER_BID_NOTIFICATION, 1024);

    l_Data << uint32(p_AuctionEntry->Id);
    l_Data << uint64(p_AuctionEntry->bid);

    Item::BuildDynamicItemDatas(l_Data, p_Item);

    l_Data << uint64(p_AuctionEntry->GetAuctionOutBid());
    l_Data.appendPackGUID(MAKE_NEW_GUID(p_AuctionEntry->bidder, 0, HighGuid::HIGHGUID_PLAYER));

    SendPacket(&l_Data);
}

// This void creates new auction and adds auction to some auction house
void WorldSession::HandleAuctionSellItem(WorldPacket& p_RecvData)
{
    uint64 l_SellerGuid = 0;
    uint64 l_Bid, l_Buyout;
    uint32 l_ItemCount, l_ETTime;

    p_RecvData.readPackGUID(l_SellerGuid);
    p_RecvData >> l_Bid >> l_Buyout;
    p_RecvData >> l_ETTime;

    l_ItemCount = p_RecvData.ReadBits(5);
    p_RecvData.ResetBitReading();

    std::vector<uint64> l_ItemGuids(l_ItemCount, 0);
    std::vector<uint32> l_CountOfItems(l_ItemCount, 0);
    for (uint8 l_Iter = 0; l_Iter < l_ItemCount; l_Iter++)
    {
        p_RecvData.readPackGUID(l_ItemGuids[l_Iter]);
        p_RecvData >> l_CountOfItems[l_Iter];

        for (uint32 l_J = 0; l_J < l_Iter; ++l_J)
        {
            if (l_ItemGuids[l_Iter] != 0 && l_ItemGuids[l_J] != 0 && l_ItemGuids[l_Iter] == l_ItemGuids[l_J])
            {
                // duplicate guid, cheating
                SendAuctionCommandResult(0, AUCTION_SELL_ITEM, ERR_AUCTION_ITEM_NOT_FOUND);
                return;
            }
        }
    }

    if (!l_Bid || !l_ETTime || l_Bid >= MAX_MONEY_AMOUNT || l_Buyout >= MAX_MONEY_AMOUNT)
        return;

    Creature* l_Auctioneer = m_Player->GetNPCIfCanInteractWith(l_SellerGuid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Auctioneer)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionSellItem - Unit (GUID: %u) not found or you can't interact with him.", GUID_LOPART(l_SellerGuid));
        return;
    }

    AuctionHouseEntry const* l_AuctionHouse = AuctionHouseMgr::GetAuctionHouseEntry(l_Auctioneer->getFaction());
    if (!l_AuctionHouse)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionSellItem - Unit (GUID: %u) has wrong faction.", GUID_LOPART(l_SellerGuid));
        return;
    }

    l_ETTime *= MINUTE;

    switch (l_ETTime)
    {
        case 1*MIN_AUCTION_TIME:
        case 2*MIN_AUCTION_TIME:
        case 4*MIN_AUCTION_TIME:
            break;
        default:
            return;
    }

    if (m_Player->HasUnitState(UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    std::unique_ptr<Item*[]> l_Items = std::make_unique<Item*[]>(l_ItemCount);
    uint32 l_FinalCount = 0;
    uint32 l_TrueItemEntry = 0;

    for (uint32 l_Iter = 0; l_Iter < l_ItemCount; ++l_Iter)
    {
        Item* l_Item = m_Player->GetItemByGuid(l_ItemGuids[l_Iter]);
        if (!l_Item)
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_ITEM_NOT_FOUND);
            return;
        }

        /// Prevent sell of store items
        if (l_Item->HasCustomFlags(ItemCustomFlags::FromStore))
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_ITEM_NOT_FOUND);
            return;
        }

        if (!l_TrueItemEntry)
            l_TrueItemEntry = l_Item->GetEntry();
        else if (l_Item->GetEntry() != l_TrueItemEntry)
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_ITEM_NOT_FOUND);
            return;
        }

        if (sAuctionMgr->GetAItem(l_Item->GetGUIDLow()) || !l_Item->CanBeTraded() || l_Item->IsNotEmptyBag() ||
            l_Item->GetTemplate()->Flags & int32(ItemFlags::CONJURED) || l_Item->GetUInt32Value(ITEM_FIELD_EXPIRATION) ||
            l_Item->GetCount() < l_CountOfItems[l_Iter])
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
            return;
        }

        l_Items[l_Iter] = l_Item;
        l_FinalCount += l_CountOfItems[l_Iter];
    }

    if (!l_FinalCount)
    {
        SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
        return;
    }

    if (!m_Player->HasItemCount(l_TrueItemEntry, l_FinalCount))
    {
        SendAuctionCommandResult(0, AUCTION_SELL_ITEM, ERR_AUCTION_ITEM_NOT_FOUND);
        return;
    }

    for (uint32 l_Iter = 0; l_Iter < l_ItemCount; ++l_Iter)
    {
        Item* l_Item = l_Items[l_Iter];
        if (l_Item->GetMaxStackCount() < l_FinalCount)
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
            return;
        }
    }

    for (uint32 l_Iter = 0; l_Iter < l_ItemCount; ++l_Iter)
    {
        Item* l_Item = l_Items[l_Iter];

        uint32 l_AuctionTime = uint32(l_ETTime * sWorld->getRate(RATE_AUCTION_TIME));
        AuctionHouseObject* l_AuctionHouseObj = sAuctionMgr->GetAuctionsMap(l_Auctioneer->getFaction());

        uint64 l_Deposit = sAuctionMgr->GetAuctionDeposit(l_ETTime, l_Item, l_FinalCount);
        if (!m_Player->HasEnoughMoney(l_Deposit))
        {
            SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_NOT_ENOUGHT_MONEY);
            return;
        }

        m_Player->ModifyMoney(-int64(l_Deposit)), "HandleAuctionSellItem " + std::to_string(l_Item->GetEntry());

        AuctionEntryPtr l_AHEntry = std::make_shared<AuctionEntry>(AuctionEntry());
        l_AHEntry->Id = sObjectMgr->GenerateAuctionID();

        if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
            l_AHEntry->auctioneer = 174444;
        else
            l_AHEntry->auctioneer = GUID_LOPART(l_SellerGuid);

        ASSERT(sObjectMgr->GetCreatureData(l_AHEntry->auctioneer)); // Tentative de vendre un item a un pnj qui n'existe pas, mieux vaut crash ici sinon l'item en question risque de disparaitre tout simplement

        // Required stack size of auction matches to current item stack size, just move item to auctionhouse
        if (l_ItemCount == 1 && l_Item->GetCount() == l_CountOfItems[l_Iter])
        {
            if (GetSecurity() > SEC_PLAYER && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
            {
                sLog->outCommand(GetAccountId(), "", m_Player->GetGUIDLow(), m_Player->GetName(), 0, "", 0, "",
                                "GM %s (Account: %u) create auction: %s (Entry: %u Count: %u)",
                                GetPlayerName().c_str(), GetAccountId(), l_Item->GetTemplate()->Name1->Get(sWorld->GetDefaultDb2Locale()), l_Item->GetEntry(), l_Item->GetCount());
            }

            l_AHEntry->itemGUIDLow = l_Item->GetGUIDLow();
            l_AHEntry->itemEntry = l_Item->GetEntry();
            l_AHEntry->itemCount = l_Item->GetCount();
            l_AHEntry->owner = m_Player->GetGUIDLow();
            l_AHEntry->startbid = l_Bid;
            l_AHEntry->bidder = 0;
            l_AHEntry->bid = 0;
            l_AHEntry->buyout = l_Buyout;
            l_AHEntry->expire_time = time(NULL) + l_AuctionTime;
            l_AHEntry->deposit = l_Deposit;
            l_AHEntry->auctionHouseEntry = l_AuctionHouse;

            sAuctionMgr->AddAItem(l_Item);
            l_AuctionHouseObj->AddAuction(l_AHEntry);

            m_Player->MoveItemFromInventory(l_Item->GetBagSlot(), l_Item->GetSlot(), true);

            SQLTransaction l_Trans = CharacterDatabase.BeginTransaction();
            l_Item->DeleteFromInventoryDB(l_Trans);
            l_Item->SaveToDB(l_Trans);
            l_AHEntry->SaveToDB(l_Trans);
            m_Player->SaveInventoryAndGoldToDB(l_Trans);
            CharacterDatabase.CommitTransaction(l_Trans);

            SendAuctionCommandResult(l_AHEntry, AUCTION_SELL_ITEM, ERR_AUCTION_OK);

            m_Player->UpdateCriteria(CRITERIA_TYPE_CREATE_AUCTION, 1);
            return;
        }
        else // Required stack size of auction does not match to current item stack size, clone item and set correct stack size
        {
            Item* l_NewItem = l_Item->CloneItem(l_FinalCount, m_Player);
            if (!l_NewItem)
            {
                sLog->outError(LOG_FILTER_NETWORKIO, "CMSG_AUCTION_SELL_ITEM: Could not create clone of item %u", l_Item->GetEntry());
                SendAuctionCommandResult(NULL, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
                return;
            }

            if (GetSecurity() > SEC_PLAYER && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
            {
                sLog->outCommand(GetAccountId(), "", m_Player->GetGUIDLow(), m_Player->GetName(), 0, "", 0, "",
                                "GM %s (Account: %u) create auction: %s (Entry: %u Count: %u)",
                                GetPlayerName().c_str(), GetAccountId(), l_Item->GetTemplate()->Name1->Get(sWorld->GetDefaultDb2Locale()), l_Item->GetEntry(), l_Item->GetCount());
            }

            l_AHEntry->itemGUIDLow = l_NewItem->GetGUIDLow();
            l_AHEntry->itemEntry = l_NewItem->GetEntry();
            l_AHEntry->itemCount = l_NewItem->GetCount();
            l_AHEntry->owner = m_Player->GetGUIDLow();
            l_AHEntry->startbid = l_Bid;
            l_AHEntry->bidder = 0;
            l_AHEntry->bid = 0;
            l_AHEntry->buyout = l_Buyout;
            l_AHEntry->expire_time = time(NULL) + l_AuctionTime;
            l_AHEntry->deposit = l_Deposit;
            l_AHEntry->auctionHouseEntry = l_AuctionHouse;

            sLog->outInfo(LOG_FILTER_NETWORKIO, "CMSG_AUCTION_SELL_ITEM: Player %s (guid %d) is selling item %s entry %u (guid %d) to auctioneer %u with count %u with initial bid %lu with buyout %lu and with time %u (in sec) in auctionhouse %u", m_Player->GetName(), m_Player->GetGUIDLow(), l_NewItem->GetTemplate()->Name1->Get(sWorld->GetDefaultDb2Locale()), l_NewItem->GetEntry(), l_NewItem->GetGUIDLow(), l_AHEntry->auctioneer, l_NewItem->GetCount(), l_Bid, l_Buyout, l_AuctionTime, l_AHEntry->GetHouseId());
            sAuctionMgr->AddAItem(l_NewItem);
            l_AuctionHouseObj->AddAuction(l_AHEntry);

            for (uint32 l_Index = 0; l_Index < l_ItemCount; ++l_Index)
            {
                Item* l_ItemOther = l_Items[l_Index];

                // Item stack count equals required count, ready to delete item - cloned item will be used for auction
                if (l_ItemOther->GetCount() == l_CountOfItems[l_Index])
                {
                    m_Player->MoveItemFromInventory(l_ItemOther->GetBagSlot(), l_ItemOther->GetSlot(), true);

                    SQLTransaction l_Trans = CharacterDatabase.BeginTransaction();
                    l_ItemOther->DeleteFromInventoryDB(l_Trans);
                    l_ItemOther->DeleteFromDB(l_Trans);
                    CharacterDatabase.CommitTransaction(l_Trans);
                }
                else // Item stack count is bigger than required count, update item stack count and save to database - cloned item will be used for auction
                {
                    l_ItemOther->SetCount(l_ItemOther->GetCount() - l_CountOfItems[l_Index]);
                    l_ItemOther->SetState(ITEM_CHANGED, m_Player);
                    m_Player->ItemRemovedQuestCheck(l_ItemOther->GetEntry(), l_CountOfItems[l_Index]);
                    l_ItemOther->SendUpdateToPlayer(m_Player);

                    SQLTransaction l_Trans = CharacterDatabase.BeginTransaction();
                    l_ItemOther->SaveToDB(l_Trans);
                    CharacterDatabase.CommitTransaction(l_Trans);
                }
            }

            SQLTransaction l_Trans = CharacterDatabase.BeginTransaction();
            l_NewItem->SaveToDB(l_Trans);
            l_AHEntry->SaveToDB(l_Trans);
            m_Player->SaveInventoryAndGoldToDB(l_Trans);
            CharacterDatabase.CommitTransaction(l_Trans);

            SendAuctionCommandResult(l_AHEntry, AUCTION_SELL_ITEM, ERR_AUCTION_OK);

            m_Player->UpdateCriteria(CRITERIA_TYPE_CREATE_AUCTION, 1);
            return;
        }
    }
}

// This function is called when client bids or buys out auction
void WorldSession::HandleAuctionPlaceBid(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;
    uint32 l_AuctionID;
    uint64 l_Price;

    p_RecvData.readPackGUID(l_Guid);            ///< Auctioneer
    p_RecvData >> l_AuctionID;                  ///< AuctionItemID
    p_RecvData >> l_Price;                      ///< BidAmount

    if (!l_AuctionID || !l_Price)
        return;

    Creature* l_Auctioneer = m_Player->GetNPCIfCanInteractWith(l_Guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Auctioneer)
        return;

    // Remove fake death
    if (m_Player->HasUnitState(UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* l_AuctionHouse = sAuctionMgr->GetAuctionsMap(l_Auctioneer->getFaction());

    AuctionEntryPtr l_Auction = l_AuctionHouse->GetAuction(l_AuctionID);

    if (!l_Auction)
    {
        SendAuctionCommandResult(nullptr, AUCTION_PLACE_BID, ERR_AUCTION_DATABASE_ERROR);
        return;
    }

    if (l_Auction->owner == m_Player->GetGUIDLow())
    {
        SendAuctionCommandResult(nullptr, AUCTION_PLACE_BID, ERR_AUCTION_BID_OWN);
        return;
    }

    // Cheating
    if (l_Price <= l_Auction->bid || l_Price < l_Auction->startbid)
    {
        SendAuctionCommandResult(nullptr, AUCTION_PLACE_BID, ERR_AUCTION_HIGHER_BID);
        return;
    }

    // Price too low for next bid if not buyout
    if ((l_Price < l_Auction->buyout || l_Auction->buyout == 0) &&
        l_Price < l_Auction->bid + l_Auction->GetAuctionOutBid())
    {
        SendAuctionCommandResult(nullptr, AUCTION_PLACE_BID, ERR_AUCTION_HIGHER_BID);
        return;
    }

    if (!m_Player->HasEnoughMoney(l_Price))
    {
        SendAuctionCommandResult(nullptr, AUCTION_PLACE_BID, ERR_AUCTION_NOT_ENOUGHT_MONEY);
        return;
    }

    if (l_Price > MAX_MONEY_AMOUNT)
        l_Price = MAX_MONEY_AMOUNT;

    SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();

    if (l_Price < l_Auction->buyout || l_Auction->buyout == 0)
    {
        if (l_Auction->bidder > 0)
        {
            if (l_Auction->bidder == m_Player->GetGUIDLow())
                m_Player->ModifyMoney(-int64(l_Price - l_Auction->bid), "HandleAuctionPlaceBid " + std::to_string(l_Auction->itemEntry));
            else
            {
                // mail to last bidder and return money
                sAuctionMgr->SendAuctionOutbiddedMail(l_Auction, l_Price, m_Player, l_Transaction);
                m_Player->ModifyMoney(-int64(l_Price), "HandleAuctionPlaceBid " + std::to_string(l_Auction->itemEntry));
            }
        }
        else
            m_Player->ModifyMoney(-int64(l_Price), "HandleAuctionPlaceBid " + std::to_string(l_Auction->itemEntry));

        SendAuctionCommandResult(nullptr, AUCTION_PLACE_BID, ERR_AUCTION_OK);
        l_Auction->bidder = m_Player->GetGUIDLow();
        l_Auction->bid = l_Price;
        m_Player->UpdateCriteria(CRITERIA_TYPE_HIGHEST_AUCTION_BID, l_Price);

        PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_AUCTION_BID);
        l_Statement->setUInt32(0, l_Auction->bidder);
        l_Statement->setUInt64(1, l_Auction->bid);
        l_Statement->setUInt32(2, l_Auction->Id);
        l_Transaction->Append(l_Statement);

        /// Not sure if we must send this now.
        Player* l_Owner = ObjectAccessor::FindPlayer(l_Auction->owner);
        Item* l_Item = sAuctionMgr->GetAItem(l_Auction->itemGUIDLow);
        if (l_Owner && l_Item)
            l_Owner->GetSession()->SendAuctionOwnerBidNotification(l_Auction, l_Item);
    }
    else
    {
        // Buyout:
        if (m_Player->GetGUIDLow() == l_Auction->bidder)
            m_Player->ModifyMoney(-int64(l_Auction->buyout - l_Auction->bid), "HandleAuctionPlaceBid " + std::to_string(l_Auction->itemEntry));
        else
        {
            m_Player->ModifyMoney(-int64(l_Auction->buyout), "HandleAuctionPlaceBid " + std::to_string(l_Auction->itemEntry));
            if (l_Auction->bidder)                          //buyout for bidded auction ..
                sAuctionMgr->SendAuctionOutbiddedMail(l_Auction, l_Auction->buyout, m_Player, l_Transaction);
        }

        SendAuctionCommandResult(nullptr, AUCTION_PLACE_BID, ERR_AUCTION_OK);
        l_Auction->bidder = m_Player->GetGUIDLow();
        l_Auction->bid = l_Auction->buyout;
        m_Player->UpdateCriteria(CRITERIA_TYPE_HIGHEST_AUCTION_BID, l_Auction->buyout);

        //- Mails must be under transaction control too to prevent data loss
        sAuctionMgr->SendAuctionSuccessfulMail(l_Auction, l_Transaction);
        sAuctionMgr->SendAuctionWonMail(l_Auction, l_Transaction);

        l_Auction->DeleteFromDB(l_Transaction);

        uint32 l_ItemEntry = l_Auction->itemEntry;
        sAuctionMgr->RemoveAItem(l_Auction->itemGUIDLow);
        l_AuctionHouse->RemoveAuction(l_Auction, l_ItemEntry);
    }

    m_Player->SaveInventoryAndGoldToDB(l_Transaction);
    CharacterDatabase.CommitTransaction(l_Transaction);
}

// This void is called when auction_owner cancels his auction
void WorldSession::HandleAuctionRemoveItem(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;
    uint32 l_AuctionID;

    p_RecvData.readPackGUID(l_Guid);
    p_RecvData >> l_AuctionID;

    Creature* l_Auctioneer = m_Player->GetNPCIfCanInteractWith(l_Guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Auctioneer)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionRemoveItem - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(l_Guid)));
        return;
    }

    // Remove fake death
    if (m_Player->HasUnitState(UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* l_AuctionHouse = sAuctionMgr->GetAuctionsMap(l_Auctioneer->getFaction());

    AuctionEntryPtr l_Auction = l_AuctionHouse->GetAuction(l_AuctionID);

    SQLTransaction l_Trans = CharacterDatabase.BeginTransaction();
    if (l_Auction && l_Auction->owner == m_Player->GetGUIDLow())
    {
        Item* l_Item = sAuctionMgr->GetAItem(l_Auction->itemGUIDLow);
        if (l_Item)
        {
            if (l_Auction->bidder > 0)                        // If we have a bidder, we have to send him the money he paid
            {
                uint64 l_AuctionCut = l_Auction->GetAuctionCut();
                if (!m_Player->HasEnoughMoney((uint64)l_AuctionCut))          //player doesn't have enough money, maybe message needed
                    return;
                sAuctionMgr->SendAuctionCancelledToBidderMail(l_Auction, l_Trans, l_Item);
                m_Player->ModifyMoney(-int64(l_AuctionCut), "HandleAuctionRemoveItem");
            }

            // Item will deleted or added to received mail list
            MailDraft(l_Auction->BuildAuctionMailSubject(AUCTION_CANCELED), AuctionEntry::BuildAuctionMailBody(0, 0, l_Auction->buyout, l_Auction->deposit, 0))
                .AddItem(l_Item)
                .SendMailTo(l_Trans, m_Player, l_Auction, MAIL_CHECK_MASK_COPIED);
        }
        else
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Auction id: %u got non existing item (item guid : %u)!", l_Auction->Id, l_Auction->itemGUIDLow);
            SendAuctionCommandResult(NULL, AUCTION_CANCEL, ERR_AUCTION_DATABASE_ERROR);
            return;
        }
    }
    else
    {
        SendAuctionCommandResult(NULL, AUCTION_CANCEL, ERR_AUCTION_DATABASE_ERROR);
        sLog->outError(LOG_FILTER_NETWORKIO, "CHEATER: %u tried to cancel auction (id: %u) of another player or auction is NULL", m_Player->GetGUIDLow(), l_AuctionID);
        return;
    }

    // Inform player, that auction is removed
    SendAuctionCommandResult(l_Auction, AUCTION_CANCEL, ERR_AUCTION_OK);

    // Now remove the auction
    m_Player->SaveInventoryAndGoldToDB(l_Trans);
    l_Auction->DeleteFromDB(l_Trans);
    CharacterDatabase.CommitTransaction(l_Trans);

    uint32 l_ItemEntry = l_Auction->itemEntry;
    sAuctionMgr->RemoveAItem(l_Auction->itemGUIDLow);
    l_AuctionHouse->RemoveAuction(l_Auction, l_ItemEntry);
}

// Called when player lists his bids
void WorldSession::HandleAuctionListBidderItems(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;                                      //NPC guid
    uint32 l_ListFrom;                                      //page of auctions
    uint32 l_OutbiddedCount;                                //count of outbidded auctions
    std::list<uint32> l_OutbidedAuctionIds;

    p_RecvData.readPackGUID(l_Guid);
    p_RecvData >> l_ListFrom;                               // not used in fact (this list not have page control in client)
    l_OutbiddedCount = p_RecvData.ReadBits(7);
    p_RecvData.ResetBitReading();

    for (uint8 l_Iter = 0; l_Iter < l_OutbiddedCount; ++l_Iter)
        l_OutbidedAuctionIds.push_back(p_RecvData.read<uint32>());

    Creature* l_Auctioneer = m_Player->GetNPCIfCanInteractWith(l_Guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Auctioneer)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionListBidderItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(l_Guid)));
        return;
    }

    // remove fake death
    if (m_Player->HasUnitState(UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* l_AuctionHouse = sAuctionMgr->GetAuctionsMap(l_Auctioneer->getFaction());

    WorldPacket l_Data(SMSG_AUCTION_LIST_BIDDER_ITEMS_RESULT, 10 * 1024);
    uint32 l_Count = 0;
    uint32 l_TotalCount = 0;

    size_t l_CountPos = l_Data.wpos();
    l_Data << l_Count;
    l_Data << uint32(300);                  ///<
    size_t l_TotalCountPos = l_Data.wpos();
    l_Data << l_TotalCount;                 ///<

    l_AuctionHouse->BuildListBidderItems(l_Data, m_Player, l_Count, l_TotalCount);

    l_Data.put(l_CountPos, l_Count);
    l_Data.put(l_TotalCountPos, l_TotalCount);

    SendPacket(&l_Data);
}

// This void sends player info about his auctions
void WorldSession::HandleAuctionListOwnerItems(WorldPacket& p_RecvData)
{
    uint32 l_ListFrom;
    uint64 l_Guid = 0;

    p_RecvData.readPackGUID(l_Guid);
    p_RecvData >> l_ListFrom;                                  // not used in fact (this list not have page control in client)

    Creature* l_Auctioneer = m_Player->GetNPCIfCanInteractWith(l_Guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Auctioneer)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionListOwnerItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(l_Guid)));
        return;
    }

    // remove fake death
    if (m_Player->HasUnitState(UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* l_AuctionHouse = sAuctionMgr->GetAuctionsMap(l_Auctioneer->getFaction());

    WorldPacket l_Data(SMSG_AUCTION_LIST_OWNER_ITEMS_RESULT, 100 * 1024);

    size_t l_CountPos = l_Data.wpos();
    l_Data << uint32(0);                                    // Count place holder
    l_Data << uint32(300);                                  // Desired Delay
    size_t l_TotalCountPos = l_Data.wpos();
    l_Data << uint32(0);                                    // TotalCount place holder

    uint32 l_Count = 0;
    uint32 l_TotalCount = 0;

    l_AuctionHouse->BuildListOwnerItems(l_Data, m_Player, l_Count, l_TotalCount);

    l_Data.put<uint32>(l_CountPos, l_Count);
    l_Data.put<uint32>(l_TotalCountPos, l_TotalCount);

    SendPacket(&l_Data);
}

// This void is called when player clicks on search button
void WorldSession::HandleAuctionListItems(WorldPacket& p_RecvData)
{
    AuctionListItems l_AuctionListItems(p_RecvData);

    Creature* l_Auctioneer = m_Player->GetNPCIfCanInteractWith(l_AuctionListItems.Auctioneer, NPCFlags::UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Auctioneer)
    {
        sLog->outDebug(LogFilterType::LOG_FILTER_NETWORKIO, "WORLD: HandleAuctionListItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(l_AuctionListItems.Auctioneer)));
        return;
    }

    /// Remove fake death
    if (m_Player->HasUnitState(UnitState::UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(AuraType::SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* l_AuctionHouse = sAuctionMgr->GetAuctionsMap(l_Auctioneer->getFaction());

    // converting string that we try to find to lower case
    std::wstring l_SearcherName;
    if (!Utf8toWStr(l_AuctionListItems.Name, l_SearcherName))
        return;

    wstrToLower(l_SearcherName);

    WorldPacket l_Data(Opcodes::SMSG_AUCTION_LIST_ITEMS_RESULT, 10 * 1024);
    Optional<AuctionSearchFilters> l_Filters;

    if (!l_AuctionListItems.ClassFilters.empty())
    {
        l_Filters.emplace();

        for (auto const& l_ClassFilter : l_AuctionListItems.ClassFilters)
        {
            if (!l_ClassFilter.SubClassFilters.empty())
            {
                for (auto const& l_SubClassFilter : l_ClassFilter.SubClassFilters)
                {
                    l_Filters->Classes[l_ClassFilter.ItemClass].SubclassMask |= 1 << l_SubClassFilter.ItemSubclass;
                    l_Filters->Classes[l_ClassFilter.ItemClass].InvTypes[l_SubClassFilter.ItemSubclass] = l_SubClassFilter.InvTypeMask;
                }
            }
            else
                l_Filters->Classes[l_ClassFilter.ItemClass].SubclassMask = AuctionSearchFilters::FilterSkipSubclass;
        }
    }

    l_AuctionHouse->BuildListAuctionItems(l_Data, m_Player, l_SearcherName, l_AuctionListItems, l_Filters);

    m_Player->GetSession()->SendPacket(&l_Data);
}

void WorldSession::HandleAuctionListPendingSales(WorldPacket& /*p_RecvData*/)
{
    WorldPacket l_Data(SMSG_AUCTION_LIST_PENDING_SALES_RESULT, 4);
    l_Data << uint32(0);    ///< mail
    SendPacket(&l_Data);
}

/// @TODO
void WorldSession::HandleReplicateItems(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;
    uint32 l_ChangeNumberGlobal, l_ChangeNumberCursor, l_ChangeNumberTombstone, l_Count;

    p_RecvData.readPackGUID(l_Guid);

    p_RecvData >> l_ChangeNumberGlobal;
    p_RecvData >> l_ChangeNumberCursor;
    p_RecvData >> l_ChangeNumberTombstone;
    p_RecvData >> l_Count;

    Creature* l_Auctioneer = m_Player->GetNPCIfCanInteractWith(l_Guid, NPCFlags::UNIT_NPC_FLAG_AUCTIONEER);
    if (!l_Auctioneer)
    {
        sLog->outDebug(LogFilterType::LOG_FILTER_NETWORKIO, "WORLD: HandleReplicateItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(l_Guid)));
        return;
    }

    /// Remove fake death
    if (m_Player->HasUnitState(UnitState::UNIT_STATE_DIED))
        m_Player->RemoveAurasByType(AuraType::SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* l_AuctionHouse = sAuctionMgr->GetAuctionsMap(l_Auctioneer->getFaction());

    /*WorldPacket l_Data(Opcodes::SMSG_AUCTION_REPLICATE_RESPONSE, 1024);
    SendPacket(&l_Data);*/
}
#endif
