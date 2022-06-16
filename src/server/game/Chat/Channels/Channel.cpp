////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include <regex>

#include "Channel.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "SocialMgr.h"
#include "World.h"
#include "DatabaseEnv.h"
#include "AccountMgr.h"
#include "WordFilterMgr.h"

Channel::Channel(const std::string& name, uint32 channel_id, uint32 Team)
 : m_announce(true), _special(false), m_ownership(true), m_name(name), m_password(""), m_flags(0), m_channelId(channel_id), m_ownerGUID(0), m_Team(Team), m_SlowModeInterval(0), m_LockDown(false)
{
    m_IsSaved = false;

    // set special flags if built-in channel
    if (ChatChannelsEntry const* ch = sChatChannelsStore.LookupEntry(channel_id)) // check whether it's a built-in channel
    {
        m_announce = false;                                 // no join/leave announces
        m_ownership = false;                                // no ownership handout

        m_flags |= CHANNEL_FLAG_GENERAL;                    // for all built-in channels

        if (ch->Flags & CHANNEL_DBC_FLAG_TRADE)             // for trade channel
            m_flags |= CHANNEL_FLAG_TRADE;

        if (ch->Flags & CHANNEL_DBC_FLAG_CITY_ONLY2)        // for city only channels
            m_flags |= CHANNEL_FLAG_CITY;

        if (ch->Flags & CHANNEL_DBC_FLAG_LFG)               // for LFG channel
            m_flags |= CHANNEL_FLAG_LFG;
        else                                                // for all other channels
            m_flags |= CHANNEL_FLAG_NOT_LFG;
    }
    else if (IsWorld())
    {
        m_announce = false;
        _special = true;
    }
    else                                                    // it's custom channel
    {
        m_flags |= CHANNEL_FLAG_CUSTOM;

        std::string temp_str(m_name);
        std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), tolower);

        // If storing custom channels in the db is enabled either load or save the channel
        if (sWorld->getBoolConfig(CONFIG_PRESERVE_CUSTOM_CHANNELS) && temp_str.find("world") != std::string::npos)
        {
            PreparedStatement *stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHANNEL);
            stmt->setString(0, name);
            stmt->setUInt32(1, m_Team);

            CharacterDatabase.AsyncQuery(stmt, sWorld->GetQueryCallbackMgr(), [this](PreparedQueryResult const& p_Result)
            {
                if (p_Result) //load
                {
                    Field* fields = p_Result->Fetch();
                    m_announce = fields[0].GetBool();
                    m_ownership = fields[1].GetBool();
                    m_password = fields[2].GetString();
                    const char* db_BannedList = fields[3].GetCString();

                    if (db_BannedList)
                    {
                        Tokenizer tokens(db_BannedList, ' ');
                        Tokenizer::const_iterator iter;
                        for (iter = tokens.begin(); iter != tokens.end(); ++iter)
                        {
                            uint64 banned_guid = atol(*iter);
                            if (banned_guid)
                            {
                                sLog->outDebug(LOG_FILTER_CHATSYS, "Channel(%s) loaded banned guid:" UI64FMTD "", m_name.c_str(), banned_guid);
                                banned.insert(banned_guid);
                            }
                        }
                    }
                }
                else // save
                {
                    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHANNEL);
                    stmt->setString(0, m_name);
                    stmt->setUInt32(1, m_Team);
                    CharacterDatabase.Execute(stmt);
                    sLog->outDebug(LOG_FILTER_CHATSYS, "Channel(%s) saved in database", m_name.c_str());
                }
                m_IsSaved = true;
            });
        }
    }
}

bool Channel::IsWorld() const
{
    std::string lowername;
    uint32 nameLength = m_name.length();
    for (uint32 i = 0; i < nameLength; ++i)
        lowername.push_back(std::towlower(m_name[i]));

    if (lowername == "world" || lowername == "all" || lowername == "english")
        return true;

    return false;
}

bool Channel::IsWorldLocal(LocaleConstant p_LocaleConstant) const
{
    std::string lowername;
    uint32 nameLength = m_name.length();
    for (uint32 i = 0; i < nameLength; ++i)
        lowername.push_back(std::towlower(m_name[i]));

    if (p_LocaleConstant == LocaleConstant::LOCALE_ruRU && lowername == "all")
        return true;
    if (p_LocaleConstant == LocaleConstant::LOCALE_ruRU && lowername == "english")
        return true;
    if (p_LocaleConstant == LocaleConstant::LOCALE_frFR && lowername == "english")
        return true;
    if (p_LocaleConstant == LocaleConstant::LOCALE_enUS && lowername == "english")
        return true;
    if ((p_LocaleConstant == LocaleConstant::LOCALE_esES || p_LocaleConstant == LocaleConstant::LOCALE_esMX) && lowername == "english")
        return true;
    if (p_LocaleConstant == LocaleConstant::LOCALE_ptPT && lowername == "english")
        return true;

    return false;
}

void Channel::UpdateChannelInDB() const
{
    if (m_IsSaved)
    {
        std::ostringstream banlist;
        BannedList::const_iterator iter;
        for (iter = banned.begin(); iter != banned.end(); ++iter)
            banlist << (*iter) << ' ';

        std::string banListStr = banlist.str();

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHANNEL);
        stmt->setBool(0, m_announce);
        stmt->setBool(1, m_ownership);
        stmt->setString(2, m_password);
        stmt->setString(3, banListStr);
        stmt->setString(4, m_name);
        stmt->setUInt32(5, m_Team);
        CharacterDatabase.Execute(stmt);

        sLog->outDebug(LOG_FILTER_CHATSYS, "Channel(%s) updated in database", m_name.c_str());
    }

}

void Channel::UpdateChannelUseageInDB() const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHANNEL_USAGE);
    stmt->setString(0, m_name);
    stmt->setUInt32(1, m_Team);
    CharacterDatabase.Execute(stmt);
}

void Channel::CleanOldChannelsInDB()
{
    if (sWorld->getIntConfig(CONFIG_PRESERVE_CUSTOM_CHANNEL_DURATION) > 0)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_OLD_CHANNELS);
        stmt->setUInt32(0, sWorld->getIntConfig(CONFIG_PRESERVE_CUSTOM_CHANNEL_DURATION) * DAY);
        CharacterDatabase.Execute(stmt);

        sLog->outDebug(LOG_FILTER_CHATSYS, "Cleaned out unused custom chat channels.");
    }
}

/// Update world chat locale filtering for a specific player
/// @p_Player : Player instance to update
void Channel::UpdateChatLocaleFiltering(Player* p_Player)
{
    if (!(IsWorld() || IsConstant()) || !p_Player)
        return;

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (m_Players.find(p_Player->GetGUID()) == m_Players.end())
        return;

    if (p_Player->GetSession()->HasCustomFlags(AccountCustomFlags::NoChatLocaleFiltering))
        m_Players[p_Player->GetGUID()].LocaleFilter = 0xFFFFFFFF;
    else
        m_Players[p_Player->GetGUID()].LocaleFilter = 1 << (p_Player->GetSession()->GetSessionDbLocaleIndex() + 1);
}

void Channel::Join(uint64 p, const char *pass)
{
    WorldPacket data;
    if (IsOn(p))
    {
        if (!IsConstant())                                   // non send error message for built-in channels
        {
            MakePlayerAlreadyMember(&data, p);
            SendToOne(&data, p);
        }
        return;
    }

    if (IsBanned(p))
    {
        MakeBanned(&data);
        SendToOne(&data, p);
        return;
    }

    if (m_password.length() > 0 && strcmp(pass, m_password.c_str()))
    {
        MakeWrongPassword(&data);
        SendToOne(&data, p);
        return;
    }

    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);

    if (player)
    {
        if (HasFlag(CHANNEL_FLAG_LFG) &&
            sWorld->getBoolConfig(CONFIG_RESTRICTED_LFG_CHANNEL) && AccountMgr::IsPlayerAccount(player->GetSession()->GetSecurity()) && player->GetGroup())
        {
            MakeNotInLfg(&data);
            SendToOne(&data, p);
            return;
        }

        player->JoinedChannel(this);
    }

    if (m_announce && (!player || !AccountMgr::IsModeratorAccount(player->GetSession()->GetSecurity()) || !sWorld->getBoolConfig(CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL)))
    {
        MakeJoined(&data, p);
        SendToAll(&data);
    }

    data.clear();

    PlayerInfo pinfo;
    pinfo.player = p;
    pinfo.flags = MEMBER_FLAG_NONE;
    pinfo.LocaleFilter = 0xFFFFFFFF;

    if ((IsWorld() || IsConstant()) && player && player->GetSession() && !player->GetSession()->HasCustomFlags(AccountCustomFlags::NoChatLocaleFiltering))
    {
        pinfo.LocaleFilter = 1 << (player->GetSession()->GetSessionDbLocaleIndex() + 1);
    }

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    m_Players[p] = pinfo;

    MakeYouJoined(&data);
    SendToOne(&data, p);

    if (!sWorld->getBoolConfig(CONFIG_SILENTLY_JOIN_LEAVE_TO_CHANNEL))
        JoinNotify(p);

    // Custom channel handling
    if (!IsConstant())
    {
        // Update last_used timestamp in db
        if (!m_Players.empty())
            UpdateChannelUseageInDB();

        /// Special case for the world channel, players can't be moderator or owner.
        if (m_name == "world" || m_name == "all" || m_name == "english")
        {
            if (player && player->GetSession()->GetSecurity() > AccountTypes::SEC_MODERATOR)
            {
                m_Players[p].SetModerator(true);
                if (!m_ownerGUID && m_ownership)
                    SetOwner(p, (m_Players.size() > 1 ? true : false));
            }
        }
        // If the channel has no owner yet and ownership is allowed, set the new owner.
        else if (!m_ownerGUID && m_ownership)
        {
            SetOwner(p, (m_Players.size() > 1 ? true : false));
            m_Players[p].SetModerator(true);
        }
    }
}

void Channel::Leave(uint64 p, bool send)
{
    if (!IsOn(p))
    {
        if (send)
        {
            WorldPacket data;
            MakeNotMember(&data);
            SendToOne(&data, p);
        }
    }
    else
    {
        Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);

        if (send)
        {
            WorldPacket data;
            MakeYouLeft(&data);
            SendToOne(&data, p);
            if (player)
                player->LeftChannel(this);
            data.clear();
        }

        ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

        bool changeowner = m_Players[p].IsOwner();

        m_Players.erase(p);

        if (m_announce && (!player || !AccountMgr::IsModeratorAccount(player->GetSession()->GetSecurity()) || !sWorld->getBoolConfig(CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL)))
        {
            WorldPacket data;
            MakeLeft(&data, p);
            SendToAll(&data);
        }

        if (!sWorld->getBoolConfig(CONFIG_SILENTLY_JOIN_LEAVE_TO_CHANNEL))
            LeaveNotify(p);

        if (!IsConstant())
        {
            // Update last_used timestamp in db
            UpdateChannelUseageInDB();

            // If the channel owner left and there are still players inside, pick a new owner
            if (changeowner && m_ownership && !m_Players.empty())
            {
                uint64 newowner = m_Players.begin()->second.player;
                m_Players[newowner].SetModerator(true);
                SetOwner(newowner);
            }
        }
    }
}

void Channel::KickOrBan(uint64 good, const char *badname, bool ban)
{
    AccountTypes sec = SEC_PLAYER;
    Player* gplr = ObjectAccessor::FindPlayerInOrOutOfWorld(good);
    if (gplr)
        sec = gplr->GetSession()->GetSecurity();

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (!IsOn(good))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, good);
    }
    else if (!m_Players[good].IsModerator() && !AccountMgr::IsModeratorAccount(sec))
    {
        WorldPacket data;
        MakeNotModerator(&data);
        SendToOne(&data, good);
    }
    else
    {
        Player* bad = sObjectAccessor->FindPlayerByNameInOrOutOfWorld(badname);
        if (bad == NULL || !IsOn(bad->GetGUID()))
        {
            WorldPacket data;
            MakePlayerNotFound(&data, badname);
            SendToOne(&data, good);
        }
        else if (!AccountMgr::IsModeratorAccount(sec) && bad->GetGUID() == m_ownerGUID && good != m_ownerGUID)
        {
            WorldPacket data;
            MakeNotOwner(&data);
            SendToOne(&data, good);
        }
        else
        {
            bool changeowner = (m_ownerGUID == bad->GetGUID());

            WorldPacket data;
            bool notify = !(AccountMgr::IsModeratorAccount(sec) && sWorld->getBoolConfig(CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL));

            if (ban && !IsBanned(bad->GetGUID()))
            {
                banned.insert(bad->GetGUID());
                UpdateChannelInDB();

                if (notify)
                    MakePlayerBanned(&data, bad->GetGUID(), good);

                //sLog->outExtChat("#sylvanas-world-punish", "", false, "*_Channel Ban_*\n*User*: %s\n*Moderator*: %s\n*Channel*: %s", bad->GetName(), gplr->GetName(), GetName().c_str());
            }
            else if (notify)
            {
                MakePlayerKicked(&data, bad->GetGUID(), good);
                //sLog->outExtChat("#sylvanas-world-punish", "", false, "*_Channel Kick_*\n*User*: %s\n*Moderator*: %s\n*Channel*: %s", bad->GetName(), gplr->GetName(), GetName().c_str());
            }

            if (notify)
                SendToAll(&data);

            m_Players.erase(bad->GetGUID());
            bad->LeftChannel(this);

            if (changeowner && m_ownership && !m_Players.empty())
            {
                uint64 newowner = good;
                m_Players[newowner].SetModerator(true);
                SetOwner(newowner);
            }
        }
    }
}

void Channel::UnBan(uint64 good, const char *badname)
{
    uint32 sec = 0;
    Player* gplr = ObjectAccessor::FindPlayerInOrOutOfWorld(good);
    if (gplr)
        sec = gplr->GetSession()->GetSecurity();

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (!IsOn(good))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, good);
    }
    else if (!m_Players[good].IsModerator() && !AccountMgr::IsModeratorAccount(sec))
    {
        WorldPacket data;
        MakeNotModerator(&data);
        SendToOne(&data, good);
    }
    else
    {
        Player* bad = sObjectAccessor->FindPlayerByNameInOrOutOfWorld(badname);
        if (bad == NULL || !IsBanned(bad->GetGUID()))
        {
            WorldPacket data;
            MakePlayerNotFound(&data, badname);
            SendToOne(&data, good);
        }
        else
        {
            banned.erase(bad->GetGUID());

            WorldPacket data;
            MakePlayerUnbanned(&data, bad->GetGUID(), good);
            SendToAll(&data);

            UpdateChannelInDB();
        }
    }
}

void Channel::Password(uint64 p, const char *pass)
{
    uint32 sec = 0;
    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);
    if (player)
        sec = player->GetSession()->GetSecurity();

    ChatHandler chat(player);

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (!IsOn(p))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, p);
    }
    else if (!m_Players[p].IsModerator() && !AccountMgr::IsModeratorAccount(sec))
    {
        WorldPacket data;
        MakeNotModerator(&data);
        SendToOne(&data, p);
    }
    else if (IsWorld())
    {
        WorldPacket data;
        MakeNotModerator(&data);
        SendToOne(&data, p);
    }
    else
    {
        m_password = pass;

        WorldPacket data;
        MakePasswordChanged(&data, p);
        SendToAll(&data);

        UpdateChannelInDB();
    }
}

void Channel::SetMode(uint64 p, const char *p2n, bool mod, bool set)
{
    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);
    if (!player)
        return;

    uint32 sec = player->GetSession()->GetSecurity();

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (!IsOn(p))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, p);
    }
    else if (!m_Players[p].IsModerator() && !AccountMgr::IsModeratorAccount(sec))
    {
        WorldPacket data;
        MakeNotModerator(&data);
        SendToOne(&data, p);
    }
    else
    {
        Player* newp = sObjectAccessor->FindPlayerByNameInOrOutOfWorld(p2n);
        if (!newp)
        {
            WorldPacket data;
            MakePlayerNotFound(&data, p2n);
            SendToOne(&data, p);
            return;
        }

        if (p == m_ownerGUID && newp->GetGUID() == m_ownerGUID && mod)
            return;

        if (!IsOn(newp->GetGUID()))
        {
            WorldPacket data;
            MakePlayerNotFound(&data, p2n);
            SendToOne(&data, p);
            return;
        }

        // allow make moderator from another team only if both is GMs
        // at this moment this only way to show channel post for GM from another team
        if ((!AccountMgr::IsModeratorAccount(player->GetSession()->GetSecurity()) || !AccountMgr::IsModeratorAccount(newp->GetSession()->GetSecurity())) &&
            player->GetTeam() != newp->GetTeam() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL))
        {
            WorldPacket data;
            MakePlayerNotFound(&data, p2n);
            SendToOne(&data, p);
            return;
        }

        if (m_ownerGUID == newp->GetGUID() && m_ownerGUID != p)
        {
            WorldPacket data;
            MakeNotOwner(&data);
            SendToOne(&data, p);
            return;
        }

        if (mod)
            SetModerator(newp->GetGUID(), set);
        else
            SetMute(newp->GetGUID(), set);
    }
}

void Channel::SetOwner(uint64 p, const char *newname)
{
    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);
    if (!player)
        return;

    uint32 sec = player->GetSession()->GetSecurity();

    if (!IsOn(p))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, p);
        return;
    }

    if (!AccountMgr::IsModeratorAccount(sec) && p != m_ownerGUID)
    {
        WorldPacket data;
        MakeNotOwner(&data);
        SendToOne(&data, p);
        return;
    }

    Player* newp = sObjectAccessor->FindPlayerByNameInOrOutOfWorld(newname);
    if (newp == NULL || !IsOn(newp->GetGUID()))
    {
        WorldPacket data;
        MakePlayerNotFound(&data, newname);
        SendToOne(&data, p);
        return;
    }

    if (newp->GetTeam() != player->GetTeam() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL))
    {
        WorldPacket data;
        MakePlayerNotFound(&data, newname);
        SendToOne(&data, p);
        return;
    }

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    m_Players[newp->GetGUID()].SetModerator(true);
    SetOwner(newp->GetGUID());
}

void Channel::SendWhoOwner(uint64 p)
{
    if (!IsOn(p))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, p);
    }
    else
    {
        WorldPacket data;
        MakeChannelOwner(&data);
        SendToOne(&data, p);
    }
}

void Channel::List(Player * p_Player)
{
    uint64 l_PlayerGuid = p_Player->GetGUID();

    if (!IsOn(l_PlayerGuid))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, l_PlayerGuid);
    }
    else
    {
        ByteBuffer l_Buffer;
        uint32 l_MemberCount = 0;
        uint32 l_GmLevelInWhoList = sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST);

        m_PlayersLock.acquire();
        for (PlayerList::const_iterator l_I = m_Players.begin(); l_I != m_Players.end(); ++l_I)
        {
            Player* l_Member = ObjectAccessor::FindPlayerInOrOutOfWorld(l_I->first);

            /// PLAYER can't see MODERATOR, GAME MASTER, ADMINISTRATOR characters
            /// MODERATOR, GAME MASTER, ADMINISTRATOR can see all
            if (l_Member && (!AccountMgr::IsPlayerAccount(p_Player->GetSession()->GetSecurity()) || l_Member->GetSession()->GetSecurity() <= AccountTypes(l_GmLevelInWhoList)) &&
                l_Member->IsVisibleGloballyFor(p_Player))
            {
                l_Buffer.appendPackGUID(l_I->first);                ///< Member guid
                l_Buffer << uint32(g_RealmID);                      ///< virtualRealmAddress
                l_Buffer << uint8(l_I->second.flags);               ///< Flags

                ++l_MemberCount;
            }
        }
        m_PlayersLock.release();

        WorldPacket l_Data(SMSG_CHANNEL_LIST, 1 + 1 + 4 + GetName().length() + (l_MemberCount * (8 + 4 + 1)));
        l_Data.WriteBit(1);                                     ///< Display
        l_Data.WriteBits(GetName().length(), 7);                ///< Channel name length
        l_Data.FlushBits();

        l_Data << uint32(GetFlags());                           ///< Channel flags
        l_Data << uint32(l_MemberCount);                        ///< Member count
        l_Data.WriteString(GetName());                          ///< Channel name

        l_Data.append(l_Buffer);

        SendToOne(&l_Data, l_PlayerGuid);
    }
}

void Channel::Announce(uint64 p)
{
    uint32 sec = 0;
    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);
    if (player)
        sec = player->GetSession()->GetSecurity();

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (!IsOn(p))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, p);
    }
    else if (!m_Players[p].IsModerator() && !AccountMgr::IsModeratorAccount(sec))
    {
        WorldPacket data;
        MakeNotModerator(&data);
        SendToOne(&data, p);
    }
    else
    {
        m_announce = !m_announce;

        WorldPacket data;
        if (m_announce)
            MakeAnnouncementsOn(&data, p);
        else
            MakeAnnouncementsOff(&data, p);
        SendToAll(&data);

        UpdateChannelInDB();
    }
}

void Channel::Say(uint64 p, const char *what, uint32 lang)
{
    if (!what)
        return;
    if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL))
        lang = LANG_UNIVERSAL;

    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);
    if (!player)
        return;

    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (IsWorld() && sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE))
    {
        if (m_PlayersHistory.find(p) == m_PlayersHistory.end())
        {
            m_PlayersHistory[p].Warnings    = 0;
            m_PlayersHistory[p].Muted       = false;
        }

        ChatHistory* l_PlayerHistory = &m_PlayersHistory[p];

        if (l_PlayerHistory)
        {
            time_t l_Time = sWorld->GetGameTime();

            if (l_PlayerHistory->Muted)
            {
                time_t l_MuteDiff = m_MuteTimes[std::min(l_PlayerHistory->Warnings, static_cast<uint32>(m_MuteTimes.size() - 1))] - (l_Time - l_PlayerHistory->MuteStart);
                if (l_MuteDiff > 0)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(TrinityStrings::MessageNotSendBecauseMute, l_MuteDiff);
                    return;
                }

                l_PlayerHistory->LastWarningTime = l_Time + l_MuteDiff;
                l_PlayerHistory->Muted = false;
            }

            if (l_PlayerHistory->Warnings && (l_PlayerHistory->LastWarningTime - l_Time > 10 * TimeConstants::MINUTE))
                l_PlayerHistory->Warnings = 0;

            if (!player->isGameMaster())
            {
                std::string badWord = sWordFilterMgr->FindBadWord(what);
                if (!badWord.empty())
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_WORD_FILTER_FOUND_BAD_WORD_IN_CHAT);
                    ChatHandler(player->GetSession()).SendGlobalFormatedGMSysMessage("System: Player %s (GUID %u) has been given a warning for using badword in %s", player->GetName(), player->GetGUIDLow(), what);
                    time_t l_NextMuteTime = m_MuteTimes[std::min(l_PlayerHistory->Warnings + 1, static_cast<uint32>(m_MuteTimes.size() - 1))];
                    l_PlayerHistory->Warnings++;
                    l_PlayerHistory->Muted = true;
                    l_PlayerHistory->MuteStart = l_Time;
                    if (l_NextMuteTime)
                        ChatHandler(player->GetSession()).SendGlobalFormatedGMSysMessage("System: Player %s (GUID %u) has been given a %u seconds mute for badword in world", player->GetName(), player->GetGUIDLow(), l_NextMuteTime);
                    return;
                }

                std::vector<std::regex> const& l_ForbiddenRegex = sObjectMgr->GetForbiddenRegex(eChannelType::ChannelWorld);
                for (std::regex l_Regex : l_ForbiddenRegex)
                {
                    if (std::regex_search(what, l_Regex))
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage(LANG_WORD_FILTER_FOUND_BAD_WORD_IN_CHAT);
                        ChatHandler(player->GetSession()).SendGlobalFormatedGMSysMessage("System: Player %s (GUID %u) has been given a warning for using badword in %s", player->GetName(), player->GetGUIDLow(), what);
                        time_t l_NextMuteTime = m_MuteTimes[std::min(l_PlayerHistory->Warnings + 1, static_cast<uint32>(m_MuteTimes.size() - 1))];
                        l_PlayerHistory->Warnings++;
                        l_PlayerHistory->Muted = true;
                        l_PlayerHistory->MuteStart = l_Time;
                        if (l_NextMuteTime)
                            ChatHandler(player->GetSession()).SendGlobalFormatedGMSysMessage("System: Player %s (GUID %u) has been given a %u seconds mute for badword in world", player->GetName(), player->GetGUIDLow(), l_NextMuteTime);
                        return;
                    }
                }
            }

            if (!player->GetSession()->GetSecurity())
            {
                std::array<std::pair<uint32, uint32>, 3> l_MessagesOnTime =
                {
                    {
                        {10, 0},
                        {30, 0},
                        {60, 0}
                    }
                };

                for (auto l_Itr = l_PlayerHistory->MessageTimes.begin(); l_Itr != l_PlayerHistory->MessageTimes.end();)
                {
                    time_t l_Diff = l_Time - *l_Itr;
                    if (l_Diff > 60)
                    {
                        l_PlayerHistory->MessageTimes.pop_front();
                        l_Itr = l_PlayerHistory->MessageTimes.begin();
                        continue;
                    }

                    for (std::pair<uint32, uint32>& l_Pair : l_MessagesOnTime)
                    {
                        if (l_Diff < l_Pair.first)
                            ++l_Pair.second;
                    }

                    ++l_Itr;
                }

                for (uint32 l_I = 0; l_I < l_MessagesOnTime.size(); ++l_I)
                {
                    if (l_MessagesOnTime[l_I].second + 1 > 3 * (l_I + 1))
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage(TrinityStrings::MessageNotSendBecauseFlood);
                        time_t l_NextMuteTime = m_MuteTimes[std::min(l_PlayerHistory->Warnings + 1, static_cast<uint32>(m_MuteTimes.size() - 1))];
                        l_PlayerHistory->Warnings++;
                        l_PlayerHistory->Muted = true;
                        l_PlayerHistory->MuteStart = l_Time;
                        if (l_NextMuteTime)
                        {
                            ChatHandler(player->GetSession()).PSendSysMessage(TrinityStrings::MutedForXSeconds, l_NextMuteTime);
                            ChatHandler(player->GetSession()).SendGlobalFormatedGMSysMessage("System: Player %s (GUID %u) has been given a %u seconds mute in world chat for flood", player->GetName(), player->GetGUIDLow(), l_NextMuteTime);
                        }

                        time_t l_FollowingMuteTime = m_MuteTimes[std::min(l_PlayerHistory->Warnings + 1, static_cast<uint32>(m_MuteTimes.size() - 1))];
                        ChatHandler(player->GetSession()).PSendSysMessage(TrinityStrings::NextWarningIsAMute, l_FollowingMuteTime);
                        return;
                    }
                }

                l_PlayerHistory->MessageTimes.push_back(l_Time);
            }
        }
    }

    if (!IsOn(p))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, p);
    }
    else if (m_Players[p].IsMuted())
    {
        WorldPacket data;
        MakeMuted(&data);
        SendToOne(&data, p);
    }
    else
    {
        uint32 messageLength = strlen(what) + 1; ///< messageLenght is never read 01/18/16

        WorldSession* l_Session = player->GetSession();
        if (l_Session && l_Session->GetSecurity() == AccountTypes::SEC_PLAYER)
        {
            if (IsLockedDown())
            {
                ChatHandler(player).PSendSysMessage("Message failed: \'%s\' is currently under lock down.", GetName().c_str());
                return;
            }

            if (!CheckPlayerSlowMode(p))
                return;
        }

        WorldPacket data;
        player->BuildPlayerChat(&data, 0, CHAT_MSG_CHANNEL, what, lang, NULL, m_name);
        SendToAll(&data, !m_Players[p].IsModerator() ? p : false);

        if (IsWorld())
        {
            std::regex l_WorldFilter("(?:need|besoin|recherche).*(?:tank|dps|heal)|guild|recrute|m[eé]tier|craft|pas cher|prix|n[eé]gociable|vend|ach[eé]te|seek|ilevel|go tag", std::regex_constants::icase);

            if (!std::regex_search(what, l_WorldFilter))
            {
                std::string l_Msg = what;
                std::smatch l_WoWLinkInfo;
                std::regex  l_WoWLinkFilter("\\|cff([a-z0-9]+)\\|H([a-z]+):([a-zA-Z0-9-]*)(?::([^:\\|]+))?[^\\|]*\\|h(\\[[^\\|]+\\]+)\\|h\\|r");

                /// http://wowprogramming.com/docs/api_types section: hyperlink

                while (std::regex_search(l_Msg, l_WoWLinkInfo, l_WoWLinkFilter))
                {
                    //std::string l_ColorCode = l_WoWLinkInfo[1];
                    std::string l_Type      = l_WoWLinkInfo[2];
                    std::string l_Id        = l_WoWLinkInfo[3];
                    std::string l_Name      = l_WoWLinkInfo[5];

                    if (l_Type == "enchant")
                        l_Type = "spell";

                    if (l_Type == "talent")
                    {
                        /// *Doing a hand gesture* you saw nothing.
                        /// Use http://www.wowhead.com/search?q=TALENT_NAME#talents because the id it's not the talent spell id
                        l_Type = "search?q";
                        l_Id = l_Name + "#talents";
                    }

                    if (l_Type == "trade")
                    {
                        l_Type = "spell";
                        l_Id   = l_WoWLinkInfo[4];
                    }

                    std::ostringstream l_NewMsg;
                    l_NewMsg << l_WoWLinkInfo.prefix()
                             << "<http://www.wowhead.com/" << l_Type << "=" << l_Id << "|" << l_Name << ">"
                             << l_WoWLinkInfo.suffix();

                    l_Msg = l_NewMsg.str();
                }
            }
        }
    }
}

void Channel::Invite(uint64 p, const char *newname)
{
    if (!IsOn(p))
    {
        WorldPacket data;
        MakeNotMember(&data);
        SendToOne(&data, p);
        return;
    }

    Player* newp = sObjectAccessor->FindPlayerByNameInOrOutOfWorld(newname);
    if (!newp || !newp->isGMVisible())
    {
        WorldPacket data;
        MakePlayerNotFound(&data, newname);
        SendToOne(&data, p);
        return;
    }

    if (IsBanned(newp->GetGUID()))
    {
        WorldPacket data;
        MakePlayerInviteBanned(&data, newname);
        SendToOne(&data, p);
        return;
    }

    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(p);
    if (!player)
        return;

    if (newp->GetTeam() != player->GetTeam() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL))
    {
        WorldPacket data;
        MakeInviteWrongFaction(&data);
        SendToOne(&data, p);
        return;
    }

    if (IsOn(newp->GetGUID()))
    {
        WorldPacket data;
        MakePlayerAlreadyMember(&data, newp->GetGUID());
        SendToOne(&data, p);
        return;
    }

    WorldPacket data;
#ifndef CROSS
    if (!newp->GetSocial()->HasIgnore(GUID_LOPART(p)))
#else /* CROSS */
    if (!newp->GetSocial() || !newp->GetSocial()->HasIgnore(GUID_LOPART(p)))
#endif /* CROSS */
    {
        MakeInvite(&data, p);
        SendToOne(&data, newp->GetGUID());
        data.clear();
    }
    MakePlayerInvited(&data, newp->GetName());
    SendToOne(&data, p);
}

void Channel::SetOwner(uint64 guid, bool exclaim)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    if (m_ownerGUID)
    {
        // [] will re-add player after it possible removed
        PlayerList::iterator p_itr = m_Players.find(m_ownerGUID);
        if (p_itr != m_Players.end())
            p_itr->second.SetOwner(false);
    }

    m_ownerGUID = guid;
    if (m_ownerGUID)
    {
        uint8 oldFlag = GetPlayerFlags(m_ownerGUID);
        m_Players[m_ownerGUID].SetModerator(true);
        m_Players[m_ownerGUID].SetOwner(true);

        WorldPacket data;
        MakeModeChange(&data, m_ownerGUID, oldFlag);
        SendToAll(&data);

        if (exclaim)
        {
            MakeOwnerChanged(&data, m_ownerGUID);
            SendToAll(&data);
        }

        UpdateChannelInDB();
    }
}

void Channel::SendToAll(WorldPacket* data, uint64 p, uint64 p_SenderGUID)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    for (PlayerList::const_iterator i = m_Players.begin(); i != m_Players.end(); ++i)
    {
        Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(i->first);
        if (player)
        {
#ifndef CROSS
            if (!p || !player->GetSocial()->HasIgnore(GUID_LOPART(p)))
            {
                if (IsWorld() || IsConstant())
                    player->GetSession()->SendPacket(data);
                else if (!(IsWorld() || IsConstant()))
                    player->GetSession()->SendPacket(data);
            }
#else /* CROSS */
            if (!p || !player->GetSocial() || !player->GetSocial()->HasIgnore(GUID_LOPART(p)))
                player->GetSession()->SendPacket(data);
#endif /* CROSS */
        }
    }
}

void Channel::SendToAllButOne(WorldPacket* data, uint64 who)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> l_Guard(m_PlayersLock);

    for (PlayerList::const_iterator i = m_Players.begin(); i != m_Players.end(); ++i)
    {
        if (i->first != who)
        {
            Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(i->first);
            if (player)
                player->GetSession()->SendPacket(data);
        }
    }
}

void Channel::SendToOne(WorldPacket* data, uint64 who)
{
    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(who);
    if (player)
        player->GetSession()->SendPacket(data);
}

// done
void Channel::MakeNotifyPacket(WorldPacket* data, uint8 notify_type, uint64 p_SenderGUID, uint64 p_TargetGUID, std::string p_SenderName, uint8 p_OldFlags, uint8 p_NewFlags)
{
    Player * l_SenderPlayer = sObjectAccessor->FindPlayerInOrOutOfWorld(p_SenderGUID);

    data->Initialize(SMSG_CHANNEL_NOTIFY, 3 + 16 + 2 + 16 + 2 + 4 + 16 + 2 + 4 + 4 + 4 + 4 + m_name.size() + p_SenderName.size());
    data->WriteBits(notify_type, 6);
    data->WriteBits(m_name.length(), 7);
    data->WriteBits(p_SenderName.length(), 6);

    data->FlushBits();

    data->appendPackGUID(p_SenderGUID);
    data->appendPackGUID(l_SenderPlayer ? l_SenderPlayer->GetSession()->GetWoWAccountGUID() : 0);
    *data << uint32(p_SenderGUID ? g_RealmID : 0);
    data->appendPackGUID(p_TargetGUID);
    *data << uint32(p_TargetGUID ? g_RealmID : 0);
    *data << int32(m_channelId);

    if (notify_type == CHAT_MODE_CHANGE_NOTICE)
    {
        *data << uint8(p_OldFlags);
        *data << uint8(p_NewFlags);
    }

    data->WriteString(m_name);
    data->WriteString(p_SenderName);
}

// done 0x00
void Channel::MakeJoined(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_JOINED_NOTICE, guid, 0, "");
}

// done 0x01
void Channel::MakeLeft(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_LEFT_NOTICE, guid, 0, "");
}

// done 0x02
void Channel::MakeYouJoined(WorldPacket* p_Data)
{
    std::string l_ChannelWelcomeMsg = "";

    p_Data->Initialize(SMSG_CHANNEL_NOTIFY_JOINED, 3 + 1 + 4 + 8 + m_name.size() + l_ChannelWelcomeMsg.size());
    p_Data->WriteBits(GetName().length(), 7);   ///< Channel Name
    p_Data->WriteBits(l_ChannelWelcomeMsg.length(), 10);///< ChannelWelcomeMsg

    p_Data->FlushBits();

    *p_Data << uint32(GetFlags());              ///< Channel Flags
    *p_Data << uint32(GetChannelId());          ///< Channel ID
    *p_Data << uint64(0);                       ///< InstanceID
    p_Data->WriteString(GetName());             ///< Channel Name
    p_Data->WriteString(l_ChannelWelcomeMsg);   ///< ChannelWelcomeMsg
}

// done 0x03
void Channel::MakeYouLeft(WorldPacket* p_Data)
{
    p_Data->Initialize(SMSG_CHANNEL_NOTIFY_LEFT, 1 + m_name.size() + 4);
    p_Data->WriteBits(GetName().length(), 7);   ///< Channel Name
    p_Data->WriteBit(0);                        ///< Is suspended
    p_Data->FlushBits();
    *p_Data << uint32(GetChannelId());          ///< Channel ID
    p_Data->WriteString(GetName());             ///< Channel Name
}

// done 0x04
void Channel::MakeWrongPassword(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_WRONG_PASSWORD_NOTICE, 0, 0, "");
}

// done 0x05
void Channel::MakeNotMember(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_NOT_MEMBER_NOTICE, 0, 0, "");
}

// done 0x06
void Channel::MakeNotModerator(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_NOT_MODERATOR_NOTICE, 0, 0, "");
}

// done 0x07
void Channel::MakePasswordChanged(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_PASSWORD_CHANGED_NOTICE, guid, 0, "");
}

// done 0x08
void Channel::MakeOwnerChanged(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_OWNER_CHANGED_NOTICE, guid, 0, "");
}

// done 0x09
void Channel::MakePlayerNotFound(WorldPacket* data, const std::string& name)
{
    MakeNotifyPacket(data, CHAT_PLAYER_NOT_FOUND_NOTICE, 0, 0, name);
}

// done 0x0A
void Channel::MakeNotOwner(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_NOT_OWNER_NOTICE, 0, 0, "");
}

// done 0x0B
void Channel::MakeChannelOwner(WorldPacket* data)
{
    std::string name = "";

    if (!sObjectMgr->GetPlayerNameByGUID(m_ownerGUID, name) || name.empty())
        name = "PLAYER_NOT_FOUND";

    MakeNotifyPacket(data, CHAT_CHANNEL_OWNER_NOTICE, 0, 0, ((IsConstant() || !m_ownerGUID) ? "Nobody" : name));
}

// done 0x0C
void Channel::MakeModeChange(WorldPacket* data, uint64 guid, uint8 oldflags)
{
    MakeNotifyPacket(data, CHAT_MODE_CHANGE_NOTICE, guid, 0, "", oldflags, GetPlayerFlags(guid));
}

// done 0x0D
void Channel::MakeAnnouncementsOn(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_ANNOUNCEMENTS_ON_NOTICE, guid, 0, "");
}

// done 0x0E
void Channel::MakeAnnouncementsOff(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_ANNOUNCEMENTS_OFF_NOTICE, guid, 0, "");
}

// done 0x11
void Channel::MakeMuted(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_MUTED_NOTICE, 0, 0, "");
}

// done 0x12
void Channel::MakePlayerKicked(WorldPacket* data, uint64 bad, uint64 good)
{
    MakeNotifyPacket(data, CHAT_PLAYER_KICKED_NOTICE, bad, good, "");
}

// done 0x13
void Channel::MakeBanned(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_BANNED_NOTICE, 0, 0, "");
}

// done 0x14
void Channel::MakePlayerBanned(WorldPacket* data, uint64 bad, uint64 good)
{
    MakeNotifyPacket(data, CHAT_PLAYER_BANNED_NOTICE, bad, good, "");
}

// done 0x15
void Channel::MakePlayerUnbanned(WorldPacket* data, uint64 bad, uint64 good)
{
    MakeNotifyPacket(data, CHAT_PLAYER_UNBANNED_NOTICE, bad, good, "");
}

// done 0x16
void Channel::MakePlayerNotBanned(WorldPacket* data, const std::string &name)
{
    MakeNotifyPacket(data, CHAT_PLAYER_NOT_BANNED_NOTICE, 0, 0, name);
}

// done 0x17
void Channel::MakePlayerAlreadyMember(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_PLAYER_ALREADY_MEMBER_NOTICE, guid, 0, "");
}

// done 0x18
void Channel::MakeInvite(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_INVITE_NOTICE, guid, 0, "");
}

// done 0x19
void Channel::MakeInviteWrongFaction(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_INVITE_WRONG_FACTION_NOTICE, 0, 0, "");
}

// done 0x1A
void Channel::MakeWrongFaction(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_WRONG_FACTION_NOTICE, 0, 0, "");
}

// done 0x1B
void Channel::MakeInvalidName(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_INVALID_NAME_NOTICE, 0, 0, "");
}

// done 0x1C
void Channel::MakeNotModerated(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_NOT_MODERATED_NOTICE, 0, 0, "");
}

// done 0x1D
void Channel::MakePlayerInvited(WorldPacket* data, const std::string& name)
{
    MakeNotifyPacket(data, CHAT_PLAYER_INVITED_NOTICE, 0, 0, name);
}

// done 0x1E
void Channel::MakePlayerInviteBanned(WorldPacket* data, const std::string& name)
{
    MakeNotifyPacket(data, CHAT_PLAYER_INVITE_BANNED_NOTICE, 0, 0, name);
}

// done 0x1F
void Channel::MakeThrottled(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_THROTTLED_NOTICE, 0, 0, "");
}

// done 0x20
void Channel::MakeNotInArea(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_NOT_IN_AREA_NOTICE, 0, 0, "");
}

// done 0x21
void Channel::MakeNotInLfg(WorldPacket* data)
{
    MakeNotifyPacket(data, CHAT_NOT_IN_LFG_NOTICE, 0, 0, "");
}

// done 0x22
void Channel::MakeVoiceOn(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_VOICE_ON_NOTICE, guid, 0 , "");
}

// done 0x23
void Channel::MakeVoiceOff(WorldPacket* data, uint64 guid)
{
    MakeNotifyPacket(data, CHAT_VOICE_OFF_NOTICE, guid, 0, "");
}

void Channel::JoinNotify(uint64 guid)
{
    WorldPacket data(SMSG_USERLIST_ADD);
    data.appendPackGUID(guid);
    data << uint8(m_flags);
    data << uint8(GetPlayerFlags(guid));
    data << uint32(m_channelId);

    data.WriteBits(GetName().size(), 7);
    data.FlushBits();

    data.WriteString(GetName());

    SendToAllButOne(&data, guid);
}

void Channel::LeaveNotify(uint64 guid)
{
    WorldPacket data(SMSG_USERLIST_REMOVE);

    data.appendPackGUID(guid);
    data << uint32(m_flags);
    data << uint32(m_channelId);

    data.WriteBits(GetName().size(), 7);
    data.FlushBits();

    data.WriteString(GetName());

    if (IsConstant())
        SendToAllButOne(&data, guid);
    else
        SendToAll(&data);
}
