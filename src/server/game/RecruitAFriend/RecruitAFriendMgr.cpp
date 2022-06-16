////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "RecruitAFriendMgr.hpp"
#include "Group.h"
#include "HelperDefines.h"
#include "InterRealmOpcodes.h"
#include "InterRealmMgr.h"
#include <openssl/sha.h>
#include "GarbageCollector.h"

RecruitAFriendMgr::RecruitAFriendRequest::RecruitAFriendRequest(std::string p_Mail) :
    Mail(p_Mail), Note("") { }

RecruitAFriendMgr::RecruitAFriendRequest::RecruitAFriendRequest() : Mail(""), Note("") { }

RecruitAFriendMgr::RecruitAFriendData::RecruitAFriendData() :
    RefererAccountID(0), RecruitAccountID(0), Faction(-1), RecruitCharacterGUID(0), StartTime(0),
    RefererCharacterGUID(0), InRange(false), InSameGroup(false), Active(false),
    RefererLoggedIn(false), RecruitLoggedIn(false) { }

RecruitAFriendMgr::RecruitAFriendMgrEvent::RecruitAFriendMgrEvent(uint32 p_EventType) :
    EventType(p_EventType), Apply(false), GUID(0), GUID2(0), String(""), UInt(0), Ptr(nullptr) { }

RecruitAFriendMgr::RecruitAFriendPlayer::RecruitAFriendPlayer(uint64 p_Guid) :
    Guid(p_Guid), Referer(0) { }

RecruitAFriendMgr::RecruitAFriendPlayer::RecruitAFriendPlayer() :
    Guid(0), Referer(0) { }

RecruitAFriendMgr::RecruitAFriendPlayer::RAFBonuses::RAFBonuses() :
    Initialized(false), Legendary(false), RewardMask(0), NotificationsEnabled(true), RewardAvailable(0) { }

RecruitAFriendMgr::RecruitAFriendEventPacket::RecruitAFriendEventPacket() : EventCount(0) { }

RecruitAFriendMgr::RAFMailList::RAFMailList(std::set<std::string> const& p_MailList) : Mails(p_MailList) {}

RecruitAFriendMgr::RAFMailList::RAFMailList() {}

std::set<std::string>::const_iterator RecruitAFriendMgr::RAFMailList::find(std::string p_String)
{
    auto l_Itr = Mails.find(p_String);
    if (l_Itr == Mails.end())
    {
        std::ostringstream l_Oss;
        l_Oss << p_String << sWorld->GetRAFSalt();;

        std::string const l_Hash = CalculateSHA1Hash(l_Oss.str());

        l_Itr = Mails.find(l_Hash);
    }

    return l_Itr;
}

std::string RecruitAFriendMgr::RAFMailList::CalculateSHA1Hash(std::string const& p_Content)
{
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)p_Content.c_str(), p_Content.length(), (unsigned char*)&digest);

    return ByteArrayToHexStr(digest, SHA_DIGEST_LENGTH);
}

std::set<std::string>::const_iterator RecruitAFriendMgr::RAFMailList::end()
{
    return Mails.end();
}

void RecruitAFriendMgr::RAFMailList::insert(std::string p_String)
{
    Mails.insert(p_String);
}

std::set<std::string> const& RecruitAFriendMgr::RAFMailList::operator=(std::set<std::string> const& p_Rhs)
{
    return Mails = p_Rhs;
}

void RecruitAFriendMgr::RecruitAFriendPlayer::SaveBonuses(Player* p_Player)
{
#ifndef CROSS
    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_REP_RAF_ACC_BONUSES);
    l_Statement->setUInt32(0, p_Player->GetSession()->GetAccountId()); ///< AccountId
    l_Statement->setBool(1, Bonuses.Legendary); ///< Legendary
    l_Statement->setUInt32(2, Bonuses.RewardMask); ///< RewardMask
    l_Statement->setUInt32(3, Bonuses.RewardAvailable); ///< RewardMask
    l_Statement->setBool(4, Bonuses.NotificationsEnabled); ///< RewardMask
    LoginDatabase.AsyncQuery(l_Statement);
#endif // !CROSS
}

float RecruitAFriendMgr::GetFloatConstant(eRecruitAFriendConstants p_Constant)
{
    if (k_FloatConstants.find(p_Constant) == k_FloatConstants.end())
        return 1.0f;

    return k_FloatConstants[p_Constant];
}

void RecruitAFriendMgr::DeleteReferencesAndData(RecruitAFriendData* p_Data)
{
    m_RecruitXData.erase(p_Data->RecruitAccountID);
    m_RecruitCharXData.erase(p_Data->RecruitCharacterGUID);

    if (m_RefererXData.find(p_Data->RefererAccountID) != m_RefererXData.end())
    {
        auto l_Pred = [p_Data](RecruitAFriendData const* i_Data) -> bool
        {
            return i_Data == p_Data;
        };

        auto l_Itr = std::find_if(m_RefererXData[p_Data->RefererAccountID].begin(), m_RefererXData[p_Data->RefererAccountID].end(), l_Pred);
        if (l_Itr != m_RefererXData[p_Data->RefererAccountID].end())
            m_RefererXData[p_Data->RefererAccountID].erase(l_Itr);

        if (m_RefererXData[p_Data->RefererAccountID].empty())
            m_RefererXData.erase(p_Data->RefererAccountID);
    }

    if (m_RefererCharXData.find(p_Data->RefererCharacterGUID) != m_RefererCharXData.end())
    {
        auto l_Pred = [p_Data](RecruitAFriendData const* i_Data) -> bool
        {
            return i_Data == p_Data;
        };

        auto l_Itr = std::find_if(m_RefererCharXData[p_Data->RefererCharacterGUID].begin(), m_RefererCharXData[p_Data->RefererCharacterGUID].end(), l_Pred);
        if (l_Itr != m_RefererCharXData[p_Data->RefererCharacterGUID].end())
            m_RefererCharXData[p_Data->RefererCharacterGUID].erase(l_Itr);

        if (m_RefererCharXData[p_Data->RefererCharacterGUID].empty())
            m_RefererCharXData.erase(p_Data->RefererCharacterGUID);
    }

    sGarbageCollector->Add(p_Data);
}

void RecruitAFriendMgr::Initialize()
{
    uint32 l_OldMSTime = getMSTime();

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_RAF_REQUEST_MAILS);
    PreparedQueryResult l_Result = LoginDatabase.Query(l_Statement);
    if (l_Result)
    {
        uint32 l_Count = 0;
        do
        {
            Field* l_Fields = l_Result->Fetch();
            m_Mails.insert(l_Fields[0].GetString());

            l_Count++;
        }
        while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u recruit a friend mails in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
    }

    l_Statement = WorldDatabase.GetPreparedStatement(WORLD_SEL_RAF_REWARDS);
    l_Result = WorldDatabase.Query(l_Statement);

    if (l_Result)
    {
        do
        {
            RecruitAFriendReward l_Reward;
            Field* l_Fields = l_Result->Fetch();
            l_Reward.Index                                          = l_Fields[0].GetUInt8();
            l_Reward.LocalizedName[LocaleConstant::LOCALE_enUS]     = l_Fields[1].GetCString();
            l_Reward.LocalizedSubtitle[LocaleConstant::LOCALE_enUS] = l_Fields[2].GetCString();
            l_Reward.ModelDisplayID                                 = l_Fields[3].GetUInt32();
            l_Reward.Flags                                          = l_Fields[4].GetUInt32();
            l_Reward.ItemID                                         = l_Fields[5].GetUInt32();
            l_Reward.Quantity                                       = l_Fields[6].GetUInt32();

            l_Reward.Unique = l_Reward.Flags & eRewardFlags::RafRewardUnique;

            m_Rewards[l_Reward.Index] = l_Reward;

        } while (l_Result->NextRow());
    }

    l_Statement = WorldDatabase.GetPreparedStatement(WORLD_SEL_RAF_REWARDS_LOCALE);
    l_Result = WorldDatabase.Query(l_Statement);

    if (l_Result)
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint32 l_Index = l_Fields[0].GetUInt8();
            if (m_Rewards.find(l_Index) == m_Rewards.end())
                continue;

            LocaleConstant l_Locale = GetLocaleByName(l_Fields[1].GetCString());

            m_Rewards[l_Index].LocalizedName[l_Locale]      = l_Fields[2].GetCString();
            m_Rewards[l_Index].LocalizedSubtitle[l_Locale]  = l_Fields[3].GetCString();

        } while (l_Result->NextRow());
    }

    m_RefreshMailCacheTimer = 0;
}

void RecruitAFriendMgr::UpdateRequestMails(std::set<std::string> const& p_NewMailList)
{
    m_TempMailList = p_NewMailList;
}

void RecruitAFriendMgr::AddEvent(uint32 p_EventType, ...)
{
    if (!sWorld->getBoolConfig(WorldBoolConfigs::CONFIG_ENABLE_RAF))
        return;

    va_list ap;
    va_start(ap, p_EventType);

    bool l_NeedsBroadcast = (RAF::k_RafEventsInterRealm.find(p_EventType) != RAF::k_RafEventsInterRealm.end()) && RAF::k_RafEventsInterRealm[p_EventType];
#ifdef CROSS
    uint32 l_RealmID = 0;
#endif

    ByteBuffer l_EventBuffer;
    l_EventBuffer << p_EventType;

    ///sLog->outAshran("RAF--=============================");
    ///sLog->outAshran("RAF--RecruitAFriendMgr::AddEvent(var args)");
    ///sLog->outAshran("RAF--EventType : %lu", p_EventType);

    for (uint32 l_Parameter : RAF::k_RafEventsStructs[p_EventType])
    {
        switch (l_Parameter)
        {
            case eParameters::ParamGUID:
            {
                uint64 l_GUID = va_arg(ap, uint64);
                l_EventBuffer << l_GUID;
                ///sLog->outAshran("RAF--GUID 1 : %llu", l_GUID);
#ifndef CROSS
#else
                if (l_NeedsBroadcast)
                {
                    Player* l_Player = sObjectAccessor->FindPlayer(l_GUID);
                    if (!l_Player)
                        break;

                    l_RealmID = l_Player->GetSession()->GetInterRealmNumber();
                }
#endif
                break;
            }
            case eParameters::ParamGUID2:
            {
                uint64 l_GUID = va_arg(ap, uint64);
                l_EventBuffer << l_GUID;
                ///sLog->outAshran("RAF--GUID 2 : %llu", l_GUID);
                break;
            }
            case eParameters::ParamUInt:
            {
                uint32 l_UInt = va_arg(ap, uint32);
                l_EventBuffer << l_UInt;
                ///sLog->outAshran("RAF--Uint : %llu", l_UInt);
                break;
            }
            case eParameters::ParamApply:
            {
                uint32 l_Apply = va_arg(ap, uint32);
                l_EventBuffer << l_Apply;
                ///sLog->outAshran("RAF--Apply : %c", l_Apply ? 't' : 'f');
                break;
            }
            case eParameters::ParamString:
            {
                std::string l_String = va_arg(ap, char*);
                l_EventBuffer << static_cast<uint8>(l_String.length());
                l_EventBuffer.WriteString(l_String);
                ///sLog->outAshran("RAF--String : %s", l_String.c_str());
                break;
            }
            case eParameters::ParamPtr:
            {
                std::uintptr_t l_PtrValue = va_arg(ap, std::uintptr_t);
                l_EventBuffer << l_PtrValue;
                ///sLog->outAshran("RAF--Ptr : %llu", static_cast<uint64>(l_PtrValue));
                break;
            }
            default:
                va_end(ap);
                return;
        }
    }

    ///sLog->outAshran("RAF--=============================");

    if (l_NeedsBroadcast)
    {
        m_EventPacketsLock.lock();
#ifndef CROSS
        m_EventPacket.EventCount++;
        m_EventPacket.Events.append(l_EventBuffer);
#else
        if (l_RealmID)
        {
            m_EventPackets[l_RealmID].EventCount++;
            m_EventPackets[l_RealmID].Events.append(l_EventBuffer);
        }
        else
        {
            m_EventPackets[0].EventCount++;
            m_EventPackets[0].Events.append(l_EventBuffer);
        }
#endif
        m_EventPacketsLock.unlock();
    }

    AddEvent(l_EventBuffer);

    va_end(ap);
}

void RecruitAFriendMgr::AddEvent(ByteBuffer& p_Event)
{
    if (!sWorld->getBoolConfig(WorldBoolConfigs::CONFIG_ENABLE_RAF))
        return;

    uint32 l_EventType = p_Event.read<uint32>();

    RecruitAFriendMgrEvent l_Event(l_EventType);
    if (!l_EventType || l_EventType > eRecruitAFriendMgrEvents::EventMax)
        return;

    ///sLog->outAshran("RAF--=============================");
    ///sLog->outAshran("RAF--RecruitAFriendMgr::AddEvent(ByteBuffer)");
    ///sLog->outAshran("RAF--EventType : %lu", l_EventType);

    for (uint32 l_Parameter : RAF::k_RafEventsStructs[l_EventType])
    {
        switch (l_Parameter)
        {
            case eParameters::ParamGUID:
            {
                l_Event.GUID = p_Event.read<uint64>();
                ///sLog->outAshran("RAF--GUID : %llu", l_Event.GUID);
                break;
            }
            case eParameters::ParamGUID2:
            {
                l_Event.GUID2 = p_Event.read<uint64>();
                ///sLog->outAshran("RAF--GUID 2 : %llu", l_Event.GUID2);
                break;
            }
            case eParameters::ParamUInt:
            {
                l_Event.UInt = p_Event.read<uint32>();
                ///sLog->outAshran("RAF--Uint : %llu", l_Event.UInt);
                break;
            }
            case eParameters::ParamApply:
            {
                l_Event.Apply = p_Event.read<uint32>();
                ///sLog->outAshran("RAF--Apply : %c", l_Event.Apply ? 't' : 'f');
                break;
            }
            case eParameters::ParamString:
            {
                l_Event.String = p_Event.ReadString(p_Event.read<uint8>());
                ///sLog->outAshran("RAF--String : %s", l_Event.String.c_str());
                break;
            }
            case eParameters::ParamPtr:
            {
                l_Event.Ptr = reinterpret_cast<void*>(p_Event.read<std::uintptr_t>());
                ///sLog->outAshran("RAF--Ptr : %llu", l_Event.Ptr);
                break;
            }
            default:
                return;
        }
    }

    ///sLog->outAshran("RAF--=============================");
    m_Events.add(l_Event);
}

void RecruitAFriendMgr::Update(uint32 const p_Diff)
{
    RecruitAFriendMgrEvent l_Event(eRecruitAFriendMgrEvents::EventNone);
    while (m_Events.next(l_Event))
    {
        switch (l_Event.EventType)
        {
            case eRecruitAFriendMgrEvents::EventStartRequest:
                StartRequest(l_Event.GUID, l_Event.String);
                break;
            case eRecruitAFriendMgrEvents::EventAppendNote:
                AppendNote(l_Event.GUID, l_Event.String);
                break;
            case eRecruitAFriendMgrEvents::EventEndRequest:
                EndRequest(l_Event.GUID);
                break;
            case eRecruitAFriendMgrEvents::EventSelectCharacter:
                SelectCharacter(l_Event.GUID);
                break;
            case eRecruitAFriendMgrEvents::EventPlayerLogin:
                PlayerLogin(l_Event.GUID);
                break;
            case eRecruitAFriendMgrEvents::EventLoadAccount:
            {
                ///sLog->outAshran("RAFUpdate -> EventLoadAccount Account %llu, PTR %llu", l_Event.UInt, l_Event.Ptr);
                LoadAccount(l_Event.UInt, reinterpret_cast<RecruitAFriendData*>(l_Event.Ptr));
                break;
            }
            case eRecruitAFriendMgrEvents::EventLoadBonuses:
            {
                ///sLog->outAshran("RAFUpdate -> EventLoadBonuses GUID %llu, PTR %llu", l_Event.GUID, l_Event.Ptr);
                LoadBonuses(l_Event.GUID, reinterpret_cast<RecruitAFriendPlayer::RAFBonuses*>(l_Event.Ptr));
                break;
            }
            case eRecruitAFriendMgrEvents::EventLootLegendary:
                LootLegendary(l_Event.GUID);
                break;
            case eRecruitAFriendMgrEvents::EventSelectReward:
                SelectReward(l_Event.GUID, l_Event.UInt);
                break;
            case eRecruitAFriendMgrEvents::EventPlayerLogout:
                PlayerLogout(l_Event.GUID, l_Event.String);
                break;
            case eRecruitAFriendMgrEvents::EventSetInGroup:
                SetInGroup(l_Event.GUID, l_Event.GUID2, l_Event.Apply);
                break;
            case eRecruitAFriendMgrEvents::EventPlayerLevelMax:
                PlayerLevelMax(l_Event.GUID);
                break;
            case eRecruitAFriendMgrEvents::EventAddRewardAvailable:
                AddRewardAvailable(l_Event.GUID, l_Event.GUID2);
                break;
            case eRecruitAFriendMgrEvents::EventSetNotificationsEnabled:
                SetNotificationsEnabled(l_Event.GUID, l_Event.Apply);
                break;
            case eRecruitAFriendMgrEvents::EventSummonRAFFriend:
                SummonRAFFriend(l_Event.GUID, l_Event.GUID2);
                break;
            default:
                break;
        }
    }

    m_EventPacketsLock.lock();
#ifndef CROSS
    if (m_EventPacket.EventCount)
    {
        WorldPacket l_Packet(IR_CMSG_RAF_EVENT, 4 + 18 * m_EventPacket.EventCount);
        l_Packet << m_EventPacket.EventCount;
        l_Packet.append(m_EventPacket.Events);

        if (InterRealmSession* l_InterRealmSession = sWorld->GetInterRealmSession())
            l_InterRealmSession->SendPacket(&l_Packet);
    }

    m_EventPacket.Events.clear();
    m_EventPacket.EventCount = 0;
#else
    for (auto const& l_Pair : m_EventPackets)
    {
        WorldPacket l_Packet(IR_SMSG_RAF_EVENT, 4 + 18 * l_Pair.second.EventCount);
        l_Packet << l_Pair.second.EventCount;
        l_Packet.append(l_Pair.second.Events);

        if (!l_Pair.first)
        {
            sInterRealmMgr->SendPacketToAll(&l_Packet);
            continue;
        }

        InterRealmClient* l_IRClient = sInterRealmMgr->GetClientByRealmNumber(l_Pair.first);
        if (!l_IRClient)
            continue;

        l_IRClient->SendPacket(&l_Packet);
    }

    m_EventPackets.clear();
#endif // !CROSS
    m_EventPacketsLock.unlock();

    if (!m_Updates.empty())
    {
        uint32 l_GameTime = sWorld->GetGameTime();
        while (m_Updates.front().first < l_GameTime)
        {
            uint64 p_PlayerGUID = m_Updates.front().second;
            m_Updates.pop();

            if (m_Players.find(p_PlayerGUID) != m_Players.end())
                UpdateDistances(p_PlayerGUID);

            m_Updates.push(std::make_pair(l_GameTime + RAF_DISTANCE_UPDATE_DELAY, p_PlayerGUID));
        }
    }

    m_RefreshMailCacheTimer += p_Diff;
    if (m_RefreshMailCacheTimer > RAF_MAIL_CACHE_REFRESH_DELAY)
    {
        PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_RAF_REQUEST_MAILS);
        LoginDatabase.AsyncQuery(l_Statement, sWorld->GetQueryCallbackMgr(), [](PreparedQueryResult p_Result) -> void
        {
            if (p_Result)
            {
                std::set<std::string> l_NewMailList;
                uint32 l_Count = 0;
                do
                {
                    Field* l_Fields = p_Result->Fetch();
                    l_NewMailList.insert(l_Fields[0].GetString());

                    l_Count++;
                } while (p_Result->NextRow());

                sRecruitAFriendMgr->UpdateRequestMails(l_NewMailList);
            }
        });

        m_RefreshMailCacheTimer = 0;
    }

    if (!m_TempMailList.empty())
    {
        m_Mails = m_TempMailList;
        m_TempMailList.clear();
    }
}

void RecruitAFriendMgr::StartRequest(uint64 p_PlayerGUID, std::string p_TargetMail)
{
    if (m_Mails.find(p_TargetMail) != m_Mails.end())
        return;

    m_Requests[p_PlayerGUID] = RecruitAFriendRequest(p_TargetMail);
}

void RecruitAFriendMgr::AppendNote(uint64 p_PlayerGUID, std::string p_Note)
{
    if (m_Requests.find(p_PlayerGUID) == m_Requests.end())
        return;

    m_Requests[p_PlayerGUID].Note += p_Note;
}

void RecruitAFriendMgr::EndRequest(uint64 p_PlayerGUID)
{
    if (m_Requests.find(p_PlayerGUID) == m_Requests.end())
        return;

    if (m_Mails.find(m_Requests[p_PlayerGUID].Mail) != m_Mails.end())
        return;

    Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_PlayerGUID);
    if (!l_Player || !l_Player->GetSession())
        return;

    uint8 l_Index = 0;
    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_INS_RAF_REQUEST);
    l_Statement->setString(l_Index++, m_Requests[p_PlayerGUID].Mail);           ///< mail
    l_Statement->setString(l_Index++, l_Player->GetName());                     ///< refererName
    l_Statement->setUInt8(l_Index++, l_Player->GetTeamId());                    ///< faction
    l_Statement->setString(l_Index++, m_Requests[p_PlayerGUID].Note);           ///< message
    l_Statement->setUInt32(l_Index++, l_Player->GetSession()->GetAccountId());  ///< refererAccountId
    l_Statement->setUInt32(l_Index++, g_RealmID);                               ///< realmId

    LoginDatabase.Execute(l_Statement);

    m_Mails.insert(m_Requests[p_PlayerGUID].Mail);
    m_Requests.erase(p_PlayerGUID);
}

void RecruitAFriendMgr::SelectCharacter(uint64 p_PlayerGUID)
{
#ifdef CROSS
    return;
#endif // CROSS
    Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_PlayerGUID);
    if (!l_Player || !l_Player->GetSession())
        return;

    uint32 l_AccountId = l_Player->GetSession()->GetAccountId();

    auto l_Itr = m_RecruitXData.find(l_AccountId);
    if (l_Itr == m_RecruitXData.end())
        return;

    uint32 l_Now = sWorld->GetGameTime();

    l_Itr->second->RecruitCharacterGUID = p_PlayerGUID;
    l_Itr->second->StartTime = l_Now;
    m_RecruitCharXData[p_PlayerGUID] = l_Itr->second;

    uint8 l_Index = 0;
    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_UPD_RAF_CHARACTER);
    l_Statement->setUInt64(l_Index++, GUID_LOPART(p_PlayerGUID));    ///< recruitCharacterGuid
    l_Statement->setUInt32(l_Index++, l_Now);           ///< startTime
    l_Statement->setUInt32(l_Index++, l_AccountId);     ///< recruitAccountId

    LoginDatabase.Execute(l_Statement);
}

void RecruitAFriendMgr::PlayerLogin(uint64 p_PlayerGUID)
{
    Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_PlayerGUID);
    if (!l_Player || !l_Player->GetSession())
        return;

    std::vector<std::string> l_RAFGeneratedFunctions = GetRewardDataForPlayer(l_Player);
    std::vector<std::string> l_String;
    l_String.push_back("");
    for (std::string const& l_Function : l_RAFGeneratedFunctions)
    {
        l_String[0] = l_Function;
        l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
    }

    l_String[0] = "RedrawRewards()";
    l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);

    uint32 l_AccountId = l_Player->GetSession()->GetAccountId();

    m_Players[p_PlayerGUID] = RecruitAFriendPlayer(p_PlayerGUID);
    m_Updates.push(std::make_pair(sWorld->GetGameTime() + RAF_DISTANCE_UPDATE_DELAY, p_PlayerGUID));

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_RAF_ACC_BONUSES);
    l_Statement->setUInt32(0, l_AccountId);

    LoginDatabase.AsyncQuery(l_Statement, sWorld->GetQueryCallbackMgr(), [p_PlayerGUID](PreparedQueryResult p_Result) -> void
    {
        if (p_Result)
        {
            do
            {
                Field* l_Fields = p_Result->Fetch();
                uint32 l_Index = 0;

                RecruitAFriendPlayer::RAFBonuses* l_Bonuses = new RecruitAFriendPlayer::RAFBonuses();
                l_Bonuses->Legendary   = l_Fields[l_Index++].GetBool();
                l_Bonuses->RewardMask  = l_Fields[l_Index++].GetUInt32();
                l_Bonuses->RewardAvailable = l_Fields[l_Index++].GetUInt32();
                l_Bonuses->NotificationsEnabled = l_Fields[l_Index++].GetBool();
                l_Bonuses->Initialized = true;

                sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventLoadBonuses, p_PlayerGUID, l_Bonuses);
            } while (p_Result->NextRow());
        }
        else
        {
            RecruitAFriendPlayer::RAFBonuses* l_Bonuses = new RecruitAFriendPlayer::RAFBonuses();
            l_Bonuses->Legendary = false;
            l_Bonuses->RewardMask = 0;
            l_Bonuses->RewardAvailable = 0;
            l_Bonuses->Initialized = true;
            l_Bonuses->NotificationsEnabled = true;

            sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventLoadBonuses, p_PlayerGUID, l_Bonuses);
        }
    });
}

void RecruitAFriendMgr::LoadAccount(uint32 p_AccountID, RecruitAFriendData* p_Datas)
{
    RecruitAFriendData* l_Data = nullptr;
    auto l_RecruitItr = m_RecruitXData.find(p_AccountID);
    if (l_RecruitItr != m_RecruitXData.end())
    {
        if (l_RecruitItr->second->RecruitAccountID == p_Datas->RecruitAccountID)
        {
            delete p_Datas;
            l_Data = l_RecruitItr->second;
        }
    }

    if (!l_Data)
    {
        auto l_RefererItr = m_RefererXData.find(p_AccountID);
        if (l_RefererItr != m_RefererXData.end())
        {
            for (RecruitAFriendData* l_DataItr : l_RefererItr->second)
            {
                if (l_DataItr->RecruitAccountID == p_Datas->RecruitAccountID)
                {
                    delete p_Datas;
                    l_Data = l_DataItr;
                    break;
                }
            }
        }
    }

    if (!l_Data)
        l_Data = p_Datas;

    LoadAccountData(p_AccountID, l_Data);

    if (l_Data->RefererLoggedIn && l_Data->RecruitLoggedIn)
    {
        Player* l_Recruit = sObjectAccessor->FindPlayerInOrOutOfWorld(l_Data->RecruitCharacterGUID);
        Player* l_Referer = sObjectAccessor->FindPlayerInOrOutOfWorld(l_Data->RefererCharacterGUID);
        if (l_Recruit && l_Referer)
        {
            std::vector<std::string> l_StringVect = { l_Recruit->GetName() };

            if (m_Players.find(l_Referer->GetGUID()) != m_Players.end())
            {
                if (m_Players[l_Referer->GetGUID()].Bonuses.NotificationsEnabled)
                    l_Referer->SendCustomMessage("FSC_RAF_RECRUIT_LOGGEDIN", l_StringVect);
            }

            l_StringVect = { l_Referer->GetName() };

            if (m_Players.find(l_Recruit->GetGUID()) != m_Players.end())
            {
                if (m_Players[l_Recruit->GetGUID()].Bonuses.NotificationsEnabled)
                    l_Recruit->SendCustomMessage("FSC_RAF_REFERER_LOGGEDIN", l_StringVect);
            }
        }
    }

    /// In Case already in group and near to each other
    UpdateRAFState(l_Data);
}

void RecruitAFriendMgr::LoadAccountData(uint32 p_AccountID, RecruitAFriendData* p_Datas)
{
    if (p_Datas->RefererAccountID == p_AccountID)
    {
        m_RefererXData[p_AccountID].insert(p_Datas);
        m_RecruitXData[p_Datas->RecruitAccountID] = p_Datas;

#ifdef CROSS
        WorldSession* l_Session = sWorld->FindCrossSession(p_AccountID);
#else
        WorldSession* l_Session = sWorld->FindSession(p_AccountID);
#endif // CROSS

        if (!l_Session)
            return;

        Player* l_Player = l_Session->GetPlayer();
        if (!l_Player)
            return;

        p_Datas->RefererCharacterGUID = l_Player->GetGUID();
        InitGroup(p_Datas, l_Player);

        m_RefererCharXData[p_Datas->RefererCharacterGUID].insert(p_Datas);

        p_Datas->RefererLoggedIn = true;
    }
    else if (p_Datas->RecruitAccountID == p_AccountID)
    {
        m_RecruitXData[p_AccountID] = p_Datas;
        m_RefererXData[p_Datas->RefererAccountID].insert(p_Datas);

        if (!p_Datas->RecruitCharacterGUID)
        {
#ifdef CROSS
            WorldSession* l_Session = sWorld->FindCrossSession(p_AccountID);
#else
            WorldSession* l_Session = sWorld->FindSession(p_AccountID);
#endif // CROSS
            if (!l_Session)
                return;

            Player* l_Player = l_Session->GetPlayer();
            if (!l_Player)
                return;

            l_Player->SendCustomMessage("FSC_RAF_AVAILABLE");
        }
        else
        {
#ifdef CROSS
            WorldSession* l_Session = sWorld->FindCrossSession(p_AccountID);
#else
            WorldSession* l_Session = sWorld->FindSession(p_AccountID);
#endif // CROSS
            if (!l_Session)
                return;

            Player* l_Player = l_Session->GetPlayer();
            if (!l_Player || l_Player->GetGUIDLow() != GUID_LOPART(p_Datas->RecruitCharacterGUID))
                return;

            p_Datas->RecruitCharacterGUID = l_Player->GetGUID();
            InitGroup(p_Datas, l_Player);

            m_RecruitCharXData[p_Datas->RecruitCharacterGUID] = p_Datas;
        }

        p_Datas->RecruitLoggedIn = true;
    }
}

void RecruitAFriendMgr::LoadBonuses(uint64 p_PlayerGUID, RecruitAFriendPlayer::RAFBonuses* p_Bonuses)
{
    Player* l_Player = nullptr;
    if (m_Players.find(p_PlayerGUID) != m_Players.end())
    {
        m_Players[p_PlayerGUID].Bonuses = *p_Bonuses;
        if ((l_Player = sObjectAccessor->FindPlayer(p_PlayerGUID)))
        {
            std::vector<std::string> l_String;
            l_String.push_back("");

            std::vector<std::string> l_RAFGeneratedFunctions = GetRewardDataForPlayer(l_Player);

            for (std::string const& l_Function : l_RAFGeneratedFunctions)
            {
                l_String[0] = l_Function;
                l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
            }

            l_String[0] = "RedrawRewards()";
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);

            l_String[0] = "SetRewardCount(" + std::to_string(m_Players[p_PlayerGUID].Bonuses.RewardAvailable) + ")";
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);

            std::string l_Enabled = m_Players[p_PlayerGUID].Bonuses.NotificationsEnabled ? "true" : "false";
            l_String[0] = "SetNotificationsEnabled(" + l_Enabled + ")";
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
        }
    }

    delete p_Bonuses;

    if (!l_Player)
        l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_PlayerGUID);

    if (!l_Player)
        return;

    uint32 l_AccountId = l_Player->GetSession()->GetAccountId();

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_RAF_ACC);
    l_Statement->setUInt32(0, l_AccountId);     ///< refererAccountId
    l_Statement->setUInt32(1, l_AccountId);     ///< recruitAccountId

#ifndef CROSS
    uint32 l_RealmID = g_RealmID;
#else
    uint32 l_RealmID = l_Player->GetSession()->GetInterRealmNumber();
#endif // !CROSS

    LoginDatabase.AsyncQuery(l_Statement, sWorld->GetQueryCallbackMgr(), [l_AccountId, l_RealmID](PreparedQueryResult p_Result) -> void
    {
        if (p_Result)
        {
            do
            {
                Field* l_Fields = p_Result->Fetch();
                uint32 l_Index = 0;

                if (l_RealmID != l_Fields[l_Index++].GetUInt32())
                    continue;

                RecruitAFriendData* l_Data = new RecruitAFriendData();
                l_Data->RefererAccountID = l_Fields[l_Index++].GetUInt32();
                l_Data->RecruitAccountID = l_Fields[l_Index++].GetUInt32();
                l_Data->Faction = l_Fields[l_Index++].GetUInt8();
                l_Data->RecruitCharacterGUID = l_Fields[l_Index++].GetUInt64();
                l_Data->StartTime = l_Fields[l_Index++].GetUInt32();

                sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventLoadAccount, l_AccountId, l_Data);
            } while (p_Result->NextRow());
        }
    });
}

void RecruitAFriendMgr::PlayerLogout(uint64 p_PlayerGUID, std::string p_PlayerName)
{
    auto l_RecruitItr = m_RecruitCharXData.find(p_PlayerGUID);
    if (l_RecruitItr != m_RecruitCharXData.end())
    {
        l_RecruitItr->second->RecruitLoggedIn = false;

        if (m_Players.find(l_RecruitItr->second->RefererCharacterGUID) != m_Players.end())
        {
            if (m_Players[l_RecruitItr->second->RefererCharacterGUID].Bonuses.NotificationsEnabled)
            {
                Player* l_Referer = sObjectAccessor->FindPlayerInOrOutOfWorld(l_RecruitItr->second->RefererCharacterGUID);
                if (l_Referer)
                {
                    std::vector<std::string> l_PlayerName = { p_PlayerName };
                    l_Referer->SendCustomMessage("FSC_RAF_RECRUIT_LOGGEDOUT", l_PlayerName);
                }
            }
        }

        UpdateRAFState(l_RecruitItr->second);

        if (!l_RecruitItr->second->RefererLoggedIn)
            DeleteReferencesAndData(l_RecruitItr->second);
        else
            m_RecruitCharXData.erase(l_RecruitItr);
    }

    auto l_RefererItr = m_RefererCharXData.find(p_PlayerGUID);
    if (l_RefererItr != m_RefererCharXData.end())
    {
        uint32 l_DeletedData = 0;
        std::set<RecruitAFriendData*> l_Datas = l_RefererItr->second;
        for (RecruitAFriendData* l_Data : l_Datas)
        {
            l_Data->RefererLoggedIn = false;
            UpdateRAFState(l_Data);

            if (!l_Data->RecruitLoggedIn)
            {
                DeleteReferencesAndData(l_Data);
                l_DeletedData++;
                continue;
            }

            l_Data->RefererCharacterGUID = 0;
        }

        if (!l_DeletedData || l_DeletedData != l_Datas.size())
            m_RefererCharXData.erase(l_RefererItr);
    }

    m_Players.erase(p_PlayerGUID);
}

void RecruitAFriendMgr::LootLegendary(uint64 p_PlayerGUID)
{
    if (m_Players.find(p_PlayerGUID) == m_Players.end())
        return;

    std::vector<uint64> l_NearPlayerGUIDs;
    if (m_Players[p_PlayerGUID].Referer)
        l_NearPlayerGUIDs.push_back(m_Players[p_PlayerGUID].Referer);

    for (uint64 l_PlayerGUID : m_Players[p_PlayerGUID].Recruits)
        l_NearPlayerGUIDs.push_back(l_PlayerGUID);

    for (uint64 l_PlayerGUID : l_NearPlayerGUIDs)
    {
        if (m_Players.find(l_PlayerGUID) == m_Players.end())
            continue;

        if (!m_Players[l_PlayerGUID].Bonuses.Initialized || m_Players[l_PlayerGUID].Bonuses.Legendary)
            continue;

        Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGUID);
        if (!l_Player)
            continue;

        l_Player->AddCriticalOperation([l_Player]() -> void
        {
            l_Player->CompleteLegendaryActivity(LegendaryActivity::RecruitAFriend);
        });

        m_Players[l_PlayerGUID].Bonuses.Legendary = true;
        m_Players[l_PlayerGUID].SaveBonuses(l_Player);
    }
}

void RecruitAFriendMgr::SelectReward(uint64 p_PlayerGUID, uint32 p_RewardID)
{
    if (m_Rewards.find(p_RewardID) == m_Rewards.end())
        return;

    Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_PlayerGUID);
    if (!l_Player)
        return;

    RecruitAFriendReward const& l_Reward = m_Rewards[p_RewardID];

    bool l_Available = CanHaveReward(p_PlayerGUID, l_Reward);
    if (!l_Available || !m_Players[p_PlayerGUID].Bonuses.RewardAvailable)
        return;

    m_Players[p_PlayerGUID].Bonuses.RewardAvailable--;
    if (l_Reward.Unique)
        m_Players[p_PlayerGUID].Bonuses.RewardMask |= (1 << l_Reward.Index);

    m_Players[p_PlayerGUID].SaveBonuses(l_Player);

    if (!l_Player->IsInWorld())
        return;

    l_Player->AddItem(m_Rewards[p_RewardID].ItemID, m_Rewards[p_RewardID].Quantity, {}, false, 0, false, true);

    std::vector<std::string> l_String;
    l_String.push_back("");

    std::vector<std::string> l_RAFGeneratedFunctions = GetRewardDataForPlayer(l_Player);

    for (std::string const& l_Function : l_RAFGeneratedFunctions)
    {
        l_String[0] = l_Function;
        l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
    }

    l_String[0] = "RedrawRewards()";
    l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);

    l_String[0] = "SetRewardCount(" + std::to_string(m_Players[p_PlayerGUID].Bonuses.RewardAvailable) + ")";
    l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
}

void RecruitAFriendMgr::InitGroup(RecruitAFriendData* p_Datas, Player* p_Player)
{
    if (GroupPtr l_Group = p_Player->GetGroup())
    {
        l_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Slot) -> void
        {
            if (l_Slot->guid == p_Player->GetGUID())
                return;

            if (l_Slot->guid == p_Datas->RecruitCharacterGUID || l_Slot->guid == p_Datas->RefererCharacterGUID)
                p_Datas->InSameGroup = true;
        });
    }
}

void RecruitAFriendMgr::SetInGroup(uint64 p_GUID1, uint64 p_GUID2, bool p_Apply)
{
    if (p_GUID1 == p_GUID2)
        return;

    /// Check if p_GUID1 is recruit of p_GUID2
    auto l_Itr = m_RecruitCharXData.find(p_GUID1);
    if (l_Itr != m_RecruitCharXData.end())
    {
        if (l_Itr->second->RefererCharacterGUID == p_GUID2)
        {
            l_Itr->second->InSameGroup = p_Apply;
            UpdateRAFState(l_Itr->second);
        }
    }

    /// Check if p_GUID2 is recruit of p_GUID1
    l_Itr = m_RecruitCharXData.find(p_GUID2);
    if (l_Itr != m_RecruitCharXData.end())
    {
        if (l_Itr->second->RefererCharacterGUID == p_GUID1)
        {
            l_Itr->second->InSameGroup = p_Apply;
            UpdateRAFState(l_Itr->second);
        }
    }
}

void RecruitAFriendMgr::UpdateRAFState(RecruitAFriendData* p_Data)
{
    bool l_Active = p_Data->RefererLoggedIn && p_Data->RecruitLoggedIn && p_Data->InSameGroup && p_Data->InRange;
    if (l_Active == p_Data->Active)
        return;

    m_Players[p_Data->RecruitCharacterGUID].SetWithReferer(p_Data->RefererCharacterGUID, l_Active);
    m_Players[p_Data->RefererCharacterGUID].SetWithRecruit(p_Data->RecruitCharacterGUID, l_Active);

    p_Data->Active = l_Active;
}

void RecruitAFriendMgr::RecruitAFriendPlayer::SetWithRecruit(uint64 p_RecruitGUID, bool p_Apply)
{
    if (p_Apply)
        Recruits.insert(p_RecruitGUID);
    else
        Recruits.erase(p_RecruitGUID);

    if (Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(Guid))
    {
        bool l_Activate = !(Recruits.empty());
        l_Player->AddCriticalOperation([l_Player, l_Activate]() -> void
        {
            if (l_Activate && !l_Player->HasAura(eRAFSpells::RAFCustomAura))
                l_Player->AddAura(eRAFSpells::RAFCustomAura, l_Player);

            if (!l_Activate)
                l_Player->RemoveAura(eRAFSpells::RAFCustomAura);
        });
    }
}

void RecruitAFriendMgr::RecruitAFriendPlayer::SetWithReferer(uint64 p_RefererGUID, bool p_Apply)
{
    Referer = p_Apply ? p_RefererGUID : 0;

    if (Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(Guid))
    {
        l_Player->AddCriticalOperation([l_Player, p_Apply]() -> void
        {
            if (p_Apply && !l_Player->HasAura(eRAFSpells::RAFCustomAura))
                l_Player->AddAura(eRAFSpells::RAFCustomAura, l_Player);

            if (!p_Apply)
                l_Player->RemoveAura(eRAFSpells::RAFCustomAura);
        });
    }
}

void RecruitAFriendMgr::UpdateDistances(uint64 p_PlayerGUID)
{
    Player* l_Player = sObjectAccessor->FindPlayer(p_PlayerGUID);
    if (!l_Player)
        return;

    for (RecruitAFriendData* l_Data : m_RefererCharXData[p_PlayerGUID])
    {
        Player* l_Recruit = sObjectAccessor->GetPlayer(*l_Player, l_Data->RecruitCharacterGUID);
        bool l_InRange = l_Recruit && l_Recruit->IsWithinDistInMap(l_Player, RAF_MAX_ACTIVE_DISTANCE);

        l_Data->InRange = l_InRange;
        UpdateRAFState(l_Data);
    }
}

std::vector<std::string> RecruitAFriendMgr::GetRewardDataForPlayer(Player* p_Player)
{
    std::vector<std::string> l_Functions;
    std::string l_MasterFunction = "InitRewards = function() ";

    LocaleConstant l_Locale = p_Player->GetSession()->GetSessionDbLocaleIndex();

    for (std::pair<uint8, RecruitAFriendReward>const& l_Pair : m_Rewards)
    {
        l_MasterFunction += "RAFReward_";
        l_MasterFunction += std::to_string(l_Pair.first);
        l_MasterFunction += "() ";
        l_Functions.push_back(PrepareRafRewardFunction(l_Pair.second, p_Player->GetGUID(), l_Locale));
    }

    l_MasterFunction += "end";
    l_Functions.push_back(l_MasterFunction);

    return l_Functions;
}

bool RecruitAFriendMgr::CanHaveReward(uint64 p_PlayerGUID, RecruitAFriendReward const& p_Reward)
{
    if (m_Players.find(p_PlayerGUID) == m_Players.end())
        return false;

    if (!m_Players[p_PlayerGUID].Bonuses.RewardAvailable)
        return false;

    bool l_Owned = m_Players[p_PlayerGUID].Bonuses.Initialized && (m_Players[p_PlayerGUID].Bonuses.RewardMask & (1 << p_Reward.Index));
    if (p_Reward.Unique && l_Owned)
        return false;

    if (p_Reward.Flags & (eRewardFlags::RafRewardAlliance | eRewardFlags::RafRewardHorde))
    {
        Player* l_Player = sObjectAccessor->FindPlayer(p_PlayerGUID);
        if (!l_Player)
            return false;

        if ((p_Reward.Flags & eRewardFlags::RafRewardAlliance) && l_Player->GetTeamId() != TEAM_ALLIANCE)
            return false;

        if ((p_Reward.Flags & eRewardFlags::RafRewardHorde) && l_Player->GetTeamId() != TEAM_HORDE)
            return false;
    }

    return true;
}

std::string RecruitAFriendMgr::PrepareRafRewardFunction(RecruitAFriendReward const& p_Reward, uint64 p_PlayerGUID, LocaleConstant p_Locale)
{
    std::string l_Name;
    std::string l_Subtitle;

    auto l_Itr = p_Reward.LocalizedName.find(p_Locale);
    if (l_Itr != p_Reward.LocalizedName.end())
        l_Name = l_Itr->second;
    else if ((l_Itr = p_Reward.LocalizedName.find(LocaleConstant::LOCALE_enUS)) != p_Reward.LocalizedName.end())
        l_Name = l_Itr->second;

    l_Itr = p_Reward.LocalizedSubtitle.find(p_Locale);
    if (l_Itr != p_Reward.LocalizedSubtitle.end())
        l_Subtitle = l_Itr->second;
    else if ((l_Itr = p_Reward.LocalizedSubtitle.find(LocaleConstant::LOCALE_enUS)) != p_Reward.LocalizedSubtitle.end())
        l_Subtitle = l_Itr->second;

    bool l_Owned = m_Players[p_PlayerGUID].Bonuses.Initialized && (m_Players[p_PlayerGUID].Bonuses.RewardMask & (1 << p_Reward.Index));
    bool l_Disabled = !CanHaveReward(p_PlayerGUID, p_Reward);

    std::string l_Function = "RAFReward_" + std::to_string(p_Reward.Index);
    l_Function += " = function() local data = {} data.name = \"";
    l_Function += l_Name;
    l_Function += "\" data.subtitle = \"";
    l_Function += l_Subtitle;
    l_Function += "\" data.modelDisplayID = ";
    l_Function += std::to_string(p_Reward.ModelDisplayID);
    l_Function += " data.alreadyHas = ";
    l_Function += l_Owned ? "true" : "false";
    l_Function += " data.disabled = ";
    l_Function += l_Disabled ? "true" : "false";
    l_Function += " FS_RAF_PRODUCTS[";
    l_Function += std::to_string(p_Reward.Index);
    l_Function += "] = data end";

    return l_Function;
}

void RecruitAFriendMgr::PlayerLevelMax(uint64 p_PlayerGUID)
{
    if (m_RecruitCharXData.find(p_PlayerGUID) == m_RecruitCharXData.end())
        return;

    AddEvent(eRecruitAFriendMgrEvents::EventAddRewardAvailable, m_RecruitCharXData[p_PlayerGUID]->RefererCharacterGUID, p_PlayerGUID);
}

void RecruitAFriendMgr::AddRewardAvailable(uint64 p_RefererGUID, uint64 p_RecruitGUID)
{
    Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_RefererGUID);
    if (!l_Player)
    {
#ifndef CROSS
        PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_UPD_RAF_ACC_BONUSES_REWARD);
        l_Statement->setUInt64(0, GUID_LOPART(p_RecruitGUID));

        LoginDatabase.Execute(l_Statement);
#endif // !CROSS
        return;
    }

    m_Players[p_RefererGUID].Bonuses.RewardAvailable++;
    m_Players[p_RefererGUID].SaveBonuses(l_Player);

    std::vector<std::string> l_String;
    l_String.push_back("");

    std::vector<std::string> l_RAFGeneratedFunctions = GetRewardDataForPlayer(l_Player);

    for (std::string const& l_Function : l_RAFGeneratedFunctions)
    {
        l_String[0] = l_Function;
        l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
    }

    l_String[0] = "RedrawRewards()";
    l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);

    l_String[0] = "SetRewardCount(" + std::to_string(m_Players[p_RefererGUID].Bonuses.RewardAvailable) + ")";
    l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
}

void RecruitAFriendMgr::SetNotificationsEnabled(uint64 p_PlayerGUID, bool p_Apply)
{
    if (m_Players.find(p_PlayerGUID) == m_Players.end())
        return;

    Player* l_Player = sObjectAccessor->FindPlayerInOrOutOfWorld(p_PlayerGUID);
    if (!l_Player)
        return;

    m_Players[p_PlayerGUID].Bonuses.NotificationsEnabled = p_Apply;
    m_Players[p_PlayerGUID].SaveBonuses(l_Player);
}

void RecruitAFriendMgr::SummonRAFFriend(uint64 p_SummonerGUID, uint64 p_TargetGUID)
{
    RecruitAFriendData* l_Data = nullptr;
    if (m_RecruitCharXData.find(p_SummonerGUID) != m_RecruitCharXData.end())
    {
        if (m_RecruitCharXData[p_SummonerGUID]->RefererCharacterGUID == p_TargetGUID)
            l_Data = m_RecruitCharXData[p_SummonerGUID];
    }

    if (m_RecruitCharXData.find(p_TargetGUID) != m_RecruitCharXData.end())
    {
        if (m_RecruitCharXData[p_TargetGUID]->RefererCharacterGUID == p_SummonerGUID)
            l_Data = m_RecruitCharXData[p_TargetGUID];
    }

    if (!l_Data)
        return;

    Player* l_Summoner = sObjectAccessor->FindPlayer(p_SummonerGUID);
    if (!l_Summoner)
        return;

    l_Summoner->AddCriticalOperation([l_Summoner, p_TargetGUID]() -> void
    {
        Player* l_Target = sObjectAccessor->FindPlayer(p_TargetGUID);
        if (!l_Target)
            return;

        l_Summoner->m_SpellHelper.GetUint64(eSpellHelpers::RAFSummonTarget) = p_TargetGUID;
        l_Summoner->CastSpell(l_Target, eRAFSpells::RAFSummoning, false);
    });
}

bool RecruitAFriendMgr::AreRAFFriends(uint64 p_GUID1, uint64 p_GUID2)
{
    auto l_Itr = m_RecruitCharXData.find(p_GUID1);
    if (m_RecruitCharXData.find(p_GUID1) != m_RecruitCharXData.end())
    {
        if (m_RecruitCharXData[p_GUID1]->RefererCharacterGUID == p_GUID2)
            return true;
    }

    if (m_RecruitCharXData.find(p_GUID2) != m_RecruitCharXData.end())
    {
        if (m_RecruitCharXData[p_GUID2]->RefererCharacterGUID == p_GUID1)
            return true;
    }

    return false;
}
