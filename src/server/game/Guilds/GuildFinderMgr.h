////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CROSS
#ifndef __TRINITY_GUILDFINDER_H
#define __TRINITY_GUILDFINDER_H

#include "Common.h"
#include "World.h"
#include "GuildMgr.h"

enum GuildFinderOptionsInterest
{
    INTEREST_QUESTING        = 0x01,
    INTEREST_DUNGEONS        = 0x02,
    INTEREST_RAIDS           = 0x04,
    INTEREST_PVP             = 0x08,
    INTEREST_ROLE_PLAYING    = 0x10,
    ALL_INTERESTS            = INTEREST_QUESTING | INTEREST_DUNGEONS | INTEREST_RAIDS | INTEREST_PVP | INTEREST_ROLE_PLAYING
};

enum GuildFinderOptionsAvailability
{
    AVAILABILITY_WEEKDAYS     = 0x1,
    AVAILABILITY_WEEKENDS     = 0x2,
    ALL_WEEK                  = AVAILABILITY_WEEKDAYS | AVAILABILITY_WEEKENDS
};

enum GuildFinderOptionsRoles
{
    GUILDFINDER_ROLE_TANK        = 0x1,
    GUILDFINDER_ROLE_HEALER      = 0x2,
    GUILDFINDER_ROLE_DAMAGE         = 0x4,
    GUILDFINDER_ALL_ROLES        = GUILDFINDER_ROLE_TANK | GUILDFINDER_ROLE_HEALER | GUILDFINDER_ROLE_DAMAGE
};

enum GuildFinderOptionsLevel
{
    ANY_FINDER_LEVEL   = 0x1,
    MAX_FINDER_LEVEL   = 0x2,
    ALL_GUILDFINDER_LEVELS = ANY_FINDER_LEVEL | MAX_FINDER_LEVEL
};

/// Holds all required informations about a membership request
struct MembershipRequest
{
    public:
        MembershipRequest(MembershipRequest const& settings) : _comment(settings.GetComment())
        {
            _availability = settings.GetAvailability();
            _classRoles = settings.GetClassRoles();
            _interests = settings.GetInterests();
            _guildId = settings.GetGuildId();
            _playerGUID = settings.GetPlayerGUID();
            _time = settings.GetSubmitTime();
        }

        MembershipRequest(uint32 playerGUID, uint32 guildId, uint32 availability, uint32 classRoles, uint32 interests, std::string& comment, time_t submitTime) :
             _comment(comment), _guildId(guildId), _playerGUID(playerGUID), _availability(availability), _classRoles(classRoles),
             _interests(interests), _time(submitTime) {}

        MembershipRequest() : _guildId(0), _playerGUID(0), _availability(0), _classRoles(0),
            _interests(0), _time(time(NULL)) {}

        uint32 GetGuildId() const      { return _guildId; }
        uint32 GetPlayerGUID() const   { return _playerGUID; }
        uint8 GetAvailability() const  { return _availability; }
        uint8 GetClassRoles() const    { return _classRoles; }
        uint8 GetInterests() const     { return _interests; }

        uint8 GetClass() const
        {
            const CharacterInfo* l_NameData = sWorld->GetCharacterInfo(GetPlayerGUID());
            return l_NameData ? l_NameData->Class : 0;
        }

        uint8 GetLevel() const
        {
            const CharacterInfo* l_NameData = sWorld->GetCharacterInfo(GetPlayerGUID());
            return l_NameData ? l_NameData->Level : 1;
        }

        time_t GetSubmitTime() const   { return _time; }
        time_t GetExpiryTime() const   { return time_t(_time + 30 * 24 * 3600); } // Adding 30 days
        std::string const& GetComment() const { return _comment; }
        std::string const& GetName() const
        {
            const CharacterInfo  *l_NameData = sWorld->GetCharacterInfo(GetPlayerGUID());
            static const std::string name = "";
            return l_NameData ? l_NameData->Name : name;
        }

    private:
        std::string _comment;

        uint32 _guildId;
        uint32 _playerGUID;

        uint8 _availability;
        uint8 _classRoles;
        uint8 _interests;

        time_t _time;
};

using MembershipRequestPtr = std::shared_ptr<MembershipRequest>;

/// Holds all informations about a player's finder settings. _NOT_ stored in database.
struct LFGuildPlayer
{
    public:
        LFGuildPlayer()
        {
            _guid = 0;
            _roles = 0;
            _availability = 0;
            _interests = 0;
            _level = 0;
        }

        LFGuildPlayer(uint32 guid, uint8 role, uint8 availability, uint8 interests, uint8 level)
        {
            _guid = guid;
            _roles = role;
            _availability = availability;
            _interests = interests;
            _level = level;
        }

        LFGuildPlayer(uint32 guid, uint8 role, uint8 availability, uint8 interests, uint8 level, std::string& comment) : _comment(comment)
        {
            _guid = guid;
            _roles = role;
            _availability = availability;
            _interests = interests;
            _level = level;
        }

        LFGuildPlayer(LFGuildPlayer const& settings) : _comment(settings.GetComment())
        {
            _guid = settings.GetGUID();
            _roles = settings.GetClassRoles();
            _availability = settings.GetAvailability();
            _interests = settings.GetInterests();
            _level = settings.GetLevel();
        }

        uint32 GetGUID() const         { return _guid; }
        uint8 GetClassRoles() const    { return _roles; }
        uint8 GetAvailability() const  { return _availability; }
        uint8 GetInterests() const     { return _interests; }
        uint8 GetLevel() const         { return _level; }
        std::string const& GetComment() const { return _comment; }

    private:
        std::string _comment;
        uint32 _guid;
        uint8 _roles;
        uint8 _availability;
        uint8 _interests;
        uint8 _level;
};

/// Holds settings for a guild in the finder system. Saved to database.
struct LFGuildSettings : public LFGuildPlayer
{
    public:
        LFGuildSettings() : LFGuildPlayer(), _team(TEAM_ALLIANCE), _listed(false) {}

        LFGuildSettings(bool listed, TeamId team) : LFGuildPlayer(), _team(team), _listed(listed) {}

        LFGuildSettings(bool listed, TeamId team, uint32 guid, uint8 role, uint8 availability, uint8 interests, uint8 level) :
            LFGuildPlayer(guid, role, availability, interests, level), _team(team), _listed(listed) {}

        LFGuildSettings(bool listed, TeamId team, uint32 guid, uint8 role, uint8 availability, uint8 interests, uint8 level, std::string& comment) :
            LFGuildPlayer(guid, role, availability, interests, level, comment), _team(team), _listed(listed) {}

        LFGuildSettings(LFGuildSettings const& settings) : LFGuildPlayer(settings), _team(settings.GetTeam()), _listed(settings.IsListed()) {}


        bool IsListed() const      { return _listed; }
        void SetListed(bool state) { _listed = state; }

        TeamId GetTeam() const     { return _team; }
    private:
        TeamId _team;
        bool _listed;
};

using LFGuildSettingsPtr = std::shared_ptr<LFGuildSettings>;

typedef ACE_Based::LockedMap<uint32 /* guildGuid */, LFGuildSettingsPtr> LFGuildStore;
typedef ACE_Based::LockedMap<uint32 /* guildGuid */, ACE_Based::LockedVector<MembershipRequestPtr> > MembershipRequestStore;

class GuildFinderMgr
{
    friend class ACE_Singleton<GuildFinderMgr, ACE_Null_Mutex>;

    private:
        GuildFinderMgr();
        ~GuildFinderMgr();

        LFGuildStore  _guildSettings;

        MembershipRequestStore _membershipRequests;

        void LoadGuildSettings();
        void LoadMembershipRequests();

    public:
        void LoadFromDB();

        /**
         * @brief Stores guild settings and begins an asynchronous database insert
         * @param guildGuid The guild's database guid.
         * @param LFGuildSettings The guild's settings storage.
         */
        void SetGuildSettings(uint32 guildGuid, LFGuildSettingsPtr const settings);

        /**
         * @brief Returns settings for a guild.
         * @param guildGuid The guild's database guid.
         */
        LFGuildSettingsPtr GetGuildSettings(uint32 guildGuid)
        {
            return _guildSettings.find(guildGuid) != _guildSettings.end() ? _guildSettings[guildGuid] : std::make_shared<LFGuildSettings>();
        }

        /**
         * @brief Files a membership request to a guild
         * @param guildGuid The guild's database GUID.
         * @param MembershipRequest An object storing all data related to the request.
         */
        void AddMembershipRequest(uint32 guildGuid, MembershipRequestPtr const request);

        /**
         * @brief Removes all membership request from a player.
         * @param playerId The player's database guid whose application shall be deleted.
         */
        void RemoveAllMembershipRequestsFromPlayer(uint32 playerId);

        /**
         * @brief Removes a membership request to a guild.
         * @param playerId The player's database guid whose application shall be deleted.
         * @param guildId  The guild's database guid
         */
        void RemoveMembershipRequest(uint32 playerId, uint32 guildId);

        /// wipes everything related to a guild. Used when that guild is disbanded
        void DeleteGuild(uint32 guildId);

        /**
         * @brief Returns a set of membership requests for a guild
         * @param guildGuid The guild's database guid.
         */
        ACE_Based::LockedVector<MembershipRequestPtr> GetAllMembershipRequestsForGuild(uint32 guildGuid)
        {
            return _membershipRequests.find(guildGuid) != _membershipRequests.end() ?  _membershipRequests[guildGuid] : std::vector<MembershipRequestPtr>();
        }

        /**
         * @brief Returns a list of membership requests for a player.
         * @param playerGuid The player's database guid.
         */
        std::list<MembershipRequestPtr> GetAllMembershipRequestsForPlayer(uint32 playerGuid);

        /**
         * @brief Returns a store of guilds matching the settings provided, using bitmask operators.
         * @param settings The player's finder settings
         * @param teamId   The player's faction (TEAM_ALLIANCE or TEAM_HORDE)
         */
        LFGuildStore GetGuildsMatchingSetting(LFGuildPlayer& settings, TeamId faction);

        /// Provided a player DB guid and a guild DB guid, determines if a pending request is filed with these keys.
        bool HasRequest(uint32 playerId, uint32 guildId);

        /// Counts the amount of pending membership requests, given the player's db guid.
        uint8 CountRequestsFromPlayer(uint32 playerId);

        void SendApplicantListUpdate(Guild& guild);
        void SendMembershipRequestListUpdate(Player& player);
};

#define sGuildFinderMgr ACE_Singleton<GuildFinderMgr, ACE_Null_Mutex>::instance()

#endif // __TRINITY_GUILDFINDER_H
#endif
