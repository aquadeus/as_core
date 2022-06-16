////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CROSS
#include "ObjectMgr.h"
#include "GuildFinderMgr.h"
#include "GuildMgr.h"
#include "World.h"

GuildFinderMgr::GuildFinderMgr()
{
}

GuildFinderMgr::~GuildFinderMgr()
{
}

void GuildFinderMgr::LoadFromDB()
{
    LoadGuildSettings();
    LoadMembershipRequests();
}

void GuildFinderMgr::LoadGuildSettings()
{
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading guild finder guild-related settings...");
    //                                                           0                1             2                  3             4           5             6         7
    QueryResult result = CharacterDatabase.Query("SELECT gfgs.guildId, gfgs.availability, gfgs.classRoles, gfgs.interests, gfgs.level, gfgs.listed, gfgs.comment, c.race "
                                                 "FROM guild_finder_guild_settings gfgs "
                                                 "LEFT JOIN guild_member gm ON gm.guildid=gfgs.guildId "
                                                 "LEFT JOIN characters c ON c.guid = gm.guid LIMIT 1");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 guild finder guild-related settings. Table `guild_finder_guild_settings` is empty.");
        return;
    }

    uint32 count = 0;
    uint32 oldMSTime = getMSTime();
    do
    {
        Field* fields = result->Fetch();
        uint32 guildId      = fields[0].GetUInt32();
        uint8  availability = fields[1].GetUInt8();
        uint8  classRoles   = fields[2].GetUInt8();
        uint8  interests    = fields[3].GetUInt8();
        uint8  level        = fields[4].GetUInt8();
        bool   listed       = (fields[5].GetUInt8() != 0);
        std::string comment = fields[6].GetString();

        TeamId guildTeam = TEAM_ALLIANCE;
        if (ChrRacesEntry const* raceEntry = sChrRacesStore.LookupEntry(fields[7].GetUInt8()))
            if (raceEntry->BaseLanguage == 1)
                guildTeam = TEAM_HORDE;

        LFGuildSettingsPtr settings = std::make_shared<LFGuildSettings>(listed, guildTeam, guildId, classRoles, availability, interests, level, comment);
        _guildSettings[guildId] = settings;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u guild finder guild-related settings in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void GuildFinderMgr::LoadMembershipRequests()
{
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading guild finder membership requests...");
    //                                                      0         1           2            3           4         5         6
    QueryResult result = CharacterDatabase.Query("SELECT guildId, playerGuid, availability, classRole, interests, comment, submitTime "
                                                 "FROM guild_finder_applicant");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 guild finder membership requests. Table `guild_finder_applicant` is empty.");
        return;
    }

    uint32 count = 0;
    uint32 oldMSTime = getMSTime();
    do
    {
        Field* fields = result->Fetch();
        uint32 guildId      = fields[0].GetUInt32();
        uint32 playerId     = fields[1].GetUInt32();
        uint8  availability = fields[2].GetUInt8();
        uint8  classRoles   = fields[3].GetUInt8();
        uint8  interests    = fields[4].GetUInt8();
        std::string comment = fields[5].GetString();
        uint32 submitTime   = fields[6].GetUInt32();

        MembershipRequestPtr request = std::make_shared<MembershipRequest>(playerId, guildId, availability, classRoles, interests, comment, time_t(submitTime));

        _membershipRequests[guildId].push_back(request);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u guild finder membership requests in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void GuildFinderMgr::AddMembershipRequest(uint32 guildGuid, MembershipRequestPtr const request)
{
    _membershipRequests[guildGuid].push_back(request);

    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GUILD_FINDER_APPLICANT);
    stmt->setUInt32(0, request->GetGuildId());
    stmt->setUInt32(1, request->GetPlayerGUID());
    stmt->setUInt8(2, request->GetAvailability());
    stmt->setUInt8(3, request->GetClassRoles());
    stmt->setUInt8(4, request->GetInterests());
    stmt->setString(5, request->GetComment());
    stmt->setUInt32(6, request->GetSubmitTime());
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);

    // Notify the applicant his submittion has been added
    if (Player* player = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(request->GetPlayerGUID(), 0, HIGHGUID_PLAYER)))
        SendMembershipRequestListUpdate(*player);

    // Notify the guild master and officers the list changed
    if (Guild* guild = sGuildMgr->GetGuildById(guildGuid))
        SendApplicantListUpdate(*guild);
}

void GuildFinderMgr::RemoveAllMembershipRequestsFromPlayer(uint32 playerId)
{
    _membershipRequests.safe_foreach(true, [&](MembershipRequestStore::iterator& l_Itr) -> void
    {
        auto l_Itr2 = l_Itr->second.find_itr([&](MembershipRequestPtr l_Request) -> bool
        {
            if (l_Request->GetPlayerGUID() == playerId)
                return true;

            return false;
        });

        if (l_Itr2 == l_Itr->second.end())
            return;

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_FINDER_APPLICANT);
        stmt->setUInt32(0, (*l_Itr2)->GetGuildId());
        stmt->setUInt32(1, (*l_Itr2)->GetPlayerGUID());
        trans->Append(stmt);

        CharacterDatabase.CommitTransaction(trans);
        l_Itr->second.erase(*l_Itr2);

        // Notify the guild master and officers the list changed
        if (Guild* guild = sGuildMgr->GetGuildById(l_Itr->first))
            SendApplicantListUpdate(*guild);
    });
}

void GuildFinderMgr::RemoveMembershipRequest(uint32 playerId, uint32 guildId)
{
    auto l_Itr = _membershipRequests[guildId].find_itr([&](MembershipRequestPtr l_Request) -> bool
    {
        if (l_Request->GetPlayerGUID() == playerId)
            return true;

        return false;
    });

    if (l_Itr == _membershipRequests[guildId].end())
        return;

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_FINDER_APPLICANT);
    stmt->setUInt32(0, (*l_Itr)->GetGuildId());
    stmt->setUInt32(1, (*l_Itr)->GetPlayerGUID());
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    _membershipRequests[guildId].erase(l_Itr);

    // Notify the applicant his submittion has been removed
    if (Player* player = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(playerId, 0, HIGHGUID_PLAYER)))
        SendMembershipRequestListUpdate(*player);

    // Notify the guild master and officers the list changed
    if (Guild* guild = sGuildMgr->GetGuildById(guildId))
        SendApplicantListUpdate(*guild);
}

std::list<MembershipRequestPtr> GuildFinderMgr::GetAllMembershipRequestsForPlayer(uint32 playerGuid)
{
    std::list<MembershipRequestPtr> resultSet;

    _membershipRequests.safe_foreach(true, [&](MembershipRequestStore::iterator& p_Itr)
    {
        p_Itr->second.foreach_until([&](MembershipRequestPtr l_Request) -> bool
        {
            if (l_Request->GetPlayerGUID() == playerGuid)
            {
                resultSet.push_back(l_Request);
                return true;
            }

            return false;
        });
    });

    return resultSet;
}

uint8 GuildFinderMgr::CountRequestsFromPlayer(uint32 playerId)
{
    uint8 result = 0;

    _membershipRequests.safe_foreach(true, [&](MembershipRequestStore::iterator& p_Itr)
    {
        p_Itr->second.foreach_until([&](MembershipRequestPtr l_Request) -> bool
        {
            if (l_Request->GetPlayerGUID() != playerId)
                return false;

            result++;
            return true;
        });
    });

    return result;
}

LFGuildStore GuildFinderMgr::GetGuildsMatchingSetting(LFGuildPlayer& settings, TeamId faction)
{
    LFGuildStore resultSet;

    _guildSettings.safe_foreach(true, [&](LFGuildStore::iterator& p_Itr) -> void
    {
        LFGuildSettingsPtr const guildSettings = p_Itr->second;

        if (guildSettings->GetTeam() != faction)
            return;

        if (!(guildSettings->GetAvailability() & settings.GetAvailability()))
            return;

        if (!(guildSettings->GetClassRoles() & settings.GetClassRoles()))
            return;

        if (!(guildSettings->GetInterests() & settings.GetInterests()))
            return;

        if (!(guildSettings->GetLevel() & settings.GetLevel()))
            return;

        resultSet.insert(std::make_pair(p_Itr->first, guildSettings));
    });

    return resultSet;
}

bool GuildFinderMgr::HasRequest(uint32 playerId, uint32 guildId)
{
    return _membershipRequests[guildId].exist([&](MembershipRequestPtr l_Request) -> bool
    {
        if (l_Request->GetPlayerGUID() == playerId)
            return true;
        return false;
    });
}

void GuildFinderMgr::SetGuildSettings(uint32 guildGuid, LFGuildSettingsPtr const settings)
{
    _guildSettings[guildGuid] = settings;

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GUILD_FINDER_GUILD_SETTINGS);
    stmt->setUInt32(0, settings->GetGUID());
    stmt->setUInt8(1, settings->GetAvailability());
    stmt->setUInt8(2, settings->GetClassRoles());
    stmt->setUInt8(3, settings->GetInterests());
    stmt->setUInt8(4, settings->GetLevel());
    stmt->setUInt8(5, settings->IsListed());
    stmt->setString(6, settings->GetComment());
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void GuildFinderMgr::DeleteGuild(uint32 guildId)
{
    _membershipRequests[guildId].foreach([&](MembershipRequestPtr l_Request) -> void
    {
        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        uint32 applicant = l_Request->GetPlayerGUID();

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_FINDER_APPLICANT);
        stmt->setUInt32(0, l_Request->GetGuildId());
        stmt->setUInt32(1, applicant);
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_FINDER_GUILD_SETTINGS);
        stmt->setUInt32(0, l_Request->GetGuildId());
        trans->Append(stmt);

        CharacterDatabase.CommitTransaction(trans);

        // Notify the applicant his submition has been removed
        if (Player* player = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(applicant, 0, HIGHGUID_PLAYER)))
            SendMembershipRequestListUpdate(*player);
    });

    _membershipRequests.erase(guildId);
    _guildSettings.erase(guildId);

    // Notify the guild master the list changed (even if he's not a GM any more, not sure if needed)
    if (Guild* guild = sGuildMgr->GetGuildById(guildId))
        SendApplicantListUpdate(*guild);
}

void GuildFinderMgr::SendApplicantListUpdate(Guild & p_Guild)
{
    WorldPacket l_Data(SMSG_LF_GUILD_APPLICANT_LIST_CHANGED, 0);
    p_Guild.BroadcastPacketToRank(&l_Data, GR_OFFICER);
    p_Guild.BroadcastPacketToRank(&l_Data, GR_GUILDMASTER);
}

void GuildFinderMgr::SendMembershipRequestListUpdate(Player & p_Player)
{
    WorldPacket l_Data(SMSG_LF_GUILD_APPLICATIONS_LIST_CHANGED);
    p_Player.SendDirectMessage(&l_Data);
}
#endif