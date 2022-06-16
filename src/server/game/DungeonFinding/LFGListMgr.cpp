////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Object.h"
#include "LFGListMgr.h"
#include "GroupMgr.h"

LFGListMgr::LFGListMgr()
{
    m_ApplicantIDCounter = 0;
}

LFGListEntry::LFGListApplicationEntry::LFGListApplicationEntry(uint32 p_PlayerGuid, LFGListEntryPtr p_Owner)
{
    m_ID = sLFGListMgr->GenerateNewIDForApplicant();
    m_ApplicationTime = time(nullptr);
    m_PlayerLowGuid = p_PlayerGuid;
    m_Timeout = m_ApplicationTime + LFG_LIST_APPLY_FOR_GROUP_TIMEOUT;
    m_Status = LFG_LIST_APPLICATION_STATUS_NONE;
    m_Listed = true;
    m_Owner = p_Owner;
    m_Error = LFGListMgr::LFG_LIST_STATUS_ERROR_NONE;
}

bool LFGListMgr::Insert(LFGListEntryPtr p_LFGEntry, Player* p_Requester)
{
    if (!IsEligibleForQueue(p_Requester))
    {
        p_Requester->GetSession()->SendLfgListJoinResult(p_LFGEntry, LFG_LIST_STATUS_ERR_ALREADY_USING_LFG_LIST_LIST);
        return false;
    }

    GroupPtr l_Group = p_Requester->GetGroup();
    if (l_Group && l_Group->isBGGroup())
        l_Group = p_Requester->GetOriginalGroup();

    /// Create a new group fyyiiii
    if (!l_Group)
    {
        l_Group = std::make_shared<Group>();
        if (!l_Group->AddLeaderInvite(p_Requester))
        {
            p_Requester->GetSession()->SendLfgListJoinResult(p_LFGEntry, LFG_LIST_STATUS_ERR_LFG_LIST_NO_LFG_LIST_OBJECT);
            return false;
        }

        l_Group->Create(p_Requester);
        sGroupMgr->AddGroup(l_Group);
    }

    p_LFGEntry->m_Group = l_Group;
    m_LFGListQueue.insert({l_Group->GetLowGUID(), p_LFGEntry});
    SendLFGListStatusUpdate(p_LFGEntry);
    return true;
}

/// Stupid error messages: Todo: Find more appropriate
bool LFGListMgr::CanInsert(LFGListEntryPtr const p_LFGEntry, Player* p_Requester, bool p_SendError /* = false */) const
{
    if (!p_LFGEntry->m_ActivityEntry)
    {
        if (p_SendError)
            p_Requester->GetSession()->SendLfgListJoinResult(p_LFGEntry, LFG_LIST_STATUS_ERR_LFG_LIST_NO_LFG_LIST_OBJECT);
        return false;
    }

    if (!IsEligibleForQueue(p_Requester) || p_LFGEntry->m_Group)
    {
        if (p_SendError)
            p_Requester->GetSession()->SendLfgListJoinResult(p_LFGEntry, LFG_LIST_STATUS_ERR_ALREADY_USING_LFG_LIST_LIST);
        return false;
    }

    if (GetPlayerItemLevelForActivity(p_LFGEntry->m_ActivityEntry, p_Requester) < p_LFGEntry->m_RequiredItemLevel)
    {
        if (p_SendError)
            p_Requester->GetSession()->SendLfgListJoinResult(p_LFGEntry, LFG_LIST_STATUS_ERR_LFG_LIST_INVALID_SLOT);
        return false;
    }

    GroupPtr l_Group = p_Requester->GetGroup();

    if (l_Group)
    {
        if ((!l_Group->isRaidGroup() || !l_Group->IsAssistant(p_Requester->GetGUID())) && !l_Group->IsLeader(p_Requester->GetGUID()))
        {
            if (p_SendError)
                p_Requester->GetSession()->SendLfgListJoinResult(p_LFGEntry, LFG_LIST_STATUS_ERR_LFG_LIST_NO_LFG_LIST_OBJECT);
            return false;
        }
    }

    return true;
}

bool LFGListMgr::IsEligibleForQueue(Player* p_Requester) const
{
    if (!p_Requester)
        return false;

    GroupPtr l_Group = p_Requester->GetGroup();
    if (l_Group && l_Group->isBGGroup())
        l_Group = p_Requester->GetOriginalGroup();

    return !IsGroupQueued(l_Group);
}

bool LFGListMgr::IsGroupQueued(GroupPtr const p_Group) const
{
    if (!p_Group)
        return false;

    return m_LFGListQueue.find(p_Group->GetLowGUID()) != m_LFGListQueue.end();
}

void LFGListMgr::SendLFGListStatusUpdate(LFGListEntryPtr p_LFGEntry, WorldSession* p_WorldSession /* = nullptr */, bool p_Listed /* = true */)
{
    WorldPacket l_Data;
    WorldSession::BuildLfgListQueueUpdate(&l_Data, p_LFGEntry, p_Listed);

    if (p_WorldSession)
        p_WorldSession->SendPacket(&l_Data);
    else if (GroupPtr l_Group = p_LFGEntry->m_Group) ///< l_Group is unused
        p_LFGEntry->BroadcastPacketToGrop(&l_Data);
}

bool LFGListMgr::Remove(uint32 l_GroupGuidLow, Player* p_Requester /* = nullptr */, bool p_Disband /* = true */)
{
   auto l_Iter = m_LFGListQueue.find(l_GroupGuidLow);
    if (l_Iter == m_LFGListQueue.end())
        return false;

    GroupPtr l_Group = l_Iter->second->m_Group;
    if (!l_Group)
        return false;

    if (p_Requester && ((!l_Group->isRaidGroup() || !l_Group->IsAssistant(p_Requester->GetGUID())) && !l_Group->IsLeader(p_Requester->GetGUID())))
        return false;

    for (auto l_Itr = l_Iter->second->m_Applications.begin(); l_Itr != l_Iter->second->m_Applications.end();)
    {
        sLFGListMgr->ChangeApplicantStatus(l_Itr->second, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_CANCELLED);
        l_Itr = l_Iter->second->m_Applications.begin();
    }

    LFGListEntryPtr l_Entry = l_Iter->second;

    m_LFGListQueue.erase(l_Iter);

    SendLFGListStatusUpdate(l_Entry, nullptr, false);

    if (p_Disband && l_Group->GetMembersCount() < 2)
        l_Group->Disband();

    return true;
}

void LFGListMgr::PlayerAddedToGroup(Player* p_Player, GroupPtr p_Group)
{
    auto l_Iter = m_LFGListQueue.find(p_Group->GetLowGUID());
    if (l_Iter == m_LFGListQueue.end())
        return;

    SendLFGListStatusUpdate(l_Iter->second, p_Player->GetSession(), true);
}

void LFGListMgr::PlayerRemoveFromGroup(Player* p_Player, GroupPtr p_Group)
{
    auto l_Iter = m_LFGListQueue.find(p_Group->GetLowGUID());
    if (l_Iter == m_LFGListQueue.end())
        return;

    SendLFGListStatusUpdate(l_Iter->second, p_Player->GetSession(), false);
}

std::list<LFGListEntryPtr> LFGListMgr::GetFilteredList(uint32 p_ActivityCategory, uint32 /*p_ActivitySubCategory*/, std::string p_FilterString, Player* p_Player)
{
    std::list<LFGListEntryPtr> l_LFGFiltered;

    for (auto& l_Iter : m_LFGListQueue)
    {
        LFGListEntryPtr const l_ListEntry = l_Iter.second;

        if (l_ListEntry->m_ActivityEntry->CategoryID != p_ActivityCategory) ///< Comparison of integers of different signs: 'const int32' (aka 'const int') and 'uint32' (aka 'unsigned int')
            continue;

        if (p_FilterString.length() && l_ListEntry->m_Name.length())
        {
            std::string l_UpperName = l_ListEntry->m_Name;
            std::transform(l_UpperName.begin(), l_UpperName.end(), l_UpperName.begin(), toupper);
            std::transform(p_FilterString.begin(), p_FilterString.end(), p_FilterString.begin(), toupper);

            if (l_UpperName.find(p_FilterString) == std::string::npos)
                continue;
        }

        if (CanQueueFor(l_Iter.second, p_Player, false) != LFG_LIST_STATUS_ERROR_NONE)
            continue;

        l_LFGFiltered.push_back(l_ListEntry);
    }
    return l_LFGFiltered;
}

LFGListEntryPtr const LFGListMgr::GetEntrybyGuidLow(uint32 p_ID)
{
    auto l_Iter = m_LFGListQueue.find(p_ID);
    if (l_Iter == m_LFGListQueue.end())
        return nullptr;

    return l_Iter->second;
}

void LFGListMgr::OnPlayerApplyForGroup(LFGListEntry::LFGListApplicationEntryPtr p_Application, uint32 p_GroupID)
{
    LFGListEntryPtr l_Entry = GetEntrybyGuidLow(p_GroupID);

    if (!l_Entry)
        return;

    Player* l_Player = p_Application->GetPlayer();

    if (!l_Player)
        return;

    if (l_Entry->m_Applications.find(p_Application->m_ID) != l_Entry->m_Applications.end())
        return;

    l_Entry->m_Applications.insert(std::make_pair(p_Application->m_ID, p_Application));

    LFGListEntry::LFGListApplicationEntryPtr l_Application = l_Entry->m_Applications.find(p_Application->m_ID)->second;

    l_Application->m_Error = CanQueueFor(l_Application->m_Owner, l_Player);

    if (l_Application->m_Error != LFGListMgr::LFG_LIST_STATUS_ERROR_NONE)
    {
        ChangeApplicantStatus(l_Application, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_FAILED);
    }
    else
    {
        if (l_Entry->m_AutoAcceptInvites)
        {
            if (!l_Entry->m_Group->isRaidGroup() && GetMemeberCountInGroupIncludingInvite(l_Entry) == 5)
            {
                ChangeApplicantStatus(l_Application, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_APPLIED);
                // Handled clientside -- OnAccept = function(self, applicantID) ConvertToRaid(); C_LFGList.InviteApplicant(applicantID) end,
            }
            else
            ChangeApplicantStatus(l_Application, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_INVITED);
        }
        else
            ChangeApplicantStatus(l_Application, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_APPLIED);
    }
}

void LFGListEntry::BroadcastPacketToGrop(WorldPacket* p_Data)
{
    m_Group->BroadcastPacket(p_Data, false);
}

void LFGListEntry::BroadcastPacketToApplicants(WorldPacket* p_Data)
{
    m_Applications.safe_foreach(true, [&p_Data](ACE_Based::LockedMap<uint32, LFGListApplicationEntryPtr>::iterator& p_Itr)
    {
        if (Player* l_Player = p_Itr->second->GetPlayer())
            l_Player->SendDirectMessage(p_Data);
    });
}

Player* LFGListEntry::LFGListApplicationEntry::GetPlayer() const
{
    return sObjectMgr->GetPlayerByLowGUID(m_PlayerLowGuid);
};

bool LFGListEntry::IsApplied(uint32 p_GuidLow) const
{
    return m_Applications.find(p_GuidLow) != m_Applications.end();
}

bool LFGListEntry::IsApplied(Player* p_Player) const
{
    return IsApplied(p_Player->GetGUIDLow());
}

void LFGListEntry::BroadcastApplicantUpdate(LFGListApplicationEntryPtr const l_Applicant)
{
    WorldPacket l_Data(SMSG_LFG_LIST_APPLICANT_LIST_UPDATE, 0x100);
    WorldSession::BuildLfgListApplicantListUpdate(&l_Data, shared_from_this(), l_Applicant);
    BroadcastPacketToGrop(&l_Data);
}

void LFGListEntry::InviteApplicant(LFGListApplicationEntryPtr const /*l_Applicant*/)
{
}

void LFGListEntry::LFGListApplicationEntry::ResetTimeout()
{
    m_Timeout = time(nullptr) + (m_Status == LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_INVITED ? LFG_LIST_INVITE_TO_GROUP_TIMEOUT : LFG_LIST_APPLY_FOR_GROUP_TIMEOUT);
}

void LFGListEntry::ResetTimeout()
{
    m_Timeout = time(nullptr) + LFG_LIST_GROUP_TIMEOUT;
    sLFGListMgr->SendLFGListStatusUpdate(shared_from_this());
}

uint32 LFGListEntry::LFGListApplicationEntry::GetRemainingTimeoutTime() const
{
    return m_Timeout - time(nullptr);
}

uint32 LFGListEntry::LFGListApplicationEntry::GetTimeoutTime() const
{
    return m_Timeout;
}

void LFGListMgr::ChangeApplicantStatus(LFGListEntry::LFGListApplicationEntryPtr p_Application, LFGListEntry::LFGListApplicationEntry::LFGListApplicationStatus p_Status, bool p_Notify /* = true */)
{
    if (p_Application->m_Status == p_Status)
        return;

    bool l_Remove = false;

    p_Application->m_Status = p_Status;

    switch (p_Status)
    {
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_INVITED:
        {
            if ((!p_Application->m_Owner->m_Group->isRaidGroup() && GetMemeberCountInGroupIncludingInvite(p_Application->m_Owner) >= 5) || CanQueueFor(p_Application->m_Owner, p_Application->GetPlayer()) != LFG_LIST_STATUS_ERROR_NONE)
                break;
        }
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_APPLIED:
        {
            p_Application->ResetTimeout();
            p_Application->m_Owner->ResetTimeout();

            if (p_Notify && p_Application->GetPlayer() != nullptr)
                p_Application->GetPlayer()->GetSession()->SendLfgListApplyForGroupResult(p_Application->m_Owner, p_Application);

            break;
        }
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_INVITE_DECLINED:
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_DECLINED:
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_CANCELLED:
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_TIMEOUT:
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_FAILED:
        {
            p_Application->m_Listed = false;
            l_Remove = true;

            if (p_Notify && p_Application->GetPlayer() != nullptr)
                p_Application->GetPlayer()->GetSession()->SendLfgListApplicantGroupInviteResponse(p_Application);

            break;
        }
        case LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_INVITE_ACCEPTED:
        {
            if ((!p_Application->m_Owner->m_Group->isRaidGroup() && GetMemeberCountInGroupIncludingInvite(p_Application->m_Owner) >= 5) || CanQueueFor(p_Application->m_Owner, p_Application->GetPlayer()) != LFG_LIST_STATUS_ERROR_NONE)
                break;

            p_Application->m_Listed = false;
            l_Remove = true;

            if (Player* l_Player = p_Application->GetPlayer())
            {
                if (!l_Player->GetGroup())
                    p_Application->m_Owner->m_Group->AddMember(p_Application->GetPlayer());
            }

            if (p_Notify && p_Application->GetPlayer() != nullptr)
                p_Application->GetPlayer()->GetSession()->SendLfgListApplicantGroupInviteResponse(p_Application);

            break;
        }
        default:
            break;
    }

    p_Application->m_Owner->BroadcastApplicantUpdate(p_Application);

    AutoInviteApplicantsIfPossible(p_Application->m_Owner);

    if (l_Remove)
        p_Application->m_Owner->m_Applications.erase(p_Application->m_Owner->m_Applications.find(p_Application->m_ID));
}

LFGListEntry::LFGListApplicationEntryPtr LFGListMgr::GetApplicationByID(uint32 p_ID)
{
    LFGListEntry::LFGListApplicationEntryPtr l_Result = nullptr;
    m_LFGListQueue.safe_foreach(false, [&](ACE_Based::LockedMap<uint32, LFGListEntryPtr>::iterator& p_Itr)
    {
        p_Itr->second->m_Applications.safe_foreach(false, [&](ACE_Based::LockedMap<uint32, LFGListEntry::LFGListApplicationEntryPtr>::iterator& p_Itr2)
        {
            if (p_Itr2->first == p_ID)
            {
                l_Result = p_Itr2->second;
                p_Itr2 = p_Itr->second->m_Applications.end();
            }
            else
                p_Itr2++;
        });

        if (l_Result)
            p_Itr = m_LFGListQueue.end();
        else
            p_Itr++;

    });

    return l_Result;
}

void LFGListMgr::RemoveAllApplicationsByPlayer(uint32 l_PlayerGUID, bool p_Notify /* = false */)
{
    while (LFGListEntry::LFGListApplicationEntryPtr l_Application = GetApplicationByID(l_PlayerGUID))
    {
        sLFGListMgr->ChangeApplicantStatus(l_Application, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_CANCELLED, p_Notify);
    }
}

uint8 LFGListMgr::GetApplicationCountByPlayer(uint32 p_GUIDLow)
{
    uint8 l_Counter = 0;

    m_LFGListQueue.safe_foreach(true, [&l_Counter, &p_GUIDLow](ACE_Based::LockedMap<uint32, LFGListEntryPtr>::iterator& p_Itr)
    {
        p_Itr->second->m_Applications.safe_foreach(true, [&l_Counter, &p_GUIDLow](ACE_Based::LockedMap<uint32, LFGListEntry::LFGListApplicationEntryPtr>::iterator& p_Itr)
        {
            if (p_Itr->second->m_PlayerLowGuid == p_GUIDLow)
                ++l_Counter;
        });
    });

    return l_Counter;
}

void LFGListMgr::Update(uint32 const p_Diff)
{
    std::vector<uint32> l_GroupsToDelete;

    m_LFGListQueue.safe_foreach(true, [&](ACE_Based::LockedMap<uint32, LFGListEntryPtr>::iterator& p_Itr)
    {
        if (!p_Itr->second->Update(p_Diff))
            l_GroupsToDelete.push_back((*p_Itr).second->GetID());
    });

    for (uint32 l_GroupGuid : l_GroupsToDelete)
        sLFGListMgr->Remove(l_GroupGuid);
}

uint32 LFGListEntry::GetID() const
{
    return m_Group->GetLowGUID();
}

bool LFGListEntry::Update(uint32 const p_Diff)
{

    std::vector<LFGListEntry::LFGListApplicationEntryPtr> l_ApplicationsTimeout;
    m_Applications.safe_foreach(true, [&](ACE_Based::LockedMap<uint32, LFGListEntry::LFGListApplicationEntryPtr>::iterator& p_Itr)
    {
        if (!p_Itr->second->Update(p_Diff))
            l_ApplicationsTimeout.push_back(p_Itr->second);
    });

    for (auto l_Application : l_ApplicationsTimeout)
        sLFGListMgr->ChangeApplicantStatus(l_Application, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_TIMEOUT);

    return m_Timeout > time(nullptr);
}

bool LFGListEntry::LFGListApplicationEntry::Update(uint32 const /*p_Diff*/)
{
    return m_Timeout > time(nullptr); ///< Bye bye
}

LFGListEntry::LFGListApplicationEntryPtr LFGListEntry::GetApplicant(uint32 p_ID)
{
    auto l_Iter = m_Applications.find(p_ID);
    return l_Iter != m_Applications.end() ? l_Iter->second : nullptr;
}

LFGListEntry::LFGListApplicationEntryPtr LFGListEntry::GetApplicantByGUIDLow(uint32 p_ID)
{
    LFGListEntry::LFGListApplicationEntryPtr l_Result = nullptr;
    m_Applications.safe_foreach(false, [&](ACE_Based::LockedMap<uint32, LFGListEntry::LFGListApplicationEntryPtr>::iterator& p_Itr)
    {
        if (p_Itr->second->m_PlayerLowGuid == p_ID)
        {
            l_Result = p_Itr->second;
            p_Itr = m_Applications.end();
        }

        p_Itr++;
    });

    return l_Result;
}

void LFGListMgr::RemovePlayerDueToLogout(uint32 p_LowGUID) ///< This is wrong, but cba to do it other way for now
{
    RemoveAllApplicationsByPlayer(p_LowGUID);
}

void LFGListMgr::OnPlayerLogin(Player* l_Player)
{
    if (GroupPtr l_Group = l_Player->GetGroup())
    {
        if (LFGListEntryPtr l_Entry = GetEntrybyGuidLow(l_Group->GetLowGUID()))
            SendLFGListStatusUpdate(l_Entry, l_Player->GetSession());
    }
}

LFGListMgr::LFGListStatus LFGListMgr::CanQueueFor(LFGListEntryPtr p_Entry, Player* p_RequestingPlayer, bool p_Apply /* = true */)
{
    if (!p_RequestingPlayer)
        return LFG_LIST_STATUS_ERR_LFG_LIST_ROLE_CHECK_FAILED;

    GroupPtr l_Group = p_Entry->m_Group;
    GroupFinderActivityEntry const* l_Activity = p_Entry->m_ActivityEntry;
    float l_ILvl = sLFGListMgr->GetPlayerItemLevelForActivity(l_Activity, p_RequestingPlayer);

    if (p_RequestingPlayer->GetTeam() != l_Group->GetTeam())
        return LFG_LIST_STATUS_ERR_LFG_LIST_INVALID_SLOT;   ///< Shouldnt be a problem, because its only for filters

    if ((l_Activity->RequiredILvl && l_ILvl < l_Activity->RequiredILvl) || l_ILvl < p_Entry->m_RequiredItemLevel)
        return LFG_LIST_STATUS_ERR_LFG_LIST_INVALID_SLOT;   ///< Same as above, filtered out

    if ((l_Activity->MaxPlayers && (int32)l_Group->GetMembersCount() >= l_Activity->MaxPlayers) || l_Group->GetMembersCount() >= 40)
        return LFG_LIST_STATUS_ERR_LFG_LIST_TOO_MANY_MEMBERS;

    if (p_RequestingPlayer->getLevel() < l_Activity->MinLevel || (l_Activity->MaxLevel && p_RequestingPlayer->getLevel() > l_Activity->MaxLevel))
        return LFG_LIST_STATUS_ERR_LFG_LIST_INVALID_SLOT;   ///< Filtered out

    if (p_Apply)
        return LFG_LIST_STATUS_ERROR_NONE;

    if (sLFGListMgr->GetApplicationCountByPlayer(p_RequestingPlayer->GetGUIDLow()) >= LFG_LIST_MAX_APPLICATIONS)
        return LFG_LIST_STATUS_ERR_LFG_LIST_REASON_TOO_MANY_LFG_LIST;

    return LFG_LIST_STATUS_ERROR_NONE;
}

uint32 LFGListMgr::GenerateNewIDForApplicant()
{
    uint32 l_NewID = ++m_ApplicantIDCounter;

    if (l_NewID == 0xFFFFFFFF)
    {
        sLog->outError(LOG_FILTER_GENERAL, "LFGList applicant ID overflow!! Can't continue, shutting down server.");
        World::StopNow(ERROR_EXIT_CODE);
    }

    return l_NewID;
}

bool LFGListMgr::IsActivityPvP(GroupFinderActivityEntry const* p_Activity) const
{
    if (!p_Activity)
        return false;

    switch (p_Activity->CategoryID)
    {
        case LFG_LIST_ACTIVITY_CATEGORY_ARENA:
        case LFG_LIST_ACTIVITY_CATEGORY_ARENA_SKIRMISH:
        case LFG_LIST_ACTIVITY_CATEGORY_BATTLEGROUNDS:
        case LFG_LIST_ACTIVITY_CATEGORY_RATED_BATTLEGROUNDS:
        case LFG_LIST_ACTIVITY_CATEGORY_OUTDOOR_PVP:
            return true;
    }

    return p_Activity->ID == 17;    ///< Custom PvP
}

float LFGListMgr::GetPlayerItemLevelForActivity(GroupFinderActivityEntry const* p_Activity, Player* p_Player) const
{
    if (p_Player == nullptr)
        return 0.0f;

    return p_Player->GetFloatValue(PLAYER_FIELD_AVG_ITEM_LEVEL + PlayerAvgItemLevelOffsets::EquippedAvgItemLevel);
}

float LFGListMgr::GetLowestItemLevelInGroup(LFGListEntryPtr p_Entry) const
{
    float l_MinIlvl = 100000.0f;

    p_Entry->m_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr)
    {
        if (Player* l_Player = l_Itr->player)
            l_MinIlvl = std::min(l_MinIlvl, GetPlayerItemLevelForActivity(p_Entry->m_ActivityEntry, l_Player));
    });

    return l_MinIlvl != 100000.0f ? l_MinIlvl : 0.0f;
}

uint8 LFGListMgr::GetMemeberCountInGroupIncludingInvite(LFGListEntryPtr p_Entry)
{
    uint8 l_InviteCount = CountEntryApplicationsWithStatus(p_Entry, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_INVITE_DECLINED);
    return l_InviteCount + p_Entry->m_Group->GetMembersCount();
}

uint8 LFGListMgr::CountEntryApplicationsWithStatus(LFGListEntryPtr p_Entry, LFGListEntry::LFGListApplicationEntry::LFGListApplicationStatus p_Status)
{
    uint8 l_Count = 0;
    p_Entry->m_Applications.safe_foreach(true, [&](ACE_Based::LockedMap<uint32, LFGListEntry::LFGListApplicationEntryPtr>::iterator& p_Itr)
    {
        if (p_Itr->second->m_Status == p_Status)
            l_Count++;
    });

    return l_Count;
}

void LFGListMgr::AutoInviteApplicantsIfPossible(LFGListEntryPtr p_Entry)
{
    if (!p_Entry->m_AutoAcceptInvites)
        return;

    if (!p_Entry->m_Group->isRaidGroup() && GetMemeberCountInGroupIncludingInvite(p_Entry) >= 5)
        return;

    p_Entry->m_Applications.safe_foreach(true, [&](ACE_Based::LockedMap<uint32, LFGListEntry::LFGListApplicationEntryPtr>::iterator& p_Itr)
    {
        if (CanQueueFor(p_Entry, p_Itr->second->GetPlayer()) == LFG_LIST_STATUS_ERROR_NONE)
            ChangeApplicantStatus(p_Itr->second, LFGListEntry::LFGListApplicationEntry::LFG_LIST_APPLICATION_STATUS_INVITED);
    });
}
