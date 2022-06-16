////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _RECRUITAFRIENDMGR_H
#define _RECRUITAFRIENDMGR_H

#include "Common.h"
#include "LockedQueue.h"
#include "ObjectMgr.h"

#define RAF_DISTANCE_UPDATE_DELAY   5
#define RAF_MAX_ACTIVE_DISTANCE     100.0f
#define RAF_MAIL_CACHE_REFRESH_DELAY 1 * HOUR * IN_MILLISECONDS

enum eRecruitAFriendConstants
{
    RateXP,
    RateNethershards,
    RateHonor,
    BonusLootChance,

    AKBonus = 1,
    TrackerSpell = 13
};

enum eRecruitAFriendMgrEvents
{
    EventNone,
    EventStartRequest,              ///< void StartRequest(uint64 p_PlayerGUID, std::string p_TargetMail);
    EventAppendNote,                ///< void AppendNote(uint64 p_PlayerGUID, std::string p_Note);
    EventEndRequest,                ///< void EndRequest(uint64 p_PlayerGUID);
    EventSelectCharacter,           ///< void SelectCharacter(uint64 p_PlayerGUID);
    EventPlayerLogin,               ///< void PlayerLogin(uint64 p_PlayerGUID);
    EventLoadAccount,               ///< void LoadAccount(uint32 p_AccountID, RecruitAFriendData* p_Datas);
    EventPlayerLogout,              ///< void PlayerLogout(uint64 p_PlayerGUID);
    EventSetInGroup,                ///< void SetInGroup(uint64 p_GUID1, uint64 p_GUID2, bool p_Apply);
    EventLoadBonuses,               ///< void LoadBonuses(uint64 p_PlayerGUID, RecruitAFriendPlayer::RAFBonuses* p_Bonuses)
    EventLootLegendary,             ///< void LootLegendary(uint64 p_PlayerGUID)
    EventSelectReward,              ///< void SelectReward(uint64 p_PlayerGUID, uint32 p_Reward)
    EventPlayerLevelMax,            ///< void PlayerLevelMax(uint64 p_PlayerGUID)
    EventAddRewardAvailable,        ///< void AddRewardAvailable(uint64 p_RefererGUID, uint64 p_RecruitGUID);
    EventSetNotificationsEnabled,   ///< void SetNotificationsEnabled(uint64 p_PlayerGUID, bool p_Apply)
    EventSummonRAFFriend,           ///< void SummonRAFFriend(uint64 p_SummonerGUID, uint64 p_TargetGUID)

    EventIREvent,
    EventMax
};

enum eRAFSpells
{
    RAFCustomAura   = 13,
    RAFSummoning    = 48955
};

enum eRewardFlags
{
    RafRewardUnique     = 0x1,
    RafRewardHorde      = 0x2,
    RafRewardAlliance   = 0x4
};

enum eParameters
{
    ParamGUID = 1,
    ParamGUID2,
    ParamString,
    ParamUInt,
    ParamPtr,
    ParamApply,
};

namespace RAF
{
    static std::map<uint32, std::vector<eParameters>> k_RafEventsStructs =
    {
        { eRecruitAFriendMgrEvents::EventNone,                      {} },
        { eRecruitAFriendMgrEvents::EventStartRequest,              { eParameters::ParamGUID,  eParameters::ParamString } },
        { eRecruitAFriendMgrEvents::EventAppendNote,                { eParameters::ParamGUID,  eParameters::ParamString } },
        { eRecruitAFriendMgrEvents::EventEndRequest,                { eParameters::ParamGUID } },
        { eRecruitAFriendMgrEvents::EventSelectCharacter,           { eParameters::ParamGUID } },
        { eRecruitAFriendMgrEvents::EventPlayerLogin,               { eParameters::ParamGUID } },
        { eRecruitAFriendMgrEvents::EventLoadAccount,               { eParameters::ParamUInt,  eParameters::ParamPtr } },
        { eRecruitAFriendMgrEvents::EventPlayerLogout,              { eParameters::ParamGUID,  eParameters::ParamString } },
        { eRecruitAFriendMgrEvents::EventSetInGroup,                { eParameters::ParamGUID,  eParameters::ParamGUID2,   eParameters::ParamApply } },
        { eRecruitAFriendMgrEvents::EventLoadBonuses,               { eParameters::ParamGUID,  eParameters::ParamPtr } },
        { eRecruitAFriendMgrEvents::EventLootLegendary,             { eParameters::ParamGUID } },
        { eRecruitAFriendMgrEvents::EventSelectReward,              { eParameters::ParamGUID,  eParameters::ParamUInt } },
        { eRecruitAFriendMgrEvents::EventPlayerLevelMax,            { eParameters::ParamGUID } },
        { eRecruitAFriendMgrEvents::EventAddRewardAvailable,        { eParameters::ParamGUID,  eParameters::ParamGUID2 } },
        { eRecruitAFriendMgrEvents::EventSetNotificationsEnabled,   { eParameters::ParamGUID,  eParameters::ParamApply } },
        { eRecruitAFriendMgrEvents::EventSummonRAFFriend,           { eParameters::ParamGUID,  eParameters::ParamGUID2 } },

        { eRecruitAFriendMgrEvents::EventIREvent,                   {} }
    };

    static std::map<uint32, bool> k_RafEventsInterRealm =
    {
        { eRecruitAFriendMgrEvents::EventNone,                      false },
        { eRecruitAFriendMgrEvents::EventStartRequest,              false },
        { eRecruitAFriendMgrEvents::EventAppendNote,                false },
        { eRecruitAFriendMgrEvents::EventEndRequest,                false },
        { eRecruitAFriendMgrEvents::EventSelectCharacter,           false },
        { eRecruitAFriendMgrEvents::EventPlayerLogin,               false },
        { eRecruitAFriendMgrEvents::EventLoadAccount,               false },
        { eRecruitAFriendMgrEvents::EventPlayerLogout,              false },
        { eRecruitAFriendMgrEvents::EventSetInGroup,                true },
        { eRecruitAFriendMgrEvents::EventLoadBonuses,               false },
        { eRecruitAFriendMgrEvents::EventLootLegendary,             true },
        { eRecruitAFriendMgrEvents::EventSelectReward,              true },
        { eRecruitAFriendMgrEvents::EventPlayerLevelMax,            false },
        { eRecruitAFriendMgrEvents::EventAddRewardAvailable,        true },
        { eRecruitAFriendMgrEvents::EventSetNotificationsEnabled,   true },
        { eRecruitAFriendMgrEvents::EventSummonRAFFriend,           false },

        { eRecruitAFriendMgrEvents::EventIREvent,                   false }
    };
}

class RecruitAFriendMgr
{
    public:
        struct RecruitAFriendData
        {
            RecruitAFriendData();
            uint32 RefererAccountID;
            uint32 RecruitAccountID;
            uint8  Faction;

            uint64 RecruitCharacterGUID;
            uint32 StartTime;

            uint64 RefererCharacterGUID;

            bool InRange;
            bool InSameGroup;

            bool RefererLoggedIn;
            bool RecruitLoggedIn;

            bool Active;
        };

    private:
        std::map<uint32, float> k_FloatConstants =
        {
            { eRecruitAFriendConstants::RateXP, 2.0f },
            { eRecruitAFriendConstants::RateNethershards, 1.3f },
            { eRecruitAFriendConstants::RateHonor, 1.3f },
            { eRecruitAFriendConstants::BonusLootChance, 10.0f }
        };

        struct RecruitAFriendRequest
        {
            RecruitAFriendRequest(std::string p_Mail);
            RecruitAFriendRequest();

            std::string Mail;
            std::string Note;
        };

        struct RecruitAFriendMgrEvent
        {
            RecruitAFriendMgrEvent(uint32 p_EventType);
            uint32 EventType;

            bool Apply;
            uint64 GUID;
            uint64 GUID2;
            std::string String;
            uint32 UInt;
            void* Ptr;
        };

        struct RecruitAFriendPlayer
        {
            struct RAFBonuses
            {
                RAFBonuses();

                bool Initialized;
                bool Legendary;
                uint32 RewardMask;
                uint32 RewardAvailable;
                bool NotificationsEnabled;
            };

            RecruitAFriendPlayer(uint64 p_Guid);
            RecruitAFriendPlayer();

            uint64           Guid;
            RAFBonuses       Bonuses;

            uint64           Referer;
            std::set<uint64> Recruits;

            void SaveBonuses(Player* p_Player);

            void SetWithRecruit(uint64 p_RecruitGUID, bool p_Apply);
            void SetWithReferer(uint64 p_RefererGUID, bool p_Apply);
        };

        struct RecruitAFriendReward
        {
            RecruitAFriendReward() {}

            uint8 Index;
            std::map<LocaleConstant, std::string> LocalizedName;
            std::map<LocaleConstant, std::string> LocalizedSubtitle;
            uint32 ModelDisplayID;

            uint32 Flags;
            uint32 ItemID;
            uint32 Quantity;

            bool Unique;
        };

        struct RecruitAFriendEventPacket
        {
            RecruitAFriendEventPacket();

            uint32 EventCount;
            ByteBuffer Events;
        };

        struct RAFMailList
        {
            RAFMailList(std::set<std::string> const& p_MailList);
            RAFMailList();

            std::set<std::string>::const_iterator find(std::string p_String);
            std::set<std::string>::const_iterator end();
            void insert(std::string p_String);
            std::set<std::string> const& operator=(std::set<std::string> const& p_Rhs);

            std::string CalculateSHA1Hash(std::string const& p_Content);

            std::set<std::string> Mails;
        };

    public:
        RecruitAFriendMgr() {}

    public:
        void Initialize();
        void UpdateRequestMails(std::set<std::string> const& p_NewMailList);
        void AddEvent(uint32 p_EventType, ...);
        void AddEvent(ByteBuffer& p_Event);
        void Update(uint32 const p_Diff);

        float GetFloatConstant(eRecruitAFriendConstants p_Constant);

        std::vector<std::string> GetRewardDataForPlayer(Player* p_Player);

        bool AreRAFFriends(uint64 p_GUID1, uint64 p_GUID2);

    private:
        void StartRequest(uint64 p_PlayerGUID, std::string p_TargetMail);
        void AppendNote(uint64 p_PlayerGUID, std::string p_Note);
        void EndRequest(uint64 p_PlayerGUID);

        /// Activates RAF bonuses for this unique character on recruit account
        void SelectCharacter(uint64 p_PlayerGUID);
        void PlayerLogin(uint64 p_GUID);

        void LoadAccount(uint32 p_AccountID, RecruitAFriendData* p_Datas);
        void LoadAccountData(uint32 p_AccountID, RecruitAFriendData* p_Datas);
        void LoadBonuses(uint64 p_PlayerGUID, RecruitAFriendPlayer::RAFBonuses* p_Bonuses);
        void PlayerLogout(uint64 p_GUID, std::string p_PlayerName);

        void LootLegendary(uint64 p_PlayerGUID);
        void SelectReward(uint64 p_PlayerGUID, uint32 p_Reward);

        void InitGroup(RecruitAFriendData* p_Datas, Player* p_Player);
        void SetInGroup(uint64 p_GUID1, uint64 p_GUID2, bool p_Apply);

        void UpdateDistances(uint64 p_PlayerGUID);
        void UpdateRAFState(RecruitAFriendData* p_Data);

        void PlayerLevelMax(uint64 p_PlayerGUID);
        void AddRewardAvailable(uint64 p_RefererGUID, uint64 p_RecruitGUID);

        void SetNotificationsEnabled(uint64 p_PlayerGUID, bool p_Apply);
        void SummonRAFFriend(uint64 p_SummonerGUID, uint64 p_TargetGUID);

        bool CanHaveReward(uint64 p_PlayerGUID, RecruitAFriendReward const& p_Reward);
    protected:
        void DeleteReferencesAndData(RecruitAFriendData* p_Data);
        std::string PrepareRafRewardFunction(RecruitAFriendReward const& p_Reward, uint64 p_PlayerGUID, LocaleConstant p_Locale);

        std::map<uint64, RecruitAFriendRequest> m_Requests;
        RAFMailList m_Mails;

        std::map<uint32, std::set<RecruitAFriendData*>> m_RefererXData;
        std::map<uint64, std::set<RecruitAFriendData*>> m_RefererCharXData;
        std::map<uint32, RecruitAFriendData*> m_RecruitXData;
        ACE_Based::LockedMap<uint64, RecruitAFriendData*> m_RecruitCharXData;

        ACE_Based::LockedMap<uint64, RecruitAFriendPlayer> m_Players;

        std::queue<std::pair<uint32, uint64>> m_Updates;
        ACE_Based::LockedQueue<RecruitAFriendMgrEvent, ACE_Thread_Mutex> m_Events;

        uint64 m_RefreshMailCacheTimer;
        std::set<std::string> m_TempMailList;

#ifndef CROSS
        RecruitAFriendEventPacket m_EventPacket;
#else
        std::map<uint32, RecruitAFriendEventPacket> m_EventPackets;
#endif

        std::mutex m_EventPacketsLock;

        std::map<uint8, RecruitAFriendReward> m_Rewards;

        std::set<RecruitAFriendData*> m_RAFDataDeleted;
        std::set<uint64> m_RAFDataCreated;
};

#define sRecruitAFriendMgr ACE_Singleton<RecruitAFriendMgr, ACE_Null_Mutex>::instance()

#endif // _RECRUITAFRIENDMGR_H
