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
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "AuctionHouseMgr.h"
#include "Item.h"
#include "Language.h"
#include "Log.h"
#include "Common.h"
#include <algorithm>

enum eAuctionHouse
{
    AH_MINIMUM_DEPOSIT = 100
};

AuctionHouseMgr::AuctionHouseMgr()
{
}

AuctionHouseMgr::~AuctionHouseMgr()
{
}

AuctionHouseObject* AuctionHouseMgr::GetAuctionsMap(uint32 factionTemplateId)
{
    if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
        return &mNeutralAuctions;

    // team have linked auction houses
    FactionTemplateEntry const* u_entry = sFactionTemplateStore.LookupEntry(factionTemplateId);
    if (!u_entry)
        return &mNeutralAuctions;
    else if (u_entry->FactionGroup & FACTION_MASK_ALLIANCE)
        return &mAllianceAuctions;
    else if (u_entry->FactionGroup & FACTION_MASK_HORDE)
        return &mHordeAuctions;
    else
        return &mNeutralAuctions;
}

uint64 AuctionHouseMgr::GetAuctionDeposit(uint32 p_Time, Item* p_Item, uint32 p_Count)
{
    uint32 l_SellPrice = p_Item->GetTemplate()->SellPrice;

    if (l_SellPrice <= 0)
        return eAuctionHouse::AH_MINIMUM_DEPOSIT * sWorld->getRate(Rates::RATE_AUCTION_DEPOSIT);

    /// Magic value found in full-debug client, this is working just right
    float l_Multiplier = 0.0041666669f;
    uint32 l_RunTime = p_Time / 60;
    uint64 l_Deposit = p_Count * uint32(l_RunTime * l_Multiplier * floor(l_SellPrice * 5.0f / 100.0f)) * sWorld->getRate(Rates::RATE_AUCTION_DEPOSIT);

    sLog->outDebug(LogFilterType::LOG_FILTER_AUCTIONHOUSE, "SellPrice:  %u", l_SellPrice);
    sLog->outDebug(LogFilterType::LOG_FILTER_AUCTIONHOUSE, "Items:      %u", p_Count);
    sLog->outDebug(LogFilterType::LOG_FILTER_AUCTIONHOUSE, "Deposit:    " UI64FMTD "", l_Deposit);

    if (l_Deposit < eAuctionHouse::AH_MINIMUM_DEPOSIT * sWorld->getRate(Rates::RATE_AUCTION_DEPOSIT))
        return eAuctionHouse::AH_MINIMUM_DEPOSIT * sWorld->getRate(Rates::RATE_AUCTION_DEPOSIT);
    else
        return l_Deposit;
}

//does not clear ram
void AuctionHouseMgr::SendAuctionWonMail(AuctionEntryPtr auction, SQLTransaction& trans)
{
    Item* pItem = GetAItem(auction->itemGUIDLow);
    if (!pItem)
        return;

    uint32 bidder_accId = 0;
    uint64 bidder_guid = MAKE_NEW_GUID(auction->bidder, 0, HIGHGUID_PLAYER);
    Player* bidder = ObjectAccessor::FindPlayer(bidder_guid);
    // data for gm.log
    if (sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
    {
        uint32 bidder_security = 0;
        std::string bidder_name;
        if (bidder)
        {
            bidder_accId = bidder->GetSession()->GetAccountId();
            bidder_security = bidder->GetSession()->GetSecurity();
            bidder_name = bidder->GetName();
        }
        else
        {
            bidder_accId = sObjectMgr->GetPlayerAccountIdByGUID(bidder_guid);
            bidder_security = sWorld->GetAccountSecurity(bidder_accId);

            if (!AccountMgr::IsPlayerAccount(bidder_security)) // not do redundant DB requests
            {
                if (!sObjectMgr->GetPlayerNameByGUID(bidder_guid, bidder_name))
                    bidder_name = sObjectMgr->GetTrinityStringForDB2Locale(LANG_UNKNOWN);
            }
        }
        if (!AccountMgr::IsPlayerAccount(bidder_security))
        {
            std::string owner_name;
            if (!sObjectMgr->GetPlayerNameByGUID(auction->owner, owner_name))
                owner_name = sObjectMgr->GetTrinityStringForDB2Locale(LANG_UNKNOWN);

            uint32 owner_accid = sObjectMgr->GetPlayerAccountIdByGUID(auction->owner);

            sLog->outCommand(bidder_accId, "", 0, bidder_name.c_str(), owner_accid, "", 0, owner_name.c_str(),
                "GM %s (Account: %u) won item in auction: %s (Entry: %u Count: %u) and pay money: " UI64FMTD ". Original owner %s (Account: %u)",
                bidder_name.c_str(), bidder_accId, pItem->GetTemplate()->Name1->Get(sWorld->GetDefaultDb2Locale()), pItem->GetEntry(), pItem->GetCount(), auction->bid, owner_name.c_str(), owner_accid);
        }
    }

    std::ostringstream l_ItemBonuses;
    for (auto l_Bonus : pItem->GetAllItemBonuses())
        l_ItemBonuses << l_Bonus << " ";

    auto l_OwnerCharData = sWorld->GetCharacterInfo(auction->owner);
    bool l_BotSell = l_OwnerCharData && l_OwnerCharData->AccountId == sWorld->getIntConfig(CONFIG_AUCTION_HOUSE_BOT_ACCOUNT);

    PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_AUCTION_HOUSE_SELL_LOG);
    l_Statement->setUInt32(0, time(nullptr));
    l_Statement->setUInt32(1, pItem->GetEntry());
    l_Statement->setString(2, l_ItemBonuses.str());
    l_Statement->setUInt32(3, pItem->GetCount());
    l_Statement->setUInt32(4, pItem->GetItemSuffixFactor());
    l_Statement->setInt32(5, pItem->GetItemRandomPropertyId());
    l_Statement->setInt64(6, auction->buyout);
    l_Statement->setBool(7, l_BotSell);
    LogDatabase.Execute(l_Statement);

    // receiver exist
    if (bidder || bidder_accId)
    {
        // set owner to bidder (to prevent delete item with sender char deleting)
        // owner in `data` will set at mail receive and item extracting
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ITEM_OWNER);
        stmt->setUInt32(0, auction->bidder);
        stmt->setUInt32(1, pItem->GetGUIDLow());
        trans->Append(stmt);

        if (bidder)
        {
            bidder->GetSession()->SendAuctionWonNotification(auction, pItem);
            // FIXME: for offline player need also
            bidder->UpdateCriteria(CRITERIA_TYPE_WON_AUCTIONS, 1);
        }

        MailDraft(auction->BuildAuctionMailSubject(AUCTION_WON), AuctionEntry::BuildAuctionMailBody(auction->owner, auction->bid, auction->buyout, 0, 0))
            .AddItem(pItem)
            .SendMailTo(trans, MailReceiver(bidder, auction->bidder), auction, MAIL_CHECK_MASK_COPIED);
    }
}

/// Call this method to send mail to auction owner, when auction is successful, it does not clear ram
void AuctionHouseMgr::SendAuctionSuccessfulMail(AuctionEntryPtr p_Auction, SQLTransaction& p_Transaction)
{
    uint64 l_OwnerGUID = MAKE_NEW_GUID(p_Auction->owner, 0, HIGHGUID_PLAYER);
    Player* l_Owner = ObjectAccessor::FindPlayer(l_OwnerGUID);
    uint32 l_OwnerAccID = sObjectMgr->GetPlayerAccountIdByGUID(l_OwnerGUID);
    Item* l_Item = GetAItem(p_Auction->itemGUIDLow);

    /// owner exist
    if (l_Owner || l_OwnerAccID)
    {
        uint64 l_Profit = p_Auction->bid + p_Auction->deposit - p_Auction->GetAuctionCut();

        ///FIXME: what do if owner offline
        if (l_Owner && l_Item)
        {
            l_Owner->UpdateCriteria(CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS, l_Profit);
            l_Owner->UpdateCriteria(CRITERIA_TYPE_HIGHEST_AUCTION_SOLD, p_Auction->bid);
            /// Send auction owner notification, bidder must be current!
            l_Owner->GetSession()->SendAuctionClosedNotification(p_Auction, float(sWorld->getIntConfig(CONFIG_MAIL_DELIVERY_DELAY)), true, l_Item);
        }

        MailDraft(p_Auction->BuildAuctionMailSubject(AUCTION_SUCCESSFUL), AuctionEntry::BuildAuctionMailBody(p_Auction->bidder, p_Auction->bid, p_Auction->buyout, p_Auction->deposit, p_Auction->GetAuctionCut()))
            .AddMoney(l_Profit)
            .SendMailTo(p_Transaction, MailReceiver(l_Owner, p_Auction->owner), p_Auction, MAIL_CHECK_MASK_COPIED, sWorld->getIntConfig(CONFIG_MAIL_DELIVERY_DELAY));
    }
}

/// Does not clear ram
void AuctionHouseMgr::SendAuctionExpiredMail(AuctionEntryPtr p_Auction, SQLTransaction& p_Transaction)
{
    /// Return an item in auction to its owner by mail
    Item* l_Item = GetAItem(p_Auction->itemGUIDLow);
    if (!l_Item)
        return;

    uint64 l_OwnerGUID = MAKE_NEW_GUID(p_Auction->owner, 0, HIGHGUID_PLAYER);
    Player* l_Owner = ObjectAccessor::FindPlayer(l_OwnerGUID);
    uint32 l_OwnerAccID = sObjectMgr->GetPlayerAccountIdByGUID(l_OwnerGUID);

    /// owner exist
    if (l_Owner || l_OwnerAccID)
    {
        if (l_Owner)
            l_Owner->GetSession()->SendAuctionClosedNotification(p_Auction, 0.0f, false, l_Item);

        MailDraft(p_Auction->BuildAuctionMailSubject(AUCTION_EXPIRED), AuctionEntry::BuildAuctionMailBody(0, 0, p_Auction->buyout, p_Auction->deposit, 0))
            .AddItem(l_Item)
            .SendMailTo(p_Transaction, MailReceiver(l_Owner, p_Auction->owner), p_Auction, MAIL_CHECK_MASK_COPIED, 0);
    }
}

/// This function sends mail to old bidder
void AuctionHouseMgr::SendAuctionOutbiddedMail(AuctionEntryPtr p_Auction, uint64 newPrice, Player* newBidder, SQLTransaction& p_Transaction)
{
    uint64 l_OldBidderGuid = MAKE_NEW_GUID(p_Auction->bidder, 0, HIGHGUID_PLAYER);
    Player* l_OldBidder = ObjectAccessor::FindPlayer(l_OldBidderGuid);
    Item* l_Item = GetAItem(p_Auction->itemGUIDLow);

    uint32 l_OldBidderAccID = 0;
    if (!l_OldBidder)
        l_OldBidderAccID = sObjectMgr->GetPlayerAccountIdByGUID(l_OldBidderGuid);

    /// Old bidder exist
    if (l_OldBidder || l_OldBidderAccID)
    {
        if (l_OldBidder && newBidder)
            l_OldBidder->GetSession()->SendAuctionOutBidNotification(p_Auction, l_Item);

        MailDraft(p_Auction->BuildAuctionMailSubject(AUCTION_OUTBIDDED), AuctionEntry::BuildAuctionMailBody(p_Auction->owner, p_Auction->bid, p_Auction->buyout, p_Auction->deposit, p_Auction->GetAuctionCut()))
            .AddMoney(p_Auction->bid)
            .SendMailTo(p_Transaction, MailReceiver(l_OldBidder, p_Auction->bidder), p_Auction, MAIL_CHECK_MASK_COPIED);
    }
}

//this function sends mail, when auction is cancelled to old bidder
void AuctionHouseMgr::SendAuctionCancelledToBidderMail(AuctionEntryPtr auction, SQLTransaction& trans, Item* /*item*/)
{
    uint64 bidder_guid = MAKE_NEW_GUID(auction->bidder, 0, HIGHGUID_PLAYER);
    Player* bidder = ObjectAccessor::FindPlayer(bidder_guid);

    uint32 bidder_accId = 0;
    if (!bidder)
        bidder_accId = sObjectMgr->GetPlayerAccountIdByGUID(bidder_guid);

    // bidder exist
    if (bidder || bidder_accId)
        MailDraft(auction->BuildAuctionMailSubject(AUCTION_CANCELLED_TO_BIDDER), AuctionEntry::BuildAuctionMailBody(auction->owner, auction->bid, auction->buyout, auction->deposit, 0))
            .AddMoney(auction->bid)
            .SendMailTo(trans, MailReceiver(bidder, auction->bidder), auction, MAIL_CHECK_MASK_COPIED);
}

void AuctionHouseMgr::LoadAuctionItems()
{
    uint32 oldMSTime = getMSTime();

    // need to clear in case we are reloading
    if (!mAitems.empty())
    {
        for (ItemMap::iterator itr = mAitems.begin(); itr != mAitems.end(); ++itr)
            delete itr->second;

        mAitems.clear();
    }

    if (uint32 l_BotAccId = sWorld->getIntConfig(CONFIG_AUCTION_HOUSE_BOT_ACCOUNT))
    {
        QueryResult l_Result = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account = %u", l_BotAccId);
        if (l_Result)
        {
            do
            {
                Field* l_Field = l_Result->Fetch();
                uint32 l_Guid = l_Field[0].GetUInt32();
                sLog->outAshran("AuctionHouseMgr::LoadAuctionItems: bot char %u", l_Guid);
                g_AuctionHouseBotCharacters.push_back(l_Guid);
            }
            while (l_Result->NextRow());
        }
    }

    /// Load auction house sell log with price higher than 10.000 golds of this month
    QueryResult l_Result = CharacterDatabase.PQuery("SELECT itemEntry, itemBonuses, price FROM auctionhouse_sell_log WHERE price > 100000000 and time > %u", time(nullptr) - MONTH);
    if (l_Result)
    {
        do
        {
            Field* l_Field = l_Result->Fetch();

            uint32 l_ItemId          = l_Field[0].GetUInt32();
            std::string l_BonusesTxt = l_Field[1].GetString();
            uint64 l_Price           = l_Field[2].GetUInt64();

            ItemTemplate const* l_ItemTemplate = sObjectMgr->GetItemTemplate(l_ItemId);
            if (!l_ItemTemplate)
                continue;

            /// Compute the item ilevel based on his bonuses
            BonusData l_BonusData;
            l_BonusData.Initialize(l_ItemTemplate);

            std::vector<uint32> l_Bonuses;
            Tokenizer l_BonusTokens(l_BonusesTxt, ' ');
            for (uint8 l_I = 0; l_I < l_BonusTokens.size(); ++l_I)
            {
                uint32 l_BonusID = atoi(l_BonusTokens[l_I]);
                l_Bonuses.push_back(l_BonusID);

                auto l_Bonuses = GetItemBonusesByID(l_BonusID);
                if (l_Bonuses)
                {
                    for (auto l_Bonus : *l_Bonuses)
                    {
                        if (l_Bonus == nullptr)
                            continue;

                        l_BonusData.AddBonus(l_Bonus->Type, l_Bonus->Value);
                    }
                }
            }

            uint32 l_ItemLevel = l_ItemTemplate->GetBaseItemLevel();
            if (l_BonusData.HasItemLevelBonus || !l_BonusData.ItemLevelOverride)
                l_ItemLevel += l_BonusData.ItemLevelBonus;
            else
                l_ItemLevel = l_BonusData.ItemLevelOverride;

            /// We only care about gear items, skip if the item level is lower than heroic dungeon legion items
            if (l_ItemLevel < 825)
                continue;

            uint32 l_Key = l_ItemTemplate->Class << 16 | l_ItemTemplate->SubClass << 8 | l_ItemTemplate->InventoryType;

            AuctionHouseHistoryEntry* l_HistoryEntry = nullptr;
            std::vector<AuctionHouseHistoryEntry*>& l_HistoryEntries = m_SellLogs[l_Key][l_ItemLevel];

            /// Check if the item with the same bonuses list already exist in the history list
            for (AuctionHouseHistoryEntry* l_Entry : l_HistoryEntries)
            {
                if (l_Entry->ItemEntry == l_ItemId && l_Entry->Bonuses.size() == l_Bonuses.size())
                {
                    bool l_Match = true;
                    for (uint32 l_Idx = 0; l_Idx < l_Bonuses.size(); l_Idx++)
                    {
                        if (l_Entry->Bonuses[l_Idx] != l_Bonuses[l_Idx])
                        {
                            l_Match = false;
                            break;
                        }
                    }

                    if (!l_Match)
                        continue;

                    l_HistoryEntry = l_Entry;
                }
            }

            /// Item didn't exist yet, create the history entry
            if (!l_HistoryEntry)
            {
                l_HistoryEntry = new AuctionHouseHistoryEntry();
                l_HistoryEntry->ItemEntry = l_ItemId;

                for (auto l_BonusID : l_Bonuses)
                    l_HistoryEntry->Bonuses.push_back(l_BonusID);

                l_HistoryEntries.push_back(l_HistoryEntry);
            }

            l_HistoryEntry->Prices.push_back(l_Price);
        }
        while (l_Result->NextRow());
    }

    for (auto l_Itr : m_SellLogs)
    {
        sLog->outAshran("m_SellLogs[%u]:", l_Itr.first);
        for (auto l_Itr2 : l_Itr.second)
            sLog->outAshran("----------->[%u] (%u entries) ", l_Itr2.first, (uint32)l_Itr2.second.size());
    }

    // data needs to be at first place for Item::LoadFromDB
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_AUCTION_ITEMS);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 auction items. DB table `auctionhouse` or `item_instance` is empty!");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 item_guid    = fields[ItemInstanceField::Guid].GetUInt32();
        uint32 itemEntry    = fields[ItemInstanceField::ItemEntry].GetUInt32();

        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemEntry);
        if (!proto)
        {
            sLog->outError(LOG_FILTER_GENERAL, "AuctionHouseMgr::LoadAuctionItems: Unknown item (GUID: %u id: #%u) in auction, skipped.", item_guid, itemEntry);
            continue;
        }

        Item* item = NewItemOrBag(proto);
        if (!item->LoadFromDB(item_guid, 0, fields, itemEntry))
        {
            delete item;
            continue;
        }
        AddAItem(item);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u auction items in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void AuctionHouseMgr::LoadAuctions()
{
    uint32 oldMSTime = getMSTime();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_AUCTIONS);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 auctions. DB table `auctionhouse` is empty.");

        return;
    }

    uint32 count = 0;

    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    do
    {
        Field* fields = result->Fetch();

        AuctionEntryPtr aItem = std::make_shared<AuctionEntry>(AuctionEntry());
        if (!aItem->LoadFromDB(fields))
        {
            aItem->DeleteFromDB(trans);
            continue;
        }

        GetAuctionsMap(aItem->factionTemplateId)->AddAuction(aItem);
        count++;
    }
    while (result->NextRow());

    CharacterDatabase.CommitTransaction(trans);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u auctions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));

}

void AuctionHouseMgr::AddAItem(Item* it)
{
    ASSERT(it);
    ASSERT(mAitems.find(it->GetGUIDLow()) == mAitems.end());
    mAitems[it->GetGUIDLow()] = it;
}

bool AuctionHouseMgr::RemoveAItem(uint32 id)
{
    ItemMap::iterator i = mAitems.find(id);
    if (i == mAitems.end())
        return false;

    mAitems.erase(i);
    return true;
}

void AuctionHouseMgr::Update()
{
    mHordeAuctions.Update();
    mAllianceAuctions.Update();
    mNeutralAuctions.Update();

    if (time(nullptr) > sWorld->getWorldState(WS_NEXT_AUCTION_HOUSE_BOT_UPDATE))
    {
        mNeutralAuctions.UpdateAHBot();
        sWorld->setWorldState(WS_NEXT_AUCTION_HOUSE_BOT_UPDATE, time(nullptr) + HOUR * 3);
    }
}

AuctionHouseEntry const* AuctionHouseMgr::GetAuctionHouseEntry(uint32 factionTemplateId)
{
    uint32 houseid = 7; // goblin auction house

    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        //FIXME: found way for proper auctionhouse selection by another way
        // AuctionHouse.db2 have faction field with _player_ factions associated with auction house races.
        // but no easy way convert creature faction to player race faction for specific city
        switch (factionTemplateId)
        {
            case   12: houseid = 1; break; // human
            case   29: houseid = 6; break; // orc, and generic for horde
            case   55: houseid = 2; break; // dwarf, and generic for alliance
            case   68: houseid = 4; break; // undead
            case   80: houseid = 3; break; // n-elf
            case  104: houseid = 5; break; // trolls
            case  120: houseid = 7; break; // booty bay, neutral
            case  474: houseid = 7; break; // gadgetzan, neutral
            case  855: houseid = 7; break; // everlook, neutral
            case 1604: houseid = 6; break; // b-elfs,
            default:                       // for unknown case
            {
                FactionTemplateEntry const* u_entry = sFactionTemplateStore.LookupEntry(factionTemplateId);
                if (!u_entry)
                    houseid = 7; // goblin auction house
                else if (u_entry->FactionGroup & FACTION_MASK_ALLIANCE)
                    houseid = 1; // human auction house
                else if (u_entry->FactionGroup & FACTION_MASK_HORDE)
                    houseid = 6; // orc auction house
                else
                    houseid = 7; // goblin auction house
                break;
            }
        }
    }

    return sAuctionHouseStore.LookupEntry(houseid);
}

void AuctionHouseObject::AddAuction(AuctionEntryPtr auction)
{
    ASSERT(auction);

    AuctionsMap[auction->Id] = auction;
    sScriptMgr->OnAuctionAdd(this, auction);
}

bool AuctionHouseObject::RemoveAuction(AuctionEntryPtr auction, uint32 /*itemEntry*/)
{
    bool wasInMap = AuctionsMap.erase(auction->Id) ? true : false;

    sScriptMgr->OnAuctionRemove(this, auction);

    // we need to delete the entry, it is not referenced any more
    return wasInMap;
}

template <typename It>
auto Median(It begin, It end)
{
    const auto size = std::distance(begin, end);
    std::nth_element(begin, begin + size / 2, end);
    return *std::next(begin, size / 2);
}

void AuctionHouseObject::UpdateAHBot()
{
    if (!sWorld->getIntConfig(CONFIG_AUCTION_HOUSE_BOT_ACCOUNT) || g_AuctionHouseBotCharacters.empty())
      return;

    ItemSubclassArmor l_ArmorTypes[] = { ITEM_SUBCLASS_ARMOR_MISCELLANEOUS, ITEM_SUBCLASS_ARMOR_CLOTH, ITEM_SUBCLASS_ARMOR_LEATHER, ITEM_SUBCLASS_ARMOR_MAIL, ITEM_SUBCLASS_ARMOR_PLATE };
    InventoryType     l_ArmorSlots[] = { INVTYPE_HEAD, INVTYPE_SHOULDERS, INVTYPE_CHEST, INVTYPE_WAIST, INVTYPE_LEGS, INVTYPE_FEET, INVTYPE_WRISTS, INVTYPE_HANDS, INVTYPE_NECK, INVTYPE_TRINKET, INVTYPE_CLOAK, INVTYPE_FINGER };

    std::unordered_map<uint32, uint32> l_BotItemsForSell;

    time_t l_CurrTime = sWorld->GetGameTime();

    /// Build the map of what we currently sell
    AuctionsMap.safe_foreach(true, [&](AuctionEntryMap::iterator& p_Itr)
    {
        AuctionEntryPtr l_AHentry = p_Itr->second;
        if (!l_AHentry)
            return;

        /// Skip expired auctions
        if (l_AHentry->expire_time < l_CurrTime)
            return;

        auto l_CharacterInfo = sWorld->GetCharacterInfo(l_AHentry->owner);
        if (!l_CharacterInfo || l_CharacterInfo->AccountId != sWorld->getIntConfig(CONFIG_AUCTION_HOUSE_BOT_ACCOUNT))
            return;

        Item* l_Item = sAuctionMgr->GetAItem(l_AHentry->itemGUIDLow);
        if (!l_Item)
            return;

        uint32 l_Key = l_Item->GetTemplate()->SubClass << 8 | l_Item->GetTemplate()->InventoryType;

        auto l_Find = l_BotItemsForSell.find(l_Key);
        if (l_Find == l_BotItemsForSell.end())
            l_BotItemsForSell[l_Key] = 1;
        else
            l_Find->second++;

        sLog->outAshran("AuctionHouseObject::UpdateAHBot: char %u item %u counter %u", l_AHentry->owner, l_Item->GetEntry(), l_BotItemsForSell[l_Key]);
    });

    for (ItemSubclassArmor l_ArmorType : l_ArmorTypes)
    {
        for (InventoryType l_ArmorSlot : l_ArmorSlots)
        {
            int8 l_MissingItems = 0;
            uint32 l_Key = l_ArmorType << 8 | l_ArmorSlot;
            auto l_Find = l_BotItemsForSell.find(l_Key);

            if (l_Find == l_BotItemsForSell.end())
                l_MissingItems = 3;
            else
                l_MissingItems = 3 - l_Find->second;

            sLog->outAshran("AuctionHouseObject::UpdateAHBot: Try to add %u %u (missing %u)", l_ArmorType, l_ArmorSlot, l_MissingItems);

            if (l_MissingItems <= 0)
                continue;

            uint32 l_ItemTypeKey = ItemClass::ITEM_CLASS_ARMOR << 16 | l_ArmorType << 8 | l_ArmorSlot;
            std::vector<uint16> l_PossibleItemLevel;

            for (uint32 l_Ilvl = 840; l_Ilvl < 865; l_Ilvl += 5)
            {
                if (sAuctionMgr->SellHistorysContainsItemTypeWithItemLevel(l_ItemTypeKey, l_Ilvl))
                    l_PossibleItemLevel.push_back(l_Ilvl);
            }

            for (auto l_Ilvl : l_PossibleItemLevel)
                sLog->outAshran("AuctionHouseObject::UpdateAHBot: ------> ilvl possible %u", l_Ilvl);

            /// Not enough sell history data to fill this item category
            if (l_PossibleItemLevel.empty())
                continue;

            std::random_shuffle(l_PossibleItemLevel.begin(), l_PossibleItemLevel.end());

            for (uint8 l_I = 0; l_I < l_MissingItems; l_I++)
            {
                AuctionHouseHistoryEntry* l_ItemSellData = sAuctionMgr->GetRandomItemTypeWithItemLevelInSellHistory(l_ItemTypeKey, l_PossibleItemLevel[l_I]);
                if (!l_ItemSellData)
                    continue;

                std::vector<uint64>& l_PricesData = l_ItemSellData->Prices;
                uint64 l_Price = l_PricesData.size() < 3 ? l_PricesData[l_ItemSellData->Prices.size() - 1] : Median(l_PricesData.begin(), l_PricesData.end());

                Item* l_Item = Item::CreateItem(l_ItemSellData->ItemEntry, 1);
                for (auto l_Bonus : l_ItemSellData->Bonuses)
                    l_Item->AddItemBonus(l_Bonus);

                AuctionEntryPtr l_AHEntry = std::make_shared<AuctionEntry>(AuctionEntry());
                l_AHEntry->Id           = sObjectMgr->GenerateAuctionID();
                l_AHEntry->auctioneer   = 174444;
                l_AHEntry->itemGUIDLow  = l_Item->GetGUIDLow();
                l_AHEntry->itemEntry    = l_Item->GetEntry();
                l_AHEntry->itemCount    = l_Item->GetCount();
                l_AHEntry->owner        = g_AuctionHouseBotCharacters[rand() % g_AuctionHouseBotCharacters.size()];
                l_AHEntry->startbid     = l_Price - GOLD * 1000;
                l_AHEntry->bidder       = 0;
                l_AHEntry->bid          = 0;
                l_AHEntry->buyout       = l_Price;
                l_AHEntry->expire_time  = time(NULL) + 2 * MIN_AUCTION_TIME;
                l_AHEntry->deposit      = 0;
                l_AHEntry->auctionHouseEntry = sAuctionHouseStore.LookupEntry(7);

                sAuctionMgr->AddAItem(l_Item);
                AddAuction(l_AHEntry);

                SQLTransaction l_Trans = CharacterDatabase.BeginTransaction();
                l_Item->SaveToDB(l_Trans);
                l_AHEntry->SaveToDB(l_Trans);
                CharacterDatabase.CommitTransaction(l_Trans);

                sLog->outAshran("AuctionHouseObject::UpdateAHBot: add item %u (ilvl %u)", l_Item->GetEntry(), l_PossibleItemLevel[l_I]);
            }
        }
    }
}

void AuctionHouseObject::Update()
{
    time_t curTime = sWorld->GetGameTime();
    ///- Handle expired auctions

    // If storage is empty, no need to update. next == NULL in this case.
    if (AuctionsMap.empty())
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_AUCTION_BY_TIME);
    stmt->setUInt32(0, (uint32)curTime+60);

    PreparedQueryResult l_Result = CharacterDatabase.Query(stmt);
    if (!l_Result)
        return;

    do
    {
        // from auctionhousehandler.cpp, creates auction pointer & player pointer
        AuctionEntryPtr auction = GetAuction(l_Result->Fetch()->GetUInt32());

        if (!auction)
            continue;

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        ///- Either cancel the auction if there was no bidder
        if (auction->bidder == 0)
        {
            sAuctionMgr->SendAuctionExpiredMail(auction, trans);
            sScriptMgr->OnAuctionExpire(this, auction);
        }
        ///- Or perform the transaction
        else
        {
            //we should send an "item sold" message if the seller is online
            //we send the item to the winner
            //we send the money to the seller
            sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
            sAuctionMgr->SendAuctionWonMail(auction, trans);
            sScriptMgr->OnAuctionSuccessful(this, auction);
        }

        uint32 itemEntry = auction->itemEntry;

        ///- In any case clear the auction
        auction->DeleteFromDB(trans);
        CharacterDatabase.CommitTransaction(trans);

        sAuctionMgr->RemoveAItem(auction->itemGUIDLow);
        RemoveAuction(auction, itemEntry);
    }
    while (l_Result->NextRow());
}

void AuctionHouseObject::BuildListBidderItems(WorldPacket& data, Player* player, uint32& count, uint32& totalcount)
{
    AuctionsMap.safe_foreach(true, [&](AuctionEntryMap::iterator& p_Itr)
    {
        AuctionEntryPtr Aentry = p_Itr->second;
        if (Aentry && Aentry->bidder == player->GetGUIDLow())
        {
            if (p_Itr->second->BuildAuctionInfo(data, false))
                ++count;

            ++totalcount;
        }
    });
}

void AuctionHouseObject::BuildListOwnerItems(WorldPacket& data, Player* player, uint32& count, uint32& totalcount)
{
    AuctionsMap.safe_foreach(true, [&](AuctionEntryMap::iterator& p_Itr)
    {
        AuctionEntryPtr Aentry = p_Itr->second;
        if (Aentry && Aentry->owner == player->GetGUIDLow())
        {
            if (Aentry->BuildAuctionInfo(data, false))
                ++count;

            ++totalcount;
        }
    });
}

void AuctionHouseObject::BuildListAuctionItems(WorldPacket& p_Data, Player* p_Player, std::wstring const& p_SearchedName, AuctionListItems p_ListItems, Optional<AuctionSearchFilters> const& p_Filters)
{
    time_t l_CurrTime = sWorld->GetGameTime();

    uint32 l_Count          = 0;
    size_t l_CountPos       = p_Data.wpos();

    p_Data << l_Count;

    uint32 l_TotalCount     = 0;
    size_t l_TotalCountPos  = p_Data.wpos();

    p_Data << l_TotalCount;
    p_Data << uint32(300);  ///< DesiredDelay

    p_Data.WriteBit(p_ListItems.OnlyUsable);

    p_Data.FlushBits();

    AuctionsMap.safe_foreach(true, [&](AuctionEntryMap::iterator& p_Itr)
    {
        AuctionEntryPtr l_AuctionEntry = p_Itr->second;

        /// Skip expired auctions
        if (l_AuctionEntry->expire_time < l_CurrTime)
            return;

        Item* l_Item = sAuctionMgr->GetAItem(l_AuctionEntry->itemGUIDLow);
        if (!l_Item)
            return;

        ItemTemplate const* l_Proto = l_Item->GetTemplate();
        if (p_Filters)
        {
            /// If we don't want any class filters, Optional is not initialized
            /// If we don't want this class included, SubclassMask is set to FILTER_SKIP_CLASS
            /// If we want this class and did not specify and subclasses, its set to FILTER_SKIP_SUBCLASS
            /// otherwise full restrictions apply
            if (p_Filters->Classes[l_Proto->Class].SubclassMask == AuctionSearchFilters::FilterSkipClass)
                return;

            if (p_Filters->Classes[l_Proto->Class].SubclassMask != AuctionSearchFilters::FilterSkipSubclass)
            {
                if (!(p_Filters->Classes[l_Proto->Class].SubclassMask & (1 << l_Proto->SubClass)))
                    return;

                if (!(p_Filters->Classes[l_Proto->Class].InvTypes[l_Proto->SubClass] & (1 << l_Proto->InventoryType)))
                    return;
            }
        }

        if (p_ListItems.Quality != 0xFFFFFFFF && l_Proto->Quality != p_ListItems.Quality)
            return;

        if (p_ListItems.MinLevel != 0 && (l_Item->GetRequiredLevel() < p_ListItems.MinLevel || (p_ListItems.MaxLevel != 0 && l_Item->GetRequiredLevel() > p_ListItems.MaxLevel)))
            return;

        if (p_ListItems.OnlyUsable && p_Player->CanUseItem(l_Item) != InventoryResult::EQUIP_ERR_OK)
            return;

        /// Allow search by suffix (eg: of the Monkey) or partial name (eg: Monkey)
        /// No need to do any of this if no search term was entered
        if (!p_SearchedName.empty())
        {
            auto localeIndex = p_Player->GetSession()->GetSessionDbLocaleIndex();
            std::string l_Name = l_Proto->Name1->Get(p_Player->GetSession()->GetSessionDbLocaleIndex());
            if (l_Name.empty())
                return;

            /// DO NOT use GetItemEnchantMod(proto->RandomProperty) as it may return a result
            /// that matches the search but it may not equal item->GetItemRandomPropertyId()
            /// used in BuildAuctionInfo() which then causes wrong items to be listed
            if (int32 l_PropRefID = l_Item->GetItemRandomPropertyId())
            {
                /// Append the suffix to the name (eg: of the Monkey) if one exists
                /// These are found in ItemRandomSuffix.db2 and ItemRandomProperties.db2
                /// even though the DB2 names seem misleading

                char const* l_Suffix = nullptr;

                if (l_PropRefID < 0)
                {
                    ItemRandomSuffixEntry const* l_RandSuffixEntry = sItemRandomSuffixStore.LookupEntry(-l_PropRefID);
                    if (l_RandSuffixEntry)
                        l_Suffix = l_RandSuffixEntry->Name->Get(localeIndex);
                }
                else
                {
                    ItemRandomPropertiesEntry const* l_RandPropEntry = sItemRandomPropertiesStore.LookupEntry(l_PropRefID);
                    if (l_RandPropEntry)
                        l_Suffix = l_RandPropEntry->nameSuffix->Get(localeIndex);
                }

                /// db2 local name
                if (l_Suffix)
                {
                    /// Append the suffix (eg: of the Monkey) to the name using localization
                    /// or default enUS if localization is invalid
                    l_Name += ' ';
                    l_Name += l_Suffix;
                }
            }

            /// Perform the search (with or without suffix)
            if (!Utf8FitTo(l_Name, p_SearchedName))
                return;
        }

        /// Add the item if no search term or if entered search term was found
        if (l_Count < 50 && l_TotalCount >= p_ListItems.Offset)
        {
            l_AuctionEntry->BuildAuctionInfo(p_Data, true);
            ++l_Count;
        }

        ++l_TotalCount;
    });

    p_Data.put(l_CountPos, l_Count);
    p_Data.put(l_TotalCountPos, l_TotalCount);
}

/// This function inserts to WorldPacket auction's data
bool AuctionEntry::BuildAuctionInfo(WorldPacket& p_Data, bool p_CensorServerSideInfo) const
{
    Item* l_Item = sAuctionMgr->GetAItem(itemGUIDLow);
    if (!l_Item)
    {
        sLog->outError(LogFilterType::LOG_FILTER_GENERAL, "AuctionEntry::BuildAuctionInfo: Auction %u has a non-existent item: %u", Id, itemGUIDLow);
        return false;
    }

    bool l_CensorBidInfo = false;

    Item::BuildDynamicItemDatas(p_Data, l_Item);

    p_Data << uint32(l_Item->GetCount());
    p_Data << uint32(l_Item->GetSpellCharges());
    p_Data << uint32(0);    ///< Flags
    p_Data << uint32(Id);
    p_Data.appendPackGUID(MAKE_NEW_GUID(owner, 0, HighGuid::HIGHGUID_PLAYER));
    p_Data << uint64(startbid);
    p_Data << uint64(bid ? GetAuctionOutBid() : 0);
    p_Data << uint64(buyout);
    p_Data << uint32((expire_time - time(nullptr)) * TimeConstants::IN_MILLISECONDS);
    p_Data << uint8(0);     ///< Delete reason

    ByteBuffer l_EnchantDatas;
    uint32 l_EnchantCount = 0;
    for (uint8 l_EnchantSlot = 0; l_EnchantSlot < EnchantmentSlot::MAX_ENCHANTMENT_SLOT; ++l_EnchantSlot)
    {
        uint32 l_EnchantID = l_Item->GetEnchantmentId(EnchantmentSlot(l_EnchantSlot));
        if (!l_EnchantID)
            continue;

        ++l_EnchantCount;

        l_EnchantDatas << uint32(l_Item->GetEnchantmentId(EnchantmentSlot(l_EnchantSlot)));
        l_EnchantDatas << uint32(l_Item->GetEnchantmentDuration(EnchantmentSlot(l_EnchantSlot)));
        l_EnchantDatas << uint32(l_Item->GetEnchantmentCharges(EnchantmentSlot(l_EnchantSlot)));
        l_EnchantDatas << uint8(l_EnchantSlot);
    }

    uint32 l_GemCount = 0;
    ByteBuffer l_GemDatas;
    for (ItemDynamicFieldGems const& l_GemData : l_Item->GetGems())
    {
        if (sObjectMgr->GetItemTemplate(l_GemData.ItemId))
        {
            std::vector<uint32> l_Bonuses;
            for (uint16 l_BonusListId : l_GemData.BonusListIDs)
            {
                if (l_BonusListId != 0 && GetItemBonusesByID(l_BonusListId))
                    l_Bonuses.push_back(l_BonusListId);
            }
            l_GemDatas << uint8(l_GemCount);
            Item::BuildDynamicItemDatas(l_GemDatas, l_GemData.ItemId, l_Bonuses);
            ++l_GemCount;
        }
    }

    p_Data.WriteBits(l_EnchantCount, 4);
    p_Data.WriteBits(l_GemCount, 2);
    p_Data.WriteBit(p_CensorServerSideInfo);
    p_Data.WriteBit(l_CensorBidInfo);

    p_Data.FlushBits();

    if (l_GemDatas.size())
        p_Data.append(l_GemDatas);

    if (l_EnchantDatas.size())
        p_Data.append(l_EnchantDatas);

    if (!p_CensorServerSideInfo)
    {
        p_Data.appendPackGUID(l_Item->GetGUID());
        p_Data.appendPackGUID(0);   ///< OwnerAccountID
        p_Data << int32(expire_time);
    }

    if (!l_CensorBidInfo)
    {
        p_Data.appendPackGUID(MAKE_NEW_GUID(bidder, 0, HighGuid::HIGHGUID_PLAYER));
        p_Data << uint64(bid);
    }

    return true;
}

uint64 AuctionEntry::GetAuctionCut() const
{
    int64 cut = int64(CalculatePct(bid, auctionHouseEntry->ConsignmentRate) * sWorld->getRate(RATE_AUCTION_CUT));
    return std::max(cut, int64(0));
}

/// the sum of outbid is (1% from current bid)*5, if bid is very small, it is 1c
uint64 AuctionEntry::GetAuctionOutBid() const
{
    uint64 outbid = CalculatePct(bid, 5);
    return outbid ? outbid : 1;
}

void AuctionEntry::DeleteFromDB(SQLTransaction& trans) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_AUCTION);
    stmt->setUInt32(0, Id);
    trans->Append(stmt);
}

void AuctionEntry::SaveToDB(SQLTransaction& trans) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_AUCTION);
    stmt->setUInt32(0, Id);
    stmt->setUInt32(1, auctioneer);
    stmt->setUInt32(2, itemGUIDLow);
    stmt->setUInt32(3, owner);
    stmt->setUInt64(4, buyout);
    stmt->setUInt32(5, expire_time);
    stmt->setUInt32(6, bidder);
    stmt->setUInt64(7, bid);
    stmt->setUInt64(8, startbid);
    stmt->setUInt64(9, deposit);
    trans->Append(stmt);
}

bool AuctionEntry::LoadFromDB(Field* fields)
{
    uint8 index = 0;

    Id          = fields[index++].GetUInt32();
    auctioneer  = fields[index++].GetUInt32();
    itemGUIDLow = fields[index++].GetUInt32();
    itemEntry   = fields[index++].GetUInt32();
    itemCount   = fields[index++].GetUInt32();
    owner       = fields[index++].GetUInt32();
    buyout      = fields[index++].GetUInt64();
    expire_time = fields[index++].GetUInt32();
    bidder      = fields[index++].GetUInt32();
    bid         = fields[index++].GetUInt64();
    startbid    = fields[index++].GetUInt64();
    deposit     = fields[index++].GetUInt64();

    CreatureData const* auctioneerData = sObjectMgr->GetCreatureData(auctioneer);
    if (!auctioneerData)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Auction %u has not a existing auctioneer (GUID : %u)", Id, auctioneer);
        return false;
    }

    CreatureTemplate const* auctioneerInfo = sObjectMgr->GetCreatureTemplate(auctioneerData->id);
    if (!auctioneerInfo)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Auction %u has not a existing auctioneer (GUID : %u Entry: %u)", Id, auctioneer, auctioneerData->id);
        return false;
    }

    factionTemplateId = auctioneerInfo->faction;
    auctionHouseEntry = AuctionHouseMgr::GetAuctionHouseEntry(factionTemplateId);
    if (!auctionHouseEntry)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Auction %u has auctioneer (GUID : %u Entry: %u) with wrong faction %u", Id, auctioneer, auctioneerData->id, factionTemplateId);
        return false;
    }

    // check if sold item exists for guid
    // and itemEntry in fact (GetAItem will fail if problematic in result check in AuctionHouseMgr::LoadAuctionItems)
    if (!sAuctionMgr->GetAItem(itemGUIDLow))
    {
        sLog->outError(LOG_FILTER_GENERAL, "Auction %u has not a existing item : %u", Id, itemGUIDLow);
        return false;
    }
    return true;
}

void AuctionHouseMgr::DeleteExpiredAuctionsAtStartup()
{
    // Deletes expired auctions. Should be called at server start before loading auctions.

    // DO NOT USE after auctions are already loaded since this deletes from the DB
    //  and assumes the auctions HAVE NOT been loaded into a list or AuctionEntryMap yet

    uint32 oldMSTime = getMSTime();
    uint32 expirecount = 0;
    time_t curTime = sWorld->GetGameTime();

    // Query the DB to see if there are any expired auctions
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXPIRED_AUCTIONS);
    stmt->setUInt32(0, (uint32)curTime+60);
    PreparedQueryResult expAuctions = CharacterDatabase.Query(stmt);

    if (!expAuctions)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> No expired auctions to delete");
        return;
    }

    do
    {
        Field* fields = expAuctions->Fetch();

        AuctionEntryPtr auction = std::make_shared<AuctionEntry>(AuctionEntry());

        // Can't use LoadFromDB() because it assumes the auction map is loaded
        if (!auction->LoadFromFieldList(fields))
        {
            // For some reason the record in the DB is broken (possibly corrupt
            //  faction info). Delete the object and move on.
            continue;
        }

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        if (auction->bidder == 0)
        {
            // Cancel the auction, there was no bidder
            sAuctionMgr->SendAuctionExpiredMail(auction, trans);
        }
        else
        {
            // Send the item to the winner and money to seller
            sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
            sAuctionMgr->SendAuctionWonMail(auction, trans);
        }

        // Call the appropriate AuctionHouseObject script
        //  ** Do we need to do this while core is still loading? **
        sScriptMgr->OnAuctionExpire(GetAuctionsMap(auction->factionTemplateId), auction);

        // Delete the auction from the DB
        auction->DeleteFromDB(trans);
        CharacterDatabase.CommitTransaction(trans);

        // Release memory
        ++expirecount;

    }
    while (expAuctions->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Deleted %u expired auctions in %u ms", expirecount, GetMSTimeDiffToNow(oldMSTime));
}

bool AuctionEntry::LoadFromFieldList(Field* fields)
{
    // Loads an AuctionEntry item from a field list. Unlike "LoadFromDB()", this one
    //  does not require the AuctionEntryMap to have been loaded with items. It simply
    //  acts as a wrapper to fill out an AuctionEntry struct from a field list

    uint8 index = 0;

    Id          = fields[index++].GetUInt32();
    auctioneer  = fields[index++].GetUInt32();
    itemGUIDLow = fields[index++].GetUInt32();
    itemEntry   = fields[index++].GetUInt32();
    itemCount   = fields[index++].GetUInt32();
    owner       = fields[index++].GetUInt32();
    buyout      = fields[index++].GetUInt64();
    expire_time = fields[index++].GetUInt32();
    bidder      = fields[index++].GetUInt32();
    bid         = fields[index++].GetUInt64();
    startbid    = fields[index++].GetUInt64();
    deposit     = fields[index++].GetUInt64();

    CreatureData const* auctioneerData = sObjectMgr->GetCreatureData(auctioneer);
    if (!auctioneerData)
    {
        sLog->outError(LOG_FILTER_GENERAL, "AuctionEntry::LoadFromFieldList() - Auction %u has not a existing auctioneer (GUID : %u)", Id, auctioneer);
        return false;
    }

    CreatureTemplate const* auctioneerInfo = sObjectMgr->GetCreatureTemplate(auctioneerData->id);
    if (!auctioneerInfo)
    {
        sLog->outError(LOG_FILTER_GENERAL, "AuctionEntry::LoadFromFieldList() - Auction %u has not a existing auctioneer (GUID : %u Entry: %u)", Id, auctioneer, auctioneerData->id);
        return false;
    }

    factionTemplateId = auctioneerInfo->faction;
    auctionHouseEntry = AuctionHouseMgr::GetAuctionHouseEntry(factionTemplateId);

    if (!auctionHouseEntry)
    {
        sLog->outError(LOG_FILTER_GENERAL, "AuctionEntry::LoadFromFieldList() - Auction %u has auctioneer (GUID : %u Entry: %u) with wrong faction %u", Id, auctioneer, auctioneerData->id, factionTemplateId);
        return false;
    }

    return true;
}

std::string AuctionEntry::BuildAuctionMailSubject(MailAuctionAnswers response) const
{
    std::ostringstream strm;
    strm << itemEntry << ":0:" << response << ':' << Id << ':' << itemCount;
    strm << ":0" << ":0" << ":0" << ":0" << ":0" << ":0" << ":0";
    return strm.str();
}

std::string AuctionEntry::BuildAuctionMailBody(uint32 p_LowGUID, uint64 p_BID, uint64 p_Buyout, uint64 p_Deposit, uint64 p_Cut)
{
    std::ostringstream l_Body;

    l_Body << "Player-" << g_RealmID << '-' << std::hex << p_LowGUID;
    l_Body << std::dec << ':' << p_BID << ':' << p_Buyout;
    l_Body << ':' << p_Deposit << ':' << p_Cut << ":0";

    return l_Body.str();
}

AuctionListItems::AuctionListItems(WorldPacket& p_Packet)
{
    p_Packet >> Offset;
    p_Packet.readPackGUID(Auctioneer);
    p_Packet >> MinLevel;
    p_Packet >> MaxLevel;
    p_Packet >> Quality;
    p_Packet >> SortCount;

    KnownPets.resize(p_Packet.read<uint32>());

    p_Packet >> MaxPetLevel;

    for (std::size_t l_I = 0; l_I < KnownPets.size(); ++l_I)
        p_Packet >> KnownPets[l_I];

    Name = p_Packet.ReadString(p_Packet.ReadBits(8));

    ClassFilters.resize(p_Packet.ReadBits(3));

    OnlyUsable = p_Packet.ReadBit();
    ExactMatch = p_Packet.ReadBit();

    for (ClassFilter& l_ClassFilter : ClassFilters)
    {
        p_Packet >> l_ClassFilter.ItemClass;

        l_ClassFilter.SubClassFilters.resize(p_Packet.ReadBits(5));

        for (ClassFilter::SubClassFilter& l_SubClassFilter : l_ClassFilter.SubClassFilters)
        {
            p_Packet >> l_SubClassFilter.ItemSubclass;
            p_Packet >> l_SubClassFilter.InvTypeMask;
        }
    }

    p_Packet.read_skip<uint32>(); ///< DataSize = (SortCount * 2)

    for (int32 l_I = 0; l_I < SortCount; ++l_I)
    {
        Sort l_Sort;

        p_Packet >> l_Sort.Type;
        p_Packet >> l_Sort.Direction;

        DataSort.push_back(l_Sort);
    }
}
#endif
