////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "DB2Structure.h"
#pragma once
#include "Common.h"
#include <memory>

#define LFG_LIST_APPLY_FOR_GROUP_TIMEOUT 300
#define LFG_LIST_INVITE_TO_GROUP_TIMEOUT 60
#define LFG_LIST_GROUP_TIMEOUT  1800
#define LFG_LIST_MAX_APPLICATIONS 5

class Player;
class Group;
using GroupPtr = std::shared_ptr<Group>;
struct LFGListEntry;
using LFGListEntryPtr = std::shared_ptr<LFGListEntry>;
class LFGListMgr;

struct LFGListEntry : public std::enable_shared_from_this<LFGListEntry>
{
    struct LFGListApplicationEntry
    {
        /// Last update: 7.0.3 22522
        enum LFGListApplicationStatus
        {
            LFG_LIST_APPLICATION_STATUS_NONE            = 0,
            LFG_LIST_APPLICATION_STATUS_APPLIED         = 1,
            LFG_LIST_APPLICATION_STATUS_INVITED         = 2,
            LFG_LIST_APPLICATION_STATUS_CANCELLED       = 3,
            LFG_LIST_APPLICATION_STATUS_FAILED          = 4,
            LFG_LIST_APPLICATION_STATUS_DECLINED        = 5,
            LFG_LIST_APPLICATION_STATUS_TIMEOUT         = 6,
            LFG_LIST_APPLICATION_STATUS_INVITE_DECLINED = 7,
            LFG_LIST_APPLICATION_STATUS_INVITE_ACCEPTED = 8
        };

        LFGListApplicationEntry(uint32 p_PlayerGuid, LFGListEntryPtr p_Owner);

        uint32 m_ID;
        uint8 m_RoleMask;
        uint32 m_ApplicationTime;
        std::string m_Comment;
        uint32 m_Timeout;
        uint32 m_PlayerLowGuid;
        LFGListApplicationStatus m_Status;
        bool m_Listed;
        LFGListEntryPtr m_Owner;
        uint32 m_Error;

        Player* GetPlayer() const;

        uint32 GetRemainingTimeoutTime() const;
        uint32 GetTimeoutTime() const;
        void ResetTimeout();

        bool Update(uint32 const p_Diff);
    };

    using LFGListApplicationEntryPtr = std::shared_ptr<LFGListApplicationEntry>;

    LFGListEntry()
    {
        m_CreationTime = uint32(time(nullptr));
        m_Timeout = m_CreationTime + LFG_LIST_GROUP_TIMEOUT;
    }

    float m_RequiredItemLevel;
    bool m_AutoAcceptInvites;
    uint32 m_Timeout;
    uint32 m_CreationTime;

    ACE_Based::LockedMap<uint32, LFGListApplicationEntryPtr> m_Applications;
    GroupFinderActivityEntry const* m_ActivityEntry;
    std::string m_Name;
    std::string m_Comment;
    std::string m_VoiceChat;
    GroupPtr m_Group;

    bool IsApplied(uint32 p_GuidLow) const;
    bool IsApplied(Player* p_Player) const;

    void BroadcastApplicantUpdate(LFGListApplicationEntryPtr const l_Applicant);
    void InviteApplicant(LFGListApplicationEntryPtr const l_Applicant);

    void BroadcastPacketToGrop(WorldPacket* p_Data);
    void BroadcastPacketToApplicants(WorldPacket* p_Data);

    LFGListApplicationEntryPtr GetApplicant(uint32 p_ID);
    LFGListApplicationEntryPtr GetApplicantByGUIDLow(uint32 p_ID);

    bool Update(uint32 const p_Diff);
    void ResetTimeout();

    uint32 GetID() const;
};

class LFGListMgr
{
public:

    enum LFGListActivityCategory
    {
        LFG_LIST_ACTIVITY_CATEGORY_ZONE                 = 1,
        LFG_LIST_ACTIVITY_CATEGORY_DUNGEON              = 2,
        LFG_LIST_ACTIVITY_CATEGORY_RAID                 = 3,
        LFG_LIST_ACTIVITY_CATEGORY_ARENA                = 4,
        LFG_LIST_ACTIVITY_CATEGORY_SCENARIO             = 5,
        LFG_LIST_ACTIVITY_CATEGORY_CUSTOM               = 6,
        LFG_LIST_ACTIVITY_CATEGORY_ARENA_SKIRMISH       = 7,
        LFG_LIST_ACTIVITY_CATEGORY_BATTLEGROUNDS        = 8,
        LFG_LIST_ACTIVITY_CATEGORY_RATED_BATTLEGROUNDS  = 9,
        LFG_LIST_ACTIVITY_CATEGORY_OUTDOOR_PVP          = 10
    };

    enum LFGListStatus
    {
        LFG_LIST_STATUS_ERROR_NONE                                        = 59, ///< Used by Blizzard like this ...
        //LFG_LIST_STATUS_ERR_LFG_LIST_MEMBERS_NOT_PRESENT                  = 16,
        LFG_LIST_STATUS_ERR_LFG_LIST_GROUP_FULL                           = 30,
        LFG_LIST_STATUS_ERR_LFG_LIST_NO_LFG_LIST_OBJECT                   = 32,
        LFG_LIST_STATUS_ERR_LFG_LIST_NO_SLOTS_PLAYER                      = 33,
        LFG_LIST_STATUS_ERR_LFG_LIST_MISMATCHED_SLOTS                     = 34,
        LFG_LIST_STATUS_ERR_LFG_LIST_MISMATCHED_SLOTS_LOCAL_XREALM        = 54,
        LFG_LIST_STATUS_ERR_LFG_LIST_PARTY_PLAYERS_FROM_DIFFERENT_REALMS  = 35,
        LFG_LIST_STATUS_ERR_LFG_LIST_MEMBERS_NOT_PRESENT                  = 36,
        LFG_LIST_STATUS_ERR_LFG_LIST_GET_INFO_TIMEOUT                     = 37,
        LFG_LIST_STATUS_ERR_LFG_LIST_INVALID_SLOT                         = 38,
        LFG_LIST_STATUS_ERR_LFG_LIST_DESERTER_PLAYER                      = 39,
        LFG_LIST_STATUS_ERR_LFG_LIST_DESERTER_PARTY                       = 40,
        LFG_LIST_STATUS_ERR_LFG_LIST_RANDOM_COOLDOWN_PLAYER               = 41,
        LFG_LIST_STATUS_ERR_LFG_LIST_RANDOM_COOLDOWN_PARTY                = 42,
        LFG_LIST_STATUS_ERR_LFG_LIST_TOO_MANY_MEMBERS                     = 43,
        LFG_LIST_STATUS_ERR_LFG_LIST_CANT_USE_DUNGEONS                    = 44,
        LFG_LIST_STATUS_ERR_LFG_LIST_ROLE_CHECK_FAILED                    = 45,
        LFG_LIST_STATUS_ERR_LFG_LIST_TOO_FEW_MEMBERS                      = 51,
        LFG_LIST_STATUS_ERR_LFG_LIST_REASON_TOO_MANY_LFG_LIST             = 52,
        LFG_LIST_STATUS_ERR_LFG_LIST_LIST_ENTRY_EXPIRED_TIMEOUT           = 58,
        LFG_LIST_STATUS_ERR_ALREADY_USING_LFG_LIST_LIST                   = 62,
        LFG_LIST_STATUS_ERR_USER_SQUELCHED                                = 65,
    };

    LFGListMgr();

    bool Insert(LFGListEntryPtr p_LFGEntry, Player* p_Requester);
    bool CanInsert(LFGListEntryPtr const p_LFGEntry, Player* p_Requester, bool p_SendError = false) const;
    bool IsEligibleForQueue(Player* p_Player) const;
    bool IsGroupQueued(GroupPtr const p_Group) const;
    void SendLFGListStatusUpdate(LFGListEntryPtr p_LFGEntry, WorldSession* p_WorldSession = nullptr, bool p_Listed = true);
    bool Remove(uint32 l_GroupGuidLow, Player* p_Requester = nullptr, bool p_Disband = true);
    void PlayerAddedToGroup(Player* p_Player, GroupPtr p_Group);
    void PlayerRemoveFromGroup(Player* p_Player, GroupPtr p_Group);
    std::list<LFGListEntryPtr> GetFilteredList(uint32 p_ActivityCategory, uint32 p_ActivitySubCategory, std::string p_FilterString, Player* p_Player);
    LFGListEntryPtr const GetEntrybyGuidLow(uint32 p_ID);
    void OnPlayerApplyForGroup(LFGListEntry::LFGListApplicationEntryPtr p_Application, uint32 p_GroupID);
    void ChangeApplicantStatus(LFGListEntry::LFGListApplicationEntryPtr p_Application, LFGListEntry::LFGListApplicationEntry::LFGListApplicationStatus p_Status, bool p_Notify = true);
    LFGListEntry::LFGListApplicationEntryPtr GetApplicationByID(uint32 p_ID);
    void Update(uint32 const p_Diff);
    void RemovePlayerDueToLogout(uint32 p_LowGUID);
    void OnPlayerLogin(Player* l_Player);
    LFGListStatus CanQueueFor(LFGListEntryPtr p_Entry, Player* p_RequestingPlayer, bool l_Apply = true);
    uint32 GenerateNewIDForApplicant();
    void RemoveAllApplicationsByPlayer(uint32 l_PlayerGUID, bool p_Notify = false);
    bool IsActivityPvP(GroupFinderActivityEntry const* p_Activity) const;
    float GetPlayerItemLevelForActivity(GroupFinderActivityEntry const* p_Activity, Player* p_Player) const;
    uint8 GetApplicationCountByPlayer(uint32 p_GUIDLow);
    float GetLowestItemLevelInGroup(LFGListEntryPtr p_Entry) const;
    uint8 GetMemeberCountInGroupIncludingInvite(LFGListEntryPtr p_Entry);
    uint8 CountEntryApplicationsWithStatus(LFGListEntryPtr p_Entry, LFGListEntry::LFGListApplicationEntry::LFGListApplicationStatus p_Status);
    void AutoInviteApplicantsIfPossible(LFGListEntryPtr p_Entry);

private:
    ACE_Based::LockedMap<uint32, LFGListEntryPtr> m_LFGListQueue;
    uint32 m_ApplicantIDCounter;
};

#define sLFGListMgr ACE_Singleton<LFGListMgr, ACE_Null_Mutex>::instance()
