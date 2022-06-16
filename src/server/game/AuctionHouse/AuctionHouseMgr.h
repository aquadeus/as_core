////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CROSS
#ifndef _AUCTION_HOUSE_MGR_H
#define _AUCTION_HOUSE_MGR_H

#include "Common.h"
#include "DatabaseEnv.h"

class Item;
class Player;
class WorldPacket;
class AuctionListItems;

struct AuctionSearchFilters;

#define MIN_AUCTION_TIME (12*HOUR)

enum AuctionError
{
    ERR_AUCTION_OK                  = 0,
    ERR_AUCTION_INVENTORY           = 1,
    ERR_AUCTION_DATABASE_ERROR      = 2,
    ERR_AUCTION_NOT_ENOUGHT_MONEY   = 3,
    ERR_AUCTION_ITEM_NOT_FOUND      = 4,
    ERR_AUCTION_HIGHER_BID          = 5,
    ERR_AUCTION_BID_INCREMENT       = 7,
    ERR_AUCTION_BID_OWN             = 10,
    ERR_AUCTION_RESTRICTED_ACCOUNT  = 13
};

enum AuctionAction
{
    AUCTION_SELL_ITEM   = 0,
    AUCTION_CANCEL      = 1,
    AUCTION_PLACE_BID   = 2
};

enum MailAuctionAnswers
{
    AUCTION_OUTBIDDED           = 0,
    AUCTION_WON                 = 1,
    AUCTION_SUCCESSFUL          = 2,
    AUCTION_EXPIRED             = 3,
    AUCTION_CANCELLED_TO_BIDDER = 4,
    AUCTION_CANCELED            = 5,
    AUCTION_SALE_PENDING        = 6
};

struct AuctionDataRequestItems
{
    uint64 PlayerGUID;
    uint32 FactionId;
};

struct AuctionEntry
{
    uint32 Id;
    uint32 auctioneer;                                      // creature low guid
    uint32 itemGUIDLow;
    uint32 itemEntry;
    uint32 itemCount;
    uint32 owner;
    uint64 startbid;                                        //maybe useless
    uint64 bid;
    uint64 buyout;
    time_t expire_time;
    uint32 bidder;
    uint64 deposit;                                         //deposit can be calculated only when creating auction
    AuctionHouseEntry const* auctionHouseEntry;             // in AuctionHouse.db2
    uint32 factionTemplateId;

    // helpers
    uint32 GetHouseId() const { return auctionHouseEntry->ID; }
    uint32 GetHouseFaction() const { return auctionHouseEntry->FactionID; }
    uint64 GetAuctionCut() const;
    uint64 GetAuctionOutBid() const;
    bool BuildAuctionInfo(WorldPacket& p_Data, bool p_CensorServerSideInfo) const;
    void DeleteFromDB(SQLTransaction& trans) const;
    void SaveToDB(SQLTransaction& trans) const;
    bool LoadFromDB(Field* fields);
    bool LoadFromFieldList(Field* fields);
    std::string BuildAuctionMailSubject(MailAuctionAnswers response) const;
    static std::string BuildAuctionMailBody(uint32 p_LowGUID, uint64 p_BID, uint64 p_Buyout, uint64 p_Deposit, uint64 p_Cut);
};

using AuctionEntryPtr = std::shared_ptr<AuctionEntry>;

struct AuctionHouseHistoryEntry
{
    uint32              ItemEntry;
    std::vector<uint32> Bonuses;
    std::vector<uint64> Prices;
};

static std::vector<uint32> g_AuctionHouseBotCharacters;

//this class is used as auctionhouse instance
class AuctionHouseObject
{
  public:
    // Initialize storage
    AuctionHouseObject() { next = AuctionsMap.begin(); }
    ~AuctionHouseObject()
    {
        AuctionsMap.clear();
    }

    typedef ACE_Based::LockedMap<uint32, AuctionEntryPtr> AuctionEntryMap;

    uint32 Getcount() const { return AuctionsMap.size(); }

    AuctionEntryMap::iterator GetAuctionsBegin() {return AuctionsMap.begin();}
    AuctionEntryMap::iterator GetAuctionsEnd() {return AuctionsMap.end();}

    AuctionEntryPtr GetAuction(uint32 id) const
    {
        AuctionEntryMap::const_iterator itr = AuctionsMap.find(id);
        return itr != AuctionsMap.end() ? itr->second : NULL;
    }

    void AddAuction(AuctionEntryPtr auction);

    bool RemoveAuction(AuctionEntryPtr auction, uint32 itemEntry);

    void Update();
    void UpdateAHBot();

    void BuildListBidderItems(WorldPacket& data, Player* player, uint32& count, uint32& totalcount);
    void BuildListOwnerItems(WorldPacket& data, Player* player, uint32& count, uint32& totalcount);
    void BuildListAuctionItems(WorldPacket& p_Data, Player* p_Player, std::wstring const& p_SearchedName, AuctionListItems p_ListItems, Optional<AuctionSearchFilters> const& p_Filters);

  private:
    AuctionEntryMap AuctionsMap;

    // storage for "next" auction item for next Update()
    AuctionEntryMap::const_iterator next;
};

class AuctionHouseMgr
{
    friend class ACE_Singleton<AuctionHouseMgr, ACE_Null_Mutex>;

    private:
        AuctionHouseMgr();
        ~AuctionHouseMgr();

    public:

        typedef ACE_Based::LockedMap<uint32, Item*> ItemMap;

        AuctionHouseObject* GetAuctionsMap(uint32 factionTemplateId);

        Item* GetAItem(uint32 id)
        {
            ItemMap::const_iterator itr = mAitems.find(id);
            if (itr != mAitems.end())
                return itr->second;

            return NULL;
        }

        //auction messages
        void SendAuctionWonMail(AuctionEntryPtr auction, SQLTransaction& trans);
        void SendAuctionSuccessfulMail(AuctionEntryPtr auction, SQLTransaction& trans);
        void SendAuctionExpiredMail(AuctionEntryPtr auction, SQLTransaction& trans);
        void SendAuctionOutbiddedMail(AuctionEntryPtr auction, uint64 newPrice, Player* newBidder, SQLTransaction& trans);
        void SendAuctionCancelledToBidderMail(AuctionEntryPtr auction, SQLTransaction& trans, Item* item);

        bool SellHistorysContainsItemTypeWithItemLevel(uint32 p_ItemTypeKey, uint32 p_Ilevel)
        {
            if (m_SellLogs[p_ItemTypeKey][p_Ilevel].empty())
                return false;
            return true;
        }

        AuctionHouseHistoryEntry* GetRandomItemTypeWithItemLevelInSellHistory(uint32 p_ItemTypeKey, uint32 p_Ilevel)
        {
            std::vector<AuctionHouseHistoryEntry*>& l_Histories = m_SellLogs[p_ItemTypeKey][p_Ilevel];
            if (l_Histories.empty())
                return nullptr;

            return l_Histories[rand() % l_Histories.size()];
        }

        static uint64 GetAuctionDeposit(uint32 p_Time, Item* p_Item, uint32 p_Count);
        static AuctionHouseEntry const* GetAuctionHouseEntry(uint32 factionTemplateId);

    public:

        // Used primarily at server start to avoid loading a list of expired auctions
        void DeleteExpiredAuctionsAtStartup();

        //load first auction items, because of check if item exists, when loading
        void LoadAuctionItems();
        void LoadAuctions();

        void AddAItem(Item* it);
        bool RemoveAItem(uint32 id);

        void Update();

    private:

        AuctionHouseObject mHordeAuctions;
        AuctionHouseObject mAllianceAuctions;
        AuctionHouseObject mNeutralAuctions;

        std::unordered_map<uint32/*Class/Subclass/Inventory*/, std::unordered_map<uint32/*ItemIlevel*/, std::vector<AuctionHouseHistoryEntry*>>> m_SellLogs;

        ItemMap mAitems;
};

struct AuctionSearchFilters
{
    enum FilterType : uint32
    {
        FilterSkipClass     = 0,
        FilterSkipSubclass  = 0xFFFFFFFF,
        FilterSkipInvType   = 0xFFFFFFFF
    };

    struct SubclassFilter
    {
        uint32 SubclassMask = FilterType::FilterSkipClass;
        std::array<uint32, MAX_ITEM_SUBCLASS_WEAPON> InvTypes;
    };

    std::array<SubclassFilter, MAX_ITEM_CLASS> Classes;
};

class AuctionListItems
{
    public:
        AuctionListItems(WorldPacket& p_Packet);

        struct Sort
        {
            uint8 Type      = 0;
            uint8 Direction = 0;
        };

        struct ClassFilter
        {
            struct SubClassFilter
            {
                int32 ItemSubclass;
                uint32 InvTypeMask;
            };

            int32 ItemClass;
            std::vector<SubClassFilter> SubClassFilters;
        };

        uint64 Auctioneer;
        uint32 Offset   = 0;
        uint8 MinLevel  = 1;
        uint8 MaxLevel  = 100;
        int32 Quality   = 0;
        uint8 SortCount = 0;
        std::vector<uint8> KnownPets;
        int8 MaxPetLevel = 0;
        std::string Name;
        std::vector<ClassFilter> ClassFilters;
        bool ExactMatch = true;
        bool OnlyUsable = false;
        std::vector<Sort> DataSort;
};

#define sAuctionMgr ACE_Singleton<AuctionHouseMgr, ACE_Null_Mutex>::instance()

#endif
#endif
