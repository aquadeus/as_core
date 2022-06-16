////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "WorldSession.h"
#include "WorldPacket.h"
#include "Player.h"
#include "Group.h"
#include "LFGMgr.h"
#include "ObjectMgr.h"
#include "GroupMgr.h"
#include "GameEventMgr.h"
#include "InstanceScript.h"
#include "InstanceSaveMgr.h"

void WorldSession::HandleLfgJoinOpcode(WorldPacket& p_Packet)
{
    LfgDungeonSet l_Slots;

    uint32 l_SlotCount      = 0;
    uint32 l_Roles          = 0;

    uint8 l_PartyIndex  = 0;
    bool l_QueueAsGroup = false;
    bool l_UnkLegion    = false;

    l_QueueAsGroup  = p_Packet.ReadBit(); ///< l_QueueAsGroup is never read 01/18/16
    l_UnkLegion     = p_Packet.ReadBit(); ///< l_UnkLegion is never read 09/29/16

    p_Packet.ResetBitReading();

    p_Packet >> l_PartyIndex;
    p_Packet >> l_Roles;
    p_Packet >> l_SlotCount;

    for (uint32 l_I = 0; l_I < l_SlotCount; ++l_I)
        l_Slots.insert(p_Packet.read<uint32>() & 0xFFFFFF); ///< Remove the type from the dungeon entry

    if (!l_SlotCount)
        return;

    LFGDungeonEntry const* l_Entry = sLFGDungeonStore.LookupEntry(*l_Slots.begin() & 0xFFFFFF);

    uint8 l_Type = LfgType::TYPEID_DUNGEON;
    uint8 l_MaxGroupSize = 5;

    if (l_Entry != nullptr)
        l_Type = l_Entry->type;

    if (l_Type == LfgType::LFG_SUBTYPEID_RAID)
        l_MaxGroupSize = 25;
    if (l_Type == LfgType::LFG_SUBTYPEID_SCENARIO)
        l_MaxGroupSize = 3;

    GroupPtr l_Group = m_Player->GetGroup();

    if (!sWorld->getBoolConfig(WorldBoolConfigs::CONFIG_DUNGEON_FINDER_ENABLE) ||
        (l_Group && l_Group->GetLeaderGUID() != m_Player->GetGUID() &&
        (l_Group->GetMembersCount() == l_MaxGroupSize || !l_Group->isLFGGroup())))
    {
        return;
    }

    sLFGMgr->Join(m_Player, uint8(l_Roles), l_Slots, "");
}

void WorldSession::HandleLfgLeaveOpcode(WorldPacket& /*p_Packet*/)
{
    GroupPtr l_Group = m_Player->GetGroup();

    /// Check cheating - only leader can leave the queue
    if (!l_Group || l_Group->GetLeaderGUID() == m_Player->GetGUID())
        sLFGMgr->Leave(m_Player, l_Group);
}

void WorldSession::HandleLfgProposalResultOpcode(WorldPacket& p_Packet)
{
    uint32 l_ProposalID;                                    ///< Internal lfgGroupID
    bool l_Accepted;                                        ///< Accept to join?

    uint64 l_RequesterGUID  = 0;
    uint64 l_QueueID        = 0;

    uint32 l_TicketID = 0;
    uint32 l_TicketType = 0;
    uint32 l_TicketTime = 0;

    p_Packet.readPackGUID(l_RequesterGUID);                 ///< Requester Guid
    l_TicketID = p_Packet.read<uint32>();                           ///< Ticket ID
    l_TicketType = p_Packet.read<uint32>();                           ///< Ticket type
    l_TicketTime = p_Packet.read<uint32>();                           ///< Ticket Time

    p_Packet >> l_QueueID;                                  ///< Instance ID
    p_Packet >> l_ProposalID;                               ///< Proposal ID
    l_Accepted = p_Packet.ReadBit();

    sLFGMgr->UpdateProposal(l_ProposalID, GetPlayer()->GetGUID(), l_Accepted);
}

void WorldSession::HandleDfSetRolesOpcode(WorldPacket& p_Packet)
{
    uint32 l_RolesDesired;
    uint8 l_PartyIndex;

    p_Packet >> l_RolesDesired;
    p_Packet >> l_PartyIndex;

    uint64 l_Guid = GetPlayer()->GetGUID();
    GroupPtr l_Group = GetPlayer()->GetGroup();
    if (!l_Group)
        return;

    uint64 l_GroupGUID = l_Group->GetGUID();
    sLFGMgr->UpdateRoleCheck(l_GroupGUID, l_Guid, l_RolesDesired);
}

void WorldSession::HandleLfgSetBootVoteOpcode(WorldPacket& recvData)
{
    bool agree;                                            // Agree to kick player
    agree = recvData.ReadBit();

    sLFGMgr->UpdateBoot(GetPlayer(), agree);
}

void WorldSession::HandleLfgTeleportOpcode(WorldPacket& recvData)
{
    bool out;
    out = recvData.ReadBit();

    sLFGMgr->TeleportPlayer(GetPlayer(), out, true);
}

void WorldSession::HandleLfgLockInfoRequestOpcode(WorldPacket& p_Packet)
{
    uint8 l_PartyIndex = 0;
    bool l_GroupPacket = false;

    l_GroupPacket = p_Packet.ReadBit();
    p_Packet >> l_PartyIndex;

    if (l_GroupPacket)
    {
        uint32 l_TimeMesure = getMSTime();

        GroupPtr l_Group = m_Player->GetGroup();
        if (!l_Group)
            return;

        std::map<uint64, LfgLockMap> l_LockMap;

        l_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr) -> void
        {
            Player* l_CurrentGroupPlayer = l_Itr->player;

            if (!l_CurrentGroupPlayer || l_CurrentGroupPlayer == m_Player)
                return;

            LfgLockMap const& l_LockedDungeons = sLFGMgr->GetLockedDungeons(l_CurrentGroupPlayer->GetGUID());
            for (auto l_Iter : l_LockedDungeons)
                l_LockMap[l_CurrentGroupPlayer->GetGUID()].insert(l_Iter);
        });

        uint32 l_Time1 = getMSTime() - l_TimeMesure;

        WorldPacket l_Data(Opcodes::SMSG_LFG_PARTY_INFO, 150 * 1024);

        l_Data << uint32(l_LockMap.size());

        for (std::map<uint64, LfgLockMap>::const_iterator l_It = l_LockMap.begin(); l_It != l_LockMap.end(); ++l_It)
        {
            l_Data.WriteBit(l_It->first != 0);
            l_Data.FlushBits();

            l_Data << uint32(l_It->second.size());

            if (l_It->first)
                l_Data.appendPackGUID(l_It->first);

            for (LfgLockMap::const_iterator l_It2 = l_It->second.begin(); l_It2 != l_It->second.end(); ++l_It2)
            {
                LfgLockStatus l_LockData = l_It2->second;
                l_Data << uint32(l_It2->first);                         ///< Slot
                l_Data << uint32(l_LockData.lockstatus);                ///< Reason
                l_Data << uint32(l_LockData.SubReason1);                ///< Sub Reason 1
                l_Data << uint32(l_LockData.SubReason2);                ///< Sub Reason 2
            }
        }

        SendPacket(&l_Data);

        uint32 l_Time2 = getMSTime() - l_TimeMesure;

        if (l_Time2 > 10)
            sLog->outAshran("CMSG_DFGET_SYSTEM_INFO Group %u %u", l_Time1, l_Time2);
    }
    else
    {
        uint32 l_TimeMesure = getMSTime();

        /// Get Random dungeons that can be done at a certain level and expansion
        LfgDungeonSet l_RandomDungeons;

        uint8 l_Level = m_Player->getLevel();
        uint8 l_Expansion = m_Player->GetSession()->Expansion();

        for (uint32 l_I = 0; l_I < sLFGDungeonStore.GetNumRows(); ++l_I)
        {
            LFGDungeonEntry const* l_Dungeon = sLFGDungeonStore.LookupEntry(l_I);

            if (l_Dungeon && l_Dungeon->expansion <= l_Expansion && l_Dungeon->minlevel <= l_Level && l_Level <= l_Dungeon->maxlevel)
            {
                /// 12-02-2016
                /// @TODO: Someday, it'll needs to be done properly...
                if (l_Dungeon->Flags & LfgFlags::LFG_FLAG_SEASONAL)
                {
                    uint8 l_EventEntry = 0;
                    switch (l_Dungeon->ID)
                    {
                        case 285: ///< The Headless Horseman
                            l_EventEntry = 12; ///< Hallow's End
                            break;
                        case 286: ///< The Frost Lord Ahune
                            l_EventEntry = 1; ///< Midsummer Fire Festival
                            break;
                        case 287: ///< Coren Direbrew
                            l_EventEntry = 26; ///< Brewfest
                            break;
                        case 288: ///< The Crown Chemical Co.
                            l_EventEntry = 8; ///< Love is in the Air
                            break;
                        default:
                            break;
                    }

                    if (l_EventEntry && sGameEventMgr->IsActiveEvent(l_EventEntry))
                        l_RandomDungeons.insert(l_Dungeon->Entry());

                    continue;
                }
                else if (l_Dungeon->type != LfgType::TYPEID_RANDOM_DUNGEON && l_Dungeon->type != LfgType::TYPEID_DUNGEON)
                    continue;
                /// Mythic difficulty shouldn't be offered in LFG, Scenarios aswell
                else if (l_Dungeon->difficulty == Difficulty::DifficultyMythic || l_Dungeon->difficulty == Difficulty::DifficultyNScenario || l_Dungeon->difficulty == Difficulty::DifficultyHCScenario)
                    continue;

                l_RandomDungeons.insert(l_Dungeon->Entry());
            }
        }

        uint32 l_Time1 = getMSTime() - l_TimeMesure;


        /// Get player locked Dungeons
        LfgLockMap l_Lock = sLFGMgr->GetLockedDungeons(m_Player->GetGUID());

        uint32 l_DungeonCount = uint32(l_RandomDungeons.size());
        uint32 l_BlackListCount = uint32(l_Lock.size());

        bool l_HasGUID = true;

        uint32 l_Time2 = getMSTime() - l_TimeMesure;

        WorldPacket l_Data(Opcodes::SMSG_LFG_PLAYER_INFO, 15 * 1024);

        l_Data << uint32(l_DungeonCount);

        l_Data.WriteBit(l_HasGUID);
        l_Data.FlushBits();
        l_Data << uint32(l_BlackListCount);

        if (l_HasGUID)
            l_Data.appendPackGUID(m_Player->GetGUID());

        for (LfgLockMap::const_iterator l_It = l_Lock.begin(); l_It != l_Lock.end(); ++l_It)
        {
            LfgLockStatus l_LockData = l_It->second;
            l_Data << uint32(l_It->first);
            l_Data << uint32(l_LockData.lockstatus);
            l_Data << uint32(l_LockData.SubReason1);
            l_Data << uint32(l_LockData.SubReason2);
        }

        uint32 l_Time3 = getMSTime() - l_TimeMesure;

        for (LfgDungeonSet::const_iterator l_It = l_RandomDungeons.begin(); l_It != l_RandomDungeons.end(); ++l_It)
        {
            LfgReward const* l_Reward = sLFGMgr->GetRandomDungeonReward(*l_It, l_Level);
            Quest const* l_Quest = nullptr;
            LFGDungeonEntry const* l_Dungeon = sLFGDungeonStore.LookupEntry((*l_It) & 0x00FFFFFF);

            bool l_IsDone = false;
            if (l_Reward)
            {
                l_Quest = sObjectMgr->GetQuestTemplate(l_Reward->reward[0].questId);

                if (l_Quest)
                {
                    l_IsDone = !m_Player->CanRewardQuest(l_Quest, false);

                    if (l_IsDone)
                        l_Quest = sObjectMgr->GetQuestTemplate(l_Reward->reward[1].questId);
                }
            }

            uint32 l_CompletedEncouterMask = 0;
            DungeonEncounterList const* l_Encounters = sObjectMgr->GetDungeonEncounterList(l_Dungeon ? l_Dungeon->map : 0, Difficulty(l_Dungeon ? l_Dungeon->difficulty : 0));
            if (l_Encounters && l_Encounters->size())
            {
                for (DungeonEncounterList::const_iterator l_Iter = l_Encounters->begin(); l_Iter != l_Encounters->end(); ++l_Iter)
                {
                    for (auto l_Value : m_Player->GetBossLooted())
                    {
                        uint32 l_Entry          = l_Value.first & 0xFFFFFFFF;
                        uint32 l_DisplayID      = l_Value.first >> 32;
                        uint32 l_DifficultyID   = l_Value.second;

                        if (uint32(l_Dungeon->difficulty) != l_DifficultyID)
                            continue;

                        if (l_Entry == (*l_Iter)->creditEntry || l_DisplayID == (*l_Iter)->dbcEntry->CreatureDisplayID)
                            l_CompletedEncouterMask |= 1 << (*l_Iter)->dbcEntry->Bit;
                    }
                }
            }

            l_Data << uint32(*l_It);                    ///< Slots
            l_Data << uint32(0);                        ///< Completion Quantity
            l_Data << uint32(0);                        ///< CompletionLimit
            l_Data << uint32(0);                        ///< CompletionCurrencyID
            l_Data << uint32(0);                        ///< SpecificQuantity
            l_Data << uint32(0);                        ///< SpecificLimit
            l_Data << uint32(0);                        ///< OverallQuantity
            l_Data << uint32(0);                        ///< OverallLimit
            l_Data << uint32(0);                        ///< PurseWeeklyQuantity
            l_Data << uint32(0);                        ///< PurseWeeklyLimit
            l_Data << uint32(0);                        ///< PurseQuantity
            l_Data << uint32(0);                        ///< PurseLimit
            l_Data << uint32(0);                        ///< Quantity
            l_Data << uint32(0);                        ///< Unk 7.1.5
            l_Data << uint32(l_CompletedEncouterMask);

            uint32 l_ShortageRewardCount = 0;
            l_Data << uint32(l_ShortageRewardCount);

            l_Data.WriteBit(0);                         ///< FirstReward
            l_Data.WriteBit(!l_IsDone);                 ///< ShortageEligible
            l_Data.FlushBits();

            /// Rewards
            {
                uint32 l_BonusCurrencyCount = 0;

                if (l_Quest)
                {
                    l_Data << uint32(0);                ///< Mask
                    l_Data << uint32(l_Quest->MoneyValue(m_Player->getLevel()));
                    l_Data << uint32(m_Player->getLevel() < MAX_LEVEL ? l_Quest->XPValue(m_Player) : 0);
                    l_Data << uint32(l_Quest->GetRewItemsCount());
                    l_Data << uint32(l_Quest->GetRewCurrencyCount());
                    l_Data << uint32(l_BonusCurrencyCount);
                }
                else
                {
                    l_Data << uint32(0);
                    l_Data << uint32(0);
                    l_Data << uint32(0);
                    l_Data << uint32(0);
                    l_Data << uint32(0);
                    l_Data << uint32(0);
                }

                if (l_Quest)
                {
                    if (l_Quest->GetRewItemsCount())
                    {
                        for (uint8 l_I = 0; l_I < QUEST_REWARDS_COUNT; ++l_I)
                        {
                            if (!l_Quest->RewardItemId[l_I])
                                continue;

                            l_Data << uint32(l_Quest->RewardItemId[l_I]);
                            l_Data << uint32(l_Quest->RewardItemIdCount[l_I]);
                        }
                    }

                    if (l_Quest->GetRewCurrencyCount())
                    {
                        for (uint8 l_I = 0; l_I < QUEST_REWARDS_COUNT; ++l_I)
                        {
                            if (!l_Quest->RewardCurrencyId[l_I])
                                continue;

                            l_Data << uint32(l_Quest->RewardCurrencyId[l_I]);
                            l_Data << uint32(l_Quest->RewardCurrencyCount[l_I]);
                        }
                    }

                    for (uint32 l_I = 0; l_I < l_BonusCurrencyCount; ++l_I)
                    {
                        l_Data << uint32(0);    ///< CurrencyID
                        l_Data << uint32(0);    ///< CurrencyCount
                    }
                }

                bool l_UnkBit10 = false;
                bool l_UnkBit18 = false;
                bool l_UnkBit20 = false;
                bool l_UnkBit28 = false;

                l_Data.WriteBit(l_UnkBit10);
                l_Data.WriteBit(l_UnkBit18);
                l_Data.WriteBit(l_UnkBit20);
                l_Data.WriteBit(l_UnkBit28);

                if (l_UnkBit10)
                    l_Data << uint32(0);

                if (l_UnkBit18)
                    l_Data << uint32(0);

                if (l_UnkBit20)
                    l_Data << uint32(0);

                if (l_UnkBit28)
                    l_Data << uint32(0);

                for (uint32 l_I = 0; l_I < l_ShortageRewardCount; ++l_I)
                {
                    l_Data << uint32(0);                ///< Mask
                    l_Data << uint32(0);                ///< RewardMoney
                    l_Data << uint32(0);                ///< RewardXP
                    l_Data << uint32(0);                ///< ItemCount
                    l_Data << uint32(0);                ///< CurrencyCount
                    l_Data << uint32(0);                ///< QuantityCount

                    l_Data.WriteBit(false);             ///< UnkBit30
                }

                l_Data.FlushBits();
            }
        }

        SendPacket(&l_Data);

        uint32 l_Time4 = getMSTime() - l_TimeMesure;

        if (l_Time4 > 10)
            sLog->outAshran("CMSG_DFGET_SYSTEM_INFO  %u %u %u %u", l_Time1, l_Time2, l_Time3, l_Time4);
    }
}

void WorldSession::HandleLfrSearchOpcode(WorldPacket& recvData)
{
    uint32 entry;                                          // Raid id to search
    recvData >> entry;
    //SendLfrUpdateListOpcode(entry);
}

void WorldSession::HandleLfrLeaveOpcode(WorldPacket& recvData)
{
    uint32 dungeonId;                                      // Raid id queue to leave
    recvData >> dungeonId;
    //sLFGMgr->LeaveLfr(GetPlayer(), dungeonId);
}

void WorldSession::HandleLfgGetStatus(WorldPacket& /*recvData*/)
{
    sLog->outDebug(LOG_FILTER_LFG, "CMSG_LFG_GET_STATUS [" UI64FMTD "]", GetPlayer()->GetGUID());

    uint64 guid = GetPlayer()->GetGUID();
    LfgUpdateData updateData = sLFGMgr->GetLfgStatus(guid);

    sLFGMgr->SendUpdateStatus(GetPlayer(), updateData);
}

void WorldSession::SendLfgRoleChosen(uint64 p_Guid, uint8 p_Roles)
{
    WorldPacket l_Data(SMSG_LFG_ROLE_CHOSEN, 12);
    l_Data.appendPackGUID(p_Guid);
    l_Data << uint32(p_Roles);

    l_Data.WriteBit(p_Roles > 0);
    l_Data.FlushBits();

    SendPacket(&l_Data);
}

void WorldSession::SendLfgRoleCheckUpdate(const LfgRoleCheck * p_RoleCheck)
{
    ASSERT(p_RoleCheck);
    LfgDungeonSet l_JoinSlots;

    if (p_RoleCheck->rDungeonId)
        l_JoinSlots.insert(p_RoleCheck->rDungeonId);
    else
        l_JoinSlots = p_RoleCheck->dungeons;

    uint64 l_BGqueueID = 0;

    GroupPtr l_Group = m_Player->GetGroup();

    WorldPacket l_Data(SMSG_LFG_ROLE_CHECK_UPDATE, 200);

    l_Data << uint8(l_Group ? l_Group->GetPartyIndex() : 0);                                ///< Party index
    l_Data << uint8(p_RoleCheck->state);                                                    ///< Role check status
    l_Data << uint32(l_JoinSlots.size());                                                   ///< Join Slots count
    l_Data << uint64(l_BGqueueID);                                                          ///< BG Queue ID
    l_Data << uint32(0);                                                                    ///< Activity ID
    l_Data << uint32(p_RoleCheck->roles.size());                                            ///< Member count

    if (!l_JoinSlots.empty())
    {
        for (LfgDungeonSet::iterator l_It = l_JoinSlots.begin(); l_It != l_JoinSlots.end(); ++l_It)
        {
            LFGDungeonEntry const* l_DungeonEntry = sLFGDungeonStore.LookupEntry(*l_It);
            l_Data << uint32(l_DungeonEntry ? l_DungeonEntry->Entry() : 0);                 ///< Dungeon
        }
    }

    l_Data.WriteBit(p_RoleCheck->state == LFG_ROLECHECK_INITIALITING);                      ///< Is Beginning
    l_Data.WriteBit(false);

    if (!p_RoleCheck->roles.empty())
    {
        ObjectGuid guid = p_RoleCheck->leader;
        uint8 l_Roles = p_RoleCheck->roles.find(guid)->second;
        Player* l_CurrentPlayer = ObjectAccessor::FindPlayer(guid);

        l_Data.appendPackGUID(l_CurrentPlayer ? l_CurrentPlayer->GetGUID() : 0);            ///< Guid
        l_Data << uint32(l_Roles);                                                          ///< Roles Desired
        l_Data << uint8(l_CurrentPlayer ? l_CurrentPlayer->getLevel() : 0);                 ///< Level
        l_Data.WriteBit(l_Roles > 0);                                                       ///< Role Check Complete
        l_Data.FlushBits();

        for (LfgRolesMap::const_reverse_iterator l_It = p_RoleCheck->roles.rbegin(); l_It != p_RoleCheck->roles.rend(); ++l_It)
        {
            if (l_It->first == p_RoleCheck->leader)
                continue;

            l_Roles = l_It->second;
            l_CurrentPlayer = ObjectAccessor::FindPlayer(l_It->first);

            l_Data.appendPackGUID(l_CurrentPlayer ? l_CurrentPlayer->GetGUID() : 0);        ///< Guid
            l_Data << uint32(l_Roles);                                                      ///< Roles Desired
            l_Data << uint8(l_CurrentPlayer ? l_CurrentPlayer->getLevel() : 0);             ///< Level
            l_Data.WriteBit(l_Roles > 0);                                                   ///< Role Check Complete
            l_Data.FlushBits();
        }
    }                                                 ///< Re queue

    SendPacket(&l_Data);
}

void WorldSession::SendLfgJoinResult(uint64 p_Guid, const LfgJoinResultData & p_JoinData)
{
    uint32 l_JoinTime = sLFGMgr->GetLfgQueueJoinTime(GetPlayer()->GetGroup() ? GetPlayer()->GetGroup()->GetGUID() : GetPlayer()->GetGUID());

    WorldPacket l_Data(SMSG_LFG_JOIN_RESULT, 500);

    l_Data.appendPackGUID(p_Guid);                                      ///< Requester Guid
    l_Data << uint32(m_Player->GetTeam());                              ///< ID
    l_Data << uint32(2);                                                ///< Type
    l_Data << uint32(l_JoinTime);                                       ///< Time

    l_Data << uint8(p_JoinData.result);                                 ///< Result
    l_Data << uint8(p_JoinData.state);                                  ///< Result Detail
    l_Data << uint32(p_JoinData.lockmap.size());                        ///< BlackList count

    for (LfgLockPartyMap::const_iterator l_It = p_JoinData.lockmap.begin(); l_It != p_JoinData.lockmap.end(); ++l_It)
    {
        l_Data.appendPackGUID(l_It->first);                             ///< Guid
        l_Data << uint32(l_It->second.size());                          ///< Slot count

        LfgLockMap l_LockMap = l_It->second;
        for (LfgLockMap::const_iterator l_LockMapIT = l_LockMap.begin(); l_LockMapIT != l_LockMap.end(); ++l_LockMapIT)
        {
            auto l_LockData = l_LockMapIT->second;
            l_Data << uint32(l_LockMapIT->first);                       ///< Dungeon entry (id + type)
            l_Data << uint32(l_LockData.lockstatus);                    ///< Reason
            l_Data << uint32(l_LockData.SubReason1);                    ///< Sub Reason 1
            l_Data << uint32(l_LockData.SubReason2);                    ///< Sub Reason 2
        }
    }

    SendPacket(&l_Data);
}

void WorldSession::SendLfgQueueStatus(uint32 p_Dungeon, int32 p_WaitTime, int32 p_AvgWaitTime, int32 p_WaitTimeTanks, int32 p_WaitTimeHealer, int32 p_WaitTimeDps, uint32 p_QueuedTime, uint8 p_TankCount, uint8 p_HealerCount, uint8 p_DPSCount)
{
    uint32 l_JoinTime = sLFGMgr->GetLfgQueueJoinTime(GetPlayer()->GetGroup() ? GetPlayer()->GetGroup()->GetGUID() : GetPlayer()->GetGUID());

    WorldPacket l_Data(SMSG_LFG_QUEUE_STATUS, 50);
    l_Data.appendPackGUID(GetPlayer()->GetGUID());          ///< Requester Guid
    l_Data << uint32(GetPlayer()->GetTeam());               ///< Id
    l_Data << uint32(2);                                    ///< Type
    l_Data << int32(l_JoinTime);                            ///< Time

    l_Data << uint32(p_Dungeon);                            ///< Slot
    l_Data << uint32(p_AvgWaitTime);                        ///< Avg Wait Time
    l_Data << uint32(p_QueuedTime);                         ///< Queued Time

    l_Data << int32(p_WaitTimeTanks);                       ///< Avg Wait Time By Role 0
    l_Data << uint8(p_TankCount);                           ///< Last Needed 0

    l_Data << int32(p_WaitTimeHealer);                      ///< Avg Wait Time By Role 1
    l_Data << uint8(p_HealerCount);                         ///< Last Needed 1

    l_Data << int32(p_WaitTimeDps);                         ///< Avg Wait Time By Role 2
    l_Data << uint8(p_DPSCount);                            ///< Last Needed 2

    l_Data << uint32(p_WaitTime);                           ///< Avg Wait Time Me

    SendPacket(&l_Data);
}

void WorldSession::SendLfgPlayerReward(uint32 rdungeonEntry, uint32 sdungeonEntry, uint8 /*done*/, const LfgReward* /*reward*/, const Quest* p_Quest)
{
    if (!rdungeonEntry || !sdungeonEntry || !p_Quest)
        return;

    uint8 l_ItemCount = uint8(p_Quest ? p_Quest->GetRewItemsCount() + p_Quest->GetRewCurrencyCount()  : 0);

    WorldPacket l_Data(SMSG_LFG_PLAYER_REWARD, 4 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 1 + l_ItemCount * (4 + 4 + 4));
    l_Data << uint32(sdungeonEntry);                             ///< Actual Slot
    l_Data << uint32(rdungeonEntry);                             ///< Queue Slot
    l_Data << uint32(p_Quest->MoneyValue(m_Player->getLevel())); //< Reward money
    l_Data << uint32(p_Quest->XPValue(GetPlayer()));             ///< Added XP
    l_Data << uint32(l_ItemCount);                               ///< Reward count

    if (p_Quest && p_Quest->GetRewItemsCount())
    {
        for (uint8 l_I = 0; l_I < QUEST_REWARDS_COUNT; ++l_I)
        {
            if (!p_Quest->RewardItemId[l_I])
                continue;

            l_Data << uint32(p_Quest->RewardItemId[l_I]);       ///< RewardItem
            l_Data << uint32(p_Quest->RewardItemIdCount[l_I]);  ///< RewardItemQuantity
            l_Data << uint32(0);                                ///< BonusCurrency

            l_Data.WriteBit(0);                                 ///< Is Currency
            l_Data.FlushBits();
        }
    }
    if (p_Quest && p_Quest->GetRewCurrencyCount())
    {
        for (uint8 l_I = 0; l_I < QUEST_REWARD_CURRENCY_COUNT; ++l_I)
        {
            if (!p_Quest->RewardCurrencyId[l_I])
                continue;

            l_Data << uint32(p_Quest->RewardCurrencyId[l_I]);   ///< RewardItem
            l_Data << uint32(p_Quest->RewardCurrencyCount[l_I]);///< RewardItemQuantity
            l_Data << uint32(0);                                ///< BonusCurrency

            l_Data.WriteBit(1);                                 ///< Is Currency
            l_Data.FlushBits();
        }
    }

    SendPacket(&l_Data);
}

void WorldSession::SendLfgBootPlayer(const LfgPlayerBoot* p_Boot)
{
    LfgAnswer l_MyVote = p_Boot->votes.find(GetPlayer()->GetGUID())->second;
    uint8 l_VoteCount = 0;
    uint8 l_BootVoteCount = 0;
    uint32 l_TimeLeft = uint8((p_Boot->cancelTime - time(NULL)) / 1000);

    for (LfgAnswerMap::const_iterator l_It = p_Boot->votes.begin(); l_It != p_Boot->votes.end(); ++l_It)
    {
        if (l_It->second != LFG_ANSWER_PENDING)
        {
            ++l_VoteCount;

            if (l_It->second == LFG_ANSWER_AGREE)
                ++l_BootVoteCount;
        }
    }

    WorldPacket l_Data(SMSG_LFG_BOOT_PLAYER, 1 + 1 + 1 + 8 + 4 + 4 + 4 + 4 + p_Boot->reason.length());
    l_Data.WriteBit(p_Boot->inProgress);                        ///< Vote in progress
    l_Data.WriteBit(l_BootVoteCount >= p_Boot->votedNeeded);    ///< Vote Passed
    l_Data.WriteBit(l_MyVote != LFG_ANSWER_PENDING);            ///< My Vote Completed
    l_Data.WriteBit(l_MyVote);                                  ///< My Vote
    l_Data.WriteBits(p_Boot->reason.size(), 8);                 ///< Reason length
    l_Data.FlushBits();

    l_Data.appendPackGUID(p_Boot->victim);                      ///< Target
    l_Data << uint32(l_VoteCount);                              ///< Total Votes
    l_Data << uint32(l_BootVoteCount);                          ///< Boot votes
    l_Data << uint32(l_TimeLeft);                               ///< Time left
    l_Data << uint32(p_Boot->votedNeeded);                      ///< Votes needed

    l_Data.WriteString(p_Boot->reason);                         ///< Reason

    SendPacket(&l_Data);
}

void WorldSession::SendLfgUpdateProposal(uint32 p_ProposalID, const LfgProposal * p_Proposal)
{
    if (!p_Proposal)
        return;

    uint64 l_Guid = GetPlayer()->GetGUID();
    LfgProposalPlayerMap::const_iterator l_PlayerIt = p_Proposal->players.find(l_Guid);

    if (l_PlayerIt == p_Proposal->players.end())                  // Player MUST be in the proposal
        return;

    LfgProposalPlayer* l_ProposalPlayer = l_PlayerIt->second;

    uint32 l_CompletedEncounters    = 0;
    uint32 l_PlayerLowGuid          = l_ProposalPlayer->groupLowGuid;
    uint32 l_GroupLowGuid           = p_Proposal->groupLowGuid;
    uint32 l_DungeonID              = p_Proposal->dungeonId;

    bool l_Silent           = false;
    bool l_CompletedMask    = false;
    bool l_IsContinue       = false;

    GroupPtr l_Group = l_GroupLowGuid ? sGroupMgr->GetGroupByGUID(l_GroupLowGuid) : NULL;

    if (l_Group)
    {
        l_IsContinue    = l_Group->isLFGGroup() && sLFGMgr->GetState(l_Group->GetGUID()) != LFG_STATE_FINISHED_DUNGEON;
        l_CompletedMask = l_Group->isLFGGroup() && sLFGMgr->GetState(l_Group->GetGUID()) == LFG_STATE_FINISHED_DUNGEON;
        l_Silent        = GetPlayer()->GetGroup() == l_Group && l_IsContinue;
    }

    WorldPacket l_Data(SMSG_LFG_PROPOSAL_UPDATE, 300);

    if (!l_IsContinue)  ///< Only show proposal dungeon if it's continue
    {
        LfgDungeonSet playerDungeons = sLFGMgr->GetSelectedDungeons(l_Guid);

        if (playerDungeons.size() == 1)
            l_DungeonID = (*playerDungeons.begin());
    }

    if (const LFGDungeonEntry* l_DungeonEntry = sLFGDungeonStore.LookupEntry(l_DungeonID))
    {
        l_DungeonID = l_DungeonEntry->Entry();

        /// Select a player inside to be get completed encounters from
        if (l_Group)
        {
            l_Group->GetMemberSlots().foreach_until([&](Group::MemberSlotPtr l_Itr)
            {
                Player * l_GroupMember = l_Itr->player;

                if (l_GroupMember && l_GroupMember->GetMapId() == uint32(l_DungeonEntry->map))
                {
                    if (InstanceScript* l_Instance = l_GroupMember->GetInstanceScript())
                        l_CompletedEncounters = l_Instance->GetCompletedEncounterMask();

                    return true;
                }

                return false;
            });
        }
    }

    uint64 InstanceSaveGUID = MAKE_NEW_GUID(l_DungeonID, 0, HIGHGUID_INSTANCE_SAVE);

    uint32 l_JoinTime = sLFGMgr->GetLfgQueueJoinTime(GetPlayer()->GetGroup() ? GetPlayer()->GetGroup()->GetGUID() : GetPlayer()->GetGUID());

    l_Data.appendPackGUID(l_Guid);                                          ///< RequesterGuid
    l_Data << uint32(m_Player->GetTeam());                                  ///< Id
    l_Data << uint32(2);                                                    ///< Type
    l_Data << uint32(l_JoinTime);                                           ///< Time

    l_Data << uint64(InstanceSaveGUID);                                     ///< InstanceID
    l_Data << uint32(p_ProposalID);                                         ///< ProposalID
    l_Data << uint32(l_DungeonID);                                          ///< Slot
    l_Data << uint8(p_Proposal->state);                                     ///< State
    l_Data << uint32(l_CompletedEncounters);                                ///< CompletedMask
    l_Data << uint32(0);                                                    ///< EncounterMask
    l_Data << uint32(p_Proposal->players.size());                           ///< PlayersCount
    l_Data << uint8(0);

    l_Data.WriteBit(l_CompletedMask);                                       ///< Valid Completed Mask
    l_Data.WriteBit(l_Silent);                                              ///< Proposal Silent
    l_Data.WriteBit(false);                                                 ///< UnkLegion
    l_Data.FlushBits();

    for (l_PlayerIt = p_Proposal->players.begin(); l_PlayerIt != p_Proposal->players.end(); ++l_PlayerIt)
    {
        bool l_MyParty   = false;
        bool l_SameParty = false;

        if (l_PlayerIt->second->groupLowGuid)
        {
            l_MyParty = l_PlayerIt->second->groupLowGuid == l_GroupLowGuid;
            l_SameParty = l_PlayerIt->second->groupLowGuid == l_PlayerLowGuid;
        }

        l_Data << uint32(l_PlayerIt->second->role);                         ///< Roles

        l_Data.WriteBit(l_PlayerIt->first == l_Guid);                       ///< Self player
        l_Data.WriteBit(l_SameParty);                                       ///< Same Group than player
        l_Data.WriteBit(l_MyParty);                                         ///< In dungeon (silent)
        l_Data.WriteBit(l_PlayerIt->second->accept != LFG_ANSWER_PENDING);  ///< Answered
        l_Data.WriteBit(l_PlayerIt->second->accept == LFG_ANSWER_AGREE);    ///< Accepted
        l_Data.FlushBits();
    }

    SendPacket(&l_Data);
}

void WorldSession::SendLfgDisabled()
{
    WorldPacket data(SMSG_LFG_DISABLED, 0);
    SendPacket(&data);
}

void WorldSession::SendLfgOfferContinue(uint32 p_DungeonEntry)
{
    WorldPacket l_Data(SMSG_LFG_OFFER_CONTINUE, 4);
    l_Data << uint32(p_DungeonEntry);

    SendPacket(&l_Data);
}

void WorldSession::SendLfgTeleportError(uint8 p_Error)
{
    WorldPacket l_Data(SMSG_LFG_TELEPORT_DENIED, 4);
    l_Data.WriteBits(p_Error, 4);                                   // Error
    l_Data.FlushBits();

    SendPacket(&l_Data);
}
